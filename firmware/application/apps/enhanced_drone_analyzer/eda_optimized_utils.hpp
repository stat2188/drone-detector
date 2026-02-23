// * * @file eda_optimized_utils.hpp * @brief Diamond-Optimized Utilities for Enhanced Drone Analyzer

#ifndef EDA_OPTIMIZED_UTILS_HPP_
#define EDA_OPTIMIZED_UTILS_HPP_

#include <array>
#include <algorithm>
#include <cstdint>
#include <inttypes.h>
#include <cstdio>
#include <cstring>
#include "eda_constants.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Type Aliases (Semantic Types)
using BufferIndex = uint16_t;

// * * @brief Size type for window operations * @note Using uint16_t for memory efficiency (supports up to 65535 elements)
using WindowSize = uint16_t;

// Named Constants (No Magic Numbers)
namespace RSSIThresholds {
    constexpr int32_t CRITICAL = -50;
    constexpr int32_t HIGH = -60;
    constexpr int32_t MEDIUM = -70;
    constexpr int32_t LOW = -80;
}
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
namespace TrendConstants {
    constexpr uint8_t NUM_SYMBOLS = 4;
}
namespace BufferSizes {
    constexpr size_t ERROR_BUFFER_SIZE = 128;
    constexpr size_t FREQ_BUFFER_SIZE = 32;
}
template<typename T, WindowSize N = 11>
class MedianFilter {
public:
    // / @brief Constructor - initializes empty filter
    constexpr MedianFilter() noexcept : window_{}, head_(0), full_(false) {
    }

    // * * @brief Add a new value to the filter window * @param value Value to add
    void add(const T value) noexcept {
        window_[head_] = value;
        head_ = (head_ + 1) % N;
        if (head_ == 0) full_ = true;
    }

    // * * @brief Get current number of values in the window * @return Current window size (0 to N)
    constexpr WindowSize get_current_size() const noexcept {
        return full_ ? N : head_;
    }

