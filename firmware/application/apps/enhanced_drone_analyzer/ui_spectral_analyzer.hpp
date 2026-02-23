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
    NOISE = 0,
    WIDEBAND_WIFI = 1,
    NARROWBAND_DRONE = 2,
    DIGITAL_FPV = 3
};

using Signature = SignalSignature;
using Threat = ThreatLevel;
using Drone = DroneType;

// Constants (Flash-Resident)
namespace SpectralAnalysisConstants {
    constexpr size_t SPECTRAL_BIN_COUNT = 256;
    constexpr size_t VALID_BIN_START = 8;
    constexpr size_t VALID_BIN_END = 240;
    constexpr size_t VALID_BIN_COUNT = VALID_BIN_END - VALID_BIN_START;
    constexpr uint8_t SNR_THRESHOLD = 10;
    constexpr uint8_t PEAK_THRESHOLD_DB = 6;
    constexpr uint32_t DRONE_MAX_WIDTH_HZ = 2'500'000;
    constexpr uint32_t WIFI_MIN_WIDTH_HZ = 10'000'000;
    constexpr uint32_t MAX_SLICE_BANDWIDTH_HZ = 28'000'000;
    constexpr uint32_t INV_BIN_COUNT_Q16 = (65536 + VALID_BIN_COUNT / 2) / VALID_BIN_COUNT;
    constexpr uint8_t HISTOGRAM_BIN_DIVISOR = 4;
    constexpr size_t HISTOGRAM_BINS = 64;
    constexpr uint32_t Q16_FIXED_POINT_SCALE = 65536;

    static_assert(VALID_BIN_COUNT > 0, "VALID_BIN_COUNT must be positive");
    static_assert(static_cast<uint64_t>(MAX_SLICE_BANDWIDTH_HZ) * INV_BIN_COUNT_Q16 <= UINT64_MAX,
                  "Bin width calculation must not overflow");
}

// Spectral Analysis Result
struct SpectralAnalysisResult {
    SignalSignature signature = SignalSignature::NOISE;
    uint8_t max_val = 0;
    uint8_t avg_val = 0;
    uint8_t noise_floor = 0;
    uint8_t snr = 0;
    uint32_t signal_width_hz = 0;
    uint8_t width_bins = 0;
    bool is_valid = false;
};

// Spectral Analysis Parameters
struct SpectralAnalysisParams {
    uint32_t slice_bandwidth_hz;
    Frequency center_freq_hz;
};

// Spectral Analyzer (Stack-Optimized)
class SpectralAnalyzer {
public:
    using HistogramBuffer = std::array<uint16_t, SpectralAnalysisConstants::HISTOGRAM_BINS>;

    SpectralAnalyzer() = default;
    ~SpectralAnalyzer() = default;

    SpectralAnalyzer(const SpectralAnalyzer&) = delete;
    SpectralAnalyzer& operator=(const SpectralAnalyzer&) = delete;
    SpectralAnalyzer(SpectralAnalyzer&&) = delete;
    SpectralAnalyzer& operator=(SpectralAnalyzer&&) = delete;

    static SpectralAnalysisResult analyze(
        const std::array<uint8_t, 256>& db_buffer,
        const SpectralAnalysisParams& params,
        HistogramBuffer& histogram_buffer) noexcept;

    static ThreatLevel get_threat_level(SignalSignature signature, uint8_t snr) noexcept;
    static DroneType get_drone_type(Frequency frequency_hz, SignalSignature signature) noexcept;

private:
    static inline SignalSignature classify_signal(uint32_t width_hz, Frequency freq_hz) noexcept;

    static inline uint32_t fixed_point_divide(uint32_t numerator,
                                            uint32_t inv_denominator_q16) noexcept {
        return static_cast<uint32_t>((static_cast<uint64_t>(numerator) * inv_denominator_q16) >> 16);
    }
};

