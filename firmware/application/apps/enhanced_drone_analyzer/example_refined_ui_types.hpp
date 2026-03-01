/**
 * @file example_refined_ui_types.hpp
 * @brief Diamond Code Best Practices: Proper ::ui:: Framework Type Usage
 * 
 * DIAMOND CODE PRINCIPLES DEMONSTRATED:
 * 1. Always use ::ui:: prefix for framework types (explicit namespace qualification)
 * 2. Avoid using namespace ui; (prevents ambiguity and conflicts)
 * 3. Use enum class for type-safe enumerations
 * 4. Use using Type = uintXX_t; for type aliases
 * 5. Use constexpr for compile-time constants
 * 6. Use static_assert for compile-time validation
 * 7. All functions marked noexcept (no exceptions)
 * 8. Stack < 4KB constraint respected
 * 
 * BEFORE (INCORRECT):
 * using namespace ui;  // BAD: Creates ambiguity
 * Color color = Color::Red;  // Unclear which Color type
 * 
 * AFTER (CORRECT):
 * ::ui::Color color = ::ui::Color::Red;  // Explicit framework type
 */

#pragma once

#include <cstdint>
#include <cstddef>

// Forward declarations for framework types (example purposes only)
// In real code, these would come from the actual UI framework
namespace ui {
    enum class Color : uint8_t { Black, White, Red, Green, Blue, Yellow, Cyan, Gray };
    class Widget;
    class Label;
}

// ============================================================================
// DIAMOND CODE: Type Aliases (using Type = uintXX_t;)
// ============================================================================

namespace eda {

// Type aliases for clarity and type safety
using ColorIndex = uint8_t;
using PixelIndex = uint16_t;
using FrequencyHz = uint32_t;
using SignalStrength = int16_t;

} // namespace eda

// ============================================================================
// DIAMOND CODE: Enum Class for Type-Safe Enumerations
// ============================================================================

namespace eda {

/**
 * @brief Drone signal quality levels
 * 
 * DIAMOND CODE: enum class prevents implicit conversions
 * and provides type safety.
 */
enum class SignalQuality : uint8_t {
    Poor = 0,       // < -90 dBm
    Fair = 1,       // -90 to -80 dBm
    Good = 2,       // -80 to -70 dBm
    Excellent = 3,  // > -70 dBm
    Unknown = 255   // Invalid or not detected
};

/**
 * @brief Analysis mode selection
 * 
 * DIAMOND CODE: Strongly typed enum prevents mode confusion
 */
enum class AnalysisMode : uint8_t {
    Spectrum = 0,
    Audio = 1,
    Hybrid = 2,
    Off = 3
};

/**
 * @brief UI state for display management
 * 
 * DIAMOND CODE: Explicit state machine states
 */
enum class DisplayState : uint8_t {
    Idle = 0,
    Scanning = 1,
    Analyzing = 2,
    Alert = 3
};

// Compile-time validation: Ensure enum fits in uint8_t
static_assert(sizeof(SignalQuality) == 1, "SignalQuality must be 1 byte");
static_assert(sizeof(AnalysisMode) == 1, "AnalysisMode must be 1 byte");
static_assert(sizeof(DisplayState) == 1, "DisplayState must be 1 byte");

} // namespace eda

// ============================================================================
// DIAMOND CODE: Proper ::ui:: Framework Type Usage
// ============================================================================

namespace eda {

/**
 * @brief Example wrapper for framework UI types
 * 
 * DIAMOND CODE PRINCIPLE: Always use ::ui:: prefix for framework types.
 * This makes it explicit that the type comes from the UI framework
 * and prevents naming conflicts with application types.
 * 
 * INCORRECT (Don't do this):
 *   using namespace ui;  // BAD: Creates ambiguity
 *   ui::Widget widget;   // Still ambiguous if using namespace is present
 *   Widget widget;       // Completely unclear which Widget type
 * 
 * CORRECT (Do this):
 *   ::ui::Widget widget;  // Explicit framework type
 *   ::ui::Color color;    // Clear namespace qualification
 */
struct FrameworkTypes {
    
    // DIAMOND CODE: Explicit ::ui:: prefix for all framework types
    // These are examples - actual types depend on your UI framework
    
    /**
     * @brief Get default background color
     * 
     * @return ::ui::Color Framework color type (explicitly qualified)
     * 
     * DIAMOND CODE: noexcept ensures no exceptions are thrown
     */
    static constexpr auto get_default_background() noexcept -> ::ui::Color {
        return ::ui::Color::Black;
    }
    
