# STAGE 4: Diamond Code Synthesis - Final Code Generation (Part 3)

**Date:** 2026-03-02
**Based on:** Stage 3 Red Team Attack (with 7 revisions applied)
**Target Architecture:** STM32F405 (ARM Cortex-M4, 128KB RAM)

---

## Fix #6: Stack Size Unification (with Revisions)

### eda_constants.hpp

```cpp
/**
 * @file eda_constants.hpp
 * @brief Enhanced Drone Analyzer - Constants with Stack Size Unification
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All constants are constexpr
 * - No exceptions: All operations are noexcept
 * - Type-safe: Uses semantic type aliases
 * - Memory-safe: Compile-time size validation
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 *
 * @author Diamond Code Pipeline
 * @date 2026-03-02
 */

#ifndef EDA_CONSTANTS_HPP_
#define EDA_CONSTANTS_HPP_

#include <cstdint>
#include <cstddef>

// ============================================================================
// NAMESPACE: EDA
// ============================================================================

namespace EDA {

// ============================================================================
// SEMANTIC TYPE ALIASES
// ============================================================================

/**
 * @brief Frequency in Hertz (64-bit signed)
 * @note Covers -9.2e18 to 9.2e18 Hz (sufficient for all RF frequencies)
 * @note Matches rf::Frequency type in Mayhem framework
 */
using Frequency = int64_t;

/**
 * @brief RSSI in dBm (32-bit signed)
 * @note Typical range: -120 to 0 dBm
 */
using RSSI = int32_t;

/**
 * @brief Spectrum bin index (8-bit unsigned)
 * @note Covers 0 to 255 bins
 */
using BinIndex = uint8_t;

/**
 * @brief Threshold value (8-bit unsigned)
 * @note Covers 0 to 255
 */
using Threshold = uint8_t;

/**
 * @brief Decibel value (8-bit unsigned)
 * @note Covers 0 to 255 dB
 */
using Decibel = uint8_t;

/**
 * @brief Timestamp in seconds (32-bit unsigned)
 * @note Covers 0 to 4,294,967,295 seconds (~136 years)
 */
using Timestamp = uint32_t;

// ============================================================================
// STACK SIZE CONSTANTS
// ============================================================================

/**
 * @brief Stack size constants for all threads
 *
 * Implements Revision #6 and #7 from Stage 3 Red Team Attack:
 * - Revision #6: Increase Main UI thread stack to 3KB
 * - Revision #7: Add runtime stack monitoring
 *
 * All stack sizes are unified and validated at compile time.
 * Runtime monitoring detects high stack usage before overflow.
 */
namespace StackSizes {
    /**
     * @brief Minimum stack size (1KB)
     */
    static constexpr size_t MINIMUM_STACK_SIZE = 1024;

    /**
     * @brief Maximum stack size (8KB)
     */
    static constexpr size_t MAXIMUM_STACK_SIZE = 8192;

    /**
     * @brief Main UI thread stack size (3KB)
     *
     * Revision #6: Increased from 2KB to 3KB to accommodate
     * worst-case recursive painting scenarios (2,496 bytes calculated usage).
     * This provides 576 bytes of headroom.
     */
    static constexpr size_t MAIN_UI_THREAD = 3072;

    /**
     * @brief Scanning thread stack size (5KB)
     */
    static constexpr size_t SCANNING_THREAD = 5120;

    /**
     * @brief Coordinator thread stack size (2KB)
     */
    static constexpr size_t COORDINATOR_THREAD = 2048;

    /**
     * @brief Logger worker thread stack size (4KB)
     */
    static constexpr size_t LOGGER_WORKER_THREAD = 4096;

    /**
     * @brief Audio worker thread stack size (2KB)
     */
    static constexpr size_t AUDIO_WORKER_THREAD = 2048;

    /**
     * @brief Stack alignment (64 bytes)
     *
     * Required for ARM Cortex-M4 stack alignment.
     */
    static constexpr size_t STACK_ALIGNMENT = 64;

    // =========================================================================
    // COMPILE-TIME VALIDATION
    // =========================================================================

    /**
     * @brief Validate all stack sizes are within limits
     */
    static_assert(MAIN_UI_THREAD >= MINIMUM_STACK_SIZE,
                  "MAIN_UI_THREAD stack size below minimum");

    static_assert(MAIN_UI_THREAD <= MAXIMUM_STACK_SIZE,
                  "MAIN_UI_THREAD stack size exceeds maximum");

    static_assert(SCANNING_THREAD >= MINIMUM_STACK_SIZE,
                  "SCANNING_THREAD stack size below minimum");

    static_assert(SCANNING_THREAD <= MAXIMUM_STACK_SIZE,
                  "SCANNING_THREAD stack size exceeds maximum");

    static_assert(COORDINATOR_THREAD >= MINIMUM_STACK_SIZE,
                  "COORDINATOR_THREAD stack size below minimum");

    static_assert(COORDINATOR_THREAD <= MAXIMUM_STACK_SIZE,
                  "COORDINATOR_THREAD stack size exceeds maximum");

    static_assert(LOGGER_WORKER_THREAD >= MINIMUM_STACK_SIZE,
                  "LOGGER_WORKER_THREAD stack size below minimum");

    static_assert(LOGGER_WORKER_THREAD <= MAXIMUM_STACK_SIZE,
                  "LOGGER_WORKER_THREAD stack size exceeds maximum");

    static_assert(AUDIO_WORKER_THREAD >= MINIMUM_STACK_SIZE,
                  "AUDIO_WORKER_THREAD stack size below minimum");

    static_assert(AUDIO_WORKER_THREAD <= MAXIMUM_STACK_SIZE,
                  "AUDIO_WORKER_THREAD stack size exceeds maximum");

    /**
     * @brief Validate stack alignment
     */
    static_assert(STACK_ALIGNMENT == 64,
                  "STACK_ALIGNMENT must be 64 bytes for ARM Cortex-M4");

    /**
     * @brief Validate all stack sizes are aligned
     */
    static_assert(MAIN_UI_THREAD % STACK_ALIGNMENT == 0,
                  "MAIN_UI_THREAD not aligned to STACK_ALIGNMENT");

    static_assert(SCANNING_THREAD % STACK_ALIGNMENT == 0,
                  "SCANNING_THREAD not aligned to STACK_ALIGNMENT");

    static_assert(COORDINATOR_THREAD % STACK_ALIGNMENT == 0,
                  "COORDINATOR_THREAD not aligned to STACK_ALIGNMENT");

    static_assert(LOGGER_WORKER_THREAD % STACK_ALIGNMENT == 0,
                  "LOGGER_WORKER_THREAD not aligned to STACK_ALIGNMENT");

    static_assert(AUDIO_WORKER_THREAD % STACK_ALIGNMENT == 0,
                  "AUDIO_WORKER_THREAD not aligned to STACK_ALIGNMENT");
}

// ============================================================================
// STACK MONITORING CONSTANTS
// ============================================================================

/**
 * @brief Stack monitoring constants
 *
 * Implements Revision #7 from Stage 3 Red Team Attack:
 * - Add runtime stack monitoring
 */
namespace StackMonitoring {
    /**
     * @brief Warning threshold (80% of stack size)
     */
    static constexpr float WARNING_THRESHOLD = 80.0f;

    /**
     * @brief Error threshold (95% of stack size)
     */
    static constexpr float ERROR_THRESHOLD = 95.0f;

    /**
     * @brief Minimum free stack threshold (512 bytes)
     */
    static constexpr size_t MIN_FREE_THRESHOLD = 512;
}

// ============================================================================
// THREAD STACK ALLOCATOR
// ============================================================================

/**
 * @brief Thread stack allocator
 *
 * Provides unified stack allocation with size validation
 * and runtime monitoring support.
 */
class ThreadStackAllocator {
public:
    /**
     * @brief Allocate stack for a thread
     *
     * @param size Stack size in bytes
     * @param thread_name Thread name for debugging
     * @return Pointer to allocated stack, or nullptr if allocation failed
     *
     * @note Triggers hard fault if size is invalid
     */
    static void* allocate_stack(size_t size, const char* thread_name) noexcept;

    /**
     * @brief Get stack size for a thread type
     *
     * @param thread_type Thread type enum
     * @return Stack size in bytes
     */
    static size_t get_stack_size(uint8_t thread_type) noexcept;

    /**
     * @brief Validate stack size
     *
     * @param size Stack size in bytes
     * @return true if size is valid
     */
    static bool validate_stack_size(size_t size) noexcept;

    /**
     * @brief Thread type enumeration
     */
    enum class ThreadType : uint8_t {
        MAIN_UI = 0,
        SCANNING = 1,
        COORDINATOR = 2,
        LOGGER_WORKER = 3,
        AUDIO_WORKER = 4
    };

private:
    /**
     * @brief Stack pool (static allocation, no heap)
     */
    static uint8_t stack_pool_[16384];  // 16KB total stack pool

    /**
     * @brief Current pool offset
     */
    static size_t pool_offset_;
};

// ============================================================================
// IMPLEMENTATION: ThreadStackAllocator
// ============================================================================

inline void* ThreadStackAllocator::allocate_stack(size_t size, const char* thread_name) noexcept {
    // Guard clause: Validate stack size
    if (!validate_stack_size(size)) {
        log_error("Invalid stack size %zu for thread %s", size, thread_name);
        trigger_hard_fault(EDA::Constants::ErrorCodes::MEM_ACCESS_VIOLATION);
        __builtin_unreachable();
    }

    // Guard clause: Validate alignment
    if (size % StackSizes::STACK_ALIGNMENT != 0) {
        log_error("Stack size %zu not aligned to %zu bytes for thread %s",
                  size, StackSizes::STACK_ALIGNMENT, thread_name);
        trigger_hard_fault(EDA::Constants::ErrorCodes::MEM_ACCESS_VIOLATION);
        __builtin_unreachable();
    }

    // Guard clause: Stack pool exhausted
    if (pool_offset_ + size > sizeof(stack_pool_)) {
        log_error("Stack pool exhausted for thread %s (offset=%zu, size=%zu, total=%zu)",
                  thread_name, pool_offset_, size, sizeof(stack_pool_));
        trigger_hard_fault(EDA::Constants::ErrorCodes::MEM_ACCESS_VIOLATION);
        __builtin_unreachable();
    }

    // Allocate stack from pool
    void* stack = &stack_pool_[pool_offset_];
    pool_offset_ += size;

    // Initialize stack canary for this thread
    // (This would be called after thread creation)
    // eda::thread_safety::ThreadLocalStackCanary::initialize(thread_id);

    return stack;
}

inline size_t ThreadStackAllocator::get_stack_size(uint8_t thread_type) noexcept {
    switch (static_cast<ThreadType>(thread_type)) {
        case ThreadType::MAIN_UI:
            return StackSizes::MAIN_UI_THREAD;
        case ThreadType::SCANNING:
            return StackSizes::SCANNING_THREAD;
        case ThreadType::COORDINATOR:
            return StackSizes::COORDINATOR_THREAD;
        case ThreadType::LOGGER_WORKER:
            return StackSizes::LOGGER_WORKER_THREAD;
        case ThreadType::AUDIO_WORKER:
            return StackSizes::AUDIO_WORKER_THREAD;
        default:
            return StackSizes::MINIMUM_STACK_SIZE;
    }
}

inline bool ThreadStackAllocator::validate_stack_size(size_t size) noexcept {
    return size >= StackSizes::MINIMUM_STACK_SIZE &&
           size <= StackSizes::MAXIMUM_STACK_SIZE;
}

// ============================================================================
// STATIC MEMBER INITIALIZATION
// ============================================================================

// Stack pool (BSS segment, zero-initialized)
uint8_t ThreadStackAllocator::stack_pool_[16384] = {0};

// Pool offset (initialized to 0)
size_t ThreadStackAllocator::pool_offset_ = 0;

// ============================================================================
// RUNTIME STACK MONITORING
// ============================================================================

/**
 * @brief Stack monitor for runtime stack usage detection
 *
 * Implements Revision #7 from Stage 3 Red Team Attack:
 * - Add runtime stack monitoring
 *
 * Provides early warning of potential stack overflow by
 * monitoring stack usage and logging warnings at 80% and
 * errors at 95% usage.
 */
class StackMonitor {
public:
    /**
     * @brief Check stack usage and log warning if high
     *
     * @param function_name Function name for debugging
     */
    static void check_stack_usage(const char* function_name) noexcept;

    /**
     * @brief Get stack usage percentage
     *
     * @return Stack usage percentage (0.0 to 100.0)
     */
    static float get_stack_usage_percent() noexcept;

    /**
     * @brief Get free stack bytes
     *
     * @return Free stack bytes
     */
    static size_t get_free_stack_bytes() noexcept;

    /**
     * @brief Get used stack bytes
     *
     * @return Used stack bytes
     */
    static size_t get_used_stack_bytes() noexcept;

private:
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
// IMPLEMENTATION: StackMonitor
// ============================================================================

inline void StackMonitor::check_stack_usage(const char* function_name) noexcept {
    float usage_percent = get_stack_usage_percent();

    // Log warning if usage > 80%
    if (usage_percent > StackMonitoring::WARNING_THRESHOLD) {
        log_warning("Stack usage high in %s: %.1f%%",
                    function_name ? function_name : "unknown", usage_percent);
    }

    // Log error if usage > 95%
    if (usage_percent > StackMonitoring::ERROR_THRESHOLD) {
        log_error("Stack usage critical in %s: %.1f%%",
                  function_name ? function_name : "unknown", usage_percent);
    }
}

inline float StackMonitor::get_stack_usage_percent() noexcept {
    size_t used = get_used_stack_bytes();
    size_t total = get_stack_size();

    if (total == 0) {
        return 0.0f;
    }

    return 100.0f * static_cast<float>(used) / static_cast<float>(total);
}

inline size_t StackMonitor::get_free_stack_bytes() noexcept {
    size_t used = get_used_stack_bytes();
    size_t total = get_stack_size();

    if (used >= total) {
        return 0;
    }

    return total - used;
}

inline size_t StackMonitor::get_used_stack_bytes() noexcept {
    void* stack_limit = get_stack_limit();
    void* current_sp = get_stack_pointer();

    return static_cast<uint8_t*>(stack_limit) - static_cast<uint8_t*>(current_sp);
}

inline void* StackMonitor::get_stack_pointer() noexcept {
    // ARM Cortex-M4 specific: read stack pointer register
    void* sp;
    __asm__ volatile ("mov %0, sp" : "=r" (sp));
    return sp;
}

inline void* StackMonitor::get_stack_limit() noexcept {
    // Get current thread reference
    thread_t* thread = chThdGetSelfX();

    // Get working area (stack limit is start of working area)
    return chThdGetWorkingAreaX(thread);
}

inline size_t StackMonitor::get_stack_size() noexcept {
    // Get current thread reference
    thread_t* thread = chThdGetSelfX();

    // Get working area size
    return chThdGetWorkingAreaSizeX(thread);
}

// ============================================================================
// STACK MONITORING MACRO
// ============================================================================

/**
 * @brief Stack monitoring macro
 *
 * Automatically checks stack usage and logs warnings.
 *
 * Usage:
 * @code
 * void my_function() {
 *     STACK_MONITOR();
 *     // ... function body ...
 * }
 * @endcode
 */
#define STACK_MONITOR() \
    ::EDA::StackMonitor::check_stack_usage(__func__)

} // namespace EDA

#endif // EDA_CONSTANTS_HPP_
```

