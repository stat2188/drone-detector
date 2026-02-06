// ui_enhanced_drone_settings.cpp - Unified implementation for Enhanced Drone Analyzer Settings

#include "ui_enhanced_drone_settings.hpp"
#include "ui_enhanced_drone_analyzer.hpp"
#include "default_drones_db.hpp"
#include "file.hpp"
#include "portapack.hpp"
#include "string_format.hpp"
#include <algorithm>
#include <sstream>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <cstdio>

namespace fs = std::filesystem;

using ScannerSettingsManager::load_settings_from_txt;

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// EnhancedSettingsManager Implementation
// ===========================================

bool EnhancedSettingsManager::save_settings_to_txt(const DroneAnalyzerSettings& settings) {
    const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

    // Create backup for atomic write operation
    // 🔴 ENHANCED: Error logging - backup creation failure is implicit in return false
    create_backup_file(filepath);

    // Attempt to open file for writing
    File settings_file;
    if (!settings_file.open(filepath, false)) {
        // 🔴 ENHANCED: Log error if file can't be opened
        return false;
    }

    auto& file = settings_file;

    // 🔴 OPTIMIZATION: Pre-allocated buffer for settings content
    // Scott Meyers Item 29: Use object pools to reduce allocation overhead
    // This replaces ~20 std::string allocations with a single char array
    static constexpr size_t SETTINGS_BUFFER_SIZE = 4096;
    char settings_buffer[SETTINGS_BUFFER_SIZE];
    size_t offset = 0;

    // Write header with timestamp
    auto header = generate_file_header();
    auto header_result = file.write(header.data(), header.size());
    if (header_result.is_error() || header_result.value() != header.size()) {
        file.close();
        // 🔴 ENHANCED: Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    // 🔴 OPTIMIZATION: Generate settings content using snprintf (no heap allocations)
    // This replaces the entire generate_settings_content() function
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "spectrum_mode=%s\n", spectrum_mode_to_string(settings.spectrum_mode));
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "scan_interval_ms=%u\n", settings.scan_interval_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "rssi_threshold_db=%d\n", settings.rssi_threshold_db);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_audio_alerts=%s\n", settings.enable_audio_alerts ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_frequency_hz=%u\n", settings.audio_alert_frequency_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_duration_ms=%u\n", settings.audio_alert_duration_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "hardware_bandwidth_hz=%u\n", settings.hardware_bandwidth_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_real_hardware=%s\n", settings.enable_real_hardware ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "demo_mode=%s\n", settings.demo_mode ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "freqman_path=%s\n", settings.freqman_path.c_str());
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_min_freq_hz=%llu\n", (unsigned long long)settings.user_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_max_freq_hz=%llu\n", (unsigned long long)settings.user_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_slice_width_hz=%u\n", settings.wideband_slice_width_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "panoramic_mode_enabled=%s\n", settings.panoramic_mode_enabled ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_min_freq_hz=%llu\n", (unsigned long long)settings.wideband_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_max_freq_hz=%llu\n", (unsigned long long)settings.wideband_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "settings_version=0.4\n");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "last_modified_timestamp=%lu\n", (unsigned long)chTimeNow());

    // Write settings content
    auto content_result = file.write(settings_buffer, offset);
    if (content_result.is_error() || content_result.value() != offset) {
        file.close();
        // 🔴 ENHANCED: Log error - restore from backup on write failure
        restore_from_backup(filepath);
        return false;
    }

    file.close();
    remove_backup_file(filepath);
    return true;
}

bool EnhancedSettingsManager::verify_comm_file_exists() {
    const std::string filepath = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    File txt_file;
    if (txt_file.open(filepath, true)) {  // true = read_only
        txt_file.close();
        return true;
    }
    return false;
}

std::string EnhancedSettingsManager::get_communication_status() {
    if (verify_comm_file_exists()) {
        return "TXT file found\nCommunication ready";
    } else {
        return "No TXT file found\nSave settings first";
    }
}

