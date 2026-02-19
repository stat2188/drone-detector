/**
 * @file eda_safe_string.hpp
 * @brief Safe String Operations for Enhanced Drone Analyzer
 *
 * DIAMOND CODE STANDARDS:
 * - Zero-heap allocation (no std::string, no malloc)
 * - Fixed-size buffers only
 * - Bounds checking on all operations
 * - noexcept for exception-free operation
 * - Guard clauses for early error returns
 * - Doxygen comments for public APIs
 *
 * @target STM32F405 (ARM Cortex-M4, 128KB RAM)
 * @os ChibiOS (bare-metal RTOS)
 */

#ifndef EDA_SAFE_STRING_HPP_
#define EDA_SAFE_STRING_HPP_

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <string_view>
#include "eda_constants.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// SAFE STRING COPY
// ========================================

/**
 * @brief Safely copy string with bounds checking
 *
 * Copies source string to destination buffer with guaranteed null-termination.
 * Prevents buffer overflow by limiting copy to buffer_size - 1 characters.
 *
 * @param dest Destination buffer (must be at least buffer_size bytes)
 * @param src Source string (may be nullptr)
 * @param buffer_size Size of destination buffer in bytes
 * @return Number of characters copied (excluding null terminator)
 *
 * USAGE:
 *   char buffer[64];
 *   safe_strcpy(buffer, "Hello World", sizeof(buffer));
 *   // buffer now contains "Hello World\0"
 *
 * @note Always null-terminates destination
 * @note Returns 0 if dest or buffer_size is 0
 * @note Returns 0 if src is nullptr
 * @note Does NOT write beyond buffer_size - 1
 */
inline size_t safe_strcpy(char* dest, const char* src, size_t buffer_size) noexcept {
    // Guard clause: Check for null or zero-size buffer
    if (!dest || buffer_size == 0) {
        return 0;
    }
    
    // Guard clause: Check for null source
    if (!src) {
        dest[0] = '\0';
        return 0;
    }
    
    // Guard clause: Single-byte buffer
    if (buffer_size == 1) {
        dest[0] = '\0';
        return 0;
    }
    
    // Copy characters with bounds checking
    size_t i = 0;
    while (i < buffer_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    
    // Always null-terminate
    dest[i] = '\0';
    
    return i;
}

// ========================================
// SAFE STRING CONCATENATE
// ========================================

/**
 * @brief Safely concatenate string with bounds checking
 *
 * Appends source string to destination buffer with guaranteed null-termination.
 * Prevents buffer overflow by limiting total length to buffer_size - 1.
 *
 * @param dest Destination buffer (must be at least buffer_size bytes)
 * @param src Source string to append (may be nullptr)
 * @param buffer_size Size of destination buffer in bytes
 * @return Number of characters appended (excluding null terminator)
 *
 * USAGE:
 *   char buffer[64];
 *   safe_strcpy(buffer, "Hello", sizeof(buffer));
 *   safe_strcat(buffer, " World", sizeof(buffer));
 *   // buffer now contains "Hello World\0"
 *
 * @note Always null-terminates destination
 * @note Returns 0 if dest or buffer_size is 0
 * @note Returns 0 if src is nullptr
 * @note Does NOT write beyond buffer_size - 1
 */
inline size_t safe_strcat(char* dest, const char* src, size_t buffer_size) noexcept {
    // Guard clause: Check for null or zero-size buffer
    if (!dest || buffer_size == 0) {
        return 0;
    }
    
    // Guard clause: Check for null source
    if (!src) {
        return 0;
    }
    
    // Find current length of destination
    size_t dest_len = 0;
    while (dest_len < buffer_size - 1 && dest[dest_len] != '\0') {
        ++dest_len;
    }
    
    // Guard clause: Buffer is already full
    if (dest_len >= buffer_size - 1) {
        return 0;
    }
    
    // Append source with bounds checking
    size_t i = 0;
    while (dest_len + i < buffer_size - 1 && src[i] != '\0') {
        dest[dest_len + i] = src[i];
        ++i;
    }
    
    // Always null-terminate
    dest[dest_len + i] = '\0';
    
    return i;
}

// ========================================
// SAFE STRING LENGTH
// ========================================

/**
 * @brief Safely get string length with bounds checking
 *
 * Returns length of null-terminated string, limited to max_len.
 * Prevents reading beyond buffer bounds.
 *
 * @param str String to measure (may be nullptr)
 * @param max_len Maximum length to check
 * @return Length of string (up to max_len)
 *
 * USAGE:
 *   char buffer[64];
 *   size_t len = safe_strlen(buffer, sizeof(buffer));
 *
 * @note Returns 0 if str is nullptr
 * @note Returns max_len if no null terminator found within max_len
 */
inline size_t safe_strlen(const char* str, size_t max_len) noexcept {
    // Guard clause: Check for null pointer
    if (!str) {
        return 0;
    }
    
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        ++len;
    }
    
    return len;
}

