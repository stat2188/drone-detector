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

// ============================================================================
// RangeNameSelector — zero-heap replacement for a full RANGE_NAMES OptionsField
// ============================================================================
// OptionsField stores its options as std::vector<std::pair<std::string,int>>
// (~36 B of heap per entry — for the current table size that is >1 KB, plus a
// second transient vector inside set_options()). RANGE_NAMES is a compile-time Flash table, so the selector
// needs only one index: the label string is resolved at paint time via
// range_name_to_string(). Value space IS the index space — set_by_value() /
// on_change() keep the exact OptionsField API (size_t, int32_t), so the
// existing wiring compiles unchanged.
// Heap: 0 B. SRAM: ~40 B (inside SweepWindowView). Flash: ~200 B.

constexpr size_t range_name_max_len() noexcept {
    size_t max_len = 0;
    for (size_t i = 0; i < RANGE_NAME_COUNT; ++i) {
        size_t len = 0;
        while (RANGE_NAMES[i][len] != '\0') ++len;
        if (len > max_len) max_len = len;
    }
    return max_len;
}

static_assert(range_name_max_len() <= 10,
    "RANGE_NAMES entry no longer fits the 10-char field_label_name_ selector");

void RangeNameSelector::set_selected_index(
    const size_t new_index, const bool trigger_change) noexcept {
    if (new_index >= RANGE_NAME_COUNT) return;
    if (new_index != selected_index_ || trigger_change) {
        selected_index_ = new_index;
        if (on_change) {
            on_change(selected_index_, static_cast<int32_t>(selected_index_));
        }
        set_dirty();
    }
}

void RangeNameSelector::set_by_value(const value_t v) noexcept {
    // Value space IS the index space: RANGE_NAMES[i] <-> i.
    set_selected_index(
        (v >= 0 && v < static_cast<value_t>(RANGE_NAME_COUNT))
            ? static_cast<size_t>(v) : 0U);
}

bool RangeNameSelector::on_encoder(const ui::EncoderEvent delta) {
    // Wrap in both directions (parity with ui::OptionsField::on_encoder).
    int32_t new_value = static_cast<int32_t>(selected_index_) + delta;
    if (new_value < 0) {
        new_value = static_cast<int32_t>(RANGE_NAME_COUNT) - 1;
    } else if (static_cast<size_t>(new_value) >= RANGE_NAME_COUNT) {
        new_value = 0;
    }
    set_selected_index(static_cast<size_t>(new_value));
    return true;
}

bool RangeNameSelector::on_keyboard(const ui::KeyboardEvent key) {
    // Parity with ui::OptionsField::on_keyboard.
    if (key == '+' || key == ' ' || key == 10) return on_encoder(1);
    if (key == '-' || key == 8) return on_encoder(-1);
    return false;
}

bool RangeNameSelector::on_touch(const ui::TouchEvent event) {
    // Parity with ui::OptionsField::on_touch: tap = focus only, the value is
    // edited with the encoder (same UX as the R1..R5 slot selector above).
    if (event.type == ui::TouchEvent::Type::Start) {
        focus();
    }
    return true;
}

void RangeNameSelector::on_focus() {
    set_dirty();  // repaint with the focused (inverted) style
}

void RangeNameSelector::on_blur() {
    set_dirty();  // repaint without the focused style
}

void RangeNameSelector::paint(ui::Painter& painter) {
    const auto paint_style = has_focus() ? style().invert() : style();

    // Dark background box erases the previous (possibly longer) label —
    // mirrors ui::OptionsField::paint().
    painter.fill_rectangle(
        {screen_rect().location(), {static_cast<ui::Dim>(length_ * 8), 16}},
        Theme::getInstance()->bg_darkest->background);

    // Label lives in Flash (range_names.hpp) — zero heap, zero copy.
    painter.draw_string(
        screen_pos(), paint_style,
        range_name_to_string(static_cast<uint8_t>(selected_index_)));
}

