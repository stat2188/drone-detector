// ui_scanner_combined.cpp - Unified implementation for Enhanced Drone Analyzer Scanner App
// Combines: ui_drone_scanner.cpp, ui_drone_hardware.cpp, ui_drone_ui.cpp
// Created during migration: Split monolithic app into focused Scanner application

#include "ui_scanner_combined.hpp"
#include "ui_drone_audio.hpp"
#include "gradient.hpp"
#include "baseband_api.hpp"
#include "portapack.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "file_path.hpp"
#include "string_format.hpp"
#include "file.hpp"
#include "tone_key.hpp"
#include "rtc_time.hpp"
#include "ui_fileman.hpp"

#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <cstring>

using namespace portapack;
using namespace tonekey;
#include <ch.h>

// Выносим переменную наружу, в область видимости файла (static глобальная переменная файла)
// Это убирает вызов __cxa_guard, так как инициализация происходит на старте, а не при вызове.
const TrackedDrone& get_empty_drone() {
    static TrackedDrone empty;
    return empty;
}

namespace ui::external_app::enhanced_drone_analyzer {

// Функция парсит строку, меняя её содержимое (вставляет \0 вместо =)
void parse_settings_line_inplace(char* line, DroneAnalyzerSettings& settings) {
    // 1. Пропускаем комментарии
    if (line[0] == '#' || line[0] == 0) return;

    // 2. Ищем разделитель '='
    char* equals_ptr = strchr(line, '=');
    if (!equals_ptr) return;

    // 3. Разделяем ключ и значение
    *equals_ptr = 0; // Разрываем строку: "key\0value"
    char* key = line;
    char* value = equals_ptr + 1;

    // 4. Тримминг пробелов (простой вариант)
    // Функция trim должна быть легкой, пропускаем leading spaces
    while (*key == ' ' || *key == '\t') key++;
    while (*value == ' ' || *value == '\t') value++;

    // Удаление trailing spaces для value (обычно в конце строки может быть \r)
    size_t val_len = strlen(value);
    while (val_len > 0 && (value[val_len-1] == ' ' || value[val_len-1] == '\t' || value[val_len-1] == '\r')) {
        value[val_len-1] = 0;
        val_len--;
    }

    // 5. Сравнение ключей (используем strcmp вместо string ==)
    if (strcmp(key, "spectrum_mode") == 0) {
        if (strcmp(value, "NARROW") == 0) settings.spectrum_mode = SpectrumMode::NARROW;
        else if (strcmp(value, "MEDIUM") == 0) settings.spectrum_mode = SpectrumMode::MEDIUM;
        else if (strcmp(value, "WIDE") == 0) settings.spectrum_mode = SpectrumMode::WIDE;
        else if (strcmp(value, "ULTRA_WIDE") == 0) settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
    }
    else if (strcmp(key, "scan_interval_ms") == 0) {
        settings.scan_interval_ms = strtoul(value, nullptr, 10);
    }
    else if (strcmp(key, "rssi_threshold_db") == 0) {
        settings.rssi_threshold_db = strtol(value, nullptr, 10);
    }
    else if (strcmp(key, "enable_audio_alerts") == 0) {
        settings.enable_audio_alerts = (strcmp(value, "true") == 0);
    }
    else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
        settings.hardware_bandwidth_hz = strtoul(value, nullptr, 10);
    }
    else if (strcmp(key, "enable_real_hardware") == 0) {
        settings.enable_real_hardware = (strcmp(value, "true") == 0);
    }
}

bool load_settings_from_sd_card(DroneAnalyzerSettings& settings) {
    File settings_file;
    auto error = settings_file.open("/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt");
    if (error) return false;

    char read_buffer[128];      // Читаем блоками по 128 байт
    char line_buffer[128];      // Буфер для сборки одной строки
    size_t line_idx = 0;

    while (true) {
        // Читаем блок
        auto read_res = settings_file.read(read_buffer, sizeof(read_buffer));
        if (read_res.is_error()) break;

        size_t bytes_read = read_res.value();
        if (bytes_read == 0) break; // EOF

        // Обрабатываем прочитанный блок
        for (size_t i = 0; i < bytes_read; i++) {
            char c = read_buffer[i];

            if (c == '\n') {
                // Конец строки найден
                line_buffer[line_idx] = 0; // Null-terminate

                // Парсим готовую строку
                parse_settings_line_inplace(line_buffer, settings);

                // Сброс для следующей строки
                line_idx = 0;
            }
            else if (c != '\r') {
                // Игнорируем \r, накапливаем остальные символы
                if (line_idx < sizeof(line_buffer) - 1) {
                    line_buffer[line_idx++] = c;
                }
                // Если строка длиннее буфера, лишние символы просто игнорируются
                // до встречи \n, чтобы не переполнить стек
            }
        }
    }

    // Обработка последней строки, если файл не заканчивается на \n
    if (line_idx > 0) {
        line_buffer[line_idx] = 0;
        parse_settings_line_inplace(line_buffer, settings);
    }

    return true;
}



// ===========================================
// PART 2: DRONE SCANNER IMPLEMENTATION
// ===========================================

DroneScanner::DroneScanner()
    : scanning_thread_(nullptr),
      scanning_active_(false),
      freq_db_(),
      current_db_index_(0),
      last_scanned_frequency_(0),
      freq_db_loaded_(false),
      scan_cycles_(0),
      total_detections_(0),
      scanning_mode_(ScanningMode::DATABASE),
      is_real_mode_(true),
      tracked_drones_(),
      tracked_count_(0),
      approaching_count_(0),
      receding_count_(0),
      static_count_(0),
      max_detected_threat_(ThreatLevel::NONE),
      last_valid_rssi_(-120),
      wideband_scan_data_(),
      drone_database_(),
      detection_logger_(),
      data_mutex(),
      detection_ring_buffer_()
{
    // IMPORTANT: Initialize mutex before everything else
    chMtxInit(&data_mutex);

    initialize_database_and_scanner();
    initialize_wideband_scanning();
}

DroneScanner::~DroneScanner() {
    stop_scanning();
    cleanup_database_and_scanner();
}

void DroneScanner::initialize_wideband_scanning() {
    wideband_scan_data_.reset();
    setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
}

void DroneScanner::setup_wideband_range(Frequency min_freq, Frequency max_freq) {
    wideband_scan_data_.min_freq = min_freq;
    wideband_scan_data_.max_freq = max_freq;

    Frequency scanning_range = max_freq - min_freq;
    if (scanning_range > WIDEBAND_SLICE_WIDTH) {
        wideband_scan_data_.slices_nb = (scanning_range + WIDEBAND_SLICE_WIDTH - 1) / WIDEBAND_SLICE_WIDTH;
        if (wideband_scan_data_.slices_nb > WIDEBAND_MAX_SLICES) {
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }
        Frequency slices_span = wideband_scan_data_.slices_nb * WIDEBAND_SLICE_WIDTH;
        Frequency offset = ((scanning_range - slices_span) / 2) + (WIDEBAND_SLICE_WIDTH / 2);
        Frequency center_frequency = min_freq + offset;

        std::generate_n(wideband_scan_data_.slices,
                       wideband_scan_data_.slices_nb,
                       [&center_frequency, slice_index = 0]() mutable -> WidebandSlice {
                           WidebandSlice slice;
                           slice.center_frequency = center_frequency;
                           slice.index = slice_index++;
                           center_frequency += WIDEBAND_SLICE_WIDTH;
                           return slice;
                       });
    } else {
        wideband_scan_data_.slices[0].center_frequency = (max_freq + min_freq) / 2;
        wideband_scan_data_.slices_nb = 1;
    }
    wideband_scan_data_.slice_counter = 0;
}

void DroneScanner::start_scanning() {
    if (scanning_active_) return;

    scanning_active_ = true;
    scan_cycles_ = 0;
    total_detections_ = 0;
}

void DroneScanner::stop_scanning() {
    if (!scanning_active_) return;

    scanning_active_ = false;
    remove_stale_drones();
}

bool DroneScanner::load_frequency_database() {
    current_db_index_ = 0;

    freqman_load_options options;
    options.max_entries = 150;
    options.load_freqs = true;
    options.load_ranges = true;
    options.load_hamradios = true;
    options.load_repeaters = true;

    freqman_db temp_db;
    if (!load_freqman_file("DRONES", temp_db, options)) {
        return false;
    }

    // Блок критической секции
    {
        MutexLock lock(data_mutex); // Захват мьютекса

        drone_database_.clear();
        // ОПТИМИЗАЦИЯ: Перемещаем указатели вместо копирования и удаления.
        // Это быстрее и не фрагментирует память.
        for (auto& entry_ptr : temp_db) {
            if (entry_ptr) {
                drone_database_.push_back(std::move(entry_ptr));
            }
        }
    } // Мьютекс освобождается здесь автоматически

    // Очищаем temp_db, который теперь содержит пустые (moved-from) указатели
    temp_db.clear();

    if (drone_database_.size() > 100) {
        handle_scan_error("Large database loaded");
    }

    freq_db_loaded_ = true;
    return drone_database_.size() > 0;
}

size_t DroneScanner::get_database_size() const {
    return freq_db_.entry_count();
}

void DroneScanner::set_scanning_mode(ScanningMode mode) {
    scanning_mode_ = mode;
    stop_scanning();
    reset_scan_cycles();

    if (scanning_mode_ == ScanningMode::DATABASE || scanning_mode_ == ScanningMode::HYBRID) {
        load_frequency_database();
    }
}

const char* DroneScanner::scanning_mode_name() const {
    switch (scanning_mode_) {
        case ScanningMode::DATABASE: return "Database Scan";
        case ScanningMode::WIDEBAND_CONTINUOUS: return "Wideband Monitor";
        case ScanningMode::HYBRID: return "Hybrid Discovery";
        default: return "Unknown";
    }
}

void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    if (!scanning_active_) return;

    switch (scanning_mode_) {
        case ScanningMode::DATABASE:
            perform_database_scan_cycle(hardware);
            break;
        case ScanningMode::WIDEBAND_CONTINUOUS:
            perform_wideband_scan_cycle(hardware);
            break;
        case ScanningMode::HYBRID:
            perform_hybrid_scan_cycle(hardware);
            break;
    }
    scan_cycles_++;
}

