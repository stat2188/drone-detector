#include <cstdint>
#include <cstring>

#include "ui.hpp"
#include "ui_text.hpp"

#include "pattern_list_view.hpp"
#include "drone_scanner_ui.hpp"
#include "scanner.hpp"
#include "pattern_manager.hpp"
#include "constants.hpp"

namespace drone_analyzer {

PatternListView::PatternListView(NavigationView& nav) noexcept
    : View()
    , nav_(nav)
    , labels_{
        {{UI_POS_X(0), UI_POS_Y(0)}, "PTR List", Color::white()},
    }
    , field_patterns_{{0, 20}, 22, {}, false}
    , button_delete_{{UI_POS_X(0), 270, UI_POS_WIDTH(5), 20}, "Del"}
    , button_back_{{UI_POS_X(24), 270, UI_POS_WIDTH(3), 20}, "<="}
    , label_status_{{UI_POS_X(6), 270, UI_POS_WIDTH(18), 20}, ""} {

    add_children({
        &labels_,
        &field_patterns_,
        &button_delete_,
        &button_back_,
        &label_status_
    });

    button_back_.on_select = [this](ui::Button&) {
        nav_.pop();
    };

    button_delete_.on_select = [this](ui::Button&) {
        delete_selected();
    };

    field_patterns_.on_change = [this](size_t index, int32_t) {
        selected_index_ = static_cast<uint8_t>(index);
    };
}

void PatternListView::on_show() noexcept {
    DroneScanner* scanner_ptr = get_scanner_ptr();
    if (scanner_ptr != nullptr) {
        pm_ = &scanner_ptr->get_pattern_manager();
        (void)pm_->reload_patterns();
    }
    refresh_list();
}

void PatternListView::focus() noexcept {
    refresh_list();
    button_delete_.focus();
}

void PatternListView::paint(ui::Painter& painter) noexcept {
    (void)painter;
}

void PatternListView::refresh_list() noexcept {
    if (pm_ == nullptr) {
        label_status_.set("No scanner");
        set_dirty();
        return;
    }

    constexpr size_t MAX_OPTS = MAX_PATTERNS + 1;
    char texts[MAX_OPTS][64]{};
    ui::OptionsField::option_t opts[MAX_OPTS];
    size_t count = 0;

    const size_t pattern_count = pm_->get_pattern_count();
    for (size_t i = 0; i < pattern_count && i < MAX_PATTERNS && count < MAX_OPTS - 1; ++i) {
        const SignalPattern* p = pm_->get_pattern(i);
        if (p != nullptr) {
            const char* s = p->is_enabled() ? "+" : "-";
            snprintf(texts[count], sizeof(texts[count]), "[%s] %.20s", s, p->name);
            opts[count] = {texts[count], static_cast<int32_t>(i)};
            ++count;
        }
    }

    if (count == 0) {
        snprintf(texts[0], sizeof(texts[0]), "No patterns");
        opts[0] = {texts[0], 0};
        count = 1;
    }

    field_patterns_.set_options({opts, opts + count});

    char status[16];
    snprintf(status, sizeof(status), "%zu pattern(s)", pattern_count);
    label_status_.set(status);
    set_dirty();
}

void PatternListView::delete_selected() noexcept {
    if (pm_ == nullptr || selected_index_ >= pm_->get_pattern_count()) return;

    const ErrorCode err = pm_->delete_pattern(selected_index_);
    if (err == ErrorCode::SUCCESS) {
        DroneScanner* scanner_ptr = get_scanner_ptr();
        if (scanner_ptr != nullptr) {
            scanner_ptr->refresh_patterns();
        }
        refresh_list();
    } else {
        label_status_.set("Delete failed");
        set_dirty();
    }
}

} // namespace drone_analyzer