// ============================================================================
// Detection-range ↔ sweep-window clamp (SWP-tab bugfix)
// ============================================================================
// The five detection ranges (R1..R5) of a sweep window must never extend
// outside that window's [Start, End] scan range. Previously the From/To
// NumberFields accepted any value in {0, 7200} MHz, so a range entered
// outside the window silently gated detection into a band the sweep never
// visits and persisted the invalid value into the settings file. The clamp
// below is the single rule applied on every entry path: widget encoder /
// keyboard steps, the on-screen frequency keypad, and the config load in
// the DroneSweepView constructor.

/**
 * @brief Clamp one detection-range bound into the enclosing sweep window
 * @param value_mhz Bound as entered, MHz (0 = slot OFF — passed through)
 * @param win_a_mhz Window Start, MHz
 * @param win_b_mhz Window End, MHz (order-agnostic; inverted windows tolerated)
 * @return value_mhz forced into [min(win_a, win_b), max(win_a, win_b)];
 *         0 is returned unchanged — the documented slot-OFF sentinel
 *         (constants.hpp::is_det_win_slot_active) must stay reachable.
 * @note MHz domain: every UI-side store (WindowData, ScanConfig/SettingsStruct
 *       MHz mirror, displayed NumberField values) is MHz-quantized, so the
 *       clamp and the display always agree with no rounding drift.
 */
