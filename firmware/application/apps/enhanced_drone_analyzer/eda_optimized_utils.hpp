/**
 * Diamond-Optimized Utilities for Enhanced Drone Analyzer
 * 
 * GOALS:
 * - Eliminate code duplication through templates and LUTs
 * - Reduce binary size by unifying isomorphic logic
 * - Maintain zero heap allocation (all stack-based)
 * - Scott Meyers Effective C++ best practices applied
 * 
 * CONSTRAINTS:
 * - Cortex-M4 (ARMv7E-M)
 * - No heap allocation
 * - Strict memory constraints
 * - C++17 constexpr support
 * 
 * USAGE:
 * Include this file AFTER your standard includes.
 * Requires: <array>, <algorithm>, <cstdint>
 *           - Frequency type (int64_t)
 *           - Color, DroneType, ThreatLevel enums
 *           - to_string_dec_int() function
 * 
 */

#ifndef EDA_OPTIMIZED_UTILS_HPP_
#define EDA_OPTIMIZED_UTILS_HPP_

#include <array>
#include <algorithm>
#include <cstdint>
#include <string>
#include <cstdio>
#include <cstring>

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// UNIFIED MEDIAN FILTER TEMPLATE
// ===========================================
// Eliminates duplicate WidebandMedianFilter and FastMedianFilter
// Scott Meyers Item 41: Understand implicit interfaces
// USAGE: Include this BEFORE using MedianFilter<T>
template<typename T, size_t N = 11>
class MedianFilter {
public:
    MedianFilter() : window_{}, head_(0), full_(false) {
    }

    void add(T value) {
        window_[head_] = value;
        head_ = (head_ + 1) % N;
        if (head_ == 0) full_ = true;
    }

    T get_median() const {
        if (!full_) return window_[0];

        std::array<T, N> temp = window_;
        size_t k = N / 2;

        // Selection algorithm: O(N*k) where k=N/2
        // More efficient than full sort (O(N log N))
        for (size_t i = 0; i <= k; ++i) {
            size_t min_idx = i;
            for (size_t j = i + 1; j < N; ++j) {
                if (temp[j] < temp[min_idx]) {
                    min_idx = j;
                }
            }
            if (min_idx != i) {
                std::swap(temp[i], temp[min_idx]);
            }
        }
        return temp[k];
    }

    void reset() {
        full_ = false;
        head_ = 0;
        window_.fill(static_cast<T>(0));
    }

private:
    std::array<T, N> window_;
    size_t head_;
    bool full_;
};

// ===========================================
// MOVED TO UNIFIED LOOKUP TABLES
// ===========================================
// Spectrum mode names:    UnifiedStringLookup::SPECTRUM_MODE_NAMES
// Threat colors:          UnifiedColorLookup::threat()
// Drone colors:            UnifiedColorLookup::drone()
// Threat names:            UnifiedStringLookup::threat_name()
// Drone type names:        UnifiedStringLookup::drone_type_name()
// Single source of truth:  color_lookup_unified.hpp

// ===========================================
// CACHED VALUE HELPER
// ===========================================
// Eliminates duplicate Check-Before-Update logic
// Scott Meyers Item 11: Handle assignment to self in operator=
template<typename T>
class CachedValue {
public:
    explicit CachedValue(const T& initial_value = T{}) 
        : value_(initial_value), dirty_(true) {}

    bool update(const T& new_value) {
        if (value_ != new_value) {
            value_ = new_value;
            dirty_ = true;
            return true;  // Changed
        }
        return false;  // Unchanged
    }

    bool is_dirty() const { return dirty_; }
    void clear_dirty() { dirty_ = false; }
    const T& get() const { return value_; }
    T& get() { return value_; }

private:
    T value_;
    bool dirty_;
};

// ===========================================
// CONSTEXPR FREQUENCY VALIDATION
// ===========================================
// Eliminates duplicate frequency range checking
// Scott Meyers Item 15: Prefer constexpr to #define
// REQUIRES: Frequency type (int64_t)
struct FrequencyValidator {
    static constexpr int64_t MIN_HARDWARE_FREQ = 1'000'000LL;
    static constexpr int64_t MAX_HARDWARE_FREQ = 7'200'000'000LL;

