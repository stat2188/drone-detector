// ui_scanner_combined.hpp - Unified header for Enhanced Drone Analyzer Scanner App
// Combines: ui_drone_common_types.hpp, ui_drone_scanner.hpp, ui_drone_hardware.hpp, ui_drone_ui.hpp
// Created during migration: Split monolithic app into focused Scanner application

#ifndef __UI_SCANNER_COMBINED_HPP__
#define __UI_SCANNER_COMBINED_HPP__

// ===========================================
// PART 1: COMMON TYPES AND IMPORTS
// ===========================================

#include <cstdint>
#include <string>
#include <vector>
#include <array>

// Include shared utilities
#include "ui_drone_common_types.hpp"
#include "ui_signal_processing.hpp"
#include "scanner_settings.hpp"
// Ensure SpectrumMode enum is available
enum class SpectrumMode;

#include "gradient.hpp"

// Fixed include paths for proper compilation
#include <memory>

#include "ui_drone_audio.hpp"
#include "scanning_coordinator.hpp"

// Forward declaration for AudioManager (defined in ui_drone_audio.hpp)
class AudioManager;

// Include necessary headers for Color and MessageHandlerRegistration - Fixed paths
#include "ui.hpp"  // for Color
#include "../event_m0.hpp"   // for MessageHandlerRegistration
#include "ui_widget.hpp"  // for Button, Text, OptionsField
#include "message.hpp"  // for Message::ID etc

// Move includes outside namespace to avoid std pollution
#include "freqman_db.hpp"
#include "log_file.hpp"
#include <ch.h>

#include "radio_state.hpp"
#include "baseband_api.hpp"
#include "portapack.hpp"
#include "radio.hpp"
#include "message.hpp"
#include "irq_controls.hpp"

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_widget.hpp"  // For Button, Text, etc.
#include "app_settings.hpp"
#include "string_format.hpp"
#include "tone_key.hpp"
#include "message_queue.hpp"

class LogFile;

using Frequency = uint64_t;

// Preset entry for frequency ranges
struct preset_entry {
    Frequency min = 0;
    Frequency max = 0;
    std::string label;
};

// Audio alert system migrated from Looking Glass - defined in ui_drone_audio.hpp

// Constants (no duplicates)
static constexpr uint8_t LOOKING_GLASS_MAX_IQ_PHASE_CAL = 63;
static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -80;
static constexpr uint32_t ALERT_PERSISTENCE_THRESHOLD = 3;
static constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;
// Constants moved to ui_drone_common_types.hpp to avoid duplicates
static constexpr uint8_t MIN_DETECTION_COUNT = 3;
static constexpr uint32_t SCANNING_THREAD_STACK_SIZE = 2048;
// Undefine macros from ui_looking_glass_app.hpp to avoid conflicts
#undef LOOKING_GLASS_SINGLEPASS
#undef LOOKING_GLASS_FASTSCAN
#undef LOOKING_GLASS_SLOWSCAN
#undef SPEC_NB_BINS
#undef LOOKING_GLASS_SLICE_WIDTH_MAX
#undef LOOKING_GLASS_MAX_SAMPLERATE
#undef MHZ_DIV
static constexpr int LOOKING_GLASS_SINGLEPASS = 0;
static constexpr int LOOKING_GLASS_FASTSCAN = 1;
static constexpr int LOOKING_GLASS_SLOWSCAN = 2;
static constexpr size_t SPEC_NB_BINS = 256;
static constexpr uint32_t LOOKING_GLASS_SLICE_WIDTH_MAX = 24000000;
static constexpr uint32_t LOOKING_GLASS_MAX_SAMPLERATE = 24000000;
static constexpr uint32_t MHZ_DIV = 1000000;

// Audio alert system migrated from Looking Glass - defined in ui_drone_audio.hpp