[[nodiscard]] static constexpr int32_t clamp_det_mhz_to_window(
    const int32_t value_mhz, const int32_t win_a_mhz, const int32_t win_b_mhz) noexcept {
    if (value_mhz == 0) {
        return 0;  // 0 = slot OFF — always legal, never clamped
    }
    const int32_t lo = (win_a_mhz < win_b_mhz) ? win_a_mhz : win_b_mhz;
    const int32_t hi = (win_a_mhz < win_b_mhz) ? win_b_mhz : win_a_mhz;
    return (value_mhz < lo) ? lo : (value_mhz > hi) ? hi : value_mhz;
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
// ZERO-HEAP CLOSURE CONTRACT (verified against GCC 9.2.1 bits/std_function.h):
// std::function's inline storage on ARM32 is _M_max_size = sizeof(_Nocopy_types)
// = 8 B. Any closure larger than 8 B falls back to operator new → chHeapAlloc
// (common/chibios_cpp.cpp) — one allocation per keypad open, and the exact
// allocation that starved the SWP-tab keypad before the RangeNameSelector
// commit (23bc0a40). The closure below captures {SweepFieldID (1 B),
// SweepWindowView* (4 B)} = 8 B, alignment 4 → stored locally, zero heap.
// All per-field work (target widget lookup, window-bound clamp, config mirror
// write, scanner update) lives in SweepWindowView::on_freq_keypad_done().
static void open_freq_keypad_push(
    NavigationView& nav,
    SweepFieldID field_id,
    FreqHz initial_hz,
    SweepWindowView& host) noexcept {
    baseband::spectrum_streaming_stop();
    auto* new_view = nav.push<FrequencyKeypadView>(
        static_cast<rf::Frequency>(initial_hz));
    new_view->on_changed = [field_id, &host](rf::Frequency f) {
        host.on_freq_keypad_done(field_id, f);
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

    // on_select callbacks route through the bound window index.
    // SWP bugfix: on_change on Start/End re-clamps ALL 10 detection-range
    // fields into the NEW window span — previously valid ranges are reset to
    // the window min/max the moment the user shrinks or moves the scan
    // window. set_value(..., false) suppresses the per-field widget clamp.
    // All keypad closures are ≤ 8 B ([this] = 4 B / [field_id, &host] = 8 B)
    // → std::function inline storage, ZERO heap (see open_freq_keypad_push).
    field_start_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, start_field_id(bound_index_),
            static_cast<FreqHz>(field_start_.value()) * MHZ, *this);
    };
    field_end_.on_select = [this](NumberField&) {
        open_freq_keypad_push(nav_, end_field_id(bound_index_),
            static_cast<FreqHz>(field_end_.value()) * MHZ, *this);
    };
    field_start_.on_change = [this](int32_t) { (void)sanitize_det_ranges(); };
    field_end_.on_change = [this](int32_t) { (void)sanitize_det_ranges(); };

    // Detection-window range fields (5 × From/To).
    // SWP bugfix: the NumberField's static range is {0, 7200} MHz — it knows
    // nothing about the enclosing scan window. on_change feeds every widget
    // step (encoder / keyboard) back through clamp_det_field_to_window(),
    // which writes the corrected value back via set_value() when the user
    // picked a bound outside the window. set_value() re-fires on_change only
    // when the value actually differs → the correction pass finds the value
    // already inside the window and the loop terminates at depth 2.
    // Keypad entry is clamped separately (see open_freq_keypad_push).
    field_dw0_start_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw0_start_, v); };
    field_dw0_end_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw0_end_, v); };
    field_dw1_start_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw1_start_, v); };
    field_dw1_end_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw1_end_, v); };
    field_dw2_start_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw2_start_, v); };
    field_dw2_end_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw2_end_, v); };
    field_dw3_start_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw3_start_, v); };
    field_dw3_end_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw3_end_, v); };
    field_dw4_start_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw4_start_, v); };
    field_dw4_end_.on_change = [this](int32_t v) { clamp_det_field_to_window(field_dw4_end_, v); };
    // Detection-range keypad entry (5 × From/To): one data-driven loop — the
    // enum is dense (W{N}_DW{slot}_{FROM|TO}) and on_freq_keypad_done() decodes
    // the slot arithmetically. Closure budget: [this, i] = 8 B — exactly the
    // std::function inline storage → ZERO heap (see open_freq_keypad_push).
    {
        ui::NumberField* const dw_start_fields[DETECTION_WINDOWS_PER_WINDOW] = {
            &field_dw0_start_, &field_dw1_start_, &field_dw2_start_, &field_dw3_start_, &field_dw4_start_};
        ui::NumberField* const dw_end_fields[DETECTION_WINDOWS_PER_WINDOW] = {
            &field_dw0_end_, &field_dw1_end_, &field_dw2_end_, &field_dw3_end_, &field_dw4_end_};
        for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
            dw_start_fields[i]->on_select = [this, i](NumberField&) {
                open_freq_keypad_push(nav_, dw_field_id(bound_index_, i, false),
                    static_cast<FreqHz>(dw_field_(i, false)->value()) * MHZ, *this);
            };
            dw_end_fields[i]->on_select = [this, i](NumberField&) {
                open_freq_keypad_push(nav_, dw_field_id(bound_index_, i, true),
                    static_cast<FreqHz>(dw_field_(i, true)->value()) * MHZ, *this);
            };
        }
    }

    // Range-label editor wiring (lower half). Zero heap: the selector draws
    // labels straight from the Flash-resident RANGE_NAMES table.
    // Slot selector: on switch, show the newly selected slot's current label.
    field_label_slot_.on_change = [this](size_t, int32_t v) {
        label_slot_ = (v >= 0 && v < DETECTION_WINDOWS_PER_WINDOW)
            ? static_cast<uint8_t>(v) : 0U;
        refresh_label_selector();
    };
    // Label selector: write straight into the bound WindowData (SAVE
    // propagates it into ScanConfig + the settings file).
    // Hardened: the stored index is clamped before use (load-time normalization
    // guarantees < DETECTION_WINDOWS_PER_WINDOW; zero-cost defensive check).
    field_label_name_.on_change = [this](size_t, int32_t v) {
        if (bound_data_ == nullptr) return;
        const uint8_t slot = (label_slot_ < DETECTION_WINDOWS_PER_WINDOW)
            ? label_slot_
            : static_cast<uint8_t>(DETECTION_WINDOWS_PER_WINDOW - 1);
        bound_data_->name_idx[slot] =
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
        // Zero-alloc invariant: labels_ was constructed with 3 entries
        // (capacity 3), every text here is <= 15 chars ("-- Win 4 --" = 11)
        // → libstdc++ reuses the vector buffer and SSO strings on every
        // set_labels call — NO heap after construction. Keep the entry count
        // at 3 and texts <= 15 chars, or each bind() re-allocates again.
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
    // on_change suppressed on Start/End as well: sanitize_det_ranges() must
    // not run against the intermediate state (Start already updated, End
    // still stale from the previous window). The bound WindowData is already
    // normalized — see sanitize_window_det_ranges() / sync_from_widgets().
    field_start_.set_value(static_cast<int32_t>(bound_data_->start_freq / MHZ), false);
    field_end_.set_value(static_cast<int32_t>(bound_data_->end_freq / MHZ), false);
    check_enabled_.set_value(bound_data_->enabled);

    ui::NumberField* dw_start_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_start_, &field_dw1_start_, &field_dw2_start_, &field_dw3_start_, &field_dw4_start_};
    ui::NumberField* dw_end_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_end_, &field_dw1_end_, &field_dw2_end_, &field_dw3_end_, &field_dw4_end_};
    // Sync with on_change suppressed: the widget-side clamp
    // (clamp_det_field_to_window) must NOT re-fire here — values coming from
    // the bound WindowData have already been normalized by the constructor's
    // sanitize_window_det_ranges() pass (load-time normalization), and
    // re-writing inside set_value's own on_change would recurse.
    for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
        dw_start_fields[i]->set_value(static_cast<int32_t>(bound_data_->det_windows[i].start_hz / MHZ), false);
        dw_end_fields[i]->set_value(static_cast<int32_t>(bound_data_->det_windows[i].end_hz / MHZ), false);
    }

    // Label editor — reflect the selected slot's stored label index.
    refresh_label_selector();
}

