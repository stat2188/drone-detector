#ifndef __UI_DRONE_COMMON_TYPES_HPP__
#define __UI_DRONE_COMMON_TYPES_HPP__

#include <cstdint>
#include <string>

// 1. Перечисления (Enums)
enum class SpectrumMode { NARROW, MEDIUM, WIDE, ULTRA_WIDE, ULTRA_NARROW };

enum class ThreatLevel {
    NONE = 0, LOW = 1, MEDIUM = 2, HIGH = 3, CRITICAL = 4, UNKNOWN = 5
};

// 2. Структура настроек (Единый источник правды)
namespace ui::external_app::enhanced_drone_analyzer {

struct DroneAnalyzerSettings {
    // Основные настройки
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    
    // Аудио
    bool enable_audio_alerts = true;
    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    
    // Железо
    uint32_t hardware_bandwidth_hz = 24000000;
    bool enable_real_hardware = true;
    bool demo_mode = false; // Инверсия enable_real_hardware

    // Путь к файлу настроек (константа)
    static constexpr const char* FILE_PATH = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
};

} // namespace ui::external_app::enhanced_drone_analyzer

#endif // __UI_DRONE_COMMON_TYPES_HPP__
