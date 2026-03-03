# STAGE 4: Diamond Code Synthesis - Final Code Generation (Part 2)

**Date:** 2026-03-02
**Based on:** Stage 3 Red Team Attack (with 7 revisions applied)
**Target Architecture:** STM32F405 (ARM Cortex-M4, 128KB RAM)

---

## Fix #3: Thread-Local Stack Canary (with Revisions)

### eda_optimized_utils.hpp

```cpp
/**
 * @file eda_optimized_utils.hpp
 * @brief Enhanced Drone Analyzer - Optimized Utilities with Stack Canary
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All utilities use static storage
 * - No exceptions: All functions marked noexcept
 * - Thread-safe: ChibiOS primitives for synchronization
 * - Guard clauses: Early returns for edge cases
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 *
 * @author Diamond Code Pipeline
 * @date 2026-03-02
 */

#ifndef EDA_OPTIMIZED_UTILS_HPP_
#define EDA_OPTIMIZED_UTILS_HPP_

#include <cstdint>
#include <cstddef>
#include "ch.h"
#include "eda_constants.hpp"

// ============================================================================
// NAMESPACE: eda::thread_safety
// ============================================================================

namespace eda {
namespace thread_safety {

/**
 * @brief Thread-local storage structure for stack canary
 *
 * This structure implements Revision #3 from Stage 3 Red Team Attack:
 * - Uses ChibiOS thread-local storage API instead of C++ thread_local
 * - Ensures proper integration with ChibiOS thread switching
 *
 * The TLS is stored at the end of the thread's working area (stack).
 * This ensures it's accessible from the thread context and survives
 * thread switches without requiring C++ thread_local support.
 */
struct TLS {
    uint8_t thread_id;           ///< Thread ID for debugging
    uint32_t canary_value;       ///< Canary value for overflow detection
    uint32_t stack_canary_bottom;///< Canary at bottom of stack
    uint32_t stack_canary_top;   ///< Canary at top of stack
    bool initialized;            ///< TLS initialization flag
};

/**
 * @brief Thread-local stack canary guard
 *
 * Provides automatic stack overflow/underflow detection using
 * ChibiOS thread-local storage API.
 *
 * Usage:
 * @code
 * void my_function() {
 *     STACK_CANARY_GUARD();  // Automatically checks on entry and exit
 *     // ... function body ...
 * }
 * @endcode
 */
class StackCanaryGuard {
public:
    /**
     * @brief Constructor - checks stack canary on function entry
     *
     * @param function_name Function name for debugging (optional)
     */
    explicit StackCanaryGuard(const char* function_name = nullptr) noexcept
        : function_name_(function_name) {
        check_on_entry();
    }

    /**
     * @brief Destructor - checks stack canary on function exit
     *
     * Destructor is noexcept to prevent undefined behavior.
     */
    ~StackCanaryGuard() noexcept {
        check_on_exit();
    }

private:
    const char* function_name_;  ///< Function name for debugging

    /**
     * @brief Check stack canary on function entry
     *
     * Detects stack overflow/underflow that occurred before
     * entering this function.
     */
    void check_on_entry() noexcept;

    /**
     * @brief Check stack canary on function exit
     *
     * Detects stack overflow/underflow that occurred during
     * function execution.
     */
    void check_on_exit() noexcept;
};

/**
 * @brief Thread-local stack canary manager
 *
 * Manages initialization and access to thread-local storage
 * using ChibiOS API instead of C++ thread_local.
 */
class ThreadLocalStackCanary {
public:
    /**
     * @brief Initialize TLS for a thread
     *
     * @param thread_id Thread ID for debugging
     * @return true if initialization succeeded
     */
    static bool initialize(uint8_t thread_id) noexcept;

    /**
     * @brief Check stack canary on function entry
     *
     * @param function_name Function name for debugging
     */
    static void check_on_entry(const char* function_name = nullptr) noexcept;

    /**
     * @brief Check stack canary on function exit
     *
     * @param function_name Function name for debugging
     */
    static void check_on_exit(const char* function_name = nullptr) noexcept;

    /**
     * @brief Get thread ID prefix for debugging
     *
     * @return Thread ID prefix string (e.g., "T01:")
     */
    static const char* get_thread_id_prefix() noexcept;

private:
    /**
     * @brief Get thread-local storage reference
     *
     * Uses ChibiOS thread-local storage API instead of C++ thread_local.
     * This implements Revision #3 from Stage 3 Red Team Attack.
     *
     * @return Reference to TLS structure
     */
    static TLS& get_tls() noexcept;

    /**
     * @brief Generate canary value for a thread
     *
     * @param thread_id Thread ID
     * @param timestamp Current timestamp (ms)
     * @return Canary value
     */
    static uint32_t generate_canary(uint8_t thread_id, uint32_t timestamp) noexcept;

    /**
     * @brief Get current stack pointer (ARM Cortex-M4 specific)
     *
     * @return Current stack pointer
     */
    static void* get_stack_pointer() noexcept;

    /**
     * @brief Get stack limit for current thread
     *
     * @return Stack limit (bottom of stack)
     */
    static void* get_stack_limit() noexcept;

    /**
     * @brief Get stack size for current thread
     *
     * @return Stack size in bytes
     */
    static size_t get_stack_size() noexcept;
};

// ============================================================================
// STACK CANARY GUARD MACRO
// ============================================================================

/**
 * @brief Stack canary guard macro
 *
 * Automatically creates a StackCanaryGuard object that checks
 * stack canary on function entry and exit.
 *
 * Usage:
 * @code
 * void my_function() {
 *     STACK_CANARY_GUARD();
 *     // ... function body ...
 * }
 * @endcode
 */
#define STACK_CANARY_GUARD() \
    ::eda::thread_safety::StackCanaryGuard __stack_canary_guard(__func__)

// ============================================================================
// IMPLEMENTATION
// ============================================================================

inline void StackCanaryGuard::check_on_entry() noexcept {
    ThreadLocalStackCanary::check_on_entry(function_name_);
}

inline void StackCanaryGuard::check_on_exit() noexcept {
    ThreadLocalStackCanary::check_on_exit(function_name_);
}

inline bool ThreadLocalStackCanary::initialize(uint8_t thread_id) noexcept {
    auto& tls = get_tls();

    // Guard clause: Already initialized
    if (tls.initialized) {
        return false;
    }

    // Initialize TLS
    tls.thread_id = thread_id;
    tls.canary_value = generate_canary(thread_id, chVTGetSystemTime());
    tls.stack_canary_bottom = tls.canary_value;
    tls.stack_canary_top = tls.canary_value;
    tls.initialized = true;

    // Place canaries at stack boundaries
    void* stack_limit = get_stack_limit();
    size_t stack_size = get_stack_size();

    // Bottom canary (at stack limit)
    uint32_t* bottom_canary = static_cast<uint32_t*>(stack_limit);
    *bottom_canary = tls.canary_value;

    // Top canary (at end of working area, before TLS)
    uint8_t* working_area_end = static_cast<uint8_t*>(stack_limit) + stack_size;
    uint32_t* top_canary = reinterpret_cast<uint32_t*>(working_area_end - sizeof(uint32_t));
    *top_canary = tls.canary_value;

    return true;
}

inline void ThreadLocalStackCanary::check_on_entry(const char* function_name) noexcept {
    auto& tls = get_tls();

    // Guard clause: TLS not initialized
    if (!tls.initialized) {
        log_error("Stack canary check failed: TLS not initialized in %s",
                  function_name ? function_name : "unknown");
        trigger_hard_fault(EDA::Constants::ErrorCodes::STACK_CORRUPTION);
        __builtin_unreachable();
    }

    // Check bottom canary (stack overflow detection)
    void* stack_limit = get_stack_limit();
    uint32_t* bottom_canary = static_cast<uint32_t*>(stack_limit);

    if (*bottom_canary != tls.canary_value) {
        log_error("Stack overflow detected in %s (thread %u): canary = 0x%08X, expected = 0x%08X",
                  function_name ? function_name : "unknown",
                  tls.thread_id,
                  *bottom_canary,
                  tls.canary_value);
        trigger_hard_fault(EDA::Constants::ErrorCodes::STACK_CORRUPTION);
        __builtin_unreachable();
    }

    // Check top canary (stack underflow detection)
    size_t stack_size = get_stack_size();
    uint8_t* working_area_end = static_cast<uint8_t*>(stack_limit) + stack_size;
    uint32_t* top_canary = reinterpret_cast<uint32_t*>(working_area_end - sizeof(uint32_t));

    if (*top_canary != tls.canary_value) {
        log_error("Stack underflow detected in %s (thread %u): canary = 0x%08X, expected = 0x%08X",
                  function_name ? function_name : "unknown",
                  tls.thread_id,
                  *top_canary,
                  tls.canary_value);
        trigger_hard_fault(EDA::Constants::ErrorCodes::STACK_CORRUPTION);
        __builtin_unreachable();
    }
}

inline void ThreadLocalStackCanary::check_on_exit(const char* function_name) noexcept {
    // Same check as entry - canaries should still be valid
    check_on_entry(function_name);
}

inline const char* ThreadLocalStackCanary::get_thread_id_prefix() noexcept {
    auto& tls = get_tls();

    // Static buffer for thread ID prefix
    static char prefix_buffer[8];

    // Format: "T01:" (T + 2-digit thread ID + colon)
    prefix_buffer[0] = 'T';
    prefix_buffer[1] = '0' + (tls.thread_id / 10);
    prefix_buffer[2] = '0' + (tls.thread_id % 10);
    prefix_buffer[3] = ':';
    prefix_buffer[4] = '\0';

    return prefix_buffer;
}

inline TLS& ThreadLocalStackCanary::get_tls() noexcept {
    // Get current thread reference
    thread_t* thread = chThdGetSelfX();

    // Get working area (stack) for current thread
    void* working_area = chThdGetWorkingAreaX(thread);
    size_t working_area_size = chThdGetWorkingAreaSizeX(thread);

    // TLS is stored at end of working area (before stack canary)
    // Working area layout: [stack ...][TLS][canary]
    TLS* tls = reinterpret_cast<TLS*>(
        static_cast<uint8_t*>(working_area) + working_area_size - sizeof(TLS) - sizeof(uint32_t)
    );

    return *tls;
}

inline uint32_t ThreadLocalStackCanary::generate_canary(uint8_t thread_id, uint32_t timestamp) noexcept {
    // Simple but effective canary generation
    // Combines thread ID, timestamp, and magic constant
    uint32_t canary = EDA::Constants::Memory::STACK_CANARY_VALUE;
    canary ^= (static_cast<uint32_t>(thread_id) << 24);
    canary ^= timestamp;
    canary ^= 0x5A5A5A5A;  // Additional magic pattern

    return canary;
}

inline void* ThreadLocalStackCanary::get_stack_pointer() noexcept {
    // ARM Cortex-M4 specific: read stack pointer register
    void* sp;
    __asm__ volatile ("mov %0, sp" : "=r" (sp));
    return sp;
}

inline void* ThreadLocalStackCanary::get_stack_limit() noexcept {
    // Get current thread reference
    thread_t* thread = chThdGetSelfX();

    // Get working area (stack limit is start of working area)
    return chThdGetWorkingAreaX(thread);
}

inline size_t ThreadLocalStackCanary::get_stack_size() noexcept {
    // Get current thread reference
    thread_t* thread = chThdGetSelfX();

    // Get working area size
    return chThdGetWorkingAreaSizeX(thread);
}

} // namespace thread_safety
} // namespace eda

#endif // EDA_OPTIMIZED_UTILS_HPP_
```

