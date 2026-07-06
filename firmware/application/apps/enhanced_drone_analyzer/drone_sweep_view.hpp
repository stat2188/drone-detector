#ifndef DRONE_SWEEP_VIEW_HPP
#define DRONE_SWEEP_VIEW_HPP

#include <cstdint>
#include <cstddef>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_tabview.hpp"

#include "drone_types.hpp"
#include "constants.hpp"
#include "scanner.hpp"

namespace drone_analyzer {

class DroneScanner;

/**
 * @brief Configures 2 sweep windows (start/end/step + 5 exception freqs each).
 * @param window_index 0 = windows 1-2, 1 = windows 3-4
 */
class SweepWindowGroupView : public ui::View {
public:
    SweepWindowGroupView(NavigationView& nav, Rect parent_rect, uint8_t window_index) noexcept;
    void focus() override;

    NumberField& field_start(uint8_t w) noexcept;
    NumberField& field_end(uint8_t w) noexcept;
    NumberField& field_step(uint8_t w) noexcept;
    Checkbox& check_enabled(uint8_t w) noexcept;
    NumberField& field_exc(uint8_t w, uint8_t slot) noexcept;

private:
    NavigationView& nav_;
    uint8_t window_index_;

    // Window A (index 0 in pair): Group0=always-on, Group1=has checkbox (win3)
    Labels labels_a_;
    Checkbox check_a_enabled_;
    NumberField field_a_start_;
    NumberField field_a_end_;
    NumberField field_a_step_;

    // Window B: optional (index 1 in pair)
    Labels labels_b_;
    Checkbox check_b_enabled_;
    NumberField field_b_start_;
    NumberField field_b_end_;
    NumberField field_b_step_;

    // Exception fields for both windows (5 each)
    // Individual members — NumberField has deleted copy/move, so C-style arrays cannot be aggregate-initialized
    Labels labels_exc_a_;
    NumberField field_exc_a0_, field_exc_a1_, field_exc_a2_, field_exc_a3_, field_exc_a4_;
    Labels labels_exc_b_;
    NumberField field_exc_b0_, field_exc_b1_, field_exc_b2_, field_exc_b3_, field_exc_b4_;
};

/**
 * @brief Sweep settings view — accessible via SWP button
 * @note TabView layout: Tab 1 = Win 1-2, Tab 2 = Win 3-4
 */
class DroneSweepView : public ui::View {
public:
    explicit DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept;
    ~DroneSweepView() noexcept override;

    DroneSweepView(const DroneSweepView&) = delete;
    DroneSweepView& operator=(const DroneSweepView&) = delete;

    void focus() override;

    std::string title() const override {
        static const std::string t = "SWP Settings";
        return t;
    }

private:
    static constexpr ui::Dim TAB_BAR_H = 24;

    NavigationView& nav_;
    DroneScanner* scanner_ptr_;
    ScanConfig original_config_;

    SweepWindowGroupView view_group1_;
    SweepWindowGroupView view_group2_;
    ui::TabView tab_view_;

    ui::NumberField field_exc_radius_{{UI_POS_X(0), 285}, 3, {1, 100}, 1, ' '};
    ui::Labels labels_exc_radius_{
        {{UI_POS_X(4), 285}, "Exc R(MHz):", Color::white()},
    };
    ui::Button button_defaults_{{UI_POS_X(15), 285, UI_POS_WIDTH(7), 20}, "DEFAULTS"};
    ui::Button button_save_{{UI_POS_X(22), 285, UI_POS_WIDTH(7), 20}, "SAVE"};

    void save_settings() noexcept;
    void apply_defaults() noexcept;
    void populate_from_config(const ScanConfig& cfg) noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SWEEP_VIEW_HPP
