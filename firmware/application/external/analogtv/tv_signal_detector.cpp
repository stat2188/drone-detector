/*
 * Copyright (C) 2024 Shao
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

#include "tv_signal_detector.hpp"
#include <cstring>
#include <algorithm>

namespace ui::external_app::analogtv {

TVSignalDetector::TVSignalDetector() {
}

// Single-pass signal detection algorithm for optimal performance
TVSignalDetector::DetectionResult TVSignalDetector::detect_tv_signal(
    const ChannelSpectrum& spectrum,
    int64_t current_frequency) {

    DetectionResult result;
    result.frequency = current_frequency;

    const auto* db = spectrum.db.data();

    // Initialize all metrics in one pass
    int max_db = -127;
    int avg_db = 0;
    int left_power = 0;
    int right_power = 0;
    int video_peak_idx = -1;
    int video_peak_val = -127;
    int audio_peak_idx = -1;
    int audio_peak_val = -127;
    int first_edge = -1;
    int last_edge = -1;

    // Single pass through spectrum data - data-oriented design
    for (int i = 0; i < DetectorConstants::TV_CHANNEL_WIDTH_SAMPLES; i++) {
        const int v = db[i];

        // Track maximum signal
        if (v > max_db) {
            max_db = v;
        }

        // Accumulate for average calculation
        avg_db += v;

        // Split power calculation (left/right halves)
        if (i < DetectorConstants::SPECTRUM_HALF_WIDTH) {
            left_power += v;
        } else {
            right_power += v;
        }

        // Video carrier peak detection (first half)
        if (i < DetectorConstants::VIDEO_PEAK_THRESHOLD_IDX && v > video_peak_val) {
            video_peak_val = v;
            video_peak_idx = i;
        }

        // Audio carrier peak detection (last quarter)
        if (i >= DetectorConstants::AUDIO_PEAK_START_IDX && v > audio_peak_val) {
            audio_peak_val = v;
            audio_peak_idx = i;
        }

        // Edge detection for bandwidth measurement
        if (first_edge == -1 && v > DetectorConstants::MIN_SIGNAL_DB) {
            first_edge = i;
        }
        if (v > DetectorConstants::MIN_SIGNAL_DB) {
            last_edge = i;
        }
    }

    // Calculate average (divide by 256 using bit shift)
    avg_db >>= 8;
    result.signal_strength = static_cast<int>(max_db);

    // Signal-to-noise ratio check
    const int signal_to_noise = max_db - avg_db;
    if (max_db < DetectorConstants::MIN_SIGNAL_DB || 
        signal_to_noise < DetectorConstants::CARRIER_THRESHOLD_DB) {
        return result;
    }

    // Video carrier validation
    if (video_peak_idx == -1 || video_peak_val < DetectorConstants::MIN_CARRIER_DB) {
        return result;
    }

    // Audio carrier validation
    if (audio_peak_idx == -1 || audio_peak_val < DetectorConstants::MIN_CARRIER_DB) {
        return result;
    }

    // Carrier ordering check (video must come before audio)
    if (video_peak_idx >= audio_peak_idx) {
        return result;
    }

    // Carrier spacing check
    const int spacing = audio_peak_idx - video_peak_idx;
    if (spacing < DetectorConstants::MIN_CARRIER_SPACING || 
        spacing > DetectorConstants::MAX_CARRIER_SPACING) {
        return result;
    }

    // Bandwidth validation
    if (first_edge == -1 || last_edge == -1) {
        return result;
    }

    const int bandwidth = last_edge - first_edge;
    if (bandwidth < DetectorConstants::MIN_BANDWIDTH_SAMPLES || 
        bandwidth > DetectorConstants::MAX_BANDWIDTH_SAMPLES) {
        return result;
    }

    // Asymmetry check (TV signals have asymmetric power distribution)
    int asymmetry = left_power - right_power;
    asymmetry = std::abs(asymmetry);
    if (asymmetry < DetectorConstants::MIN_ASYMMETRY) {
        return result;
    }

    // PAL/NTSC discrimination based on video carrier position
    if (video_peak_idx < DetectorConstants::PAL_NTSC_THRESHOLD_IDX) {
        // Use safe string copy with null termination
        size_t len = std::strlen(DetectorConstants::MODULATION_PAL);
        len = std::min(len, DetectorConstants::MODULATION_TYPE_MAX_LEN);
        std::memcpy(result.modulation_type.data(), DetectorConstants::MODULATION_PAL, len);
        result.modulation_type[len] = '\0';
    } else {
        size_t len = std::strlen(DetectorConstants::MODULATION_NTSC);
        len = std::min(len, DetectorConstants::MODULATION_TYPE_MAX_LEN);
        std::memcpy(result.modulation_type.data(), DetectorConstants::MODULATION_NTSC, len);
        result.modulation_type[len] = '\0';
    }

    result.video_carrier_offset = video_peak_idx;
    result.audio_carrier_offset = audio_peak_idx;
    result.is_tv_signal = true;

    return result;
}

} // namespace ui::external_app::analogtv