// ========================================
// SAFE STRING COMPARE
// ========================================

/**
 * @brief Safely compare two strings with bounds checking
 *
 * Compares two strings up to max_len characters.
 * Prevents reading beyond buffer bounds.
 *
 * @param str1 First string (may be nullptr)
 * @param str2 Second string (may be nullptr)
 * @param max_len Maximum length to compare
 * @return Negative if str1 < str2, zero if equal, positive if str1 > str2
 *
 * USAGE:
 *   char buffer1[64] = "Hello";
 *   char buffer2[64] = "World";
 *   int cmp = safe_strcmp(buffer1, buffer2, sizeof(buffer1));
 *
 * @note Treats nullptr as empty string
 * @note Returns 0 if both are nullptr
 */
inline int safe_strcmp(const char* str1, const char* str2, size_t max_len) noexcept {
    // Guard clause: Both null
    if (!str1 && !str2) {
        return 0;
    }
    
    // Guard clause: First is null
    if (!str1) {
        return -1;
    }
    
    // Guard clause: Second is null
    if (!str2) {
        return 1;
    }
    
    // Compare up to max_len characters
    for (size_t i = 0; i < max_len; ++i) {
        if (str1[i] == '\0' && str2[i] == '\0') {
            return 0;  // Both ended
        }
        if (str1[i] == '\0') {
            return -1;  // str1 ended first
        }
        if (str2[i] == '\0') {
            return 1;   // str2 ended first
        }
        if (str1[i] != str2[i]) {
            return static_cast<int>(static_cast<unsigned char>(str1[i])) -
                   static_cast<int>(static_cast<unsigned char>(str2[i]));
        }
    }
    
    return 0;  // Equal up to max_len
}

// ========================================
// SAFE STRING N-COPY
// ========================================

/**
 * @brief Safely copy N characters with bounds checking
 *
 * Copies up to n characters from source to destination buffer.
 * Always null-terminates destination.
 *
 * @param dest Destination buffer (must be at least buffer_size bytes)
 * @param src Source string (may be nullptr)
 * @param n Maximum number of characters to copy
 * @param buffer_size Size of destination buffer in bytes
 * @return Number of characters copied (excluding null terminator)
 *
 * USAGE:
 *   char buffer[64];
 *   safe_strncpy(buffer, "Hello World", 5, sizeof(buffer));
 *   // buffer now contains "Hello\0"
 *
 * @note Always null-terminates destination
 * @note Copies min(n, buffer_size - 1) characters
 * @note Returns 0 if dest or buffer_size is 0
 * @note Returns 0 if src is nullptr
 */
