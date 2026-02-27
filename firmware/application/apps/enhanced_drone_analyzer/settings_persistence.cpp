// * * Diamond-Optimized Settings Persistence Implementation
// * * Explicit template instantiation for DroneAnalyzerSettings

// Corresponding header (must be first)
#include "settings_persistence.hpp"

// C standard library headers (alphabetical order)
#include <inttypes.h>

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Third-party library headers
#include <ch.h>

// Project-specific headers (alphabetical order)
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Explicit template instantiation for DroneAnalyzerSettings
template class SettingsPersistence<DroneAnalyzerSettings>;

// Settings Buffer Mutex Definition
Mutex settings_buffer_mutex;

// Errno Mutex Definition
Mutex errno_mutex;

// Phase 1 Optimization: Eliminated SettingsStaticBuffer (4KB) by writing directly to file

// Load buffer now uses instance members in get_load_buffer()

} // namespace ui::apps::enhanced_drone_analyzer
