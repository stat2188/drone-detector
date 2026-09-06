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

void TrackedDrone::absorb_from(const TrackedDrone& other) noexcept {
    // Array-size derivation instead of RSSI_HISTORY_SIZE / TIMESTAMP_HISTORY_SIZE:
    // this .cpp's header must not include constants.hpp (circular include),
    // same policy as observe_rssi(). constants.hpp static_asserts both == 6.
    constexpr size_t RSSI_LEN = sizeof(rssi_history_) / sizeof(rssi_history_[0]);
    constexpr size_t TS_LEN = sizeof(timestamp_history_) / sizeof(timestamp_history_[0]);
    static_assert(RSSI_LEN == TS_LEN, "history arrays must stay aligned");

    // Stack: 12 * 8 B = 96 bytes + scalars ≈ 128 bytes. Well under the 512 B
    // per-frame limit. No heap, no recursion.
    struct HistorySample {
        SystemTime timestamp;
        int16_t rssi;
    };
    HistorySample merged[RSSI_LEN * 2];
    size_t count = 0;

    // Extract this (survivor) entry's valid samples, oldest -> newest.
    // The last own_n writes live at slots (history_index_ - own_n + i) % LEN;
    // the base guard also covers the theoretical uint16 cursor wrap.
    const size_t own_n = (update_count < RSSI_LEN) ? update_count : RSSI_LEN;
    const size_t own_base = (static_cast<size_t>(history_index_) >= own_n)
        ? (static_cast<size_t>(history_index_) - own_n)
        : (static_cast<size_t>(history_index_) + RSSI_LEN - own_n);
    for (size_t i = 0; i < own_n; ++i) {
        const size_t slot = (own_base + i) % RSSI_LEN;
        merged[count].timestamp = timestamp_history_[slot];
        merged[count].rssi = rssi_history_[slot];
        ++count;
    }

    // Extract the absorbed entry's valid samples the same way.
    const size_t other_n = (other.update_count < RSSI_LEN) ? other.update_count : RSSI_LEN;
    const size_t other_base = (static_cast<size_t>(other.history_index_) >= other_n)
        ? (static_cast<size_t>(other.history_index_) - other_n)
        : (static_cast<size_t>(other.history_index_) + RSSI_LEN - other_n);
    for (size_t i = 0; i < other_n; ++i) {
        const size_t slot = (other_base + i) % RSSI_LEN;
        merged[count].timestamp = other.timestamp_history_[slot];
        merged[count].rssi = other.rssi_history_[slot];
        ++count;
    }

    // Insertion sort by timestamp, ascending. n <= 12 → max 66 comparisons.
    for (size_t i = 1; i < count; ++i) {
        const HistorySample key = merged[i];
        size_t j = i;
        while (j > 0 && merged[j - 1].timestamp > key.timestamp) {
            merged[j] = merged[j - 1];
            --j;
        }
        merged[j] = key;
    }

    // Keep only the newest RSSI_LEN samples; rewrite the circular buffer in
    // chronological order starting at slot 0 (matches the read pattern in
    // get_movement_trend(): start = history_index_ % LEN, n = update_count).
    const size_t keep = (count > RSSI_LEN) ? RSSI_LEN : count;
    const size_t first = count - keep;
    for (size_t i = 0; i < keep; ++i) {
        rssi_history_[i] = merged[first + i].rssi;
        timestamp_history_[i] = merged[first + i].timestamp;
    }
    history_index_ = static_cast<uint16_t>(keep);
    update_count = static_cast<uint8_t>(keep);

    if (keep > 0) {
        // Newest merged sample becomes the current observation — the merged
        // entry reads exactly as if every detection had hit one tracker.
        const HistorySample& newest = merged[count - 1];
        last_rssi_ = newest.rssi;
        rssi = newest.rssi;
        last_seen = newest.timestamp;
    }

    // ---- Lifecycle: keep the oldest birth, the freshest sighting ----
    if (other.created_time_ != 0
        && (created_time_ == 0 || other.created_time_ < created_time_)) {
        created_time_ = other.created_time_;
    }
    if (other.last_seen_time_ > last_seen_time_) {
        last_seen_time_ = other.last_seen_time_;
    }

    // ---- Decay state: most optimistic (the merged drone was just seen) ----
    if (other.missed_cycles_ < missed_cycles_) {
        missed_cycles_ = other.missed_cycles_;
    }
    if (other.sweep_cycles_missed_ < sweep_cycles_missed_) {
        sweep_cycles_missed_ = other.sweep_cycles_missed_;
    }
    if (other.rssi_decrease_counter_ < rssi_decrease_counter_) {
        rssi_decrease_counter_ = other.rssi_decrease_counter_;
    }

    // ---- RSSI-increase tracking: union, keep the freshest timestamp ----
    if (other.rssi_increased_) {
        rssi_increased_ = true;
        if (other.last_increase_time_ > last_increase_time_) {
            last_increase_time_ = other.last_increase_time_;
        }
    }

    // ---- Classification: fill UNKNOWN type, keep the higher threat ----
    // The next update_rssi() reclassifies the threat from the merged history.
    if (drone_type == DroneType::UNKNOWN && other.drone_type != DroneType::UNKNOWN) {
        drone_type = other.drone_type;
    }
    if (other.threat_level > threat_level) {
        threat_level = other.threat_level;
    }

    // ---- Sweep trend state: merge cycle peaks so the trend continues ----
    if (other.last_cycle_peak_rssi_ != SWEEP_CYCLE_PEAK_INVALID_DBM) {
        if (last_cycle_peak_rssi_ == SWEEP_CYCLE_PEAK_INVALID_DBM
            || other.last_cycle_peak_rssi_ > last_cycle_peak_rssi_) {
            last_cycle_peak_rssi_ = other.last_cycle_peak_rssi_;
        }
    }
    if (!has_prev_cycle_peak_ && other.has_prev_cycle_peak_) {
        prev_cycle_peak_rssi_ = other.prev_cycle_peak_rssi_;
        has_prev_cycle_peak_ = true;
    }
    sweep_mode_active_ = sweep_mode_active_ || other.sweep_mode_active_;

    // ---- Mahalanobis statistics: keep the richer sample set ----
    if (other.mahalanobis_stats_.sample_count > mahalanobis_stats_.sample_count) {
        mahalanobis_stats_ = other.mahalanobis_stats_;
    }

    // ---- Trend hysteresis: deliberately kept from the survivor (this).
    // The survivor is the HIGHEST-THREAT entry (ties broken by oldest), so
    // its cached_trend_ carries the most stable hysteresis state; merging
    // hold counters would be meaningless. ----
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
                // Sweep mode uses a tighter threshold (2 dB) than normal mode
                // (3 dB) because cycle-peak comparisons across frequencies have
                // less dynamic range than split-buffer averaging.
                constexpr int32_t SWEEP_THRESHOLD = SWEEP_TREND_THRESHOLD_DB;
                const int32_t diff = static_cast<int32_t>(last_cycle_peak_rssi_)
                                   - static_cast<int32_t>(prev_cycle_peak_rssi_);
                if (diff > SWEEP_THRESHOLD) {
                    raw_trend = MovementTrend::APPROACHING;
                } else if (diff < -SWEEP_THRESHOLD) {
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
    //
    // UNKNOWN is treated as "no data" rather than a trend disagreement.
    // It never increments the hold counter and never resets it. This
    // prevents the counter from being corrupted when raw_trend alternates
    // between UNKNOWN (drone not detected this pass) and a real trend.
    //
    // Transition from UNKNOWN to a real trend is accepted immediately —
    // the first evaluation with enough data should snap to a real value
    // rather than requiring TREND_HYSTERESIS_COUNT agreeing frames.
    if (raw_trend == cached_trend_) {
        trend_hold_count_ = 0;  // same direction — reset hold counter
    } else if (raw_trend == MovementTrend::UNKNOWN) {
        if (cached_trend_ != MovementTrend::UNKNOWN) {
            // No new data — hold the last known real trend.
            return cached_trend_;
        }
        // Both UNKNOWN — no data yet, fall through to assign UNKNOWN.
    } else if (cached_trend_ == MovementTrend::UNKNOWN) {
        // First real trend after UNKNOWN — accept immediately.
        trend_hold_count_ = 0;
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
