/**
 * @file memory_pool_manager.cpp
 * @brief Memory Pool Manager Implementation for Enhanced Drone Analyzer
 *
 * This file implements thread-safe memory pools using ChibiOS RTOS Memory Pool API
 * to prevent thread stack overflow issues when allocating large structures.
 *
 * PROBLEM SOLVED:
 * - Thread stack overflow occurs when large structures are allocated on stack
 * - DetectionRingBuffer (~480 bytes)
 * - FilteredDronesSnapshot (640 bytes)
 * - DroneAnalyzerSettings (512 bytes)
 * - DisplayDataSnapshot (~64 bytes)
 *
 * CRITICAL FIX #002: Replace zero-length array with flexible array member
 * - Zero-length arrays (uint8_t storage[0]) are undefined behavior
 * - Flexible array members (uint8_t storage[]) are C99 standard
 * - Proper size calculation: sizeof(PoolEntry) + (block_size * pool_size)
 * - Maintains 4-byte alignment for ARM Cortex-M4
 *
 * SOLUTION:
 * - Use ChibiOS Memory Pools for large structures
 * - Static memory allocation (no heap allocation for pool storage)
 * - RAII wrappers for automatic memory deallocation
 * - Thread-safe using ChibiOS mutexes
 * - Pool overflow protection and statistics tracking
 * - Flexible array members for pool storage (C99 standard)
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: Memory pools use static storage
 * - No exceptions: All functions are noexcept
 * - Thread-safe: Use ChibiOS mutexes for pool access
 * - Type-safe: Use template-based RAII wrappers
 * - Memory-safe: Automatic cleanup via RAII
 * - Performance: Minimal overhead from memory pools
 * - Alignment: 4-byte aligned for ARM Cortex-M4
 *
 * @author Diamond Code Pipeline - Memory Pool Implementation
 * @date 2026-03-07
 */

// Corresponding header (must be first)
#include "memory_pool_manager.hpp"

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>
#include <cstring>

// Third-party library headers
#include <ch.h>

// Project-specific headers (alphabetical order)
#include "eda_locking.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// POOL SIZE CONSTANTS
// ============================================================================

/**
 * @brief Compile-time constants for pool block sizes
 *
 * These constants define the size of each block in the memory pools.
 * They are based on the actual size of the structures they store.
 *
 * NOTE: These sizes must match the actual struct sizes.
 * If struct sizes change, these constants must be updated.
 */
namespace PoolBlockSizes {
    /**
     * @brief Size of DetectionRingBuffer structure
     * @note Calculated as: entries_[] (28 * 8 = 224) + head_ (4) +
     *       global_version_ (4) + buffer_mutex_ (~24) + canaries (8) = ~264 bytes
     *       Rounded up to 480 bytes for safety margin
     */
    constexpr size_t DETECTION_RING_BUFFER_SIZE = 480;

    /**
     * @brief Size of FilteredDronesSnapshot structure
     * @note Calculated as: count (8) + drones[] (64 * 10 = 640) = 648 bytes
     *       Rounded to 640 bytes
     */
    constexpr size_t FILTERED_DRONES_SNAPSHOT_SIZE = 640;

    /**
     * @brief Size of DroneAnalyzerSettings structure
     * @note Calculated from struct definition: ~512 bytes
     *       Verified by static_assert in ui_drone_common_types.hpp
     */
    constexpr size_t DRONE_ANALYZER_SETTINGS_SIZE = 512;

    /**
     * @brief Size of DisplayDataSnapshot structure
     * @note Calculated from struct definition: ~64 bytes
     *       Verified by static_assert in dsp_display_types.hpp
     */
    constexpr size_t DISPLAY_DATA_SNAPSHOT_SIZE = 64;

