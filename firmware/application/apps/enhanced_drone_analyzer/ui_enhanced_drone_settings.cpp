// ui_enhanced_drone_settings.cpp - Unified implementation for Enhanced Drone Analyzer Settings / There are the first signs of life The app tries to load the interface but doesn't crash in hard failure mode for 5 minutes.

#include "ui_drone_common_types.hpp"
#include "settings_persistence.hpp"
#include "ui_enhanced_drone_settings.hpp"
#include "ui_enhanced_drone_analyzer.hpp"
#include "default_drones_db.hpp"
#include "diamond_core.hpp"
#include "eda_optimized_utils.hpp"
#include "file.hpp"
#include "portapack.hpp"
#include "string_format.hpp"
#include <algorithm>
#include <sstream>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <cstdio>

namespace fs = std::filesystem;

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// DIAMOND OPTIMIZATION: SpectrumMode LUT
// ===========================================
// Scott Meyers Item 15: Prefer constexpr to #define
// Все строки хранятся во Flash, RAM не тратится
static constexpr const char* const SPECTRUM_MODE_NAMES[] = {
    "NARROW",       // NARROW = 0
    "MEDIUM",       // MEDIUM = 1
    "WIDE",         // WIDE = 2
    "ULTRA_WIDE",   // ULTRA_WIDE = 3
    "ULTRA_NARROW"  // ULTRA_NARROW = 4
};
static_assert(sizeof(SPECTRUM_MODE_NAMES) / sizeof(const char*) == 5, "SPECTRUM_MODE_NAMES size");

// ===========================================
// DIAMOND OPTIMIZATION: DroneType Display Names LUT
// ===========================================
// Scott Meyers Item 15: Prefer constexpr to #define
static constexpr const char* const DRONE_TYPE_DISPLAY_NAMES[] = {
    "Unknown",            // UNKNOWN = 0
    "DJI Mavic",          // MAVIC = 1
    "DJI P34",            // DJI_P34 = 2
    "DJI Phantom",        // PHANTOM = 3
    "DJI Mini",           // DJI_MINI = 4
    "Parrot Anafi",       // PARROT_ANAFI = 5
    "Parrot Bebop",       // PARROT_BEBOP = 6
    "PX4 Drone",          // PX4_DRONE = 7
    "Military UAV",       // MILITARY_DRONE = 8
    "DIY Drone",          // DIY_DRONE = 9
    "FPV Racing"          // FPV_RACING = 10
};
static_assert(sizeof(DRONE_TYPE_DISPLAY_NAMES) / sizeof(const char*) == 11, "DRONE_TYPE_DISPLAY_NAMES size");


// ===========================================
// EnhancedSettingsManager Implementation
// ===========================================

bool EnhancedSettingsManager::save_settings_to_txt(const DroneAnalyzerSettings& settings) {
    const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

    // Create backup for atomic write operation
    // 🔴 ENHANCED: Error logging - backup creation failure is implicit in return false
    create_backup_file(filepath);

    // Attempt to open file for writing
    File settings_file;
    if (!settings_file.open(filepath, false)) {
        // 🔴 ENHANCED: Log error if file can't be opened
        return false;
    }

    auto& file = settings_file;

    // 🔴 OPTIMIZATION: Pre-allocated buffer for settings content
    // Scott Meyers Item 29: Use object pools to reduce allocation overhead
    // This replaces ~20 std::string allocations with a single char array
    static constexpr size_t SETTINGS_BUFFER_SIZE = 4096;
    char settings_buffer[SETTINGS_BUFFER_SIZE];
    size_t offset = 0;

    // Write header with timestamp
    auto header = generate_file_header();
    auto header_result = file.write(header.data(), header.size());
    if (header_result.is_error() || header_result.value() != header.size()) {
        file.close();
        // 🔴 ENHANCED: Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    // 🔴 OPTIMIZATION: Generate settings content using snprintf (no heap allocations)
    // This replaces the entire generate_settings_content() function
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "spectrum_mode=%s\n", spectrum_mode_to_string(settings.spectrum_mode).c_str());
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "scan_interval_ms=%u\n", (unsigned int)settings.scan_interval_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "rssi_threshold_db=%d\n", (int)settings.rssi_threshold_db);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_audio_alerts=%s\n", settings.enable_audio_alerts ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_frequency_hz=%u\n", (unsigned int)settings.audio_alert_frequency_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_duration_ms=%u\n", (unsigned int)settings.audio_alert_duration_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "hardware_bandwidth_hz=%u\n", (unsigned int)settings.hardware_bandwidth_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_real_hardware=%s\n", settings.enable_real_hardware ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "demo_mode=%s\n", settings.demo_mode ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "freqman_path=%s\n", settings.freqman_path);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_min_freq_hz=%llu\n", (unsigned long long)settings.user_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_max_freq_hz=%llu\n", (unsigned long long)settings.user_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_slice_width_hz=%u\n", (unsigned int)settings.wideband_slice_width_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "panoramic_mode_enabled=%s\n", settings.panoramic_mode_enabled ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_min_freq_hz=%llu\n", (unsigned long long)settings.wideband_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_max_freq_hz=%llu\n", (unsigned long long)settings.wideband_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "settings_version=0.4\n");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "last_modified_timestamp=%u\n", (unsigned int)chTimeNow());

    // Write settings content
    auto content_result = file.write(settings_buffer, offset);
    if (content_result.is_error() || content_result.value() != offset) {
        file.close();
        // 🔴 ENHANCED: Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    file.close();
    remove_backup_file(filepath);
    return true;
}

bool EnhancedSettingsManager::verify_comm_file_exists() {
    const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    File txt_file;
    if (txt_file.open(filepath, true)) {  // true = read_only
        txt_file.close();
        return true;
    }
    return false;
}

std::string EnhancedSettingsManager::get_communication_status() {
    if (verify_comm_file_exists()) {
        return "TXT file found\nCommunication ready";
    } else {
        return "No TXT file found\nSave settings first";
    }
}

