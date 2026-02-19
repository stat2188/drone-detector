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
// 🔴 PHASE 3: Use constants from EDA::Constants instead of magic numbers
static constexpr size_t MAX_PATH_LEN = EDA::Constants::MAX_PATH_LENGTH;
static constexpr size_t MAX_NAME_LEN = EDA::Constants::MAX_NAME_LENGTH;
static constexpr size_t MAX_FORMAT_LEN = EDA::Constants::MAX_FORMAT_LENGTH;

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
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t audio_flags = 0x01;  // bit0: enable_alerts, bit1: repeat_alerts

    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;

    // ===== HARDWARE SETTINGS =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t hardware_flags = 0x01;  // bit0: enable_real_hardware, bit1: demo_mode, bit2: iq_calibration_enabled, bit3: rf_amp_enabled

    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    uint64_t user_min_freq_hz = 50000000ULL;
    uint64_t user_max_freq_hz = 6000000000ULL;

    // ===== SCANNING SETTINGS =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t scanning_flags = 0x06;  // bit0: enable_wideband_scanning, bit1: panoramic_mode_enabled, bit2: enable_intelligent_scanning

    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;

    // ===== DETECTION SETTINGS =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t detection_flags = 0x03;  // bit0: enable_fhss_detection, bit1: enable_intelligent_tracking

    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint8_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;

    // ===== LOGGING SETTINGS (Zero-Heap Strings) =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t logging_flags = 0x0F;  // bit0: auto_save_logs, bit1: enable_session_logging, bit2: include_timestamp, bit3: include_rssi_values

    char log_file_path[MAX_PATH_LEN] = "/eda_logs";
    char log_format[MAX_FORMAT_LEN] = "CSV";
    uint32_t max_log_file_size_kb = 1024;

    // ===== DISPLAY SETTINGS (Zero-Heap Strings) =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t display_flags = 0x1F;  // bit0: show_detailed_info, bit1: show_mini_spectrum, bit2: show_rssi_history, bit3: show_frequency_ruler, bit4: auto_ruler_style

    char color_scheme[MAX_NAME_LEN] = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    uint8_t frequency_ruler_style = 5;
    uint8_t compact_ruler_tick_count = 4;

    // ===== PROFILE SETTINGS (Zero-Heap Strings) =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t profile_flags = 0x01;  // bit0: enable_quick_profiles, bit1: auto_save_on_change

    char current_profile_name[MAX_NAME_LEN] = "Default";

    // ===== SYSTEM SETTINGS (Zero-Heap Strings) =====
    char freqman_path[MAX_NAME_LEN] = "DRONES";
    char settings_file_path[MAX_PATH_LEN] = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
};

#pragma pack(pop)

static_assert(sizeof(DroneAnalyzerSettings) <= 512, "DroneAnalyzerSettings exceeds 512 bytes");

// FIX #27: Helper functions for explicit bitfield access (replaces bitfields for portability)
// These functions provide type-safe access to bit flags without implementation-defined behavior

// Audio flags helpers (bit0: enable_alerts, bit1: repeat_alerts)
inline bool audio_get_enable_alerts(const DroneAnalyzerSettings& s) noexcept { return (s.audio_flags & 0x01) != 0; }
inline void audio_set_enable_alerts(DroneAnalyzerSettings& s, bool v) noexcept { s.audio_flags = (s.audio_flags & ~0x01) | (v ? 0x01 : 0); }
inline bool audio_get_repeat_alerts(const DroneAnalyzerSettings& s) noexcept { return (s.audio_flags & 0x02) != 0; }
inline void audio_set_repeat_alerts(DroneAnalyzerSettings& s, bool v) noexcept { s.audio_flags = (s.audio_flags & ~0x02) | (v ? 0x02 : 0); }

