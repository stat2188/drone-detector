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
    , button_add_{{UI_POS_X(0), 270, UI_POS_WIDTH(4), 20}, "Capt"}
    , button_save_{{UI_POS_X(5), 270, UI_POS_WIDTH(4), 20}, "Save"}
    , button_edit_{{UI_POS_X(10), 270, UI_POS_WIDTH(4), 20}, "Edit"}
    , button_delete_{{UI_POS_X(15), 270, UI_POS_WIDTH(4), 20}, "Del"}
    , button_clear_all_{{UI_POS_X(20), 270, UI_POS_WIDTH(4), 20}, "Clr"}
    , button_back_{{UI_POS_X(24), 270, UI_POS_WIDTH(3), 20}, "<="}
    , button_start_capture_{{UI_POS_X(15), 270, UI_POS_WIDTH(5), 20}, "START"}
    , label_status_{{UI_POS_X(0), 30, UI_POS_WIDTH(28), 20}, "Idle"}
    , label_range_{{UI_POS_X(14), 0, UI_POS_WIDTH(10), 20}, "Rng:"}
    , view_state_(ViewState::IDLE)
    , selected_bin_(-1)
    , bin_selected_(false)
    , selected_range_idx_(0)
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

    for (size_t i = 0; i < FFT_BIN_COUNT; ++i) {
        capture_spectrum_[i] = 0;
        capture_spectrum_avg_[i] = 0;
    }

    add_children({
        &labels_,
        &label_range_,
        &field_range_,
        &button_freq_,
        &field_patterns_,
        &button_add_,
        &button_save_,
        &button_edit_,
        &button_delete_,
        &button_clear_all_,
        &button_back_,
        &button_start_capture_,
        &label_status_
    });

    button_back_.on_select = [this](ui::Button&) {
        // CRITICAL: Stop streaming before going back to prevent
        // DBLREG hard fault when returning to main scanner view.
        // Mirror Looking Glass: stop streaming immediately.
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
            label_status_.set("Pattern manager not ready");
            set_dirty();
            return;
        }
        if (!bin_selected_ || selected_bin_ < 0) {
            label_status_.set("Select bin first");
            set_dirty();
            return;
        }
        if (capture_active_) {
            label_status_.set("Capture in progress");
            set_dirty();
            return;
        }
        if (capture_spectrum_[0] == 0) {
            label_status_.set("No capture data - run Capt first");
            set_dirty();
            return;
        }
        char default_name[PATTERN_NAME_MAX_LEN];
        const size_t count = pattern_manager_ptr_->get_pattern_count();
        snprintf(default_name, sizeof(default_name), "PTR_%zu", count + 1);
        const ErrorCode err = save_current_pattern(default_name);
        if (err == ErrorCode::SUCCESS) {
            label_status_.set("Pattern saved!");
            refresh_list();
            std::memset(capture_spectrum_, 0, sizeof(capture_spectrum_));
            selected_bin_ = -1;
            bin_selected_ = false;
        } else if (err == ErrorCode::BUFFER_FULL) {
            label_status_.set("Max patterns reached");
        } else {
            label_status_.set("Save failed!");
        }
        set_dirty();
    };

    button_edit_.on_select = [this](ui::Button&) {
        show_pattern_details();
    };

    button_delete_.on_select = [this](ui::Button&) {
        delete_selected_pattern();
    };

    button_clear_all_.on_select = [this](ui::Button&) {
        clear_all_patterns();
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

    ScanConfig cfg = scanner_ptr->get_config();
    current_range_start_ = 0;
    current_range_end_ = 0;
    live_center_frequency_ = 0;
    live_bin_step_hz_ = 0;
    bool range_enabled = false;

    switch (selected_range_idx_) {
        case 0:
            current_range_start_ = cfg.sweep_start_freq;
            current_range_end_ = cfg.sweep_end_freq;
            live_center_frequency_ = current_range_start_ + (current_range_end_ - current_range_start_) / 2;
            live_bin_step_hz_ = (current_range_end_ - current_range_start_) / 240;
            range_enabled = true;
            break;
        case 1:
            if (cfg.sweep2_enabled) {
                current_range_start_ = cfg.sweep2_start_freq;
                current_range_end_ = cfg.sweep2_end_freq;
                live_center_frequency_ = current_range_start_ + (current_range_end_ - current_range_start_) / 2;
                live_bin_step_hz_ = (current_range_end_ - current_range_start_) / 240;
                range_enabled = true;
            }
            break;
        case 2:
            if (cfg.sweep3_enabled) {
                current_range_start_ = cfg.sweep3_start_freq;
                current_range_end_ = cfg.sweep3_end_freq;
                live_center_frequency_ = current_range_start_ + (current_range_end_ - current_range_start_) / 2;
                live_bin_step_hz_ = (current_range_end_ - current_range_start_) / 240;
                range_enabled = true;
            }
            break;
        case 3:
            if (cfg.sweep4_enabled) {
                current_range_start_ = cfg.sweep4_start_freq;
                current_range_end_ = cfg.sweep4_end_freq;
                live_center_frequency_ = current_range_start_ + (current_range_end_ - current_range_start_) / 2;
                live_bin_step_hz_ = (current_range_end_ - current_range_start_) / 240;
                range_enabled = true;
            }
            break;
    }

    char range_info[32];
    if (range_enabled && current_range_start_ > 0 && current_range_end_ > current_range_start_) {
        const uint32_t start_mhz = static_cast<uint32_t>(current_range_start_ / 1000000);
        const uint32_t end_mhz = static_cast<uint32_t>(current_range_end_ / 1000000);
        snprintf(range_info, sizeof(range_info), "%lu-%luMHz",
                 static_cast<unsigned long>(start_mhz), static_cast<unsigned long>(end_mhz));
    } else if (!range_enabled) {
        snprintf(range_info, sizeof(range_info), "Disabled");
    } else {
        snprintf(range_info, sizeof(range_info), "N/A");
    }
    label_status_.set(range_info);
    set_dirty();
}

