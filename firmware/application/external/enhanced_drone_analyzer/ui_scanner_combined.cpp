// ui_scanner_combined.cpp - Unified implementation for Enhanced Drone Analyzer Scanner App
// Combines implementations from: ui_drone_scanner.cpp, ui_drone_hardware.cpp, ui_drone_ui.cpp
// Combines all required implementations for scanner functionality

#include "ui_scanner_combined.hpp"
#include "ui_drone_audio.hpp"
#include "../../gradient.hpp"
#include "../../baseband_api.hpp"
#include <algorithm>
#include <sstream>
#include <cstdlib>

// Add ChibiOS headers for threading
#include <ch.h>

// Global settings loading functions
bool validate_loaded_settings(const DroneAnalyzerSettings& settings);
bool parse_settings_from_content(const std::string& content, DroneAnalyzerSettings& settings);
bool load_settings_from_sd_card(DroneAnalyzerSettings& settings);

static constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;
static constexpr uint32_t SCAN_THREAD_STACK_SIZE = 8192;
static constexpr uint32_t CLEANUP_THREAD_STACK_SIZE = 4096;
static constexpr uint32_t AUDIO_THREAD_STACK_SIZE = 4096;

WORKING_AREA(scanning_thread_wa, SCAN_THREAD_STACK_SIZE);
#define MSG_OK (msg_t)0

using namespace ui::external_app::enhanced_drone_analyzer;

namespace ui::external_app::enhanced_drone_analyzer {

// Implementation for all class methods inside namespace

// DetectionRingBuffer implementations
DetectionRingBuffer global_detection_ring;
DetectionRingBuffer& local_detection_ring = global_detection_ring;

void DetectionRingBuffer::clear() {
    memset(detection_counts_, 0, sizeof(detection_counts_));
    memset(rssi_values_, 0, sizeof(rssi_values_));
    for (size_t i = 0; i < DETECTION_TABLE_SIZE; i++) {
        rssi_values_[i] = -120;
    }
}

void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    const size_t index = frequency_hash % DETECTION_TABLE_SIZE;
    detection_counts_[index] = detection_count;
    rssi_values_[index] = rssi_value;
    __DMB();
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
    const size_t index = frequency_hash % DETECTION_TABLE_SIZE;
    return detection_counts_[index];
}

int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const {
    const size_t index = frequency_hash % DETECTION_TABLE_SIZE;
    return rssi_values_[index];
}

// DroneScanner implementations
DroneScanner::DroneScanner() : DroneScanner(DroneAnalyzerSettings{}) {}

DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
    : scanning_active_(false),
      scanning_thread_(nullptr),
      current_db_index_(0),
      last_scanned_frequency_(0),
      scan_cycles_(0),
      total_detections_(0),
      is_real_mode_(config.enable_real_hardware),
      tracked_drones_count_(0),
      approaching_count_(0),
      receding_count_(0),
      static_count_(0),
      max_detected_threat_(ThreatLevel::NONE),
      last_valid_rssi_(-120),
      wideband_scan_data_(),
      freq_db_(),
      scanning_mode_(ScanningMode::DATABASE),
      tracked_drones_(),
      detection_processor_(this),
      scan_interval_ms_(config.scan_interval_ms),
      rssi_threshold_db_(config.rssi_threshold_db),
      audio_alerts_enabled_(config.enable_audio_alerts)
{}

DroneScanner::~DroneScanner() {
    stop_scanning();
    cleanup_database_and_scanner();
}

size_t DroneScanner::get_total_memory_usage() const {
    return sizeof(*this) + (tracked_drones_.size() * sizeof(TrackedDroneData)) +
           (freq_db_.entry_count() > 0 ? freq_db_.entry_count() * sizeof(freqman_entry) : 0);
}

void DroneScanner::initialize_database_and_scanner() {
    auto db_path = get_freqman_path("DRONES");
    if (!freq_db_.open(db_path)) {
        // Continue without enhanced drone data
    }
}

