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
#include <cstring>
#include <new>  // For placement new

// Third-party library headers
#include <ch.h>         // ChibiOS RTOS
#include "chthreads.h"  // ChibiOS threading functions
#include "chmtx.h"      // ChibiOS mutex functions

// Project-specific headers (alphabetical order)
#include "eda_locking.hpp"               // Unified MutexLock
#include "radio.hpp"                     // For rf::Frequency type
#include "ui_drone_common_types.hpp"     // For DroneAnalyzerSettings
#include "ui_enhanced_drone_analyzer.hpp" // Scanner interface
#include "ui_navigation.hpp"              // For NavigationView

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// SINGLETON STORAGE DEFINITION
// ============================================================================

// Singleton instance storage (BSS segment, zero-initialized)
ScanningCoordinator* ScanningCoordinator::instance_ptr_ = nullptr;
Mutex ScanningCoordinator::init_mutex_;
bool ScanningCoordinator::initialized_ = false;

// Static initializer for init_mutex_ (called before main)
namespace {
    struct InitMutexInitializer {
        InitMutexInitializer() noexcept {
            chMtxInit(&ScanningCoordinator::init_mutex_);
        }
    } init_mutex_initializer;
}

// Coordinator Thread Working Area Definition
// FIX #SO-1: Increased from 1536 to 2048 bytes
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
    constexpr msg_t INITIALIZATION_TIMEOUT = -3;
}

// ============================================================================
// SINGLETON IMPLEMENTATION
// ============================================================================

/**
 * @brief Get singleton instance
 * @return Reference to singleton instance
 * @note Must call initialize() before using instance
 */
ScanningCoordinator& ScanningCoordinator::instance() noexcept {
    return *instance_ptr_;
}

/**
 * @brief Initialize singleton instance
 * @param nav Navigation view reference
 * @param hardware Hardware controller reference
 * @param scanner Scanner reference
 * @param display_controller Display controller reference
 * @param audio_controller Audio controller reference
 * @return true if initialization successful, false if already initialized
 * @note Can only be called once per program lifetime
 */
bool ScanningCoordinator::initialize(NavigationView& nav,
                                   DroneHardwareController& hardware,
                                   DroneScanner& scanner,
                                   DroneDisplayController& display_controller,
                                   ::AudioManager& audio_controller) noexcept {
    MutexLock lock(init_mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Already initialized
    if (initialized_) {
        return false;
    }

    // Allocate instance on heap (single allocation, never freed)
    // Note: This is acceptable for singleton pattern as instance lives for entire program lifetime
    instance_ptr_ = new ScanningCoordinator(nav, hardware, scanner, display_controller, audio_controller);

    initialized_ = true;
    return true;
}

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

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
    , state_mutex_{}
    , thread_mutex_{}
    , scanning_active_(false)
    , thread_terminated_(false)
    , thread_generation_(0)
    , scanning_thread_(nullptr)
    , scan_interval_ms_(EDA::Constants::DEFAULT_SCAN_INTERVAL_MS)
{
    chMtxInit(&state_mutex_);
    chMtxInit(&thread_mutex_);
}

ScanningCoordinator::~ScanningCoordinator() noexcept {
    stop_coordinated_scanning();
}

StartResult ScanningCoordinator::start_coordinated_scanning() noexcept {
    MutexLock thread_lock(thread_mutex_, LockOrder::DATA_MUTEX);
    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Already scanning
    if (scanning_active_) {
        return StartResult::ALREADY_ACTIVE;
    }

    // Check initialization complete before starting scanning
    // Prevents scanning from starting before database is loaded
    if (!scanner_.is_initialization_complete()) {
        return StartResult::INITIALIZATION_NOT_COMPLETE;
    }

    scanning_active_ = true;
    thread_terminated_ = false;
    thread_generation_++;

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
        scanning_active_ = false;
        thread_terminated_ = false;
        return StartResult::THREAD_CREATION_FAILED;
    }

    return StartResult::SUCCESS;
}

void ScanningCoordinator::stop_coordinated_scanning() noexcept {
    MutexLock thread_lock(thread_mutex_, LockOrder::DATA_MUTEX);

    bool was_scanning;
    uint32_t expected_generation;
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        was_scanning = scanning_active_;
        if (!was_scanning) {
            return;
        }
        expected_generation = thread_generation_;
        scanning_active_ = false;
    }

    // Wait for thread to terminate with timeout
    constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
    constexpr uint32_t POLL_INTERVAL_MS = 10;
    systime_t deadline = chTimeNow() + MS2ST(TERMINATION_TIMEOUT_MS);

    bool terminated = false;
    while (chTimeNow() < deadline) {
        {
            MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
            if (thread_terminated_ && thread_generation_ == expected_generation) {
                terminated = true;
                break;
            }
        }
        chThdSleepMilliseconds(POLL_INTERVAL_MS);
    }

    scanning_thread_ = nullptr;
}

bool ScanningCoordinator::is_scanning_active() const noexcept {
    // FIX #RC-1: Full mutex protection for state access
    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
    return scanning_active_;
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept {
    // Update scan interval
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        scan_interval_ms_ = settings.scan_interval_ms;
    }

    // Guard clause: Not scanning, no need to update scanner
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        if (!scanning_active_) {
            return;
        }
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
    // during thread restart: each thread start increments counter, thread only
    // sets termination flag if generation matches, coordinator waits for matching generation.
    uint32_t my_generation;
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        my_generation = thread_generation_;
    }

    // Convert timeout constant to ChibiOS systime_t
    constexpr systime_t SCAN_CYCLE_TIMEOUT_ST = MS2ST(CoordinatorConstants::SCAN_CYCLE_TIMEOUT_MS);

    // Counters for error detection
    TimeoutCount consecutive_timeouts = 0;

    // Track initialization wait time outside loop to ensure timeout is
    // calculated correctly across loop iterations
    systime_t init_wait_start_time = chTimeNow();

    // FIX #RC-1: Document synchronization pattern for scanning_active_ access
    // - All reads/writes to scanning_active_ are protected by state_mutex_
    // - No lock-free reads - all state access is synchronized
    while (true) {
        // Check if still active (with mutex protection)
        bool active;
        {
            MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
            active = scanning_active_;
        }
        if (!active) {
            break;
        }

        // Check initialization complete with timeout to prevent infinite loop
        if (!scanner_.is_initialization_complete()) {
            // Check if initialization timeout has been reached
            systime_t init_wait_time = chTimeNow() - init_wait_start_time;
            if (init_wait_time > MS2ST(CoordinatorConstants::INITIALIZATION_TIMEOUT_MS)) {
                // Initialization timeout - signal stop to coordinator
                {
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                    scanning_active_ = false;
                }

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                {
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
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
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                    scanning_active_ = false;
                }

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                {
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
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
        uint32_t interval_ms;
        {
            MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
            interval_ms = scan_interval_ms_;
        }
        chThdSleepMilliseconds(interval_ms);
    }

    // Set termination flag before exit (thread-safe)
    // Only set flag if generation matches (prevents missed signal race condition)
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        if (thread_generation_ == my_generation) {
            thread_terminated_ = true;
        }
    }

    // Normal exit
    chThdExit(ReturnCodes::SUCCESS);
    return ReturnCodes::SUCCESS;
}

} // namespace ui::apps::enhanced_drone_analyzer
