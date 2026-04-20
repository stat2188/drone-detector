#include <cstdint>
#include <cstring>
#include <vector>
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
        {{UI_POS_X(0), 20}, "Select freq from spectrum", Color::white()}
    }
    , field_patterns_{{0, LIST_Y}, 18, {}, false}
    , button_add_{{UI_POS_X(0), 270, UI_POS_WIDTH(4), 20}, "Capt"}
    , button_save_{{UI_POS_X(5), 270, UI_POS_WIDTH(4), 20}, "Save"}
    , button_edit_{{UI_POS_X(10), 270, UI_POS_WIDTH(4), 20}, "Edit"}
    , button_delete_{{UI_POS_X(15), 270, UI_POS_WIDTH(4), 20}, "Del"}
    , button_clear_all_{{UI_POS_X(20), 270, UI_POS_WIDTH(4), 20}, "Clr"}
    , button_back_{{UI_POS_X(24), 270, UI_POS_WIDTH(3), 20}, "<="}
    , button_start_capture_{{120, 270, UI_POS_WIDTH(5), 20}, "START"}
    , label_status_{
        {{UI_POS_X(15), 20}, "Idle", Color::white()}
    }
    , view_state_(ViewState::IDLE)
    , selected_bin_(-1)
    , bin_selected_(false)
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
        nav_.pop();
    };

    button_add_.on_select = [this](ui::Button&) {
        start_capture_sequence();
    };

    button_save_.on_select = [this](ui::Button&) {
        if (!bin_selected_) {
            label_status_.set_text("Select bin first");
            return;
        }
        auto freq_view = nav_.push<FrequencyKeypadView>(capture_frequency_);
        freq_view->on_changed = [this](rf::Frequency f) {
            char name_buf[PATTERN_NAME_MAX_LEN] = {};
            size_t name_len = 0;
            const char* src = "Pattern";
            while (src[name_len] != '\0' && name_len < PATTERN_NAME_MAX_LEN - 1) {
                name_buf[name_len++] = src[name_len];
            }
            snprintf(name_buf + name_len, sizeof(name_buf) - name_len, "_%u",
                     (unsigned int)(f / 1000000));
            name_buf[sizeof(name_buf) - 1] = '\0';

            ErrorCode err = save_current_pattern(name_buf);
            if (err == ErrorCode::SUCCESS) {
                label_status_.set_text("Saved!");
                refresh_list();
            } else {
                label_status_.set_text("Save failed");
            }
        };
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
        start_capture_sequence();
    };

    field_patterns_.on_change = [this](size_t index, int32_t) {
        selected_index_ = static_cast<uint8_t>(index);
    };
}

void PatternManagerView::on_show() noexcept {
    DroneScanner& scanner_ref = get_scanner_instance();
    pattern_manager_ptr_ = &scanner_ref.get_pattern_manager();

    if (pattern_manager_ptr_->get_pattern_count() == 0) {
        (void)pattern_manager_ptr_->load_patterns();
    }

    refresh_list();
    set_dirty();
}

void PatternManagerView::on_hide() noexcept {
}

void PatternManagerView::focus() noexcept {
    if (pattern_manager_ptr_ == nullptr) {
        DroneScanner& scanner_ref = get_scanner_instance();
        pattern_manager_ptr_ = &scanner_ref.get_pattern_manager();
    }

    refresh_list();
    button_start_capture_.focus();
}

void PatternManagerView::on_channel_spectrum_config(ChannelSpectrumFIFO* fifo) noexcept {
    spectrum_fifo_ = fifo;
}

