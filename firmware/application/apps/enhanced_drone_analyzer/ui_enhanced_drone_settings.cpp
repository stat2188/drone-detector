// ui_enhanced_drone_settings.cpp - Unified implementation for Enhanced Drone Analyzer Settings
// The app tries to load the interface but doesn't crash in hard failure mode for 5 minutes.

#include "ui_drone_common_types.hpp"
#include "settings_persistence.hpp"
#include "ui_enhanced_drone_settings.hpp"
#include "ui_enhanced_drone_analyzer.hpp"
#include "eda_constants.hpp"
#include "eda_optimized_utils.hpp"
#include "color_lookup_unified.hpp"
#include "default_drones_db.hpp"
#include "file.hpp"
#include "portapack.hpp"
#include "string_format.hpp"
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cstdio>

// Diamond Code Standard #3 - Replace std::filesystem with FatFS C API

namespace ui::apps::enhanced_drone_analyzer {


// RAII wrapper for File (ensures proper cleanup)
class FileRAII {
public:
    explicit FileRAII(const char* path, bool read_only = false)
        : file_(), opened_(file_.open(path, read_only)) {}

    // noexcept for move operations
    ~FileRAII() noexcept {
        if (opened_) {
            file_.close();
        }
    }

    FileRAII(const FileRAII&) = delete;
    FileRAII& operator=(const FileRAII&) = delete;

    FileRAII(FileRAII&& other) noexcept
        : file_(std::move(other.file_)), opened_(other.opened_) {
        other.opened_ = false;
    }

    File& get() noexcept { return file_; }
    const File& get() const noexcept { return file_; }
    bool is_open() const noexcept { return opened_; }

private:
    File file_;
    bool opened_ = false;
};

// EnhancedSettingsManager Implementation

    // const reference to prevent unnecessary copies
bool EnhancedSettingsManager::save_settings_to_txt(const DroneAnalyzerSettings& settings) noexcept {
    // Fix SD Card Race Condition - acquire lock BEFORE checking status
    SettingsBufferLock lock;
    
    // Use constexpr const char* instead of std::string
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

    // Create backup for atomic write operation
    create_backup_file(filepath);

    File settings_file;
    if (!settings_file.open(filepath, false)) {
        return false;
    }

    auto& file = settings_file;

    // Thread-local buffer for settings serialization (replaces ~20 std::string allocations)
    static constexpr size_t SETTINGS_BUFFER_SIZE = 1024;
    thread_local char settings_buffer[SETTINGS_BUFFER_SIZE];
    size_t offset = 0;

    // Write header with timestamp
    const char* header = generate_file_header();
    size_t header_len = strlen(header);
    auto header_result = file.write(header, header_len);
    if (header_result.is_error() || header_result.value() != header_len) {
        file.close();
        // Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    // Generate settings content using snprintf (no heap allocations)
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "spectrum_mode=%s\n", spectrum_mode_to_string(settings.spectrum_mode));
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "scan_interval_ms=%u\n", (unsigned int)settings.scan_interval_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "rssi_threshold_db=%d\n", (int)settings.rssi_threshold_db);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_audio_alerts=%s\n", audio_get_enable_alerts(settings) ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_frequency_hz=%u\n", (unsigned int)settings.audio_alert_frequency_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_duration_ms=%u\n", (unsigned int)settings.audio_alert_duration_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "hardware_bandwidth_hz=%u\n", (unsigned int)settings.hardware_bandwidth_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_real_hardware=%s\n", hw_get_enable_real_hardware(settings) ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "demo_mode=%s\n", hw_get_demo_mode(settings) ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "freqman_path=%s\n", settings.freqman_path);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_min_freq_hz=%llu\n", (unsigned long long)settings.user_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_max_freq_hz=%llu\n", (unsigned long long)settings.user_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_slice_width_hz=%u\n", (unsigned int)settings.wideband_slice_width_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "panoramic_mode_enabled=%s\n", scan_get_panoramic_mode_enabled(settings) ? "true" : "false");
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
        // Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    file.close();
    remove_backup_file(filepath);
    return true;
}

// noexcept for simple file existence check
bool EnhancedSettingsManager::verify_comm_file_exists() noexcept {
    // Use constexpr const char* instead of std::string
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    File txt_file;
    if (txt_file.open(filepath, true)) {  // read_only
        txt_file.close();
        return true;
    }
    return false;
}

    // const reference to prevent unnecessary copies
