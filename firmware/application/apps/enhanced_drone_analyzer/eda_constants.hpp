// eda_constants.hpp
// Diamond Code: Flash-resident constants and utilities for Enhanced Drone Analyzer
// Memory-safe, zero-heap, optimized for STM32F405 (128KB RAM)

#ifndef EDA_CONSTANTS_HPP_
#define EDA_CONSTANTS_HPP_

#include <cstdint>
#include <cinttypes>
#include <array>
#include "rf_path.hpp"

// Flash storage attribute for Cortex-M4
#ifdef __GNUC__
    #define EDA_FLASH_CONST __attribute__((section(".rodata")))
#else
    #define EDA_FLASH_CONST const
#endif

namespace EDA {

// ========================================
// TYPE ALIASES (Semantic Types)
// ========================================

using Frequency = rf::Frequency;  ///< Frequency type (uint64_t, Hz)
using RSSI = int32_t;              ///< Received Signal Strength Indicator (dBm)
using BinIndex = size_t;           ///< Spectrum bin index
using Threshold = int32_t;         ///< Threshold value (dBm)
using Decibel = int32_t;           ///< Decibel value (dB)
using Timestamp = uint32_t;        ///< Timestamp (ms)

// ========================================
// CONSTANTS (Flash-Resident)
// ========================================

namespace Constants {

// ===== FREQUENCY LIMITS =====
namespace FrequencyLimits {
    constexpr Frequency MIN_HARDWARE_FREQ =     1'000'000ULL;  ///< Minimum hardware frequency (1 MHz)
    constexpr Frequency MAX_HARDWARE_FREQ =  7'200'000'000ULL;  ///< Maximum hardware frequency (7.2 GHz)
    constexpr Frequency MIN_SAFE_FREQ =        50'000'000ULL;  ///< Minimum safe frequency (50 MHz, PLL stability)
    constexpr Frequency MAX_SAFE_FREQ =     6'000'000'000ULL;  ///< Maximum safe frequency (6 GHz, PLL stability)
}

// ===== SCANNING MODES =====
enum class ScanningMode : uint8_t {
    STRICT_DRONE = 0,  ///< Scan only drone frequencies
    FULL_SPECTRUM = 1  ///< Scan full spectrum
};

// ===== FREQUENCY RANGES =====
constexpr Frequency MIN_433MHZ = 433'000'000ULL;     ///< 433 MHz band minimum
constexpr Frequency MAX_433MHZ = 435'000'000ULL;     ///< 433 MHz band maximum

constexpr Frequency MIN_900MHZ = 860'000'000ULL;     ///< 900 MHz band minimum
constexpr Frequency MAX_900MHZ = 930'000'000ULL;     ///< 900 MHz band maximum

constexpr Frequency MIN_24GHZ = 2'400'000'000ULL;    ///< 2.4 GHz band minimum
constexpr Frequency MAX_24GHZ = 2'483'500'000ULL;    ///< 2.4 GHz band maximum
constexpr Frequency DEFAULT_24GHZ_CENTER = 2'450'000'000ULL;  ///< Default 2.4 GHz center frequency
constexpr Frequency WIDEBAND_24GHZ_MIN = 2'400'000'000ULL;    ///< Wideband 2.4 GHz minimum
constexpr Frequency WIDEBAND_24GHZ_MAX = 2'500'000'000ULL;    ///< Wideband 2.4 GHz maximum

constexpr Frequency MIN_58GHZ = 5'725'000'000ULL;    ///< 5.8 GHz band minimum
constexpr Frequency MAX_58GHZ = 5'875'000'000ULL;    ///< 5.8 GHz band maximum
constexpr Frequency DEFAULT_58GHZ_CENTER = 5'800'000'000ULL;  ///< Default 5.8 GHz center frequency

// ===== FREQUENCY THRESHOLDS =====
constexpr Frequency BAND_SPLIT_FREQ_5GHZ = 5'000'000'000ULL;  ///< Band split frequency (5 GHz)

// ===== SIGNAL WIDTHS =====
constexpr uint32_t NARROWBAND_DRONE_MAX_WIDTH_HZ = 2'500'000ULL;   ///< Narrowband drone max width (Hz)
constexpr uint32_t WIDEBAND_WIFI_MIN_WIDTH_HZ = 10'000'000ULL;      ///< Wideband WiFi min width (Hz)
constexpr uint32_t WIDEBAND_DEFAULT_SLICE_WIDTH = 22'000'000ULL;    ///< Default wideband slice width (Hz)
constexpr uint32_t WIDEBAND_MAX_SLICES = 10;                         ///< Maximum wideband slices
constexpr Frequency WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;        ///< Default wideband minimum frequency
constexpr Frequency WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;        ///< Default wideband maximum frequency
constexpr uint32_t WIDEBAND_SLICE_WIDTH = WIDEBAND_DEFAULT_SLICE_WIDTH;  ///< Wideband slice width alias

// ===== RSSI THRESHOLDS =====
constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;  ///< Default RSSI threshold (dBm)
constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -80;  ///< Wideband RSSI threshold (dBm)
constexpr int32_t CRITICAL_RSSI_DB = -50;            ///< Critical RSSI (dBm)
constexpr int32_t HIGH_RSSI_DB = -60;               ///< High RSSI (dBm)
constexpr int32_t MEDIUM_RSSI_DB = -70;             ///< Medium RSSI (dBm)
constexpr int32_t LOW_RSSI_DB = -80;                ///< Low RSSI (dBm)
constexpr int32_t NOISE_FLOOR_RSSI = -110;          ///< Noise floor RSSI (dBm)
constexpr int32_t MIN_VALID_RSSI = -110;             ///< Minimum valid RSSI (dBm)
constexpr int32_t MAX_VALID_RSSI = 10;               ///< Maximum valid RSSI (dBm)

// ===== DETECTION THRESHOLDS =====
constexpr int32_t MAX_CONSECUTIVE_FAILURES = 10;  ///< Max allowed hardware failures before abort

// ===== SPECTRAL ANALYSIS =====
constexpr uint8_t SPECTRAL_SNR_THRESHOLD = 10;       ///< Spectral SNR threshold (dB)
constexpr uint8_t SPECTRAL_PEAK_THRESHOLD_DB = 6;    ///< Spectral peak threshold (dB)
constexpr size_t SPECTRAL_VALID_BIN_START = 8;       ///< Valid spectral bin start index
constexpr size_t SPECTRAL_VALID_BIN_END = 240;       ///< Valid spectral bin end index
constexpr size_t SPECTRAL_BIN_COUNT = 256;           ///< Spectral bin count

// ===== DETECTION =====
constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;       ///< Minimum scan interval (ms)
constexpr uint32_t MAX_SCAN_INTERVAL_MS = 10000;     ///< Maximum scan interval (ms)
constexpr uint32_t DEFAULT_SCAN_INTERVAL_MS = 1000;  ///< Default scan interval (ms)
constexpr uint8_t MIN_DETECTION_COUNT = 3;           ///< Minimum detection count
constexpr uint32_t ALERT_PERSISTENCE_THRESHOLD = 3;   ///< Alert persistence threshold

// ===== ADAPTIVE SCAN INTERVALS =====
constexpr uint32_t FAST_SCAN_INTERVAL_MS = 250;          ///< Fast scan interval (CRITICAL threat)
constexpr uint32_t HIGH_THREAT_SCAN_INTERVAL_MS = 400;   ///< High threat scan interval
constexpr uint32_t NORMAL_SCAN_INTERVAL_MS = 750;        ///< Normal scan interval (MEDIUM threat)
constexpr uint32_t SLOW_SCAN_INTERVAL_MS = 1000;         ///< Slow scan interval (LOW threat)
constexpr uint32_t VERY_SLOW_SCAN_INTERVAL_MS = 2000;   ///< Very slow scan interval (no detections)
constexpr uint32_t HIGH_DENSITY_SCAN_CAP_MS = 500;      ///< High density scan cap (>5 detections)
constexpr uint32_t PROGRESSIVE_SLOWDOWN_DIVISOR = 10;   ///< Progressive slowdown divisor

// ===== INTELLIGENT SCANNING =====
constexpr uint32_t PRIORITY_SLICE_SKIP_THRESHOLD = 10;   ///< Priority slice skip threshold
constexpr uint32_t PREDICTION_FRESHNESS_MS = 5000;      ///< Prediction freshness threshold (ms)
constexpr uint32_t FHSS_TRACKING_CONFIDENCE_MAX = 10;    ///< FHSS tracking confidence max
constexpr uint32_t PRIORITY_SCAN_INTERVAL = 10;          ///< Priority scan interval
constexpr uint32_t MAX_FREQUENCY_PREDICTIONS = 10;       ///< Maximum frequency predictions
constexpr uint32_t PREDICTION_STALE_MS = 5000;           ///< Prediction staleness (ms)
constexpr uint32_t CONFIDENCE_BOOST_INCREMENT = 1;       ///< Confidence boost increment
constexpr uint32_t CONFIDENCE_MAX = 10;                  ///< Maximum confidence

// ===== HARDWARE TIMING =====
constexpr uint32_t RSSI_TIMEOUT_MS = 60;                  ///< RSSI timeout (ms)
constexpr uint32_t RSSI_POLL_DELAY_MS = 2;                ///< RSSI poll delay (ms)
constexpr uint32_t PLL_STABILIZATION_ITERATIONS = 3;      ///< PLL stabilization iterations
constexpr uint32_t SPECTRUM_TIMEOUT_MS = 32;              ///< Spectrum timeout (ms)
constexpr uint32_t CHECK_INTERVAL_MS = 2;                ///< Check interval (ms)
constexpr uint32_t MAX_SCAN_BATCH_SIZE = 10;             ///< Maximum scan batch size

// ===== MOVEMENT TREND =====
constexpr int32_t MOVEMENT_TREND_THRESHOLD_APPROACHING = 3;   ///< Approaching threshold
constexpr int32_t MOVEMENT_TREND_THRESHOLD_RECEEDING = -3;     ///< Receding threshold
constexpr int32_t MOVEMENT_TREND_MIN_HISTORY = 4;              ///< Minimum history for trend
constexpr int32_t MOVEMENT_TREND_SILENCE_THRESHOLD = -110;     ///< Silence threshold (dBm)

// ===== MEMORY =====
constexpr size_t MAX_TRACKED_DRONES = 4;              ///< Maximum tracked drones
constexpr size_t MAX_DISPLAYED_DRONES = 3;            ///< Maximum displayed drones
constexpr size_t DETECTION_TABLE_SIZE = 256;          ///< Detection table size

// ===== AUDIO ALERT PARAMETERS =====
constexpr uint32_t MIN_AUDIO_FREQ = 200;              ///< Minimum audio frequency (Hz)
constexpr uint32_t MAX_AUDIO_FREQ = 20000;            ///< Maximum audio frequency (Hz)
constexpr uint32_t MIN_AUDIO_DURATION = 50;           ///< Minimum audio duration (ms)
constexpr uint32_t MAX_AUDIO_DURATION = 5000;         ///< Maximum audio duration (ms)
constexpr uint32_t DEFAULT_ALERT_FREQ_HZ = 800;       ///< Default alert frequency (Hz)
constexpr uint32_t DEFAULT_ALERT_DURATION_MS = 500;   ///< Default alert duration (ms)
constexpr uint32_t DEFAULT_ALERT_VOLUME_LEVEL = 50;   ///< Default alert volume level
constexpr uint32_t MCU_MAX_AUDIO_FREQ_HZ = 4000;      ///< MCU maximum audio frequency (Hz)
constexpr uint32_t GENERAL_MAX_AUDIO_FREQ_HZ = 20000; ///< General maximum audio frequency (Hz)
constexpr uint32_t DEFAULT_ALERT_COOLDOWN_MS = 100;   ///< Default alert cooldown (ms)
constexpr uint32_t AUDIO_COOLDOWN_MS = 100;            ///< Audio cooldown (ms)
constexpr uint32_t AUDIO_BEEP_DURATION_MS = 50;        ///< Audio beep duration (ms)
constexpr uint32_t AUDIO_BEEP_FREQUENCY_HZ = 800;     ///< Audio beep frequency (Hz)

// ===== BANDWIDTH PARAMETERS =====
constexpr uint32_t MIN_BANDWIDTH = 10000;             ///< Minimum bandwidth (Hz)
constexpr uint32_t MAX_BANDWIDTH = 28000000;          ///< Maximum bandwidth (Hz)

// ===== UI DIMENSIONS =====
constexpr uint32_t SCREEN_WIDTH = 240;                ///< Screen width (pixels)
constexpr uint32_t SCREEN_HEIGHT = 320;               ///< Screen height (pixels)
constexpr uint32_t TEXT_HEIGHT = 16;                   ///< Text height (pixels)
constexpr uint32_t TEXT_LINE_HEIGHT = 24;              ///< Text line height (pixels)
constexpr uint32_t DISPLAY_UPDATE_INTERVAL_MS = 100;   ///< Display update interval (ms)
constexpr uint32_t UI_REFRESH_RATE_MS = 50;            ///< UI refresh rate (ms)
constexpr uint32_t SCREEN_BLANK_TIMEOUT_MS = 60000;   ///< Screen blank timeout (ms)

// ===== SPECTRUM PARAMETERS =====
constexpr uint32_t SPECTRUM_BIN_COUNT = 256;          ///< Spectrum bin count
constexpr uint32_t SPECTRUM_BIN_COUNT_240 = 240;      ///< Spectrum bin count (valid range)
constexpr uint32_t MINI_SPECTRUM_WIDTH = 200;         ///< Mini spectrum width (pixels)
constexpr uint32_t MINI_SPECTRUM_HEIGHT = 24;         ///< Mini spectrum height (pixels)
constexpr uint32_t SPECTRUM_ROW_SIZE = 240;            ///< Spectrum row size (pixels)
constexpr uint32_t RENDER_LINE_SIZE = 240;            ///< Render line size (pixels)
constexpr uint32_t WATERFALL_SIZE = 40 * 240;          ///< Waterfall size (9.6KB)
constexpr uint8_t SPECTRUM_NOISE_FLOOR_DEFAULT = 20;   ///< Default spectrum noise floor
constexpr uint8_t SPECTRUM_PEAK_THRESHOLD_DEFAULT = 10;///< Default spectrum peak threshold
constexpr uint32_t SPECTRUM_UPDATE_RATE_HZ = 60;      ///< Spectrum update rate (Hz)

// ===== BUFFER SIZES =====
constexpr uint32_t ERROR_MESSAGE_BUFFER_SIZE = 128;   ///< Error message buffer size
constexpr uint32_t DEFAULT_BUFFER_SIZE_4KB = 4096;    ///< Default buffer size (4KB)
constexpr uint32_t WORKER_STACK_SIZE_8KB = 8192;       ///< Worker thread stack size (8KB)
constexpr uint32_t DB_LOADING_STACK_SIZE_8KB = 8192;   ///< DB loading thread stack size (8KB)
constexpr uint32_t POOL_SIZE_2KB = 2048;               ///< Pool size (2KB)
constexpr uint32_t POOL_SIZE_1KB = 1024;               ///< Pool size (1KB)
constexpr uint32_t MAX_STRING_LENGTH_256 = 256;         ///< Maximum string length
constexpr uint32_t FREQ_DB_STORAGE_SIZE_4KB = 4096;    ///< Frequency DB storage size (4KB)
constexpr uint32_t FREQ_DB_STORAGE_SIZE_2KB = 2048;    ///< Frequency DB storage size (2KB)

// ===== THREAD STACK SIZES =====
constexpr uint32_t WORKER_STACK_SIZE_4KB = 4096;       ///< Worker thread stack size (4KB)
constexpr uint32_t DB_LOADING_STACK_SIZE_4KB = 4096;    ///< DB loading thread stack size (4KB)
constexpr uint32_t COORDINATOR_STACK_SIZE_6KB = 6144;  ///< Coordinator thread stack size (6KB)

// ===== RSSI PARAMETERS =====
constexpr int32_t RSSI_SILENCE_DBM = -120;             ///< RSSI silence threshold (dBm)
constexpr int32_t RSSI_INVALID_DBM = -127;             ///< Invalid RSSI value (dBm)

// ===== THREAD TIMEOUTS =====
constexpr uint32_t THREAD_JOIN_TIMEOUT_MS = 5000;      ///< Thread join timeout (ms)
constexpr uint32_t THREAD_TERMINATION_TIMEOUT_MS = 3000; ///< Thread termination timeout (ms)

// ===== DATABASE PARAMETERS =====
constexpr uint32_t MAX_DB_ENTRIES = 75;                ///< Maximum database entries (reduced from 150 to save 2KB RAM)
constexpr uint32_t DB_SYNC_INTERVAL_MS = 5000;         ///< Database sync interval (ms)
constexpr uint32_t DB_LOAD_RETRY_COUNT = 3;            ///< Database load retry count
constexpr uint32_t DB_LOAD_RETRY_DELAY_MS = 500;       ///< Database load retry delay (ms)

// ===== WIDEBAND SCANNING =====
constexpr uint32_t WIDEBAND_SLICE_COUNT_DEFAULT = 10;  ///< Default wideband slice count
constexpr uint32_t WIDEBAND_SLICE_COUNT_MIN = 1;       ///< Minimum wideband slice count
constexpr uint32_t WIDEBAND_SLICE_COUNT_MAX = 20;      ///< Maximum wideband slice count

// ===== DETECTION LOGGING =====
constexpr uint32_t MAX_LOG_ENTRIES = 500;              ///< Maximum log entries (reduced from 1000 to save memory)
constexpr uint32_t LOG_FLUSH_INTERVAL_MS = 10000;      ///< Log flush interval (ms)
constexpr uint32_t LOG_FILE_MAX_SIZE_KB = 1024;        ///< Log file max size (KB)

// ===== FREQUENCY HASHING =====
constexpr uint32_t FREQ_HASH_DIVISOR = 100000;         ///< Frequency hash divisor
constexpr uint32_t FREQ_HASH_TABLE_SIZE = 32;          ///< Frequency hash table size
constexpr uint32_t FREQ_HASH_MASK = FREQ_HASH_TABLE_SIZE - 1;  ///< Frequency hash mask

// ===== TIMESTAMPS =====
constexpr Timestamp TIMESTAMP_WRAP_THRESHOLD = 0xFFFFFFFFUL / 2;  ///< Timestamp wrap threshold
constexpr uint32_t TIMESTAMP_MAX_AGE_MS = 60000;        ///< Maximum timestamp age (ms, 1 minute)

// ===== ERROR HANDLING =====
constexpr uint32_t ERROR_MESSAGE_DISPLAY_TIME_MS = 3000; ///< Error message display time (ms)
constexpr uint32_t MAX_ERROR_MESSAGES = 10;             ///< Maximum error messages

// ===== MEMORY POOLS =====
constexpr uint32_t BUFFER_POOL_SIZE = 8;                ///< Buffer pool size
constexpr uint32_t BUFFER_POOL_ENTRY_SIZE = 512;       ///< Buffer pool entry size

// ===== FILE I/O =====
constexpr uint32_t FILE_READ_CHUNK_SIZE = 256;          ///< File read chunk size
constexpr uint32_t FILE_WRITE_CHUNK_SIZE = 512;         ///< File write chunk size
constexpr uint32_t FILE_OPEN_TIMEOUT_MS = 2000;         ///< File open timeout (ms)

// ===== THREAT LEVELS =====
constexpr uint32_t THREAT_LEVEL_CRITICAL_THRESHOLD = 3; ///< Critical threat threshold
constexpr uint32_t THREAT_LEVEL_HIGH_THRESHOLD = 2;     ///< High threat threshold
constexpr uint32_t THREAT_LEVEL_MEDIUM_THRESHOLD = 1;   ///< Medium threat threshold
constexpr uint32_t THREAT_LEVEL_LOW_THRESHOLD = 0;      ///< Low threat threshold

// ===== SETTINGS STORAGE =====
constexpr uint32_t SETTINGS_TEMPLATE_SIZE_4KB = 4096;  ///< Settings template size (4KB)
constexpr uint32_t SETTINGS_TEMPLATE_SIZE_2KB = 2048;   ///< Settings template size (2KB)
constexpr uint32_t MAX_SETTINGS_FILE_SIZE_64KB = 65536; ///< Maximum settings file size (64KB)
constexpr uint32_t MAX_SETTINGS_LINES = 1000;           ///< Maximum settings lines
constexpr uint32_t MAX_LINE_LENGTH = 128;               ///< Maximum line length
constexpr uint32_t MAX_SETTING_STR_LEN = 65;             ///< Maximum setting string length

// ===== STRING LENGTH CONSTANTS =====
constexpr size_t MAX_PATH_LENGTH = 64;                   ///< Maximum path length
constexpr size_t MAX_NAME_LENGTH = 32;                   ///< Maximum name length
constexpr size_t MAX_FORMAT_LENGTH = 8;                  ///< Maximum format length

} // namespace Constants

// ========================================
// VALIDATION UTILITIES
// ========================================

namespace Validation {

// ===== VALIDATION CONSTANTS =====
static constexpr Frequency MIN_HARDWARE_FREQ = 1'000'000ULL;      ///< Minimum hardware frequency (Hz)
static constexpr Frequency MAX_HARDWARE_FREQ = 7'200'000'000ULL;  ///< Maximum hardware frequency (Hz)
static constexpr Frequency MIN_2_4GHZ = 2'400'000'000ULL;        ///< 2.4 GHz band minimum
static constexpr Frequency MAX_2_4GHZ = 2'483'500'000ULL;        ///< 2.4 GHz band maximum
static constexpr Frequency MIN_5_8GHZ = 5'725'000'000ULL;        ///< 5.8 GHz band minimum
static constexpr Frequency MAX_5_8GHZ = 5'875'000'000ULL;        ///< 5.8 GHz band maximum
static constexpr Frequency MIN_MILITARY = 860'000'000ULL;         ///< Military band minimum
static constexpr Frequency MAX_MILITARY = 930'000'000ULL;         ///< Military band maximum
static constexpr Frequency MIN_433MHZ = 433'000'000ULL;          ///< 433 MHz band minimum
static constexpr Frequency MAX_433MHZ = 435'000'000ULL;           ///< 433 MHz band maximum

// ===== VALIDATION FUNCTIONS =====

/**
 * @brief Check if value is in range [min_val, max_val]
 * @param value Value to check
 * @param min_val Minimum value (inclusive)
 * @param max_val Maximum value (inclusive)
 * @return true if value is in range, false otherwise
 */
static constexpr bool is_in_range(Frequency value, Frequency min_val, Frequency max_val) noexcept {
    return value >= min_val && value <= max_val;
}

/**
 * @brief Validate frequency value
 * @param freq_hz Frequency in Hz
 * @return true if frequency is valid, false otherwise
 */
static constexpr bool validate_frequency(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_HARDWARE_FREQ, MAX_HARDWARE_FREQ);
}

/**
 * @brief Validate RSSI value
 * @param rssi_db RSSI in dBm
 * @return true if RSSI is valid, false otherwise
 */
static constexpr bool validate_rssi(int32_t rssi_db) noexcept {
    return rssi_db >= -110 && rssi_db <= 10;
}

/**
 * @brief Check if frequency is in 2.4 GHz band
 * @param freq_hz Frequency in Hz
 * @return true if frequency is in 2.4 GHz band, false otherwise
 */
static constexpr bool is_2_4ghz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_2_4GHZ, MAX_2_4GHZ);
}