// Hardware flags helpers (bit0: enable_real_hardware, bit1: demo_mode, bit2: iq_calibration_enabled, bit3: rf_amp_enabled)
inline bool hw_get_enable_real_hardware(const DroneAnalyzerSettings& s) noexcept { return (s.hardware_flags & 0x01) != 0; }
inline void hw_set_enable_real_hardware(DroneAnalyzerSettings& s, bool v) noexcept { s.hardware_flags = (s.hardware_flags & ~0x01) | (v ? 0x01 : 0); }
inline bool hw_get_demo_mode(const DroneAnalyzerSettings& s) noexcept { return (s.hardware_flags & 0x02) != 0; }
inline void hw_set_demo_mode(DroneAnalyzerSettings& s, bool v) noexcept { s.hardware_flags = (s.hardware_flags & ~0x02) | (v ? 0x02 : 0); }
inline bool hw_get_iq_calibration_enabled(const DroneAnalyzerSettings& s) noexcept { return (s.hardware_flags & 0x04) != 0; }
inline void hw_set_iq_calibration_enabled(DroneAnalyzerSettings& s, bool v) noexcept { s.hardware_flags = (s.hardware_flags & ~0x04) | (v ? 0x04 : 0); }
inline bool hw_get_rf_amp_enabled(const DroneAnalyzerSettings& s) noexcept { return (s.hardware_flags & 0x08) != 0; }
inline void hw_set_rf_amp_enabled(DroneAnalyzerSettings& s, bool v) noexcept { s.hardware_flags = (s.hardware_flags & ~0x08) | (v ? 0x08 : 0); }

// Scanning flags helpers (bit0: enable_wideband_scanning, bit1: panoramic_mode_enabled, bit2: enable_intelligent_scanning)
inline bool scan_get_enable_wideband_scanning(const DroneAnalyzerSettings& s) noexcept { return (s.scanning_flags & 0x01) != 0; }
inline void scan_set_enable_wideband_scanning(DroneAnalyzerSettings& s, bool v) noexcept { s.scanning_flags = (s.scanning_flags & ~0x01) | (v ? 0x01 : 0); }
inline bool scan_get_panoramic_mode_enabled(const DroneAnalyzerSettings& s) noexcept { return (s.scanning_flags & 0x02) != 0; }
inline void scan_set_panoramic_mode_enabled(DroneAnalyzerSettings& s, bool v) noexcept { s.scanning_flags = (s.scanning_flags & ~0x02) | (v ? 0x02 : 0); }
inline bool scan_get_enable_intelligent_scanning(const DroneAnalyzerSettings& s) noexcept { return (s.scanning_flags & 0x04) != 0; }
inline void scan_set_enable_intelligent_scanning(DroneAnalyzerSettings& s, bool v) noexcept { s.scanning_flags = (s.scanning_flags & ~0x04) | (v ? 0x04 : 0); }

// Detection flags helpers (bit0: enable_fhss_detection, bit1: enable_intelligent_tracking)
inline bool detect_get_enable_fhss_detection(const DroneAnalyzerSettings& s) noexcept { return (s.detection_flags & 0x01) != 0; }
inline void detect_set_enable_fhss_detection(DroneAnalyzerSettings& s, bool v) noexcept { s.detection_flags = (s.detection_flags & ~0x01) | (v ? 0x01 : 0); }
inline bool detect_get_enable_intelligent_tracking(const DroneAnalyzerSettings& s) noexcept { return (s.detection_flags & 0x02) != 0; }
inline void detect_set_enable_intelligent_tracking(DroneAnalyzerSettings& s, bool v) noexcept { s.detection_flags = (s.detection_flags & ~0x02) | (v ? 0x02 : 0); }

