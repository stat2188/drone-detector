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

// ============================================================================
// Sweep field ID → config mapping
// ============================================================================
// Identifies which g_workspace_cfg field a NumberField in the SWP view maps to.
// Used by open_freq_keypad_replace to write the keypad result directly to
// the persistent config without capturing a view reference (which would
// dangle after nav.replace() destroys the DroneSweepView).
// ============================================================================

enum class SweepFieldID : uint8_t {
    W1_START,
    W1_END,
    W2_START,
    W2_END,
    W3_START,
    W3_END,
    W4_START,
    W4_END,
    W1_EXC0,
    W1_EXC1,
    W1_EXC2,
    W1_EXC3,
    W1_EXC4,
    W2_EXC0,
    W2_EXC1,
    W2_EXC2,
    W2_EXC3,
    W2_EXC4,
    W3_EXC0,
    W3_EXC1,
    W3_EXC2,
    W3_EXC3,
    W3_EXC4,
    W4_EXC0,
    W4_EXC1,
    W4_EXC2,
    W4_EXC3,
    W4_EXC4,
};

/**
 * @brief Write rf::Frequency to the correct g_workspace_cfg field by SweepFieldID.
 * @note Called from FrequencyKeypadView::on_changed callback AFTER DroneSweepView
 *       has been destroyed by nav.replace(). Must NOT reference any view objects.
 */
static void set_config_field_by_id(SweepFieldID field_id, rf::Frequency f) noexcept {
    switch (field_id) {
        case SweepFieldID::W1_START:
            g_workspace_cfg.sweep_start_freq = f;
            break;
        case SweepFieldID::W1_END:
            g_workspace_cfg.sweep_end_freq = f;
            break;
        case SweepFieldID::W2_START:
            g_workspace_cfg.sweep2_start_freq = f;
            break;
        case SweepFieldID::W2_END:
            g_workspace_cfg.sweep2_end_freq = f;
            break;
        case SweepFieldID::W3_START:
            g_workspace_cfg.sweep3_start_freq = f;
            break;
        case SweepFieldID::W3_END:
            g_workspace_cfg.sweep3_end_freq = f;
            break;
        case SweepFieldID::W4_START:
            g_workspace_cfg.sweep4_start_freq = f;
            break;
        case SweepFieldID::W4_END:
            g_workspace_cfg.sweep4_end_freq = f;
            break;
        case SweepFieldID::W1_EXC0:
            g_workspace_cfg.sweep_exceptions[0][0] = f;
            break;
        case SweepFieldID::W1_EXC1:
            g_workspace_cfg.sweep_exceptions[0][1] = f;
            break;
        case SweepFieldID::W1_EXC2:
            g_workspace_cfg.sweep_exceptions[0][2] = f;
            break;
        case SweepFieldID::W1_EXC3:
            g_workspace_cfg.sweep_exceptions[0][3] = f;
            break;
        case SweepFieldID::W1_EXC4:
            g_workspace_cfg.sweep_exceptions[0][4] = f;
            break;
        case SweepFieldID::W2_EXC0:
            g_workspace_cfg.sweep_exceptions[1][0] = f;
            break;
        case SweepFieldID::W2_EXC1:
            g_workspace_cfg.sweep_exceptions[1][1] = f;
            break;
        case SweepFieldID::W2_EXC2:
            g_workspace_cfg.sweep_exceptions[1][2] = f;
            break;
        case SweepFieldID::W2_EXC3:
            g_workspace_cfg.sweep_exceptions[1][3] = f;
            break;
        case SweepFieldID::W2_EXC4:
            g_workspace_cfg.sweep_exceptions[1][4] = f;
            break;
        case SweepFieldID::W3_EXC0:
            g_workspace_cfg.sweep_exceptions[2][0] = f;
            break;
        case SweepFieldID::W3_EXC1:
            g_workspace_cfg.sweep_exceptions[2][1] = f;
            break;
        case SweepFieldID::W3_EXC2:
            g_workspace_cfg.sweep_exceptions[2][2] = f;
            break;
        case SweepFieldID::W3_EXC3:
            g_workspace_cfg.sweep_exceptions[2][3] = f;
            break;
        case SweepFieldID::W3_EXC4:
            g_workspace_cfg.sweep_exceptions[2][4] = f;
            break;
        case SweepFieldID::W4_EXC0:
            g_workspace_cfg.sweep_exceptions[3][0] = f;
            break;
        case SweepFieldID::W4_EXC1:
            g_workspace_cfg.sweep_exceptions[3][1] = f;
            break;
        case SweepFieldID::W4_EXC2:
            g_workspace_cfg.sweep_exceptions[3][2] = f;
            break;
        case SweepFieldID::W4_EXC3:
            g_workspace_cfg.sweep_exceptions[3][3] = f;
            break;
        case SweepFieldID::W4_EXC4:
            g_workspace_cfg.sweep_exceptions[3][4] = f;
            break;
    }
}

