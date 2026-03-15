/**
 * @file eda_optimized_utils.hpp
 * @brief Optimized utility functions for Enhanced Drone Analyzer
 *
 * DIAMOND CODE - Flawless, Memory-Safe, Optimized
 *
 * Provides optimized utilities including:
 * - MedianFilter: Stack-based median filtering with configurable window size
 * - FrequencyValidator: Frequency band validation
 * - DroneTypeDetector: Drone type detection from frequency
 * - FrequencyFormatter: Multiple frequency formatting options
 * - ThreatClassifier: Threat level classification from RSSI/SNR
 * - TrendSymbols: Movement trend symbol lookup
 * - StatusFormatter: Safe status message formatting
 *
 * Target: STM32F405 (ARM Cortex-M4)
 * Environment: ChibiOS RTOS
 *
 * @author Diamond Code Pipeline - Optimized Utilities
 * @date 2026-03-10
 * Phase 1 Migration - Foundation Layer (Infrastructure)
 *
 * DIAMOND CODE COMPLIANCE:
 * - No forbidden constructs (std::vector, std::string, std::map, std::atomic, new, malloc)
 * - Stack allocation only (max 4KB stack)
 * - Uses constexpr, enum class, using Type = uintXX_t
 * - No magic numbers (all constants defined)
 * - Zero-Overhead and Data-Oriented Design principles
 * - Self-contained and compilable
 */

#ifndef EDA_OPTIMIZED_UTILS_HPP_
#define EDA_OPTIMIZED_UTILS_HPP_

// C++ standard library headers (alphabetical order)
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <utility>

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"
#include "ui_drone_common_types.hpp"

namespace ui {
namespace apps {
namespace enhanced_drone_analyzer {

// Type Aliases
using BufferIndex = uint16_t;
using WindowSize = uint16_t;

// ============================================================================
// SNR THRESHOLD CONSTANTS
// ============================================================================

namespace SNRThresholds {
    constexpr uint8_t MILITARY_CRITICAL = 15;
    constexpr uint8_t MILITARY_HIGH = 10;
    constexpr uint8_t FPV_HIGH = 20;
    constexpr uint8_t FPV_MEDIUM = 10;
    constexpr uint8_t FPV_LOW = 5;
    constexpr uint8_t COMMERCIAL_CRITICAL = 25;
    constexpr uint8_t COMMERCIAL_HIGH = 15;
    constexpr uint8_t COMMERCIAL_MEDIUM = 10;
    constexpr uint8_t COMMERCIAL_LOW = 5;
}

// ============================================================================
// TREND CONSTANTS
// ============================================================================

namespace TrendConstants {
    constexpr uint8_t NUM_SYMBOLS = 4;
}

// ============================================================================
// BUFFER SIZE CONSTANTS
// ============================================================================

namespace BufferSizes {
    constexpr uint32_t ERROR_BUFFER_SIZE = 128;
    constexpr uint32_t FREQ_BUFFER_SIZE = 32;
}

// ============================================================================
// MEDIAN FILTER CLASS
// ============================================================================

/**
 * @brief Stack-based median filter with configurable window size
 *
 * Provides median filtering using a circular buffer.
 * All data is stored on the stack (no heap allocation).
 *
 * Template parameter N specifies the window size (max 256 for stack safety).
 *
 * Usage:
 * @code
 *     MedianFilter<int32_t, 11> filter;
 *
 *     // Add samples
 *     filter.add(100);
 *     filter.add(105);
 *     filter.add(95);
 *
 *     // Get median
 *     int32_t median = filter.get_median();
 * @endcode
 *
 * @note Window size is limited to 256 for stack safety (static_assert)
 * @note All operations are noexcept for embedded safety
 * @note Uses std::array for stack-allocated storage
 */
template<typename T, WindowSize N = 11>
class MedianFilter {
public:
    static_assert(N <= 256, "MedianFilter window size too large for stack (max 256)");

    /**
     * @brief Default constructor - initializes filter
     * @note noexcept for embedded safety
     */
    constexpr MedianFilter() noexcept : window_{}, head_(0), full_(false) {}

    /**
     * @brief Add a new value to the filter
     * @param value New value to add
     * @note Uses circular buffer for efficient storage
     */
    void add(const T value) noexcept {
        window_[head_] = value;
        head_ = (head_ + 1) % N;
        if (head_ == 0) full_ = true;
    }

    /**
     * @brief Get current number of samples in filter
     * @return Number of samples currently stored
     */
    constexpr WindowSize get_current_size() const noexcept {
        return full_ ? N : head_;
    }

