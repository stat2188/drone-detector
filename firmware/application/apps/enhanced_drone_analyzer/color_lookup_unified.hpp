/**
 * 🎨 UNIFIED COLOR LOOKUP - Fixed version
 * 
 * IMPROVEMENTS:
 * - ✅ Correct RGB888 → RGB565 conversion
 * - ✅ Eliminated duplication (HEADER_STYLES, CARD_STYLES)
 * - ✅ DRONE_COLORS updated from 8 → 11 types
 * - ✅ All colors stored in Flash (constexpr)
 * 
 * ⚡ O(1) lookup instead of switch/if-else
 * 💾 RAM: 0 bytes (all data in Flash)
 * 
 * @author Optimized via Diamond Core Protocol
 */

#ifndef COLOR_LOOKUP_UNIFIED_HPP_
#define COLOR_LOOKUP_UNIFIED_HPP_

#include <cstdint>
#include "ui.hpp"  // Для Color class

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// RGB888 → RGB565 CONVERTER (constexpr)
// ===========================================
// Color format (ui.hpp): rrrrrGGGGGGbbbbb (RGB565, 16-bit)
// LUT format: 0xRRGGBB (RGB888, 24-bit)
// 
// R888 → R565: trunc 3 MSB bits (8 → 5)
// G888 → G665: trunc 2 MSB bits (8 → 6)  
// B888 → B565: trunc 3 MSB bits (8 → 5)

struct ColorConverter {
    // ✅ constexpr RGB888 → RGB565 conversion (O(1))
    static constexpr uint16_t rgb888_to_rgb565(uint32_t rgb888) {
        uint8_t r = (rgb888 >> 16) & 0xFF;
        uint8_t g = (rgb888 >> 8) & 0xFF;
        uint8_t b = rgb888 & 0xFF;
        
        // RGB565 format: rrrrrGGGGGGbbbbb
        uint16_t r565 = (r & 0xF8) << 8;    // 5 bits R (bits 11-15)
        uint16_t g565 = (g & 0xFC) << 3;    // 6 bits G (bits 5-10)
        uint16_t b565 = (b & 0xF8) >> 3;    // 5 bits B (bits 0-4)
        
        return r565 | g565 | b565;
    }
    
    // ✅ constexpr wrapper for Color (O(1))
    static constexpr Color rgb888_to_color(uint32_t rgb888) {
        return Color(rgb888_to_rgb565(rgb888));
    }
};

// ===========================================
// UNIFIED THREAT COLORS (6 levels)
// ===========================================
// Eliminates duplication: HEADER_STYLES + CARD_STYLES + THREAT_COLORS
// Stored in Flash (constexpr), 0 bytes RAM

struct ThreatColorLUT {
    static constexpr uint16_t COLORS[6] = {
        ColorConverter::rgb888_to_rgb565(0xFF0000),   // Red - NONE (0)
        ColorConverter::rgb888_to_rgb565(0x00FF00),   // Green - LOW (1)
        ColorConverter::rgb888_to_rgb565(0xFFFF00),   // Yellow - MEDIUM (2)
        ColorConverter::rgb888_to_rgb565(0xFFA500),   // Orange - HIGH (3)
        ColorConverter::rgb888_to_rgb565(0x800000),   // Dark Red - CRITICAL (4)
        ColorConverter::rgb888_to_rgb565(0x808080)    // Grey - UNKNOWN (5)
    };
    static_assert(sizeof(COLORS) == sizeof(uint16_t) * 6, "COLORS size mismatch");
    
    // Card styles (background + text) - UNIFIED
    struct CardStyle {
        uint16_t bg_color;
        uint16_t text_color;
    };
    
    static constexpr CardStyle CARD_STYLES[5] = {
        {ColorConverter::rgb888_to_rgb565(0x000040), ColorConverter::rgb888_to_rgb565(0xFFFFFF)},  // Dark Blue bg, White text - NONE (0)
        {ColorConverter::rgb888_to_rgb565(0x002000), ColorConverter::rgb888_to_rgb565(0xFFFFFF)},  // Dark Green bg, White text - LOW (1)
        {ColorConverter::rgb888_to_rgb565(0x202000), ColorConverter::rgb888_to_rgb565(0x000000)},  // Dark Yellow bg, Black text - MEDIUM (2)
        {ColorConverter::rgb888_to_rgb565(0x402000), ColorConverter::rgb888_to_rgb565(0xFFFFFF)},  // Dark Orange bg, White text - HIGH (3)
        {ColorConverter::rgb888_to_rgb565(0x400000), ColorConverter::rgb888_to_rgb565(0xFFFFFF)}   // Dark Red bg, White text - CRITICAL (4)
    };
    static_assert(sizeof(CARD_STYLES) == sizeof(CardStyle) * 5, "CARD_STYLES size mismatch");
    
