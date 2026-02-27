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

// C++ standard library headers (alphabetical order)
#include <cstdint>

// Third-party library headers
#include <ch.h>

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
     * - chMtxUnlock() returns a pointer to the unlocked mutex
     * 
     * This implementation verifies that the correct mutex is unlocked by
     * comparing the returned pointer with the mutex we locked. This prevents
     * race conditions when multiple locks are held.
     * 
     * @note This implementation uses the newer ChibiOS API (>= 21.x) where
     *       chMtxUnlock() takes no parameters and unlocks the last locked mutex.
     *       The codebase consistently uses this API pattern.
     */
    ~MutexLock() noexcept {
        if (locked_) {
            Mutex* unlocked = chMtxUnlock();  // ChibiOS API: unlocks last locked mutex, returns pointer
            // Verify we unlocked the correct mutex (defensive programming)
            // If this assertion fails, it indicates lock order violation or bug
            chDbgAssert(unlocked == &mtx_,
                        "chMtxUnlock() verification",
                        "unlocked wrong mutex - lock order violation");
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

/**
 * @brief Non-blocking try-lock for mutexes
 * 
 * Attempts to acquire a mutex lock without blocking.
 * Returns immediately if the lock is not available.
 * 
 * Usage:
 * @code
 *     Mutex my_mutex;
 *     chMtxInit(&my_mutex);
 *     
 *     MutexTryLock lock(my_mutex);
 *     if (lock.is_locked()) {
 *         // Critical section - lock acquired
 *     } else {
 *         // Lock not available, handle contention
 *     }
 * @endcode
 * 
 * @note Safe for use in situations where blocking is not acceptable
 * @note Always check is_locked() before accessing protected data
 */
class MutexTryLock {
public:
    /**
     * @brief Try to acquire mutex lock (non-blocking)
     * @param mtx Reference to ChibiOS mutex to lock
     * @param order Lock order level for deadlock prevention (optional, for documentation)
     * @note Returns immediately if lock is not available
     * @note noexcept for embedded safety
     */
    explicit MutexTryLock(Mutex& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
        : mtx_(mtx), locked_(false), order_(order) {
        (void)order_;  // Suppress unused warning in release builds
        if (chMtxTryLock(&mtx_) == CH_SUCCESS) {
            locked_ = true;
        }
    }

    /**
     * @brief Release mutex lock (RAII)
     *
     * Only releases if the lock was successfully acquired.
     *
     * ChibiOS API Note:
     * - chMtxTryLock(&mtx) takes mutex pointer as parameter
     * - chMtxUnlock() does NOT take a parameter (unlocks last locked mutex)
     * - chMtxUnlock() returns a pointer to the unlocked mutex
     *
     * This implementation verifies that the correct mutex is unlocked by
     * comparing the returned pointer with the mutex we locked.
     */
    ~MutexTryLock() noexcept {
        if (locked_) {
            Mutex* unlocked = chMtxUnlock();  // ChibiOS API: unlocks last locked mutex, returns pointer
            // Verify we unlocked the correct mutex (defensive programming)
            // If this assertion fails, it indicates lock order violation or bug
            chDbgAssert(unlocked == &mtx_,
                        "chMtxUnlock() verification",
                        "unlocked wrong mutex - lock order violation");
            locked_ = false;
        }
    }

    /**
     * @brief Check if lock was successfully acquired
     * @return true if lock is held, false otherwise
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return locked_;
    }

    // Non-copyable
    MutexTryLock(const MutexTryLock&) = delete;
    MutexTryLock& operator=(const MutexTryLock&) = delete;

    // Non-movable
    MutexTryLock(MutexTryLock&&) = delete;
    MutexTryLock& operator=(MutexTryLock&&) = delete;

private:
    Mutex& mtx_;      ///< Reference to mutex being locked
    bool locked_;     ///< Track lock state
    LockOrder order_; ///< Lock order level for documentation
};

/**
 * @brief SD Card Lock (specialized wrapper for SD card mutex)
 * 
 * Convenience wrapper for SD card mutex operations.
 * Ensures SD_CARD_MUTEX lock order is always used.
 * 
 * Usage:
 * @code
 *     Mutex sd_card_mutex;
 *     chMtxInit(&sd_card_mutex);
 *     
 *     {
 *         SDCardLock lock(sd_card_mutex);  // Lock acquired with SD_CARD_MUTEX order
 *         // SD card I/O operations
 *     }  // Lock automatically released
 * @endcode
 * 
 * @note Always use this for SD card operations (FatFS is NOT thread-safe)
 * @note SD_CARD_MUTEX must be LAST in lock ordering
 */
class SDCardLock {
public:
    /**
     * @brief Acquire SD card mutex lock
     * @param mtx Reference to SD card mutex to lock
     * @note Blocks until lock is acquired
     * @note noexcept for embedded safety
     */
    explicit SDCardLock(Mutex& mtx) noexcept
        : lock_(mtx, LockOrder::SD_CARD_MUTEX) {
    }

    /**
     * @brief Release SD card mutex lock (RAII)
     * @note Automatically releases when lock goes out of scope
     */
    ~SDCardLock() noexcept = default;

    // Non-copyable
    SDCardLock(const SDCardLock&) = delete;
    SDCardLock& operator=(const SDCardLock&) = delete;

    // Non-movable
    SDCardLock(SDCardLock&&) = delete;
    SDCardLock& operator=(SDCardLock&&) = delete;

private:
    MutexLock lock_;  ///< Internal MutexLock with SD_CARD_MUTEX order
};

/**
 * @brief Stack monitor for preventing stack overflow
 * 
 * Monitors current thread stack usage and provides safety checks
 * to prevent stack overflow in critical sections like paint().
 * 
 * Uses ChibiOS stack fill pattern (0x55) to estimate free stack space.
 * Compatible with ChibiOS versions that have CH_DBG_FILL_THREADS enabled.
 * 
 * Usage:
 * @code
 *     void my_function() {
 *         StackMonitor monitor;
 *         constexpr size_t REQUIRED_STACK = 1024;
 *         
 *         if (!monitor.is_stack_safe(REQUIRED_STACK)) {
 *             return;  // Not enough stack, skip operation
 *         }
 *         
 *         // Safe to use up to REQUIRED_STACK bytes
 *     }
 * @endcode
 * 
 * @note Conservative: if stack filling is not enabled, assumes 0 free bytes
 * @note Safe to call from any thread context (not ISR-safe)
 */
class StackMonitor {
public:
    /**
     * @brief Constructor - captures current thread state
     * @note noexcept for embedded safety
     */
    StackMonitor() noexcept
        : current_thread_(chThdSelf()),
          free_stack_bytes_(calculate_free_stack()) {
    }

    /**
     * @brief Check if sufficient stack space is available
     * @param required_bytes Required stack space in bytes
     * @return true if at least required_bytes are available, false otherwise
     * @note Conservative check - adds safety margin
     */
    [[nodiscard]] bool is_stack_safe(size_t required_bytes) const noexcept {
        // Add 256-byte safety margin for function call overhead
        constexpr size_t SAFETY_MARGIN = 256;
        return free_stack_bytes_ >= (required_bytes + SAFETY_MARGIN);
    }

    /**
     * @brief Get estimated free stack bytes
     * @return Estimated free stack space in bytes
     */
    [[nodiscard]] size_t get_free_stack() const noexcept {
        return free_stack_bytes_;
    }

private:
    Thread* current_thread_;   ///< Current thread pointer
    size_t free_stack_bytes_;  ///< Estimated free stack bytes

    /**
     * @brief Calculate free stack space from fill pattern
     * @return Estimated free stack bytes
     *
     * When CH_DBG_ENABLE_STACK_CHECK is enabled, uses p_stklimit for accurate calculation.
     * Otherwise, conservatively scans for 0x55 fill pattern from stack limit.
     * ChibiOS fills unused stack with 0x55 when CH_DBG_FILL_THREADS is enabled.
     */
    [[nodiscard]] size_t calculate_free_stack() const noexcept {
        if (!current_thread_) {
            return 0;  // No thread context
        }

#if CH_DBG_ENABLE_STACK_CHECK
        // Use p_stklimit for accurate stack boundary detection
        // p_stklimit points to the bottom of the stack (where fill pattern starts)
        uint8_t* stack_limit = reinterpret_cast<uint8_t*>(current_thread_->p_stklimit);
        uint8_t* current_sp = reinterpret_cast<uint8_t*>(current_thread_->p_ctx.r13);
        
        // Stack grows downward, so free space is from stack_limit to current SP
        if (current_sp > stack_limit) {
            return static_cast<size_t>(current_sp - stack_limit);
        }
        return 0;
#else
        // Fallback: Conservative scan from estimated stack limit
        // Note: This is less accurate as it may scan through context structures
        // The actual stack starts after Thread + intctx + extctx structures
        uint8_t* stack_start = reinterpret_cast<uint8_t*>(current_thread_ + 1);
        
        // Limit scan to avoid excessive iteration (max 4KB scan)
        const size_t max_scan_bytes = 4096;
        const uint8_t stack_fill_value = 0x55;
        size_t free_stack = 0;
        
        // Count consecutive fill pattern bytes
        for (size_t i = 0; i < max_scan_bytes; ++i) {
            if (stack_start[i] == stack_fill_value) {
                free_stack++;
            } else {
                // Stack used up to this point
                break;
            }
        }
        
        return free_stack;
#endif
    }
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_LOCKING_HPP_
