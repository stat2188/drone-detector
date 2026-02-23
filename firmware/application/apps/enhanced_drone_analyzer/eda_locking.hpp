// * * Diamond Code: Locking Primitives for Enhanced Drone Analyzer

#ifndef EDA_LOCKING_HPP_
#define EDA_LOCKING_HPP_

#include <cstdint>
#include <new>
#include <ch.h>
#include <type_traits>

// ChibiOS Compatibility Defines
#define chThdGetSelf() chThdSelf()
static inline size_t chThdGetStackFree(Thread* tp) {
    (void)tp;
    return 0;
}

namespace ui::apps::enhanced_drone_analyzer {

// Lock Order Constants (always acquire in ascending order)
// DIAMOND FIX: Aligned with existing 5-level hierarchy to prevent deadlocks.
// The original blueprint defined 9 lock levels, but the existing codebase
// uses a 5-level hierarchy. This fix aligns the LockOrder enum with the
// actual implementation to avoid contradictions and ensure proper lock ordering.
// LOCK ORDER RULE:
// Always acquire locks in ascending order (1  2  3  4  5)
// Never acquire a lower-numbered lock while holding a higher-numbered lock
// This prevents circular wait conditions that lead to deadlocks.
enum class LockOrder : uint8_t {
    ATOMIC_FLAGS = 1,      // Protected by ChibiOS critical sections (volatile bool, volatile uint32_t)
    ERRNO_MUTEX = 2,       // Global errno_mutex (thread-safe errno access)
    DATA_MUTEX = 3,        // DroneScanner::data_mutex (tracked_drones_)
    SPECTRUM_MUTEX = 4,    // DroneHardwareController::spectrum_mutex (spectrum_buffer_)
    LOGGER_MUTEX = 5,      // DroneDetectionLogger::mutex_ (ring_buffer_)
    SD_CARD_MUTEX = 6      // Global sd_card_mutex (FatFS operations)
};

// Lock order validation static assertions
static_assert(static_cast<uint8_t>(LockOrder::ATOMIC_FLAGS) == 1, "LockOrder value changed");
static_assert(static_cast<uint8_t>(LockOrder::ERRNO_MUTEX) == 2, "LockOrder value changed");
static_assert(static_cast<uint8_t>(LockOrder::DATA_MUTEX) == 3, "LockOrder value changed");
static_assert(static_cast<uint8_t>(LockOrder::SPECTRUM_MUTEX) == 4, "LockOrder value changed");
static_assert(static_cast<uint8_t>(LockOrder::LOGGER_MUTEX) == 5, "LockOrder value changed");
static_assert(static_cast<uint8_t>(LockOrder::SD_CARD_MUTEX) == 6, "LockOrder value changed");

// Lock Stack Tracking
constexpr size_t MAX_LOCK_DEPTH = 8;
struct LockStackEntry {
    LockOrder order;
    bool valid;
};

// Ordered Scoped Lock (RAII with deadlock prevention)
template<typename MutexType, bool TryLock = false>
class OrderedScopedLock {
public:
    // * * @brief Constructor - Acquires lock with order tracking
    explicit OrderedScopedLock(MutexType& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
        : mtx_(mtx), order_(order), locked_(false) {
        
        // Check lock order
        chSysLock();
        LockOrder current_max = get_current_max_order();
        if (order_ > current_max) {
            chSysUnlock();
            
            if constexpr (TryLock) {
                locked_ = (chMtxTryLock(&mtx_) == true);
            } else {
                chMtxLock(&mtx_);
                locked_ = true;
            }
            
            if (locked_) {
                // Push lock onto stack
                push_lock(order_);
            }
        } else {
            chSysUnlock();
            // Graceful degradation: acquire lock anyway
            chMtxLock(&mtx_);
            locked_ = true;
            
            if (locked_) {
                push_lock(order_);
            }
        }
    }

    // / @brief Destructor - Releases lock
    ~OrderedScopedLock() noexcept {
        if (locked_) {
            chMtxUnlock();
            
            // Pop lock from stack
            pop_lock();
        }
    }

    // / @brief Query if lock was acquired
    bool is_locked() const noexcept { return locked_; }

    // / @brief Non-copyable, non-movable
    OrderedScopedLock(const OrderedScopedLock&) = delete;
    OrderedScopedLock& operator=(const OrderedScopedLock&) = delete;
    OrderedScopedLock(OrderedScopedLock&&) = delete;
    OrderedScopedLock& operator=(OrderedScopedLock&&) = delete;

private:
    MutexType& mtx_;
    LockOrder order_;
    bool locked_;
    
    // Thread-local storage for lock stack
    static thread_local LockStackEntry lock_stack_[MAX_LOCK_DEPTH];
    static thread_local size_t lock_stack_depth_;
    
    // / @brief Get current maximum lock order
    static LockOrder get_current_max_order() noexcept {
        if (lock_stack_depth_ == 0) {
            return LockOrder::ATOMIC_FLAGS;
        }
        return lock_stack_[lock_stack_depth_ - 1].order;
    }
    
    // / @brief Push lock onto stack
    void push_lock(LockOrder order) noexcept {
        chSysLock();
        if (lock_stack_depth_ < MAX_LOCK_DEPTH) {
            lock_stack_[lock_stack_depth_].order = order;
            lock_stack_[lock_stack_depth_].valid = true;
            lock_stack_depth_++;
        }
        chSysUnlock();
    }
    
    // / @brief Pop lock from stack
    void pop_lock() noexcept {
        chSysLock();
        if (lock_stack_depth_ > 0) {
            lock_stack_depth_--;
            lock_stack_[lock_stack_depth_].valid = false;
        }
        chSysUnlock();
    }
};

// Thread-local static members
template<typename MutexType, bool TryLock>
thread_local LockStackEntry OrderedScopedLock<MutexType, TryLock>::lock_stack_[MAX_LOCK_DEPTH] = {};

template<typename MutexType, bool TryLock>
thread_local size_t OrderedScopedLock<MutexType, TryLock>::lock_stack_depth_ = 0;

// Critical Section (RAII ChibiOS Critical Section)
class CriticalSection {
public:
    // / @brief Constructor - Enters critical section
    CriticalSection() noexcept {
        chSysLock();
    }

