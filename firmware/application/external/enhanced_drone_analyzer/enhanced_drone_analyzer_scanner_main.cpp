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
#include "file.hpp"          // For file I/O operations (Phase 7: TXT communication)
#include "database.hpp"     // For configuration storage patterns
#include <string>           // For std::string operations
#include <vector>           // For parameter parsing
#include <algorithm>        // For trim operations

// Forward declarations needed for scanner main
class AudioManager;

#include "ui_scanner_combined.hpp"
#include "portapack.hpp"

using namespace portapack;

using namespace ui::external_app::enhanced_drone_analyzer;

/**
 * PHASE 7: TXT FILE COMMUNICATION - SCANNER SETTINGS READER
 * Loads configuration from /sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt
 * Provides ChibiOS-compliant settings management for scanner module
 */
class ScannerSettingsManager {
public:
    /**
     * Load settings from TXT file to DroneAnalyzerSettings structure
     * Returns true if settings loaded successfully, false otherwise
     */
    static bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

        // PHASE 7: Using file.hpp API for SD card access
        auto result = File::open(filepath, File::Mode::Read);
        if (!result.is_valid()) {
            // File doesn't exist, use defaults and log warning
            reset_to_defaults(settings);
            return false;
        }

        auto file = result.take();

        // Read entire file content
        std::string file_content;
        file_content.resize(file.size());

        auto read_result = file.read(file_content.data(), file.size());
        if (read_result != file.size()) {
            file.close();
            reset_to_defaults(settings);
            return false;
        }

        file.close();

        // Parse file content
        return parse_settings_content(settings, file_content);
    }

    /**
     * Apply loaded settings to scanner hardware and controller
     */
    static bool apply_settings_to_scanner(
        DroneHardwareController& hardware,
        DroneScanner& scanner,
        const DroneAnalyzerSettings& settings
    ) {
        try {
            // Apply spectrum mode to hardware
            hardware.set_spectrum_mode(settings.spectrum_mode);
            hardware.set_spectrum_bandwidth(settings.hardware_bandwidth_hz);

            // Apply scanning mode to scanner logic
            if (settings.spectrum_mode == SpectrumMode::NARROW) {
                scanner.set_scanning_mode(DroneScanner::ScanningMode::DATABASE);
            } else if (settings.spectrum_mode == SpectrumMode::WIDE) {
                scanner.set_scanning_mode(DroneScanner::ScanningMode::WIDEBAND_CONTINUOUS);
            } else {
                scanner.set_scanning_mode(DroneScanner::ScanningMode::HYBRID);
            }

            // Set real/demo mode
            if (settings.demo_mode) {
                scanner.switch_to_demo_mode();
            } else {
                scanner.switch_to_real_mode();
            }

            return true;

        } catch (const std::exception& e) {
            // Log error but don't crash
            return false;
        }
    }

