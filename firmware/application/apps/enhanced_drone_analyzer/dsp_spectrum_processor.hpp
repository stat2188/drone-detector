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
     */
    static size_t process_mini_spectrum(
        const ChannelSpectrum& spectrum,
        uint8_t power_levels[240],
        size_t& bins_hz_size,
        Frequency each_bin_size,
        Frequency marker_pixel_step,
        uint8_t min_color_power
    ) noexcept;
    
private:
    static constexpr size_t MINI_SPECTRUM_WIDTH = 240;
};

} // namespace ui::apps::enhanced_drone_analyzer::dsp

#endif // DSP_SPECTRUM_PROCESSOR_HPP_