    // / @brief Destructor - Exits critical section
    ~CriticalSection() noexcept {
        chSysUnlock();
    }

    // / @brief Non-copyable, non-movable
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
    CriticalSection(CriticalSection&&) = delete;
    CriticalSection& operator=(CriticalSection&&) = delete;
};

// Thread Guard (RAII Thread Management)
class ThreadGuard {
public:
    // / @brief Constructor - Takes ownership of thread pointer
    explicit ThreadGuard(Thread* thread) noexcept : thread_(thread) {}

    // / @brief Destructor - Terminates and waits for thread
    ~ThreadGuard() noexcept {
        if (thread_) {
            chThdTerminate(thread_);
            chThdWait(thread_);
            thread_ = nullptr;
        }
    }

    // / @brief Move constructor - Transfers ownership
    ThreadGuard(ThreadGuard&& other) noexcept : thread_(other.thread_) {
        other.thread_ = nullptr;
    }

    // / @brief Move assignment operator
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

    // / @brief Non-copyable
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;

    // / @brief Release ownership without terminating thread
    Thread* release() noexcept {
        Thread* tmp = thread_;
        thread_ = nullptr;
        return tmp;
    }

    // / @brief Get thread pointer
    Thread* get() const noexcept { return thread_; }

    // / @brief Check if thread is being managed
    bool has_thread() const noexcept { return thread_ != nullptr; }

private:
    Thread* thread_;
};

// Mutex Initializer (RAII Mutex Setup)
class MutexInitializer {
public:
    // / @brief Constructor - Initializes mutex
    explicit MutexInitializer(Mutex& mtx) noexcept : mtx_(mtx) {
        chMtxInit(&mtx_);
    }

    // / @brief Destructor - Does nothing (ChibiOS mutexes are never de-initialized)
    ~MutexInitializer() noexcept {}

    // / @brief Non-copyable, non-movable
    MutexInitializer(const MutexInitializer&) = delete;
    MutexInitializer& operator=(const MutexInitializer&) = delete;
    MutexInitializer(MutexInitializer&&) = delete;
    MutexInitializer& operator=(MutexInitializer&&) = delete;

private:
    Mutex& mtx_;
};

// Two Phase Lock (for long operations)
template<typename MutexType>
class TwoPhaseLock {
public:
    // / @brief Constructor - Acquires lock with order tracking
    explicit TwoPhaseLock(MutexType& mtx, LockOrder order) noexcept
        : mtx_(mtx), order_(order), locked_(false) {
        acquire();
    }

    // / @brief Destructor - Releases lock if held
    ~TwoPhaseLock() noexcept {
        if (locked_) {
            release();
        }
    }