FreqHz PatternManagerView::get_range_center_freq(uint8_t range_idx) const noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) {
        return 0;
    }

    ScanConfig cfg = scanner_ptr->get_config();

    switch (range_idx) {
        case 0:
            return cfg.sweep_start_freq + (cfg.sweep_end_freq - cfg.sweep_start_freq) / 2;
        case 1:
            if (cfg.sweep2_enabled) {
                return cfg.sweep2_start_freq + (cfg.sweep2_end_freq - cfg.sweep2_start_freq) / 2;
            }
            break;
        case 2:
            if (cfg.sweep3_enabled) {
                return cfg.sweep3_start_freq + (cfg.sweep3_end_freq - cfg.sweep3_start_freq) / 2;
            }
            break;
        case 3:
            if (cfg.sweep4_enabled) {
                return cfg.sweep4_start_freq + (cfg.sweep4_end_freq - cfg.sweep4_start_freq) / 2;
            }
            break;
    }
    return 0;
}

FreqHz PatternManagerView::get_range_bin_step(uint8_t range_idx) const noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr == nullptr) {
        return 0;
    }

    ScanConfig cfg = scanner_ptr->get_config();

    switch (range_idx) {
        case 0:
            return (cfg.sweep_end_freq - cfg.sweep_start_freq) / 240;
        case 1:
            if (cfg.sweep2_enabled) {
                return (cfg.sweep2_end_freq - cfg.sweep2_start_freq) / 240;
            }
            break;
        case 2:
            if (cfg.sweep3_enabled) {
                return (cfg.sweep3_end_freq - cfg.sweep3_start_freq) / 240;
            }
            break;
        case 3:
            if (cfg.sweep4_enabled) {
                return (cfg.sweep4_end_freq - cfg.sweep4_start_freq) / 240;
            }
            break;
    }
    return 0;
}

