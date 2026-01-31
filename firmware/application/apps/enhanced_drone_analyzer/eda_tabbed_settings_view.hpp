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

    Text text_audio_alerts_{
        {16, 16, 224, 16},
        "Audio Alerts"
    };
    Checkbox checkbox_enable_{{16, 48}, 20, "Enable Alerts", false};
    Text text_enable_{
        {40, 64, 160, 16},
        "Sound on detection"
    };

    Text text_frequency_{
        {16, 96, 120, 16},
        "Frequency (Hz):"
    };
    NumberField field_frequency_{{16, 112}, 5, {200, 20000}, 50, ' ', false};

    Text text_duration_{
        {16, 144, 120, 16},
        "Duration (ms):"
    };
    NumberField field_duration_{{16, 160}, 4, {50, 5000}, 100, ' ', false};

    Text text_volume_{
        {16, 192, 96, 16},
        "Volume (%):"
    };
    NumberField field_volume_{{16, 208}, 3, {0, 100}, 5, ' ', false};

    Checkbox checkbox_repeat_{{16, 240}, 14, "Repeat Alerts", false};
    Text text_repeat_{
        {40, 256, 160, 16},
        "Repeat until acknowledged"
    };

    Button button_save_{{16, 288, 224, 32}, "Save", false};
    Button button_cancel_{{16, 328, 96, 32}, "Back", false};
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

    Text text_hardware_{
        {16, 16, 200, 16},
        "Hardware Settings"
    };

    Checkbox checkbox_real_hw_{{16, 48}, 18, "Use Real Hardware", false};
    Text text_real_hw_{
        {40, 64, 160, 16},
        "Use Real Hardware (Disable for Demo)"
    };

    Text text_spectrum_mode_{
        {16, 96, 120, 16},
        "Spectrum Mode:"
    };
    OptionsField field_spectrum_mode_{{16, 112}, 15,
        {{"Narrow", 0}, {"Medium", 1}, {"Wide", 2}, {"Ultra Wide", 3}}};

    Text text_bandwidth_{
        {16, 144, 120, 16},
        "Bandwidth (Hz):"
    };
    NumberField field_bandwidth_{{16, 160}, 8, {1000000, 24000000}, 1000000, ' ', false};

    Text text_min_freq_{
        {16, 192, 120, 16},
        "Min Freq (Hz):"
    };
    NumberField field_min_freq_{{16, 208}, 10, {1000000, 6000000000ULL}, 1000000, ' ', false};

    Text text_max_freq_{
        {16, 240, 120, 16},
        "Max Freq (Hz):"
    };
    NumberField field_max_freq_{{16, 256}, 10, {1000000, 6000000000ULL}, 1000000, ' ', false};

    Button button_save_{{16, 288, 224, 32}, "Save", false};
    Button button_cancel_{{16, 328, 96, 32}, "Back", false};
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

    Text text_scanning_{
        {16, 16, 200, 16},
        "Scanning Settings"
    };

    Text text_scan_interval_{
        {16, 48, 120, 16},
        "Scan Interval (ms):"
    };
    NumberField field_scan_interval_{{16, 64}, 5, {100, 10000}, 100, ' ', false};

    Text text_rssi_threshold_{
        {16, 96, 120, 16},
        "RSSI Threshold (dB):"
    };
    NumberField field_rssi_threshold_{{16, 112}, 4, {-120, 0}, 5, ' ', false};

    Checkbox checkbox_wideband_{{16, 144}, 20, "Enable Wideband Scanning", false};
    Text text_wideband_{
        {40, 160, 160, 16},
        "Enable Wideband Scanning"
    };

    Text text_wb_min_{
        {16, 192, 120, 16},
        "Wideband Min (Hz):"
    };
    NumberField field_wb_min_{{16, 208}, 10, {1000000, 6000000000ULL}, 1000000, ' ', false};

    Text text_wb_max_{
        {16, 240, 120, 16},
        "Wideband Max (Hz):"
    };
    NumberField field_wb_max_{{16, 256}, 10, {1000000, 6000000000ULL}, 1000000, ' ', false};

    Checkbox checkbox_panoramic_{{16, 288}, 20, "Enable Panoramic Mode", false};
    Text text_panoramic_{
        {40, 304, 160, 16},
        "Show full spectrum"
    };

    Button button_save_{{16, 336, 224, 32}, "Save", false};
    Button button_cancel_{{16, 376, 96, 32}, "Back", false};
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

    Text text_detection_{
        {16, 16, 200, 16},
        "Detection Settings"
    };

    Checkbox checkbox_fhss_{{16, 48}, 20, "Enable FHSS Detection", false};
    Text text_fhss_{
        {40, 64, 160, 16},
        "Detect frequency hopping"
    };

    Text text_sensitivity_{
        {16, 96, 120, 16},
        "Movement Sensitivity:"
    };
    OptionsField field_sensitivity_{{16, 112}, 15,
        {{"Low", 0}, {"Medium", 1}, {"High", 2}, {"Very High", 3}}};

    Text text_threat_{
        {16, 144, 120, 16},
        "Threat Level Filter:"
    };
    OptionsField field_threat_{{16, 160}, 15,
        {{"All", 0}, {"Low Only", 1}, {"Medium+", 2}, {"High+", 3}}};

    Checkbox checkbox_intelligent_{{16, 192}, 20, "Intelligent Scanning", false};
    Text text_intelligent_{
        {40, 208, 160, 16},
        "Auto-track active frequencies"
    };

    Button button_save_{{16, 240, 224, 32}, "Save", false};
    Button button_cancel_{{16, 280, 96, 32}, "Back", false};
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
    std::string path_buffer_ = "/eda_logs";

    Text text_logging_{
        {16, 16, 200, 16},
        "Logging Settings"
    };

    Checkbox checkbox_autosave_{{16, 48}, 20, "Auto-save Logs", false};
    Text text_autosave_{
        {40, 64, 160, 16},
        "Save detections immediately"
    };

    Text text_format_{
        {16, 96, 120, 16},
        "Log Format:"
    };
    OptionsField field_format_{{16, 112}, 15,
        {{"CSV", 0}, {"JSON", 1}, {"TXT", 2}}};

    Text text_max_size_{
        {16, 144, 120, 16},
        "Max Log Size (KB):"
    };
    NumberField field_max_size_{{16, 160}, 6, {100, 10240}, 100, ' ', false};

    Text text_path_{
        {16, 192, 120, 16},
        "Log Path:"
    };
    TextEdit field_path_{path_buffer_, 200, {16, 208}, 28};

    Button button_save_{{16, 256, 224, 32}, "Save", false};
    Button button_cancel_{{16, 296, 96, 32}, "Back", false};
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

    Text text_display_{
        {16, 16, 200, 16},
        "Display Settings"
    };

    Text text_color_scheme_{
        {16, 48, 120, 16},
        "Color Scheme:"
    };
    OptionsField field_color_scheme_{{16, 64}, 15,
        {{"Dark", 0}, {"Light", 1}}};

    Text text_font_size_{
        {16, 96, 120, 16},
        "Font Size:"
    };
    OptionsField field_font_size_{{16, 112}, 15,
        {{"Small", 0}, {"Medium", 1}, {"Large", 2}}};

    Text text_spectrum_density_{
        {16, 144, 120, 16},
        "Spectrum Density:"
    };
    OptionsField field_density_{{16, 160}, 15,
        {{"Low", 0}, {"Medium", 1}, {"High", 2}}};

    Text text_waterfall_speed_{
        {16, 192, 120, 16},
        "Waterfall Speed:"
    };
    NumberField field_waterfall_{{16, 208}, 2, {1, 10}, 1, ' ', false};

    Checkbox checkbox_show_ruler_{{16, 240}, 20, "Freq Ruler", false};
    Text text_show_ruler_{
        {40, 256, 200, 16},
        "Show frequency scale"
    };

    Text text_ruler_style_{
        {16, 288, 120, 16},
        "Ruler Style:"
    };
    OptionsField field_ruler_style_{{16, 304}, 15,
        {{"Auto", 0}, {"Compact GHz", 1}, {"Compact MHz", 2},
         {"Standard GHz", 3}, {"Standard MHz", 4}, {"Detailed", 5}, {"SPACED GHz", 6}}};

    Checkbox checkbox_auto_style_{{16, 336}, 14, "Auto Style", false};
    Text text_auto_style_{
        {40, 352, 160, 16},
        "Auto-select based on range"
    };

    Text text_tick_count_{
        {16, 384, 120, 16},
        "Tick Count:"
    };
    OptionsField field_tick_count_{{16, 400}, 15,
        {{"3 ticks", 3}, {"4 ticks", 4}, {"5 ticks", 5}}};

    Checkbox checkbox_detailed_{{16, 432}, 20, "Detailed Info", false};
    Text text_detailed_{
        {40, 448, 200, 16},
        "Show full drone info"
    };

    Button button_save_{{16, 480, 224, 32}, "Save", false};
    Button button_cancel_{{16, 520, 96, 32}, "Back", false};
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

    Button button_defaults_{Rect{16, 0, 96, 16}, "Defaults", false};
    Button button_import_{Rect{112, 0, 96, 16}, "Import", false};
    Button button_export_{Rect{208, 0, 96, 16}, "Export", false};

    void on_defaults();
    void on_import();
    void on_export();
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __EDA_TABBED_SETTINGS_VIEW_HPP__
