#ifndef PATTERN_MATCHER_HPP
#define PATTERN_MATCHER_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>
#include "pattern_types.hpp"
#include "spectrum_shape.hpp"
#include "constants.hpp"

namespace drone_analyzer {

class PatternMatcher {
public:
    static constexpr uint16_t CORRELATION_SCALE_BITS = 8;
    static constexpr uint16_t CORRELATION_MAX_SCORE = 1000;
    static constexpr uint16_t CORRELATION_EXCELLENT = 800;
    static constexpr uint16_t CORRELATION_STRONG = 600;
    static constexpr uint16_t CORRELATION_MODERATE = 400;
    static constexpr uint16_t CORRELATION_WEAK = 200;

    PatternMatcher() noexcept = default;
    ~PatternMatcher() noexcept = default;

    PatternMatcher(const PatternMatcher&) = delete;
    PatternMatcher& operator=(const PatternMatcher&) = delete;

    void set_patterns(const SignalPattern* patterns, size_t count) noexcept;

    [[nodiscard]] PatternMatchResult match(
        const uint8_t* spectrum,
        const SpectrumShape::AnalysisResult& shape
    ) noexcept;

    void clear_patterns() noexcept;

private:
    const SignalPattern* patterns_{nullptr};
    size_t pattern_count_{0};

    uint8_t normalized_[PATTERN_WAVEFORM_SIZE];
    std::array<size_t, 4> candidates_;

    void normalize_spectrum(
        const uint8_t* fft_256,
        uint8_t* wave_16
    ) noexcept;

    [[nodiscard]] const std::array<size_t, 4>& get_candidates(
        uint8_t peak_position,
        uint8_t width
    ) noexcept;

    [[nodiscard]] uint16_t compute_correlation(
        const uint8_t* pattern_a,
        const uint8_t* pattern_b
    ) const noexcept;

    [[nodiscard]] uint8_t compute_feature_distance(
        const SpectrumShape::AnalysisResult& shape,
        const PatternFeatures& features
    ) const noexcept;

    [[nodiscard]] static uint32_t isqrt(uint32_t x) noexcept;

    [[nodiscard]] PatternMatchResult try_match_with_offset(
        uint8_t peak_position,
        uint8_t width,
        int8_t offset
    ) noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MATCHER_HPP
