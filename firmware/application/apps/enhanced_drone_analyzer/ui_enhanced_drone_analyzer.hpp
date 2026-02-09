#ifndef UI_SCANNER_COMBINED_HPP_
#define UI_SCANNER_COMBINED_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <atomic>
#include <memory>
#include <cstdio>

#include "ui_drone_common_types.hpp"
#include "ui_signal_processing.hpp"
#include "settings_persistence.hpp"
#include "scanning_coordinator.hpp"
#include "gradient.hpp"
#include "ui_drone_audio.hpp"
#include "ui_spectral_analyzer.hpp"
#include "drone_constants.hpp"
#include "eda_optimized_utils.hpp"
#include "color_lookup_unified.hpp"

#include "ui.hpp"
#include "event_m0.hpp"
#include "ui_widget.hpp"
#include "message.hpp"

#include "freqman_db.hpp"
#include "freqman.hpp"
#include "log_file.hpp"
#include <ch.h>
#include <chtypes.h>

#include "radio_state.hpp"
#include "baseband_api.hpp"
#include "portapack.hpp"
#include "radio.hpp"
#include "irq_controls.hpp"

using rf::Frequency;

#include "ui_navigation.hpp"
#include "app_settings.hpp"
#include "string_format.hpp"
#include "tone_key.hpp"
#include "message_queue.hpp"
// #include "eda_unified_settings.hpp"  // DEPRECATED - removed, use DroneAnalyzerSettings instead

class LogFile;

// Enum for scanner modes
enum class ScannerMode : uint8_t {
    DATABASE_ONLY = 0,
    WIDEBAND_ONLY = 1,
    HYBRID = 2,
    SPECTRUM_VIEW = 3
};

// Application specific namespace starts here to ensure all classes are properly scoped
namespace ui::apps::enhanced_drone_analyzer {

// Signal processing utilities
#include "ui_signal_processing.hpp"

struct preset_entry {
    Frequency min = 0;
    Frequency max = 0;
    std::string label;
};

struct RssiMeasurement {
    int16_t rssi_db;
    systime_t timestamp_ms;
};

// Constants moved to drone_constants.hpp - single source of truth

namespace UIStyles {
    static constexpr Style RED_STYLE{font::fixed_8x16, Color::black(), Color::red()};
    static constexpr Style YELLOW_STYLE{font::fixed_8x16, Color::black(), Color::yellow()};
    static constexpr Style GREEN_STYLE{font::fixed_8x16, Color::black(), Color::green()};
    static constexpr Style LIGHT_STYLE{font::fixed_8x16, Color::black(), Color::white()};
    static constexpr Style DARK_STYLE{font::fixed_8x16, Color::black(), Color::dark_grey()};
    static constexpr Style ORANGE_STYLE{font::fixed_8x16, Color::black(), Color::orange()};
}

class TrackedDrone {
public:
    TrackedDrone() : frequency(0), drone_type(static_cast<uint8_t>(DroneType::UNKNOWN)),
                     threat_level(static_cast<uint8_t>(ThreatLevel::NONE)), update_count(0),
                     last_seen(0), rssi(-120), rssi_history_{}, timestamp_history_{}, history_index_(0) {
        // Initialize array with "silence" (-120 dBm), not zeros
        std::fill(std::begin(rssi_history_), std::end(rssi_history_), -120);
        std::fill(std::begin(timestamp_history_), std::end(timestamp_history_), 0);
    }

    TrackedDrone& operator=(const TrackedDrone& other) {
        if (this != &other) {
            frequency = other.frequency;
            drone_type = other.drone_type;
            threat_level = other.threat_level;
            update_count = other.update_count;
            last_seen = other.last_seen;
            rssi = other.rssi;
            for(size_t i=0; i<MAX_HISTORY; i++) {
                rssi_history_[i] = other.rssi_history_[i];
                timestamp_history_[i] = other.timestamp_history_[i];
            }
            history_index_ = other.history_index_;
        }
        return *this;
    }

    void add_rssi(const RssiMeasurement& measurement) {
        rssi_history_[history_index_] = measurement.rssi_db;
        timestamp_history_[history_index_] = measurement.timestamp_ms;
        history_index_ = (history_index_ + 1) % MAX_HISTORY;

        this->rssi = measurement.rssi_db;

        if (measurement.timestamp_ms > last_seen) {
            last_seen = measurement.timestamp_ms;
            if (update_count < 255) update_count++;
        }
    }

    MovementTrend get_trend() const {
        if (update_count < 4) return MovementTrend::UNKNOWN;

        int32_t recent_sum = 0;
        int32_t older_sum = 0;
        size_t half_window = MAX_HISTORY / 2;

        for (size_t i = 0; i < MAX_HISTORY; i++) {
            size_t logical_idx = (history_index_ + i) % MAX_HISTORY;
            int16_t val = rssi_history_[logical_idx];
            if (val <= -110) continue;

            if (i < half_window) {
                older_sum += val;
            } else {
                recent_sum += val;
            }
        }

        int32_t avg_old = older_sum / (int32_t)half_window;
        int32_t avg_new = recent_sum / (int32_t)half_window;
        int32_t diff = avg_new - avg_old;

        if (diff > 3) return MovementTrend::APPROACHING;
        if (diff < -3) return MovementTrend::RECEDING;
        return MovementTrend::STATIC;
    }

    uint32_t frequency;
    uint8_t drone_type;
    uint8_t threat_level;
    uint8_t update_count;
    systime_t last_seen;
    int32_t rssi = -120;

private:
    static constexpr size_t MAX_HISTORY = 8;
    int16_t rssi_history_[MAX_HISTORY];
    systime_t timestamp_history_[MAX_HISTORY] = {0};
    size_t history_index_ = 0;
};

struct DisplayDroneEntry {
    Frequency frequency = 0;
    DroneType type = DroneType::UNKNOWN;
    ThreatLevel threat = ThreatLevel::NONE;
    int32_t rssi = -120;
    systime_t last_seen = 0;
    std::string type_name = "";
    Color display_color = Color::white();
    MovementTrend trend = MovementTrend::UNKNOWN;
};

// Removed duplicate constants - using from drone_constants.hpp
// MAX_TRACKED_DRONES, MAX_DISPLAYED_DRONES defined in drone_constants.hpp
// Constants moved to drone_constants.hpp - single source of truth

// Local constants for DroneDisplayController (not in drone_constants.hpp)
static constexpr int SPEC_WIDTH = 240;
static constexpr int SPEC_HEIGHT = 40;  // Heap optimization: 80 -> 40 (saves 9.6KB)

struct WidebandSlice {
    Frequency center_frequency;
    size_t index;
};

struct WidebandScanData {
    Frequency min_freq;
    Frequency max_freq;
    size_t slices_nb;
    WidebandSlice slices[20];
    size_t slice_counter;

    void reset() {
        min_freq = WIDEBAND_DEFAULT_MIN;
        max_freq = WIDEBAND_DEFAULT_MAX;
        slices_nb = 0;
        slice_counter = 0;
    }
};

struct DroneDetectionMessage {
    DroneType type;
    Frequency frequency;
    int32_t rssi;
    ThreatLevel threat_level;
    systime_t timestamp;
};

struct DroneUpdateMessage {
    enum class Type { DETECTION, STALE_REMOVAL, SCAN_COMPLETE } type;
    union {
        DroneDetectionMessage detection;
        systime_t scan_complete_timestamp;
    } data;
};

// Struct for drone signal parameters (prevents easily-swappable-parameters warning)
struct DroneSignal {
    Frequency frequency_hz;
    int32_t rssi_db;
};

// 🔴 NEW: Enhanced Settings Validation with detailed checks
class EnhancedDroneSettingsValidator {
public:
struct ValidationResult {
    bool is_valid;
    uint32_t warning_count;
    std::string error_message;

    ValidationResult() : is_valid(true), warning_count(0), error_message{} {}
};
    
