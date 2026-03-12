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

// Third-party library headers
#include <ch.h>

// ChibiOS headers are included through <ch.h>
// No need to include internal headers directly

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"  // For EDA_FLASH_CONST macro
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer::dsp {

// Use int64_t as Frequency type (consistent with rf::Frequency in rf_path.hpp)
using Frequency = int64_t;

// PHASE 4 FIX #13: Define RSSIValue type alias for consistency
// Matches ui_signal_processing.hpp: using RSSIValue = int32_t;
using RSSIValue = int32_t;

// ============================================================================
// PHASE 3 FIX #9: Named constants for magic numbers
// ============================================================================

/**
 * @brief Named constants for display types
 *
 * PHASE 3 FIX #9: Define MAX_FILTERED_DRONES = 10
 * - Replaces magic number 10 with named constant
 * - Maximum number of drones to display in filtered list
 * - Limits UI rendering complexity and stack usage
 */
namespace DisplayTypeConstants {
    /**
     * @brief Maximum number of filtered drones to display
     * @note Replaces magic number 10 in FilteredDronesSnapshot
     * @note Limits UI rendering complexity and stack usage
     */
    static constexpr size_t MAX_FILTERED_DRONES = 10;

    /**
     * @brief Maximum length for drone type name
     * @note Replaces magic number 16 in DisplayDroneEntry
     */
    static constexpr size_t MAX_DRONE_TYPE_NAME_LENGTH = 16;

    /**
     * @brief Maximum length for frequency string
     * @note Replaces magic number 16 in DroneDisplayText
     */
    static constexpr size_t MAX_FREQ_STRING_LENGTH = 16;

    /**
     * @brief Number of color levels for threat display
     * @note Replaces magic number 5 in color arrays
     */
    static constexpr size_t NUM_COLOR_LEVELS = 5;
}

/**
 * @brief Display drone entry for UI rendering
 *
 * This structure contains drone data that has been prepared for UI display.
 * It includes frequency, type name, RSSI, and trend information.
 *
 * PHASE 3 FIX #9: Replaced magic number 16 with MAX_DRONE_TYPE_NAME_LENGTH
 * PHASE 4 FIX #13: Use RSSIValue consistently instead of int32_t
 */
struct DisplayDroneEntry {
    char type_name[DisplayTypeConstants::MAX_DRONE_TYPE_NAME_LENGTH];  ///< Drone type name (e.g., "MAVIC")
    Frequency frequency;            ///< Drone frequency in Hz
    RSSIValue rssi;               ///< RSSI in dBm (PHASE 4 FIX #13: Use RSSIValue)
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
 *
 * MEMORY ALLOCATION:
 * - Stack-allocated only (no heap allocation)
 * - Fixed-size structure (no dynamic memory)
 * - Safe for embedded use
 *
 * USAGE EXAMPLE:
 * @code
 *   // Create snapshot on stack
 *   DisplayDataSnapshot snapshot{};
 *   // Use snapshot...
 *   snapshot.is_scanning = true;
 * @endcode
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
 *
 * PHASE 4 FIX #13: Use RSSIValue consistently instead of int32_t
 * PHASE 4 FIX #12: Use systime_t consistently for timestamps
 */
struct TrackedDroneData {
    Frequency frequency;            ///< Drone frequency in Hz
    uint8_t drone_type;            ///< Drone type (DroneType enum)
    uint8_t threat_level;          ///< Threat level (ThreatLevel enum)
    RSSIValue rssi;               ///< RSSI in dBm (PHASE 4 FIX #13: Use RSSIValue)
    systime_t last_seen;           ///< Last seen timestamp (PHASE 4 FIX #12: Use systime_t)
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
 *
 * MEMORY ALLOCATION:
 * - Stack-allocated only (no heap allocation)
 * - Fixed-size structure with fixed-size array
 * - Safe for embedded use
 *
 * PHASE 3 FIX #9: Replaced magic number 10 with MAX_FILTERED_DRONES
 *
 * USAGE EXAMPLE:
 * @code
 *   // Create snapshot on stack
 *   FilteredDronesSnapshot snapshot{};
 *   // Use snapshot...
 *   snapshot.count = 5;
 * @endcode
 */
struct FilteredDronesSnapshot {
    size_t count;                                          ///< Number of drones in snapshot
    TrackedDroneData drones[DisplayTypeConstants::MAX_FILTERED_DRONES];  ///< Filtered drone data (max MAX_FILTERED_DRONES)

};

// ============================================================================
// CRITICAL FIX #E004: STRONGLY-TYPED WRAPPER CLASSES
// ============================================================================
/**
 * @brief Strongly-typed wrapper for stale timeout value
 *
 * DIAMOND FIX #E004: Prevents accidental parameter swapping
 * - StaleTimeout and CurrentTime are distinct types
 * - Cannot be implicitly converted to systime_t
 * - Compile-time error if swapped in filter_stale_drones() function
 * - Zero runtime overhead (constexpr, inline)
 *
 * USAGE:
 * @code
 *   // CORRECT: Types prevent swapping
 *   filter_stale_drones(
 *       snapshot,
 *       StaleTimeout(5000),   // Timeout value
 *       CurrentTime(chTimeNow())  // Current time
 *   );
 *
 *   // WRONG: Compiler error - types don't match!
 *   // filter_stale_drones(
 *   //     snapshot,
 *   //     CurrentTime(chTimeNow()),  // Wrong type!
 *   //     StaleTimeout(5000)       // Wrong type!
 *   // );
 * @endcode
 *
 * @note Follows Scott Meyers' principle: "Make interfaces hard to use incorrectly"
 * @note Zero runtime overhead: constexpr and inline optimization
 * @note Type-safe: Cannot be confused with CurrentTime
 */
class StaleTimeout {
public:
    /**
     * @brief Construct StaleTimeout from systime_t value
     * @param value Timeout value in milliseconds
     * @note constexpr enables compile-time evaluation
     * @note explicit prevents implicit conversion from systime_t
     */
    explicit constexpr StaleTimeout(systime_t value) noexcept : value_(value) {}

    /**
     * @brief Get the underlying systime_t value
     * @return Timeout value in milliseconds
     * @note constexpr enables compile-time evaluation
     */
    [[nodiscard]] constexpr systime_t get() const noexcept { return value_; }

private:
    systime_t value_;  ///< Underlying systime_t value
};

/**
 * @brief Strongly-typed wrapper for current time value
 *
 * DIAMOND FIX #E004: Prevents accidental parameter swapping
 * - CurrentTime and StaleTimeout are distinct types
 * - Cannot be implicitly converted to systime_t
 * - Compile-time error if swapped in filter_stale_drones() function
 * - Zero runtime overhead (constexpr, inline)
 *
 * USAGE:
 * @code
 *   // CORRECT: Types prevent swapping
 *   filter_stale_drones(
 *       snapshot,
 *       StaleTimeout(5000),   // Timeout value
 *       CurrentTime(chTimeNow())  // Current time
 *   );
 *
 *   // WRONG: Compiler error - types don't match!
 *   // filter_stale_drones(
 *   //     snapshot,
 *   //     StaleTimeout(5000),       // Wrong type!
 *   //     CurrentTime(chTimeNow())  // Wrong type!
 *   // );
 * @endcode
 *
 * @note Follows Scott Meyers' principle: "Make interfaces hard to use incorrectly"
 * @note Zero runtime overhead: constexpr and inline optimization
 * @note Type-safe: Cannot be confused with StaleTimeout
 */
class CurrentTime {
public:
    /**
     * @brief Construct CurrentTime from systime_t value
     * @param value Current time value
     * @note constexpr enables compile-time evaluation
     * @note explicit prevents implicit conversion from systime_t
     */
    explicit constexpr CurrentTime(systime_t value) noexcept : value_(value) {}

    /**
     * @brief Get the underlying systime_t value
     * @return Current time value
     * @note constexpr enables compile-time evaluation
     */
    [[nodiscard]] constexpr systime_t get() const noexcept { return value_; }

private:
    systime_t value_;  ///< Underlying systime_t value
};

/**
 * @brief Drone display text for UI rendering
 *
 * This structure contains pre-formatted text data for rendering a drone entry.
 * It includes the type name, formatted frequency string, RSSI, and trend symbol.
 *
 * This structure is used to separate text formatting (DSP logic) from
 * UI rendering (presentation logic).
 *
 * PHASE 3 FIX #9: Replaced magic numbers 16 with MAX_DRONE_TYPE_NAME_LENGTH and MAX_FREQ_STRING_LENGTH
 */
struct DroneDisplayText {
    char type_name[DisplayTypeConstants::MAX_DRONE_TYPE_NAME_LENGTH];  ///< Drone type name (e.g., "MAVIC")
    char freq_string[DisplayTypeConstants::MAX_FREQ_STRING_LENGTH];  ///< Formatted frequency string (e.g., "2.400GHz")
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
 * PHASE 5 FIX #16: Move to Flash using EDA_FLASH_CONST constexpr (~64 bytes)
 * Note: inline constexpr to avoid ODR violations when header is included
 * in multiple translation units.
 */
EDA_FLASH_CONST inline static constexpr FreqFormatEntry FREQ_FORMAT_TABLE[] = {
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
    static constexpr int SPECTRUM_Y_START = 120;  ///< Y position of spectrum display
    static constexpr int BAR_HEIGHT_MAX = 40;        ///< Maximum bar height
    static constexpr uint8_t NOISE_THRESHOLD = 10;   ///< Minimum power to display

    // Color level thresholds (0-255 scale)
    static constexpr uint8_t COLOR_THRESHOLD_LOW = 50;       ///< Low power threshold (20%)
    static constexpr uint8_t COLOR_THRESHOLD_MED_LOW = 100;  ///< Medium-low power threshold (40%)
    static constexpr uint8_t COLOR_THRESHOLD_MED_HIGH = 150; ///< Medium-high power threshold (60%)
    static constexpr uint8_t COLOR_THRESHOLD_HIGH = 200;  ///< High power threshold (80%)
    
    /// Bar colors for different power levels (NUM_COLOR_LEVELS levels)
    /// PHASE 3 FIX #9: Replaced magic number 5 with NUM_COLOR_LEVELS
    /// PHASE 5 FIX #15: Move to Flash using EDA_FLASH_CONST constexpr (~20 bytes)
    EDA_FLASH_CONST static constexpr uint32_t BAR_COLORS[DisplayTypeConstants::NUM_COLOR_LEVELS] = {
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

    /// PHASE 3 FIX #9: Moved NUM_COLOR_LEVELS to DisplayTypeConstants namespace
    /// This constant is now defined in DisplayTypeConstants namespace
    static constexpr size_t NUM_COLOR_LEVELS = DisplayTypeConstants::NUM_COLOR_LEVELS;

    // Color level thresholds (0-255 scale)
    static constexpr uint8_t COLOR_THRESHOLD_20PCT = 51;   ///< 20% threshold
    static constexpr uint8_t COLOR_THRESHOLD_40PCT = 102;  ///< 40% threshold
    static constexpr uint8_t COLOR_THRESHOLD_60PCT = 153;  ///< 60% threshold
    static constexpr uint8_t COLOR_THRESHOLD_80PCT = 204;  ///< 80% threshold

    /// Histogram colors for different signal levels (NUM_COLOR_LEVELS levels)
    /// PHASE 3 FIX #9: Replaced magic number 5 with NUM_COLOR_LEVELS
    /// PHASE 5 FIX #15: Move to Flash using EDA_FLASH_CONST constexpr (~20 bytes)
    EDA_FLASH_CONST static constexpr uint32_t HISTOGRAM_COLORS[DisplayTypeConstants::NUM_COLOR_LEVELS] = {
        0x001F,  // Blue (20%)
        0x07E0,  // Green (40%)
        0xFFE0,  // Yellow (60%)
        0xF800,  // Red (80%)
        0xF81F   // Magenta (100%)
    };
};

// ============================================================================
// PARAMETER STRUCTS FOR DSP FUNCTIONS (Prevent Swappable Parameters)
// ============================================================================
// P1-HIGH FIX: Create parameter structs to prevent parameter swapping issues
// These structs group convertible types (uint8_t, size_t) to prevent accidental swapping

/**
 * @brief Parameters for calculate_bar_render_data function
 * 
 * Groups power and x_position parameters to prevent accidental swapping.
 * Both parameters are convertible types (uint8_t and size_t), so grouping
 * them in a struct prevents the compiler from warning about potential swaps.
 * 
 * DIAMOND CODE PRINCIPLES:
 * - Type-safe: Uses semantic type aliases
 * - Stack-only: No heap allocation
 * - noexcept: All functions are noexcept
 */
struct BarRenderParams {
    uint8_t power;       ///< Power level (0-255)
    size_t x_position;   ///< X position in spectrum
};

/**
 * @brief Parameters for calculate_histogram_bin_render_data function
 * 
 * Groups bin_idx and bin_count parameters to prevent accidental swapping.
 * Both parameters are convertible types (size_t and uint8_t), so grouping
 * them in a struct prevents the compiler from warning about potential swaps.
 * 
 * DIAMOND CODE PRINCIPLES:
 * - Type-safe: Uses semantic type aliases
 * - Stack-only: No heap allocation
 * - noexcept: All functions are noexcept
 */
struct HistogramBinRenderParams {
    size_t bin_idx;      ///< Bin index in histogram
    uint8_t bin_count;   ///< Bin count (0-255)
};

/**
 * @brief Parameters for scale_histogram_for_display function
 * 
 * Groups histogram_size and noise_floor parameters to prevent accidental swapping.
 * Both parameters are convertible types (size_t and uint8_t), so grouping
 * them in a struct prevents the compiler from warning about potential swaps.
 * 
 * DIAMOND CODE PRINCIPLES:
 * - Type-safe: Uses semantic type aliases
 * - Stack-only: No heap allocation
 * - noexcept: All functions are noexcept
 */
struct HistogramScaleParams {
    size_t histogram_size;  ///< Size of histogram buffer
    uint8_t noise_floor;    ///< Noise floor from spectral analysis
};

// Compile-time size validation
// PHASE 3 FIX #9: Updated static_assert comments to reference named constants
static_assert(sizeof(DisplayDataSnapshot) <= 64, "DisplayDataSnapshot exceeds 64 bytes");
static_assert(sizeof(FilteredDronesSnapshot) <= 640, "FilteredDronesSnapshot exceeds 640 bytes (MAX_FILTERED_DRONES * 64 bytes per drone)");
static_assert(sizeof(DroneDisplayText) <= 48, "DroneDisplayText exceeds 48 bytes");
static_assert(sizeof(BarSpectrumRenderData) <= 24, "BarSpectrumRenderData exceeds 24 bytes");
static_assert(sizeof(HistogramDisplayBuffer) <= 67, "HistogramDisplayBuffer exceeds 67 bytes");
static_assert(sizeof(HistogramBinRenderData) <= 32, "HistogramBinRenderData exceeds 32 bytes");
static_assert(sizeof(BarRenderParams) <= 16, "BarRenderParams exceeds 16 bytes");
static_assert(sizeof(HistogramBinRenderParams) <= 16, "HistogramBinRenderParams exceeds 16 bytes");
static_assert(sizeof(HistogramScaleParams) <= 16, "HistogramScaleParams exceeds 16 bytes");

// ============================================================================
// DIAMOND FIX: LOCK-FREE RENDER CACHE (DOUBLE BUFFERING)
// ============================================================================
namespace RenderCache {
    struct BarSpectrumCache {
        BarSpectrumRenderData render_data[240];
        bool valid;
    };

    struct HistogramCache {
        HistogramBinRenderData render_data[64];
        bool valid;
    };

    constexpr uint8_t FRONT = 0;
    constexpr uint8_t BACK = 1;
}

// Forward declarations for utility functions (implemented in dsp_display_utils.cpp)
DroneDisplayText format_drone_display_text(const DisplayDroneEntry& drone) noexcept;
BarSpectrumRenderData calculate_bar_render_data(
    const BarRenderParams& params,
    int spectrum_height,
    const BarSpectrumConfig& config
) noexcept;
HistogramBinRenderData calculate_histogram_bin_render_data(
    const HistogramBinRenderParams& params,
    uint8_t max_count,
    const HistogramColorConfig& config
) noexcept;
HistogramDisplayBuffer scale_histogram_for_display(
    const uint16_t* analysis_histogram,
    const HistogramScaleParams& params
) noexcept;

// ============================================================================
// DRONE FILTERING AND SORTING (DSP Layer)
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
    StaleTimeout stale_timeout_ms,
    CurrentTime now
) noexcept;

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
) noexcept;

} // namespace ui::apps::enhanced_drone_analyzer::dsp

#endif // DSP_DISPLAY_TYPES_HPP_
