// ui_drone_audio.hpp - Audio Alert Manager for Enhanced Drone Analyzer
// Migrated from detector_app patterns, simplified for mayhem-firmware integration

#ifndef UI_DRONE_AUDIO_HPP_
#define UI_DRONE_AUDIO_HPP_

#include <stdint.h>
#include "baseband_api.hpp"
#include "ui_drone_common_types.hpp"

// DIAMOND FIX: Moved audio_enabled_ to .cpp to fix ODR violation
// Prevents memory corruption from multiple definitions across translation units
struct AudioAlertManager {
    static void play_alert(ui::apps::enhanced_drone_analyzer::ThreatLevel level);
    static void set_enabled(bool enable);
    static bool is_enabled();

private:
    // DIAMOND FIX: Declaration only - definition moved to .cpp
    // Single instance prevents ODR violation and undefined behavior
    static bool audio_enabled_;
};

// Class-compatible interface to maintain API compatibility
// Simplified like detector_app's play_beep inline method
class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() = default;

    bool is_audio_enabled() const { return AudioAlertManager::is_enabled(); }
    void toggle_audio() { AudioAlertManager::set_enabled(!AudioAlertManager::is_enabled()); }
    void play_detection_beep(ui::apps::enhanced_drone_analyzer::ThreatLevel threat) {
        AudioAlertManager::play_alert(threat);
    }
    void stop_audio() { /* Simple beeps don't persist - no stop needed */ }

    // Legacy API for compatibility
    uint16_t get_alert_frequency() const { return 800; }
    void set_alert_frequency([[maybe_unused]] uint16_t freq) { (void)freq; /* Fixed to 800Hz like detector_app */ }
    uint32_t get_alert_duration_ms() const { return 200; }
    void set_alert_duration_ms([[maybe_unused]] uint32_t duration) { (void)duration; /* Fixed 200ms like detector_app */ }
};

struct DroneAudioSettings {
    bool audio_enabled = true;
    ui::apps::enhanced_drone_analyzer::ThreatLevel test_threat_level = ui::apps::enhanced_drone_analyzer::ThreatLevel::HIGH;
};

#endif // UI_DRONE_AUDIO_HPP_
