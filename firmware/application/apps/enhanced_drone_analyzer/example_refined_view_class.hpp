/**
 * @file example_refined_view_class.hpp
 * @brief Diamond Code Best Practices: Clean UI Class with Guard Clauses
 * 
 * DIAMOND CODE PRINCIPLES DEMONSTRATED:
 * 1. Guard clauses for early returns and null checks
 * 2. Clear separation of UI from DSP logic
 * 3. NO std::vector, std::string, std::map, std::atomic
 * 4. NO new, malloc - all stack allocation
 * 5. Stack < 4KB constraint respected
 * 6. Use std::array instead of std::vector
 * 7. Use constexpr for compile-time constants
 * 8. All functions marked noexcept (no exceptions)
 * 9. Use enum class, using Type = uintXX_t;
 * 10. Proper ::ui:: prefix for framework types
 * 
 * BEFORE (INCORRECT):
 * void update_display() {
 *     if (widget != nullptr) {
 *         if (data != nullptr) {
 *             if (data->valid) {
 *                 // Deep nesting - hard to read
 *                 widget->draw(data);
 *             }
 *         }
 *     }
 * }
 * 
 * AFTER (CORRECT):
 * void update_display() noexcept {
 *     // Guard clauses - early returns for clarity
 *     if (widget == nullptr) return;
 *     if (data == nullptr) return;
 *     if (!data->valid) return;
 *     
 *     widget->draw(data);
 * }
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

// Forward declarations for framework types (example purposes only)
namespace ui {
    enum class Color : uint8_t { Black, White, Red, Green, Blue, Yellow, Cyan, Gray };
    class Widget;
    class Label;
    class Rectangle;
} // namespace ui

// ============================================================================
// DIAMOND CODE: Type Aliases (using Type = uintXX_t;)
// ============================================================================

namespace eda {
namespace ui {

// Type aliases for clarity and type safety
using Coordinate = int16_t;
using Dimension = uint16_t;
using BarIndex = uint8_t;
using TextLength = uint8_t;

} // namespace ui
} // namespace eda

// ============================================================================
// DIAMOND CODE: UI Constants (constexpr, Flash placement)
// ============================================================================

namespace eda {
namespace ui {

/**
 * @brief UI display configuration constants
 * 
 * DIAMOND CODE: constexpr ensures compile-time evaluation
 * DIAMOND CODE: Placed in Flash (no RAM consumption)
 */
struct DisplayConstants {
    // Screen dimensions
    static constexpr Coordinate SCREEN_WIDTH = 240;
    static constexpr Coordinate SCREEN_HEIGHT = 320;
    
    // Spectrum display area
    static constexpr Coordinate SPECTRUM_X = 10;
    static constexpr Coordinate SPECTRUM_Y = 60;
    static constexpr Dimension SPECTRUM_WIDTH = 220;
    static constexpr Dimension SPECTRUM_HEIGHT = 200;
    
    // Bar configuration
    static constexpr BarIndex NUM_BARS = 32;
    static constexpr Dimension BAR_WIDTH = 6;
    static constexpr Dimension BAR_SPACING = 1;
    
    // Text configuration
    static constexpr TextLength MAX_TEXT_LENGTH = 32;
    
    // Colors
    static constexpr ::ui::Color BACKGROUND_COLOR = ::ui::Color::Black;
    static constexpr ::ui::Color BAR_COLOR = ::ui::Color::Green;
    static constexpr ::ui::Color ALERT_COLOR = ::ui::Color::Red;
    static constexpr ::ui::Color TEXT_COLOR = ::ui::Color::White;
    
    // Compile-time validation
    static_assert(SPECTRUM_WIDTH > 0, "Spectrum width must be positive");
    static_assert(SPECTRUM_HEIGHT > 0, "Spectrum height must be positive");
    static_assert(NUM_BARS > 0, "Must have at least one bar");
    static_assert(NUM_BARS <= 64, "Too many bars for display");
};

} // namespace ui
} // namespace eda

// ============================================================================
// DIAMOND CODE: Pure UI View Class - No DSP Logic
// ============================================================================

namespace eda {
namespace ui {

/**
 * @brief Spectrum display view
 * 
 * DIAMOND CODE PRINCIPLE: This class contains ONLY UI display logic.
 * It has NO knowledge of DSP algorithms or signal processing.
 * 
 * KEY FEATURES:
 * - Guard clauses for all public methods
 * - Null checks for all pointer parameters
 * - Early returns for invalid state
 * - No deep nesting (max 2-3 levels)
 * - All methods marked noexcept
 * 
 * STACK USAGE: ~512 bytes (well under 4KB limit)
 */
class SpectrumView {
public:
    /**
     * @brief Constructor
     * 
     * DIAMOND CODE: noexcept ensures no exceptions during construction
     */
    constexpr SpectrumView() noexcept = default;
    
