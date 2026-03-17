#ifndef RSSI_DETECTOR_HPP
#define RSSI_DETECTOR_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "drone_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief RSSI detection result
 * @note Fixed-size structure, no heap allocation
 */
struct RSSIDetectionResult {
    bool drone_detected;                // Drone detected flag
    ThreatLevel threat_level;            // Threat level classification
    RssiValue rssi;                      // Current RSSI value
    RssiValue average_rssi;              // Average RSSI over samples
    MovementTrend movement_trend;        // Movement trend
    uint8_t sample_count;                // Number of samples processed
    uint8_t reserved[7];                 // Reserved for future use (padding)
};

/**
 * @brief RSSI statistics
 * @note Fixed-size structure, no heap allocation
 */
struct RSSIStatistics {
    uint32_t total_samples;              // Total samples processed
    uint32_t detections;                 // Total detections
    uint32_t high_threat_count;          // High threat detections
    uint32_t critical_threat_count;      // Critical threat detections
    RssiValue min_rssi;                  // Minimum RSSI observed
    RssiValue max_rssi;                  // Maximum RSSI observed
    uint32_t approaching_count;          // Approaching drone count
    uint32_t receding_count;             // Receding drone count
    uint32_t static_count;               // Static drone count
};

/**
 * @brief RSSI-based drone detector
 * @note No floating-point operations, uses integer arithmetic
 * @note No heap allocation, uses fixed-size arrays
 * @note Separated from UI layer (no UI dependencies)
 * @note Removed: FHSS tracking, intelligent scanning
 */
class RSSIDetector {
public:
    /**
     * @brief Default constructor
     */
    RSSIDetector() noexcept;

    /**
     * @brief Initialize RSSI detector
     * @param detection_threshold RSSI detection threshold in dBm
     * @return ErrorCode::SUCCESS if initialized, error code otherwise
     */
    [[nodiscard]] ErrorCode initialize(RssiValue detection_threshold) noexcept;

    /**
     * @brief Process RSSI sample
     * @param rssi RSSI value in dBm
     * @param timestamp Sample timestamp
     * @return ErrorCode::SUCCESS if processed, error code otherwise
     * @note Updates internal state and statistics
     */
    [[nodiscard]] ErrorCode process_rssi_sample(
        RssiValue rssi,
        SystemTime timestamp
    ) noexcept;

    /**
     * @brief Detect drone based on RSSI samples
     * @param result Output detection result
     * @return ErrorCode::SUCCESS if detected, error code otherwise
     * @note Uses threshold-based detection with trend analysis
     */
    [[nodiscard]] ErrorCode detect_drone(RSSIDetectionResult& result) noexcept;

    /**
     * @brief Calculate threat level from RSSI
     * @param rssi RSSI value in dBm
     * @param average_rssi Average RSSI over samples
     * @return Threat level classification
     * @note Uses fixed thresholds from constants
     */
    [[nodiscard]] ThreatLevel calculate_threat_level(
        RssiValue rssi,
        RssiValue average_rssi
    ) const noexcept;

    /**
     * @brief Update statistics
     * @param rssi RSSI value to include in statistics
     * @param detected Whether drone was detected
     * @param threat_level Threat level of detection
     * @note Updates min/max RSSI, detection counts, etc.
     */
    void update_statistics(
        RssiValue rssi,
        bool detected,
        ThreatLevel threat_level
    ) noexcept;

    /**
     * @brief Reset RSSI detector state
     * @note Clears history and resets statistics
     */
    void reset() noexcept;

    /**
     * @brief Get current statistics
     * @param stats Output statistics structure
     */
    void get_statistics(RSSIStatistics& stats) const noexcept;

    /**
     * @brief Get movement trend from RSSI history
     * @return Movement trend classification
     * @note Analyzes RSSI history to determine movement
     */
    [[nodiscard]] MovementTrend get_movement_trend() const noexcept;

    /**
     * @brief Get average RSSI from history
     * @return Average RSSI in dBm
     * @note Uses integer arithmetic, no floating-point
     */
    [[nodiscard]] RssiValue get_average_rssi() const noexcept;

    /**
     * @brief Set detection threshold
     * @param threshold RSSI detection threshold in dBm
     * @note Threshold must be between RSSI_MIN_DBM and RSSI_MAX_DBM
     */
    void set_detection_threshold(RssiValue threshold) noexcept;

    /**
     * @brief Get detection threshold
     * @return Current detection threshold in dBm
     */
    [[nodiscard]] RssiValue get_detection_threshold() const noexcept;

private:
    /**
     * @brief Validate RSSI value
     * @param rssi RSSI value to validate
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_rssi(RssiValue rssi) const noexcept;

    /**
     * @brief Add RSSI sample to history
     * @param rssi RSSI value to add
     * @param timestamp Sample timestamp
     * @note Uses circular buffer for history
     */
    void add_to_history(RssiValue rssi, SystemTime timestamp) noexcept;

    /**
     * @brief Calculate movement trend from history
     * @return Movement trend classification
     * @note Compares recent samples to older samples
     */
    [[nodiscard]] MovementTrend calculate_movement_trend() const noexcept;

    /**
     * @brief Check if RSSI is increasing (approaching)
     * @return true if RSSI is increasing
     * @note Uses simple linear regression approximation
     */
    [[nodiscard]] bool is_approaching() const noexcept;

    /**
     * @brief Check if RSSI is decreasing (receding)
     * @return true if RSSI is decreasing
     * @note Uses simple linear regression approximation
     */
    [[nodiscard]] bool is_receding() const noexcept;

    /**
     * @brief Check if RSSI is stable (static)
     * @return true if RSSI is stable
     * @note Uses variance calculation
     */
    [[nodiscard]] bool is_static() const noexcept;

    /**
     * @brief Calculate RSSI variance
     * @return RSSI variance
     * @note Uses integer arithmetic
     */
    [[nodiscard]] uint32_t calculate_variance() const noexcept;

    // Configuration
    RssiValue detection_threshold_;

    // History buffers (fixed-size, no heap allocation)
    std::array<RssiValue, RSSI_HISTORY_SIZE> rssi_history_;
    std::array<SystemTime, TIMESTAMP_HISTORY_SIZE> timestamp_history_;
    size_t history_index_;

    // Statistics
    RSSIStatistics statistics_;

    // State
    bool initialized_;
};

} // namespace drone_analyzer

#endif // RSSI_DETECTOR_HPP