---

## UI/DSP Separation Implementation

### eda_snapshot_types.hpp

```cpp
/**
 * @file eda_snapshot_types.hpp
 * @brief Enhanced Drone Analyzer - UI/DSP Separation Types
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All types use fixed-size arrays
 * - No exceptions: All functions marked noexcept
 * - Thread-safe: Volatile flags and memory barriers
 * - Guard clauses: Early returns for edge cases
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 *
 * @author Diamond Code Pipeline
 * @date 2026-03-02
 */

#ifndef EDA_SNAPSHOT_TYPES_HPP_
#define EDA_SNAPSHOT_TYPES_HPP_

#include <cstdint>
#include <cstddef>
#include <array>
#include "ch.h"
#include "eda_constants.hpp"

// ============================================================================
// NAMESPACE: eda::snapshot
// ============================================================================

namespace eda {
namespace snapshot {

/**
 * @brief Drone type enumeration
 */
enum class DroneType : uint8_t {
    UNKNOWN = 0,
    DJI = 1,
    PARROT = 2,
    YUNEEC = 3,
    3DR = 4,
    AUTEL = 5,
    HOVER = 6,
    WALKERA = 7,
    CHEERSON = 8,
    SYMA = 9,
    OTHER = 255
};

/**
 * @brief Threat level enumeration
 */
enum class ThreatLevel : uint8_t {
    NONE = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

/**
 * @brief Tracked drone display data
 *
 * Represents a single drone for UI display.
 * All fields are value types (no heap allocation).
 */
struct TrackedDroneDisplay {
    uint64_t frequency_hz;        ///< Frequency in Hz
    int32_t rssi_db;             ///< RSSI in dBm
    uint8_t snr;                 ///< Signal-to-noise ratio
    DroneType drone_type;          ///< Drone type
    ThreatLevel threat_level;       ///< Threat level
    uint32_t last_detection_age_ms; ///< Age of last detection
};

/**
 * @brief Display data structure
 *
 * Contains all data needed for UI rendering.
 * This is a snapshot of the DSP state, copied atomically.
 * All fields are value types (no heap allocation).
 *
 * Size: ~520 bytes (including padding)
 */
struct DisplayData {
    /**
     * @brief Spectrum power levels (256 bins)
     */
    std::array<uint8_t, 256> spectrum_db;

    /**
     * @brief Tracked drones (up to 10 drones)
     */
    std::array<TrackedDroneDisplay, 10> tracked_drones;

    /**
     * @brief Number of tracked drones
     */
    uint8_t tracked_drone_count;

    /**
     * @brief Scanning active flag
     */
    bool scanning_active;

    /**
     * @brief Scan progress percentage (0-100)
     */
    uint32_t scan_progress_percent;

    /**
     * @brief Snapshot timestamp (ms)
     */
    uint32_t snapshot_timestamp_ms;

    /**
     * @brief Snapshot version (for change detection)
     */
    uint32_t snapshot_version;
};

/**
 * @brief Drone snapshot structure
 *
 * Thread-safe snapshot for UI/DSP communication.
 * Uses volatile flag and memory barriers for synchronization.
 * Replaces std::atomic with ChibiOS primitives (Revision #1 from Stage 3).
 *
 * Size: ~527 bytes (including padding)
 */
struct DroneSnapshot {
    /**
     * @brief Valid flag (volatile for thread safety)
     *
     * Set to true after data is written.
     * Read by consumer to check if snapshot is ready.
     */
    volatile bool valid;

    /**
     * @brief Display data (aligned for atomic access)
     */
    alignas(4) DisplayData data;

    /**
     * @brief Version counter (volatile, no std::atomic)
     *
     * Revision #1: Replace std::atomic with volatile + memory barriers.
     * Incremented on each snapshot write.
     */
    volatile uint32_t version;

    /**
     * @brief Producer thread ID
     */
    uint8_t producer_thread_id;

    /**
     * @brief Last consumer thread ID
     */
    uint8_t last_consumer_thread_id;

    /**
     * @brief Increment version with memory barrier
     *
     * Revision #1: Use ChibiOS memory barriers instead of std::atomic.
     */
    void increment_version() noexcept;

    /**
     * @brief Get version with memory barrier
     *
     * Revision #1: Use ChibiOS memory barriers instead of std::atomic.
     *
     * @return Current version
     */
    uint32_t get_version() const noexcept;
};

/**
 * @brief Spectrum buffer structure
 *
 * Thread-safe buffer for spectrum data.
 * Uses mutex for synchronization.
 *
 * Size: ~285 bytes (BSS allocation, not stack)
 */
struct SpectrumBuffer {
    /**
     * @brief Spectrum buffer (256 bins)
     *
     * Allocated in BSS segment, not on stack.
     */
    std::array<uint8_t, 256> buffer_;

    /**
     * @brief Mutex for thread-safe access
     */
    mutable Mutex buffer_mutex_;

    /**
     * @brief Data updated flag (volatile for thread safety)
     */
    volatile bool data_updated_;

    /**
     * @brief Update timestamp (ms)
     */
    uint32_t update_timestamp_ms_;

    /**
     * @brief Constructor
     */
    SpectrumBuffer() noexcept;

    /**
     * @brief Destructor
     */
    ~SpectrumBuffer() noexcept;

    /**
     * @brief Write spectrum data
     *
     * @param data Pointer to spectrum data (256 bytes)
     * @param timestamp_ms Timestamp in milliseconds
     */
    void write_spectrum(const uint8_t* data, uint32_t timestamp_ms) noexcept;

    /**
     * @brief Read spectrum data
     *
     * @param data Pointer to buffer to fill (256 bytes)
     * @return true if data was read successfully
     */
    bool read_spectrum(uint8_t* data) const noexcept;

    /**
     * @brief Check if data has been updated
     *
     * @return true if data has been updated since last read
     */
    bool is_data_updated() const noexcept;

    /**
     * @brief Clear updated flag
     */
    void clear_updated_flag() noexcept;
};

// ============================================================================
// IMPLEMENTATION: DroneSnapshot
// ============================================================================

inline void DroneSnapshot::increment_version() noexcept {
    // Revision #1: Use ChibiOS memory barriers instead of std::atomic
    chSysLock();
    ++version;
    chSysUnlock();
}

inline uint32_t DroneSnapshot::get_version() const noexcept {
    // Revision #1: Use ChibiOS memory barriers instead of std::atomic
    chSysLock();
    uint32_t v = version;
    chSysUnlock();
    return v;
}

// ============================================================================
// IMPLEMENTATION: SpectrumBuffer
// ============================================================================

inline SpectrumBuffer::SpectrumBuffer() noexcept
    : data_updated_(false)
    , update_timestamp_ms_(0) {
    // Initialize buffer to zeros
    for (size_t i = 0; i < 256; ++i) {
        buffer_[i] = 0;
    }
}

inline SpectrumBuffer::~SpectrumBuffer() noexcept {
    // Destructor body - no heap deallocation needed
}

inline void SpectrumBuffer::write_spectrum(const uint8_t* data, uint32_t timestamp_ms) noexcept {
    // Guard clause: Null pointer
    if (data == nullptr) {
        return;
    }

    // Acquire mutex for thread-safe write
    MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);

    // Copy spectrum data
    for (size_t i = 0; i < 256; ++i) {
        buffer_[i] = data[i];
    }

    // Update timestamp and flag
    update_timestamp_ms_ = timestamp_ms;
    data_updated_ = true;

    // Memory barrier ensures write visibility
    chSysLock();
    chSysUnlock();
}

inline bool SpectrumBuffer::read_spectrum(uint8_t* data) const noexcept {
    // Guard clause: Null pointer
    if (data == nullptr) {
        return false;
    }

    // Acquire mutex for thread-safe read
    MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);

    // Copy spectrum data
    for (size_t i = 0; i < 256; ++i) {
        data[i] = buffer_[i];
    }

    return true;
}

inline bool SpectrumBuffer::is_data_updated() const noexcept {
    // Read volatile flag with memory barrier
    chSysLock();
    bool updated = data_updated_;
    chSysUnlock();
    return updated;
}

inline void SpectrumBuffer::clear_updated_flag() noexcept {
    // Clear volatile flag with memory barrier
    chSysLock();
    data_updated_ = false;
    chSysUnlock();
}

} // namespace snapshot
} // namespace eda

#endif // EDA_SNAPSHOT_TYPES_HPP_
```

