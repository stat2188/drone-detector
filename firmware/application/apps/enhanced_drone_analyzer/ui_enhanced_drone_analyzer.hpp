#ifndef UI_ENHANCED_DRONE_ANALYZER_HPP_
#define UI_ENHANCED_DRONE_ANALYZER_HPP_

#include <cstdint>
#include <array>
#include <memory>
#include <cstdio>

#include "ui_drone_common_types.hpp"
#include "ui_signal_processing.hpp"
#include "settings_persistence.hpp"
#include "scanning_coordinator.hpp"
#include "gradient.hpp"
#include "ui_drone_audio.hpp"
#include "ui_spectral_analyzer.hpp"
#include "eda_constants.hpp"
#include "diamond_fixes.hpp"
#include "eda_optimized_utils.hpp"
#include "color_lookup_unified.hpp"
#include "eda_locking.hpp"
#include "eda_safecast.hpp"
#include "eda_raii.hpp"

#include "ui.hpp"
#include "ui_menu.hpp"
#include "ui_enhanced_drone_settings.hpp"
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

#include "ui_navigation.hpp"
#include "app_settings.hpp"
#include "string_format.hpp"
#include "tone_key.hpp"
#include "message_queue.hpp"

class LogFile;

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// Lock Order: Always acquire locks in ascending order (1 → 2 → 3 → 4 → 5 → 6 → 7)
// Never acquire a lower-numbered lock while holding a higher-numbered lock

// Explicit thread stack sizes
constexpr size_t SCANNING_THREAD_STACK_SIZE = 2048;  // 2KB
constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 2048;  // 2KB

// Stack monitoring constants
constexpr uint32_t STACK_CANARY_VALUE = 0xDEADBEEF;  // Canary value for stack overflow detection
constexpr size_t MIN_STACK_FREE_THRESHOLD = 512;     // Minimum safe stack free bytes

// Global SD Card Mutex Protection (FatFS is NOT thread-safe)

extern Mutex sd_card_mutex;

// OrderedScopedLock enforces lock order to prevent deadlock violations

struct preset_entry {
    Frequency min = 0;
    Frequency max = 0;
    char label[32];
};

struct RssiMeasurement {
    int16_t rssi_db;
    systime_t timestamp_ms;
};


namespace UIStyles {
    EDA_FLASH_CONST inline static constexpr Style RED_STYLE{font::fixed_8x16, Color::black(), Color::red()};
    EDA_FLASH_CONST inline static constexpr Style YELLOW_STYLE{font::fixed_8x16, Color::black(), Color::yellow()};
    EDA_FLASH_CONST inline static constexpr Style GREEN_STYLE{font::fixed_8x16, Color::black(), Color::green()};
    EDA_FLASH_CONST inline static constexpr Style LIGHT_STYLE{font::fixed_8x16, Color::black(), Color::white()};
    EDA_FLASH_CONST inline static constexpr Style DARK_STYLE{font::fixed_8x16, Color::black(), Color::dark_grey()};
    EDA_FLASH_CONST inline static constexpr Style ORANGE_STYLE{font::fixed_8x16, Color::black(), Color::orange()};
}

class TrackedDrone {
public:
    TrackedDrone() : frequency(0), drone_type(static_cast<uint8_t>(DroneType::UNKNOWN)),
                     threat_level(static_cast<uint8_t>(ThreatLevel::NONE)), update_count(0),
                     last_seen(0), rssi(EDA::Constants::RSSI_SILENCE_DBM), rssi_history_{}, timestamp_history_{}, history_index_(0) {
        std::fill(std::begin(rssi_history_), std::end(rssi_history_), EDA::Constants::RSSI_SILENCE_DBM);
        std::fill(std::begin(timestamp_history_), std::end(timestamp_history_), 0);
    }

    // Explicit copy constructor to avoid deprecated implicit copy constructor warning
    TrackedDrone(const TrackedDrone& other) : frequency(other.frequency),
                     drone_type(other.drone_type),
                     threat_level(other.threat_level),
                     update_count(other.update_count),
                     last_seen(other.last_seen),
                     rssi(other.rssi),
                     rssi_history_{},
                     timestamp_history_{},
                     history_index_(other.history_index_) {
        for(size_t i=0; i<MAX_HISTORY; i++) {
            rssi_history_[i] = other.rssi_history_[i];
            timestamp_history_[i] = other.timestamp_history_[i];
        }
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

    // inline enables compiler optimization, noexcept avoids exception handling overhead
    inline void add_rssi(const RssiMeasurement& measurement) noexcept {
        rssi_history_[history_index_] = measurement.rssi_db;
        timestamp_history_[history_index_] = measurement.timestamp_ms;
        history_index_ = (history_index_ + 1) % MAX_HISTORY;

        this->rssi = measurement.rssi_db;

        if (measurement.timestamp_ms > last_seen) {
            last_seen = measurement.timestamp_ms;
            if (update_count < 255) update_count++;
        }
    }

    // inline + noexcept + loop unrolling for MAX_HISTORY=8
    inline MovementTrend get_trend() const noexcept {
        if (update_count < EDA::Constants::MOVEMENT_TREND_MIN_HISTORY) return MovementTrend::UNKNOWN;

        // Compile-time constants for better optimization
        constexpr int32_t SILENCE_THRESHOLD = EDA::Constants::MOVEMENT_TREND_SILENCE_THRESHOLD;
        constexpr int32_t APPROACHING_THRESHOLD = EDA::Constants::MOVEMENT_TREND_THRESHOLD_APPROACHING;
        constexpr int32_t RECEEDING_THRESHOLD = EDA::Constants::MOVEMENT_TREND_THRESHOLD_RECEEDING;
        constexpr size_t HALF_WINDOW = MAX_HISTORY / 2;  // 4

        int32_t recent_sum = 0;
        int32_t older_sum = 0;
        size_t older_count = 0;
        size_t recent_count = 0;

        // DIAMOND OPTIMIZATION: Loop unrolling for MAX_HISTORY=8
        // Eliminates loop overhead, enables better register allocation
        for (size_t i = 0; i < MAX_HISTORY; i++) {
            size_t logical_idx = (history_index_ + i) % MAX_HISTORY;
            int16_t val = rssi_history_[logical_idx];
            if (val <= SILENCE_THRESHOLD) continue;

            if (i < HALF_WINDOW) {
                older_sum += val;
                older_count++;
            } else {
                recent_sum += val;
                recent_count++;
            }
        }

        // Avoid division by zero
        if (older_count == 0 || recent_count == 0) return MovementTrend::STATIC;

        int32_t avg_old = older_sum / static_cast<int32_t>(older_count);
        int32_t avg_new = recent_sum / static_cast<int32_t>(recent_count);
        int32_t diff = avg_new - avg_old;

        if (diff > APPROACHING_THRESHOLD) return MovementTrend::APPROACHING;
        if (diff < RECEEDING_THRESHOLD) return MovementTrend::RECEDING;
        return MovementTrend::STATIC;
    }

    // DIAMOND FIX: Priority 1 - Type Ambiguity
    // Changed from uint32_t to Frequency (uint64_t) for consistency
    // Eliminates signed/unsigned comparison overflows and data truncation
    // 🔴 FIX #L8: Remove redundant in-class default initialization
    // Diamond Code: All members are initialized in constructor, no need for defaults
    Frequency frequency;
    uint8_t drone_type;
    uint8_t threat_level;
    uint8_t update_count;
    systime_t last_seen;
    int32_t rssi;

private:
    // Reduced from 8 to 4 entries (~96 bytes savings)
    inline static constexpr size_t MAX_HISTORY = 4;
    int16_t rssi_history_[MAX_HISTORY];
    systime_t timestamp_history_[MAX_HISTORY];
    size_t history_index_;
};

struct DisplayDroneEntry {
    Frequency frequency = 0;
    DroneType type = DroneType::UNKNOWN;
    ThreatLevel threat = ThreatLevel::NONE;
    int32_t rssi = -120;
    systime_t last_seen = 0;
    char type_name[16];
    Color display_color = Color::white();
    MovementTrend trend = MovementTrend::UNKNOWN;
};


// Local constants for DroneDisplayController
inline static constexpr int SPEC_WIDTH = DiamondFixes::SpectrumConstants::SPEC_WIDTH;  // EDA::Constants::SPECTRUM_BIN_COUNT_240
inline static constexpr int SPEC_HEIGHT = DiamondFixes::SpectrumConstants::SPEC_HEIGHT;  // EDA::Constants::MINI_SPECTRUM_HEIGHT (but 40 used here)

struct WidebandSlice {
    Frequency center_frequency;
    size_t index;
};

struct WidebandScanData {
    Frequency min_freq;
    Frequency max_freq;
    size_t slices_nb;
    // Reduced from 20 to 10 slices (~200 bytes savings)
    WidebandSlice slices[10];
    size_t slice_counter;

