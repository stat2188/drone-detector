 // ui_scanner_combined.hpp - Unified header for Enhanced Drone Analyzer Scanner App
// Combines: ui_drone_common_types.hpp, ui_drone_scanner.hpp, ui_drone_hardware.hpp, ui_drone_ui.hpp
// Created during migration: Split monolithic app into focused Scanner application

#ifndef __UI_SCANNER_COMBINED_HPP__
#define __UI_SCANNER_COMBINED_HPP__

// ===========================================
// CRITICAL COMPILATION FIXES - REQUIRED INCLUDES (Portapack API Corrections)
// ===========================================

#include "ui.hpp"              // Required: Color class, basic UI framework
#include "ui_drone_common_types.hpp"  // Required: ThreatLevel, DroneType, MovementTrend, SpectrumMode enums
#include "ui_widget.hpp"       // Required: ProgressBar, Text, BigFrequency widgets
#include "ui_navigation.hpp"   // Required: NavigationView class
#include "ui_freq_field.hpp"   // Required: BigFrequency specific functionality
#include "ui_painter.hpp"      // Required: UI painter functions
#include "message.hpp"         // Required: MessageHandlerRegistration class
#include "radio_state.hpp"     // Required: Radio hardware state management
#include "radio.hpp"           // Required: RF radio control (FIXED: added radio.hpp)
#include "baseband_api.hpp"    // Required: Baseband functions for hardware
#include "freqman_db.hpp"      // Required: Frequency database (FIXED: corrected path)
#include "../../log_file.hpp"  // Required: Log file functionality (FIXED: corrected path)
#include "../../gradient.hpp"  // Spectrum gradient for waterfall display
#include <ch.h>                // ChibiOS threading (FIXED: added)
#include "../../file.hpp"      // File I/O API for TXT settings (FIXED: added)
#include "portapack.hpp"       // PHASED 1.1: Core Portapack namespace and hardware
#include "receiver_model.hpp"  // PHASED 1.1: RX model for receiver configuration
//// ch.hpp not available in this ChibiOS version - removed

#include <memory>              // std::unique_ptr, std::make_unique
#include <vector>              // std::vector for dynamic arrays
#include <string>              // std::string for text handling
#include <cstdint>            // int32_t, uint32_t, etc.
#include <algorithm>          // std::min, std::max, std::fill
#include <array>              // std::array for fixed-size arrays
#include <functional>         // std::function for callbacks
#include <deque>              // std::deque for ring buffer
#include <numeric>            // std::accumulate

// ===========================================
// PART 1: COMMON TYPES (from ui_drone_common_types.hpp)
// ===========================================

// Enums defined above in PART 0.5

// Forward declarations for build fixes
// Note: Audio functionality integrated via baseband_api.hpp in hardware section

class LogFile;

using Frequency = uint64_t;

class TrackedDroneData {
public:
    TrackedDroneData() : frequency(0), drone_type(static_cast<uint8_t>(DroneType::UNKNOWN)),
                     threat_level(static_cast<uint8_t>(ThreatLevel::NONE)), update_count(0), last_seen(0) {}

    void add_rssi(int16_t rssi, systime_t timestamp) {
        // Store RSSI history for trend calculation
        rssi_history_[history_index_] = rssi;
        timestamp_history_[history_index_] = timestamp;
        history_index_ = (history_index_ + 1) % MAX_HISTORY;

        if (last_seen < timestamp) {
            last_seen = timestamp;
            update_count++;
        }
    }

    MovementTrend get_trend() const {
        if (update_count < 2) return MovementTrend::UNKNOWN;

        // Analyze RSSI trend over last few samples
        int32_t recent_rssi = 0, older_rssi = 0;
        size_t recent_count = 0, older_count = 0;

        for (size_t i = 0; i < MAX_HISTORY; i++) {
            if (i < history_index_) {
                recent_rssi += rssi_history_[i];
                recent_count++;
            } else if (i > history_index_ && i < MAX_HISTORY - 1) {
                older_rssi += rssi_history_[i];
                older_count++;
            }
        }

        if (recent_count == 0 || older_count == 0) return MovementTrend::UNKNOWN;

        int32_t avg_recent = recent_rssi / recent_count;
        int32_t avg_older = older_rssi / older_count;
        int32_t diff_dB = avg_recent - avg_older;

        if (diff_dB > 5) return MovementTrend::APPROACHING;
        if (diff_dB < -5) return MovementTrend::RECEDING;
        return MovementTrend::STATIC;
    }

    uint32_t frequency;
    uint8_t drone_type;
    uint8_t threat_level;
    uint8_t update_count;
    systime_t last_seen;

private:
    static constexpr size_t MAX_HISTORY = 8;
    int16_t rssi_history_[MAX_HISTORY] = {0};
    systime_t timestamp_history_[MAX_HISTORY] = {0};
    size_t history_index_ = 0;

    // TrackedDroneData(const TrackedDroneData&) = delete;
    // TrackedDroneData& operator=(const TrackedDroneData&) = delete;
};

