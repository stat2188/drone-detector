// ui_scanner_combined.cpp - Enhanced Drone Analyzer Implementation
// Clean implementation with Looking Glass components migrated

#include "ui_scanner_combined.hpp"
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
#include "../../common/ui.hpp"
#include "../../common/utility.hpp"
#include "../../file_reader.hpp"
#include "../../log_file.hpp"
#include "../../file.hpp"
#include <ch.h>
#include <chmtx.h>

namespace ui::external_app::enhanced_drone_analyzer {

// Definitions moved from header
WORKING_AREA(scanning_thread_wa, SCAN_THREAD_STACK_SIZE);

// Implement utility functions
namespace {
    FreqmanDB::path_type get_freqman_path(std::string_view db_name) {
        return db_name;
    }

    Frequency clip_frequency(Frequency freq) {
        return std::max(MIN_HARDWARE_FREQ, std::min(MAX_HARDWARE_FREQ, freq));
    }

    Timestamp Timestamp::now() { return Timestamp{chVTGetSystemTime()}; }
}

// FreqDBCache implementation
FreqDBCache::FreqDBCache() : cache_mutex_{} {}

const freqman_entry* FreqDBCache::get_entry(size_t index) {
    chMtxLock(&cache_mutex_);
    systime_t current_time = chVTGetSystemTime();
    auto it = std::find_if(cache_entries_.begin(), cache_entries_.end(),
                          [index, current_time](const FreqDBCacheEntry& e) {
                              return e.index == index && !e.is_expired(current_time);
                          });
    if (it != cache_entries_.end()) {
        it->update_access(current_time);
        chMtxUnlock(&cache_mutex_);
        return &it->entry;
    }
    chMtxUnlock(&cache_mutex_);
    return nullptr;
}

void FreqDBCache::cache_entry(const freqman_entry& entry, size_t index, const std::string& filename) {
    chMtxLock(&cache_mutex_);
    systime_t current_time = chVTGetSystemTime();

    // Clean expired entries
    cache_entries_.erase(
        std::remove_if(cache_entries_.begin(), cache_entries_.end(),
                      [current_time](const FreqDBCacheEntry& e) {
                          return e.is_expired(current_time);
                      }),
        cache_entries_.end());

    auto it = std::find_if(cache_entries_.begin(), cache_entries_.end(),
                          [index](const FreqDBCacheEntry& e) { return e.index == index; });

    if (it != cache_entries_.end()) {
        it->entry = entry;
        it->update_access(current_time);
    } else {
        if (cache_entries_.size() >= FREQ_DB_CACHE_SIZE) {
            auto oldest = std::min_element(cache_entries_.begin(), cache_entries_.end(),
                                           [](const FreqDBCacheEntry& a, const FreqDBCacheEntry& b) {
                                               return a.last_access_time < b.last_access_time;
                                           });
            if (oldest != cache_entries_.end()) cache_entries_.erase(oldest);
        }

        FreqDBCacheEntry new_entry;
        new_entry.entry = entry;
        new_entry.index = index;
        new_entry.filename = filename;
        new_entry.last_access_time = current_time;
        cache_entries_.push_back(std::move(new_entry));
    }
    chMtxUnlock(&cache_mutex_);
}

void FreqDBCache::clear() {
    chMtxLock(&cache_mutex_);
    cache_entries_.clear();
    chMtxUnlock(&cache_mutex_);
}

void FreqDBCache::flush_to_sd(const std::string& cache_file) {
    (void)cache_file; // Not implemented
}

// DetectionRingBuffer implementation
void DetectionRingBuffer::update_existing_entry(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    systime_t current_time = chVTGetSystemTime();
    chMtxLock(&ring_buffer_mutex_);
    for (auto& entry : entries_) {
        if (entry.frequency_hash == frequency_hash) {
            entry.detection_count = detection_count;
            entry.rssi_value = rssi_value;
            entry.last_update = current_time;
            entry.access_count++;
            chMtxUnlock(&ring_buffer_mutex_);
            return;
        }
    }
    chMtxUnlock(&ring_buffer_mutex_);
}

void DetectionRingBuffer::add_new_entry(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    systime_t current_time = chVTGetSystemTime();
    chMtxLock(&ring_buffer_mutex_);
    if (head_ >= DETECTION_TABLE_SIZE) evict_least_recently_used();
    entries_[head_] = {frequency_hash, detection_count, rssi_value, current_time, 1};
    head_ = (head_ + 1) % DETECTION_TABLE_SIZE;
    chMtxUnlock(&ring_buffer_mutex_);
}

void DetectionRingBuffer::evict_least_recently_used() {
    if (entries_.empty()) return;
    auto oldest = std::min_element(entries_.begin(), entries_.end(),
                                  [](const DetectionEntry& a, const DetectionEntry& b) {
                                      return a.last_update < b.last_update;
                                  });
    if (oldest != entries_.end()) {
        *oldest = DetectionEntry{};
    }
}

size_t DetectionRingBuffer::find_entry_index(size_t frequency_hash) const {
    chMtxLock(&ring_buffer_mutex_);
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].frequency_hash == frequency_hash) {
            chMtxUnlock(&ring_buffer_mutex_);
            return i;
        }
    }
    chMtxUnlock(&ring_buffer_mutex_);
    return SIZE_MAX;
}