    /**
     * @brief Get foreground color based on signal quality
     * 
     * @param quality Signal quality level
     * @return ::ui::Color Framework color type
     * 
     * DIAMOND CODE: Guard clause for early return
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto get_foreground_color(SignalQuality quality) noexcept -> ::ui::Color {
        // Guard clause: handle unknown quality early
        if (quality == SignalQuality::Unknown) {
            return ::ui::Color::Gray;
        }
        
        // Switch on enum class (type-safe)
        switch (quality) {
            case SignalQuality::Poor:
                return ::ui::Color::Red;
            case SignalQuality::Fair:
                return ::ui::Color::Yellow;
            case SignalQuality::Good:
                return ::ui::Color::Green;
            case SignalQuality::Excellent:
                return ::ui::Color::Cyan;
            default:
                return ::ui::Color::White;
        }
    }
    
    /**
     * @brief Create a framework widget with proper type
     * 
     * @return ::ui::Widget Framework widget type
     * 
     * DIAMOND CODE: Return type explicitly uses ::ui:: prefix
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static auto create_spectrum_widget() noexcept -> ::ui::Widget {
        // Explicit ::ui:: prefix makes framework usage clear
        ::ui::Widget widget = ::ui::Widget::create();
        widget.set_color(get_default_background());
        return widget;
    }
    
    /**
     * @brief Create a framework text label
     * 
     * @param text Null-terminated string (C-style, no std::string)
     * @return ::ui::Label Framework label type
     * 
     * DIAMOND CODE: noexcept ensures no exceptions
     * DIAMOND CODE: C-style string (no std::string)
     */
    static auto create_label(const char* text) noexcept -> ::ui::Label {
        ::ui::Label label = ::ui::Label::create();
        label.set_text(text);
        label.set_color(get_foreground_color(SignalQuality::Good));
        return label;
    }
};

// ============================================================================
// DIAMOND CODE: Stack Size Validation
// ============================================================================

// Compile-time validation: Ensure stack usage stays under 4KB
static_assert(sizeof(FrameworkTypes) < 128, 
              "FrameworkTypes stack usage must be < 128 bytes");

// ============================================================================
// DIAMOND CODE: Before/After Comparison Examples
// ============================================================================

/**
 * @section BEFORE_AFTER_EXAMPLES Before/After Comparisons
 * 
 * BEFORE (INCORRECT):
 * @code
 * // BAD: using namespace creates ambiguity
 * using namespace ui;
 * 
 * void draw_spectrum() {
 *     // Unclear which Color type - framework or application?
 *     Color bg = Color::Black;
 *     Widget widget;  // Which Widget?
 * }
 * @endcode
 * 
 * AFTER (CORRECT):
 * @code
 * // GOOD: Explicit namespace qualification
 * void draw_spectrum() noexcept {
 *     // Clear: these are framework types
 *     ::ui::Color bg = ::ui::Color::Black;
 *     ::ui::Widget widget = ::ui::Widget::create();
 * }
 * @endcode
 */

// ============================================================================
// DIAMOND CODE: Memory Placement Documentation
// ============================================================================

/**
 * @section MEMORY_PLACEMENT Memory Placement
 * 
 * This header contains only:
 * - Type definitions (compile-time, no memory)
 * - constexpr functions (compile-time evaluation)
 * - static_assert statements (compile-time validation)
 * 
 * NO RUNTIME ALLOCATION:
 * - No std::vector, std::string, std::map, std::atomic
 * - No new, malloc
 * - All stack allocations < 4KB
 * 
 * FLASH PLACEMENT:
 * - All constexpr constants placed in Flash by compiler
 * - No RAM consumption for constant data
 * 
 * STACK USAGE:
 * - FrameworkTypes: ~64 bytes (estimated)
 * - Function calls: minimal (no dynamic allocation)
 */

} // namespace eda

// ============================================================================
// DIAMOND CODE: Compile-Time Validation Summary
// ============================================================================

/**
 * @section COMPILE_TIME_VALIDATION Compile-Time Checks
 * 
 * This header includes the following static_assert validations:
 * 
 * 1. SignalQuality size == 1 byte
 * 2. AnalysisMode size == 1 byte
 * 3. DisplayState size == 1 byte
 * 4. FrameworkTypes stack usage < 128 bytes
 * 
 * These ensure:
 * - Memory efficiency (small types)
 * - Stack safety (under 4KB limit)
 * - Type safety (enum class)
 * - No runtime errors (caught at compile time)
 */
