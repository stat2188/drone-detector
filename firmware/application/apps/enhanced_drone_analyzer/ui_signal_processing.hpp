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

// Signal processing constants
static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;

// DIAMOND OPTIMIZATION: Using unified MedianFilter template
// Replaces old WidebandMedianFilter class
using WidebandMedianFilter = MedianFilter<int16_t, 11>;

// ========================================
// ENTRY STRUCT (POD for memory safety)
// ========================================
struct DetectionEntry {
    size_t frequency_hash;
    uint8_t detection_count;
    int32_t rssi_value;
    uint32_t timestamp;
};

// ========================================
// DETECTION RING BUFFER (Zero-Heap, Lock-Free)
// ========================================
class DetectionRingBuffer {
public:
    static constexpr size_t MAX_ENTRIES = 32;

    DetectionRingBuffer() noexcept = default;
    ~DetectionRingBuffer() = default;

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

    void update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) noexcept;

    uint8_t get_detection_count(size_t frequency_hash) const noexcept;
    int32_t get_rssi_value(size_t frequency_hash) const noexcept;
    void clear() noexcept;

private:
    struct AtomicDetectionEntry {
        std::atomic<size_t> frequency_hash{0};
        std::atomic<uint8_t> detection_count{0};
        std::atomic<int32_t> rssi_value{-120};
        std::atomic<uint32_t> timestamp{0};
    };
    
    std::array<AtomicDetectionEntry, MAX_ENTRIES> entries_;
    std::atomic<size_t> head_{0};
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SIGNAL_PROCESSING_HPP_