---

## Fix #4: Ring Buffer Bounds Protection (with Revisions)

### eda_detection_ring_buffer.hpp

```cpp
/**
 * @file eda_detection_ring_buffer.hpp
 * @brief Enhanced Drone Analyzer - Detection Ring Buffer with Bounds Protection
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: Fixed-size ring buffer
 * - No exceptions: All functions marked noexcept
 * - Thread-safe: ChibiOS mutexes for synchronization
 * - Guard clauses: Early returns for edge cases
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 *
 * @author Diamond Code Pipeline
 * @date 2026-03-02
 */

#ifndef EDA_DETECTION_RING_BUFFER_HPP_
#define EDA_DETECTION_RING_BUFFER_HPP_

#include <cstdint>
#include <cstddef>
#include <array>
#include "ch.h"
#include "eda_constants.hpp"
#include "eda_locking.hpp"

// ============================================================================
// NAMESPACE: eda::detection
// ============================================================================

namespace eda {
namespace detection {

/**
 * @brief Detection entry structure
 *
 * Represents a single drone detection event.
 */
struct DetectionEntry {
    uint64_t frequency_hz;          ///< Frequency in Hz (int64_t range)
    int32_t rssi_db;               ///< RSSI in dBm
    uint8_t snr;                   ///< Signal-to-noise ratio
    uint32_t timestamp_ms;          ///< Detection timestamp
    uint8_t confidence;             ///< Detection confidence (0-100)
};

/**
 * @brief Ring buffer for drone detections
 *
 * Fixed-size ring buffer with bounds protection.
 * Implements Revision #1 and #2 from Stage 3 Red Team Attack:
 * - Revision #1: Add static_assert for HASH_MASK alignment
 * - Revision #2: Add runtime bounds checking
 *
 * Key features:
 * - Fixed-size allocation (no heap)
 * - O(1) insertion and lookup by frequency hash
 * - Bounds checking for all operations
 * - Compile-time alignment validation
 */
class DetectionRingBuffer {
public:
    // =========================================================================
    // CONSTANTS
    // =========================================================================

    /**
     * @brief Maximum number of detections in ring buffer
     */
    static constexpr size_t MAX_DETECTIONS = 64;

    /**
     * @brief Hash mask for frequency hashing
     *
     * Must be power of 2 minus 1 for efficient modulo operation.
     * 63 = 0x3F, which is 2^6 - 1.
     */
    static constexpr uint32_t HASH_MASK = MAX_DETECTIONS - 1;

    /**
     * @brief Hash prime for frequency hashing
     *
     * 65537 is a prime number, providing good distribution.
     */
    static constexpr uint32_t HASH_PRIME = 65537;

    // =========================================================================
    // COMPILE-TIME VALIDATION
    // =========================================================================

    /**
     * @brief Revision #1: Static assert for HASH_MASK alignment
     *
     * Ensures HASH_MASK is power of 2 minus 1 for efficient modulo.
     * This prevents buffer overflow from hash calculation.
     */
    static_assert((HASH_MASK & (HASH_MASK + 1)) == 0,
                  "HASH_MASK must be power of 2 minus 1 for efficient modulo");

    static_assert(MAX_DETECTIONS <= 256,
                  "MAX_DETECTIONS exceeds 256, may cause performance issues");

    static_assert(MAX_DETECTIONS >= 16,
                  "MAX_DETECTIONS below 16, may cause excessive collisions");

    // =========================================================================
    // CONSTRUCTORS / DESTRUCTORS
    // =========================================================================

    /**
     * @brief Constructor
     *
     * Initializes ring buffer to empty state.
     */
    DetectionRingBuffer() noexcept;

    /**
     * @brief Destructor
     *
     * Destructor is noexcept to prevent undefined behavior.
     */
    ~DetectionRingBuffer() noexcept;

    // Delete copy/move operations
    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer(DetectionRingBuffer&&) = delete;
    DetectionRingBuffer& operator=(DetectionRingBuffer&&) = delete;

    // =========================================================================
    // PUBLIC INTERFACE
    // =========================================================================

    /**
     * @brief Add detection to ring buffer
     *
     * @param entry Detection entry to add
     * @return true if added successfully, false if buffer full
     */
    bool add_detection(const DetectionEntry& entry) noexcept;

    /**
     * @brief Get detection by frequency
     *
     * @param frequency_hz Frequency in Hz
     * @return Pointer to detection entry, or nullptr if not found
     */
    [[nodiscard]] const DetectionEntry* get_detection(uint64_t frequency_hz) const noexcept;

    /**
     * @brief Get detection by index
     *
     * @param index Index in ring buffer
     * @return Pointer to detection entry, or nullptr if index invalid
     */
    [[nodiscard]] const DetectionEntry* get_detection_by_index(size_t index) const noexcept;

    /**
     * @brief Remove detection by frequency
     *
     * @param frequency_hz Frequency in Hz
     * @return true if removed successfully, false if not found
     */
    bool remove_detection(uint64_t frequency_hz) noexcept;

    /**
     * @brief Clear all detections
     */
    void clear() noexcept;

    /**
     * @brief Get number of detections
     *
     * @return Number of detections in buffer
     */
    [[nodiscard]] size_t get_count() const noexcept;

    /**
     * @brief Check if buffer is full
     *
     * @return true if buffer is full
     */
    [[nodiscard]] bool is_full() const noexcept;

    /**
     * @brief Check if buffer is empty
     *
     * @return true if buffer is empty
     */
    [[nodiscard]] bool is_empty() const noexcept;

    /**
     * @brief Iterate over all detections
     *
     * @param callback Function to call for each detection
     * @return Number of detections processed
     */
    size_t iterate_detections(void (*callback)(const DetectionEntry& entry)) noexcept;

private:
    // =========================================================================
    // PRIVATE MEMBERS
    // =========================================================================

    /**
     * @brief Detection storage array
     *
     * Fixed-size array, no heap allocation.
     */
    std::array<DetectionEntry, MAX_DETECTIONS> detections_;

    /**
     * @brief Occupancy flags (true if slot is in use)
     */
    std::array<bool, MAX_DETECTIONS> occupied_;

    /**
     * @brief Mutex for thread-safe access
     */
    mutable Mutex mutex_;

    /**
     * @brief Number of detections in buffer
     */
    size_t count_;

    // =========================================================================
    // PRIVATE METHODS
    // =========================================================================

    /**
     * @brief Safe frequency hash function
     *
     * Implements Revision #2 from Stage 3 Red Team Attack:
     * - Adds runtime bounds checking
     *
     * Hashes frequency to index in ring buffer.
     * Uses modulo with prime number for good distribution.
     * Bounds checking prevents buffer overflow.
     *
     * @param frequency_hz Frequency in Hz
     * @return Hashed index (always valid: 0 to MAX_DETECTIONS-1)
     */
    static size_t safe_frequency_hash(uint64_t frequency_hz) noexcept;

    /**
     * @brief Find empty slot
     *
     * @return Index of empty slot, or MAX_DETECTIONS if full
     */
    size_t find_empty_slot() const noexcept;

    /**
     * @brief Find slot by frequency
     *
     * @param frequency_hz Frequency in Hz
     * @return Index of slot, or MAX_DETECTIONS if not found
     */
    size_t find_slot_by_frequency(uint64_t frequency_hz) const noexcept;
};

// ============================================================================
// IMPLEMENTATION
// ============================================================================

inline DetectionRingBuffer::DetectionRingBuffer() noexcept
    : count_(0) {
    // Initialize occupancy flags to false
    for (size_t i = 0; i < MAX_DETECTIONS; ++i) {
        occupied_[i] = false;
    }
}

inline DetectionRingBuffer::~DetectionRingBuffer() noexcept {
    // Destructor body - no heap deallocation needed
}

inline bool DetectionRingBuffer::add_detection(const DetectionEntry& entry) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Buffer full
    if (is_full()) {
        return false;
    }

    // Find empty slot
    size_t index = find_empty_slot();

    // Guard clause: No empty slot (should not happen if not full)
    if (index >= MAX_DETECTIONS) {
        return false;
    }

    // Add detection
    detections_[index] = entry;
    occupied_[index] = true;
    ++count_;

    return true;
}

inline const DetectionEntry* DetectionRingBuffer::get_detection(uint64_t frequency_hz) const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Empty buffer
    if (is_empty()) {
        return nullptr;
    }

    // Find slot by frequency
    size_t index = find_slot_by_frequency(frequency_hz);

    // Guard clause: Not found
    if (index >= MAX_DETECTIONS) {
        return nullptr;
    }

    return &detections_[index];
}

inline const DetectionEntry* DetectionRingBuffer::get_detection_by_index(size_t index) const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Index out of bounds (Revision #2: runtime bounds checking)
    if (index >= MAX_DETECTIONS) {
        return nullptr;
    }

    // Guard clause: Slot not occupied
    if (!occupied_[index]) {
        return nullptr;
    }

    return &detections_[index];
}

inline bool DetectionRingBuffer::remove_detection(uint64_t frequency_hz) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Empty buffer
    if (is_empty()) {
        return false;
    }

    // Find slot by frequency
    size_t index = find_slot_by_frequency(frequency_hz);

    // Guard clause: Not found
    if (index >= MAX_DETECTIONS) {
        return false;
    }

    // Remove detection
    occupied_[index] = false;
    --count_;

    return true;
}

inline void DetectionRingBuffer::clear() noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Clear all occupancy flags
    for (size_t i = 0; i < MAX_DETECTIONS; ++i) {
        occupied_[i] = false;
    }

    count_ = 0;
}

inline size_t DetectionRingBuffer::get_count() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return count_;
}

inline bool DetectionRingBuffer::is_full() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return count_ >= MAX_DETECTIONS;
}

inline bool DetectionRingBuffer::is_empty() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return count_ == 0;
}

inline size_t DetectionRingBuffer::iterate_detections(void (*callback)(const DetectionEntry& entry)) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    size_t processed = 0;

    for (size_t i = 0; i < MAX_DETECTIONS; ++i) {
        if (occupied_[i]) {
            callback(detections_[i]);
            ++processed;
        }
    }

    return processed;
}

inline size_t DetectionRingBuffer::safe_frequency_hash(uint64_t frequency_hz) noexcept {
    // Revision #2: Runtime bounds checking

    // Calculate hash using modulo with prime
    uint32_t hash = static_cast<uint32_t>(frequency_hz % HASH_PRIME);

    // Ensure hash is within bounds (0 to MAX_DETECTIONS-1)
    // This prevents buffer overflow from hash calculation
    if (hash >= MAX_DETECTIONS) {
        // Clamp to valid range
        hash = hash % MAX_DETECTIONS;
    }

    return static_cast<size_t>(hash);
}

inline size_t DetectionRingBuffer::find_empty_slot() const noexcept {
    // Linear search for empty slot
    for (size_t i = 0; i < MAX_DETECTIONS; ++i) {
        if (!occupied_[i]) {
            return i;
        }
    }

    // No empty slot found
    return MAX_DETECTIONS;
}

inline size_t DetectionRingBuffer::find_slot_by_frequency(uint64_t frequency_hz) const noexcept {
    // Calculate hash
    size_t hash = safe_frequency_hash(frequency_hz);

    // Check slot at hash index
    if (occupied_[hash] && detections_[hash].frequency_hz == frequency_hz) {
        return hash;
    }

    // Linear search for matching frequency (collision handling)
    for (size_t i = 0; i < MAX_DETECTIONS; ++i) {
        if (occupied_[i] && detections_[i].frequency_hz == frequency_hz) {
            return i;
        }
    }

    // Not found
    return MAX_DETECTIONS;
}

} // namespace detection
} // namespace eda

#endif // EDA_DETECTION_RING_BUFFER_HPP_
```