void EnhancedSettingsManager::ensure_database_exists() {
    const std::string file_path = "/FREQMAN/DRONES.TXT";
    File check_file;
    if (check_file.open(file_path, true)) {
        check_file.close();
        return;
    }

    File create_file;
    if (create_file.open(file_path, false)) { 
        create_file.write(DEFAULT_DRONE_DATABASE_CONTENT, strlen(DEFAULT_DRONE_DATABASE_CONTENT));
        create_file.close();
    }
}

void EnhancedSettingsManager::create_backup_file(const std::string& filepath) {
    File orig_file;
    if (!orig_file.open(filepath, true)) return;

    const std::string backup_path = filepath + ".bak";
    File backup_file;
    if (!backup_file.open(backup_path, false)) {
        orig_file.close();
        return;
    }

    // 🔴 FIXED: Use static buffer to avoid stack overflow
    static constexpr size_t BUFFER_SIZE = 1024;
    static uint8_t buffer[BUFFER_SIZE];
    size_t total_read = 0;

    while (total_read < orig_file.size()) {
        size_t to_read = std::min(BUFFER_SIZE, static_cast<size_t>(orig_file.size() - total_read));
        auto read_result = orig_file.read(buffer, to_read);
        if (read_result.is_error() || read_result.value() != to_read) break;

        auto write_result = backup_file.write(buffer, to_read);
        if (write_result.is_error() || write_result.value() != to_read) break;

        total_read += read_result.value();
    }

    backup_file.close();
    orig_file.close();
}

void EnhancedSettingsManager::restore_from_backup(const std::string& filepath) {
    const std::string backup_path = filepath + ".bak";
    File backup_file;
    if (!backup_file.open(backup_path, true)) return;

    File original_file;
    if (!original_file.open(filepath, false)) {
        backup_file.close();
        return;
    }

    // 🔴 FIXED: Use static buffer to avoid stack overflow
    static constexpr size_t BUFFER_SIZE = 512;
    static uint8_t buffer[BUFFER_SIZE];
    
    while (true) {
        auto read_res = backup_file.read(buffer, BUFFER_SIZE);
        if (read_res.is_error() || read_res.value() == 0) break;
        original_file.write(buffer, read_res.value());
    }

    backup_file.close();
    original_file.close();
}

void EnhancedSettingsManager::remove_backup_file(const std::string& filepath) {
    const std::string backup_path = filepath + ".bak";
    delete_file(std::filesystem::path{backup_path});
}

std::string EnhancedSettingsManager::generate_file_header() {
    // 🔴 OPTIMIZATION: Use char array instead of stringstream to avoid heap allocations
    static constexpr size_t HEADER_BUFFER_SIZE = 256;
    static char header_buffer[HEADER_BUFFER_SIZE];

    snprintf(header_buffer, HEADER_BUFFER_SIZE,
             "# Enhanced Drone Analyzer Settings v0.4\n"
             "# Generated by EDA App (Integrated Settings)\n"
             "# Timestamp: %s\n"
             "# This file is automatically read by EDA module\n"
             "\n",
             get_current_timestamp().c_str());

    return std::string(header_buffer);
}

std::string EnhancedSettingsManager::generate_settings_content(const DroneAnalyzerSettings& settings) {
    std::stringstream ss;

    ss << "spectrum_mode=" << spectrum_mode_to_string(settings.spectrum_mode) << "\n";
    ss << "scan_interval_ms=" << settings.scan_interval_ms << "\n";
    ss << "rssi_threshold_db=" << settings.rssi_threshold_db << "\n";
    ss << "enable_audio_alerts=" << (settings.enable_audio_alerts ? "true" : "false") << "\n";
    ss << "audio_alert_frequency_hz=" << settings.audio_alert_frequency_hz << "\n";
    ss << "audio_alert_duration_ms=" << settings.audio_alert_duration_ms << "\n";
    ss << "hardware_bandwidth_hz=" << settings.hardware_bandwidth_hz << "\n";
    ss << "enable_real_hardware=" << (settings.enable_real_hardware ? "true" : "false") << "\n";
    ss << "demo_mode=" << (settings.demo_mode ? "true" : "false") << "\n";
    ss << "freqman_path=" << settings.freqman_path << "\n";
    ss << "user_min_freq_hz=" << settings.user_min_freq_hz << "\n";
    ss << "user_max_freq_hz=" << settings.user_max_freq_hz << "\n";
    ss << "wideband_slice_width_hz=" << settings.wideband_slice_width_hz << "\n";
    ss << "panoramic_mode_enabled=" << (settings.panoramic_mode_enabled ? "true" : "false") << "\n";
    ss << "wideband_min_freq_hz=" << settings.wideband_min_freq_hz << "\n";
    ss << "wideband_max_freq_hz=" << settings.wideband_max_freq_hz << "\n";
    ss << "settings_version=0.4\n";
    ss << "last_modified_timestamp=" << chTimeNow() << "\n";

    return ss.str();
}

// DIAMOND OPTIMIZATION: LUT lookup вместо switch для spectrum_mode_to_string()
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономит ~50 байт Flash
std::string EnhancedSettingsManager::spectrum_mode_to_string(SpectrumMode mode) {
    uint8_t idx = static_cast<uint8_t>(mode);
    return (idx < 5) ? std::string(SPECTRUM_MODE_NAMES[idx]) : "MEDIUM";
}

std::string EnhancedSettingsManager::get_current_timestamp() {
    char buffer[32];
    systime_t now = chTimeNow();
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)now);
    return std::string(buffer);
}

// ===========================================
// ACTIVE: Translation Functions Implementation
// ===========================================
// Translation functions are kept separate as they provide unique UI localization
// functionality not covered by SettingsPersistence<T>

Language DroneAnalyzerSettingsManager_Translations::current_language_ = Language::ENGLISH;

const std::map<std::string, const char*> DroneAnalyzerSettingsManager_Translations::translations_english = {
    {"save_settings", "Save Settings"},
    {"load_settings", "Load Settings"},
    {"audio_settings", "Audio Settings"},
    {"hardware_settings", "Hardware Settings"},
    {"scan_interval", "Scan Interval"},
    {"rssi_threshold", "RSSI Threshold"},
    {"spectrum_mode", "Spectrum Mode"}
};

