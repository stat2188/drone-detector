#include <iostream>
#include <cstdint>

struct DroneAnalyzerSettings {
    // ===== AUDIO SETTINGS =====
    struct AudioFlags {
        bool enable_alerts : 1;
        bool repeat_alerts : 1;
        uint8_t reserved : 6;
    } audio_flags = {true, false, 0};

    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;

    // ===== HARDWARE SETTINGS =====
    struct HardwareFlags {
        bool enable_real_hardware : 1;
        bool demo_mode : 1;
        bool iq_calibration_enabled : 1;
        bool rf_amp_enabled : 1;
        uint8_t reserved : 4;
    } hardware_flags = {true, false, false, false, 0};

    enum class SpectrumMode : uint8_t { NARROW = 0, MEDIUM = 1, WIDE = 2, ULTRA_WIDE = 3, ULTRA_NARROW = 4 };
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    uint64_t user_min_freq_hz = 50000000ULL;
    uint64_t user_max_freq_hz = 6000000000ULL;

    // ===== SCANNING SETTINGS =====
    struct ScanningFlags {
        bool enable_wideband_scanning : 1;
        bool panoramic_mode_enabled : 1;
        bool enable_intelligent_scanning : 1;
        uint8_t reserved : 5;
    } scanning_flags = {false, true, true, 0};

    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;

    // ===== DETECTION SETTINGS =====
    struct DetectionFlags {
        bool enable_fhss_detection : 1;
        bool enable_intelligent_tracking : 1;
        uint8_t reserved : 6;
    } detection_flags = {true, true, 0};

    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint8_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;

    // ===== LOGGING SETTINGS (Zero-Heap Strings) =====
    struct LoggingFlags {
        bool auto_save_logs : 1;
        bool enable_session_logging : 1;
        bool include_timestamp : 1;
        bool include_rssi_values : 1;
        uint8_t reserved : 4;
    } logging_flags = {true, true, true, true, 0};

    static constexpr size_t MAX_PATH_LEN = 64;
    char log_file_path[MAX_PATH_LEN] = "/eda_logs";
    static constexpr size_t MAX_FORMAT_LEN = 8;
    char log_format[MAX_FORMAT_LEN] = "CSV";
    uint32_t max_log_file_size_kb = 1024;

    // ===== DISPLAY SETTINGS (Zero-Heap Strings) =====
    struct DisplayFlags {
        bool show_detailed_info : 1;
        bool show_mini_spectrum : 1;
        bool show_rssi_history : 1;
        bool show_frequency_ruler : 1;
        bool auto_ruler_style : 1;
        uint8_t reserved : 3;
    } display_flags = {true, true, true, true, true, 0};

    static constexpr size_t MAX_NAME_LEN = 32;
    char color_scheme[MAX_NAME_LEN] = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    uint8_t frequency_ruler_style = 5;
    uint8_t compact_ruler_tick_count = 4;

    // ===== PROFILE SETTINGS (Zero-Heap Strings) =====
    struct ProfileFlags {
        bool enable_quick_profiles : 1;
        bool auto_save_on_change : 1;
        uint8_t reserved : 6;
    } profile_flags = {true, false, 0};

    char current_profile_name[MAX_NAME_LEN] = "Default";

    // ===== SYSTEM SETTINGS (Zero-Heap Strings) =====
    char freqman_path[MAX_NAME_LEN] = "DRONES";
    char settings_file_path[MAX_PATH_LEN] = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
} __attribute__((packed));

int main() {
    std::cout << "sizeof(DroneAnalyzerSettings) = " << sizeof(DroneAnalyzerSettings) << " bytes" << std::endl;
    return 0;
}
