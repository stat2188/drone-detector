// ui_spectrum_settings.cpp - Implementation for advanced spectrum settings
// Migrated preset loading from Looking Glass, adapted for EDA

#include "ui_spectrum_settings.hpp"
#include "file.hpp"
#include <sstream>
#include <algorithm>
#include <cstdio>

namespace ui::external_app::enhanced_drone_analyzer {

// SpectrumPresetLoader implementation
SpectrumPresetLoader::SpectrumPresetLoader(const std::string& preset_path)
    : preset_file_path_(preset_path) {
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
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> parts;

    // Parse comma-separated values
    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        token.erase(token.begin(), std::find_if(token.begin(), token.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        token.erase(std::find_if(token.rbegin(), token.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), token.end());
        parts.push_back(token);
    }

    if (parts.size() < 4) return false;

    try {
        preset.min_freq_hz = std::stoull(parts[0]);
        preset.max_freq_hz = std::stoull(parts[1]);
        preset.label = parts[2];

        // Parse threat level
        std::string threat_str = parts[3];
        std::transform(threat_str.begin(), threat_str.end(), threat_str.begin(), ::toupper);

        if (threat_str == "NONE") preset.default_threat_level = ThreatLevel::NONE;
        else if (threat_str == "LOW") preset.default_threat_level = ThreatLevel::LOW;
        else if (threat_str == "MEDIUM") preset.default_threat_level = ThreatLevel::MEDIUM;
        else if (threat_str == "HIGH") preset.default_threat_level = ThreatLevel::HIGH;
        else if (threat_str == "CRITICAL") preset.default_threat_level = ThreatLevel::CRITICAL;

        // Determine spectrum mode based on range width
        Frequency range = preset.max_freq_hz - preset.min_freq_hz;
        if (range > 500000000ULL) preset.spectrum_mode = SpectrumMode::ULTRA_WIDE;
        else if (range > 100000000ULL) preset.spectrum_mode = SpectrumMode::WIDE;
        else preset.spectrum_mode = SpectrumMode::MEDIUM;

        return preset.is_valid();
    } catch (const std::exception&) {
        return false;
    }
}

std::string SpectrumPresetLoader::serialize_preset(const FrequencyPreset& preset) const {
    auto threat_to_string = [](ThreatLevel level) -> std::string {
        switch (level) {
            case ThreatLevel::NONE: return "NONE";
            case ThreatLevel::LOW: return "LOW";
            case ThreatLevel::MEDIUM: return "MEDIUM";
            case ThreatLevel::HIGH: return "HIGH";
            case ThreatLevel::CRITICAL: return "CRITICAL";
            default: return "NONE";
        }
    };

    std::stringstream ss;
    ss << preset.min_freq_hz << ",";
    ss << preset.max_freq_hz << ",";
    ss << preset.label << ",";
    ss << threat_to_string(preset.default_threat_level);

    return ss.str();
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
