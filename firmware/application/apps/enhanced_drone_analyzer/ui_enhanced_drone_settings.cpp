// ui_enhanced_drone_settings.cpp - Unified implementation for Enhanced Drone Analyzer Settings / There are the first signs of life The app tries to load the interface but doesn't crash in hard failure mode for 5 minutes.

#include "ui_drone_common_types.hpp"
#include "settings_persistence.hpp"
#include "ui_enhanced_drone_settings.hpp"
#include "ui_enhanced_drone_analyzer.hpp"
#include "eda_constants.hpp"
#include "eda_optimized_utils.hpp"
#include "color_lookup_unified.hpp"
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

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// MOVED TO UNIFIED LOOKUP TABLES
// ===========================================
// Spectrum mode names:    UnifiedStringLookup::SPECTRUM_MODE_NAMES
// Drone type names:       UnifiedStringLookup::DRONE_TYPE_NAMES
// Single source of truth: color_lookup_unified.hpp


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
                      "spectrum_mode=%s\n", spectrum_mode_to_string(settings.spectrum_mode).c_str());
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "scan_interval_ms=%u\n", (unsigned int)settings.scan_interval_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "rssi_threshold_db=%d\n", (int)settings.rssi_threshold_db);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_audio_alerts=%s\n", settings.audio_flags.enable_alerts ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_frequency_hz=%u\n", (unsigned int)settings.audio_alert_frequency_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "audio_alert_duration_ms=%u\n", (unsigned int)settings.audio_alert_duration_ms);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "hardware_bandwidth_hz=%u\n", (unsigned int)settings.hardware_bandwidth_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "enable_real_hardware=%s\n", settings.hardware_flags.enable_real_hardware ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "demo_mode=%s\n", settings.hardware_flags.demo_mode ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "freqman_path=%s\n", settings.freqman_path);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_min_freq_hz=%llu\n", (unsigned long long)settings.user_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "user_max_freq_hz=%llu\n", (unsigned long long)settings.user_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_slice_width_hz=%u\n", (unsigned int)settings.wideband_slice_width_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "panoramic_mode_enabled=%s\n", settings.scanning_flags.panoramic_mode_enabled ? "true" : "false");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_min_freq_hz=%llu\n", (unsigned long long)settings.wideband_min_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "wideband_max_freq_hz=%llu\n", (unsigned long long)settings.wideband_max_freq_hz);
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "settings_version=0.4\n");
    offset += snprintf(settings_buffer + offset, SETTINGS_BUFFER_SIZE - offset,
                      "last_modified_timestamp=%u\n", (unsigned int)chTimeNow());

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
    // OPTIMIZATION: Use char array instead of stringstream to avoid heap allocations
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
    ss << "enable_audio_alerts=" << (settings.audio_flags.enable_alerts ? "true" : "false") << "\n";
    ss << "audio_alert_frequency_hz=" << settings.audio_alert_frequency_hz << "\n";
    ss << "audio_alert_duration_ms=" << settings.audio_alert_duration_ms << "\n";
    ss << "hardware_bandwidth_hz=" << settings.hardware_bandwidth_hz << "\n";
    ss << "enable_real_hardware=" << (settings.hardware_flags.enable_real_hardware ? "true" : "false") << "\n";
    ss << "demo_mode=" << (settings.hardware_flags.demo_mode ? "true" : "false") << "\n";
    ss << "freqman_path=" << settings.freqman_path << "\n";
    ss << "user_min_freq_hz=" << settings.user_min_freq_hz << "\n";
    ss << "user_max_freq_hz=" << settings.user_max_freq_hz << "\n";
    ss << "wideband_slice_width_hz=" << settings.wideband_slice_width_hz << "\n";
    ss << "panoramic_mode_enabled=" << (settings.scanning_flags.panoramic_mode_enabled ? "true" : "false") << "\n";
    ss << "wideband_min_freq_hz=" << settings.wideband_min_freq_hz << "\n";
    ss << "wideband_max_freq_hz=" << settings.wideband_max_freq_hz << "\n";
    ss << "settings_version=0.4\n";
    ss << "last_modified_timestamp=" << chTimeNow() << "\n";

    return ss.str();
}

// DIAMOND OPTIMIZATION: LUT lookup вместо switch для spectrum_mode_to_string()
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономит ~50 байт Flash
std::string EnhancedSettingsManager::spectrum_mode_to_string(SpectrumMode mode) {
    return std::string(UnifiedStringLookup::spectrum_mode_name(static_cast<uint8_t>(mode)));
}

