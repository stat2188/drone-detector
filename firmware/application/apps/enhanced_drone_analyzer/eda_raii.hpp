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

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_RAII_HPP_