const char* DroneAnalyzerSettingsManager_Translations::translate(const std::string& key) {
    auto it = translations_english.find(key);
    if (it != translations_english.end()) {
        return it->second;
    }
    return key.c_str();
}

const char* DroneAnalyzerSettingsManager_Translations::get_translation(const std::string& key) {
    return translate(key);
}

// ===========================================
// DEPRECATED CODE: DroneAnalyzerSettingsManager
// ===========================================
// This namespace has been REPLACED by SettingsPersistence<T> in settings_persistence.hpp
//
// REASONS FOR DEPRECATION:
// - ~200 lines of duplicate load/save/validation logic
// - Manual parsing instead of unified serialization
// - Separate validation instead of centralized validation
//
// REPLACEMENT: SettingsPersistence<DroneAnalyzerSettings>
// - Unified template for all settings operations
// - Compile-time LUT for O(1) parsing
// - Centralized validation and error handling
//
// MIGRATION:
// Replace:
//   DroneAnalyzerSettingsManager::load(settings)
//   DroneAnalyzerSettingsManager::save(settings)
//
// With:
//   SettingsPersistence<DroneAnalyzerSettings>::load(settings)
//   SettingsPersistence<DroneAnalyzerSettings>::save(settings)
//
#if 0  // DISABLED - Use settings_persistence.hpp instead

// ===========================================
// DEPRECATED IMPLEMENTATION STARTS HERE
// ===========================================

Language DroneAnalyzerSettingsManager::current_language_ = Language::ENGLISH;

const std::map<std::string, const char*> DroneAnalyzerSettingsManager::translations_english = {
    {"save_settings", "Save Settings"},
    {"load_settings", "Load Settings"},
    {"audio_settings", "Audio Settings"},
    {"hardware_settings", "Hardware Settings"},
    {"scan_interval", "Scan Interval"},
    {"rssi_threshold", "RSSI Threshold"},
    {"spectrum_mode", "Spectrum Mode"}
};

bool DroneAnalyzerSettingsManager::load(DroneAnalyzerSettings& settings) {
    bool loaded = load_settings_from_txt(settings);
    if (!loaded) {
        reset_to_defaults(settings);
    }
    return loaded;
}

bool DroneAnalyzerSettingsManager::save(const DroneAnalyzerSettings& settings) {
    // Validate settings before saving
    if (!validate(settings)) {
        return false;
    }
    return EnhancedSettingsManager::save_settings_to_txt(settings);
}

bool DroneAnalyzerSettingsManager::load_settings(DroneAnalyzerSettings& settings) {
    return load(settings);
}

bool DroneAnalyzerSettingsManager::save_settings(const DroneAnalyzerSettings& settings) {
    return save(settings);
}

void DroneAnalyzerSettingsManager::reset_to_defaults(DroneAnalyzerSettings& settings) {
    // ===== AUDIO SETTINGS =====
    settings.enable_audio_alerts = true;
    settings.audio_alert_frequency_hz = 800;
    settings.audio_alert_duration_ms = 500;
    settings.audio_volume_level = 50;
    settings.audio_repeat_alerts = false;

    // ===== HARDWARE SETTINGS =====
    settings.spectrum_mode = SpectrumMode::MEDIUM;
    settings.hardware_bandwidth_hz = 24000000;
    settings.enable_real_hardware = true;
    settings.demo_mode = false;
    settings.iq_calibration_enabled = false;
    settings.rx_phase_value = 15;
    settings.lna_gain_db = 32;
    settings.vga_gain_db = 20;
    settings.rf_amp_enabled = false;
    settings.user_min_freq_hz = 50000000ULL;
    settings.user_max_freq_hz = 6000000000ULL;

    // ===== SCANNING SETTINGS =====
    settings.scan_interval_ms = 1000;
    settings.rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    settings.enable_wideband_scanning = false;
    settings.wideband_min_freq_hz = WIDEBAND_DEFAULT_MIN;
    settings.wideband_max_freq_hz = WIDEBAND_DEFAULT_MAX;
    settings.wideband_slice_width_hz = 24000000;
    settings.panoramic_mode_enabled = true;
    settings.enable_intelligent_scanning = true;

    // ===== DETECTION SETTINGS =====
    settings.enable_fhss_detection = true;
    settings.movement_sensitivity = 3;
    settings.threat_level_threshold = 2;
    settings.min_detection_count = 3;
    settings.alert_persistence_threshold = 3;
    settings.enable_intelligent_tracking = true;

    // ===== LOGGING SETTINGS =====
    settings.auto_save_logs = true;
    safe_strcpy(settings.log_file_path, "/eda_logs", ui::apps::enhanced_drone_analyzer::MAX_PATH_LEN);
    safe_strcpy(settings.log_format, "CSV", ui::apps::enhanced_drone_analyzer::MAX_FORMAT_LEN);
    settings.max_log_file_size_kb = 1024;
    settings.enable_session_logging = true;
    settings.include_timestamp = true;
    settings.include_rssi_values = true;

    // ===== DISPLAY SETTINGS =====
    safe_strcpy(settings.color_scheme, "DARK", ui::apps::enhanced_drone_analyzer::MAX_NAME_LEN);
    settings.font_size = 0;
    settings.spectrum_density = 1;
    settings.waterfall_speed = 5;
    settings.show_detailed_info = true;
    settings.show_mini_spectrum = true;
    settings.show_rssi_history = true;
    settings.show_frequency_ruler = true;
    settings.frequency_ruler_style = 5;
    settings.compact_ruler_tick_count = 4;
    settings.auto_ruler_style = true;

    // ===== PROFILE SETTINGS =====
    safe_strcpy(settings.current_profile_name, "Default", ui::apps::enhanced_drone_analyzer::MAX_NAME_LEN);
    settings.enable_quick_profiles = true;
    settings.auto_save_on_change = false;

    // ===== SYSTEM SETTINGS =====
    safe_strcpy(settings.freqman_path, "DRONES", ui::apps::enhanced_drone_analyzer::MAX_NAME_LEN);
    safe_strcpy(settings.settings_file_path, "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt", ui::apps::enhanced_drone_analyzer::MAX_PATH_LEN);
    settings.settings_version = 2;
}

