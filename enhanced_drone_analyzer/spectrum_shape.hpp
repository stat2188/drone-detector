#ifndef SPECTRUM_SHAPE_HPP
#define SPECTRUM_SHAPE_HPP

#include <cstdint>
#include <cstddef>
#include "constants.hpp"


namespace drone_analyzer {

/**
 * @brief Spectrum shape analysis utility — single source of truth for V-shape detection.
 * @note Extracted from duplicated code in scanner.cpp:analyze_spectrum_shape()
 *       and scanner.cpp:process_spectrum_sweep().
 * @note Pure integer arithmetic — no floating point, no heap, no exceptions.
 * @note All buffers are caller-provided (no internal allocation).
 */
class SpectrumShape {
public:
    /**
     * @brief Result of spectrum shape analysis
     */
    struct AnalysisResult {
        bool signal_detected{false};
        uint8_t peak_value{0};
        size_t peak_index{0};
        uint8_t noise_floor{0};
        uint8_t peak_margin{0};
        size_t signal_width{0};
        size_t signal_left{0};
        size_t signal_right{0};
        int32_t avg_margin{0};
        uint8_t peak_flatness{0};
        uint8_t peak_symmetry{0};
    };

    /**
     * @brief Configuration for shape analysis filters
     */
    struct Config {
        uint8_t margin{DEFAULT_SPECTRUM_MARGIN};
        uint8_t min_width{DEFAULT_SPECTRUM_MIN_WIDTH};
        uint8_t max_width{DEFAULT_SPECTRUM_MAX_WIDTH};
        uint8_t peak_sharpness{DEFAULT_SPECTRUM_PEAK_SHARPNESS};
        uint8_t peak_ratio{DEFAULT_SPECTRUM_PEAK_RATIO};
        uint8_t valley_depth{DEFAULT_SPECTRUM_VALLEY_DEPTH};
        uint8_t flatness{DEFAULT_SPECTRUM_FLATNESS};
        uint8_t symmetry{DEFAULT_SPECTRUM_SYMMETRY};
        bool use_cfar{false};
    };

    /**
     * @brief Analyze spectrum shape for V/U signal peaks.
     * @param spectrum      256-bin FFT power values
     * @param sort_buf      Caller-provided buffer (>= 256 bytes) for quickselect
     * @param config        Filter configuration
     * @return AnalysisResult with detection status and parameters
     * @note Skips DC spike (bins 120-135) and edge bins (0-5, 250-255)
     */
    [[nodiscard]] static AnalysisResult analyze(
        const uint8_t* spectrum,
        uint8_t* sort_buf,
        const Config& config
    ) noexcept;

    /**
     * @brief Проверить проходит ли сигнал все фильтры по форме
     * @param result Результат анализа от analyze()
     * @param config Конфигурация фильтров
     * @return true если сигнал прошел все проверки
     */
    [[nodiscard]] static bool pass_filters(
        const uint8_t* spectrum,
        const AnalysisResult& result,
        const Config& config
    ) noexcept;

private:
    /**
     * @brief Compute noise floor via quickselect median (O(n))
     */
    [[nodiscard]] static uint8_t compute_noise_floor(
        const uint8_t* spectrum,
        uint8_t* sort_buf,
        size_t& usable_count
    ) noexcept;

    /**
     * @brief Find peak bin (skipping DC spike and edges)
     */
    [[nodiscard]] static size_t find_peak(
        const uint8_t* spectrum,
        uint8_t noise_floor,
        uint8_t& out_peak_value
    ) noexcept;

    /**
     * @brief Measure signal width around peak
     */
    [[nodiscard]] static size_t measure_width(
        const uint8_t* spectrum,
        size_t peak_index,
        uint8_t elevated_threshold,
        size_t& out_left,
        size_t& out_right
    ) noexcept;

    /**
     * @brief Compute average margin across signal bins
     */
    [[nodiscard]] static int32_t compute_avg_margin(
        const uint8_t* spectrum,
        size_t left,
        size_t right,
        uint8_t noise_floor
    ) noexcept;

    /**
     * @brief Check peak sharpness (V-shape enforcement)
     */
    [[nodiscard]] static bool check_sharpness(
        uint8_t peak_margin,
        int32_t avg_margin,
        uint8_t threshold
    ) noexcept;

    /**
     * @brief Check peak ratio (tall + narrow = inverted-V)
     */
    [[nodiscard]] static bool check_peak_ratio(
        uint8_t peak_margin,
        size_t signal_width,
        uint8_t threshold
    ) noexcept;

    /**
     * @brief Check valley depth (deep valleys flanking peak = V-shape)
     */
    [[nodiscard]] static bool check_valley_depth(
        const uint8_t* spectrum,
        size_t left,
        size_t right,
        uint8_t noise_floor,
        uint8_t threshold
    ) noexcept;

    /**
     * @brief Check flatness (flat-top detection for wideband signals)
     * @param spectrum FFT spectrum data (256 bins)
     * @param peak_index Peak bin index
     * @param left Signal left boundary (from measure_width)
     * @param right Signal right boundary (from measure_width)
     * @param threshold Flatness threshold (0-100%)
     * @return true if sharp signal (low flatness), false if flat-top (high flatness)
     * @note Counts consecutive bins at 90%+ of peak power
     * @note WiFi/BT flat-top: flatness ~ 50-80% (many bins near peak)
     * @note Drone V-shape: flatness ~ 5-20% (only peak bin at high power)
     * @note Higher threshold = stricter (rejects more flat signals)
     */
    [[nodiscard]] static bool check_flatness(
        const uint8_t* spectrum,
        size_t peak_index,
        size_t left,
        size_t right,
        uint8_t threshold
    ) noexcept;

    /**
     * @brief Check symmetry (left/right width similarity)
     */
    [[nodiscard]] static bool check_symmetry(
        size_t peak_index,
        size_t left,
        size_t right,
        size_t signal_width,
        uint8_t threshold
    ) noexcept;
};

} // namespace drone_analyzer

#endif // SPECTRUM_SHAPE_HPP
