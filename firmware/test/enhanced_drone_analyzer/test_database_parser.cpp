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
#include <array>

// ============================================================================
// Mock Types for Database Parser Testing
// ============================================================================

namespace ui::apps::enhanced_drone_analyzer {

// Mock Frequency type
using Frequency = uint64_t;

// Mock UnifiedDroneEntry structure (48 bytes)
struct UnifiedDroneEntry {
    Frequency frequency_hz = 0;
    char description[32] = "";
    uint8_t threat_level = 0;
    uint8_t frequency_band = 0;
    uint16_t flags = 0;
    uint8_t reserved[4] = {};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return frequency_hz > 0;
    }

    [[nodiscard]] constexpr bool has_description() const noexcept {
        return description[0] != '\0';
    }

    constexpr void clear() noexcept {
        frequency_hz = 0;
        description[0] = '\0';
        threat_level = 0;
        frequency_band = 0;
        flags = 0;
    }
};

// ============================================================================
// Database Parser Implementation (Inline for Testing)
// ============================================================================

namespace DatabaseParser {

// Safe string length with max limit
inline size_t safe_strlen(const char* str, size_t max_len) noexcept {
    if (str == nullptr) return 0;
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        ++len;
    }
    return len;
}

// Safe string copy with null termination
inline void safe_strcpy(char* dest, const char* src, size_t dest_size) noexcept {
    if (dest == nullptr || src == nullptr || dest_size == 0) {
        return;
    }

    size_t i = 0;
    while (i < dest_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

// Skip whitespace characters
inline const char* skip_whitespace(const char* str) noexcept {
    if (str == nullptr) return nullptr;

    while (*str == ' ' || *str == '\t') {
        ++str;
    }
    return str;
}

// Parse unsigned 64-bit integer from string
inline bool parse_uint64(const char* str, uint64_t& result) noexcept {
    if (str == nullptr || *str == '\0') {
        return false;
    }

    result = 0;

    while (*str >= '0' && *str <= '9') {
        uint64_t digit = static_cast<uint64_t>(*str - '0');

        // Check for overflow
        if (result > (UINT64_MAX - digit) / 10) {
            return false;
        }

        result = result * 10 + digit;
        ++str;
    }

    return true;
}

// Parse freqman format line: f=frequency,d=description
inline bool parse_freqman_line(const char* line, UnifiedDroneEntry& entry) noexcept {
    if (line == nullptr) {
        return false;
    }

    // Clear entry
    entry.clear();

    // Skip leading whitespace
    line = skip_whitespace(line);

    // Look for 'f=' prefix
    if (line[0] != 'f' || line[1] != '=') {
        return false;
    }

    // Move past "f="
    line += 2;

    // Find the frequency value (until comma or end)
    const char* freq_start = line;
    while (*line != '\0' && *line != ',' && *line != ' ' && *line != '\t') {
        ++line;
    }

    // Extract frequency string
    size_t freq_len = static_cast<size_t>(line - freq_start);
    if (freq_len == 0 || freq_len >= 32) {
        return false;
    }

    // Parse frequency
    char freq_buffer[32];
    safe_strcpy(freq_buffer, freq_start, freq_len + 1);

    uint64_t frequency = 0;
    if (!parse_uint64(freq_buffer, frequency)) {
        return false;
    }

    entry.frequency_hz = static_cast<Frequency>(frequency);

    // Look for description part
    if (*line == ',') {
        ++line;

        // Skip whitespace after comma
        line = skip_whitespace(line);

        // Look for 'd=' prefix
        if (line[0] == 'd' && line[1] == '=') {
            line += 2;

            // Skip whitespace after 'd='
            line = skip_whitespace(line);

            // Copy description (up to 31 characters)
            size_t desc_len = safe_strlen(line, 32);
            if (desc_len > 0) {
                safe_strcpy(entry.description, line, sizeof(entry.description));
            }
        }
    }

    return entry.frequency_hz > 0;
}

// Parse CSV format line: frequency,description
inline bool parse_csv_line(const char* line, UnifiedDroneEntry& entry) noexcept {
    if (line == nullptr) {
        return false;
    }

    // Clear entry
    entry.clear();

    // Skip leading whitespace
    line = skip_whitespace(line);

    // Find the comma separator
    const char* comma = strchr(line, ',');
    if (comma == nullptr) {
        // No comma - try to parse as just a frequency
        uint64_t frequency = 0;
        if (parse_uint64(line, frequency) && frequency > 0) {
            entry.frequency_hz = static_cast<Frequency>(frequency);
            return true;
        }
        return false;
    }

    // Extract frequency part
    size_t freq_len = static_cast<size_t>(comma - line);
    if (freq_len == 0 || freq_len >= 32) {
        return false;
    }

    char freq_buffer[32];
    safe_strcpy(freq_buffer, line, freq_len + 1);

    uint64_t frequency = 0;
    if (!parse_uint64(freq_buffer, frequency)) {
        return false;
    }

    entry.frequency_hz = static_cast<Frequency>(frequency);

    // Extract description part (after comma)
    const char* desc_start = comma + 1;
    desc_start = skip_whitespace(desc_start);

    size_t desc_len = safe_strlen(desc_start, 32);
    if (desc_len > 0) {
        safe_strcpy(entry.description, desc_start, sizeof(entry.description));
    }

    return entry.frequency_hz > 0;
}

// Parse a line, auto-detecting format (freqman or CSV)
inline bool parse_line(const char* line, UnifiedDroneEntry& entry) noexcept {
    if (line == nullptr || *line == '\0') {
        return false;
    }

    // Skip leading whitespace
    const char* trimmed = skip_whitespace(line);

    // Skip comment lines
    if (*trimmed == '#' || *trimmed == ';') {
        return false;
    }

    // Detect format by looking for 'f=' prefix
    if (trimmed[0] == 'f' && trimmed[1] == '=') {
        return parse_freqman_line(trimmed, entry);
    }

    // Try CSV format
    return parse_csv_line(trimmed, entry);
}

// Write entry in freqman format: f=frequency,d=description
inline size_t write_freqman_line(const UnifiedDroneEntry& entry, char* buffer, size_t buffer_size) noexcept {
    if (buffer == nullptr || buffer_size == 0) {
        return 0;
    }

    // Format: f=frequency,d=description
    int written = 0;

    if (entry.has_description()) {
        written = snprintf(buffer, buffer_size, "f=%llu,d=%s",
                          static_cast<unsigned long long>(entry.frequency_hz),
                          entry.description);
    } else {
        written = snprintf(buffer, buffer_size, "f=%llu",
                          static_cast<unsigned long long>(entry.frequency_hz));
    }

    if (written < 0 || static_cast<size_t>(written) >= buffer_size) {
        buffer[0] = '\0';
        return 0;
    }

    return static_cast<size_t>(written);
}

// Write entry in CSV format: frequency,description
inline size_t write_csv_line(const UnifiedDroneEntry& entry, char* buffer, size_t buffer_size) noexcept {
    if (buffer == nullptr || buffer_size == 0) {
        return 0;
    }

    // Format: frequency,description
    int written = 0;

    if (entry.has_description()) {
        written = snprintf(buffer, buffer_size, "%llu,%s",
                          static_cast<unsigned long long>(entry.frequency_hz),
                          entry.description);
    } else {
        written = snprintf(buffer, buffer_size, "%llu,",
                          static_cast<unsigned long long>(entry.frequency_hz));
    }

    if (written < 0 || static_cast<size_t>(written) >= buffer_size) {
        buffer[0] = '\0';
        return 0;
    }

    return static_cast<size_t>(written);
}

} // namespace DatabaseParser

} // namespace ui::apps::enhanced_drone_analyzer