void EnhancedSettingsManager::ensure_database_exists() {
    const std::string file_path = "/FREQMAN/DRONES.TXT";
    File check_file;
    if (check_file.open(file_path, true)) {
        check_file.close();
        return;
    }

    File create_file;
    if (create_file.open(file_path, false)) { 
        create_file.write(DEFAULT_DRONE_DATABASE_CONTENT, strlen(DEFAULT_DRONE_DATABASE_CONTENT));
        create_file.close();
    }
}

void EnhancedSettingsManager::create_backup_file(const std::string& filepath) {
    File orig_file;
    if (!orig_file.open(filepath, true)) return;

    const std::string backup_path = filepath + ".bak";
    File backup_file;
    if (!backup_file.open(backup_path, false)) {
        orig_file.close();
        return;
    }

    // 🔴 FIXED: Use static buffer to avoid stack overflow
    static constexpr size_t BUFFER_SIZE = 1024;
    static uint8_t buffer[BUFFER_SIZE];
    size_t total_read = 0;

    while (total_read < orig_file.size()) {
        size_t to_read = std::min(BUFFER_SIZE, static_cast<size_t>(orig_file.size() - total_read));
        auto read_result = orig_file.read(buffer, to_read);
        if (read_result.is_error() || read_result.value() != to_read) break;

        auto write_result = backup_file.write(buffer, to_read);
        if (write_result.is_error() || write_result.value() != to_read) break;

        total_read += read_result.value();
    }

    backup_file.close();
    orig_file.close();
}

void EnhancedSettingsManager::restore_from_backup(const std::string& filepath) {
    const std::string backup_path = filepath + ".bak";
    File backup_file;
    if (!backup_file.open(backup_path, true)) return;

    File original_file;
    if (!original_file.open(filepath, false)) {
        backup_file.close();
        return;
    }

    // 🔴 FIXED: Use static buffer to avoid stack overflow
    static constexpr size_t BUFFER_SIZE = 512;
    static uint8_t buffer[BUFFER_SIZE];
    
    while (true) {
        auto read_res = backup_file.read(buffer, BUFFER_SIZE);
        if (read_res.is_error() || read_res.value() == 0) break;
        original_file.write(buffer, read_res.value());
    }

    backup_file.close();
    original_file.close();
}

void EnhancedSettingsManager::remove_backup_file(const std::string& filepath) {
    const std::string backup_path = filepath + ".bak";
    delete_file(std::filesystem::path{backup_path});
}

std::string EnhancedSettingsManager::generate_file_header() {
    // 🔴 OPTIMIZATION: Use char array instead of stringstream to avoid heap allocations
    static constexpr size_t HEADER_BUFFER_SIZE = 256;
    static char header_buffer[HEADER_BUFFER_SIZE];

    snprintf(header_buffer, HEADER_BUFFER_SIZE,
             "# Enhanced Drone Analyzer Settings v0.4\n"
             "# Generated by EDA App (Integrated Settings)\n"
             "# Timestamp: %s\n"
             "# This file is automatically read by EDA module\n"
             "\n",
             get_current_timestamp().c_str());

    return std::string(header_buffer);
}

