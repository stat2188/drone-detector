// ui_settings_combined.cpp - Unified implementation for Enhanced Drone Analyzer Settings App
// Phase 7: TXT file communication - settings writes configuration
// Combines implementations from Settings UI classes and manager classes

#include "ui_settings_combined.hpp"
#include "default_drones_db.hpp"
#include "file.hpp"          // Portapack file I/O for SD card access
#include "portapack.hpp"    // Portapack hardware access for navigation
#include "string_format.hpp" // For frequency formatting
#include <algorithm>        // For std::find_if_not in trim operations
#include <sstream>          // For stringstream parsing
#include <vector>           // For container operations
#include <memory>           // For unique_ptr management
#include <cstring>          // For strlen
#include <cstdlib>          // For strtoull

// Use ScannerSettingsManager for loading settings from TXT
using ScannerSettingsManager::load_settings_from_txt;

namespace ui::external_app::enhanced_drone_analyzer {

/**
 * PHASE 7: ENHANCED SETTINGS MANAGER WITH TXT FILE COMMUNICATION
 * Uses file.hpp API for robust SD card communication with scanner module
 */
class EnhancedSettingsManager {
public:
    /**
     * Save settings to TXT file at /sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt
     * Returns true on successful save, false on error
     */
    static bool save_settings_to_txt(const DroneAnalyzerSettings& settings) {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

        // Create backup for atomic write operation
        create_backup_file(filepath);

        // Attempt to open file for writing
        File settings_file;
        if (!settings_file.open(filepath, false)) {
            // SD card error
            return false;
        }

        auto& file = settings_file;

        // Write header with timestamp
        std::string header = generate_file_header();
        auto header_result = file.write(header.data(), header.size());
        if (header_result != header.size()) {
            file.close();
            // Restore from backup on error
            restore_from_backup(filepath);
            return false;
        }

        // Generate and write all settings
        std::string content = generate_settings_content(settings);
        auto content_result = file.write(content.data(), content.size());
        if (content_result != content.size()) {
            file.close();
            // Restore from backup on error
            restore_from_backup(filepath);
            return false;
        }

        file.close();

        // Remove backup on successful write
        remove_backup_file(filepath);

        return true;
    }

    /**
     * Verify TXT file exists and is readable by scanner module
     */
    static bool verify_comm_file_exists() {
        const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
        File txt_file;
        if (txt_file.open(filepath, true)) {  // true = read_only
            txt_file.close();
            return true;
        }
        return false;
    }

    /**
     * Get status string for communication testing
     */
    static std::string get_communication_status() {
        if (verify_comm_file_exists()) {
            return "✓ TXT file found\n✓ Communication ready";
        } else {
            return "✗ No TXT file found\n✗ Save settings first";
        }
    }

    /**
     * Ensures that the default drone frequency database exists on the SD card.
     * If not, it creates it from the embedded list.
     */
    static void ensure_database_exists() {
        // Путь к файлу. Обычно базы лежат в /FREQMAN/
        // Мы используем специфичное имя для дронов
        const std::string file_path = "/FREQMAN/DRONES.TXT";

        // 2. Проверяем, существует ли файл
        File check_file;
        if (check_file.open(file_path, true)) {
            // Файл существует и читается. Ничего делать не нужно.
            check_file.close();
            return;
        }

        // 3. Файла нет. Создаем и пишем дефолтные данные.
        File create_file;
        if (create_file.open(file_path, false)) { // false = write/create
            // Пишем данные из header файла
            create_file.write(DEFAULT_DRONE_DATABASE_CONTENT, strlen(DEFAULT_DRONE_DATABASE_CONTENT));
            create_file.close();
        }
    }

private:
    /**
     * Create backup of existing file for atomic writes
     */
    static void create_backup_file(const std::string& filepath) {
        const std::string backup_path = filepath + ".bak";

        File orig_file;
        if (!orig_file.open(filepath, true)) return; // Error opening original

        File backup_file;
        if (!backup_file.open(backup_path, false)) {
            orig_file.close();
            return; // Error opening backup
        }

        // Copy content to backup (simplified)
        std::vector<uint8_t> buffer(1024);
        size_t total_read = 0;

        while (total_read < orig_file.size()) {
            size_t to_read = std::min(size_t(1024), static_cast<size_t>(orig_file.size() - total_read));
            auto read_result = orig_file.read(buffer.data(), to_read);
            if (read_result != to_read) break;

            auto write_result = backup_file.write(buffer.data(), to_read);
            if (write_result != to_read) break;

            total_read += read_result;
        }

        backup_file.close();
        orig_file.close();
    }

    /**
     * Restore from backup file on write error
     */
    static void restore_from_backup(const std::string& filepath) {
        const std::string backup_path = filepath + ".bak";

        // Пытаемся открыть бэкап на чтение
        File backup_file;
        if (!backup_file.open(backup_path, true)) return; // Бэкапа нет, выходим

        // Пытаемся открыть оригинал на запись (создаст новый или перезапишет битый)
        File original_file;
        if (!original_file.open(filepath, false)) {
            backup_file.close();
            return;
        }

        // Копируем данные блоками
        std::vector<uint8_t> buffer(512);
        while (true) {
            auto read_res = backup_file.read(buffer.data(), buffer.size());
            if (read_res.is_error() || read_res.value() == 0) break;

            original_file.write(buffer.data(), read_res.value());
        }

        backup_file.close();
        original_file.close();
    }

