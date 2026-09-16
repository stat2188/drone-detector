#include <cstdint>
#include <cstring>

#include "drone_sweep_view.hpp"
#include "drone_settings.hpp"
#include "settings_manager.hpp"
#include "scanner.hpp"
#include "constants.hpp"
#include "range_names.hpp"
#include "ui_receiver.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "baseband_api.hpp"
#include "drone_scanner_ui.hpp"

namespace drone_analyzer {

// Helpers: NumberField value in MHz/kHz → FreqHz
static FreqHz read_mhz_field(const ui::NumberField& field) noexcept {
    return static_cast<FreqHz>(field.value()) * MHZ;
}

// Build the OptionsField entries for the range-label selector from the
// Flash-resident RANGE_NAMES table (single source of truth). NOTE: heap use
// is confined to the UI layer (std::string options), same pattern as the
// "Win 1..4" selector — never touched from the scanner/DSP paths.
static ui::OptionsField::options_t build_range_name_options() noexcept {
    ui::OptionsField::options_t options;
    options.reserve(RANGE_NAME_COUNT);
    for (uint8_t i = 0; i < RANGE_NAME_COUNT; ++i) {
        options.emplace_back(
            RANGE_NAMES[i], static_cast<ui::OptionsField::value_t>(i));
    }
    return options;
}

// ============================================================================
// Sweep field ID → config mapping (for FrequencyKeypadView callbacks)
// ============================================================================

enum class SweepFieldID : uint8_t {
    W1_START, W1_END,
    W2_START, W2_END,
    W3_START, W3_END,
    W4_START, W4_END,
    // Detection-window ranges: W{N}_DW{slot}_{FROM|TO} — values MUST stay
    // dense & ordered (W1_DW0_FROM..W4_DW4_TO); set_dw_field_by_id() decodes
    // them arithmetically (see below).
    W1_DW0_FROM, W1_DW0_TO, W1_DW1_FROM, W1_DW1_TO, W1_DW2_FROM, W1_DW2_TO, W1_DW3_FROM, W1_DW3_TO, W1_DW4_FROM, W1_DW4_TO,
    W2_DW0_FROM, W2_DW0_TO, W2_DW1_FROM, W2_DW1_TO, W2_DW2_FROM, W2_DW2_TO, W2_DW3_FROM, W2_DW3_TO, W2_DW4_FROM, W2_DW4_TO,
    W3_DW0_FROM, W3_DW0_TO, W3_DW1_FROM, W3_DW1_TO, W3_DW2_FROM, W3_DW2_TO, W3_DW3_FROM, W3_DW3_TO, W3_DW4_FROM, W3_DW4_TO,
    W4_DW0_FROM, W4_DW0_TO, W4_DW1_FROM, W4_DW1_TO, W4_DW2_FROM, W4_DW2_TO, W4_DW3_FROM, W4_DW3_TO, W4_DW4_FROM, W4_DW4_TO,
};

// ============================================================================
// Detection-window field encoding
// Enum layout (dense):  [W1..W4 start/end] then 40 dw ids W{N}_DW{S}_{F,T}.
// off = (w*10) + (slot*2) + is_end  →  w = off/10, slot = (off%10)/2, end = off&1.
// NOTE: helpers are defined BEFORE set_config_field_by_id() — the default
// branch of that dispatcher calls set_dw_field_by_id(), and a static free
// function must be declared before its point of use (fixes
// "error: 'set_dw_field_by_id' was not declared in this scope").
// ============================================================================

static SweepFieldID dw_field_id(uint8_t w, uint8_t slot, bool is_end) noexcept {
    const uint8_t off = static_cast<uint8_t>((w * 10U) + (slot * 2U) + (is_end ? 1U : 0U));
    return (w < MAX_SWEEP_WINDOWS && slot < DETECTION_WINDOWS_PER_WINDOW)
        ? static_cast<SweepFieldID>(static_cast<uint8_t>(SweepFieldID::W1_DW0_FROM) + off)
        : SweepFieldID::W1_START;
}

static void set_dw_field_by_id(SweepFieldID id, FreqHz value) noexcept {
    const uint8_t raw = static_cast<uint8_t>(id);
    const uint8_t base = static_cast<uint8_t>(SweepFieldID::W1_DW0_FROM);
    if (raw < base) return;  // not a detection-window field
    const uint8_t off = raw - base;
    const uint8_t w = off / 10U;            // 0..3 (window index)
    const uint8_t slot = (off % 10U) / 2U;  // 0..4 (range slot)
    const bool is_end = (off & 1U) != 0;
    if (w >= MAX_SWEEP_WINDOWS || slot >= DETECTION_WINDOWS_PER_WINDOW) return;
    // ScanConfig stores detection windows as a MHz mirror (see scanner.hpp):
    // keypad delivers Hz, down-convert to MHz (UI displays MHz anyway).
    if (is_end) {
        g_workspace_cfg.sweep_det_win_end_mhz[w][slot] = static_cast<uint32_t>(value / MHZ);
    } else {
        g_workspace_cfg.sweep_det_win_start_mhz[w][slot] = static_cast<uint32_t>(value / MHZ);
    }
}

// W1..W4 window bounds; every other ID dispatches into the detection-window
// decoder above (for FrequencyKeypadView callbacks).
static void set_config_field_by_id(SweepFieldID field_id, rf::Frequency f) noexcept {
    const FreqHz value = static_cast<FreqHz>(f);
    switch (field_id) {
        case SweepFieldID::W1_START: g_workspace_cfg.sweep_start_freq = value; break;
        case SweepFieldID::W1_END: g_workspace_cfg.sweep_end_freq = value; break;
        case SweepFieldID::W2_START: g_workspace_cfg.sweep2_start_freq = value; break;
        case SweepFieldID::W2_END: g_workspace_cfg.sweep2_end_freq = value; break;
        case SweepFieldID::W3_START: g_workspace_cfg.sweep3_start_freq = value; break;
        case SweepFieldID::W3_END: g_workspace_cfg.sweep3_end_freq = value; break;
        case SweepFieldID::W4_START: g_workspace_cfg.sweep4_start_freq = value; break;
        case SweepFieldID::W4_END: g_workspace_cfg.sweep4_end_freq = value; break;
        default:
            set_dw_field_by_id(field_id, value);
            break;
    }
}

static SweepFieldID start_field_id(uint8_t w) noexcept {
    constexpr SweepFieldID table[4] = {SweepFieldID::W1_START, SweepFieldID::W2_START, SweepFieldID::W3_START, SweepFieldID::W4_START};
    return (w < 4) ? table[w] : SweepFieldID::W1_START;
}

static SweepFieldID end_field_id(uint8_t w) noexcept {
    constexpr SweepFieldID table[4] = {SweepFieldID::W1_END, SweepFieldID::W2_END, SweepFieldID::W3_END, SweepFieldID::W4_END};
    return (w < 4) ? table[w] : SweepFieldID::W1_END;
}

// ============================================================================
// Helper: open frequency keypad with nav.push() — returns to sweep view on Done
// ============================================================================
static void open_freq_keypad_push(
    NavigationView& nav,
    SweepFieldID field_id,
    FreqHz initial_hz,
    DroneScanner* scanner,
    ui::NumberField& target_field) noexcept {
    baseband::spectrum_streaming_stop();
    auto* new_view = nav.push<FrequencyKeypadView>(
        static_cast<rf::Frequency>(initial_hz));
    new_view->on_changed = [field_id, scanner, &target_field](rf::Frequency f) {
        set_config_field_by_id(field_id, f);
        target_field.set_value(static_cast<int32_t>(f / MHZ));
        if (scanner != nullptr) {
            (void)scanner->set_config(g_workspace_cfg);
        }
    };
}

// ============================================================================
// SweepWindowView — single window, reused for all 4
// ============================================================================

SweepWindowView::SweepWindowView(NavigationView& nav, const Rect parent_rect, DroneScanner* scanner_ptr) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr) {
    set_parent_rect(parent_rect);
    // add_children order == visual row order. It only serves as the tie-break
    // order for the geometric FocusManager (min_element keeps the first
    // equidistant widget), but keeping it aligned with the layout guarantees a
    // deterministic selector path.
    add_children({
        &labels_,                             // r0 title, r1 Start, r2 End
        &labels_dw_,                          // r0 From/To header
        &labels_dw_idx_,                      // r1..r5 range indexes
        &field_start_,                        // r1 left
        &field_dw0_start_, &field_dw0_end_,   // r1 right
        &field_end_,                          // r2 left
        &field_dw1_start_, &field_dw1_end_,   // r2 right
        &check_enabled_,                      // r3 left
        &field_dw2_start_, &field_dw2_end_,   // r3 right
        &field_dw3_start_, &field_dw3_end_,   // r4 right
        &field_dw4_start_, &field_dw4_end_,   // r5 right
        &labels_name_,                        // r8 Range:, r9 Name:
        &field_label_slot_,                   // r8 slot selector R1..R5
        &field_label_name_,                   // r9 label selector (RANGE_NAMES)
    });

    // on_select callbacks route through the bound window index
    field_start_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, start_field_id(bound_index_),
            static_cast<FreqHz>(field_start_.value()) * MHZ, scanner_ptr_, field_start_);
    };
    field_end_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, end_field_id(bound_index_),
            static_cast<FreqHz>(field_end_.value()) * MHZ, scanner_ptr_, field_end_);
    };

    // Detection-window range fields (5 × From/To)
    field_dw0_start_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 0, false),
            static_cast<FreqHz>(field_dw0_start_.value()) * MHZ, scanner_ptr_, field_dw0_start_);
    };
    field_dw0_end_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 0, true),
            static_cast<FreqHz>(field_dw0_end_.value()) * MHZ, scanner_ptr_, field_dw0_end_);
    };
    field_dw1_start_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 1, false),
            static_cast<FreqHz>(field_dw1_start_.value()) * MHZ, scanner_ptr_, field_dw1_start_);
    };
    field_dw1_end_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 1, true),
            static_cast<FreqHz>(field_dw1_end_.value()) * MHZ, scanner_ptr_, field_dw1_end_);
    };
    field_dw2_start_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 2, false),
            static_cast<FreqHz>(field_dw2_start_.value()) * MHZ, scanner_ptr_, field_dw2_start_);
    };
    field_dw2_end_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 2, true),
            static_cast<FreqHz>(field_dw2_end_.value()) * MHZ, scanner_ptr_, field_dw2_end_);
    };
    field_dw3_start_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 3, false),
            static_cast<FreqHz>(field_dw3_start_.value()) * MHZ, scanner_ptr_, field_dw3_start_);
    };
    field_dw3_end_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 3, true),
            static_cast<FreqHz>(field_dw3_end_.value()) * MHZ, scanner_ptr_, field_dw3_end_);
    };
    field_dw4_start_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 4, false),
            static_cast<FreqHz>(field_dw4_start_.value()) * MHZ, scanner_ptr_, field_dw4_start_);
    };
    field_dw4_end_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, dw_field_id(bound_index_, 4, true),
            static_cast<FreqHz>(field_dw4_end_.value()) * MHZ, scanner_ptr_, field_dw4_end_);
    };

    // Range-label editor wiring (lower half).
    field_label_name_.set_options(build_range_name_options());
    // Slot selector: on switch, show the newly selected slot's current label.
    field_label_slot_.on_change = [this](size_t, int32_t v) {
        label_slot_ = (v >= 0 && v < DETECTION_WINDOWS_PER_WINDOW)
            ? static_cast<uint8_t>(v) : 0U;
        if (bound_data_ != nullptr) {
            field_label_name_.set_by_value(
                static_cast<int32_t>(bound_data_->name_idx[label_slot_]));
        }
    };
    // Label selector: write straight into the bound WindowData (SAVE
    // propagates it into ScanConfig + the settings file).
    field_label_name_.on_change = [this](size_t, int32_t v) {
        if (bound_data_ == nullptr) return;
        if (label_slot_ >= DETECTION_WINDOWS_PER_WINDOW) return;
        bound_data_->name_idx[label_slot_] =
            (v >= 0 && v < static_cast<int32_t>(RANGE_NAME_COUNT))
                ? static_cast<uint8_t>(v) : 0U;
    };
}

