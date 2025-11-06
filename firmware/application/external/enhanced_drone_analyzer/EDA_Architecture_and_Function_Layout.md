# Enhanced Drone Analyzer (EDA) - Детальная Архитектура и Схема Работы

## Обзор Архитектуры

Enhanced Drone Analyzer (EDA) - это комплексная система обнаружения дронов, интегрированная в Mayhem Firmware для PortaPack. Архитектура состоит из нескольких модулей с четким разделением ответственности:

- **Сигнальная обработка**: WidebandMedianFilter, DetectionRingBuffer
- **Аудио система**: AudioAlertManager, AudioManager
- **Настройки**: ScannerSettingsManager, SpectrumPresetLoader
- **Сканер**: DroneScanner, DroneHardwareController
- **Пользовательский интерфейс**: Умные компоненты для отображения и взаимодействия

## Полный Список Функций (Верифицированный)

### Модуль: ui_signal_processing
#### WidebandMedianFilter (Фильтр шума широкополосный)
```cpp
void add_sample(int16_t rssi)                    // Добавление RSSI в окно фильтра
int16_t get_median_threshold() const               // Получение медианного порога
void reset()                                      // Сброс фильтра
```
#### DetectionRingBuffer (Кольцевой буфер обнаружений)
```cpp
void update_detection(size_t frequency_hash, int32_t rssi_value)  // Обновление обнаружения
uint8_t get_detection_count(size_t frequency_hash) const          // Получение счетчика обнаружений
int32_t get_rssi_value(size_t frequency_hash) const               // Получение значения RSSI
void clear()                                                      // Очистка буфера
```

### Модуль: ui_drone_audio
#### AudioAlertManager (Менеджер аудио алертов)
```cpp
static void play_alert(AlertLevel level)           // Воспроизведение алерта
static void set_enabled(bool enable)               // Включение/выключение аудио
static bool is_enabled()                           // Проверка статуса аудио
```
#### AudioManager (Управление аудио)
```cpp
bool is_audio_enabled() const                      // Проверка статуса аудио
void toggle_audio()                                // Переключение аудио
void play_detection_beep(ThreatLevel threat)       // Воспроизведдения сигнала обнаружения
void stop_audio()                                  // Остановка аудио
uint16_t get_alert_frequency() const                // Получение частоты алерта
void set_alert_frequency(uint16_t freq)             // Установка частоты алерта
uint32_t get_alert_duration_ms() const              // Получение длительности
void set_alert_duration_ms(uint32_t duration)       // Установка длительности
```

### Модуль: scanner_settings
#### ScannerSettingsManager (Менеджер настроек сканера)
```cpp
template<typename T> T validate_range(T value, T min_val, T max_val)  // Валидация диапазона
void reset_to_defaults(DroneAnalyzerSettings& settings)              // Сброс настроек по умолчанию
SpectrumMode parse_spectrum_mode(const std::string& value)            // Парсинг режима спектра
std::string trim_line(const std::string& line)                        // Обрезка строки
bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line)  // Парсинг ключ-значение
bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content)  // Парсинг контента
bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings)  // Загрузка из файла
bool load_settings_from_txt(DroneAnalyzerSettings& settings)          // Основная функция загрузки
```

### Модуль: ui_spectrum_settings
#### IQCalibrationSettings (Калибровка IQ)
```cpp
void set_phase_value(uint8_t value)                 // Установка фазового значения
uint8_t get_phase_value() const                     // Получение фазового значения
```
#### AmplifierControl (Управление усилителями)
```cpp
bool is_valid_lna_gain() const                      // Валидация усиления LNA
bool is_valid_vga_gain() const                      // Валидация усиления VGA
void set_lna_gain(uint8_t gain)                     // Установка усиления LNA
void set_vga_gain(uint8_t gain)                     // Установка усиления VGA
```
#### RangeLockSettings (Блокировка диапазона)
```cpp
void lock_range(Frequency min, Frequency max)       // Блокировка диапазона
void unlock_range()                                 // Разблокировка диапазона
bool is_in_locked_range() const                     // Проверка блокировки
```
#### SpectrumAnalyzerSettings (Настройки анализатора спектра)
```cpp
void add_preset(const FrequencyPreset& preset)      // Добавление предустановки
void set_frequency_range(Frequency min, Frequency max)  // Установка диапазона частот
bool load_preset(size_t index)                       // Загрузка предустановки
const FrequencyPreset* get_current_preset() const   // Получение текущей предустановки
```
#### SpectrumPresetLoader (Загрузчик предустановок спектра)
```cpp
explicit SpectrumPresetLoader(const std::string& preset_path = "EDA_PRESETS")
~SpectrumPresetLoader()
void initialize_default_presets()                   // Инициализация предустановок по умолчанию
bool load_presets_from_file()                        // Загрузка предустановок из файла
bool save_presets_to_file() const                    // Сохранение предустановок в файл
bool parse_preset_line(const std::string& line, FrequencyPreset& preset)  // Парсинг предустановки
std::string serialize_preset(const FrequencyPreset& preset) const  // Сериализация предустановки
std::string get_preset_filename() const              // Получение имени файла предустановки
const FrequencyPreset* get_preset(size_t index) const  // Получение предустановки по индексу
bool add_custom_preset(const FrequencyPreset& preset)  // Добавление пользовательской предустановки
```

