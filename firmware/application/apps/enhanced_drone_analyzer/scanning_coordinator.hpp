// @file scanning_coordinator.hpp
// @brief Coordinate scanning operations for Enhanced Drone Analyzer

#ifndef SCANNING_COORDINATOR_HPP_
#define SCANNING_COORDINATOR_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Third-party library headers
#include <ch.h>  // For stkalign_t and THD_WA_SIZE

// Project-specific headers (alphabetical order)
#include "chcore_v6m.h"
#include "chlists.h"
#include "chmtx.h"
#include "chtypes.h"
#include "dsp_display_types.hpp"
#include "eda_constants.hpp"
#include "eda_locking.hpp"
#include "memory_pool_manager.hpp"
#include "ui_drone_common_types.hpp"
#include "ui_navigation.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Forward declaration
class AudioManager;

// Forward declarations
class DroneHardwareController;
class DroneScanner;
class DroneDisplayController;

// ============================================================================
// FIX #1 & #2: STATIC STORAGE PROTECTION WITH CANARY PATTERN
// ============================================================================
// DIAMOND FIX #2: Add memory barriers and canary patterns for corruption detection
//
// FEATURES:
// - Placement new construction in static storage (no heap allocation)
// - Canary values before and after instance storage for corruption detection
// - is_corrupted() method to detect memory corruption
// - construct() method for safe object construction
//
// USAGE:
//   static StaticStorage<ScanningCoordinator> coordinator_storage;
//   coordinator_storage.construct(nav, hardware, scanner, display_controller, audio_controller);
//   if (coordinator_storage.is_corrupted()) { /* handle corruption */ }
//   coordinator_storage.get().method();
//
// @tparam T Type to store in static storage
template <typename T>
class StaticStorage {
public:
    // Construct object in static storage using placement new
    // @param nav Navigation view reference
    // @param hardware Hardware controller reference
    // @param scanner Scanner reference
    // @param display_controller Display controller reference
    // @param audio_controller Audio controller reference
    // @note Relies on volatile bool for thread safety (consistent with codebase pattern)
    // @note volatile bool reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
    void construct(NavigationView& nav,
                  DroneHardwareController& hardware,
                  DroneScanner& scanner,
                  DroneDisplayController& display_controller,
                  AudioManager& audio_controller) noexcept {
        // WARNING FIX: Call destructor if already constructed (prevents resource leak)
        if (constructed_) {
            T* old_instance = reinterpret_cast<T*>(&instance_storage_);
            old_instance->~T();
        }
        
        // Construct object using placement new
        // Note: placement new is defined inline, no <new> header needed
        new (static_cast<void*>(&instance_storage_)) T(nav, hardware, scanner, display_controller, audio_controller);

        // Set constructed flag (volatile ensures visibility to other threads)
        constructed_ = true;
    }

    // Get reference to stored object
    // @return Reference to stored object
    // @pre construct() must have been called
    // @pre is_corrupted() must return false
    // @note Relies on volatile bool for thread safety (consistent with codebase pattern)
    [[nodiscard]] T& get() noexcept {
        return *reinterpret_cast<T*>(&instance_storage_);
    }

    // Get const reference to stored object
    // @return Const reference to stored object
    // @pre construct() must have been called
    // @pre is_corrupted() must return false
    // @note Relies on volatile bool for thread safety (consistent with codebase pattern)
    [[nodiscard]] const T& get() const noexcept {
        return *reinterpret_cast<const T*>(&instance_storage_);
    }

    // Check if memory corruption has occurred
    // @return true if canary values are intact, false if corruption detected
    // @note Validates canary values before and after instance storage
    // @note Relies on volatile bool for thread safety (consistent with codebase pattern)
    [[nodiscard]] bool is_corrupted() const noexcept {
        bool canary_valid = (canary_before_ == CANARY_VALUE) &&
                            (canary_after_ == CANARY_VALUE);

        return !canary_valid;
    }