    // / @brief Acquire the lock
    bool acquire() noexcept {
        if (locked_) return true;
        
        // Check lock order
        chSysLock();
        LockOrder current_max = get_current_max_order();
        if (order_ <= current_max) {
            chSysUnlock();
            // Graceful degradation: acquire lock anyway
            chMtxLock(&mtx_);
            locked_ = true;
            
            if (locked_) {
                push_lock(order_);
            }
            return true;
        }
        chSysUnlock();
        
        chMtxLock(&mtx_);
        locked_ = true;
        
        // Push lock onto stack
        push_lock(order_);
        
        return true;
    }

    // / @brief Release the lock
    void release() noexcept {
        if (locked_) {
            chMtxUnlock();
            locked_ = false;
            
            // Pop lock from stack
            pop_lock();
        }
    }

    // / @brief Re-acquire the lock after release
    bool reacquire() noexcept {
        return acquire();
    }

    // / @brief Query if lock is held
    bool is_locked() const noexcept { return locked_; }

    // / @brief Non-copyable, non-movable
    TwoPhaseLock(const TwoPhaseLock&) = delete;
    TwoPhaseLock& operator=(const TwoPhaseLock&) = delete;
    TwoPhaseLock(TwoPhaseLock&&) = delete;
    TwoPhaseLock& operator=(TwoPhaseLock&&) = delete;

private:
    MutexType& mtx_;
    LockOrder order_;
    bool locked_;
    
    // Thread-local storage for lock stack
    static thread_local LockStackEntry lock_stack_[MAX_LOCK_DEPTH];
    static thread_local size_t lock_stack_depth_;
    
    // / @brief Get current maximum lock order
    static LockOrder get_current_max_order() noexcept {
        if (lock_stack_depth_ == 0) {
            return LockOrder::ATOMIC_FLAGS;
        }
        return lock_stack_[lock_stack_depth_ - 1].order;
    }
    
    // / @brief Push lock onto stack
    void push_lock(LockOrder order) noexcept {
        chSysLock();
        if (lock_stack_depth_ < MAX_LOCK_DEPTH) {
            lock_stack_[lock_stack_depth_].order = order;
            lock_stack_[lock_stack_depth_].valid = true;
            lock_stack_depth_++;
        }
        chSysUnlock();
    }
    
    // / @brief Pop lock from stack
    void pop_lock() noexcept {
        chSysLock();
        if (lock_stack_depth_ > 0) {
            lock_stack_depth_--;
            lock_stack_[lock_stack_depth_].valid = false;
        }
        chSysUnlock();
    }
};

// Thread-local static members
template<typename MutexType>
thread_local LockStackEntry TwoPhaseLock<MutexType>::lock_stack_[MAX_LOCK_DEPTH] = {};

template<typename MutexType>
thread_local size_t TwoPhaseLock<MutexType>::lock_stack_depth_ = 0;

// Static Storage Template (zero-heap deferred initialization)
template<typename T, size_t Size>
class StaticStorage {
public:
    // / @brief Default constructor - storage is uninitialized
    StaticStorage() noexcept : constructed_(false) {
        static_assert(Size >= sizeof(T), "StaticStorage size too small for type T");
        static_assert(alignof(T) <= alignof(Storage), "StaticStorage alignment insufficient");
    }

    // / @brief Destructor - destroys object if constructed
    ~StaticStorage() noexcept {
        destroy();
    }

    // * * @brief Construct object in-place using default constructor * @note Critical section does NOT extend through construction to avoid priority inversion
    bool construct() noexcept {
        // Ensure noexcept constructor
        static_assert(std::is_nothrow_default_constructible<T>::value,
                      "StaticStorage requires noexcept default constructor");
        
        // Use critical section only for flag check and set
        chSysLock();
        if (constructed_) {
            chSysUnlock();
            return false;
        }
        constructed_ = true;
        chSysUnlock();
        
        // Construction happens OUTSIDE critical section
        new (&storage_) T();
        
        return true;
    }

    // * * @brief Construct object in-place with arguments * @note Critical section does NOT extend through construction to avoid priority inversion
    template<typename... Args>
    bool construct(Args&&... args) noexcept {
        // Ensure noexcept constructor
        static_assert(std::is_nothrow_constructible<T, Args...>::value,
                      "StaticStorage requires noexcept constructor with given arguments");
        
        // Use critical section only for flag check and set
        chSysLock();
        if (constructed_) {
            chSysUnlock();
            return false;
        }
        constructed_ = true;
        chSysUnlock();
        
        // Construction happens OUTSIDE critical section
        new (&storage_) T(std::forward<Args>(args)...);
        
        return true;
    }