class TrackedDrone {
public:
    TrackedDrone() : frequency(0), drone_type(static_cast<uint8_t>(DroneType::UNKNOWN)),
                     threat_level(static_cast<uint8_t>(ThreatLevel::NONE)), update_count(0),
                     last_seen(0) {
        // Initialize array with "silence" (-120 dBm), not zeros
        for(auto& r : rssi_history_) r = -120;
    }

    TrackedDrone& operator=(const TrackedDrone& other) {
        if (this != &other) {
            frequency = other.frequency;
            drone_type = other.drone_type;
            threat_level = other.threat_level;
            update_count = other.update_count;
            last_seen = other.last_seen;
            rssi = other.rssi;
            // Copy arrays manually or via std::copy
            for(size_t i=0; i<MAX_HISTORY; i++) {
                rssi_history_[i] = other.rssi_history_[i];
                timestamp_history_[i] = other.timestamp_history_[i];
            }
            history_index_ = other.history_index_;
        }
        return *this;
    }

    void add_rssi(int16_t new_rssi, systime_t timestamp) {
        rssi_history_[history_index_] = new_rssi;
        timestamp_history_[history_index_] = timestamp;
        history_index_ = (history_index_ + 1) % MAX_HISTORY;

        this->rssi = new_rssi;

        // Update last_seen only if this is actually a new event
        if (timestamp > last_seen) {
            last_seen = timestamp;
            // Overflow protection for update_count is not necessary for uint8, but logical
            if (update_count < 255) update_count++;
        }
    }

    MovementTrend get_trend() const {
        // Need at least 4 measurements for adequate trend
        if (update_count < 4) return MovementTrend::UNKNOWN;

        int32_t recent_sum = 0;
        int32_t older_sum = 0;
        size_t half_window = MAX_HISTORY / 2;

        // Go through buffer in chronological order
        // i=0 - oldest, i=MAX-1 - newest
        for (size_t i = 0; i < MAX_HISTORY; i++) {
            // Calculate physical index in ring buffer
            // (history_index_ points to NEXT write position,
            // so history_index_ - 1 is the most recent).
            // Formula for getting index from oldest to newest:
            size_t logical_idx = (history_index_ + i) % MAX_HISTORY;

            int16_t val = rssi_history_[logical_idx];

            // Ignore "empty" initialization values if buffer is not yet full
            if (val <= -110) continue;

            if (i < half_window) {
                older_sum += val;
            } else {
                recent_sum += val;
            }
        }

        // Simplified average: divide by 4 (since MAX_HISTORY=8, half=4)
        // If update_count < 8, division will not be entirely accurate but acceptable for trend
        int32_t avg_old = older_sum / (int32_t)half_window;
        int32_t avg_new = recent_sum / (int32_t)half_window;

        int32_t diff = avg_new - avg_old;

        // Sensitivity threshold: 3dB (5dB might be too much for smooth approaching)
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
    int16_t rssi_history_[MAX_HISTORY]; // Initialized in constructor
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
// Constants for spectrum buffer
static constexpr int SPEC_HEIGHT = 32; // Экономия 7.5 КБ RAM!
static constexpr int SPEC_WIDTH = 240;
static constexpr uint32_t MIN_HARDWARE_FREQ = 1'000'000;
static constexpr uint64_t MAX_HARDWARE_FREQ = 7'200'000'000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 25'000'000;
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
    uint32_t frequency_hz;
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    float confidence_score;
};

// Message structures for thread-safe communication
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

// ===========================================
// PART 2: CONFIGURATION STRUCTURES (Shared with Settings App)
// ===========================================

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

namespace ui::external_app::enhanced_drone_analyzer {

// RAII wrapper for ChibiOS mutexes
class MutexLock {
public:
    explicit MutexLock(Mutex& mtx) : mtx_(mtx) {
        chMtxLock(&mtx_);
    }

    ~MutexLock() {
        chMtxUnlock();
    }

    // Disable copying
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;

private:
    Mutex& mtx_;
};

class DroneDetectionLogger {
public:
    DroneDetectionLogger();
    ~DroneDetectionLogger();

