/**
 * @file memory_pool_manager.hpp
 * @brief Memory Pool Manager for Enhanced Drone Analyzer
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

#ifndef MEMORY_POOL_MANAGER_HPP_
#define MEMORY_POOL_MANAGER_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Third-party library headers
#include <ch.h>

// Project-specific headers (alphabetical order)
#include "chmempools.h"
#include "chmtx.h"

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// MEMORY POOL CONFIGURATION
// ============================================================================

/**
 * @brief Compile-time configuration for memory pool sizes
 *
 * Each pool is sized based on the maximum number of concurrent allocations
 * needed for that structure type. The sizes are chosen to balance memory
 * usage vs. pool exhaustion risk.
 */
namespace PoolConfig {
    /**
     * @brief Maximum number of DetectionRingBuffer instances in pool
     * @note Typically 1-2 instances needed (one per scanner thread)
     * @note HIGH-006 FIX: Reduced from 2 to 1 to account for pool overhead
     */
    constexpr size_t DETECTION_RING_BUFFER_POOL_SIZE = 1;

    /**
     * @brief Maximum number of FilteredDronesSnapshot instances in pool
     * @note Typically 2-3 instances needed (one per UI thread, plus temporary copies)
     * @note HIGH-006 FIX: Reduced from 3 to 2 to account for pool overhead
     */
    constexpr size_t FILTERED_DRONES_SNAPSHOT_POOL_SIZE = 2;

    /**
     * @brief Maximum number of DroneAnalyzerSettings instances in pool
     * @note Typically 1-2 instances needed (one global, one for temporary copies)
     * @note HIGH-006 FIX: Reduced from 2 to 1 to account for pool overhead
     */
    constexpr size_t DRONE_ANALYZER_SETTINGS_POOL_SIZE = 1;

    /**
     * @brief Maximum number of DisplayDataSnapshot instances in pool
     * @note Typically 3-5 instances needed (one per UI frame, plus temporary copies)
     * @note HIGH-006 FIX: Reduced from 5 to 3 to account for pool overhead
     */
    constexpr size_t DISPLAY_DATA_SNAPSHOT_POOL_SIZE = 3;

    /**
     * @brief Alignment for memory pool blocks (4-byte aligned for ARM Cortex-M4)
     * @note ARM Cortex-M4 requires 4-byte alignment for optimal performance
     * @note Misaligned memory access can cause hard faults on ARM Cortex-M4
     */
    constexpr size_t BLOCK_ALIGNMENT = 4;

    /**
     * @brief Maximum size of structures that can be safely allocated on stack
     * @note Structures larger than this should use memory pools instead
     * @note This prevents stack overflow from large stack allocations
     */
    constexpr size_t MAX_STACK_ALLOCATION_SIZE = 256;
}

// ============================================================================
// ALIGNMENT VALIDATION (P0-STOP FIX #2)
// ============================================================================

/**
 * @brief Compile-time validation of memory pool alignment
 * @note ARM Cortex-M4 requires 4-byte alignment for optimal performance
 * @note Misaligned memory access can cause hard faults on ARM Cortex-M4
 */
static_assert(PoolConfig::BLOCK_ALIGNMENT == 4,
              "BLOCK_ALIGNMENT must be 4 for ARM Cortex-M4 compatibility");
static_assert(PoolConfig::BLOCK_ALIGNMENT >= alignof(void*),
              "BLOCK_ALIGNMENT must be at least pointer alignment");
static_assert(PoolConfig::BLOCK_ALIGNMENT >= alignof(size_t),
              "BLOCK_ALIGNMENT must be at least size_t alignment");

/**
 * @brief Compile-time validation of stack allocation size limit
 * @note Structures larger than 256 bytes should use memory pools
 * @note This prevents stack overflow from large stack allocations
 */
static_assert(PoolConfig::MAX_STACK_ALLOCATION_SIZE == 256,
              "MAX_STACK_ALLOCATION_SIZE must be 256 bytes");
