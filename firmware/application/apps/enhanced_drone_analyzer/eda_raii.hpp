/**
 * @file eda_raii.hpp
 * @brief RAII Wrappers for Enhanced Drone Analyzer
 *
 * DIAMOND CODE STANDARDS:
 * - Zero-heap allocation (no new, malloc, std::vector, std::string)
 * - RAII wrappers for automatic resource management
 * - noexcept for exception-free operation
 * - Guard clauses for early error returns
 * - Doxygen comments for public APIs
 *
 * @target STM32F405 (ARM Cortex-M4, 128KB RAM)
 * @os ChibiOS (bare-metal RTOS)
 */

#ifndef EDA_RAI_HPP_
#define EDA_RAI_HPP_

#include <cstdint>
#include <atomic>
#include <ch.h>
#include <chtypes.h>
#include <chthreads.h>
#include "eda_constants.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Forward declaration of LockOrder (defined in eda_locking.hpp)
// Using uint8_t directly to avoid circular dependency
enum class LockOrder : uint8_t {
    ATOMIC_FLAGS = 1,
    DATA_MUTEX = 2,
    SPECTRUM_MUTEX = 3,
    LOGGER_MUTEX = 4,
    SD_CARD_MUTEX = 5,
    SETTINGS_MUTEX = 6
};

// ========================================
// SCOPED LOCK (RAII Mutex Wrapper)
// ========================================

/**
 * @brief RAII lock wrapper for automatic mutex management
 *
 * Acquires lock on construction, releases on destruction.
 * Prevents deadlocks through lock order tracking.
 *
 * @tparam MutexType The mutex type (typically Mutex)
 * @tparam TryLock If true, uses non-blocking try-lock
 *
 * USAGE:
 *   ScopedLock<Mutex, false> lock(data_mutex, LockOrder::DATA_MUTEX);
 *   // Critical section here
 *   // Lock automatically released when 'lock' goes out of scope
 *
 * @note Non-copyable, non-movable (RAII requirement)
 * @note Zero-overhead abstraction (optimizes to direct chMtxLock/chMtxUnlock)
 */
template<typename MutexType, bool TryLock = false>
class ScopedLock {
public:
    /**
     * @brief Constructor - Acquires lock with order tracking
     * @param mtx Reference to mutex to lock
     * @param order Lock order level (must be higher than any held lock)
     */
    explicit ScopedLock(MutexType& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
        : mtx_(mtx), order_(order), locked_(false) {
        
        // Guard clause: Check lock order before attempting acquisition
        if (order_ <= current_max_order_) {
            return;  // Order violation - skip acquisition
        }
        
        // Acquire lock
        if constexpr (TryLock) {
            locked_ = (chMtxTryLock(&mtx_) == true);
        } else {
            chMtxLock(&mtx_);
            locked_ = true;
        }
        
        // Update current lock order if lock was acquired
        if (locked_) {
            current_max_order_ = order_;
        }
    }

    /// @brief Destructor - Releases lock and updates order tracking
    ~ScopedLock() noexcept {
        if (locked_) {
            chMtxUnlock(&mtx_);
            current_max_order_ = LockOrder::ATOMIC_FLAGS;
        }
    }

    /// @brief Query if lock was successfully acquired
    bool is_locked() const noexcept { return locked_; }

    /// @brief Non-copyable, non-movable (RAII requirement)
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
    ScopedLock(ScopedLock&&) = delete;
    ScopedLock& operator=(ScopedLock&&) = delete;

private:
    MutexType& mtx_;
    LockOrder order_;
    bool locked_;
    
    // Thread-local storage for tracking current lock order
    static thread_local LockOrder current_max_order_;
};

// Thread-local static member initialization
template<typename MutexType, bool TryLock>
thread_local LockOrder ScopedLock<MutexType, TryLock>::current_max_order_{LockOrder::ATOMIC_FLAGS};

// ========================================
// CRITICAL SECTION (RAII ChibiOS Critical Section)
// ========================================

/**
 * @brief RAII wrapper for ChibiOS critical section
 *
 * Disables interrupts on construction, re-enables on destruction.
 * Used for very short critical sections (no blocking operations).
 *
 * USAGE:
 *   {
 *       CriticalSection cs;
 *       // Critical section here (interrupts disabled)
 *       // No blocking operations allowed!
 *   }
 *   // Interrupts re-enabled here
 *
 * @note Must be used for very short sections only
 * @note No blocking operations (I/O, sleep) allowed inside critical section
 * @warning DO NOT use with file I/O or long operations
 */
class CriticalSection {
public:
    /**
     * @brief Constructor - Enters critical section (disables interrupts)
     */
    CriticalSection() noexcept {
        chSysLock();
    }