// ============================================================================
// Test: String Utilities
// ============================================================================

TEST_SUITE_BEGIN("Database Parser - String Utilities");

TEST_CASE("safe_strlen returns correct length") {
    using namespace ui::apps::enhanced_drone_analyzer::DatabaseParser;

    CHECK_EQ(safe_strlen("hello", 10), 5);
    CHECK_EQ(safe_strlen("hello", 3), 3);
    CHECK_EQ(safe_strlen("", 10), 0);
    CHECK_EQ(safe_strlen(nullptr, 10), 0);
}

TEST_CASE("safe_strcpy copies correctly") {
    using namespace ui::apps::enhanced_drone_analyzer::DatabaseParser;

    char dest[10];
    safe_strcpy(dest, "hello", sizeof(dest));
    CHECK_EQ(strcmp(dest, "hello"), 0);

    safe_strcpy(dest, "verylongstring", sizeof(dest));
    CHECK_EQ(dest[9], '\0');  // Should be null-terminated
    CHECK_EQ(dest[0], 'v');   // Should have copied first 9 chars
}

TEST_CASE("skip_whitespace skips leading spaces and tabs") {
    using namespace ui::apps::enhanced_drone_analyzer::DatabaseParser;

    CHECK_EQ(skip_whitespace("  hello"), "hello");
    CHECK_EQ(skip_whitespace("\thello"), "hello");
    CHECK_EQ(skip_whitespace(" \t hello"), "hello");
    CHECK_EQ(skip_whitespace("hello"), "hello");
    CHECK_EQ(skip_whitespace(nullptr), nullptr);
}

