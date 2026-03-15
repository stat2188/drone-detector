// ui_enhanced_drone_settings.cpp - Unified implementation for Enhanced Drone Analyzer Settings
// The app tries to load the interface but doesn't crash in hard failure mode for 5 minutes.
//
// PHASE 5 MIGRATION - UI LAYER
// Migrated from LEGACY/ to main enhanced_drone_analyzer/ directory
//
// DIAMOND CODE COMPLIANCE:
// - Stack allocation only (max 4KB stack per thread on STM32F405)
// - Uses fixed-size buffers for all data storage
// - No heap allocation (no new, malloc, std::vector, std::map)
// - Uses constexpr, enum class, using Type = uintXX_t
// - No magic numbers (all constants defined)
// - Zero-Overhead and Data-Oriented Design principles
//
// FRAMEWORK EXCEPTIONS:
// - std::filesystem::path: Used only when required by PortaPack UI framework (FileLoadView callback)
// - std::string: Used only when required by PortaPack UI framework (FileLoadView callback)
// - std::function: Used only when required by PortaPack UI framework (event callbacks)
// - These are framework types that must be used when interacting with the PortaPack UI
//
// INTERNAL DATA STRUCTURES:
// - All internal data uses fixed-size buffers (char arrays, std::array)
// - Settings stored in DroneAnalyzerSettings struct (fixed-size)
// - No dynamic memory allocation for application data
//
// Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
// Environment: ChibiOS RTOS / PortaPack UI Framework

// Corresponding header (must be first)
#include "ui_enhanced_drone_settings.hpp"

// C++ standard library headers (alphabetical order)
#include <algorithm>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>  // For std::move (used in FileRAII move constructor)

// Third-party library headers
#include <ch.h>

// ChibiOS headers are included through <ch.h>
// No need to include internal headers directly

// Project-specific headers
#include "eda_optimized_utils.hpp"

// UI framework headers (must be included before UI types are used)
#include "ui.hpp"
#include "ui_navigation.hpp"

// File manager for database selection (FileLoadView)
#include "ui_fileman.hpp"

// Project-specific headers (alphabetical order)
#include "color_lookup_unified.hpp"
#include "eda_constants.hpp"
#include "eda_locking.hpp"
#include "eda_unified_database.hpp"
#include "file.hpp"
// Note: eda_database_parser.hpp is not used (only referenced in comments)
#include "settings_persistence.hpp"
#include "ui_drone_common_types.hpp"
#include "ui_widget.hpp"
// Note: portapack.hpp is not used in this file
// Note: string_format.hpp is not used in this file

// Diamond Code Standard #3 - Replace std::filesystem with FatFS C API