void DroneScanner::perform_database_scan_cycle(DroneHardwareController& hardware) {
    // ИСПРАВЛЕНИЕ DEADLOCK: Не держим общий мьютекс во время работы с железом!

    size_t total_entries = 0;

    // 1. Быстро узнаем размер базы (под мьютексом)
    {
        MutexLock lock(data_mutex);
        if (drone_database_.empty()) {
            if (scan_cycles_ % 50 == 0) {
                // handle_scan_error вызывается здесь, но флаг меняем аккуратно
                scanning_active_ = false;
            }
            return;
        }
        total_entries = drone_database_.size();
    }

    const size_t batch_size = std::min(static_cast<size_t>(20), total_entries);

    for (size_t i = 0; i < batch_size; ++i) {
        // Проверяем флаг атомарно перед каждой итерацией
        if (!scanning_active_) break;

        freqman_entry entry_copy;
        bool entry_valid = false;

        // 2. Копируем одну запись для работы (под мьютексом)
        {
            MutexLock lock(data_mutex);
            // Проверяем, не изменился ли размер пока мы спали
            if (!drone_database_.empty()) {
                if (current_db_index_ >= drone_database_.size()) {
                    current_db_index_ = 0;
                }

                if (drone_database_[current_db_index_]) {
                    entry_copy = *drone_database_[current_db_index_];
                    entry_valid = true;
                }
                current_db_index_ = (current_db_index_ + 1) % drone_database_.size();
            }
        } // Мьютекс освобождается здесь

        // 3. Работаем с железом БЕЗ блокировки (UI не будет виснуть)
        if (entry_valid) {
            Frequency target_freq_hz = entry_copy.frequency_a;

            if (target_freq_hz >= 50000000 && target_freq_hz <= 6000000000) {
                if (hardware.tune_to_frequency(target_freq_hz)) {
                    chThdSleepMilliseconds(30);
                    int32_t real_rssi = hardware.get_real_rssi_from_hardware(target_freq_hz);

                    // Этот метод сам возьмет мьютекс внутри, когда нужно будет сохранить результат
                    process_rssi_detection(entry_copy, real_rssi);

                    last_scanned_frequency_ = target_freq_hz;
                }
            }
        }
    }
}

void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
    // Initialize range if needed (leave as is)
    if (wideband_scan_data_.slices_nb == 0) {
        setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
    }

    const WidebandSlice& current_slice = wideband_scan_data_.slices[wideband_scan_data_.slice_counter];

    // 1. Tune to SLICE CENTER
    if (hardware.tune_to_frequency(current_slice.center_frequency)) {

        // 2. Wait for stabilization (same as in Database Scan)
        chThdSleepMilliseconds(25); // Slightly less, since retuning to adjacent channel is faster

        // 3. Get total RSSI of entire band (Wideband RSSI)
        int32_t slice_rssi = hardware.get_real_rssi_from_hardware(current_slice.center_frequency);

        // 4. If signal is strong, we assume drone presence
        if (slice_rssi > WIDEBAND_RSSI_THRESHOLD_DB) {

            // Instead of trying to find exact peak by enumeration (which is slow),
            // we register detection at slice center frequency.
            // (For more accurate analysis FFT is needed, which works in DisplayController,
            // but here, in scanner logic, the fact of energy presence is important).

            freqman_entry fake_entry{
                .frequency_a = static_cast<int64_t>(current_slice.center_frequency),
                .frequency_b = static_cast<int64_t>(current_slice.center_frequency),
                .description = "Wideband Activity",
                .type = freqman_type::Single,
                // ... initialization of other fields by default ...
            };

            // Call detection handler (now protected by mutex from Stage 1)
            wideband_detection_override(fake_entry, slice_rssi, WIDEBAND_RSSI_THRESHOLD_DB);
        }

        last_scanned_frequency_ = current_slice.center_frequency;
    } else {
        // Handle tuning error
        if (scan_cycles_ % 100 == 0) {
            handle_scan_error("Hardware tuning failed in wideband mode");
        }
    }

    // Transition to next slice
    wideband_scan_data_.slice_counter = (wideband_scan_data_.slice_counter + 1) % wideband_scan_data_.slices_nb;
}

void DroneScanner::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
    if (rssi >= threshold_override) {
        freqman_entry wideband_entry = entry;
        wideband_entry.description = "Wideband Enhanced Detection";
        process_wideband_detection_with_override(wideband_entry, rssi, DEFAULT_RSSI_THRESHOLD_DB, threshold_override);
    }
}

void DroneScanner::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                           int32_t /*original_threshold*/, int32_t wideband_threshold) {
    // 1. Валидация (без блокировок)
    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::UNKNOWN) ||
        !SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    bool should_log = false;
    DetectionLogEntry log_entry_to_write;
    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level; // ... логика определения threat level ...

    // Логика threat_level (скопировать из оригинала)
    if (rssi > -70) threat_level = ThreatLevel::HIGH;
    else if (rssi > -80) threat_level = ThreatLevel::LOW;
    else threat_level = ThreatLevel::UNKNOWN;
    if (entry.frequency_a >= 2'400'000'000 && entry.frequency_a <= 2'500'000'000) {
        threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
    }

    // --- НАЧАЛО КРИТИЧЕСКОЙ СЕКЦИИ ---
    {
        MutexLock lock(data_mutex);

        total_detections_++; // Защищаем счетчик

        size_t freq_hash = entry.frequency_a / 100000;
        int32_t effective_threshold = wideband_threshold;

        if (detection_ring_buffer_.get_rssi_value(freq_hash) < wideband_threshold) {
            effective_threshold = wideband_threshold + HYSTERESIS_MARGIN_DB;
        }

        if (rssi >= effective_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
            detection_ring_buffer_.update_detection(freq_hash, current_count, rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                should_log = true;

                log_entry_to_write = {
                    chTimeNow(),
                    static_cast<uint32_t>(entry.frequency_a),
                    rssi,
                    threat_level,
                    detected_type,
                    current_count,
                    0.6f
                };

                update_tracked_drone_internal(detected_type, entry.frequency_a, rssi, threat_level);
            }
        } else {
            // Логика уменьшения счетчика...
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // --- КОНЕЦ КРИТИЧЕСКОЙ СЕКЦИИ ---

    // 3. Логирование
    if (should_log && detection_logger_.is_session_active()) {
        detection_logger_.log_detection(log_entry_to_write);
    }
}

void DroneScanner::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
    if (scan_cycles_ % 2 == 0) {
        perform_wideband_scan_cycle(hardware);
    } else {
        perform_database_scan_cycle(hardware);
    }
}

void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
    // 1. Предварительная фильтрация (не требует блокировок)
    const int32_t INVALID_RSSI = -128;
    const int32_t MIN_VALID_RSSI = -110;
    const int32_t MAX_VALID_RSSI = 10;

    if (rssi <= INVALID_RSSI || rssi < MIN_VALID_RSSI || rssi > MAX_VALID_RSSI) {
        return;
    }

    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::UNKNOWN)) {
        return;
    }

    if (!SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    // Определяем параметры (локально)
    int32_t detection_threshold = -90;
    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level = SimpleDroneValidation::classify_signal_strength(rssi);

    // Простой поиск по БД (если drone_database_ не меняется в рантайме, это безопасно читать без мьютекса,
    // НО если БД может перезагружаться, здесь нужен reader-lock. Для простоты допустим, что БД статична во время скана)
    for (const auto& db_entry : drone_database_) {
        if (db_entry && db_entry->frequency_a == entry.frequency_a) {
            detected_type = DroneType::MAVIC;
            threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
            break;
        }
    }

    if (rssi < detection_threshold) {
        return;
    }

    // Флаг и данные для отложенного логирования
    bool should_log = false;
    DetectionLogEntry log_entry_to_write;

    // --- НАЧАЛО КРИТИЧЕСКОЙ СЕКЦИИ ---
    {
        MutexLock lock(data_mutex); // Блокируем доступ UI к данным

        total_detections_++; // Защищаем счетчик

        size_t freq_hash = entry.frequency_a / 100000;
        int32_t effective_threshold = detection_threshold;
        if (detection_ring_buffer_.get_rssi_value(freq_hash) < detection_threshold) {
            effective_threshold = detection_threshold + HYSTERESIS_MARGIN_DB;
        }

        if (rssi >= effective_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));

            // Пишем в буфер (безопасно)
            detection_ring_buffer_.update_detection(freq_hash, current_count, rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                // Мы НЕ пишем лог здесь. Мы только готовим данные.
                should_log = true;

                // Подготовка данных для лога
                log_entry_to_write.timestamp = chTimeNow();
                log_entry_to_write.frequency_hz = static_cast<uint32_t>(entry.frequency_a);
                log_entry_to_write.rssi_db = rssi;
                log_entry_to_write.threat_level = threat_level;
                log_entry_to_write.drone_type = detected_type;
                log_entry_to_write.detection_count = current_count;
                log_entry_to_write.confidence_score = 0.85f;

                // Обновляем трекинг дронов для UI (внутренний метод, который НЕ берет мьютекс повторно)
                update_tracked_drone_internal(detected_type, entry.frequency_a, rssi, threat_level);
            }
        } else {
            // Логика "leaky bucket"
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);

            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // --- КОНЕЦ КРИТИЧЕСКОЙ СЕКЦИИ ---
    // Мьютекс освобожден. UI может перерисовываться.

    // 3. Выполняем тяжелые операции ввода-вывода
    if (should_log) {
        // Имитация задержки "анти-дребезга" для логгера, если нужно, но лучше без sleep в цикле скана
        // chThdSleepMilliseconds(DETECTION_DELAY);

        if (detection_logger_.is_session_active()) {
            // Это займет время, но UI поток не будет заблокирован, так как data_mutex свободен
            detection_logger_.log_detection(log_entry_to_write);
        }
    }
}

void DroneScanner::send_drone_detection_message(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
    update_tracked_drone(type, frequency, rssi, threat_level);
}

void DroneScanner::update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
    // Lock access. While we're here, UI will wait.
    MutexLock lock(data_mutex);
    update_tracked_drone_internal(type, frequency, rssi, threat_level);
}

void DroneScanner::update_tracked_drone_internal(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
    // Internal method - assumes that the mutex is ALREADY acquired by the caller
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones_[i].frequency == static_cast<uint32_t>(frequency) && tracked_drones_[i].update_count > 0) {
            tracked_drones_[i].add_rssi(static_cast<int16_t>(rssi), chTimeNow());
            tracked_drones_[i].drone_type = static_cast<uint8_t>(type);
            tracked_drones_[i].threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
    }

    if (tracked_count_ < MAX_TRACKED_DRONES) {
        TrackedDrone new_drone;
        new_drone.frequency = static_cast<uint32_t>(frequency);
        new_drone.drone_type = static_cast<uint8_t>(type);
        new_drone.threat_level = static_cast<uint8_t>(threat_level);
        new_drone.add_rssi(static_cast<int16_t>(rssi), chTimeNow());
        tracked_drones_[tracked_count_] = new_drone;
        tracked_count_++;
        update_tracking_counts();
        return;
    }

    size_t oldest_index = 0;
    systime_t oldest_time = tracked_drones_[0].last_seen;
    for (size_t i = 1; i < tracked_count_; i++) {
        if (tracked_drones_[i].last_seen < oldest_time) {
            oldest_time = tracked_drones_[i].last_seen;
            oldest_index = i;
        }
    }

    tracked_drones_[oldest_index] = TrackedDrone();
    tracked_drones_[oldest_index].frequency = static_cast<uint32_t>(frequency);
    tracked_drones_[oldest_index].drone_type = static_cast<uint8_t>(type);
    tracked_drones_[oldest_index].threat_level = static_cast<uint8_t>(threat_level);
    tracked_drones_[oldest_index].add_rssi(static_cast<int16_t>(rssi), chTimeNow());
    update_tracking_counts();
}