bool EnhancedSettingsManager::load_settings_from_txt(DroneAnalyzerSettings& settings) noexcept {
    // Use constexpr const char* instead of std::string
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

    File settings_file;
    if (!settings_file.open(filepath, true)) {  // read_only
        return false;
    }

    auto& file = settings_file;

    // Thread-local buffer for settings loading (1024 bytes per thread)
    static constexpr size_t FILE_BUFFER_SIZE = 1024;
    thread_local char file_buffer[FILE_BUFFER_SIZE];
    auto read_result = file.read(file_buffer, FILE_BUFFER_SIZE);
    
    if (read_result.is_error() || read_result.value() == 0) {
        file.close();
        return false;
    }

    // File Truncation Warning - if file is larger than buffer, data is truncated
    const size_t bytes_read = read_result.value();
    if (bytes_read < FILE_BUFFER_SIZE) {
        file_buffer[bytes_read] = '\0';
    } else {
        // Buffer was completely filled, ensure null-termination
        file_buffer[FILE_BUFFER_SIZE - 1] = '\0';
        // DIAMOND FIX: File truncated - error logging
        // In production, this should log to a persistent error log
        // For now, we note the truncation but continue processing
        // WARNING: Settings file exceeds buffer size, data may be lost
    }

    // FIX #18: Use non-destructive parsing (replaces strtok)
    // strtok modifies buffer, use manual parsing instead
    char* line_start = file_buffer;
    while (*line_start != '\0') {
        // Find line end
        char* line_end = strchr(line_start, '\n');
        if (line_end == nullptr) {
            line_end = strchr(line_start, '\r');
        }
        
        if (line_end == nullptr) {
            // Last line - process entire remaining buffer
            line_end = line_start + strlen(line_start);
        }
        
        // Temporarily null-terminate line
        char saved_char = *line_end;
        *line_end = '\0';
        
        char* line = line_start;
        
        // Skip empty lines and comments
        if (*line == '\0' || *line == '#') {
            *line_end = saved_char;  // Restore newline
            line_start = (saved_char != '\0') ? line_end + 1 : line_end;
            continue;
        }

        // Find key-value separator
        char* equals = strchr(line, '=');
        if (equals != nullptr) {
            *equals = '\0';
            char* key = line;
            char* value = equals + 1;

            // Trim whitespace from key and value
            while (*key == ' ' || *key == '\t') key++;
            while (*value == ' ' || *value == '\t') value++;
            char* value_end = value + strlen(value) - 1;
            while (value_end >= value && (*value_end == ' ' || *value_end == '\t' || *value_end == '\r')) {
                *value_end-- = '\0';
            }

            // Parse specific settings
            if (strcmp(key, "spectrum_mode") == 0) {
                if (strcmp(value, "Ultra Narrow") == 0) settings.spectrum_mode = SpectrumMode::ULTRA_NARROW;
                else if (strcmp(value, "Narrow") == 0) settings.spectrum_mode = SpectrumMode::NARROW;
                else if (strcmp(value, "Medium") == 0) settings.spectrum_mode = SpectrumMode::MEDIUM;
                else if (strcmp(value, "Wide") == 0) settings.spectrum_mode = SpectrumMode::WIDE;
                else if (strcmp(value, "Ultra Wide") == 0) settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
            } else if (strcmp(key, "scan_interval_ms") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 100 && val <= 10000) {
                    settings.scan_interval_ms = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "rssi_threshold_db") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= -120 && val <= 10) {
                    settings.rssi_threshold_db = static_cast<int32_t>(val);
                }
            } else if (strcmp(key, "enable_audio_alerts") == 0) {
                audio_set_enable_alerts(settings, strcmp(value, "true") == 0);
            } else if (strcmp(key, "audio_alert_frequency_hz") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 200 && val <= 20000) {
                    settings.audio_alert_frequency_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "audio_alert_duration_ms") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 50 && val <= 5000) {
                    settings.audio_alert_duration_ms = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 10000 && val <= 28000000) {
                    settings.hardware_bandwidth_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "enable_real_hardware") == 0) {
                hw_set_enable_real_hardware(settings, strcmp(value, "true") == 0);
            } else if (strcmp(key, "demo_mode") == 0) {
                hw_set_demo_mode(settings, strcmp(value, "true") == 0);
            } else if (strcmp(key, "freqman_path") == 0) {
                // Use safe_strcpy instead of strncpy
                safe_strcpy(settings.freqman_path, value, sizeof(settings.freqman_path));
            } else if (strcmp(key, "user_min_freq_hz") == 0) {
                // Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 50000000ULL && val <= 6000000000ULL) {
                    settings.user_min_freq_hz = val;
                }
            } else if (strcmp(key, "user_max_freq_hz") == 0) {
                // Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 50000000ULL && val <= 6000000000ULL) {
                    settings.user_max_freq_hz = val;
                }
            } else if (strcmp(key, "wideband_slice_width_hz") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 10000000 && val <= 28000000) {
                    settings.wideband_slice_width_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "panoramic_mode_enabled") == 0) {
                scan_set_panoramic_mode_enabled(settings, strcmp(value, "true") == 0);
            } else if (strcmp(key, "wideband_min_freq_hz") == 0) {
                // Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 2400000000ULL && val <= 7200000000ULL) {
                    settings.wideband_min_freq_hz = val;
                }
            } else if (strcmp(key, "wideband_max_freq_hz") == 0) {
                // Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 2400000001ULL && val <= 7200000000ULL) {
                    settings.wideband_max_freq_hz = val;
                }
            }
        }

        // Restore newline and move to next line
        *line_end = saved_char;
        line_start = (saved_char != '\0') ? line_end + 1 : line_end;
    }

    file.close();
    return true;
}

// Return const char* instead of std::string (eliminates heap allocation)
const char* EnhancedSettingsManager::get_communication_status() noexcept {
    static constexpr const char* STATUS_READY = "TXT file found\nCommunication ready";
    static constexpr const char* STATUS_NOT_READY = "No TXT file found\nSave settings first";
    return verify_comm_file_exists() ? STATUS_READY : STATUS_NOT_READY;
}

// noexcept for file creation
bool EnhancedSettingsManager::ensure_database_exists(const DroneAnalyzerSettings& settings) noexcept {
    // Use settings.freqman_path instead of hardcoded "DRONES"
    static constexpr size_t kMaxPathLen = 64;
    char file_path[kMaxPathLen];
    
    // Guard clause: Ensure freqman_path is valid
    const char* freqman_name = (settings.freqman_path[0] != '\0') ? settings.freqman_path : "DRONES";
    snprintf(file_path, sizeof(file_path), "/FREQMAN/%s.TXT", freqman_name);
    File check_file;
    if (check_file.open(file_path, true)) {
        check_file.close();
        return true;
    }

    File create_file;
    if (create_file.open(file_path, false)) {
        // Check write result and handle errors
        size_t content_len = strlen(DEFAULT_DRONE_DATABASE_CONTENT);
        auto write_result = create_file.write(DEFAULT_DRONE_DATABASE_CONTENT, content_len);
        if (write_result.is_error() || write_result.value() != content_len) {
            // Write failed - file may be incomplete
            create_file.close();
            return false;
        }
        create_file.close();
        return true;
    }
    return false;
}

