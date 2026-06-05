#include <cstdint>
#include <cstring>
#include <array>

#include "ch.h"

#include "ui.hpp"
#include "ui_text.hpp"
#include "ui_receiver.hpp"
#include "portapack_persistent_memory.hpp"
#include "portapack.hpp"

#include "drone_scanner_ui.hpp"
#include "pattern_manager_view.hpp"
#include "scanner.hpp"
#include "pattern_manager.hpp"
#include "pattern_matcher.hpp"
#include "peak_detector.hpp"
#include "constants.hpp"
#include "baseband_api.hpp"
#include "radio.hpp"

namespace drone_analyzer {

PatternManagerView::PatternManagerView(NavigationView& nav) noexcept
    : View()
    , nav_(nav)
    , labels_{
        {{UI_POS_X(0), UI_POS_Y(0)}, "PTR Pattern", Color::white()},
        {{UI_POS_X(0), 20}, "Tap spectrum to select", Color::white()}
    }
    , field_patterns_{{0, LIST_Y}, 18, {}, false}
    , field_range_{{UI_POS_X(18), 0}, 4, {
        {"SWP1", 0}, {"SWP2", 1}, {"SWP3", 2}, {"SWP4", 3}
    }}
    , button_freq_{{UI_POS_X(23), 0, UI_POS_WIDTH(3), 16}, "Freq"}
    , button_add_{{UI_POS_X(0), 270, UI_POS_WIDTH(5), 20}, "Capt"}
    , button_save_{{UI_POS_X(6), 270, UI_POS_WIDTH(5), 20}, "Save"}
    , button_delete_{{UI_POS_X(12), 270, UI_POS_WIDTH(5), 20}, "Del"}
    , button_back_{{UI_POS_X(24), 270, UI_POS_WIDTH(3), 20}, "<="}
    , button_start_capture_{{UI_POS_X_RIGHT(3), 270, UI_POS_WIDTH(3), 20}, "START"}
    , label_status_{{UI_POS_X(0), 30, UI_POS_WIDTH(28), 20}, "Idle"}
    , label_range_{{UI_POS_X(14), 0, UI_POS_WIDTH(10), 20}, "Rng:"}
    , message_handler_spectrum_config{
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->on_channel_spectrum_config(message.fifo);
        }
    }
    , message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            this->on_frame_sync();
        }
    } {

    add_children({
        &labels_,
        &label_range_,
        &field_range_,
        &button_freq_,
        &field_patterns_,
        &button_add_,
        &button_save_,
        &button_delete_,
        &button_back_,
        &button_start_capture_,
        &label_status_
    });

    button_back_.on_select = [this](ui::Button&) {
        if (view_state_ == ViewState::LIVE || view_state_ == ViewState::CAPTURING) {
            baseband::spectrum_streaming_stop();
            view_state_ = ViewState::IDLE;
        }
        nav_.pop();
    };

    button_freq_.on_select = [this](ui::Button&) {
        show_frequency_keypad();
    };

    field_range_.on_change = [this](size_t index, int32_t) {
        selected_range_idx_ = static_cast<uint8_t>(index);
        load_sweep_ranges();
    };

    button_add_.on_select = [this](ui::Button&) {
        if (!bin_selected_) {
            label_status_.set("Select bin first!");
            set_dirty();
            return;
        }
        start_capture_sequence();
    };

    button_save_.on_select = [this](ui::Button&) {
        if (pattern_manager_ptr_ == nullptr) {
            label_status_.set("No pattern mgr");
            set_dirty();
            return;
        }
        if (!bin_selected_ || selected_bin_ < 0) {
            label_status_.set("Select bin first");
            set_dirty();
            return;
        }
        if (capture_active_) {
            label_status_.set("Capture active");
            set_dirty();
            return;
        }
        if (!capture_completed_) {
            label_status_.set("Run Capt first");
            set_dirty();
            return;
        }

        char default_name[PATTERN_NAME_MAX_LEN];
        const size_t count = pattern_manager_ptr_->get_pattern_count();
        snprintf(default_name, sizeof(default_name), "PTR_%zu", count + 1);

        const ErrorCode err = save_current_pattern(default_name);
        if (err == ErrorCode::SUCCESS) {
            label_status_.set("Saved!");
            (void)pattern_manager_ptr_->reload_patterns();
            DroneScanner* scanner_ptr = get_scanner_ptr();
            if (scanner_ptr != nullptr) {
                scanner_ptr->refresh_patterns();
            }
            refresh_list();
            capture_completed_ = false;
            selected_bin_ = -1;
            bin_selected_ = false;
        } else if (err == ErrorCode::BUFFER_FULL) {
            label_status_.set("Max patterns");
        } else {
            label_status_.set("Save failed");
        }
        set_dirty();
    };

    button_delete_.on_select = [this](ui::Button&) {
        delete_selected_pattern();
    };

    button_start_capture_.on_select = [this](ui::Button&) {
        if (view_state_ == ViewState::LIVE) {
            view_state_ = ViewState::IDLE;
            button_start_capture_.set_text("START");
            label_status_.set("Stopped");
        } else if (bin_selected_) {
            start_capture_sequence();
        } else {
            start_live_spectrum();
        }
    };

    field_patterns_.on_change = [this](size_t index, int32_t) {
        selected_index_ = static_cast<uint8_t>(index);
    };
}

