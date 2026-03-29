#ifndef DRONE_SWEEP_VIEW_HPP
#define DRONE_SWEEP_VIEW_HPP

#include <cstdint>
#include <cstddef>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"

#include "drone_types.hpp"
#include "constants.hpp"
#include "scanner.hpp"

namespace drone_analyzer {

class DroneScanner;

/**
 * @brief Sweep settings view — accessible via SWP button
 * @note Flat layout (no TabView) — all fields directly in view for focus support
 * @note Save writes sweep keys to SETTINGS/eda_settings.txt
 * @note Defaults resets both sweep windows to factory values
 */
class DroneSweepView : public ui::View {
public:
    explicit DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept;

    ~DroneSweepView() noexcept override;

    DroneSweepView(const DroneSweepView&) = delete;
    DroneSweepView& operator=(const DroneSweepView&) = delete;

    void focus() override;

    std::string title() const override { return "SWP Settings"; }

private:
    NavigationView& nav_;
    DroneScanner* scanner_ptr_;
    ScanConfig original_config_;

    // SW1 section
    ui::Labels labels_sw1_{
        {{UI_POS_X(0), UI_POS_Y(0)}, "-- Window 1 --", Theme::getInstance()->fg_cyan->foreground},
        {{UI_POS_X(1), UI_POS_Y(1)}, "Start(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(3)}, "End(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(5)}, "Step(kHz):", Theme::getInstance()->fg_light->foreground},
    };
    ui::NumberField field_sw1_start_{{UI_POS_X(1), UI_POS_Y(2)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_sw1_end_{{UI_POS_X(1), UI_POS_Y(4)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_sw1_step_{{UI_POS_X(1), UI_POS_Y(6)}, 5, {1000, 99999}, 1000, ' '};

    // SW2 section
    ui::Labels labels_sw2_{
        {{UI_POS_X(0), UI_POS_Y(8)}, "-- Window 2 --", Theme::getInstance()->fg_green->foreground},
        {{UI_POS_X(1), UI_POS_Y(10)}, "Start(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(12)}, "End(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(14)}, "Step(kHz):", Theme::getInstance()->fg_light->foreground},
    };
    ui::Checkbox check_sw2_enabled_{{UI_POS_X(1), UI_POS_Y(9)}, 8, "Enabled", false};
    ui::NumberField field_sw2_start_{{UI_POS_X(1), UI_POS_Y(11)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_sw2_end_{{UI_POS_X(1), UI_POS_Y(13)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_sw2_step_{{UI_POS_X(1), UI_POS_Y(15)}, 5, {1000, 99999}, 1000, ' '};

    // Buttons
    ui::Button button_defaults_{{UI_POS_X(0), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(13), 20}, "DEFAULTS"};
    ui::Button button_save_{{UI_POS_X(15), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(14), 20}, "SAVE"};

    void save_settings() noexcept;
    void apply_defaults() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SWEEP_VIEW_HPP
