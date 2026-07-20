#ifndef WATERFALL_HISTORY_HPP
#define WATERFALL_HISTORY_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Ring buffer of recent FFT spectrum frames for temporal analysis.
 *
 * Enables multi-frame integration (Track-Before-Detect), FHSS hop detection,
 * Doppler estimation, and waterfall visualization. Stores the last N raw
 * FFT frames (256 bins each) in a circular buffer.
 *
 * @note No heap allocation — fixed-size std::array in BSS.
 * @note No floating-point — pure integer arithmetic.
 * @note No new threads — fed from existing UI thread message handlers.
 * @note Thread-safety: single-producer (UI thread), no concurrent access needed.
 *
 * SRAM: HISTORY_DEPTH × 256 = 8 × 256 = 2,048 bytes
 * Stack: ~4 bytes per call (write_index read)
 */
class WaterfallHistory {
public:
    /**
     * @brief Number of recent frames to retain.
     * @note 8 frames at 60 fps = 133 ms temporal window.
     *       At 20 fps (normal scanning): 400 ms window.
     *       Enough for FHSS hop detection (DJI hops at 50 Hz = 20 ms period).
     */
    static constexpr size_t HISTORY_DEPTH = 8;

    /**
     * @brief Push a new spectrum frame into the ring buffer.
     * @param spectrum_256 Pointer to 256-bin FFT data (uint8_t[256])
     * @note Copies all 256 bytes. Caller must ensure spectrum_256 is valid.
     *       Overwrites the oldest frame when buffer is full.
     */
    void push(const uint8_t* spectrum_256) noexcept {
        if (spectrum_256 == nullptr) return;
        auto& frame = frames_[write_index_];
        for (size_t i = 0; i < FFT_BIN_COUNT; ++i) {
            frame[i] = spectrum_256[i];
        }
        write_index_ = (write_index_ + 1) % HISTORY_DEPTH;
        if (frame_count_ < HISTORY_DEPTH) frame_count_++;
    }

    /**
     * @brief Get a frame by age (0 = most recent, HISTORY_DEPTH-1 = oldest).
     * @param age Frame age (0 = newest)
     * @return Pointer to 256-bin frame, or nullptr if no frames stored
     */
    [[nodiscard]] const uint8_t* get_frame(size_t age) const noexcept {
        if (age >= frame_count_) return nullptr;
        const size_t idx = (write_index_ + HISTORY_DEPTH - 1 - age) % HISTORY_DEPTH;
        return frames_[idx].data();
    }

    /**
     * @brief Get a single bin value from a specific frame.
     * @param bin FFT bin index (0-255)
     * @param age Frame age (0 = newest)
     * @return Bin power value (0-255), or 0 if frame not available
     */
    [[nodiscard]] uint8_t get_pixel(size_t bin, size_t age) const noexcept {
        if (bin >= FFT_BIN_COUNT) return 0;
        const uint8_t* frame = get_frame(age);
        return (frame != nullptr) ? frame[bin] : 0;
    }

    /**
     * @brief Get maximum power at a bin across all stored frames.
     * @param bin FFT bin index (0-255)
     * @return Maximum power value across all frames at this bin
     * @note Used for multi-frame peak detection (Track-Before-Detect).
     *       A weak signal present in multiple frames accumulates
     *       evidence even if no single frame exceeds the threshold.
     */
    [[nodiscard]] uint8_t get_max_across_frames(size_t bin) const noexcept {
        if (bin >= FFT_BIN_COUNT) return 0;
        uint8_t max_val = 0;
        for (size_t age = 0; age < frame_count_; ++age) {
            const uint8_t val = get_pixel(bin, age);
            if (val > max_val) max_val = val;
        }
        return max_val;
    }

    /**
     * @brief Count frames where bin exceeds a threshold.
     * @param bin FFT bin index (0-255)
     * @param threshold Minimum power value to count
     * @return Number of frames where bin power >= threshold
     * @note Used for TBD: if a bin exceeds threshold in >= min_frames
     *       out of HISTORY_DEPTH, it's a confirmed signal.
     */
    [[nodiscard]] uint8_t count_above_threshold(size_t bin, uint8_t threshold) const noexcept {
        if (bin >= FFT_BIN_COUNT) return 0;
        uint8_t count = 0;
        for (size_t age = 0; age < frame_count_; ++age) {
            if (get_pixel(bin, age) >= threshold) count++;
        }
        return count;
    }

    /**
     * @brief Compute frame-to-frame energy change at a bin (for Doppler).
     * @param bin FFT bin index (0-255)
     * @return Signed change: positive = energy increasing, negative = decreasing
     * @note Uses newest and second-newest frames. Returns 0 if < 2 frames.
     */
    [[nodiscard]] int8_t get_energy_delta(size_t bin) const noexcept {
        if (bin >= FFT_BIN_COUNT || frame_count_ < 2) return 0;
        const int16_t curr = static_cast<int16_t>(get_pixel(bin, 0));
        const int16_t prev = static_cast<int16_t>(get_pixel(bin, 1));
        const int16_t delta = curr - prev;
        return static_cast<int8_t>((delta > 127) ? 127 : ((delta < -127) ? -127 : delta));
    }

    /**
     * @brief Compute sum of power across all frames at a bin (coherent integration).
     * @param bin FFT bin index (0-255)
     * @return Sum of power values (max: 255 × 8 = 2040, fits uint16_t)
     * @note Integration gain: summing N frames improves SNR by ~10*log10(N) dB.
     *       8 frames → ~9 dB gain (3× sensitivity improvement).
     */
    [[nodiscard]] uint16_t get_integrated_power(size_t bin) const noexcept {
        if (bin >= FFT_BIN_COUNT) return 0;
        uint16_t sum = 0;
        for (size_t age = 0; age < frame_count_; ++age) {
            sum += get_pixel(bin, age);
        }
        return sum;
    }

    /**
     * @brief Reset all frames to zero.
     */
    void reset() noexcept {
        for (auto& frame : frames_) {
            frame.fill(0);
        }
        write_index_ = 0;
        frame_count_ = 0;
    }

    /**
     * @brief Check if buffer has at least min_frames stored.
     */
    [[nodiscard]] bool is_warm(uint8_t min_frames = 3) const noexcept {
        return frame_count_ >= min_frames;
    }

    /**
     * @brief Get number of frames currently stored.
     */
    [[nodiscard]] uint8_t size() const noexcept {
        return frame_count_;
    }

private:
    std::array<std::array<uint8_t, FFT_BIN_COUNT>, HISTORY_DEPTH> frames_{};
    uint8_t write_index_{0};
    uint8_t frame_count_{0};
};

} // namespace drone_analyzer

#endif // WATERFALL_HISTORY_HPP
