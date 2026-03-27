#ifndef DRONE_SWEEP_VIEW_HPP
#define DRONE_SWEEP_VIEW_HPP

#include <cstdint>
#include <cstddef>
#include <string>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_tabview.hpp"

#include "drone_types.hpp"
#include "constants.hpp"
#include "scanner.hpp"

namespace drone_analyzer {

class DroneScanner;

/**
 * @brief Sweep window 1 settings sub-view
 */
class Sweep1View : public ui::View {
public:
    Sweep1View() noexcept;

    void focus() override;

    void set_values(FreqHz start, FreqHz end, FreqHz step) noexcept;
    [[nodiscard]] FreqHz get_start() const noexcept;
    [[nodiscard]] FreqHz get_end() const noexcept;
    [[nodiscard]] FreqHz get_step() const noexcept;

private:
    static constexpr uint16_t START_MHZ_DEFAULT = 5700;
    static constexpr uint16_t END_MHZ_DEFAULT = 5900;
    static constexpr uint32_t STEP_KHZ_DEFAULT = 20000;

    ui::Labels labels_{
        {{UI_POS_X(1), UI_POS_Y(1)}, "Start(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(3)}, "End(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(5)}, "Step(kHz):", Theme::getInstance()->fg_light->foreground},
    };

    ui::NumberField field_start_{{UI_POS_X(1), UI_POS_Y(2)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_end_{{UI_POS_X(1), UI_POS_Y(4)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_step_{{UI_POS_X(1), UI_POS_Y(6)}, 5, {1000, 99999}, 1000, ' '};
};

/**
 * @brief Sweep window 2 settings sub-view
 */
class Sweep2View : public ui::View {
public:
    Sweep2View() noexcept;

    void focus() override;

    void set_values(bool enabled, FreqHz start, FreqHz end, FreqHz step) noexcept;
    [[nodiscard]] bool get_enabled() const noexcept;
    [[nodiscard]] FreqHz get_start() const noexcept;
    [[nodiscard]] FreqHz get_end() const noexcept;
    [[nodiscard]] FreqHz get_step() const noexcept;

private:
    static constexpr uint16_t START_MHZ_DEFAULT = 2400;
    static constexpr uint16_t END_MHZ_DEFAULT = 2500;
    static constexpr uint32_t STEP_KHZ_DEFAULT = 20000;

    ui::Checkbox check_enabled_{{UI_POS_X(1), UI_POS_Y(0)}, 8, "Enabled", false};

    ui::Labels labels_{
        {{UI_POS_X(1), UI_POS_Y(2)}, "Start(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(4)}, "End(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(6)}, "Step(kHz):", Theme::getInstance()->fg_light->foreground},
    };

    ui::NumberField field_start_{{UI_POS_X(1), UI_POS_Y(3)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_end_{{UI_POS_X(1), UI_POS_Y(5)}, 5, {100, 7200}, 1, ' '};
    ui::NumberField field_step_{{UI_POS_X(1), UI_POS_Y(7)}, 5, {1000, 99999}, 1000, ' '};
};

/**
 * @brief Sweep settings view — accessible via SWP button
 * @note Uses TabView to separate SW1 and SW2 settings
 * @note Save writes to SETTINGS/eda_settings.txt
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

    static constexpr int TAB_VIEW_Y = 3;
    static constexpr int TAB_CONTENT_H = 160;

    Rect view_rect_{0, UI_POS_Y(TAB_VIEW_Y), screen_width, TAB_CONTENT_H};

    Sweep1View view_sw1_;
    Sweep2View view_sw2_;

    ui::TabView tab_view_{
        {"SW1", Theme::getInstance()->fg_cyan->foreground, &view_sw1_},
        {"SW2", Theme::getInstance()->fg_green->foreground, &view_sw2_},
    };

    ui::Button button_defaults_{{UI_POS_X(0), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(13), 20}, "DEFAULTS"};
    ui::Button button_save_{{UI_POS_X(15), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(14), 20}, "SAVE"};

    void save_settings() noexcept;
    void apply_defaults() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SWEEP_VIEW_HPP
