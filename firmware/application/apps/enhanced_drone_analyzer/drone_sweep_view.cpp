#include <cstdint>
#include <cstring>

#include "drone_sweep_view.hpp"
#include "settings_manager.hpp"
#include "scanner.hpp"
#include "constants.hpp"
#include "ui_receiver.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "baseband_api.hpp"

namespace drone_analyzer {

// ============================================================================
// Helper: push FrequencyKeypadView for any NumberField
// ============================================================================
static void setup_freq_keypad(NavigationView& nav, NumberField& field) noexcept {
    field.on_select = [&nav, &field](NumberField&) {
        baseband::spectrum_streaming_stop();
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field.value()) * 1000000ULL);
        new_view->on_changed = [&field](rf::Frequency f) {
            field.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };
}

// ============================================================================
// SweepWindowGroupView — shared implementation for both tab pages
// ============================================================================

SweepWindowGroupView::SweepWindowGroupView(
    NavigationView& nav, Rect parent_rect, uint8_t window_index) noexcept
    : ui::View()
    , nav_(nav)
    , window_index_(window_index)
    , labels_a_({
        {{UI_POS_X(0), UI_POS_Y(0)}, (window_index == 0) ? "-- Window 1 --" : "-- Window 3 --", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(1)}, "Start(MHz):", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(3)}, "End(MHz):", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(5)}, "Step(kHz):", Color::white()},
    })
    , check_a_enabled_{{UI_POS_X(1), UI_POS_Y(1)}, 8, "Enabled", false}
    , field_a_start_{{UI_POS_X(1), UI_POS_Y(2)}, 5, {100, 7200}, 1, ' '}
    , field_a_end_{{UI_POS_X(1), UI_POS_Y(4)}, 5, {100, 7200}, 1, ' '}
    , field_a_step_{{UI_POS_X(1), UI_POS_Y(6)}, 5, {17813, 99999}, 17813, ' '}
    , labels_b_({
        {{UI_POS_X(0), UI_POS_Y(8)}, (window_index == 0) ? "-- Window 2 --" : "-- Window 4 --", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(10)}, "Start(MHz):", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(12)}, "End(MHz):", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(14)}, "Step(kHz):", Color::white()},
    })
    , check_b_enabled_{{UI_POS_X(1), UI_POS_Y(9)}, 8, "Enabled", false}
    , field_b_start_{{UI_POS_X(1), UI_POS_Y(11)}, 5, {100, 7200}, 1, ' '}
    , field_b_end_{{UI_POS_X(1), UI_POS_Y(13)}, 5, {100, 7200}, 1, ' '}
    , field_b_step_{{UI_POS_X(1), UI_POS_Y(15)}, 5, {17813, 99999}, 17813, ' '}
    , labels_exc_a_{{{UI_POS_X(16), UI_POS_Y(0)}, "Exc(MHz):", Color::white()}}
    , field_exc_a0_{{UI_POS_X(16), UI_POS_Y(1)}, 5, {0, 7200}, 1, ' '}
    , field_exc_a1_{{UI_POS_X(16), UI_POS_Y(2)}, 5, {0, 7200}, 1, ' '}
    , field_exc_a2_{{UI_POS_X(16), UI_POS_Y(3)}, 5, {0, 7200}, 1, ' '}
    , field_exc_a3_{{UI_POS_X(16), UI_POS_Y(4)}, 5, {0, 7200}, 1, ' '}
    , field_exc_a4_{{UI_POS_X(16), UI_POS_Y(5)}, 5, {0, 7200}, 1, ' '}
    , labels_exc_b_{{{UI_POS_X(16), UI_POS_Y(8)}, "Exc(MHz):", Color::white()}}
    , field_exc_b0_{{UI_POS_X(16), UI_POS_Y(9)}, 5, {0, 7200}, 1, ' '}
    , field_exc_b1_{{UI_POS_X(16), UI_POS_Y(10)}, 5, {0, 7200}, 1, ' '}
    , field_exc_b2_{{UI_POS_X(16), UI_POS_Y(11)}, 5, {0, 7200}, 1, ' '}
    , field_exc_b3_{{UI_POS_X(16), UI_POS_Y(12)}, 5, {0, 7200}, 1, ' '}
    , field_exc_b4_{{UI_POS_X(16), UI_POS_Y(13)}, 5, {0, 7200}, 1, ' '}
    {

    set_parent_rect(parent_rect);
    add_children({
        &labels_a_,
        &field_a_start_, &field_a_end_, &field_a_step_,
        &labels_b_, &check_b_enabled_, &field_b_start_, &field_b_end_, &field_b_step_,
        &labels_exc_a_,
        &field_exc_a0_, &field_exc_a1_, &field_exc_a2_, &field_exc_a3_, &field_exc_a4_,
        &labels_exc_b_,
        &field_exc_b0_, &field_exc_b1_, &field_exc_b2_, &field_exc_b3_, &field_exc_b4_,
    });
    if (window_index_ == 1) {
        add_children({&check_a_enabled_});
    } else {
        check_a_enabled_.hidden(true);
    }

    // Single helper replaces 24 identical lambdas
    setup_freq_keypad(nav_, field_a_start_);
    setup_freq_keypad(nav_, field_a_end_);
    setup_freq_keypad(nav_, field_b_start_);
    setup_freq_keypad(nav_, field_b_end_);
    setup_freq_keypad(nav_, field_exc_a0_);
    setup_freq_keypad(nav_, field_exc_a1_);
    setup_freq_keypad(nav_, field_exc_a2_);
    setup_freq_keypad(nav_, field_exc_a3_);
    setup_freq_keypad(nav_, field_exc_a4_);
    setup_freq_keypad(nav_, field_exc_b0_);
    setup_freq_keypad(nav_, field_exc_b1_);
    setup_freq_keypad(nav_, field_exc_b2_);
    setup_freq_keypad(nav_, field_exc_b3_);
    setup_freq_keypad(nav_, field_exc_b4_);
}

