#include "scanner_settings.hpp"
#include <string>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <climits>
#include "file.hpp"
#include "ui_drone_common_types.hpp"
#include "string_format.hpp"

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

    // Safe string to number conversion with error checking
    static bool safe_parse_uint32(const char* str, uint32_t* result) {
        if (!str || !result) return false;
        char* endptr = nullptr;
        unsigned long val = strtoul(str, &endptr, 10);
        
        // Check for conversion errors
        if (endptr == str || *endptr != '\0') {
            return false;  // No valid digits or trailing garbage
        }
        
        if (val == ULONG_MAX && errno == ERANGE) {
            return false;  // Overflow
        }
        
        *result = static_cast<uint32_t>(val);
        return true;
    }

    static bool safe_parse_int32(const char* str, int32_t* result) {
        if (!str || !result) return false;
        char* endptr = nullptr;
        long val = strtol(str, &endptr, 10);
        
        // Check for conversion errors
        if (endptr == str || *endptr != '\0') {
            return false;
        }
        
        if ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE) {
            return false;  // Overflow/underflow
        }
        
        *result = static_cast<int32_t>(val);
        return true;
    }

    static bool safe_parse_uint64(const char* str, uint64_t* result) {
        if (!str || !result) return false;
        char* endptr = nullptr;
        unsigned long long val = strtoull(str, &endptr, 10);
        
        // Check for conversion errors
        if (endptr == str || *endptr != '\0') {
            return false;
        }
        
        if (val == ULLONG_MAX && errno == ERANGE) {
            return false;  // Overflow
        }
        
        *result = val;
        return true;
    }

    // Function implementations
    void reset_to_defaults(ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        // ===== AUDIO SETTINGS =====
        settings.enable_audio_alerts = true;
        settings.audio_alert_frequency_hz = 800;
        settings.audio_alert_duration_ms = 500;
        settings.audio_volume_level = 50;
        settings.audio_repeat_alerts = false;

        // ===== HARDWARE SETTINGS =====
        settings.spectrum_mode = ::SpectrumMode::MEDIUM;
        settings.hardware_bandwidth_hz = 24000000;
        settings.enable_real_hardware = true;
        settings.demo_mode = false;
        settings.iq_calibration_enabled = false;
        settings.rx_phase_value = 15;
        settings.lna_gain_db = 32;
        settings.vga_gain_db = 20;
        settings.rf_amp_enabled = false;
        settings.user_min_freq_hz = 50000000ULL;
        settings.user_max_freq_hz = 6000000000ULL;

        // ===== SCANNING SETTINGS =====
        settings.scan_interval_ms = 1000;
        settings.rssi_threshold_db = -90;
        settings.enable_wideband_scanning = false;
        settings.wideband_min_freq_hz = 2400000000ULL;
        settings.wideband_max_freq_hz = 2500000000ULL;
        settings.wideband_slice_width_hz = 24000000;
        settings.panoramic_mode_enabled = true;
        settings.enable_intelligent_scanning = true;

        // ===== DETECTION SETTINGS =====
        settings.enable_fhss_detection = true;
        settings.movement_sensitivity = 3;
        settings.threat_level_threshold = 2;
        settings.min_detection_count = 3;
        settings.alert_persistence_threshold = 3;
        settings.enable_intelligent_tracking = true;

        // ===== LOGGING SETTINGS =====
        settings.auto_save_logs = true;
        settings.log_file_path = "/eda_logs";
        settings.log_format = "CSV";
        settings.max_log_file_size_kb = 1024;
        settings.enable_session_logging = true;
        settings.include_timestamp = true;
        settings.include_rssi_values = true;

        // ===== DISPLAY SETTINGS =====
        settings.color_scheme = "DARK";
        settings.font_size = 0;
        settings.spectrum_density = 1;
        settings.waterfall_speed = 5;
        settings.show_detailed_info = true;
        settings.show_mini_spectrum = true;
        settings.show_rssi_history = true;
        settings.show_frequency_ruler = true;
        settings.frequency_ruler_style = 5;
        settings.compact_ruler_tick_count = 4;
        settings.auto_ruler_style = true;

        // ===== PROFILE SETTINGS =====
        settings.current_profile_name = "Default";
        settings.enable_quick_profiles = true;
        settings.auto_save_on_change = false;

        // ===== SYSTEM SETTINGS =====
        settings.freqman_path = "DRONES";
        settings.settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        settings.settings_version = 2;
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
        
        // ===== AUDIO SETTINGS =====
        if (strcmp(key, "enable_audio_alerts") == 0) {
            settings.enable_audio_alerts = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "audio_alert_frequency_hz") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.audio_alert_frequency_hz = validate_range<uint32_t>(temp, DroneConstants::MIN_AUDIO_FREQ, DroneConstants::MAX_AUDIO_FREQ);
            return true;
        }
        else if (strcmp(key, "audio_alert_duration_ms") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.audio_alert_duration_ms = validate_range<uint32_t>(temp, DroneConstants::MIN_AUDIO_DURATION, DroneConstants::MAX_AUDIO_DURATION);
            return true;
        }
        else if (strcmp(key, "audio_volume_level") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.audio_volume_level = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 100);
            return true;
        }
        else if (strcmp(key, "audio_repeat_alerts") == 0) {
            settings.audio_repeat_alerts = (strcmp(value, "true") == 0);
            return true;
        }
        
        // ===== HARDWARE SETTINGS =====
        else if (strcmp(key, "spectrum_mode") == 0) {
            settings.spectrum_mode = parse_spectrum_mode(value);
            return true;
        }
        else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.hardware_bandwidth_hz = validate_range<uint32_t>(temp, DroneConstants::MIN_BANDWIDTH, DroneConstants::MAX_BANDWIDTH);
            return true;
        }
        else if (strcmp(key, "enable_real_hardware") == 0) {
            settings.enable_real_hardware = (strcmp(value, "true") == 0);
            settings.demo_mode = !settings.enable_real_hardware; // Sync flags
            return true;
        }
        else if (strcmp(key, "iq_calibration_enabled") == 0) {
            settings.iq_calibration_enabled = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "rx_phase_value") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.rx_phase_value = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 63);
            return true;
        }
        else if (strcmp(key, "lna_gain_db") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.lna_gain_db = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 63);
            return true;
        }
        else if (strcmp(key, "vga_gain_db") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.vga_gain_db = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 62);
            return true;
        }
        else if (strcmp(key, "rf_amp_enabled") == 0) {
            settings.rf_amp_enabled = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "user_min_freq_hz") == 0) {
            uint64_t temp;
            if (!safe_parse_uint64(value, &temp)) return false;
            settings.user_min_freq_hz = temp;
            return true;
        }
        else if (strcmp(key, "user_max_freq_hz") == 0) {
            uint64_t temp;
            if (!safe_parse_uint64(value, &temp)) return false;
            // Validate that max > min
            if (temp <= settings.user_min_freq_hz) {
                return false;
            }
            settings.user_max_freq_hz = temp;
            return true;
        }
        
        // ===== SCANNING SETTINGS =====
        else if (strcmp(key, "scan_interval_ms") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.scan_interval_ms = validate_range<uint32_t>(temp, DroneConstants::MIN_SCAN_INTERVAL_MS, DroneConstants::MAX_SCAN_INTERVAL_MS);
            return true;
        }
        else if (strcmp(key, "rssi_threshold_db") == 0) {
            int32_t temp;
            if (!safe_parse_int32(value, &temp)) return false;
            settings.rssi_threshold_db = validate_range<int32_t>(temp, -120, -30);
            return true;
        }
        else if (strcmp(key, "enable_wideband_scanning") == 0) {
            settings.enable_wideband_scanning = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "wideband_min_freq_hz") == 0) {
            uint64_t temp;
            if (!safe_parse_uint64(value, &temp)) return false;
            settings.wideband_min_freq_hz = temp;
            return true;
        }
        else if (strcmp(key, "wideband_max_freq_hz") == 0) {
            uint64_t temp;
            if (!safe_parse_uint64(value, &temp)) return false;
            settings.wideband_max_freq_hz = temp;
            return true;
        }
        else if (strcmp(key, "wideband_slice_width_hz") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.wideband_slice_width_hz = temp;
            return true;
        }
        else if (strcmp(key, "panoramic_mode_enabled") == 0) {
            settings.panoramic_mode_enabled = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "enable_intelligent_scanning") == 0) {
            settings.enable_intelligent_scanning = (strcmp(value, "true") == 0);
            return true;
        }

        // ===== DETECTION SETTINGS =====
        else if (strcmp(key, "enable_fhss_detection") == 0) {
            settings.enable_fhss_detection = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "movement_sensitivity") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.movement_sensitivity = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 3);
            return true;
        }
        else if (strcmp(key, "threat_level_threshold") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.threat_level_threshold = validate_range<uint32_t>(temp, 0, 4);
            return true;
        }
        else if (strcmp(key, "min_detection_count") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.min_detection_count = validate_range<uint8_t>(static_cast<uint8_t>(temp), 1, 10);
            return true;
        }
        else if (strcmp(key, "alert_persistence_threshold") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.alert_persistence_threshold = validate_range<uint32_t>(temp, 1, 10);
            return true;
        }
        else if (strcmp(key, "enable_intelligent_tracking") == 0) {
            settings.enable_intelligent_tracking = (strcmp(value, "true") == 0);
            return true;
        }
        
        // ===== LOGGING SETTINGS =====
        else if (strcmp(key, "auto_save_logs") == 0) {
            settings.auto_save_logs = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "log_file_path") == 0) {
            settings.log_file_path = value;
            return true;
        }
        else if (strcmp(key, "log_format") == 0) {
            settings.log_format = value;
            return true;
        }
        else if (strcmp(key, "max_log_file_size_kb") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.max_log_file_size_kb = validate_range<uint32_t>(temp, 100, 10240);
            return true;
        }
        else if (strcmp(key, "enable_session_logging") == 0) {
            settings.enable_session_logging = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "include_timestamp") == 0) {
            settings.include_timestamp = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "include_rssi_values") == 0) {
            settings.include_rssi_values = (strcmp(value, "true") == 0);
            return true;
        }
        
        // ===== DISPLAY SETTINGS =====
        else if (strcmp(key, "color_scheme") == 0) {
            settings.color_scheme = value;
            return true;
        }
        else if (strcmp(key, "font_size") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.font_size = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 2);
            return true;
        }
        else if (strcmp(key, "spectrum_density") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.spectrum_density = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 2);
            return true;
        }
        else if (strcmp(key, "waterfall_speed") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.waterfall_speed = validate_range<uint8_t>(static_cast<uint8_t>(temp), 1, 10);
            return true;
        }
        else if (strcmp(key, "show_detailed_info") == 0) {
            settings.show_detailed_info = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "show_mini_spectrum") == 0) {
            settings.show_mini_spectrum = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "show_rssi_history") == 0) {
            settings.show_rssi_history = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "show_frequency_ruler") == 0) {
            settings.show_frequency_ruler = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "frequency_ruler_style") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.frequency_ruler_style = validate_range<uint8_t>(static_cast<uint8_t>(temp), 0, 6);
            return true;
        }
        else if (strcmp(key, "compact_ruler_tick_count") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.compact_ruler_tick_count = validate_range<uint8_t>(static_cast<uint8_t>(temp), 3, 5);
            return true;
        }
        else if (strcmp(key, "auto_ruler_style") == 0) {
            settings.auto_ruler_style = (strcmp(value, "true") == 0);
            return true;
        }
        
        // ===== PROFILE SETTINGS =====
        else if (strcmp(key, "current_profile_name") == 0) {
            settings.current_profile_name = value;
            return true;
        }
        else if (strcmp(key, "enable_quick_profiles") == 0) {
            settings.enable_quick_profiles = (strcmp(value, "true") == 0);
            return true;
        }
        else if (strcmp(key, "auto_save_on_change") == 0) {
            settings.auto_save_on_change = (strcmp(value, "true") == 0);
            return true;
        }
        
        // ===== SYSTEM SETTINGS =====
        else if (strcmp(key, "freqman_path") == 0) {
            settings.freqman_path = value;
            return true;
        }
        else if (strcmp(key, "settings_file_path") == 0) {
            settings.settings_file_path = value;
            return true;
        }
        else if (strcmp(key, "settings_version") == 0) {
            uint32_t temp;
            if (!safe_parse_uint32(value, &temp)) return false;
            settings.settings_version = temp;
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
             reset_to_defaults(settings);
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

    void write_setting_line(File& file, const std::string& key, const std::string& value) {
        file.write(key.data(), key.length());
        file.write("=", 1);
        file.write(value.data(), value.length());
        file.write("\r\n", 2);
    }

    bool save_settings_to_txt(const ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings& settings) {
        File file;
        if (!file.open("/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt", false)) {
            return false;
        }

        std::string header = "# Enhanced Drone Analyzer Settings v2.0 (Unified)\r\n";
        auto result = file.write(header.data(), header.size());
        if (result.is_error() || result.value() != header.size()) {
            file.close();
            return false;
        }

        // ===== AUDIO SETTINGS =====
        write_setting_line(file, "enable_audio_alerts", settings.enable_audio_alerts ? "true" : "false");
        write_setting_line(file, "audio_alert_frequency_hz", to_string_dec_uint(settings.audio_alert_frequency_hz));
        write_setting_line(file, "audio_alert_duration_ms", to_string_dec_uint(settings.audio_alert_duration_ms));
        write_setting_line(file, "audio_volume_level", to_string_dec_uint(settings.audio_volume_level));
        write_setting_line(file, "audio_repeat_alerts", settings.audio_repeat_alerts ? "true" : "false");

        // ===== HARDWARE SETTINGS =====
        write_setting_line(file, "spectrum_mode", std::to_string(static_cast<int>(settings.spectrum_mode)));
        write_setting_line(file, "hardware_bandwidth_hz", to_string_dec_uint(settings.hardware_bandwidth_hz));
        write_setting_line(file, "enable_real_hardware", settings.enable_real_hardware ? "true" : "false");
        write_setting_line(file, "demo_mode", settings.demo_mode ? "true" : "false");
        write_setting_line(file, "iq_calibration_enabled", settings.iq_calibration_enabled ? "true" : "false");
        write_setting_line(file, "rx_phase_value", to_string_dec_uint(settings.rx_phase_value));
        write_setting_line(file, "lna_gain_db", to_string_dec_uint(settings.lna_gain_db));
        write_setting_line(file, "vga_gain_db", to_string_dec_uint(settings.vga_gain_db));
        write_setting_line(file, "rf_amp_enabled", settings.rf_amp_enabled ? "true" : "false");
        write_setting_line(file, "user_min_freq_hz", to_string_dec_uint(settings.user_min_freq_hz));
        write_setting_line(file, "user_max_freq_hz", to_string_dec_uint(settings.user_max_freq_hz));

        // ===== SCANNING SETTINGS =====
        write_setting_line(file, "scan_interval_ms", to_string_dec_uint(settings.scan_interval_ms));
        write_setting_line(file, "rssi_threshold_db", to_string_dec_int(settings.rssi_threshold_db));
        write_setting_line(file, "enable_wideband_scanning", settings.enable_wideband_scanning ? "true" : "false");
        write_setting_line(file, "wideband_min_freq_hz", to_string_dec_uint(settings.wideband_min_freq_hz));
        write_setting_line(file, "wideband_max_freq_hz", to_string_dec_uint(settings.wideband_max_freq_hz));
        write_setting_line(file, "wideband_slice_width_hz", to_string_dec_uint(settings.wideband_slice_width_hz));
        write_setting_line(file, "panoramic_mode_enabled", settings.panoramic_mode_enabled ? "true" : "false");
        write_setting_line(file, "enable_intelligent_scanning", settings.enable_intelligent_scanning ? "true" : "false");

        // ===== DETECTION SETTINGS =====
        write_setting_line(file, "enable_fhss_detection", settings.enable_fhss_detection ? "true" : "false");
        write_setting_line(file, "movement_sensitivity", to_string_dec_uint(settings.movement_sensitivity));
        write_setting_line(file, "threat_level_threshold", to_string_dec_uint(settings.threat_level_threshold));
        write_setting_line(file, "min_detection_count", to_string_dec_uint(settings.min_detection_count));
        write_setting_line(file, "alert_persistence_threshold", to_string_dec_uint(settings.alert_persistence_threshold));
        write_setting_line(file, "enable_intelligent_tracking", settings.enable_intelligent_tracking ? "true" : "false");

        // ===== LOGGING SETTINGS =====
        write_setting_line(file, "auto_save_logs", settings.auto_save_logs ? "true" : "false");
        write_setting_line(file, "log_file_path", settings.log_file_path);
        write_setting_line(file, "log_format", settings.log_format);
        write_setting_line(file, "max_log_file_size_kb", to_string_dec_uint(settings.max_log_file_size_kb));
        write_setting_line(file, "enable_session_logging", settings.enable_session_logging ? "true" : "false");
        write_setting_line(file, "include_timestamp", settings.include_timestamp ? "true" : "false");
        write_setting_line(file, "include_rssi_values", settings.include_rssi_values ? "true" : "false");

        // ===== DISPLAY SETTINGS =====
        write_setting_line(file, "color_scheme", settings.color_scheme);
        write_setting_line(file, "font_size", to_string_dec_uint(settings.font_size));
        write_setting_line(file, "spectrum_density", to_string_dec_uint(settings.spectrum_density));
        write_setting_line(file, "waterfall_speed", to_string_dec_uint(settings.waterfall_speed));
        write_setting_line(file, "show_detailed_info", settings.show_detailed_info ? "true" : "false");
        write_setting_line(file, "show_mini_spectrum", settings.show_mini_spectrum ? "true" : "false");
        write_setting_line(file, "show_rssi_history", settings.show_rssi_history ? "true" : "false");
        write_setting_line(file, "show_frequency_ruler", settings.show_frequency_ruler ? "true" : "false");
        write_setting_line(file, "frequency_ruler_style", to_string_dec_uint(settings.frequency_ruler_style));
        write_setting_line(file, "compact_ruler_tick_count", to_string_dec_uint(settings.compact_ruler_tick_count));
        write_setting_line(file, "auto_ruler_style", settings.auto_ruler_style ? "true" : "false");

        // ===== PROFILE SETTINGS =====
        write_setting_line(file, "current_profile_name", settings.current_profile_name);
        write_setting_line(file, "enable_quick_profiles", settings.enable_quick_profiles ? "true" : "false");
        write_setting_line(file, "auto_save_on_change", settings.auto_save_on_change ? "true" : "false");

        // ===== SYSTEM SETTINGS =====
        write_setting_line(file, "freqman_path", settings.freqman_path);
        write_setting_line(file, "settings_file_path", settings.settings_file_path);
        write_setting_line(file, "settings_version", to_string_dec_uint(settings.settings_version));

        file.close();
        return true;
    }

}  // namespace ScannerSettingsManager
