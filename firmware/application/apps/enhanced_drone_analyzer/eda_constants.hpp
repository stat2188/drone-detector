// Diamond Code: Flash-resident constants and utilities for Enhanced Drone Analyzer

#ifndef EDA_CONSTANTS_HPP_
#define EDA_CONSTANTS_HPP_

#include <cstdint>
#include <cinttypes>
#include <array>
#include <limits>
#include "rf_path.hpp"

// Flash storage attribute for Cortex-M4
#ifdef __GNUC__
    #define EDA_FLASH_CONST __attribute__((section(".rodata")))
#else
    #define EDA_FLASH_CONST const
#endif

namespace EDA {

using Frequency = rf::Frequency;
using RSSI = int32_t;
using BinIndex = size_t;
using Threshold = int32_t;
using Decibel = int32_t;
using Timestamp = uint32_t;

namespace Constants {

namespace FrequencyLimits {
    constexpr Frequency MIN_HARDWARE_FREQ =     1'000'000ULL;
    constexpr Frequency MAX_HARDWARE_FREQ =  7'200'000'000ULL;
    constexpr Frequency MIN_SAFE_FREQ =        50'000'000ULL;
    constexpr Frequency MAX_SAFE_FREQ =     6'000'000'000ULL;
}

enum class ScanningMode : uint8_t {
    STRICT_DRONE = 0,
    FULL_SPECTRUM = 1
};

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

constexpr Frequency BAND_SPLIT_FREQ_5GHZ = 5'000'000'000ULL;

constexpr uint32_t NARROWBAND_DRONE_MAX_WIDTH_HZ = 2'500'000ULL;
constexpr uint32_t WIDEBAND_WIFI_MIN_WIDTH_HZ = 10'000'000ULL;
constexpr uint32_t WIDEBAND_DEFAULT_SLICE_WIDTH = 22'000'000ULL;
constexpr uint32_t WIDEBAND_MAX_SLICES = 10;
constexpr Frequency WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
constexpr Frequency WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
constexpr uint32_t WIDEBAND_SLICE_WIDTH = WIDEBAND_DEFAULT_SLICE_WIDTH;

constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -80;
constexpr int32_t CRITICAL_RSSI_DB = -50;
constexpr int32_t HIGH_RSSI_DB = -60;
constexpr int32_t MEDIUM_RSSI_DB = -70;
constexpr int32_t LOW_RSSI_DB = -80;
constexpr int32_t NOISE_FLOOR_RSSI = -110;
constexpr int32_t MIN_VALID_RSSI = -110;
constexpr int32_t MAX_VALID_RSSI = 10;

constexpr int32_t MAX_CONSECUTIVE_FAILURES = 10;

constexpr uint8_t SPECTRAL_SNR_THRESHOLD = 10;
constexpr uint8_t SPECTRAL_PEAK_THRESHOLD_DB = 6;
constexpr size_t SPECTRAL_VALID_BIN_START = 8;
constexpr size_t SPECTRAL_VALID_BIN_END = 240;
constexpr size_t SPECTRAL_BIN_COUNT = 256;

constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;
constexpr uint32_t MAX_SCAN_INTERVAL_MS = 10000;
constexpr uint32_t DEFAULT_SCAN_INTERVAL_MS = 1000;
constexpr uint8_t MIN_DETECTION_COUNT = 3;
constexpr uint32_t ALERT_PERSISTENCE_THRESHOLD = 3;
constexpr uint32_t STALE_DRONE_TIMEOUT_MS = 30000;

constexpr uint32_t FAST_SCAN_INTERVAL_MS = 250;
constexpr uint32_t HIGH_THREAT_SCAN_INTERVAL_MS = 400;
constexpr uint32_t NORMAL_SCAN_INTERVAL_MS = 750;
constexpr uint32_t SLOW_SCAN_INTERVAL_MS = 1000;
constexpr uint32_t VERY_SLOW_SCAN_INTERVAL_MS = 2000;
constexpr uint32_t HIGH_DENSITY_SCAN_CAP_MS = 500;
constexpr uint32_t PROGRESSIVE_SLOWDOWN_DIVISOR = 10;

constexpr uint32_t PRIORITY_SLICE_SKIP_THRESHOLD = 10;
constexpr uint32_t PREDICTION_FRESHNESS_MS = 5000;
constexpr uint32_t FHSS_TRACKING_CONFIDENCE_MAX = 10;
constexpr uint32_t PRIORITY_SCAN_INTERVAL = 10;
constexpr uint32_t MAX_FREQUENCY_PREDICTIONS = 10;
constexpr uint32_t PREDICTION_STALE_MS = 5000;
constexpr uint32_t CONFIDENCE_BOOST_INCREMENT = 1;
constexpr uint32_t CONFIDENCE_MAX = 10;

constexpr uint32_t RSSI_TIMEOUT_MS = 60;
constexpr uint32_t RSSI_POLL_DELAY_MS = 2;
constexpr uint32_t PLL_STABILIZATION_ITERATIONS = 3;
constexpr uint32_t PLL_STABILIZATION_DELAY_MS = 10;
constexpr uint32_t SPECTRUM_TIMEOUT_MS = 32;
constexpr uint32_t CHECK_INTERVAL_MS = 2;
constexpr uint32_t SCAN_CYCLE_CHECK_INTERVAL = 50;
constexpr uint32_t MAX_SCAN_BATCH_SIZE = 10;

constexpr uint32_t HYBRID_SCAN_DIVISOR = 2;

constexpr int32_t MOVEMENT_TREND_THRESHOLD_APPROACHING = 3;
constexpr int32_t MOVEMENT_TREND_THRESHOLD_RECEEDING = -3;
constexpr int32_t MOVEMENT_TREND_MIN_HISTORY = 4;
constexpr int32_t MOVEMENT_TREND_SILENCE_THRESHOLD = -110;

constexpr size_t MAX_TRACKED_DRONES = 4;
constexpr size_t MAX_DISPLAYED_DRONES = 3;
constexpr size_t DETECTION_TABLE_SIZE = 256;

constexpr uint32_t MIN_AUDIO_FREQ = 200;
constexpr uint32_t MAX_AUDIO_FREQ = 20000;
constexpr uint32_t MIN_AUDIO_DURATION = 50;
constexpr uint32_t MAX_AUDIO_DURATION = 5000;
constexpr uint32_t DEFAULT_ALERT_FREQ_HZ = 800;
constexpr uint32_t DEFAULT_ALERT_DURATION_MS = 500;
constexpr uint32_t DEFAULT_ALERT_VOLUME_LEVEL = 50;
constexpr uint32_t MCU_MAX_AUDIO_FREQ_HZ = 4000;
constexpr uint32_t GENERAL_MAX_AUDIO_FREQ_HZ = 20000;
constexpr uint32_t DEFAULT_ALERT_COOLDOWN_MS = 100;
constexpr uint32_t AUDIO_COOLDOWN_MS = 100;
constexpr uint32_t AUDIO_BEEP_DURATION_MS = 50;
constexpr uint32_t AUDIO_BEEP_FREQUENCY_HZ = 800;

// Bandwidth Parameters
constexpr uint32_t MIN_BANDWIDTH = 10000;
constexpr uint32_t MAX_BANDWIDTH = 28000000;

// UI Dimensions
constexpr uint32_t SCREEN_WIDTH = 240;
constexpr uint32_t SCREEN_HEIGHT = 320;
constexpr uint32_t TEXT_HEIGHT = 16;
constexpr uint32_t TEXT_LINE_HEIGHT = 24;
constexpr uint32_t DISPLAY_UPDATE_INTERVAL_MS = 100;
constexpr uint32_t UI_REFRESH_RATE_MS = 50;
constexpr uint32_t SCREEN_BLANK_TIMEOUT_MS = 60000;

// Spectrum Parameters
constexpr uint32_t SPECTRUM_BIN_COUNT = 256;
constexpr uint32_t SPECTRUM_BIN_COUNT_240 = 240;
constexpr uint32_t MINI_SPECTRUM_WIDTH = 200;
constexpr uint32_t MINI_SPECTRUM_HEIGHT = 24;
constexpr uint32_t SPECTRUM_ROW_SIZE = 240;
constexpr uint32_t RENDER_LINE_SIZE = 240;
constexpr uint32_t WATERFALL_SIZE = 40 * 240;
constexpr uint8_t SPECTRUM_NOISE_FLOOR_DEFAULT = 20;
constexpr uint8_t SPECTRUM_PEAK_THRESHOLD_DEFAULT = 10;
constexpr uint32_t SPECTRUM_UPDATE_RATE_HZ = 60;

// UI Constants
constexpr size_t LAST_TEXT_BUFFER_SIZE = 128;
constexpr size_t CARD_TEXT_BUFFER_SIZE = 64;

// Buffer Sizes
constexpr uint32_t ERROR_MESSAGE_BUFFER_SIZE = 128;
constexpr uint32_t DEFAULT_BUFFER_SIZE_4KB = 4096;
constexpr uint32_t WORKER_STACK_SIZE_8KB = 8192;
constexpr uint32_t DB_LOADING_STACK_SIZE_8KB = 8192;
constexpr uint32_t POOL_SIZE_2KB = 2048;
constexpr uint32_t POOL_SIZE_1KB = 1024;
constexpr uint32_t MAX_STRING_LENGTH_256 = 256;
constexpr uint32_t FREQ_DB_STORAGE_SIZE_4KB = 4096;
constexpr uint32_t FREQ_DB_STORAGE_SIZE_2KB = 2048;

// Thread Stack Sizes
constexpr uint32_t WORKER_STACK_SIZE_4KB = 4096;
constexpr uint32_t DB_LOADING_STACK_SIZE_4KB = 4096;
constexpr uint32_t COORDINATOR_STACK_SIZE_6KB = 6144;

// RSSI Parameters
constexpr int32_t RSSI_SILENCE_DBM = -120;
constexpr int32_t RSSI_INVALID_DBM = -127;

// Thread Timeouts
constexpr uint32_t THREAD_JOIN_TIMEOUT_MS = 5000;
constexpr uint32_t THREAD_TERMINATION_TIMEOUT_MS = 3000;
constexpr uint32_t THREAD_TERMINATION_POLL_INTERVAL_MS = 10;

// Database Parameters
constexpr uint32_t MAX_DB_ENTRIES = 75;
constexpr uint32_t DB_SYNC_INTERVAL_MS = 5000;
constexpr uint32_t DB_LOAD_RETRY_COUNT = 3;
constexpr uint32_t DB_LOAD_RETRY_DELAY_MS = 500;
constexpr uint32_t SD_CARD_MOUNT_TIMEOUT_MS = 5000;
constexpr uint32_t SD_CARD_POLL_INTERVAL_MS = 100;
constexpr uint32_t SD_CARD_POLL_INTERVAL_SHORT_MS = 50;
constexpr uint32_t SETTINGS_LOAD_TIMEOUT_MS = 2000;
constexpr uint32_t BASEBOARD_STOP_DELAY_MS = 10;

// Wideband Scanning
constexpr uint32_t WIDEBAND_SLICE_COUNT_DEFAULT = 10;
constexpr uint32_t WIDEBAND_SLICE_COUNT_MIN = 1;
constexpr uint32_t WIDEBAND_SLICE_COUNT_MAX = 20;

// Detection Logging
constexpr uint32_t MAX_LOG_ENTRIES = 500;
constexpr uint32_t LOG_FLUSH_INTERVAL_MS = 10000;
constexpr uint32_t LOG_FILE_MAX_SIZE_KB = 1024;
constexpr uint32_t LOG_LINE_BUFFER_SIZE = 128;
constexpr uint32_t LOG_WRITE_INTERVAL_MS = 200;
constexpr uint32_t DETECTION_RING_BUFFER_SIZE = 32;

// Frequency Hashing
constexpr uint32_t FREQ_HASH_DIVISOR = 100000;
constexpr uint32_t FREQ_HASH_TABLE_SIZE = 32;
constexpr uint32_t FREQ_HASH_MASK = FREQ_HASH_TABLE_SIZE - 1;

// Timestamps
constexpr Timestamp TIMESTAMP_WRAP_THRESHOLD = 0xFFFFFFFFUL / 2;
constexpr uint32_t TIMESTAMP_MAX_AGE_MS = 60000;

// Error Handling
constexpr uint32_t ERROR_MESSAGE_DISPLAY_TIME_MS = 3000;
constexpr uint32_t MAX_ERROR_MESSAGES = 10;

// Memory Pools
constexpr uint32_t BUFFER_POOL_SIZE = 8;
constexpr uint32_t BUFFER_POOL_ENTRY_SIZE = 512;

// File I/O
constexpr uint32_t FILE_READ_CHUNK_SIZE = 256;
constexpr uint32_t FILE_WRITE_CHUNK_SIZE = 512;
constexpr uint32_t FILE_OPEN_TIMEOUT_MS = 2000;

// Threat Levels
constexpr uint32_t THREAT_LEVEL_CRITICAL_THRESHOLD = 3;
constexpr uint32_t THREAT_LEVEL_HIGH_THRESHOLD = 2;
constexpr uint32_t THREAT_LEVEL_MEDIUM_THRESHOLD = 1;
constexpr uint32_t THREAT_LEVEL_LOW_THRESHOLD = 0;

// Settings Storage
constexpr uint32_t SETTINGS_TEMPLATE_SIZE_4KB = 4096;
constexpr uint32_t SETTINGS_TEMPLATE_SIZE_2KB = 2048;
constexpr uint32_t MAX_SETTINGS_FILE_SIZE_64KB = 65536;
constexpr uint32_t MAX_SETTINGS_LINES = 1000;
constexpr uint32_t MAX_LINE_LENGTH = 128;
constexpr uint32_t MAX_SETTING_STR_LEN = 65;

// String Length Constants
constexpr size_t MAX_PATH_LENGTH = 64;
constexpr size_t MAX_NAME_LENGTH = 32;
constexpr size_t MAX_FORMAT_LENGTH = 8;

} // namespace Constants

// Validation Utilities

namespace Validation {

// Validation Constants
static constexpr Frequency MIN_HARDWARE_FREQ = 1'000'000ULL;
static constexpr Frequency MAX_HARDWARE_FREQ = 7'200'000'000ULL;
static constexpr Frequency MIN_2_4GHZ = 2'400'000'000ULL;
static constexpr Frequency MAX_2_4GHZ = 2'483'500'000ULL;
static constexpr Frequency MIN_5_8GHZ = 5'725'000'000ULL;
static constexpr Frequency MAX_5_8GHZ = 5'875'000'000ULL;
static constexpr Frequency MIN_MILITARY = 860'000'000ULL;
static constexpr Frequency MAX_MILITARY = 930'000'000ULL;
static constexpr Frequency MIN_433MHZ = 433'000'000ULL;
static constexpr Frequency MAX_433MHZ = 435'000'000ULL;

// Validation Functions
static constexpr bool is_in_range(Frequency value, Frequency min_val, Frequency max_val) noexcept {
    return value >= min_val && value <= max_val;
}

static constexpr bool validate_frequency(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_HARDWARE_FREQ, MAX_HARDWARE_FREQ);
}

static constexpr bool validate_rssi(int32_t rssi_db) noexcept {
    return rssi_db >= -110 && rssi_db <= 10;
}

static constexpr bool is_2_4ghz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_2_4GHZ, MAX_2_4GHZ);
}

