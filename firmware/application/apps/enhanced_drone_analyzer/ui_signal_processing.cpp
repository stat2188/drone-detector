#include "ui_signal_processing.hpp"
#include <algorithm>

namespace ui::apps::enhanced_drone_analyzer {

// Global Definitions removed - DetectionRingBuffer now lives inside DroneScanner instances

// WidebandMedianFilter implementations
void WidebandMedianFilter::add_sample(int16_t rssi) {
    window_[head_] = rssi;
    head_ = (head_ + 1) % WINDOW_SIZE;
    if (head_ == 0) full_ = true;
}

int16_t WidebandMedianFilter::get_median_threshold() const {
    if (!full_) return DEFAULT_RSSI_THRESHOLD_DB; 

    // 🔴 OPTIMIZED: Use std::nth_element for O(n) median calculation
    // This is much more efficient than the previous O(n²) partial selection sort
    auto temp = window_;
    size_t k = WINDOW_SIZE / 2;

    // std::nth_element rearranges elements so that the element at position k is the k-th smallest
    // This is O(n) average complexity instead of O(n²)
    std::nth_element(temp.begin(), temp.begin() + k, temp.end());

    return temp[k] - HYSTERESIS_MARGIN_DB;
}

void WidebandMedianFilter::reset() {
    full_ = false;
    head_ = 0;
    window_ = {};
}

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
DetectionRingBuffer::DetectionRingBuffer() : entries_{}, head_(0) {
    clear();
}

void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    systime_t current_time = chTimeNow();
    size_t start_idx = frequency_hash % MAX_ENTRIES;

    // Linear probing to find existing entry or empty slot
    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        size_t idx = (start_idx + probe) % MAX_ENTRIES;

        // Found existing entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].detection_count = detection_count;
            entries_[idx].last_update = current_time;
            return;
        }

        // Found empty slot
        if (entries_[idx].frequency_hash == 0) {
            entries_[idx] = {frequency_hash, detection_count, rssi_value, current_time};
            return;
        }
    }

    // Table full - replace oldest entry (circular buffer behavior)
    entries_[head_] = {frequency_hash, detection_count, rssi_value, current_time};
    head_ = (head_ + 1) % MAX_ENTRIES;
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
    size_t start_idx = frequency_hash % MAX_ENTRIES;

    // Linear probing - O(1) average case
    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        size_t idx = (start_idx + probe) % MAX_ENTRIES;

        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].detection_count;
        }

        // Empty slot - entry not found
        if (entries_[idx].frequency_hash == 0) {
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

        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].rssi_value;
        }

        // Empty slot - entry not found
        if (entries_[idx].frequency_hash == 0) {
            return -120;
        }
    }
    return -120;
}

void DetectionRingBuffer::clear() {
    for (auto& entry : entries_) {
        entry = DetectionEntry{0, 0, -120, 0};
    }
    head_ = 0;
}

} // namespace ui::apps::enhanced_drone_analyzer