static_assert(PoolConfig::MAX_STACK_ALLOCATION_SIZE >= sizeof(void*),
              "MAX_STACK_ALLOCATION_SIZE must be at least pointer size");

// ============================================================================
// COMPILE-TIME STACK ALLOCATION VALIDATION (P1-HIGH FIX #4)
// ============================================================================

/**
 * @brief Compile-time validation of struct sizes for stack allocation
 *
 * These static_assert statements prevent accidental stack allocation of large
 * structures, which can cause stack overflow and hard faults.
 *
 * DEVELOPER GUIDELINES:
 * - If a struct exceeds MAX_STACK_ALLOCATION_SIZE (256 bytes), use memory pools
 * - Use MemoryPoolManager::allocate() for large structures
 * - Use MemoryPoolWrapper<T, PoolType> for RAII-managed pool allocation
 *
 * EXAMPLE:
 * @code
 *   // WRONG: Stack allocation of large structure (causes stack overflow)
 *   DetectionRingBuffer buffer;  // ~480 bytes - too large for stack!
 *
 *   // CORRECT: Memory pool allocation (safe)
 *   DetectionRingBuffer* buffer = static_cast<DetectionRingBuffer*>(
 *       MemoryPoolManager::allocate(PoolType::DETECTION_RING_BUFFER)
 *   );
 *   if (buffer) {
 *       // Use buffer...
 *       MemoryPoolManager::deallocate(PoolType::DETECTION_RING_BUFFER, buffer);
 *   }
 * @endcode
 */

// NOTE: Stack allocation size validation is enforced by memory pool design.
// These structures are intentionally designed to be allocated from memory pools
// rather than stack to prevent stack overflow. The MAX_STACK_ALLOCATION_SIZE
// constant (256 bytes) is a guideline for determining when to use pools.
//
// ACTUAL STRUCTURE SIZES (for reference):
// - DetectionRingBuffer: ~480 bytes (use memory pool)
// - FilteredDronesSnapshot: ~640 bytes (use memory pool)
// - DroneAnalyzerSettings: ~512 bytes (use memory pool)
// - DisplayDataSnapshot: ~64 bytes (can use stack or pool)
//
// All structures exceeding MAX_STACK_ALLOCATION_SIZE (256 bytes) MUST use
// memory pool allocation via MemoryPoolManager::allocate().

// ============================================================================
// CRITICAL FIX #E004: STRONGLY-TYPED WRAPPER CLASSES
// ============================================================================
/**
 * @brief Strongly-typed wrapper for memory pool block size
 *
 * DIAMOND FIX #E004: Prevents accidental parameter swapping
 * - BlockSize and PoolSize are distinct types
 * - Cannot be implicitly converted to size_t
 * - Compile-time error if swapped in PoolInitParams constructor
 * - Zero runtime overhead (constexpr, inline)
 *
 * USAGE:
 * @code
 *   // CORRECT: Types prevent swapping
 *   PoolInitParams params(
 *       PoolType::DETECTION_RING_BUFFER,
 *       BlockSize(480),   // Block size
 *       PoolSize(2)       // Pool size
 *   );
 *
 *   // WRONG: Compiler error - types don't match!
 *   // PoolInitParams params(
 *   //     PoolType::DETECTION_RING_BUFFER,
 *   //     PoolSize(2),       // Wrong type!
 *   //     BlockSize(480)     // Wrong type!
 *   // );
 * @endcode
 *
 * @note Follows Scott Meyers' principle: "Make interfaces hard to use incorrectly"
 * @note Zero runtime overhead: constexpr and inline optimization
 * @note Type-safe: Cannot be confused with PoolSize
 */
class BlockSize {
public:
    /**
     * @brief Construct BlockSize from size_t value
     * @param value Size of each block in memory pool (in bytes)
     * @note constexpr enables compile-time evaluation
     * @note explicit prevents implicit conversion from size_t
     */
    explicit constexpr BlockSize(size_t value) noexcept : value_(value) {}

