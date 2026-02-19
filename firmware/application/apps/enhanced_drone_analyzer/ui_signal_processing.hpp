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
// STAGE 4 DIAMOND FIX: Thread-safe with atomic head and mutex
// ============================================================
// This ring buffer is designed for multi-threaded access pattern.
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
//   - Mutex-protected writer methods (update_detection, clear)
//   - Lock-free reader methods using atomic head_ index
//   - Memory ordering: Acquire/release for atomic operations
//
// RATIONALE FOR HYBRID LOCKING:
//   1. Writer methods use mutex for consistency during multi-writer scenarios
//   2. Reader methods are lock-free for performance (UI updates are frequent)
//   3. Atomic head_ ensures readers see consistent buffer state
//   4. Worst case: reader sees slightly stale data (acceptable for UI)
//
// STAGE 4 FIXES:
//   - Added volatile size_t head_ for head index (protected by mutex)
//   - Added mutable Mutex buffer_mutex_ for thread-safe writer access
//   - Implemented mutex-protected writer methods
//   - Implemented lock-free reader methods
// ========================================
class DetectionRingBuffer {
public:
    // Phase 3 Optimization: Reduced from 32 to 16 entries (~256 bytes savings)
    static constexpr size_t MAX_ENTRIES = 16;

    DetectionRingBuffer() noexcept = default;
    ~DetectionRingBuffer() = default;

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

    /// @brief Update detection entry (mutex-protected)
    /// @param frequency_hash Hash of frequency to update
    /// @param detection_count New detection count
    /// @param rssi_value New RSSI value
    /// @note Acquires buffer_mutex_ for thread safety
    void update_detection(FrequencyHash frequency_hash, DetectionCount detection_count, RSSIValue rssi_value) noexcept;

    /// @brief Get detection count (lock-free reader)
    /// @param frequency_hash Hash of frequency to query
    /// @return Detection count (0 if not found)
    /// @note Uses atomic head_ for lock-free access
    DetectionCount get_detection_count(FrequencyHash frequency_hash) const noexcept;

    /// @brief Get RSSI value (lock-free reader)
    /// @param frequency_hash Hash of frequency to query
    /// @return RSSI value (DEFAULT_RSSI_DBM if not found)
    /// @note Uses volatile head_ with mutex protection for access
    RSSIValue get_rssi_value(FrequencyHash frequency_hash) const noexcept;

    /// @brief Clear all entries (mutex-protected)
    /// @note Acquires buffer_mutex_ for thread safety
    void clear() noexcept;

private:
    std::array<DetectionEntry, MAX_ENTRIES> entries_{};
    volatile size_t head_{0};  // STAGE 4 FIX: Volatile head index (protected by mutex)
    mutable Mutex buffer_mutex_;      // STAGE 4 FIX: Mutex for thread-safe writer access
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SIGNAL_PROCESSING_HPP_
