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

#include "audio_alerts.hpp"
#include "baseband_api.hpp"
#include "audio.hpp"

namespace drone_analyzer {

// ============================================================================
// Static Member Initialization
// ============================================================================

bool AudioAlertManager::enabled_ = true;
bool AudioAlertManager::audio_initialized_ = false;
AlertPriority AudioAlertManager::current_priority_ = AlertPriority::LOW;

// ============================================================================
// AudioAlertConfig Implementation
// ============================================================================

AudioAlertConfig::AudioAlertConfig() noexcept
    : frequency_hz(1000)
    , duration_ms(150)
    , sample_rate_hz(24000)
    , beep_count(1)
    , beep_gap_ms(0)
    , priority(AlertPriority::MEDIUM) {
}

AudioAlertConfig::AudioAlertConfig(
    uint32_t freq,
    uint32_t duration,
    uint32_t sample_rate,
    uint8_t count,
    uint32_t gap,
    AlertPriority prio
) noexcept
    : frequency_hz(freq)
    , duration_ms(duration)
    , sample_rate_hz(sample_rate)
    , beep_count(count)
    , beep_gap_ms(gap)
    , priority(prio) {
}

// ============================================================================
// AudioAlertManager Implementation
// ============================================================================

const AudioAlertConfig& AudioAlertManager::get_alert_config(AlertType alert_type) noexcept {
    // Static configurations for each alert type
    static const AudioAlertConfig configs[] = {
        // NEW_DRONE: Single beep at 1000 Hz
        AudioAlertConfig(1000, 150, 24000, 1, 0, AlertPriority::MEDIUM),
        
        // THREAT_INCREASED: Double beep at 1200 Hz
        AudioAlertConfig(1200, 100, 24000, 2, 50, AlertPriority::HIGH),
        
        // THREAT_CRITICAL: Triple beep at 1500 Hz
        AudioAlertConfig(1500, 80, 24000, 3, 40, AlertPriority::CRITICAL),
        
        // DRONE_APPROACHING: Single beep at 1200 Hz (rising tone)
        AudioAlertConfig(1200, 200, 24000, 1, 0, AlertPriority::HIGH),
        
        // DRONE_RECEDING: Single beep at 800 Hz (falling tone)
        AudioAlertConfig(800, 200, 24000, 1, 0, AlertPriority::LOW)
    };
    
    const uint8_t index = static_cast<uint8_t>(alert_type);
    if (index < sizeof(configs) / sizeof(configs[0])) {
        return configs[index];
    }
    
    // Default to NEW_DRONE configuration
    return configs[0];
}

const AudioAlertConfig& AudioAlertManager::get_threat_alert_config(ThreatLevel threat_level) noexcept {
    // Map threat levels to alert configurations
    switch (threat_level) {
        case ThreatLevel::CRITICAL:
            return get_alert_config(AlertType::THREAT_CRITICAL);
            
        case ThreatLevel::HIGH:
            return get_alert_config(AlertType::THREAT_INCREASED);
            
        case ThreatLevel::MEDIUM:
            return get_alert_config(AlertType::NEW_DRONE);
            
        case ThreatLevel::LOW:
        case ThreatLevel::NONE:
        default:
            // No audio alert for low/none threat
            static const AudioAlertConfig no_alert(0, 0, 24000, 0, 0, AlertPriority::LOW);
            return no_alert;
    }
}

void AudioAlertManager::play_alert(const AudioAlertConfig& config) noexcept {
    // Check if audio alerts are enabled
    if (!enabled_) {
        return;
    }
    
    // Check if priority is LOW (no audio alert)
    if (config.priority == AlertPriority::LOW) {
        return;
    }
    
    // Priority override logic: only play if priority is higher or equal to current
    if (config.priority < current_priority_) {
        // Lower priority alert, don't play
        return;
    }
    
    // Update current priority
    current_priority_ = config.priority;
    
    // Initialize audio only once
    if (!audio_initialized_) {
        audio::set_rate(audio::Rate::Hz_24000);
        audio::output::start();
        audio_initialized_ = true;
    }
    
    // Play beep sequence
    for (uint8_t i = 0; i < config.beep_count; ++i) {
        // Play beep
        baseband::request_audio_beep(
            config.frequency_hz,
            config.sample_rate_hz,
            config.duration_ms
        );
        
        // Wait for gap between beeps (if not last beep)
        if (i < config.beep_count - 1 && config.beep_gap_ms > 0) {
            // Note: Beep gaps are not yet implemented due to timer constraints.
            // All beeps in a sequence will play immediately without spacing.
            // Future implementation should use a timer to properly space beeps
            // according to beep_gap_ms.
            // For now, we rely on the baseband's non-blocking nature
            // and let the next beep be queued.
        }
    }
}

void AudioAlertManager::play_alert(AlertType alert_type) noexcept {
    const AudioAlertConfig& config = get_alert_config(alert_type);
    play_alert(config);
}

void AudioAlertManager::play_alert(ThreatLevel threat_level) noexcept {
    const AudioAlertConfig& config = get_threat_alert_config(threat_level);
    play_alert(config);
}

void AudioAlertManager::stop_alert() noexcept {
    // Stop any ongoing audio beep
    baseband::request_beep_stop();
    
    // Reset current priority
    current_priority_ = AlertPriority::LOW;
}

bool AudioAlertManager::is_enabled() noexcept {
    return enabled_;
}

void AudioAlertManager::set_enabled(bool enabled) noexcept {
    enabled_ = enabled;
    
    // Stop audio if disabled
    if (!enabled) {
        stop_alert();
    }
}

} // namespace drone_analyzer
