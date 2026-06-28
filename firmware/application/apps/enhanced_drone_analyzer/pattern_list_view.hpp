#ifndef PATTERN_LIST_VIEW_HPP
#define PATTERN_LIST_VIEW_HPP

#include <cstdint>
#include <cstddef>

#include "ui_painter.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"

#include "pattern_manager.hpp"
#include "scanner.hpp"
#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Minimal pattern list view — list + delete + back only.
 * @note No spectrum display, no capture, no LIVE mode.
 *       Pattern capture is handled inline by DroneScannerUI.
 *
 * Stack: ~968 bytes (11 × 64B option texts + 264B option_t array).
 * Flash: ~512 bytes (code).
 * SRAM: ~64 bytes instance.
 */
class PatternListView : public ui::View {
public:
    explicit PatternListView(NavigationView& nav) noexcept;

    PatternListView(const PatternListView&) = delete;
    PatternListView& operator=(const PatternListView&) = delete;

    void paint(ui::Painter& painter) override;
    void focus() override;
    void on_show() override;

    std::string title() const override { return "PTR List"; }

private:
    NavigationView& nav_;

    PatternManager* pm_{nullptr};

    ui::Labels labels_;
    ui::OptionsField field_patterns_;
    ui::Button button_delete_;
    ui::Button button_back_;
    ui::Text label_status_;

    uint8_t selected_index_{0};

    void refresh_list() noexcept;
    void delete_selected() noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_LIST_VIEW_HPP