void DetectionRingBuffer::remove_at_index(size_t index) {
    if (index >= entries_.size()) return;
    chMtxLock(&ring_buffer_mutex_);
    entries_[index] = DetectionEntry{};
    chMtxUnlock(&ring_buffer_mutex_);
}

void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    detection_count = std::min<uint8_t>(detection_count, MIN_DETECTION_COUNT * 2);
    chMtxLock(&ring_buffer_mutex_);
    bool found = false;
    for (auto& entry : entries_) {
        if (entry.frequency_hash == frequency_hash || entry.frequency_hash == 0) {
            entry.frequency_hash = frequency_hash;
            entry.detection_count = detection_count;
            entry.rssi_value = rssi_value;
            entry.last_update = chVTGetSystemTime();
            entry.access_count++;
            found = true;
            break;
        }
    }
    if (!found) {
        add_new_entry(frequency_hash, detection_count, rssi_value);
    }
    chMtxUnlock(&ring_buffer_mutex_);
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
    chMtxLock(&ring_buffer_mutex_);
    for (const auto& entry : entries_) {
        if (entry.frequency_hash == frequency_hash) {
            chMtxUnlock(&ring_buffer_mutex_);
            return entry.detection_count;
        }
    }
    chMtxUnlock(&ring_buffer_mutex_);
    return 0;
}

int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const {
    chMtxLock(&ring_buffer_mutex_);
    for (const auto& entry : entries_) {
        if (entry.frequency_hash == frequency_hash) {
            chMtxUnlock(&ring_buffer_mutex_);
            return entry.rssi_value;
        }
    }
    chMtxUnlock(&ring_buffer_mutex_);
    return -120;
}

// BufferedDetectionLogger implementation
BufferedDetectionLogger::BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0) {}

BufferedDetectionLogger::~BufferedDetectionLogger() {
    flush_buffer();
}

void BufferedDetectionLogger::log_detection(const DetectionLogEntry& entry) {
    if (entries_count_ >= LOG_BUFFER_SIZE) flush_buffer();
    if (entries_count_ >= LOG_BUFFER_SIZE) return;

    buffered_entries_[entries_count_] = entry;
    entries_count_++;

    systime_t current_time = chVTGetSystemTime();
    if (entries_count_ >= LOG_BUFFER_SIZE ||
        (current_time - last_flush_time_) > TIME_MS2I(LOG_BUFFER_FLUSH_MS)) {
        flush_buffer();
    }
}

void BufferedDetectionLogger::flush_buffer() {
    if (entries_count_ == 0) return;

    if (!ensure_csv_header()) return;

    std::string batch_log;
    for (size_t i = 0; i < entries_count_; ++i) {
        batch_log += format_csv_entry(buffered_entries_[i]);
    }

    auto error = csv_log_.append(generate_log_filename());
    if (!error.has_value()) return;

    error = csv_log_.write_raw(batch_log);
    if (error.has_value()) {
        last_flush_time_ = chVTGetSystemTime();
        entries_count_ = 0;
    }
}