TEST_CASE("parse_uint64 parses valid numbers") {
    using namespace ui::apps::enhanced_drone_analyzer::DatabaseParser;

    uint64_t result;
    CHECK(parse_uint64("123", result));
    CHECK_EQ(result, 123);

    CHECK(parse_uint64("0", result));
    CHECK_EQ(result, 0);

    CHECK(parse_uint64("18446744073709551615", result));  // UINT64_MAX
    CHECK_EQ(result, UINT64_MAX);
}

TEST_CASE("parse_uint64 rejects invalid input") {
    using namespace ui::apps::enhanced_drone_analyzer::DatabaseParser;

    uint64_t result;
    CHECK_FALSE(parse_uint64(nullptr, result));
    CHECK_FALSE(parse_uint64("", result));
    CHECK_FALSE(parse_uint64("abc", result));
    CHECK_FALSE(parse_uint64("123abc", result));
}

TEST_SUITE_END();

// ============================================================================
// Test: Freqman Format Parser
// ============================================================================

TEST_SUITE_BEGIN("Database Parser - Freqman Format");

SCENARIO("Parse basic freqman entry") {
    GIVEN("A freqman format line with frequency only") {
        const char* line = "f=2400000000";

        WHEN("Parsing the line") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_freqman_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 2400000000ULL);
            }

            THEN("Entry should be valid") {
                CHECK(entry.is_valid());
            }

            THEN("Description should be empty") {
                CHECK_FALSE(entry.has_description());
            }
        }
    }
}

SCENARIO("Parse freqman entry with description") {
    GIVEN("A freqman format line with frequency and description") {
        const char* line = "f=2400000000,d=DJI Phantom";

        WHEN("Parsing the line") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_freqman_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 2400000000ULL);
            }

            THEN("Description should be correct") {
                CHECK_EQ(strcmp(entry.description, "DJI Phantom"), 0);
            }

            THEN("Entry should have description") {
                CHECK(entry.has_description());
            }
        }
    }
}

SCENARIO("Parse freqman entry with whitespace") {
    GIVEN("A freqman format line with leading whitespace") {
        const char* line = "  f=5800000000,d=FPV Drone";

        WHEN("Parsing the line") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_freqman_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 5800000000ULL);
            }

            THEN("Description should be correct") {
                CHECK_EQ(strcmp(entry.description, "FPV Drone"), 0);
            }
        }
    }
}