    void start_session();
    void end_session();
    bool log_detection(const DetectionLogEntry& entry);
    std::string get_log_filename() const;
    bool is_session_active() const { return session_active_; }

private:
    LogFile csv_log_;
    bool session_active_ = false;
    systime_t session_start_ = 0;
    uint32_t logged_count_ = 0;
    bool header_written_ = false;
    char line_buffer_[192]; // Buffer moved from stack to class member for safety

public:
    std::string format_session_summary(size_t scan_cycles, size_t total_detections) const;
private:
    bool ensure_csv_header();
    std::string format_csv_entry(const DetectionLogEntry& entry);
    std::string generate_log_filename() const;

    DroneDetectionLogger(const DroneDetectionLogger&) = delete;
    DroneDetectionLogger& operator=(const DroneDetectionLogger&) = delete;
};

class DroneScanner {
   public:
    ~DroneScanner();

public:
    enum class ScanningMode {
        DATABASE,
        WIDEBAND_CONTINUOUS,
        HYBRID
    };

    DroneScanner();

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

    // Public method for updating scan range
    void update_scan_range(Frequency min_freq, Frequency max_freq) {
        // Protection from invalid data
        if (min_freq >= max_freq) return;
        if (min_freq < 1000000) min_freq = 1000000;
        if (max_freq > 7200000000ULL) max_freq = 7200000000ULL;

        setup_wideband_range(min_freq, max_freq);
    }

    // Add public method for safe reading by UI controller
    // Returns RSSI by frequency hash (used for rendering)
    int32_t get_detection_rssi_safe(size_t freq_hash) const;

    // Method for getting detection count
    uint8_t get_detection_count_safe(size_t freq_hash) const;

    void perform_scan_cycle(DroneHardwareController& hardware);
    void process_rssi_detection(const freqman_entry& entry, int32_t rssi);
    // Message queue for thread-safe UI updates
    void send_drone_detection_message(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level);

    void update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level);
    void update_tracked_drone_internal(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level);
    void remove_stale_drones();

    Frequency get_current_scanning_frequency() const;
    ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
    const TrackedDrone& getTrackedDrone(size_t index) const;
    void handle_scan_error(const char* error_msg);

    std::string get_session_summary() const;
    size_t get_approaching_count() const { return approaching_count_; }
    size_t get_receding_count() const { return receding_count_; }
    size_t get_static_count() const { return static_count_; }
    uint32_t get_total_detections() const { return total_detections_; }
    uint32_t get_scan_cycles() const { return scan_cycles_; }
    bool is_real_mode() const { return is_real_mode_; }
    size_t get_total_memory_usage() const { return 0; } // placeholder

    DroneScanner(const DroneScanner&) = delete;
    DroneScanner(DroneScanner&&) = delete;
    DroneScanner& operator=(const DroneScanner&) = delete;
    DroneScanner& operator=(DroneScanner&&) = delete;

    // Utility functions for UI
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

    // NEW METHOD: Get safe data copy for UI
    // Returns structure with fixed array to avoid malloc/new for std::vector
    struct DroneSnapshot {
        TrackedDrone drones[MAX_TRACKED_DRONES];
        size_t count = 0;
    };

