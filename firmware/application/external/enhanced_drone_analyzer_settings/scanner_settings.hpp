#ifndef __SCANNER_SETTINGS_HPP__
#define __SCANNER_SETTINGS_HPP__

#include <string>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cctype>
#include "file.hpp"
#include "ui_drone_common_types.hpp"

// Forward declarations
enum class SpectrumMode;
using ScanFile = File;

namespace ScannerSettingsManager {

    // Template declarations
    template<typename T> T validate_range(T value, T min_val, T max_val);

    // Function declarations
    inline void reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);
    inline ::SpectrumMode parse_spectrum_mode(const char* value);
    inline std::string trim_line(const std::string& line);
    inline char* trim_in_place(char* str);
    inline bool parse_key_value(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, char* line_buffer);
    inline bool parse_settings_content(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& content);
    inline bool load_from_txt_impl(const std::string& filepath, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings);

    // Public interface (marked inline)
    inline bool load_settings_from_txt(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        bool settings_loaded = load_from_txt_impl(filepath, settings);
        if (!settings_loaded) {
            reset_to_defaults(settings);
        }
        return settings_loaded;
    }

    // Implementation (marked inline)
    inline bool parse_settings_content(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& content) {
        std::istringstream iss(content);
        std::string line;
        int parsed_count = 0;
        while (std::getline(iss, line)) {
            auto trimmed_line = trim_line(line);
            if (trimmed_line.empty() || trimmed_line[0] == '#') continue;
        char line_buffer[256];
        strncpy(line_buffer, trimmed_line.c_str(), sizeof(line_buffer) - 1);
        line_buffer[sizeof(line_buffer) - 1] = 0; // Ensure null-terminator
            if (parse_key_value(settings, line_buffer)) parsed_count++;
        }
        return parsed_count > 3;
    }

    inline bool parse_key_value(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, char* line_buffer) {
        char* equals_ptr = strchr(line_buffer, '=');
        if (!equals_ptr) return false;

        *equals_ptr = 0;
        char* key = trim_in_place(line_buffer);
        char* value = trim_in_place(equals_ptr + 1);

        if (strcmp(key, "spectrum_mode") == 0) {
            settings.spectrum_mode = parse_spectrum_mode(value);
            return true;
        }
        else if (strcmp(key, "scan_interval_ms") == 0) {
        // FIX: Use common constant MAX_SCAN_INTERVAL_MS (10000)
        settings.scan_interval_ms = validate_range<uint32_t>(
            static_cast<uint32_t>(strtoul(value, nullptr, 10)),
            MIN_SCAN_INTERVAL_MS, MAX_SCAN_INTERVAL_MS);
            return true;
        }
        else if (strcmp(key, "rssi_threshold_db") == 0) {
            settings.rssi_threshold_db = validate_range<int32_t>(strtol(value, nullptr, 10), -120, -30);
            return true;
        }
        else if (strcmp(key, "enable_audio_alerts") == 0) {
            settings.enable_audio_alerts = (strcmp(value, "true") == 0);
            return true;
        }
        // --- НОВЫЕ БЛОКИ ---
        else if (strcmp(key, "audio_alert_frequency_hz") == 0) {
            settings.audio_alert_frequency_hz = validate_range<uint16_t>(
                static_cast<uint16_t>(strtoul(value, nullptr, 10)), MIN_AUDIO_FREQ, MAX_AUDIO_FREQ);
            return true;
        }
        else if (strcmp(key, "audio_alert_duration_ms") == 0) {
            settings.audio_alert_duration_ms = validate_range<uint32_t>(
                static_cast<uint32_t>(strtoul(value, nullptr, 10)), MIN_AUDIO_DURATION, MAX_AUDIO_DURATION);
            return true;
        }
        else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
            settings.hardware_bandwidth_hz = strtoul(value, nullptr, 10);
            return true;
        }
        else if (strcmp(key, "enable_real_hardware") == 0) {
            settings.enable_real_hardware = (strcmp(value, "true") == 0);
            settings.demo_mode = !settings.enable_real_hardware; // Synchronize flags
            return true;
        }
        else if (strcmp(key, "freqman_path") == 0) {
            settings.freqman_path = value; // Assignment from char* to std::string works automatically
            return true;
        }
        // -------------------

        return false;
    }

    inline void reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
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
        settings.freqman_path = "DRONES.TXT";
    }

    inline ::SpectrumMode parse_spectrum_mode(const char* value) {
        if (strcmp(value, "NARROW") == 0) return ::SpectrumMode::NARROW;
        if (strcmp(value, "MEDIUM") == 0) return ::SpectrumMode::MEDIUM;
        if (strcmp(value, "WIDE") == 0) return ::SpectrumMode::WIDE;
        if (strcmp(value, "ULTRA_WIDE") == 0) return ::SpectrumMode::ULTRA_WIDE;
        if (strcmp(value, "ULTRA_NARROW") == 0) return ::SpectrumMode::ULTRA_NARROW;
        return ::SpectrumMode::MEDIUM;
    }

    inline std::string trim_line(const std::string& line) {
        auto start = std::find_if_not(line.begin(), line.end(), ::isspace);
        auto end = std::find_if_not(line.rbegin(), line.rend(), ::isspace).base();
        return (start < end) ? std::string(start, end) : std::string();
    }

    inline char* trim_in_place(char* str) {
        if (!str) return str;
        while (*str && isspace((unsigned char)*str)) str++;
        if (*str == 0) return str;
        char* end = str + strlen(str) - 1;
        while (end > str && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';
        return str;
    }

    template<typename T>
    T validate_range(T value, T min_val, T max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }

    inline bool load_from_txt_impl(const std::string& filepath, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
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

}  // namespace ScannerSettingsManager

#endif // __SCANNER_SETTINGS_HPP__