    /**
     * Remove backup file after successful write
     */
    static void remove_backup_file(const std::string& filepath) {
        const std::string backup_path = filepath + ".bak";
        // Remove operation would need system call
    }

    /**
     * Generate standardized file header with timestamp
     */
    static std::string generate_file_header() {
        std::stringstream ss;
        ss << "# Enhanced Drone Analyzer Settings v0.3\n";
        ss << "# Generated by Settings App\n";
        ss << "# Timestamp: " << get_current_timestamp() << "\n";
        ss << "# This file is automatically read by Scanner module\n";
        ss << "\n";
        return ss.str();
    }

    /**
     * Generate complete settings content in key=value format
     */
    static std::string generate_settings_content(const DroneAnalyzerSettings& settings) {
        std::stringstream ss;

        // Core scanning parameters
        ss << "spectrum_mode=" << spectrum_mode_to_string(settings.spectrum_mode) << "\n";
        ss << "scan_interval_ms=" << settings.scan_interval_ms << "\n";
        ss << "rssi_threshold_db=" << settings.rssi_threshold_db << "\n";

        // Audio settings
        ss << "enable_audio_alerts=" << (settings.enable_audio_alerts ? "true" : "false") << "\n";
        ss << "audio_alert_frequency_hz=" << settings.audio_alert_frequency_hz << "\n";
        ss << "audio_alert_duration_ms=" << settings.audio_alert_duration_ms << "\n";

        // Hardware settings
        ss << "hardware_bandwidth_hz=" << settings.hardware_bandwidth_hz << "\n";
        ss << "enable_real_hardware=" << (settings.enable_real_hardware ? "true" : "false") << "\n";
        ss << "demo_mode=" << (settings.demo_mode ? "true" : "false") << "\n";

        // Data management
        ss << "freqman_path=" << settings.freqman_path << "\n";

        // Metadata
        ss << "settings_version=0.3\n";
        ss << "last_modified_timestamp=" << chTimeNow() << "\n";

        return ss.str();
    }

    /**
     * Convert spectrum mode enum to string
     */
    static std::string spectrum_mode_to_string(SpectrumMode mode) {
        switch (mode) {
            case SpectrumMode::NARROW: return "NARROW";
            case SpectrumMode::MEDIUM: return "MEDIUM";
            case SpectrumMode::WIDE: return "WIDE";
            case SpectrumMode::ULTRA_WIDE: return "ULTRA_WIDE";
            case SpectrumMode::ULTRA_NARROW: return "ULTRA_NARROW";
            default: return "MEDIUM";
        }
    }

    /**
     * Get current timestamp as formatted string
     */
    static std::string get_current_timestamp() {
        // Simplified timestamp (would use proper formatting in production)
        char buffer[32];
        systime_t now = chTimeNow();
        snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)now);
        return std::string(buffer);
    }
};

// ===========================================
// PART 1: SETTINGS MANAGER IMPLEMENTATION (from ui_drone_config.hpp)
// ===========================================

Language DroneAnalyzerSettingsManager::current_language_ = Language::ENGLISH;

const std::map<std::string, const char*> DroneAnalyzerSettingsManager::translations_english = {
    {"save_settings", "Save Settings"},
    {"load_settings", "Load Settings"},
    {"audio_settings", "Audio Settings"},
    {"hardware_settings", "Hardware Settings"},
    {"scan_interval", "Scan Interval"},
    {"rssi_threshold", "RSSI Threshold"},
    {"spectrum_mode", "Spectrum Mode"}
};

DroneAnalyzerSettingsManager& get_settings_manager() {
    static DroneAnalyzerSettingsManager instance;
    return instance;
}

bool DroneAnalyzerSettingsManager::load(DroneAnalyzerSettings& settings) {
    // Use ScannerSettingsManager for loading
    bool loaded = load_settings_from_txt(settings);
    if (!loaded) {
        reset_to_defaults(settings);
    }
    return loaded;
}

bool DroneAnalyzerSettingsManager::save(const DroneAnalyzerSettings& settings) {
    // Используем мощный менеджер с бэкапами и полной сериализацией
    return EnhancedSettingsManager::save_settings_to_txt(settings);
}

void DroneAnalyzerSettingsManager::reset_to_defaults(DroneAnalyzerSettings& settings) {
    settings.spectrum_mode = SpectrumMode::MEDIUM;
    settings.scan_interval_ms = 1000;
    settings.rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    settings.enable_audio_alerts = true;
    settings.audio_alert_frequency_hz = 800;
    settings.audio_alert_duration_ms = 500;
    settings.enable_wideband_scanning = false;
    settings.wideband_min_freq_hz = WIDEBAND_DEFAULT_MIN;
    settings.wideband_max_freq_hz = WIDEBAND_DEFAULT_MAX;
    settings.min_frequency_hz = 2400000000ULL;
    settings.max_frequency_hz = 2500000000ULL;
    settings.show_detailed_info = true;
    settings.auto_save_logs = true;
    settings.log_file_path = "/eda_logs";
    settings.freqman_path = "DRONES.TXT";
    settings.settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    settings.hardware_bandwidth_hz = 24000000;
    settings.enable_real_hardware = true;
    settings.demo_mode = false;
}