static constexpr bool is_5_8ghz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_5_8GHZ, MAX_5_8GHZ);
}

static constexpr bool is_military_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_MILITARY, MAX_MILITARY);
}

static constexpr bool is_433mhz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_433MHZ, MAX_433MHZ);
}

} // namespace Validation

// Unified Spectrum Mode LUT (O(1) Lookup)

namespace LUTs {

constexpr size_t SPECTRUM_MODE_COUNT = 5;
constexpr uint8_t DEFAULT_SPECTRUM_MODE_INDEX = 2;
constexpr const char* DEFAULT_SPECTRUM_MODE_NAME = "MEDIUM";
constexpr const char* DEFAULT_SPECTRUM_MODE_DISPLAY_NAME = "Medium";
constexpr uint32_t DEFAULT_BANDWIDTH_HZ = 24000000;

struct SpectrumModeInfo {
    const char* short_name;
    const char* display_name;
    uint32_t   bandwidth_hz;
    uint8_t    ui_index;
};

EDA_FLASH_CONST static constexpr SpectrumModeInfo SPECTRUM_MODES[SPECTRUM_MODE_COUNT] = {
    {"ULTRA_NARROW", "Ultra Narrow",  8000000,  0},
    {"NARROW",       "Narrow",        12000000, 1},
    {"MEDIUM",       "Medium",        24000000, 2},
    {"WIDE",         "Wide",          48000000, 3},
    {"ULTRA_WIDE",   "Ultra Wide",    96000000, 4}
};

EDA_FLASH_CONST static constexpr std::array<uint8_t, SPECTRUM_MODE_COUNT> UI_INDEX_TO_MODE_IDX = {0, 1, 2, 3, 4};

inline constexpr const char* spectrum_mode_short_name(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].short_name : DEFAULT_SPECTRUM_MODE_NAME;
}