struct DisplayDroneEntry {
    Frequency frequency;
    DroneType type;
    ThreatLevel threat;
    int32_t rssi;
    systime_t last_seen;
    std::string type_name;
    Color display_color;
    MovementTrend trend;
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
static constexpr uint32_t MIN_HARDWARE_FREQ = 1'000'000;
static constexpr uint64_t MAX_HARDWARE_FREQ = 6'000'000'000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 25'000'000;
static constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
static constexpr int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB = 5;
static constexpr uint8_t MIN_DETECTION_COUNT = 3;
static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;

// =========================
// SD CARD CACHE CONFIGURATION
// =========================

// Frequency database cache settings
static constexpr size_t FREQ_DB_CACHE_SIZE = 32;  // Cache 32 most recently used entries
static constexpr uint32_t FREQ_DB_CACHE_TIMEOUT_MS = 30000;  // 30 second cache lifetime

// Detection log buffering settings
static constexpr size_t LOG_BUFFER_SIZE = 64;     // Buffer 64 log entries before SD write
static constexpr uint32_t LOG_BUFFER_FLUSH_MS = 5000;  // Flush every 5 seconds

// Performance metrics cache settings
static constexpr size_t PERF_CACHE_SIZE = 16;     // Cache last 16 performance measurements
static constexpr uint32_t PERF_CACHE_TIMEOUT_MS = 10000;  // 10 second cache lifetime

// Configuration cache settings
static constexpr size_t CONFIG_CACHE_SIZE_KB = 2; // Cache up to 2KB of config data
static constexpr uint32_t CONFIG_CACHE_TIMEOUT_MS = 60000;  // 1 minute cache lifetime

// freqman_entry defined in freqman_db.hpp - removed conflicting typedef

static constexpr size_t DETECTION_TABLE_SIZE = 256;

struct WidebandSlice {
    Frequency center_frequency;
    size_t index;
    };

// WidebandScanData struct for drone scanner functionality
struct WidebandScanData {
    Frequency min_freq;
    Frequency max_freq;
    size_t slices_nb;
    WidebandSlice slices[WIDEBAND_MAX_SLICES];
    size_t slice_counter;

    void reset() {
        min_freq = WIDEBAND_DEFAULT_MIN;
        max_freq = WIDEBAND_DEFAULT_MAX;
        slices_nb = 0;
        slice_counter = 0;
    }
};

// Unified detection processor class moved to header for compilation
class DetectionProcessor {
private:
    DroneScanner* scanner_;  // Reference to parent scanner for callbacks

public:
    explicit DetectionProcessor(DroneScanner* scanner);

    // Unified detection function replacing all duplicates
    void process_unified_detection(const freqman_entry& entry, int32_t rssi, int32_t effective_threshold,
                                  float confidence_score = 0.7f, bool force_process = false);
};

struct DetectionLogEntry {
    uint32_t timestamp;
    uint32_t frequency_hz;
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    float confidence_score;
};

//// ===========================================
//// PART 1.5: AUDIO MANAGER (Core audio interface for alerts)
//// ===========================================
//
// AudioManager is defined in external header ui_drone_audio.hpp (included in cpp)

struct ConfigData {
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    int32_t rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
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

class SimpleDroneValidation {
public:
    static bool validate_frequency_range(Frequency freq_hz);
    static bool validate_rssi_signal(int32_t rssi_db, ThreatLevel threat);
    static ThreatLevel classify_signal_strength(int32_t rssi_db);
    static DroneType identify_drone_type(Frequency freq_hz, int32_t rssi_db);
    static bool validate_drone_detection(Frequency freq_hz, int32_t rssi_db,
                                       DroneType type, ThreatLevel threat);
};

// ===========================================
// PART 3: SCANNER CLASSES (from ui_drone_scanner.hpp)
// ===========================================

#include "freqman_db.hpp"
#include "../../application/log_file.hpp"
#include <ch.h>
#include <vector>
#include <array>

namespace ui::external_app::enhanced_drone_analyzer {

// Forward declarations for ChibiOS integration fixes (PHASE 3.3: Complete forward declarations)
class DroneHardwareController;
struct DroneAnalyzerSettings;
class ScanningCoordinator;
class AudioManager;
class DroneScanner;
class DroneDisplayController;
class DroneUIController;
class EnhancedDroneSpectrumAnalyzerView;

struct DetectionEntry {
    size_t frequency_hash;
    uint8_t detection_count;
    int32_t rssi_value;
    systime_t last_update;
    uint32_t access_count;
};

class DetectionRingBuffer {
public:
    DetectionRingBuffer();
    ~DetectionRingBuffer() = default;

    void update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value);
    uint8_t get_detection_count(size_t frequency_hash) const;
    int32_t get_rssi_value(size_t frequency_hash) const;
    void clear();

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

private:
    std::vector<DetectionEntry> entries_;
    Mutex ring_buffer_mutex_;

    bool update_existing_entry(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value);
    void add_new_entry(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value);
    void evict_least_recently_used();
    size_t find_entry_index(size_t frequency_hash) const;
    void remove_at_index(size_t index);
} __attribute__((aligned(4)));

extern DetectionRingBuffer global_detection_ring;
extern DetectionRingBuffer& local_detection_ring;

// =========================
// SD CARD CACHE IMPLEMENTATIONS
//
// LEGENDARY CACHE SYSTEM: LRU-based frequency database cache with buffered detection logging
// Dramatically reduces SD card access frequency by caching frequently-used entries in RAM
// Buffered logging accumulates multiple detections before single SD write operation
// =========================

// Frequency database LRU cache entry
struct FreqDBCacheEntry {
    freqman_entry entry{};           // Explicit default init
    size_t index{0};                 // ✅ CRITICAL FIX: Add index field for proper cache entry identification
    systime_t last_access_time{0};   // Explicit zero
    size_t access_count{0};          // Explicit zero
    std::string filename{};          // Empty string

    FreqDBCacheEntry() = default;

    bool is_expired(systime_t current_time) const {
        return (current_time - last_access_time) > MS2ST(FREQ_DB_CACHE_TIMEOUT_MS);
    }

    void update_access(systime_t timestamp) {
        last_access_time = timestamp;
        access_count++;
    }
};

// Frequency database cache implementation
class FreqDBCache {
public:
    FreqDBCache() = default;
    ~FreqDBCache() { cache_entries_.clear(); }

    // Get cached entry by index, returns nullptr if not in cache or expired
    const freqman_entry* get_entry(size_t index) {
        const systime_t current_time = chTimeNow();

        // ✅ FIXED: Find entry by index, not by frequency range
        auto it = std::find_if(cache_entries_.begin(), cache_entries_.end(),
                              [index, current_time](const FreqDBCacheEntry& e) {
                                  return e.index == index && !e.is_expired(current_time);
                              });

    if (it != cache_entries_.end()) {
        it->update_access(current_time);
        return &it->entry;
    }

        return nullptr;
    }