namespace ui::apps::enhanced_drone_analyzer {

// Settings Buffer Mutex Definition
// Mutex load_buffer_mutex is defined below (line 64)
//
// IMPORTANT LINKER DEPENDENCY:
// This mutex is defined here and declared as extern in settings_persistence.hpp.
// The linker will resolve this symbol at link time, so both files must be compiled
// and linked together for the firmware to build successfully.
//
// Purpose: Protects settings buffer during load/save operations
// Usage: Use SettingsBufferLock RAII wrapper for automatic locking
Mutex settings_buffer_mutex;

// Load Buffer Mutex Definition
// Protects the static load buffer from concurrent access
// Corresponding declaration in settings_persistence.hpp
Mutex load_buffer_mutex;

// Header Buffer Mutex Definition
// CRITICAL FIX #5: Protects static header_buffer from concurrent access
// If generate_file_header() is called from multiple threads, the static
// header_buffer will be corrupted. This mutex prevents that.
//
// Thread Safety: generate_file_header() uses a static buffer that is
// shared across all function calls. Without mutex protection, concurrent
// calls from multiple threads will corrupt the buffer.
//
// Lock Order: Must use SD_CARD_MUTEX order (highest level) since
// this function is called during SD card I/O operations.
Mutex header_buffer_mutex;

// DIAMOND FIX #HIGH #4: Static member buffer definitions for EnhancedSettingsManager
// Replaces thread_local buffers to prevent initialization order issues
char EnhancedSettingsManager::settings_buffer_[EnhancedSettingsManager::SETTINGS_BUFFER_SIZE];
char EnhancedSettingsManager::file_buffer_[EnhancedSettingsManager::FILE_BUFFER_SIZE];
char EnhancedSettingsManager::timestamp_buffer_[EnhancedSettingsManager::TIMESTAMP_BUFFER_SIZE];
uint8_t EnhancedSettingsManager::backup_buffer_[EnhancedSettingsManager::BACKUP_BUFFER_SIZE];
char EnhancedSettingsManager::backup_path_[EnhancedSettingsManager::BACKUP_PATH_SIZE];

// ============================================================================
// STACK USAGE VALIDATION
// ============================================================================
// Embedded systems have limited stack space (4KB per thread on STM32F405).
// These static_assert statements validate stack usage at compile time to prevent
// stack overflow at runtime, which is difficult to debug.
// ============================================================================

// Validate settings buffer sizes are safe for stack allocation
// Stack buffers should be kept small (<1KB) to prevent stack overflow
static constexpr size_t SETTINGS_BUFFER_SIZE = 512;
static constexpr size_t FILE_BUFFER_SIZE = 512;
static constexpr size_t kMaxPathLen = 256;
static constexpr size_t HEADER_BUFFER_SIZE = 256;

static_assert(SETTINGS_BUFFER_SIZE <= 1024,
              "SETTINGS_BUFFER_SIZE exceeds 1KB safe stack buffer limit");
static_assert(SETTINGS_BUFFER_SIZE >= 256,
              "SETTINGS_BUFFER_SIZE below 256 bytes minimum for safe operation");
static_assert(FILE_BUFFER_SIZE <= 1024,
              "FILE_BUFFER_SIZE exceeds 1KB safe stack buffer limit");
static_assert(FILE_BUFFER_SIZE >= 256,
              "FILE_BUFFER_SIZE below 256 bytes minimum for safe operation");
static_assert(kMaxPathLen <= 512,
              "kMaxPathLen exceeds 512 bytes safe stack buffer limit");
static_assert(kMaxPathLen >= 64,
              "kMaxPathLen below 64 bytes minimum for safe operation");
static_assert(HEADER_BUFFER_SIZE <= 512,
              "HEADER_BUFFER_SIZE exceeds 512 bytes safe stack buffer limit");
static_assert(HEADER_BUFFER_SIZE >= 128,
              "HEADER_BUFFER_SIZE below 128 bytes minimum for safe operation");

// RAII wrapper for File (ensures proper cleanup)
class FileRAII {
public:
    explicit FileRAII(const char* path, bool read_only = false)
        : file_(), opened_(false) {
        // Guard clause: null pointer check for path parameter
        if (!path) {
            return;
        }
        // Explicitly construct std::filesystem::path using const TCHAR* constructor
        // to avoid template constructor that tries to use std::begin()/std::end()
        // on const char*, which doesn't work for C strings
        const std::filesystem::path fs_path{reinterpret_cast<const TCHAR*>(path)};
        opened_ = file_.open(fs_path, read_only);
    }

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
    // DIAMOND FIX #HIGH #4: Use static member buffer instead of thread_local
    // Acquire mutex for thread safety (shared static buffer)
    MutexLock lock(settings_buffer_mutex);
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    File file;

    // Open file for writing
    // CRITICAL FIX #10: Error Handling for SD Card I/O
    // Validates: SD card availability, file path validity, write permissions
    // Fallback: Returns false on failure, caller handles restore from backup
    // Explicitly construct std::filesystem::path using const TCHAR* constructor
    // to avoid template constructor that tries to use std::begin()/std::end()
    // on const char*, which doesn't work for C strings
    const std::filesystem::path fs_path{reinterpret_cast<const TCHAR*>(filepath)};
    if (!file.open(fs_path, false)) {
        // Error: SD card not available, file path invalid, or write permission denied
        // Recovery: Caller will restore from backup (see line 180)
        return false;
    }
    
    size_t offset = 0;

