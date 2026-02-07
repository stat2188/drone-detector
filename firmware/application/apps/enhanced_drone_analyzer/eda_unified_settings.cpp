/*
 * ========================================
 * LEGACY FILE - DEPRECATED
 * ========================================
 * This file has been DEPRECATED in favor of using
 * DroneAnalyzerSettings as the single source of truth.
 * All settings functions have been moved to
 * ScannerSettingsManager.
 *
 * Migration: Use ScannerSettingsManager::load/save
 * for DroneAnalyzerSettings instead.
 * ========================================
 */

// #include "eda_unified_settings.hpp"
// #include "ui_drone_common_types.hpp"
// #include "drone_constants.hpp"
//
// namespace ui::apps::enhanced_drone_analyzer {
//
// bool EDAUnifiedSettings::is_valid() const {
//     if (audio.alert_frequency_hz < DroneConstants::MIN_AUDIO_FREQ ||
//         audio.alert_frequency_hz > DroneConstants::MAX_AUDIO_FREQ) {
//         return false;
//     }
//
//     if (audio.alert_duration_ms < DroneConstants::MIN_AUDIO_DURATION ||
//         audio.alert_duration_ms > DroneConstants::MAX_AUDIO_DURATION) {
//         return false;
//     }
//
//     if (hardware.hardware_bandwidth_hz < DroneConstants::MIN_BANDWIDTH ||
//         hardware.hardware_bandwidth_hz > DroneConstants::MAX_BANDWIDTH) {
//         return false;
//     }
//
//     if (hardware.user_min_freq_hz >= hardware.user_max_freq_hz) {
//         return false;
//     }
//
//     if (scanning.scan_interval_ms < DroneConstants::MIN_SCAN_INTERVAL_MS ||
//         scanning.scan_interval_ms > DroneConstants::MAX_SCAN_INTERVAL_MS) {
//         return false;
//     }
//
//     if (scanning.rssi_threshold_db < DroneConstants::NOISE_FLOOR_RSSI ||
//         scanning.rssi_threshold_db > DroneConstants::MAX_VALID_RSSI) {
//         return false;
//     }
//
//     return true;
// }
//
// void EDAUnifiedSettings::reset_to_defaults() {
//     audio = AudioSettings{};
//
//     hardware = HardwareSettings{};
//     hardware.enable_real_hardware = true;
//     hardware.spectrum_mode = SpectrumMode::MEDIUM;
//     hardware.hardware_bandwidth_hz = 24000000;
//     hardware.user_min_freq_hz = DroneConstants::FrequencyLimits::MIN_SAFE_FREQ;
//     hardware.user_max_freq_hz = DroneConstants::FrequencyLimits::MAX_SAFE_FREQ;
//     hardware.lna_gain_db = 32;
//     hardware.vga_gain_db = 20;
//
//     scanning = ScanningSettings{};
//     scanning.scan_interval_ms = 1000;
//     scanning.rssi_threshold_db = DroneConstants::DEFAULT_RSSI_THRESHOLD_DB;
//     scanning.enable_wideband_scanning = false;
//     scanning.wideband_min_freq_hz = DroneConstants::WIDEBAND_DEFAULT_MIN;
//     scanning.wideband_max_freq_hz = DroneConstants::WIDEBAND_DEFAULT_MAX;
//     scanning.wideband_slice_width_hz = DroneConstants::WIDEBAND_DEFAULT_SLICE_WIDTH;
//
//     detection = DetectionSettings{};
//     detection.enable_fhss_detection = true;
//     detection.movement_sensitivity = 3;
//     detection.threat_level_threshold = 2;
//
//     logging = LoggingSettings{};
//     logging.auto_save_logs = true;
//     logging.log_file_path = "/eda_logs";
//     logging.log_format = "CSV";
//     logging.max_log_file_size_kb = 1024;
//
//     display = DisplaySettings{};
//     display.color_scheme = "DARK";
//     display.font_size = 0;
//     display.spectrum_density = 1;
//     display.waterfall_speed = 5;
//     display.show_detailed_info = true;
//
//     profile = ProfileSettings{};
//     profile.current_profile_name = "Default";
//     profile.enable_quick_profiles = true;
//
//     settings_version = 1;
// }
//
// } // namespace ui::apps::enhanced_drone_analyzer
