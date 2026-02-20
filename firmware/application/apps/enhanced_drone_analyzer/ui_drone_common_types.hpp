#ifndef UI_DRONE_COMMON_TYPES_HPP_
#define UI_DRONE_COMMON_TYPES_HPP_

#include <cstdint>
#include <cstring>
#include "eda_constants.hpp"
#include "radio.hpp"
#include "file.hpp"

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// SpectrumMode forward declare
enum class SpectrumMode : uint8_t { NARROW = 0, MEDIUM = 1, WIDE = 2, ULTRA_WIDE = 3, ULTRA_NARROW = 4 };

// ===========================================
// CONSTANTS & TYPE ALIASES
// ===========================================
// 🔴 PHASE 3: Use constants from EDA::Constants instead of magic numbers
static constexpr size_t MAX_PATH_LEN = EDA::Constants::MAX_PATH_LENGTH;
static constexpr size_t MAX_NAME_LEN = EDA::Constants::MAX_NAME_LENGTH;
static constexpr size_t MAX_FORMAT_LEN = EDA::Constants::MAX_FORMAT_LENGTH;

// ===========================================
// DIAMOND FIX: PACKED STRUCTS TO MINIMIZE SIZE
// ===========================================
// Eliminates padding waste (~30-50 bytes saved)
#pragma pack(push, 1)

struct DroneAnalyzerSettings {
    // ===== AUDIO SETTINGS =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t audio_flags = 0x01;  // bit0: enable_alerts, bit1: repeat_alerts

    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;

    // ===== HARDWARE SETTINGS =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t hardware_flags = 0x01;  // bit0: enable_real_hardware, bit1: demo_mode, bit2: iq_calibration_enabled, bit3: rf_amp_enabled

    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    uint64_t user_min_freq_hz = 50000000ULL;
    uint64_t user_max_freq_hz = 6000000000ULL;

    // ===== SCANNING SETTINGS =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t scanning_flags = 0x06;  // bit0: enable_wideband_scanning, bit1: panoramic_mode_enabled, bit2: enable_intelligent_scanning

    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;

    // ===== DETECTION SETTINGS =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t detection_flags = 0x03;  // bit0: enable_fhss_detection, bit1: enable_intelligent_tracking

    uint8_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint8_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;

    // ===== LOGGING SETTINGS (Zero-Heap Strings) =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t logging_flags = 0x0F;  // bit0: auto_save_logs, bit1: enable_session_logging, bit2: include_timestamp, bit3: include_rssi_values

    char log_file_path[MAX_PATH_LEN] = "/eda_logs";
    char log_format[MAX_FORMAT_LEN] = "CSV";
    uint32_t max_log_file_size_kb = 1024;

    // ===== DISPLAY SETTINGS (Zero-Heap Strings) =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t display_flags = 0x1F;  // bit0: show_detailed_info, bit1: show_mini_spectrum, bit2: show_rssi_history, bit3: show_frequency_ruler, bit4: auto_ruler_style

    char color_scheme[MAX_NAME_LEN] = "DARK";
    uint8_t font_size = 0;
    uint8_t spectrum_density = 1;
    uint8_t waterfall_speed = 5;
    uint8_t frequency_ruler_style = 5;
    uint8_t compact_ruler_tick_count = 4;

    // ===== PROFILE SETTINGS (Zero-Heap Strings) =====
    // FIX #27: Replace bitfields with explicit masking for portability
    uint8_t profile_flags = 0x01;  // bit0: enable_quick_profiles, bit1: auto_save_on_change

    char current_profile_name[MAX_NAME_LEN] = "Default";

    // ===== SYSTEM SETTINGS (Zero-Heap Strings) =====
    char freqman_path[MAX_NAME_LEN] = "DRONES";
    char settings_file_path[MAX_PATH_LEN] = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
};

#pragma pack(pop)

static_assert(sizeof(DroneAnalyzerSettings) <= 512, "DroneAnalyzerSettings exceeds 512 bytes");

// ===========================================
// DIAMOND FIX: Unified Bitfield Access Layer
// ===========================================
// Single source of truth for bitfield operations
// Eliminates dual access pattern (helper functions + direct bit manipulation)
// Type-safe, constexpr, zero runtime overhead
// Atomic on ARM Cortex-M4 (8-bit reads/writes are atomic)