### Модуль: ui_scanner_combined
#### DetectionRingBuffer (дублированный)
```cpp
void update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value)
uint8_t get_detection_count(size_t frequency_hash) const
int32_t get_rssi_value(size_t frequency_hash) const
void clear()
```
#### DroneDetectionLogger (Логгер обнаружений)
```cpp
void start_session()                                // Начало сессии логирования
void end_session()                                  // Окончание сессии логирования
bool log_detection(const DetectionLogEntry& entry)  // Логирование обнаружения
std::string get_log_filename() const                // Получение имени лог-файла
bool is_session_active() const                      // Проверка активности сессии
bool ensure_csv_header()                            // Обеспечение заголовка CSV
std::string format_csv_entry(const DetectionLogEntry& entry)  // Форматирование записи CSV
std::string format_session_summary(size_t scan_cycles, size_t total_detections) const  // Форматирование сводки
std::string generate_log_filename() const           // Генерация имени лог-файла
```
#### DroneScanner (Сканер дронов)
```cpp
void start_scanning()                               // Начало сканирования
void stop_scanning()                                 // Остановка сканирования
bool is_scanning_active() const                      // Проверка активности сканирования
bool load_frequency_database()                       // Загрузка базы данных частот
size_t get_database_size() const                     // Получение размера базы данных
void set_scanning_mode(ScanningMode mode)            // Установка режима сканирования
std::string scanning_mode_name() const               // Имя режима сканирования
void perform_scan_cycle(DroneHardwareController& hardware)  // Выполнение цикла сканирования
void perform_database_scan_cycle(DroneHardwareController& hardware)  // Цикл сканирования БД
void perform_wideband_scan_cycle(DroneHardwareController& hardware)  // Широкополосный цикл
void process_rssi_detection(const freqman_entry& entry, int32_t rssi)  // Обработка RSSI
void update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level)  // Обновление отслеживаемого дрона
void remove_stale_drones()                           // Удаление устаревших дронов
Frequency get_current_scanning_frequency() const    // Получение текущей частоты
const TrackedDrone& getTrackedDrone(size_t index) const  // Получение отслеживаемого дрона
std::string get_session_summary() const             // Получение сводки сессии
size_t get_approaching_count() const                // Количество приближающихся
size_t get_receding_count() const                   // Количество удаляющихся
size_t get_static_count() const                     // Количество статичных
uint32_t get_total_detections() const               // Общее количество обнаружений
bool is_real_mode() const                           // Проверка реального режима
```
#### DroneHardwareController (Контроллер аппаратного обеспечения)
```cpp
void initialize_hardware()                          // Инициализация аппаратного обеспечения
void on_hardware_show()                             // Обработка показа аппаратного обеспечения
void on_hardware_hide()                             // Обработка скрытия аппаратного обеспечения
void shutdown_hardware()                            // Выключение аппаратного обеспечения
void set_spectrum_mode(SpectrumMode mode)            // Установка режима спектра
uint32_t get_spectrum_bandwidth() const              // Получение ширины полосы
void set_spectrum_center_frequency(Frequency center_freq)  // Установка центральной частоты
bool tune_to_frequency(Frequency frequency_hz)      // Настройка на частоту
void start_spectrum_streaming()                      // Начало стриминга спектра
void stop_spectrum_streaming()                       // Остановка стриминга спектра
int32_t get_real_rssi_from_hardware(Frequency target_frequency)  // Получение RSSI с аппаратуры
void handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message)  // Обработка конфигурации спектра
void process_channel_spectrum_data(const ChannelSpectrum& spectrum)  // Обработка данных спектра
```
#### SmartThreatHeader (Умный заголовок угроз)
```cpp
void update(ThreatLevel max_threat, size_t approaching, size_t static_count, size_t receding,
           Frequency current_freq, bool is_scanning)  // Обновление
void set_max_threat(ThreatLevel threat)             // Установка максимальной угрозы
void set_movement_counts(size_t approaching, size_t static_count, size_t receding)  // Установка счетчиков движения
void set_current_frequency(Frequency freq)           // Установка текущей частоты
void set_scanning_state(bool is_scanning)            // Установка состояния сканирования
Color get_threat_bar_color(ThreatLevel level) const  // Получение цвета полосы угрозы
Color get_threat_text_color(ThreatLevel level) const  // Получение цвета текста угрозы
std::string get_threat_icon_text(ThreatLevel level) const  // Получение текста иконки
```
#### ThreatCard (Карточка угрозы)
```cpp
void update_card(const DisplayDroneEntry& drone)    // Обновление карточки
void clear_card()                                   // Очистка карточки
std::string render_compact() const                   // Компактный рендер
Color get_card_bg_color() const                      // Получение цвета фона
Color get_card_text_color() const                    // Получение цвета текста
```
#### DroneDisplayController (Контроллер отображения дронов)
```cpp
void update_detection_display(const DroneScanner& scanner)  // Обновление отображения обнаружений
void set_scanning_status(bool active, const std::string& message)  // Установка статуса сканирования
void set_frequency_display(Frequency freq)           // Установка отображения частоты
void add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi)  // Добавление дрона
void update_drones_display(const DroneScanner& scanner)  // Обновление отображения дронов
void initialize_mini_spectrum()                      // Инициализация мини-спектра
void process_mini_spectrum_data(const ChannelSpectrum& spectrum)  // Обработка данных спектра
bool process_bins(uint8_t* power_level)             // Обработка бинов
void render_mini_spectrum()                          // Рендер мини-спектра
void clear_spectrum_buffers()                        // Очистка буферов спектра
size_t frequency_to_spectrum_bin(Frequency freq_hz) const  // Преобразование частоты в бин
```

