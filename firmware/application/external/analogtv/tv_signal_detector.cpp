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

namespace ui::external_app::analogtv {

TVSignalDetector::TVSignalDetector() {
    // Constructor
}

TVSignalDetector::DetectionResult TVSignalDetector::detect_tv_signal(
    const ChannelSpectrum& spectrum, 
    int64_t current_frequency) {
    
    DetectionResult result;
    result.frequency = current_frequency;
    // Calculate max_db from the db array since ChannelSpectrum doesn't have max_db member
    int max_db = -127;
    for (int i = 0; i < 256; i++) {
        if (spectrum.db[i] > max_db) {
            max_db = spectrum.db[i];
        }
    }
    result.signal_strength = max_db;
    
    // Check signal level
    if (!check_signal_strength(spectrum)) {
        return result;
    }
    
    // Find carriers
    int video_offset = 0;
    int audio_offset = 0;
    
    if (!find_carriers(spectrum, video_offset, audio_offset)) {
        return result;
    }
    
    // Validate carrier spacing
    if (!validate_carrier_spacing(video_offset, audio_offset)) {
        return result;
    }
    
    // Check spectrum shape
    if (!check_spectrum_shape(spectrum)) {
        return result;
    }
    
    // Determine modulation type
    result.modulation_type = determine_modulation_type(spectrum);
    result.video_carrier_offset = video_offset;
    result.audio_carrier_offset = audio_offset;
    result.is_tv_signal = true;
    
    return result;
}

bool TVSignalDetector::check_signal_strength(const ChannelSpectrum& spectrum) {
    // Calculate max_db and avg_db from the db array since ChannelSpectrum doesn't have these members
    int max_db = -127;
    int sum_db = 0;
    
    for (int i = 0; i < 256; i++) {
        if (spectrum.db[i] > max_db) {
            max_db = spectrum.db[i];
        }
        sum_db += spectrum.db[i];
    }
    
    int avg_db = sum_db / 256;
    
    // Check minimum signal level
    if (max_db < MIN_SIGNAL_DB) {
        return false;
    }
    
    // Check difference between max and average
    int signal_to_noise = max_db - avg_db;
    if (signal_to_noise < CARRIER_THRESHOLD) {
        return false;
    }
    
    return true;
}

bool TVSignalDetector::find_carriers(const ChannelSpectrum& spectrum, 
                                   int& video_offset, int& audio_offset) {
    // Find video carrier in lower part of spectrum
    video_offset = find_peak_in_range(spectrum, 0, 64);
    if (video_offset == -1 || spectrum.db[video_offset] < MIN_CARRIER_DB) {
        return false;
    }
    
    // Find audio carrier in upper part of spectrum
    audio_offset = find_peak_in_range(spectrum, 192, 256);
    if (audio_offset == -1 || spectrum.db[audio_offset] < MIN_CARRIER_DB) {
        return false;
    }
    
    return true;
}

bool TVSignalDetector::validate_carrier_spacing(int video_offset, int audio_offset) {
    if (video_offset >= audio_offset) {
        return false;
    }
    
    int spacing = audio_offset - video_offset;
    
    // For PAL: ~176 samples (4.5 MHz)
    // For NTSC: ~180 samples (4.5 MHz)
    if (spacing < 160 || spacing > 200) {
        return false;
    }
    
    return true;
}

bool TVSignalDetector::check_spectrum_shape(const ChannelSpectrum& spectrum) {
    // Check channel width (should be around 6-8 MHz)
    int bandwidth = calculate_signal_bandwidth(spectrum);
    if (bandwidth < 180 || bandwidth > 240) {
        return false;
    }
    
    // Check TV signal asymmetry
    int left_power = 0;
    int right_power = 0;
    
    for (int i = 0; i < 128; i++) {
        left_power += spectrum.db[i];
        right_power += spectrum.db[255 - i];
    }
    
    // TV signal is usually asymmetric due to AM video and FM audio
    int asymmetry = abs(left_power - right_power);
    if (asymmetry < 500) {
        return false;
    }
    
    return true;
}

std::string TVSignalDetector::determine_modulation_type(const ChannelSpectrum& spectrum) {
    // Simple determination by carrier positions
    int video_offset = find_peak_in_range(spectrum, 0, 64);
    int audio_offset = find_peak_in_range(spectrum, 192, 256);
    
    if (video_offset != -1 && audio_offset != -1) {
        int spacing = audio_offset - video_offset;
        
        // PAL: video carrier is closer to channel edge
        if (video_offset < 30 && spacing > 170) {
            return "PAL";
        }
        // NTSC: video carrier is slightly farther from edge
        else if (video_offset > 15 && video_offset < 40 && spacing > 170) {
            return "NTSC";
        }
    }
    
    return "UNKNOWN";
}

int TVSignalDetector::find_peak_in_range(const ChannelSpectrum& spectrum, 
                                       int start_idx, int end_idx) {
    int max_db = -127;
    int peak_idx = -1;
    
    for (int i = start_idx; i < end_idx; i++) {
        if (spectrum.db[i] > max_db) {
            max_db = spectrum.db[i];
            peak_idx = i;
        }
    }
    
    return (max_db > MIN_CARRIER_DB) ? peak_idx : -1;
}

int TVSignalDetector::calculate_signal_bandwidth(const ChannelSpectrum& spectrum) {
    int first_edge = -1;
    int last_edge = -1;
    
    // Find left edge of signal
    for (int i = 0; i < 256; i++) {
        if (spectrum.db[i] > MIN_SIGNAL_DB) {
            first_edge = i;
            break;
        }
    }
    
    // Find right edge of signal
    for (int i = 255; i >= 0; i--) {
        if (spectrum.db[i] > MIN_SIGNAL_DB) {
            last_edge = i;
            break;
        }
    }
    
    if (first_edge == -1 || last_edge == -1) {
        return 0;
    }
    
    return last_edge - first_edge;
}

} // namespace ui::external_app::analogtv