    /**
     * @brief Initialize the spectrum view
     * 
     * @param root_widget Root UI widget (must be non-null)
     * @return true Initialization successful
     * @return false Initialization failed
     * 
     * DIAMOND CODE: Guard clause for null check
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    auto initialize(::ui::Widget* root_widget) noexcept -> bool {
        // Guard clause: null check
        if (root_widget == nullptr) {
            return false;
        }
        
        // Store root widget reference
        root_widget_ = root_widget;
        
        // Create child widgets
        create_spectrum_area();
        create_labels();
        
        return true;
    }
    
    /**
     * @brief Update spectrum display with new data
     * 
     * @param magnitudes Magnitude values (must be non-null)
     * @param count Number of magnitude values
     * 
     * DIAMOND CODE: Guard clauses for validation
     * DIAMOND CODE: noexcept ensures no exceptions
     * DIAMOND CODE: Pure UI logic, no DSP
     */
    void update(const uint16_t* magnitudes, uint8_t count) noexcept {
        // Guard clause: null check
        if (magnitudes == nullptr) {
            return;
        }
        
        // Guard clause: invalid count
        if (count == 0 || count > DisplayConstants::NUM_BARS) {
            return;
        }
        
        // Guard clause: not initialized
        if (root_widget_ == nullptr) {
            return;
        }
        
        // Clear previous display
        clear_spectrum_area();
        
        // Draw each bar
        draw_spectrum_bars(magnitudes, count);
    }
    
