/**
 * Settings Bitfields - COMPATIBILITY HEADER
 * 
 * DEPRECATED: This file is kept for backward compatibility only.
 * All bitfield definitions have been moved to ui_drone_common_types.hpp
 * to avoid ODR (One Definition Rule) violations.
 * 
 * To use bitfields, include ui_drone_common_types.hpp instead:
 * #include "ui_drone_common_types.hpp"
 * 
 * The following structures are now defined in DroneAnalyzerSettings:
 * - AudioFlags
 * - HardwareFlags
 * - ScanningFlags
 * - DetectionFlags
 * - LoggingFlags
 * - DisplayFlags
 * - ProfileFlags
 * 
 * @deprecated Use ui_drone_common_types.hpp instead
 */

#ifndef SETTINGS_BITFIELDS_HPP_
#define SETTINGS_BITFIELDS_HPP_

#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Aliases for backward compatibility
using SettingsAudioFlags = DroneAnalyzerSettings::AudioFlags;
using SettingsHardwareFlags = DroneAnalyzerSettings::HardwareFlags;
using SettingsScanningFlags = DroneAnalyzerSettings::ScanningFlags;
using SettingsDetectionFlags = DroneAnalyzerSettings::DetectionFlags;
using SettingsLoggingFlags = DroneAnalyzerSettings::LoggingFlags;
using SettingsDisplayFlags = DroneAnalyzerSettings::DisplayFlags;
using SettingsProfileFlags = DroneAnalyzerSettings::ProfileFlags;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // SETTINGS_BITFIELDS_HPP_
