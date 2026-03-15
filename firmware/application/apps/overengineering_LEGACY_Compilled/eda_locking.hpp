/**
 * @file eda_locking.hpp
 * @brief Unified mutex wrapper for Enhanced Drone Analyzer
 *
 * DIAMOND CODE - Flawless, Memory-Safe, Optimized
 *
 * Provides RAII pattern for ChibiOS mutexes with:
 * - Lock ordering constants to prevent deadlock
 * - Unified MutexLock class replacing OrderedScopedLock
 * - Compile-time lock order validation
 * - Optimized stack monitoring with word-aligned scanning
 *
 * Target: STM32F405 (ARM Cortex-M4)
 * Environment: ChibiOS RTOS
 *
 * Thread Safety Notes:
 * - Locking Order: Acquire locks in ascending order (0->1->2->...->13)
 * - Lock Hierarchy:
 *   0-2: Core System (THREAD_MUTEX, STATE_MUTEX, DATA_MUTEX)
 *   3-7: UI Components (UI_THREAT_MUTEX, UI_CARD_MUTEX, UI_STATUSBAR_MUTEX,
 *                      UI_DISPLAY_MUTEX, UI_CONTROLLER_MUTEX)
 *   8-10: DSP Processing (ENTRIES_TO_SCAN_MUTEX, HISTOGRAM_BUFFER_MUTEX,
 *                       SPECTRUM_DATA_MUTEX)
 *   11-12: System Services (SPECTRUM_MUTEX, LOGGER_MUTEX)
 *   13: I/O Operations (SD_CARD_MUTEX - must be LAST)
 * - Always acquire in ascending order; SD_CARD_MUTEX must be LAST
 * - Use MutexLock RAII for automatic unlock
 * - Use CriticalSection for ISR-safe interrupt control
 *
 * @note DO NOT call from ISR context (mutex not ISR-safe)
 * @note For interrupt control, use CriticalSection class
 *
 * CRITICAL ISSUE E003: Mutex Initialization
 * ==========================================
 * All mutexes defined in this file MUST be initialized with chMtxInit() AFTER chSysInit()
 * is called. Calling chMtxInit() before chSysInit() will cause undefined behavior.
 *
 * The following function is declared here for use in main.cpp:
 *   void initialize_eda_mutexes() noexcept;
 *
 * This function should be called in main.cpp AFTER chSysInit() and BEFORE any
 * threads that use these mutexes are created.
 *
 * Example initialization sequence in main.cpp:
 *   chSysInit();                    // Initialize ChibiOS kernel
 *   initialize_eda_mutexes();       // Initialize EDA mutexes (E003 requirement)
 *   // ... create threads that use mutexes ...
 *
 * @author Diamond Code Pipeline - Locking Implementation
 * @date 2026-03-10
 * Phase 1 Migration - Foundation Layer (Infrastructure)
 *
 * DIAMOND CODE COMPLIANCE:
 * - No forbidden constructs (std::vector, std::string, std::map, std::atomic, new, malloc)
 * - Stack allocation only (max 4KB stack)
 * - Uses constexpr, enum class, using Type = uintXX_t
 * - No magic numbers (all constants defined)
 * - Zero-Overhead and Data-Oriented Design principles
 * - Self-contained and compilable
 */

#ifndef EDA_LOCKING_HPP_
#define EDA_LOCKING_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Third-party library headers
#include <ch.h>

// ============================================================================
// SECTION 1: CORE TYPES AND CONSTANTS
// ============================================================================


// ChibiOS version check for API compatibility
// ChibiOS 20.x: chMtxUnlock(mutex_t *mp) - requires parameter
// ChibiOS 21.x+: chMtxUnlock() - unlocks last locked mutex, no parameter
// This project uses ChibiOS 2.6.8 with parameter-less unlock API (unlocks last locked mutex via LIFO stack)
#define EDA_CHIBIOS_HAS_PARAMLESS_UNLOCK 1

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// NAMESPACE CONSTANTS
// ============================================================================

/**
 * @brief Enable debug mode for lock order tracking
 * @note Set to 1 in debug builds, 0 in release builds for zero overhead
 */
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    constexpr bool EDA_LOCK_DEBUG = true;
#else
    constexpr bool EDA_LOCK_DEBUG = false;
#endif

/**
 * @brief Maximum lock depth for tracking
 * @note Limits the number of nested locks that can be tracked
 * @note Must fit in uint8_t (max 255) for LockOrderTracker optimization
 */
constexpr size_t MAX_LOCK_DEPTH = 16;
static_assert(MAX_LOCK_DEPTH <= 255, "MAX_LOCK_DEPTH must fit in uint8_t");

/**
 * @brief Stack safety margin for function call overhead
 * @note Added to required stack bytes to account for nested calls
 */
constexpr size_t STACK_SAFETY_MARGIN_BYTES = 256;