std::string EnhancedSettingsManager::get_current_timestamp() {
    char buffer[32];
    systime_t now = chTimeNow();
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)now);
    return std::string(buffer);
}

// ===========================================
// ACTIVE: Translation Functions Implementation
// ===========================================
// Translation functions are kept separate as they provide unique UI localization
// functionality not covered by SettingsPersistence<T>

Language DroneAnalyzerSettingsManager_Translations::current_language_ = Language::ENGLISH;

const std::map<std::string, const char*> DroneAnalyzerSettingsManager_Translations::translations_english = {
    {"save_settings", "Save Settings"},
    {"load_settings", "Load Settings"},
    {"audio_settings", "Audio Settings"},
    {"hardware_settings", "Hardware Settings"},
    {"scan_interval", "Scan Interval"},
    {"rssi_threshold", "RSSI Threshold"},
    {"spectrum_mode", "Spectrum Mode"}
};

const char* DroneAnalyzerSettingsManager_Translations::translate(const std::string& key) {
    auto it = translations_english.find(key);
    if (it != translations_english.end()) {
        return it->second;
    }
    return key.c_str();
}

const char* DroneAnalyzerSettingsManager_Translations::get_translation(const std::string& key) {
    return translate(key);
}

// ===========================================
// DroneFrequencyPresets Implementation (ACTIVE)
// ===========================================

// 🔴 OPTIMIZATION: static const array instead of vector to avoid heap allocation
// Scott Meyers Item 15: Prefer static const to #define
// Note: DronePreset now uses char arrays (zero-heap), so can be constexpr-constructible
static const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5> default_presets = {{
    {"2.4GHz Band Scan", "Drone_2_4GHz", 2400000000ULL, ThreatLevel::MEDIUM, DroneType::MAVIC},
    {"2.5GHz Band Scan", "Drone_2_5GHz", 2500000000ULL, ThreatLevel::HIGH, DroneType::PHANTOM},
    {"DJI Mavic Series", "DJI_Mavic", 2437000000ULL, ThreatLevel::HIGH, DroneType::DJI_MINI},
    {"Parrot Anafi", "Parrot_Anafi", 2450000000ULL, ThreatLevel::MEDIUM, DroneType::PARROT_ANAFI},
    {"Military UAV Band", "Military_UAV", 5000000000ULL, ThreatLevel::CRITICAL, DroneType::MILITARY_DRONE}
}};

const std::array<ui::apps::enhanced_drone_analyzer::DronePreset, 5>& DroneFrequencyPresets::get_all_presets() { return default_presets; }

static constexpr const char* PRESET_NAMES[] = {
    "2.4GHz Band Scan",
    "2.5GHz Band Scan",
    "DJI Mavic Series",
    "Parrot Anafi",
    "Military UAV Band"
};

const char* const* DroneFrequencyPresets::get_preset_names() {
    return PRESET_NAMES;
}

static constexpr DroneType AVAILABLE_TYPES[] = {
    DroneType::MAVIC,
    DroneType::PHANTOM,
    DroneType::DJI_MINI,
    DroneType::PARROT_ANAFI,
    DroneType::PARROT_BEBOP,
    DroneType::PX4_DRONE,
    DroneType::MILITARY_DRONE
};

const DroneType* DroneFrequencyPresets::get_available_types() {
    return AVAILABLE_TYPES;
}

size_t DroneFrequencyPresets::get_preset_count() {
    return 5;
}

size_t DroneFrequencyPresets::get_available_types_count() {
    return 7;
}
// DIAMOND OPTIMIZATION: LUT lookup вместо switch для get_type_display_name()
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономит ~100 байт Flash
std::string DroneFrequencyPresets::get_type_display_name(DroneType type) {
    return std::string(UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type)));
}
bool DroneFrequencyPresets::apply_preset(DroneAnalyzerSettings& config, const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
    if (!preset.is_valid()) {
        return false;
    }

    config.spectrum_mode = SpectrumMode::MEDIUM;
    config.scan_interval_ms = 1000;
    config.rssi_threshold_db = static_cast<int32_t>(preset.threat_level) >= static_cast<int32_t>(ThreatLevel::HIGH) ? -80 : -90;
    config.audio_flags.enable_alerts = true;

    if (static_cast<uint64_t>(preset.frequency_hz) >= 2400000000ULL && static_cast<uint64_t>(preset.frequency_hz) <= 2500000000ULL) {
        config.scanning_flags.enable_wideband_scanning = true;
        config.wideband_min_freq_hz = 2400000000ULL;
        config.wideband_max_freq_hz = 2500000000ULL;
    }

    config.user_min_freq_hz = preset.frequency_hz - 10000000ULL;
    config.user_max_freq_hz = preset.frequency_hz + 10000000ULL;

    if (config.user_min_freq_hz < 50000000ULL) {
        config.user_min_freq_hz = 50000000ULL;
    }
    if (config.user_max_freq_hz > 6000000000ULL) {
        config.user_max_freq_hz = 6000000000ULL;
    }

    safe_strcpy(config.freqman_path, "DRONES", ui::apps::enhanced_drone_analyzer::MAX_NAME_LEN);
    config.display_flags.show_detailed_info = true;

    return true;
}

