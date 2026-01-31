/*
 * Enhanced Drone Analyzer - Unified Settings Manager (Simplified Working Version)
 */

#include "eda_settings_manager.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "string_format.hpp"
#include "portapack_persistent_memory.hpp"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <string_view>

namespace fs = std::filesystem;

namespace ui::apps::enhanced_drone_analyzer {

using namespace portapack;

// ==========================================
// EDASettingsManager Implementation (Simplified)
// ==========================================

EDASettingsManager::EDASettingsManager()
    : eda_settings_{} {
    initialize_default_values();
}

EDASettingsManager::~EDASettingsManager() = default;

void EDASettingsManager::initialize_default_values() {
    eda_settings_.audio_alert_frequency_hz = 800;
    eda_settings_.audio_alert_duration_ms = 500;
    eda_settings_.audio_volume_percent = 50;
    eda_settings_.audio_repeat_alerts = false;

    eda_settings_.enable_fhss_detection = true;
    eda_settings_.movement_sensitivity = 3;
    eda_settings_.threat_level_threshold = 2;
    eda_settings_.enable_intelligent_scanning = true;

    eda_settings_.spectrum_mode = SpectrumMode::MEDIUM;
    eda_settings_.scan_interval_ms = 1000;
    eda_settings_.rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    eda_settings_.enable_wideband_scanning = false;
    eda_settings_.wideband_min_freq_hz = WIDEBAND_DEFAULT_MIN;
    eda_settings_.wideband_max_freq_hz = WIDEBAND_DEFAULT_MAX;
    eda_settings_.wideband_slice_width_hz = WIDEBAND_SLICE_WIDTH;
    eda_settings_.panoramic_mode_enabled = true;

    eda_settings_.user_min_freq_hz = DroneConstants::FrequencyLimits::MIN_SAFE_FREQ;
    eda_settings_.user_max_freq_hz = DroneConstants::FrequencyLimits::MAX_SAFE_FREQ;
    eda_settings_.hardware_bandwidth_hz = DroneConstants::MIN_BANDWIDTH;
    eda_settings_.enable_real_hardware = true;

    eda_settings_.auto_save_logs = true;
    eda_settings_.log_file_path = "/eda_logs";
    eda_settings_.log_format = "CSV";
    eda_settings_.max_log_file_size_kb = 1024;
    eda_settings_.enable_session_logging = true;

    eda_settings_.color_scheme = "DARK";
    eda_settings_.font_size = 0;
    eda_settings_.spectrum_density = 1;
    eda_settings_.waterfall_speed = 5;
    eda_settings_.show_detailed_info = true;

    eda_settings_.current_preset_name = "Factory Default";
    eda_settings_.enable_quick_presets = true;
}

bool EDASettingsManager::reload() {
    File settings_file;
    auto path = fs::path(settings_dir) / (std::string{app_name_} + ".ini");

    auto error = settings_file.open(path);
    if (error)
        return false;

    std::string content;
    content.resize(settings_file.size());
    settings_file.read(content.data(), content.size());
    settings_file.close();

    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        auto key = line.substr(0, pos);
        auto value = line.substr(pos + 1);

        if (key == "audio_alert_frequency_hz")
            eda_settings_.audio_alert_frequency_hz = std::stoul(value);
        else if (key == "scan_interval_ms")
            eda_settings_.scan_interval_ms = std::stoul(value);
        else if (key == "rssi_threshold_db")
            eda_settings_.rssi_threshold_db = std::stoi(value);
        else if (key == "spectrum_mode") {
            if (value == "NARROW")
                eda_settings_.spectrum_mode = SpectrumMode::NARROW;
            else if (value == "MEDIUM")
                eda_settings_.spectrum_mode = SpectrumMode::MEDIUM;
            else if (value == "WIDE")
                eda_settings_.spectrum_mode = SpectrumMode::WIDE;
            else if (value == "ULTRA_WIDE")
                eda_settings_.spectrum_mode = SpectrumMode::ULTRA_WIDE;
        }
        else if (key == "user_min_freq_hz")
            eda_settings_.user_min_freq_hz = std::stoull(value);
        else if (key == "user_max_freq_hz")
            eda_settings_.user_max_freq_hz = std::stoull(value);
    }