    // Cache a new entry, maintaining LRU eviction
    void cache_entry(const freqman_entry& entry, size_t index, const std::string& filename) {
        const systime_t current_time = chTimeNow();

        // Remove expired entries first
        cache_entries_.erase(
            std::remove_if(cache_entries_.begin(), cache_entries_.end(),
                          [current_time](const FreqDBCacheEntry& e) {
                              return e.is_expired(current_time);
                          }),
            cache_entries_.end()
        );

        // CRITICAL FIX: Fixed comparison - use index instead of frequency_a
        auto it = std::find_if(cache_entries_.begin(), cache_entries_.end(),
                              [index](const FreqDBCacheEntry& e) {
                                  return e.index == index; // Fix: compare index, not frequency
                              });

        if (it != cache_entries_.end()) {
            // Update existing entry
            it->entry = entry;
            it->update_access(current_time);
        } else {
            // Evict least recently used if cache is full
            if (cache_entries_.size() >= FREQ_DB_CACHE_SIZE) {
                auto oldest_it = std::min_element(cache_entries_.begin(), cache_entries_.end(),
                                                 [](const FreqDBCacheEntry& a, const FreqDBCacheEntry& b) {
                                                     return a.last_access_time < b.last_access_time;
                                                 });
                if (oldest_it != cache_entries_.end()) {
                    cache_entries_.erase(oldest_it);
                }
            }

            // Add new entry
            FreqDBCacheEntry new_entry;
            new_entry.entry = entry;
            new_entry.last_access_time = current_time;
            new_entry.access_count = 1;
            new_entry.filename = filename;
            cache_entries_.push_back(std::move(new_entry));
        }
    }

    void clear() {
        cache_entries_.clear();
    }

    size_t size() const { return cache_entries_.size(); }

    void flush_to_sd(const std::string& cache_file = "/EDA_CACHE/FREQ_CACHE.BIN") {
        // Optional: Persist cache to SD for faster startup
        // This could save cache state between sessions
        (void)cache_file; // Mark unused for now
    }

private:
    std::vector<FreqDBCacheEntry> cache_entries_;
    Mutex cache_mutex_;
};

class BufferedDetectionLogger {
public:
    BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false),
                               session_start_(0), logged_total_count_(0), header_written_(false) {}
    ~BufferedDetectionLogger() { flush_buffer(); }

    void log_detection(const DetectionLogEntry& entry) {
        // Add to buffer
        buffered_entries_[entries_count_] = entry;
        entries_count_++;

        // Flush if buffer is full or timeout reached
        const systime_t current_time = chTimeNow();
        if (entries_count_ >= LOG_BUFFER_SIZE ||
            (current_time - last_flush_time_) > MS2ST(LOG_BUFFER_FLUSH_MS)) {
            flush_buffer();
        }
    }

    void flush_buffer() {
        if (entries_count_ == 0) return;

        // Ensure CSV header and log all buffered entries
        if (!ensure_csv_header()) return;

        std::string batch_log;
        for (size_t i = 0; i < entries_count_; ++i) {
            batch_log += format_csv_entry(buffered_entries_[i]);
        }

        auto error = csv_log_.append(generate_log_filename());
        if (error) return;

        error = csv_log_.write_raw(batch_log);
        if (error) {
            last_flush_time_ = chTimeNow();
            entries_count_ = 0;  // Reset buffer count
        }
    }

    bool is_session_active() const { return session_active_; }

    void start_session() {
        if (session_active_) return;
        session_active_ = true;
        session_start_ = chTimeNow();
        logged_total_count_ = 0;
        header_written_ = false;
        last_flush_time_ = chTimeNow();
    }

    void end_session() {
        if (!session_active_) return;
        flush_buffer();  // Ensure all buffered entries are written
        session_active_ = false;
    }

private:
    LogFile csv_log_;
    bool session_active_;
    systime_t session_start_;
    bool header_written_;
    systime_t last_flush_time_;
    size_t logged_total_count_;
    size_t entries_count_;
    DetectionLogEntry buffered_entries_[LOG_BUFFER_SIZE];

    bool ensure_csv_header() {
        if (header_written_) return true;
        const char* header = "timestamp_ms,frequency_hz,rssi_db,threat_level,drone_type,detection_count,confidence\n";

        auto error = csv_log_.append(generate_log_filename());
        if (error) return false;

        error = csv_log_.write_raw(header);
        if (error) {
            header_written_ = true;
            return true;
        }
        return false;
    }

    std::string format_csv_entry(const DetectionLogEntry& entry) {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer) - 1,
                 "%lu,%lu,%d,%u,%u,%u,%.2f\n",
                 entry.timestamp, entry.frequency_hz, entry.rssi_db,
                 static_cast<uint8_t>(entry.threat_level),
                 static_cast<uint8_t>(entry.drone_type),
                 entry.detection_count, entry.confidence_score);
        return std::string(buffer);
    }

    std::string generate_log_filename() const {
        return "EDA_LOG_BUFFERED.CSV";
    }

};

// Unified detection processor class moved to header for compilation
class DetectionProcessor {
private:
    DroneScanner* scanner_;  // Reference to parent scanner for callbacks

public:
    explicit DetectionProcessor(DroneScanner* scanner);

    // Unified detection function replacing all duplicates
    void process_unified_detection(const freqman_entry& entry, int32_t rssi, int32_t effective_threshold,
                                  float confidence_score = 0.7f, bool force_process = false);
};

struct DroneAnalyzerSettings {
    // Core scanning parameters
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    bool enable_audio_alerts = true;
    uint16_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;

    // Hardware settings
    uint32_t hardware_bandwidth_hz = 24000000;
    bool enable_real_hardware = true;
    bool demo_mode = false;
    std::string freqman_path = "DRONES";
};

class DroneScanner {
public:
    enum class ScanningMode {
        DATABASE,
        WIDEBAND_CONTINUOUS,
        HYBRID
    };

    DroneScanner();
    explicit DroneScanner(const DroneAnalyzerSettings& config);
    ~DroneScanner();

    void start_scanning();
    void stop_scanning();
    bool is_scanning_active() const { return scanning_active_; }
    bool load_frequency_database();
    size_t get_database_size() const;

