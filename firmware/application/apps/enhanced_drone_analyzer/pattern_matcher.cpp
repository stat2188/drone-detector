#include "pattern_matcher.hpp"
#include "constants.hpp"

namespace drone_analyzer {

void PatternMatcher::set_patterns(const SignalPattern* patterns, size_t count) noexcept {
    if (patterns == nullptr) {
        patterns_ = nullptr;
        pattern_count_ = 0;
        return;
    }

    // CRITICAL FIX: Copy patterns instead of storing pointer to prevent race conditions
    // PatternManager may modify its internal array while matcher is using it
    const size_t copy_count = (count > MAX_PATTERNS) ? MAX_PATTERNS : count;
    
    for (size_t i = 0; i < copy_count; ++i) {
        patterns_copy_[i] = patterns[i];
    }
    
    patterns_ = patterns_copy_.data();
    pattern_count_ = copy_count;
}

void PatternMatcher::clear_patterns() noexcept {
    patterns_ = nullptr;
    pattern_count_ = 0;
}

void PatternMatcher::normalize_spectrum(
    const uint8_t* fft_256,
    uint8_t* wave_16
) noexcept {
    if (fft_256 == nullptr || wave_16 == nullptr) {
        return;
    }

    constexpr size_t valid_start = PATTERN_NORM_EDGE_SKIP;
    constexpr size_t valid_end = FFT_BIN_COUNT - PATTERN_NORM_EDGE_SKIP;
    constexpr size_t valid_bins = valid_end - valid_start;
    constexpr size_t bins_per_pattern = valid_bins / PATTERN_WAVEFORM_SIZE;

    for (size_t i = 0; i < PATTERN_WAVEFORM_SIZE; ++i) {
        const size_t bin_start = valid_start + (i * bins_per_pattern);
        size_t bin_end = bin_start + bins_per_pattern;

        if (bin_end > valid_end) {
            bin_end = valid_end;
        }

        uint32_t sum = 0;
        uint8_t count = 0;

        for (size_t j = bin_start; j < bin_end; ++j) {
            if (j >= FFT_DC_SPIKE_START && j < FFT_DC_SPIKE_END) {
                continue;
            }

            sum += fft_256[j];
            ++count;
        }

        if (count > 0) {
            wave_16[i] = static_cast<uint8_t>(sum / count);
        } else {
            wave_16[i] = 0;
        }
    }
}

const std::array<size_t, 4>& PatternMatcher::get_candidates(
    uint8_t peak_position,
    uint8_t width
) noexcept {
    candidates_.fill(INVALID_PATTERN_INDEX);
    size_t count = 0;

    if (patterns_ == nullptr || pattern_count_ == 0) {
        return candidates_;
    }

    for (size_t i = 0; i < pattern_count_ && count < 4; ++i) {
        const SignalPattern& pattern = patterns_[i];

        if (!pattern.is_enabled()) {
            continue;
        }

        const int8_t peak_diff = static_cast<int8_t>(pattern.features.peak_position) -
                               static_cast<int8_t>(peak_position);
        const int8_t width_diff = static_cast<int8_t>(pattern.features.width) -
                               static_cast<int8_t>(width);

        if (peak_diff >= -PATTERN_CANDIDATE_TOLERANCE && peak_diff <= PATTERN_CANDIDATE_TOLERANCE &&
            width_diff >= -PATTERN_CANDIDATE_TOLERANCE && width_diff <= PATTERN_CANDIDATE_TOLERANCE) {
            candidates_[count++] = i;
        }
    }

    return candidates_;
}

uint16_t PatternMatcher::compute_correlation(
    const uint8_t* pattern_a,
    const uint8_t* pattern_b
) const noexcept {
    if (pattern_a == nullptr || pattern_b == nullptr) {
        return 0;
    }

    int64_t sum_xy = 0;
    int64_t sum_x = 0;
    int64_t sum_y = 0;
    int64_t sum_x2 = 0;
    int64_t sum_y2 = 0;

    for (size_t i = 0; i < PATTERN_WAVEFORM_SIZE; ++i) {
        const int64_t x = static_cast<int64_t>(pattern_a[i]);
        const int64_t y = static_cast<int64_t>(pattern_b[i]);

        sum_xy += x * y;
        sum_x += x;
        sum_y += y;
        sum_x2 += x * x;
        sum_y2 += y * y;
    }

    const int64_t n = static_cast<int64_t>(PATTERN_WAVEFORM_SIZE);
    const int64_t numerator = (n * sum_xy) - (sum_x * sum_y);
    const int64_t denom_x = (n * sum_x2) - (sum_x * sum_x);
    const int64_t denom_y = (n * sum_y2) - (sum_y * sum_y);

    if (denom_x == 0 || denom_y == 0) {
        return 0;
    }

    const uint64_t denom_product = static_cast<uint64_t>(denom_x) *
                                     static_cast<uint64_t>(denom_y);
    const int64_t denominator = static_cast<int64_t>(isqrt64(denom_product));

    if (denominator == 0) {
        return 0;
    }

    // CRITICAL FIX: Use saturation arithmetic to prevent integer overflow
    // numerator can be up to ±INT64_MAX, shifting left by 8 can overflow
    constexpr int64_t SHIFT_BITS = 8;
    constexpr int64_t MAX_SAFE_NUMERATOR = (INT64_MAX >> SHIFT_BITS);

    int64_t r_scaled;
    if (numerator >= MAX_SAFE_NUMERATOR) {
        r_scaled = (INT64_MAX >> SHIFT_BITS);  // Saturate to max positive
    } else if (numerator <= -MAX_SAFE_NUMERATOR) {
        r_scaled = -(INT64_MAX >> SHIFT_BITS);  // Saturate to max negative
    } else {
        r_scaled = (numerator << SHIFT_BITS);
    }
    
    const int64_t r = r_scaled / denominator;

    if (r < 0) {
        return 0;
    }

    if (r > 1000) {
        return 1000;
    }

    return static_cast<uint16_t>(r);
}

uint8_t PatternMatcher::compute_feature_distance(
    const SpectrumShape::AnalysisResult& shape,
    const PatternFeatures& features
) const noexcept {
    int16_t diff_peak = static_cast<int16_t>(shape.peak_value) -
                        static_cast<int16_t>(features.peak_value);
    int16_t diff_margin = static_cast<int16_t>(shape.peak_margin) -
                        static_cast<int16_t>(features.margin);
    int16_t diff_width = static_cast<int16_t>(shape.signal_width) -
                        static_cast<int16_t>(features.width);

    if (diff_peak < 0) diff_peak = -diff_peak;
    if (diff_margin < 0) diff_margin = -diff_margin;
    if (diff_width < 0) diff_width = -diff_width;

    return static_cast<uint8_t>((diff_peak + diff_margin + diff_width) / 3);
}

uint32_t PatternMatcher::isqrt(uint32_t x) noexcept {
    if (x == 0) {
        return 0;
    }

    uint32_t result = 0;
    uint32_t bit = 1u << 30;

    while (bit > x) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (x >= result + bit) {
            x -= result + bit;
            result = (result >> 1u) + bit;
        } else {
            result = result >> 1u;
        }
        bit >>= 2u;
    }