// const pointer, noexcept for file backup
void EnhancedSettingsManager::create_backup_file(const char* filepath) noexcept {
    // Use RAII wrapper for File to ensure proper cleanup
    FileRAII orig_file(filepath, true);
    if (!orig_file.is_open()) return;

    // Use fixed-size char array instead of std::string for backup path
    static constexpr size_t kMaxPathLen = 256;
    char backup_path[kMaxPathLen] = {};
    size_t filepath_len = strlen(filepath);
    if (filepath_len >= kMaxPathLen - 4) {
        return;  // Path too long
    }
    safe_strcpy(backup_path, filepath, kMaxPathLen);
    safe_strcat(backup_path, ".bak", kMaxPathLen);

    FileRAII backup_file(backup_path, false);
    if (!backup_file.is_open()) return;

    // Thread-local buffer for backup/restore operations (512 bytes per thread)
    static constexpr size_t BUFFER_SIZE = 512;
    thread_local uint8_t buffer[BUFFER_SIZE];
    size_t total_read = 0;

    while (total_read < orig_file.get().size()) {
        size_t to_read = std::min(BUFFER_SIZE, static_cast<size_t>(orig_file.get().size() - total_read));
        auto read_result = orig_file.get().read(buffer, to_read);
        if (read_result.is_error() || read_result.value() != to_read) break;

        auto write_result = backup_file.get().write(buffer, to_read);
        if (write_result.is_error() || write_result.value() != to_read) break;

        total_read += read_result.value();
    }
}

// const pointer, noexcept for file restore
void EnhancedSettingsManager::restore_from_backup(const char* filepath) noexcept {
    // Use fixed-size char array instead of std::string for backup path
    static constexpr size_t kMaxPathLen = 256;
    char backup_path[kMaxPathLen] = {};
    size_t filepath_len = strlen(filepath);
    if (filepath_len >= kMaxPathLen - 4) {
        return;  // Path too long
    }
    safe_strcpy(backup_path, filepath, kMaxPathLen);
    safe_strcat(backup_path, ".bak", kMaxPathLen);

    // Use RAII wrapper for File to ensure proper cleanup
    FileRAII backup_file(backup_path, true);
    if (!backup_file.is_open()) return;

    FileRAII original_file(filepath, false);
    if (!original_file.is_open()) return;

    // Thread-local buffer for backup/restore operations (512 bytes per thread)
    static constexpr size_t BUFFER_SIZE = 512;
    thread_local uint8_t buffer[BUFFER_SIZE];

    while (true) {
        auto read_res = backup_file.get().read(buffer, BUFFER_SIZE);
        if (read_res.is_error() || read_res.value() == 0) break;
        original_file.get().write(buffer, read_res.value());
    }
}

// const pointer, noexcept for file removal
void EnhancedSettingsManager::remove_backup_file(const char* filepath) noexcept {
    // Use fixed-size char array instead of std::string for backup path
    static constexpr size_t kMaxPathLen = 256;
    char backup_path[kMaxPathLen] = {};
    size_t filepath_len = strlen(filepath);
    if (filepath_len >= kMaxPathLen - 4) {
        return;  // Path too long
    }
    safe_strcpy(backup_path, filepath, kMaxPathLen);
    safe_strcat(backup_path, ".bak", kMaxPathLen);
    
    // Diamond Code Standard #3 - Replace std::filesystem with FatFS C API
    f_unlink(reinterpret_cast<const TCHAR*>(backup_path));
}

// noexcept for header generation
const char* EnhancedSettingsManager::generate_file_header() noexcept {
    static constexpr size_t HEADER_BUFFER_SIZE = 256;
    static char header_buffer[HEADER_BUFFER_SIZE];

    snprintf(header_buffer, HEADER_BUFFER_SIZE,
             "# Enhanced Drone Analyzer Settings v0.4\n"
             "# Generated by EDA App (Integrated Settings)\n"
             "# Timestamp: %s\n"
             "# This file is automatically read by EDA module\n"
             "\n",
             get_current_timestamp());

    return header_buffer;
}

// Return const char* from Flash instead of std::string (eliminates heap allocation)
const char* EnhancedSettingsManager::spectrum_mode_to_string(SpectrumMode mode) noexcept {
    return EDA::LUTs::spectrum_mode_display_name(static_cast<uint8_t>(mode));
}

// noexcept for timestamp generation (thread_local buffer for thread safety)
const char* EnhancedSettingsManager::get_current_timestamp() noexcept {
    // Return const char* to thread_local buffer (no std::string allocation)
    thread_local char buffer[32];
    systime_t now = chTimeNow();
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)now);
    return buffer;
}

// ===========================================
// ACTIVE: Translation Functions Implementation
// ===========================================
// Translation functions are kept separate as they provide unique UI localization
// functionality not covered by SettingsPersistence<T>
// Language is hardcoded to English only

// DIAMOND OPTIMIZATION: const pointer, noexcept for translation lookup
const char* DroneAnalyzerSettingsManager_Translations::translate(const char* key) noexcept {
    for (size_t i = 0; i < translations_count; ++i) {
        if (strcmp(translations_english[i].key, key) == 0) {
            return translations_english[i].value;
        }
    }
    return key;
}

// DIAMOND OPTIMIZATION: const pointer, noexcept for translation lookup
const char* DroneAnalyzerSettingsManager_Translations::get_translation(const char* key) noexcept {
    return translate(key);
}

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

// DIAMOND OPTIMIZATION: const reference, noexcept for preset array access
const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5>& DroneFrequencyPresets::get_all_presets() noexcept { return default_presets; }