void DroneScanner::cleanup_database_and_scanner() {
    if (scanning_thread_) {
        scanning_active_ = false;
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }
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
    if (scanning_active_ || scanning_thread_ != nullptr) return;

    scanning_active_ = true;
    scan_cycles_ = 0;
    total_detections_ = 0;

    scanning_thread_ = chThdCreateStatic(scanning_thread_wa, sizeof(scanning_thread_wa),
                                        NORMALPRIO + 10, scanning_thread_function, this);
    if (!scanning_thread_) {
        scanning_active_ = false;
    }
}

void DroneScanner::start_spectrum_for_scanning(DroneHardwareController& hardware) {
    if (scanning_active_ && !hardware.is_spectrum_streaming_active()) {
        hardware.start_spectrum_streaming();
    }
}

void DroneScanner::stop_spectrum_for_scanning(DroneHardwareController& hardware) {
    if (!scanning_active_ && hardware.is_spectrum_streaming_active()) {
        hardware.stop_spectrum_streaming();
    }
}

void DroneScanner::stop_scanning() {
    if (!scanning_active_) return;

    scanning_active_ = false;
    if (scanning_thread_ && chThdTerminated(scanning_thread_) == false) {
        chThdTerminate(scanning_thread_);
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }
    remove_stale_drones();

    if (detection_logger_.is_session_active()) {
        detection_logger_.end_session();
    }
}

msg_t DroneScanner::scanning_thread_function(void* arg) {
    auto* self = static_cast<DroneScanner*>(arg);
    return self->scanning_thread();
}

msg_t DroneScanner::scanning_thread() {
    while (scanning_active_ && !chThdShouldTerminate()) {
        chThdSleepMilliseconds(scan_interval_ms_);
        scan_cycles_++;
    }
    scanning_active_ = false;
    scanning_thread_ = nullptr;
    chThdExit(MSG_OK);
    return MSG_OK;
}

void DroneScanner::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
    // Process wideband detection with custom threshold
    process_wideband_detection_with_override(entry, rssi, rssi_threshold_db_, threshold_override);
}

void DroneScanner::master_wideband_detection_handler(DroneHardwareController& hardware, Frequency frequency, int32_t rssi, bool is_approaching) {
    // Wrapper for existing wideband detection handler
    const bool force_detection = !is_approaching; // Force detection if not approaching
    master_wideband_detection_handler(hardware, frequency, rssi, force_detection);
}

bool DroneScanner::load_frequency_database() {
    if (freq_db_.entry_count() == 0) return false;
    current_db_index_ = 0;
    last_scanned_frequency_ = 0;

    if (freq_db_.entry_count() > 100) handle_scan_error("Large database loaded");
    scan_init_from_loaded_frequencies();
    return true;
}

size_t DroneScanner::get_database_size() const {
    size_t count = freq_db_.entry_count();
    return count > 0 ? count : 0;
}

void DroneScanner::set_scanning_mode(ScanningMode mode) {
    scanning_mode_ = mode;
    stop_scanning();
    scan_cycles_ = 0;
    total_detections_ = 0;

    if (scanning_mode_ == ScanningMode::DATABASE || scanning_mode_ == ScanningMode::HYBRID) {
        load_frequency_database();
    }
}

std::string DroneScanner::scanning_mode_name() const {
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
    size_t entry_count = freq_db_.entry_count();
    if (entry_count == 0) {
        if (scan_cycles_ % 50 == 0) {
            handle_scan_error("No frequency database loaded");
            scanning_active_ = false;
        }
        return;
    }

    const size_t total_entries = freq_db_.entry_count();
    if (current_db_index_ >= total_entries) {
        current_db_index_ = 0;
    }

    const freqman_entry& entry = freq_db_[current_db_index_];
    if (entry.frequency_a > 0) {
        Frequency target_freq_hz = entry.frequency_a;
        if (target_freq_hz >= 50000000 && target_freq_hz <= 6000000000) {
            if (hardware.tune_to_frequency(target_freq_hz)) {
                int32_t real_rssi = hardware.get_real_rssi_from_hardware(target_freq_hz);
                process_rssi_detection(entry, real_rssi);
                last_scanned_frequency_ = target_freq_hz;
            }
        }
        current_db_index_ = (current_db_index_ + 1) % total_entries;
    } else {
        current_db_index_ = (current_db_index_ + 1) % total_entries;
    }
}

