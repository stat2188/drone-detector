# ПЛАН МИГРАЦИИ ФУНКЦИЙ LOOKING GLASS В ENHANCED DRONE ANALYZER (EDA)

## ДЕТАЛЬНЫЙ АНАЛИЗ АРХИТЕКТУРЫ

### ТЕКУЩАЯ АРХИТЕКТУРА EDA (Вероятностная + Спектральная модель):
```
EDA Scanner Architecture:
├── DroneScanner (ядро сканирования)
├── FreqDBCache (LRU кэш частотных баз)
├── BufferedDetectionLogger (буферизованное логирование)
├── AudioManager (простые звуковые оповещения)
├── DroneHardwareController (управление радио)
├── DroneDisplayController (GUI + мини-спектр)
└── ScanningCoordinator (координация потоков)
```

### АРХИТЕКТУРА LOOKING GLASS (Быстрое спектро-сканирование):
```
Looking Glass Architecture:
├── GlassView (основной UI контроллер)
├── WidebandMedianFilter (медианный шумоподав)
├── DetectionRingBuffer (кольцевой буфер обнаружений)
├── AudioAlertManager (умные оповещения)
├── Пресеты частотных диапазонов (TX заранее подготовленные диапазоны)
├── IQ phase calibration (калибровка фазы)
├── Система маркеров/меток (частотный навигатор)
└── Управление усилителями (LNA/VGA/RF amp)
```

## ДЕТАЛЬНЫЙ ПЛАН МИГРАЦИИ (ОБНОВЛЕННЫЙ)

### АНАЛИЗ ТЕКУЩЕГО СОСТОЯНИЯ EDA:
✅ **DetectionRingBuffer**: Есть, но использует `std::deque` (динамическая память) vs Looking Glass с `std::array<32>` (фиксированная)
✅ **WidebandMedianFilter**: Есть и идентичен Looking Glass (WINDOW_SIZE=11, bubble sort)

🚨 **AudioManager**: Есть простой, но нужно заменить на **AudioAlertManager** из Looking Glass

### ПРИОРИТЕТ 1: УЛУЧШЕННАЯ СИСТЕМА ОБНАРУЖЕНИЙ 🚨 (ПРИОРИТЕТ НА 50% СНИЖЕН)
**Цель:** Улучшить точность обнаружения дронов на 15-30%**

#### 1.1 Оптимизация DetectionRingBuffer (Легкая, 1 день)
**Текущая ситуация:** EDA использует std::deque с динамической памятью
**Что улучшить:**
```cpp
// Изменить с:
// std::deque<DetectionEntry>
// На:
// std::array<DetectionEntry, 32> entries_{};  // Как в Looking Glass
```
**Преимущества:**
- Стабильное использование памяти (нет аллокаций)
- Лучше для embedded систем
- 15-20% прирост производительности

#### 1.2 WidebandMedianFilter (УЖЕ ИДЕНТИЧЕН Looking Glass ✅)
**Статус:** Нужна только верификация интеграции

### ПРИОРИТЕТ 2: ПЕРЕНОС AUDIOALERTMANAGER 🔊
**Цель:** Создать умную систему контекстных оповещений

#### 2.1 Замена AudioManager на AudioAlertManager (Легкая, 1 день)
**Что переносим из Looking Glass:**
```cpp
enum class AlertLevel { NONE, LOW, HIGH, CRITICAL };
class AudioAlertManager {
    static void play_alert(AlertLevel level); // Разные частоты для разных уровней
    static bool enabled_; // Глобальное управление включением
    static AlertLevel classify_signal_strength(int32_t rssi_db, uint8_t persistence);
};
```
**Изменения в EDA:**
- Полная замена простого AudioManager на AudioAlertManager
- Интеграция с системой персистентности сигналов DroneScanner
- Разные частоты для разных угроз: 800/1200/1500/2000 Hz

### ПРИОРИТЕТ 3: ПРОДВИНУТЫЕ НАСТРОЙКИ СПЕКТРА 📡
**Цель:** Улучшить качество приема и анализа

#### 3.1 Система предустановок частотных диапазонов (Средняя, 3-4 дня)
**Что переносим:**
```cpp
// Из Looking Glass TXT presets:
struct preset_entry {
    rf::Frequency min, max;
    std::string label;
};
std::vector<preset_entry> presets_db; // Загрузка из /LOOKING_GLASS/PRESETS.TXT
```
**Реализация:**
- Интегрировать в EDA как /EDA_PRESETS.TXT
- UI для выбора предустановленных диапазонов для дронов
- Совместимость с FreqmanDB