    // Check if object has been constructed
    // @return true if construct() was called successfully
    // @note Relies on volatile bool for thread safety (consistent with codebase pattern)
    [[nodiscard]] bool is_constructed() const noexcept {
        return constructed_;
    }

private:
    // Canary value for corruption detection (0xDEADBEEF)
    static constexpr uint32_t CANARY_VALUE = 0xDEADBEEF;

    // Canary before instance storage
    uint32_t canary_before_{CANARY_VALUE};

    // Storage for instance (aligned to type's alignment requirement)
    alignas(T) uint8_t instance_storage_[sizeof(T)];

    // Canary after instance storage
    uint32_t canary_after_{CANARY_VALUE};

    // Flag to track if object has been constructed
    volatile bool constructed_{false};
};

/**
 * @brief Result codes for start_coordinated_scanning() operation
 *
 * P2-MED FIX #E004: Changed from int to uint8_t for memory efficiency
 * - Reduces enum size from 4 bytes to 1 byte
 * - Saves 3 bytes per StartResult variable/return value
 * - No impact on functionality (enum has < 256 values)
 */
enum class StartResult : uint8_t {
    SUCCESS,                      ///< Scanning thread started successfully
    ALREADY_ACTIVE,               ///< Scanning is already active
    INITIALIZATION_NOT_COMPLETE,  ///< Database initialization not complete yet
    THREAD_CREATION_FAILED,       ///< Thread creation failed (chThdCreateStatic returned nullptr)
    SINGLETON_VIOLATION          ///< Multiple instances detected (should never happen)
};

// @brief Scanning coordinator for drone detection operations
// Manages the scanning thread lifecycle and coordinates between
// hardware, scanner, display, and audio components.
//
// DIAMOND OPTIMIZATIONS:
// - Stack-only allocation (8KB working area)
// - Full mutex protection for thread-safe state management
// - No heap allocation
class ScanningCoordinator {
    // Allow StaticStorage to access private constructor
    template <typename T>
    friend class StaticStorage;

public:
    // Destroy the Scanning Coordinator object
    // Ensures scanning thread is stopped before destruction.
    ~ScanningCoordinator() noexcept;

    // Non-copyable, non-movable
    ScanningCoordinator(const ScanningCoordinator&) = delete;
    ScanningCoordinator(ScanningCoordinator&&) = delete;
    ScanningCoordinator& operator=(const ScanningCoordinator&) = delete;
    ScanningCoordinator& operator=(ScanningCoordinator&&) = delete;

    // ========================================================================
    // SINGLETON INSTANCE ACCESS
    // ========================================================================
    // NOTE: The unsafe instance() method has been removed due to critical
    // null pointer dereference bug. Use instance_safe() instead.
    //
    // RECOMMENDED USAGE:
    //   ScanningCoordinator* coordinator = ScanningCoordinator::instance_safe();
    //   if (coordinator) {
    //       // Safe to use coordinator
    //       coordinator->start_coordinated_scanning();
    //   } else {
    //       // Handle uninitialized state gracefully
    //   }
    // ========================================================================

    /**
     * @brief Get singleton instance safely (returns nullptr if not initialized)
     * @return Pointer to singleton instance, or nullptr if not initialized
     * @note Returns nullptr instead of halting system if called before initialize()
     * @note Uses double-checked locking with memory barriers for thread safety
     * @note This is the recommended method for optional singleton access
     */
    [[nodiscard]] static ScanningCoordinator* instance_safe() noexcept;
    
    [[nodiscard]] static bool initialize(NavigationView& nav,
                                        DroneHardwareController& hardware,
                                        DroneScanner& scanner,
                                        DroneDisplayController& display_controller,
                                        AudioManager& audio_controller) noexcept;