// ============================================================================
// Helper: open frequency keypad with nav.replace() instead of nav.push()
// ============================================================================
// FIX (HEAP OOM): Uses nav.replace() to DESTROY DroneSweepView (~5.8KB heap)
// BEFORE allocating FrequencyKeypadView (~2KB heap). This avoids the triple
// residency (DroneScannerUI + DroneSweepView + FrequencyKeypadView) that
// exhausted the heap and caused chDbgPanic("Out of Memory") → HardFault.
//
// The on_changed callback writes directly to g_workspace_cfg via SweepFieldID.
// After Done, nav.pop() returns to DroneScannerUI (not SWP view). The user
// can press SWP again to see the updated config values.
// ============================================================================

/**
 * @brief Open FrequencyKeypadView via nav.replace().
 * @param nav       NavigationView reference
 * @param field_id  Identifies which g_workspace_cfg field to write to
 * @param initial_hz  Initial keypad value in Hz (read from NumberField BEFORE replace)
 * @note DroneSweepView is destroyed before FrequencyKeypadView is allocated.
 * @note on_changed callback captures field_id by value (safe after view destroy).
 */
static void open_freq_keypad_replace(
    NavigationView& nav,
    SweepFieldID field_id,
    FreqHz initial_hz) noexcept {
    baseband::spectrum_streaming_stop();
    auto* new_view = nav.replace<FrequencyKeypadView>(
        static_cast<rf::Frequency>(initial_hz));
    new_view->on_changed = [field_id](rf::Frequency f) {
        set_config_field_by_id(field_id, f);
    };
}

// ============================================================================
// SweepWindowGroup1View — Tab 1: Windows 1-2
// ============================================================================

SweepWindowGroup1View::SweepWindowGroup1View(NavigationView& nav, const Rect parent_rect) noexcept
    : ui::View()
    , nav_(nav) {
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
        &labels_exc2_,
        &field_sw2_exc0_,
        &field_sw2_exc1_,
        &field_sw2_exc2_,
        &field_sw2_exc3_,
        &field_sw2_exc4_,
    });

    // Use open_freq_keypad_replace which writes to g_workspace_cfg via SweepFieldID.
    // nav.replace() frees DroneSweepView before allocating FrequencyKeypadView.
    // field.value() read while this is alive (before replace destroys the view).
    field_sw1_start_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W1_START,
            static_cast<FreqHz>(field_sw1_start_.value()) * 1000000ULL);
    };
    field_sw1_end_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W1_END,
            static_cast<FreqHz>(field_sw1_end_.value()) * 1000000ULL);
    };
    field_sw2_start_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W2_START,
            static_cast<FreqHz>(field_sw2_start_.value()) * 1000000ULL);
    };
    field_sw2_end_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W2_END,
            static_cast<FreqHz>(field_sw2_end_.value()) * 1000000ULL);
    };

    // Exception fields
    field_sw1_exc0_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W1_EXC0,
            static_cast<FreqHz>(field_sw1_exc0_.value()) * 1000000ULL);
    };
    field_sw1_exc1_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W1_EXC1,
            static_cast<FreqHz>(field_sw1_exc1_.value()) * 1000000ULL);
    };
    field_sw1_exc2_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W1_EXC2,
            static_cast<FreqHz>(field_sw1_exc2_.value()) * 1000000ULL);
    };
    field_sw1_exc3_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W1_EXC3,
            static_cast<FreqHz>(field_sw1_exc3_.value()) * 1000000ULL);
    };
    field_sw1_exc4_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W1_EXC4,
            static_cast<FreqHz>(field_sw1_exc4_.value()) * 1000000ULL);
    };
    field_sw2_exc0_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W2_EXC0,
            static_cast<FreqHz>(field_sw2_exc0_.value()) * 1000000ULL);
    };
    field_sw2_exc1_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W2_EXC1,
            static_cast<FreqHz>(field_sw2_exc1_.value()) * 1000000ULL);
    };
    field_sw2_exc2_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W2_EXC2,
            static_cast<FreqHz>(field_sw2_exc2_.value()) * 1000000ULL);
    };
    field_sw2_exc3_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W2_EXC3,
            static_cast<FreqHz>(field_sw2_exc3_.value()) * 1000000ULL);
    };
    field_sw2_exc4_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W2_EXC4,
            static_cast<FreqHz>(field_sw2_exc4_.value()) * 1000000ULL);
    };
}