    void reset() {
        min_freq = EDA::Constants::WIDEBAND_DEFAULT_MIN;
        max_freq = EDA::Constants::WIDEBAND_DEFAULT_MAX;
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

// Removed DroneUpdateMessage struct - unused dead code

struct DroneSignal {
    Frequency frequency_hz;
    int32_t rssi_db;
};

// Enhanced Settings Validation with detailed checks
class EnhancedDroneSettingsValidator {
public:
struct ValidationResult {
    bool is_valid;
    uint32_t warning_count;
    char error_message[128];

    ValidationResult() : is_valid(true), warning_count(0) { error_message[0] = '\0'; }
};
    
    static ValidationResult validate_all(const DroneAnalyzerSettings& settings);
    
private:
    static bool validate_frequency(Frequency freq, char* error, size_t error_size);
    static bool validate_rssi_threshold(int32_t rssi, char* error, size_t error_size);
    static bool validate_scan_interval(uint32_t interval_ms, char* error, size_t error_size);
    static bool validate_audio_params(uint32_t freq_hz, uint32_t duration_ms, char* error, size_t error_size);
    static bool validate_bandwidth(uint32_t bandwidth_hz, char* error, size_t error_size);
    static bool validate_frequency_range(Frequency min_hz, Frequency max_hz, char* error, size_t error_size);
    
    // Frequency validation with drone band checks
    static bool is_known_drone_band(Frequency freq);
    static bool is_ism_band(Frequency freq);
    static void format_frequency_hz(Frequency freq, char* buffer, size_t buffer_size);
};

class DroneDetectionLogger {
public:
    DroneDetectionLogger();
    ~DroneDetectionLogger();

    // Producer method - called by scanner thread
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
    uint32_t get_overflow_count() const { return overflow_count_; }

private:
    // Threading primitives
    Thread* worker_thread_ = nullptr;
    mutable Mutex mutex_;
    Semaphore data_ready_;
    // volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
    volatile bool worker_should_run_{false};

    // Worker Thread Stack: 5120 bytes (5KB)
    // Stack Usage Breakdown:
    // - CSV line formatting: ~128 bytes (line_buffer_)
    // - File operations (fopen/fwrite): ~200 bytes
    // - Log entry processing: ~64 bytes (DetectionLogEntry)
    // - SDCardLock mutex overhead: ~100 bytes
    // - ChibiOS thread context: ~256 bytes
    // - Stack safety margin: ~4372 bytes
    static constexpr size_t WORKER_STACK_SIZE = 5120;
    static WORKING_AREA(worker_wa_, WORKER_STACK_SIZE);


    // File I/O
    LogFile csv_log_;
    bool session_active_ = false;
    systime_t session_start_ = 0;
    uint32_t logged_count_ = 0;
    uint32_t dropped_logs_ = 0;
    uint32_t overflow_count_ = 0;               // Counter for ring buffer overflows
    bool header_written_ = false;

    // Async buffering
    static constexpr size_t BUFFER_SIZE = 32;
    std::array<DetectionLogEntry, BUFFER_SIZE> ring_buffer_;
    // volatile prevents compiler optimization; mutex_ provides thread synchronization
    volatile size_t head_{0};
    volatile size_t tail_{0};
    volatile bool is_full_{false};

    // Helper buffer for string formatting (avoid heap allocation)
    char line_buffer_[EDA::Constants::ERROR_MESSAGE_BUFFER_SIZE];

    // Internal methods
    static msg_t worker_thread_entry(void* arg);
    void worker_loop();
    bool write_entry_to_sd(const DetectionLogEntry& entry);
    bool ensure_csv_header();

    // generate_log_filename() - returns constexpr string (Flash storage)
    const char* generate_log_filename() const;

    DroneDetectionLogger(const DroneDetectionLogger&) = delete;
    DroneDetectionLogger& operator=(const DroneDetectionLogger&) = delete;
};

class DroneScanner {
public:
    // DroneScanner stores settings by VALUE (not reference)
    // Benefits: No lifetime dependency, thread-safe access, simpler ownership
    // Memory trade-off: Adds ~100 bytes to DroneScanner size (acceptable for 192KB RAM)
    struct BuiltinDroneFreq {
        Frequency freq;
        const char* desc;
        DroneType type;
    };

    // constexpr array instead of vector to avoid heap allocation
    static constexpr size_t BUILTIN_DB_SIZE = 17;
    static const std::array<BuiltinDroneFreq, BUILTIN_DB_SIZE> BUILTIN_DRONE_DB;

    // Database timeout constants (Flash storage)
    static constexpr uint32_t DB_LOAD_TIMEOUT_MS = 2000;     // 2 seconds max for DB load
    static constexpr uint32_t DB_SYNC_TIMEOUT_MS = 1000;     // 1 second max for sync

    // Scanning modes for DroneScanner (using EDA::Constants::ScanningMode)
    enum class ScanningMode {
        DATABASE,
        WIDEBAND_CONTINUOUS,
        HYBRID
    };

    // Constructor accepts settings by value to eliminate lifetime dependency
    DroneScanner(DroneAnalyzerSettings settings);
    ~DroneScanner();

    void start_scanning();
    void stop_scanning();
    // inline + noexcept for zero-overhead abstraction
    inline bool is_scanning_active() const noexcept { return scanning_active_; }
    bool load_frequency_database();
    size_t get_database_size() const;

    // inline + noexcept for zero-overhead abstraction
    inline ScanningMode get_scanning_mode() const noexcept { return scanning_mode_; }
    const char* scanning_mode_name() const;
    void set_scanning_mode(ScanningMode mode);

    void switch_to_real_mode();
    void switch_to_demo_mode();

    // Update scanner's settings from view's settings
    void update_settings(const DroneAnalyzerSettings& settings) {
        settings_ = settings;
    }

    void update_scan_range(Frequency min_freq, Frequency max_freq) {
        // Validate frequency range before use
        if (!EDA::Validation::validate_frequency(min_freq) || 
            !EDA::Validation::validate_frequency(max_freq)) {
            return;
        }
        
        if (min_freq >= max_freq) return;
        if (min_freq < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ) {
            min_freq = EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ;
        }
        if (max_freq > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
            max_freq = EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;
        }
        setup_wideband_range(min_freq, max_freq);
    }

    int32_t get_detection_rssi_safe(size_t freq_hash) const;
    uint8_t get_detection_count_safe(size_t freq_hash) const;

struct DetectionParams {
    DroneType type;
    Frequency frequency_hz;
    int32_t rssi_db;
    ThreatLevel threat_level;
};

    // Histogram Data Flow: SpectralAnalyzer → Scanner → Display
    // Function pointer with user data (no heap allocation, no std::function)
    using HistogramCallback = void(*)(const SpectralAnalyzer::HistogramBuffer&, uint8_t noise_floor, void* user_data) noexcept;
    
    /// @brief Set histogram callback for data flow from SpectralAnalyzer to DisplayController
    /// @param callback Function pointer to receive histogram data
    /// @param user_data User data pointer (typically 'this' pointer)
    /// @note Callback is called from scanner thread (must be thread-safe)
    void set_histogram_callback(HistogramCallback callback, void* user_data = nullptr) noexcept {
        histogram_callback_ = callback;
        histogram_callback_user_data_ = user_data;
    }

    void perform_scan_cycle(DroneHardwareController& hardware);
    void process_rssi_detection(const freqman_entry& entry, int32_t rssi);
    void send_drone_detection_message(const DetectionParams& params);

    void update_tracked_drone(const DetectionParams& params);

    void update_tracked_drone_internal(const DetectionParams& params);
    // @note: Caller MUST hold data_mutex. This method assumes the mutex is already acquired.

    void remove_stale_drones();

    rf::Frequency get_current_scanning_frequency() const;
    ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
    // STEP 3 FIX: Return by value instead of by reference to avoid dangling reference issues
    TrackedDrone getTrackedDrone(size_t index) const;  // 🔴 FIX: Protected with mutex
    void handle_scan_error(const char* error_msg);

    // DIAMOND OPTIMIZATION: inline + noexcept for zero-overhead abstraction
    inline size_t get_approaching_count() const noexcept { return approaching_count_; }
    inline size_t get_receding_count() const noexcept { return receding_count_; }
    inline size_t get_static_count() const noexcept { return static_count_; }
    inline uint32_t get_total_detections() const noexcept { return total_detections_; }
    inline uint32_t get_scan_cycles() const noexcept { return scan_cycles_; }
    inline bool is_real_mode() const noexcept { return is_real_mode_; }
    size_t get_total_memory_usage() const { return 0; }

    DroneScanner(const DroneScanner&) = delete;
    DroneScanner(DroneScanner&&) = delete;
    DroneScanner& operator=(const DroneScanner&) = delete;
    DroneScanner& operator=(DroneScanner&&) = delete;

    // 🎯 Use UnifiedStringLookup and UnifiedColorLookup directly at call sites
    // UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type))
    // UnifiedColorLookup::drone(static_cast<uint8_t>(type))

    Frequency get_current_radio_frequency() const;

    struct DroneSnapshot {
        TrackedDrone drones[DroneConstants::MAX_TRACKED_DRONES];
        size_t count = 0;
    };

    DroneSnapshot get_tracked_drones_snapshot() const;

    bool try_get_tracked_drones_snapshot(DroneSnapshot& out_snapshot) const;

    // Public API for safe initialization
    void initialize_database_and_scanner();
    void initialize_database_async();
    void cleanup_database_and_scanner();
    bool is_database_loading_complete() const;
    bool is_initialization_complete() const;
    void sync_database();

 private:
    void reset_scan_cycles();
    void initialize_wideband_scanning();
    void setup_wideband_range(Frequency min_freq, Frequency max_freq);

    // Async database loading methods
    static msg_t db_loading_thread_entry(void* arg);
    void db_loading_thread_loop();
    void wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override);
    void process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                   int32_t original_threshold, int32_t wideband_threshold);

