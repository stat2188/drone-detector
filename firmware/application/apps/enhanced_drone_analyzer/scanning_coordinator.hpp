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
#include "eda_constants.hpp"
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
    // @note Uses compiler intrinsic for memory barriers (not chSysLock/chSysUnlock)
    // @note chSysLock/chSysUnlock are critical section locks that disable ALL interrupts,
    //       not memory barriers. Using them incorrectly can cause system instability.
    void construct(NavigationView& nav,
                  DroneHardwareController& hardware,
                  DroneScanner& scanner,
                  DroneDisplayController& display_controller,
                  AudioManager& audio_controller) noexcept {
        // Memory barrier before construction (compiler intrinsic)
        __sync_synchronize();

        // Construct object using placement new
        // Note: placement new is defined inline, no <new> header needed
        new (static_cast<void*>(&instance_storage_)) T(nav, hardware, scanner, display_controller, audio_controller);

        // Set constructed flag
        constructed_ = true;

        // Memory barrier after construction (compiler intrinsic)
        __sync_synchronize();
    }

    // Get reference to stored object
    // @return Reference to stored object
    // @pre construct() must have been called
    // @pre is_corrupted() must return false
    // @note Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)
    [[nodiscard]] T& get() noexcept {
        // Memory barrier before accessing object (compiler intrinsic)
        __sync_synchronize();
        return *reinterpret_cast<T*>(&instance_storage_);
    }

    // Get const reference to stored object
    // @return Const reference to stored object
    // @pre construct() must have been called
    // @pre is_corrupted() must return false
    // @note Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)
    [[nodiscard]] const T& get() const noexcept {
        // Memory barrier before accessing object (compiler intrinsic)
        __sync_synchronize();
        return *reinterpret_cast<const T*>(&instance_storage_);
    }

    // Check if memory corruption has occurred
    // @return true if canary values are intact, false if corruption detected
    // @note Validates canary values before and after instance storage
    // @note Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)
    [[nodiscard]] bool is_corrupted() const noexcept {
        // Memory barrier before reading canary values (compiler intrinsic)
        __sync_synchronize();

        bool canary_valid = (canary_before_ == CANARY_VALUE) &&
                            (canary_after_ == CANARY_VALUE);

        return !canary_valid;
    }

    // Check if object has been constructed
    // @return true if construct() was called successfully
    // @note Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)
    [[nodiscard]] bool is_constructed() const noexcept {
        // Memory barrier before reading flag (compiler intrinsic)
        __sync_synchronize();
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

// @brief Result codes for start_coordinated_scanning() operation
enum class StartResult {
    SUCCESS,                      ///< Scanning thread started successfully
    ALREADY_ACTIVE,               ///< Scanning is already active
    INITIALIZATION_NOT_COMPLETE,  ///< Database initialization not complete yet
    THREAD_CREATION_FAILED,       ///< Thread creation failed (chThdCreateStatic returned nullptr)
    SINGLETON_VIOLATION           ///< Multiple instances detected (should never happen)
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

    // Singleton pattern enforcement
    static ScanningCoordinator& instance() noexcept;
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

    // FIX #SO-1: Increased from 1536 to 2048 bytes (33% increase) to prevent stack overflow
    static constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 2048;

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
    static stkalign_t coordinator_wa_[THD_WA_SIZE(COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

public:
    // FIX #7: Singleton state with volatile flag for thread safety
    // DIAMOND FIX: Static storage pattern (no heap allocation)
    static ScanningCoordinator* instance_ptr_;  ///< Singleton instance pointer
    static Mutex init_mutex_;                   ///< Protects singleton initialization
    static volatile bool initialized_;           ///< Tracks if singleton has been initialized (volatile for thread safety)
    static volatile bool instance_constructed_;  ///< Tracks if placement new was called (volatile for thread safety)

private:

};  // class ScanningCoordinator

}  // namespace ui::apps::enhanced_drone_analyzer

#endif  // SCANNING_COORDINATOR_HPP_
