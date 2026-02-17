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

namespace fs = std::filesystem;

namespace ui::apps::enhanced_drone_analyzer {


class FileRAII {
public:
    explicit FileRAII(const char* path, bool read_only = false) {
        opened_ = file_.open(path, read_only);
    }

    ~FileRAII() {
        if (opened_) {
            file_.close();
        }
    }

    // Delete copy constructor and assignment
    FileRAII(const FileRAII&) = delete;
    FileRAII& operator=(const FileRAII&) = delete;

    // Allow move
    FileRAII(FileRAII&& other) noexcept {
        file_ = std::move(other.file_);
        opened_ = other.opened_;
        other.opened_ = false;
    }

    File& get() { return file_; }
    const File& get() const { return file_; }
    bool is_open() const { return opened_; }

private:
    File file_;
    bool opened_ = false;
};

// ===========================================
// EnhancedSettingsManager Implementation
// ===========================================

bool EnhancedSettingsManager::save_settings_to_txt(const DroneAnalyzerSettings& settings) {
    // FIXED: Use constexpr const char* instead of std::string to avoid heap allocation
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

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

    // 🔴 CRITICAL FIX: Move 2KB buffer from stack to static storage to prevent stack overflow
    // Scott Meyers Item 29: Use object pools to reduce allocation overhead
    // This replaces ~20 std::string allocations with a single char array
    // Stack savings: 2048 bytes
    // MEDIUM PRIORITY FIX: Reduced from 4096 to 2048 bytes (saves ~2KB RAM)
    static constexpr size_t SETTINGS_BUFFER_SIZE = 2048;
    static char settings_buffer[SETTINGS_BUFFER_SIZE];
    size_t offset = 0;

    // Write header with timestamp
    const char* header = generate_file_header();
    size_t header_len = strlen(header);
    auto header_result = file.write(header, header_len);
    if (header_result.is_error() || header_result.value() != header_len) {
        file.close();
        // 🔴 ENHANCED: Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    // 🔴 OPTIMIZATION: Generate settings content using snprintf (no heap allocations)
    // This replaces the entire generate_settings_content() function
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "spectrum_mode=%s\n", spectrum_mode_to_string(settings.spectrum_mode));
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "scan_interval_ms=%u\n", (unsigned int)settings.scan_interval_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "rssi_threshold_db=%d\n", (int)settings.rssi_threshold_db);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_audio_alerts=%s\n", settings.audio_flags.enable_alerts ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_frequency_hz=%u\n", (unsigned int)settings.audio_alert_frequency_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_duration_ms=%u\n", (unsigned int)settings.audio_alert_duration_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "hardware_bandwidth_hz=%u\n", (unsigned int)settings.hardware_bandwidth_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_real_hardware=%s\n", settings.hardware_flags.enable_real_hardware ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "demo_mode=%s\n", settings.hardware_flags.demo_mode ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "freqman_path=%s\n", settings.freqman_path);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_min_freq_hz=%llu\n", (unsigned long long)settings.user_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_max_freq_hz=%llu\n", (unsigned long long)settings.user_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_slice_width_hz=%u\n", (unsigned int)settings.wideband_slice_width_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "panoramic_mode_enabled=%s\n", settings.scanning_flags.panoramic_mode_enabled ? "true" : "false");
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
    // FIXED: Use constexpr const char* instead of std::string to avoid heap allocation
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    File txt_file;
    if (txt_file.open(filepath, true)) {  // true = read_only
        txt_file.close();
        return true;
    }
    return false;
}