void SweepWindowGroupView::focus() {
    field_a_start_.focus();
}

NumberField& SweepWindowGroupView::field_start(uint8_t w) noexcept {
    return (w == 0) ? field_a_start_ : field_b_start_;
}
NumberField& SweepWindowGroupView::field_end(uint8_t w) noexcept {
    return (w == 0) ? field_a_end_ : field_b_end_;
}
NumberField& SweepWindowGroupView::field_step(uint8_t w) noexcept {
    return (w == 0) ? field_a_step_ : field_b_step_;
}
Checkbox& SweepWindowGroupView::check_enabled(uint8_t w) noexcept {
    return (w == 0) ? check_a_enabled_ : check_b_enabled_;
}
NumberField& SweepWindowGroupView::field_exc(uint8_t w, uint8_t slot) noexcept {
    if (w == 0) {
        if (slot == 0) return field_exc_a0_;
        if (slot == 1) return field_exc_a1_;
        if (slot == 2) return field_exc_a2_;
        if (slot == 3) return field_exc_a3_;
        return field_exc_a4_;
    }
    if (slot == 0) return field_exc_b0_;
    if (slot == 1) return field_exc_b1_;
    if (slot == 2) return field_exc_b2_;
    if (slot == 3) return field_exc_b3_;
    return field_exc_b4_;
}

// ============================================================================
// DroneSweepView
// ============================================================================

DroneSweepView::DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , original_config_(config)
    , view_group1_(nav_, Rect{0, TAB_BAR_H, screen_width, screen_height - TAB_BAR_H}, 0)
    , view_group2_(nav_, Rect{0, TAB_BAR_H, screen_width, screen_height - TAB_BAR_H}, 1)
    , tab_view_({
        {"Win 1-2", Color::white(), &view_group1_},
        {"Win 3-4", Color::white(), &view_group2_}
    }) {
    view_group2_.hidden(true);
    add_children({&tab_view_, &view_group1_, &view_group2_, &labels_exc_radius_, &field_exc_radius_, &button_defaults_, &button_save_});
    tab_view_.set_selected(0);

    populate_from_config(config);

    button_save_.on_select = [this](ui::Button&) {
        save_settings();
        nav_.pop();
    };
    button_defaults_.on_select = [this](ui::Button&) {
        apply_defaults();
    };
}

DroneSweepView::~DroneSweepView() noexcept {}

void DroneSweepView::focus() {
    (tab_view_.selected() == 0) ? view_group1_.focus() : view_group2_.focus();
}

void DroneSweepView::populate_from_config(const ScanConfig& cfg) noexcept {
    auto set_mhz = [](NumberField& f, FreqHz hz) {
        f.set_value(static_cast<int32_t>(hz / 1000000ULL));
    };
    auto set_khz = [](NumberField& f, FreqHz hz) {
        f.set_value(static_cast<int32_t>(hz / 1000ULL));
    };

    // Group1: windows 0,1
    set_mhz(view_group1_.field_start(0), cfg.sweep_start_freq);
    set_mhz(view_group1_.field_end(0), cfg.sweep_end_freq);
    set_khz(view_group1_.field_step(0), cfg.sweep_step_freq);
    view_group1_.check_enabled(1).set_value(cfg.sweep2_enabled);
    set_mhz(view_group1_.field_start(1), cfg.sweep2_start_freq);
    set_mhz(view_group1_.field_end(1), cfg.sweep2_end_freq);
    set_khz(view_group1_.field_step(1), cfg.sweep2_step_freq);

    // Group2: windows 2,3
    view_group2_.check_enabled(0).set_value(cfg.sweep3_enabled);
    set_mhz(view_group2_.field_start(0), cfg.sweep3_start_freq);
    set_mhz(view_group2_.field_end(0), cfg.sweep3_end_freq);
    set_khz(view_group2_.field_step(0), cfg.sweep3_step_freq);
    view_group2_.check_enabled(1).set_value(cfg.sweep4_enabled);
    set_mhz(view_group2_.field_start(1), cfg.sweep4_start_freq);
    set_mhz(view_group2_.field_end(1), cfg.sweep4_end_freq);
    set_khz(view_group2_.field_step(1), cfg.sweep4_step_freq);

    // Exceptions (4 windows × 5 slots)
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        set_mhz(view_group1_.field_exc(0, i), cfg.sweep_exceptions[0][i]);
        set_mhz(view_group1_.field_exc(1, i), cfg.sweep_exceptions[1][i]);
        set_mhz(view_group2_.field_exc(0, i), cfg.sweep_exceptions[2][i]);
        set_mhz(view_group2_.field_exc(1, i), cfg.sweep_exceptions[3][i]);
    }
    field_exc_radius_.set_value(static_cast<int32_t>(cfg.exception_radius_mhz));
}

