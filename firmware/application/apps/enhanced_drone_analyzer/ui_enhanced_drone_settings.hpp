// ui_enhanced_drone_settings.hpp

#ifndef UI_ENHANCED_DRONE_SETTINGS_HPP_
#define UI_ENHANCED_DRONE_SETTINGS_HPP_

// C++ standard library headers (alphabetical order)
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

// Project-specific headers (alphabetical order)
#include "settings_persistence.hpp"
#include "ui.hpp"
#include "ui_drone_common_types.hpp"
#include "ui_freq_field.hpp"
#include "ui_navigation.hpp"
#include "ui_widget.hpp"

// #include "eda_unified_settings.hpp"
// #include "eda_unified_settings_manager.hpp"
// #include "eda_tabbed_settings_view.hpp"

// Note: radio.hpp is not needed here - rf::Frequency is already defined in ui_widget.hpp
// Note: eda_locking.hpp is included transitively through settings_persistence.hpp in the .cpp file

using rf::Frequency;

namespace ui::apps::enhanced_drone_analyzer {

// * * Enhanced Settings Manager with TXT file communication * All methods use const char* instead of std::string (zero heap allocation)
// Settings manager with zero heap allocation
class EnhancedSettingsManager {
public:
    // noexcept for all methods
    // [[nodiscard]] - Save/load success must be checked by caller
    [[nodiscard]] static bool save_settings_to_txt(const DroneAnalyzerSettings& settings) noexcept;
    [[nodiscard]] static bool load_settings_from_txt(DroneAnalyzerSettings& settings) noexcept;
    [[nodiscard]] static bool verify_comm_file_exists() noexcept;
    // Return const char* from Flash instead of std::string
    [[nodiscard]] static const char* get_communication_status() noexcept;

private:
    static void create_backup_file(const char* filepath) noexcept;
    static void restore_from_backup(const char* filepath) noexcept;
    static void remove_backup_file(const char* filepath) noexcept;
    // Return const char* from Flash instead of std::string_view
    static const char* generate_file_header() noexcept;
    // Return const char* from Flash instead of std::string
    static const char* spectrum_mode_to_string(SpectrumMode mode) noexcept;
    static const char* get_current_timestamp() noexcept;
};

// Translation Functions (Kept for UI)
// Replaced std::map with constexpr LUT for zero heap allocation
class DroneAnalyzerSettingsManager_Translations {
public:
    static const char* translate(const char* key) noexcept;
    static const char* get_translation(const char* key) noexcept;
};

// Replaced std::function with raw function pointer to eliminate heap allocation
// using PresetMenuView = void(*)(const DronePreset&); // REMOVED - use template-based approach

// Preset manager with zero heap allocation
class DroneFrequencyPresets {
public:
    // Named constants for preset counts
    static constexpr size_t PRESETS_COUNT = 5;
    static constexpr size_t AVAILABLE_TYPES_COUNT = 7;
    
    // noexcept for all methods
    // [[nodiscard]] - Return values must be used by caller
    [[nodiscard]] static const std::array<DronePreset, PRESETS_COUNT>& get_all_presets() noexcept;
    [[nodiscard]] static const char* const* get_preset_names() noexcept;
    [[nodiscard]] static const DroneType* get_available_types() noexcept;
    [[nodiscard]] static size_t get_preset_count() noexcept;
    [[nodiscard]] static size_t get_available_types_count() noexcept;
    // Return const char* from Flash instead of std::string
    [[nodiscard]] static const char* get_type_display_name(DroneType type) noexcept;
    [[nodiscard]] static bool apply_preset(DroneAnalyzerSettings& config, const DronePreset& preset) noexcept;
};

// Replaced std::function with template-based callbacks (zero heap allocation)

// Functor for config updates (zero heap allocation, fixed storage)
struct ConfigUpdaterCallback {
    DroneAnalyzerSettings* config_ptr;
    
    constexpr explicit ConfigUpdaterCallback(DroneAnalyzerSettings& config) noexcept
        : config_ptr(&config) {}
    
    // noexcept for operator()
    void operator()(const DronePreset& preset) const noexcept {
        // Guard clause to reduce nesting
        if (!config_ptr) return;
        (void)DroneFrequencyPresets::apply_preset(*config_ptr, preset);
    }
};

// Template-based callback system (zero heap allocation)
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