void DroneScanner::remove_stale_drones() {
    // Protect deletion
    MutexLock lock(data_mutex);

    const systime_t STALE_TIMEOUT = 30000;
    systime_t current_time = chTimeNow();

    // Compact array by shifting valid drones to the front
    size_t write_index = 0;
    for (size_t read_index = 0; read_index < tracked_count_; ++read_index) {
        if (!(tracked_drones_[read_index].update_count > 0 && (current_time - tracked_drones_[read_index].last_seen) > STALE_TIMEOUT)) {
            // Keep this drone
            if (write_index != read_index) {
                tracked_drones_[write_index] = tracked_drones_[read_index];
            }
            write_index++;
        }
    }
    tracked_count_ = write_index;
    update_tracking_counts();
}

void DroneScanner::update_tracking_counts() {
    approaching_count_ = 0;
    receding_count_ = 0;
    static_count_ = 0;

    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones_[i].update_count < 2) continue;

        MovementTrend trend = tracked_drones_[i].get_trend();
        switch (trend) {
            case MovementTrend::APPROACHING: approaching_count_++; break;
            case MovementTrend::RECEDING: receding_count_++; break;
            case MovementTrend::STATIC:
            case MovementTrend::UNKNOWN:
            default: static_count_++; break;
        }
    }
    update_trends_compact_display();
}

void DroneScanner::update_trends_compact_display() {
}

void DroneScanner::reset_scan_cycles() {
    scan_cycles_ = 0;
}

void DroneScanner::switch_to_real_mode() {
    is_real_mode_ = true;
}

void DroneScanner::switch_to_demo_mode() {
    is_real_mode_ = false;
}

void DroneScanner::initialize_database_and_scanner() {
    // Initialization logic
}

void DroneScanner::cleanup_database_and_scanner() {
    // Cleanup logic
}

void DroneScanner::scan_init_from_loaded_frequencies() {
}

bool DroneScanner::validate_detection_simple(int32_t rssi_db, ThreatLevel threat) {
    return SimpleDroneValidation::validate_rssi_signal(rssi_db, threat);
}

Frequency DroneScanner::get_current_scanning_frequency() const {
    if (!drone_database_.empty() && current_db_index_ < drone_database_.size() && drone_database_[current_db_index_]) {
        return drone_database_[current_db_index_]->frequency_a;
    }
    return 433000000;
}

Frequency DroneScanner::get_current_radio_frequency() const {
    return get_current_scanning_frequency();
}

const TrackedDrone& DroneScanner::getTrackedDrone(size_t index) const {
    if (index < tracked_count_) {
        return tracked_drones_[index];
    }
    return get_empty_drone();
}

std::string DroneScanner::get_session_summary() const {
    return detection_logger_.format_session_summary(get_scan_cycles(), get_total_detections());
}

void DroneScanner::handle_scan_error(const char* error_msg) {
    (void)error_msg;
}

DroneScanner::DroneSnapshot DroneScanner::get_tracked_drones_snapshot() const {
    DroneSnapshot snapshot;
    MutexLock lock(data_mutex);
    snapshot.count = tracked_count_;
    for (size_t i = 0; i < tracked_count_ && i < MAX_TRACKED_DRONES; ++i) {
        snapshot.drones[i] = tracked_drones_[i];
    }
    return snapshot;
}

    // Implementation of safe read methods

int32_t DroneScanner::get_detection_rssi_safe(size_t freq_hash) const {
    MutexLock lock(data_mutex); // Lock reading while scanner can write
    return detection_ring_buffer_.get_rssi_value(freq_hash);
}

uint8_t DroneScanner::get_detection_count_safe(size_t freq_hash) const {
    MutexLock lock(data_mutex);
    return detection_ring_buffer_.get_detection_count(freq_hash);
}

// DroneDetectionLogger implementations
DroneDetectionLogger::DroneDetectionLogger()
    : csv_log_(), session_active_(false), session_start_(0), logged_count_(0), header_written_(false) {
    memset(line_buffer_, 0, sizeof(line_buffer_));
    start_session();
}

DroneDetectionLogger::~DroneDetectionLogger() {
    end_session();
}

void DroneDetectionLogger::start_session() {
    if (session_active_) return;
    session_active_ = true;
    session_start_ = chTimeNow();
    logged_count_ = 0;
    header_written_ = false;
}

void DroneDetectionLogger::end_session() {
    if (!session_active_) return;
    session_active_ = false;
}

bool DroneDetectionLogger::log_detection(const DetectionLogEntry& entry) {
    // Двойная проверка, так как session_active_ не атомарен,
    // но в рамках одного потока сканера это допустимо.
    if (!session_active_) return false;

    // Пытаемся записать заголовок
    if (!ensure_csv_header()) return false;

    // Используем уже существующий в классе line_buffer_ (он там есть, 192 байта)
    // snprintf пишет сразу в char array, без создания std::string
    int len = snprintf(line_buffer_, sizeof(line_buffer_),
             "%lu,%lu,%ld,%u,%u,%u,%ld\n", // %f (float) тяжел для M4, лучше писать int * 100
             static_cast<unsigned long>(entry.timestamp),
             static_cast<unsigned long>(entry.frequency_hz),
             (long int)entry.rssi_db,
             static_cast<uint8_t>(entry.threat_level),
             static_cast<uint8_t>(entry.drone_type),
             entry.detection_count,
             (long int)(entry.confidence_score * 100)); // Трюк с float

    // Проверка на ошибку форматирования
    if (len <= 0) return false;

    auto error = csv_log_.append(generate_log_filename());
    if (error && !error->ok()) return false;

    // Пишем raw buffer. API Mayhem принимает void* и size
    // Внимание: метод write_raw в Mayhem обычно принимает строку или буфер.
    // Если он принимает std::string, придется сделать write_raw(std::string(line_buffer_, len)),
    // но лучше использовать write(data, len) если доступно.

    // Предполагая стандартный API LogFile в Mayhem:
    error = csv_log_.write_raw(std::string(line_buffer_, len));

    if (error && error->ok()) {
        logged_count_++;
        return true;
    }
    return false;
}

bool DroneDetectionLogger::ensure_csv_header() {
    if (header_written_) return true;
    const char* header = "timestamp_ms,frequency_hz,rssi_db,threat_level,drone_type,detection_count,confidence\n";
    auto error = csv_log_.append(generate_log_filename());
    if (error && !error->ok()) return false;
    error = csv_log_.write_raw(header);
    if (error && error->ok()) {
        header_written_ = true;
        return true;
    }
    return false;
}

std::string DroneDetectionLogger::format_csv_entry(const DetectionLogEntry& entry) {
    // Use member buffer to avoid stack allocation in deep call chains
    int ret = snprintf(line_buffer_, sizeof(line_buffer_),
             "%lu,%lu,%ld,%u,%u,%u,%ld\n",
             static_cast<unsigned long>(entry.timestamp), static_cast<unsigned long>(entry.frequency_hz),
             (long int)entry.rssi_db,
             static_cast<uint8_t>(entry.threat_level),
             static_cast<uint8_t>(entry.drone_type),
             entry.detection_count, (long int)(entry.confidence_score * 100));

    // Ensure null termination if snprintf didn't have enough space
    if (ret >= static_cast<int>(sizeof(line_buffer_))) {
        line_buffer_[sizeof(line_buffer_) - 1] = '\0';
    }

    return std::string(line_buffer_);
}

std::string DroneDetectionLogger::generate_log_filename() const {
    return std::string("EDA_LOG.CSV");
}

std::string DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) const {
    std::string result;

    // FIX: Резервируем память один раз с запасом.
    // Избавляет от ~10 malloc/free вызовов при конкатенации.
    result.reserve(512);

    result += "SCANNING SESSION COMPLETE\n========================\n\nSESSION STATISTICS:\n";

    // Вычисляем длительность сессии
    uint32_t session_duration_ms = chTimeNow() - session_start_;
    if (session_duration_ms == 0) session_duration_ms = 1;

    char duration_buf[64]; // Увеличен буфер для безопасности
    snprintf(duration_buf, sizeof(duration_buf), "Duration: %lu.%lu seconds\n",
             session_duration_ms / 1000,
             (session_duration_ms % 1000) / 100);
    result += duration_buf;

    result += "Scan Cycles: ";
    result += to_string_dec_uint(scan_cycles);
    result += "\nTotal Detections: ";
    result += to_string_dec_uint(total_detections);
    result += "\n\nPERFORMANCE:\n";

    // Вычисляем метрики
    uint32_t avg_det_x100 = (scan_cycles > 0) ? (total_detections * 100) / scan_cycles : 0;
    uint32_t rate_x10 = (uint64_t)total_detections * 10000 / session_duration_ms;

    char perf_buf[128];
    snprintf(perf_buf, sizeof(perf_buf),
             "Avg. detections/cycle: %lu.%02lu\nDetection rate: %lu.%lu/sec\nLogged entries: ",
             avg_det_x100 / 100, avg_det_x100 % 100,
             rate_x10 / 10, rate_x10 % 10);
    result += perf_buf;

    result += to_string_dec_uint(logged_count_);
    result += "\n\nEnhanced Drone Analyzer v0.3";

    return result;
}

// ===========================================
// PART 3: HARDWARE CONTROLLER IMPLEMENTATION
// ===========================================

DroneHardwareController::DroneHardwareController(SpectrumMode mode)
    : message_handler_spectrum_config_(nullptr), message_handler_frame_sync_(nullptr),
      message_handler_spectrum_(nullptr), message_handler_channel_statistics_(nullptr),
      spectrum_mode_(mode), center_frequency_(2400000000ULL), bandwidth_hz_(24000000),
      radio_state_(), fifo_(nullptr), spectrum_streaming_active_(false), last_valid_rssi_(-120)
{
    // Initialize message handlers - these will be properly set up in initialize_spectrum_collector
}