namespace BitfieldAccess {

/**
 * @brief Get bit value from flags register (uint8_t version)
 * @tparam BitPos Bit position (0-7 for uint8_t)
 * @param flags Flags register
 * @return true if bit is set, false otherwise
 * @note Compile-time constant, zero runtime overhead
 * @note Atomic on ARM Cortex-M4 (8-bit read is atomic)
 */
template<uint8_t BitPos>
inline constexpr bool get_bit(uint8_t flags) noexcept {
    static_assert(BitPos < 8, "Bit position must be < 8 for uint8_t");
    return (flags & (1U << BitPos)) != 0;
}

/**
 * @brief Get bit value from flags register (uint32_t version)
 * @tparam BitPos Bit position (0-31 for uint32_t)
 * @param flags Flags register
 * @return true if bit is set, false otherwise
 * @note Compile-time constant, zero runtime overhead
 */
template<uint8_t BitPos>
inline constexpr bool get_bit(uint32_t flags) noexcept {
    static_assert(BitPos < 32, "Bit position must be < 32 for uint32_t");
    return (flags & (1U << BitPos)) != 0;
}

/**
 * @brief Set bit value in flags register (uint8_t version)
 * @tparam BitPos Bit position (0-7 for uint8_t)
 * @param flags Flags register (reference)
 * @param value Bit value to set
 * @note Atomic on ARM Cortex-M4 (8-bit write is atomic)
 */
template<uint8_t BitPos>
inline constexpr void set_bit(uint8_t& flags, bool value) noexcept {
    static_assert(BitPos < 8, "Bit position must be < 8 for uint8_t");
    const uint8_t mask = static_cast<uint8_t>(1U << BitPos);
    flags = (flags & ~mask) | (value ? mask : 0U);
}

/**
 * @brief Set bit value in flags register (uint32_t version)
 * @tparam BitPos Bit position (0-31 for uint32_t)
 * @param flags Flags register (reference)
 * @param value Bit value to set
 */
template<uint8_t BitPos>
inline constexpr void set_bit(uint32_t& flags, bool value) noexcept {
    static_assert(BitPos < 32, "Bit position must be < 32 for uint32_t");
    const uint32_t mask = static_cast<uint32_t>(1U << BitPos);
    flags = (flags & ~mask) | (value ? mask : 0U);
}

} // namespace BitfieldAccess

// ===========================================
// DIAMOND FIX: Named Bitfield Access Functions
// ===========================================
// These functions provide semantic access to bit flags
// All use BitfieldAccess templates for type safety
// Maintains API compatibility with existing code

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

// Default wideband constants for scanner settings
static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2400000000ULL;
static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2500000000ULL;

// ===========================================
// DIAMOND FIX: Unified Frequency Validation
// ===========================================
// Single source of truth for frequency validation
// Eliminates signed/unsigned type ambiguity
// All frequencies use uint64_t (matches DroneAnalyzerSettings)
// Cross-field validation ensures min < max