void SweepWindowView::focus() {
    field_start_.focus();
}

void SweepWindowView::bind(WindowData* data, uint8_t window_index) noexcept {
    bound_data_ = data;
    bound_index_ = window_index;
    if (data != nullptr) {
        char label_buf[20];
        // 12 chars → x0..95: must stay clear of the "From" header at x104.
        snprintf(label_buf, sizeof(label_buf), "-- Win %d --", static_cast<int>(window_index) + 1);
        labels_.set_labels({
            {{UI_POS_X(0), UI_POS_Y(0)}, label_buf, Color::white()},
            {{UI_POS_X(0), UI_POS_Y(1)}, "Start", Color::white()},
            {{UI_POS_X(0), UI_POS_Y(2)}, "End", Color::white()},
        });
        sync_to_widgets();
    }
}

void SweepWindowView::sync_to_widgets() noexcept {
    if (bound_data_ == nullptr) return;
    field_start_.set_value(static_cast<int32_t>(bound_data_->start_freq / MHZ));
    field_end_.set_value(static_cast<int32_t>(bound_data_->end_freq / MHZ));
    check_enabled_.set_value(bound_data_->enabled);

    ui::NumberField* dw_start_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_start_, &field_dw1_start_, &field_dw2_start_, &field_dw3_start_, &field_dw4_start_};
    ui::NumberField* dw_end_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_end_, &field_dw1_end_, &field_dw2_end_, &field_dw3_end_, &field_dw4_end_};
    for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
        dw_start_fields[i]->set_value(static_cast<int32_t>(bound_data_->det_windows[i].start_hz / MHZ));
        dw_end_fields[i]->set_value(static_cast<int32_t>(bound_data_->det_windows[i].end_hz / MHZ));
    }

    // Label editor — reflect the selected slot's stored label index.
    if (label_slot_ < DETECTION_WINDOWS_PER_WINDOW) {
        field_label_name_.set_by_value(
            static_cast<int32_t>(bound_data_->name_idx[label_slot_]));
    }
}