### Модуль: ui_settings_combined.cpp
#### EnhancedSettingsManager (Расширенный менеджер настроек)
```cpp
static bool save_settings_to_txt(const DroneAnalyzerSettings& settings)
static bool verify_comm_file_exists()
static std::string get_communication_status()
static void create_backup_file(const std::string& filepath)
static void restore_from_backup(const std::string& filepath)
static void remove_backup_file(const std::string& filepath)
static std::string generate_file_header()
static std::string generate_settings_content(const DroneAnalyzerSettings& settings)
static std::string spectrum_mode_to_string(SpectrumMode mode)
static std::string get_current_timestamp()
```
#### DroneAnalyzerSettingsManager (Менеджер настроек EDA)
```cpp
static bool load(DroneAnalyzerSettings& settings)
static bool save(const DroneAnalyzerSettings& settings)
static void reset_to_defaults(DroneAnalyzerSettings& settings)
static bool validate(const DroneAnalyzerSettings& settings)
static std::string serialize(const DroneAnalyzerSettings& settings)
static bool deserialize(DroneAnalyzerSettings& settings, const std::string& data)
static void set_language(Language lang)
static Language get_language()
static const char* translate(const std::string& key)
static const char* get_translation(const std::string& key)
```

## Экстремально Детальная Схема Работы

