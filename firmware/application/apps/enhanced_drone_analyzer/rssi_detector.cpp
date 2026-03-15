#include "rssi_detector.hpp"

namespace drone_analyzer {

// ============================================================================
// Constructor / Destructor
// ============================================================================

RSSIDetector::RSSIDetector() noexcept
    : detection_threshold_(RSSI_DETECTION_THRESHOLD_DBM)
    , rssi_history_{}
    , timestamp_history_{}
    , history_index_(0)
    , statistics_{}
    , initialized_(false) {
}

// ============================================================================
// Initialization
// ============================================================================

ErrorCode RSSIDetector::initialize(RssiValue detection_threshold) noexcept {
    // Validate detection threshold
    if (detection_threshold < RSSI_MIN_DBM || detection_threshold > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }

    detection_threshold_ = detection_threshold;

    // Clear history buffers
    for (size_t i = 0; i < RSSI_HISTORY_SIZE; ++i) {
        rssi_history_[i] = RSSI_MIN_DBM;
    }
    for (size_t i = 0; i < TIMESTAMP_HISTORY_SIZE; ++i) {
        timestamp_history_[i] = 0;
    }
    history_index_ = 0;

    // Reset statistics
    statistics_.total_samples = 0;
    statistics_.detections = 0;
    statistics_.high_threat_count = 0;
    statistics_.critical_threat_count = 0;
    statistics_.min_rssi = RSSI_MAX_DBM;
    statistics_.max_rssi = RSSI_MIN_DBM;
    statistics_.approaching_count = 0;
    statistics_.receding_count = 0;
    statistics_.static_count = 0;

    initialized_ = true;
    return ErrorCode::SUCCESS;
}

// ============================================================================
// Public Methods
// ============================================================================

ErrorCode RSSIDetector::process_rssi_sample(
    RssiValue rssi,
    SystemTime timestamp
) noexcept {
    // Validate RSSI value
    ErrorCode error = validate_rssi(rssi);
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    // Add to history
    add_to_history(rssi, timestamp);

    // Update statistics
    statistics_.total_samples++;
    if (rssi < statistics_.min_rssi) {
        statistics_.min_rssi = rssi;
    }
    if (rssi > statistics_.max_rssi) {
        statistics_.max_rssi = rssi;
    }

    return ErrorCode::SUCCESS;
}

ErrorCode RSSIDetector::detect_drone(RSSIDetectionResult& result) noexcept {
    // Initialize result
    result.drone_detected = false;
    result.threat_level = ThreatLevel::NONE;
    result.rssi = RSSI_MIN_DBM;
    result.average_rssi = RSSI_MIN_DBM;
    result.movement_trend = MovementTrend::UNKNOWN;
    result.sample_count = static_cast<uint8_t>(history_index_);

    // Check if we have enough samples
    if (history_index_ < 2) {
        return ErrorCode::SUCCESS;
    }

    // Get average RSSI
    result.average_rssi = get_average_rssi();

    // Get latest RSSI sample
    const size_t latest_index = (history_index_ > 0) ? (history_index_ - 1) : 0;
    result.rssi = rssi_history_[latest_index];

    // Detect drone based on threshold
    if (result.rssi >= detection_threshold_) {
        result.drone_detected = true;

        // Calculate threat level
        result.threat_level = calculate_threat_level(result.rssi, result.average_rssi);

        // Get movement trend
        result.movement_trend = get_movement_trend();

        // Update statistics
        update_statistics(result.rssi, true, result.threat_level);
    }

    return ErrorCode::SUCCESS;
}

ThreatLevel RSSIDetector::calculate_threat_level(
    RssiValue rssi,
    RssiValue average_rssi
) const noexcept {
    // Use RSSI thresholds from constants
    if (rssi >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM) {
        return ThreatLevel::CRITICAL;
    } else if (rssi >= RSSI_HIGH_THREAT_THRESHOLD_DBM) {
        return ThreatLevel::HIGH;
    } else if (rssi >= RSSI_DETECTION_THRESHOLD_DBM) {
        return ThreatLevel::MEDIUM;
    } else if (rssi >= (RSSI_DETECTION_THRESHOLD_DBM - 10)) {
        return ThreatLevel::LOW;
    } else {
        return ThreatLevel::NONE;
    }
}

void RSSIDetector::update_statistics(
    RssiValue rssi,
    bool detected,
    ThreatLevel threat_level
) noexcept {
    if (detected) {
        statistics_.detections++;

        // Update threat level counts
        if (threat_level == ThreatLevel::HIGH) {
            statistics_.high_threat_count++;
        } else if (threat_level == ThreatLevel::CRITICAL) {
            statistics_.critical_threat_count++;
        }

        // Update movement trend counts
        const MovementTrend trend = get_movement_trend();
        if (trend == MovementTrend::APPROACHING) {
            statistics_.approaching_count++;
        } else if (trend == MovementTrend::RECEDING) {
            statistics_.receding_count++;
        } else if (trend == MovementTrend::STATIC) {
            statistics_.static_count++;
        }
    }
}

void RSSIDetector::reset() noexcept {
    // Clear history buffers
    for (size_t i = 0; i < RSSI_HISTORY_SIZE; ++i) {
        rssi_history_[i] = RSSI_MIN_DBM;
    }
    for (size_t i = 0; i < TIMESTAMP_HISTORY_SIZE; ++i) {
        timestamp_history_[i] = 0;
    }
    history_index_ = 0;

    // Reset statistics
    statistics_.total_samples = 0;
    statistics_.detections = 0;
    statistics_.high_threat_count = 0;
    statistics_.critical_threat_count = 0;
    statistics_.min_rssi = RSSI_MAX_DBM;
    statistics_.max_rssi = RSSI_MIN_DBM;
    statistics_.approaching_count = 0;
    statistics_.receding_count = 0;
    statistics_.static_count = 0;
}

void RSSIDetector::get_statistics(RSSIStatistics& stats) const noexcept {
    stats = statistics_;
}

MovementTrend RSSIDetector::get_movement_trend() const noexcept {
    if (history_index_ < 2) {
        return MovementTrend::UNKNOWN;
    }

    return calculate_movement_trend();
}

RssiValue RSSIDetector::get_average_rssi() const noexcept {
    if (history_index_ == 0) {
        return RSSI_MIN_DBM;
    }

    // Calculate sum using uint32_t to avoid overflow
    uint32_t sum = 0;
    for (size_t i = 0; i < history_index_; ++i) {
        // Add offset to make values positive for calculation
        sum += static_cast<uint32_t>(rssi_history_[i] - RSSI_MIN_DBM);
    }

    // Calculate average using bit shift (divide by power of 2 approximation)
    uint32_t avg = 0;
    if (history_index_ >= 256) {
        avg = sum >> 8;  // Divide by 256
    } else if (history_index_ >= 128) {
        avg = sum >> 7;  // Divide by 128
    } else if (history_index_ >= 64) {
        avg = sum >> 6;  // Divide by 64
    } else if (history_index_ >= 32) {
        avg = sum >> 5;  // Divide by 32
    } else if (history_index_ >= 16) {
        avg = sum >> 4;  // Divide by 16
    } else if (history_index_ >= 8) {
        avg = sum >> 3;  // Divide by 8
    } else if (history_index_ >= 4) {
        avg = sum >> 2;  // Divide by 4
    } else if (history_index_ >= 2) {
        avg = sum >> 1;  // Divide by 2
    } else {
        avg = sum;
    }

    // Remove offset
    return static_cast<RssiValue>(avg + RSSI_MIN_DBM);
}

void RSSIDetector::set_detection_threshold(RssiValue threshold) noexcept {
    if (threshold >= RSSI_MIN_DBM && threshold <= RSSI_MAX_DBM) {
        detection_threshold_ = threshold;
    }
}

RssiValue RSSIDetector::get_detection_threshold() const noexcept {
    return detection_threshold_;
}

// ============================================================================
// Private Methods
// ============================================================================

ErrorCode RSSIDetector::validate_rssi(RssiValue rssi) const noexcept {
    if (rssi < RSSI_MIN_DBM || rssi > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }
    return ErrorCode::SUCCESS;
}

void RSSIDetector::add_to_history(RssiValue rssi, SystemTime timestamp) noexcept {
    // Use circular buffer
    if (history_index_ < RSSI_HISTORY_SIZE) {
        rssi_history_[history_index_] = rssi;
        timestamp_history_[history_index_] = timestamp;
        history_index_++;
    } else {
        // Buffer full, shift all elements (simple approach)
        for (size_t i = 0; i < RSSI_HISTORY_SIZE - 1; ++i) {
            rssi_history_[i] = rssi_history_[i + 1];
            timestamp_history_[i] = timestamp_history_[i + 1];
        }
        rssi_history_[RSSI_HISTORY_SIZE - 1] = rssi;
        timestamp_history_[TIMESTAMP_HISTORY_SIZE - 1] = timestamp;
    }
}

MovementTrend RSSIDetector::calculate_movement_trend() const noexcept {
    if (history_index_ < 2) {
        return MovementTrend::UNKNOWN;
    }

    // Check variance first
    const uint32_t variance = calculate_variance();
    constexpr uint32_t VARIANCE_THRESHOLD = 25;  // 5^2

    if (variance < VARIANCE_THRESHOLD) {
        return MovementTrend::STATIC;
    }

    // Check if approaching or receding
    if (is_approaching()) {
        return MovementTrend::APPROACHING;
    } else if (is_receding()) {
        return MovementTrend::RECEDING;
    } else {
        return MovementTrend::STATIC;
    }
}

bool RSSIDetector::is_approaching() const noexcept {
    if (history_index_ < 2) {
        return false;
    }

    // Compare latest sample with oldest sample
    const RssiValue latest = rssi_history_[history_index_ - 1];
    const RssiValue oldest = rssi_history_[0];

    // RSSI increasing means approaching
    // Use threshold of 3 dB to avoid noise
    return (latest - oldest) >= 3;
}

bool RSSIDetector::is_receding() const noexcept {
    if (history_index_ < 2) {
        return false;
    }

    // Compare latest sample with oldest sample
    const RssiValue latest = rssi_history_[history_index_ - 1];
    const RssiValue oldest = rssi_history_[0];

    // RSSI decreasing means receding
    // Use threshold of 3 dB to avoid noise
    return (oldest - latest) >= 3;
}

bool RSSIDetector::is_static() const noexcept {
    const uint32_t variance = calculate_variance();
    constexpr uint32_t VARIANCE_THRESHOLD = 25;  // 5^2
    return variance < VARIANCE_THRESHOLD;
}

uint32_t RSSIDetector::calculate_variance() const noexcept {
    if (history_index_ < 2) {
        return 0;
    }

    // Calculate mean
    const RssiValue mean = get_average_rssi();

    // Calculate variance using integer arithmetic
    uint32_t sum_squared_diff = 0;
    for (size_t i = 0; i < history_index_; ++i) {
        const int32_t diff = static_cast<int32_t>(rssi_history_[i]) - static_cast<int32_t>(mean);
        sum_squared_diff += static_cast<uint32_t>(diff * diff);
    }

    // Divide by count (use bit shift approximation)
    uint32_t variance = 0;
    if (history_index_ >= 256) {
        variance = sum_squared_diff >> 8;
    } else if (history_index_ >= 128) {
        variance = sum_squared_diff >> 7;
    } else if (history_index_ >= 64) {
        variance = sum_squared_diff >> 6;
    } else if (history_index_ >= 32) {
        variance = sum_squared_diff >> 5;
    } else if (history_index_ >= 16) {
        variance = sum_squared_diff >> 4;
    } else if (history_index_ >= 8) {
        variance = sum_squared_diff >> 3;
    } else if (history_index_ >= 4) {
        variance = sum_squared_diff >> 2;
    } else if (history_index_ >= 2) {
        variance = sum_squared_diff >> 1;
    } else {
        variance = sum_squared_diff;
    }

    return variance;
}

} // namespace drone_analyzer