void SweepWindowView::sync_from_widgets() noexcept {
    if (bound_data_ == nullptr) return;
    bound_data_->start_freq = read_mhz_field(field_start_);
    bound_data_->end_freq = read_mhz_field(field_end_);
    bound_data_->enabled = check_enabled_.value();

    const ui::NumberField* dw_start_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_start_, &field_dw1_start_, &field_dw2_start_, &field_dw3_start_, &field_dw4_start_};
    const ui::NumberField* dw_end_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_end_, &field_dw1_end_, &field_dw2_end_, &field_dw3_end_, &field_dw4_end_};
    for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
        bound_data_->det_windows[i].start_hz =
            static_cast<FreqHz>(dw_start_fields[i]->value()) * MHZ;
        bound_data_->det_windows[i].end_hz =
            static_cast<FreqHz>(dw_end_fields[i]->value()) * MHZ;
    }
}

// ============================================================================
// DroneSweepView — main view with single reusable SweepWindowView
// ============================================================================

DroneSweepView::DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , sweep_view_(nav, Rect{0, UI_POS_Y(2), screen_width, screen_height - UI_POS_Y(3)}, scanner_ptr)
    , selected_window_(0) {
    add_children({
        &field_window_select_,
        &sweep_view_,
        &button_defaults_,
        &button_save_,
    });

    // Load config into windows_[] data array
    windows_[0].start_freq = config.sweep_start_freq;
    windows_[0].end_freq = config.sweep_end_freq;
    windows_[0].step_freq = config.sweep_step_freq;
    windows_[0].enabled = true;  // Window 1 always enabled
    windows_[1].start_freq = config.sweep2_start_freq;
    windows_[1].end_freq = config.sweep2_end_freq;
    windows_[1].step_freq = config.sweep2_step_freq;
    windows_[1].enabled = config.sweep2_enabled;
    windows_[2].start_freq = config.sweep3_start_freq;
    windows_[2].end_freq = config.sweep3_end_freq;
    windows_[2].step_freq = config.sweep3_step_freq;
    windows_[2].enabled = config.sweep3_enabled;
    windows_[3].start_freq = config.sweep4_start_freq;
    windows_[3].end_freq = config.sweep4_end_freq;
    windows_[3].step_freq = config.sweep4_step_freq;
    windows_[3].enabled = config.sweep4_enabled;

    // Load 5 detection ranges per window (MHz mirror → UI-side Hz)
    for (uint8_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
        for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
            windows_[w].det_windows[i].start_hz =
                static_cast<FreqHz>(config.sweep_det_win_start_mhz[w][i]) * MHZ;
            windows_[w].det_windows[i].end_hz =
                static_cast<FreqHz>(config.sweep_det_win_end_mhz[w][i]) * MHZ;
            windows_[w].name_idx[i] = config.sweep_det_win_name_idx[w][i];
        }
    }

    // Window selector callback
    field_window_select_.on_change = [this](size_t, int32_t v) {
        switch_to_window(static_cast<uint8_t>(v));
    };

    // Bind to window 0
    sweep_view_.bind(&windows_[0], 0);

    button_save_.on_select = [this](ui::Button&) {
        save_settings();
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        apply_defaults();
    };
}