    DroneSnapshot get_tracked_drones_snapshot() const;

private:
    // Declare missing methods
    void reset_scan_cycles();
    void initialize_wideband_scanning();
    void setup_wideband_range(Frequency min_freq, Frequency max_freq);
    void wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override);
    void process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                 int32_t original_threshold, int32_t wideband_threshold);

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

    Thread* scanning_thread_ = nullptr;
    volatile bool scanning_active_{false};

    FreqmanDB freq_db_;
    size_t current_db_index_ = 0;
    Frequency last_scanned_frequency_ = 0;
    bool freq_db_loaded_ = false;

    volatile uint32_t scan_cycles_ = 0;
    uint32_t total_detections_ = 0;

    ScanningMode scanning_mode_ = ScanningMode::DATABASE;
    bool is_real_mode_ = true;

    TrackedDrone tracked_drones_[MAX_TRACKED_DRONES];
    size_t tracked_count_ = 0;

    size_t approaching_count_ = 0;
    size_t receding_count_ = 0;
    size_t static_count_ = 0;

    ThreatLevel max_detected_threat_ = ThreatLevel::NONE;
    int32_t last_valid_rssi_ = -120;

    static constexpr uint8_t DETECTION_DELAY = 2;
    WidebandScanData wideband_scan_data_;
    std::vector<std::unique_ptr<freqman_entry>> drone_database_;
    DroneDetectionLogger detection_logger_;

    // Add Mutex.
    // mutable allows locking even inside const methods (like get_snapshot)
    mutable Mutex data_mutex;

    // ADD HERE:
    DetectionRingBuffer detection_ring_buffer_; // Now the buffer lives inside the class instance
};

// ===========================================
// PART 3: HARDWARE CLASSES (from ui_drone_hardware.hpp)
// ===========================================

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

    // Direct initialization in constructor
    MessageHandlerRegistration* message_handler_spectrum_config_ = nullptr;
    MessageHandlerRegistration* message_handler_frame_sync_ = nullptr;
    MessageHandlerRegistration* message_handler_spectrum_ = nullptr;
    MessageHandlerRegistration* message_handler_channel_statistics_ = nullptr;

    SpectrumMode spectrum_mode_;
    Frequency center_frequency_;
    uint32_t bandwidth_hz_;
    RxRadioState radio_state_;
    ChannelSpectrumFIFO* fifo_ = nullptr;
    bool spectrum_streaming_active_ = false;
    volatile int32_t last_valid_rssi_ = -120;
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

    ThreatLevel current_threat_ = ThreatLevel::NONE;
    bool is_scanning_ = false;
    Frequency current_freq_ = 2400000000ULL;
    size_t approaching_count_ = 0;
    size_t static_count_ = 0;
    size_t receding_count_ = 0;
    std::string current_text_;
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
    std::string current_text_;  // Store current text for change detection
    bool is_active_ = false;
    Rect parent_rect_;

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
    Rect parent_rect_;

    Text progress_text_  {{0, 1, screen_width, 16}, ""};
    Text alert_text_     {{0, 1, screen_width, 16}, ""};
    Text normal_text_    {{0, 1, screen_width, 16}, ""};

    void paint(Painter& painter) override;
};

class DroneDisplayController {
public:
    explicit DroneDisplayController(NavigationView& nav);
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
    Text big_display_{{4, 6 * 16, 28 * 8, 52}, ""};
    ProgressBar scanning_progress_{{0, 7 * 16, screen_width, 8}};
    Text text_threat_summary_{{0, 8 * 16, screen_width, 16}, "THREAT: NONE"};
    Text text_status_info_{{0, 9 * 16, screen_width, 16}, "Ready"};
    Text text_scanner_stats_{{0, 10 * 16, screen_width, 16}, "No database"};
    Text text_trends_compact_{{0, 11 * 16, screen_width, 16}, ""};
    Text text_drone_1_{{screen_width - 120, 12 * 16, 120, 16}, ""};
    Text text_drone_2_{{screen_width - 120, 13 * 16, 120, 16}, ""};
    Text text_drone_3_{{screen_width - 120, 14 * 16, 120, 16}, ""};

    std::vector<DisplayDroneEntry> detected_drones_;
    std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES> displayed_drones_;

    std::array<Color, 240u> spectrum_row;
    std::array<uint8_t, 200> spectrum_power_levels_;
    struct ThreatBin { size_t bin; ThreatLevel threat; };
    std::array<ThreatBin, MAX_DISPLAYED_DRONES> threat_bins_;
    size_t threat_bins_count_ = 0;

    // Waterfall spectrum buffer in RAM
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
    NavigationView& nav_;

    // Объявляем только указатели, без инициализации
    MessageHandlerRegistration* message_handler_spectrum_config_ = nullptr;
    MessageHandlerRegistration* message_handler_frame_sync_ = nullptr;