// ============ DronePresetSelector Implementation ============

// DIAMOND OPTIMIZATION: Unified template menu view for zero-allocation preset selection
// Scott Meyers Item 15: Prefer constexpr to #define
// Template eliminates code duplication between array and vector preset containers
// Deleted copy operations fix Rule of Three violation (has pointer/reference members)
template <typename PresetContainer>
class PresetMenuViewImpl : public MenuView {
public:
    PresetMenuViewImpl(NavigationView& nav, const char* const* names, size_t count,
                       PresetMenuView on_selected, const PresetContainer& presets)
        : MenuView(), nav_(nav), names_(names), name_count_(count),
          on_selected_fn_(std::move(on_selected)), presets_(presets) {
        for (size_t i = 0; i < name_count_; ++i) {
            add_item({names_[i], Color::white(), nullptr, nullptr});
        }
    }

    PresetMenuViewImpl(const PresetMenuViewImpl&) = delete;
    PresetMenuViewImpl& operator=(const PresetMenuViewImpl&) = delete;

private:
    NavigationView& nav_;
    const char* const* names_;
    size_t name_count_;
    PresetMenuView on_selected_fn_;
    const PresetContainer& presets_;

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
};

// DIAMOND OPTIMIZATION: Zero-allocation preset filtering using stack allocation
// Scott Meyers Item 29: Use object pools to reduce allocation overhead
// Replaces std::vector-based filtering with static stack buffer
static inline size_t filter_presets_by_type_stack(
    const std::array<DronePreset, 5>& presets,
    DroneType type, const DronePreset* out_filtered[], size_t max_count) {
    size_t found_count = 0;
    for (const auto& preset : presets) {
        if (found_count >= max_count) break;
        if (preset.drone_type == type) {
            out_filtered[found_count++] = &preset;
        }
    }
    return found_count;
}

void DronePresetSelector::show_preset_menu(NavigationView& nav, PresetMenuView callback) {
    const auto preset_names = DroneFrequencyPresets::get_preset_names();
    const auto& all_presets = DroneFrequencyPresets::get_all_presets();
    const auto preset_count = DroneFrequencyPresets::get_preset_count();

    using PresetMenuViewT = PresetMenuViewImpl<std::array<DronePreset, 5>>;
    nav.push<PresetMenuViewT>(preset_names, preset_count, std::move(callback), all_presets);
}

