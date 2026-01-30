#ifndef UI_SCANNER_COMBINED_HPP_
#define UI_SCANNER_COMBINED_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <atomic>
#include <memory>

#include "ui_drone_common_types.hpp"
#include "ui_signal_processing.hpp"
#include "scanner_settings.hpp"
#include "scanning_coordinator.hpp"
#include "gradient.hpp"
#include "ui_drone_audio.hpp"
#include "ui_spectral_analyzer.hpp"
#include "drone_constants.hpp"

#include "ui.hpp"
#include "event_m0.hpp"
#include "ui_widget.hpp"
#include "message.hpp"

#include "freqman_db.hpp"
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

using Frequency = uint64_t;

// Enum for scanner modes
enum class ScannerMode {
    DATABASE_ONLY,
    WIDEBAND_ONLY,
    HYBRID,
    SPECTRUM_VIEW
};

// Application specific namespace starts here to ensure all classes are properly scoped
namespace ui::apps::enhanced_drone_analyzer {

struct preset_entry {
    Frequency min = 0;
    Frequency max = 0;
    std::string label;
};

struct RssiMeasurement {
    int16_t rssi_db;
    systime_t timestamp_ms;
};

static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -80;
static constexpr uint32_t ALERT_PERSISTENCE_THRESHOLD = 3;

namespace UIStyles {
    static constexpr Style RED_STYLE{font::fixed_8x16, Color::black(), Color::red()};
    static constexpr Style YELLOW_STYLE{font::fixed_8x16, Color::black(), Color(255, 255, 0)};
    static constexpr Style GREEN_STYLE{font::fixed_8x16, Color::black(), Color::green()};
    static constexpr Style LIGHT_STYLE{font::fixed_8x16, Color::black(), Color::white()};
    static constexpr Style DARK_STYLE{font::fixed_8x16, Color::black(), Color::dark_grey()};
    static constexpr Style ORANGE_STYLE{font::fixed_8x16, Color::black(), Color(255, 165, 0)};
}

class TrackedDrone {
public:
    TrackedDrone() : frequency(0), drone_type(static_cast<uint8_t>(DroneType::UNKNOWN)),
                     threat_level(static_cast<uint8_t>(ThreatLevel::NONE)), update_count(0),
                     last_seen(0), rssi(-120), history_index_(0) {
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

struct DronePreset {
    std::string display_name;
    std::string name_template;
    Frequency frequency_hz;
    ThreatLevel threat_level;
    DroneType drone_type;

    bool is_valid() const {
        return !display_name.empty() && frequency_hz > 0;
    }
};

static constexpr size_t MAX_TRACKED_DRONES = 8;
static constexpr size_t MAX_DISPLAYED_DRONES = 3;
static constexpr size_t MINI_SPECTRUM_WIDTH = 200;
static constexpr size_t MINI_SPECTRUM_HEIGHT = 24;
static constexpr int SPEC_HEIGHT = 32;
static constexpr int SPEC_WIDTH = 240;
static constexpr uint32_t MIN_HARDWARE_FREQ = 1'000'000;
static constexpr uint64_t MAX_HARDWARE_FREQ = 7'200'000'000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 22'000'000; // Optimized for 2.4GHz band coverage
static constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
static constexpr size_t DETECTION_TABLE_SIZE = 256;

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

struct DetectionLogEntry {
    uint32_t timestamp;
    uint64_t frequency_hz;  // uint64_t for frequencies > 4GHz
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    uint8_t confidence_percent;  // Integer 0-100% instead of float for memory efficiency
    uint8_t width_bins;           // Signal width in bins (for calibration)
    uint32_t signal_width_hz;     // Signal width in Hz (for calibration)
    uint8_t snr;                  // Signal-to-Noise Ratio (for calibration)
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

struct ConfigData {
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    int32_t rssi_threshold_db = -90;
    uint32_t scan_interval_ms = 1000;
    bool enable_audio_alerts = true;
    std::string freqman_path = "DRONES";
};

class ScannerConfig {
public:
    explicit ScannerConfig(ConfigData config = {});
    ~ScannerConfig() = default;

    bool load_from_file(const std::string& filepath);
    bool save_to_file(const std::string& filepath) const;

    const ConfigData& get_data() const { return config_data_; }
    ConfigData& get_data() { return config_data_; }

    void set_frequency_range(uint32_t min_hz, uint32_t max_hz);
    void set_rssi_threshold(int32_t threshold);
    void set_scan_interval(uint32_t interval_ms);
    void set_audio_alerts(bool enabled);
    void set_freqman_path(const std::string& path);

    void set_scanning_mode(const std::string& mode);
    bool is_valid() const;

    ScannerConfig(const ScannerConfig&) = delete;
    ScannerConfig& operator=(const ScannerConfig&) = delete;

private:
    ConfigData config_data_;
};

// Struct for drone signal parameters (prevents easily-swappable-parameters warning)
struct DroneSignal {
    Frequency frequency_hz;
    int32_t rssi_db;
};

// Moved inside namespace to fix compilation error
class SimpleDroneValidation {
public:
    static bool validate_frequency_range(Frequency freq_hz);
    static bool validate_rssi_signal(int32_t rssi_db, ThreatLevel threat);
    static ThreatLevel classify_signal_strength(int32_t rssi_db);
    static DroneType identify_drone_type(const DroneSignal& signal);
    static bool validate_drone_detection(const DroneSignal& signal,
                                       DroneType type, ThreatLevel threat);
    
    // Mode for frequency validation
    static void set_scanning_mode(DroneConstants::ScanningMode mode) {
        scanning_mode_ = mode;
    }
    
    static DroneConstants::ScanningMode get_scanning_mode() {
        return scanning_mode_;
    }

private:
    static DroneConstants::ScanningMode scanning_mode_;
};

// 🔴 NEW: Enhanced Settings Validation with detailed checks
class EnhancedDroneSettingsValidator {
public:
    struct ValidationResult {
        bool is_valid;
        std::string error_message;
        uint32_t warning_count;
        
        ValidationResult() : is_valid(true), warning_count(0) {}
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

    // Complete built-in database of known drone frequencies (2025)
    static const std::vector<BuiltinDroneFreq> BUILTIN_DRONE_DB;

public:
    enum class ScanningMode {
        DATABASE,
        WIDEBAND_CONTINUOUS,
        HYBRID
    };

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
        if (max_freq > 7200000000ULL) max_freq = 7200000000ULL;
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

    Frequency get_current_scanning_frequency() const;
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

    const char* get_drone_type_name(DroneType type) const {
        switch (type) {
            case DroneType::MAVIC: return "MAVIC";
            case DroneType::DJI_P34: return "DJI P34";
            case DroneType::UNKNOWN: default: return "UNKNOWN";
        }
    }
    Color get_drone_type_color(DroneType type) const {
        switch (type) {
            case DroneType::MAVIC: return Color::red();
            case DroneType::DJI_P34: return Color::orange();
            case DroneType::UNKNOWN: default: return Color::white();
        }
    }

    Frequency get_current_radio_frequency() const;

    struct DroneSnapshot {
        TrackedDrone drones[MAX_TRACKED_DRONES];
        size_t count = 0;
    };

    DroneSnapshot get_tracked_drones_snapshot() const;

    bool try_get_tracked_drones_snapshot(DroneSnapshot& out_snapshot) const;

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

    void update_trends_compact_display();
    bool validate_detection_simple(int32_t rssi_db, ThreatLevel threat);

    static msg_t scanning_thread_function(void* arg);
    msg_t scanning_thread();

    void initialize_database_and_scanner();
    void cleanup_database_and_scanner();
    void scan_init_from_loaded_frequencies();

    void perform_database_scan_cycle(DroneHardwareController& hardware);
    void perform_wideband_scan_cycle(DroneHardwareController& hardware);
    void perform_hybrid_scan_cycle(DroneHardwareController& hardware);

    void update_tracking_counts();

    // Intelligent scanning methods
    size_t get_next_slice_with_intelligence();
    void update_frequency_predictions(Frequency detected_freq, ThreatLevel threat_level);
    void update_priority_slice_detection(size_t slice_idx, bool detected_something_interesting);

    Thread* scanning_thread_ = nullptr;
    mutable Mutex data_mutex;
    std::atomic<bool> scanning_active_{false};

    FreqmanDB freq_db_;
    size_t current_db_index_ = 0;
    Frequency last_scanned_frequency_ = 0;
    bool freq_db_loaded_ = false;

    std::atomic<uint32_t> scan_cycles_{0};
    std::atomic<uint32_t> total_detections_{0};

    ScanningMode scanning_mode_ = ScanningMode::DATABASE;
    std::atomic<bool> is_real_mode_{true};

    TrackedDrone tracked_drones_[MAX_TRACKED_DRONES];
    size_t tracked_count_ = 0;

    size_t approaching_count_ = 0;
    size_t receding_count_ = 0;
    size_t static_count_ = 0;

    ThreatLevel max_detected_threat_ = ThreatLevel::NONE;
    int32_t last_valid_rssi_ = -120;

    static constexpr uint8_t DETECTION_DELAY = 2;
    WidebandScanData wideband_scan_data_;
    static constexpr size_t MAX_DB_ENTRIES = 128;
    std::array<freqman_entry, MAX_DB_ENTRIES> drone_database_;
    size_t db_entry_count_ = 0;
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
    
    // 🔴 FIX: TOCTOU race condition - atomic check-and-fetch method
    bool get_rssi_if_fresh(int32_t& out_rssi);

    // NEW: Spectrum data access method (atomic check-and-fetch to avoid TOCTOU race)
    bool get_latest_spectrum_if_fresh(std::array<uint8_t, 256>& out_db_buffer);
    bool try_get_latest_spectrum(std::array<uint8_t, 256>& out_db_buffer);
    void clear_spectrum_flag();

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

    MessageHandlerRegistration message_handler_spectrum_config_ {
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }
    };
    
    MessageHandlerRegistration message_handler_frame_sync_ {
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
        }
    };
    
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
};

enum class DisplayMode { SCANNING, ALERT, NORMAL };

class ConsoleStatusBar : public View {
public:
    explicit ConsoleStatusBar(size_t bar_index = 0, Rect parent_rect = {0, 0, screen_width, 16});
    ~ConsoleStatusBar() = default;

    void update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles = 0, uint32_t detections = 0);
    void update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg);
    void update_normal_status(const std::string& primary, const std::string& secondary);
    void set_display_mode(DisplayMode mode);

