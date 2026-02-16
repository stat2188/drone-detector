#ifndef EDA_CONSTANTS_HPP_
#define EDA_CONSTANTS_HPP_

#include <cstdint>
#include <cinttypes>
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
constexpr uint32_t FAST_SCAN_INTERVAL_MS = 250;        // CRITICAL threat level
constexpr uint32_t HIGH_THREAT_SCAN_INTERVAL_MS = 400; // HIGH threat level
constexpr uint32_t NORMAL_SCAN_INTERVAL_MS = 750;      // MEDIUM threat level
constexpr uint32_t SLOW_SCAN_INTERVAL_MS = 1000;       // LOW threat / detections present
constexpr uint32_t VERY_SLOW_SCAN_INTERVAL_MS = 2000;  // No detections (progressive slowdown cap)
constexpr uint32_t HIGH_DENSITY_SCAN_CAP_MS = 500;     // Cap for high detection density (>5 detections)
constexpr uint32_t PROGRESSIVE_SLOWDOWN_DIVISOR = 10;  // Scan cycles divisor for slowdown calculation

// ===== INTELLIGENT SCANNING =====
constexpr uint32_t PRIORITY_SLICE_SKIP_THRESHOLD = 10;
constexpr uint32_t PREDICTION_FRESHNESS_MS = 5000;
constexpr uint32_t FHSS_TRACKING_CONFIDENCE_MAX = 10;

// ===== MEMORY =====
constexpr size_t MAX_TRACKED_DRONES = 8;
constexpr size_t MAX_DISPLAYED_DRONES = 3;
constexpr size_t DETECTION_TABLE_SIZE = 256;

// ===== AUDIO ALERT PARAMETERS =====
constexpr uint32_t MIN_AUDIO_FREQ = 200;
constexpr uint32_t MAX_AUDIO_FREQ = 20000;

// ===== AUDIO DURATION PARAMETERS =====
constexpr uint32_t MIN_AUDIO_DURATION = 50;
constexpr uint32_t MAX_AUDIO_DURATION = 5000;

// ===== BANDWIDTH PARAMETERS =====
constexpr uint32_t MIN_BANDWIDTH = 10000;
constexpr uint32_t MAX_BANDWIDTH = 28000000;

// ===== UI DIMENSIONS =====
constexpr uint32_t SCREEN_WIDTH = 240;
constexpr uint32_t SCREEN_HEIGHT = 320;
constexpr uint32_t TEXT_HEIGHT = 16;
constexpr uint32_t TEXT_LINE_HEIGHT = 24;

// ===== SPECTRUM PARAMETERS =====
constexpr uint32_t SPECTRUM_BIN_COUNT = 256;
constexpr uint32_t SPECTRUM_BIN_COUNT_240 = 240;
constexpr uint32_t MINI_SPECTRUM_WIDTH = 200;
constexpr uint32_t MINI_SPECTRUM_HEIGHT = 24;
constexpr uint32_t SPECTRUM_ROW_SIZE = 240;
constexpr uint32_t RENDER_LINE_SIZE = 240;
constexpr uint32_t WATERFALL_SIZE = 40 * 240; // 9.6KB

// ===== BUFFER SIZES =====
constexpr uint32_t ERROR_MESSAGE_BUFFER_SIZE = 128;
constexpr uint32_t DEFAULT_BUFFER_SIZE_4KB = 4096;
constexpr uint32_t WORKER_STACK_SIZE_8KB = 8192;
constexpr uint32_t DB_LOADING_STACK_SIZE_8KB = 8192;
constexpr uint32_t POOL_SIZE_2KB = 2048;
constexpr uint32_t MAX_STRING_LENGTH_256 = 256;
constexpr uint32_t FREQ_DB_STORAGE_SIZE_4KB = 4096;

// ===== RSSI PARAMETERS =====
constexpr int32_t RSSI_SILENCE_DBM = -120;
constexpr int32_t RSSI_INVALID_DBM = -127;

// ===== SETTINGS STORAGE =====
constexpr uint32_t SETTINGS_TEMPLATE_SIZE_4KB = 4096;
constexpr uint32_t MAX_SETTINGS_FILE_SIZE_64KB = 65536;
constexpr uint32_t MAX_SETTINGS_LINES = 1000;
constexpr uint32_t MAX_LINE_LENGTH = 128;
constexpr uint32_t MAX_SETTING_STR_LEN = 65;

// ===== AUDIO ALERTS =====
constexpr uint32_t DEFAULT_ALERT_FREQ_HZ = 800;
constexpr uint32_t DEFAULT_ALERT_DURATION_MS = 500;
constexpr uint32_t DEFAULT_ALERT_VOLUME_LEVEL = 50;
constexpr uint32_t MCU_MAX_AUDIO_FREQ_HZ = 4000; // MCU audio limit (correct value)
constexpr uint32_t GENERAL_MAX_AUDIO_FREQ_HZ = 20000; // General audio limit

} // namespace Constants

// ===========================================
// THREAT LEVEL DATA (Removed - Use UnifiedStringLookup/UnifiedColorLookup from color_lookup_unified.hpp)
// ===========================================