DroneHardwareController::~DroneHardwareController() {
    shutdown_hardware();
}

void DroneHardwareController::initialize_hardware() {
    initialize_radio_state();
    initialize_spectrum_collector();
}

void DroneHardwareController::on_hardware_show() {
    initialize_hardware();
}

void DroneHardwareController::on_hardware_hide() {
    cleanup_spectrum_collector();
}

void DroneHardwareController::shutdown_hardware() {
    // Stop Baseband data streaming
    stop_spectrum_streaming();

    // Explicitly disable radio chip (CPLD/R820T/MAX2837)
    // This is critical for power saving and stability of the next application launch
    receiver_model.disable();

    // Clean up message subscriptions
    cleanup_spectrum_collector();
}

void DroneHardwareController::initialize_radio_state() {
    receiver_model.enable();
    receiver_model.set_modulation(ReceiverModel::Mode::SpectrumAnalysis);
    receiver_model.set_sampling_rate(get_configured_sampling_rate());
    receiver_model.set_baseband_bandwidth(get_configured_bandwidth());
    receiver_model.set_squelch_level(0);

    // --- ДОБАВЛЕНО: Установка чувствительности ---

    // 1. RF AMP (Усилитель): Включаем (+14dB).
    // Если у вас внешняя активная антенна, поставьте false. Для штыревой антенны - true.
    receiver_model.set_rf_amp(true);

    // 2. LNA Gain (МШУ): 32dB (Диапазон 0-40).
    // Отвечает за прием слабых сигналов.
    receiver_model.set_lna(32);

    // 3. VGA Gain (ПП): 32dB (Диапазон 0-62).
    // Отвечает за громкость сигнала после LNA.
    receiver_model.set_vga(32);
}

void DroneHardwareController::initialize_spectrum_collector() {
    // FIX: Передаем лямбду напрямую. Убираем std::move и явный std::function<...>.
    // Это предотвращает создание сложных VTable переходов, вызывающих warning 0xade...

    message_handler_spectrum_config_ = new MessageHandlerRegistration(
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            this->handle_channel_spectrum_config(static_cast<const ChannelSpectrumConfigMessage*>(p));
        }
    );

    message_handler_frame_sync_ = new MessageHandlerRegistration(
        Message::ID::DisplayFrameSync,
        [this](Message* const p) {
             (void)p;
             // Frame sync logic here if needed
        }
    );

    message_handler_channel_statistics_ = new MessageHandlerRegistration(
        Message::ID::ChannelStatistics,
        [this](Message* const p) {
            const auto* statistics_msg = static_cast<const ChannelStatisticsMessage*>(p);
            this->handle_channel_statistics(statistics_msg->statistics);
        }
    );
}

void DroneHardwareController::cleanup_spectrum_collector() {
    spectrum_streaming_active_ = false;

    delete message_handler_spectrum_config_;
    message_handler_spectrum_config_ = nullptr;

    delete message_handler_frame_sync_;
    message_handler_frame_sync_ = nullptr;

    delete message_handler_channel_statistics_;
    message_handler_channel_statistics_ = nullptr;
}

void DroneHardwareController::set_spectrum_mode(SpectrumMode mode) {
    spectrum_mode_ = mode;
}

uint32_t DroneHardwareController::get_spectrum_bandwidth() const {
    return bandwidth_hz_;
}

void DroneHardwareController::set_spectrum_bandwidth(uint32_t bandwidth_hz) {
    bandwidth_hz_ = bandwidth_hz;
}

Frequency DroneHardwareController::get_spectrum_center_frequency() const {
    return center_frequency_;
}

void DroneHardwareController::set_spectrum_center_frequency(Frequency center_freq) {
    center_frequency_ = center_freq;
}

bool DroneHardwareController::tune_to_frequency(Frequency frequency_hz) {
    receiver_model.set_target_frequency(frequency_hz);
    return true;
}

void DroneHardwareController::start_spectrum_streaming() {
    if (spectrum_streaming_active_) return;
    spectrum_streaming_active_ = true;
    baseband::spectrum_streaming_start();
}

void DroneHardwareController::stop_spectrum_streaming() {
    spectrum_streaming_active_ = false;
    baseband::spectrum_streaming_stop();
}

int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency) {
    (void)target_frequency;
    return last_valid_rssi_;
}

void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message) {
    if (message) {
        fifo_ = message->fifo;
        if (fifo_) {
            ChannelSpectrum spectrum;
            while (fifo_->out(spectrum)) {
                process_channel_spectrum_data(spectrum);
            }
        }
    }
}

void DroneHardwareController::handle_channel_statistics(const ChannelStatistics& statistics) {
    last_valid_rssi_ = statistics.max_db;
}

void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum) {
    (void)spectrum;
}

int32_t DroneHardwareController::get_configured_sampling_rate() const {
    return bandwidth_hz_;
}

int32_t DroneHardwareController::get_configured_bandwidth() const {
    return bandwidth_hz_;
}

bool DroneHardwareController::is_spectrum_streaming_active() const {
    return spectrum_streaming_active_;
}

int32_t DroneHardwareController::get_current_rssi() const {
    return last_valid_rssi_;
}

void DroneHardwareController::update_spectrum_for_scanner() {
}

// ===========================================
// PART 4: UI IMPLEMENTATIONS
// ===========================================

SmartThreatHeader::SmartThreatHeader(Rect parent_rect)
    : View(parent_rect),
      threat_progress_bar_({0, 0, screen_width, 16}),
      threat_status_main_({0, 20, screen_width, 16}, "THREAT: LOW | <0 ~0 >0"),
      threat_frequency_({0, 38, screen_width, 16}, "2400.0MHz SCANNING"),
      current_text_() {
    add_children({&threat_progress_bar_, &threat_status_main_, &threat_frequency_});
    update(ThreatLevel::NONE, 0, 0, 0, 2400000000ULL, false);
}

void SmartThreatHeader::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
                               size_t receding, Frequency current_freq, bool is_scanning) {
    current_threat_ = max_threat;
    is_scanning_ = is_scanning;
    current_freq_ = current_freq;
    approaching_count_ = approaching;
    static_count_ = static_count;
    receding_count_ = receding;

    size_t total_drones = approaching + static_count + receding;
    threat_progress_bar_.set_value(total_drones * 10);

    char buffer[64];
    std::string threat_name = get_threat_icon_text(max_threat);
    if (total_drones > 0) {
        snprintf(buffer, sizeof(buffer), "THREAT: %s | <%zu ~%zu >%zu",
                threat_name.c_str(), approaching, static_count, receding);
    } else if (is_scanning) {
        snprintf(buffer, sizeof(buffer), "SCANNING: <%zu ~%zu >%zu",
                approaching, static_count, receding);
    } else {
        snprintf(buffer, sizeof(buffer), "READY");
    }
    threat_status_main_.set(buffer);
    threat_status_main_.set_style(Theme::getInstance()->fg_red);
    current_text_ = buffer;

    if (current_freq > 0) {
        if (current_freq >= 1000000000ULL) {
            uint32_t ghz = current_freq / 1000000000ULL;
            uint32_t decimals = ((current_freq % 1000000000ULL) / 10000000ULL);
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%lu.%02luGHz SCANNING", (unsigned long)ghz, (unsigned long)decimals);
            } else {
                snprintf(buffer, sizeof(buffer), "%lu.%02luGHz READY", (unsigned long)ghz, (unsigned long)decimals);
            }
        } else {
            uint32_t mhz = current_freq / 1000000ULL;
            uint32_t decimals = (current_freq % 1000000ULL) / 100000ULL;
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%lu.%01luMHz SCANNING", (unsigned long)mhz, (unsigned long)decimals);
            } else {
                snprintf(buffer, sizeof(buffer), "%lu.%01luMHz READY", (unsigned long)mhz, (unsigned long)decimals);
            }
        }
        threat_frequency_.set(buffer);
    } else {
        threat_frequency_.set("NO SIGNAL");
    }

    switch (max_threat) {
        case ThreatLevel::CRITICAL:
            threat_frequency_.set_style(Theme::getInstance()->fg_red);
            break;
        case ThreatLevel::HIGH:
            threat_frequency_.set_style(Theme::getInstance()->fg_yellow);
            break;
        case ThreatLevel::MEDIUM:
            threat_frequency_.set_style(Theme::getInstance()->fg_yellow);
            break;
        case ThreatLevel::LOW:
            threat_frequency_.set_style(Theme::getInstance()->fg_green);
            break;
        case ThreatLevel::NONE:
        default:
            threat_frequency_.set_style(Theme::getInstance()->fg_light);
            break;
    }
    set_dirty();
}

void SmartThreatHeader::set_max_threat(ThreatLevel threat) {
    if (threat != current_threat_) {
        update(threat, approaching_count_, static_count_, receding_count_,
               current_freq_, is_scanning_);
    }
}

void SmartThreatHeader::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
    update(current_threat_, approaching, static_count, receding,
           current_freq_, is_scanning_);
}

void SmartThreatHeader::set_current_frequency(Frequency freq) {
    if (freq != current_freq_) {
        update(current_threat_, approaching_count_, static_count_, receding_count_,
               freq, is_scanning_);
    }
}

void SmartThreatHeader::set_scanning_state(bool is_scanning) {
    if (is_scanning != is_scanning_) {
        update(current_threat_, approaching_count_, static_count_, receding_count_,
               current_freq_, is_scanning);
    }
}

void SmartThreatHeader::set_color_scheme(bool use_dark_theme) {
    (void)use_dark_theme;
}

Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color(255, 140, 0);
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        case ThreatLevel::NONE:
        default: return Color::blue();
    }
}

Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color(255, 165, 0);
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        case ThreatLevel::NONE:
        default: return Color::white();
    }
}

const char* SmartThreatHeader::get_threat_icon_text(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return "CRITICAL";
        case ThreatLevel::HIGH: return "HIGH";
        case ThreatLevel::MEDIUM: return "MEDIUM";
        case ThreatLevel::LOW: return "LOW";
        case ThreatLevel::NONE:
        default: return "CLEAR";
    }
}

void SmartThreatHeader::paint(Painter& painter) {
    // 1. Fill the entire background with threat color
    Color bg_color = get_threat_bar_color(current_threat_);
    painter.fill_rectangle(screen_rect(), bg_color);

    // 2. Draw large centered text with white color on colored background
    // Calculate centered position
    const int text_width = current_text_.length() * 8; // fixed_8x16 is 8px per char
    const int text_height = 16;
    const int center_x = (screen_width - text_width) / 2;
    const int center_y = (60 - text_height) / 2; // Header height is 60px
    Point text_pos = {center_x, center_y};
    painter.draw_string(text_pos, font::fixed_8x16, Color::white(), bg_color, current_text_);
}