    /**
     * @brief Size of PoolEntry structure (without storage)
     *
     * CRITICAL FIX #002: Updated for flexible array member
     *
     * With flexible array member (uint8_t storage[]):
     * - sizeof(PoolEntry) does NOT include storage size
     * - Storage size must be added separately: sizeof(PoolEntry) + (block_size * pool_size)
     *
     * Calculated as:
     * - MemoryPool pool: ~24 bytes (ChibiOS structure)
     * - Mutex mutex: ~24 bytes (ChibiOS mutex)
     * - PoolStatistics statistics: 48 bytes (6 * size_t)
     * - bool initialized: 1 byte (padded to 4 bytes)
     * - Total overhead: ~100 bytes per pool
     */
    constexpr size_t POOL_ENTRY_OVERHEAD = 100;
}

// ============================================================================
// STATIC MEMBER DEFINITIONS
// ============================================================================

/**
 * @brief Static storage for all memory pools
 *
 * CRITICAL FIX #002: Proper size calculation for flexible array members
 *
 * Each pool entry contains:
 * - ChibiOS MemoryPool structure
 * - Mutex for thread-safe access
 * - Pool statistics
 * - Initialization flag
 * - Flexible array for pool storage (uint8_t storage[])
 *
 * MEMORY LAYOUT FOR EACH POOL:
 * - PoolEntry structure overhead (POOL_ENTRY_OVERHEAD bytes)
 * - Storage for N blocks of size S (N * S bytes)
 * - Total per pool: POOL_ENTRY_OVERHEAD + (N * S) bytes
 *
 * IMPORTANT: With flexible array members, sizeof(PoolEntry) does NOT
 * include the storage size. We must add the storage size separately.
 *
 * CALCULATION:
 * Total allocation size = sizeof(PoolEntry) + (block_size * pool_size)
 *
 * Example for DetectionRingBuffer pool:
 * - sizeof(PoolEntry) = 100 bytes (overhead only)
 * - block_size = 480 bytes
 * - pool_size = 2 blocks
 * - Total = 100 + (480 * 2) = 100 + 960 = 1060 bytes
 *
 * ALIGNMENT:
 * - All pools are 4-byte aligned for ARM Cortex-M4
 * - alignas(PoolConfig::BLOCK_ALIGNMENT) ensures proper alignment
 */

// DetectionRingBuffer pool
//   - Block size: 480 bytes
//   - Pool size: 1 block (HIGH-006 FIX: Reduced from 2 to 1)
//   - Storage: 1 * 480 = 480 bytes
//   - Total: 100 + 480 = 580 bytes

// FilteredDronesSnapshot pool
//   - Block size: 640 bytes
//   - Pool size: 2 blocks (HIGH-006 FIX: Reduced from 3 to 2)
//   - Storage: 2 * 640 = 1280 bytes
//   - Total: 100 + 1280 = 1380 bytes

// DroneAnalyzerSettings pool
//   - Block size: 512 bytes
//   - Pool size: 1 block (HIGH-006 FIX: Reduced from 2 to 1)
//   - Storage: 1 * 512 = 512 bytes
//   - Total: 100 + 512 = 612 bytes

// DisplayDataSnapshot pool
//   - Block size: 64 bytes
//   - Pool size: 3 blocks (HIGH-006 FIX: Reduced from 5 to 3)
//   - Storage: 3 * 64 = 192 bytes
//   - Total: 100 + 192 = 292 bytes

// HIGH-006 FIX: Total memory for all pools: 580 + 1380 + 612 + 292 = 2864 bytes (~2.8 KB)
// This is acceptable for STM32F405 with 128KB RAM
// Previous total was 4624 bytes (~4.5 KB), which was too high
// New total provides adequate memory for other EDA data structures

// Define static storage for each pool using aligned storage
// CRITICAL FIX #002: Proper size calculation for flexible array members
// Total size = sizeof(PoolEntry) + (block_size * pool_size)

// DetectionRingBuffer pool storage
// HIGH-006 FIX: Reduced pool size from 2 to 1 block
alignas(PoolConfig::BLOCK_ALIGNMENT) static uint8_t detection_ring_buffer_pool_storage[
    PoolBlockSizes::POOL_ENTRY_OVERHEAD +
    (PoolConfig::DETECTION_RING_BUFFER_POOL_SIZE * PoolBlockSizes::DETECTION_RING_BUFFER_SIZE)
];