inline size_t safe_strncpy(char* dest, const char* src, size_t n, size_t buffer_size) noexcept {
    // Guard clause: Check for null or zero-size buffer
    if (!dest || buffer_size == 0) {
        return 0;
    }
    
    // Guard clause: Check for null source
    if (!src) {
        dest[0] = '\0';
        return 0;
    }
    
    // Guard clause: Single-byte buffer
    if (buffer_size == 1) {
        dest[0] = '\0';
        return 0;
    }
    
    // Limit n to buffer_size - 1
    const size_t max_copy = (n < buffer_size - 1) ? n : buffer_size - 1;
    
    // Copy characters
    size_t i = 0;
    while (i < max_copy && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    
    // Always null-terminate
    dest[i] = '\0';
    
    return i;
}

// ========================================
// FIXED STRING BUFFER
// ========================================

/**
 * @brief Fixed-size string buffer with safe operations
 *
 * Provides a stack-allocated string buffer with bounds-checked operations.
 * Eliminates heap allocation and prevents buffer overflows.
 *
 * @tparam N Maximum buffer size (including null terminator)
 *
 * USAGE:
 *   FixedString<64> str;
 *   str.set("Hello World");
 *   str.append("!");
 *   const char* cstr = str.c_str();
 *   size_t len = str.length();
 *
 * @note Zero heap allocation
 * @note Always null-terminated
 * @note All operations are noexcept
 */
template<size_t N>
class FixedString {
public:
    /// @brief Default constructor - initializes to empty string
    FixedString() noexcept {
        buffer_[0] = '\0';
        length_ = 0;
    }

    /**
     * @brief Constructor from C string
     * @param str Source string (may be nullptr)
     */
    explicit FixedString(const char* str) noexcept {
        set(str);
    }

    /**
     * @brief Constructor from string_view
     * @param sv String view to copy
     */
    explicit FixedString(std::string_view sv) noexcept {
        set(sv);
    }

    /**
     * @brief Set string from C string
     * @param str Source string (may be nullptr)
     * @return Number of characters copied
     */
    size_t set(const char* str) noexcept {
        length_ = safe_strcpy(buffer_, str, N);
        return length_;
    }

    /**
     * @brief Set string from string_view
     * @param sv String view to copy
     * @return Number of characters copied
     */
    size_t set(std::string_view sv) noexcept {
        // Guard clause: Check for empty view
        if (sv.empty()) {
            buffer_[0] = '\0';
            length_ = 0;
            return 0;
        }

        // Limit copy to buffer size - 1
        const size_t copy_len = (sv.size() < N - 1) ? sv.size() : N - 1;
        
        // Copy characters
        std::memcpy(buffer_, sv.data(), copy_len);
        buffer_[copy_len] = '\0';
        length_ = copy_len;
        
        return copy_len;
    }

    /**
     * @brief Append C string
     * @param str String to append (may be nullptr)
     * @return Number of characters appended
     */
    size_t append(const char* str) noexcept {
        size_t appended = safe_strcat(buffer_, str, N);
        length_ = safe_strlen(buffer_, N);
        return appended;
    }

    /**
     * @brief Append string_view
     * @param sv String view to append
     * @return Number of characters appended
     */
    size_t append(std::string_view sv) noexcept {
        // Guard clause: Check for empty view
        if (sv.empty()) {
            return 0;
        }

        // Find current length
        size_t current_len = safe_strlen(buffer_, N);
        
        // Guard clause: Buffer is already full
        if (current_len >= N - 1) {
            return 0;
        }

        // Calculate how many characters we can append
        const size_t space = N - 1 - current_len;
        const size_t append_len = (sv.size() < space) ? sv.size() : space;
        
        // Append characters
        std::memcpy(buffer_ + current_len, sv.data(), append_len);
        buffer_[current_len + append_len] = '\0';
        length_ = current_len + append_len;
        
        return append_len;
    }

    /**
     * @brief Clear string
     */
    void clear() noexcept {
        buffer_[0] = '\0';
        length_ = 0;
    }

    /**
     * @brief Get C string pointer
     * @return Pointer to null-terminated string
     */
    const char* c_str() const noexcept {
        return buffer_;
    }

    /**
     * @brief Get string length
     * @return Length of string (excluding null terminator)
     */
    size_t length() const noexcept {
        return length_;
    }

    /**
     * @brief Check if string is empty
     * @return true if string is empty
     */
    bool empty() const noexcept {
        return length_ == 0;
    }

    /**
     * @brief Get string_view
     * @return String view of the string
     */
    std::string_view view() const noexcept {
        return std::string_view{buffer_, length_};
    }

    /**
     * @brief Get maximum capacity
     * @return Maximum number of characters (excluding null terminator)
     */
    static constexpr size_t capacity() noexcept {
        return N - 1;
    }

    /**
     * @brief Check if string fits in buffer
     * @param str String to check
     * @return true if string fits
     */
    static constexpr bool fits(const char* str) noexcept {
        if (!str) return true;
        size_t len = 0;
        while (str[len] != '\0') {
            if (++len >= N - 1) return false;
        }
        return true;
    }

    /**
     * @brief Array subscript operator
     * @param index Character index
     * @return Character at index (or '\0' if out of bounds)
     */
    char operator[](size_t index) const noexcept {
        return (index < length_) ? buffer_[index] : '\0';
    }

private:
    char buffer_[N];
    size_t length_;
};

// ========================================
// COMMON BUFFER TYPE ALIASES
// ========================================

/// @brief 64-character string buffer (for descriptions)
using DescriptionBuffer = FixedString<64>;

/// @brief 32-character string buffer (for titles)
using TitleBuffer = FixedString<32>;

/// @brief 128-character string buffer (for error messages)
using ErrorMessageBuffer = FixedString<128>;

/// @brief 256-character string buffer (for file paths)
using PathBuffer = FixedString<256>;

/// @brief 16-character string buffer (for short names)
using ShortNameBuffer = FixedString<16>;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_SAFE_STRING_HPP_
