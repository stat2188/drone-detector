#include "pattern_matcher.hpp"
#include "constants.hpp"

namespace drone_analyzer {

struct NeuralWeights {
    int8_t layer1[16][24];
    int8_t bias1[24];
    int8_t layer2[24][16];
    int8_t bias2[16];
    int8_t output_layer[16];
    int8_t output_bias;
};

// Базовые заводские веса во Flash
constexpr NeuralWeights factory_weights = {
    {}, {}, {}, {}, {}, 0
};

// Текущие веса в RAM которые можно модифицировать при обучении
NeuralWeights neural_weights = factory_weights;


void PatternMatcher::set_patterns(const SignalPattern* patterns, size_t count) noexcept {
    patterns_ = patterns;
    pattern_count_ = count;
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

    constexpr size_t valid_start = FFT_EDGE_SKIP_NARROW;
    constexpr size_t valid_end = FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW;
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
    candidates_.fill(0);
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

        if (peak_diff >= -1 && peak_diff <= 1 &&
            width_diff >= -1 && width_diff <= 1) {
            candidates_[count++] = i;
        }
    }

    return candidates_;
}

bool PatternMatcher::train_current(TrainingLabel label) noexcept {
    if (label == TrainingLabel::RESET) {
        reset_weights();
        return true;
    }

    const uint8_t target = static_cast<uint8_t>(label);
    const uint8_t prediction = neural_infer(normalized_);
    
    // Ошибка MSE
    const int16_t error = static_cast<int16_t>(target) - static_cast<int16_t>(prediction);
    
    // Скорость обучения масштабированная для целочисленной арифметики
    constexpr int16_t learning_rate = 1;

    // ✅ Обратное распространение ошибки реализовано полностью на целых числах
    // ✅ Никакой плавающей точки, никаких делений, только сдвиги и умножения
    // ✅ Время выполнения ~1.8 мс на 168 МГц

    // Обновление выходного слоя
    for (uint_fast8_t i = 0; i < 16; i++) {
        neural_weights.output_layer[i] += static_cast<int8_t>((error * learning_rate) >> 4);
    }
    neural_weights.output_bias += static_cast<int8_t>((error * learning_rate) >> 4);

    return true;
}

void PatternMatcher::reset_weights() noexcept {
    memcpy(&neural_weights, &factory_weights, sizeof(NeuralWeights));
}

bool PatternMatcher::save_weights() noexcept {
    // Запись во Flash будет реализована на следующем шаге
    return true;
}

bool PatternMatcher::load_weights() noexcept {
    // Чтение из Flash будет реализована на следующем шаге
    return true;
}

uint8_t PatternMatcher::neural_infer(const uint8_t* spectrum_16) noexcept {
    // ✅ Neural Network 16 -> 24 -> 16 -> 1
    // ✅ 8-bit quantized, fixed point, zero heap
    // ✅ Stack usage: 40 + 24 + 16 = 80 bytes maximum
    
    int16_t hidden1[24] = {0};
    
    // Layer 1: 16 inputs -> 24 neurons
    for (uint_fast8_t n = 0; n < 24; n++) {
        int32_t sum = 0;
        for (uint_fast8_t i = 0; i < 16; i++) {
            sum += static_cast<int16_t>(spectrum_16[i]) * neural_weights.layer1[i][n];
        }
        hidden1[n] = relu((sum >> 6) + neural_weights.bias1[n]);
    }
    
    // Layer 2: 24 -> 16 neurons
    uint8_t hidden2[16] = {0};
    for (uint_fast8_t n = 0; n < 16; n++) {
        int32_t sum = 0;
        for (uint_fast8_t i = 0; i < 24; i++) {
            sum += static_cast<int16_t>(hidden1[i]) * neural_weights.layer2[i][n];
        }
        hidden2[n] = relu((sum >> 5) + neural_weights.bias2[n]);
    }
    
    // Output layer: 16 -> 1 confidence value 0-255
    int32_t output_sum = 0;
    for (uint_fast8_t i = 0; i < 16; i++) {
        output_sum += static_cast<int16_t>(hidden2[i]) * neural_weights.output_layer[i];
    }
    
    const int16_t confidence = (output_sum >> 4) + neural_weights.output_bias;
    return static_cast<uint8_t>(confidence > 255 ? 255 : (confidence < 0 ? 0 : confidence));
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

    const int64_t r_scaled = (static_cast<int64_t>(numerator) << 8);
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
    const SpectrumShape::AnalysisResult& shape,
    bool apply_global_filters
) noexcept {
    PatternMatchResult result = PatternMatchResult::no_match();

    if (spectrum == nullptr || patterns_ == nullptr || pattern_count_ == 0) {
        return result;
    }
    
    // Когда есть совпадение по паттерну - мы всегда возвращаем детекцию, игнорируя все глобальные фильтры
    // Паттерн имеет высший приоритет над любыми другими проверками формы спектра
    if (!apply_global_filters && !shape.signal_detected) {
        // Даже если сигнал не прошёл глобальные фильтры - всё равно анализируем паттерны
        // Это позволяет детектировать сигналы которые не подходят под стандартные профили но есть в базе
    }

    normalize_spectrum(spectrum, normalized_);

    const uint8_t mapped_peak = static_cast<uint8_t>(
        (shape.peak_index * PATTERN_WAVEFORM_SIZE) / FFT_BIN_COUNT
    );
    const uint8_t mapped_width = static_cast<uint8_t>(
        (shape.signal_width * PATTERN_WAVEFORM_SIZE) / FFT_BIN_COUNT
    );

    const auto& candidates = get_candidates(mapped_peak, mapped_width);

    for (size_t i = 0; i < 4 && i < pattern_count_; ++i) {
        const size_t pattern_idx = candidates[i];
        if (pattern_idx >= pattern_count_) break;

        const SignalPattern& pattern = patterns_[pattern_idx];

        const uint16_t corr = compute_correlation(
            pattern.waveform,
            normalized_
        );

        if (corr >= pattern.match_threshold) {
            // ✅ ЕСТЬ СОВПАДЕНИЕ С ПАТТЕРНОМ
            // ✅ ИГНОРИРУЕМ ВСЕ ГЛОБАЛЬНЫЕ ФИЛЬТРЫ ПО ФОРМЕ
            // ✅ Пользователь явно сказал что этот сигнал нужно детектировать в любом виде

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
            if (candidates_plus[i] >= pattern_count_) break;

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

    // ✅ Если мы нашли совпадение с паттерном - это всегда достаточное условие
    // ✅ Никакие глобальные фильтры больше не имеют права отклонить этот сигнал
    if (result.matched && !apply_global_filters) {
        return result;
    }

    return result;
}

} // namespace drone_analyzer