    ConsoleStatusBar(const ConsoleStatusBar&) = delete;
    ConsoleStatusBar& operator=(const ConsoleStatusBar&) = delete;

private:
    size_t bar_index_;
    DisplayMode mode_ = DisplayMode::NORMAL;
    Rect parent_rect_;

    Text progress_text_  {{0, 1, screen_width, 16}, ""};
    Text alert_text_     {{0, 1, screen_width, 16}, ""};
    Text normal_text_    {{0, 1, screen_width, 16}, ""};

    void paint(Painter& painter) override;
};

class DroneDisplayController : public View {
public:
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

    void update_detection_display(const DroneScanner& scanner);
    void update_trends_display(size_t approaching, size_t static_count, size_t receding);
    void set_scanning_status(bool active, const std::string& message);
    void set_frequency_display(Frequency freq);
    void add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi);
    void update_drones_display(const DroneScanner& scanner);
    void sort_drones_by_rssi();
    void render_drone_text_display();

    void initialize_mini_spectrum();
    void process_mini_spectrum_data(const ChannelSpectrum& spectrum);
    bool process_bins(uint8_t* power_level);
    void render_mini_spectrum();
    void highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES>& drones);
    size_t frequency_to_spectrum_bin(Frequency freq_hz) const;
    void clear_spectrum_buffers();
    bool validate_spectrum_data() const;
    size_t get_safe_spectrum_index(size_t x, size_t y) const;

    void set_spectrum_range(Frequency min_freq, Frequency max_freq);
    void update_signal_type_display(const std::string& signal_type);

    DroneDisplayController(const DroneDisplayController&) = delete;
    DroneDisplayController& operator=(const DroneDisplayController&) = delete;

    static constexpr const char* DRONE_DISPLAY_FORMAT = "%s %s %-4lddB %c";
    struct SpectrumConfig {
        Frequency min_freq = 2400000000ULL;
        Frequency max_freq = 2500000000ULL;
        uint32_t bandwidth = 24000000;
        uint32_t sampling_rate = 24000000;
    };