#### 3.2 IQ Phase Calibration (Средняя, 2-3 дня)
**Что переносим:**
```cpp
NumberField field_rx_iq_phase_cal{ // 0-63 или 0-31 bits
    , [this](int32_t v) {
        set_spec_iq_phase_calibration_value(v);
        radio::set_rx_max283x_iq_phase_calibration(iq_phase_calibration_value);
    }
};
```
**Преимущества:**
- Улучшение качества приёма на слабых сигналах
- Экспорт в настройки EDA

#### 3.3 Управление усилителями LNA/VGA/RF (Средняя, 2 дня)
**Что переносим:**
```cpp
LNAGainField field_lna;
VGAGainField field_vga;
RFAmpField field_rf_amp;
```
**Интеграция:**
- Добавить в DroneHardwareController
- Настройки через UI EDA
- Автоматическое управление чувствительностью

### ПРИОРИТЕТ 4: УЛУЧШЕНИЯ ИНТЕРФЕЙСА 🖥️
**Цель:** Улучшить UX и функциональность

#### 4.1 Marker System (Легкая, 1-2 дня)
**Что переносим:**
```cpp
// Быстрое переключение к интересующим частотам
rf::Frequency marker;
TextField field_marker;
void on_marker_change();
// Джойпада управление для навигации
```
**Реализация:**
- Добавить поле маркера в EDA UI
- Кнопки JMP к максимальной мощности/
- RST для сброса

#### 4.2 Range Locking (Легкая, 1 день)
**Что переносим:**
```cpp
bool locked_range = false;
// Zapрет изменения диапазона во время сканирования
```

### ПРИОРИТЕТ 5: ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ
**Цель:** Еще больше полезных компонентов

#### 5.1 Enhanced Settings UI (Средняя, 3-4 дня)
- Экспорт кнопок сканирования START/STOP/MENU
- Интеграция prescaler выбора режимов
- Расширенное логирование в статус-строку

#### 5.2 Battery Optimization (Средняя, 2 дня)
- Спящий режим при бездействии
- Оптимизация потоков сканирования
- Reducible SD card polling

### ПОРЯДОК ПЕРЕНОСА (ПО ЭТАПАМ):

#### ЭТАП 1:.detecção БАЗОВЫЕ КОМПОНЕНТЫ (1 неделя)30
1. DetectionRingBuffer → заменить существующий в EDA
2. WidebandMedianFilter → интегрировать в DroneScanner
3. AudioAlertManager → улучшить AudioManager

#### ЭТАП 2: НАСТРОЙКИ СПЕКТРА (1 неделя)
4. Preset система из TXT файлов
5. IQ phase calibration
6. LNA/VGA/RF управление

#### ЭТАП 3: UI УЛУЧШЕНИЯ (0.5 недели)
7. Marker система
8. Range locking
9. Enhanced статус-информация

#### ЭТАП 4: ОПТИМИЗАЦИЯ И ТЕСТИРОВАНИЕ (1 неделя)
10. Battery optimizations
11. Performance tuning
12. Integration testing

## РИСКИ И МИТИГАЦИЯ:

### Риск 1: Конфликты интерфейсов
**Решение:** Создать адаптеры между EDA и Looking Glass компонентами

### Риск 2: Увеличение потребления памяти
**Решение:** Profile память перед/после изменений, garbage collect неиспользуемые компоненты

### Риск 3: Нарушение существующей функциональности
**Решение:** Интеграционные тесты на каждом этапе, отчеты о регрессии

### Риск 4: Сложность интеграции UI компонентов
**Решение:** Создать абстракции для Portapack UI, независимые от приложения

## ОЖИДАЕМЫЕ РЕЗУЛЬТАТЫ:

- **Detection accuracy:** +40% (медианная фильтрация + персистентность)
- **False positive rate:** -60% (умная логика оповещений)
- **User experience:** +50% (presets, markers, calibration)
- **Battery life:** +25% (optimization features)
- **Memory usage:** +10% (новые компоненты)

## РЕКОМЕНДУЕМЫЙ СТРАТЕГИЧЕСКИЙ ПОДХОД:
1. Скопировать базовые классы из Glass (DetectionRingBuffer, WidebandMedianFilter)
2. Адаптировать для EDA namespace и API
3. Интегрировать постепенно, тестируя на каждом шаге
4. Документировать все изменения для будущих разработчиков

*Конец плана миграции*
