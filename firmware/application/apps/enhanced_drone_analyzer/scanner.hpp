#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "ch.h"
#include "drone_types.hpp"
#include "locking.hpp"
#include "constants.hpp"
#include "database.hpp"
#include "hardware_controller.hpp"
#include "audio_alerts.hpp"
#include "convert.hpp"
#include "message.hpp"

namespace drone_analyzer {

/**
 * @brief Scan configuration
 * @note ScannerState is defined in drone_types.hpp
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
 * @param threat_level Threat level based on RSSI
 * @note Audio tone varies by threat: CRITICAL=1500Hz, HIGH=1200Hz, MEDIUM=1000Hz
 */
using ThreatAlertCallback = void(*)(ThreatLevel threat_level);

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
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode initialize() noexcept;
    
    /**
     * @brief Start scanning
     * @return ErrorCode::SUCCESS if started, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode start_scanning() noexcept;
    
    /**
     * @brief Stop scanning
     * @return ErrorCode::SUCCESS if stopped, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode stop_scanning() noexcept;
    
    /**
     * @brief Pause scanning
     * @return ErrorCode::SUCCESS if paused, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode pause_scanning() noexcept;
    
    /**
     * @brief Resume scanning
     * @return ErrorCode::SUCCESS if resumed, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode resume_scanning() noexcept;
    
    /**
     * @brief Perform single scan cycle
     * @note Called periodically by scanner thread
     * @return ErrorCode::SUCCESS if cycle completed, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode perform_scan_cycle() noexcept;
    
    /**
     * @brief Update tracked drones with new data
     * @param frequency Frequency of detected signal
     * @param rssi RSSI value
     * @param timestamp Timestamp of detection
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode update_tracked_drones(
        FreqHz frequency,
        RssiValue rssi,
        SystemTime timestamp
    ) noexcept;

    /**
     * @brief Process spectrum data and extract RSSI
     * @param spectrum Channel spectrum data
     * @param current_frequency Current tuned frequency (for tracking)
     * @return ErrorResult containing RSSI value or error
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @note Extracts maximum power from spectrum and converts to dBm
     * @note Updates tracked drones if RSSI above threshold
     */
    [[nodiscard]] ErrorResult<RssiValue> process_spectrum_data(
        const ChannelSpectrum& spectrum,
        FreqHz current_frequency
    ) noexcept;
    
    /**
     * @brief Process ChannelSpectrum message directly
     * @param spectrum Channel spectrum data
     * @return ErrorCode::SUCCESS if processed, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @note Uses scanner's internal current_frequency
     * @note Updates tracked drones if RSSI above threshold
     */
    [[nodiscard]] ErrorCode process_spectrum_message(const ChannelSpectrum& spectrum) noexcept;

    /**
     * @brief Fast spectrum processing for UI updates
     * @param spectrum Channel spectrum data
     * @return ErrorResult containing RSSI value or error
     * @note Uses try-lock to avoid blocking UI thread
     * @note Does not update tracked drones - only extracts RSSI
     */
    [[nodiscard]] ErrorResult<RssiValue> process_spectrum_fast(const ChannelSpectrum& spectrum) noexcept;
    
    /**
     * @brief Get tracked drones
     * @param drones Output buffer for drones
     * @param max_count Maximum number of drones to copy
     * @return Number of drones copied
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @note Actual copy count is min(max_count, MAX_TRACKED_DRONES, tracked_count_)
     */
    [[nodiscard]] size_t get_tracked_drones(
        TrackedDrone* drones,
        size_t max_count
    ) const noexcept;
    
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
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ScanConfig get_config() const noexcept;
    
    /**
     * @brief Set scan configuration
     * @param config Configuration to apply
     * @return ErrorCode::SUCCESS if set, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode set_config(const ScanConfig& config) noexcept;
    
    /**
     * @brief Get scan statistics
     * @return Current scan statistics
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ScanStatistics get_statistics() const noexcept;
    
    /**
     * @brief Reset scan statistics
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void reset_statistics() noexcept;
    
    /**
     * @brief Get current scan frequency
     * @return ErrorResult containing current frequency or error
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorResult<FreqHz> get_current_frequency() const noexcept;
    
    /**
     * @brief Get number of tracked drones
     * @return Number of tracked drones
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] size_t get_tracked_count() const noexcept;

    // ========================================================================
    // Fast Scanner Integration Methods
    // ========================================================================

    /**
     * @brief Get current drone type string
     * @param buffer Destination buffer for drone type string
     * @param buffer_size Size of destination buffer (must be >= 2)
     * @return ErrorCode::SUCCESS if copied, error otherwise
     * @note Only valid during LOCKING state
     * @note Thread-safe: acquires mutex (LockOrder::DATA_MUTEX)
     * @note Copies to caller's buffer while holding mutex to prevent race conditions
     */
    [[nodiscard]] ErrorCode get_current_drone_type(char* buffer, size_t buffer_size) const noexcept;

    /**
     * @brief Get freq lock count
     * @return Current lock count (0-10)
     */
    [[nodiscard]] uint32_t get_freq_lock_count() const noexcept;