### Фаза 1: Инициализация Системы
```cpp
EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView()
├── Инициализация NavigationView
├── Создание компонентов:
│   ├── hardware_ = DroneHardwareController(DEFAULT_MODE)
│   ├── scanner_ = DroneScanner()
│   ├── audio_ = AudioManager(enabled=true)
│   └── display_controller_ = DroneDisplayController(nav)
├── Инициализация UI:
│   ├── button_start_stop = Button("Start Scan")
│   ├── button_menu = Button("Menu")
│   └── status_bar = ConsoleStatusBar()
└── initialize_modern_layout(): Позиционирование 240x320 UI сетки

ScannerSettingsManager::load_settings_from_txt()
├── File::open("/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt", MODE_READ)
├── parse_settings_content():
│   ├── while(getline(content, line))
│   │   ├── trim_line(line) removes spaces
│   │   └── parse_key_value() maps "key=value"
│   └── validate_range<>() clamps to [min,max]
└── reset_to_defaults() if file missing/empty
    ├── spectrum_mode = MEDIUM
    ├── rssi_threshold_db = -90
    └── audio_enabled = true, freq=800Hz, duration=200ms

DroneHardwareController::initialize_hardware()
├── set_spectrum_mode(SpectrumMode::MEDIUM) → BW=12000000 Hz
├── initialize_radio_state():
│   ├── receiver_model.enable()
│   ├── set_modulation(SpectrumAnalysis)
│   └── set_sampling_rate(24000000)
└── initialize_spectrum_collector(): ChannelSpectrumFIFO allocation
```

### Фаза 2: Главный Цикл Сканирования
```cpp
handle_start_stop_button()
├── button_start_stop.set_text("Scanning...")
├── ScanningCoordinator::start_coordinated_scanning()
│   ├── scanner_.start_scanning() → spawn scanning_thread()
│   ├── hardware_.start_spectrum_streaming() → baseband::spectrum_streaming_start()
│   └── timer for UI updates every 100ms

DroneScanner::scanning_thread() [Dedicated Thread]
├── while(scanning_active_)
│   ├── perform_scan_cycle(hardware_)
│   │   ├── DATABASE MODE:
│   │   │   ├── current_index = (current_index + 1) % db.size()
│   │   │   ├── freq_entry = database[current_index]
│   │   │   ├── hardware.tune_to_frequency(freq_entry.frequency)
│   │   │   ├── chThdSleepMilliseconds(10) // stabilization
│   │   │   ├── rssi = hardware.get_real_rssi_from_hardware(freq_entry.frequency)
│   │   │   └── process_rssi_detection(freq_entry, rssi)
│   │   │
│   │   ├── WIDEBAND MODE:
│   │   │   ├── slices = divide_range_25MHz(min_freq, max_freq, MAX_SLICES=20)
│   │   │   ├── for each slice:
│   │   │   │   ├── hardware.tune_to_frequency(slice.center)
│   │   │   │   ├── collect spectrum data through FIFO
│   │   │   │   └── process_wideband_detection(..., threshold_override)
│   │   │
│   │   └── HYBRID MODE: combine both above
│   │
│   ├── remove_stale_drones()
│   │   └── erase_if(last_seen > STALE_TIMEOUT)
│   │
│   └── update_tracking_counts()
│       ├── scan_cycles_++
│       ├── analyze TrackedDrone::get_trend()
│       └── classify approaching/static/receding
│
└── chThdSleepMilliseconds(scan_interval_ms_) // ~750-1000ms

process_rssi_detection(freqman_entry& entry, int32_t rssi)
├── WidebandMedianFilter::add_sample(rssi & 0xFFFF)
├── threshold = get_median_threshold() // window[WINDOW_SIZE/2] - HYSTERESIS
├── if(rssi >= threshold - MIN_DETECTION_COUNT)
│   ├── threat = classify_signal_strength(rssi)
│   │   ├── rssi > -60dB → HIGH
│   │   ├── rssi > -75dB → MEDIUM
│   │   └── else LOW
│   ├── drone_type = identify_drone_type(entry.frequency, rssi)
│   ├── update_tracked_drone(type, entry.frequency, rssi, threat)
│   └── total_detections_++
└── DetectionRingBuffer::update_detection(hash(entry.frequency), detection_count++, rssi)
```

