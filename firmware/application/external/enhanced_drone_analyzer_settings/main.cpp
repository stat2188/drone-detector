// enhanced_drone_analyzer_settings_main.cpp - Portapack app entry point for Settings App
// Integrates settings management with Portapack menu system

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"
#include "ui_settings_combined.hpp"
#include "../enhanced_drone_analyzer/shared_settings.hpp"

// Define VERSION_MD5 if not defined (build system should provide this)
#ifndef VERSION_MD5
#define VERSION_MD5 1
#endif

namespace ui::external_app::enhanced_drone_analyzer_settings {
void initialize_app(ui::NavigationView& nav) {
    // Ensure default drone database exists on SD card at startup
    ui::external_app::enhanced_drone_analyzer::EnhancedSettingsManager::ensure_database_exists();

    nav.push<ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettingsView>();
}
}  // namespace ui::external_app::enhanced_drone_analyzer_settings

extern "C" {

__attribute__((section(".external_app.app_enhanced_drone_analyzer_settings.application_information"), used)) application_information_t _application_information_enhanced_drone_analyzer_settings = {
    /*.memory_location = */ (uint8_t*)0xADF10000,
    /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer_settings::initialize_app,
    /*.header_version = */ CURRENT_HEADER_VERSION,
    /*.app_version = */ VERSION_MD5,

    /*.app_name = */ "EDA Settings",
    /*.bitmap_data = */ {
        0x9C, 0x00, 0x9C, 0x00, 0x00, 0x00, 0x9C, 0x00, 0x9C, 0x00, 0x00, 0x00, 0x63, 0x0F, 0xC0, 0x7F,
        0x0C, 0x18, 0x00, 0x01, 0x08, 0x10, 0x00, 0x01, 0x08, 0x10, 0x00, 0x01, 0x0C, 0x18, 0x00, 0x01
    },
    /*.icon_color = */ ui::Color::blue().v,
    /*.menu_location = */ app_location_t::SETTINGS,
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = portapack::spi_flash::image_tag_none */ {0, 0, 0, 0},
    /*.m4_app_offset = */ 0x00000000,  // will be filled at compile time
};
}