void SweepWindowGroup1View::focus() {
    field_sw1_start_.focus();
}

// ============================================================================
// SweepWindowGroup2View — Tab 2: Windows 3-4
// ============================================================================

SweepWindowGroup2View::SweepWindowGroup2View(NavigationView& nav, const Rect parent_rect) noexcept
    : ui::View()
    , nav_(nav) {
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
        &labels_exc4_,
        &field_sw4_exc0_,
        &field_sw4_exc1_,
        &field_sw4_exc2_,
        &field_sw4_exc3_,
        &field_sw4_exc4_,
    });

    field_sw3_start_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W3_START,
            static_cast<FreqHz>(field_sw3_start_.value()) * 1000000ULL);
    };
    field_sw3_end_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W3_END,
            static_cast<FreqHz>(field_sw3_end_.value()) * 1000000ULL);
    };
    field_sw4_start_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W4_START,
            static_cast<FreqHz>(field_sw4_start_.value()) * 1000000ULL);
    };
    field_sw4_end_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W4_END,
            static_cast<FreqHz>(field_sw4_end_.value()) * 1000000ULL);
    };

    field_sw3_exc0_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W3_EXC0,
            static_cast<FreqHz>(field_sw3_exc0_.value()) * 1000000ULL);
    };
    field_sw3_exc1_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W3_EXC1,
            static_cast<FreqHz>(field_sw3_exc1_.value()) * 1000000ULL);
    };
    field_sw3_exc2_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W3_EXC2,
            static_cast<FreqHz>(field_sw3_exc2_.value()) * 1000000ULL);
    };
    field_sw3_exc3_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W3_EXC3,
            static_cast<FreqHz>(field_sw3_exc3_.value()) * 1000000ULL);
    };
    field_sw3_exc4_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W3_EXC4,
            static_cast<FreqHz>(field_sw3_exc4_.value()) * 1000000ULL);
    };
    field_sw4_exc0_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W4_EXC0,
            static_cast<FreqHz>(field_sw4_exc0_.value()) * 1000000ULL);
    };
    field_sw4_exc1_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W4_EXC1,
            static_cast<FreqHz>(field_sw4_exc1_.value()) * 1000000ULL);
    };
    field_sw4_exc2_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W4_EXC2,
            static_cast<FreqHz>(field_sw4_exc2_.value()) * 1000000ULL);
    };
    field_sw4_exc3_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W4_EXC3,
            static_cast<FreqHz>(field_sw4_exc3_.value()) * 1000000ULL);
    };
    field_sw4_exc4_.on_select = [this](NumberField&) {
        open_freq_keypad_replace(nav_, SweepFieldID::W4_EXC4,
            static_cast<FreqHz>(field_sw4_exc4_.value()) * 1000000ULL);
    };
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
    , original_config_(config)
    , view_group1_(nav_, Rect{0, TAB_BAR_H, screen_width, screen_height - TAB_BAR_H})
    , view_group2_(nav_, Rect{0, TAB_BAR_H, screen_width, screen_height - TAB_BAR_H})
    , tab_view_({
        {"Win 1-2", Color::white(), &view_group1_},
        {"Win 3-4", Color::white(), &view_group2_}
    }) {
    view_group2_.hidden(true);

    add_children({
        &tab_view_,
        &view_group1_,
        &view_group2_,
        &labels_exc_radius_,
        &field_exc_radius_,
        &button_defaults_,
        &button_save_,
    });

    tab_view_.set_selected(0);

    view_group1_.field_sw1_start_.set_value(static_cast<int32_t>(config.sweep_start_freq / 1000000ULL));
    view_group1_.field_sw1_end_.set_value(static_cast<int32_t>(config.sweep_end_freq / 1000000ULL));
    view_group1_.field_sw1_step_.set_value(static_cast<int32_t>(config.sweep_step_freq / 1000ULL));

    view_group1_.check_sw2_enabled_.set_value(config.sweep2_enabled);
    view_group1_.field_sw2_start_.set_value(static_cast<int32_t>(config.sweep2_start_freq / 1000000ULL));
    view_group1_.field_sw2_end_.set_value(static_cast<int32_t>(config.sweep2_end_freq / 1000000ULL));
    view_group1_.field_sw2_step_.set_value(static_cast<int32_t>(config.sweep2_step_freq / 1000ULL));

    view_group2_.check_sw3_enabled_.set_value(config.sweep3_enabled);
    view_group2_.field_sw3_start_.set_value(static_cast<int32_t>(config.sweep3_start_freq / 1000000ULL));
    view_group2_.field_sw3_end_.set_value(static_cast<int32_t>(config.sweep3_end_freq / 1000000ULL));
    view_group2_.field_sw3_step_.set_value(static_cast<int32_t>(config.sweep3_step_freq / 1000ULL));

    view_group2_.check_sw4_enabled_.set_value(config.sweep4_enabled);
    view_group2_.field_sw4_start_.set_value(static_cast<int32_t>(config.sweep4_start_freq / 1000000ULL));
    view_group2_.field_sw4_end_.set_value(static_cast<int32_t>(config.sweep4_end_freq / 1000000ULL));
    view_group2_.field_sw4_step_.set_value(static_cast<int32_t>(config.sweep4_step_freq / 1000ULL));

    // Exception fields — read directly from config without static pointer arrays.
    // BUGFIX: Was using static ui::NumberField* arrays that pointed to member fields
    // of the FIRST instance, causing use-after-free on second construction.
    set_exception_field(&view_group1_.field_sw1_exc0_, 0, config.sweep_exceptions[0][0]);
    set_exception_field(&view_group1_.field_sw1_exc1_, 1, config.sweep_exceptions[0][1]);
    set_exception_field(&view_group1_.field_sw1_exc2_, 2, config.sweep_exceptions[0][2]);
    set_exception_field(&view_group1_.field_sw1_exc3_, 3, config.sweep_exceptions[0][3]);
    set_exception_field(&view_group1_.field_sw1_exc4_, 4, config.sweep_exceptions[0][4]);

    set_exception_field(&view_group1_.field_sw2_exc0_, 0, config.sweep_exceptions[1][0]);
    set_exception_field(&view_group1_.field_sw2_exc1_, 1, config.sweep_exceptions[1][1]);
    set_exception_field(&view_group1_.field_sw2_exc2_, 2, config.sweep_exceptions[1][2]);
    set_exception_field(&view_group1_.field_sw2_exc3_, 3, config.sweep_exceptions[1][3]);
    set_exception_field(&view_group1_.field_sw2_exc4_, 4, config.sweep_exceptions[1][4]);

    set_exception_field(&view_group2_.field_sw3_exc0_, 0, config.sweep_exceptions[2][0]);
    set_exception_field(&view_group2_.field_sw3_exc1_, 1, config.sweep_exceptions[2][1]);
    set_exception_field(&view_group2_.field_sw3_exc2_, 2, config.sweep_exceptions[2][2]);
    set_exception_field(&view_group2_.field_sw3_exc3_, 3, config.sweep_exceptions[2][3]);
    set_exception_field(&view_group2_.field_sw3_exc4_, 4, config.sweep_exceptions[2][4]);

    set_exception_field(&view_group2_.field_sw4_exc0_, 0, config.sweep_exceptions[3][0]);
    set_exception_field(&view_group2_.field_sw4_exc1_, 1, config.sweep_exceptions[3][1]);
    set_exception_field(&view_group2_.field_sw4_exc2_, 2, config.sweep_exceptions[3][2]);
    set_exception_field(&view_group2_.field_sw4_exc3_, 3, config.sweep_exceptions[3][3]);
    set_exception_field(&view_group2_.field_sw4_exc4_, 4, config.sweep_exceptions[3][4]);

    field_exc_radius_.set_value(static_cast<int32_t>(config.exception_radius_mhz));

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
    if (tab_view_.selected() == 0) {
        view_group1_.focus();
    } else {
        view_group2_.focus();
    }
}

