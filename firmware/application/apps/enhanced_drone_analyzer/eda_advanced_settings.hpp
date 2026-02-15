/*
 * Enhanced Drone Analyzer - Advanced Settings UI
 * Following Mayhem firmware UI patterns
 */

#ifndef __EDA_ADVANCED_SETTINGS_HPP__
#define __EDA_ADVANCED_SETTINGS_HPP__

#include <string>
#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_widget.hpp"
#include "ui_tabview.hpp"
#include "ui_menu.hpp"
#include "theme.hpp"

#include "ui_enhanced_drone_settings.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ==========================================
// Detection Settings Tab
// ==========================================

class DetectionSettingsView : public View {
public:
    explicit DetectionSettingsView(NavigationView& nav);
    ~DetectionSettingsView() = default;

    void focus() override;
    std::string title() const override { return "Detection Settings"; }

private:
    NavigationView& nav_;
    
    Text text_title_{{8, 8, 224, 16}, "Detection Options"};
    
    Checkbox checkbox_fhss_{{8, 32}, 20, "Enable FHSS Detection"};
    Text text_fhss_{{28, 48, 200, 16}, "Detect frequency hopping"};
    
    OptionsField field_movement_sensitivity_{{8, 80}, 18,
        {{"Low", 0}, {"Medium", 1}, {"High", 2}, {"Very High", 3}}};
    Text text_movement_{{28, 104, 200, 16}, "Movement sensitivity"};
    
    OptionsField field_threat_level_{{8, 128}, 18,
        {{"All", 0}, {"Low Only", 1}, {"Medium+", 2}, {"High+", 3}}};
    Text text_threat_{{28, 152, 200, 16}, "Threat level filter"};
    
    Checkbox checkbox_intelligent_{{8, 176}, 20, "Intelligent Scanning"};
    Text text_intelligent_{{28, 192, 200, 16}, "Auto-track active freqs"};
    
    Button button_save_{{8, 208, 224, 32}, "Save"};
    
    void save_settings();
};

// ==========================================
// Logging Settings Tab
// ==========================================

class LoggingSettingsView : public View {
public:
    explicit LoggingSettingsView(NavigationView& nav);
    ~LoggingSettingsView() noexcept = default;

    void focus() override;
    std::string title() const override { return "Logging Settings"; }

private:
    NavigationView& nav_;
    
    Text text_title_{{8, 8, 224, 16}, "Logging Options"};
    
    Checkbox checkbox_auto_save_{{8, 32}, 20, "Auto-save logs"};
    Text text_auto_save_{{28, 48, 200, 16}, "Save detections immediately"};
    
    OptionsField field_log_format_{{8, 80}, 18,
        {{"CSV", 0}, {"JSON", 1}}};
    Text text_format_{{28, 104, 200, 16}, "Log file format"};
    
    NumberField number_max_size_{{8, 128}, 6,
        {100, 10240}, 100, ' '};
    Text text_max_size_{{28, 152, 200, 16}, "Max log size (KB)"};
    
    Checkbox checkbox_session_{{8, 176}, 20, "Enable Session Logging"};
    Text text_session_{{28, 192, 200, 16}, "Track scan sessions"};

    std::string log_path_buffer_;
    TextEdit field_log_path_{log_path_buffer_, 200, {8, 208}, 28};
    
    Button button_save_{{8, 244, 224, 32}, "Save"};
    
    void save_settings();
};

// ==========================================
// Display Settings Tab
// ==========================================

class DisplaySettingsView : public View {
public:
    explicit DisplaySettingsView(NavigationView& nav);
    ~DisplaySettingsView() = default;

    void focus() override;
    std::string title() const override { return "Display Settings"; }

private:
    NavigationView& nav_;
    
    Text text_title_{{8, 8, 224, 16}, "Display Options"};
    
    OptionsField field_color_scheme_{{8, 32}, 18,
        {{"Dark", 0}, {"Light", 1}}};
    Text text_color_{{28, 48, 200, 16}, "Color scheme"};
    
    OptionsField field_font_size_{{8, 80}, 18,
        {{"Small", 0}, {"Medium", 1}, {"Large", 2}}};
    Text text_font_{{28, 104, 200, 16}, "Font size"};
    
    OptionsField field_density_{{8, 128}, 18,
        {{"Low", 0}, {"Medium", 1}, {"High", 2}}};
    Text text_density_{{28, 152, 200, 16}, "Spectrum density"};
    
    NumberField field_waterfall_speed_{{8, 176}, 6,
        {1, 10}, 1, ' '};
    Text text_waterfall_{{28, 200, 200, 16}, "Waterfall speed (1-10)"};
    
    Checkbox checkbox_detailed_{{8, 224}, 20, "Show Detailed Info"};
    
    Button button_save_{{8, 256, 224, 32}, "Save"};
    
    void save_settings();
};

// ==========================================
// Advanced Settings View (Tabbed)
// ==========================================

class AdvancedSettingsView : public View {
public:
    explicit AdvancedSettingsView(NavigationView& nav);
    ~AdvancedSettingsView() = default;

    void focus() override;
    std::string title() const override { return "Advanced Settings"; }

 private:
    NavigationView& nav_;

    DetectionSettingsView view_detection_{nav_};
    LoggingSettingsView view_logging_{nav_};
    DisplaySettingsView view_display_{nav_};

    std::array<View*, 3> settings_views_{{&view_detection_, &view_logging_, &view_display_}};

    TabView tab_view_{
        {"Detection", Theme::getInstance()->fg_cyan->foreground, settings_views_[0]},
        {"Logging", Theme::getInstance()->fg_green->foreground, settings_views_[1]},
        {"Display", Theme::getInstance()->fg_yellow->foreground, settings_views_[2]}};
    
    Button button_defaults_{{0, 240, 120, 32}, "Load Defaults"};
    Button button_export_{{120, 240, 120, 32}, "Export All"};
    
    void show_detection_tab();
    void show_logging_tab();
    void show_display_tab();
    void load_defaults();
    void export_all_settings();
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __EDA_ADVANCED_SETTINGS_HPP__