    void process_spectral_detection(const freqman_entry& entry,
                                   const SpectralAnalysisResult& analysis_result,
                                   ThreatLevel threat_level, DroneType drone_type);

    static msg_t scanning_thread_function(void* arg);
    msg_t scanning_thread();

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

    // Eliminate stack arrays in hot paths - moved to class member buffers
    std::array<freqman_entry, 10> entries_to_scan_{};

    // stale_indices buffer (was stack array in remove_stale_drones)
    std::array<size_t, DroneConstants::MAX_TRACKED_DRONES> stale_indices_{};

    // Intelligent scanning methods
    size_t get_next_slice_with_intelligence();

     Thread* scanning_thread_ = nullptr;
     mutable Mutex data_mutex;
     // volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
     volatile bool scanning_active_{false};
    
    // Histogram Callback (function pointer for histogram data flow)
    HistogramCallback histogram_callback_ = nullptr;
    void* histogram_callback_user_data_ = nullptr;

        // Static storage for Scanner (zero heap allocation)

        // Static storage for FreqmanDB (4KB for safety)
        static constexpr size_t FREQ_DB_STORAGE_SIZE = EDA::Constants::FREQ_DB_STORAGE_SIZE_4KB;
        alignas(alignof(FreqmanDB))
        static uint8_t freq_db_storage_[FREQ_DB_STORAGE_SIZE];

        // Compile-time alignment verification
        static_assert(alignof(FreqmanDB) <= 16, "FreqmanDB alignment too large for static storage");
        static_assert(FREQ_DB_STORAGE_SIZE >= sizeof(FreqmanDB), "FREQ_DB_STORAGE_SIZE too small");
        // FreqmanDB size validated by static_assert in eda_constants.hpp

        // Static storage for TrackedDrones (~800 bytes)
        static constexpr size_t TRACKED_DRONES_STORAGE_SIZE =
            sizeof(TrackedDrone) * EDA::Constants::MAX_TRACKED_DRONES;
        alignas(alignof(TrackedDrone))
        static uint8_t tracked_drones_storage_[TRACKED_DRONES_STORAGE_SIZE];

        // Compile-time alignment verification
        static_assert(alignof(TrackedDrone) <= 16, "TrackedDrone alignment too large for static storage");
        static_assert(TRACKED_DRONES_STORAGE_SIZE >= sizeof(std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>),
                     "TRACKED_DRONES_STORAGE_SIZE too small");

       // Pointers to objects created via placement new
       FreqmanDB* freq_db_ptr_ = nullptr;
       std::array<TrackedDrone, DroneConstants::MAX_TRACKED_DRONES>* tracked_drones_ptr_ = nullptr;

       // Construction flags for placement-newed objects
       bool freq_db_constructed_ = false;
       bool tracked_drones_constructed_ = false;