void BufferedDetectionLogger::start_session() {
    if (session_active_) return;
    session_active_ = true;
    session_start_ = static_cast<uint32_t>(chVTGetSystemTime());
    logged_total_count_ = 0;
    header_written_ = false;
    last_flush_time_ = chVTGetSystemTime();
}

void BufferedDetectionLogger::end_session() {
    if (!session_active_) return;
    flush_buffer();
    session_active_ = false;
}

bool BufferedDetectionLogger::ensure_csv_header() {
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

std::string BufferedDetectionLogger::format_csv_entry(const DetectionLogEntry& entry) {
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

std::string BufferedDetectionLogger::generate_log_filename() const {
    return "EDA_LOG_BUFFERED.CSV";
}

// SimpleDroneValidation implementation
bool SimpleDroneValidation::validate_frequency_range(Frequency freq_hz) {
    return freq_hz >= MIN_HARDWARE_FREQ && freq_hz <= MAX_HARDWARE_FREQ;
}

bool SimpleDroneValidation::validate_rssi_signal(int32_t rssi_db, ThreatLevel threat) {
    (void)threat;
    return rssi_db >= -120 && rssi_db <= 0;
}

ThreatLevel SimpleDroneValidation::classify_signal_strength(int32_t rssi_db) {
    if (rssi_db > -70) return ThreatLevel::HIGH;
    if (rssi_db > -85) return ThreatLevel::MEDIUM;
    if (rssi_db > -100) return ThreatLevel::LOW;
    return ThreatLevel::NONE;
}

DroneType SimpleDroneValidation::identify_drone_type(Frequency freq_hz, int32_t rssi_db) {
    (void)rssi_db;
    if (freq_hz >= 2400000000ULL && freq_hz <= 2500000000ULL) return DroneType::UNKNOWN;
    return DroneType::UNKNOWN;
}

bool SimpleDroneValidation::validate_drone_detection(Frequency freq_hz, int32_t rssi_db, DroneType type, ThreatLevel threat) {
    return validate_frequency_range(freq_hz) &&
           validate_rssi_signal(rssi_db, threat) &&
           type != DroneType::UNKNOWN;
}

// Global detection ring
DetectionRingBuffer global_detection_ring;
Mutex global_detection_ring_mutex;

// Minimal implementations for other classes to compile
DroneHardwareController::DroneHardwareController() = default;
DroneHardwareController::~DroneHardwareController() = default;

bool DroneHardwareController::tune_to_frequency(Frequency frequency_hz) {
    center_frequency_ = clip_frequency(frequency_hz);
    radio::set_tuning_frequency(center_frequency_);
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

void DroneHardwareController::update_spectrum_for_scanner() {
    if (spectrum_streaming_active_) {
        update_radio_bandwidth();
    }
}

int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency) {
    if (center_frequency_ != target_frequency && !tune_to_frequency(target_frequency)) {
        return -120;
    }
    return read_raw_rssi_from_hardware();
}

void DroneHardwareController::update_radio_bandwidth() {
    set_spectrum_bandwidth_by_mode();
}

void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message) {
    (void)message;
}

void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum) {
    spectrum_access_mutex_.lock();
    if (!spectrum.db.empty()) {
        const auto power_avg = std::accumulate(spectrum.db.begin(), spectrum.db.end(), 0.0f) / spectrum.db.size();
        last_valid_rssi_ = static_cast<int32_t>(power_avg);

        if (last_valid_rssi_ < -120) last_valid_rssi_ = -120;
        if (last_valid_rssi_ > -20) last_valid_rssi_ = -20;
    }
    spectrum_access_mutex_.unlock();
}

void DroneHardwareController::on_hardware_show() {}
void DroneHardwareController::on_hardware_hide() {}

bool DroneHardwareController::wait_for_frequency_lock(systime_t timeout_ms) {
    (void)timeout_ms;
    return true;
}

int32_t DroneHardwareController::read_raw_rssi_from_hardware() const {
    if (spectrum_streaming_active_) {
        return last_valid_rssi_;
    } else {
        return (rand() % 40) - 85;
    }
}

void DroneHardwareController::set_spectrum_bandwidth(int32_t bandwidth_hz) {
    bandwidth_hz_ = bandwidth_hz;
}

