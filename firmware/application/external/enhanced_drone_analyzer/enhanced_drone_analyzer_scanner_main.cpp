/*
 * Enhanced Drone Analyzer Scanner App - Modular Implementation
 * Phase 7: TXT file communication - scanner reads configuration
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// Fixed include paths to use proper relative paths from EDA directory
#include "../../common/ui.hpp"
#include "../../ui_navigation.hpp"
#include "../../common/external_app.hpp"
#include "../file.hpp"  // For file I/O operations (Phase 7: TXT communication)
#include "../database.hpp"     // For configuration storage patterns
#include <string>           // For std::string operations
#include <vector>           // For parameter parsing
#include <algorithm>        // For trim operations
#include <sstream>          // For std::istringstream

#include "ui_drone_common_types.hpp"
#include "ui_drone_audio.hpp"
#include "ui_signal_processing.hpp"
#include "scanner_settings.hpp"
#include "ui_scanner_combined.hpp"
#include "portapack.hpp"

// Define VERSION_MD5 if not defined (build system should provide this)
#ifndef VERSION_MD5
#define VERSION_MD5 1
#endif

using namespace portapack;

namespace ui::external_app::enhanced_drone_analyzer {

// Classes are defined in included headers - no forward declarations needed



// CORRECTED: Remove blocking operations from entry point (Critical Fix #1)
void initialize_app(ui::NavigationView& nav) {
    // Only push main view - all logic moved to UI constructor
    nav.push<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView>();
}
}  // namespace ui::external_app::enhanced_drone_analyzer

// Correct namespace for application entry point
namespace ui::external_app::enhanced_drone_analyzer_scanner {
void initialize_app(ui::NavigationView& nav) {
    // Delegate to the actual implementation
    ui::external_app::enhanced_drone_analyzer::initialize_app(nav);
}
}  // namespace ui::external_app::enhanced_drone_analyzer_scanner

extern "C" {

__attribute__((section(".external_app.app_enhanced_drone_analyzer_scanner.application_information"), used)) application_information_t _application_information_enhanced_drone_analyzer_scanner = {
    /*.memory_location = */ (uint8_t*)0x00000000,
    /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer_scanner::initialize_app,
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
    /*.desired_menu_position = */ -1,

    /*.m4_app_tag = */ portapack::spi_flash::image_tag_wideband_spectrum,
    /*.m4_app_offset = */ 0x00000000,  // will be filled at compile time
};
}