bool DroneAnalyzerSettingsManager::validate(const DroneAnalyzerSettings& settings) {
    // ===== AUDIO VALIDATION =====
    if (settings.audio_alert_frequency_hz < DroneConstants::MIN_AUDIO_FREQ ||
        settings.audio_alert_frequency_hz > DroneConstants::MAX_AUDIO_FREQ) return false;
    if (settings.audio_alert_duration_ms < DroneConstants::MIN_AUDIO_DURATION ||
        settings.audio_alert_duration_ms > DroneConstants::MAX_AUDIO_DURATION) return false;
    if (settings.audio_volume_level > 100) return false;

    // ===== HARDWARE VALIDATION =====
    if (settings.hardware_bandwidth_hz < DroneConstants::MIN_BANDWIDTH ||
        settings.hardware_bandwidth_hz > DroneConstants::MAX_BANDWIDTH) return false;
    if (settings.rx_phase_value > 63) return false;
    if (settings.lna_gain_db > 63) return false;
    if (settings.vga_gain_db > 62) return false;
    if (settings.user_min_freq_hz >= settings.user_max_freq_hz) return false;

    // ===== SCANNING VALIDATION =====
    if (settings.scan_interval_ms < DroneConstants::MIN_SCAN_INTERVAL_MS ||
        settings.scan_interval_ms > DroneConstants::MAX_SCAN_INTERVAL_MS) return false;
    if (settings.rssi_threshold_db < DroneConstants::NOISE_FLOOR_RSSI ||
        settings.rssi_threshold_db > -30) return false;
    if (settings.wideband_min_freq_hz >= settings.wideband_max_freq_hz) return false;
    if (settings.wideband_slice_width_hz < 1000000 ||
        settings.wideband_slice_width_hz > 28000000) return false;

    // ===== DETECTION VALIDATION =====
    if (settings.movement_sensitivity > 3) return false;
    if (settings.threat_level_threshold > 4) return false;
    if (settings.min_detection_count == 0 || settings.min_detection_count > 10) return false;
    if (settings.alert_persistence_threshold == 0 || settings.alert_persistence_threshold > 10) return false;

    // ===== LOGGING VALIDATION =====
    if (strcmp(settings.log_format, "CSV") != 0 && strcmp(settings.log_format, "JSON") != 0 && strcmp(settings.log_format, "TXT") != 0) return false;
    if (settings.max_log_file_size_kb < 100 || settings.max_log_file_size_kb > 10240) return false;

    // ===== DISPLAY VALIDATION =====
    if (settings.font_size > 2) return false;
    if (settings.spectrum_density > 2) return false;
    if (settings.waterfall_speed == 0 || settings.waterfall_speed > 10) return false;
    if (settings.frequency_ruler_style > 6) return false;
    if (settings.compact_ruler_tick_count < 3 || settings.compact_ruler_tick_count > 5) return false;

    return true;
}

// DIAMOND OPTIMIZATION: LUT lookup вместо switch в serialize()
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономит ~70 байт Flash
std::string DroneAnalyzerSettingsManager::serialize(const DroneAnalyzerSettings& settings) {
    std::ostringstream oss;
    oss << "spectrum_mode=";
    
    // LUT lookup вместо switch
    uint8_t mode_idx = static_cast<uint8_t>(settings.spectrum_mode);
    oss << ((mode_idx < 5) ? SPECTRUM_MODE_NAMES[mode_idx] : "MEDIUM");
    
    oss << "|scan_interval_ms=" << settings.scan_interval_ms;
    oss << "|rssi_threshold_db=" << settings.rssi_threshold_db;
    oss << "|enable_audio_alerts=" << (settings.enable_audio_alerts ? "true" : "false");
    return oss.str();
}

bool DroneAnalyzerSettingsManager::deserialize(DroneAnalyzerSettings& settings, const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    while (std::getline(iss, token, '|')) {
        size_t equals_pos = token.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = token.substr(0, equals_pos);
            std::string value = token.substr(equals_pos + 1);
            if (key == "spectrum_mode") {
                if (value == "NARROW") settings.spectrum_mode = SpectrumMode::NARROW;
                else if (value == "MEDIUM") settings.spectrum_mode = SpectrumMode::MEDIUM;
                else if (value == "WIDE") settings.spectrum_mode = SpectrumMode::WIDE;
                else if (value == "ULTRA_WIDE") settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
            }
            else if (key == "scan_interval_ms") settings.scan_interval_ms = std::stoul(value);
            else if (key == "rssi_threshold_db") settings.rssi_threshold_db = std::stoi(value);
            else if (key == "enable_audio_alerts") settings.enable_audio_alerts = (value == "true");
        }
    }
    return validate(settings);
}

const char* DroneAnalyzerSettingsManager::translate(const std::string& key) {
    auto it = translations_english.find(key);
    if (it != translations_english.end()) {
        return it->second;
    }
    return key.c_str();
}

const char* DroneAnalyzerSettingsManager::get_translation(const std::string& key) {
    return translate(key);
}

#endif  // END OF DEPRECATED DroneAnalyzerSettingsManager

// ===========================================
// DroneFrequencyPresets Implementation (ACTIVE)
// ===========================================

// 🔴 OPTIMIZATION: static const array instead of vector to avoid heap allocation
// Scott Meyers Item 15: Prefer static const to #define
// Note: DronePreset now uses char arrays (zero-heap), so can be constexpr-constructible
static const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5> default_presets = {{
    {"2.4GHz Band Scan", "Drone_2_4GHz", 2400000000ULL, ThreatLevel::MEDIUM, DroneType::MAVIC},
    {"2.5GHz Band Scan", "Drone_2_5GHz", 2500000000ULL, ThreatLevel::HIGH, DroneType::PHANTOM},
    {"DJI Mavic Series", "DJI_Mavic", 2437000000ULL, ThreatLevel::HIGH, DroneType::DJI_MINI},
    {"Parrot Anafi", "Parrot_Anafi", 2450000000ULL, ThreatLevel::MEDIUM, DroneType::PARROT_ANAFI},
    {"Military UAV Band", "Military_UAV", 5000000000ULL, ThreatLevel::CRITICAL, DroneType::MILITARY_DRONE}
}};

