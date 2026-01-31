/*
 * EDA Tabbed Settings View Implementation
 */

#include "eda_tabbed_settings_view.hpp"
#include "eda_unified_settings.hpp"
#include "ui_navigation.hpp"
#include "portapack.hpp"
#include "file.hpp"
#include <cstring>

using namespace ui;
using namespace portapack;

namespace ui::apps::enhanced_drone_analyzer {

AudioSettingsTab::AudioSettingsTab(NavigationView& nav)
    : nav_(nav) {
    add_children({&text_audio_alerts_, &checkbox_enable_, &text_enable_,
                  &text_frequency_, &field_frequency_,
                  &text_duration_, &field_duration_,
                  &text_volume_, &field_volume_,
                  &checkbox_repeat_, &text_repeat_,
                  &button_save_, &button_cancel_});

    button_save_.on_select = [this](Button&) {
        auto settings = save_to_settings();
        if (settings.audio_enabled) {
            nav_.display_modal("Success", "Audio settings saved");
        } else {
            nav_.display_modal("Info", "Audio alerts disabled");
        }
        nav_.pop();
    };

    button_cancel_.on_select = [this](Button&) {
        nav_.pop();
    };
}

void AudioSettingsTab::focus() {
    checkbox_enable_.focus();
}

void AudioSettingsTab::load_from_settings(const AudioSettings& settings) {
    checkbox_enable_.set_value(settings.audio_enabled);
    field_frequency_.set_value(settings.alert_frequency_hz);
    field_duration_.set_value(settings.alert_duration_ms);
    field_volume_.set_value(settings.volume_level);
    checkbox_repeat_.set_value(settings.repeat_alerts);
}

AudioSettings AudioSettingsTab::save_to_settings() {
    AudioSettings settings;
    settings.audio_enabled = checkbox_enable_.value();
    settings.alert_frequency_hz = field_frequency_.value();
    settings.alert_duration_ms = field_duration_.value();
    settings.volume_level = field_volume_.value();
    settings.repeat_alerts = checkbox_repeat_.value();
    return settings;
}

HardwareSettingsTab::HardwareSettingsTab(NavigationView& nav)
    : nav_(nav) {
    add_children({&text_hardware_, &checkbox_real_hw_, &text_real_hw_,
                  &text_spectrum_mode_, &field_spectrum_mode_,
                  &text_bandwidth_, &field_bandwidth_,
                  &text_min_freq_, &field_min_freq_,
                  &text_max_freq_, &field_max_freq_,
                  &button_save_, &button_cancel_});

    button_save_.on_select = [this](Button&) {
        auto settings = save_to_settings();
        if (settings.enable_real_hardware) {
            nav_.display_modal("Success", "Hardware settings saved");
        } else {
            nav_.display_modal("Info", "Demo mode enabled");
        }
        nav_.pop();
    };

    button_cancel_.on_select = [this](Button&) {
        nav_.pop();
    };
}

void HardwareSettingsTab::focus() {
    checkbox_real_hw_.focus();
}

void HardwareSettingsTab::load_from_settings(const HardwareSettings& settings) {
    checkbox_real_hw_.set_value(settings.enable_real_hardware);
    field_spectrum_mode_.set_selected_index(static_cast<size_t>(settings.spectrum_mode));
    field_bandwidth_.set_value(settings.hardware_bandwidth_hz);
    field_min_freq_.set_value(settings.user_min_freq_hz);
    field_max_freq_.set_value(settings.user_max_freq_hz);
}

HardwareSettings HardwareSettingsTab::save_to_settings() {
    HardwareSettings settings;
    settings.enable_real_hardware = checkbox_real_hw_.value();
    size_t mode_idx = field_spectrum_mode_.selected_index();
    if (mode_idx == 0) settings.spectrum_mode = SpectrumMode::NARROW;
    else if (mode_idx == 1) settings.spectrum_mode = SpectrumMode::MEDIUM;
    else if (mode_idx == 2) settings.spectrum_mode = SpectrumMode::WIDE;
    else if (mode_idx == 3) settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
    settings.hardware_bandwidth_hz = field_bandwidth_.value();
    settings.user_min_freq_hz = field_min_freq_.value();
    settings.user_max_freq_hz = field_max_freq_.value();
    return settings;
}

ScanningSettingsTab::ScanningSettingsTab(NavigationView& nav)
    : nav_(nav) {
    add_children({&text_scanning_, &text_scan_interval_, &field_scan_interval_,
                  &text_rssi_threshold_, &field_rssi_threshold_,
                  &checkbox_wideband_, &text_wideband_,
                  &text_wb_min_, &field_wb_min_,
                  &text_wb_max_, &field_wb_max_,
                  &checkbox_panoramic_, &text_panoramic_,
                  &button_save_, &button_cancel_});

    button_save_.on_select = [this](Button&) {
        auto settings = save_to_settings();
        if (settings.enable_wideband_scanning) {
            nav_.display_modal("Success", "Wideband scanning enabled");
        } else {
            nav_.display_modal("Success", "Scanning settings saved");
        }
        nav_.pop();
    };

    button_cancel_.on_select = [this](Button&) {
        nav_.pop();
    };
}

void ScanningSettingsTab::focus() {
    field_scan_interval_.focus();
}

void ScanningSettingsTab::load_from_settings(const ScanningSettings& settings) {
    field_scan_interval_.set_value(settings.scan_interval_ms);
    field_rssi_threshold_.set_value(settings.rssi_threshold_db);
    checkbox_wideband_.set_value(settings.enable_wideband_scanning);
    field_wb_min_.set_value(settings.wideband_min_freq_hz);
    field_wb_max_.set_value(settings.wideband_max_freq_hz);
    checkbox_panoramic_.set_value(settings.panoramic_mode_enabled);
}

ScanningSettings ScanningSettingsTab::save_to_settings() {
    ScanningSettings settings;
    settings.scan_interval_ms = field_scan_interval_.value();
    settings.rssi_threshold_db = field_rssi_threshold_.value();
    settings.enable_wideband_scanning = checkbox_wideband_.value();
    settings.wideband_min_freq_hz = field_wb_min_.value();
    settings.wideband_max_freq_hz = field_wb_max_.value();
    settings.panoramic_mode_enabled = checkbox_panoramic_.value();
    return settings;
}

DetectionSettingsTab::DetectionSettingsTab(NavigationView& nav)
    : nav_(nav) {
    add_children({&text_detection_, &checkbox_fhss_, &text_fhss_,
                  &text_sensitivity_, &field_sensitivity_,
                  &text_threat_, &field_threat_,
                  &checkbox_intelligent_, &text_intelligent_,
                  &button_save_, &button_cancel_});

    button_save_.on_select = [this](Button&) {
        auto settings = save_to_settings();
        if (settings.enable_fhss_detection) {
            nav_.display_modal("Success", "FHSS detection enabled");
        }
        nav_.pop();
    };

    button_cancel_.on_select = [this](Button&) {
        nav_.pop();
    };
}

void DetectionSettingsTab::focus() {
    checkbox_fhss_.focus();
}

void DetectionSettingsTab::load_from_settings(const DetectionSettings& settings) {
    checkbox_fhss_.set_value(settings.enable_fhss_detection);
    field_sensitivity_.set_selected_index(settings.movement_sensitivity);
    field_threat_.set_selected_index(settings.threat_level_threshold);
    checkbox_intelligent_.set_value(settings.enable_intelligent_tracking);
}

DetectionSettings DetectionSettingsTab::save_to_settings() {
    DetectionSettings settings;
    settings.enable_fhss_detection = checkbox_fhss_.value();
    settings.movement_sensitivity = static_cast<uint8_t>(field_sensitivity_.selected_index());
    settings.threat_level_threshold = field_threat_.selected_index();
    settings.enable_intelligent_tracking = checkbox_intelligent_.value();
    return settings;
}

LoggingSettingsTab::LoggingSettingsTab(NavigationView& nav)
    : nav_(nav), path_buffer_("/eda_logs") {
    add_children({&text_logging_, &checkbox_autosave_, &text_autosave_,
                  &text_format_, &field_format_,
                  &text_max_size_, &field_max_size_,
                  &text_path_, &field_path_,
                  &button_save_, &button_cancel_});

    button_save_.on_select = [this](Button&) {
        auto settings = save_to_settings();
        if (settings.auto_save_logs) {
            nav_.display_modal("Success", "Logging settings saved");
        }
        nav_.pop();
    };

    button_cancel_.on_select = [this](Button&) {
        nav_.pop();
    };
}

void LoggingSettingsTab::focus() {
    checkbox_autosave_.focus();
}

void LoggingSettingsTab::load_from_settings(const LoggingSettings& settings) {
    checkbox_autosave_.set_value(settings.auto_save_logs);
    field_format_.set_selected_index(settings.log_format == "CSV" ? 0 : settings.log_format == "JSON" ? 1 : 2);
    field_max_size_.set_value(settings.max_log_file_size_kb);
    path_buffer_ = settings.log_file_path;
}

LoggingSettings LoggingSettingsTab::save_to_settings() {
    LoggingSettings settings;
    settings.auto_save_logs = checkbox_autosave_.value();
    size_t fmt_idx = field_format_.selected_index();
    settings.log_format = (fmt_idx == 0) ? "CSV" : (fmt_idx == 1) ? "JSON" : "TXT";
    settings.max_log_file_size_kb = field_max_size_.value();
    settings.log_file_path = path_buffer_;
    return settings;
}

DisplaySettingsTab::DisplaySettingsTab(NavigationView& nav)
    : nav_(nav) {
    add_children({&text_display_, &text_color_scheme_, &field_color_scheme_,
                  &text_font_size_, &field_font_size_,
                  &text_spectrum_density_, &field_density_,
                  &text_waterfall_speed_, &field_waterfall_,
                  &checkbox_show_ruler_, &text_show_ruler_,
                  &text_ruler_style_, &field_ruler_style_,
                  &checkbox_auto_style_, &text_auto_style_,
                  &text_tick_count_, &field_tick_count_,
                  &checkbox_detailed_, &text_detailed_,
                  &button_save_, &button_cancel_});

    button_save_.on_select = [this](Button&) {
        auto settings = save_to_settings();
        nav_.display_modal("Success", "Display settings saved");
        nav_.pop();
    };

    button_cancel_.on_select = [this](Button&) {
        nav_.pop();
    };
}

void DisplaySettingsTab::focus() {
    field_color_scheme_.focus();
}

void DisplaySettingsTab::load_from_settings(const DisplaySettings& settings) {
    field_color_scheme_.set_selected_index(settings.color_scheme == "Dark" ? 0 : 1);
    field_font_size_.set_selected_index(settings.font_size);
    field_density_.set_selected_index(settings.spectrum_density);
    field_waterfall_.set_value(settings.waterfall_speed);
    checkbox_show_ruler_.set_value(settings.show_frequency_ruler);
    field_ruler_style_.set_selected_index(settings.frequency_ruler_style);
    checkbox_auto_style_.set_value(settings.auto_ruler_style);
    field_tick_count_.set_selected_index(settings.compact_ruler_tick_count - 3);
    checkbox_detailed_.set_value(settings.show_detailed_info);
}

DisplaySettings DisplaySettingsTab::save_to_settings() {
    DisplaySettings settings;
    settings.color_scheme = (field_color_scheme_.selected_index() == 0) ? "Dark" : "Light";
    settings.font_size = field_font_size_.selected_index();
    settings.spectrum_density = field_density_.selected_index();
    settings.waterfall_speed = field_waterfall_.value();
    settings.show_frequency_ruler = checkbox_show_ruler_.value();
    settings.frequency_ruler_style = field_ruler_style_.selected_index();
    settings.auto_ruler_style = checkbox_auto_style_.value();
    settings.compact_ruler_tick_count = field_tick_count_.selected_index() + 3;
    settings.show_detailed_info = checkbox_detailed_.value();
    return settings;
}

EDATabbedSettingsView::EDATabbedSettingsView(NavigationView& nav)
    : nav_(nav),
      tab_audio_(nav_),
      tab_hardware_(nav_),
      tab_scanning_(nav_),
      tab_detection_(nav_),
      tab_logging_(nav_),
      tab_display_(nav_),
      button_defaults_{Rect{UI_POS_X(2), UI_POS_Y(0), UI_WIDTH(12), 16}, "Defaults"},
      button_import_{Rect{UI_POS_X(14), UI_POS_Y(0), UI_WIDTH(12), 16}, "Import"},
      button_export_{Rect{UI_POS_X(26), UI_POS_Y(0), UI_WIDTH(12), 16}, "Export"} {

    add_children({&tab_audio_, &tab_hardware_, &tab_scanning_,
                  &tab_detection_, &tab_logging_, &tab_display_});

    button_defaults_.on_select = [this](Button&) { on_defaults(); };
    button_import_.on_select = [this](Button&) { on_import(); };
    button_export_.on_select = [this](Button&) { on_export(); };
}

void EDATabbedSettingsView::focus() {
    tab_audio_.focus();
}

void EDATabbedSettingsView::load_settings(const EDAUnifiedSettings& settings) {
    tab_audio_.load_from_settings(settings.audio);
    tab_hardware_.load_from_settings(settings.hardware);
    tab_scanning_.load_from_settings(settings.scanning);
    tab_detection_.load_from_settings(settings.detection);
    tab_logging_.load_from_settings(settings.logging);
    tab_display_.load_from_settings(settings.display);
}

EDAUnifiedSettings EDATabbedSettingsView::save_settings() {
    EDAUnifiedSettings settings;
    settings.audio = tab_audio_.save_to_settings();
    settings.hardware = tab_hardware_.save_to_settings();
    settings.scanning = tab_scanning_.save_to_settings();
    settings.detection = tab_detection_.save_to_settings();
    settings.logging = tab_logging_.save_to_settings();
    settings.display = tab_display_.save_to_settings();
    return settings;
}

void EDATabbedSettingsView::on_defaults() {
    EDAUnifiedSettings settings;
    settings.reset_to_defaults();
    load_settings(settings);
    nav_.display_modal("Reset", "Settings reset to defaults");
}

void EDATabbedSettingsView::on_import() {
    nav_.display_modal("Info", "Import feature coming soon");
}

void EDATabbedSettingsView::on_export() {
    nav_.display_modal("Info", "Export feature coming soon");
}

} // namespace ui::apps::enhanced_drone_analyzer