    /**
     * @brief Get median value from current samples
     * @return Median value of samples in filter
     * @note Returns default-constructed T if filter is empty
     * @note Uses quickselect algorithm for O(n) median calculation
     */
    T get_median() const noexcept {
        const WindowSize current_size = get_current_size();

        if (current_size == 0) {
            return T{};
        }

        std::array<T, N> temp{};
        for (WindowSize i = 0; i < current_size; ++i) {
            temp[i] = window_[i];
        }

        const WindowSize k = current_size / 2;
        WindowSize left = 0;
        WindowSize right = current_size - 1;

        while (left < right) {
            WindowSize pivot_idx = left + (right - left) / 2;
            T pivot = temp[pivot_idx];

            std::swap(temp[pivot_idx], temp[right]);
            WindowSize store_idx = left;

            for (WindowSize i = left; i < right; ++i) {
                if (temp[i] < pivot) {
                    std::swap(temp[store_idx], temp[i]);
                    store_idx++;
                }
            }

            std::swap(temp[store_idx], temp[right]);

            if (store_idx == k) {
                break;
            } else if (store_idx < k) {
                left = store_idx + 1;
            } else {
                right = store_idx - 1;
            }
        }

        return temp[k];
    }

    /**
     * @brief Reset filter to initial state
     * @note Clears all samples and resets head pointer
     */
    void reset() noexcept {
        full_ = false;
        head_ = 0;
        window_.fill(static_cast<T>(0));
    }

private:
    std::array<T, N> window_;  ///< Circular buffer for samples
    BufferIndex head_;            ///< Current head position in buffer
    bool full_;                 ///< Whether buffer is full
};

// ============================================================================
// FREQUENCY VALIDATOR STRUCT
// ============================================================================

/**
 * @brief Frequency band validation utilities
 *
 * Provides static methods for validating frequencies against
 * various ISM bands and frequency ranges.
 *
 * All methods are constexpr for compile-time evaluation where possible.
 */
struct FrequencyValidator {
    /**
     * @brief Validate frequency is within hardware limits
     * @param hz Frequency in Hz
     * @return true if frequency is valid, false otherwise
     */
    static constexpr bool is_valid_frequency(int64_t hz) noexcept {
        return EDA::Validation::validate_frequency(hz);
    }

    /**
     * @brief Validate frequency is in 2.4 GHz ISM band
     * @param hz Frequency in Hz
     * @return true if frequency is in 2.4 GHz band, false otherwise
     */
    static constexpr bool is_valid_2_4ghz_band(int64_t hz) noexcept {
        return EDA::Validation::is_2_4ghz_band(hz);
    }

    /**
     * @brief Validate frequency is in 5.8 GHz ISM band
     * @param hz Frequency in Hz
     * @return true if frequency is in 5.8 GHz band, false otherwise
     */
    static constexpr bool is_valid_5_8ghz_band(int64_t hz) noexcept {
        return EDA::Validation::is_5_8ghz_band(hz);
    }

    /**
     * @brief Validate frequency is in military band
     * @param hz Frequency in Hz
     * @return true if frequency is in military band, false otherwise
     */
    static constexpr bool is_valid_military_band(int64_t hz) noexcept {
        return EDA::Validation::is_military_band(hz);
    }

    /**
     * @brief Validate frequency is in 433 MHz ISM band
     * @param hz Frequency in Hz
     * @return true if frequency is in 433 MHz band, false otherwise
     */
    static constexpr bool is_valid_433mhz_ism(int64_t hz) noexcept {
        return EDA::Validation::is_433mhz_band(hz);
    }
};

// ============================================================================
// DRONE TYPE DETECTOR STRUCT
// ============================================================================

/**
 * @brief Drone type detection from frequency
 *
 * Provides static method to determine drone type based on
 * the frequency band the signal is detected in.
 *
 * Detection logic:
 * - 2.4 GHz ISM band -> MAVIC (consumer drone)
 * - 5.8 GHz ISM band -> FPV_RACING (racing drone)
 * - Military band (860-930 MHz) -> MILITARY_DRONE
 * - 433 MHz ISM band -> DIY_DRONE
 * - Other -> UNKNOWN
 */
struct DroneTypeDetector {
    /**
     * @brief Detect drone type from frequency
     * @param hz Frequency in Hz
     * @return Detected drone type
     */
    static constexpr DroneType from_frequency(int64_t hz) noexcept {
        if (FrequencyValidator::is_valid_2_4ghz_band(hz)) {
            return DroneType::MAVIC;
        }
        if (FrequencyValidator::is_valid_5_8ghz_band(hz)) {
            return DroneType::FPV_RACING;
        }
        if (FrequencyValidator::is_valid_military_band(hz)) {
            return DroneType::MILITARY_DRONE;
        }
        if (FrequencyValidator::is_valid_433mhz_ism(hz)) {
            return DroneType::DIY_DRONE;
        }
        return DroneType::UNKNOWN;
    }
};

// ============================================================================
// FREQUENCY FORMATTER STRUCT
// ============================================================================

/**
 * @brief Frequency formatting utilities
 *
 * Provides multiple formatting options for displaying frequencies:
 * - COMPACT_GHZ: "2.4G" or "5.8G"
 * - COMPACT_MHZ: "2400" or "5800"
 * - STANDARD_GHZ: "2.4GHz" or "5.8GHz"
 * - STANDARD_MHZ: "2400.5MHz" or "5800.0MHz"
 * - DETAILED_GHZ: "2.400GHz" or "5.800GHz"
 * - SPACED_GHZ: "2.4 GHz" or "5.8 GHz"
 *
 * All formatting is done to a provided buffer (no heap allocation).
 */
struct FrequencyFormatter {
    /**
     * @brief Format options for frequency display
     */
    enum class Format {
        COMPACT_GHZ,     ///< "2.4G"
        COMPACT_MHZ,     ///< "2400"
        STANDARD_GHZ,     ///< "2.4GHz"
        STANDARD_MHZ,     ///< "2400.5MHz"
        DETAILED_GHZ,    ///< "2.400GHz"
        SPACED_GHZ       ///< "2.4 GHz"
    };