DroneSweepView::~DroneSweepView() noexcept {
}

void DroneSweepView::focus() {
    // Entry focus: the window selector ("Win 1"). It is the first element of
    // the navigation grid, so the encoder switches windows immediately; Down
    // then reaches Start/End/Enabled and the detection windows predictably.
    field_window_select_.focus();
}

void DroneSweepView::switch_to_window(uint8_t index) noexcept {
    if (index >= NUM_WINDOWS || index == selected_window_) return;

    // Save current widgets → data before switching
    sweep_view_.sync_from_widgets();

    selected_window_ = index;
    sweep_view_.bind(&windows_[index], index);
}

// ============================================================================
// Save / Defaults
// ============================================================================

void DroneSweepView::save_settings() noexcept {
    // Sync widgets → data array
    sweep_view_.sync_from_widgets();

    if (scanner_ptr_ != nullptr) {
        scanner_ptr_->get_config(g_workspace_cfg);

        g_workspace_cfg.sweep_start_freq = windows_[0].start_freq;
        g_workspace_cfg.sweep_end_freq = windows_[0].end_freq;
        g_workspace_cfg.sweep_step_freq = windows_[0].step_freq;

        g_workspace_cfg.sweep2_start_freq = windows_[1].start_freq;
        g_workspace_cfg.sweep2_end_freq = windows_[1].end_freq;
        g_workspace_cfg.sweep2_step_freq = windows_[1].step_freq;
        g_workspace_cfg.sweep2_enabled = windows_[1].enabled;

        g_workspace_cfg.sweep3_start_freq = windows_[2].start_freq;
        g_workspace_cfg.sweep3_end_freq = windows_[2].end_freq;
        g_workspace_cfg.sweep3_step_freq = windows_[2].step_freq;
        g_workspace_cfg.sweep3_enabled = windows_[2].enabled;

        g_workspace_cfg.sweep4_start_freq = windows_[3].start_freq;
        g_workspace_cfg.sweep4_end_freq = windows_[3].end_freq;
        g_workspace_cfg.sweep4_step_freq = windows_[3].step_freq;
        g_workspace_cfg.sweep4_enabled = windows_[3].enabled;

        // Detection ranges (UI-side Hz → MHz mirror in ScanConfig)
        for (uint8_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
            for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
                g_workspace_cfg.sweep_det_win_start_mhz[w][i] = static_cast<uint32_t>(
                    windows_[w].det_windows[i].start_hz / MHZ);
                g_workspace_cfg.sweep_det_win_end_mhz[w][i] = static_cast<uint32_t>(
                    windows_[w].det_windows[i].end_hz / MHZ);
                g_workspace_cfg.sweep_det_win_name_idx[w][i] = windows_[w].name_idx[i];
            }
        }

        (void)scanner_ptr_->set_config(g_workspace_cfg);
    }

    SettingsFileManager::extract_from_config(g_workspace_cfg, g_workspace_settings);
    (void)SettingsFileManager::save(scanner_ptr_, g_workspace_settings);
}