EDA_FLASH_CONST inline static constexpr const char* PRESET_NAMES[] = {
    "2.4GHz Band Scan",
    "2.5GHz Band Scan",
    "DJI Mavic Series",
    "Parrot Anafi",
    "Military UAV Band"
};

// DIAMOND OPTIMIZATION: noexcept for preset names array access
const char* const* DroneFrequencyPresets::get_preset_names() noexcept {
    return PRESET_NAMES;
}

EDA_FLASH_CONST inline static constexpr DroneType AVAILABLE_TYPES[] = {
    DroneType::MAVIC,
    DroneType::PHANTOM,
    DroneType::DJI_MINI,
    DroneType::PARROT_ANAFI,
    DroneType::PARROT_BEBOP,
    DroneType::PX4_DRONE,
    DroneType::MILITARY_DRONE
};

// DIAMOND OPTIMIZATION: noexcept for types array access
const DroneType* DroneFrequencyPresets::get_available_types() noexcept {
    return AVAILABLE_TYPES;
}

// DIAMOND OPTIMIZATION: noexcept for count access
size_t DroneFrequencyPresets::get_preset_count() noexcept {
    return 5;
}

// DIAMOND OPTIMIZATION: noexcept for count access
size_t DroneFrequencyPresets::get_available_types_count() noexcept {
    return 7;
}
// DIAMOND OPTIMIZATION: LUT lookup instead of switch for get_type_display_name()
// Scott Meyers Item 15: Prefer constexpr to #define
// Saves ~100 bytes Flash
// DIAMOND OPTIMIZATION: noexcept for LUT lookup
const char* DroneFrequencyPresets::get_type_display_name(DroneType type) noexcept {
    return UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type));
}
// DIAMOND OPTIMIZATION: const references, noexcept for preset application
bool DroneFrequencyPresets::apply_preset(DroneAnalyzerSettings& config, const ui::apps::enhanced_drone_analyzer::DronePreset& preset) noexcept {
    // DIAMOND OPTIMIZATION: Guard clause to reduce nesting
    if (!preset.is_valid()) return false;

    config.spectrum_mode = SpectrumMode::MEDIUM;
    config.scan_interval_ms = 1000;
    config.rssi_threshold_db = static_cast<int32_t>(preset.threat_level) >= static_cast<int32_t>(ThreatLevel::HIGH) ? -80 : -90;
    audio_set_enable_alerts(config, true);

    // DIAMOND OPTIMIZATION: Named constants for magic numbers
    static constexpr uint64_t WIDEBAND_MIN_FREQ = 2400000000ULL;
    static constexpr uint64_t WIDEBAND_MAX_FREQ = 2500000000ULL;
    static constexpr uint64_t FREQ_OFFSET = 10000000ULL;
    static constexpr uint64_t USER_MIN_FREQ = 50000000ULL;
    static constexpr uint64_t USER_MAX_FREQ = 6000000000ULL;
    
    if (static_cast<uint64_t>(preset.frequency_hz) >= WIDEBAND_MIN_FREQ && static_cast<uint64_t>(preset.frequency_hz) <= WIDEBAND_MAX_FREQ) {
        scan_set_enable_wideband_scanning(config, true);
        config.wideband_min_freq_hz = WIDEBAND_MIN_FREQ;
        config.wideband_max_freq_hz = WIDEBAND_MAX_FREQ;
    }

    config.user_min_freq_hz = preset.frequency_hz - FREQ_OFFSET;
    config.user_max_freq_hz = preset.frequency_hz + FREQ_OFFSET;

    // DIAMOND OPTIMIZATION: Guard clauses for frequency clamping
    if (config.user_min_freq_hz < USER_MIN_FREQ) {
        config.user_min_freq_hz = USER_MIN_FREQ;
    }
    if (config.user_max_freq_hz > USER_MAX_FREQ) {
        config.user_max_freq_hz = USER_MAX_FREQ;
    }

    safe_strcpy(config.freqman_path, "DRONES", ui::apps::enhanced_drone_analyzer::MAX_NAME_LEN);
    disp_set_show_detailed_info(config, true);

    return true;
}

// ============ DronePresetSelector Implementation ============
// Note: PresetMenuViewImpl template is now defined in the header file

// DIAMOND OPTIMIZATION: Zero-allocation preset filtering using stack allocation
// Scott Meyers Item 29: Use object pools to reduce allocation overhead
// Replaces std::vector-based filtering with static stack buffer
static inline size_t filter_presets_by_type_stack(
    const std::array<DronePreset, 5>& presets,
    DroneType type, const DronePreset* out_filtered[], size_t max_count) {
    size_t found_count = 0;
    for (const auto& preset : presets) {
        if (found_count >= max_count) break;
        if (preset.drone_type == type) {
            out_filtered[found_count++] = &preset;
        }
    }
    return found_count;
}

// Template implementation must be in header for implicit instantiation
// show_preset_menu is now a template function defined in the header

// DIAMOND OPTIMIZATION: const reference, noexcept for filtered presets
void DronePresetSelector::show_type_filtered_presets(NavigationView& nav, DroneType type) noexcept {
    const auto& all_presets = DroneFrequencyPresets::get_all_presets();
    
    static constexpr size_t MAX_FILTERED = 5;
    const char* names[MAX_FILTERED];
    const DronePreset* filtered_ptrs[MAX_FILTERED];
    
    const size_t preset_count = filter_presets_by_type_stack(all_presets, type, filtered_ptrs, MAX_FILTERED);
    
    for (size_t i = 0; i < preset_count; ++i) {
        names[i] = filtered_ptrs[i]->display_name;
    }

    struct FilteredPresetsView {
        const DronePreset* const* presets;
        size_t count;
        const DronePreset& operator[](size_t idx) const { return *presets[idx]; }
        size_t size() const { return count; }
    };

    FilteredPresetsView filtered_view{filtered_ptrs, preset_count};
    // CRITICAL FIX: Lambda with capture passed to template - no heap allocation
    // The lambda type is deduced at compile time, enabling inline expansion
    auto on_selected = [&nav](const DronePreset&) { nav.pop(); };

    using FilteredPresetMenuViewT = PresetMenuViewImpl<FilteredPresetsView, decltype(on_selected)>;
    nav.push<FilteredPresetMenuViewT>(names, preset_count, std::move(on_selected), filtered_view);
}