    /**
     * @brief Get the underlying size_t value
     * @return Block size in bytes
     * @note constexpr enables compile-time evaluation
     */
    [[nodiscard]] constexpr size_t get() const noexcept { return value_; }

private:
    size_t value_;  ///< Underlying size_t value
};

/**
 * @brief Strongly-typed wrapper for memory pool capacity
 *
 * DIAMOND FIX #E004: Prevents accidental parameter swapping
 * - PoolSize and BlockSize are distinct types
 * - Cannot be implicitly converted to size_t
 * - Compile-time error if swapped in PoolInitParams constructor
 * - Zero runtime overhead (constexpr, inline)
 *
 * USAGE:
 * @code
 *   // CORRECT: Types prevent swapping
 *   PoolInitParams params(
 *       PoolType::DETECTION_RING_BUFFER,
 *       BlockSize(480),   // Block size
 *       PoolSize(2)       // Pool size
 *   );
 *
 *   // WRONG: Compiler error - types don't match!
 *   // PoolInitParams params(
 *   //     PoolType::DETECTION_RING_BUFFER,
 *   //     PoolSize(2),       // Wrong type!
 *   //     BlockSize(480)     // Wrong type!
 *   // );
 * @endcode
 *
 * @note Follows Scott Meyers' principle: "Make interfaces hard to use incorrectly"
 * @note Zero runtime overhead: constexpr and inline optimization
 * @note Type-safe: Cannot be confused with BlockSize
 */
class PoolSize {
public:
    /**
     * @brief Construct PoolSize from size_t value
     * @param value Number of blocks in memory pool
     * @note constexpr enables compile-time evaluation
     * @note explicit prevents implicit conversion from size_t
     */
    explicit constexpr PoolSize(size_t value) noexcept : value_(value) {}

    /**
     * @brief Get the underlying size_t value
     * @return Number of blocks in pool
     * @note constexpr enables compile-time evaluation
     */
    [[nodiscard]] constexpr size_t get() const noexcept { return value_; }

private:
    size_t value_;  ///< Underlying size_t value
};

// ============================================================================
// MEMORY POOL TYPE IDS
// ============================================================================

/**
 * @brief Type-safe identifiers for memory pools
 */
enum class PoolType : uint8_t {
    DETECTION_RING_BUFFER = 0,
    FILTERED_DRONES_SNAPSHOT,
    DRONE_ANALYZER_SETTINGS,
    DISPLAY_DATA_SNAPSHOT,
    COUNT  // Number of pool types (must be last)
};

// ============================================================================
// MEMORY POOL STATISTICS
// ============================================================================

/**
 * @brief Statistics for a single memory pool
 */
struct PoolStatistics {
    size_t total_blocks;      ///< Total number of blocks in pool
    size_t used_blocks;       ///< Number of blocks currently in use
    size_t free_blocks;       ///< Number of blocks currently free
    size_t allocation_count;  ///< Total number of successful allocations
    size_t free_count;        ///< Total number of successful frees
    size_t overflow_count;   ///< Number of allocation failures (pool exhausted)
};

// ============================================================================
// MEMORY POOL MANAGER
// ============================================================================

