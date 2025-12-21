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

void TVView::set_x_correction(int32_t value) {
    x_correction_ = value;
}


void TVView::on_channel_spectrum(const ChannelSpectrum& spectrum) {
    add_line_to_buffer(spectrum, 0);    // Первая строка (сэмплы 0..127)
    add_line_to_buffer(spectrum, 128);  // Вторая строка (сэмплы 128..255)
    
    // Проверяем, нужно ли рендерить
    if (buffer_line_count >= RENDER_THRESHOLD) {
        render_buffer_batch();
    }
}

void TVView::add_line_to_buffer(const ChannelSpectrum& spectrum, int offset_idx) {
    const auto rect = screen_rect();
    const int max_y = rect.height();

    // Проверка переполнения буфера
    if (buffer_line_count >= LINE_BUFFER_SIZE) {
        process_buffer_overflow();
        return;
    }

    // Генерируем строку из TV_LINE_WIDTH пикселей (оптимизированная ширина)
    for (int i = 0; i < TV_LINE_WIDTH; i++) {
        int source_idx = offset_idx + i + x_correction_;
        // Оптимизированная проверка границ
        if (source_idx < 0) source_idx = 0;
        else if (source_idx > 255) source_idx = 255;

        uint8_t db_val = 255 - spectrum.db[source_idx];
        line_buffer_[buffer_line_count][i] = spectrum_rgb4_lut[db_val];
    }
    
    buffer_line_count++;
}

void TVView::render_buffer_batch() {
    const auto rect = screen_rect();
    const int max_y = rect.height();

    // Проверяем, не выйдем ли за пределы экрана
    if (scan_line + buffer_line_count >= max_y) {
        scan_line = 0;
    }

    // Рендерим все строки из буфера (оптимизированная ширина)
    for (int i = 0; i < buffer_line_count; i++) {
        display.render_line({rect.left(), rect.top() + scan_line + i},
                           TV_LINE_WIDTH, line_buffer_[i].data());
    }

    // Обновляем позицию сканирования
    scan_line += buffer_line_count;
    
    // Очищаем буфер
    buffer_line_count = 0;
}

void TVView::process_buffer_overflow() {
    // При переполнении буфера - рендерим текущий буфер и начинаем с новой позиции
    render_buffer_batch();
    // Сброс позиции сканирования для избежания артефактов
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
    sampling_rate = spectrum.sampling_rate;
    
    // Детекция TV сигнала
    auto detection_result = signal_detector.detect_tv_signal(spectrum, receiver_model.target_frequency());
    
    if (detection_result.is_tv_signal && on_tv_signal_detected) {
        on_tv_signal_detected(detection_result);
    }
}

void TVWidget::on_audio_spectrum() {
    // Обрабатываем аудио спектр только если он активен
    if (audio_spectrum_view && audio_spectrum_data) {
        audio_spectrum_view->on_audio_spectrum(audio_spectrum_data);
    }
}

} /* namespace tv */
}  // namespace ui::external_app::analogtv
