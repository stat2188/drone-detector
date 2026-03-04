/**
 * @file dsp_display_utils.cpp
 * @brief Utility functions for display formatting in Enhanced Drone Analyzer
 * 
 * This file implements utility functions for formatting display data.
 * It provides functions for text formatting, bar spectrum rendering,
 * and histogram rendering calculations.
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
#include "dsp_display_types.hpp"

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>
#include <cstdio>

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"
#include "eda_optimized_utils.hpp"

namespace ui::apps::enhanced_drone_analyzer::dsp {

/**
 * @brief Helper function for minimum of two values
 */
template<typename T>
constexpr T min(T a, T b) noexcept {
    return (a < b) ? a : b;
}

/**
 * @brief Format drone display text for UI rendering
 * 
 * This function formats drone data into text suitable for display.
 * It includes type name, formatted frequency, RSSI, and trend symbol.
 * 
 * @param drone Drone entry to format
 * @return Formatted display text
 * 
 * @note This is a pure DSP/formatting function with no UI dependencies
 * @note noexcept for embedded safety
 */
DroneDisplayText format_drone_display_text(const DisplayDroneEntry& drone) noexcept {
    DroneDisplayText result;
    
    // Copy type name (manual copy to avoid strncpy dependency)
    size_t copy_len = sizeof(result.type_name) - 1;
    for (size_t i = 0; i < copy_len && drone.type_name[i] != '\0'; ++i) {
        result.type_name[i] = drone.type_name[i];
    }
    result.type_name[copy_len] = '\0';
    
    // Format frequency (DSP/Logic)
    const auto& format_entry = FREQ_FORMAT_TABLE[
        (drone.frequency >= 1000000000LL) ? 0 :
        (drone.frequency >= 1000000LL) ? 1 :
        (drone.frequency >= 1000LL) ? 2 : 3
    ];
    
    int64_t int_part = drone.frequency / format_entry.divider;
    int64_t dec_part = (format_entry.decimal_div > 1) ?
                      (drone.frequency % format_entry.divider) / format_entry.decimal_div : 0;
    
    // Manual formatting to avoid snprintf dependency
    size_t pos = 0;
    
    // Format integer part
    if (int_part == 0) {
        result.freq_string[pos++] = '0';
    } else {
        char temp[21];  // 20 digits for 64-bit int + null terminator
        size_t temp_pos = 0;
        while (int_part > 0 && temp_pos < 20) {
            temp[temp_pos++] = '0' + (int_part % 10);
            int_part /= 10;
        }
        // Reverse and copy
        while (temp_pos > 0 && pos < 14) {
            result.freq_string[pos++] = temp[--temp_pos];
        }
    }
    
    // Format decimal part if needed
    if (format_entry.decimal_div > 1 && pos < 14) {
        result.freq_string[pos++] = '.';
        
        // Format decimal part with leading zeros
        int64_t temp_dec = dec_part;
        char dec_str[8];
        size_t dec_len = 0;
        while (temp_dec > 0 && dec_len < 7) {
            dec_str[dec_len++] = '0' + (temp_dec % 10);
            temp_dec /= 10;
        }
        
        // Reverse and copy
        while (dec_len > 0 && pos < 14) {
            result.freq_string[pos++] = dec_str[--dec_len];
        }
    }
    
    // Add unit
    const char* unit = (drone.frequency >= 1000000000LL) ? "GHz" :
                      (drone.frequency >= 1000000LL) ? "MHz" :
                      (drone.frequency >= 1000LL) ? "kHz" : "Hz";
    while (*unit != '\0' && pos < 15) {
        result.freq_string[pos++] = *unit++;
    }
    result.freq_string[pos] = '\0';
    
    // Copy RSSI and trend
    result.rssi = drone.rssi;
    result.trend_symbol = TrendSymbols::from_trend(static_cast<uint8_t>(drone.trend));
    
    return result;
}

/**
 * @brief Calculate bar spectrum render data for UI rendering
 * 
 * This function calculates rendering data for a single bar in spectrum display.
 * It includes position, height, color index, and a flag indicating whether
 * the bar should be drawn.
 * 
 * @param power Power level (0-255)
 * @param x_position X position in spectrum
 * @param spectrum_height Spectrum display height
 * @param config Bar spectrum configuration
 * @return Calculated render data
 * 
 * @note This is a pure DSP/calculation function with no UI dependencies
 * @note noexcept for embedded safety
 */
