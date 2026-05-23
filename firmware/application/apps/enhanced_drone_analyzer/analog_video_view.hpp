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
 * @brief Analog video rendering widget — zero allocation, renders NTSC/PAL-like video
 *        from ChannelSpectrum data using the AM TV baseband.
 *
 * Accumulates 26 × 256-byte ChannelSpectrum frames into a 6.8KB frame buffer,
 * then renders 52 video lines (doubled to 104 screen lines) via display.render_line()
 * using the spectrum_rgb4_lut greyscale color lookup table.
 *
 * Memory: (per AnalogVideoView instance on heap)
 *   Instance: ~6,912 bytes (video_buffer_[6784] + line_buffer_[512] + state ~16B)
 *   Stack per on_channel_spectrum(): 0 bytes (no local allocations)
 *   Stack per render_frame(): 0 bytes (line_buffer_ is member)
 *   Flash: ~512 bytes (code)
 *
 * @note Audio is NOT rendered — simplified view for drone video inspection
 * @note No heap allocation beyond the instance itself
 * @note Halved vertical resolution (104→52 lines) for memory fit on 128KB SRAM
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
     * @note Accumulates 26 frames, then renders full video frame
     */
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;

    void show_audio_spectrum_view(bool) const noexcept {}

private:
    // Reduced from 52→26 frames to fit within 128KB SRAM:
    //   Before: 52 frames × 256 = 13,312 bytes BSS → OOM at app launch
    //   After:  26 frames × 256 =  6,656 bytes as instance → heap during video push
    static constexpr uint16_t VIDEO_LINES = 104;      // 52 × 2 (line doubling)
    static constexpr uint16_t VIDEO_LINES_HALF = 52;  // Native video lines
    static constexpr int16_t VIDEO_START_Y = 16;        // Below header
    static constexpr int16_t VIDEO_START_X = 56;        // Centered: (240-128)/2
    static constexpr uint8_t ACCUMULATED_FRAMES = 26; // Frames per video frame (reduced for memory)
    static constexpr size_t VIDEO_BUFFER_SIZE = 6784; // 26×256 + 128 xcorr padding
    static constexpr uint16_t LINE_WIDTH = 128;       // Pixels per video line

    static constexpr uint8_t DEFAULT_X_CORRECTION = 10;

    /** @brief Frame buffer — instance member, ~6.8KB on heap when AnalogVideoView is pushed */
    uint8_t video_buffer_[VIDEO_BUFFER_SIZE]{};

    /** @brief Line render buffer — instance member, eliminates 256-byte stack allocation */
    std::array<ui::Color, LINE_WIDTH> line_buffer_{};

    uint32_t frame_count_{0};    //!< Number of spectra accumulated (0..25)
    bool active_{false};          //!< Rendering active
    uint8_t x_correction_{DEFAULT_X_CORRECTION};   //!< Horizontal correction offset (default 10, matches analogtv)

    /**
     * @brief Render accumulated frame to display
     * @note Stack: 0 bytes (line_buffer_ is class member)
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
 *   Instance: ~7,504 bytes (VideoWidget ~6,912B + spectrum_buffer_ ~272B + state ~300B + handler ~16B)
 *   Stack per paint(): ~16 bytes (freq_str[16] + locals)
 *   Stack per frame_sync handler: 0 bytes (spectrum_buffer_ is class member)
 *   Flash: ~768 bytes (code)
 *
 * @note No audio — audio is muted for the session
 * @note No gain controls — simplified for drone video inspection
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

    std::string title() const override {
        static const std::string t = "FPV Video";
        return t;
    }

    [[nodiscard]] bool is_valid() const noexcept;

private:
    NavigationView& nav_;
    FreqHz frequency_{0};

    // Video rendering widget (heap member when view is pushed, ~6.9KB)
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