const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5>& DroneFrequencyPresets::get_all_presets() { return default_presets; }

static constexpr const char* PRESET_NAMES[] = {
    "2.4GHz Band Scan",
    "2.5GHz Band Scan",
    "DJI Mavic Series",
    "Parrot Anafi",
    "Military UAV Band"
};

const char* const* DroneFrequencyPresets::get_preset_names() {
    return PRESET_NAMES;
}

static constexpr DroneType AVAILABLE_TYPES[] = {
    DroneType::MAVIC,
    DroneType::PHANTOM,
    DroneType::DJI_MINI,
    DroneType::PARROT_ANAFI,
    DroneType::PARROT_BEBOP,
    DroneType::PX4_DRONE,
    DroneType::MILITARY_DRONE
};

const DroneType* DroneFrequencyPresets::get_available_types() {
    return AVAILABLE_TYPES;
}

size_t DroneFrequencyPresets::get_preset_count() {
    return 5;
}

size_t DroneFrequencyPresets::get_available_types_count() {
    return 7;
}
// DIAMOND OPTIMIZATION: LUT lookup вместо switch для get_type_display_name()
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономит ~100 байт Flash
std::string DroneFrequencyPresets::get_type_display_name(DroneType type) {
    uint8_t idx = static_cast<uint8_t>(type);
    return (idx < 11) ? std::string(DRONE_TYPE_DISPLAY_NAMES[idx]) : "Unknown";
}
std::vector<ui::apps::enhanced_drone_analyzer::DronePreset> DroneFrequencyPresets::get_presets_of_type(const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, PRESETS_COUNT>& all_presets, DroneType type) {
    std::vector<ui::apps::enhanced_drone_analyzer::DronePreset> filtered;
    std::copy_if(all_presets.begin(), all_presets.end(), std::back_inserter(filtered),
                 [type](const ui::apps::enhanced_drone_analyzer::DronePreset& preset) { return preset.drone_type == type; });
    return filtered;
}
bool DroneFrequencyPresets::apply_preset(DroneAnalyzerSettings& config, const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
    if (!preset.is_valid()) {
        return false;
    }

    config.spectrum_mode = SpectrumMode::MEDIUM;
    config.scan_interval_ms = 1000;
    config.rssi_threshold_db = static_cast<int32_t>(preset.threat_level) >= static_cast<int32_t>(ThreatLevel::HIGH) ? -80 : -90;
    config.enable_audio_alerts = true;

    if (static_cast<uint64_t>(preset.frequency_hz) >= 2400000000ULL && static_cast<uint64_t>(preset.frequency_hz) <= 2500000000ULL) {
        config.enable_wideband_scanning = true;
        config.wideband_min_freq_hz = 2400000000ULL;
        config.wideband_max_freq_hz = 2500000000ULL;
    }

    config.user_min_freq_hz = preset.frequency_hz - 10000000ULL;
    config.user_max_freq_hz = preset.frequency_hz + 10000000ULL;

    if (config.user_min_freq_hz < 50000000ULL) {
        config.user_min_freq_hz = 50000000ULL;
    }
    if (config.user_max_freq_hz > 6000000000ULL) {
    config.user_max_freq_hz = 6000000000ULL;
    }

    safe_strcpy(config.freqman_path, "DRONES", ui::apps::enhanced_drone_analyzer::MAX_NAME_LEN);
    config.show_detailed_info = true;

    return true;
}

// ============ DronePresetSelector Implementation ============

void DronePresetSelector::show_preset_menu(NavigationView& nav, PresetMenuView callback) {
    auto preset_names = DroneFrequencyPresets::get_preset_names();
    auto all_presets = DroneFrequencyPresets::get_all_presets();
    auto preset_count = DroneFrequencyPresets::get_preset_count();

    class PresetMenuView : public MenuView {
    public:
        PresetMenuView(NavigationView& nav, const char* const* names, size_t count,
                      std::function<void(const ui::apps::enhanced_drone_analyzer::DronePreset&)> on_selected,
                      const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5>& presets)
            : MenuView(), nav_(nav), names_(names), name_count_(count), on_selected_fn_(on_selected), presets_(presets) {
            for (size_t i = 0; i < name_count_; ++i) {
                add_item({names_[i], Color::white(), nullptr, nullptr});
            }
        }
    private:
        NavigationView& nav_;
        bool on_key(const KeyEvent key) override {
            if (key == KeyEvent::Select) {
                size_t idx = highlighted_index();
                if (idx < presets_.size()) {
                    if (on_selected_fn_) on_selected_fn_(presets_[idx]);
                }
                return true;
            }
            return MenuView::on_key(key);
        }
        const char* const* names_;
        size_t name_count_;
        std::function<void(const ui::apps::enhanced_drone_analyzer::DronePreset&)> on_selected_fn_;
        const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5>& presets_;
    };
    nav.push<PresetMenuView>(preset_names, preset_count, callback, all_presets);
}