inline constexpr const char* spectrum_mode_display_name(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].display_name : DEFAULT_SPECTRUM_MODE_DISPLAY_NAME;
}

inline constexpr uint32_t spectrum_mode_bandwidth(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].bandwidth_hz : DEFAULT_BANDWIDTH_HZ;
}

inline constexpr uint8_t spectrum_mode_ui_index(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].ui_index : DEFAULT_SPECTRUM_MODE_INDEX;
}

inline constexpr uint8_t ui_index_to_spectrum_mode(uint8_t ui_index) noexcept {
    return (ui_index < SPECTRUM_MODE_COUNT) ? UI_INDEX_TO_MODE_IDX[ui_index] : DEFAULT_SPECTRUM_MODE_INDEX;
}

} // namespace LUTs

// Error Message LUT (Flash-Resident)

namespace ErrorHandling {

constexpr size_t ERROR_MESSAGE_COUNT = 10;

EDA_FLASH_CONST static constexpr const char* const ERROR_MESSAGES[ERROR_MESSAGE_COUNT] = {
    "Success",
    "Invalid argument",
    "Value out of range",
    "Null pointer",
    "Buffer overflow",
    "Allocation failed",
    "File I/O error",
    "Invalid frequency",
    "Invalid RSSI",
    "Timeout"
};

} // namespace ErrorHandling

