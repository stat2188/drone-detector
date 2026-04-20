#include <cstdint>
#include <cstring>
#include <vector>
#include <array>

#include "ch.h"

#include "ui.hpp"
#include "ui_text.hpp"
#include "ui_receiver.hpp"
#include "portapack_persistent_memory.hpp"
#include "portapack.hpp"

#include "drone_scanner_ui.hpp"
#include "pattern_manager_view.hpp"
#include "scanner.hpp"
#include "pattern_manager.hpp"

namespace drone_analyzer {

/**
 * @note UI FRAMEWORK LIMITATION:
 * OptionsField::set_options() requires std::vector<options_t> parameter.
 * This is a limitation of the existing UI widget system that cannot be changed
 * without rewriting the entire UI framework.
 *
 * WORKAROUND: Use scoped std::vector with emplace_back (move semantics).
 * The vector is destroyed immediately after set_options() returns, so heap usage
 * is transient and bounded (max 21 options = ~336 bytes).
 *
 * ALTERNATIVE: Rewrite OptionsField to accept pointer+size (requires ui_widget.hpp changes).
 *
 * DIAMOND CODE STANDARD: Core logic (pattern_manager.cpp) uses std::array.
 * UI layer (this file) must conform to OptionsField API requirements.
 */

    PatternManagerView::PatternManagerView(NavigationView& nav) noexcept
    : View()
    , nav_(nav)
    , labels_{
        {{UI_POS_X(0), UI_POS_Y(0)}, "Pattern Manager", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(2)}, "Count: --", Color::white()}
    }
    , field_patterns_{{0, LIST_Y}, 20, {}, false}
    , button_add_{{UI_POS_X(0), 284, UI_POS_WIDTH(3), 28}, "Add"}
    , button_edit_{{UI_POS_X(7), 284, UI_POS_WIDTH(3), 28}, "Edt"}
    , button_delete_{{UI_POS_X(11), 284, UI_POS_WIDTH(3), 28}, "Del"}
    , button_clear_all_{{UI_POS_X(15), 284, UI_POS_WIDTH(3), 28}, "Clr"}
    , button_back_{{UI_POS_X(22), 284, UI_POS_WIDTH(3), 28}, "Back"} {

    add_children({
        &labels_,
        &field_patterns_,
        &button_add_,
        &button_edit_,
        &button_delete_,
        &button_clear_all_,
        &button_back_
    });

    button_add_.on_select = [this](ui::Button&) {
        create_new_pattern();
    };

    button_edit_.on_select = [this](ui::Button&) {
        show_pattern_details();
    };

    button_delete_.on_select = [this](ui::Button&) {
        delete_selected_pattern();
    };

    button_clear_all_.on_select = [this](ui::Button&) {
        clear_all_patterns();
    };

    button_back_.on_select = [this](ui::Button&) {
        nav_.pop();
    };

    field_patterns_.on_change = [this](size_t index, int32_t) {
        selected_index_ = static_cast<uint8_t>(index);
    };
}

void PatternManagerView::focus() noexcept {
    DroneScanner& scanner_ref = get_scanner_instance();
    pattern_manager_ptr_ = &scanner_ref.get_pattern_manager();

    refresh_list();
}

void PatternManagerView::refresh_list() noexcept {
    if (pattern_manager_ptr_ == nullptr) {
        return;
    }

    const size_t pattern_count = pattern_manager_ptr_->get_pattern_count();

    std::vector<ui::OptionsField::option_t> options;
    options.reserve(21);
    
    char item_str[64];

    for (size_t i = 0; i < pattern_count && i < 20; ++i) {
        const SignalPattern* pattern = pattern_manager_ptr_->get_pattern(i);
        if (pattern != nullptr) {
            const char* status = pattern->is_enabled() ? "+" : "-";
            snprintf(item_str, sizeof(item_str), "[%s] %.28s", status, pattern->name);
            item_str[sizeof(item_str) - 1] = '\0';
            options.emplace_back(item_str, static_cast<int32_t>(i));
        }
    }

    if (pattern_count == 0) {
        options.emplace_back("No patterns", 0);
    }

    field_patterns_.set_options(options);

    char count_str[32];
    snprintf(count_str, sizeof(count_str), "Count: %zu", pattern_count);
}

void PatternManagerView::show_pattern_details() noexcept {
    if (pattern_manager_ptr_ == nullptr || selected_index_ >= pattern_manager_ptr_->get_pattern_count()) {
        return;
    }

    const SignalPattern* pattern = pattern_manager_ptr_->get_pattern(selected_index_);
    if (pattern == nullptr) {
        return;
    }

    char info[128];
    snprintf(info, sizeof(info), "Pattern: %s\nThreshold: %d\nCorr: 0-1000",
             pattern->name, pattern->match_threshold);

    // Could show detailed view here in future
}

void PatternManagerView::delete_selected_pattern() noexcept {
    if (pattern_manager_ptr_ == nullptr || selected_index_ >= pattern_manager_ptr_->get_pattern_count()) {
        return;
    }

    const ErrorCode err = pattern_manager_ptr_->delete_pattern(selected_index_);
    if (err == ErrorCode::SUCCESS) {
        refresh_list();
    }
}

void PatternManagerView::clear_all_patterns() noexcept {
    if (pattern_manager_ptr_ == nullptr) {
        return;
    }

    pattern_manager_ptr_->clear_all_patterns();
    refresh_list();
}

void PatternManagerView::create_new_pattern() noexcept {
    // Future: open view to create pattern from current spectrum
    char info[64];
    snprintf(info, sizeof(info), "Create new pattern\nNot implemented yet");

    // Placeholder for future implementation
}

void PatternManagerView::paint(ui::Painter& painter) noexcept {
    (void)painter;
    // UI components handle their own rendering
}

PatternManagerView::~PatternManagerView() noexcept = default;

} // namespace drone_analyzer