    static ValidationResult validate_all(const DroneAnalyzerSettings& settings);
    
private:
    static bool validate_frequency(Frequency freq, std::string& error);
    static bool validate_rssi_threshold(int32_t rssi, std::string& error);
    static bool validate_scan_interval(uint32_t interval_ms, std::string& error);
    static bool validate_audio_params(uint32_t freq_hz, uint32_t duration_ms, std::string& error);
    static bool validate_bandwidth(uint32_t bandwidth_hz, std::string& error);
    static bool validate_frequency_range(Frequency min_hz, Frequency max_hz, std::string& error);
    
    // Enhanced frequency validation with drone band checks
    static bool is_known_drone_band(Frequency freq);
    static bool is_ism_band(Frequency freq);
    static std::string format_frequency_hz(Frequency freq);
};

// RAII wrapper for ChibiOS mutexes
class MutexLock {
public:
    explicit MutexLock(Mutex& mtx) : mtx_(mtx) {
        chMtxLock(&mtx_);
    }

    ~MutexLock() {
        chMtxUnlock();
    }

    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;

private:
    Mutex& mtx_;
};

// 🔴 OPTIMIZATION: String Pool for heap fragmentation reduction
// Scott Meyers Item 29: Consider using object pools for frequently allocated objects
class StringPool {
public:
    static constexpr size_t POOL_SIZE = 2048;
    static constexpr size_t MAX_STRING_LENGTH = 256;

    StringPool() : pool_{}, offset_(0) {
    }

    char* allocate(size_t length) {
        if (length >= MAX_STRING_LENGTH) return nullptr;

        // Check if we have enough space
        if (offset_ + length + 1 >= POOL_SIZE) {
            // Pool full - reset to beginning (simple circular buffer)
            offset_ = 0;
        }

        char* result = pool_ + offset_;
        offset_ += length + 1;
        pool_[offset_ - 1] = '\0';  // Null-terminate
        return result;
    }

    void reset() {
        offset_ = 0;
    }

    StringPool(const StringPool&) = delete;
    StringPool& operator=(const StringPool&) = delete;

private:
    char pool_[POOL_SIZE];
    size_t offset_;
};

class DroneDetectionLogger {
public:
    DroneDetectionLogger();
    ~DroneDetectionLogger();

    // Producer method - called by scanner thread (non-blocking)
    bool log_detection_async(const DetectionLogEntry& entry);
    
    // Consumer lifecycle methods
    void start_worker();
    void stop_worker();
    
    // Session management
    void start_session();
    void end_session();
    bool is_session_active() const { return session_active_; }
    
    // Statistics for monitoring
    uint32_t get_dropped_logs_count() const { return dropped_logs_; }
    uint32_t get_logged_count() const { return logged_count_; }

    std::string get_log_filename() const;
    std::string format_session_summary(size_t scan_cycles, size_t total_detections) const;
    
private:
    // --- THREADING PRIMITIVES ---
    Thread* worker_thread_ = nullptr;           // Declared 1st
    mutable Mutex mutex_;                       // Declared 2nd
    Semaphore data_ready_;                      // Declared 3rd
    volatile bool worker_should_run_ = false;   // Declared 4th
    
    // --- FILE I/O ---
    LogFile csv_log_;                           // Declared 5th
    bool session_active_ = false;               // Declared 6th
    systime_t session_start_ = 0;               // Declared 7th
    uint32_t logged_count_ = 0;                 // Declared 8th
    uint32_t dropped_logs_ = 0;                 // Declared 9th
    bool header_written_ = false;               // Declared 10th
    
    // --- ASYNC BUFFERING ---
    static constexpr size_t BUFFER_SIZE = 32;   // Declared 11th
    std::array<DetectionLogEntry, BUFFER_SIZE> ring_buffer_; // Declared 12th
    size_t head_ = 0;                           // Declared 13th
    size_t tail_ = 0;                           // Declared 14th
    bool is_full_ = false;                      // Declared 15th

    // Helper buffer for string formatting (avoid heap allocation)
    char line_buffer_[128];                     // Declared last

    // --- INTERNAL METHODS ---
    static msg_t worker_thread_entry(void* arg);
    void worker_loop();
    bool write_entry_to_sd(const DetectionLogEntry& entry);
    bool ensure_csv_header();
    std::string generate_log_filename() const;

    DroneDetectionLogger(const DroneDetectionLogger&) = delete;
    DroneDetectionLogger& operator=(const DroneDetectionLogger&) = delete;
};

class DroneScanner {
public:
    // Built-in database structure for frequencies
    struct BuiltinDroneFreq {
        Frequency freq;
        const char* desc;
        DroneType type;
    };

    // 🔴 OPTIMIZATION: constexpr array instead of vector to avoid heap allocation
    // Scott Meyers Item 15: Prefer constexpr to #define and const
    static constexpr size_t BUILTIN_DB_SIZE = 31;
    static const std::array<BuiltinDroneFreq, BUILTIN_DB_SIZE> BUILTIN_DRONE_DB;

    // Scanning modes for DroneScanner (different from DroneConstants::ScanningMode)
    enum class ScanningMode {
        DATABASE,
        WIDEBAND_CONTINUOUS,
        HYBRID
    };

    // Alias to avoid ambiguity with DroneConstants::ScanningMode
    using ScannerMode = ScanningMode;

    DroneScanner(const DroneAnalyzerSettings& settings);
    ~DroneScanner();

    void start_scanning();
    void stop_scanning();
    bool is_scanning_active() const { return scanning_active_; }
    bool load_frequency_database();
    size_t get_database_size() const;

    ScanningMode get_scanning_mode() const { return scanning_mode_; }
    const char* scanning_mode_name() const;
    void set_scanning_mode(ScanningMode mode);

    void switch_to_real_mode();
    void switch_to_demo_mode();

    void update_scan_range(Frequency min_freq, Frequency max_freq) {
        if (min_freq >= max_freq) return;
        if (min_freq < 1000000) min_freq = 1000000;
        if (max_freq > 7200000000LL) max_freq = 7200000000LL;
        setup_wideband_range(min_freq, max_freq);
    }

    int32_t get_detection_rssi_safe(size_t freq_hash) const;
    uint8_t get_detection_count_safe(size_t freq_hash) const;

// Struct for drone detection parameters (prevents easily-swappable-parameters warning)
struct DetectionParams {
    DroneType type;
    Frequency frequency_hz;
    int32_t rssi_db;
    ThreatLevel threat_level;
};

    void perform_scan_cycle(DroneHardwareController& hardware);
    void process_rssi_detection(const freqman_entry& entry, int32_t rssi);
    void send_drone_detection_message(const DetectionParams& params);

    void update_tracked_drone(const DetectionParams& params);

    void update_tracked_drone_internal(const DetectionParams& params);
    // @note: Caller MUST hold data_mutex. This method assumes the mutex is already acquired.

    void remove_stale_drones();

    rf::Frequency get_current_scanning_frequency() const;
    ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
    const TrackedDrone& getTrackedDrone(size_t index) const;  // 🔴 FIX: Protected with mutex
    void handle_scan_error(const char* error_msg);

    std::string get_session_summary() const;
    size_t get_approaching_count() const { return approaching_count_; }
    size_t get_receding_count() const { return receding_count_; }
    size_t get_static_count() const { return static_count_; }
    uint32_t get_total_detections() const { return total_detections_; }
    uint32_t get_scan_cycles() const { return scan_cycles_; }
    bool is_real_mode() const { return is_real_mode_; }
    size_t get_total_memory_usage() const { return 0; }

    DroneScanner(const DroneScanner&) = delete;
    DroneScanner(DroneScanner&&) = delete;
    DroneScanner& operator=(const DroneScanner&) = delete;
    DroneScanner& operator=(DroneScanner&&) = delete;

    // DIAMOND OPTIMIZATION: Unified lookup from eda_optimized_utils.hpp
    const char* get_drone_type_name(DroneType type) const {
        return StringMappings::get_drone_type_name(static_cast<uint8_t>(type));
    }
    Color get_drone_type_color(DroneType type) const {
        return Color(ColorMappings::get_drone_color_value(static_cast<uint8_t>(type)));
    }

    Frequency get_current_radio_frequency() const;