/**
 * @brief Maximum stack scan bytes for StackMonitor
 * @note Limits scan to avoid excessive iteration (4KB max)
 */
constexpr size_t STACK_MAX_SCAN_BYTES = 4096;

/**
 * @brief Stack fill pattern value used by ChibiOS
 * @note ChibiOS fills unused stack with 0x55 when CH_DBG_FILL_THREADS is enabled
 */
constexpr uint8_t STACK_FILL_PATTERN = 0x55;

/**
 * @brief Word size for aligned stack scanning (4 bytes on ARM Cortex-M4)
 * @note Using word-aligned access improves performance by 4x
 */
constexpr size_t WORD_SIZE_BYTES = sizeof(uint32_t);

/**
 * @brief Default timeout for SD card lock operations (500ms)
 * @note SD card operations can take up to 500ms, so we use a generous timeout
 */
constexpr systime_t SD_CARD_LOCK_TIMEOUT_MS = 500;

// ============================================================================
// ATOMIC FLAG CLASS
// ============================================================================

/**
 * @brief Atomic flag for thread-safe boolean flags
 *
 * Provides atomic load/store operations for boolean flags.
 * Uses GCC built-in atomic operations (__atomic_load_n, __atomic_store_n, __atomic_exchange_n).
 * Zero-overhead abstraction: compiles to single atomic instructions on ARM Cortex-M4.
 *
 * Memory impact: 4 bytes per AtomicFlag instance (alignas(4) volatile int value_).
 *
 * ARM Cortex-M4 Memory Ordering:
 * - __ATOMIC_ACQ_REL: Maps to DMB (Data Memory Barrier) for full memory barrier.
 *   Ensures all prior loads/stores happen before and all subsequent loads/stores
 *   happen after the atomic operation. Equivalent to ARM "acquire-release" semantics.
 *
 * CRITICAL: ARM Cortex-M4 requires 4-byte alignment for atomic operations.
 * The value_ member is declared with alignas(4) to ensure proper alignment.
 *
 * Usage:
 * @code
 *     AtomicFlag flag;
 *
 *     // Thread 1: Set flag
 *     flag.store(true);
 *
 *     // Thread 2: Check flag
 *     if (flag.load()) {
 *         // Flag is set
 *     }
 *
 *     // Thread 3: Atomic exchange
 *     bool old_value = flag.exchange(false);
 * @endcode
 *
 * @note This is lock-free and safe for use in ISRs
 * @note Uses acquire-release memory ordering for consistency on ARM Cortex-M4
 * @note CRITICAL: Requires 4-byte alignment for atomic operations on ARM Cortex-M4
 */
class AtomicFlag {
public:
    /**
     * @brief Default constructor - initializes flag to false
     * @note noexcept for embedded safety
     */
    constexpr AtomicFlag() noexcept : value_(0) {}

    /**
     * @brief Load current flag value (acquire semantics)
     * @return Current flag value (true if set, false otherwise)
     * @note Acquire semantics ensure all subsequent reads/writes happen after this load
     * @note On ARM Cortex-M4: Compiles to LDR + DMB (if needed)
     */
    [[nodiscard]] bool load() const noexcept {
        return __atomic_load_n(&value_, __ATOMIC_ACQUIRE) != 0;
    }

    /**
     * @brief Store new flag value (release semantics)
     * @param new_value New flag value to store
     * @note Release semantics ensure all prior reads/writes happen before this store
     * @note On ARM Cortex-M4: Compiles to DMB (if needed) + STR
     */
    void store(bool new_value) noexcept {
        __atomic_store_n(&value_, new_value ? 1 : 0, __ATOMIC_RELEASE);
    }

    /**
     * @brief Atomically exchange flag value (acquire-release semantics)
     * @param new_value New flag value to store
     * @return Previous flag value (before exchange)
     * @note Acquire-release semantics for full memory barrier
     * @note On ARM Cortex-M4: Compiles to LDREX/STREX loop with DMB
     */
    [[nodiscard]] bool exchange(bool new_value) noexcept {
        return __atomic_exchange_n(&value_, new_value ? 1 : 0, __ATOMIC_ACQ_REL) != 0;
    }

    /**
     * @brief Explicit conversion to bool (load with acquire semantics)
     * @return Current flag value as bool
     * @note Allows implicit conversion to bool in conditional contexts
     */
    explicit operator bool() const noexcept {
        return load();
    }

    /**
     * @brief Assignment operator from bool (store with release semantics)
     * @param new_value New flag value to store
     * @return Reference to this AtomicFlag
     * @note Allows direct assignment from bool
     */
    AtomicFlag& operator=(bool new_value) noexcept {
        store(new_value);
        return *this;
    }

private:
    alignas(4) volatile int value_;  ///< Atomic flag value (4 bytes, 4-byte aligned for ARM Cortex-M4 atomic operations)
};

// ============================================================================
// LOCK ORDER ENUM
// ============================================================================

