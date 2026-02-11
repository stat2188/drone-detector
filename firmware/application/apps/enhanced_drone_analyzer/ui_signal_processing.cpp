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
        
        size_t entry_hash = entries_[idx].frequency_hash.load(std::memory_order_acquire);
        
        if (entry_hash == frequency_hash) {
            entries_[idx].rssi_value.store(rssi_value, std::memory_order_release);
            entries_[idx].detection_count.store(detection_count, std::memory_order_release);
            entries_[idx].timestamp.store(current_time, std::memory_order_release);
            return;
        }
        
        if (entry_hash == 0) {
            size_t expected = 0;
            if (entries_[idx].frequency_hash.compare_exchange_strong(
                    expected, frequency_hash,
                    std::memory_order_acq_rel,
                    std::memory_order_acquire)) {
                entries_[idx].detection_count.store(detection_count, std::memory_order_release);
                entries_[idx].rssi_value.store(rssi_value, std::memory_order_release);
                entries_[idx].timestamp.store(current_time, std::memory_order_release);
                head_.store(idx, std::memory_order_release);
                return;
            }
        }
    }
    
    size_t old_head = head_.load(std::memory_order_acquire);
    size_t new_head = (old_head + 1) % MAX_ENTRIES;
    while (!head_.compare_exchange_weak(old_head, new_head,
                                     std::memory_order_acq_rel,
                                     std::memory_order_acquire)) {
        new_head = (old_head + 1) % MAX_ENTRIES;
    }
    
    entries_[new_head].frequency_hash.store(frequency_hash, std::memory_order_release);
    entries_[new_head].detection_count.store(detection_count, std::memory_order_release);
    entries_[new_head].rssi_value.store(rssi_value, std::memory_order_release);
    entries_[new_head].timestamp.store(current_time, std::memory_order_release);
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const noexcept {
    const size_t start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const size_t idx = (start_idx + probe) % MAX_ENTRIES;
        
        size_t entry_hash = entries_[idx].frequency_hash.load(std::memory_order_acquire);
        
        if (entry_hash == frequency_hash) {
            return entries_[idx].detection_count.load(std::memory_order_acquire);
        }
        
        if (entry_hash == 0) {
            return 0;
        }
    }
    return 0;
}

int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const noexcept {
    const size_t start_idx = frequency_hash % MAX_ENTRIES;

    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        const size_t idx = (start_idx + probe) % MAX_ENTRIES;
        
        size_t entry_hash = entries_[idx].frequency_hash.load(std::memory_order_acquire);
        
        if (entry_hash == frequency_hash) {
            return entries_[idx].rssi_value.load(std::memory_order_acquire);
        }
        
        if (entry_hash == 0) {
            return -120;
        }
    }
    return -120;
}

void DetectionRingBuffer::clear() noexcept {
    for (auto& entry : entries_) {
        entry.frequency_hash.store(0, std::memory_order_release);
        entry.detection_count.store(0, std::memory_order_release);
        entry.rssi_value.store(-120, std::memory_order_release);
        entry.timestamp.store(0, std::memory_order_release);
    }
    head_.store(0, std::memory_order_release);
}

} // namespace ui::apps::enhanced_drone_analyzer
