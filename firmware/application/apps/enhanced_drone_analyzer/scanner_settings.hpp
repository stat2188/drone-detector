#ifndef SCANNER_SETTINGS_HPP_
#define SCANNER_SETTINGS_HPP_

#include <string>
#include <algorithm>
#include <cstdint>
#include "file.hpp"
#include "ui_drone_common_types.hpp"

// Forward declarations
enum class SpectrumMode;
using ScanFile = File;

namespace ScannerSettingsManager {

    // Template declarations
    template<typename T> T validate_range(T value, T min_val, T max_val);

    // Function declarations
    void reset_to_defaults(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    ::SpectrumMode parse_spectrum_mode(const char* value);
    char* trim_in_place(char* str);
    bool parse_key_value(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, char* line_buffer);
    bool parse_settings_content(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& content);
    bool load_from_txt_impl(const std::string& filepath, ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);

    // Public interface
    bool load_settings_from_txt(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);

}  // namespace ScannerSettingsManager

#endif // __SCANNER_SETTINGS_HPP__