    return result;
}

uint64_t PatternMatcher::isqrt64(uint64_t x) noexcept {
    if (x == 0) {
        return 0;
    }

    uint64_t result = 0;
    uint64_t bit = 1ULL << 62;

    while (bit > x) {
        bit >>= 2ULL;
    }

    while (bit != 0ULL) {
        if (x >= result + bit) {
            x -= result + bit;
            result = (result >> 1ULL) + bit;
        } else {
            result = result >> 1ULL;
        }
        bit >>= 2ULL;
    }

    return result;
}

PatternMatchResult PatternMatcher::match(
    const uint8_t* spectrum,
    const SpectrumShape::AnalysisResult& shape
) noexcept {
    PatternMatchResult result = PatternMatchResult::no_match();

    if (spectrum == nullptr || patterns_ == nullptr || pattern_count_ == 0) {
        return result;
    }

    normalize_spectrum(spectrum, normalized_);

    const uint8_t mapped_peak = static_cast<uint8_t>(
        (shape.peak_index * PATTERN_WAVEFORM_SIZE) / FFT_BIN_COUNT
    );
    const uint8_t mapped_width = static_cast<uint8_t>(
        (shape.signal_width * PATTERN_WAVEFORM_SIZE) / FFT_BIN_COUNT
    );

    const auto& candidates = get_candidates(mapped_peak, mapped_width);

    for (size_t i = 0; i < 4; ++i) {
        const size_t pattern_idx = candidates[i];
        if (pattern_idx >= pattern_count_ || pattern_idx == INVALID_PATTERN_INDEX) continue;

        const SignalPattern& pattern = patterns_[pattern_idx];

        const uint16_t corr = compute_correlation(
            pattern.waveform,
            normalized_
        );

        if (corr >= pattern.match_threshold) {
            const uint8_t feature_dist = compute_feature_distance(
                shape,
                pattern.features
            );

            if (corr > result.correlation_score) {
                result.pattern_index = pattern_idx;
                result.correlation_score = corr;
                result.feature_distance = feature_dist;
                result.matched = true;

                if (corr >= 800) {
                    result.status = PatternMatchStatus::EXCELLENT_MATCH;
                } else if (corr >= 600) {
                    result.status = PatternMatchStatus::STRONG_MATCH;
                } else if (corr >= 400) {
                    result.status = PatternMatchStatus::MODERATE_MATCH;
                } else {
                    result.status = PatternMatchStatus::WEAK_MATCH;
                }
            }
        }
    }

    if (!result.matched) {
        const auto& candidates_plus = get_candidates(mapped_peak + 1, mapped_width);
        for (size_t i = 0; i < 4; ++i) {
            if (candidates_plus[i] >= pattern_count_ || candidates_plus[i] == INVALID_PATTERN_INDEX) continue;

            const size_t pattern_idx = candidates_plus[i];
            const SignalPattern& pattern = patterns_[pattern_idx];

            const uint16_t corr = compute_correlation(
                pattern.waveform,
                normalized_
            );

            if (corr >= pattern.match_threshold) {
                result.pattern_index = pattern_idx;
                result.correlation_score = corr;
                result.matched = true;
                result.status = PatternMatchStatus::MODERATE_MATCH;
                break;
            }
        }
    }

    return result;
}

