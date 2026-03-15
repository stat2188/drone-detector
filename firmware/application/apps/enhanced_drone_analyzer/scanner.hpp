#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <cstdint>
#include <cstddef>
#include "drone_types.hpp"
#include "error_handler.hpp"
#include "locking.hpp"
#include "constants.hpp"
#include "database.hpp"
#include "hardware_controller.hpp"

// Forward declarations for ChibiOS types
struct mutex_t;

namespace drone_analyzer {

/**
 * @brief Scanner state
 */
enum class ScannerState : uint8_t {
    IDLE = 0,
    SCANNING = 1,
    PAUSED = 2,
    ERROR = 3
};

/**
 * @brief Scan configuration
 */
struct ScanConfig {
    ScanningMode mode;
    FreqHz start_frequency;
    FreqHz end_frequency;
    uint32_t scan_interval_ms;
    int32_t rssi_threshold_dbm;
    uint32_t stale_timeout_ms;
    
    /**
     * @brief Default constructor
     */
    ScanConfig() noexcept;
    
    /**
     * @brief Constructor with values
     */
    ScanConfig(ScanningMode m, FreqHz start, FreqHz end) noexcept;
};

/**
 * @brief Scan statistics
 */
struct ScanStatistics {
    uint32_t total_scan_cycles;
    uint32_t successful_cycles;
    uint32_t failed_cycles;
    uint32_t drones_detected;
    uint32_t max_rssi_dbm;
    
    /**
     * @brief Default constructor
     */
    ScanStatistics() noexcept;
    
    /**
     * @brief Reset statistics
     */
    void reset() noexcept;
};

/**
 * @brief Alert callback function type
 * @param message Alert message
 * @param priority Alert priority (0=LOW, 1=MEDIUM, 2=HIGH, 3=CRITICAL)
 */
using AlertCallback = void(*)(const char* message, uint8_t priority);

/**
 * @brief Drone scanner
 * @note Main scanning logic for drone detection
 * @note Simplified: removed wideband/hybrid/panoramic modes, removed FHSS detection
 * @note Thread-safe with mutex protection
 */
class DroneScanner {
public:
    /**
     * @brief Constructor
     * @param database Reference to database manager
     * @param hardware Reference to hardware controller
     */
    DroneScanner(DatabaseManager& database, HardwareController& hardware) noexcept;
    
    /**
     * @brief Destructor
     */
    ~DroneScanner() noexcept;
    
    // Delete copy and move operations
    DroneScanner(const DroneScanner&) = delete;
    DroneScanner& operator=(const DroneScanner&) = delete;
    DroneScanner(DroneScanner&&) = delete;
    DroneScanner& operator=(DroneScanner&&) = delete;
    
    /**
     * @brief Initialize scanner
     * @return ErrorCode::SUCCESS if initialized, error code otherwise
     */
    [[nodiscard]] ErrorCode initialize() noexcept;
    
    /**
     * @brief Start scanning
     * @return ErrorCode::SUCCESS if started, error code otherwise
     */
    [[nodiscard]] ErrorCode start_scanning() noexcept;
    
    /**
     * @brief Stop scanning
     * @return ErrorCode::SUCCESS if stopped, error code otherwise
     */
    [[nodiscard]] ErrorCode stop_scanning() noexcept;
    
    /**
     * @brief Pause scanning
     * @return ErrorCode::SUCCESS if paused, error code otherwise
     */
    [[nodiscard]] ErrorCode pause_scanning() noexcept;
    
    /**
     * @brief Resume scanning
     * @return ErrorCode::SUCCESS if resumed, error code otherwise
     */
    [[nodiscard]] ErrorCode resume_scanning() noexcept;
    
    /**
     * @brief Perform single scan cycle
     * @note Called periodically by scanner thread
     * @return ErrorCode::SUCCESS if cycle completed, error code otherwise
     */
    [[nodiscard]] ErrorCode perform_scan_cycle() noexcept;
    
    /**
     * @brief Update tracked drones with new data
     * @param frequency Frequency of detected signal
     * @param rssi RSSI value
     * @param timestamp Timestamp of detection
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     */
    [[nodiscard]] ErrorCode update_tracked_drones(
        FreqHz frequency,
        RssiValue rssi,
        SystemTime timestamp
    ) noexcept;
    
    /**
     * @brief Get tracked drones
     * @param drones Output buffer for drones
     * @param max_count Maximum number of drones to copy
     * @return Number of drones copied
     */
    [[nodiscard]] size_t get_tracked_drones(
        TrackedDrone* drones,
        size_t max_count
    ) const noexcept;
    
    /**
     * @brief Get display data for UI
     * @param display_data Output display data
     * @return ErrorCode::SUCCESS if data retrieved, error code otherwise
     */
    [[nodiscard]] ErrorCode get_display_data(DisplayData& display_data) const noexcept;
    
    /**
     * @brief Get scanner state
     * @return Current scanner state
     */
    [[nodiscard]] ScannerState get_state() const noexcept;
    
