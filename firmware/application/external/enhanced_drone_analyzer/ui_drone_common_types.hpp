#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ch.h"

// SpectrumMode forward declare
enum class SpectrumMode { NARROW, MEDIUM, WIDE, ULTRA_WIDE, ULTRA_NARROW };

namespace ui::external_app::enhanced_drone_analyzer {

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

} // namespace ui::external_app::enhanced_drone_analyzer

// DroneAnalyzerSettingsManager class
class DroneAnalyzerSettingsManager {
public:
    static bool load_settings(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    static bool save_settings(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    static void reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);

private:
    static std::string get_settings_path();
    static bool parse_line(const std::string& line, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
};

// Implementations marked as inline to prevent ODR violations
inline std::string DroneAnalyzerSettingsManager::get_settings_path() {
    return "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
}

inline void DroneAnalyzerSettingsManager::reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
    settings.spectrum_mode = SpectrumMode::MEDIUM;
    settings.scan_interval_ms = 1000;
    settings.rssi_threshold_db = -90;
    settings.enable_audio_alerts = true;
    settings.audio_alert_frequency_hz = 800;
    settings.audio_alert_duration_ms = 500;
    settings.hardware_bandwidth_hz = 24000000;
    settings.enable_real_hardware = true;
    settings.demo_mode = false;
}

inline bool DroneAnalyzerSettingsManager::parse_line(const std::string& line, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
    size_t equals_pos = line.find('=');
    if (equals_pos == std::string::npos) return false;
    std::string key = line.substr(0, equals_pos);
    std::string value = line.substr(equals_pos + 1);

    // Trim whitespace
    key.erase(0, key.find_first_not_of(" \t"));
    if (key.length() > 0) key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    if (value.length() > 0) value.erase(value.find_last_not_of(" \t") + 1);

    if (key == "spectrum_mode") {
        if (value == "NARROW") settings.spectrum_mode = SpectrumMode::NARROW;
        else if (value == "MEDIUM") settings.spectrum_mode = SpectrumMode::MEDIUM;
        else if (value == "WIDE") settings.spectrum_mode = SpectrumMode::WIDE;
        else if (value == "ULTRA_WIDE") settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
        return true;
    } else if (key == "scan_interval_ms") {
        settings.scan_interval_ms = std::stoul(value);
        return true;
    } else if (key == "rssi_threshold_db") {
        settings.rssi_threshold_db = std::stoi(value);
        return true;
    } else if (key == "enable_audio_alerts") {
        settings.enable_audio_alerts = (value == "true");
        return true;
    }
    return false;
}

inline bool DroneAnalyzerSettingsManager::load_settings(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
    reset_to_defaults(settings);
    return true;
}

inline bool DroneAnalyzerSettingsManager::save_settings(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
    (void)settings;
    return false;
}

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
    MILITARY_DRONE = 8
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

// Inline implementations to avoid multiple definition linker errors
inline Language Translator::current_language_ = Language::ENGLISH;

inline const char* Translator::get_english(const std::string& key) {
    if (key == "load_database") return "Load Database";
    if (key == "save_frequency") return "Save Frequency";
    if (key == "advanced") return "Advanced";
    if (key == "constant_settings") return "Constant Settings";
    if (key == "select_language") return "Select Language";
    if (key == "about_author") return "About Author";
    if (key == "english") return "English";
    if (key == "russian") return "Russian";
    if (key == "english_selected") return "Language updated to English";
    if (key == "russian_selected") return "Язык изменен на русский";
    return key.c_str();
}

inline const char* Translator::get_russian(const std::string& key) {
    if (key == "load_database") return "Загрузить БД";
    if (key == "save_frequency") return "Сохранить частоту";
    if (key == "advanced") return "Расширенные";
    if (key == "constant_settings") return "Постоянные настройки";
    if (key == "select_language") return "Выбрать язык";
    if (key == "about_author") return "Об авторе";
    if (key == "english") return "Английский";
    if (key == "russian") return "Русский";
    if (key == "english_selected") return "Language updated to English";
    if (key == "russian_selected") return "Язык изменен на русский";
    return key.c_str();
}

inline void Translator::set_language(Language lang) {
    current_language_ = lang;
}

inline Language Translator::get_language() {
    return current_language_;
}

inline const char* Translator::translate(const std::string& key) {
    return get_translation(key);
}

inline const char* Translator::get_translation(const std::string& key) {
    if (current_language_ == Language::RUSSIAN) {
        return get_russian(key);
    } else {
        return get_english(key);
    }
}

static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
static constexpr msg_t MSG_OK = 0;
