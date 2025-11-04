// ui_drone_audio.hpp - Audio Alert Manager for Enhanced Drone Analyzer
// Provides intelligent threat-based audio alerts (migrated from Looking Glass)

#ifndef __UI_DRONE_AUDIO_HPP__
#define __UI_DRONE_AUDIO_HPP__

#include <stdint.h>
#include "ui_drone_common_types.hpp"
#include "../baseband_api.hpp"

class AudioAlertManager {
public:
    enum class AlertLevel { NONE, LOW, HIGH, CRITICAL };

    static void play_alert(ThreatLevel threat) {
        if (!audio_enabled_) return;

        AlertLevel level = map_threat_to_alert(threat);
        play_alert(level);
    }

    static void play_alert(AlertLevel level) {
        if (!audio_enabled_) return;

        uint16_t freq_hz = get_alert_frequency(level);
        uint32_t duration_ms = get_alert_duration(level);

        // Use baseband API to generate audio beep
        baseband::request_audio_beep(freq_hz, 48000, duration_ms);
    }

    static void set_enabled(bool enable) { audio_enabled_ = enable; }
    static bool is_enabled() { return audio_enabled_; }

    static void set_alert_frequency(ThreatLevel threat, uint16_t freq) {
        alert_frequencies_[static_cast<size_t>(threat)] = freq;
    }

    static void set_alert_duration(uint32_t duration) { alert_duration_ms_ = duration; }

private:
    static AlertLevel map_threat_to_alert(ThreatLevel threat) {
        switch (threat) {
            case ThreatLevel::NONE: return AlertLevel::NONE;
            case ThreatLevel::LOW: return AlertLevel::LOW;
            case ThreatLevel::MEDIUM: return AlertLevel::LOW;
            case ThreatLevel::HIGH: return AlertLevel::HIGH;
            case ThreatLevel::CRITICAL: return AlertLevel::CRITICAL;
            default: return AlertLevel::NONE;
        }
    }

    static uint16_t get_alert_frequency(AlertLevel level) {
        switch (level) {
            case AlertLevel::LOW: return alert_frequencies_[static_cast<size_t>(ThreatLevel::LOW)];
            case AlertLevel::HIGH: return alert_frequencies_[static_cast<size_t>(ThreatLevel::HIGH)];
            case AlertLevel::CRITICAL: return alert_frequencies_[static_cast<size_t>(ThreatLevel::CRITICAL)];
            default: return 800;
        }
    }

    static uint32_t get_alert_duration(AlertLevel level) {
        return alert_duration_ms_;
    }

    // Static member definitions
    inline static bool audio_enabled_ = true;
    inline static uint16_t alert_frequencies_[5] = {800, 800, 1200, 1500, 2000}; // NONE, LOW, MEDIUM, HIGH, CRITICAL
    inline static uint32_t alert_duration_ms_ = 200;
};

// Legacy AudioManager class for backward compatibility
class AudioManager {
public:
    AudioManager() : audio_enabled_(true) {}
    ~AudioManager() {}

    // Core audio control
    bool is_audio_enabled() const { return AudioAlertManager::is_enabled(); }
    void toggle_audio() { AudioAlertManager::set_enabled(!AudioAlertManager::is_enabled()); }
    void play_detection_beep(ThreatLevel threat) { AudioAlertManager::play_alert(threat); }
    void stop_audio() { /* Implementation not needed for simple beeps */ }

    // Audio parameter getters/setters (use legacy interface)
    uint16_t get_alert_frequency() const { return 800; }
    void set_alert_frequency(uint16_t freq) { (void)freq; /* Not supported - use AlertManager */ }
    uint32_t get_alert_duration_ms() const { return 500; }
    void set_alert_duration_ms(uint32_t duration) { AudioAlertManager::set_alert_duration(duration); }

private:
    bool audio_enabled_;
};

struct DroneAudioSettings {
    bool audio_enabled = true;
    ThreatLevel test_threat_level = ThreatLevel::HIGH; // Use enum instead of int
};

#endif // __UI_DRONE_AUDIO_HPP__