    // * * @brief Get the median value from the current window * @return Median value, or T{} if window is empty * * @note Uses QuickSelect algorithm for O(n) average time complexity * @note Creates a temporary copy of the active window for sorting
    T get_median() const noexcept {
        const WindowSize current_size = get_current_size();

        if (current_size == 0) {
            return T{};
        }

        // Create temporary copy for sorting
        std::array<T, N> temp{};
        for (WindowSize i = 0; i < current_size; ++i) {
            temp[i] = window_[i];
        }

        const WindowSize k = current_size / 2;

        // QuickSelect implementation for median
        WindowSize left = 0;
        WindowSize right = current_size - 1;

        while (left < right) {
            WindowSize pivot_idx = left + (right - left) / 2;
            T pivot = temp[pivot_idx];

            // Partition
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

    // * * @brief Reset the filter to empty state
    void reset() noexcept {
        full_ = false;
        head_ = 0;
        window_.fill(static_cast<T>(0));
    }

private:
    std::array<T, N> window_;  ///< Circular buffer window
    BufferIndex head_;         ///< Current write position
    bool full_;                ///< Whether window has wrapped around
};

// Moved to Unified Lookup Tables (color_lookup_unified.hpp)
template<typename T>
class CachedValue {
public:
    // / @brief Constructor with optional initial value
    explicit constexpr CachedValue(const T& initial_value = T{}) noexcept
        : value_(initial_value), dirty_(true) {}

    // / @brief Update value and track if changed
    bool update(const T& new_value) noexcept {
        if (value_ != new_value) {
            value_ = new_value;
            dirty_ = true;
            return true;
        }
        return false;
    }

    // / @brief Check if value has been modified since last clear
    constexpr bool is_dirty() const noexcept { return dirty_; }

    // / @brief Clear dirty flag
    void clear_dirty() noexcept { dirty_ = false; }

    // / @brief Get const reference to value
    constexpr const T& get() const noexcept { return value_; }

    // / @brief Get mutable reference to value
    T& get() noexcept { return value_; }

private:
    T value_;   ///< Stored value
    bool dirty_; ///< Dirty flag (true if modified since last clear)
};

// Constants for Frequency Validation
namespace FrequencyValidationConstants {
    constexpr int64_t MIN_HARDWARE_FREQ = 1'000'000LL;      ///< Minimum hardware frequency (1 MHz)
    constexpr int64_t MAX_HARDWARE_FREQ = 7'200'000'000LL; ///< Maximum hardware frequency (7.2 GHz)
    constexpr int64_t MIN_2_4GHZ = 2'400'000'000LL;        ///< Minimum 2.4 GHz band
    constexpr int64_t MAX_2_4GHZ = 2'483'500'000LL;        ///< Maximum 2.4 GHz band
    constexpr int64_t MIN_5_8GHZ = 5'725'000'000LL;        ///< Minimum 5.8 GHz band
    constexpr int64_t MAX_5_8GHZ = 5'875'000'000LL;        ///< Maximum 5.8 GHz band
    constexpr int64_t MIN_MILITARY = 860'000'000LL;        ///< Minimum military band
    constexpr int64_t MAX_MILITARY = 930'000'000LL;        ///< Maximum military band
    constexpr int64_t MIN_433MHZ = 433'000'000LL;          ///< Minimum 433 MHz ISM band
    constexpr int64_t MAX_433MHZ = 435'000'000LL;          ///< Maximum 433 MHz ISM band
}

// Constexpr Frequency Validation
struct FrequencyValidator {
    // / @brief Check if frequency is within hardware limits
    static constexpr bool is_valid_frequency(int64_t hz) noexcept {
        return EDA::Validation::validate_frequency(hz);
    }

    // / @brief Check if frequency is in 2.4 GHz band
    static constexpr bool is_valid_2_4ghz_band(int64_t hz) noexcept {
        return EDA::Validation::is_2_4ghz_band(hz);
    }

    // / @brief Check if frequency is in 5.8 GHz band
    static constexpr bool is_valid_5_8ghz_band(int64_t hz) noexcept {
        return EDA::Validation::is_5_8ghz_band(hz);
    }

    // / @brief Check if frequency is in military band
    static constexpr bool is_valid_military_band(int64_t hz) noexcept {
        return EDA::Validation::is_military_band(hz);
    }

    // / @brief Check if frequency is in 433 MHz ISM band
    static constexpr bool is_valid_433mhz_ism(int64_t hz) noexcept {
        return EDA::Validation::is_433mhz_band(hz);
    }
};

// Drone Type Detection
struct DroneTypeDetector {
    // * * @brief Detect drone type from operating frequency * @note 2.4 GHz band  MAVIC, 5.8 GHz band  FPV_RACING, Military band  MILITARY_DRONE, 433 MHz ISM band  DIY_DRONE
    static constexpr DroneType from_frequency(int64_t hz) noexcept {
        // 2.4 GHz band
        if (FrequencyValidator::is_valid_2_4ghz_band(hz)) {
            return DroneType::MAVIC;
        }
        // 5.8 GHz band
        if (FrequencyValidator::is_valid_5_8ghz_band(hz)) {
            return DroneType::FPV_RACING;
        }
        // Military band
        if (FrequencyValidator::is_valid_military_band(hz)) {
            return DroneType::MILITARY_DRONE;
        }
        // 433 MHz ISM band
        if (FrequencyValidator::is_valid_433mhz_ism(hz)) {
            return DroneType::DIY_DRONE;
        }
        return DroneType::UNKNOWN;
    }
};

// Frequency Formatter
struct FrequencyFormatter {
    // / @brief Format type enumeration
    enum class Format {
        COMPACT_GHZ,      ///< "2.4G" - for tight spaces
        COMPACT_MHZ,      ///< "2400" - for MHz values
        STANDARD_GHZ,      ///< "2.450GHz" - for detailed display
        STANDARD_MHZ,      ///< "2400.0MHz" - for MHz with decimal
        DETAILED_GHZ,      ///< "2.450GHz" - 3 decimal places
        SPACED_GHZ         ///< "2.4 GHz" - for labels with space
    };

    // * * @brief Format frequency to buffer (non-allocating) * @note Writes directly to user-provided buffer (no malloc/free)
    static void format_to_buffer(char* __restrict__ buffer, size_t buffer_size, 
                                  int64_t freq_hz, Format fmt) noexcept {
        // Guard clause: invalid buffer
        if (!buffer || buffer_size == 0) return;

        switch (fmt) {
            case Format::COMPACT_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 500000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000LL) / 100000000ULL);
                if (decimal > 0) {
                    snprintf(buffer, buffer_size, "%lu.%luG",
                             static_cast<unsigned long>(ghz),
                             static_cast<unsigned long>(decimal));
                } else {
                    snprintf(buffer, buffer_size, "%luG",
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::COMPACT_MHZ: {
                uint32_t mhz = static_cast<uint32_t>((freq_hz + 500000) / 1000000LL);
                snprintf(buffer, buffer_size, "%lu",
                         static_cast<unsigned long>(mhz));
                break;
            }
            case Format::STANDARD_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 50000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000LL) / 100000000ULL);
                if (decimal > 0) {
                    snprintf(buffer, buffer_size, "%lu.%luGHz",
                             static_cast<unsigned long>(ghz),
                             static_cast<unsigned long>(decimal));
                } else {
                    snprintf(buffer, buffer_size, "%luGHz",
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::STANDARD_MHZ: {
                uint32_t mhz = static_cast<uint32_t>(freq_hz / 1000000LL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000LL) / 100000ULL);
                if (decimals > 0) {
                    snprintf(buffer, buffer_size, "%lu.%luMHz",
                             static_cast<unsigned long>(mhz),
                             static_cast<unsigned long>(decimals));
                } else {
                    snprintf(buffer, buffer_size, "%luMHz",
                             static_cast<unsigned long>(mhz));
                }
                break;
            }
            case Format::DETAILED_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 5000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000000LL) / 10000000ULL);
                snprintf(buffer, buffer_size, "%lu.%03luGHz",
                         static_cast<unsigned long>(ghz),
                         static_cast<unsigned long>(decimals));
                break;
            }
            case Format::SPACED_GHZ: {
                uint64_t freq_rounded = static_cast<uint64_t>(freq_hz) + 500000000ULL;
                uint32_t ghz = static_cast<uint32_t>(freq_rounded / 1000000000ULL);
                snprintf(buffer, buffer_size, "%lu GHz",
                         static_cast<unsigned long>(ghz));
                break;
            }
        }
    }

    // / @brief Format frequency to short string (compact GHz format)
    static void to_string_short_freq_buffer(char* __restrict__ buffer, size_t buffer_size, 
                                              int64_t freq_hz) noexcept {
        format_to_buffer(buffer, buffer_size, freq_hz, Format::COMPACT_GHZ);
    }
};

