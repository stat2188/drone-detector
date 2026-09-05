#ifndef MINI_WATERFALL_HPP
#define MINI_WATERFALL_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Compact row-oriented scrolling waterfall (time-ordered ring buffer).
 *
 * Each sweep pass produces one ROW of 60 frequency bands (compressed from 240 bins).
 * Rows are stored time-ordered: row 0 = oldest, row count()-1 = newest (ring buffer).
 * The renderer places the newest row at the TOP (top-down flow) by reading rows
 * in reverse order — see DroneDisplay::render_waterfall()/render_sweep_waterfalls().
 *
 * Layout: X axis = frequency (bands left-to-right), Y axis = time (newest at top).
 *
 * @note Row-oriented; rendering convention (newest-at-top) set by the consumer.
 * @note 4-bit packed: 2 bands per byte (high nibble = even band, low nibble = odd band).
 * @note No heap allocation -- fixed-size std::array in BSS.
 * @note No floating-point -- pure integer arithmetic.
 * @note Thread-safety: single-producer (UI thread), no concurrent access needed.
 *
 * SRAM: MAX_ROWS * ROW_SIZE = 12 * 30 = 360 bytes
 * Stack: ~12 bytes per push_row(), ~8 bytes per get_pixel()
 * Flash: ~400 bytes (all methods inline)
 */
class MiniWaterfall {
public:
    static constexpr uint8_t BANDS = WATERFALL_HEIGHT;
    static constexpr uint8_t MAX_ROWS = 12;
    static constexpr uint8_t BAND_SIZE = 4;
    static constexpr uint8_t ROW_SIZE = BANDS / 2;
    static constexpr size_t PALETTE_SIZE = 16;

    static constexpr uint32_t PALETTE[PALETTE_SIZE] = {
        0x000000,  //  0: black
        0x000040,  //  1: dark blue
        0x0000FF,  //  2: blue
        0x004040,  //  3: dark cyan
        0x008080,  //  4: cyan
        0x004000,  //  5: dark green
        0x00FF00,  //  6: green
        0x80FF00,  //  7: yellow-green
        0xFFFF00,  //  8: yellow
        0xFF8000,  //  9: orange
        0x800000,  // 10: dark red
        0xFF0000,  // 11: red
        0xFF0080,  // 12: pink
        0xFF40A0,  // 13: light pink
        0xFF80C0,  // 14: light red
        0xFFFFFF,  // 15: white
    };

    MiniWaterfall() noexcept = default;

    /**
     * @brief Compress and push one 240-byte composite as a new waterfall row.
     * @param composite_240 Pointer to 240-byte composite spectrum data.
     * @note Compression: 240 bins -> 60 bands of 4 -> peak per band -> 4-bit quantize.
     *       New row appears at the bottom; oldest row scrolls off the top.
     *       Stack: ~0 bytes (no locals beyond loop vars).
     * @return true if a scroll occurred (buffer was full, oldest row overwritten).
     */
    bool push_row(const uint8_t* composite_240) noexcept {
        if (composite_240 == nullptr) return false;

        const bool scrolled = (count_ >= MAX_ROWS);
        const uint16_t offset = static_cast<uint16_t>(write_pos_) * ROW_SIZE;

        for (uint8_t band = 0; band < BANDS; ++band) {
            const uint8_t band_start = band * BAND_SIZE;
            uint8_t peak = 0;
            for (uint8_t i = 0; i < BAND_SIZE; ++i) {
                const uint8_t val = composite_240[band_start + i];
                if (val > peak) peak = val;
            }
            const uint8_t nibble = peak >> 4;

            const uint8_t byte_idx = band / 2;
            if ((band & 1) == 0) {
                buffer_[offset + byte_idx] = nibble << 4;
            } else {
                buffer_[offset + byte_idx] |= nibble;
            }
        }

        write_pos_ = (write_pos_ + 1) % MAX_ROWS;
        if (count_ < MAX_ROWS) ++count_;
        return scrolled;
    }

    /**
     * @brief Push a single peak power value as a uniform waterfall row.
     * @param peak_power Maximum FFT bin power (0-255) for this frame.
     * @note For non-sweep mode: all 60 bands get the same quantized value.
     *       Stack: ~0 bytes.
     * @return true if a scroll occurred (buffer was full, oldest row overwritten).
     */
    bool push_single_value(uint8_t peak_power) noexcept {
        const bool scrolled = (count_ >= MAX_ROWS);
        const uint8_t nibble = peak_power >> 4;
        const uint8_t packed = static_cast<uint8_t>((nibble << 4) | nibble);

        const uint16_t offset = static_cast<uint16_t>(write_pos_) * ROW_SIZE;
        for (uint8_t i = 0; i < ROW_SIZE; ++i) {
            buffer_[offset + i] = packed;
        }

        write_pos_ = (write_pos_ + 1) % MAX_ROWS;
        if (count_ < MAX_ROWS) ++count_;
        return scrolled;
    }

    /**
     * @brief Read a single pixel from the waterfall buffer.
     * @param row Row index (0 = oldest/top, count()-1 = newest/bottom).
     * @param band Frequency band index (0 = lowest freq/left, BANDS-1 = highest/right).
     * @return 4-bit palette index (0-15), or 0 if out of range.
     * @note Stack: ~0 bytes. O(1) read.
     */
    [[nodiscard]] uint8_t get_pixel(uint8_t row, uint8_t band) const noexcept {
        if (row >= count_ || band >= BANDS) return 0;

        uint8_t abs_row;
        if (count_ < MAX_ROWS) {
            abs_row = row;
        } else {
            abs_row = (write_pos_ + row) % MAX_ROWS;
        }

        const uint8_t byte_val = buffer_[abs_row * ROW_SIZE + band / 2];
        return (band & 1) ? (byte_val & 0x0F) : (byte_val >> 4);
    }

    /**
     * @brief Number of stored rows (max MAX_ROWS).
     */
    [[nodiscard]] uint8_t count() const noexcept { return count_; }

    /**
     * @brief Reset all stored rows to empty state.
     * @note Stack: ~0 bytes. O(MAX_ROWS * ROW_SIZE) memset.
     */
    void reset() noexcept {
        buffer_.fill(0);
        write_pos_ = 0;
        count_ = 0;
    }

private:
    std::array<uint8_t, MAX_ROWS * ROW_SIZE> buffer_{};
    uint8_t write_pos_{0};
    uint8_t count_{0};
};

} // namespace drone_analyzer

#endif // MINI_WATERFALL_HPP
