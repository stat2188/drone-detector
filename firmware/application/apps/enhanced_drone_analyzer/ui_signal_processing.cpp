#include "ui_signal_processing.hpp"
#include <algorithm>
#include <ch.h>
#include <limits>

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// CONSTANTS
// ========================================
constexpr size_t HASH_TABLE_SIZE = DetectionRingBuffer::MAX_ENTRIES;
constexpr FrequencyHash EMPTY_HASH_MARKER = std::numeric_limits<FrequencyHash>::max();
constexpr RSSIValue DEFAULT_RSSI_DBM = EDA::Constants::RSSI_INVALID_DBM;
constexpr Timestamp ZERO_TIMESTAMP = 0;

// ========================================
// HELPER FUNCTIONS
// ========================================
static inline Timestamp get_current_time_ticks() noexcept {
    return static_cast<Timestamp>(chTimeNow());
}

static inline void init_entry(DetectionEntry& entry) noexcept {
    entry.frequency_hash = EMPTY_HASH_MARKER;
    entry.detection_count = 0;
    entry.rssi_value = DEFAULT_RSSI_DBM;
    entry.timestamp = ZERO_TIMESTAMP;
}

// ========================================
// DETECTION RING BUFFER IMPLEMENTATION
// ========================================
void DetectionRingBuffer::update_detection(FrequencyHash frequency_hash, DetectionCount detection_count, RSSIValue rssi_value) noexcept {
    const Timestamp current_time = get_current_time_ticks();
    const EntryIndex start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const EntryIndex idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries_[idx].frequency_hash == frequency_hash) {
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].detection_count = detection_count;
            entries_[idx].timestamp = current_time;
            return;
        }

        if (entries_[idx].frequency_hash == EMPTY_HASH_MARKER) {
            entries_[idx] = {frequency_hash, detection_count, rssi_value, current_time};
            return;
        }
    }

    entries_[head_] = {frequency_hash, detection_count, rssi_value, current_time};
    head_ = (head_ + 1) % MAX_ENTRIES;
}

DetectionCount DetectionRingBuffer::get_detection_count(FrequencyHash frequency_hash) const noexcept {
    const EntryIndex start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const EntryIndex idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].detection_count;
        }

        if (entries_[idx].frequency_hash == EMPTY_HASH_MARKER) {
            return 0;
        }
    }
    return 0;
}

RSSIValue DetectionRingBuffer::get_rssi_value(FrequencyHash frequency_hash) const noexcept {
    const EntryIndex start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const EntryIndex idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].rssi_value;
        }

        if (entries_[idx].frequency_hash == EMPTY_HASH_MARKER) {
            return DEFAULT_RSSI_DBM;
        }
    }
    return DEFAULT_RSSI_DBM;
}

void DetectionRingBuffer::clear() noexcept {
    for (auto& entry : entries_) {
        init_entry(entry);
    }
    head_ = 0;
}

} // namespace ui::apps::enhanced_drone_analyzer
