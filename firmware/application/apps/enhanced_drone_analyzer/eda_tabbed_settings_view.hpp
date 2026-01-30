/*
 * EDA Tabbed Settings View - Unified settings interface with optimized layout
 * Following Mayhem UI patterns and positioning helpers
 */

#ifndef __EDA_TABBED_SETTINGS_VIEW_HPP__
#define __EDA_TABBED_SETTINGS_VIEW_HPP__

#include "eda_unified_settings.hpp"
#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_widget.hpp"
#include "ui_tabview.hpp"
#include "ui_menu.hpp"
#include "theme.hpp"
#include "string_format.hpp"

namespace ui::apps::enhanced_drone_analyzer {

class AudioSettingsTab : public View {
public:
    explicit AudioSettingsTab(NavigationView& nav);
    ~AudioSettingsTab() = default;

    void focus() override;
    std::string title() const override { return "Audio"; }

    void load_from_settings(const AudioSettings& settings);
    AudioSettings save_to_settings();

private:
    NavigationView& nav_;

    Text text_audio_alerts_{{16, 16, 224, 16}, "Audio Alerts"};
    Checkbox checkbox_enable_{{16, 48}, 20, "Enable Alerts", false};
    Text text_enable_{{40, 64, 160, 16}, "Sound on detection"};

    Text text_frequency_{{16, 96, 120, 16}, "Frequency (Hz):"};
    NumberField field_frequency_{{16, 112}, 5, {200, 20000}, 50, ' ', false};

    Text text_duration_{{16, 144, 120, 16}, "Duration (ms):"};
    NumberField field_duration_{{16, 160}, 4, {50, 5000}, 100, ' ', false};

    Text text_volume_{{16, 192, 96, 16}, "Volume (%):"};
    NumberField field_volume_{{16, 208}, 3, {0, 100}, 5, ' ', false};

    Checkbox checkbox_repeat_{{16, 240}, 14, "Repeat Alerts", false};
    Text text_repeat_{{40, 256, 160, 16}, "Repeat until acknowledged"};

    Button button_save_{{16, 288, 160, 32}, "Save", false};
    Button button_cancel_{{144, 288, 80, 32}, "Back", false};
};

class HardwareSettingsTab : public View {
public:
    explicit HardwareSettingsTab(NavigationView& nav);
    ~HardwareSettingsTab() = default;

    void focus() override;
    std::string title() const override { return "Hardware"; }

    void load_from_settings(const HardwareSettings& settings);
    HardwareSettings save_to_settings();

private:
    NavigationView& nav_;

    Text text_hardware_{{16, 16}, "Hardware Settings"};

    Checkbox checkbox_real_hw_{{16, 48}, 18, "Real Hardware", false};
    Text text_real_hw_{{40, 64}, "Disable for demo mode"};

    Text text_spectrum_mode_{{16, 96}, "Spectrum Mode:"};
    OptionsField field_spectrum_mode_{{16, 112}, 18,
        {{"Narrow", 0}, {"Medium", 1}, {"Wide", 2}, {"Ultra Wide", 3}}};

    Text text_bandwidth_{{16, 144}, "Bandwidth (Hz):"};
    NumberField field_bandwidth_{{16, 160}, 8, {1000000, 28000000}, 1000000, ' ', false};

    Text text_min_freq_{{16, 192}, "Min Freq (Hz):"};
    NumberField field_min_freq_{{16, 208}, 10, {50000000, 7200000000ULL}, 1000000, ' ', false};

    Text text_max_freq_{{16, 240}, "Max Freq (Hz):"};
    NumberField field_max_freq_{{16, 256}, 10, {50000000, 7200000000ULL}, 1000000, ' ', false};

    Button button_save_{{16, 288, 224, 32}, "Save", false};
    Button button_cancel_{{144, 288, 96, 32}, "Back", false};
};

class ScanningSettingsTab : public View {
public:
    explicit ScanningSettingsTab(NavigationView& nav);
    ~ScanningSettingsTab() = default;

    void focus() override;
    std::string title() const override { return "Scanning"; }

    void load_from_settings(const ScanningSettings& settings);
    ScanningSettings save_to_settings();

private:
    NavigationView& nav_;

    Text text_scanning_{{16, 16}, "Scanning Settings"};

    Text text_scan_interval_{{16, 48}, "Interval (ms):"};
    NumberField field_scan_interval_{{16, 64}, 5, {100, 10000}, 100, ' ', false};

    Text text_rssi_threshold_{{16, 96}, "RSSI Threshold:"};
    NumberField field_rssi_threshold_{{16, 112}, 4, {-120, 0}, 5, ' ', false};

    Checkbox checkbox_wideband_{{16, 144}, 20, "Wideband Scan", false};
    Text text_wideband_{{40, 160}, "Scan wide frequency ranges"};

    Text text_wb_min_{{16, 192}, "WB Min (Hz):"};
    NumberField field_wb_min_{{16, 208}, 10, {2400000000ULL, 2500000000ULL}, 1000000, ' ', false};

    Text text_wb_max_{{16, 240}, "WB Max (Hz):"};
    NumberField field_wb_max_{{16, 256}, 10, {2400000000ULL, 2500000000ULL}, 1000000, ' ', false};

    Checkbox checkbox_panoramic_{{16, 288}, 15, "Panoramic", false};
    Text text_panoramic_{{40, 304}, "Save full spectrum data"};

