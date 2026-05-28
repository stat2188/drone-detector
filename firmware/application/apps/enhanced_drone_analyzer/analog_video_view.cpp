/*
 * Copyright (C) 2025 PortaPack Mayhem Edition Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "analog_video_view.hpp"

#include <cstdint>
#include <cstddef>
#include <new>

#include "baseband_api.hpp"
#include "portapack.hpp"
#include "portapack_shared_memory.hpp"
#include "audio.hpp"
#include "audio_alerts.hpp"
#include "spectrum_color_lut.hpp"
#include "lcd_ili9341.hpp"

using namespace portapack;

namespace drone_analyzer {

// ============================================================================
// VideoWidget implementation
// ============================================================================
// Memory:
//   Instance: ~3,360 bytes (video_buffer_[3328] + state ~32B)
//   Stack per render_frame(): ~520 bytes (line_buffer on stack)
//     OK: called at frame sync rate, total stack < 4KB per AGENTS.md

VideoWidget::VideoWidget()
    : video_buffer_{}
    , frame_count_{0}
    , active_{false}
    , x_correction_{DEFAULT_X_CORRECTION} {
    set_focusable(true);
}

void VideoWidget::on_show() {
    active_ = true;
    frame_count_ = 0;
}

void VideoWidget::on_hide() {
    active_ = false;
}

void VideoWidget::paint(Painter& painter) {
    (void)painter;
    // Video rendering is done directly via display.render_line() in render_frame()
    // Paint is a no-op — the View's paint() handles the header
}

void VideoWidget::focus() {
    // Focus lands here so FocusManager has a target; parent handles all events.
}

void VideoWidget::x_correction_increment(int8_t delta) noexcept {
    int16_t val = static_cast<int16_t>(x_correction_) + delta;
    if (val < 0) val = 0;
    if (val > MAX_X_CORRECTION) val = MAX_X_CORRECTION;
    x_correction_ = static_cast<uint8_t>(val);
}

void VideoWidget::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    // Stack: ~0 bytes (no local allocations)
    if (!active_) {
        return;
    }

    // 13 frames × 256 bytes = 3328, always inside VIDEO_BUFFER_SIZE (3328)
    const size_t offset = frame_count_ * 256;
    for (size_t i = 0; i < 256; ++i) {
        // Invert: strong signal (0) -> white (255), noise (255) -> black (0)
        video_buffer_[offset + i] = 255 - spectrum.db[i];
    }

    ++frame_count_;

    // When 13 spectra accumulated (26 video lines), render the frame (line-doubled to 52)
    if (frame_count_ >= ACCUMULATED_FRAMES) {
        render_frame();
        frame_count_ = 0;
    }
}

void VideoWidget::render_frame() noexcept {
    // Stack: ~272 bytes (line_buffer 256B + 16B locals)
    // Called at frame sync rate, well within 4KB stack limit
    ui::Color line_buffer[LINE_WIDTH];

    // Line-doubling: each native video line is rendered twice (two consecutive scanlines)
    // 26 native lines × 2 = 52 display lines, starting at Y=32 (after 16px header)
    constexpr ui::Coord VIDEO_START_Y = 32;

    const uint8_t xcorr = x_correction_;

    for (uint16_t line = 0; line < VIDEO_LINES_HALF; ++line) {
        const size_t buf_offset = line * LINE_WIDTH;

        for (uint16_t px = 0; px < LINE_WIDTH; ++px) {
            const uint8_t src_idx = static_cast<uint8_t>((px + xcorr) & (LINE_WIDTH - 1));
            line_buffer[px] = spectrum_rgb4_lut[video_buffer_[buf_offset + src_idx]];
        }

        const ui::Coord y = static_cast<ui::Coord>(VIDEO_START_Y + line * 2);
        portapack::display.render_line(
            {VIDEO_START_X, y},
            LINE_WIDTH, line_buffer);
        portapack::display.render_line(
            {VIDEO_START_X, static_cast<ui::Coord>(y + 1)},
            LINE_WIDTH, line_buffer);
    }
}

// ============================================================================
// AnalogVideoView implementation
// ============================================================================
// Memory:
//   Instance: ~3,828 bytes (VideoWidget ~3.3KB + spectrum_buffer_ ~272B + handler_storage ~128B + state ~100B)
//   Stack per paint(): ~16 bytes (freq_str[16] + locals)
//   Stack per frame_sync handler: 0 bytes (spectrum_buffer_ is class member)
//   Flash: ~768 bytes (code)

[[nodiscard]] bool AnalogVideoView::is_frequency_valid(FreqHz freq) noexcept {
    return (freq >= MIN_FREQUENCY_HZ) && (freq <= MAX_FREQUENCY_HZ);
}

AnalogVideoView::AnalogVideoView(NavigationView& nav, FreqHz frequency) noexcept
    : nav_(nav)
    , frequency_(frequency)
    , receiver_active_(false)
    , handlers_active_(false)
    , spectrum_fifo_(nullptr) {

    add_child(&video_widget_);
    video_widget_.set_parent_rect({0, HEADER_H, 240, 320 - HEADER_H});
}

AnalogVideoView::~AnalogVideoView() noexcept {
    // Unregister handlers BEFORE destroying hardware
    unregister_handlers();

    if (receiver_active_) {
        restore_receiver();
    }
}

void AnalogVideoView::register_handlers() noexcept {
    if (handlers_active_) return;

    auto* const h = reinterpret_cast<HandlerStorage*>(handler_storage_);

    // Handler for ChannelSpectrumConfig -- receives the FIFO pointer from baseband
    new (&h->spectrum_config) MessageHandlerRegistration{
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);

            // HIGH FIX: Validate FIFO pointer before use.
            // If baseband failed to initialize the FIFO, message.fifo will be nullptr.
            // Only assign if valid — prevents null dereference in frame_sync handler.
            if (message.fifo != nullptr) {
                this->spectrum_fifo_ = message.fifo;
            }
        }
    };

    // Handler for DisplayFrameSync -- dequeue spectra and feed to video widget
    // Uses class member spectrum_buffer_ instead of stack allocation (272 bytes saved).
    new (&h->frame_sync) MessageHandlerRegistration{
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            if (this->spectrum_fifo_ != nullptr) {
                while (this->spectrum_fifo_->out(this->spectrum_buffer_)) {
                    this->video_widget_.on_channel_spectrum(this->spectrum_buffer_);
                }
            }
        }
    };

    handlers_active_ = true;
}

void AnalogVideoView::unregister_handlers() noexcept {
    if (!handlers_active_) return;

    auto* const h = reinterpret_cast<HandlerStorage*>(handler_storage_);
    h->spectrum_config.~MessageHandlerRegistration();
    h->frame_sync.~MessageHandlerRegistration();

    handlers_active_ = false;
    spectrum_fifo_ = nullptr;
}

void AnalogVideoView::setup_video_receiver() noexcept {
    // Stack: ~64 bytes (chThdSleepMilliseconds frame)
    // Flash: ~128 bytes (function calls)

    audio::output::mute();

    // Shut down current baseband (wideband spectrum from scanner)
    baseband::shutdown();

    // Load AM TV baseband image from SPI flash by image tag.
    // NOTE: The pre-linked code at memory::map::m4_code.base() is the host app's
    // baseband (drone scanner wideband), NOT the AM TV baseband.
    // The AM TV baseband is only accessible via SPI flash tag {'P','A','M','T'}.
    // This matches image_tag_am_tv in the external analogtv app.
    static constexpr portapack::spi_flash::image_tag_t am_tv_tag = {'P', 'A', 'M', 'T'};
    baseband::run_image(am_tv_tag);

    // Let baseband image fully settle before sending configuration commands.
    // run_image waits for baseband_ready, but an extra 1ms prevents
    // send_message() spin collisions when receiver_model calls follow immediately.
    chThdSleepMilliseconds(1);

    // Configure RF frontend for analog video.
    // Must match external analogtv: modulation=WFM, 2MHz sampling.
    // The AM TV baseband processes raw IQ regardless of modulation setting.
    receiver_model.set_modulation(ReceiverModel::Mode::WidebandFMAudio);
    receiver_model.set_sampling_rate(2000000);
    receiver_model.set_baseband_bandwidth(2000000);
    receiver_model.set_target_frequency(frequency_);
    receiver_model.enable();
}

void AnalogVideoView::restore_receiver() noexcept {
    // Stack: ~0 bytes

    audio::output::mute();
    baseband::spectrum_streaming_stop();
    receiver_model.disable();
    baseband::shutdown();
}

void AnalogVideoView::on_show() {
    // Guard: prevent double-initialization
    if (receiver_active_) {
        return;
    }

    // MEDIUM: Stop any ongoing audio alerts before video playback.
    // Prevents SOS beep from interfering with video audio (if unmuted).
    AudioAlertManager::stop_alert();

    if (!is_frequency_valid(frequency_)) {
        // Invalid frequency -- show empty screen with "Invalid Freq"
        video_widget_.on_show();
        return;
    }

    // CRITICAL: Register message handlers BEFORE enabling the receiver.
    // The baseband sends ChannelSpectrumConfig (containing the FIFO pointer)
    // during initialization, right after run_image completes.
    // If handlers aren't registered by then, spectrum_fifo_ stays nullptr
    // and the DisplayFrameSync handler silently drops all spectra → black screen.
    register_handlers();

    // Setup hardware: load AM TV baseband, configure receiver
    setup_video_receiver();

    // CRITICAL: Wait for PLL to settle after frequency change before starting
    // spectrum capture. Si5351/MAX2837 PLL requires ~1-5ms to lock.
    // Without this delay, the first FFT frames capture stale frequency data.
    chThdSleepMilliseconds(5);

    // Start spectrum streaming
    baseband::spectrum_streaming_start();

    // Notify video widget that we're live
    video_widget_.on_show();
    receiver_active_ = true;
}

void AnalogVideoView::on_hide() {
    video_widget_.on_hide();

    // Unregister handlers before stopping hardware
    unregister_handlers();

    if (receiver_active_) {
        restore_receiver();
        receiver_active_ = false;
    }
}

void AnalogVideoView::paint(Painter& painter) {
    // Stack: ~16 bytes (freq_str[16])

    // CRITICAL: Use screen_rect() for absolute screen coordinates.
    // Painter methods (fill_rectangle, draw_string) use absolute coords.
    // screen_rect() = parent_rect + parent()->screen_pos()
    // For this view: {NavView.left, NavView.top+HEADER_H, 240, 304}
    const auto r = screen_rect();

    // Draw header background at the view's screen position
    painter.fill_rectangle({r.left(), r.top(), r.width(), HEADER_H}, Color::black());

    // Paint children (video_widget_ — paint is a no-op)
    View::paint(painter);

    // Cache theme pointers — eliminate repeated dereference chains
    const auto theme = Theme::getInstance();
    const auto& font = theme->fg_light->font;
    const auto  fg   = theme->fg_light->foreground;

    // Format frequency for header display
    char freq_str[16];
    if (!is_frequency_valid(frequency_)) {
        freq_str[0] = 'B'; freq_str[1] = 'a'; freq_str[2] = 'd';
        freq_str[3] = ' '; freq_str[4] = 'F'; freq_str[5] = 'r';
        freq_str[6] = 'e'; freq_str[7] = 'q'; freq_str[8] = '\0';
    } else {
        // One 64-bit div instead of two (saves ~80 cycles on Cortex-M4F)
        const uint32_t khz_total = static_cast<uint32_t>(frequency_ / 1000ULL);
        const uint32_t mhz = khz_total / 1000u;
        const uint32_t khz = khz_total % 1000u;

        char* p = freq_str;
        uint32_t m = mhz;

        if (m >= 1000) { *p++ = '0' + static_cast<char>((m / 1000) % 10); }
        if (m >= 100)  { *p++ = '0' + static_cast<char>((m / 100) % 10); }
        if (m >= 10)   { *p++ = '0' + static_cast<char>((m / 10) % 10); }
        *p++ = '0' + static_cast<char>(m % 10);

        *p++ = '.';
        *p++ = '0' + static_cast<char>((khz / 100) % 10);
        *p++ = '0' + static_cast<char>((khz / 10) % 10);
        *p++ = '0' + static_cast<char>(khz % 10);

        *p++ = ' '; *p++ = 'M'; *p++ = 'H'; *p++ = 'z'; *p = '\0';
    }

    // Offset from view's screen position so frequency text appears within the header
    painter.draw_string(
        Point{r.left() + 4, r.top() + 4},
        font, fg,
        Color::black(),
        freq_str);

    // Show X correction value for encoder feedback
    const uint8_t xc = video_widget_.x_correction();
    char xc_str[8];
    xc_str[0] = 'X'; xc_str[1] = ':';
    xc_str[2] = '0' + (xc / 10);
    xc_str[3] = '0' + (xc % 10);
    xc_str[4] = '\0';
    painter.draw_string(
        Point{r.right() - 36, r.top() + 4},
        font, fg,
        Color::black(),
        xc_str);
}

void AnalogVideoView::focus() {
    video_widget_.focus();
}

bool AnalogVideoView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Back || key == KeyEvent::Select || key == KeyEvent::Left) {
        nav_.pop();
        return true;
    }
    if (key == KeyEvent::Up) {
        video_widget_.x_correction_increment(1);
        set_dirty();
        return true;
    }
    if (key == KeyEvent::Down) {
        video_widget_.x_correction_increment(-1);
        set_dirty();
        return true;
    }
    return false;
}

bool AnalogVideoView::on_encoder(const EncoderEvent delta) {
    // Joystick encoder adjusts horizontal X correction for video alignment
    const int8_t step = (delta > 0) ? 1 : -1;
    video_widget_.x_correction_increment(step);
    set_dirty();
    return true;
}

[[nodiscard]] bool AnalogVideoView::is_valid() const noexcept {
    return receiver_active_ && is_frequency_valid(frequency_);
}

}  // namespace drone_analyzer