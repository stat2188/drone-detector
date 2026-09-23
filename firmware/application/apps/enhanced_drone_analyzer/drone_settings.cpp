#include <cstdint>
#include <cstring>
#include <cstdio>

#include "drone_settings.hpp"
#include "settings_manager.hpp"
#include "constants.hpp"
#include "scanner.hpp"
#include "audio_alerts.hpp"
#include "drone_display.hpp"
#include "ui_receiver.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "receiver_model.hpp"
#include "drone_scanner_ui.hpp"

namespace drone_analyzer {

namespace {

// ============================================================================
// CfarModeSelector — zero-heap replacement for the 7-entry ui::OptionsField
// ============================================================================
// ui::OptionsField stores its options as
// std::vector<std::pair<std::string, int32_t>> (~196 B of heap for 7 entries,
// plus transient reallocations inside set_options()). The CFAR option table
// is compile-time Flash data, so the selector keeps a single index and
// resolves the label string at paint time. Heap: 0 B.

struct CfarOption {
    const char* name;
    int32_t value;
};

// Order MUST match the CFARMode enum sequence (constants.hpp): value == index.
constexpr CfarOption CFAR_OPTIONS[] = {
    {"OFF", static_cast<int32_t>(CFARMode::OFF)},
    {"CA", static_cast<int32_t>(CFARMode::CA)},
    {"GO", static_cast<int32_t>(CFARMode::GO)},
    {"SO", static_cast<int32_t>(CFARMode::SO)},
    {"HYBRID", static_cast<int32_t>(CFARMode::HYBRID)},
    {"OS", static_cast<int32_t>(CFARMode::OS)},
    {"VI", static_cast<int32_t>(CFARMode::VI)},
};
constexpr size_t CFAR_OPTION_COUNT = sizeof(CFAR_OPTIONS) / sizeof(CFAR_OPTIONS[0]);

constexpr size_t cfar_option_max_len() noexcept {
    size_t max_len = 0;
    for (size_t i = 0; i < CFAR_OPTION_COUNT; ++i) {
        size_t len = 0;
        while (CFAR_OPTIONS[i].name[len] != '\0') ++len;
        if (len > max_len) max_len = len;
    }
    return max_len;
}

// The selector paints inside a length*8 px box; a longer label would overflow
// into the neighbouring "Ref:" field (kept in sync with the widget length 7).
static_assert(cfar_option_max_len() <= 7,
    "CFAR option name no longer fits the CfarModeSelector box");

}  // namespace

// CfarModeSelector — OptionsField-compatible API, zero heap ------------------

void CfarModeSelector::set_selected_index(
    const size_t new_index, const bool trigger_change) noexcept {
    if (new_index >= CFAR_OPTION_COUNT) return;
    if (new_index != selected_index_ || trigger_change) {
        selected_index_ = new_index;
        if (on_change) {
            on_change(selected_index_, CFAR_OPTIONS[selected_index_].value);
        }
        set_dirty();
    }
}

void CfarModeSelector::set_by_value(const value_t v) noexcept {
    // Linear scan over a 7-entry Flash table — value space is tiny.
    for (size_t i = 0; i < CFAR_OPTION_COUNT; ++i) {
        if (CFAR_OPTIONS[i].value == v) {
            set_selected_index(i);
            return;
        }
    }
    set_selected_index(0);  // unknown value → OFF (CFARMode::OFF == 0)
}

bool CfarModeSelector::on_encoder(const ui::EncoderEvent delta) {
    // Wrap in both directions (parity with ui::OptionsField::on_encoder).
    int32_t new_value = static_cast<int32_t>(selected_index_) + delta;
    if (new_value < 0) {
        new_value = static_cast<int32_t>(CFAR_OPTION_COUNT) - 1;
    } else if (static_cast<size_t>(new_value) >= CFAR_OPTION_COUNT) {
        new_value = 0;
    }
    set_selected_index(static_cast<size_t>(new_value));
    return true;
}

bool CfarModeSelector::on_keyboard(const ui::KeyboardEvent key) {
    // Parity with ui::OptionsField::on_keyboard.
    if (key == '+' || key == ' ' || key == 10) return on_encoder(1);
    if (key == '-' || key == 8) return on_encoder(-1);
    return false;
}

bool CfarModeSelector::on_touch(const ui::TouchEvent event) {
    // Parity with ui::OptionsField::on_touch: tap = focus only, the value is
    // edited with the encoder.
    if (event.type == ui::TouchEvent::Type::Start) {
        focus();
    }
    return true;
}

void CfarModeSelector::on_focus() {
    set_dirty();  // repaint with the focused (inverted) style
}

void CfarModeSelector::on_blur() {
    set_dirty();  // repaint without the focused style
}

void CfarModeSelector::paint(ui::Painter& painter) {
    const auto paint_style = has_focus() ? style().invert() : style();

    // Dark background box erases the previous (possibly longer) label —
    // mirrors ui::OptionsField::paint().
    painter.fill_rectangle(
        {screen_rect().location(), {static_cast<ui::Dim>(length_ * 8), 16}},
        Theme::getInstance()->bg_darkest->background);

    // Option name lives in Flash (CFAR_OPTIONS) — zero heap, zero copy.
    painter.draw_string(
        screen_pos(), paint_style, CFAR_OPTIONS[selected_index_].name);
}

// ============================================================================
// DroneSettingsView Constructor / Destructor
// ============================================================================

DroneSettingsView::DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display) noexcept
    : ui::View()
    , labels_({
        {{UI_POS_X(1), UI_POS_Y(1)}, "Int(ms):", Color::white()},
        {{UI_POS_X(1), UI_POS_Y(3)}, "Sens:", Color::white()},
        {{UI_POS_X(13), UI_POS_Y(2)}, "Vol:", Color::white()},
        {{UI_POS_X(13), UI_POS_Y(3)}, "Cyc:", Color::white()},
        {{UI_POS_X(17), UI_POS_Y(5)}, "Mar:", Color::white()},
        {{UI_POS_X(17), UI_POS_Y(6)}, "Min:", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(5)}, "MaxW:", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(6)}, "Shrp:", Color::white()},
        {{UI_POS_X(10), UI_POS_Y(5)}, "Rat:", Color::white()},
        {{UI_POS_X(10), UI_POS_Y(6)}, "Vly:", Color::white()},
        {{UI_POS_X(10), UI_POS_Y(4)}, "Flat:", Color::white()},
        {{UI_POS_X(17), UI_POS_Y(4)}, "Sym:", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(0)}, "CFAR:", Color::white()},
        {{UI_POS_X(11), UI_POS_Y(0)}, "Ref:", Color::white()},
        {{UI_POS_X(17), UI_POS_Y(0)}, "Grd:", Color::white()},
        {{UI_POS_X(22), UI_POS_Y(0)}, "Thr:", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(16)}, "Lo:", Color::white()},
        {{UI_POS_X(8), UI_POS_Y(16)}, "Md:", Color::white()},
        {{UI_POS_X(16), UI_POS_Y(16)}, "Hi:", Color::white()},
        {{UI_POS_X(23), UI_POS_Y(16)}, "Cr:", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(7)}, "Mrg:", Color::white()},
    })
    , field_scan_interval_({UI_POS_X(1), UI_POS_Y(2)}, 4, {10, 1000}, 10, ' ')
    , field_rssi_threshold_({UI_POS_X(1), UI_POS_Y(4)}, 3, {0, 100}, 1, ' ')
    , field_volume_({UI_POS_X(17), UI_POS_Y(2)}, 2, {0, 99}, 1, ' ')
    , field_rssi_dec_cyc_({UI_POS_X(17), UI_POS_Y(3)}, 2, {1, 50}, 1, ' ')
    , field_freq_match_radius_({UI_POS_X(4), UI_POS_Y(7)}, 3, {0, 100}, 1, ' ')
    , check_audio_alerts_({UI_POS_X(1), UI_POS_Y(9)}, 6, "Audio", false)
    , check_spectrum_visible_({UI_POS_X(20), UI_POS_Y(9)}, 5, "SpVis", false)
    , check_timeline_visible_({UI_POS_X(20), UI_POS_Y(13)}, 5, "WF", false)

    , check_dwell_enabled_({UI_POS_X(1), UI_POS_Y(11)}, 6, "Dwell", false)
    , check_confirm_count_({UI_POS_X(1), UI_POS_Y(13)}, 8, "Confirm", false)
    , field_confirm_count_({UI_POS_X(13), UI_POS_Y(13)}, 2, {1, 10}, 1, ' ')
    , field_miss_tolerance_({UI_POS_X(20), UI_POS_Y(13)}, 2, {1, 20}, 1, ' ')
        , check_spectrum_detection_({UI_POS_X(20), UI_POS_Y(11)}, 5, "SpDet", false)
    , field_neighbor_margin_({UI_POS_X(17), UI_POS_Y(15)}, 2, {0, 15}, 1, ' ')
    , check_neighbor_margin_({UI_POS_X(20), UI_POS_Y(15)}, 4, "NB", false)
    , check_noise_blacklist_({UI_POS_X(1), UI_POS_Y(15)}, 8, "Blklist", false)
    , check_rssi_variance_({UI_POS_X(20), UI_POS_Y(7)}, 5, "RVar", false)
    , preview_({0, 152, 240, 48})
    , check_mahalanobis_({UI_POS_X(20), UI_POS_Y(2)}, 3, "MG", false)
    , field_mahalanobis_threshold_({UI_POS_X(22), UI_POS_Y(1)}, 3,
                               {MAHALANOBIS_THRESHOLD_MIN_X10, MAHALANOBIS_THRESHOLD_MAX_X10},
                               DEFAULT_MAHALOBIS_THRESHOLD_X10, ' ')
    , check_sensitive_mode_({UI_POS_X(10), UI_POS_Y(10)}, 4, "Sens", false)
    , field_spectrum_margin_({UI_POS_X(20), UI_POS_Y(5)}, 3, {5, 200}, 5, ' ')
    , field_spectrum_min_width_({UI_POS_X(20), UI_POS_Y(6)}, 3, {1, 100}, 1, ' ')
    , field_spectrum_max_width_({UI_POS_X(6), UI_POS_Y(5)}, 3, {2, 255}, 1, ' ')
    , field_spectrum_peak_sharpness_({UI_POS_X(6), UI_POS_Y(6)}, 3, {50, 250}, 5, ' ')
    , field_spectrum_peak_ratio_({UI_POS_X(13), UI_POS_Y(5)}, 3, {0, 255}, 5, ' ')
    , field_spectrum_valley_depth_({UI_POS_X(13), UI_POS_Y(6)}, 3, {0, 200}, 5, ' ')
    , field_spectrum_flatness_({UI_POS_X(14), UI_POS_Y(4)}, 3, {0, 100}, 5, ' ')
    , field_spectrum_symmetry_({UI_POS_X(20), UI_POS_Y(4)}, 3, {0, 100}, 5, ' ')
    , button_defaults_({UI_POS_X(0), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(13), 20}, "DEFAULT")
    , button_about_({UI_POS_X(13), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(2), 20}, "!")
    , button_save_({UI_POS_X(15), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(14), 20}, "SAVE")
    , check_shape_bypass_({UI_POS_X(9), UI_POS_Y(7)}, 5, "Byp", false)
    , check_median_enabled_({UI_POS_X(15), UI_POS_Y(7)}, 4, "Md+", false)
    // Threat shelf fields: length 4 — the range includes 4-glyph values
    // ("-120"); a 3-char field would paint outside its rect and leave stale
    // digits ("990" ghosts) when a 4-glyph value is replaced by a 3-glyph one.
    , field_threat_low_({UI_POS_X(3), UI_POS_Y(16)}, 4, {RSSI_MIN_DBM, RSSI_MAX_DBM}, 1, ' ')
    , field_threat_medium_({UI_POS_X(11), UI_POS_Y(16)}, 4, {RSSI_MIN_DBM, RSSI_MAX_DBM}, 1, ' ')
    , field_threat_high_({UI_POS_X(19), UI_POS_Y(16)}, 4, {RSSI_MIN_DBM, RSSI_MAX_DBM}, 1, ' ')
    , field_threat_critical_({UI_POS_X(26), UI_POS_Y(16)}, 4, {RSSI_MIN_DBM, RSSI_MAX_DBM}, 1, ' ')
    // Zero-heap selector over the Flash CFAR_OPTIONS table (was an
    // ui::OptionsField with 7 options ≈ 196 B of heap + transient reallocs).
    , field_cfar_mode_({UI_POS_X(4), UI_POS_Y(0)}, 7)
    , field_cfar_ref_cells_({UI_POS_X(14), UI_POS_Y(0)}, 2, {4, 64}, 4, ' ')
    , field_cfar_guard_cells_({UI_POS_X(20), UI_POS_Y(0)}, 1, {0, 8}, 1, ' ')
    , field_cfar_threshold_({UI_POS_X(25), UI_POS_Y(0)}, 3, {10, 100}, 5, ' ')
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , display_ptr_(display)
    , original_config_(config)
    , settings_()
    , settings_dirty_(false) {

    // Extract current scanner config into settings struct
    SettingsFileManager::extract_from_config(config, settings_);

    add_children({
        &preview_,
        &labels_,
        &field_scan_interval_,
        &field_rssi_threshold_,
        &field_volume_,
        &field_rssi_dec_cyc_,
        &field_freq_match_radius_,
        &check_audio_alerts_,
        &check_spectrum_visible_,
        &check_timeline_visible_,

        &check_dwell_enabled_,
        &check_confirm_count_,
        &field_confirm_count_,
        &field_miss_tolerance_,
        &check_spectrum_detection_,
        &field_neighbor_margin_,
        &check_neighbor_margin_,
        &check_noise_blacklist_,
        &check_rssi_variance_,
        &check_mahalanobis_,
        &field_mahalanobis_threshold_,
        &check_sensitive_mode_,
        &field_spectrum_margin_,
        &field_spectrum_min_width_,
        &field_spectrum_max_width_,
        &field_spectrum_peak_sharpness_,
        &field_spectrum_peak_ratio_,
        &field_spectrum_valley_depth_,
        &field_spectrum_flatness_,
        &field_spectrum_symmetry_,
        &button_defaults_,
        &button_about_,
        &button_save_,
        &check_shape_bypass_,
        &check_median_enabled_,
        &field_cfar_mode_,
        &field_cfar_ref_cells_,
        &field_cfar_guard_cells_,
        &field_cfar_threshold_,
        &field_threat_low_,
        &field_threat_medium_,
        &field_threat_high_,
        &field_threat_critical_,
    });

    // Load persisted settings from SD card (overrides config-based defaults)
    // If load fails, settings_ retains constructor defaults
    (void)SettingsFileManager::load(settings_);

    // Populate UI fields from loaded settings
    apply_settings_to_ui();

    // --- Callbacks ---

    field_scan_interval_.on_change = [this](int32_t v) {
        settings_.scan_interval_ms = static_cast<uint32_t>(v);
        settings_dirty_ = true;
    };

    field_rssi_threshold_.on_change = [this](int32_t v) {
        settings_.alert_rssi_threshold_dbm = -20 - v;
        settings_.scan_sensitivity = static_cast<uint8_t>(v);
        // Detection threshold must stay below the medium threat threshold,
        // otherwise the LOW band collapses (LOW becomes unreachable).
        normalize_threat_ladder(4);
        settings_dirty_ = true;
    };

    field_volume_.on_change = [this](int32_t v) {
        settings_.volume = static_cast<uint8_t>(v);
        portapack::receiver_model.set_normalized_headphone_volume(static_cast<uint8_t>(v));
        settings_dirty_ = true;
    };

    field_rssi_dec_cyc_.on_change = [this](int32_t v) {
        settings_.rssi_decrease_cycles = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_freq_match_radius_.on_change = [this](int32_t v) {
        settings_.freq_match_radius_mhz = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    check_audio_alerts_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.audio_alerts_enabled = v;
        AudioAlertManager::set_enabled(v);
        settings_dirty_ = true;
    };

    check_spectrum_visible_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.spectrum_visible = v;
        if (display_ptr_ != nullptr) {
            display_ptr_->set_spectrum_visible(v);
        }
        settings_dirty_ = true;
    };

    check_timeline_visible_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.timeline_visible = v;
        if (display_ptr_ != nullptr) {
            display_ptr_->set_timeline_visible(v);
        }
        settings_dirty_ = true;
    };



    check_dwell_enabled_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.dwell_enabled = v;
        settings_dirty_ = true;
    };

    check_confirm_count_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.confirm_count_enabled = v;
        field_confirm_count_.visible(v);
        settings_dirty_ = true;
    };

    field_confirm_count_.on_change = [this](int32_t v) {
        settings_.confirm_count = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_miss_tolerance_.on_change = [this](int32_t v) {
        settings_.miss_tolerance = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    check_noise_blacklist_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.noise_blacklist_enabled = v;
        settings_dirty_ = true;
    };

    check_spectrum_detection_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.spectrum_detection_enabled = v;
        set_shape_filter_visibility(v);
        settings_dirty_ = true;
    };

    check_neighbor_margin_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.neighbor_margin_db = v ? DEFAULT_NEIGHBOR_MARGIN_DB : 0;
        settings_dirty_ = true;
    };

    field_neighbor_margin_.on_change = [this](int32_t v) {
        settings_.neighbor_margin_db = v;
        settings_dirty_ = true;
    };

    check_rssi_variance_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.rssi_variance_enabled = v;
        settings_dirty_ = true;
    };

    // "Mar" is the BASE of the Step-3 gate (README §14). The scanner applies
    // shape_gate_margin(): normal mode + Sens>75 → Mar + (Sens-75)/2
    // (fresh default Sens 85 → effective gate = Mar+5, so Mar=20 gates at 25);
    // Sensitive mode → Mar-2 with NO scaling. Single source of truth:
    // DroneScanner::shape_gate_margin().
    field_spectrum_margin_.on_change = [this](int32_t v) {
        settings_.spectrum_margin = static_cast<uint8_t>(v);
        update_preview();
        settings_dirty_ = true;
    };

    field_spectrum_min_width_.on_change = [this](int32_t v) {
        settings_.spectrum_min_width = static_cast<uint8_t>(v);
        if (settings_.spectrum_min_width > settings_.spectrum_max_width) {
            settings_.spectrum_max_width = settings_.spectrum_min_width;
            field_spectrum_max_width_.set_value(settings_.spectrum_max_width);
        }
        update_preview();
        settings_dirty_ = true;
    };

    field_spectrum_max_width_.on_change = [this](int32_t v) {
        settings_.spectrum_max_width = static_cast<uint8_t>(v);
        if (settings_.spectrum_max_width < settings_.spectrum_min_width) {
            settings_.spectrum_min_width = settings_.spectrum_max_width;
            field_spectrum_min_width_.set_value(settings_.spectrum_min_width);
        }
        update_preview();
        settings_dirty_ = true;
    };

    field_spectrum_peak_sharpness_.on_change = [this](int32_t v) {
        settings_.spectrum_peak_sharpness = static_cast<uint8_t>(v);
        update_preview();
        settings_dirty_ = true;
    };

    field_spectrum_peak_ratio_.on_change = [this](int32_t v) {
        settings_.spectrum_peak_ratio = static_cast<uint8_t>(v);
        update_preview();
        settings_dirty_ = true;
    };

    field_spectrum_valley_depth_.on_change = [this](int32_t v) {
        settings_.spectrum_valley_depth = static_cast<uint8_t>(v);
        update_preview();
        settings_dirty_ = true;
    };

    field_spectrum_flatness_.on_change = [this](int32_t v) {
        settings_.spectrum_flatness = static_cast<uint8_t>(v);
        update_preview();
        settings_dirty_ = true;
    };

    field_spectrum_symmetry_.on_change = [this](int32_t v) {
        settings_.spectrum_symmetry = static_cast<uint8_t>(v);
        update_preview();
        settings_dirty_ = true;
    };

    // SAVE button: apply to scanner + save to SD card
    // FIX: Eliminated 160-byte exc[4][5] stack array by writing sweep fields
    // directly to settings_ BEFORE apply_to_config() overwrites them.
    // Stack budget: ~150 bytes (was ~310 bytes).
    button_save_.on_select = [this](ui::Button&) {
        // Commit logic lives in commit_to_scanner() — shared with the
        // destructor (apply-on-exit): the field callbacks only STAGE values
        // into settings_, so both exit paths must push them to the scanner.
        if (!commit_to_scanner()) {
            // Lazy static: ONE heap allocation on first open instead of a
            // fresh temporary per open. Framework ModalMessageView copies
            // the message anyway (title_/message_ are std::string), so a
            // temporary literal here would double the per-open cost.
            static const std::string k_invalid_settings_msg{
                "Invalid settings.\nCheck min<=max\nand valid ranges."};
            nav_.display_modal("Error", k_invalid_settings_msg);
            return;
        }
        settings_dirty_ = false;  // committed — destructor re-commit skipped
        save_settings_to_sd();
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        settings_ = SettingsStruct();
        portapack::receiver_model.set_normalized_headphone_volume(settings_.volume);
        settings_dirty_ = true;
        apply_settings_to_ui();
    };

    button_about_.on_select = [this](ui::Button&) {
        // Lazy static (~190 B): one heap allocation on first open instead of
        // two per open (source temporary + ModalMessageView copy). Content is
        // intentionally NOT shortened to <=15-char lines — the per-paint
        // split_string() vector cost is framework-side and unaffected.
        static const std::string k_about_msg{
            "Author: Kuznetsov Maxim\n"
            "Orenburg\n"
            "Card: 2202 20202 5787 1695\n"
            "YooMoney: 41001810704697\n"
            "TON: UQCdtMxQB5zbQBOICkY90l\n"
            "TQQqcs8V-V28Bf2AGvl8xOc5HR\n"
            "Telegram: @max_ai_master\n"
            "TM PowerHamster2188"};
        nav_.display_modal("About", k_about_msg);
    };

    // Median filter toggle (spike rejection on RSSI samples)
    check_median_enabled_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.median_enabled = v;
        settings_dirty_ = true;
    };

    // Very-strong shape-filter bypass toggle (default OFF)
    check_shape_bypass_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.shape_bypass_enabled = v;
        settings_dirty_ = true;
    };

    // CFAR callbacks
    field_cfar_mode_.on_change = [this](size_t, int32_t v) {
        settings_.cfar_mode = static_cast<CFARMode>(v);
        settings_dirty_ = true;
    };

    field_cfar_ref_cells_.on_change = [this](int32_t v) {
        settings_.cfar_ref_cells = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_cfar_guard_cells_.on_change = [this](int32_t v) {
        settings_.cfar_guard_cells = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_cfar_threshold_.on_change = [this](int32_t v) {
        settings_.cfar_threshold_x10 = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    // Mahalanobis gate callbacks
    check_mahalanobis_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.mahalanobis_enabled = v;
        settings_dirty_ = true;
    };

    // Sensitive mode toggle
    check_sensitive_mode_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.sensitive_mode = v;
        settings_dirty_ = true;
    };

    field_mahalanobis_threshold_.on_change = [this](int32_t v) {
        settings_.mahalanobis_threshold_x10 = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    // Threat threshold callbacks — enforce ordering: low <= medium <= high <= critical
    // AND keep medium above the detection threshold (guarantees a non-empty LOW band).
    // trigger_change=false prevents re-entrant callback chains when auto-adjusting neighbors.
    field_threat_low_.on_change = [this](int32_t v) {
        settings_.threat_low_dbm = v;
        normalize_threat_ladder(0);
        settings_dirty_ = true;
    };

    field_threat_medium_.on_change = [this](int32_t v) {
        settings_.threat_medium_dbm = v;
        normalize_threat_ladder(1);
        settings_dirty_ = true;
    };

    field_threat_high_.on_change = [this](int32_t v) {
        settings_.threat_high_dbm = v;
        normalize_threat_ladder(2);
        settings_dirty_ = true;
    };

    field_threat_critical_.on_change = [this](int32_t v) {
        settings_.threat_critical_dbm = v;
        normalize_threat_ladder(3);
        settings_dirty_ = true;
    };

}

bool DroneSettingsView::commit_to_scanner() noexcept {
    if (scanner_ptr_ == nullptr) return false;

    // Step 1: stash LIVE sweep fields from the scanner (SWP view may have
    // changed them after this view's ctor snapshot) into settings_ — the
    // struct BOTH consumers (SD save, apply below) read from.
    scanner_ptr_->get_config(g_workspace_cfg);
    settings_.sweep_start_freq = g_workspace_cfg.sweep_start_freq;
    settings_.sweep_end_freq = g_workspace_cfg.sweep_end_freq;
    settings_.sweep_step_freq = g_workspace_cfg.sweep_step_freq;
    settings_.sweep2_start_freq = g_workspace_cfg.sweep2_start_freq;
    settings_.sweep2_end_freq = g_workspace_cfg.sweep2_end_freq;
    settings_.sweep2_step_freq = g_workspace_cfg.sweep2_step_freq;
    settings_.sweep2_enabled = g_workspace_cfg.sweep2_enabled;
    settings_.sweep3_start_freq = g_workspace_cfg.sweep3_start_freq;
    settings_.sweep3_end_freq = g_workspace_cfg.sweep3_end_freq;
    settings_.sweep3_step_freq = g_workspace_cfg.sweep3_step_freq;
    settings_.sweep3_enabled = g_workspace_cfg.sweep3_enabled;
    settings_.sweep4_start_freq = g_workspace_cfg.sweep4_start_freq;
    settings_.sweep4_end_freq = g_workspace_cfg.sweep4_end_freq;
    settings_.sweep4_step_freq = g_workspace_cfg.sweep4_step_freq;
    settings_.sweep4_enabled = g_workspace_cfg.sweep4_enabled;
    for (uint8_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
        for (uint8_t i = 0; i < DETECTION_WINDOWS_PER_WINDOW; ++i) {
            // Both sides store the MHz mirror — direct POD copy.
            settings_.sweep_det_win_start_mhz[w][i] =
                g_workspace_cfg.sweep_det_win_start_mhz[w][i];
            settings_.sweep_det_win_end_mhz[w][i] =
                g_workspace_cfg.sweep_det_win_end_mhz[w][i];
            settings_.sweep_det_win_name_idx[w][i] =
                g_workspace_cfg.sweep_det_win_name_idx[w][i];
        }
    }

    // Step 2: rebuild from the ctor snapshot + user edits, then push.
    // apply_to_config() copies the (now-fresh) sweep fields from settings_
    // into g_workspace_cfg, so the scanner receives the correct sweep
    // config via set_config(). Validation failure keeps the old config.
    g_workspace_cfg = original_config_;
    SettingsFileManager::apply_to_config(settings_, g_workspace_cfg);
    return scanner_ptr_->set_config(g_workspace_cfg) == ErrorCode::SUCCESS;
}

DroneSettingsView::~DroneSettingsView() noexcept {
    // APPLY-ON-EXIT (D2 fix): every field callback only staged settings_ +
    // settings_dirty_ — before this fix the scanner received edits ONLY via
    // the SAVE button, so tweaks made and verified with the Back button were
    // silently discarded ("I turn the knobs, the filters do nothing").
    // Contract: Back = apply to the live scanner (session-only), SAVE =
    // apply + persist to SD. While this view is open the parent has stopped
    // the scanner thread and spectrum streaming (button_settings_ in
    // drone_scanner_ui.cpp), so DATA_MUTEX is uncontended here.
    // Validation failure inside set_config keeps the previous config —
    // fail-safe; no modal is possible mid-destruction.
    if (!settings_dirty_) return;
    (void)commit_to_scanner();
}

void DroneSettingsView::paint(ui::Painter& painter) {
    (void)painter;
}

void DroneSettingsView::focus() {
    field_scan_interval_.focus();
}

// ============================================================================
// UI Population
// ============================================================================

void DroneSettingsView::apply_settings_to_ui() noexcept {
    // Sync the gate tracker BEFORE any set_value() can fire the Sens on_change
    // callback (e.g. the DEFAULTS button path, where callbacks are already
    // attached): this makes the ladder-translation delta zero for a pure
    // reload, so loaded/default shelf values are never spuriously shifted.
    prev_gate_dbm_ = settings_.alert_rssi_threshold_dbm;

    field_scan_interval_.set_value(settings_.scan_interval_ms);
    {
        const int32_t sens = -(settings_.alert_rssi_threshold_dbm + 20);
        field_rssi_threshold_.set_value(sens < 0 ? 0 : (sens > 100 ? 100 : sens));
    }
    field_volume_.set_value(static_cast<int32_t>(settings_.volume));
    field_rssi_dec_cyc_.set_value(static_cast<int32_t>(settings_.rssi_decrease_cycles));
    field_freq_match_radius_.set_value(static_cast<int32_t>(settings_.freq_match_radius_mhz));
    check_audio_alerts_.set_value(settings_.audio_alerts_enabled);
    check_spectrum_visible_.set_value(settings_.spectrum_visible);
    check_timeline_visible_.set_value(settings_.timeline_visible);

    check_dwell_enabled_.set_value(settings_.dwell_enabled);
    check_confirm_count_.set_value(settings_.confirm_count_enabled);
    field_confirm_count_.set_value(static_cast<int32_t>(settings_.confirm_count));
    field_confirm_count_.visible(settings_.confirm_count_enabled);
    field_miss_tolerance_.set_value(static_cast<int32_t>(settings_.miss_tolerance));
    check_noise_blacklist_.set_value(settings_.noise_blacklist_enabled);
    check_spectrum_detection_.set_value(settings_.spectrum_detection_enabled);
    field_spectrum_margin_.set_value(static_cast<int32_t>(settings_.spectrum_margin));
    field_spectrum_min_width_.set_value(static_cast<int32_t>(settings_.spectrum_min_width));
    field_spectrum_max_width_.set_value(static_cast<int32_t>(settings_.spectrum_max_width));
    field_spectrum_peak_sharpness_.set_value(static_cast<int32_t>(settings_.spectrum_peak_sharpness));
    field_spectrum_peak_ratio_.set_value(static_cast<int32_t>(settings_.spectrum_peak_ratio));
    field_spectrum_valley_depth_.set_value(static_cast<int32_t>(settings_.spectrum_valley_depth));
    field_spectrum_flatness_.set_value(static_cast<int32_t>(settings_.spectrum_flatness));
    field_spectrum_symmetry_.set_value(static_cast<int32_t>(settings_.spectrum_symmetry));
    check_neighbor_margin_.set_value(settings_.neighbor_margin_db > 0);
    field_neighbor_margin_.set_value(static_cast<int32_t>(settings_.neighbor_margin_db));
    check_rssi_variance_.set_value(settings_.rssi_variance_enabled);
    check_shape_bypass_.set_value(settings_.shape_bypass_enabled);
    check_median_enabled_.set_value(settings_.median_enabled);
    check_mahalanobis_.set_value(settings_.mahalanobis_enabled);
    field_mahalanobis_threshold_.set_value(static_cast<int32_t>(settings_.mahalanobis_threshold_x10));
    check_sensitive_mode_.set_value(settings_.sensitive_mode);
    field_cfar_mode_.set_by_value(static_cast<int32_t>(settings_.cfar_mode));
    field_cfar_ref_cells_.set_value(static_cast<int32_t>(settings_.cfar_ref_cells));
    field_cfar_guard_cells_.set_value(static_cast<int32_t>(settings_.cfar_guard_cells));
    field_cfar_threshold_.set_value(static_cast<int32_t>(settings_.cfar_threshold_x10));

    // Threat thresholds (trigger_change=false: suppress cascading callbacks during init)
    field_threat_low_.set_value(settings_.threat_low_dbm, false);
    field_threat_medium_.set_value(settings_.threat_medium_dbm, false);
    field_threat_high_.set_value(settings_.threat_high_dbm, false);
    field_threat_critical_.set_value(settings_.threat_critical_dbm, false);

    // Safety net: enforce the LOW-reachability invariant on any loaded/old config,
    // without disturbing the user's ordering choices (edited_field=4; the gate
    // tracker was synced above, so the translation delta is zero here).
    normalize_threat_ladder(4);

    // Set initial visibility based on spectrum detection state
    set_shape_filter_visibility(settings_.spectrum_detection_enabled);

    update_preview();
}

// ============================================================================
// Preview Update Helper
// ============================================================================

void DroneSettingsView::update_preview() noexcept {
    preview_.set_params(
        settings_.spectrum_margin,
        settings_.spectrum_min_width,
        settings_.spectrum_max_width,
        settings_.spectrum_peak_sharpness,
        settings_.spectrum_peak_ratio,
        settings_.spectrum_valley_depth,
        settings_.spectrum_flatness,
        settings_.spectrum_symmetry);
}

void DroneSettingsView::set_shape_filter_visibility(bool visible) noexcept {
    field_spectrum_margin_.visible(visible);
    field_spectrum_min_width_.visible(visible);
    field_spectrum_max_width_.visible(visible);
    field_spectrum_peak_sharpness_.visible(visible);
    field_spectrum_peak_ratio_.visible(visible);
    field_spectrum_valley_depth_.visible(visible);
    field_spectrum_flatness_.visible(visible);
    field_spectrum_symmetry_.visible(visible);
    check_shape_bypass_.visible(visible);
    check_median_enabled_.visible(visible);
    // CFAR fields are also gated by spectrum detection
    field_cfar_mode_.visible(visible);
    field_cfar_ref_cells_.visible(visible);
    field_cfar_guard_cells_.visible(visible);
    field_cfar_threshold_.visible(visible);
}

// ============================================================================
// Threat Ladder Normalization
// ============================================================================
//
// Enforces ordering plus the LOW-reachability invariants after any threat-field
// or sensitivity edit:
//   (1) low <= medium <= high <= critical
//   (2) medium > detection_threshold + RSSI_MIN_MEDIUM_ABOVE_DETECTION_DB
//   (3) On a Sens change, the WHOLE ladder is translated by the gate delta
//       (prev_gate_dbm_ -> settings_.alert_rssi_threshold_dbm)
//
// Invariant (2) is the LOW-reachability guarantee: a signal that just passes
// the detection gate (threshold + 2 dB hysteresis) must still have room to
// classify as LOW. If detection is raised up to/beyond medium, every detected
// signal would classify MEDIUM+ and LOW would never appear on screen.
//
// Invariant (3) makes all four threat shelves ("полки") move COHERENTLY when
// the user changes Sensitivity: every shelf shifts by the same delta, so the
// user-tuned gaps between shelves are preserved. (The previous design dragged
// only LOW along the gate via a stored offset — with factory defaults that
// offset was zero, so visibly only Lo moved, and it tracked the raw gate.)
//
// @param edited_field 0=low, 1=medium, 2=high, 3=critical, 4=detection (Sens).
//                     Used to prioritize the field the user just touched, while
//                     neighboring thresholds are auto-adjusted to keep order.
//                     On Sens edits (4), translation is anchored to the gate
//                     delta recorded in prev_gate_dbm_; prev_gate_dbm_ is
//                     refreshed at the end of the call.
void DroneSettingsView::normalize_threat_ladder(uint8_t edited_field) noexcept {
    // Priority order: the edited field's value wins; neighbors are pushed up
    // or down only to satisfy ordering relative to it.
    int32_t low = settings_.threat_low_dbm;
    int32_t medium = settings_.threat_medium_dbm;
    int32_t high = settings_.threat_high_dbm;
    int32_t critical = settings_.threat_critical_dbm;

    // When the detection gate (Sens) changes, translate the ENTIRE ladder by
    // the gate delta so all shelves move together. clip() is monotone, so a
    // uniform shift + clamp can never invert the low<=medium<=high<=critical
    // ordering. Stack: ~48 bytes total for this function.
    if (edited_field == 4) {
        const int32_t gate_delta =
            settings_.alert_rssi_threshold_dbm - prev_gate_dbm_;
        if (gate_delta != 0) {
            low = clip(low + gate_delta, RSSI_MIN_DBM, RSSI_MAX_DBM);
            medium = clip(medium + gate_delta, RSSI_MIN_DBM, RSSI_MAX_DBM);
            high = clip(high + gate_delta, RSSI_MIN_DBM, RSSI_MAX_DBM);
            critical = clip(critical + gate_delta, RSSI_MIN_DBM, RSSI_MAX_DBM);
        }
    }

    // Median-clamp: restore ordering, preserving the edited value.
    switch (edited_field) {
        case 4:  // Detection (Sens) edited: the whole ladder was translated
                 // above; uniform shift + monotone clamp preserves order, but
                 // cap LOW at medium as a belt-and-braces guard (the
                 // LOW-reachability invariant below still guards the gap).
            if (low > medium) low = medium;
            break;
        case 0:  // LOW edited
            if (low > medium) medium = low;
            if (high < medium) high = medium;
            if (critical < high) critical = high;
            break;
        case 1:  // MEDIUM edited
            if (low > medium) low = medium;
            if (high < medium) high = medium;
            if (critical < high) critical = high;
            break;
        case 2:  // HIGH edited
            if (medium > high) medium = high;
            if (low > medium) low = medium;
            if (critical < high) critical = high;
            break;
        case 3:  // CRITICAL edited
            if (high > critical) high = critical;
            if (medium > high) medium = high;
            if (low > medium) low = medium;
            break;
        default:
            break;
    }

    // LOW-reachability invariant: keep medium strictly above the detection gate,
    // leaving at least a small LOW band. Only raises medium upward; lowering it
    // below the gate would otherwise silence LOW entirely.
    const int32_t min_medium = settings_.alert_rssi_threshold_dbm
        + RSSI_MIN_MEDIUM_ABOVE_DETECTION_DB;
    if (medium < min_medium) {
        medium = min_medium;
        if (high < medium) high = medium;
        if (critical < high) critical = high;
    }

    // Final clamp to valid RSSI range (NumberField clips anyway on set_value).
    low = clip(low, RSSI_MIN_DBM, RSSI_MAX_DBM);
    medium = clip(medium, RSSI_MIN_DBM, RSSI_MAX_DBM);
    high = clip(high, RSSI_MIN_DBM, RSSI_MAX_DBM);
    critical = clip(critical, RSSI_MIN_DBM, RSSI_MAX_DBM);

    // Track the gate so the next Sens edit translates the ladder by the true
    // delta (encoder steps fire one on_change per click).
    if (edited_field == 4) {
        prev_gate_dbm_ = settings_.alert_rssi_threshold_dbm;
    }

    settings_.threat_low_dbm = low;
    settings_.threat_medium_dbm = medium;
    settings_.threat_high_dbm = high;
    settings_.threat_critical_dbm = critical;

    // Reflect any auto-adjustment back onto the widgets without re-entrancy.
    field_threat_low_.set_value(low, false);
    field_threat_medium_.set_value(medium, false);
    field_threat_high_.set_value(high, false);
    field_threat_critical_.set_value(critical, false);
}

// ============================================================================
// SD Card Save (via centralized SettingsFileManager)
// ============================================================================

void DroneSettingsView::save_settings_to_sd() noexcept {
    (void)SettingsFileManager::save(scanner_ptr_, settings_);
}

} // namespace drone_analyzer