    Button button_save_{{16, 352, 224, 32}, "Save", false};
    Button button_cancel_{{144, 352, 96, 32}, "Back", false};
};

class DetectionSettingsTab : public View {
public:
    explicit DetectionSettingsTab(NavigationView& nav);
    ~DetectionSettingsTab() = default;

    void focus() override;
    std::string title() const override { return "Detection"; }

    void load_from_settings(const DetectionSettings& settings);
    DetectionSettings save_to_settings();

private:
    NavigationView& nav_;

    Text text_detection_{{16, 16}, "Detection Settings"};

    Checkbox checkbox_fhss_{{16, 48}, 15, "FHSS Detect", false};
    Text text_fhss_{{40, 64}, "Detect frequency hopping"};

    Text text_sensitivity_{{16, 96}, "Movement Sensitivity:"};
    OptionsField field_sensitivity_{{16, 112}, 18,
        {{"Low", 0}, {"Medium", 1}, {"High", 2}, {"Very High", 3}}};

    Text text_threat_{{16, 144}, "Threat Threshold:"};
    OptionsField field_threat_{{16, 160}, 18,
        {{"All", 0}, {"Low+", 1}, {"Medium+", 2}, {"High+", 3}}};

    Checkbox checkbox_intelligent_{{16, 192}, 20, "Intelligent Track", false};
    Text text_intelligent_{{40, 208}, "Auto-track active signals"};

    Button button_save_{{16, 256, 224, 32}, "Save", false};
    Button button_cancel_{{144, 256, 96, 32}, "Back", false};
};

class LoggingSettingsTab : public View {
public:
    explicit LoggingSettingsTab(NavigationView& nav);
    ~LoggingSettingsTab() = default;

    void focus() override;
    std::string title() const override { return "Logging"; }

    void load_from_settings(const LoggingSettings& settings);
    LoggingSettings save_to_settings();

private:
    NavigationView& nav_;

    Text text_logging_{{16, 16}, "Logging Settings"};

    Checkbox checkbox_autosave_{{16, 48}, 15, "Auto Save", false};
    Text text_autosave_{{40, 64}, "Save detections immediately"};

    Text text_format_{{16, 96}, "Log Format:"};
    OptionsField field_format_{{16, 112}, 15,
        {{"CSV", 0}, {"JSON", 1}, {"TXT", 2}}};

    Text text_max_size_{{16, 144}, "Max Size (KB):"};
    NumberField field_max_size_{{16, 160}, 6, {100, 10240}, 100, ' ', false};

    Text text_path_{{16, 192}, "Log Path:"};
    std::string path_buffer_;
    TextEdit field_path_{path_buffer_, 30, {16, 208}, 224};

    Button button_save_{{16, 240, 224, 32}, "Save", false};
    Button button_cancel_{{144, 240, 96, 32}, "Back", false};
};

class DisplaySettingsTab : public View {
public:
    explicit DisplaySettingsTab(NavigationView& nav);
    ~DisplaySettingsTab() = default;

    void focus() override;
    std::string title() const override { return "Display"; }

    void load_from_settings(const DisplaySettings& settings);
    DisplaySettings save_to_settings();

private:
    NavigationView& nav_;

    Text text_display_{{16, 16}, "Display Settings"};

    Text text_color_scheme_{{16, 48}, "Color Scheme:"};
    OptionsField field_color_scheme_{{16, 64}, 15,
        {{"Dark", 0}, {"Light", 1}}};

    Text text_font_size_{{16, 96}, "Font Size:"};
    OptionsField field_font_size_{{16, 112}, 15,
        {{"Small", 0}, {"Medium", 1}, {"Large", 2}}};

    Text text_spectrum_density_{{16, 144}, "Spectrum Density:"};
    OptionsField field_density_{{16, 160}, 15,
        {{"Low", 0}, {"Medium", 1}, {"High", 2}}};

    Text text_waterfall_speed_{{16, 192}, "Waterfall Speed:"};
    NumberField field_waterfall_{{16, 208}, 2, {1, 10}, 1, ' ', false};

    Checkbox checkbox_detailed_{{16, 240}, 15, "Detailed Info", false};
    Text text_detailed_{{40, 256}, "Show full drone info"};

    Button button_save_{{16, 288, 224, 32}, "Save", false};
    Button button_cancel_{{144, 288, 96, 32}, "Back", false};
};

class EDATabbedSettingsView : public View {
public:
    explicit EDATabbedSettingsView(NavigationView& nav);
    ~EDATabbedSettingsView() = default;

    void focus() override;
    std::string title() const override { return "EDA Settings"; }

    void load_settings(const EDAUnifiedSettings& settings);
    EDAUnifiedSettings save_settings();

private:
    NavigationView& nav_;

    AudioSettingsTab tab_audio_{nav_};
    HardwareSettingsTab tab_hardware_{nav_};
    ScanningSettingsTab tab_scanning_{nav_};
    DetectionSettingsTab tab_detection_{nav_};
    LoggingSettingsTab tab_logging_{nav_};
    DisplaySettingsTab tab_display_{nav_};

    Button button_defaults_{{16, 0, 96, 16}, "Defaults", false};
    Button button_import_{{112, 0, 96, 16}, "Import", false};
    Button button_export_{{208, 0, 96, 16}, "Export", false};

    void on_defaults();
    void on_import();
    void on_export();
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __EDA_TABBED_SETTINGS_VIEW_HPP__