// Threat Classification
struct ThreatClassifier {
    // * * @brief Classify threat level by RSSI * @note >= -50 dB  CRITICAL, >= -60 dB  HIGH, >= -70 dB  MEDIUM, >= -80 dB  LOW, < -80 dB  NONE
    static constexpr ThreatLevel from_rssi(int32_t rssi_db) noexcept {
        if (rssi_db >= RSSIThresholds::CRITICAL) return ThreatLevel::CRITICAL;
        if (rssi_db >= RSSIThresholds::HIGH) return ThreatLevel::HIGH;
        if (rssi_db >= RSSIThresholds::MEDIUM) return ThreatLevel::MEDIUM;
        if (rssi_db >= RSSIThresholds::LOW) return ThreatLevel::LOW;
        return ThreatLevel::NONE;
    }

    // * * @brief Classify threat level by SNR and drone type * @note Military: CRITICAL at SNR>=15, HIGH at SNR>=10, FPV: HIGH at SNR>=20, MEDIUM at SNR>=10, LOW at SNR>=5, Commercial: CRITICAL at SNR>=25, HIGH at SNR>=15
    static constexpr ThreatLevel from_snr_and_type(uint8_t snr, uint8_t type) noexcept {
        // Military drones (type=8)
        if (type == static_cast<uint8_t>(DroneType::MILITARY_DRONE)) {
            if (snr >= SNRThresholds::MILITARY_CRITICAL) return ThreatLevel::CRITICAL;
            if (snr >= SNRThresholds::MILITARY_HIGH) return ThreatLevel::HIGH;
            return ThreatLevel::MEDIUM;
        }

        // FPV racing (type=10)
        if (type == static_cast<uint8_t>(DroneType::FPV_RACING)) {
            if (snr >= SNRThresholds::FPV_HIGH) return ThreatLevel::HIGH;
            if (snr >= SNRThresholds::FPV_MEDIUM) return ThreatLevel::MEDIUM;
            if (snr >= SNRThresholds::FPV_LOW) return ThreatLevel::LOW;
            return ThreatLevel::NONE;
        }

        // Commercial drones (MAVIC=1, PHANTOM=3)
        if (snr >= SNRThresholds::COMMERCIAL_CRITICAL) return ThreatLevel::CRITICAL;
        if (snr >= SNRThresholds::COMMERCIAL_HIGH) return ThreatLevel::HIGH;
        if (snr >= SNRThresholds::COMMERCIAL_MEDIUM) return ThreatLevel::MEDIUM;
        if (snr >= SNRThresholds::COMMERCIAL_LOW) return ThreatLevel::LOW;
        return ThreatLevel::NONE;
    }
};

