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

#include <cstdint>
#include <cstddef>
#include <array>
#include <string>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "message.hpp"
#include "drone_types.hpp"
#include "constants.hpp"  // For MIN_FREQUENCY_HZ, MAX_FREQUENCY_HZ

namespace drone_analyzer {

/**
 * @brief Lightweight analog video widget — zero allocation, no std::function, no std::unique_ptr
 *
 * Self-contained Widget that renders analog TV video from ChannelSpectrum data.
 * Replaces the heavyweight ui::tv::TVWidget (uses banned std::unique_ptr/std::function
 * and has namespace collisions with external/analogtv).
 *
 * Memory:
 *   BSS: ~260 bytes (line_buffer_ + state) — class member, NOT stack
 *   Stack per paint(): 0 bytes (line buffer is class member)
 *   Flash: ~400 bytes (code)
 *
 * @note No MessageHandlerRegistration — parent pushes data via public method
 * @note No heap allocation — all buffers are compile-time fixed arrays
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
     * @brief Push a ChannelSpectrum sample for video rendering
     * @param spectrum Incoming spectrum data
     * @note Stack: ~8 bytes
     * @note Called from UI thread only (DisplayFrameSync handler)
     */
    void on_channel_spectrum(const ChannelSpectrum& spectrum);

    /**
     * @brief No-op stub: audio spectrum not rendered in this view
     */
    void show_audio_spectrum_view(bool) const noexcept {}

private:
    static constexpr uint16_t VIDEO_WIDTH = 240;
    static constexpr uint16_t VIDEO_HEIGHT = 284;

    /** @brief Scanline buffer — BSS, 256 bytes */
    std::array<uint8_t, 256> line_buffer_{};

    uint32_t frame_count_{0};    //!< Frame counter for sync
    bool active_{false};          //!< Rendering active
    uint8_t x_correction_{0};    //!< Horizontal correction offset
};

/**
 * @brief Full-screen analog video view with frequency header
 *
 * Uses lightweight VideoWidget (no external deps, no heap).
 * Hardware control is separated from UI paint logic.
 *
 * Memory:
 *   BSS: ~264 bytes (AnalogVideoView + VideoWidget)
 *   Stack per paint(): ~48 bytes (freq_str[24] + local vars)
 *   Flash: ~512 bytes (code)
 */
class AnalogVideoView : public ui::View {
public:
    static constexpr uint16_t HEADER_H = 16;
    static constexpr uint16_t SCALE_H = 20;
    static constexpr uint16_t VIDEO_H = 284;

    explicit AnalogVideoView(NavigationView& nav, FreqHz frequency) noexcept;
    ~AnalogVideoView() noexcept override;

    AnalogVideoView(const AnalogVideoView&) = delete;
    AnalogVideoView& operator=(const AnalogVideoView&) = delete;

    void on_show() override;
    void on_hide() override;
    void paint(Painter& painter) override;
    void focus() override;

    /**
     * @brief View title string (matches base class signature)
     */
    std::string title() const override {
        static const std::string t = "FPV Video";
        return t;
    }

    /**
     * @brief Forward spectrum data to the video rendering pipeline
     * @param spectrum Incoming ChannelSpectrum data
     * @note Safe to call from message handler callbacks (UI thread)
     */
    void on_video_spectrum(const ChannelSpectrum& spectrum) {
        video_widget_.on_channel_spectrum(spectrum);
    }

    /**
     * @brief Check if video receiver is configured and ready
     */
    [[nodiscard]] bool is_valid() const noexcept;

private:
    NavigationView& nav_;
    FreqHz frequency_{0};

    // Zero-allocation video rendering widget (BSS member)
    VideoWidget video_widget_{};

    // Guard against double-initialization
    bool receiver_active_{false};

    void setup_video_receiver() noexcept;
    void restore_receiver() noexcept;

    [[nodiscard]] static bool is_frequency_valid(FreqHz freq) noexcept;
};

}  // namespace drone_analyzer

#endif  // ANALOG_VIDEO_VIEW_HPP