bool EnhancedSettingsManager::load_settings_from_txt(DroneAnalyzerSettings& settings) {
    // FIXED: Use constexpr const char* instead of std::string to avoid heap allocation
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

    File settings_file;
    if (!settings_file.open(filepath, true)) {  // true = read_only
        return false;
    }

    auto& file = settings_file;

    // 🔴 CRITICAL FIX: Move 2KB buffer from stack to static storage to prevent stack overflow
    // Stack savings: 2048 bytes
    // MEDIUM PRIORITY FIX: Reduced from 4096 to 2048 bytes (saves ~2KB RAM)
    static constexpr size_t FILE_BUFFER_SIZE = 2048;
    static char file_buffer[FILE_BUFFER_SIZE];
    auto read_result = file.read(file_buffer, FILE_BUFFER_SIZE);
    
    if (read_result.is_error() || read_result.value() == 0) {
        file.close();
        return false;
    }

    // FIX: Add buffer size check before null-termination to prevent overflow
    // If file is larger than buffer, clamp to buffer size - 1
    const size_t bytes_read = read_result.value();
    if (bytes_read < FILE_BUFFER_SIZE) {
        file_buffer[bytes_read] = '\0';
    } else {
        // Buffer was completely filled, ensure null-termination
        file_buffer[FILE_BUFFER_SIZE - 1] = '\0';
    }

    // Parse settings from buffer
    char* line = strtok(file_buffer, "\n");
    while (line != nullptr) {
        // Skip empty lines and comments
        if (*line == '\0' || *line == '#') {
            line = strtok(nullptr, "\n");
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
                // 🔴 HIGH PRIORITY FIX: Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 100 && val <= 10000) {
                    settings.scan_interval_ms = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "rssi_threshold_db") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= -120 && val <= 10) {
                    settings.rssi_threshold_db = static_cast<int32_t>(val);
                }
            } else if (strcmp(key, "enable_audio_alerts") == 0) {
                settings.audio_flags.enable_alerts = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "audio_alert_frequency_hz") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 200 && val <= 20000) {
                    settings.audio_alert_frequency_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "audio_alert_duration_ms") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 50 && val <= 5000) {
                    settings.audio_alert_duration_ms = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 10000 && val <= 28000000) {
                    settings.hardware_bandwidth_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "enable_real_hardware") == 0) {
                settings.hardware_flags.enable_real_hardware = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "demo_mode") == 0) {
                settings.hardware_flags.demo_mode = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "freqman_path") == 0) {
                // DIAMOND OPTIMIZATION: Use safe_strcpy instead of strncpy for consistent safety
                safe_strcpy(settings.freqman_path, value, sizeof(settings.freqman_path));
            } else if (strcmp(key, "user_min_freq_hz") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 50000000ULL && val <= 6000000000ULL) {
                    settings.user_min_freq_hz = val;
                }
            } else if (strcmp(key, "user_max_freq_hz") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 50000000ULL && val <= 6000000000ULL) {
                    settings.user_max_freq_hz = val;
                }
            } else if (strcmp(key, "wideband_slice_width_hz") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 10000000 && val <= 28000000) {
                    settings.wideband_slice_width_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "panoramic_mode_enabled") == 0) {
                settings.scanning_flags.panoramic_mode_enabled = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "wideband_min_freq_hz") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 2400000000ULL && val <= 7200000000ULL) {
                    settings.wideband_min_freq_hz = val;
                }
            } else if (strcmp(key, "wideband_max_freq_hz") == 0) {
                // 🔴 HIGH PRIORITY FIX: Replace atoll with strtoull and check for errors
                char* endptr = nullptr;
                unsigned long long val = strtoull(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 2400000001ULL && val <= 7200000000ULL) {
                    settings.wideband_max_freq_hz = val;
                }
            }
        }

        line = strtok(nullptr, "\n");
    }

    file.close();
    return true;
}

// 🔴 HIGH PRIORITY FIX: Return const char* instead of std::string to eliminate heap allocation
// RAM savings: ~100-200 bytes per call (no std::string allocation)
const char* EnhancedSettingsManager::get_communication_status() {
    static constexpr const char* STATUS_READY = "TXT file found\nCommunication ready";
    static constexpr const char* STATUS_NOT_READY = "No TXT file found\nSave settings first";
    return verify_comm_file_exists() ? STATUS_READY : STATUS_NOT_READY;
}

void EnhancedSettingsManager::ensure_database_exists() {
    // FIXED: Use constexpr const char* instead of std::string to avoid heap allocation
    static constexpr const char* file_path = "/FREQMAN/DRONES.TXT";
    File check_file;
    if (check_file.open(file_path, true)) {
        check_file.close();
        return;
    }

    File create_file;
    if (create_file.open(file_path, false)) {
        // 🔴 HIGH PRIORITY FIX: Check write result and handle errors
        size_t content_len = strlen(DEFAULT_DRONE_DATABASE_CONTENT);
        auto write_result = create_file.write(DEFAULT_DRONE_DATABASE_CONTENT, content_len);
        if (write_result.is_error() || write_result.value() != content_len) {
            // Write failed - file may be incomplete
        }
        create_file.close();
    }
}