    struct DroneSnapshot {
        TrackedDrone drones[DroneConstants::MAX_TRACKED_DRONES];
        size_t count = 0;
    };

    DroneSnapshot get_tracked_drones_snapshot() const;

    bool try_get_tracked_drones_snapshot(DroneSnapshot& out_snapshot) const;

    // 🔴 FIX: Public API for safe initialization
    // These methods must be public to allow lazy initialization after constructor
    void initialize_database_and_scanner();
    void cleanup_database_and_scanner();

 private:
    void reset_scan_cycles();
    void initialize_wideband_scanning();
    void setup_wideband_range(Frequency min_freq, Frequency max_freq);
    void wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override);
    void process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                   int32_t original_threshold, int32_t wideband_threshold);

    void process_spectral_detection(const freqman_entry& entry,
                                   const SpectralAnalysisResult& analysis_result,
                                   ThreatLevel threat_level, DroneType drone_type);

    bool validate_detection_simple(int32_t rssi_db, ThreatLevel threat);

    static msg_t scanning_thread_function(void* arg);
    msg_t scanning_thread();

    void scan_init_from_loaded_frequencies();

    void perform_database_scan_cycle(DroneHardwareController& hardware);
    void perform_wideband_scan_cycle(DroneHardwareController& hardware);
    void perform_hybrid_scan_cycle(DroneHardwareController& hardware);

    void update_tracking_counts();

    // Helper for heap-allocated tracked_drones_ access
    std::array<TrackedDrone, DroneConstants::MAX_TRACKED_DRONES>& tracked_drones() {
        return *tracked_drones_ptr_;
    }
    const std::array<TrackedDrone, DroneConstants::MAX_TRACKED_DRONES>& tracked_drones() const {
        return *tracked_drones_ptr_;
    }

    // Intelligent scanning methods
    size_t get_next_slice_with_intelligence();
    void update_frequency_predictions(Frequency detected_freq, ThreatLevel threat_level);
    void update_priority_slice_detection(size_t slice_idx, bool detected_something_interesting);

     Thread* scanning_thread_ = nullptr;
     mutable Mutex data_mutex;
     std::atomic<bool> scanning_active_{false};

     // 🔴 OPTIMIZATION: Heap-allocated FreqmanDB (saves ~2KB stack in constructor)
     std::unique_ptr<FreqmanDB> freq_db_ptr_;
     size_t current_db_index_ = 0;
     Frequency last_scanned_frequency_ = 0;
     bool freq_db_loaded_ = false;

     std::atomic<uint32_t> scan_cycles_{0};
     std::atomic<uint32_t> total_detections_{0};

     ScanningMode scanning_mode_ = ScanningMode::DATABASE;  // Uses DroneScanner::ScanningMode
     std::atomic<bool> is_real_mode_{true};

     // 🔴 OPTIMIZATION: Heap-allocated tracked_drones (reduces stack usage)
     std::unique_ptr<std::array<TrackedDrone, DroneConstants::MAX_TRACKED_DRONES>> tracked_drones_ptr_;
    size_t tracked_count_ = 0;

    size_t approaching_count_ = 0;
    size_t receding_count_ = 0;
    size_t static_count_ = 0;

    ThreatLevel max_detected_threat_ = ThreatLevel::NONE;
    int32_t last_valid_rssi_ = -120;

    static constexpr uint8_t DETECTION_DELAY = 2;
    WidebandScanData wideband_scan_data_;
    DroneDetectionLogger detection_logger_;
    DetectionRingBuffer detection_ring_buffer_;

    // Intelligent scanning features
    int32_t priority_slice_index_ = -1;  // For priority scanning
    mutable Mutex priority_slice_mutex_;  // 🔴 FIX: Race condition protection
    size_t priority_scan_counter_ = 0;   // Counter for priority slice scanning
    static constexpr size_t PRIORITY_SCAN_INTERVAL = 3; // Scan priority slice every N cycles
    static constexpr size_t MAX_FREQUENCY_PREDICTIONS = 5; // Max predicted frequencies to track
    
    // Frequency prediction for FHSS drones
    struct FrequencyPrediction {
        Frequency predicted_freq;
        size_t confidence;
        systime_t last_seen;
    };
    std::array<FrequencyPrediction, MAX_FREQUENCY_PREDICTIONS> frequency_predictions_{};
    mutable Mutex predictions_mutex_;  // 🔴 FIX: Race condition protection
    size_t prediction_count_ = 0;

    // Settings for user-defined frequency ranges
    const DroneAnalyzerSettings& settings_;

    // ===========================================
    // DIAMOND OPTIMIZATION: LUTs в конце класса (после всех объявлений)
    // ===========================================
    // Scott Meyers Item 15: Prefer constexpr to #define
    // Все LUT объявлены здесь для доступа к private членам/методам
    
    // LUT для MovementTrend счётчиков (указатели на члены класса)
    static constexpr size_t DroneScanner::* const TREND_COUNTERS[] = {
        &DroneScanner::static_count_,      // STATIC = 0
        &DroneScanner::approaching_count_, // APPROACHING = 1
        &DroneScanner::receding_count_,    // RECEDING = 2
        &DroneScanner::static_count_       // UNKNOWN = 3 (fallback)
    };
    static_assert(sizeof(TREND_COUNTERS) / sizeof(size_t DroneScanner::*) == 4, "TREND_COUNTERS size");
    
    // LUT для функций сканирования (указатели на методы класса)
    static constexpr void (DroneScanner::* const SCAN_FUNCTIONS[])(DroneHardwareController&) = {
        &DroneScanner::perform_database_scan_cycle,    // DATABASE = 0
        &DroneScanner::perform_wideband_scan_cycle,   // WIDEBAND_CONTINUOUS = 1
        &DroneScanner::perform_hybrid_scan_cycle        // HYBRID = 2
    };
    static_assert(sizeof(SCAN_FUNCTIONS) / sizeof(void (DroneScanner::*)(DroneHardwareController&)) == 3, "SCAN_FUNCTIONS size");
};

class DroneHardwareController {
public:
    explicit DroneHardwareController(SpectrumMode mode = SpectrumMode::MEDIUM);
    ~DroneHardwareController();

    void initialize_hardware();
    void on_hardware_show();
    void on_hardware_hide();
    void shutdown_hardware();

    void set_spectrum_mode(SpectrumMode mode);
    uint32_t get_spectrum_bandwidth() const;
    void set_spectrum_bandwidth(uint32_t bandwidth_hz);
    Frequency get_spectrum_center_frequency() const;
    void set_spectrum_center_frequency(Frequency center_freq);

    bool tune_to_frequency(Frequency frequency_hz);
    void start_spectrum_streaming();
    void stop_spectrum_streaming();
    int32_t get_real_rssi_from_hardware(Frequency target_frequency);
    bool is_spectrum_streaming_active() const;
    int32_t get_current_rssi() const;
    void update_spectrum_for_scanner();

    void stop_spectrum_before_scan();
    void resume_spectrum_after_scan();
    bool is_spectrum_compatible_with_scanning() const;
    
    void clear_rssi_flag();
    bool is_rssi_fresh() const;
    
    // 🔴 FIXED: TOCTOU race condition - atomic check-and-fetch method
    bool get_rssi_if_fresh(int32_t& out_rssi) {
        // Atomic check-and-fetch to prevent TOCTOU race
        bool was_fresh = rssi_updated_.exchange(false, std::memory_order_acq_rel);
        if (was_fresh) {
            out_rssi = last_valid_rssi_.load(std::memory_order_acquire);
        }
        return was_fresh;
    }

    // NEW: Spectrum data access method (atomic check-and-fetch to avoid TOCTOU race)
    bool get_latest_spectrum_if_fresh(std::array<uint8_t, 256>& out_db_buffer) {
        // Atomic check-and-fetch to prevent TOCTOU race
        bool was_fresh = spectrum_updated_.exchange(false, std::memory_order_acq_rel);
        if (was_fresh) {
            MutexLock lock(spectrum_mutex_);
            out_db_buffer = last_spectrum_db_;
        }
        return was_fresh;
    }
    bool try_get_latest_spectrum(std::array<uint8_t, 256>& out_db_buffer) {
        MutexLock lock(spectrum_mutex_);
        out_db_buffer = last_spectrum_db_;
        return true;
    }
    void clear_spectrum_flag() {
        spectrum_updated_.store(false, std::memory_order_release);
    }