BarSpectrumRenderData calculate_bar_render_data(
    uint8_t power,
    size_t x_position,
    int spectrum_height,
    const BarSpectrumConfig& config
) noexcept {
    // Suppress unused parameter warning: x_position is intentionally unused
    // This function calculates only vertical rendering data (y_top, bar_height, color_idx)
    // while the caller manages horizontal positioning (separation of concerns)
    (void)x_position;
    
    BarSpectrumRenderData result;
    
    // Noise threshold check (DSP/Logic)
    result.should_draw = (power >= config.NOISE_THRESHOLD);
    if (!result.should_draw) {
        result.y_top = 0;
        result.bar_height = 0;
        result.color_idx = 0;
        return result;
    }
    
    // Calculate bar height (DSP/Logic)
    result.bar_height = (power * spectrum_height) / 255;
    if (result.bar_height < 1) result.bar_height = 1;
    if (result.bar_height > spectrum_height) result.bar_height = spectrum_height;
    
    // Calculate color index (DSP/Logic)
    // Color based on power level: 0-4
    if (power < BarSpectrumConfig::COLOR_THRESHOLD_LOW) {
        result.color_idx = 0;
    } else if (power < BarSpectrumConfig::COLOR_THRESHOLD_MED_LOW) {
        result.color_idx = 1;
    } else if (power < BarSpectrumConfig::COLOR_THRESHOLD_MED_HIGH) {
        result.color_idx = 2;
    } else if (power < BarSpectrumConfig::COLOR_THRESHOLD_HIGH) {
        result.color_idx = 3;
    } else {
        result.color_idx = 4;
    }
    
    // Calculate Y position (DSP/Logic)
    const int bar_spectrum_y_start = config.WATERFALL_Y_START;
    result.y_top = (bar_spectrum_y_start + spectrum_height) - result.bar_height;
    
    return result;
}

/**
 * @brief Calculate histogram bin render data for UI rendering
 * 
 * This function calculates rendering data for a single bin in histogram display.
 * It includes position, dimensions, color index, and a flag indicating whether
 * the bin should be drawn.
 * 
 * @param bin_idx Bin index in histogram
 * @param bin_count Bin count (0-255)
 * @param max_count Maximum bin count (for scaling)
 * @param config Histogram color configuration
 * @return Calculated render data
 * 
 * @note This is a pure DSP/calculation function with no UI dependencies
 * @note noexcept for embedded safety
 */
HistogramBinRenderData calculate_histogram_bin_render_data(
    size_t bin_idx,
    uint8_t bin_count,
    uint8_t max_count,
    const HistogramColorConfig& config
) noexcept {
    HistogramBinRenderData result;
    
    // Skip empty bins
    result.should_draw = (bin_count > 0);
    if (!result.should_draw) {
        result.bin_x = 0;
        result.y_top = 0;
        result.bin_width = 0;
        result.bin_height = 0;
        result.color_idx = 0;
        return result;
    }
    
    // Calculate bin dimensions (DSP/Logic)
    const int scale_factor = config.HISTOGRAM_HEIGHT;
    
    // Normalize bin height against actual max_count for proper scaling
    // Guard clause: avoid division by zero if max_count is 0
    if (max_count == 0) {
        result.bin_height = 0;
    } else {
        result.bin_height = (static_cast<int>(bin_count) * scale_factor) / static_cast<int>(max_count);
    }
    if (result.bin_height < 1) result.bin_height = 1;
    if (result.bin_height > config.HISTOGRAM_HEIGHT) result.bin_height = config.HISTOGRAM_HEIGHT;
    
    result.bin_x = static_cast<int>(bin_idx * config.HISTOGRAM_BIN_WIDTH);
    result.bin_width = config.HISTOGRAM_BIN_WIDTH;
    result.y_top = (config.HISTOGRAM_Y + config.HISTOGRAM_HEIGHT) - result.bin_height;
    
    // Calculate color level (DSP/Logic)
    uint8_t color_level;
    if (bin_count <= HistogramColorConfig::COLOR_THRESHOLD_20PCT) {  // 20% threshold
        color_level = 0;
    } else if (bin_count <= HistogramColorConfig::COLOR_THRESHOLD_40PCT) {  // 40% threshold
        color_level = 1;
    } else if (bin_count <= HistogramColorConfig::COLOR_THRESHOLD_60PCT) {  // 60% threshold
        color_level = 2;
    } else if (bin_count <= HistogramColorConfig::COLOR_THRESHOLD_80PCT) {  // 80% threshold
        color_level = 3;
    } else {
        color_level = 4;
    }
    
    if (color_level >= config.NUM_COLOR_LEVELS) {
        color_level = 4;
    }
    
    result.color_idx = color_level;
    
    return result;
}

/**
 * @brief Scale histogram for display
 * 
 * This function scales histogram data from analysis buffer to display buffer.
 * It converts from uint16_t to uint8_t (0-255) for display.
 * 
 * @param analysis_histogram Analysis histogram buffer (uint16_t values)
 * @param noise_floor Noise floor from spectral analysis
 * @return Scaled histogram display buffer
 * 
 * @note This is a pure DSP/scaling function with no UI dependencies
 * @note noexcept for embedded safety
 */