FreqHz PatternManagerView::bin_to_frequency(int16_t bin) const noexcept {
    if (live_center_frequency_ == 0 || live_bin_step_hz_ == 0) {
        return 0;
    }
    constexpr FreqHz SLICE_BW = SWEEP_SLICE_BW;
    constexpr FreqHz BIN_SIZE = SLICE_BW / FFT_BIN_COUNT;
    FreqHz offset = 0;
    if (bin >= static_cast<int16_t>(FFT_DC_SPIKE_END)) {
        offset = static_cast<FreqHz>(bin - 256) * BIN_SIZE;
    } else if (bin < static_cast<int16_t>(FFT_DC_SPIKE_START)) {
        offset = static_cast<FreqHz>(bin - 128) * BIN_SIZE;
    }
    return live_center_frequency_ + offset;
}

int16_t PatternManagerView::frequency_to_bin(FreqHz freq) const noexcept {
    if (live_center_frequency_ == 0 || live_bin_step_hz_ == 0) {
        return -1;
    }
    const FreqHz bin_size = SWEEP_SLICE_BW / FFT_BIN_COUNT;
    const int32_t offset = static_cast<int32_t>(freq - live_center_frequency_);
    int16_t bin = static_cast<int16_t>(offset / static_cast<int32_t>(bin_size)) + 128;
    if (bin < 0) bin = 0;
    if (bin >= static_cast<int16_t>(FFT_BIN_COUNT)) bin = static_cast<int16_t>(FFT_BIN_COUNT - 1);
    return bin;
}

void PatternManagerView::show_frequency_keypad() noexcept {
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

            int16_t bin = static_cast<int16_t>(x);
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
    range_select_state_ = RangeSelectState::WAITING_FOR_CAPTURE;

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

    (void)pattern_manager_ptr_->load_patterns();

    load_sweep_ranges();
    refresh_list();
    set_dirty();
}

void PatternManagerView::on_hide() noexcept {
    // CRITICAL: Stop baseband streaming before hiding to prevent
    // DBLREG hard fault when returning to main scanner view.
    // Mirror Looking Glass: stop streaming immediately.
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
    if (spectrum_fifo_ == nullptr) {
        return;
    }

    if (view_state_ == ViewState::IDLE && !capture_active_) {
        return;
    }

    ChannelSpectrum spectrum;
    if (spectrum_fifo_->out(spectrum)) {
        if (view_state_ == ViewState::LIVE) {
            for (size_t i = 0; i < FFT_BIN_COUNT && i < spectrum.db.size(); ++i) {
                capture_spectrum_[i] = spectrum.db[i];
            }
            set_dirty();
            return;
        }

        if (!capture_active_) {
            return;
        }

        if (fifo_count_ < MAX_SPECTRUM_FIFO) {
            for (size_t i = 0; i < FFT_BIN_COUNT && i < spectrum.db.size(); ++i) {
                fft_capture_buf_[fifo_count_][i] = spectrum.db[i];
            }
            ++fifo_count_;
        }

        if (fifo_count_ >= AVG_PASSES) {
            for (size_t i = 0; i < FFT_BIN_COUNT; ++i) {
                uint32_t sum = 0;
                for (size_t j = 0; j < AVG_PASSES; ++j) {
                    sum += fft_capture_buf_[j][i];
                }
                capture_spectrum_avg_[i] = static_cast<uint8_t>(sum / AVG_PASSES);
            }

            capture_active_ = false;
            view_state_ = ViewState::IDLE;
            button_start_capture_.set_text("START");
            on_capture_complete();
            return;
        }

        radio::set_tuning_frequency(rf::Frequency(capture_frequency_));
        baseband::spectrum_streaming_start();
    }
}

