// ui_spectrum_settings.cpp - Implementation for advanced spectrum settings
// Migrated preset loading from Looking Glass, adapted for EDA

#include "ui_spectrum_settings.hpp"
#include "file.hpp"
#include "string_format.hpp" // Важно для to_string_dec_uint

// <sstream> больше не нужен!
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

namespace ui::external_app::enhanced_drone_analyzer {

// SpectrumPresetLoader implementation
SpectrumPresetLoader::SpectrumPresetLoader(const std::string& preset_path)
    : preset_file_path_(preset_path), settings_() {
    // Initialize with default drone presets
    initialize_default_presets();
}

SpectrumPresetLoader::~SpectrumPresetLoader() {
    // Auto-save if needed could be added here
}

void SpectrumPresetLoader::initialize_default_presets() {
    // Add common drone frequency ranges from Looking Glass pattern
    // Migrated and adapted for EDA focus
    settings_.add_preset({
        2410000000ULL,  // 2.41 GHz (DJI/Mavic)
        2460000000ULL,  // 2.46 GHz
        "DJI/Mavic Band",
        SpectrumMode::MEDIUM,
        ThreatLevel::HIGH
    });

    settings_.add_preset({
        2425000000ULL,  // 2.425 GHz (Parrot)
        2475000000ULL,  // 2.475 GHz
        "Parrot Band",
        SpectrumMode::MEDIUM,
        ThreatLevel::MEDIUM
    });

    settings_.add_preset({
        5705000000ULL,  // 5.705 GHz (Experimental)
        5905000000ULL,  // 5.905 GHz
        "5.8GHz Band",
        SpectrumMode::WIDE,
        ThreatLevel::LOW
    });

    settings_.add_preset({
        915000000ULL,   // 915 MHz (US drones)
        928000000ULL,   // 928 MHz
        "915MHz Band",
        SpectrumMode::MEDIUM,
        ThreatLevel::LOW
    });
}

bool SpectrumPresetLoader::load_presets_from_file() {
    // For now, use default presets - file loading implementation
    // can be added once proper File API is confirmed
    initialize_default_presets();
    return true;
}

bool SpectrumPresetLoader::save_presets_to_file() const {
    // File saving implementation deferred - return success for now
    // Will be implemented once File API details are confirmed
    return true;
}

bool SpectrumPresetLoader::parse_preset_line(const std::string& line, FrequencyPreset& preset) {
    char buffer[128];
    // Копируем безопасно
    strncpy(buffer, line.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // 1. MIN FREQUENCY
    char* token = strtok(buffer, ",");
    if (!token) return false;

    char* end;
    preset.min_freq_hz = std::strtoull(token, &end, 10);
    // Проверка: если число не распарсилось, end будет равен началу token
    if (token == end) return false;

    // 2. MAX FREQUENCY
    token = strtok(NULL, ",");
    if (!token) return false;
    preset.max_freq_hz = std::strtoull(token, &end, 10);
    if (token == end) return false;

    // 3. LABEL
    token = strtok(NULL, ",");
    if (!token) return false;

    // Trim leading spaces
    while(*token == ' ' || *token == '\t') token++;

    // Trim trailing newline/return (частая проблема CSV)
    size_t len = strlen(token);
    while(len > 0 && (token[len-1] == '\r' || token[len-1] == '\n' || token[len-1] == ' ')) {
        token[len-1] = '\0';
        len--;
    }

    preset.label = token; // Единственная аллокация (неизбежна для хранения)

    // 4. THREAT LEVEL
    token = strtok(NULL, ",");
    // Если threat level отсутствует, ставим дефолт и не считаем это ошибкой
    if (!token) {
        preset.default_threat_level = ThreatLevel::NONE;
    } else {
        while(*token == ' ' || *token == '\t') token++;

        // In-place uppercase (без создания std::string)
        for(char* p = token; *p; ++p) {
            *p = std::toupper((unsigned char)*p);
            // Убираем возможные символы переноса строки в конце
            if (*p == '\r' || *p == '\n') { *p = '\0'; break; }
        }

        if (strncmp(token, "NONE", 4) == 0) preset.default_threat_level = ThreatLevel::NONE;
        else if (strncmp(token, "LOW", 3) == 0) preset.default_threat_level = ThreatLevel::LOW;
        else if (strncmp(token, "MEDIUM", 6) == 0) preset.default_threat_level = ThreatLevel::MEDIUM;
        else if (strncmp(token, "HIGH", 4) == 0) preset.default_threat_level = ThreatLevel::HIGH;
        else if (strncmp(token, "CRITICAL", 8) == 0) preset.default_threat_level = ThreatLevel::CRITICAL;
        else preset.default_threat_level = ThreatLevel::NONE;
    }

    // 5. ВАЖНО: Расчет режима спектра (восстановленная логика)
    int64_t range = (int64_t)(preset.max_freq_hz - preset.min_freq_hz);
    if (range > 500000000) preset.spectrum_mode = SpectrumMode::ULTRA_WIDE;
    else if (range > 100000000) preset.spectrum_mode = SpectrumMode::WIDE;
    else preset.spectrum_mode = SpectrumMode::MEDIUM;

    return preset.is_valid();
}

std::string SpectrumPresetLoader::serialize_preset(const FrequencyPreset& preset) const {
    // Лямбда возвращает const char* (без аллокации string)
    auto threat_to_string = [](ThreatLevel level) -> const char* {
        switch (level) {
            case ThreatLevel::LOW: return "LOW";
            case ThreatLevel::MEDIUM: return "MEDIUM";
            case ThreatLevel::HIGH: return "HIGH";
            case ThreatLevel::CRITICAL: return "CRITICAL";
            case ThreatLevel::NONE: default: return "NONE";
        }
    };

    // Используем to_string_dec_uint из string_format.hpp (стандарт Mayhem)
    // Если его нет, можно использовать std::to_string или snprintf
    
    std::string line;
    // Резервируем память заранее, чтобы избежать 5-6 переаллокаций при +=
    line.reserve(128);

    line += to_string_dec_uint(preset.min_freq_hz);
    line += ",";
    line += to_string_dec_uint(preset.max_freq_hz);
    line += ",";
    line += preset.label;
    line += ",";
    line += threat_to_string(preset.default_threat_level);
    
    // Добавляем перенос строки, так как это CSV
    line += "\n";

    return line;
}

std::string SpectrumPresetLoader::get_preset_filename() const {
    // Use EDA preset directory instead of Looking Glass
    return preset_file_path_ + "/presets.txt";
}

const FrequencyPreset* SpectrumPresetLoader::get_preset(size_t index) const {
    if (index < settings_.preset_ranges.size()) {
        return &settings_.preset_ranges[index];
    }
    return nullptr;
}

bool SpectrumPresetLoader::add_custom_preset(const FrequencyPreset& preset) {
    if (preset.is_valid()) {
        settings_.add_preset(preset);
        return save_presets_to_file();  // Auto-save to file
    }
    return false;
}

} // namespace ui::external_app::enhanced_drone_analyzer
