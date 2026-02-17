// scanning_coordinator.hpp - Coordinate scanning operations for EDA

#ifndef SCANNING_COORDINATOR_HPP_
#define SCANNING_COORDINATOR_HPP_

#include <memory>
#include <atomic>
#include "ui_drone_common_types.hpp"
#include "ui_navigation.hpp"
#include "thread_base.hpp"
#include "ui_drone_audio.hpp"

class AudioManager;

namespace ui::apps::enhanced_drone_analyzer {

class DroneHardwareController;
class DroneScanner;
class DroneDisplayController;

class ScanningCoordinator {
public:
    ScanningCoordinator(NavigationView& nav,
                       DroneHardwareController& hardware,
                       DroneScanner& scanner,
                       DroneDisplayController& display_controller,
                       ::AudioManager& audio_controller);
    ~ScanningCoordinator();

    void start_coordinated_scanning();
    void stop_coordinated_scanning();
    bool is_scanning_active() const { return scanning_active_; }
    void update_runtime_parameters(const DroneAnalyzerSettings& settings);
    void show_session_summary(const std::string& summary);

    ScanningCoordinator(const ScanningCoordinator&) = delete;
    ScanningCoordinator(ScanningCoordinator&&) = delete;
    ScanningCoordinator& operator=(const ScanningCoordinator&) = delete;
    ScanningCoordinator& operator=(ScanningCoordinator&&) = delete;

private:
    static msg_t scanning_thread_function(void* arg);
    msg_t coordinated_scanning_thread();

    NavigationView& nav_;
    DroneHardwareController& hardware_;
    DroneScanner& scanner_;
    DroneDisplayController& display_controller_;
    ::AudioManager& audio_controller_;
    std::atomic<bool> scanning_active_{false};
    Thread* scanning_thread_ = nullptr;
    uint32_t scan_interval_ms_ = 712;
    // Phase 2 Optimization: Reduced from 12KB to 6KB for memory savings
    // Scott Meyers Item 15: Prefer constexpr to #define
    static constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 6144;

    static WORKING_AREA(coordinator_wa_, COORDINATOR_THREAD_STACK_SIZE);
};


}  // namespace ui::apps::enhanced_drone_analyzer

#endif  // SCANNING_COORDINATOR_HPP_