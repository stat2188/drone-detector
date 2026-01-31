/*
 * EDA Unified Settings - Single source of truth for all EDA settings
 * Follows Mayhem BoundSetting pattern for automatic serialization
 */

#ifndef __EDA_UNIFIED_SETTINGS_HPP__
#define __EDA_UNIFIED_SETTINGS_HPP__

#include <stdint.h>
#include <string>
#include <array>

#include "ui_drone_common_types.hpp"
#include "drone_constants.hpp"

namespace ui::apps::enhanced_drone_analyzer {

struct AudioSettings {
    bool audio_enabled = true;
    uint32_t alert_frequency_hz = 800;
    uint32_t alert_duration_ms = 500;
    uint8_t volume_level = 50;
    bool repeat_alerts = false;
};

struct HardwareSettings {
    bool enable_real_hardware = true;
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    uint64_t user_min_freq_hz = DroneConstants::FrequencyLimits::MIN_SAFE_FREQ;
    uint64_t user_max_freq_hz = DroneConstants::FrequencyLimits::MAX_SAFE_FREQ;
    bool iq_calibration_enabled = false;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    bool rf_amp_enabled = false;
};

struct ScanningSettings {
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = DroneConstants::DEFAULT_RSSI_THRESHOLD_DB;
    bool enable_wideband_scanning = false;
    uint64_t wideband_min_freq_hz = DroneConstants::WIDEBAND_DEFAULT_MIN;
    uint64_t wideband_max_freq_hz = DroneConstants::WIDEBAND_DEFAULT_MAX;
    uint32_t wideband_slice_width_hz = DroneConstants::WIDEBAND_DEFAULT_SLICE_WIDTH;
    bool panoramic_mode_enabled = true;
    bool enable_intelligent_scanning = true;
};

struct DetectionSettings {
    bool enable_fhss_detection = true;
    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    bool enable_intelligent_tracking = true;
    uint8_t min_detection_count = DroneConstants::MIN_DETECTION_COUNT;
    uint32_t alert_persistence_threshold = DroneConstants::ALERT_PERSISTENCE_THRESHOLD;
};

struct LoggingSettings {
    bool auto_save_logs = true;
    std::string log_file_path = "/eda_logs";
    std::string log_format = "CSV";
    uint32_t max_log_file_size_kb = 1024;
    bool enable_session_logging = true;
    bool include_timestamp = true;
    bool include_rssi_values = true;
};

struct DisplaySettings {
    std::string color_scheme = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    bool show_detailed_info = true;
    bool show_mini_spectrum = true;
    bool show_rssi_history = true;
    bool show_frequency_ruler = true;
    uint8_t frequency_ruler_style = 0;
    uint8_t compact_ruler_tick_count = 4;
    bool auto_ruler_style = true;
};

struct ProfileSettings {
    std::string current_profile_name = "Default";
    bool enable_quick_profiles = true;
    bool auto_save_on_change = false;
};

struct EDAUnifiedSettings {
    AudioSettings audio;
    HardwareSettings hardware;
    ScanningSettings scanning;
    DetectionSettings detection;
    LoggingSettings logging;
    DisplaySettings display;
    ProfileSettings profile;

    uint32_t settings_version = 1;
    std::string settings_file_path = "/sdcard/EDA_UNIFIED_SETTINGS.txt";

    bool is_valid() const;
    void reset_to_defaults();
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // __EDA_UNIFIED_SETTINGS_HPP__
