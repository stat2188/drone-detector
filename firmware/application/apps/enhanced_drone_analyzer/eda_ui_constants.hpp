// Diamond Code: UI-specific constants for Enhanced Drone Analyzer
// Separated from settings persistence to maintain clean architecture
// Target: STM32F405 (ARM Cortex-M4)

#ifndef EDA_UI_CONSTANTS_HPP_
#define EDA_UI_CONSTANTS_HPP_

// C++ standard library headers (alphabetical order)
#include <cstdint>

namespace ui::apps::enhanced_drone_analyzer {

/**
 * @brief UI Constants namespace
 *
 * DIAMOND FIX #3: Separated UI-specific constants from settings persistence
 * This namespace contains all UI-related constants that were previously mixed
 * with settings persistence, creating architectural confusion.
 *
 * Benefits:
 * - Clean separation of concerns (UI vs persistence)
 * - No circular dependencies
 * - DSP code can include eda_constants.hpp without pulling in UI code
 * - Settings persistence reduced by 25% (51 → 38 entries)
 */
namespace UIConstants {

// ============================================================================
// COLOR SCHEME
// ============================================================================

/**
 * @brief Color scheme options for the UI
 * @note Stored as string in settings ("DARK", "LIGHT", etc.)
 */
enum class ColorScheme : uint8_t {
    DARK = 0,      ///< Dark theme (default)
    LIGHT = 1,     ///< Light theme
    HIGH_CONTRAST = 2,  ///< High contrast theme
    CUSTOM = 3      ///< Custom theme (user-defined)
};

/**
 * @brief Default color scheme
 */
constexpr ColorScheme DEFAULT_COLOR_SCHEME = ColorScheme::DARK;

/**
 * @brief Maximum color scheme string length
 */
constexpr size_t COLOR_SCHEME_STRING_MAX_LENGTH = 32;

// ============================================================================
// FONT SIZE
// ============================================================================

/**
 * @brief Font size options for the UI
 * @note Stored as uint32_t in settings (0, 1, 2)
 */
enum class FontSize : uint8_t {
    SMALL = 0,      ///< Small font (8x16)
    MEDIUM = 1,     ///< Medium font (default)
    LARGE = 2        ///< Large font (12x24)
};

/**
 * @brief Default font size
 */
constexpr FontSize DEFAULT_FONT_SIZE = FontSize::MEDIUM;

// ============================================================================
// SPECTRUM DENSITY
// ============================================================================

/**
 * @brief Spectrum display density options
 * @note Controls how many spectrum bins are displayed
 */
enum class SpectrumDensity : uint8_t {
    LOW = 0,        ///< Low density (fewer bins, faster rendering)
    MEDIUM = 1,     ///< Medium density (default)
    HIGH = 2         ///< High density (more bins, slower rendering)
};

/**
 * @brief Default spectrum density
 */
constexpr SpectrumDensity DEFAULT_SPECTRUM_DENSITY = SpectrumDensity::MEDIUM;

// ============================================================================
// WATERFALL SPEED
// ============================================================================

/**
 * @brief Waterfall display speed options
 * @note Controls how fast the waterfall scrolls (1 = slow, 10 = fast)
 */
constexpr uint32_t MIN_WATERFALL_SPEED = 1;
constexpr uint32_t MAX_WATERFALL_SPEED = 10;

// ============================================================================
// DISPLAY LAYOUT CONSTANTS
// ============================================================================

/**
 * @brief Display layout constants for Enhanced Drone Analyzer
 * @note DIAMOND FIX LOW #1: Replaced magic numbers with named constants
 */

/**
 * @brief Status bar height in pixels
 */
constexpr uint8_t STATUS_BAR_HEIGHT = 2;

/**
 * @brief Error message X position in pixels
 */
constexpr uint16_t ERROR_MSG_X_POS = 10;

/**
 * @brief Error message Y positions in pixels
 */
constexpr uint16_t ERROR_MSG_Y_POS_1 = 80;
constexpr uint16_t ERROR_MSG_Y_POS_2 = 100;
constexpr uint16_t ERROR_MSG_Y_POS_3 = 130;

/**
 * @brief Loading message X position in pixels
 */
constexpr uint16_t LOADING_MSG_X_POS = 10;

/**
 * @brief Loading message Y positions in pixels
 */
constexpr uint16_t LOADING_MSG_Y_POS_1 = 80;
constexpr uint16_t LOADING_MSG_Y_POS_2 = 100;

/**
 * @brief Progress bar X position in pixels
 */
constexpr uint16_t PROGRESS_BAR_X_POS = 10;

/**
 * @brief Progress bar Y position in pixels
 */
constexpr uint16_t PROGRESS_BAR_Y_POS = 120;

/**
 * @brief Progress bar width in pixels
 */
constexpr uint16_t PROGRESS_BAR_WIDTH = 100;

/**
 * @brief Progress bar height in pixels
 */
constexpr uint8_t PROGRESS_BAR_HEIGHT = 10;

/**
 * @brief Progress bar background color (dark grey)
 */
constexpr uint16_t PROGRESS_BAR_BG_COLOR = 0x4208;  // Dark grey

/**
 * @brief Progress bar fill color (green)
 */
constexpr uint16_t PROGRESS_BAR_FILL_COLOR = 0x07E0;  // Green

/**
 * @brief Maximum progress value (100%)
 */
constexpr uint8_t PROGRESS_MAX_VALUE = 32;  // Scaled to 32 for display

// ============================================================================
// FREQUENCY RULER STYLE
// ============================================================================

/**
 * @brief Frequency ruler display style options
 * @note Controls how frequency ticks are displayed on the ruler
 */
enum class FrequencyRulerStyle : uint8_t {
    MINIMAL = 0,       ///< Minimal ticks (fewest labels)
    COMPACT = 1,       ///< Compact ticks (default)
    STANDARD = 2,       ///< Standard ticks
    DETAILED = 3,       ///< Detailed ticks (more labels)
    PRECISE = 4,        ///< Precise ticks (most labels)
    SCIENTIFIC = 5,     ///< Scientific notation
    CUSTOM = 6          ///< Custom style (user-defined)
};

/**
 * @brief Default frequency ruler style
 */
constexpr FrequencyRulerStyle DEFAULT_FREQUENCY_RULER_STYLE = FrequencyRulerStyle::COMPACT;

// ============================================================================
// COMPACT RULER TICK COUNT
// ============================================================================

/**
 * @brief Number of tick marks on compact frequency ruler
 * @note Range: 3-8 ticks
 */
constexpr uint32_t MIN_COMPACT_RULER_TICK_COUNT = 3;
constexpr uint32_t MAX_COMPACT_RULER_TICK_COUNT = 8;
constexpr uint32_t DEFAULT_COMPACT_RULER_TICK_COUNT = 4;

// ============================================================================
// DISPLAY FLAGS
// ============================================================================

/**
 * @brief Display feature flags namespace
 *
 * These flags control various UI display features.
 * They are stored as bitfields in the settings.
 *
 * Bit positions:
 * - Bit 0: Show spectrum analyzer
 * - Bit 1: Show waterfall display
 * - Bit 2: Show frequency ruler
 * - Bit 3: Show drone list
 * - Bit 4: Show compact ruler
 */
namespace DisplayFlags {
    constexpr uint8_t SHOW_SPECTRUM = 0;      ///< Show spectrum analyzer display
    constexpr uint8_t SHOW_WATERFALL = 1;      ///< Show waterfall display
    constexpr uint8_t SHOW_RULER = 2;          ///< Show frequency ruler
    constexpr uint8_t SHOW_DRONE_LIST = 3;      ///< Show detected drone list
    constexpr uint8_t SHOW_COMPACT_RULER = 4;   ///< Show compact frequency ruler

