#include <cstdint>
#include <cstring>

#include "drone_sweep_view.hpp"
#include "drone_settings.hpp"
#include "settings_manager.hpp"
#include "scanner.hpp"
#include "constants.hpp"
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
static FreqHz read_khz_field(const ui::NumberField& field) noexcept {
    return static_cast<FreqHz>(field.value()) * KHZ;
}

// ============================================================================
// Sweep field ID → config mapping (for FrequencyKeypadView callbacks)
// ============================================================================

enum class SweepFieldID : uint8_t {
    W1_START, W1_END,
    W2_START, W2_END,
    W3_START, W3_END,
    W4_START, W4_END,
    W1_EXC0, W1_EXC1, W1_EXC2, W1_EXC3, W1_EXC4,
    W2_EXC0, W2_EXC1, W2_EXC2, W2_EXC3, W2_EXC4,
    W3_EXC0, W3_EXC1, W3_EXC2, W3_EXC3, W3_EXC4,
    W4_EXC0, W4_EXC1, W4_EXC2, W4_EXC3, W4_EXC4,
};

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
        case SweepFieldID::W1_EXC0: g_workspace_cfg.sweep_exceptions[0][0] = value; break;
        case SweepFieldID::W1_EXC1: g_workspace_cfg.sweep_exceptions[0][1] = value; break;
        case SweepFieldID::W1_EXC2: g_workspace_cfg.sweep_exceptions[0][2] = value; break;
        case SweepFieldID::W1_EXC3: g_workspace_cfg.sweep_exceptions[0][3] = value; break;
        case SweepFieldID::W1_EXC4: g_workspace_cfg.sweep_exceptions[0][4] = value; break;
        case SweepFieldID::W2_EXC0: g_workspace_cfg.sweep_exceptions[1][0] = value; break;
        case SweepFieldID::W2_EXC1: g_workspace_cfg.sweep_exceptions[1][1] = value; break;
        case SweepFieldID::W2_EXC2: g_workspace_cfg.sweep_exceptions[1][2] = value; break;
        case SweepFieldID::W2_EXC3: g_workspace_cfg.sweep_exceptions[1][3] = value; break;
        case SweepFieldID::W2_EXC4: g_workspace_cfg.sweep_exceptions[1][4] = value; break;
        case SweepFieldID::W3_EXC0: g_workspace_cfg.sweep_exceptions[2][0] = value; break;
        case SweepFieldID::W3_EXC1: g_workspace_cfg.sweep_exceptions[2][1] = value; break;
        case SweepFieldID::W3_EXC2: g_workspace_cfg.sweep_exceptions[2][2] = value; break;
        case SweepFieldID::W3_EXC3: g_workspace_cfg.sweep_exceptions[2][3] = value; break;
        case SweepFieldID::W3_EXC4: g_workspace_cfg.sweep_exceptions[2][4] = value; break;
        case SweepFieldID::W4_EXC0: g_workspace_cfg.sweep_exceptions[3][0] = value; break;
        case SweepFieldID::W4_EXC1: g_workspace_cfg.sweep_exceptions[3][1] = value; break;
        case SweepFieldID::W4_EXC2: g_workspace_cfg.sweep_exceptions[3][2] = value; break;
        case SweepFieldID::W4_EXC3: g_workspace_cfg.sweep_exceptions[3][3] = value; break;
        case SweepFieldID::W4_EXC4: g_workspace_cfg.sweep_exceptions[3][4] = value; break;
    }
}