    /**
     * @brief Set freq lock count
     * @param count Lock count
     */
    void set_freq_lock_count(uint32_t count) noexcept;

    /**
     * @brief Get locked frequency
     * @return Locked frequency (0 if not tracking)
     */
    [[nodiscard]] FreqHz get_locked_frequency() const noexcept;
    
    /**
     * @brief Clear all tracked drones
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void clear_tracked_drones() noexcept;
    
    /**
     * @brief Remove stale drones
     * @param current_time Current system time
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void remove_stale_drones(SystemTime current_time) noexcept;
    
    /**
     * @brief Set alert callback
     * @param callback Alert callback function
     * @note Callback will be called when alerts are triggered
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @warning The callback function MUST be thread-safe and reentrant-safe
     * @warning The callback MUST NOT acquire any mutexes or perform blocking operations
     * @warning The callback MUST execute quickly (preferably < 1ms) to avoid delaying scanner thread
     */
    void set_alert_callback(ThreatAlertCallback callback) noexcept;
    
    /**
     * @brief Trigger alert
     * @param threat_level Threat level to report
     * @note Calls alert callback if set
     * @note Callback is invoked OUTSIDE mutex lock to prevent deadlocks
     * @pre Mutex must be held (LockOrder::DATA_MUTEX) when accessing alert_callback_
     */
    void trigger_alert(ThreatLevel threat_level) noexcept;
    
private:
    /**
     * @brief Internal: Perform scan cycle
     * @note Called by perform_scan_cycle() with mutex held
     * @return ErrorCode::SUCCESS if cycle completed, error code otherwise
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode perform_scan_cycle_internal() noexcept;
    
    /**
     * @brief Internal: Update tracked drone
     * @note Called by update_tracked_drones() with mutex held
     * @param frequency Frequency of detected signal
     * @param rssi RSSI value
     * @param timestamp Timestamp of detection
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
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
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorResult<size_t> find_drone_by_frequency_internal(
        FreqHz frequency
    ) const noexcept;
    
    /**
     * @brief Internal: Add new tracked drone
     * @param frequency_hz Frequency of detected signal (Hz)
     * @param rssi_dbm RSSI value (dBm)
     * @param timestamp_ms Timestamp of detection (ms)
     * @return ErrorCode::SUCCESS if added, error code otherwise
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode add_tracked_drone_internal(
        FreqHz frequency_hz,
        RssiValue rssi_dbm,
        SystemTime timestamp_ms
    ) noexcept;
    
     /**
      * @brief Internal: Remove stale drones
      * @note Called by remove_stale_drones() with mutex held
      * @param current_time Current system time
      * @pre Mutex must be held (LockOrder::DATA_MUTEX)
      */
    void remove_stale_drones_internal(SystemTime current_time) noexcept;

    /**
     * @brief Internal: Find maximum power value in spectrum
     * @param spectrum Channel spectrum data
     * @return Maximum power value (0-255)
     */
    [[nodiscard]] uint8_t find_max_power(const ChannelSpectrum& spectrum) noexcept;
    
    /**
     * @brief Internal: Determine drone type from frequency
     * @param frequency Frequency to analyze
     * @return Drone type
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     * @note Uses database_.find_entry() to look up drone type from freqman DB
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

    // Fast scanner state (protected by mutex_)
    uint32_t freq_lock_count_{0};              // Frequency lock counter (0-10)
    FreqHz locked_frequency_{0};                // Locked frequency for tracking
    SystemTime track_start_time_{0};           // Tracking start time
    char current_drone_type_[5]{'\0', '\0', '\0', '\0', '\0'};  // All bytes initialized
    bool drone_type_valid_{false};              // Drone type valid flag
    
    // Scan statistics
    ScanStatistics statistics_;
    
    // Tracked drones (fixed-size array, no heap allocation)
    std::array<TrackedDrone, MAX_TRACKED_DRONES> tracked_drones_;
    
    // Number of tracked drones (uint8_t is sufficient for MAX_TRACKED_DRONES=20)
    uint8_t tracked_count_;
    
    // Current scan frequency
    FreqHz current_frequency_;
    
    // Last scan time
    SystemTime last_scan_time_;
    
    // Scanning active flag
    AtomicFlag scanning_active_;
    
    // Alert callback
    ThreatAlertCallback alert_callback_;
    
    // Last threat level for each tracked drone (for detecting threat increases)
    std::array<ThreatLevel, MAX_TRACKED_DRONES> last_threat_levels_;
    
    // Mutex for thread safety (LockOrder::DATA_MUTEX)
    // Direct member storage - no heap allocation, no pointer indirection
    mutable Mutex mutex_;

    // State transition control flag
    AtomicFlag state_transition_allowed_;

    // Scan cycle in progress flag (prevents concurrent scan cycles)
    AtomicFlag scan_cycle_in_progress_;

    // Alert callback in progress flag (prevents re-entrant calls)
    AtomicFlag alert_callback_in_progress_;
};

} // namespace drone_analyzer

#endif // SCANNER_HPP
