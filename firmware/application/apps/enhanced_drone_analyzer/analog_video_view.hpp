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
 * Accumulates 52 × 256-byte ChannelSpectrum frames into a 13KB frame buffer,
 * then renders 104 video lines (doubled to 208 screen lines) via display.render_line()
 * using the spectrum_rgb4_lut greyscale color lookup table.
 *
 * Memory:
 *   BSS (static): ~13,952 bytes (video_buffer_[13312+128] + line_buffer_[256], shared)
 *   Instance: ~16 bytes (frame_count_ + active_ + x_correction_)
 *   Stack per on_channel_spectrum(): 0 bytes (no local allocations)
 *   Stack per render_frame(): 0 bytes (line_buffer_ in BSS)
 *   Flash: ~512 bytes (code)
 *
 * @note Audio is NOT rendered — simplified view for drone video inspection
 * @note No heap allocation — all buffers are compile-time fixed arrays
 * @note Static buffers are shared across ALL VideoWidget instances (BSS, not per-instance)
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
     * @note Accumulates 52 frames, then renders full video frame
     */
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;

    void show_audio_spectrum_view(bool) const noexcept {}

private:
    static constexpr uint16_t VIDEO_LINES = 208;     // 104 × 2 (line doubling)
    static constexpr uint16_t VIDEO_LINES_HALF = 104; // Native video lines
    static constexpr int16_t VIDEO_START_Y = 16;        // Below header
    static constexpr int16_t VIDEO_START_X = 56;        // Centered: (240-128)/2
    static constexpr uint8_t ACCUMULATED_FRAMES = 52; // Frames per video frame
    static constexpr size_t VIDEO_BUFFER_SIZE = 13312 + 128; // 52×256 + xcorr padding
    static constexpr uint16_t LINE_WIDTH = 128;       // Pixels per video line

    /**
     * @brief Default horizontal pixel shift for video centering.
     * @note Value 10 shifts image 10px right to center NTSC/PAL video on 128px display.
     *       Matches analogtv app behavior.
     */
    static constexpr uint8_t DEFAULT_X_CORRECTION = 10;

    /** @brief Frame buffer — BSS, ~13,440 bytes (shared across all VideoWidget instances) */
    static uint8_t video_buffer_[VIDEO_BUFFER_SIZE];

    /** @brief Line render buffer — BSS, eliminates 256-byte stack allocation (shared) */
    static std::array<ui::Color, LINE_WIDTH> line_buffer_;

    uint32_t frame_count_{0};    //!< Number of spectra accumulated (0..51)
    bool active_{false};          //!< Rendering active
    uint8_t x_correction_{DEFAULT_X_CORRECTION};   //!< Horizontal correction offset (default 10, matches analogtv)

    /**
     * @brief Render accumulated frame to display
     * @note Stack: 0 bytes (line_buffer_ in BSS)
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
 *   BSS (static): ~13,952 bytes (VideoWidget buffers shared, not per-instance)
 *   Instance: ~592 bytes (VideoWidget ~16B + spectrum_buffer_ ~272B + state ~300B)
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

    // Video rendering widget (BSS member, ~13.5KB)
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
