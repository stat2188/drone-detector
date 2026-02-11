// ui_enhanced_drone_settings.hpp

#ifndef __UI_ENHANCED_DRONE_SETTINGS_HPP__
#define __UI_ENHANCED_DRONE_SETTINGS_HPP__

// ===========================================
// PART 1: COMMON TYPES (Shared with Scanner App)
// ===========================================

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <functional>

// UI framework includes
#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_widget.hpp"
#include "ui_tabview.hpp"
#include "ui_freq_field.hpp"

// Include common types from local header
#include "ui_drone_common_types.hpp"

// DIAMOND OPTIMIZATION: Unified settings persistence
#include "settings_persistence.hpp"

// DEPRECATED: Old unified settings files removed
// #include "eda_unified_settings.hpp"
// #include "eda_unified_settings_manager.hpp"
// #include "eda_tabbed_settings_view.hpp"

#include "radio.hpp"

using rf::Frequency;

namespace ui::apps::enhanced_drone_analyzer {

/**
 * PHASE 7: ENHANCED SETTINGS MANAGER WITH TXT FILE COMMUNICATION
 * Uses file.hpp API for robust SD card communication with scanner module
 */
class EnhancedSettingsManager {
public:
    static void ensure_database_exists();
    static bool save_settings_to_txt(const DroneAnalyzerSettings& settings);
    static bool verify_comm_file_exists();
    static std::string get_communication_status();

private:
    static void create_backup_file(const std::string& filepath);
    static void restore_from_backup(const std::string& filepath);
    static void remove_backup_file(const std::string& filepath);
    static std::string generate_file_header();
    static std::string generate_settings_content(const DroneAnalyzerSettings& settings);
    static std::string spectrum_mode_to_string(SpectrumMode mode);
    static std::string get_current_timestamp();
};

// ===========================================
// ACTIVE: Translation Functions (Kept for UI)
// ===========================================
class DroneAnalyzerSettingsManager_Translations {
public:
    static Language current_language_;
    static const std::map<std::string, const char*> translations_english;
    
    static void set_language(Language lang) { current_language_ = lang; }
    static Language get_language() { return current_language_; }
    static const char* translate(const std::string& key);
    static const char* get_translation(const std::string& key);
};

using PresetMenuView = std::function<void(const DronePreset&)>;
// using FilteredPresetMenuView = std::function<void(const DronePreset&, const std::vector<DronePreset>&)>; // DEPRECATED - not used

class DroneFrequencyPresets {
public:
    static constexpr size_t PRESETS_COUNT = 5;
    static constexpr size_t AVAILABLE_TYPES_COUNT = 7;
    static const std::array<DronePreset, PRESETS_COUNT>& get_all_presets();
    static const char* const* get_preset_names();
    static const DroneType* get_available_types();
    static size_t get_preset_count();
    static size_t get_available_types_count();
    static std::string get_type_display_name(DroneType type);
    static bool apply_preset(DroneAnalyzerSettings& config, const DronePreset& preset);
};

class DronePresetSelector {
public:
    static void show_preset_menu(NavigationView& nav, PresetMenuView callback);
    static void show_type_filtered_presets(NavigationView& nav, DroneType type);
    static PresetMenuView create_config_updater(DroneAnalyzerSettings& config_to_update);
};

struct DroneFrequencyEntry {
    Frequency frequency_hz;
    DroneType drone_type;
    ThreatLevel threat_level;
    int32_t rssi_threshold_db;
    uint32_t bandwidth_hz;
    const char* description;

    DroneFrequencyEntry(Frequency freq, DroneType type, ThreatLevel threat,
                       int32_t rssi_thresh, uint32_t bw_hz, const char* desc);

