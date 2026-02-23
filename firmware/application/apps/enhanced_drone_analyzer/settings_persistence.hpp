// Settings persistence for Enhanced Drone Analyzer
// Zero-heap, RAII, template-based implementation
// Target: STM32F405 (ARM Cortex-M4)

#ifndef SETTINGS_PERSISTENCE_HPP_
#define SETTINGS_PERSISTENCE_HPP_

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <inttypes.h>
#include <array>
#include <ch.h>
#include "eda_safe_string.hpp"
#include "eda_constants.hpp"
#include "ui_drone_common_types.hpp"
#include "eda_locking.hpp"
#include "sd_card.hpp"
#include "lpc43xx_cpp.hpp"

namespace ui::apps::enhanced_drone_analyzer {

extern Mutex settings_buffer_mutex;
extern Mutex errno_mutex;

#ifdef __GNUC__
    #define FLASH_STORAGE __attribute__((section(".rodata")))
#else
    #define FLASH_STORAGE
#endif

enum SettingType : uint8_t {
    TYPE_BOOL = 0,
    TYPE_UINT32 = 1,
    TYPE_INT32 = 2,
    TYPE_STR = 3,
    TYPE_UINT64 = 4,
    TYPE_BITFIELD = 5
};

struct SettingMetadata {
    const char* key;
    uint16_t offset;
    uint8_t type;
    uint8_t bit_pos;
    int64_t min_val;
    int64_t max_val;
    const char* default_str;
};

#define SET_META(name, type_code, min, max, def) \
    { #name, static_cast<uint16_t>(offsetof(DroneAnalyzerSettings, name)), type_code, 0xFF, min, max, def }

#define SET_META_BIT(name, bit_idx, def) \
    { #name, static_cast<uint16_t>(offsetof(DroneAnalyzerSettings, name)), TYPE_BITFIELD, bit_idx, 0, 1, def }

constexpr size_t SETTINGS_COUNT = 52;

inline constexpr SettingMetadata SETTINGS_LUT[] FLASH_STORAGE = {
    SET_META_BIT(audio_flags, 0, "true"),
    SET_META(audio_alert_frequency_hz, TYPE_UINT32, 200, 20000, "800"),
    SET_META(audio_alert_duration_ms, TYPE_UINT32, 50, 5000, "500"),
    SET_META(audio_volume_level, TYPE_UINT32, 0, 100, "50"),
    SET_META_BIT(audio_flags, 1, "false"),
    SET_META(hardware_bandwidth_hz, TYPE_UINT32, 10000, 28000000, "24000000"),
    SET_META_BIT(hardware_flags, 0, "true"),
    SET_META_BIT(hardware_flags, 1, "false"),
    SET_META_BIT(hardware_flags, 2, "false"),
    SET_META(rx_phase_value, TYPE_UINT32, 0, 31, "15"),
    SET_META(lna_gain_db, TYPE_UINT32, 0, 40, "32"),
    SET_META(vga_gain_db, TYPE_UINT32, 0, 62, "20"),
    SET_META_BIT(hardware_flags, 3, "false"),

    // Scanning settings
    SET_META(scan_interval_ms, TYPE_UINT32, 100, 10000, "1000"),
    SET_META(rssi_threshold_db, TYPE_INT32, -120, 10, "-90"),
    SET_META_BIT(scanning_flags, 0, "false"),
    SET_META(wideband_min_freq_hz, TYPE_UINT64, 2400000000ULL, 7200000000ULL, "2400000000"),
    SET_META(wideband_max_freq_hz, TYPE_UINT64, 2400000001ULL, 7200000000ULL, "2500000000"),
    SET_META(wideband_slice_width_hz, TYPE_UINT32, 10000000, 28000000, "24000000"),
    SET_META_BIT(scanning_flags, 1, "true"),
    SET_META_BIT(scanning_flags, 2, "true"),

    // Detection settings
    SET_META_BIT(detection_flags, 0, "true"),
    SET_META(movement_sensitivity, TYPE_UINT32, 1, 5, "3"),
    SET_META(threat_level_threshold, TYPE_UINT32, 0, 4, "2"),
    SET_META(min_detection_count, TYPE_UINT32, 1, 10, "3"),
    SET_META(alert_persistence_threshold, TYPE_UINT32, 1, 10, "3"),
    SET_META_BIT(detection_flags, 1, "true"),

    // Logging settings
    SET_META_BIT(logging_flags, 0, "true"),
    SET_META(log_file_path, TYPE_STR, 64, 0, "/eda_logs"),
    SET_META(log_format, TYPE_STR, 8, 0, "CSV"),
    SET_META(max_log_file_size_kb, TYPE_UINT32, 1, 10240, "1024"),
    SET_META_BIT(logging_flags, 1, "true"),
    SET_META_BIT(logging_flags, 2, "true"),
    SET_META_BIT(logging_flags, 3, "true"),

    // Display settings
    SET_META(color_scheme, TYPE_STR, 32, 0, "DARK"),
    SET_META(font_size, TYPE_UINT32, 0, 2, "0"),
    SET_META(spectrum_density, TYPE_UINT32, 0, 2, "1"),
    SET_META(waterfall_speed, TYPE_UINT32, 1, 10, "5"),
    SET_META_BIT(display_flags, 0, "true"),
    SET_META_BIT(display_flags, 1, "true"),
    SET_META_BIT(display_flags, 2, "true"),
    SET_META_BIT(display_flags, 3, "true"),
    SET_META(frequency_ruler_style, TYPE_UINT32, 0, 6, "5"),
    SET_META(compact_ruler_tick_count, TYPE_UINT32, 3, 8, "4"),
    SET_META_BIT(display_flags, 4, "true"),

    SET_META(current_profile_name, TYPE_STR, 32, 0, "Default"),
    SET_META_BIT(profile_flags, 0, "true"),
    SET_META_BIT(profile_flags, 1, "false"),

    SET_META(freqman_path, TYPE_STR, 32, 0, "DRONES"),
    SET_META(settings_file_path, TYPE_STR, 64, 0, "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt"),
    SET_META(settings_version, TYPE_UINT32, 1, 999, "2")
};

inline bool is_value_in_range(int64_t val, int64_t min_val, int64_t max_val) {
    return val >= min_val && val <= max_val;
}

inline size_t const_strlen(const char* str) noexcept {
    return str ? strlen(str) : 0;
}

// RED TEAM FIX: Safe String Parsing
inline EDA::ErrorResult<uint64_t> safe_str_to_uint64(const char* str, int base = 10) noexcept {
    // Guard clause: null or empty string
    if (!str || *str == '\0') {
        return EDA::ErrorResult<uint64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    // Skip leading whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    // Guard clause: string was only whitespace
    if (*str == '\0') {
        return EDA::ErrorResult<uint64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    // RED TEAM FIX: Protect errno access with mutex
    // DIAMOND FIX: Use ERRNO_MUTEX lock order (value 2)
    MutexLock lock(errno_mutex, LockOrder::ERRNO_MUTEX);
    errno = 0;
    char* endptr = nullptr;
    unsigned long long val = strtoull(str, &endptr, base);

    if (errno == ERANGE) {
        return EDA::ErrorResult<uint64_t>::fail(EDA::ErrorCode::OUT_OF_RANGE);
    }

    if (endptr == nullptr || endptr == str) {
        return EDA::ErrorResult<uint64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    const char* p = endptr;
    while (*p == ' ' || *p == '\t' || *p == '\r') {
        p++;
    }
    if (*p != '\0') {
        return EDA::ErrorResult<uint64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    return EDA::ErrorResult<uint64_t>::ok(static_cast<uint64_t>(val));
}

// * * @brief Safely parse signed integer from string (errno protected) * * RED TEAM FIX: All errno access is protected by errno_mutex to prevent * race conditions when multiple threads call strtoll(). * * @param str String to parse * @param base Number base (default 10) * @return EDA::ErrorResult<int64_t> with parsed value or error * * @note Handles overflow, partial parsing, and invalid input * @note Clears errno before parsing to detect overflow * @note Skips leading whitespace for robustness
inline EDA::ErrorResult<int64_t> safe_str_to_int64(const char* str, int base = 10) noexcept {
    // Guard clause: null or empty string
    if (!str || *str == '\0') {
        return EDA::ErrorResult<int64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    // Skip leading whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    // Guard clause: string was only whitespace
    if (*str == '\0') {
        return EDA::ErrorResult<int64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    // RED TEAM FIX: Protect errno access with mutex
    // DIAMOND FIX: Use ERRNO_MUTEX lock order (value 2)
    MutexLock lock(errno_mutex, LockOrder::ERRNO_MUTEX);
    errno = 0;
    char* endptr = nullptr;
    long long val = strtoll(str, &endptr, base);

    if (errno == ERANGE) {
        return EDA::ErrorResult<int64_t>::fail(EDA::ErrorCode::OUT_OF_RANGE);
    }

    if (endptr == nullptr || endptr == str) {
        return EDA::ErrorResult<int64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    const char* p = endptr;
    while (*p == ' ' || *p == '\t' || *p == '\r') {
        p++;
    }
    if (*p != '\0') {
        return EDA::ErrorResult<int64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    return EDA::ErrorResult<int64_t>::ok(static_cast<int64_t>(val));
}

// * * @brief Safely parse boolean from string * @param str String to parse * @return EDA::ErrorResult<bool> with parsed value or error * * @note Accepts "true" or "false" (case-sensitive) * @note Rejects partial matches like "trueX"
inline EDA::ErrorResult<bool> safe_str_to_bool(const char* str) noexcept {
    // Guard clause: null or empty string
    if (!str || *str == '\0') {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    // Skip leading whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    // Guard clause: string was only whitespace
    if (*str == '\0') {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    // DIAMOND FIX: Use strcmp instead of strncmp for exact match
    // CRITICAL BUG FIX: strncmp("trueX", "true", 4) == 0 was accepting invalid values
    if (strcmp(str, "true") == 0) {
        return EDA::ErrorResult<bool>::ok(true);
    }
    if (strcmp(str, "false") == 0) {
        return EDA::ErrorResult<bool>::ok(false);
    }

    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
}

// SETTINGS BUFFER LOCK (RAII)
// * * @brief RAII wrapper for settings buffer mutex * * Ensures mutex is always unlocked when scope exits. * * @note Diamond Code: noexcept, deleted copy/move for safety * @note Uses ChibiOS chMtxUnlock() without parameter (newer API)
class SettingsBufferLock {
public:
    SettingsBufferLock() noexcept {
        chMtxLock(&settings_buffer_mutex);
    }

    ~SettingsBufferLock() noexcept {
        chMtxUnlock();  // ChibiOS newer API: unlocks last locked mutex
    }

    SettingsBufferLock(const SettingsBufferLock&) = delete;
    SettingsBufferLock& operator=(const SettingsBufferLock&) = delete;
};

// SETTINGS SERIALIZATION HELPER
// * * @brief Serialize setting to buffer * @param buf Output buffer * @param offset Offset in buffer * @param max_size Maximum buffer size * @param s Settings struct * @param meta Setting metadata * @return Number of bytes written * * @note Handles SpectrumMode enum class (stored as uint8_t underlying type)
inline size_t serialize_setting(char* buf, size_t offset, size_t max_size,
                                const DroneAnalyzerSettings& s, const SettingMetadata& meta) {
    // Guard clause: null check for buffer pointer
    if (!buf) return 0;
    if (offset >= max_size) return 0;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(&s) + meta.offset;
    switch (meta.type) {
        case TYPE_BOOL:
            return snprintf(buf + offset, max_size - offset, "%s=%s\n",
                   meta.key, *reinterpret_cast<const bool*>(data) ? "true" : "false");
        case TYPE_UINT32:
            // Handle SpectrumMode enum class (underlying type uint8_t, stored as uint32_t in LUT)
            if (strcmp(meta.key, "spectrum_mode") == 0) {
                auto mode = *reinterpret_cast<const SpectrumMode*>(data);
                // DIAMOND FIX: Use PRIu32 macro for platform-independent format specifier
                // uint32_t may be long unsigned int on some platforms, requiring %lu or PRIu32
                return snprintf(buf + offset, max_size - offset, "%s=%" PRIu32 "\n",
                       meta.key, static_cast<uint32_t>(mode));
            }
            return snprintf(buf + offset, max_size - offset, "%s=%" PRIu32 "\n",
                   meta.key, *reinterpret_cast<const uint32_t*>(data));
        case TYPE_INT32:
            return snprintf(buf + offset, max_size - offset, "%s=%" PRId32 "\n",
                   meta.key, *reinterpret_cast<const int32_t*>(data));
        case TYPE_UINT64:
            return snprintf(buf + offset, max_size - offset, "%s=%llu\n",
                   meta.key, *reinterpret_cast<const uint64_t*>(data));
        case TYPE_STR:
            return snprintf(buf + offset, max_size - offset, "%s=%s\n",
                   meta.key, reinterpret_cast<const char*>(data));
        case TYPE_BITFIELD: {
            bool bit_val = (*data >> meta.bit_pos) & 1;
            return snprintf(buf + offset, max_size - offset, "%s=%s\n",
                   meta.key, bit_val ? "true" : "false");
        }
    }
    return 0;
}

// DISPATCH OPERATION ENUM
enum class DispatchOp : uint8_t {
    PARSE,      ///< Parse string value
    RESET,       ///< Reset to default value
    VALIDATE    ///< Validate current value
};

// DISPATCH BY TYPE HELPER
// * * @brief Dispatch setting value based on type and operation * * @param op Operation to perform (PARSE, RESET, VALIDATE) * @param data_ptr Pointer to setting data in DroneAnalyzerSettings struct * @param meta Metadata for setting * @param value_str String value to parse (for PARSE operation) * @return true if operation succeeded, false otherwise * * @note DIAMOND FIX: Uses safe parsing functions with error detection * @note CRITICAL BUG FIX: Replaced unsafe strtoul/strtol with safe_str_to_uint64/safe_str_to_int64 * @note CRITICAL BUG FIX: Replaced insecure strncmp("true", 4) with safe_str_to_bool * @note Handles SpectrumMode enum class (stored as uint8_t underlying type)
inline bool dispatch_by_type(DispatchOp op, uint8_t* data_ptr,
                          const SettingMetadata& meta, char* value_str = nullptr) noexcept {
    switch (meta.type) {
        case TYPE_BOOL: {
            bool* ptr = reinterpret_cast<bool*>(data_ptr);
            bool val;

            if (op == DispatchOp::RESET) {
                val = (strcmp(meta.default_str, "true") == 0);
            } else {
                // DIAMOND FIX: Use safe_str_to_bool for exact match
                auto result = safe_str_to_bool(value_str);
                if (result.is_error()) {
                    return false;  // Invalid boolean value
                }
                val = result.value;
            }

            if (op != DispatchOp::VALIDATE) *ptr = val;
            return true;
        }
        case TYPE_UINT32: {
            // Handle SpectrumMode enum class (underlying type uint8_t, stored as uint32_t in LUT)
            if (strcmp(meta.key, "spectrum_mode") == 0) {
                SpectrumMode* ptr = reinterpret_cast<SpectrumMode*>(data_ptr);
                uint32_t val;

                if (op == DispatchOp::RESET) {
                    auto result = safe_str_to_uint64(meta.default_str);
                    if (result.is_error()) {
                        val = static_cast<uint32_t>(SpectrumMode::MEDIUM);
                    } else {
                        val = static_cast<uint32_t>(result.value);
                    }
                } else {
                    auto result = safe_str_to_uint64(value_str);
                    if (result.is_error()) {
                        return false;  // Invalid value
                    }
                    val = static_cast<uint32_t>(result.value);
                }

                if (op != DispatchOp::VALIDATE) {
                    *ptr = static_cast<SpectrumMode>(val);
                }
                return true;
            }

            // Standard uint32_t handling
            uint32_t* ptr = reinterpret_cast<uint32_t*>(data_ptr);
            uint32_t val;

            if (op == DispatchOp::RESET) {
                auto result = safe_str_to_uint64(meta.default_str);
                if (result.is_error()) {
                    val = 0;  // Fallback to 0 on error
                } else {
                    val = static_cast<uint32_t>(result.value);
                }
            } else {
                // DIAMOND FIX: Use safe_str_to_uint64 with error detection
                auto result = safe_str_to_uint64(value_str);
                if (result.is_error()) {
                    return false;  // Invalid value
                }
                val = static_cast<uint32_t>(result.value);
            }

            if (op != DispatchOp::VALIDATE) *ptr = val;
            return true;
        }
        case TYPE_INT32: {
            int32_t* ptr = reinterpret_cast<int32_t*>(data_ptr);
            int32_t val;

            if (op == DispatchOp::RESET) {
                auto result = safe_str_to_int64(meta.default_str);
                if (result.is_error()) {
                    val = 0;  // Fallback to 0 on error
                } else {
                    val = static_cast<int32_t>(result.value);
                }
            } else {
                // DIAMOND FIX: Use safe_str_to_int64 with error detection
                auto result = safe_str_to_int64(value_str);
                if (result.is_error()) {
                    return false;  // Invalid value
                }
                val = static_cast<int32_t>(result.value);
            }

            if (op != DispatchOp::VALIDATE) *ptr = val;
            return true;
        }
        case TYPE_UINT64: {
            uint64_t* ptr = reinterpret_cast<uint64_t*>(data_ptr);
            uint64_t val;

            if (op == DispatchOp::RESET) {
                auto result = safe_str_to_uint64(meta.default_str);
                if (result.is_error()) {
                    val = 0;  // Fallback to 0 on error
                } else {
                    val = result.value;
                }
            } else {
                // DIAMOND FIX: Use safe_str_to_uint64 with error detection
                auto result = safe_str_to_uint64(value_str);
                if (result.is_error()) {
                    return false;  // Invalid value
                }
                val = result.value;
            }

            if (op != DispatchOp::VALIDATE) *ptr = val;
            return true;
        }
        case TYPE_STR: {
            char* ptr = reinterpret_cast<char*>(data_ptr);
            const char* val = (op == DispatchOp::RESET) ? meta.default_str : value_str;
            if (op != DispatchOp::VALIDATE) safe_strcpy(ptr, val, static_cast<size_t>(meta.min_val));
            return true;
        }
        case TYPE_BITFIELD: {
            bool bit_val;

            if (op == DispatchOp::RESET) {
                bit_val = (strcmp(meta.default_str, "true") == 0);
            } else {
                // DIAMOND FIX: Use safe_str_to_bool for exact match
                auto result = safe_str_to_bool(value_str);
                if (result.is_error()) {
                    return false;  // Invalid boolean value
                }
                bit_val = result.value;
            }

            if (op != DispatchOp::VALIDATE) {
                if (bit_val) *data_ptr |= (1 << meta.bit_pos);
                else *data_ptr &= ~(1 << meta.bit_pos);
            }
            return true;
        }
    }
    return false;
}

// SETTINGS LOAD BUFFER (Timeouts)
static constexpr systime_t SD_CARD_MOUNT_TIMEOUT_MS = 2000;
static constexpr size_t MAX_READ_ITERATIONS = 10000;
static constexpr systime_t READ_TIMEOUT_MS = 5000;

// * * @brief Settings load buffer structure * * Stack usage: 256 bytes total (FIX #3: Reduced from 400 bytes) * - LINE_BUFFER_SIZE: 128 bytes (reduced from 144) * - READ_BUFFER_SIZE: 128 bytes (reduced from 256)
struct SettingsLoadBuffer {
    // / @brief Line buffer for individual settings lines
    // / @note 128 bytes: 112 bytes max line + 16 byte safety margin (FIX #3)
    static constexpr size_t LINE_BUFFER_SIZE = 128;

    // / @brief Read buffer for raw SD card data
    // / @note 128 bytes: Reduced to save stack space (FIX #3)
    static constexpr size_t READ_BUFFER_SIZE = 128;

    char line_buffer[LINE_BUFFER_SIZE];
    char read_buffer[READ_BUFFER_SIZE];
};

// SETTINGS LOAD BUFFER (Getter)
inline SettingsLoadBuffer& get_load_buffer() noexcept {
    static SettingsLoadBuffer buffer{};
    return buffer;
}

// M4 INTERRUPT MASKING
// * * @brief RAII wrapper for M4 interrupt masking * * Disables M4 interrupts during critical sections to prevent * race conditions with M4Core_IRQHandler. This prevents SV#8 * hard faults when M4 interrupts fire while M0 kernel is locked. * * Usage: * { * M4InterruptMask m4_mask; // Disables M4 interrupts * // Perform SD card operations here * } // Automatically re-enables M4 interrupts * * @note Diamond Code: noexcept, deleted copy/move for safety * @note Uses creg::m4txevent for M4->M0 interrupt control
class M4InterruptMask {
public:
    M4InterruptMask() noexcept {
        lpc43xx::creg::m4txevent::disable();
    }

    ~M4InterruptMask() noexcept {
        lpc43xx::creg::m4txevent::enable();
    }

    M4InterruptMask(const M4InterruptMask&) = delete;
    M4InterruptMask& operator=(const M4InterruptMask&) = delete;
};

// SETTINGS PERSISTENCE TEMPLATE
// * * @brief Settings persistence template * * Provides load/save/reset/validate operations for any settings type. * Uses look-up table for efficient parsing and validation. * * @tparam T Settings type (must be compatible with SETTINGS_LUT)
template<typename T>
class SettingsPersistence {
public:
    // / @brief Load settings from file
    // / @param settings Settings struct to populate
    // / @return EDA::ErrorResult<bool> with success/failure
    static EDA::ErrorResult<bool> load(T& settings);

    // / @brief Save settings to file
    // / @param settings Settings struct to save
    // / @return EDA::ErrorResult<bool> with success/failure
    static EDA::ErrorResult<bool> save(const T& settings);

    // / @brief Reset settings to defaults
    // / @param settings Settings struct to reset
    static void reset(T& settings);

    // / @brief Validate settings values
    // / @param settings Settings struct to validate
    // / @return EDA::ErrorResult<bool> with success/failure
    static EDA::ErrorResult<bool> validate(const T& settings);

    // / @brief Compatibility alias for reset_to_defaults
    static void reset_to_defaults(T& settings) { reset(settings); }

private:
    // / @brief Parse single settings line
    // / @param line Line to parse
    // / @param settings Settings struct to update
    // / @return true if parsing succeeded
    static bool parse_line(char* line, T& settings);
};

// IMPLEMENTATION: load()
template<typename T>
EDA::ErrorResult<bool> SettingsPersistence<T>::load(T& settings) {
    // FIX: Mask M4 interrupts during file I/O to prevent SV#8 hard fault
    M4InterruptMask m4_mask;

    // Validate SD card status with timeout
    systime_t sd_check_start = chTimeNow();
    while (sd_card::status() < sd_card::Status::Mounted) {
        if ((chTimeNow() - sd_check_start) > MS2ST(SD_CARD_MOUNT_TIMEOUT_MS)) {
            return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::TIMEOUT);
        }
        chThdSleepMilliseconds(50);
    }

    File file;
    const char* path = settings.settings_file_path;
    auto error = file.open(path);
    if (error) {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::FILE_IO_ERROR);
    }

    // Use thread-local buffer to prevent stack overflow
    auto& load_buf = get_load_buffer();
    char* line_buffer = load_buf.line_buffer;
    char* read_buffer = load_buf.read_buffer;
    size_t line_idx = 0;

    size_t total_bytes_read = 0;
    size_t lines_processed = 0;
    size_t read_iterations = 0;

    // Timeout protection for infinite loops
    systime_t read_start_time = chTimeNow();

    while (read_iterations < MAX_READ_ITERATIONS) {
        // Timeout guard: abort if reading takes too long
        if ((chTimeNow() - read_start_time) > MS2ST(READ_TIMEOUT_MS)) {
            file.close();
            return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::TIMEOUT);
        }

        auto read_res = file.read(read_buffer, SettingsLoadBuffer::READ_BUFFER_SIZE);
        if (read_res.is_error() || read_res.value() == 0) {
            break;
        }

        size_t bytes_read = read_res.value();
        total_bytes_read += bytes_read;

        if (total_bytes_read > EDA::Constants::MAX_SETTINGS_FILE_SIZE_64KB) {
            file.close();
            return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::BUFFER_OVERFLOW);
        }

        for (size_t i = 0; i < bytes_read; i++) {
            char c = read_buffer[i];

            if (c == '\n') {
                // Explicit bounds check before null terminator write
                // DIAMOND FIX: Consistent bounds checking before null terminator write
                // Use LINE_BUFFER_SIZE - 1 to ensure space for null terminator
                if (line_idx < (SettingsLoadBuffer::LINE_BUFFER_SIZE - 1)) {
                    line_buffer[line_idx] = '\0';
                    parse_line(line_buffer, settings);
                }
                line_idx = 0;
                lines_processed++;

                if (lines_processed > EDA::Constants::MAX_SETTINGS_LINES) {
                    file.close();
                    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::BUFFER_OVERFLOW);
                }
            } else if (c != '\r' && line_idx < (SettingsLoadBuffer::LINE_BUFFER_SIZE - 1)) {
                // DIAMOND FIX: Consistent bounds checking for character storage
                // Always leave space for null terminator
                line_buffer[line_idx++] = c;
            }
        }

        read_iterations++;
    }

    // Handle case where max iterations reached
    if (read_iterations >= MAX_READ_ITERATIONS) {
        file.close();
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::TIMEOUT);
    }

    // Handle last line without newline (with consistent bounds checking)
    // DIAMOND FIX: Use LINE_BUFFER_SIZE - 1 to ensure space for null terminator
    if (line_idx > 0 && line_idx < (SettingsLoadBuffer::LINE_BUFFER_SIZE - 1)) {
        line_buffer[line_idx] = '\0';
        parse_line(line_buffer, settings);
    }

    file.close();
    return EDA::ErrorResult<bool>::ok(true);
}

// IMPLEMENTATION: parse_line()
template<typename T>
bool SettingsPersistence<T>::parse_line(char* line, T& settings) {
    // Guard clause: null or empty line
    if (!line || *line == '\0') return true;

    size_t line_len = safe_strlen(line, EDA::Constants::MAX_LINE_LENGTH);
    if (line_len == 0) return true;

    char* equals = static_cast<char*>(memchr(line, '=', line_len));
    if (!equals) return true;

    *equals = '\0';
    char* key = line;
    char* value = equals + 1;

    size_t key_len = equals - key;
    size_t value_len = line + line_len - value;

    if (key_len == 0 || value_len == 0) return true;

    char* key_end = key + key_len - 1;
    while (key_end >= key && (*key_end == ' ' || *key_end == '\t')) {
        *key_end-- = '\0';
        key_len--;
    }

    while (*key == ' ' || *key == '\t') {
        key++;
        key_len--;
    }

    if (key_len == 0) return true;

    while (*value == ' ' || *value == '\t') {
        value++;
        value_len--;
    }

    char* value_end = value + value_len - 1;
    while (value_end >= value && (*value_end == ' ' || *value_end == '\t' || *value_end == '\r')) {
        *value_end-- = '\0';
        value_len--;
    }

    if (value_len == 0) return true;

    value[value_len] = '\0';

    for (size_t i = 0; i < SETTINGS_COUNT; i++) {
        if (strncmp(key, SETTINGS_LUT[i].key, key_len) == 0 &&
            const_strlen(SETTINGS_LUT[i].key) == key_len) {
            const auto& meta = SETTINGS_LUT[i];
            uint8_t* data_ptr = reinterpret_cast<uint8_t*>(&settings) + meta.offset;

            return dispatch_by_type(DispatchOp::PARSE, data_ptr, meta, value);
        }
    }

    return true;
}

// IMPLEMENTATION: save()
template<typename T>
EDA::ErrorResult<bool> SettingsPersistence<T>::save(const T& settings) {
    SettingsBufferLock lock;

    if (sd_card::status() < sd_card::Status::Mounted) {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::FILE_IO_ERROR);
    }

    File file;
    const char* path = settings.settings_file_path;
    // DIAMOND FIX: Use create() instead of append() to overwrite file
    // CRITICAL BUG FIX: append() causes file to grow indefinitely with duplicate data
    auto error = file.create(path);
    if (error) {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::FILE_IO_ERROR);
    }

    // Write header
    constexpr char SETTINGS_TEMPLATE[] = "# EDA Settings v2\n";
    auto write_result = file.write(SETTINGS_TEMPLATE, const_strlen(SETTINGS_TEMPLATE));
    if (write_result.is_error()) {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::FILE_IO_ERROR);
    }

    // Write each setting directly to file (no intermediate buffer)
    // Use small stack buffer for each line (128 bytes)
    char line_buffer[128];
    for (size_t i = 0; i < SETTINGS_COUNT; ++i) {
        int written = serialize_setting(line_buffer, 0, sizeof(line_buffer), settings, SETTINGS_LUT[i]);
        if (written <= 0 || static_cast<size_t>(written) > sizeof(line_buffer)) {
            return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::BUFFER_OVERFLOW);
        }

        write_result = file.write(line_buffer, static_cast<File::Size>(written));
        if (write_result.is_error()) {
            return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::FILE_IO_ERROR);
        }
    }

    return EDA::ErrorResult<bool>::ok(true);
}

// IMPLEMENTATION: reset()
template<typename T>
void SettingsPersistence<T>::reset(T& settings) {
    for (size_t i = 0; i < SETTINGS_COUNT; i++) {
        const auto& meta = SETTINGS_LUT[i];
        uint8_t* data_ptr = reinterpret_cast<uint8_t*>(&settings) + meta.offset;
        dispatch_by_type(DispatchOp::RESET, data_ptr, meta);
    }
}

// IMPLEMENTATION: validate()
template<typename T>
EDA::ErrorResult<bool> SettingsPersistence<T>::validate(const T& settings) {
    for (size_t i = 0; i < SETTINGS_COUNT; i++) {
        const auto& meta = SETTINGS_LUT[i];
        const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(&settings) + meta.offset;

        switch (meta.type) {
            case TYPE_BOOL:
                break;
            case TYPE_UINT32: {
                // Handle SpectrumMode enum class (stored as uint8_t, validated as uint32_t)
                if (strcmp(meta.key, "spectrum_mode") == 0) {
                    auto mode = *reinterpret_cast<const SpectrumMode*>(data_ptr);
                    uint32_t mode_val = static_cast<uint32_t>(mode);
                    if (mode_val < meta.min_val || mode_val > meta.max_val) {
                        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::OUT_OF_RANGE);
                    }
                    break;
                }
                // Standard uint32_t handling (for fields that are actually uint32_t)
                uint32_t val = *reinterpret_cast<const uint32_t*>(data_ptr);
                int64_t val_signed = static_cast<int64_t>(val);
                if (val_signed < meta.min_val || val_signed > meta.max_val) {
                    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::OUT_OF_RANGE);
                }
                break;
            }
            case TYPE_INT32: {
                int32_t val = *reinterpret_cast<const int32_t*>(data_ptr);
                int64_t min_val = meta.min_val;
                int64_t max_val = meta.max_val;
                if (val < min_val || val > max_val) {
                    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::OUT_OF_RANGE);
                }
                break;
            }
            case TYPE_UINT64: {
                uint64_t val = *reinterpret_cast<const uint64_t*>(data_ptr);
                if (val > 0x7FFFFFFFFFFFFFFFULL) {
                    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::OUT_OF_RANGE);
                }
                int64_t val_signed = static_cast<int64_t>(val);
                if (val_signed < meta.min_val || val_signed > meta.max_val) {
                    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::OUT_OF_RANGE);
                }
                break;
            }
            case TYPE_STR: {
                const char* str = reinterpret_cast<const char*>(data_ptr);
                if (str[0] == '\0') {
                    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
                }
                break;
            }
            case TYPE_BITFIELD:
                break;
        }
    }

    // Cross-field validation
    // NOTE: user_min_freq_hz and user_max_freq_hz are uint64_t in ui_drone_common_types.hpp
    if (settings.user_min_freq_hz >= settings.user_max_freq_hz) {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::OUT_OF_RANGE);
    }
    if (settings.wideband_min_freq_hz >= settings.wideband_max_freq_hz) {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::OUT_OF_RANGE);
    }

    return EDA::ErrorResult<bool>::ok(true);
}

} // namespace ui::apps::enhanced_drone_analyzer

#endif // SETTINGS_PERSISTENCE_HPP_
