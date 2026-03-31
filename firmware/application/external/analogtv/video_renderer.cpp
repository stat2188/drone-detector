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

#include "video_renderer.hpp"
#include "portapack.hpp"

#include <cstring>

using namespace portapack;

namespace ui::external_app::analogtv {

// ============================================================================
// Pre-Computed LUTs (compile-time, ~1.3 KB flash total)
//
// These constexpr functions return std::array by value (trivial copy).
// The compiler evaluates them entirely at compile time, placing the
// results in .rodata (flash). Zero runtime cost, zero RAM cost.
// ============================================================================

/**
 * @brief Horizontal scale-index LUT (240 bytes flash)
 *
 * horizontal_index_lut[x] = source pixel column for display column x.
 * Eliminates: (x * 128) / 240 per pixel (1 multiply + 1 divide each).
 */
static constexpr auto generate_h_lut() {
    std::array<uint8_t, DISPLAY_WIDTH> lut{};
    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
        lut[x] = static_cast<uint8_t>((x * VIDEO_WIDTH) / DISPLAY_WIDTH);
    }
    return lut;
}

/**
 * @brief Vertical row-offset LUT (640 bytes flash)
 *
 * vertical_offset_lut[y] = byte offset into video_buffer_ for display row y.
 * Pre-combines: source_row * VIDEO_WIDTH + x_correction.
 * Eliminates: (y * 104) / 320 * 128 + 10 per line.
 */
static constexpr auto generate_v_lut() {
    std::array<uint16_t, DISPLAY_HEIGHT> lut{};
    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
        const uint16_t src_y = (y * 104) / DISPLAY_HEIGHT;
        lut[y] = static_cast<uint16_t>(src_y * VIDEO_WIDTH + DEFAULT_X_CORRECTION);
    }
    return lut;
}

/**
 * @brief Gamma-corrected intensity LUT (768 bytes flash)
 *
 * Maps raw spectrum byte directly to RGB565 color with CRT gamma emulation.
 * Combines: inversion + gamma correction + color packing in one lookup.
 *
 * Gamma 1/2.2 is the standard CRT transfer function.
 * The gamma_table[] below is pre-computed at build time.
 */
static constexpr auto generate_gamma_lut() {
    // Pre-computed: round(pow(i/255, 1/2.2) * 255) for i = 0..255
    // Computed by Python at build design time, not at runtime.
    constexpr uint8_t gamma_table[256] = {
          0,  21,  28,  34,  39,  43,  46,  50,  53,  56,  59,  61,  64,  66,  68,  70,
         72,  74,  76,  78,  80,  82,  84,  85,  87,  89,  90,  92,  93,  95,  96,  98,
         99, 101, 102, 103, 105, 106, 107, 109, 110, 111, 112, 114, 115, 116, 117, 118,
        119, 120, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
        136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145, 146, 147, 148, 149, 150,
        151, 151, 152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 160, 161, 162, 163,
        164, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175,
        175, 176, 177, 178, 178, 179, 180, 180, 181, 182, 182, 183, 184, 184, 185, 186,
        186, 187, 188, 188, 189, 190, 190, 191, 192, 192, 193, 194, 194, 195, 195, 196,
        197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203, 203, 204, 205, 205, 206,
        206, 207, 207, 208, 209, 209, 210, 210, 211, 212, 212, 213, 213, 214, 214, 215,
        215, 216, 217, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 223, 223, 224,
        224, 225, 225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232,
        232, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 239, 240,
        240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246, 246, 247, 247, 248,
        248, 249, 249, 249, 250, 250, 251, 251, 252, 252, 253, 253, 254, 254, 255, 255,
    };

    std::array<ui::Color, SPECTRUM_COLOR_LUT_SIZE> lut{};
    for (uint16_t i = 0; i < SPECTRUM_COLOR_LUT_SIZE; i++) {
        const auto v = gamma_table[i];
        lut[i] = ui::Color{v, v, v};
    }
    return lut;
}