    /**
     * @brief Check if scanning is active
     * @return true if scanning, false otherwise
     */
    [[nodiscard]] bool is_scanning() const noexcept;
    
    /**
     * @brief Get scan configuration
     * @return Current scan configuration
     */
    [[nodiscard]] ScanConfig get_config() const noexcept;
    
    /**
     * @brief Set scan configuration
     * @param config Configuration to apply
     * @return ErrorCode::SUCCESS if set, error code otherwise
     */
    [[nodiscard]] ErrorCode set_config(const ScanConfig& config) noexcept;
    
    /**
     * @brief Get scan statistics
     * @return Current scan statistics
     */
    [[nodiscard]] ScanStatistics get_statistics() const noexcept;
    
    /**
     * @brief Reset scan statistics
     */
    void reset_statistics() noexcept;
    
    /**
     * @brief Get current scan frequency
     * @return ErrorResult containing current frequency or error
     */
    [[nodiscard]] ErrorResult<FreqHz> get_current_frequency() const noexcept;
    
    /**
     * @brief Get number of tracked drones
     * @return Number of tracked drones
     */
    [[nodiscard]] size_t get_tracked_count() const noexcept;
    
    /**
     * @brief Clear all tracked drones
     */
    void clear_tracked_drones() noexcept;
    
    /**
     * @brief Remove stale drones
     * @param current_time Current system time
     */
    void remove_stale_drones(SystemTime current_time) noexcept;
    
private:
    /**
     * @brief Internal: Perform scan cycle
     * @note Called by perform_scan_cycle() with mutex held
     * @return ErrorCode::SUCCESS if cycle completed, error code otherwise
     */
    [[nodiscard]] ErrorCode perform_scan_cycle_internal() noexcept;
    
    /**
     * @brief Internal: Update tracked drone
     * @note Called by update_tracked_drones() with mutex held
     * @param frequency Frequency of detected signal
     * @param rssi RSSI value
     * @param timestamp Timestamp of detection
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     */
    [[nodiscard]] ErrorCode update_tracked_drone_internal(
        FreqHz frequency,
        RssiValue rssi,
        SystemTime timestamp
    ) noexcept;
    
    /**
     * @brief Internal: Find drone by frequency
     * @param frequency Frequency to find
     * @return ErrorResult containing index or error
     */
    [[nodiscard]] ErrorResult<size_t> find_drone_by_frequency_internal(
        FreqHz frequency
    ) const noexcept;
    
    /**
     * @brief Internal: Add new tracked drone
     * @param frequency Frequency of detected signal
     * @param rssi RSSI value
     * @param timestamp Timestamp of detection
     * @return ErrorCode::SUCCESS if added, error code otherwise
     */
    [[nodiscard]] ErrorCode add_tracked_drone_internal(
        FreqHz frequency,
        RssiValue rssi,
        SystemTime timestamp
    ) noexcept;
    
    /**
     * @brief Internal: Remove stale drones
     * @note Called by remove_stale_drones() with mutex held
     * @param current_time Current system time
     */
    void remove_stale_drones_internal(SystemTime current_time) noexcept;
    
    /**
     * @brief Internal: Update display data
     * @note Called by get_display_data() with mutex held
     * @param display_data Output display data
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     */
    [[nodiscard]] ErrorCode update_display_data_internal(
        DisplayData& display_data
    ) const noexcept;
    
    /**
     * @brief Internal: Determine drone type from frequency
     * @param frequency Frequency to analyze
     * @return Drone type
     */
    [[nodiscard]] DroneType determine_drone_type_internal(FreqHz frequency) const noexcept;
    
    /**
     * @brief Internal: Determine threat level from RSSI
     * @param rssi RSSI value
     * @return Threat level
     */
    [[nodiscard]] ThreatLevel determine_threat_level_internal(RssiValue rssi) const noexcept;
    
    /**
     * @brief Internal: Validate scan configuration
     * @param config Configuration to validate
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_config_internal(const ScanConfig& config) const noexcept;
    
    // References to dependencies
    DatabaseManager& database_;
    HardwareController& hardware_;
    
    // Scanner state
    ScannerState state_;
    
    // Scan configuration
    ScanConfig config_;
    
    // Scan statistics
    ScanStatistics statistics_;
    
    // Tracked drones (fixed-size array, no heap allocation)
    std::array<TrackedDrone, MAX_TRACKED_DRONES> tracked_drones_;
    
    // Number of tracked drones
    size_t tracked_count_;
    
    // Current scan frequency
    FreqHz current_frequency_;
    
    // Last scan time
    SystemTime last_scan_time_;
    
    // Scanning active flag
    AtomicFlag scanning_active_;
    
    // Mutex for thread safety (LockOrder::DATA_MUTEX)
    mutable mutex_t* mutex_;
};

} // namespace drone_analyzer

#endif // SCANNER_HPP