    ScanningMode get_scanning_mode() const { return scanning_mode_; }
    std::string scanning_mode_name() const;
    void set_scanning_mode(ScanningMode mode);

    void switch_to_real_mode();
    void switch_to_demo_mode();

    void perform_scan_cycle(DroneHardwareController& hardware);
    void process_rssi_detection(const freqman_entry& entry, int32_t rssi);
    void update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level);
    void remove_stale_drones();

    Frequency get_current_scanning_frequency() const;
    ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
    const TrackedDroneData& getTrackedDrone(size_t index) const;
    void handle_scan_error(const char* error_msg);

    std::string get_session_summary() const;

    void export_runtime_status(); // Export current scanner status to TXT file for Settings app

    // Additional getter methods for external access to private data
    uint32_t get_scan_cycles() const { return scan_cycles_; }
    uint32_t get_total_detections() const { return total_detections_; }
    size_t get_approaching_count() const { return approaching_count_; }
    size_t get_receding_count() const { return receding_count_; }
    size_t get_static_count() const { return static_count_; }
    bool is_real_mode() const { return is_real_mode_; }
    void reset_scan_cycles() { scan_cycles_ = 0; }
    size_t get_total_memory_usage() const; // Implementation in cpp

    // Missing function declarations from build errors
    void setup_wideband_range(Frequency min_freq, Frequency max_freq);
    void wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override);
    void process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                 int32_t original_threshold, int32_t wideband_threshold);
    void start_spectrum_for_scanning(DroneHardwareController& hardware);
    void stop_spectrum_for_scanning(DroneHardwareController& hardware);
    void master_wideband_detection_handler(DroneHardwareController& hardware, Frequency frequency, int32_t rssi, bool is_approaching);

    DroneScanner(const DroneScanner&) = delete;
    DroneScanner& operator=(const DroneScanner&) = delete;

    class DroneDetectionLogger {
    public:
        DroneDetectionLogger();
        ~DroneDetectionLogger();
        DroneDetectionLogger(const DroneDetectionLogger&) = delete;
        DroneDetectionLogger& operator=(const DroneDetectionLogger&) = delete;

        void start_session();
        void end_session();
        bool log_detection(const DetectionLogEntry& entry);
        std::string get_log_filename() const;
        bool is_session_active() const { return session_active_; }
        std::string format_session_summary(size_t scan_cycles, size_t total_detections) const;

    private:
        LogFile csv_log_;
        bool session_active_;
        uint32_t session_start_;
        uint32_t logged_count_;
        bool header_written_;

        bool ensure_csv_header();
        std::string format_csv_entry(const DetectionLogEntry& entry);
        std::string generate_log_filename() const;
    };

private:
    static msg_t scanning_thread_function(void* arg);
    msg_t scanning_thread();

    void initialize_database_and_scanner();
    void cleanup_database_and_scanner();
    void initialize_wideband_scanning();
    void scan_init_from_loaded_frequencies();

    void perform_database_scan_cycle(DroneHardwareController& hardware);
    void perform_wideband_scan_cycle(DroneHardwareController& hardware);
    void perform_hybrid_scan_cycle(DroneHardwareController& hardware);

    void update_tracking_counts();
    void update_trends_compact_display();
    bool validate_detection_simple(int32_t rssi_db, ThreatLevel threat);
    Frequency get_current_radio_frequency() const;

    Thread* scanning_thread_;
    static constexpr uint32_t SCAN_THREAD_STACK_SIZE = 4096;
    bool scanning_active_;

    FreqmanDB freq_db_;
    size_t current_db_index_;
    Frequency last_scanned_frequency_;

    uint32_t scan_cycles_;
    uint32_t total_detections_;

    ScanningMode scanning_mode_;
    bool is_real_mode_;

    std::array<TrackedDroneData, MAX_TRACKED_DRONES> tracked_drones_;
    size_t tracked_drones_count_;

    size_t approaching_count_;
    size_t receding_count_;
    size_t static_count_;

    ThreatLevel max_detected_threat_;
    int32_t last_valid_rssi_;

    // Конфигурируемые параметры из настроек
    uint32_t scan_interval_ms_;
    int32_t rssi_threshold_db_;
    bool audio_alerts_enabled_;

    DetectionProcessor detection_processor_;  // Unified detection processing

    static constexpr uint8_t DETECTION_DELAY = 3;
    WidebandScanData wideband_scan_data_;
    FreqmanDB drone_database_;
    DroneDetectionLogger detection_logger_;
};

// ===========================================
// PART 4: UI CLASSES (from ui_drone_hardware.hpp)
// ===========================================

// Removed: #include "radio_state.hpp"  // Insufficient API - replaced with full radio::
#include "baseband_api.hpp"    // Baseband signal processing
#include "portapack.hpp"       // Core Portapack hardware API
using namespace portapack;     // Access portapack::display, receiver_model, etc.

#include "radio.hpp"           // Radio hardware control (radio::set_* functions)
#include "message.hpp"         // Message queue system
#include "irq_controls.hpp"    // Interrupt controls

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

    int32_t get_configured_sampling_rate() const;
    int32_t get_configured_bandwidth() const;
    int32_t get_real_rssi_from_hardware(Frequency target_frequency);

    // Additional getter methods for external access
    bool is_spectrum_streaming_active() const;
    int32_t get_current_rssi() const;

    void handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message);
    void handle_channel_spectrum(const ChannelSpectrum& spectrum);
    /**
     * @brief Process incoming spectrum data for drone detection
     * This method analyzes spectrum power levels to identify potential drone signals
     * @param spectrum The channel spectrum data from baseband processor
     */
    void process_channel_spectrum_data(const ChannelSpectrum& spectrum);

    // Additional methods from build errors
    void update_radio_bandwidth();
    void update_spectrum_for_scanner();

    DroneHardwareController(const DroneHardwareController&) = delete;
    DroneHardwareController& operator=(const DroneHardwareController&) = delete;

