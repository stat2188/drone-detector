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
    WIDEBAND_WIFI,  // Широкое "плато" (> 5-10 МГц)
    NARROWBAND_DRONE, // Узкий пик (< 2-3 МГц)
    DIGITAL_FPV      // Цифровое FPV видео (DJI O3, Vista и др.)
};

// Spectral analysis configuration
struct SpectralAnalysisConfig {
    static constexpr uint8_t SNR_THRESHOLD = 10;        // Сигнал должен быть на 10dB выше шума
    static constexpr uint8_t PEAK_THRESHOLD_DB = 6;     // Порог для измерения ширины (Пик - 6dB)
    static constexpr uint32_t DRONE_MAX_WIDTH_HZ = 2500000; // ~2.5 MHz для дронов
    static constexpr uint32_t WIFI_MIN_WIDTH_HZ = 10000000;  // ~10 MHz для WiFi
    static constexpr size_t VALID_BIN_START = 8;        // Пропускаем первые бины (DC и края)
    static constexpr size_t VALID_BIN_END = 240;        // Пропускаем последние бины
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

// Main spectral analyzer class
class SpectralAnalyzer {
public:
    static SpectralAnalysisResult analyze(const std::array<uint8_t, 256>& db_buffer, 
                                        uint32_t slice_bandwidth_hz,
                                        Frequency center_freq_hz) {
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
        // total_valid_bins всегда > 0 (константа 232)
        uint32_t bin_width_hz = slice_bandwidth_hz / total_valid_bins;
        result.signal_width_hz = result.width_bins * bin_width_hz;

        // 7. Classify signal based on width and context
        if (result.signal_width_hz >= SpectralAnalysisConfig::WIFI_MIN_WIDTH_HZ) { // Ширина > 10 MHz
            // Контекстный анализ по диапазону частот
            if (center_freq_hz >= 5000000000ULL) {
                // На 5.8 ГГц широкий сигнал с высокой вероятностью является цифровым FPV (DJI O3, Vista)
                // WiFi на 5.8 тоже есть, но в полевых условиях это чаще дрон
                result.signature = SignalSignature::DIGITAL_FPV;
            } else {
                // На 2.4 ГГц это скорее всего WiFi (хотя DJI O3 тоже бывает, но WiFi чаще)
                result.signature = SignalSignature::WIDEBAND_WIFI;
            }
        // cppcheck-suppress branch-clone
        // Note: "Grey zone" 3-10 MHz is intentionally classified as drone for safety
        } else if (result.signal_width_hz <= SpectralAnalysisConfig::DRONE_MAX_WIDTH_HZ) {
            result.signature = SignalSignature::NARROWBAND_DRONE; // Аналог, ELRS, TBS
        } else {
            // "Серая зона" 3-10 МГц. Часто это плохое аналоговое видео. Считаем дроном для безопасности.
            result.signature = SignalSignature::NARROWBAND_DRONE; 
        }

        result.is_valid = true;
        return result;
    }

    // Helper method to get threat level from signal signature
    static ThreatLevel get_threat_level(SignalSignature signature, uint8_t snr) {
        switch (signature) {
            case SignalSignature::DIGITAL_FPV:
                // Цифра передает HD видео, это современный и опасный дрон
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
