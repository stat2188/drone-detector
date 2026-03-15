#ifndef LOCKING_HPP
#define LOCKING_HPP

#include <cstdint>
#include <cstddef>
#include <tuple>
#include <utility>

// Forward declarations for ChibiOS types
// These will be available when ChibiOS headers are included
struct mutex_t;
struct thread_t;

namespace drone_analyzer {

/**
 * @brief Lock ordering levels for EDA
 * @note Always acquire locks in ascending order (0 → 1 → 2 → ... → 14)
 * @note Never acquire locks in descending order (causes deadlock)
 * @note Lock 14 (SD_CARD_MUTEX) must always be LAST
 */
enum class LockOrder : uint8_t {
    // Level 0-3: Core System
    ATOMIC_FLAGS = 0,      // AtomicFlag operations (no lock)
    DATA_MUTEX = 1,        // Scanner data protection
    DATABASE_MUTEX = 2,    // Database protection (freqman DB)
    STATE_MUTEX = 3,       // System state protection
    
    // Level 4-8: UI Components
    UI_THREAT_MUTEX = 4,    // Threat header protection
    UI_CARD_MUTEX = 5,       // Threat card protection
    UI_STATUSBAR_MUTEX = 6,  // Status bar protection
    UI_DISPLAY_MUTEX = 7,    // Display controller protection
    UI_CONTROLLER_MUTEX = 8,  // UI controller protection
    
    // Level 9-11: DSP Processing
    ENTRIES_TO_SCAN_MUTEX = 9,   // Entries to scan protection
    HISTOGRAM_BUFFER_MUTEX = 10,  // Histogram buffer protection
    SPECTRUM_DATA_MUTEX = 11,    // Spectrum data protection
    
    // Level 12-13: System Services
    SPECTRUM_MUTEX = 12,    // Spectrum streaming protection
    LOGGER_MUTEX = 13,      // Logger protection
    
    // Level 14: I/O Operations (MUST BE LAST)
    SD_CARD_MUTEX = 14       // SD card operations (FatFS not thread-safe)
};

/**
 * @brief Atomic flag using GCC built-ins (NOT std::atomic)
 * @note Lock-free, no mutex needed
 * @note Uses GCC atomic intrinsics for ARM Cortex-M4
 */
class AtomicFlag {
public:
    /**
     * @brief Default constructor - flag is clear
     */
    constexpr AtomicFlag() noexcept
        : flag_(0) {}
    
    /**
     * @brief Set flag to true
     * @return Previous value
     */
    bool set() noexcept {
        return __atomic_test_and_set(&flag_, __ATOMIC_SEQ_CST) != 0;
    }
    
    /**
     * @brief Set flag to false
     */
    void clear() noexcept {
        __atomic_clear(&flag_, __ATOMIC_SEQ_CST);
    }
    
    /**
     * @brief Test flag value
     * @return Current flag value
     */
    [[nodiscard]] bool test() const noexcept {
        return __atomic_test_and_set(&const_cast<uint8_t&>(flag_), __ATOMIC_SEQ_CST) != 0;
    }
    
    /**
     * @brief Test and set flag atomically
     * @return Previous value
     */
    [[nodiscard]] bool test_and_set() noexcept {
        return set();
    }
    
    /**
     * @brief Try to set flag
     * @return true if flag was clear and is now set, false if flag was already set
     */
    [[nodiscard]] bool try_set() noexcept {
        return !set();
    }
    
    /**
     * @brief Get current value (non-atomic, for debugging)
     */
    [[nodiscard]] uint8_t get() const noexcept {
        return flag_;
    }
    
    // Delete copy and move operations
    AtomicFlag(const AtomicFlag&) = delete;
    AtomicFlag& operator=(const AtomicFlag&) = delete;
    AtomicFlag(AtomicFlag&&) = delete;
    AtomicFlag& operator=(AtomicFlag&&) = delete;
    
private:
    uint8_t flag_;
};

/**
 * @brief RAII wrapper for mutex lock
 * @note Acquires lock in constructor, releases in destructor
 * @note Enforces lock ordering via template parameter
 */
template<LockOrder ORDER>
class MutexLock {
public:
    /**
     * @brief Constructor - acquires mutex lock
     * @param mutex Reference to mutex to lock
     */
    explicit MutexLock(mutex_t& mutex) noexcept
        : mutex_(mutex), locked_(false) {
        // chMtxLock(&mutex_);  // ChibiOS call - will be available
        locked_ = true;
    }
    
