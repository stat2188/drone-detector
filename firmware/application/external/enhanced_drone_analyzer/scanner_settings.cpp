#include "scanner_settings.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include "file.hpp"
#include "ui_drone_common_types.hpp"

// Forward declarations
enum class SpectrumMode;
using ScanFile = File;

namespace ScannerSettingsManager {

    // Template implementations
    template<typename T>
    T validate_range(T value, T min_val, T max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }

    // Function implementations
    void reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        // Reset to reasonable defaults
        settings.spectrum_mode = ::SpectrumMode::MEDIUM;
        settings.scan_interval_ms = 712;
        settings.rssi_threshold_db = -80;
        settings.enable_audio_alerts = true;
        settings.audio_alert_frequency_hz = 800;
        settings.audio_alert_duration_ms = 200;
        settings.hardware_bandwidth_hz = 24000000;
        settings.enable_real_hardware = true;
        settings.demo_mode = false;
        settings.freqman_path = "DRONES";
    }

    ::SpectrumMode parse_spectrum_mode(const std::string& value) {
        if (value == "NARROW") return ::SpectrumMode::NARROW;
        if (value == "MEDIUM") return ::SpectrumMode::MEDIUM;
        if (value == "WIDE") return ::SpectrumMode::WIDE;
        if (value == "ULTRA_WIDE") return ::SpectrumMode::ULTRA_WIDE;
        return ::SpectrumMode::MEDIUM;
    }

    std::string trim_line(const std::string& line) {
        auto start = std::find_if_not(line.begin(), line.end(), ::isspace);
        auto end = std::find_if_not(line.rbegin(), line.rend(), ::isspace).base();
        return (start < end) ? std::string(start, end) : std::string();
    }

    bool parse_key_value(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& line) {
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) return false;
        std::string key = trim_line(line.substr(0, equals_pos));
        std::string value = trim_line(line.substr(equals_pos + 1));

        if (key == "spectrum_mode") {
            settings.spectrum_mode = parse_spectrum_mode(value);
            return true;
        } else if (key == "scan_interval_ms") {
            settings.scan_interval_ms = validate_range<uint32_t>(
                static_cast<uint32_t>(std::stoul(value)), 100U, 5000U);
            return true;
        } else if (key == "rssi_threshold_db") {
            settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
            return true;
        } else if (key == "enable_audio_alerts") {
            settings.enable_audio_alerts = (value == "true");
            return true;
        }
        // Additional fields can be added here
        return false;
    }

    bool parse_settings_content(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& content) {
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

    bool load_from_txt_impl(const std::string& filepath, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        ScanFile txt_file;
        if (!txt_file.open(filepath)) { // removed read_only parameter if not supported by API wrapper
            reset_to_defaults(settings);
            return false;
        }
        std::string file_content;
        file_content.resize(txt_file.size());
        auto read_result = txt_file.read(file_content.data(), txt_file.size());
        txt_file.close();

        if (read_result.is_error()) {
             reset_to_defaults(settings);
             return false;
        }

        return parse_settings_content(settings, file_content);
    }

    // Public interface
    bool load_settings_from_txt(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        bool settings_loaded = load_from_txt_impl(filepath, settings);
        if (!settings_loaded) {
            reset_to_defaults(settings);
        }
        return settings_loaded;
    }

}  // namespace ScannerSettingsManager
