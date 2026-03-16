#include "scanner.hpp"

// ChibiOS headers (will be available when integrated)
// #include "ch.h"

namespace drone_analyzer {

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
    , statistics_()
    , tracked_drones_()
    , tracked_count_(0)
    , current_frequency_(0)
    , last_scan_time_(0)
    , scanning_active_()
    , alert_callback_(nullptr)
    , last_threat_levels_{}
    , mutex_()
    , state_transition_allowed_()
    , scan_cycle_in_progress_() {

    // Initialize mutex
    chMtxObjectInit(&mutex_);

    // Initialize last threat levels to NONE
    for (size_t i = 0; i < MAX_TRACKED_DRONES; ++i) {
        last_threat_levels_[i] = ThreatLevel::NONE;
    }
}

DroneScanner::~DroneScanner() noexcept {
    // Stop scanning if active
    if (scanning_active_.test()) {
        stop_scanning();
    }
    
    // Note: Do NOT call chMtxDeinit - it doesn't exist in ChibiOS
}

ErrorCode DroneScanner::initialize() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (state_ != ScannerState::IDLE) {
        return ErrorCode::INITIALIZATION_INCOMPLETE;
    }

    // Check if database is loaded
    if (!database_.is_loaded()) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }

    // Initialize dependencies
    ErrorCode db_result = database_.load_frequency_database();
    if (db_result != ErrorCode::SUCCESS && db_result != ErrorCode::DATABASE_EMPTY) {
        return db_result;
    }

    ErrorCode hw_result = hardware_.initialize();
    if (hw_result != ErrorCode::SUCCESS) {
        return hw_result;
    }

    // Get first frequency from database
    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(0);
    if (freq_result.has_value()) {
        current_frequency_ = freq_result.value();
    } else {
        current_frequency_ = DEFAULT_SCAN_FREQUENCY_HZ;
    }

    state_ = ScannerState::IDLE;
    statistics_.reset();

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::start_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ == ScannerState::SCANNING) {
        return ErrorCode::SUCCESS;  // Already scanning
    }
    
    // Start spectrum streaming
    ErrorCode stream_result = hardware_.start_spectrum_streaming();
    if (stream_result != ErrorCode::SUCCESS) {
        state_ = ScannerState::ERROR;
        return stream_result;
    }
    
    state_ = ScannerState::SCANNING;
    scanning_active_.set();
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::stop_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ == ScannerState::IDLE) {
        return ErrorCode::SUCCESS;  // Already stopped
    }
    
    // Stop spectrum streaming
    ErrorCode stream_result = hardware_.stop_spectrum_streaming();
    if (stream_result != ErrorCode::SUCCESS) {
        state_ = ScannerState::ERROR;
        return stream_result;
    }
    
    state_ = ScannerState::IDLE;
    scanning_active_.clear();
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::pause_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ != ScannerState::SCANNING) {
        return ErrorCode::SUCCESS;  // Not scanning
    }
    
    state_ = ScannerState::PAUSED;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::resume_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ != ScannerState::PAUSED) {
        return ErrorCode::SUCCESS;  // Not paused
    }
    
    state_ = ScannerState::SCANNING;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::perform_scan_cycle() noexcept {
    if (!scanning_active_.test()) {
        return ErrorCode::SUCCESS;  // Not scanning, skip
    }
    
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ != ScannerState::SCANNING) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    return perform_scan_cycle_internal();
}

