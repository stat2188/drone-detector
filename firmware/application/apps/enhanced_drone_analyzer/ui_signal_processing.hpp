// Signal processing utilities for Enhanced Drone Analyzer
// Zero-heap, RAII, noexcept, thread-safe implementations
// Target: STM32F405 (ARM Cortex-M4)

#ifndef UI_SIGNAL_PROCESSING_HPP_
#define UI_SIGNAL_PROCESSING_HPP_

#include <array>
#include <cstdint>
#include <cstddef>
#include <limits>
#include "ui_drone_common_types.hpp"
#include "eda_optimized_utils.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// TYPE ALIASES
// DIAMOND FIX: Changed FrequencyHash from size_t to uint64_t
// This prevents FNV constant truncation warnings (64-bit constants fit without overflow)
// On ARM Cortex-M4, this uses 64-bit operations (2 instructions per operation)
// which is acceptable for hash operations that are not in the DSP loop.
using FrequencyHash = uint64_t;
using DetectionCount = uint8_t;
using RSSIValue = int32_t;
using Timestamp = uint32_t;
using EntryIndex = size_t;

// CONSTANTS
namespace DetectionBufferConstants {
    // / @brief Maximum number of entries in detection buffer (power of 2 for efficient masking)
    constexpr size_t MAX_ENTRIES = 16;

    // / @brief Hash table size (must match MAX_ENTRIES to prevent buffer overflow)
    constexpr size_t HASH_TABLE_SIZE = MAX_ENTRIES;

    // / @brief Bitmask for power-of-2 modulo operation
    constexpr size_t HASH_MASK = HASH_TABLE_SIZE - 1;

    // / @brief Default RSSI value when no signal is detected
    constexpr RSSIValue DEFAULT_RSSI_DBM = EDA::Constants::RSSI_INVALID_DBM;

    // / @brief Marker for empty hash table slots
    constexpr FrequencyHash EMPTY_HASH_MARKER = std::numeric_limits<FrequencyHash>::max();

    // / @brief Zero timestamp value
    constexpr Timestamp ZERO_TIMESTAMP = 0;

    // / @brief Threshold for detecting timestamp wrap-around (half of max value)
    constexpr Timestamp WRAP_THRESHOLD = 0xFFFFFFFFUL / 2;

    // / @brief FNV-1a hash function offset basis (64-bit)
    constexpr FrequencyHash FNV_OFFSET_BASIS = 14695981039346656037ULL;

    // / @brief FNV-1a hash function prime (64-bit)
    constexpr FrequencyHash FNV_PRIME = 1099511628211ULL;
}

// GLOBAL CONSTANTS
// / @brief Hysteresis margin for RSSI comparisons (dB)
static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;

// / @brief Default RSSI value when no signal is detected (invalid dBm marker)
static constexpr RSSIValue DEFAULT_RSSI_DBM = EDA::Constants::RSSI_INVALID_DBM;

// DIAMOND OPTIMIZATION: Unified MedianFilter
// / @brief Median filter for wideband RSSI values
// / @note Using unified MedianFilter template to eliminate code duplication
using WidebandMedianFilter = MedianFilter<int16_t, 11>;

// ENTRY STRUCT (Atomic-Safe POD)
// * * @brief Detection entry in ring buffer * * CRITICAL FIX: Aligned to 4 bytes to ensure atomic 32-bit reads on ARM Cortex-M4. * CRITICAL FIX: Version field added to detect concurrent updates (not torn reads on 32-bit ARM). * * DIAMOND FIX: FrequencyHash changed from size_t to uint64_t to prevent FNV constant truncation. * This increased struct size from 24 to 28 bytes. The 4-byte increase per entry is acceptable * for the embedded system (64 bytes total for 16 entries) and provides better hash quality. * * On 32-bit ARM Cortex-M4: * - uint64_t is 8 bytes (requires 2 instructions for atomic access) * - Version field detects concurrent updates during read * * On 64-bit ARM: * - uint64_t is 8 bytes (can be torn) * - Version field detects torn reads * * @note Memory layout with #pragma pack(push,4): * - version: 4 bytes (offset 0-3) * - frequency_hash: 8 bytes (offset 4-11) * - detection_count: 1 byte (offset 12) * - _padding1: 1 byte (offset 13) * - rssi_value: 4 bytes (offset 14-17) * - timestamp: 4 bytes (offset 18-21) * - _padding2: 2 bytes (offset 22-23) * Total: 28 bytes (4-byte aligned)
#pragma pack(push, 4)
struct DetectionEntry {
    uint32_t version;              ///< Sequence counter for concurrent update detection
    FrequencyHash frequency_hash;  ///< Hash of frequency (8 bytes, may be torn on 64-bit)
    DetectionCount detection_count; ///< Number of detections
    uint8_t _padding1;            ///< Padding for alignment
    RSSIValue rssi_value;         ///< RSSI value in dBm
    Timestamp timestamp;            ///< Timestamp of last detection
    uint16_t _padding2;           ///< Padding for 4-byte alignment
};
#pragma pack(pop)

static_assert(sizeof(DetectionEntry) == 28, "DetectionEntry must be 28 bytes");
static_assert(alignof(DetectionEntry) == 4, "DetectionEntry must be 4-byte aligned");