void SweepWindowView::sync_from_widgets() noexcept {
    if (bound_data_ == nullptr) return;
    bound_data_->start_freq = read_mhz_field(field_start_);
    bound_data_->end_freq = read_mhz_field(field_end_);
    bound_data_->enabled = check_enabled_.value();

    // SWP bugfix: the bound window bounds from the WIDGETS (not the stale
    // WindowData copies — sync_from_widgets runs BEFORE those are updated on
    // switch/save). sanitize_det_ranges() then clamps every range outside
    // [min(Start, End), max(Start, End)] back into the window, so the stored
    // data can never drift outside the visible scan range.
    const FreqHz win_a_hz = static_cast<FreqHz>(field_start_.value()) * MHZ;
    const FreqHz win_b_hz = static_cast<FreqHz>(field_end_.value()) * MHZ;
    const FreqHz lo_hz = (win_a_hz < win_b_hz) ? win_a_hz : win_b_hz;
    const FreqHz hi_hz = (win_a_hz < win_b_hz) ? win_b_hz : win_a_hz;

    const ui::NumberField* dw_start_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_start_, &field_dw1_start_, &field_dw2_start_, &field_dw3_start_, &field_dw4_start_};
    const ui::NumberField* dw_end_fields[DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_end_, &field_dw1_end_, &field_dw2_end_, &field_dw3_end_, &field_dw4_end_};
    for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
        FreqHz s_hz = static_cast<FreqHz>(dw_start_fields[i]->value()) * MHZ;
        FreqHz e_hz = static_cast<FreqHz>(dw_end_fields[i]->value()) * MHZ;
        if (s_hz != 0) {
            s_hz = (s_hz < lo_hz) ? lo_hz : (s_hz > hi_hz) ? hi_hz : s_hz;
        }
        if (e_hz != 0) {
            e_hz = (e_hz < lo_hz) ? lo_hz : (e_hz > hi_hz) ? hi_hz : e_hz;
        }
        bound_data_->det_windows[i].start_hz = s_hz;
        bound_data_->det_windows[i].end_hz = e_hz;
    }
}

// ----------------------------------------------------------------------------
// SWP bugfix helpers — detection ranges must stay inside the scan window
// ----------------------------------------------------------------------------

void SweepWindowView::clamp_det_field_to_window(
    ui::NumberField& field, const int32_t entered_mhz) noexcept {
    if (bound_data_ == nullptr) return;
    // [min, max] of the CURRENT window bounds (order-agnostic: a window
    // edited to Start > End still clamps into the real span instead of
    // collapsing every range to a degenerate empty interval).
    const DetRangeWindowClamp c = det_range_clamp();
    if (!c.active()) return;  // unreachable: bound_data_ != nullptr ⇒ window loaded
    const int32_t clamped = clamp_det_mhz_to_window(entered_mhz, c.min_mhz, c.max_mhz);
    if (clamped == entered_mhz) return;  // inside the window — nothing to fix
    // Re-fires on_change ONLY if the value differs (ui_widget.cpp) → the
    // correction pass sees the value already inside the window and returns.
    // Recursion depth: exactly 2.
    field.set_value(clamped);
}