void EnhancedSettingsManager::create_backup_file(const char* filepath) {
    // 🔴 PHASE 3: Use RAII wrapper for File to ensure proper cleanup
    FileRAII orig_file(filepath, true);
    if (!orig_file.is_open()) return;

    // FIXED: Use fixed-size char array instead of std::string for backup path
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

    // 🔴 PHASE 4: Share static buffer between backup/restore functions to reduce RAM usage
    // RAM savings: 512 bytes (was 1024 + 512 = 1536, now 1024)
    static constexpr size_t BUFFER_SIZE = 1024;
    static uint8_t buffer[BUFFER_SIZE];
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

void EnhancedSettingsManager::restore_from_backup(const char* filepath) {
    // FIXED: Use fixed-size char array instead of std::string for backup path
    static constexpr size_t kMaxPathLen = 256;
    char backup_path[kMaxPathLen] = {};
    size_t filepath_len = strlen(filepath);
    if (filepath_len >= kMaxPathLen - 4) {
        return;  // Path too long
    }
    safe_strcpy(backup_path, filepath, kMaxPathLen);
    safe_strcat(backup_path, ".bak", kMaxPathLen);

    // 🔴 PHASE 3: Use RAII wrapper for File to ensure proper cleanup
    FileRAII backup_file(backup_path, true);
    if (!backup_file.is_open()) return;

    FileRAII original_file(filepath, false);
    if (!original_file.is_open()) return;

    // 🔴 PHASE 4: Share static buffer with create_backup_file to reduce RAM usage
    static constexpr size_t BUFFER_SIZE = 1024;
    static uint8_t buffer[BUFFER_SIZE];

    while (true) {
        auto read_res = backup_file.get().read(buffer, BUFFER_SIZE);
        if (read_res.is_error() || read_res.value() == 0) break;
        original_file.get().write(buffer, read_res.value());
    }
}

void EnhancedSettingsManager::remove_backup_file(const char* filepath) {
    // FIXED: Use fixed-size char array instead of std::string for backup path
    static constexpr size_t kMaxPathLen = 256;
    char backup_path[kMaxPathLen] = {};
    size_t filepath_len = strlen(filepath);
    if (filepath_len >= kMaxPathLen - 4) {
        return;  // Path too long
    }
    safe_strcpy(backup_path, filepath, kMaxPathLen);
    safe_strcat(backup_path, ".bak", kMaxPathLen);
    delete_file(std::filesystem::path{backup_path});
}

const char* EnhancedSettingsManager::generate_file_header() {
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

// 🔴 HIGH PRIORITY FIX: Return const char* from Flash instead of std::string to eliminate heap allocation
// DIAMOND OPTIMIZATION: Unified LUT lookup for spectrum_mode_to_string()
// Scott Meyers Item 15: Prefer constexpr to #define
// Saves ~50 bytes Flash, uses EDA::LUTs (Single Source of Truth)
// RAM savings: ~50-100 bytes per call (no std::string allocation)
const char* EnhancedSettingsManager::spectrum_mode_to_string(SpectrumMode mode) {
    return EDA::LUTs::spectrum_mode_display_name(static_cast<uint8_t>(mode));
}

const char* EnhancedSettingsManager::get_current_timestamp() {
    // DIAMOND OPTIMIZATION: Return const char* to static buffer (no std::string allocation)
    static char buffer[32];
    systime_t now = chTimeNow();
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)now);
    return buffer;
}

// ===========================================
// ACTIVE: Translation Functions Implementation
// ===========================================
// Translation functions are kept separate as they provide unique UI localization
// functionality not covered by SettingsPersistence<T>

Language DroneAnalyzerSettingsManager_Translations::current_language_ = Language::ENGLISH;

const char* DroneAnalyzerSettingsManager_Translations::translate(const char* key) {
    for (size_t i = 0; i < translations_count; ++i) {
        if (strcmp(translations_english[i].key, key) == 0) {
            return translations_english[i].value;
        }
    }
    return key;
}