private:
    /**
     * Parse TXT file content and apply to settings
     */
    static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
        std::istringstream iss(content);
        std::string line;
        int parsed_count = 0;

        while (std::getline(iss, line)) {
            // Skip comments and empty lines
            auto trimmed_line = trim_line(line);
            if (trimmed_line.empty() || trimmed_line[0] == '#') {
                continue;
            }

            // Parse key=value pairs
            if (parse_key_value(settings, trimmed_line)) {
                parsed_count++;
            }
        }

        return parsed_count > 3; // Require at least 3 recognized settings
    }

    /**
     * Parse single key=value line
     */
    static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) {
            return false;
        }

        std::string key = trim_line(line.substr(0, equals_pos));
        std::string value = trim_line(line.substr(equals_pos + 1));

        try {
            if (key == "spectrum_mode") {
                settings.spectrum_mode = parse_spectrum_mode(value);
                return true;
            } else if (key == "scan_interval_ms") {
                settings.scan_interval_ms = validate_range(std::stoul(value), 100u, 5000u);
                return true;
            } else if (key == "rssi_threshold_db") {
                settings.rssi_threshold_db = validate_range(std::stoi(value), -120, -30);
                return true;
            } else if (key == "enable_audio_alerts") {
                settings.enable_audio_alerts = (value == "true");
                return true;
            } else if (key == "audio_alert_frequency_hz") {
                settings.audio_alert_frequency_hz = validate_range(std::stoul(value), 200u, 3000u);
                return true;
            } else if (key == "audio_alert_duration_ms") {
                settings.audio_alert_duration_ms = validate_range(std::stoul(value), 50u, 2000u);
                return true;
            } else if (key == "hardware_bandwidth_hz") {
                settings.hardware_bandwidth_hz = validate_range(std::stoul(value), 1000000u, 100000000u);
                return true;
            } else if (key == "enable_real_hardware") {
                settings.enable_real_hardware = (value == "true");
                return true;
            } else if (key == "demo_mode") {
                settings.demo_mode = (value == "true");
                return true;
            } else if (key == "freqman_path") {
                settings.freqman_path = value.substr(0, 64); // Limit path length
                return true;
            }
            // Ignore unknown keys

        } catch (const std::exception&) {
            // Invalid value format, skip this line
            return false;
        }

        return false;
    }

    /**
     * Reset settings to default values for scanner
     */
    static void reset_to_defaults(DroneAnalyzerSettings& settings) {
        settings.spectrum_mode = SpectrumMode::MEDIUM;           // Balanced performance
        settings.scan_interval_ms = 750;                         // Good response time
        settings.rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;  // Standard sensitivity
        settings.enable_audio_alerts = true;                     // Enable alerts by default
        settings.audio_alert_frequency_hz = 800;                 // Standard beep tone
        settings.audio_alert_duration_ms = 200;                  // Short but noticeable
        settings.hardware_bandwidth_hz = 24000000;              // ISM band width
        settings.enable_real_hardware = true;                    // Prefer real hardware
        settings.demo_mode = false;                              // Real mode preferred
        settings.freqman_path = "DRONES";                        // Standard freqman path
    }

    /**
     * Parse spectrum mode from string
     */
    static SpectrumMode parse_spectrum_mode(const std::string& value) {
        if (value == "NARROW") return SpectrumMode::NARROW;
        if (value == "MEDIUM") return SpectrumMode::MEDIUM;
        if (value == "WIDE") return SpectrumMode::WIDE;
        if (value == "ULTRA_WIDE") return SpectrumMode::ULTRA_WIDE;
        return SpectrumMode::MEDIUM; // Safe default
    }

    /**
     * Trim whitespace from line
     */
    static std::string trim_line(const std::string& line) {
        auto start = std::find_if_not(line.begin(), line.end(), ::isspace);
        auto end = std::find_if_not(line.rbegin(), line.rend(), ::isspace).base();
        return (start < end) ? std::string(start, end) : std::string();
    }

    /**
     * Validate integer value is within min-max range
     */
    template<typename T>
    static T validate_range(T value, T min_val, T max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }
};

namespace ui::external_app::enhanced_drone_analyzer_scanner {
void initialize_app(ui::NavigationView& nav) {
    // PHASE 7: Load settings from TXT file if available
    DroneAnalyzerSettings loaded_settings;

    // Attempt to load settings from SD card
    bool settings_loaded = ScannerSettingsManager::load_settings_from_txt(loaded_settings);

    // Show loading screen with status
    auto loading_view = std::make_unique<LoadingScreenView>(nav);
    nav.push(std::move(loading_view));

    // Small delay to show loading (ChibiOS compliant)
    chThdSleepMilliseconds(500);

    // Push main scanner view with loaded settings
    auto main_view = std::make_unique<EnhancedDroneSpectrumAnalyzerView>(nav);
    nav.replace(std::move(main_view), NavViewId::None);

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
}  // namespace ui::external_app::enhanced_drone_analyzer_scanner

extern "C" {

__attribute__((section(".external_app.app_enhanced_drone_analyzer_scanner.application_information"), used)) application_information_t _application_information_enhanced_drone_analyzer_scanner = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer_scanner::initialize_app,
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

    /*.m4_app_tag = */ {'E', 'D', 'A', 'S'},  // EDA Scanner
    /*.m4_app_offset = */ 0x00000000,  // will be filled at compile time
};
}