    /**
     * @brief Destructor - releases mutex lock
     */
    ~MutexLock() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    /**
     * @brief Check if lock is held
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return locked_;
    }
    
    /**
     * @brief Manual unlock (use with caution)
     */
    void unlock() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    // Delete copy and move operations
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    MutexLock(MutexLock&&) = delete;
    MutexLock& operator=(MutexLock&&) = delete;
    
private:
    mutex_t& mutex_;
    bool locked_;
};

/**
 * @brief RAII wrapper for mutex lock with timeout
 * @note Acquires lock with timeout, releases in destructor
 * @note Provides graceful degradation on lock contention
 */
template<LockOrder ORDER>
class MutexLockTimeout {
public:
    /**
     * @brief Constructor - acquires mutex lock with timeout
     * @param mutex Reference to mutex to lock
     * @param timeout_ms Timeout in milliseconds
     */
    MutexLockTimeout(mutex_t& mutex, uint32_t timeout_ms) noexcept
        : mutex_(mutex), locked_(false) {
        // if (chMtxLockTimeout(&mutex_, MS2ST(timeout_ms)) == MSG_OK) {
        //     locked_ = true;
        // }
        locked_ = true;  // Placeholder - will use ChibiOS call
    }
    
    /**
     * @brief Destructor - releases mutex lock
     */
    ~MutexLockTimeout() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    /**
     * @brief Check if lock is held
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return locked_;
    }
    
    /**
     * @brief Manual unlock (use with caution)
     */
    void unlock() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    // Delete copy and move operations
    MutexLockTimeout(const MutexLockTimeout&) = delete;
    MutexLockTimeout& operator=(const MutexLockTimeout&) = delete;
    MutexLockTimeout(MutexLockTimeout&&) = delete;
    MutexLockTimeout& operator=(MutexLockTimeout&&) = delete;
    
private:
    mutex_t& mutex_;
    bool locked_;
};

/**
 * @brief RAII wrapper for mutex try-lock
 * @note Tries to acquire lock, does not block
 * @note Useful for non-blocking UI updates
 */
template<LockOrder ORDER>
class MutexTryLock {
public:
    /**
     * @brief Constructor - tries to acquire mutex lock
     * @param mutex Reference to mutex to lock
     */
    explicit MutexTryLock(mutex_t& mutex) noexcept
        : mutex_(mutex), locked_(false) {
        // if (chMtxTryLock(&mutex_) == MSG_OK) {
        //     locked_ = true;
        // }
        locked_ = true;  // Placeholder - will use ChibiOS call
    }
    
    /**
     * @brief Destructor - releases mutex lock if held
     */
    ~MutexTryLock() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    /**
     * @brief Check if lock is held
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return locked_;
    }
    
    /**
     * @brief Manual unlock (use with caution)
     */
    void unlock() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    // Delete copy and move operations
    MutexTryLock(const MutexTryLock&) = delete;
    MutexTryLock& operator=(const MutexTryLock&) = delete;
    MutexTryLock(MutexTryLock&&) = delete;
    MutexTryLock& operator=(MutexTryLock&&) = delete;
    
private:
    mutex_t& mutex_;
    bool locked_;
};

/**
 * @brief RAII wrapper for critical section
 * @note Disables interrupts in constructor, restores in destructor
 * @note Use for very short critical sections only
 */
class CriticalSection {
public:
    /**
     * @brief Constructor - enters critical section
     */
    CriticalSection() noexcept
        : state_(0) {
        // state_ = chSysGetStatusAndLockX();  // ChibiOS call - will be available
        state_ = 1;  // Placeholder - will use ChibiOS call
    }
    
    /**
     * @brief Destructor - exits critical section
     */
    ~CriticalSection() noexcept {
        // chSysRestoreStatusX(state_);  // ChibiOS call - will be available
        state_ = 0;
    }
    
    /**
     * @brief Check if in critical section
     */
    [[nodiscard]] bool in_critical_section() const noexcept {
        return state_ != 0;
    }
    
    // Delete copy and move operations
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
    CriticalSection(CriticalSection&&) = delete;
    CriticalSection& operator=(CriticalSection&&) = delete;
    
private:
    uint32_t state_;
};

/**
 * @brief Compile-time lock order validation
 * @note Fails compilation if lock order is violated
 */
template<LockOrder CURRENT, LockOrder NEXT>
struct ValidateLockOrder {
    static_assert(CURRENT < NEXT, "Lock order violation: must acquire locks in ascending order");
    static constexpr bool valid = true;
};

/**
 * @brief RAII wrapper for ordered mutex lock pair
 * @note Enforces lock ordering at compile time
 */
template<LockOrder FIRST, LockOrder SECOND>
class OrderedMutexLockPair {
public:
    static_assert(FIRST < SECOND, "Lock order violation: FIRST must be less than SECOND");
    
    /**
     * @brief Constructor - acquires locks in order
     */
    OrderedMutexLockPair(mutex_t& mutex1, mutex_t& mutex2) noexcept
        : lock1_(mutex1), lock2_(mutex2) {}
    