ThreatCard::ThreatCard(size_t card_index, Rect parent_rect)
    : View(parent_rect), card_index_(card_index), current_text_(),
      parent_rect_(parent_rect) {
    add_children({&card_text_});
}

void ThreatCard::update_card(const DisplayDroneEntry& drone) {
    // Check if threat level changed (affects background color in paint)
    bool visual_state_changed = (threat_ != drone.threat) || (!is_active_);

    is_active_ = true;
    frequency_ = drone.frequency;
    threat_ = drone.threat;
    rssi_ = drone.rssi;
    last_seen_ = drone.last_seen;
    threat_name_ = drone.type_name;
    trend_ = drone.trend;

    char buffer[32];
    char trend_char = (trend_ == MovementTrend::APPROACHING) ? '^' :
                      (trend_ == MovementTrend::RECEDING)    ? 'v' :
                      '=';

    uint32_t mhz = frequency_ / 1000000;

    // OPTIMIZATION:
    // %-7.7s : Left-align, minimum 7 characters, MAXIMUM 7 characters.
    // This replaces .substr(0,7) and doesn't create temporary strings.
    snprintf(buffer, sizeof(buffer), "%-7.7s %c %4luM %3ld",
            threat_name_.c_str(), // Take raw pointer to original
            trend_char,
            mhz,
            (long int)rssi_);

    // String comparison (std::string with char* works efficiently)
    if (current_text_ != buffer) {
        current_text_ = buffer; // Allocate only if text actually changed
        card_text_.set(current_text_);
        visual_state_changed = true;
    }

    // Redraw widget ONLY if something changed
    // (either text, or threat color for background)
    if (visual_state_changed) {
        card_text_.set_style(Theme::getInstance()->fg_light);
        set_dirty();
    }
}

void ThreatCard::clear_card() {
    is_active_ = false;
    current_text_ = "";
    card_text_.set("");
    set_dirty();
}



Color ThreatCard::get_card_bg_color() const {
    if (!is_active_) return Color::black();
    switch (threat_) {
        case ThreatLevel::CRITICAL: return Color(64, 0, 0);
        case ThreatLevel::HIGH: return Color(64, 32, 0);
        case ThreatLevel::MEDIUM: return Color(32, 32, 0);
        case ThreatLevel::LOW: return Color(0, 32, 0);
        case ThreatLevel::NONE:
        default: return Color(0, 0, 64);
    }
}

Color ThreatCard::get_card_text_color() const {
    // Always white text, since threat backgrounds (Red, Orange) are dark
    // Exception: if background Yellow (Medium), text better Black
    if (threat_ == ThreatLevel::MEDIUM) return Color::black();
    return Color::white();
}

void ThreatCard::paint(Painter& painter) {
    View::paint(painter);
    if (is_active_) {
        Color bg_color = get_card_bg_color();
        painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, bg_color);
    }
}

ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
    : View(parent_rect), bar_index_(bar_index), parent_rect_(parent_rect) {
    add_children({&progress_text_, &alert_text_, &normal_text_});
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    set_display_mode(DisplayMode::SCANNING);

    char progress_bar[25] = "--------";
    uint8_t filled = (progress_percent * 8) / 100;
    for (uint8_t i = 0; i < filled; i++) {
        progress_bar[i] = '=';
    }

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
            progress_bar, (unsigned long)progress_percent, (unsigned long)total_cycles, (unsigned long)detections);
    progress_text_.set(buffer);
    progress_text_.set_style(Theme::getInstance()->fg_blue);

    if (detections > 0) {
        set_display_mode(DisplayMode::ALERT);
        char alert_buffer[64];
        snprintf(alert_buffer, sizeof(alert_buffer), "[!] DETECTED: %lu threats found!", static_cast<unsigned long>(detections));
        alert_text_.set(alert_buffer);
        alert_text_.set_style(Theme::getInstance()->fg_red);
    }
    set_dirty();
}

void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
    set_display_mode(DisplayMode::ALERT);

    const char* icons[5] = {"(i)", "[!]", "[O]", "[X]", "[!!!]"};
    size_t icon_idx = std::min(static_cast<size_t>(threat), size_t(4));

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%s ALERT: %zu drones | %s",
            icons[icon_idx], total_drones, alert_msg.c_str());

    alert_text_.set(buffer);
    if (threat >= ThreatLevel::CRITICAL) {
        alert_text_.set_style(Theme::getInstance()->fg_red);
    } else {
        alert_text_.set_style(Theme::getInstance()->fg_yellow);
    }
    set_dirty();
}

void ConsoleStatusBar::update_normal_status(const std::string& primary, const std::string& secondary) {
    set_display_mode(DisplayMode::NORMAL);

    char buffer[64];
    if (secondary.empty()) {
        snprintf(buffer, sizeof(buffer), "%s", primary.c_str());
    } else {
        snprintf(buffer, sizeof(buffer), "%s | %s", primary.c_str(), secondary.c_str());
    }
    normal_text_.set(buffer);
    normal_text_.set_style(Theme::getInstance()->fg_light);
    set_dirty();
}

void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
    if (mode_ == mode) return;
    mode_ = mode;

    progress_text_.hidden(true);
    alert_text_.hidden(true);
    normal_text_.hidden(true);

    switch (mode) {
        case DisplayMode::SCANNING: progress_text_.hidden(false); break;
        case DisplayMode::ALERT: alert_text_.hidden(false); break;
        case DisplayMode::NORMAL: default: normal_text_.hidden(false); break;
    }
    set_dirty();
}

void ConsoleStatusBar::paint(Painter& painter) {
    View::paint(painter);
    if (mode_ == DisplayMode::ALERT) {
        painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, Color(32, 0, 0));
    }
}

DroneDisplayController::~DroneDisplayController() {
    // Явное удаление хендлеров
    if (message_handler_spectrum_config_) {
        delete message_handler_spectrum_config_;
        message_handler_spectrum_config_ = nullptr;
    }
    if (message_handler_frame_sync_) {
        delete message_handler_frame_sync_;
        message_handler_frame_sync_ = nullptr;
    }
}

DroneDisplayController::DroneDisplayController(NavigationView& nav)
    : big_display_({4, 6 * 16, 28 * 8, 52}, ""),
      scanning_progress_({0, 7 * 16, screen_width, 8}),
      text_threat_summary_({0, 8 * 16, screen_width, 16}, "THREAT: NONE"),
      text_status_info_({0, 9 * 16, screen_width, 16}, "Ready"),
      text_scanner_stats_({0, 10 * 16, screen_width, 16}, "No database"),
      text_trends_compact_({0, 11 * 16, screen_width, 16}, ""),
      text_drone_1_({screen_width - 120, 12 * 16, 120, 16}, ""),
      text_drone_2_({screen_width - 120, 13 * 16, 120, 16}, ""),
      text_drone_3_({screen_width - 120, 14 * 16, 120, 16}, ""),
      detected_drones_(),
      displayed_drones_(),
      spectrum_row(), spectrum_power_levels_(), threat_bins_(), threat_bins_count_(0),
      waterfall_buffer_(),
      spectrum_gradient_(), spectrum_fifo_(nullptr),
      pixel_index(0), bins_hz_size(0), each_bin_size(100000), min_color_power(0),
      marker_pixel_step(1000000), max_power(0), range_max_power(0), mode(LOOKING_GLASS_SINGLEPASS),
      spectrum_config_(), nav_(nav)
{
    // FIX: Резервируем память заранее.
    // MAX_TRACKED_DRONES обычно около 8-10, +2 про запас.
    detected_drones_.reserve(MAX_TRACKED_DRONES + 2);

    for (auto& drone : displayed_drones_) {
        drone = DisplayDroneEntry{};
    }

    if (!spectrum_gradient_.load_file(default_gradient_file)) {
        spectrum_gradient_.set_default();
    }
    initialize_mini_spectrum();

    // --- ДОБАВИТЬ ЭТО В КОНЕЦ КОНСТРУКТОРА ---

    // Инициализация хендлеров в куче (Runtime), что безопасно для адресации
    message_handler_spectrum_config_ = new MessageHandlerRegistration(
        Message::ID::ChannelSpectrumConfig,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }
    );

    message_handler_frame_sync_ = new MessageHandlerRegistration(
        Message::ID::DisplayFrameSync,
        [this](const Message* const) {
            if (this->spectrum_fifo_) {
                ChannelSpectrum channel_spectrum;
                while (spectrum_fifo_->out(channel_spectrum)) {
                    this->process_mini_spectrum_data(channel_spectrum);
                    this->render_mini_spectrum();
                }
            }
        }
    );

    // ... остальной код конструктора ...
}

void DroneDisplayController::update_detection_display(const DroneScanner& scanner) {
    if (scanner.is_scanning_active()) {
        Frequency current_freq = scanner.get_current_scanning_frequency();
        if (current_freq > 0) {
            big_display_.set(to_string_short_freq(current_freq));
        } else {
            big_display_.set("2400.0MHz");
        }
    } else {
        big_display_.set("READY");
    }

    size_t total_freqs = scanner.get_database_size();
    if (total_freqs > 0 && scanner.is_scanning_active()) {
        uint32_t progress_percent = 50;
        scanning_progress_.set_value(std::min(progress_percent, (uint32_t)100));
    } else {
        scanning_progress_.set_value(0);
    }

    ThreatLevel max_threat = scanner.get_max_detected_threat();
    bool has_detections = (scanner.get_approaching_count() + scanner.get_receding_count() + scanner.get_static_count()) > 0;

    if (has_detections) {
        char summary_buffer[64];
        std::string threat_name = get_threat_level_name(max_threat);
        snprintf(summary_buffer, sizeof(summary_buffer), "THREAT: %s | <%zu ~%zu >%zu",
                threat_name.c_str(), scanner.get_approaching_count(),
                scanner.get_static_count(), scanner.get_receding_count());
        text_threat_summary_.set(summary_buffer);
        text_threat_summary_.set_style(Theme::getInstance()->fg_red);
    } else {
        text_threat_summary_.set("THREAT: NONE | All clear");
        text_threat_summary_.set_style(Theme::getInstance()->fg_green);
    }

    char status_buffer[64];
    if (scanner.is_scanning_active()) {
        std::string mode_str = scanner.is_real_mode() ? "REAL" : "DEMO";
        snprintf(status_buffer, sizeof(status_buffer), "%s - Detections: %lu",
                mode_str.c_str(), static_cast<unsigned long>(scanner.get_total_detections()));
    } else {
        snprintf(status_buffer, sizeof(status_buffer), "Ready - Enhanced Drone Analyzer");
    }
    text_status_info_.set(status_buffer);

    size_t loaded_freqs = scanner.get_database_size();
    char stats_buffer[64];
    if (scanner.is_scanning_active() && loaded_freqs > 0) {
        size_t current_idx = 0;
        snprintf(stats_buffer, sizeof(stats_buffer), "Freq: %zu/%zu | Cycle: %lu",
                current_idx + 1, loaded_freqs, static_cast<unsigned long>(scanner.get_scan_cycles()));
    } else if (loaded_freqs > 0) {
        snprintf(stats_buffer, sizeof(stats_buffer), "Loaded: %zu frequencies", loaded_freqs);
    } else {
        snprintf(stats_buffer, sizeof(stats_buffer), "No database loaded");
    }
    text_scanner_stats_.set(stats_buffer);

    if (max_threat >= ThreatLevel::HIGH) {
        big_display_.set_style(Theme::getInstance()->fg_red);
    } else if (max_threat >= ThreatLevel::MEDIUM) {
        big_display_.set_style(Theme::getInstance()->fg_yellow);
    } else if (has_detections) {
        big_display_.set_style(Theme::getInstance()->fg_orange);
    } else if (scanner.is_scanning_active()) {
        big_display_.set_style(Theme::getInstance()->fg_green);
    } else {
        big_display_.set_style(Theme::getInstance()->bg_darkest);
    }
}

