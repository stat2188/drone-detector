#include <iostream>
#include <cassert>
#include <array>
#include <cstdint>

using Frequency = uint64_t;

enum class ThreatLevel {
    NONE = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4,
    UNKNOWN = 5
};

enum class DroneType {
    UNKNOWN = 0,
    MAVIC = 1,
    DJI_P34 = 2,
    PHANTOM = 3,
    DJI_MINI = 4,
    PARROT_ANAFI = 5,
    PARROT_BEBOP = 6,
    PX4_DRONE = 7,
    MILITARY_DRONE = 8,
    DIY_DRONE = 9,
    FPV_RACING = 10
};

enum class SignalSignature {
    NOISE,
    WIDEBAND_WIFI,
    NARROWBAND_DRONE,
    DIGITAL_FPV
};

namespace DroneConstants {
    constexpr uint32_t NARROWBAND_DRONE_MAX_WIDTH_HZ = 2'500'000ULL;
    constexpr uint32_t WIDEBAND_WIFI_MIN_WIDTH_HZ = 10'000'000ULL;
    constexpr uint8_t SPECTRAL_SNR_THRESHOLD = 10;
    constexpr uint8_t SPECTRAL_PEAK_THRESHOLD_DB = 6;
    constexpr size_t SPECTRAL_VALID_BIN_START = 8;
    constexpr size_t SPECTRAL_VALID_BIN_END = 240;
    constexpr Frequency MIN_58GHZ = 5'725'000'000ULL;
    constexpr Frequency MAX_58GHZ = 5'875'000'000ULL;
}

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

struct FastMedianFilter {
private:
    static constexpr size_t WINDOW_SIZE = 11;
    unsigned char window_[WINDOW_SIZE] = {};
    size_t head_ = 0;
    bool full_ = false;

public:
    void add_sample(unsigned char value) {
        window_[head_] = value;
        head_ = (head_ + 1) % WINDOW_SIZE;
        if (head_ == 0) full_ = true;
    }

    unsigned char get_median() const {
        if (!full_) return 0;
        unsigned char temp[WINDOW_SIZE];
        for (size_t i = 0; i < WINDOW_SIZE; i++) {
            temp[i] = window_[i];
        }
        size_t k = WINDOW_SIZE / 2;
        for (size_t i = 0; i <= k; ++i) {
            size_t min_idx = i;
            for (size_t j = i + 1; j < WINDOW_SIZE; ++j) {
                if (temp[j] < temp[min_idx]) {
                    min_idx = j;
                }
            }
            if (min_idx != i) {
                unsigned char tmp = temp[i];
                temp[i] = temp[min_idx];
                temp[min_idx] = tmp;
            }
        }
        return temp[k];
    }

    void reset() {
        full_ = false;
        head_ = 0;
        for (size_t i = 0; i < WINDOW_SIZE; i++) {
            window_[i] = 0;
        }
    }
};

struct SpectralAnalysisParams {
    uint32_t slice_bandwidth_hz;
    Frequency center_freq_hz;
};