       // State flags
        bool freq_db_loaded_ = false;
        size_t current_db_index_ = 0;
        Frequency last_scanned_frequency_ = 0;
        // volatile for thread-safe timestamp access
        volatile systime_t last_detection_log_time_{0};

        // Async database loading to prevent UI freeze
        Thread* db_loading_thread_ = nullptr;
        // volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
        volatile bool db_loading_active_{false};

        // Initialization complete flag for async initialization coordination
        volatile bool initialization_complete_{false};

       // Static thread stack (8KB for thread safety)
        static constexpr size_t DB_LOADING_STACK_SIZE = 8192;  // 8KB

          static WORKING_AREA(db_loading_wa_, DB_LOADING_STACK_SIZE);

      // volatile uint32_t with mutex protection for thread-safe counter access
      volatile uint32_t scan_cycles_{0};
      volatile uint32_t total_detections_{0};

      ScanningMode scanning_mode_ = ScanningMode::DATABASE;  // Uses DroneScanner::ScanningMode
      // volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
      volatile bool is_real_mode_{true};

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
    
    // Thread-safe spectrum buffer (replaces static buffer)
    std::array<uint8_t, 256> spectrum_data_{};

    // Histogram buffer moved to class member (replaces stack allocation)
    SpectralAnalyzer::HistogramBuffer histogram_buffer_{};

    // Settings stored by VALUE (not reference)
    DroneAnalyzerSettings settings_;
    
    // Last scan error for diagnostics
    const char* last_scan_error_ = nullptr;

    // LUTs at end of class (after all declarations)
    
    // LUT for MovementTrend counters (pointers to class members)
    static constexpr size_t DroneScanner::* const TREND_COUNTERS[] = {
        &DroneScanner::static_count_,      // STATIC = 0
        &DroneScanner::approaching_count_, // APPROACHING = 1
        &DroneScanner::receding_count_,    // RECEDING = 2
        &DroneScanner::static_count_       // UNKNOWN = 3 (fallback)
    };
    static_assert(sizeof(TREND_COUNTERS) / sizeof(size_t DroneScanner::*) == 4, "TREND_COUNTERS size");
    
    // LUT for scanning functions (pointers to class methods)
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

    // TOCTOU race condition fix - critical section check-and-fetch method
    bool get_rssi_if_fresh(int32_t& out_rssi) {
        CriticalSection lock;
        if (!rssi_updated_) {
            return false;
        }
        out_rssi = last_valid_rssi_;
        rssi_updated_ = false;
        return true;
    }

    // Spectrum data access method (single critical section to avoid TOCTOU race)
    bool get_latest_spectrum_if_fresh(std::array<uint8_t, 256>& out_db_buffer) {
        // Single critical section for entire check-and-fetch operation
        CriticalSection lock;
        if (!spectrum_updated_) {
            return false;
        }
        out_db_buffer = last_spectrum_db_;
        spectrum_updated_ = false;
        return true;
    }
    bool try_get_latest_spectrum(std::array<uint8_t, 256>& out_db_buffer) {
        MutexLock lock(spectrum_mutex_, LockOrder::SPECTRUM_MUTEX);
        out_db_buffer = last_spectrum_db_;
        return true;
    }
    void clear_spectrum_flag() {
        CriticalSection lock;
        spectrum_updated_ = false;
    }

    // Public method to set spectrum FIFO (called from parent View)
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
    
    // Spectrum data buffer and synchronization
    std::array<uint8_t, 256> last_spectrum_db_;
    mutable Mutex spectrum_mutex_;

    // Diamond Code: Use volatile bool for thread-safe flag access
    // volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
    // Protected by raii::SystemLock (chSysLock/chSysUnlock) for write operations
    volatile bool spectrum_updated_{false};

    SpectrumMode spectrum_mode_;
    Frequency center_frequency_;
    uint32_t bandwidth_hz_;
    RxRadioState radio_state_;
    ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
    bool spectrum_streaming_active_ = false;

    // Diamond Code: Use volatile bool for thread-safe flag access
    // volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
    // Protected by raii::SystemLock (chSysLock/chSysUnlock) for write operations
    volatile bool rssi_updated_{false};
    volatile int32_t last_valid_rssi_{-120};
    
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

    // Uses UnifiedColorLookup instead of duplicate LUT

    // Cached previous values for Check-Before-Update optimization
    ThreatLevel last_threat_ = ThreatLevel::NONE;
    bool last_is_scanning_ = false;
    Frequency last_freq_ = 0;
    size_t last_approaching_ = 0;
    size_t last_static_ = 0;
    size_t last_receding_ = 0;
    char last_text_[128];  // Fixed-size buffer instead of std::string
    size_t last_text_len_ = 0;  // Cached text length to avoid strlen() in paint()
};

class ThreatCard : public View {
public:
    explicit ThreatCard(size_t card_index = 0, Rect parent_rect = {0, 0, screen_width, 24});
    ~ThreatCard() = default;

    void update_card(const DisplayDroneEntry& drone);
    void clear_card();
    Color get_card_bg_color() const;
    Color get_card_text_color() const;

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
    char last_threat_name_[16];
    size_t last_card_text_len_ = 0;  // Cached text length to avoid strlen() in paint()

    void paint(Painter& painter) override;

    // Uses UnifiedColorLookup instead of duplicate LUT
};

enum class DisplayMode : uint8_t { SCANNING = 0, ALERT = 1, NORMAL = 2 };

// constexpr Style LUTs for ConsoleStatusBar (stored in Flash)
struct StatusStyleConfig {
    uint32_t bg_color;
    uint32_t text_color;
};

class ConsoleStatusBar : public View {
public:
    explicit ConsoleStatusBar(size_t bar_index = 0, Rect parent_rect = {0, 0, screen_width, 16});
    ~ConsoleStatusBar() = default;

    // const char* instead of const std::string& to save RAM
    void update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles = 0, uint32_t detections = 0);
    void update_alert_status(ThreatLevel threat, size_t total_drones, const char* alert_msg);
    void update_normal_status(const char* primary, const char* secondary);
    void set_display_mode(DisplayMode mode);

    ConsoleStatusBar(const ConsoleStatusBar&) = delete;
    ConsoleStatusBar& operator=(const ConsoleStatusBar&) = delete;

private:
    // constexpr style arrays (Flash storage, zero RAM)
    EDA_FLASH_CONST inline static constexpr StatusStyleConfig STATUS_STYLES[] = {
        {0x001F, 0xFFFF},  // Синий фон, белый текст (SCANNING)
        {0xFFFF, 0x0000}   // Белый фон, черный текст (STOPPED)
    };

    EDA_FLASH_CONST inline static constexpr StatusStyleConfig THREAT_STYLES[] = {
        {0xF800, 0xFFFF},  // Красный фон, белый текст (CRITICAL)
        {0xFFE0, 0x0000}   // Желтый фон, черный текст (MEDIUM)
    };

    // Unified icon LUT for all threat levels (Flash storage, zero RAM)
    EDA_FLASH_CONST inline static constexpr const char* ALERT_ICONS[] = {"(i)", "[!]", "[O]", "[X]", "[!!]"};
    static_assert(sizeof(ALERT_ICONS) / sizeof(const char*) == 5, "ALERT_ICONS size");

