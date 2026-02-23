#ifndef EDA_RAII_HPP_
#define EDA_RAII_HPP_

// Include the canonical locking header
#include "eda_locking.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Backward compatibility: All RAII wrappers are now defined in eda_locking.hpp

} // namespace ui::apps::enhanced_drone_analyzer

namespace raii {

// RAII wrapper for ChibiOS system lock (critical section)
// Must be used for very short sections only - no blocking operations allowed
class SystemLock {
public:
    SystemLock() noexcept { chSysLock(); }
    ~SystemLock() noexcept { chSysUnlock(); }

    // Non-copyable, non-movable
    SystemLock(const SystemLock&) = delete;
    SystemLock& operator=(const SystemLock&) = delete;
};

} // namespace raii

namespace ui::apps::enhanced_drone_analyzer {

// Atomic flag using volatile bool with ChibiOS critical sections
// Provides atomic operations on a boolean flag without using std::atomic
// Uses ChibiOS critical sections (chSysLock/chSysUnlock) for synchronization
// DIAMOND FIX: Correct compare_and_swap implementation to prevent race condition
class AtomicFlag {
public:
    // Default constructor - initializes to false
    constexpr AtomicFlag() noexcept : value_(false) {}

    // Constructor with initial value
    constexpr explicit AtomicFlag(bool initial) noexcept : value_(initial) {}

    // Get current value
    // Volatile bool reads are atomic on ARM Cortex-M
    bool get() const noexcept {
        return value_;
    }

    // Set flag to specified value (thread-safe: uses critical section)
    void set(bool value) noexcept {
        chSysLock();
        value_ = value;
        chSysUnlock();
    }

    // Atomic compare-and-swap operation
    // DIAMOND FIX: Original implementation had race condition - fixed with atomic read/write in critical section
    bool compare_and_swap(bool expected, bool desired) noexcept {
        chSysLock();
        bool actual = value_;  // Atomic read
        if (actual == expected) {
            value_ = desired;  // Atomic write
        }
        chSysUnlock();
        return actual == expected;
    }

    // Test-and-set operation (thread-safe: uses critical section)
    bool test_and_set() noexcept {
        chSysLock();
        bool previous = value_;
        value_ = true;
        chSysUnlock();
        return previous;
    }

    // Clear flag (set to false) - thread-safe via critical section
    void clear() noexcept {
        chSysLock();
        value_ = false;
        chSysUnlock();
    }

    // Non-copyable
    AtomicFlag(const AtomicFlag&) = delete;
    AtomicFlag& operator=(const AtomicFlag&) = delete;

private:
    // Flag value (volatile for atomicity on ARM Cortex-M)
    volatile bool value_;
};

} // namespace ui::apps::enhanced_drone_analyzer

