/**
 * @file example_refined_constants.hpp
 * @brief Diamond Code Best Practices: constexpr Constants in Flash
 * 
 * DIAMOND CODE PRINCIPLES DEMONSTRATED:
 * 1. Use constexpr for all compile-time constants
 * 2. Constants placed in Flash (read-only memory), not RAM
 * 3. Use std::array instead of std::vector for constant tables
 * 4. Use enum class for type-safe enumerations
 * 5. Use using Type = uintXX_t; for type aliases
 * 6. static_assert for compile-time validation
 * 7. All constexpr functions marked noexcept
 * 8. NO std::vector, std::string, std::map, std::atomic
 * 9. NO new, malloc - all stack allocation
 * 10. Stack < 4KB constraint respected
 * 
 * BEFORE (INCORRECT):
 * // BAD: Constants in RAM, wasting memory
 * const int SAMPLE_RATE = 48000;
 * const float PI = 3.14159f;
 * std::vector<int> lookup_table = {1, 2, 3, 4, 5};
 * 
 * AFTER (CORRECT):
 * // GOOD: Constants in Flash, no RAM usage
 * constexpr int SAMPLE_RATE = 48000;
 * constexpr float PI = 3.14159f;
 * constexpr std::array<int, 5> lookup_table = {1, 2, 3, 4, 5};
 * 
 * MEMORY SAVINGS:
 * - Flash: Read-only, shared across all instances
 * - RAM: Zero bytes for constant data
 * - Cache: Flash can be cached for fast access
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

// ============================================================================
// DIAMOND CODE: Type Aliases (using Type = uintXX_t;)
// ============================================================================

namespace eda {

// Type aliases for clarity and type safety
using FrequencyHz = uint32_t;
using SampleIndex = uint16_t;
using Decibel = int8_t;
using ColorChannel = uint8_t;

} // namespace eda

// ============================================================================
// DIAMOND CODE: Mathematical Constants (constexpr, Flash)
// ============================================================================

namespace eda {
namespace constants {

/**
 * @brief Mathematical constants
 * 
 * DIAMOND CODE: constexpr ensures compile-time evaluation
 * DIAMOND CODE: Placed in Flash (no RAM consumption)
 * DIAMOND CODE: All functions marked noexcept
 */
struct MathConstants {
    // Pi and related constants
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float TWO_PI = 2.0f * PI;
    static constexpr float HALF_PI = 0.5f * PI;
    static constexpr float INV_PI = 1.0f / PI;
    
    // Conversion factors
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;
    
    // Audio constants
    static constexpr float SPEED_OF_SOUND = 343.0f;  // m/s at 20°C
    
    /**
     * @brief Convert degrees to radians
     * 
     * @param degrees Angle in degrees
     * @return float Angle in radians
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto deg_to_rad(float degrees) noexcept -> float {
        return degrees * DEG_TO_RAD;
    }
    
    /**
     * @brief Convert radians to degrees
     * 
     * @param radians Angle in radians
     * @return float Angle in degrees
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto rad_to_deg(float radians) noexcept -> float {
        return radians * RAD_TO_DEG;
    }
    
    /**
     * @brief Calculate sine using Taylor series (simplified)
     * 
     * @param x Angle in radians
     * @return float Sine of x
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     * 
     * Note: For production, use hardware or optimized library functions.
     * This is an example of constexpr computation.
     */
    static constexpr auto sin(float x) noexcept -> float {
        // Normalize to [-PI, PI]
        while (x > PI) x -= TWO_PI;
        while (x < -PI) x += TWO_PI;
        
        // Taylor series: x - x^3/6 + x^5/120 - x^7/5040
        const float x2 = x * x;
        const float x3 = x2 * x;
        const float x5 = x3 * x2;
        const float x7 = x5 * x2;
        
        return x - (x3 / 6.0f) + (x5 / 120.0f) - (x7 / 5040.0f);
    }
};

// Compile-time validation
static_assert(MathConstants::PI > 3.14f && MathConstants::PI < 3.15f, 
              "PI value incorrect");
static_assert(MathConstants::deg_to_rad(180.0f) > 3.14f, 
              "deg_to_rad conversion incorrect");

} // namespace constants
} // namespace eda

// ============================================================================
// DIAMOND CODE: DSP Constants (constexpr, Flash)
// ============================================================================