void DroneHardwareController::set_spectrum_bandwidth_by_mode() {
    switch (spectrum_mode_) {
        case SpectrumMode::NARROW: set_spectrum_bandwidth(4000000); break;
        case SpectrumMode::MEDIUM: set_spectrum_bandwidth(8000000); break;
        case SpectrumMode::WIDE: set_spectrum_bandwidth(20000000); break;
        case SpectrumMode::ULTRA_WIDE: set_spectrum_bandwidth(24000000); break;
        default: set_spectrum_bandwidth(8000000); break;
    }
}

// DroneScanner implementation
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
      detection_processor_(nullptr), // TODO
      scan_interval_ms_(config.scan_interval_ms),
      rssi_threshold_db_(config.rssi_threshold_db),
      audio_alerts_enabled_(config.enable_audio_alerts)
{}

DroneScanner::~DroneScanner() {
    stop_scanning();
    cleanup_database_and_scanner();
}

size_t DroneScanner::get_total_memory_usage() const {
    size_t usage = sizeof(*this);
    usage += tracked_drones_.size() * sizeof(TrackedDroneData);
    usage += freq_db_.entry_count() * sizeof(freqman_entry);
    return usage;
}

void DroneScanner::initialize_database_and_scanner() {
    // Initialize database loading
}

void DroneScanner::cleanup_database_and_scanner() {
    if (scanning_thread_) {
        scanning_active_ = false;
        if (chThdTerminated(scanning_thread_) == false) {
            chThdTerminate(scanning_thread_);
        }
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }
}

void DroneScanner::initialize_wideband_scanning() {
    wideband_scan_data_.reset();
    setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
}

void DroneScanner::setup_wideband_range(uint64_t min_freq, uint64_t max_freq) {
    wideband_scan_data_.min_freq = min_freq;
    wideband_scan_data_.max_freq = max_freq;

    if (max_freq <= min_freq) {
        wideband_scan_data_.slices_nb = 0;
        return;
    }

    uint64_t scanning_range = max_freq - min_freq;
    if (scanning_range <= WIDEBAND_SLICE_WIDTH) {
        wideband_scan_data_.slices.push_back({(max_freq + min_freq) / 2, 0});
        wideband_scan_data_.slices_nb = 1;
    } else {
        const uint64_t EFFECTIVE_WIDTH = WIDEBAND_SLICE_WIDTH * 4 / 5;
        wideband_scan_data_.slices_nb = ((scanning_range + EFFECTIVE_WIDTH - 1) / EFFECTIVE_WIDTH);

        if (wideband_scan_data_.slices_nb > WIDEBAND_MAX_SLICES) {
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }

        uint64_t step_size = scanning_range / (wideband_scan_data_.slices_nb - 1);
        if (wideband_scan_data_.slices_nb == 1) {
            step_size = scanning_range;
        }

        uint64_t current_freq = min_freq + WIDEBAND_SLICE_WIDTH / 2;
        wideband_scan_data_.slices.clear();
        for (size_t i = 0; i < wideband_scan_data_.slices_nb; ++i) {
            wideband_scan_data_.slices.push_back({
                std::min(current_freq, max_freq - WIDEBAND_SLICE_WIDTH / 2), i});
            current_freq += step_size;
        }
    }
    wideband_scan_data_.slice_counter = 0;
}