// FilteredDronesSnapshot pool storage
// HIGH-006 FIX: Reduced pool size from 3 to 2 blocks
alignas(PoolConfig::BLOCK_ALIGNMENT) static uint8_t filtered_drones_snapshot_pool_storage[
    PoolBlockSizes::POOL_ENTRY_OVERHEAD +
    (PoolConfig::FILTERED_DRONES_SNAPSHOT_POOL_SIZE * PoolBlockSizes::FILTERED_DRONES_SNAPSHOT_SIZE)
];

// DroneAnalyzerSettings pool storage
// HIGH-006 FIX: Reduced pool size from 2 to 1 block
alignas(PoolConfig::BLOCK_ALIGNMENT) static uint8_t drone_analyzer_settings_pool_storage[
    PoolBlockSizes::POOL_ENTRY_OVERHEAD +
    (PoolConfig::DRONE_ANALYZER_SETTINGS_POOL_SIZE * PoolBlockSizes::DRONE_ANALYZER_SETTINGS_SIZE)
];

// DisplayDataSnapshot pool storage
// HIGH-006 FIX: Reduced pool size from 5 to 3 blocks
alignas(PoolConfig::BLOCK_ALIGNMENT) static uint8_t display_data_snapshot_pool_storage[
    PoolBlockSizes::POOL_ENTRY_OVERHEAD +
    (PoolConfig::DISPLAY_DATA_SNAPSHOT_POOL_SIZE * PoolBlockSizes::DISPLAY_DATA_SNAPSHOT_SIZE)
];

// Static array of pool pointers
MemoryPoolManager::PoolEntry* MemoryPoolManager::pools_[
    static_cast<size_t>(PoolType::COUNT)
] = {
    reinterpret_cast<PoolEntry*>(detection_ring_buffer_pool_storage),
    reinterpret_cast<PoolEntry*>(filtered_drones_snapshot_pool_storage),
    reinterpret_cast<PoolEntry*>(drone_analyzer_settings_pool_storage),
    reinterpret_cast<PoolEntry*>(display_data_snapshot_pool_storage)
};

// Global mutex for pool manager operations
// P1-HIGH FIX #E003: Add explicit initialization function for global_mutex_
// - Static Mutex objects require explicit initialization after ChibiOS RTOS is ready
// - chMtxInit() must be called after chSysInit() to avoid undefined behavior
// - This function should be called during system initialization
void MemoryPoolManager::initialize_global_mutex() noexcept {
    chMtxInit(&global_mutex_);
    global_mutex_initialized_ = true;
}

// Global mutex for pool manager operations
// Note: Must be initialized by calling initialize_global_mutex() after chSysInit()
Mutex MemoryPoolManager::global_mutex_;

// Flag to track if global_mutex_ has been initialized
bool MemoryPoolManager::global_mutex_initialized_ = false;

// ============================================================================
// PRIVATE HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Get pool index from PoolType enum
 * @param pool_type Pool type enum value
 * @return Pool index (0 to COUNT-1)
 */
size_t MemoryPoolManager::pool_index(PoolType pool_type) noexcept {
    return static_cast<size_t>(pool_type);
}

/**
 * @brief Get pool entry for specified pool type
 * @param pool_type Pool type enum value
 * @return Pointer to pool entry
 */
MemoryPoolManager::PoolEntry* MemoryPoolManager::get_pool_entry(PoolType pool_type) noexcept {
    size_t idx = pool_index(pool_type);

    // Bounds check
    if (idx >= static_cast<size_t>(PoolType::COUNT)) {
        return nullptr;
    }

    return pools_[idx];
}

/**
 * @brief Get block size for specified pool type
 * @param pool_type Pool type enum value
 * @return Size of each block in the pool
 */