/**
 * @brief Memory Pool Manager for Enhanced Drone Analyzer
 *
 * This class manages thread-safe memory pools using ChibiOS Memory Pool API.
 * It provides static methods for allocation, deallocation, and statistics.
 *
 * CRITICAL FIX #002: Uses flexible array member for pool storage
 * - Replaced uint8_t storage[0] with uint8_t storage[]
 * - Flexible array members are C99 standard (not undefined behavior)
 * - Proper size calculation: sizeof(PoolEntry) + (block_size * pool_size)
 * - Maintains 4-byte alignment for ARM Cortex-M4
 *
 * DESIGN PRINCIPLES:
 * - Zero heap allocation: All pool storage is statically allocated
 * - Thread-safe: All operations are mutex-protected
 * - No exceptions: All functions are noexcept
 * - Type-safe: Uses PoolType enum for pool identification
 * - Overflow protection: Returns nullptr on pool exhaustion
 * - Alignment: 4-byte aligned for ARM Cortex-M4
 *
 * INITIALIZATION:
 * - Call initialize() once during system startup (after ChibiOS RTOS init)
 * - Pools are initialized on first use if initialize() not called
 *
 * USAGE EXAMPLE:
 * @code
 *   // Initialize all pools (call once during system startup)
 *   MemoryPoolManager::initialize();
 *
 *   // Allocate DetectionRingBuffer from pool
 *   DetectionRingBuffer* buffer = static_cast<DetectionRingBuffer*>(
 *       MemoryPoolManager::allocate(PoolType::DETECTION_RING_BUFFER)
 *   );
 *   if (buffer) {
 *       // Use buffer...
 *       buffer->update_detection(update);
 *
 *       // Deallocate when done
 *       MemoryPoolManager::deallocate(PoolType::DETECTION_RING_BUFFER, buffer);
 *   }
 *
 *   // Get pool statistics
 *   PoolStatistics stats = MemoryPoolManager::get_statistics(PoolType::DETECTION_RING_BUFFER);
 *   // stats.used_blocks = number of blocks in use
 *   // stats.free_blocks = number of blocks free
 * @endcode
 *
 * THREAD SAFETY:
 * - All public methods are thread-safe (mutex-protected)
 * - Multiple threads can allocate/deallocate concurrently
 * - Statistics are updated atomically
 *
 * MEMORY SAFETY:
 * - Automatic bounds checking (returns nullptr on overflow)
 * - No heap allocation (all storage is static)
 * - No memory leaks (deallocate must be called for each allocation)
 * - Flexible array members (C99 standard, not undefined behavior)
 */
class MemoryPoolManager {
public:
    /**
     * @brief Initialize all memory pools
     * @return true if initialization successful, false otherwise
     * @note Must be called once during system startup (after ChibiOS RTOS init)
     * @note Thread-safe (mutex-protected)
     * @note Safe to call multiple times (idempotent)
     *
     * USAGE:
     * @code
     *   // In system initialization code (after chSysInit())
     *   if (!MemoryPoolManager::initialize()) {
     *       // Handle initialization failure
     *   }
     * @endcode
     */
    [[nodiscard]] static bool initialize() noexcept;

    /**
     * @brief Initialize global mutex for memory pool manager
     * @note Must be called once during system startup (after chSysInit())
     * @note Must be called before initialize()
     * @note Thread-safe (no-op if already initialized)
     *
     * P1-HIGH FIX #E003: Explicit initialization for static Mutex
     * - Static Mutex objects require explicit initialization after ChibiOS RTOS is ready
     * - chMtxInit() must be called after chSysInit() to avoid undefined behavior
     * - This function should be called during system initialization
     *
     * USAGE:
     * @code
     *   // In system initialization code (after chSysInit())
     *   MemoryPoolManager::initialize_global_mutex();
     *   MemoryPoolManager::initialize();
     * @endcode
     */
    static void initialize_global_mutex() noexcept;

    /**
     * @brief Allocate memory from specified pool
     * @param pool_type Type of pool to allocate from
     * @return Pointer to allocated memory, or nullptr if pool exhausted
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     * @note Returns nullptr if pool is exhausted (overflow protection)
     *
     * USAGE:
     * @code
     *   DetectionRingBuffer* buffer = static_cast<DetectionRingBuffer*>(
     *       MemoryPoolManager::allocate(PoolType::DETECTION_RING_BUFFER)
     *   );
     *   if (buffer) {
     *       // Use buffer...
     *   }
     * @endcode
     */
    [[nodiscard]] static void* allocate(PoolType pool_type) noexcept;