SCENARIO("Parse invalid freqman entries") {
    GIVEN("Various invalid freqman format lines") {
        WHEN("Parsing line without f= prefix") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_freqman_line("2400000000", entry);

            THEN("Parsing should fail") {
                CHECK_FALSE(result);
            }
        }

        WHEN("Parsing line with null pointer") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_freqman_line(nullptr, entry);

            THEN("Parsing should fail") {
                CHECK_FALSE(result);
            }
        }

        WHEN("Parsing line with zero frequency") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_freqman_line("f=0", entry);

            THEN("Parsing should fail") {
                CHECK_FALSE(result);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: CSV Format Parser
// ============================================================================

TEST_SUITE_BEGIN("Database Parser - CSV Format");

SCENARIO("Parse basic CSV entry") {
    GIVEN("A CSV format line with frequency only") {
        const char* line = "2400000000";

        WHEN("Parsing the line") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_csv_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 2400000000ULL);
            }

            THEN("Entry should be valid") {
                CHECK(entry.is_valid());
            }

            THEN("Description should be empty") {
                CHECK_FALSE(entry.has_description());
            }
        }
    }
}

SCENARIO("Parse CSV entry with description") {
    GIVEN("A CSV format line with frequency and description") {
        const char* line = "2400000000,DJI Phantom";

        WHEN("Parsing the line") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_csv_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 2400000000ULL);
            }

            THEN("Description should be correct") {
                CHECK_EQ(strcmp(entry.description, "DJI Phantom"), 0);
            }

            THEN("Entry should have description") {
                CHECK(entry.has_description());
            }
        }
    }
}

SCENARIO("Parse CSV entry with whitespace") {
    GIVEN("A CSV format line with whitespace") {
        const char* line = "  5800000000,  FPV Drone";

        WHEN("Parsing the line") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_csv_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 5800000000ULL);
            }

            THEN("Description should be correct (trimmed)") {
                CHECK_EQ(strcmp(entry.description, "FPV Drone"), 0);
            }
        }
    }
}

SCENARIO("Parse invalid CSV entries") {
    GIVEN("Various invalid CSV format lines") {
        WHEN("Parsing line with non-numeric frequency") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_csv_line("abc,description", entry);

            THEN("Parsing should fail") {
                CHECK_FALSE(result);
            }
        }

        WHEN("Parsing line with null pointer") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_csv_line(nullptr, entry);

            THEN("Parsing should fail") {
                CHECK_FALSE(result);
            }
        }

        WHEN("Parsing empty line") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_csv_line("", entry);

            THEN("Parsing should fail") {
                CHECK_FALSE(result);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Auto-Detect Parser
// ============================================================================

TEST_SUITE_BEGIN("Database Parser - Auto-Detect");

SCENARIO("Auto-detect freqman format") {
    GIVEN("A freqman format line") {
        const char* line = "f=2400000000,d=DJI Phantom";

        WHEN("Parsing with auto-detect") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 2400000000ULL);
            }

            THEN("Description should be correct") {
                CHECK_EQ(strcmp(entry.description, "DJI Phantom"), 0);
            }
        }
    }
}

SCENARIO("Auto-detect CSV format") {
    GIVEN("A CSV format line") {
        const char* line = "2400000000,DJI Phantom";

        WHEN("Parsing with auto-detect") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_line(line, entry);

            THEN("Parsing should succeed") {
                CHECK(result);
            }

            THEN("Frequency should be correct") {
                CHECK_EQ(entry.frequency_hz, 2400000000ULL);
            }

            THEN("Description should be correct") {
                CHECK_EQ(strcmp(entry.description, "DJI Phantom"), 0);
            }
        }
    }
}