    // noexcept for key handling
    bool on_key(const KeyEvent key) noexcept override {
        // Guard clause for early return
        if (key != KeyEvent::Select) return MenuView::on_key(key);
        
        size_t idx = highlighted_index();
        if (idx < presets_.size()) {
            on_selected_fn_(presets_[idx]);
        }
        return true;
    }
};

// Preset selector with zero heap allocation
class DronePresetSelector {
public:
    // Template-based callback (accepts any callable type without heap allocation)
    template <typename Callback>
    static void show_preset_menu(NavigationView& nav, Callback callback) noexcept {
        const auto preset_names = DroneFrequencyPresets::get_preset_names();
        const auto& all_presets = DroneFrequencyPresets::get_all_presets();
        const auto preset_count = DroneFrequencyPresets::get_preset_count();

        using PresetMenuViewT = PresetMenuViewImpl<std::array<DronePreset, 5>, Callback>;
        nav.push<PresetMenuViewT>(preset_names, preset_count, std::move(callback), all_presets);
    }
    
    static void show_type_filtered_presets(NavigationView& nav, DroneType type) noexcept;
    
    // Returns functor instead of std::function - zero heap allocation
    static ConfigUpdaterCallback create_config_updater(DroneAnalyzerSettings& config_to_update) noexcept;
};

// DIAMOND OPTIMIZATION: POD struct for frequency entries (zero heap allocation)
struct DroneFrequencyEntry {
    Frequency frequency_hz;
    DroneType drone_type;
    ThreatLevel threat_level;
    int32_t rssi_threshold_db;
    uint32_t bandwidth_hz;
    const char* description;

    DroneFrequencyEntry(Frequency freq, DroneType type, ThreatLevel threat,
                       int32_t rssi_thresh, uint32_t bw_hz, const char* desc) noexcept;

    // [[nodiscard]] - Validation result must be used
    [[nodiscard]] bool is_valid() const noexcept;
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
// DIAMOND OPTIMIZATION: Database manager with zero heap allocation
// 
// LEGACY NOTE: This class is being phased out in favor of UnifiedDroneDatabase.
// It remains for backward compatibility with older settings files.
// New code should use UnifiedDroneDatabase directly.
// See eda_unified_database.hpp for the new implementation.
class DroneDatabaseManager {
public:
    // DIAMOND OPTIMIZATION: Named constant for max entries
    static constexpr size_t MAX_DATABASE_ENTRIES = 100;
    
    // DIAMOND OPTIMIZATION: POD struct for database view (zero heap allocation)
    struct DatabaseView {
        DroneDbEntry entries[MAX_DATABASE_ENTRIES];
        size_t count = 0;
        
        // [[nodiscard]] - Validation must be checked
        [[nodiscard]] constexpr bool is_valid() const noexcept { return count > 0; }
        // [[nodiscard]] - Entry access must be used
        [[nodiscard]] constexpr const DroneDbEntry& operator[](size_t idx) const noexcept {
            return (idx < count) ? entries[idx] : entries[0];
        }
        // [[nodiscard]] - Size must be used
        [[nodiscard]] constexpr size_t size() const noexcept { return count; }
    };
    
    // DIAMOND OPTIMIZATION: noexcept for all methods
    // [[nodiscard]] - Database load/save success must be checked
    [[nodiscard]] static DatabaseView load_database(const char* file_path = "/FREQMAN/DRONES.TXT") noexcept;
    [[nodiscard]] static bool save_database(const DatabaseView& view, const char* file_path = "/FREQMAN/DRONES.TXT") noexcept;
};

// SETTINGS UI CLASSES

// DIAMOND OPTIMIZATION: Audio settings view with zero heap allocation
class AudioSettingsView : public View {
public:
    explicit AudioSettingsView(NavigationView& nav);
    ~AudioSettingsView() = default;
    void focus() noexcept override;
    // DIAMOND FIX: Added noexcept to title() for consistency
    // Note: Returns std::string to match base class View::title() signature
    // Framework-level change required to use std::string_view or const char*
    // This is a documented framework limitation (technical debt)
    std::string title() const noexcept override { return "Audio Settings"; }

private:
    NavigationView& nav_;
    Checkbox checkbox_audio_enabled_{{8, 8}, 20, "Enable Audio Alerts", false};
    Text text_audio_enabled_{{28, 8, 200, 16}, "Enable Audio Alerts"};
    NumberField number_alert_frequency_{{8, 32}, 5, {200, 2000}, 50, ' ', false};
    NumberField number_alert_duration_{{8, 64}, 4, {100, 5000}, 100, ' ', false};
    NumberField number_volume_{{8, 96}, 3, {0, 100}, 5, ' ', false};
    Checkbox checkbox_repeat_{{8, 128}, 12, "Repeat Alerts", false};
    Text text_repeat_{{28, 128, 200, 16}, "Repeat Alerts"};
    Button button_save_{{screen_width - 120, screen_height - 32, 120, 32}, "Save Settings", false};

