/**
 * @file dsp_spectrum_processor.cpp
 * @brief Implementation of spectrum processor for Enhanced Drone Analyzer
 * 
 * This file implements the SpectrumProcessor class which handles spectrum data
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

// Corresponding header (must be first)
#include "dsp_spectrum_processor.hpp"

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

namespace ui::apps::enhanced_drone_analyzer::dsp {

size_t SpectrumProcessor::process_mini_spectrum(
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

} // namespace ui::apps::enhanced_drone_analyzer::dsp