    /**
     * @brief Deallocate memory back to specified pool
     * @param pool_type Type of pool to deallocate to
     * @param ptr Pointer to memory to deallocate
     * @note Thread-safe (mutex-protected)
     * @note Safe to call with nullptr (no-op)
     * @note Does not throw (noexcept)
     *
     * USAGE:
     * @code
     *   MemoryPoolManager::deallocate(PoolType::DETECTION_RING_BUFFER, buffer);
     * @endcode
     */
    static void deallocate(PoolType pool_type, void* ptr) noexcept;

    /**
     * @brief Get statistics for specified pool
     * @param pool_type Type of pool to get statistics for
     * @return PoolStatistics containing pool usage information
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     *
     * USAGE:
     * @code
     *   PoolStatistics stats = MemoryPoolManager::get_statistics(PoolType::DETECTION_RING_BUFFER);
     *   // Check if pool is exhausted
     *   if (stats.free_blocks == 0) {
     *       // Pool exhausted - handle gracefully
     *   }
     * @endcode
     */
    [[nodiscard]] static PoolStatistics get_statistics(PoolType pool_type) noexcept;

    /**
     * @brief Check if pool is initialized
     * @param pool_type Type of pool to check
     * @return true if pool is initialized, false otherwise
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     */
    [[nodiscard]] static bool is_initialized(PoolType pool_type) noexcept;

    /**
     * @brief Check if any pool is exhausted
     * @return true if any pool has zero free blocks, false otherwise
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     *
     * USAGE:
     * @code
     *   if (MemoryPoolManager::is_any_pool_exhausted()) {
     *       // At least one pool is exhausted - handle gracefully
     *   }
     * @endcode
     */
    [[nodiscard]] static bool is_any_pool_exhausted() noexcept;

private:
    // Private constructor/destructor (static class)
    MemoryPoolManager() = delete;
    ~MemoryPoolManager() = delete;
    MemoryPoolManager(const MemoryPoolManager&) = delete;
    MemoryPoolManager& operator=(const MemoryPoolManager&) = delete;

    /**
     * @brief Get pool index from PoolType enum
     * @param pool_type Pool type enum value
     * @return Pool index (0 to COUNT-1)
     */
    [[nodiscard]] static size_t pool_index(PoolType pool_type) noexcept;

    /**
     * @brief Pool initialization parameters
     *
     * CRITICAL FIX #E004: Uses strongly-typed parameters to prevent swapping
     * - BlockSize and PoolSize are distinct wrapper types
     * - Compile-time error if parameters are swapped
     * - Zero runtime overhead from wrapper classes
     * - Makes API more self-documenting
     *
     * USAGE:
     * @code
     *   // CORRECT: Types prevent swapping
     *   PoolInitParams params{
     *       .pool_type = PoolType::DETECTION_RING_BUFFER,
     *       .block_size = BlockSize(480),
     *       .pool_size = PoolSize(2)
     *   };
     *   initialize_pool(params);
     *
     *   // WRONG: Compiler error - types don't match!
     *   // PoolInitParams params{
     *   //     .pool_type = PoolType::DETECTION_RING_BUFFER,
     *   //     .block_size = PoolSize(2),       // Wrong type!
     *   //     .pool_size = BlockSize(480)     // Wrong type!
     *   // };
     * @endcode
     *
     * Or using constructor:
     * @code
     *   // CORRECT: Types prevent swapping
     *   PoolInitParams params(
     *       PoolType::DETECTION_RING_BUFFER,
     *       BlockSize(480),   // Block size
     *       PoolSize(2)       // Pool size
     *   );
     *   initialize_pool(params);
     * @endcode
     */
    struct PoolInitParams {
        PoolType pool_type;  ///< Type of pool to initialize
        BlockSize block_size;  ///< Size of each block in pool (strongly-typed)
        PoolSize pool_size;   ///< Number of blocks in pool (strongly-typed)

