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

namespace drone_analyzer {

// ============================================================================
// DroneSettingsView Constructor / Destructor
// ============================================================================

DroneSettingsView::DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display) noexcept
    : ui::View()
    , labels_({
        {{UI_POS_X(1), UI_POS_Y(1)}, "Int(ms):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(3)}, "Sens:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(13), UI_POS_Y(2)}, "Vol:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(17), UI_POS_Y(5)}, "Mar:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(17), UI_POS_Y(6)}, "Wid:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(0), UI_POS_Y(5)}, "MaxW:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(0), UI_POS_Y(6)}, "Shrp:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(10), UI_POS_Y(5)}, "Rat:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(10), UI_POS_Y(6)}, "Vly:", Theme::getInstance()->fg_light->foreground},
    })
    , field_scan_mode_({UI_POS_X(0), UI_POS_Y(0)}, 1, {
        {"-", 0},
    })
    , field_scan_interval_({UI_POS_X(1), UI_POS_Y(2)}, 4, {10, 1000}, 10, ' ')
    , field_rssi_threshold_({UI_POS_X(1), UI_POS_Y(4)}, 3, {0, 100}, 1, ' ')
    , field_volume_({UI_POS_X(17), UI_POS_Y(2)}, 2, {0, 99}, 1, ' ')
    , check_audio_alerts_({UI_POS_X(1), UI_POS_Y(9)}, 6, "Audio", false)
    , check_spectrum_visible_({UI_POS_X(20), UI_POS_Y(9)}, 5, "Spec", false)
    , check_histogram_visible_({UI_POS_X(20), UI_POS_Y(11)}, 5, "Hist", false)
    , check_dwell_enabled_({UI_POS_X(1), UI_POS_Y(11)}, 6, "Dwell", false)
    , check_confirm_count_({UI_POS_X(1), UI_POS_Y(13)}, 8, "Confirm", false)
    , check_noise_blacklist_({UI_POS_X(1), UI_POS_Y(15)}, 8, "Blklist", false)
    , check_rssi_variance_({UI_POS_X(20), UI_POS_Y(7)}, 5, "RVar", false)
    , check_neighbor_margin_({UI_POS_X(1), UI_POS_Y(17)}, 4, "NB", false)
    , field_neighbor_margin_({UI_POS_X(6), UI_POS_Y(17)}, 2, {0, 15}, 1, ' ')
    , check_spectrum_detection_({UI_POS_X(10), UI_POS_Y(13)}, 4, "Mar", false)
    , field_spectrum_margin_({UI_POS_X(20), UI_POS_Y(5)}, 3, {5, 200}, 5, ' ')
    , field_spectrum_min_width_({UI_POS_X(20), UI_POS_Y(6)}, 2, {1, 20}, 1, ' ')
    , field_spectrum_max_width_({UI_POS_X(6), UI_POS_Y(5)}, 3, {1, 100}, 1, ' ')
    , field_spectrum_peak_sharpness_({UI_POS_X(6), UI_POS_Y(6)}, 3, {50, 250}, 5, ' ')
    , field_spectrum_peak_ratio_({UI_POS_X(13), UI_POS_Y(5)}, 3, {0, 255}, 5, ' ')
    , field_spectrum_valley_depth_({UI_POS_X(13), UI_POS_Y(6)}, 3, {0, 200}, 5, ' ')
    , button_defaults_({UI_POS_X(0), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(13), 20}, "DEFAULT")
    , button_about_({UI_POS_X(13), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(2), 20}, "!")
    , button_save_({UI_POS_X(15), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(14), 20}, "SAVE")
    , button_info_margin_({UI_POS_X(0), UI_POS_Y(7), UI_POS_WIDTH(4), 16}, "Mrg?")
    , button_info_width_({UI_POS_X(5), UI_POS_Y(7), UI_POS_WIDTH(4), 16}, "Wid?")
    , button_info_sharp_({UI_POS_X(10), UI_POS_Y(7), UI_POS_WIDTH(4), 16}, "Shp?")
    , button_info_ratio_({UI_POS_X(15), UI_POS_Y(7), UI_POS_WIDTH(4), 16}, "Rat?")
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , display_ptr_(display)
    , original_config_(config)
    , settings_()
    , settings_dirty_(false) {

    // Extract current scanner config into settings struct
    SettingsFileManager::extract_from_config(config, settings_);

    add_children({
        &labels_,
        &field_scan_interval_,
        &field_rssi_threshold_,
        &field_volume_,
        &check_audio_alerts_,
        &check_spectrum_visible_,
        &check_histogram_visible_,
        &check_dwell_enabled_,
        &check_confirm_count_,
        &check_noise_blacklist_,
        &check_spectrum_detection_,
        &check_neighbor_margin_,
        &field_neighbor_margin_,
        &check_rssi_variance_,
        &field_spectrum_margin_,
        &field_spectrum_min_width_,
        &field_spectrum_max_width_,
        &field_spectrum_peak_sharpness_,
        &field_spectrum_peak_ratio_,
        &field_spectrum_valley_depth_,
        &button_defaults_,
        &button_about_,
        &button_save_,
        &button_info_margin_,
        &button_info_width_,
        &button_info_sharp_,
        &button_info_ratio_
    });

    // Load persisted settings from SD card (overrides config-based defaults)
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
        settings_dirty_ = true;
    };

    field_volume_.on_change = [this](int32_t v) {
        portapack::receiver_model.set_normalized_headphone_volume(static_cast<uint8_t>(v));
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

    check_histogram_visible_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.histogram_visible = v;
        if (display_ptr_ != nullptr) {
            display_ptr_->set_histogram_visible(v);
        }
        settings_dirty_ = true;
    };

    check_dwell_enabled_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.dwell_enabled = v;
        settings_dirty_ = true;
    };

    check_confirm_count_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.confirm_count_enabled = v;
        settings_dirty_ = true;
    };

    check_noise_blacklist_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.noise_blacklist_enabled = v;
        settings_dirty_ = true;
    };

    check_spectrum_detection_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.spectrum_detection_enabled = v;
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

    field_spectrum_margin_.on_change = [this](int32_t v) {
        settings_.spectrum_margin = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_spectrum_min_width_.on_change = [this](int32_t v) {
        settings_.spectrum_min_width = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_spectrum_max_width_.on_change = [this](int32_t v) {
        settings_.spectrum_max_width = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_spectrum_peak_sharpness_.on_change = [this](int32_t v) {
        settings_.spectrum_peak_sharpness = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_spectrum_peak_ratio_.on_change = [this](int32_t v) {
        settings_.spectrum_peak_ratio = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_spectrum_valley_depth_.on_change = [this](int32_t v) {
        settings_.spectrum_valley_depth = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    // SAVE button: apply to scanner + save to SD card
    button_save_.on_select = [this](ui::Button&) {
        // Apply settings to scanner config
        if (scanner_ptr_ != nullptr) {
            ScanConfig updated_config = original_config_;
            SettingsFileManager::apply_to_config(settings_, updated_config);
            // Preserve sweep settings from scanner (SWP view manages them)
            ScanConfig current_cfg = scanner_ptr_->get_config();
            updated_config.sweep_start_freq = current_cfg.sweep_start_freq;
            updated_config.sweep_end_freq = current_cfg.sweep_end_freq;
            updated_config.sweep_step_freq = current_cfg.sweep_step_freq;
            updated_config.sweep2_start_freq = current_cfg.sweep2_start_freq;
            updated_config.sweep2_end_freq = current_cfg.sweep2_end_freq;
            updated_config.sweep2_step_freq = current_cfg.sweep2_step_freq;
            updated_config.sweep2_enabled = current_cfg.sweep2_enabled;
            updated_config.sweep3_start_freq = current_cfg.sweep3_start_freq;
            updated_config.sweep3_end_freq = current_cfg.sweep3_end_freq;
            updated_config.sweep3_step_freq = current_cfg.sweep3_step_freq;
            updated_config.sweep3_enabled = current_cfg.sweep3_enabled;
            updated_config.sweep4_start_freq = current_cfg.sweep4_start_freq;
            updated_config.sweep4_end_freq = current_cfg.sweep4_end_freq;
            updated_config.sweep4_step_freq = current_cfg.sweep4_step_freq;
            updated_config.sweep4_enabled = current_cfg.sweep4_enabled;

            const ErrorCode err = scanner_ptr_->set_config(updated_config);
            if (err != ErrorCode::SUCCESS) {
                return;
            }
        }

        save_settings_to_sd();
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        settings_ = SettingsStruct();
        portapack::receiver_model.set_normalized_headphone_volume(70);
        settings_dirty_ = true;
        apply_settings_to_ui();
    };

    button_about_.on_select = [this](ui::Button&) {
        nav_.display_modal("About",
            "Author: Kuznetsov Maxim\n"
            "Orenburg\n"
            "Card: 2202 20202 5787 1695\n"
            "YooMoney: 41001810704697\n"
            "TON: UQCdtMxQB5zbQBOICkY90l\n"
            "TQQqcs8V-V28Bf2AGvl8xOc5HR\n"
            "Telegram: @max_ai_master\n"
            "TM PowerHamster2188");
    };

    // Info buttons for spectrum filter settings
    button_info_margin_.on_select = [this](ui::Button&) {
        nav_.display_modal("Margin",
            "Porog shuma signala.\n"
            "Skolko dB piki dolzhny\n"
            "byt vyshe fona.\n"
            "Bolshe = menshe lozhnyh.\n"
            "15-25 dlya FPV.");
    };

    button_info_width_.on_select = [this](ui::Button&) {
        nav_.display_modal("Max Width",
            "Maks. shirina signala.\n"
            "Otbrosit shirokie ploskie\n"
            "signaly (pomehi).\n"
            "Drony = uzkij pik.\n"
            "30-50 optimalno.");
    };

    button_info_sharp_.on_select = [this](ui::Button&) {
        nav_.display_modal("Sharpness",
            "Ostota pika signala.\n"
            "Video link drona = V-forma.\n"
            "Bolshe = strogij filtr.\n"
            "80-150 dlya FPV video.\n"
            "50 = ljuboj signal.");
    };

    button_info_ratio_.on_select = [this](ui::Button&) {
        nav_.display_modal("Peak Ratio",
            "Otnoshenie vysoty k shirine.\n"
            "Visokij + uzkoj = dron.\n"
            "Nizkij = pomeha.\n"
            "15-30 dlya FPV.\n"
            "0 = otklychen.");
    };
}

DroneSettingsView::~DroneSettingsView() noexcept {
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
    field_scan_interval_.set_value(settings_.scan_interval_ms);
    {
        const int32_t sens = -(settings_.alert_rssi_threshold_dbm + 20);
        field_rssi_threshold_.set_value(sens < 0 ? 0 : (sens > 100 ? 100 : sens));
    }
    field_volume_.set_value(portapack::receiver_model.normalized_headphone_volume());
    check_audio_alerts_.set_value(settings_.audio_alerts_enabled);
    check_spectrum_visible_.set_value(settings_.spectrum_visible);
    check_histogram_visible_.set_value(settings_.histogram_visible);
    check_dwell_enabled_.set_value(settings_.dwell_enabled);
    check_confirm_count_.set_value(settings_.confirm_count_enabled);
    check_noise_blacklist_.set_value(settings_.noise_blacklist_enabled);
    check_spectrum_detection_.set_value(settings_.spectrum_detection_enabled);
    field_spectrum_margin_.set_value(static_cast<int32_t>(settings_.spectrum_margin));
    field_spectrum_min_width_.set_value(static_cast<int32_t>(settings_.spectrum_min_width));
    field_spectrum_max_width_.set_value(static_cast<int32_t>(settings_.spectrum_max_width));
    field_spectrum_peak_sharpness_.set_value(static_cast<int32_t>(settings_.spectrum_peak_sharpness));
    field_spectrum_peak_ratio_.set_value(static_cast<int32_t>(settings_.spectrum_peak_ratio));
    field_spectrum_valley_depth_.set_value(static_cast<int32_t>(settings_.spectrum_valley_depth));
    check_neighbor_margin_.set_value(settings_.neighbor_margin_db > 0);
    field_neighbor_margin_.set_value(static_cast<int32_t>(settings_.neighbor_margin_db));
    check_rssi_variance_.set_value(settings_.rssi_variance_enabled);
}

// ============================================================================
// SD Card Save (via centralized SettingsFileManager)
// ============================================================================

void DroneSettingsView::save_settings_to_sd() noexcept {
    (void)SettingsFileManager::save(scanner_ptr_, settings_);
}

} // namespace drone_analyzer