### Фаза 3: Обработка Спектральных Данных
```cpp
ChannelSpectrum Message Handler
├── DroneHardwareController::handle_channel_spectrum()
│   ├── while(spectrum_fifo_->out_readable())
│   │   ├── spectrum_fifo_->out_read(&spectrum)
│   │   └── process_channel_spectrum_data(spectrum)
│   │       ├── for each bin in spectrum.bins[0..bins_nb-1]:
│   │       │   ├── apply IQ calibration (phase_value)
│   │       │   ├── apply amplifier gains (lna, vga, rf)
│   │       │   └── WidebandMedianFilter::add_sample(rssi_bin)
│   │       │
│   │       └── DroneDisplayController::process_mini_spectrum_data(spectrum)
│   │           ├── process_bins(): convert RSSI to power levels
│   │           ├── find max_power in current bin
│   │           └── add_spectrum_pixel_from_bin(max_power)
│   │               └── spectrum_row_[pixel_index++] = gradient_color

DroneDisplayController::render_mini_spectrum()
├── for each pixel in spectrum_row_[0..MINI_SPECTRUM_WIDTH-1]:
│   ├── draw line from bottom to top (MINI_SPECTRUM_HEIGHT)
│   ├── color spectrum based on power level (blue→red gradient)
│   └── if frequency matches detected_drones:
│       └── overlay yellow threat indicator
└── update every frame through UI refresh cycle
```

### Фаза 4: Пользовательский Интерфейс Обновления
```cpp
handle_scanner_update() [UI Thread Timer]
├── SmartThreatHeader::update()
│   ├── max_threat = scanner.get_max_detected_threat()
│   ├── movement_counts = scanner.get_[approaching/static/receding]_count()
│   ├── current_freq = scanner.get_current_scanning_frequency()
│   └── set_scanning_state(scanner.is_scanning_active())

ThreatCard[3]::update_card()
├── sort_drones by RSSI descending
├── for i in 0..2: if i < total_drones:
│   ├── update with display_drones[i]
│   └── set background color by threat (green/yellow/red)
└── clear unused cards

ConsoleStatusBar - dynamic status
├── SCANNING: show_progress(current_progress_percent, scan_cycles, detections)
├── ALERT: "MULTIPLE DRONES | threat: HIGH | total: {}"
└── NORMAL: "SYSTEM READY | DB: loaded | threshold: {}dB"

Audio Alarm System
├── update_tracked_drone() → validate_detection_simple()
│   ├── if threat_level > NONE && rssi > DEFAULT_RSSI_THRESHOLD
│   └── AudioAlertManager::play_alert(AlertLevel::HIGH)
│       └── baseband::request_audio_beep(1200Hz, 48000Hz, 200ms)

DroneDetectionLogger
├── log_detection(entry) → CSV format
│   ├── timestamp,frequency,rssi,threat_level,drone_type,detection_count,confidence
│   ├── ensure_csv_header() on first write
│   └── File::write_line(formatted_entry)
└── session summary on stop_scanning()
```

### Фаза 5: Остановка и Очистка
```cpp
stop_scanning()
├── scanning_active_ = false
├── wait for scanning_thread_ to exit
├── hardware_.stop_spectrum_streaming() → baseband::spectrum_streaming_stop()
├── DetectionRingBuffer::clear()
└── DroneDetectionLogger::end_session() with summary

Hardware Shutdown
├── DroneHardwareController::shutdown_hardware()
│   ├── cleanup_spectrum_collector()
│   ├── receiver_model.disable()
│   └── baseband::shutdown()

Settings Persistence
├── DroneAnalyzerSettingsManager::save(settings)
│   ├── verify_comm_file_exists("/sdcard/")
│   ├── generate_settings_content() → key=value pairs
│   ├── create_backup_file(old_settings)
│   ├── write(new_settings) to main file
│   └── remove_backup on success, restore on failure
└── SpectrumPresetLoader::save_presets_to_file()

Application Exit
└── reset all controllers to safe state
    ├── hardware_ = nullptr
    ├── scanner_ = nullptr
    ├── audio_ = nullptr
    └── display_controller_ cleaned up
```

### Критические Механизмы Синхронизации
```cpp
// Потоковые взаимодействия
UI Thread ↔ Scanning Thread via mutex-protected shared data
Baseband Messages ↔ Hardware Controller via FIFO queues
Settings Manager ↔ All components via atomic updates

// Защита от ошибок
Hardware Failure → switch_to_demo_mode() fallback
Buffer Overflow → global_detection_ring.clear()
Memory Limits → MAX_TRACKED_DRONES = 8 (fits 320KB RAM)
File I/O failure → compile-time defaults, no crash
```

---
*Этот документ верифицирован на основе исходного кода EDA. Все сигнатуры функций и логика работы соответствуют фактическим реализациям в коде. Версия: 1.0. Документ создан: $(date)*