void PatternManagerView::load_sweep_ranges() noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) {
        current_range_start_ = 0;
        current_range_end_ = 0;
        live_center_frequency_ = 0;
        live_bin_step_hz_ = 0;
        label_status_.set("Scanner error");
        set_dirty();
        return;
    }

    const ScanConfig& cfg = scanner_ptr->get_config();
    current_range_start_ = 0;
    current_range_end_ = 0;
    live_center_frequency_ = 0;
    live_bin_step_hz_ = 0;
    bool range_enabled = false;

    switch (selected_range_idx_) {
        case 0:
            current_range_start_ = cfg.sweep_start_freq;
            current_range_end_ = cfg.sweep_end_freq;
            range_enabled = true;
            break;
        case 1:
            if (cfg.sweep2_enabled) {
                current_range_start_ = cfg.sweep2_start_freq;
                current_range_end_ = cfg.sweep2_end_freq;
                range_enabled = true;
            }
            break;
        case 2:
            if (cfg.sweep3_enabled) {
                current_range_start_ = cfg.sweep3_start_freq;
                current_range_end_ = cfg.sweep3_end_freq;
                range_enabled = true;
            }
            break;
        case 3:
            if (cfg.sweep4_enabled) {
                current_range_start_ = cfg.sweep4_start_freq;
                current_range_end_ = cfg.sweep4_end_freq;
                range_enabled = true;
            }
            break;
    }

    if (range_enabled && current_range_start_ > 0 && current_range_end_ > current_range_start_) {
        live_center_frequency_ = current_range_start_ + (current_range_end_ - current_range_start_) / 2;
        live_bin_step_hz_ = (current_range_end_ - current_range_start_) / SWEEP_PIXELS_PER_SLICE;

        char range_info[32];
        const uint32_t start_mhz = static_cast<uint32_t>(current_range_start_ / 1000000);
        const uint32_t end_mhz = static_cast<uint32_t>(current_range_end_ / 1000000);
        snprintf(range_info, sizeof(range_info), "%lu-%luMHz",
                 static_cast<unsigned long>(start_mhz), static_cast<unsigned long>(end_mhz));
        label_status_.set(range_info);
    } else {
        label_status_.set(range_enabled ? "N/A" : "Disabled");
    }
    set_dirty();
}

