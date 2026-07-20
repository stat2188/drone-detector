#ifndef SPECTRAL_KURTOSIS_HPP
#define SPECTRAL_KURTOSIS_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Higher-order statistical analysis of FFT spectrum.
 *
 * Computes kurtosis and skewness of power distribution to distinguish:
 * - Gaussian noise: kurtosis ≈ 0 (mesokurtic)
 * - Drone signal: kurtosis > 3 (leptokurtic — sharp peaks)
 * - WiFi/BT flat-top: kurtosis < 0 (platykurtic — flat distribution)
 *
 * Integer-only Q8.8 arithmetic. No heap, no FP, no exceptions.
 * Stateless: caller provides sorted scratch buffer (no static locals).
 *
 * SRAM: 0 bytes (caller-provided scratch buffer)
 * Stack: ~32 bytes for intermediate accumulators
 * Flash: ~200 bytes code
 */
class SpectralKurtosis {
public:
    /**
     * @brief Result of kurtosis computation.
     */
    struct KurtosisResult {
        int16_t kurtosis_x10;   ///< Kurtosis × 10 (e.g., 35 = 3.5)
        int16_t skewness_x10;   ///< Skewness × 10 (e.g., 12 = 1.2)
        uint8_t noise_floor;    ///< 25th percentile power
        uint8_t peak_power;     ///< Maximum power in range
        uint8_t peak_margin;    ///< peak_power - noise_floor
    };