    static constexpr bool is_in_range(int64_t value, int64_t min_val, int64_t max_val) {
        return value >= min_val && value <= max_val;
    }

    static constexpr bool is_valid_frequency(int64_t hz) {
        return is_in_range(hz, MIN_HARDWARE_FREQ, MAX_HARDWARE_FREQ);
    }

    static constexpr bool is_valid_2_4ghz_band(int64_t hz) {
        return is_in_range(hz, 2400000000LL, 2483500000LL);
    }

    static constexpr bool is_valid_5_8ghz_band(int64_t hz) {
        return is_in_range(hz, 5725000000LL, 5875000000LL);
    }

    static constexpr bool is_valid_military_band(int64_t hz) {
        return is_in_range(hz, 860000000LL, 930000000LL);
    }

    static constexpr bool is_valid_433mhz_ism(int64_t hz) {
        return is_in_range(hz, 433000000LL, 435000000LL);
    }
};

// ===========================================
// DRONE TYPE DETECTION
// ===========================================
// Eliminates duplicate identify_drone_type() functions
// REQUIRES: Frequency type (int64_t), DroneType enum
struct DroneTypeDetector {
    static constexpr uint8_t from_frequency(int64_t hz) {
        // 2.4 GHz band
        if (FrequencyValidator::is_valid_2_4ghz_band(hz)) {
            return 1;  // MAVIC - DJI typically uses 2.4GHz
        }
        // 5.8 GHz band
        if (FrequencyValidator::is_valid_5_8ghz_band(hz)) {
            return 4;  // FPV_RACING - 5.8GHz is common for FPV
        }
        // Military band
        if (FrequencyValidator::is_valid_military_band(hz)) {
            return 6;  // MILITARY_DRONE
        }
        // 433 MHz ISM band
        if (FrequencyValidator::is_valid_433mhz_ism(hz)) {
            return 5;  // DIY_DRONE
        }
        return 0;  // UNKNOWN
    }
};

// ===========================================
// FREQUENCY FORMATTER
// ===========================================
// Eliminates duplicate frequency formatting logic
// Scott Meyers Item 25: Consider support for implicit interfaces
// REQUIRES: Frequency type (int64_t alias), to_string_dec_uint()
// NOTE: Frequency is defined as using Frequency = int64_t; in ui_drone_common_types.hpp
struct FrequencyFormatter {
    enum class Format { 
        COMPACT_GHZ,      // "2.4G" - for tight spaces
        COMPACT_MHZ,      // "2400" - for MHz values
        STANDARD_GHZ,      // "2.450GHz" - for detailed display
        STANDARD_MHZ,      // "2400.0MHz" - for MHz with decimal
        DETAILED_GHZ,      // "2.450GHz" - 3 decimal places
        SPACED_GHZ         // "2.4 GHz" - for labels with space
    };

