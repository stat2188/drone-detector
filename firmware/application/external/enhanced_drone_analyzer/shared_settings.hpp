/*
 * shared_settings.hpp - Shared settings infrastructure for Enhanced Drone Analyzer
 * This header provides common settings definitions and functions for both apps
 */

#pragma once

#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace ui::external_app::enhanced_drone_analyzer {

// Language enum for localization
enum class Language {
    ENGLISH,
    RUSSIAN
};

// Spectrum mode enum
enum class SpectrumMode {
    LOW,
    MEDIUM,
    HIGH
};

// Settings structure
struct DroneAnalyzerSettings {
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t scan_interval_ms = 1000;
    uint32_t max_frequency_hz = 6000000000ULL;
    bool enable_real_hardware = true;
    bool enable_audio_alerts = true;
    uint32_t audio_alert_duration_ms = 1000;
    bool enable_wideband_scanning = false;
};

// EnhancedSettingsManager - Shared implementation
class EnhancedSettingsManager {
public:
    // Save settings to text file
    static bool save_settings_to_txt(const DroneAnalyzerSettings& settings);
    
    // Verify communication file exists
    static bool verify_comm_file_exists();
    
    // Get communication status
    static std::string get_communication_status();
    
    // Ensure drone database exists
    static void ensure_database_exists();
    
    // Create backup file
    static void create_backup_file(const std::string& filepath);
    
    // Restore from backup
    static void restore_from_backup(const std::string& filepath);
    
    // Remove backup file
    static void remove_backup_file(const std::string& filepath);
    
    // Generate file header
    static std::string generate_file_header();
    
    // Generate settings content
    static std::string generate_settings_content(const DroneAnalyzerSettings& settings);
    
    // Convert spectrum mode to string
    static std::string spectrum_mode_to_string(SpectrumMode mode);
    
    // Get current timestamp
    static std::string get_current_timestamp();
};

// DroneAnalyzerSettingsManager - Settings management
class DroneAnalyzerSettingsManager {
public:
    // Load settings from text file
    static bool load(DroneAnalyzerSettings& settings);
    
    // Save settings to text file
    static bool save(const DroneAnalyzerSettings& settings);
    
    // Reset to default settings
    static void reset_to_defaults(DroneAnalyzerSettings& settings);
    
    // Validate settings
    static bool validate(const DroneAnalyzerSettings& settings);
    
    // Serialize settings to string
    static std::string serialize(const DroneAnalyzerSettings& settings);
    
    // Deserialize settings from string
    static bool deserialize(DroneAnalyzerSettings& settings, const std::string& data);
    
    // Translate strings (for localization)
    static const char* translate(const std::string& key);
    static const char* get_translation(const std::string& key);

private:
    // Current language
    static Language current_language_;
    
    // English translations
    static const std::map<std::string, const char*> translations_english;
};

} // namespace ui::external_app::enhanced_drone_analyzer
