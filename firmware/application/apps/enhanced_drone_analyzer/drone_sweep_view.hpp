#ifndef DRONE_SWEEP_VIEW_HPP
#define DRONE_SWEEP_VIEW_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"

#include "drone_types.hpp"
#include "constants.hpp"
#include "range_names.hpp"

namespace drone_analyzer {

// Forward declarations to avoid heavy scanner.hpp include
class DroneScanner;
struct ScanConfig;

// ============================================================================
// Flash-resident option tables for the SWP selectors (zero heap).
//
// These feed ui::StaticOptionField — names live in .rodata, values are plain
// ints, and NOTHING is copied into the heap at view construction time. They
// replace the previous std::vector<std::pair<std::string, int32_t>> options
// of ui::OptionsField, which allocated ~1.1 KB of heap on EVERY SWP open
// (fragmenting the arena until the FrequencyKeypadView push failed).
// ============================================================================
constexpr size_t SWEEP_WINDOW_OPTION_COUNT = 4;
constexpr ui::StaticOptionField::Option kSweepWindowOptions[SWEEP_WINDOW_OPTION_COUNT] = {
    {"Win 1", 0},
    {"Win 2", 1},
    {"Win 3", 2},
    {"Win 4", 3},
};

constexpr size_t SWEEP_RANGE_SLOT_OPTION_COUNT = 5;
constexpr ui::StaticOptionField::Option kSweepRangeSlotOptions[SWEEP_RANGE_SLOT_OPTION_COUNT] = {
    {"R1", 0},
    {"R2", 1},
    {"R3", 2},
    {"R4", 3},
    {"R5", 4},
};

constexpr size_t SWEEP_RANGE_NAME_OPTION_COUNT = RANGE_NAME_COUNT;
static_assert(SWEEP_RANGE_NAME_OPTION_COUNT > 0, "RANGE_NAMES table must not be empty");
constexpr ui::StaticOptionField::Option kSweepRangeNameOptions[SWEEP_RANGE_NAME_OPTION_COUNT] = {
    {RANGE_NAMES[0], 0}, {RANGE_NAMES[1], 1},  {RANGE_NAMES[2], 2},  {RANGE_NAMES[3], 3},
    {RANGE_NAMES[4], 4}, {RANGE_NAMES[5], 5},  {RANGE_NAMES[6], 6},  {RANGE_NAMES[7], 7},
    {RANGE_NAMES[8], 8}, {RANGE_NAMES[9], 9},  {RANGE_NAMES[10], 10}, {RANGE_NAMES[11], 11},
    {RANGE_NAMES[12], 12}, {RANGE_NAMES[13], 13}, {RANGE_NAMES[14], 14}, {RANGE_NAMES[15], 15},
    {RANGE_NAMES[16], 16}, {RANGE_NAMES[17], 17},
};
static_assert(kSweepRangeNameOptions[0].value == 0, "label index 0 must map to \"no label\"");
// The initializer list above is hand-expanded — catch drift vs RANGE_NAME_COUNT.
static_assert(sizeof(kSweepRangeNameOptions) / sizeof(kSweepRangeNameOptions[0]) ==
                  SWEEP_RANGE_NAME_OPTION_COUNT,
    "kSweepRangeNameOptions must list every RANGE_NAMES entry (SWP name selector)");

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
    std::array<FreqRangeHz, DETECTION_WINDOWS_PER_WINDOW> det_windows{};
    // Per-slot LABEL index into RANGE_NAMES (range_names.hpp); 0 = no label.
    // Edited in the lower-half Range/Name selector; persisted per slot.
    std::array<uint8_t, DETECTION_WINDOWS_PER_WINDOW> name_idx{};
};

/**
 * @brief Single-window sweep configuration view (reuses widgets for all 4 windows)
 * @note Contains Start/End + Enabled + 5 detection ranges (From/To) for ONE
 *       window. DroneSweepView switches the data pointer to show different
 *       windows. (The sweep center pitch is auto-derived for gapless coverage —
 *       SweepWindow::init() ignores step_freq. See SWEEP_GAPLESS_STEP_MAX_HZ.)
 * @note SRAM: ~840 bytes (24 widgets × ~30 bytes each + labels + 2 zero-heap
 *       StaticOptionFields; the old ui::OptionsField pair cost ~1.1 KB of
 *       HEAP per SWP open on top of that — see StaticOptionField docs)
 *       vs old design: ~2,400 bytes (2 group views × 30 widgets × ~32 bytes)
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
     */
    void sync_from_widgets() noexcept;

    /**
     * @brief Write WindowData values into widgets
     * @note Called after bind() and by DroneSweepView after loading config
     */
    void sync_to_widgets() noexcept;

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
    // Slot selector R1..R5 over a Flash-resident table — Heap: 0 B.
    ui::StaticOptionField field_label_slot_{
        {UI_POS_X(7), UI_POS_Y(8)},
        4,
        kSweepRangeSlotOptions,
        SWEEP_RANGE_SLOT_OPTION_COUNT};
    // Label selector over the Flash-resident RANGE_NAMES table — Heap: 0 B.
    // (Was ui::OptionsField: 18 heap blocks per SWP open starved the
    // FrequencyKeypadView allocation on 128 KB SRAM.)
    ui::StaticOptionField field_label_name_{
        {UI_POS_X(7), UI_POS_Y(9)},
        10,
        kSweepRangeNameOptions,
        SWEEP_RANGE_NAME_OPTION_COUNT};
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

    // Window selector — positioned above "-- Window --" label.
    // Flash-resident table + StaticOptionField — Heap: 0 B (was ~180 B).
    ui::StaticOptionField field_window_select_{
        {UI_POS_X(0), UI_POS_Y(1)},
        5,
        kSweepWindowOptions,
        SWEEP_WINDOW_OPTION_COUNT};

    // Buttons — row 8, directly below the detection-window grid: Down from the
    // bottom row (dw4 / Enabled) lands on them in one predictable step instead
    // of teleporting to y=285 (~10 empty rows below the editor grid).
    ui::Button button_defaults_{{UI_POS_X(15), UI_POS_Y(8), UI_POS_WIDTH(7), 20}, "DEFAULTS"};
    ui::Button button_save_{{UI_POS_X(22), UI_POS_Y(8), UI_POS_WIDTH(7), 20}, "SAVE"};

    uint8_t selected_window_{0};

    void switch_to_window(uint8_t index) noexcept;
    void save_settings() noexcept;
    void apply_defaults() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SWEEP_VIEW_HPP