    // Write header with timestamp
    const char* header = generate_file_header();
    size_t header_len = strlen(header);
    auto header_result = file.write(static_cast<const void*>(header),
                                        static_cast<File::Size>(header_len));
    if (header_result.is_error() || header_result.value() != header_len) {
        file.close();
        // Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    // Generate settings content using snprintf (no heap allocations)
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "spectrum_mode=%s\n", spectrum_mode_to_string(settings.spectrum_mode));
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "scan_interval_ms=%u\n", (unsigned int)settings.scan_interval_ms);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "rssi_threshold_db=%d\n", (int)settings.rssi_threshold_db);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_audio_alerts=%s\n", audio_get_enable_alerts(settings) ? "true" : "false");
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_frequency_hz=%u\n", (unsigned int)settings.audio_alert_frequency_hz);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_duration_ms=%u\n", (unsigned int)settings.audio_alert_duration_ms);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "hardware_bandwidth_hz=%u\n", (unsigned int)settings.hardware_bandwidth_hz);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_real_hardware=%s\n", hw_get_enable_real_hardware(settings) ? "true" : "false");
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "demo_mode=%s\n", hw_get_demo_mode(settings) ? "true" : "false");
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "freqman_path=%s\n", settings.freqman_path);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_min_freq_hz=%llu\n", (unsigned long long)settings.user_min_freq_hz);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_max_freq_hz=%llu\n", (unsigned long long)settings.user_max_freq_hz);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_slice_width_hz=%u\n", (unsigned int)settings.wideband_slice_width_hz);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "panoramic_mode_enabled=%s\n", scan_get_panoramic_mode_enabled(settings) ? "true" : "false");
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_min_freq_hz=%llu\n", (unsigned long long)settings.wideband_min_freq_hz);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_max_freq_hz=%llu\n", (unsigned long long)settings.wideband_max_freq_hz);
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "settings_version=0.4\n");
    offset += snprintf(settings_buffer_ + offset, SETTINGS_BUFFER_SIZE - offset,
                      "last_modified_timestamp=%u\n", (unsigned int)chTimeNow());

    // Write settings content
    auto content_result = file.write(static_cast<const void*>(settings_buffer_),
                                          static_cast<File::Size>(offset));
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
    // Explicitly construct std::filesystem::path using const TCHAR* constructor
    // to avoid template constructor that tries to use std::begin()/std::end()
    // on const char*, which doesn't work for C strings
    const std::filesystem::path fs_path{reinterpret_cast<const TCHAR*>(filepath)};
    if (txt_file.open(fs_path, true)) {  // read_only
        txt_file.close();
        return true;
    }
    return false;
}

    // const reference to prevent unnecessary copies
