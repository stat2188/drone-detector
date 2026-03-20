#include "scanner.hpp"
#include "receiver_model.hpp"
#include "portapack_persistent_memory.hpp"

#include "ch.h"

namespace drone_analyzer {

static int32_t extract_rssi(const ChannelSpectrum& spectrum) noexcept {
    uint8_t max_power = 0;
    for (size_t i = 0; i < spectrum.db.size(); ++i) {
        const uint8_t p = static_cast<uint8_t>(spectrum.db[i]);
        if (p > max_power) max_power = p;
    }
    return static_cast<int32_t>(max_power) - 120;
}

// ============================================================================
// ScanConfig Implementation
// ============================================================================

ScanConfig::ScanConfig() noexcept
    : mode(DEFAULT_SCANNING_MODE)
    , start_frequency(MIN_FREQUENCY_HZ)
    , end_frequency(MAX_FREQUENCY_HZ)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , rssi_threshold_dbm(RSSI_DETECTION_THRESHOLD_DBM)
    , stale_timeout_ms(DRONE_STALE_TIMEOUT_MS) {
}

ScanConfig::ScanConfig(ScanningMode m, FreqHz start, FreqHz end) noexcept
    : mode(m)
    , start_frequency(start)
    , end_frequency(end)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , rssi_threshold_dbm(RSSI_DETECTION_THRESHOLD_DBM)
    , stale_timeout_ms(DRONE_STALE_TIMEOUT_MS) {
}

// ============================================================================
// ScanStatistics Implementation
// ============================================================================

ScanStatistics::ScanStatistics() noexcept
    : total_scan_cycles(0)
    , successful_cycles(0)
    , failed_cycles(0)
    , drones_detected(0)
    , max_rssi_dbm(RSSI_NOISE_FLOOR_DBM) {
}

void ScanStatistics::reset() noexcept {
    total_scan_cycles = 0;
    successful_cycles = 0;
    failed_cycles = 0;
    drones_detected = 0;
    max_rssi_dbm = RSSI_NOISE_FLOOR_DBM;
}

// ============================================================================
// DroneScanner Implementation
// ============================================================================

DroneScanner::DroneScanner(DatabaseManager& database, HardwareController& hardware) noexcept
    : database_(database)
    , hardware_(hardware)
    , state_(ScannerState::IDLE)
    , config_()
    , freq_lock_count_{0}
    , locked_frequency_{0}
    , track_start_time_{0}
    , current_drone_type_{'\0', '\0', '\0', '\0', '\0'}
    , drone_type_valid_{false}
    , statistics_()
    , tracked_drones_()
    , tracked_count_{0}
    , current_frequency_{0}
    , last_scan_time_{0}
    , scanning_active_()
    , alert_callback_(nullptr)
    , mutex_()
    , state_transition_allowed_()
    , alert_callback_in_progress_()
    , rssi_detector_()
    , histogram_processor_() {

    // Initialize mutex
    chMtxInit(&mutex_);

    (void)rssi_detector_.initialize(RSSI_DETECTION_THRESHOLD_DBM);
    (void)histogram_processor_.initialize(HISTOGRAM_BIN_COUNT);
}

DroneScanner::~DroneScanner() noexcept {
    // Stop scanning if active
    if (scanning_active_.test()) {
        (void)stop_scanning();
    }

    // Note: Do NOT call chMtxDeinit - it doesn't exist in ChibiOS
}

ErrorCode DroneScanner::initialize() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (state_ != ScannerState::IDLE) {
        return ErrorCode::INITIALIZATION_INCOMPLETE;
    }

    ErrorCode hw_result = hardware_.initialize();
    if (hw_result != ErrorCode::SUCCESS) {
        return hw_result;
    }

    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(0);
    if (!freq_result.has_value()) {
        return freq_result.error();
    }
    current_frequency_ = freq_result.value();

    state_ = ScannerState::IDLE;
    statistics_.reset();

    return ErrorCode::SUCCESS;
}

// ============================================================================
// Fast Scanner Integration Getters
// ============================================================================

uint32_t DroneScanner::get_freq_lock_count() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return __atomic_load_n(&freq_lock_count_, __ATOMIC_RELAXED);
    }
    return freq_lock_count_;
}