// Trend Symbol Lookup
struct TrendSymbols {
    // * * @brief Trend symbol lookup table * @note '=': STATIC (0), '^': APPROACHING (1), 'v': RECEDING (2), '~': UNKNOWN (3)
    static constexpr char SYMBOLS[TrendConstants::NUM_SYMBOLS] = {
        '=',  // STATIC (0)
        '^',  // APPROACHING (1)
        'v',  // RECEDING (2)
        '~'   // UNKNOWN (3)
    };

    // / @brief Get trend symbol from trend value
    static constexpr char from_trend(uint8_t trend) noexcept {
        return (trend < TrendConstants::NUM_SYMBOLS) ? SYMBOLS[trend] : SYMBOLS[3];
    }
};

// Safe Buffer Access Helper
template<typename T, size_t N>
struct SafeBufferAccess {
    // / @brief Get element from array with bounds checking
    static constexpr T& get(std::array<T, N>& arr, size_t idx, T& fallback) noexcept {
        return (idx < N) ? arr[idx] : fallback;
    }

    // / @brief Get const element from array with bounds checking
    static constexpr const T& get(const std::array<T, N>& arr, size_t idx, 
                                   const T& fallback) noexcept {
        return (idx < N) ? arr[idx] : fallback;
    }

    // / @brief Check if index is valid for array
    static constexpr bool is_valid(size_t idx) noexcept {
        return idx < N;
    }
};

// Status Formatting Helper
struct StatusFormatter {
    // / @brief Format arguments to buffer (non-allocating)
    template<size_t N, typename... Args>
    static void format_to(char (&buffer)[N], const char* fmt, Args&&... args) noexcept {
        snprintf(buffer, N, fmt, std::forward<Args>(args)...);
    }
};

// Validator Error Formatter
struct ValidatorFormatter {
    // / @brief Format out-of-range error for int64_t
    static void out_of_range(char* __restrict__ buffer, size_t buffer_size,
                              const char* name, int64_t value, 
                              int64_t min, int64_t max) noexcept {
        if (!buffer || buffer_size == 0) return;
        snprintf(buffer, buffer_size, "%s %lld invalid (must be %lld to %lld)",
                 name, static_cast<long long>(value),
                 static_cast<long long>(min), static_cast<long long>(max));
    }

    // / @brief Format out-of-range error for uint64_t
    static void out_of_range(char* __restrict__ buffer, size_t buffer_size,
                              const char* name, uint64_t value, 
                              uint64_t min, uint64_t max) noexcept {
        if (!buffer || buffer_size == 0) return;
        snprintf(buffer, buffer_size, "%s %llu invalid (must be %llu to %llu)",
                 name, static_cast<unsigned long long>(value),
                 static_cast<unsigned long long>(min), 
                 static_cast<unsigned long long>(max));
    }

    // / @brief Format out-of-range error for int32_t
    static void out_of_range(char* __restrict__ buffer, size_t buffer_size,
                              const char* name, int32_t value, 
                              int32_t min, int32_t max) noexcept {
        out_of_range(buffer, buffer_size, name, 
                     static_cast<int64_t>(value), 
                     static_cast<int64_t>(min), 
                     static_cast<int64_t>(max));
    }

    // / @brief Format out-of-range error for uint32_t
    static void out_of_range(char* __restrict__ buffer, size_t buffer_size,
                              const char* name, uint32_t value, 
                              uint32_t min, uint32_t max) noexcept {
        out_of_range(buffer, buffer_size, name, 
                     static_cast<uint64_t>(value), 
                     static_cast<uint64_t>(min), 
                     static_cast<uint64_t>(max));
    }
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_OPTIMIZED_UTILS_HPP_