SCENARIO("Skip comment lines") {
    GIVEN("Comment lines") {
        WHEN("Parsing hash comment") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_line("# This is a comment", entry);

            THEN("Parsing should fail (comment skipped)") {
                CHECK_FALSE(result);
            }
        }

        WHEN("Parsing semicolon comment") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_line("; This is a comment", entry);

            THEN("Parsing should fail (comment skipped)") {
                CHECK_FALSE(result);
            }
        }

        WHEN("Parsing comment with whitespace") {
            ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
            bool result = ui::apps::enhanced_drone_analyzer::DatabaseParser::parse_line("  # Comment", entry);

            THEN("Parsing should fail (comment skipped)") {
                CHECK_FALSE(result);
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: Freqman Format Writer
// ============================================================================

TEST_SUITE_BEGIN("Database Parser - Freqman Writer");

SCENARIO("Write freqman entry without description") {
    GIVEN("An entry with frequency only") {
        ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
        entry.frequency_hz = 2400000000ULL;

        WHEN("Writing to buffer") {
            char buffer[64];
            size_t written = ui::apps::enhanced_drone_analyzer::DatabaseParser::write_freqman_line(entry, buffer, sizeof(buffer));

            THEN("Writing should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "f=2400000000"), 0);
            }
        }
    }
}

SCENARIO("Write freqman entry with description") {
    GIVEN("An entry with frequency and description") {
        ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
        entry.frequency_hz = 5800000000ULL;
        ui::apps::enhanced_drone_analyzer::DatabaseParser::safe_strcpy(entry.description, "FPV Drone", sizeof(entry.description));

        WHEN("Writing to buffer") {
            char buffer[64];
            size_t written = ui::apps::enhanced_drone_analyzer::DatabaseParser::write_freqman_line(entry, buffer, sizeof(buffer));

            THEN("Writing should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "f=5800000000,d=FPV Drone"), 0);
            }
        }
    }
}

SCENARIO("Write freqman entry with small buffer") {
    GIVEN("An entry and a small buffer") {
        ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
        entry.frequency_hz = 2400000000ULL;

        WHEN("Writing to insufficient buffer") {
            char buffer[5];
            size_t written = ui::apps::enhanced_drone_analyzer::DatabaseParser::write_freqman_line(entry, buffer, sizeof(buffer));

            THEN("Writing should fail") {
                CHECK(written == 0);
            }

            THEN("Buffer should be empty") {
                CHECK_EQ(buffer[0], '\0');
            }
        }
    }
}

TEST_SUITE_END();

// ============================================================================
// Test: CSV Format Writer
// ============================================================================

TEST_SUITE_BEGIN("Database Parser - CSV Writer");

SCENARIO("Write CSV entry without description") {
    GIVEN("An entry with frequency only") {
        ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
        entry.frequency_hz = 2400000000ULL;

        WHEN("Writing to buffer") {
            char buffer[64];
            size_t written = ui::apps::enhanced_drone_analyzer::DatabaseParser::write_csv_line(entry, buffer, sizeof(buffer));

            THEN("Writing should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "2400000000,"), 0);
            }
        }
    }
}

SCENARIO("Write CSV entry with description") {
    GIVEN("An entry with frequency and description") {
        ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
        entry.frequency_hz = 5800000000ULL;
        ui::apps::enhanced_drone_analyzer::DatabaseParser::safe_strcpy(entry.description, "FPV Drone", sizeof(entry.description));

        WHEN("Writing to buffer") {
            char buffer[64];
            size_t written = ui::apps::enhanced_drone_analyzer::DatabaseParser::write_csv_line(entry, buffer, sizeof(buffer));

            THEN("Writing should succeed") {
                CHECK(written > 0);
            }

            THEN("Output should be correct") {
                CHECK_EQ(strcmp(buffer, "5800000000,FPV Drone"), 0);
            }
        }
    }
}

SCENARIO("Write CSV entry with small buffer") {
    GIVEN("An entry and a small buffer") {
        ui::apps::enhanced_drone_analyzer::UnifiedDroneEntry entry;
        entry.frequency_hz = 2400000000ULL;

        WHEN("Writing to insufficient buffer") {
            char buffer[5];
            size_t written = ui::apps::enhanced_drone_analyzer::DatabaseParser::write_csv_line(entry, buffer, sizeof(buffer));

            THEN("Writing should fail") {
                CHECK(written == 0);
            }

            THEN("Buffer should be empty") {
                CHECK_EQ(buffer[0], '\0');
            }
        }
    }
}

TEST_SUITE_END();
