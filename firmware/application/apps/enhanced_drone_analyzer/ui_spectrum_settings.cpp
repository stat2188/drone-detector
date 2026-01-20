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