bool SweepWindowView::sanitize_det_ranges() noexcept {
    if (bound_data_ == nullptr) return false;
    // Widget bounds are authoritative here: on switch/save this runs right
    // after sync_from_widgets(), which wrote the widget values into the bound
    // window bounds — always in sync, no stale-copy hazard.
    const int32_t win_a = field_start_.value();
    const int32_t win_b = field_end_.value();
    const int32_t lo = (win_a < win_b) ? win_a : win_b;
    const int32_t hi = (win_a < win_b) ? win_b : win_a;

    ui::NumberField* fields[2 * DETECTION_WINDOWS_PER_WINDOW] = {
        &field_dw0_start_, &field_dw0_end_,
        &field_dw1_start_, &field_dw1_end_,
        &field_dw2_start_, &field_dw2_end_,
        &field_dw3_start_, &field_dw3_end_,
        &field_dw4_start_, &field_dw4_end_};

    bool changed = false;
    for (uint8_t i = 0; i < (2 * DETECTION_WINDOWS_PER_WINDOW); ++i) {
        const int32_t entered = fields[i]->value();
        const int32_t clamped = clamp_det_mhz_to_window(entered, lo, hi);
        if (clamped != entered) {
            // set_value suppresses on_change → no widget-clamp ping-pong.
            fields[i]->set_value(clamped, false);
            changed = true;
        }
    }
    return changed;
}

void SweepWindowView::refresh_label_selector() noexcept {
    if (bound_data_ == nullptr) return;
    const uint8_t slot = (label_slot_ < DETECTION_WINDOWS_PER_WINDOW)
        ? label_slot_
        : static_cast<uint8_t>(DETECTION_WINDOWS_PER_WINDOW - 1);
    field_label_name_.set_by_value(
        static_cast<int32_t>(bound_data_->name_idx[slot]));
}

DetRangeWindowClamp SweepWindowView::det_range_clamp() const noexcept {
    if (bound_data_ == nullptr) return {};  // clamp disabled
    const int32_t win_a = field_start_.value();
    const int32_t win_b = field_end_.value();
    return {(win_a < win_b) ? win_a : win_b,
            (win_a < win_b) ? win_b : win_a};
}

ui::NumberField* SweepWindowView::dw_field_(
    const uint8_t slot, const bool is_end) noexcept {
    // slot < DETECTION_WINDOWS_PER_WINDOW by construction (loop index / decoded
    // dense enum); clamped defensively so the helper can never index OOB.
    const uint8_t s = (slot < DETECTION_WINDOWS_PER_WINDOW)
        ? slot
        : static_cast<uint8_t>(DETECTION_WINDOWS_PER_WINDOW - 1);
    switch (s) {
        case 0: return is_end ? &field_dw0_end_ : &field_dw0_start_;
        case 1: return is_end ? &field_dw1_end_ : &field_dw1_start_;
        case 2: return is_end ? &field_dw2_end_ : &field_dw2_start_;
        case 3: return is_end ? &field_dw3_end_ : &field_dw3_start_;
        default: return is_end ? &field_dw4_end_ : &field_dw4_start_;
    }
}

