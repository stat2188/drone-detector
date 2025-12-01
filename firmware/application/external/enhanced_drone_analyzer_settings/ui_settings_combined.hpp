#ifndef __UI_SETTINGS_COMBINED_HPP__
#define __UI_SETTINGS_COMBINED_HPP__

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_widget.hpp"
#include "ui_drone_common_types.hpp"

namespace ui::external_app::enhanced_drone_analyzer {

// Класс для Чтения и Записи файла (зеркало логики сканера)
class SettingsIO {
public:
    // Загружает настройки из файла в структуру
    static bool load(DroneAnalyzerSettings& settings);
    // Сохраняет структуру в файл в формате ключ=значение
    static bool save(const DroneAnalyzerSettings& settings);

private:
    // Вспомогательные методы для конвертации Enum <-> String
    static std::string spectrum_mode_to_string(SpectrumMode mode);
    static SpectrumMode string_to_spectrum_mode(const std::string& str);
};

// 1. Экран Аудио
class AudioSettingsView : public View {
public:
    explicit AudioSettingsView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Audio Config"; }
private:
    NavigationView& nav_;
    Checkbox checkbox_enable_{{16, 16}, 20, "Enable Alerts", false};
    Text label_freq_{{16, 50, 200, 16}, "Freq (Hz):"};
    NumberField field_freq_{{16, 68}, 4, {200, 4000}, 50, ' ', false};
    Text label_dur_{{16, 100, 200, 16}, "Duration (ms):"};
    NumberField field_dur_{{16, 118}, 4, {100, 5000}, 50, ' ', false};
    Button button_save_{{16, 160, 208, 40}, "Save"};
    void save_ui();
};

// 2. Экран Сканирования
class ScanningSettingsView : public View {
public:
    explicit ScanningSettingsView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Scan Config"; }
private:
    NavigationView& nav_;
    Text label_mode_{{16, 10, 100, 16}, "Mode:"};
    OptionsField field_mode_{{16, 28}, 12, {
        {"NARROW", 0}, {"MEDIUM", 1}, {"WIDE", 2}, {"U-WIDE", 3}
    }};
    Text label_int_{{16, 60, 200, 16}, "Interval (ms):"};
    NumberField field_int_{{16, 78}, 4, {100, 5000}, 50, ' ', false};
    Text label_rssi_{{16, 110, 200, 16}, "RSSI (dB):"};
    NumberField field_rssi_{{16, 128}, 4, {-120, -30}, 5, ' ', false};
    Button button_save_{{16, 160, 208, 40}, "Save"};
    void save_ui();
};

// 3. Экран Железа
class HardwareSettingsView : public View {
public:
    explicit HardwareSettingsView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Hardware"; }
private:
    NavigationView& nav_;
    Text label_bw_{{16, 10, 200, 16}, "Bandwidth (Hz):"};
    NumberField field_bw_{{16, 28}, 8, {2000000, 24000000}, 1000000, ' ', false};
    Checkbox check_real_{{16, 60}, 20, "Real Hardware", true};
    Button button_save_{{16, 160, 208, 40}, "Save"};
    void save_ui();
};

// 4. Главное Меню
class DroneSettingsMainView : public View {
public:
    explicit DroneSettingsMainView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "EDA Settings"; }
private:
    NavigationView& nav_;
    Button btn_audio_{{16, 16, 208, 32}, "Audio Settings"};
    Button btn_scan_{{16, 56, 208, 32}, "Scan Settings"};
    Button btn_hw_{{16, 96, 208, 32}, "Hardware Info"};
    Button btn_defaults_{{16, 144, 208, 32}, "Reset to Defaults"};
    Text text_status_{{16, 190, 208, 16}, "Ready"};
};

} // namespace

#endif