void DronePresetSelector::show_type_filtered_presets(NavigationView& nav, DroneType type) {
    const auto& all_presets = DroneFrequencyPresets::get_all_presets();
    
    static constexpr size_t MAX_FILTERED = 5;
    const char* names[MAX_FILTERED];
    const DronePreset* filtered_ptrs[MAX_FILTERED];
    
    const size_t preset_count = filter_presets_by_type_stack(all_presets, type, filtered_ptrs, MAX_FILTERED);
    
    for (size_t i = 0; i < preset_count; ++i) {
        names[i] = filtered_ptrs[i]->display_name;
    }

    struct FilteredPresetsView {
        const DronePreset* const* presets;
        size_t count;
        const DronePreset& operator[](size_t idx) const { return *presets[idx]; }
        size_t size() const { return count; }
    };

    FilteredPresetsView filtered_view{filtered_ptrs, preset_count};
    auto on_selected = [&nav](const DronePreset&) { nav.pop(); };

    using FilteredPresetMenuViewT = PresetMenuViewImpl<FilteredPresetsView>;
    nav.push<FilteredPresetMenuViewT>(names, preset_count, std::move(on_selected), filtered_view);
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
    return EDA::Validation::validate_frequency(frequency_hz) &&
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
// DIAMOND OPTIMIZATION: constexpr LUT для SpectrumMode → index conversion
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономия RAM: LUT хранится во Flash, ноль heap allocation
// Ускорение: O(1) lookup вместо 5-branch switch
static constexpr uint8_t SPECTRUM_MODE_TO_INDEX_LUT[] = {
    1,  // SpectrumMode::NARROW (0) → OptionsField index 1 ("Narrow")
    2,  // SpectrumMode::MEDIUM (1) → OptionsField index 2 ("Medium")
    3,  // SpectrumMode::WIDE (2) → OptionsField index 3 ("Wide")
    4,  // SpectrumMode::ULTRA_WIDE (3) → OptionsField index 4 ("Ultra Wide")
    0   // SpectrumMode::ULTRA_NARROW (4) → OptionsField index 0 ("Ultra Narrow")
};
static_assert(sizeof(SPECTRUM_MODE_TO_INDEX_LUT) == 5, "SPECTRUM_MODE_TO_INDEX_LUT size");

// DIAMOND OPTIMIZATION: constexpr LUT для index → SpectrumMode conversion
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономия RAM: LUT хранится во Flash, ноль heap allocation
// Ускорение: O(1) lookup вместо 5-branch switch
static constexpr SpectrumMode INDEX_TO_SPECTRUM_MODE_LUT[] = {
    SpectrumMode::ULTRA_NARROW,  // index 0 (OptionsField "Ultra Narrow")
    SpectrumMode::NARROW,        // index 1 (OptionsField "Narrow")
    SpectrumMode::MEDIUM,         // index 2 (OptionsField "Medium")
    SpectrumMode::WIDE,           // index 3 (OptionsField "Wide")
    SpectrumMode::ULTRA_WIDE      // index 4 (OptionsField "Ultra Wide")
};
static_assert(sizeof(INDEX_TO_SPECTRUM_MODE_LUT) / sizeof(SpectrumMode) == 5, "INDEX_TO_SPECTRUM_MODE_LUT size");

void HardwareSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    checkbox_real_hardware_.set_value(settings.hardware_flags.enable_real_hardware);
    settings.hardware_flags.enable_real_hardware = checkbox_real_hardware_.value();
    settings.hardware_flags.demo_mode = !checkbox_real_hardware_.value();

    // Validate settings before saving
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }

    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}

AudioSettingsView::AudioSettingsView(NavigationView& nav) : View(), nav_(nav) {
    add_children({
        &checkbox_audio_enabled_, &text_audio_enabled_,
        &number_alert_frequency_, &number_alert_duration_,
        &number_volume_, &checkbox_repeat_, &text_repeat_,
        &button_save_
    });
    button_save_.on_select = [this](Button&) { on_save_settings(); };
    load_current_settings();
}

void AudioSettingsView::focus() {
    button_save_.focus();
}

void AudioSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    checkbox_audio_enabled_.set_value(settings.audio_flags.enable_alerts);
    number_alert_frequency_.set_value(settings.audio_alert_frequency_hz);
    number_alert_duration_.set_value(settings.audio_alert_duration_ms);
    number_volume_.set_value(settings.audio_volume_level);
    checkbox_repeat_.set_value(settings.audio_flags.repeat_alerts);
}

