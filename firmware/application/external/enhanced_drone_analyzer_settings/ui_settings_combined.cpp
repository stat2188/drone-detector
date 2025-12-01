#include "ui_settings_combined.hpp"
#include "file.hpp"
#include <sstream>
#include <cstring>

namespace ui::external_app::enhanced_drone_analyzer {

// =============================================================================
// UTILITIES
// =============================================================================

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string SettingsIO::spectrum_mode_to_string(SpectrumMode mode) {
    switch(mode) {
        case SpectrumMode::NARROW: return "NARROW";
        case SpectrumMode::MEDIUM: return "MEDIUM";
        case SpectrumMode::WIDE: return "WIDE";
        case SpectrumMode::ULTRA_WIDE: return "ULTRA_WIDE";
        default: return "MEDIUM";
    }
}

SpectrumMode SettingsIO::string_to_spectrum_mode(const std::string& str) {
    if (str == "NARROW") return SpectrumMode::NARROW;
    if (str == "WIDE") return SpectrumMode::WIDE;
    if (str == "ULTRA_WIDE") return SpectrumMode::ULTRA_WIDE;
    return SpectrumMode::MEDIUM; // Дефолт
}

// =============================================================================
// SETTINGS I/O (CORE LOGIC)
// =============================================================================

bool SettingsIO::load(DroneAnalyzerSettings& settings) {
    // 1. Сначала заполняем структуру дефолтными значениями.
    // Это гарантирует, что если файл поврежден или отсутствует поле, 
    // в структуре будет валидное значение.
    settings = DroneAnalyzerSettings(); 

    File file;
    if (!file.open(DroneAnalyzerSettings::FILE_PATH)) {
        // Файла нет - это не ошибка, просто вернем дефолтные настройки
        // Возвращаем true, чтобы интерфейс мог открыться с дефолтными значениями
        return true; 
    }

    // 2. Читаем файл целиком
    std::string content;
    content.resize(file.size());
    file.read(content.data(), file.size());
    file.close();

    // 3. Парсим и ОБНОВЛЯЕМ поля структуры
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));

        // Важно: Мы используем ту же логику, что и сканер, чтобы правильно прочитать
        if (key == "spectrum_mode") settings.spectrum_mode = string_to_spectrum_mode(val);
        else if (key == "scan_interval_ms") settings.scan_interval_ms = std::stoul(val);
        else if (key == "rssi_threshold_db") settings.rssi_threshold_db = std::stoi(val);
        else if (key == "enable_audio_alerts") settings.enable_audio_alerts = (val == "true");
        else if (key == "audio_alert_frequency_hz") settings.audio_alert_frequency_hz = std::stoul(val);
        else if (key == "audio_alert_duration_ms") settings.audio_alert_duration_ms = std::stoul(val);
        else if (key == "hardware_bandwidth_hz") settings.hardware_bandwidth_hz = std::stoul(val);
        else if (key == "enable_real_hardware") settings.enable_real_hardware = (val == "true");
    }
    
    return true;
}

bool SettingsIO::save(const DroneAnalyzerSettings& settings) {
    File file;
    // Открываем с флагом create/truncate (перезапись)
    if (!file.open(DroneAnalyzerSettings::FILE_PATH, false)) {
        return false;
    }

    std::stringstream ss;
    ss << "# EDA Settings (Edited)\n";
    // Сериализуем ВСЮ структуру целиком. 
    // Так как мы перед сохранением сделали load(), тут содержатся
    // и старые нетронутые данные, и новые измененные.
    ss << "spectrum_mode=" << spectrum_mode_to_string(settings.spectrum_mode) << "\n";
    ss << "scan_interval_ms=" << settings.scan_interval_ms << "\n";
    ss << "rssi_threshold_db=" << settings.rssi_threshold_db << "\n";
    ss << "enable_audio_alerts=" << (settings.enable_audio_alerts ? "true" : "false") << "\n";
    ss << "audio_alert_frequency_hz=" << settings.audio_alert_frequency_hz << "\n";
    ss << "audio_alert_duration_ms=" << settings.audio_alert_duration_ms << "\n";
    ss << "hardware_bandwidth_hz=" << settings.hardware_bandwidth_hz << "\n";
    ss << "enable_real_hardware=" << (settings.enable_real_hardware ? "true" : "false") << "\n";
    
    std::string content = ss.str();
    file.write(content.data(), content.size());
    file.close();
    return true;
}

// =============================================================================
// VIEW IMPLEMENTATIONS
// =============================================================================

// 1. AUDIO SETTINGS
AudioSettingsView::AudioSettingsView(NavigationView& nav) : nav_(nav) {
    add_children({&checkbox_enable_, &label_freq_, &field_freq_, &label_dur_, &field_dur_, &button_save_});
    
    // При открытии ЧИТАЕМ текущий файл
    DroneAnalyzerSettings s;
    SettingsIO::load(s);
    
    checkbox_enable_.set_value(s.enable_audio_alerts);
    field_freq_.set_value(s.audio_alert_frequency_hz);
    field_dur_.set_value(s.audio_alert_duration_ms);

    button_save_.on_select = [this](Button&) { save_ui(); };
}

