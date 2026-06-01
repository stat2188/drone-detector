#ifndef PATTERN_MATCHER_HPP
#define PATTERN_MATCHER_HPP

#include <cstdint>
#include <cstddef>

#include "pattern_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief SAD-based pattern matching with frequency-proximity pre-filter.
 * @note Pure 16-bin waveform comparison. Each pattern's match_threshold is
 *       auto-tuned at save time from the captured peak's SNR margin
 *       (see PatternManagerView::save_current_pattern).
 * @note Pure integer math, no heap, no exceptions, no virtual functions.
 * @note Reentrant — single instance can be shared between threads if the
 *       caller serializes access (matches PatternManager's mutex).
 *
 * Stack: ~16 bytes per call (normalized[16] on caller frame).
 * Flash: 0 (header only).
 * SRAM: 0 (no static state, no members).
 */
class PatternMatcher {
public:
    PatternMatcher() noexcept = default;

    void set_patterns(const SignalPattern* patterns, size_t count) noexcept;
    void clear_patterns() noexcept;

    /**
     * @brief Match spectrum against all enabled patterns.
     * @param spectrum_256 Raw FFT spectrum (256 bins)
     * @param current_freq Current tuned frequency (Hz). If non-zero, patterns
     *                     with center_freq set are filtered by frequency proximity.
     *                     Pass 0 to disable frequency filtering (shape-only match).
     * @return Best match result (may be no_match if score below per-pattern threshold).
     */
    [[nodiscard]] PatternMatchResult match(
        const uint8_t* spectrum_256,
        FreqHz current_freq = 0
    ) noexcept;

    /**
     * @brief Normalize 256-bin FFT to 16-bin waveform for pattern matching.
     * @param fft_256 Raw FFT spectrum data (256 bins, 0-255)
     * @param wave_16 Output 16-bin normalized waveform
     * @note Skips DC spike (bins FFT_DC_SPIKE_START..FFT_DC_SPIKE_END)
     *       and filter rolloff edges (0..PATTERN_NORM_EDGE_SKIP-1, tail end).
     * @note Shared between PatternMatcher and PatternManagerView so saved
     *       patterns match against live spectra identically.
     */
    static void normalize(
        const uint8_t* fft_256,
        uint8_t* wave_16
    ) noexcept;

private:
    const SignalPattern* patterns_{nullptr};
    size_t pattern_count_{0};

    /**
     * @brief Compute SAD-based similarity (0-1000) after amplitude-normalizing
     *        both waveforms to their own peak.
     * @note Pure waveform comparison, no feature awareness.
     */
    [[nodiscard]] static uint16_t compute_similarity(
        const uint8_t* wave_16,
        const uint8_t* pattern_wave
    ) noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MATCHER_HPP
