// ui_drone_audio.hpp - Audio Manager for Enhanced Drone Analyzer
// Provides audio alert functionality for drone detection

#ifndef __UI_DRONE_AUDIO_HPP__
#define __UI_DRONE_AUDIO_HPP__

#include <cstdint>

class AudioManager {
public:
    AudioManager();
    virtual ~AudioManager();

    // Core audio control
    virtual bool is_audio_enabled() const;
    virtual void toggle_audio();
    virtual void play_detection_beep(ThreatLevel threat);
    virtual void stop_audio();

    // Audio parameter getters/setters
    virtual uint16_t get_alert_frequency() const { return 800; }
    virtual void set_alert_frequency(uint16_t freq) { (void)freq; /* TODO: Implement frequency mapping */ }
    virtual uint32_t get_alert_duration_ms() const { return 500; }
    virtual void set_alert_duration_ms(uint32_t duration) { (void)duration; /* TODO: Implement duration control */ }

private:
    bool audio_enabled_;
};

struct DroneAudioSettings {
    bool audio_enabled = true;
    ThreatLevel test_threat_level = ThreatLevel::HIGH; // Use enum instead of int
};

#endif // __UI_DRONE_AUDIO_HPP__
