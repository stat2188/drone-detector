#include "ui_signal_processing.hpp"
#include <algorithm>

namespace ui::apps::enhanced_drone_analyzer {

// FIX: Use simple hash-based lookup for O(1) instead of O(n) std::find_if
// For MAX_ENTRIES = 32, we use direct index = hash % MAX_ENTRIES
// with linear probing for collision resolution
constexpr size_t HASH_TABLE_SIZE = 32;

class HashIndexCache {
public:
    size_t get_index(size_t frequency_hash) const {
        return frequency_hash % HASH_TABLE_SIZE;
    }
};

// DetectionRingBuffer implementations
DetectionRingBuffer::DetectionRingBuffer()
    : entries_ptr_(std::make_unique<std::array<DetectionEntry, MAX_ENTRIES>>()),
      head_(0) {
    clear();
}

void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    systime_t current_time = chTimeNow();
    size_t start_idx = frequency_hash % MAX_ENTRIES;

    // Linear probing to find existing entry or empty slot
    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        size_t idx = (start_idx + probe) % MAX_ENTRIES;

        // Found existing entry
        if (entries()[idx].frequency_hash == frequency_hash) {
            entries()[idx].rssi_value = rssi_value;
            entries()[idx].detection_count = detection_count;
            entries()[idx].last_update = current_time;
            return;
        }

        // Found empty slot
        if (entries()[idx].frequency_hash == 0) {
            entries()[idx] = {frequency_hash, detection_count, rssi_value, current_time};
            return;
        }
    }

    // Table full - replace oldest entry (circular buffer behavior)
    entries()[head_] = {frequency_hash, detection_count, rssi_value, current_time};
    head_ = (head_ + 1) % MAX_ENTRIES;
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
    size_t start_idx = frequency_hash % MAX_ENTRIES;

    // Linear probing - O(1) average case
    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        size_t idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries()[idx].frequency_hash == frequency_hash) {
            return entries()[idx].detection_count;
        }

        // Empty slot - entry not found
        if (entries()[idx].frequency_hash == 0) {
            return 0;
        }
    }
    return 0;
}

int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const {
    size_t start_idx = frequency_hash % MAX_ENTRIES;

    // Linear probing - O(1) average case
    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        size_t idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries()[idx].frequency_hash == frequency_hash) {
            return entries()[idx].rssi_value;
        }

        // Empty slot - entry not found
        if (entries()[idx].frequency_hash == 0) {
            return -120;
        }
    }
    return -120;
}

void DetectionRingBuffer::clear() {
    for (auto& entry : entries()) {
        entry = DetectionEntry{0, 0, -120, 0};
    }
    head_ = 0;
}

} // namespace ui::apps::enhanced_drone_analyzer
