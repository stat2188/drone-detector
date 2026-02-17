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

#include <cstring>
#include <algorithm>

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

// RAII for baseband resources
class ScopedBaseband {
public:
    ScopedBaseband() {
        baseband::run_prepared_image(portapack::memory::map::m4_code.base());
    }
    
    ~ScopedBaseband() {
        baseband::shutdown();
    }
};

// RAII for receiver resources
class ScopedReceiver {
public:
    ScopedReceiver() {
        receiver_model.enable();
    }
    
    ~ScopedReceiver() {
        receiver_model.disable();
    }
};

// RAII for audio output
class ScopedAudio {
public:
    ScopedAudio() {
        audio::output::stop();
    }
    
    ~ScopedAudio() {
        audio::output::stop();
    }
};

/* FoundChannel implementation */

void AnalogTvView::FoundChannel::set_from_detector(const TVSignalDetector::DetectionResult& result) {
    frequency = result.frequency;
    signal_strength = static_cast<int8_t>(result.signal_strength);
    is_valid = result.is_tv_signal;

    auto freq_str = to_string_short_freq(frequency);
    size_t len = std::min(freq_str.length(), Constants::CHANNEL_NAME_MAX_LEN);
    std::memcpy(name.data(), freq_str.c_str(), len);
    name[len] = '\0';

    // Copy modulation type from detector result
    std::memcpy(modulation_type.data(), result.modulation_type.data(), result.modulation_type.size());
}

/* AnalogTvView *******************************************************/

