// ui_signal_processing.hpp - Signal processing utilities for Enhanced Drone Analyzer
// Contains DetectionRingBuffer and other signal processing components
// Based on Looking Glass migration
//
// DIAMOND OPTIMIZATION: WidebandMedianFilter replaced with MedianFilter<int16_t>
// from eda_optimized_utils.hpp to eliminate code duplication
//
// DIAMOND FIX: Added atomic operations to DetectionRingBuffer to fix race conditions

#ifndef UI_SIGNAL_PROCESSING_HPP_
#define UI_SIGNAL_PROCESSING_HPP_

#include <array>
#include <cstdint>
#include <cstddef>
#include <atomic>
#include "ui_drone_common_types.hpp"
#include "eda_optimized_utils.hpp"
#include <ch.h>

namespace ui::apps::enhanced_drone_analyzer {

using FrequencyHash = size_t;
using DetectionCount = uint8_t;
using RssiValue = int32_t;
using Timestamp = uint32_t;

static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
static constexpr FrequencyHash EMPTY_HASH = 0;
static constexpr RssiValue DEFAULT_RSSI = -120;

using WidebandMedianFilter = MedianFilter<int16_t, 11>;

struct DetectionEntry {
    FrequencyHash frequency_hash;
    DetectionCount detection_count;
    RssiValue rssi_value;
    Timestamp timestamp;
};

class DetectionRingBuffer {
public:
    static constexpr size_t MAX_ENTRIES = 32;

    DetectionRingBuffer() noexcept = default;
    ~DetectionRingBuffer() = default;

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

    void update_detection(FrequencyHash frequency_hash, DetectionCount detection_count, RssiValue rssi_value) noexcept;

    DetectionCount get_detection_count(FrequencyHash frequency_hash) const noexcept;
    RssiValue get_rssi_value(FrequencyHash frequency_hash) const noexcept;
    void clear() noexcept;

private:
    struct AtomicDetectionEntry {
        std::atomic<FrequencyHash> frequency_hash{EMPTY_HASH};
        std::atomic<DetectionCount> detection_count{0};
        std::atomic<RssiValue> rssi_value{DEFAULT_RSSI};
        std::atomic<Timestamp> timestamp{0};
    };
    
    std::array<AtomicDetectionEntry, MAX_ENTRIES> entries_{};
    std::atomic<size_t> head_{0};
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SIGNAL_PROCESSING_HPP_
