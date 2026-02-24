// Database Parser Utilities for Enhanced Drone Analyzer
// Stage 4: File format parsers for freqman and CSV formats

#ifndef EDA_DATABASE_PARSER_HPP_
#define EDA_DATABASE_PARSER_HPP_

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include "eda_unified_database.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// Database Parser Namespace
// ============================================================================

namespace DatabaseParser {

// ============================================================================
// String Utilities (Zero-Heap)
// ============================================================================

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

// ============================================================================
// Freqman Format Parser
// ============================================================================

// Parse freqman format line: f=frequency,d=description
// Returns true if parsing succeeded, false otherwise
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

// ============================================================================
// CSV Format Parser
// ============================================================================

// Parse CSV format line: frequency,description
// Returns true if parsing succeeded, false otherwise
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

// ============================================================================
// Auto-Detect Format Parser
// ============================================================================

// Parse a line, auto-detecting format (freqman or CSV)
// Returns true if parsing succeeded, false otherwise
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

// ============================================================================
// Freqman Format Writer
// ============================================================================

// Write entry in freqman format: f=frequency,d=description
// Returns number of characters written (excluding null terminator)
inline size_t write_freqman_line(const UnifiedDroneEntry& entry, char* buffer, size_t buffer_size) noexcept {
    if (buffer == nullptr || buffer_size == 0) {
        return 0;
    }
    
    // Format: f=frequency,d=description
    int written = 0;
    
    if (entry.has_description()) {
        written = snprintf(buffer, buffer_size, "f=%" PRIu64 ",d=%s",
                          static_cast<uint64_t>(entry.frequency_hz),
                          entry.description);
    } else {
        written = snprintf(buffer, buffer_size, "f=%" PRIu64,
                          static_cast<uint64_t>(entry.frequency_hz));
    }
    
    if (written < 0 || static_cast<size_t>(written) >= buffer_size) {
        buffer[0] = '\0';
        return 0;
    }
    
    return static_cast<size_t>(written);
}

// ============================================================================
// CSV Format Writer
// ============================================================================

// Write entry in CSV format: frequency,description
// Returns number of characters written (excluding null terminator)
inline size_t write_csv_line(const UnifiedDroneEntry& entry, char* buffer, size_t buffer_size) noexcept {
    if (buffer == nullptr || buffer_size == 0) {
        return 0;
    }
    
    // Format: frequency,description
    int written = 0;
    
    if (entry.has_description()) {
        written = snprintf(buffer, buffer_size, "%" PRIu64 ",%s",
                          static_cast<uint64_t>(entry.frequency_hz),
                          entry.description);
    } else {
        written = snprintf(buffer, buffer_size, "%" PRIu64 ",",
                          static_cast<uint64_t>(entry.frequency_hz));
    }
    
    if (written < 0 || static_cast<size_t>(written) >= buffer_size) {
        buffer[0] = '\0';
        return 0;
    }
    
    return static_cast<size_t>(written);
}

} // namespace DatabaseParser

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_DATABASE_PARSER_HPP_
