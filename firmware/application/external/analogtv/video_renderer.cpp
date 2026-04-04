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
#include "spectrum_color_lut.hpp"
#include "portapack.hpp"

using namespace portapack;

namespace ui::external_app::analogtv {

bool VideoRenderer::process_spectrum(const ChannelSpectrum& spectrum) {
    if (callback_count_ >= CALLBACKS_PER_FRAME) {
        return false;
    }

    const size_t offset = callback_count_ * SPECTRUM_BINS;
    for (size_t i = 0; i < SPECTRUM_BINS; i++) {
        video_buffer_[offset + i] = spectrum_rgb4_lut[255 - spectrum.db[i]];
    }

    callback_count_++;

    if (callback_count_ >= CALLBACKS_PER_FRAME) {
        render_frame();
        callback_count_ = 0;
        return true;
    }

    return false;
}

void VideoRenderer::render_source_line(uint16_t source_line) {
    Color line_buffer[VIDEO_WIDTH];
    const size_t line_offset = source_line * VIDEO_WIDTH;
    Coord display_y = static_cast<Coord>(source_line * 2 + 100);

    for (size_t x = 0; x < VIDEO_WIDTH; x++) {
        line_buffer[x] = video_buffer_[line_offset + x + x_correction_];
    }

    display.render_line({0, display_y}, VIDEO_WIDTH, line_buffer);
    display.render_line({0, static_cast<Coord>(display_y + 1)}, VIDEO_WIDTH, line_buffer);
}

void VideoRenderer::render_frame() {
    for (uint16_t line = 0; line < SOURCE_LINES; line++) {
        render_source_line(line);
    }
}

void VideoRenderer::clear_buffer() {
    for (size_t i = 0; i < sizeof(video_buffer_) / sizeof(video_buffer_[0]); i++) {
        video_buffer_[i] = Color::black();
    }
    callback_count_ = 0;
}

void VideoRenderer::set_x_correction(uint8_t correction) {
    x_correction_ = (correction > X_CORRECTION_MAX) ? X_CORRECTION_MAX : correction;
}

}  // namespace ui::external_app::analogtv