bool DroneAnalyzerSettingsManager::validate(const DroneAnalyzerSettings& settings) {
    if (settings.scan_interval_ms < 100 || settings.scan_interval_ms > 10000) return false;
    if (settings.rssi_threshold_db < -120 || settings.rssi_threshold_db > -30) return false;
    if (settings.audio_alert_frequency_hz < 200 || settings.audio_alert_frequency_hz > 3000) return false;
    if (settings.audio_alert_duration_ms < 50 || settings.audio_alert_duration_ms > 2000) return false;
    if (settings.hardware_bandwidth_hz < 1000000 || settings.hardware_bandwidth_hz > 100000000) return false;
    return true;
}

std::string DroneAnalyzerSettingsManager::serialize(const DroneAnalyzerSettings& settings) {
    std::ostringstream oss;

    oss << "spectrum_mode=";
    switch (settings.spectrum_mode) {
        case SpectrumMode::NARROW: oss << "NARROW"; break;
        case SpectrumMode::MEDIUM: oss << "MEDIUM"; break;
        case SpectrumMode::WIDE: oss << "WIDE"; break;
        case SpectrumMode::ULTRA_WIDE: oss << "ULTRA_WIDE"; break;
        case SpectrumMode::ULTRA_NARROW: oss << "ULTRA_NARROW"; break;
    }
    oss << "|scan_interval_ms=" << settings.scan_interval_ms;
    oss << "|rssi_threshold_db=" << settings.rssi_threshold_db;
    oss << "|enable_audio_alerts=" << (settings.enable_audio_alerts ? "true" : "false");
    // Add more fields as needed...

    return oss.str();
}

bool DroneAnalyzerSettingsManager::deserialize(DroneAnalyzerSettings& settings, const std::string& data) {
    std::istringstream iss(data);
    std::string token;

    while (std::getline(iss, token, '|')) {
        size_t equals_pos = token.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = token.substr(0, equals_pos);
            std::string value = token.substr(equals_pos + 1);

            // Parse and apply values
            if (key == "spectrum_mode") {
                if (value == "NARROW") settings.spectrum_mode = SpectrumMode::NARROW;
                else if (value == "MEDIUM") settings.spectrum_mode = SpectrumMode::MEDIUM;
                else if (value == "WIDE") settings.spectrum_mode = SpectrumMode::WIDE;
                else if (value == "ULTRA_WIDE") settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
            }
            else if (key == "scan_interval_ms") settings.scan_interval_ms = std::stoul(value);
            else if (key == "rssi_threshold_db") settings.rssi_threshold_db = std::stoi(value);
            else if (key == "enable_audio_alerts") settings.enable_audio_alerts = (value == "true");
        }
    }

    return validate(settings);
}

const char* DroneAnalyzerSettingsManager::translate(const std::string& key) {
    auto it = translations_english.find(key);
    if (it != translations_english.end()) {
        return it->second;
    }
    return key.c_str();  // Fallback to the key itself
}

const char* DroneAnalyzerSettingsManager::get_translation(const std::string& key) {
    return translate(key);
}

// ============ ScannerConfig Implementation ============

ScannerConfig::ScannerConfig(ConfigData config)
    : config_data_(config) {
}