    // Add missing methods for drone type/color lookup
    const char* get_drone_type_name(DroneType type) const;
    Color get_drone_type_color(DroneType type) const;
    Color get_threat_level_color(ThreatLevel level) const;
    const char* get_threat_level_name(ThreatLevel level) const;

    void get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power);
    void add_spectrum_pixel(uint8_t power);
};

// Missing constants referenced in implementation
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
                     ::AudioManager& audio_mgr);
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
    void on_select_language();
    void on_about();
    void on_audio_settings();
    void on_hardware_control();
    void on_view_logs();

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
    std::unique_ptr<DroneDisplayController> display_controller_;
    ::ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings settings_;

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

    // Simplified UI methods using basic widgets only
    void on_spectrum_mode();

    // Hardware control methods
    void on_set_bandwidth();
    void on_set_center_freq();
    void show_hardware_status();
};
class EnhancedDroneSpectrumAnalyzerView : public View {
public:
    explicit EnhancedDroneSpectrumAnalyzerView(NavigationView& nav);
    ~EnhancedDroneSpectrumAnalyzerView() override;

    void focus() override;
    std::string title() const override { return "Enhanced Drone Analyzer"; };

    void paint(Painter& painter) override;
    bool on_key(const KeyEvent key) override;
    bool on_touch(const TouchEvent event) override;
    void on_show() override;
    void on_hide() override;

private:
    NavigationView& nav_;

    // Core components
    std::unique_ptr<DroneHardwareController> hardware_;
    std::unique_ptr<DroneScanner> scanner_;
    ::AudioManager audio_;  // Direct member now

    // Forward declare SettingsManager to avoid circular dependency
    std::unique_ptr<DroneUIController> ui_controller_;
    std::unique_ptr<DroneDisplayController> display_controller_;
    std::unique_ptr<ScanningCoordinator> scanning_coordinator_;

    // UI components (modern layout)
    std::unique_ptr<SmartThreatHeader> smart_header_;
    std::unique_ptr<ConsoleStatusBar> status_bar_;
    std::array<std::unique_ptr<ThreatCard>, 3> threat_cards_;

    // Simple UI widgets (replacing complex ones)
    Button button_start_stop_;
    Button button_menu_;
    Button button_audio_;

    // Options field for scanning mode - simplified
    OptionsField field_scanning_mode_;

    bool scanning_active_ = false;
    ::ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings settings_;

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
    LoadingScreenView(NavigationView& nav);
    ~LoadingScreenView() = default;

    void paint(Painter& painter) override;

private:
    NavigationView& nav_;
    Text text_eda_{Rect{108, 213, 24, 16}, "EDA"};
    systime_t timer_start_ = 0;
};

// 1. Объявляем класс меню
class DroneSettingsMenuView : public View {
public:
    // Конструктор принимает навигацию и контроллер, чтобы вызывать функции
    DroneSettingsMenuView(NavigationView& nav, DroneUIController& controller);

    void focus() override;
    std::string title() const override { return "Settings"; };

private:
    DroneUIController& controller_;

    // Объявляем кнопки. Координаты: {x, y, ширина, высота}
    // Делаем их большими (40px высотой) для удобства нажатия
    Button button_load_db_   { {16, 16,  208, 40}, "Load Freq Database" };
    Button button_audio_     { {16, 64,  208, 40}, "Audio Alerts: Toggle" };
    Button button_hw_        { {16, 112, 208, 40}, "Hardware Info" };
    Button button_logs_      { {16, 160, 208, 40}, "View CSV Logs" };
    Button button_about_     { {16, 208, 208, 40}, "About EDA" };

    // Текстовая подсказка внизу
    Text text_info_          { {16, 260, 208, 16}, "Ver: 0.3 | Mayhem" };
};

} // namespace ui::external_app::enhanced_drone_analyzer



#endif // __UI_SCANNER_COMBINED_HPP__