void DroneScanner::set_freq_lock_count(uint32_t count) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    freq_lock_count_ = count;
}

FreqHz DroneScanner::get_locked_frequency() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return __atomic_load_n(&locked_frequency_, __ATOMIC_RELAXED);
    }
    return locked_frequency_;
}

// ============================================================================
// Scanner Control Methods (Restored for functionality)
// ============================================================================

ErrorCode DroneScanner::start_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (state_ == ScannerState::SCANNING) {
        return ErrorCode::SUCCESS;
    }

    ErrorCode hw_result = hardware_.start_spectrum_streaming();
    if (hw_result != ErrorCode::SUCCESS) {
        return hw_result;
    }

    state_ = ScannerState::SCANNING;
    scanning_active_.set();

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::stop_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (state_ == ScannerState::IDLE) {
        return ErrorCode::SUCCESS;
    }

    state_ = ScannerState::IDLE;
    scanning_active_.clear();

    (void)hardware_.stop_spectrum_streaming();

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::pause_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ != ScannerState::SCANNING) {
        return ErrorCode::SUCCESS;
    }
    
    state_ = ScannerState::PAUSED;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::resume_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ != ScannerState::PAUSED) {
        return ErrorCode::SUCCESS;
    }
    
    state_ = ScannerState::SCANNING;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::perform_scan_cycle() noexcept {
    if (!scanning_active_.test()) {
        return ErrorCode::SUCCESS;
    }
    
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ == ScannerState::PAUSED) {
        return ErrorCode::SUCCESS;
    }
    
    if (state_ != ScannerState::SCANNING) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    return perform_scan_cycle_internal();
}

ErrorCode DroneScanner::perform_scan_cycle_internal() noexcept {
    statistics_.total_scan_cycles++;
    
    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(current_frequency_);
    if (!freq_result.has_value()) {
        statistics_.failed_cycles++;
        
        // Handle empty database gracefully
        if (freq_result.error() == ErrorCode::DATABASE_EMPTY) {
            return ErrorCode::DATABASE_EMPTY;
        }
        
        return freq_result.error();
    }
    
    current_frequency_ = freq_result.value();
    
    ErrorCode tune_result = hardware_.tune_to_frequency(current_frequency_);
    if (tune_result != ErrorCode::SUCCESS) {
        statistics_.failed_cycles++;
        return tune_result;
    }
    
    statistics_.successful_cycles++;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::update_tracked_drones(
    FreqHz frequency,
    RssiValue rssi,
    SystemTime timestamp
) noexcept {
    if (frequency < MIN_FREQUENCY_HZ || frequency > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;
    }

    if (rssi < RSSI_MIN_DBM || rssi > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }

    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    return update_tracked_drone_internal(frequency, rssi, timestamp);
}

ErrorResult<RssiValue> DroneScanner::process_spectrum_data(
    const ChannelSpectrum& spectrum,
    FreqHz current_frequency
) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (current_frequency == 0) {
        return ErrorResult<RssiValue>::failure(ErrorCode::INVALID_PARAMETER);
    }

    const int32_t rssi = extract_rssi(spectrum);

    if (rssi > RSSI_DETECTION_THRESHOLD_DBM) {
        const ErrorCode err = update_tracked_drone_internal(
            current_frequency,
            rssi,
            chTimeNow()
        );

        if (err != ErrorCode::SUCCESS) {
            return ErrorResult<RssiValue>::failure(err);
        }
    }

    return ErrorResult<RssiValue>::success(rssi);
}

