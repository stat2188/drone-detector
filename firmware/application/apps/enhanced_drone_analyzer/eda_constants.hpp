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
// THREAT LEVEL DATA (Unified from color_lookup_unified.hpp)
// ===========================================

namespace ThreatLevelData {
    
struct Entry {
    uint32_t color_rgb;
    int32_t rssi_threshold_db;
    char symbol;
    const char* name;
};

constexpr Entry TABLE[] EDA_FLASH_CONST = {
    { 0x0000FF,      -120,        '-',    "NONE" },
    { 0x00FF00,      -100,        'i',    "LOW" },
    { 0xFFFF00,      -85,         'O',    "MEDIUM" },
    { 0xFFA500,      -70,         '!',    "HIGH" },
    { 0xFF0000,      -50,         '!',    "CRITICAL" },
    { 0x808080,      -120,        '?',    "UNKNOWN" }
};

constexpr size_t COUNT = sizeof(TABLE) / sizeof(Entry);

inline constexpr uint32_t color(uint8_t level) noexcept {
    return (level < COUNT) ? TABLE[level].color_rgb : TABLE[5].color_rgb;
}

inline constexpr const char* name(uint8_t level) noexcept {
    return (level < COUNT) ? TABLE[level].name : TABLE[5].name;
}

inline constexpr char symbol(uint8_t level) noexcept {
    return (level < COUNT) ? TABLE[level].symbol : TABLE[5].symbol;
}

inline constexpr int32_t rssi_threshold(uint8_t level) noexcept {
    return (level < COUNT) ? TABLE[level].rssi_threshold_db : TABLE[5].rssi_threshold_db;
}

} // namespace ThreatLevelData

// ===========================================
// DRONE TYPE DATA (Unified from color_lookup_unified.hpp)
// ===========================================