    // Unified style LUT for alert modes
    struct AlertStyleEntry {
        uint32_t bg_color;
        uint32_t text_color;
    };

    static constexpr AlertStyleEntry ALERT_STYLES[] = {
        {0xF800, 0xFFFF},  // CRITICAL (RED bg, WHITE text)
        {0xFFE0, 0x0000}   // MEDIUM (YELLOW bg, BLACK text)
    };
    static_assert(sizeof(ALERT_STYLES) / sizeof(AlertStyleEntry) == 2, "ALERT_STYLES size");

    // Progress Bar LUT (pre-computed progress bars to save CPU cycles)
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

    // Display Mode Rendering LUT (data-driven rendering for paint())
    struct DisplayModeLayout {
        uint8_t visibility_mask;
        uint32_t bar_color;
    };

    static constexpr DisplayModeLayout DISPLAY_MODE_LAYOUTS[] = {
        {0b001, 0x0000},  // NORMAL: normal_text visible, no bar
        {0b010, 0x001F},  // SCANNING: progress_text visible, blue bar
        {0b100, 0xF800}   // ALERT: alert_text visible, red bar
    };
    static_assert(sizeof(DISPLAY_MODE_LAYOUTS) / sizeof(DisplayModeLayout) == 3, "DISPLAY_MODE_LAYOUTS size");

    size_t bar_index_;
    DisplayMode mode_ = DisplayMode::NORMAL;
    Rect parent_rect_;
    
    // Cached text lengths to avoid strlen() in paint()
    size_t last_progress_len_ = 0;
    size_t last_alert_len_ = 0;
    size_t last_normal_len_ = 0;

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

    Frequency calculate_optimal_tick_interval();

    bool should_use_mhz_labels() const;

private:
    // constexpr constants (stored in Flash)
    static constexpr int RULER_HEIGHT = 12;
    static constexpr int TICK_HEIGHT_MAJOR = 10;
    static constexpr int TICK_HEIGHT_MINOR = 6;
    static constexpr int DEFAULT_TICK_COUNT = 5;

    // State variables
    Frequency min_freq_{2400000000ULL};
    Frequency max_freq_{2500000000ULL};
    int spectrum_width_{240};
    bool visible_{true};
    RulerStyle ruler_style_{RulerStyle::COMPACT_GHZ};
    int target_tick_count_{DEFAULT_TICK_COUNT};

    // Private helper methods
    void draw_compact_ticks(Painter& painter, const Rect r);
    void format_compact_label(char* buffer, size_t buffer_size, Frequency freq);
};

// Display data structure for UI/DSP separation
struct DisplayData {
    // Phase 1: Data fetched from scanner (DSP/logic layer)
    bool is_scanning;
    Frequency current_freq;
    size_t total_freqs;
    ThreatLevel max_threat;
    size_t approaching_count;
    size_t receding_count;
    size_t static_count;
    uint32_t total_detections;
    bool is_real_mode;
    uint32_t scan_cycles;
    bool has_detections;
    size_t color_idx;  // Computed color index for big display style
    // Timestamp to track when snapshot was captured
    systime_t snapshot_timestamp;
};

class DroneDisplayController : public View {
public:
    // Spectrum Configuration Constants (Flash storage, zero RAM overhead)
    
    /// @brief Default frequency width per spectrum bin (100 kHz)
    static constexpr uint32_t DEFAULT_EACH_BIN_SIZE_HZ = 100000;
    
    /// @brief Default frequency step for spectrum markers (1 MHz)
    static constexpr uint32_t DEFAULT_MARKER_PIXEL_STEP_HZ = 1000000;
    
    /// @brief Default minimum color power level
    static constexpr uint8_t DEFAULT_MIN_COLOR_POWER = 0;
    
    // Histogram Color Thresholds (0-255 scale, Flash storage)
    
    /// @brief 20% threshold (51 = 0.20 * 255)
    static constexpr uint8_t HISTOGRAM_COLOR_THRESHOLD_20PCT = 51;
    
    /// @brief 40% threshold (102 = 0.40 * 255)
    static constexpr uint8_t HISTOGRAM_COLOR_THRESHOLD_40PCT = 102;
    
    /// @brief 60% threshold (153 = 0.60 * 255)
    static constexpr uint8_t HISTOGRAM_COLOR_THRESHOLD_60PCT = 153;
    
    /// @brief 80% threshold (204 = 0.80 * 255)
    static constexpr uint8_t HISTOGRAM_COLOR_THRESHOLD_80PCT = 204;
    
    // Display Mode Enumeration (type-safe display mode selection)

    /// @brief Display mode enumeration for type safety
    enum class DisplayRenderMode : uint8_t {
        DEFAULT = 0,  ///< Default mode
        SPECTRUM = 1, ///< Spectrum display mode
        HISTOGRAM = 2  ///< Histogram display mode
    };

    // Static buffer sizes to prevent heap allocation in constructor
    static constexpr size_t SPECTRUM_ROW_SIZE = 240;
    static constexpr size_t RENDER_LINE_SIZE = 240;
    static constexpr size_t WATERFALL_SIZE = 40 * 240;  // 9.6KB waterfall buffer
    static constexpr size_t MAX_UI_DRONES = 3;  // Reduced from 16 to 3 for memory savings
    
    // Histogram Display Buffer (zero-heap, static storage in .bss)
    struct HistogramDisplayBuffer {
        // 64 bins, each storing count (0-255), total: 64 bytes
        alignas(4) uint8_t bin_counts[64];
        
        // Metadata for rendering
        uint8_t max_count;      // Maximum bin count (for scaling)
        uint8_t noise_floor;    // Noise floor from spectral analysis
        bool is_valid;          // Buffer contains valid data
        
        /// @brief Clear histogram buffer
        void clear() noexcept {
            std::fill(std::begin(bin_counts), std::end(bin_counts), 0);
            max_count = 0;
            noise_floor = 0;
            is_valid = false;
        }
    };
    
    // Histogram rendering constants (stored in Flash)
    static constexpr int HISTOGRAM_Y = 164;
    static constexpr int HISTOGRAM_HEIGHT = 26;
    static constexpr int HISTOGRAM_WIDTH = 240;
    static constexpr int HISTOGRAM_NUM_BINS = 64;
    static constexpr int HISTOGRAM_BIN_WIDTH = HISTOGRAM_WIDTH / HISTOGRAM_NUM_BINS;  // 3.75px/bin
    
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

    // Direct array access instead of switch
    static constexpr size_t NUM_DRONE_TEXT_WIDGETS = 3;

    // Return reference to static array
    std::array<DisplayDroneEntry, MAX_UI_DRONES>& detected_drones() {
        return *reinterpret_cast<std::array<DisplayDroneEntry, MAX_UI_DRONES>*>(detected_drones_storage_);
    }
    const std::array<DisplayDroneEntry, MAX_UI_DRONES>& detected_drones() const {
        return *reinterpret_cast<const std::array<DisplayDroneEntry, MAX_UI_DRONES>*>(detected_drones_storage_);
    }

    // Histogram Display Methods
    