void AudioSettingsView::focus() { button_save_.focus(); }

void AudioSettingsView::save_ui() {
    DroneAnalyzerSettings s;
    
    // ШАГ 1: Загружаем актуальное состояние файла с диска.
    // Это критически важно, чтобы не затереть настройки сканирования/железа,
    // которые мы не редактируем на этом экране.
    if (!SettingsIO::load(s)) {
        nav_.display_modal("Error", "Read Failed");
        return;
    }
    
    // ШАГ 2: Обновляем ТОЛЬКО поля, относящиеся к Audio
    s.enable_audio_alerts = checkbox_enable_.value();
    s.audio_alert_frequency_hz = field_freq_.value();
    s.audio_alert_duration_ms = field_dur_.value();
    
    // ШАГ 3: Сохраняем обновленную структуру целиком
    if (SettingsIO::save(s)) {
        nav_.pop(); // Успех - выходим
    } else {
        nav_.display_modal("Error", "Write Failed");
    }
}

// 2. SCANNING SETTINGS
ScanningSettingsView::ScanningSettingsView(NavigationView& nav) : nav_(nav) {
    add_children({&label_mode_, &field_mode_, &label_int_, &field_int_, &label_rssi_, &field_rssi_, &button_save_});
    
    DroneAnalyzerSettings s;
    SettingsIO::load(s); // Читаем текущие
    
    int idx = 1;
    if (s.spectrum_mode == SpectrumMode::NARROW) idx = 0;
    else if (s.spectrum_mode == SpectrumMode::WIDE) idx = 2;
    else if (s.spectrum_mode == SpectrumMode::ULTRA_WIDE) idx = 3;
    
    field_mode_.set_selected_index(idx);
    field_int_.set_value(s.scan_interval_ms);
    field_rssi_.set_value(s.rssi_threshold_db);

    button_save_.on_select = [this](Button&) { save_ui(); };
}

void ScanningSettingsView::focus() { button_save_.focus(); }

void ScanningSettingsView::save_ui() {
    DroneAnalyzerSettings s;
    // ШАГ 1: Читаем с диска, чтобы сохранить настройки аудио/железа
    SettingsIO::load(s); 
    
    // ШАГ 2: Обновляем поля сканирования
    switch(field_mode_.selected_index()) {
        case 0: s.spectrum_mode = SpectrumMode::NARROW; break;
        case 1: s.spectrum_mode = SpectrumMode::MEDIUM; break;
        case 2: s.spectrum_mode = SpectrumMode::WIDE; break;
        case 3: s.spectrum_mode = SpectrumMode::ULTRA_WIDE; break;
    }
    s.scan_interval_ms = field_int_.value();
    s.rssi_threshold_db = field_rssi_.value();
    
    // ШАГ 3: Записываем
    if(SettingsIO::save(s)) nav_.pop();
    else nav_.display_modal("Error", "Write Failed");
}

// 3. HARDWARE SETTINGS
HardwareSettingsView::HardwareSettingsView(NavigationView& nav) : nav_(nav) {
    add_children({&label_bw_, &field_bw_, &check_real_, &button_save_});
    
    DroneAnalyzerSettings s;
    SettingsIO::load(s); // Читаем
    
    field_bw_.set_value(s.hardware_bandwidth_hz);
    check_real_.set_value(s.enable_real_hardware);

    button_save_.on_select = [this](Button&) { save_ui(); };
}

void HardwareSettingsView::focus() { button_save_.focus(); }

void HardwareSettingsView::save_ui() {
    DroneAnalyzerSettings s;
    // ШАГ 1: Читаем с диска
    SettingsIO::load(s);
    
    // ШАГ 2: Обновляем
    s.hardware_bandwidth_hz = field_bw_.value();
    s.enable_real_hardware = check_real_.value();
    s.demo_mode = !check_real_.value(); // Синхронизация
    
    // ШАГ 3: Записываем
    if(SettingsIO::save(s)) nav_.pop();
    else nav_.display_modal("Error", "Write Failed");
}

// 4. MAIN MENU
DroneSettingsMainView::DroneSettingsMainView(NavigationView& nav) : nav_(nav) {
    add_children({&btn_audio_, &btn_scan_, &btn_hw_, &btn_defaults_, &text_status_});

    btn_audio_.on_select = [this](Button&) { nav_.push<AudioSettingsView>(); };
    btn_scan_.on_select = [this](Button&) { nav_.push<ScanningSettingsView>(); };
    btn_hw_.on_select = [this](Button&) { nav_.push<HardwareSettingsView>(); };
    
    btn_defaults_.on_select = [this](Button&) {
        // Кнопка сброса - тут мы намеренно перезаписываем всё дефолтом
        DroneAnalyzerSettings s = DroneAnalyzerSettings(); // Конструктор создает дефолт
        if(SettingsIO::save(s)) text_status_.set("Defaults Saved");
        else text_status_.set("Save Error");
    };
}

void DroneSettingsMainView::focus() { btn_audio_.focus(); }

} // namespace ui::external_app::enhanced_drone_analyzer