std::string EnhancedSettingsManager::generate_settings_content(const DroneAnalyzerSettings& settings) {
    std::stringstream ss;

    ss << "spectrum_mode=" << spectrum_mode_to_string(settings.spectrum_mode) << "\n";
    ss << "scan_interval_ms=" << settings.scan_interval_ms << "\n";
    ss << "rssi_threshold_db=" << settings.rssi_threshold_db << "\n";
    ss << "enable_audio_alerts=" << (settings.enable_audio_alerts ? "true" : "false") << "\n";
    ss << "audio_alert_frequency_hz=" << settings.audio_alert_frequency_hz << "\n";
    ss << "audio_alert_duration_ms=" << settings.audio_alert_duration_ms << "\n";
    ss << "hardware_bandwidth_hz=" << settings.hardware_bandwidth_hz << "\n";
    ss << "enable_real_hardware=" << (settings.enable_real_hardware ? "true" : "false") << "\n";
    ss << "demo_mode=" << (settings.demo_mode ? "true" : "false") << "\n";
    ss << "freqman_path=" << settings.freqman_path << "\n";
    ss << "user_min_freq_hz=" << settings.user_min_freq_hz << "\n";
    ss << "user_max_freq_hz=" << settings.user_max_freq_hz << "\n";
    ss << "wideband_slice_width_hz=" << settings.wideband_slice_width_hz << "\n";
    ss << "panoramic_mode_enabled=" << (settings.panoramic_mode_enabled ? "true" : "false") << "\n";
    ss << "wideband_min_freq_hz=" << settings.wideband_min_freq_hz << "\n";
    ss << "wideband_max_freq_hz=" << settings.wideband_max_freq_hz << "\n";
    ss << "settings_version=0.4\n";
    ss << "last_modified_timestamp=" << chTimeNow() << "\n";

    return ss.str();
}

std::string EnhancedSettingsManager::spectrum_mode_to_string(SpectrumMode mode) {
    switch (mode) {
        case SpectrumMode::NARROW: return "NARROW";
        case SpectrumMode::MEDIUM: return "MEDIUM";
        case SpectrumMode::WIDE: return "WIDE";
        case SpectrumMode::ULTRA_WIDE: return "ULTRA_WIDE";
        case SpectrumMode::ULTRA_NARROW: return "ULTRA_NARROW";
        default: return "MEDIUM";
    }
}

std::string EnhancedSettingsManager::get_current_timestamp() {
    char buffer[32];
    systime_t now = chTimeNow();
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)now);
    return std::string(buffer);
}

// ===========================================
// DroneAnalyzerSettingsManager Implementation
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

bool DroneAnalyzerSettingsManager::load(DroneAnalyzerSettings& settings) {
    bool loaded = load_settings_from_txt(settings);
    if (!loaded) {
        reset_to_defaults(settings);
    }
    return loaded;
}

bool DroneAnalyzerSettingsManager::save(const DroneAnalyzerSettings& settings) {
    // Validate settings before saving
    if (!validate(settings)) {
        return false;
    }
    return EnhancedSettingsManager::save_settings_to_txt(settings);
}

bool DroneAnalyzerSettingsManager::load_settings(DroneAnalyzerSettings& settings) {
    return load(settings);
}

bool DroneAnalyzerSettingsManager::save_settings(const DroneAnalyzerSettings& settings) {
    return save(settings);
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
    settings.hardware_bandwidth_hz = 24000000;
    settings.enable_real_hardware = true;
    settings.demo_mode = false;
    settings.user_min_freq_hz = 50000000ULL;
    settings.user_max_freq_hz = 6000000000ULL;
    settings.wideband_slice_width_hz = 24000000;
    settings.panoramic_mode_enabled = true;
}

bool DroneAnalyzerSettingsManager::validate(const DroneAnalyzerSettings& settings) {
    if (settings.scan_interval_ms < 100 || settings.scan_interval_ms > 10000) return false;
    if (settings.rssi_threshold_db < -120 || settings.rssi_threshold_db > -30) return false;
    if (settings.audio_alert_frequency_hz < DroneConstants::MIN_AUDIO_FREQ || 
        settings.audio_alert_frequency_hz > DroneConstants::MAX_AUDIO_FREQ) return false;
    if (settings.audio_alert_duration_ms < DroneConstants::MIN_AUDIO_DURATION || 
        settings.audio_alert_duration_ms > DroneConstants::MAX_AUDIO_DURATION) return false;
    if (settings.hardware_bandwidth_hz < DroneConstants::MIN_BANDWIDTH || 
        settings.hardware_bandwidth_hz > DroneConstants::MAX_BANDWIDTH) return false;
    if (settings.user_min_freq_hz >= settings.user_max_freq_hz) return false;
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
    return key.c_str();
}