    // 🔴 FIX: Public method to set spectrum FIFO (called from parent View)
    void set_spectrum_fifo(ChannelSpectrumFIFO* fifo) {
        spectrum_fifo_ = fifo;
    }

    void handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message);
    void handle_channel_statistics(const ChannelStatistics& statistics);
    void handle_channel_spectrum(const ChannelSpectrum& spectrum);
    void process_channel_spectrum_data(const ChannelSpectrum& spectrum);

    DroneHardwareController(const DroneHardwareController&) = delete;
    DroneHardwareController& operator=(const DroneHardwareController&) = delete;

private:
    void initialize_radio_state();
    void initialize_spectrum_collector();
    void cleanup_spectrum_collector();

    int32_t get_configured_sampling_rate() const;
    int32_t get_configured_bandwidth() const;
    
    // 🔴 FIX: Spectrum data buffer and synchronization
    std::array<uint8_t, 256> last_spectrum_db_;
    mutable Mutex spectrum_mutex_;
    
    // 🔴 FIX: Replace volatile with atomic with explicit memory ordering
    std::atomic<bool> spectrum_updated_{false};

    SpectrumMode spectrum_mode_;
    Frequency center_frequency_;
    uint32_t bandwidth_hz_;
    RxRadioState radio_state_;
    ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
    bool spectrum_streaming_active_ = false;
    
    // 🔴 FIX: Replace volatile with atomic with explicit memory ordering
    std::atomic<bool> rssi_updated_{false};
    std::atomic<int32_t> last_valid_rssi_{-120};
    
    // 🔴 FIX: Moved message handlers to parent View to prevent MsgDblReg
    // Only ChannelStatistics handler remains here as it's unique
    
    MessageHandlerRegistration message_handler_channel_statistics_ {
        Message::ID::ChannelStatistics,
        [this](Message* const p) {
            const auto statistics_msg = static_cast<const ChannelStatisticsMessage*>(p);
            this->handle_channel_statistics(statistics_msg->statistics);
        }
    };
};

class SmartThreatHeader : public View {
public:
    explicit SmartThreatHeader(Rect parent_rect = {0, 0, screen_width, 60});
    ~SmartThreatHeader() = default;

    void update(ThreatLevel max_threat, size_t approaching, size_t static_count,
                size_t receding, Frequency current_freq, bool is_scanning);
    void set_max_threat(ThreatLevel threat);
    void set_movement_counts(size_t approaching, size_t static_count, size_t receding);
    void set_current_frequency(Frequency freq);
    void set_scanning_state(bool is_scanning);
    void set_color_scheme(bool use_dark_theme);

    SmartThreatHeader(const SmartThreatHeader&) = delete;
    SmartThreatHeader& operator=(const SmartThreatHeader&) = delete;

private:
    ProgressBar threat_progress_bar_ {{0, 0, screen_width, 16}};
    Text threat_status_main_ {{0, 20, screen_width, 16}, "THREAT: LOW | ▲0 ■0 ▼0"};
    Text threat_frequency_ {{0, 38, screen_width, 16}, "2400.0MHz SCANNING"};

    Color get_threat_bar_color(ThreatLevel level) const;
    Color get_threat_text_color(ThreatLevel level) const;
    const char* get_threat_icon_text(ThreatLevel level) const;

    void paint(Painter& painter) override;

    // ✅ ОПТИМИЗАЦИЯ: Использует UnifiedColorLookup вместо дублирующего LUT
    // Все threat colours хранятся во Flash (color_lookup_unified.hpp)

    // Cached previous values for Check-Before-Update optimization
    ThreatLevel last_threat_ = ThreatLevel::NONE;
    bool last_is_scanning_ = false;
    Frequency last_freq_ = 0;
    size_t last_approaching_ = 0;
    size_t last_static_ = 0;
    size_t last_receding_ = 0;
    std::string last_text_;
};

class ThreatCard : public View {
public:
    explicit ThreatCard(size_t card_index = 0, Rect parent_rect = {0, 0, screen_width, 24});
    ~ThreatCard() = default;

    void update_card(const DisplayDroneEntry& drone);
    void clear_card();
    Color get_card_bg_color() const;
    Color get_card_text_color() const;
    void set_parent_rect(const Rect& rect);

    ThreatCard(const ThreatCard&) = delete;
    ThreatCard& operator=(const ThreatCard&) = delete;

private:
    size_t card_index_;
    Text card_text_ {{0, 2, screen_width, 20}, ""};
    Rect parent_rect_;
    bool is_active_ = false;

    // Cached previous values for Check-Before-Update optimization
    Frequency last_frequency_ = 0;
    ThreatLevel last_threat_ = ThreatLevel::NONE;
    MovementTrend last_trend_ = MovementTrend::UNKNOWN;
    int32_t last_rssi_ = -120;
    std::string last_threat_name_;

    void paint(Painter& painter) override;

    // ✅ ОПТИМИЗАЦИЯ: Использует UnifiedColorLookup вместо дублирующего LUT
    // Все threat colours хранятся во Flash (color_lookup_unified.hpp)
};

enum class DisplayMode : uint8_t { SCANNING = 0, ALERT = 1, NORMAL = 2 };

// DIAMOND OPTIMIZATION: constexpr Style LUTs для ConsoleStatusBar (хранятся во Flash)
// Scott Meyers Item 15: Prefer constexpr to #define
struct StatusStyleConfig {
    uint32_t bg_color;
    uint32_t text_color;
};

class ConsoleStatusBar : public View {
public:
    explicit ConsoleStatusBar(size_t bar_index = 0, Rect parent_rect = {0, 0, screen_width, 16});
    ~ConsoleStatusBar() = default;

    // DIAMOND OPTIMIZATION: const char* вместо const std::string& для экономии RAM
    // Scott Meyers Item 1: View C++ as a federation of languages
    void update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles = 0, uint32_t detections = 0);
    void update_alert_status(ThreatLevel threat, size_t total_drones, const char* alert_msg);
    void update_normal_status(const char* primary, const char* secondary);
    void set_display_mode(DisplayMode mode);

    ConsoleStatusBar(const ConsoleStatusBar&) = delete;
    ConsoleStatusBar& operator=(const ConsoleStatusBar&) = delete;

private:
    // DIAMOND OPTIMIZATION: constexpr массивы стилей (во Flash, ноль RAM)
    // Scott Meyers Item 15: Prefer constexpr to #define
    static constexpr StatusStyleConfig STATUS_STYLES[] = {
        {0x001F, 0xFFFF},  // Синий фон, белый текст (SCANNING)
        {0xFFFF, 0x0000}   // Белый фон, черный текст (STOPPED)
    };

    static constexpr StatusStyleConfig THREAT_STYLES[] = {
        {0xF800, 0xFFFF},  // Красный фон, белый текст (CRITICAL)
        {0xFFE0, 0x0000}   // Желтый фон, черный текст (MEDIUM)
    };

    // DIAMOND OPTIMIZATION: Unified icon LUT для всех уровней угрозы
    // Хранится во Flash, ноль RAM
    static constexpr const char* ALERT_ICONS[] = {"(i)", "[!]", "[O]", "[X]", "[!!]"};
    static_assert(sizeof(ALERT_ICONS) / sizeof(const char*) == 5, "ALERT_ICONS size");

    // DIAMOND OPTIMIZATION: Unified style LUT для alert режимов
    // Устраняет дублирование ALERT_STYLES + runtime ветвление
    struct AlertStyleEntry {
        uint32_t bg_color;
        uint32_t text_color;
    };

    static constexpr AlertStyleEntry ALERT_STYLES[] = {
        {0xF800, 0xFFFF},  // CRITICAL (RED bg, WHITE text)
        {0xFFE0, 0x0000}   // MEDIUM (YELLOW bg, BLACK text)
    };
    static_assert(sizeof(ALERT_STYLES) / sizeof(AlertStyleEntry) == 2, "ALERT_STYLES size");

    // DIAMOND OPTIMIZATION: Progress Bar LUT
    // Предвычисленные прогресс-бары для экономии CPU cycles
    // Хранится во Flash, ноль RAM
    // Исправлено: Перемещено в ConsoleStatusBar (где используется)
    struct ProgressBarEntry {
        uint8_t bars;
        const char* pattern;
    };

    static constexpr ProgressBarEntry PROGRESS_PATTERNS[] = {
        {0, "--------"},  // 0%
        {1, "=======-"},
        {2, "======--"},
        {3, "=====---"},
        {4, "====----"},
        {5, "===-----"},
        {6, "==------"},
        {7, "=-------"},
        {8, "--------"}   // 100% (fallback)
    };
    static_assert(sizeof(PROGRESS_PATTERNS) / sizeof(ProgressBarEntry) == 9, "PROGRESS_PATTERNS size");

    size_t bar_index_;
    DisplayMode mode_ = DisplayMode::NORMAL;
    Rect parent_rect_;

    Text progress_text_  {{0, 1, screen_width, 16}, ""};
    Text alert_text_     {{0, 1, screen_width, 16}, ""};
    Text normal_text_    {{0, 1, screen_width, 16}, ""};

    void paint(Painter& painter) override;
};

