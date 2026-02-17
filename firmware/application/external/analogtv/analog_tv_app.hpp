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
#include <atomic>
#include <array>

namespace ui::external_app::analogtv {

// Named constants for magic numbers
namespace Constants {
    // Frequency constants (Hz)
    constexpr int64_t DEFAULT_START_FREQ_HZ = 100000000;
    constexpr int64_t DEFAULT_END_FREQ_HZ = 800000000;
    constexpr int64_t DEFAULT_STEP_FREQ_HZ = 200000;
    constexpr int64_t MIN_FREQ_HZ = 10000000;
    constexpr int64_t MAX_FREQ_HZ = 1000000000;
    constexpr int64_t MIN_STEP_HZ = 50000;
    constexpr int64_t MAX_STEP_HZ = 1000000;
    constexpr int64_t FREQUENCY_TOLERANCE_HZ = 50000;
    
    // Signal strength constants (dB)
    constexpr int8_t DEFAULT_MIN_SIGNAL_DB = -60;
    constexpr int8_t MIN_SIGNAL_DB = -100;
    constexpr int8_t MAX_SIGNAL_DB = -20;
    
    // Timeout constants (ms)
    constexpr int DEFAULT_TIMEOUT_MS = 500;
    constexpr int MIN_TIMEOUT_MS = 100;
    constexpr int MAX_TIMEOUT_MS = 2000;
    
    // UI constants
    constexpr size_t MAX_FOUND_CHANNELS = 50;
    constexpr int UI_UPDATE_SKIP = 10;
    constexpr size_t CHANNEL_NAME_MAX_LEN = 19;
    constexpr size_t MODULATION_TYPE_MAX_LEN = 7;
    
    // Thread constants
    constexpr int THREAD_STACK_SIZE = 2048;
    constexpr int THREAD_PRIORITY = NORMALPRIO + 10;
    constexpr int THREAD_JOIN_TIMEOUT_MS = 1000;
}

// Strong typing with enum class
enum class ScanState : uint8_t {
    Idle = 0,
    Scanning = 1,
    Paused = 2,
    Stopping = 3,
    Complete = 4
};

// Type aliases for clarity
using ChannelName = std::array<char, Constants::CHANNEL_NAME_MAX_LEN + 1>;
using ModulationType = std::array<char, Constants::MODULATION_TYPE_MAX_LEN + 1>;

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

    // Consolidated header - essential widgets only (4 widgets instead of 8)
    RxFrequencyField field_frequency{
        {5 * 8, UI_POS_Y(0)},
        nav_};

    LNAGainField field_lna{
        {15 * 8, UI_POS_Y(0)}};

    VGAGainField field_vga{
        {18 * 8, UI_POS_Y(0)}};

    AudioVolumeField field_volume{
        {27 * 8, UI_POS_Y(0)}};

    // Removed redundant widgets (RSSI, Channel, Audio, Modulation options)
    // These are less critical for the main TV viewing experience

    std::unique_ptr<Widget> options_widget{};

    tv::TVWidget tv{};
    
    // UI elements for scanning
    Button button_scan_start{ {UI_POS_X(0), UI_POS_Y(1), 8 * 8, 2 * 8}, "SCAN" };
    Button button_scan_stop{ {UI_POS_X(1), UI_POS_Y(1), 8 * 8, 2 * 8}, "STOP" };
    Button button_manual{ {UI_POS_X(2), UI_POS_Y(1), 8 * 8, 2 * 8}, "MANUAL" };
    
    Text text_scan_status{ {UI_POS_X(0), UI_POS_Y(2), 20 * 8, 1 * 8}, "Status: Ready" };
    Text text_found_channels{ {UI_POS_X(0), UI_POS_Y(3), 20 * 8, 1 * 8}, "Channels: 0" };
    Text text_current_channel{ {UI_POS_X(0), UI_POS_Y(4), 20 * 8, 1 * 8}, "Current: -" };
    Text text_progress{ {UI_POS_X(0), UI_POS_Y(5), 20 * 8, 1 * 8}, "Progress: 0%" };
    
    // Fields for scan range configuration
    NumberField field_scan_start{ {UI_POS_X(0), UI_POS_Y(7)}, 9, {Constants::MIN_FREQ_HZ, Constants::MAX_FREQ_HZ}, 1000000, ' ' };
    NumberField field_scan_end{ {UI_POS_X(1), UI_POS_Y(7)}, 9, {Constants::MIN_FREQ_HZ, Constants::MAX_FREQ_HZ}, 1000000, ' ' };
    NumberField field_scan_step{ {UI_POS_X(0), UI_POS_Y(8)}, 6, {Constants::MIN_STEP_HZ, Constants::MAX_STEP_HZ}, 10000, ' ' };
    NumberField field_min_signal{ {UI_POS_X(1), UI_POS_Y(8)}, 4, {Constants::MIN_SIGNAL_DB, Constants::MAX_SIGNAL_DB}, 1, ' ' };
    NumberField field_scan_timeout{ {UI_POS_X(0), UI_POS_Y(9)}, 4, {Constants::MIN_TIMEOUT_MS, Constants::MAX_TIMEOUT_MS}, 10, ' ' };
    
    // Structures for scanning
    struct FoundChannel {
        int64_t frequency;
        ChannelName name{};
        int8_t signal_strength;
        ModulationType modulation_type{};
        bool is_valid;

        FoundChannel() : frequency(0), signal_strength(0), is_valid(false) {
            name.fill('\0');
            modulation_type.fill('\0');
        }

        void set_from_detector(const TVSignalDetector::DetectionResult& result);
    };

    std::array<FoundChannel, Constants::MAX_FOUND_CHANNELS> found_channels{};
    
    // Thread-safe state variables using atomic
    std::atomic<size_t> found_channels_count{0};
    std::atomic<ScanState> scan_state{ScanState::Idle};
    std::atomic<bool> thread_terminate{false};
    
    size_t current_channel_index = 0;
    int64_t current_scan_freq = 0;
    bool view_destroying = false;
    int ui_update_counter{0};
    int64_t last_added_freq{0};

    struct ScanParameters {
        int64_t start_freq = Constants::DEFAULT_START_FREQ_HZ;
        int64_t end_freq = Constants::DEFAULT_END_FREQ_HZ;
        int64_t step = Constants::DEFAULT_STEP_FREQ_HZ;
        int min_signal_db = Constants::DEFAULT_MIN_SIGNAL_DB;
        int scan_timeout_ms = Constants::DEFAULT_TIMEOUT_MS;
    };

    ScanParameters scan_params{};

    void on_baseband_bandwidth_changed(uint32_t bandwidth_hz);
    void on_modulation_changed(const ReceiverModel::Mode modulation);
    void on_show_options_frequency();
    void on_show_options_rf_gain();
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
    
    // Joystick handling
    void on_left();
    void on_right();
    
    void on_frequency_changed(rf::Frequency f);
    
    // Scanning methods
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
