// ui_spectral_analyzer.hpp - Spectral Analysis for Enhanced Drone Analyzer
// Implements intelligent signal classification using M0 FFT data

#ifndef UI_SPECTRAL_ANALYZER_HPP_
#define UI_SPECTRAL_ANALYZER_HPP_

#include <cstdint>
#include <array>
#include <algorithm>
#include "ui_drone_common_types.hpp"

using Frequency = uint64_t;

namespace ui::apps::enhanced_drone_analyzer {

// Signal classification results
enum class SignalSignature {
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

// Fast median calculation for noise floor estimation
class FastMedianFilter {
private:
    static constexpr size_t WINDOW_SIZE = 11;
    std::array<uint8_t, WINDOW_SIZE> window_{};
    size_t head_ = 0;
    bool full_ = false;

public:
    void add_sample(uint8_t value) {
        window_[head_] = value;
        head_ = (head_ + 1) % WINDOW_SIZE;
        if (head_ == 0) full_ = true;
    }

    uint8_t get_median() const {
        if (!full_) return 0;
        
        // Optimized partial bubble sort - reduces comparisons by skipping sorted tail
        std::array<uint8_t, WINDOW_SIZE> temp = window_;
        for (size_t i = 0; i < WINDOW_SIZE / 2 + 1; ++i) {
            for (size_t j = 0; j < WINDOW_SIZE - i - 1; ++j) {
                if (temp[j] > temp[j + 1]) {
                    std::swap(temp[j], temp[j + 1]);
                }
            }
        }
        return temp[WINDOW_SIZE / 2];
    }

    void reset() {
        full_ = false;
        head_ = 0;
        window_ = {};
    }
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
        
        // 1. Calculate noise floor using median filter
        FastMedianFilter median_filter;
        uint32_t sum = 0;
        size_t valid_bins = 0;
        
        for (size_t i = SpectralAnalysisConfig::VALID_BIN_START; 
             i < SpectralAnalysisConfig::VALID_BIN_END; i++) {
            median_filter.add_sample(db_buffer[i]);
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
        if (result.signal_width_hz >= SpectralAnalysisConfig::WIFI_MIN_WIDTH_HZ) {  // Width > 10 MHz
            // Context analysis based on frequency range
            if (center_freq_hz >= 5000000000ULL) {
            // On 5.8 GHz wide signal is likely digital FPV (DJI O3, Vista)
            // WiFi on 5.8 also exists, but in field conditions it's more likely a drone
            result.signature = SignalSignature::DIGITAL_FPV;
        } else {
            // On 2.4 GHz this is more likely WiFi (though DJI O3 also exists, but WiFi is more common)
            result.signature = SignalSignature::WIDEBAND_WIFI;
        }
        // NOLINTNEXTLINE(bugprone-branch-clone)
        // Note: "Grey zone" 3-10 MHz is intentionally classified as drone for safety
        } else if (result.signal_width_hz <= SpectralAnalysisConfig::DRONE_MAX_WIDTH_HZ) {
            result.signature = SignalSignature::NARROWBAND_DRONE;  // Analog, ELRS, TBS
        } else {
            // "Grey zone" 3-10 MHz. Often poor analog video. Count as drone for safety.
            result.signature = SignalSignature::NARROWBAND_DRONE;
        }

        result.is_valid = true;
        return result;
    }

    // Helper method to get threat level from signal signature
    static ThreatLevel get_threat_level(SignalSignature signature, uint8_t snr) {
        switch (signature) {
            case SignalSignature::DIGITAL_FPV:
                // Digital transmits HD video, this is a modern and dangerous drone
                if (snr >= 15) return ThreatLevel::CRITICAL;
                if (snr >= 5)  return ThreatLevel::HIGH;
                return ThreatLevel::MEDIUM;

            case SignalSignature::NARROWBAND_DRONE:
                if (snr >= 20) return ThreatLevel::CRITICAL;
                if (snr >= 15) return ThreatLevel::HIGH;
                if (snr >= 10) return ThreatLevel::MEDIUM;
                return ThreatLevel::LOW;
                
            case SignalSignature::WIDEBAND_WIFI:
                // WiFi signals are usually not threats, but could indicate drone video
                if (snr >= 25) return ThreatLevel::MEDIUM;
                if (snr >= 15) return ThreatLevel::LOW;
                return ThreatLevel::NONE;
                
            case SignalSignature::NOISE:
            default:
                return ThreatLevel::NONE;
        }
    }

    // Helper method to get drone type from frequency and signal characteristics
    static DroneType get_drone_type(Frequency frequency_hz, SignalSignature signature) {
        if (signature != SignalSignature::NARROWBAND_DRONE && 
            signature != SignalSignature::DIGITAL_FPV) {
            return DroneType::UNKNOWN;
        }

        // For digital FPV signals, map to appropriate drone type
        if (signature == SignalSignature::DIGITAL_FPV) {
            if (frequency_hz >= 5725000000ULL && frequency_hz <= 5875000000ULL) {
                return DroneType::FPV_RACING; // DJI O3, Vista, etc.
            }
        }

        // Frequency-based drone type detection for narrowband signals
        if (frequency_hz >= 2400000000ULL && frequency_hz <= 2483500000ULL) {
            return DroneType::MAVIC; // DJI typically uses 2.4GHz
        } else if (frequency_hz >= 5725000000ULL && frequency_hz <= 5875000000ULL) {
            return DroneType::FPV_RACING; // 5.8GHz is common for FPV
        } else if (frequency_hz >= 860000000ULL && frequency_hz <= 930000000ULL) {
            return DroneType::MILITARY_DRONE; // Long range systems
        } else if (frequency_hz >= 433000000ULL && frequency_hz <= 435000000ULL) {
            return DroneType::DIY_DRONE; // 433MHz ISM band
        }

        return DroneType::UNKNOWN;
    }
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __UI_SPECTRAL_ANALYZER_HPP__