// Logging flags helpers (bit0: auto_save_logs, bit1: enable_session_logging, bit2: include_timestamp, bit3: include_rssi_values)
inline bool log_get_auto_save_logs(const DroneAnalyzerSettings& s) noexcept { return (s.logging_flags & 0x01) != 0; }
inline void log_set_auto_save_logs(DroneAnalyzerSettings& s, bool v) noexcept { s.logging_flags = (s.logging_flags & ~0x01) | (v ? 0x01 : 0); }
inline bool log_get_enable_session_logging(const DroneAnalyzerSettings& s) noexcept { return (s.logging_flags & 0x02) != 0; }
inline void log_set_enable_session_logging(DroneAnalyzerSettings& s, bool v) noexcept { s.logging_flags = (s.logging_flags & ~0x02) | (v ? 0x02 : 0); }
inline bool log_get_include_timestamp(const DroneAnalyzerSettings& s) noexcept { return (s.logging_flags & 0x04) != 0; }
inline void log_set_include_timestamp(DroneAnalyzerSettings& s, bool v) noexcept { s.logging_flags = (s.logging_flags & ~0x04) | (v ? 0x04 : 0); }
inline bool log_get_include_rssi_values(const DroneAnalyzerSettings& s) noexcept { return (s.logging_flags & 0x08) != 0; }
inline void log_set_include_rssi_values(DroneAnalyzerSettings& s, bool v) noexcept { s.logging_flags = (s.logging_flags & ~0x08) | (v ? 0x08 : 0); }

// Display flags helpers (bit0: show_detailed_info, bit1: show_mini_spectrum, bit2: show_rssi_history, bit3: show_frequency_ruler, bit4: auto_ruler_style)
inline bool disp_get_show_detailed_info(const DroneAnalyzerSettings& s) noexcept { return (s.display_flags & 0x01) != 0; }
inline void disp_set_show_detailed_info(DroneAnalyzerSettings& s, bool v) noexcept { s.display_flags = (s.display_flags & ~0x01) | (v ? 0x01 : 0); }
inline bool disp_get_show_mini_spectrum(const DroneAnalyzerSettings& s) noexcept { return (s.display_flags & 0x02) != 0; }
inline void disp_set_show_mini_spectrum(DroneAnalyzerSettings& s, bool v) noexcept { s.display_flags = (s.display_flags & ~0x02) | (v ? 0x02 : 0); }
inline bool disp_get_show_rssi_history(const DroneAnalyzerSettings& s) noexcept { return (s.display_flags & 0x04) != 0; }
inline void disp_set_show_rssi_history(DroneAnalyzerSettings& s, bool v) noexcept { s.display_flags = (s.display_flags & ~0x04) | (v ? 0x04 : 0); }
inline bool disp_get_show_frequency_ruler(const DroneAnalyzerSettings& s) noexcept { return (s.display_flags & 0x08) != 0; }
inline void disp_set_show_frequency_ruler(DroneAnalyzerSettings& s, bool v) noexcept { s.display_flags = (s.display_flags & ~0x08) | (v ? 0x08 : 0); }
inline bool disp_get_auto_ruler_style(const DroneAnalyzerSettings& s) noexcept { return (s.display_flags & 0x10) != 0; }
inline void disp_set_auto_ruler_style(DroneAnalyzerSettings& s, bool v) noexcept { s.display_flags = (s.display_flags & ~0x10) | (v ? 0x10 : 0); }

// Profile flags helpers (bit0: enable_quick_profiles, bit1: auto_save_on_change)
inline bool profile_get_enable_quick_profiles(const DroneAnalyzerSettings& s) noexcept { return (s.profile_flags & 0x01) != 0; }
inline void profile_set_enable_quick_profiles(DroneAnalyzerSettings& s, bool v) noexcept { s.profile_flags = (s.profile_flags & ~0x01) | (v ? 0x01 : 0); }
inline bool profile_get_auto_save_on_change(const DroneAnalyzerSettings& s) noexcept { return (s.profile_flags & 0x02) != 0; }
inline void profile_set_auto_save_on_change(DroneAnalyzerSettings& s, bool v) noexcept { s.profile_flags = (s.profile_flags & ~0x02) | (v ? 0x02 : 0); }

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
    ENGLISH = 0
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
    static void set_language(Language lang) noexcept;
    static Language get_language() noexcept;
    static const char* translate(const char* key) noexcept;
    static const char* get_translation(const char* key) noexcept;

private:
    static Language current_language_;
    static const char* get_english(const char* key) noexcept;
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
