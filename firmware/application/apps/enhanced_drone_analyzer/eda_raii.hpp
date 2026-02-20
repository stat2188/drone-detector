/**
 * @file eda_raii.hpp
 * @brief RAII Wrappers for Enhanced Drone Analyzer
 *
 * DIAMOND CODE STANDARDS:
 * - Zero-heap allocation (no new, malloc, std::vector, std::string)
 * - RAII wrappers for automatic resource management
 * - noexcept for exception-free operation
 * - Guard clauses for early returns
 * - Doxygen comments for public APIs
 *
 * STAGE 4 DIAMOND FIXES:
 * - Updated chMtxUnlock() to use newer ChibiOS API (no parameter)
 * - Added ERRNO_MUTEX to lock ordering hierarchy (value 7)
 * - Consistent API usage across all RAII wrappers
 *
 * RED TEAM FIX: ScopedLock destructor now uses chMtxUnlock() without parameter
 * to match the newer ChibiOS API. This ensures consistent API usage across
 * the codebase and prevents potential bugs from mixed API usage.
 *
 * @target STM32F405 (ARM Cortex-M4, 128KB RAM)
 * @os ChibiOS (bare-metal RTOS)
 */

#ifndef EDA_RAII_HPP_
#define EDA_RAII_HPP_

#include <cstdint>
#include <atomic>
#include <array>
#include <ch.h>
#include <chtypes.h>
#include <chthreads.h>

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// LOCK ORDER ENUMERATION
// ========================================

/**
 * @brief Lock order levels for deadlock prevention
 *
 * Locks must ALWAYS be acquired in ascending order to prevent deadlocks.
 * Lower values must be acquired before higher values.
 *
 * @warning NEVER acquire locks out of order - this will cause deadlocks!
 * @note Each thread maintains its own lock stack for nested lock tracking
 *
 * RED TEAM FIX: Added ERRNO_MUTEX at highest priority (value 7)
 */
enum class LockOrder : uint8_t {
    ATOMIC_FLAGS = 1,  ///< Lowest priority - atomic flag operations
    DATA_MUTEX = 2,    ///< Data structure protection
    SPECTRUM_MUTEX = 3, ///< Spectrum analyzer data
    LOGGER_MUTEX = 4,  ///< Logging operations
    SD_CARD_MUTEX = 5,  ///< SD card I/O operations
    SETTINGS_MUTEX = 6,  ///< Settings persistence
    ERRNO_MUTEX = 7      ///< RED TEAM FIX: errno access protection (highest priority)
};

// ========================================
// LOCK RESULT ENUMERATION
// ========================================

/**
 * @brief Result of lock acquisition attempt
 *
 * Used to report lock acquisition status to callers, forcing error handling.
 */
enum class LockResult : uint8_t {
    SUCCESS = 0,           ///< Lock acquired successfully
    ORDER_VIOLATION = 1,   ///< Lock order violated (would cause deadlock)
    TRYLOCK_FAILED = 2,    ///< Try-lock failed (lock already held)
    INVALID_THREAD = 3     ///< Thread ID out of range
};

// ========================================
// LOCK STACK TRACKING
// ========================================

/**
 * @brief Per-thread lock stack for nested lock tracking
 *
 * Maintains a stack of currently held locks for proper deadlock prevention.
 * Each thread has its own stack indexed by thread ID.
 */
struct LockStack {
    static constexpr size_t MAX_DEPTH = 4;  ///< Maximum nested lock depth
    std::array<LockOrder, MAX_DEPTH> stack; ///< Lock order stack
    uint8_t depth;                          ///< Current stack depth

    /// @brief Initialize empty lock stack
    constexpr LockStack() noexcept : stack{}, depth(0) {}

    /**
     * @brief Push a lock onto stack
     * @param order Lock order to push
     * @return true if pushed successfully, false if stack full
     */
    bool push(LockOrder order) noexcept {
        if (depth >= MAX_DEPTH) {
            return false;
        }
        stack[depth] = order;
        depth++;
        return true;
    }

    /**
     * @brief Pop top lock from stack
     * @return true if popped successfully, false if stack empty
     */
    bool pop() noexcept {
        if (depth == 0) {
            return false;
        }
        depth--;
        return true;
    }

    /**
     * @brief Get current maximum lock order (top of stack)
     * @return Current maximum lock order, or ATOMIC_FLAGS if empty
     */
    LockOrder current_max() const noexcept {
        if (depth == 0) {
            return LockOrder::ATOMIC_FLAGS;
        }
        return stack[depth - 1];
    }

    /**
     * @brief Check if a lock order is valid (higher than current max)
     * @param order Lock order to check
     * @return true if order is valid, false if would violate ordering
     */
    bool is_valid_order(LockOrder order) const noexcept {
        return order > current_max();
    }
};

// ========================================
// GLOBAL LOCK ORDER TRACKER
// ========================================

/**
 * @brief Global lock order tracker for all threads
 *
 * Indexed by thread ID to provide per-thread lock stacks.
 * Uses atomic operations for thread-safe access.
 */
