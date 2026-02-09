#ifndef UI_DRONE_COMMON_TYPES_HPP_
#define UI_DRONE_COMMON_TYPES_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include "drone_constants.hpp"
#include "radio.hpp"
#include "file.hpp"

// SpectrumMode forward declare
enum class SpectrumMode : uint8_t { NARROW = 0, MEDIUM = 1, WIDE = 2, ULTRA_WIDE = 3, ULTRA_NARROW = 4 };

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// ===========================================
// DroneAnalyzerSettings - UNIFIED SETTINGS STRUCTURE
// Single source of truth for all EDA settings
// Merged from: EDAUnifiedSettings, EDAAppSettings, DroneAnalyzerSettings, etc.
// ===========================================
// DIAMOND OPTIMIZATION: bitfield packing для экономии RAM (смотри settings_bitfields.hpp)
// bool flags заменены на bitfields в будущем, пока совместимость сохранена
struct DroneAnalyzerSettings {
    // ===== AUDIO SETTINGS =====
    bool enable_audio_alerts = true;
    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;
    bool audio_repeat_alerts = false;

    // ===== HARDWARE SETTINGS =====
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    bool enable_real_hardware = true;
    bool demo_mode = false;

    // IQ Calibration (merged from IQCalibrationSettings)
    bool iq_calibration_enabled = false;
    uint8_t rx_phase_value = 15;

    // Amplifier Controls (merged from AmplifierControl)
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    bool rf_amp_enabled = false;

    // Frequency ranges
    uint64_t user_min_freq_hz = 50000000ULL;
    uint64_t user_max_freq_hz = 6000000000ULL;

    // ===== SCANNING SETTINGS =====
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    bool enable_wideband_scanning = false;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;
    bool panoramic_mode_enabled = true;
    bool enable_intelligent_scanning = true;

    // ===== DETECTION SETTINGS =====
    bool enable_fhss_detection = true;
    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint8_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;
    bool enable_intelligent_tracking = true;

    // ===== LOGGING SETTINGS =====
    bool auto_save_logs = true;
    std::string log_file_path = "/eda_logs";
    std::string log_format = "CSV";
    uint32_t max_log_file_size_kb = 1024;
    bool enable_session_logging = true;
    bool include_timestamp = true;
    bool include_rssi_values = true;

    // ===== DISPLAY SETTINGS =====
    std::string color_scheme = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    bool show_detailed_info = true;
    bool show_mini_spectrum = true;
    bool show_rssi_history = true;
    bool show_frequency_ruler = true;
    uint8_t frequency_ruler_style = 5;
    uint8_t compact_ruler_tick_count = 4;
    bool auto_ruler_style = true;

    // ===== PROFILE SETTINGS =====
    std::string current_profile_name = "Default";
    bool enable_quick_profiles = true;
    bool auto_save_on_change = false;

    // ===== SYSTEM SETTINGS =====
    std::string freqman_path = "DRONES";
    std::string settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
};

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

// DIAMOND OPTIMIZATION: constexpr LUTs вместо std::string сравнений
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономия RAM: все строки хранятся во Flash, ноль heap allocation
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
    std::string display_name;
    std::string name_template;
    Frequency frequency_hz;
    ThreatLevel threat_level;
    DroneType drone_type;

    bool is_valid() const {
        return !display_name.empty() && frequency_hz > 0;
    }
};

// Implementations moved to ui_drone_common_types.cpp

static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;

// Validation constants for scanner settings
static constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;
static constexpr uint8_t MIN_DETECTION_COUNT = 3;
static constexpr uint32_t MAX_SCAN_INTERVAL_MS = 10000;
static constexpr uint16_t MIN_AUDIO_FREQ = 200;
static constexpr uint16_t MAX_AUDIO_FREQ = 4000;
static constexpr uint32_t MIN_AUDIO_DURATION = 50;
static constexpr uint32_t MAX_AUDIO_DURATION = 5000;

// DetectionLogEntry structure for asynchronous logging
// This is a POD (Plain Old Data) structure for safe memory copying
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

}  // namespace ui::apps::enhanced_drone_analyzer

#endif // __UI_DRONE_COMMON_TYPES_HPP__
