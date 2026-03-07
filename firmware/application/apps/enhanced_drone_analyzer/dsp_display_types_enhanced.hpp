/**
 * @file dsp_display_types_enhanced.hpp
 * @brief Enhanced data structures for DSP/UI communication with Diamond Code stack optimizations
 * 
 * This file defines all data structures used for communication between
 * the DSP layer (signal processing) and UI layer (presentation) with
 * advanced stack optimization best practices for STM32F405 (ARM Cortex-M4).
 * 
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All structures use fixed-size arrays
 * - No exceptions: All functions are noexcept
 * - Type-safe: Uses semantic type aliases
 * - Memory-safe: Stack-only allocation
 * - ARM Cortex-M4 optimized: alignas(8) for optimal memory access
 * - Compile-time validation: static_assert for size and alignment
 * - Static storage: FilteredDronesStorage namespace for zero-heap snapshots
 * 
 * DIAMOND FIXES APPLIED:
 * - Fix #1: alignas(8) on all structures for ARM Cortex-M4 optimization
 * - Fix #2: static_assert for size and alignment validation
 * - Fix #3: Deleted new operators to prevent heap allocation
 * - Fix #4: FilteredDronesStorage namespace for static storage
 * - Fix #5: get_filtered_drones_snapshot() returns reference instead of value
 * - Fix #6: Bit field opportunities for flags
 * - Fix #7: EDA_FLASH_CONST macro for Flash storage
 * 
 * @author Diamond Code Pipeline - Stage 5
 * @date 2026-03-05
 */

#ifndef DSP_DISPLAY_TYPES_ENHANCED_HPP_
#define DSP_DISPLAY_TYPES_ENHANCED_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Third-party library headers
#include <ch.h>

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"
#include "eda_optimized_utils.hpp"
#include "ui_drone_common_types.hpp"

// Flash storage attribute for Cortex-M4
#ifdef __GNUC__
    #define EDA_FLASH_CONST __attribute__((section(".rodata")))
#else
    #define EDA_FLASH_CONST
#endif

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

// ============================================================================
// DIAMOND FIX #7: EDA_FLASH_CONST macro for Flash storage
// ============================================================================

/**
 * @brief Flash storage macro for Cortex-M4
 * 
 * Places data in read-only Flash memory instead of RAM.
 * Use for constant arrays and tables that don't change at runtime.
 * 
 * Usage:
 * @code
 *     EDA_FLASH_CONST static constexpr uint32_t MY_TABLE[10] = { ... };
 * @endcode
 * 
 * @note Data is stored in .rodata section (Flash)
 * @note Reduces RAM usage by placing constants in Flash
 * @note Compatible with ARM Cortex-M4 memory layout
 */
#define EDA_FLASH_CONST __attribute__((section(".rodata")))

// ============================================================================
// DIAMOND FIX #1 & #2: Enhanced structures with alignas(8) and static_assert
// ============================================================================

/**
 * @brief Display drone entry for UI rendering
 *
 * This structure contains drone data that has been prepared for UI display.
 * It includes frequency, type name, RSSI, and trend information.
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * PHASE 3 FIX #9: Replaced magic number 16 with MAX_DRONE_TYPE_NAME_LENGTH
 * PHASE 4 FIX #13: Use RSSIValue consistently instead of int32_t
 */