    // O(1) lookup functions (inline, no branching)
    static inline Color threat_color(uint8_t level) {
        uint8_t idx = (level < 6) ? level : 5;
        return Color(COLORS[idx]);
    }
    
    static inline Color card_bg_color(uint8_t threat) {
        uint8_t idx = (threat < 5) ? threat : 4;
        return Color(CARD_STYLES[idx].bg_color);
    }
    
    static inline Color card_text_color(uint8_t threat) {
        uint8_t idx = (threat < 5) ? threat : 4;
        return Color(CARD_STYLES[idx].text_color);
    }
};

// ===========================================
// UNIFIED DRONE COLORS (11 types - FIXED!)
// ===========================================
// FIXED: DRONE_COLORS[8] → DRONE_COLORS[11]
// Matches DroneType enum (ui_drone_common_types.hpp)

struct DroneColorLUT {
    static constexpr uint16_t COLORS[11] = {
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
    static_assert(sizeof(COLORS) == sizeof(uint16_t) * 11, "COLORS size mismatch");
    
    static inline Color drone_color(uint8_t type) {
        uint8_t idx = (type < 11) ? type : 0;
        return Color(COLORS[idx]);
    }
};

// ===========================================
// UNIFIED COLOR ACCESS
// ===========================================
// Combines ThreatColorLUT and DroneColorLUT

struct UnifiedColorLookup {
    // 🎯 Threat level colors
    static inline Color threat(uint8_t level) {
        return ThreatColorLUT::threat_color(level);
    }
    
    // 🎯 Drone type colors
    static inline Color drone(uint8_t type) {
        return DroneColorLUT::drone_color(type);
    }
    
    // 🎯 Card styles (bg + text)
    static inline Color card_bg(uint8_t threat) {
        return ThreatColorLUT::card_bg_color(threat);
    }
    
    static inline Color card_text(uint8_t threat) {
        return ThreatColorLUT::card_text_color(threat);
    }
    
    // 🎯 Header bar color (used in SmartThreatHeader)
    static inline Color header_bar(uint8_t threat) {
        // For header use threat_color (bright display)
        return ThreatColorLUT::threat_color(threat);
    }
};

// ===========================================
// UNIFIED STRING LOOKUP TABLES (SSOT)
// ===========================================
// Eliminates duplicates from: diamond_core.hpp, eda_optimized_utils.hpp, ui_enhanced_drone_settings.cpp
// Correctly sized for all enum values

struct UnifiedStringLookup {
    // Threat level names (6 levels: matches ThreatLevel enum)
    static constexpr const char* const THREAT_NAMES[6] = {
        "NONE",       // NONE (0)
        "LOW",        // LOW (1)
        "MEDIUM",     // MEDIUM (2)
        "HIGH",       // HIGH (3)
        "CRITICAL",   // CRITICAL (4)
        "UNKNOWN"     // UNKNOWN (5)
    };

    // Drone type names (11 types: matches DroneType enum)
    static constexpr const char* const DRONE_TYPE_NAMES[11] = {
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

    // Spectrum mode names (5 modes: matches SpectrumMode enum)
    static constexpr const char* const SPECTRUM_MODE_NAMES[5] = {
        "NARROW",       // NARROW (0)
        "MEDIUM",       // MEDIUM (1)
        "WIDE",         // WIDE (2)
        "ULTRA_WIDE",   // ULTRA_WIDE (3)
        "ULTRA_NARROW"  // ULTRA_NARROW (4)
    };

    // Threat level symbols (for compact display)
    static constexpr char THREAT_SYMBOLS[6] = {
        '-', 'i', 'O', '!', '!', '?'  // NONE, LOW, MEDIUM, HIGH, CRITICAL, UNKNOWN
    };

    // O(1) lookup functions
    static constexpr const char* threat_name(uint8_t level) {
        return (level < 6) ? THREAT_NAMES[level] : "UNKNOWN";
    }

    static constexpr const char* drone_type_name(uint8_t type) {
        return (type < 11) ? DRONE_TYPE_NAMES[type] : "Unknown";
    }

    static constexpr const char* spectrum_mode_name(uint8_t mode) {
        return (mode < 5) ? SPECTRUM_MODE_NAMES[mode] : "MEDIUM";
    }

    static constexpr char threat_symbol(uint8_t level) {
        return (level < 6) ? THREAT_SYMBOLS[level] : '?';
    }
};
static_assert(sizeof(UnifiedStringLookup::THREAT_NAMES) == sizeof(const char*) * 6, "THREAT_NAMES size");
static_assert(sizeof(UnifiedStringLookup::DRONE_TYPE_NAMES) == sizeof(const char*) * 11, "DRONE_TYPE_NAMES size");
static_assert(sizeof(UnifiedStringLookup::SPECTRUM_MODE_NAMES) == sizeof(const char*) * 5, "SPECTRUM_MODE_NAMES size");

} // namespace ui::apps::enhanced_drone_analyzer

#endif // COLOR_LOOKUP_UNIFIED_HPP_