private:
    Text big_display_{{4, 0, 28 * 8, 52}, ""};
    ProgressBar scanning_progress_{{0, 52, screen_width, 8}};
    Text text_threat_summary_{{0, 70, screen_width, 16}, "THREAT: NONE"};
    Text text_status_info_{{0, 86, screen_width, 16}, "Ready"};
    Text text_scanner_stats_{{0, 102, screen_width, 16}, "No database"};
    Text text_trends_compact_{{0, 118, screen_width, 16}, ""};
    Text text_drone_1_{{screen_width - 120, 134, 120, 16}, ""};
    Text text_drone_2_{{screen_width - 120, 150, 120, 16}, ""};
    Text text_drone_3_{{screen_width - 120, 166, 120, 16}, ""};
    Text text_signal_type_{{screen_width - 80, 80, 80, 16}, "SIGNAL: --"};  // Debug: Signal type marker

    static constexpr size_t MAX_UI_DRONES = 16;
    std::array<DisplayDroneEntry, MAX_UI_DRONES> detected_drones_;
    size_t detected_drones_count_ = 0;
    std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES> displayed_drones_;

    std::array<Color, 240u> spectrum_row;
    std::array<uint8_t, 200> spectrum_power_levels_;
    struct ThreatBin { size_t bin; ThreatLevel threat; };
    std::array<ThreatBin, MAX_DISPLAYED_DRONES> threat_bins_;
    size_t threat_bins_count_ = 0;

    std::array<std::array<uint8_t, SPEC_WIDTH>, SPEC_HEIGHT> waterfall_buffer_;
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

    MessageHandlerRegistration message_handler_spectrum_config_ {
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }
    };
    
    MessageHandlerRegistration message_handler_frame_sync_ {
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
        }
    };

    const char* get_drone_type_name(DroneType type) const;
    Color get_drone_type_color(DroneType type) const;
    Color get_threat_level_color(ThreatLevel level) const;
    const char* get_threat_level_name(ThreatLevel level) const;

    void get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power);
    void add_spectrum_pixel(uint8_t power);
};

