/*
 * Copyright (C) 2015 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2020 Shao
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

// "Well, so what if it looks like shit. Does it work? Yeah, it works!.Are you seriously planning to watch Netflix on this piece of junk?"
// 
// Signed-off-by: syber

#include "ui_tv.hpp"

#include "spectrum_color_lut.hpp"

#include "portapack.hpp"
using namespace portapack;

#include "baseband_api.hpp"

#include "string_format.hpp"

#include <cmath>
#include <array>
#include <algorithm>

namespace ui::external_app::analogtv {
namespace tv {

// Compile-time X-offset table generation - saves 16.5KB of RAM by placing in ROM
constexpr std::array<std::array<int8_t, 128>, 129> generate_x_offset_table() {
    std::array<std::array<int8_t, 128>, 129> table{};
    
    for (int corr = 0; corr < 129; corr++) {
        for (int i = 0; i < 128; i++) {
            int idx = i + corr;
            // Clamp to valid range [0, 255]
            idx = std::clamp(idx, 0, 255);
            table[corr][i] = static_cast<int8_t>(idx);
        }
    }
    
    return table;
}

// Constexpr table - placed in ROM by compiler
constexpr auto X_OFFSET_TABLE = generate_x_offset_table();

/* TimeScopeView******************************************************/

TimeScopeView::TimeScopeView(
    const Rect parent_rect)
    : View{parent_rect} {
    set_focusable(true);

    add_children({&waveform});
}

void TimeScopeView::paint(Painter& painter) {
    const auto r = screen_rect();
    painter.fill_rectangle(r, Color::black());
}

void TimeScopeView::on_audio_spectrum(const AudioSpectrum* spectrum) {
    for (size_t i = 0; i < spectrum->db.size(); i++) {
        audio_spectrum[i] = ((int16_t)spectrum->db[i] - 127) * 256;
    }
    waveform.set_dirty();
}

/* TVView *********************************************************/

void TVView::on_show() {
    clear();

    const auto screen_r = screen_rect();
    display.scroll_set_area(screen_r.top(), screen_r.bottom());
}

void TVView::on_hide() {
    display.scroll_disable();
}

void TVView::paint(Painter& painter) {
    (void)painter;
}

void TVView::set_x_correction(int32_t value) {
    x_correction_ = value;
}

void TVView::on_channel_spectrum(const ChannelSpectrum& spectrum) {
    add_line_to_buffer(spectrum, 0);    // First line (samples 0..127)
    add_line_to_buffer(spectrum, 128);  // Second line (samples 128..255)
    
    // Check if we need to render
    if (buffer_line_count >= RenderConstants::RENDER_THRESHOLD) {
        render_buffer_batch();
    }
}

void TVView::add_line_to_buffer(const ChannelSpectrum& spectrum, int offset_idx) {
    (void)offset_idx;
    if (buffer_line_count >= RenderConstants::LINE_BUFFER_SIZE) {
        process_buffer_overflow();
        return;
    }

    const auto* db = spectrum.db.data();
    // Access ROM table with clamped correction value
    const int corr_idx = std::clamp(x_correction_ + 64, 0, 128);
    const int8_t* offset_row = X_OFFSET_TABLE[corr_idx].data();
    const auto* lut = spectrum_rgb4_lut.data();

    // Hot path: minimal logic, direct memory access
    for (int i = 0; i < RenderConstants::TV_LINE_WIDTH; i++) {
        const uint8_t db_val = 255 - db[offset_row[i]];
        line_buffer_[buffer_line_count][i] = lut[db_val];
    }

    buffer_line_count++;
}

void TVView::render_buffer_batch() {
    const auto rect = screen_rect();

    if (scan_line + buffer_line_count >= rect.height()) {
        scan_line = 0;
    }

    for (int i = 0; i < buffer_line_count; i++) {
        display.render_line({rect.left(), rect.top() + scan_line + i},
                           RenderConstants::TV_LINE_WIDTH, line_buffer_[i].data());
    }

    scan_line += buffer_line_count;

    buffer_line_count = 0;
}

void TVView::process_buffer_overflow() {
    render_buffer_batch();
    scan_line = 0;
}

void TVView::clear() {
    display.fill_rectangle(
        screen_rect(),
        Color::black());
}

/* TVWidget *******************************************************/

TVWidget::TVWidget() {
    add_children({&tv_view,
                  &field_xcorr});
    
    field_xcorr.on_change = [this](int32_t value) {
        tv_view.set_x_correction(value);
    };
    field_xcorr.set_value(0);
}

void TVWidget::on_show() {
    baseband::spectrum_streaming_start();
}

void TVWidget::on_hide() {
    baseband::spectrum_streaming_stop();
}

void TVWidget::show_audio_spectrum_view(const bool show) {
    if ((audio_spectrum_view && show) || (!audio_spectrum_view && !show)) return;

    if (show) {
        audio_spectrum_view = std::make_unique<TimeScopeView>(audio_spectrum_view_rect);
        add_child(audio_spectrum_view.get());
        update_widgets_rect();
    } else {
        audio_spectrum_update = false;
        remove_child(audio_spectrum_view.get());
        audio_spectrum_view.reset();
        update_widgets_rect();
    }
}

void TVWidget::update_widgets_rect() {
    if (audio_spectrum_view) {
        tv_view.set_parent_rect(tv_reduced_rect);
    } else {
        tv_view.set_parent_rect(tv_normal_rect);
    }
    tv_view.on_show();
}

void TVWidget::set_parent_rect(const Rect new_parent_rect) {
    View::set_parent_rect(new_parent_rect);

    tv_normal_rect = {0, RenderConstants::SCALE_HEIGHT, new_parent_rect.width(), new_parent_rect.height() - RenderConstants::SCALE_HEIGHT};
    tv_reduced_rect = {0, RenderConstants::AUDIO_SPECTRUM_HEIGHT + RenderConstants::SCALE_HEIGHT, new_parent_rect.width(), new_parent_rect.height() - RenderConstants::SCALE_HEIGHT - RenderConstants::AUDIO_SPECTRUM_HEIGHT};

    update_widgets_rect();
}

void TVWidget::paint(Painter& painter) {
    (void)painter;
}

void TVWidget::on_channel_spectrum(const ChannelSpectrum& spectrum) {
    tv_view.on_channel_spectrum(spectrum);
    sampling_rate = spectrum.sampling_rate;

    frame_counter++;
    if (frame_counter >= RenderConstants::DETECTION_SKIP_FRAMES) {
        frame_counter = 0;
        auto detection_result = signal_detector.detect_tv_signal(spectrum, receiver_model.target_frequency());

        cached_detection = detection_result;
        has_cached_detection = true;

        if (detection_result.is_tv_signal && on_tv_signal_detected) {
            on_tv_signal_detected(detection_result);
        }
    }
}

void TVWidget::on_audio_spectrum() {
    // Process audio spectrum only if active
    if (audio_spectrum_view && audio_spectrum_data) {
        audio_spectrum_view->on_audio_spectrum(audio_spectrum_data);
    }
}

} /* namespace tv */
}  // namespace ui::external_app::analogtv
