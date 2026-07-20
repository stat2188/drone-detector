#ifndef ADAPTIVE_THRESHOLD_HPP
#define ADAPTIVE_THRESHOLD_HPP

#include <cstdint>
#include <cstddef>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Self-tuning CFAR threshold based on observed false alarm rate.
 *
 * Maintains a rolling window of detection results and adapts the threshold
 * to maintain a target false alarm rate. When noise increases (urban, WiFi
 * dense), threshold rises automatically. When noise decreases (rural),
 * threshold drops for maximum sensitivity.
 *
 * Algorithm: Compute false alarm rate from detections where RSSI was below
 * noise_floor + margin (likely noise, not real signal). If rate exceeds
 * target, raise threshold. If rate drops below half-target, lower threshold.
 *
 * SRAM: ~40 bytes (rolling window + state)
 * Stack: ~4 bytes per call
 * Flash: ~120 bytes code
 */
class AdaptiveThreshold {
public:
    /**
     * @brief Update threshold based on latest detection result.
     * @param detected Whether signal was detected this frame
     * @param rssi RSSI of detection (dBm), used to estimate if it's noise
     * @param noise_floor Current noise floor estimate (25th percentile)
     * @param current_threshold_x10 Current CFAR threshold × 10
     */
    void update(
        bool detected,
        int32_t rssi,
        uint8_t noise_floor,
        uint8_t current_threshold_x10
    ) noexcept {
        // Record whether this was a likely false alarm:
        // detected + RSSI close to noise floor = probably noise
        const bool likely_false_alarm = detected && (rssi < static_cast<int32_t>(noise_floor) + 20);

        history_[history_index_] = likely_false_alarm ? 1 : 0;
        history_index_ = (history_index_ + 1) % HISTORY_SIZE;
        if (history_count_ < HISTORY_SIZE) history_count_++;

        // Only adapt after filling the history window
        if (history_count_ < HISTORY_SIZE) return;

        // Compute false alarm rate
        uint32_t fa_count = 0;
        for (size_t i = 0; i < HISTORY_SIZE; ++i) {
            fa_count += history_[i];
        }

        const uint32_t fa_rate_percent = (fa_count * 100) / HISTORY_SIZE;

        // Adapt threshold
        if (fa_rate_percent > false_alarm_target_) {
            // Too many false alarms — raise threshold (more selective)
            if (current_threshold_x10_ < CFAR_THRESHOLD_MAX_X10) {
                current_threshold_x10_++;
            }
        } else if (fa_rate_percent < (false_alarm_target_ / 2)) {
            // Very few false alarms — lower threshold (more sensitive)
            if (current_threshold_x10_ > CFAR_THRESHOLD_MIN_X10) {
                current_threshold_x10_--;
            }
        }

        // Apply smoothing: blend with current config value
        // This prevents the adaptive threshold from diverging too far
        // from the user-configured value.
        if (current_threshold_x10 > current_threshold_x10_ + 20) {
            current_threshold_x10_ = current_threshold_x10 - 20;
        } else if (current_threshold_x10 < current_threshold_x10_ - 20) {
            current_threshold_x10_ = current_threshold_x10 + 20;
        }
    }

    /**
     * @brief Get the adapted threshold value.
     * @return Threshold × 10 (e.g., 60 = 6.0 units ≈ 1.2 dB)
     */
    [[nodiscard]] uint8_t get_optimal_threshold() const noexcept {
        return current_threshold_x10_;
    }

    /**
     * @brief Reset to initial state (call on frequency change).
     */
    void reset() noexcept {
        history_.fill(0);
        history_index_ = 0;
        history_count_ = 0;
        current_threshold_x10_ = DEFAULT_CFAR_THRESHOLD_X10;
    }

    /**
     * @brief Set target false alarm rate (percent, 1-20).
     */
    void set_target_rate(uint8_t target_percent) noexcept {
        if (target_percent < 1) target_percent = 1;
        if (target_percent > 20) target_percent = 20;
        false_alarm_target_ = target_percent;
    }

private:
    static constexpr size_t HISTORY_SIZE = 32;
    std::array<uint8_t, HISTORY_SIZE> history_{};
    uint8_t history_index_{0};
    uint8_t history_count_{0};
    uint8_t current_threshold_x10_{DEFAULT_CFAR_THRESHOLD_X10};
    uint8_t false_alarm_target_{5};  // Target: ≤5% false alarm rate
};

} // namespace drone_analyzer

#endif // ADAPTIVE_THRESHOLD_HPP