---

## Fix #5: Unified Lock Order (with Revisions)

### eda_locking.hpp

```cpp
/**
 * @file eda_locking.hpp
 * @brief Enhanced Drone Analyzer - Unified Lock Order with RAII Wrappers
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All wrappers use static storage
 * - No exceptions: All functions marked noexcept
 * - Thread-safe: ChibiOS primitives for synchronization
 * - Guard clauses: Early returns for edge cases
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 *
 * @author Diamond Code Pipeline
 * @date 2026-03-02
 */

#ifndef EDA_LOCKING_HPP_
#define EDA_LOCKING_HPP_

#include <cstdint>
#include "ch.h"
#include "eda_constants.hpp"

// ============================================================================
// NAMESPACE: eda::threading
// ============================================================================

namespace eda {
namespace threading {

/**
 * @brief Lock order enumeration for deadlock prevention
 *
 * Implements Revision #1 from Stage 3 Red Team Attack:
 * - Add LockOrder enum with clear ordering
 *
 * All locks must be acquired in ascending order to prevent deadlocks.
 * This is documented in the header for all developers to follow.
 *
 * Lock Order:
 * 1. ATOMIC_FLAGS (lowest priority)
 * 2. INIT_MUTEX
 * 3. DATA_MUTEX
 * 4. SPECTRUM_MUTEX
 * 5. STATE_MUTEX
 * 6. SNAPSHOT_MUTEX
 * 7. LOGGER_MUTEX
 * 8. SD_CARD_MUTEX (highest priority)
 */
enum class LockOrder : uint8_t {
    ATOMIC_FLAGS = 0,      ///< Atomic flags lock (lowest priority)
    INIT_MUTEX = 1,        ///< Initialization mutex
    DATA_MUTEX = 2,         ///< Data access mutex
    SPECTRUM_MUTEX = 3,     ///< Spectrum data mutex
    STATE_MUTEX = 4,         ///< State change mutex
    SNAPSHOT_MUTEX = 5,      ///< Snapshot access mutex
    LOGGER_MUTEX = 6,        ///< Logger mutex
    SD_CARD_MUTEX = 7,       ///< SD card mutex (highest priority)
    INVALID = 255            ///< Invalid lock order
};

/**
 * @brief Lock order tracker for deadlock detection
 *
 * Implements Revision #2 from Stage 3 Red Team Attack:
 * - Add LockOrderTracker class with runtime verification
 *
 * Tracks lock acquisition order and detects violations at runtime.
 * This prevents deadlocks by enforcing a global lock order.
 */
class LockOrderTracker {
public:
    /**
     * @brief Acquire lock with order verification
     *
     * @param order Lock order to acquire
     * @return true if lock acquired successfully
     *
     * @note Triggers hard fault if lock order violation detected
     */
    static bool acquire_lock(LockOrder order) noexcept;

    /**
     * @brief Release lock
     *
     * @param order Lock order to release
     */
    static void release_lock(LockOrder order) noexcept;

    /**
     * @brief Get current lock depth
     *
     * @return Current lock depth (number of locks held)
     */
    static uint8_t get_lock_depth() noexcept;

    /**
     * @brief Get current lock order
     *
     * @return Current lock order (INVALID if no locks held)
     */
    static LockOrder get_current_lock_order() noexcept;

private:
    /**
     * @brief Current lock order (highest lock held)
     */
    static volatile LockOrder current_lock_order_;

    /**
     * @brief Previous lock order (for nested locks)
     */
    static volatile LockOrder previous_lock_order_;

    /**
     * @brief Lock depth (number of locks held)
     */
    static volatile uint8_t lock_depth_;

    /**
     * @brief Maximum lock depth (for overflow detection)
     */
    static constexpr uint8_t MAX_LOCK_DEPTH = 255;
};

/**
 * @brief RAII mutex lock wrapper with lock order verification
 *
 * Automatically acquires mutex on construction and releases on destruction.
 * Enforces lock order to prevent deadlocks.
 *
 * Usage:
 * @code
 * void my_function() {
 *     MutexLock lock(my_mutex, LockOrder::DATA_MUTEX);
 *     // ... critical section ...
 * }  // Mutex automatically released
 * @endcode
 */
class MutexLock {
public:
    /**
     * @brief Constructor - acquires mutex
     *
     * @param mutex Mutex to lock
     * @param order Lock order for deadlock prevention
     *
     * @note Triggers hard fault if lock order violation detected
     */
    explicit MutexLock(Mutex& mutex, LockOrder order) noexcept
        : mutex_(mutex)
        , order_(order)
        , locked_(false) {

        // Track lock order (Revision #2: LockOrderTracker)
        LockOrderTracker::acquire_lock(order);

        // Acquire mutex
        mutex_.lock();
        locked_ = true;
    }

    /**
     * @brief Destructor - releases mutex
     *
     * Destructor is noexcept to prevent undefined behavior.
     */
    ~MutexLock() noexcept {
        if (locked_) {
            mutex_.unlock();
            LockOrderTracker::release_lock(order_);
        }
    }

    // Delete copy/move operations
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    MutexLock(MutexLock&&) = delete;
    MutexLock& operator=(MutexLock&&) = delete;

private:
    Mutex& mutex_;              ///< Reference to mutex
    LockOrder order_;           ///< Lock order
    bool locked_;               ///< Lock state
};

/**
 * @brief RAII critical section wrapper for ISR-safe interrupt control
 *
 * Automatically disables interrupts on construction and re-enables on destruction.
 * Use this for very short critical sections only.
 *
 * Usage:
 * @code
 * void my_isr_function() {
 *     CriticalSection cs;
 *     // ... critical section ...
 * }  // Interrupts automatically re-enabled
 * @endcode
 */
class CriticalSection {
public:
    /**
     * @brief Constructor - enters critical section
     *
     * Disables interrupts and locks scheduler.
     */
    CriticalSection() noexcept
        : locked_(false) {
        chSysLock();
        locked_ = true;
    }

    /**
     * @brief Destructor - exits critical section
     *
     * Destructor is noexcept to prevent undefined behavior.
     */
    ~CriticalSection() noexcept {
        if (locked_) {
            chSysUnlock();
        }
    }

    // Delete copy/move operations
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
    CriticalSection(CriticalSection&&) = delete;
    CriticalSection& operator=(CriticalSection&&) = delete;

private:
    bool locked_;  ///< Critical section state
};

// ============================================================================
// IMPLEMENTATION: LockOrderTracker
// ============================================================================

inline bool LockOrderTracker::acquire_lock(LockOrder order) noexcept {
    // Guard clause: Invalid lock order
    if (order == LockOrder::INVALID) {
        log_error("Lock order violation: trying to acquire INVALID lock");
        trigger_hard_fault(EDA::Constants::ErrorCodes::DATA_ABORT);
        __builtin_unreachable();
    }

    // Guard clause: Lock order violation (must acquire in ascending order)
    if (order < current_lock_order_) {
        log_error("Lock order violation: trying to acquire lock %u while holding lock %u",
                  static_cast<uint8_t>(order), static_cast<uint8_t>(current_lock_order_));
        trigger_hard_fault(EDA::Constants::ErrorCodes::DATA_ABORT);
        __builtin_unreachable();
    }

    // Guard clause: Lock depth overflow
    if (lock_depth_ >= MAX_LOCK_DEPTH) {
        log_error("Lock depth overflow: too many nested locks (%u)", lock_depth_);
        trigger_hard_fault(EDA::Constants::ErrorCodes::DATA_ABORT);
        __builtin_unreachable();
    }

    // Update lock order state
    previous_lock_order_ = current_lock_order_;
    current_lock_order_ = order;
    ++lock_depth_;

    return true;
}

inline void LockOrderTracker::release_lock(LockOrder order) noexcept {
    // Guard clause: Invalid lock order
    if (order == LockOrder::INVALID) {
        return;
    }

    // Guard clause: Lock depth underflow
    if (lock_depth_ == 0) {
        log_error("Lock depth underflow: releasing lock when no locks held");
        trigger_hard_fault(EDA::Constants::ErrorCodes::DATA_ABORT);
        __builtin_unreachable();
    }

    // Guard clause: Releasing wrong lock order
    if (order != current_lock_order_) {
        log_error("Lock order violation: trying to release lock %u but holding lock %u",
                  static_cast<uint8_t>(order), static_cast<uint8_t>(current_lock_order_));
        trigger_hard_fault(EDA::Constants::ErrorCodes::DATA_ABORT);
        __builtin_unreachable();
    }

    // Update lock order state
    current_lock_order_ = previous_lock_order_;
    --lock_depth_;
}

inline uint8_t LockOrderTracker::get_lock_depth() noexcept {
    return lock_depth_;
}

inline LockOrder LockOrderTracker::get_current_lock_order() noexcept {
    return current_lock_order_;
}

// ============================================================================
// STATIC MEMBER INITIALIZATION
// ============================================================================

// Lock order tracker state (initialized to default values)
volatile LockOrder LockOrderTracker::current_lock_order_ = LockOrder::INVALID;
volatile LockOrder LockOrderTracker::previous_lock_order_ = LockOrder::INVALID;
volatile uint8_t LockOrderTracker::lock_depth_ = 0;

} // namespace threading
} // namespace eda

#endif // EDA_LOCKING_HPP_
```

---

**End of Part 2** - This document continues in `stage4_diamond_synthesis_part3.md`