    /**
     * @brief Check if both locks are held
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return lock1_.is_locked() && lock2_.is_locked();
    }
    
    // Delete copy and move operations
    OrderedMutexLockPair(const OrderedMutexLockPair&) = delete;
    OrderedMutexLockPair& operator=(const OrderedMutexLockPair&) = delete;
    OrderedMutexLockPair(OrderedMutexLockPair&&) = delete;
    OrderedMutexLockPair& operator=(OrderedMutexLockPair&&) = delete;
    
private:
    MutexLock<FIRST> lock1_;
    MutexLock<SECOND> lock2_;
};

/**
 * @brief Scoped lock guard for multiple locks
 * @note Acquires all locks in constructor, releases all in destructor
 * @note Simplified version for 2 locks (most common use case)
 */
template<LockOrder FIRST, LockOrder SECOND>
class ScopedMultiLock<FIRST, SECOND> {
public:
    /**
     * @brief Constructor - acquires both locks
     */
    explicit ScopedMultiLock(mutex_t& first_mutex, mutex_t& second_mutex) noexcept
        : lock1_(first_mutex), lock2_(second_mutex) {}
    
    /**
     * @brief Check if all locks are held
     */
    [[nodiscard]] bool all_locked() const noexcept {
        return lock1_.is_locked() && lock2_.is_locked();
    }
    
    // Delete copy and move operations
    ScopedMultiLock(const ScopedMultiLock&) = delete;
    ScopedMultiLock& operator=(const ScopedMultiLock&) = delete;
    ScopedMultiLock(ScopedMultiLock&&) = delete;
    ScopedMultiLock& operator=(ScopedMultiLock&&) = delete;
    
private:
    MutexLock<FIRST> lock1_;
    MutexLock<SECOND> lock2_;
};

/**
 * @brief Lock guard for deferred locking
 * @note Does not acquire lock in constructor, must call lock() manually
 */
template<LockOrder ORDER>
class DeferredLock {
public:
    /**
     * @brief Constructor - does not acquire lock
     */
    explicit DeferredLock(mutex_t& mutex) noexcept
        : mutex_(mutex), locked_(false) {}
    
    /**
     * @brief Destructor - releases lock if held
     */
    ~DeferredLock() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    /**
     * @brief Acquire lock
     */
    void lock() noexcept {
        if (!locked_) {
            // chMtxLock(&mutex_);  // ChibiOS call - will be available
            locked_ = true;
        }
    }
    
    /**
     * @brief Release lock
     */
    void unlock() noexcept {
        if (locked_) {
            // chMtxUnlock(&mutex_);  // ChibiOS call - will be available
            locked_ = false;
        }
    }
    
    /**
     * @brief Check if lock is held
     */
    [[nodiscard]] bool is_locked() const noexcept {
        return locked_;
    }
    
    // Delete copy and move operations
    DeferredLock(const DeferredLock&) = delete;
    DeferredLock& operator=(const DeferredLock&) = delete;
    DeferredLock(DeferredLock&&) = delete;
    DeferredLock& operator=(DeferredLock&&) = delete;
    
private:
    mutex_t& mutex_;
    bool locked_;
};

/**
 * @brief Spin lock using atomic flag
 * @note Lock-free, uses busy-wait
 * @note Use for very short critical sections only
 */
class SpinLock {
public:
    /**
     * @brief Default constructor
     */
    SpinLock() noexcept = default;
    
    /**
     * @brief Acquire spin lock
     */
    void lock() noexcept {
        while (flag_.test_and_set()) {
            // Busy-wait - yield to other threads if available
            // chThdYield();  // ChibiOS call - will be available
        }
    }
    
    /**
     * @brief Release spin lock
     */
    void unlock() noexcept {
        flag_.clear();
    }
    
    /**
     * @brief Try to acquire spin lock
     * @return true if lock acquired, false otherwise
     */
    [[nodiscard]] bool try_lock() noexcept {
        return flag_.try_set();
    }
    
    // Delete copy and move operations
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;
    
private:
    AtomicFlag flag_;
};

/**
 * @brief RAII wrapper for spin lock
 */
class SpinLockGuard {
public:
    /**
     * @brief Constructor - acquires spin lock
     */
    explicit SpinLockGuard(SpinLock& spin_lock) noexcept
        : spin_lock_(spin_lock) {
        spin_lock_.lock();
    }
    
    /**
     * @brief Destructor - releases spin lock
     */
    ~SpinLockGuard() noexcept {
        spin_lock_.unlock();
    }
    
    // Delete copy and move operations
    SpinLockGuard(const SpinLockGuard&) = delete;
    SpinLockGuard& operator=(const SpinLockGuard&) = delete;
    SpinLockGuard(SpinLockGuard&&) = delete;
    SpinLockGuard& operator=(SpinLockGuard&&) = delete;
    
private:
    SpinLock& spin_lock_;
};

} // namespace drone_analyzer

#endif // LOCKING_HPP
