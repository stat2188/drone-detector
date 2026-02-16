/*
 * Enhanced Drone Analyzer - Advanced Settings UI Implementation
 * Following Mayhem firmware UI patterns
 *
 * DIAMOND OPTIMIZATION: Zero-overhead abstractions, RAII, constexpr where possible
 * FIX: Reserve capacity for std::string to prevent multiple heap allocations
 */

#include "eda_advanced_settings.hpp"
#include "settings_persistence.hpp"
#include "ui_enhanced_drone_settings.hpp"
#include "ui_drone_common_types.hpp"
#include "ui_enhanced_drone_analyzer.hpp"
#include "string_format.hpp"
#include "file.hpp"
#include <cstring>
#include <cstdio>
#include <inttypes.h>
#include <ch.h>

namespace ui::apps::enhanced_drone_analyzer {

// ==========================================
// Detection Settings Tab Implementation
// ==========================================

DetectionSettingsView::DetectionSettingsView(NavigationView& nav)
    : View({0, 0, 240, 304}), nav_(nav) {
    
    add_children({
        &text_title_,
        &checkbox_fhss_, &text_fhss_,
        &field_movement_sensitivity_, &text_movement_,
        &field_threat_level_, &text_threat_,
        &checkbox_intelligent_, &text_intelligent_,
        &button_save_
    });
    
    button_save_.on_select = [this](Button&) {
        save_settings();
        nav_.pop();
    };
}

void DetectionSettingsView::focus() {
    button_save_.focus();
}

void DetectionSettingsView::save_settings() {
    DroneAnalyzerSettings settings;
    SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    
    settings.detection_flags.enable_fhss_detection = checkbox_fhss_.value();
    settings.movement_sensitivity = static_cast<uint8_t>(field_movement_sensitivity_.selected_index_value());
    settings.threat_level_threshold = static_cast<uint32_t>(field_threat_level_.selected_index_value());
    settings.detection_flags.enable_intelligent_tracking = checkbox_intelligent_.value();
    
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}

// ==========================================
// Logging Settings Tab Implementation
// ==========================================

LoggingSettingsView::LoggingSettingsView(NavigationView& nav)
    : View({0, 0, 240, 304}), nav_(nav), log_path_buffer_{"/EDA_LOG.TXT"} {
    
    // FIXED: Pre-allocate string buffer to avoid runtime heap allocation
    log_path_buffer_.reserve(kMaxLogPathLen);

    
    add_children({
        &text_title_,
        &checkbox_auto_save_, &text_auto_save_,
        &field_log_format_, &text_format_,
        &number_max_size_, &text_max_size_,
        &checkbox_session_, &text_session_,
        &field_log_path_,
        &button_save_
    });
    
    button_save_.on_select = [this](Button&) {
        save_settings();
        nav_.pop();
    };
}

void LoggingSettingsView::focus() {
    button_save_.focus();
}

void LoggingSettingsView::save_settings() {
    DroneAnalyzerSettings settings;
    SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    
    settings.logging_flags.auto_save_logs = checkbox_auto_save_.value();
    settings.log_format[0] = (field_log_format_.selected_index_value() == 0) ? 'C' : 'J';
    settings.log_format[1] = (field_log_format_.selected_index_value() == 0) ? 'S' : 'S';
    settings.log_format[2] = (field_log_format_.selected_index_value() == 0) ? 'V' : 'O';
    settings.log_format[3] = (field_log_format_.selected_index_value() == 0) ? '\0' : 'N';
    if (field_log_format_.selected_index_value() == 1) {
        settings.log_format[4] = '\0';
    }
    
    settings.max_log_file_size_kb = static_cast<uint32_t>(number_max_size_.value());
    settings.logging_flags.enable_session_logging = checkbox_session_.value();
    
    safe_strcpy(settings.log_file_path, log_path_buffer_.c_str(), MAX_PATH_LEN);
    
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}

// ==========================================
// Display Settings Tab Implementation
// ==========================================

DisplaySettingsView::DisplaySettingsView(NavigationView& nav)
    : View({0, 0, 240, 304}), nav_(nav) {
    
    add_children({
        &text_title_,
        &field_color_scheme_, &text_color_,
        &field_font_size_, &text_font_,
        &field_density_, &text_density_,
        &field_waterfall_speed_, &text_waterfall_,
        &checkbox_detailed_,
        &button_save_
    });
    
    button_save_.on_select = [this](Button&) {
        save_settings();
        nav_.pop();
    };
}

void DisplaySettingsView::focus() {
    button_save_.focus();
}

void DisplaySettingsView::save_settings() {
    DroneAnalyzerSettings settings;
    SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    
    const char* schemes[] = {"DARK", "LIGHT"};
    safe_strcpy(settings.color_scheme, schemes[field_color_scheme_.selected_index_value()], MAX_NAME_LEN);
    
    settings.font_size = static_cast<uint8_t>(field_font_size_.selected_index_value());
    settings.spectrum_density = static_cast<uint8_t>(field_density_.selected_index_value());
    settings.waterfall_speed = static_cast<uint8_t>(field_waterfall_speed_.value());
    settings.display_flags.show_detailed_info = checkbox_detailed_.value();
    
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}

// ==========================================
// Advanced Settings View (Tabbed) Implementation
// ==========================================

AdvancedSettingsView::AdvancedSettingsView(NavigationView& nav)
    : View({0, 0, 240, 304}), nav_(nav),
      view_detection_(nav_), view_logging_(nav_), view_display_(nav_) {
    
    add_children({
        &tab_view_,
        &button_defaults_,
        &button_export_
    });
    
    button_defaults_.on_select = [this](Button&) {
        load_defaults();
    };
    
    button_export_.on_select = [this](Button&) {
        export_all_settings();
    };
}

void AdvancedSettingsView::focus() {
    button_defaults_.focus();
}

void AdvancedSettingsView::show_detection_tab() {
    tab_view_.set_selected(0);
}

void AdvancedSettingsView::show_logging_tab() {
    tab_view_.set_selected(1);
}

void AdvancedSettingsView::show_display_tab() {
    tab_view_.set_selected(2);
}

void AdvancedSettingsView::load_defaults() {
    DroneAnalyzerSettings settings;
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
    
    nav_.display_modal("Defaults Loaded", "Settings restored to defaults");
}

namespace {

constexpr size_t kExportBufferSize = 512;
constexpr size_t kMessageBufferSize = 128;

} 

void AdvancedSettingsView::export_all_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        nav_.display_modal("Export Failed", "Unable to load current settings");
        return;
    }

    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Export Failed", "Settings validation failed");
        return;
    }

    constexpr const char* kExportPath = "/sdcard/EDA_SETTINGS_EXPORT.txt";

    File file;
    auto error = file.append(kExportPath);
    if (error && !error->ok()) {
        nav_.display_modal("Export Failed", "Unable to create export file");
        return;
    }

    {
        SDCardLock lock;
        
        static char buffer[kExportBufferSize];
        int written;

        written = snprintf(buffer, kExportBufferSize, "# Enhanced Drone Analyzer Settings Export\n");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "# Generated: %" PRIu32 "\n\n", static_cast<uint32_t>(chTimeNow()));
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "# AUDIO SETTINGS\n");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "enable_alerts=%s\n", settings.audio_flags.enable_alerts ? "true" : "false");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "audio_alert_frequency_hz=%" PRIu32 "\n", settings.audio_alert_frequency_hz);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "\n# HARDWARE SETTINGS\n");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "hardware_bandwidth_hz=%" PRIu32 "\n", settings.hardware_bandwidth_hz);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "lna_gain_db=%u\n", settings.lna_gain_db);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "\n# SCANNING SETTINGS\n");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "scan_interval_ms=%" PRIu32 "\n", settings.scan_interval_ms);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "rssi_threshold_db=%" PRId32 "\n", settings.rssi_threshold_db);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "\n# DETECTION SETTINGS\n");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "enable_fhss_detection=%s\n", settings.detection_flags.enable_fhss_detection ? "true" : "false");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "\n# LOGGING SETTINGS\n");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "log_file_path=%s\n", settings.log_file_path);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "max_log_file_size_kb=%" PRIu32 "\n", settings.max_log_file_size_kb);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "\n# DISPLAY SETTINGS\n");
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        written = snprintf(buffer, kExportBufferSize, "color_scheme=%s\n", settings.color_scheme);
        if (written > 0 && static_cast<size_t>(written) < kExportBufferSize) {
            file.write(buffer, static_cast<size_t>(written));
        }

        file.close();
    }
    
    char msg[kMessageBufferSize];
    snprintf(msg, kMessageBufferSize, "Settings exported to:\n%s", kExportPath);
    nav_.display_modal("Export Complete", msg);
}

} // namespace ui::apps::enhanced_drone_analyzer
