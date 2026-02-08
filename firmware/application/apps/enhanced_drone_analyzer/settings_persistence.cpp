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

} // namespace ui::apps::enhanced_drone_analyzer