ErrorCode DroneScanner::process_spectrum_message(const ChannelSpectrum& spectrum) noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (!lock.is_locked()) {
        return ErrorCode::MUTEX_LOCK_FAILED;
    }

    if (current_frequency_ == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }

    // Feed spectrum to histogram processor for noise floor analysis
    (void)histogram_processor_.update_histogram(spectrum.db.data(), spectrum.db.size());

    const int32_t rssi = extract_rssi(spectrum);
    const SystemTime now = chTimeNow();

    // Feed RSSI detector for threat classification
    (void)rssi_detector_.process_rssi_sample(rssi, now);

    if (rssi > RSSI_DETECTION_THRESHOLD_DBM) {
        const ErrorCode err = update_tracked_drone_internal(
            current_frequency_,
            rssi,
            now
        );

        if (err != ErrorCode::SUCCESS) {
            return err;
        }

        // Update statistics
        statistics_.drones_detected++;
        if (rssi > static_cast<int32_t>(statistics_.max_rssi_dbm)) {
            statistics_.max_rssi_dbm = static_cast<uint32_t>(rssi);
        }

        if (current_frequency_ == locked_frequency_) {
            if (freq_lock_count_ < MAX_FREQ_LOCK) {
                freq_lock_count_++;
            }
            // Transition: LOCKING → TRACKING after sustained lock
            if (freq_lock_count_ >= MAX_FREQ_LOCK && state_ == ScannerState::SCANNING) {
                state_ = ScannerState::TRACKING;
            }
        } else {
            locked_frequency_ = current_frequency_;
            freq_lock_count_ = 1;
            // Transition: SCANNING → LOCKING on first detection
            if (state_ == ScannerState::SCANNING) {
                state_ = ScannerState::LOCKING;
            }
        }
    } else {
        freq_lock_count_ = 0;
        locked_frequency_ = 0;
        // Transition: LOCKING/TRACKING → SCANNING when signal lost
        if (state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING) {
            state_ = ScannerState::SCANNING;
        }
    }

    return ErrorCode::SUCCESS;
}



ErrorCode DroneScanner::update_tracked_drone_internal(
    FreqHz frequency,
    RssiValue rssi,
    SystemTime timestamp
) noexcept {
    ErrorResult<size_t> index_result = find_drone_by_frequency_internal(frequency);
    
    if (index_result.has_value()) {
        size_t index = index_result.value();
        
        ThreatLevel old_threat = tracked_drones_[index].get_threat();
        
        tracked_drones_[index].update_rssi(rssi, timestamp);
        
        ThreatLevel new_threat = tracked_drones_[index].get_threat();
        
        if (new_threat > old_threat) {
            trigger_alert(new_threat);
        }
    } else {
        ErrorCode add_result = add_tracked_drone_internal(frequency, rssi, timestamp);
        if (add_result != ErrorCode::SUCCESS) {
            return add_result;
        }
    }

    return ErrorCode::SUCCESS;
}

ErrorResult<size_t> DroneScanner::find_drone_by_frequency_internal(
    FreqHz frequency
) const noexcept {
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones_[i].frequency == frequency) {
            return ErrorResult<size_t>::success(i);
        }
    }
    
    return ErrorResult<size_t>::failure(ErrorCode::INVALID_PARAMETER);
}

ErrorCode DroneScanner::get_current_drone_type(char* buffer, size_t buffer_size) const noexcept {
    if (buffer == nullptr || buffer_size < 2) {
        return ErrorCode::INVALID_PARAMETER;
    }

    size_t copy_len = 0;
    for (size_t i = 0; i < 4 && i < (buffer_size - 1); ++i) {
        const char c = current_drone_type_[i];
        if (c != '\0') {
            buffer[i] = c;
            copy_len++;
        } else {
            break;
        }
    }

    buffer[copy_len] = '\0';
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::add_tracked_drone_internal(
    FreqHz frequency_hz,
    RssiValue rssi_dbm,
    SystemTime timestamp_ms
) noexcept {
    if (tracked_count_ >= MAX_TRACKED_DRONES) {
        return ErrorCode::BUFFER_FULL;
    }

    DroneType type = determine_drone_type_internal(frequency_hz);

    tracked_drones_[tracked_count_] = TrackedDrone(frequency_hz, type, ThreatLevel::NONE);
    tracked_drones_[tracked_count_].update_rssi(rssi_dbm, timestamp_ms);

    tracked_count_++;
    statistics_.drones_detected++;

    trigger_alert(tracked_drones_[tracked_count_ - 1].get_threat());

    return ErrorCode::SUCCESS;
}

DroneType DroneScanner::determine_drone_type_internal(FreqHz frequency) const noexcept {
    ErrorResult<FrequencyEntry> entry_result = database_.find_entry(frequency);
    
    if (entry_result.has_value()) {
        const FrequencyEntry& entry = entry_result.value();
        return entry.drone_type;
    }
    
    return DroneType::UNKNOWN;
}

size_t DroneScanner::get_tracked_drones(
    TrackedDrone* drones,
    size_t max_count
) const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (drones == nullptr || max_count == 0) {
        return 0;
    }
    
    size_t copy_count = tracked_count_ < max_count ? tracked_count_ : max_count;
    
    for (size_t i = 0; i < copy_count; ++i) {
        drones[i] = tracked_drones_[i];
    }
    
    return copy_count;
}