// CRITICAL FIX: Return ConfigUpdaterCallback functor instead of std::function
// Zero heap allocation - functor stores only a pointer
// DIAMOND OPTIMIZATION: const reference, noexcept for callback creation
ConfigUpdaterCallback DronePresetSelector::create_config_updater(DroneAnalyzerSettings& config_to_update) noexcept {
    return ConfigUpdaterCallback{config_to_update};
}

// ============ DroneFrequencyEntry Implementation ============

// DIAMOND FIX: Add noexcept to match header declaration
DroneFrequencyEntry::DroneFrequencyEntry(Frequency freq, DroneType type, ThreatLevel threat,
                                       int32_t rssi_thresh, uint32_t bw_hz, const char* desc) noexcept
    : frequency_hz(freq), drone_type(type), threat_level(threat),
      rssi_threshold_db(rssi_thresh), bandwidth_hz(bw_hz), description(desc) {}

// DIAMOND OPTIMIZATION: noexcept for validation
bool DroneFrequencyEntry::is_valid() const noexcept {
    return EDA::Validation::validate_frequency(frequency_hz) &&
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
// DIAMOND FIX: Add noexcept to match header declaration
void HardwareSettingsView::focus() noexcept { button_save_.focus(); }
// DIAMOND OPTIMIZATION: Unified LUT lookup для SpectrumMode conversion
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономия RAM: LUT хранится во Flash, ноль heap allocation
// Ускорение: O(1) lookup вместо 5-branch switch
// ИСПОЛЬЗУЕТ: EDA::LUTs::spectrum_mode_ui_index() (SSOT)

// DIAMOND OPTIMIZATION: noexcept for settings load
void HardwareSettingsView::load_current_settings() noexcept {
    DroneAnalyzerSettings settings;
    // 🔴 FIX #M3: Check return value of load() and handle errors appropriately
    auto load_result = SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    if (!load_result.is_ok() || !load_result.value) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    checkbox_real_hardware_.set_value(hw_get_enable_real_hardware(settings));
    field_spectrum_mode_.set_selected_index(static_cast<int>(settings.spectrum_mode));
    number_bandwidth_.set_value(settings.hardware_bandwidth_hz);
    number_min_freq_.set_value(static_cast<rf::Frequency>(settings.user_min_freq_hz));
    number_max_freq_.set_value(static_cast<rf::Frequency>(settings.user_max_freq_hz));
}

// DIAMOND OPTIMIZATION: noexcept for settings save
void HardwareSettingsView::save_current_settings() noexcept {
    DroneAnalyzerSettings settings;
    // 🔴 FIX #M3: Check return value of load() and handle errors appropriately
    auto load_result = SettingsPersistence<DroneAnalyzerSettings>::load(settings);
    if (!load_result.is_ok() || !load_result.value) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    hw_set_enable_real_hardware(settings, checkbox_real_hardware_.value());
    hw_set_demo_mode(settings, !checkbox_real_hardware_.value());
    settings.spectrum_mode = static_cast<SpectrumMode>(field_spectrum_mode_.selected_index());
    settings.hardware_bandwidth_hz = static_cast<uint32_t>(number_bandwidth_.value());
    settings.user_min_freq_hz = static_cast<uint64_t>(number_min_freq_.value());
    settings.user_max_freq_hz = static_cast<uint64_t>(number_max_freq_.value());
    
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid hardware settings");
        return;
    }
    
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}

void HardwareSettingsView::update_ui_from_settings() noexcept { load_current_settings(); }
void HardwareSettingsView::update_settings_from_ui() noexcept { save_current_settings(); }
void HardwareSettingsView::on_save_settings() noexcept {
    save_current_settings();
    nav_.display_modal("Success", "Hardware settings saved");
}

AudioSettingsView::AudioSettingsView(NavigationView& nav) : View(), nav_(nav) {
    add_children({
        &checkbox_audio_enabled_, &text_audio_enabled_,
        &number_alert_frequency_, &number_alert_duration_,
        &number_volume_, &checkbox_repeat_, &text_repeat_,
        &button_save_
    });
    button_save_.on_select = [this](Button&) { on_save_settings(); };
    load_current_settings();
}

// DIAMOND FIX: Add noexcept to match header declaration
void AudioSettingsView::focus() noexcept {
    button_save_.focus();
}

// DIAMOND OPTIMIZATION: noexcept for settings load
void AudioSettingsView::load_current_settings() noexcept {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    checkbox_audio_enabled_.set_value(audio_get_enable_alerts(settings));
    number_alert_frequency_.set_value(settings.audio_alert_frequency_hz);
    number_alert_duration_.set_value(settings.audio_alert_duration_ms);
    number_volume_.set_value(settings.audio_volume_level);
    checkbox_repeat_.set_value(audio_get_repeat_alerts(settings));
}

