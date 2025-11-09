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

#ifndef _UI_DETECTOR_APP
#define _UI_DETECTOR_APP

#include "ui.hpp"
#include "receiver_model.hpp"
#include "ui_receiver.hpp"
#include "audio.hpp"
#include "baseband_api.hpp"
#include "string_format.hpp"

#include <string>
#include <memory>
#include <vector>

namespace ui {

class DetectorView : public View {
   public:
    DetectorView(NavigationView& nav);
    ~DetectorView();

    void focus() override;

    std::string title() const override { return "Detector"; };

   private:
    NavigationView& nav_;

    void update_display();
    void tune_next();
    void play_beep(int db);
    int32_t map(int32_t value, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh);

    std::vector<rf::Frequency> frequencies;
    size_t current_index = 0;
    int beep_squelch = -50; // db
    bool running = true;
    uint32_t last_tune = 0;

    Labels labels{
        {{UI_POS_X(0), UI_POS_Y(0)}, "LNA:   VGA:   AMP:  ", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X_RIGHT(6), UI_POS_Y(0)}, "VOL:  ", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(0), UI_POS_Y(2)}, "Preset: ", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(0), UI_POS_Y(4)}, "Beep Squelch: ", Theme::getInstance()->fg_light->foreground}};

    LNAGainField field_lna{
        {4 * 8, UI_POS_Y(0)}};

    VGAGainField field_vga{
        {11 * 8, UI_POS_Y(0)}};

    RFAmpField field_rf_amp{
        {18 * 8, UI_POS_Y(0)}};

    AudioVolumeField field_volume{
        {UI_POS_X_RIGHT(2), UI_POS_Y(0)}};

    OptionsField field_preset{
        {UI_POS_X(0), UI_POS_Y(2)},
        10,
        {{"Remotes", 0},
         {"LoRa", 1},
         {"Tetra", 2}}};

    NumberField field_threshold{
        {UI_POS_X(0), UI_POS_Y(4)},
        3,
        {-90, 20},
        1,
        ' '};

    RSSI rssi{
        {0 * 16, 6 * 16 + 2, screen_width - 8 * 8 + 4, 12}};

    Text big_display{
        {0, 8 * 16, 21 * 8, 16}};

    Text freq_stats{
        {0, 9 * 16, 21 * 8, 16}};

    MessageHandlerRegistration message_handler_stats{
        Message::ID::ChannelStatistics,
        [this](const Message* const p) {
            on_statistics_update(static_cast<const ChannelStatisticsMessage*>(p)->statistics);
        }};

    void on_statistics_update(const ChannelStatistics& statistics);
};

} /* namespace ui */

#endif