ErrorCode DroneScanner::perform_scan_cycle_internal() noexcept {
    // Prevent concurrent scan cycles
    if (scan_cycle_in_progress_.test()) {
        return ErrorCode::UNKNOWN_ERROR;
    }

    // Set scan cycle in progress flag
    scan_cycle_in_progress_.set();

    // Update statistics
    statistics_.total_scan_cycles++;

    // Get next frequency from database
    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(current_frequency_);
    if (!freq_result.has_value()) {
        statistics_.failed_cycles++;
        scan_cycle_in_progress_.clear();
        return freq_result.error();
    }

    current_frequency_ = freq_result.value();

    // Tune to frequency
    ErrorCode tune_result = hardware_.tune_to_frequency(current_frequency_);
    if (tune_result != ErrorCode::SUCCESS) {
        statistics_.failed_cycles++;
        scan_cycle_in_progress_.clear();
        return tune_result;
    }

    // Get RSSI sample
    ErrorResult<RssiSample> rssi_result = hardware_.get_rssi_sample();
    if (!rssi_result.has_value()) {
        statistics_.failed_cycles++;
        scan_cycle_in_progress_.clear();
        return rssi_result.error();
    }

    const RssiSample& sample = rssi_result.value();

    // Check if signal detected
    if (sample.rssi >= config_.rssi_threshold_dbm) {
        // Update tracked drones
        ErrorCode update_result = update_tracked_drone_internal(
            sample.frequency,
            sample.rssi,
            sample.timestamp
        );

        if (update_result == ErrorCode::SUCCESS) {
            statistics_.successful_cycles++;

            // Update max RSSI
            if (sample.rssi > static_cast<int32_t>(statistics_.max_rssi_dbm)) {
                statistics_.max_rssi_dbm = static_cast<uint32_t>(sample.rssi);
            }
        } else {
            statistics_.failed_cycles++;
        }
    } else {
        statistics_.successful_cycles++;
    }

    // Update last scan time
    // last_scan_time_ = chTimeNow();

    // Clear scan cycle in progress flag
    scan_cycle_in_progress_.clear();

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::update_tracked_drones(
    FreqHz frequency,
    RssiValue rssi,
    SystemTime timestamp
) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    return update_tracked_drone_internal(frequency, rssi, timestamp);
}