bool ScannerConfig::load_from_file(const std::string& filepath) {
    File file;
    if (!file.open(filepath, true)) return false;  // true = read_only

    std::string line;
    char buffer[256];
    size_t bytes_read;
    while ((bytes_read = file.read(buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        line += buffer;

        size_t newline_pos;
        while ((newline_pos = line.find('\n')) != std::string::npos) {
            std::string current_line = line.substr(0, newline_pos);
            line = line.substr(newline_pos + 1);

            size_t equals_pos = current_line.find('=');
            if (equals_pos != std::string::npos) {
                std::string key = current_line.substr(0, equals_pos);
                std::string value = current_line.substr(equals_pos + 1);

                if (key == "spectrum_mode") {
                    if (value == "NARROW") config_data_.spectrum_mode = SpectrumMode::NARROW;
                    else if (value == "MEDIUM") config_data_.spectrum_mode = SpectrumMode::MEDIUM;
                    else if (value == "WIDE") config_data_.spectrum_mode = SpectrumMode::WIDE;
                    else if (value == "ULTRA_WIDE") config_data_.spectrum_mode = SpectrumMode::ULTRA_WIDE;
                }
                else if (key == "rssi_threshold_db") config_data_.rssi_threshold_db = std::stoi(value);
                else if (key == "scan_interval_ms") config_data_.scan_interval_ms = std::stoul(value);
                else if (key == "enable_audio_alerts") config_data_.enable_audio_alerts = (value == "true");
                else if (key == "freqman_path") config_data_.freqman_path = value;
            }
        }
    }

    file.close();
    return true;
}

bool ScannerConfig::save_to_file(const std::string& filepath) const {
    File file;
    if (!file.open(filepath, false)) return false;  // false = write mode

    std::string content = "spectrum_mode=";
    switch (config_data_.spectrum_mode) {
        case SpectrumMode::NARROW: content += "NARROW"; break;
        case SpectrumMode::MEDIUM: content += "MEDIUM"; break;
        case SpectrumMode::WIDE: content += "WIDE"; break;
        case SpectrumMode::ULTRA_WIDE: content += "ULTRA_WIDE"; break;
        case SpectrumMode::ULTRA_NARROW: content += "ULTRA_NARROW"; break;
    }
    content += "\n";

    content += "rssi_threshold_db=" + std::to_string(config_data_.rssi_threshold_db) + "\n";
    content += "scan_interval_ms=" + std::to_string(config_data_.scan_interval_ms) + "\n";
    content += "enable_audio_alerts=" + std::string(config_data_.enable_audio_alerts ? "true" : "false") + "\n";
    content += "freqman_path=" + config_data_.freqman_path + "\n";

    auto result = file.write(content.data(), content.size());
    file.close();
    return result == content.size();
}

void ScannerConfig::set_frequency_range(uint32_t min_hz, uint32_t max_hz) {
    (void)min_hz; (void)max_hz;
    // Implementation for frequency range setting
}

void ScannerConfig::set_rssi_threshold(int32_t threshold) {
    config_data_.rssi_threshold_db = threshold;
}

void ScannerConfig::set_scan_interval(uint32_t interval_ms) {
    config_data_.scan_interval_ms = interval_ms;
}

void ScannerConfig::set_audio_alerts(bool enabled) {
    config_data_.enable_audio_alerts = enabled;
}

void ScannerConfig::set_freqman_path(const std::string& path) {
    config_data_.freqman_path = path;
}

void ScannerConfig::set_scanning_mode(const std::string& mode) {
    if (mode == "Database") config_data_.spectrum_mode = SpectrumMode::MEDIUM;
    else if (mode == "Wideband") config_data_.spectrum_mode = SpectrumMode::WIDE;
    // Add more modes as needed
}

bool ScannerConfig::is_valid() const {
    return config_data_.rssi_threshold_db >= -120 && config_data_.rssi_threshold_db <= -30 &&
           config_data_.scan_interval_ms >= 100 && config_data_.scan_interval_ms <= 10000;
}

// ============ DroneFrequencyPresets Implementation ============

static const std::vector<DronePreset> default_presets = {
    {"2.4GHz Band Scan", "Drone_2_4GHz", 2400000000ULL, ThreatLevel::MEDIUM, DroneType::MAVIC},
    {"2.5GHz Band Scan", "Drone_2_5GHz", 2500000000ULL, ThreatLevel::HIGH, DroneType::PHANTOM},
    {"DJI Mavic Series", "DJI_Mavic", 2437000000ULL, ThreatLevel::HIGH, DroneType::DJI_MINI},
    {"Parrot Anafi", "Parrot_Anafi", 2450000000ULL, ThreatLevel::MEDIUM, DroneType::PARROT_ANAFI},
    {"Military UAV Band", "Military_UAV", 5000000000ULL, ThreatLevel::CRITICAL, DroneType::MILITARY_DRONE}
};

const std::vector<DronePreset>& DroneFrequencyPresets::get_all_presets() {
    return default_presets;
}

std::vector<std::string> DroneFrequencyPresets::get_preset_names() {
    std::vector<std::string> names;
    for (const auto& preset : default_presets) {
        names.push_back(preset.display_name);
    }
    return names;
}

std::vector<DroneType> DroneFrequencyPresets::get_available_types() {
    return {DroneType::MAVIC, DroneType::PHANTOM, DroneType::DJI_MINI,
            DroneType::PARROT_ANAFI, DroneType::PARROT_BEBOP,
            DroneType::PX4_DRONE, DroneType::MILITARY_DRONE};
}

std::string DroneFrequencyPresets::get_type_display_name(DroneType type) {
    switch (type) {
        case DroneType::MAVIC: return "DJI Mavic";
        case DroneType::PHANTOM: return "DJI Phantom";
        case DroneType::DJI_MINI: return "DJI Mini";
        case DroneType::PARROT_ANAFI: return "Parrot Anafi";
        case DroneType::PARROT_BEBOP: return "Parrot Bebop";
        case DroneType::PX4_DRONE: return "PX4 Drone";
        case DroneType::MILITARY_DRONE: return "Military UAV";
        default: return "Unknown";
    }
}

std::vector<DronePreset> DroneFrequencyPresets::get_presets_of_type(const std::vector<DronePreset>& all_presets, DroneType type) {
    std::vector<DronePreset> filtered;
    std::copy_if(all_presets.begin(), all_presets.end(), std::back_inserter(filtered),
                 [type](const DronePreset& preset) { return preset.drone_type == type; });
    return filtered;
}

bool DroneFrequencyPresets::apply_preset(ScannerConfig& config, const DronePreset& preset) {
    (void)config; (void)preset;
    // Implementation to apply preset to ScannerConfig
    return true;
}

// ============ DronePresetSelector Implementation ============

void DronePresetSelector::show_preset_menu(NavigationView& nav, PresetMenuView callback) {
    auto preset_names = DroneFrequencyPresets::get_preset_names();
    auto all_presets = DroneFrequencyPresets::get_all_presets();

    class PresetMenuView : public MenuView {
    public:
        PresetMenuView(NavigationView& nav, std::vector<std::string> names, std::function<void(const DronePreset&)> on_selected,
                      const std::vector<DronePreset>& presets)
            : MenuView(), nav_(nav), names_(names), on_selected_fn_(on_selected), presets_(presets) {
            for (const auto& name : names) {
                add_item({name, Color::white(), nullptr, nullptr});
            }
        }

    private:
        NavigationView& nav_;

        bool on_key(const KeyEvent key) override {
            if (key == KeyEvent::Select) {
                size_t idx = highlighted_index();
                if (idx < presets_.size()) {
                    if (on_selected_fn_) {
                        on_selected_fn_(presets_[idx]);
                    }
                }
                return true;
            }
            return MenuView::on_key(key);
        }

    private:
        std::vector<std::string> names_;
        std::function<void(const DronePreset&)> on_selected_fn_;
        const std::vector<DronePreset>& presets_;
    };

    nav.push<PresetMenuView>(preset_names, callback, all_presets);
}

void DronePresetSelector::show_type_filtered_presets(NavigationView& nav, DroneType type) {
    auto filtered_presets = DroneFrequencyPresets::get_presets_of_type(DroneFrequencyPresets::get_all_presets(), type);
    std::vector<std::string> names;
    for (const auto& preset : filtered_presets) {
        names.push_back(preset.display_name);
    }

    // Simplified callback that only takes the selected preset
    auto on_selected = [&nav](const DronePreset& preset) {
        // Handle preset selection - simplified to avoid signature mismatch
        (void)preset; // Suppress unused parameter warning
        nav.pop(); // Just close the menu for now
    };

    class FilteredPresetMenuView : public MenuView {
    public:
        FilteredPresetMenuView(NavigationView& nav, std::vector<std::string> names, std::function<void(const DronePreset&)> on_selected,
                              const std::vector<DronePreset>& presets)
            : MenuView(), nav_(nav), names_(names), on_selected_fn_(on_selected), presets_(presets) {
            for (const auto& name : names) {
                add_item({name, Color::white(), nullptr, nullptr});
            }
        }

    private:
        NavigationView& nav_;

        bool on_key(const KeyEvent key) override {
            if (key == KeyEvent::Select) {
                size_t idx = highlighted_index();
                if (idx < presets_.size()) {
                    if (on_selected_fn_) {
                        on_selected_fn_(presets_[idx]);
                    }
                }
                return true;
            }
            return MenuView::on_key(key);
        }

    private:
        std::vector<std::string> names_;
        std::function<void(const DronePreset&)> on_selected_fn_;
        const std::vector<DronePreset>& presets_;
    };

    nav.push<FilteredPresetMenuView>(names, on_selected, filtered_presets);
}

PresetMenuView DronePresetSelector::create_config_updater(ScannerConfig& config_to_update) {
    // Return lambda that updates config when preset selected
    return [&config_to_update](const DronePreset& preset) {
        // Apply preset to config
        DroneFrequencyPresets::apply_preset(config_to_update, preset);
    };
}

// ============ SimpleDroneValidation Implementation ============

bool SimpleDroneValidation::validate_frequency_range(Frequency freq_hz) {
    return freq_hz >= MIN_HARDWARE_FREQ && freq_hz <= MAX_HARDWARE_FREQ;
}

bool SimpleDroneValidation::validate_rssi_signal(int32_t rssi_db, ThreatLevel threat) {
    (void)threat;
    return rssi_db >= -120 && rssi_db <= 0;
}

ThreatLevel SimpleDroneValidation::classify_signal_strength(int32_t rssi_db) {
    if (rssi_db >= -70) return ThreatLevel::HIGH;
    if (rssi_db >= -80) return ThreatLevel::MEDIUM;
    return ThreatLevel::LOW;
}

DroneType SimpleDroneValidation::identify_drone_type(Frequency freq_hz, int32_t rssi_db) {
    (void)rssi_db;

    // Basic frequency-based identification
    if (freq_hz >= 2400000000ULL && freq_hz <= 2500000000ULL) {
        if (freq_hz >= 2430000000ULL && freq_hz <= 2480000000ULL) {
            return DroneType::MAVIC;
        } else if (freq_hz >= 5200000000ULL && freq_hz <= 5800000000ULL) {
            return DroneType::MILITARY_DRONE;
        }
        return DroneType::PHANTOM;
    }

    return DroneType::UNKNOWN;
}

bool SimpleDroneValidation::validate_drone_detection(Frequency freq_hz, int32_t rssi_db,
                                                   DroneType type, ThreatLevel threat) {
    return validate_frequency_range(freq_hz) && validate_rssi_signal(rssi_db, threat) && type != DroneType::UNKNOWN;
}

// ============ DroneFrequencyEntry Implementation ============

DroneFrequencyEntry::DroneFrequencyEntry(Frequency freq, DroneType type, ThreatLevel threat,
                                       int32_t rssi_thresh, uint32_t bw_hz, const char* desc)
    : frequency_hz(freq), drone_type(type), threat_level(threat),
      rssi_threshold_db(rssi_thresh), bandwidth_hz(bw_hz), description(desc) {
}

bool DroneFrequencyEntry::is_valid() const {
    return SimpleDroneValidation::validate_frequency_range(frequency_hz) &&
           rssi_threshold_db >= -120 && rssi_threshold_db <= 0 &&
           bandwidth_hz > 0;
}

// ===========================================
// PART 2: UI IMPLEMENTATIONS
// ===========================================

#include "ui_widget.hpp"
#include "ui_menu.hpp"
#include "app_settings.hpp"

// HardwareSettingsView Implementation
HardwareSettingsView::HardwareSettingsView(NavigationView& nav)
    : nav_(nav)
{
    // Using proper PortaPack UI widgets
    add_children({
        &checkbox_real_hardware_,
        &text_real_hardware_,
        &field_spectrum_mode_,
        &number_bandwidth_,
        &number_min_freq_,
        &number_max_freq_,
        &button_save_
    });

    load_current_settings();
}

void HardwareSettingsView::focus() {
    button_save_.focus();
}



void HardwareSettingsView::load_current_settings() {
    // Load settings from TXT file or defaults
    DroneAnalyzerSettings settings;
    if (!DroneAnalyzerSettingsManager::load(settings)) {
        DroneAnalyzerSettingsManager::reset_to_defaults(settings);
    }

    // Set UI elements
    checkbox_real_hardware_.set_value(settings.enable_real_hardware);

    size_t mode_idx = 0;
    switch (settings.spectrum_mode) {
        case SpectrumMode::ULTRA_NARROW: mode_idx = 0; break;
        case SpectrumMode::NARROW: mode_idx = 1; break;
        case SpectrumMode::MEDIUM: mode_idx = 2; break;
        case SpectrumMode::WIDE: mode_idx = 3; break;
        case SpectrumMode::ULTRA_WIDE: mode_idx = 4; break;
        default: mode_idx = 2; break;
    }
    field_spectrum_mode_.set_selected_index(mode_idx);

    // --- ДОБАВЛЕНО: Привязка данных к новым полям ---
    number_bandwidth_.set_value(settings.hardware_bandwidth_hz);
    number_min_freq_.set_value(settings.min_frequency_hz);
    number_max_freq_.set_value(settings.max_frequency_hz);
    // ------------------------------------------------
}

void HardwareSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    DroneAnalyzerSettingsManager::load(settings); // Load current

    settings.enable_real_hardware = checkbox_real_hardware_.value();
    settings.demo_mode = !checkbox_real_hardware_.value();

    size_t mode_idx = field_spectrum_mode_.selected_index();
    switch (mode_idx) {
        case 0: settings.spectrum_mode = SpectrumMode::ULTRA_NARROW; break;
        case 1: settings.spectrum_mode = SpectrumMode::NARROW; break;
        case 2: settings.spectrum_mode = SpectrumMode::MEDIUM; break;
        case 3: settings.spectrum_mode = SpectrumMode::WIDE; break;
        case 4: settings.spectrum_mode = SpectrumMode::ULTRA_WIDE; break;
    }

    // --- ДОБАВЛЕНО: Сохранение значений из полей ---
    settings.hardware_bandwidth_hz = number_bandwidth_.value();
    settings.min_frequency_hz = number_min_freq_.value();
    settings.max_frequency_hz = number_max_freq_.value();
    // -----------------------------------------------

    DroneAnalyzerSettingsManager::save(settings);
}

void HardwareSettingsView::update_ui_from_settings() {
    load_current_settings();
}

void HardwareSettingsView::update_settings_from_ui() {
    save_current_settings();
}

void HardwareSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Hardware settings saved\nto TXT file");
}

