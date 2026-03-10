/**
 * @file dsp_spectrum_processor.hpp
 * @brief DSP layer for spectrum processing in Enhanced Drone Analyzer
 * 
 * This file defines the SpectrumProcessor class which handles spectrum data
 * processing in the DSP layer. It provides pure signal processing
 * functions without any UI dependencies.
 * 
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All memory is stack-allocated
 * - No exceptions: All functions are noexcept
 * - Type-safe: Uses semantic type aliases
 * - Memory-safe: Stack-only allocation
 * 
 * @author Diamond Code Pipeline - Stage 5
 * @date 2026-03-03
 */

#ifndef DSP_SPECTRUM_PROCESSOR_HPP_
#define DSP_SPECTRUM_PROCESSOR_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"
#include "message.hpp"

namespace ui::apps::enhanced_drone_analyzer::dsp {

// Use int64_t as Frequency type (consistent with rf::Frequency in rf_path.hpp)
using Frequency = int64_t;

// ============================================================================
// PHASE 3 FIX #8: Named constants for magic numbers
// ============================================================================

/**
 * @brief Named constants for spectrum processor
 *
 * PHASE 3 FIX #8: Define POWER_LEVELS_COUNT = 240
 * - Replaces magic number 240 with named constant
 * - Power levels array size for spectrum display
 * - Matches screen width in pixels
 */
namespace SpectrumProcessorConstants {
    /**
     * @brief Number of power levels in spectrum display
     * @note Matches screen width (240 pixels)
     * @note Replaces magic number 240
     */
    static constexpr size_t POWER_LEVELS_COUNT = 240;
}

// ============================================================================
// CRITICAL FIX #E004: STRONGLY-TYPED WRAPPER CLASSES
// ============================================================================
/**
 * @brief Strongly-typed wrapper for frequency bin size parameter
 *
 * DIAMOND FIX #E004: Prevents accidental parameter swapping
 * - BinSize is distinct type that cannot be confused with other Frequency parameters
 * - Cannot be implicitly converted to Frequency
 * - Compile-time error if swapped in process_mini_spectrum() function
 * - Zero runtime overhead (constexpr, inline)
 *
 * USAGE:
 * @code
 *   // CORRECT: Types prevent swapping
 *   process_mini_spectrum(
 *       spectrum,
 *       power_levels,
 *       bins_hz_size,
 *       BinSize(100),     // Bin size
 *       Frequency(1000000), // Frequency for each bin
 *       Frequency(100),       // Marker pixel step
 *       MinColorPower(5)     // Min color power
 *   );
 *
 *   // WRONG: Compiler error - types don't match!
 *   // process_mini_spectrum(
 *   //     spectrum,
 *   //     power_levels,
 *   //     bins_hz_size,
 *   //     Frequency(1000000), // Wrong type!
 *   //     Frequency(100),       // Wrong type!
 *   //     BinSize(100),       // Wrong type!
 *   //     Frequency(100),       // Wrong type!
 *   //     MinColorPower(5)     // Wrong type!
 *   // );
 * @endcode
 *
 * @note Follows Scott Meyers' principle: "Make interfaces hard to use incorrectly"
 * @note Zero runtime overhead: constexpr and inline optimization
 * @note Type-safe: Cannot be confused with other Frequency parameters
 */
class BinSize {
public:
    /**
     * @brief Construct BinSize from size_t value
     * @param value Size of each frequency bin
     * @note constexpr enables compile-time evaluation
     * @note explicit prevents implicit conversion from size_t
     */
    explicit constexpr BinSize(size_t value) noexcept : value_(value) {}