namespace raii {

// RAII wrapper for ChibiOS mutex lock
// Provides exception-safe mutex management - locks on construction, unlocks on destruction
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

// The following code was originally in this file but has been moved to
// eda_locking.hpp to avoid duplication and compilation errors.
// It is preserved here for reference purposes only.

// std::atomic not supported on STM32F405/ChibiOS

// Lock ordering levels to prevent deadlock
// LOCK ORDER RULE: Always acquire locks in ascending order (1 < 2 < 3 < 4 < 5)
// Never acquire a lower-numbered lock while holding a higher-numbered lock
// enum class LockOrder : uint8_t {
// ATOMIC_FLAGS = 1,      // volatile bool, volatile uint32_t - Protected by ChibiOS critical sections
// DATA_MUTEX = 2,         // DroneScanner::data_mutex (tracked_drones_)
// SPECTRUM_MUTEX = 3,    // DroneHardwareController::spectrum_mutex (spectrum_buffer_)
// LOGGER_MUTEX = 4,       // DroneDetectionLogger::mutex_ (ring_buffer_)
// SD_CARD_MUTEX = 5,      // Global sd_card_mutex (FatFS operations)
// SETTINGS_MUTEX = 6,      // Global settings_buffer_mutex (settings I/O)
// ERRNO_MUTEX = 7         // Global errno_mutex (thread-safe errno access)
// };

// Result of lock acquisition attempt
// enum class LockResult : uint8_t {
// SUCCESS = 0,           // Lock acquired successfully
// WOULD_BLOCK = 1,       // TryLock would block (lock already held)
// ORDER_VIOLATION = 2   // Lock order violation detected
// };

// Maximum depth of nested lock tracking
// constexpr size_t MAX_LOCK_DEPTH = 8;
// Lock stack entry for tracking nested locks
// struct LockStackEntry {
// LockOrder order;
// bool valid;
// };

// Tracks lock acquisition order to prevent deadlocks
// Uses thread-local storage to maintain a stack of held locks
// class LockOrderTracker {
// public:
// static bool validate_lock_order(LockOrder order) noexcept {
// chSysLock();
// LockOrder current_max = get_current_max_order();
// bool valid = (order > current_max);
// chSysUnlock();
// return valid;
// }
// static void push_lock(LockOrder order) noexcept {
// chSysLock();
// if (lock_stack_depth_ < MAX_LOCK_DEPTH) {
// lock_stack_[lock_stack_depth_].order = order;
// lock_stack_[lock_stack_depth_].valid = true;
// lock_stack_depth_++;
// }
// chSysUnlock();
// }
// static void pop_lock() noexcept {
// chSysLock();
// if (lock_stack_depth_ > 0) {
// lock_stack_depth_--;
// lock_stack_[lock_stack_depth_].valid = false;
// }
// chSysUnlock();
// }
// private:
// static thread_local LockStackEntry lock_stack_[MAX_LOCK_DEPTH];
// static thread_local size_t lock_stack_depth_;
// static LockOrder get_current_max_order() noexcept {
// if (lock_stack_depth_ == 0) {
// return LockOrder::ATOMIC_FLAGS;
// }
// return lock_stack_[lock_stack_depth_ - 1].order;
// }
// };

// RAII lock wrapper with deadlock prevention
// Tracks lock acquisition order to prevent deadlock violations
// Automatically unlocks when going out of scope
// template<typename MutexType, bool TryLock = false>
// class ScopedLock {
// public:
// explicit ScopedLock(MutexType& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
// : mtx_(mtx), order_(order), locked_(false) {
// // Check lock order - ensure we're not violating the ordering rule
// chSysLock();
// LockOrder current_max = get_current_max_order();
// if (order_ > current_max) {
// chSysUnlock();
// if constexpr (TryLock) {
// locked_ = (chMtxTryLock(&mtx_) == true);
// } else {
// chMtxLock(&mtx_);
// locked_ = true;
// }
// if (locked_) {
// push_lock(order_);
// }
// } else {
// chSysUnlock();
// chDbgPanic("Lock order violation detected");
// }
// }
// ~ScopedLock() noexcept {
// if (locked_) {
// chMtxUnlock();
// pop_lock();
// }
// }
// bool is_locked() const noexcept { return locked_; }
// ScopedLock(const ScopedLock&) = delete;
// ScopedLock& operator=(const ScopedLock&) = delete;
// ScopedLock(ScopedLock&&) = delete;
// ScopedLock& operator=(ScopedLock&&) = delete;
// private:
// MutexType& mtx_;
// LockOrder order_;
// bool locked_;
// static thread_local LockStackEntry lock_stack_[MAX_LOCK_DEPTH];
// static thread_local size_t lock_stack_depth_;
// static LockOrder get_current_max_order() noexcept {
// if (lock_stack_depth_ == 0) {
// return LockOrder::ATOMIC_FLAGS;
// }
// return lock_stack_[lock_stack_depth_ - 1].order;
// }
// void push_lock(LockOrder order) noexcept {
// chSysLock();
// if (lock_stack_depth_ < MAX_LOCK_DEPTH) {
// lock_stack_[lock_stack_depth_].order = order;
// lock_stack_[lock_stack_depth_].valid = true;
// lock_stack_depth_++;
// }
// chSysUnlock();
// }
// void pop_lock() noexcept {
// chSysLock();
// if (lock_stack_depth_ > 0) {
// lock_stack_depth_--;
// lock_stack_[lock_stack_depth_].valid = false;
// }
// chSysUnlock();
// }
// };

// CRITICAL FIX: Use ChibiOS Mutex type (capital M, not mutex_t)
// using ScopedLock = OrderedScopedLock<Mutex, false>;
// using MutexLock = ScopedLock;
// using MutexTryLock = OrderedScopedLock<Mutex, true>;
// using SDCardLock = ScopedLock;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_RAII_HPP_