// ============================================================================
// Static Member Definitions
//
// These std::array objects are initialized entirely at compile time.
// The constexpr generator functions return std::array by value (legal in C++17).
// Final storage: .rodata section (flash), zero RAM cost.
// ============================================================================

const std::array<uint8_t, DISPLAY_WIDTH>
    VideoRenderer::horizontal_index_lut_ = generate_h_lut();

const std::array<uint16_t, DISPLAY_HEIGHT>
    VideoRenderer::vertical_offset_lut_ = generate_v_lut();

const std::array<ui::Color, SPECTRUM_COLOR_LUT_SIZE>
    VideoRenderer::intensity_lut_ = generate_gamma_lut();

// ============================================================================
// VideoRenderer Implementation
// ============================================================================

void VideoRenderer::process_spectrum(const ChannelSpectrum& spectrum) {
    if (callback_count_ >= CALLBACKS_PER_FRAME) {
        return;
    }

    const size_t offset = static_cast<size_t>(callback_count_) * SPECTRUM_BINS;
    for (size_t i = 0; i < SPECTRUM_BINS; i++) {
        video_buffer_[offset + i] = spectrum.db[i];
    }

    callback_count_++;
    if (callback_count_ >= CALLBACKS_PER_FRAME) {
        frame_ready_ = true;
        callback_count_ = 0;
    }
}

void VideoRenderer::render_line(uint16_t y, ui::Color* line_buffer) const {
    if (!line_buffer || y >= DISPLAY_HEIGHT) {
        return;
    }

    // LUT-optimized inner loop:
    //   1. vertical_offset_lut_[y]     -> base pointer (zero arithmetic)
    //   2. horizontal_index_lut_[x]    -> source column (zero arithmetic)
    //   3. intensity_lut_[...]         -> final RGB565 color (zero arithmetic)
    //
    // Per pixel: 2 array lookups + 1 pointer dereference + 1 store.
    // Total arithmetic in hot loop: NONE.

    const uint8_t* src_row = video_buffer_ + vertical_offset_lut_[y];

    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
        line_buffer[x] = intensity_lut_[src_row[horizontal_index_lut_[x]]];
    }
}

void VideoRenderer::render_frame() {
    // Single line buffer — reused per row.
    // 480 bytes stack. Well within 4KB budget (11.7%).
    ui::Color line_buffer[DISPLAY_WIDTH];

    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
        render_line(y, line_buffer);
        display.render_line(
            {0, static_cast<Coord>(y)},
            DISPLAY_WIDTH,
            line_buffer);
    }

    frame_ready_ = false;
}

void VideoRenderer::clear_buffer() {
    std::memset(video_buffer_, 0, BUFFER_SIZE);
    callback_count_ = 0;
    frame_ready_ = false;
}

void VideoRenderer::set_x_correction(uint8_t correction) {
    x_correction_ = (correction > MAX_X_CORRECTION) ? MAX_X_CORRECTION : correction;
}

bool VideoRenderer::detect_video_carrier(
    const std::array<uint8_t, 256>& spectrum_data) const {

    uint8_t consecutive_bins = 0;
    bool in_carrier = false;

    for (size_t i = 0; i < SPECTRUM_BINS; i++) {
        if (is_carrier_value(spectrum_data[i])) {
            consecutive_bins++;
            in_carrier = true;
        } else if (in_carrier) {
            if (consecutive_bins >= 3 && consecutive_bins <= 10) {
                return true;
            }
            consecutive_bins = 0;
            in_carrier = false;
        }
    }

    if (in_carrier && consecutive_bins >= 3 && consecutive_bins <= 10) {
        return true;
    }

    return false;
}

bool VideoRenderer::is_carrier_value(uint8_t value) const {
    return static_cast<int32_t>(value) - 120 >= CARRIER_THRESHOLD_DBM;
}

}  // namespace ui::external_app::analogtv
