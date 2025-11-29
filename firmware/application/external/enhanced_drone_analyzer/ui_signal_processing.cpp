#include "ui_signal_processing.hpp"

namespace ui::external_app::enhanced_drone_analyzer {

// Global Definitions
DetectionRingBuffer global_detection_ring;
DetectionRingBuffer& local_detection_ring = global_detection_ring;

// WidebandMedianFilter implementations
void WidebandMedianFilter::add_sample(int16_t rssi) {
    window_[head_] = rssi;
    head_ = (head_ + 1) % WINDOW_SIZE;
    if (head_ == 0) full_ = true;
}

int16_t WidebandMedianFilter::get_median_threshold() const {
    if (!full_) return DEFAULT_RSSI_THRESHOLD_DB;

    auto temp = window_;
    for (size_t i = 0; i < WINDOW_SIZE / 2 + 1; ++i) {
        for (size_t j = 0; j < WINDOW_SIZE - 1; ++j) {
            if (temp[j] > temp[j + 1]) std::swap(temp[j], temp[j + 1]);
        }
    }
    return temp[WINDOW_SIZE / 2] - HYSTERESIS_MARGIN_DB;
}

void WidebandMedianFilter::reset() {
    full_ = false;
    head_ = 0;
    window_ = {};
}

// DetectionRingBuffer implementations
DetectionRingBuffer::DetectionRingBuffer() : entries_{}, head_(0) {
    clear();
}

void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    systime_t current_time = chTimeNow();
    for (auto& entry : entries_) {
        if (entry.frequency_hash == frequency_hash) {
            entry.rssi_value = rssi_value;
            entry.detection_count = detection_count;
            entry.last_update = current_time;
            return;
        }
    }
    entries_[head_] = {frequency_hash, detection_count, rssi_value, current_time};
    head_ = (head_ + 1) % MAX_ENTRIES;
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
    for (const auto& entry : entries_) {
        if (entry.frequency_hash == frequency_hash) {
            return entry.detection_count;
        }
    }
    return 0;
}

int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const {
    for (const auto& entry : entries_) {
        if (entry.frequency_hash == frequency_hash) {
            return entry.rssi_value;
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

} // namespace ui::external_app::enhanced_drone_analyzer