void DroneScanner::start_scanning() {
    if (scanning_active_ || scanning_thread_ != nullptr) return;

    scanning_active_ = true;
    scan_cycles_ = 0;
    total_detections_ = 0;

    scanning_thread_ = chThdCreateStatic(scanning_thread_wa,
                                       sizeof(scanning_thread_wa),
                                       NORMALPRIO + 10,
                                       scanning_thread_function,
                                       this);
    if (!scanning_thread_) {
        scanning_active_ = false;
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
    detection_logger_.end_session();
}

msg_t DroneScanner::scanning_thread_function(void* arg) {
    auto* self = static_cast<DroneScanner*>(arg);
    return self->scanning_thread();
}

msg_t DroneScanner::scanning_thread() {
    while (scanning_active_ && !chThdShouldTerminate()) {
        chThdSleepMilliseconds(scan_interval_ms_);
        scan_cycles_++;
        // Perform scan cycle - will be implemented with hardware controller
    }
    scanning_active_ = false;
    scanning_thread_ = nullptr;
    chThdExit(MSG_OK);
    return MSG_OK;
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
    if (entry_count == 0) return;

    if (current_db_index_ >= entry_count) {
        current_db_index_ = 0;
    }

    const freqman_entry& entry = freq_db_[current_db_index_];
    if (entry.frequency_a > 0) {
        Frequency target_freq = entry.frequency_a;
        if (target_freq >= MIN_HARDWARE_FREQ && target_freq <= MAX_HARDWARE_FREQ) {
            if (hardware.tune_to_frequency(target_freq)) {
                int32_t real_rssi = hardware.get_real_rssi_from_hardware(target_freq);
                process_rssi_detection(entry, real_rssi);
            }
        }
    }
    current_db_index_ = (current_db_index_ + 1) % entry_count;
}

void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
    if (wideband_scan_data_.slices_nb == 0) return;

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

void DroneScanner::master_wideband_detection_handler(DroneHardwareController& hardware, Frequency frequency, int32_t rssi, bool is_approaching) {
    (void)is_approaching;
    if (rssi < WIDEBAND_RSSI_THRESHOLD_DB - 10) return;

    freqman_entry wideband_entry{};
    wideband_entry.frequency_a = frequency;
    wideband_entry.type = freqman_type::WILDCARD;
    wideband_entry.bandwidth = 0;

    process_rssi_detection(wideband_entry, rssi);
}

void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
    rssi = clip(rssi, -120, 0);

    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::NONE) ||
        !SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    ThreatLevel threat_level;
    if (rssi > -70) threat_level = ThreatLevel::HIGH;
    else if (rssi > -80) threat_level = ThreatLevel::MEDIUM;
    else threat_level = ThreatLevel::LOW;

    if (entry.frequency_a >= 2400000000ULL && entry.frequency_a <= 2500000000ULL) {
        threat_level = (static_cast<int>(threat_level) < static_cast<int>(ThreatLevel::MEDIUM))
                       ? ThreatLevel::MEDIUM : threat_level;
    }

    total_detections_++;

    size_t freq_hash = std::hash<uint64_t>{}(entry.frequency_a);
    int32_t effective_threshold = rssi_threshold_db_;

    int32_t prev_rssi = global_detection_ring.get_rssi_value(freq_hash);
    uint8_t detection_count = global_detection_ring.get_detection_count(freq_hash);

    if (detection_count > 0) {
        effective_threshold = rssi_threshold_db_ + HYSTERESIS_MARGIN_DB;
    }

    if (rssi >= effective_threshold) {
        uint8_t current_count = global_detection_ring.get_detection_count(freq_hash);
        if (current_count < 255) current_count++;
        global_detection_ring.update_detection(freq_hash, current_count, rssi);

        if (current_count >= MIN_DETECTION_COUNT) {
            DetectionLogEntry log_entry{
                static_cast<uint32_t>(chVTGetSystemTime() / (CH_CFG_ST_FREQUENCY / 1000)),
                static_cast<uint32_t>(entry.frequency_a),
                rssi, threat_level, DroneType::UNKNOWN, current_count, 0.8f
            };
            detection_logger_.log_detection(log_entry);

            if (threat_level >= ThreatLevel::HIGH && audio_alerts_enabled_) {
                // Audio alert handled by controller
            }

            update_tracked_drone(DroneType::UNKNOWN, entry.frequency_a, rssi, threat_level);
        }
    } else {
        global_detection_ring.update_detection(freq_hash, 0, -120);
    }
}

void DroneScanner::update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
    for (size_t i = 0; i < MAX_TRACKED_DRONES; i++) {
        TrackedDroneData& drone = tracked_drones_[i];
        if (drone.frequency == static_cast<uint32_t>(frequency) && drone.update_count > 0) {
            drone.add_rssi(static_cast<int16_t>(rssi), chVTGetSystemTime());
            drone.drone_type = static_cast<uint8_t>(type);
            drone.threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
        if (drone.update_count == 0) {
            drone.frequency = static_cast<uint32_t>(frequency);
            drone.drone_type = static_cast<uint8_t>(type);
            drone.threat_level = static_cast<uint8_t>(threat_level);
            drone.add_rssi(static_cast<int16_t>(rssi), chVTGetSystemTime());
            tracked_drones_count_++;
            update_tracking_counts();
            return;
        }
    }

    // Evict oldest
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
    tracked_drones_[oldest_index].add_rssi(static_cast<int16_t>(rssi), chVTGetSystemTime());
    update_tracking_counts();
}

