#ifndef PATTERN_MATCHER_HPP
#define PATTERN_MATCHER_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>

#include "ch.h"

#include "pattern_types.hpp"
#include "spectrum_shape.hpp"
#include "constants.hpp"

namespace drone_analyzer {

class PatternMatcher {
public:
    static constexpr size_t INVALID_PATTERN_INDEX = MAX_PATTERNS;

    PatternMatcher() noexcept
        : patterns_copy_{}
        , pattern_count_(0)
        , normalized_{}
        , candidates_{} {
    }
    ~PatternMatcher() noexcept = default;

    PatternMatcher(const PatternMatcher&) = delete;
    PatternMatcher& operator=(const PatternMatcher&) = delete;

    void set_patterns(const SignalPattern* patterns, size_t count) noexcept;

    [[nodiscard]] PatternMatchResult match(
        const uint8_t* spectrum,
        const SpectrumShape::AnalysisResult& shape
    ) noexcept;

    [[nodiscard]] PatternMatchResult match(
        const uint8_t* spectrum,
        const SpectrumShape::AnalysisResult& shape,
        bool skip_candidate_filter
    ) noexcept;

    void clear_patterns() noexcept;

private:
    std::array<SignalPattern, MAX_PATTERNS> patterns_copy_;
    size_t pattern_count_{0};

    std::array<uint8_t, PATTERN_WAVEFORM_SIZE> normalized_{};
    std::array<size_t, 4> candidates_{};

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

    [[nodiscard]] static uint64_t isqrt64(uint64_t x) noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MATCHER_HPP
