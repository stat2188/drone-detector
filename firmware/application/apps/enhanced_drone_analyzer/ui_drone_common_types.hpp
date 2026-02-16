#ifndef UI_DRONE_COMMON_TYPES_HPP_
#define UI_DRONE_COMMON_TYPES_HPP_

#include <cstdint>
#include <cstring>
#include "eda_constants.hpp"
#include "radio.hpp"
#include "file.hpp"

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// SpectrumMode forward declare
enum class SpectrumMode : uint8_t { NARROW = 0, MEDIUM = 1, WIDE = 2, ULTRA_WIDE = 3, ULTRA_NARROW = 4 };

// ===========================================
// CONSTANTS & TYPE ALIASES
// ===========================================
static constexpr size_t MAX_PATH_LEN = 64;
static constexpr size_t MAX_NAME_LEN = 32;
static constexpr size_t MAX_FORMAT_LEN = 8;

// Helper for safe string assignment (Zero-Heap, no null-padding overhead)
static inline void safe_strcpy(char* dest, const char* src, size_t max_len) {
    if (!dest || !src || max_len == 0) return;
    
    size_t i = 0;
    while (i < max_len - 1 && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

// Helper for safe string concatenation (Zero-Heap, no null-padding overhead)
static inline void safe_strcat(char* dest, const char* src, size_t max_len) {
    if (!dest || !src || max_len == 0) return;
    
    char* d = dest;
    const char* s = src;
    
    while (*d && static_cast<size_t>(d - dest) < max_len) ++d;
    
    size_t remaining = max_len - static_cast<size_t>(d - dest);
    if (remaining == 0) return;
    
    while (--remaining && *s) {
        *d++ = *s++;
    }
    *d = '\0';
}

// ===========================================
// DIAMOND FIX: PACKED STRUCTS TO MINIMIZE SIZE
// ===========================================
// Eliminates padding waste (~30-50 bytes saved)
#pragma pack(push, 1)

struct DroneAnalyzerSettings {
    // ===== AUDIO SETTINGS =====
    struct AudioFlags {
        bool enable_alerts : 1;
        bool repeat_alerts : 1;
        uint8_t reserved : 6;
    } audio_flags = {true, false, 0};

    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;

    // ===== HARDWARE SETTINGS =====
    struct HardwareFlags {
        bool enable_real_hardware : 1;
        bool demo_mode : 1;
        bool iq_calibration_enabled : 1;
        bool rf_amp_enabled : 1;
        uint8_t reserved : 4;
    } hardware_flags = {true, false, false, false, 0};

    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    uint64_t user_min_freq_hz = 50000000ULL;
    uint64_t user_max_freq_hz = 6000000000ULL;

    // ===== SCANNING SETTINGS =====
    struct ScanningFlags {
        bool enable_wideband_scanning : 1;
        bool panoramic_mode_enabled : 1;
        bool enable_intelligent_scanning : 1;
        uint8_t reserved : 5;
    } scanning_flags = {false, true, true, 0};

    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;

    // ===== DETECTION SETTINGS =====
    struct DetectionFlags {
        bool enable_fhss_detection : 1;
        bool enable_intelligent_tracking : 1;
        uint8_t reserved : 6;
    } detection_flags = {true, true, 0};

    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint8_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;

    // ===== LOGGING SETTINGS (Zero-Heap Strings) =====
    struct LoggingFlags {
        bool auto_save_logs : 1;
        bool enable_session_logging : 1;
        bool include_timestamp : 1;
        bool include_rssi_values : 1;
        uint8_t reserved : 4;
    } logging_flags = {true, true, true, true, 0};

    char log_file_path[MAX_PATH_LEN] = "/eda_logs";
    char log_format[MAX_FORMAT_LEN] = "CSV";
    uint32_t max_log_file_size_kb = 1024;

    // ===== DISPLAY SETTINGS (Zero-Heap Strings) =====
    struct DisplayFlags {
        bool show_detailed_info : 1;
        bool show_mini_spectrum : 1;
        bool show_rssi_history : 1;
        bool show_frequency_ruler : 1;
        bool auto_ruler_style : 1;
        uint8_t reserved : 3;
    } display_flags = {true, true, true, true, true, 0};

    char color_scheme[MAX_NAME_LEN] = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    uint8_t frequency_ruler_style = 5;
    uint8_t compact_ruler_tick_count = 4;

    // ===== PROFILE SETTINGS (Zero-Heap Strings) =====
    struct ProfileFlags {
        bool enable_quick_profiles : 1;
        bool auto_save_on_change : 1;
        uint8_t reserved : 6;
    } profile_flags = {true, false, 0};

    char current_profile_name[MAX_NAME_LEN] = "Default";

    // ===== SYSTEM SETTINGS (Zero-Heap Strings) =====
    char freqman_path[MAX_NAME_LEN] = "DRONES";
    char settings_file_path[MAX_PATH_LEN] = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
};

#pragma pack(pop)

static_assert(sizeof(DroneAnalyzerSettings) <= 512, "DroneAnalyzerSettings exceeds 512 bytes");

// Default wideband constants for scanner settings
static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2400000000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2500000000ULL;

// Enhanced enums for EDA
enum class ThreatLevel : uint8_t {
    NONE = 0,
    LOW =1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4,
    UNKNOWN = 5
};

enum class DroneType : uint8_t {
    UNKNOWN = 0,
    MAVIC = 1,
    DJI_P34 = 2,
    PHANTOM = 3,
    DJI_MINI = 4,
    PARROT_ANAFI = 5,
    PARROT_BEBOP = 6,
    PX4_DRONE = 7,
    MILITARY_DRONE = 8,
    DIY_DRONE = 9,
    FPV_RACING = 10
};

enum class MovementTrend : uint8_t {
    STATIC = 0,
    APPROACHING = 1,
    RECEDING = 2,
    UNKNOWN = 3
};

enum class Language : uint8_t {
    ENGLISH = 0,
    RUSSIAN = 1
};

// DIAMOND OPTIMIZATION: constexpr LUTs instead of std::string comparisons
// Scott Meyers Item 15: Prefer constexpr to #define
// RAM savings: all strings stored in Flash, 0 heap allocation
struct TranslationEntry {
    const char* key;
    const char* value;
};

class Translator {
public:
    static void set_language(Language lang);
    static Language get_language();
    static const char* translate(const char* key);
    static const char* get_translation(const char* key);

private:
    static Language current_language_;
    static const char* get_english(const char* key);
    static const char* get_russian(const char* key);
};

struct DronePreset {
    char display_name[MAX_NAME_LEN];
    char name_template[MAX_NAME_LEN];
    Frequency frequency_hz;
    ThreatLevel threat_level;
    DroneType drone_type;

    bool is_valid() const {
        return display_name[0] != '\0' && frequency_hz > 0;
    }
};

// Implementations moved to ui_drone_common_types.cpp



// DetectionLogEntry structure for asynchronous logging
// This is a POD (Plain Old Data) structure for safe memory copying
#pragma pack(push, 1)
struct DetectionLogEntry {
    uint32_t timestamp;
    uint64_t frequency_hz;  // uint64_t for frequencies > 4GHz
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    uint8_t confidence_percent;  // Integer 0-100% instead of float for memory efficiency
    uint8_t width_bins;           // Signal width in bins (for calibration)
    uint32_t signal_width_hz;     // Signal width in Hz (for calibration)
    uint8_t snr;                  // Signal-to-Noise Ratio (for calibration)
};
#pragma pack(pop)

}  // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_DRONE_COMMON_TYPES_HPP_