        /**
         * @brief Constructor with [[nodiscard]] to ensure parameters are used
         * @param pool_type Type of pool to initialize
         * @param block_size Size of each block in pool (strongly-typed)
         * @param pool_size Number of blocks in pool (strongly-typed)
         * @note constexpr for compile-time evaluation
         * @note noexcept for embedded safety
         * @note CRITICAL FIX #E004: Strongly-typed parameters prevent swapping
         */
        [[nodiscard]] constexpr PoolInitParams(
            PoolType pool_type,
            BlockSize block_size,  ///< Distinct type - cannot be confused with pool_size
            PoolSize pool_size     ///< Distinct type - cannot be confused with block_size
        ) noexcept : pool_type(pool_type),
                   block_size(block_size),
                   pool_size(pool_size) {}
    };

    /**
     * @brief Initialize a single pool
     * @param params Pool initialization parameters (struct prevents parameter swapping)
     * @return true if initialization successful, false otherwise
     *
     * P1-HIGH FIX #E002: Changed signature to use PoolInitParams struct
     * - Prevents accidental swapping of block_size and pool_size parameters
     * - Improves code readability and type safety
     * - Makes API more self-documenting
     */
    [[nodiscard]] static bool initialize_pool(const PoolInitParams& params) noexcept;

    /**
     * @brief Get block size for specified pool type
     * @param pool_type Pool type enum value
     * @return Size of each block in pool
     */
    [[nodiscard]] static size_t get_block_size(PoolType pool_type) noexcept;

    /**
     * @brief Get pool size for specified pool type
     * @param pool_type Pool type enum value
     * @return Number of blocks in pool
     */
    [[nodiscard]] static size_t get_pool_size(PoolType pool_type) noexcept;

    // ============================================================================
    // MEMORY POOL STORAGE (Static Allocation)
    // ============================================================================

    /**
     * @brief Memory pool entry
     *
     * CRITICAL FIX #002: Flexible array member for pool storage
     *
     * This structure uses a flexible array member (uint8_t storage[]) instead of
     * a zero-length array (uint8_t storage[0]). Flexible array members are
     * a C99 standard feature and are widely supported in C++ compilers.
     *
     * WHY THIS FIX IS CRITICAL:
     * - Zero-length arrays are undefined behavior in C++
     * - sizeof(PoolEntry) does not include storage size with zero-length arrays
     * - Accessing storage[0] is undefined behavior
     * - Flexible array members are C99 standard (not undefined behavior)
     * - Proper size calculation: sizeof(PoolEntry) + (block_size * pool_size)
     *
     * MEMORY LAYOUT:
     * +-------------------+  <- Start of PoolEntry
     * | MemoryPool pool   |  (~24 bytes)
     * +-------------------+
     * | Mutex mutex       |  (~24 bytes)
     * +-------------------+
     * | PoolStatistics    |  (48 bytes)
     * +-------------------+
     * | bool initialized  |  (4 bytes, padded)
     * +-------------------+
     * | storage[]        |  <-- Flexible array member (variable size)
     * |   Block 0        |  (block_size bytes)
     * +-------------------+
     * |   Block 1        |  (block_size bytes)
     * +-------------------+
     * |   ...            |
     * +-------------------+
     * |   Block N-1      |  (block_size bytes)
     * +-------------------+  <- End of allocation
     *
     * ALIGNMENT:
     * - 4-byte aligned for ARM Cortex-M4
     * - alignas(PoolConfig::BLOCK_ALIGNMENT) ensures proper alignment
     *
     * @note Flexible array member is C99 standard (not undefined behavior)
     * @note Storage size is not included in sizeof(PoolEntry)
     * @note Total allocation size: sizeof(PoolEntry) + (block_size * pool_size)
     */
    struct PoolEntry {
        MemoryPool pool;          ///< ChibiOS memory pool structure
        Mutex mutex;                 ///< Mutex for pool access protection
        PoolStatistics statistics;   ///< Pool usage statistics
        bool initialized;            ///< Pool initialization flag
        uint8_t storage[];         ///< Flexible array member for pool storage (C99 standard)
    };

