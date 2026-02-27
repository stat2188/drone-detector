// * Enhanced Drone Analyzer - Main Entry Point * Fixed: Section name matches external_app.ld (added 'app_' prefix)

// C++ standard library headers (alphabetical order)
// (none needed)

// Third-party library headers
// (none needed)

// Project-specific headers (alphabetical order)
#include "ui_enhanced_drone_analyzer.hpp"
#include "ui_navigation.hpp"

#ifndef VERSION_MD5
#define VERSION_MD5 1
#endif

namespace ui::apps::enhanced_drone_analyzer {

// TYPE ALIASES
using AppNavigationView = ui::NavigationView;

void initialize_app(AppNavigationView& nav) noexcept {
    nav.push<EnhancedDroneSpectrumAnalyzerView>();
}

} // namespace ui::apps::enhanced_drone_analyzer

// Remove extern "C" section as this is now an internal application