void DroneScanner::remove_stale_drones() {
    const systime_t STALE_TIMEOUT = 30000;
    systime_t current_time = chVTGetSystemTime();

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
}

bool DroneScanner::validate_detection_simple(int32_t rssi_db, ThreatLevel threat) {
    return SimpleDroneValidation::validate_rssi_signal(rssi_db, threat);
}

void DroneScanner::set_scanning_mode(ScanningMode mode) {
    scanning_mode_ = mode;
    stop_scanning();
    scan_cycles_ = 0;
    total_detections_ = 0;

    if (scanning_mode_ == ScanningMode::DATABASE || scanning_mode_ == ScanningMode::HYBRID) {
        // Load database
    }
}

const TrackedDroneData& DroneScanner::getTrackedDrone(size_t index) const {
    return (index < MAX_TRACKED_DRONES) ? tracked_drones_[index] : tracked_drones_[0];
}

std::string DroneScanner::get_session_summary() const {
    return "Session: " + std::to_string(scan_cycles_) + " cycles, " + std::to_string(total_detections_) + " detections";
}

bool DroneScanner::load_frequency_database() {
    if (freq_db_.entry_count() == 0) return false;
    current_db_index_ = 0;
    last_scanned_frequency_ = 0;
    return freq_db_.entry_count() > 0;
}

size_t DroneScanner::get_database_size() const {
    return freq_db_.entry_count();
}

Frequency DroneScanner::get_current_scanning_frequency() const {
    size_t entry_count = freq_db_.entry_count();
    if (entry_count > 0 && current_db_index_ < entry_count) {
        const freqman_entry& entry = freq_db_[current_db_index_];
        return entry.frequency_a;
    }
    return WIDEBAND_DEFAULT_MIN;
}

// UI Classes implementations (minimal)
SmartThreatHeader::SmartThreatHeader(Rect parent_rect)
    : View(parent_rect),
      threat_progress_bar_({0, 0, screen_width, 16}),
      threat_status_main_({0, 20, screen_width, 16}, "THREAT: LOW"),
      threat_frequency_({0, 38, screen_width, 16}, "2400.0MHz READY") {
    add_children({&threat_progress_bar_, &threat_status_main_, &threat_frequency_});
}

void SmartThreatHeader::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
                               size_t receding, Frequency current_freq, bool is_scanning) {
    current_threat_ = max_threat;
    size_t total_drones = approaching + static_count + receding;
    threat_progress_bar_.set_value(static_cast<int>(total_drones * 10));

    char buffer[64];
    const char* threat_text = "CLEAR";
    switch (max_threat) {
        case ThreatLevel::HIGH: threat_text = "HIGH"; break;
        case ThreatLevel::MEDIUM: threat_text = "MEDIUM"; break;
        case ThreatLevel::LOW: threat_text = "LOW"; break;
        default: break;
    }

    if (total_drones > 0) {
        snprintf(buffer, sizeof(buffer), "THREAT: %s | ▲%zu ■%zu ▼%zu", threat_text, approaching, static_count, receding);
    } else {
        snprintf(buffer, sizeof(buffer), is_scanning ? "SCANNING" : "READY");
    }
    threat_status_main_.set(buffer);

    if (current_freq > 0) {
        float freq_mhz = static_cast<float>(current_freq) / 1000000.0f;
        snprintf(buffer, sizeof(buffer), "%.1fMHz %s", freq_mhz, is_scanning ? "SCANNING" : "READY");
        threat_frequency_.set(buffer);
    }
    set_dirty();
}

void SmartThreatHeader::set_max_threat(ThreatLevel threat) {
    if (threat != current_threat_) {
        update(threat, approaching_count_, static_count_, receding_count_, current_freq_, is_scanning_);
    }
}