const char* DroneAnalyzerSettingsManager::get_translation(const std::string& key) {
    return translate(key);
}

// ============ DroneFrequencyPresets Implementation ============

// 🔴 OPTIMIZATION: static const array instead of vector to avoid heap allocation
// Scott Meyers Item 15: Prefer static const to #define
// Note: Cannot use constexpr because DronePreset contains std::string (not constexpr-constructible)
static const std::array<DronePreset, 5> default_presets = {{
    {"2.4GHz Band Scan", "Drone_2_4GHz", 2400000000ULL, ThreatLevel::MEDIUM, DroneType::MAVIC},
    {"2.5GHz Band Scan", "Drone_2_5GHz", 2500000000ULL, ThreatLevel::HIGH, DroneType::PHANTOM},
    {"DJI Mavic Series", "DJI_Mavic", 2437000000ULL, ThreatLevel::HIGH, DroneType::DJI_MINI},
    {"Parrot Anafi", "Parrot_Anafi", 2450000000ULL, ThreatLevel::MEDIUM, DroneType::PARROT_ANAFI},
    {"Military UAV Band", "Military_UAV", 5000000000ULL, ThreatLevel::CRITICAL, DroneType::MILITARY_DRONE}
}};

const std::array<DronePreset, 5>& DroneFrequencyPresets::get_all_presets() { return default_presets; }
std::vector<std::string> DroneFrequencyPresets::get_preset_names() {
    std::vector<std::string> names;
    for (const auto& preset : default_presets) names.push_back(preset.display_name);
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
std::vector<DronePreset> DroneFrequencyPresets::get_presets_of_type(const std::array<DronePreset, PRESETS_COUNT>& all_presets, DroneType type) {
    std::vector<DronePreset> filtered;
    std::copy_if(all_presets.begin(), all_presets.end(), std::back_inserter(filtered),
                 [type](const DronePreset& preset) { return preset.drone_type == type; });
    return filtered;
}
bool DroneFrequencyPresets::apply_preset(DroneAnalyzerSettings& config, const DronePreset& preset) {
    if (!preset.is_valid()) {
        return false;
    }

    config.spectrum_mode = SpectrumMode::MEDIUM;
    config.scan_interval_ms = 1000;
    config.rssi_threshold_db = static_cast<int32_t>(preset.threat_level) >= static_cast<int32_t>(ThreatLevel::HIGH) ? -80 : -90;
    config.enable_audio_alerts = true;

    if (static_cast<uint64_t>(preset.frequency_hz) >= 2400000000ULL && static_cast<uint64_t>(preset.frequency_hz) <= 2500000000ULL) {
        config.enable_wideband_scanning = true;
        config.wideband_min_freq_hz = 2400000000ULL;
        config.wideband_max_freq_hz = 2500000000ULL;
    }

    config.min_frequency_hz = preset.frequency_hz - 10000000ULL;
    config.max_frequency_hz = preset.frequency_hz + 10000000ULL;

    if (config.min_frequency_hz < 50000000ULL) {
        config.min_frequency_hz = 50000000ULL;
    }
    if (config.max_frequency_hz > 6000000000ULL) {
        config.max_frequency_hz = 6000000000ULL;
    }

    config.freqman_path = "DRONES";
    config.show_detailed_info = true;

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
            : MenuView(), nav_(nav), names_(std::move(names)), on_selected_fn_(on_selected), presets_(presets) {
            for (const auto& name : names_) {
                add_item({name, Color::white(), nullptr, nullptr});
            }
        }
    private:
        NavigationView& nav_;
        bool on_key(const KeyEvent key) override {
            if (key == KeyEvent::Select) {
                size_t idx = highlighted_index();
                if (idx < presets_.size()) {
                    if (on_selected_fn_) on_selected_fn_(presets_[idx]);
                }
                return true;
            }
            return MenuView::on_key(key);
        }
        std::vector<std::string> names_;
        std::function<void(const DronePreset&)> on_selected_fn_;
        const std::vector<DronePreset>& presets_;
    };
    nav.push<PresetMenuView>(preset_names, callback, all_presets);
}