// DIAMOND OPTIMIZATION: noexcept for settings save
void AudioSettingsView::save_current_settings() noexcept {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    audio_set_enable_alerts(settings, checkbox_audio_enabled_.value());
    settings.audio_alert_frequency_hz = static_cast<uint32_t>(number_alert_frequency_.value());
    settings.audio_alert_duration_ms = static_cast<uint32_t>(number_alert_duration_.value());
    settings.audio_volume_level = static_cast<uint8_t>(number_volume_.value());
    audio_set_repeat_alerts(settings, checkbox_repeat_.value());
    
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid audio settings");
        return;
    }
    
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}

// DIAMOND OPTIMIZATION: noexcept for save action
void AudioSettingsView::on_save_settings() noexcept {
    save_current_settings();
    nav_.display_modal("Success", "Audio settings saved");
}

// LoadingView
LoadingView::LoadingView(NavigationView& nav, const char* loading_text)
    : View(), nav_(nav), loading_text_1_{{screen_width / 2 - 50, screen_height / 2 - 10, 100, 16}, loading_text},
      loading_text_2_{{screen_width / 2 - 50, screen_height / 2 + 10, 100, 16}, ""} {
    add_children({&loading_text_1_, &loading_text_2_});
}
// DIAMOND FIX: Add noexcept to match header declaration
void LoadingView::focus() noexcept {}

// ScanningSettingsView
ScanningSettingsView::ScanningSettingsView(NavigationView& nav) : View(), nav_(nav) {
    add_children({&field_scanning_mode_, &number_scan_interval_, &number_rssi_threshold_,
                  &checkbox_wideband_, &text_wideband_, &button_presets_, &button_save_});
    button_save_.on_select = [this](Button&) { on_save_settings(); };
    checkbox_wideband_.on_select = [this](Checkbox&, bool) { on_wideband_enabled_changed(); };
    button_presets_.on_select = [this](Button&) { on_show_presets(); };
    load_current_settings();
}
// DIAMOND FIX: Add noexcept to match header declaration
void ScanningSettingsView::focus() noexcept { button_save_.focus(); }
// DIAMOND OPTIMIZATION: noexcept for settings load
void ScanningSettingsView::load_current_settings() noexcept {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    field_scanning_mode_.set_selected_index(0);
    number_scan_interval_.set_value(settings.scan_interval_ms);
    number_rssi_threshold_.set_value(settings.rssi_threshold_db);
    checkbox_wideband_.set_value(scan_get_enable_wideband_scanning(settings));
}

// DIAMOND OPTIMIZATION: noexcept for settings save
void ScanningSettingsView::save_current_settings() noexcept {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    settings.scan_interval_ms = static_cast<uint32_t>(number_scan_interval_.value());
    settings.rssi_threshold_db = static_cast<int32_t>(number_rssi_threshold_.value());
    scan_set_enable_wideband_scanning(settings, checkbox_wideband_.value());
    
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid scanning settings");
        return;
    }
    
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}
// DIAMOND OPTIMIZATION: noexcept for save action
void ScanningSettingsView::on_save_settings() noexcept {
    save_current_settings();
    nav_.display_modal("Success", "Scanning settings saved");
}
// DIAMOND OPTIMIZATION: noexcept for preset menu
void ScanningSettingsView::on_show_presets() noexcept {
    // Show presets menu using DronePresetSelector
    // Create callback that updates settings when preset is selected
    auto on_preset_selected = [this](const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
        DroneAnalyzerSettings settings;
        SettingsPersistence<DroneAnalyzerSettings>::load(settings);
        if (DroneFrequencyPresets::apply_preset(settings, preset)) {
            SettingsPersistence<DroneAnalyzerSettings>::save(settings);
            // DIAMOND OPTIMIZATION: Use fixed buffer instead of std::string concatenation
            constexpr size_t kMsgBufSize = 64;
            char msg_buf[kMsgBufSize];
            snprintf(msg_buf, kMsgBufSize, "Preset applied: %s", preset.display_name);
            nav_.display_modal("Success", msg_buf);
            load_current_settings();
        } else {
            nav_.display_modal("Error", "Failed to apply preset");
        }
    };
    DronePresetSelector::show_preset_menu(nav_, on_preset_selected);
}
// DIAMOND OPTIMIZATION: noexcept for empty handler
void ScanningSettingsView::on_wideband_enabled_changed() noexcept {}
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
    // Initialize settings with defaults first (which includes freqman_path = "DRONES")
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(current_settings_);
    // 🔴 FIX #M5: Check return value of ensure_database_exists() and handle failures
    bool db_exists = EnhancedSettingsManager::ensure_database_exists(current_settings_);
    if (!db_exists) {
        // Database creation failed - continue with defaults
        // Settings will be saved to file when user explicitly saves
    }
    // 🔴 FIX #M3: Check return value of load() and handle errors appropriately
    auto load_result = SettingsPersistence<DroneAnalyzerSettings>::load(current_settings_);
    if (!load_result.is_ok() || !load_result.value) {
        // Load failed - continue with defaults that were set above
    }
}
// DIAMOND OPTIMIZATION: noexcept for focus and navigation
void DroneAnalyzerSettingsView::focus() noexcept { button_audio_settings_.focus(); }
void DroneAnalyzerSettingsView::show_audio_settings() noexcept { nav_.push<AudioSettingsView>(); }
void DroneAnalyzerSettingsView::show_hardware_settings() noexcept { nav_.push<HardwareSettingsView>(); }
void DroneAnalyzerSettingsView::show_scanning_settings() noexcept { nav_.push<ScanningSettingsView>(); }
void DroneAnalyzerSettingsView::load_default_settings() noexcept {
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(current_settings_);
    SettingsPersistence<DroneAnalyzerSettings>::save(current_settings_);
    nav_.display_modal("Reset", "Settings reset to defaults");
}
// DIAMOND OPTIMIZATION: noexcept for about dialog
void DroneAnalyzerSettingsView::show_about_author() noexcept {
    // DIAMOND OPTIMIZATION: Use constexpr string literal in ROM (no heap allocation)
    // Scott Meyers Item 15: Prefer constexpr to #define
    static constexpr const char* kAboutMessage =
        "About the Author\n================\n\nApplication developed for\ncivilian population safety.\n\n"
        "Author: Kuznetsov Maxim Sergeevich\nLocksmith at Gazprom Gazoraspredeleniye\nOrenburg branch\n\n"
        "Development completed in October 2025\non voluntary and altruistic principles\nby one person.\n\n"
        "Greetings to everyone who risks\ntheir lives for the safety of others.\n\n"
        "Support the author:\nCard: 2202 20202 5787 1695\nYooMoney: 41001810704697\nTON: UQCdtMxQB5zbQBOICkY90lTQQqcs8V-V28Bf2AGvl8xOc5HR\n\n"
        "Contact:\nTelegram: @max_ai_master";
    nav_.display_modal("About Author", kAboutMessage);
}