void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
    if (wideband_scan_data_.slices_nb == 0) {
        setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
    }

    if (wideband_scan_data_.slice_counter >= wideband_scan_data_.slices_nb) {
        wideband_scan_data_.slice_counter = 0;
    }

    const WidebandSlice& current_slice = wideband_scan_data_.slices[wideband_scan_data_.slice_counter];
    master_wideband_detection_handler(hardware, current_slice.center_frequency,
                                     WIDEBAND_RSSI_THRESHOLD_DB, false);
    last_scanned_frequency_ = current_slice.center_frequency;
    wideband_scan_data_.slice_counter = (wideband_scan_data_.slice_counter + 1) % wideband_scan_data_.slices_nb;
}

void DroneScanner::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
    if (scan_cycles_ % 2 == 0) {
        perform_wideband_scan_cycle(hardware);
    } else {
        perform_database_scan_cycle(hardware);
    }
}

void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::NONE) ||
        !SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    ThreatLevel threat_level;
    if (rssi > -70) threat_level = ThreatLevel::HIGH;
    else if (rssi > -80) threat_level = ThreatLevel::MEDIUM;
    else threat_level = ThreatLevel::LOW;

    if (entry.frequency_a >= 2'400'000'000 && entry.frequency_a <= 2'500'000'000) {
        threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
    }

    total_detections_++;
    freqman_entry db_entry{};
    if (freq_db_.entry_count() > 0) {
        db_entry = freq_db_[0];
    }
    DroneType detected_type = (db_entry.bandwidth == 0 ? DroneType::MAVIC : DroneType::UNKNOWN);

    size_t freq_hash = entry.frequency_a;
    int32_t effective_threshold = rssi_threshold_db_;
    if (local_detection_ring.get_rssi_value(freq_hash) < rssi_threshold_db_) {
        effective_threshold = rssi_threshold_db_ + HYSTERESIS_MARGIN_DB;
    }

    if (rssi >= effective_threshold) {
        uint8_t current_count = local_detection_ring.get_detection_count(freq_hash);
        current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
        local_detection_ring.update_detection(freq_hash, current_count, rssi);

        if (current_count >= MIN_DETECTION_COUNT) {
            DetectionLogEntry log_entry{
                .timestamp = chTimeNow(),
                .frequency_hz = static_cast<uint32_t>(entry.frequency_a),
                .rssi_db = rssi,
                .threat_level = threat_level,
                .drone_type = detected_type,
                .detection_count = current_count,
                .confidence_score = 0.8f
            };

            if (detection_logger_.is_session_active()) {
                detection_logger_.log_detection(log_entry);
            }

            if (threat_level >= ThreatLevel::HIGH && audio_alerts_enabled_) {
                baseband::request_audio_beep(800, 48000, 200);
            }

            update_tracked_drone(detected_type, entry.frequency_a, rssi, threat_level);
        }
    } else {
        local_detection_ring.update_detection(freq_hash, 0, -120);
    }
}

void DroneScanner::update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
    for (size_t i = 0; i < MAX_TRACKED_DRONES; i++) {
        TrackedDroneData& drone = tracked_drones_[i];
        if (drone.frequency == static_cast<uint32_t>(frequency) && drone.update_count > 0) {
            drone.add_rssi(static_cast<int16_t>(rssi), chTimeNow());
            drone.drone_type = static_cast<uint8_t>(type);
            drone.threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
        if (drone.update_count == 0) {
            drone.frequency = static_cast<uint32_t>(frequency);
            drone.drone_type = static_cast<uint8_t>(type);
            drone.threat_level = static_cast<uint8_t>(threat_level);
            drone.add_rssi(static_cast<int16_t>(rssi), chTimeNow());
            tracked_drones_count_++;
            update_tracking_counts();
            return;
        }
    }

    size_t oldest_index = 0;
    systime_t oldest_time = tracked_drones_[0].last_seen;
    for (size_t i = 1; i < MAX_TRACKED_DRONES; i++) {
        if (tracked_drones_[i].last_seen < oldest_time) {
            oldest_time = tracked_drones_[i].last_seen;
            oldest_index = i;
        }
    }

    tracked_drones_[oldest_index] = TrackedDroneData();
    tracked_drones_[oldest_index].frequency = static_cast<uint32_t>(frequency);
    tracked_drones_[oldest_index].drone_type = static_cast<uint8_t>(type);
    tracked_drones_[oldest_index].threat_level = static_cast<uint8_t>(threat_level);
    tracked_drones_[oldest_index].add_rssi(static_cast<int16_t>(rssi), chTimeNow());
    update_tracking_counts();
}

