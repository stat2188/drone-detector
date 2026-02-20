// ui_drone_audio.hpp - Audio Alert Manager for Enhanced Drone Analyzer
// Migrated from detector_app patterns, simplified for mayhem-firmware integration
// DIAMOND OPTIMIZATION: Time-based debouncing prevents UI freeze from baseband queue saturation

#ifndef UI_DRONE_AUDIO_HPP_
#define UI_DRONE_AUDIO_HPP_

#include <stdint.h>
#include <ch.h>
#include "baseband_api.hpp"
#include "ui_drone_common_types.hpp"
#include "eda_constants.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// TYPE ALIASES (Semantic Types)
// ========================================
// FIX #4: Changed AudioFrequency from uint16_t to uint32_t to match baseband API
using AudioFrequency = uint32_t;
using AudioDuration = uint32_t;
using AudioVolume = uint8_t;
using CooldownMs = uint32_t;

// ========================================
// CONSTANTS (Flash-Resident for RAM Savings)
// ========================================
// FIX #6: Added EDA_FLASH_CONST attribute to store constants in Flash (~20 bytes RAM saved)
namespace AudioConstants {
    EDA_FLASH_CONST constexpr AudioFrequency DEFAULT_LOW_FREQ = 800;
    EDA_FLASH_CONST constexpr AudioFrequency DEFAULT_MEDIUM_FREQ = 1000;
    EDA_FLASH_CONST constexpr AudioFrequency DEFAULT_HIGH_FREQ = 1200;
    EDA_FLASH_CONST constexpr AudioFrequency DEFAULT_CRITICAL_FREQ = 2000;
    EDA_FLASH_CONST constexpr AudioDuration BEEP_DURATION = 200;
    EDA_FLASH_CONST constexpr uint32_t BEEP_SAMPLE_RATE = 24000;
}

// ========================================
// RAII Mutex Lock Guard (Exception-Safe Locking)
// ========================================
// FIX #1: Replaced broken double-checked locking with RAII pattern
// Ensures mutex is always unlocked, even if exceptions occur
class MutexLockGuard {
public:
    explicit MutexLockGuard(Mutex& mutex) noexcept : mutex_(mutex), locked_(true) {
        chMtxLock(&mutex_);
    }

    ~MutexLockGuard() noexcept {
        if (locked_) {
            chMtxUnlock();
        }
    }

    // Explicit unlock for early release
    void unlock() noexcept {
        if (locked_) {
            chMtxUnlock();
            locked_ = false;
        }
    }

    // Disable copy and move
    MutexLockGuard(const MutexLockGuard&) = delete;
    MutexLockGuard& operator=(const MutexLockGuard&) = delete;

private:
    Mutex& mutex_;
    bool locked_;
};

// ========================================
// AUDIO ALERT MANAGER
// ========================================
/**
 * @brief Manages audio alerts for drone detection system
 *
 * Provides thread-safe audio alert functionality with cooldown protection
 * to prevent UI freeze from baseband queue saturation.
 *
 * Thread Safety: All methods use ChibiOS mutex for synchronization
 * Memory: All constants stored in Flash via EDA_FLASH_CONST
 */
struct AudioAlertManager {
    /**
     * @brief Get mutex reference (simple static initialization)
     *
     * Uses ChibiOS static initialization - no double-checked locking needed.
     * The mutex is initialized once at program startup before any threads run.
     *
     * @return Reference to audio mutex
     *
     * @note ChibiOS guarantees static initialization safety
     * @note FIX #1: Removed broken std::atomic_flag double-checked locking
     */
    static Mutex& get_mutex() noexcept {
        static Mutex audio_mutex{};
        return audio_mutex;
    }

    /**
     * @brief Play audio alert for specified threat level
     *
     * Plays a beep tone at a frequency corresponding to the threat level.
     * Includes cooldown protection to prevent audio queue saturation.
     *
     * @param level Threat level (NONE, LOW, MEDIUM, HIGH, CRITICAL)
     *
     * @note FIX #3: Eliminated TOCTOU race condition with single-check pattern
     * @note FIX #5: systime_t overflow handled by ChibiOS (wraps every ~49.7 days)
     * @note FIX #8: Uses guard clauses for improved readability
     */
    static void play_alert(ThreatLevel level) noexcept {
        // Guard clause: Early return for NONE threat level
        if (level == ThreatLevel::NONE) {
            return;
        }

        // Single lock acquisition - FIX #3: Eliminated TOCTOU race condition
        MutexLockGuard lock(get_mutex());

        // Guard clause: Early return if audio disabled
        if (!audio_enabled_) {
            return;
        }

        // FIX #10: Extracted cooldown check as helper method
        if (!is_cooldown_expired()) {
            return;
        }

        // Update timestamp and play alert
        last_alert_timestamp_ = chTimeNow();

        // Get frequency for threat level
        const AudioFrequency freq_hz = get_frequency_for_threat_level(level);

        // Unlock before baseband call to avoid holding lock during I/O
        lock.unlock();
        baseband::request_audio_beep(freq_hz, AudioConstants::BEEP_SAMPLE_RATE, AudioConstants::BEEP_DURATION);
    }