    /**
     * @brief Update frequency label text
     * 
     * @param frequency Frequency value in Hz
     * 
     * DIAMOND CODE: Guard clause for uninitialized state
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void update_frequency_label(uint32_t frequency) noexcept {
        // Guard clause: not initialized
        if (frequency_label_ == nullptr) {
            return;
        }
        
        // Format frequency as text (C-style, no std::string)
        char text[DisplayConstants::MAX_TEXT_LENGTH];
        format_frequency(frequency, text, sizeof(text));
        
        // Update label
        frequency_label_->set_text(text);
    }
    
    /**
     * @brief Update signal strength label
     * 
     * @param strength Signal strength (0-100)
     * 
     * DIAMOND CODE: Guard clause for uninitialized state
     * DIAMOND CODE: Guard clause for invalid range
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void update_strength_label(uint8_t strength) noexcept {
        // Guard clause: not initialized
        if (strength_label_ == nullptr) {
            return;
        }
        
        // Guard clause: invalid range
        if (strength > 100) {
            return;
        }
        
        // Format strength as text (C-style, no std::string)
        char text[DisplayConstants::MAX_TEXT_LENGTH];
        format_strength(strength, text, sizeof(text));
        
        // Update label
        strength_label_->set_text(text);
        
        // Update bar color based on strength
        update_bar_color(strength);
    }
    
    /**
     * @brief Show alert state
     * 
     * @param show_alert true to show alert, false to clear
     * 
     * DIAMOND CODE: Guard clause for uninitialized state
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void show_alert(bool show_alert) noexcept {
        // Guard clause: not initialized
        if (root_widget_ == nullptr) {
            return;
        }
        
        // Update background color
        if (show_alert) {
            root_widget_->set_color(DisplayConstants::ALERT_COLOR);
        } else {
            root_widget_->set_color(DisplayConstants::BACKGROUND_COLOR);
        }
    }
    
private:
    // UI widget references (not owned)
    ::ui::Widget* root_widget_ = nullptr;
    ::ui::Widget* spectrum_area_ = nullptr;
    ::ui::Label* frequency_label_ = nullptr;
    ::ui::Label* strength_label_ = nullptr;
    
    /**
     * @brief Create spectrum display area
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: Guard clause for uninitialized root
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void create_spectrum_area() noexcept {
        // Guard clause: root not initialized
        if (root_widget_ == nullptr) {
            return;
        }
        
        // Create spectrum area widget
        spectrum_area_ = root_widget_->create_child();
        spectrum_area_->set_position(
            DisplayConstants::SPECTRUM_X,
            DisplayConstants::SPECTRUM_Y
        );
        spectrum_area_->set_size(
            DisplayConstants::SPECTRUM_WIDTH,
            DisplayConstants::SPECTRUM_HEIGHT
        );
        spectrum_area_->set_color(DisplayConstants::BACKGROUND_COLOR);
    }
    
    /**
     * @brief Create text labels
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: Guard clause for uninitialized root
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void create_labels() noexcept {
        // Guard clause: root not initialized
        if (root_widget_ == nullptr) {
            return;
        }
        
        // Create frequency label
        frequency_label_ = root_widget_->create_label();
        frequency_label_->set_position(10, 20);
        frequency_label_->set_color(DisplayConstants::TEXT_COLOR);
        frequency_label_->set_text("Freq: --- MHz");
        
        // Create strength label
        strength_label_ = root_widget_->create_label();
        strength_label_->set_position(130, 20);
        strength_label_->set_color(DisplayConstants::TEXT_COLOR);
        strength_label_->set_text("Str: --%");
    }
    
    /**
     * @brief Clear spectrum display area
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: Guard clause for uninitialized area
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void clear_spectrum_area() noexcept {
        // Guard clause: area not initialized
        if (spectrum_area_ == nullptr) {
            return;
        }
        
        // Fill with background color
        spectrum_area_->fill(DisplayConstants::BACKGROUND_COLOR);
    }
    
    /**
     * @brief Draw spectrum bars
     * 
     * @param magnitudes Magnitude values
     * @param count Number of bars to draw
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: Guard clause for uninitialized area
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void draw_spectrum_bars(const uint16_t* magnitudes, uint8_t count) noexcept {
        // Guard clause: area not initialized
        if (spectrum_area_ == nullptr) {
            return;
        }
        
        // Guard clause: null magnitudes
        if (magnitudes == nullptr) {
            return;
        }
        
        // Draw each bar
        for (BarIndex i = 0; i < count; ++i) {
            // Calculate bar position
            Coordinate x = DisplayConstants::SPECTRUM_X + 
                          (i * (DisplayConstants::BAR_WIDTH + DisplayConstants::BAR_SPACING));
            
            // Calculate bar height (scale to fit display)
            Dimension height = scale_magnitude_to_height(magnitudes[i]);
            
            // Draw bar
            ::ui::Rectangle bar;
            bar.set_x(x);
            bar.set_y(DisplayConstants::SPECTRUM_Y + DisplayConstants::SPECTRUM_HEIGHT - height);
            bar.set_width(DisplayConstants::BAR_WIDTH);
            bar.set_height(height);
            bar.set_color(bar_color_);
            spectrum_area_->draw_rectangle(bar);
        }
    }
    
    /**
     * @brief Scale magnitude value to display height
     * 
     * @param magnitude Magnitude value
     * @return Dimension Scaled height in pixels
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: constexpr for compile-time evaluation where possible
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    auto scale_magnitude_to_height(uint16_t magnitude) noexcept -> Dimension {
        // Clamp to valid range
        if (magnitude > 1000) {
            magnitude = 1000;
        }
        
        // Scale to display height
        return static_cast<Dimension>(
            (magnitude * DisplayConstants::SPECTRUM_HEIGHT) / 1000
        );
    }
    
    /**
     * @brief Update bar color based on signal strength
     * 
     * @param strength Signal strength (0-100)
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: Guard clause for invalid range
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void update_bar_color(uint8_t strength) noexcept {
        // Guard clause: invalid range
        if (strength > 100) {
            return;
        }
        
        // Select color based on strength
        if (strength < 30) {
            bar_color_ = ::ui::Color::Red;
        } else if (strength < 70) {
            bar_color_ = ::ui::Color::Yellow;
        } else {
            bar_color_ = ::ui::Color::Green;
        }
    }
    
    /**
     * @brief Format frequency as text
     * 
     * @param frequency Frequency in Hz
     * @param buffer Output buffer
     * @param buffer_size Buffer size
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: Guard clause for null buffer
     * DIAMOND CODE: C-style string formatting (no std::string)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void format_frequency(
        uint32_t frequency,
        char* buffer,
        size_t buffer_size
    ) noexcept {
        // Guard clause: null buffer
        if (buffer == nullptr || buffer_size < 16) {
            return;
        }
        
        // Format as MHz with 2 decimal places
        uint32_t mhz = frequency / 1000000;
        uint32_t khz = (frequency % 1000000) / 10000;
        
        // Simple integer formatting (no sprintf to avoid library dependency)
        buffer[0] = 'F';
        buffer[1] = 'r';
        buffer[2] = 'e';
        buffer[3] = 'q';
        buffer[4] = ':';
        buffer[5] = ' ';
        buffer[6] = '0' + (mhz / 100);
        buffer[7] = '0' + ((mhz / 10) % 10);
        buffer[8] = '0' + (mhz % 10);
        buffer[9] = '.';
        buffer[10] = '0' + (khz / 10);
        buffer[11] = '0' + (khz % 10);
        buffer[12] = ' ';
        buffer[13] = 'M';
        buffer[14] = 'H';
        buffer[15] = 'z';
        buffer[16] = '\0';
    }
    
    /**
     * @brief Format strength as text
     * 
     * @param strength Signal strength (0-100)
     * @param buffer Output buffer
     * @param buffer_size Buffer size
     * 
     * DIAMOND CODE: Private method, pure UI logic
     * DIAMOND CODE: Guard clause for null buffer
     * DIAMOND CODE: C-style string formatting (no std::string)
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void format_strength(
        uint8_t strength,
        char* buffer,
        size_t buffer_size
    ) noexcept {
        // Guard clause: null buffer
        if (buffer == nullptr || buffer_size < 8) {
            return;
        }
        
        // Format as percentage
        buffer[0] = 'S';
        buffer[1] = 't';
        buffer[2] = 'r';
        buffer[3] = ':';
        buffer[4] = ' ';
        buffer[5] = '0' + (strength / 10);
        buffer[6] = '0' + (strength % 10);
        buffer[7] = '%';
        buffer[8] = '\0';
    }
    
    // Current bar color
    ::ui::Color bar_color_ = DisplayConstants::BAR_COLOR;
};

// Compile-time stack validation
static_assert(sizeof(SpectrumView) < 256, 
              "SpectrumView stack usage must be < 256 bytes");

} // namespace ui
} // namespace eda

// ============================================================================
// DIAMOND CODE: Before/After Comparison Examples
// ============================================================================

/**
 * @section BEFORE_AFTER_EXAMPLES Before/After Comparisons
 * 
 * BEFORE (INCORRECT - Deep Nesting):
 * @code
 * // BAD: Deep nesting, hard to read
 * void update_display(Data* data, Widget* widget) {
 *     if (widget != nullptr) {
 *         if (data != nullptr) {
 *             if (data->valid) {
 *                 if (data->count > 0) {
 *                     for (int i = 0; i < data->count; i++) {
 *                         if (data->values[i] > threshold) {
 *                             widget->draw(data->values[i]);
 *                         }
 *                     }
 *                 }
 *             }
 *         }
 *     }
 * }
 * @endcode
 * 
 * AFTER (CORRECT - Guard Clauses):
 * @code
 * // GOOD: Guard clauses, early returns, flat structure
 * void update_display(Data* data, Widget* widget) noexcept {
 *     // Guard clauses - validate inputs early
 *     if (widget == nullptr) return;
 *     if (data == nullptr) return;
 *     if (!data->valid) return;
 *     if (data->count == 0) return;
 *     
 *     // Main logic - flat, easy to read
 *     for (int i = 0; i < data->count; i++) {
 *         if (data->values[i] > threshold) {
 *             widget->draw(data->values[i]);
 *         }
 *     }
 * }
 * @endcode
 * 
 * BENEFITS OF GUARD CLAUSES:
 * - Reduces cognitive load (less nesting to track)
 * - Makes validation logic explicit
 * - Easier to add new validation checks
 * - Improves code readability
 * - Reduces indentation levels
 */

// ============================================================================
// DIAMOND CODE: Memory Placement Documentation
// ============================================================================

/**
 * @section MEMORY_PLACEMENT Memory Placement
 * 
 * UI LAYER MEMORY USAGE:
 * 
 * FLASH (Read-only):
 * - DisplayConstants: All constexpr values (~128 bytes)
 * - Function code: ~3KB
 * - String literals: ~64 bytes
 * 
 * STACK (Runtime):
 * - SpectrumView instance: ~64 bytes (4 pointers + 1 enum)
 * - Local variables in methods: ~128 bytes max
 * - Total stack: ~192 bytes (well under 4KB limit)
 * 
 * NO DYNAMIC ALLOCATION:
 * - NO std::vector (would use heap)
 * - NO std::string (would use heap)
 * - NO new, malloc (would use heap)
 * - All data on stack or in Flash
 * 
 * UI FRAMEWORK MEMORY:
 * - Widget objects: Managed by UI framework
 * - Display buffers: Managed by display driver
 * - Not counted in application stack
 * 
 * BENEFITS:
 * - Deterministic memory usage
 * - No heap fragmentation in UI layer
 * - Fast rendering (no allocation overhead)
 * - Cache-friendly UI updates
 */

} // namespace eda