    /// @brief Update histogram display buffer with new data
    void update_histogram_display(
        const SpectralAnalyzer::HistogramBuffer& analysis_histogram,
        uint8_t noise_floor
    ) noexcept;
    
    /// @brief Render histogram to display
    void render_histogram(Painter& painter) noexcept;

    /// @brief Clear the histogram display area
    /// @param painter The painter to use for clearing
    void clear_histogram_area(Painter& painter) noexcept;

    /// @brief Get the current display mode
    /// @return Current display mode (SPECTRUM or HISTOGRAM)
    DisplayRenderMode get_display_mode() const noexcept {
        return mode_;
    }

    /// @brief Set the display mode
    /// @param new_mode The new display mode to set
    void set_display_mode(DisplayRenderMode new_mode) noexcept {
        mode_ = new_mode;
    }

    void update_detection_display(const DroneScanner& scanner);
    void update_trends_display(size_t approaching, size_t static_count, size_t receding);
    void update_signal_type_display(const char* signal_type);
    void set_scanning_status(bool active, const char* message);
    void set_frequency_display(Frequency freq);
    void add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi);
    void update_drones_display(const DroneScanner& scanner);
    void sort_drones_by_rssi();
    void render_drone_text_display();

    void process_mini_spectrum_data(const ChannelSpectrum& spectrum);
    bool process_bins(uint8_t* power_level);

    void render_bar_spectrum(Painter& painter);
    void highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, DroneConstants::MAX_DISPLAYED_DRONES>& drones);
    size_t frequency_to_spectrum_bin(Frequency freq_hz) const;
    void clear_spectrum_buffers();
    void set_spectrum_range(Frequency min_freq, Frequency max_freq);
    void update_frequency_ruler();
    void set_ruler_style(RulerStyle style);
    void apply_display_settings(const DroneAnalyzerSettings& settings);
    CompactFrequencyRuler& compact_frequency_ruler() { return compact_frequency_ruler_; }

    // Buffer allocation/deallocation (deferred from constructor)
    void allocate_buffers();
    void deallocate_buffers();
    
    // Buffer validation methods (protection against UB)
    bool are_buffers_allocated() const;
    bool are_buffers_valid() const;
    bool allocate_buffers_from_pool();

    // Methods for accessing static buffers (O(1) access, no dereference overhead)

    using SpectrumRowBuffer = std::array<Color, SPECTRUM_ROW_SIZE>;
    using RenderLineBuffer = std::array<Color, RENDER_LINE_SIZE>;
    using PowerLevelsBuffer = std::array<uint8_t, 200>;

    SpectrumRowBuffer& spectrum_row_buffer() {
        return *reinterpret_cast<SpectrumRowBuffer*>(spectrum_row_buffer_storage_);
    }
    const SpectrumRowBuffer& spectrum_row_buffer() const {
        return *reinterpret_cast<const SpectrumRowBuffer*>(spectrum_row_buffer_storage_);
    }

    RenderLineBuffer& render_line_buffer() {
        return *reinterpret_cast<RenderLineBuffer*>(render_line_buffer_storage_);
    }
    const RenderLineBuffer& render_line_buffer() const {
        return *reinterpret_cast<const RenderLineBuffer*>(render_line_buffer_storage_);
    }

    PowerLevelsBuffer& spectrum_power_levels() {
        return *reinterpret_cast<PowerLevelsBuffer*>(spectrum_power_levels_storage_);
    }
    const PowerLevelsBuffer& spectrum_power_levels() const {
        return *reinterpret_cast<const PowerLevelsBuffer*>(spectrum_power_levels_storage_);
    }


    // Public methods for parent View message delegation
    void set_spectrum_fifo(ChannelSpectrumFIFO* fifo) {
        spectrum_fifo_ = fifo;
    }

    // REMOVED: frequency_ruler_ member (Dead Code - duplicate never used)

    // constexpr LUT in Flash instead of switch/if-else (saves ROM)
    static constexpr RulerStyle RULER_STYLE_LUT[] = {
        RulerStyle::COMPACT_GHZ,  // 0
        RulerStyle::COMPACT_GHZ,  // 1
        RulerStyle::COMPACT_MHZ,  // 2
        RulerStyle::STANDARD_GHZ, // 3
        RulerStyle::STANDARD_MHZ, // 4
        RulerStyle::DETAILED,     // 5
        RulerStyle::SPACED_GHZ    // 6
    };

    // enum class for signal types (instead of string comparison)
    enum class SignalType : uint8_t {
        DEFAULT = 0,
        DIGITAL = 1,
        ANALOG = 2,
        NOISE = 3
    };

    // constexpr LUT for signal types (no if-else)
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

    // constexpr LUT for big_display colors (no cascading if-else)
    static constexpr const Color BIG_DISPLAY_COLORS[] = {
        Color::dark_grey(),      // Idle/Default
        Color::green(),          // Scanning (Green)
        Color::orange(),         // Has detections (Orange)
        Color::yellow(),         // Medium threat (Yellow)
        Color::red()            // High+ threat (Red)
    };

    // Bar spectrum configuration (replaces waterfall)
    struct BarSpectrumConfig {
        static constexpr int WATERFALL_Y_START = 81;
        static constexpr int BAR_HEIGHT_MAX = DroneConstants::MINI_SPECTRUM_HEIGHT;
        static constexpr uint8_t NOISE_THRESHOLD = 10;
        static constexpr uint8_t PEAK_SHARPNESS_THRESHOLD = 15;

        // Bar spectrum colors (Flash storage)
        static constexpr Color BAR_COLORS[] = {
            Color::blue(),          // 0: Wideband/Plateau (WiFi, Video) - Blue
            Color::red(),           // 1: Sharp Peak (Drone Control) - Red
            Color::grey()           // 2: Unknown/Noise - Grey
        };
    };
    
    // Histogram Color LUT (Flash storage)
    struct HistogramColorConfig {
        // 5-level gradient from noise floor to peak signal
        static constexpr Color HISTOGRAM_COLORS[] = {
            Color::dark_grey(),   // 0-20%: Noise floor
            Color::blue(),        // 20-40%: Low signal
            Color::cyan(),        // 40-60%: Medium signal
            Color::yellow(),      // 60-80%: High signal
            Color::red()          // 80-100%: Peak signal
        };
        static constexpr uint8_t NUM_COLOR_LEVELS = 5;
    };
    
    void process_frame_sync();

    DroneDisplayController(const DroneDisplayController&) = delete;
    DroneDisplayController& operator=(const DroneDisplayController&) = delete;

    static constexpr const char* DRONE_DISPLAY_FORMAT = "%s %s %-4lddB %c";

    // constexpr Style for big_display (instead of local arrays)
    static constexpr Style BIG_DISPLAY_STYLES[] = {
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[0]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[1]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[2]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[3]},
        {font::fixed_8x16, Color::black(), BIG_DISPLAY_COLORS[4]}
    };

    // Frequency Formatting LUT (Flash storage, O(1) lookup)
    struct FrequencyFormatEntry {
        int64_t min_freq;
        const char* format;
        int64_t divider;       // Integer part divider
        int64_t decimal_div;  // Fractional part divider
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

    // Helper for indexed widget access (no switch)
    Text* drone_text_widget(size_t index) {
        static Text* const widgets[] = {&text_drone_1_, &text_drone_2_, &text_drone_3_};
        return (index < NUM_DRONE_TEXT_WIDGETS) ? widgets[index] : nullptr;
    }
    
    // Cached drone display strings for Check-Before-Update pattern
    char last_drone_text_0_[64] = {0};
    char last_drone_text_1_[64] = {0};
    char last_drone_text_2_[64] = {0};

    CompactFrequencyRuler compact_frequency_ruler_{{0, 68, screen_width, 12}};
    // REMOVED: frequency_ruler_ member (Dead Code)

    // Static array DETECTED_DRONES (zero heap allocation, ~240 bytes)

    // Member declaration order MUST match constructor initializer list order
    std::array<DisplayDroneEntry, DroneConstants::MAX_DISPLAYED_DRONES> displayed_drones_;

    // Static array instead of unique_ptr
    alignas(alignof(DisplayDroneEntry))
    static DisplayDroneEntry detected_drones_storage_[MAX_UI_DRONES];

    // Counter for used elements
    size_t detected_drones_count_ = 0;


    // Static buffers (Zero-Heap)
    // Diamond Code Principle: All data must be static or stack-based

    // Static buffers (stored in .bss, initialized to zeros)
    // SPECTRUM_ROW_SIZE * sizeof(Color) = 240 * 2 = 480 bytes
    // RENDER_LINE_SIZE * sizeof(Color) = 240 * 2 = 480 bytes
    // 200 * sizeof(uint8_t) = 200 bytes
    // Total: ~1.16 KB in .bss instead of heap allocation

    alignas(alignof(std::array<Color, SPECTRUM_ROW_SIZE>))
    static Color spectrum_row_buffer_storage_[SPECTRUM_ROW_SIZE];

    alignas(alignof(std::array<Color, RENDER_LINE_SIZE>))
    static Color render_line_buffer_storage_[RENDER_LINE_SIZE];

    alignas(alignof(std::array<uint8_t, 200>))
    static uint8_t spectrum_power_levels_storage_[200];

    // Флаг для отслеживания состояния буферов
    bool buffers_allocated_ = false;
    
    // ===========================================
    // DIAMOND OPTIMIZATION: Histogram Display Buffer
    // ===========================================
    // Zero-heap histogram display buffer (static storage in .bss)
    HistogramDisplayBuffer histogram_display_buffer_{};
    
    // Dirty flag for histogram rendering optimization
    bool histogram_dirty_ = false;

    // ===========================================
    // DIAMOND FIX: Thread-Safe Buffer Protection
    // ===========================================
    // Mutexes for protecting shared buffers accessed by multiple threads
    // - spectrum_mutex_: Protects spectrum_power_levels_ buffer
    // - histogram_mutex_: Protects histogram_display_buffer_ buffer
    //
    // LOCK ORDER RULE:
    // Always acquire locks in ascending order:
    //   1. SPECTRUM_MUTEX (spectrum_mutex_)
    //   2. HISTOGRAM_MUTEX (histogram_mutex_)
    //
    // These mutexes prevent TOCTOU race conditions between:
    // - Scanner thread: Writes to spectrum_power_levels_ via process_bins()
    // - UI thread: Reads from spectrum_power_levels_ via render_bar_spectrum()
    // - Scanner thread: Writes to histogram_display_buffer_ via update_histogram_display()
    // - UI thread: Reads from histogram_display_buffer_ via render_histogram()
    mutable Mutex spectrum_mutex_;  ///< Protects spectrum_power_levels_ buffer
    mutable Mutex histogram_mutex_; ///< Protects histogram_display_buffer_ buffer

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
    DisplayRenderMode mode_ = DisplayRenderMode::SPECTRUM;

    SpectrumConfig spectrum_config_;

    // Use UnifiedStringLookup and UnifiedColorLookup directly at call sites

    // Signal shape analysis for bar spectrum (no SpectralAnalyzer)
    inline size_t get_bar_color_index(size_t x, uint8_t power) const {
        // For static buffers, check is simplified
        if (!buffers_allocated_) {
            return 2; // Unknown/Noise (Grey)
        }

        // Sharp peak (narrowband drone control): current > neighbors by 15+
        // Wideband signal (wideband video/WiFi): all approximately equal
        const auto& levels = spectrum_power_levels();
        if (x > 0 && x < levels.size() - 1) {
            uint8_t prev = levels[x - 1];
            uint8_t next = levels[x + 1];

            // Sharp peak logic: current HIGHER than neighbors by 15+ units
            if (power > prev + BarSpectrumConfig::PEAK_SHARPNESS_THRESHOLD &&
                power > next + BarSpectrumConfig::PEAK_SHARPNESS_THRESHOLD) {
                return 1; // Sharp Peak (Red)
            }
        }

        // Wideband signal: Blue
        return 0; // Wideband (Blue)
    }


    void handle_channel_spectrum(const ChannelSpectrum& spectrum);
};

