#include "ui_drone_audio.hpp"

namespace ui::apps::enhanced_drone_analyzer {

bool AudioAlertManager::audio_enabled_ = true;
bool AudioAlertManager::baseband_is_ready_ = false;
systime_t AudioAlertManager::last_alert_timestamp_ = 0;
uint32_t AudioAlertManager::cooldown_ms_ = 600;

void AudioAlertManager::play_alert(ThreatLevel level) {
    if (!audio_enabled_) return;

    if (!baseband_is_ready_) return;

    systime_t now = chTimeNow();
    systime_t elapsed_ticks = now - last_alert_timestamp_;

    if (elapsed_ticks < MS2ST(cooldown_ms_)) {
        return;
    }

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

void AudioAlertManager::set_enabled(bool enable) {
    audio_enabled_ = enable;
}

bool AudioAlertManager::is_enabled() {
    return audio_enabled_;
}

void AudioAlertManager::set_cooldown_ms(uint32_t cooldown_ms) {
    cooldown_ms_ = cooldown_ms;
}

void AudioAlertManager::mark_baseband_ready() {
    baseband_is_ready_ = true;
}

bool AudioAlertManager::is_baseband_ready() {
    return baseband_is_ready_;
}

} // namespace ui::apps::enhanced_drone_analyzer
