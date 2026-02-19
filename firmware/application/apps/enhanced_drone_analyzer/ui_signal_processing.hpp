// ui_signal_processing.hpp - Signal processing utilities for Enhanced Drone Analyzer
// Contains DetectionRingBuffer and other signal processing components
// Based on Looking Glass migration
//
// DIAMOND OPTIMIZATION: WidebandMedianFilter replaced with MedianFilter<int16_t>
// from eda_optimized_utils.hpp to eliminate code duplication

#ifndef UI_SIGNAL_PROCESSING_HPP_
#define UI_SIGNAL_PROCESSING_HPP_

#include <array>
#include <cstdint>
#include <cstddef>
#include "ui_drone_common_types.hpp"
#include "eda_optimized_utils.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// TYPE ALIASES (Semantic Types)
// ========================================
using FrequencyHash = size_t;
using DetectionCount = uint8_t;
using RSSIValue = int32_t;
using Timestamp = uint32_t;
using EntryIndex = size_t;

// ========================================
// CONSTANTS
// ========================================
static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;

/// Default RSSI value when no signal is detected (invalid dBm marker)
/// @note Uses internal linkage (static constexpr) to avoid ODR violations
static constexpr RSSIValue DEFAULT_RSSI_DBM = EDA::Constants::RSSI_INVALID_DBM;

// DIAMOND OPTIMIZATION: Using unified MedianFilter template
// Replaces old WidebandMedianFilter class
using WidebandMedianFilter = MedianFilter<int16_t, 11>;

// ========================================
// ENTRY STRUCT (POD for memory safety)
// ========================================
struct DetectionEntry {
    FrequencyHash frequency_hash;
    DetectionCount detection_count;
    RSSIValue rssi_value;
    Timestamp timestamp;
};

// ========================================
// DETECTION RING BUFFER (Zero-Heap)
// ========================================
// THREAD SAFETY DOCUMENTATION:
// ============================
// This ring buffer is designed for single-writer, single-reader access pattern.
//
// WRITER THREAD: DroneScanner::scan_thread (baseband/M0 context)
//   - Calls update_detection() to record new signal detections
//   - Called from perform_database_scan_cycle() and perform_wideband_scan_cycle()
//
// READER THREAD: UI thread (main application context)
//   - Calls get_detection_count() and get_rssi_value() for display updates
//   - Called from DroneDisplayController update methods
//
// SYNCHRONIZATION STRATEGY:
//   - NO mutex protection required due to single-writer/single-reader pattern
//   - Uses atomic head_ index updates for lock-free operation
//   - Memory ordering: Relaxed semantics sufficient for this use case
//     (detection data is eventually consistent, not safety-critical)
//
// RATIONALE FOR LOCK-FREE DESIGN:
//   1. Cortex-M4 has limited RAM - mutex overhead would add ~40 bytes
//   2. Detection data is advisory (signal strength), not safety-critical
//   3. Worst case: reader sees slightly stale data (acceptable for UI)
//   4. ChibiOS RTOS guarantees atomic 32-bit reads/writes on Cortex-M4
//
// WARNING: If multi-writer access is needed in future, add MutexLock protection
// to update_detection() method. Read methods remain lock-free.
// ========================================
class DetectionRingBuffer {
public:
    // Phase 3 Optimization: Reduced from 32 to 16 entries (~256 bytes savings)
    static constexpr size_t MAX_ENTRIES = 16;

    DetectionRingBuffer() noexcept = default;
    ~DetectionRingBuffer() = default;

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

    void update_detection(FrequencyHash frequency_hash, DetectionCount detection_count, RSSIValue rssi_value) noexcept;

    DetectionCount get_detection_count(FrequencyHash frequency_hash) const noexcept;
    RSSIValue get_rssi_value(FrequencyHash frequency_hash) const noexcept;
    void clear() noexcept;

private:
    std::array<DetectionEntry, MAX_ENTRIES> entries_{};
    size_t head_ = 0;
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SIGNAL_PROCESSING_HPP_