AudioSettingsView::AudioSettingsView(NavigationView& nav)
    : View(), nav_(nav), audio_settings_{true, 800, 500, 50, false}
{
    add_children({
        &checkbox_audio_enabled_,
        &text_audio_enabled_,
        &number_alert_frequency_,
        &number_alert_duration_,
        &number_volume_,
        &checkbox_repeat_,
        &text_repeat_,
        &button_save_
    });
}

void AudioSettingsView::focus() {
    button_save_.focus();
}



void AudioSettingsView::load_current_settings() {
    // Load from settings TXT
    DroneAnalyzerSettings settings;
    if (!DroneAnalyzerSettingsManager::load(settings)) {
        DroneAnalyzerSettingsManager::reset_to_defaults(settings);
    }

    checkbox_audio_enabled_.set_value(settings.enable_audio_alerts);
    number_alert_frequency_.set_value(settings.audio_alert_frequency_hz);
    number_alert_duration_.set_value(settings.audio_alert_duration_ms);

    // Volume not implemented yet
    // Repeat alerts not implemented yet
}

void AudioSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    DroneAnalyzerSettingsManager::load(settings); // Load current

    settings.enable_audio_alerts = checkbox_audio_enabled_.value();
    settings.audio_alert_frequency_hz = number_alert_frequency_.value();
    settings.audio_alert_duration_ms = number_alert_duration_.value();

    DroneAnalyzerSettingsManager::save(settings); // Save back to TXT
}

