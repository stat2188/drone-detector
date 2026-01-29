#include "scanner_settings.hpp"
#include <string>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cstdlib>
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
    void reset_to_defaults(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
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

    ::SpectrumMode parse_spectrum_mode(const char* value) {
        if (strcmp(value, "NARROW") == 0) return ::SpectrumMode::NARROW;
        if (strcmp(value, "MEDIUM") == 0) return ::SpectrumMode::MEDIUM;
        if (strcmp(value, "WIDE") == 0) return ::SpectrumMode::WIDE;
        if (strcmp(value, "ULTRA_WIDE") == 0) return ::SpectrumMode::ULTRA_WIDE;
        return ::SpectrumMode::MEDIUM;
    }

    char* trim_in_place(char* str) {
        if (!str) return nullptr;

        // Trim leading
        while (*str && ::isspace((unsigned char)*str)) str++;

        if (*str == 0) return str; // Empty string

        // Trim trailing
        char* end = str + strlen(str) - 1;
        while (end > str && ::isspace((unsigned char)*end)) end--;
        *(end + 1) = 0; // Null terminator

        return str;
    }

    bool parse_key_value(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, char* line_buffer) {
        // Look for separator
        char* equals_ptr = strchr(line_buffer, '=');
        if (!equals_ptr) return false;

        *equals_ptr = 0; // Split string into two parts: key and value

        char* key = trim_in_place(line_buffer);
        char* value = trim_in_place(equals_ptr + 1);

        // Comparison via strcmp (0 allocations)
        if (strcmp(key, "spectrum_mode") == 0) {
            settings.spectrum_mode = parse_spectrum_mode(value);
            return true;
        } else if (strcmp(key, "scan_interval_ms") == 0) {
            settings.scan_interval_ms = validate_range<uint32_t>(
                static_cast<uint32_t>(strtoul(value, nullptr, 10)), MIN_SCAN_INTERVAL_MS, MAX_SCAN_INTERVAL_MS);
            return true;
        } else if (strcmp(key, "rssi_threshold_db") == 0) {
            settings.rssi_threshold_db = validate_range<int32_t>(strtol(value, nullptr, 10), -120, -30);
            return true;
        } else if (strcmp(key, "enable_audio_alerts") == 0) {
            settings.enable_audio_alerts = (strcmp(value, "true") == 0);
            return true;
        }
        // --- NEW AUDIO FIELDS ---
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
        // --- NEW HARDWARE FIELDS ---
        else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
            settings.hardware_bandwidth_hz = strtoul(value, nullptr, 10);
            return true;
        }
        else if (strcmp(key, "enable_real_hardware") == 0) {
            settings.enable_real_hardware = (strcmp(value, "true") == 0);
            settings.demo_mode = !settings.enable_real_hardware; // Sync flags
            return true;
        }
        else if (strcmp(key, "freqman_path") == 0) {
            settings.freqman_path = value; // char* to std::string assignment
            return true;
        }
        // --- НОВЫЕ БЛОКИ ПАРСИНГА (Вставьте сюда) ---

        else if (strcmp(key, "user_min_freq_hz") == 0) {
            settings.user_min_freq_hz = strtoul(value, nullptr, 10);
            return true;
        }
        else if (strcmp(key, "user_max_freq_hz") == 0) {
            settings.user_max_freq_hz = strtoul(value, nullptr, 10);
            return true;
        }
        else if (strcmp(key, "wideband_slice_width_hz") == 0) {
            settings.wideband_slice_width_hz = strtoul(value, nullptr, 10);
            return true;
        }
        return false;
    }

    bool parse_settings_content(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings, const std::string& content) {
        const char* ptr = content.c_str();
        const char* end = ptr + content.size();
        int parsed_count = 0;

        while (ptr < end) {
            // Find end of line
            const char* line_end = ptr;
            while (line_end < end && *line_end != '\n' && *line_end != '\r') line_end++;

            // Copy line to buffer (assuming lines are short, < 128 chars)
            char line_buffer[128];
            size_t line_len = line_end - ptr;
            if (line_len >= sizeof(line_buffer)) line_len = sizeof(line_buffer) - 1;
            memcpy(line_buffer, ptr, line_len);
            line_buffer[line_len] = 0;

            // Trim and check
            char* trimmed = trim_in_place(line_buffer);
            if (*trimmed == 0 || *trimmed == '#') {
                ptr = line_end;
                while (ptr < end && (*ptr == '\n' || *ptr == '\r')) ptr++;
                continue;
            }

            if (parse_key_value(settings, trimmed)) parsed_count++;

            // Move to next line
            ptr = line_end;
            while (ptr < end && (*ptr == '\n' || *ptr == '\r')) ptr++;
        }

        return parsed_count > 3;
    }

    bool load_from_txt_impl(const std::string& filepath, ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
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
             DroneAnalyzerSettingsManager::reset_to_defaults(settings);
             return false;
        }

        return parse_settings_content(settings, file_content);
    }

    // Public interface
    bool load_settings_from_txt(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        bool settings_loaded = load_from_txt_impl(filepath, settings);
        if (!settings_loaded) {
            reset_to_defaults(settings);
        }
        return settings_loaded;
    }

}  // namespace ScannerSettingsManager