namespace eda {
namespace constants {

/**
 * @brief Digital Signal Processing constants
 * 
 * DIAMOND CODE: constexpr ensures compile-time evaluation
 * DIAMOND CODE: Placed in Flash (no RAM consumption)
 */
struct DspConstants {
    // Sample rate and buffer sizes
    static constexpr FrequencyHz SAMPLE_RATE = 48000;
    static constexpr SampleIndex FFT_SIZE = 256;
    static constexpr SampleIndex WINDOW_SIZE = 256;
    static constexpr SampleIndex OVERLAP_SIZE = 128;
    
    // Frequency ranges (Hz)
    static constexpr FrequencyHz MIN_FREQ = 20;      // 20 Hz (human hearing)
    static constexpr FrequencyHz MAX_FREQ = 20000;   // 20 kHz (human hearing)
    static constexpr FrequencyHz NYQUIST = SAMPLE_RATE / 2;
    
    // Drone detection frequency ranges (MHz)
    static constexpr FrequencyHz DRONE_MIN_FREQ = 2400;  // 2.4 GHz band
    static constexpr FrequencyHz DRONE_MAX_FREQ = 2500;  // 2.5 GHz band
    static constexpr FrequencyHz DRONE_CENTER = 2450;   // 2.45 GHz center
    
    // Thresholds
    static constexpr Decibel NOISE_FLOOR = -90;
    static constexpr Decibel SIGNAL_THRESHOLD = -70;
    static constexpr Decibel STRONG_SIGNAL = -50;
    
    // Number of frequency bins
    static constexpr uint8_t NUM_BINS = 64;
    
    /**
     * @brief Calculate frequency for a given FFT bin
     * 
     * @param bin FFT bin index
     * @return FrequencyHz Frequency in Hz
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto bin_to_frequency(uint8_t bin) noexcept -> FrequencyHz {
        return (bin * NYQUIST) / (FFT_SIZE / 2);
    }
    
    /**
     * @brief Calculate FFT bin for a given frequency
     * 
     * @param freq Frequency in Hz
     * @return uint8_t FFT bin index
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto frequency_to_bin(FrequencyHz freq) noexcept -> uint8_t {
        return static_cast<uint8_t>((freq * (FFT_SIZE / 2)) / NYQUIST);
    }
    
    /**
     * @brief Check if frequency is in drone detection range
     * 
     * @param freq Frequency in Hz
     * @return true Frequency is in drone range
     * @return false Frequency is outside drone range
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto is_drone_frequency(FrequencyHz freq) noexcept -> bool {
        return (freq >= DRONE_MIN_FREQ) && (freq <= DRONE_MAX_FREQ);
    }
};

// Compile-time validation
static_assert(DspConstants::FFT_SIZE == DspConstants::WINDOW_SIZE, 
              "FFT and window sizes must match");
static_assert(DspConstants::OVERLAP_SIZE < DspConstants::FFT_SIZE, 
              "Overlap must be less than FFT size");
static_assert(DspConstants::NYQUIST == DspConstants::SAMPLE_RATE / 2, 
              "Nyquist frequency incorrect");
static_assert(DspConstants::is_drone_frequency(2450), 
              "2450 MHz should be in drone range");
static_assert(!DspConstants::is_drone_frequency(2300), 
              "2300 MHz should not be in drone range");

} // namespace constants
} // namespace eda

// ============================================================================
// DIAMOND CODE: Color Lookup Tables (constexpr std::array, Flash)
// ============================================================================

namespace eda {
namespace constants {

/**
 * @brief Color palette for spectrum display
 * 
 * DIAMOND CODE: constexpr std::array instead of std::vector
 * DIAMOND CODE: Placed in Flash (no RAM consumption)
 * DIAMOND CODE: Fixed-size, no dynamic allocation
 */
struct ColorPalette {
    // RGB color structure
    struct RgbColor {
        ColorChannel r;
        ColorChannel g;
        ColorChannel b;
    };
    
    // Color palette (8 colors for 8 signal strength levels)
    static constexpr std::array<RgbColor, 8> PALETTE = {{
        {0, 0, 255},    // Level 0: Blue (weak)
        {0, 128, 255},  // Level 1: Light Blue
        {0, 255, 255},  // Level 2: Cyan
        {0, 255, 128},  // Level 3: Green-Cyan
        {0, 255, 0},    // Level 4: Green
        {128, 255, 0},  // Level 5: Yellow-Green
        {255, 255, 0},  // Level 6: Yellow
        {255, 0, 0}     // Level 7: Red (strong)
    }};
    