void DroneScanner::remove_stale_drones() {
    const systime_t STALE_TIMEOUT = 30000;
    systime_t current_time = chTimeNow();

    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < MAX_TRACKED_DRONES; read_idx++) {
        const TrackedDroneData& drone = tracked_drones_[read_idx];
        if (drone.update_count == 0) continue;

        bool is_stale = (current_time - drone.last_seen) > STALE_TIMEOUT;
        if (!is_stale) {
            if (write_idx != read_idx) {
                tracked_drones_[write_idx] = drone;
            }
            write_idx++;
        } else {
            tracked_drones_[read_idx] = TrackedDroneData();
        }
    }

    tracked_drones_count_ = write_idx;
    update_tracking_counts();
}

void DroneScanner::update_tracking_counts() {
    approaching_count_ = 0;
    receding_count_ = 0;
    static_count_ = 0;

    for (size_t i = 0; i < MAX_TRACKED_DRONES; i++) {
        const TrackedDroneData& drone = tracked_drones_[i];
        if (drone.update_count < 2) continue;

        MovementTrend trend = drone.get_trend();
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
    // Placeholder - implementation moved to UI layer
}

bool DroneScanner::validate_detection_simple(int32_t rssi_db, ThreatLevel threat) {
    return SimpleDroneValidation::validate_rssi_signal(rssi_db, threat);
}

Frequency DroneScanner::get_current_scanning_frequency() const {
    size_t entry_count = freq_db_.entry_count();
    if (entry_count > 0 && current_db_index_ < entry_count) {
        const freqman_entry& entry = freq_db_[current_db_index_];
        return entry.frequency_a;
    }
    return 433000000;
}

const TrackedDroneData& DroneScanner::getTrackedDrone(size_t index) const {
    return (index < MAX_TRACKED_DRONES) ? tracked_drones_[index] : tracked_drones_[0];
}

std::string DroneScanner::get_session_summary() const {
    return detection_logger_.format_session_summary(get_scan_cycles(), get_total_detections());
}

void DroneScanner::handle_scan_error(const char* error_msg) {
    (void)error_msg;
}

// DroneLogger implementations
DroneScanner::DroneDetectionLogger::DroneDetectionLogger()
    : session_active_(false), session_start_(0), logged_count_(0), header_written_(false) {
    start_session();
}

DroneScanner::DroneDetectionLogger::~DroneDetectionLogger() {
    end_session();
}

void DroneScanner::DroneDetectionLogger::start_session() {
    if (session_active_) return;
    session_active_ = true;
    session_start_ = chTimeNow();
    logged_count_ = 0;
    header_written_ = false;
}

void DroneScanner::DroneDetectionLogger::end_session() {
    if (!session_active_) return;
    session_active_ = false;
}

inline bool DroneScanner::DroneDetectionLogger::log_detection(const DetectionLogEntry& entry) {
    if (!session_active_) return false;
    if (!ensure_csv_header()) return false;

    std::string csv_entry = format_csv_entry(entry);
    auto error = csv_log_.append(generate_log_filename());
    if (!error.has_value()) return false;
    error = csv_log_.write_raw(csv_entry);
    if (error.has_value()) {
        logged_count_++;
        return true;
    }
    return false;
}

inline bool DroneScanner::DroneDetectionLogger::ensure_csv_header() {
    if (header_written_) return true;
    const char* header = "timestamp_ms,frequency_hz,rssi_db,threat_level,drone_type,detection_count,confidence\n";
    auto error = csv_log_.append(generate_log_filename());
    if (!error.has_value()) return false;
    error = csv_log_.write_raw(header);
    if (error.has_value()) {
        header_written_ = true;
        return true;
    }
    return false;
}

inline std::string DroneScanner::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry& entry) {
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

inline std::string DroneScanner::DroneDetectionLogger::generate_log_filename() const {
    return "EDA_LOG.CSV";
}

std::string DroneScanner::DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) const {
    uint32_t session_duration_ms = chTimeNow() - session_start_;
    float avg_detections_per_cycle = scan_cycles > 0 ? static_cast<float>(total_detections) / scan_cycles : 0.0f;
    float detections_per_second = session_duration_ms > 0 ?
        static_cast<float>(total_detections) * 1000.0f / session_duration_ms : 0.0f;

    char summary_buffer[512];
    memset(summary_buffer, 0, sizeof(summary_buffer));
    int ret = snprintf(summary_buffer, sizeof(summary_buffer) - 1,
    "SCANNING SESSION COMPLETE\n========================\n\nSESSION STATISTICS:\nDuration: %.1f seconds\nScan Cycles: %zu\nTotal Detections: %zu\n\nPERFORMANCE:\nAvg. detections/cycle: %.2f\nDetection rate: %.1f/sec\nLogged entries: %lu\n\nEnhanced Drone Analyzer v0.3",
        static_cast<float>(session_duration_ms) / 1000.0f, scan_cycles, total_detections,
        avg_detections_per_cycle, detections_per_second, logged_count_);

    if (ret < 0 || ret >= static_cast<int>(sizeof(summary_buffer))) {
        return std::string("SCANNING COMPLETE\nCycles: ") + std::to_string(scan_cycles) +
               "\nDetections: " + std::to_string(total_detections);
    }
    return std::string(summary_buffer);
}

