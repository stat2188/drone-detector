#include "pattern_settings_view.hpp"
#include <cstring>
#include <cstdio>

namespace drone_analyzer {

PatternSettingsView::PatternSettingsView(
        ScanConfig& config,
        DroneScanner* scanner
    ) noexcept
    : View()
    , config_(config)
    , scanner_(scanner)
    , enable_label_{{UI_POS_X(0), BUTTON_Y, UI_POS_WIDTH(15), 16},
                     "Pattern Matching:",
                     Theme::getInstance()->fg_light->foreground}
    , check_enable_{{UI_POS_X(16), BUTTON_Y, 18, UI_POS_HEIGHT},
                     {config_.pattern_matching_enabled}}
    , threshold_label_{{UI_POS_X(0), BUTTON_Y + BUTTON_HEIGHT + BUTTON_SPACING, UI_POS_WIDTH(15), 16},
                      "Min Correlation:",
                      Theme::getInstance()->fg_light->foreground}
    , field_threshold_{{UI_POS_X(16), BUTTON_Y + BUTTON_HEIGHT + BUTTON_SPACING, UI_POS_WIDTH(6), UI_POS_HEIGHT},
                       3,
                       {0, 255},
                       static_cast<int32_t>(config_.pattern_min_correlation),
                       ' '}
    , list_label_{{UI_POS_X(0), LIST_Y, UI_POS_WIDTH(30), 16},
                   "Loaded Patterns:",
                   Theme::getInstance()->fg_light->foreground}
    , pattern_count_label_{{UI_POS_X(0), LIST_Y + 16, UI_POS_WIDTH(30), 16},
                          "",
                          Theme::getInstance()->fg_light->foreground}
    , button_back_{{UI_POS_X(0), LIST_Y + LIST_HEIGHT - 30, UI_POS_WIDTH(6), BUTTON_HEIGHT},
                   "Back"}
    , button_refresh_{{UI_POS_X(7), LIST_Y + LIST_HEIGHT - 30, UI_POS_WIDTH(6), BUTTON_HEIGHT},
                      "Reload"}
    , button_clear_{{UI_POS_X(14), LIST_Y + LIST_HEIGHT - 30, UI_POS_WIDTH(6), BUTTON_HEIGHT},
                    "Clear"} {
    add_children({
        &enable_label_,
        &check_enable_,
        &threshold_label_,
        &field_threshold_,
        &list_label_,
        &pattern_count_label_,
        &button_back_,
        &button_refresh_,
        &button_clear_
    });
}

PatternSettingsView::~PatternSettingsView() noexcept {
}

void PatternSettingsView::focus() {
    button_back_.focus();
}

void PatternSettingsView::paint(Painter& painter) {
    const auto bg = Theme::getInstance()->bg_dark->background;
    const auto fg = Theme::getInstance()->fg_light->foreground;

    painter.fill_rectangle({0, 0, UI_POS_WIDTH(30), UI_POS_HEIGHT(320)}, bg);

    painter.draw_string({UI_POS_X(0), TITLE_Y}, "Pattern Matching", fg, bg);

    check_enable_.set_value(config_.pattern_matching_enabled);

    painter.draw_string({UI_POS_X(0), BUTTON_Y + 30}, "Min Correlation:", fg, bg);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", config_.pattern_min_correlation);
    painter.draw_string({UI_POS_X(16), BUTTON_Y + 30}, buf, fg, bg);

    const auto& pattern_manager = scanner_->get_pattern_manager();
    const size_t count = pattern_manager.get_pattern_count();

    snprintf(buf, sizeof(buf), "Loaded: %d", static_cast<int>(count));
    painter.draw_string({UI_POS_X(0), LIST_Y}, buf, fg, bg);

    for (size_t i = 0; i < count && i < 8; ++i) {
        const auto* pattern = pattern_manager.get_pattern(i);
        if (pattern == nullptr) continue;

        const uint16_t y = LIST_Y + 16 + (i * 16);

        if (pattern->is_enabled()) {
            painter.draw_string({UI_POS_X(0), y}, pattern->name, fg, bg);
        } else {
            painter.draw_string({UI_POS_X(0), y}, pattern->name,
                               Theme::getInstance()->fg_gray->foreground, bg);
        }
    }

    if (count > 8) {
        painter.draw_string({UI_POS_X(0), LIST_Y + 144}, "...", fg, bg);
    }
}

} // namespace drone_analyzer