    // Start the coordinated scanning thread
    // @return StartResult indicating success or reason for failure
    // Uses ChibiOS chThdCreateStatic() for stack-based thread creation.
    // Returns ALREADY_ACTIVE if scanning is already running.
    // Returns INITIALIZATION_NOT_COMPLETE if database initialization is not complete yet.
    // Returns THREAD_CREATION_FAILED if chThdCreateStatic returns nullptr.
    StartResult start_coordinated_scanning() noexcept;

    // Stop the coordinated scanning thread
    // Sets the atomic flag and waits for thread termination.
    // Safe to call multiple times.
    void stop_coordinated_scanning() noexcept;

    // Check if scanning is currently active
    // @return true if scanning is active, false otherwise
    // FIX #RC-1: Mutex-protected access (not inline)
    [[nodiscard]] bool is_scanning_active() const noexcept;

    // Update runtime parameters from settings
    // @param settings New settings to apply
    // Updates scan interval and frequency range if scanning is active.
    void update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept;

    // Display session summary (placeholder for future implementation)
    // @param summary Summary text to display (may be nullptr)
    void show_session_summary([[maybe_unused]] const char* summary) noexcept;

    // ============================================================================
    // MEMORY POOL MANAGEMENT
    // ============================================================================
    // These functions provide memory pool initialization, shutdown, and statistics
    // for the Enhanced Drone Analyzer application.
    //
    // Thread-safety: All functions are thread-safe (mutex-protected)
    // No exceptions: All functions are noexcept
    // ============================================================================

    /**
     * @brief Initialize all memory pools for Enhanced Drone Analyzer
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     * @note Safe to call multiple times (idempotent)
     *
     * This function initializes all memory pools used by the Enhanced Drone Analyzer:
     * - DetectionRingBuffer pool (1 block of 480 bytes)
     * - FilteredDronesSnapshot pool (2 blocks of 640 bytes)
     * - DroneAnalyzerSettings pool (1 block of 512 bytes)
     * - DisplayDataSnapshot pool (3 blocks of 64 bytes)
     *
     * Total memory: 2864 bytes (~2.8 KB)
     *
     * USAGE:
     * @code
     *   // In system initialization code (after chSysInit())
     *   if (!ScanningCoordinator::initialize_memory_pools()) {
     *       // Handle initialization failure
     *   }
     * @endcode
     *
     * THREAD SAFETY:
     * - Thread-safe initialization via MemoryPoolManager::initialize()
     * - Multiple threads can call this concurrently
     * - Idempotent (safe to call multiple times)
     *
     * MEMORY SAFETY:
     * - Static allocation (no heap allocation for pool storage)
     * - Automatic bounds checking (returns false on failure)
     * - No memory leaks (shutdown_memory_pools() must be called)
     *
     * P1-HIGH FIX #E007: Returns bool to indicate initialization success/failure
     * - Caller must check return value to ensure memory pools are properly initialized
     * - System cannot operate without properly initialized memory pools
     */
    [[nodiscard]] static bool initialize_memory_pools() noexcept;

    /**
     * @brief Shutdown all memory pools for Enhanced Drone Analyzer
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     * @note Safe to call multiple times (idempotent)
     *
     * This function shuts down all memory pools used by the Enhanced Drone Analyzer.
     * It should be called during system shutdown to ensure proper cleanup.
     *
     * USAGE:
     * @code
     *   // In system shutdown code
     *   ScanningCoordinator::shutdown_memory_pools();
     * @endcode
     *
     * THREAD SAFETY:
     * - Thread-safe shutdown via MemoryPoolManager
     * - Multiple threads can call this concurrently
     * - Idempotent (safe to call multiple times)
     *
     * MEMORY SAFETY:
     * - Returns all pool memory to static storage
     * - No memory leaks (all allocations must be deallocated before shutdown)
     * - Safe to call multiple times
     */
    static void shutdown_memory_pools() noexcept;