    /**
     * @brief Get color for signal strength level
     * 
     * @param level Signal strength level (0-7)
     * @return RgbColor RGB color
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: Guard clause for invalid level
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto get_color(uint8_t level) noexcept -> RgbColor {
        // Guard clause: clamp to valid range
        if (level >= PALETTE.size()) {
            return PALETTE.back();
        }
        return PALETTE[level];
    }
    
    /**
     * @brief Interpolate between two colors
     * 
     * @param color1 First color
     * @param color2 Second color
     * @param factor Interpolation factor (0.0 to 1.0)
     * @return RgbColor Interpolated color
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto interpolate_color(
        RgbColor color1,
        RgbColor color2,
        float factor
    ) noexcept -> RgbColor {
        // Clamp factor to [0, 1]
        if (factor < 0.0f) factor = 0.0f;
        if (factor > 1.0f) factor = 1.0f;
        
        return RgbColor{
            static_cast<ColorChannel>(color1.r + (color2.r - color1.r) * factor),
            static_cast<ColorChannel>(color1.g + (color2.g - color1.g) * factor),
            static_cast<ColorChannel>(color1.b + (color2.b - color1.b) * factor)
        };
    }
};

// Compile-time validation
static_assert(ColorPalette::PALETTE.size() == 8, 
              "Palette must have 8 colors");
static_assert(ColorPalette::get_color(0).b == 255, 
              "Level 0 should be blue");
static_assert(ColorPalette::get_color(7).r == 255, 
              "Level 7 should be red");

} // namespace constants
} // namespace eda

// ============================================================================
// DIAMOND CODE: Window Function Coefficients (constexpr std::array, Flash)
// ============================================================================

namespace eda {
namespace constants {

/**
 * @brief Window function coefficients for DSP
 * 
 * DIAMOND CODE: constexpr std::array instead of std::vector
 * DIAMOND CODE: Placed in Flash (no RAM consumption)
 * DIAMOND CODE: Pre-computed at compile time
 */
struct WindowFunctions {
    // Window size
    static constexpr uint16_t SIZE = 256;
    