// Implementation: analyze()
inline SpectralAnalysisResult SpectralAnalyzer::analyze(
    const std::array<uint8_t, 256>& db_buffer,
    const SpectralAnalysisParams& params,
    HistogramBuffer& histogram_buffer) noexcept {

    SpectralAnalysisResult result{};

    if (SpectralAnalysisConstants::VALID_BIN_COUNT == 0) {
        result.is_valid = false;
        return result;
    }

    histogram_buffer.fill(0);
    uint32_t sum = 0;

    for (size_t i = SpectralAnalysisConstants::VALID_BIN_START;
         i < SpectralAnalysisConstants::VALID_BIN_END; ++i) {
        const uint8_t value = db_buffer[i];
        histogram_buffer[value / SpectralAnalysisConstants::HISTOGRAM_BIN_DIVISOR]++;
        sum += value;
    }

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

    uint32_t avg_q16 = fixed_point_divide(sum, SpectralAnalysisConstants::INV_BIN_COUNT_Q16);
    result.avg_val = static_cast<uint8_t>(avg_q16);

    result.max_val = 0;
    for (size_t i = SpectralAnalysisConstants::VALID_BIN_START;
         i < SpectralAnalysisConstants::VALID_BIN_END; ++i) {
        if (db_buffer[i] > result.max_val) {
            result.max_val = db_buffer[i];
        }
    }

    result.snr = (result.max_val > result.noise_floor) ?
                 (result.max_val - result.noise_floor) : 0;

    if (result.snr < SpectralAnalysisConstants::SNR_THRESHOLD) {
        result.signature = SignalSignature::NOISE;
        result.is_valid = true;
        return result;
    }

    const uint8_t threshold = (result.max_val > SpectralAnalysisConstants::PEAK_THRESHOLD_DB) ?
                              (result.max_val - SpectralAnalysisConstants::PEAK_THRESHOLD_DB) : 0;

    result.width_bins = 0;
    for (size_t i = SpectralAnalysisConstants::VALID_BIN_START;
         i < SpectralAnalysisConstants::VALID_BIN_END; ++i) {
        if (db_buffer[i] >= threshold) {
            result.width_bins++;
        }
    }

    const uint64_t bin_width_calc = static_cast<uint64_t>(params.slice_bandwidth_hz) *
                                     static_cast<uint64_t>(SpectralAnalysisConstants::INV_BIN_COUNT_Q16);
    const uint32_t bin_width_hz = static_cast<uint32_t>(bin_width_calc >> 16);
    result.signal_width_hz = static_cast<uint32_t>(static_cast<uint64_t>(result.width_bins) * bin_width_hz);

    result.signature = classify_signal(result.signal_width_hz, params.center_freq_hz);
    result.is_valid = true;

    return result;
}

// Implementation: get_threat_level()
inline ThreatLevel SpectralAnalyzer::get_threat_level(SignalSignature signature, uint8_t snr) noexcept {
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
    if (signature != SignalSignature::NARROWBAND_DRONE &&
        signature != SignalSignature::DIGITAL_FPV) {
        return DroneType::UNKNOWN;
    }
    return DroneTypeDetector::from_frequency(frequency_hz);
}

// Implementation: classify_signal()
inline SignalSignature SpectralAnalyzer::classify_signal(uint32_t width_hz, Frequency freq_hz) noexcept {
    if (width_hz >= SpectralAnalysisConstants::WIFI_MIN_WIDTH_HZ) {
        return (freq_hz >= EDA::Constants::BAND_SPLIT_FREQ_5GHZ)
               ? SignalSignature::DIGITAL_FPV
               : SignalSignature::WIDEBAND_WIFI;
    }
    if (width_hz <= SpectralAnalysisConstants::DRONE_MAX_WIDTH_HZ) {
        return SignalSignature::NARROWBAND_DRONE;
    }
    return SignalSignature::NOISE;
}

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SPECTRAL_ANALYZER_HPP_