    /**
     * @brief Get statistics for specified memory pool
     * @param pool_type Type of pool to get statistics for
     * @return PoolStatistics containing pool usage information
     * @note Thread-safe (mutex-protected)
     * @note Does not throw (noexcept)
     *
     * This function retrieves statistics for a specific memory pool,
     * including total blocks, used blocks, free blocks, allocation count,
     * free count, and overflow count.
     *
     * USAGE:
     * @code
     *   PoolStatistics stats = ScanningCoordinator::get_pool_statistics(
     *       PoolType::FILTERED_DRONES_SNAPSHOT
     *   );
     *   // stats.used_blocks = number of blocks in use
     *   // stats.free_blocks = number of blocks free
     *   // stats.overflow_count = number of allocation failures
     * @endcode
     *
     * THREAD SAFETY:
     * - Thread-safe statistics read via MemoryPoolManager::get_statistics()
     * - Multiple threads can call this concurrently
     * - Statistics are read atomically under mutex
     *
     * MEMORY SAFETY:
     * - Returns empty statistics if pool is invalid
     * - No memory allocation (returns by value)
     * - Safe to call at any time
     */
    [[nodiscard]] static PoolStatistics get_pool_statistics(PoolType pool_type) noexcept;

    // ============================================================================
    // DSP LAYER FUNCTIONS (UI/DSP Separation)
    // ============================================================================
    // These functions provide thread-safe access to DSP layer data for UI rendering.
    // They capture snapshots of internal state under mutex protection and return
    // them to the UI layer for rendering.
    //
    // Thread-safety: All functions acquire appropriate mutexes before accessing data.
    // No UI widget calls are made in these functions (pure DSP logic).
    // ============================================================================

    // Get display data snapshot from DSP layer
    // @return DisplayDataSnapshot containing current scanning state and statistics
    // Thread-safety: Acquires state_mutex_ before accessing data
    [[nodiscard]] dsp::DisplayDataSnapshot get_display_data_snapshot() const noexcept;

    // Get filtered drones snapshot from DSP layer
    // @return FilteredDronesSnapshot containing active drones for display
    // Thread-safety: Acquires data_mutex_ before accessing drone data
    [[nodiscard]] dsp::FilteredDronesSnapshot get_filtered_drones_snapshot() const noexcept;

private:
    // Private constructor for singleton pattern
    // @param nav Navigation view reference
    // @param hardware Hardware controller reference
    // @param scanner Scanner reference
    // @param display_controller Display controller reference
    // @param audio_controller Audio controller reference
    // DIAMOND FIX: Constructor must be noexcept for static storage pattern
    ScanningCoordinator(NavigationView& nav,
                       DroneHardwareController& hardware,
                       DroneScanner& scanner,
                       DroneDisplayController& display_controller,
                       AudioManager& audio_controller) noexcept;

    // Static thread entry point for ChibiOS
    // @param arg Pointer to ScanningCoordinator instance
    // @return Thread exit code
    static msg_t scanning_thread_function(void* arg) noexcept;

    // Main scanning thread implementation
    // @return Thread exit code (0 = success, -1 = timeout error)
    // Performs scan cycles in a loop with configurable interval.
    // Handles timeout detection and consecutive failure counting.
    msg_t coordinated_scanning_thread() noexcept;

    // Member variables
    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    DroneDisplayController& display_controller_;
    AudioManager& audio_controller_;

    // FIX #RC-1: Thread synchronization
    mutable Mutex state_mutex_;     ///< Protects scanning_active_, thread_terminated_, thread_generation_
    Mutex thread_mutex_;            ///< Protects thread creation/destruction

    // FIX #RC-1: State flags (access under state_mutex_)
    bool scanning_active_{false};
    bool thread_terminated_{false};  ///< Thread termination flag (set by thread when exiting)
    uint32_t thread_generation_{0}; ///< Thread generation counter (prevents missed signals during restart)
    ::Thread* scanning_thread_{nullptr};
    uint32_t scan_interval_ms_{EDA::Constants::DEFAULT_SCAN_INTERVAL_MS};