void DroneSweepView::apply_defaults() noexcept {
    // Flash: ~368 bytes (.rodata) — zero stack cost
    static const ScanConfig defaults{};
    windows_[0].start_freq = defaults.sweep_start_freq;
    windows_[0].end_freq = defaults.sweep_end_freq;
    windows_[0].step_freq = defaults.sweep_step_freq;
    windows_[0].enabled = true;
    windows_[1].start_freq = defaults.sweep2_start_freq;
    windows_[1].end_freq = defaults.sweep2_end_freq;
    windows_[1].step_freq = defaults.sweep2_step_freq;
    windows_[1].enabled = false;
    windows_[2].start_freq = defaults.sweep3_start_freq;
    windows_[2].end_freq = defaults.sweep3_end_freq;
    windows_[2].step_freq = defaults.sweep3_step_freq;
    windows_[2].enabled = false;
    windows_[3].start_freq = defaults.sweep4_start_freq;
    windows_[3].end_freq = defaults.sweep4_end_freq;
    windows_[3].step_freq = defaults.sweep4_step_freq;
    windows_[3].enabled = false;

    // Zero all detection ranges → every sweep window detects its full range.
    // Labels are reset too (0 = no label → "Unknown" fallback on the list).
    for (uint8_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
        for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
            windows_[w].det_windows[i] = FreqRangeHz{};
            windows_[w].name_idx[i] = 0;
        }
    }

    sweep_view_.sync_to_widgets();
}

} // namespace drone_analyzer