enum class RulerStyle : uint8_t {
    COMPACT_GHZ = 0,
    COMPACT_MHZ = 1,
    STANDARD_GHZ = 2,
    STANDARD_MHZ = 3,
    DETAILED = 4,
    SPACED_GHZ = 5
};

class CompactFrequencyRuler : public View {
public:
    explicit CompactFrequencyRuler(Rect parent_rect = {0, 0, screen_width, 12});
    ~CompactFrequencyRuler() override = default;

    void set_frequency_range(Frequency min_freq, Frequency max_freq);
    void set_spectrum_width(int width);
    void set_visible(bool visible);
    void set_ruler_style(RulerStyle style);
    void set_tick_count(int num_ticks);
    void paint(Painter& painter) override;

    CompactFrequencyRuler(const CompactFrequencyRuler&) = delete;
    CompactFrequencyRuler& operator=(const CompactFrequencyRuler&) = delete;

private:
    Frequency min_freq_;
    Frequency max_freq_;
    int spectrum_width_;
    bool visible_;
    RulerStyle ruler_style_;
    int target_tick_count_;

    static constexpr int RULER_HEIGHT = 12;
    static constexpr int TICK_HEIGHT_MAJOR = 8;
    static constexpr int TICK_HEIGHT_MINOR = 4;
    static constexpr int DEFAULT_TICK_COUNT = 4;

    void draw_compact_ticks(Painter& painter, const Rect r);
    std::string format_compact_label(Frequency freq);
    Frequency calculate_optimal_tick_interval();
    RulerStyle determine_auto_style();
    bool should_use_mhz_labels() const;
};

class FrequencyRuler : public View {
public:
    explicit FrequencyRuler(Rect parent_rect = {0, 0, screen_width, 12});
    ~FrequencyRuler() override = default;

    void set_frequency_range(Frequency min_freq, Frequency max_freq);
    void set_spectrum_width(int width);
    void set_visible(bool visible);
    void paint(Painter& painter) override;

    FrequencyRuler(const FrequencyRuler&) = delete;
    FrequencyRuler& operator=(const FrequencyRuler&) = delete;

private:
    Frequency min_freq_;
    Frequency max_freq_;
    int spectrum_width_;
    bool visible_;

    static constexpr int RULER_HEIGHT = 12;
    static constexpr int TICK_HEIGHT_MAJOR = 10;
    static constexpr int TICK_HEIGHT_MINOR = 6;

    void draw_frequency_ticks(Painter& painter, const Rect r);
    Frequency calculate_tick_interval();
    // DIAMOND OPTIMIZATION: Changed signature to use tick_interval instead of unit string
    // Eliminates std::string allocation, uses LUT lookup instead
    std::string format_frequency_label(Frequency freq, Frequency tick_interval);
};

class DroneDisplayController : public View {
public:
    // 🔴 OPTIMIZATION: Static buffer sizes to prevent heap allocation in constructor
    static constexpr size_t SPECTRUM_ROW_SIZE = 240;
    static constexpr size_t RENDER_LINE_SIZE = 240;
    static constexpr size_t WATERFALL_SIZE = 40 * 240;  // 9.6KB waterfall buffer
    
    explicit DroneDisplayController(Rect parent_rect = {0, 60, screen_width, screen_height - 80});
    ~DroneDisplayController();

    Text& big_display() { return big_display_; }
    ProgressBar& scanning_progress() { return scanning_progress_; }
    Text& text_threat_summary() { return text_threat_summary_; }
    Text& text_status_info() { return text_status_info_; }
    Text& text_scanner_stats() { return text_scanner_stats_; }
    Text& text_trends_compact() { return text_trends_compact_; }
    Text& text_drone_1() { return text_drone_1_; }
    Text& text_drone_2() { return text_drone_2_; }
    Text& text_drone_3() { return text_drone_3_; }

    // DIAMOND OPTIMIZATION: прямой доступ к массиву вместо switch (строки 2486-2491)
    static constexpr size_t NUM_DRONE_TEXT_WIDGETS = 3;

    static constexpr size_t MAX_UI_DRONES = 16;

    std::array<DisplayDroneEntry, MAX_UI_DRONES>& detected_drones() {
        return *detected_drones_ptr_;
    }