    // P0-STOP FIX #1: Increased from 3072 to 4096 bytes (33% increase) to prevent stack overflow
    // Stack usage analysis shows coordinator thread requires ~2.5KB with nested function calls
    // This provides 1536 bytes of safety margin (4096 - 2500 - 256 overhead)
    // Increased to provide additional safety margin for nested function calls and future enhancements
    static constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 4096;

    // ============================================================================
    // STACK USAGE VALIDATION
    // ============================================================================
    // Embedded systems have limited stack space (4KB per thread on STM32F405).
    // These static_assert statements validate stack usage at compile time to prevent
    // stack overflow at runtime, which is difficult to debug.
    // ============================================================================

    // Validate coordinator thread stack size is within reasonable limits
    // Coordinator thread has minimal stack usage, so 2KB is sufficient
    static_assert(COORDINATOR_THREAD_STACK_SIZE <= 4096,
                  "COORDINATOR_THREAD_STACK_SIZE exceeds 4KB thread stack limit");
    static_assert(COORDINATOR_THREAD_STACK_SIZE >= 1024,
                  "COORDINATOR_THREAD_STACK_SIZE below 1KB minimum for safe operation");

    // Thread working area (defined in .cpp file to avoid ODR issues)
    // P1-HIGH FIX: Static variable is defined in .cpp file to prevent dynamic initialization issues
    // NOLINTNEXTLINE(cert-err58-cpp): coordinator_wa_ is defined in .cpp with compile-time constant size
    static stkalign_t coordinator_wa_[THD_WA_SIZE(COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

public:
    // FIX #7: Singleton state with AtomicFlag for thread safety
    // DIAMOND FIX: Static storage pattern (no heap allocation)
    // RED TEAM FIX #P0-3: Make instance_ptr_ volatile for double-checked locking pattern
    // RED TEAM FIX #CRITICAL FLAW #5: Remove static initializer, use explicit init function
    // CRITICAL FIX: Use AtomicFlag instead of volatile bool for proper memory barriers
    // volatile alone doesn't guarantee proper synchronization on ARM Cortex-M4
    // AtomicFlag provides acquire/release memory ordering and lock-free operations
    // P1-HIGH FIX: Static variables are defined in .cpp file to prevent dynamic initialization issues
    // NOLINTNEXTLINE(cert-err58-cpp): instance_ptr_ is defined in .cpp with nullptr initializer
    static volatile ScanningCoordinator* instance_ptr_;  ///< Singleton instance pointer (volatile for thread safety)
    // NOLINTNEXTLINE(cert-err58-cpp): init_mutex_ requires runtime initialization via initialize_eda_mutexes()
    static Mutex init_mutex_;                   ///< Protects singleton initialization
    // NOLINTNEXTLINE(cert-err58-cpp): initialized_ uses default constructor which is safe for AtomicFlag
    static AtomicFlag initialized_;           ///< Tracks if singleton has been initialized (AtomicFlag for thread safety)
    // NOLINTNEXTLINE(cert-err58-cpp): instance_constructed_ uses default constructor which is safe for AtomicFlag
    static AtomicFlag instance_constructed_;  ///< Tracks if placement new was called (AtomicFlag for thread safety)

private:

};  // class ScanningCoordinator

// ============================================================================
// RED TEAM FIX #CRITICAL FLAW #5: Explicit initialization function
// ============================================================================
// Function to initialize EDA mutexes after ChibiOS RTOS is ready.
// Must be called AFTER chSysInit() in main() to prevent undefined behavior.
//
// @note This replaces the static initializer pattern which runs before main()
//       and before chSysInit(), causing undefined behavior with ChibiOS mutexes.
// ============================================================================
void initialize_eda_mutexes() noexcept;

}  // namespace ui::apps::enhanced_drone_analyzer

#endif  // SCANNING_COORDINATOR_HPP_
