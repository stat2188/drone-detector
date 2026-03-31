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

namespace drone_analyzer {

// ============================================================================
// SweepWindowGroup1View — Tab 1: Windows 1-2
// ============================================================================

SweepWindowGroup1View::SweepWindowGroup1View(NavigationView& nav, const Rect parent_rect) noexcept
    : ui::View() {
    set_parent_rect(parent_rect);
    add_children({
        &labels_,
        &field_sw1_start_,
        &field_sw1_end_,
        &field_sw1_step_,
        &labels_sw2_,
        &check_sw2_enabled_,
        &field_sw2_start_,
        &field_sw2_end_,
        &field_sw2_step_,
        &labels_exc1_,
        &field_sw1_exc0_,
        &field_sw1_exc1_,
        &field_sw1_exc2_,
        &field_sw1_exc3_,
        &field_sw1_exc4_,
        &field_sw1_exc5_,
        &labels_exc2_,
        &field_sw2_exc0_,
        &field_sw2_exc1_,
        &field_sw2_exc2_,
        &field_sw2_exc3_,
        &field_sw2_exc4_,
        &field_sw2_exc5_,
    });

    // Keyboard callbacks for frequency fields (MHz → keypad → MHz)
    field_sw1_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw1_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw1_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw1_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw2_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw2_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw2_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    // Keyboard callbacks for exception fields (MHz → keypad → MHz)
    auto make_exc_callback = [&nav](ui::NumberField& field) {
        return [&nav, &field](NumberField&) {
            auto new_view = nav.push<FrequencyKeypadView>(
                static_cast<rf::Frequency>(field.value()) * 1000000ULL);
            new_view->on_changed = [&field](rf::Frequency f) {
                field.set_value(static_cast<int32_t>(f / 1000000ULL));
            };
        };
    };

    field_sw1_exc0_.on_select = make_exc_callback(field_sw1_exc0_);
    field_sw1_exc1_.on_select = make_exc_callback(field_sw1_exc1_);
    field_sw1_exc2_.on_select = make_exc_callback(field_sw1_exc2_);
    field_sw1_exc3_.on_select = make_exc_callback(field_sw1_exc3_);
    field_sw1_exc4_.on_select = make_exc_callback(field_sw1_exc4_);
    field_sw1_exc5_.on_select = make_exc_callback(field_sw1_exc5_);
    field_sw2_exc0_.on_select = make_exc_callback(field_sw2_exc0_);
    field_sw2_exc1_.on_select = make_exc_callback(field_sw2_exc1_);
    field_sw2_exc2_.on_select = make_exc_callback(field_sw2_exc2_);
    field_sw2_exc3_.on_select = make_exc_callback(field_sw2_exc3_);
    field_sw2_exc4_.on_select = make_exc_callback(field_sw2_exc4_);
    field_sw2_exc5_.on_select = make_exc_callback(field_sw2_exc5_);
}

void SweepWindowGroup1View::focus() {
    field_sw1_start_.focus();
}

// ============================================================================
// SweepWindowGroup2View — Tab 2: Windows 3-4
// ============================================================================

SweepWindowGroup2View::SweepWindowGroup2View(NavigationView& nav, const Rect parent_rect) noexcept
    : ui::View() {
    set_parent_rect(parent_rect);
    add_children({
        &labels_sw3_,
        &check_sw3_enabled_,
        &field_sw3_start_,
        &field_sw3_end_,
        &field_sw3_step_,
        &labels_sw4_,
        &check_sw4_enabled_,
        &field_sw4_start_,
        &field_sw4_end_,
        &field_sw4_step_,
        &labels_exc3_,
        &field_sw3_exc0_,
        &field_sw3_exc1_,
        &field_sw3_exc2_,
        &field_sw3_exc3_,
        &field_sw3_exc4_,
        &field_sw3_exc5_,
        &labels_exc4_,
        &field_sw4_exc0_,
        &field_sw4_exc1_,
        &field_sw4_exc2_,
        &field_sw4_exc3_,
        &field_sw4_exc4_,
        &field_sw4_exc5_,
    });

    // Keyboard callbacks for frequency fields (MHz → keypad → MHz)
    field_sw3_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw3_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw3_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw3_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_start_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_start_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_start_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    field_sw4_end_.on_select = [this, &nav](NumberField&) {
        auto new_view = nav.push<FrequencyKeypadView>(
            static_cast<rf::Frequency>(field_sw4_end_.value()) * 1000000ULL);
        new_view->on_changed = [this](rf::Frequency f) {
            field_sw4_end_.set_value(static_cast<int32_t>(f / 1000000ULL));
        };
    };

    // Keyboard callbacks for exception fields (MHz → keypad → MHz)
    auto make_exc_callback = [&nav](ui::NumberField& field) {
        return [&nav, &field](NumberField&) {
            auto new_view = nav.push<FrequencyKeypadView>(
                static_cast<rf::Frequency>(field.value()) * 1000000ULL);
            new_view->on_changed = [&field](rf::Frequency f) {
                field.set_value(static_cast<int32_t>(f / 1000000ULL));
            };
        };
    };

    field_sw3_exc0_.on_select = make_exc_callback(field_sw3_exc0_);
    field_sw3_exc1_.on_select = make_exc_callback(field_sw3_exc1_);
    field_sw3_exc2_.on_select = make_exc_callback(field_sw3_exc2_);
    field_sw3_exc3_.on_select = make_exc_callback(field_sw3_exc3_);
    field_sw3_exc4_.on_select = make_exc_callback(field_sw3_exc4_);
    field_sw3_exc5_.on_select = make_exc_callback(field_sw3_exc5_);
    field_sw4_exc0_.on_select = make_exc_callback(field_sw4_exc0_);
    field_sw4_exc1_.on_select = make_exc_callback(field_sw4_exc1_);
    field_sw4_exc2_.on_select = make_exc_callback(field_sw4_exc2_);
    field_sw4_exc3_.on_select = make_exc_callback(field_sw4_exc3_);
    field_sw4_exc4_.on_select = make_exc_callback(field_sw4_exc4_);
    field_sw4_exc5_.on_select = make_exc_callback(field_sw4_exc5_);
}

void SweepWindowGroup2View::focus() {
    check_sw3_enabled_.focus();
}

// ============================================================================
// DroneSweepView — Main sweep settings view with TabView
// ============================================================================

DroneSweepView::DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , original_config_(config) {
    // Hide non-active tab view BEFORE adding as children
    // (TabView::set_selected is not called until on_show)
    view_group2_.hidden(true);

    add_children({
        &tab_view_,
        &view_group1_,
        &view_group2_,
        &button_defaults_,
        &button_save_,
    });

    // Initialize tab selection (ensures correct view is visible)
    tab_view_.set_selected(0);

    // Populate Tab 1 (Windows 1-2) from config
    view_group1_.field_sw1_start_.set_value(static_cast<int32_t>(config.sweep_start_freq / 1000000ULL));
    view_group1_.field_sw1_end_.set_value(static_cast<int32_t>(config.sweep_end_freq / 1000000ULL));
    view_group1_.field_sw1_step_.set_value(static_cast<int32_t>(config.sweep_step_freq / 1000ULL));

    view_group1_.check_sw2_enabled_.set_value(config.sweep2_enabled);
    view_group1_.field_sw2_start_.set_value(static_cast<int32_t>(config.sweep2_start_freq / 1000000ULL));
    view_group1_.field_sw2_end_.set_value(static_cast<int32_t>(config.sweep2_end_freq / 1000000ULL));
    view_group1_.field_sw2_step_.set_value(static_cast<int32_t>(config.sweep2_step_freq / 1000ULL));

    // Populate Tab 2 (Windows 3-4) from config
    view_group2_.check_sw3_enabled_.set_value(config.sweep3_enabled);
    view_group2_.field_sw3_start_.set_value(static_cast<int32_t>(config.sweep3_start_freq / 1000000ULL));
    view_group2_.field_sw3_end_.set_value(static_cast<int32_t>(config.sweep3_end_freq / 1000000ULL));
    view_group2_.field_sw3_step_.set_value(static_cast<int32_t>(config.sweep3_step_freq / 1000ULL));

    view_group2_.check_sw4_enabled_.set_value(config.sweep4_enabled);
    view_group2_.field_sw4_start_.set_value(static_cast<int32_t>(config.sweep4_start_freq / 1000000ULL));
    view_group2_.field_sw4_end_.set_value(static_cast<int32_t>(config.sweep4_end_freq / 1000000ULL));
    view_group2_.field_sw4_step_.set_value(static_cast<int32_t>(config.sweep4_step_freq / 1000ULL));

    // Populate exception fields from config (MHz)
    ui::NumberField* exc1_fields[] = {
        &view_group1_.field_sw1_exc0_, &view_group1_.field_sw1_exc1_,
        &view_group1_.field_sw1_exc2_, &view_group1_.field_sw1_exc3_,
        &view_group1_.field_sw1_exc4_, &view_group1_.field_sw1_exc5_};
    ui::NumberField* exc2_fields[] = {
        &view_group1_.field_sw2_exc0_, &view_group1_.field_sw2_exc1_,
        &view_group1_.field_sw2_exc2_, &view_group1_.field_sw2_exc3_,
        &view_group1_.field_sw2_exc4_, &view_group1_.field_sw2_exc5_};
    ui::NumberField* exc3_fields[] = {
        &view_group2_.field_sw3_exc0_, &view_group2_.field_sw3_exc1_,
        &view_group2_.field_sw3_exc2_, &view_group2_.field_sw3_exc3_,
        &view_group2_.field_sw3_exc4_, &view_group2_.field_sw3_exc5_};
    ui::NumberField* exc4_fields[] = {
        &view_group2_.field_sw4_exc0_, &view_group2_.field_sw4_exc1_,
        &view_group2_.field_sw4_exc2_, &view_group2_.field_sw4_exc3_,
        &view_group2_.field_sw4_exc4_, &view_group2_.field_sw4_exc5_};

    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        exc1_fields[i]->set_value(static_cast<int32_t>(config.sweep_exceptions[0][i] / 1000000ULL));
        exc2_fields[i]->set_value(static_cast<int32_t>(config.sweep_exceptions[1][i] / 1000000ULL));
        exc3_fields[i]->set_value(static_cast<int32_t>(config.sweep_exceptions[2][i] / 1000000ULL));
        exc4_fields[i]->set_value(static_cast<int32_t>(config.sweep_exceptions[3][i] / 1000000ULL));
    }

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
    // Focus the current tab's content directly (skip tab headers for fast navigation)
    if (tab_view_.selected() == 0) {
        view_group1_.focus();
    } else {
        view_group2_.focus();
    }
}

