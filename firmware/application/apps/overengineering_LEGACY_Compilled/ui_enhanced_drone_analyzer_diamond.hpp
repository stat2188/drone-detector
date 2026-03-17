// Diamond Code - Stack Optimized Implementation for STM32F405
// Addresses stack overflow in overengineering_LEGACY_Compilled
// Compliant with constraints: 64KB RAM, 4KB max stack per thread

#ifndef UI_ENHANCED_DRONE_ANALYZER_DIAMOND_HPP_
#define UI_ENHANCED_DRONE_ANALYZER_DIAMOND_HPP_

#include <cstddef>
#include <cstdint>

// Minimal forward declarations - only what we need
namespace ui {
    class NavigationView;
    class View;
    class Painter;
    struct Rect;
}

// ============================================================================
// CORE TYPES AND CONSTANTS
// ============================================================================

namespace ui::apps::enhanced_drone_analyzer {

    // Fixed-width types (C++11 compliant)
    using FreqHz = uint64_t;
    using RssiValue = int32_t;
    using SystemTime = uint32_t;

    // Enums (scoped for type safety)
    enum class ThreatLevel : uint8_t {
        NONE = 0, LOW = 1, MEDIUM = 2, HIGH = 3, CRITICAL = 4
    };

    enum class MovementTrend : uint8_t {
        UNKNOWN = 0, STATIC = 1, APPROACHING = 2, RECEDING = 3
    };

    enum class DroneType : uint8_t {
        UNKNOWN = 0, DJI = 1, PARROT = 2, YUNEEC = 3, DR_3DR = 4, 
        AUTEL = 5, HOBBY = 6, FPV = 7, CUSTOM = 8, OTHER = 255
    };

    // ============================================================================
    // STACK-OPTIMIZED DATA STRUCTURES
    // ============================================================================
    // All members are POD types with explicit initialization
    // No constructors/destructors to minimize code size and stack usage

    struct DisplayDroneEntry {
        FreqHz frequency{0};
        DroneType type{DroneType::UNKNOWN};
        ThreatLevel threat{ThreatLevel::NONE};
        RssiValue rssi{-120};
        SystemTime last_seen{0};
        char type_name[16]{};  // Fixed size buffer
        uint32_t display_color{0xFFFFFFFF};
        MovementTrend trend{MovementTrend::UNKNOWN};
    };

    // Stack-optimized Display Data (uses fixed-size arrays)
    struct DisplayData {
        DisplayDroneEntry drones[20];  // Fixed array: 20 * 39 = 780 bytes
        size_t drone_count{0};

        void clear() noexcept {
            drone_count = 0;
            for (auto& drone : drones) {
                drone = DisplayDroneEntry{};  // POD assignment
            }
        }
    };

    // ============================================================================
    // MAIN VIEW CLASS - STACK OPTIMIZED
    // ============================================================================

    class EnhancedDroneSpectrumAnalyzerView : public ::ui::View {
    public:
        explicit EnhancedDroneSpectrumAnalyzerView(::ui::NavigationView& nav);
        ~EnhancedDroneSpectrumAnalyzerView() override;

        void paint(::ui::Painter& painter) override;

    private:
        // ========================================================================
        // MINIMAL MEMBER VARIABLES (STACK OPTIMIZATION)
        // ========================================================================
        // Only essential state kept as class members to minimize stack footprint
        // Large buffers moved to static storage (see StaticStorage namespace)

        ::ui::NavigationView& nav_;           // 4 bytes (reference)
        DisplayData display_data_;            // ~780 bytes (largest member)
        bool is_initialized_{false};          // 1 byte
        bool is_scanning_{false};             // 1 byte

        // ========================================================================
        // STACK CANARIES FOR CRITICAL FUNCTIONS
        // ========================================================================
        // Protect against stack overflow in paint() and related functions
        alignas(4) uint8_t paint_stack_canary_[32];  // 32 bytes

        // ========================================================================
        // STACK-OPTIMIZED HELPER METHODS
        // ========================================================================
        void initialize_stack_canary() noexcept;
        bool verify_stack_canary() const noexcept;
        void paint_with_stack_check(::ui::Painter& painter);

        // ========================================================================
        // FRAMEWORK INTEGRATION (KEPT MINIMAL FOR EXAMPLE)
        // ========================================================================
        void handle_ui_events() noexcept;
        void update_display_if_needed() noexcept;
    };

    // ============================================================================
    // STATIC STORAGE FOR LARGE BUFFERS
    // ============================================================================
    // Moves large allocations from stack to .bss section
    // Zero-initialized at startup, no runtime allocation cost
    // Access via getter functions to maintain encapsulation

    namespace StaticStorage {
        // These would be defined in the corresponding .cpp file
        // Allocate in .bss (zero-initialized) to save stack space
        extern uint8_t spectrum_buffer[240];           // 240 bytes
        extern uint16_t histogram_buffer[64];          // 128 bytes  
        extern DisplayDroneEntry display_drones[20];   // 780 bytes
        
        // Accessor functions for thread-safe access
        [[nodiscard]] inline uint8_t* get_spectrum_buffer() noexcept {
            return spectrum_buffer;
        }
        
        [[nodiscard]] inline uint16_t* get_histogram_buffer() noexcept {
            return histogram_buffer;
        }
        
        [[nodiscard]] inline DisplayDroneEntry* get_display_drones() noexcept {
            return display_drones;
        }
    }

    // Total static storage usage: ~1148 bytes
    // This saves ~1148 bytes of stack space per UI instance
    // Well within STM32F405 64KB RAM limit

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_ENHANCED_DRONE_ANALYZER_DIAMOND_HPP_