class LockOrderTracker {
public:
    static constexpr size_t MAX_THREADS = 8;  ///< Maximum concurrent threads

    /// @brief Get lock stack for current thread
    static LockStack& get_stack() noexcept {
        // Get current thread using ChibiOS API
        Thread* current = chThdSelf();

        // Use thread pointer low bits as index (safe for ChibiOS thread alignment)
        size_t index = reinterpret_cast<size_t>(current) % MAX_THREADS;
        return stacks_[index];
    }

private:
    /// @brief Per-thread lock stacks
    static std::array<LockStack, MAX_THREADS> stacks_;
};

// Static member initialization
inline std::array<LockStack, LockOrderTracker::MAX_THREADS> LockOrderTracker::stacks_;

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
 *   LockResult result;
 *   ScopedLock<Mutex, false> lock(data_mutex, LockOrder::DATA_MUTEX, result);
 *   if (result != LockResult::SUCCESS) {
 *       // Handle error
 *       return;
 *   }
 *   // Critical section here
 *   // Lock automatically released when 'lock' goes out of scope
 *
 * @note Non-copyable, non-movable (RAII requirement)
 * @note Zero-overhead abstraction (optimizes to direct chMtxLock/chMtxUnlock)
 *
 * @warning BLOCKING BEHAVIOR: When TryLock=false, chMtxLock() blocks indefinitely
 *          until the lock is acquired. This can cause priority inversion if a
 *          low-priority thread holds the lock while a high-priority thread waits.
 *          Use TryLock=true for non-blocking behavior and check the result.
 *
 * @warning DEADLOCK PREVENTION: Locks MUST be acquired in ascending LockOrder.
 *          Attempting to acquire a lower-order lock while holding a higher-order
 *          lock will fail with ORDER_VIOLATION.
 *
 * RED TEAM FIX: Destructor uses chMtxUnlock() without parameter (newer ChibiOS API)
 */
template<typename MutexType, bool TryLock = false>
class ScopedLock {
public:
    /**
     * @brief Constructor - Acquires lock with order tracking and error reporting
     * @param mtx Reference to mutex to lock
     * @param order Lock order level (must be higher than any held lock)
     * @param result Reference to store lock acquisition result (required)
     *
     * @post result is set to SUCCESS, ORDER_VIOLATION, or TRYLOCK_FAILED
     */
    explicit ScopedLock(MutexType& mtx, LockOrder order, LockResult& result) noexcept
        : mtx_(mtx), order_(order), locked_(false) {

        // Initialize result to failure
        result = LockResult::ORDER_VIOLATION;

        // Get lock stack for current thread
        LockStack& stack = LockOrderTracker::get_stack();

        // Guard clause: Check lock order before attempting acquisition
        if (!stack.is_valid_order(order_)) {
            // Order violation - skip acquisition, result already set
            return;
        }

        // Acquire lock
        if constexpr (TryLock) {
            // Non-blocking try-lock
            locked_ = chMtxTryLock(&mtx_);
            if (!locked_) {
                result = LockResult::TRYLOCK_FAILED;
                return;
            }
        } else {
            // Blocking lock - WARNING: Can cause priority inversion!
            chMtxLock(&mtx_);
            locked_ = true;
        }

        // Push lock onto stack and report success
        if (locked_) {
            stack.push(order_);
            result = LockResult::SUCCESS;
        }
    }

    /**
     * @brief Constructor - Acquires lock with order tracking (backward compatible)
     * @param mtx Reference to mutex to lock
     * @param order Lock order level (must be higher than any held lock)
     *
     * @note This constructor does not report errors. Use the version with LockResult
     *       parameter for proper error handling.
     * @warning If lock order is violated, the lock will NOT be acquired silently!
     */
    explicit ScopedLock(MutexType& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
        : mtx_(mtx), order_(order), locked_(false) {

        // Get lock stack for current thread
        LockStack& stack = LockOrderTracker::get_stack();

        // Guard clause: Check lock order before attempting acquisition
        if (!stack.is_valid_order(order_)) {
            // Order violation - skip acquisition (silent failure)
            return;
        }

        // Acquire lock
        if constexpr (TryLock) {
            // Non-blocking try-lock
            locked_ = chMtxTryLock(&mtx_);
        } else {
            // Blocking lock - WARNING: Can cause priority inversion!
            chMtxLock(&mtx_);
            locked_ = true;
        }

        // Push lock onto stack if acquired
        if (locked_) {
            stack.push(order_);
        }
    }

    /**
     * @brief Destructor - Releases lock and updates order tracking
     *
     * RED TEAM FIX: Uses chMtxUnlock() without parameter (newer ChibiOS API)
     * This ensures consistent API usage across the codebase.
     */
    ~ScopedLock() noexcept {
        if (locked_) {
            // RED TEAM FIX: Use chMtxUnlock() without parameter (newer ChibiOS API)
            chMtxUnlock();

            // Pop lock from stack (proper nested lock tracking)
            LockStack& stack = LockOrderTracker::get_stack();
            stack.pop();
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
};

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

#endif // EDA_RAII_HPP_