// ===========================================
// IMPLEMENTATION: DroneDisplayController::set_scanning_status
// ===========================================
void DroneDisplayController::set_scanning_status(bool active, const std::string& message) {
    if (active) {
        text_status_info_.set("SCAN: " + message);
        text_status_info_.set_style(Theme::getInstance()->fg_green);
    } else {
        text_status_info_.set("STOP: " + message);
        // Use a color that exists in Theme, falling back to white/light
        text_status_info_.set_style(Theme::getInstance()->fg_light);
    }
}

void DroneDisplayController::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) {
    systime_t now = chTimeNow();
    auto it = std::find_if(detected_drones_.begin(), detected_drones_.end(),
                          [freq](const DisplayDroneEntry& entry) {
                              return entry.frequency == freq;
                          });
    if (it != detected_drones_.end()) {
        it->rssi = rssi;
        it->threat = threat;
        it->type = type;
        it->last_seen = now;
        it->type_name = get_drone_type_name(type);
        it->display_color = get_drone_type_color(type);
    } else {
        if (detected_drones_.size() < MAX_TRACKED_DRONES) {
            DisplayDroneEntry entry;
            entry.frequency = freq;
            entry.rssi = rssi;
            entry.threat = threat;
            entry.type = type;
            entry.last_seen = now;
            entry.type_name = get_drone_type_name(type);
            entry.display_color = get_drone_type_color(type);
            entry.trend = MovementTrend::STATIC;
            detected_drones_.push_back(entry);
        }
    }
    sort_drones_by_rssi();
    render_drone_text_display();
}

void DroneDisplayController::sort_drones_by_rssi() {
    std::sort(detected_drones_.begin(), detected_drones_.end(),
              [](const DisplayDroneEntry& a, const DisplayDroneEntry& b) {
                  if (a.rssi != b.rssi) return a.rssi > b.rssi;
                  if (a.threat != b.threat) return static_cast<int>(a.threat) > static_cast<int>(b.threat);
                  return a.last_seen > b.last_seen;
              });
}

void DroneDisplayController::update_drones_display(const DroneScanner& scanner) {
    // STEP 1: Get a safe copy of data
    // Mutex is locked only for the copying time (microseconds)
    auto snapshot = scanner.get_tracked_drones_snapshot();

    // STEP 2: Work with local snapshot copy.
    // Scanner may already be looking for new drones in another thread at this time.

    const systime_t STALE_TIMEOUT = 30000;
    systime_t now = chTimeNow();

    // Clear UI list
    detected_drones_.clear();

    // Fill from snapshot
    for (size_t i = 0; i < snapshot.count; ++i) {
        const auto& drone_data = snapshot.drones[i];

        // Stale check (duplicate logic for UI filtering)
        if ((now - drone_data.last_seen) > STALE_TIMEOUT) continue;

        DisplayDroneEntry entry;
        entry.frequency = drone_data.frequency;
        entry.type = static_cast<DroneType>(drone_data.drone_type);
        entry.threat = static_cast<ThreatLevel>(drone_data.threat_level);
        entry.rssi = drone_data.rssi;
        entry.last_seen = drone_data.last_seen;
        entry.type_name = get_drone_type_name(entry.type);
        entry.display_color = get_drone_type_color(entry.type);
        entry.trend = drone_data.get_trend(); // Now this is safe to call

        detected_drones_.push_back(entry);
    }

    // STEP 3: Sorting and rendering
    sort_drones_by_rssi();

    // ... (remaining code for updating display_drones_ and calling render)
    for (auto& drone : displayed_drones_) {
        drone = DisplayDroneEntry{};
    }
    size_t count = std::min(detected_drones_.size(), MAX_DISPLAYED_DRONES);
    for (size_t i = 0; i < count; ++i) {
        displayed_drones_[i] = detected_drones_[i];
    }
    highlight_threat_zones_in_spectrum(displayed_drones_);
    render_drone_text_display();
}

void DroneDisplayController::render_drone_text_display() {
    text_drone_1_.set("");
    text_drone_2_.set("");
    text_drone_3_.set("");
    for (size_t i = 0; i < std::min(displayed_drones_.size(), size_t(3)); ++i) {
        const auto& drone = displayed_drones_[i];
        char buffer[32];
        char trend_symbol;
        switch (drone.trend) {
            case MovementTrend::APPROACHING: trend_symbol = '<'; break;
            case MovementTrend::RECEDING: trend_symbol = '>'; break;
            case MovementTrend::STATIC:
            case MovementTrend::UNKNOWN:
            default: trend_symbol = '~'; break;
        }
        std::string freq_str;
        if (drone.frequency >= 1000000000) {
            freq_str = to_string_dec_uint(drone.frequency / 1000000000, 1) + "G";
        } else if (drone.frequency >= 1000000) {
            freq_str = to_string_dec_uint(drone.frequency / 1000000, 1) + "M";
        } else {
            freq_str = to_string_dec_uint(drone.frequency / 1000, 1) + "k";
        }
        snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
                drone.type_name.c_str(),
                freq_str.c_str(),
                (long int)drone.rssi,
                trend_symbol);
        switch(i) {
            case 0:
                text_drone_1_.set(buffer);
                break;
            case 1:
                text_drone_2_.set(buffer);
                break;
            case 2:
                text_drone_3_.set(buffer);
                break;
        }
    }
}

void DroneDisplayController::initialize_mini_spectrum() {
    if (!spectrum_gradient_.load_file(default_gradient_file)) {
        spectrum_gradient_.set_default();
    }
    clear_spectrum_buffers();
}

void DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum& spectrum) {
    uint8_t current_bin_power = 0;
    for (size_t bin = 0; bin < MINI_SPECTRUM_WIDTH; bin++) {
        get_max_power_for_current_bin(spectrum, bin, current_bin_power);
        if (process_bins(&current_bin_power)) {
            return;
        }
    }
}

bool DroneDisplayController::process_bins(uint8_t* powerlevel) {
    bins_hz_size += each_bin_size;
    if (bins_hz_size >= marker_pixel_step) {
        if (*powerlevel > min_color_power)
            add_spectrum_pixel(*powerlevel);
        else
            add_spectrum_pixel(0);
        *powerlevel = 0;

        if (!pixel_index) {
            bins_hz_size = 0;
            return true;
        }
        bins_hz_size -= marker_pixel_step;
    }
    return false;
}

void DroneDisplayController::render_mini_spectrum() {
    // 1. Write new line of data to buffer (replace the oldest)
    for (size_t x = 0; x < SPEC_WIDTH; ++x) {
        uint8_t power_value = 0;
        if (x < spectrum_power_levels_.size()) {
            power_value = spectrum_power_levels_[x];
        }
        // Convert power to color index
        uint8_t color_index = (power_value * spectrum_gradient_.lut.size()) / 256;
        if (color_index >= spectrum_gradient_.lut.size()) {
            color_index = spectrum_gradient_.lut.size() - 1;
        }
        waterfall_buffer_[waterfall_line_index_][x] = color_index;
    }

    // 2. Shift index (ring buffer)
    waterfall_line_index_ = (waterfall_line_index_ + 1) % SPEC_HEIGHT;

    // 2. Rendering (OPTIMIZED)
    const int start_y = 80;

    // Line buffer on stack (moved outside loop to avoid recreating)
    std::array<Color, SPEC_WIDTH> line_colors;

    for (int y = 0; y < SPEC_HEIGHT; ++y) {
        // Correct index calculation for "flowing down"
        // Newest line (waterfall_line_index_ - 1) should be at y=0
        size_t buf_idx = (waterfall_line_index_ + SPEC_HEIGHT - 1 - y) % SPEC_HEIGHT;

        const auto& src_row = waterfall_buffer_[buf_idx];

        // Convert indices to colors
        for (int x = 0; x < SPEC_WIDTH; ++x) {
            line_colors[x] = spectrum_gradient_.lut[src_row[x]];
        }

        // Draw line
        display.draw_pixels({{0, start_y + y}, {SPEC_WIDTH, 1}}, line_colors);
    }
}

void DroneDisplayController::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES>& drones) {
    threat_bins_count_ = 0;
    for (const auto& drone : drones) {
        if (drone.frequency > 0) {
            size_t bin_x = frequency_to_spectrum_bin(drone.frequency);
            if (bin_x < MINI_SPECTRUM_WIDTH && threat_bins_count_ < MAX_DISPLAYED_DRONES) {
                threat_bins_[threat_bins_count_].bin = bin_x;
                threat_bins_[threat_bins_count_].threat = drone.threat;
                threat_bins_count_++;
            }
        }
    }
}

void DroneDisplayController::clear_spectrum_buffers() {
    std::fill(spectrum_power_levels_.begin(), spectrum_power_levels_.end(), 0);
}

bool DroneDisplayController::validate_spectrum_data() const {
    if (spectrum_power_levels_.size() != MINI_SPECTRUM_WIDTH) return false;
    if (spectrum_gradient_.lut.empty()) return false;
    return true;
}

size_t DroneDisplayController::get_safe_spectrum_index(size_t x, size_t y) const {
    if (x >= MINI_SPECTRUM_WIDTH || y >= MINI_SPECTRUM_HEIGHT) {
        return 0;
    }
    return y * MINI_SPECTRUM_WIDTH + x;
}