// ============================================================================
// Exception field helpers (inline — avoids static array pointer bugs)
// ============================================================================

void DroneSweepView::set_exception_field(
    ui::NumberField* field,
    uint8_t /*index*/,
    FreqHz value_hz
) noexcept {
    if (field != nullptr) {
        field->set_value(static_cast<int32_t>(value_hz / 1000000ULL));
    }
}

int32_t DroneSweepView::get_exception_field(const ui::NumberField* field) noexcept {
    return (field != nullptr) ? field->value() : 0;
}

void DroneSweepView::zero_exception_field(ui::NumberField* field) noexcept {
    if (field != nullptr) {
        field->set_value(0);
    }
}

// ============================================================================
// Save / Defaults
// ============================================================================

void DroneSweepView::save_settings() noexcept {
    // Stack budget: 4KB main thread stack. All local variables are small POD types.
    // FreqHz (uint64_t) × 12 = 96 bytes, bool × 4 = 4 bytes, uint8_t = 1 byte.
    // Total: ~120 bytes stack. No static arrays to member fields (was use-after-free bug).

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

    // Validate ranges
    if (sw1_start < HARDWARE_MIN_FREQ_HZ) sw1_start = HARDWARE_MIN_FREQ_HZ;
    if (sw1_end > HARDWARE_MAX_FREQ_HZ) sw1_end = HARDWARE_MAX_FREQ_HZ;
    if (sw1_start >= sw1_end) sw1_end = sw1_start + 20000000;
    if (sw2_enabled) {
        if (sw2_start < HARDWARE_MIN_FREQ_HZ) sw2_start = HARDWARE_MIN_FREQ_HZ;
        if (sw2_end > HARDWARE_MAX_FREQ_HZ) sw2_end = HARDWARE_MAX_FREQ_HZ;
        if (sw2_start >= sw2_end) sw2_end = sw2_start + 20000000;
    }
    if (sw3_enabled) {
        if (sw3_start < HARDWARE_MIN_FREQ_HZ) sw3_start = HARDWARE_MIN_FREQ_HZ;
        if (sw3_end > HARDWARE_MAX_FREQ_HZ) sw3_end = HARDWARE_MAX_FREQ_HZ;
        if (sw3_start >= sw3_end) sw3_end = sw3_start + 20000000;
    }
    if (sw4_enabled) {
        if (sw4_start < HARDWARE_MIN_FREQ_HZ) sw4_start = HARDWARE_MIN_FREQ_HZ;
        if (sw4_end > HARDWARE_MAX_FREQ_HZ) sw4_end = HARDWARE_MAX_FREQ_HZ;
        if (sw4_start >= sw4_end) sw4_end = sw4_start + 20000000;
    }

    const uint8_t exc_radius = static_cast<uint8_t>(field_exc_radius_.value());

    // Save to scanner config
    // NOTE: No intermediate exc[4][5] stack array — write directly to workspace.
    if (scanner_ptr_ != nullptr) {
        g_workspace_cfg = original_config_;
        g_workspace_cfg.sweep_start_freq = sw1_start;
        g_workspace_cfg.sweep_end_freq = sw1_end;
        g_workspace_cfg.sweep_step_freq = sw1_step;
        g_workspace_cfg.sweep2_start_freq = sw2_start;
        g_workspace_cfg.sweep2_end_freq = sw2_end;
        g_workspace_cfg.sweep2_step_freq = sw2_step;
        g_workspace_cfg.sweep2_enabled = sw2_enabled;
        g_workspace_cfg.sweep3_start_freq = sw3_start;
        g_workspace_cfg.sweep3_end_freq = sw3_end;
        g_workspace_cfg.sweep3_step_freq = sw3_step;
        g_workspace_cfg.sweep3_enabled = sw3_enabled;
        g_workspace_cfg.sweep4_start_freq = sw4_start;
        g_workspace_cfg.sweep4_end_freq = sw4_end;
        g_workspace_cfg.sweep4_step_freq = sw4_step;
        g_workspace_cfg.sweep4_enabled = sw4_enabled;
        g_workspace_cfg.sweep_exceptions[0][0] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc0_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[0][1] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc1_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[0][2] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc2_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[0][3] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc3_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[0][4] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc4_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[1][0] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc0_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[1][1] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc1_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[1][2] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc2_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[1][3] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc3_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[1][4] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc4_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[2][0] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc0_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[2][1] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc1_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[2][2] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc2_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[2][3] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc3_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[2][4] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc4_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[3][0] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc0_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[3][1] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc1_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[3][2] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc2_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[3][3] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc3_)) * 1000000ULL;
        g_workspace_cfg.sweep_exceptions[3][4] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc4_)) * 1000000ULL;
        g_workspace_cfg.exception_radius_mhz = exc_radius;
        (void)scanner_ptr_->set_config(g_workspace_cfg);
    }

    // Save to SD via SettingsFileManager
    (void)SettingsFileManager::load(g_workspace_settings);

    g_workspace_settings.sweep_start_freq = sw1_start;
    g_workspace_settings.sweep_end_freq = sw1_end;
    g_workspace_settings.sweep_step_freq = sw1_step;
    g_workspace_settings.sweep2_start_freq = sw2_start;
    g_workspace_settings.sweep2_end_freq = sw2_end;
    g_workspace_settings.sweep2_step_freq = sw2_step;
    g_workspace_settings.sweep2_enabled = sw2_enabled;
    g_workspace_settings.sweep3_start_freq = sw3_start;
    g_workspace_settings.sweep3_end_freq = sw3_end;
    g_workspace_settings.sweep3_step_freq = sw3_step;
    g_workspace_settings.sweep3_enabled = sw3_enabled;
    g_workspace_settings.sweep4_start_freq = sw4_start;
    g_workspace_settings.sweep4_end_freq = sw4_end;
    g_workspace_settings.sweep4_step_freq = sw4_step;
    g_workspace_settings.sweep4_enabled = sw4_enabled;
    g_workspace_settings.sweep_exceptions[0][0] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc0_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[0][1] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc1_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[0][2] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc2_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[0][3] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc3_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[0][4] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw1_exc4_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[1][0] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc0_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[1][1] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc1_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[1][2] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc2_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[1][3] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc3_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[1][4] = static_cast<FreqHz>(get_exception_field(&view_group1_.field_sw2_exc4_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[2][0] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc0_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[2][1] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc1_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[2][2] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc2_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[2][3] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc3_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[2][4] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw3_exc4_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[3][0] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc0_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[3][1] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc1_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[3][2] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc2_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[3][3] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc3_)) * 1000000ULL;
    g_workspace_settings.sweep_exceptions[3][4] = static_cast<FreqHz>(get_exception_field(&view_group2_.field_sw4_exc4_)) * 1000000ULL;
    g_workspace_settings.exception_radius_mhz = exc_radius;

    (void)SettingsFileManager::save(scanner_ptr_, g_workspace_settings);
}

