#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include <cstddef>
#include "drone_types.hpp"

namespace drone_analyzer {

// ============================================================================
// Frequency Constants
// ============================================================================

/**
 * @brief Minimum frequency in Hz (1 MHz - HackRF One minimum)
 */
constexpr FreqHz MIN_FREQUENCY_HZ = 1'000'000ULL;

/**
 * @brief Maximum frequency in Hz (7.2 GHz - HackRF One maximum)
 */
constexpr FreqHz MAX_FREQUENCY_HZ = 7'200'000'000ULL;

/**
 * @brief Frequency step size in Hz (1 MHz)
 */
constexpr FreqHz FREQUENCY_STEP_HZ = 1'000'000ULL;

/**
 * @brief Frequency bandwidth in Hz (20 MHz)
 */
constexpr FreqHz FREQUENCY_BANDWIDTH_HZ = 20'000'000ULL;

/**
 * @brief Default spectrum start frequency in Hz (2400 MHz)
 */
constexpr FreqHz DEFAULT_SPECTRUM_START_HZ = 2'400'000'000ULL;

/**
 * @brief Default spectrum end frequency in Hz (6000 MHz)
 */
constexpr FreqHz DEFAULT_SPECTRUM_END_HZ = 6'000'000'000ULL;

/**
 * @brief Default histogram start frequency in Hz (2400 MHz)
 */
constexpr FreqHz DEFAULT_HISTOGRAM_START_HZ = 2'400'000'000ULL;

/**
 * @brief Default histogram end frequency in Hz (6000 MHz)
 */
constexpr FreqHz DEFAULT_HISTOGRAM_END_HZ = 6'000'000'000ULL;



// ============================================================================
// Buffer Size Constants
// ============================================================================

/**
 * @brief Maximum number of tracked drones
 */
constexpr size_t MAX_TRACKED_DRONES = 16;

/**
 * @brief Maximum number of displayed drones
 */
constexpr size_t MAX_DISPLAYED_DRONES = 16;

/**
 * @brief Spectrum buffer size (bytes)
 * @note Must match ChannelSpectrum::db.size() (256 bins)
 */
constexpr size_t SPECTRUM_BUFFER_SIZE = 256;

/**
 * @brief Histogram buffer size (bins)
 */
constexpr size_t HISTOGRAM_BUFFER_SIZE = 128;

/**
 * @brief RSSI history size for each drone
 * @note Must be >= MOVEMENT_TREND_MIN_HISTORY for trend calculation
 */
constexpr size_t RSSI_HISTORY_SIZE = 6;

/**
 * @brief Timestamp history size for each drone
 */
constexpr size_t TIMESTAMP_HISTORY_SIZE = 3;

/**
 * @brief Maximum number of frequency entries to scan
 */
constexpr size_t MAX_ENTRIES_TO_SCAN = 100;

/**
 * @brief Frequency hash table size
 */
constexpr size_t FREQUENCY_HASH_TABLE_SIZE = 256;

// ============================================================================
// Hardware Constants
// ============================================================================

/**
 * @brief Default sample rate in Hz (2 MHz)
 */
constexpr uint32_t DEFAULT_SAMPLE_RATE_HZ = 2000000;

/**
 * @brief Default gain value
 */
constexpr uint16_t DEFAULT_GAIN = 20;

/**
 * @brief Default LNA gain value
 */
constexpr uint8_t DEFAULT_LNA_GAIN = 8;

/**
 * @brief Default VGA gain value
 */
constexpr uint8_t DEFAULT_VGA_GAIN = 12;

// ============================================================================
// Time Constants
// ============================================================================

/**
 * @brief Drone stale timeout in milliseconds (5 seconds)
 */
constexpr uint32_t DRONE_STALE_TIMEOUT_MS = 5000;

/**
 * @brief Drone removal timeout in milliseconds (30 seconds)
 */
constexpr uint32_t DRONE_REMOVAL_TIMEOUT_MS = 30000;

/**
 * @brief Scan cycle interval in milliseconds (100 ms)
 */
constexpr uint32_t SCAN_CYCLE_INTERVAL_MS = 100;

/**
 * @brief PLL lock timeout in milliseconds
 */
constexpr uint32_t PLL_LOCK_TIMEOUT_MS = 100;

/**
 * @brief PLL lock poll interval in milliseconds
 */
constexpr uint32_t PLL_LOCK_POLL_INTERVAL_MS = 3;

/**
 * @brief Hardware retry delay in milliseconds
 */
constexpr uint32_t HARDWARE_RETRY_DELAY_MS = 10;

/**
 * @brief SD card operation timeout in milliseconds
 */
constexpr uint32_t SD_CARD_TIMEOUT_MS = 1000;

// ============================================================================
// RSSI Constants
// ============================================================================

/**
 * @brief Minimum RSSI value (dBm)
 */
constexpr int32_t RSSI_MIN_DBM = -120;

/**
 * @brief Maximum RSSI value (dBm)
 */
constexpr int32_t RSSI_MAX_DBM = -20;

/**
 * @brief RSSI threshold for signal detection (dBm)
 */
constexpr int32_t RSSI_DETECTION_THRESHOLD_DBM = -90;

/**
 * @brief RSSI threshold for high threat (dBm)
 */
constexpr int32_t RSSI_HIGH_THREAT_THRESHOLD_DBM = -60;

/**
 * @brief RSSI threshold for critical threat (dBm)
 */
constexpr int32_t RSSI_CRITICAL_THREAT_THRESHOLD_DBM = -40;

/**
 * @brief Noise floor RSSI (dBm)
 */
constexpr int32_t RSSI_NOISE_FLOOR_DBM = -100;

// ============================================================================
// Histogram Constants
// ============================================================================

/**
 * @brief Histogram bin count
 */
constexpr size_t HISTOGRAM_BIN_COUNT = 64;

/**
 * @brief Histogram scale factor
 */
constexpr uint8_t HISTOGRAM_SCALE_FACTOR = 4;

/**
 * @brief Histogram max value (uint8_t max)
 */
constexpr uint8_t HISTOGRAM_MAX_VALUE = 255;

/**
 * @brief Histogram extended size (for calculations)
 */
constexpr size_t HISTOGRAM_EXTENDED_SIZE = 256;

/**
 * @brief Histogram buffer half size
 */
constexpr size_t HISTOGRAM_HALF_SIZE = 128;

/**
 * @brief Histogram noise floor threshold
 */
constexpr uint8_t HISTOGRAM_NOISE_FLOOR = 10;

/**
 * @brief Histogram signal threshold
 */
constexpr uint8_t HISTOGRAM_SIGNAL_THRESHOLD = 20;

// ============================================================================
// Display Constants
// ============================================================================

/**
 * @brief Display width in pixels
 */
constexpr uint16_t DISPLAY_WIDTH = 240;

/**
 * @brief Display height in pixels
 */
constexpr uint16_t DISPLAY_HEIGHT = 320;

/**
 * @brief Font height in pixels
 */
constexpr uint8_t FONT_HEIGHT = 8;

/**
 * @brief Font width in pixels
 */
constexpr uint8_t FONT_WIDTH = 5;

/**
 * @brief Maximum text length for display
 */
constexpr size_t MAX_TEXT_LENGTH = 32;

/**
 * @brief Drone type name length
 */
constexpr size_t DRONE_TYPE_NAME_LENGTH = 16;

// ============================================================================
// Threat Level Constants
// ============================================================================

/**
 * @brief Number of threat levels
 */
constexpr size_t THREAT_LEVEL_COUNT = 5;

/**
 * @brief High threat approaching count threshold
 */
constexpr size_t HIGH_THREAT_APPROACHING_COUNT = 3;

/**
 * @brief Critical threat approaching count threshold
 */
constexpr size_t CRITICAL_THREAT_APPROACHING_COUNT = 5;

// ============================================================================
// Scanning Constants
// ============================================================================

/**
 * @brief Default scanning mode
 */
constexpr ScanningMode DEFAULT_SCANNING_MODE = ScanningMode::SEQUENTIAL;

/**
 * @brief Number of scanning modes
 */
constexpr uint8_t SCANNING_MODE_COUNT = 4;

/**
 * @brief Maximum scan cycles before timeout
 */
constexpr uint32_t MAX_SCAN_CYCLES = 1000;

// ============================================================================
// Database Constants
// ============================================================================

/**
 * @brief Maximum database entries (reduced for memory constraints)
 * @note 96 entries × 80 bytes = 7,680 bytes (within budget)
 */
constexpr size_t MAX_DATABASE_ENTRIES = 96;

/**
 * @brief Database file path
 */
constexpr const char DATABASE_FILE_PATH[] = "/FREQMAN/DRONES.TXT";

/**
 * @brief Database load timeout in milliseconds
 */
constexpr uint32_t DATABASE_LOAD_TIMEOUT_MS = 2000;

/**
 * @line Database line buffer size
 */
constexpr size_t DATABASE_LINE_BUFFER_SIZE = 128;

// ============================================================================
// Settings Constants
// ============================================================================

/**
 * @brief Settings file path
 */
constexpr const char SETTINGS_FILE_PATH[] = "/EDA/SETTINGS.TXT";

/**
 * @brief Settings file version
 */
constexpr uint32_t SETTINGS_FILE_VERSION = 1;

// ============================================================================
// Audio Constants
// ============================================================================

/**
 * @brief Audio alert sample rate in Hz
 */
constexpr uint32_t AUDIO_ALERT_SAMPLE_RATE_HZ = 24000;

/**
 * @brief Audio alert frequency in Hz (default)
 */
constexpr uint32_t AUDIO_ALERT_FREQUENCY_HZ = 1000;

/**
 * @brief Audio alert high threat frequency in Hz
 */
constexpr uint32_t AUDIO_ALERT_HIGH_FREQUENCY_HZ = 1200;

/**
 * @brief Audio alert critical threat frequency in Hz
 */
constexpr uint32_t AUDIO_ALERT_CRITICAL_FREQUENCY_HZ = 1500;

/**
 * @brief Audio alert low frequency in Hz (receding)
 */
constexpr uint32_t AUDIO_ALERT_LOW_FREQUENCY_HZ = 800;

/**
 * @brief Audio alert duration in milliseconds (default)
 */
constexpr uint32_t AUDIO_ALERT_DURATION_MS = 150;

/**
 * @brief Audio alert short duration in milliseconds (critical)
 */
constexpr uint32_t AUDIO_ALERT_SHORT_DURATION_MS = 80;

/**
 * @brief Audio alert medium duration in milliseconds (threat increased)
 */
constexpr uint32_t AUDIO_ALERT_MEDIUM_DURATION_MS = 100;

/**
 * @brief Audio alert long duration in milliseconds (approaching)
 */
constexpr uint32_t AUDIO_ALERT_LONG_DURATION_MS = 200;

/**
 * @brief Audio alert gap in milliseconds (short)
 */
constexpr uint32_t AUDIO_ALERT_SHORT_GAP_MS = 40;

/**
 * @brief Audio alert gap in milliseconds (long)
 */
constexpr uint32_t AUDIO_ALERT_LONG_GAP_MS = 50;

// ============================================================================
// Memory Constants
// ============================================================================

/**
 * @brief Memory breakdown (simplified):
 * - Database entries: 96 × 12 = 1,152 bytes
 * - Tracked drones: 16 × 56 = 896 bytes
 * - Display drones: 16 × 39 = 624 bytes
 * - Spectrum buffer: 256 bytes
 * - Histogram buffer: 512 bytes
 * - Histogram processor: 256 bytes
 * - RSSI detector: ~60 bytes
 * - Scanner thread stack: 2,048 bytes (BSS)
 * - Other structures: ~200 bytes
 * - Total static RAM: ~6,004 bytes
 */
constexpr size_t STATIC_RAM_BUDGET_BYTES = 6004;

/**
 * @brief Total stack budget (bytes)
 * @note Updated to accommodate deeper call stacks for alert handling
 */
constexpr size_t STACK_BUDGET_BYTES = 4096;

/**
 * @brief Total memory budget (bytes)
 * @note Sum of static RAM and stack budgets
 */
constexpr size_t TOTAL_MEMORY_BUDGET_BYTES = 10100;

/**
 * @brief Maximum stack usage per function (bytes)
 */
constexpr size_t MAX_STACK_PER_FUNCTION = 512;

// ============================================================================
// Struct Size Validation Constants
// ============================================================================

// ============================================================================
// Error Handling Constants
// ============================================================================

/**
 * @brief Maximum hardware retry attempts
 */
constexpr uint32_t MAX_HARDWARE_RETRIES = 3;

/**
 * @brief Maximum database retry attempts
 */
constexpr uint32_t MAX_DATABASE_RETRIES = 3;

/**
 * @brief Maximum mutex retry attempts
 */
constexpr uint32_t MAX_MUTEX_RETRIES = 3;

// ============================================================================
// Thread Constants
// ============================================================================

/**
 * @brief Scanner thread priority
 */
constexpr uint8_t SCANNER_THREAD_PRIORITY = 10;

/**
 * @brief UI thread priority
 */
constexpr uint8_t UI_THREAD_PRIORITY = 5;

/**
 * @brief Scanner thread stack size (bytes)
 */
constexpr size_t SCANNER_THREAD_STACK_SIZE = 2048;

/**
 * @brief UI thread stack size (bytes)
 */
constexpr size_t UI_THREAD_STACK_SIZE = 4096;

// ============================================================================
// Color Constants (RGBA)
// ============================================================================

/**
 * @brief Color for low threat (green)
 */
constexpr uint32_t COLOR_LOW_THREAT = 0xFF00FF00;

/**
 * @brief Color for medium threat (yellow)
 */
constexpr uint32_t COLOR_MEDIUM_THREAT = 0xFFFFFF00;

/**
 * @brief Color for high threat (orange)
 */
constexpr uint32_t COLOR_HIGH_THREAT = 0xFFFF8000;

/**
 * @brief Color for critical threat (red)
 */
constexpr uint32_t COLOR_CRITICAL_THREAT = 0xFFFF0000;

/**
 * @brief Color for unknown threat (gray)
 */
constexpr uint32_t COLOR_UNKNOWN_THREAT = 0xFF808080;

/**
 * @brief Color for background (black)
 */
constexpr uint32_t COLOR_BACKGROUND = 0xFF000000;

/**
 * @brief Color for text (white)
 */
constexpr uint32_t COLOR_TEXT = 0xFFFFFFFF;

// ============================================================================
// String Constants (Flash Storage)
// ============================================================================

/**
 * @brief String for unknown drone type
 */
constexpr const char DRONE_TYPE_UNKNOWN[] = "Unknown";
constexpr const char DRONE_TYPE_DJI[] = "DJI";
constexpr const char DRONE_TYPE_PARROT[] = "Parrot";
constexpr const char DRONE_TYPE_YUNEEC[] = "Yuneec";
constexpr const char DRONE_TYPE_3DR[] = "3DR";
constexpr const char DRONE_TYPE_AUTEL[] = "Autel";
constexpr const char DRONE_TYPE_HOBBY[] = "Hobby";
constexpr const char DRONE_TYPE_FPV[] = "FPV";
constexpr const char DRONE_TYPE_CUSTOM[] = "Custom";
constexpr const char DRONE_TYPE_OTHER[] = "Other";

// ============================================================================
// Movement Trend Constants
// ============================================================================

/**
 * @brief Threshold for drone approaching detection (dB)
 * @note RSSI increase of 3 dB means drone is getting closer
 */
constexpr int32_t MOVEMENT_TREND_THRESHOLD_APPROACHING_DB = 3;

/**
 * @brief Threshold for drone receding detection (dB)
 * @note RSSI decrease of 3 dB means drone is moving away
 */
constexpr int32_t MOVEMENT_TREND_THRESHOLD_RECEEDING_DB = -3;

/**
 * @brief Minimum history samples for trend calculation
 * @note Need at least 3 samples for reliable trend
 * @note Must be <= RSSI_HISTORY_SIZE
 */
constexpr uint8_t MOVEMENT_TREND_MIN_HISTORY = 3;

/**
 * @brief Silence threshold for RSSI filtering (dBm)
 * @note RSSI values below -110 dBm are ignored as noise
 */
constexpr int32_t MOVEMENT_TREND_SILENCE_THRESHOLD_DBM = -110;

/**
 * @brief Movement trend symbols for UI display
 */
constexpr char MOVEMENT_TREND_SYMBOL_APPROACHING = '<';
constexpr char MOVEMENT_TREND_SYMBOL_RECEEDING = '>';
constexpr char MOVEMENT_TREND_SYMBOL_STATIC = '~';
constexpr char MOVEMENT_TREND_SYMBOL_UNKNOWN = '-';

// ============================================================================
// Status Messages (Flash Storage)
// ============================================================================

/**
 * @brief Status message for no drones detected
 */
constexpr const char STATUS_NO_DRONES[] = "No drones detected";
constexpr const char STATUS_SCANNING[] = "Scanning...";
constexpr const char STATUS_ERROR[] = "Error";
constexpr const char STATUS_LOADING[] = "Loading...";
constexpr const char STATUS_READY[] = "Ready";

// ============================================================================
// Validation Constants
// ============================================================================

/**
 * @brief Maximum frequency entry name length
 */
constexpr size_t MAX_FREQUENCY_NAME_LENGTH = 32;

/**
 * @brief Maximum frequency description length
 */
constexpr size_t MAX_FREQUENCY_DESCRIPTION_LENGTH = 64;

/**
 * @brief Maximum file path length
 */
constexpr size_t MAX_FILE_PATH_LENGTH = 64;

// ============================================================================
// Performance Constants
// ============================================================================

/**
 * @brief Target UI refresh rate (FPS)
 */
constexpr uint32_t TARGET_UI_FPS = 60;

/**
 * @brief UI refresh interval in milliseconds
 */
constexpr uint32_t UI_REFRESH_INTERVAL_MS = 16;

/**
 * @brief Target scan rate (frequencies per second)
 */
constexpr uint32_t TARGET_SCAN_RATE_HZ = 10;

// ============================================================================
// Fast Scanner Constants
// ============================================================================

/**
 * @brief Scanner sleep time per frequency (ms)
 * @note 50ms = 20 frequencies/second
 */
constexpr uint32_t SCANNER_SLEEP_MS = 50;

/**
 * @brief Statistics updates per second
 */
constexpr uint32_t STATISTICS_UPDATES_PER_SEC = 10;

/**
 * @brief Maximum frequency lock cycles
 * @note 10 cycles × 50ms = 500ms to verify signal
 */
constexpr uint32_t MAX_FREQ_LOCK = 10;

/**
 * @brief Signal lock time (ms)
 * @note 10 × 50ms = 500ms
 */
constexpr uint32_t SIGNAL_LOCK_TIME_MS = 500;

/**
 * @brief Drone type display duration (ms)
 * @note 500ms for showing "FPV", "DJI", etc.
 */
constexpr uint32_t DRONE_TYPE_DISPLAY_DURATION_MS = 500;

/**
 * @brief Maximum drone type display length
 * @note 4 characters + null terminator
 */
constexpr size_t MAX_DRONE_TYPE_DISPLAY = 4;

// ============================================================================
// Debug Constants
// ============================================================================

#ifdef DEBUG
/**
 * @brief Enable debug logging
 */
constexpr bool DEBUG_LOGGING_ENABLED = true;

/**
 * @brief Enable lock order validation
 */
constexpr bool DEBUG_LOCK_ORDER_VALIDATION = true;

/**
 * @brief Enable stack usage monitoring
 */
constexpr bool DEBUG_STACK_MONITORING = true;
#else
/**
 * @brief Disable debug logging
 */
constexpr bool DEBUG_LOGGING_ENABLED = false;

/**
 * @brief Disable lock order validation
 */
constexpr bool DEBUG_LOCK_ORDER_VALIDATION = false;

/**
 * @brief Disable stack usage monitoring
 */
constexpr bool DEBUG_STACK_MONITORING = false;
#endif

} // namespace drone_analyzer

#endif // CONSTANTS_HPP
