#include "constants.hpp"
#include "drone_types.hpp"
#include <cstdint>
#include <cstddef>

namespace drone_analyzer {

// ============================================================================
// freqman_entry_fixed Implementation
// ============================================================================

freqman_entry_fixed::freqman_entry_fixed() noexcept
    : frequency_a{0}
    , frequency_b{0}
    , description{'\0'}
    , type{static_cast<freqman_type>(0)}
    , modulation{255}
    , bandwidth{255}
    , step{255}
    , tone{255} {
}

freqman_entry_fixed::freqman_entry_fixed(
    int64_t freq_a,
    int64_t freq_b,
    const char* desc,
    freqman_type t
) noexcept
    : frequency_a{freq_a}
    , frequency_b{freq_b}
    , description{'\0'}
    , type{t}
    , modulation{255}
    , bandwidth{255}
    , step{255}
    , tone{255} {
    
    if (desc != nullptr) {
        [[maybe_unused]] ErrorCode err = set_description(desc);
        (void)err;
    }
}

ErrorCode freqman_entry_fixed::set_description(const char* src) noexcept {
    if (src == nullptr) {
        description[0] = '\0';
        return ErrorCode::SUCCESS;
    }
    
    size_t i = 0;
    while (i < 31 && src[i] != '\0') {
        description[i] = src[i];
        ++i;
    }
    description[i] = '\0';
    
    return ErrorCode::SUCCESS;
}

bool freqman_entry_fixed::is_valid() const noexcept {
    return (frequency_a >= static_cast<int64_t>(MIN_FREQUENCY_HZ)) &&
           (frequency_a <= static_cast<int64_t>(MAX_FREQUENCY_HZ)) &&
           (frequency_b >= 0);
}

// ============================================================================
// TrackedDrone Implementation
// ============================================================================

TrackedDrone::TrackedDrone() noexcept
    : frequency{0}
    , measured_frequency_{0}
    , drone_type{DroneType::UNKNOWN}
    , threat_level{ThreatLevel::NONE}
    , update_count{0}
    , last_seen{0}
    , rssi{RSSI_NOISE_FLOOR_DBM}
    , rssi_history_{}
    , timestamp_history_{}
    , history_index_{0}
    , missed_cycles_{0}
    , last_rssi_{RSSI_NOISE_FLOOR_DBM}
    , rssi_decrease_counter_{0}
    , rssi_increased_{false}
    , last_increase_time_{0}
    , created_time_{0}
    , last_seen_time_{0}
    , sweep_cycles_missed_{0}
    , sweep_mode_active_{false}
    , last_cycle_peak_rssi_{SWEEP_CYCLE_PEAK_INVALID_DBM}
    , prev_cycle_peak_rssi_{SWEEP_CYCLE_PEAK_INVALID_DBM}
    , has_prev_cycle_peak_{false}
    , cached_trend_{MovementTrend::UNKNOWN}
    , trend_hold_count_{0}
    , mahalanobis_stats_{} {
}

TrackedDrone::TrackedDrone(
    FreqHz freq,
    DroneType type,
    ThreatLevel threat
) noexcept
    : frequency{freq}
    , measured_frequency_{0}
    , drone_type{type}
    , threat_level{threat}
    , update_count{0}
    , last_seen{0}
    , rssi{RSSI_NOISE_FLOOR_DBM}
    , rssi_history_{}
    , timestamp_history_{}
    , history_index_{0}
    , missed_cycles_{0}
    , last_rssi_{RSSI_NOISE_FLOOR_DBM}
    , rssi_decrease_counter_{0}
    , rssi_increased_{false}
    , last_increase_time_{0}
    , created_time_{0}
    , last_seen_time_{0}
    , sweep_cycles_missed_{0}
    , sweep_mode_active_{false}
    , last_cycle_peak_rssi_{SWEEP_CYCLE_PEAK_INVALID_DBM}
    , prev_cycle_peak_rssi_{SWEEP_CYCLE_PEAK_INVALID_DBM}
    , has_prev_cycle_peak_{false}
    , cached_trend_{MovementTrend::UNKNOWN}
    , trend_hold_count_{0}
    , mahalanobis_stats_{} {
}

void TrackedDrone::update_rssi(RssiValue new_rssi, SystemTime timestamp, const ThreatThresholds& thresholds) noexcept {
    // Use circular buffer from the start
    const size_t write_idx = history_index_ % RSSI_HISTORY_SIZE;
    rssi_history_[write_idx] = static_cast<int16_t>(new_rssi);
    
    // Timestamp buffer is smaller (3 entries), use modulo for wrap
    const size_t timestamp_idx = history_index_ % TIMESTAMP_HISTORY_SIZE;
    timestamp_history_[timestamp_idx] = timestamp;
    
    // Increment index (uint8_t wrap-safe)
    history_index_++;
    
    rssi = new_rssi;
    last_seen = timestamp;
    
    // Clamp update_count to RSSI_HISTORY_SIZE
    if (update_count < RSSI_HISTORY_SIZE) {
        update_count++;
    }
    
    // Classify threat from RSSI value.
    // In NORMAL mode: use peak of rssi_history_ for stability (survives noise spikes).
    // In SWEEP mode: use direct new_rssi because rssi_history_ is contaminated
    // (each entry comes from a different frequency pass, not the drone's frequency).
    RssiValue classify_rssi = new_rssi;
    if (!sweep_mode_active_ && update_count >= 2) {
        const uint8_t count = (update_count > RSSI_HISTORY_SIZE)
            ? RSSI_HISTORY_SIZE : update_count;
        RssiValue peak_rssi = rssi_history_[0];
        for (uint8_t i = 1; i < count; ++i) {
            if (rssi_history_[i] > peak_rssi) peak_rssi = rssi_history_[i];
        }
        classify_rssi = peak_rssi;
    }

    // Weak but active signals (above the detection gate yet below the medium
    // threshold) classify as LOW. Escalates with signal strength. Signals that
    // pass the detection gate but fall below the configured LOW threshold stay
    // at NONE — the LOW threshold is the user-configurable floor of the scale.
    ThreatLevel classified = ThreatLevel::NONE;
    if (classify_rssi >= thresholds.critical) {
        classified = ThreatLevel::CRITICAL;
    } else if (classify_rssi >= thresholds.high) {
        classified = ThreatLevel::HIGH;
    } else if (classify_rssi >= thresholds.medium) {
        classified = ThreatLevel::MEDIUM;
    } else if (classify_rssi >= thresholds.low) {
        classified = ThreatLevel::LOW;
    }

    // Allow both upgrade and downgrade of threat level during re-detection.
    threat_level = classified;

    // Track RSSI trend: compare against previous sample (last_rssi_).
    // This flag is read by apply_rssi_decay() at cycle boundaries to decide decay.
    if (new_rssi > last_rssi_) {
        rssi_increased_ = true;
        last_increase_time_ = timestamp;
    }
    // Update last_rssi_ for next sample comparison (intra-cycle tracking)
    last_rssi_ = static_cast<int16_t>(new_rssi);
}

RssiValue TrackedDrone::get_average_rssi() const noexcept {
    if (update_count == 0) {
        return rssi;
    }
    
    int32_t sum = 0;
    const uint8_t count = (update_count > RSSI_HISTORY_SIZE) ? RSSI_HISTORY_SIZE : update_count;
    
    for (size_t i = 0; i < count; ++i) {
        sum += rssi_history_[i];
    }
    
    if (count == 0) {
        return rssi;
    }
    
    return sum / count;
}

bool TrackedDrone::is_stale(SystemTime current_time, SystemTime timeout_ms) const noexcept {
    if (update_count == 0) {
        return true;
    }
    
    const uint32_t elapsed = current_time - last_seen;
    return elapsed >= timeout_ms;
}

bool TrackedDrone::decay_threat() noexcept {
    switch (threat_level) {
        case ThreatLevel::CRITICAL:
            threat_level = ThreatLevel::HIGH;
            return false;
        case ThreatLevel::HIGH:
            threat_level = ThreatLevel::MEDIUM;
            return false;
        case ThreatLevel::MEDIUM:
            threat_level = ThreatLevel::LOW;
            return false;
        case ThreatLevel::LOW:
            threat_level = ThreatLevel::NONE;
            return true;  // Signal: should be removed
        case ThreatLevel::NONE:
        default:
            return true;  // Already NONE, remove
    }
}

// ============================================================================
// Other Struct Implementations
// ============================================================================

ScannerStateSnapshot::ScannerStateSnapshot() noexcept
    : max_detected_threat{ThreatLevel::NONE}
    , approaching_count{0}
    , static_count{0}
    , receding_count{0}
    , scanning_active{false}
    , is_fresh{false} {
}

DisplayData::DisplayData() noexcept
    : drones{}
    , drone_count{0}
    , state{} {
}

void DisplayData::clear() noexcept {
    drone_count = 0;
    state.max_detected_threat = ThreatLevel::NONE;
    state.approaching_count = 0;
    state.static_count = 0;
    state.receding_count = 0;
    state.scanning_active = false;
    state.is_fresh = false;
}

void DisplayDroneEntry::set_color_from_threat() noexcept {
    switch (threat) {
        case ThreatLevel::CRITICAL:
            display_color = COLOR_CRITICAL_THREAT;
            break;
        case ThreatLevel::HIGH:
            display_color = COLOR_HIGH_THREAT;
            break;
        case ThreatLevel::MEDIUM:
            display_color = COLOR_MEDIUM_THREAT;
            break;
        case ThreatLevel::LOW:
            display_color = COLOR_LOW_THREAT;
            break;
        case ThreatLevel::NONE:
        default:
            display_color = COLOR_UNKNOWN_THREAT;
            break;
    }
}

DisplayDroneEntry::DisplayDroneEntry() noexcept
    : frequency{0}
    , type{DroneType::UNKNOWN}
    , threat{ThreatLevel::NONE}
    , rssi{RSSI_NOISE_FLOOR_DBM}
    , last_seen{0}
    , type_name{'\0'}
    , display_color{0xFFFFFFFF}
    , trend{MovementTrend::UNKNOWN} {
}

DisplayDroneEntry::DisplayDroneEntry(const TrackedDrone& drone) noexcept
    : frequency(drone.get_display_frequency())
    , type(drone.drone_type)
    , threat(drone.get_threat())
    , rssi(drone.rssi)
    , last_seen(drone.last_seen)
    , type_name{0}
    , display_color(0xFFFFFFFF)
    , trend(drone.get_movement_trend()) {
    
    const char* type_str = drone_type_to_string(drone.drone_type);
    size_t i = 0;
    while (i < DRONE_TYPE_NAME_LENGTH - 1 && type_str[i] != '\0') {
        type_name[i] = type_str[i];
        ++i;
    }
    
    set_color_from_threat();
}

const char* drone_type_to_string(DroneType type) noexcept {
    switch (type) {
        case DroneType::DJI:
            return DRONE_TYPE_DJI;
        case DroneType::PARROT:
            return DRONE_TYPE_PARROT;
        case DroneType::YUNEEC:
            return DRONE_TYPE_YUNEEC;
        case DroneType::DR_3DR:
            return DRONE_TYPE_3DR;
        case DroneType::AUTEL:
            return DRONE_TYPE_AUTEL;
        case DroneType::HOBBY:
            return DRONE_TYPE_HOBBY;
        case DroneType::FPV:
            return DRONE_TYPE_FPV;
        case DroneType::CUSTOM:
            return DRONE_TYPE_CUSTOM;
        case DroneType::OTHER:
            return DRONE_TYPE_OTHER;
        case DroneType::UNKNOWN:
        default:
            return DRONE_TYPE_UNKNOWN;
    }
}

const char* DisplayDroneEntry::get_type_name() const noexcept {
    return type_name;
}

MovementTrend TrackedDrone::get_movement_trend() const noexcept {
    MovementTrend raw_trend = MovementTrend::UNKNOWN;

    // ---- SWEEP MODE: compare cycle peaks (rssi_history_ is contaminated) ----
    // Gate on the sentinel: last_cycle_peak_rssi_ is reset to the sentinel at
    // every pair boundary, and prev_cycle_peak_rssi_ is only set after a cycle
    // with a real peak. Comparing sentinel stubs would yield false RECEDING.
    // The normal-mode branch below must NOT run while sweep_mode_active_ —
    // rssi_history_ holds mixed-frequency data that would corrupt the trend.
    if (sweep_mode_active_) {
        if (last_cycle_peak_rssi_ != SWEEP_CYCLE_PEAK_INVALID_DBM) {
            if (has_prev_cycle_peak_) {
                // Normal case: compare this cycle's peak against previous cycle's peak
                constexpr int32_t THRESHOLD = MOVEMENT_TREND_THRESHOLD_APPROACHING_DB;
                const int32_t diff = static_cast<int32_t>(last_cycle_peak_rssi_)
                                   - static_cast<int32_t>(prev_cycle_peak_rssi_);
                if (diff > THRESHOLD) {
                    raw_trend = MovementTrend::APPROACHING;
                } else if (diff < -THRESHOLD) {
                    raw_trend = MovementTrend::RECEDING;
                } else {
                    raw_trend = MovementTrend::STATIC;
                }
            } else {
                // First sweep pass: drone just detected, no previous cycle peak yet.
                // Show STATIC instead of UNKNOWN so the UI reflects that the drone
                // is present and stationary until proven otherwise by the next pass.
                raw_trend = MovementTrend::STATIC;
            }
        }
    }
    // ---- NORMAL MODE: use split-buffer averaging ----
    else if (update_count >= MOVEMENT_TREND_MIN_HISTORY) {
        constexpr uint8_t HALF_HISTORY = RSSI_HISTORY_SIZE / 2;
        constexpr int32_t APPROACHING_THRESHOLD = MOVEMENT_TREND_THRESHOLD_APPROACHING_DB;
        constexpr int32_t RECEDED_THRESHOLD = MOVEMENT_TREND_THRESHOLD_RECEEDING_DB;
        constexpr int32_t SILENCE_THRESHOLD = MOVEMENT_TREND_SILENCE_THRESHOLD_DBM;

        int32_t older_sum = 0;
        int32_t recent_sum = 0;
        uint8_t older_count = 0;
        uint8_t recent_count = 0;

        // Iterate only initialized slots. When buffer is not yet full
        // (history_index_ < RSSI_HISTORY_SIZE), start from position 0
        // instead of history_index_ — the latter would visit uninitialized
        // slots (value 0) which pass SILENCE_THRESHOLD and corrupt the
        // older_sum, causing false RECEDING during warmup.
        const uint8_t n = (update_count > RSSI_HISTORY_SIZE)
            ? RSSI_HISTORY_SIZE : update_count;
        const uint8_t start = (update_count >= RSSI_HISTORY_SIZE)
            ? (history_index_ % RSSI_HISTORY_SIZE) : 0;
        for (uint8_t i = 0; i < n; ++i) {
            const uint8_t logical_idx = (start + i) % RSSI_HISTORY_SIZE;
            const int16_t val = rssi_history_[logical_idx];

            if (val <= SILENCE_THRESHOLD) {
                continue;
            }

            if (i < HALF_HISTORY) {
                older_sum += val;
                older_count++;
            } else {
                recent_sum += val;
                recent_count++;
            }
        }

        if (older_count > 0 && recent_count > 0) {
            const int32_t avg_old = older_sum / older_count;
            const int32_t avg_new = recent_sum / recent_count;
            const int32_t diff = avg_new - avg_old;

            if (diff > APPROACHING_THRESHOLD) {
                raw_trend = MovementTrend::APPROACHING;
            } else if (diff < RECEDED_THRESHOLD) {
                raw_trend = MovementTrend::RECEDING;
            } else {
                raw_trend = MovementTrend::STATIC;
            }
        }
    }

    // ---- HYSTERESIS: require TREND_HYSTERESIS_COUNT agreeing evaluations ----
    // Prevents icon flicker from single-sample noise. When no new data is
    // available (sweep gap between passes), hold the last known trend instead
    // of flipping to '-' every few hundred milliseconds.
    if (raw_trend == cached_trend_) {
        trend_hold_count_ = 0;  // same direction — reset hold counter
    } else if (raw_trend == MovementTrend::UNKNOWN) {
        if (cached_trend_ != MovementTrend::UNKNOWN) {
            // No new data this evaluation — keep showing the last known trend.
            return cached_trend_;
        }
    } else if (trend_hold_count_ < TREND_HYSTERESIS_COUNT) {
        trend_hold_count_++;
        if (trend_hold_count_ < TREND_HYSTERESIS_COUNT) {
            // Still holding — keep returning old trend
            return cached_trend_;
        }
        trend_hold_count_ = 0;  // threshold reached — accept new trend below
    } else {
        trend_hold_count_ = 0;
    }

    cached_trend_ = raw_trend;
    return raw_trend;
}

} // namespace drone_analyzer