void AudioSettingsView::update_ui_from_settings() {
    load_current_settings();
}

void AudioSettingsView::update_settings_from_ui() {
    save_current_settings();
}

void AudioSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Audio settings saved\nto TXT file");
}

// LoadingView Implementation
LoadingView::LoadingView(NavigationView& nav, const std::string& loading_text)
    : View(), nav_(nav), loading_text_(loading_text),
      loading_text_1_{{screen_width / 2 - 50, screen_height / 2 - 10, 100, 16}, loading_text},
      loading_text_2_{{screen_width / 2 - 50, screen_height / 2 + 10, 100, 16}, ""}
{
    add_children({&loading_text_1_, &loading_text_2_});
}

void LoadingView::focus() {
    // No focusable elements
}



void LoadingView::paint(Painter& painter) {
    View::paint(painter);
    // Additional painting if needed
}

void LoadingView::on_show() {
    start_time_ = chTimeNow();
}

void LoadingView::on_hide() {
    // Cleanup if needed
}

// ===========================================
// PART 3: MISSING UI IMPLEMENTATIONS
// ===========================================

// --- ScanningSettingsView Implementation ---

ScanningSettingsView::ScanningSettingsView(NavigationView& nav)
    : View(), nav_(nav)
{
    add_children({
        &field_scanning_mode_,
        &number_scan_interval_,
        &number_rssi_threshold_,
        &checkbox_wideband_,
        &text_wideband_,
        // &button_presets_, // Пока закомментируем, если пресеты сложны в реализации
        &button_save_
    });

    button_save_.on_select = [this](Button&) {
        on_save_settings();
    };

    // Добавляем обработчики изменений если нужно
    checkbox_wideband_.on_select = [this](Checkbox&, bool) {
        on_wideband_enabled_changed();
    };

    load_current_settings();
}