// DetectionProcessor implementation
DetectionProcessor::DetectionProcessor(DroneScanner* scanner) : scanner_(scanner) {}

void DetectionProcessor::process_unified_detection(const freqman_entry& entry, int32_t rssi, int32_t effective_threshold,
                                                   float confidence_score, bool force_process) {
    (void)confidence_score; (void)force_process;

    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::NONE)) return;
    if (!SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) return;

    if (rssi < effective_threshold) return;

    DetectionLogEntry log_entry{
        .timestamp = chTimeNow(),
        .frequency_hz = static_cast<uint32_t>(entry.frequency_a),
        .rssi_db = rssi,
        .threat_level = ThreatLevel::LOW,
        .drone_type = DroneType::UNKNOWN,
        .detection_count = 1,
        .confidence_score = 0.8f
    };

    if (scanner_->detection_logger_.is_session_active()) {
        scanner_->detection_logger_.log_detection(log_entry);
    }

    scanner_->update_tracked_drone(DroneType::UNKNOWN, entry.frequency_a, rssi, ThreatLevel::LOW);
    scanner_->total_detections_++;
}

// DroneHardwareController implementations
DroneHardwareController::DroneHardwareController(SpectrumMode mode)
    : spectrum_mode_(mode),
      center_frequency_(2400000000ULL),
      bandwidth_hz_(24000000),
      spectrum_streaming_active_(false),
      last_valid_rssi_(-120),
      fifo_(nullptr),
      spectrum_fifo_(nullptr)
{
    initialize_radio_state();
    initialize_spectrum_collector();
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
    stop_spectrum_streaming();
    cleanup_spectrum_collector();
}

void DroneHardwareController::initialize_radio_state() {}

void DroneHardwareController::initialize_spectrum_collector() {
    // Note: Actual MessageHandlerRegistration happens in DroneDisplayController
}

void DroneHardwareController::cleanup_spectrum_collector() {
    spectrum_streaming_active_ = false;
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
    if (frequency_hz < MIN_HARDWARE_FREQ || frequency_hz > MAX_HARDWARE_FREQ) {
        return false;
    }

    center_frequency_ = frequency_hz;
    radio::set_tuning_frequency(frequency_hz);
    update_radio_bandwidth();
    return true;
}

void DroneHardwareController::start_spectrum_streaming() {
    if (spectrum_streaming_active_) return;
    spectrum_streaming_active_ = true;
    radio::set_rf_amp(true);
    radio::set_antenna_bias(portapack::get_antenna_bias());
}

void DroneHardwareController::stop_spectrum_streaming() {
    spectrum_streaming_active_ = false;
    radio::set_rf_amp(false);
}

bool DroneHardwareController::is_spectrum_streaming_active() const {
    return spectrum_streaming_active_;
}