void DroneDisplayController::set_spectrum_range(Frequency min_freq, Frequency max_freq) {
    if (min_freq >= max_freq || min_freq < MIN_HARDWARE_FREQ || max_freq > MAX_HARDWARE_FREQ) {
        spectrum_config_.min_freq = WIDEBAND_DEFAULT_MIN;
        spectrum_config_.max_freq = WIDEBAND_DEFAULT_MAX;
        return;
    }
    spectrum_config_.min_freq = min_freq;
    spectrum_config_.max_freq = max_freq;
    spectrum_config_.bandwidth = (max_freq - min_freq) > 24000000 ?
                                24000000 : static_cast<uint32_t>(max_freq - min_freq);
    spectrum_config_.sampling_rate = spectrum_config_.bandwidth;
}

size_t DroneDisplayController::frequency_to_spectrum_bin(Frequency freq_hz) const {
    const Frequency MIN_FREQ = spectrum_config_.min_freq;
    const Frequency MAX_FREQ = spectrum_config_.max_freq;
    const Frequency FREQ_RANGE = MAX_FREQ - MIN_FREQ;
    if (freq_hz < MIN_FREQ || freq_hz > MAX_FREQ || FREQ_RANGE == 0) {
        return MINI_SPECTRUM_WIDTH;
    }
    Frequency relative_freq = freq_hz - MIN_FREQ;
    size_t bin = (relative_freq * MINI_SPECTRUM_WIDTH) / FREQ_RANGE;
    return std::min(bin, MINI_SPECTRUM_WIDTH - 1);
}

DroneUIController::DroneUIController(NavigationView& nav,
                                   DroneHardwareController& hardware,
                                   DroneScanner& scanner,
                                   ::AudioManager& audio_mgr)
    : nav_(nav),
      hardware_(hardware),
      scanner_(scanner),
      audio_mgr_(audio_mgr),
      scanning_active_(false),
      display_controller_(new DroneDisplayController(nav)),
      settings_()
{
    settings_.spectrum_mode = SpectrumMode::MEDIUM;
    settings_.scan_interval_ms = 1000;
    settings_.rssi_threshold_db = -90;
    settings_.enable_audio_alerts = true;
}

// --- ДОБАВИТЬ ЭТОТ ДЕСТРУКТОР ---
DroneUIController::~DroneUIController() {
    if (display_controller_) {
        delete display_controller_;
        display_controller_ = nullptr;
    }
}

void DroneUIController::on_start_scan() {
    if (scanning_active_) return;
    scanning_active_ = true;
    scanner_.start_scanning();
    display_controller_->set_scanning_status(true, "Scanning Active");
    display_controller_->update_detection_display(scanner_);
}

void DroneUIController::on_stop_scan() {
    scanning_active_ = false;
    scanner_.stop_scanning();
    audio_mgr_.stop_audio();
    display_controller_->set_scanning_status(false, "Stopped");
    display_controller_->update_detection_display(scanner_);
}

void DroneUIController::on_toggle_mode() {
    if (scanner_.is_real_mode()) {
        scanner_.switch_to_demo_mode();
        if (hardware_.is_spectrum_streaming_active()) {
            hardware_.stop_spectrum_streaming();
        }
    } else {
        scanner_.switch_to_real_mode();
        if (!hardware_.is_spectrum_streaming_active()) {
            hardware_.start_spectrum_streaming();
        }
    }
    display_controller_->set_scanning_status(scanning_active_,
                                           scanner_.is_real_mode() ? "Real Mode" : "Demo Mode");
}

void DroneUIController::show_menu() {
    // БЫЛО: nav_.display_modal("EDA Menu", "Available: ...");

    // СТАЛО: Открываем наш новый класс меню
    // *this передается как ссылка на контроллер, чтобы меню могло вызывать функции
    nav_.push<DroneSettingsMenuView>(*this);
}

void DroneUIController::on_load_frequency_file() {
    if (scanner_.load_frequency_database()) {
        size_t count = scanner_.get_database_size();
        char buffer[64];
        auto s = to_string_dec_uint(count);
        strcpy(buffer, "Loaded ");
        strcat(buffer, s.c_str());
        strcat(buffer, " frequencies");
        nav_.display_modal("Success", buffer);
    } else {
        nav_.display_modal("Error", "Failed to load database");
    }
}

void DroneUIController::on_save_settings() {
    settings_.save();
    nav_.display_modal("Success", "Settings saved");
}

void DroneUIController::on_audio_settings() {
    settings_.enable_audio_alerts = !settings_.enable_audio_alerts;
    char buffer[32];
    const char* status = settings_.enable_audio_alerts ? "ENABLED" : "DISABLED";
    strcpy(buffer, "Alerts ");
    strcat(buffer, status);
    nav_.display_modal("Audio Alerts", buffer);
}

void DroneUIController::on_spectrum_mode() {
    nav_.display_modal("Spectrum Mode", "Feature not implemented in this version");
}

void DroneUIController::set_spectrum_mode(SpectrumMode mode) {
    hardware_.set_spectrum_mode(mode);
    nav_.display_modal("Applied", "Spectrum mode updated");
}

void DroneUIController::on_hardware_control() {
    show_hardware_status();
}

void DroneUIController::on_set_bandwidth() {
    uint32_t current_bw = hardware_.get_spectrum_bandwidth();
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)current_bw);
    nav_.display_modal("Set Bandwidth (MHz)", buffer);
}

void DroneUIController::on_set_center_freq() {
    Frequency current_cf = hardware_.get_spectrum_center_frequency();
    hardware_.set_spectrum_center_frequency(current_cf + 1000000);
    show_hardware_status();
}

void DroneUIController::show_hardware_status() {
    char buffer[128];
    uint32_t band_mhz = hardware_.get_spectrum_bandwidth() / 1000000ULL;
    uint32_t freq_ghz = hardware_.get_spectrum_center_frequency() / 1000000000ULL;
    uint32_t freq_decimals = (hardware_.get_spectrum_center_frequency() % 1000000000ULL) / 100000000ULL;
    snprintf(buffer, sizeof(buffer),
            "Band: %lu MHz\nFreq: %lu.%03lu GHz",
            (unsigned long)band_mhz, (unsigned long)freq_ghz, (unsigned long)freq_decimals);
    nav_.display_modal("Hardware Status", buffer);
}

void DroneUIController::on_view_logs() {
    auto open_view = nav_.push<FileLoadView>(".CSV");
    open_view->push_dir("/LOGS/EDA");
}

void DroneUIController::on_about() {
    nav_.display_modal("EDA v1.0", "Enhanced Drone Analyzer\nMayhem Firmware Integration\nBased on Recon & Looking Glass");
}

// 2. Реализация меню
DroneSettingsMenuView::DroneSettingsMenuView(NavigationView& nav, DroneUIController& controller)
    : View({0, 0, screen_width, screen_height}), controller_(controller)
{
    (void)nav;
    // Добавляем элементы на экран
    add_children({
        &button_load_db_,
        &button_audio_,
        &button_hw_,
        &button_logs_,
        &button_about_,
        &text_info_
    });

    // Настраиваем стили (опционально, для красоты)
    text_info_.set_style(Theme::getInstance()->fg_light);

    // --- ПРИВЯЗКА КНОПОК К ФУНКЦИЯМ ---

    // 1. Загрузка базы частот
    button_load_db_.on_select = [&controller](Button&) {
        controller.on_load_frequency_file();
    };

    // 2. Настройка аудио (здесь мы обновляем текст кнопки после нажатия)
    button_audio_.on_select = [this, &controller](Button&) {
        controller.on_audio_settings(); // Переключает в контроллере
        // Обновляем текст кнопки, чтобы показать новое состояние
        if (controller.settings().enable_audio_alerts) {
            button_audio_.set_text("Audio Alerts: ON");
        } else {
            button_audio_.set_text("Audio Alerts: OFF");
        }
    };
    // Устанавливаем начальный текст кнопки аудио
    if (controller.settings().enable_audio_alerts) {
        button_audio_.set_text("Audio Alerts: ON");
    } else {
        button_audio_.set_text("Audio Alerts: OFF");
    }

    // 3. Информация о железе
    button_hw_.on_select = [&controller](Button&) {
        controller.on_hardware_control();
    };

    // 4. Просмотр логов (открывает файловый менеджер)
    button_logs_.on_select = [&controller](Button&) {
        controller.on_view_logs();
    };

    // 5. О программе
    button_about_.on_select = [&controller](Button&) {
        controller.on_about();
    };
}

void DroneSettingsMenuView::focus() {
    button_load_db_.focus();
}

EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      hardware_(new DroneHardwareController()),
      scanner_(new DroneScanner()),
      audio_(),
      ui_controller_(new DroneUIController(nav, *hardware_, *scanner_, audio_)),
      display_controller_(new DroneDisplayController(nav)),
      scanning_coordinator_(new ScanningCoordinator(nav, *hardware_, *scanner_, *display_controller_, audio_)),
      smart_header_(new SmartThreatHeader(Rect{0, 0, screen_width, 60})),
      status_bar_(new ConsoleStatusBar(0, Rect{0, screen_height - 80, screen_width, 16})),
      button_start_stop_({{screen_width - 80, screen_height - 72, 72, 32}, "START/STOP"}),
      button_menu_({{screen_width - 80, screen_height - 40, 72, 32}, "MENU"}),
      button_audio_(),
      field_scanning_mode_({{10, screen_height - 72}, 15, OptionsField::options_t{{"Database", 0}, {"Wideband", 1}, {"Hybrid", 2}}}),
      scanning_active_(false),
      settings_()
{
    for (size_t i = 0; i < threat_cards_.size(); ++i) {
        size_t card_y_pos = 52 + i * 26;
        threat_cards_[i] = new ThreatCard(i, Rect{0, static_cast<int>(card_y_pos), screen_width, 24});
    }

    load_settings_from_sd_card(settings_);

    if (scanner_) {
        scanner_->update_scan_range(settings_.wideband_min_freq_hz, settings_.wideband_max_freq_hz);
    }

    if (scanning_coordinator_) {
        scanning_coordinator_->update_runtime_parameters(settings_);
    }

    initialize_modern_layout();
    setup_button_handlers();
    initialize_scanning_mode();
    add_ui_elements();
    update_modern_layout();
}

EnhancedDroneSpectrumAnalyzerView::~EnhancedDroneSpectrumAnalyzerView() {
    stop_scanning_thread();

    // Delete in reverse order of creation
    if (scanning_coordinator_) delete scanning_coordinator_;
    if (display_controller_) delete display_controller_;
    if (ui_controller_) delete ui_controller_;
    if (scanner_) delete scanner_;
    if (hardware_) delete hardware_;

    // Delete UI components
    if (status_bar_) delete status_bar_;
    if (smart_header_) delete smart_header_;

    // Delete threat cards
    for (auto& card : threat_cards_) {
        if (card) delete card;
    }
}

