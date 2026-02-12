// ui_spectral_analyzer.hpp - Spectral Analysis for Enhanced Drone Analyzer
// Implements intelligent signal classification using M0 FFT data
//
// DIAMOND OPTIMIZATION: FastMedianFilter replaced with MedianFilter<uint8_t>
// from eda_optimized_utils.hpp to eliminate code duplication

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
    static constexpr uint8_t SNR_THRESHOLD = 10;         // Signal must be 10dB above noise
    static constexpr uint8_t PEAK_THRESHOLD_DB = 6;      // Threshold for width measurement (Peak - 6dB)
    static constexpr uint32_t DRONE_MAX_WIDTH_HZ = 2500000;  // ~2.5 MHz for drones
    static constexpr uint32_t WIFI_MIN_WIDTH_HZ = 10000000;  // ~10 MHz for WiFi
    static constexpr size_t VALID_BIN_START = 8;         // Skip first bins (DC and edges)
    static constexpr size_t VALID_BIN_END = 240;         // Skip last bins
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

// DIAMOND OPTIMIZATION: Using unified MedianFilter template
// Eliminates duplicate median filter implementation (~40 lines removed)
// Scott Meyers Item 22: Declare data members private
using FastMedianFilter = MedianFilter<uint8_t, 11>;

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
        
        // 1. Calculate noise floor using median filter
        FastMedianFilter median_filter;
        uint32_t sum = 0;
        size_t valid_bins = 0;
        
        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START; 
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            median_filter.add(db_buffer[i]);
            sum += db_buffer[i];
            valid_bins++;
        }
        
        if (valid_bins == 0) {
            result.is_valid = false;
            return result;
        }

        result.noise_floor = median_filter.get_median();
        result.avg_val = sum / valid_bins;

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
        uint32_t total_valid_bins = SpectralAnalysisConfig::VALID_BIN_END -
                                   SpectralAnalysisConfig::VALID_BIN_START;
        // total_valid_bins always > 0 (constant 232)
        uint32_t bin_width_hz = slice_bandwidth_hz / total_valid_bins;
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