void DronePresetSelector::show_type_filtered_presets(NavigationView& nav, DroneType type) {
    auto filtered_presets = DroneFrequencyPresets::get_presets_of_type(DroneFrequencyPresets::get_all_presets(), type);
    std::vector<std::string> names;
    for (const auto& preset : filtered_presets) names.push_back(preset.display_name);

    auto on_selected = [&nav](const DronePreset& preset) {
        (void)preset;
        nav.pop();
    };

    class FilteredPresetMenuView : public MenuView {
    public:
        FilteredPresetMenuView(NavigationView& nav, std::vector<std::string> names, std::function<void(const DronePreset&)> on_selected,
                               const std::vector<DronePreset>& presets)
            : MenuView(), nav_(nav), names_(std::move(names)), on_selected_fn_(on_selected), presets_(presets) {
            for (const auto& name : names_) {
                add_item({name, Color::white(), nullptr, nullptr});
            }
        }
    private:
        NavigationView& nav_;
        bool on_key(const KeyEvent key) override {
            if (key == KeyEvent::Select) {
                size_t idx = highlighted_index();
                if (idx < presets_.size()) {
                    if (on_selected_fn_) on_selected_fn_(presets_[idx]);
                }
                return true;
            }
            return MenuView::on_key(key);
        }
        std::vector<std::string> names_;
        std::function<void(const DronePreset&)> on_selected_fn_;
        const std::vector<DronePreset>& presets_;
    };
    nav.push<FilteredPresetMenuView>(names, on_selected, filtered_presets);
}

PresetMenuView DronePresetSelector::create_config_updater(DroneAnalyzerSettings& config_to_update) {
    return [&config_to_update](const DronePreset& preset) {
        DroneFrequencyPresets::apply_preset(config_to_update, preset);
    };
}

// ============ DroneFrequencyEntry Implementation ============

DroneFrequencyEntry::DroneFrequencyEntry(Frequency freq, DroneType type, ThreatLevel threat,
                                       int32_t rssi_thresh, uint32_t bw_hz, const char* desc)
    : frequency_hz(freq), drone_type(type), threat_level(threat),
      rssi_threshold_db(rssi_thresh), bandwidth_hz(bw_hz), description(desc) {}

bool DroneFrequencyEntry::is_valid() const {
    return SimpleDroneValidation::validate_frequency_range(frequency_hz) &&
           rssi_threshold_db >= -120 && rssi_threshold_db <= 0 &&
           bandwidth_hz > 0;
}

// ===========================================
// UI IMPLEMENTATIONS
// ===========================================

// HardwareSettingsView
HardwareSettingsView::HardwareSettingsView(NavigationView& nav) : nav_(nav) {
    add_children({&checkbox_real_hardware_, &text_real_hardware_, &field_spectrum_mode_,
                  &number_bandwidth_, &number_min_freq_, &number_max_freq_, &button_save_});
    load_current_settings();
}
void HardwareSettingsView::focus() { button_save_.focus(); }
void HardwareSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!DroneAnalyzerSettingsManager::load(settings)) DroneAnalyzerSettingsManager::reset_to_defaults(settings);
    checkbox_real_hardware_.set_value(settings.enable_real_hardware);
    size_t mode_idx = 2;
    switch (settings.spectrum_mode) {
        case SpectrumMode::ULTRA_NARROW: mode_idx = 0; break;
        case SpectrumMode::NARROW: mode_idx = 1; break;
        case SpectrumMode::MEDIUM: mode_idx = 2; break;
        case SpectrumMode::WIDE: mode_idx = 3; break;
        case SpectrumMode::ULTRA_WIDE: mode_idx = 4; break;
        default: mode_idx = 2; break;
    }
    field_spectrum_mode_.set_selected_index(mode_idx);
    number_bandwidth_.set_value(settings.hardware_bandwidth_hz);
    number_min_freq_.set_value(settings.user_min_freq_hz);
    number_max_freq_.set_value(settings.user_max_freq_hz);
}
void HardwareSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    DroneAnalyzerSettingsManager::load(settings);
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
    settings.hardware_bandwidth_hz = number_bandwidth_.value();
    settings.user_min_freq_hz = number_min_freq_.value();
    settings.user_max_freq_hz = number_max_freq_.value();
    
    // Validate settings before saving
    if (!DroneAnalyzerSettingsManager::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }
    
    DroneAnalyzerSettingsManager::save(settings);
}
void HardwareSettingsView::update_ui_from_settings() { load_current_settings(); }
void HardwareSettingsView::update_settings_from_ui() { save_current_settings(); }
void HardwareSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Hardware settings saved");
}