    return true;
}

bool EDASettingsManager::save() {
    auto validation = validate();
    if (!validation.is_valid)
        return false;

    File settings_file;
    auto path = fs::path(settings_dir) / (std::string{app_name_} + ".ini");

    fs::create_directories(settings_dir);
    auto error = settings_file.create(path);
    if (error)
        return false;

    std::stringstream ss;
    ss << "# Enhanced Drone Analyzer Settings v1.0\n";

    ss << "audio_alert_frequency_hz=" << eda_settings_.audio_alert_frequency_hz << "\n";
    ss << "scan_interval_ms=" << eda_settings_.scan_interval_ms << "\n";
    ss << "rssi_threshold_db=" << eda_settings_.rssi_threshold_db << "\n";

    const char* mode_str = "MEDIUM";
    switch (eda_settings_.spectrum_mode) {
        case SpectrumMode::NARROW: mode_str = "NARROW"; break;
        case SpectrumMode::WIDE: mode_str = "WIDE"; break;
        case SpectrumMode::ULTRA_WIDE: mode_str = "ULTRA_WIDE"; break;
    }
    ss << "spectrum_mode=" << mode_str << "\n";

    ss << "user_min_freq_hz=" << eda_settings_.user_min_freq_hz << "\n";
    ss << "user_max_freq_hz=" << eda_settings_.user_max_freq_hz << "\n";

    auto content_str = ss.str();
    settings_file.write(content_str.data(), content_str.length());
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

bool EDASettingsManager::load_legacy(DroneAnalyzerSettings& legacy_settings) {
    legacy_settings.spectrum_mode = eda_settings_.spectrum_mode;
    legacy_settings.scan_interval_ms = eda_settings_.scan_interval_ms;
    legacy_settings.rssi_threshold_db = eda_settings_.rssi_threshold_db;
    legacy_settings.audio_alert_frequency_hz = eda_settings_.audio_alert_frequency_hz;
    legacy_settings.audio_alert_duration_ms = eda_settings_.audio_alert_duration_ms;
    legacy_settings.hardware_bandwidth_hz = eda_settings_.hardware_bandwidth_hz;
    legacy_settings.enable_real_hardware = eda_settings_.enable_real_hardware;
    legacy_settings.enable_wideband_scanning = eda_settings_.enable_wideband_scanning;
    legacy_settings.wideband_min_freq_hz = eda_settings_.wideband_min_freq_hz;
    legacy_settings.wideband_max_freq_hz = eda_settings_.wideband_max_freq_hz;
    legacy_settings.user_min_freq_hz = eda_settings_.user_min_freq_hz;
    legacy_settings.user_max_freq_hz = eda_settings_.user_max_freq_hz;
    legacy_settings.wideband_slice_width_hz = eda_settings_.wideband_slice_width_hz;
    legacy_settings.panoramic_mode_enabled = eda_settings_.panoramic_mode_enabled;
    legacy_settings.show_detailed_info = eda_settings_.show_detailed_info;
    legacy_settings.auto_save_logs = eda_settings_.auto_save_logs;
    legacy_settings.log_file_path = eda_settings_.log_file_path;

    return reload();
}

bool EDASettingsManager::save_legacy(const DroneAnalyzerSettings& legacy_settings) {
    eda_settings_.spectrum_mode = legacy_settings.spectrum_mode;
    eda_settings_.scan_interval_ms = legacy_settings.scan_interval_ms;
    eda_settings_.rssi_threshold_db = legacy_settings.rssi_threshold_db;
    eda_settings_.audio_alert_frequency_hz = legacy_settings.audio_alert_frequency_hz;
    eda_settings_.audio_alert_duration_ms = legacy_settings.audio_alert_duration_ms;
    eda_settings_.hardware_bandwidth_hz = legacy_settings.hardware_bandwidth_hz;
    eda_settings_.enable_real_hardware = legacy_settings.enable_real_hardware;
    eda_settings_.enable_wideband_scanning = legacy_settings.enable_wideband_scanning;
    eda_settings_.wideband_min_freq_hz = legacy_settings.wideband_min_freq_hz;
    eda_settings_.wideband_max_freq_hz = legacy_settings.wideband_max_freq_hz;
    eda_settings_.user_min_freq_hz = legacy_settings.user_min_freq_hz;
    eda_settings_.user_max_freq_hz = legacy_settings.user_max_freq_hz;
    eda_settings_.wideband_slice_width_hz = legacy_settings.wideband_slice_width_hz;
    eda_settings_.panoramic_mode_enabled = legacy_settings.panoramic_mode_enabled;
    eda_settings_.show_detailed_info = legacy_settings.show_detailed_info;
    eda_settings_.auto_save_logs = legacy_settings.auto_save_logs;
    eda_settings_.log_file_path = legacy_settings.log_file_path;

    return save();
}

void EDASettingsManager::reset_to_legacy_defaults(DroneAnalyzerSettings& legacy_settings) {
    reset_to_defaults();
    load_legacy(legacy_settings);
}

void EDASettingsManager::apply_legacy_conversion(const DroneAnalyzerSettings& legacy) {
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
    legacy.spectrum_mode = eda_settings_.spectrum_mode;
    legacy.scan_interval_ms = eda_settings_.scan_interval_ms;
    legacy.rssi_threshold_db = eda_settings_.rssi_threshold_db;
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

std::string EDASettingsManager::get_preset_path(const std::string& preset_name) const {
    return (fs::path(settings_dir) / "EDA_PRESETS" / (preset_name + ".ini")).string();
}

bool EDASettingsManager::ensure_preset_directory() const {
    auto preset_dir = fs::path(settings_dir) / "EDA_PRESETS";
    fs::create_directories(preset_dir.string());
    return true;
}

bool EDASettingsManager::save_preset(const std::string& preset_name) {
    ensure_preset_directory();

    auto validation = validate_preset(preset_name);
    if (!validation.is_valid)
        return false;

    File preset_file;
    auto path = get_preset_path(preset_name);

    auto error = preset_file.create(path);
    if (error)
        return false;

    std::string content = "# EDA Preset: " + preset_name + "\n";

    preset_file.write(content.data(), content.length());
    preset_file.close();

    return true;
}

bool EDASettingsManager::load_preset(const std::string& preset_name) {
    auto path = get_preset_path(preset_name);

    File preset_file;
    auto error = preset_file.open(path);
    if (error)
        return false;

    std::string content;
    content.resize(preset_file.size());
    preset_file.read(content.data(), content.size());
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
    return delete_file(path);
}

EDASettingsManager::ValidationResult EDASettingsManager::validate() const {
    ValidationResult result;

    if (eda_settings_.scan_interval_ms < MIN_SCAN_INTERVAL_MS ||
        eda_settings_.scan_interval_ms > MAX_SCAN_INTERVAL_MS) {
        result.is_valid = false;
        result.error_message = "Scan interval invalid";
    }

    if (eda_settings_.rssi_threshold_db < NOISE_FLOOR_RSSI ||
        eda_settings_.rssi_threshold_db > -30) {
        if (result.is_valid) {
            result.is_valid = false;
            result.error_message = "RSSI threshold invalid";
        }
    }

    if (eda_settings_.hardware_bandwidth_hz < DroneConstants::MIN_BANDWIDTH ||
        eda_settings_.hardware_bandwidth_hz > DroneConstants::MAX_BANDWIDTH) {
        if (result.is_valid) {
            result.is_valid = false;
            result.error_message = "Bandwidth invalid";
        }
    }

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
