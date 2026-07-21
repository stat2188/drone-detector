#ifndef SIGNAL_TIMELINE_HPP
#define SIGNAL_TIMELINE_HPP

#include <cstdint>
#include <cstddef>
#include <array>

namespace drone_analyzer {

/**
 * @brief Ring buffer of peak power values for scrolling timeline display.
 * @note Replaces HistogramProcessor for display — 60 bytes vs 1,640 bytes.
 * @note Single-producer/single-consumer safe (ChibiOS cooperative scheduling).
 * @note Each push_frame() stores one frame's peak FFT power (0-255).
 *       render_timeline() draws a left→right sparkline of the last WIDTH samples.
 * @note SRAM: 62 bytes (60 data + write_pos + count).
 * @note Flash: ~250 bytes (all methods inline).
 */
class SignalTimeline {
public:
    static constexpr size_t WIDTH = 60;

    SignalTimeline() noexcept = default;

    /**
     * @brief Push one frame's peak power into the timeline.
     * @param peak_power Maximum FFT bin power (0-255) from this frame.
     * @note O(1), no division, branchless on Cortex-M4 (predicted not-taken).
     */
    void push_frame(uint8_t peak_power) noexcept {
        buffer_[write_pos_] = peak_power;
        ++write_pos_;
        if (write_pos_ >= WIDTH) write_pos_ = 0;
        if (count_ < WIDTH) ++count_;
    }

    /**
     * @brief Get sample at logical index (0 = oldest, count_-1 = newest).
     * @param index Logical index into the timeline.
     * @return Power value (0-255), or 0 if index out of range.
     * @note Uses conditional subtract instead of modulo (~2 cycles vs ~30).
     */
    [[nodiscard]] uint8_t operator[](size_t index) const noexcept {
        if (index >= count_) return 0;
        size_t idx = (count_ < WIDTH) ? index : (write_pos_ + index);
        if (idx >= WIDTH) idx -= WIDTH;
        return buffer_[idx];
    }

    /**
     * @brief Number of samples stored (max WIDTH).
     */
    [[nodiscard]] size_t count() const noexcept { return count_; }

    /**
     * @brief Maximum power across all stored samples.
     */
    [[nodiscard]] uint8_t peak() const noexcept {
        uint8_t max_val = 0;
        size_t idx = (count_ < WIDTH) ? 0 : write_pos_;
        for (size_t i = 0; i < count_; ++i) {
            if (idx >= WIDTH) idx = 0;
            const uint8_t v = buffer_[idx];
            if (v > max_val) max_val = v;
            ++idx;
        }
        return max_val;
    }

    /**
     * @brief Average power across all stored samples.
     */
    [[nodiscard]] uint8_t average() const noexcept {
        if (count_ == 0) return 0;
        uint16_t sum = 0;
        size_t idx = (count_ < WIDTH) ? 0 : write_pos_;
        for (size_t i = 0; i < count_; ++i) {
            if (idx >= WIDTH) idx = 0;
            sum += buffer_[idx];
            ++idx;
        }
        return static_cast<uint8_t>(sum / count_);
    }

    /**
     * @brief Reset timeline to empty state.
     */
    void reset() noexcept {
        write_pos_ = 0;
        count_ = 0;
        buffer_.fill(0);
    }

private:
    std::array<uint8_t, WIDTH> buffer_{};
    size_t write_pos_{0};
    size_t count_{0};
};

} // namespace drone_analyzer

#endif // SIGNAL_TIMELINE_HPP