PatternMatchResult PatternMatcher::match(
    const uint8_t* spectrum,
    const SpectrumShape::AnalysisResult& shape,
    bool skip_candidate_filter
) noexcept {
    PatternMatchResult result = PatternMatchResult::no_match();

    if (spectrum == nullptr || patterns_ == nullptr || pattern_count_ == 0) {
        return result;
    }

    normalize_spectrum(spectrum, normalized_);

    if (skip_candidate_filter) {
        for (size_t i = 0; i < pattern_count_; ++i) {
            const SignalPattern& pattern = patterns_[i];
            if (!pattern.is_enabled()) {
                continue;
            }

            const uint16_t corr = compute_correlation(
                pattern.waveform,
                normalized_
            );

            if (corr >= pattern.match_threshold) {
                if (corr > result.correlation_score) {
                    result.pattern_index = i;
                    result.correlation_score = corr;
                    result.matched = true;

                    if (corr >= 800) {
                        result.status = PatternMatchStatus::EXCELLENT_MATCH;
                    } else if (corr >= 600) {
                        result.status = PatternMatchStatus::STRONG_MATCH;
                    } else if (corr >= 400) {
                        result.status = PatternMatchStatus::MODERATE_MATCH;
                    } else {
                        result.status = PatternMatchStatus::WEAK_MATCH;
                    }
                }
            }
        }
    } else {
        return match(spectrum, shape);
    }

    return result;
}

} // namespace drone_analyzer
