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

#include "analog_tv_app.hpp"

#include "baseband_api.hpp"

#include "portapack.hpp"
#include "portapack_persistent_memory.hpp"
using namespace portapack;
using namespace tonekey;

#include "audio.hpp"
#include "file.hpp"
#include "file_reader.hpp"

#include "utility.hpp"

#include "string_format.hpp"

#include "ch.h"

namespace ui::external_app::analogtv {

/* AnalogTvView *******************************************************/

AnalogTvView::AnalogTvView(
    NavigationView& nav)
    : nav_(nav) {
    add_children({&rssi,
                  &channel,
                  &audio,
                  &field_frequency,
                  &field_lna,
                  &field_vga,
                  &options_modulation,
                  &field_volume,
                  &tv});

    field_frequency.updated = [this](rf::Frequency f) {
        this->on_frequency_changed(f);
    };

    field_lna.on_show_options = [this]() {
        this->on_show_options_rf_gain();
    };

    field_vga.on_show_options = [this]() {
        this->on_show_options_rf_gain();
    };

    const auto modulation = receiver_model.modulation();
    options_modulation.set_by_value(toUType(ReceiverModel::Mode::WidebandFMAudio));
    options_modulation.on_change = [this](size_t, OptionsField::value_t v) {
        this->on_modulation_changed(static_cast<ReceiverModel::Mode>(v));
    };
    options_modulation.on_show_options = [this]() {
        this->on_show_options_modulation();
    };

    tv.on_select = [this](int32_t offset) {
        field_frequency.set_value(receiver_model.target_frequency() + offset);
    };

    // Connect callback for TV signal detection
    tv.on_tv_signal_detected = [this](const TVSignalDetector::DetectionResult& result) {
        if (is_scanning) {
            add_found_channel(result);
        }
    };

    // Setup scanning buttons
    button_scan_start.on_select = [this](Button&) {
        start_scan();
    };
    
    button_scan_stop.on_select = [this](Button&) {
        stop_scan();
    };
    
    button_manual.on_select = [this](Button&) {
        if (is_scanning) {
            pause_scan();
        }
        text_scan_status.set("Status: Manual");
    };
    
    // Setup fields for scanning parameters
    field_scan_start.on_change = [this](int32_t v) {
        scan_params.start_freq = v;
    };
    
    field_scan_end.on_change = [this](int32_t v) {
        scan_params.end_freq = v;
    };
    
    field_scan_step.on_change = [this](int32_t v) {
        scan_params.step = v;
    };
    
    field_min_signal.on_change = [this](int32_t v) {
        scan_params.min_signal_db = v;
    };
    
    field_scan_timeout.on_change = [this](int32_t v) {
        scan_params.scan_timeout_ms = v;
    };

    update_modulation(static_cast<ReceiverModel::Mode>(modulation));
    on_modulation_changed(ReceiverModel::Mode::WidebandFMAudio);
}

AnalogTvView::~AnalogTvView() {
    audio::output::stop();
    receiver_model.disable();
    baseband::shutdown();
}

void AnalogTvView::on_hide() {
    // TODO: Terrible kludge because widget system doesn't notify Waterfall that
    // it's being shown or hidden.
    tv.on_hide();
    View::on_hide();
}

void AnalogTvView::set_parent_rect(const Rect new_parent_rect) {
    View::set_parent_rect(new_parent_rect);

    const ui::Rect tv_rect{0, header_height, new_parent_rect.width(), new_parent_rect.height() - header_height};
    tv.set_parent_rect(tv_rect);
}

void AnalogTvView::focus() {
    field_frequency.focus();
}

void AnalogTvView::on_baseband_bandwidth_changed(uint32_t bandwidth_hz) {
    receiver_model.set_baseband_bandwidth(bandwidth_hz);
}

void AnalogTvView::on_modulation_changed(const ReceiverModel::Mode modulation) {
    // TODO: Terrible kludge because widget system doesn't notify Waterfall that
    // it's being shown or hidden.
    tv.on_hide();
    update_modulation(modulation);
    on_show_options_modulation();
    tv.on_show();
}

void AnalogTvView::remove_options_widget() {
    if (options_widget) {
        remove_child(options_widget.get());
        options_widget.reset();
    }

    field_lna.set_style(nullptr);
    options_modulation.set_style(nullptr);
    field_frequency.set_style(nullptr);
}

void AnalogTvView::set_options_widget(std::unique_ptr<Widget> new_widget) {
    remove_options_widget();

    if (new_widget) {
        options_widget = std::move(new_widget);
    } else {
        // TODO: Lame hack to hide options view due to my bad paint/damage algorithm.
        options_widget = std::make_unique<Rectangle>(options_view_rect, Theme::getInstance()->option_active->background);
    }
    add_child(options_widget.get());
}

void AnalogTvView::on_show_options_frequency() {
    auto widget = std::make_unique<FrequencyOptionsView>(options_view_rect, Theme::getInstance()->option_active);

    widget->set_step(receiver_model.frequency_step());
    widget->on_change_step = [this](rf::Frequency f) {
        this->on_frequency_step_changed(f);
    };
    widget->set_reference_ppm_correction(persistent_memory::correction_ppb() / 1000);
    widget->on_change_reference_ppm_correction = [this](int32_t v) {
        this->on_reference_ppm_correction_changed(v);
    };

    set_options_widget(std::move(widget));
    field_frequency.set_style(Theme::getInstance()->option_active);
}

void AnalogTvView::on_show_options_rf_gain() {
    auto widget = std::make_unique<RadioGainOptionsView>(options_view_rect, Theme::getInstance()->option_active);

    set_options_widget(std::move(widget));
    field_lna.set_style(Theme::getInstance()->option_active);
}

void AnalogTvView::on_show_options_modulation() {
    std::unique_ptr<Widget> widget;

    tv.show_audio_spectrum_view(true);

    set_options_widget(std::move(widget));
    options_modulation.set_style(Theme::getInstance()->option_active);
}

void AnalogTvView::on_frequency_step_changed(rf::Frequency f) {
    receiver_model.set_frequency_step(f);
    field_frequency.set_step(f);
}

void AnalogTvView::on_reference_ppm_correction_changed(int32_t v) {
    persistent_memory::set_correction_ppb(v * 1000);
}

void AnalogTvView::update_modulation(const ReceiverModel::Mode modulation) {
    audio::output::mute();

    baseband::shutdown();

    // portapack::spi_flash::image_tag_t image_tag; //moved to ext app, disabled
    // image_tag = portapack::spi_flash::image_tag_am_tv;

    // baseband::run_image(image_tag);
    baseband::run_prepared_image(portapack::memory::map::m4_code.base());  // moved the baseband too

    receiver_model.set_modulation(modulation);
    receiver_model.set_sampling_rate(2000000);
    receiver_model.set_baseband_bandwidth(2000000);
    receiver_model.enable();
}

void AnalogTvView::on_freqchg(int64_t freq) {
    field_frequency.set_value(freq);
}

void AnalogTvView::on_frequency_changed(rf::Frequency f) {
    receiver_model.set_target_frequency(f);
}

// Handle stick controls
void AnalogTvView::on_left() {
    if (!is_scanning && !found_channels.empty()) {
        // Switch to previous channel
        if (current_channel_index > 0) {
            current_channel_index--;
            switch_to_channel(current_channel_index);
        }
    }
}

void AnalogTvView::on_right() {
    if (!is_scanning && !found_channels.empty()) {
        // Switch to next channel
        if (current_channel_index < found_channels.size() - 1) {
            current_channel_index++;
            switch_to_channel(current_channel_index);
        }
    }
}

// Implementation of scanning methods
void AnalogTvView::start_scan() {
    if (is_scanning) return;
    
    is_scanning = true;
    scan_paused = false;
    found_channels.clear();
    current_scan_freq = scan_params.start_freq;
    
    // Update UI
    text_scan_status.set("Status: Scanning...");
    text_found_channels.set("Channels: 0");
    text_progress.set("Progress: 0%");
    
    // Start asynchronous scanning
    chThdCreateFromHeap(nullptr, 2048, NORMALPRIO + 10, 
                       [](void* arg) -> msg_t {
                           auto self = static_cast<AnalogTvView*>(arg);
                           return self->scan_worker_thread();
                       }, this);
}

msg_t AnalogTvView::scan_worker_thread() {
    while (is_scanning && current_scan_freq <= scan_params.end_freq) {
        if (scan_paused) {
            text_scan_status.set("Status: Paused");
            while (scan_paused && is_scanning) {
                chThdSleepMilliseconds(100);
            }
            if (!is_scanning) break;
            text_scan_status.set("Status: Scanning...");
        }
        
        // Set frequency
        receiver_model.set_target_frequency(current_scan_freq);
        
        // Wait for signal stabilization
        chThdSleepMilliseconds(scan_params.scan_timeout_ms);
        
        // Update progress
        update_scan_progress();
        
        current_scan_freq += scan_params.step;
    }
    
    if (is_scanning) {
        is_scanning = false;
        text_scan_status.set("Status: Complete");
        if (!found_channels.empty()) {
            switch_to_channel(0);
        }
    }
    
    return 0;
}

void AnalogTvView::update_scan_progress() {
    if (scan_params.end_freq > scan_params.start_freq) {
        int64_t total_range = scan_params.end_freq - scan_params.start_freq;
        int64_t current_pos = current_scan_freq - scan_params.start_freq;
        int percentage = (current_pos * 100) / total_range;
        text_progress.set("Progress: " + std::to_string(percentage) + "%");
    }
}

void AnalogTvView::stop_scan() {
    is_scanning = false;
    scan_paused = false;
}

void AnalogTvView::pause_scan() {
    scan_paused = true;
}

void AnalogTvView::resume_scan() {
    scan_paused = false;
}

void AnalogTvView::add_found_channel(const TVSignalDetector::DetectionResult& result) {
    FoundChannel channel;
    channel.frequency = result.frequency;
    channel.signal_strength = result.signal_strength;
    channel.modulation_type = result.modulation_type;
    channel.is_valid = result.is_tv_signal;
    
    // Generate channel name
    channel.name = "TV_" + to_string_short_freq(result.frequency);
    
    found_channels.push_back(channel);
    
    // Update UI
    text_found_channels.set("Channels: " + std::to_string(found_channels.size()));
    text_current_channel.set("Current: " + channel.name + 
                           " (" + to_string_short_freq(channel.frequency) + ")");
    
    // Save channel automatically
    save_found_channels();
}

void AnalogTvView::switch_to_channel(size_t index) {
    if (index >= found_channels.size()) return;
    
    auto& channel = found_channels[index];
    current_channel_index = index;
    
    // Set frequency
    receiver_model.set_target_frequency(channel.frequency);
    field_frequency.set_value(channel.frequency);
    
    text_current_channel.set("Current: " + channel.name + 
                           " (" + to_string_short_freq(channel.frequency) + ")");
    text_scan_status.set("Watching: " + channel.name);
}

void AnalogTvView::save_found_channels() {
    if (found_channels.empty()) return;
    
    // Create file to save found channels
    std::string filename = "TV_CHANNELS.txt";
    
    File file;
    auto error = file.create(filename);
    if (error) return;
    
    // Write header
    std::string header = "Found TV Channels\n==================\n\n";
    file.write(header.c_str(), header.length());
    
    // Write each channel
    for (const auto& channel : found_channels) {
        std::string line = to_string_short_freq(channel.frequency) + " MHz | " +
                          channel.modulation_type + " | " +
                          to_string_dec_int(channel.signal_strength) + " dB\n";
        file.write(line.c_str(), line.length());
    }
    
    file.close();
}

void AnalogTvView::load_scan_settings() {
    // Load scanning settings from persistent memory
    // Use standard default values
    scan_params.start_freq = 100000000;  // 100 MHz
    scan_params.end_freq = 800000000;    // 800 MHz
    scan_params.step = 200000;           // 200 kHz
    scan_params.min_signal_db = -60;     // -60 dB
    scan_params.scan_timeout_ms = 500;   // 500 ms
    
    // Update UI fields
    field_scan_start.set_value(scan_params.start_freq);
    field_scan_end.set_value(scan_params.end_freq);
    field_scan_step.set_value(scan_params.step);
    field_min_signal.set_value(scan_params.min_signal_db);
    field_scan_timeout.set_value(scan_params.scan_timeout_ms);
}

void AnalogTvView::save_scan_settings() {
    // Save scanning settings (stub)
    // In real implementation, need to add corresponding functions to persistent_memory
}

}  // namespace ui::external_app::analogtv