    bool is_valid() const;
};

using DroneFrequencyDatabase = std::vector<DroneFrequencyEntry>;

// Structure of one entry
struct DroneDbEntry {
    Frequency freq = 0;
    std::string description = "";
};

// Database manager class
class DroneDatabaseManager {
public:
    static std::vector<DroneDbEntry> load_database(const std::string& file_path = "/FREQMAN/DRONES.TXT");
    static bool save_database(const std::vector<DroneDbEntry>& entries, const std::string& file_path = "/FREQMAN/DRONES.TXT");
};

// ===========================================
// PART 3: SETTINGS UI CLASSES
// ===========================================

// ===== DEPRECATED: DroneAudioSettings =====
// All audio settings now part of DroneAnalyzerSettings:
// - enable_audio_alerts
// - audio_alert_frequency_hz
// - audio_alert_duration_ms
// - audio_volume_level
// - audio_repeat_alerts
// ===========================================

// struct DroneAudioSettings {
//     bool audio_enabled = true;
//     uint16_t alert_frequency_hz = 800;
//     uint32_t alert_duration_ms = 500;
//     uint8_t volume_level = 50;
//     bool repeat_alerts = false;
// };

class AudioSettingsView : public View {
public:
    explicit AudioSettingsView(NavigationView& nav);
    ~AudioSettingsView() = default;
    void focus() override;
    std::string title() const override { return "Audio Settings"; }

private:
    NavigationView& nav_;
    // DroneAudioSettings audio_settings_;  // DEPRECATED - now using DroneAnalyzerSettings directly
    Checkbox checkbox_audio_enabled_{{8, 8}, 20, "Enable Audio Alerts", false};
    Text text_audio_enabled_{{28, 8, 200, 16}, "Enable Audio Alerts"};
    NumberField number_alert_frequency_{{8, 32}, 5, {200, 2000}, 50, ' ', false};
    NumberField number_alert_duration_{{8, 64}, 4, {100, 5000}, 100, ' ', false};
    NumberField number_volume_{{8, 96}, 3, {0, 100}, 5, ' ', false};
    Checkbox checkbox_repeat_{{8, 128}, 12, "Repeat Alerts", false};
    Text text_repeat_{{28, 128, 200, 16}, "Repeat Alerts"};
    Button button_save_{{screen_width - 120, screen_height - 32, 120, 32}, "Save Settings", false};

    void load_current_settings();
    void save_current_settings();
    void update_ui_from_settings();
    void update_settings_from_ui();
    void on_save_settings();
};

class HardwareSettingsView : public View {
public:
    explicit HardwareSettingsView(NavigationView& nav);
    ~HardwareSettingsView() = default;
    void focus() override;
    std::string title() const override { return "Hardware Settings"; }

private:
    NavigationView& nav_;
    Checkbox checkbox_real_hardware_{{8, 8}, 18, "Use Real Hardware", false};
    Text text_real_hardware_{{28, 8, 224, 16}, "Use Real Hardware (Disable for Demo)"};
    OptionsField field_spectrum_mode_{{8, 32}, 10, { {"Ultra Narrow", 0}, {"Narrow", 1}, {"Medium", 2}, {"Wide", 3}, {"Ultra Wide", 4} }};
    NumberField number_bandwidth_{{8, 64}, 8, {1000000, 24000000}, 1000000, ' ', false};
    NumberField number_min_freq_{{8, 96}, 10, {1000000, 6000000000ULL}, 1000000, ' ', false};
    NumberField number_max_freq_{{8, 128}, 10, {1000000, 6000000000ULL}, 1000000, ' ', false};
    Button button_save_{{screen_width - 120, screen_height - 32, 120, 32}, "Save Settings", false};

    void load_current_settings();
    void save_current_settings();
    void update_ui_from_settings();
    void update_settings_from_ui();
    void on_save_settings();
};

class ScanningSettingsView : public View {
public:
    explicit ScanningSettingsView(NavigationView& nav);
    ~ScanningSettingsView() = default;
    void focus() override;
    std::string title() const override { return "Scanning Settings"; }

private:
    NavigationView& nav_;
    OptionsField field_scanning_mode_{{8, 8}, 15, { {"Database Scan", 0}, {"Wideband Monitor", 1}, {"Hybrid Discovery", 2} }};
    NumberField number_scan_interval_{{8, 32}, 5, {100, 10000}, 100, ' ', false};
    NumberField number_rssi_threshold_{{8, 64}, 4, {-120, 0}, 5, ' ', false};
    Checkbox checkbox_wideband_{{8, 96}, 20, "Enable Wideband Scanning", false};
    Text text_wideband_{{28, 96, 200, 16}, "Enable Wideband Scanning"};
    Button button_presets_{{8, 128, 120, 32}, "Frequency Presets", false};
    Button button_save_{{screen_width - 120, screen_height - 32, 120, 32}, "Save Settings", false};