void PatternManagerView::init_sweep_range(uint8_t range_idx) noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) {
        sweep_start_ = 0;
        sweep_end_ = 0;
        sweep_step_ = 0;
        current_sweep_freq_ = 0;
        return;
    }

    const ScanConfig& cfg = scanner_ptr->get_config();
    FreqHz start = 0;
    FreqHz end = 0;
    FreqHz step = 0;
    bool enabled = false;

    switch (range_idx) {
        case 0:
            start = cfg.sweep_start_freq;
            end = cfg.sweep_end_freq;
            step = cfg.sweep_step_freq;
            enabled = true;
            break;
        case 1:
            if (cfg.sweep2_enabled) {
                start = cfg.sweep2_start_freq;
                end = cfg.sweep2_end_freq;
                step = cfg.sweep2_step_freq;
                enabled = true;
            }
            break;
        case 2:
            if (cfg.sweep3_enabled) {
                start = cfg.sweep3_start_freq;
                end = cfg.sweep3_end_freq;
                step = cfg.sweep3_step_freq;
                enabled = true;
            }
            break;
        case 3:
            if (cfg.sweep4_enabled) {
                start = cfg.sweep4_start_freq;
                end = cfg.sweep4_end_freq;
                step = cfg.sweep4_step_freq;
                enabled = true;
            }
            break;
    }

    if (!enabled || start >= end) {
        sweep_start_ = 0;
        sweep_end_ = 0;
        sweep_step_ = 0;
        current_sweep_freq_ = 0;
        return;
    }

    sweep_start_ = start;
    sweep_end_ = end;
    sweep_step_ = (step > 0) ? step : static_cast<FreqHz>(SWEEP_BINS_PER_STEP) * SWEEP_BIN_SIZE;
    current_sweep_freq_ = start;
}

FreqHz PatternManagerView::get_range_center_freq(uint8_t range_idx) const noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) return 0;

    const ScanConfig& cfg = scanner_ptr->get_config();
    switch (range_idx) {
        case 0: return cfg.sweep_start_freq + (cfg.sweep_end_freq - cfg.sweep_start_freq) / 2;
        case 1: if (cfg.sweep2_enabled) return cfg.sweep2_start_freq + (cfg.sweep2_end_freq - cfg.sweep2_start_freq) / 2; break;
        case 2: if (cfg.sweep3_enabled) return cfg.sweep3_start_freq + (cfg.sweep3_end_freq - cfg.sweep3_start_freq) / 2; break;
        case 3: if (cfg.sweep4_enabled) return cfg.sweep4_start_freq + (cfg.sweep4_end_freq - cfg.sweep4_start_freq) / 2; break;
    }
    return 0;
}

FreqHz PatternManagerView::get_range_bin_step(uint8_t range_idx) const noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) return 0;

    const ScanConfig& cfg = scanner_ptr->get_config();
    switch (range_idx) {
        case 0: return (cfg.sweep_end_freq - cfg.sweep_start_freq) / SWEEP_PIXELS_PER_SLICE;
        case 1: if (cfg.sweep2_enabled) return (cfg.sweep2_end_freq - cfg.sweep2_start_freq) / SWEEP_PIXELS_PER_SLICE; break;
        case 2: if (cfg.sweep3_enabled) return (cfg.sweep3_end_freq - cfg.sweep3_start_freq) / SWEEP_PIXELS_PER_SLICE; break;
        case 3: if (cfg.sweep4_enabled) return (cfg.sweep4_end_freq - cfg.sweep4_start_freq) / SWEEP_PIXELS_PER_SLICE; break;
    }
    return 0;
}

FreqHz PatternManagerView::bin_to_frequency(int16_t bin) const noexcept {
    if (live_center_frequency_ == 0 || live_bin_step_hz_ == 0) return 0;
    FreqHz offset = 0;
    if (bin >= static_cast<int16_t>(FFT_DC_SPIKE_END)) {
        offset = static_cast<FreqHz>(bin - 256) * SWEEP_BIN_SIZE;
    } else if (bin < static_cast<int16_t>(FFT_DC_SPIKE_START)) {
        offset = static_cast<FreqHz>(bin - 128) * SWEEP_BIN_SIZE;
    }
    return live_center_frequency_ + offset;
}

