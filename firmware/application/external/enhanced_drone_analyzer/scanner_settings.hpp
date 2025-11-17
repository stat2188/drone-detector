#ifndef __SCANNER_SETTINGS_HPP__
#define __SCANNER_SETTINGS_HPP__

#include <string>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include "file.hpp"

// Forward declarations to avoid circular includes
enum class SpectrumMode;

// Fixed File reference since it's not nested
using ScanFile = File;

namespace ui::external_app::enhanced_drone_analyzer {

struct DroneAnalyzerSettings {
    // Core scanning parameters
    ::SpectrumMode spectrum_mode = ::SpectrumMode::MEDIUM;
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    bool enable_audio_alerts = true;
    uint16_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;

    // Hardware settings
    uint32_t hardware_bandwidth_hz = 24000000;
    bool enable_real_hardware = true;
    bool demo_mode = false;

    // Save settings to TXT file
    bool save() const {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        std::stringstream ss;

        ss << "spectrum_mode=" << spectrum_mode_to_string(spectrum_mode) << "\n";
        ss << "scan_interval_ms=" << scan_interval_ms << "\n";
        ss << "rssi_threshold_db=" << rssi_threshold_db << "\n";
        ss << "enable_audio_alerts=" << (enable_audio_alerts ? "true" : "false") << "\n";
        ss << "audio_alert_frequency_hz=" << audio_alert_frequency_hz << "\n";
        ss << "audio_alert_duration_ms=" << audio_alert_duration_ms << "\n";
        ss << "hardware_bandwidth_hz=" << hardware_bandwidth_hz << "\n";
        ss << "enable_real_hardware=" << (enable_real_hardware ? "true" : "false") << "\n";
        ss << "demo_mode=" << (demo_mode ? "true" : "false") << "\n";

        std::string content = ss.str();

        ScanFile file;
        if (!file.create(filepath)) {
            return false;
        }

        auto result = file.write(content.data(), content.size());
        file.close();

        return result == content.size();
    }

private:
    std::string spectrum_mode_to_string(::SpectrumMode mode) const {
        switch (mode) {
            case ::SpectrumMode::NARROW: return "NARROW";
            case ::SpectrumMode::MEDIUM: return "MEDIUM";
            case ::SpectrumMode::WIDE: return "WIDE";
            case ::SpectrumMode::ULTRA_WIDE: return "ULTRA_WIDE";
            default: return "MEDIUM";
        }
    }
};

}  // namespace ui::external_app::enhanced_drone_analyzer

namespace ScannerSettingsManager {

    // Template declarations
    template<typename T> T validate_range(T value, T min_val, T max_val);

    // Function declarations
    void reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    ::SpectrumMode parse_spectrum_mode(const std::string& value);
    std::string trim_line(const std::string& line);
    bool parse_key_value(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& line);
    bool parse_settings_content(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& content);
    bool load_from_txt_impl(const std::string& filepath, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);

    // Public interface
    bool load_settings_from_txt(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        bool settings_loaded = load_from_txt_impl(filepath, settings);
        if (!settings_loaded) {
            reset_to_defaults(settings);
        }
        return settings_loaded;
    }

    // Implementation
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
        } else if (key == "audio_alert_frequency_hz") {
            settings.audio_alert_frequency_hz = validate_range<uint16_t>(
                static_cast<uint16_t>(std::stoul(value)), 200U, 3000U);
            return true;
        } else if (key == "audio_alert_duration_ms") {
            settings.audio_alert_duration_ms = validate_range<uint32_t>(
                static_cast<uint32_t>(std::stoul(value)), 50U, 2000U);
            return true;
        } else if (key == "hardware_bandwidth_hz") {
            settings.hardware_bandwidth_hz = validate_range<uint32_t>(
                static_cast<uint32_t>(std::stoul(value)), 1000000U, 100000000U);
            return true;
        } else if (key == "enable_real_hardware") {
            settings.enable_real_hardware = (value == "true");
            return true;
        } else if (key == "demo_mode") {
            settings.demo_mode = (value == "true");
            return true;
        }
        return false;
    }

    void reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        settings.spectrum_mode = ::SpectrumMode::MEDIUM;
        settings.scan_interval_ms = 712;
        settings.rssi_threshold_db = -80;
        settings.enable_audio_alerts = true;
        settings.audio_alert_frequency_hz = 800;
        settings.audio_alert_duration_ms = 200;
        settings.hardware_bandwidth_hz = 24000000;
        settings.enable_real_hardware = true;
        settings.demo_mode = false;
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

    template<typename T>
    T validate_range(T value, T min_val, T max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }

    bool load_from_txt_impl(const std::string& filepath, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        ScanFile txt_file;
        if (!txt_file.open(filepath, true)) {  // true = read_only parameter
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

}  // namespace ScannerSettingsManager

#endif // __SCANNER_SETTINGS_HPP__