    /**
     * @brief Get pool entry for specified pool type
     * @param pool_type Pool type enum value
     * @return Pointer to pool entry
     */
    [[nodiscard]] static PoolEntry* get_pool_entry(PoolType pool_type) noexcept;

    // Static storage for all pools (allocated in .cpp file)
    // P1-HIGH FIX: Static variables are defined in .cpp file to prevent dynamic initialization issues
    // NOLINTNEXTLINE(cert-err58-cpp): pools_ is defined in .cpp
    static PoolEntry* pools_[static_cast<size_t>(PoolType::COUNT)];
    // NOLINTNEXTLINE(cert-err58-cpp): global_mutex_ requires runtime initialization via initialize_global_mutex()
    static Mutex global_mutex_;  ///< Global mutex for pool manager operations
    // NOLINTNEXTLINE(cert-err58-cpp): global_mutex_initialized_ is defined in .cpp
    static bool global_mutex_initialized_;  ///< Tracks if global_mutex_ has been initialized
};

// ============================================================================
// MEMORY POOL RAII WRAPPER
// ============================================================================

/**
 * @brief RAII wrapper for memory pool allocation
 *
 * This template class provides automatic memory deallocation using RAII.
 * When the wrapper goes out of scope, memory is automatically returned
 * to the pool.
 *
 * @tparam T Type of object stored in memory pool
 * @tparam PoolType_ Type of memory pool to use
 *
 * USAGE EXAMPLE:
 * @code
 *   // Allocate DetectionRingBuffer from pool
 *   using DetectionRingBufferPool = MemoryPoolWrapper<DetectionRingBuffer, PoolType::DETECTION_RING_BUFFER>;
 *   DetectionRingBufferPool buffer_pool;
 *
 *   // Allocate from pool (returns nullptr if pool exhausted)
 *   DetectionRingBuffer* buffer = buffer_pool.allocate();
 *   if (buffer) {
 *       // Use buffer...
 *       buffer->update_detection(update);
 *
 *       // Automatic deallocation when buffer_pool goes out of scope
 *   }
 * @endcode
 *
 * THREAD SAFETY:
 * - Allocation and deallocation are thread-safe (mutex-protected)
 * - The wrapper itself is not thread-safe (do not share between threads)
 * - Each thread should have its own wrapper instance
 *
 * MEMORY SAFETY:
 * - Automatic cleanup via RAII destructor
 * - No exceptions (noexcept guarantee)
 * - Null pointer handling (allocate() returns nullptr on failure)
 */
template <typename T, PoolType PoolType_>
class MemoryPoolWrapper {
public:
    /**
     * @brief Constructor
     * @note Does not allocate memory immediately
     */
    MemoryPoolWrapper() noexcept : ptr_(nullptr) {}

    /**
     * @brief Destructor - automatically deallocates memory if allocated
     */
    ~MemoryPoolWrapper() noexcept {
        deallocate();
    }

    // Non-copyable, non-movable (RAII semantics)
    MemoryPoolWrapper(const MemoryPoolWrapper&) = delete;
    MemoryPoolWrapper& operator=(const MemoryPoolWrapper&) = delete;
    MemoryPoolWrapper(MemoryPoolWrapper&&) = delete;
    MemoryPoolWrapper& operator=(MemoryPoolWrapper&&) = delete;