/**
 * @brief Check if frequency is in 5.8 GHz band
 * @param freq_hz Frequency in Hz
 * @return true if frequency is in 5.8 GHz band, false otherwise
 */
static constexpr bool is_5_8ghz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_5_8GHZ, MAX_5_8GHZ);
}

/**
 * @brief Check if frequency is in military band
 * @param freq_hz Frequency in Hz
 * @return true if frequency is in military band, false otherwise
 */
static constexpr bool is_military_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_MILITARY, MAX_MILITARY);
}

/**
 * @brief Check if frequency is in 433 MHz band
 * @param freq_hz Frequency in Hz
 * @return true if frequency is in 433 MHz band, false otherwise
 */
static constexpr bool is_433mhz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, MIN_433MHZ, MAX_433MHZ);
}

} // namespace Validation

// ========================================
// UNIFIED SPECTRUM MODE LUT (O(1) Lookup)
// ========================================

namespace LUTs {

/// @brief Number of spectrum modes
constexpr size_t SPECTRUM_MODE_COUNT = 5;

/// @brief Default spectrum mode index (MEDIUM)
constexpr uint8_t DEFAULT_SPECTRUM_MODE_INDEX = 2;

/// @brief Default spectrum mode short name
constexpr const char* DEFAULT_SPECTRUM_MODE_NAME = "MEDIUM";

/// @brief Default spectrum mode display name
constexpr const char* DEFAULT_SPECTRUM_MODE_DISPLAY_NAME = "Medium";

/// @brief Default bandwidth (Hz)
constexpr uint32_t DEFAULT_BANDWIDTH_HZ = 24000000;

/**
 * @brief Spectrum mode information structure
 */
struct SpectrumModeInfo {
    const char* short_name;   ///< Short name (e.g., "NARROW")
    const char* display_name; ///< Display name (e.g., "Narrow Band")
    uint32_t   bandwidth_hz;  ///< Bandwidth in Hz
    uint8_t    ui_index;      ///< UI field index
};

/// @brief Spectrum mode lookup table (Flash-resident)
EDA_FLASH_CONST static constexpr SpectrumModeInfo SPECTRUM_MODES[SPECTRUM_MODE_COUNT] = {
    {"ULTRA_NARROW", "Ultra Narrow",  8000000,  0},
    {"NARROW",       "Narrow",        12000000, 1},
    {"MEDIUM",       "Medium",        24000000, 2},
    {"WIDE",         "Wide",          48000000, 3},
    {"ULTRA_WIDE",   "Ultra Wide",    96000000, 4}
};

/// @brief UI index to mode index lookup table (O(1))
EDA_FLASH_CONST static constexpr std::array<uint8_t, SPECTRUM_MODE_COUNT> UI_INDEX_TO_MODE_IDX = {0, 1, 2, 3, 4};

/**
 * @brief Get spectrum mode short name
 * @param mode_idx Mode index (0-4)
 * @return Short name or default if out of range
 */
inline constexpr const char* spectrum_mode_short_name(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].short_name : DEFAULT_SPECTRUM_MODE_NAME;
}