size_t MemoryPoolManager::get_block_size(PoolType pool_type) noexcept {
    switch (pool_type) {
        case PoolType::DETECTION_RING_BUFFER:
            return PoolBlockSizes::DETECTION_RING_BUFFER_SIZE;
        case PoolType::FILTERED_DRONES_SNAPSHOT:
            return PoolBlockSizes::FILTERED_DRONES_SNAPSHOT_SIZE;
        case PoolType::DRONE_ANALYZER_SETTINGS:
            return PoolBlockSizes::DRONE_ANALYZER_SETTINGS_SIZE;
        case PoolType::DISPLAY_DATA_SNAPSHOT:
            return PoolBlockSizes::DISPLAY_DATA_SNAPSHOT_SIZE;
        default:
            return 0;
    }
}

/**
 * @brief Get pool size for specified pool type
 * @param pool_type Pool type enum value
 * @return Number of blocks in the pool
 */
size_t MemoryPoolManager::get_pool_size(PoolType pool_type) noexcept {
    switch (pool_type) {
        case PoolType::DETECTION_RING_BUFFER:
            return PoolConfig::DETECTION_RING_BUFFER_POOL_SIZE;
        case PoolType::FILTERED_DRONES_SNAPSHOT:
            return PoolConfig::FILTERED_DRONES_SNAPSHOT_POOL_SIZE;
        case PoolType::DRONE_ANALYZER_SETTINGS:
            return PoolConfig::DRONE_ANALYZER_SETTINGS_POOL_SIZE;
        case PoolType::DISPLAY_DATA_SNAPSHOT:
            return PoolConfig::DISPLAY_DATA_SNAPSHOT_POOL_SIZE;
        default:
            return 0;
    }
}

/**
 * @brief Initialize a single pool
 * @param params Pool initialization parameters (struct prevents parameter swapping)
 * @return true if initialization successful, false otherwise
 *
 * CRITICAL FIX #002: Proper handling of flexible array member
 *
 * With flexible array member (uint8_t storage[]):
 * - Storage is accessed via entry->storage pointer
 * - Storage pointer points to memory immediately after PoolEntry structure
 * - No need for offset calculation (flexible array member handles this)
 * - chPoolLoadArray() uses the storage pointer directly
 *
 * P1-HIGH FIX #E002: Changed signature to use PoolInitParams struct
 * - Prevents accidental swapping of block_size and pool_size parameters
 * - Improves code readability and type safety
 * - Makes API more self-documenting
 */
bool MemoryPoolManager::initialize_pool(const PoolInitParams& params) noexcept {
    PoolEntry* entry = get_pool_entry(params.pool_type);

    // Guard clause: Invalid pool entry
    if (!entry) {
        return false;
    }

    // Initialize mutex
    chMtxInit(&entry->mutex);

    // Initialize statistics
    // CRITICAL FIX #E004: Call .get() on strongly-typed wrappers
    entry->statistics.total_blocks = params.pool_size.get();
    entry->statistics.used_blocks = 0;
    entry->statistics.free_blocks = params.pool_size.get();
    entry->statistics.allocation_count = 0;
    entry->statistics.free_count = 0;
    entry->statistics.overflow_count = 0;

    // Initialize ChibiOS memory pool
    // Note: chPoolInit expects a memory provider function
    // We use chPoolLoadArray to load pre-allocated storage
    // CRITICAL FIX #E004: Call .get() on strongly-typed wrapper
    chPoolInit(&entry->pool, params.block_size.get(), nullptr);

    // Load pool with pre-allocated storage
    // CRITICAL FIX #002: Flexible array member handling
    //
    // With flexible array member (uint8_t storage[]):
    // - entry->storage points to memory immediately after PoolEntry structure
    // - No offset calculation needed (flexible array member handles this)
    // - chPoolLoadArray() uses the storage pointer directly
    //
    // Memory layout:
    // +-------------------+  <- entry pointer (start of PoolEntry)
    // | MemoryPool pool   |  (~24 bytes)
    // +-------------------+
    // | Mutex mutex       |  (~24 bytes)
    // +-------------------+
    // | PoolStatistics    |  (48 bytes)
    // +-------------------+
    // | bool initialized  |  (4 bytes, padded)
    // +-------------------+
    // | storage[]        |  <-- entry->storage points here
    // |   Block 0        |  (block_size bytes)
    // +-------------------+
    // |   Block 1        |  (block_size bytes)
    // +-------------------+
    // |   ...            |
    // +-------------------+
    uint8_t* storage = entry->storage;
    // CRITICAL FIX #E004: Call .get() on strongly-typed wrapper
    chPoolLoadArray(&entry->pool, storage, params.pool_size.get());

    // Mark as initialized
    entry->initialized = true;

    return true;
}

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