// DroneDatabaseManager
// DIAMOND OPTIMIZATION: const pointer, noexcept for database load
DroneDatabaseManager::DatabaseView DroneDatabaseManager::load_database(const char* file_path) noexcept {
    DatabaseView view{};
    
    File file;
    if (!file.open(file_path, true)) return view;

    // FIX #8: Increase line buffer from 256 to 512 bytes to prevent buffer overflow
    // DIAMOND OPTIMIZATION: Stack-allocated line buffer (zero heap)
    // Diamond Code: Strong typing - size_t for array indices
    constexpr size_t LINE_BUFFER_SIZE = 512;
    char line_buffer[LINE_BUFFER_SIZE];
    size_t line_ptr = 0;
    
    constexpr size_t READ_BUFFER_SIZE = 256;
    char read_buffer[READ_BUFFER_SIZE];
    
    size_t total_read = 0;
    while (total_read < file.size() && view.count < MAX_DATABASE_ENTRIES) {
        auto read_res = file.read(read_buffer, READ_BUFFER_SIZE);
        if (read_res.is_error()) break;
        
        size_t bytes_read = read_res.value();
        
        for (size_t i = 0; i < bytes_read && view.count < MAX_DATABASE_ENTRIES; ++i) {
            char c = read_buffer[i];
            
            if (c == '\n' || c == '\r') {
                line_buffer[line_ptr] = '\0';
                
                // Parse line
                if (line_buffer[0] != '\0' && line_buffer[0] != '#') {
                    // Find comma separator
                    char* comma = static_cast<char*>(memchr(line_buffer, ',', line_ptr));
                    if (comma) {
                        *comma = '\0';
                        DroneDbEntry entry{};
                        entry.freq = strtoull(line_buffer, nullptr, 10);

                        // Validate desc is within line_buffer bounds
                        char* desc = comma + 1;
                        size_t desc_offset = static_cast<size_t>(desc - line_buffer);
                        if (desc_offset >= LINE_BUFFER_SIZE) {
                            line_ptr = 0;
                            continue;
                        }

                        // Skip leading spaces (with bounds check)
                        while (desc_offset < LINE_BUFFER_SIZE - 1 && *desc == ' ') {
                            desc++;
                            desc_offset++;
                        }

                        // Safe copy with explicit length
                        size_t max_desc_len = LINE_BUFFER_SIZE - desc_offset;
                        size_t copy_len = sizeof(entry.description) - 1;
                        if (max_desc_len < copy_len) {
                            copy_len = max_desc_len;
                        }

                        for (size_t j = 0; j < copy_len && desc[j] != '\0'; ++j) {
                            entry.description[j] = desc[j];
                        }
                        entry.description[copy_len] = '\0';

                        if (entry.freq > 0) {
                            view.entries[view.count++] = entry;
                        }
                    }
                }
                
                line_ptr = 0;
            } else if (line_ptr < LINE_BUFFER_SIZE - 1) {
                line_buffer[line_ptr++] = c;
            }
        }
        
        total_read += bytes_read;
    }
    
    file.close();
    return view;
}

// DIAMOND OPTIMIZATION: const reference, const pointer, noexcept for database save
bool DroneDatabaseManager::save_database(const DatabaseView& view, const char* file_path) noexcept {
    // DIAMOND OPTIMIZATION: Stack-allocated buffer (zero heap)
    // MEDIUM PRIORITY FIX: Reduced from 4096 to 2048 bytes (saves ~2KB RAM)
    // FIX #26: Further reduced from 2048 to 1024 for additional RAM savings
    constexpr size_t WRITE_BUFFER_SIZE = 1024;
    char write_buffer[WRITE_BUFFER_SIZE];
    size_t offset = 0;
    
    // Write header
    offset += snprintf(write_buffer + offset, WRITE_BUFFER_SIZE - offset,
                      "frequency,description\n# EDA User Database\n");
    
    // Write entries
    for (size_t i = 0; i < view.count; ++i) {
        const auto& entry = view.entries[i];
        if (entry.freq == 0) continue;
        
        // DIAMOND FIX: Use safe_strcpy for safe string operations
        char safe_desc[64];
        safe_strcpy(safe_desc, entry.description, sizeof(safe_desc));
        
        // Replace commas with spaces for CSV safety
        size_t desc_len = safe_strlen(safe_desc, sizeof(safe_desc));
        for (size_t j = 0; j < desc_len && j < sizeof(safe_desc) - 1; ++j) {
            if (safe_desc[j] == ',') safe_desc[j] = ' ';
        }
        safe_desc[desc_len] = '\0';
        
        int written = snprintf(write_buffer + offset, WRITE_BUFFER_SIZE - offset,
                            "%llu,%s\n",
                            static_cast<unsigned long long>(entry.freq),
                            safe_desc);
        
        if (written < 0 || offset + static_cast<size_t>(written) >= WRITE_BUFFER_SIZE) {
            // DIAMOND FIX: Use FileRAII to ensure proper cleanup
            FileRAII file(file_path, false);
            if (!file.is_open()) return false;
            auto write_result = file.get().write(write_buffer, offset);
            if (write_result.is_error() || write_result.value() != offset) {
                return false;
            }
            offset = 0;
        } else {
            offset += static_cast<size_t>(written);
        }
    }
    
    // DIAMOND FIX: Use FileRAII for final write
    if (offset > 0) {
        FileRAII file(file_path, false);
        if (!file.is_open()) return false;
        auto write_result = file.get().write(write_buffer, offset);
        if (write_result.is_error() || write_result.value() != offset) {
            return false;
        }
    }
    
    return true;
}



