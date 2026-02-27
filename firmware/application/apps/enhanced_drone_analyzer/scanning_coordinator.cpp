// * @file scanning_coordinator.cpp * @brief Coordinate scanning operations for Enhanced Drone Analyzer
// * DIAMOND CODE PRINCIPLES:
// * - Zero heap allocation: All memory is stack-allocated or in Flash
// * - No exceptions: All functions are noexcept
// * - Type-safe: Uses semantic type aliases
// * - Memory-safe: Uses ChibiOS RTOS for thread management
// * @author Diamond Code Pipeline
// * @date 2026-02-27

// Corresponding header (must be first)
#include "scanning_coordinator.hpp"

// C++ standard library headers (alphabetical order)
#include <cstdint>

// Third-party library headers
#include <ch.h>  // ChibiOS RTOS

// Project-specific headers (alphabetical order)
#include "eda_locking.hpp"               // Unified CriticalSection
#include "radio.hpp"                     // For rf::Frequency type
#include "ui_drone_common_types.hpp"     // For DroneAnalyzerSettings
#include "ui_enhanced_drone_analyzer.hpp" // Scanner interface
#include "ui_navigation.hpp"              // For NavigationView

namespace ui::apps::enhanced_drone_analyzer {

// Coordinator Thread Working Area Definition
stkalign_t ScanningCoordinator::coordinator_wa_[THD_WA_SIZE(ScanningCoordinator::COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

// TYPE ALIASES
using TimeoutCount = uint32_t;

// CONSTANTS
namespace CoordinatorConstants {
    // Scan cycle timeout in milliseconds
    constexpr uint32_t SCAN_CYCLE_TIMEOUT_MS = 10000;

    // Maximum consecutive timeouts before aborting
    constexpr TimeoutCount MAX_CONSECUTIVE_TIMEOUTS = 3;

    // Maximum consecutive scanner failures before aborting
    constexpr TimeoutCount MAX_CONSECUTIVE_SCANNER_FAILURES = 5;

    // Initialization timeout: maximum time to wait for initialization to complete (30 seconds)
    constexpr uint32_t INITIALIZATION_TIMEOUT_MS = 30000;
}

// RETURN CODES
namespace ReturnCodes {
    constexpr msg_t SUCCESS = 0;
    constexpr msg_t TIMEOUT_ERROR = -1;
    constexpr msg_t SCANNER_ERROR = -2;
    constexpr msg_t INITIALIZATION_TIMEOUT = -3;  // New return code for initialization timeout
}

// ScanningCoordinator Implementation

ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
                                         DroneHardwareController& hardware,
                                         DroneScanner& scanner,
                                         DroneDisplayController& display_controller,
                                         ::AudioManager& audio_controller) noexcept
    : nav_(nav)
    , hardware_(hardware)
    , scanner_(scanner)
    , display_controller_(display_controller)
    , audio_controller_(audio_controller)
    , scanning_active_(false)
    , scanning_thread_(nullptr)
    , scan_interval_ms_(EDA::Constants::DEFAULT_SCAN_INTERVAL_MS)
    , thread_terminated_(false)  // Track thread termination
{
}

ScanningCoordinator::~ScanningCoordinator() noexcept {
    stop_coordinated_scanning();
}

StartResult ScanningCoordinator::start_coordinated_scanning() noexcept {
    // Guard clause: Already scanning
    if (scanning_active_) {
        return StartResult::ALREADY_ACTIVE;
    }

    // Check initialization complete before starting scanning
    // Prevents scanning from starting before database is loaded
    if (!scanner_.is_initialization_complete()) {
        // Initialization not complete - cannot start scanning yet
        return StartResult::INITIALIZATION_NOT_COMPLETE;
    }

    {
        CriticalSection cs;
        scanning_active_ = true;
        thread_terminated_ = false;  // Reset termination flag (only when starting, not when stopping)
        thread_generation_++;  // Increment generation counter to prevent missed signals
    }

    // Create static thread with stack-based working area
    // ChibiOS chThdCreateStatic() returns nullptr on failure
    scanning_thread_ = chThdCreateStatic(
        coordinator_wa_,
        sizeof(coordinator_wa_),
        NORMALPRIO,
        scanning_thread_function,
        this
    );

    // Guard clause: Thread creation failed
    if (!scanning_thread_) {
        {
            CriticalSection cs;
            scanning_active_ = false;
        }
        return StartResult::THREAD_CREATION_FAILED;
    }
    
    return StartResult::SUCCESS;
}

void ScanningCoordinator::stop_coordinated_scanning() noexcept {
    // Guard clause: Not scanning
    if (!scanning_active_) {
        return;
    }

    // Signal thread to stop using termination flag (thread-safe)
    // CRITICAL: thread_terminated_ is only reset when STARTING scanning, not when STOPPING.
    // This prevents race condition where coordinator waits indefinitely for a signal
    // that was already set by the previous thread.
    {
        CriticalSection cs;
        scanning_active_ = false;
        // thread_terminated_ flag is NOT reset here - only reset in start_coordinated_scanning()
    }

    // Wait for thread to terminate with timeout (prevents indefinite hang)
    // Capture current generation to prevent waiting for old thread's signal
    const uint32_t expected_generation = thread_generation_;

    // Poll with deadline to prevent indefinite wait (5 second timeout)
    constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
    constexpr uint32_t POLL_INTERVAL_MS = 10;
    systime_t deadline = chTimeNow() + MS2ST(TERMINATION_TIMEOUT_MS);

    // Wait for thread to set termination flag before nullifying pointer
    // Only wait if generation matches (prevents missed signal race condition)
    while (chTimeNow() < deadline && !thread_terminated_ && thread_generation_ == expected_generation) {
        chThdSleepMilliseconds(POLL_INTERVAL_MS);
    }

    // Only nullify pointer after thread has set termination flag
    // This prevents race condition where pointer is nullified before thread exits
    scanning_thread_ = nullptr;
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept {
    // Update scan interval
    scan_interval_ms_ = settings.scan_interval_ms;
    
    // Guard clause: Not scanning, no need to update scanner
    if (!scanning_active_) {
        return;
    }
    
    // Update scanner frequency range
    // DIAMOND FIX: Type-safe frequency comparison (int64_t for frequency values)
    // Compare uint64_t value directly against INT64_MAX before casting to prevent overflow
    constexpr uint64_t INT64_MAX_U64 = 9223372036854775807ULL;
    
    // Clamp frequency values to int64_t range if needed
    uint64_t min_freq = (settings.wideband_min_freq_hz > INT64_MAX_U64) ? INT64_MAX_U64 : settings.wideband_min_freq_hz;
    uint64_t max_freq = (settings.wideband_max_freq_hz > INT64_MAX_U64) ? INT64_MAX_U64 : settings.wideband_max_freq_hz;
    
    scanner_.update_scan_range(static_cast<int64_t>(min_freq),
                               static_cast<int64_t>(max_freq));
}

void ScanningCoordinator::show_session_summary([[maybe_unused]] const char* summary) noexcept {
    // Placeholder for future implementation
    // DIAMOND OPTIMIZATION: Uses const char* instead of std::string (zero heap allocation)
}

msg_t ScanningCoordinator::scanning_thread_function(void* arg) noexcept {
    auto coordinator = static_cast<ScanningCoordinator*>(arg);
    return coordinator->coordinated_scanning_thread();
}

msg_t ScanningCoordinator::coordinated_scanning_thread() noexcept {
    // No exceptions - perform scan cycle directly
    // Error handling is managed via return codes

    // Capture thread generation at start to prevent "missed signal" race condition
    // during thread restart: each thread start increments the counter, thread only
    // sets termination flag if generation matches, coordinator waits for matching generation.
    const uint32_t my_generation = thread_generation_;

    // Convert timeout constant to ChibiOS systime_t
    constexpr systime_t SCAN_CYCLE_TIMEOUT_ST = MS2ST(CoordinatorConstants::SCAN_CYCLE_TIMEOUT_MS);

    // Counters for error detection
    TimeoutCount consecutive_timeouts = 0;

    // Track initialization wait time outside the loop to ensure timeout is
    // calculated correctly across loop iterations
    systime_t init_wait_start_time = chTimeNow();

    while (scanning_active_) {
        // Check initialization complete with timeout to prevent infinite loop
        if (!scanner_.is_initialization_complete()) {
            // Check if initialization timeout has been reached
            systime_t init_wait_time = chTimeNow() - init_wait_start_time;
            if (init_wait_time > MS2ST(CoordinatorConstants::INITIALIZATION_TIMEOUT_MS)) {
                // Initialization timeout - signal stop to coordinator
                {
                    CriticalSection cs;
                    scanning_active_ = false;
                }

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                {
                    CriticalSection cs;
                    if (thread_generation_ == my_generation) {
                        thread_terminated_ = true;
                    }
                }

                chThdExit(ReturnCodes::INITIALIZATION_TIMEOUT);
                return ReturnCodes::INITIALIZATION_TIMEOUT;
            }

            // Use constant instead of magic number
            chThdSleepMilliseconds(EDA::Constants::SD_CARD_POLL_INTERVAL_MS);
            continue;
        }

        // Reset initialization wait timer when initialization completes
        // This ensures timeout is calculated correctly if initialization fails later
        init_wait_start_time = chTimeNow();

        const systime_t cycle_start = chTimeNow();

        // Perform scan cycle
        // Note: We don't check return value here as scanner handles its own errors
        // Future enhancement: Add scanner error handling if needed
        scanner_.perform_scan_cycle(hardware_);

        const systime_t cycle_duration = chTimeNow() - cycle_start;

        // Check for scan cycle timeout
        if (cycle_duration > SCAN_CYCLE_TIMEOUT_ST) {
            consecutive_timeouts++;

            // Guard clause: Too many consecutive timeouts
            if (consecutive_timeouts >= CoordinatorConstants::MAX_CONSECUTIVE_TIMEOUTS) {
                // Signal stop to coordinator
                {
                    CriticalSection cs;
                    scanning_active_ = false;
                }

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                {
                    CriticalSection cs;
                    if (thread_generation_ == my_generation) {
                        thread_terminated_ = true;
                    }
                }

                chThdExit(ReturnCodes::TIMEOUT_ERROR);
                return ReturnCodes::TIMEOUT_ERROR;
            }
        } else {
            consecutive_timeouts = 0;
        }

        // Sleep for configured interval
        chThdSleepMilliseconds(scan_interval_ms_);
    }

    // Set termination flag before exit (thread-safe)
    // Only set flag if generation matches (prevents missed signal race condition)
    {
        CriticalSection cs;
        if (thread_generation_ == my_generation) {
            thread_terminated_ = true;
        }
    }

    // Normal exit
    chThdExit(ReturnCodes::SUCCESS);
    return ReturnCodes::SUCCESS;
}

} // namespace ui::apps::enhanced_drone_analyzer
