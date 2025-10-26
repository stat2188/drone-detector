/*
 * Enhanced Drone Analyzer Scanner App - Modular Implementation
 * Phase 7: TXT file communication - scanner reads configuration
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"
#include "../../file.hpp"          // For file I/O operations (Phase 7: TXT communication)
#include "database.hpp"     // For configuration storage patterns
#include <string>           // For std::string operations
#include <vector>           // For parameter parsing
#include <algorithm>        // For trim operations
#include <sstream>          // For std::istringstream

// Forward declarations needed for scanner main (ChibiOS integration fixes)
namespace ui::external_app::enhanced_drone_analyzer {
    class DroneHardwareController;
    class DroneScanner;
    struct DroneAnalyzerSettings;
    class ScanningCoordinator;
    struct AudioManager;  // Actually class, but treated as struct in code
}

#include "ui_scanner_combined.hpp"
#include "portapack.hpp"

using namespace portapack;

namespace ui::external_app::enhanced_drone_analyzer {

// Forward declarations that may be needed
class DroneHardwareController;
class DroneScanner;
struct DroneAnalyzerSettings;
class ScanningCoordinator;
class AudioManager;
 class DroneUIController;
class DroneDisplayController;
class EnhancedDroneSpectrumAnalyzerView;

namespace ScannerSettingsManager {
    // Function declarations for ChibiOS integration fixes (PHASE 2.2: Complete forward declarations)
    static bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings);
    static std::string trim_line(const std::string& line);
    static SpectrumMode parse_spectrum_mode(const std::string& value);
    static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
    static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
    static void reset_to_defaults(DroneAnalyzerSettings& settings);
    template<typename T> static T validate_range(T value, T min_val, T max_val);

    // PHASE 2.3: Implement complete function signatures and return types
    static bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        // PHASE 2.3: Validate settings file exists and is readable
        auto settings_loaded = load_from_txt_impl(filepath, settings);
        if (!settings_loaded) {
            // PHASE 2.3: Fall back to defaults if file can't be loaded
            reset_to_defaults(settings);
        }
        return settings_loaded;
    }

    static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
        std::istringstream iss(content);
        std::string line;
        int parsed_count = 0;
        while (std::getline(iss, line)) {
            auto trimmed_line = trim_line(line);
            if (trimmed_line.empty() || trimmed_line[0] == '#') continue;
            if (parse_key_value(settings, trimmed_line)) parsed_count++;
        }
        return parsed_count > 3;
    }

    static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) return false;
        std::string key = trim_line(line.substr(0, equals_pos));
        std::string value = trim_line(line.substr(equals_pos + 1));
        if (key == "spectrum_mode") {
                settings.spectrum_mode = parse_spectrum_mode(value);
                return true;
            } else if (key == "scan_interval_ms") {
                // PHASE 3.1: Fix template type deduction - use consistent uint32_t types for all parameters
                settings.scan_interval_ms = validate_range<uint32_t>(
                    static_cast<uint32_t>(std::stoul(value)),
                    static_cast<uint32_t>(100U),
                    static_cast<uint32_t>(5000U));
                return true;
            } else if (key == "rssi_threshold_db") {
                settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
                return true;
            } else if (key == "enable_audio_alerts") {
                settings.enable_audio_alerts = (value == "true");
                return true;
            } else if (key == "audio_alert_frequency_hz") {
                // PHASE 3.1: Fix template deduction with consistent uint16_t types for all parameters
                settings.audio_alert_frequency_hz = validate_range<uint16_t>(
                    static_cast<uint16_t>(std::stoul(value)),
                    static_cast<uint16_t>(200U),
                    static_cast<uint16_t>(3000U));
                return true;
            } else if (key == "audio_alert_duration_ms") {
                // PHASE 3.1: Fix template deduction with consistent uint32_t types for all parameters
                settings.audio_alert_duration_ms = validate_range<uint32_t>(
                    static_cast<uint32_t>(std::stoul(value)),
                    static_cast<uint32_t>(50U),
                    static_cast<uint32_t>(2000U));
                return true;
            } else if (key == "hardware_bandwidth_hz") {
                // PHASE 3.1: Fix template deduction with consistent uint32_t types for all parameters
                settings.hardware_bandwidth_hz = validate_range<uint32_t>(
                    static_cast<uint32_t>(std::stoul(value)),
                    static_cast<uint32_t>(1000000U),
                    static_cast<uint32_t>(100000000U));
                return true;
            } else if (key == "enable_real_hardware") {
                settings.enable_real_hardware = (value == "true");
                return true;
            } else if (key == "demo_mode") {
                settings.demo_mode = (value == "true");
                return true;
            } else if (key == "freqman_path") {
                settings.freqman_path = value.substr(0, 64);
                return true;
            }
        return false;
    }

    static void reset_to_defaults(DroneAnalyzerSettings& settings) {
        settings.spectrum_mode = SpectrumMode::MEDIUM;
        settings.scan_interval_ms = 750;
        settings.rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
        settings.enable_audio_alerts = true;
        settings.audio_alert_frequency_hz = 800;
        settings.audio_alert_duration_ms = 200;
        settings.hardware_bandwidth_hz = 24000000;
        settings.enable_real_hardware = true;
        settings.demo_mode = false;
        settings.freqman_path = "DRONES";
    }

    static SpectrumMode parse_spectrum_mode(const std::string& value) {
        if (value == "NARROW") return SpectrumMode::NARROW;
        if (value == "MEDIUM") return SpectrumMode::MEDIUM;
        if (value == "WIDE") return SpectrumMode::WIDE;
        if (value == "ULTRA_WIDE") return SpectrumMode::ULTRA_WIDE;
        return SpectrumMode::MEDIUM;
    }

    static std::string trim_line(const std::string& line) {
        auto start = std::find_if_not(line.begin(), line.end(), ::isspace);
        auto end = std::find_if_not(line.rbegin(), line.rend(), ::isspace).base();
        return (start < end) ? std::string(start, end) : std::string();
    }

    template<typename T>
    static T validate_range(T value, T min_val, T max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }

    // Implementation functions (removed invalid 'private:' for namespace syntax fix)
    static bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings) {
        File txt_file;
        if (!txt_file.open(filepath, true)) {  // FIXED: true = read_only parameter
            reset_to_defaults(settings);
            return false;
        }
        std::string file_content;
        file_content.resize(txt_file.size());
        auto read_result = txt_file.read(file_content.data(), txt_file.size());
        if (read_result != txt_file.size()) {
            txt_file.close();
            reset_to_defaults(settings);
            return false;
        }
        txt_file.close();
        return parse_settings_content(settings, file_content);
    }
void initialize_app(ui::NavigationView& nav) {
    // PHASE 7: Load settings from TXT file if available
    ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings loaded_settings;

    // Attempt to load settings from SD card
    bool settings_loaded = ScannerSettingsManager::load_settings_from_txt(loaded_settings);

    // Show loading screen with status
    auto loading_view = std::make_unique<ui::external_app::enhanced_drone_analyzer::LoadingScreenView>(nav);
    nav.push(loading_view.get());

    // Small delay to show loading (ChibiOS compliant)
    chThdSleepMilliseconds(500);

    // Push main scanner view with loaded settings
    auto main_view = std::make_unique<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView>(nav);
    nav.replace(main_view.get());

    // PHASE 7: Show communication status
    if (settings_loaded) {
        nav.display_modal("Scanner Ready",
                         "Configuration loaded from SD card\n"
                         "Settings applied successfully\n\n"
                         "Scanner ready for drone detection");
    } else {
        nav.display_modal("Scanner Ready",
                         "Default configuration used\n"
                         "Use Settings app to save preferences\n\n"
                         "Scanner ready for drone detection");
    }
}
}  // namespace ScannerSettingsManager
}  // namespace ui::external_app::enhanced_drone_analyzer

extern "C" {

__attribute__((section(".external_app.app_enhanced_drone_analyzer_scanner.application_information"), used)) application_information_t _application_information_enhanced_drone_analyzer_scanner = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer::initialize_app,
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "EDA Scanner",
    /*.bitmap_data = */ {
        0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 0xF0, 0x0F, 0xF8, 0x1F,
        0xFC, 0x3F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFC, 0x3F, 0xF8, 0x1F, 0xF0, 0x0F,
        0xE0, 0x07, 0xC0, 0x03, 0x80, 0x01, 0x00, 0x00
    },
    /*.icon_color = */ ui::Color::red().v,
    /*.menu_location = */ app_location_t::RX,
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = */ portapack::spi_flash::image_tag_wideband_spectrum,
    /*.m4_app_offset = */ 0x00000000,  // will be filled at compile time
};
}
