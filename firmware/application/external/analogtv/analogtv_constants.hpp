/*
 * Copyright (C) 2024 Mayhem Firmware Contributors
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ANALOGTV_CONSTANTS_HPP
#define ANALOGTV_CONSTANTS_HPP

#include <cstdint>
#include <cstddef>

namespace ui::external_app::analogtv {

// ============================================================================
// Display Constants (Fullscreen)
// ============================================================================

/**
 * @brief Fullscreen display width in pixels
 */
constexpr uint16_t DISPLAY_WIDTH = 240;

/**
 * @brief Fullscreen display height in pixels
 */
constexpr uint16_t DISPLAY_HEIGHT = 320;

/**
 * @brief Video display width (internal processing)
 */
constexpr uint16_t VIDEO_WIDTH = 128;

/**
 * @brief Video display height (internal processing)
 */
constexpr uint16_t VIDEO_HEIGHT = 208;

/**
 * @brief Video buffer size (must accommodate VIDEO_WIDTH * VIDEO_HEIGHT)
 */
constexpr size_t VIDEO_BUFFER_SIZE = 13312;  // 128 * 104 = 13312

/**
 * @brief Spectrum bins per callback (256 = 2 lines of 128 pixels)
 */
constexpr size_t SPECTRUM_BINS = 256;

/**
 * @brief Line buffer size for rendering
 */
constexpr size_t LINE_BUFFER_SIZE = 128;

// ============================================================================
// Frequency Scanning Constants
// ============================================================================

/**
 * @brief Minimum scan frequency in Hz (50 MHz - VHF low band)
 */
constexpr uint64_t SCAN_START_HZ = 50'000'000ULL;

/**
 * @brief Maximum scan frequency in Hz (7200 MHz - HackRF One maximum)
 */
constexpr uint64_t SCAN_END_HZ = 7'200'000'000ULL;

/**
 * @brief Frequency step size in Hz (1 MHz)
 */
constexpr uint64_t SCAN_STEP_HZ = 1'000'000ULL;

/**
 * @brief Dwell time per frequency in milliseconds
 */
constexpr uint32_t DWELL_TIME_MS = 100;

/**
 * @brief Video carrier detection threshold in dBm
 * @note Analog TV video carriers are typically -80 to -60 dBm
 */
constexpr int32_t CARRIER_THRESHOLD_DBM = -80;

/**
 * @brief Minimum carrier width in bins for valid detection
 */
constexpr uint8_t MIN_CARRIER_WIDTH_BINS = 3;

/**
 * @brief Maximum carrier width in bins for valid detection
 */
constexpr uint8_t MAX_CARRIER_WIDTH_BINS = 10;

// ============================================================================
// Hardware Constants
// ============================================================================

/**
 * @brief Default sample rate in Hz (2 MHz)
 */
constexpr uint32_t DEFAULT_SAMPLE_RATE_HZ = 2'000'000;

/**
 * @brief Default baseband bandwidth in Hz (2 MHz)
 */
constexpr uint32_t DEFAULT_BASEBAND_BANDWIDTH_HZ = 2'000'000;

/**
 * @brief Maximum hardware retry attempts
 */
constexpr uint8_t MAX_HARDWARE_RETRIES = 3;

/**
 * @brief Hardware retry delay in milliseconds
 */
constexpr uint32_t HARDWARE_RETRY_DELAY_MS = 10;

/**
 * @brief PLL lock timeout in milliseconds
 */
constexpr uint32_t PLL_LOCK_TIMEOUT_MS = 100;

/**
 * @brief PLL lock poll interval in milliseconds
 */
constexpr uint32_t PLL_LOCK_POLL_INTERVAL_MS = 3;

// ============================================================================
// UI Constants
// ============================================================================

/**
 * @brief UI refresh interval in milliseconds (60 FPS target)
 */
constexpr uint32_t UI_REFRESH_INTERVAL_MS = 16;