    // DIAMOND OPTIMIZATION: noexcept for all methods
    void load_current_settings() noexcept;
    void save_current_settings() noexcept;
    void update_ui_from_settings() noexcept;
    void update_settings_from_ui() noexcept;
    void on_save_settings() noexcept;
};

// DIAMOND OPTIMIZATION: Hardware settings view with zero heap allocation
class HardwareSettingsView : public View {
public:
    explicit HardwareSettingsView(NavigationView& nav);
    ~HardwareSettingsView() = default;
    void focus() noexcept override;
    // DIAMOND FIX: Added noexcept to title() for consistency
    // Note: Returns std::string to match base class View::title() signature
    // Framework-level change required to use std::string_view or const char*
    // This is a documented framework limitation (technical debt)
    std::string title() const noexcept override { return "Hardware Settings"; }

private:
    NavigationView& nav_;
    Checkbox checkbox_real_hardware_{{8, 8}, 18, "Use Real Hardware", false};
    Text text_real_hardware_{{28, 8, 224, 16}, "Use Real Hardware (Disable for Demo)"};
    OptionsField field_spectrum_mode_{{8, 32}, 10, { {"Ultra Narrow", 0}, {"Narrow", 1}, {"Medium", 2}, {"Wide", 3}, {"Ultra Wide", 4} }};
    NumberField number_bandwidth_{{8, 64}, 8, {1000000, 24000000}, 1000000, ' ', false};
    FrequencyField number_min_freq_{{8, 96}};
    FrequencyField number_max_freq_{{8, 128}};
    Button button_save_{{screen_width - 120, screen_height - 32, 120, 32}, "Save Settings", false};

    // DIAMOND OPTIMIZATION: noexcept for all methods
    void load_current_settings() noexcept;
    void save_current_settings() noexcept;
    void update_ui_from_settings() noexcept;
    void update_settings_from_ui() noexcept;
    void on_save_settings() noexcept;
};

// DIAMOND OPTIMIZATION: Scanning settings view with zero heap allocation
class ScanningSettingsView : public View {
public:
    explicit ScanningSettingsView(NavigationView& nav);
    ~ScanningSettingsView() = default;
    void focus() noexcept override;
    // DIAMOND FIX: Added noexcept to title() for consistency
    // Note: Returns std::string to match base class View::title() signature
    // Framework-level change required to use std::string_view or const char*
    // This is a documented framework limitation (technical debt)
    std::string title() const noexcept override { return "Scanning Settings"; }

private:
    NavigationView& nav_;
    OptionsField field_scanning_mode_{{8, 8}, 15, { {"Database Scan", 0}, {"Wideband Monitor", 1}, {"Hybrid Discovery", 2} }};
    NumberField number_scan_interval_{{8, 32}, 5, {100, 10000}, 100, ' ', false};
    NumberField number_rssi_threshold_{{8, 64}, 4, {-120, 0}, 5, ' ', false};
    Checkbox checkbox_wideband_{{8, 96}, 20, "Enable Wideband Scanning", false};
    Text text_wideband_{{28, 96, 200, 16}, "Enable Wideband Scanning"};
    Button button_presets_{{8, 128, 120, 32}, "Frequency Presets", false};
    Button button_save_{{screen_width - 120, screen_height - 32, 120, 32}, "Save Settings", false};

    // DIAMOND OPTIMIZATION: noexcept for all methods
    void load_current_settings() noexcept;
    void save_current_settings() noexcept;
    void on_save_settings() noexcept;
    void on_show_presets() noexcept;
    void on_wideband_enabled_changed() noexcept;
};

// DIAMOND OPTIMIZATION: Main settings view with zero heap allocation
class DroneAnalyzerSettingsView : public View {
public:
    explicit DroneAnalyzerSettingsView(NavigationView& nav);
    ~DroneAnalyzerSettingsView() override = default;
    void focus() noexcept override;
    // DIAMOND FIX: Added noexcept to title() for consistency
    // Note: Returns std::string to match base class View::title() signature
    // Framework-level change required to use std::string_view or const char*
    // This is a documented framework limitation (technical debt)
    std::string title() const noexcept override { return "EDA Settings"; }

