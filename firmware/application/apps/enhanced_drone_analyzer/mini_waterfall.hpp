#ifndef MINI_WATERFALL_HPP
#define MINI_WATERFALL_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Compact 4-bit packed scrolling waterfall for sweep visualization.
 *
 * Stores a ring buffer of compressed waterfall columns. Each column is derived
 * from a 240-byte composite spectrum by computing the peak power in each of
 * 24 horizontal bands (10 bins per band), then quantizing to 4-bit (16 colors).
 * Two rows are packed per byte (high nibble = even row, low nibble = odd row).
 *
 * @note No heap allocation — fixed-size std::array in BSS.
 * @note No floating-point — pure integer arithmetic.
 * @note Thread-safety: single-producer (UI thread), no concurrent access needed.
 * @note Replaces SignalTimeline (62 B) with richer 2D history (723 B).
 *
 * SRAM: HISTORY * COL_BYTES + 3 metadata = 60 * 12 + 3 = 723 bytes
 * Stack: ~16 bytes per push_column(), ~8 bytes per get_pixel()
 * Flash: ~300 bytes (all methods inline)
 */
class MiniWaterfall {
public:
    static constexpr uint16_t HEIGHT = WATERFALL_HEIGHT;
    static constexpr uint16_t HISTORY = WATERFALL_HISTORY;
    static constexpr uint8_t BAND_SIZE = 10;
    static constexpr uint8_t COL_BYTES = HEIGHT / 2;
    static constexpr size_t PALETTE_SIZE = 16;

    /**
     * @brief 16-color waterfall palette (RGB888 packed as uint32_t).
     * @note Index 0 = black (noise floor), Index 15 = white (strongest signal).
     *       Gradient: black → blue → cyan → green → yellow → orange → red → pink → white.
     *       Stored in Flash (constexpr), accessed via Color::RGB() at render time.
     */
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
     * @brief Compress and push one 240-byte composite column into the ring buffer.
     * @param composite_240 Pointer to 240-byte composite spectrum data.
     * @note Compression: 240 bins → 24 bands of 10 → peak per band → 4-bit quantize.
     *       Stack: ~16 bytes. O(240) integer comparisons + O(12) writes.
     */
    void push_column(const uint8_t* composite_240) noexcept {
        if (composite_240 == nullptr) return;

        std::array<uint8_t, COL_BYTES> col{};

        for (uint8_t row = 0; row < HEIGHT; ++row) {
            const uint8_t band_start = row * BAND_SIZE;
            const uint8_t peak = find_peak(composite_240, band_start, BAND_SIZE);
            const uint8_t nibble = peak >> 4;

            const uint8_t byte_idx = row / 2;
            if ((row & 1) == 0) {
                col[byte_idx] = nibble << 4;
            } else {
                col[byte_idx] |= nibble;
            }
        }

        write_column(col.data());
    }

    /**
     * @brief Push a single peak power value as a uniform waterfall column.
     * @param peak_power Maximum FFT bin power (0-255) for this frame.
     * @note For non-sweep mode: all 24 rows get the same quantized value.
     *       Avoids creating a 240-byte fake composite on the stack.
     *       Stack: ~8 bytes. O(12) writes.
     */
    void push_single_value(uint8_t peak_power) noexcept {
        const uint8_t nibble = peak_power >> 4;
        const uint8_t packed = static_cast<uint8_t>((nibble << 4) | nibble);

        std::array<uint8_t, COL_BYTES> col{};
        col.fill(packed);
        write_column(col.data());
    }

    /**
     * @brief Push columns from multiple active sweep windows.
     * @param composite1 First window's 240-byte composite (required).
     * @param composite2 Second window's composite (or nullptr).
     * @param composite3 Third window's composite (or nullptr).
     * @param composite4 Fourth window's composite (or nullptr).
     * @note Calls push_column() for each non-null pointer.
     *       Stack: ~24 bytes (4 pointer params + locals).
     */
    void push_multi_window(
        const uint8_t* composite1,
        const uint8_t* composite2 = nullptr,
        const uint8_t* composite3 = nullptr,
        const uint8_t* composite4 = nullptr
    ) noexcept {
        if (composite1 != nullptr) push_column(composite1);
        if (composite2 != nullptr) push_column(composite2);
        if (composite3 != nullptr) push_column(composite3);
        if (composite4 != nullptr) push_column(composite4);
    }

    /**
     * @brief Read a single pixel from the waterfall buffer.
     * @param col Column index (0 = oldest, count()-1 = newest).
     * @param row Row index (0 = bottom, HEIGHT-1 = top).
     * @return 4-bit palette index (0-15), or 0 if out of range.
     * @note Stack: ~8 bytes. O(1) read.
     */
    [[nodiscard]] uint8_t get_pixel(uint16_t col, uint8_t row) const noexcept {
        if (col >= count_ || row >= HEIGHT) return 0;

        uint16_t abs_col = (count_ < HISTORY)
            ? col
            : (write_pos_ + col);
        if (abs_col >= HISTORY) abs_col -= HISTORY;

        const uint8_t byte_val = buffer_[abs_col * COL_BYTES + row / 2];
        return (row & 1) ? (byte_val & 0x0F) : (byte_val >> 4);
    }

    /**
     * @brief Number of columns stored (max HISTORY).
     */
    [[nodiscard]] uint16_t count() const noexcept { return count_; }

    /**
     * @brief Reset all stored columns to empty state.
     * @note Stack: ~4 bytes. O(HISTORY * COL_BYTES) memset.
     */
    void reset() noexcept {
        buffer_.fill(0);
        write_pos_ = 0;
        count_ = 0;
    }

    /**
     * @brief Set the number of active sweep windows (1-4).
     * @param n Number of active windows (clamped to 1-4).
     * @note Affects render layout, not data storage.
     */
    void set_active_windows(uint8_t n) noexcept {
        active_windows_ = (n > 0 && n <= 4) ? n : 1;
    }

    /**
     * @brief Get the number of active sweep windows.
     */
    [[nodiscard]] uint8_t get_active_windows() const noexcept {
        return active_windows_;
    }

private:
    /**
     * @brief Find peak power in a band of bins.
     * @param data Pointer to 240-byte composite data.
     * @param start Starting bin index.
     * @param size Number of bins to examine.
     * @return Maximum power value in the band.
     * @note Stack: ~4 bytes. O(size) comparisons.
     */
    [[nodiscard]] static uint8_t find_peak(
        const uint8_t* data,
        uint8_t start,
        uint8_t size
    ) noexcept {
        uint8_t peak = 0;
        for (uint8_t i = 0; i < size; ++i) {
            const uint8_t val = data[start + i];
            if (val > peak) peak = val;
        }
        return peak;
    }

    /**
     * @brief Write a compressed column into the ring buffer.
     * @param col Pointer to COL_BYTES of packed data.
     */
    void write_column(const uint8_t* col) noexcept {
        const uint16_t offset = write_pos_ * COL_BYTES;
        for (uint8_t i = 0; i < COL_BYTES; ++i) {
            buffer_[offset + i] = col[i];
        }
        write_pos_ = (write_pos_ + 1) % HISTORY;
        if (count_ < HISTORY) ++count_;
    }

    std::array<uint8_t, HISTORY * COL_BYTES> buffer_{};
    uint16_t write_pos_{0};
    uint16_t count_{0};
    uint8_t active_windows_{1};
};

} // namespace drone_analyzer

#endif // MINI_WATERFALL_HPP
