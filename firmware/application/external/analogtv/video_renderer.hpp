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

    void process_spectrum(const ChannelSpectrum& spectrum);
    void render_line(uint16_t y, ui::Color* line_buffer) const;
    void render_frame();
    void clear_buffer();
    bool is_frame_ready() const { return frame_ready_; }
    void reset_frame_ready() { frame_ready_ = false; }
    void set_x_correction(uint8_t correction);
    bool detect_video_carrier(const std::array<uint8_t, 256>& spectrum_data) const;

private:
    static constexpr uint8_t MAX_X_CORRECTION = 127;
    static constexpr uint16_t SOURCE_LINES = 104;
    static constexpr size_t BUFFER_PADDING = 128;
    static constexpr size_t BUFFER_SIZE =
        static_cast<size_t>(SOURCE_LINES) * VIDEO_WIDTH + BUFFER_PADDING;
    static constexpr uint32_t CALLBACKS_PER_FRAME = 52;
    static constexpr int32_t CARRIER_THRESHOLD_DBM = -80;

    bool is_carrier_value(uint8_t value) const;

    uint8_t video_buffer_[BUFFER_SIZE]{};
    uint32_t callback_count_{0};
    uint8_t x_correction_{DEFAULT_X_CORRECTION};
    bool frame_ready_{false};

    // Pre-computed LUTs (std::array for constexpr init, stored in flash)
    static const std::array<uint8_t, DISPLAY_WIDTH> horizontal_index_lut_;
    static const std::array<uint16_t, DISPLAY_HEIGHT> vertical_offset_lut_;
    static const std::array<ui::Color, SPECTRUM_COLOR_LUT_SIZE> intensity_lut_;
};

}  // namespace ui::external_app::analogtv

#endif /* VIDEO_RENDERER_HPP */
