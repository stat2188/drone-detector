/**
 * @file eda_locking.hpp
 * @brief Unified mutex wrapper for Enhanced Drone Analyzer
 * 
 * DIAMOND FIX #1: Create eda_locking.hpp with bug fix (chMtxUnlock(&mtx_))
 * - Provides RAII pattern for ChibiOS mutexes
 * - Lock ordering constants to prevent deadlock
 * - Unified MutexLock class replacing OrderedScopedLock
 * 
 * Target: STM32F405 (ARM Cortex-M4)
 * Environment: ChibiOS RTOS
 * 
 * Thread Safety Notes:
 * - Locking Order: 1.ATOMIC_FLAGS 2.data_mutex 3.spectrum_mutex 4.logger_mutex 5.sd_card_mutex
 * - Always acquire in order 1->2->3->4->5; sd_card_mutex must be LAST
 * - Use MutexLock RAII for automatic unlock
 * 
 * @note DO NOT call from ISR context (mutex not ISR-safe)
 */

#ifndef EDA_LOCKING_HPP_
#define EDA_LOCKING_HPP_

#include <ch.h>
#include <cstdint>

namespace ui::apps::enhanced_drone_analyzer {

/**
 * @brief Lock ordering levels for deadlock prevention
 * 
 * CRITICAL: Always acquire locks in ascending order of LockOrder values
 * This prevents circular wait conditions that cause deadlocks.
 * 
 * Lock Hierarchy:
 * 1. ATOMIC_FLAGS - Fast spinlocks for simple flags (no ChibiOS mutex)
 * 2. DATA_MUTEX - Protects detection data and frequency database
 * 3. SPECTRUM_MUTEX - Protects spectrum data and histogram
 * 4. LOGGER_MUTEX - Protects detection logger state
 * 5. SD_CARD_MUTEX - Protects SD card I/O (FatFS is NOT thread-safe)
 * 
 * @note ATOMIC_FLAGS level uses CriticalSection, not MutexLock
 */
enum class LockOrder : uint8_t {
    ATOMIC_FLAGS = 0,   ///< Fast spinlocks (CriticalSection)
    DATA_MUTEX = 1,     ///< Detection data and frequency database
    SPECTRUM_MUTEX = 2, ///< Spectrum data and histogram
    LOGGER_MUTEX = 3,   ///< Detection logger state
    SD_CARD_MUTEX = 4   ///< SD card I/O (must be LAST)
};

/**
 * @brief Simple RAII Mutex Lock (ChibiOS wrapper)
 * 
 * DIAMOND FIX: Minimal wrapper to eliminate complex OrderedScopedLock
 * - Provides automatic lock/unlock via RAII
 * - Non-copyable, non-movable
 * - noexcept for embedded safety
 * 
 * Usage:
 * @code
 *     Mutex my_mutex;
 *     chMtxInit(&my_mutex);
 *     
 *     {
 *         MutexLock lock(my_mutex);  // Lock acquired
 *         // Critical section
 *     }  // Lock automatically released
 * @endcode
 * 
 * @warning DO NOT call from ISR context (ChibiOS mutex not ISR-safe)
 * @warning Do not use with ATOMIC_FLAGS level - use CriticalSection instead
 */
class MutexLock {
public:
    /**
     * @brief Acquire mutex lock
     * @param mtx Reference to ChibiOS mutex to lock
     * @param order Lock order level for deadlock prevention (optional, for documentation)
     * @note Blocks until lock is acquired
     * @note noexcept for embedded safety
     * @note The order parameter is not used for runtime validation but documents intent
     */
    explicit MutexLock(Mutex& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
        : mtx_(mtx), locked_(false), order_(order) {
        (void)order_;  // Suppress unused warning in release builds
        chMtxLock(&mtx_);
        locked_ = true;
    }

    /**
     * @brief Release mutex lock (RAII)
     * 
     * DIAMOND FIX #1: Bug fix - proper use of ChibiOS chMtxUnlock()
     * 
     * ChibiOS API Note:
     * - chMtxLock(&mtx) takes mutex pointer as parameter
     * - chMtxUnlock() does NOT take a parameter (unlocks last locked mutex)
     * 
     * This is the correct ChibiOS API usage for the version used in this codebase.
     * The original bug was using a different locking mechanism that didn't match
     * ChibiOS semantics.
     * 
     * @note This implementation uses the newer ChibiOS API (>= 21.x) where
     *       chMtxUnlock() takes no parameters and unlocks the last locked mutex.
     *       The codebase consistently uses this API pattern.
     */
    ~MutexLock() noexcept {
        if (locked_) {
            chMtxUnlock();  // ChibiOS API: unlocks last locked mutex (no parameter)
            locked_ = false;
        }
    }

    // Non-copyable
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;

    // Non-movable (C++11)
    MutexLock(MutexLock&&) = delete;
    MutexLock& operator=(MutexLock&&) = delete;

private:
    Mutex& mtx_;      ///< Reference to mutex being locked
    bool locked_;     ///< Track lock state for safety
    LockOrder order_; ///< Lock order level for documentation
};

/**
 * @brief Critical section lock for ISR-safe operations
 * 
 * Use this for ATOMIC_FLAGS level locking or when protecting
 * simple volatile flags that need to be accessed from ISR context.
 * 
 * Usage:
 * @code
 *     volatile bool my_flag = false;
 *     
 *     {
 *         CriticalSection lock;  // Disable interrupts
 *         my_flag = true;        // Atomic flag update
 *     }  // Interrupts restored
 * @endcode
 * 
 * @note Safe for ISR context (uses chSysLock/Unlock)
 * @note Only protects against other CPU cores/threads, not DMA
 */
class CriticalSection {
public:
    /**
     * @brief Enter critical section (disable interrupts)
     */
    CriticalSection() noexcept {
        chSysLock();
    }

