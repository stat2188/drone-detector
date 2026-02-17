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

// ========================================
// TYPE ALIASES (Semantic Types)
// ========================================
using AudioFrequency = uint16_t;
using AudioDuration = uint32_t;
using AudioVolume = uint8_t;
using CooldownMs = uint32_t;

// ========================================
// CONSTANTS
// ========================================
namespace AudioConstants {
    constexpr AudioFrequency DEFAULT_LOW_FREQ = 800;
    constexpr AudioFrequency DEFAULT_MEDIUM_FREQ = 1000;
    constexpr AudioFrequency DEFAULT_HIGH_FREQ = 1200;
    constexpr AudioFrequency DEFAULT_CRITICAL_FREQ = 2000;
    constexpr AudioDuration BEEP_DURATION = 200;
    constexpr uint32_t BEEP_SAMPLE_RATE = 24000;
}

// 🔴 HIGH PRIORITY FIX: Add mutex protection for static variables
// Prevents race conditions when audio is accessed from multiple threads
static Mutex audio_mutex;

struct AudioAlertManager {
    // DIAMOND OPTIMIZATION: noexcept enables compiler optimization, avoids exception handling overhead
    static void play_alert(ThreatLevel level) noexcept {
        chMtxLock(&audio_mutex);
        // FIX: Store audio_enabled_ in local variable while holding lock to prevent race condition
        bool audio_enabled = audio_enabled_;
        if (!audio_enabled) {
            chMtxUnlock();
            return;
        }
        systime_t now = chTimeNow();
        systime_t elapsed_ticks = now - last_alert_timestamp_;
        if (elapsed_ticks < MS2ST(cooldown_ms_)) {
            chMtxUnlock();
            return;
        }
        last_alert_timestamp_ = now;
        AudioFrequency freq_hz = AudioConstants::DEFAULT_LOW_FREQ;
        switch (level) {
            case ThreatLevel::NONE: chMtxUnlock(); return;
            case ThreatLevel::LOW: freq_hz = AudioConstants::DEFAULT_LOW_FREQ; break;
            case ThreatLevel::MEDIUM: freq_hz = AudioConstants::DEFAULT_MEDIUM_FREQ; break;
            case ThreatLevel::HIGH: freq_hz = AudioConstants::DEFAULT_HIGH_FREQ; break;
            case ThreatLevel::CRITICAL: freq_hz = AudioConstants::DEFAULT_CRITICAL_FREQ; break;
            default: freq_hz = AudioConstants::DEFAULT_LOW_FREQ; break;
        }
        chMtxUnlock();
        // Use local variable to ensure consistent state
        if (audio_enabled) {
            baseband::request_audio_beep(freq_hz, AudioConstants::BEEP_SAMPLE_RATE, AudioConstants::BEEP_DURATION);
        }
    }
    // DIAMOND OPTIMIZATION: noexcept for zero-overhead abstraction
    static void set_enabled(bool enable) noexcept {
        chMtxLock(&audio_mutex);
        audio_enabled_ = enable;
        chMtxUnlock();
    }
    static bool is_enabled() noexcept {
        chMtxLock(&audio_mutex);
        bool enabled = audio_enabled_;
        chMtxUnlock();
        return enabled;
    }
    static void set_cooldown_ms(CooldownMs cooldown_ms) noexcept {
        chMtxLock(&audio_mutex);
        cooldown_ms_ = cooldown_ms;
        chMtxUnlock();
    }

private:
    inline static bool audio_enabled_ = true;
    inline static systime_t last_alert_timestamp_ = 0;
    // 🔴 PHASE 3: Use constant from EDA::Constants instead of magic number
    inline static CooldownMs cooldown_ms_ = EDA::Constants::DEFAULT_ALERT_COOLDOWN_MS;
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
};

struct DroneAudioSettings {
    bool audio_enabled = true;
    ui::apps::enhanced_drone_analyzer::ThreatLevel test_threat_level = ui::apps::enhanced_drone_analyzer::ThreatLevel::HIGH;
};

#endif // UI_DRONE_AUDIO_HPP_