    void update_detection_display(const DroneScanner& scanner);
    void update_trends_display(size_t approaching, size_t static_count, size_t receding);
    // DIAMOND OPTIMIZATION: const char* вместо const std::string& для экономии RAM
    void set_scanning_status(bool active, const char* message);
    void set_frequency_display(Frequency freq);
    void add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi);
    void update_drones_display(const DroneScanner& scanner);
    void sort_drones_by_rssi();
    void render_drone_text_display();

    void initialize_mini_spectrum();
    void process_mini_spectrum_data(const ChannelSpectrum& spectrum);
    bool process_bins(uint8_t* power_level);

    void render_bar_spectrum(Painter& painter);
    void highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, DroneConstants::MAX_DISPLAYED_DRONES>& drones);
    size_t frequency_to_spectrum_bin(Frequency freq_hz) const;
    void clear_spectrum_buffers();
    bool validate_spectrum_data() const;
    size_t get_safe_spectrum_index(size_t x, size_t y) const;

    void set_spectrum_range(Frequency min_freq, Frequency max_freq);
    // DIAMOND OPTIMIZATION: const char* вместо const std::string& для экономии RAM
    void update_signal_type_display(const char* signal_type);
    void update_frequency_ruler();
    void lazy_initialize_gradient();  // CRITICAL FIX: Prevent blocking I/O on startup
    void set_ruler_style(RulerStyle style);
    void apply_display_settings(const DroneAnalyzerSettings& settings);
    CompactFrequencyRuler& compact_frequency_ruler() { return compact_frequency_ruler_; }
    FrequencyRuler& frequency_ruler() { return frequency_ruler_; }

    // 🔴 FIX: Buffer allocation/deallocation (deferred from constructor)
    void allocate_buffers();
    void deallocate_buffers();
    
    // 🔴 DIAMOND OPTIMIZATION: Buffer validation methods (защита от UB)
    bool are_buffers_allocated() const;
    bool are_buffers_valid() const;
    bool allocate_buffers_from_pool();

    // 🔴 FIX: Safe buffer accessors (без waterfall_buffer - не используется)
    std::array<Color, SPECTRUM_ROW_SIZE>& spectrum_row_buffer() { return *spectrum_row_buffer_ptr_; }
    const std::array<Color, SPECTRUM_ROW_SIZE>& spectrum_row_buffer() const { return *spectrum_row_buffer_ptr_; }
    std::array<Color, RENDER_LINE_SIZE>& render_line_buffer() { return *render_line_buffer_ptr_; }
    const std::array<Color, RENDER_LINE_SIZE>& render_line_buffer() const { return *render_line_buffer_ptr_; }
    std::array<uint8_t, 200>& spectrum_power_levels() { return *spectrum_power_levels_ptr_; }
    const std::array<uint8_t, 200>& spectrum_power_levels() const { return *spectrum_power_levels_ptr_; }

    // 🔴 FIX: Public methods for parent View message delegation
    void set_spectrum_fifo(ChannelSpectrumFIFO* fifo) {
        spectrum_fifo_ = fifo;
    }

    // DIAMOND OPTIMIZATION: constexpr LUT в Flash вместо switch/if-else (экономия ROM)
    static constexpr RulerStyle RULER_STYLE_LUT[] = {
        RulerStyle::COMPACT_GHZ,  // 0
        RulerStyle::COMPACT_GHZ,  // 1
        RulerStyle::COMPACT_MHZ,  // 2
        RulerStyle::STANDARD_GHZ, // 3
        RulerStyle::STANDARD_MHZ, // 4
        RulerStyle::DETAILED,     // 5
        RulerStyle::SPACED_GHZ    // 6
    };

    // DIAMOND OPTIMIZATION: enum class для типов сигналов (вместо string comparison)
    enum class SignalType : uint8_t {
        DEFAULT = 0,
        DIGITAL = 1,
        ANALOG = 2,
        NOISE = 3
    };

    // DIAMOND OPTIMIZATION: constexpr LUT для типов сигналов (без if-else)
    static constexpr struct SignalTypeConfig {
        const char* name;
        Color color;
    } SIGNAL_TYPE_CONFIG[] = {
        {"--",       Color::white()},           // DEFAULT/Unknown
        {"DIGITAL",  Color::green()},          // Digital (Green)
        {"ANALOG",   Color::yellow()},         // Analog (Yellow)
        {"NOISE",    Color::grey()}           // Noise (Grey)
    };
    static_assert(sizeof(SIGNAL_TYPE_CONFIG) == sizeof(SignalTypeConfig) * 4, "SIGNAL_TYPE_CONFIG size");

    // DIAMOND OPTIMизация: constexpr LUT для цветов big_display (без каскадного if-else)
    static constexpr const Color BIG_DISPLAY_COLORS[] = {
        Color::dark_grey(),      // Idle/Default
        Color::green(),          // Scanning (Green)
        Color::orange(),         // Has detections (Orange)
        Color::yellow(),         // Medium threat (Yellow)
        Color::red()            // High+ threat (Red)
    };

    // DIAMOND OPTIMIZATION: конфигурация для bar spectrum (заменяет waterfall)
    struct BarSpectrumConfig {
        static constexpr int WATERFALL_Y_START = 81;
        static constexpr int BAR_HEIGHT_MAX = DroneConstants::MINI_SPECTRUM_HEIGHT;
        static constexpr uint8_t NOISE_THRESHOLD = 10;
        static constexpr uint8_t PEAK_SHARPNESS_THRESHOLD = 15; // Баланс: 15 единиц

        // Цвета для bar spectrum (в Flash)
        static constexpr Color BAR_COLORS[] = {
            Color::blue(),          // 0: Wideband/Plateau (WiFi, Video) - Blue
            Color::red(),           // 1: Sharp Peak (Drone Control) - Red
            Color::grey()           // 2: Unknown/Noise - Grey
        };
    };

    void process_frame_sync();

    DroneDisplayController(const DroneDisplayController&) = delete;
    DroneDisplayController& operator=(const DroneDisplayController&) = delete;

    static constexpr const char* DRONE_DISPLAY_FORMAT = "%s %s %-4lddB %c";

    // DIAMOND OPTIMIZATION: constexpr Style для big_display (вместо локальных массивов)
    static constexpr Style BIG_DISPLAY_STYLES[] = {
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[0]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[1]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[2]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[3]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[4]}
    };

    // ===========================================
    // DIAMOND OPTIMIZATION: Frequency Formatting LUT
    // ===========================================
    // Scott Meyers Item 15: Prefer constexpr to #define
    // Устраняет каскадный if/else в render_drone_text_display()
    // Хранится во Flash, ноль RAM, O(1) lookup
    struct FrequencyFormatEntry {
        int64_t min_freq;
        const char* format;
        int64_t divider;       // Делитель для целой части
        int64_t decimal_div;  // Делитель для дробной части
    };

    static constexpr FrequencyFormatEntry FREQ_FORMAT_TABLE[] = {
        {1000000000LL, "%lu.%luG", 1000000000LL, 100000000LL},  // GHz range
        {1000000LL, "%luM", 1000000LL, 1},                       // MHz range
        {1000LL, "%luk", 1000LL, 1},                             // kHz range
        {0LL, "%lu", 1LL, 1}                                      // Hz fallback
    };
    static_assert(sizeof(FREQ_FORMAT_TABLE) / sizeof(FrequencyFormatEntry) == 4, "FREQ_FORMAT_TABLE size");

    struct SpectrumConfig {
        Frequency min_freq = 2400000000LL;
        Frequency max_freq = 2500000000LL;
        uint32_t bandwidth = 24000000;
        uint32_t sampling_rate = 24000000;
    };

private:
    Text big_display_{{4, 0, 28 * 8, 52}, ""};
    ProgressBar scanning_progress_{{0, 52, screen_width, 8}};
    Text text_threat_summary_{{0, 82, screen_width, 16}, "THREAT: NONE"};
    Text text_status_info_{{0, 98, screen_width, 16}, "Ready"};
    Text text_scanner_stats_{{0, 114, screen_width, 16}, "No database"};
    Text text_trends_compact_{{0, 130, screen_width, 16}, ""};
    Text text_drone_1_{{screen_width - 120, 146, 120, 16}, ""};
    Text text_drone_2_{{screen_width - 120, 162, 120, 16}, ""};
    Text text_drone_3_{{screen_width - 120, 178, 120, 16}, ""};
    Text text_signal_type_{{screen_width - 80, 80, 80, 16}, "SIGNAL: --"};  // Debug: Signal type marker

    // DIAMOND OPTIMизация: хелпер для индексного доступа к виджетам (без switch)
    Text* drone_text_widget(size_t index) {
        static Text* const widgets[] = {&text_drone_1_, &text_drone_2_, &text_drone_3_};
        return (index < NUM_DRONE_TEXT_WIDGETS) ? widgets[index] : nullptr;
    }

    CompactFrequencyRuler compact_frequency_ruler_{{0, 68, screen_width, 12}};
    FrequencyRuler frequency_ruler_{{0, 68, screen_width, 12}};

    std::unique_ptr<std::array<DisplayDroneEntry, MAX_UI_DRONES>> detected_drones_ptr_;
    size_t detected_drones_count_ = 0;
    std::array<DisplayDroneEntry, DroneConstants::MAX_DISPLAYED_DRONES> displayed_drones_;

    // 🔴 FIX: Heap-allocated buffers (waterfall_buffer удалён - не нужен, экономия ~9.6KB RAM)
    std::unique_ptr<std::array<Color, SPECTRUM_ROW_SIZE>> spectrum_row_buffer_ptr_;
    std::unique_ptr<std::array<Color, RENDER_LINE_SIZE>> render_line_buffer_ptr_;
    std::unique_ptr<std::array<uint8_t, 200>> spectrum_power_levels_ptr_;
    struct ThreatBin { size_t bin; ThreatLevel threat; };
    std::array<ThreatBin, DroneConstants::MAX_DISPLAYED_DRONES> threat_bins_;
    size_t threat_bins_count_ = 0;
    size_t waterfall_line_index_ = 0;

    Gradient spectrum_gradient_;
    ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
    size_t pixel_index = 0;
    uint32_t bins_hz_size = 0;
    uint32_t each_bin_size = 100000;
    uint8_t min_color_power = 0;
    const uint8_t ignore_dc = 4;
    uint32_t marker_pixel_step = 1000000;
    uint8_t max_power = 0;
    uint8_t range_max_power = 0;
    int mode = 0;

    SpectrumConfig spectrum_config_;

    const char* get_drone_type_name(DroneType type) const;
    Color get_drone_type_color(DroneType type) const;
    Color get_threat_level_color(ThreatLevel level) const;
    const char* get_threat_level_name(ThreatLevel level) const;

    void get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power);
    void add_spectrum_pixel(uint8_t power);

    // DIAMOND OPTIMIZATION: анализ формы сигнала для bar spectrum (без SpectralAnalyzer)
    inline size_t get_bar_color_index(size_t x, uint8_t power) const {
        if (!SafeBufferAccess<uint8_t, 200>::is_valid(spectrum_power_levels_ptr_)) {
            return 2; // Unknown/Noise (Grey)
        }

        // Острый пик (narrowband drone control): текущее > соседей на 15+
        // Широкий сигнал (wideband video/WiFi): все примерно равны
        const auto& levels = spectrum_power_levels();
        if (x > 0 && x < levels.size() - 1) {
            uint8_t prev = levels[x - 1];
            uint8_t next = levels[x + 1];

            // Логика "Острого пика": текущий ВЫШЕ соседей на 15+ единиц
            if (power > prev + BarSpectrumConfig::PEAK_SHARPNESS_THRESHOLD &&
                power > next + BarSpectrumConfig::PEAK_SHARPNESS_THRESHOLD) {
                return 1; // Sharp Peak (Red)
            }
        }

        // Широкий сигнал: Blue
        return 0; // Wideband (Blue)
    }

    void handle_channel_spectrum(const ChannelSpectrum& spectrum);
    void analyze_spectrum_for_threats(const ChannelSpectrum& spectrum);
    Frequency spectrum_bin_to_frequency(size_t bin) const;
    void update_or_create_drone_from_spectrum(Frequency freq_hz, uint8_t power);
};