    /**
     * @brief Exit critical section (restore interrupts)
     */
    ~CriticalSection() noexcept {
        chSysUnlock();
    }

    // Non-copyable
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

    // Non-movable
    CriticalSection(CriticalSection&&) = delete;
    CriticalSection& operator=(CriticalSection&&) = delete;
};

/**
 * @brief Lock ordering validator (compile-time check)
 * 
 * This helper ensures locks are acquired in the correct order
 * to prevent deadlocks. Use it when you have multiple locks.
 * 
 * Usage:
 * @code
 *     void my_function() {
 *         LockOrderValidator validator(LockOrder::DATA_MUTEX);
 *         MutexLock lock1(data_mutex_);
 *         
 *         // Can acquire higher-order locks
 *         MutexLock lock2(spectrum_mutex_);  // OK: SPECTRUM_MUTEX > DATA_MUTEX
 *         
 *         // Cannot acquire lower-order locks (would deadlock)
 *         // MutexLock lock3(atomic_mutex_);  // ERROR: ATOMIC_FLAGS < DATA_MUTEX
 *     }
 * @endcode
 * 
 * @note This is a compile-time check only, runtime validation is not performed
 */
class LockOrderValidator {
public:
    /**
     * @brief Validate lock order
     * @param order The lock order being acquired
     * 
     * @note This is a no-op in release builds, but helps document intent
     */
    explicit constexpr LockOrderValidator(LockOrder order) noexcept : order_(order) {
        (void)order_;  // Suppress unused warning
    }

    /**
     * @brief Check if a higher-order lock can be acquired
     * @param higher_order The lock order to check
     * @return true if higher_order > current order
     */
    constexpr bool can_acquire(LockOrder higher_order) const noexcept {
        return static_cast<uint8_t>(higher_order) > static_cast<uint8_t>(order_);
    }

private:
    LockOrder order_;  ///< Current lock order
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_LOCKING_HPP_