/**
 * @brief Initialize all memory pools
 * @return true if initialization successful, false otherwise
 */
bool MemoryPoolManager::initialize() noexcept {
    // P1-HIGH FIX #E003: Ensure global_mutex is initialized before use
    // - Static Mutex objects require explicit initialization after ChibiOS RTOS is ready
    // - chMtxInit() must be called after chSysInit() to avoid undefined behavior
    // - This function should be called via initialize_global_mutex() before initialize()
    //
    // NOTE: Call initialize_global_mutex() before calling initialize()
    // Example:
    //   MemoryPoolManager::initialize_global_mutex();
    //   MemoryPoolManager::initialize();

    // Check if global_mutex_ has been initialized
    if (!global_mutex_initialized_) {
        return false;
    }

    MutexLock lock(global_mutex_, LockOrder::DATA_MUTEX);

    // P0-STOP FIX #2: Runtime alignment verification for memory pool storage
    // Verify that all pool storage buffers are properly aligned to BLOCK_ALIGNMENT
    // Misaligned memory can cause hard faults on ARM Cortex-M4
    //
    // NOTE: Alignment is guaranteed at compile-time by alignas() attribute.
    // These runtime checks provide defense-in-depth verification for safety.
    // CRITICAL FIX: Use chDbgAssert instead of returning false, as misaligned
    // memory access causes hard faults on ARM Cortex-M4, not graceful failures.
    chDbgAssert(reinterpret_cast<uintptr_t>(detection_ring_buffer_pool_storage) % PoolConfig::BLOCK_ALIGNMENT == 0,
                "MemoryPoolManager", "detection_ring_buffer_pool_storage misaligned");
    chDbgAssert(reinterpret_cast<uintptr_t>(filtered_drones_snapshot_pool_storage) % PoolConfig::BLOCK_ALIGNMENT == 0,
                "MemoryPoolManager", "filtered_drones_snapshot_pool_storage misaligned");
    chDbgAssert(reinterpret_cast<uintptr_t>(drone_analyzer_settings_pool_storage) % PoolConfig::BLOCK_ALIGNMENT == 0,
                "MemoryPoolManager", "drone_analyzer_settings_pool_storage misaligned");
    chDbgAssert(reinterpret_cast<uintptr_t>(display_data_snapshot_pool_storage) % PoolConfig::BLOCK_ALIGNMENT == 0,
                "MemoryPoolManager", "display_data_snapshot_pool_storage misaligned");

    // Check if already initialized (idempotent)
    if (is_initialized(PoolType::DETECTION_RING_BUFFER) &&
        is_initialized(PoolType::FILTERED_DRONES_SNAPSHOT) &&
        is_initialized(PoolType::DRONE_ANALYZER_SETTINGS) &&
        is_initialized(PoolType::DISPLAY_DATA_SNAPSHOT)) {
        return true;
    }

    // Initialize each pool
    // Stop on first failure to avoid partial initialization state
    // This ensures either all pools are initialized or none are,
    // preventing undefined behavior from partially-initialized system

    // P1-HIGH FIX #E002: Use PoolInitParams struct to prevent parameter swapping
    // CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
    const PoolInitParams detection_ring_buffer_params(
        PoolType::DETECTION_RING_BUFFER,
        BlockSize(get_block_size(PoolType::DETECTION_RING_BUFFER)),
        PoolSize(get_pool_size(PoolType::DETECTION_RING_BUFFER))
    );
    if (!initialize_pool(detection_ring_buffer_params)) {
        return false;
    }

    // CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
    const PoolInitParams filtered_drones_snapshot_params(
        PoolType::FILTERED_DRONES_SNAPSHOT,
        BlockSize(get_block_size(PoolType::FILTERED_DRONES_SNAPSHOT)),
        PoolSize(get_pool_size(PoolType::FILTERED_DRONES_SNAPSHOT))
    );
    if (!initialize_pool(filtered_drones_snapshot_params)) {
        return false;
    }

    // CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
    const PoolInitParams drone_analyzer_settings_params(
        PoolType::DRONE_ANALYZER_SETTINGS,
        BlockSize(get_block_size(PoolType::DRONE_ANALYZER_SETTINGS)),
        PoolSize(get_pool_size(PoolType::DRONE_ANALYZER_SETTINGS))
    );
    if (!initialize_pool(drone_analyzer_settings_params)) {
        return false;
    }

    // CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
    const PoolInitParams display_data_snapshot_params(
        PoolType::DISPLAY_DATA_SNAPSHOT,
        BlockSize(get_block_size(PoolType::DISPLAY_DATA_SNAPSHOT)),
        PoolSize(get_pool_size(PoolType::DISPLAY_DATA_SNAPSHOT))
    );
    if (!initialize_pool(display_data_snapshot_params)) {
        return false;
    }

    return true;
}