int32_t DroneHardwareController::get_current_rssi() const {
    return last_valid_rssi_;
}

int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency) {
    if (center_frequency_ != target_frequency) {
        tune_to_frequency(target_frequency);
        chThdSleepMilliseconds(10);
    }

    if (spectrum_streaming_active_) {
        if (last_valid_rssi_ == 0) {
            last_valid_rssi_ = -85;
        }
    } else {
        last_valid_rssi_ = -85;
    }

    if (last_valid_rssi_ < -120) last_valid_rssi_ = -120;
    if (last_valid_rssi_ > 0) last_valid_rssi_ = 0;

    return last_valid_rssi_;
}

void DroneHardwareController::update_radio_bandwidth() {
    switch (spectrum_mode_) {
        case SpectrumMode::NARROW: set_spectrum_bandwidth(4000000); break;
        case SpectrumMode::MEDIUM: set_spectrum_bandwidth(8000000); break;
        case SpectrumMode::WIDE: set_spectrum_bandwidth(20000000); break;
        case SpectrumMode::ULTRA_WIDE: set_spectrum_bandwidth(24000000); break;
        default: set_spectrum_bandwidth(8000000); break;
    }
}

void DroneHardwareController::update_spectrum_for_scanner() {
    if (spectrum_streaming_active_) {
        update_radio_bandwidth();
    }
}

void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message) {
    (void)message;
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

// UI implementations

// SmartThreatHeader implementation
SmartThreatHeader::SmartThreatHeader(Rect parent_rect)
    : View(parent_rect),
      threat_progress_bar_({0, 0, screen_width, 16}),
      threat_status_main_({0, 20, screen_width, 16}, "THREAT: LOW | ▲0 ■0 ▼0"),
      threat_frequency_({0, 38, screen_width, 16}, "2400.0MHz SCANNING") {
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
    std::string threat_name = get_threat_text(max_threat);
    if (total_drones > 0) {
        snprintf(buffer, sizeof(buffer), "THREAT: %s | ▲%zu ■%zu ▼%zu",
                threat_name.c_str(), approaching, static_count, receding);
    } else if (is_scanning) {
        snprintf(buffer, sizeof(buffer), "SCANNING: ▲%zu ■%zu ▼%zu",
                approaching, static_count, receding);
    } else {
        snprintf(buffer, sizeof(buffer), "READY: No Threats Detected");
    }
    threat_status_main_.set(buffer);

    Style status_style{
        .foreground = get_threat_text_color(max_threat),
        .background = Color::black(),
        .font = Theme::getInstance()->fg_light->font
    };
    threat_status_main_.set_style(&status_style);

    if (current_freq > 0) {
        float freq_mhz = static_cast<float>(current_freq) / 1000000.0f;
        if (freq_mhz >= 1000) {
            freq_mhz /= 1000;
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%.2fGHz SCANNING", freq_mhz);
            } else {
                snprintf(buffer, sizeof(buffer), "%.2fGHz READY", freq_mhz);
            }
        } else {
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%.1fMHz SCANNING", freq_mhz);
            } else {
                snprintf(buffer, sizeof(buffer), "%.1fMHz READY", freq_mhz);
            }
        }
        threat_frequency_.set(buffer);
    } else {
        threat_frequency_.set("NO SIGNAL");
    }
    threat_frequency_.set_style(&get_threat_text_color(max_threat));
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
        case ThreatLevel::HIGH: return Color(255, 165, 0);
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

std::string SmartThreatHeader::get_threat_text(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return "CRITICAL 🔴";
        case ThreatLevel::HIGH: return "HIGH 🟠";
        case ThreatLevel::MEDIUM: return "MEDIUM 🟡";
        case ThreatLevel::LOW: return "LOW 🟢";
        case ThreatLevel::NONE:
        default: return "CLEAR ✅";
    }
}

void SmartThreatHeader::paint(Painter& painter) {
    View::paint(painter);

    // Flash effect for high threats
    if (current_threat_ >= ThreatLevel::HIGH) {
        static uint32_t pulse_timer = 0;
        pulse_timer++;
        uint8_t alpha = (pulse_timer % 20) < 10 ? 50 : 100;

        Color base_color = get_threat_bar_color(current_threat_);
        painter.fill_rectangle(Rect(parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 4), base_color);
    }
}