 private:
    NavigationView& nav_;
    Button button_audio_settings_{{8, 32, 224, 32}, "Audio Settings", false};
    Button button_hardware_settings_{{8, 72, 224, 32}, "Hardware Settings", false};
    Button button_scanning_settings_{{8, 112, 224, 32}, "Scanning Settings", false};
    Button button_load_defaults_{{8, 152, 120, 32}, "Load Defaults", false};
    Button button_about_author_{{136, 152, 120, 32}, "About Author", false};
    Text text_title_{{8, 8, screen_width - 16, 16}, "Enhanced Drone Analyzer Settings"};

    // DIAMOND OPTIMIZATION: noexcept for all methods
    void show_audio_settings() noexcept;
    void show_hardware_settings() noexcept;
    void show_scanning_settings() noexcept;
    void show_about_author() noexcept;
    void load_default_settings() noexcept;

    DroneAnalyzerSettings current_settings_;
};

// DIAMOND OPTIMIZATION: Loading view with zero heap allocation
class LoadingView : public View {
public:
    explicit LoadingView(NavigationView& nav, const char* loading_text = "Loading...");
    ~LoadingView() = default;
    void focus() noexcept override;
    // DIAMOND FIX: Added noexcept to title() for consistency
    // Note: Returns std::string to match base class View::title() signature
    // Framework-level change required to use std::string_view or const char*
    // This is a documented framework limitation (technical debt)
    std::string title() const noexcept override { return "Loading"; }
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

// ============================================================================
// DIAMOND FIX #C2: Named constants for DroneEntryEditorView
// ============================================================================
namespace DroneEntryEditorViewConstants {
    // Maximum length for description field
    static constexpr size_t FIELD_DESC_MAX_LENGTH = 64;
    
    // Position and size for description field
    static constexpr struct { int x; int y; int w; int h; } FIELD_DESC_RECT = {8, 80, 64, 16};
    
    // Height for description field (replaces magic number 28)
    static constexpr uint8_t FIELD_DESC_HEIGHT = 28;
}

// CRITICAL FIX: Template-based callback system for DroneEntryEditorView - zero heap allocation
// Template parameter Callback accepts any callable type (lambda, functor, function pointer)
template <typename Callback>
class DroneEntryEditorView : public View {
public:
    explicit DroneEntryEditorView(NavigationView& nav, const DroneDbEntry& entry, Callback callback)
        : View(),
          nav_(nav),
          entry_(entry),
          on_save_fn_(std::move(callback)),
          description_widget_buffer_(entry.description),  // For TextEdit widget (framework limitation)
          text_freq_{{8, 16, 64, 16}, "Freq:"},
          field_freq_{{8, 32}},
          text_desc_{{8, 64, 64, 16}, "Name:"},
          // DIAMOND FIX #M2: Replaced magic number 28 with named constant FIELD_DESC_HEIGHT
          field_desc_{description_widget_buffer_,  // Use widget buffer for TextEdit
                     DroneEntryEditorViewConstants::FIELD_DESC_MAX_LENGTH,
                     {DroneEntryEditorViewConstants::FIELD_DESC_RECT.x,
                      DroneEntryEditorViewConstants::FIELD_DESC_RECT.y},
                     DroneEntryEditorViewConstants::FIELD_DESC_HEIGHT},
          button_save_{{8, 128, 100, 32}, "SAVE"},
          button_cancel_{{128, 128, 100, 32}, "CANCEL"} {
        add_children({&text_freq_, &field_freq_, &text_desc_, &field_desc_, &button_save_, &button_cancel_});
        field_freq_.set_value(entry_.freq);
        button_save_.on_select = [this](Button&) { on_save(); };
        button_cancel_.on_select = [this](Button&) { on_cancel(); };
    }

