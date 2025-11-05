// ui_drone_audio.hpp - Audio Alert Manager for Enhanced Drone Analyzer
// Provides intelligent threat-based audio alerts (migrated from Looking Glass)

#ifndef __UI_DRONE_AUDIO_HPP__
#define __UI_DRONE_AUDIO_HPP__

#include <stdint.h>
#include "ui_drone_common_types.hpp"
#include "baseband_api.hpp"

// AudioAlertManager migrated from Looking Glass
class AudioAlertManager {
public:
    enum class AlertLevel { NONE, LOW, HIGH, CRITICAL };

    AudioAlertManager() = default;

    static void play_alert(AlertLevel level) {
        if (!audio_enabled_) return;

        uint16_t freq_hz = 800;
        switch (level) {
            case AlertLevel::NONE: return;
            case AlertLevel::LOW: freq_hz = 800; break;
            case AlertLevel::HIGH: freq_hz = 1200; break;
            case AlertLevel::CRITICAL: freq_hz = 2000; break;
        }
        baseband::request_audio_beep(freq_hz, 48000, 200);
    }

    static void set_enabled(bool enable) { audio_enabled_ = enable; }
    static bool is_enabled() { return audio_enabled_; }

private:
    static bool audio_enabled_;
};

#ifndef UI_DRONE_AUDIO_HPP_STATIC_INIT
#define UI_DRONE_AUDIO_HPP_STATIC_INIT
bool AudioAlertManager::audio_enabled_ = true;
#endif // UI_DRONE_AUDIO_HPP_STATIC_INIT

#include "baseband_api.hpp"

// Legacy AudioManager class for backward compatibility
class AudioManager {
public:
    AudioManager() : audio_enabled_(true) {}
    ~AudioManager() {}

    // Core audio control
    bool is_audio_enabled() const { return AudioAlertManager::is_enabled(); }
    void toggle_audio() { AudioAlertManager::set_enabled(!AudioAlertManager::is_enabled()); }
    void play_detection_beep(ThreatLevel threat) { play_alert_for_threat(threat); }
    void stop_audio() { /* Implementation not needed for simple beeps */ }

    // Audio parameter getters/setters (use legacy interface)
    uint16_t get_alert_frequency() const { return 800; }
    void set_alert_frequency(uint16_t freq) { (void)freq; /* Not supported - use AlertManager */ }
    uint32_t get_alert_duration_ms() const { return 200; } // Fixed duration as per Looking Glass
    void set_alert_duration_ms(uint32_t duration) { (void)duration; /* Not supported in Looking Glass */ }

private:
    static void play_alert_for_threat(ThreatLevel threat) {
        using AlertLevel = AudioAlertManager::AlertLevel;
        AlertLevel level = AlertLevel::LOW;
        switch (threat) {
            case ThreatLevel::NONE: return; // No alert
            case ThreatLevel::LOW: level = AlertLevel::LOW; break;
            case ThreatLevel::MEDIUM: level = AlertLevel::HIGH; break;
            case ThreatLevel::HIGH: level = AlertLevel::HIGH; break;
            case ThreatLevel::CRITICAL: level = AlertLevel::CRITICAL; break;
            default: level = AlertLevel::LOW; break;
        }
        AudioAlertManager::play_alert(level);
    }

    bool audio_enabled_;
};

struct DroneAudioSettings {
    bool audio_enabled = true;
    ThreatLevel test_threat_level = ThreatLevel::HIGH; // Use enum instead of int
};

#endif // __UI_DRONE_AUDIO_HPP__
