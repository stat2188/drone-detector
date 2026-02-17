// ui_enhanced_drone_settings.hpp

#ifndef __UI_ENHANCED_DRONE_SETTINGS_HPP__
#define __UI_ENHANCED_DRONE_SETTINGS_HPP__

// ===========================================
// PART 1: COMMON TYPES (Shared with Scanner App)
// ===========================================

#include <cstdint>
#include <array>
// MEDIUM PRIORITY FIX: Removed <functional> to eliminate std::function heap allocation
// std::function allocates on heap - replaced with raw function pointers

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
 * DIAMOND OPTIMIZATION: All methods use const char* instead of std::string (zero heap allocation)
 */
class EnhancedSettingsManager {
public:
    static void ensure_database_exists();
    static bool save_settings_to_txt(const DroneAnalyzerSettings& settings);
    static bool load_settings_from_txt(DroneAnalyzerSettings& settings);
    static bool verify_comm_file_exists();
    // DIAMOND OPTIMIZATION: Return const char* from Flash instead of std::string
    static const char* get_communication_status();

private:
    static void create_backup_file(const char* filepath);
    static void restore_from_backup(const char* filepath);
    static void remove_backup_file(const char* filepath);
    // DIAMOND OPTIMIZATION: Return const char* from Flash instead of std::string_view
    static const char* generate_file_header();
    // DIAMOND OPTIMIZATION: Return const char* from Flash instead of std::string
    static const char* spectrum_mode_to_string(SpectrumMode mode);
    static const char* get_current_timestamp();
};

// ===========================================
// ACTIVE: Translation Functions (Kept for UI)
// ===========================================
// DIAMOND OPTIMIZATION: Replaced std::map with constexpr LUT for zero heap allocation
class DroneAnalyzerSettingsManager_Translations {
public:
    static Language current_language_;
    
    static void set_language(Language lang) { current_language_ = lang; }
    static Language get_language() { return current_language_; }
    static const char* translate(const char* key);
    static const char* get_translation(const char* key);

private:
    struct TranslationEntry {
        const char* key;
        const char* value;
    };
    
    static constexpr TranslationEntry translations_english[] = {
        {"save_settings", "Save Settings"},
        {"load_settings", "Load Settings"},
        {"audio_settings", "Audio Settings"},
        {"hardware_settings", "Hardware Settings"},
        {"scan_interval", "Scan Interval"},
        {"rssi_threshold", "RSSI Threshold"},
        {"spectrum_mode", "Spectrum Mode"}
    };
    
    static constexpr size_t translations_count = sizeof(translations_english) / sizeof(TranslationEntry);
};

// MEDIUM PRIORITY FIX: Replaced std::function with raw function pointer to eliminate heap allocation
// std::function allocates on heap - raw function pointers are stack-only
// For stateful callbacks, use template-based approach (see DronePresetSelector below)
// using PresetMenuView = void(*)(const DronePreset&); // REMOVED - use template-based approach
// using FilteredPresetMenuView = void(*)(const DronePreset&, const std::vector<DronePreset>&); // DEPRECATED - not used

class DroneFrequencyPresets {
public:
    static constexpr size_t PRESETS_COUNT = 5;
    static constexpr size_t AVAILABLE_TYPES_COUNT = 7;
    static const std::array<DronePreset, PRESETS_COUNT>& get_all_presets();
    static const char* const* get_preset_names();
    static const DroneType* get_available_types();
    static size_t get_preset_count();
    static size_t get_available_types_count();
    // DIAMOND OPTIMIZATION: Return const char* from Flash instead of std::string
    static const char* get_type_display_name(DroneType type);
    static bool apply_preset(DroneAnalyzerSettings& config, const DronePreset& preset);
};

// CRITICAL FIX: Replaced std::function with template-based callbacks
// std::function heap-allocates when capturing - FORBIDDEN in embedded systems
// Scott Meyers Item 30: Understand the ins and outs of inlining
// Template-based approach enables compile-time polymorphism with zero heap allocation

// Functor for config updates - zero heap allocation, fixed storage
struct ConfigUpdaterCallback {
    DroneAnalyzerSettings* config_ptr;
    
    constexpr explicit ConfigUpdaterCallback(DroneAnalyzerSettings& config) noexcept 
        : config_ptr(&config) {}
    
    void operator()(const DronePreset& preset) const {
        if (config_ptr) {
            DroneFrequencyPresets::apply_preset(*config_ptr, preset);
        }
    }
};

// CRITICAL FIX: Template-based callback system - zero heap allocation
// Scott Meyers Item 46: Define non-member functions inside templates when type conversions are needed
// Template parameter Callback accepts any callable type (lambda, functor, function pointer)
// This eliminates std::function which heap-allocates when capturing state
template <typename PresetContainer, typename Callback>
class PresetMenuViewImpl : public MenuView {
public:
    PresetMenuViewImpl(NavigationView& nav, const char* const* names, size_t count,
                       Callback on_selected, const PresetContainer& presets)
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
    Callback on_selected_fn_;  // Template parameter - no heap allocation
    const PresetContainer& presets_;

    bool on_key(const KeyEvent key) override {
        if (key == KeyEvent::Select) {
            size_t idx = highlighted_index();
            if (idx < presets_.size()) {
                on_selected_fn_(presets_[idx]);
            }
            return true;
        }
        return MenuView::on_key(key);
    }
};