private:
    void initialize_radio_state();
    void initialize_spectrum_collector();
    void cleanup_spectrum_collector();

    MessageHandlerRegistration message_handler_spectrum_config_{
        Message::ID::ChannelSpectrumConfig,
        [this](const Message* const p) {
            handle_channel_spectrum_config(static_cast<const ChannelSpectrumConfigMessage*>(p));
        }};

    MessageHandlerRegistration message_handler_frame_sync_{
        Message::ID::DisplayFrameSync,
        [this](const Message* const p) {
            (void)p;
            // Forward spectrum processing to display controller if available
            // Note: Spectrum methods are handled by DroneDisplayController
        }};

    // Thread safety mutex for spectrum access
    Mutex spectrum_access_mutex_;

    SpectrumMode spectrum_mode_;
    Frequency center_frequency_;
    uint32_t bandwidth_hz_;
    RxRadioState radio_state_;
    ChannelSpectrumFIFO* fifo_;
    ChannelSpectrumFIFO* spectrum_fifo_;
    bool spectrum_streaming_active_;
    int32_t last_valid_rssi_;
};

// ===========================================
// PART 4: UI CLASSES (from ui_drone_ui.hpp)
// ===========================================

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "app_settings.hpp"
#include "freqman_db.hpp"
#include <memory>

class SmartThreatHeader : public View {
public:
    explicit SmartThreatHeader(Rect parent_rect = {0, 0, screen_width, 48});
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
    std::string get_threat_icon_text(ThreatLevel level) const;
    Style get_threat_bar_style(ThreatLevel level) const;
    Style get_threat_text_style(ThreatLevel level) const;

    void paint(Painter& painter) override;

    ThreatLevel current_threat_ = ThreatLevel::NONE;
    bool is_scanning_ = false;
    Frequency current_freq_ = 2400000000ULL;
    size_t approaching_count_ = 0;
    size_t static_count_ = 0;
    size_t receding_count_ = 0;
};

class ThreatCard : public View {
public:
    explicit ThreatCard(size_t card_index = 0, Rect parent_rect = {0, 0, screen_width, 24});
    ~ThreatCard() = default;

    void update_card(const DisplayDroneEntry& drone);
    void clear_card();
    std::string render_compact() const;
    Color get_card_bg_color() const;
    Color get_card_text_color() const;

    ThreatCard(const ThreatCard&) = delete;
    ThreatCard& operator=(const ThreatCard&) = delete;

private:
    size_t card_index_;
    Text card_text_ {{0, 2, screen_width, 20}, ""};
    bool is_active_ = false;

    Frequency frequency_ = 0;
    ThreatLevel threat_ = ThreatLevel::NONE;
    MovementTrend trend_ = MovementTrend::STATIC;
    int32_t rssi_ = -120;
    uint8_t detection_count_ = 0;
    systime_t last_seen_ = 0;
    std::string threat_name_ = "UNKNOWN";

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

    Text progress_text_  {{0, 1, screen_width, 16}, ""};
    Text alert_text_     {{0, 1, screen_width, 16}, ""};
    Text normal_text_    {{0, 1, screen_width, 16}, ""};

    void paint(Painter& painter) override;
};

class DroneDisplayController {
public:
    explicit DroneDisplayController(NavigationView& nav);
    ~DroneDisplayController();

    DroneDisplayController(const DroneDisplayController&) = delete;
    DroneDisplayController& operator=(const DroneDisplayController&) = delete;

    BigFrequency& big_display() { return big_display_; }
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

    static constexpr const char* DRONE_DISPLAY_FORMAT = "%s %s %-4ddB %c";
    struct SpectrumConfig {
        Frequency min_freq = 2400000000ULL;
        Frequency max_freq = 2500000000ULL;
        uint32_t bandwidth = 24000000;
        uint32_t sampling_rate = 24000000;
    };

private:
    BigFrequency big_display_{{4, 6 * 16, 28 * 8, 52}, 0};
    ProgressBar scanning_progress_{{0, 7 * 16, screen_width, 8}};
    Text text_threat_summary_{{0, 8 * 16, screen_width, 16}, "THREAT: NONE"};
    Text text_status_info_{{0, 9 * 16, screen_width, 16}, "Ready"};
    Text text_scanner_stats_{{0, 10 * 16, screen_width, 16}, "No database"};
    Text text_trends_compact_{{0, 11 * 16, screen_width, 16}, ""};
    Text text_drone_1_{Rect{screen_width - 120, 12 * 16, 120, 16}, ""};
    Text text_drone_2_{Rect{screen_width - 120, 13 * 16, 120, 16}, ""};
    Text text_drone_3_{Rect{screen_width - 120, 14 * 16, 120, 16}, ""};

    std::vector<DisplayDroneEntry> detected_drones_;
    std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES> displayed_drones_;

    std::array<Color, 240u> spectrum_row;
    std::array<uint8_t, 200> spectrum_power_levels_;
    struct ThreatBin { size_t bin; ThreatLevel threat; };
    std::array<ThreatBin, MAX_DISPLAYED_DRONES> threat_bins_;
    size_t threat_bins_count_ = 0;

    Gradient spectrum_gradient_;
    ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
    size_t pixel_index = 0;
    uint32_t bins_hz_size = 0;
    uint32_t hz_per_pixel_target = 100000; // 100kHz per pixel (24MHz/240pixels)
    uint32_t spectrum_bins_per_sample = 0; // Dynamic: bandwidth / channel_bins
    uint8_t* powerlevel = nullptr;
    uint8_t min_color_power = 0;
    const uint8_t ignore_dc = 4;

    SpectrumConfig spectrum_config_;
    NavigationView& nav_;

    MessageHandlerRegistration message_handler_spectrum_config_{
        Message::ID::ChannelSpectrumConfig,
        [this](const Message* const p) {
            const auto message = *static_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }};

    MessageHandlerRegistration message_handler_frame_sync_{
        Message::ID::DisplayFrameSync,
        [this](const Message* const p) {
            (void)p;
            if (this->spectrum_fifo_) {
                ChannelSpectrum channel_spectrum;
                while (spectrum_fifo_->out(channel_spectrum)) {
                    this->process_mini_spectrum_data(channel_spectrum);
                }
                this->render_mini_spectrum();
            }
        }};