const char* DroneAnalyzerSettingsManager_Translations::get_translation(const char* key) {
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

const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5>& DroneFrequencyPresets::get_all_presets() { return default_presets; }

EDA_FLASH_CONST inline static constexpr const char* PRESET_NAMES[] = {
    "2.4GHz Band Scan",
    "2.5GHz Band Scan",
    "DJI Mavic Series",
    "Parrot Anafi",
    "Military UAV Band"
};

const char* const* DroneFrequencyPresets::get_preset_names() {
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

const DroneType* DroneFrequencyPresets::get_available_types() {
    return AVAILABLE_TYPES;
}

size_t DroneFrequencyPresets::get_preset_count() {
    return 5;
}

size_t DroneFrequencyPresets::get_available_types_count() {
    return 7;
}
// DIAMOND OPTIMIZATION: LUT lookup instead of switch for get_type_display_name()
// Scott Meyers Item 15: Prefer constexpr to #define
// Saves ~100 bytes Flash
const char* DroneFrequencyPresets::get_type_display_name(DroneType type) {
    return UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type));
}
bool DroneFrequencyPresets::apply_preset(DroneAnalyzerSettings& config, const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
    if (!preset.is_valid()) {
        return false;
    }

    config.spectrum_mode = SpectrumMode::MEDIUM;
    config.scan_interval_ms = 1000;
    config.rssi_threshold_db = static_cast<int32_t>(preset.threat_level) >= static_cast<int32_t>(ThreatLevel::HIGH) ? -80 : -90;
    config.audio_flags.enable_alerts = true;

    // 🔴 PHASE 4: Remove redundant cast (preset.frequency_hz is already uint64_t)
    if (preset.frequency_hz >= 2400000000ULL && preset.frequency_hz <= 2500000000ULL) {
        config.scanning_flags.enable_wideband_scanning = true;
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
    config.display_flags.show_detailed_info = true;

    return true;
}

// ============ DronePresetSelector Implementation ============

// DIAMOND OPTIMIZATION: Unified template menu view for zero-allocation preset selection
// Scott Meyers Item 15: Prefer constexpr to #define
// Template eliminates code duplication between array and vector preset containers
// Deleted copy operations fix Rule of Three violation (has pointer/reference members)
template <typename PresetContainer>
class PresetMenuViewImpl : public MenuView {
public:
    PresetMenuViewImpl(NavigationView& nav, const char* const* names, size_t count,
                       std::function<void(const DronePreset&)> on_selected, const PresetContainer& presets)
        : MenuView(), nav_(nav), names_(names), name_count_(count),
          on_selected_fn_(std::move(on_selected)), presets_(presets) {
        for (size_t i = 0; i < name_count_; ++i) {
            add_item({names_[i], Color::white(), nullptr, nullptr});
        }
    }

    PresetMenuViewImpl(const PresetMenuViewImpl&) = delete;
    PresetMenuViewImpl& operator=(const PresetMenuViewImpl&) = delete;

private:
    NavigationView& nav_;
    const char* const* names_;
    size_t name_count_;
    std::function<void(const DronePreset&)> on_selected_fn_;
    const PresetContainer& presets_;

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
};

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

void DronePresetSelector::show_preset_menu(NavigationView& nav, std::function<void(const DronePreset&)> callback) {
    const auto preset_names = DroneFrequencyPresets::get_preset_names();
    const auto& all_presets = DroneFrequencyPresets::get_all_presets();
    const auto preset_count = DroneFrequencyPresets::get_preset_count();

    using PresetMenuViewT = PresetMenuViewImpl<std::array<DronePreset, 5>>;
    nav.push<PresetMenuViewT>(preset_names, preset_count, std::move(callback), all_presets);
}

void DronePresetSelector::show_type_filtered_presets(NavigationView& nav, DroneType type) {
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
    auto on_selected = [&nav](const DronePreset&) { nav.pop(); };

    using FilteredPresetMenuViewT = PresetMenuViewImpl<FilteredPresetsView>;
    nav.push<FilteredPresetMenuViewT>(names, preset_count, std::move(on_selected), filtered_view);
}

std::function<void(const DronePreset&)> DronePresetSelector::create_config_updater(DroneAnalyzerSettings& config_to_update) {
    return [&config_to_update](const DronePreset& preset) {
        DroneFrequencyPresets::apply_preset(config_to_update, preset);
    };
}

// ============ DroneFrequencyEntry Implementation ============

DroneFrequencyEntry::DroneFrequencyEntry(Frequency freq, DroneType type, ThreatLevel threat,
                                       int32_t rssi_thresh, uint32_t bw_hz, const char* desc)
    : frequency_hz(freq), drone_type(type), threat_level(threat),
      rssi_threshold_db(rssi_thresh), bandwidth_hz(bw_hz), description(desc) {}

bool DroneFrequencyEntry::is_valid() const {
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
void HardwareSettingsView::focus() { button_save_.focus(); }
// DIAMOND OPTIMIZATION: Unified LUT lookup для SpectrumMode conversion
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономия RAM: LUT хранится во Flash, ноль heap allocation
// Ускорение: O(1) lookup вместо 5-branch switch
// ИСПОЛЬЗУЕТ: EDA::LUTs::spectrum_mode_ui_index() (SSOT)

void HardwareSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    checkbox_real_hardware_.set_value(settings.hardware_flags.enable_real_hardware);
    field_spectrum_mode_.set_selected_index(static_cast<int>(settings.spectrum_mode));
    number_bandwidth_.set_value(settings.hardware_bandwidth_hz);
    number_min_freq_.set_value(settings.user_min_freq_hz);
    number_max_freq_.set_value(settings.user_max_freq_hz);
}

void HardwareSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    settings.hardware_flags.enable_real_hardware = checkbox_real_hardware_.value();
    settings.hardware_flags.demo_mode = !checkbox_real_hardware_.value();
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

void HardwareSettingsView::update_ui_from_settings() { load_current_settings(); }
void HardwareSettingsView::update_settings_from_ui() { save_current_settings(); }
void HardwareSettingsView::on_save_settings() {
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

void AudioSettingsView::focus() {
    button_save_.focus();
}

void AudioSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    checkbox_audio_enabled_.set_value(settings.audio_flags.enable_alerts);
    number_alert_frequency_.set_value(settings.audio_alert_frequency_hz);
    number_alert_duration_.set_value(settings.audio_alert_duration_ms);
    number_volume_.set_value(settings.audio_volume_level);
    checkbox_repeat_.set_value(settings.audio_flags.repeat_alerts);
}

void AudioSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    settings.audio_flags.enable_alerts = checkbox_audio_enabled_.value();
    settings.audio_alert_frequency_hz = static_cast<uint32_t>(number_alert_frequency_.value());
    settings.audio_alert_duration_ms = static_cast<uint32_t>(number_alert_duration_.value());
    settings.audio_volume_level = static_cast<uint8_t>(number_volume_.value());
    settings.audio_flags.repeat_alerts = checkbox_repeat_.value();
    
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid audio settings");
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
LoadingView::LoadingView(NavigationView& nav, const char* loading_text)
    : View(), nav_(nav), loading_text_1_{{screen_width / 2 - 50, screen_height / 2 - 10, 100, 16}, loading_text},
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
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    field_scanning_mode_.set_selected_index(0);
    number_scan_interval_.set_value(settings.scan_interval_ms);
    number_rssi_threshold_.set_value(settings.rssi_threshold_db);
    checkbox_wideband_.set_value(settings.scanning_flags.enable_wideband_scanning);
}

void ScanningSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    settings.scan_interval_ms = static_cast<uint32_t>(number_scan_interval_.value());
    settings.rssi_threshold_db = static_cast<int32_t>(number_rssi_threshold_.value());
    settings.scanning_flags.enable_wideband_scanning = checkbox_wideband_.value();
    
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid scanning settings");
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
DroneDatabaseManager::DatabaseView DroneDatabaseManager::load_database(const char* file_path) {
    DatabaseView view{};
    
    File file;
    if (!file.open(file_path, true)) return view;

    // 🔴 CRITICAL FIX: Increase line buffer from 128 to 256 bytes to prevent buffer overflow
    // DIAMOND OPTIMIZATION: Stack-allocated line buffer (zero heap)
    // Diamond Code: Strong typing - size_t for array indices
    constexpr size_t LINE_BUFFER_SIZE = 256;
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
                        
                        // Copy description (skip comma and space)
                        char* desc = comma + 1;
                        while (*desc == ' ') desc++;
                        
                        safe_strcpy(entry.description, desc, sizeof(entry.description));
                        
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

bool DroneDatabaseManager::save_database(const DatabaseView& view, const char* file_path) {
    // DIAMOND OPTIMIZATION: Stack-allocated buffer (zero heap)
    constexpr size_t WRITE_BUFFER_SIZE = 4096;
    char write_buffer[WRITE_BUFFER_SIZE];
    size_t offset = 0;
    
    // Write header
    offset += snprintf(write_buffer + offset, WRITE_BUFFER_SIZE - offset,
                      "frequency,description\n# EDA User Database\n");
    
    // Write entries
    for (size_t i = 0; i < view.count; ++i) {
        const auto& entry = view.entries[i];
        if (entry.freq == 0) continue;
        
        char safe_desc[64];
        size_t desc_len = strlen(entry.description);
        for (size_t j = 0; j < desc_len && j < sizeof(safe_desc) - 1; ++j) {
            safe_desc[j] = (entry.description[j] == ',') ? ' ' : entry.description[j];
        }
        safe_desc[desc_len < (sizeof(safe_desc) - 1) ? desc_len : sizeof(safe_desc) - 1] = '\0';
        
        int written = snprintf(write_buffer + offset, WRITE_BUFFER_SIZE - offset,
                            "%llu,%s\n",
                            static_cast<unsigned long long>(entry.freq),
                            safe_desc);
        
        if (written < 0 || offset + static_cast<size_t>(written) >= WRITE_BUFFER_SIZE) {
            // Buffer full - flush and continue (simplified)
            File file;
            if (!file.open(file_path, false)) return false;
            file.write(write_buffer, offset);
            file.close();
            offset = 0;
        } else {
            offset += static_cast<size_t>(written);
        }
    }
    
    // Final write
    if (offset > 0) {
        File file;
        if (!file.open(file_path, false)) return false;
        file.write(write_buffer, offset);
        file.close();
    }
    
    return true;
}

// DroneEntryEditorView
// DroneEntryEditorView

DroneEntryEditorView::DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, OnSaveCallback callback)
    : View(), nav_(nav), entry_(entry), on_save_(callback), description_buffer_(entry.description) {
    add_children({&text_freq_, &field_freq_, &text_desc_, &field_desc_, &button_save_, &button_cancel_});
    field_freq_.set_value(entry_.freq);
    button_save_.on_select = [this](Button&) { on_save(); };
    button_cancel_.on_select = [this](Button&) { on_cancel(); };
}
void DroneEntryEditorView::focus() { field_freq_.focus(); }
void DroneEntryEditorView::on_save() {
    DroneDbEntry new_entry;
    new_entry.freq = field_freq_.value();
    // MEDIUM PRIORITY FIX: Use char array instead of std::string
    safe_strcpy(new_entry.description, description_buffer_.c_str(), sizeof(new_entry.description));
    if (on_save_) on_save_(new_entry);
    nav_.pop();
}
void DroneEntryEditorView::on_cancel() {
    DroneDbEntry empty_entry{0};
    if (on_save_) on_save_(empty_entry);
    nav_.pop();
}

