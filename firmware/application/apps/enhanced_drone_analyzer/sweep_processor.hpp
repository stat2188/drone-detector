#ifndef SWEEP_PROCESSOR_HPP
#define SWEEP_PROCESSOR_HPP

#include <cstdint>
#include <cstddef>
#include "message.hpp"
#include "drone_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Maps FFT bins to composite sweep pixels (frequency-ascending order).
 * @note Pure function — no UI dependencies, no state, no heap.
 * @note Iterates bins in frequency-ascending order for correct sweep composite:
 * @note   bin 0..117   → FFT bins 2..119   (upper sideband, lower frequencies)
 * @note   bin 118..119 → FFT bins 134..135 (DC spike — skipped entirely, no Hz)
 * @note   bin 120..237 → FFT bins 136..253 (lower sideband, higher frequencies)
 * @note   bin 238..239 unused (end of slice)
 * @note M0 baseband handles FFT; M4 only maps bins to pixels.
 */
class SweepProcessor {
public:
    static constexpr uint8_t UPPER_OFFSET = SWEEP_FFT_MAP_CROSSOVER - 2;
    static constexpr uint8_t UPPER_PIXEL_END = SWEEP_PIXELS_PER_SLICE - 2;

    /**
     * @brief Process one FFT frame into a composite pixel buffer.
     * @param spectrum       256-bin FFT power values from baseband
     * @param composite      Output pixel buffer (COMPOSITE_SIZE bytes)
     * @param pixel_index    In/out: progress position (scan-head marker +
     *                       line_full completion bookkeeping). Advanced by the
     *                       internal accumulator, NOT by data placement.
     * @param pixel_max      Unused since true-position painting (kept for API
     *                       stability; the old sequential flush used it).
     * @param bins_hz_acc    In/out: accumulated Hz remainder (integer remainder
     *                       prevents drift) driving pixel_index.
     * @param pixel_step_hz  Hz per pixel on the window scale (range / 240)
     * @param f_center       FFT slice center frequency (Hz)
     * @param exception_radius_hz Exclusion radius around exception frequencies
     * @param exceptions     Exception frequency array
     * @param num_exceptions Number of valid exception entries
     * @param effective_bin_size Hz contributed per FFT bin to the progress
     *        accumulator. Must equal step_hz / 236 (236 = 240 total bins - 2
     *        end skip - 2 DC spike) so each slice advances the accumulator by
     *        exactly step_hz. DC spike bins are skipped (no Hz, no power).
     * @param f_min          Window lower bound (Hz) — linear scale anchor
     * @param f_max          Window upper bound (Hz) — linear scale anchor
     * @return Updated pixel_index
     * @note DATA PLACEMENT — TRUE POSITION, not sequential: every bin is
     *       written at composite[(freq - f_min) * 240 / range] — the exact
     *       pixel its RF frequency occupies on the linear window scale, i.e.
     *       the same mapping the band title (f_min..f_max) and every tracked
     *       drone's frequency use. The previous sequential placement squeezed
     *       each slice's ~20 MHz of RF content into its step_hz pixel slot
     *       (~2.4x compression at the gapless 8.83 MHz step), drawing peaks at
     *       columns that did not match the tracked frequency, and duplicating
     *       every signal once per overlapping slice (ghost peaks).
     * @note Overlapping slices (gapless step < slice RF span) now write the
     *       SAME column — max-hold (composite[px] = max(old, power)) makes
     *       them reinforce instead of ghosting, and the 1.8 MHz DC notch of
     *       slice k is covered by slices k-1/k+1. Stack: ~16 bytes.
     */
    static uint16_t process_frame(
        const ChannelSpectrum& spectrum,
        uint8_t* composite,
        uint16_t& pixel_index,
        uint8_t& pixel_max,
        FreqHz& bins_hz_acc,
        FreqHz pixel_step_hz,
        FreqHz f_center,
        FreqHz exception_radius_hz,
        const FreqHz* exceptions,
        uint8_t num_exceptions,
        FreqHz effective_bin_size,
        FreqHz f_min,
        FreqHz f_max
    ) noexcept;

    /**
     * @brief Reorder a single FFT frame into Looking Glass pixel order.
     * @param spectrum    256-bin FFT power values from baseband
     * @param lg_buffer   Output buffer (COMPOSITE_SIZE bytes = 240 pixels)
     * @note Produces the Looking Glass display mapping:
     *       Pixels 0..119 = FFT bins 134..253 (lower sideband, above center)
     *       Pixels 120..237 = FFT bins 2..119 (upper sideband, below center)
     *       This is the per-frame LG view used for shape analysis — continuous
     *       frequency ordering that matches the Looking Glass display.
     * @note Pixels 238-239 are zeroed (map to DC spike bins 120-121).
     * @note Stack: ~0 bytes (writes to caller buffer). Flash: <100 bytes.
     * @note process_frame() uses a DIFFERENT mapping (frequency-ascending) for
     *       the sweep composite — this function is NOT used for composite data.
     */
    static void reorder_frame(
        const ChannelSpectrum& spectrum,
        uint8_t* lg_buffer
    ) noexcept;

private:
    [[nodiscard]] static bool is_exception_freq(
        FreqHz hz,
        FreqHz exception_radius_hz,
        const FreqHz* exceptions,
        uint8_t num_exceptions
    ) noexcept;
};

} // namespace drone_analyzer

#endif // SWEEP_PROCESSOR_HPP
