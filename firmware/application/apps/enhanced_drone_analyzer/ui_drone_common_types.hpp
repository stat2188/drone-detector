#ifndef UI_DRONE_COMMON_TYPES_HPP_
#define UI_DRONE_COMMON_TYPES_HPP_

#include <cstdint>
#include <cstring>
#include "eda_constants.hpp"
#include "radio.hpp"
#include "file.hpp"

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

enum class SpectrumMode : uint8_t { NARROW = 0, MEDIUM = 1, WIDE = 2, ULTRA_WIDE = 3, ULTRA_NARROW = 4 };

static constexpr size_t MAX_PATH_LEN = EDA::Constants::MAX_PATH_LENGTH;
static constexpr size_t MAX_NAME_LEN = EDA::Constants::MAX_NAME_LENGTH;
static constexpr size_t MAX_FORMAT_LEN = EDA::Constants::MAX_FORMAT_LENGTH;

#pragma pack(push, 1)

struct DroneAnalyzerSettings {
    // Audio settings
    uint8_t audio_flags = 0x01;  // bit0: enable_alerts, bit1: repeat_alerts

    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;

    // Hardware settings
    uint8_t hardware_flags = 0x01;  // bit0: enable_real_hardware, bit1: demo_mode, bit2: iq_calibration_enabled, bit3: rf_amp_enabled

    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    uint64_t user_min_freq_hz = 50000000ULL;
    uint64_t user_max_freq_hz = 6000000000ULL;

    // Scanning settings
    uint8_t scanning_flags = 0x06;  // bit0: enable_wideband_scanning, bit1: panoramic_mode_enabled, bit2: enable_intelligent_scanning

    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;

    // Detection settings
    uint8_t detection_flags = 0x03;  // bit0: enable_fhss_detection, bit1: enable_intelligent_tracking

    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint8_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;

    // Logging settings
    uint8_t logging_flags = 0x0F;  // bit0: auto_save_logs, bit1: enable_session_logging, bit2: include_timestamp, bit3: include_rssi_values

    char log_file_path[MAX_PATH_LEN] = "/eda_logs";
    char log_format[MAX_FORMAT_LEN] = "CSV";
    uint32_t max_log_file_size_kb = 1024;

    // Display settings
    uint8_t display_flags = 0x1F;  // bit0: show_detailed_info, bit1: show_mini_spectrum, bit2: show_rssi_history, bit3: show_frequency_ruler, bit4: auto_ruler_style

    char color_scheme[MAX_NAME_LEN] = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    uint8_t frequency_ruler_style = 5;
    uint8_t compact_ruler_tick_count = 4;

    // Profile settings
    uint8_t profile_flags = 0x01;  // bit0: enable_quick_profiles, bit1: auto_save_on_change

    char current_profile_name[MAX_NAME_LEN] = "Default";

    // System settings
    char freqman_path[MAX_NAME_LEN] = "DRONES";
    char settings_file_path[MAX_PATH_LEN] = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
};

#pragma pack(pop)

static_assert(sizeof(DroneAnalyzerSettings) <= 512, "DroneAnalyzerSettings exceeds 512 bytes");

namespace BitfieldAccess {

template<uint8_t BitPos>
inline constexpr bool get_bit(uint8_t flags) noexcept {
    static_assert(BitPos < 8, "Bit position must be < 8 for uint8_t");
    return (flags & (1U << BitPos)) != 0;
}

template<uint8_t BitPos>
inline constexpr bool get_bit(uint32_t flags) noexcept {
    static_assert(BitPos < 32, "Bit position must be < 32 for uint32_t");
    return (flags & (1U << BitPos)) != 0;
}

template<uint8_t BitPos>
inline constexpr void set_bit(uint8_t& flags, bool value) noexcept {
    static_assert(BitPos < 8, "Bit position must be < 8 for uint8_t");
    const uint8_t mask = static_cast<uint8_t>(1U << BitPos);
    flags = (flags & ~mask) | (value ? mask : 0U);
}

template<uint8_t BitPos>
inline constexpr void set_bit(uint32_t& flags, bool value) noexcept {
    static_assert(BitPos < 32, "Bit position must be < 32 for uint32_t");
    const uint32_t mask = static_cast<uint32_t>(1U << BitPos);
    flags = (flags & ~mask) | (value ? mask : 0U);
}

} // namespace BitfieldAccess

