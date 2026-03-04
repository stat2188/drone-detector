/**
 * @file dsp_display_types.hpp
 * @brief Data structures for DSP/UI communication in Enhanced Drone Analyzer
 * 
 * This file defines all data structures used for communication between
 * the DSP layer (signal processing) and UI layer (presentation).
 * 
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All structures use fixed-size arrays
 * - No exceptions: All functions are noexcept
 * - Type-safe: Uses semantic type aliases
 * - Memory-safe: Stack-only allocation
 * 
 * @author Diamond Code Pipeline - Stage 5
 * @date 2026-03-03
 */

#ifndef DSP_DISPLAY_TYPES_HPP_
#define DSP_DISPLAY_TYPES_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"
#include "eda_optimized_utils.hpp"
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer::dsp {

// Use int64_t as Frequency type (consistent with rf::Frequency in rf_path.hpp)
using Frequency = int64_t;

/**
 * @brief Display drone entry for UI rendering
 * 
 * This structure contains drone data that has been prepared for UI display.
 * It includes frequency, type name, RSSI, and trend information.
 */
struct DisplayDroneEntry {
    char type_name[16];            ///< Drone type name (e.g., "MAVIC")
    Frequency frequency;            ///< Drone frequency in Hz
    int32_t rssi;                 ///< RSSI in dBm
    MovementTrend trend;           ///< Movement trend
};

/**
 * @brief Display data snapshot for UI rendering
 * 
 * This structure provides a snapshot of all display data from the DSP layer
 * to the UI layer. It includes scanning state, threat levels, drone counts,
 * and pre-calculated color indices.
 * 
 * Thread-safety: Snapshot should be captured under mutex and then
 * used without further synchronization.
 */
struct DisplayDataSnapshot {
    bool is_scanning;              ///< Scanning active flag
    Frequency current_freq;          ///< Current scanning frequency
    size_t total_freqs;            ///< Total frequencies in database
    ThreatLevel max_threat;         ///< Maximum detected threat level
    size_t approaching_count;       ///< Number of approaching drones
    size_t receding_count;         ///< Number of receding drones
    size_t static_count;           ///< Number of static drones
    size_t total_detections;        ///< Total detection count
    bool is_real_mode;              ///< Real mode (vs demo mode)
    uint32_t scan_cycles;           ///< Number of scan cycles completed
    bool has_detections;            ///< Has any detections
    uint8_t color_idx;             ///< Pre-calculated color index
    size_t current_freq_idx;         ///< Current frequency index in database (for progress calculation)
};

/**
 * @brief Tracked drone data for UI display
 * 
 * This structure contains drone data that has been filtered and
 * prepared for UI display. It includes frequency, type, threat level,
 * RSSI, and trend information.
 */
struct TrackedDroneData {
    Frequency frequency;            ///< Drone frequency in Hz
    uint8_t drone_type;            ///< Drone type (DroneType enum)
    uint8_t threat_level;          ///< Threat level (ThreatLevel enum)
    int32_t rssi;                 ///< RSSI in dBm
    systime_t last_seen;           ///< Last seen timestamp
    MovementTrend trend;           ///< Movement trend
};

/**
 * @brief Filtered drones snapshot for UI rendering
 * 
 * This structure provides a snapshot of filtered drone data from the DSP layer
 * to the UI layer. It includes only active drones that have been seen recently.
 * 
 * Thread-safety: Snapshot should be captured under mutex and then
 * used without further synchronization.
 */
struct FilteredDronesSnapshot {
    size_t count;                                          ///< Number of drones in snapshot
    TrackedDroneData drones[10];                           ///< Filtered drone data (max 10)
};

/**
 * @brief Drone display text for UI rendering
 * 
 * This structure contains pre-formatted text data for rendering a drone entry.
 * It includes the type name, formatted frequency string, RSSI, and trend symbol.
 * 
 * This structure is used to separate text formatting (DSP logic) from
 * UI rendering (presentation logic).
 */
struct DroneDisplayText {
    char type_name[16];            ///< Drone type name (e.g., "MAVIC")
    char freq_string[16];          ///< Formatted frequency string (e.g., "2.400GHz")
    int32_t rssi;                 ///< RSSI in dBm
    char trend_symbol;             ///< Trend symbol ('=', '^', 'v', '~')
};

/**
 * @brief Bar spectrum render data for UI rendering
 * 
 * This structure contains pre-calculated rendering data for a single bar
 * in the spectrum display. It includes position, height, color index,
 * and a flag indicating whether the bar should be drawn.
 * 
 * This structure is used to separate signal analysis (DSP logic) from
 * UI rendering (presentation logic).
 */
struct BarSpectrumRenderData {
    int y_top;                    ///< Top Y position for drawing
    int bar_height;                ///< Bar height in pixels
    size_t color_idx;              ///< Color index for bar
    bool should_draw;              ///< Whether to draw this bar (false if below noise threshold)
};

/**
 * @brief Histogram display buffer for UI rendering
 * 
 * This structure contains scaled histogram data ready for display.
 * It includes bin counts, maximum count, noise floor, and validity flag.
 * 
 * This structure is used to separate data scaling (DSP logic) from
 * UI rendering (presentation logic).
 */
struct HistogramDisplayBuffer {
    uint8_t bin_counts[64];       ///< Histogram bin counts (0-255)
    uint8_t max_count;            ///< Maximum bin count (for scaling)
    uint8_t noise_floor;          ///< Noise floor from spectral analysis
    bool is_valid;                ///< Buffer contains valid data
};

/**
 * @brief Histogram bin render data for UI rendering
 * 
 * This structure contains pre-calculated rendering data for a single bin
 * in the histogram display. It includes position, dimensions, color index,
 * and a flag indicating whether the bin should be drawn.
 * 
 * This structure is used to separate histogram calculations (DSP logic) from
 * UI rendering (presentation logic).
 */
struct HistogramBinRenderData {
    int bin_x;                     ///< X position for drawing
    int y_top;                     ///< Top Y position for drawing
    int bin_width;                 ///< Bin width in pixels
    int bin_height;                ///< Bin height in pixels
    size_t color_idx;              ///< Color index for bin
    bool should_draw;              ///< Whether to draw this bin (false if empty)
};

/**
 * @brief Frequency format entry for frequency formatting
 * 
 * This structure contains formatting parameters for displaying frequencies
 * at different scales (GHz, MHz, kHz, Hz).
 */
struct FreqFormatEntry {
    int64_t divider;               ///< Divider to get integer part
    int64_t decimal_div;           ///< Divider to get decimal part
    const char* format;             ///< Format string for snprintf
};

/**
 * @brief Frequency format table for different frequency ranges
 * 
 * This table provides formatting parameters for displaying frequencies
 * at different scales. The index is determined by the frequency magnitude.
 * 
 * Note: inline constexpr to avoid ODR violations when header is included
 * in multiple translation units.
 */
inline constexpr FreqFormatEntry FREQ_FORMAT_TABLE[] = {
    {1000000000LL, 100000000LL, "%lu.%03lu GHz"},  // >= 1 GHz
    {1000000LL, 100000LL, "%lu.%03lu MHz"},        // >= 1 MHz
    {1000LL, 100LL, "%lu.%02lu kHz"},            // >= 1 kHz
    {1LL, 1, "%lu Hz"}                           // < 1 kHz
};

/**
 * @brief Bar spectrum configuration constants
 * 
 * This structure contains configuration constants for bar spectrum rendering.
 */
struct BarSpectrumConfig {
    static constexpr int WATERFALL_Y_START = 120;  ///< Y position of spectrum display
    static constexpr int BAR_HEIGHT_MAX = 40;        ///< Maximum bar height
    static constexpr uint8_t NOISE_THRESHOLD = 10;   ///< Minimum power to display

    // Color level thresholds (0-255 scale)
    static constexpr uint8_t COLOR_THRESHOLD_LOW = 50;       ///< Low power threshold (20%)
    static constexpr uint8_t COLOR_THRESHOLD_MED_LOW = 100;  ///< Medium-low power threshold (40%)
    static constexpr uint8_t COLOR_THRESHOLD_MED_HIGH = 150; ///< Medium-high power threshold (60%)
    static constexpr uint8_t COLOR_THRESHOLD_HIGH = 200;  ///< High power threshold (80%)
    
    /// Bar colors for different power levels (5 levels)
    static constexpr uint32_t BAR_COLORS[] = {
        0x001F,  // Blue (low)
        0x07E0,  // Green (medium-low)
        0xFFE0,  // Yellow (medium)
        0xF800,  // Red (high)
        0xF81F   // Magenta (critical)
    };
};

/**
 * @brief Histogram color configuration constants
 * 
 * This structure contains configuration constants for histogram rendering.
 */
struct HistogramColorConfig {
    static constexpr int HISTOGRAM_Y = 164;          ///< Y position of histogram
    static constexpr int HISTOGRAM_HEIGHT = 26;      ///< Histogram height
    static constexpr int HISTOGRAM_WIDTH = 240;      ///< Histogram width
    static constexpr int HISTOGRAM_NUM_BINS = 64;    ///< Number of histogram bins
    static constexpr int HISTOGRAM_BIN_WIDTH = HISTOGRAM_WIDTH / HISTOGRAM_NUM_BINS;  // 3.75px/bin

    static constexpr size_t NUM_COLOR_LEVELS = 5;     ///< Number of color levels

    // Color level thresholds (0-255 scale)
    static constexpr uint8_t COLOR_THRESHOLD_20PCT = 51;   ///< 20% threshold
    static constexpr uint8_t COLOR_THRESHOLD_40PCT = 102;  ///< 40% threshold
    static constexpr uint8_t COLOR_THRESHOLD_60PCT = 153;  ///< 60% threshold
    static constexpr uint8_t COLOR_THRESHOLD_80PCT = 204;  ///< 80% threshold

    /// Histogram colors for different signal levels (5 levels)
    static constexpr uint32_t HISTOGRAM_COLORS[] = {
        0x001F,  // Blue (20%)
        0x07E0,  // Green (40%)
        0xFFE0,  // Yellow (60%)
        0xF800,  // Red (80%)
        0xF81F   // Magenta (100%)
    };
};

// Compile-time size validation
static_assert(sizeof(DisplayDataSnapshot) <= 64, "DisplayDataSnapshot exceeds 64 bytes");
static_assert(sizeof(FilteredDronesSnapshot) <= 640, "FilteredDronesSnapshot exceeds 640 bytes");
static_assert(sizeof(DroneDisplayText) <= 48, "DroneDisplayText exceeds 48 bytes");
static_assert(sizeof(BarSpectrumRenderData) <= 24, "BarSpectrumRenderData exceeds 24 bytes");
static_assert(sizeof(HistogramDisplayBuffer) <= 67, "HistogramDisplayBuffer exceeds 67 bytes");
static_assert(sizeof(HistogramBinRenderData) <= 32, "HistogramBinRenderData exceeds 32 bytes");

// Forward declarations for utility functions (implemented in dsp_display_utils.cpp)
DroneDisplayText format_drone_display_text(const DisplayDroneEntry& drone) noexcept;
BarSpectrumRenderData calculate_bar_render_data(
    uint8_t power,
    size_t x_position,
    int spectrum_height,
    const BarSpectrumConfig& config
) noexcept;
HistogramBinRenderData calculate_histogram_bin_render_data(
    size_t bin_idx,
    uint8_t bin_count,
    uint8_t max_count,
    const HistogramColorConfig& config
) noexcept;
HistogramDisplayBuffer scale_histogram_for_display(
    const uint16_t* analysis_histogram,
    size_t histogram_size,
    uint8_t noise_floor
) noexcept;

} // namespace ui::apps::enhanced_drone_analyzer::dsp

#endif // DSP_DISPLAY_TYPES_HPP_
