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

#include <cstring>
#include <cstdio>
#include <new>

#include "baseband_api.hpp"
#include "portapack.hpp"
#include "portapack_shared_memory.hpp"
#include "audio.hpp"
#include "spectrum_color_lut.hpp"
#include "lcd_ili9341.hpp"

using namespace portapack;

namespace drone_analyzer {

// ============================================================================
// VideoWidget implementation
// ============================================================================
// Memory:
//   BSS: ~13,440 bytes (video_buffer_[13312+128])
//   Stack per on_channel_spectrum(): 0 bytes
//   Stack per render_frame(): ~256 bytes (line_buffer of ui::Color[128])

VideoWidget::VideoWidget()
    : video_buffer_{}
    , frame_count_{0}
    , active_{false}
    , x_correction_{10} {
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

    // Accumulate 256 spectrum bytes into frame buffer
    // Each callback provides 256 bytes which is 2 video lines at 128px each
    const size_t offset = frame_count_ * 256;
    for (size_t i = 0; i < 256 && offset + i < VIDEO_BUFFER_SIZE; ++i) {
        // Invert: spectrum.db[i]=0 (strong) -> 255 (white), spectrum.db[i]=255 (noise) -> 0 (black)
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
    // Stack: ~256 bytes (line_buffer[128] of ui::Color = 128 * 2 bytes = 256 bytes)
    // Flash: ~200 bytes (loop code)

    // Render 104 native video lines, each doubled to span 208 screen lines
    // Starting at Y=VIDEO_START_Y (below header), centered at X=VIDEO_START_X
    // Uses spectrum_rgb4_lut for grayscale color mapping (same as analogtv)

    ui::Color line_buffer[LINE_WIDTH];  // Stack: 256 bytes

    for (uint16_t line = 0; line < VIDEO_LINES_HALF; ++line) {
        // Build one video line from the frame buffer with x_correction
        const size_t buf_offset = line * 128;
        for (uint16_t px = 0; px < LINE_WIDTH; ++px) {
            // Apply horizontal correction (shift by x_correction_ pixels)
            // Wrap around within the 128-pixel line
            const size_t src_idx = buf_offset + ((px + x_correction_) % LINE_WIDTH);
            const uint8_t pixel_val = video_buffer_[src_idx];
            line_buffer[px] = spectrum_rgb4_lut[pixel_val];
        }

        // Write each native line twice (line doubling for vertical scaling)
        const ui::Coord y0 = VIDEO_START_Y + static_cast<ui::Coord>(line * 2);
        const ui::Coord y1 = VIDEO_START_Y + static_cast<ui::Coord>(line * 2 + 1);

        portapack::display.render_line(
            {VIDEO_START_X, y0},
            LINE_WIDTH,
            line_buffer);
        portapack::display.render_line(
            {VIDEO_START_X, y1},
            LINE_WIDTH,
            line_buffer);
    }
}

// ============================================================================
// AnalogVideoView implementation
// ============================================================================
// Memory:
//   BSS: ~13,600 bytes (VideoWidget + state + handler storage)
//   Stack per paint(): ~48 bytes (freq_str[24] + loop vars)
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
            this->spectrum_fifo_ = message.fifo;
        }
    };

    // Handler for DisplayFrameSync -- dequeue spectra and feed to video widget
    new (&h->frame_sync) MessageHandlerRegistration{
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            if (this->spectrum_fifo_ != nullptr) {
                ChannelSpectrum spectrum;
                while (this->spectrum_fifo_->out(spectrum)) {
                    this->video_widget_.on_channel_spectrum(spectrum);
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
    // Stack: ~0 bytes
    // Flash: ~128 bytes (function calls)

    audio::output::mute();

    // Shut down current baseband (wideband spectrum from scanner)
    baseband::shutdown();

    // Load AM TV baseband image
    // The tag {'P','A','M','T'} matches image_tag_am_tv in the external analogtv app
    static constexpr portapack::spi_flash::image_tag_t am_tv_tag = {'P', 'A', 'M', 'T'};
    baseband::run_image(am_tv_tag);

    // Configure receiver for analog TV: WFM modulation at 2MHz sampling rate
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

    if (!is_frequency_valid(frequency_)) {
        // Invalid frequency -- show empty screen with "Invalid Freq"
        video_widget_.on_show();
        return;
    }

    // Setup hardware: load AM TV baseband, configure receiver
    setup_video_receiver();

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
    // Stack: ~48 bytes (freq_str[24] + loop vars)

    // First paint children (video_widget_)
    View::paint(painter);

    // Draw header background
    painter.fill_rectangle({0, 0, 240, HEADER_H}, Color::black());

    // Format frequency for header display
    char freq_str[24];
    if (!is_frequency_valid(frequency_)) {
        strncpy(freq_str, "Invalid Freq", sizeof(freq_str) - 1);
        freq_str[sizeof(freq_str) - 1] = '\0';
    } else {
        const uint32_t mhz = static_cast<uint32_t>(frequency_ / 1'000'000ULL);
        const uint32_t khz = static_cast<uint32_t>((frequency_ % 1'000'000ULL) / 1'000ULL);
        snprintf(freq_str, sizeof(freq_str), "%lu.%03lu MHz",
                 static_cast<unsigned long>(mhz),
                 static_cast<unsigned long>(khz));
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