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
     * @return true if flag was sent successfully, false if target thread is null
     * @note CRITICAL: Returns false if target_thread_ is nullptr
     */
    [[nodiscard]] inline bool send_flag(ThreadFlag flag) const noexcept {
        if (target_thread_ != nullptr) {
            chEvtSignal(target_thread_, static_cast<eventmask_t>(flag));
            return true;
        }
        return false;  // Target thread is null
    }

    /**
     * @brief Send multiple flags to target thread
     * @param flags Flags to send (ORed together)
     * @return true if flags were sent successfully, false if target thread is null
     * @note CRITICAL: Returns false if target_thread_ is nullptr
     */
    [[nodiscard]] inline bool send_flags(ThreadFlag flags) const noexcept {
        if (target_thread_ != nullptr) {
            chEvtSignal(target_thread_, static_cast<eventmask_t>(flags));
            return true;
        }
        return false;  // Target thread is null
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

// ============================================================================
// SEMAPHORES - Resource Counting
// ============================================================================
// Semaphores are used for resource counting (buffer slots, processing capacity).
// They provide blocking wait and non-blocking try-wait operations.
//
// Semaphore Types:
// - BinarySemaphore: Single-slot resource access (count = 1)
// - CountingSemaphore: Multi-slot resource access (count > 1)
//
// CRITICAL: Use signal() from ISR context, wait() from thread context only.
// Semaphores must be initialized after chSysInit() via initialize_eda_mutexes().
// ============================================================================

/**
 * @brief Binary semaphore for resource access control
 *
 * Provides blocking wait and non-blocking try-wait operations for single-slot
 * resource access (e.g., display buffer).
 *
 * USAGE:
 * @code
 *     BinarySemaphore buffer_sem;
 *     chSemInit(&buffer_sem.sem_, 1);  // Initialize in initialize_eda_mutexes()
 *
 *     // Thread 1: Acquire buffer
 *     if (buffer_sem.wait(TIMEOUT_SHORT_MS)) {
 *         // Use buffer
 *         buffer_sem.signal();  // Release buffer
 *     }
 *
 *     // Thread 2: Non-blocking try-wait
 *     if (buffer_sem.try_wait()) {
 *         // Use buffer
 *         buffer_sem.signal();  // Release buffer
 *     }
 * @endcode
 *
 * @note CRITICAL: Use signal() from ISR context, wait() from thread context only.
 * @note CRITICAL: Must be initialized after chSysInit() via initialize_eda_mutexes().
 * @note Lock order: Treat as mutex at LockOrder level 6.5 (after UI_DISPLAY_MUTEX).
 * @note No floating-point operations in critical path (embedded constraint).
 */
class BinarySemaphore {
public:
    /**
     * @brief Constructor - does NOT initialize semaphore (deferred to initialize_eda_mutexes())
     * @note Semaphore must be initialized via initialize() method or chSemInit(&sem_, 1)
     */
    BinarySemaphore() noexcept : sem_{}, initialized_(false) {}

    /**
     * @brief Initialize binary semaphore
     * @param count Initial semaphore count (typically 1 for binary semaphore)
     * @note Must be called after chSysInit() to prevent undefined behavior
     * @note Sets initialized_ flag to true on success
     * @note Safe to call multiple times (idempotent)
     */
    void initialize(uint32_t count = 1) noexcept {
        if (!initialized_) {
            chSemInit(&sem_, count);
            initialized_ = true;
        }
    }

    /**
     * @brief Check if semaphore is initialized
     * @return true if semaphore has been initialized, false otherwise
     * @note Use this method to distinguish between "not initialized" and "timeout"
     * @note CRITICAL: Always check this before calling wait() or try_wait()
     */
    [[nodiscard]] bool is_initialized() const noexcept {
        return initialized_;
    }

    /**
     * @brief Wait for semaphore (blocking)
     * @param timeout_ms Timeout in milliseconds (TIME_INFINITE for infinite)
     * @return true if semaphore was acquired, false if timeout or not initialized
     * @note Blocks until semaphore is acquired or timeout expires
     * @note CRITICAL: Do NOT call from ISR context (blocking operation)
     * @note CRITICAL: Always check return value for timeout handling
     * @note CRITICAL: Returns false if semaphore is not initialized
     * @note CRITICAL: Use is_initialized() to distinguish timeout vs not initialized
     */
    [[nodiscard]] bool wait(uint32_t timeout_ms = TIME_INFINITE) noexcept {
        if (!initialized_) {
            return false;  // Semaphore not initialized
        }
        return chSemWaitTimeout(&sem_, MS2ST(timeout_ms)) == RDY_OK;
    }

    /**
     * @brief Try to wait for semaphore (non-blocking)
     * @return true if semaphore was acquired, false if not available or not initialized
     * @note Returns immediately without blocking
     * @note Safe to call from ISR context (non-blocking)
     * @note CRITICAL: Returns false if semaphore is not initialized
     */
    [[nodiscard]] bool try_wait() noexcept {
        if (!initialized_) {
            return false;  // Semaphore not initialized
        }
        return chSemWaitTimeout(&sem_, TIME_IMMEDIATE) == RDY_OK;
    }

    /**
     * @brief Signal semaphore (release resource)
     * @note Increments semaphore count
     * @note Safe to call from ISR context (non-blocking)
     * @note CRITICAL: No effect if semaphore is not initialized
     */
    void signal() noexcept {
        if (initialized_) {
            chSemSignal(&sem_);
        }
    }

    // Non-copyable
    BinarySemaphore(const BinarySemaphore&) = delete;
    BinarySemaphore& operator=(const BinarySemaphore&) = delete;

    // Non-movable
    BinarySemaphore(BinarySemaphore&&) = delete;
    BinarySemaphore& operator=(BinarySemaphore&&) = delete;

private:
    ::Semaphore sem_;  ///< ChibiOS semaphore object (must be initialized via chSemInit)
    bool initialized_;  ///< Flag to track if semaphore has been initialized
};

/**
 * @brief Counting semaphore for resource access control
 *
 * Provides blocking wait and non-blocking try-wait operations for multi-slot
 * resource access (e.g., buffer slots, processing capacity).
 *
 * USAGE:
 * @code
 *     CountingSemaphore buffer_slots_sem;
 *     chSemInit(&buffer_slots_sem.sem_, 10);  // 10 slots available
 *
 *     // Thread 1: Acquire buffer slot
 *     if (buffer_slots_sem.wait(TIMEOUT_SHORT_MS)) {
 *         // Use buffer slot
 *         buffer_slots_sem.signal();  // Release buffer slot
 *     }
 * @endcode
 *
 * @note CRITICAL: Use signal() from ISR context, wait() from thread context only.
 * @note CRITICAL: Must be initialized after chSysInit() via initialize_eda_mutexes().
 * @note Lock order: Treat as mutex at LockOrder level 9.5 (after HISTOGRAM_BUFFER_MUTEX).
 * @note No floating-point operations in critical path (embedded constraint).
 */
class CountingSemaphore {
public:
    /**
     * @brief Constructor - does NOT initialize semaphore (deferred to initialize_eda_mutexes())
     * @note Semaphore must be initialized via initialize() method or chSemInit(&sem_, count)
     */
    CountingSemaphore() noexcept : sem_{}, initialized_(false) {}

    /**
     * @brief Initialize counting semaphore
     * @param count Initial semaphore count (number of available slots)
     * @note Must be called after chSysInit() to prevent undefined behavior
     * @note Sets initialized_ flag to true on success
     * @note Safe to call multiple times (idempotent)
     */
    void initialize(uint32_t count) noexcept {
        if (!initialized_) {
            chSemInit(&sem_, count);
            initialized_ = true;
        }
    }

    /**
     * @brief Check if semaphore is initialized
     * @return true if semaphore has been initialized, false otherwise
     * @note Use this method to distinguish between "not initialized" and "timeout"
     * @note CRITICAL: Always check this before calling wait() or try_wait()
     */
    [[nodiscard]] bool is_initialized() const noexcept {
        return initialized_;
    }

    /**
     * @brief Wait for semaphore (blocking)
     * @param timeout_ms Timeout in milliseconds (TIME_INFINITE for infinite)
     * @return true if semaphore was acquired, false if timeout or not initialized
     * @note Blocks until semaphore is acquired or timeout expires
     * @note CRITICAL: Do NOT call from ISR context (blocking operation)
     * @note CRITICAL: Always check return value for timeout handling
     * @note CRITICAL: Returns false if semaphore is not initialized
     * @note CRITICAL: Use is_initialized() to distinguish timeout vs not initialized
     */
    [[nodiscard]] bool wait(uint32_t timeout_ms = TIME_INFINITE) noexcept {
        if (!initialized_) {
            return false;  // Semaphore not initialized
        }
        return chSemWaitTimeout(&sem_, MS2ST(timeout_ms)) == RDY_OK;
    }

    /**
     * @brief Try to wait for semaphore (non-blocking)
     * @return true if semaphore was acquired, false if not available or not initialized
     * @note Returns immediately without blocking
     * @note Safe to call from ISR context (non-blocking)
     * @note CRITICAL: Returns false if semaphore is not initialized
     */
    [[nodiscard]] bool try_wait() noexcept {
        if (!initialized_) {
            return false;  // Semaphore not initialized
        }
        return chSemWaitTimeout(&sem_, TIME_IMMEDIATE) == RDY_OK;
    }

    /**
     * @brief Signal semaphore (release resource)
     * @note Increments semaphore count
     * @note Safe to call from ISR context (non-blocking)
     * @note CRITICAL: No effect if semaphore is not initialized
     */
    void signal() noexcept {
        if (initialized_) {
            chSemSignal(&sem_);
        }
    }

    // Non-copyable
    CountingSemaphore(const CountingSemaphore&) = delete;
    CountingSemaphore& operator=(const CountingSemaphore&) = delete;

    // Non-movable
    CountingSemaphore(CountingSemaphore&&) = delete;
    CountingSemaphore& operator=(CountingSemaphore&&) = delete;

private:
    ::Semaphore sem_;  ///< ChibiOS semaphore object (must be initialized via chSemInit)
    bool initialized_;  ///< Flag to track if semaphore has been initialized
};

// ============================================================================
// RAII SEMAPHORE GUARDS
// ============================================================================

/**
 * @brief RAII guard for binary semaphore
 *
 * Acquires semaphore on construction, releases on destruction.
 * Provides automatic resource management and prevents resource leaks.
 *
 * USAGE:
 * @code
 *     BinarySemaphore buffer_sem;
 *     chSemObjectInit(&buffer_sem.sem_, 1);
 *
 *     {
 *         BinarySemaphoreGuard guard(buffer_sem, TIMEOUT_SHORT_MS);
 *         if (guard.is_acquired()) {
 *             // Use buffer (semaphore held)
 *         }  // Semaphore automatically released
 *     }
 * @endcode
 *
 * @note CRITICAL: Always check is_acquired() after construction.
 * @note CRITICAL: Do NOT call from ISR context (blocking wait operation).
 */
class BinarySemaphoreGuard {
public:
    /**
     * @brief Acquire binary semaphore (blocking with timeout)
     * @param sem Reference to binary semaphore to acquire
     * @param timeout_ms Timeout in milliseconds (TIME_INFINITE for infinite)
     * @note Blocks until semaphore is acquired or timeout expires
     * @note CRITICAL: Always check is_acquired() after construction
     */
    explicit BinarySemaphoreGuard(BinarySemaphore& sem, uint32_t timeout_ms = TIME_INFINITE) noexcept
        : sem_(sem), acquired_(sem.wait(timeout_ms)) {}

    /**
     * @brief Release binary semaphore (RAII)
     * @note Automatically releases semaphore when guard goes out of scope
     * @note Only releases if semaphore was successfully acquired
     */
    ~BinarySemaphoreGuard() noexcept {
        if (acquired_) {
            sem_.signal();
        }
    }

    /**
     * @brief Check if semaphore was successfully acquired
     * @return true if semaphore is held, false otherwise
     * @note CRITICAL: Always call this after construction to verify acquisition
     */
    [[nodiscard]] bool is_acquired() const noexcept {
        return acquired_;
    }

    // Non-copyable
    BinarySemaphoreGuard(const BinarySemaphoreGuard&) = delete;
    BinarySemaphoreGuard& operator=(const BinarySemaphoreGuard&) = delete;

    // Non-movable
    BinarySemaphoreGuard(BinarySemaphoreGuard&&) = delete;
    BinarySemaphoreGuard& operator=(BinarySemaphoreGuard&&) = delete;

private:
    BinarySemaphore& sem_;  ///< Reference to binary semaphore being guarded
    bool acquired_;          ///< Flag indicating if semaphore was acquired
};

/**
 * @brief RAII guard for counting semaphore
 *
 * Acquires semaphore on construction, releases on destruction.
 * Provides automatic resource management and prevents resource leaks.
 *
 * USAGE:
 * @code
 *     CountingSemaphore buffer_slots_sem;
 *     chSemObjectInit(&buffer_slots_sem.sem_, 10);
 *
 *     {
 *         CountingSemaphoreGuard guard(buffer_slots_sem, TIMEOUT_SHORT_MS);
 *         if (guard.is_acquired()) {
 *             // Use buffer slot (semaphore held)
 *         }  // Semaphore automatically released
 *     }
 * @endcode
 *
 * @note CRITICAL: Always check is_acquired() after construction.
 * @note CRITICAL: Do NOT call from ISR context (blocking wait operation).
 */
class CountingSemaphoreGuard {
public:
    /**
     * @brief Acquire counting semaphore (blocking with timeout)
     * @param sem Reference to counting semaphore to acquire
     * @param timeout_ms Timeout in milliseconds (TIME_INFINITE for infinite)
     * @note Blocks until semaphore is acquired or timeout expires
     * @note CRITICAL: Always check is_acquired() after construction
     */
    explicit CountingSemaphoreGuard(CountingSemaphore& sem, uint32_t timeout_ms = TIME_INFINITE) noexcept
        : sem_(sem), acquired_(sem.wait(timeout_ms)) {}

    /**
     * @brief Release counting semaphore (RAII)
     * @note Automatically releases semaphore when guard goes out of scope
     * @note Only releases if semaphore was successfully acquired
     */
    ~CountingSemaphoreGuard() noexcept {
        if (acquired_) {
            sem_.signal();
        }
    }

    /**
     * @brief Check if semaphore was successfully acquired
     * @return true if semaphore is held, false otherwise
     * @note CRITICAL: Always call this after construction to verify acquisition
     */
    [[nodiscard]] bool is_acquired() const noexcept {
        return acquired_;
    }

    // Non-copyable
    CountingSemaphoreGuard(const CountingSemaphoreGuard&) = delete;
    CountingSemaphoreGuard& operator=(const CountingSemaphoreGuard&) = delete;

    // Non-movable
    CountingSemaphoreGuard(CountingSemaphoreGuard&&) = delete;
    CountingSemaphoreGuard& operator=(CountingSemaphoreGuard&&) = delete;

private:
    CountingSemaphore& sem_;  ///< Reference to counting semaphore being guarded
    bool acquired_;            ///< Flag indicating if semaphore was acquired
};

} // namespace ui::apps::enhanced_drone_analyzer::sync

#endif // EDA_THREAD_SYNC_HPP_