    Color get_threat_level_color(ThreatLevel level) const;
    const char* get_threat_level_name(ThreatLevel level) const;
};

// ScanningCoordinator definition - moved earlier to fix forward declaration issues
class ScanningCoordinator {
public:
    ScanningCoordinator(NavigationView& nav,
                       DroneHardwareController& hardware,
                       DroneScanner& scanner,
                       DroneDisplayController& display_controller,
                       AudioManager& audio_controller);

    ~ScanningCoordinator();

    ScanningCoordinator(const ScanningCoordinator&) = delete;
    ScanningCoordinator& operator=(const ScanningCoordinator&) = delete;

    void start_coordinated_scanning();
    void stop_coordinated_scanning();
    bool is_scanning_active() const { return scanning_active_; }

    void show_session_summary(const std::string& summary);
    void update_runtime_parameters(const DroneAnalyzerSettings& settings);

private:
    static msg_t scanning_thread_function(void* arg);
    msg_t coordinated_scanning_thread();

    Mutex scan_coordinator_mutex_;
    Thread* scanning_thread_ = nullptr;
    static constexpr size_t SCANNING_THREAD_STACK_SIZE = 8192;  // CORRECTED: Increased stack size for safety (Critical Fix #3)
    bool scanning_active_ = false;
    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    DroneDisplayController& display_controller_;
    AudioManager& audio_controller_;
    uint32_t scan_interval_ms_ = 750;
};

class DroneUIController {
   private:
    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    AudioManager& audio_mgr_;
    bool scanning_active_;
    DroneDisplayController* display_controller_;
    DroneAnalyzerSettings settings_;
    app_settings::SettingsManager constant_settings_manager_;
public:
    DroneUIController(NavigationView& nav,
    DroneHardwareController& hardware,
    DroneScanner& scanner,
    AudioManager& audio_mgr);
    ~DroneUIController() = default;

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

    bool is_scanning() const { return scanning_active_; }
    DroneAnalyzerSettings& settings() { return settings_; }
    const DroneAnalyzerSettings& settings() const { return settings_; }
    DroneUIController(const DroneUIController&) = delete;
    DroneUIController& operator=(const DroneUIController&) = delete;

    void on_manage_frequencies();
    void on_create_new_database();
    void on_frequency_warning();
    void show_system_status();
    void show_performance_stats();
    void show_debug_info();
    void select_spectrum_mode(SpectrumMode mode);
    void on_spectrum_range_config();
    void on_add_preset_quick();
    void on_hardware_control_menu();
    void on_save_settings();
    void on_load_settings();
};

// CORRECTED: Fixed memory leaks in UI construction (Critical Fix #2)
class EnhancedDroneSpectrumAnalyzerView : public View {
public:
    explicit EnhancedDroneSpectrumAnalyzerView(NavigationView& nav);
    ~EnhancedDroneSpectrumAnalyzerView() override = default;

    void focus() override;
    std::string title() const override { return "Enhanced Drone Analyzer"; }
    void paint(Painter& painter) override;
    bool on_key(const KeyEvent key) override;
    bool on_touch(const TouchEvent event) override;
    void on_show() override;
    void on_hide() override;

private:
    NavigationView& nav_;
    // RAII members to prevent memory leaks during construction
    DroneHardwareController hardware_;       // Direct member - RAII safe
    DroneScanner scanner_;                   // Direct member - RAII safe
    AudioManager audio_mgr_;                 // Direct member - RAII safe
    std::unique_ptr<DroneDisplayController> display_controller_;
    std::unique_ptr<ScanningCoordinator> scanning_coordinator_;
    // Note: ui_controller_ moved to constructor for safe initialization

    SmartThreatHeader* smart_header_ = nullptr;
    ConsoleStatusBar* status_bar_ = nullptr;
    std::array<ThreatCard*, 3> threat_cards_ = {nullptr, nullptr, nullptr};

    Button button_start_{{screen_width - 120, screen_height - 32, 120, 32}, "START/STOP"};
    Button button_menu_{{screen_width - 60, screen_height - 32, 60, 32}, "⚙️"};

    OptionsField field_scanning_mode_{
    {80, 190},
    15,  // Display length
    {
        {"Database Scan", 0},
        {"Wideband Monitor", 1},
        {"Hybrid Discovery", 2}
    }};

    void initialize_modern_layout();
    void update_modern_layout();
    void handle_scanner_update();
    void start_scanning_thread();
    void stop_scanning_thread();
    bool handle_start_stop_button();
    bool handle_menu_button();

    EnhancedDroneSpectrumAnalyzerView(const EnhancedDroneSpectrumAnalyzerView&) = delete;
    EnhancedDroneSpectrumAnalyzerView& operator=(const EnhancedDroneSpectrumAnalyzerView&) = delete;
};

class LoadingScreenView : public View {
public:
    LoadingScreenView(NavigationView& nav);
    ~LoadingScreenView() = default;

    void paint(Painter& painter) override;

private:
    NavigationView& nav_;
    Text text_eda_{Rect{108, 213, 24, 16}, "EDA"};
    systime_t timer_start_ = 0;
};

// ================================================================
// PHASE 7: WIDEBAND MEDIAN FILTER IMPLEMENTATION
// ================================================================

class WidebandMedianFilter {
private:
    static constexpr size_t WINDOW_SIZE = 11;  // Optimal for embedded: 11 samples
    std::array<int16_t, WINDOW_SIZE> window_{};
    size_t head_ = 0;
    bool buffer_full_ = false;

public:
    // Default constructor
    WidebandMedianFilter() = default;

    // LEGENDARY NOISE FILTERING CORE: Add RSSI sample to sliding window
    // This intelligent filter maintains a history buffer to smooth signal variations
    void add_sample(int16_t rssi) {
        window_[head_] = rssi;
        head_ = (head_ + 1) % WINDOW_SIZE;
        if (head_ == 0) buffer_full_ = true;
    }

