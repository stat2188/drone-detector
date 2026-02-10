/**
 * Diamond-Optimized Settings Persistence Implementation
 * 
 * Explicit template instantiation for DroneAnalyzerSettings
 * Eliminates duplicate managers: ScannerSettingsManager, DroneAnalyzerSettingsManager
 */

#include "settings_persistence.hpp"
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Explicit template instantiation for DroneAnalyzerSettings
template class SettingsPersistence<DroneAnalyzerSettings>;

// DIAMOND FIX: Static buffer definition (one definition per translation unit)
// Saves ~4KB of stack space during save operations
char SettingsStaticBuffer::buffer[SettingsStaticBuffer::SIZE] = {0};

} // namespace ui::apps::enhanced_drone_analyzer