// ConsoleStatusBar implementation
ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
    : View(parent_rect), bar_index_(bar_index),
      progress_text_ {{0, 1, screen_width, 16}, ""},
      alert_text_ {{0, 1, screen_width, 16}, ""},
      normal_text_ {{0, 1, screen_width, 16}, ""} {
    add_children({&progress_text_, &alert_text_, &normal_text_});
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    set_display_mode(DisplayMode::SCANNING);

    char progress_bar[9] = "########";
    uint8_t filled = (progress_percent * 8) / 100;
    for (uint8_t i = filled; i < 8; i++) {
        progress_bar[i] = '.';
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
            progress_bar, progress_percent, total_cycles, detections);
    progress_text_.set(buffer);
    progress_text_.set_style(Style{Theme::getInstance()->fg_blue->foreground, Color::black(), Theme::getInstance()->fg_blue->font});

    if (detections > 0) {
        set_display_mode(DisplayMode::ALERT);
        snprintf(buffer, sizeof(buffer), "⚠️ DETECTED: %lu threats found!", detections);
        alert_text_.set(buffer);
        Style alert_style{(detections > 10) ? Theme::getInstance()->fg_red->foreground : Theme::getInstance()->fg_yellow->foreground,
                         Color::black(), Theme::getInstance()->fg_yellow->font};
        alert_text_.set_style(&alert_style);
    }
    set_dirty();
}

void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
    set_display_mode(DisplayMode::ALERT);

    const char* icons[5] = {"ℹ️", "⚠️", "🟠", "🔴", "🚨"};
    size_t icon_idx = std::min(static_cast<size_t>(threat), size_t(4));

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%s ALERT: %zu drones | %s",
            icons[icon_idx], total_drones, alert_msg.c_str());

    alert_text_.set(buffer);

    Style alert_style{(threat >= ThreatLevel::CRITICAL) ? Theme::getInstance()->fg_red->foreground : Theme::getInstance()->fg_yellow->foreground,
                     Color::black(), Theme::getInstance()->fg_yellow->font};
    alert_text_.set_style(&alert_style);
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
    normal_text_.set_style(Style{Theme::getInstance()->fg_light->foreground, Color::black(), Theme::getInstance()->fg_light->font});
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

    // Background highlight for alerts
    if (mode_ == DisplayMode::ALERT) {
        painter.fill_rectangle(Rect(parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2), Color(32, 0, 0));
    }
}

// Global utility functions
const char* get_drone_type_name(uint8_t type) {
    switch (static_cast<DroneType>(type)) {
        case DroneType::MAVIC: return "MAVIC";
        case DroneType::PHANTOM: return "PHANTOM";
        case DroneType::DJI_MINI: return "DJI MINI";
        case DroneType::PARROT_ANAFI: return "PARROT ANAFI";
        case DroneType::PARROT_BEBOP: return "PARROT BEBOP";
        case DroneType::PX4_DRONE: return "PX4";
        case DroneType::MILITARY_DRONE: return "MILITARY";
        case DroneType::UNKNOWN: default: return "UNKNOWN";
    }
}

Color get_drone_type_color(uint8_t type) {
    switch (static_cast<DroneType>(type)) {
        case DroneType::MAVIC: return Color::yellow();
        case DroneType::PHANTOM: return Color::green();
        case DroneType::DJI_MINI: return Color::blue();
        case DroneType::PARROT_ANAFI: return Color(255, 165, 0);
        case DroneType::PARROT_BEBOP: return Color(255, 165, 0);
        case DroneType::PX4_DRONE: return Color::red();
        case DroneType::MILITARY_DRONE: return Color::red();
        case DroneType::UNKNOWN: default: return Color::white();
    }
}

// Global settings functions
bool validate_loaded_settings(const DroneAnalyzerSettings& settings) {
    if (settings.scan_interval_ms < 100 || settings.scan_interval_ms > 30000) return false;
    if (settings.rssi_threshold_db < -120 || settings.rssi_threshold_db > 0) return false;
    if (settings.audio_alert_frequency_hz < 200 || settings.audio_alert_frequency_hz > 3000) return false;
    if (settings.audio_alert_duration_ms < 50 || settings.audio_alert_duration_ms > 2000) return false;
    if (settings.hardware_bandwidth_hz < 1000000 || settings.hardware_bandwidth_hz > 100000000) return false;
    return true;
}

