/*
 * shared_settings.cpp - Shared settings implementation for Enhanced Drone Analyzer
 * This file provides common settings functionality for both apps
 */

#include "shared_settings.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "string_format.hpp"
#include <ctime>
#include <iomanip>

namespace ui::external_app::enhanced_drone_analyzer {

// Static member initialization
Language DroneAnalyzerSettingsManager::current_language_ = Language::ENGLISH;

// English translations
const std::map<std::string, const char*> DroneAnalyzerSettingsManager::translations_english = {
    {"save_settings", "Save Settings"},
    {"load_settings", "Load Settings"},
    {"reset_defaults", "Reset to Defaults"},
    {"communication_error", "Communication Error"},
    {"file_not_found", "Settings file not found"},
    {"invalid_format", "Invalid file format"},
    {"save_success", "Settings saved successfully"},
    {"load_success", "Settings loaded successfully"},
    {"reset_success", "Settings reset to defaults"}
};

// EnhancedSettingsManager implementations
bool EnhancedSettingsManager::save_settings_to_txt(const DroneAnalyzerSettings& settings) {
    const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    
    try {
        // Generate content
        std::string content = generate_file_header() + generate_settings_content(settings);
        
        // Write to file
        File settings_file;
        if (!settings_file.open(filepath, File::Mode::Write)) {
            return false;
        }
        
        settings_file.write(content.c_str(), content.length());
        settings_file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

bool EnhancedSettingsManager::verify_comm_file_exists() {
    const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    return std::filesystem::exists(filepath);
}

std::string EnhancedSettingsManager::get_communication_status() {
    if (verify_comm_file_exists()) {
        return "Communication: OK";
    } else {
        return "Communication: ERROR";
    }
}

void EnhancedSettingsManager::ensure_database_exists() {
    const std::string file_path = "/FREQMAN/DRONES.TXT";
    
    // Check if file exists
    if (std::filesystem::exists(file_path)) {
        return;
    }
    
    // Create default database
    try {
        File db_file;
        if (!db_file.open(file_path, File::Mode::Write)) {
            return;
        }
        
        // Write default drone database content
        std::string default_db = R"(# Enhanced Drone Analyzer - Default Drone Database
# Format: Frequency(Hz), Bandwidth(Hz), Modulation, Drone Type, Description
# Example entries:

# DJI Mavic Pro
5725000000, 40000000, OFDM, DJI Mavic Pro, Video Downlink
2412000000, 20000000, OFDM, DJI Mavic Pro, Control Link

# DJI Phantom 4
5760000000, 40000000, OFDM, DJI Phantom 4, Video Downlink
2437000000, 20000000, OFDM, DJI Phantom 4, Control Link

# Parrot Bebop
5805000000, 40000000, OFDM, Parrot Bebop, Video Downlink
2452000000, 20000000, OFDM, Parrot Bebop, Control Link

# Autel EVO
5855000000, 40000000, OFDM, Autel EVO, Video Downlink
2484000000, 20000000, OFDM, Autel EVO, Control Link

# Skydio X2
5905000000, 40000000, OFDM, Skydio X2, Video Downlink
2517000000, 20000000, OFDM, Skydio X2, Control Link
)";
        
        db_file.write(default_db.c_str(), default_db.length());
        db_file.close();
    } catch (...) {
        // Handle error silently
    }
}

void EnhancedSettingsManager::create_backup_file(const std::string& filepath) {
    const std::string backup_path = filepath + ".bak";
    
    try {
        if (std::filesystem::exists(filepath)) {
            std::filesystem::copy_file(filepath, backup_path, std::filesystem::copy_options::overwrite_existing);
        }
    } catch (...) {
        // Handle error silently
    }
}

void EnhancedSettingsManager::restore_from_backup(const std::string& filepath) {
    const std::string backup_path = filepath + ".bak";
    
    try {
        if (std::filesystem::exists(backup_path)) {
            std::filesystem::copy_file(backup_path, filepath, std::filesystem::copy_options::overwrite_existing);
        }
    } catch (...) {
        // Handle error silently
    }
}

void EnhancedSettingsManager::remove_backup_file(const std::string& filepath) {
    const std::string backup_path = filepath + ".bak";
    
    try {
        if (std::filesystem::exists(backup_path)) {
            std::filesystem::remove(backup_path);
        }
    } catch (...) {
        // Handle error silently
    }
}

std::string EnhancedSettingsManager::generate_file_header() {
    std::stringstream ss;
    ss << "# Enhanced Drone Analyzer Settings\n";
    ss << "# Generated on: " << get_current_timestamp() << "\n";
    ss << "# Format: KEY=VALUE\n";
    ss << "\n";
    return ss.str();
}

std::string EnhancedSettingsManager::generate_settings_content(const DroneAnalyzerSettings& settings) {
    std::stringstream ss;
    
    ss << "SPECTRUM_MODE=" << EnhancedSettingsManager::spectrum_mode_to_string(settings.spectrum_mode) << "\n";
    ss << "SCAN_INTERVAL_MS=" << settings.scan_interval_ms << "\n";
    ss << "MAX_FREQUENCY_HZ=" << settings.max_frequency_hz << "\n";
    ss << "ENABLE_REAL_HARDWARE=" << (settings.enable_real_hardware ? "true" : "false") << "\n";
    ss << "ENABLE_AUDIO_ALERTS=" << (settings.enable_audio_alerts ? "true" : "false") << "\n";
    ss << "AUDIO_ALERT_DURATION_MS=" << settings.audio_alert_duration_ms << "\n";
    ss << "ENABLE_WIDEBAND_SCANNING=" << (settings.enable_wideband_scanning ? "true" : "false") << "\n";
    
    return ss.str();
}

std::string EnhancedSettingsManager::spectrum_mode_to_string(SpectrumMode mode) {
    switch (mode) {
        case SpectrumMode::LOW: return "LOW";
        case SpectrumMode::MEDIUM: return "MEDIUM";
        case SpectrumMode::HIGH: return "HIGH";
        default: return "MEDIUM";
    }
}

std::string EnhancedSettingsManager::get_current_timestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// DroneAnalyzerSettingsManager implementations
bool DroneAnalyzerSettingsManager::load(DroneAnalyzerSettings& settings) {
    bool loaded = false;
    
    // Try to load from text file
    File settings_file;
    const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    
    if (settings_file.open(filepath, File::Mode::Read)) {
        std::string content;
        content.resize(settings_file.size());
        settings_file.read(content.data(), content.size());
        settings_file.close();
        
        // Parse content
        std::istringstream iss(content);
        std::string line;
        
        while (std::getline(iss, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;
            
            size_t equals_pos = line.find('=');
            if (equals_pos != std::string::npos) {
                std::string key = line.substr(0, equals_pos);
                std::string value = line.substr(equals_pos + 1);
                
                // Parse key-value pairs
                if (key == "SPECTRUM_MODE") {
                    if (value == "LOW") settings.spectrum_mode = SpectrumMode::LOW;
                    else if (value == "MEDIUM") settings.spectrum_mode = SpectrumMode::MEDIUM;
                    else if (value == "HIGH") settings.spectrum_mode = SpectrumMode::HIGH;
                } else if (key == "SCAN_INTERVAL_MS") {
                    settings.scan_interval_ms = std::stoul(value);
                } else if (key == "MAX_FREQUENCY_HZ") {
                    settings.max_frequency_hz = std::stoull(value);
                } else if (key == "ENABLE_REAL_HARDWARE") {
                    settings.enable_real_hardware = (value == "true");
                } else if (key == "ENABLE_AUDIO_ALERTS") {
                    settings.enable_audio_alerts = (value == "true");
                } else if (key == "AUDIO_ALERT_DURATION_MS") {
                    settings.audio_alert_duration_ms = std::stoul(value);
                } else if (key == "ENABLE_WIDEBAND_SCANNING") {
                    settings.enable_wideband_scanning = (value == "true");
                }
            }
        }
        
        loaded = true;
    }
    
    if (!loaded) {
        reset_to_defaults(settings);
    }
    
    return loaded;
}

bool DroneAnalyzerSettingsManager::save(const DroneAnalyzerSettings& settings) {
    return EnhancedSettingsManager::save_settings_to_txt(settings);
}

void DroneAnalyzerSettingsManager::reset_to_defaults(DroneAnalyzerSettings& settings) {
    settings.spectrum_mode = SpectrumMode::MEDIUM;
    settings.scan_interval_ms = 1000;
    settings.max_frequency_hz = 6000000000ULL;
    settings.enable_real_hardware = true;
    settings.enable_audio_alerts = true;
    settings.audio_alert_duration_ms = 1000;
    settings.enable_wideband_scanning = false;
}

bool DroneAnalyzerSettingsManager::validate(const DroneAnalyzerSettings& settings) {
    if (settings.scan_interval_ms < 100 || settings.scan_interval_ms > 10000) return false;
    if (settings.max_frequency_hz < 1000000 || settings.max_frequency_hz > 6000000000ULL) return false;
    if (settings.audio_alert_duration_ms < 100 || settings.audio_alert_duration_ms > 10000) return false;
    return true;
}

std::string DroneAnalyzerSettingsManager::serialize(const DroneAnalyzerSettings& settings) {
    std::ostringstream oss;
    oss << "SPECTRUM_MODE:" << EnhancedSettingsManager::spectrum_mode_to_string(settings.spectrum_mode) << ";";
    oss << "SCAN_INTERVAL_MS:" << settings.scan_interval_ms << ";";
    oss << "MAX_FREQUENCY_HZ:" << settings.max_frequency_hz << ";";
    oss << "ENABLE_REAL_HARDWARE:" << (settings.enable_real_hardware ? "true" : "false") << ";";
    oss << "ENABLE_AUDIO_ALERTS:" << (settings.enable_audio_alerts ? "true" : "false") << ";";
    oss << "AUDIO_ALERT_DURATION_MS:" << settings.audio_alert_duration_ms << ";";
    oss << "ENABLE_WIDEBAND_SCANNING:" << (settings.enable_wideband_scanning ? "true" : "false");
    return oss.str();
}

bool DroneAnalyzerSettingsManager::deserialize(DroneAnalyzerSettings& settings, const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    
    while (std::getline(iss, token, ';')) {
        size_t colon_pos = token.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = token.substr(0, colon_pos);
            std::string value = token.substr(colon_pos + 1);
            
            if (key == "SPECTRUM_MODE") {
                if (value == "LOW") settings.spectrum_mode = SpectrumMode::LOW;
                else if (value == "MEDIUM") settings.spectrum_mode = SpectrumMode::MEDIUM;
                else if (value == "HIGH") settings.spectrum_mode = SpectrumMode::HIGH;
            } else if (key == "SCAN_INTERVAL_MS") {
                settings.scan_interval_ms = std::stoul(value);
            } else if (key == "MAX_FREQUENCY_HZ") {
                settings.max_frequency_hz = std::stoull(value);
            } else if (key == "ENABLE_REAL_HARDWARE") {
                settings.enable_real_hardware = (value == "true");
            } else if (key == "ENABLE_AUDIO_ALERTS") {
                settings.enable_audio_alerts = (value == "true");
            } else if (key == "AUDIO_ALERT_DURATION_MS") {
                settings.audio_alert_duration_ms = std::stoul(value);
            } else if (key == "ENABLE_WIDEBAND_SCANNING") {
                settings.enable_wideband_scanning = (value == "true");
            }
        }
    }
    
    return true;
}

const char* DroneAnalyzerSettingsManager::translate(const std::string& key) {
    auto it = translations_english.find(key);
    if (it != translations_english.end()) {
        return it->second;
    }
    return key.c_str();
}

const char* DroneAnalyzerSettingsManager::get_translation(const std::string& key) {
    return translate(key);
}

} // namespace ui::external_app::enhanced_drone_analyzer