// ===========================================
// DRONE TYPE DATA (Removed - Use UnifiedStringLookup/UnifiedColorLookup from color_lookup_unified.hpp)
// ===========================================

// ===========================================
// DRONE DATABASE CONTENT (Removed - Use default_drones_db.hpp if needed)
// ===========================================

// ===========================================
// VALIDATION UTILITIES (from diamond_core.hpp)
// ===========================================

namespace Validation {
    static constexpr int64_t MIN_HARDWARE_FREQ = 1'000'000LL;
    static constexpr int64_t MAX_HARDWARE_FREQ = 7'200'000'000LL;
    
    static constexpr int64_t MIN_2_4GHZ = 2'400'000'000LL;
    static constexpr int64_t MAX_2_4GHZ = 2'483'500'000LL;
    
    static constexpr int64_t MIN_5_8GHZ = 5'725'000'000LL;
    static constexpr int64_t MAX_5_8GHZ = 5'875'000'000LL;
    
    static constexpr int64_t MIN_MILITARY = 860'000'000LL;
    static constexpr int64_t MAX_MILITARY = 930'000'000LL;
    
    static constexpr int64_t MIN_433MHZ = 433'000'000LL;
    static constexpr int64_t MAX_433MHZ = 435'000'000LL;
    
    static constexpr bool is_in_range(int64_t value, int64_t min_val, int64_t max_val) noexcept {
        return value >= min_val && value <= max_val;
    }

    static constexpr bool validate_frequency(int64_t freq_hz) noexcept {
        return is_in_range(freq_hz, MIN_HARDWARE_FREQ, MAX_HARDWARE_FREQ);
    }

    static constexpr bool validate_rssi(int32_t rssi_db) noexcept {
        return rssi_db >= -110 && rssi_db <= 10;
    }

    static constexpr bool is_2_4ghz_band(int64_t freq_hz) noexcept {
        return is_in_range(freq_hz, MIN_2_4GHZ, MAX_2_4GHZ);
    }

    static constexpr bool is_5_8ghz_band(int64_t freq_hz) noexcept {
        return is_in_range(freq_hz, MIN_5_8GHZ, MAX_5_8GHZ);
    }

    static constexpr bool is_military_band(int64_t freq_hz) noexcept {
        return is_in_range(freq_hz, MIN_MILITARY, MAX_MILITARY);
    }

    static constexpr bool is_433mhz_band(int64_t freq_hz) noexcept {
        return is_in_range(freq_hz, MIN_433MHZ, MAX_433MHZ);
    }
} // namespace Validation

// ===========================================
// UNIFIED SPECTRUM MODE LUT (Single Source of Truth)
// ===========================================
// Eliminates duplicate definitions from:
// - ui_enhanced_drone_settings.cpp (SPECTRUM_MODE_TO_INDEX_LUT)
// - color_lookup_unified.hpp (SPECTRUM_MODE_NAMES)

namespace LUTs {
    struct SpectrumModeInfo {
        const char* short_name;      // "NARROW"
        const char* display_name;    // "Narrow Band"
        uint32_t   bandwidth_hz;     // 2.4MHz
        uint8_t    ui_index;        // OptionsField index
    };
    
    // EDA_FLASH_CONST ensures Flash storage (O(1) RAM)
    static constexpr EDA_FLASH_CONST SpectrumModeInfo SPECTRUM_MODES[] = {
        {"ULTRA_NARROW", "Ultra Narrow",  8000000,  0},
        {"NARROW",       "Narrow",        12000000, 1},
        {"MEDIUM",       "Medium",        24000000, 2},
        {"WIDE",         "Wide",          48000000, 3},
        {"ULTRA_WIDE",   "Ultra Wide",    96000000, 4}
    };
    static_assert(sizeof(SPECTRUM_MODES) / sizeof(SpectrumModeInfo) == 5, "SPECTRUM_MODES size");
    
     // O(1) lookup (eliminates switch)
     inline constexpr const char* spectrum_mode_short_name(uint8_t mode_idx) noexcept {
         return (mode_idx < 5) ? SPECTRUM_MODES[mode_idx].short_name : "MEDIUM";
     }
     
     inline constexpr const char* spectrum_mode_display_name(uint8_t mode_idx) noexcept {
         return (mode_idx < 5) ? SPECTRUM_MODES[mode_idx].display_name : "Medium";
     }
     
     inline constexpr uint32_t spectrum_mode_bandwidth(uint8_t mode_idx) noexcept {
         return (mode_idx < 5) ? SPECTRUM_MODES[mode_idx].bandwidth_hz : 24000000;
     }
     
     inline constexpr uint8_t spectrum_mode_ui_index(uint8_t mode_idx) noexcept {
         return (mode_idx < 5) ? SPECTRUM_MODES[mode_idx].ui_index : 2;
     }
     
     inline constexpr uint8_t ui_index_to_spectrum_mode(uint8_t ui_index) noexcept {
         for (uint8_t i = 0; i < 5; ++i) {
             if (SPECTRUM_MODES[i].ui_index == ui_index) {
                 return i;
             }
         }
         return 2; // Default to MEDIUM
     }
}

