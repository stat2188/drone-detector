#ifndef __UI_DRONE_COMMON_TYPES_HPP__
#define __UI_DRONE_COMMON_TYPES_HPP__

#include <cstdint>
#include <string>
#include <vector>
#include "ch.h" // For Mayhem/ChibiOS types if necessary

// 1. Forward declarations and Enums

// FIX: Moved up for visibility in structures
using Frequency = uint64_t;

enum class SpectrumMode { NARROW, MEDIUM, WIDE, ULTRA_WIDE, ULTRA_NARROW };

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

struct DronePreset {
    std::string display_name;
    std::string name_template;
    Frequency frequency_hz; // Now Frequency is defined above
    ThreatLevel threat_level;
    DroneType drone_type;

    bool is_valid() const {
        return !display_name.empty() && frequency_hz > 0;
    }
};

namespace ui::external_app::enhanced_drone_analyzer {

// 2. Settings Structure
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

    // Additional members
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

    // --- НОВЫЕ ПОЛЯ (Вставьте в конец структуры) ---
    
    // Пользовательский диапазон сканирования
    // Дефолтные: 50 МГц - 6000 МГц (полный функционал)
    uint64_t user_min_freq_hz = 50000000ULL;   
    uint64_t user_max_freq_hz = 6000000000ULL; 
    
    // Ширина "реза" (среза) в режимах Wideband/Hybrid
    // Чем меньше это число, тем детальнее спектр, но медленнее сканирование
    uint32_t wideband_slice_width_hz = 24000000; // 24 MHz
    
    // Сохранять ли полный спектр (true) или только найденное (false)
    bool panoramic_mode_enabled = true; 
};

} // namespace ui::external_app::enhanced_drone_analyzer

// 3. Settings Manager (Lightweight implementation for Settings App)
class DroneAnalyzerSettingsManager {
public:
    static void reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    static const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& get_current_settings();
};

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

inline const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& DroneAnalyzerSettingsManager::get_current_settings() {
    static ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings current_settings;
    return current_settings;
}

// 4. Helper Constants
static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2400000000;
static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2500000000;
static constexpr uint32_t MIN_HARDWARE_FREQ = 1000000;
static constexpr uint64_t MAX_HARDWARE_FREQ = 7200000000ULL;

// 5. Translator Class (Stub for settings)
class Translator {
public:
    static void set_language(Language lang) { current_language_ = lang; }
    static const char* get_translation(const std::string& key);
private:
    static Language current_language_;
};

inline Language Translator::current_language_ = Language::ENGLISH;

inline const char* Translator::get_translation(const std::string& key) {
    if (current_language_ == Language::RUSSIAN) {
        if (key == "save_settings") return "Save Settings";
        if (key == "audio_settings") return "Audio";
    }
    return key.c_str();
}

// Validation constants
static constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;
static constexpr uint32_t MAX_SCAN_INTERVAL_MS = 10000;
static constexpr uint16_t MIN_AUDIO_FREQ = 200;
static constexpr uint16_t MAX_AUDIO_FREQ = 4000;
static constexpr uint32_t MIN_AUDIO_DURATION = 50;
static constexpr uint32_t MAX_AUDIO_DURATION = 5000;

#endif // __UI_DRONE_COMMON_TYPES_HPP__
