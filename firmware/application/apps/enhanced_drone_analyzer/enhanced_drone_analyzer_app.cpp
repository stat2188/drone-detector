/*
 * Enhanced Drone Analyzer - Main Entry Point
 * Fixed: Section name matches external_app.ld (added 'app_' prefix)
 */

#include "ui_navigation.hpp"
#include "portapack.hpp"

#include "ui_enhanced_drone_analyzer.hpp"
#include "ui_drone_common_types.hpp"

#ifndef VERSION_MD5
#define VERSION_MD5 1
#endif

using namespace portapack;

namespace ui::apps::enhanced_drone_analyzer {

void initialize_app(ui::NavigationView& nav) {
    nav.push<EnhancedDroneSpectrumAnalyzerView>();
}

} // namespace ui::apps::enhanced_drone_analyzer

// Remove extern "C" section as this is now an internal application
