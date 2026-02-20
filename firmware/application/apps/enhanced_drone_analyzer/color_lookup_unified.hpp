/**
 * @file color_lookup_unified.hpp
 * @brief Unified Color Lookup Table - Diamond Code Refinement
 * 
 * DIAMOND STANDARD: Memory-safe, optimized, zero-allocation
 * 
 * FEATURES:
 * - ✅ Correct RGB888 → RGB565 conversion
 * - ✅ Eliminated code duplication (HEADER_STYLES, CARD_STYLES)
 * - ✅ DRONE_COLORS: 11 types (matches DroneType enum)
 * - ✅ All data stored in Flash (constexpr FLASH_STORAGE)
 * - ✅ O(1) lookup with bounds checking
 * - ✅ noexcept on all functions
 * - ✅ constexpr where possible
 * 
 * MEMORY: 0 bytes RAM (all data in Flash)
 * PERFORMANCE: O(1) lookup, compile-time evaluation
 * 
 * @author Diamond Core Protocol Refinement
 * @version 2.0.0
 */

#ifndef COLOR_LOOKUP_UNIFIED_HPP_
#define COLOR_LOOKUP_UNIFIED_HPP_

#include <cstdint>
#include "ui.hpp"  // For Color class

// Flash storage attributes for Cortex-M4
#ifdef __GNUC__
    #define FLASH_STORAGE __attribute__((section(".rodata")))
#else
    #define FLASH_STORAGE
#endif

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// TYPE ALIASES (Semantic Types)
// ===========================================
using RGB888 = uint32_t;
using RGB565 = uint16_t;
using RGB888Component = uint8_t;
using ColorIndex = uint8_t;

// ===========================================
// CONSTANTS
// ===========================================
namespace ColorConstants {
    constexpr RGB888Component RED_MASK_888 = 0xFF;
    constexpr RGB888Component GREEN_MASK_888 = 0xFF;
    constexpr RGB888Component BLUE_MASK_888 = 0xFF;
    constexpr RGB565 RED_MASK_565 = 0xF8;
    constexpr RGB565 GREEN_MASK_565 = 0xFC;
    constexpr RGB565 BLUE_MASK_565 = 0xF8;
    
    // Bit shift positions
    constexpr int RED_SHIFT = 16;
    constexpr int GREEN_SHIFT = 8;
    constexpr int BLUE_SHIFT = 0;
    constexpr int RED_565_SHIFT = 8;
    constexpr int GREEN_565_SHIFT = 3;
    constexpr int BLUE_565_RIGHT_SHIFT = 3;  // Right shift for blue component
    
    // Array bounds
    constexpr uint8_t THREAT_LEVEL_COUNT = 6;
    constexpr uint8_t DRONE_TYPE_COUNT = 11;
    constexpr uint8_t CARD_STYLE_COUNT = 5;
    constexpr uint8_t SPECTRUM_MODE_COUNT = 5;
}

// ===========================================
// RGB888 → RGB565 CONVERTER (constexpr)
// ===========================================
// Color format (ui.hpp): rrrrrGGGGGGbbbbb (RGB565, 16-bit)
// LUT format: 0xRRGGBB (RGB888, 24-bit)
//
// R888 → R565: truncate 3 LSB bits (8 → 5)
// G888 → G665: truncate 2 LSB bits (8 → 6)
// B888 → B565: truncate 3 LSB bits (8 → 5)

struct ColorConverter {
    /**
     * @brief Convert RGB888 to RGB565 (O(1), constexpr)
     * @param rgb888 24-bit RGB value (0xRRGGBB)
     * @return 16-bit RGB565 value
     */
    static constexpr RGB565 rgb888_to_rgb565(const RGB888 rgb888) noexcept {
        const RGB888Component r = (rgb888 >> ColorConstants::RED_SHIFT) & ColorConstants::RED_MASK_888;
        const RGB888Component g = (rgb888 >> ColorConstants::GREEN_SHIFT) & ColorConstants::GREEN_MASK_888;
        const RGB888Component b = rgb888 & ColorConstants::BLUE_MASK_888;

        const RGB565 r565 = (r & ColorConstants::RED_MASK_565) << ColorConstants::RED_565_SHIFT;
        const RGB565 g565 = (g & ColorConstants::GREEN_MASK_565) << ColorConstants::GREEN_565_SHIFT;
        const RGB565 b565 = (b & ColorConstants::BLUE_MASK_565) >> ColorConstants::BLUE_565_RIGHT_SHIFT;

        return r565 | g565 | b565;
    }