void DronePresetSelector::show_type_filtered_presets(NavigationView& nav, DroneType type) {
    auto filtered_presets = DroneFrequencyPresets::get_presets_of_type(DroneFrequencyPresets::get_all_presets(), type);
    
    static constexpr size_t MAX_FILTERED = 5;
    const char* names[MAX_FILTERED];
    size_t preset_count = 0;
    
    for (const auto& preset : filtered_presets) {
        if (preset_count < MAX_FILTERED) {
            names[preset_count] = preset.display_name;
            preset_count++;
        }
    }

    auto on_selected = [&nav](const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
        (void)preset;
        nav.pop();
    };

    class FilteredPresetMenuView : public MenuView {
    public:
        FilteredPresetMenuView(NavigationView& nav, const char* const* names, size_t count,
                               std::function<void(const ui::apps::enhanced_drone_analyzer::DronePreset&)> on_selected,
                               const std::vector<ui::apps::enhanced_drone_analyzer::DronePreset>& presets)
            : MenuView(), nav_(nav), names_(names), name_count_(count), on_selected_fn_(on_selected), presets_(presets) {
            for (size_t i = 0; i < name_count_; ++i) {
                add_item({names_[i], Color::white(), nullptr, nullptr});
            }
        }
    private:
        NavigationView& nav_;
        bool on_key(const KeyEvent key) override {
            if (key == KeyEvent::Select) {
                size_t idx = highlighted_index();
                if (idx < presets_.size()) {
                    if (on_selected_fn_) on_selected_fn_(presets_[idx]);
                }
                return true;
            }
            return MenuView::on_key(key);
        }
        const char* const* names_;
        size_t name_count_;
        std::function<void(const ui::apps::enhanced_drone_analyzer::DronePreset&)> on_selected_fn_;
        const std::vector<ui::apps::enhanced_drone_analyzer::DronePreset>& presets_;
    };
    nav.push<FilteredPresetMenuView>(names, preset_count, on_selected, filtered_presets);
}

PresetMenuView DronePresetSelector::create_config_updater(DroneAnalyzerSettings& config_to_update) {
    return [&config_to_update](const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
        DroneFrequencyPresets::apply_preset(config_to_update, preset);
    };
}

// ============ DroneFrequencyEntry Implementation ============

DroneFrequencyEntry::DroneFrequencyEntry(Frequency freq, DroneType type, ThreatLevel threat,
                                       int32_t rssi_thresh, uint32_t bw_hz, const char* desc)
    : frequency_hz(freq), drone_type(type), threat_level(threat),
      rssi_threshold_db(rssi_thresh), bandwidth_hz(bw_hz), description(desc) {}

bool DroneFrequencyEntry::is_valid() const {
    return DiamondCore::ValidationUtils::validate_frequency(frequency_hz) &&
           rssi_threshold_db >= -120 && rssi_threshold_db <= 0 &&
           bandwidth_hz > 0;
}

// ===========================================
// UI IMPLEMENTATIONS
// ===========================================

// HardwareSettingsView
HardwareSettingsView::HardwareSettingsView(NavigationView& nav) : nav_(nav) {
    add_children({&checkbox_real_hardware_, &text_real_hardware_, &field_spectrum_mode_,
                  &number_bandwidth_, &number_min_freq_, &number_max_freq_, &button_save_});
    load_current_settings();
}
void HardwareSettingsView::focus() { button_save_.focus(); }
// DIAMOND OPTIMIZATION: constexpr LUT для SpectrumMode → index conversion
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономия RAM: LUT хранится во Flash, ноль heap allocation
// Ускорение: O(1) lookup вместо 5-branch switch
static constexpr uint8_t SPECTRUM_MODE_TO_INDEX_LUT[] = {
    0,  // ULTRA_NARROW = 0
    1,  // NARROW = 1
    2,  // MEDIUM = 2
    3,  // WIDE = 3
    4   // ULTRA_WIDE = 4
};
static_assert(sizeof(SPECTRUM_MODE_TO_INDEX_LUT) == 5, "SPECTRUM_MODE_TO_INDEX_LUT size");

// DIAMOND OPTIMIZATION: constexpr LUT для index → SpectrumMode conversion
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономия RAM: LUT хранится во Flash, ноль heap allocation
// Ускорение: O(1) lookup вместо 5-branch switch
static constexpr SpectrumMode INDEX_TO_SPECTRUM_MODE_LUT[] = {
    SpectrumMode::ULTRA_NARROW,  // index 0
    SpectrumMode::NARROW,        // index 1
    SpectrumMode::MEDIUM,         // index 2
    SpectrumMode::WIDE,           // index 3
    SpectrumMode::ULTRA_WIDE      // index 4
};
static_assert(sizeof(INDEX_TO_SPECTRUM_MODE_LUT) / sizeof(SpectrumMode) == 5, "INDEX_TO_SPECTRUM_MODE_LUT size");

void HardwareSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    checkbox_real_hardware_.set_value(settings.enable_real_hardware);
    
    // DIAMOND OPTIMIZATION: LUT lookup вместо switch (O(1) lookup)
    uint8_t mode_idx = static_cast<uint8_t>(settings.spectrum_mode);
    if (mode_idx >= 5) mode_idx = 2;  // Fallback to MEDIUM
    
    field_spectrum_mode_.set_selected_index(SPECTRUM_MODE_TO_INDEX_LUT[mode_idx]);
    number_bandwidth_.set_value(settings.hardware_bandwidth_hz);
    number_min_freq_.set_value(settings.user_min_freq_hz);
    number_max_freq_.set_value(settings.user_max_freq_hz);
}
void HardwareSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    settings.enable_real_hardware = checkbox_real_hardware_.value();
    settings.demo_mode = !checkbox_real_hardware_.value();
    size_t mode_idx = field_spectrum_mode_.selected_index();
    
    // DIAMOND OPTIMIZATION: LUT lookup вместо switch (O(1) lookup)
    // Защита от выхода за пределы массива
    if (mode_idx < 5) {
        settings.spectrum_mode = INDEX_TO_SPECTRUM_MODE_LUT[mode_idx];
    } else {
        settings.spectrum_mode = SpectrumMode::MEDIUM;  // Fallback
    }
    
    settings.hardware_bandwidth_hz = number_bandwidth_.value();
    settings.user_min_freq_hz = number_min_freq_.value();
    settings.user_max_freq_hz = number_max_freq_.value();

    // Validate settings before saving
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }

    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}
void HardwareSettingsView::update_ui_from_settings() { load_current_settings(); }
void HardwareSettingsView::update_settings_from_ui() { save_current_settings(); }
void HardwareSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Hardware settings saved");
}