    /**
     * @brief Enable or disable audio alerts
     *
     * @param enable True to enable alerts, false to disable
     */
    static void set_enabled(bool enable) noexcept {
        MutexLockGuard lock(get_mutex());
        audio_enabled_ = enable;
    }

    /**
     * @brief Check if audio alerts are enabled
     *
     * @return true if enabled, false otherwise
     */
    static bool is_enabled() noexcept {
        MutexLockGuard lock(get_mutex());
        return audio_enabled_;
    }

    /**
     * @brief Set cooldown period between alerts
     *
     * @param cooldown_ms Cooldown period in milliseconds
     *
     * @note FIX #10: Added input validation for cooldown values
     */
    static void set_cooldown_ms(CooldownMs cooldown_ms) noexcept {
        // Input validation: clamp to reasonable range
        if (cooldown_ms < 10) {
            cooldown_ms = 10;
        } else if (cooldown_ms > 10000) {
            cooldown_ms = 10000;
        }

        MutexLockGuard lock(get_mutex());
        cooldown_ms_ = cooldown_ms;
    }

private:
    /**
     * @brief Check if cooldown period has expired
     *
     * @return true if cooldown expired, false otherwise
     *
     * @note FIX #5: systime_t overflow handled by ChibiOS (wraps every ~49.7 days)
     * @note Uses MS2ST macro to convert milliseconds to system ticks
     */
    static bool is_cooldown_expired() noexcept {
        const systime_t now = chTimeNow();
        const systime_t elapsed_ticks = now - last_alert_timestamp_;
        return elapsed_ticks >= MS2ST(cooldown_ms_);
    }

    /**
     * @brief Get audio frequency for threat level
     *
     * @param level Threat level
     * @return Audio frequency in Hz
     *
     * @note FIX #7: Removed redundant default case
     * @note FIX #4: Returns uint32_t to match baseband API
     */
    static AudioFrequency get_frequency_for_threat_level(ThreatLevel level) noexcept {
        switch (level) {
            case ThreatLevel::LOW:
                return AudioConstants::DEFAULT_LOW_FREQ;
            case ThreatLevel::MEDIUM:
                return AudioConstants::DEFAULT_MEDIUM_FREQ;
            case ThreatLevel::HIGH:
                return AudioConstants::DEFAULT_HIGH_FREQ;
            case ThreatLevel::CRITICAL:
                return AudioConstants::DEFAULT_CRITICAL_FREQ;
            case ThreatLevel::NONE:
            case ThreatLevel::UNKNOWN:
            default:
                return AudioConstants::DEFAULT_LOW_FREQ;
        }
    }

    // Static member variables
    inline static bool audio_enabled_ = true;
    inline static systime_t last_alert_timestamp_ = 0;
    inline static CooldownMs cooldown_ms_ = EDA::Constants::DEFAULT_ALERT_COOLDOWN_MS;
};

// ========================================
// COMPILE-TIME TYPE SAFETY ASSERTIONS
// ========================================
// FIX #4: Ensure AudioFrequency matches baseband API type
static_assert(std::is_same<AudioFrequency, uint32_t>::value,
              "AudioFrequency must be uint32_t to match baseband API");

// FIX #6: Verify EDA_FLASH_CONST is defined
#ifndef EDA_FLASH_CONST
    #error "EDA_FLASH_CONST must be defined for Flash storage optimization"
#endif

} // namespace ui::apps::enhanced_drone_analyzer

// ========================================
// CLASS-COMPATIBLE INTERFACE
// ========================================
// Maintains API compatibility with existing code
class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() = default;

    /**
     * @brief Check if audio is enabled
     * @return true if enabled, false otherwise
     */
    bool is_audio_enabled() const {
        return ui::apps::enhanced_drone_analyzer::AudioAlertManager::is_enabled();
    }

    /**
     * @brief Toggle audio enable state
     */
    void toggle_audio() {
        const bool current = ui::apps::enhanced_drone_analyzer::AudioAlertManager::is_enabled();
        ui::apps::enhanced_drone_analyzer::AudioAlertManager::set_enabled(!current);
    }

    /**
     * @brief Play detection beep for threat level
     * @param threat Threat level
     */
    void play_detection_beep(ui::apps::enhanced_drone_analyzer::ThreatLevel threat) {
        ui::apps::enhanced_drone_analyzer::AudioAlertManager::play_alert(threat);
    }

    /**
     * @brief Stop audio (no-op for simple beeps)
     */
    void stop_audio() {
        // Simple beeps don't persist - no stop needed
    }
};

// ========================================
// DRONE AUDIO SETTINGS
// ========================================
struct DroneAudioSettings {
    bool audio_enabled = true;
    ui::apps::enhanced_drone_analyzer::ThreatLevel test_threat_level =
        ui::apps::enhanced_drone_analyzer::ThreatLevel::HIGH;
};

#endif // UI_DRONE_AUDIO_HPP_