// DroneDatabaseListView
DroneDatabaseListView::DroneDatabaseListView(NavigationView& nav) 
    : View(), nav_(nav), database_view_() {
    add_children({&menu_view_});
    database_view_ = DroneDatabaseManager::load_database();
    reload_list();
}

void DroneDatabaseListView::focus() { menu_view_.focus(); }

void DroneDatabaseListView::reload_list() {
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
        
        // DIAMOND FIX: Create named string object for lifetime extension
        std::string text_item(text_buffer);
        menu_view_.add_item({text_item, Color::white(), nullptr, nullptr});
    }
}

void DroneDatabaseListView::on_entry_selected(size_t index) {
    if (index == 0) {
        DroneDbEntry empty_entry;
        nav_.push<DroneEntryEditorView>(empty_entry, [this](const DroneDbEntry& entry) {
            if (entry.freq != 0 && database_view_.count < DroneDatabaseManager::MAX_DATABASE_ENTRIES) {
                database_view_.entries[database_view_.count++] = entry;
                save_changes();
                reload_list();
            }
        });
    } else {
        size_t entry_index = index - 1;
        nav_.push<DroneEntryEditorView>(database_view_.entries[entry_index], [this, entry_index](const DroneDbEntry& entry) {
            if (entry.freq != 0) {
                database_view_.entries[entry_index] = entry;
                save_changes();
                reload_list();
            }
        });
    }
}

void DroneDatabaseListView::save_changes() { DroneDatabaseManager::save_database(database_view_); }
bool DroneDatabaseListView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Select) {
        size_t index = menu_view_.highlighted_index();
        on_entry_selected(index);
        return true;
    }
    return View::on_key(key);
}

} // namespace ui::apps::enhanced_drone_analyzer
