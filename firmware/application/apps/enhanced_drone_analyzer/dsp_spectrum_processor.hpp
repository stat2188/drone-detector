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
     * @param power_levels Output power levels array (240 bins)
     * @param bins_hz_size Running Hz accumulator (stateful, updated by reference)
     * @param each_bin_size Hz per bin
     * @param marker_pixel_step Hz per pixel
     * @param min_color_power Minimum power to display
     * @return Updated pixel index
     *
     * @note This is a pure DSP function with no UI dependencies
     * @note noexcept for embedded safety
     * @note Implementation merged from dsp_spectrum_processor.cpp
     */
    static inline size_t process_mini_spectrum(
        const ChannelSpectrum& spectrum,
        uint8_t power_levels[240],
        size_t& bins_hz_size,
        Frequency each_bin_size,
        Frequency marker_pixel_step,
        uint8_t min_color_power
    ) noexcept {
        size_t pixel_index = 0;

        for (size_t bin = 0; bin < MINI_SPECTRUM_WIDTH; bin++) {
            uint8_t current_bin_power;
            if (bin < spectrum.db.size()) {
                current_bin_power = spectrum.db[bin];
            } else {
                current_bin_power = 0;
            }

            bins_hz_size += each_bin_size;
            if (bins_hz_size >= marker_pixel_step) {
                // Bounds check BEFORE array access to prevent buffer overflow
                if (pixel_index < 240) {
                    power_levels[pixel_index] = (current_bin_power > min_color_power) ?
                                                current_bin_power : 0;
                }
                current_bin_power = 0;

                bins_hz_size -= marker_pixel_step;
                pixel_index++;
            }
        }

        return pixel_index;
    }
    
private:
    static constexpr size_t MINI_SPECTRUM_WIDTH = 240;
};

} // namespace ui::apps::enhanced_drone_analyzer::dsp

#endif // DSP_SPECTRUM_PROCESSOR_HPP_