    /**
     * @brief Get the underlying size_t value
     * @return Size of each frequency bin
     * @note constexpr enables compile-time evaluation
     */
    [[nodiscard]] constexpr size_t get() const noexcept { return value_; }

private:
    size_t value_;  ///< Underlying size_t value
};

/**
 * @brief Strongly-typed wrapper for minimum color power
 *
 * DIAMOND FIX #E004: Prevents accidental parameter swapping
 * - MinColorPower is distinct type that cannot be confused with other parameters
 * - Cannot be implicitly converted to uint8_t
 * - Compile-time error if swapped in process_mini_spectrum() function
 * - Zero runtime overhead (constexpr, inline)
 *
 * USAGE:
 * @code
 *   // CORRECT: Types prevent swapping
 *   process_mini_spectrum(
 *       spectrum,
 *       power_levels,
 *       bins_hz_size,
 *       BinSize(100),     // Bin size
 *       Frequency(1000000), // Frequency for each bin
 *       Frequency(100),       // Marker pixel step
 *       MinColorPower(5)     // Min color power
 *   );
 *
 *   // WRONG: Compiler error - types don't match!
 *   // process_mini_spectrum(
 *   //     spectrum,
 *   //     power_levels,
 *   //     bins_hz_size,
 *   //     Frequency(1000000), // Wrong type!
 *   //     Frequency(100),       // Wrong type!
 *   //     BinSize(100),       // Wrong type!
 *   //     Frequency(100),       // Wrong type!
 *   //     MinColorPower(5)     // Wrong type!
 *   // );
 * @endcode
 *
 * @note Follows Scott Meyers' principle: "Make interfaces hard to use incorrectly"
 * @note Zero runtime overhead: constexpr and inline optimization
 * @note Type-safe: Cannot be confused with other parameters
 */
class MinColorPower {
public:
    /**
     * @brief Construct MinColorPower from uint8_t value
     * @param value Minimum color power level
     * @note constexpr enables compile-time evaluation
     * @note explicit prevents implicit conversion from uint8_t
     */
    explicit constexpr MinColorPower(uint8_t value) noexcept : value_(value) {}

    /**
     * @brief Get the underlying uint8_t value
     * @return Minimum color power level
     * @note constexpr enables compile-time evaluation
     */
    [[nodiscard]] constexpr uint8_t get() const noexcept { return value_; }

private:
    uint8_t value_;  ///< Underlying uint8_t value
};

/**
 * @brief Spectrum data processor (DSP layer)
 * 
 * This class provides static methods for processing spectrum data
 * in the DSP layer. It has no UI dependencies and performs
 * pure signal processing operations.
 * 
 * Thread-safety: All methods are static and stateless, making them
 * inherently thread-safe.
 */
class SpectrumProcessor {
public:
    /**
     * @brief Process mini spectrum data into display bins
     *
     * This method processes raw spectrum data and converts it into
     * display bins suitable for UI rendering. It handles frequency
     * scaling and power level filtering.
     *
     * @param spectrum Input spectrum data
     * @param power_levels Output power levels array (POWER_LEVELS_COUNT bins)
     * @param bins_hz_size Running Hz accumulator (stateful, updated by reference)
     * @param each_bin_size Hz per bin
     * @param marker_pixel_step Hz per pixel
     * @param min_color_power Minimum power to display
     * @return Updated pixel index
     *
     * @note This is a pure DSP function with no UI dependencies
     * @note noexcept for embedded safety
     * @note Implementation merged from dsp_spectrum_processor.cpp
     * @note PHASE 3 FIX #8: Replaced magic number 240 with POWER_LEVELS_COUNT
     */
    // CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
    static inline size_t process_mini_spectrum(
        const ChannelSpectrum& spectrum,
        uint8_t power_levels[SpectrumProcessorConstants::POWER_LEVELS_COUNT],
        size_t& bins_hz_size,
        BinSize each_bin_size,
        Frequency marker_pixel_step,
        MinColorPower min_color_power
    ) noexcept {
        size_t pixel_index = 0;

        for (size_t bin = 0; bin < EDA::Constants::MINI_SPECTRUM_WIDTH; bin++) {
            uint8_t current_bin_power;
            if (bin < spectrum.db.size()) {
                current_bin_power = spectrum.db[bin];
            } else {
                current_bin_power = 0;
            }

            bins_hz_size += each_bin_size.get();
            if (bins_hz_size >= marker_pixel_step) {
                // Bounds check BEFORE array access to prevent buffer overflow
                // PHASE 3 FIX #8: Replaced magic number 240 with POWER_LEVELS_COUNT
                if (pixel_index < SpectrumProcessorConstants::POWER_LEVELS_COUNT) {
                    power_levels[pixel_index] = (current_bin_power > min_color_power.get()) ?
                                                current_bin_power : 0;
                }
                current_bin_power = 0;

                bins_hz_size -= marker_pixel_step;
                pixel_index++;
            }
        }

        return pixel_index;
    }
};

} // namespace ui::apps::enhanced_drone_analyzer::dsp

#endif // DSP_SPECTRUM_PROCESSOR_HPP_