static constexpr const char* DEFAULT_CONFIG_PATH = "DRONES/DATA.CFG";
static constexpr const char* FALLBACK_CONFIG_PATH = "APP/SETTINGS/DRONES.CFG";
static constexpr uint32_t DEFAULT_BANDWIDTH = 24000000UL;
static constexpr Frequency DEFAULT_CENTER_FREQUENCY = 2400000000ULL;
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

    DroneUIController(const DroneUIController&) = delete;
    DroneUIController& operator=(const DroneUIController&) = delete;

private:
    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    ::AudioManager& audio_mgr_;
    bool scanning_active_ = false;
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
    std::string title() const override { return "Enhanced Drone Analyzer"; };

    void paint(Painter& painter) override;
    bool on_key(const KeyEvent key) override;
    bool on_touch(const TouchEvent event) override;
    void on_show() override;
    void on_hide() override;

private:
    NavigationView& nav_;

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
    ::ui::apps::enhanced_drone_analyzer::DroneAnalyzerSettings settings_;

    MessageHandlerRegistration message_handler_stats_ {
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
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

class DroneSettingsMenuView : public View {
public:
    DroneSettingsMenuView(NavigationView& nav, DroneUIController& controller);

    void focus() override;
    std::string title() const override { return "Settings"; };

private:
    NavigationView& nav_;  // Added NavigationView reference
    DroneUIController& controller_;

    Button button_load_db_   { {16, 16,  208, 40}, "Load Freq Database" };
    Button button_audio_     { {16, 64,  208, 40}, "Audio Alerts: Toggle" };
    Button button_hw_        { {16, 112, 208, 40}, "Hardware Info" };
    Button button_freq_range_{ {16, 160, 208, 40}, "Set Freq Range" };
    Button button_logs_      { {16, 208, 208, 40}, "View CSV Logs" };
    Button button_about_     { {16, 256, 208, 40}, "About EDA" };

    Text text_info_          { {16, 304, 208, 16}, "Ver: 0.3 | Mayhem" };
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __UI_SCANNER_COMBINED_HPP__