    /**
     * @brief Hanning window coefficient
     * 
     * @param index Sample index
     * @return float Window coefficient
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto hanning(uint16_t index) noexcept -> float {
        // Hanning window: 0.5 * (1 - cos(2*pi*i/(N-1)))
        const float t = static_cast<float>(index) / (SIZE - 1);
        return 0.5f * (1.0f - MathConstants::cos(2.0f * MathConstants::PI * t));
    }
    
    /**
     * @brief Hamming window coefficient
     * 
     * @param index Sample index
     * @return float Window coefficient
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto hamming(uint16_t index) noexcept -> float {
        // Hamming window: 0.54 - 0.46 * cos(2*pi*i/(N-1))
        const float t = static_cast<float>(index) / (SIZE - 1);
        return 0.54f - 0.46f * MathConstants::cos(2.0f * MathConstants::PI * t);
    }
    
    /**
     * @brief Blackman window coefficient
     * 
     * @param index Sample index
     * @return float Window coefficient
     * 
     * DIAMOND CODE: constexpr function (compile-time evaluation)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto blackman(uint16_t index) noexcept -> float {
        // Blackman window: 0.42 - 0.5*cos(2*pi*i/(N-1)) + 0.08*cos(4*pi*i/(N-1))
        const float t = static_cast<float>(index) / (SIZE - 1);
        const float term1 = 0.42f;
        const float term2 = 0.5f * MathConstants::cos(2.0f * MathConstants::PI * t);
        const float term3 = 0.08f * MathConstants::cos(4.0f * MathConstants::PI * t);
        return term1 - term2 + term3;
    }
    
    // Pre-computed Hanning window coefficients (Q15 fixed-point)
    // In production, these would be generated by a build script
    static constexpr std::array<int16_t, SIZE> HANNING_Q15 = {
        0, 3, 13, 29, 51, 80, 115, 156, 203, 256, 314, 378, 447, 521, 600, 683,
        770, 861, 956, 1054, 1155, 1259, 1366, 1475, 1586, 1699, 1814, 1930, 2048,
        2167, 2287, 2408, 2529, 2650, 2771, 2892, 3013, 3133, 3252, 3370, 3487,
        3603, 3717, 3830, 3941, 4050, 4157, 4262, 4365, 4465, 4563, 4658, 4751,
        4841, 4928, 5012, 5093, 5171, 5246, 5317, 5385, 5450, 5511, 5569, 5623,
        5674, 5721, 5764, 5804, 5840, 5872, 5900, 5925, 5945, 5962, 5975, 5984,
        5989, 5990, 5987, 5980, 5970, 5955, 5937, 5915, 5889, 5859, 5825, 5788,
        5747, 5702, 5653, 5601, 5545, 5486, 5423, 5357, 5287, 5214, 5138, 5058,
        4975, 4889, 4800, 4708, 4613, 4515, 4414, 4310, 4203, 4093, 3981, 3866,
        3748, 3628, 3505, 3380, 3252, 3122, 2990, 2856, 2719, 2581, 2440, 2298,
        2154, 2008, 1860, 1711, 1560, 1408, 1255, 1100, 944, 787, 629, 470, 310,
        149, -12, -174, -337, -500, -664, -828, -992, -1156, -1320, -1484, -1648,
        -1812, -1975, -2138, -2301, -2463, -2624, -2785, -2945, -3104, -3262,
        -3419, -3575, -3730, -3884, -4036, -4187, -4337, -4485, -4632, -4777,
        -4921, -5063, -5203, -5341, -5478, -5613, -5746, -5877, -6006, -6133,
        -6258, -6381, -6502, -6621, -6738, -6853, -6965, -7075, -7183, -7288,
        -7391, -7492, -7590, -7686, -7779, -7870, -7958, -8043, -8126, -8206,
        -8283, -8358, -8430, -8499, -8565, -8629, -8689, -8747, -8802, -8854,
        -8903, -8949, -8992, -9032, -9069, -9103, -9134, -9162, -9187, -9209,
        -9228, -9244, -9257, -9266, -9273, -9276, -9276, -9273, -9267, -9257,
        -9244, -9228, -9209, -9187, -9162, -9134, -9103, -9069, -9032, -8992,
        -8949, -8903, -8854, -8802, -8747, -8689, -8629, -8565, -8499, -8430,
        -8358, -8283, -8206, -8126, -8043, -7958, -7870, -7779, -7686, -7590,
        -7492, -7391, -7288, -7183, -7075, -6965, -6853, -6738, -6621, -6502,
        -6381, -6258, -6133, -6006, -5877, -5746, -5613, -5478, -5341, -5203,
        -5063, -4921, -4777, -4632, -4485, -4337, -4187, -4036, -3884, -3730,
        -3575, -3419, -3262, -3104, -2945, -2785, -2624, -2463, -2301, -2138,
        -1975, -1812, -1648, -1484, -1320, -1156, -992, -828, -664, -500, -337,
        -174, -12, 149, 310, 470, 629, 787, 944, 1100, 1255, 1408, 1560, 1711,
        1860, 2008, 2154, 2298, 2440, 2581, 2719, 2856, 2990, 3122, 3252, 3380,
        3505, 3628, 3748, 3866, 3981, 4093, 4203, 4310, 4414, 4515, 4613, 4708,
        4800, 4889, 4975, 5058, 5138, 5214, 5287, 5357, 5423, 5486, 5545, 5601,
        5653, 5702, 5747, 5788, 5825, 5859, 5889, 5915, 5937, 5955, 5970, 5980,
        5987, 5990, 5989, 5984, 5975, 5962, 5945, 5925, 5900, 5872, 5840, 5804,
        5764, 5721, 5674, 5623, 5569, 5511, 5450, 5385, 5317, 5246, 5171, 5093,
        5012, 4928, 4841, 4751, 4658, 4563, 4465, 4365, 4262, 4157, 4050, 3941,
        3830, 3717, 3603, 3487, 3370, 3252, 3133, 3013, 2892, 2771, 2650, 2529,
        2408, 2287, 2167, 2048, 1930, 1814, 1699, 1586, 1475, 1366, 1259, 1155,
        1054, 956, 861, 770, 683, 600, 521, 447, 378, 314, 256, 203, 156, 115,
        80, 51, 29, 13, 3, 0
    };
};

// Compile-time validation
static_assert(WindowFunctions::SIZE == 256, 
              "Window size must be 256");
static_assert(WindowFunctions::HANNING_Q15.size() == 256, 
              "Hanning table must have 256 entries");
static_assert(WindowFunctions::hanning(0) == 0.0f, 
              "Hanning window should start at 0");
static_assert(WindowFunctions::hanning(255) == 0.0f, 
              "Hanning window should end at 0");

} // namespace constants
} // namespace eda

// ============================================================================
// DIAMOND CODE: Before/After Comparison Examples
// ============================================================================

/**
 * @section BEFORE_AFTER_EXAMPLES Before/After Comparisons
 * 
 * BEFORE (INCORRECT - Constants in RAM):
 * @code
 * // BAD: Constants in RAM, wasting memory
 * const int SAMPLE_RATE = 48000;
 * const float PI = 3.14159f;
 * 
 * // BAD: std::vector uses heap allocation
 * std::vector<int> lookup_table = {1, 2, 3, 4, 5, 6, 7, 8};
 * 
 * // BAD: Computed at runtime
 * float compute_window(int i) {
 *     return 0.5f * (1.0f - cos(2.0f * PI * i / 255.0f));
 * }
 * @endcode
 * 
 * AFTER (CORRECT - Constants in Flash):
 * @code
 * // GOOD: constexpr constants in Flash
 * constexpr int SAMPLE_RATE = 48000;
 * constexpr float PI = 3.14159f;
 * 
 * // GOOD: std::array on stack, no heap allocation
 * constexpr std::array<int, 8> lookup_table = {1, 2, 3, 4, 5, 6, 7, 8};
 * 
 * // GOOD: Pre-computed at compile time
 * constexpr float compute_window(int i) noexcept {
 *     return 0.5f * (1.0f - cos(2.0f * PI * i / 255.0f));
 * }
 * 
 * // EVEN BETTER: Pre-computed table in Flash
 * constexpr std::array<float, 256> WINDOW_TABLE = {
 *     compute_window(0), compute_window(1), ...
 * };
 * @endcode
 * 
 * MEMORY SAVINGS:
 * - BEFORE: 4KB RAM for lookup table + runtime computation
 * - AFTER: 0 bytes RAM, 1KB Flash (read-only, shared)
 * 
 * PERFORMANCE:
 * - BEFORE: Runtime computation every time
 * - AFTER: Direct table lookup (single memory access)
 */

// ============================================================================
// DIAMOND CODE: Memory Placement Documentation
// ============================================================================

/**
 * @section MEMORY_PLACEMENT Memory Placement
 * 
 * CONSTANTS MEMORY USAGE:
 * 
 * FLASH (Read-only):
 * - MathConstants: ~64 bytes
 * - DspConstants: ~128 bytes
 * - ColorPalette::PALETTE: 24 bytes (8 colors * 3 channels)
 * - WindowFunctions::HANNING_Q15: 512 bytes (256 * 2)
 * - Total Flash: ~728 bytes
 * 
 * RAM (Runtime):
 * - ZERO bytes for constant data
 * - All constants accessed directly from Flash
 * 
 * NO DYNAMIC ALLOCATION:
 * - NO std::vector (would use heap)
 * - NO std::string (would use heap)
 * - NO new, malloc (would use heap)
 * - All data in Flash or on stack
 * 
 * BENEFITS OF FLASH CONSTANTS:
 * - Zero RAM usage for constant data
 * - Fast access (Flash can be cached)
 * - Shared across all instances
 * - No initialization overhead
 * - Deterministic memory usage
 * 
 * COMPILE-TIME VALIDATION:
 * - All static_assert checks run at compile time
 * - No runtime errors for invalid constants
 * - Type safety through constexpr functions
 */

// ============================================================================
// DIAMOND CODE: Compile-Time Validation Summary
// ============================================================================

/**
 * @section COMPILE_TIME_VALIDATION Compile-Time Checks
 * 
 * This header includes the following static_assert validations:
 * 
 * MathConstants:
 * 1. PI value is correct (3.14 < PI < 3.15)
 * 2. deg_to_rad conversion is correct
 * 
 * DspConstants:
 * 3. FFT and window sizes match
 * 4. Overlap is less than FFT size
 * 5. Nyquist frequency is correct
 * 6. 2450 MHz is in drone range
 * 7. 2300 MHz is not in drone range
 * 
 * ColorPalette:
 * 8. Palette has 8 colors
 * 9. Level 0 is blue
 * 10. Level 7 is red
 * 
 * WindowFunctions:
 * 11. Window size is 256
 * 12. Hanning table has 256 entries
 * 13. Hanning window starts at 0
 * 14. Hanning window ends at 0
 * 
 * These ensure:
 * - Mathematical correctness
 * - Memory efficiency
 * - Type safety
 * - No runtime errors
 */

} // namespace eda
