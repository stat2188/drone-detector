/**
 * @file ui_spectral_analyzer.hpp
 * @brief Spectral Analysis for Enhanced Drone Analyzer
 */

#ifndef UI_SPECTRAL_ANALYZER_HPP_
#define UI_SPECTRAL_ANALYZER_HPP_

#include <cstdint>
#include <array>
#include <algorithm>
#include <cstring>
#include "ui_drone_common_types.hpp"
#include "radio.hpp"
#include "eda_optimized_utils.hpp"
#include "eda_constants.hpp"

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// Type Aliases (Semantic Types)
using BinValue = uint8_t;
using HistogramIndex = uint16_t;
using SpectralBin = size_t;
using SNRValue = uint8_t;
using SignalWidthHz = uint32_t;
using WidthBins = uint8_t;

// Signal Signature Enum
enum class SignalSignature : uint8_t {
    NOISE = 0,            ///< No significant signal detected
    WIDEBAND_WIFI = 1,    ///< Wideband WiFi signal
    NARROWBAND_DRONE = 2,  ///< Narrowband drone signal
    DIGITAL_FPV = 3        ///< Digital FPV signal
};

using Signature = SignalSignature;
using Threat = ThreatLevel;
using Drone = DroneType;

// Constants (Flash-Resident)
namespace SpectralAnalysisConstants {
    /// @brief Number of spectral bins
    constexpr size_t SPECTRAL_BIN_COUNT = 256;

    /// @brief First valid spectral bin (exclude DC offset)
    constexpr size_t VALID_BIN_START = 8;

    /// @brief Last valid spectral bin (exclude edge effects)
    constexpr size_t VALID_BIN_END = 240;

    /// @brief Number of valid spectral bins
    constexpr size_t VALID_BIN_COUNT = VALID_BIN_END - VALID_BIN_START;

    /// @brief SNR threshold for signal detection (dB)
    constexpr uint8_t SNR_THRESHOLD = 10;

    /// @brief Peak threshold for signal width calculation (dB below peak)
    constexpr uint8_t PEAK_THRESHOLD_DB = 6;

    /// @brief Maximum signal width for narrowband drone (Hz)
    constexpr uint32_t DRONE_MAX_WIDTH_HZ = 2'500'000;

    /// @brief Minimum signal width for wideband WiFi (Hz)
    constexpr uint32_t WIFI_MIN_WIDTH_HZ = 10'000'000;

    /// @brief Maximum slice bandwidth (Hz)
    constexpr uint32_t MAX_SLICE_BANDWIDTH_HZ = 28'000'000;

    /// @brief Compile-time inverse for fixed-point division (Q16)
    constexpr uint32_t INV_BIN_COUNT_Q16 = (65536 + VALID_BIN_COUNT / 2) / VALID_BIN_COUNT;

    /// @brief Histogram bin divisor (value / 4 = histogram index)
    constexpr uint8_t HISTOGRAM_BIN_DIVISOR = 4;

    /// @brief Number of histogram bins (64 bins × 2 bytes = 128 bytes)
    constexpr size_t HISTOGRAM_BINS = 64;

    /// @brief Q16 fixed-point scale factor
    constexpr uint32_t Q16_FIXED_POINT_SCALE = 65536;

    /// @brief Compile-time overflow check: VALID_BIN_COUNT must be positive
    static_assert(VALID_BIN_COUNT > 0, "VALID_BIN_COUNT must be positive");

    /// @brief Compile-time overflow check: bin width calculation must not overflow
    static_assert(static_cast<uint64_t>(MAX_SLICE_BANDWIDTH_HZ) * INV_BIN_COUNT_Q16 <= UINT64_MAX,
                  "Bin width calculation must not overflow");
}

// Spectral Analysis Result
struct SpectralAnalysisResult {
    SignalSignature signature = SignalSignature::NOISE;  ///< Signal type
    uint8_t max_val = 0;                             ///< Maximum bin value
    uint8_t avg_val = 0;                             ///< Average bin value
    uint8_t noise_floor = 0;                          ///< Noise floor level
    uint8_t snr = 0;                                 ///< Signal-to-noise ratio (dB)
    uint32_t signal_width_hz = 0;                      ///< Signal width (Hz)
    uint8_t width_bins = 0;                            ///< Signal width (bins)
    bool is_valid = false;                              ///< Result validity flag
};

