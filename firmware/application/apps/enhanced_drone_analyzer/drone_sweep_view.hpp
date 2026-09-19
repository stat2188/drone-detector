#ifndef DRONE_SWEEP_VIEW_HPP
#define DRONE_SWEEP_VIEW_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"

#include "drone_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

// Forward declarations to avoid heavy scanner.hpp include
class DroneScanner;
struct ScanConfig;

// Opaque declaration — defined in drone_sweep_view.cpp (dense & ordered:
// W1_START..W4_END, then W1_DW0_FROM..W4_DW4_TO). Only the .cpp decodes ids.
enum class SweepFieldID : uint8_t;

/**
 * @brief Per-window sweep configuration data (POD, no UI widgets)
 * @note SRAM: 117 bytes (3×FreqHz=24B + enabled=1+7pad + det_windows[5]×16=80B
 *       + name_idx[5]=5B). Total for array[4]: ~468 bytes BSS.
 * @note Stored as std::array<WindowData, MAX_SWEEP_WINDOWS> in DroneSweepView
 *       to avoid duplicating widgets for each window.
 */
struct WindowData {
    FreqHz start_freq{0};
    FreqHz end_freq{0};
    FreqHz step_freq{0};
    bool enabled{false};
    // 5 inclusive detection ranges; slot active iff start>0 && end>0 &&
    // start<=end. All inactive (0/0) → detect over the ENTIRE window range.
    // SWP invariant: every active slot is clamped into the window's own
    // [min(start_freq, end_freq), max(start_freq, end_freq)] span — see
    // clamp_det_mhz_to_window() / sanitize_det_ranges() in drone_sweep_view.cpp.
    std::array<FreqRangeHz, DETECTION_WINDOWS_PER_WINDOW> det_windows{};
    // Per-slot LABEL index into RANGE_NAMES (range_names.hpp); 0 = no label.
    // Edited in the lower-half Range/Name selector; persisted per slot.
    std::array<uint8_t, DETECTION_WINDOWS_PER_WINDOW> name_idx{};
};
/**
 * @brief Snapshot of the sweep-window bounds (MHz) handed to the keypad
 * @note Captured BY VALUE when the keypad is pushed. The keypad is modal, so
 *       the window bounds cannot change between push and the on_changed
 *       callback. min_mhz/max_mhz > 0 marks "clamp enabled" — the default
 *       {0, 0} is used for the window Start/End fields themselves (they
 *       DEFINE the clamp window and must stay free).
 */
struct DetRangeWindowClamp {
    int32_t min_mhz{0};
    int32_t max_mhz{0};

    [[nodiscard]] constexpr bool active() const noexcept {
        return min_mhz > 0 && max_mhz > 0;
    }
};

/**
 * @brief Zer0-heap selector over the Flash-resident RANGE_NAMES table.
 *
 * Drop-in replacement for an OptionsField whose options vector would cost
 * ~1.1 KB of heap (std::vector + 18 std::string slots) — the exact
 * allocation that starved the SWP-tab frequency keypad after the
 * detection-range-labels commit (23bc0a40).
 *
 * @note API mirrors ui::OptionsField (on_change(size_t, int32_t),
 *       set_by_value(), set_selected_index()) so the existing wiring
 *       compiles unchanged. Value space == index space into RANGE_NAMES.
 * @note Heap: 0 B. SRAM: ~40 B (member of SweepWindowView). Flash: ~200 B.
 * @note UI-thread only (paint / encoder / touch) — same contract as every
 *       other selector widget in this view.
 */
class RangeNameSelector : public ui::Widget {
public:
    using value_t = int32_t;

    std::function<void(size_t, value_t)> on_change{};

    RangeNameSelector(ui::Point parent_pos, size_t length) noexcept
        : ui::Widget{{parent_pos, {8 * static_cast<int>(length), 16}}},
          length_{length} {
        set_focusable(true);
    }

    RangeNameSelector(const RangeNameSelector&) = delete;
    RangeNameSelector& operator=(const RangeNameSelector&) = delete;

    [[nodiscard]] value_t value() const noexcept {
        return static_cast<value_t>(selected_index_);
    }

