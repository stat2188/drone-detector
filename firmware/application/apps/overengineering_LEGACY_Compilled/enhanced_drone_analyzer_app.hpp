/**
 * @file enhanced_drone_analyzer_app.hpp
 * @brief Enhanced Drone Analyzer - Main Entry Point
 *
 * PHASE 5 MIGRATION - UI LAYER
 * Migrated from LEGACY/ to main enhanced_drone_analyzer/ directory
 *
 * DIAMOND CODE COMPLIANCE:
 * - Stack allocation only (max 4KB stack per thread on STM32F405)
 * - No heap allocation (no new, malloc, std::vector, std::map)
 * - Uses constexpr, enum class, using Type = uintXX_t
 * - No magic numbers (all constants defined)
 * - Zero-Overhead and Data-Oriented Design principles
 *
 * INITIALIZATION SEQUENCE:
 * This file is the application entry point called from main.cpp:
 * - System Initialization (in main.cpp): halInit() → chSysInit() → initialize_eda_mutexes() → initialize_app()
 * - Application Initialization: initialize_app() pushes EnhancedDroneSpectrumAnalyzerView onto navigation stack
 *
 * FRAMEWORK EXCEPTIONS:
 * - None - This file only uses framework navigation types
 *
 * INTERNAL DATA STRUCTURES:
 * - No internal data structures (header-only implementation)
 * - Uses inline function to avoid stack allocation
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 * Environment: ChibiOS RTOS / PortaPack UI Framework
 */

// Enhanced Drone Analyzer - Main Entry Point
// Fixed: Section name matches external_app.ld (added 'app_' prefix)

#ifndef ENHANCED_DRONE_ANALYZER_APP_HPP_
#define ENHANCED_DRONE_ANALYZER_APP_HPP_

// C++ standard library headers (alphabetical order)
// (none needed)

// Third-party library headers
// (none needed)

// Project-specific headers (alphabetical order)
#include "ui_navigation.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// TYPE ALIASES
// DIAMOND FIX: Use fully qualified name with global scope operator (::)
// to prevent namespace pollution. The '::ui::NavigationView' explicitly
// references global 'ui' namespace, not nested namespace.
using AppNavigationView = ::ui::NavigationView;

/**
 * @brief Initialize the Enhanced Drone Analyzer application
 * @param nav Navigation view to push the analyzer view onto
 * 
 * This function pushes the EnhancedDroneSpectrumAnalyzerView onto the
 * navigation stack, making it the active view.
 * 
 * @note Merged from enhanced_drone_analyzer_app.cpp
 * @note Header-only implementation for simplicity
 */
inline void initialize_app(AppNavigationView& nav) noexcept {
    nav.push<::ui::apps::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView>();
}

} // namespace ui::apps::enhanced_drone_analyzer

#endif // ENHANCED_DRONE_ANALYZER_APP_HPP_
