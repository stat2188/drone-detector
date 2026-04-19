#ifndef PATTERN_MANAGER_VIEW_HPP
#define PATTERN_MANAGER_VIEW_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>

#include "ui_painter.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"

#include "pattern_manager.hpp"
#include "pattern_types.hpp"
#include "scanner.hpp"

namespace drone_analyzer {

class PatternManagerView : public ui::View {
public:
    explicit PatternManagerView(NavigationView& nav) noexcept;
    ~PatternManagerView() noexcept override;

    PatternManagerView(const PatternManagerView&) = delete;
    PatternManagerView& operator=(const PatternManagerView&) = delete;

    void paint(ui::Painter& painter) override;
    void focus() override;

    std::string title() const override { return "Pattern Manager"; }

private:
    static constexpr uint16_t LIST_Y = 40;
    static constexpr uint16_t LIST_HEIGHT = 180;
    static constexpr uint16_t LIST_VISIBLE_ITEMS = 8;

    NavigationView& nav_;

    PatternManager* pattern_manager_ptr_{nullptr};

    ui::Labels labels_;
    ui::OptionsField field_patterns_;
    ui::Button button_add_;
    ui::Button button_edit_;
    ui::Button button_delete_;
    ui::Button button_clear_all_;
    ui::Button button_back_;

    uint8_t selected_index_{0};

    void refresh_list() noexcept;
    void show_pattern_details() noexcept;
    void delete_selected_pattern() noexcept;
    void clear_all_patterns() noexcept;
    void create_new_pattern() noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MANAGER_VIEW_HPP