void DroneSweepView::save_settings() noexcept {
    auto mhz = [](NumberField& f) -> FreqHz {
        return static_cast<FreqHz>(f.value()) * 1000000ULL;
    };
    auto khz = [](NumberField& f) -> FreqHz {
        return static_cast<FreqHz>(f.value()) * 1000ULL;
    };

    static ScanConfig cfg;
    cfg = original_config_;
    cfg.sweep_start_freq  = mhz(view_group1_.field_start(0));
    cfg.sweep_end_freq    = mhz(view_group1_.field_end(0));
    cfg.sweep_step_freq   = khz(view_group1_.field_step(0));
    cfg.sweep2_start_freq = mhz(view_group1_.field_start(1));
    cfg.sweep2_end_freq   = mhz(view_group1_.field_end(1));
    cfg.sweep2_step_freq  = khz(view_group1_.field_step(1));
    cfg.sweep2_enabled    = view_group1_.check_enabled(1).value();
    cfg.sweep3_start_freq = mhz(view_group2_.field_start(0));
    cfg.sweep3_end_freq   = mhz(view_group2_.field_end(0));
    cfg.sweep3_step_freq  = khz(view_group2_.field_step(0));
    cfg.sweep3_enabled    = view_group2_.check_enabled(0).value();
    cfg.sweep4_start_freq = mhz(view_group2_.field_start(1));
    cfg.sweep4_end_freq   = mhz(view_group2_.field_end(1));
    cfg.sweep4_step_freq  = khz(view_group2_.field_step(1));
    cfg.sweep4_enabled    = view_group2_.check_enabled(1).value();

    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        cfg.sweep_exceptions[0][i] = mhz(view_group1_.field_exc(0, i));
        cfg.sweep_exceptions[1][i] = mhz(view_group1_.field_exc(1, i));
        cfg.sweep_exceptions[2][i] = mhz(view_group2_.field_exc(0, i));
        cfg.sweep_exceptions[3][i] = mhz(view_group2_.field_exc(1, i));
    }
    cfg.exception_radius_mhz = static_cast<uint8_t>(field_exc_radius_.value());

    // Clamp ranges
    auto clamp_win = [](FreqHz& s, FreqHz& e) {
        if (s < HARDWARE_MIN_FREQ_HZ) s = HARDWARE_MIN_FREQ_HZ;
        if (e > HARDWARE_MAX_FREQ_HZ) e = HARDWARE_MAX_FREQ_HZ;
        if (s >= e) e = s + 20000000;
    };
    clamp_win(cfg.sweep_start_freq, cfg.sweep_end_freq);
    if (cfg.sweep2_enabled) clamp_win(cfg.sweep2_start_freq, cfg.sweep2_end_freq);
    if (cfg.sweep3_enabled) clamp_win(cfg.sweep3_start_freq, cfg.sweep3_end_freq);
    if (cfg.sweep4_enabled) clamp_win(cfg.sweep4_start_freq, cfg.sweep4_end_freq);

    if (scanner_ptr_ != nullptr) {
        (void)scanner_ptr_->set_config(cfg);
    }

    // Save to SD
    static SettingsStruct current;
    (void)SettingsFileManager::load(current);
    current.sweep_start_freq = cfg.sweep_start_freq;
    current.sweep_end_freq = cfg.sweep_end_freq;
    current.sweep_step_freq = cfg.sweep_step_freq;
    current.sweep2_start_freq = cfg.sweep2_start_freq;
    current.sweep2_end_freq = cfg.sweep2_end_freq;
    current.sweep2_step_freq = cfg.sweep2_step_freq;
    current.sweep2_enabled = cfg.sweep2_enabled;
    current.sweep3_start_freq = cfg.sweep3_start_freq;
    current.sweep3_end_freq = cfg.sweep3_end_freq;
    current.sweep3_step_freq = cfg.sweep3_step_freq;
    current.sweep3_enabled = cfg.sweep3_enabled;
    current.sweep4_start_freq = cfg.sweep4_start_freq;
    current.sweep4_end_freq = cfg.sweep4_end_freq;
    current.sweep4_step_freq = cfg.sweep4_step_freq;
    current.sweep4_enabled = cfg.sweep4_enabled;
    for (uint8_t w = 0; w < 4; ++w) {
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            current.sweep_exceptions[w][i] = cfg.sweep_exceptions[w][i];
        }
    }
    current.exception_radius_mhz = cfg.exception_radius_mhz;
    (void)SettingsFileManager::save(scanner_ptr_, current);
}

void DroneSweepView::apply_defaults() noexcept {
    populate_from_config(ScanConfig{});
}

} // namespace drone_analyzer