    void set_selected_index(size_t new_index, bool trigger_change = true) noexcept;
    void set_by_value(value_t v) noexcept;

    void paint(ui::Painter& painter) override;
    bool on_encoder(const ui::EncoderEvent delta) override;
    bool on_keyboard(const ui::KeyboardEvent key) override;
    bool on_touch(const ui::TouchEvent event) override;
    void on_focus() override;
    void on_blur() override;

private:
    const size_t length_;
    size_t selected_index_{0};
};

/**
 * @brief Single-window sweep configuration view (reuses widgets for all 4 windows)
 * @note Contains Start/End + Enabled + 5 detection ranges (From/To) for ONE
 *       window. DroneSweepView switches the data pointer to show different
 *       windows. (The sweep center pitch is auto-derived for gapless coverage —
 *       SweepWindow::init() ignores step_freq. See SWEEP_GAPLESS_STEP_MAX_HZ.)
 * @note SRAM: ~900 bytes heap (24 widgets × ~32 bytes each + labels
 *       + 1 OptionsField + RangeNameSelector). The range-label selector is
 *       zero-heap (Flash strings) — it replaced a second OptionsField whose
 *       18-entry options vector cost ~1.1 KB of heap.
 */
class SweepWindowView : public ui::View {
public:
    SweepWindowView(NavigationView& nav, const Rect parent_rect, DroneScanner* scanner_ptr) noexcept;

    SweepWindowView(const SweepWindowView&) = delete;
    SweepWindowView& operator=(const SweepWindowView&) = delete;

    void focus() override;

    /**
     * @brief Bind this view to a specific window's data
     * @param data Pointer to the WindowData to display/edit
     * @param window_index Window index (0-3) for frequency keypad routing
     */
    void bind(WindowData* data, uint8_t window_index) noexcept;

    /**
     * @brief Read current widget values back into the bound WindowData
     * @note Called by DroneSweepView before save
     * @note SWP bugfix: detection-range values are sanitized against the
     *       WIDGET Start/End bounds on every sync — a range outside the
     *       window's scan range is reset into [min(Start, End), max(Start, End)]
     *       before it reaches WindowData, ScanConfig or the settings file.
     */
    void sync_from_widgets() noexcept;

    /**
     * @brief Write WindowData values into widgets
     * @note Called after bind() and by DroneSweepView after loading config
     * @note Sync runs with per-widget on_change suppressed — the data side is
     *       already normalized; see sanitize_det_ranges() / sanitize_window_det_ranges()
     */
    void sync_to_widgets() noexcept;

    /**
     * @brief Frequency-keypad callback — invoked from the pushed keypad view
     * @param field_id Exact SweepFieldID that opened the keypad
     * @param f Frequency confirmed on the keypad (Hz)
     * @note Detection-range fields are clamped into the CURRENT window bounds
     *       before the value reaches the widget or the config mirror (SWP
     *       bugfix); window Start/End fields store the raw value.
     * @note ZERO-HEAP CLOSURE CONTRACT: on_changed captures {SweepFieldID (1 B),
     *       SweepWindowView* (4 B)} = 8 B — exactly std::function's inline
     *       storage (GCC 9.2.1 bits/std_function.h, _M_max_size =
     *       sizeof(_Nocopy_types) = 8 B on ARM32) → no operator new /
     *       chHeapAlloc per keypad open.
     */
    void on_freq_keypad_done(SweepFieldID field_id, rf::Frequency f) noexcept;

    /**
     * @brief Clamp one detection-range bound into the bound window's range
     * @param field The From/To NumberField whose widget value is corrected
     * @param entered_mhz The just-entered widget value, MHz (0 = slot OFF)
     * @note Widget-entry guard (encoder / keyboard steps): a value outside the
     *       bound window's [Start, End] is reset to the window min/max. The
     *       correction set_value() re-fires on_change only when the value
     *       differs, so the clamp terminates at depth 2. 0 passes through —
     *       the documented slot-OFF sentinel (is_det_win_slot_active).
     */
    void clamp_det_field_to_window(ui::NumberField& field, int32_t entered_mhz) noexcept;

