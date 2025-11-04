// ui_scanner_combined.hpp - Enhanced Drone Analyzer Header
// Combined with Looking Glass components for improved functionality

#ifndef __UI_SCANNER_COMBINED_HPP__
#define __UI_SCANNER_COMBINED_HPP__

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "app_settings.hpp"
#include "baseband_api.hpp"
#include "radio_state.hpp"
#include "receiver_model.hpp"
#include "ui_widget.hpp"
#include "ui_receiver.hpp"
#include "string_format.hpp"
#include "gradient.hpp"
#include "../../common/ui_painter.hpp"
#include "../../common/ui.hpp"
#include "../../freqman.hpp"
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <ch.h>
#include <chmtx.h>

namespace ui::external_app::enhanced_drone_analyzer {

// Constants (moved from looking glass)
#define LOOKING_GLASS_SLICE_WIDTH_MAX 20000000
#define LOOKING_GLASS_MAX_SAMPLERATE 20000000
#define MHZ_DIV 1000000
#define WINDOW_SIZE 11
#define HYSTERESIS_MARGIN_DB 5
#define DEFAULT_RSSI_THRESHOLD_DB -80

// Enums
enum class SpectrumMode { NARROW, MEDIUM, WIDE, ULTRA_WIDE };
enum class ThreatLevel { NONE, LOW, MEDIUM, HIGH, CRITICAL };
enum class DroneType { MAVIC, PHANTOM, DJI_MINI, PARROT_ANAFI, PARROT_BEBOP, PX4_DRONE, MILITARY_DRONE, UNKNOWN };
enum class ScanningMode { DATABASE, WIDEBAND_CONTINUOUS, HYBRID };
enum class MovementTrend { STATIC, APPROACHING, RECEDING, UNKNOWN };

// Structs
struct DetectionLogEntry {
    uint32_t timestamp;
    uint32_t frequency_hz;
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    float confidence_score;
};

struct TrackedDroneData {
    uint32_t frequency = 0;
    uint8_t type = static_cast<uint8_t>(DroneType::UNKNOWN);
    uint8_t threat_level = static_cast<uint8_t>(ThreatLevel::NONE);
    int16_t rssi = -120;
    systime_t last_seen = 0;
    size_t update_count = 0;

    TrackedDroneData() = default;

    void add_rssi(int16_t new_rssi, systime_t timestamp) {
        rssi = (rssi + new_rssi) / 2;
        last_seen = timestamp;
        update_count++;
    }

    MovementTrend get_trend() const {
        if (update_count < 2) return MovementTrend::UNKNOWN;
        // Simple implementation: assume static if within 5dB
        return MovementTrend::STATIC;
    }
};

struct WidebandSlice {
    uint64_t center_frequency = 0;
    size_t index = 0;
};

struct WidebandScanData {
    uint64_t min_freq = 2400000000ULL;
    uint64_t max_freq = 2500000000ULL;
    std::vector<WidebandSlice> slices;
    size_t slices_nb = 0;
    size_t slice_counter = 0;

    void reset() {
        slices.clear();
        slices_nb = 0;
        slice_counter = 0;
    }
};

struct DroneAnalyzerSettings {
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t scan_interval_ms = 750;
    int32_t rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
    bool enable_audio_alerts = true;
    uint16_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 200;
    uint32_t hardware_bandwidth_hz = 24000000;
    bool enable_real_hardware = true;
    bool demo_mode = false;
    std::string freqman_path = "DRONES";
};

struct DisplayDroneEntry {
    uint32_t frequency = 0;
    DroneType type = DroneType::UNKNOWN;
    ThreatLevel threat = ThreatLevel::NONE;
    int32_t rssi = -120;
    Timestamp last_seen = Timestamp{0};
    std::string type_name = "";
    Color display_color = Color::white();

