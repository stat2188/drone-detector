/*
 * Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2018 Furrtek
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

#ifndef __ANALOG_TV_APP_H__
#define __ANALOG_TV_APP_H__

#include "receiver_model.hpp"

#include "ui_receiver.hpp"
#include "ui_freq_field.hpp"
#include "ui_tv.hpp"
#include "ui_record_view.hpp"
#include "app_settings.hpp"
#include "radio_state.hpp"

#include "tone_key.hpp"
#include "tv_signal_detector.hpp"

#include <vector>
#include <string>

namespace ui::external_app::analogtv {

class AnalogTvView : public View {
   public:
    AnalogTvView(NavigationView& nav);
    ~AnalogTvView();

    void on_hide() override;

    void set_parent_rect(const Rect new_parent_rect) override;

    void focus() override;

    std::string title() const override { return "AnalogTV RX"; };

   private:
    static constexpr ui::Dim header_height = 3 * 16;

    NavigationView& nav_;
    RxRadioState radio_state_{};
    app_settings::SettingsManager settings_{
        "rx_tv", app_settings::Mode::RX};

    const Rect options_view_rect{UI_POS_X(0), 1 * 16, screen_width, 1 * 16};
    const Rect nbfm_view_rect{UI_POS_X(0), 1 * 16, 18 * 8, 1 * 16};

    RSSI rssi{
        {21 * 8, 0, 6 * 8, 4}};

    Channel channel{
        {21 * 8, 5, 6 * 8, 4}};

    Audio audio{
        {21 * 8, 10, 6 * 8, 4}};

    RxFrequencyField field_frequency{
        {5 * 8, UI_POS_Y(0)},
        nav_};

    LNAGainField field_lna{
        {15 * 8, UI_POS_Y(0)}};

    VGAGainField field_vga{
        {18 * 8, UI_POS_Y(0)}};

    OptionsField options_modulation{
        {UI_POS_X(0), UI_POS_Y(0)},
        4,
        {
            {"TV ", toUType(ReceiverModel::Mode::WidebandFMAudio)},
            {"TV ", toUType(ReceiverModel::Mode::WidebandFMAudio)},
            {"TV ", toUType(ReceiverModel::Mode::WidebandFMAudio)},
        }};

    AudioVolumeField field_volume{
        {27 * 8, UI_POS_Y(0)}};

    std::unique_ptr<Widget> options_widget{};

    tv::TVWidget tv{};
    
    // UI элементы для сканирования
    Button button_scan_start{ {UI_POS_X(0), UI_POS_Y(1), 8 * 8, 2 * 8}, "SCAN" };
    Button button_scan_stop{ {UI_POS_X(1), UI_POS_Y(1), 8 * 8, 2 * 8}, "STOP" };
    Button button_manual{ {UI_POS_X(2), UI_POS_Y(1), 8 * 8, 2 * 8}, "MANUAL" };
    
    Text text_scan_status{ {UI_POS_X(0), UI_POS_Y(2), 20 * 8, 1 * 8}, "Status: Ready" };
    Text text_found_channels{ {UI_POS_X(0), UI_POS_Y(3), 20 * 8, 1 * 8}, "Channels: 0" };
    Text text_current_channel{ {UI_POS_X(0), UI_POS_Y(4), 20 * 8, 1 * 8}, "Current: -" };
    Text text_progress{ {UI_POS_X(0), UI_POS_Y(5), 20 * 8, 1 * 8}, "Progress: 0%" };
    
    // Поля для настройки диапазона
    NumberField field_scan_start{ {UI_POS_X(0), UI_POS_Y(7)}, 9, {10000000, 1000000000}, 1000000, ' ' };
    NumberField field_scan_end{ {UI_POS_X(1), UI_POS_Y(7)}, 9, {10000000, 1000000000}, 1000000, ' ' };
    NumberField field_scan_step{ {UI_POS_X(0), UI_POS_Y(8)}, 6, {50000, 1000000}, 10000, ' ' };
    NumberField field_min_signal{ {UI_POS_X(1), UI_POS_Y(8)}, 4, {-100, -20}, 1, ' ' };
    NumberField field_scan_timeout{ {UI_POS_X(0), UI_POS_Y(9)}, 4, {100, 2000}, 10, ' ' };
    
    // Структуры для сканирования
    struct ScanParameters {
        int64_t start_freq = 100000000;    // 100 МГц
        int64_t end_freq = 800000000;      // 800 МГц
        int64_t step = 200000;             // 200 кГц
        int min_signal_db = -60;           // Минимальный уровень сигнала
        int scan_timeout_ms = 500;         // Таймаут на каждой частоте
    };
    
    struct FoundChannel {
        int64_t frequency{0};
        std::string name{""};
        int signal_strength{0};
        std::string modulation_type{""};
        bool is_valid{false};
        
        FoundChannel() = default;
        
        FoundChannel(int64_t freq, const std::string& mod_type, int strength, bool valid = false)
            : frequency(freq), name("TV_" + to_string_short_freq(freq)), 
              signal_strength(strength), modulation_type(mod_type), is_valid(valid) {}
    };
    
    ScanParameters scan_params{};
    std::vector<FoundChannel> found_channels{};
    volatile bool is_scanning = false;
    volatile bool scan_paused = false;
    volatile bool thread_terminate = false;
    size_t current_channel_index = 0;
    int64_t current_scan_freq = 0;
    bool view_destroying = false;

    void on_baseband_bandwidth_changed(uint32_t bandwidth_hz);
    void on_modulation_changed(const ReceiverModel::Mode modulation);
    void on_show_options_frequency();
    void on_show_options_rf_gain();
    void on_show_options_modulation();
    void on_frequency_step_changed(rf::Frequency f);
    void on_reference_ppm_correction_changed(int32_t v);

    void remove_options_widget();
    void set_options_widget(std::unique_ptr<Widget> new_widget);

    void update_modulation(const ReceiverModel::Mode modulation);

    MessageHandlerRegistration message_handler_freqchg{
        Message::ID::FreqChangeCommand,
        [this](Message* const p) {
            const auto message = static_cast<const FreqChangeCommandMessage*>(p);
            this->on_freqchg(message->freq);
        }};

    void on_freqchg(int64_t freq);
    
    // Обработка управления стиками
    void on_left();
    void on_right();
    
    void on_frequency_changed(rf::Frequency f);
    
    // Методы для сканирования
    void start_scan();
    void stop_scan();
    void pause_scan();
    void resume_scan();
    msg_t scan_worker_thread();
    void update_scan_progress();
    void add_found_channel(const TVSignalDetector::DetectionResult& result);
    void switch_to_channel(size_t index);
    void save_found_channels();
    void load_scan_settings();
    void save_scan_settings();
};

}  // namespace ui::external_app::analogtv

#endif /*__ANALOG_TV_APP_H__*/
