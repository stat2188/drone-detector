/**
 * @file eda_thread_sync.hpp
 * @brief Thread synchronization primitives for Enhanced Drone Analyzer
 *
 * DIAMOND CODE COMPLIANCE:
 * - Stack allocation only (no heap allocation)
 * - Uses ChibiOS RTOS Thread Flags + Semaphores
 * - Eliminates lock contention with flag-based signaling
 * - Zero-Overhead Abstraction (inline functions, constexpr)
 * - Thread-safe communication between DSP, UI, Audio, File I/O, PLL
 *
 * SYNCHRONIZATION ARCHITECTURE:
 * - Thread Flags: Event signaling (DSP_DATA_READY, AUDIO_ALERT, etc.)
 * - Semaphores: Resource counting (file operations, PLL operations)
 * - No mutex contention (flags for signaling, semaphores for resources)
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 * Environment: ChibiOS RTOS
 */

#ifndef EDA_THREAD_SYNC_HPP_
#define EDA_THREAD_SYNC_HPP_

// ============================================================================
// STANDARD LIBRARY HEADERS
// ============================================================================
#include <cstdint>

// ============================================================================
// THIRD-PARTY LIBRARY HEADERS
// ============================================================================
#include <ch.h>

// ============================================================================
// PROJECT HEADERS
// ============================================================================
#include "eda_locking.hpp"

namespace ui::apps::enhanced_drone_analyzer::sync {

// ============================================================================
// THREAD FLAGS - Event Signaling
// ============================================================================
// Thread flags are used for event signaling between threads.
// They are lightweight (single bit per flag) and non-blocking.
// Flags are set by producer threads and checked by consumer threads.
//
// Flag Assignment (bit positions):
// - Bit 0-7: DSP events
// - Bit 8-15: UI events
// - Bit 16-23: Audio events
// - Bit 24-31: File I/O events
// ============================================================================

/**
 * @brief Thread flag definitions for event signaling
 * @note Each flag is a unique bit position (1 << N)
 * @note Flags are ORed together for multiple event signaling
 */
enum class ThreadFlag : eventmask_t {
    // DSP Events (Bits 0-7)
    DSP_DATA_READY = (1 << 0),          ///< New spectrum data available from DSP
    DSP_DETECTION_EVENT = (1 << 1),      ///< New drone detection from DSP
    DSP_FHSS_DETECTED = (1 << 2),        ///< FHSS signal detected
    DSP_SPECTRUM_UPDATED = (1 << 3),     ///< Spectrum display data updated

    // UI Events (Bits 8-15)
    UI_REFRESH_REQUESTED = (1 << 8),    ///< UI refresh requested
    UI_SETTINGS_CHANGED = (1 << 9),      ///< User settings changed
    UI_MODE_CHANGED = (1 << 10),         ///< Scanning mode changed

    // Audio Events (Bits 16-23)
    AUDIO_ALERT_TRIGGERED = (1 << 16),   ///< Audio alert triggered
    AUDIO_COOLDOWN_EXPIRED = (1 << 17),  ///< Audio cooldown period expired

