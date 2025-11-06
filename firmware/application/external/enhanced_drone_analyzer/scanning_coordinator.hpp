// scanning_coordinator.hpp - Coordinate scanning operations for EDA

#pragma once

#include <memory>
#include "ui_drone_common_types.hpp"
#include "ui_scanner_combined.hpp"  // For namespace
#include "ui_navigation.hpp"
#include "thread_base.hpp"

namespace ui::external_app::enhanced_drone_analyzer {

class DroneHardwareController;
class DroneScanner;
class DroneDisplayController;
class AudioManager;

class ScanningCoordinator {
public:
    ScanningCoordinator(NavigationView& nav,
                       DroneHardwareController& hardware,
                       DroneScanner& scanner,
                       DroneDisplayController& display_controller,
                       AudioManager& audio_controller);
    ~ScanningCoordinator();

    void start_coordinated_scanning();
    void stop_coordinated_scanning();
    bool is_scanning_active() const { return scanning_active_; }
    void update_runtime_parameters(const DroneAnalyzerSettings& settings);

private:
    static msg_t scanning_thread_function(void* arg);
    msg_t coordinated_scanning_thread();

    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    DroneDisplayController& display_controller_;
    AudioManager& audio_;
    bool scanning_active_ = false;
    Thread* coordinator_thread_ = nullptr;
    static constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 4096;
};

// Dummy implementation to avoid incomplete type
inline ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
                                              DroneHardwareController& hardware,
                                              DroneScanner& scanner,
                                              DroneDisplayController& display_controller,
                                              AudioManager& audio_controller)
    : nav_(nav), hardware_(hardware), scanner_(scanner), display_controller_(display_controller), audio_(audio_controller) {}

inline ScanningCoordinator::~ScanningCoordinator() {
    stop_coordinated_scanning();
}

inline void ScanningCoordinator::start_coordinated_scanning() {
    if (scanning_active_) return;
    scanning_active_ = true;
    // Start thread logic would go here
}

inline void ScanningCoordinator::stop_coordinated_scanning() {
    if (!scanning_active_) return;
    scanning_active_ = false;
    // Stop thread logic would go here
}

inline void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
    (void)settings;  // Suppress unused parameter warning
}

inline msg_t ScanningCoordinator::scanning_thread_function(void* arg) {
    return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
}

inline msg_t ScanningCoordinator::coordinated_scanning_thread() {
    return MSG_OK;
}

} // namespace ui::external_app::enhanced_drone_analyzer