// FNV-1a HASH FUNCTION
// * * @brief FNV-1a hash function for frequency hashing * * Better hash function to reduce collisions compared to simple bitmask. * Replaces simple bitmask hash from original implementation. * * @note FNV-1a: hash ^= byte; hash *= prime; * @note ~20-30 cycles for 64-bit value on ARM Cortex-M4
struct FrequencyHasher {
    // / @brief Compute FNV-1a hash of frequency
    // / @param frequency Frequency value to hash
    // / @return Hash value
    static constexpr FrequencyHash hash(FrequencyHash frequency) noexcept {
        FrequencyHash hash = DetectionBufferConstants::FNV_OFFSET_BASIS;
        const uint8_t* data = reinterpret_cast<const uint8_t*>(&frequency);
        for (size_t i = 0; i < sizeof(FrequencyHash); ++i) {
            hash ^= data[i];
            hash *= DetectionBufferConstants::FNV_PRIME;
        }
        return hash;
    }
};

// DETECTION RING BUFFER (Thread-Safe)
// * * @brief Thread-safe ring buffer for detection entries * * SYNCHRONIZATION STRATEGY: * - Writer methods (update_detection, clear): Mutex-protected * - Reader methods (get_detection_count, get_rssi_value): Lock-free with atomic head_ * - Memory ordering: Acquire/release for atomic operations * - Version field: Detects concurrent updates during reads * * THREADING MODEL: * - Writer thread: DroneScanner::scan_thread (baseband/M0 context) * - Reader thread: UI thread (main application context) * * RATIONALE FOR HYBRID LOCKING: * 1. Writer methods use mutex for consistency during multi-writer scenarios * 2. Reader methods are lock-free for performance (UI updates are frequent at 60 Hz) * 3. Atomic head_ ensures readers see consistent buffer state * 4. Worst case: reader sees slightly stale data (acceptable for UI) * * CRITICAL: DO NOT call from ISR context (mutex not ISR-safe) * * ALLOCATION REQUIREMENT: * DetectionRingBuffer MUST be allocated as a member variable (BSS segment). * DO NOT allocate on stack - this causes stack overflow (480 bytes). * * @note Memory usage: ~480 bytes (entries_[]: 448 bytes + head_: 4 bytes + * global_version_: 4 bytes + buffer_mutex_: ~24 bytes) * * DIAMOND FIX: Updated memory usage to reflect DetectionEntry size increase from 24 to 28 bytes * due to FrequencyHash change from size_t to uint64_t (prevents FNV constant truncation).
class DetectionRingBuffer {
public:
    // DIAMOND FIX: Explicit constructor with member initialization
    // Eliminates compiler warnings about uninitialized members
    // Initializes all entries to empty state on construction
    DetectionRingBuffer() noexcept
        : head_{0},
          global_version_{0},
          buffer_mutex_{},
          recursion_depth_{0} {
        for (auto& entry : entries_) {
            init_entry(entry, 0);
        }
    }
    ~DetectionRingBuffer() = default;

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

    // / @brief Update detection entry (mutex-protected writer)
    // / @param frequency_hash Hash of frequency to update
    // / @param detection_count New detection count
    // / @param rssi_value New RSSI value
    // / @note Acquires buffer_mutex_ for thread safety
    // / @note Includes recursion detection to prevent deadlock
    // / @warning DO NOT call from ISR context (mutex not ISR-safe)
    void update_detection(FrequencyHash frequency_hash,
                        DetectionCount detection_count,
                        RSSIValue rssi_value) noexcept;

    // / @brief Get detection count (lock-free reader with versioning)
    // / @param frequency_hash Hash of frequency to query
    // / @return Detection count (0 if not found or concurrent update detected)
    // / @note Uses atomic head_ and versioning for consistency
    // / @note May return stale data (acceptable for UI)
    DetectionCount get_detection_count(FrequencyHash frequency_hash) const noexcept;

    // / @brief Get RSSI value (lock-free reader with versioning)
    // / @param frequency_hash Hash of frequency to query
    // / @return RSSI value (DEFAULT_RSSI_DBM if not found or concurrent update detected)
    // / @note Uses atomic head_ and versioning for consistency
    // / @note May return stale data (acceptable for UI)
    RSSIValue get_rssi_value(FrequencyHash frequency_hash) const noexcept;

    // / @brief Clear all entries (mutex-protected)
    // / @note Acquires buffer_mutex_ for thread safety
    // / @warning DO NOT call from ISR context (mutex not ISR-safe)
    void clear() noexcept;

private:
    std::array<DetectionEntry, DetectionBufferConstants::MAX_ENTRIES> entries_{};
    size_t head_;                         ///< Head index for ring buffer eviction
    uint32_t global_version_;             ///< Global version counter for concurrent update detection
    mutable Mutex buffer_mutex_;           ///< Mutex for writer synchronization
    int recursion_depth_;                 ///< Recursion depth counter for deadlock prevention

    // / @brief Compute hash index from frequency hash
    // / @param hash Frequency hash to index
    // / @return Hash table index (0 to HASH_TABLE_SIZE - 1)
    size_t hash_index(FrequencyHash hash) const noexcept {
        return FrequencyHasher::hash(hash) & DetectionBufferConstants::HASH_MASK;
    }

    // / @brief Initialize entry to empty state
    // / @param entry Entry to initialize
    // / @param version Version number to assign
    void init_entry(DetectionEntry& entry, uint32_t version) noexcept {
        entry.version = version;
        entry.frequency_hash = DetectionBufferConstants::EMPTY_HASH_MARKER;
        entry.detection_count = 0;
        entry.rssi_value = DetectionBufferConstants::DEFAULT_RSSI_DBM;
        entry.timestamp = DetectionBufferConstants::ZERO_TIMESTAMP;
    }

    // / @brief Check if entry is consistent (no concurrent update during read)
    // / @param entry Entry to check
    // / @return true if entry is consistent, false otherwise
    bool is_entry_consistent(const DetectionEntry& entry) const noexcept {
        const uint32_t version1 = entry.version;
        const uint32_t version2 = entry.version;
        return version1 == version2;
    }
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SIGNAL_PROCESSING_HPP_