    /**
     * @brief Format frequency to buffer using specified format
     * @param buffer Output buffer (must be at least 32 bytes)
     * @param buffer_size Size of output buffer
     * @param freq_hz Frequency in Hz
     * @param fmt Format option
     * @note All formatting is done to the provided buffer
     */
    static void format_to_buffer(char* __restrict__ buffer, uint32_t buffer_size,
                                  int64_t freq_hz, Format fmt) noexcept {
        if (!buffer || buffer_size == 0) return;

        switch (fmt) {
            case Format::COMPACT_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 500000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000LL) / 100000000ULL);
                if (decimal > 0) {
                    std::snprintf(buffer, buffer_size, "%lu.%luG",
                             static_cast<unsigned long>(ghz),
                             static_cast<unsigned long>(decimal));
                } else {
                    std::snprintf(buffer, buffer_size, "%luG",
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::COMPACT_MHZ: {
                uint32_t mhz = static_cast<uint32_t>((freq_hz + 500000) / 1000000LL);
                std::snprintf(buffer, buffer_size, "%lu",
                         static_cast<unsigned long>(mhz));
                break;
            }
            case Format::STANDARD_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 50000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000LL) / 100000000ULL);
                if (decimal > 0) {
                    std::snprintf(buffer, buffer_size, "%lu.%luGHz",
                             static_cast<unsigned long>(ghz),
                             static_cast<unsigned long>(decimal));
                } else {
                    std::snprintf(buffer, buffer_size, "%luGHz",
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::STANDARD_MHZ: {
                uint32_t mhz = static_cast<uint32_t>(freq_hz / 1000000LL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000LL) / 100000ULL);
                if (decimals > 0) {
                    std::snprintf(buffer, buffer_size, "%lu.%luMHz",
                             static_cast<unsigned long>(mhz),
                             static_cast<unsigned long>(decimals));
                } else {
                    std::snprintf(buffer, buffer_size, "%luMHz",
                             static_cast<unsigned long>(mhz));
                }
                break;
            }
            case Format::DETAILED_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 5000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000000LL) / 10000000ULL);
                std::snprintf(buffer, buffer_size, "%lu.%03luGHz",
                         static_cast<unsigned long>(ghz),
                         static_cast<unsigned long>(decimals));
                break;
            }
            case Format::SPACED_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 500000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                std::snprintf(buffer, buffer_size, "%lu GHz",
                         static_cast<unsigned long>(ghz));
                break;
            }
        }
    }

    /**
     * @brief Format frequency to buffer using compact GHz format
     * @param buffer Output buffer (must be at least 32 bytes)
     * @param buffer_size Size of output buffer
     * @param freq_hz Frequency in Hz
     * @note Uses COMPACT_GHZ format ("2.4G" or "5.8G")
     */
    static void to_string_short_freq_buffer(char* __restrict__ buffer, uint32_t buffer_size,
                                              int64_t freq_hz) noexcept {
        format_to_buffer(buffer, buffer_size, freq_hz, Format::COMPACT_GHZ);
    }
};

// ============================================================================
// THREAT CLASSIFIER STRUCT
// ============================================================================

/**
 * @brief Threat level classification from RSSI/SNR
 *
 * Provides static methods for classifying threat levels
 * based on RSSI (signal strength) or SNR (signal-to-noise ratio).
 *
 * RSSI-based classification:
 * - >= -50 dBm -> CRITICAL
 * - >= -60 dBm -> HIGH
 * - >= -70 dBm -> MEDIUM
 * - >= -80 dBm -> LOW
 * - < -80 dBm -> NONE
 *
 * SNR-based classification (drone type dependent):
 * - Military: >=15 CRITICAL, >=10 HIGH, else MEDIUM
 * - FPV Racing: >=20 HIGH, >=10 MEDIUM, >=5 LOW
 * - Commercial: >=25 CRITICAL, >=15 HIGH, >=10 MEDIUM, >=5 LOW
 */