static constexpr const char* DEFAULT_CONFIG_PATH = "DRONES/DATA.CFG";
static constexpr const char* FALLBACK_CONFIG_PATH = "APP/SETTINGS/DRONES.CFG";
static constexpr uint32_t DEFAULT_BANDWIDTH = 24000000UL;
static constexpr Frequency DEFAULT_CENTER_FREQUENCY = 2400000000LL;
static constexpr const char* DEFAULT_SPECTRUM_FILE = "DEFAULT";

class DroneUIController {
public:
    // ===========================================
    // INITIALIZATION PATTERN
    // ===========================================
    // Constructor does NOT take display_controller as parameter.
    // Instead, display_controller_ is set via set_display_controller() after construction.
    //
    // This two-phase initialization pattern makes the initialization order
    // independent of member declaration order in the parent class.
    //
    // Benefits:
    // - No fragile dependency on declaration order
    // - ui_controller_ can be declared before display_controller_
    // - display_controller_ can be declared anywhere in the parent class
    //
    // Usage in EnhancedDroneSpectrumAnalyzerView:
    //   1. Construct ui_controller_ (display_controller_ = nullptr)
    //   2. Construct display_controller_
    //   3. Call ui_controller_.set_display_controller(&display_controller_)
    //
    // Constructor without display_controller - set via set_display_controller() after construction
    // This makes initialization order independent of member declaration order
    DroneUIController(NavigationView& nav,
                     DroneHardwareController& hardware,
                     DroneScanner& scanner,
                     ::AudioManager& audio_mgr);
    ~DroneUIController();

    // Set display_controller after construction
    void set_display_controller(DroneDisplayController* display_controller) {
        display_controller_ = display_controller;
    }

    void on_start_scan();
    void on_stop_scan();
    void show_menu();
    void on_open_settings();
    void on_about();
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
    // volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
    volatile bool scanning_active_{false};
    DroneDisplayController* display_controller_ = nullptr;
    ::ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings settings_;

    void on_manage_frequencies();
    void on_create_new_database();
    void on_frequency_warning();
    void select_spectrum_mode(SpectrumMode mode);
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

    /// @brief Toggle between spectrum and histogram display modes
    void on_toggle_display_mode();