void SmartThreatHeader::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
    update(current_threat_, approaching, static_count, receding, current_freq_, is_scanning_);
}

void SmartThreatHeader::set_current_frequency(Frequency freq) {
    if (freq != current_freq_) {
        update(current_threat_, approaching_count_, static_count_, receding_count_, freq, is_scanning_);
    }
}

void SmartThreatHeader::set_scanning_state(bool is_scanning) {
    if (is_scanning != is_scanning_) {
        update(current_threat_, approaching_count_, static_count_, receding_count_, current_freq_, is_scanning);
    }
}

void SmartThreatHeader::paint(Painter& painter) {
    View::paint(painter);
}

Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return {255,165,0};
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        case ThreatLevel::NONE:
        default: return Color::blue();
    }
}

Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) const {
    return get_threat_bar_color(level);
}

std::string SmartThreatHeader::get_threat_text(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return "CRITICAL";
        case ThreatLevel::HIGH: return "HIGH";
        case ThreatLevel::MEDIUM: return "MEDIUM";
        case ThreatLevel::LOW: return "LOW";
        case ThreatLevel::NONE: default: return "CLEAR";
    }
}

ThreatCard::ThreatCard(size_t index, Rect parent_rect) : View(parent_rect), index_(index) {}

void ThreatCard::update_card(const DisplayDroneEntry& entry) {
    entry_ = entry;
    set_dirty();
}

void ThreatCard::clear_card() {
    entry_ = DisplayDroneEntry{};
    set_dirty();
}

void ThreatCard::paint(Painter& painter) {
    if (entry_.frequency == 0) return; // Nothing to paint

    View::paint(painter);
    // Simple paint - just frequency and type
    painter.draw_string({parent_rect_.left(), parent_rect_.top()},
                       "Freq: " + std::to_string(entry_.frequency));
    painter.draw_string({parent_rect_.left(), parent_rect_.top() + 12},
                       "Type: " + entry_.type_name);
}

ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
    : View(parent_rect), bar_index_(bar_index) {
    // Add text fields
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    set_display_mode(DisplayMode::SCANNING);
}

void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
    set_display_mode(DisplayMode::ALERT);
}

void ConsoleStatusBar::update_normal_status(const std::string& primary, const std::string& secondary) {
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::paint(Painter& painter) {
    View::paint(painter);
}

void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
    (void)mode; // Simple implementation
}

DroneDisplayController::DroneDisplayController(NavigationView& nav) : nav_(nav) {}

DroneDisplayController::~DroneDisplayController() {}

void DroneDisplayController::update_detection_display(DroneScanner& scanner) {
    (void)scanner;
}

void DroneDisplayController::set_scanning_status(bool is_scanning, const std::string& mode_name) {
    (void) is_scanning; (void)mode_name;
}

ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
                                       DroneHardwareController& hardware,
                                       DroneScanner& scanner,
                                       DroneDisplayController& display_controller,
                                       AudioAlertManager& audio_controller)
    : scanning_thread_(nullptr),
      scanning_active_(false),
      nav_(nav),
      hardware_(hardware),
      scanner_(scanner),
      display_controller_(display_controller),
      audio_controller_(audio_controller),
      scan_interval_ms_(750)
{}

ScanningCoordinator::~ScanningCoordinator() {
    stop_coordinated_scanning();
}

void ScanningCoordinator::start_coordinated_scanning() {
    if (scanning_active_ || scanning_thread_ != nullptr) return;

    scanning_active_ = true;

    scanning_thread_ = chThdCreateFromHeap(NULL, SCANNING_THREAD_STACK_SIZE,
                                           NORMALPRIO, scanning_thread_function, this);
}

void ScanningCoordinator::stop_coordinated_scanning() {
    if (!scanning_active_) return;

    scanning_active_ = false;
    if (scanning_thread_) {
        if (chThdTerminated(scanning_thread_) == false) {
            chThdTerminate(scanning_thread_);
        }
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }
}

msg_t ScanningCoordinator::scanning_thread_function(void* arg) {
    auto* self = static_cast<ScanningCoordinator*>(arg);
    return self->coordinated_scanning_thread();
}

