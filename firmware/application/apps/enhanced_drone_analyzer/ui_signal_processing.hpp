// ui_signal_processing.hpp - Signal processing utilities for Enhanced Drone Analyzer
// Contains DetectionRingBuffer, WidebandMedianFilter, and other signal processing components
// Based on Looking Glass migration

#ifndef UI_SIGNAL_PROCESSING_HPP_
#define UI_SIGNAL_PROCESSING_HPP_

#include <array>
#include <cstdint>
#include <ch.h>  // for time_t
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Signal processing constants
static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;

// WidebandMedianFilter for noise reduction
class WidebandMedianFilter {
private:
    static constexpr size_t WINDOW_SIZE = 11;
    std::array<int16_t, WINDOW_SIZE> window_{};
    size_t head_ = 0;
    bool full_ = false;

public:
    void add_sample(int16_t rssi);
    int16_t get_median_threshold() const;
    void reset();
};

struct DetectionEntry {
    size_t frequency_hash;
    uint8_t detection_count;
    int32_t rssi_value;
    time_t last_update;
};

class DetectionRingBuffer {
public:
    static constexpr size_t MAX_ENTRIES = 32;

    DetectionRingBuffer();
    ~DetectionRingBuffer() = default;

    void update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value);

    /**
     * @brief Get detection count for a frequency hash
     * @param frequency_hash Hash of frequency to look up
     * @return Detection count (0 if not found)
     *
     * @note Time complexity: O(1) average case, O(MAX_ENTRIES) worst case
     *       Linear probing hash table with 32 entries. Average probes: 1-2,
     *       worst case: 32 probes when all entries collide.
     */
    uint8_t get_detection_count(size_t frequency_hash) const;

    int32_t get_rssi_value(size_t frequency_hash) const;
    void clear();

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

private:
    std::array<DetectionEntry, MAX_ENTRIES> entries_{};
    size_t head_ = 0;
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SIGNAL_PROCESSING_HPP_