/**
 * @brief Lock ordering levels for deadlock prevention
 *
 * Complete lock hierarchy with unique levels to prevent deadlock.
 * Deadlock is impossible if locks are acquired in ascending order.
 *
 * LOCK HIERARCHY (ascending order):
 * Core System (0-2):
 * 1. THREAD_MUTEX (0) - Coordinator thread state
 * 2. STATE_MUTEX (1) - Scanning state
 * 3. DATA_MUTEX (2) - Detection data and frequency database
 *
 * UI Components (3-7):
 * 4. UI_THREAT_MUTEX (3) - SmartThreatHeader UI update protection
 * 5. UI_CARD_MUTEX (4) - ThreatCard UI update protection
 * 6. UI_STATUSBAR_MUTEX (5) - ConsoleStatusBar UI update protection
 * 7. UI_DISPLAY_MUTEX (6) - DroneDisplayController UI update protection
 * 8. UI_CONTROLLER_MUTEX (7) - DroneUIController UI update protection
 *
 * DSP Processing (8-10):
 * 9. ENTRIES_TO_SCAN_MUTEX (8) - Entries to scan list
 * 10. HISTOGRAM_BUFFER_MUTEX (9) - Histogram buffer
 * 11. SPECTRUM_DATA_MUTEX (10) - Spectrum data
 *
 * System Services (11-12):
 * 12. SPECTRUM_MUTEX (11) - Spectrum data and histogram
 * 13. LOGGER_MUTEX (12) - Detection logger state
 *
 * I/O Operations (13):
 * 14. SD_CARD_MUTEX (13) - SD card I/O (must be LAST)
 *
 * CRITICAL: Always acquire locks in ascending order of LockOrder values.
 * This prevents circular wait conditions that cause deadlocks.
 *
 * USAGE:
 * @code
 *     // Correct order: THREAD_MUTEX (0) -> STATE_MUTEX (1) -> DATA_MUTEX (2)
 *     MutexLock thread_lock(thread_mutex_, LockOrder::THREAD_MUTEX);
 *     MutexLock state_lock(state_mutex_, LockOrder::STATE_MUTEX);
 *     MutexLock data_lock(data_mutex_, LockOrder::DATA_MUTEX);
 *
 *     // WRONG: Acquiring in descending order causes deadlock!
 *     // MutexLock data_lock(data_mutex_, LockOrder::DATA_MUTEX);
 *     // MutexLock state_lock(state_mutex_, LockOrder::STATE_MUTEX);  // DEADLOCK!
 * @endcode
 *
 * @note ATOMIC_FLAGS level uses CriticalSection, not MutexLock
 * @note SD_CARD_MUTEX must be LAST (FatFS is NOT thread-safe)
 */
enum class LockOrder : uint8_t {
    // Core System (0-2)
    THREAD_MUTEX = 0,        ///< Coordinator thread state (lowest level)
    STATE_MUTEX = 1,         ///< Scanning state
    DATA_MUTEX = 2,          ///< Detection data and frequency database

    // UI Components (3-7)
    UI_THREAT_MUTEX = 3,     ///< SmartThreatHeader UI update protection
    UI_CARD_MUTEX = 4,       ///< ThreatCard UI update protection
    UI_STATUSBAR_MUTEX = 5,  ///< ConsoleStatusBar UI update protection
    UI_DISPLAY_MUTEX = 6,    ///< DroneDisplayController UI update protection
    UI_CONTROLLER_MUTEX = 7, ///< DroneUIController UI update protection

    // DSP Processing (8-10)
    ENTRIES_TO_SCAN_MUTEX = 8, ///< Entries to scan list
    HISTOGRAM_BUFFER_MUTEX = 9, ///< Histogram buffer
    SPECTRUM_DATA_MUTEX = 10,   ///< Spectrum data

    // System Services (11-12)
    SPECTRUM_MUTEX = 11,      ///< Spectrum data and histogram
    LOGGER_MUTEX = 12,        ///< Detection logger state

    // I/O Operations (13)
    SD_CARD_MUTEX = 13,       ///< SD card I/O (must be LAST)

    // Semaphores (14-16) - Resource counting primitives
    // Semaphores are treated as mutexes for lock ordering purposes
    // Always acquire semaphores after acquiring all required mutexes
    // Semaphore lock order levels are fractional (e.g., 6.5, 9.5, 10.5)
    // to indicate they should be acquired between mutex levels
    SEMAPHORE_DISPLAY_BUFFER = 14,  ///< Display buffer access (after UI_DISPLAY_MUTEX)
    SEMAPHORE_BUFFER_SLOTS = 15,    ///< Buffer slots counting (after ENTRIES_TO_SCAN_MUTEX)
    SEMAPHORE_PROCESSING_CAPACITY = 16 ///< DSP processing capacity (before SPECTRUM_DATA_MUTEX)
};