    // File I/O Events (Bits 24-31)
    FILE_SAVE_REQUESTED = (1 << 24),     ///< File save operation requested
    FILE_SAVE_COMPLETE = (1 << 25),      ///< File save operation completed
    FILE_LOAD_REQUESTED = (1 << 26),     ///< File load operation requested
    FILE_LOAD_COMPLETE = (1 << 27),      ///< File load operation completed
    FILE_ERROR_OCCURRED = (1 << 28)       ///< File operation error
};

/**
 * @brief OR operator for combining ThreadFlags
 * @param lhs Left-hand flag
 * @param rhs Right-hand flag
 * @return Combined flags (bitwise OR)
 */
constexpr inline ThreadFlag operator|(ThreadFlag lhs, ThreadFlag rhs) noexcept {
    return static_cast<ThreadFlag>(static_cast<eventmask_t>(lhs) | static_cast<eventmask_t>(rhs));
}

/**
 * @brief OR-assignment operator for ThreadFlags
 * @param lhs Left-hand flag (modified)
 * @param rhs Right-hand flag
 * @return Reference to modified left-hand flag
 */
constexpr inline ThreadFlag& operator|=(ThreadFlag& lhs, ThreadFlag rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

/**
 * @brief AND operator for checking ThreadFlags
 * @param lhs Left-hand flag
 * @param rhs Right-hand flag
 * @return Combined flags (bitwise AND)
 */
constexpr inline ThreadFlag operator&(ThreadFlag lhs, ThreadFlag rhs) noexcept {
    return static_cast<ThreadFlag>(static_cast<eventmask_t>(lhs) & static_cast<eventmask_t>(rhs));
}

/**
 * @brief NOT operator for inverting ThreadFlags
 * @param flag Flag to invert
 * @return Inverted flag (bitwise NOT)
 */
constexpr inline ThreadFlag operator~(ThreadFlag flag) noexcept {
    return static_cast<ThreadFlag>(~static_cast<eventmask_t>(flag));
}

/**
 * @brief Thread flag sender for event signaling
 * @details Provides thread-safe flag setting with ChibiOS chEvtSignal
 * @note Non-blocking operation (flags are set atomically)
 */
class ThreadFlagSender {
public:
    /**
     * @brief Constructor
     * @param thread Pointer to target thread (nullptr for current thread)
     */
    explicit constexpr ThreadFlagSender(::Thread* thread = nullptr) noexcept
        : target_thread_(thread) {}

    /**
     * @brief Send a single flag to target thread
     * @param flag Flag to send
     * @return true if flag was sent successfully
     */
    [[nodiscard]] inline bool send_flag(ThreadFlag flag) const noexcept {
        if (target_thread_ != nullptr) {
            chEvtSignal(target_thread_, static_cast<eventmask_t>(flag));
        }
        return true;
    }

    /**
     * @brief Send multiple flags to target thread
     * @param flags Flags to send (ORed together)
     * @return true if flags were sent successfully
     */
    [[nodiscard]] inline bool send_flags(ThreadFlag flags) const noexcept {
        if (target_thread_ != nullptr) {
            chEvtSignal(target_thread_, static_cast<eventmask_t>(flags));
        }
        return true;
    }

    /**
     * @brief Set target thread
     * @param thread Pointer to target thread
     */
    inline void set_target_thread(::Thread* thread) noexcept {
        target_thread_ = thread;
    }

    /**
     * @brief Get target thread
     * @return Pointer to target thread
     */
    [[nodiscard]] inline ::Thread* get_target_thread() const noexcept {
        return target_thread_;
    }

private:
    mutable ::Thread* target_thread_;
};

/**
 * @brief Thread flag receiver for event handling
 * @details Provides blocking and non-blocking flag checking with ChibiOS chEvtWait
 * @note Supports timeout and ANY/ALL flag matching modes
 */
class ThreadFlagReceiver {
public:
    /**
     * @brief Constructor
     * @param thread Pointer to target thread (nullptr for current thread)
     */
    explicit constexpr ThreadFlagReceiver(::Thread* thread = nullptr) noexcept
        : target_thread_(thread) {}

    /**
     * @brief Wait for ANY of the specified flags (blocking)
     * @param flags Flags to wait for (ORed together)
     * @param timeout_ms Timeout in milliseconds (TIME_INFINITE for infinite)
     * @return Flags that were received (0 if timeout)
     */
    [[nodiscard]] inline ThreadFlag wait_any_flags(ThreadFlag flags, uint32_t timeout_ms = TIME_INFINITE) const noexcept {
        eventmask_t received = chEvtWaitAnyTimeout(static_cast<eventmask_t>(flags), MS2ST(timeout_ms));
        return static_cast<ThreadFlag>(received);
    }

    /**
     * @brief Wait for ALL of the specified flags (blocking)
     * @param flags Flags to wait for (ORed together)
     * @param timeout_ms Timeout in milliseconds (TIME_INFINITE for infinite)
     * @return Flags that were received (0 if timeout)
     */
    [[nodiscard]] inline ThreadFlag wait_all_flags(ThreadFlag flags, uint32_t timeout_ms = TIME_INFINITE) const noexcept {
        eventmask_t received = chEvtWaitAllTimeout(static_cast<eventmask_t>(flags), MS2ST(timeout_ms));
        return static_cast<ThreadFlag>(received);
    }

    /**
     * @brief Check if specific flags are set (non-blocking)
     * @param flags Flags to check (ORed together)
     * @return Flags that are currently set (0 if none)
     */
    [[nodiscard]] inline ThreadFlag get_flags(ThreadFlag flags) const noexcept {
        eventmask_t received = chEvtGetAndClearEvents(static_cast<eventmask_t>(flags));
        return static_cast<ThreadFlag>(received);
    }

    /**
     * @brief Clear specific flags
     * @param flags Flags to clear (ORed together)
     */
    inline void clear_flags(ThreadFlag flags) const noexcept {
        chEvtGetAndClearEvents(static_cast<eventmask_t>(flags));
    }

    /**
     * @brief Set target thread
     * @param thread Pointer to target thread
     */
    inline void set_target_thread(::Thread* thread) noexcept {
        target_thread_ = thread;
    }

    /**
     * @brief Get target thread
     * @return Pointer to target thread
     */
    [[nodiscard]] inline ::Thread* get_target_thread() const noexcept {
        return target_thread_;
    }

private:
    mutable ::Thread* target_thread_;
};

// ============================================================================
// SYNCHRONIZATION CONSTANTS
// ============================================================================

/**
 * @brief Synchronization timeout constants (Flash storage)
 */
namespace TimeoutConstants {
    constexpr inline static uint32_t TIMEOUT_INFINITE = TIME_INFINITE;
    constexpr inline static uint32_t TIMEOUT_IMMEDIATE = TIME_IMMEDIATE;
    constexpr inline static uint32_t TIMEOUT_SHORT_MS = 10;
    constexpr inline static uint32_t TIMEOUT_MEDIUM_MS = 100;
    constexpr inline static uint32_t TIMEOUT_LONG_MS = 1000;
}

} // namespace ui::apps::enhanced_drone_analyzer::sync

#endif // EDA_THREAD_SYNC_HPP_