    // Stack-based formatting (no heap allocation)
    // Returns std::string with SSO optimization for short strings
    // NOTE: Frequency must be included/defined before using this
    static std::string format(int64_t freq_hz, Format fmt) {
        char buffer[24]; // Sufficient for "5.875000GHz" + null
        
        switch (fmt) {
            case Format::COMPACT_GHZ: {
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000ULL) / 100000000ULL);
                if (decimal > 0) {
                    snprintf(buffer, sizeof(buffer), "%lu.%luG", 
                             static_cast<unsigned long>(ghz), 
                             static_cast<unsigned long>(decimal));
                } else {
                    snprintf(buffer, sizeof(buffer), "%luG", 
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::COMPACT_MHZ: {
                uint32_t mhz = static_cast<uint32_t>((freq_hz + 500000) / 1000000ULL);
                snprintf(buffer, sizeof(buffer), "%lu", 
                         static_cast<unsigned long>(mhz));
                break;
            }
            case Format::STANDARD_GHZ: {
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000ULL);
                uint32_t decimal = static_cast<uint32_t>((freq_hz % 1000000000ULL) / 100000000ULL);
                if (decimal > 0) {
                    snprintf(buffer, sizeof(buffer), "%lu.%luGHz", 
                             static_cast<unsigned long>(ghz), 
                             static_cast<unsigned long>(decimal));
                } else {
                    snprintf(buffer, sizeof(buffer), "%luGHz", 
                             static_cast<unsigned long>(ghz));
                }
                break;
            }
            case Format::STANDARD_MHZ: {
                uint32_t mhz = static_cast<uint32_t>(freq_hz / 1000000ULL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000ULL) / 100000ULL);
                if (decimals > 0) {
                    snprintf(buffer, sizeof(buffer), "%lu.%luMHz", 
                             static_cast<unsigned long>(mhz), 
                             static_cast<unsigned long>(decimals));
                } else {
                    snprintf(buffer, sizeof(buffer), "%luMHz", 
                             static_cast<unsigned long>(mhz));
                }
                break;
            }
            case Format::DETAILED_GHZ: {
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000ULL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000000ULL) / 10000000ULL);
                snprintf(buffer, sizeof(buffer), "%lu.%03luGHz", 
                         static_cast<unsigned long>(ghz), 
                         static_cast<unsigned long>(decimals));
                break;
            }
            case Format::SPACED_GHZ: {
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000ULL);
                snprintf(buffer, sizeof(buffer), "%lu GHz", 
                         static_cast<unsigned long>(ghz));
                break;
            }
        }

        return std::string(buffer, strlen(buffer)); // SSO optimization
    }

    // 🔴 FIX: Non-allocating version for performance-critical paths
    // Writes directly to user-provided buffer (no malloc/free)
    static void format_to_buffer(char* buffer, size_t buffer_size, int64_t freq_hz, Format fmt) {
        if (!buffer || buffer_size == 0) return;

        switch (fmt) {
            case Format::COMPACT_GHZ: {
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000LL);
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
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000LL);
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
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000LL);
                uint32_t decimals = static_cast<uint32_t>((freq_hz % 1000000000LL) / 10000000ULL);
                snprintf(buffer, buffer_size, "%lu.%03luGHz",
                         static_cast<unsigned long>(ghz),
                         static_cast<unsigned long>(decimals));
                break;
            }
            case Format::SPACED_GHZ: {
                uint32_t ghz = static_cast<uint32_t>(freq_hz / 1000000000LL);
                snprintf(buffer, buffer_size, "%lu GHz",
                         static_cast<unsigned long>(ghz));
                break;
            }
        }
    }

    // Alias for existing code compatibility
    static std::string to_string_short_freq(int64_t freq_hz) {
        return format(freq_hz, Format::COMPACT_GHZ);
    }

    // 🔴 FIX: Alias for buffer-based formatting (non-allocating)
    static void to_string_short_freq_buffer(char* buffer, size_t buffer_size, int64_t freq_hz) {
        format_to_buffer(buffer, buffer_size, freq_hz, Format::COMPACT_GHZ);
    }
};

// ===========================================
// THREAT CLASSIFICATION
// ===========================================
// Eliminates duplicate threat classification logic
// REQUIRES: ThreatLevel, DroneType enums
struct ThreatClassifier {
    // Classify threat by RSSI (simple version)
    static constexpr ThreatLevel from_rssi(int32_t rssi_db) {
        if (rssi_db >= -50) return static_cast<ThreatLevel>(4);  // CRITICAL
        if (rssi_db >= -60) return static_cast<ThreatLevel>(3);  // HIGH
        if (rssi_db >= -70) return static_cast<ThreatLevel>(2);  // MEDIUM
        if (rssi_db >= -80) return static_cast<ThreatLevel>(1);  // LOW
        return static_cast<ThreatLevel>(0);  // NONE
    }

    // Classify threat by SNR and signal type (advanced version)
    static constexpr ThreatLevel from_snr_and_type(uint8_t snr, uint8_t type) {
        // Military drones (type=6) = always high threat
        if (type == 6) {
            if (snr >= 15) return static_cast<ThreatLevel>(4);  // CRITICAL
            if (snr >= 10) return static_cast<ThreatLevel>(3);  // HIGH
            return static_cast<ThreatLevel>(2);  // MEDIUM
        }

        // FPV racing (type=4) = variable threat
        if (type == 4) {
            if (snr >= 20) return static_cast<ThreatLevel>(3);  // HIGH
            if (snr >= 10) return static_cast<ThreatLevel>(2);  // MEDIUM
            if (snr >= 5) return static_cast<ThreatLevel>(1);  // LOW
            return static_cast<ThreatLevel>(0);  // NONE
        }

        // Commercial drones (MAVIC=1, PHANTOM=3) = moderate threat
        if (snr >= 25) return static_cast<ThreatLevel>(4);  // CRITICAL
        if (snr >= 15) return static_cast<ThreatLevel>(3);  // HIGH
        if (snr >= 10) return static_cast<ThreatLevel>(2);  // MEDIUM
        if (snr >= 5) return static_cast<ThreatLevel>(1);  // LOW
        return static_cast<ThreatLevel>(0);  // NONE
    }
};