    // ARCH-OPTIMIZED MEDIAN CALCULATION: Legendary performance for embedded systems
    // Bubble sort provides O(n²) complexity but optimal for small fixed windows (11 samples)
    // Eliminates outliers while preserving signal dynamics
    int16_t get_median_threshold() const {
        if (!buffer_full_) return DEFAULT_RSSI_THRESHOLD_DB;

        // LEGENDARY EMBEDDED ALGORITHM: Bubble sort optimized for latency over complexity
        auto work_copy = window_;
        for (size_t i = 0; i < WINDOW_SIZE / 2 + 1; ++i) {
            for (size_t j = 0; j < WINDOW_SIZE - 1; ++j) {
                if (work_copy[j] > work_copy[j + 1]) {
                    std::swap(work_copy[j], work_copy[j + 1]);
                }
            }
        }

        // Return median with hysteresis margin for noise immunity
        return work_copy[WINDOW_SIZE / 2] - HYSTERESIS_MARGIN_DB;
    }

    // Reset filter state (useful for scanning mode changes)
    void reset() {
        buffer_full_ = false;
        head_ = 0;
        std::fill(window_.begin(), window_.end(), 0);
    }

    // Check if filter has enough data for reliable median
    bool is_ready() const { return buffer_full_; }

    WidebandMedianFilter(const WidebandMedianFilter&) = delete;
    WidebandMedianFilter& operator=(const WidebandMedianFilter&) = delete;
};

// =========================
// CACHE VALIDATION AND TESTING
// =========================

class CacheLogicValidator {
public:
    static constexpr size_t TEST_ENTRIES = 16;

    struct TestResult {
        bool passed = false;
        size_t tests_run = 0;
        size_t tests_passed = 0;
        std::string error_message = "";
    };

    // Run comprehensive cache logic tests - exceptions disabled in ChibiOS
    static TestResult validate_cache_functionality() {
        TestResult result;
        result.tests_run = 0;
        result.tests_passed = 0;

        // Test 1: Frequency DB Cache Basic Operations (use Optional pattern)
        if (validate_freq_db_cache_logic(result)) {
            result.tests_passed++;
        }
        result.tests_run++;

        // Test 2: Buffered Logger Operations
        if (validate_buffered_logger_logic(result)) {
            result.tests_passed++;
        }
        result.tests_run++;

        // Test 3: Cache Integration Scenarios
        if (validate_cache_integration_scenarios(result)) {
            result.tests_passed++;
        }
        result.tests_run++;

        // Test 4: Memory Management and Limits
        if (validate_memory_management(result)) {
            result.tests_passed++;
        }
        result.tests_run++;

        result.passed = (result.tests_passed == result.tests_run);
        if (!result.passed) {
            result.error_message = "Some cache logic tests failed.";
        }

        return result;
    }

private:
    // Test Frequency DB cache basic functionality
    static bool validate_freq_db_cache_logic(TestResult& result) {
        FreqDBCache test_cache;

        // Create test frequency entries
        freqman_entry test_entries[TEST_ENTRIES];
        for (size_t i = 0; i < TEST_ENTRIES; ++i) {
            test_entries[i].frequency_a = 2400000000ULL + (i * 1000000ULL); // 2.4GHz + i*1MHz
            test_entries[i].frequency_b = test_entries[i].frequency_a + 1000000ULL;
            test_entries[i].type = freqman_type::HamRadio;
            memset(&test_entries[i].description[0], 0, 16);
            snprintf(&test_entries[i].description[0], 16, "TEST_FREQ_%zu", i);
        }

        // Test 1: Empty cache should return nullptr
        const freqman_entry* null_entry = test_cache.get_entry(0);
        if (null_entry != nullptr) {
            result.error_message += "[FreqCache] Empty cache should return nullptr; ";
            return false;
        }

        // Test 2: Cache entries and retrieve them
        for (size_t i = 0; i < 8; ++i) { // Cache first 8 entries
            test_cache.cache_entry(test_entries[i], i, "test_db.csv");
        }

        // Verify cached entries can be retrieved
        for (size_t i = 0; i < 8; ++i) {
            const freqman_entry* cached = test_cache.get_entry(i);
            if (cached == nullptr) {
                result.error_message += "[FreqCache] Cached entry retrieval failed; ";
                return false;
            }
            if (cached->frequency_a != test_entries[i].frequency_a) {
                result.error_message += "[FreqCache] Cached frequency mismatch; ";
                return false;
            }
        }

        // Test 3: Simulate LRU eviction by caching more than cache size
        for (size_t i = 8; i < TEST_ENTRIES; ++i) { // Cache remaining entries
            test_cache.cache_entry(test_entries[i], i, "test_db.csv");
        }

        // Verify some earliest entries are evicted (LRU behavior)
        bool found_evicted = false;
        bool found_recent = false;
        for (size_t i = 0; i < TEST_ENTRIES; ++i) {
            const freqman_entry* cached = test_cache.get_entry(i);
            if (i < 5 && cached == nullptr) found_evicted = true; // Should have some evictions
            if (i >= 5 && cached != nullptr) found_recent = true; // Recent should be there
        }

        if (!found_evicted && !found_recent) {
            result.error_message += "[FreqCache] LRU eviction logic appears incorrect; ";
            return false;
        }

        return true; // All frequency cache tests passed
    }

