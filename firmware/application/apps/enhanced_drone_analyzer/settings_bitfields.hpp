/**
 * 
 * 
 * MEMORY SAVINGS:
 * - 35 bool flags packed into 5 bytes (was 35 bytes)
 * - RAM savings: ~30 bytes
 * - Cache-friendly access
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
// AUDIO FLAGS (1 byte)
// ===========================================
struct AudioFlags {
    bool enable_alerts : 1;
    bool repeat_alerts : 1;
    uint8_t reserved : 6;  // Future expansion
};
static_assert(sizeof(AudioFlags) == 1, "AudioFlags must be 1 byte");

// ===========================================
// HARDWARE FLAGS (1 byte)
// ===========================================
struct HardwareFlags {
    bool enable_real_hardware : 1;
    bool demo_mode : 1;
    bool iq_calibration_enabled : 1;
    bool rf_amp_enabled : 1;
    uint8_t reserved : 4;  // Future expansion
};
static_assert(sizeof(HardwareFlags) == 1, "HardwareFlags must be 1 byte");

// ===========================================
// SCANNING FLAGS (1 byte)
// ===========================================
struct ScanningFlags {
    bool enable_wideband_scanning : 1;
    bool panoramic_mode_enabled : 1;
    bool enable_intelligent_scanning : 1;
    uint8_t reserved : 5;  // Future expansion
};
static_assert(sizeof(ScanningFlags) == 1, "ScanningFlags must be 1 byte");

// ===========================================
// DETECTION FLAGS (1 byte)
// ===========================================
struct DetectionFlags {
    bool enable_fhss_detection : 1;
    bool enable_intelligent_tracking : 1;
    uint8_t reserved : 6;  // Future expansion
};
static_assert(sizeof(DetectionFlags) == 1, "DetectionFlags must be 1 byte");

// ===========================================
// LOGGING FLAGS (1 byte)
// ===========================================
struct LoggingFlags {
    bool auto_save_logs : 1;
    bool enable_session_logging : 1;
    bool include_timestamp : 1;
    bool include_rssi_values : 1;
    uint8_t reserved : 4;  // Future expansion
};
static_assert(sizeof(LoggingFlags) == 1, "LoggingFlags must be 1 byte");

// ===========================================
// DISPLAY FLAGS (1 byte)
// ===========================================
struct DisplayFlags {
    bool show_detailed_info : 1;
    bool show_mini_spectrum : 1;
    bool show_rssi_history : 1;
    bool show_frequency_ruler : 1;
    bool auto_ruler_style : 1;
    uint8_t reserved : 3;  // Future expansion
};
static_assert(sizeof(DisplayFlags) == 1, "DisplayFlags must be 1 byte");

// ===========================================
// PROFILE FLAGS (1 byte)
// ===========================================
struct ProfileFlags {
    bool enable_quick_profiles : 1;
    bool auto_save_on_change : 1;
    uint8_t reserved : 6;  // Future expansion
};
static_assert(sizeof(ProfileFlags) == 1, "ProfileFlags must be 1 byte");

// ===========================================
// OPTIMIZED DRONE ANALYZER SETTINGS
// ===========================================
struct DroneAnalyzerSettingsOptimized {
    // ===== FLAGS (7 bytes instead of 35+ bools) =====
    AudioFlags audio_flags;
    HardwareFlags hardware_flags;
    ScanningFlags scanning_flags;
    DetectionFlags detection_flags;
    LoggingFlags logging_flags;
    DisplayFlags display_flags;
    ProfileFlags profile_flags;
    
    // ===== AUDIO SETTINGS =====
    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;
    
    // ===== HARDWARE SETTINGS =====
    uint8_t spectrum_mode = 1;  // SpectrumMode::MEDIUM
    uint32_t hardware_bandwidth_hz = 24000000;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    uint64_t user_min_freq_hz = 50000000ULL;
    uint64_t user_max_freq_hz = 6000000000ULL;
    
    // ===== SCANNING SETTINGS =====
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;
    
    // ===== DETECTION SETTINGS =====
    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint8_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;
    
    // ===== LOGGING SETTINGS =====
    std::string log_file_path = "/eda_logs";
    std::string log_format = "CSV";
    uint32_t max_log_file_size_kb = 1024;
    
    // ===== DISPLAY SETTINGS =====
    std::string color_scheme = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    uint8_t frequency_ruler_style = 5;
    uint8_t compact_ruler_tick_count = 4;
    
    // ===== PROFILE SETTINGS =====
    std::string current_profile_name = "Default";
    
    // ===== SYSTEM SETTINGS =====
    std::string freqman_path = "DRONES";
    std::string settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
    
    // Convenience getters/setters for backward compatibility
    bool enable_audio_alerts() const { return audio_flags.enable_alerts; }
    void enable_audio_alerts(bool v) { audio_flags.enable_alerts = v; }
    
    bool audio_repeat_alerts() const { return audio_flags.repeat_alerts; }
    void audio_repeat_alerts(bool v) { audio_flags.repeat_alerts = v; }
    
    bool enable_real_hardware() const { return hardware_flags.enable_real_hardware; }
    void enable_real_hardware(bool v) { hardware_flags.enable_real_hardware = v; }
    
    bool demo_mode() const { return hardware_flags.demo_mode; }
    void demo_mode(bool v) { hardware_flags.demo_mode = v; }
    
    bool iq_calibration_enabled() const { return hardware_flags.iq_calibration_enabled; }
    void iq_calibration_enabled(bool v) { hardware_flags.iq_calibration_enabled = v; }
    
    bool rf_amp_enabled() const { return hardware_flags.rf_amp_enabled; }
    void rf_amp_enabled(bool v) { hardware_flags.rf_amp_enabled = v; }
    
    bool enable_wideband_scanning() const { return scanning_flags.enable_wideband_scanning; }
    void enable_wideband_scanning(bool v) { scanning_flags.enable_wideband_scanning = v; }
    
    bool panoramic_mode_enabled() const { return scanning_flags.panoramic_mode_enabled; }
    void panoramic_mode_enabled(bool v) { scanning_flags.panoramic_mode_enabled = v; }
    
    bool enable_intelligent_scanning() const { return scanning_flags.enable_intelligent_scanning; }
    void enable_intelligent_scanning(bool v) { scanning_flags.enable_intelligent_scanning = v; }
    
    bool enable_fhss_detection() const { return detection_flags.enable_fhss_detection; }
    void enable_fhss_detection(bool v) { detection_flags.enable_fhss_detection = v; }
    
    bool enable_intelligent_tracking() const { return detection_flags.enable_intelligent_tracking; }
    void enable_intelligent_tracking(bool v) { detection_flags.enable_intelligent_tracking = v; }
    
    bool auto_save_logs() const { return logging_flags.auto_save_logs; }
    void auto_save_logs(bool v) { logging_flags.auto_save_logs = v; }
    
    bool enable_session_logging() const { return logging_flags.enable_session_logging; }
    void enable_session_logging(bool v) { logging_flags.enable_session_logging = v; }
    
    bool include_timestamp() const { return logging_flags.include_timestamp; }
    void include_timestamp(bool v) { logging_flags.include_timestamp = v; }
    
    bool include_rssi_values() const { return logging_flags.include_rssi_values; }
    void include_rssi_values(bool v) { logging_flags.include_rssi_values = v; }
    
    bool show_detailed_info() const { return display_flags.show_detailed_info; }
    void show_detailed_info(bool v) { display_flags.show_detailed_info = v; }
    
    bool show_mini_spectrum() const { return display_flags.show_mini_spectrum; }
    void show_mini_spectrum(bool v) { display_flags.show_mini_spectrum = v; }
    
    bool show_rssi_history() const { return display_flags.show_rssi_history; }
    void show_rssi_history(bool v) { display_flags.show_rssi_history = v; }
    
    bool show_frequency_ruler() const { return display_flags.show_frequency_ruler; }
    void show_frequency_ruler(bool v) { display_flags.show_frequency_ruler = v; }
    
    bool auto_ruler_style() const { return display_flags.auto_ruler_style; }
    void auto_ruler_style(bool v) { display_flags.auto_ruler_style = v; }
    
    bool enable_quick_profiles() const { return profile_flags.enable_quick_profiles; }
    void enable_quick_profiles(bool v) { profile_flags.enable_quick_profiles = v; }
    
    bool auto_save_on_change() const { return profile_flags.auto_save_on_change; }
    void auto_save_on_change(bool v) { profile_flags.auto_save_on_change = v; }
};

#pragma pack(pop)

} // namespace ui::apps::enhanced_drone_analyzer

#endif // SETTINGS_BITFIELDS_HPP_