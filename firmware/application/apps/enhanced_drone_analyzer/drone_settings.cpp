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

DroneSettingsView::DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , display_ptr_(display)
    , original_config_(config)
    , settings_()
    , settings_dirty_(false) {

    SettingsFileManager::extract_from_config(config, settings_);

    add_children({
        &labels_cfar_, &field_cfar_mode_, &field_cfar_threshold_,
        &labels_row1_, &field_scan_interval_, &labels_sens_, &field_rssi_threshold_,
        &labels_vol_, &field_volume_, &labels_cyc_, &field_rssi_dec_cyc_,
        &check_mahalanobis_, &field_mahalanobis_threshold_,
        &labels_shape_,
        &field_spectrum_max_width_, &field_spectrum_peak_sharpness_,
        &field_spectrum_peak_ratio_, &field_spectrum_valley_depth_,
        &field_spectrum_flatness_, &field_spectrum_symmetry_,
        &field_spectrum_margin_, &field_spectrum_min_width_,
        &check_audio_alerts_, &check_spectrum_visible_, &check_histogram_visible_,
        &check_dwell_enabled_, &check_confirm_count_, &field_confirm_count_,
        &check_spectrum_detection_, &check_neighbor_margin_, &field_neighbor_margin_,
        &check_noise_blacklist_, &check_rssi_variance_, &check_pattern_matching_,
        &labels_threat_,
        &field_threat_low_, &field_threat_medium_, &field_threat_high_, &field_threat_critical_,
        &button_defaults_, &button_about_, &button_save_,
    });

    (void)SettingsFileManager::load(settings_);
    if (scanner_ptr_ != nullptr) {
        settings_.median_enabled = scanner_ptr_->get_median_enabled();
    }
    apply_settings_to_ui();

    // --- Compact callback wiring ---

    field_scan_interval_.on_change = [this](int32_t v) { settings_.scan_interval_ms = static_cast<uint32_t>(v); settings_dirty_ = true; };
    field_rssi_threshold_.on_change = [this](int32_t v) {
        settings_.alert_rssi_threshold_dbm = -20 - v;
        settings_.scan_sensitivity = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };
    field_volume_.on_change = [this](int32_t v) {
        settings_.volume = static_cast<uint8_t>(v);
        portapack::receiver_model.set_normalized_headphone_volume(static_cast<uint8_t>(v));
        settings_dirty_ = true;
    };
    field_rssi_dec_cyc_.on_change = [this](int32_t v) { settings_.rssi_decrease_cycles = static_cast<uint8_t>(v); settings_dirty_ = true; };

    check_audio_alerts_.on_select = [this](ui::Checkbox&, bool v) { settings_.audio_alerts_enabled = v; AudioAlertManager::set_enabled(v); settings_dirty_ = true; };
    check_spectrum_visible_.on_select = [this](ui::Checkbox&, bool v) { settings_.spectrum_visible = v; if (display_ptr_) display_ptr_->set_spectrum_visible(v); settings_dirty_ = true; };
    check_histogram_visible_.on_select = [this](ui::Checkbox&, bool v) { settings_.histogram_visible = v; if (display_ptr_) display_ptr_->set_histogram_visible(v); settings_dirty_ = true; };
    check_dwell_enabled_.on_select = [this](ui::Checkbox&, bool v) { settings_.dwell_enabled = v; settings_dirty_ = true; };
    check_confirm_count_.on_select = [this](ui::Checkbox&, bool v) { settings_.confirm_count_enabled = v; field_confirm_count_.visible(v); settings_dirty_ = true; };
    field_confirm_count_.on_change = [this](int32_t v) { settings_.confirm_count = static_cast<uint8_t>(v); settings_dirty_ = true; };
    check_noise_blacklist_.on_select = [this](ui::Checkbox&, bool v) { settings_.noise_blacklist_enabled = v; settings_dirty_ = true; };

    check_spectrum_detection_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.spectrum_detection_enabled = v;
        set_shape_filter_visibility(v);
        settings_dirty_ = true;
    };
    check_neighbor_margin_.on_select = [this](ui::Checkbox&, bool v) { settings_.neighbor_margin_db = v ? DEFAULT_NEIGHBOR_MARGIN_DB : 0; settings_dirty_ = true; };
    field_neighbor_margin_.on_change = [this](int32_t v) { settings_.neighbor_margin_db = v; settings_dirty_ = true; };
    check_rssi_variance_.on_select = [this](ui::Checkbox&, bool v) { settings_.rssi_variance_enabled = v; settings_dirty_ = true; };
    check_pattern_matching_.on_select = [this](ui::Checkbox&, bool v) { settings_.pattern_matching_enabled = v; settings_dirty_ = true; };

    // Shape filter fields — compact one-liners
    field_spectrum_margin_.on_change = [this](int32_t v) { settings_.spectrum_margin = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_min_width_.on_change = [this](int32_t v) {
        settings_.spectrum_min_width = static_cast<uint8_t>(v);
        if (v > settings_.spectrum_max_width) { settings_.spectrum_max_width = v; field_spectrum_max_width_.set_value(v); }
        settings_dirty_ = true;
    };
    field_spectrum_max_width_.on_change = [this](int32_t v) {
        settings_.spectrum_max_width = static_cast<uint8_t>(v);
        if (v < settings_.spectrum_min_width) { settings_.spectrum_min_width = v; field_spectrum_min_width_.set_value(v); }
        settings_dirty_ = true;
    };
    field_spectrum_peak_sharpness_.on_change = [this](int32_t v) { settings_.spectrum_peak_sharpness = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_peak_ratio_.on_change = [this](int32_t v) { settings_.spectrum_peak_ratio = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_valley_depth_.on_change = [this](int32_t v) { settings_.spectrum_valley_depth = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_flatness_.on_change = [this](int32_t v) { settings_.spectrum_flatness = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_symmetry_.on_change = [this](int32_t v) { settings_.spectrum_symmetry = static_cast<uint8_t>(v); settings_dirty_ = true; };

    // Mahalanobis
    check_mahalanobis_.on_select = [this](ui::Checkbox&, bool v) { settings_.mahalanobis_enabled = v; settings_dirty_ = true; };
    field_mahalanobis_threshold_.on_change = [this](int32_t v) { settings_.mahalanobis_threshold_x10 = static_cast<uint8_t>(v); settings_dirty_ = true; };

    // CFAR
    field_cfar_mode_.on_change = [this](size_t, int32_t v) { settings_.cfar_mode = static_cast<CFARMode>(v); settings_dirty_ = true; };
    field_cfar_threshold_.on_change = [this](int32_t v) { settings_.cfar_threshold_x10 = static_cast<uint8_t>(v); settings_dirty_ = true; };

    // Threat thresholds — simple one-way enforcement: low <= medium <= high <= critical
    field_threat_low_.on_change = [this](int32_t v) {
        settings_.threat_low_dbm = v;
        if (v > settings_.threat_medium_dbm) { settings_.threat_medium_dbm = v; field_threat_medium_.set_value(v); }
        settings_dirty_ = true;
    };
    field_threat_medium_.on_change = [this](int32_t v) {
        settings_.threat_medium_dbm = v;
        if (v < settings_.threat_low_dbm) { settings_.threat_low_dbm = v; field_threat_low_.set_value(v); }
        if (v > settings_.threat_high_dbm) { settings_.threat_high_dbm = v; field_threat_high_.set_value(v); }
        settings_dirty_ = true;
    };
    field_threat_high_.on_change = [this](int32_t v) {
        settings_.threat_high_dbm = v;
        if (v < settings_.threat_medium_dbm) { settings_.threat_medium_dbm = v; field_threat_medium_.set_value(v); }
        if (v > settings_.threat_critical_dbm) { settings_.threat_critical_dbm = v; field_threat_critical_.set_value(v); }
        settings_dirty_ = true;
    };
    field_threat_critical_.on_change = [this](int32_t v) {
        settings_.threat_critical_dbm = v;
        if (v < settings_.threat_high_dbm) { settings_.threat_high_dbm = v; field_threat_high_.set_value(v); }
        settings_dirty_ = true;
    };

    // SAVE
    button_save_.on_select = [this](ui::Button&) {
        if (scanner_ptr_ != nullptr) {
            static ScanConfig updated;
            updated = original_config_;
            SettingsFileManager::apply_to_config(settings_, updated);
            static ScanConfig live;
            scanner_ptr_->get_config_to(live);
            updated.sweep_start_freq = live.sweep_start_freq;
            updated.sweep_end_freq = live.sweep_end_freq;
            updated.sweep_step_freq = live.sweep_step_freq;
            updated.sweep2_start_freq = live.sweep2_start_freq;
            updated.sweep2_end_freq = live.sweep2_end_freq;
            updated.sweep2_step_freq = live.sweep2_step_freq;
            updated.sweep2_enabled = live.sweep2_enabled;
            updated.sweep3_start_freq = live.sweep3_start_freq;
            updated.sweep3_end_freq = live.sweep3_end_freq;
            updated.sweep3_step_freq = live.sweep3_step_freq;
            updated.sweep3_enabled = live.sweep3_enabled;
            updated.sweep4_start_freq = live.sweep4_start_freq;
            updated.sweep4_end_freq = live.sweep4_end_freq;
            updated.sweep4_step_freq = live.sweep4_step_freq;
            updated.sweep4_enabled = live.sweep4_enabled;
            for (uint8_t w = 0; w < 4; ++w)
                for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i)
                    updated.sweep_exceptions[w][i] = live.sweep_exceptions[w][i];

            if (scanner_ptr_->set_config(updated) != ErrorCode::SUCCESS) {
                nav_.display_modal("Error", "Invalid settings.\nCheck min<=max\nand valid ranges.");
                return;
            }
        }
        (void)SettingsFileManager::save(scanner_ptr_, settings_);
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        settings_ = SettingsStruct();
        portapack::receiver_model.set_normalized_headphone_volume(settings_.volume);
        settings_dirty_ = true;
        apply_settings_to_ui();
    };

    button_about_.on_select = [this](ui::Button&) {
        nav_.display_modal("About",
            "Author: Kuznetsov Maxim\n"
            "Orenburg\n"
            "Telegram: @max_ai_master");
    };
}

DroneSettingsView::~DroneSettingsView() noexcept {}

void DroneSettingsView::paint(ui::Painter& painter) { (void)painter; }
void DroneSettingsView::focus() { field_scan_interval_.focus(); }

void DroneSettingsView::apply_settings_to_ui() noexcept {
    field_scan_interval_.set_value(settings_.scan_interval_ms);
    {
        const int32_t sens = -(settings_.alert_rssi_threshold_dbm + 20);
        field_rssi_threshold_.set_value(sens < 0 ? 0 : (sens > 100 ? 100 : sens));
    }
    field_volume_.set_value(settings_.volume);
    field_rssi_dec_cyc_.set_value(settings_.rssi_decrease_cycles);
    check_audio_alerts_.set_value(settings_.audio_alerts_enabled);
    check_spectrum_visible_.set_value(settings_.spectrum_visible);
    check_histogram_visible_.set_value(settings_.histogram_visible);
    check_dwell_enabled_.set_value(settings_.dwell_enabled);
    check_confirm_count_.set_value(settings_.confirm_count_enabled);
    field_confirm_count_.set_value(settings_.confirm_count);
    field_confirm_count_.visible(settings_.confirm_count_enabled);
    check_noise_blacklist_.set_value(settings_.noise_blacklist_enabled);
    check_spectrum_detection_.set_value(settings_.spectrum_detection_enabled);
    field_spectrum_margin_.set_value(settings_.spectrum_margin);
    field_spectrum_min_width_.set_value(settings_.spectrum_min_width);
    field_spectrum_max_width_.set_value(settings_.spectrum_max_width);
    field_spectrum_peak_sharpness_.set_value(settings_.spectrum_peak_sharpness);
    field_spectrum_peak_ratio_.set_value(settings_.spectrum_peak_ratio);
    field_spectrum_valley_depth_.set_value(settings_.spectrum_valley_depth);
    field_spectrum_flatness_.set_value(settings_.spectrum_flatness);
    field_spectrum_symmetry_.set_value(settings_.spectrum_symmetry);
    check_neighbor_margin_.set_value(settings_.neighbor_margin_db > 0);
    field_neighbor_margin_.set_value(settings_.neighbor_margin_db);
    check_rssi_variance_.set_value(settings_.rssi_variance_enabled);
    check_mahalanobis_.set_value(settings_.mahalanobis_enabled);
    field_mahalanobis_threshold_.set_value(settings_.mahalanobis_threshold_x10);
    check_pattern_matching_.set_value(settings_.pattern_matching_enabled);
    field_cfar_mode_.set_by_value(static_cast<int32_t>(settings_.cfar_mode));
    field_cfar_threshold_.set_value(settings_.cfar_threshold_x10);
    field_threat_low_.set_value(settings_.threat_low_dbm);
    field_threat_medium_.set_value(settings_.threat_medium_dbm);
    field_threat_high_.set_value(settings_.threat_high_dbm);
    field_threat_critical_.set_value(settings_.threat_critical_dbm);
    set_shape_filter_visibility(settings_.spectrum_detection_enabled);
}

void DroneSettingsView::set_shape_filter_visibility(bool vis) noexcept {
    field_spectrum_margin_.visible(vis);
    field_spectrum_min_width_.visible(vis);
    field_spectrum_max_width_.visible(vis);
    field_spectrum_peak_sharpness_.visible(vis);
    field_spectrum_peak_ratio_.visible(vis);
    field_spectrum_valley_depth_.visible(vis);
    field_spectrum_flatness_.visible(vis);
    field_spectrum_symmetry_.visible(vis);
    field_cfar_mode_.visible(vis);
    field_cfar_threshold_.visible(vis);
}

} // namespace drone_analyzer