    /**
     * @brief Convert RGB888 to Color object (O(1), constexpr)
     * @param rgb888 24-bit RGB value (0xRRGGBB)
     * @return Color object for UI rendering
     */
    static constexpr Color rgb888_to_color(const RGB888 rgb888) noexcept {
        return Color(rgb888_to_rgb565(rgb888));
    }
};

// ===========================================
// UNIFIED THREAT COLORS (6 levels)
// ===========================================
// Eliminates duplication: HEADER_STYLES + CARD_STYLES + THREAT_COLORS
// Stored in Flash (constexpr FLASH_STORAGE), 0 bytes RAM

struct ThreatColorLUT {
    /**
     * @brief Threat level indicator colors (bright display)
     */
    static constexpr uint16_t COLORS[ColorConstants::THREAT_LEVEL_COUNT] FLASH_STORAGE = {
        ColorConverter::rgb888_to_rgb565(0xFF0000),   // Red - NONE (0)
        ColorConverter::rgb888_to_rgb565(0x00FF00),   // Green - LOW (1)
        ColorConverter::rgb888_to_rgb565(0xFFFF00),   // Yellow - MEDIUM (2)
        ColorConverter::rgb888_to_rgb565(0xFFA500),   // Orange - HIGH (3)
        ColorConverter::rgb888_to_rgb565(0x800000),   // Dark Red - CRITICAL (4)
        ColorConverter::rgb888_to_rgb565(0x808080)    // Grey - UNKNOWN (5)
    };
    static_assert(sizeof(COLORS) == sizeof(uint16_t) * ColorConstants::THREAT_LEVEL_COUNT, 
                  "COLORS size mismatch");
    
    /**
     * @brief Card styles (background + text) - UNIFIED
     */
    struct CardStyle {
        uint16_t bg_color;
        uint16_t text_color;
    };
    
    static constexpr CardStyle CARD_STYLES[ColorConstants::CARD_STYLE_COUNT] FLASH_STORAGE = {
        {ColorConverter::rgb888_to_rgb565(0x000040), ColorConverter::rgb888_to_rgb565(0xFFFFFF)},  // Dark Blue bg, White text - NONE (0)
        {ColorConverter::rgb888_to_rgb565(0x002000), ColorConverter::rgb888_to_rgb565(0xFFFFFF)},  // Dark Green bg, White text - LOW (1)
        {ColorConverter::rgb888_to_rgb565(0x202000), ColorConverter::rgb888_to_rgb565(0x000000)},  // Dark Yellow bg, Black text - MEDIUM (2)
        {ColorConverter::rgb888_to_rgb565(0x402000), ColorConverter::rgb888_to_rgb565(0xFFFFFF)},  // Dark Orange bg, White text - HIGH (3)
        {ColorConverter::rgb888_to_rgb565(0x400000), ColorConverter::rgb888_to_rgb565(0xFFFFFF)}   // Dark Red bg, White text - CRITICAL (4)
    };
    static_assert(sizeof(CARD_STYLES) == sizeof(CardStyle) * ColorConstants::CARD_STYLE_COUNT, 
                  "CARD_STYLES size mismatch");
    
    /**
     * @brief Get threat level color (O(1) lookup with bounds checking)
     * @param level Threat level index (0-5)
     * @return Color object for the threat level
     */
    static inline Color threat_color(uint8_t level) noexcept {
        if (level >= ColorConstants::THREAT_LEVEL_COUNT) {
            level = ColorConstants::THREAT_LEVEL_COUNT - 1;  // Default to UNKNOWN
        }
        return Color(COLORS[level]);
    }
    
    /**
     * @brief Get card background color (O(1) lookup with bounds checking)
     * @param threat Threat level index (0-4)
     * @return Color object for card background
     */
    static inline Color card_bg_color(uint8_t threat) noexcept {
        if (threat >= ColorConstants::CARD_STYLE_COUNT) {
            threat = ColorConstants::CARD_STYLE_COUNT - 1;  // Default to CRITICAL
        }
        return Color(CARD_STYLES[threat].bg_color);
    }
    
    /**
     * @brief Get card text color (O(1) lookup with bounds checking)
     * @param threat Threat level index (0-4)
     * @return Color object for card text
     */
    static inline Color card_text_color(uint8_t threat) noexcept {
        if (threat >= ColorConstants::CARD_STYLE_COUNT) {
            threat = ColorConstants::CARD_STYLE_COUNT - 1;  // Default to CRITICAL
        }
        return Color(CARD_STYLES[threat].text_color);
    }
};

