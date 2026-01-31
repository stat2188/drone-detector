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
    Checkbox checkbox_enable_{Rect{16, 48, 160, 16}, "Enable Alerts", false};
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

    Checkbox checkbox_repeat_{Rect{16, 240}, 14, "Repeat Alerts", false};
    Text text_repeat_{
        {40, 256, 160, 16},
        "Repeat until acknowledged"
    };

    Button button_save_{Rect{16, 240, 224, 32}, "Save", false};
    Button button_cancel_{Rect{144, 240, 96, 32}, "Back", false};
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

    Checkbox checkbox_detailed_{Rect{16, 240}, 15, "Detailed Info", false};
    Text text_detailed_{
        {40, 256, 200, 16},
        "Show full drone info"
    };

    Button button_save_{Rect{16, 288, 224, 32}, "Save", false};
    Button button_cancel_{Rect{144, 288, 96, 32}, "Back", false};
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
