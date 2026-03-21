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
    , drone_type{DroneType::UNKNOWN}
    , threat_level{ThreatLevel::NONE}
    , update_count{0}
    , last_seen{0}
    , rssi{RSSI_NOISE_FLOOR_DBM}
    , rssi_history_{0}
    , timestamp_history_{0}
    , history_index_{0} {
}

TrackedDrone::TrackedDrone(
    FreqHz freq,
    DroneType type,
    ThreatLevel threat
) noexcept
    : frequency{freq}
    , drone_type{type}
    , threat_level{threat}
    , update_count{0}
    , last_seen{0}
    , rssi{RSSI_NOISE_FLOOR_DBM}
    , rssi_history_{0}
    , timestamp_history_{0}
    , history_index_{0} {
}

void TrackedDrone::update_rssi(RssiValue new_rssi, SystemTime timestamp) noexcept {
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
    
    // Require minimum samples before HIGH/CRITICAL classification
    // to prevent false positives from single noisy readings
    if (update_count < 2) {
        threat_level = ThreatLevel::LOW;
        return;
    }

    // Use average RSSI for threat classification (smoother than single sample)
    // This prevents a single noise spike from triggering CRITICAL
    const RssiValue avg = get_average_rssi();
    
    if (avg >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM) {
        threat_level = ThreatLevel::CRITICAL;
    } else if (avg >= RSSI_HIGH_THREAT_THRESHOLD_DBM) {
        threat_level = ThreatLevel::HIGH;
    } else if (avg >= RSSI_DETECTION_THRESHOLD_DBM) {
        threat_level = ThreatLevel::MEDIUM;
    } else {
        threat_level = ThreatLevel::LOW;
    }
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

DisplayDroneEntry::DisplayDroneEntry(const TrackedDrone& drone) noexcept
    : frequency(drone.frequency)
    , type(drone.drone_type)
    , threat(drone.threat_level)
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
    type_name[i] = '\0';
    
    set_color_from_threat();
}

const char* DisplayDroneEntry::get_type_name() const noexcept {
    return type_name;
}

MovementTrend TrackedDrone::get_movement_trend() const noexcept {
    if (update_count < MOVEMENT_TREND_MIN_HISTORY) {
        return MovementTrend::UNKNOWN;
    }

    constexpr uint8_t HALF_HISTORY = RSSI_HISTORY_SIZE / 2;
    constexpr int32_t APPROACHING_THRESHOLD = MOVEMENT_TREND_THRESHOLD_APPROACHING_DB;
    constexpr int32_t RECEDED_THRESHOLD = MOVEMENT_TREND_THRESHOLD_RECEEDING_DB;
    constexpr int32_t SILENCE_THRESHOLD = MOVEMENT_TREND_SILENCE_THRESHOLD_DBM;

    int32_t older_sum = 0;
    int32_t recent_sum = 0;
    uint8_t older_count = 0;
    uint8_t recent_count = 0;

    for (size_t i = 0; i < RSSI_HISTORY_SIZE; ++i) {
        const uint8_t logical_idx = (history_index_ + i) % RSSI_HISTORY_SIZE;
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

    if (older_count == 0 || recent_count == 0) {
        return MovementTrend::UNKNOWN;
    }

    const int32_t avg_old = older_sum / older_count;
    const int32_t avg_new = recent_sum / recent_count;
    const int32_t diff = avg_new - avg_old;

    if (diff > APPROACHING_THRESHOLD) {
        return MovementTrend::APPROACHING;
    }
    if (diff < RECEDED_THRESHOLD) {
        return MovementTrend::RECEDING;
    }
    return MovementTrend::STATIC;
}

} // namespace drone_analyzer
