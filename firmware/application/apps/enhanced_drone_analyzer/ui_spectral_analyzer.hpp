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
#include "ui_drone_common_types.hpp"
#include "radio.hpp"
#include "eda_optimized_utils.hpp"

using rf::Frequency;

namespace ui::apps::enhanced_drone_analyzer {

// Signal classification results
enum class SignalSignature : uint8_t {
    NOISE,
    WIDEBAND_WIFI,   // Wide "plateau" (> 5-10 MHz)
    NARROWBAND_DRONE, // Narrow peak (< 2-3 MHz)
    DIGITAL_FPV      // Digital FPV video (DJI O3, Vista, etc.)
};

// Spectral analysis configuration
struct SpectralAnalysisConfig {
    static constexpr uint8_t SNR_THRESHOLD = 10;         
    static constexpr uint8_t PEAK_THRESHOLD_DB = 6;      
    static constexpr uint32_t DRONE_MAX_WIDTH_HZ = 2500000;  
    static constexpr uint32_t WIFI_MIN_WIDTH_HZ = 10000000;  
    static constexpr size_t VALID_BIN_START = 8;         
    static constexpr size_t VALID_BIN_END = 240;         
    static constexpr size_t VALID_BIN_COUNT = VALID_BIN_END - VALID_BIN_START;
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
    static SpectralAnalysisResult analyze(const std::array<uint8_t, 256>& db_buffer,
                                          const SpectralAnalysisParams& params) {
        uint32_t slice_bandwidth_hz = params.slice_bandwidth_hz;
        Frequency center_freq_hz = params.center_freq_hz;
        SpectralAnalysisResult result;

        if (SpectralAnalysisConfig::VALID_BIN_COUNT == 0) {
            result.is_valid = false;
            return result;
        }

        // 1. Calculate noise floor using 128-bin histogram (O(N) time, 256 bytes stack)
        // Maps uint8_t values [0-255] to histogram bins [0-127] as bin = value/2
        constexpr size_t HISTOGRAM_BINS = 128;
        uint16_t histogram[HISTOGRAM_BINS] = {0};
        uint32_t sum = 0;

        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START;
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            uint8_t value = db_buffer[i];
            histogram[value / 2]++;
            sum += value;
        }

        // Find median: count up to half of total bins
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

        // 2. Find maximum peak
        result.max_val = 0;
        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START; 
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            if (db_buffer[i] > result.max_val) {
                result.max_val = db_buffer[i];
            }
        }

        // 3. Calculate SNR
        result.snr = (result.max_val > result.noise_floor) ? 
                     (result.max_val - result.noise_floor) : 0;

        // 4. Early exit for noise
        if (result.snr < SpectralAnalysisConfig::SNR_THRESHOLD) {
            result.signature = SignalSignature::NOISE;
            result.is_valid = true;
            return result;
        }

        // 5. Calculate signal width
        uint8_t threshold = (result.max_val > SpectralAnalysisConfig::PEAK_THRESHOLD_DB) ?
                           (result.max_val - SpectralAnalysisConfig::PEAK_THRESHOLD_DB) : 0;
        
        result.width_bins = 0;
        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START; 
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            if (db_buffer[i] >= threshold) {
                result.width_bins++;
            }
        }

        // 6. Convert bins to Hz
        uint32_t bin_width_hz = slice_bandwidth_hz / SpectralAnalysisConfig::VALID_BIN_COUNT;
        result.signal_width_hz = result.width_bins * bin_width_hz;

        // 7. Classify signal based on width and context
        result.signature = classify_signal(result.signal_width_hz, center_freq_hz);

        result.is_valid = true;
        return result;
    }

    // DIAMOND OPTIMIZATION: Delegation to unified ThreatClassifier from eda_optimized_utils.hpp
    static ThreatLevel get_threat_level(SignalSignature signature, uint8_t snr) {
        // Map SignalSignature to DroneType for unified classification
        uint8_t drone_type = 0;
        switch (signature) {
            case SignalSignature::DIGITAL_FPV:
                drone_type = static_cast<uint8_t>(DroneType::FPV_RACING);
                break;
            case SignalSignature::NARROWBAND_DRONE:
                drone_type = static_cast<uint8_t>(DroneType::MAVIC); // Default type for narrowband
                break;
            case SignalSignature::WIDEBAND_WIFI:
                drone_type = static_cast<uint8_t>(DroneType::UNKNOWN);
                break;
            case SignalSignature::NOISE:
            default:
                return ThreatLevel::NONE;
        }
        // Use unified ThreatClassifier from eda_optimized_utils.hpp
        return static_cast<ThreatLevel>(ThreatClassifier::from_snr_and_type(snr, drone_type));
    }

    // DIAMOND OPTIMIZATION: Delegation to unified DroneTypeDetector from eda_optimized_utils.hpp
    static DroneType get_drone_type(Frequency frequency_hz, SignalSignature signature) {
        // For non-drone signatures, return UNKNOWN early
        if (signature != SignalSignature::NARROWBAND_DRONE && 
            signature != SignalSignature::DIGITAL_FPV) {
            return DroneType::UNKNOWN;
        }
        // Use unified DroneTypeDetector from eda_optimized_utils.hpp
        // Returns uint8_t, cast to DroneType enum
        uint8_t type_code = DroneTypeDetector::from_frequency(frequency_hz);
        return static_cast<DroneType>(type_code);
    }

private:
    static SignalSignature classify_signal(uint32_t width_hz, Frequency freq_hz) noexcept {
        if (width_hz >= SpectralAnalysisConfig::WIFI_MIN_WIDTH_HZ) {
            return (freq_hz >= DroneConstants::BAND_SPLIT_FREQ_5GHZ) 
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