AnalogTvView::AnalogTvView(
    NavigationView& nav)
    : nav_(nav) {
    // Consolidated header - only essential widgets
    add_children({&field_frequency,
                  &field_lna,
                  &field_vga,
                  &field_volume,
                  &tv,
                  &button_scan_start,
                  &button_scan_stop,
                  &button_manual,
                  &text_scan_status,
                  &text_found_channels,
                  &text_current_channel,
                  &text_progress,
                  &field_scan_start,
                  &field_scan_end,
                  &field_scan_step,
                  &field_min_signal,
                  &field_scan_timeout});

    field_frequency.updated = [this](rf::Frequency f) {
        this->on_frequency_changed(f);
    };

    field_lna.on_show_options = [this]() {
        this->on_show_options_rf_gain();
    };

    field_vga.on_show_options = [this]() {
        this->on_show_options_rf_gain();
    };

    // Set default modulation (no UI widget needed)
    const auto modulation = receiver_model.modulation();
    update_modulation(static_cast<ReceiverModel::Mode>(modulation));

    tv.on_select = [this](int32_t offset) {
        field_frequency.set_value(receiver_model.target_frequency() + offset);
    };

    // Connect callback for TV signal detection
    tv.on_tv_signal_detected = [this](const TVSignalDetector::DetectionResult& result) {
        if (scan_state.load(std::memory_order_acquire) == ScanState::Scanning) {
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
        if (scan_state.load(std::memory_order_acquire) == ScanState::Scanning) {
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

    update_modulation(ReceiverModel::Mode::WidebandFMAudio);
}

AnalogTvView::~AnalogTvView() {
    view_destroying = true;
    
    // Signal thread to terminate
    thread_terminate.store(true, std::memory_order_release);
    
    // Wait for thread to finish with timeout
    const auto current_state = scan_state.load(std::memory_order_acquire);
    if (current_state == ScanState::Scanning || current_state == ScanState::Paused) {
        // Give thread time to exit gracefully
        chThdSleepMilliseconds(Constants::THREAD_JOIN_TIMEOUT_MS);
    }
    
    // RAII cleanup
    ScopedAudio audio_cleanup;
    ScopedReceiver receiver_cleanup;
    ScopedBaseband baseband_cleanup;
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
    tv.on_show();
}

void AnalogTvView::remove_options_widget() {
    if (options_widget) {
        remove_child(options_widget.get());
        options_widget.reset();
    }

    field_lna.set_style(nullptr);
    field_frequency.set_style(nullptr);
}

void AnalogTvView::set_options_widget(std::unique_ptr<Widget> new_widget) {
    remove_options_widget();

    if (new_widget) {
        options_widget = std::move(new_widget);
    } else {
        // Use rectangle for options view
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

void AnalogTvView::on_left() {
    const auto current_state = scan_state.load(std::memory_order_acquire);
    if (current_state == ScanState::Idle && found_channels_count.load(std::memory_order_acquire) > 0) {
        if (current_channel_index > 0) {
            current_channel_index--;
            switch_to_channel(current_channel_index);
        }
    }
}

void AnalogTvView::on_right() {
    const auto current_state = scan_state.load(std::memory_order_acquire);
    if (current_state == ScanState::Idle && found_channels_count.load(std::memory_order_acquire) > 0) {
        const auto count = found_channels_count.load(std::memory_order_acquire);
        if (current_channel_index < count - 1) {
            current_channel_index++;
            switch_to_channel(current_channel_index);
        }
    }
}

// Implementation of scanning methods
void AnalogTvView::start_scan() {
    const auto current_state = scan_state.load(std::memory_order_acquire);
    if (current_state == ScanState::Scanning) return;

    scan_state.store(ScanState::Scanning, std::memory_order_release);
    thread_terminate.store(false, std::memory_order_release);
    found_channels_count.store(0, std::memory_order_release);
    current_scan_freq = scan_params.start_freq;
    ui_update_counter = 0;
    last_added_freq = 0;

    text_scan_status.set("Status: Scanning...");
    text_found_channels.set("Channels: 0");
    text_progress.set("Progress: 0%");

    chThdCreateFromHeap(nullptr, Constants::THREAD_STACK_SIZE, Constants::THREAD_PRIORITY,
                       [](void* arg) -> msg_t {
                           auto self = static_cast<AnalogTvView*>(arg);
                           return self->scan_worker_thread();
                       }, this);
}

msg_t AnalogTvView::scan_worker_thread() {
    while (scan_state.load(std::memory_order_acquire) == ScanState::Scanning && 
           current_scan_freq <= scan_params.end_freq && 
           !thread_terminate.load(std::memory_order_acquire)) {
        
        if (scan_state.load(std::memory_order_acquire) == ScanState::Paused) {
            text_scan_status.set("Status: Paused");
            while (scan_state.load(std::memory_order_acquire) == ScanState::Paused && 
                   !thread_terminate.load(std::memory_order_acquire)) {
                chThdSleepMilliseconds(100);
            }
            const auto state = scan_state.load(std::memory_order_acquire);
            if (state != ScanState::Scanning || thread_terminate.load(std::memory_order_acquire)) break;
            text_scan_status.set("Status: Scanning...");
        }

        if (view_destroying) {
            scan_state.store(ScanState::Idle, std::memory_order_release);
            break;
        }

        receiver_model.set_target_frequency(current_scan_freq);

        chThdSleepMilliseconds(scan_params.scan_timeout_ms);

        if (view_destroying) {
            scan_state.store(ScanState::Idle, std::memory_order_release);
            break;
        }

        update_scan_progress();

        current_scan_freq += scan_params.step;
    }

    const auto final_state = scan_state.load(std::memory_order_acquire);
    if (final_state == ScanState::Scanning && !thread_terminate.load(std::memory_order_acquire)) {
        scan_state.store(ScanState::Complete, std::memory_order_release);
        text_scan_status.set("Status: Complete");
        save_found_channels();
        if (found_channels_count.load(std::memory_order_acquire) > 0) {
            switch_to_channel(0);
        }
    } else if (thread_terminate.load(std::memory_order_acquire)) {
        scan_state.store(ScanState::Idle, std::memory_order_release);
        text_scan_status.set("Status: Stopped");
        save_found_channels();
    }

    return 0;
}

void AnalogTvView::update_scan_progress() {
    ui_update_counter++;
    if (ui_update_counter < Constants::UI_UPDATE_SKIP) {
        return;
    }
    ui_update_counter = 0;

    if (scan_params.end_freq > scan_params.start_freq) {
        // Use 64-bit arithmetic to prevent integer overflow
        const int64_t total_range = scan_params.end_freq - scan_params.start_freq;
        const int64_t current_pos = current_scan_freq - scan_params.start_freq;
        
        // Clamp percentage to [0, 100] to prevent overflow
        const int64_t percentage_scaled = (current_pos * 100) / total_range;
        const int percentage = static_cast<int>(std::clamp(percentage_scaled, int64_t{0}, int64_t{100}));

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Progress: %d%%", percentage);
        text_progress.set(buffer);
    }
}

void AnalogTvView::stop_scan() {
    scan_state.store(ScanState::Idle, std::memory_order_release);
    thread_terminate.store(true, std::memory_order_release);
}

void AnalogTvView::pause_scan() {
    scan_state.store(ScanState::Paused, std::memory_order_release);
}

void AnalogTvView::resume_scan() {
    scan_state.store(ScanState::Scanning, std::memory_order_release);
}

void AnalogTvView::add_found_channel(const TVSignalDetector::DetectionResult& result) {
    if (!result.is_tv_signal) return;

    if (result.frequency == last_added_freq) return;

    const auto current_count = found_channels_count.load(std::memory_order_acquire);
    
    // Bounds check BEFORE access
    if (current_count >= Constants::MAX_FOUND_CHANNELS) return;

    // Check for duplicates
    for (size_t i = 0; i < current_count; i++) {
        if (std::abs(found_channels[i].frequency - result.frequency) < Constants::FREQUENCY_TOLERANCE_HZ) {
            return;
        }
    }

    found_channels[current_count].set_from_detector(result);
    found_channels_count.store(current_count + 1, std::memory_order_release);
    last_added_freq = result.frequency;

    ui_update_counter++;
    if (ui_update_counter >= Constants::UI_UPDATE_SKIP || current_count == 0) {
        ui_update_counter = 0;

        char buffer[32];
        const auto new_count = found_channels_count.load(std::memory_order_acquire);
        snprintf(buffer, sizeof(buffer), "Channels: %u", static_cast<unsigned>(new_count));
        text_found_channels.set(buffer);

        snprintf(buffer, sizeof(buffer), "Current: %s (%s)",
                 found_channels[new_count - 1].name.data(),
                 to_string_short_freq(found_channels[new_count - 1].frequency).c_str());
        text_current_channel.set(buffer);
    }
}

void AnalogTvView::switch_to_channel(size_t index) {
    const auto count = found_channels_count.load(std::memory_order_acquire);
    if (index >= count) return;

    auto& channel = found_channels[index];
    current_channel_index = index;

    receiver_model.set_target_frequency(channel.frequency);
    field_frequency.set_value(channel.frequency);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Current: %s (%s)",
             channel.name.data(),
             to_string_short_freq(channel.frequency).c_str());
    text_current_channel.set(buffer);

    snprintf(buffer, sizeof(buffer), "Watching: %s", channel.name.data());
    text_scan_status.set(buffer);
}

void AnalogTvView::save_found_channels() {
    const auto count = found_channels_count.load(std::memory_order_acquire);
    if (count == 0) return;

    std::string filename = "TV_CHANNELS.txt";

    File file;
    auto error = file.create(filename);
    if (error) return;

    std::string header = "Found TV Channels\n==================\n\n";
    file.write(header.c_str(), header.length());

    char line_buffer[64];
    for (size_t i = 0; i < count; i++) {
        const auto& channel = found_channels[i];
        snprintf(line_buffer, sizeof(line_buffer), "%s MHz | %s | %d dB\n",
                 to_string_short_freq(channel.frequency).c_str(),
                 channel.modulation_type.data(),
                 channel.signal_strength);
        file.write(line_buffer, strlen(line_buffer));
    }

    file.close();
}

void AnalogTvView::load_scan_settings() {
    // Load scanning settings from persistent memory
    // Use standard default values
    scan_params.start_freq = Constants::DEFAULT_START_FREQ_HZ;
    scan_params.end_freq = Constants::DEFAULT_END_FREQ_HZ;
    scan_params.step = Constants::DEFAULT_STEP_FREQ_HZ;
    scan_params.min_signal_db = Constants::DEFAULT_MIN_SIGNAL_DB;
    scan_params.scan_timeout_ms = Constants::DEFAULT_TIMEOUT_MS;
    
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
