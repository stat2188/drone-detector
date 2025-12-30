// ui_settings_combined.hpp

#ifndef __UI_SETTINGS_COMBINED_HPP__
#define __UI_SETTINGS_COMBINED_HPP__

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
#include "scanner_settings.hpp"
#include "app_settings.hpp"

namespace ui::external_app::enhanced_drone_analyzer {

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

class DroneAnalyzerSettingsManager {
public:
    static bool load(DroneAnalyzerSettings& settings);
    static bool save(const DroneAnalyzerSettings& settings);
    static void reset_to_defaults(DroneAnalyzerSettings& settings);
    static bool validate(const DroneAnalyzerSettings& settings);
    static std::string serialize(const DroneAnalyzerSettings& settings);
    static bool deserialize(DroneAnalyzerSettings& settings, const std::string& data);

    static void set_language(Language lang) { current_language_ = lang; }
    static Language get_language() { return current_language_; }
    static const char* translate(const std::string& key);

    // Default translations (English)
    static const std::map<std::string, const char*> translations_english;
    static const char* get_translation(const std::string& key);

private:
    static Language current_language_;
};

struct ConfigData {
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    int32_t rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    uint32_t scan_interval_ms = 1000;
    bool enable_audio_alerts = true;
    std::string freqman_path = "DRONES";
};

class ScannerConfig {
public:
    explicit ScannerConfig(ConfigData config = {});
    ~ScannerConfig() = default;

    bool load_from_file(const std::string& filepath);
    bool save_to_file(const std::string& filepath) const;

    const ConfigData& get_data() const { return config_data_; }
    ConfigData& get_data() { return config_data_; }

    void set_frequency_range(uint32_t min_hz, uint32_t max_hz);
    void set_rssi_threshold(int32_t threshold);
    void set_scan_interval(uint32_t interval_ms);
    void set_audio_alerts(bool enabled);
    void set_freqman_path(const std::string& path);
    void set_scanning_mode(const std::string& mode);
    bool is_valid() const;

    ScannerConfig(const ScannerConfig&) = delete;
    ScannerConfig& operator=(const ScannerConfig&) = delete;

private:
    ConfigData config_data_;
};

using PresetMenuView = std::function<void(const DronePreset&)>;
using FilteredPresetMenuView = std::function<void(const DronePreset&, const std::vector<DronePreset>&)>;

class DroneFrequencyPresets {
public:
    static const std::vector<DronePreset>& get_all_presets();
    static std::vector<std::string> get_preset_names();
    static std::vector<DroneType> get_available_types();
    static std::string get_type_display_name(DroneType type);
    static std::vector<DronePreset> get_presets_of_type(const std::vector<DronePreset>& all_presets, DroneType type);
    static bool apply_preset(ScannerConfig& config, const DronePreset& preset);

private:
    static const std::vector<DronePreset> standard_presets_;
};

class DronePresetSelector {
public:
    static void show_preset_menu(NavigationView& nav, PresetMenuView callback);
    static void show_type_filtered_presets(NavigationView& nav, DroneType type);
    static PresetMenuView create_config_updater(ScannerConfig& config_to_update);
};

class SimpleDroneValidation {
public:
    static bool validate_frequency_range(Frequency freq_hz);
    static bool validate_rssi_signal(int32_t rssi_db, ThreatLevel threat);
    static ThreatLevel classify_signal_strength(int32_t rssi_db);
    static DroneType identify_drone_type(Frequency freq_hz, int32_t rssi_db);
    static bool validate_drone_detection(Frequency freq_hz, int32_t rssi_db,
                                       DroneType type, ThreatLevel threat);
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
    Frequency freq = 0;              // Init to 0
    std::string description = "";    // Init to empty string
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

struct DroneAudioSettings {
    bool audio_enabled = true;
    uint16_t alert_frequency_hz = 800;
    uint32_t alert_duration_ms = 500;
    uint8_t volume_level = 50;
    bool repeat_alerts = false;
};

class AudioSettingsView : public View {
public:
    explicit AudioSettingsView(NavigationView& nav);
    ~AudioSettingsView() = default;
    void focus() override;
    std::string title() const override { return "Audio Settings"; }

private:
    NavigationView& nav_;
    DroneAudioSettings audio_settings_;
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
    OptionsField field_spectrum_mode_{{8, 32}, 10, { {"Narrow", 0}, {"Medium", 1}, {"Wide", 2}, {"Ultra Wide", 3} }};
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
    Button button_audio_settings_{{8, 32, 120, 32}, "Audio Settings", false};
    Button button_hardware_settings_{{136, 32, 120, 32}, "Hardware Settings", false};
    Button button_scanning_settings_{{8, 72, 120, 32}, "Scanning Settings", false};
    Button button_advanced_settings_{{136, 72, 120, 32}, "Advanced Settings", false};
    Button button_load_defaults_{{8, 112, 120, 32}, "Load Defaults", false};
    Button button_about_author_{{136, 112, 120, 32}, "About Author", false};
    Button button_manage_db_{{8, 152, 224, 32}, "Manage Frequency DB", false};
    Text text_title_{{8, 8, screen_width - 16, 16}, "Enhanced Drone Analyzer Settings"};

    void show_audio_settings();
    void show_hardware_settings();
    void show_scanning_settings();
    void show_advanced_settings();
    void show_about_author();
    void load_default_settings();

    DroneAnalyzerSettings current_settings_;
};

class LoadingView : public View {
public:
    LoadingView(NavigationView& nav, const std::string& loading_text = "Loading...");
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
    DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, OnSaveCallback callback);
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

} // namespace ui::external_app::enhanced_drone_analyzer

#endif // __UI_SETTINGS_COMBINED_HPP__