namespace DroneTypeData {
    
struct Entry {
    uint32_t color_rgb;
    const char* name;
    Frequency typical_freq_hz;
};

constexpr Entry TABLE[] EDA_FLASH_CONST = {
    { 0x808080,   "Unknown",        0 },
    { 0x0000FF,   "DJI Mavic",      2'437'000'000ULL },
    { 0xFFA500,   "DJI P34",        2'437'000'000ULL },
    { 0xFFFF00,   "DJI Phantom",    2'437'000'000ULL },
    { 0x00FFFF,   "DJI Mini",       2'437'000'000ULL },
    { 0xFF00FF,   "Parrot Anafi",   2'412'000'000ULL },
    { 0x00FF00,   "Parrot Bebop",   2'437'000'000ULL },
    { 0x00FF00,   "PX4 Drone",      915'000'000ULL },
    { 0x00FF00,   "Military UAV",   868'000'000ULL },
    { 0xFF00FF,   "DIY Drone",      433'075'000ULL },
    { 0x00FFFF,   "FPV Racing",     5'800'000'000ULL }
};

constexpr size_t COUNT = sizeof(TABLE) / sizeof(Entry);

inline constexpr uint32_t color(uint8_t type) noexcept {
    return (type < COUNT) ? TABLE[type].color_rgb : TABLE[0].color_rgb;
}

inline constexpr const char* name(uint8_t type) noexcept {
    return (type < COUNT) ? TABLE[type].name : TABLE[0].name;
}

inline constexpr uint8_t from_frequency(Frequency hz) noexcept {
    if (hz >= Constants::MIN_58GHZ && hz <= Constants::MAX_58GHZ) return 10;
    if (hz >= Constants::MIN_24GHZ && hz <= Constants::MAX_24GHZ) return 1;
    if (hz >= Constants::MIN_900MHZ && hz <= Constants::MAX_900MHZ) return 8;
    if (hz >= Constants::MIN_433MHZ && hz <= Constants::MAX_433MHZ) return 9;
    return 0;
}

} // namespace DroneTypeData

// ===========================================
// DRONE DATABASE CONTENT (from default_drones_db.hpp)
// ===========================================

namespace Database {

constexpr const char* DEFAULT_DRONE_DATABASE_CONTENT =
    "# EDA Factory Drone Database\n"
    "# Format: Freq(Hz), Description\n"
    "\n"
    "# --- DJI OcuSync / Lightbridge (2.4GHz) ---\n"
    "2406500000,DJI CH 1\n"
    "2416500000,DJI CH 2\n"
    "2426500000,DJI CH 3\n"
    "2436500000,DJI CH 4\n"
    "2446500000,DJI CH 5\n"
    "2456500000,DJI CH 6\n"
    "2466500000,DJI CH 7\n"
    "2476500000,DJI CH 8\n"
    "\n"
    "# --- Parrot / WiFi Drones (2.4GHz) ---\n"
    "2412000000,WiFi CH 1 (Parrot)\n"
    "2437000000,WiFi CH 6 (Parrot)\n"
    "2462000000,WiFi CH 11 (Parrot)\n"
    "2472000000,WiFi CH 13 (EU)\n"
    "\n"
    "# --- FPV Analog / Digital (5.8GHz) ---\n"
    "# RaceBand (Most common)\n"
    "5658000000,FPV RB CH1\n"
    "5695000000,FPV RB CH2\n"
    "5732000000,FPV RB CH3\n"
    "5769000000,FPV RB CH4\n"
    "5806000000,FPV RB CH5\n"
    "5843000000,FPV RB CH6\n"
    "5880000000,FPV RB CH7\n"
    "5917000000,FPV RB CH8\n"
    "\n"
    "# DJI FPV System (Digital)\n"
    "5660000000,DJI FPV CH1\n"
    "5695000000,DJI FPV CH2\n"
    "5735000000,DJI FPV CH3\n"
    "5770000000,DJI FPV CH4\n"
    "5805000000,DJI FPV CH5\n"
    "5839000000,DJI FPV CH6\n"
    "5878000000,DJI FPV CH7\n"
    "5914000000,DJI FPV CH8\n"
    "\n"
    "# --- Long Range Telemetry (868/915 MHz) ---\n"
    "868000000,TBS Crossfire EU\n"
    "915000000,TBS Crossfire US\n"
    "868400000,ELRS EU Start\n"
    "915400000,ELRS US Start\n"
    "\n"
    "# --- Legacy / Other ---\n"
    "433050000,LRS 433 Control\n"
    "5200000000,DJI 5.2G Aux\n"
    "5800000000,Generic 5.8 Center\n";

} // namespace Database

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

inline constexpr uint64_t parse_mhz_string(const char* str) noexcept {
    if (!str || *str == '\0') return 0;
    
    while (*str == ' ' || *str == '\t') str++;
    
    uint64_t mhz = 0;
    while (*str >= '0' && *str <= '9') {
        uint8_t digit = static_cast<uint8_t>(*str - '0');
        if (mhz > (UINT64_MAX - digit) / 10) return 0;
        mhz = mhz * 10 + digit;
        str++;
    }
    
    if (mhz > 18000ULL) return 0;
    
    uint64_t hz_fraction = 0;
    if (*str == '.') {
        str++;
        
        uint64_t multiplier = 100000ULL;
        
        for (int i = 0; i < 6 && *str >= '0' && *str <= '9'; i++) {
            uint8_t digit = static_cast<uint8_t>(*str - '0');
            hz_fraction = hz_fraction * 10 + digit;
            if (multiplier > 1) multiplier /= 10;
            str++;
        }
        
        hz_fraction *= multiplier;
    }
    
    uint64_t result = mhz * 1000000ULL;
    
    if (result > UINT64_MAX - hz_fraction) return 0;
    result += hz_fraction;
    
    return result;
}

} // namespace Validation

// ===========================================
// FREQUENCY BANDS (from diamond_core.hpp)
// ===========================================

namespace Bands {

struct Band {
    Frequency min_hz;
    Frequency max_hz;
    const char* name;
};

constexpr Band BANDS[] EDA_FLASH_CONST = {
    { 433'000'000ULL, 435'000'000ULL, "433MHz ISM" },
    { 860'000'000ULL, 930'000'000ULL, "900MHz LRS" },
    { 2'400'000'000ULL, 2'483'500'000ULL, "2.4GHz ISM" },
    { 5'725'000'000ULL, 5'875'000'000ULL, "5.8GHz FPV" }
};

constexpr size_t BAND_COUNT = sizeof(BANDS) / sizeof(Band);

inline constexpr size_t find_band(Frequency hz) noexcept {
    for (size_t i = 0; i < BAND_COUNT; ++i) {
        if (hz >= BANDS[i].min_hz && hz <= BANDS[i].max_hz) return i;
    }
    return BAND_COUNT;
}

inline constexpr bool is_2_4ghz(Frequency hz) noexcept {
    return hz >= BANDS[2].min_hz && hz <= BANDS[2].max_hz;
}

inline constexpr bool is_5_8ghz(Frequency hz) noexcept {
    return hz >= BANDS[3].min_hz && hz <= BANDS[3].max_hz;
}

} // namespace Bands

// ===========================================
// SPECTRAL CONFIG (from ui_spectral_analyzer.hpp)
// ===========================================

namespace Spectral {

struct Config {
    static constexpr uint8_t SNR_THRESHOLD = 10;
    static constexpr uint8_t PEAK_THRESHOLD_DB = 6;
    static constexpr size_t VALID_BIN_START = 8;
    static constexpr size_t VALID_BIN_END = 240;
    static constexpr size_t BIN_COUNT = 256;
    static constexpr size_t VALID_BIN_COUNT = VALID_BIN_END - VALID_BIN_START;
    
    static constexpr uint32_t DRONE_MAX_WIDTH_HZ = 2'500'000;
    static constexpr uint32_t WIFI_MIN_WIDTH_HZ = 10'000'000;
    
    static constexpr uint32_t INV_BIN_COUNT_Q16 = 
        (65536 + VALID_BIN_COUNT / 2) / VALID_BIN_COUNT;
};

} // namespace Spectral

// ===========================================
// RSSI UTILITIES (from diamond_core.hpp)
// ===========================================

namespace RSSI {

constexpr int32_t THRESHOLDS[5] EDA_FLASH_CONST = {
    -120,
    -100,
    -85,
    -70,
    -50
};

inline constexpr int32_t threshold(uint8_t threat_idx) noexcept {
    return (threat_idx < 5) ? THRESHOLDS[threat_idx] : THRESHOLDS[0];
}

inline constexpr bool validate_rssi(int32_t rssi, uint8_t threat_idx) noexcept {
    return rssi >= threshold(threat_idx);
}

inline constexpr bool is_strong(int32_t rssi) noexcept {
    return rssi >= -70;
}

inline constexpr bool is_weak(int32_t rssi) noexcept {
    return rssi <= -100;
}

} // namespace RSSI

// ===========================================
// MOVEMENT TREND UTILITIES (from diamond_core.hpp)
// ===========================================

namespace Trend {

inline constexpr char symbol(uint8_t trend_idx) noexcept {
    switch (trend_idx) {
        case 1: return '<';
        case 2: return '>';
        case 0:
        case 3:
        default: return '~';
    }
}

inline constexpr const char* name(uint8_t trend_idx) noexcept {
    switch (trend_idx) {
        case 1: return "APPROACHING";
        case 2: return "RECEDING";
        case 0: return "STATIC";
        case 3:
        default: return "UNKNOWN";
    }
}

} // namespace Trend

// ===========================================
// COLOR CONVERTER (from color_lookup_unified.hpp)
// ===========================================

namespace Color {

inline constexpr uint16_t rgb888_to_rgb565(uint32_t rgb888) noexcept {
    uint8_t r = (rgb888 >> 16) & 0xFF;
    uint8_t g = (rgb888 >> 8) & 0xFF;
    uint8_t b = rgb888 & 0xFF;
    
    uint16_t r565 = (r & 0xF8) << 8;
    uint16_t g565 = (g & 0xFC) << 3;
    uint16_t b565 = (b & 0xF8) >> 3;
    
    return r565 | g565 | b565;
}

} // namespace Color

} // namespace EDA

// Backward compatibility aliases
namespace DroneConstants = EDA::Constants;

#endif // EDA_CONSTANTS_HPP_
