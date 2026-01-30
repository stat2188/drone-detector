/*
 * Enhanced Drone Analyzer - Unified Settings Manager Implementation
 */

#include "eda_settings_manager.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "string_format.hpp"
#include "file_reader.hpp"

#include <algorithm>
#include <cstring>

namespace fs = std::filesystem;

namespace ui::apps::enhanced_drone_analyzer {

// ==========================================
// EDASettingsManager Implementation
// ==========================================

EDASettingsManager::EDASettingsManager()
    : eda_settings_{} {
    initialize_default_values();
}

EDASettingsManager::~EDASettingsManager() = default;

void EDASettingsManager::initialize_default_values() {
    // Audio defaults
    eda_settings_.audio_alert_frequency_hz = 800;
    eda_settings_.audio_alert_duration_ms = 500;
    eda_settings_.audio_volume_percent = 50;
    eda_settings_.audio_repeat_alerts = false;

    // Detection defaults
    eda_settings_.enable_fhss_detection = true;
    eda_settings_.movement_sensitivity = 3;
    eda_settings_.threat_level_threshold = 2;
    eda_settings_.enable_intelligent_scanning = true;

    // Scanning defaults
    eda_settings_.spectrum_mode = SpectrumMode::MEDIUM;
    eda_settings_.scan_interval_ms = 1000;
    eda_settings_.rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    eda_settings_.enable_wideband_scanning = false;
    eda_settings_.wideband_min_freq_hz = WIDEBAND_DEFAULT_MIN;
    eda_settings_.wideband_max_freq_hz = WIDEBAND_DEFAULT_MAX;
    eda_settings_.wideband_slice_width_hz = WIDEBAND_SLICE_WIDTH;
    eda_settings_.panoramic_mode_enabled = true;

    // Frequency defaults
    eda_settings_.user_min_freq_hz = DroneConstants::FrequencyLimits::MIN_SAFE_FREQ;
    eda_settings_.user_max_freq_hz = DroneConstants::FrequencyLimits::MAX_SAFE_FREQ;
    eda_settings_.hardware_bandwidth_hz = DroneConstants::MIN_BANDWIDTH;
    eda_settings_.enable_real_hardware = true;

    // Logging defaults
    eda_settings_.auto_save_logs = true;
    eda_settings_.log_file_path = "/eda_logs";
    eda_settings_.log_format = "CSV";
    eda_settings_.max_log_file_size_kb = 1024;
    eda_settings_.enable_session_logging = true;

    // Display defaults
    eda_settings_.color_scheme = "DARK";
    eda_settings_.font_size = 0;
    eda_settings_.spectrum_density = 1;
    eda_settings_.waterfall_speed = 5;
    eda_settings_.show_detailed_info = true;

    // Preset defaults
    eda_settings_.current_preset_name = "Factory Default";
    eda_settings_.enable_quick_presets = true;
}

bool EDASettingsManager::reload() {
    File settings_file;
    auto path = get_settings_path(std::string{app_name_});

    auto error = settings_file.open(path);
    if (error)
        return false;

    auto reader = FileLineReader(settings_file);
    for (const auto& line : reader) {
        auto cols = split_string(line, '=');
        if (cols.size() != 2)
            continue;

        const auto& key = cols[0];
        const auto& value = cols[1];

        // Audio settings
        if (key == "audio_alert_frequency_hz")
            parse_int(value, eda_settings_.audio_alert_frequency_hz);
        else if (key == "audio_alert_duration_ms")
            parse_int(value, eda_settings_.audio_alert_duration_ms);
        else if (key == "audio_volume_percent")
            parse_int(value, eda_settings_.audio_volume_percent);
        else if (key == "audio_repeat_alerts")
            eda_settings_.audio_repeat_alerts = (value == "1");

        // Detection settings
        else if (key == "enable_fhss_detection")
            eda_settings_.enable_fhss_detection = (value == "1");
        else if (key == "movement_sensitivity")
            parse_int(value, eda_settings_.movement_sensitivity);
        else if (key == "threat_level_threshold")
            parse_int(value, eda_settings_.threat_level_threshold);
        else if (key == "enable_intelligent_scanning")
            eda_settings_.enable_intelligent_scanning = (value == "1");

        // Scanning settings
        else if (key == "spectrum_mode") {
            if (value == "NARROW")
                eda_settings_.spectrum_mode = SpectrumMode::NARROW;
            else if (value == "MEDIUM")
                eda_settings_.spectrum_mode = SpectrumMode::MEDIUM;
            else if (value == "WIDE")
                eda_settings_.spectrum_mode = SpectrumMode::WIDE;
            else if (value == "ULTRA_WIDE")
                eda_settings_.spectrum_mode = SpectrumMode::ULTRA_WIDE;
            else if (value == "ULTRA_NARROW")
                eda_settings_.spectrum_mode = SpectrumMode::ULTRA_NARROW;
        }
        else if (key == "scan_interval_ms")
            parse_int(value, eda_settings_.scan_interval_ms);
        else if (key == "rssi_threshold_db")
            parse_int(value, eda_settings_.rssi_threshold_db);
        else if (key == "enable_wideband_scanning")
            eda_settings_.enable_wideband_scanning = (value == "1");
        else if (key == "wideband_min_freq_hz")
            parse_int(value, eda_settings_.wideband_min_freq_hz);
        else if (key == "wideband_max_freq_hz")
            parse_int(value, eda_settings_.wideband_max_freq_hz);
        else if (key == "wideband_slice_width_hz")
            parse_int(value, eda_settings_.wideband_slice_width_hz);
        else if (key == "panoramic_mode_enabled")
            eda_settings_.panoramic_mode_enabled = (value == "1");

        // Frequency settings
        else if (key == "user_min_freq_hz")
            parse_int(value, eda_settings_.user_min_freq_hz);
        else if (key == "user_max_freq_hz")
            parse_int(value, eda_settings_.user_max_freq_hz);
        else if (key == "hardware_bandwidth_hz")
            parse_int(value, eda_settings_.hardware_bandwidth_hz);
        else if (key == "enable_real_hardware")
            eda_settings_.enable_real_hardware = (value == "1");

        // Logging settings
        else if (key == "auto_save_logs")
            eda_settings_.auto_save_logs = (value == "1");
        else if (key == "log_file_path")
            eda_settings_.log_file_path = trim(value);
        else if (key == "log_format")
            eda_settings_.log_format = trim(value);
        else if (key == "max_log_file_size_kb")
            parse_int(value, eda_settings_.max_log_file_size_kb);
        else if (key == "enable_session_logging")
            eda_settings_.enable_session_logging = (value == "1");

        // Display settings
        else if (key == "color_scheme")
            eda_settings_.color_scheme = trim(value);
        else if (key == "font_size")
            parse_int(value, eda_settings_.font_size);
        else if (key == "spectrum_density")
            parse_int(value, eda_settings_.spectrum_density);
        else if (key == "waterfall_speed")
            parse_int(value, eda_settings_.waterfall_speed);
        else if (key == "show_detailed_info")
            eda_settings_.show_detailed_info = (value == "1");

        // Preset settings
        else if (key == "current_preset_name")
            eda_settings_.current_preset_name = trim(value);
        else if (key == "enable_quick_presets")
            eda_settings_.enable_quick_presets = (value == "1");

        // Version tracking
        else if (key == "settings_version")
            parse_int(value, settings_version_);
    }

    settings_file.close();
    return true;
}

bool EDASettingsManager::save() {
    // Validate before saving
    auto validation = validate();
    if (!validation.is_valid) {
        return false;
    }

    File settings_file;
    auto path = get_settings_path(std::string{app_name_});

    ensure_directory(settings_dir);
    auto error = settings_file.create(path);
    if (error)
        return false;

    // Header
    auto header = "# Enhanced Drone Analyzer Settings v1.0\n";
    settings_file.write(header.data(), header.length());

    // Audio settings
    write_setting_line(settings_file, "audio_alert_frequency_hz", to_string_dec_uint(eda_settings_.audio_alert_frequency_hz));
    write_setting_line(settings_file, "audio_alert_duration_ms", to_string_dec_uint(eda_settings_.audio_alert_duration_ms));
    write_setting_line(settings_file, "audio_volume_percent", to_string_dec_uint(eda_settings_.audio_volume_percent));
    write_setting_line(settings_file, "audio_repeat_alerts", eda_settings_.audio_repeat_alerts ? "1" : "0");

    // Detection settings
    write_setting_line(settings_file, "enable_fhss_detection", eda_settings_.enable_fhss_detection ? "1" : "0");
    write_setting_line(settings_file, "movement_sensitivity", to_string_dec_uint(eda_settings_.movement_sensitivity));
    write_setting_line(settings_file, "threat_level_threshold", to_string_dec_uint(eda_settings_.threat_level_threshold));
    write_setting_line(settings_file, "enable_intelligent_scanning", eda_settings_.enable_intelligent_scanning ? "1" : "0");

    // Scanning settings
    const char* mode_str = "MEDIUM";
    switch (eda_settings_.spectrum_mode) {
        case SpectrumMode::NARROW: mode_str = "NARROW"; break;
        case SpectrumMode::MEDIUM: mode_str = "MEDIUM"; break;
        case SpectrumMode::WIDE: mode_str = "WIDE"; break;
        case SpectrumMode::ULTRA_WIDE: mode_str = "ULTRA_WIDE"; break;
        case SpectrumMode::ULTRA_NARROW: mode_str = "ULTRA_NARROW"; break;
    }
    write_setting_line(settings_file, "spectrum_mode", mode_str);
    write_setting_line(settings_file, "scan_interval_ms", to_string_dec_uint(eda_settings_.scan_interval_ms));
    write_setting_line(settings_file, "rssi_threshold_db", to_string_dec_int(eda_settings_.rssi_threshold_db));
    write_setting_line(settings_file, "enable_wideband_scanning", eda_settings_.enable_wideband_scanning ? "1" : "0");
    write_setting_line(settings_file, "wideband_min_freq_hz", to_string_dec_uint(eda_settings_.wideband_min_freq_hz));
    write_setting_line(settings_file, "wideband_max_freq_hz", to_string_dec_uint(eda_settings_.wideband_max_freq_hz));
    write_setting_line(settings_file, "wideband_slice_width_hz", to_string_dec_uint(eda_settings_.wideband_slice_width_hz));
    write_setting_line(settings_file, "panoramic_mode_enabled", eda_settings_.panoramic_mode_enabled ? "1" : "0");

    // Frequency settings
    write_setting_line(settings_file, "user_min_freq_hz", to_string_dec_uint(eda_settings_.user_min_freq_hz));
    write_setting_line(settings_file, "user_max_freq_hz", to_string_dec_uint(eda_settings_.user_max_freq_hz));
    write_setting_line(settings_file, "hardware_bandwidth_hz", to_string_dec_uint(eda_settings_.hardware_bandwidth_hz));
    write_setting_line(settings_file, "enable_real_hardware", eda_settings_.enable_real_hardware ? "1" : "0");

    // Logging settings
    write_setting_line(settings_file, "auto_save_logs", eda_settings_.auto_save_logs ? "1" : "0");
    write_setting_line(settings_file, "log_file_path", eda_settings_.log_file_path);
    write_setting_line(settings_file, "log_format", eda_settings_.log_format);
    write_setting_line(settings_file, "max_log_file_size_kb", to_string_dec_uint(eda_settings_.max_log_file_size_kb));
    write_setting_line(settings_file, "enable_session_logging", eda_settings_.enable_session_logging ? "1" : "0");

    // Display settings
    write_setting_line(settings_file, "color_scheme", eda_settings_.color_scheme);
    write_setting_line(settings_file, "font_size", to_string_dec_uint(eda_settings_.font_size));
    write_setting_line(settings_file, "spectrum_density", to_string_dec_uint(eda_settings_.spectrum_density));
    write_setting_line(settings_file, "waterfall_speed", to_string_dec_uint(eda_settings_.waterfall_speed));
    write_setting_line(settings_file, "show_detailed_info", eda_settings_.show_detailed_info ? "1" : "0");

    // Preset settings
    write_setting_line(settings_file, "current_preset_name", eda_settings_.current_preset_name);
    write_setting_line(settings_file, "enable_quick_presets", eda_settings_.enable_quick_presets ? "1" : "0");

    // Version
    write_setting_line(settings_file, "settings_version", to_string_dec_uint(settings_version_));

    settings_file.close();
    return true;
}

void EDASettingsManager::write_setting_line(File& file, const std::string& key, const std::string& value) {
    file.write(key.data(), key.length());
    file.write("=", 1);
    file.write(value.data(), value.length());
    file.write("\r\n", 2);
}

bool EDASettingsManager::reset_to_defaults() {
    initialize_default_values();
    return save();
}

// ==========================================
// Legacy Compatibility
// ==========================================

bool EDASettingsManager::load_legacy(DroneAnalyzerSettings& legacy_settings) {
    apply_legacy_conversion(legacy_settings);
    return reload();
}

bool EDASettingsManager::save_legacy(const DroneAnalyzerSettings& legacy_settings) {
    // Load current settings first
    reload();

    // Apply legacy settings to current
    // (Conversion logic goes here)

    return save();
}

void EDASettingsManager::reset_to_legacy_defaults(DroneAnalyzerSettings& legacy_settings) {
    // Reset to defaults in legacy format
    // Reset and save legacy settings
    // Then load into new format
    reset_to_defaults();
    load_legacy(legacy_settings);
}

void EDASettingsManager::apply_legacy_conversion(const DroneAnalyzerSettings& legacy) {
    // Convert legacy DroneAnalyzerSettings to EDAAppSettings
    eda_settings_.spectrum_mode = legacy.spectrum_mode;
    eda_settings_.scan_interval_ms = legacy.scan_interval_ms;
    eda_settings_.rssi_threshold_db = legacy.rssi_threshold_db;
    eda_settings_.audio_alert_frequency_hz = legacy.audio_alert_frequency_hz;
    eda_settings_.audio_alert_duration_ms = legacy.audio_alert_duration_ms;
    eda_settings_.hardware_bandwidth_hz = legacy.hardware_bandwidth_hz;
    eda_settings_.enable_real_hardware = legacy.enable_real_hardware;
    eda_settings_.enable_wideband_scanning = legacy.enable_wideband_scanning;
    eda_settings_.wideband_min_freq_hz = legacy.wideband_min_freq_hz;
    eda_settings_.wideband_max_freq_hz = legacy.wideband_max_freq_hz;
    eda_settings_.user_min_freq_hz = legacy.user_min_freq_hz;
    eda_settings_.user_max_freq_hz = legacy.user_max_freq_hz;
    eda_settings_.wideband_slice_width_hz = legacy.wideband_slice_width_hz;
    eda_settings_.panoramic_mode_enabled = legacy.panoramic_mode_enabled;
    eda_settings_.show_detailed_info = legacy.show_detailed_info;
    eda_settings_.auto_save_logs = legacy.auto_save_logs;
    eda_settings_.log_file_path = legacy.log_file_path;
}

void EDASettingsManager::convert_to_legacy(DroneAnalyzerSettings& legacy) const {
    // Convert EDAAppSettings back to legacy format
    legacy.spectrum_mode = eda_settings_.spectrum_mode;
    legacy.scan_interval_ms = eda_settings_.scan_interval_ms;
    legacy.rssi_threshold_db = eda_settings_.rssi_threshold_db;
    legacy.enable_audio_alerts = true;
    legacy.audio_alert_frequency_hz = eda_settings_.audio_alert_frequency_hz;
    legacy.audio_alert_duration_ms = eda_settings_.audio_alert_duration_ms;
    legacy.hardware_bandwidth_hz = eda_settings_.hardware_bandwidth_hz;
    legacy.enable_real_hardware = eda_settings_.enable_real_hardware;
    legacy.demo_mode = !eda_settings_.enable_real_hardware;
    legacy.enable_wideband_scanning = eda_settings_.enable_wideband_scanning;
    legacy.wideband_min_freq_hz = eda_settings_.wideband_min_freq_hz;
    legacy.wideband_max_freq_hz = eda_settings_.wideband_max_freq_hz;
    legacy.user_min_freq_hz = eda_settings_.user_min_freq_hz;
    legacy.user_max_freq_hz = eda_settings_.user_max_freq_hz;
    legacy.wideband_slice_width_hz = eda_settings_.wideband_slice_width_hz;
    legacy.panoramic_mode_enabled = eda_settings_.panoramic_mode_enabled;
    legacy.show_detailed_info = eda_settings_.show_detailed_info;
    legacy.auto_save_logs = eda_settings_.auto_save_logs;
    legacy.log_file_path = eda_settings_.log_file_path;
}

// ==========================================
// Preset Management
// ==========================================

std::string EDASettingsManager::get_preset_path(const std::string& preset_name) const {
    return (fs::path(settings_dir) / "EDA_PRESETS" / (preset_name + ".ini")).string();
}

bool EDASettingsManager::ensure_preset_directory() const {
    auto preset_dir = fs::path(settings_dir) / "EDA_PRESETS";
    ensure_directory(preset_dir.string());
    return true;
}

bool EDASettingsManager::save_preset(const std::string& preset_name) {
    ensure_preset_directory();

    // Validate preset name
    auto validation = validate_preset(preset_name);
    if (!validation.is_valid) {
        return false;
    }

    // Save current settings as preset
    File preset_file;
    auto path = get_preset_path(preset_name);

    auto error = preset_file.create(path);
    if (error)
        return false;

    preset_file.write("# EDA Preset: ", 14);
    preset_file.write(preset_name.data(), preset_name.length());
    preset_file.write("\r\n", 2);

    preset_file.close();
    return true;
}

bool EDASettingsManager::load_preset(const std::string& preset_name) {
    auto path = get_preset_path(preset_name);

    File preset_file;
    auto error = preset_file.open(path);
    if (error)
        return false;

    preset_file.close();
    eda_settings_.current_preset_name = preset_name;
    return true;
}

std::vector<std::string> EDASettingsManager::list_presets() const {
    std::vector<std::string> presets;
    return presets;
}

bool EDASettingsManager::delete_preset(const std::string& preset_name) {
    auto path = get_preset_path(preset_name);
    return filesystem::delete_file(path);
}

// ==========================================
// Validation
// ==========================================

EDASettingsManager::ValidationResult EDASettingsManager::validate() const {
    ValidationResult result;

    // Validate scan interval
    if (eda_settings_.scan_interval_ms < MIN_SCAN_INTERVAL_MS ||
        eda_settings_.scan_interval_ms > MAX_SCAN_INTERVAL_MS) {
        result.is_valid = false;
        result.error_message = "Scan interval invalid";
    }

    // Validate RSSI threshold
    if (eda_settings_.rssi_threshold_db < NOISE_FLOOR_RSSI ||
        eda_settings_.rssi_threshold_db > -30) {
        if (result.is_valid) {
            result.is_valid = false;
            result.error_message = "RSSI threshold invalid";
        }
    }

    // Validate bandwidth
    if (eda_settings_.hardware_bandwidth_hz < DroneConstants::MIN_BANDWIDTH ||
        eda_settings_.hardware_bandwidth_hz > DroneConstants::MAX_BANDWIDTH) {
        if (result.is_valid) {
            result.is_valid = false;
            result.error_message = "Bandwidth invalid";
        }
    }

    // Validate frequency range
    if (eda_settings_.user_min_freq_hz >= eda_settings_.user_max_freq_hz) {
        if (result.is_valid) {
            result.is_valid = false;
            result.error_message = "Frequency range invalid";
        }
    }

    return result;
}

EDASettingsManager::ValidationResult EDASettingsManager::validate_preset(const std::string& preset_name) const {
    ValidationResult result;

    if (preset_name.empty() || preset_name.length() > 32) {
        result.is_valid = false;
        result.error_message = "Invalid preset name";
    }

    return result;
}

// ==========================================
// Settings Version Management
// ==========================================

bool EDASettingsManager::is_settings_migration_needed() const {
    return settings_version_ < 1;
}

bool EDASettingsManager::migrate_settings() {
    if (!is_settings_migration_needed())
        return true;

    settings_version_ = 1;
    return save();
}

} // namespace ui::apps::enhanced_drone_analyzer
