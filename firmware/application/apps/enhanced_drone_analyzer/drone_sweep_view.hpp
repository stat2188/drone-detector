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

/**
 * @brief Per-window sweep configuration data (POD, no UI widgets)
 * @note SRAM: 72 bytes (3×FreqHz=24B + enabled=1+7pad + exceptions[5]×8=40B)
 *       Total for array[4]: 288 bytes BSS.
 * @note Stored as std::array<WindowData, MAX_SWEEP_WINDOWS> in DroneSweepView
 *       to avoid duplicating widgets for each window.
 */
struct WindowData {
    FreqHz start_freq{0};
    FreqHz end_freq{0};
    FreqHz step_freq{0};
    bool enabled{false};
    std::array<FreqHz, EXCEPTIONS_PER_WINDOW> exceptions{};
};

/**
 * @brief Single-window sweep configuration view (reuses widgets for all 4 windows)
 * @note Contains Start/End/Step + Enabled + 5 exception fields for ONE window.
 *       DroneSweepView switches the data pointer to show different windows.
 * @note SRAM: ~480 bytes (15 widgets × ~32 bytes each)
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

    ui::Labels labels_{
        {{UI_POS_X(0), UI_POS_Y(0)}, "-- Window --", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(1)}, "Start(MHz):", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(3)}, "End(MHz):", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(5)}, "Step(kHz):", Color::white()},
    };
    ui::Checkbox check_enabled_{{UI_POS_X(1), UI_POS_Y(7)}, 8, "Enabled", false};
    ui::NumberField field_start_{{UI_POS_X(1), UI_POS_Y(2)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_end_{{UI_POS_X(1), UI_POS_Y(4)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_step_{{UI_POS_X(1), UI_POS_Y(6)}, 5, {17813, 99999}, 17813, ' '};

    // Exception fields — right side (5 slots)
    ui::Labels labels_exc_{
        {{UI_POS_X(16), UI_POS_Y(0)}, "Exc(MHz):", Color::white()},
    };
    ui::NumberField field_exc0_{{UI_POS_X(16), UI_POS_Y(1)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_exc1_{{UI_POS_X(16), UI_POS_Y(2)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_exc2_{{UI_POS_X(16), UI_POS_Y(3)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_exc3_{{UI_POS_X(16), UI_POS_Y(4)}, 5, {0, 7200}, 1, ' '};
    ui::NumberField field_exc4_{{UI_POS_X(16), UI_POS_Y(5)}, 5, {0, 7200}, 1, ' '};
};

/**
 * @brief Sweep settings view — accessible via SWP button
 * @note Uses a SINGLE SweepWindowView that swaps data for each window.
 *       A window selector (OptionsField) switches which window is displayed.
 *       Total SRAM: ~688B (SweepWindowView ~392B + WindowData[4] 288B + UI ~8B)
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

    // Buttons
    ui::NumberField field_exc_radius_{{UI_POS_X(0), 285}, 3, {1, 100}, 1, ' '};
    ui::Labels labels_exc_radius_{
        {{UI_POS_X(4), 285}, "Exc R(MHz):", Color::white()},
    };
    ui::Button button_defaults_{{UI_POS_X(15), 285, UI_POS_WIDTH(7), 20}, "DEFAULTS"};
    ui::Button button_save_{{UI_POS_X(22), 285, UI_POS_WIDTH(7), 20}, "SAVE"};

    uint8_t selected_window_{0};

    void switch_to_window(uint8_t index) noexcept;
    void save_settings() noexcept;
    void apply_defaults() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SWEEP_VIEW_HPP