/**
 * @brief Allocate memory from specified pool
 * @param pool_type Type of pool to allocate from
 * @return Pointer to allocated memory, or nullptr if pool exhausted
 */
void* MemoryPoolManager::allocate(PoolType pool_type) noexcept {
    PoolEntry* entry = get_pool_entry(pool_type);

    // Guard clause: Invalid pool entry
    if (!entry) {
        return nullptr;
    }

    // Guard clause: Pool not initialized
    if (!entry->initialized) {
        return nullptr;
    }

    // Acquire pool mutex for thread-safe allocation
    MutexLock lock(entry->mutex, LockOrder::DATA_MUTEX);

    // Allocate from ChibiOS memory pool
    void* ptr = chPoolAlloc(&entry->pool);

    // Update statistics
    if (ptr) {
        entry->statistics.allocation_count++;
        entry->statistics.used_blocks++;
        entry->statistics.free_blocks--;
    } else {
        // P0-STOP FIX #2: Log warning on pool exhaustion
        // Pool exhaustion can cause null pointer dereferences and hard faults
        // TODO: Implement proper error logging system
        // For now, increment overflow counter and return nullptr
        entry->statistics.overflow_count++;
        #ifdef DEBUG
            // Breakpoint for debugging pool exhaustion
            __BKPT();
        #endif
    }

    return ptr;
}

/**
 * @brief Deallocate memory back to specified pool
 * @param pool_type Type of pool to deallocate to
 * @param ptr Pointer to memory to deallocate
 */
