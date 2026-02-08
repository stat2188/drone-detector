#ifndef DRONE_CONSTANTS_HPP_
#define DRONE_CONSTANTS_HPP_

#include <cstdint>
#include "rf_path.hpp"

namespace DroneConstants {

// ===== FREQUENCY LIMITS =====
namespace FrequencyLimits {
    // Absolute hardware limits for HackRF One (MAX2837)
    constexpr rf::Frequency MIN_HARDWARE_FREQ =     1'000'000ULL; // 1 MHz
    constexpr rf::Frequency MAX_HARDWARE_FREQ =  7'200'000'000ULL; // 7.2 GHz (hardware maximum)

    // Safe operational limits to protect PLL and ensure stability
    constexpr rf::Frequency MIN_SAFE_FREQ =        50'000'000ULL; // 50 MHz (PLL stability)
    constexpr rf::Frequency MAX_SAFE_FREQ =     6'000'000'000ULL; // 6 GHz (PLL stability)
}

// ===== SCANNING MODES =====
enum class ScanningMode : uint8_t {
    STRICT_DRONE = 0,    // Only validate known drone frequency bands
    FULL_SPECTRUM = 1    // Accept all frequencies within hardware limits
};

// ===== FREQUENCY RANGES (Legacy compatibility) =====
constexpr rf::Frequency MIN_HARDWARE_FREQ = 1'000'000ULL;
constexpr rf::Frequency MAX_HARDWARE_FREQ = 7'200'000'000ULL;

// 433 MHz ISM band (DIY drones)
constexpr rf::Frequency MIN_433MHZ = 433'000'000ULL;
constexpr rf::Frequency MAX_433MHZ = 435'000'000ULL;

// 860-930 MHz (Long range, Military)
constexpr rf::Frequency MIN_900MHZ = 860'000'000ULL;
constexpr rf::Frequency MAX_900MHZ = 930'000'000ULL;

// 2.4 GHz ISM band (DJI, consumer drones)
constexpr rf::Frequency MIN_24GHZ = 2'400'000'000ULL;
constexpr rf::Frequency MAX_24GHZ = 2'483'500'000ULL;
constexpr rf::Frequency DEFAULT_24GHZ_CENTER = 2'450'000'000ULL;
constexpr rf::Frequency WIDEBAND_24GHZ_MIN = 2'400'000'000ULL;
constexpr rf::Frequency WIDEBAND_24GHZ_MAX = 2'500'000'000ULL;

// 5.8 GHz band (FPV racing, DJI O3)
constexpr rf::Frequency MIN_58GHZ = 5'725'000'000ULL;
constexpr rf::Frequency MAX_58GHZ = 5'875'000'000ULL;
constexpr rf::Frequency DEFAULT_58GHZ_CENTER = 5'800'000'000ULL;

// ===== SIGNAL WIDTHS =====
constexpr uint32_t NARROWBAND_DRONE_MAX_WIDTH_HZ = 2'500'000ULL;
constexpr uint32_t WIDEBAND_WIFI_MIN_WIDTH_HZ = 10'000'000ULL;
constexpr uint32_t WIDEBAND_DEFAULT_SLICE_WIDTH = 22'000'000ULL;
constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
constexpr rf::Frequency WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;  // 2.4 GHz
constexpr rf::Frequency WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;  // 2.5 GHz

// Alias for backward compatibility (Scott Meyers Item 1: View C++ as a federation of languages)
constexpr uint32_t WIDEBAND_SLICE_WIDTH = WIDEBAND_DEFAULT_SLICE_WIDTH;

// ===== RSSI THRESHOLDS =====
constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -80;
constexpr int32_t CRITICAL_RSSI_DB = -50;
constexpr int32_t HIGH_RSSI_DB = -60;
constexpr int32_t MEDIUM_RSSI_DB = -70;
constexpr int32_t LOW_RSSI_DB = -80;

// RSSI Validation thresholds
// RF receiver noise floor is typically -128 to -110 dBm
// Values below -110 dBm are considered pure noise
constexpr int32_t NOISE_FLOOR_RSSI = -110;        // Below this is noise
constexpr int32_t MIN_VALID_RSSI = -110;          // Minimum valid signal level
constexpr int32_t MAX_VALID_RSSI = 10;            // Maximum realistic RSSI

// ===== SPECTRAL ANALYSIS =====
constexpr uint8_t SPECTRAL_SNR_THRESHOLD = 10;
constexpr uint8_t SPECTRAL_PEAK_THRESHOLD_DB = 6;
constexpr size_t SPECTRAL_VALID_BIN_START = 8;
constexpr size_t SPECTRAL_VALID_BIN_END = 240;
constexpr size_t SPECTRAL_BIN_COUNT = 256;

// ===== DETECTION =====
constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;
constexpr uint32_t MAX_SCAN_INTERVAL_MS = 10000;
constexpr uint32_t DEFAULT_SCAN_INTERVAL_MS = 1000;
constexpr uint8_t MIN_DETECTION_COUNT = 3;
constexpr uint32_t ALERT_PERSISTENCE_THRESHOLD = 3;

// ===== ADAPTIVE SCAN INTERVALS (in milliseconds) =====
constexpr uint32_t FAST_SCAN_INTERVAL_MS = 250;
constexpr uint32_t NORMAL_SCAN_INTERVAL_MS = 750;
constexpr uint32_t SLOW_SCAN_INTERVAL_MS = 1000;
constexpr uint32_t VERY_SLOW_SCAN_INTERVAL_MS = 2000;

// ===== INTELLIGENT SCANNING (New constants for Portapack optimization) =====
constexpr uint32_t PRIORITY_SLICE_SKIP_THRESHOLD = 10; // Skip slice if clean for N cycles
constexpr uint32_t PREDICTION_FRESHNESS_MS = 5000;  // 5 seconds
constexpr uint32_t FHSS_TRACKING_CONFIDENCE_MAX = 10;

// ===== MEMORY =====
constexpr size_t MAX_TRACKED_DRONES = 8;
constexpr size_t MAX_DISPLAYED_DRONES = 3;
constexpr size_t DETECTION_TABLE_SIZE = 256;
constexpr size_t MINI_SPECTRUM_WIDTH = 200;
constexpr size_t MINI_SPECTRUM_HEIGHT = 24;

// ===== AUDIO ALERT PARAMETERS =====
constexpr uint32_t MIN_AUDIO_FREQ = 200;       // 200 Hz minimum
constexpr uint32_t MAX_AUDIO_FREQ = 20000;     // 20 kHz maximum

// ===== AUDIO DURATION PARAMETERS =====
constexpr uint32_t MIN_AUDIO_DURATION = 50;    // 50 ms minimum
constexpr uint32_t MAX_AUDIO_DURATION = 5000;  // 5000 ms maximum

// ===== BANDWIDTH PARAMETERS =====
constexpr uint32_t MIN_BANDWIDTH = 10000;     // 10 kHz minimum
constexpr uint32_t MAX_BANDWIDTH = 28000000;  // 28 MHz maximum (HackRF limit)

} // namespace DroneConstants

#endif // DRONE_CONSTANTS_HPP_