    /**
     * @brief Clamp all 10 detection-range bounds into the widget window bounds
     * @note Invoked whenever the window Start/End fields change: previously
     *       valid ranges are re-clamped into the new scan range, restoring the
     *       "ranges always live inside the window" invariant.
     * @note Call after sync_from_widgets() on switch/save (widget bounds read).
     * @return true if at least one field changed (caller may repaint)
     */
    bool sanitize_det_ranges() noexcept;

  private:
    /**
     * @brief Reload the label selector from the bound data (slot-clamped)
     */
    void refresh_label_selector() noexcept;

    /**
     * @brief Clamp snapshot for the modal frequency keypad (MHz)
     * @return {min(Start, End), max(Start, End)} of the CURRENT widget values,
     *         or {0, 0} when no window data is bound (clamp disabled)
     */
    DetRangeWindowClamp det_range_clamp() const noexcept;

    /**
     * @brief Resolve a detection-range bound widget by slot index
     * @param slot 0..4 (R1..R5)
     * @param is_end true → To field, false → From field
     * @note Lets the on_select closures capture only {this, slot} = 8 B —
     *       std::function inline storage, zero heap.
     */
    ui::NumberField* dw_field_(uint8_t slot, bool is_end) noexcept;

    NavigationView& nav_;
    DroneScanner* scanner_ptr_;
    WindowData* bound_data_{nullptr};
    uint8_t bound_index_{0};