class SpectralAnalyzer {
public:
    static SpectralAnalysisResult analyze(const std::array<unsigned char, 256>& db_buffer,
                                          const SpectralAnalysisParams& params) {
        uint32_t slice_bandwidth_hz = params.slice_bandwidth_hz;
        Frequency center_freq_hz = params.center_freq_hz;
        SpectralAnalysisResult result;
        
        FastMedianFilter median_filter;
        uint32_t sum = 0;
        size_t valid_bins = 0;
        
        for (size_t i = DroneConstants::SPECTRAL_VALID_BIN_START; 
             i < DroneConstants::SPECTRAL_VALID_BIN_END; i++) {
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

        result.max_val = 0;
        for (size_t i = DroneConstants::SPECTRAL_VALID_BIN_START; 
             i < DroneConstants::SPECTRAL_VALID_BIN_END; i++) {
            if (db_buffer[i] > result.max_val) {
                result.max_val = db_buffer[i];
            }
        }

        result.snr = (result.max_val > result.noise_floor) ? 
                     (result.max_val - result.noise_floor) : 0;

        if (result.snr < DroneConstants::SPECTRAL_SNR_THRESHOLD) {
            result.signature = SignalSignature::NOISE;
            result.is_valid = true;
            return result;
        }

        unsigned char threshold = (result.max_val > DroneConstants::SPECTRAL_PEAK_THRESHOLD_DB) ?
                           (result.max_val - DroneConstants::SPECTRAL_PEAK_THRESHOLD_DB) : 0;
        
        result.width_bins = 0;
        for (size_t i = DroneConstants::SPECTRAL_VALID_BIN_START; 
             i < DroneConstants::SPECTRAL_VALID_BIN_END; i++) {
            if (db_buffer[i] >= threshold) {
                result.width_bins++;
            }
        }

        uint32_t total_valid_bins = DroneConstants::SPECTRAL_VALID_BIN_END -
                                   DroneConstants::SPECTRAL_VALID_BIN_START;
        uint32_t bin_width_hz = slice_bandwidth_hz / total_valid_bins;
        result.signal_width_hz = result.width_bins * bin_width_hz;

        if (result.signal_width_hz >= DroneConstants::WIDEBAND_WIFI_MIN_WIDTH_HZ) {
            if (center_freq_hz >= 5000000000ULL) {
                result.signature = SignalSignature::DIGITAL_FPV;
            } else {
                result.signature = SignalSignature::WIDEBAND_WIFI;
            }
        } else if (result.signal_width_hz <= DroneConstants::NARROWBAND_DRONE_MAX_WIDTH_HZ) {
            result.signature = SignalSignature::NARROWBAND_DRONE;
        }

        result.is_valid = true;
        return result;
    }

    static ThreatLevel get_threat_level(SignalSignature signature, uint8_t snr) {
        switch (signature) {
            case SignalSignature::DIGITAL_FPV:
                if (snr >= 15) return ThreatLevel::CRITICAL;
                if (snr >= 5)  return ThreatLevel::HIGH;
                return ThreatLevel::MEDIUM;

            case SignalSignature::NARROWBAND_DRONE:
                if (snr >= 20) return ThreatLevel::CRITICAL;
                if (snr >= 15) return ThreatLevel::HIGH;
                if (snr >= 10) return ThreatLevel::MEDIUM;
                return ThreatLevel::LOW;
                
            case SignalSignature::WIDEBAND_WIFI:
                if (snr >= 25) return ThreatLevel::MEDIUM;
                if (snr >= 15) return ThreatLevel::LOW;
                return ThreatLevel::NONE;
                
            case SignalSignature::NOISE:
            default:
                return ThreatLevel::NONE;
        }
    }

    static DroneType get_drone_type(Frequency frequency_hz, SignalSignature signature) {
        if (signature != SignalSignature::NARROWBAND_DRONE && 
            signature != SignalSignature::DIGITAL_FPV) {
            return DroneType::UNKNOWN;
        }

        if (signature == SignalSignature::DIGITAL_FPV) {
            if (frequency_hz >= DroneConstants::MIN_58GHZ && 
                frequency_hz <= DroneConstants::MAX_58GHZ) {
                return DroneType::FPV_RACING;
            }
        }

        if (frequency_hz >= 2400000000ULL && frequency_hz <= 2483500000ULL) {
            return DroneType::MAVIC;
        } else if (frequency_hz >= 5725000000ULL && frequency_hz <= 5875000000ULL) {
            return DroneType::FPV_RACING;
        } else if (frequency_hz >= 860000000ULL && frequency_hz <= 930000000ULL) {
            return DroneType::MILITARY_DRONE;
        } else if (frequency_hz >= 433000000ULL && frequency_hz <= 435000000ULL) {
            return DroneType::DIY_DRONE;
        }

        return DroneType::UNKNOWN;
    }
};

void test_noise_detection() {
    std::array<unsigned char, 256> noise_buffer;
    noise_buffer.fill(30);
    
    SpectralAnalysisParams params{24'000'000, 2'450'000'000ULL};
    
    auto result = SpectralAnalyzer::analyze(noise_buffer, params);
    
    assert(result.signature == SignalSignature::NOISE);
    assert(result.snr < DroneConstants::SPECTRAL_SNR_THRESHOLD);
    std::cout << "PASS: Noise detection" << std::endl;
}

void test_narrowband_drone_detection() {
    std::array<unsigned char, 256> narrow_buffer;
    narrow_buffer.fill(30);
    
    for (size_t i = 100; i < 110; i++) {
        narrow_buffer[i] = 70;
    }
    
    SpectralAnalysisParams params{24'000'000, 2'450'000'000ULL};
    
    auto result = SpectralAnalyzer::analyze(narrow_buffer, params);
    
    assert(result.signature == SignalSignature::NARROWBAND_DRONE);
    assert(result.snr >= DroneConstants::SPECTRAL_SNR_THRESHOLD);
    assert(result.signal_width_hz <= DroneConstants::NARROWBAND_DRONE_MAX_WIDTH_HZ);
    std::cout << "PASS: Narrowband drone detection" << std::endl;
}

void test_wideband_wifi_detection() {
    std::array<unsigned char, 256> wide_buffer;
    wide_buffer.fill(30);
    
    for (size_t i = 50; i < 150; i++) {
        wide_buffer[i] = 70;
    }
    
    SpectralAnalysisParams params{24'000'000, 2'450'000'000ULL};
    
    auto result = SpectralAnalyzer::analyze(wide_buffer, params);
    
    assert(result.signature == SignalSignature::WIDEBAND_WIFI);
    assert(result.signal_width_hz >= DroneConstants::WIDEBAND_WIFI_MIN_WIDTH_HZ);
    std::cout << "PASS: Wideband WiFi detection" << std::endl;
}

void test_digital_fpv_detection() {
    std::array<unsigned char, 256> wide_buffer;
    wide_buffer.fill(30);
    
    for (size_t i = 50; i < 150; i++) {
        wide_buffer[i] = 70;
    }
    
    SpectralAnalysisParams params{24'000'000, 5'800'000'000ULL};
    
    auto result = SpectralAnalyzer::analyze(wide_buffer, params);
    
    assert(result.signature == SignalSignature::DIGITAL_FPV);
    std::cout << "PASS: Digital FPV detection" << std::endl;
}

void test_threat_level_calculation() {
    auto threat = SpectralAnalyzer::get_threat_level(
        SignalSignature::DIGITAL_FPV, 20);
    assert(threat == ThreatLevel::CRITICAL);
    
    threat = SpectralAnalyzer::get_threat_level(
        SignalSignature::NARROWBAND_DRONE, 12);
    assert(threat == ThreatLevel::MEDIUM);
    
    threat = SpectralAnalyzer::get_threat_level(
        SignalSignature::WIDEBAND_WIFI, 10);
    assert(threat == ThreatLevel::NONE);
    
    std::cout << "PASS: Threat level calculation" << std::endl;
}

void test_drone_type_from_frequency() {
    auto type = SpectralAnalyzer::get_drone_type(
        2'450'000'000ULL, SignalSignature::NARROWBAND_DRONE);
    assert(type == DroneType::MAVIC);
    
    type = SpectralAnalyzer::get_drone_type(
        5'800'000'000ULL, SignalSignature::DIGITAL_FPV);
    assert(type == DroneType::FPV_RACING);
    
    type = SpectralAnalyzer::get_drone_type(
        433'500'000ULL, SignalSignature::NARROWBAND_DRONE);
    assert(type == DroneType::DIY_DRONE);
    
    std::cout << "PASS: Drone type from frequency" << std::endl;
}

int main() {
    std::cout << "=== Running SpectralAnalyzer Tests ===" << std::endl;
    
    test_noise_detection();
    test_narrowband_drone_detection();
    test_wideband_wifi_detection();
    test_digital_fpv_detection();
    test_threat_level_calculation();
    test_drone_type_from_frequency();
    
    std::cout << "=== All tests passed! ===" << std::endl;
    return 0;
}
