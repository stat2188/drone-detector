/**
 * @file eda_runtime_constants.hpp
 * @brief Runtime constants for Enhanced Drone Analyzer
 *
 * DIAMOND CODE COMPLIANCE:
 * - All constants stored in Flash (constexpr)
 * - No magic numbers (all constants have semantic names)
 * - Stack allocation only (no heap allocation)
 * - Zero-Overhead Abstraction (constexpr for compile-time evaluation)
 *
 * CONSTANT CATEGORIES:
 * - DSP Processing Constants: Spectrum processing, detection thresholds
 * - UI Rendering Constants: Display dimensions, colors, fonts
 * - Audio Alert Constants: Alert types, cooldown periods, volumes
 * - File I/O Constants: Buffer sizes, timeouts, paths
 * - PLL Control Constants: Lock timeouts, stabilization delays
 * - Thread Synchronization Constants: Timeout values, stack sizes
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 * Environment: ChibiOS RTOS
 */

#ifndef EDA_RUNTIME_CONSTANTS_HPP_
#define EDA_RUNTIME_CONSTANTS_HPP_

// ============================================================================
// STANDARD LIBRARY HEADERS
// ============================================================================
#include <cstdint>
#include <cstddef>

// ============================================================================
// PROJECT HEADERS
// ============================================================================
#include "eda_constants.hpp"
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer::runtime {

// ============================================================================
// DSP PROCESSING CONSTANTS
// ============================================================================

/**
 * @brief DSP processing constants
 * @note All values are constexpr and stored in Flash
 */
namespace DSP {
    // Spectrum Processing
    constexpr inline static unsigned int SPECTRUM_BIN_COUNT = 240;
    constexpr inline static unsigned int SPECTRUM_BIN_COUNT_MINI = 128;
    constexpr inline static unsigned int SPECTRUM_HEIGHT = 40;
    constexpr inline static unsigned int SPECTRUM_HEIGHT_MINI = 20;
    constexpr inline static unsigned int SPECTRUM_BAR_WIDTH = 1;

    // Detection Thresholds (dBm)
    constexpr inline static int RSSI_SILENCE_DBM = -120;
    constexpr inline static int RSSI_NOISE_FLOOR_DBM = -100;
    constexpr inline static int RSSI_DETECTION_THRESHOLD_DBM = -90;
    constexpr inline static int RSSI_HIGH_THRESHOLD_DBM = -80;
    constexpr inline static int RSSI_VERY_HIGH_THRESHOLD_DBM = -60;

    // Detection Counters
    constexpr inline static unsigned int DETECTION_DELAY = 2;
    constexpr inline static unsigned int MIN_DETECTIONS_FOR_CONFIRMATION = 3;
    constexpr inline static unsigned int MAX_DETECTIONS_PER_CYCLE = 10;

    // FHSS Detection
    constexpr inline static unsigned long long FHSS_HOP_THRESHOLD_HZ = 1000000ULL;  // 1 MHz
    constexpr inline static unsigned int FHSS_HOP_TIME_WINDOW_MS = 100;
    constexpr inline static unsigned int FHSS_MIN_HOP_COUNT = 3;
    constexpr inline static unsigned int FHSS_TRACKING_WINDOW_MS = 1000;

    // Noise Floor Estimation
    constexpr inline static unsigned int NOISE_FLOOR_SAMPLES = 100;
    constexpr inline static unsigned int NOISE_FLOOR_UPDATE_INTERVAL_MS = 5000;

    // Peak Detection
    constexpr inline static unsigned int PEAK_DETECTION_THRESHOLD = 10;
    constexpr inline static unsigned int PEAK_HOLD_TIME_MS = 100;
}

// ============================================================================
// UI RENDERING CONSTANTS
// ============================================================================

/**
 * @brief UI rendering constants
 * @note All values are constexpr and stored in Flash
 */
namespace UI {
    // Display Dimensions
    constexpr inline static unsigned int DISPLAY_WIDTH = 240;
    constexpr inline static unsigned int DISPLAY_HEIGHT = 320;

    // Drone List Display
    constexpr inline static unsigned int MAX_UI_DRONES = 10;
    constexpr inline static unsigned int DRONE_LIST_Y_START = 40;
    constexpr inline static unsigned int DRONE_LIST_Y_END = 280;
    constexpr inline static unsigned int DRONE_LIST_ITEM_HEIGHT = 24;

    // Mini Spectrum Display
    constexpr inline static unsigned int MINI_SPECTRUM_Y = 285;
    constexpr inline static unsigned int MINI_SPECTRUM_HEIGHT = 35;
    constexpr inline static unsigned int MINI_SPECTRUM_WIDTH = 240;

    // Text Display
    constexpr inline static unsigned int TEXT_Y_OFFSET = 2;
    constexpr inline static unsigned int TEXT_X_OFFSET = 4;

    // Color Palette
    constexpr inline static unsigned int COLOR_PALETTE_SIZE = 256;

    // Refresh Rates
    constexpr inline static unsigned int UI_REFRESH_INTERVAL_MS = 50;
    constexpr inline static unsigned int UI_FAST_REFRESH_INTERVAL_MS = 20;
    constexpr inline static unsigned int UI_SLOW_REFRESH_INTERVAL_MS = 100;
}

// ============================================================================
// AUDIO ALERT CONSTANTS
// ============================================================================

/**
 * @brief Audio alert constants
 * @note All values are constexpr and stored in Flash
 */
namespace Audio {
    // Alert Types
    enum class AlertType : unsigned char {
        NONE = 0,
        DETECTION = 1,
        HIGH_THREAT = 2,
        CRITICAL_THREAT = 3,
        FHSS_DETECTED = 4
    };

    // Alert Duration (ms)
    constexpr inline static unsigned int ALERT_DURATION_SHORT_MS = 100;
    constexpr inline static unsigned int ALERT_DURATION_MEDIUM_MS = 250;
    constexpr inline static unsigned int ALERT_DURATION_LONG_MS = 500;

    // Alert Cooldown (ms)
    constexpr inline static unsigned int ALERT_COOLDOWN_MS = 2000;
    constexpr inline static unsigned int ALERT_COOLDOWN_SHORT_MS = 1000;

    // Alert Volume (0-255)
    constexpr inline static unsigned int ALERT_VOLUME_LOW = 64;
    constexpr inline static unsigned int ALERT_VOLUME_MEDIUM = 128;
    constexpr inline static unsigned int ALERT_VOLUME_HIGH = 192;
    constexpr inline static unsigned int ALERT_VOLUME_MAX = 255;

    // Alert Frequency (Hz)
    constexpr inline static unsigned int ALERT_FREQUENCY_HZ = 1000;
    constexpr inline static unsigned int ALERT_FREQUENCY_HIGH_HZ = 2000;

    // Maximum Concurrent Alerts
    constexpr inline static unsigned int MAX_CONCURRENT_ALERTS = 1;
}

// ============================================================================
// FILE I/O CONSTANTS
// ============================================================================

/**
 * @brief File I/O constants
 * @note All values are constexpr and stored in Flash
 */
namespace FileIO {
    // Buffer Sizes
    constexpr inline static unsigned int FILE_BUFFER_SIZE = 512;
    constexpr inline static unsigned int FILE_BUFFER_SIZE_SMALL = 256;
    constexpr inline static unsigned int FILE_BUFFER_SIZE_LARGE = 1024;

    // Path Lengths
    constexpr inline static unsigned int MAX_PATH_LENGTH = 64;
    constexpr inline static unsigned int MAX_FILENAME_LENGTH = 32;

    // Timeouts (ms)
    constexpr inline static unsigned int FILE_OPEN_TIMEOUT_MS = 1000;
    constexpr inline static unsigned int FILE_WRITE_TIMEOUT_MS = 2000;
    constexpr inline static unsigned int FILE_READ_TIMEOUT_MS = 2000;
    constexpr inline static unsigned int FILE_CLOSE_TIMEOUT_MS = 500;

    // Retry Counts
    constexpr inline static unsigned int FILE_RETRY_COUNT = 3;
    constexpr inline static unsigned int FILE_RETRY_DELAY_MS = 100;

    // CSV Logging
    constexpr inline static unsigned int CSV_LINE_BUFFER_SIZE = 128;
    constexpr inline static unsigned int CSV_MAX_ENTRIES_PER_WRITE = 10;
}

// ============================================================================
// PLL CONTROL CONSTANTS
// ============================================================================

/**
 * @brief PLL control constants
 * @note All values are constexpr and stored in Flash
 */
namespace PLL {
    // Lock Verification
    constexpr inline static unsigned int PLL_LOCK_RETRIES = 3;
    constexpr inline static unsigned int PLL_LOCK_POLL_INTERVAL_MS = 3;
    constexpr inline static unsigned int PLL_LOCK_TIMEOUT_MS = 100;

    // Stabilization
    constexpr inline static unsigned int PLL_STABILIZATION_DELAY_MS = 10;
    constexpr inline static unsigned int PLL_STABILIZATION_ITERATIONS = 3;

    // Fallback Frequency
    constexpr inline static unsigned long long DEFAULT_FALLBACK_FREQ_HZ = 100000000ULL;  // 100 MHz
}

// ============================================================================
// THREAD SYNCHRONIZATION CONSTANTS
// ============================================================================

/**
 * @brief Thread synchronization constants
 * @note All values are constexpr and stored in Flash
 */
namespace ThreadSync {
    // Timeouts (ms)
    constexpr inline static unsigned int TIMEOUT_IMMEDIATE = 0;
    constexpr inline static unsigned int TIMEOUT_SHORT_MS = 10;
    constexpr inline static unsigned int TIMEOUT_MEDIUM_MS = 100;
    constexpr inline static unsigned int TIMEOUT_LONG_MS = 1000;
    constexpr inline static unsigned int TIMEOUT_INFINITE = 0xFFFFFFFF;

    // Stack Sizes (bytes)
    // DIAMOND OPTIMIZATION: Reduced coordinator thread stack from 4KB to 3.5KB
    // based on Stage 3 Red Team Attack analysis (22% safety margin)
    constexpr inline static unsigned int STACK_SIZE_SMALL = 2048;
    constexpr inline static unsigned int STACK_SIZE_MEDIUM = 3584;  // 3.5KB (revised from 4KB)
    constexpr inline static unsigned int STACK_SIZE_LARGE = 4096;
    constexpr inline static unsigned int STACK_SIZE_COORDINATOR = 3584;  // 3.5KB (revised from 4KB)

    // Thread Priorities (ChibiOS)
    constexpr inline static unsigned int PRIORITY_LOW = 10;
    constexpr inline static unsigned int PRIORITY_NORMAL = 15;
    constexpr inline static unsigned int PRIORITY_HIGH = 20;
}

// ============================================================================
// MEMORY MANAGEMENT CONSTANTS
// ============================================================================

/**
 * @brief Memory management constants
 * @note All values are constexpr and stored in Flash
 */
namespace Memory {
    // Stack Safety
    constexpr inline static unsigned int STACK_CANARY_SIZE = 32;
    constexpr inline static unsigned int STACK_SAFETY_MARGIN_BYTES = 512;

    // Buffer Sizes
    constexpr inline static unsigned int TEMP_BUFFER_SIZE = 256;
    constexpr inline static unsigned int WORK_BUFFER_SIZE = 512;

    // Alignment
    constexpr inline static unsigned int ALIGNMENT_4 = 4;
    constexpr inline static unsigned int ALIGNMENT_8 = 8;
    constexpr inline static unsigned int ALIGNMENT_16 = 16;
}

// ============================================================================
// DATA STRUCTURE CONSTANTS
// ============================================================================

/**
 * @brief Data structure constants
 * @note All values are constexpr and stored in Flash
 */
namespace Data {
    // Tracked Drone
    constexpr inline static unsigned int MAX_TRACKED_DRONES = 20;
    constexpr inline static unsigned int MAX_HISTORY_ENTRIES = 3;
    constexpr inline static unsigned int DRONE_STALE_TIMEOUT_MS = 10000;  // 10 seconds

    // Detection Ring Buffer
    constexpr inline static unsigned int DETECTION_RING_BUFFER_SIZE = 32;

    // Frequency Database
    constexpr inline static unsigned int MAX_DB_ENTRIES = 150;
    constexpr inline static unsigned int MAX_SCAN_BATCH_SIZE = 10;

    // Histogram
    constexpr inline static unsigned int HISTOGRAM_BINS = 64;
    constexpr inline static unsigned int HISTOGRAM_BIN_SIZE = 4;
}

// ============================================================================
// SCANNING CONSTANTS
// ============================================================================

/**
 * @brief Scanning constants
 * @note All values are constexpr and stored in Flash
 */
namespace Scanning {
    // Scan Intervals (ms)
    constexpr inline static unsigned int FAST_SCAN_INTERVAL_MS = 50;
    constexpr inline static unsigned int NORMAL_SCAN_INTERVAL_MS = 200;
    constexpr inline static unsigned int SLOW_SCAN_INTERVAL_MS = 500;
    constexpr inline static unsigned int VERY_SLOW_SCAN_INTERVAL_MS = 2000;

    // Adaptive Scanning
    constexpr inline static unsigned int HIGH_DENSITY_DETECTION_THRESHOLD = 50;
    constexpr inline static unsigned int HIGH_DENSITY_SCAN_CAP_MS = 100;
    constexpr inline static unsigned int PROGRESSIVE_SLOWDOWN_DIVISOR = 100;

    // Wideband Scanning
    constexpr inline static unsigned int WIDEBAND_SLICE_COUNT_MIN = 1;
    constexpr inline static unsigned int WIDEBAND_SLICE_COUNT_MAX = 10;
    constexpr inline static unsigned long long WIDEBAND_DEFAULT_MIN_HZ = 100000000ULL;   // 100 MHz
    constexpr inline static unsigned long long WIDEBAND_DEFAULT_MAX_HZ = 6000000000ULL;  // 6 GHz

    // RSSI Timeout
    constexpr inline static unsigned int RSSI_TIMEOUT_MS = 100;
    constexpr inline static unsigned int RSSI_POLL_INTERVAL_MS = 10;
}

// ============================================================================
// ERROR HANDLING CONSTANTS
// ============================================================================

/**
 * @brief Error handling constants
 * @note All values are constexpr and stored in Flash
 */
namespace Error {
    // Error Reporting
    constexpr inline static unsigned int ERROR_REPORTING_INTERVAL_CYCLES = 100;

    // Error Recovery
    constexpr inline static unsigned int ERROR_RECOVERY_DELAY_MS = 1000;
    constexpr inline static unsigned int ERROR_RECOVERY_RETRY_COUNT = 3;
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Clamp value to range
 * @param value Value to clamp
 * @param min Minimum value
 * @param max Maximum value
 * @return Clamped value
 */
constexpr inline int clamp(int value, int min, int max) noexcept {
    return (value < min) ? min : ((value > max) ? max : value);
}

/**
 * @brief Clamp value to range (unsigned int)
 * @param value Value to clamp
 * @param min Minimum value
 * @param max Maximum value
 * @return Clamped value
 */
constexpr inline unsigned int clamp_u32(unsigned int value, unsigned int min, unsigned int max) noexcept {
    return (value < min) ? min : ((value > max) ? max : value);
}

/**
 * @brief Check if value is in range
 * @param value Value to check
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return true if value is in range
 */
constexpr inline bool in_range(int value, int min, int max) noexcept {
    return (value >= min) && (value <= max);
}

/**
 * @brief Map value from one range to another
 * @param value Input value
 * @param in_min Input range minimum
 * @param in_max Input range maximum
 * @param out_min Output range minimum
 * @param out_max Output range maximum
 * @return Mapped value
 */
constexpr inline int map_range(int value, int in_min, int in_max,
                          int out_min, int out_max) noexcept {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

} // namespace ui::apps::enhanced_drone_analyzer::runtime

#endif // EDA_RUNTIME_CONSTANTS_HPP_
