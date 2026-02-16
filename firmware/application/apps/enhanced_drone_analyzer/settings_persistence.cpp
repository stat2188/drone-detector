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

// DIAMOND FIX: Static load buffer definitions to prevent stack overflow
// Saves ~337 bytes of stack space during load operations
char SettingsLoadBuffer::line_buffer[SettingsLoadBuffer::LINE_BUFFER_SIZE] = {0};
char SettingsLoadBuffer::read_buffer[SettingsLoadBuffer::READ_BUFFER_SIZE] = {0};

} // namespace ui::apps::enhanced_drone_analyzer