// Audio flags helpers (bit0: enable_alerts, bit1: repeat_alerts)
inline bool audio_get_enable_alerts(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<0>(s.audio_flags);
}
inline void audio_set_enable_alerts(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<0>(s.audio_flags, v);
}
inline bool audio_get_repeat_alerts(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<1>(s.audio_flags);
}
inline void audio_set_repeat_alerts(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<1>(s.audio_flags, v);
}

// Hardware flags helpers (bit0: enable_real_hardware, bit1: demo_mode, bit2: iq_calibration_enabled, bit3: rf_amp_enabled)
inline bool hw_get_enable_real_hardware(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<0>(s.hardware_flags);
}
inline void hw_set_enable_real_hardware(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<0>(s.hardware_flags, v);
}
inline bool hw_get_demo_mode(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<1>(s.hardware_flags);
}
inline void hw_set_demo_mode(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<1>(s.hardware_flags, v);
}
inline bool hw_get_iq_calibration_enabled(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<2>(s.hardware_flags);
}
inline void hw_set_iq_calibration_enabled(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<2>(s.hardware_flags, v);
}
inline bool hw_get_rf_amp_enabled(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<3>(s.hardware_flags);
}
inline void hw_set_rf_amp_enabled(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<3>(s.hardware_flags, v);
}

// Scanning flags helpers (bit0: enable_wideband_scanning, bit1: panoramic_mode_enabled, bit2: enable_intelligent_scanning)
inline bool scan_get_enable_wideband_scanning(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<0>(s.scanning_flags);
}
inline void scan_set_enable_wideband_scanning(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<0>(s.scanning_flags, v);
}
inline bool scan_get_panoramic_mode_enabled(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<1>(s.scanning_flags);
}
inline void scan_set_panoramic_mode_enabled(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<1>(s.scanning_flags, v);
}
inline bool scan_get_enable_intelligent_scanning(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<2>(s.scanning_flags);
}
inline void scan_set_enable_intelligent_scanning(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<2>(s.scanning_flags, v);
}

// Detection flags helpers (bit0: enable_fhss_detection, bit1: enable_intelligent_tracking)
inline bool detect_get_enable_fhss_detection(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<0>(s.detection_flags);
}
inline void detect_set_enable_fhss_detection(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<0>(s.detection_flags, v);
}
inline bool detect_get_enable_intelligent_tracking(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<1>(s.detection_flags);
}
inline void detect_set_enable_intelligent_tracking(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<1>(s.detection_flags, v);
}

// Logging flags helpers (bit0: auto_save_logs, bit1: enable_session_logging, bit2: include_timestamp, bit3: include_rssi_values)
inline bool log_get_auto_save_logs(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<0>(s.logging_flags);
}
inline void log_set_auto_save_logs(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<0>(s.logging_flags, v);
}
inline bool log_get_enable_session_logging(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<1>(s.logging_flags);
}
inline void log_set_enable_session_logging(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<1>(s.logging_flags, v);
}
inline bool log_get_include_timestamp(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<2>(s.logging_flags);
}
inline void log_set_include_timestamp(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<2>(s.logging_flags, v);
}
inline bool log_get_include_rssi_values(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<3>(s.logging_flags);
}
inline void log_set_include_rssi_values(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<3>(s.logging_flags, v);
}

// Display flags helpers (bit0: show_detailed_info, bit1: show_mini_spectrum, bit2: show_rssi_history, bit3: show_frequency_ruler, bit4: auto_ruler_style)
inline bool disp_get_show_detailed_info(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<0>(s.display_flags);
}
inline void disp_set_show_detailed_info(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<0>(s.display_flags, v);
}
inline bool disp_get_show_mini_spectrum(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<1>(s.display_flags);
}
inline void disp_set_show_mini_spectrum(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<1>(s.display_flags, v);
}
inline bool disp_get_show_rssi_history(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<2>(s.display_flags);
}
inline void disp_set_show_rssi_history(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<2>(s.display_flags, v);
}
inline bool disp_get_show_frequency_ruler(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<3>(s.display_flags);
}
inline void disp_set_show_frequency_ruler(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<3>(s.display_flags, v);
}
inline bool disp_get_auto_ruler_style(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<4>(s.display_flags);
}
inline void disp_set_auto_ruler_style(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<4>(s.display_flags, v);
}

