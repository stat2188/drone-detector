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

// Heap savings: original_config_ (ScanConfig ~400B) + settings_ (SettingsStruct ~360B)
// moved from instance members to file-statics. Only one DroneSettingsView exists
// at a time (PortaPack navigation model), so static storage is safe.
static ScanConfig s_original_config;
static SettingsStruct s_settings;

DroneSettingsView::DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , display_ptr_(display)
    , settings_dirty_(false) {

    // Store config in static (not member) — saves ~400B heap per view instance
    s_original_config = config;

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

    // --- Compact callback wiring ---
    // NOTE: Settings load + apply_settings_to_ui() deferred to on_show()
    // to reduce constructor stack depth during nav_.push<>() heap allocation.

    field_scan_interval_.on_change = [this](int32_t v) { s_settings.scan_interval_ms = static_cast<uint32_t>(v); settings_dirty_ = true; };
    field_rssi_threshold_.on_change = [this](int32_t v) {
        s_settings.alert_rssi_threshold_dbm = -20 - v;
        s_settings.scan_sensitivity = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };
    field_volume_.on_change = [this](int32_t v) {
        s_settings.volume = static_cast<uint8_t>(v);
        portapack::receiver_model.set_normalized_headphone_volume(static_cast<uint8_t>(v));
        settings_dirty_ = true;
    };
    field_rssi_dec_cyc_.on_change = [this](int32_t v) { s_settings.rssi_decrease_cycles = static_cast<uint8_t>(v); settings_dirty_ = true; };

    check_audio_alerts_.on_select = [this](ui::Checkbox&, bool v) { s_settings.audio_alerts_enabled = v; AudioAlertManager::set_enabled(v); settings_dirty_ = true; };
    check_spectrum_visible_.on_select = [this](ui::Checkbox&, bool v) { s_settings.spectrum_visible = v; if (display_ptr_) display_ptr_->set_spectrum_visible(v); settings_dirty_ = true; };
    check_histogram_visible_.on_select = [this](ui::Checkbox&, bool v) { s_settings.histogram_visible = v; if (display_ptr_) display_ptr_->set_histogram_visible(v); settings_dirty_ = true; };
    check_dwell_enabled_.on_select = [this](ui::Checkbox&, bool v) { s_settings.dwell_enabled = v; settings_dirty_ = true; };
    check_confirm_count_.on_select = [this](ui::Checkbox&, bool v) { s_settings.confirm_count_enabled = v; field_confirm_count_.visible(v); settings_dirty_ = true; };
    field_confirm_count_.on_change = [this](int32_t v) { s_settings.confirm_count = static_cast<uint8_t>(v); settings_dirty_ = true; };
    check_noise_blacklist_.on_select = [this](ui::Checkbox&, bool v) { s_settings.noise_blacklist_enabled = v; settings_dirty_ = true; };

    check_spectrum_detection_.on_select = [this](ui::Checkbox&, bool v) {
        s_settings.spectrum_detection_enabled = v;
        set_shape_filter_visibility(v);
        settings_dirty_ = true;
    };
    check_neighbor_margin_.on_select = [this](ui::Checkbox&, bool v) { s_settings.neighbor_margin_db = v ? DEFAULT_NEIGHBOR_MARGIN_DB : 0; settings_dirty_ = true; };
    field_neighbor_margin_.on_change = [this](int32_t v) { s_settings.neighbor_margin_db = v; settings_dirty_ = true; };
    check_rssi_variance_.on_select = [this](ui::Checkbox&, bool v) { s_settings.rssi_variance_enabled = v; settings_dirty_ = true; };
    check_pattern_matching_.on_select = [this](ui::Checkbox&, bool v) { s_settings.pattern_matching_enabled = v; settings_dirty_ = true; };

    // Shape filter fields
    field_spectrum_margin_.on_change = [this](int32_t v) { s_settings.spectrum_margin = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_min_width_.on_change = [this](int32_t v) {
        s_settings.spectrum_min_width = static_cast<uint8_t>(v);
        if (v > s_settings.spectrum_max_width) { s_settings.spectrum_max_width = v; field_spectrum_max_width_.set_value(v); }
        settings_dirty_ = true;
    };
    field_spectrum_max_width_.on_change = [this](int32_t v) {
        s_settings.spectrum_max_width = static_cast<uint8_t>(v);
        if (v < s_settings.spectrum_min_width) { s_settings.spectrum_min_width = v; field_spectrum_min_width_.set_value(v); }
        settings_dirty_ = true;
    };
    field_spectrum_peak_sharpness_.on_change = [this](int32_t v) { s_settings.spectrum_peak_sharpness = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_peak_ratio_.on_change = [this](int32_t v) { s_settings.spectrum_peak_ratio = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_valley_depth_.on_change = [this](int32_t v) { s_settings.spectrum_valley_depth = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_flatness_.on_change = [this](int32_t v) { s_settings.spectrum_flatness = static_cast<uint8_t>(v); settings_dirty_ = true; };
    field_spectrum_symmetry_.on_change = [this](int32_t v) { s_settings.spectrum_symmetry = static_cast<uint8_t>(v); settings_dirty_ = true; };

    // Mahalanobis
    check_mahalanobis_.on_select = [this](ui::Checkbox&, bool v) { s_settings.mahalanobis_enabled = v; settings_dirty_ = true; };
    field_mahalanobis_threshold_.on_change = [this](int32_t v) { s_settings.mahalanobis_threshold_x10 = static_cast<uint8_t>(v); settings_dirty_ = true; };

    // CFAR
    field_cfar_mode_.on_change = [this](size_t, int32_t v) { s_settings.cfar_mode = static_cast<CFARMode>(v); settings_dirty_ = true; };
    field_cfar_threshold_.on_change = [this](int32_t v) { s_settings.cfar_threshold_x10 = static_cast<uint8_t>(v); settings_dirty_ = true; };

    // Threat thresholds — one-way enforcement: low <= medium <= high <= critical
    field_threat_low_.on_change = [this](int32_t v) {
        s_settings.threat_low_dbm = v;
        if (v > s_settings.threat_medium_dbm) { s_settings.threat_medium_dbm = v; field_threat_medium_.set_value(v); }
        settings_dirty_ = true;
    };
    field_threat_medium_.on_change = [this](int32_t v) {
        s_settings.threat_medium_dbm = v;
        if (v < s_settings.threat_low_dbm) { s_settings.threat_low_dbm = v; field_threat_low_.set_value(v); }
        if (v > s_settings.threat_high_dbm) { s_settings.threat_high_dbm = v; field_threat_high_.set_value(v); }
        settings_dirty_ = true;
    };
    field_threat_high_.on_change = [this](int32_t v) {
        s_settings.threat_high_dbm = v;
        if (v < s_settings.threat_medium_dbm) { s_settings.threat_medium_dbm = v; field_threat_medium_.set_value(v); }
        if (v > s_settings.threat_critical_dbm) { s_settings.threat_critical_dbm = v; field_threat_critical_.set_value(v); }
        settings_dirty_ = true;
    };
    field_threat_critical_.on_change = [this](int32_t v) {
        s_settings.threat_critical_dbm = v;
        if (v < s_settings.threat_high_dbm) { s_settings.threat_high_dbm = v; field_threat_high_.set_value(v); }
        settings_dirty_ = true;
    };

    // SAVE
    button_save_.on_select = [this](ui::Button&) {
        if (scanner_ptr_ != nullptr) {
            static ScanConfig updated;
            updated = s_original_config;
            SettingsFileManager::apply_to_config(s_settings, updated);
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
        (void)SettingsFileManager::save(scanner_ptr_, s_settings);
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        s_settings = SettingsStruct();
        portapack::receiver_model.set_normalized_headphone_volume(s_settings.volume);
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
    field_scan_interval_.set_value(s_settings.scan_interval_ms);
    {
        const int32_t sens = -(s_settings.alert_rssi_threshold_dbm + 20);
        field_rssi_threshold_.set_value(sens < 0 ? 0 : (sens > 100 ? 100 : sens));
    }
    field_volume_.set_value(s_settings.volume);
    field_rssi_dec_cyc_.set_value(s_settings.rssi_decrease_cycles);
    check_audio_alerts_.set_value(s_settings.audio_alerts_enabled);
    check_spectrum_visible_.set_value(s_settings.spectrum_visible);
    check_histogram_visible_.set_value(s_settings.histogram_visible);
    check_dwell_enabled_.set_value(s_settings.dwell_enabled);
    check_confirm_count_.set_value(s_settings.confirm_count_enabled);
    field_confirm_count_.set_value(s_settings.confirm_count);
    field_confirm_count_.visible(s_settings.confirm_count_enabled);
    check_noise_blacklist_.set_value(s_settings.noise_blacklist_enabled);
    check_spectrum_detection_.set_value(s_settings.spectrum_detection_enabled);
    field_spectrum_margin_.set_value(s_settings.spectrum_margin);
    field_spectrum_min_width_.set_value(s_settings.spectrum_min_width);
    field_spectrum_max_width_.set_value(s_settings.spectrum_max_width);
    field_spectrum_peak_sharpness_.set_value(s_settings.spectrum_peak_sharpness);
    field_spectrum_peak_ratio_.set_value(s_settings.spectrum_peak_ratio);
    field_spectrum_valley_depth_.set_value(s_settings.spectrum_valley_depth);
    field_spectrum_flatness_.set_value(s_settings.spectrum_flatness);
    field_spectrum_symmetry_.set_value(s_settings.spectrum_symmetry);
    check_neighbor_margin_.set_value(s_settings.neighbor_margin_db > 0);
    field_neighbor_margin_.set_value(s_settings.neighbor_margin_db);
    check_rssi_variance_.set_value(s_settings.rssi_variance_enabled);
    check_mahalanobis_.set_value(s_settings.mahalanobis_enabled);
    field_mahalanobis_threshold_.set_value(s_settings.mahalanobis_threshold_x10);
    check_pattern_matching_.set_value(s_settings.pattern_matching_enabled);
    field_cfar_mode_.set_by_value(static_cast<int32_t>(s_settings.cfar_mode));
    field_cfar_threshold_.set_value(s_settings.cfar_threshold_x10);
    field_threat_low_.set_value(s_settings.threat_low_dbm);
    field_threat_medium_.set_value(s_settings.threat_medium_dbm);
    field_threat_high_.set_value(s_settings.threat_high_dbm);
    field_threat_critical_.set_value(s_settings.threat_critical_dbm);
    set_shape_filter_visibility(s_settings.spectrum_detection_enabled);
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

void DroneSettingsView::on_show() {
    // Deferred load: removed from constructor to reduce stack depth
    // during nav_.push<>() heap allocation (both views coexist briefly).
    (void)SettingsFileManager::load(s_settings);
    if (scanner_ptr_ != nullptr) {
        s_settings.median_enabled = scanner_ptr_->get_median_enabled();
    }
    apply_settings_to_ui();
    settings_dirty_ = false;
}

} // namespace drone_analyzer