/**
 * @brief Get spectrum mode display name
 * @param mode_idx Mode index (0-4)
 * @return Display name or default if out of range
 */
inline constexpr const char* spectrum_mode_display_name(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].display_name : DEFAULT_SPECTRUM_MODE_DISPLAY_NAME;
}

/**
 * @brief Get spectrum mode bandwidth
 * @param mode_idx Mode index (0-4)
 * @return Bandwidth in Hz or default if out of range
 */
inline constexpr uint32_t spectrum_mode_bandwidth(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].bandwidth_hz : DEFAULT_BANDWIDTH_HZ;
}

/**
 * @brief Get spectrum mode UI index
 * @param mode_idx Mode index (0-4)
 * @return UI index or default if out of range
 */
inline constexpr uint8_t spectrum_mode_ui_index(uint8_t mode_idx) noexcept {
    return (mode_idx < SPECTRUM_MODE_COUNT) ? SPECTRUM_MODES[mode_idx].ui_index : DEFAULT_SPECTRUM_MODE_INDEX;
}

/**
 * @brief Convert UI index to spectrum mode index (O(1) lookup)
 * @param ui_index UI field index (0-4)
 * @return Mode index or default if out of range
 */
inline constexpr uint8_t ui_index_to_spectrum_mode(uint8_t ui_index) noexcept {
    return (ui_index < SPECTRUM_MODE_COUNT) ? UI_INDEX_TO_MODE_IDX[ui_index] : DEFAULT_SPECTRUM_MODE_INDEX;
}

} // namespace LUTs