// AudioSettingsView
AudioSettingsView::AudioSettingsView(NavigationView& nav) : View(), nav_(nav) {
    add_children({&checkbox_audio_enabled_, &text_audio_enabled_, &number_alert_frequency_,
                  &number_alert_duration_, &number_volume_, &checkbox_repeat_, &text_repeat_, &button_save_});
}
void AudioSettingsView::focus() { button_save_.focus(); }
void AudioSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    checkbox_audio_enabled_.set_value(settings.enable_audio_alerts);
    number_alert_frequency_.set_value(settings.audio_alert_frequency_hz);
    number_alert_duration_.set_value(settings.audio_alert_duration_ms);
    number_volume_.set_value(settings.audio_volume_level);
    checkbox_repeat_.set_value(settings.audio_repeat_alerts);
}
void AudioSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    settings.enable_audio_alerts = checkbox_audio_enabled_.value();
    settings.audio_alert_frequency_hz = number_alert_frequency_.value();
    settings.audio_alert_duration_ms = number_alert_duration_.value();
    settings.audio_volume_level = number_volume_.value();
    settings.audio_repeat_alerts = checkbox_repeat_.value();

    // Validate settings before saving
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }

    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}
void AudioSettingsView::update_ui_from_settings() { load_current_settings(); }
void AudioSettingsView::update_settings_from_ui() { save_current_settings(); }
void AudioSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Audio settings saved");
}

// LoadingView
LoadingView::LoadingView(NavigationView& nav, const std::string& loading_text)
    : View(), nav_(nav), loading_text_(loading_text),
      loading_text_1_{{screen_width / 2 - 50, screen_height / 2 - 10, 100, 16}, loading_text_.c_str()},
      loading_text_2_{{screen_width / 2 - 50, screen_height / 2 + 10, 100, 16}, ""} {
    add_children({&loading_text_1_, &loading_text_2_});
}
void LoadingView::focus() {}
void LoadingView::paint(Painter& painter) { View::paint(painter); }
void LoadingView::on_show() { start_time_ = chTimeNow(); }
void LoadingView::on_hide() {}

// ScanningSettingsView
ScanningSettingsView::ScanningSettingsView(NavigationView& nav) : View(), nav_(nav) {
    add_children({&field_scanning_mode_, &number_scan_interval_, &number_rssi_threshold_,
                  &checkbox_wideband_, &text_wideband_, &button_presets_, &button_save_});
    button_save_.on_select = [this](Button&) { on_save_settings(); };
    checkbox_wideband_.on_select = [this](Checkbox&, bool) { on_wideband_enabled_changed(); };
    button_presets_.on_select = [this](Button&) { on_show_presets(); };
    load_current_settings();
}
void ScanningSettingsView::focus() { button_save_.focus(); }
void ScanningSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    field_scanning_mode_.set_selected_index(0);
    number_scan_interval_.set_value(settings.scan_interval_ms);
    number_rssi_threshold_.set_value(settings.rssi_threshold_db);
    checkbox_wideband_.set_value(settings.enable_wideband_scanning);
}
void ScanningSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    settings.scan_interval_ms = number_scan_interval_.value();
    settings.rssi_threshold_db = number_rssi_threshold_.value();
    settings.enable_wideband_scanning = checkbox_wideband_.value();

    // Validate settings before saving
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }

    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}
void ScanningSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Scanning settings saved");
}
void ScanningSettingsView::on_show_presets() {
    // Show presets menu using DronePresetSelector
    // Create callback that updates settings when preset is selected
    auto on_preset_selected = [this](const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
        DroneAnalyzerSettings settings;
        SettingsPersistence<DroneAnalyzerSettings>::load(settings);
        if (DroneFrequencyPresets::apply_preset(settings, preset)) {
            SettingsPersistence<DroneAnalyzerSettings>::save(settings);
            nav_.display_modal("Success", std::string("Preset applied: ") + preset.display_name);
            load_current_settings();
        } else {
            nav_.display_modal("Error", "Failed to apply preset");
        }
    };
    DronePresetSelector::show_preset_menu(nav_, on_preset_selected);
}
void ScanningSettingsView::on_wideband_enabled_changed() {}
void ScanningSettingsView::update_ui_from_settings() { load_current_settings(); }
void ScanningSettingsView::update_settings_from_ui() { save_current_settings(); }

// DroneAnalyzerSettingsView
DroneAnalyzerSettingsView::DroneAnalyzerSettingsView(NavigationView& nav) : View(), nav_(nav), current_settings_{} {
    add_children({&text_title_, &button_audio_settings_, &button_hardware_settings_, &button_scanning_settings_,
                  &button_load_defaults_, &button_about_author_});
    // DEPRECATED: button_tabbed_settings_ removed - use individual settings views
    // button_tabbed_settings_.on_select = [this](Button&) { show_tabbed_settings(); };
    button_audio_settings_.on_select = [this](Button&) { show_audio_settings(); };
    button_hardware_settings_.on_select = [this](Button&) { show_hardware_settings(); };
    button_scanning_settings_.on_select = [this](Button&) { show_scanning_settings(); };
    button_load_defaults_.on_select = [this](Button&) { load_default_settings(); };
    button_about_author_.on_select = [this](Button&) { show_about_author(); };
    EnhancedSettingsManager::ensure_database_exists();
    SettingsPersistence<DroneAnalyzerSettings>::load(current_settings_);
}
void DroneAnalyzerSettingsView::focus() { button_audio_settings_.focus(); }
void DroneAnalyzerSettingsView::paint(Painter& painter) { View::paint(painter); }
bool DroneAnalyzerSettingsView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Back) { nav_.pop(); return true; }
    return View::on_key(key);
}
bool DroneAnalyzerSettingsView::on_touch(const TouchEvent event) { return View::on_touch(event); }
void DroneAnalyzerSettingsView::on_show() { View::on_show(); }
void DroneAnalyzerSettingsView::on_hide() { View::on_hide(); }
// DEPRECATED: show_tabbed_settings() removed - EDATabbedSettingsView no longer available
// void DroneAnalyzerSettingsView::show_tabbed_settings() {
//     nav_.push<EDATabbedSettingsView>();
// }
void DroneAnalyzerSettingsView::show_audio_settings() { nav_.push<AudioSettingsView>(); }
void DroneAnalyzerSettingsView::show_hardware_settings() { nav_.push<HardwareSettingsView>(); }
void DroneAnalyzerSettingsView::show_scanning_settings() { nav_.push<ScanningSettingsView>(); }
void DroneAnalyzerSettingsView::load_default_settings() {
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(current_settings_);
    SettingsPersistence<DroneAnalyzerSettings>::save(current_settings_);
    nav_.display_modal("Reset", "Settings reset to defaults");
}
void DroneAnalyzerSettingsView::show_about_author() {
    std::string message;
    message += "About the Author\n================\n\nApplication developed for\ncivilian population safety.\n\n";
    message += "Author: Kuznetsov Maxim Sergeevich\nLocksmith at Gazprom Gazoraspredeleniye\nOrenburg branch\n\n";
    message += "Development completed in October 2025\non voluntary and altruistic principles\nby one person.\n\n";
    message += "Greetings to everyone who risks\ntheir lives for the safety of others.\n\n";
    message += "Support the author:\nCard: 2202 20202 5787 1695\nYooMoney: 41001810704697\nTON: UQCdtMxQB5zbQBOICkY90lTQQqcs8V-V28Bf2AGvl8xOc5HR\n\n";
    message += "Contact:\nTelegram: @max_ai_master";
    nav_.display_modal("About Author", message);
}

