/*
 * Copyright (C) 2015 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2018 Furrtek
 * Copyright (C) 2023 gullradriel, Nilorea Studio Inc.
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

#include "ui_detector_app.hpp"

#include "baseband_api.hpp"
#include "string_format.hpp"

using namespace portapack;

namespace ui {

const std::vector<std::vector<rf::Frequency>> preset_frequencies = {
    // Remotes
    {315000000, 433920000},
    // LoRa
    {433375000, 434125000, 434875000, 867375000, 868125000, 868875000, 914250000, 915000000, 915750000},
    // Tetra
    {380375000, 381125000, 381875000, 382625000, 383375000, 384125000, 384875000, 385625000, 386375000, 387125000, 387875000, 388625000, 389375000, 390125000}
};

void DetectorView::update_display() {
    big_display.set("FREQ:" + to_string_short_freq(frequencies[current_index]) + " MHz");
    freq_stats.set("RSSI: " + to_string_dec_int(rssi.get_min()) + "/" + to_string_dec_int(rssi.get_avg()) + "/" + to_string_dec_int(rssi.get_max()) + " db");
}

void DetectorView::tune_next() {
    if (frequencies.empty()) return;
    current_index = (current_index + 1) % frequencies.size();
    receiver_model.set_target_frequency(frequencies[current_index]);
    update_display();
}

void DetectorView::play_beep(int db) {
    uint32_t freq = map(db, -100, 20, 400, 2600);
    baseband::request_audio_beep(freq, 24000, 150);
}

int32_t DetectorView::map(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh) {
    return toLow + (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
}

DetectorView::DetectorView(NavigationView& nav)
    : nav_(nav) {
    baseband::run_image(portapack::spi_flash::image_tag_am_audio);
    receiver_model.set_modulation(ReceiverModel::Mode::AMAudio);
    receiver_model.set_am_configuration(0); // DSB 9k
    receiver_model.enable();
    audio::output::start();

    add_children({&labels,
                  &field_lna,
                  &field_vga,
                  &field_rf_amp,
                  &field_volume,
                  &field_preset,
                  &field_threshold,
                  &rssi,
                  &big_display,
                  &freq_stats});

    field_preset.on_change = [this](size_t, OptionsField::value_t v) {
        frequencies = preset_frequencies[v];
        current_index = 0;
        if (!frequencies.empty()) {
            receiver_model.set_target_frequency(frequencies[0]);
        }
        update_display();
    };

    field_threshold.on_change = [this](int32_t v) {
        beep_squelch = v;
    };

    // Set initial preset to Remotes
    field_preset.set_selected_index(0);
    frequencies = preset_frequencies[0];
    if (!frequencies.empty()) {
        receiver_model.set_target_frequency(frequencies[0]);
    }
    update_display();

    rssi.set_peak(true, 500);
}

DetectorView::~DetectorView() {
    audio::output::stop();
    receiver_model.disable();
    baseband::shutdown();
}

void DetectorView::focus() {
    field_preset.focus();
}

void DetectorView::on_statistics_update(const ChannelStatistics& statistics) {
    if (!running || frequencies.empty()) return;

    int db = statistics.max_db;
    if (db > beep_squelch) {
        play_beep(db);
    }

    // Tune to next frequency periodically
    if (chTimeNow() - last_tune > 100) { // every 100ms
        tune_next();
        last_tune = chTimeNow();
    }

    update_display();
}

} /* namespace ui */
