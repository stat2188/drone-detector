// ui_drone_common_types.hpp - Common types for Enhanced Drone Analyzer
// Defines enums and structs used across multiple modules

#ifndef __UI_DRONE_COMMON_TYPES_HPP__
#define __UI_DRONE_COMMON_TYPES_HPP__

#include <cstdint>

// Define enums shared across modules
enum class ThreatLevel {
    NONE,
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum class DroneType {
    UNKNOWN,
    MAVIC,
    PHANTOM,
    DJI_MINI,
    PARROT_ANAFI,
    PARROT_BEBOP,
    PX4_DRONE,
    MILITARY_DRONE
};

enum class MovementTrend {
    UNKNOWN,
    STATIC,
    APPROACHING,
    RECEDING
};

enum class SpectrumMode {
    NARROW,
    MEDIUM,
    WIDE,
    ULTRA_WIDE
};

#endif // __UI_DRONE_COMMON_TYPES_HPP__