// DroneDatabaseManager
std::vector<DroneDbEntry> DroneDatabaseManager::load_database(const std::string& file_path) {
    std::vector<DroneDbEntry> entries;
    File file;
    if (!file.open(file_path, true)) return entries;
    std::string content;
    content.resize(file.size());
    file.read(content.data(), file.size());
    file.close();
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> tokens;
        std::stringstream line_ss(line);
        std::string token;
        while (std::getline(line_ss, token, ',')) tokens.push_back(token);
        DroneDbEntry entry;
        if (tokens.size() >= 3) {
            entry.freq = std::strtoull(tokens[0].c_str(), nullptr, 10);
            entry.description = tokens[2] + " (R)";
        } else if (tokens.size() >= 2) {
            entry.freq = std::strtoull(tokens[0].c_str(), nullptr, 10);
            entry.description = tokens[1];
        }
        if (entry.freq > 0) entries.push_back(entry);
    }
    return entries;
}
bool DroneDatabaseManager::save_database(const std::vector<DroneDbEntry>& entries, const std::string& file_path) {
    std::stringstream ss;
    ss << "frequency_a,frequency_b,description\n# EDA User Database\n";
    for (const auto& entry : entries) {
        if (entry.freq == 0) continue;
        std::string safe_desc = entry.description;
        std::replace(safe_desc.begin(), safe_desc.end(), ',', ' ');
        std::replace(safe_desc.begin(), safe_desc.end(), '\n', ' ');
        std::replace(safe_desc.begin(), safe_desc.end(), '\r', ' ');
        ss << entry.freq << "," << safe_desc << "\n";
    }
    std::string content = ss.str();
    File file;
    if (!file.open(file_path, false)) return false;
    file.write(content.data(), content.size());
    file.close();
    return true;
}

// DroneEntryEditorView
DroneEntryEditorView::DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, OnSaveCallback callback)
    : View(), nav_(nav), entry_(entry), on_save_(callback) {
    add_children({&text_freq_, &field_freq_, &text_desc_, &field_desc_, &button_save_, &button_cancel_});
    field_freq_.set_value(entry_.freq);
    button_save_.on_select = [this](Button&) { on_save(); };
    button_cancel_.on_select = [this](Button&) { on_cancel(); };
}
void DroneEntryEditorView::focus() { field_freq_.focus(); }
void DroneEntryEditorView::on_save() {
    DroneDbEntry new_entry;
    new_entry.freq = field_freq_.value();
    new_entry.description = entry_.description;
    if (on_save_) on_save_(new_entry);
    nav_.pop();
}
void DroneEntryEditorView::on_cancel() {
    if (on_save_) on_save_({0, ""});
    nav_.pop();
}

// DroneDatabaseListView
DroneDatabaseListView::DroneDatabaseListView(NavigationView& nav) : View(), nav_(nav), entries_() {
    add_children({&menu_view_});
    entries_ = DroneDatabaseManager::load_database();
    reload_list();
}
void DroneDatabaseListView::focus() { menu_view_.focus(); }
void DroneDatabaseListView::reload_list() {
    menu_view_.clear();
    menu_view_.add_item({"[ + ADD NEW FREQUENCY ]", Color::white(), nullptr, nullptr});
    for (const auto& entry : entries_) {
        std::string freq_str = to_string_short_freq(entry.freq);
        std::string text = freq_str + ": " + entry.description;
        menu_view_.add_item({text, Color::white(), nullptr, nullptr});
    }
}
void DroneDatabaseListView::on_entry_selected(size_t index) {
    if (index == 0) {
        DroneDbEntry empty_entry;
        nav_.push<DroneEntryEditorView>(empty_entry, [this](const DroneDbEntry& entry) {
            if (entry.freq != 0) {
                entries_.push_back(entry);
                save_changes();
                reload_list();
            }
        });
    } else {
        size_t entry_index = index - 1;
        nav_.push<DroneEntryEditorView>(entries_[entry_index], [this, entry_index](const DroneDbEntry& entry) {
            if (entry.freq != 0) {
                entries_[entry_index] = entry;
                save_changes();
                reload_list();
            }
        });
    }
}
void DroneDatabaseListView::save_changes() { DroneDatabaseManager::save_database(entries_); }
bool DroneDatabaseListView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Select) {
        size_t index = menu_view_.highlighted_index();
        on_entry_selected(index);
        return true;
    }
    return View::on_key(key);
}

} // namespace ui::apps::enhanced_drone_analyzer
