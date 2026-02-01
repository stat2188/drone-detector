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

namespace ui::external_app::analogtv {

TVSignalDetector::TVSignalDetector() {
}

TVSignalDetector::DetectionResult TVSignalDetector::detect_tv_signal(
    const ChannelSpectrum& spectrum,
    int64_t current_frequency) {

    DetectionResult result;
    result.frequency = current_frequency;

    const auto* db = spectrum.db.data();

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

    for (int i = 0; i < 256; i++) {
        int v = db[i];

        if (v > max_db) {
            max_db = v;
        }

        avg_db += v;

        if (i < 128) {
            left_power += v;
        } else {
            right_power += v;
        }

        if (i < 64 && v > video_peak_val) {
            video_peak_val = v;
            video_peak_idx = i;
        }

        if (i >= 192 && v > audio_peak_val) {
            audio_peak_val = v;
            audio_peak_idx = i;
        }

        if (first_edge == -1 && v > MIN_SIGNAL_DB) {
            first_edge = i;
        }
        if (v > MIN_SIGNAL_DB) {
            last_edge = i;
        }
    }

    avg_db >>= 8;
    result.signal_strength = max_db;

    int signal_to_noise = max_db - avg_db;
    if (max_db < MIN_SIGNAL_DB || signal_to_noise < CARRIER_THRESHOLD) {
        return result;
    }

    if (video_peak_idx == -1 || video_peak_val < MIN_CARRIER_DB) {
        return result;
    }

    if (audio_peak_idx == -1 || audio_peak_val < MIN_CARRIER_DB) {
        return result;
    }

    if (video_peak_idx >= audio_peak_idx) {
        return result;
    }

    int spacing = audio_peak_idx - video_peak_idx;
    if (spacing < 160 || spacing > 200) {
        return result;
    }

    if (first_edge == -1 || last_edge == -1) {
        return result;
    }

    int bandwidth = last_edge - first_edge;
    if (bandwidth < 180 || bandwidth > 240) {
        return result;
    }

    int asymmetry = left_power - right_power;
    if (asymmetry < 0) {
        asymmetry = -asymmetry;
    }
    if (asymmetry < 500) {
        return result;
    }

    if (video_peak_idx < 30) {
        strncpy(result.modulation_type, "PAL", 7);
    } else {
        strncpy(result.modulation_type, "NTSC", 7);
    }

    result.video_carrier_offset = video_peak_idx;
    result.audio_carrier_offset = audio_peak_idx;
    result.is_tv_signal = true;

    return result;
}

} // namespace ui::external_app::analogtv