// ===========================================
// TREND SYMBOL LOOKUP
// ===========================================
// Eliminates duplicate switch statements for trend symbols
struct TrendSymbols {
    static constexpr char SYMBOLS[4] = {
        '=',  // STATIC (0)
        '^',  // APPROACHING (1)
        'v',  // RECEDING (2)
        '~'   // UNKNOWN (3)
    };

    static constexpr char from_trend(uint8_t trend) {
        return (trend < 4) ? SYMBOLS[trend] : SYMBOLS[3];
    }
};

// ===========================================
// SAFE BUFFER ACCESS HELPER
// ===========================================
// Eliminates duplicate nullptr checks for heap-allocated buffers
// Scott Meyers Item 17: Understand special member function generation
template<typename T, size_t N>
struct SafeBufferAccess {
    static constexpr T& get(std::unique_ptr<std::array<T, N>>& ptr, size_t idx, T& fallback) {
        return ptr ? (*ptr)[idx] : fallback;
    }

    static constexpr const T& get(const std::unique_ptr<std::array<T, N>>& ptr, size_t idx, const T& fallback) {
        return ptr ? (*ptr)[idx] : fallback;
    }

    static constexpr bool is_valid(const std::unique_ptr<std::array<T, N>>& ptr) {
        return ptr != nullptr;
    }
};

// ===========================================
// STATUS FORMATTING HELPER
// ===========================================
// Eliminates repeated snprintf + widget.set patterns
// Scott Meyers Item 25: Consider support for implicit interfaces
struct StatusFormatter {
    template<size_t N, typename... Args>
    static void format_to(char (&buffer)[N], const char* fmt, Args&&... args) {
        snprintf(buffer, N, fmt, std::forward<Args>(args)...);
    }

    template<size_t N, typename... Args>
    static std::string make_string(const char* fmt, Args&&... args) {
        char buffer[N];
        snprintf(buffer, N, fmt, std::forward<Args>(args)...);
        return std::string(buffer);
    }
};

// ===========================================
// VALIDATOR ERROR FORMATTER
// ===========================================
// Eliminates duplicate error formatting in validators
struct ValidatorFormatter {
    static constexpr size_t ERROR_BUFFER_SIZE = 128;

    static std::string out_of_range(const char* name, int64_t value, int64_t min, int64_t max) {
        char buffer[ERROR_BUFFER_SIZE];
        snprintf(buffer, ERROR_BUFFER_SIZE, "%s %lld invalid (must be %lld to %lld)",
                 name, static_cast<long long>(value),
                 static_cast<long long>(min), static_cast<long long>(max));
        return buffer;
    }

    static std::string out_of_range(const char* name, uint64_t value, uint64_t min, uint64_t max) {
        char buffer[ERROR_BUFFER_SIZE];
        snprintf(buffer, ERROR_BUFFER_SIZE, "%s %llu invalid (must be %llu to %llu)",
                 name, static_cast<unsigned long long>(value),
                 static_cast<unsigned long long>(min), static_cast<unsigned long long>(max));
        return buffer;
    }

    static std::string out_of_range(const char* name, int32_t value, int32_t min, int32_t max) {
        return out_of_range(name, static_cast<int64_t>(value), static_cast<int64_t>(min), static_cast<int64_t>(max));
    }

    static std::string out_of_range(const char* name, uint32_t value, uint32_t min, uint32_t max) {
        return out_of_range(name, static_cast<uint64_t>(value), static_cast<uint64_t>(min), static_cast<uint64_t>(max));
    }
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_OPTIMIZED_UTILS_HPP_