void SweepWindowView::on_freq_keypad_done(
    const SweepFieldID field_id, const rf::Frequency f) noexcept {
    if (bound_data_ == nullptr) return;

    // Resolve the target widget arithmetically (the enum is dense & ordered):
    //   ids 0..7   → window bounds:  even = START, odd = END (W{N}_START/END)
    //   ids 8..47  → detection ranges: rel = id - W1_DW0_FROM,
    //                window = rel/10, slot = (rel%10)/2, odd = TO
    // Only the BOUND window's widgets are ever edited (the keypad is opened
    // from the bound window and is modal), so the widget side ignores the
    // window sub-index — set_config_field_by_id() still writes the exact id.
    ui::NumberField* target = nullptr;
    bool is_det_range = false;

    const uint8_t id = static_cast<uint8_t>(field_id);
    if (id >= static_cast<uint8_t>(SweepFieldID::W1_DW0_FROM)) {
        static constexpr uint8_t SLOTS_PER_WINDOW = 2 * DETECTION_WINDOWS_PER_WINDOW;  // 10
        const uint8_t rel = static_cast<uint8_t>(
            id - static_cast<uint8_t>(SweepFieldID::W1_DW0_FROM));
        const uint8_t slot = static_cast<uint8_t>((rel % SLOTS_PER_WINDOW) / 2U);
        const bool to_field = ((rel % SLOTS_PER_WINDOW) % 2U) != 0U;

        ui::NumberField* const dw_start_fields[DETECTION_WINDOWS_PER_WINDOW] = {
            &field_dw0_start_, &field_dw1_start_, &field_dw2_start_, &field_dw3_start_, &field_dw4_start_};
        ui::NumberField* const dw_end_fields[DETECTION_WINDOWS_PER_WINDOW] = {
            &field_dw0_end_, &field_dw1_end_, &field_dw2_end_, &field_dw3_end_, &field_dw4_end_};
        target = to_field ? dw_end_fields[slot] : dw_start_fields[slot];
        is_det_range = true;
    } else {
        target = ((id % 2U) == 0U) ? &field_start_ : &field_end_;
    }

    if (target == nullptr) return;

    if (is_det_range) {
        // SWP bugfix: reset to the window min/max when the user picked a value
        // outside the scan window. Clamp is read at callback time (the keypad
        // is modal — the bounds cannot change while it is open).
        const DetRangeWindowClamp c = det_range_clamp();
        const int32_t clamped_mhz = clamp_det_mhz_to_window(
            static_cast<int32_t>(f / MHZ), c.min_mhz, c.max_mhz);
        set_config_field_by_id(field_id, static_cast<FreqHz>(clamped_mhz) * MHZ);
        target->set_value(clamped_mhz);
    } else {
        // Window Start/End entry: raw keypad value, as before.
        set_config_field_by_id(field_id, f);
        target->set_value(static_cast<int32_t>(f / MHZ));
    }

    if (scanner_ptr_ != nullptr) {
        (void)scanner_ptr_->set_config(g_workspace_cfg);
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

    // SWP bugfix: configs saved by older firmware may carry detection ranges
    // outside their window's scan range (the old UI accepted any value in
    // {0, 7200} MHz). Normalize BEFORE the first bind so the widgets never
    // display out-of-window ranges; the first SAVE heals the settings file.
    sanitize_window_det_ranges();

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

void DroneSweepView::sanitize_window_det_ranges() noexcept {
    // Load-time normalization (SWP bugfix). Every detection range of every
    // window is forced into its window's [min(Start, End), max(Start, End)]
    // span; 0/0 (slot OFF) passes through untouched. Operates on the MHz
    // domain — identical to what the widgets display and what the MHz mirror
    // in ScanConfig / the settings file stores. In-place, no heap, O(4×5).
    for (uint8_t w = 0; w < NUM_WINDOWS; ++w) {
        const FreqHz win_a = windows_[w].start_freq;
        const FreqHz win_b = windows_[w].end_freq;
        const FreqHz lo_hz = (win_a < win_b) ? win_a : win_b;
        const FreqHz hi_hz = (win_a < win_b) ? win_b : win_a;
        for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
            FreqHz s_hz = windows_[w].det_windows[i].start_hz;
            FreqHz e_hz = windows_[w].det_windows[i].end_hz;
            if (s_hz != 0) {
                s_hz = (s_hz < lo_hz) ? lo_hz : (s_hz > hi_hz) ? hi_hz : s_hz;
            }
            if (e_hz != 0) {
                e_hz = (e_hz < lo_hz) ? lo_hz : (e_hz > hi_hz) ? hi_hz : e_hz;
            }
            windows_[w].det_windows[i].start_hz = s_hz;
            windows_[w].det_windows[i].end_hz = e_hz;
        }
    }
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
