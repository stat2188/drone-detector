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

#include "video_processor.hpp"
#include "spectrum_color_lut.hpp"

namespace ui::external_app::analogtv {

void VideoProcessor::process_spectrum(const ChannelSpectrum& spectrum) {
    // Process 256 spectrum bins into video buffer
    // Each callback gives 256 samples = 2 lines of 128 pixels
    for (size_t i = 0; i < SPECTRUM_BINS; i++) {
        // Store inverted spectrum value (brighter = stronger signal)
        video_buffer_[i + count_ * SPECTRUM_BINS] = SPECTRUM_MAX_VALUE - spectrum.db[i];
    }

    count_++;

    // Check if we have a complete frame (52 lines = 104 pixels height, doubled)
    if (count_ >= 52) {
        frame_ready_ = true;
        count_ = 0;
    }
}

bool VideoProcessor::detect_video_carrier(const uint8_t* spectrum_data, size_t size) const {
    // Guard clause: validate input
    if (!spectrum_data || size < SPECTRUM_BINS) {
        return false;
    }

    // Scan spectrum for video carrier signature
    // Analog TV video carriers are typically:
    // 1. Strong signal (above threshold)
    // 2. Narrow bandwidth (3-10 bins)
    // 3. Stable (not noise spikes)

    uint8_t carrier_start_bin = 0;
    uint8_t consecutive_carrier_bins = 0;
    bool in_carrier = false;

    for (size_t i = 0; i < SPECTRUM_BINS; i++) {
        if (is_carrier_value(spectrum_data[i])) {
            if (!in_carrier) {
                // Start of potential carrier
                carrier_start_bin = i;
                in_carrier = true;
                consecutive_carrier_bins = 1;
            } else {
                // Continue carrier
                consecutive_carrier_bins++;
            }
        } else {
            if (in_carrier) {
                // End of carrier - check if valid
                if (consecutive_carrier_bins >= MIN_CARRIER_WIDTH_BINS &&
                    consecutive_carrier_bins <= MAX_CARRIER_WIDTH_BINS) {
                    // Valid carrier found
                    return true;
                }
                // Reset for next potential carrier
                in_carrier = false;
                consecutive_carrier_bins = 0;
            }
        }
    }

    // Check if carrier extends to end of spectrum
    if (in_carrier && 
        consecutive_carrier_bins >= MIN_CARRIER_WIDTH_BINS &&
        consecutive_carrier_bins <= MAX_CARRIER_WIDTH_BINS) {
        return true;
    }

    return false;
}

void VideoProcessor::render_video_line(uint16_t line_number, ui::Color* line_buffer) const {
    // Guard clause: validate input
    if (!line_buffer || line_number >= VIDEO_HEIGHT) {
        return;
    }

    // Calculate line offset in video buffer
    const size_t line_offset = (line_number / 2) * VIDEO_WIDTH;

    // Render line with spectrum color lookup
    for (size_t x = 0; x < VIDEO_WIDTH; x++) {
        const uint8_t value = video_buffer_[line_offset + x + x_correction_];
        line_buffer[x] = spectrum_rgb4_lut[value];
    }
}

void VideoProcessor::clear_buffer() {
    // Clear video buffer (set to black)
    for (size_t i = 0; i < VIDEO_BUFFER_SIZE; i++) {
        video_buffer_[i] = 0;
    }

    // Reset state
    count_ = 0;
    frame_ready_ = false;
    carrier_detected_count_ = 0;
    last_carrier_frequency_ = 0;
}

int32_t VideoProcessor::calculate_rssi(uint8_t value) const {
    // Convert spectrum value to RSSI in dBm
    // HackRF baseband: spectrum.db = clamp(dBV*5 + 255, 0, 255)
    // Approximate conversion: dBm = (value - 255) / 5 - gain_offset
    // Simplified: dBm = value - 120 (rough approximation)
    return static_cast<int32_t>(value) - 120;
}

bool VideoProcessor::is_carrier_value(uint8_t value) const {
    // Check if value represents a strong enough signal to be a carrier
    // Convert to RSSI and compare with threshold
    const int32_t rssi = calculate_rssi(value);
    return rssi >= CARRIER_THRESHOLD_DBM;
}

uint8_t VideoProcessor::find_carrier_width(const uint8_t* spectrum_data, size_t start_bin) const {
    // Guard clause: validate input
    if (!spectrum_data || start_bin >= SPECTRUM_BINS) {
        return 0;
    }

    // Find width of carrier starting at start_bin
    uint8_t width = 0;
    for (size_t i = start_bin; i < SPECTRUM_BINS; i++) {
        if (is_carrier_value(spectrum_data[i])) {
            width++;
            if (width > MAX_CARRIER_WIDTH_BINS) {
                // Too wide - probably not a carrier
                return 0;
            }
        } else {
            // End of carrier
            break;
        }
    }

    return width;
}

}  // namespace ui::external_app::analogtv