bool EnhancedSettingsManager::load_settings_from_txt(DroneAnalyzerSettings& settings) noexcept {
    // DIAMOND FIX #HIGH #4: Use static member buffer instead of thread_local
    // Acquire mutex for thread safety (shared static buffer)
    MutexLock lock(settings_buffer_mutex);
    static constexpr const char* filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    File file;

    // Open file for reading
    // CRITICAL FIX #10: Error Handling for SD Card I/O
    // Validates: SD card availability, file path validity, read permissions
    // Fallback: Returns false on failure, caller handles error gracefully
    // Explicitly construct std::filesystem::path using const TCHAR* constructor
    // to avoid template constructor that tries to use std::begin()/std::end()
    // on const char*, which doesn't work for C strings
    const std::filesystem::path fs_path{reinterpret_cast<const TCHAR*>(filepath)};
    if (!file.open(fs_path, true)) {
        // Error: SD card not available, file path invalid, or read permission denied
        // Recovery: Caller handles false return gracefully
        return false;
    }

    auto read_result = file.read(file_buffer_, FILE_BUFFER_SIZE);
    
    // CRITICAL FIX #10: Error Handling for Read Operations
    // Validates: Read success, non-empty buffer, data integrity
    // Fallback: Returns false on failure, caller handles error gracefully
    if (read_result.is_error() || read_result.value() == 0) {
        // Error: Read operation failed, empty file, or SD card removed
        // Recovery: Caller handles false return gracefully
        file.close();
        return false;
    }

    // File Truncation Warning - if file is larger than buffer, data is truncated
    const size_t bytes_read = read_result.value();
    if (bytes_read < FILE_BUFFER_SIZE) {
        file_buffer_[bytes_read] = '\0';
    } else {
        // Buffer was completely filled, ensure null-termination
        file_buffer_[FILE_BUFFER_SIZE - 1] = '\0';
        // DIAMOND FIX: File truncated - error logging
        // In production, this should log to a persistent error log
        // For now, we note the truncation but continue processing
        // WARNING: Settings file exceeds buffer size, data may be lost
    }

    // FIX #18: Use non-destructive parsing (replaces strtok)
    // strtok modifies buffer, use manual parsing instead
    char* line_start = file_buffer_;
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
                // Use consolidated spectrum mode display names from eda_constants.hpp
                if (strcmp(value, EDA::LUTs::spectrum_mode_display_name(0)) == 0) settings.spectrum_mode = SpectrumMode::ULTRA_NARROW;
                else if (strcmp(value, EDA::LUTs::spectrum_mode_display_name(1)) == 0) settings.spectrum_mode = SpectrumMode::NARROW;
                else if (strcmp(value, EDA::LUTs::spectrum_mode_display_name(2)) == 0) settings.spectrum_mode = SpectrumMode::MEDIUM;
                else if (strcmp(value, EDA::LUTs::spectrum_mode_display_name(3)) == 0) settings.spectrum_mode = SpectrumMode::WIDE;
                else if (strcmp(value, EDA::LUTs::spectrum_mode_display_name(4)) == 0) settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
            } else if (strcmp(key, "scan_interval_ms") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 0 &&
                    static_cast<uint32_t>(val) >= EDA::Constants::MIN_SCAN_INTERVAL_MS &&
                    static_cast<uint32_t>(val) <= EDA::Constants::MAX_SCAN_INTERVAL_MS) {
                    settings.scan_interval_ms = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "rssi_threshold_db") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= EDA::Constants::RSSI_INVALID_DBM && val <= EDA::Constants::MAX_VALID_RSSI) {
                    settings.rssi_threshold_db = static_cast<int32_t>(val);
                }
            } else if (strcmp(key, "enable_audio_alerts") == 0) {
                audio_set_enable_alerts(settings, strcmp(value, "true") == 0);
            } else if (strcmp(key, "audio_alert_frequency_hz") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 0 &&
                    static_cast<uint32_t>(val) >= EDA::Constants::MIN_AUDIO_FREQ &&
                    static_cast<uint32_t>(val) <= EDA::Constants::MAX_AUDIO_FREQ) {
                    settings.audio_alert_frequency_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "audio_alert_duration_ms") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 0 &&
                    static_cast<uint32_t>(val) >= EDA::Constants::MIN_AUDIO_DURATION &&
                    static_cast<uint32_t>(val) <= EDA::Constants::MAX_AUDIO_DURATION) {
                    settings.audio_alert_duration_ms = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
                // Replace atoi with strtol and check for errors
                char* endptr = nullptr;
                long val = strtol(value, &endptr, 10);
                if (endptr != value && *endptr == '\0' && val >= 0 &&
                    static_cast<uint32_t>(val) >= EDA::Constants::MIN_BANDWIDTH &&
                    static_cast<uint32_t>(val) <= EDA::Constants::MAX_BANDWIDTH) {
                    settings.hardware_bandwidth_hz = static_cast<uint32_t>(val);
                }
            } else if (strcmp(key, "enable_real_hardware") == 0) {
                hw_set_enable_real_hardware(settings, strcmp(value, "true") == 0);
            } else if (strcmp(key, "demo_mode") == 0) {
                hw_set_demo_mode(settings, strcmp(value, "true") == 0);
            } else if (strcmp(key, "freqman_path") == 0) {
                snprintf(settings.freqman_path, sizeof(settings.freqman_path), "%s", value);
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

// Helper function to build backup path (appends ".bak" to filepath)
// Returns true if successful, false if path too long
// Thread-safe: Must be called with settings_buffer_mutex held
bool EnhancedSettingsManager::build_backup_path(const char* filepath) noexcept {
    if (!filepath) {
        return false;
    }
    size_t filepath_len = strlen(filepath);
    if (filepath_len >= BACKUP_PATH_SIZE - 4) {
        return false;  // Path too long
    }
    snprintf(backup_path_, BACKUP_PATH_SIZE, "%s", filepath);
    size_t current_len = strlen(backup_path_);
    snprintf(backup_path_ + current_len, BACKUP_PATH_SIZE - current_len, "%s", ".bak");
    return true;
}

// const pointer, noexcept for file restore
void EnhancedSettingsManager::restore_from_backup(const char* filepath) noexcept {
    // Guard clause: null pointer check for filepath parameter
    if (!filepath) {
        return;
    }

    // Build backup path using helper function
    if (!build_backup_path(filepath)) {
        return;  // Path too long
    }

    // Use RAII wrapper for File to ensure proper cleanup
    FileRAII backup_file(backup_path_, true);
    if (!backup_file.is_open()) return;

    FileRAII original_file(filepath, false);
    if (!original_file.is_open()) return;

    // DIAMOND FIX #HIGH #4: Use static member buffer instead of thread_local
    // Thread-safe: Caller must hold settings_buffer_mutex

    while (true) {
        auto read_res = backup_file.get().read(backup_buffer_, BACKUP_BUFFER_SIZE);
        if (read_res.is_error() || read_res.value() == 0) break;
        original_file.get().write(static_cast<const void*>(backup_buffer_),
                                   static_cast<File::Size>(read_res.value()));
    }
}

// const pointer, noexcept for file removal
void EnhancedSettingsManager::remove_backup_file(const char* filepath) noexcept {
    // Guard clause: null pointer check for filepath parameter
    if (!filepath) {
        return;
    }

    // Build backup path using helper function
    if (!build_backup_path(filepath)) {
        return;  // Path too long
    }

    // Diamond Code Standard #3 - Replace std::filesystem with FatFS C API
    f_unlink(reinterpret_cast<const TCHAR*>(backup_path_));
}

// noexcept for header generation
// CRITICAL FIX #5: Uses header_buffer_mutex to protect static buffer
// from concurrent access. Without this mutex, multiple threads calling
// this function simultaneously will corrupt the static header_buffer.
const char* EnhancedSettingsManager::generate_file_header() noexcept {
    static constexpr size_t HEADER_BUFFER_SIZE = 256;
    static char header_buffer[HEADER_BUFFER_SIZE];

    // Protect static header_buffer from concurrent access
    MutexLock lock(header_buffer_mutex, LockOrder::SD_CARD_MUTEX);

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

// DIAMOND FIX #HIGH #4: Use static member buffer instead of thread_local
// Note: Caller (generate_file_header/save_settings_to_txt) already holds settings_buffer_mutex
const char* EnhancedSettingsManager::get_current_timestamp() noexcept {
    systime_t now = chTimeNow();
    snprintf(timestamp_buffer_, TIMESTAMP_BUFFER_SIZE, "%lu", (unsigned long)now);
    return timestamp_buffer_;
}

// Translation Functions Implementation
// Translation functions are kept separate as they provide unique UI localization
// functionality not covered by SettingsPersistence<T>
// Language is hardcoded to English only
// DIAMOND OPTIMIZATION: Use consolidated Translator from ui_drone_common_types.cpp

// DIAMOND OPTIMIZATION: const pointer, noexcept for translation lookup
const char* DroneAnalyzerSettingsManager_Translations::translate(const char* key) noexcept {
    return Translator::translate(key);
}

// DIAMOND OPTIMIZATION: const pointer, noexcept for translation lookup
const char* DroneAnalyzerSettingsManager_Translations::get_translation(const char* key) noexcept {
    return Translator::get_translation(key);
}

// DroneFrequencyPresets Implementation

// OPTIMIZATION: static const array instead of vector to avoid heap allocation
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

    snprintf(config.freqman_path, EDA::Constants::MAX_NAME_LENGTH, "%s", "DRONES");
    disp_set_show_detailed_info(config, true);

    return true;
}

// DronePresetSelector Implementation

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

// DroneFrequencyEntry Implementation

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

// UI IMPLEMENTATIONS

// HardwareSettingsView
HardwareSettingsView::HardwareSettingsView(NavigationView& nav) : nav_(nav) {
    add_children({&checkbox_real_hardware_, &text_real_hardware_, &field_spectrum_mode_,
                  &number_bandwidth_, &number_min_freq_, &number_max_freq_, &button_save_});
    load_current_settings();
}
// DIAMOND FIX: Add noexcept to match header declaration
void HardwareSettingsView::focus() noexcept { button_save_.focus(); }
// DIAMOND OPTIMIZATION: Unified LUT lookup  SpectrumMode conversion
// Scott Meyers Item 15: Prefer constexpr to #define
// RAM: LUT   Flash,  heap allocation
// : O(1) lookup  5-branch switch
// : EDA::LUTs::spectrum_mode_ui_index() (SSOT)

// DIAMOND OPTIMIZATION: noexcept for settings load
void HardwareSettingsView::load_current_settings() noexcept {
    static DroneAnalyzerSettings settings; // DIAMOND FIX: moved from stack to .bss
    // FIX #M3: Check return value of load() and handle errors appropriately
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
    static DroneAnalyzerSettings settings; // DIAMOND FIX: moved from stack to .bss
    // FIX #M3: Check return value of load() and handle errors appropriately
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
    static DroneAnalyzerSettings settings; // DIAMOND FIX: moved from stack to .bss
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
    static DroneAnalyzerSettings settings; // DIAMOND FIX: moved from stack to .bss
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
    // MODIFICATION: Added button_load_database_ to children list
    add_children({&field_scanning_mode_, &number_scan_interval_, &number_rssi_threshold_,
                  &checkbox_wideband_, &text_wideband_, &button_load_database_,
                  &button_presets_, &button_save_});
    button_save_.on_select = [this](Button&) { on_save_settings(); };
    checkbox_wideband_.on_select = [this](Checkbox&, bool) { on_wideband_enabled_changed(); };
    button_presets_.on_select = [this](Button&) { on_show_presets(); };

    // MODIFICATION: Added database selection handler
    button_load_database_.on_select = [this](Button&) { on_select_database(); };

    load_current_settings();
}
// DIAMOND FIX: Add noexcept to match header declaration
void ScanningSettingsView::focus() noexcept { button_save_.focus(); }
// DIAMOND OPTIMIZATION: noexcept for settings load
void ScanningSettingsView::load_current_settings() noexcept {
    static DroneAnalyzerSettings settings; // DIAMOND FIX: moved from stack to .bss
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
    static DroneAnalyzerSettings settings; // DIAMOND FIX: moved from stack to .bss
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
        static DroneAnalyzerSettings settings; // DIAMOND FIX: moved from stack to .bss
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

// MODIFICATION: New method for database file selection
void ScanningSettingsView::on_select_database() noexcept {
    // Launch FileLoadView with .TXT filter for frequency database files
    auto open_view = nav_.push<FileLoadView>(".TXT");

    // Set callback to handle file selection when user picks a file
    open_view->on_changed = [this](std::filesystem::path new_file_path) {
        // FIX #5: Use fixed-size char buffer instead of std::string
        // Eliminates heap allocation (~100 bytes)
        
        constexpr size_t MAX_FILENAME_LENGTH = EDA::Constants::MAX_NAME_LENGTH;
        char filename[MAX_FILENAME_LENGTH];
        
        // Extract filename without extension using manual parsing
        // Get full path string
        const std::string& path_str = new_file_path.string();
        const char* path_cstr = path_str.c_str();
        
        // Find last directory separator
        const char* last_slash = nullptr;
        for (const char* p = path_cstr; *p != '\0'; p++) {
            if (*p == '/' || *p == '\\') {
                last_slash = p;
            }
        }
        
        // Find last dot (extension separator) after last separator
        const char* last_dot = nullptr;
        const char* start_pos = (last_slash != nullptr) ? last_slash + 1 : path_cstr;
        for (const char* p = start_pos; *p != '\0'; p++) {
            if (*p == '.') {
                last_dot = p;
            }
        }
        
        // Find start of filename (after last separator)
        const char* filename_start = (last_slash != nullptr) ? last_slash + 1 : path_cstr;
        
        // Extract filename without extension
        size_t filename_len = 0;
        const char* end_pos = (last_dot != nullptr) ? last_dot : path_cstr + path_str.length();
        for (const char* p = filename_start; p < end_pos && filename_len < MAX_FILENAME_LENGTH - 1; p++) {
            filename[filename_len++] = *p;
        }
        filename[filename_len] = '\0';
        
        // Validate filename length to prevent buffer overflow
        if (filename_len >= MAX_FILENAME_LENGTH) {
            filename[MAX_FILENAME_LENGTH - 1] = '\0';
        }
        
        // Load current settings from persistence
        static DroneAnalyzerSettings settings;
        if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
            nav_.display_modal("Error", "Failed to load settings");
            return;
        }

        // Update freqman_path with selected filename
        snprintf(settings.freqman_path, sizeof(settings.freqman_path), "%s", filename);

        // Save updated settings to persistent storage
        if (!SettingsPersistence<DroneAnalyzerSettings>::save(settings)) {
            nav_.display_modal("Error", "Failed to save settings");
            return;
        }

        // Display success message with selected filename
        char msg_buf[64];
        snprintf(msg_buf, sizeof(msg_buf), "Database: %s", filename);
        nav_.display_modal("Success", msg_buf);
    };

    // Start file browser in FREQMAN directory (standard Mayhem database location)
    // Explicitly construct std::filesystem::path using const TCHAR* constructor
    // to avoid template constructor that tries to use std::begin()/std::end()
    // on const char*, which doesn't work for C strings
    open_view->push_dir(std::filesystem::path(reinterpret_cast<const TCHAR*>("/FREQMAN")));
}

// DroneAnalyzerSettingsView
DroneAnalyzerSettingsView::DroneAnalyzerSettingsView(NavigationView& nav) : View(), nav_(nav), current_settings_{} {
    add_children({&text_title_, &button_audio_settings_, &button_hardware_settings_, &button_scanning_settings_,
                  &button_load_defaults_, &button_about_author_});
    button_audio_settings_.on_select = [this](Button&) { show_audio_settings(); };
    button_hardware_settings_.on_select = [this](Button&) { show_hardware_settings(); };
    button_scanning_settings_.on_select = [this](Button&) { show_scanning_settings(); };
    button_load_defaults_.on_select = [this](Button&) { load_default_settings(); };
    button_about_author_.on_select = [this](Button&) { show_about_author(); };
    // Initialize settings with defaults first (which includes freqman_path = "DRONES")
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(current_settings_);
    // FIX #M3: Check return value of load() and handle errors appropriately
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
// Stage 4: Updated to use UnifiedDroneDatabase
// DIAMOND OPTIMIZATION: const pointer, noexcept for database load
DroneDatabaseManager::DatabaseView DroneDatabaseManager::load_database(const char* file_path) noexcept {
    DatabaseView view{};
    
    // Use UnifiedDroneDatabase for loading
    auto& db = UnifiedDroneDatabase::instance();
    
    // Initialize and load from file
    if (!db.initialize()) {
        return view;
    }
    
    if (!db.load(file_path)) {
        // Return empty view if load fails
        return view;
    }
    
    // Convert UnifiedDroneDatabase entries to DatabaseView format
    size_t db_count = db.size();
    size_t count = 0;
    
    for (size_t i = 0; i < db_count && count < MAX_DATABASE_ENTRIES; ++i) {
        const UnifiedDroneEntry* entry = db.get_entry(i);
        if (entry == nullptr || entry->frequency_hz == 0) continue;
        
        view.entries[count].freq = entry->frequency_hz;
        
        // Copy description safely (max 31 chars + null terminator)
        size_t desc_len = 0;
        while (desc_len < 31 && entry->description[desc_len] != '\0') {
            view.entries[count].description[desc_len] = entry->description[desc_len];
            desc_len++;
        }
        view.entries[count].description[desc_len] = '\0';
        
        count++;
    }
    
    view.count = count;
    return view;
}

// DIAMOND OPTIMIZATION: const reference, const pointer, noexcept for database save
// Stage 4: Updated to use UnifiedDroneDatabase with validation
bool DroneDatabaseManager::save_database(const DatabaseView& view, const char* file_path) noexcept {
    auto& db = UnifiedDroneDatabase::instance();
    
    // Clear database for fresh import
    db.clear();
    
    // Validate and add entries
    for (size_t i = 0; i < view.count; ++i) {
        const auto& entry = view.entries[i];
        if (entry.freq == 0) continue;
        
        // Create UnifiedDroneEntry for validation
        UnifiedDroneEntry unified_entry;
        unified_entry.frequency_hz = entry.freq;
        
        // Copy description safely (max 31 chars + null terminator for UnifiedDroneEntry)
        size_t desc_len = 0;
        while (desc_len < 31 && entry.description[desc_len] != '\0') {
            unified_entry.description[desc_len] = entry.description[desc_len];
            desc_len++;
        }
        unified_entry.description[desc_len] = '\0';
        
        // Validate entry before adding
        auto validation = FrequencyValidation::validate_entry(unified_entry);
        if (!validation.valid) {
            // Skip invalid entries but continue processing
            continue;
        }

        // Add to unified database
        (void)db.add_entry(unified_entry);
    }
    
    // Save to file
    bool success = db.save(file_path);
    
    // Observers are automatically notified by save()
    return success;
}

// DroneDatabaseListView
// Static member definitions (required for ODR compliance)
DroneDatabaseManager::DatabaseView DroneDatabaseListView::g_database_view{};
Mutex DroneDatabaseListView::g_database_mutex{};
// BUG #1 FIX: Initialize g_database_mutex - must be called AFTER chSysInit()
// The mutex is value-initialized above but needs explicit chMtxInit() call
// This initialization is done in initialize_eda_mutexes() in scanning_coordinator.cpp
// CRITICAL FIX: Use AtomicFlag for thread-safe flag access with acquire/release semantics
// Access is protected by g_database_mutex for consistency with g_database_view
// AtomicFlag provides acquire/release memory ordering and lock-free operations
AtomicFlag DroneDatabaseListView::g_database_loaded;

DroneDatabaseListView::DroneDatabaseListView(NavigationView& nav)
    : View(), nav_(nav) {
    add_children({&menu_view_});
    // Lock mutex before accessing static member
    MutexLock lock(g_database_mutex, LockOrder::DATA_MUTEX);
    // Lazy initialization: only load database once
    // CRITICAL FIX: Use AtomicFlag::load() with acquire semantics for thread-safe check
    if (!g_database_loaded.load()) {
        g_database_view = DroneDatabaseManager::load_database();
        // CRITICAL FIX: Use AtomicFlag::store() with release semantics for thread-safe set
        g_database_loaded.store(true);
    }
    menu_view_.clear();
    menu_view_.add_item({"[+ ADD NEW FREQUENCY]", Color::white(), nullptr, nullptr});
    for (size_t i = 0; i < g_database_view.count; ++i) {
        const auto& entry = g_database_view.entries[i];
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
            MutexLock lock(g_database_mutex, LockOrder::DATA_MUTEX);
            if (entry.freq != 0 && g_database_view.count < DroneDatabaseManager::MAX_DATABASE_ENTRIES) {
                g_database_view.entries[g_database_view.count++] = entry;
                save_changes_locked();  // Use unlocked version since we already hold the mutex
                menu_view_.clear();
                menu_view_.add_item({"[+ ADD NEW FREQUENCY]", Color::white(), nullptr, nullptr});
                for (size_t i = 0; i < g_database_view.count; ++i) {
                    const auto& entry = g_database_view.entries[i];
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
            MutexLock lock(g_database_mutex, LockOrder::DATA_MUTEX);
            if (entry.freq != 0) {
                g_database_view.entries[entry_index] = entry;
                save_changes_locked();  // Use unlocked version since we already hold the mutex
                menu_view_.clear();
                menu_view_.add_item({"[+ ADD NEW FREQUENCY]", Color::white(), nullptr, nullptr});
                for (size_t i = 0; i < g_database_view.count; ++i) {
                    const auto& entry = g_database_view.entries[i];
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
        nav_.push<EditorViewT>(g_database_view.entries[entry_index], std::move(on_edit));
    }
}

// DIAMOND OPTIMIZATION: noexcept for save
// BUG #1 FIX: Implementation of initialize_mutex() function
// This function must be called AFTER chSysInit() to prevent undefined behavior
// Called from initialize_eda_mutexes() in scanning_coordinator.cpp
void DroneDatabaseListView::initialize_mutex() noexcept {
    chMtxInit(&g_database_mutex);
}

// Note: Assumes g_database_mutex is already held by caller
// This prevents deadlock when called from on_entry_selected() which already holds the lock
void DroneDatabaseListView::save_changes_locked() noexcept {
    (void)DroneDatabaseManager::save_database(g_database_view);
}

// Public wrapper that acquires the lock for external callers
void DroneDatabaseListView::save_changes() noexcept {
    MutexLock lock(g_database_mutex, LockOrder::DATA_MUTEX);
    save_changes_locked();
}

// DIAMOND OPTIMIZATION: noexcept for key handling
bool DroneDatabaseListView::on_key(const KeyEvent key) noexcept {
    if (key == KeyEvent::Select) {
        size_t index = menu_view_.highlighted_index();
        on_entry_selected(index);
        return true;
    }
    return View::on_key(key);
}

// =============================================================================
// DISABLED: Duplicate DroneDatabaseParser class
// This class has been replaced by the unified DatabaseParser namespace in
// eda_database_parser.hpp. The new implementation provides:
//   - DatabaseParser::parse_freqman_line() for freqman format
//   - DatabaseParser::parse_csv_line() for CSV format
//   - DatabaseParser::parse_line() for auto-detect format
//   - DatabaseParser::write_freqman_line() for writing freqman format
//   - DatabaseParser::write_csv_line() for writing CSV format
// =============================================================================
#if 0
// CSV Parser for Drone Database - Extracted for separation of concerns
namespace {
// DIAMOND OPTIMIZATION: POD struct for parse result (zero heap allocation)
struct ParseResult {
    DroneDbEntry entry{};
    bool success = false;
};

// DIAMOND OPTIMIZATION: Static parser class with zero heap allocation
class DroneDatabaseParser {
public:
    // DIAMOND OPTIMIZATION: Static method with noexcept for performance
    // Parses a single CSV line and returns ParseResult
    // Format: "frequency,description" or "# comment"
    static ParseResult parse_line(const char* line, size_t line_length) noexcept {
        ParseResult result{};
        
        // Handle empty lines
        if (line_length == 0 || line[0] == '\0') {
            return result;
        }
        
        // Handle comment lines (lines starting with #)
        if (line[0] == '#') {
            return result;
        }
        
        // Find comma separator
        const char* comma = static_cast<const char*>(memchr(line, ',', line_length));
        if (!comma) {
            return result;
        }
        
        // Calculate frequency string length
        size_t freq_length = static_cast<size_t>(comma - line);
        if (freq_length == 0) {
            return result;
        }
        
        // Parse frequency
        char freq_str[64];
        size_t copy_len = (freq_length < sizeof(freq_str) - 1) ? freq_length : sizeof(freq_str) - 1;
        for (size_t i = 0; i < copy_len; ++i) {
            freq_str[i] = line[i];
        }
        freq_str[copy_len] = '\0';
        
        result.entry.freq = strtoull(freq_str, nullptr, 10);
        
        // Calculate description pointer and length
        const char* desc = comma + 1;
        size_t desc_offset = static_cast<size_t>(desc - line);
        size_t desc_length = line_length - desc_offset;
        
        // Skip leading spaces
        while (desc_length > 0 && *desc == ' ') {
            desc++;
            desc_length--;
        }
        
        // Safe copy description with bounds checking
        size_t max_desc_len = sizeof(result.entry.description) - 1;
        size_t actual_copy_len = (desc_length < max_desc_len) ? desc_length : max_desc_len;
        
        for (size_t j = 0; j < actual_copy_len && desc[j] != '\0'; ++j) {
            result.entry.description[j] = desc[j];
        }
        result.entry.description[actual_copy_len] = '\0';
        
        // Mark as successful only if frequency is valid
        result.success = (result.entry.freq > 0);
        
        return result;
    }
};
} // anonymous namespace
#endif // DISABLED: Duplicate DroneDatabaseParser - use DatabaseParser from eda_database_parser.hpp

} // namespace ui::apps::enhanced_drone_analyzer
