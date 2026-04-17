#include "pattern_settings_view.hpp"
#include <cstring>
#include <cstdio>

namespace drone_analyzer {

static constexpr uint16_t FIELD_LENGTH = 3;
static constexpr int SCREEN_HEIGHT = 240;
static constexpr int SCREEN_WIDTH = 320;
static constexpr uint16_t TITLE_Y = 4;
static constexpr uint16_t BUTTON_Y = 30;
static constexpr uint16_t BUTTON_HEIGHT = 30;
static constexpr uint16_t BUTTON_SPACING = 4;
static constexpr uint16_t LIST_Y = 70;
static constexpr uint16_t LIST_HEIGHT = 180;

PatternSettingsView::PatternSettingsView(
        ScanConfig& config,
        DroneScanner* scanner
    ) noexcept
    : View()
    , config_(config)
    , scanner_(scanner)
    , enable_label_(Rect{UI_POS_X(0), BUTTON_Y}, "Pattern Matching:")
    , check_enable_(Point{UI_POS_X(16), BUTTON_Y}, 18, "Enable", false)
    , threshold_label_(Rect{UI_POS_X(0), BUTTON_Y + BUTTON_HEIGHT + BUTTON_SPACING}, "Min Correlation:")
    , field_threshold_(Rect{UI_POS_X(16), BUTTON_Y + BUTTON_HEIGHT + BUTTON_SPACING}, FIELD_LENGTH, {0, 255},
                     static_cast<int32_t>(config_.pattern_min_correlation), ' ')
    , list_label_(Rect{UI_POS_X(0), LIST_Y}, "Loaded Patterns:")
    , pattern_count_label_(Rect{UI_POS_X(0), LIST_Y + 16}, "")
    , button_back_(Rect{UI_POS_X(0), LIST_Y + LIST_HEIGHT - 30, UI_POS_WIDTH(6), BUTTON_HEIGHT}, "Back")
    , button_refresh_(Rect{UI_POS_X(7), LIST_Y + LIST_HEIGHT - 30, UI_POS_WIDTH(6), BUTTON_HEIGHT}, "Reload")
    , button_clear_(Rect{UI_POS_X(14), LIST_Y + LIST_HEIGHT - 30, UI_POS_WIDTH(6), BUTTON_HEIGHT}, "Clear") {

    check_enable_.on_select = [this](Checkbox&, bool value) {
        config_.pattern_matching_enabled = value;
    };

    field_threshold_.on_change = [this](int32_t value) {
        config_.pattern_min_correlation = static_cast<uint8_t>(value);
    };

    button_back_.on_select = [this](Button&) {
        on_cancel();
    };

    button_refresh_.on_select = [this](Button&) {
        on_refresh();
    };

    button_clear_.on_select = [this](Button&) {
        on_clear();
    };

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

    painter.fill_rectangle({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, bg);

    painter.draw_string({UI_POS_X(0), TITLE_Y}, Theme::getInstance()->fg_light->font,
                     Theme::getInstance()->fg_light->foreground, bg, "Pattern Matching");

    check_enable_.set_value(config_.pattern_matching_enabled);
    field_threshold_.set_value(config_.pattern_min_correlation, false);

    if (scanner_ != nullptr) {
        const size_t count = scanner_->get_pattern_count();

        char count_buf[16];
        snprintf(count_buf, sizeof(count_buf), "Loaded: %d", static_cast<int>(count));
        pattern_count_label_.set(count_buf);

        const auto* patterns = scanner_->get_patterns();
        for (size_t i = 0; i < count && i < 8; ++i) {
            const auto* pattern = &patterns[i];
            if (pattern == nullptr) continue;
        }

        if (count > 8) {
            painter.draw_string({UI_POS_X(0), LIST_Y + 144}, Theme::getInstance()->fg_light->font,
                         Theme::getInstance()->fg_light->foreground, bg, "...");
        }
    } else {
        pattern_count_label_.set("No scanner");
    }
}

} // namespace drone_analyzer