    static constexpr size_t MAX_DISPLAYED_DRONES = 8;
};

// Utility functions
static inline int32_t clip(int32_t value, int32_t min_val, int32_t max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

static inline uint8_t clip(uint8_t value, uint8_t min_val, uint8_t max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Enhanced spectrum analysis (migrated from Looking Glass)
class WidebandMedianFilter {
private:
    static constexpr size_t WINDOW_SIZE = 11;
    std::array<int16_t, WINDOW_SIZE> window_{};
    size_t head_ = 0;
    bool full_ = false;

public:
    void add_sample(int16_t rssi) {
        window_[head_] = rssi;
        head_ = (head_ + 1) % WINDOW_SIZE;
        if (head_ == 0) full_ = true;
    }

    int16_t get_median_threshold() const {
        if (!full_) return DEFAULT_RSSI_THRESHOLD_DB;
        auto temp = window_;
        for (size_t i = 0; i < WINDOW_SIZE / 2 + 1; ++i) {
            for (size_t j = 0; j < WINDOW_SIZE - 1; ++j) {
                if (temp[j] > temp[j + 1]) std::swap(temp[j], temp[j + 1]);
            }
        }
        return temp[WINDOW_SIZE / 2] - HYSTERESIS_MARGIN_DB;
    }

    void reset() { full_ = false; head_ = 0; window_ = {}; }
};

// Detection persistence tracking (updated to use std::array)
struct DetectionEntry {
    size_t frequency_hash = 0;
    uint8_t detection_count = 0;
    int32_t rssi_value = -120;
    systime_t last_update = 0;
    size_t access_count = 0;
};

class DetectionRingBuffer {
private:
    static constexpr size_t DETECTION_TABLE_SIZE = 32;
    std::array<DetectionEntry, DETECTION_TABLE_SIZE> entries_{};
    size_t head_ = 0;
    Mutex ring_buffer_mutex_;

public:
    DetectionRingBuffer() = default;

    void update_existing_entry(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value);

    void add_new_entry(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value);

    void evict_least_recently_used();

    size_t find_entry_index(size_t frequency_hash) const;

    void remove_at_index(size_t index);

    void update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value);

    uint8_t get_detection_count(size_t frequency_hash) const;

    int32_t get_rssi_value(size_t frequency_hash) const;
};

// Enhanced audio manager for intelligent alerts (migrated from Looking Glass)
class AudioAlertManager {
public:
    enum class AlertLevel { NONE, LOW, HIGH, CRITICAL };

    AudioAlertManager() : audio_enabled_(true) {}

    void play_alert(AlertLevel level) {
        if (!audio_enabled_) return;

        uint16_t freq_hz = 800;
        switch (level) {
            case AlertLevel::LOW: freq_hz = 800; break;
            case AlertLevel::HIGH: freq_hz = 1200; break;
            case AlertLevel::CRITICAL: freq_hz = 2000; break;
            default: return;
        }
        baseband::request_audio_beep(freq_hz, 48000, 200);
    }

    void stop_audio() {}

    bool is_audio_enabled() const { return audio_enabled_; }
    void set_audio_enabled(bool enabled) { audio_enabled_ = enabled; }

private:
    bool audio_enabled_;
};

// Forward declarations
class DroneScanner;
class DroneHardwareController;
class DroneDisplayController;
class ScanningCoordinator;
class FreqDBCache;
class BufferedDetectionLogger;

// Buffered detection logger for reduced SD writes
class BufferedDetectionLogger {
private:
    static constexpr size_t LOG_BUFFER_SIZE = 32;
    static constexpr uint32_t LOG_BUFFER_FLUSH_MS = 5000;

    LogFile csv_log_;
    bool session_active_ = false;
    uint32_t session_start_ = 0;
    bool header_written_ = false;
    systime_t last_flush_time_ = 0;
    size_t logged_total_count_ = 0;

    std::array<DetectionLogEntry, LOG_BUFFER_SIZE> buffered_entries_;
    size_t entries_count_ = 0;

public:
    BufferedDetectionLogger();
    ~BufferedDetectionLogger();

    void log_detection(const DetectionLogEntry& entry);

    void flush_buffer();

    bool is_session_active() const { return session_active_; }

    void start_session();
    void end_session();

private:
    bool ensure_csv_header();
    std::string format_csv_entry(const DetectionLogEntry& entry);
    std::string generate_log_filename() const;
};

// Frequency database LRU cache entry
struct FreqDBCacheEntry {
    freqman_entry entry;
    size_t index = 0;
    systime_t last_access_time = 0;
    size_t access_count = 1;
    std::string filename = "";

    bool is_expired(systime_t current_time) const {
        return (current_time - last_access_time) > TIME_MS2I(30000);
    }

    void update_access(systime_t timestamp) {
        last_access_time = timestamp;
        access_count++;
    }
};

// Frequency database cache implementation
class FreqDBCache {
private:
    static constexpr size_t FREQ_DB_CACHE_SIZE = 64;
    std::vector<FreqDBCacheEntry> cache_entries_;
    Mutex cache_mutex_;

public:
    FreqDBCache();

    const freqman_entry* get_entry(size_t index);

    void cache_entry(const freqman_entry& entry, size_t index, const std::string& filename = "DRONES.TXT");

    void clear();

    size_t size() const { return cache_entries_.size(); }

    void flush_to_sd(const std::string& cache_file = "/EDA_CACHE/FREQ_CACHE.BIN");
};

// Other class declarations (skeleton for now)
class DroneScanner {
public:
    static constexpr size_t SCAN_THREAD_STACK_SIZE = 4096;
    static constexpr uint8_t DETECTION_DELAY = 3;
    static constexpr size_t MAX_TRACKED_DRONES = DisplayDroneEntry::MAX_DISPLAYED_DRONES;

    DroneScanner() : DroneScanner(DroneAnalyzerSettings{}) {}
    DroneScanner(const DroneAnalyzerSettings& config);
    ~DroneScanner();

    void start_scanning();
    void stop_scanning();
    bool is_scanning_active() const { return scanning_active_; }

    size_t get_total_memory_usage() const;
    void initialize_database_and_scanner();
    void cleanup_database_and_scanner();

    void initialize_wideband_scanning();
    void setup_wideband_range(uint64_t min_freq, uint64_t max_freq);

    void perform_scan_cycle(DroneHardwareController& hardware);
    void process_rssi_detection(const freqman_entry& entry, int32_t rssi);

    const TrackedDroneData& getTrackedDrone(size_t index) const;
    size_t get_approaching_count() const;
    size_t get_static_count() const;
    size_t get_receding_count() const;

    std::string get_session_summary() const;
    Frequency get_current_scanning_frequency() const;
    ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
    uint32_t get_scan_cycles() const { return scan_cycles_; }
    uint32_t get_total_detections() const { return total_detections_; }

    bool validate_detection_simple(int32_t rssi_db, ThreatLevel threat);

    void set_scanning_mode(ScanningMode mode);
    ScanningMode get_scanning_mode() const { return scanning_mode_; }

private:
    // Implementation in .cpp
    void perform_database_scan_cycle(DroneHardwareController& hardware);
    void perform_wideband_scan_cycle(DroneHardwareController& hardware);
    void perform_hybrid_scan_cycle(DroneHardwareController& hardware);
    void wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override);
    void master_wideband_detection_handler(DroneHardwareController& hardware, Frequency frequency, int32_t rssi, bool is_approaching);
    void update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level);
    void remove_stale_drones();
    void update_tracking_counts();
    msg_t scanning_thread();
    static msg_t scanning_thread_function(void* arg);