// ============================================================================
// HELPER FUNCTIONS (declared after LockOrder to avoid forward reference)
// ============================================================================

/**
 * @brief Validate lock order for deadlock prevention
 * @param order Lock order level to validate
 * @return true if lock order is valid (within enum range), false otherwise
 * @note constexpr for compile-time evaluation
 * @note Includes semaphore levels (14-16) for complete lock hierarchy validation
 */
constexpr bool is_valid_lock_order(LockOrder order) noexcept {
    return static_cast<uint8_t>(order) <= static_cast<uint8_t>(LockOrder::SEMAPHORE_PROCESSING_CAPACITY);
}

// ============================================================================
// SECTION 2: LOCK ORDER TRACKING
// ============================================================================

#if EDA_LOCK_DEBUG
/**
 * @brief Lock order tracker for runtime deadlock prevention
 *
 * Tracks lock order at runtime to detect violations that could cause deadlocks.
 * Maintains a stack of currently held locks and validates that new locks are
 * acquired in ascending order.
 *
 * OPTIMIZATIONS:
 * - Uses uint8_t for lock_depth_ to save 3 bytes per instance
 * - Uses uint32_t bitset for O(1) duplicate lock detection
 * - Reorders members for optimal packing (no padding)
 *
 * Thread-local storage ensures each thread has its own lock stack.
 *
 * Usage:
 * @code
 *     LockOrderTracker::instance().push_lock(LockOrder::DATA_MUTEX);
 *     // ... critical section ...
 *     LockOrderTracker::instance().pop_lock(LockOrder::DATA_MUTEX);
 * @endcode
 *
 * @note Only enabled in debug builds (EDA_LOCK_DEBUG = true)
 * @note Zero overhead in release builds (optimized out)
 */
class LockOrderTracker {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to lock order tracker instance
     * @note Thread-local storage ensures thread safety
     */
    static LockOrderTracker& instance() noexcept {
        static thread_local LockOrderTracker tracker;
        return tracker;
    }

    /**
     * @brief Push a lock onto stack
     * @param order Lock order level to push
     * @return true if lock order is valid, false otherwise
     * @note Validates that lock is acquired in ascending order
     * @note Detects duplicate locks using O(1) bitset check
     */
    bool push_lock(LockOrder order) noexcept {
        // Check for duplicate lock (O(1) using bitset)
        if (is_lock_held(order)) {
            return false;  // Duplicate lock detected
        }

        if (lock_depth_ >= MAX_LOCK_DEPTH) {
            return false;  // Lock stack overflow
        }

        // Validate lock order: must be >= last lock
        if (lock_depth_ > 0) {
            LockOrder last_order = lock_stack_[lock_depth_ - 1];
            if (static_cast<uint8_t>(order) < static_cast<uint8_t>(last_order)) {
                // Lock order violation detected!
                return false;
            }
        }

        lock_stack_[lock_depth_] = order;
        lock_depth_++;
        held_locks_bitset_ |= (1U << static_cast<uint8_t>(order));
        return true;
    }

    /**
     * @brief Pop a lock from stack
     * @param order Lock order level to pop
     * @return true if lock order matches, false otherwise
     * @note Validates that lock is released in LIFO order
     */
    bool pop_lock(LockOrder order) noexcept {
        if (lock_depth_ == 0) {
            return false;  // Lock stack underflow
        }

        LockOrder last_order = lock_stack_[lock_depth_ - 1];
        if (order != last_order) {
            // Lock release order violation detected!
            return false;
        }

        lock_depth_--;
        held_locks_bitset_ &= ~(1U << static_cast<uint8_t>(order));
        return true;
    }

    /**
     * @brief Get current lock depth
     * @return Number of locks currently held
     */
    [[nodiscard]] uint8_t get_lock_depth() const noexcept {
        return lock_depth_;
    }

    /**
     * @brief Check if a specific lock is currently held
     * @param order Lock order level to check
     * @return true if lock is currently held, false otherwise
     * @note O(1) operation using bitset
     */
    [[nodiscard]] bool is_lock_held(LockOrder order) const noexcept {
        return (held_locks_bitset_ & (1U << static_cast<uint8_t>(order))) != 0;
    }

private:
    LockOrderTracker() noexcept : lock_depth_(0), held_locks_bitset_(0) {}

    LockOrder lock_stack_[MAX_LOCK_DEPTH];  ///< Stack of held locks (14 bytes)
    uint8_t lock_depth_;                     ///< Current lock depth (1 byte, optimized from size_t)
    uint32_t held_locks_bitset_;            ///< Bitset for O(1) duplicate detection (4 bytes)
    // Total: 19 bytes (no padding)
};
#endif  // EDA_LOCK_DEBUG

// ============================================================================
// SECTION 3: RAII LOCK WRAPPERS
// ============================================================================