ErrorCode DroneScanner::update_tracked_drone_internal(
    FreqHz frequency,
    RssiValue rssi,
    SystemTime timestamp
) noexcept {
    // Check if drone already tracked
    ErrorResult<size_t> index_result = find_drone_by_frequency_internal(frequency);
    
    if (index_result.has_value()) {
        // Update existing drone
        size_t index = index_result.value();
        
        // Get current threat level before update
        ThreatLevel old_threat = tracked_drones_[index].get_threat();
        
        // Update drone RSSI
        tracked_drones_[index].update_rssi(rssi, timestamp);
        
        // Get new threat level after update
        ThreatLevel new_threat = tracked_drones_[index].get_threat();
        
        // Store new threat level
        last_threat_levels_[index] = new_threat;
        
        // Trigger alert for threat level changes
        if (new_threat > old_threat) {
            // Threat level increased
            if (new_threat == ThreatLevel::CRITICAL) {
                trigger_alert(AlertType::THREAT_CRITICAL);
            } else {
                trigger_alert(AlertType::THREAT_INCREASED);
            }
            // Drone is approaching
            trigger_alert(AlertType::DRONE_APPROACHING);
        } else if (new_threat < old_threat) {
            // Drone is receding
            trigger_alert(AlertType::DRONE_RECEDING);
        }
    } else {
        // Add new drone
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

ErrorCode DroneScanner::add_tracked_drone_internal(
    FreqHz frequency,
    RssiValue rssi,
    SystemTime timestamp
) noexcept {
    // Check if tracked drones array is full
    if (tracked_count_ >= MAX_TRACKED_DRONES) {
        return ErrorCode::BUFFER_FULL;
    }
    
    // Determine drone type and threat level
    DroneType type = determine_drone_type_internal(frequency);
    ThreatLevel threat = determine_threat_level_internal(rssi);
    
    // Create new tracked drone
    tracked_drones_[tracked_count_] = TrackedDrone(frequency, type, threat);
    tracked_drones_[tracked_count_].update_rssi(rssi, timestamp);
    
    // Store initial threat level
    last_threat_levels_[tracked_count_] = threat;
    
    tracked_count_++;
    
    // Update statistics
    statistics_.drones_detected++;
    
    // Trigger alert for new drone detection
    trigger_alert(AlertType::NEW_DRONE);
    
    // Trigger alert if threat level is critical
    if (threat == ThreatLevel::CRITICAL) {
        trigger_alert(AlertType::THREAT_CRITICAL);
    }
    
    return ErrorCode::SUCCESS;
}

DroneType DroneScanner::determine_drone_type_internal(FreqHz frequency) const noexcept {
    // Look up drone type from freqman database
    ErrorResult<FrequencyEntry> entry_result = database_.find_entry(frequency);

    if (entry_result.has_value()) {
        const FrequencyEntry& entry = entry_result.value();
        return entry.drone_type;
    }

    // Entry not found in database
    return DroneType::UNKNOWN;
}

ThreatLevel DroneScanner::determine_threat_level_internal(RssiValue rssi) const noexcept {
    // Classify threat based on RSSI
    if (rssi >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM) {
        return ThreatLevel::CRITICAL;
    } else if (rssi >= RSSI_HIGH_THREAT_THRESHOLD_DBM) {
        return ThreatLevel::HIGH;
    } else if (rssi >= RSSI_DETECTION_THRESHOLD_DBM) {
        return ThreatLevel::MEDIUM;
    } else {
        return ThreatLevel::LOW;
    }
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

ErrorCode DroneScanner::get_display_data(DisplayData& display_data) const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    return update_display_data_internal(display_data);
}

ErrorCode DroneScanner::update_display_data_internal(
    DisplayData& display_data
) const noexcept {
    // Clear display data
    display_data.clear();
    
    // Copy tracked drones to display entries
    size_t copy_count = tracked_count_ < MAX_DISPLAYED_DRONES ? tracked_count_ : MAX_DISPLAYED_DRONES;
    
    for (size_t i = 0; i < copy_count; ++i) {
        display_data.drones[i] = DisplayDroneEntry(tracked_drones_[i]);
    }
    
    display_data.drone_count = copy_count;
    
    // Update state snapshot
    display_data.state.scanning_active = scanning_active_.test();
    display_data.state.is_fresh = true;
    
    // Calculate threat statistics
    display_data.state.max_detected_threat = ThreatLevel::NONE;
    display_data.state.approaching_count = 0;
    display_data.state.static_count = 0;
    display_data.state.receding_count = 0;
    
    for (size_t i = 0; i < copy_count; ++i) {
        // Update max threat
        if (display_data.drones[i].threat > display_data.state.max_detected_threat) {
            display_data.state.max_detected_threat = display_data.drones[i].threat;
        }
        
        // Count movement trends
        switch (display_data.drones[i].trend) {
            case MovementTrend::APPROACHING:
                display_data.state.approaching_count++;
                break;
            case MovementTrend::STATIC:
                display_data.state.static_count++;
                break;
            case MovementTrend::RECEDING:
                display_data.state.receding_count++;
                break;
            default:
                break;
        }
    }
    
    return ErrorCode::SUCCESS;
}

ScannerState DroneScanner::get_state() const noexcept {
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
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
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
            // Keep this drone
            if (write_index != read_index) {
                tracked_drones_[write_index] = tracked_drones_[read_index];
                // Update last threat level for moved drone
                last_threat_levels_[write_index] = last_threat_levels_[read_index];
            }
            write_index++;
        }
        // Stale drones are skipped (removed)
    }
    
    tracked_count_ = write_index;
}

// ============================================================================
// Alert Callback Implementation
// ============================================================================

void DroneScanner::set_alert_callback(AlertCallback callback) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    alert_callback_ = callback;
}

void DroneScanner::trigger_alert(AlertType alert_type) noexcept {
    // Make local copy of callback pointer to prevent race condition
    // This ensures we use the callback that was set at alert trigger time
    AlertCallback local_callback = alert_callback_;
    
    // Call alert callback if set (invoked OUTSIDE any lock to prevent deadlocks)
    if (local_callback != nullptr) {
        AlertPriority priority = AlertPriority::LOW;
        
        // Determine priority based on alert type
        switch (alert_type) {
            case AlertType::NEW_DRONE:
                priority = AlertPriority::MEDIUM;
                break;
            case AlertType::THREAT_INCREASED:
                priority = AlertPriority::HIGH;
                break;
            case AlertType::THREAT_CRITICAL:
                priority = AlertPriority::CRITICAL;
                break;
            case AlertType::DRONE_APPROACHING:
                priority = AlertPriority::HIGH;
                break;
            case AlertType::DRONE_RECEDING:
                priority = AlertPriority::LOW;
                break;
        }
        
        // Call callback with alert type and priority
        local_callback(alert_type, static_cast<uint8_t>(priority));
    }
}

} // namespace drone_analyzer