    bool scanning_active_ = false;
    Thread* scanning_thread_ = nullptr;
    size_t current_db_index_ = 0;
    Frequency last_scanned_frequency_ = 0;
    uint32_t scan_cycles_ = 0;
    uint32_t total_detections_ = 0;
    bool is_real_mode_ = true;
    size_t tracked_drones_count_ = 0;
    size_t approaching_count_ = 0;
    size_t receding_count_ = 0;
    size_t static_count_ = 0;
    ThreatLevel max_detected_threat_ = ThreatLevel::NONE;
    int32_t last_valid_rssi_ = -120;

    WidebandScanData wideband_scan_data_;
    FreqmanDB freq_db_;
    ScanningMode scanning_mode_ = ScanningMode::DATABASE;
    std::array<TrackedDroneData, MAX_TRACKED_DRONES> tracked_drones_;
    FreqDBCache freq_db_cache_;
    BufferedDetectionLogger detection_logger_;

    uint32_t scan_interval_ms_ = 750;
    int32_t rssi_threshold_db_ = DEFAULT_RSSI_THRESHOLD_DB;
    bool audio_alerts_enabled_ = true;
};

class DroneHardwareController {
public:
    static constexpr uint64_t MIN_HARDWARE_FREQ = 50000000ULL;
    static constexpr uint64_t MAX_HARDWARE_FREQ = 6000000000ULL;
    static constexpr uint64_t WIDEBAND_DEFAULT_MIN = 2400000000ULL;
    static constexpr uint64_t WIDEBAND_DEFAULT_MAX = 2480000000ULL;
    static constexpr uint64_t WIDEBAND_SLICE_WIDTH = 1000000ULL;
    static constexpr uint32_t WIDEBAND_MAX_SLICES = 32;
    static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -85;