/**
 * @brief Scan status display duration in milliseconds
 */
constexpr uint32_t SCAN_STATUS_DURATION_MS = 2000;

/**
 * @brief Frequency display update interval in milliseconds
 */
constexpr uint32_t FREQ_DISPLAY_UPDATE_MS = 500;

/**
 * @brief Maximum text length for display
 */
constexpr size_t MAX_TEXT_LENGTH = 32;

// ============================================================================
// Spectrum Processing Constants
// ============================================================================

/**
 * @brief Spectrum color lookup table size
 */
constexpr size_t SPECTRUM_COLOR_LUT_SIZE = 256;

/**
 * @brief Default X correction value (horizontal alignment)
 */
constexpr uint8_t DEFAULT_X_CORRECTION = 10;

/**
 * @brief Minimum spectrum value for processing
 */
constexpr uint8_t SPECTRUM_MIN_VALUE = 0;

/**
 * @brief Maximum spectrum value for processing
 */
constexpr uint8_t SPECTRUM_MAX_VALUE = 255;

// ============================================================================
// State Machine Constants
// ============================================================================

/**
 * @brief Scan state enumeration
 */
enum class ScanState : uint8_t {
    IDLE = 0,      // Not scanning, manual mode
    SCANNING,      // Auto-scanning frequencies
    CARRIER_FOUND, // Video carrier detected
    LOCKED,        // Locked onto carrier
    ERROR          // Error state
};

/**
 * @brief Scan mode enumeration
 */
enum class ScanMode : uint8_t {
    MANUAL = 0,    // Manual frequency tuning
    AUTO_SCAN,     // Automatic frequency scanning
    PRESET         // Preset frequency selection
};

// ============================================================================
// Error Codes
// ============================================================================

/**
 * @brief Error code enumeration
 */
enum class ErrorCode : uint8_t {
    SUCCESS = 0,
    HARDWARE_INIT_FAILED,
    TUNING_FAILED,
    NO_CARRIER_FOUND,
    BUFFER_OVERFLOW,
    TIMEOUT,
    UNKNOWN_ERROR
};

// ============================================================================
// Common Analog TV Frequencies (MHz)
// ============================================================================

/**
 * @brief Common VHF TV channels (North America)
 */
constexpr uint64_t VHF_CHANNEL_2_HZ = 55'250'000ULL;    // 55.25 MHz
constexpr uint64_t VHF_CHANNEL_3_HZ = 61'250'000ULL;    // 61.25 MHz
constexpr uint64_t VHF_CHANNEL_4_HZ = 67'250'000ULL;    // 67.25 MHz
constexpr uint64_t VHF_CHANNEL_5_HZ = 77'250'000ULL;    // 77.25 MHz
constexpr uint64_t VHF_CHANNEL_6_HZ = 83'250'000ULL;    // 83.25 MHz

/**
 * @brief Common UHF TV channels (North America)
 */
constexpr uint64_t UHF_CHANNEL_14_HZ = 471'250'000ULL;  // 471.25 MHz
constexpr uint64_t UHF_CHANNEL_15_HZ = 477'250'000ULL;  // 477.25 MHz
constexpr uint64_t UHF_CHANNEL_16_HZ = 483'250'000ULL;  // 483.25 MHz
constexpr uint64_t UHF_CHANNEL_83_HZ = 885'250'000ULL;  // 885.25 MHz

/**
 * @brief Common PAL TV frequencies (Europe)
 */
constexpr uint64_t PAL_CHANNEL_S1_HZ = 110'000'000ULL;  // 110 MHz
constexpr uint64_t PAL_CHANNEL_S2_HZ = 118'000'000ULL;  // 118 MHz
constexpr uint64_t PAL_CHANNEL_S3_HZ = 126'000'000ULL;  // 126 MHz

}  // namespace ui::external_app::analogtv

#endif /* ANALOGTV_CONSTANTS_HPP */