    /**
     * @brief Destructor - Exits critical section (re-enables interrupts)
     */
    ~CriticalSection() noexcept {
        chSysUnlock();
    }

    /// @brief Non-copyable, non-movable (RAII requirement)
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
    CriticalSection(CriticalSection&&) = delete;
    CriticalSection& operator=(CriticalSection&&) = delete;
};

// ========================================
// THREAD GUARD (RAII Thread Management)
// ========================================

/**
 * @brief RAII wrapper for ChibiOS thread lifecycle
 *
 * Terminates and waits for thread on destruction.
 * Ensures threads are properly cleaned up even on early returns.
 *
 * USAGE:
 *   ThreadGuard guard(scanning_thread_);
 *   // Thread will be automatically terminated when 'guard' goes out of scope
 *
 * @note Non-copyable, movable (supports move semantics)
 * @note Thread is terminated and waited for in destructor
 */
class ThreadGuard {
public:
    /**
     * @brief Constructor - Takes ownership of thread pointer
     * @param thread Pointer to ChibiOS thread (may be nullptr)
     */
    explicit ThreadGuard(Thread* thread) noexcept : thread_(thread) {}

    /**
     * @brief Destructor - Terminates and waits for thread
     *
     * If thread pointer is not nullptr:
     * 1. Signals thread to terminate
     * 2. Waits for thread to exit
     */
    ~ThreadGuard() noexcept {
        if (thread_) {
            chThdTerminate(thread_);
            chThdWait(thread_);
            thread_ = nullptr;
        }
    }

    /**
     * @brief Move constructor - Transfers ownership
     * @param other ThreadGuard to move from
     */
    ThreadGuard(ThreadGuard&& other) noexcept : thread_(other.thread_) {
        other.thread_ = nullptr;
    }

    /**
     * @brief Move assignment operator
     * @param other ThreadGuard to move from
     * @return Reference to this
     */
    ThreadGuard& operator=(ThreadGuard&& other) noexcept {
        if (this != &other) {
            // Clean up current thread
            if (thread_) {
                chThdTerminate(thread_);
                chThdWait(thread_);
            }
            // Transfer ownership
            thread_ = other.thread_;
            other.thread_ = nullptr;
        }
        return *this;
    }

    /// @brief Non-copyable (RAII requirement)
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;

    /**
     * @brief Release ownership without terminating thread
     * @return Pointer to thread (caller takes ownership)
     */
    Thread* release() noexcept {
        Thread* tmp = thread_;
        thread_ = nullptr;
        return tmp;
    }

    /**
     * @brief Get thread pointer
     * @return Pointer to thread (may be nullptr)
     */
    Thread* get() const noexcept { return thread_; }

    /**
     * @brief Check if thread is being managed
     * @return true if thread pointer is not nullptr
     */
    bool has_thread() const noexcept { return thread_ != nullptr; }

private:
    Thread* thread_;
};

// ========================================
// MUTEX INITIALIZER (RAII Mutex Setup)
// ========================================

/**
 * @brief RAII wrapper for ChibiOS mutex initialization
 *
 * Initializes mutex on construction, does NOT clean up on destruction
 * (ChibiOS mutexes are typically static and never de-initialized).
 *
 * USAGE:
 *   MutexInitializer init(my_mutex);
 *   // my_mutex is now initialized and ready to use
 *
 * @note Mutex is NOT de-initialized (ChibiOS design)
 * @note Use for static mutexes that live for entire program lifetime
 */
class MutexInitializer {
public:
    /**
     * @brief Constructor - Initializes mutex
     * @param mtx Reference to mutex to initialize
     */
    explicit MutexInitializer(Mutex& mtx) noexcept : mtx_(mtx) {
        chMtxInit(&mtx_);
    }

    /// @brief Destructor - Does nothing (ChibiOS mutexes are never de-initialized)
    ~MutexInitializer() noexcept {
        // ChibiOS mutexes are typically static and never de-initialized
        // Leaving this empty is intentional
    }

    /// @brief Non-copyable, non-movable
    MutexInitializer(const MutexInitializer&) = delete;
    MutexInitializer& operator=(const MutexInitializer&) = delete;
    MutexInitializer(MutexInitializer&&) = delete;
    MutexInitializer& operator=(MutexInitializer&&) = delete;

private:
    Mutex& mtx_;
};

// ========================================
// BACKWARD COMPATIBILITY ALIASES
// ========================================

/// @brief Alias for ScopedLock with blocking behavior
using MutexLock = ScopedLock<Mutex, false>;

/// @brief Alias for ScopedLock with try-lock behavior
using MutexTryLock = ScopedLock<Mutex, true>;

/// @brief Alias for ScopedLock with SD card lock order
using SDCardLock = ScopedLock<Mutex, false>;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_RAI_HPP_