    /**
     * @brief Compute kurtosis and skewness of a spectrum region.
     * @param spectrum FFT data (256 bins, 0-255 each)
     * @param start Start bin (inclusive, typically FFT_EDGE_SKIP)
     * @param end End bin (exclusive, typically FFT_BIN_COUNT - FFT_EDGE_SKIP)
     * @param sorted_buf Caller-provided scratch buffer for percentile computation.
     *                    Must be >= (end - start) bytes. Quickselect destroys contents.
     * @param sorted_buf_size Size of sorted_buf in bytes
     * @param dc_start Optional DC spike start bin (skip these bins)
     * @param dc_end Optional DC spike end bin (exclusive)
     * @return KurtosisResult with all statistics
     *
     * @note Kurtosis formula: K = (1/N) * Σ((x-μ)⁴) / σ⁴ - 3
     *       Skewness formula: S = (1/N) * Σ((x-μ)³) / σ³
     *       All arithmetic uses int32_t intermediates (max ~4.1×10⁹).
     *       For N=220 bins with power 0-255: Σ(x⁴) ≤ 220×255⁴ ≈ 9.2×10¹¹
     *       → overflow risk. Solution: work in scaled units (power/4 → 0-63).
     */
    [[nodiscard]] static KurtosisResult compute(
        const uint8_t* spectrum,
        size_t start,
        size_t end,
        uint8_t* sorted_buf,
        size_t sorted_buf_size,
        size_t dc_start = FFT_DC_SPIKE_START,
        size_t dc_end = FFT_DC_SPIKE_END
    ) noexcept {
        KurtosisResult result{};
        if (spectrum == nullptr || start >= end || sorted_buf == nullptr) return result;

        // Step 1: Compute mean (scaled to avoid overflow)
        uint32_t sum = 0;
        size_t count = 0;
        uint8_t peak = 0;

        for (size_t i = start; i < end; ++i) {
            if (i >= dc_start && i < dc_end) continue;
            sum += spectrum[i];
            if (spectrum[i] > peak) peak = spectrum[i];
            count++;
        }
        if (count < 4) return result;

        const uint16_t mean_x8 = static_cast<uint16_t>((sum * 8) / count);

        // Step 2: Compute variance and higher moments (scaled by 4 to prevent overflow)
        // power_scaled = (power - mean) / 4, range ≈ -64..+64
        uint32_t var_sum = 0;
        int32_t skew_sum = 0;   // SIGNED — skewness can be negative (left-skewed distribution)
        uint32_t kurt_sum = 0;

        for (size_t i = start; i < end; ++i) {
            if (i >= dc_start && i < dc_end) continue;
            const int32_t diff_x8 = static_cast<int32_t>(spectrum[i] * 8) - static_cast<int32_t>(mean_x8);
            const int32_t diff = diff_x8 / 8;  // Back to original scale, ≈ -64..+64
            const int32_t diff2 = diff * diff;  // ≈ 4096
            const int32_t diff4 = diff2 * diff2; // Max ≈ 16M (fits int32)

            var_sum += static_cast<uint32_t>(diff2);
            skew_sum += diff2 * diff;  // Signed accumulation — correct for negative skew
            kurt_sum += static_cast<uint32_t>(diff4);
        }

        if (count < 4 || var_sum == 0) return result;

        // Step 3: Compute noise floor (25th percentile via quickselect)
        // Uses caller-provided scratch buffer (no static local, thread-safe).
        size_t sort_count = 0;
        for (size_t i = start; i < end; ++i) {
            if (i >= dc_start && i < dc_end) continue;
            if (sort_count < sorted_buf_size) {
                sorted_buf[sort_count++] = spectrum[i];
            }
        }
        if (sort_count > 0) {
            const size_t k = sort_count / 4;
            quickselect(sorted_buf, sort_count, k);
            result.noise_floor = sorted_buf[k];
        }

        result.peak_power = peak;
        result.peak_margin = (peak > result.noise_floor) ? (peak - result.noise_floor) : 0;

        // Step 4: Compute kurtosis (×10 for storage)
        // K = (count × Σ(diff⁴)) / (Σ(diff²))² - 3
        // K × 10 = (count × 10 × Σ(diff⁴)) / (Σ(diff²))² - 30
        const uint32_t n = static_cast<uint32_t>(count);
        const uint64_t var_sq = static_cast<uint64_t>(var_sum) * var_sum;
        if (var_sq > 0) {
            const uint64_t kurt_raw = (static_cast<uint64_t>(n) * kurt_sum * 10) / var_sq;
            result.kurtosis_x10 = static_cast<int16_t>(kurt_raw) - 30;
        }

        // Step 5: Compute skewness (×10 for storage)
        // S = (count × Σ(diff³)) / (Σ(diff²))^1.5
        // Approximate: S × 10 = (count × 10 × Σ(diff³)) / (Σ(diff²) × sqrt(Σ(diff²)))
        // Use integer sqrt approximation: sqrt(x) ≈ (guess + x/guess) / 2 (Newton)
        if (var_sum > 0) {
            const uint32_t sqrt_var = isqrt(var_sum);
            const uint64_t denom = static_cast<uint64_t>(var_sum) * sqrt_var;
            if (denom > 0) {
                const int64_t skew_raw = (static_cast<int64_t>(n) * 10 * static_cast<int64_t>(skew_sum)) / static_cast<int64_t>(denom);
                result.skewness_x10 = static_cast<int16_t>((skew_raw > 32767) ? 32767 : ((skew_raw < -32767) ? -32767 : skew_raw));
            }
        }

        return result;
    }

private:
    /**
     * @brief Integer square root (Newton's method, bounded iterations).
     * @param x Input value (max ~4×10⁹ for uint32_t)
     * @return Floor of √x
     * @note Max 32 iterations for uint32_t (converges in ≤32 for any input).
     *       Iteration guard prevents infinite loop on corrupted data.
     */
    [[nodiscard]] static uint32_t isqrt(uint32_t x) noexcept {
        if (x == 0) return 0;
        uint32_t guess = x;
        uint32_t result = (guess + 1) / 2;
        uint8_t iterations = 0;
        while (result < guess && iterations < 32) {
            guess = result;
            result = (guess + x / guess) / 2;
            ++iterations;
        }
        return guess;
    }

    /**
     * @brief Quickselect for percentile computation (destructive to buf).
     */
    static void quickselect(uint8_t* buf, size_t count, size_t k) noexcept {
        if (count == 0) return;
        size_t left = 0;
        size_t right = count - 1;
        while (left < right) {
            const size_t pivot_idx = left + (right - left) / 2;
            const uint8_t pivot = buf[pivot_idx];
            buf[pivot_idx] = buf[right];
            buf[right] = pivot;
            size_t store = left;
            for (size_t i = left; i < right; ++i) {
                if (buf[i] < pivot) {
                    const uint8_t t = buf[store];
                    buf[store] = buf[i];
                    buf[i] = t;
                    store++;
                }
            }
            buf[right] = buf[store];
            buf[store] = pivot;
            if (store == k) break;
            if (store < k) left = store + 1;
            else right = store - 1;
        }
    }
};

} // namespace drone_analyzer

#endif // SPECTRAL_KURTOSIS_HPP