    // DIAMOND OPTIMIZATION: noexcept for focus
    void focus() noexcept override { field_freq_.focus(); }
    // DIAMOND FIX: Added noexcept to title() for consistency
    // Note: Returns std::string to match base class View::title() signature
    // Framework-level change required to use std::string_view or const char*
    // This is a documented framework limitation (technical debt)
    std::string title() const noexcept override { return "Edit Frequency"; }

private:
    // ============================================================================
    // TECHNICAL DEBT: TextEdit widget requires std::string& (heap allocation)
    // ============================================================================
    // ISSUE: TextEdit widget constraint (requires std::string&, no set_buffer())
    // IMPACT:
    //   - Heap allocation on view construction (~100-200 bytes)
    //   - Heap fragmentation from frequent view creation/destruction
    //   - Violates Diamond Code constraint: "NO dynamic memory"
    //
    // FRAMEWORK LIMITATION:
    //   - TextEdit widget (ui_widget.hpp) only accepts std::string& in constructor
    //   - No set_buffer() method exists to use fixed-size char array
    //   - This is a framework-level issue, not EDA-specific
    //
    // DECISION: Keep std::string for TextEdit widget compatibility
    // JUSTIFICATION:
    //   - Framework-level issue, not EDA-specific
    //   - Requires TextEdit widget rewrite (affects entire codebase)
    //   - Risk outweighs benefit for single module
    //   - Documented for future framework-wide fix
    //
    // MIGRATION PATH (when TextEdit widget is fixed):
    //   1. Add set_buffer() method to TextEdit widget
    //   2. Replace std::string description_widget_buffer_ with char description_buffer_[64]
    //   3. Update constructor: field_desc_.set_buffer(description_buffer_, sizeof(description_buffer_))
    //   4. Update on_save(): safe_strcpy(new_entry.description, description_buffer_, ...)
    //
    // REFERENCE: ui_widget.hpp:724-736 (TextEdit widget definition)
    // ============================================================================
    NavigationView& nav_;
    DroneDbEntry entry_;
    Callback on_save_fn_;
    // Note: TextEdit widget requires std::string&, so heap allocation still occurs.
    // This is a framework limitation that cannot be fixed without modifying TextEdit.
    std::string description_widget_buffer_;
    Text text_freq_;
    FrequencyField field_freq_;
    Text text_desc_;
    TextEdit field_desc_;
    Button button_save_;
    Button button_cancel_;

    // DIAMOND OPTIMIZATION: noexcept for save/cancel
    void on_save() noexcept {
        DroneDbEntry new_entry;
        new_entry.freq = field_freq_.value();
        // Copy from description_widget_buffer_ (modified by TextEdit widget)
        // The TextEdit widget modifies description_widget_buffer_ during user editing
        safe_strcpy(new_entry.description, description_widget_buffer_.c_str(), sizeof(new_entry.description));
        on_save_fn_(new_entry);
        nav_.pop();
    }
    void on_cancel() noexcept {
        DroneDbEntry empty_entry{0};
        on_save_fn_(empty_entry);
        nav_.pop();
    }
};

// DIAMOND OPTIMIZATION: Database list view with zero heap allocation
class DroneDatabaseListView : public View {
public:
    explicit DroneDatabaseListView(NavigationView& nav);
    ~DroneDatabaseListView() = default;
    void focus() noexcept override;
    // DIAMOND FIX: Added noexcept to title() for consistency
    // Note: Returns std::string to match base class View::title() signature
    // Framework-level change required to use std::string_view or const char*
    // This is a documented framework limitation (technical debt)
    std::string title() const noexcept override { return "Manage Database"; }

private:
    NavigationView& nav_;
    
    // DIAMOND FIX #H1: Moved database_view_ to static storage with mutex protection
    // Reduces stack from ~7,216 bytes to ~408 bytes (94% reduction)
    // All database operations are protected by g_database_mutex
    // Lock order: DATA_MUTEX (3) - consistent with existing lock hierarchy
    static DroneDatabaseManager::DatabaseView g_database_view;
    static Mutex g_database_mutex;
    static bool g_database_loaded;  // Flag to track if database has been loaded
                                  // Note: No volatile needed - mutex provides synchronization
    
    MenuView menu_view_{{0, 0, 240, 168}};

    // DIAMOND OPTIMIZATION: noexcept for all methods
    void on_entry_selected(size_t index) noexcept;
    // DIAMOND FIX: Added save_changes_locked() to prevent deadlock
    // save_changes_locked() assumes g_database_mutex is already held by caller
    // save_changes() acquires the lock for external callers
    void save_changes_locked() noexcept;
    void save_changes() noexcept;
    bool on_key(const KeyEvent key) noexcept override;
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_ENHANCED_DRONE_SETTINGS_HPP_