void PatternManagerView::on_frame_sync() noexcept {
    if (!capture_active_ || spectrum_fifo_ == nullptr) {
        return;
    }

    ChannelSpectrum spectrum;
    if (spectrum_fifo_->out(spectrum)) {
        if (fifo_count_ < MAX_SPECTRUM_FIFO) {
            for (size_t i = 0; i < FFT_BIN_COUNT && i < spectrum.db.size(); ++i) {
                spectrum_fifo_[fifo_count_][i] = spectrum.db[i];
            }
            ++fifo_count_;
        }

        if (fifo_count_ >= AVG_PASSES) {
            for (size_t i = 0; i < FFT_BIN_COUNT; ++i) {
                uint32_t sum = 0;
                for (size_t j = 0; j < AVG_PASSES; ++j) {
                    sum += spectrum_fifo_[j][i];
                }
                capture_spectrum_avg_[i] = static_cast<uint8_t>(sum / AVG_PASSES);
            }

            capture_active_ = false;
            on_capture_complete();
            return;
        }

        radio::set_tuning_frequency(rf::Frequency(capture_frequency_));
        baseband::spectrum_streaming_start();
    }
}

void PatternManagerView::start_capture_sequence() noexcept {
    DroneScanner& scanner_ref = get_scanner_instance();
    ScanConfig cfg = scanner_ref.get_config();

    capture_frequency_ = cfg.sweep_start_freq;

    label_status_.set_text("Capturing...");
    view_state_ = ViewState::CAPTURING;
    capture_active_ = true;
    capture_pass_ = 0;
    fifo_count_ = 0;
    selected_bin_ = -1;
    bin_selected_ = false;

    std::memset(spectrum_fifo_, 0, sizeof(spectrum_fifo_));

    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, SWEEP_FFT_TRIGGER);

    radio::set_tuning_frequency(rf::Frequency(capture_frequency_));
    baseband::spectrum_streaming_start();

    set_dirty();
}

void PatternManagerView::on_capture_complete() noexcept {
    view_state_ = ViewState::IDLE;

    std::memcpy(capture_spectrum_, capture_spectrum_avg_, FFT_BIN_COUNT);

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
    } else {
        selected_bin_ = -1;
        bin_selected_ = false;
    }

    char status_buf[32];
    if (bin_selected_) {
        snprintf(status_buf, sizeof(status_buf), "Peak: bin %d (sel)",
                 (int)selected_bin_);
    } else {
        snprintf(status_buf, sizeof(status_buf), "Done - %d bins", (int)fifo_count_);
    }
    label_status_.set_text(status_buf);

    set_dirty();
}

void PatternManagerView::draw_spectrum_with_selection(
    ui::Painter& painter,
    const uint8_t* spectrum,
    int16_t sel_bin
) noexcept {
    const uint16_t start_x = 0;
    const uint16_t start_y = SPECTRUM_Y;
    const uint16_t width = 240;
    const uint16_t height = SPECTRUM_HEIGHT;

    painter.fill_rectangle(
        {start_x, start_y, width, height},
        Color::black()
    );

    if (spectrum == nullptr) {
        return;
    }

    const uint8_t max_display = 180;
    bool prev_in_sel = false;

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

    if (sel_bin >= 0) {
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

    if (view_state_ == ViewState::CAPTURING || capture_spectrum_[0] != 0) {
        draw_spectrum_with_selection(painter, capture_spectrum_, selected_bin_);
    }
}

ErrorCode PatternManagerView::save_current_pattern(const char* name) noexcept {
    if (!bin_selected_ || selected_bin_ < 0) {
        return ErrorCode::INVALID_PARAMETER;
    }

    SignalPattern new_pattern{};
    std::memset(&new_pattern, 0, sizeof(SignalPattern));

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

    const size_t pattern_count = pattern_manager_ptr_->get_pattern_count();

    std::vector<ui::OptionsField::option_t> options;
    options.reserve(21);

    char item_str[64];

    for (size_t i = 0; i < pattern_count && i < 20; ++i) {
        const SignalPattern* pattern = pattern_manager_ptr_->get_pattern(i);
        if (pattern != nullptr) {
            const char* status = pattern->is_enabled() ? "+" : "-";
            snprintf(item_str, sizeof(item_str), "[%s] %.20s", status, pattern->name);
            item_str[sizeof(item_str) - 1] = '\0';
            options.emplace_back(item_str, static_cast<int32_t>(i));
        }
    }

    if (pattern_count == 0) {
        options.emplace_back("No patterns", 0);
    }

    field_patterns_.set_options(options);

    char count_str[32];
    snprintf(count_str, sizeof(count_str), "Count: %zu", pattern_count);
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