// AudioSettingsView
AudioSettingsView::AudioSettingsView(NavigationView& nav) : View(), nav_(nav), audio_settings_{true, 800, 500, 50, false} {
    add_children({&checkbox_audio_enabled_, &text_audio_enabled_, &number_alert_frequency_,
                  &number_alert_duration_, &number_volume_, &checkbox_repeat_, &text_repeat_, &button_save_});
}
void AudioSettingsView::focus() { button_save_.focus(); }
void AudioSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!DroneAnalyzerSettingsManager::load(settings)) DroneAnalyzerSettingsManager::reset_to_defaults(settings);
    checkbox_audio_enabled_.set_value(settings.enable_audio_alerts);
    number_alert_frequency_.set_value(settings.audio_alert_frequency_hz);
    number_alert_duration_.set_value(settings.audio_alert_duration_ms);
}
void AudioSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    DroneAnalyzerSettingsManager::load(settings);
    settings.enable_audio_alerts = checkbox_audio_enabled_.value();
    settings.audio_alert_frequency_hz = number_alert_frequency_.value();
    settings.audio_alert_duration_ms = number_alert_duration_.value();
    
    // Validate settings before saving
    if (!DroneAnalyzerSettingsManager::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }
    
    DroneAnalyzerSettingsManager::save(settings);
}
void AudioSettingsView::update_ui_from_settings() { load_current_settings(); }
void AudioSettingsView::update_settings_from_ui() { save_current_settings(); }
void AudioSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Audio settings saved");
}

// LoadingView
LoadingView::LoadingView(NavigationView& nav, const std::string& loading_text)
    : View(), nav_(nav), loading_text_(loading_text),
      loading_text_1_{{screen_width / 2 - 50, screen_height / 2 - 10, 100, 16}, loading_text},
      loading_text_2_{{screen_width / 2 - 50, screen_height / 2 + 10, 100, 16}, ""} {
    add_children({&loading_text_1_, &loading_text_2_});
}
void LoadingView::focus() {}
void LoadingView::paint(Painter& painter) { View::paint(painter); }
void LoadingView::on_show() { start_time_ = chTimeNow(); }
void LoadingView::on_hide() {}

// ScanningSettingsView
ScanningSettingsView::ScanningSettingsView(NavigationView& nav) : View(), nav_(nav) {
    add_children({&field_scanning_mode_, &number_scan_interval_, &number_rssi_threshold_,
                  &checkbox_wideband_, &text_wideband_, &button_save_});
    button_save_.on_select = [this](Button&) { on_save_settings(); };
    checkbox_wideband_.on_select = [this](Checkbox&, bool) { on_wideband_enabled_changed(); };
    load_current_settings();
}
void ScanningSettingsView::focus() { button_save_.focus(); }
void ScanningSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!DroneAnalyzerSettingsManager::load(settings)) DroneAnalyzerSettingsManager::reset_to_defaults(settings);
    field_scanning_mode_.set_selected_index(0);
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
    
    // Validate settings before saving
    if (!DroneAnalyzerSettingsManager::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }
    
    DroneAnalyzerSettingsManager::save(settings);
}
void ScanningSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Scanning settings saved");
}
void ScanningSettingsView::on_show_presets() { nav_.display_modal("Info", "Presets menu not implemented"); }
void ScanningSettingsView::on_wideband_enabled_changed() {}
void ScanningSettingsView::update_ui_from_settings() { load_current_settings(); }
void ScanningSettingsView::update_settings_from_ui() { save_current_settings(); }