static constexpr const char* DEFAULT_CONFIG_PATH = "DRONES/DATA.CFG";
static constexpr const char* FALLBACK_CONFIG_PATH = "APP/SETTINGS/DRONES.CFG";
static constexpr uint32_t DEFAULT_BANDWIDTH = 24000000UL;
static constexpr Frequency DEFAULT_CENTER_FREQUENCY = 2400000000LL;
static constexpr const char* DEFAULT_SPECTRUM_FILE = "DEFAULT";

class DroneUIController {
public:
    DroneUIController(NavigationView& nav,
                     DroneHardwareController& hardware,
                     DroneScanner& scanner,
                     ::AudioManager& audio_mgr,
                     DroneDisplayController& display_controller);
    ~DroneUIController();

    void on_start_scan();
    void on_stop_scan();
    void on_toggle_mode();
    void show_menu();
    void on_load_frequency_file();
    void on_save_frequency();
    void on_toggle_audio_simple();
    void on_audio_toggle();
    void on_advanced_settings();
    void on_open_settings();
    void on_open_constant_settings();
    void on_select_language();
    void on_about();
    void on_audio_settings();
    void on_hardware_control();
    void on_view_logs();
    void update_scanner_range(Frequency min_freq, Frequency max_freq);

    bool is_scanning() const { return scanning_active_; }
    DroneAnalyzerSettings& settings() { return settings_; }
    const DroneAnalyzerSettings& settings() const { return settings_; }
    DroneAnalyzerSettings& settings_ref() { return settings_; }
    const DroneAnalyzerSettings& settings_ref() const { return settings_; }

    DroneUIController(const DroneUIController&) = delete;
    DroneUIController& operator=(const DroneUIController&) = delete;

private:
    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    ::AudioManager& audio_mgr_;
    std::atomic<bool> scanning_active_{false};
    DroneDisplayController* display_controller_ = nullptr;
    ::ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings settings_;

    void on_manage_frequencies();
    void on_create_new_database();
    void on_frequency_warning();
    void show_system_status();
    void show_performance_stats();
    void show_debug_info();
    void select_spectrum_mode(SpectrumMode mode);
    void on_spectrum_range_config();
    void on_add_preset_quick();
    void show_current_bandwidth();
    void show_current_center_freq();
    void on_set_bandwidth_config();
    void on_set_center_freq_config();
    void set_bandwidth_from_menu(uint32_t bandwidth_hz);
    void set_center_freq_from_menu(Frequency center_freq);
    void add_preset_to_scanner(const DronePreset& preset);
    void on_save_settings();
    void on_load_settings();
    void set_spectrum_mode(SpectrumMode mode);
    void on_spectrum_mode();
    void on_set_bandwidth();
    void on_set_center_freq();
    void show_hardware_status();
};

class EnhancedDroneSpectrumAnalyzerView : public View {
public:
    explicit EnhancedDroneSpectrumAnalyzerView(NavigationView& nav);
    ~EnhancedDroneSpectrumAnalyzerView() override;

    EnhancedDroneSpectrumAnalyzerView(const EnhancedDroneSpectrumAnalyzerView&) = delete;
    EnhancedDroneSpectrumAnalyzerView& operator=(const EnhancedDroneSpectrumAnalyzerView&) = delete;

    void focus() override;
    std::string title() const override { return "EDA"; };

    void paint(Painter& painter) override;
    bool on_key(const KeyEvent key) override;
    bool on_touch(const TouchEvent event) override;
    void on_show() override;
    void on_hide() override;

    // ===========================================
    // INITIALIZATION STATE
    // ===========================================
    // 🔴 ФАЗА 2.1: Deferred initialization state machine
    enum class InitState : uint8_t {
        CONSTRUCTED = 0,           // Constructor completed
        BUFFERS_ALLOCATED,         // display buffers heap-allocated
        DATABASE_LOADED,           // scanner_.initialize_database_and_scanner() completed
        HARDWARE_READY,            // hardware_.on_hardware_show() completed
        UI_LAYOUT_READY,           // initialize_modern_layout() completed
        SETTINGS_LOADED,           // SettingsPersistence::load() completed
         COORDINATOR_READY,         // coordinator thread creation prepared
         FULLY_INITIALIZED = 7,     // Ready for operation
         INITIALIZATION_ERROR = 8   // Timeout or critical error (NEW!)
     };

 private:
    // 🔴 DIAMOND OPTIMIZATION: Forward declarations for phase initialization methods
    // Scott Meyers Item 11: Handle assignment to self in operator=
    // These methods declared here for use in constexpr LUT
    void init_phase_allocate_buffers();
    void init_phase_load_database();
    void init_phase_init_hardware();
    void init_phase_setup_ui();
    void init_phase_load_settings();
    void init_phase_finalize();

    // ===========================================
    // INITIALIZATION TIMING CONSTANTS
    // ===========================================
    // Scott Meyers Item 15: Prefer constexpr to #define
    // Eliminates magic numbers, improves maintainability
    struct InitTiming {
        static constexpr uint32_t TIMEOUT_MS = 5000;           // 5 seconds for total initialization
        static constexpr uint32_t PHASE_INTERVAL_MS = 50;       // Base interval between phases
        static constexpr uint32_t PHASE_DELAY_1_MS = 50;      // Phase 1: Buffer allocation
        static constexpr uint32_t PHASE_DELAY_2_MS = 100;     // Phase 2: Database loading
        static constexpr uint32_t PHASE_DELAY_3_MS = 200;     // Phase 3: Hardware init
        static constexpr uint32_t PHASE_DELAY_4_MS = 300;     // Phase 4: UI setup
        static constexpr uint32_t PHASE_DELAY_5_MS = 400;     // Phase 5: Settings loading
        static constexpr uint32_t PHASE_DELAY_6_MS = 500;     // Phase 6: Finalization
        static constexpr uint8_t MAX_PHASES = 6;
    };

    // DIAMOND OPTIMIZATION: constexpr LUT для сообщений инициализации в Flash
    static constexpr const char* const INIT_STATUS_MESSAGES[] = {
        "Starting up...",      // CONSTRUCTED = 0
        "Buffers ready",       // BUFFERS_ALLOCATED = 1
        "Database ready",      // DATABASE_LOADED = 2
        "Hardware ready",      // HARDWARE_READY = 3
        "UI ready",            // UI_LAYOUT_READY = 4
        "Settings loaded",     // SETTINGS_LOADED = 5 (NEW!)
        "Coordinator ready",   // COORDINATOR_READY = 6
        "All systems go"       // FULLY_INITIALIZED = 7
    };
    static constexpr const char* const INIT_STATUS_TITLES[] = {
        "INIT",               // CONSTRUCTED through COORDINATOR_READY (0-6)
        "EDA Ready"           // FULLY_INITIALIZED = 7
    };