class DronePresetSelector {
public:
    // Template-based callback - accepts any callable type without heap allocation
    // Implementation must be in header for implicit template instantiation
    template <typename Callback>
    static void show_preset_menu(NavigationView& nav, Callback callback) {
        const auto preset_names = DroneFrequencyPresets::get_preset_names();
        const auto& all_presets = DroneFrequencyPresets::get_all_presets();
        const auto preset_count = DroneFrequencyPresets::get_preset_count();

        using PresetMenuViewT = PresetMenuViewImpl<std::array<DronePreset, 5>, Callback>;
        nav.push<PresetMenuViewT>(preset_names, preset_count, std::move(callback), all_presets);
    }
    
    static void show_type_filtered_presets(NavigationView& nav, DroneType type);
    
    // Returns functor instead of std::function - zero heap allocation
    static ConfigUpdaterCallback create_config_updater(DroneAnalyzerSettings& config_to_update);
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

// DIAMOND OPTIMIZATION: Fixed-size array instead of std::vector (zero heap allocation)
static constexpr size_t MAX_FREQUENCY_ENTRIES = 100;
using DroneFrequencyDatabase = std::array<DroneFrequencyEntry, MAX_FREQUENCY_ENTRIES>;

// Structure of one entry
// DIAMOND OPTIMIZATION: Fixed-size char array instead of std::string (zero heap allocation)
struct DroneDbEntry {
    Frequency freq = 0;
    char description[64] = "";  // 64 chars max for description
};

// Database manager class
class DroneDatabaseManager {
public:
    // DIAMOND OPTIMIZATION: Use DatabaseView instead of std::vector
    // Returns stack-allocated view (zero heap allocation)
    static constexpr size_t MAX_DATABASE_ENTRIES = 100;
    
    struct DatabaseView {
        DroneDbEntry entries[MAX_DATABASE_ENTRIES];
        size_t count = 0;
        
        constexpr bool is_valid() const noexcept { return count > 0; }
        constexpr const DroneDbEntry& operator[](size_t idx) const noexcept {
            return (idx < count) ? entries[idx] : entries[0];
        }
        constexpr size_t size() const noexcept { return count; }
    };
    
    static DatabaseView load_database(const char* file_path = "/FREQMAN/DRONES.TXT");
    static bool save_database(const DatabaseView& view, const char* file_path = "/FREQMAN/DRONES.TXT");
};

// ===========================================
// PART 3: SETTINGS UI CLASSES
// ===========================================

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

    DroneAnalyzerSettings current_settings_;
};

class LoadingView : public View {
public:
    explicit LoadingView(NavigationView& nav, const char* loading_text = "Loading...");
    ~LoadingView() = default;
    void focus() override;
    std::string title() const override { return "Loading"; }
    void paint(Painter& painter) override;
    void on_show() override;
    void on_hide() override;

private:
    NavigationView& nav_;
    // DIAMOND OPTIMIZATION: Fixed-size char array instead of std::string (zero heap allocation)
    char loading_text_[64] = "Loading...";
    Text loading_text_1_;
    Text loading_text_2_;
    systime_t start_time_ = 0;
};

// CRITICAL FIX: Template-based callback system for DroneEntryEditorView - zero heap allocation
// Template parameter Callback accepts any callable type (lambda, functor, function pointer)
template <typename Callback>
class DroneEntryEditorView : public View {
public:
    explicit DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, Callback callback)
        : View(), nav_(nav), entry_(entry), on_save_fn_(std::move(callback)),
          description_buffer_(entry.description),
          text_freq_{{8, 16, 64, 16}, "Freq:"},
          field_freq_{{8, 32}},
          text_desc_{{8, 64, 64, 16}, "Name:"},
          field_desc_{description_buffer_, {8, 80}, 28},
          button_save_{{8, 128, 100, 32}, "SAVE"},
          button_cancel_{{128, 128, 100, 32}, "CANCEL"} {
        add_children({&text_freq_, &field_freq_, &text_desc_, &field_desc_, &button_save_, &button_cancel_});
        field_freq_.set_value(entry_.freq);
        button_save_.on_select = [this](Button&) { on_save(); };
        button_cancel_.on_select = [this](Button&) { on_cancel(); };
    }

    void focus() override { field_freq_.focus(); }
    std::string title() const override { return "Edit Frequency"; }

private:
    NavigationView& nav_;
    DroneDbEntry entry_;
    Callback on_save_fn_;
    std::string description_buffer_;
    Text text_freq_;
    FrequencyField field_freq_;
    Text text_desc_;
    TextEdit field_desc_;
    Button button_save_;
    Button button_cancel_;

    void on_save() {
        DroneDbEntry new_entry;
        new_entry.freq = field_freq_.value();
        safe_strcpy(new_entry.description, description_buffer_.c_str(), sizeof(new_entry.description));
        on_save_fn_(new_entry);
        nav_.pop();
    }
    void on_cancel() {
        DroneDbEntry empty_entry{0};
        on_save_fn_(empty_entry);
        nav_.pop();
    }
};

class DroneDatabaseListView : public View {
public:
    DroneDatabaseListView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Manage Database"; }

private:
    NavigationView& nav_;
    DroneDatabaseManager::DatabaseView database_view_;
    MenuView menu_view_{{0, 0, 240, 168}};

    void reload_list();
    void on_entry_selected(size_t index);
    void save_changes();
    bool on_key(const KeyEvent key) override;
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __UI_ENHANCED_DRONE_SETTINGS_HPP__