struct alignas(8) DisplayDroneEntry {
    char type_name[DisplayTypeConstants::MAX_DRONE_TYPE_NAME_LENGTH];  ///< Drone type name (e.g., "MAVIC")
    Frequency frequency;            ///< Drone frequency in Hz
    RSSIValue rssi;               ///< RSSI in dBm (PHASE 4 FIX #13: Use RSSIValue)
    MovementTrend trend;           ///< Movement trend

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for DisplayDroneEntry
static_assert(sizeof(DisplayDroneEntry) <= 40, 
              "DisplayDroneEntry exceeds 40 bytes");
static_assert(alignof(DisplayDroneEntry) == 8, 
              "DisplayDroneEntry must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Display data snapshot for UI rendering
 * 
 * This structure provides a snapshot of all display data from the DSP layer
 * to the UI layer. It includes scanning state, threat levels, drone counts,
 * and pre-calculated color indices.
 * 
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #6: Bit field opportunities for flags
 * 
 * Thread-safety: Snapshot should be captured under mutex and then
 * used without further synchronization.
 */
struct alignas(8) DisplayDataSnapshot {
    // DIAMOND FIX #6: Bit field for boolean flags (reduces size from 8 bytes to 1 byte)
    struct {
        unsigned is_scanning : 1;          ///< Scanning active flag
        unsigned is_real_mode : 1;         ///< Real mode (vs demo mode)
        unsigned has_detections : 1;        ///< Has any detections
        unsigned reserved : 5;             ///< Reserved for future use
    } flags;

    Frequency current_freq;          ///< Current scanning frequency
    size_t total_freqs;            ///< Total frequencies in database
    ThreatLevel max_threat;         ///< Maximum detected threat level
    size_t approaching_count;       ///< Number of approaching drones
    size_t receding_count;         ///< Number of receding drones
    size_t static_count;           ///< Number of static drones
    size_t total_detections;        ///< Total detection count
    uint32_t scan_cycles;           ///< Number of scan cycles completed
    uint8_t color_idx;             ///< Pre-calculated color index
    size_t current_freq_idx;         ///< Current frequency index in database (for progress calculation)

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for DisplayDataSnapshot
static_assert(sizeof(DisplayDataSnapshot) <= 72, 
              "DisplayDataSnapshot exceeds 72 bytes");
static_assert(alignof(DisplayDataSnapshot) == 8, 
              "DisplayDataSnapshot must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Tracked drone data for UI display
 *
 * This structure contains drone data that has been filtered and
 * prepared for UI display. It includes frequency, type, threat level,
 * RSSI, and trend information.
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #6: Bit field opportunities for flags
 * PHASE 4 FIX #13: Use RSSIValue consistently instead of int32_t
 * PHASE 4 FIX #12: Use systime_t consistently for timestamps
 */
struct alignas(8) TrackedDroneData {
    Frequency frequency;            ///< Drone frequency in Hz
    uint8_t drone_type;            ///< Drone type (DroneType enum)
    uint8_t threat_level;          ///< Threat level (ThreatLevel enum)
    RSSIValue rssi;               ///< RSSI in dBm (PHASE 4 FIX #13: Use RSSIValue)
    systime_t last_seen;           ///< Last seen timestamp (PHASE 4 FIX #12: Use systime_t)
    MovementTrend trend;           ///< Movement trend

    // DIAMOND FIX #6: Bit field for additional flags (reduces size from 4 bytes to 1 byte)
    struct {
        unsigned is_approaching : 1;      ///< Drone is approaching
        unsigned is_receding : 1;         ///< Drone is receding
        unsigned is_static : 1;           ///< Drone is static
        unsigned is_new_detection : 1;    ///< New detection in this scan cycle
        unsigned reserved : 4;           ///< Reserved for future use
    } flags;

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for TrackedDroneData
static_assert(sizeof(TrackedDroneData) <= 40, 
              "TrackedDroneData exceeds 40 bytes");
static_assert(alignof(TrackedDroneData) == 8, 
              "TrackedDroneData must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Filtered drones snapshot for UI rendering
 *
 * This structure provides a snapshot of filtered drone data from the DSP layer
 * to the UI layer. It includes only active drones that have been seen recently.
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #4: FilteredDronesStorage namespace for static storage
 * PHASE 3 FIX #9: Replaced magic number 10 with MAX_FILTERED_DRONES
 * 
 * Thread-safety: Snapshot should be captured under mutex and then
 * used without further synchronization.
 */
struct alignas(8) FilteredDronesSnapshot {
    size_t count;                                          ///< Number of drones in snapshot
    TrackedDroneData drones[DisplayTypeConstants::MAX_FILTERED_DRONES];  ///< Filtered drone data (max MAX_FILTERED_DRONES)

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for FilteredDronesSnapshot
// Note: 10 drones * 40 bytes each + 8 bytes count = 408 bytes total
static_assert(sizeof(FilteredDronesSnapshot) <= 416, 
              "FilteredDronesSnapshot exceeds 416 bytes (MAX_FILTERED_DRONES * 40 bytes per drone + 8 bytes count)");
static_assert(alignof(FilteredDronesSnapshot) == 8, 
              "FilteredDronesSnapshot must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Drone display text for UI rendering
 *
 * This structure contains pre-formatted text data for rendering a drone entry.
 * It includes the type name, formatted frequency string, RSSI, and trend symbol.
 *
 * This structure is used to separate text formatting (DSP logic) from
 * UI rendering (presentation logic).
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #7: EDA_FLASH_CONST macro for Flash storage
 * PHASE 3 FIX #9: Replaced magic numbers 16 with MAX_DRONE_TYPE_NAME_LENGTH and MAX_FREQ_STRING_LENGTH
 */
struct alignas(8) DroneDisplayText {
    char type_name[DisplayTypeConstants::MAX_DRONE_TYPE_NAME_LENGTH];  ///< Drone type name (e.g., "MAVIC")
    char freq_string[DisplayTypeConstants::MAX_FREQ_STRING_LENGTH];  ///< Formatted frequency string (e.g., "2.400GHz")
    int32_t rssi;                 ///< RSSI in dBm
    char trend_symbol;             ///< Trend symbol ('=', '^', 'v', '~')

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for DroneDisplayText
static_assert(sizeof(DroneDisplayText) <= 48, 
              "DroneDisplayText exceeds 48 bytes");
static_assert(alignof(DroneDisplayText) == 8, 
              "DroneDisplayText must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Bar spectrum render data for UI rendering
 * 
 * This structure contains pre-calculated rendering data for a single bar
 * in the spectrum display. It includes position, height, color index,
 * and a flag indicating whether the bar should be drawn.
 * 
 * This structure is used to separate signal analysis (DSP logic) from
 * UI rendering (presentation logic).
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #6: Bit field opportunities for flags
 */
struct alignas(8) BarSpectrumRenderData {
    int y_top;                    ///< Top Y position for drawing
    int bar_height;                ///< Bar height in pixels
    size_t color_idx;              ///< Color index for bar
    bool should_draw;              ///< Whether to draw this bar (false if below noise threshold)

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for BarSpectrumRenderData
static_assert(sizeof(BarSpectrumRenderData) <= 24, 
              "BarSpectrumRenderData exceeds 24 bytes");
static_assert(alignof(BarSpectrumRenderData) == 8, 
              "BarSpectrumRenderData must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Histogram display buffer for UI rendering
 * 
 * This structure contains scaled histogram data ready for display.
 * It includes bin counts, maximum count, noise floor, and validity flag.
 * 
 * This structure is used to separate data scaling (DSP logic) from
 * UI rendering (presentation logic).
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #6: Bit field opportunities for flags
 */
struct alignas(8) HistogramDisplayBuffer {
    uint8_t bin_counts[64];       ///< Histogram bin counts (0-255)
    uint8_t max_count;            ///< Maximum bin count (for scaling)
    uint8_t noise_floor;          ///< Noise floor from spectral analysis
    bool is_valid;                ///< Buffer contains valid data

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for HistogramDisplayBuffer
// Note: 64 bytes bin_counts + 1 byte max_count + 1 byte noise_floor + 1 byte is_valid + 5 bytes padding = 72 bytes total
static_assert(sizeof(HistogramDisplayBuffer) <= 72, 
              "HistogramDisplayBuffer exceeds 72 bytes");
static_assert(alignof(HistogramDisplayBuffer) == 8, 
              "HistogramDisplayBuffer must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Histogram bin render data for UI rendering
 * 
 * This structure contains pre-calculated rendering data for a single bin
 * in the histogram display. It includes position, dimensions, color index,
 * and a flag indicating whether the bin should be drawn.
 * 
 * This structure is used to separate histogram calculations (DSP logic) from
 * UI rendering (presentation logic).
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #6: Bit field opportunities for flags
 */
struct alignas(8) HistogramBinRenderData {
    int bin_x;                     ///< X position for drawing
    int y_top;                     ///< Top Y position for drawing
    int bin_width;                 ///< Bin width in pixels
    int bin_height;                ///< Bin height in pixels
    size_t color_idx;              ///< Color index for bin
    bool should_draw;              ///< Whether to draw this bin (false if empty)

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for HistogramBinRenderData
static_assert(sizeof(HistogramBinRenderData) <= 32, 
              "HistogramBinRenderData exceeds 32 bytes");
static_assert(alignof(HistogramBinRenderData) == 8, 
              "HistogramBinRenderData must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Frequency format entry for frequency formatting
 * 
 * This structure contains formatting parameters for displaying frequencies
 * at different scales (GHz, MHz, kHz, Hz).
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #2: static_assert for size validation
 * DIAMOND FIX #3: Deleted new operators to prevent heap allocation
 * DIAMOND FIX #7: EDA_FLASH_CONST macro for Flash storage
 */
struct alignas(8) FreqFormatEntry {
    int64_t divider;               ///< Divider to get integer part
    int64_t decimal_div;           ///< Divider to get decimal part
    const char* format;             ///< Format string for snprintf

    // DIAMOND FIX #3: Delete new operators to prevent heap allocation
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

// DIAMOND FIX #2: Compile-time size validation for FreqFormatEntry
static_assert(sizeof(FreqFormatEntry) <= 24, 
              "FreqFormatEntry exceeds 24 bytes");
static_assert(alignof(FreqFormatEntry) == 8, 
              "FreqFormatEntry must be 8-byte aligned for ARM Cortex-M4");

/**
 * @brief Frequency format table for different frequency ranges
 *
 * This table provides formatting parameters for displaying frequencies
 * at different scales. The index is determined by the frequency magnitude.
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #7: Move to Flash using EDA_FLASH_CONST constexpr (~64 bytes)
 * Note: inline constexpr to avoid ODR violations when header is included
 * in multiple translation units.
 */
EDA_FLASH_CONST inline static constexpr FreqFormatEntry FREQ_FORMAT_TABLE[] = {
    {1000000000LL, 100000000LL, "%lu.%03lu GHz"},  // >= 1 GHz
    {1000000LL, 100000LL, "%lu.%03lu MHz"},        // >= 1 MHz
    {1000LL, 100LL, "%lu.%02lu kHz"},            // >= 1 kHz
    {1LL, 1, "%lu Hz"}                           // < 1 kHz
};

// DIAMOND FIX #2: Compile-time size validation for FREQ_FORMAT_TABLE
static_assert(sizeof(FREQ_FORMAT_TABLE) <= 96, 
              "FREQ_FORMAT_TABLE exceeds 96 bytes");

/**
 * @brief Bar spectrum configuration constants
 * 
 * This structure contains configuration constants for bar spectrum rendering.
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #7: Move to Flash using EDA_FLASH_CONST constexpr
 */
struct alignas(8) BarSpectrumConfig {
    static constexpr int WATERFALL_Y_START = 120;  ///< Y position of spectrum display
    static constexpr int BAR_HEIGHT_MAX = 40;        ///< Maximum bar height
    static constexpr uint8_t NOISE_THRESHOLD = 10;   ///< Minimum power to display

    // Color level thresholds (0-255 scale)
    static constexpr uint8_t COLOR_THRESHOLD_LOW = 50;       ///< Low power threshold (20%)
    static constexpr uint8_t COLOR_THRESHOLD_MED_LOW = 100;  ///< Medium-low power threshold (40%)
    static constexpr uint8_t COLOR_THRESHOLD_MED_HIGH = 150; ///< Medium-high power threshold (60%)
    static constexpr uint8_t COLOR_THRESHOLD_HIGH = 200;  ///< High power threshold (80%)
    
    /// Bar colors for different power levels (NUM_COLOR_LEVELS levels)
    /// PHASE 3 FIX #9: Replaced magic number 5 with NUM_COLOR_LEVELS
    /// DIAMOND FIX #7: Move to Flash using EDA_FLASH_CONST constexpr (~20 bytes)
    EDA_FLASH_CONST static constexpr uint32_t BAR_COLORS[DisplayTypeConstants::NUM_COLOR_LEVELS] = {
        0x001F,  // Blue (low)
        0x07E0,  // Green (medium-low)
        0xFFE0,  // Yellow (medium)
        0xF800,  // Red (high)
        0xF81F   // Magenta (critical)
    };
};

// DIAMOND FIX #2: Compile-time size validation for BarSpectrumConfig
static_assert(sizeof(BarSpectrumConfig::BAR_COLORS) <= 20, 
              "BarSpectrumConfig::BAR_COLORS exceeds 20 bytes");

/**
 * @brief Histogram color configuration constants
 * 
 * This structure contains configuration constants for histogram rendering.
 *
 * DIAMOND FIX #1: alignas(8) for ARM Cortex-M4 optimization
 * DIAMOND FIX #7: Move to Flash using EDA_FLASH_CONST constexpr
 */
struct alignas(8) HistogramColorConfig {
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
    /// DIAMOND FIX #7: Move to Flash using EDA_FLASH_CONST constexpr (~20 bytes)
    EDA_FLASH_CONST static constexpr uint32_t HISTOGRAM_COLORS[DisplayTypeConstants::NUM_COLOR_LEVELS] = {
        0x001F,  // Blue (20%)
        0x07E0,  // Green (40%)
        0xFFE0,  // Yellow (60%)
        0xF800,  // Red (80%)
        0xF81F   // Magenta (100%)
    };
};

// DIAMOND FIX #2: Compile-time size validation for HistogramColorConfig
static_assert(sizeof(HistogramColorConfig::HISTOGRAM_COLORS) <= 20, 
              "HistogramColorConfig::HISTOGRAM_COLORS exceeds 20 bytes");

// ============================================================================
// DIAMOND FIX #4: FilteredDronesStorage namespace for static storage
// ============================================================================

/**
 * @brief Static storage for filtered drones snapshot
 * 
 * This namespace provides static storage for the filtered drones snapshot,
 * eliminating heap allocation and providing thread-safe access to the snapshot.
 * 
 * DIAMOND FIX #4: Static storage pattern (no heap allocation)
 * - Uses static storage instead of heap allocation
 * - Provides thread-safe access via mutex
 * - Returns reference instead of value to avoid copy
 * 
 * USAGE:
 * @code
 *     // Initialize snapshot (call once at startup)
 *     FilteredDronesStorage::initialize();
 *     
 *     // Update snapshot (call from DSP thread)
 *     FilteredDronesStorage::update_snapshot(new_snapshot);
 *     
 *     // Get snapshot reference (call from UI thread)
 *     const FilteredDronesSnapshot& snapshot = FilteredDronesStorage::get_snapshot();
 * @endcode
 * 
 * @note All storage is static (no heap allocation)
 * @note Thread-safe access via mutex
 * @note Returns reference to avoid copy overhead
 */
namespace FilteredDronesStorage {
    /**
     * @brief Initialize static storage
     * @note Must be called once at startup
     */
    void initialize() noexcept;

    /**
     * @brief Update snapshot with new data
     * @param snapshot New snapshot data to store
     * @note Thread-safe (acquires mutex)
     */
    void update_snapshot(const FilteredDronesSnapshot& snapshot) noexcept;

    /**
     * @brief Get reference to current snapshot
     * @return Reference to current snapshot
     * @note Thread-safe (acquires mutex)
     * @note Returns reference instead of value to avoid copy
     * 
     * DIAMOND FIX #5: Returns reference instead of value
     * This avoids copying the entire snapshot (up to 416 bytes)
     * and provides direct access to the static storage.
     */
    [[nodiscard]] const FilteredDronesSnapshot& get_snapshot() noexcept;

    /**
     * @brief Check if snapshot has been initialized
     * @return true if initialized, false otherwise
     */
    [[nodiscard]] bool is_initialized() noexcept;
}

// ============================================================================
// Forward declarations for utility functions (implemented in dsp_display_utils.cpp)
// ============================================================================

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
    systime_t stale_timeout_ms,
    systime_t now
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

#endif // DSP_DISPLAY_TYPES_ENHANCED_HPP_
