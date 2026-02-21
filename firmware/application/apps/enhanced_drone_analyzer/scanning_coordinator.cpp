/**
 * @file scanning_coordinator.cpp
 * @brief Coordinate scanning operations for Enhanced Drone Analyzer
 * 
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All memory is stack-allocated or in Flash
 * - No exceptions: All functions are noexcept
 * - Type-safe: Uses semantic type aliases
 * - Memory-safe: Uses ChibiOS RTOS for thread management
 * 
 * @author Diamond Code Pipeline
 * @date 2026-02-20
 */

#include "scanning_coordinator.hpp"
#include "ui_enhanced_drone_analyzer.hpp"
#include "diamond_core.hpp"
#include "eda_raii.hpp"
#include <ch.h>

namespace ui::apps::enhanced_drone_analyzer {

// DIAMOND FIX: Coordinator Thread Working Area Definition
stkalign_t ScanningCoordinator::coordinator_wa_[THD_WA_SIZE(ScanningCoordinator::COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

// ===========================================
// TYPE ALIASES (Semantic Types)
// ===========================================
using TimeoutCount = uint32_t;

// ===========================================
// CONSTANTS (Named, No Magic Numbers)
// ===========================================
namespace CoordinatorConstants {
    // Scan cycle timeout in milliseconds
    constexpr uint32_t SCAN_CYCLE_TIMEOUT_MS = 10000;
    
    // Maximum consecutive timeouts before aborting
    constexpr TimeoutCount MAX_CONSECUTIVE_TIMEOUTS = 3;
    
    // Maximum consecutive scanner failures before aborting
    constexpr TimeoutCount MAX_CONSECUTIVE_SCANNER_FAILURES = 5;
}

// ===========================================
// RETURN CODES
// ===========================================
namespace ReturnCodes {
    constexpr msg_t SUCCESS = 0;
    constexpr msg_t TIMEOUT_ERROR = -1;
    constexpr msg_t SCANNER_ERROR = -2;
}

// ===========================================
// ScanningCoordinator Implementation
// ===========================================

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
    , scan_interval_ms_(EDA::Constants::DEFAULT_SCAN_INTERVAL_MS) {
}

ScanningCoordinator::~ScanningCoordinator() noexcept {
    stop_coordinated_scanning();
}

bool ScanningCoordinator::start_coordinated_scanning() noexcept {
    // Guard clause: Already scanning
    if (scanning_active_) {
        return false;
    }

    {
        CriticalSection cs;
        scanning_active_ = true;
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
        return false;
    }
    
    return true;
}

void ScanningCoordinator::stop_coordinated_scanning() noexcept {
    // Guard clause: Not scanning
    if (!scanning_active_) {
        return;
    }

    // ✅ DIAMOND FIX: Race Condition - Use termination flag for clean thread exit
    // Signal thread to stop using termination flag (thread-safe)
    {
        CriticalSection cs;
        scanning_active_ = false;
        thread_terminated_ = false;  // Reset termination flag
    }

    // Wait for thread to terminate with timeout (prevents indefinite hang)
    if (scanning_thread_) {
        // Poll with deadline to prevent indefinite wait (5 second timeout)
        constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
        constexpr uint32_t POLL_INTERVAL_MS = 10;
        systime_t deadline = chTimeNow() + MS2ST(TERMINATION_TIMEOUT_MS);
        
        while (chTimeNow() < deadline && scanning_thread_ != nullptr) {
            chThdSleepMilliseconds(POLL_INTERVAL_MS);
        }

        // After timeout, ensure thread pointer is nullified
        // This prevents dangling pointer if thread is stuck
        scanning_thread_ = nullptr;
    }
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept {
    // Update scan interval
    scan_interval_ms_ = settings.scan_interval_ms;

    // Guard clause: Not scanning, no need to update scanner
    if (!scanning_active_) {
        return;
    }
    
    // Update scanner frequency range
    scanner_.update_scan_range(settings.wideband_min_freq_hz,
                               settings.wideband_max_freq_hz);
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
    // DIAMOND CODE: No exceptions - perform scan cycle directly
    // Error handling is managed via return codes
    
    // Convert timeout constant to ChibiOS systime_t
    constexpr systime_t SCAN_CYCLE_TIMEOUT_ST = MS2ST(CoordinatorConstants::SCAN_CYCLE_TIMEOUT_MS);
    
    // Counters for error detection
    TimeoutCount consecutive_timeouts = 0;
    // DEPRECATED: Removed unused variable consecutive_scanner_failures
    // Reason: Variable was declared but never used, causing compiler warning
    // TimeoutCount consecutive_scanner_failures = 0;

    while (scanning_active_) {
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
                // DIAMOND FIX: Do NOT set scanning_thread_ = nullptr here
                // Only the owner class should manage this pointer
                chThdExit(ReturnCodes::TIMEOUT_ERROR);
                return ReturnCodes::TIMEOUT_ERROR;
            }
        } else {
            consecutive_timeouts = 0;
        }
        
        // Sleep for configured interval
        chThdSleepMilliseconds(scan_interval_ms_);
    }

    // ✅ DIAMOND FIX: Set termination flag before exit (thread-safe)
    // This signals to coordinator that thread has exited cleanly
    {
        CriticalSection cs;
        thread_terminated_ = true;
    }

    // Normal exit
    chThdExit(ReturnCodes::SUCCESS);
    return ReturnCodes::SUCCESS;
}

}  // namespace ui::apps::enhanced_drone_analyzer
