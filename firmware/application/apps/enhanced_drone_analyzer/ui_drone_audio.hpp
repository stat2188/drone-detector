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

struct AudioAlertManager {
    // DIAMOND OPTIMIZATION: noexcept enables compiler optimization, avoids exception handling overhead
    static void play_alert(ThreatLevel level) noexcept {
        if (!audio_enabled_) return;
        systime_t now = chTimeNow();
        systime_t elapsed_ticks = now - last_alert_timestamp_;
        if (elapsed_ticks < MS2ST(cooldown_ms_)) return;
        last_alert_timestamp_ = now;
        uint16_t freq_hz = 800;
        switch (level) {
            case ThreatLevel::NONE: return;
            case ThreatLevel::LOW: freq_hz = 800; break;
            case ThreatLevel::MEDIUM: freq_hz = 1000; break;
            case ThreatLevel::HIGH: freq_hz = 1200; break;
            case ThreatLevel::CRITICAL: freq_hz = 2000; break;
            default: freq_hz = 800; break;
        }
        baseband::request_audio_beep(freq_hz, 24000, 200);
    }
    // DIAMOND OPTIMIZATION: noexcept for zero-overhead abstraction
    static void set_enabled(bool enable) noexcept { audio_enabled_ = enable; }
    static bool is_enabled() noexcept { return audio_enabled_; }
    static void set_cooldown_ms(uint32_t cooldown_ms) noexcept { cooldown_ms_ = cooldown_ms; }

private:
    inline static bool audio_enabled_ = true;
    inline static systime_t last_alert_timestamp_ = 0;
    inline static uint32_t cooldown_ms_ = 100;
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
