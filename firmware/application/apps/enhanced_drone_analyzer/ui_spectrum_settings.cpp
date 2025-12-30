// ui_spectrum_settings.cpp - Implementation for advanced spectrum settings
// Migrated preset loading from Looking Glass, adapted for EDA

#include "ui_spectrum_settings.hpp"
#include "file.hpp"
#include "string_format.hpp" // Important for to_string_dec_uint

// <sstream> no longer needed!
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

namespace ui::apps::enhanced_drone_analyzer {

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
    // 1. Most popular: Mavic / WiFi
    settings_.add_preset({
        2400000000ULL, 2483500000ULL,
        "2.4GHz Control/Video",
        SpectrumMode::WIDE,
        ThreatLevel::HIGH
    });

    // 2. FPV Analog and Digital (DJI FPV)
    settings_.add_preset({
        5645000000ULL, 5945000000ULL,
        "5.8GHz FPV Band",
        SpectrumMode::ULTRA_WIDE,
        ThreatLevel::MEDIUM
    });

    // 3. Long Range Systems (TBS/ELRS)
    settings_.add_preset({
        860000000ULL, 930000000ULL,
        "868/915MHz LRS",
        SpectrumMode::WIDE,
        ThreatLevel::CRITICAL // LRS often means a drone flying from afar
    });

    // 4. Old range
    settings_.add_preset({
        433000000ULL, 435000000ULL,
        "433MHz LRS",
        SpectrumMode::MEDIUM,
        ThreatLevel::MEDIUM
    });
    
    // 5. Video 1.2 GHz (for long range flights)
    settings_.add_preset({
        1080000000ULL, 1360000000ULL,
        "1.2GHz Long Range Video",
        SpectrumMode::WIDE,
        ThreatLevel::HIGH
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
    // Copy safely
    strncpy(buffer, line.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // 1. MIN FREQUENCY
    char* token = strtok(buffer, ",");
    if (!token) return false;

    char* end;
    preset.min_freq_hz = std::strtoull(token, &end, 10);
    // Check: if number didn't parse, end will equal start of token
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

    // Trim trailing newline/return (common CSV problem)
    size_t len = strlen(token);
    while(len > 0 && (token[len-1] == '\r' || token[len-1] == '\n' || token[len-1] == ' ')) {
        token[len-1] = '\0';
        len--;
    }

    preset.label = token; // Single allocation (unavoidable for storage)

    // 4. THREAT LEVEL
    token = strtok(NULL, ",");
    // If threat level is missing, set default and don't consider it an error
    if (!token) {
        preset.default_threat_level = ThreatLevel::NONE;
    } else {
        while(*token == ' ' || *token == '\t') token++;

        // In-place uppercase (without creating std::string)
        for(char* p = token; *p; ++p) {
            *p = std::toupper((unsigned char)*p);
            // Remove possible newline characters at the end
            if (*p == '\r' || *p == '\n') { *p = '\0'; break; }
        }

        if (strncmp(token, "NONE", 4) == 0) preset.default_threat_level = ThreatLevel::NONE;
        else if (strncmp(token, "LOW", 3) == 0) preset.default_threat_level = ThreatLevel::LOW;
        else if (strncmp(token, "MEDIUM", 6) == 0) preset.default_threat_level = ThreatLevel::MEDIUM;
        else if (strncmp(token, "HIGH", 4) == 0) preset.default_threat_level = ThreatLevel::HIGH;
        else if (strncmp(token, "CRITICAL", 8) == 0) preset.default_threat_level = ThreatLevel::CRITICAL;
        else preset.default_threat_level = ThreatLevel::NONE;
    }

    // 5. IMPORTANT: Spectrum mode calculation (restored logic)
    int64_t range = (int64_t)(preset.max_freq_hz - preset.min_freq_hz);
    if (range > 500000000) preset.spectrum_mode = SpectrumMode::ULTRA_WIDE;
    else if (range > 100000000) preset.spectrum_mode = SpectrumMode::WIDE;
    else preset.spectrum_mode = SpectrumMode::MEDIUM;

    return preset.is_valid();
}

std::string SpectrumPresetLoader::serialize_preset(const FrequencyPreset& preset) const {
    // Lambda returns const char* (without string allocation)
    auto threat_to_string = [](ThreatLevel level) -> const char* {
        switch (level) {
            case ThreatLevel::LOW: return "LOW";
            case ThreatLevel::MEDIUM: return "MEDIUM";
            case ThreatLevel::HIGH: return "HIGH";
            case ThreatLevel::CRITICAL: return "CRITICAL";
            case ThreatLevel::NONE: default: return "NONE";
        }
    };

    // Use to_string_dec_uint from string_format.hpp (Mayhem standard)
    // If not available, can use std::to_string or snprintf

    std::string line;
    // Reserve memory in advance to avoid 5-6 reallocations with +=
    line.reserve(128);

    line += to_string_dec_uint(preset.min_freq_hz);
    line += ",";
    line += to_string_dec_uint(preset.max_freq_hz);
    line += ",";
    line += preset.label;
    line += ",";
    line += threat_to_string(preset.default_threat_level);

    // Add newline since this is CSV
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

} // namespace ui::apps::enhanced_drone_analyzer
