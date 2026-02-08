/**
 * DIAMOND OPTIMIZATION: Settings Bitfields
 * 
 * REMOVED: DroneAnalyzerSettingsOptimized (never used, ~200 lines)
 * SAVINGS: ~200 lines of code, ~30 bytes RAM (not used anyway)
 * 
 * NOTE: If bitfield packing is needed in the future, apply to DroneAnalyzerSettings
 * instead of creating duplicate structures.
 * 
 * CONSTRAINTS:
 * - Cortex-M4 alignment-safe
 * - Packed for flash storage
 */

#ifndef SETTINGS_BITFIELDS_HPP_
#define SETTINGS_BITFIELDS_HPP_

#include <cstdint>
#include <string>

#pragma pack(push, 1)

namespace ui::apps::enhanced_drone_analyzer {

// ===========================================
// BITFIELD FLAGS (for future use with DroneAnalyzerSettings)
// ===========================================
// These are defined for potential future optimization but not currently used

struct AudioFlags {
    bool enable_alerts : 1;
    bool repeat_alerts : 1;
    uint8_t reserved : 6;
};
static_assert(sizeof(AudioFlags) == 1, "AudioFlags must be 1 byte");

struct HardwareFlags {
    bool enable_real_hardware : 1;
    bool demo_mode : 1;
    bool iq_calibration_enabled : 1;
    bool rf_amp_enabled : 1;
    uint8_t reserved : 4;
};
static_assert(sizeof(HardwareFlags) == 1, "HardwareFlags must be 1 byte");

struct ScanningFlags {
    bool enable_wideband_scanning : 1;
    bool panoramic_mode_enabled : 1;
    bool enable_intelligent_scanning : 1;
    uint8_t reserved : 5;
};
static_assert(sizeof(ScanningFlags) == 1, "ScanningFlags must be 1 byte");

struct DetectionFlags {
    bool enable_fhss_detection : 1;
    bool enable_intelligent_tracking : 1;
    uint8_t reserved : 6;
};
static_assert(sizeof(DetectionFlags) == 1, "DetectionFlags must be 1 byte");

struct LoggingFlags {
    bool auto_save_logs : 1;
    bool enable_session_logging : 1;
    bool include_timestamp : 1;
    bool include_rssi_values : 1;
    uint8_t reserved : 4;
};
static_assert(sizeof(LoggingFlags) == 1, "LoggingFlags must be 1 byte");

struct DisplayFlags {
    bool show_detailed_info : 1;
    bool show_mini_spectrum : 1;
    bool show_rssi_history : 1;
    bool show_frequency_ruler : 1;
    bool auto_ruler_style : 1;
    uint8_t reserved : 3;
};
static_assert(sizeof(DisplayFlags) == 1, "DisplayFlags must be 1 byte");

struct ProfileFlags {
    bool enable_quick_profiles : 1;
    bool auto_save_on_change : 1;
    uint8_t reserved : 6;
};
static_assert(sizeof(ProfileFlags) == 1, "ProfileFlags must be 1 byte");

#pragma pack(pop)

} // namespace ui::apps::enhanced_drone_analyzer

#endif // SETTINGS_BITFIELDS_HPP_