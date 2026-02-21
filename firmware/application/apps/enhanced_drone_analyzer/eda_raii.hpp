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
 * CONSOLIDATION FIX:
 * - All RAII wrappers consolidated into eda_locking.hpp
 * - This header provides backward compatibility by including eda_locking.hpp
 * - Removed duplicate definitions to fix compilation errors
 * - NO std::atomic usage (not supported on STM32F405/ChibiOS)
 *
 * @target STM32F405 (ARM Cortex-M4, 128KB RAM)
 * @os ChibiOS (bare-metal RTOS)
 */

#ifndef EDA_RAII_HPP_
#define EDA_RAII_HPP_

// Include the canonical locking header
#include "eda_locking.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// BACKWARD COMPATIBILITY
// ========================================
// All RAII wrappers are now defined in eda_locking.hpp
// This header provides backward compatibility for existing code

} // namespace ui::apps::enhanced_drone_analyzer

// ========================================
// RAAI NAMESPACE (for SystemLock and MutexLock)
// ========================================
namespace raii {

/**
 * @brief RAII wrapper for ChibiOS system lock (critical section)
 *
 * Provides exception-safe (though exceptions are disabled) critical section
 * management. Locks on construction, unlocks on destruction.
 *
 * USAGE:
 *   {
 *       SystemLock lock;
 *       // Critical section here (interrupts disabled)
 *       // No blocking operations allowed!
 *   }
 *   // Interrupts re-enabled here
 *
 * @note Must be used for very short sections only
 * @note No blocking operations (I/O, sleep) allowed inside critical section
 */
class SystemLock {
public:
    SystemLock() noexcept { chSysLock(); }
    ~SystemLock() noexcept { chSysUnlock(); }

    // Non-copyable, non-movable
    SystemLock(const SystemLock&) = delete;
    SystemLock& operator=(const SystemLock&) = delete;
};

/**
 * @brief RAII wrapper for ChibiOS mutex lock
 *
 * Provides exception-safe mutex management. Locks on construction,
 * unlocks on destruction.
 *
 * USAGE:
 *   {
 *       MutexLock lock(&my_mutex);
 *       // Critical section here
 *   }
 *   // Mutex automatically unlocked here
 *
 * @note ChibiOS chMtxUnlock() takes no parameters (uses thread-local storage)
 */
class MutexLock {
public:
    explicit MutexLock(Mutex* mutex) noexcept : mutex_(mutex) {
        chMtxLock(mutex_);
    }
    ~MutexLock() noexcept { chMtxUnlock(); }

    // Non-copyable, non-movable
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;

private:
    Mutex* mutex_;
};

} // namespace raii

