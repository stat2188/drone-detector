/*
 * Copyright (C) 2025 Enhanced Drone Analyzer
 *
 * This file is part of PortaPack Mayhem Firmware.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef AUDIO_ALERTS_HPP
#define AUDIO_ALERTS_HPP

#include <cstdint>
#include <cstddef>
#include "drone_types.hpp"

namespace drone_analyzer {

/**
 * @brief Alert types for drone detection
 */
enum class AlertType : uint8_t {
    NEW_DRONE = 0,        ///< New drone detected
    THREAT_INCREASED = 1,  ///< Threat level increased
    THREAT_CRITICAL = 2,     ///< Critical threat level
    DRONE_APPROACHING = 3,  ///< Drone approaching (RSSI increasing)
    DRONE_RECEDING = 4      ///< Drone receding (RSSI decreasing)
};

/**
 * @brief Alert priority levels
 */
enum class AlertPriority : uint8_t {
    LOW = 0,      ///< Low priority (visual only)
    MEDIUM = 1,    ///< Medium priority
    HIGH = 2,      ///< High priority
    CRITICAL = 3    ///< Critical priority (immediate alert)
};

/**
 * @brief Audio alert configuration
 */
struct AudioAlertConfig {
    uint32_t frequency_hz;      ///< Alert frequency in Hz
    uint32_t duration_ms;       ///< Alert duration in milliseconds
    uint32_t sample_rate_hz;    ///< Sample rate in Hz
    uint8_t beep_count;         ///< Number of beeps in sequence
    uint32_t beep_gap_ms;       ///< Gap between beeps in milliseconds
    AlertPriority priority;       ///< Alert priority
    
    /**
     * @brief Default constructor
     */
    AudioAlertConfig() noexcept;
    
    /**
     * @brief Constructor with values
     */
    AudioAlertConfig(
        uint32_t freq,
        uint32_t duration,
        uint32_t sample_rate,
        uint8_t count,
        uint32_t gap,
        AlertPriority prio
    ) noexcept;
};

/**
 * @brief Audio alert manager
 * @note Handles audio alerts for drone detection
 * @note Non-blocking implementation using baseband API
 */
class AudioAlertManager {
public:
    /**
     * @brief Get alert configuration for alert type
     * @param alert_type Alert type
     * @return Audio alert configuration
     */
    static const AudioAlertConfig& get_alert_config(AlertType alert_type) noexcept;
    
    /**
     * @brief Get alert configuration for threat level
     * @param threat_level Threat level
     * @return Audio alert configuration
     */
    static const AudioAlertConfig& get_threat_alert_config(ThreatLevel threat_level) noexcept;
    
    /**
     * @brief Play audio alert
     * @param config Audio alert configuration
     * @note Non-blocking - sends message to baseband processor
     */
    static void play_alert(const AudioAlertConfig& config) noexcept;
    
    /**
     * @brief Play audio alert for alert type
     * @param alert_type Alert type
     * @note Non-blocking - sends message to baseband processor
     */
    static void play_alert(AlertType alert_type) noexcept;
    
    /**
     * @brief Play audio alert for threat level
     * @param threat_level Threat level
     * @note Non-blocking - sends message to baseband processor
     */
    static void play_alert(ThreatLevel threat_level) noexcept;
    
    /**
     * @brief Stop audio alerts
     * @note Stops any ongoing audio playback
     */
    static void stop_alert() noexcept;
    
    /**
     * @brief Check if audio alerts are enabled
     * @return true if enabled, false otherwise
     */
    static bool is_enabled() noexcept;
    
    /**
     * @brief Enable or disable audio alerts
     * @param enabled Enable state
     */
    static void set_enabled(bool enabled) noexcept;
    
private:
    // Audio alerts enabled flag
    static bool enabled_;
    
    // Audio initialization flag (to avoid repeated initialization)
    static bool audio_initialized_;
    
    // Current alert priority (for priority override logic)
    static AlertPriority current_priority_;
};

} // namespace drone_analyzer

#endif // AUDIO_ALERTS_HPP
