// scanning_coordinator.cpp - Coordinate scanning operations for EDA
// Implementation of ScanningCoordinator class

#include "scanning_coordinator.hpp"
#include "ui_enhanced_drone_analyzer.hpp"
#include "diamond_core.hpp"
#include <ch.h>

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// ScanningCoordinator Implementation
// ===========================================

ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
                                        DroneHardwareController& hardware,
                                        DroneScanner& scanner,
                                        DroneDisplayController& display_controller,
                                        ::AudioManager& audio_controller)
    : nav_(nav)
    , hardware_(hardware)
    , scanner_(scanner)
    , display_controller_(display_controller)
    , audio_controller_(audio_controller)
    , scanning_active_(false)
    , scanning_thread_(nullptr)
    , scan_interval_ms_(EDA::Constants::DEFAULT_SCAN_INTERVAL_MS) {
}

ScanningCoordinator::~ScanningCoordinator() {
    stop_coordinated_scanning();
}

bool ScanningCoordinator::start_coordinated_scanning() noexcept {
    if (scanning_active_.load(std::memory_order_acquire)) return false;
    scanning_active_.store(true, std::memory_order_release);
    
    // FIX #23: Add proper error handling for thread creation
    // If thread creation fails, properly clean up state
    scanning_thread_ = chThdCreateStatic(
        coordinator_wa_,
        sizeof(coordinator_wa_),
        NORMALPRIO,
        scanning_thread_function,
        this
    );
    if (!scanning_thread_) {
        // FIX #23: Thread creation failed - properly clean up state
        scanning_active_.store(false, std::memory_order_release);
        return false;  // Indicate failure
    }
    return true;  // Indicate success
}

void ScanningCoordinator::stop_coordinated_scanning() noexcept {
    if (scanning_active_.load(std::memory_order_acquire)) {
        scanning_active_.store(false, std::memory_order_release);

        if (scanning_thread_) {
            chThdWait(scanning_thread_);
            scanning_thread_ = nullptr;
        }
    }
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept {
    scan_interval_ms_ = settings.scan_interval_ms;

    if (scanning_active_.load(std::memory_order_acquire)) {
        scanner_.update_scan_range(settings.wideband_min_freq_hz,
                                   settings.wideband_max_freq_hz);
    }
}

// DIAMOND OPTIMIZATION: Use const char* instead of std::string (zero heap allocation)
void ScanningCoordinator::show_session_summary([[maybe_unused]] const char* summary) noexcept {
}

msg_t ScanningCoordinator::scanning_thread_function(void* arg) noexcept {
    auto coordinator = static_cast<ScanningCoordinator*>(arg);
    return coordinator->coordinated_scanning_thread();
}

msg_t ScanningCoordinator::coordinated_scanning_thread() noexcept {
    // DIAMOND CODE: No exceptions - perform scan cycle directly
    // Error handling is managed by scanner_.perform_scan_cycle() via return codes

    // Timeout protection constants
    constexpr systime_t SCAN_CYCLE_TIMEOUT_MS = MS2ST(10000);
    uint32_t consecutive_timeouts = 0;
    constexpr uint32_t MAX_CONSECUTIVE_TIMEOUTS = 3;

    while (scanning_active_.load(std::memory_order_acquire)) {
        systime_t cycle_start = chTimeNow();

        scanner_.perform_scan_cycle(hardware_);

        systime_t cycle_duration = chTimeNow() - cycle_start;

        // Check for scan cycle timeout
        if (cycle_duration > SCAN_CYCLE_TIMEOUT_MS) {
            consecutive_timeouts++;
            if (consecutive_timeouts >= MAX_CONSECUTIVE_TIMEOUTS) {
                scanning_active_.store(false, std::memory_order_release);
                scanning_thread_ = nullptr;
                chThdExit(static_cast<msg_t>(-1));
                return -1;
            }
        } else {
            consecutive_timeouts = 0;
        }

        chThdSleepMilliseconds(scan_interval_ms_);
    }
    scanning_active_.store(false, std::memory_order_release);
    scanning_thread_ = nullptr;
    chThdExit(0);
    return 0;
}

}  // namespace ui::apps::enhanced_drone_analyzer

// ===========================================
// TrendUtils::TREND_NAMES Definition
// ===========================================

namespace ui::apps::enhanced_drone_analyzer::DiamondCore {

const char* const TrendUtils::TREND_NAMES[] = {
    "STATIC",    // 0
    "APPROACHING", // 1
    "RECEDING",  // 2
    "UNKNOWN"    // 3
};

};
