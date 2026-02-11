// ui_drone_audio.hpp - Audio Alert Manager for Enhanced Drone Analyzer
// Migrated from detector_app patterns, simplified for mayhem-firmware integration
// DIAMOND OPTIMIZATION: Time-based debouncing prevents UI freeze from baseband queue saturation

#ifndef UI_DRONE_AUDIO_HPP_
#define UI_DRONE_AUDIO_HPP_

#include <stdint.h>
#include <ch.h>
#include "baseband_api.hpp"
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// DIAMOND FIX: Moved audio_enabled_ to .cpp to fix ODR violation
// Prevents memory corruption from multiple definitions across translation units
// DIAMOND OPTIMIZATION: Added last_alert_timestamp_ for debouncing to prevent UI freeze
// DIAMOND FIX: Added baseband_is_ready_ guard to prevent deadlock during initialization
struct AudioAlertManager {
    static void play_alert(ThreatLevel level);
    static void set_enabled(bool enable);
    static bool is_enabled();
    static void set_cooldown_ms(uint32_t cooldown_ms);
    
    // CRITICAL FIX: Mark baseband as ready after baseband::run_image() succeeds
    // Prevents deadlock in send_message() busy-wait loop (baseband_api.cpp:62)
    static void mark_baseband_ready();
    static bool is_baseband_ready();

private:
    // DIAMOND FIX: Declaration only - definition moved to .cpp
    // Single instance prevents ODR violation and undefined behavior
    static bool audio_enabled_;
    static bool baseband_is_ready_;  // NEW: Guard flag to prevent calls before baseband startup
    
    // DIAMOND OPTIMIZATION: Last alert timestamp for debouncing
    // Prevents baseband queue saturation when called at 60 FPS
    // baseband::send_message() has busy-wait spin loop (baseband_api.cpp:54-64)
    // Without debouncing, UI thread would freeze indefinitely
    static systime_t last_alert_timestamp_;
    static uint32_t cooldown_ms_;
};

} // namespace ui::apps::enhanced_drone_analyzer

// Class-compatible interface to maintain API compatibility
// Simplified like detector_app's play_beep inline method
class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() = default;

    bool is_audio_enabled() const { return ui::apps::enhanced_drone_analyzer::AudioAlertManager::is_enabled(); }
    void toggle_audio() { ui::apps::enhanced_drone_analyzer::AudioAlertManager::set_enabled(!ui::apps::enhanced_drone_analyzer::AudioAlertManager::is_enabled()); }
    void play_detection_beep(ui::apps::enhanced_drone_analyzer::ThreatLevel threat) {
        ui::apps::enhanced_drone_analyzer::AudioAlertManager::play_alert(threat);
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