int16_t PatternManagerView::frequency_to_bin(FreqHz freq) const noexcept {
    if (live_center_frequency_ == 0 || live_bin_step_hz_ == 0) return -1;
    if (current_range_start_ > 0 && current_range_end_ > current_range_start_) {
        if (freq < current_range_start_ || freq > current_range_end_) return -1;
    }
    const FreqHz bin_size = SWEEP_SLICE_BW / FFT_BIN_COUNT;
    const int32_t offset = static_cast<int32_t>(freq - live_center_frequency_);
    int16_t bin = static_cast<int16_t>(offset / static_cast<int32_t>(bin_size)) + 128;
    if (bin < 0) bin = 0;
    if (bin >= static_cast<int16_t>(FFT_BIN_COUNT)) bin = static_cast<int16_t>(FFT_BIN_COUNT - 1);
    return bin;
}

void PatternManagerView::show_frequency_keypad() noexcept {
    if (live_center_frequency_ == 0 || live_bin_step_hz_ == 0) {
        load_sweep_ranges();
        if (live_center_frequency_ == 0 || live_bin_step_hz_ == 0) {
            label_status_.set("Select sweep range!");
            set_dirty();
            return;
        }
    }

    baseband::spectrum_streaming_stop();
    auto freq_view = nav_.push<FrequencyKeypadView>(capture_frequency_);
    freq_view->on_changed = [this](rf::Frequency f) {
        capture_frequency_ = static_cast<FreqHz>(f);
        int16_t bin = frequency_to_bin(capture_frequency_);
        if (bin >= 0) {
            selected_bin_ = bin;
            bin_selected_ = true;
            char status[32];
            const uint32_t mhz = static_cast<uint32_t>(capture_frequency_ / 1000000);
            const uint32_t khz = static_cast<uint32_t>((capture_frequency_ % 1000000) / 1000);
            snprintf(status, sizeof(status), "Bin:%d %lu.%03luMHz",
                     (int)selected_bin_, static_cast<unsigned long>(mhz), static_cast<unsigned long>(khz));
            label_status_.set(status);
        } else {
            label_status_.set("Out of range!");
        }
        set_dirty();
    };
}

bool PatternManagerView::on_touch(const ui::TouchEvent event) noexcept {
    if (event.type == ui::TouchEvent::Type::Start) {
        int16_t x = event.point.x();
        int16_t y = event.point.y();

        if (x >= SPECTRUM_X && x < SPECTRUM_X + SPECTRUM_WIDTH &&
            y >= SPECTRUM_Y && y < SPECTRUM_Y + SPECTRUM_HEIGHT) {
            int16_t bin = static_cast<int16_t>((x - SPECTRUM_X) * FFT_BIN_COUNT / SPECTRUM_WIDTH);
            if (bin >= 0 && bin < static_cast<int16_t>(FFT_BIN_COUNT)) {
                on_bin_selected(bin);
                return true;
            }
        }
    }
    return false;
}

void PatternManagerView::on_bin_selected(int16_t bin) noexcept {
    selected_bin_ = bin;
    bin_selected_ = true;
    capture_frequency_ = bin_to_frequency(bin);

    char status[32];
    const uint32_t mhz = static_cast<uint32_t>(capture_frequency_ / 1000000);
    const uint32_t tenths = static_cast<uint32_t>((capture_frequency_ % 1000000) / 100000);
    snprintf(status, sizeof(status), "Bin:%d %lu.%01luMHz",
             (int)bin, static_cast<unsigned long>(mhz), static_cast<unsigned long>(tenths));
    label_status_.set(status);
    set_dirty();
}

void PatternManagerView::on_show() noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) {
        label_status_.set("Scanner not ready");
        set_dirty();
        return;
    }

    PatternManager& pm = scanner_ptr->get_pattern_manager();
    pattern_manager_ptr_ = &pm;

    if (const auto reload_err = pattern_manager_ptr_->reload_patterns(); reload_err != ErrorCode::SUCCESS) {
        label_status_.set("Load failed");
    }

    capture_completed_ = false;
    load_sweep_ranges();
    refresh_list();
    set_dirty();
}

