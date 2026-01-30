/*
 * EDA Unified Settings Manager Implementation
 */

#include "eda_unified_settings_manager.hpp"
#include "file.hpp"
#include "string_format.hpp"
#include "file_reader.hpp"

#include <cstring>
#include <cstdlib>

using namespace ui;

namespace ui::apps::enhanced_drone_analyzer {

bool EDAUnifiedSettingsManager::parse_setting(
    const std::string& line,
    std::string& key,
    std::string& value) {

    size_t equals_pos = line.find('=');
    if (equals_pos == std::string::npos || equals_pos == 0) {
        return false;
    }

    key = line.substr(0, equals_pos);
    value = line.substr(equals_pos + 1);

    return true;
}

bool EDAUnifiedSettingsManager::load(EDAUnifiedSettings& settings) {
    File settings_file;
    auto error = settings_file.open(SETTINGS_FILENAME);
    if (error) {
        reset_to_defaults(settings);
        return false;
    }

    std::string line;
    char buffer[256];
    size_t total_read = 0;

    while (total_read < settings_file.size()) {
        size_t to_read = sizeof(buffer) - 1;
        if (to_read > settings_file.size() - total_read) {
            to_read = settings_file.size() - total_read;
        }

        auto read_result = settings_file.read(buffer, to_read);
        if (read_result.is_error()) break;

        buffer[read_result.value()] = '\0';
        line += buffer;
        total_read += read_result.value();

        size_t newline_pos;
        while ((newline_pos = line.find('\n')) != std::string::npos) {
            std::string current_line = line.substr(0, newline_pos);
            line = line.substr(newline_pos + 1);

            std::string key, value;
            if (!parse_setting(current_line, key, value)) {
                continue;
            }

            if (key.empty() || key[0] == '#') continue;

            if (key == "audio_enabled") {
                settings.audio.audio_enabled = (value == "1" || value == "true");
            } else if (key == "alert_frequency_hz") {
                settings.audio.alert_frequency_hz = std::stoul(value);
            } else if (key == "alert_duration_ms") {
                settings.audio.alert_duration_ms = std::stoul(value);
            } else if (key == "volume_level") {
                settings.audio.volume_level = static_cast<uint8_t>(std::stoul(value));
            } else if (key == "repeat_alerts") {
                settings.audio.repeat_alerts = (value == "1" || value == "true");
            }

            else if (key == "enable_real_hardware") {
                settings.hardware.enable_real_hardware = (value == "1" || value == "true");
            } else if (key == "spectrum_mode") {
                if (value == "NARROW") settings.hardware.spectrum_mode = SpectrumMode::NARROW;
                else if (value == "MEDIUM") settings.hardware.spectrum_mode = SpectrumMode::MEDIUM;
                else if (value == "WIDE") settings.hardware.spectrum_mode = SpectrumMode::WIDE;
                else if (value == "ULTRA_WIDE") settings.hardware.spectrum_mode = SpectrumMode::ULTRA_WIDE;
                else if (value == "ULTRA_NARROW") settings.hardware.spectrum_mode = SpectrumMode::ULTRA_NARROW;
            } else if (key == "hardware_bandwidth_hz") {
                settings.hardware.hardware_bandwidth_hz = std::stoul(value);
            } else if (key == "user_min_freq_hz") {
                settings.hardware.user_min_freq_hz = std::stoull(value);
            } else if (key == "user_max_freq_hz") {
                settings.hardware.user_max_freq_hz = std::stoull(value);
            }

            else if (key == "scan_interval_ms") {
                settings.scanning.scan_interval_ms = std::stoul(value);
            } else if (key == "rssi_threshold_db") {
                settings.scanning.rssi_threshold_db = std::stol(value);
            } else if (key == "enable_wideband_scanning") {
                settings.scanning.enable_wideband_scanning = (value == "1" || value == "true");
            } else if (key == "wideband_min_freq_hz") {
                settings.scanning.wideband_min_freq_hz = std::stoull(value);
            } else if (key == "wideband_max_freq_hz") {
                settings.scanning.wideband_max_freq_hz = std::stoull(value);
            }

            else if (key == "enable_fhss_detection") {
                settings.detection.enable_fhss_detection = (value == "1" || value == "true");
            } else if (key == "movement_sensitivity") {
                settings.detection.movement_sensitivity = static_cast<uint8_t>(std::stoul(value));
            } else if (key == "threat_level_threshold") {
                settings.detection.threat_level_threshold = std::stoul(value);
            }

            else if (key == "auto_save_logs") {
                settings.logging.auto_save_logs = (value == "1" || value == "true");
            } else if (key == "log_file_path") {
                settings.logging.log_file_path = value;
            } else if (key == "log_format") {
                settings.logging.log_format = value;
            } else if (key == "max_log_file_size_kb") {
                settings.logging.max_log_file_size_kb = std::stoul(value);
            }

            else if (key == "color_scheme") {
                settings.display.color_scheme = value;
            } else if (key == "font_size") {
                settings.display.font_size = static_cast<uint8_t>(std::stoul(value));
            } else if (key == "spectrum_density") {
                settings.display.spectrum_density = static_cast<uint8_t>(std::stoul(value));
            } else if (key == "waterfall_speed") {
                settings.display.waterfall_speed = static_cast<uint8_t>(std::stoul(value));
            }

            else if (key == "settings_version") {
                settings.settings_version = std::stoul(value);
            }
        }
    }

    settings_file.close();

    if (!settings.is_valid()) {
        reset_to_defaults(settings);
        return false;
    }

    return true;
}

void EDAUnifiedSettingsManager::write_section_header(File& file, const char* section_name) {
    auto header = std::string("\n# ===== ") + section_name + " =====\n";
    file.write(header.data(), header.length());
}

void EDAUnifiedSettingsManager::write_setting(File& file, const char* key, const std::string& value) {
    auto line = std::string(key) + "=" + value + "\n";
    file.write(line.data(), line.length());
}

void EDAUnifiedSettingsManager::write_setting(File& file, const char* key, uint32_t value) {
    auto value_str = to_string_dec_uint(value);
    write_setting(file, key, value_str);
}

void EDAUnifiedSettingsManager::write_setting(File& file, const char* key, int32_t value) {
    auto value_str = to_string_dec_int(value);
    write_setting(file, key, value_str);
}

void EDAUnifiedSettingsManager::write_setting(File& file, const char* key, bool value) {
    write_setting(file, key, value ? "1" : "0");
}

void EDAUnifiedSettingsManager::write_setting(File& file, const char* key, uint64_t value) {
    auto value_str = to_string_dec_uint(value);
    write_setting(file, key, value_str);
}

bool EDAUnifiedSettingsManager::save(const EDAUnifiedSettings& settings) {
    if (!settings.is_valid()) {
        return false;
    }

    File settings_file;
    auto error = settings_file.create(SETTINGS_FILENAME);
    if (error) {
        return false;
    }

    auto header = SETTINGS_HEADER;
    settings_file.write(header, strlen(header));

    write_section_header(settings_file, "Audio Settings");
    write_setting(settings_file, "audio_enabled", settings.audio.audio_enabled);
    write_setting(settings_file, "alert_frequency_hz", settings.audio.alert_frequency_hz);
    write_setting(settings_file, "alert_duration_ms", settings.audio.alert_duration_ms);
    write_setting(settings_file, "volume_level", settings.audio.volume_level);
    write_setting(settings_file, "repeat_alerts", settings.audio.repeat_alerts);

    write_section_header(settings_file, "Hardware Settings");
    write_setting(settings_file, "enable_real_hardware", settings.hardware.enable_real_hardware);
    const char* mode_str = "MEDIUM";
    switch (settings.hardware.spectrum_mode) {
        case SpectrumMode::NARROW: mode_str = "NARROW"; break;
        case SpectrumMode::MEDIUM: mode_str = "MEDIUM"; break;
        case SpectrumMode::WIDE: mode_str = "WIDE"; break;
        case SpectrumMode::ULTRA_WIDE: mode_str = "ULTRA_WIDE"; break;
        case SpectrumMode::ULTRA_NARROW: mode_str = "ULTRA_NARROW"; break;
    }
    write_setting(settings_file, "spectrum_mode", mode_str);
    write_setting(settings_file, "hardware_bandwidth_hz", settings.hardware.hardware_bandwidth_hz);
    write_setting(settings_file, "user_min_freq_hz", settings.hardware.user_min_freq_hz);
    write_setting(settings_file, "user_max_freq_hz", settings.hardware.user_max_freq_hz);

    write_section_header(settings_file, "Scanning Settings");
    write_setting(settings_file, "scan_interval_ms", settings.scanning.scan_interval_ms);
    write_setting(settings_file, "rssi_threshold_db", settings.scanning.rssi_threshold_db);
    write_setting(settings_file, "enable_wideband_scanning", settings.scanning.enable_wideband_scanning);
    write_setting(settings_file, "wideband_min_freq_hz", settings.scanning.wideband_min_freq_hz);
    write_setting(settings_file, "wideband_max_freq_hz", settings.scanning.wideband_max_freq_hz);

    write_section_header(settings_file, "Detection Settings");
    write_setting(settings_file, "enable_fhss_detection", settings.detection.enable_fhss_detection);
    write_setting(settings_file, "movement_sensitivity", settings.detection.movement_sensitivity);
    write_setting(settings_file, "threat_level_threshold", settings.detection.threat_level_threshold);

    write_section_header(settings_file, "Logging Settings");
    write_setting(settings_file, "auto_save_logs", settings.logging.auto_save_logs);
    write_setting(settings_file, "log_file_path", settings.logging.log_file_path);
    write_setting(settings_file, "log_format", settings.logging.log_format);
    write_setting(settings_file, "max_log_file_size_kb", settings.logging.max_log_file_size_kb);

    write_section_header(settings_file, "Display Settings");
    write_setting(settings_file, "color_scheme", settings.display.color_scheme);
    write_setting(settings_file, "font_size", settings.display.font_size);
    write_setting(settings_file, "spectrum_density", settings.display.spectrum_density);
    write_setting(settings_file, "waterfall_speed", settings.display.waterfall_speed);

    write_section_header(settings_file, "Profile Settings");
    write_setting(settings_file, "current_profile_name", settings.profile.current_profile_name);
    write_setting(settings_file, "enable_quick_profiles", settings.profile.enable_quick_profiles);

    write_section_header(settings_file, "Version");
    write_setting(settings_file, "settings_version", settings.settings_version);

    settings_file.close();
    return true;
}

void EDAUnifiedSettingsManager::reset_to_defaults(EDAUnifiedSettings& settings) {
    settings.reset_to_defaults();
}

bool EDAUnifiedSettingsManager::validate(const EDAUnifiedSettings& settings) {
    return settings.is_valid();
}

} // namespace ui::apps::enhanced_drone_analyzer