// Profile flags helpers (bit0: enable_quick_profiles, bit1: auto_save_on_change)
inline bool profile_get_enable_quick_profiles(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<0>(s.profile_flags);
}
inline void profile_set_enable_quick_profiles(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<0>(s.profile_flags, v);
}
inline bool profile_get_auto_save_on_change(const DroneAnalyzerSettings& s) noexcept {
    return BitfieldAccess::get_bit<1>(s.profile_flags);
}
inline void profile_set_auto_save_on_change(DroneAnalyzerSettings& s, bool v) noexcept {
    BitfieldAccess::set_bit<1>(s.profile_flags, v);
}

namespace FrequencyValidation {

inline constexpr bool is_valid(uint64_t freq_hz) noexcept {
    return freq_hz >= EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ &&
           freq_hz <= EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;
}

inline constexpr bool is_safe(uint64_t freq_hz) noexcept {
    return freq_hz >= EDA::Constants::FrequencyLimits::MIN_SAFE_FREQ &&
           freq_hz <= EDA::Constants::FrequencyLimits::MAX_SAFE_FREQ;
}

inline constexpr bool is_range_valid(uint64_t min_freq_hz, uint64_t max_freq_hz) noexcept {
    if (!is_valid(min_freq_hz) || !is_valid(max_freq_hz)) {
        return false;
    }
    return min_freq_hz < max_freq_hz;
}

inline constexpr bool is_range_safe(uint64_t min_freq_hz, uint64_t max_freq_hz) noexcept {
    if (!is_safe(min_freq_hz) || !is_safe(max_freq_hz)) {
        return false;
    }
    return min_freq_hz < max_freq_hz;
}

inline constexpr uint64_t clamp(uint64_t freq_hz) noexcept {
    if (freq_hz < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ) {
        return EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ;
    }
    if (freq_hz > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        return EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;
    }
    return freq_hz;
}

} // namespace FrequencyValidation

inline bool is_frequency_valid(uint64_t freq_hz) noexcept {
    return FrequencyValidation::is_valid(freq_hz);
}

inline bool is_frequency_range_valid(uint64_t min_freq_hz, uint64_t max_freq_hz) noexcept {
    return FrequencyValidation::is_range_valid(min_freq_hz, max_freq_hz);
}

// Enhanced enums for EDA
enum class ThreatLevel : uint8_t {
    NONE = 0,
    LOW =1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4,
    UNKNOWN = 5
};

enum class DroneType : uint8_t {
    UNKNOWN = 0,
    MAVIC = 1,
    DJI_P34 = 2,
    PHANTOM = 3,
    DJI_MINI = 4,
    PARROT_ANAFI = 5,
    PARROT_BEBOP = 6,
    PX4_DRONE = 7,
    MILITARY_DRONE = 8,
    DIY_DRONE = 9,
    FPV_RACING = 10
};

enum class MovementTrend : uint8_t {
    STATIC = 0,
    APPROACHING = 1,
    RECEDING = 2,
    UNKNOWN = 3
};

struct TranslationEntry {
    const char* key;
    const char* value;
};

class Translator {
public:
    static const char* translate(const char* key) noexcept;
    static const char* get_translation(const char* key) noexcept;

private:
    static const char* get_english(const char* key) noexcept;
};

struct DronePreset {
    char display_name[MAX_NAME_LEN];
    char name_template[MAX_NAME_LEN];
    Frequency frequency_hz;
    ThreatLevel threat_level;
    DroneType drone_type;

    bool is_valid() const {
        return display_name[0] != '\0' && frequency_hz > 0;
    }
};

// Implementations moved to ui_drone_common_types.cpp

#pragma pack(push, 1)
struct DetectionLogEntry {
    uint32_t timestamp;
    uint64_t frequency_hz;
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    uint8_t confidence_percent;
    uint8_t width_bins;
    uint32_t signal_width_hz;
    uint8_t snr;
};
#pragma pack(pop)

}  // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_DRONE_COMMON_TYPES_HPP_