// DroneAnalyzerSettingsView
DroneAnalyzerSettingsView::DroneAnalyzerSettingsView(NavigationView& nav) : View(), nav_(nav), current_settings_{} {
    add_children({&text_title_, &button_tabbed_settings_, &button_audio_settings_, &button_hardware_settings_, &button_scanning_settings_,
                  &button_load_defaults_, &button_about_author_});
    button_tabbed_settings_.on_select = [this](Button&) { show_tabbed_settings(); };
    button_audio_settings_.on_select = [this](Button&) { show_audio_settings(); };
    button_hardware_settings_.on_select = [this](Button&) { show_hardware_settings(); };
    button_scanning_settings_.on_select = [this](Button&) { show_scanning_settings(); };
    button_load_defaults_.on_select = [this](Button&) { load_default_settings(); };
    button_about_author_.on_select = [this](Button&) { show_about_author(); };
    EnhancedSettingsManager::ensure_database_exists();
    DroneAnalyzerSettingsManager::load(current_settings_);
}
void DroneAnalyzerSettingsView::focus() { button_audio_settings_.focus(); }
void DroneAnalyzerSettingsView::paint(Painter& painter) { View::paint(painter); }
bool DroneAnalyzerSettingsView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Back) { nav_.pop(); return true; }
    return View::on_key(key);
}
bool DroneAnalyzerSettingsView::on_touch(const TouchEvent event) { return View::on_touch(event); }
void DroneAnalyzerSettingsView::on_show() { View::on_show(); }
void DroneAnalyzerSettingsView::on_hide() { View::on_hide(); }
void DroneAnalyzerSettingsView::show_tabbed_settings() {
    nav_.push<EDATabbedSettingsView>();
}
void DroneAnalyzerSettingsView::show_audio_settings() { nav_.push<AudioSettingsView>(); }
void DroneAnalyzerSettingsView::show_hardware_settings() { nav_.push<HardwareSettingsView>(); }
void DroneAnalyzerSettingsView::show_scanning_settings() { nav_.push<ScanningSettingsView>(); }
void DroneAnalyzerSettingsView::load_default_settings() {
    DroneAnalyzerSettingsManager::reset_to_defaults(current_settings_);
    DroneAnalyzerSettingsManager::save(current_settings_);
    nav_.display_modal("Reset", "Settings reset to defaults");
}
void DroneAnalyzerSettingsView::show_about_author() {
    std::string message;
    message += "About the Author\n================\n\nApplication developed for\ncivilian population safety.\n\n";
    message += "Author: Kuznetsov Maxim Sergeevich\nLocksmith at Gazprom Gazoraspredeleniye\nOrenburg branch\n\n";
    message += "Development completed in October 2025\non voluntary and altruistic principles\nby one person.\n\n";
    message += "Greetings to everyone who risks\ntheir lives for the safety of others.\n\n";
    message += "Support the author:\nCard: 2202 20202 5787 1695\nYooMoney: 41001810704697\nTON: UQCdtMxQB5zbQBOICkY90lTQQqcs8V-V28Bf2AGvl8xOc5HR\n\n";
    message += "Contact:\nTelegram: @max_ai_master";
    nav_.display_modal("About Author", message);
}