    DroneHardwareController();
    ~DroneHardwareController();

    bool tune_to_frequency(Frequency frequency_hz);
    void start_spectrum_streaming();
    void stop_spectrum_streaming();
    bool is_spectrum_streaming_active() const;
    int32_t get_current_rssi() const;
    void update_spectrum_for_scanner();

    int32_t get_real_rssi_from_hardware(Frequency target_frequency);
    void update_radio_bandwidth();
    void handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message);
    void process_channel_spectrum_data(const ChannelSpectrum& spectrum);

    void on_hardware_show();
    void on_hardware_hide();

private:
    Frequency center_frequency_ = 2440000000ULL;
    SpectrumMode spectrum_mode_ = SpectrumMode::MEDIUM;
    int32_t bandwidth_hz_ = 8000000;
    int32_t last_valid_rssi_ = -120;
    bool spectrum_streaming_active_ = false;
    Mutex spectrum_access_mutex_;
    systime_t frequency_lock_timeout_ms_ = 100;

    bool wait_for_frequency_lock(systime_t timeout_ms);
    int32_t read_raw_rssi_from_hardware() const;
    void set_spectrum_bandwidth(int32_t bandwidth_hz);
    void set_spectrum_bandwidth_by_mode();
};

// UI classes (skeleton)
class SmartThreatHeader : public View {
public:
    SmartThreatHeader(Rect parent_rect);

    void update(ThreatLevel max_threat, size_t approaching, size_t static_count, size_t receding, Frequency current_freq, bool is_scanning);
    void set_max_threat(ThreatLevel threat);
    void set_movement_counts(size_t approaching, size_t static_count, size_t receding);
    void set_current_frequency(Frequency freq);
    void set_scanning_state(bool is_scanning);

    void paint(Painter& painter) override;

private:
    ProgressBar threat_progress_bar_;
    TextField threat_status_main_;
    TextField threat_frequency_;

    ThreatLevel current_threat_ = ThreatLevel::NONE;
    bool is_scanning_ = false;
    Frequency current_freq_ = 0;
    size_t approaching_count_ = 0;
    size_t static_count_ = 0;
    size_t receding_count_ = 0;

    Color get_threat_bar_color(ThreatLevel level) const;
    Color get_threat_text_color(ThreatLevel level) const;
    std::string get_threat_text(ThreatLevel level) const;
};

class ThreatCard : public View {
public:
    ThreatCard(size_t index, Rect parent_rect);
    void update_card(const DisplayDroneEntry& entry);
    void clear_card();
    void paint(Painter& painter) override;

private:
    size_t index_;
    DisplayDroneEntry entry_;
};

class ConsoleStatusBar : public View {
public:
    enum class DisplayMode { NORMAL, ALERT, SCANNING };

    ConsoleStatusBar(size_t bar_index, Rect parent_rect);

    void update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections);
    void update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg);
    void update_normal_status(const std::string& primary, const std::string& secondary);

    void paint(Painter& painter) override;