void DroneSweepView::save_settings() noexcept {
    // Read values from child views
    FreqHz sw1_start = static_cast<FreqHz>(view_group1_.field_sw1_start_.value()) * 1000000ULL;
    FreqHz sw1_end = static_cast<FreqHz>(view_group1_.field_sw1_end_.value()) * 1000000ULL;
    FreqHz sw1_step = static_cast<FreqHz>(view_group1_.field_sw1_step_.value()) * 1000ULL;

    bool sw2_enabled = view_group1_.check_sw2_enabled_.value();
    FreqHz sw2_start = static_cast<FreqHz>(view_group1_.field_sw2_start_.value()) * 1000000ULL;
    FreqHz sw2_end = static_cast<FreqHz>(view_group1_.field_sw2_end_.value()) * 1000000ULL;
    FreqHz sw2_step = static_cast<FreqHz>(view_group1_.field_sw2_step_.value()) * 1000ULL;

    bool sw3_enabled = view_group2_.check_sw3_enabled_.value();
    FreqHz sw3_start = static_cast<FreqHz>(view_group2_.field_sw3_start_.value()) * 1000000ULL;
    FreqHz sw3_end = static_cast<FreqHz>(view_group2_.field_sw3_end_.value()) * 1000000ULL;
    FreqHz sw3_step = static_cast<FreqHz>(view_group2_.field_sw3_step_.value()) * 1000ULL;

    bool sw4_enabled = view_group2_.check_sw4_enabled_.value();
    FreqHz sw4_start = static_cast<FreqHz>(view_group2_.field_sw4_start_.value()) * 1000000ULL;
    FreqHz sw4_end = static_cast<FreqHz>(view_group2_.field_sw4_end_.value()) * 1000000ULL;
    FreqHz sw4_step = static_cast<FreqHz>(view_group2_.field_sw4_step_.value()) * 1000ULL;

    // Read exception frequencies (MHz → Hz)
    // Static to avoid stack overflow (192 bytes + 96 bytes pointers)
    static FreqHz exc[4][EXCEPTIONS_PER_WINDOW]{};
    static ui::NumberField* exc1_fields[] = {
        &view_group1_.field_sw1_exc0_, &view_group1_.field_sw1_exc1_,
        &view_group1_.field_sw1_exc2_, &view_group1_.field_sw1_exc3_,
        &view_group1_.field_sw1_exc4_, &view_group1_.field_sw1_exc5_};
    static ui::NumberField* exc2_fields[] = {
        &view_group1_.field_sw2_exc0_, &view_group1_.field_sw2_exc1_,
        &view_group1_.field_sw2_exc2_, &view_group1_.field_sw2_exc3_,
        &view_group1_.field_sw2_exc4_, &view_group1_.field_sw2_exc5_};
    static ui::NumberField* exc3_fields[] = {
        &view_group2_.field_sw3_exc0_, &view_group2_.field_sw3_exc1_,
        &view_group2_.field_sw3_exc2_, &view_group2_.field_sw3_exc3_,
        &view_group2_.field_sw3_exc4_, &view_group2_.field_sw3_exc5_};
    static ui::NumberField* exc4_fields[] = {
        &view_group2_.field_sw4_exc0_, &view_group2_.field_sw4_exc1_,
        &view_group2_.field_sw4_exc2_, &view_group2_.field_sw4_exc3_,
        &view_group2_.field_sw4_exc4_, &view_group2_.field_sw4_exc5_};

    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        exc[0][i] = static_cast<FreqHz>(exc1_fields[i]->value()) * 1000000ULL;
        exc[1][i] = static_cast<FreqHz>(exc2_fields[i]->value()) * 1000000ULL;
        exc[2][i] = static_cast<FreqHz>(exc3_fields[i]->value()) * 1000000ULL;
        exc[3][i] = static_cast<FreqHz>(exc4_fields[i]->value()) * 1000000ULL;
    }

    // Validate: start must be < end
    if (sw1_start >= sw1_end) sw1_end = sw1_start + 20000000;
    if (sw2_enabled && sw2_start >= sw2_end) sw2_end = sw2_start + 20000000;
    if (sw3_enabled && sw3_start >= sw3_end) sw3_end = sw3_start + 20000000;
    if (sw4_enabled && sw4_start >= sw4_end) sw4_end = sw4_start + 20000000;

    // Update scanner config in memory
    // Static to avoid stack overflow (~400 bytes ScanConfig)
    if (scanner_ptr_ != nullptr) {
        static ScanConfig updated_config;
        updated_config = original_config_;
        updated_config.sweep_start_freq = sw1_start;
        updated_config.sweep_end_freq = sw1_end;
        updated_config.sweep_step_freq = sw1_step;
        updated_config.sweep2_start_freq = sw2_start;
        updated_config.sweep2_end_freq = sw2_end;
        updated_config.sweep2_step_freq = sw2_step;
        updated_config.sweep2_enabled = sw2_enabled;
        updated_config.sweep3_start_freq = sw3_start;
        updated_config.sweep3_end_freq = sw3_end;
        updated_config.sweep3_step_freq = sw3_step;
        updated_config.sweep3_enabled = sw3_enabled;
        updated_config.sweep4_start_freq = sw4_start;
        updated_config.sweep4_end_freq = sw4_end;
        updated_config.sweep4_step_freq = sw4_step;
        updated_config.sweep4_enabled = sw4_enabled;
        for (uint8_t w = 0; w < 4; ++w) {
            for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
                updated_config.sweep_exceptions[w][i] = exc[w][i];
            }
        }
        (void)scanner_ptr_->set_config(updated_config);
    }

    // Save to SD card via centralized settings manager
    // Read current settings first (preserves non-sweep settings)
    // Static to avoid stack overflow (~200 bytes SettingsStruct)
    static SettingsStruct current;
    (void)SettingsFileManager::load(current);

    // Update sweep fields from UI
    current.sweep_start_freq = sw1_start;
    current.sweep_end_freq = sw1_end;
    current.sweep_step_freq = sw1_step;
    current.sweep2_start_freq = sw2_start;
    current.sweep2_end_freq = sw2_end;
    current.sweep2_step_freq = sw2_step;
    current.sweep2_enabled = sw2_enabled;
    current.sweep3_start_freq = sw3_start;
    current.sweep3_end_freq = sw3_end;
    current.sweep3_step_freq = sw3_step;
    current.sweep3_enabled = sw3_enabled;
    current.sweep4_start_freq = sw4_start;
    current.sweep4_end_freq = sw4_end;
    current.sweep4_step_freq = sw4_step;
    current.sweep4_enabled = sw4_enabled;
    for (uint8_t w = 0; w < 4; ++w) {
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            current.sweep_exceptions[w][i] = exc[w][i];
        }
    }

    (void)SettingsFileManager::save(scanner_ptr_, current);
}