// ===========================================
// UNIFIED DRONE COLORS (11 types)
// ===========================================
// Matches DroneType enum (ui_drone_common_types.hpp)

struct DroneColorLUT {
    /**
     * @brief Drone type indicator colors
     */
    static constexpr uint16_t COLORS[ColorConstants::DRONE_TYPE_COUNT] FLASH_STORAGE = {
        ColorConverter::rgb888_to_rgb565(0xFFFFFF),   // White - UNKNOWN (0)
        ColorConverter::rgb888_to_rgb565(0xFF0000),   // Red - MAVIC (1)
        ColorConverter::rgb888_to_rgb565(0xFFA500),   // Orange - DJI_P34 (2)
        ColorConverter::rgb888_to_rgb565(0xFFFF00),   // Yellow - PHANTOM (3)
        ColorConverter::rgb888_to_rgb565(0x00FFFF),   // Cyan - DJI_MINI (4)
        ColorConverter::rgb888_to_rgb565(0xFF00FF),   // Magenta - PARROT_ANAFI (5)
        ColorConverter::rgb888_to_rgb565(0x00FF00),   // Green - PARROT_BEBOP (6)
        ColorConverter::rgb888_to_rgb565(0x800080),   // Purple - PX4_DRONE (7)
        ColorConverter::rgb888_to_rgb565(0x000000),   // Black - MILITARY_DRONE (8)
        ColorConverter::rgb888_to_rgb565(0xC0C0C0),   // Silver - DIY_DRONE (9)
        ColorConverter::rgb888_to_rgb565(0xFFC0CB)    // Pink - FPV_RACING (10)
    };
    static_assert(sizeof(COLORS) == sizeof(uint16_t) * ColorConstants::DRONE_TYPE_COUNT, 
                  "COLORS size mismatch");
    
    /**
     * @brief Get drone type color (O(1) lookup with bounds checking)
     * @param type Drone type index (0-10)
     * @return Color object for the drone type
     */
    static inline Color drone_color(uint8_t type) noexcept {
        if (type >= ColorConstants::DRONE_TYPE_COUNT) {
            type = 0;  // Default to UNKNOWN
        }
        return Color(COLORS[type]);
    }
};

// ===========================================
// UNIFIED COLOR ACCESS
// ===========================================
// Combines ThreatColorLUT and DroneColorLUT

struct UnifiedColorLookup {
    /**
     * @brief Get threat level color
     * @param level Threat level index (0-5)
     * @return Color object for the threat level
     */
    static inline Color threat(uint8_t level) noexcept {
        return ThreatColorLUT::threat_color(level);
    }
    
    /**
     * @brief Get drone type color
     * @param type Drone type index (0-10)
     * @return Color object for the drone type
     */
    static inline Color drone(uint8_t type) noexcept {
        return DroneColorLUT::drone_color(type);
    }
    
    /**
     * @brief Get card background color
     * @param threat Threat level index (0-4)
     * @return Color object for card background
     */
    static inline Color card_bg(uint8_t threat) noexcept {
        return ThreatColorLUT::card_bg_color(threat);
    }
    
    /**
     * @brief Get card text color
     * @param threat Threat level index (0-4)
     * @return Color object for card text
     */
    static inline Color card_text(uint8_t threat) noexcept {
        return ThreatColorLUT::card_text_color(threat);
    }
    
    /**
     * @brief Get header bar color (used in SmartThreatHeader)
     * @param threat Threat level index (0-5)
     * @return Color object for header bar
     */
    static inline Color header_bar(uint8_t threat) noexcept {
        // For header use threat_color (bright display)
        return ThreatColorLUT::threat_color(threat);
    }
};

// ===========================================
// UNIFIED STRING LOOKUP TABLES (SSOT)
// ===========================================
// Eliminates duplicates from: diamond_core.hpp, eda_optimized_utils.hpp, ui_enhanced_drone_settings.cpp
// Correctly sized for all enum values
// Stored in Flash (constexpr FLASH_STORAGE), 0 bytes RAM

struct UnifiedStringLookup {
    /**
     * @brief Threat level names (6 levels: matches ThreatLevel enum)
     */
    static constexpr const char* const THREAT_NAMES[ColorConstants::THREAT_LEVEL_COUNT] FLASH_STORAGE = {
        "NONE",       // NONE (0)
        "LOW",        // LOW (1)
        "MEDIUM",     // MEDIUM (2)
        "HIGH",       // HIGH (3)
        "CRITICAL",   // CRITICAL (4)
        "UNKNOWN"     // UNKNOWN (5)
    };