private:
    size_t bar_index_;
    DisplayMode mode_ = DisplayMode::NORMAL;
    TextField progress_text_;
    TextField alert_text_;
    TextField normal_text_;

    void set_display_mode(DisplayMode mode);
};

class DroneDisplayController {
public:
    DroneDisplayController(NavigationView& nav);
    ~DroneDisplayController();

    void update_detection_display(DroneScanner& scanner);
    void set_scanning_status(bool is_scanning, const std::string& mode_name);

private:
    NavigationView& nav_;
};

class ScanningCoordinator {
public:
    static constexpr size_t SCANNING_THREAD_STACK_SIZE = 8192;

    ScanningCoordinator(NavigationView& nav, DroneHardwareController& hardware, DroneScanner& scanner, DroneDisplayController& display_controller, AudioAlertManager& audio_controller);
    ~ScanningCoordinator();

    void start_coordinated_scanning();
    void stop_coordinated_scanning();
    bool is_scanning_active() const;
    void update_runtime_parameters(const DroneAnalyzerSettings& settings);

    void show_session_summary(const std::string& summary);

private:
    msg_t coordinated_scanning_thread();
    static msg_t scanning_thread_function(void* arg);

    Thread* scanning_thread_ = nullptr;
    bool scanning_active_ = false;
    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    DroneDisplayController& display_controller_;
    AudioAlertManager& audio_controller_;
    uint32_t scan_interval_ms_ = 750;
};

class EnhancedDroneSpectrumAnalyzerView : public View {
public:
    EnhancedDroneSpectrumAnalyzerView(NavigationView& nav);
    ~EnhancedDroneSpectrumAnalyzerView() = default;

    void focus() override;
    void on_show() override;
    void on_hide() override;
    bool on_key(const KeyEvent key) override;
    bool on_touch(const TouchEvent event) override;
    void paint(Painter& painter) override;

private:
    NavigationView& nav_;

    std::unique_ptr<SmartThreatHeader> smart_header_;
    std::unique_ptr<ConsoleStatusBar> status_bar_;
    std::array<std::unique_ptr<ThreatCard>, 3> threat_cards_;

    std::unique_ptr<DroneHardwareController> hardware_;
    std::unique_ptr<DroneScanner> scanner_;
    std::unique_ptr<DroneDisplayController> display_controller_;
    std::unique_ptr<ScanningCoordinator> scanning_coordinator_;
    DroneAnalyzerSettings ui_controller_settings_;

    AudioAlertManager audio_mgr_;

    Button button_start_;
    Button button_menu_;

    void initialize_modern_layout();
    void update_modern_layout();
    void handle_scanner_update();

    void start_scanning_thread();
    void stop_scanning_thread();

    bool handle_start_stop_button();
    bool handle_menu_button();

    void ui_controller_on_start_scan() { start_scanning_thread(); }
    void ui_controller_on_stop_scan() { stop_scanning_thread(); }
};

class LoadingScreenView : public View {
public:
    LoadingScreenView(NavigationView& nav);
    ~LoadingScreenView() = default;

    void focus() override { set_focusable(false); }
    void paint(Painter& painter) override;

private:
    NavigationView& nav_;
    TextField text_eda_;
    Timestamp timer_start_;
};

// Global functions
bool load_settings_from_sd_card(DroneAnalyzerSettings& settings);
void initialize_app(NavigationView& nav);

// Simple validation
class SimpleDroneValidation {
public:
    static bool validate_frequency_range(Frequency freq_hz);
    static bool validate_rssi_signal(int32_t rssi_db, ThreatLevel threat = ThreatLevel::NONE);
    static ThreatLevel classify_signal_strength(int32_t rssi_db);
    static DroneType identify_drone_type(Frequency freq_hz, int32_t rssi_db);
    static bool validate_drone_detection(Frequency freq_hz, int32_t rssi_db, DroneType type, ThreatLevel threat);
};

} // namespace ui::external_app::enhanced_drone_analyzer

#endif // __UI_SCANNER_COMBINED_HPP__