void PatternManagerView::on_hide() noexcept {
    if (view_state_ == ViewState::LIVE || view_state_ == ViewState::CAPTURING) {
        baseband::spectrum_streaming_stop();
    }
    view_state_ = ViewState::IDLE;
    capture_active_ = false;
    button_start_capture_.set_text("START");
}

void PatternManagerView::focus() noexcept {
    if (pattern_manager_ptr_ == nullptr) {
        DroneScanner* scanner_ptr = get_scanner_ptr();
        if (scanner_ptr != nullptr) {
            pattern_manager_ptr_ = &scanner_ptr->get_pattern_manager();
        }
    }
    refresh_list();
    button_start_capture_.focus();
}

void PatternManagerView::on_channel_spectrum_config(ChannelSpectrumFIFO* fifo) noexcept {
    spectrum_fifo_ = fifo;
}

void PatternManagerView::on_frame_sync() noexcept {
    if (spectrum_fifo_ == nullptr) return;
    if (view_state_ == ViewState::IDLE && !capture_active_) return;

    ChannelSpectrum& spectrum = spectrum_buffer_;
    if (!spectrum_fifo_->out(spectrum)) return;

    // LIVE mode: display spectrum and step frequency.
    if (view_state_ == ViewState::LIVE) {
        for (size_t i = 0; i < FFT_BIN_COUNT && i < spectrum.db.size(); ++i) {
            capture_spectrum_[i] = spectrum.db[i];
        }
        set_dirty();

        if (sweep_start_ > 0 && sweep_end_ > sweep_start_ && sweep_step_ > 0) {
            current_sweep_freq_ = (current_sweep_freq_ < sweep_end_)
                ? current_sweep_freq_ + sweep_step_
                : sweep_start_;
            radio::set_tuning_frequency(rf::Frequency(current_sweep_freq_));
            chThdSleepMilliseconds(5);
            baseband::spectrum_streaming_start();
        }
        return;
    }

    // CAPTURING mode: single-pass capture.
    if (!capture_active_) return;

    for (size_t i = 0; i < FFT_BIN_COUNT && i < spectrum.db.size(); ++i) {
        capture_spectrum_[i] = spectrum.db[i];
    }
    capture_active_ = false;
    on_capture_complete();
}

void PatternManagerView::start_live_spectrum() noexcept {
    init_sweep_range(selected_range_idx_);

    if (sweep_start_ == 0 || sweep_end_ <= sweep_start_) {
        label_status_.set("Range not set!");
        set_dirty();
        return;
    }

    live_center_frequency_ = capture_frequency_;
    live_bin_step_hz_ = get_range_bin_step(selected_range_idx_);

    label_status_.set("Live...");
    view_state_ = ViewState::LIVE;
    capture_active_ = false;
    capture_completed_ = false;
    bin_selected_ = false;
    selected_bin_ = -1;

    button_start_capture_.set_text("STOP");

    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, SWEEP_FFT_TRIGGER);

    radio::set_tuning_frequency(rf::Frequency(current_sweep_freq_));
    chThdSleepMilliseconds(5);
    baseband::spectrum_streaming_start();

    set_dirty();
}

void PatternManagerView::start_capture_sequence() noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) {
        label_status_.set("Scanner error");
        set_dirty();
        return;
    }

    if (!bin_selected_ || selected_bin_ < 0 || capture_frequency_ == 0) {
        label_status_.set("Select bin first!");
        set_dirty();
        return;
    }

    live_center_frequency_ = capture_frequency_;
    live_bin_step_hz_ = get_range_bin_step(selected_range_idx_);

    if (live_bin_step_hz_ == 0) {
        const ScanConfig& cfg = scanner_ptr->get_config();
        live_bin_step_hz_ = (cfg.sweep_end_freq - cfg.sweep_start_freq) / SWEEP_PIXELS_PER_SLICE;
    }

    label_status_.set("Capturing...");
    view_state_ = ViewState::CAPTURING;
    capture_active_ = true;
    capture_completed_ = false;

    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, SWEEP_FFT_TRIGGER);

    radio::set_tuning_frequency(rf::Frequency(capture_frequency_));
    chThdSleepMilliseconds(5);
    baseband::spectrum_streaming_start();

    set_dirty();
}