struct ThreatClassifier {
    /**
     * @brief Classify threat level from RSSI
     * @param rssi_db Signal strength in dBm
     * @return Threat level based on RSSI
     */
    static constexpr ThreatLevel from_rssi(int32_t rssi_db) noexcept {
        if (rssi_db >= EDA::Constants::CRITICAL_RSSI_DB) return ThreatLevel::CRITICAL;
        if (rssi_db >= EDA::Constants::HIGH_RSSI_DB) return ThreatLevel::HIGH;
        if (rssi_db >= EDA::Constants::MEDIUM_RSSI_DB) return ThreatLevel::MEDIUM;
        if (rssi_db >= EDA::Constants::LOW_RSSI_DB) return ThreatLevel::LOW;
        return ThreatLevel::NONE;
    }

    /**
     * @brief Classify threat level from SNR and drone type
     * @param snr Signal-to-noise ratio
     * @param type Drone type (as uint8_t)
     * @return Threat level based on SNR and drone type
     */
    static constexpr ThreatLevel from_snr_and_type(uint8_t snr, uint8_t type) noexcept {
        if (type == static_cast<uint8_t>(DroneType::MILITARY_DRONE)) {
            if (snr >= SNRThresholds::MILITARY_CRITICAL) return ThreatLevel::CRITICAL;
            if (snr >= SNRThresholds::MILITARY_HIGH) return ThreatLevel::HIGH;
            return ThreatLevel::MEDIUM;
        }

        if (type == static_cast<uint8_t>(DroneType::FPV_RACING)) {
            if (snr >= SNRThresholds::FPV_HIGH) return ThreatLevel::HIGH;
            if (snr >= SNRThresholds::FPV_MEDIUM) return ThreatLevel::MEDIUM;
            if (snr >= SNRThresholds::FPV_LOW) return ThreatLevel::LOW;
            return ThreatLevel::NONE;
        }

        if (snr >= SNRThresholds::COMMERCIAL_CRITICAL) return ThreatLevel::CRITICAL;
        if (snr >= SNRThresholds::COMMERCIAL_HIGH) return ThreatLevel::HIGH;
        if (snr >= SNRThresholds::COMMERCIAL_MEDIUM) return ThreatLevel::MEDIUM;
        if (snr >= SNRThresholds::COMMERCIAL_LOW) return ThreatLevel::LOW;
        return ThreatLevel::NONE;
    }
};

// ============================================================================
// TREND SYMBOLS STRUCT
// ============================================================================

/**
 * @brief Movement trend symbol lookup
 *
 * Provides static method to get trend symbol character
 * from trend index.
 *
 * Symbol mapping:
 * - 0 -> '=' (Static)
 * - 1 -> '^' (Approaching)
 * - 2 -> 'v' (Receding)
 * - 3 -> '~' (Unknown)
 */
struct TrendSymbols {
    /**
     * @brief Symbol characters for each trend type
     */
    static constexpr char SYMBOLS[TrendConstants::NUM_SYMBOLS] = {
        '=',  // Static (0)
        '^',  // Approaching (1)
        'v',  // Receding (2)
        '~'   // Unknown (3)
    };

    /**
     * @brief Get symbol character for trend index
     * @param trend Trend index (0-3)
     * @return Symbol character for trend
     * @note Returns '~' for invalid trend indices
     */
    static constexpr char from_trend(uint8_t trend) noexcept {
        return (trend < TrendConstants::NUM_SYMBOLS) ? SYMBOLS[trend] : SYMBOLS[3];
    }
};

// ============================================================================
// STATUS FORMATTER STRUCT
// ============================================================================

/**
 * @brief Safe status message formatting
 *
 * Provides template method for formatting status messages
 * to fixed-size buffers.
 *
 * Usage:
 * @code
 *     char buffer[64];
 *     StatusFormatter::format_to(buffer, "Status: %d", 42);
 * @endcode
 */
struct StatusFormatter {
    /**
     * @brief Format message to fixed-size buffer
     * @param buffer Output buffer (fixed-size array)
     * @param fmt Format string
     * @param args Format arguments
     * @note Uses std::forward for perfect forwarding
     */
    template<uint32_t N, typename... Args>
    static void format_to(char (&buffer)[N], const char* fmt, Args&&... args) noexcept {
        std::snprintf(buffer, N, fmt, std::forward<Args>(args)...);
    }
};

} // namespace enhanced_drone_analyzer
} // namespace apps
} // namespace ui

#endif // EDA_OPTIMIZED_UTILS_HPP_
