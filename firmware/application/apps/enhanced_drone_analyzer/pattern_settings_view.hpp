#ifndef PATTERN_SETTINGS_VIEW_HPP
#define PATTERN_SETTINGS_VIEW_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include "ch.h"
#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_text.hpp"
#include "ui_painter.hpp"
#include "ui_menu.hpp"
#include "drone_types.hpp"
#include "pattern_types.hpp"
#include "pattern_manager.hpp"
#include "scanner.hpp"
#include "constants.hpp"

namespace drone_analyzer {

class PatternSettingsView : public ui::View {
public:
    explicit PatternSettingsView(
        ScanConfig& config,
        DroneScanner* scanner
    ) noexcept;
    ~PatternSettingsView() noexcept override;

    PatternSettingsView(const PatternSettingsView&) = delete;
    PatternSettingsView& operator=(const PatternSettingsView&) = delete;

    std::string title() const override { return "Pattern Settings"; }
    void paint(Painter& painter) override;
    void focus() override;

private:
    static constexpr uint16_t TITLE_Y = 4;
    static constexpr uint16_t BUTTON_Y = 30;
    static constexpr uint16_t BUTTON_HEIGHT = 30;
    static constexpr uint16_t BUTTON_SPACING = 4;
    static constexpr uint16_t LIST_Y = 70;
    static constexpr uint16_t LIST_HEIGHT = 180;

    ScanConfig& config_;
    DroneScanner* scanner_;

    ui::Text enable_label_;
    ui::Checkbox check_enable_;
    ui::Text threshold_label_;
    ui::NumberField field_threshold_;
    ui::Text list_label_;
    ui::Text pattern_count_label_;
    ui::Button button_back_;
    ui::Button button_refresh_;
    ui::Button button_clear_;

    std::function<void()> on_cancel;
    std::function<void()> on_refresh;
    std::function<void()> on_clear;
};

} // namespace drone_analyzer

#endif // PATTERN_SETTINGS_VIEW_HPP