// DroneDatabaseManager
std::vector<DroneDbEntry> DroneDatabaseManager::load_database(const std::string& file_path) {
    std::vector<DroneDbEntry> entries;
    File file;
    if (!file.open(file_path, true)) return entries;
    std::string content;
    content.resize(file.size());
    file.read(content.data(), file.size());
    file.close();
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> tokens;
        std::stringstream line_ss(line);
        std::string token;
        while (std::getline(line_ss, token, ',')) tokens.push_back(token);
        DroneDbEntry entry;
        if (tokens.size() >= 3) {
            entry.freq = std::strtoull(tokens[0].c_str(), nullptr, 10);
            entry.description = tokens[2] + " (R)";
        } else if (tokens.size() >= 2) {
            entry.freq = std::strtoull(tokens[0].c_str(), nullptr, 10);
            entry.description = tokens[1];
        }
        if (entry.freq > 0) entries.push_back(entry);
    }
    return entries;
}
bool DroneDatabaseManager::save_database(const std::vector<DroneDbEntry>& entries, const std::string& file_path) {
    std::stringstream ss;
    ss << "frequency_a,frequency_b,description\n# EDA User Database\n";
    for (const auto& entry : entries) {
        if (entry.freq == 0) continue;
        std::string safe_desc = entry.description;
        std::replace(safe_desc.begin(), safe_desc.end(), ',', ' ');
        std::replace(safe_desc.begin(), safe_desc.end(), '\n', ' ');
        std::replace(safe_desc.begin(), safe_desc.end(), '\r', ' ');
        ss << entry.freq << "," << safe_desc << "\n";
    }
    std::string content = ss.str();
    File file;
    if (!file.open(file_path, false)) return false;
    file.write(content.data(), content.size());
    file.close();
    return true;
}

// DroneEntryEditorView
DroneEntryEditorView::DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, OnSaveCallback callback)
    : View(), nav_(nav), entry_(entry), on_save_(callback) {
    add_children({&text_freq_, &field_freq_, &text_desc_, &field_desc_, &button_save_, &button_cancel_});
    field_freq_.set_value(entry_.freq);
    button_save_.on_select = [this](Button&) { on_save(); };
    button_cancel_.on_select = [this](Button&) { on_cancel(); };
}
void DroneEntryEditorView::focus() { field_freq_.focus(); }
void DroneEntryEditorView::on_save() {
    DroneDbEntry new_entry;
    new_entry.freq = field_freq_.value();
    new_entry.description = entry_.description;
    if (on_save_) on_save_(new_entry);
    nav_.pop();
}
void DroneEntryEditorView::on_cancel() {
    if (on_save_) on_save_({0, ""});
    nav_.pop();
}

// DroneDatabaseListView
DroneDatabaseListView::DroneDatabaseListView(NavigationView& nav) : View(), nav_(nav), entries_() {
    add_children({&menu_view_});
    entries_ = DroneDatabaseManager::load_database();
    reload_list();
}
void DroneDatabaseListView::focus() { menu_view_.focus(); }
void DroneDatabaseListView::reload_list() {
    menu_view_.clear();
    menu_view_.add_item({"[ + ADD NEW FREQUENCY ]", Color::white(), nullptr, nullptr});
    for (const auto& entry : entries_) {
        std::string freq_str = to_string_short_freq(entry.freq);
        std::string text = freq_str + ": " + entry.description;
        menu_view_.add_item({text, Color::white(), nullptr, nullptr});
    }
}
void DroneDatabaseListView::on_entry_selected(size_t index) {
    if (index == 0) {
        DroneDbEntry empty_entry;
        nav_.push<DroneEntryEditorView>(empty_entry, [this](const DroneDbEntry& entry) {
            if (entry.freq != 0) {
                entries_.push_back(entry);
                save_changes();
                reload_list();
            }
        });
    } else {
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
void DroneDatabaseListView::save_changes() { DroneDatabaseManager::save_database(entries_); }
bool DroneDatabaseListView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Select) {
        size_t index = menu_view_.highlighted_index();
        on_entry_selected(index);
        return true;
    }
    return View::on_key(key);
}

} // namespace ui::apps::enhanced_drone_analyzer
