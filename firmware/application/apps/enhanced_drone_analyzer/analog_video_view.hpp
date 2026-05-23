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

#ifndef ANALOG_VIDEO_VIEW_HPP
#define ANALOG_VIDEO_VIEW_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "message.hpp"
#include "drone_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Analog video rendering widget — minimum memory, maximum reliability.
 *
 * Accumulates 8 × 256-byte ChannelSpectrum frames (2.2KB buffer) into
 * 16 native lines, rendered as horizontal stripes across the screen.
 *
 * Memory:
 *   Instance: ~2.2KB (video_buffer_[2176] + state ~32B)
 *   Stack per render_frame(): ~520 bytes (line_buffer on stack, ~3/sec)
 *   Flash: ~512 bytes (code)
 *
 * @note Aggressively reduced for OOM prevention. FPS sacrificed for reliability.
 */
class VideoWidget : public ui::Widget {
public:
    VideoWidget();

    VideoWidget(const VideoWidget&) = delete;
    VideoWidget& operator=(const VideoWidget&) = delete;
    VideoWidget(VideoWidget&&) = delete;
    VideoWidget& operator=(VideoWidget&&) = delete;

    void on_show() override;
    void on_hide() override;
    void paint(Painter& painter) override;
    void focus() override;

    /**
     * @brief Push a ChannelSpectrum sample for video accumulation
     * @param spectrum Incoming 256-byte spectrum data
     * @note Stack: 0 bytes
     * @note Called from DisplayFrameSync handler (UI thread)
     * @note Accumulates 8 frames (16 native lines), then renders
     */
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;

    void show_audio_spectrum_view(bool) const noexcept {}

private:
    // 8 frames × 256 = 2048 → 16 native lines of 128px each
    static constexpr uint16_t VIDEO_LINES = 16;
    static constexpr uint16_t VIDEO_LINES_HALF = 16;
    static constexpr int16_t VIDEO_START_Y = 16;
    static constexpr int16_t VIDEO_START_X = 56;        // Centered: (240-128)/2
    static constexpr uint8_t ACCUMULATED_FRAMES = 8;    // Frames per video frame
    static constexpr size_t VIDEO_BUFFER_SIZE = 2176;   // 8×256 + 128 xcorr padding
    static constexpr uint16_t LINE_WIDTH = 128;         // Pixels per video line

    static constexpr uint8_t DEFAULT_X_CORRECTION = 10;

    /** @brief Frame buffer — instance member, ~2.2KB */
    uint8_t video_buffer_[VIDEO_BUFFER_SIZE]{};

    uint32_t frame_count_{0};    //!< Number of spectra accumulated (0..7)
    bool active_{false};          //!< Rendering active
    uint8_t x_correction_{DEFAULT_X_CORRECTION};   //!< Horizontal correction offset

    /**
     * @brief Render accumulated frame to display
     * @note Stack: ~520 bytes (line_buffer on stack, 512B + locals)
     * @note Uses display.render_line() for direct pixel write
     */
    void render_frame() noexcept;
};

/**
 * @brief Simplified analog video view with frequency header
 *
 * Loads AM TV baseband, configures receiver, registers ChannelSpectrum handlers,
 * and forwards data to VideoWidget for rendering.
 *
 * Memory:
 *   Instance: ~2,800 bytes (VideoWidget ~2.2KB + spectrum_buffer_ ~272B + handler_storage ~128B + state ~100B)
 *   Stack per paint(): ~48 bytes (freq_str[16] + locals)
 *   Stack per frame_sync handler: ~520 bytes (line_buffer in render_frame)
 *   Flash: ~768 bytes (code)
 *
 * @note No audio, no gain controls
 * @note 16 stripes of video at ~2.5 FPS
 * @note Press BACK to exit
 */
class AnalogVideoView : public ui::View {
public:
    static constexpr uint16_t HEADER_H = 16;

    explicit AnalogVideoView(NavigationView& nav, FreqHz frequency) noexcept;
    ~AnalogVideoView() noexcept override;

    AnalogVideoView(const AnalogVideoView&) = delete;
    AnalogVideoView& operator=(const AnalogVideoView&) = delete;

    void on_show() override;
    void on_hide() override;
    void paint(Painter& painter) override;
    void focus() override;
    bool on_key(const KeyEvent key) override;

    std::string title() const override {
        return "FPV Video";
    }

    [[nodiscard]] bool is_valid() const noexcept;

private:
    NavigationView& nav_;
    FreqHz frequency_{0};

    // Video rendering widget (on heap when view is pushed, ~2.2KB)
    VideoWidget video_widget_{};

    bool receiver_active_{false};

    /** @brief Storage for MessageHandlerRegistration — placement new for manual lifetime */
    struct HandlerStorage {
        MessageHandlerRegistration spectrum_config;
        MessageHandlerRegistration frame_sync;
    };
    static_assert(sizeof(HandlerStorage) <= 128, "HandlerStorage > 128 bytes");
    alignas(alignof(HandlerStorage)) uint8_t handler_storage_[sizeof(HandlerStorage)];
    bool handlers_active_{false};
    ChannelSpectrumFIFO* spectrum_fifo_{nullptr};

    // Reusable buffer to prevent 272-byte stack allocation in frame_sync handler.
    // Same pattern as DroneScannerUI::spectrum_buffer_ — class member instead of local.
    ChannelSpectrum spectrum_buffer_{};

    void register_handlers() noexcept;
    void unregister_handlers() noexcept;
    void setup_video_receiver() noexcept;
    void restore_receiver() noexcept;
    [[nodiscard]] static bool is_frequency_valid(FreqHz freq) noexcept;
};

}  // namespace drone_analyzer

#endif  // ANALOG_VIDEO_VIEW_HPP
