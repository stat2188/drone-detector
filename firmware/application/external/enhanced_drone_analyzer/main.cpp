/*
 * Enhanced Drone Analyzer - Main Entry Point
 * Fixed: Section name matches external_app.ld (added 'app_' prefix)
 */

#include "ui_navigation.hpp"
#include "external_app.hpp"
#include "portapack.hpp"

#include "ui_scanner_combined.hpp"

#ifndef VERSION_MD5
#define VERSION_MD5 1
#endif

using namespace portapack;

namespace ui::external_app::enhanced_drone_analyzer {

void initialize_app(ui::NavigationView& nav) {
    nav.push<EnhancedDroneSpectrumAnalyzerView>();
}

} // namespace ui::external_app::enhanced_drone_analyzer

extern "C" {

// !!! ИСПРАВЛЕНИЕ ЗДЕСЬ !!!
// Добавлен префикс 'app_' после .external_app.
// Было: .external_app.enhanced_drone_analyzer...
// Стало: .external_app.app_enhanced_drone_analyzer...
__attribute__((section(".external_app.app_enhanced_drone_analyzer.application_information"), used)) 
application_information_t enhanced_drone_analyzer_application_information = {
    
    /*.memory_location = */ (uint8_t*)0x00000000,
    
    /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer::initialize_app, 
    
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "EDA Scanner",

    /*.bitmap_data = */ {
        0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 0xF0, 0x0F, 0xF8, 0x1F,
        0xFC, 0x3F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFC, 0x3F, 0xF8, 0x1F, 0xF0, 0x0F,
        0xE0, 0x07, 0xC0, 0x03, 0x80, 0x01, 0x00, 0x00
    },

    /*.icon_color = */ ui::Color::red().v,
    /*.menu_location = */ app_location_t::RX,
    /*.desired_menu_position = */ 1,

    /*.m4_app_tag = */ { 'P', 'W', 'F', 'M' },

    /*.m4_app_offset = */ 0x00000000 
};

}