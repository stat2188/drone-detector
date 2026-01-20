// ui_drone_audio.hpp - Audio Alert Manager for Enhanced Drone Analyzer
// Migrated from detector_app patterns, simplified for mayhem-firmware integration

#ifndef UI_DRONE_AUDIO_HPP_
#define UI_DRONE_AUDIO_HPP_

#include <stdint.h>
#include "baseband_api.hpp"
#include "ui_drone_common_types.hpp"

// Simplified migraine to detector_app patterns - no static initialization issues
struct AudioAlertManager {
    static void play_alert(ThreatLevel level) {
        if (!audio_enabled_) return;

        uint16_t freq_hz = 800;
        switch (level) {
            case ThreatLevel::NONE: return; // No alert
            case ThreatLevel::LOW: freq_hz = 800; break;
            case ThreatLevel::MEDIUM: freq_hz = 1000; break;
            case ThreatLevel::HIGH: freq_hz = 1200; break;
            case ThreatLevel::CRITICAL: freq_hz = 2000; break;
            default: freq_hz = 800; break;
        }
        baseband::request_audio_beep(freq_hz, 24000, 200);
    }

    static void set_enabled(bool enable) { audio_enabled_ = enable; }
    static bool is_enabled() { return audio_enabled_; }

private:
    // Inline definition for C++17 (Mayhem standard) to allow header inclusion in multiple TUs
    inline static bool audio_enabled_ = true;
};

// Class-compatible interface to maintain API compatibility
// Simplified like detector_app's play_beep inline method
class AudioManager {
public:
    AudioManager() : audio_enabled_(true) {}
    ~AudioManager() {}

    bool is_audio_enabled() const { return AudioAlertManager::is_enabled(); }
    void toggle_audio() { AudioAlertManager::set_enabled(!AudioAlertManager::is_enabled()); }
    void play_detection_beep(ThreatLevel threat) { AudioAlertManager::play_alert(threat); }
    void stop_audio() { /* Simple beeps don't persist - no stop needed */ }

    // Legacy API for compatibility
    uint16_t get_alert_frequency() const { return 800; }
    void set_alert_frequency([[maybe_unused]] uint16_t freq) { (void)freq; /* Fixed to 800Hz like detector_app */ }
    uint32_t get_alert_duration_ms() const { return 200; }
    void set_alert_duration_ms([[maybe_unused]] uint32_t duration) { (void)duration; /* Fixed 200ms like detector_app */ }

private:
    bool audio_enabled_; // Not used - using global static
};

struct DroneAudioSettings {
    bool audio_enabled = true;
    ThreatLevel test_threat_level = ThreatLevel::HIGH;
};

#endif // UI_DRONE_AUDIO_HPP_
