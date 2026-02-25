/*
 * Copyright (C) 2023
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "doctest.h"
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <cerrno>

// ============================================================================
// Mock Types for Settings Persistence Testing
// ============================================================================

namespace ui::apps::enhanced_drone_analyzer {

// Mock ErrorResult type
namespace EDA {
enum class ErrorCode : uint8_t {
    OK = 0,
    INVALID_ARGUMENT = 1,
    OUT_OF_RANGE = 2
};

template<typename T>
struct ErrorResult {
    bool is_error_;
    T value;

    [[nodiscard]] bool is_error() const noexcept { return is_error_; }
    [[nodiscard]] static ErrorResult ok(T val) noexcept { return {false, val}; }
    [[nodiscard]] static ErrorResult fail(ErrorCode) noexcept { return {true, T{}}; }
};
}

// Mock Mutex (for test environment)
struct Mutex {
    int locked = 0;
};

struct LockOrder {
    static constexpr int ERRNO_MUTEX = 2;
};

class MutexLock {
public:
    MutexLock(Mutex& mutex, int) noexcept : mutex_(mutex) {
        mutex_.locked = 1;
    }
    ~MutexLock() noexcept {
        mutex_.locked = 0;
    }
private:
    Mutex& mutex_;
};

// Mock Mutex instances
Mutex settings_buffer_mutex;
Mutex errno_mutex;

// ============================================================================
// Settings Persistence Implementation (Inline for Testing)
// ============================================================================

// Inline wrapper for strnlen
inline size_t strnlen_wrapper(const char* str, size_t max_len) noexcept {
    if (!str) return 0;
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        ++len;
    }
    return len;
}

// Safe string copy with overlap detection
inline void safe_strcpy(char* dest, const char* src, size_t max_len) noexcept {
    if (!dest || !src || max_len == 0) {
        return;
    }

    // Check for overlapping buffers
    if (src < dest && src + max_len > dest) {
        memmove(dest, src, max_len);
        dest[max_len - 1] = '\0';
        return;
    }

    // No overlap - use fast character-by-character copy
    size_t i = 0;
    while (i < max_len - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

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

// Mock DroneAnalyzerSettings structure
struct DroneAnalyzerSettings {
    // Audio settings
    bool audio_enabled = true;
    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint32_t audio_volume_level = 50;
    bool audio_mute_on_detection = false;

    // Hardware settings
    uint32_t hardware_bandwidth_hz = 24000000;
    bool hardware_rf_amp_enabled = true;
    bool hardware_lna_enabled = false;
    bool hardware_vga_enabled = false;
    uint32_t rx_phase_value = 15;
    uint32_t lna_gain_db = 32;
    uint32_t vga_gain_db = 20;
    bool hardware_use_external_antenna = false;

    // Scanning settings
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    bool scanning_continuous_mode = false;
    uint64_t wideband_min_freq_hz = 2400000000ULL;
    uint64_t wideband_max_freq_hz = 2500000000ULL;
    uint32_t wideband_slice_width_hz = 24000000;
    bool scanning_auto_detect = true;
    bool scanning_adaptive_speed = true;

    // Detection settings
    bool detection_enabled = true;
    uint32_t movement_sensitivity = 3;
    uint32_t threat_level_threshold = 2;
    uint32_t min_detection_count = 3;
    uint32_t alert_persistence_threshold = 3;
    bool detection_use_ml_classifier = true;

    // Logging settings
    bool logging_enabled = true;
    char log_file_path[64] = "/eda_logs";
    char log_format[8] = "CSV";
    uint32_t max_log_file_size_kb = 1024;
    bool logging_timestamp = true;
    bool logging_frequency = true;
    bool logging_signal_strength = true;
    bool logging_threat_level = true;

    // Display settings
    char color_scheme[32] = "DARK";
    uint32_t font_size = 0;
    uint32_t spectrum_density = 1;
    uint32_t waterfall_speed = 5;
    bool display_show_spectrum = true;
    bool display_show_waterfall = true;
    bool display_show_frequency_ruler = true;
    bool display_show_signal_strength = true;
    uint32_t frequency_ruler_style = 5;
    uint32_t compact_ruler_tick_count = 4;
    bool display_compact_mode = true;

    // Profile settings
    char current_profile_name[32] = "Default";
    bool profile_auto_save = true;
    bool profile_auto_load = false;

    // Path settings
    char freqman_path[32] = "DRONES";
    char settings_file_path[64] = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
    uint32_t settings_version = 2;
};

// Safe string parsing functions
inline EDA::ErrorResult<uint64_t> safe_str_to_uint64(const char* str, int base = 10) noexcept {
    if (!str || *str == '\0') {
        return EDA::ErrorResult<uint64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    while (*str == ' ' || *str == '\t') {
        str++;
    }

    if (*str == '\0') {
        return EDA::ErrorResult<uint64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

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

inline EDA::ErrorResult<int64_t> safe_str_to_int64(const char* str, int base = 10) noexcept {
    if (!str || *str == '\0') {
        return EDA::ErrorResult<int64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    while (*str == ' ' || *str == '\t') {
        str++;
    }

    if (*str == '\0') {
        return EDA::ErrorResult<int64_t>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

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

inline EDA::ErrorResult<bool> safe_str_to_bool(const char* str) noexcept {
    if (!str || *str == '\0') {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    while (*str == ' ' || *str == '\t') {
        str++;
    }

    if (*str == '\0') {
        return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
    }

    if (strcmp(str, "true") == 0) {
        return EDA::ErrorResult<bool>::ok(true);
    }
    if (strcmp(str, "false") == 0) {
        return EDA::ErrorResult<bool>::ok(false);
    }

    return EDA::ErrorResult<bool>::fail(EDA::ErrorCode::INVALID_ARGUMENT);
}

// Dispatch operation enum
enum class DispatchOp : uint8_t {
    PARSE,
    RESET,
    VALIDATE
};

// Dispatch by type helper
inline bool dispatch_by_type(DispatchOp op, uint8_t* data_ptr,
                          const SettingMetadata& meta, char* value_str = nullptr) noexcept {
    switch (meta.type) {
        case TYPE_BOOL: {
            bool* ptr = reinterpret_cast<bool*>(data_ptr);
            bool val;

            if (op == DispatchOp::RESET) {
                val = (strcmp(meta.default_str, "true") == 0);
            } else {
                auto result = safe_str_to_bool(value_str);
                if (result.is_error()) {
                    return false;
                }
                val = result.value;
            }

            if (op != DispatchOp::VALIDATE) *ptr = val;
            return true;
        }
        case TYPE_UINT32: {
            uint32_t* ptr = reinterpret_cast<uint32_t*>(data_ptr);
            uint32_t val;

            if (op == DispatchOp::RESET) {
                auto result = safe_str_to_uint64(meta.default_str);
                if (result.is_error()) {
                    val = 0;
                } else {
                    val = static_cast<uint32_t>(result.value);
                }
            } else {
                auto result = safe_str_to_uint64(value_str);
                if (result.is_error()) {
                    return false;
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
                    val = 0;
                } else {
                    val = static_cast<int32_t>(result.value);
                }
            } else {
                auto result = safe_str_to_int64(value_str);
                if (result.is_error()) {
                    return false;
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
                    val = 0;
                } else {
                    val = result.value;
                }
            } else {
                auto result = safe_str_to_uint64(value_str);
                if (result.is_error()) {
                    return false;
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
                auto result = safe_str_to_bool(value_str);
                if (result.is_error()) {
                    return false;
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

// Serialize setting to buffer
inline size_t serialize_setting(char* buf, size_t offset, size_t max_size,
                                const DroneAnalyzerSettings& s, const SettingMetadata& meta) {
    if (!buf) return 0;
    if (offset >= max_size) return 0;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(&s) + meta.offset;
    switch (meta.type) {
        case TYPE_BOOL:
            return snprintf(buf + offset, max_size - offset, "%s=%s\n",
                   meta.key, *reinterpret_cast<const bool*>(data) ? "true" : "false");
        case TYPE_UINT32:
            return snprintf(buf + offset, max_size - offset, "%s=%u\n",
                   meta.key, *reinterpret_cast<const uint32_t*>(data));
        case TYPE_INT32:
            return snprintf(buf + offset, max_size - offset, "%s=%d\n",
                   meta.key, *reinterpret_cast<const int32_t*>(data));
        case TYPE_UINT64:
            return snprintf(buf + offset, max_size - offset, "%s=%llu\n",
                   meta.key, static_cast<unsigned long long>(*reinterpret_cast<const uint64_t*>(data)));
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

} // namespace ui::apps::enhanced_drone_analyzer

// ============================================================================
// Test: String Utilities
// ============================================================================

TEST_SUITE_BEGIN("Settings Persistence - String Utilities");

TEST_CASE("strnlen_wrapper returns correct length") {
    using namespace ui::apps::enhanced_drone_analyzer;

    CHECK_EQ(strnlen_wrapper("hello", 10), 5);
    CHECK_EQ(strnlen_wrapper("hello", 3), 3);
    CHECK_EQ(strnlen_wrapper("", 10), 0);
    CHECK_EQ(strnlen_wrapper(nullptr, 10), 0);
}

TEST_CASE("safe_strcpy copies correctly") {
    using namespace ui::apps::enhanced_drone_analyzer;

    char dest[10];
    safe_strcpy(dest, "hello", sizeof(dest));
    CHECK_EQ(strcmp(dest, "hello"), 0);

    safe_strcpy(dest, "verylongstring", sizeof(dest));
    CHECK_EQ(dest[9], '\0');
    CHECK_EQ(dest[0], 'v');
}

TEST_CASE("safe_strcpy handles null pointers") {
    using namespace ui::apps::enhanced_drone_analyzer;

    char dest[10];
    safe_strcpy(dest, nullptr, sizeof(dest));
    // Should not crash

    safe_strcpy(nullptr, "hello", sizeof(dest));
    // Should not crash

    safe_strcpy(dest, "hello", 0);
    // Should not crash
}

TEST_SUITE_END();

// ============================================================================
// Test: Safe String Parsing
// ============================================================================

TEST_SUITE_BEGIN("Settings Persistence - String Parsing");

SCENARIO("Parse uint64 from string") {
    GIVEN("A valid numeric string") {
        WHEN("Parsing '12345'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_uint64("12345");

            THEN("Parsing should succeed") {
                CHECK_FALSE(result.is_error());
            }

            THEN("Value should be correct") {
                CHECK_EQ(result.value, 12345ULL);
            }
        }
    }

    GIVEN("A string with leading whitespace") {
        WHEN("Parsing '  999'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_uint64("  999");

            THEN("Parsing should succeed") {
                CHECK_FALSE(result.is_error());
            }

            THEN("Value should be correct") {
                CHECK_EQ(result.value, 999ULL);
            }
        }
    }

    GIVEN("An invalid string") {
        WHEN("Parsing 'abc'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_uint64("abc");

            THEN("Parsing should fail") {
                CHECK(result.is_error());
            }
        }
    }

    GIVEN("A null pointer") {
        WHEN("Parsing nullptr") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_uint64(nullptr);

            THEN("Parsing should fail") {
                CHECK(result.is_error());
            }
        }
    }
}

SCENARIO("Parse int64 from string") {
    GIVEN("A valid numeric string") {
        WHEN("Parsing '-12345'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_int64("-12345");

            THEN("Parsing should succeed") {
                CHECK_FALSE(result.is_error());
            }

            THEN("Value should be correct") {
                CHECK_EQ(result.value, -12345LL);
            }
        }
    }

    GIVEN("A string with leading whitespace") {
        WHEN("Parsing '  -999'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_int64("  -999");

            THEN("Parsing should succeed") {
                CHECK_FALSE(result.is_error());
            }

            THEN("Value should be correct") {
                CHECK_EQ(result.value, -999LL);
            }
        }
    }

    GIVEN("An invalid string") {
        WHEN("Parsing 'abc'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_int64("abc");

            THEN("Parsing should fail") {
                CHECK(result.is_error());
            }
        }
    }
}

SCENARIO("Parse bool from string") {
    GIVEN("A valid boolean string") {
        WHEN("Parsing 'true'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_bool("true");

            THEN("Parsing should succeed") {
                CHECK_FALSE(result.is_error());
            }

            THEN("Value should be true") {
                CHECK(result.value);
            }
        }

        WHEN("Parsing 'false'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_bool("false");

            THEN("Parsing should succeed") {
                CHECK_FALSE(result.is_error());
            }

            THEN("Value should be false") {
                CHECK_FALSE(result.value);
            }
        }
    }

    GIVEN("A string with leading whitespace") {
        WHEN("Parsing '  true'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_bool("  true");

            THEN("Parsing should succeed") {
                CHECK_FALSE(result.is_error());
            }

            THEN("Value should be true") {
                CHECK(result.value);
            }
        }
    }

    GIVEN("An invalid boolean string") {
        WHEN("Parsing 'yes'") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_bool("yes");

            THEN("Parsing should fail") {
                CHECK(result.is_error());
            }
        }

        WHEN("Parsing 'TRUE' (case-sensitive)") {
            auto result = ui::apps::enhanced_drone_analyzer::safe_str_to_bool("TRUE");

            THEN("Parsing should fail") {
                CHECK(result.is_error());
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Dispatch by Type
// ============================================================================

TEST_SUITE_BEGIN("Settings Persistence - Dispatch");

SCENARIO("Dispatch bool setting") {
    GIVEN("A bool setting metadata") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        SettingMetadata meta = SET_META(audio_enabled, TYPE_BOOL, 0, 1, "true");

        WHEN("Parsing 'true'") {
            char value[] = "true";
            bool result = dispatch_by_type(DispatchOp::PARSE,
                                          reinterpret_cast<uint8_t*>(&settings.audio_enabled),
                                          meta, value);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Value should be true") {
                CHECK(settings.audio_enabled);
            }
        }

        WHEN("Parsing 'false'") {
            settings.audio_enabled = true;
            char value[] = "false";
            bool result = dispatch_by_type(DispatchOp::PARSE,
                                          reinterpret_cast<uint8_t*>(&settings.audio_enabled),
                                          meta, value);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Value should be false") {
                CHECK_FALSE(settings.audio_enabled);
            }
        }

        WHEN("Resetting to default") {
            settings.audio_enabled = false;
            bool result = dispatch_by_type(DispatchOp::RESET,
                                          reinterpret_cast<uint8_t*>(&settings.audio_enabled),
                                          meta, nullptr);

            THEN("Reset should succeed") {
                CHECK(result);
            }

            THEN("Value should be default (true)") {
                CHECK(settings.audio_enabled);
            }
        }
    }
}

SCENARIO("Dispatch uint32 setting") {
    GIVEN("A uint32 setting metadata") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        SettingMetadata meta = SET_META(audio_alert_frequency_hz, TYPE_UINT32, 200, 20000, "800");

        WHEN("Parsing '1000'") {
            char value[] = "1000";
            bool result = dispatch_by_type(DispatchOp::PARSE,
                                          reinterpret_cast<uint8_t*>(&settings.audio_alert_frequency_hz),
                                          meta, value);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Value should be 1000") {
                CHECK_EQ(settings.audio_alert_frequency_hz, 1000U);
            }
        }

        WHEN("Resetting to default") {
            settings.audio_alert_frequency_hz = 9999;
            bool result = dispatch_by_type(DispatchOp::RESET,
                                          reinterpret_cast<uint8_t*>(&settings.audio_alert_frequency_hz),
                                          meta, nullptr);

            THEN("Reset should succeed") {
                CHECK(result);
            }

            THEN("Value should be default (800)") {
                CHECK_EQ(settings.audio_alert_frequency_hz, 800U);
            }
        }
    }
}

SCENARIO("Dispatch int32 setting") {
    GIVEN("An int32 setting metadata") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        SettingMetadata meta = SET_META(rssi_threshold_db, TYPE_INT32, -120, 10, "-90");

        WHEN("Parsing '-100'") {
            char value[] = "-100";
            bool result = dispatch_by_type(DispatchOp::PARSE,
                                          reinterpret_cast<uint8_t*>(&settings.rssi_threshold_db),
                                          meta, value);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Value should be -100") {
                CHECK_EQ(settings.rssi_threshold_db, -100);
            }
        }

        WHEN("Resetting to default") {
            settings.rssi_threshold_db = -50;
            bool result = dispatch_by_type(DispatchOp::RESET,
                                          reinterpret_cast<uint8_t*>(&settings.rssi_threshold_db),
                                          meta, nullptr);

            THEN("Reset should succeed") {
                CHECK(result);
            }

            THEN("Value should be default (-90)") {
                CHECK_EQ(settings.rssi_threshold_db, -90);
            }
        }
    }
}

SCENARIO("Dispatch uint64 setting") {
    GIVEN("A uint64 setting metadata") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        SettingMetadata meta = SET_META(wideband_min_freq_hz, TYPE_UINT64, 2400000000ULL, 7200000000ULL, "2400000000");

        WHEN("Parsing '2500000000'") {
            char value[] = "2500000000";
            bool result = dispatch_by_type(DispatchOp::PARSE,
                                          reinterpret_cast<uint8_t*>(&settings.wideband_min_freq_hz),
                                          meta, value);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Value should be 2500000000") {
                CHECK_EQ(settings.wideband_min_freq_hz, 2500000000ULL);
            }
        }
    }
}

SCENARIO("Dispatch string setting") {
    GIVEN("A string setting metadata") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        SettingMetadata meta = SET_META(log_format, TYPE_STR, 8, 0, "CSV");

        WHEN("Parsing 'JSON'") {
            char value[] = "JSON";
            bool result = dispatch_by_type(DispatchOp::PARSE,
                                          reinterpret_cast<uint8_t*>(&settings.log_format),
                                          meta, value);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Value should be 'JSON'") {
                CHECK_EQ(strcmp(settings.log_format, "JSON"), 0);
            }
        }

        WHEN("Resetting to default") {
            strcpy(settings.log_format, "JSON");
            bool result = dispatch_by_type(DispatchOp::RESET,
                                          reinterpret_cast<uint8_t*>(&settings.log_format),
                                          meta, nullptr);

            THEN("Reset should succeed") {
                CHECK(result);
            }

            THEN("Value should be default ('CSV')") {
                CHECK_EQ(strcmp(settings.log_format, "CSV"), 0);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Serialize Setting
// ============================================================================

TEST_SUITE_BEGIN("Settings Persistence - Serialization");

SCENARIO("Serialize bool setting") {
    GIVEN("A bool setting") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        settings.audio_enabled = true;
        SettingMetadata meta = SET_META(audio_enabled, TYPE_BOOL, 0, 1, "true");

        WHEN("Serializing to buffer") {
            char buffer[64];
            size_t written = serialize_setting(buffer, 0, sizeof(buffer), settings, meta);

            THEN("Serialization should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "audio_enabled=true\n"), 0);
            }
        }
    }
}

SCENARIO("Serialize uint32 setting") {
    GIVEN("A uint32 setting") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        settings.audio_alert_frequency_hz = 1000;
        SettingMetadata meta = SET_META(audio_alert_frequency_hz, TYPE_UINT32, 200, 20000, "800");

        WHEN("Serializing to buffer") {
            char buffer[64];
            size_t written = serialize_setting(buffer, 0, sizeof(buffer), settings, meta);

            THEN("Serialization should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "audio_alert_frequency_hz=1000\n"), 0);
            }
        }
    }
}

SCENARIO("Serialize int32 setting") {
    GIVEN("An int32 setting") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        settings.rssi_threshold_db = -100;
        SettingMetadata meta = SET_META(rssi_threshold_db, TYPE_INT32, -120, 10, "-90");

        WHEN("Serializing to buffer") {
            char buffer[64];
            size_t written = serialize_setting(buffer, 0, sizeof(buffer), settings, meta);

            THEN("Serialization should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "rssi_threshold_db=-100\n"), 0);
            }
        }
    }
}

SCENARIO("Serialize uint64 setting") {
    GIVEN("A uint64 setting") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        settings.wideband_min_freq_hz = 2500000000ULL;
        SettingMetadata meta = SET_META(wideband_min_freq_hz, TYPE_UINT64, 2400000000ULL, 7200000000ULL, "2400000000");

        WHEN("Serializing to buffer") {
            char buffer[64];
            size_t written = serialize_setting(buffer, 0, sizeof(buffer), settings, meta);

            THEN("Serialization should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "wideband_min_freq_hz=2500000000\n"), 0);
            }
        }
    }
}

SCENARIO("Serialize string setting") {
    GIVEN("A string setting") {
        using namespace ui::apps::enhanced_drone_analyzer;
        DroneAnalyzerSettings settings;
        strcpy(settings.log_format, "JSON");
        SettingMetadata meta = SET_META(log_format, TYPE_STR, 8, 0, "CSV");

        WHEN("Serializing to buffer") {
            char buffer[64];
            size_t written = serialize_setting(buffer, 0, sizeof(buffer), settings, meta);

            THEN("Serialization should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "log_format=JSON\n"), 0);
            }
        }
    }
}

TEST_SUITE_END();
