#ifndef EDA_CONSTANTS_HPP_
#define EDA_CONSTANTS_HPP_

#include <cstdint>
#include "rf_path.hpp"

// Flash storage attribute for Cortex-M4
#ifdef __GNUC__
    #define EDA_FLASH_CONST __attribute__((section(".rodata")))
#else
    #define EDA_FLASH_CONST const
#endif

namespace EDA {

using Frequency = rf::Frequency;

namespace Constants {

// ===== FREQUENCY LIMITS =====
namespace FrequencyLimits {
    using Frequency = rf::Frequency;
    
    constexpr Frequency MIN_HARDWARE_FREQ =     1'000'000ULL; // 1 MHz
    constexpr Frequency MAX_HARDWARE_FREQ =  7'200'000'000ULL; // 7.2 GHz (hardware maximum)

    constexpr Frequency MIN_SAFE_FREQ =        50'000'000ULL; // 50 MHz (PLL stability)
    constexpr Frequency MAX_SAFE_FREQ =     6'000'000'000ULL; // 6 GHz (PLL stability)
}

// ===== SCANNING MODES =====
enum class ScanningMode : uint8_t {
    STRICT_DRONE = 0,
    FULL_SPECTRUM = 1
};

// ===== FREQUENCY RANGES =====
using Frequency = rf::Frequency;

constexpr Frequency MIN_HARDWARE_FREQ = FrequencyLimits::MIN_HARDWARE_FREQ;
constexpr Frequency MAX_HARDWARE_FREQ = FrequencyLimits::MAX_HARDWARE_FREQ;

constexpr Frequency MIN_433MHZ = 433'000'000ULL;
constexpr Frequency MAX_433MHZ = 435'000'000ULL;

constexpr Frequency MIN_900MHZ = 860'000'000ULL;
constexpr Frequency MAX_900MHZ = 930'000'000ULL;

constexpr Frequency MIN_24GHZ = 2'400'000'000ULL;
constexpr Frequency MAX_24GHZ = 2'483'500'000ULL;
constexpr Frequency DEFAULT_24GHZ_CENTER = 2'450'000'000ULL;
constexpr Frequency WIDEBAND_24GHZ_MIN = 2'400'000'000ULL;
constexpr Frequency WIDEBAND_24GHZ_MAX = 2'500'000'000ULL;

constexpr Frequency MIN_58GHZ = 5'725'000'000ULL;
constexpr Frequency MAX_58GHZ = 5'875'000'000ULL;
constexpr Frequency DEFAULT_58GHZ_CENTER = 5'800'000'000ULL;

// ===== FREQUENCY THRESHOLDS =====
constexpr Frequency BAND_SPLIT_FREQ_5GHZ = 5'000'000'000ULL;

// ===== SIGNAL WIDTHS =====
constexpr uint32_t NARROWBAND_DRONE_MAX_WIDTH_HZ = 2'500'000ULL;
constexpr uint32_t WIDEBAND_WIFI_MIN_WIDTH_HZ = 10'000'000ULL;
constexpr uint32_t WIDEBAND_DEFAULT_SLICE_WIDTH = 22'000'000ULL;
constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
constexpr Frequency WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
constexpr Frequency WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;

constexpr uint32_t WIDEBAND_SLICE_WIDTH = WIDEBAND_DEFAULT_SLICE_WIDTH;

// ===== RSSI THRESHOLDS =====
constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -80;
constexpr int32_t CRITICAL_RSSI_DB = -50;
constexpr int32_t HIGH_RSSI_DB = -60;
constexpr int32_t MEDIUM_RSSI_DB = -70;
constexpr int32_t LOW_RSSI_DB = -80;

constexpr int32_t NOISE_FLOOR_RSSI = -110;
constexpr int32_t MIN_VALID_RSSI = -110;
constexpr int32_t MAX_VALID_RSSI = 10;

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

// ===== ADAPTIVE SCAN INTERVALS =====
constexpr uint32_t FAST_SCAN_INTERVAL_MS = 250;
constexpr uint32_t NORMAL_SCAN_INTERVAL_MS = 750;
constexpr uint32_t SLOW_SCAN_INTERVAL_MS = 1000;
constexpr uint32_t VERY_SLOW_SCAN_INTERVAL_MS = 2000;

// ===== INTELLIGENT SCANNING =====
constexpr uint32_t PRIORITY_SLICE_SKIP_THRESHOLD = 10;
constexpr uint32_t PREDICTION_FRESHNESS_MS = 5000;
constexpr uint32_t FHSS_TRACKING_CONFIDENCE_MAX = 10;

// ===== MEMORY =====
constexpr size_t MAX_TRACKED_DRONES = 8;
constexpr size_t MAX_DISPLAYED_DRONES = 3;
constexpr size_t DETECTION_TABLE_SIZE = 256;
constexpr size_t MINI_SPECTRUM_WIDTH = 200;
constexpr size_t MINI_SPECTRUM_HEIGHT = 24;

// ===== AUDIO ALERT PARAMETERS =====
constexpr uint32_t MIN_AUDIO_FREQ = 200;
constexpr uint32_t MAX_AUDIO_FREQ = 20000;

// ===== AUDIO DURATION PARAMETERS =====
constexpr uint32_t MIN_AUDIO_DURATION = 50;
constexpr uint32_t MAX_AUDIO_DURATION = 5000;

// ===== BANDWIDTH PARAMETERS =====
constexpr uint32_t MIN_BANDWIDTH = 10000;
constexpr uint32_t MAX_BANDWIDTH = 28000000;

} // namespace Constants

// ===========================================
// THREAT LEVEL DATA (Removed - Use UnifiedStringLookup/UnifiedColorLookup from color_lookup_unified.hpp)
// ===========================================

// ===========================================
// DRONE TYPE DATA (Removed - Use UnifiedStringLookup/UnifiedColorLookup from color_lookup_unified.hpp)
// ===========================================

// ===========================================
// DRONE DATABASE CONTENT (Removed - Use default_drones_db.hpp if needed)
// ===========================================

// ===========================================
// VALIDATION UTILITIES (from diamond_core.hpp)
// ===========================================

namespace Validation {

inline constexpr bool validate_frequency(int64_t freq_hz) noexcept {
    return freq_hz >= 1000000LL && freq_hz <= 7200000000LL;
}

inline constexpr bool validate_rssi(int32_t rssi_db) noexcept {
    return rssi_db >= -110 && rssi_db <= 10;
}

inline constexpr bool is_2_4ghz_band(int64_t freq_hz) noexcept {
    return freq_hz >= 2400000000LL && freq_hz <= 2483500000LL;
}

inline constexpr bool is_5_8ghz_band(int64_t freq_hz) noexcept {
    return freq_hz >= 5725000000LL && freq_hz <= 5875000000LL;
}

inline constexpr bool is_military_band(int64_t freq_hz) noexcept {
    return freq_hz >= 860000000LL && freq_hz <= 930000000LL;
}

inline constexpr bool is_433mhz_band(int64_t freq_hz) noexcept {
    return freq_hz >= 433000000LL && freq_hz <= 435000000LL;
}

} // namespace Validation

// ===========================================
// FREQUENCY BANDS (Removed - Use diamond_core.hpp if needed)
// ===========================================

// ===========================================
// SPECTRAL CONFIG (Removed - Use ui_spectral_analyzer.hpp if needed)
// ===========================================

// ===========================================
// RSSI UTILITIES (Removed - Use diamond_core.hpp RSSIUtils if needed)
// ===========================================

// ===========================================
// MOVEMENT TREND UTILITIES (Removed - Use diamond_core.hpp TrendUtils if needed)
// ===========================================

// ===========================================
// COLOR CONVERTER (Removed - Use color_lookup_unified.hpp ColorConverter if needed)
// ===========================================

} // namespace EDA

// Backward compatibility aliases
namespace DroneConstants = EDA::Constants;

#endif // EDA_CONSTANTS_HPP_