// Spectral Analysis Parameters
struct SpectralAnalysisParams {
    uint32_t slice_bandwidth_hz;  ///< Slice bandwidth (Hz)
    Frequency center_freq_hz;       ///< Center frequency (Hz)
};

// Spectral Analyzer (Stack-Optimized)
class SpectralAnalyzer {
public:
    /// @brief Stack-safe histogram buffer (caller-allocated)
    using HistogramBuffer = std::array<uint16_t, SpectralAnalysisConstants::HISTOGRAM_BINS>;

    SpectralAnalyzer() = default;
    ~SpectralAnalyzer() = default;

    SpectralAnalyzer(const SpectralAnalyzer&) = delete;
    SpectralAnalyzer& operator=(const SpectralAnalyzer&) = delete;
    SpectralAnalyzer(SpectralAnalyzer&&) = delete;
    SpectralAnalyzer& operator=(SpectralAnalyzer&&) = delete;

    /**
     * @brief Analyze spectrum (caller provides histogram buffer)
     * @note Stack-safe: histogram buffer allocated by caller
     */
    static SpectralAnalysisResult analyze(
        const std::array<uint8_t, 256>& db_buffer,
        const SpectralAnalysisParams& params,
        HistogramBuffer& histogram_buffer) noexcept;

    /// @brief Get threat level from signature and SNR
    static ThreatLevel get_threat_level(SignalSignature signature, uint8_t snr) noexcept;

    /// @brief Get drone type from frequency and signature
    static DroneType get_drone_type(Frequency frequency_hz, SignalSignature signature) noexcept;

private:
    /// @brief Classify signal based on width and frequency
    static inline SignalSignature classify_signal(uint32_t width_hz, Frequency freq_hz) noexcept;

    /**
     * @brief Fixed-point division helper (eliminates runtime division)
     */
    static inline uint32_t fixed_point_divide(uint32_t numerator,
                                            uint32_t inv_denominator_q16) noexcept {
        // (numerator * inv_denominator) >> 16
        return static_cast<uint32_t>((static_cast<uint64_t>(numerator) * inv_denominator_q16) >> 16);
    }
};