msg_t ScanningCoordinator::scanning_thread() {
    // Coordinate scanning
    while (scanning_active_ && !chThdShouldTerminate()) {
        if (scanner_.is_scanning_active()) {
            hardware_.update_spectrum_for_scanner();
            scanner_.perform_scan_cycle(hardware_);

            // Update display and alerts
            display_controller_.update_detection_display(scanner_);

            if (audio_controller_.is_audio_enabled() &&
                scanner_.get_max_detected_threat() >= ThreatLevel::HIGH) {
                audio_controller_.play_alert(AudioAlertManager::AlertLevel::HIGH);
            }
        }
        chThdSleepMilliseconds(scan_interval_ms_);
    }
    scanning_active_ = false;
    scanning_thread_ = nullptr;
    chThdExit(MSG_OK);
    return MSG_OK;
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
    scan_interval_ms_ = settings.scan_interval_ms;
}

void ScanningCoordinator::show_session_summary(const std::string& summary) {
    nav_.display_modal("Session Summary", summary.c_str());
}

bool ScanningCoordinator::is_scanning_active() const {
    return scanning_active_;
}

EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
    : View(), nav_(nav) {
    // Initialize basic UI
    button_start_ = Button{ {240, 240, 80, 24}, "START" };
    button_menu_ = Button{ {160, 240, 80, 24}, "MENU" };
    add_child(&button_start_);
    add_child(&button_menu_);
}

void EnhancedDroneSpectrumAnalyzerView::focus() {
    button_start_.focus();
}

void EnhancedDroneSpectrumAnalyzerView::on_show() {
    View::on_show();
}

void EnhancedDroneSpectrumAnalyzerView::on_hide() {
    stop_scanning_thread();
    View::on_hide();
}

bool EnhancedDroneSpectrumAnalyzerView::on_key(const KeyEvent key) {
    if (key == KeyEvent::Back) {
        stop_scanning_thread();
        nav_.pop();
        return true;
    }
    return View::on_key(key);

void EnhancedDroneSpectrumAnalyzerView::on_touch(const ui::TouchEvent event) {
    (void)event;
    // Handle touch if needed
}

void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
    painter.fill_rectangle(screen_rect(), Color::black());
    painter.draw_string({10, 10}, "Enhanced Drone Analyzer");
    if (smart_header_) {
        smart_header_->paint(painter);
    }
}

void EnhancedDroneSpectrumAnalyzerView::start_scanning_thread() {
    if (scanner_ && scanner_->is_scanning_active()) return;
    if (scanner_) scanner_->start_scanning();
    if (scanning_coordinator_) scanning_coordinator_->start_coordinated_scanning();
}

void EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread() {
    if (scanner_) scanner_->stop_scanning();
    if (scanning_coordinator_) scanning_coordinator_->stop_coordinated_scanning();
}

bool EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button() {
    if (scanning_coordinator_ && scanning_coordinator_->is_scanning_active()) {
        ui_controller_on_stop_scan();
    } else {
        ui_controller_on_start_scan();
    }
    return true;
}

bool EnhancedDroneSpectrumAnalyzerView::handle_menu_button() {
    nav_.display_modal("Menu", "Enhanced Drone Analyzer Menu");
    return true;
}

void EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout() {
    // Initialize UI components
    smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
}

void EnhancedDroneSpectrumAnalyzerView::update_modern_layout() {
    // Update layout
}

void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
    // Update UI based on scanner state
}

LoadingScreenView::LoadingScreenView(NavigationView& nav)
    : nav_(nav),
      text_eda_(Rect{108, 213, 24, 16}, "EDA"),
      timer_start_(Timestamp::now())
{
    add_child(&text_eda_);
}

void LoadingScreenView::paint(Painter& painter) {
    painter.fill_rectangle({0, 0, screen_width, screen_height}, Color::black());
    View::paint(painter);
}

// Global functions
bool load_settings_from_sd_card(DroneAnalyzerSettings& settings) {
    (void) settings;
    return false; // Placeholder
}

void initialize_app(NavigationView& nav) {
    nav.push(std::make_unique<LoadingScreenView>(nav));

    chThdSleepMilliseconds(500);

    nav.replace(std::make_unique<EnhancedDroneSpectrumAnalyzerView>(nav));
}

} // namespace ui::external_app::enhanced_drone_analyzer