### eda_snapshot_manager.hpp

```cpp
/**
 * @file eda_snapshot_manager.hpp
 * @brief Enhanced Drone Analyzer - Snapshot Manager for UI/DSP Separation
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All snapshots use static storage
 * - No exceptions: All functions marked noexcept
 * - Thread-safe: ChibiOS mutexes for synchronization
 * - Guard clauses: Early returns for edge cases
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 *
 * @author Diamond Code Pipeline
 * @date 2026-03-02
 */

#ifndef EDA_SNAPSHOT_MANAGER_HPP_
#define EDA_SNAPSHOT_MANAGER_HPP_

#include <cstdint>
#include <cstddef>
#include "ch.h"
#include "eda_snapshot_types.hpp"
#include "eda_locking.hpp"

// ============================================================================
// NAMESPACE: eda::snapshot
// ============================================================================

namespace eda {
namespace snapshot {

/**
 * @brief Snapshot manager for UI/DSP separation
 *
 * Manages atomic snapshots between producer (DSP thread) and
 * consumer (UI thread). Uses double-buffering for lock-free reads.
 *
 * Two-phase rendering pattern:
 * 1. Producer writes to back buffer
 * 2. Producer sets valid flag and increments version
 * 3. Consumer reads from front buffer
 * 4. Consumer swaps buffers if new version available
 */
class SnapshotManager {
public:
    // =========================================================================
    // CONSTANTS
    // =========================================================================

    /**
     * @brief Number of snapshot buffers (double-buffering)
     */
    static constexpr size_t NUM_SNAPSHOTS = 2;

    /**
     * @brief Version counter initial value
     */
    static constexpr uint32_t INITIAL_VERSION = 1;

    // =========================================================================
    // CONSTRUCTORS / DESTRUCTORS
    // =========================================================================

    /**
     * @brief Constructor
     */
    SnapshotManager() noexcept;

    /**
     * @brief Destructor
     */
    ~SnapshotManager() noexcept;

    // Delete copy/move operations
    SnapshotManager(const SnapshotManager&) = delete;
    SnapshotManager& operator=(const SnapshotManager&) = delete;
    SnapshotManager(SnapshotManager&&) = delete;
    SnapshotManager& operator=(SnapshotManager&&) = delete;

    // =========================================================================
    // PUBLIC INTERFACE (Producer)
    // =========================================================================

    /**
     * @brief Write snapshot (producer thread)
     *
     * @param data Display data to write
     * @param thread_id Producer thread ID
     * @return true if write succeeded
     */
    bool write_snapshot(const DisplayData& data, uint8_t thread_id) noexcept;

    // =========================================================================
    // PUBLIC INTERFACE (Consumer)
    // =========================================================================

    /**
     * @brief Read snapshot (consumer thread)
     *
     * @param data Display data to fill
     * @param thread_id Consumer thread ID
     * @return true if read succeeded and data is valid
     */
    bool read_snapshot(DisplayData& data, uint8_t thread_id) noexcept;

    /**
     * @brief Get current snapshot version
     *
     * @return Current snapshot version
     */
    [[nodiscard]] uint32_t get_version() const noexcept;

    /**
     * @brief Check if new snapshot is available
     *
     * @return true if new snapshot is available
     */
    [[nodiscard]] bool is_new_snapshot_available() const noexcept;

private:
    // =========================================================================
    // PRIVATE MEMBERS
    // =========================================================================

    /**
     * @brief Snapshot buffers (double-buffering)
     */
    DroneSnapshot snapshots_[NUM_SNAPSHOTS];

    /**
     * @brief Current write buffer index (producer)
     */
    volatile size_t write_index_;

    /**
     * @brief Current read buffer index (consumer)
     */
    volatile size_t read_index_;

    /**
     * @brief Global version counter
     */
    volatile uint32_t version_counter_;

    /**
     * @brief Mutex for buffer swap
     */
    Mutex swap_mutex_;

    // =========================================================================
    // PRIVATE METHODS
    // =========================================================================

    /**
     * @brief Swap buffers (atomic operation)
     */
    void swap_buffers() noexcept;

    /**
     * @brief Get write buffer reference
     *
     * @return Reference to write buffer
     */
    DroneSnapshot& get_write_buffer() noexcept;

    /**
     * @brief Get read buffer reference
     *
     * @return Reference to read buffer
     */
    const DroneSnapshot& get_read_buffer() const noexcept;
};

// ============================================================================
// IMPLEMENTATION
// ============================================================================

inline SnapshotManager::SnapshotManager() noexcept
    : write_index_(0)
    , read_index_(1)
    , version_counter_(INITIAL_VERSION) {
    // Initialize snapshots
    for (size_t i = 0; i < NUM_SNAPSHOTS; ++i) {
        snapshots_[i].valid = false;
        snapshots_[i].version = 0;
        snapshots_[i].producer_thread_id = 0;
        snapshots_[i].last_consumer_thread_id = 0;
    }
}

inline SnapshotManager::~SnapshotManager() noexcept {
    // Destructor body - no heap deallocation needed
}

inline bool SnapshotManager::write_snapshot(const DisplayData& data, uint8_t thread_id) noexcept {
    // Guard clause: Null data pointer
    if (&data == nullptr) {
        return false;
    }

    // Get write buffer
    DroneSnapshot& snapshot = get_write_buffer();

    // Copy data to write buffer
    snapshot.data = data;

    // Set metadata
    snapshot.producer_thread_id = thread_id;
    snapshot.data.snapshot_timestamp_ms = chVTGetSystemTime();
    snapshot.data.snapshot_version = ++version_counter_;

    // Set valid flag with memory barrier
    chSysLock();
    snapshot.valid = true;
    chSysUnlock();

    // Swap buffers for next write
    swap_buffers();

    return true;
}

inline bool SnapshotManager::read_snapshot(DisplayData& data, uint8_t thread_id) noexcept {
    // Guard clause: Null data pointer
    if (&data == nullptr) {
        return false;
    }

    // Get read buffer
    const DroneSnapshot& snapshot = get_read_buffer();

    // Check if snapshot is valid
    chSysLock();
    bool valid = snapshot.valid;
    chSysUnlock();

    // Guard clause: Snapshot not valid yet
    if (!valid) {
        return false;
    }

    // Copy data from read buffer
    data = snapshot.data;

    // Update consumer thread ID
    snapshots_[read_index_].last_consumer_thread_id = thread_id;

    return true;
}

inline uint32_t SnapshotManager::get_version() const noexcept {
    // Read version counter with memory barrier
    chSysLock();
    uint32_t version = version_counter_;
    chSysUnlock();
    return version;
}

inline bool SnapshotManager::is_new_snapshot_available() const noexcept {
    // Get read buffer
    const DroneSnapshot& snapshot = get_read_buffer();

    // Check if snapshot version matches global version
    uint32_t snapshot_version = snapshot.get_version();
    uint32_t global_version = get_version();

    return snapshot_version < global_version;
}

inline void SnapshotManager::swap_buffers() noexcept {
    // Acquire mutex for atomic swap
    MutexLock lock(swap_mutex_, LockOrder::SNAPSHOT_MUTEX);

    // Swap indices
    size_t temp = write_index_;
    write_index_ = read_index_;
    read_index_ = temp;

    // Memory barrier ensures swap is visible
    chSysLock();
    chSysUnlock();
}

inline DroneSnapshot& SnapshotManager::get_write_buffer() noexcept {
    return snapshots_[write_index_];
}

inline const DroneSnapshot& SnapshotManager::get_read_buffer() const noexcept {
    return snapshots_[read_index_];
}

} // namespace snapshot
} // namespace eda

#endif // EDA_SNAPSHOT_MANAGER_HPP_
```

---

**End of Part 3** - This document continues in `stage4_diamond_synthesis_part4.md`
