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

#ifndef VIDEO_PROCESSOR_HPP
#define VIDEO_PROCESSOR_HPP

#include "analogtv_constants.hpp"
#include "ui.hpp"
#include "message.hpp"

#include <array>

namespace ui::external_app::analogtv {

/**
 * @brief Video processing and carrier detection
 * 
 * This class handles:
 * - Video buffer management (static allocation)
 * - Spectrum data processing
 * - Video carrier detection algorithm
 * - Video line rendering
 * 
 * @note All buffers are statically allocated to avoid heap usage
 * @note Designed for minimal stack usage
 */
class VideoProcessor {
public:
    /**
     * @brief Construct a new Video Processor
     */
    VideoProcessor() = default;

    /**
     * @brief Destructor
     */
    ~VideoProcessor() = default;

    // Disable copy and move (contains large static buffers)
    VideoProcessor(const VideoProcessor&) = delete;
    VideoProcessor& operator=(const VideoProcessor&) = delete;
    VideoProcessor(VideoProcessor&&) = delete;
    VideoProcessor& operator=(VideoProcessor&&) = delete;

    /**
     * @brief Process spectrum data and update video buffer
     * @param spectrum Channel spectrum data
     */
    void process_spectrum(const ChannelSpectrum& spectrum);

    /**
     * @brief Detect video carrier in spectrum data
     * @param spectrum_data Spectrum data array (256 bins)
     * @return true if carrier detected
     */
    bool detect_video_carrier(const std::array<uint8_t, 256>& spectrum_data) const;

    /**
     * @brief Render video line to display
     * @param line_number Line number to render (0-based)
     * @param line_buffer Buffer to store rendered line
     */
    void render_video_line(uint16_t line_number, ui::Color* line_buffer) const;

    /**
     * @brief Clear video buffer
     */
    void clear_buffer();

    /**
     * @brief Get video buffer pointer (for direct access)
     * @return const uint8_t* Pointer to video buffer
     */
    const uint8_t* get_video_buffer() const { return video_buffer_; }

    /**
     * @brief Get video buffer size
     * @return size_t Buffer size in bytes
     */
    size_t get_buffer_size() const { return VIDEO_BUFFER_SIZE; }

    /**
     * @brief Set X correction value (horizontal alignment)
     * @param correction Correction value (0-128)
     */
    void set_x_correction(uint8_t correction) { 
        x_correction_ = (correction > 128) ? 128 : correction; 
    }

    /**
     * @brief Get current X correction value
     * @return uint8_t Current correction
     */
    uint8_t get_x_correction() const { return x_correction_; }

    /**
     * @brief Check if video buffer is ready for display
     * @return true if buffer has complete frame
     */
    bool is_frame_ready() const { return frame_ready_; }

    /**
     * @brief Reset frame ready flag
     */
    void reset_frame_ready() { frame_ready_ = false; }

private:
    /**
     * @brief Check if value is within carrier threshold
     * @param value Spectrum value
     * @return true if within threshold
     */
    bool is_carrier_value(uint8_t value) const;

    // Static video buffer (13KB) - in .bss section, not stack
    uint8_t video_buffer_[VIDEO_BUFFER_SIZE]{};

    // Processing state
    uint32_t count_{0};
    uint8_t x_correction_{DEFAULT_X_CORRECTION};
    bool frame_ready_{false};
};

}  // namespace ui::external_app::analogtv

#endif /* VIDEO_PROCESSOR_HPP */