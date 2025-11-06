// ui_signal_processing.hpp - Signal processing utilities for Enhanced Drone Analyzer
// Contains DetectionRingBuffer, WidebandMedianFilter, and other signal processing components
// Based on Looking Glass migration

#ifndef __UI_SIGNAL_PROCESSING_HPP__
#define __UI_SIGNAL_PROCESSING_HPP__

#include <array>
#include <cstdint>
#include <ch.h>  // for systime_t
#include "ui_drone_common_types.hpp"

// Signal processing constants
static constexpr int16_t HYSTERESIS_MARGIN_DB = 5;



// WidebandMedianFilter for noise reduction
class WidebandMedianFilter {
private:
    static constexpr size_t WINDOW_SIZE = 11;
    std::array<int16_t, WINDOW_SIZE> window_{};
    size_t head_ = 0;
    bool full_ = false;

public:
    void add_sample(int16_t rssi) {
        window_[head_] = rssi;
        head_ = (head_ + 1) % WINDOW_SIZE;
        if (head_ == 0) full_ = true;
    }

    int16_t get_median_threshold() const {
        if (!full_) return DEFAULT_RSSI_THRESHOLD_DB;

        auto temp = window_;
        for (size_t i = 0; i < WINDOW_SIZE / 2 + 1; ++i) {
            for (size_t j = 0; j < WINDOW_SIZE - 1; ++j) {
                if (temp[j] > temp[j + 1]) std::swap(temp[j], temp[j + 1]);
            }
        }
        return temp[WINDOW_SIZE / 2] - HYSTERESIS_MARGIN_DB;
    }

    void reset() { full_ = false; head_ = 0; window_ = {}; }
};

struct DetectionEntry {
    size_t frequency_hash;
    uint8_t detection_count;
    int32_t rssi_value;
    systime_t last_update;
};

class DetectionRingBuffer {
public:
    static constexpr size_t MAX_ENTRIES = 32;

    DetectionRingBuffer() = default;
    ~DetectionRingBuffer() = default;

    // Replaces dynamic std::deque with fixed-size array for embedded systems
    void update_detection(size_t frequency_hash, int32_t rssi_value) {
        systime_t current_time = chTimeNow();

        // Find existing entry or use LRU entry
        for (auto& entry : entries_) {
            if (entry.frequency_hash == frequency_hash) {
                entry.rssi_value = rssi_value;
                entry.last_update = current_time;
                if (entry.detection_count < 255) entry.detection_count++;
                return;
            }
        }

        // Add new entry (overwrites oldest)
        entries_[head_] = {frequency_hash, 1, rssi_value, current_time};
        head_ = (head_ + 1) % MAX_ENTRIES;
    }

    uint8_t get_detection_count(size_t frequency_hash) const {
        for (const auto& entry : entries_) {
            if (entry.frequency_hash == frequency_hash) {
                return entry.detection_count;
            }
        }
        return 0;
    }

    int32_t get_rssi_value(size_t frequency_hash) const {
        for (const auto& entry : entries_) {
            if (entry.frequency_hash == frequency_hash) {
                return entry.rssi_value;
            }
        }
        return -120;
    }

    void clear() {
        entries_ = {};
        head_ = 0;
    }

    DetectionRingBuffer(const DetectionRingBuffer&) = delete;
    DetectionRingBuffer& operator=(const DetectionRingBuffer&) = delete;

private:
    std::array<DetectionEntry, MAX_ENTRIES> entries_{};
    size_t head_ = 0;
};

// Global instance for thread-safe access across components
extern DetectionRingBuffer global_detection_ring;

#endif // __UI_SIGNAL_PROCESSING_HPP__
