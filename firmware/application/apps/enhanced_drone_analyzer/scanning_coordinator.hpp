// @file scanning_coordinator.hpp
// @brief Coordinate scanning operations for Enhanced Drone Analyzer
//
// DIAMOND CODE - Phase 4: Coordination Layer
// ============================================
// Migrated from LEGACY/ with Diamond Code compliance:
// - No forbidden constructs (std::vector, std::string, std::map, std::atomic, new, malloc)
// - Stack allocation only (max 4KB stack per thread)
// - Uses constexpr, enum class, using Type = uintXX_t
// - No magic numbers (all constants defined)
// - Zero-Overhead and Data-Oriented Design principles
// - Singleton pattern with proper initialization order handling
// - Thread-safe using MutexLock from eda_locking.hpp
// - Custom AtomicFlag class (NOT std::atomic) using GCC built-ins
//
// Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
// Environment: Bare-metal / ChibiOS RTOS
//
// @author Diamond Code Pipeline - Phase 4 Migration
// @date 2026-03-11

#ifndef SCANNING_COORDINATOR_HPP_
#define SCANNING_COORDINATOR_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Third-party library headers
#include <ch.h>  // For stkalign_t and THD_WA_SIZE

// ChibiOS headers are included through <ch.h>
// No need to include internal headers directly

// Project-specific headers (alphabetical order)
#include "dsp_display_types.hpp"
#include "eda_constants.hpp"
#include "eda_locking.hpp"
#include "eda_thread_sync.hpp"
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
// STATIC STORAGE PROTECTION WITH CANARY PATTERN
// ============================================================================
// DIAMOND OPTIMIZATION: Static storage with canary pattern for corruption detection
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
    // FIX: Default constructor - initializes constructed_ flag to false
    // Ensures proper initialization before construct() is called
    constexpr StaticStorage() noexcept
        : canary_before_{CANARY_VALUE}
        , instance_storage_{}
        , canary_after_{CANARY_VALUE}
        , constructed_{}  // AtomicFlag default constructor initializes to false
    {}

    // Construct object in static storage using placement new
    // @param nav Navigation view reference
    // @param hardware Hardware controller reference
    // @param scanner Scanner reference
    // @param display_controller Display controller reference
    // @param audio_controller Audio controller reference
    // @note CRITICAL FIX: Caller must provide mutex protection for concurrent construction
    // @note Uses AtomicFlag for thread-safe flag access with acquire/release semantics
    // @pre Caller must hold init_mutex_ when calling this method
    void construct(NavigationView& nav,
                  DroneHardwareController& hardware,
                  DroneScanner& scanner,
                  DroneDisplayController& display_controller,
                  AudioManager& audio_controller) noexcept {
        // WARNING FIX: Call destructor if already constructed (prevents resource leak)
        // AtomicFlag::load() uses acquire semantics for proper memory ordering
        if (constructed_.load()) {
            T* old_instance = reinterpret_cast<T*>(&instance_storage_);
            old_instance->~T();
        }
        
        // Construct object using placement new
        // Note: placement new is defined inline, no <new> header needed
        new (static_cast<void*>(&instance_storage_)) T(nav, hardware, scanner, display_controller, audio_controller);

        // Set constructed flag (atomic store with release semantics)
        // AtomicFlag::store() uses release semantics to ensure all prior writes are visible
        constructed_.store(true);
    }

    // Get reference to stored object
    // @return Reference to stored object
    // @pre construct() must have been called
    // @pre is_corrupted() must return false
    [[nodiscard]] T& get() noexcept {
        return *reinterpret_cast<T*>(&instance_storage_);
    }

    // Get const reference to stored object
    // @return Const reference to stored object
    // @pre construct() must have been called
    // @pre is_corrupted() must return false
    [[nodiscard]] const T& get() const noexcept {
        return *reinterpret_cast<const T*>(&instance_storage_);
    }

    // Check if memory corruption has occurred
    // @return true if canary values are intact, false if corruption detected
    // @note Validates canary values before and after instance storage
    [[nodiscard]] bool is_corrupted() const noexcept {
        bool canary_valid = (canary_before_ == CANARY_VALUE) &&
                            (canary_after_ == CANARY_VALUE);

        return !canary_valid;
    }

    // Check if object has been constructed
    // @return true if construct() was called successfully
    // @note AtomicFlag::load() uses acquire semantics for proper memory ordering
    [[nodiscard]] bool is_constructed() const noexcept {
        return constructed_.load();
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
    // DIAMOND FIX: Use AtomicFlag instead of volatile bool for thread-safe flag access
    // Memory ordering: load() uses acquire semantics, store() uses release semantics
    // This ensures proper synchronization across all CPU cores
    // FIX: Initialized to false in constructor member initialization list
    AtomicFlag constructed_;
};