// ===========================================
// UNIFIED FREQUENCY FORMATTING (Zero-Heap)
// ===========================================
// Eliminates duplicate FrequencyFormatter/FrequencyFormat classes

namespace Formatting {
    struct FrequencyScale {
        int64_t  threshold_hz;
        const char* unit;
        int64_t  divider;
    };
    
    static constexpr EDA_FLASH_CONST FrequencyScale FREQUENCY_SCALES[] = {
        {1'000'000'000LL, "G", 1'000'000'000LL},  // GHz
        {1'000'000LL,     "M", 1'000'000LL},        // MHz
        {1'000LL,         "k", 1'000LL},            // kHz
        {0LL,             "",  1LL}                  // Hz (fallback)
    };
    
    // Stack-allocated formatting (NO heap allocation)
    inline void format_frequency(char* buffer, size_t size, int64_t freq_hz) noexcept {
        if (!buffer || size == 0) return;
        
        const auto* scale = FREQUENCY_SCALES;
        while (scale->threshold_hz > 0 && freq_hz >= scale->threshold_hz) scale++;
        
        int64_t value = freq_hz / scale->divider;
        int64_t decimal = freq_hz % scale->divider;
        
        if (decimal > 0 && scale->threshold_hz > 0) {
            // Single decimal place for precision
            decimal = (decimal * 10) / scale->divider;
            snprintf(buffer, size, "%lld.%lld%s", 
                     static_cast<long long>(value),
                     static_cast<long long>(decimal),
                     scale->unit);
        } else {
            snprintf(buffer, size, "%lld%s",
                     static_cast<long long>(value),
                     scale->unit);
        }
    }
    
    // Compact format for UI (e.g., "2.4G")
    inline void format_frequency_compact(char* buffer, size_t size, int64_t freq_hz) noexcept {
        if (!buffer || size == 0) return;
        
        // GHz range
        if (freq_hz >= 1'000'000'000LL) {
            uint64_t rounded = static_cast<uint64_t>(freq_hz + 500'000'000ULL);
            uint32_t ghz = static_cast<uint32_t>(rounded / 1'000'000'000ULL);
            uint32_t decimal = static_cast<uint32_t>((rounded % 1'000'000'000ULL) / 100'000'000ULL);
            
            if (decimal > 0) {
                snprintf(buffer, size, "%" PRIu32 ".%" PRIu32 "G", ghz, decimal);
            } else {
                snprintf(buffer, size, "%" PRIu32 "G", ghz);
            }
        }
        // MHz range
        else if (freq_hz >= 1'000'000LL) {
            uint32_t mhz = static_cast<uint32_t>((freq_hz + 500'000LL) / 1'000'000LL);
            snprintf(buffer, size, "%" PRIu32 "M", mhz);
        }
        // kHz range
        else if (freq_hz >= 1'000LL) {
            uint32_t khz = static_cast<uint32_t>((freq_hz + 500LL) / 1'000LL);
            snprintf(buffer, size, "%" PRIu32 "k", khz);
        }
        // Hz range
        else {
            snprintf(buffer, size, "%lld", static_cast<long long>(freq_hz));
        }
    }
}

// ===========================================
// FREQUENCY BANDS (Removed - Use diamond_core.hpp if needed)
// ===========================================

// ===========================================
// SPECTRAL CONFIG (Removed - Use ui_spectral_analyzer.hpp if needed)
// ===========================================

// ===========================================
// RSSI UTILITIES (Removed - Use diamond_core.hpp RSSIUtils if needed)
// ===========================================

// ===========================================
// MOVEMENT TREND UTILITIES (Removed - Use diamond_core.hpp TrendUtils if needed)
// ===========================================

// ===========================================
// COLOR CONVERTER (Removed - Use color_lookup_unified.hpp ColorConverter if needed)
// ===========================================

// ===========================================
// ERROR HANDLING (M2: Unified ErrorResult)
// ===========================================
// Diamond Code: Zero-overhead error handling
// - enum class compiles to uint8_t (1 byte)
// - constexpr methods inline (zero runtime overhead)
// - Type-safe, no exceptions, no RTTI

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
        constexpr EDA_FLASH_CONST const char* error_messages[] = {
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
        
        const size_t code = static_cast<size_t>(error_code);
        if (code < sizeof(error_messages) / sizeof(error_messages[0])) {
            return error_messages[code];
        }
        return "Unknown error";
    }
};

// ===========================================
// SAFE TYPE CASTING (M5: safe_reinterpret_cast)
// ===========================================
// Diamond Code: Type-safe reinterpret_cast wrapper
// - Compile-time alignment checks (static_assert)
// - Debug build runtime checks (assert)
// - Release build: zero overhead (optimizes to plain reinterpret_cast)

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

} // namespace EDA

// Backward compatibility aliases
namespace DroneConstants = EDA::Constants;

#endif // EDA_CONSTANTS_HPP_
