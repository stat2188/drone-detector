// Enhanced Drone Analyzer - Main Entry Point
// Fixed: Section name matches external_app.ld (added 'app_' prefix)

#ifndef ENHANCED_DRONE_ANALYZER_APP_HPP_
#define ENHANCED_DRONE_ANALYZER_APP_HPP_

// C++ standard library headers (alphabetical order)
// (none needed)

// Third-party library headers
// (none needed)

// Project-specific headers (alphabetical order)
#include "ui_enhanced_drone_analyzer.hpp"
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