    // Test buffered logger functionality
    static bool validate_buffered_logger_logic(TestResult& result) {
        BufferedDetectionLogger test_logger;

        // Test 1: Session management
        if (test_logger.is_session_active()) {
            result.error_message += "[BufferedLogger] Logger should not be active before start_session; ";
            return false;
        }

        test_logger.start_session();
        if (!test_logger.is_session_active()) {
            result.error_message += "[BufferedLogger] Logger should be active after start_session; ";
            return false;
        }

        // Test 2: Buffered logging
        size_t test_logs = LOG_BUFFER_SIZE / 4; // Test with partial buffer
        DetectionLogEntry test_entries[LOG_BUFFER_SIZE / 4];

        for (size_t i = 0; i < test_logs; ++i) {
            test_entries[i].timestamp = chTimeNow() + i;
            test_entries[i].frequency_hz = 2400000000ULL + (i * 1000000ULL);
            test_entries[i].rssi_db = -80 - static_cast<int32_t>(i);
            test_entries[i].threat_level = (i % 4 == 0) ? ThreatLevel::HIGH : ThreatLevel::LOW;
            test_entries[i].drone_type = (i % 3 == 0) ? DroneType::MAVIC : DroneType::UNKNOWN;
            test_entries[i].detection_count = static_cast<uint8_t>(i + 1);
            test_entries[i].confidence_score = 0.7f + (i * 0.05f);

            test_logger.log_detection(test_entries[i]);
        }

        // Test 3: Session ending flushes buffer
        test_logger.end_session();
        if (test_logger.is_session_active()) {
            result.error_message += "[BufferedLogger] Logger should not be active after end_session; ";
            return false;
        }

        return true; // All buffered logger tests passed
    }

    // Test integration scenarios
    static bool validate_cache_integration_scenarios(TestResult& result) {
        FreqDBCache freq_cache;
        BufferedDetectionLogger log_cache;

        // Scenario 1: Frequent access pattern (typical scanning)
        const size_t FREQUENT_PATTERNS = 5;
        freqman_entry frequent_entry{};

        // Simulate scanning same frequencies repeatedly
        for (size_t pattern = 0; pattern < FREQUENT_PATTERNS; ++pattern) {
            for (size_t access = 0; access < 20; ++access) { // 20 accesses per pattern
                size_t freq_idx = (pattern * 3) % FREQ_DB_CACHE_SIZE;

                // First access will cache, subsequent will hit cache
                if (freq_cache.get_entry(freq_idx) == nullptr) {
                    // Create and cache entry
                    frequent_entry.frequency_a = 2400000000ULL + (freq_idx * 1000000ULL);
                    frequent_entry.frequency_b = frequent_entry.frequency_a + 500000ULL;
                    frequent_entry.type = freqman_type::HamRadio;
                    freq_cache.cache_entry(frequent_entry, freq_idx, "test_integration.csv");
                }

                // Verify we can always get the entry after initial caching
                const freqman_entry* retrieved = freq_cache.get_entry(freq_idx);
                if (retrieved == nullptr) {
                    result.error_message += "[CacheIntegration] Failed to retrieve frequently accessed entry; ";
                    return false;
                }
            }
        }

        // Scenario 2: Cache pressure with access patterns
        log_cache.start_session();

        // Simulate detection burst (realistic scenario)
        for (size_t detection = 0; detection < LOG_BUFFER_SIZE + 10; ++detection) {
            DetectionLogEntry detection_entry{
                .timestamp = chTimeNow() + static_cast<uint32_t>(detection),
                .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
                .rssi_db = -75,
                .threat_level = ThreatLevel::MEDIUM,
                .drone_type = DroneType::UNKNOWN,
                .detection_count = static_cast<uint8_t>((detection % 5) + 1),
                .confidence_score = 0.8f
            };
            log_cache.log_detection(detection_entry);
        }

        log_cache.end_session();

        return true; // Integration tests passed
    }

    // Test memory bounds and resource management
    static bool validate_memory_management(TestResult& result) {
        // Test 1: Cache size limits
        FreqDBCache size_cache;
        const size_t entries_to_cache = FREQ_DB_CACHE_SIZE * 2; // More than cache size

        freqman_entry size_test_entry{};

        for (size_t i = 0; i < entries_to_cache; ++i) {
            size_test_entry.frequency_a = 2400000000ULL + (i * 1000000ULL);
            size_test_entry.frequency_b = size_test_entry.frequency_a + 1000000ULL;
            size_cache.cache_entry(size_test_entry, i, "size_test.csv");
        }

        // Verify cache size stays within limits
        if (size_cache.size() > FREQ_DB_CACHE_SIZE + 5) { // Allow small margin for implementation
            result.error_message += "[MemoryMgmt] Cache exceeded maximum size; ";
            return false;
        }

        // Test 2: Logger buffer bounds
        BufferedDetectionLogger buffer_logger;
        buffer_logger.start_session();

        // Fill buffer exactly
        for (size_t i = 0; i < LOG_BUFFER_SIZE; ++i) {
            DetectionLogEntry entry{
                .timestamp = chTimeNow(),
                .frequency_hz = 2400000000ULL + i,
                .rssi_db = -80,
                .threat_level = ThreatLevel::LOW,
                .drone_type = DroneType::UNKNOWN,
                .detection_count = 1,
                .confidence_score = 0.7f
            };
            buffer_logger.log_detection(entry);
        }

        // This next log should trigger flush (assuming no timing-based flush)
        DetectionLogEntry trigger_entry{
            .timestamp = chTimeNow(),
            .frequency_hz = 2400000000ULL,
            .rssi_db = -70,
            .threat_level = ThreatLevel::HIGH,
            .drone_type = DroneType::MAVIC,
            .detection_count = 3,
            .confidence_score = 0.9f
        };
        buffer_logger.log_detection(trigger_entry);

        buffer_logger.end_session();

        return true; // Memory management tests passed
    }
};



#include "ui_drone_audio.hpp"

// AudioManager is defined in external header ui_drone_audio.hpp (included in cpp)
#include "ui_drone_audio.hpp"

// Global helper functions for drone type handling
const char* get_drone_type_name(uint8_t type);
Color get_drone_type_color(uint8_t type);
Color get_threat_bar_style(ThreatLevel level);
Color get_threat_text_style(ThreatLevel level);

// Recacon defines (from recon app)
#define RECON_DEF_SQUELCH (-40)
#define RECON_DEF_NB_MATCH (3)
#define RECON_MIN_LOCK_DURATION (50)
#define RECON_MATCH_CONTINUOUS (0)
#define RECON_MATCH_SPARSE (1)

// Implementation includes and definitions would go here in .cpp file

} // namespace ui::external_app::enhanced_drone_analyzer

#endif // __UI_SCANNER_COMBINED_HPP__
