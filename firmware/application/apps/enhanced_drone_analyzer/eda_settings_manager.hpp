/*
 * ========================================
 * LEGACY FILE - DEPRECATED
 * ========================================
 * This file has been DEPRECATED in favor of using
 * DroneAnalyzerSettings as single source of truth.
 * All EDAAppSettings fields have been merged
 * into DroneAnalyzerSettings.
 *
 * Migration: Use ScannerSettingsManager for
 * all settings operations.
 * ========================================
 */

// #ifndef __EDA_SETTINGS_MANAGER_HPP__
// #define __EDA_SETTINGS_MANAGER_HPP__
//
// #include <string>
// #include <vector>
// #include <cstdint>
//
// #include "ui_drone_common_types.hpp"
// #include "drone_constants.hpp"
//
// // Forward declarations
// class File;
//
// namespace ui::apps::enhanced_drone_analyzer {
//
// // Extended settings for EDA-specific configuration
// struct EDAAppSettings {
//     // Audio settings
//     uint32_t audio_alert_frequency_hz = 800;
//     uint32_t audio_alert_duration_ms = 500;
//     uint8_t audio_volume_percent = 50;
//     bool audio_repeat_alerts = false;
//
//     // Detection settings
//     bool enable_fhss_detection = true;
//     uint8_t movement_sensitivity = 3;
//     uint32_t threat_level_threshold = 2; // 0=LOW, 1=MEDIUM, 2=HIGH, 3=CRITICAL
//     bool enable_intelligent_scanning = true;
//
//     // Scanning settings
//     SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
//     uint32_t scan_interval_ms = 1000;
//     int32_t rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
//     bool enable_wideband_scanning = false;
//     uint64_t wideband_min_freq_hz = 2400000000ULL;
//     uint64_t wideband_max_freq_hz = 2500000000ULL;
//     uint32_t wideband_slice_width_hz = 24000000;
//     bool panoramic_mode_enabled = true;
//
//     // Frequency settings
//     uint64_t user_min_freq_hz = 50000000ULL;
//     uint64_t user_max_freq_hz = 6000000000ULL;
//     uint32_t hardware_bandwidth_hz = 24000000;
//     bool enable_real_hardware = true;
//
//     // Logging settings
//     bool auto_save_logs = true;
//     std::string log_file_path = "/eda_logs";
//     std::string log_format = "CSV";
//     uint32_t max_log_file_size_kb = 1024;
//     bool enable_session_logging = true;
//
//     // Display settings
//     std::string color_scheme = "DARK";
//     uint8_t font_size = 0; // 0=Small, 1=Medium, 2=Large
//     uint8_t spectrum_density = 1; // 0=Low, 1=Medium, 2=High
//     uint8_t waterfall_speed = 5; // 1-10
//     bool show_detailed_info = true;
//
//     // Preset settings
//     std::string current_preset_name = "Factory Default";
//     bool enable_quick_presets = true;
// };
//
// // Unified Settings Manager for EDA
// class EDASettingsManager {
// public:
//     explicit EDASettingsManager();
//     ~EDASettingsManager();
//
//     // Core settings operations
//     bool reload();
//     bool save();
//     bool reset_to_defaults();
//
//     // Access to settings
//     EDAAppSettings& settings() { return eda_settings_; }
//     const EDAAppSettings& settings() const { return eda_settings_; }
//
//     // Legacy compatibility methods
//     bool load_legacy(DroneAnalyzerSettings& legacy_settings);
//     bool save_legacy(const DroneAnalyzerSettings& legacy_settings);
//     void reset_to_legacy_defaults(DroneAnalyzerSettings& legacy_settings);
//
//     // Preset management
//     struct PresetInfo {
//         std::string name;
//         EDAAppSettings settings;
//     };
//
//     bool save_preset(const std::string& preset_name);
//     bool load_preset(const std::string& preset_name);
//     std::vector<std::string> list_presets() const;
//     bool delete_preset(const std::string& preset_name);
//
//     // Validation
//     struct ValidationResult {
//         bool is_valid;
//         std::string error_message;
//         uint32_t warning_count;
//         std::vector<std::string> warnings;
//
//         ValidationResult() : is_valid(true), error_message{}, warning_count(0), warnings{} {}
//     };
//
//     ValidationResult validate() const;
//     ValidationResult validate_preset(const std::string& preset_name) const;
//
//     // Settings version management
//     uint32_t get_settings_version() const { return settings_version_; }
//     bool is_settings_migration_needed() const;
//     bool migrate_settings();
//
// private:
//     std::string app_name_ = "EDA";
//     EDAAppSettings eda_settings_;
//     uint32_t settings_version_ = 1;
//
//     void initialize_default_values();
//     void apply_legacy_conversion(const DroneAnalyzerSettings& legacy);
//     void convert_to_legacy(DroneAnalyzerSettings& legacy) const;
//     void write_setting_line(::File& file, const std::string& key, const std::string& value);
//
//     // Preset file operations
//     std::string get_preset_path(const std::string& preset_name) const;
//     bool ensure_preset_directory() const;
// };
//
// } // namespace ui::apps::enhanced_drone_analyzer
//
// #endif // __EDA_SETTINGS_MANAGER_HPP__