void PatternManagerView::start_live_spectrum() noexcept {
    capture_frequency_ = get_range_center_freq(selected_range_idx_);

    if (capture_frequency_ == 0) {
        label_status_.set("Range not set!");
        set_dirty();
        return;
    }

    live_center_frequency_ = capture_frequency_;
    live_bin_step_hz_ = get_range_bin_step(selected_range_idx_);

    label_status_.set("Live mode...");
    view_state_ = ViewState::LIVE;
    capture_active_ = false;
    bin_selected_ = false;
    selected_bin_ = -1;

    button_start_capture_.set_text("STOP");

    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, SWEEP_FFT_TRIGGER);

    radio::set_tuning_frequency(rf::Frequency(capture_frequency_));
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

    ScanConfig cfg = scanner_ptr->get_config();

    if (!bin_selected_ || selected_bin_ < 0 || capture_frequency_ == 0) {
        label_status_.set("Select bin first!");
        set_dirty();
        return;
    }

    live_center_frequency_ = capture_frequency_;
    live_bin_step_hz_ = get_range_bin_step(selected_range_idx_);

    if (live_bin_step_hz_ == 0) {
        live_bin_step_hz_ = (cfg.sweep_end_freq - cfg.sweep_start_freq) / 240;
    }

    label_status_.set("Capturing...");
    view_state_ = ViewState::CAPTURING;
    capture_active_ = true;
    capture_pass_ = 0;
    fifo_count_ = 0;

    std::memset(fft_capture_buf_, 0, sizeof(fft_capture_buf_));

    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, SWEEP_FFT_TRIGGER);

    radio::set_tuning_frequency(rf::Frequency(capture_frequency_));
    baseband::spectrum_streaming_start();

    set_dirty();
}

void PatternManagerView::on_capture_complete() noexcept {
    view_state_ = ViewState::IDLE;
    range_select_state_ = RangeSelectState::CAPTURE_COMPLETE;

    std::memcpy(capture_spectrum_, capture_spectrum_avg_, FFT_BIN_COUNT);

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
        snprintf(status_buf, sizeof(status_buf), "Done! Bin:%d",
                 (int)selected_bin_);
    } else {
        snprintf(status_buf, sizeof(status_buf), "Done - weak sig");
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

    painter.fill_rectangle(
        {start_x, start_y, width, height},
        Color::black()
    );

    if (spectrum == nullptr) {
        return;
    }

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
                            1,
                            h
                        }, Color::red());
                    }
                }
                continue;
            }
        } else {
            if (val > 150) {
                bar_color = Color::yellow();
            } else if (val > 100) {
                bar_color = Color::cyan();
            }
        }

        painter.fill_rectangle({
            start_x + static_cast<uint16_t>(i),
            start_y + height - bar_height,
            1,
            bar_height
        }, bar_color);
    }

    if (sel_bin >= 0 && sel_bin < static_cast<int16_t>(FFT_BIN_COUNT)) {
        painter.draw_rectangle({
            static_cast<uint16_t>(sel_bin - 2) + start_x,
            start_y,
            5,
            height
        }, Color::red());
    }
}

void PatternManagerView::paint(ui::Painter& painter) noexcept {
    (void)painter;

    if (view_state_ == ViewState::CAPTURING || view_state_ == ViewState::LIVE ||
        capture_spectrum_[0] != 0) {
        draw_spectrum_with_selection(painter, capture_spectrum_, selected_bin_);
    }
}