    // Left column — ALL focusable rows share the 16px row grid with the
    // detection-window column on the right. Labels sit INLINE with their
    // fields (Start|field on row 1, End|field on row 2, Enabled on row 3), so
    // the geometric focus manager (FocusManager::update → rect_distances,
    // see ui_focus.cpp) always finds an axis-aligned neighbour: Up/Down walk
    // rows 1→2→3 without skips, Left/Right swap columns on the same row.
    ui::Labels labels_{
        {{UI_POS_X(0), UI_POS_Y(0)}, "-- Win --", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(1)}, "Start", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(2)}, "End", Color::white()},
    };
    ui::Checkbox check_enabled_{{UI_POS_X(0), UI_POS_Y(3)}, 8, "Enabled", false};
    ui::NumberField field_start_{{UI_POS_X(6), UI_POS_Y(1)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_end_{{UI_POS_X(4), UI_POS_Y(2)}, 5, {100, 7200}, 1, ' '};

    // Detection windows — right side (5 ranges × From/To), rows 0-5.
    // A range is OFF when BOTH fields are 0 (default); when at least one range
    // is ON, detections outside every ON range are ignored (scanner-side gate).
    // SWP bugfix: any non-zero From/To entry is clamped into the enclosing
    // window's [Start, End] scan range on every entry path (encoder, keypad,
    // settings load) — a range outside the window is reset to the window
    // min/max instead of silently gating a band the sweep never visits.
    ui::Labels labels_dw_{
        {{UI_POS_X(13), UI_POS_Y(0)}, "From", Color::white()},
        {{UI_POS_X(19), UI_POS_Y(0)}, "To", Color::white()},
    };
    ui::Labels labels_dw_idx_{
        {{UI_POS_X(12), UI_POS_Y(1)}, "1", Color::grey()},
        {{UI_POS_X(12), UI_POS_Y(2)}, "2", Color::grey()},
        {{UI_POS_X(12), UI_POS_Y(3)}, "3", Color::grey()},
        {{UI_POS_X(12), UI_POS_Y(4)}, "4", Color::grey()},
        {{UI_POS_X(12), UI_POS_Y(5)}, "5", Color::grey()},
    };
    ui::NumberField field_dw0_start_{{UI_POS_X(13), UI_POS_Y(1)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw0_end_{{UI_POS_X(19), UI_POS_Y(1)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw1_start_{{UI_POS_X(13), UI_POS_Y(2)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw1_end_{{UI_POS_X(19), UI_POS_Y(2)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw2_start_{{UI_POS_X(13), UI_POS_Y(3)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw2_end_{{UI_POS_X(19), UI_POS_Y(3)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw3_start_{{UI_POS_X(13), UI_POS_Y(4)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw3_end_{{UI_POS_X(19), UI_POS_Y(4)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw4_start_{{UI_POS_X(13), UI_POS_Y(5)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_dw4_end_{{UI_POS_X(19), UI_POS_Y(5)}, 5, {0, 7200}, 1, ' '};

    // Range-label editor — lower half (rows 8-9, below the SAVE/DEFAULTS row):
    // pick the range slot (R1..R5 of THIS window), then scroll a hardcoded
    // label from RANGE_NAMES (Flash-only strings). Detections landing in that
    // slot's range show the label on the drone list instead of "Unknown".
    ui::Labels labels_name_{
        {{UI_POS_X(0), UI_POS_Y(8)}, "Range:", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(9)}, "Name:", Color::white()},
    };
    ui::OptionsField field_label_slot_{
        {UI_POS_X(7), UI_POS_Y(8)},
        4,
        {
            {"R1", 0},
            {"R2", 1},
            {"R3", 2},
            {"R4", 3},
            {"R5", 4},
        }
    };
    // Zero-heap selector: draws the label straight from the Flash-resident
    // RANGE_NAMES table — no options vector, no std::string (saves ~1.1 KB
    // of heap vs an OptionsField with 18 entries; that vector was the
    // allocation that starved the SWP-tab frequency keypad).
    RangeNameSelector field_label_name_{{UI_POS_X(7), UI_POS_Y(9)}, 10};
    uint8_t label_slot_{0};  // currently edited range slot (0-4)
};

/**
 * @brief Sweep settings view — accessible via SWP button
 * @note Uses a SINGLE SweepWindowView that swaps data for each window.
 *       A window selector (OptionsField) switches which window is displayed.
 *       Total SRAM: ~1,260B (SweepWindowView ~800B + WindowData[4] 448B + UI ~10B)
 *       vs old design: ~4,800B (2 group views ~2,400B each + TabView + buttons)
 */
class DroneSweepView : public ui::View {
public:
    explicit DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept;

    ~DroneSweepView() noexcept override;

    DroneSweepView(const DroneSweepView&) = delete;
    DroneSweepView& operator=(const DroneSweepView&) = delete;

    void focus() override;

    std::string title() const override {
        static const std::string t = "SWP Settings";  // SSO — no heap allocation
        return t;
    }

private:
    static constexpr uint8_t NUM_WINDOWS = MAX_SWEEP_WINDOWS;

    NavigationView& nav_;
    DroneScanner* scanner_ptr_;

    // Window data for all 4 windows (compact POD array, 288 bytes BSS)
    std::array<WindowData, NUM_WINDOWS> windows_{};

    // Single reusable view (~480 bytes) — bound to windows_[selected]
    SweepWindowView sweep_view_;

    // Window selector — positioned above "-- Window --" label
    ui::OptionsField field_window_select_{
        {UI_POS_X(0), UI_POS_Y(1)},
        5,
        {
            {"Win 1", 0},
            {"Win 2", 1},
            {"Win 3", 2},
            {"Win 4", 3},
        }
    };

    // Buttons — row 8, directly below the detection-window grid: Down from the
    // bottom row (dw4 / Enabled) lands on them in one predictable step instead
    // of teleporting to y=285 (~10 empty rows below the editor grid).
    ui::Button button_defaults_{{UI_POS_X(15), UI_POS_Y(8), UI_POS_WIDTH(7), 20}, "DEFAULTS"};
    ui::Button button_save_{{UI_POS_X(22), UI_POS_Y(8), UI_POS_WIDTH(7), 20}, "SAVE"};

    uint8_t selected_window_{0};

    void switch_to_window(uint8_t index) noexcept;
    void save_settings() noexcept;
    void apply_defaults() noexcept;

    /**
     * @brief Normalize the freshly loaded workspace config (SWP bugfix)
     * @note Configs persisted by older firmware may contain detection ranges
     *       OUTSIDE their window's scan range (the old UI accepted any value
     *       in {0, 7200} MHz). Before the data reaches the widgets, every
     *       range is clamped into [min(Start, End), max(Start, End)] of its
     *       window (0 = slot OFF passes through). The first SAVE then
     *       rewrites the healed values into the settings file.
     */
    void sanitize_window_det_ranges() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SWEEP_VIEW_HPP