// Implementation: analyze()
inline SpectralAnalysisResult SpectralAnalyzer::analyze(
    const std::array<uint8_t, 256>& db_buffer,
    const SpectralAnalysisParams& params,
    HistogramBuffer& histogram_buffer) noexcept {

    SpectralAnalysisResult result{};

    // Guard clause: invalid bin count
    if (SpectralAnalysisConstants::VALID_BIN_COUNT == 0) {
        result.is_valid = false;
        return result;
    }

    // Clear histogram (caller-allocated buffer)
    histogram_buffer.fill(0);

    uint32_t sum = 0;

    // Build histogram and compute sum
    for (size_t i = SpectralAnalysisConstants::VALID_BIN_START;
         i < SpectralAnalysisConstants::VALID_BIN_END; ++i) {
        const uint8_t value = db_buffer[i];
        histogram_buffer[value / SpectralAnalysisConstants::HISTOGRAM_BIN_DIVISOR]++;
        sum += value;
    }

    // Compute median (O(N) using histogram)
    constexpr size_t median_target = SpectralAnalysisConstants::VALID_BIN_COUNT / 2;
    size_t cumulative = 0;
    uint8_t noise_floor = 0;

    for (size_t bin = 0; bin < SpectralAnalysisConstants::HISTOGRAM_BINS; ++bin) {
        cumulative += histogram_buffer[bin];
        if (cumulative > median_target) {
            noise_floor = static_cast<uint8_t>(bin * SpectralAnalysisConstants::HISTOGRAM_BIN_DIVISOR);
            break;
        }
    }

    result.noise_floor = noise_floor;

    // Use fixed-point division instead of runtime division
    uint32_t avg_q16 = fixed_point_divide(sum, SpectralAnalysisConstants::INV_BIN_COUNT_Q16);
    result.avg_val = static_cast<uint8_t>(avg_q16);

    // Find maximum value
    result.max_val = 0;
    for (size_t i = SpectralAnalysisConstants::VALID_BIN_START;
         i < SpectralAnalysisConstants::VALID_BIN_END; ++i) {
        if (db_buffer[i] > result.max_val) {
            result.max_val = db_buffer[i];
        }
    }

    // Compute SNR
    result.snr = (result.max_val > result.noise_floor) ?
                 (result.max_val - result.noise_floor) : 0;

    // Early exit if SNR too low
    if (result.snr < SpectralAnalysisConstants::SNR_THRESHOLD) {
        result.signature = SignalSignature::NOISE;
        result.is_valid = true;
        return result;
    }

    // Compute signal width
    const uint8_t threshold = (result.max_val > SpectralAnalysisConstants::PEAK_THRESHOLD_DB) ?
                              (result.max_val - SpectralAnalysisConstants::PEAK_THRESHOLD_DB) : 0;

    result.width_bins = 0;
    for (size_t i = SpectralAnalysisConstants::VALID_BIN_START;
         i < SpectralAnalysisConstants::VALID_BIN_END; ++i) {
        if (db_buffer[i] >= threshold) {
            result.width_bins++;
        }
    }

    // Use 64-bit intermediate to prevent overflow
    const uint64_t bin_width_calc = static_cast<uint64_t>(params.slice_bandwidth_hz) *
                                     static_cast<uint64_t>(SpectralAnalysisConstants::INV_BIN_COUNT_Q16);
    const uint32_t bin_width_hz = static_cast<uint32_t>(bin_width_calc >> 16);
    result.signal_width_hz = static_cast<uint32_t>(static_cast<uint64_t>(result.width_bins) * bin_width_hz);

    // Classify signal
    result.signature = classify_signal(result.signal_width_hz, params.center_freq_hz);
    result.is_valid = true;

    return result;
}

// Implementation: get_threat_level()
inline ThreatLevel SpectralAnalyzer::get_threat_level(SignalSignature signature, uint8_t snr) noexcept {
    // Use explicit uint8_t cast for type safety
    uint8_t drone_type = 0;
    switch (signature) {
        case SignalSignature::DIGITAL_FPV:
            drone_type = static_cast<uint8_t>(DroneType::FPV_RACING);
            break;
        case SignalSignature::NARROWBAND_DRONE:
            drone_type = static_cast<uint8_t>(DroneType::MAVIC);
            break;
        case SignalSignature::WIDEBAND_WIFI:
            drone_type = static_cast<uint8_t>(DroneType::UNKNOWN);
            break;
        case SignalSignature::NOISE:
        default:
            return ThreatLevel::NONE;
    }
    return static_cast<ThreatLevel>(ThreatClassifier::from_snr_and_type(snr, drone_type));
}

// Implementation: get_drone_type()
inline DroneType SpectralAnalyzer::get_drone_type(Frequency frequency_hz, SignalSignature signature) noexcept {
    // Guard clause: only drone signatures have specific types
    if (signature != SignalSignature::NARROWBAND_DRONE &&
        signature != SignalSignature::DIGITAL_FPV) {
        return DroneType::UNKNOWN;
    }
    return DroneTypeDetector::from_frequency(frequency_hz);
}

// Implementation: classify_signal()
inline SignalSignature SpectralAnalyzer::classify_signal(uint32_t width_hz, Frequency freq_hz) noexcept {
    // Add UNKNOWN case for completeness
    if (width_hz >= SpectralAnalysisConstants::WIFI_MIN_WIDTH_HZ) {
        return (freq_hz >= EDA::Constants::BAND_SPLIT_FREQ_5GHZ)
               ? SignalSignature::DIGITAL_FPV
               : SignalSignature::WIDEBAND_WIFI;
    }
    if (width_hz <= SpectralAnalysisConstants::DRONE_MAX_WIDTH_HZ) {
        return SignalSignature::NARROWBAND_DRONE;
    }
    // Return NOISE for ambiguous signals
    return SignalSignature::NOISE;
}

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SPECTRAL_ANALYZER_HPP_