void PatternManagerView::on_capture_complete() noexcept {
    view_state_ = ViewState::IDLE;
    capture_completed_ = true;

    if (!bin_selected_ || selected_bin_ < 0) {
        size_t peak_bin = 0;
        uint8_t peak_val = 0;
        for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
            if (capture_spectrum_[i] > peak_val) {
                peak_val = capture_spectrum_[i];
                peak_bin = i;
            }
        }
        if (peak_val > 50) {
            selected_bin_ = static_cast<int16_t>(peak_bin);
            bin_selected_ = true;
            capture_frequency_ = bin_to_frequency(selected_bin_);
        }
    }

    char status_buf[32];
    if (bin_selected_ && selected_bin_ >= 0) {
        snprintf(status_buf, sizeof(status_buf), "Done! Bin:%d", (int)selected_bin_);
    } else {
        snprintf(status_buf, sizeof(status_buf), "Done - weak");
    }
    label_status_.set(status_buf);
    set_dirty();
}

void PatternManagerView::draw_spectrum_with_selection(
    ui::Painter& painter,
    const uint8_t* spectrum,
    int16_t sel_bin
) noexcept {
    const uint16_t start_x = SPECTRUM_X;
    const uint16_t start_y = SPECTRUM_Y;
    const uint16_t width = SPECTRUM_WIDTH;
    const uint16_t height = SPECTRUM_HEIGHT;

    painter.fill_rectangle({start_x, start_y, width, height}, Color::black());

    if (spectrum == nullptr) return;

    const uint8_t max_display = 180;

    for (size_t i = 0; i < 240; ++i) {
        const uint8_t val = (i < FFT_BIN_COUNT) ? spectrum[i] : 0;
        const uint16_t bar_height = static_cast<uint16_t>((val * height) / max_display);

        bool in_sel = (sel_bin >= 0 && i >= static_cast<size_t>(sel_bin - 2) && i <= static_cast<size_t>(sel_bin + 2));

        Color bar_color = Color::green();
        if (in_sel) {
            bar_color = Color::red();
            if (sel_bin > 0) {
                for (int16_t j = sel_bin - 2; j <= sel_bin + 2; ++j) {
                    if (j >= 0 && j < static_cast<int16_t>(FFT_BIN_COUNT)) {
                        uint16_t h = static_cast<uint16_t>((spectrum[j] * height) / max_display);
                        painter.fill_rectangle({
                            start_x + static_cast<uint16_t>(j),
                            start_y + height - h,
                            1, h
                        }, Color::red());
                    }
                }
                continue;
            }
        } else {
            if (val > 150) bar_color = Color::yellow();
            else if (val > 100) bar_color = Color::cyan();
        }

        painter.fill_rectangle({
            start_x + static_cast<uint16_t>(i),
            start_y + height - bar_height,
            1, bar_height
        }, bar_color);
    }

    if (sel_bin >= 0 && sel_bin < static_cast<int16_t>(FFT_BIN_COUNT)) {
        painter.draw_rectangle({
            static_cast<uint16_t>(sel_bin - 2) + start_x,
            start_y,
            5, height
        }, Color::red());
    }
}

void PatternManagerView::paint(ui::Painter& painter) noexcept {
    (void)painter;
    if (view_state_ == ViewState::CAPTURING || view_state_ == ViewState::LIVE || capture_completed_) {
        draw_spectrum_with_selection(painter, capture_spectrum_, selected_bin_);
    }
}