void DroneSweepView::apply_defaults() noexcept {
    // Static const defaults — initialized once, stored in .rodata (Flash)
    // Stack: 0 bytes (was ~360 B with stack-allocated SettingsStruct)
    static const SettingsStruct defaults{};

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

    // Zero all exception fields (no static pointer array — bugfix)
    zero_exception_field(&view_group1_.field_sw1_exc0_);
    zero_exception_field(&view_group1_.field_sw1_exc1_);
    zero_exception_field(&view_group1_.field_sw1_exc2_);
    zero_exception_field(&view_group1_.field_sw1_exc3_);
    zero_exception_field(&view_group1_.field_sw1_exc4_);
    zero_exception_field(&view_group1_.field_sw2_exc0_);
    zero_exception_field(&view_group1_.field_sw2_exc1_);
    zero_exception_field(&view_group1_.field_sw2_exc2_);
    zero_exception_field(&view_group1_.field_sw2_exc3_);
    zero_exception_field(&view_group1_.field_sw2_exc4_);
    zero_exception_field(&view_group2_.field_sw3_exc0_);
    zero_exception_field(&view_group2_.field_sw3_exc1_);
    zero_exception_field(&view_group2_.field_sw3_exc2_);
    zero_exception_field(&view_group2_.field_sw3_exc3_);
    zero_exception_field(&view_group2_.field_sw3_exc4_);
    zero_exception_field(&view_group2_.field_sw4_exc0_);
    zero_exception_field(&view_group2_.field_sw4_exc1_);
    zero_exception_field(&view_group2_.field_sw4_exc2_);
    zero_exception_field(&view_group2_.field_sw4_exc3_);
    zero_exception_field(&view_group2_.field_sw4_exc4_);

    field_exc_radius_.set_value(static_cast<int32_t>(DEFAULT_EXCEPTION_RADIUS_MHZ));
}

} // namespace drone_analyzer