ErrorCode PatternManagerView::save_current_pattern(const char* name) noexcept {
    if (!bin_selected_ || selected_bin_ < 0) {
        return ErrorCode::INVALID_PARAMETER;
    }

    SignalPattern new_pattern{};

    size_t name_len = 0;
    while (name[name_len] != '\0' && name_len < PATTERN_NAME_MAX_LEN - 1) {
        new_pattern.name[name_len] = name[name_len];
        ++name_len;
    }
    new_pattern.name[name_len] = '\0';

    constexpr size_t valid_start = FFT_EDGE_SKIP_NARROW;
    constexpr size_t valid_end = FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW;
    constexpr size_t valid_bins = valid_end - valid_start;
    constexpr size_t bins_per_pattern = valid_bins / PATTERN_WAVEFORM_SIZE;

    for (size_t i = 0; i < PATTERN_WAVEFORM_SIZE; ++i) {
        const size_t bin_start = valid_start + (i * bins_per_pattern);
        size_t bin_end = bin_start + bins_per_pattern;
        if (bin_end > valid_end) {
            bin_end = valid_end;
        }

        uint32_t sum = 0;
        uint8_t count = 0;
        for (size_t j = bin_start; j < bin_end; ++j) {
            if (j >= FFT_DC_SPIKE_START && j < FFT_DC_SPIKE_END) {
                continue;
            }
            sum += capture_spectrum_[j];
            ++count;
        }
        new_pattern.waveform[i] = (count > 0) ? static_cast<uint8_t>(sum / count) : 0;
    }

    uint8_t peak_val = 0;
    size_t peak_idx = 0;
    for (size_t i = 0; i < FFT_BIN_COUNT; ++i) {
        if (capture_spectrum_[i] > peak_val) {
            peak_val = capture_spectrum_[i];
            peak_idx = i;
        }
    }

    new_pattern.features.peak_position = static_cast<uint8_t>(peak_idx);
    new_pattern.features.peak_value = peak_val;

    uint8_t noise_floor = 255;
    for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
        if (capture_spectrum_[i] < noise_floor && capture_spectrum_[i] > 0) {
            noise_floor = capture_spectrum_[i];
        }
    }
    new_pattern.features.noise_floor = noise_floor;

    new_pattern.features.margin = (peak_val > noise_floor) ? (peak_val - noise_floor) : 0;

    size_t left = peak_idx;
    while (left > 0 && capture_spectrum_[left] > noise_floor + new_pattern.features.margin / 2) {
        --left;
    }
    size_t right = peak_idx;
    while (right < FFT_BIN_COUNT - 1 && capture_spectrum_[right] > noise_floor + new_pattern.features.margin / 2) {
        ++right;
    }
    new_pattern.features.width = static_cast<uint8_t>(right - left);

    new_pattern.match_threshold = DEFAULT_PATTERN_CORRELATION_THRESHOLD;
    new_pattern.flags = SignalPattern::Flags::ENABLED;
    new_pattern.created_time = chTimeNow();
    new_pattern.match_count = 0;

    return pattern_manager_ptr_->save_pattern(new_pattern);
}

void PatternManagerView::refresh_list() noexcept {
    if (pattern_manager_ptr_ == nullptr) {
        return;
    }

    constexpr size_t MAX_OPTIONS = MAX_PATTERNS + 1;
    static char option_texts[MAX_OPTIONS][64]{};
    static ui::OptionsField::option_t options[MAX_OPTIONS];
    size_t option_count = 0;

    const size_t pattern_count = pattern_manager_ptr_->get_pattern_count();

    for (size_t i = 0; i < pattern_count && i < MAX_PATTERNS && option_count < MAX_OPTIONS - 1; ++i) {
        const SignalPattern* pattern = pattern_manager_ptr_->get_pattern(i);
        if (pattern != nullptr) {
            const char* status = pattern->is_enabled() ? "+" : "-";
            snprintf(option_texts[option_count], sizeof(option_texts[option_count]), "[%s] %.20s", status, pattern->name);
            option_texts[option_count][sizeof(option_texts[option_count]) - 1] = '\0';
            options[option_count] = {option_texts[option_count], static_cast<int32_t>(i)};
            ++option_count;
        }
    }

    if (option_count == 0) {
        snprintf(option_texts[0], sizeof(option_texts[0]), "No patterns");
        options[0] = {option_texts[0], 0};
        option_count = 1;
    }

    field_patterns_.set_options({options, options + option_count});
}

void PatternManagerView::show_pattern_details() noexcept {
    if (pattern_manager_ptr_ == nullptr || selected_index_ >= pattern_manager_ptr_->get_pattern_count()) {
        return;
    }

    const SignalPattern* pattern = pattern_manager_ptr_->get_pattern(selected_index_);
    if (pattern == nullptr) {
        return;
    }
}

void PatternManagerView::delete_selected_pattern() noexcept {
    if (pattern_manager_ptr_ == nullptr || selected_index_ >= pattern_manager_ptr_->get_pattern_count()) {
        return;
    }

    const ErrorCode err = pattern_manager_ptr_->delete_pattern(selected_index_);
    if (err == ErrorCode::SUCCESS) {
        refresh_list();
    }
}

void PatternManagerView::clear_all_patterns() noexcept {
    if (pattern_manager_ptr_ == nullptr) {
        return;
    }

    pattern_manager_ptr_->clear_all_patterns();
    refresh_list();
}

PatternManagerView::~PatternManagerView() noexcept = default;

} // namespace drone_analyzer