    /**
     * @brief Drone type names (11 types: matches DroneType enum)
     */
    static constexpr const char* const DRONE_TYPE_NAMES[ColorConstants::DRONE_TYPE_COUNT] FLASH_STORAGE = {
        "Unknown",        // UNKNOWN (0)
        "DJI Mavic",      // MAVIC (1)
        "DJI P34",        // DJI_P34 (2)
        "DJI Phantom",    // PHANTOM (3)
        "DJI Mini",       // DJI_MINI (4)
        "Parrot Anafi",   // PARROT_ANAFI (5)
        "Parrot Bebop",   // PARROT_BEBOP (6)
        "PX4 Drone",      // PX4_DRONE (7)
        "Military UAV",   // MILITARY_DRONE (8)
        "DIY Drone",      // DIY_DRONE (9)
        "FPV Racing"      // FPV_RACING (10)
    };

    /**
     * @brief Spectrum mode names (5 modes: matches SpectrumMode enum)
     */
    static constexpr const char* const SPECTRUM_MODE_NAMES[ColorConstants::SPECTRUM_MODE_COUNT] FLASH_STORAGE = {
        "NARROW",       // NARROW (0)
        "MEDIUM",       // MEDIUM (1)
        "WIDE",         // WIDE (2)
        "ULTRA_WIDE",   // ULTRA_WIDE (3)
        "ULTRA_NARROW"  // ULTRA_NARROW (4)
    };

    /**
     * @brief Threat level symbols (for compact display)
     */
    static constexpr char THREAT_SYMBOLS[ColorConstants::THREAT_LEVEL_COUNT] FLASH_STORAGE = {
        '-', 'i', 'O', '!', '!', '?'  // NONE, LOW, MEDIUM, HIGH, CRITICAL, UNKNOWN
    };

    /**
     * @brief Get threat level name (O(1) lookup with bounds checking)
     * @param level Threat level index (0-5)
     * @return String literal for the threat level
     */
    static constexpr const char* threat_name(uint8_t level) noexcept {
        if (level >= ColorConstants::THREAT_LEVEL_COUNT) {
            return THREAT_NAMES[ColorConstants::THREAT_LEVEL_COUNT - 1];  // "UNKNOWN"
        }
        return THREAT_NAMES[level];
    }

    /**
     * @brief Get drone type name (O(1) lookup with bounds checking)
     * @param type Drone type index (0-10)
     * @return String literal for the drone type
     */
    static constexpr const char* drone_type_name(uint8_t type) noexcept {
        if (type >= ColorConstants::DRONE_TYPE_COUNT) {
            return DRONE_TYPE_NAMES[0];  // "Unknown"
        }
        return DRONE_TYPE_NAMES[type];
    }

    /**
     * @brief Get spectrum mode name (O(1) lookup with bounds checking)
     * @param mode Spectrum mode index (0-4)
     * @return String literal for the spectrum mode
     */
    static constexpr const char* spectrum_mode_name(uint8_t mode) noexcept {
        if (mode >= ColorConstants::SPECTRUM_MODE_COUNT) {
            return SPECTRUM_MODE_NAMES[1];  // "MEDIUM"
        }
        return SPECTRUM_MODE_NAMES[mode];
    }

    /**
     * @brief Get threat level symbol (O(1) lookup with bounds checking)
     * @param level Threat level index (0-5)
     * @return Single character symbol for the threat level
     */
    static constexpr char threat_symbol(uint8_t level) noexcept {
        if (level >= ColorConstants::THREAT_LEVEL_COUNT) {
            return THREAT_SYMBOLS[ColorConstants::THREAT_LEVEL_COUNT - 1];  // '?'
        }
        return THREAT_SYMBOLS[level];
    }
};

// Compile-time assertions for array sizes
static_assert(sizeof(UnifiedStringLookup::THREAT_NAMES) == sizeof(const char*) * ColorConstants::THREAT_LEVEL_COUNT, 
              "THREAT_NAMES size");
static_assert(sizeof(UnifiedStringLookup::DRONE_TYPE_NAMES) == sizeof(const char*) * ColorConstants::DRONE_TYPE_COUNT, 
              "DRONE_TYPE_NAMES size");
static_assert(sizeof(UnifiedStringLookup::SPECTRUM_MODE_NAMES) == sizeof(const char*) * ColorConstants::SPECTRUM_MODE_COUNT, 
              "SPECTRUM_MODE_NAMES size");

} // namespace ui::apps::enhanced_drone_analyzer

#endif // COLOR_LOOKUP_UNIFIED_HPP_