void EnhancedDroneSpectrumAnalyzerView::focus() {
    button_start_stop_.focus();
}

void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
    View::paint(painter);
}

bool EnhancedDroneSpectrumAnalyzerView::on_key(const KeyEvent key) {
    switch(key) {
        case KeyEvent::Back:
            stop_scanning_thread();
            nav_.pop();
            return true;
        default:
            break;
    }
    return View::on_key(key);
}

bool EnhancedDroneSpectrumAnalyzerView::on_touch(const TouchEvent event) {
    return View::on_touch(event);
}

void EnhancedDroneSpectrumAnalyzerView::on_show() {
    View::on_show();
    display.scroll_set_area(109, screen_height - 1);
    hardware_->on_hardware_show();
}

void EnhancedDroneSpectrumAnalyzerView::on_hide() {
    // 1. First stop high-level logic (scanning threads)
    // This will prevent new attempts to access hardware.
    stop_scanning_thread();

    // 2. Stop scanner logic (reset states)
    if (scanner_) {
        scanner_->stop_scanning();
    }

    // 3. Disable "hardware"
    if (hardware_) {
        // Stop spectrum streaming (Baseband)
        hardware_->stop_spectrum_streaming();
        // Completely disable radio module
        hardware_->shutdown_hardware();

        // Call on_hardware_hide (if there is specific logic there)
        hardware_->on_hardware_hide();
    }

    // 4. Pass control to base class (hide widgets)
    View::on_hide();
}

void EnhancedDroneSpectrumAnalyzerView::start_scanning_thread() {
    if (scanning_coordinator_->is_scanning_active()) return;
    scanning_coordinator_->start_coordinated_scanning();
}

void EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread() {
    if (!scanning_coordinator_->is_scanning_active()) return;
    scanning_coordinator_->stop_coordinated_scanning();
}

bool EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button() {
    if (scanning_coordinator_->is_scanning_active()) {
        ui_controller_->on_stop_scan();
        button_start_stop_.set_text("START/STOP");
    } else {
        ui_controller_->on_start_scan();
        button_start_stop_.set_text("STOP");
    }
    return true;
}

bool EnhancedDroneSpectrumAnalyzerView::handle_menu_button() {
    ui_controller_->show_menu();
    return true;
}

void EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout() {
    for (size_t i = 0; i < threat_cards_.size(); ++i) {
        if (threat_cards_[i]) {
            size_t card_y_pos = 60 + i * 26;
            threat_cards_[i]->set_parent_rect(Rect{0, static_cast<int>(card_y_pos), screen_width, 24});
        }
    }
    handle_scanner_update();
}

void EnhancedDroneSpectrumAnalyzerView::update_modern_layout() {
    handle_scanner_update();
}

void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
    if (!scanner_ || !smart_header_ || !status_bar_) return;

    ThreatLevel max_threat = scanner_->get_max_detected_threat();
    size_t approaching = scanner_->get_approaching_count();
    size_t static_count = scanner_->get_static_count();
    size_t receding = scanner_->get_receding_count();
    bool is_scanning = scanner_->is_scanning_active();
    Frequency current_freq = scanner_->get_current_scanning_frequency();
    uint32_t total_detections = scanner_->get_total_detections();

    if (smart_header_) {
        smart_header_->update(max_threat, approaching, static_count, receding,
                             current_freq, is_scanning);
    }

    if (status_bar_) {
        if (is_scanning) {
            uint32_t cycles = scanner_->get_scan_cycles();
            uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), (uint32_t)100);
            status_bar_->update_scanning_progress(progress, cycles, total_detections);
        } else if (approaching + static_count + receding > 0) {
            size_t total_drones = approaching + static_count + receding;
            const char* alert_msg = (max_threat >= ThreatLevel::CRITICAL) ? "CRITICAL THREATS!" :
                                   (max_threat >= ThreatLevel::HIGH) ? "HIGH THREATS!" : "Threats detected";
            status_bar_->update_alert_status(max_threat, total_drones, alert_msg);
        } else {
            const char* primary_msg = (!display_controller_) ?
                                     "EDA Ready" : "EDA Ready";
            char secondary_buffer[32];
            if (total_detections > 0) {
                snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: %lu", (unsigned long)total_detections);
            } else {
                strcpy(secondary_buffer, "Awaiting commands");
            }
            status_bar_->update_normal_status(primary_msg, secondary_buffer);
        }
    }

    if (display_controller_) {
        display_controller_->update_detection_display(*scanner_);
    }
}

void EnhancedDroneSpectrumAnalyzerView::setup_button_handlers() {
    button_start_stop_.on_select = [this](Button&) {
        handle_start_stop_button();
    };
    button_menu_.on_select = [this](Button&) -> void {
        ui_controller_->show_menu();
    };
    button_audio_.on_select = [this](Button&) {
        // Toggle audio alerts setting
        settings_.enable_audio_alerts = !settings_.enable_audio_alerts;
        // Update button text immediately
        button_audio_.set_text(settings_.enable_audio_alerts ? "AUDIO: ON" : "AUDIO: OFF");
        button_audio_.set_style(settings_.enable_audio_alerts ?
                               Theme::getInstance()->fg_green :
                               Theme::getInstance()->fg_medium);
    };

    field_scanning_mode_.on_change = [this](size_t index, int32_t value) -> void {
        (void)value;
        set_scanning_mode_from_index(index);
    };
}

void EnhancedDroneSpectrumAnalyzerView::initialize_scanning_mode() {
    int initial_mode = static_cast<int>(scanner_->get_scanning_mode());
    field_scanning_mode_.set_selected_index(initial_mode);
}

void EnhancedDroneSpectrumAnalyzerView::set_scanning_mode_from_index(size_t index) {
    DroneScanner::ScanningMode mode = static_cast<DroneScanner::ScanningMode>(index);
    scanner_->set_scanning_mode(mode);
    display_controller_->set_scanning_status(ui_controller_->is_scanning(),
                                             scanner_->scanning_mode_name());
    update_modern_layout();
}

void EnhancedDroneSpectrumAnalyzerView::add_ui_elements() {
    add_children({smart_header_, status_bar_});
    for (auto& card : threat_cards_) {
        if(card) add_child(card);
    }
    add_children({&button_start_stop_, &button_menu_, &button_audio_});
}

LoadingScreenView::LoadingScreenView(NavigationView& nav)
    : nav_(nav),
      text_eda_(Rect{108, 213, 24, 16}, "EDA"),
      timer_start_(chTimeNow())
{
    text_eda_.set_style(Theme::getInstance()->fg_red);
    add_child(&text_eda_);
    set_focusable(false);
}

LoadingScreenView::~LoadingScreenView() {}

void LoadingScreenView::paint(Painter& painter) {
    painter.fill_rectangle(
        {0, 0, portapack::display.width(), portapack::display.height()},
        Color::black()
    );
    View::paint(painter);
}

// ===========================================
// PART 5: SCANNINGCOORDINATOR IMPLEMENTATION
// ===========================================

ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
                                       DroneHardwareController& hardware,
                                       DroneScanner& scanner,
                                       DroneDisplayController& display_controller,
                                       ::AudioManager& audio_controller)
    : nav_(nav), hardware_(hardware), scanner_(scanner), display_controller_(display_controller), audio_controller_(audio_controller),
      scanning_active_(false), scanning_thread_(nullptr), scan_interval_ms_(750)
{
}

ScanningCoordinator::~ScanningCoordinator() {
    stop_coordinated_scanning();
}

void ScanningCoordinator::start_coordinated_scanning() {
    if (scanning_active_) return;
    scanning_active_ = true;

    scanning_thread_ = chThdCreateFromHeap(NULL, COORDINATOR_THREAD_STACK_SIZE,
                                         NORMALPRIO,
                                         scanning_thread_function, this);
    if (!scanning_thread_) {
        scanning_active_ = false;
    }
}

void ScanningCoordinator::stop_coordinated_scanning() {
    // 1. First check if thread is active at all
    if (scanning_active_) {
        // 2. Reset flag. Thread will see this in while(scanning_active_) loop and exit.
        scanning_active_ = false;

        // 3. If thread pointer exists, wait for its completion.
        if (scanning_thread_) {
            // chThdWait blocks current (UI) thread until scanning_thread_ calls chThdExit
            chThdWait(scanning_thread_);
            scanning_thread_ = nullptr;
        }
    }
}

msg_t ScanningCoordinator::scanning_thread_function(void* arg) {
    return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
}

msg_t ScanningCoordinator::coordinated_scanning_thread() {
    while (scanning_active_) {
        if (scanning_active_) {
            scanner_.perform_scan_cycle(hardware_);
            // Removed UI update from scanning thread - UI will pull data when needed
        }

        chThdSleepMilliseconds(scan_interval_ms_);
    }
    scanning_active_ = false;
    scanning_thread_ = nullptr;
    chThdExit(MSG_OK);
    return MSG_OK;
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
    (void)settings;
}

void ScanningCoordinator::show_session_summary(const std::string& summary) {
    (void)summary;
}

// ===========================================
// PART 6: DISPLAY HELPER IMPLEMENTATIONS
// ===========================================

const char* DroneDisplayController::get_drone_type_name(DroneType type) const {
    switch (type) {
        case DroneType::MAVIC: return "MAVIC";
        case DroneType::DJI_P34: return "DJI P34";
        case DroneType::UNKNOWN: default: return "UNKNOWN";
    }
}

Color DroneDisplayController::get_drone_type_color(DroneType type) const {
    switch (type) {
        case DroneType::MAVIC: return Color::red();
        case DroneType::DJI_P34: return Color::orange();
        case DroneType::UNKNOWN: default: return Color::white();
    }
}

Color DroneDisplayController::get_threat_level_color(ThreatLevel level) const {
     switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color::orange();
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        default: return Color::grey();
    }
}

const char* DroneDisplayController::get_threat_level_name(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return "CRITICAL";
        case ThreatLevel::HIGH: return "HIGH";
        case ThreatLevel::MEDIUM: return "MEDIUM";
        case ThreatLevel::LOW: return "LOW";
        default: return "NONE";
    }
}

void DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power) {
    if (bin >= spectrum.db.size()) {
        max_power = 0;
        return;
    }
    max_power = spectrum.db[bin];
}

void DroneDisplayController::add_spectrum_pixel(uint8_t power) {
    if (pixel_index < spectrum_row.size()) {
        spectrum_row[pixel_index] = spectrum_gradient_.lut[power];
        pixel_index++;
    }
}



} // namespace ui::external_app::enhanced_drone_analyzer