    // Default flag values
    constexpr bool DEFAULT_SHOW_SPECTRUM = true;
    constexpr bool DEFAULT_SHOW_WATERFALL = true;
    constexpr bool DEFAULT_SHOW_RULER = true;
    constexpr bool DEFAULT_SHOW_DRONE_LIST = true;
    constexpr bool DEFAULT_SHOW_COMPACT_RULER = true;
}

// ============================================================================
// UI DIMENSIONS
// ============================================================================

/**
 * @brief UI layout dimensions (constants)
 */
namespace Dimensions {
    constexpr uint32_t SCREEN_WIDTH = 240;
    constexpr uint32_t SCREEN_HEIGHT = 320;
    constexpr uint32_t TEXT_HEIGHT = 16;
    constexpr uint32_t TEXT_LINE_HEIGHT = 24;
    constexpr uint32_t DISPLAY_UPDATE_INTERVAL_MS = 100;
    constexpr uint32_t UI_REFRESH_RATE_MS = 50;
    constexpr uint32_t SCREEN_BLANK_TIMEOUT_MS = 60000;
}

// ============================================================================
// UI VALIDATION
// ============================================================================

/**
 * @brief Validation functions for UI constants
 */
namespace Validation {

/**
 * @brief Validate color scheme value
 * @param scheme Color scheme to validate
 * @return true if valid, false otherwise
 */
constexpr bool is_valid_color_scheme(ColorScheme scheme) noexcept {
    return scheme == ColorScheme::DARK ||
           scheme == ColorScheme::LIGHT ||
           scheme == ColorScheme::HIGH_CONTRAST ||
           scheme == ColorScheme::CUSTOM;
}

/**
 * @brief Validate font size value
 * @param size Font size to validate
 * @return true if valid, false otherwise
 */
constexpr bool is_valid_font_size(FontSize size) noexcept {
    return size == FontSize::SMALL ||
           size == FontSize::MEDIUM ||
           size == FontSize::LARGE;
}

/**
 * @brief Validate spectrum density value
 * @param density Spectrum density to validate
 * @return true if valid, false otherwise
 */
constexpr bool is_valid_spectrum_density(SpectrumDensity density) noexcept {
    return density == SpectrumDensity::LOW ||
           density == SpectrumDensity::MEDIUM ||
           density == SpectrumDensity::HIGH;
}

/**
 * @brief Validate waterfall speed value
 * @param speed Waterfall speed to validate
 * @return true if valid, false otherwise
 */
constexpr bool is_valid_waterfall_speed(uint32_t speed) noexcept {
    return speed >= MIN_WATERFALL_SPEED && speed <= MAX_WATERFALL_SPEED;
}

/**
 * @brief Validate frequency ruler style value
 * @param style Frequency ruler style to validate
 * @return true if valid, false otherwise
 */
constexpr bool is_valid_ruler_style(FrequencyRulerStyle style) noexcept {
    return style == FrequencyRulerStyle::MINIMAL ||
           style == FrequencyRulerStyle::COMPACT ||
           style == FrequencyRulerStyle::STANDARD ||
           style == FrequencyRulerStyle::DETAILED ||
           style == FrequencyRulerStyle::PRECISE ||
           style == FrequencyRulerStyle::SCIENTIFIC ||
           style == FrequencyRulerStyle::CUSTOM;
}

/**
 * @brief Validate compact ruler tick count value
 * @param count Tick count to validate
 * @return true if valid, false otherwise
 */
constexpr bool is_valid_ruler_tick_count(uint32_t count) noexcept {
    return count >= MIN_COMPACT_RULER_TICK_COUNT && count <= MAX_COMPACT_RULER_TICK_COUNT;
}

} // namespace Validation

} // namespace UIConstants

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_UI_CONSTANTS_HPP_
