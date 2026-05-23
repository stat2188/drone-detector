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

    [[nodiscard]] PatternMatchResult match(const uint8_t* spectrum_256) noexcept;

private:
    const SignalPattern* patterns_{nullptr};
    size_t pattern_count_{0};

    void normalize(
        const uint8_t* fft_256,
        uint8_t* wave_16
    ) const noexcept;

    [[nodiscard]] static uint16_t compute_similarity(
        const uint8_t* wave_16,
        const uint8_t* pattern_wave
    ) noexcept;
};

} // namespace drone_analyzer

#endif // PATTERN_MATCHER_HPP