    // 🔴 DIAMOND OPTIMIZATION: constexpr LUT для фаз инициализации (хранится во Flash)
    // Scott Meyers Item 15: Prefer constexpr to #define
    // Phase dependencies documented:
    //   Phase 0 (Allocate): No dependencies
    //   Phase 1 (Database):  Requires Phase 0 (buffers)
    //   Phase 2 (Hardware):  Requires Phase 1 (database)
    //   Phase 3 (UI Setup):   Requires Phase 2 (hardware)
    //   Phase 4 (Settings):   Requires Phase 3 (UI layout)
    //   Phase 5 (Finalize):   Requires Phase 4 (settings)
    struct InitPhaseConfig {
        const char* const name;           // Строка во Flash (const char*)
        uint32_t delay_ms;               // Задержка в мс
        void (EnhancedDroneSpectrumAnalyzerView::*init_func)();  // Сырой указатель на метод
    };

    static constexpr InitPhaseConfig INIT_PHASES[] = {
        {"Allocating buffers...",   InitTiming::PHASE_DELAY_1_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_allocate_buffers},
        {"Loading database...",     InitTiming::PHASE_DELAY_2_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_load_database},
        {"Initializing hardware...", InitTiming::PHASE_DELAY_3_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_init_hardware},
        {"Setting up UI...",        InitTiming::PHASE_DELAY_4_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_setup_ui},
        {"Loading settings...",     InitTiming::PHASE_DELAY_5_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_load_settings},
        {"Finalizing...",           InitTiming::PHASE_DELAY_6_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_finalize}
    };

    // 🔴 DIAMOND OPTIMIZATION: constexpr массив для сообщений об ошибках (Flash)
    static constexpr const char* const ERROR_MESSAGES[] = {
        "Init timeout",      // GENERAL_TIMEOUT = 0
        "Allocation failed",  // ALLOCATION_FAILED = 1
        "Database error"      // DATABASE_ERROR = 2
    };
    static_assert(sizeof(ERROR_MESSAGES) / sizeof(const char*) == 3, "ERROR_MESSAGES size");

 private:
    NavigationView& nav_;

    // ===========================================
    // MEMBER DECLARATION ORDER (CRITICAL)
    // ===========================================
    // Scott Meyers Effective C++ Item 12: Never change the order of member initialization
    // C++ initialization order: members are initialized in DECLARATION order, NOT init-list order
    //
    // Dependencies:
    //   settings_               - No dependencies (declared first)
    //   hardware_               - No dependencies
    //   scanner_                - Depends on settings_
    //   audio_                  - No dependencies
    //   display_controller_      - No dependencies
    //   ui_controller_           - Depends on hardware_, scanner_, audio_, display_controller_
    //   scanning_coordinator_   - Depends on hardware_, scanner_, display_controller_, audio_
    //   smart_header_            - No dependencies
    //   status_bar_              - No dependencies
    //   threat_cards_            - No dependencies
    //   button_*                 - No dependencies (initialized in constructor)
    //   field_scanning_mode_     - No dependencies (initialized in constructor)
    //   scanning_active_         - No dependencies (initialized last)
    // ===========================================

    ::ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings settings_;

    // Stack-allocated objects following Mayhem pattern
    DroneHardwareController hardware_;
    DroneScanner scanner_;
    ::AudioManager audio_;

    DroneDisplayController display_controller_;
    DroneUIController ui_controller_;
    ScanningCoordinator scanning_coordinator_;

    SmartThreatHeader smart_header_;
    ConsoleStatusBar status_bar_;
    std::array<ThreatCard, 3> threat_cards_;

    Button button_start_stop_;
    Button button_menu_;
    Button button_audio_;

    OptionsField field_scanning_mode_;

    bool scanning_active_ = false;

    // 🔴 FIX: Unified message handlers to prevent MsgDblReg
    // These handlers delegate to controllers and internal methods
    
    MessageHandlerRegistration message_handler_spectrum_config_ {
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            // Delegate FIFO pointer to both controllers that need it
            hardware_.set_spectrum_fifo(message.fifo);
            display_controller_.set_spectrum_fifo(message.fifo);
        }
    };

    MessageHandlerRegistration message_handler_frame_sync_ {
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            // 🔴 DIAMOND OPTIMIZATION: Enhanced initialization with timeout protection
            // Executed every frame (~16ms at 60 FPS)
            if (init_state_ == InitState::INITIALIZATION_ERROR) {
                // Не обновляем UI, ошибка уже показана
                return;
            }
            
            if (init_state_ != InitState::FULLY_INITIALIZED) {
                step_deferred_initialization();
                // Return early if not fully initialized
                return;
            }

            // 🔴 SAFETY: Проверка валидности буферов перед операциями
            if (!display_controller_.are_buffers_valid()) {
                return;
            }

            // Normal operation mode
            // 1. Process spectrum data in display controller
            display_controller_.process_frame_sync();

            // 2. Handle scanner updates in main view
            this->handle_scanner_update();
        }
    };

    void start_scanning_thread();
    void stop_scanning_thread();
    bool handle_start_stop_button();
    bool handle_menu_button();
    void initialize_modern_layout();
    void update_modern_layout();
    void handle_scanner_update();
    void setup_button_handlers();
    void initialize_scanning_mode();
    void set_scanning_mode_from_index(size_t index);
    void add_ui_elements();

    // 🔴 ФАЗА 2.2: Deferred initialization methods
    void step_deferred_initialization();
    void update_init_progress_display();

    // 🔴 ФАЗА 2.3: Initialization state variables
    InitState init_state_ = InitState::CONSTRUCTED;
    systime_t init_start_time_ = 0;
    systime_t last_init_progress_ = 0;
    bool initialization_in_progress_ = false;
    
    // 🔴 DIAMOND OPTIMIZATION: Error handling enum
    enum class InitError : uint8_t {
        NONE = 0,
        GENERAL_TIMEOUT = 1,
        ALLOCATION_FAILED = 2,
        DATABASE_ERROR = 3
    };
    InitError init_error_ = InitError::NONE;
};


class LoadingScreenView : public View {
public:
    explicit LoadingScreenView(NavigationView& nav);
    ~LoadingScreenView() = default;

    void paint(Painter& painter) override;

private:
    NavigationView& nav_;
    Text text_eda_{Rect{108, 213, 24, 16}, "EDA"};
    systime_t timer_start_ = 0;
};

// --- НОВЫЙ КЛАСС ДЛЯ НАСТРОЙКИ ДИАПАЗОНА ---
class FrequencyRangeSetupView : public View {
public:
    FrequencyRangeSetupView(NavigationView& nav, DroneUIController& controller);
    
    void focus() override;

private:
    NavigationView& nav_;
    DroneUIController& controller_;
    
    // Виджеты интерфейса
    Text text_title_ { { 4, 4, 224, 16 }, "Panoramic Spectrum" };
    
    Text label_min_ { { 4, 36, 80, 16 }, "Start Freq:" };
    TextField field_min_ { { 88, 36, 160, 16 }, "2400.000000" };
    
    Text label_max_ { { 4, 68, 80, 16 }, "End Freq:" };
    TextField field_max_ { { 88, 68, 160, 16 }, "2500.000000" };
    
    Text label_slice_ { { 4, 100, 80, 16 }, "Res (BW):" };
    Text field_slice_ { { 88, 100, 160, 16 }, "24 MHz" };

    Button button_save_ { { 4, 140, 224, 40 }, "Apply Range" };
    Button button_cancel_ { { 4, 190, 224, 40 }, "Cancel" };

    void on_save();
    void on_cancel();
};

// Forward declaration of integrated settings view
class DroneAnalyzerSettingsView;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SCANNER_COMBINED_HPP_
