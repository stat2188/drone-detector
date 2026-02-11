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
// SPECTRUM MODE LOOKUP TABLES
// ===========================================
// Eliminates duplicate spectrum_mode switches
// Scott Meyers Item 15: Prefer constexpr to #define
struct SpectrumModeMappings {
    static constexpr const char* const NAMES[] = {
        "NARROW",       // NARROW = 0
        "MEDIUM",       // MEDIUM = 1
        "WIDE",         // WIDE = 2
        "ULTRA_WIDE",   // ULTRA_WIDE = 3
        "ULTRA_NARROW"  // ULTRA_NARROW = 4
    };
    
    static constexpr const char* get_name(uint8_t mode) {
        return (mode < 5) ? NAMES[mode] : "MEDIUM";
    }
};

// ===========================================
// COLOR LOOKUP TABLES
// ===========================================
// Eliminates 150+ lines of switch-case statements
// Scott Meyers Item 15: Prefer constexpr to #define
// USAGE: Use ColorMappings::get_threat_color(ThreatLevel::LOW)
// REQUIRES: Color enum from ui namespace
struct ColorMappings {
    // RGB values for colors (uint32_t format: 0xRRGGBB)
    // Eliminates dependency on Color class
    struct RGBColor {
        uint32_t value;
        
        static constexpr RGBColor blue()   { return RGBColor{0xFF0000}; }
        static constexpr RGBColor green()  { return RGBColor{0x00FF00}; }
        static constexpr RGBColor yellow() { return RGBColor{0xFFFF00}; }
        static constexpr RGBColor orange() { return RGBColor{0xFFA500}; }
        static constexpr RGBColor red()    { return RGBColor{0x0000FF}; }
        static constexpr RGBColor grey()   { return RGBColor{0x808080}; }
        static constexpr RGBColor white()  { return RGBColor{0xFFFFFF}; }
        static constexpr RGBColor cyan()    { return RGBColor{0xFFFF00}; }
        static constexpr RGBColor magenta(){ return RGBColor{0xFF00FF}; }
        static constexpr RGBColor dark_grey(){ return RGBColor{0x404040}; }
    };

    // Threat level colors (6 levels) - RGB888 format: 0xRRGGBB
    static constexpr uint32_t THREAT_COLORS[6] = {
        0xFF0000,   // Red - NONE (0)
        0x00FF00,   // Green - LOW (1)
        0xFFFF00,   // Yellow - MEDIUM (2)
        0xFFA500,   // Orange - HIGH (3)
        0x800000,   // Dark Red - CRITICAL (4)
        0x808080    // Grey - UNKNOWN (5)
    };

    // Drone type colors (11 types - matches DroneType enum)
    static constexpr uint32_t DRONE_COLORS[11] = {
        0xFFFFFF,   // white - UNKNOWN (0)
        0xFF0000,   // red - MAVIC (1)
        0xFFA500,   // orange - DJI_P34 (2)
        0xFFFF00,   // yellow - PHANTOM (3)
        0x00FFFF,   // cyan - DJI_MINI (4)
        0xFF00FF,   // magenta - PARROT_ANAFI (5)
        0x00FF00,   // green - PARROT_BEBOP (6)
        0x800080,   // purple - PX4_DRONE (7)
        0x000000,   // black - MILITARY_DRONE (8)
        0xC0C0C0,   // silver - DIY_DRONE (9)
        0xFFC0CB    // pink - FPV_RACING (10)
    };

    // NOTE: These return uint32_t RGB values, not Color objects
    // Caller must convert to Color if needed
    static constexpr uint32_t get_threat_color_value(uint8_t level) {
        return (level < 6) ? THREAT_COLORS[level] : THREAT_COLORS[5];
    }

    static constexpr uint32_t get_drone_color_value(uint8_t type) {
        return (type < 11) ? DRONE_COLORS[type] : DRONE_COLORS[0];
    }
};

// ===========================================
// STRING LOOKUP TABLES
// ===========================================
// Eliminates duplicate get_drone_type_name() functions
// Scott Meyers Item 15: Prefer constexpr to #define
struct StringMappings {
    static constexpr const char* const THREAT_NAMES[6] = {
        "NONE",     // NONE (0)
        "LOW",       // LOW (1)
        "MEDIUM",    // MEDIUM (2)
        "HIGH",      // HIGH (3)
        "CRITICAL",  // CRITICAL (4)
        "UNKNOWN"     // UNKNOWN (5)
    };

    static constexpr const char* const DRONE_TYPE_NAMES[11] = {
        "UNKNOWN",        // UNKNOWN (0)
        "MAVIC",          // MAVIC (1)
        "DJI P34",       // DJI_P34 (2)
        "PHANTOM",        // PHANTOM (3)
        "MINI",           // DJI_MINI (4)
        "ANAFI",          // PARROT_ANAFI (5)
        "BEBOP",          // PARROT_BEBOP (6)
        "PX4",            // PX4_DRONE (7)
        "MILITARY",       // MILITARY_DRONE (8)
        "DIY DRONE",      // DIY_DRONE (9)
        "FPV RACING"      // FPV_RACING (10)
    };

    static constexpr const char* get_threat_name(uint8_t level) {
        return (level < 6) ? THREAT_NAMES[level] : "UNKNOWN";
    }

    static constexpr const char* get_drone_type_name(uint8_t type) {
        return (type < 11) ? DRONE_TYPE_NAMES[type] : "UNKNOWN";
    }
};

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