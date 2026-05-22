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
// VideoWidget static member definitions — BSS allocation (~13,952 bytes total)
// Shared across all VideoWidget instances, not per-instance.
// This eliminates ~13.5KB from each AnalogVideoView object, preventing OOM
// when nav_.push creates AnalogVideoView after DroneScannerUI.
// ============================================================================
uint8_t VideoWidget::video_buffer_[VideoWidget::VIDEO_BUFFER_SIZE];
std::array<ui::Color, VideoWidget::LINE_WIDTH> VideoWidget::line_buffer_;

// ============================================================================
// VideoWidget implementation
// ============================================================================
// Memory:
//   BSS (static): ~13,952 bytes (video_buffer_ + line_buffer_, shared across instances)
//   Instance: ~16 bytes (frame_count_ + active_ + x_correction_)
//   Stack per on_channel_spectrum(): 0 bytes
//   Stack per render_frame(): 0 bytes (line_buffer_ in BSS)

VideoWidget::VideoWidget()
    : frame_count_{0}
    , active_{false}
    , x_correction_{DEFAULT_X_CORRECTION} {
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
    // No child widgets to focus
}

void VideoWidget::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    // Stack: ~0 bytes (no local allocations)
    if (!active_) {
        return;
    }

    // 52 frames × 256 bytes = 13312, always inside VIDEO_BUFFER_SIZE (13440)
    const size_t offset = frame_count_ * 256;
    for (size_t i = 0; i < 256; ++i) {
        // Invert: strong signal (0) -> white (255), noise (255) -> black (0)
        video_buffer_[offset + i] = 255 - spectrum.db[i];
    }

    ++frame_count_;

    // When 52 spectra accumulated (104 video lines), render the frame
    if (frame_count_ >= ACCUMULATED_FRAMES) {
        render_frame();
        frame_count_ = 0;
    }
}

void VideoWidget::render_frame() noexcept {
    // Stack: 0 bytes (line_buffer_ lives in BSS)
    // Flash: ~200 bytes (loop code)

    for (uint16_t line = 0; line < VIDEO_LINES_HALF; ++line) {
        const size_t buf_offset = line * LINE_WIDTH;

        for (uint16_t px = 0; px < LINE_WIDTH; ++px) {
            // Fast modulo-128 via bitmask (LINE_WIDTH is power of two)
            const uint8_t src_idx = static_cast<uint8_t>((px + x_correction_) & (LINE_WIDTH - 1));
            line_buffer_[px] = spectrum_rgb4_lut[video_buffer_[buf_offset + src_idx]];
        }

        const ui::Coord y0 = VIDEO_START_Y + static_cast<ui::Coord>(line * 2);

        portapack::display.render_line(
            {VIDEO_START_X, y0},
            LINE_WIDTH,
            line_buffer_.data());

        portapack::display.render_line(
            {VIDEO_START_X, static_cast<ui::Coord>(y0 + 1)},
            LINE_WIDTH,
            line_buffer_.data());
    }
}

// ============================================================================
// AnalogVideoView implementation
// ============================================================================
// Memory:
//   BSS (static VideoWidget): ~13,952 bytes (shared, not per-instance)
//   Instance: ~592 bytes (VideoWidget ~16B + spectrum_buffer_ ~272B + state ~300B)
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

    // Load AM TV baseband image
    // The tag {'P','A','M','T'} matches image_tag_am_tv in the external analogtv app
    static constexpr portapack::spi_flash::image_tag_t am_tv_tag = {'P', 'A', 'M', 'T'};
    baseband::run_image(am_tv_tag);

    // Let baseband image fully settle before sending configuration commands.
    // run_image waits for baseband_ready, but an extra 1ms prevents
    // send_message() spin collisions when receiver_model calls follow immediately.
    chThdSleepMilliseconds(1);

    // Configure RF frontend for analog video at the target frequency.
    // NOTE: Modulation is intentionally NOT set here. The AM TV baseband
    // handles its own demodulation regardless of the modulation register.
    // The external analogtv app also keeps set_modulation() commented out
    // for this reason — setting WidebandFMAudio would conflict with raw AM
    // processing and may cause undefined baseband behavior.
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

    // Setup hardware: load AM TV baseband, configure receiver
    setup_video_receiver();

    // CRITICAL: Wait for PLL to settle after frequency change before starting
    // spectrum capture. Si5351/MAX2837 PLL requires ~1-5ms to lock.
    // Without this delay, the first FFT frames capture stale frequency data.
    chThdSleepMilliseconds(5);

    // Register message handlers for spectrum streaming
    register_handlers();

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
    // Stack: ~16 bytes (freq_str[16] + loop vars)

    // First paint children (video_widget_)
    View::paint(painter);

    // Draw header background
    painter.fill_rectangle({0, 0, 240, HEADER_H}, Color::black());

    // Format frequency for header display — inline zero-allocation decimal formatter
    char freq_str[16];
    if (!is_frequency_valid(frequency_)) {
        freq_str[0] = 'B'; freq_str[1] = 'a'; freq_str[2] = 'd';
        freq_str[3] = ' '; freq_str[4] = 'F'; freq_str[5] = 'r';
        freq_str[6] = 'e'; freq_str[7] = 'q'; freq_str[8] = '\0';
    } else {
        const uint32_t mhz = static_cast<uint32_t>(frequency_ / 1'000'000ULL);
        const uint32_t khz = static_cast<uint32_t>((frequency_ % 1'000'000ULL) / 1'000ULL);

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

    painter.draw_string(
        Point{4, 4},
        Theme::getInstance()->fg_light->font,
        Theme::getInstance()->fg_light->foreground,
        Color::black(),
        freq_str);

}

void AnalogVideoView::focus() {
    video_widget_.focus();
}

[[nodiscard]] bool AnalogVideoView::is_valid() const noexcept {
    return receiver_active_ && is_frequency_valid(frequency_);
}

}  // namespace drone_analyzer