// Map (window_index, exc_slot) → SweepFieldID
static SweepFieldID exc_field_id(uint8_t w, uint8_t s) noexcept {
    constexpr SweepFieldID table[4][5] = {
        {SweepFieldID::W1_EXC0, SweepFieldID::W1_EXC1, SweepFieldID::W1_EXC2, SweepFieldID::W1_EXC3, SweepFieldID::W1_EXC4},
        {SweepFieldID::W2_EXC0, SweepFieldID::W2_EXC1, SweepFieldID::W2_EXC2, SweepFieldID::W2_EXC3, SweepFieldID::W2_EXC4},
        {SweepFieldID::W3_EXC0, SweepFieldID::W3_EXC1, SweepFieldID::W3_EXC2, SweepFieldID::W3_EXC3, SweepFieldID::W3_EXC4},
        {SweepFieldID::W4_EXC0, SweepFieldID::W4_EXC1, SweepFieldID::W4_EXC2, SweepFieldID::W4_EXC3, SweepFieldID::W4_EXC4},
    };
    return (w < 4 && s < 5) ? table[w][s] : SweepFieldID::W1_START;
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
// Helper: open frequency keypad with nav.replace()
// ============================================================================
static void open_freq_keypad_replace(
    NavigationView& nav,
    SweepFieldID field_id,
    FreqHz initial_hz,
    DroneScanner* scanner) noexcept {
    baseband::spectrum_streaming_stop();
    auto* new_view = nav.replace<FrequencyKeypadView>(
        static_cast<rf::Frequency>(initial_hz));
    new_view->on_changed = [field_id, scanner](rf::Frequency f) {
        set_config_field_by_id(field_id, f);
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
    add_children({
        &labels_,
        &check_enabled_,
        &field_start_,
        &field_end_,
        &field_step_,
        &labels_exc_,
        &field_exc0_,
        &field_exc1_,
        &field_exc2_,
        &field_exc3_,
        &field_exc4_,
    });

    // on_select callbacks route through the bound window index
    field_start_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, start_field_id(bound_index_),
            static_cast<FreqHz>(field_start_.value()) * MHZ, scanner_ptr_);
    };
    field_end_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, end_field_id(bound_index_),
            static_cast<FreqHz>(field_end_.value()) * MHZ, scanner_ptr_);
    };

    // Exception fields
    field_exc0_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, exc_field_id(bound_index_, 0),
            static_cast<FreqHz>(field_exc0_.value()) * MHZ, scanner_ptr_);
    };
    field_exc1_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, exc_field_id(bound_index_, 1),
            static_cast<FreqHz>(field_exc1_.value()) * MHZ, scanner_ptr_);
    };
    field_exc2_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, exc_field_id(bound_index_, 2),
            static_cast<FreqHz>(field_exc2_.value()) * MHZ, scanner_ptr_);
    };
    field_exc3_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, exc_field_id(bound_index_, 3),
            static_cast<FreqHz>(field_exc3_.value()) * MHZ, scanner_ptr_);
    };
    field_exc4_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, exc_field_id(bound_index_, 4),
            static_cast<FreqHz>(field_exc4_.value()) * MHZ, scanner_ptr_);
    };
}

void SweepWindowView::focus() {
    field_start_.focus();
}

void SweepWindowView::bind(WindowData* data, uint8_t window_index) noexcept {
    bound_data_ = data;
    bound_index_ = window_index;
    if (data != nullptr) {
        // Update label to show window number
        char label_buf[20];
        snprintf(label_buf, sizeof(label_buf), "-- Window %d --", static_cast<int>(window_index) + 1);
        labels_.set(0, label_buf);
        sync_to_widgets();
    }
}

void SweepWindowView::sync_to_widgets() noexcept {
    if (bound_data_ == nullptr) return;
    field_start_.set_value(static_cast<int32_t>(bound_data_->start_freq / MHZ));
    field_end_.set_value(static_cast<int32_t>(bound_data_->end_freq / MHZ));
    field_step_.set_value(static_cast<int32_t>(bound_data_->step_freq / KHZ));
    check_enabled_.set_value(bound_data_->enabled);

    ui::NumberField* exc_fields[5] = {&field_exc0_, &field_exc1_, &field_exc2_, &field_exc3_, &field_exc4_};
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        exc_fields[i]->set_value(static_cast<int32_t>(bound_data_->exceptions[i] / MHZ));
    }
}

void SweepWindowView::sync_from_widgets() noexcept {
    if (bound_data_ == nullptr) return;
    bound_data_->start_freq = read_mhz_field(field_start_);
    bound_data_->end_freq = read_mhz_field(field_end_);
    bound_data_->step_freq = read_khz_field(field_step_);
    bound_data_->enabled = check_enabled_.value();

    const ui::NumberField* exc_fields[5] = {&field_exc0_, &field_exc1_, &field_exc2_, &field_exc3_, &field_exc4_};
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        bound_data_->exceptions[i] = static_cast<FreqHz>(exc_fields[i]->value()) * MHZ;
    }
}

