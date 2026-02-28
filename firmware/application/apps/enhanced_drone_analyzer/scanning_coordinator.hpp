// * * @file scanning_coordinator.hpp * @brief Coordinate scanning operations for Enhanced Drone Analyzer

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

// * @brief Result codes for start_coordinated_scanning() operation
enum class StartResult {
    SUCCESS,                      ///< Scanning thread started successfully
    ALREADY_ACTIVE,               ///< Scanning is already active
    INITIALIZATION_NOT_COMPLETE,  ///< Database initialization not complete yet
    THREAD_CREATION_FAILED,       ///< Thread creation failed (chThdCreateStatic returned nullptr)
    SINGLETON_VIOLATION           ///< Multiple instances detected (should never happen)
};

// * * @brief Scanning coordinator for drone detection operations * * Manages the scanning thread lifecycle and coordinates between * hardware, scanner, display, and audio components. * * DIAMOND OPTIMIZATIONS: * - Stack-only allocation (8KB working area) * - Full mutex protection for thread-safe state management * - No heap allocation
class ScanningCoordinator {
public:
    // * * @brief Destroy the Scanning Coordinator object * * Ensures scanning thread is stopped before destruction.
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

    // * @brief Start the coordinated scanning thread
    // @return StartResult indicating success or reason for failure
    // * Uses ChibiOS chThdCreateStatic() for stack-based thread creation.
    // * Returns ALREADY_ACTIVE if scanning is already running.
    // * Returns INITIALIZATION_NOT_COMPLETE if database initialization is not complete yet.
    // * Returns THREAD_CREATION_FAILED if chThdCreateStatic returns nullptr.
    StartResult start_coordinated_scanning() noexcept;

    // * * @brief Stop the coordinated scanning thread * * Sets the atomic flag and waits for thread termination. * Safe to call multiple times.
    void stop_coordinated_scanning() noexcept;

    // * * @brief Check if scanning is currently active * @return true if scanning is active, false otherwise
    // FIX #RC-1: Mutex-protected access (not inline)
    [[nodiscard]] bool is_scanning_active() const noexcept;

    // * * @brief Update runtime parameters from settings * @param settings New settings to apply * * Updates scan interval and frequency range if scanning is active.
    void update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept;

    // * * @brief Display session summary (placeholder for future implementation) * @param summary Summary text to display (may be nullptr)
    void show_session_summary([[maybe_unused]] const char* summary) noexcept;

private:
    // * * @brief Private constructor for singleton pattern * @param nav Navigation view reference * @param hardware Hardware controller reference * @param scanner Scanner reference * @param display_controller Display controller reference * @param audio_controller Audio controller reference
    ScanningCoordinator(NavigationView& nav,
                       DroneHardwareController& hardware,
                       DroneScanner& scanner,
                       DroneDisplayController& display_controller,
                       AudioManager& audio_controller) noexcept;

    // * * @brief Static thread entry point for ChibiOS * @param arg Pointer to ScanningCoordinator instance * @return Thread exit code
    static msg_t scanning_thread_function(void* arg) noexcept;

    // * * @brief Main scanning thread implementation * @return Thread exit code (0 = success, -1 = timeout error) * * Performs scan cycles in a loop with configurable interval. * Handles timeout detection and consecutive failure counting.
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

    // Thread working area (defined in .cpp file to avoid ODR issues)
    static stkalign_t coordinator_wa_[THD_WA_SIZE(COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

public:
    // Singleton state
    static ScanningCoordinator* instance_ptr_;  ///< Singleton instance pointer
    static Mutex init_mutex_;                   ///< Protects singleton initialization
    static bool initialized_;                   ///< Tracks if singleton has been initialized

private:

};  // namespace ui::apps::enhanced_drone_analyzer

#endif  // SCANNING_COORDINATOR_HPP_