void ScanningSettingsView::focus() {
    button_save_.focus();
}

void ScanningSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!DroneAnalyzerSettingsManager::load(settings)) {
        DroneAnalyzerSettingsManager::reset_to_defaults(settings);
    }

    // Маппинг режима сканирования в индекс (упрощенно)
    // 0: Database, 1: Wideband, 2: Hybrid
    field_scanning_mode_.set_selected_index(0); // По умолчанию

    number_scan_interval_.set_value(settings.scan_interval_ms);
    number_rssi_threshold_.set_value(settings.rssi_threshold_db);
    checkbox_wideband_.set_value(settings.enable_wideband_scanning);
}

void ScanningSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    DroneAnalyzerSettingsManager::load(settings);

    settings.scan_interval_ms = number_scan_interval_.value();
    settings.rssi_threshold_db = number_rssi_threshold_.value();
    settings.enable_wideband_scanning = checkbox_wideband_.value();

    // Сохранение режима сканирования пропустим для простоты или добавим логику позже

    DroneAnalyzerSettingsManager::save(settings);
}

void ScanningSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Scanning settings\nsaved to TXT");
}

void ScanningSettingsView::on_show_presets() {
    // Placeholder
    nav_.display_modal("Info", "Presets menu\nnot implemented");
}

void ScanningSettingsView::on_wideband_enabled_changed() {
    // Логика UI обновления при переключении галочки
}

void ScanningSettingsView::update_ui_from_settings() {
    load_current_settings();
}

void ScanningSettingsView::update_settings_from_ui() {
    save_current_settings();
}

// --- DroneAnalyzerSettingsView Implementation (MAIN SETTINGS MENU) ---

DroneAnalyzerSettingsView::DroneAnalyzerSettingsView(NavigationView& nav)
    : View(), nav_(nav), current_settings_{}
{
    add_children({
        &text_title_,
        &button_audio_settings_,
        &button_hardware_settings_,
        &button_scanning_settings_,
        &button_advanced_settings_,
        &button_load_defaults_,
        &button_manage_db_
    });

    // Настройка обработчиков кнопок
    button_audio_settings_.on_select = [this](Button&) {
        show_audio_settings();
    };

    button_hardware_settings_.on_select = [this](Button&) {
        show_hardware_settings();
    };

    button_scanning_settings_.on_select = [this](Button&) {
        show_scanning_settings();
    };

    button_advanced_settings_.on_select = [this](Button&) {
        show_advanced_settings();
    };

    button_load_defaults_.on_select = [this](Button&) {
        load_default_settings();
    };

    button_manage_db_.on_select = [this](Button&) {
        nav_.push<DroneDatabaseListView>();
    };

    // --- ДОБАВЛЕНО: Проверка и создание базы частот ---
    EnhancedSettingsManager::ensure_database_exists();

    // Загружаем текущие настройки при старте
    DroneAnalyzerSettingsManager::load(current_settings_);
}

void DroneAnalyzerSettingsView::focus() {
    button_audio_settings_.focus();
}

void DroneAnalyzerSettingsView::paint(Painter& painter) {
    View::paint(painter);
    // Можно добавить рамку или фон
}

bool DroneAnalyzerSettingsView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Back) {
        nav_.pop();
        return true;
    }
    return View::on_key(key);
}

bool DroneAnalyzerSettingsView::on_touch(const TouchEvent event) {
    return View::on_touch(event);
}

void DroneAnalyzerSettingsView::on_show() {
    View::on_show();
    // Обновляем состояние если нужно
}

void DroneAnalyzerSettingsView::on_hide() {
    View::on_hide();
}

void DroneAnalyzerSettingsView::show_audio_settings() {
    nav_.push<AudioSettingsView>();
}

void DroneAnalyzerSettingsView::show_hardware_settings() {
    nav_.push<HardwareSettingsView>();
}

void DroneAnalyzerSettingsView::show_scanning_settings() {
    nav_.push<ScanningSettingsView>();
}

void DroneAnalyzerSettingsView::show_advanced_settings() {
    nav_.display_modal("Info", "Advanced settings\ncoming soon");
}