// ============================================================================
// DroneSweepView — main view with single reusable SweepWindowView
// ============================================================================

DroneSweepView::DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , sweep_view_(nav, Rect{0, 24, screen_width, screen_height - 48}, scanner_ptr)
    , selected_window_(0) {
    add_children({
        &field_window_select_,
        &sweep_view_,
        &labels_exc_radius_,
        &field_exc_radius_,
        &button_defaults_,
        &button_save_,
    });

    // Load config into windows_[] data array
    windows_[0].start_freq = config.sweep_start_freq;
    windows_[0].end_freq = config.sweep_end_freq;
    windows_[0].step_freq = config.sweep_step_freq;
    windows_[0].enabled = true;  // Window 1 always enabled
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        windows_[0].exceptions[i] = config.sweep_exceptions[0][i];
    }

    windows_[1].start_freq = config.sweep2_start_freq;
    windows_[1].end_freq = config.sweep2_end_freq;
    windows_[1].step_freq = config.sweep2_step_freq;
    windows_[1].enabled = config.sweep2_enabled;
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        windows_[1].exceptions[i] = config.sweep_exceptions[1][i];
    }

    windows_[2].start_freq = config.sweep3_start_freq;
    windows_[2].end_freq = config.sweep3_end_freq;
    windows_[2].step_freq = config.sweep3_step_freq;
    windows_[2].enabled = config.sweep3_enabled;
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        windows_[2].exceptions[i] = config.sweep_exceptions[2][i];
    }

    windows_[3].start_freq = config.sweep4_start_freq;
    windows_[3].end_freq = config.sweep4_end_freq;
    windows_[3].step_freq = config.sweep4_step_freq;
    windows_[3].enabled = config.sweep4_enabled;
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        windows_[3].exceptions[i] = config.sweep_exceptions[3][i];
    }

    field_exc_radius_.set_value(static_cast<int32_t>(config.exception_radius_mhz));

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
    sweep_view_.focus();
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

    const uint8_t exc_radius = static_cast<uint8_t>(field_exc_radius_.value());

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

        for (uint8_t w = 0; w < 4; ++w) {
            for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
                g_workspace_cfg.sweep_exceptions[w][i] = windows_[w].exceptions[i];
            }
        }

        g_workspace_cfg.exception_radius_mhz = exc_radius;

        (void)scanner_ptr_->set_config(g_workspace_cfg);
    }

    SettingsFileManager::extract_from_config(g_workspace_cfg, g_workspace_settings);
    (void)SettingsFileManager::save(scanner_ptr_, g_workspace_settings);
}

void DroneSweepView::apply_defaults() noexcept {
    ScanConfig defaults;
    windows_[0].start_freq = defaults.sweep_start_freq;
    windows_[0].end_freq = defaults.sweep_end_freq;
    windows_[0].step_freq = defaults.sweep_step_freq;
    windows_[0].enabled = true;
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) windows_[0].exceptions[i] = 0;

    windows_[1].start_freq = defaults.sweep2_start_freq;
    windows_[1].end_freq = defaults.sweep2_end_freq;
    windows_[1].step_freq = defaults.sweep2_step_freq;
    windows_[1].enabled = false;
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) windows_[1].exceptions[i] = 0;

    windows_[2].start_freq = defaults.sweep3_start_freq;
    windows_[2].end_freq = defaults.sweep3_end_freq;
    windows_[2].step_freq = defaults.sweep3_step_freq;
    windows_[2].enabled = false;
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) windows_[2].exceptions[i] = 0;

    windows_[3].start_freq = defaults.sweep4_start_freq;
    windows_[3].end_freq = defaults.sweep4_end_freq;
    windows_[3].step_freq = defaults.sweep4_step_freq;
    windows_[3].enabled = false;
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) windows_[3].exceptions[i] = 0;

    sweep_view_.sync_to_widgets();
    field_exc_radius_.set_value(DEFAULT_EXCEPTION_RADIUS_MHZ);
}

} // namespace drone_analyzer