    void load_current_settings();
    void save_current_settings();
    void update_ui_from_settings();
    void update_settings_from_ui();
    void on_save_settings();
    void on_show_presets();
    void on_wideband_enabled_changed();
};

class DroneAnalyzerSettingsView : public View {
public:
    explicit DroneAnalyzerSettingsView(NavigationView& nav);
    ~DroneAnalyzerSettingsView() override = default;
    void focus() override;
    std::string title() const override { return "EDA Settings"; }
    void paint(Painter& painter) override;
    bool on_key(const KeyEvent key) override;
    bool on_touch(const TouchEvent event) override;
    void on_show() override;
    void on_hide() override;

 private:
    NavigationView& nav_;
    // DEPRECATED: Tabbed settings view removed - use individual settings views instead
    // Button button_tabbed_settings_{{8, 32, 224, 32}, "Tabbed Settings", false};
    Button button_audio_settings_{{8, 32, 224, 32}, "Audio Settings", false};
    Button button_hardware_settings_{{8, 72, 224, 32}, "Hardware Settings", false};
    Button button_scanning_settings_{{8, 112, 224, 32}, "Scanning Settings", false};
    Button button_load_defaults_{{8, 152, 120, 32}, "Load Defaults", false};
    Button button_about_author_{{136, 152, 120, 32}, "About Author", false};
    Text text_title_{{8, 8, screen_width - 16, 16}, "Enhanced Drone Analyzer Settings"};

    void show_audio_settings();
    void show_hardware_settings();
    void show_scanning_settings();
    void show_about_author();
    void load_default_settings();
    // DEPRECATED: show_tabbed_settings() removed - tabbed view no longer available

    DroneAnalyzerSettings current_settings_;
};

class LoadingView : public View {
public:
    explicit LoadingView(NavigationView& nav, const std::string& loading_text = "Loading...");
    ~LoadingView() = default;
    void focus() override;
    std::string title() const override { return "Loading"; }
    void paint(Painter& painter) override;
    void on_show() override;
    void on_hide() override;

private:
    NavigationView& nav_;
    std::string loading_text_;
    Text loading_text_1_;
    Text loading_text_2_;
    systime_t start_time_ = 0;
};

class DroneEntryEditorView : public View {
public:
    using OnSaveCallback = std::function<void(const DroneDbEntry&)>;
    explicit DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, OnSaveCallback callback);
    void focus() override;
    std::string title() const override { return "Edit Frequency"; }

private:
    NavigationView& nav_;
    DroneDbEntry entry_;
    OnSaveCallback on_save_;
    Text text_freq_{{8, 16, 64, 16}, "Freq:"};
    FrequencyField field_freq_{{8, 32}};
    Text text_desc_{{8, 64, 64, 16}, "Name:"};
    TextEdit field_desc_{entry_.description, 30, {8, 80}, 28};
    Button button_save_{{8, 128, 100, 32}, "SAVE"};
    Button button_cancel_{{128, 128, 100, 32}, "CANCEL"};

    void on_save();
    void on_cancel();
};

class DroneDatabaseListView : public View {
public:
    DroneDatabaseListView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Manage Database"; }

private:
    NavigationView& nav_;
    std::vector<DroneDbEntry> entries_;
    MenuView menu_view_{{0, 0, 240, 168}};

    void reload_list();
    void on_entry_selected(size_t index);
    void save_changes();
    bool on_key(const KeyEvent key) override;
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_ENHANCED_DRONE_SETTINGS_HPP_