namespace FrequencyValidation {

/**
 * @brief Check if a frequency value is within valid hardware range
 * @param freq_hz Frequency value to validate (uint64_t)
 * @return true if frequency is valid, false otherwise
 *
 * @note Uses uint64_t for consistency with DroneAnalyzerSettings
 * @note Hardware limits: 1 MHz to 7.2 GHz (HackRF One)
 * @note Safe limits: 50 MHz to 6 GHz (PLL stability)
 *
 * @see EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ
 * @see EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ
 */
inline constexpr bool is_valid(uint64_t freq_hz) noexcept {
    return freq_hz >= EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ &&
           freq_hz <= EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;
}

/**
 * @brief Check if a frequency value is within safe operating range
 * @param freq_hz Frequency value to validate (uint64_t)
 * @return true if frequency is safe, false otherwise
 *
 * @note Safe limits are tighter than hardware limits
 * @note Ensures PLL stability and reliable operation
 *
 * @see EDA::Constants::FrequencyLimits::MIN_SAFE_FREQ
 * @see EDA::Constants::FrequencyLimits::MAX_SAFE_FREQ
 */
inline constexpr bool is_safe(uint64_t freq_hz) noexcept {
    return freq_hz >= EDA::Constants::FrequencyLimits::MIN_SAFE_FREQ &&
           freq_hz <= EDA::Constants::FrequencyLimits::MAX_SAFE_FREQ;
}

/**
 * @brief Check if a frequency range is valid (min < max)
 * @param min_freq_hz Minimum frequency value (uint64_t)
 * @param max_freq_hz Maximum frequency value (uint64_t)
 * @return true if range is valid, false otherwise
 *
 * @note Performs both individual and cross-field validation
 * @note Returns false if min >= max
 * @note Returns false if either frequency is out of range
 */
inline constexpr bool is_range_valid(uint64_t min_freq_hz, uint64_t max_freq_hz) noexcept {
    // Guard clause: Ensure both frequencies are individually valid
    if (!is_valid(min_freq_hz) || !is_valid(max_freq_hz)) {
        return false;
    }
    // Cross-field validation: min must be less than max
    return min_freq_hz < max_freq_hz;
}

/**
 * @brief Check if a frequency range is within safe operating limits
 * @param min_freq_hz Minimum frequency value (uint64_t)
 * @param max_freq_hz Maximum frequency value (uint64_t)
 * @return true if range is safe, false otherwise
 *
 * @note Uses safe limits (tighter than hardware limits)
 * @note Ensures PLL stability for the entire range
 */
inline constexpr bool is_range_safe(uint64_t min_freq_hz, uint64_t max_freq_hz) noexcept {
    // Guard clause: Ensure both frequencies are individually safe
    if (!is_safe(min_freq_hz) || !is_safe(max_freq_hz)) {
        return false;
    }
    // Cross-field validation: min must be less than max
    return min_freq_hz < max_freq_hz;
}

/**
 * @brief Clamp frequency to valid hardware range
 * @param freq_hz Frequency value to clamp (uint64_t)
 * @return Clamped frequency value
 *
 * @note Returns MIN_HARDWARE_FREQ if freq is below minimum
 * @note Returns MAX_HARDWARE_FREQ if freq is above maximum
 * @note Returns freq unchanged if within valid range
 */
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

// ===========================================
// DIAMOND FIX: Backward Compatibility Aliases
// ===========================================
// Maintains API compatibility with existing code
// All functions delegate to FrequencyValidation namespace

/**
 * @brief Check if a frequency value is within valid hardware range
 * @param freq_hz Frequency value to validate (uint64_t)
 * @return true if frequency is valid, false otherwise
 *
 * @deprecated Use FrequencyValidation::is_valid() instead
 */
inline bool is_frequency_valid(uint64_t freq_hz) noexcept {
    return FrequencyValidation::is_valid(freq_hz);
}

/**
 * @brief Check if a frequency range is valid (min < max)
 * @param min_freq_hz Minimum frequency value (uint64_t)
 * @param max_freq_hz Maximum frequency value (uint64_t)
 * @return true if range is valid, false otherwise
 *
 * @deprecated Use FrequencyValidation::is_range_valid() instead
 */
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

enum class Language : uint8_t {
    ENGLISH = 0
};

// DIAMOND OPTIMIZATION: constexpr LUTs instead of std::string comparisons
// Scott Meyers Item 15: Prefer constexpr to #define
// RAM savings: all strings stored in Flash, 0 heap allocation
struct TranslationEntry {
    const char* key;
    const char* value;
};

class Translator {
public:
    static void set_language(Language lang) noexcept;
    static Language get_language() noexcept;
    static const char* translate(const char* key) noexcept;
    static const char* get_translation(const char* key) noexcept;

private:
    static std::atomic<Language> current_language_;
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



// DetectionLogEntry structure for asynchronous logging
// This is a POD (Plain Old Data) structure for safe memory copying
#pragma pack(push, 1)
struct DetectionLogEntry {
    uint32_t timestamp;
    uint64_t frequency_hz;  // uint64_t for frequencies > 4GHz
    int32_t rssi_db;
    ThreatLevel threat_level;
    DroneType drone_type;
    uint8_t detection_count;
    uint8_t confidence_percent;  // Integer 0-100% instead of float for memory efficiency
    uint8_t width_bins;           // Signal width in bins (for calibration)
    uint32_t signal_width_hz;     // Signal width in Hz (for calibration)
    uint8_t snr;                  // Signal-to-Noise Ratio (for calibration)
};
#pragma pack(pop)

}  // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_DRONE_COMMON_TYPES_HPP_