ErrorCode PatternManagerView::save_current_pattern(const char* name) noexcept {
    if (!bin_selected_ || selected_bin_ < 0) return ErrorCode::INVALID_PARAMETER;

    SignalPattern new_pattern{};

    size_t name_len = 0;
    while (name[name_len] != '\0' && name_len < PATTERN_NAME_MAX_LEN - 1) {
        new_pattern.name[name_len] = name[name_len];
        ++name_len;
    }
    new_pattern.name[name_len] = '\0';

    PatternMatcher::normalize(capture_spectrum_, new_pattern.waveform);

    // Use spectrum_buffer_ as scratch for PeakDetector (saves 256 B vs separate buffer).
    const PeakDetector::PeakInfo peak = PeakDetector::find(
        capture_spectrum_, spectrum_buffer_.db.data(),
        PeakDetector::Range::Full, PeakDetector::EdgePolicy::Wide);

    new_pattern.features.peak_position = static_cast<uint8_t>(peak.index / PATTERN_BIN_SCALE_FACTOR);
    new_pattern.features.peak_value = peak.value;
    new_pattern.features.noise_floor = peak.noise_floor;
    new_pattern.features.margin = peak.margin;

    constexpr uint16_t MIN_AUTO_THRESHOLD = 400;
    constexpr uint16_t MAX_AUTO_THRESHOLD = 800;
    const uint16_t auto_th = static_cast<uint16_t>(
        MIN_AUTO_THRESHOLD + static_cast<uint16_t>(new_pattern.features.margin) * 10U);
    new_pattern.match_threshold = (auto_th > MAX_AUTO_THRESHOLD) ? MAX_AUTO_THRESHOLD : auto_th;

    new_pattern.flags = SignalPattern::Flags::ENABLED;
    new_pattern.created_time = chTimeNow();
    new_pattern.center_freq = capture_frequency_;
    new_pattern.range_width = (current_range_end_ > current_range_start_)
        ? (current_range_end_ - current_range_start_)
        : SWEEP_SLICE_BW;

    return pattern_manager_ptr_->save_pattern(new_pattern);
}

void PatternManagerView::refresh_list() noexcept {
    if (pattern_manager_ptr_ == nullptr) return;

    // Stack: 10 × (64 + 4) = 680 bytes. Bounded by MAX_PATTERNS=10.
    constexpr size_t MAX_OPTS = MAX_PATTERNS + 1;
    char texts[MAX_OPTS][64]{};
    ui::OptionsField::option_t opts[MAX_OPTS];
    size_t count = 0;

    const size_t pattern_count = pattern_manager_ptr_->get_pattern_count();
    for (size_t i = 0; i < pattern_count && i < MAX_PATTERNS && count < MAX_OPTS - 1; ++i) {
        const SignalPattern* p = pattern_manager_ptr_->get_pattern(i);
        if (p != nullptr) {
            const char* s = p->is_enabled() ? "+" : "-";
            snprintf(texts[count], sizeof(texts[count]), "[%s] %.20s", s, p->name);
            opts[count] = {texts[count], static_cast<int32_t>(i)};
            ++count;
        }
    }

    if (count == 0) {
        snprintf(texts[0], sizeof(texts[0]), "No patterns");
        opts[0] = {texts[0], 0};
        count = 1;
    }

    field_patterns_.set_options({opts, opts + count});
}

void PatternManagerView::delete_selected_pattern() noexcept {
    if (pattern_manager_ptr_ == nullptr || selected_index_ >= pattern_manager_ptr_->get_pattern_count()) return;

    const ErrorCode err = pattern_manager_ptr_->delete_pattern(selected_index_);
    if (err == ErrorCode::SUCCESS) {
        DroneScanner* scanner_ptr = get_scanner_ptr();
        if (scanner_ptr != nullptr) {
            scanner_ptr->refresh_patterns();
        }
        refresh_list();
    }
}

PatternManagerView::~PatternManagerView() noexcept = default;

} // namespace drone_analyzer