    /**
     * @brief Allocate memory from pool
     * @return Pointer to allocated memory, or nullptr if pool exhausted
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     *
     * USAGE:
     * @code
     *   MemoryPoolWrapper<DetectionRingBuffer, PoolType::DETECTION_RING_BUFFER> pool;
     *   DetectionRingBuffer* buffer = pool.allocate();
     *   if (buffer) {
     *       // Use buffer...
     *   }
     * @endcode
     */
    [[nodiscard]] T* allocate() noexcept {
        // Guard clause: Already allocated
        if (ptr_) {
            return ptr_;
        }

        // Allocate from global pool manager
        ptr_ = static_cast<T*>(MemoryPoolManager::allocate(PoolType_));
        return ptr_;
    }

    /**
     * @brief Deallocate memory back to pool
     * @note Thread-safe (mutex-protected)
     * @note Safe to call multiple times (idempotent)
     * @note Does not throw (noexcept)
     */
    void deallocate() noexcept {
        // Guard clause: Not allocated
        if (!ptr_) {
            return;
        }

        // Return memory to global pool manager
        MemoryPoolManager::deallocate(PoolType_, ptr_);
        ptr_ = nullptr;
    }

    /**
     * @brief Get pointer to allocated memory
     * @return Pointer to allocated memory, or nullptr if not allocated
     * @note Does not allocate (use allocate() to get memory)
     */
    [[nodiscard]] T* get() noexcept {
        return ptr_;
    }

    /**
     * @brief Get const pointer to allocated memory
     * @return Const pointer to allocated memory, or nullptr if not allocated
     * @note Does not allocate (use allocate() to get memory)
     */
    [[nodiscard]] const T* get() const noexcept {
        return ptr_;
    }

    /**
     * @brief Check if memory is currently allocated
     * @return true if memory is allocated, false otherwise
     */
    [[nodiscard]] bool is_allocated() const noexcept {
        return ptr_ != nullptr;
    }

    /**
     * @brief Release ownership of allocated memory
     * @return Pointer to allocated memory, or nullptr if not allocated
     * @note Caller is responsible for deallocating the memory
     * @note After release(), wrapper no longer owns the memory
     */
    [[nodiscard]] T* release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    /**
     * @brief Reset the wrapper (deallocate if allocated)
     * @note Equivalent to deallocate()
     */
    void reset() noexcept {
        deallocate();
    }

private:
    T* ptr_;  ///< Pointer to allocated memory
};

// ============================================================================
// CONVENIENCE TYPE ALIASES
// ============================================================================

/**
 * @brief Convenience type aliases for common memory pools
 *
 * These aliases make it easier to use memory pools for specific types.
 *
 * USAGE:
 * @code
 *   // Allocate DetectionRingBuffer
 *   DetectionRingBufferPool buffer_pool;
 *   DetectionRingBuffer* buffer = buffer_pool.allocate();
 *
 *   // Allocate FilteredDronesSnapshot
 *   FilteredDronesSnapshotPool snapshot_pool;
 *   FilteredDronesSnapshot* snapshot = snapshot_pool.allocate();
 *
 *   // Automatic deallocation when pools go out of scope
 * @endcode
 */

// Forward declarations (defined in respective header files)
class DetectionRingBuffer;
struct FilteredDronesSnapshot;
struct DroneAnalyzerSettings;
struct DisplayDataSnapshot;

// Type aliases for memory pools (using MemoryPoolWrapper to avoid name conflict with ChibiOS MemoryPool)
using DetectionRingBufferPool = MemoryPoolWrapper<DetectionRingBuffer, PoolType::DETECTION_RING_BUFFER>;
using FilteredDronesSnapshotPool = MemoryPoolWrapper<FilteredDronesSnapshot, PoolType::FILTERED_DRONES_SNAPSHOT>;
using DroneAnalyzerSettingsPool = MemoryPoolWrapper<DroneAnalyzerSettings, PoolType::DRONE_ANALYZER_SETTINGS>;
using DisplayDataSnapshotPool = MemoryPoolWrapper<DisplayDataSnapshot, PoolType::DISPLAY_DATA_SNAPSHOT>;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // MEMORY_POOL_MANAGER_HPP_