bool parse_settings_from_content(const std::string& content, DroneAnalyzerSettings& settings) {
    std::istringstream iss(content);
    std::string line;
    size_t parsed_lines = 0;

    while (std::getline(iss, line)) {
        auto it = std::find_if(line.begin(), line.end(), [](int ch) { return !std::isspace(ch); });
        line.erase(line.begin(), it);
        auto rit = std::find_if(line.rbegin(), line.rend(), [](int ch) { return !std::isspace(ch); });
        line.erase(rit.base(), line.end());

        if (line.empty() || line[0] == '#') continue;

        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) continue;

        std::string key = line.substr(0, equals_pos);
        std::string value = line.substr(equals_pos + 1);

        // Trim strings
        key.erase(key.begin(), std::find_if(key.begin(), key.end(), [](int ch) { return !std::isspace(ch); }));
        key.erase(std::find_if(key.rbegin(), key.rend(), [](int ch) { return !std::isspace(ch); }).base(), key.end());
        value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](int ch) { return !std::isspace(ch); }));
        value.erase(std::find_if(value.rbegin(), value.rend(), [](int ch) { return !std::isspace(ch); }).base(), value.end());

        if (key == "spectrum_mode") {
            if (value == "NARROW") settings.spectrum_mode = SpectrumMode::NARROW;
            else if (value == "MEDIUM") settings.spectrum_mode = SpectrumMode::MEDIUM;
            else if (value == "WIDE") settings.spectrum_mode = SpectrumMode::WIDE;
            else if (value == "ULTRA_WIDE") settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
            parsed_lines++;
        } else if (key == "scan_interval_ms") {
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val <= 30000) {
                settings.scan_interval_ms = static_cast<uint32_t>(val);
                parsed_lines++;
            }
        } else if (key == "rssi_threshold_db") {
            char* endptr = nullptr;
            long val = strtol(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= -120 && val <= 0) {
                settings.rssi_threshold_db = static_cast<int32_t>(val);
                parsed_lines++;
            }
        } else if (key == "enable_audio_alerts") {
            settings.enable_audio_alerts = (value == "true");
            parsed_lines++;
        } else if (key == "audio_alert_frequency_hz") {
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= 200 && val <= 3000) {
                settings.audio_alert_frequency_hz = static_cast<uint16_t>(val);
                parsed_lines++;
            }
        } else if (key == "audio_alert_duration_ms") {
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= 50 && val <= 2000) {
                settings.audio_alert_duration_ms = static_cast<uint32_t>(val);
                parsed_lines++;
            }
        } else if (key == "enable_real_hardware") {
            settings.enable_real_hardware = (value == "true");
            parsed_lines++;
        } else if (key == "demo_mode") {
            settings.demo_mode = (value == "true");
            parsed_lines++;
        } else if (key == "hardware_bandwidth_hz") {
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= 1000000 && val <= 100000000) {
                settings.hardware_bandwidth_hz = static_cast<uint32_t>(val);
                parsed_lines++;
            }
        }
    }

    return parsed_lines >= 5;
}

bool load_settings_from_sd_card(DroneAnalyzerSettings& settings) {
    const std::string SETTINGS_FILE_PATH = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

    DroneAnalyzerSettings original_settings = settings;

    File settings_file;
    if (!settings_file.open(SETTINGS_FILE_PATH, true)) {
        return false;
    }

    std::string file_content;
    file_content.resize(settings_file.size());
    auto read_result = settings_file.read(file_content.data(), settings_file.size());
    if (read_result != settings_file.size()) {
        settings_file.close();
        settings = original_settings;
        return false;
    }
    settings_file.close();

    if (!parse_settings_from_content(file_content, settings)) {
        settings = original_settings;
        return false;
    }

    if (!validate_loaded_settings(settings)) {
        settings = original_settings;
        return false;
    }

    return true;
}

} // namespace ui::external_app::enhanced_drone_analyzer
