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

#pragma once

#include "baseband_api.hpp"
#include "string_format.hpp"
#include <array>

namespace ui::external_app::analogtv {

// Named constants for TV signal detection
namespace DetectorConstants {
    // Spectrum analysis constants
    static constexpr int TV_CHANNEL_WIDTH_SAMPLES = 256;
    static constexpr int SPECTRUM_HALF_WIDTH = 128;
    
    // Signal strength thresholds (dB)
    static constexpr int8_t MIN_SIGNAL_DB = -60;
    static constexpr int8_t CARRIER_THRESHOLD_DB = 20;
    static constexpr int8_t MIN_CARRIER_DB = -50;
    
    // Peak detection indices
    static constexpr int VIDEO_PEAK_THRESHOLD_IDX = 64;
    static constexpr int AUDIO_PEAK_START_IDX = 192;
    
    // Bandwidth constraints (samples)
    static constexpr int MIN_BANDWIDTH_SAMPLES = 180;
    static constexpr int MAX_BANDWIDTH_SAMPLES = 240;
    
    // Carrier spacing constraints (samples)
    static constexpr int MIN_CARRIER_SPACING = 160;
    static constexpr int MAX_CARRIER_SPACING = 200;
    
    // Asymmetry threshold
    static constexpr int MIN_ASYMMETRY = 500;
    
    // PAL/NTSC discrimination threshold
    static constexpr int PAL_NTSC_THRESHOLD_IDX = 30;
    
    // Modulation type strings
    static constexpr const char* MODULATION_PAL = "PAL";
    static constexpr const char* MODULATION_NTSC = "NTSC";
    static constexpr size_t MODULATION_TYPE_MAX_LEN = 7;
}

// Type alias for modulation type
using ModulationType = std::array<char, DetectorConstants::MODULATION_TYPE_MAX_LEN + 1>;

class TVSignalDetector {
public:
    struct DetectionResult {
        bool is_tv_signal;
        int64_t frequency;
        int signal_strength;
        ModulationType modulation_type{};
        int video_carrier_offset;
        int audio_carrier_offset;

        DetectionResult() : is_tv_signal(false), frequency(0), signal_strength(0),
                           video_carrier_offset(0), audio_carrier_offset(0) {
            modulation_type.fill('\0');
        }
    };

    TVSignalDetector();

    DetectionResult detect_tv_signal(const ChannelSpectrum& spectrum, int64_t current_frequency);

private:
    // No private members needed - all constants are in namespace
};

} // namespace ui::external_app::analogtv
