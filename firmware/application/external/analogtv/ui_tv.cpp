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

#include "ui_tv.hpp"

#include "spectrum_color_lut.hpp"

#include "portapack.hpp"
using namespace portapack;

#include "baseband_api.hpp"

#include "string_format.hpp"

#include <cmath>
#include <array>

namespace ui::external_app::analogtv {
namespace tv {

/* TimeScopeView******************************************************/

TimeScopeView::TimeScopeView(
    const Rect parent_rect)
    : View{parent_rect} {
    set_focusable(true);

    add_children({//&labels,
                  //&field_frequency,
                  &waveform});

    /*field_frequency.on_change = [this](int32_t) {
                set_dirty();
        };
        field_frequency.set_value(10);*/
}

void TimeScopeView::paint(Painter& painter) {
    const auto r = screen_rect();

    painter.fill_rectangle(r, Color::black());

    // Cursor
    /*
        const Rect r_cursor {
                field_frequency.value() / (48000 / 240), r.bottom() - 32 - cursor_band_height,
                1, cursor_band_height
        };
        painter.fill_rectangle(
                r_cursor,
                Color::red()
        );*/
}

void TimeScopeView::on_audio_spectrum(const AudioSpectrum* spectrum) {
    for (size_t i = 0; i < spectrum->db.size(); i++)
        audio_spectrum[i] = ((int16_t)spectrum->db[i] - 127) * 256;
    waveform.set_dirty();
}

/* TVView *********************************************************/

void TVView::on_show() {
    clear();

    const auto screen_r = screen_rect();
    display.scroll_set_area(screen_r.top(), screen_r.bottom());
}

void TVView::on_hide() {
    /* TODO: Clear region to eliminate brief flash of content at un-shifted
     * position?
     */
    display.scroll_disable();
}

void TVView::paint(Painter& painter) {
    // Do nothing.
    (void)painter;
}

void TVView::on_adjust_xcorr(uint8_t xcorr) {
    x_correction = xcorr;
}

void TVView::on_adjust_contrast(int contrast) {
    this->contrast = contrast;
}

void TVView::on_channel_spectrum(
    const ChannelSpectrum& spectrum) {
    // Добавляем константы для гистерезиса синхронизации
    static constexpr int SYNC_SEARCH_WINDOW = 20; // Искать пик в пределах +/- 20 отсчетов

    // Поиск синхроимпульса (максимальное значение в spectrum.db, т.к. негативная модуляция)
    // Мы ищем локальный максимум, чтобы выровнять phase/offset

    // ПРИМЕЧАНИЕ: Это упрощенная логика. В идеале нужно скользящее окно.
    // Здесь мы просто наполняем буфер, а анализ делаем перед отрисовкой.

    for (size_t i = 0; i < 256; i++) {
        // Сохраняем "сырые" данные для постобработки
        raw_buffer[i + count * 256] = spectrum.db[i];
        // update min/max for AGC
        min_lvl = std::min(min_lvl, spectrum.db[i]);
        max_lvl = std::max(max_lvl, spectrum.db[i]);
    }

    // find max_index in current spectrum
    size_t max_index = 0;
    uint8_t max_val = spectrum.db[0];
    for (size_t i = 1; i < 256; i++) {
        if (spectrum.db[i] > max_val) {
            max_val = spectrum.db[i];
            max_index = i;
        }
    }
    max_indices[count] = (int8_t)max_index;

    count = count + 1;
    if (count == 52) {
        count = 0;
        // compute average max
        int32_t sum = 0;
        for (size_t i = 0; i < 52; i++) sum += max_indices[i];
        int16_t average_max = sum / 52;
        int16_t desired_offset = -average_max;

        // smooth the offset
        dynamic_offset = (dynamic_offset * 15 + desired_offset) / 16;

        // compute AGC factors
        if (max_lvl > min_lvl) {
            gain = 255.0f / (max_lvl - min_lvl);
            offset = (float)min_lvl;
        } else {
            gain = 1.0f;
            offset = 0.0f;
        }
        // reset for next frame
        min_lvl = 255;
        max_lvl = 0;

        // fill video_buffer_int with corrected data with AGC and contrast
        for (size_t j = 0; j < 13312; j++) {
            int32_t idx = (int32_t)j + dynamic_offset;
            if (idx < 0) idx += 13312;
            if (idx >= 13312) idx -= 13312;
            uint8_t raw = raw_buffer[idx];
            float scaled = (raw - offset) * gain * (contrast / 128.0f);
            int16_t pixel_val = (int16_t)(scaled + 0.5f);
            pixel_val = std::max<int>(0, std::min<int>(255, pixel_val));
            video_buffer_int[j] = 255 - pixel_val;
        }

        ui::Color line_buffer[128];
        Coord line;
        uint32_t bmp_px;

        // Оптимизация: цвет вычисляется один раз на пару линий
        for (line = 0; line < 208; line = line + 2) {
            for (bmp_px = 0; bmp_px < 128; bmp_px++) {
                // line_buffer[bmp_px] = spectrum_rgb4_lut[video_buffer_int[bmp_px + line/2 * 128]];
                line_buffer[bmp_px] = spectrum_rgb4_lut[video_buffer_int[bmp_px + line / 2 * 128 + x_correction]];
            }

            display.render_line({0, line + 100}, 128, line_buffer);
            display.render_line({0, line + 101}, 128, line_buffer);
        }
        count = 0;
    }
}

void TVView::clear() {
    display.fill_rectangle(
        screen_rect(),
        Color::black());
}

/* TVWidget *******************************************************/

TVWidget::TVWidget() {
    add_children({&tv_view,
                  &field_contrast});
    field_contrast.set_value(128);
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

    tv_normal_rect = {0, scale_height, new_parent_rect.width(), new_parent_rect.height() - scale_height};
    tv_reduced_rect = {0, audio_spectrum_height + scale_height, new_parent_rect.width(), new_parent_rect.height() - scale_height - audio_spectrum_height};

    update_widgets_rect();
}

void TVWidget::paint(Painter& painter) {
    // TODO:
    (void)painter;
}

void TVWidget::on_channel_spectrum(const ChannelSpectrum& spectrum) {
    tv_view.on_channel_spectrum(spectrum);
    tv_view.on_adjust_contrast(field_contrast.value());
    sampling_rate = spectrum.sampling_rate;
}

void TVWidget::on_audio_spectrum() {
    audio_spectrum_view->on_audio_spectrum(audio_spectrum_data);
}

} /* namespace tv */
}  // namespace ui::external_app::analogtv