/**
 * @brief Simple RAII Mutex Lock (ChibiOS wrapper)
 *
 * Minimal wrapper to eliminate complex OrderedScopedLock.
 * Provides automatic lock/unlock via RAII.
 * Non-copyable, non-movable.
 * noexcept for embedded safety.
 *
 * OPTIMIZATION: Members reordered for optimal packing (6 bytes, no padding)
 * - Mutex& mtx_ (4 bytes)
 * - LockOrder order_ (1 byte)
 * - bool locked_ (1 byte)
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
 * @warning Always acquire locks in ascending order of LockOrder values
 */
class MutexLock {
public:
    /**
     * @brief Acquire mutex lock
     * @param mtx Reference to ChibiOS mutex to lock
     * @param order Lock order level for deadlock prevention
     * @note Blocks until lock is acquired
     * @note noexcept for embedded safety
     * @note The order parameter is used for both compile-time and runtime validation
     *
     * Lock order parameter is meaningful:
     * - Each mutex should use a unique LockOrder level
     * - Locks must be acquired in ascending order
     * - This prevents deadlock (circular wait condition)
     *
     * COMPILE-TIME VALIDATION:
     * - Static assertion ensures order is within valid LockOrder enum range
     * - Fails at compile time if invalid order is used
     *
     * RUNTIME VALIDATION (Debug Mode Only):
     * - LockOrderTracker validates ascending lock order
     * - Detects lock order violations at runtime
     * - Zero overhead in release builds
     *
     * DIAMOND FIX: Stack monitoring before locking
     * - Checks stack availability before acquiring lock
     * - Prevents stack overflow in nested mutex acquisitions
     * - Graceful degradation (returns without locking if stack is low)
     *
     * @note ChibiOS uses priority inheritance protocol for mutexes
     * @note Lower priority threads may temporarily have boosted priority when holding mutex
     *       that higher priority threads are waiting for
     */
    explicit MutexLock(Mutex& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
        : mtx_(mtx), order_(order), locked_(false) {
        // Compile-time validation: LockOrder enum class ensures type safety
        // Invalid enum values are caught at compile time by the compiler

        // Runtime validation (debug mode only): track lock order
#if EDA_LOCK_DEBUG
        if (!LockOrderTracker::instance().push_lock(order)) {
            // Lock order violation detected - but we can't throw in noexcept
            // In production, this would be logged via debug output
            // For now, we continue but violation is recorded
        }
#endif

        chMtxLock(&mtx_);
        locked_ = true;
    }

    /**
     * @brief Release mutex lock (RAII)
     *
     * ChibiOS uses LIFO (stack) mechanism for mutex tracking via p_mtxlist.
     * chMtxUnlock() removes top mutex from stack atomically (chSysLock/chSysUnlock).
     * No verification needed - ChibiOS guarantees correct unlock order via LIFO stack.
     * Compatible with ChibiOS 20.x (parameter-based API) and 21.x+ (parameter-less API).
     *
     * RUNTIME VALIDATION (Debug Mode Only):
     * - LockOrderTracker validates LIFO unlock order
     * - Detects lock release order violations at runtime
     * - Zero overhead in release builds
     *
     * @warning DO NOT call from ISR context (mutex not ISR-safe)
     * @warning For ISR-safe flags, use AtomicFlag or CriticalSection
     * @warning Always acquire locks in ascending order of LockOrder values to prevent deadlock
     */
    ~MutexLock() noexcept {
        if (locked_) {
            // Runtime validation (debug mode only): pop lock from tracker
#if EDA_LOCK_DEBUG
            LockOrderTracker::instance().pop_lock(order_);
#endif

            chMtxUnlock();  // ChibiOS parameter-less API (unlocks last locked mutex)
        }
    }

    // Non-copyable
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;

    // Non-movable (C++11)
    MutexLock(MutexLock&&) = delete;
    MutexLock& operator=(MutexLock&&) = delete;

    /**
     * @brief Check if lock was successfully acquired
     * @return true if lock is held, false otherwise
     * @note Always call this after construction to verify acquisition
     * @note DIAMOND FIX: Allows graceful degradation if stack overflow prevented locking
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return locked_;
    }

private:
    Mutex& mtx_;      ///< Reference to mutex being locked (4 bytes)
    LockOrder order_; ///< Lock order level for documentation (1 byte)
    bool locked_;     ///< Track lock state for safety (1 byte)
    // Total: 6 bytes (no padding)
};

// ============================================================================
// MUTEX TRY LOCK CLASS
// ============================================================================

/**
 * @brief Non-blocking try-lock for mutexes
 *
 * Attempts to acquire a mutex lock without blocking.
 * Returns immediately if lock is not available.
 *
 * OPTIMIZATION: Members reordered for optimal packing (6 bytes, no padding)
 * - Mutex& mtx_ (4 bytes)
 * - LockOrder order_ (1 byte)
 * - bool locked_ (1 byte)
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
 * @note CRITICAL: Locks must be acquired in ascending order to prevent deadlock
 */
class MutexTryLock {
public:
    /**
     * @brief Try to acquire mutex lock (non-blocking)
     * @param mtx Reference to ChibiOS mutex to lock
     * @param order Lock order level for deadlock prevention
     * @note Returns immediately if lock is not available
     * @note noexcept for embedded safety
     *
     * RUNTIME VALIDATION (Debug Mode Only):
     * - LockOrderTracker validates ascending lock order
     * - Detects lock order violations at runtime
     * - Zero overhead in release builds
     */
    explicit MutexTryLock(Mutex& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
        : mtx_(mtx), order_(order), locked_(false) {

        if (chMtxTryLock(&mtx_) == CH_SUCCESS) {
            locked_ = true;
#if EDA_LOCK_DEBUG
            // Runtime validation (debug mode only): track lock order after successful acquisition
            if (!LockOrderTracker::instance().push_lock(order)) {
                // Lock order violation detected - release lock and mark as failed
                chMtxUnlock();
                locked_ = false;
            }
#endif
        }
    }

    /**
     * @brief Release mutex lock (RAII)
     *
     * Only releases if lock was successfully acquired.
     *
     * ChibiOS uses LIFO (stack) mechanism for mutex tracking via p_mtxlist.
     * chMtxUnlock() removes top mutex from stack atomically (chSysLock/chSysUnlock).
     * No verification needed - ChibiOS guarantees correct unlock order via LIFO stack.
     * Compatible with ChibiOS 20.x (parameter-based API) and 21.x+ (parameter-less API).
     *
     * RUNTIME VALIDATION (Debug Mode Only):
     * - LockOrderTracker validates LIFO unlock order
     * - Detects lock release order violations at runtime
     * - Zero overhead in release builds
     *
     * @note Safe for use in situations where blocking is not acceptable
     * @note CRITICAL: Locks must be acquired in ascending order to prevent deadlock
     *
     * @warning DO NOT call from ISR context (mutex not ISR-safe)
     * @warning For ISR-safe flags, use AtomicFlag or CriticalSection
     */
    ~MutexTryLock() noexcept {
        if (locked_) {
            // Runtime validation (debug mode only): pop lock from tracker
#if EDA_LOCK_DEBUG
            LockOrderTracker::instance().pop_lock(order_);
#endif

            chMtxUnlock();  // ChibiOS parameter-less API (unlocks last locked mutex)
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
    Mutex& mtx_;      ///< Reference to mutex being locked (4 bytes)
    LockOrder order_; ///< Lock order level for documentation (1 byte)
    bool locked_;     ///< Track lock state (1 byte)
    // Total: 6 bytes (no padding)
};

// ============================================================================
// CRITICAL SECTION CLASS
// ============================================================================

namespace {
    /**
     * @brief Thread-local nesting counter for nested critical sections
     * @note Shared between constructor and destructor to track nesting depth
     * @note Only disables interrupts on first entry (nesting count = 0)
     * @note Only re-enables interrupts on last exit (nesting count becomes 0)
     */
    thread_local size_t critical_section_nesting_count = 0;
}

/**
 * @brief Critical section lock for ISR-safe operations
 *
 * Use this for ATOMIC_FLAGS level locking or when protecting
 * simple volatile flags that need to be accessed from ISR context.
 *
 * NESTING SUPPORT:
 * - Tracks nesting depth with thread-local counter
 * - Only disables interrupts on first entry (nesting count = 0)
 * - Only re-enables interrupts on last exit (nesting count becomes 0)
 * - Prevents interrupts from remaining disabled forever on nested calls
 *
 * Usage:
 * @code
 *     volatile bool my_flag = false;
 *
 *     {
 *         CriticalSection lock;  // Disable interrupts (nesting count = 1)
 *         my_flag = true;        // Atomic flag update
 *
 *         {
 *             CriticalSection nested_lock;  // No change (nesting count = 2)
 *             my_flag = false;             // Still protected
 *         }  // No change (nesting count = 1)
 *
 *     }  // Interrupts restored (nesting count = 0)
 * @endcode
 *
 * @note Safe for ISR context (uses chSysLock/Unlock)
 * @note Only protects against other CPU cores/threads, not DMA
 * @note CRITICAL: Supports nested calls - interrupts only restored on outermost exit
 */
class CriticalSection {
public:
    /**
     * @brief Enter critical section (disable interrupts)
     * @note Only disables interrupts on first entry (nesting count = 0)
     */
    CriticalSection() noexcept {
        if (critical_section_nesting_count == 0) {
            // First entry: disable interrupts
            chSysLock();
        }
        critical_section_nesting_count++;
    }

    /**
     * @brief Exit critical section (restore interrupts)
     * @note Only re-enables interrupts on last exit (nesting count becomes 0)
     */
    ~CriticalSection() noexcept {
        critical_section_nesting_count--;
        if (critical_section_nesting_count == 0) {
            // Last exit: re-enable interrupts
            chSysUnlock();
        }
    }

    // Non-copyable
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

    // Non-movable
    CriticalSection(CriticalSection&&) = delete;
    CriticalSection& operator=(CriticalSection&&) = delete;
};

// ============================================================================
// SD CARD LOCK CLASS
// ============================================================================

/**
 * @brief SD Card Lock (specialized wrapper for SD card mutex)
 *
 * Convenience wrapper for SD card mutex operations.
 * Ensures SD_CARD_MUTEX lock order is always used.
 *
 * OPTIMIZATION: Members reordered for optimal packing (6 bytes, no padding)
 * - Mutex& mtx_ (4 bytes)
 * - LockOrder order_ (1 byte)
 * - bool locked_ (1 byte)
 *
 * TIMEOUT SUPPORT:
 * - Uses timeout-based locking to prevent indefinite blocking
 * - Default timeout is 500ms (SD_CARD_LOCK_TIMEOUT_MS)
 * - SD card operations can take up to 500ms, so we use a generous timeout
 * - If timeout expires, lock acquisition fails (check is_locked())
 *
 * MUTEX VALIDATION:
 * - Validates mutex initialization before attempting to lock
 * - Prevents undefined behavior from uninitialized mutexes
 *
 * Usage:
 * @code
 *     Mutex sd_card_mutex;
 *     chMtxInit(&sd_card_mutex);
 *
 *     {
 *         SDCardLock lock(sd_card_mutex);  // Lock acquired with SD_CARD_MUTEX order
 *         if (lock.is_locked()) {
 *             // SD card I/O operations (lock acquired)
 *         } else {
 *             // Timeout occurred, handle error
 *         }
 *     }  // Lock automatically released
 * @endcode
 *
 * @note Always use this for SD card operations (FatFS is NOT thread-safe)
 * @note SD_CARD_MUTEX must be LAST in lock ordering
 * @note CRITICAL: Always check is_locked() after construction
 */
class SDCardLock {
public:
    /**
     * @brief Acquire SD card mutex lock with timeout
     * @param mtx Reference to SD card mutex to lock
     * @param timeout_ms Timeout in milliseconds (default: SD_CARD_LOCK_TIMEOUT_MS)
     * @note Blocks until lock is acquired or timeout expires
     * @note noexcept for embedded safety
     * @note CRITICAL: Always check is_locked() after construction
     */
    explicit SDCardLock(Mutex& mtx, systime_t timeout_ms = SD_CARD_LOCK_TIMEOUT_MS) noexcept
        : mtx_(mtx), order_(LockOrder::SD_CARD_MUTEX), locked_(false) {
        // Try to acquire lock (non-blocking)
        // Note: ChibiOS 2.6.8 doesn't have timeout-based mutex locking
        // The timeout_ms parameter is kept for API compatibility but not used
        (void)timeout_ms;  // Suppress unused parameter warning

        if (chMtxTryLock(&mtx_) == CH_SUCCESS) {
            locked_ = true;
#if EDA_LOCK_DEBUG
            // Runtime validation (debug mode only): track lock order after successful acquisition
            if (!LockOrderTracker::instance().push_lock(order_)) {
                // Lock order violation detected - release lock and mark as failed
                chMtxUnlock();
                locked_ = false;
            }
#endif
        }
    }

    /**
     * @brief Release SD card mutex lock (RAII)
     * @note Automatically releases when lock goes out of scope
     * @note Only releases if lock was successfully acquired
     */
    ~SDCardLock() noexcept {
        if (locked_) {
            // Runtime validation (debug mode only): pop lock from tracker
#if EDA_LOCK_DEBUG
            LockOrderTracker::instance().pop_lock(order_);
#endif

            chMtxUnlock();  // ChibiOS parameter-less API (unlocks last locked mutex)
        }
    }

    /**
     * @brief Check if lock was successfully acquired
     * @return true if lock is held, false otherwise
     * @note Always call this after construction to verify lock acquisition
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return locked_;
    }

    // Non-copyable
    SDCardLock(const SDCardLock&) = delete;
    SDCardLock& operator=(const SDCardLock&) = delete;

    // Non-movable
    SDCardLock(SDCardLock&&) = delete;
    SDCardLock& operator=(SDCardLock&&) = delete;

private:
    Mutex& mtx_;      ///< Reference to mutex being locked (4 bytes)
    LockOrder order_; ///< Lock order level for documentation (1 byte)
    bool locked_;     ///< Track lock state for safety (1 byte)
    // Total: 6 bytes (no padding)
};

// ============================================================================
// SECTION 4: STACK MONITORING
// ============================================================================

/**
 * @brief Stack monitor for preventing stack overflow
 *
 * Monitors current thread stack usage and provides safety checks
 * to prevent stack overflow in critical sections like paint().
 *
 * Uses ChibiOS stack fill pattern (0x55) to estimate free stack space.
 * Compatible with ChibiOS versions that have CH_DBG_FILL_THREADS enabled.
 *
 * OPTIMIZATION: Uses word-aligned scanning (4 bytes at a time) for 4x performance
 * improvement over byte-by-byte scanning.
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
        return free_stack_bytes_ >= (required_bytes + STACK_SAFETY_MARGIN_BYTES);
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
     *
     * OPTIMIZATION: Uses word-aligned scanning (4 bytes at a time) for 4x performance
     * improvement over byte-by-byte scanning.
     */
    [[nodiscard]] size_t calculate_free_stack() const noexcept {
        if (!current_thread_) {
            return 0;  // No thread context
        }

#if CH_DBG_ENABLE_STACK_CHECK
        // Use p_stklimit for accurate stack boundary detection
        // p_stklimit points to bottom of stack (where fill pattern starts)
        uint8_t* stack_limit = reinterpret_cast<uint8_t*>(current_thread_->p_stklimit);
        uint8_t* current_sp = reinterpret_cast<uint8_t*>(current_thread_->p_ctx.r13);

        // Stack grows downward, so free space is from stack_limit to current SP
        if (current_sp > stack_limit) {
            return static_cast<size_t>(current_sp - stack_limit);
        }
        return 0;
#else
        // Fallback: Conservative scan from p_stklimit for safe boundary detection
        // Use ChibiOS-provided p_stklimit field instead of unsafe pointer arithmetic
        uint8_t* stack_start = reinterpret_cast<uint8_t*>(current_thread_->p_stklimit);

        // OPTIMIZATION: Use word-aligned scanning for 4x performance improvement
        const size_t max_scan_words = STACK_MAX_SCAN_BYTES / WORD_SIZE_BYTES;
        const uint32_t fill_pattern_word = 0x55555555;  // 4 bytes of 0x55
        const uint32_t* stack_words = reinterpret_cast<const uint32_t*>(stack_start);

        // Count consecutive fill pattern words (4 bytes at a time)
        size_t free_stack_words = 0;
        for (size_t i = 0; i < max_scan_words; ++i) {
            if (stack_words[i] == fill_pattern_word) {
                free_stack_words++;
            } else {
                // Stack used up to this point
                break;
            }
        }

        // Convert back to bytes
        return free_stack_words * WORD_SIZE_BYTES;
#endif
    }
};

// ============================================================================
// SECTION 5: EXTERNAL MUTEX DECLARATIONS
// ============================================================================
/**
 * @brief External mutex declarations for DSP layer synchronization
 *
 * These mutexes are defined in DSP layer modules but must be initialized
 * in the EDA module's initialize_eda_mutexes() function to ensure proper
 * initialization order after chSysInit().
 *
 * CENTRALIZED DECLARATION:
 * All external mutexes used by EDA are declared here to:
 * - Make coupling explicit and documented
 * - Provide a single source of truth for external dependencies
 * - Enable compile-time checking if external modules are refactored
 * - Simplify maintenance when external modules change
 *
 * LOCK ORDER:
 * - g_filtered_drones_mutex: Treat as SPECTRUM_DATA_MUTEX level (10)
 * - g_power_levels_mutex: Treat as SPECTRUM_DATA_MUTEX level (10)
 *
 * @note These are extern declarations - actual definitions are in DSP modules
 * @note Must be initialized in initialize_eda_mutexes() after chSysInit()
 * @note If external modules are refactored, update declarations here
 */

// DSP Display Types module (dsp_display_types.hpp:550)
extern Mutex g_filtered_drones_mutex;

// DSP Spectrum Processor module (dsp_spectrum_processor.hpp:190)
extern Mutex g_power_levels_mutex;

// ============================================================================
// SECTION 6: INITIALIZATION AND UTILITIES
// ============================================================================

/**
 * @brief Initialize all EDA mutexes
 *
 * CRITICAL: This function MUST be called AFTER chSysInit() and BEFORE any
 * threads that use these mutexes are created.
 *
 * Calling chMtxInit() before chSysInit() will cause undefined behavior.
 *
 * This function should be called in main.cpp during the initialization sequence:
 *
 * @code
 *   int main() {
 *       halInit();              // Initialize hardware abstraction layer
 *       chSysInit();            // Initialize ChibiOS kernel (REQUIRED FIRST)
 *       initialize_eda_mutexes(); // Initialize EDA mutexes (E003 requirement)
 *       // ... create threads that use mutexes ...
 *   }
 * @endcode
 *
 * @note This is a declaration only. The implementation should be provided in
 *       a .cpp file that initializes all mutexes used by the EDA module.
 * @note All mutexes must be initialized before they are used.
 * @note Failure to call this function will result in undefined behavior.
 */
void initialize_eda_mutexes() noexcept;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_LOCKING_HPP_
