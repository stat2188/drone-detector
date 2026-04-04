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

#include "ui.hpp"
#include "message.hpp"

#include <cstdint>
#include <cstddef>

namespace ui::external_app::analogtv {

/**
 * @brief Analog TV video renderer (M4 side) — Pre-converted Color Buffer
 *
 * Architecture:
 * - M0: streams raw spectrum (256 bins per callback) via FIFO
 * - M4: converts to Color on ingest, renders with zero per-pixel math
 *
 * Speed strategy: Color conversion happens during spectrum reception
 * (spread across 52 callbacks), not during render.
 * Render loop: pure memory copy — no LUT, no arithmetic.
 *
 * Memory layout:
 * - video_buffer: 13312 + 128 × sizeof(Color) = ~26.5 KB (.bss, M4 RAM)
 * - line_buffer: 128 × sizeof(Color) = 256 bytes (stack per render call)
 *
 * @note 2x render speed vs uint8_t buffer approach
 * @note No heap allocation, no LUT during render
 * @note M0 does zero DSP — only raw sample streaming
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
     * @brief Process one spectrum callback (256 bins = 2 source lines)
     * @param spectrum Channel spectrum data from M0 FIFO
     * @return true if a complete frame was assembled and rendered
     */
    bool process_spectrum(const ChannelSpectrum& spectrum);

    /**
     * @brief Clear the video buffer and reset frame counter
     */
    void clear_buffer();

    /**
     * @brief Set horizontal alignment offset
     * @param correction Pixel offset (0-128)
     */
    void set_x_correction(uint8_t correction);

    /**
     * @brief Get current horizontal correction
     */
    uint8_t get_x_correction() const { return x_correction_; }

private:
    /**
     * @brief Render accumulated frame line-by-line with line doubling
     */
    void render_frame();

    /**
     * @brief Render a single source line to display (doubled vertically)
     * @param source_line Source line index (0-103)
     */
    void render_source_line(uint16_t source_line);

    static constexpr size_t VIDEO_WIDTH = 128;
    static constexpr size_t SOURCE_LINES = 104;
    static constexpr size_t BUFFER_SIZE = VIDEO_WIDTH * SOURCE_LINES;
    static constexpr size_t BUFFER_PADDING = 128;
    static constexpr size_t X_CORRECTION_MAX = 128;
    static constexpr uint32_t CALLBACKS_PER_FRAME = 52;
    static constexpr size_t SPECTRUM_BINS = 256;

    ui::Color video_buffer_[BUFFER_SIZE + BUFFER_PADDING]{};
    uint32_t callback_count_{0};
    uint8_t x_correction_{10};
};

}  // namespace ui::external_app::analogtv

#endif /* VIDEO_RENDERER_HPP */
