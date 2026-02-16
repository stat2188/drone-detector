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
    , audio_controller_(audio_controller) {
}

ScanningCoordinator::~ScanningCoordinator() {
    stop_coordinated_scanning();
}

void ScanningCoordinator::start_coordinated_scanning() {
    if (!scanning_active_) {
        scanning_active_ = true;
        scanning_thread_ = chThdCreateStatic(coordinator_wa_, sizeof(coordinator_wa_),
                                           NORMALPRIO + 1,
                                           scanning_thread_function, this);
    }
}

void ScanningCoordinator::stop_coordinated_scanning() {
    if (scanning_active_) {
        scanning_active_ = false;
        if (scanning_thread_) {
            chThdWait(scanning_thread_);
            scanning_thread_ = nullptr;
        }
    }
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
    // TODO: Implement parameter update logic
}

void ScanningCoordinator::show_session_summary(const std::string& summary) {
    // TODO: Implement session summary display
}

msg_t ScanningCoordinator::scanning_thread_function(void* arg) {
    auto coordinator = static_cast<ScanningCoordinator*>(arg);
    return coordinator->coordinated_scanning_thread();
}

msg_t ScanningCoordinator::coordinated_scanning_thread() {
    while (scanning_active_) {
        // TODO: Implement coordinated scanning logic
        chThdSleepMilliseconds(scan_interval_ms_);
    }
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