void DroneSweepView::apply_defaults() noexcept {
    SettingsStruct defaults;

    view_group1_.field_sw1_start_.set_value(static_cast<int32_t>(defaults.sweep_start_freq / 1000000ULL));
    view_group1_.field_sw1_end_.set_value(static_cast<int32_t>(defaults.sweep_end_freq / 1000000ULL));
    view_group1_.field_sw1_step_.set_value(static_cast<int32_t>(defaults.sweep_step_freq / 1000ULL));

    view_group1_.check_sw2_enabled_.set_value(defaults.sweep2_enabled);
    view_group1_.field_sw2_start_.set_value(static_cast<int32_t>(defaults.sweep2_start_freq / 1000000ULL));
    view_group1_.field_sw2_end_.set_value(static_cast<int32_t>(defaults.sweep2_end_freq / 1000000ULL));
    view_group1_.field_sw2_step_.set_value(static_cast<int32_t>(defaults.sweep2_step_freq / 1000ULL));

    view_group2_.check_sw3_enabled_.set_value(defaults.sweep3_enabled);
    view_group2_.field_sw3_start_.set_value(static_cast<int32_t>(defaults.sweep3_start_freq / 1000000ULL));
    view_group2_.field_sw3_end_.set_value(static_cast<int32_t>(defaults.sweep3_end_freq / 1000000ULL));
    view_group2_.field_sw3_step_.set_value(static_cast<int32_t>(defaults.sweep3_step_freq / 1000ULL));

    view_group2_.check_sw4_enabled_.set_value(defaults.sweep4_enabled);
    view_group2_.field_sw4_start_.set_value(static_cast<int32_t>(defaults.sweep4_start_freq / 1000000ULL));
    view_group2_.field_sw4_end_.set_value(static_cast<int32_t>(defaults.sweep4_end_freq / 1000000ULL));
    view_group2_.field_sw4_step_.set_value(static_cast<int32_t>(defaults.sweep4_step_freq / 1000ULL));

    // Reset all exception fields to 0 (disabled)
    ui::NumberField* all_exc[] = {
        &view_group1_.field_sw1_exc0_, &view_group1_.field_sw1_exc1_,
        &view_group1_.field_sw1_exc2_, &view_group1_.field_sw1_exc3_,
        &view_group1_.field_sw1_exc4_, &view_group1_.field_sw1_exc5_,
        &view_group1_.field_sw2_exc0_, &view_group1_.field_sw2_exc1_,
        &view_group1_.field_sw2_exc2_, &view_group1_.field_sw2_exc3_,
        &view_group1_.field_sw2_exc4_, &view_group1_.field_sw2_exc5_,
        &view_group2_.field_sw3_exc0_, &view_group2_.field_sw3_exc1_,
        &view_group2_.field_sw3_exc2_, &view_group2_.field_sw3_exc3_,
        &view_group2_.field_sw3_exc4_, &view_group2_.field_sw3_exc5_,
        &view_group2_.field_sw4_exc0_, &view_group2_.field_sw4_exc1_,
        &view_group2_.field_sw4_exc2_, &view_group2_.field_sw4_exc3_,
        &view_group2_.field_sw4_exc4_, &view_group2_.field_sw4_exc5_,
    };
    for (auto* f : all_exc) {
        f->set_value(0);
    }
}

} // namespace drone_analyzer
