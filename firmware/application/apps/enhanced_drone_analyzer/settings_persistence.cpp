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

// DIAMOND FIX: Settings Buffer Mutex Definition
Mutex settings_buffer_mutex;

// Phase 1 Optimization: Eliminated SettingsStaticBuffer (4KB) by writing directly to file
// Saves ~2.4 KB of static RAM - see settings_persistence.hpp:456

// DIAMOND FIX: Load buffer now uses instance members in get_load_buffer()
// Saves ~337 bytes of stack space during load operations

} // namespace ui::apps::enhanced_drone_analyzer