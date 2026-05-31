#ifndef PATTERN_MATCHER_HPP
#define PATTERN_MATCHER_HPP

#include <cstdint>
#include <cstddef>

#include "pattern_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

class PatternMatcher {
public:
    PatternMatcher() noexcept = default;

    void set_patterns(const SignalPattern* patterns, size_t count) noexcept;
    void clear_patterns() noexcept;

    /**
     * @brief Match spectrum against all enabled patterns
     * @param spectrum_256 Raw FFT spectrum (256 bins)
     * @param current_freq Current tuned frequency (Hz). If non-zero, patterns with
     *                     center_freq set will be filtered by frequency proximity.
     *                     Pass 0 to disable frequency filtering (shape-only match).
     * @return Best match result (may be no_match if score below threshold)
     */
    [[nodiscard]] PatternMatchResult match(
        const uint8_t* spectrum_256,
        FreqHz current_freq = 0
    ) noexcept;

    /**
     * @brief Normalize 256-bin FFT to 16-bin waveform for pattern matching
     * @param fft_256 Raw FFT spectrum data (256 bins, 0-255)
     * @param wave_16 Output 16-bin normalized waveform
     * @note Skips DC spike (bins 120-135) and filter rolloff edges (0-9, 246-255)
     * @note Shared between PatternMatcher and PatternManagerView to ensure
     *       saved patterns match against live spectra identically
     */
    static void normalize(
        const uint8_t* fft_256,
        uint8_t* wave_16
    ) noexcept;

private:
    const SignalPattern* patterns_{nullptr};
    size_t pattern_count_{0};

    [[nodiscard]] static uint16_t compute_similarity(
        const uint8_t* wave_16,
        const uint8_t* pattern_wave
    ) noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MATCHER_HPP
