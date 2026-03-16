#include "constants.hpp"
#include "drone_types.hpp"
#include <cstdint>
#include <cstddef>

namespace drone_analyzer {

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