HistogramDisplayBuffer scale_histogram_for_display(
    const uint16_t* analysis_histogram,
    size_t histogram_size,
    uint8_t noise_floor
) noexcept {
    HistogramDisplayBuffer result;
    result.max_count = 0;
    result.noise_floor = noise_floor;
    result.is_valid = true;
    
    // Scale from uint16_t to uint8_t (DSP/Logic)
    // Use minimum of histogram_size and 64 to prevent buffer overflow
    size_t bin_count = (histogram_size < 64) ? histogram_size : 64;
    for (size_t i = 0; i < bin_count; ++i) {
        uint16_t raw_count = analysis_histogram[i];
        result.bin_counts[i] = static_cast<uint8_t>(
            min<uint16_t>(raw_count, static_cast<uint16_t>(255))
        );
        
        if (result.bin_counts[i] > result.max_count) {
            result.max_count = result.bin_counts[i];
        }
    }
    
    // Zero-fill remaining bins if histogram_size < 64
    for (size_t i = bin_count; i < 64; ++i) {
        result.bin_counts[i] = 0;
    }
    
    return result;
}

// ============================================================================
// DRONE FILTERING AND SORTING IMPLEMENTATIONS
// ============================================================================

/**
 * @brief Filter drones by stale timeout
 * 
 * This function filters out drones that have not been seen recently.
 * It returns a filtered snapshot containing only active drones.
 * 
 * @param snapshot Input snapshot of tracked drones
 * @param stale_timeout_ms Timeout in milliseconds for stale detection
 * @param now Current timestamp in milliseconds
 * @return Filtered snapshot with only active drones
 * 
 * @note This is a pure DSP filtering function with no UI dependencies
 * @note noexcept for embedded safety
 */
FilteredDronesSnapshot filter_stale_drones(
    const FilteredDronesSnapshot& snapshot,
    systime_t stale_timeout_ms,
    systime_t now
) noexcept {
    FilteredDronesSnapshot result;
    result.count = 0;
    
    // Filter out stale drones
    for (size_t i = 0; i < snapshot.count && result.count < 10; ++i) {
        const auto& drone = snapshot.drones[i];
        
        // Check if drone is stale (not seen recently)
        if ((now - drone.last_seen) <= stale_timeout_ms) {
            // Drone is active, add to result
            result.drones[result.count] = drone;
            result.count++;
        }
    }
    
    return result;
}

/**
 * @brief Sort drones by RSSI, threat level, and last seen
 * 
 * This function sorts an array of TrackedDroneData entries by:
 * 1. RSSI (descending - stronger signals first)
 * 2. Threat level (descending - higher threats first)
 * 3. Last seen timestamp (descending - more recent first)
 * 
 * @param drones Array of drone data to sort
 * @param count Number of drones in array
 * 
 * @note This is a pure DSP sorting function with no UI dependencies
 * @note Uses insertion sort for small arrays (O(n^2) worst case, but O(n) for nearly sorted)
 * @note noexcept for embedded safety
 */
void sort_drones_by_priority(
    TrackedDroneData drones[],
    size_t count
) noexcept {
    // Guard clause: Nothing to sort
    if (count <= 1) {
        return;
    }
    
    // Use insertion sort for small arrays (typically < 10 drones)
    // Insertion sort is efficient for small datasets and nearly sorted data
    for (size_t i = 1; i < count; ++i) {
        TrackedDroneData key = drones[i];
        size_t j = i;
        
        // Shift elements that are greater than key to the right
        while (j > 0) {
            const auto& prev = drones[j - 1];
            const auto& curr = key;
            
            // Compare by RSSI (descending)
            if (prev.rssi > curr.rssi) {
                break;  // Previous is stronger, stop shifting
            }
            if (prev.rssi < curr.rssi) {
                // Current is stronger, shift previous to the right
                drones[j] = prev;
                j--;
                continue;
            }
            
            // RSSI is equal, compare by threat level (descending)
            if (prev.threat_level > curr.threat_level) {
                break;  // Previous has higher threat, stop shifting
            }
            if (prev.threat_level < curr.threat_level) {
                // Current has higher threat, shift previous to the right
                drones[j] = prev;
                j--;
                continue;
            }
            
            // Threat level is equal, compare by last seen (descending)
            if (prev.last_seen >= curr.last_seen) {
                break;  // Previous is more recent or equal, stop shifting
            }
            
            // Current is more recent, shift previous to the right
            drones[j] = prev;
            j--;
        }
        
        // Insert key at correct position
        drones[j] = key;
    }
}

} // namespace ui::apps::enhanced_drone_analyzer::dsp