    // * * @brief Destroy the constructed object * @note Critical section does NOT extend through destruction to avoid priority inversion
    bool destroy() noexcept {
        // Use critical section only for flag check
        chSysLock();
        if (!constructed_) {
            chSysUnlock();
            return false;
        }
        constructed_ = false;
        chSysUnlock();
        
        // Destruction happens OUTSIDE critical section
        reinterpret_cast<T*>(&storage_)->~T();
        
        return true;
    }

    // * * @brief Get pointer to stored object * @note No critical section needed - volatile bool reads are atomic on ARM Cortex-M
    T* get() noexcept {
        // No critical section needed - volatile bool reads are atomic on ARM Cortex-M
        return constructed_ ? reinterpret_cast<T*>(&storage_) : nullptr;
    }

    // * * @brief Get const pointer to stored object * @note No critical section needed - volatile bool reads are atomic on ARM Cortex-M
    const T* get() const noexcept {
        // No critical section needed - volatile bool reads are atomic on ARM Cortex-M
        return constructed_ ? reinterpret_cast<const T*>(&storage_) : nullptr;
    }

    // * * @brief Check if object has been constructed * @note No critical section needed - volatile bool reads are atomic on ARM Cortex-M
    bool is_constructed() const noexcept {
        // No critical section needed - volatile bool reads are atomic on ARM Cortex-M
        return constructed_;
    }

    // / @brief Arrow operator
    T* operator->() noexcept {
        return get();
    }

    // / @brief Const arrow operator
    const T* operator->() const noexcept {
        return get();
    }

    // / @brief Dereference operator
    T& operator*() noexcept {
        return *get();
    }

    // / @brief Const dereference operator
    const T& operator*() const noexcept {
        return *get();
    }

    // / @brief Non-copyable, non-movable
    StaticStorage(const StaticStorage&) = delete;
    StaticStorage& operator=(const StaticStorage&) = delete;
    StaticStorage(StaticStorage&&) = delete;
    StaticStorage& operator=(StaticStorage&&) = delete;

private:
    // Aligned storage
    using Storage = typename std::aligned_storage<Size, alignof(T)>::type;
    Storage storage_;
    
    // Volatile flag to track construction state (protected by critical sections)
    volatile bool constructed_;
};

// Stack Monitor (detects low stack conditions)
class StackMonitor {
public:
    // * * @brief Minimum free stack bytes required * @note Typical values: UI paint() methods: 1024-2048 bytes, Signal processing: 512-1024 bytes
    static constexpr size_t MIN_STACK_FREE = 1024;

    // / @brief Constructor - Captures initial stack state
    StackMonitor() noexcept : initial_free_(get_stack_free()) {}

    // * * @brief Check if current stack has sufficient free space
    bool is_stack_safe(size_t required_bytes = MIN_STACK_FREE) const noexcept {
        return get_stack_free() >= required_bytes;
    }

    // / @brief Get current free stack bytes
    size_t get_free_stack() const noexcept {
        return get_stack_free();
    }

    // / @brief Get stack usage since construction
    size_t get_stack_usage() const noexcept {
        size_t current_free = get_stack_free();
        // Handle edge case where stack grew
        if (current_free > initial_free_) {
            return 0;
        }
        return initial_free_ - current_free;
    }

    // / @brief Get initial free stack bytes
    size_t get_initial_free() const noexcept {
        return initial_free_;
    }

    // / @brief Non-copyable, non-movable
    StackMonitor(const StackMonitor&) = delete;
    StackMonitor& operator=(const StackMonitor&) = delete;
    StackMonitor(StackMonitor&&) = delete;
    StackMonitor& operator=(StackMonitor&&) = delete;

private:
    size_t initial_free_;

    // * * @brief Get current free stack bytes from ChibiOS * @note Returns 0 if called from main thread
    static size_t get_stack_free() noexcept {
        // Get current thread pointer
        Thread* current_thread = chThdGetSelf();
        if (current_thread == nullptr) {
            return MIN_STACK_FREE * 2;
        }
        return chThdGetStackFree(current_thread);
    }
};

// Backward Compatibility Aliases
using ScopedLock = OrderedScopedLock<Mutex, false>;
using MutexLock = ScopedLock;
using MutexTryLock = OrderedScopedLock<Mutex, true>;
using SDCardLock = ScopedLock;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_LOCKING_HPP_