namespace ui::apps::enhanced_drone_analyzer {

// The following classes are available from eda_locking.hpp:
// - CriticalSection: RAII wrapper for ChibiOS critical sections
// - ThreadGuard: RAII wrapper for ChibiOS thread lifecycle
// - MutexInitializer: RAII wrapper for ChibiOS mutex initialization
// - OrderedScopedLock: RAII lock wrapper with deadlock prevention
// - TwoPhaseLock: Two-phase lock for long operations
// - StaticStorage: Zero-heap deferred initialization pattern

// Type aliases are also available from eda_locking.hpp:
// - ScopedLock: Alias for OrderedScopedLock<Mutex, false>
// - MutexLock: Alias for ScopedLock
// - MutexTryLock: Alias for OrderedScopedLock<Mutex, true>
// - SDCardLock: Alias for ScopedLock

// ============================================================
// DEPRECATED: Removed to fix duplicate definitions
// Reason: Duplicate with eda_locking.hpp, uses std::atomic (not supported)
// ============================================================
// The following code was originally in this file but has been moved to
// eda_locking.hpp to avoid duplication and compilation errors.
// It is preserved here for reference purposes only.

// ============================================================
// DEPRECATED: Removed <atomic> include
// Reason: std::atomic not supported on STM32F405/ChibiOS
// ============================================================
// #include <atomic>

// ============================================================
// DEPRECATED: LockOrder enum
// Reason: Moved to eda_locking.hpp to avoid duplicate definitions
// ============================================================
// /**
//  * @brief Lock ordering levels to prevent deadlock
//  *
//  * LOCK ORDER RULE:
//  * Always acquire locks in ascending order (1 → 2 → 3 → 4 → 5)
//  * Never acquire a lower-numbered lock while holding a higher-numbered lock
//  */
// enum class LockOrder : uint8_t {
//     ATOMIC_FLAGS = 1,      // volatile bool, volatile uint32_t - Protected by ChibiOS critical sections
//     DATA_MUTEX = 2,         // DroneScanner::data_mutex (tracked_drones_)
//     SPECTRUM_MUTEX = 3,    // DroneHardwareController::spectrum_mutex (spectrum_buffer_)
//     LOGGER_MUTEX = 4,       // DroneDetectionLogger::mutex_ (ring_buffer_)
//     SD_CARD_MUTEX = 5,      // Global sd_card_mutex (FatFS operations)
//     SETTINGS_MUTEX = 6,      // Global settings_buffer_mutex (settings I/O)
//     ERRNO_MUTEX = 7         // Global errno_mutex (thread-safe errno access)
// };

// ============================================================
// DEPRECATED: LockResult enum
// Reason: Moved to eda_locking.hpp to avoid duplicate definitions
// ============================================================
// /**
//  * @brief Result of lock acquisition attempt
//  */
// enum class LockResult : uint8_t {
//     SUCCESS = 0,           // Lock acquired successfully
//     WOULD_BLOCK = 1,       // TryLock would block (lock already held)
//     ORDER_VIOLATION = 2   // Lock order violation detected
// };

// ============================================================
// DEPRECATED: LockStack struct
// Reason: Moved to eda_locking.hpp to avoid duplicate definitions
// ============================================================
// /**
//  * @brief Maximum depth of nested lock tracking
//  */
// constexpr size_t MAX_LOCK_DEPTH = 8;
//
// /**
//  * @brief Lock stack entry for tracking nested locks
//  */
// struct LockStackEntry {
//     LockOrder order;
//     bool valid;
// };

// ============================================================
// DEPRECATED: LockOrderTracker class
// Reason: Moved to eda_locking.hpp to avoid duplicate definitions
// ============================================================
// /**
//  * @brief Tracks lock acquisition order to prevent deadlocks
//  *
//  * Uses thread-local storage to maintain a stack of held locks.
//  * Validates that locks are always acquired in ascending order.
//  */
// class LockOrderTracker {
// public:
//     /**
//      * @brief Check if acquiring a lock would violate ordering rules
//      * @param order Lock order level to check
//      * @return true if order is valid, false otherwise
//      */
//     static bool validate_lock_order(LockOrder order) noexcept {
//         chSysLock();
//         LockOrder current_max = get_current_max_order();
//         bool valid = (order > current_max);
//         chSysUnlock();
//         return valid;
//     }
//
//     /**
//      * @brief Push a lock onto the stack
//      * @param order Lock order level
//      */
//     static void push_lock(LockOrder order) noexcept {
//         chSysLock();
//         if (lock_stack_depth_ < MAX_LOCK_DEPTH) {
//             lock_stack_[lock_stack_depth_].order = order;
//             lock_stack_[lock_stack_depth_].valid = true;
//             lock_stack_depth_++;
//         }
//         chSysUnlock();
//     }
//
//     /**
//      * @brief Pop a lock from the stack
//      */
//     static void pop_lock() noexcept {
//         chSysLock();
//         if (lock_stack_depth_ > 0) {
//             lock_stack_depth_--;
//             lock_stack_[lock_stack_depth_].valid = false;
//         }
//         chSysUnlock();
//     }
//
// private:
//     static thread_local LockStackEntry lock_stack_[MAX_LOCK_DEPTH];
//     static thread_local size_t lock_stack_depth_;
//
//     static LockOrder get_current_max_order() noexcept {
//         if (lock_stack_depth_ == 0) {
//             return LockOrder::ATOMIC_FLAGS;
//         }
//         return lock_stack_[lock_stack_depth_ - 1].order;
//     }
// };

// ============================================================
// DEPRECATED: ScopedLock class
// Reason: Moved to eda_locking.hpp as OrderedScopedLock
// ============================================================
// /**
//  * @brief RAII lock wrapper with deadlock prevention
//  *
//  * Tracks lock acquisition order to prevent deadlock violations.
//  * Automatically unlocks when going out of scope.
//  *
//  * @tparam MutexType The mutex type (typically mutex_t)
//  * @tparam TryLock If true, uses non-blocking try-lock
//  */
// template<typename MutexType, bool TryLock = false>
// class ScopedLock {
// public:
//     explicit ScopedLock(MutexType& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
//         : mtx_(mtx), order_(order), locked_(false) {
//
//         // Check lock order - ensure we're not violating the ordering rule
//         chSysLock();
//         LockOrder current_max = get_current_max_order();
//         if (order_ > current_max) {
//             chSysUnlock();
//
//             if constexpr (TryLock) {
//                 locked_ = (chMtxTryLock(&mtx_) == true);
//             } else {
//                 chMtxLock(&mtx_);
//                 locked_ = true;
//             }
//
//             if (locked_) {
//                 push_lock(order_);
//             }
//         } else {
//             chSysUnlock();
//             chDbgPanic("Lock order violation detected");
//         }
//     }
//
//     ~ScopedLock() noexcept {
//         if (locked_) {
//             chMtxUnlock();
//             pop_lock();
//         }
//     }
//
//     bool is_locked() const noexcept { return locked_; }
//
//     ScopedLock(const ScopedLock&) = delete;
//     ScopedLock& operator=(const ScopedLock&) = delete;
//     ScopedLock(ScopedLock&&) = delete;
//     ScopedLock& operator=(ScopedLock&&) = delete;
//
// private:
//     MutexType& mtx_;
//     LockOrder order_;
//     bool locked_;
//
//     static thread_local LockStackEntry lock_stack_[MAX_LOCK_DEPTH];
//     static thread_local size_t lock_stack_depth_;
//
//     static LockOrder get_current_max_order() noexcept {
//         if (lock_stack_depth_ == 0) {
//             return LockOrder::ATOMIC_FLAGS;
//         }
//         return lock_stack_[lock_stack_depth_ - 1].order;
//     }
//
//     void push_lock(LockOrder order) noexcept {
//         chSysLock();
//         if (lock_stack_depth_ < MAX_LOCK_DEPTH) {
//             lock_stack_[lock_stack_depth_].order = order;
//             lock_stack_[lock_stack_depth_].valid = true;
//             lock_stack_depth_++;
//         }
//         chSysUnlock();
//     }
//
//     void pop_lock() noexcept {
//         chSysLock();
//         if (lock_stack_depth_ > 0) {
//             lock_stack_depth_--;
//             lock_stack_[lock_stack_depth_].valid = false;
//         }
//         chSysUnlock();
//     }
// };

// ============================================================
// DEPRECATED: Type aliases
// Reason: Moved to eda_locking.hpp to avoid duplicate definitions
// ============================================================
// // CRITICAL FIX: Use ChibiOS Mutex type (capital M, not mutex_t)
// using ScopedLock = OrderedScopedLock<Mutex, false>;
// using MutexLock = ScopedLock;
// using MutexTryLock = OrderedScopedLock<Mutex, true>;
// using SDCardLock = ScopedLock;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_RAII_HPP_