// Unified Frequency Formatting (Zero-Heap)

namespace Formatting {

constexpr size_t MAX_FORMATTED_FREQ_LENGTH = 32;

struct FrequencyScale {
    Frequency   threshold_hz;
    const char* unit;
    Frequency   divider;
};

constexpr size_t FREQUENCY_SCALE_COUNT = 4;

EDA_FLASH_CONST static constexpr FrequencyScale FREQUENCY_SCALES[FREQUENCY_SCALE_COUNT] = {
    {1'000'000'000ULL, "G", 1'000'000'000ULL},
    {1'000'000ULL,     "M", 1'000'000ULL},
    {1'000ULL,         "k", 1'000ULL},
    {0ULL,             "",  1ULL}
};

inline void format_frequency(char* buffer, size_t size, Frequency freq_hz) noexcept {
    if (!buffer || size < MAX_FORMATTED_FREQ_LENGTH) {
        return;
    }

    const auto* scale = FREQUENCY_SCALES;
    while (scale->threshold_hz > 0 && freq_hz >= scale->threshold_hz) {
        scale++;
    }

    uint64_t value = freq_hz / scale->divider;
    uint64_t decimal = freq_hz % scale->divider;

    if (decimal > 0 && scale->threshold_hz > 0) {
        decimal = (decimal * 10) / scale->divider;
        snprintf(buffer, size, "%llu.%llu%s",
                 static_cast<unsigned long long>(value),
                 static_cast<unsigned long long>(decimal),
                 scale->unit);
    } else {
        snprintf(buffer, size, "%llu%s",
                 static_cast<unsigned long long>(value),
                 scale->unit);
    }
}

inline void format_frequency_compact(char* buffer, size_t size, Frequency freq_hz) noexcept {
    if (!buffer || size < MAX_FORMATTED_FREQ_LENGTH) {
        return;
    }

    // DIAMOND FIX: Cast to uint64_t to avoid signed/unsigned comparison warnings
    const uint64_t freq_u64 = static_cast<uint64_t>(freq_hz);

    if (freq_u64 >= 1'000'000'000ULL) {
        if (freq_u64 > UINT64_MAX - 500'000'000ULL) {
            snprintf(buffer, size, "%" PRIu64 "G", UINT64_MAX / 1'000'000'000ULL);
            return;
        }
        uint64_t rounded = freq_u64 + 500'000'000ULL;
        uint32_t ghz = static_cast<uint32_t>(rounded / 1'000'000'000ULL);
        uint32_t decimal = static_cast<uint32_t>((rounded % 1'000'000'000ULL) / 100'000'000ULL);

        if (decimal > 0) {
            snprintf(buffer, size, "%" PRIu32 ".%" PRIu32 "G", ghz, decimal);
        } else {
            snprintf(buffer, size, "%" PRIu32 "G", ghz);
        }
    }
    else if (freq_u64 >= 1'000'000ULL) {
        uint32_t mhz = static_cast<uint32_t>((freq_u64 + 500'000ULL) / 1'000'000ULL);
        snprintf(buffer, size, "%" PRIu32 "M", mhz);
    }
    else if (freq_u64 >= 1'000ULL) {
        uint32_t khz = static_cast<uint32_t>((freq_u64 + 500ULL) / 1'000ULL);
        snprintf(buffer, size, "%" PRIu32 "k", khz);
    }
    else {
        snprintf(buffer, size, "%" PRIu64, freq_u64);
    }
}

inline void format_frequency_fixed(char* buffer, size_t size, Frequency freq_hz) noexcept {
    if (!buffer || size < 32) return;
    
    uint64_t freq_u64 = static_cast<uint64_t>(freq_hz);
    
    if (freq_u64 >= 1'000'000'000ULL) {
        uint32_t ghz = static_cast<uint32_t>(freq_u64 / 1'000'000'000ULL);
        uint32_t decimal = static_cast<uint32_t>((freq_u64 % 1'000'000'000ULL) / 100'000'000ULL);
        if (decimal > 0) {
            snprintf(buffer, size, "%" PRIu32 ".%" PRIu32 "G", ghz, decimal);
        } else {
            snprintf(buffer, size, "%" PRIu32 "G", ghz);
        }
    }
    else if (freq_u64 >= 1'000'000ULL) {
        uint32_t mhz = static_cast<uint32_t>((freq_u64 + 500'000ULL) / 1'000'000ULL);
        snprintf(buffer, size, "%" PRIu32 "M", mhz);
    }
    else if (freq_u64 >= 1'000ULL) {
        uint32_t khz = static_cast<uint32_t>((freq_u64 + 500ULL) / 1'000ULL);
        snprintf(buffer, size, "%" PRIu32 "k", khz);
    }
    else {
        snprintf(buffer, size, "%" PRIu64, freq_u64);
    }
}

} // namespace Formatting

// Error Handling (Zero-Overhead)

enum class ErrorCode : uint8_t {
    SUCCESS = 0,
    INVALID_ARGUMENT = 1,
    OUT_OF_RANGE = 2,
    NULL_POINTER = 3,
    BUFFER_OVERFLOW = 4,
    ALLOCATION_FAILED = 5,
    FILE_IO_ERROR = 6,
    INVALID_FREQUENCY = 7,
    INVALID_RSSI = 8,
    TIMEOUT = 9,
    UNKNOWN_ERROR = 255
};

template<typename T>
struct ErrorResult {
    ErrorCode error_code;
    T value;

    static constexpr ErrorResult ok(T val) noexcept {
        return ErrorResult{ErrorCode::SUCCESS, val};
    }

    static constexpr ErrorResult fail(ErrorCode code) noexcept {
        return ErrorResult{code, T{}};
    }

    static constexpr ErrorResult fail(ErrorCode code, T default_val) noexcept {
        return ErrorResult{code, default_val};
    }

    constexpr bool is_ok() const noexcept { return error_code == ErrorCode::SUCCESS; }
    constexpr bool is_error() const noexcept { return error_code != ErrorCode::SUCCESS; }
    constexpr explicit operator bool() const noexcept { return is_ok(); }

    constexpr const char* error_message() const noexcept {
        const size_t code = static_cast<size_t>(error_code);
        if (code < ErrorHandling::ERROR_MESSAGE_COUNT) {
            return ErrorHandling::ERROR_MESSAGES[code];
        }
        return "Unknown error";
    }
};

// Safe Type Casting (Zero-Overhead)
template<typename To, typename From>
constexpr To safe_reinterpret_cast(From* ptr) noexcept {
    static_assert(alignof(To) <= alignof(std::max_align_t), "Target alignment too large");
    return reinterpret_cast<To>(ptr);
}

template<typename To, typename From>
constexpr To safe_reinterpret_cast_volatile(From* ptr) noexcept {
    static_assert(alignof(To) <= alignof(std::max_align_t), "Target alignment too large");
    return reinterpret_cast<To>(ptr);
}

template<typename To>
constexpr To safe_reinterpret_cast_addr(uintptr_t addr) noexcept {
    static_assert(alignof(To) <= alignof(std::max_align_t), "Target alignment too large");
    return reinterpret_cast<To>(addr);
}

// Safe Arithmetic Helpers
namespace SafeArithmetic {

template<typename T>
constexpr bool would_add_overflow(T a, T b) noexcept {
    return a > std::numeric_limits<T>::max() - b;
}

template<typename T>
constexpr bool would_mul_overflow(T a, T b) noexcept {
    return a != 0 && b > std::numeric_limits<T>::max() / a;
}

template<typename T>
constexpr T checked_add(T a, T b, T clamp_value) noexcept {
    return would_add_overflow(a, b) ? clamp_value : a + b;
}

} // namespace SafeArithmetic

} // namespace EDA

// Backward compatibility aliases
namespace DroneConstants = EDA::Constants;

#endif // EDA_CONSTANTS_HPP_