ScannerState DroneScanner::get_state() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (lock.is_locked()) {
        return state_;
    }
    // Fallback: return last known state (atomic word-sized read is safe on Cortex-M4)
    return state_;
}

bool DroneScanner::is_scanning() const noexcept {
    return scanning_active_.test();
}

ScanConfig DroneScanner::get_config() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    return config_;
}

ErrorCode DroneScanner::set_config(const ScanConfig& config) noexcept {
    ErrorCode validate_result = validate_config_internal(config);
    if (validate_result != ErrorCode::SUCCESS) {
        return validate_result;
    }
    
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    config_ = config;
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::validate_config_internal(const ScanConfig& config) const noexcept {
    if (config.start_frequency < MIN_FREQUENCY_HZ ||
        config.end_frequency > MAX_FREQUENCY_HZ ||
        config.start_frequency >= config.end_frequency) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (config.rssi_threshold_dbm < RSSI_MIN_DBM ||
        config.rssi_threshold_dbm > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

ScanStatistics DroneScanner::get_statistics() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    return statistics_;
}

void DroneScanner::reset_statistics() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    statistics_.reset();
}

ErrorResult<FreqHz> DroneScanner::get_current_frequency() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (current_frequency_ == 0) {
        return ErrorResult<FreqHz>::failure(ErrorCode::HARDWARE_NOT_INITIALIZED);
    }
    return ErrorResult<FreqHz>::success(current_frequency_);
}

size_t DroneScanner::get_tracked_count() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return tracked_count_;  // Atomic word-read fallback on Cortex-M4
    }
    return tracked_count_;
}

void DroneScanner::clear_tracked_drones() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    tracked_count_ = 0;
}

void DroneScanner::remove_stale_drones(SystemTime current_time) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    remove_stale_drones_internal(current_time);
}

void DroneScanner::remove_stale_drones_internal(SystemTime current_time) noexcept {
    size_t write_index = 0;
    
    for (size_t read_index = 0; read_index < tracked_count_; ++read_index) {
        if (!tracked_drones_[read_index].is_stale(current_time, config_.stale_timeout_ms)) {
            if (write_index != read_index) {
                tracked_drones_[write_index] = tracked_drones_[read_index];
            }
            write_index++;
        }
    }
    
    tracked_count_ = write_index;
}

/**
 * @brief Sets the alert callback function
 * @param callback Function to call when alerts are triggered
 * @note The callback function MUST be thread-safe and reentrant-safe
 * @note The callback MUST NOT acquire any mutexes or perform blocking operations
 * @note The callback MUST execute quickly (preferably < 1ms) to avoid delaying scanner thread
 * @note Failure to meet timing requirements may result in missed alerts or buffer overflows
 * @warning Violating these constraints can cause system instability
 */
void DroneScanner::set_alert_callback(ThreatAlertCallback callback) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    alert_callback_ = callback;
}

void DroneScanner::trigger_alert(ThreatLevel threat_level) noexcept {
    // Copy callback pointer while potentially under lock
    // If called from process_spectrum_message(), mutex is already held via TryLock.
    // If called from scanner thread, mutex is already held via MutexLock.
    // Either way, we can safely read the pointer (atomic word-sized read on Cortex-M4).
    ThreatAlertCallback local_callback = alert_callback_;

    if (local_callback == nullptr) {
        return;
    }

    // Re-entrancy guard (AtomicFlag is lock-free)
    if (alert_callback_in_progress_.test_and_set()) {
        return;  // Already in progress
    }

    // Invoke callback outside any lock
    local_callback(threat_level);

    alert_callback_in_progress_.clear();
}

} // namespace drone_analyzer
