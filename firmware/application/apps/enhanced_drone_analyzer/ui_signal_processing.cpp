#include "ui_signal_processing.hpp"
#include <algorithm>
#include <ch.h>

namespace ui::apps::enhanced_drone_analyzer {

constexpr size_t HASH_TABLE_SIZE = DetectionRingBuffer::MAX_ENTRIES;

static inline uint32_t get_current_time_ticks() noexcept {
    return static_cast<uint32_t>(chTimeNow());
}

void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) noexcept {
    const uint32_t current_time = get_current_time_ticks();
    const size_t start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const size_t idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries_[idx].frequency_hash == frequency_hash) {
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].detection_count = detection_count;
            entries_[idx].timestamp = current_time;
            return;
        }

        if (entries_[idx].frequency_hash == 0) {
            entries_[idx] = {frequency_hash, detection_count, rssi_value, current_time};
            return;
        }
    }

    entries_[head_] = {frequency_hash, detection_count, rssi_value, current_time};
    head_ = (head_ + 1) % MAX_ENTRIES;
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const noexcept {
    const size_t start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const size_t idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].detection_count;
        }

        if (entries_[idx].frequency_hash == 0) {
            return 0;
        }
    }
    return 0;
}

int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const noexcept {
    const size_t start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const size_t idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].rssi_value;
        }

        if (entries_[idx].frequency_hash == 0) {
            return -120;
        }
    }
    return -120;
}

void DetectionRingBuffer::clear() noexcept {
    constexpr DetectionEntry empty_entry{0, 0, -120, 0};
    for (auto& entry : entries_) {
        entry = empty_entry;
    }
    head_ = 0;
}

} // namespace ui::apps::enhanced_drone_analyzer