void DroneAnalyzerSettingsView::load_default_settings() {
    DroneAnalyzerSettingsManager::reset_to_defaults(current_settings_);
    DroneAnalyzerSettingsManager::save(current_settings_);
    nav_.display_modal("Reset", "Settings reset to\ndefaults");
}

// ===========================================
// PART 4: DRONE DATABASE MANAGER IMPLEMENTATION
// ===========================================

std::vector<DroneDbEntry> DroneDatabaseManager::load_database(const std::string& file_path) {
    std::vector<DroneDbEntry> entries;
    File file;
    if (!file.open(file_path, true)) return entries; // Ошибка или файла нет

    std::string content;
    content.resize(file.size());
    file.read(content.data(), file.size());
    file.close();

    std::stringstream ss(content);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty() || line[0] == '#') continue; // Пропуск комментариев

        size_t comma_pos = line.find(',');
        if (comma_pos != std::string::npos) {
            std::string freq_str = line.substr(0, comma_pos);
            std::string desc_str = line.substr(comma_pos + 1);

            // Очистка от \r если есть
            if (!desc_str.empty() && desc_str.back() == '\r') desc_str.pop_back();

            DroneDbEntry entry;
            entry.freq = std::strtoull(freq_str.c_str(), nullptr, 10);
            entry.description = desc_str;
            entries.push_back(entry);
        }
    }
    return entries;
}

bool DroneDatabaseManager::save_database(const std::vector<DroneDbEntry>& entries, const std::string& file_path) {
    std::stringstream ss;
    // Заголовок
    ss << "# EDA User Database\n# Format: Freq, Desc\n";

    for (const auto& entry : entries) {
        ss << entry.freq << "," << entry.description << "\n";
    }

    std::string content = ss.str();

    // Перезапись файла
    File file;
    if (!file.open(file_path, false)) return false; // Ошибка создания

    file.write(content.data(), content.size());
    file.close();
    return true;
}

// ===========================================
// PART 5: DRONE ENTRY EDITOR VIEW IMPLEMENTATION
// ===========================================

DroneEntryEditorView::DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, OnSaveCallback callback)
    : View(), nav_(nav), entry_(entry), on_save_(callback)
{
    add_children({
        &text_freq_,
        &field_freq_,
        &text_desc_,
        &field_desc_,
        &button_save_,
        &button_cancel_
    });

    // Initialize fields with entry data
    field_freq_.set_value(entry_.freq);
    // field_desc_ is already bound to entry_.description

    // Set up button handlers
    button_save_.on_select = [this](Button&) {
        on_save();
    };

    button_cancel_.on_select = [this](Button&) {
        on_cancel();
    };
}

void DroneEntryEditorView::focus() {
    field_freq_.focus();
}

void DroneEntryEditorView::on_save() {
    // Create new entry from field values
    DroneDbEntry new_entry;
    new_entry.freq = field_freq_.value();
    new_entry.description = entry_.description; // TextEdit modifies this directly

    // Call callback with new entry
    if (on_save_) {
        on_save_(new_entry);
    }

    // Close the view
    nav_.pop();
}

void DroneEntryEditorView::on_cancel() {
    // Call callback with freq=0 to indicate cancel/delete
    if (on_save_) {
        DroneDbEntry cancel_entry{0, ""};
        on_save_(cancel_entry);
    }

    // Close the view
    nav_.pop();
}

// ===========================================
// PART 6: DRONE DATABASE LIST VIEW IMPLEMENTATION
// ===========================================

DroneDatabaseListView::DroneDatabaseListView(NavigationView& nav)
    : View(), nav_(nav)
{
    add_children({&menu_view_});

    entries_ = DroneDatabaseManager::load_database();

    reload_list();
}

void DroneDatabaseListView::focus() {
    menu_view_.focus();
}

void DroneDatabaseListView::reload_list() {
    menu_view_.clear();

    // Add "ADD NEW FREQUENCY" item
    menu_view_.add_item({"[ + ADD NEW FREQUENCY ]", Color::white(), nullptr, nullptr});

    // Add existing entries
    for (const auto& entry : entries_) {
        std::string freq_str = to_string_short_freq(entry.freq);
        std::string text = freq_str + ": " + entry.description;
        menu_view_.add_item({text, Color::white(), nullptr, nullptr});
    }
}

void DroneDatabaseListView::on_entry_selected(size_t index) {
    if (index == 0) {
        // Add new frequency
        DroneDbEntry empty_entry{0, ""};
        nav_.push<DroneEntryEditorView>(empty_entry, [this](const DroneDbEntry& entry) {
            if (entry.freq != 0) {
                entries_.push_back(entry);
                save_changes();
                reload_list();
            }
        });
    } else {
        // Edit existing entry
        size_t entry_index = index - 1;
        nav_.push<DroneEntryEditorView>(entries_[entry_index], [this, entry_index](const DroneDbEntry& entry) {
            if (entry.freq != 0) {
                entries_[entry_index] = entry;
                save_changes();
                reload_list();
            }
        });
    }
}

void DroneDatabaseListView::save_changes() {
    DroneDatabaseManager::save_database(entries_);
}

bool DroneDatabaseListView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Select) {
        size_t index = menu_view_.highlighted_index();
        on_entry_selected(index);
        return true;
    }
    return View::on_key(key);
}

} // namespace ui::external_app::enhanced_drone_analyzer
