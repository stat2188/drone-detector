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

namespace ui::external_app::analogtv {

class TVSignalDetector {
public:
    struct DetectionResult {
        bool is_tv_signal;
        int64_t frequency;
        int signal_strength;
        std::string modulation_type;
        int video_carrier_offset;
        int audio_carrier_offset;
        
        DetectionResult() : is_tv_signal(false), frequency(0), signal_strength(0), 
                           modulation_type("UNKNOWN"), video_carrier_offset(0), audio_carrier_offset(0) {}
    };

    TVSignalDetector();
    
    DetectionResult detect_tv_signal(const ChannelSpectrum& spectrum, int64_t current_frequency);
    
private:
    static constexpr int TV_CHANNEL_WIDTH_SAMPLES = 256;
    static constexpr int MIN_SIGNAL_DB = -60;
    static constexpr int CARRIER_THRESHOLD = 20;
    static constexpr int MIN_CARRIER_DB = -50;
    
    // TV стандарты
    static constexpr int PAL_VIDEO_OFFSET = 16;      // ~1.25 МГц
    static constexpr int PAL_AUDIO_OFFSET = 192;     // ~4.5 МГц
    static constexpr int NTSC_VIDEO_OFFSET = 20;     // ~1.25 МГц  
    static constexpr int NTSC_AUDIO_OFFSET = 200;    // ~4.5 МГц
    
    bool check_signal_strength(const ChannelSpectrum& spectrum);
    bool find_carriers(const ChannelSpectrum& spectrum, int& video_offset, int& audio_offset);
    bool validate_carrier_spacing(int video_offset, int audio_offset);
    bool check_spectrum_shape(const ChannelSpectrum& spectrum);
    std::string determine_modulation_type(const ChannelSpectrum& spectrum);
    int find_peak_in_range(const ChannelSpectrum& spectrum, int start_idx, int end_idx);
    int calculate_signal_bandwidth(const ChannelSpectrum& spectrum);
};

} // namespace ui::external_app::analogtv
