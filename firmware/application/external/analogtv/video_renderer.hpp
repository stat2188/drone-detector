/*
 * Copyright (C) 2024 Mayhem Firmware Contributors
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef VIDEO_RENDERER_HPP
#define VIDEO_RENDERER_HPP

#include "analogtv_constants.hpp"
#include "spectrum_color_lut.hpp"
#include "ui.hpp"
#include "message.hpp"

#include <array>
#include <cstdint>
#include <cstddef>

namespace ui::external_app::analogtv {

/**
 * @brief LUT-optimized fullscreen video renderer
 *
 * Eliminates all per-pixel arithmetic by pre-computing:
 * - Horizontal scale-index LUT (display X -> source X)
 * - Vertical row-offset LUT (display Y -> buffer offset)
 * - Gamma-corrected intensity LUT (raw -> RGB565)
 *
 * The inner render loop reduces to two array lookups + memcpy
 * per pixel: zero division, zero multiplication.
 *
 * @note All buffers statically allocated. No heap, no stack overflow.
 * @note Flash overhead: ~1.3 KB for LUTs. Saves ~50K cycles per frame.
 */
class VideoRenderer {
public:
    VideoRenderer() = default;
    ~VideoRenderer() = default;

    VideoRenderer(const VideoRenderer&) = delete;
    VideoRenderer& operator=(const VideoRenderer&) = delete;
    VideoRenderer(VideoRenderer&&) = delete;
    VideoRenderer& operator=(VideoRenderer&&) = delete;

    /**
     * @brief Process spectrum callback into video buffer
     * @param spectrum Channel spectrum data (256 bins = 2 lines of 128 px)
     */
    void process_spectrum(const ChannelSpectrum& spectrum);

    /**
     * @brief Render a single video line to display using LUT lookups
     * @param y Display row index (0 to DISPLAY_HEIGHT - 1)
     * @param line_buffer Output color buffer (must hold DISPLAY_WIDTH pixels)
     */
    void render_line(uint16_t y, ui::Color* line_buffer) const;

    /**
     * @brief Full render of one frame (all display rows)
     */
    void render_frame();

    /**
     * @brief Clear video buffer and reset state
     */
    void clear_buffer();

    /**
     * @brief Check if a complete frame has been accumulated
     */
    bool is_frame_ready() const { return frame_ready_; }

    /**
     * @brief Reset the frame-ready flag after rendering
     */
    void reset_frame_ready() { frame_ready_ = false; }

    /**
     * @brief Set horizontal correction offset
     * @param correction Offset in pixels (0 to MAX_X_CORRECTION)
     */
    void set_x_correction(uint8_t correction);

    /**
     * @brief Detect video carrier signature in spectrum data
     * @param spectrum_data Raw spectrum array
     * @return true if carrier-like signal found
     */
    bool detect_video_carrier(const std::array<uint8_t, 256>& spectrum_data) const;

private:
    /**
     * @brief Maximum safe x_correction for buffer bounds
     * @note buffer_size = source_lines * VIDEO_WIDTH + padding
     *       max_offset for last line = (SOURCE_LINES-1)*VIDEO_WIDTH + 127 + xcorr
     *       must be < buffer_size
     */
    static constexpr uint8_t MAX_X_CORRECTION = 127;

    /**
     * @brief Source lines in video buffer (before line doubling)
     */
    static constexpr uint16_t SOURCE_LINES = 104;

    /**
     * @brief Buffer padding for x_correction safety (matches classic variant)
     */
    static constexpr size_t BUFFER_PADDING = 128;

    /**
     * @brief Total video buffer size
     */
    static constexpr size_t BUFFER_SIZE =
        static_cast<size_t>(SOURCE_LINES) * VIDEO_WIDTH + BUFFER_PADDING;

    /**
     * @brief Number of spectrum callbacks per frame
     */
    static constexpr uint32_t CALLBACKS_PER_FRAME = 52;

    /**
     * @brief Carrier detection threshold (dBm)
     */
    static constexpr int32_t CARRIER_THRESHOLD_DBM = -80;

    /**
     * @brief Check if spectrum value exceeds carrier threshold
     */
    bool is_carrier_value(uint8_t value) const;

    // Video buffer (BSS section, not stack)
    uint8_t video_buffer_[BUFFER_SIZE]{};

    // Processing state
    uint32_t callback_count_{0};
    uint8_t x_correction_{DEFAULT_X_CORRECTION};
    bool frame_ready_{false};

    // Pre-computed LUTs (defined in video_renderer.cpp, stored in flash)
    // horizontal_index_lut_[x] = source pixel column for display column x
    static const uint8_t horizontal_index_lut_[DISPLAY_WIDTH];

    // vertical_offset_lut_[y] = byte offset into video_buffer_ for display row y
    static const uint16_t vertical_offset_lut_[DISPLAY_HEIGHT];

    // intensity_lut_[raw] = RGB565 color with gamma correction
    // Replaces: 255 - raw  +  spectrum_rgb4_lut[]
    static const ui::Color intensity_lut_[SPECTRUM_COLOR_LUT_SIZE];
};

}  // namespace ui::external_app::analogtv

#endif /* VIDEO_RENDERER_HPP */
