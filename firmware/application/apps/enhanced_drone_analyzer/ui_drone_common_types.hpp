#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ch.h"

// SpectrumMode forward declare
enum class SpectrumMode { NARROW, MEDIUM, WIDE, ULTRA_WIDE, ULTRA_NARROW };

namespace ui::apps::enhanced_drone_analyzer {

// DroneAnalyzerSettings struct
struct DroneAnalyzerSettings {
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    bool enable_audio_alerts = true;
    uint16_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint32_t hardware_bandwidth_hz = 24000000;
    bool enable_real_hardware = true;
    bool demo_mode = false;

    // Additional members required by settings parser
    std::string freqman_path = "DRONES";
    std::string settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    bool enable_wideband_scanning = false;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint64_t min_frequency_hz = 2400000000ULL;
    uint64_t max_frequency_hz = 2500000000ULL;
    bool show_detailed_info = true;
    bool auto_save_logs = true;
    std::string log_file_path = "/eda_logs";

    // Save settings to TXT file
    bool save() const {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        // Implementation would require File API - for now just return false
        (void)filepath;
        return false;
    }
};

} // namespace ui::apps::enhanced_drone_analyzer

// DroneAnalyzerSettingsManager class
class DroneAnalyzerSettingsManager {
public:
    static bool load_settings(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    static bool save_settings(const ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    static void reset_to_defaults(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);

private:
    static std::string get_settings_path();
    static bool parse_line(const std::string& line, ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
};

// Implementations moved to ui_drone_common_types.cpp

// Enhanced enums for EDA
enum class ThreatLevel {
    NONE = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4,
    UNKNOWN = 5
};

enum class DroneType {
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

enum class MovementTrend {
    STATIC = 0,
    APPROACHING = 1,
    RECEDING = 2,
    UNKNOWN = 3
};

enum class Language {
    ENGLISH,
    RUSSIAN
};

class Translator {
public:
    static void set_language(Language lang);
    static Language get_language();
    static const char* translate(const std::string& key);
    static const char* get_translation(const std::string& key);

private:
    static Language current_language_;
    static const char* get_english(const std::string& key);
    static const char* get_russian(const std::string& key);
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
    uint64_t frequency_hz; // uint64_t для частот > 4ГГц
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    uint8_t confidence_percent; // Используем 0-100% вместо float для экономии памяти и тактов
};
