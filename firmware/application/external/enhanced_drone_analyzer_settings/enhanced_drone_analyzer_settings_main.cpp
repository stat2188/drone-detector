#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_settings_combined.hpp"

#ifndef VERSION_MD5
#define VERSION_MD5 1
#endif

namespace ui::external_app::enhanced_drone_analyzer_settings {
    void initialize_app(ui::NavigationView& nav) {
        nav.push<ui::external_app::enhanced_drone_analyzer::DroneSettingsMainView>();
    }
}

extern "C" {
    __attribute__((section(".external_app.app_eda_settings.application_information"), used)) 
    application_information_t _application_information_enhanced_drone_analyzer_settings = {
        (uint8_t*)0x00000000,
        ui::external_app::enhanced_drone_analyzer_settings::initialize_app,
        CURRENT_HEADER_VERSION,
        VERSION_MD5,
        "EDA Settings",
        {
            0x00, 0x00, 0x3C, 0x3C, 0x42, 0x42, 0x81, 0x81, 
            0x81, 0x81, 0x42, 0x42, 0x3C, 0x3C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18,
            0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        ui::Color::grey().v,
        app_location_t::SETTINGS,
        -1,
        {0, 0, 0, 0},
        0x00000000,
    };
}