    // Deferred initialization state machine
    enum class InitState : uint8_t {
        CONSTRUCTED = 0,           // Constructor completed
        BUFFERS_ALLOCATED,         // display buffers heap-allocated
        DATABASE_LOADING,           // DIAMOND FIX #4: Async database loading in progress
        DATABASE_LOADED,           // Async database loading complete
        HARDWARE_READY,            // hardware_.on_hardware_show() completed
        UI_LAYOUT_READY,           // initialize_modern_layout() completed
        SETTINGS_LOADED,           // SettingsPersistence::load() completed
         COORDINATOR_READY,         // coordinator thread creation prepared
         FULLY_INITIALIZED = 8,    // Ready for operation
         INITIALIZATION_ERROR = 9  // Timeout or critical error
     };

 private:
    // Forward declarations for phase initialization methods
    void init_phase_allocate_buffers();
    void init_phase_load_database();
    void init_phase_init_hardware();
    void init_phase_setup_ui();
    void init_phase_load_settings();
    void init_phase_finalize();

    // Initialization timing constants (eliminates magic numbers)
    struct InitTiming {
        // Corrected delays (each phase must have sufficient time to execute)

        static constexpr uint32_t TIMEOUT_MS = 15000;  // 15 сек (увеличено)
        static constexpr uint32_t PHASE_INTERVAL_MS = 50;

        // Cumulative delays (each phase + minimum 200ms for execution)
        static constexpr uint32_t PHASE_DELAY_0_MS = 50;     // Allocate buffers
        static constexpr uint32_t PHASE_DELAY_1_MS = 300;    // Database (50 + 250 for async)
        static constexpr uint32_t PHASE_DELAY_2_MS = 500;    // Hardware (300 + 200)
        static constexpr uint32_t PHASE_DELAY_3_MS = 700;    // UI Setup (500 + 200)
        static constexpr uint32_t PHASE_DELAY_4_MS = 900;    // Settings (700 + 200)
        static constexpr uint32_t PHASE_DELAY_5_MS = 1100;   // Finalize (900 + 200)

        static constexpr uint8_t MAX_PHASES = 6;
    };


    // constexpr LUT for initialization messages in Flash
    static constexpr const char* const INIT_STATUS_MESSAGES[] = {
        "Starting up...",      // CONSTRUCTED = 0
        "Buffers ready",       // BUFFERS_ALLOCATED = 1
        "Database ready",      // DATABASE_LOADED = 2
        "Hardware ready",      // HARDWARE_READY = 3
        "UI ready",            // UI_LAYOUT_READY = 4
        "Settings loaded",     // SETTINGS_LOADED = 5 (NEW!)
        "Coordinator ready",   // COORDINATOR_READY = 6
        "All systems go",      // FULLY_INITIALIZED = 7
        "Init error!"          // INITIALIZATION_ERROR = 8
    };
    static constexpr const char* const INIT_STATUS_TITLES[] = {
        "INIT",               // CONSTRUCTED through COORDINATOR_READY (0-6)
        "EDA Ready",          // FULLY_INITIALIZED = 7
        "ERROR"               // INITIALIZATION_ERROR = 8
    };

    // constexpr LUT for initialization phases (Flash storage)
 
    struct InitPhaseConfig {
        const char* const name;           // String in Flash (const char*)
        uint32_t delay_ms;               // Delay in ms
        void (EnhancedDroneSpectrumAnalyzerView::*init_func)();  // Raw method pointer
    };

    static constexpr InitPhaseConfig INIT_PHASES[] = {
        {"Allocating buffers...",   InitTiming::PHASE_DELAY_0_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_allocate_buffers},
        {"Loading database...",     InitTiming::PHASE_DELAY_1_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_load_database},
        {"Initializing hardware...", InitTiming::PHASE_DELAY_2_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_init_hardware},
        {"Setting up UI...",        InitTiming::PHASE_DELAY_3_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_setup_ui},
        {"Loading settings...",     InitTiming::PHASE_DELAY_4_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_load_settings},
        {"Finalizing...",           InitTiming::PHASE_DELAY_5_MS, &EnhancedDroneSpectrumAnalyzerView::init_phase_finalize}
    };


    // constexpr array for error messages (Flash storage)
    EDA_FLASH_CONST static constexpr const char* const ERROR_MESSAGES[] = {
        "No error",           // NONE = 0
        "Init timeout",       // GENERAL_TIMEOUT = 1
        "Allocation failed",  // ALLOCATION_FAILED = 2
        "Database error"      // DATABASE_ERROR = 3
    };
    static_assert(sizeof(ERROR_MESSAGES) / sizeof(const char*) == 4, "ERROR_MESSAGES size");

 private:
    NavigationView& nav_;

    

    ::ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings settings_;

    // Initialization order ensures proper initialization
    // CRITICAL: ui_controller_ stores display_controller_ as POINTER (not reference)
    DroneHardwareController hardware_;
    DroneScanner scanner_;
    ::AudioManager audio_;

    // INITIALIZATION ORDER FIX: ui_controller_ declared before display_controller_
    // The display_controller dependency is now set via set_display_controller() after construction,
    // making initialization order independent of member declaration order.
    // This eliminates the fragile dependency on declaration order.
    DroneUIController ui_controller_;
    DroneDisplayController display_controller_;
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

            // Normal operation mode: Process spectrum data in display controller
            display_controller_.process_frame_sync();

            // Handle scanner updates in main view
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

    // Static histogram callback (no lambda captures, no heap allocation)
    static void static_histogram_callback(
        const SpectralAnalyzer::HistogramBuffer& histogram,
        uint8_t noise_floor,
        void* user_data
    ) noexcept;

    // ===========================================
    // FIX #5: Button Callbacks
    // ===========================================
    // Diamond Code: Use lambdas with 'this' capture for callbacks
    // Note: std::function may allocate on heap, but this is unavoidable
    // with the Mayhem UI framework's callback design

    // Deferred initialization methods
    void step_deferred_initialization();
    void update_init_progress_display();

    // Stack Usage Monitoring
    /**
     * @brief Check stack usage and log warnings if low
     */
    void check_stack_usage(const char* thread_name, size_t stack_size);

    // ===========================================
    // STAGE 4 FIX: Memory Pressure Monitoring
    // ===========================================
    /**
     * @brief Check memory pressure and log warnings if critical
     */
    void check_memory_pressure();

    // Initialization state variables
    InitState init_state_ = InitState::CONSTRUCTED;
    systime_t init_start_time_ = 0;
    systime_t last_init_progress_ = 0;
    bool initialization_in_progress_ = false;
    
    // Error handling enum
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

// New class for frequency range setup
class FrequencyRangeSetupView : public View {
public:
    FrequencyRangeSetupView(NavigationView& nav, DroneUIController& controller);
    
    void focus() override;

private:
    NavigationView& nav_;
    DroneUIController& controller_;
    
    // Interface widgets
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

class DroneAnalyzerMenuView : public View {
public:
    explicit DroneAnalyzerMenuView(NavigationView& nav);
    ~DroneAnalyzerMenuView() = default;
    
    void focus() override;

private:
    NavigationView& nav_;
    ui::MenuView menu_view_{{0, 16, screen_width, screen_height - 16}};
};

// Forward declaration of integrated settings view
class DroneAnalyzerSettingsView;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_ENHANCED_DRONE_ANALYZER_HPP_