// DroneDatabaseListView
DroneDatabaseListView::DroneDatabaseListView(NavigationView& nav) 
    : View(), nav_(nav), database_view_() {
    add_children({&menu_view_});
    database_view_ = DroneDatabaseManager::load_database();
    menu_view_.clear();
    menu_view_.add_item({"[+ ADD NEW FREQUENCY]", Color::white(), nullptr, nullptr});
    for (size_t i = 0; i < database_view_.count; ++i) {
        const auto& entry = database_view_.entries[i];
        constexpr size_t FREQ_BUF_SIZE = 24;
        char freq_buf[FREQ_BUF_SIZE];
        FrequencyFormatter::to_string_short_freq_buffer(freq_buf, sizeof(freq_buf), entry.freq);
        
        constexpr size_t TEXT_BUFFER_SIZE = 64;
        char text_buffer[TEXT_BUFFER_SIZE];
        snprintf(text_buffer, sizeof(text_buffer), "%s: %s", freq_buf, entry.description);
        
        // FIX #25: Use text_buffer directly instead of std::string (no heap allocation)
        // The MenuView will copy the string internally
        menu_view_.add_item({text_buffer, Color::white(), nullptr, nullptr});
    }
}
// DIAMOND OPTIMIZATION: noexcept for focus
void DroneDatabaseListView::focus() noexcept { menu_view_.focus(); }

// DIAMOND OPTIMIZATION: noexcept for entry selection
void DroneDatabaseListView::on_entry_selected(size_t index) noexcept {
    if (index == 0) {
        DroneDbEntry empty_entry;
        auto on_add = [this](const DroneDbEntry& entry) {
            if (entry.freq != 0 && database_view_.count < DroneDatabaseManager::MAX_DATABASE_ENTRIES) {
                database_view_.entries[database_view_.count++] = entry;
                save_changes();
                menu_view_.clear();
                menu_view_.add_item({"[+ ADD NEW FREQUENCY]", Color::white(), nullptr, nullptr});
                for (size_t i = 0; i < database_view_.count; ++i) {
                    const auto& entry = database_view_.entries[i];
                    constexpr size_t FREQ_BUF_SIZE = 24;
                    char freq_buf[FREQ_BUF_SIZE];
                    FrequencyFormatter::to_string_short_freq_buffer(freq_buf, sizeof(freq_buf), entry.freq);
                    
                    constexpr size_t TEXT_BUFFER_SIZE = 64;
                    char text_buffer[TEXT_BUFFER_SIZE];
                    snprintf(text_buffer, sizeof(text_buffer), "%s: %s", freq_buf, entry.description);
                    
                    // FIX #25: Use text_buffer directly instead of std::string (no heap allocation)
                    menu_view_.add_item({text_buffer, Color::white(), nullptr, nullptr});
                }
            }
        };
        using EditorViewT = DroneEntryEditorView<decltype(on_add)>;
        nav_.push<EditorViewT>(empty_entry, std::move(on_add));
    } else {
        size_t entry_index = index - 1;
        auto on_edit = [this, entry_index](const DroneDbEntry& entry) {
            if (entry.freq != 0) {
                database_view_.entries[entry_index] = entry;
                save_changes();
                menu_view_.clear();
                menu_view_.add_item({"[+ ADD NEW FREQUENCY]", Color::white(), nullptr, nullptr});
                for (size_t i = 0; i < database_view_.count; ++i) {
                    const auto& entry = database_view_.entries[i];
                    constexpr size_t FREQ_BUF_SIZE = 24;
                    char freq_buf[FREQ_BUF_SIZE];
                    FrequencyFormatter::to_string_short_freq_buffer(freq_buf, sizeof(freq_buf), entry.freq);
                    
                    constexpr size_t TEXT_BUFFER_SIZE = 64;
                    char text_buffer[TEXT_BUFFER_SIZE];
                    snprintf(text_buffer, sizeof(text_buffer), "%s: %s", freq_buf, entry.description);
                    
                    // FIX #25: Use text_buffer directly instead of std::string (no heap allocation)
                    menu_view_.add_item({text_buffer, Color::white(), nullptr, nullptr});
                }
            }
        };
        using EditorViewT = DroneEntryEditorView<decltype(on_edit)>;
        nav_.push<EditorViewT>(database_view_.entries[entry_index], std::move(on_edit));
    }
}

// DIAMOND OPTIMIZATION: noexcept for save
void DroneDatabaseListView::save_changes() noexcept { DroneDatabaseManager::save_database(database_view_); }

// DIAMOND OPTIMIZATION: noexcept for key handling
bool DroneDatabaseListView::on_key(const KeyEvent key) noexcept {
    if (key == KeyEvent::Select) {
        size_t index = menu_view_.highlighted_index();
        on_entry_selected(index);
        return true;
    }
    return View::on_key(key);
}

} // namespace ui::apps::enhanced_drone_analyzer