// ========================================
// ERROR MESSAGE LUT (Flash-Resident)
// ========================================

namespace ErrorHandling {

/// @brief Number of error messages
constexpr size_t ERROR_MESSAGE_COUNT = 10;

/// @brief Error message lookup table (Flash-resident)
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

// ========================================
// UNIFIED FREQUENCY FORMATTING (Zero-Heap)
// ========================================

namespace Formatting {

/// @brief Maximum formatted frequency length (bytes)
constexpr size_t MAX_FORMATTED_FREQ_LENGTH = 32;

/**
 * @brief Frequency scale information structure
 */
struct FrequencyScale {
    Frequency   threshold_hz;  ///< Threshold frequency for this scale
    const char* unit;          ///< Unit string (e.g., "G", "M", "k")
    Frequency   divider;       ///< Divider for this scale
};

/// @brief Number of frequency scales
constexpr size_t FREQUENCY_SCALE_COUNT = 4;

/// @brief Frequency scale lookup table (Flash-resident)
EDA_FLASH_CONST static constexpr FrequencyScale FREQUENCY_SCALES[FREQUENCY_SCALE_COUNT] = {
    {1'000'000'000ULL, "G", 1'000'000'000ULL},  // GHz
    {1'000'000ULL,     "M", 1'000'000ULL},        // MHz
    {1'000ULL,         "k", 1'000ULL},            // kHz
    {0ULL,             "",  1ULL}                  // Hz (fallback)
};

/**
 * @brief Format frequency to string (detailed format)
 * @param buffer Output buffer (must be at least MAX_FORMATTED_FREQ_LENGTH bytes)
 * @param size Buffer size
 * @param freq_hz Frequency in Hz
 * @note UI-only function, not for ISR/DSP (uses snprintf)
 * @note Marked noexcept for ISR safety
 */
inline void format_frequency(char* buffer, size_t size, Frequency freq_hz) noexcept {
    // Guard clause: validate buffer
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
        // Single decimal place for precision
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

/**
 * @brief Format frequency to string (compact format for UI)
 * @param buffer Output buffer (must be at least MAX_FORMATTED_FREQ_LENGTH bytes)
 * @param size Buffer size
 * @param freq_hz Frequency in Hz
 * @note UI-only function, not for ISR/DSP (uses snprintf)
 * @note Marked noexcept for ISR safety
 */
inline void format_frequency_compact(char* buffer, size_t size, Frequency freq_hz) noexcept {
    // Guard clause: validate buffer
    if (!buffer || size < MAX_FORMATTED_FREQ_LENGTH) {
        return;
    }

    // DIAMOND FIX: Cast to uint64_t to avoid signed/unsigned comparison warnings
    // Frequency is int64_t (rf::Frequency), but we compare with unsigned literals
    const uint64_t freq_u64 = static_cast<uint64_t>(freq_hz);

    // GHz range
    if (freq_u64 >= 1'000'000'000ULL) {
        // Check for overflow before addition
        if (freq_u64 > UINT64_MAX - 500'000'000ULL) {
            // Clamp to max value
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
    // MHz range
    else if (freq_u64 >= 1'000'000ULL) {
        uint32_t mhz = static_cast<uint32_t>((freq_u64 + 500'000ULL) / 1'000'000ULL);
        snprintf(buffer, size, "%" PRIu32 "M", mhz);
    }
    // kHz range
    else if (freq_u64 >= 1'000ULL) {
        uint32_t khz = static_cast<uint32_t>((freq_u64 + 500ULL) / 1'000ULL);
        snprintf(buffer, size, "%" PRIu32 "k", khz);
    }
    // Hz range
    else {
        snprintf(buffer, size, "%" PRIu64, freq_u64);
    }
}

} // namespace Formatting

// ========================================
// ERROR HANDLING (Zero-Overhead)
// ========================================

/**
 * @brief Error code enumeration (enum class for type safety)
 */
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

/**
 * @brief Error result template (zero-overhead error handling)
 * @tparam T Value type
 */
template<typename T>
struct ErrorResult {
    ErrorCode error_code;  ///< Error code
    T value;               ///< Return value

    /**
     * @brief Create success result
     * @param val Return value
     * @return ErrorResult with SUCCESS error code
     */
    static constexpr ErrorResult ok(T val) noexcept {
        return ErrorResult{ErrorCode::SUCCESS, val};
    }

    /**
     * @brief Create error result
     * @param code Error code
     * @return ErrorResult with specified error code
     */
    static constexpr ErrorResult fail(ErrorCode code) noexcept {
        return ErrorResult{code, T{}};
    }

    /**
     * @brief Create error result with default value
     * @param code Error code
     * @param default_val Default return value
     * @return ErrorResult with specified error code and default value
     */
    static constexpr ErrorResult fail(ErrorCode code, T default_val) noexcept {
        return ErrorResult{code, default_val};
    }

    /**
     * @brief Check if result is success
     * @return true if success, false otherwise
     */
    constexpr bool is_ok() const noexcept { return error_code == ErrorCode::SUCCESS; }

    /**
     * @brief Check if result is error
     * @return true if error, false otherwise
     */
    constexpr bool is_error() const noexcept { return error_code != ErrorCode::SUCCESS; }

    /**
     * @brief Boolean conversion (true if success)
     * @return true if success, false otherwise
     */
    constexpr explicit operator bool() const noexcept { return is_ok(); }

    /**
     * @brief Get error message
     * @return Error message string
     */
    constexpr const char* error_message() const noexcept {
        const size_t code = static_cast<size_t>(error_code);
        if (code < ErrorHandling::ERROR_MESSAGE_COUNT) {
            return ErrorHandling::ERROR_MESSAGES[code];
        }
        return "Unknown error";
    }
};

// ========================================
// SAFE TYPE CASTING (Zero-Overhead)
// ========================================

/**
 * @brief Safe reinterpret_cast wrapper with alignment checks
 * @tparam To Target type
 * @tparam From Source type
 * @param ptr Source pointer
 * @return Reinterpreted pointer
 * @note Compile-time alignment check (static_assert)
 * @note Release build: zero overhead (optimizes to plain reinterpret_cast)
 */
template<typename To, typename From>
constexpr To safe_reinterpret_cast(From* ptr) noexcept {
    static_assert(alignof(To) <= alignof(std::max_align_t), "Target alignment too large");
    return reinterpret_cast<To>(ptr);
}

/**
 * @brief Safe reinterpret_cast wrapper for volatile pointers
 * @tparam To Target type
 * @tparam From Source type
 * @param ptr Source pointer
 * @return Reinterpreted volatile pointer
 */
template<typename To, typename From>
constexpr To safe_reinterpret_cast_volatile(From* ptr) noexcept {
    static_assert(alignof(To) <= alignof(std::max_align_t), "Target alignment too large");
    return reinterpret_cast<To>(ptr);
}

/**
 * @brief Safe reinterpret_cast wrapper for address
 * @tparam To Target type
 * @param addr Address value
 * @return Reinterpreted pointer
 */
template<typename To>
constexpr To safe_reinterpret_cast_addr(uintptr_t addr) noexcept {
    static_assert(alignof(To) <= alignof(std::max_align_t), "Target alignment too large");
    return reinterpret_cast<To>(addr);
}

} // namespace EDA

// Backward compatibility aliases
namespace DroneConstants = EDA::Constants;

#endif // EDA_CONSTANTS_HPP_