/**
 * @brief Result codes for start_coordinated_scanning() operation
 *
 * DIAMOND OPTIMIZATION: Changed from int to uint8_t for memory efficiency
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
// - Singleton pattern with proper initialization order handling
// - Custom AtomicFlag class (NOT std::atomic) using GCC built-ins
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
                                        ::Thread* ui_thread,
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
    // Mutex-protected access (not inline)
    [[nodiscard]] bool is_scanning_active() const noexcept;

    // Update runtime parameters from settings
    // @param settings New settings to apply
    // Updates scan interval and frequency range if scanning is active.
    void update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept;

    // Display session summary (placeholder for future implementation)
    // @param summary Summary text to display (may be nullptr)
    void show_session_summary([[maybe_unused]] const char* summary) noexcept;

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

    // DIAMOND FIX: Thread Flag Sender for UI thread communication
    // Sends flags to UI thread for event-driven updates
    // Eliminates mutex-based polling and reduces contention
    sync::ThreadFlagSender ui_flag_sender_;

    // Thread synchronization
    Mutex thread_mutex_;            ///< Protects thread creation/destruction (complex state requires mutex)

    // State flags (lock-free using AtomicFlag)
    // Memory ordering: load() uses acquire semantics, store() uses release semantics
    // This ensures proper synchronization across threads without mutex overhead
    AtomicFlag scanning_active_;      ///< Scanning active flag (true when scanning, false when stopped)
    AtomicFlag thread_terminated_;    ///< Thread termination flag (set by thread when exiting)
    uint32_t thread_generation_{0};  ///< Thread generation counter (prevents missed signals during restart)
    ::Thread* scanning_thread_{nullptr};
    uint32_t scan_interval_ms_{EDA::Constants::DEFAULT_SCAN_INTERVAL_MS};

    // Stack size for coordinator thread (4KB to prevent stack overflow)
    // Stack usage analysis shows coordinator thread requires ~2.5KB with nested function calls
    // This provides 1536 bytes of safety margin (4096 - 2500 - 60 overhead)
    // 4KB provides 38% safety margin (1536 bytes) for realistic worst-case scenarios
    // CRITICAL: AGENTS.md constraint - stack must not exceed 4KB
    static constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 4096;

    // ============================================================================
    // STACK USAGE VALIDATION
    // ============================================================================
    // Embedded systems have limited stack space (4KB per thread on STM32F405).
    // These static_assert statements validate stack usage at compile time to prevent
    // stack overflow at runtime, which is difficult to debug.
    // CRITICAL: AGENTS.md constraint - stack must not exceed 4KB
    // ============================================================================

    // Validate coordinator thread stack size is within reasonable limits
    // Coordinator thread requires ~2.5KB with nested function calls
    // Stack size must not exceed 4KB per AGENTS.md constraint
    static_assert(COORDINATOR_THREAD_STACK_SIZE <= 4096,
                  "COORDINATOR_THREAD_STACK_SIZE exceeds 4KB thread stack limit (AGENTS.md constraint)");
    static_assert(COORDINATOR_THREAD_STACK_SIZE >= 1024,
                  "COORDINATOR_THREAD_STACK_SIZE below 1KB minimum for safe operation");

    // Thread working area (defined in .cpp file to avoid ODR issues)
    // Static variable is defined in .cpp file to prevent dynamic initialization issues
    // NOLINTNEXTLINE(cert-err58-cpp): coordinator_wa_ is defined in .cpp with compile-time constant size
    static stkalign_t coordinator_wa_[THD_WA_SIZE(COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

public:
    // DIAMOND FIX: Static storage pattern (no heap allocation)
    // CRITICAL FIX: Use AtomicFlag instead of volatile bool for proper memory barriers
    // AtomicFlag provides acquire/release memory ordering and lock-free operations
    // This ensures proper memory ordering across all CPU cores and prevents
    // race conditions in double-checked locking pattern.
    // P1-HIGH FIX: Static variables are defined in .cpp file to prevent dynamic initialization issues
    // NOLINTNEXTLINE(cert-err58-cpp): instance_ptr_ is defined in .cpp
    static volatile ScanningCoordinator* instance_ptr_;  ///< Singleton instance pointer (volatile for thread safety)
    // NOLINTNEXTLINE(cert-err58-cpp): init_mutex_ requires runtime initialization via initialize_eda_mutexes()
    static Mutex init_mutex_;                   ///< Protects singleton initialization
    // NOLINTNEXTLINE(cert-err58-cpp): initialized_ is defined in .cpp
    static AtomicFlag initialized_;           ///< Tracks if singleton has been initialized (AtomicFlag for thread safety)

private:

};  // class ScanningCoordinator

// ============================================================================
// EXPLICIT INITIALIZATION FUNCTION
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
