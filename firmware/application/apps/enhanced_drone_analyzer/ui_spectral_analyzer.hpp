// ui_spectral_analyzer.hpp - Spectral Analysis for Enhanced Drone Analyzer
// Implements intelligent signal classification using M0 FFT data
//
// DIAMOND FIX: Rolling median filter replaced with 128-bin histogram noise estimator
// - Correctness: Median computed from ENTIRE spectrum (232 bins), not just tail (11 bins)
// - Performance: O(N) time, 256 bytes stack, O(1) median lookup
// - Removed runtime division: VALID_BIN_COUNT computed at compile time

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

enum class SignalSignature : uint8_t {
    NOISE = 0,
    WIDEBAND_WIFI = 1,
    NARROWBAND_DRONE = 2,
    DIGITAL_FPV = 3
};

using Signature = SignalSignature;
using Threat = ThreatLevel;
using Drone = DroneType;

struct SpectralAnalysisConfig {
    static constexpr uint8_t SNR_THRESHOLD = 10;
    static constexpr uint8_t PEAK_THRESHOLD_DB = 6;
    static constexpr uint32_t DRONE_MAX_WIDTH_HZ = 2500000;
    static constexpr uint32_t WIFI_MIN_WIDTH_HZ = 10000000;
    static constexpr size_t VALID_BIN_START = 8;
    static constexpr size_t VALID_BIN_END = 240;
    static constexpr size_t VALID_BIN_COUNT = VALID_BIN_END - VALID_BIN_START;
    static constexpr uint32_t INV_BIN_COUNT_Q16 = (65536 + VALID_BIN_COUNT / 2) / VALID_BIN_COUNT;
};

// Spectral analysis statistics
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

// Parameters for spectral analysis (prevents easily-swappable-parameters warning)
struct SpectralAnalysisParams {
    uint32_t slice_bandwidth_hz;
    Frequency center_freq_hz;
};

// Main spectral analyzer class
class SpectralAnalyzer {
public:
    SpectralAnalyzer() = default;
    
    SpectralAnalyzer(const SpectralAnalyzer&) = delete;
    SpectralAnalyzer& operator=(const SpectralAnalyzer&) = delete;
    SpectralAnalyzer(SpectralAnalyzer&&) = delete;
    SpectralAnalyzer& operator=(SpectralAnalyzer&&) = delete;
    
    static inline SpectralAnalysisResult analyze(const std::array<uint8_t, 256>& db_buffer,
                                                  const SpectralAnalysisParams& params) noexcept {
        SpectralAnalysisResult result{};

        if (SpectralAnalysisConfig::VALID_BIN_COUNT == 0) {
            result.is_valid = false;
            return result;
        }

        constexpr size_t HISTOGRAM_BINS = 128;

        std::array<uint16_t, HISTOGRAM_BINS> histogram{};
        histogram.fill(0);

        uint32_t sum = 0;

        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START;
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            const uint8_t value = db_buffer[i];
            histogram[value / 2]++;
            sum += value;
        }

        constexpr size_t MEDIAN_TARGET = SpectralAnalysisConfig::VALID_BIN_COUNT / 2;
        size_t cumulative = 0;
        uint8_t noise_floor = 0;

        for (size_t bin = 0; bin < HISTOGRAM_BINS; bin++) {
            cumulative += histogram[bin];
            if (cumulative > MEDIAN_TARGET) {
                noise_floor = static_cast<uint8_t>(bin * 2);
                break;
            }
        }

        result.noise_floor = noise_floor;
        result.avg_val = static_cast<uint8_t>(sum / SpectralAnalysisConfig::VALID_BIN_COUNT);

        result.max_val = 0;
        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START;
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            if (db_buffer[i] > result.max_val) {
                result.max_val = db_buffer[i];
            }
        }

        result.snr = (result.max_val > result.noise_floor) ?
                     (result.max_val - result.noise_floor) : 0;

        if (result.snr < SpectralAnalysisConfig::SNR_THRESHOLD) {
            result.signature = SignalSignature::NOISE;
            result.is_valid = true;
            return result;
        }

        const uint8_t threshold = (result.max_val > SpectralAnalysisConfig::PEAK_THRESHOLD_DB) ?
                                  (result.max_val - SpectralAnalysisConfig::PEAK_THRESHOLD_DB) : 0;

        result.width_bins = 0;
        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START;
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            if (db_buffer[i] >= threshold) {
                result.width_bins++;
            }
        }

        const uint64_t bin_width_calc = static_cast<uint64_t>(params.slice_bandwidth_hz) *
                                         static_cast<uint64_t>(SpectralAnalysisConfig::INV_BIN_COUNT_Q16);
        const uint32_t bin_width_hz = static_cast<uint32_t>(bin_width_calc >> 16);
        result.signal_width_hz = result.width_bins * bin_width_hz;

        result.signature = classify_signal(result.signal_width_hz, params.center_freq_hz);

        result.is_valid = true;
        return result;
    }

    static inline ThreatLevel get_threat_level(const SignalSignature signature, const uint8_t snr) noexcept {
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

    static inline DroneType get_drone_type(const Frequency frequency_hz, const SignalSignature signature) noexcept {
        if (signature != SignalSignature::NARROWBAND_DRONE &&
            signature != SignalSignature::DIGITAL_FPV) {
            return DroneType::UNKNOWN;
        }
        return DroneTypeDetector::from_frequency(frequency_hz);
    }

private:
    static inline SignalSignature classify_signal(const uint32_t width_hz, const Frequency freq_hz) noexcept {
        if (width_hz >= SpectralAnalysisConfig::WIFI_MIN_WIDTH_HZ) {
            return (freq_hz >= EDA::Constants::BAND_SPLIT_FREQ_5GHZ)
                   ? SignalSignature::DIGITAL_FPV
                   : SignalSignature::WIDEBAND_WIFI;
        }
        if (width_hz <= SpectralAnalysisConfig::DRONE_MAX_WIDTH_HZ) {
            return SignalSignature::NARROWBAND_DRONE;
        }
        return SignalSignature::NARROWBAND_DRONE;
    }
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SPECTRAL_ANALYZER_HPP_