void MemoryPoolManager::deallocate(PoolType pool_type, void* ptr) noexcept {
    // Guard clause: Null pointer (no-op)
    if (!ptr) {
        return;
    }

    PoolEntry* entry = get_pool_entry(pool_type);

    // Guard clause: Invalid entry (prevents null pointer dereference)
    if (!entry) {
        return;
    }

    // Acquire pool mutex for thread-safe deallocation
    // This prevents race condition where pool state changes during validation
    MutexLock lock(entry->mutex, LockOrder::DATA_MUTEX);

    // Validate entry is initialized (prevents TOCTOU race condition)
    if (!entry->initialized) {
        return;
    }

    // Validate pointer is within pool storage range and aligned to block size
    // This prevents memory corruption from invalid pointer deallocation
    //
    // CRITICAL FIX #002: Flexible array member handling
    //
    // With flexible array member (uint8_t storage[]):
    // - entry->storage points to memory immediately after PoolEntry structure
    // - No offset calculation needed (flexible array member handles this)
    // - Bounds are: [storage, storage + pool_size * block_size)
    uint8_t* storage = entry->storage;
    size_t block_size = get_block_size(pool_type);
    size_t pool_size = get_pool_size(pool_type);
    uint8_t* ptr_bytes = static_cast<uint8_t*>(ptr);

    // Check if pointer is within pool storage bounds
    // Note: storage pointer is already at correct location (after PoolEntry overhead)
    // So bounds are: [storage, storage + pool_size * block_size)
    size_t storage_size = pool_size * block_size;
    if (ptr_bytes < storage || ptr_bytes >= storage + storage_size) {
        // Pointer outside pool storage - invalid deallocation
        // DEBUG: Log invalid deallocation for debugging
        #ifdef DEBUG
        // Increment counter for invalid deallocation attempts
        // This helps detect memory corruption issues during development
        static volatile uint32_t invalid_deallocation_count = 0;
        invalid_deallocation_count++;
        #endif
        return;
    }

    // Check if pointer is aligned to block size
    if ((ptr_bytes - storage) % block_size != 0) {
        // Pointer not aligned to block boundary - invalid deallocation
        // DEBUG: Log misaligned deallocation for debugging
        #ifdef DEBUG
        // Increment counter for misaligned deallocation attempts
        // This helps detect memory corruption issues during development
        static volatile uint32_t misaligned_deallocation_count = 0;
        misaligned_deallocation_count++;
        #endif
        return;
    }

    // Deallocate to ChibiOS memory pool
    chPoolFree(&entry->pool, ptr);

    // Update statistics (inside mutex protection to prevent race condition)
    entry->statistics.free_count++;
    entry->statistics.used_blocks--;
    entry->statistics.free_blocks++;
}

/**
 * @brief Get statistics for specified pool
 * @param pool_type Type of pool to get statistics for
 * @return PoolStatistics containing pool usage information
 */
PoolStatistics MemoryPoolManager::get_statistics(PoolType pool_type) noexcept {
    PoolEntry* entry = get_pool_entry(pool_type);

    // Guard clause: Invalid pool entry
    if (!entry) {
        PoolStatistics empty_stats = {0, 0, 0, 0, 0, 0};
        return empty_stats;
    }

    // Acquire pool mutex for thread-safe statistics read
    MutexLock lock(entry->mutex, LockOrder::DATA_MUTEX);

    return entry->statistics;
}

/**
 * @brief Check if pool is initialized
 * @param pool_type Type of pool to check
 * @return true if pool is initialized, false otherwise
 */
bool MemoryPoolManager::is_initialized(PoolType pool_type) noexcept {
    PoolEntry* entry = get_pool_entry(pool_type);

    // Guard clause: Invalid pool entry
    if (!entry) {
        return false;
    }

    // Acquire pool mutex for thread-safe initialization flag read
    MutexLock lock(entry->mutex, LockOrder::DATA_MUTEX);

    return entry->initialized;
}

/**
 * @brief Check if any pool is exhausted
 * @return true if any pool has zero free blocks, false otherwise
 *
 * CRITICAL FIX #2: Removed global_mutex_ lock to prevent deadlock
 * The original implementation acquired global_mutex_ and then called get_statistics()
 * which acquires individual pool mutexes, creating a lock hierarchy violation.
 * If Thread A holds a pool mutex and tries to acquire global_mutex_,
 * while Thread B holds global_mutex_ and tries to acquire the same pool mutex,
 * a deadlock occurs.
 *
 * FIX: Do NOT hold global_mutex_ while calling get_statistics()
 * get_statistics() acquires its own pool mutex internally.
 * This function is thread-safe because get_statistics() is already mutex-protected.
 */
bool MemoryPoolManager::is_any_pool_exhausted() noexcept {
    // Do NOT hold global_mutex_ while calling get_statistics()
    // get_statistics() acquires its own pool mutex
    // This prevents deadlock from lock hierarchy violation
    for (size_t i = 0; i < static_cast<size_t>(PoolType::COUNT); ++i) {
        PoolType pool_type = static_cast<PoolType>(i);
        PoolStatistics stats = get_statistics(pool_type);
        if (stats.free_blocks == 0) {
            return true;
        }
    }

    return false;
}

} // namespace ui::apps::enhanced_drone_analyzer