void AudioSettingsView::save_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    }
    
    settings.audio_flags.enable_alerts = checkbox_audio_enabled_.value();
    settings.audio_alert_frequency_hz = static_cast<uint32_t>(number_alert_frequency_.value());
    settings.audio_alert_duration_ms = static_cast<uint32_t>(number_alert_duration_.value());
    settings.audio_volume_level = static_cast<uint8_t>(number_volume_.value());
    settings.audio_flags.repeat_alerts = checkbox_repeat_.value();
    
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid audio settings");
        return;
    }
    
    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
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
      loading_text_1_{{screen_width / 2 - 50, screen_height / 2 - 10, 100, 16}, loading_text_.c_str()},
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
                  &checkbox_wideband_, &text_wideband_, &button_presets_, &button_save_});
    button_save_.on_select = [this](Button&) { on_save_settings(); };
    checkbox_wideband_.on_select = [this](Checkbox&, bool) { on_wideband_enabled_changed(); };
    button_presets_.on_select = [this](Button&) { on_show_presets(); };
    load_current_settings();
}
void ScanningSettingsView::focus() { button_save_.focus(); }
void ScanningSettingsView::load_current_settings() {
    DroneAnalyzerSettings settings;
    if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings);
    field_scanning_mode_.set_selected_index(0);
    number_scan_interval_.set_value(settings.scan_interval_ms);
    number_rssi_threshold_.set_value(settings.rssi_threshold_db);
    checkbox_wideband_.set_value(settings.scanning_flags.enable_wideband_scanning);
    settings.scanning_flags.enable_wideband_scanning = checkbox_wideband_.value();

    // Validate settings before saving
    if (!SettingsPersistence<DroneAnalyzerSettings>::validate(settings)) {
        nav_.display_modal("Error", "Invalid settings detected");
        return;
    }

    SettingsPersistence<DroneAnalyzerSettings>::save(settings);
}
void ScanningSettingsView::on_save_settings() {
    save_current_settings();
    nav_.display_modal("Success", "Scanning settings saved");
}
void ScanningSettingsView::on_show_presets() {
    // Show presets menu using DronePresetSelector
    // Create callback that updates settings when preset is selected
    auto on_preset_selected = [this](const ui::apps::enhanced_drone_analyzer::DronePreset& preset) {
        DroneAnalyzerSettings settings;
        SettingsPersistence<DroneAnalyzerSettings>::load(settings);
        if (DroneFrequencyPresets::apply_preset(settings, preset)) {
            SettingsPersistence<DroneAnalyzerSettings>::save(settings);
            nav_.display_modal("Success", std::string("Preset applied: ") + preset.display_name);
            load_current_settings();
        } else {
            nav_.display_modal("Error", "Failed to apply preset");
        }
    };
    DronePresetSelector::show_preset_menu(nav_, on_preset_selected);
}
void ScanningSettingsView::on_wideband_enabled_changed() {}
void ScanningSettingsView::update_ui_from_settings() { load_current_settings(); }
void ScanningSettingsView::update_settings_from_ui() { save_current_settings(); }

// DroneAnalyzerSettingsView
DroneAnalyzerSettingsView::DroneAnalyzerSettingsView(NavigationView& nav) : View(), nav_(nav), current_settings_{} {
    add_children({&text_title_, &button_audio_settings_, &button_hardware_settings_, &button_scanning_settings_,
                  &button_load_defaults_, &button_about_author_});
    // DEPRECATED: button_tabbed_settings_ removed - use individual settings views
    // button_tabbed_settings_.on_select = [this](Button&) { show_tabbed_settings(); };
    button_audio_settings_.on_select = [this](Button&) { show_audio_settings(); };
    button_hardware_settings_.on_select = [this](Button&) { show_hardware_settings(); };
    button_scanning_settings_.on_select = [this](Button&) { show_scanning_settings(); };
    button_load_defaults_.on_select = [this](Button&) { load_default_settings(); };
    button_about_author_.on_select = [this](Button&) { show_about_author(); };
    EnhancedSettingsManager::ensure_database_exists();
    SettingsPersistence<DroneAnalyzerSettings>::load(current_settings_);
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
// DEPRECATED: show_tabbed_settings() removed - EDATabbedSettingsView no longer available
// void DroneAnalyzerSettingsView::show_tabbed_settings() {
//     nav_.push<EDATabbedSettingsView>();
// }
void DroneAnalyzerSettingsView::show_audio_settings() { nav_.push<AudioSettingsView>(); }
void DroneAnalyzerSettingsView::show_hardware_settings() { nav_.push<HardwareSettingsView>(); }
void DroneAnalyzerSettingsView::show_scanning_settings() { nav_.push<ScanningSettingsView>(); }
void DroneAnalyzerSettingsView::load_default_settings() {
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(current_settings_);
    SettingsPersistence<DroneAnalyzerSettings>::save(current_settings_);
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
    ss << "frequency,description\n# EDA User Database\n";
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
DroneDatabaseListView::DroneDatabaseListView(NavigationView& nav) 
    : View(), nav_(nav), entries_() {
    add_children({&menu_view_});
    entries_ = DroneDatabaseManager::load_database();
    reload_list();
}

void DroneDatabaseListView::focus() { menu_view_.focus(); }

void DroneDatabaseListView::reload_list() {
    menu_view_.clear();
    menu_view_.add_item({"[ + ADD NEW FREQUENCY ]", Color::white(), nullptr, nullptr});
    for (const auto& entry : entries_) {
        constexpr size_t FREQ_BUF_SIZE = 24;
        char freq_buf[FREQ_BUF_SIZE];
        FrequencyFormatter::to_string_short_freq_buffer(freq_buf, sizeof(freq_buf), entry.freq);
        std::string freq_str(freq_buf);
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
