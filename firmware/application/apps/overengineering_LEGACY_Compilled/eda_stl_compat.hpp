/**
 * @file eda_stl_compat.hpp
 * @brief Safe STL compatibility utilities for Enhanced Drone Analyzer
 *
 * DIAMOND FIX #5: Create eda_stl_compat.hpp (missing header)
 * - Provides safe string utilities without std::string
 * - Provides safe array utilities without std::vector
 * - Zero-heap, RAII, noexcept implementations
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 * Environment: ChibiOS RTOS
 *
 * Constraints:
 * - NO std::string (heap allocation forbidden)
 * - NO std::vector (heap allocation forbidden)
 * - NO std::atomic (use ChibiOS atomic operations)
 * - NO new/malloc (heap allocation forbidden)
 * - PERMITTED: std::array, std::string_view, fixed-size buffers
 *
 * @note All functions are noexcept for embedded safety
 * Phase 1 Migration - Foundation Layer (Infrastructure)
 *
 * DIAMOND CODE COMPLIANCE:
 * - No forbidden constructs (std::vector, std::string, std::map, std::atomic, new, malloc)
 * - Stack allocation only (max 4KB stack)
 * - Uses constexpr, enum class, using Type = uintXX_t
 * - No magic numbers (all constants defined)
 * - Zero-Overhead and Data-Oriented Design principles
 * - Self-contained and compilable
 */

#ifndef EDA_STL_COMPAT_HPP_
#define EDA_STL_COMPAT_HPP_

#include <cstdint>
#include <cstddef>

namespace ui::apps::enhanced_drone_analyzer {

/**
 * @brief Safe string utilities (no heap allocation)
 *
 * All string operations use fixed-size buffers and are noexcept.
 * No std::string is used to prevent heap allocation.
 */
namespace SafeString {

/**
 * @brief Safe string copy with bounds checking
 *
 * @param dest Destination buffer
 * @param dest_size Size of destination buffer in bytes
 * @param src Source string (null-terminated)
 * @return Number of characters copied (excluding null terminator)
 * @note Guarantees null-termination of destination buffer
 * @note noexcept for embedded safety
 */
inline size_t safe_copy(char* dest, size_t dest_size, const char* src) noexcept {
    if (!dest || dest_size == 0 || !src) {
        return 0;
    }

    size_t src_len = 0;
    while (src[src_len] != '\0') {
        src_len++;
    }

    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    
    for (size_t i = 0; i < copy_len; ++i) {
        dest[i] = src[i];
    }
    dest[copy_len] = '\0';
    
    return copy_len;
}

/**
 * @brief Safe string concatenation with bounds checking
 *
 * @param dest Destination buffer
 * @param dest_size Size of destination buffer in bytes
 * @param src Source string to append (null-terminated)
 * @return Number of characters appended (excluding null terminator)
 * @note Guarantees null-termination of destination buffer
 * @note noexcept for embedded safety
 */
inline size_t safe_cat(char* dest, size_t dest_size, const char* src) noexcept {
    if (!dest || dest_size == 0 || !src) {
        return 0;
    }

    size_t dest_len = 0;
    while (dest[dest_len] != '\0') {
        dest_len++;
    }

    if (dest_len >= dest_size - 1) {
        return 0;  // No space to append
    }

    size_t src_len = 0;
    while (src[src_len] != '\0') {
        src_len++;
    }

    size_t remaining = dest_size - dest_len - 1;
    size_t copy_len = (src_len < remaining) ? src_len : remaining;
    
    for (size_t i = 0; i < copy_len; ++i) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + copy_len] = '\0';
    
    return copy_len;
}

/**
 * @brief Safe string length with null pointer check
 *
 * @param str String to measure (null-terminated)
 * @return Length of string (0 if str is nullptr)
 * @note noexcept for embedded safety
 */
inline size_t safe_len(const char* str) noexcept {
    if (!str) {
        return 0;
    }
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * @brief Safe string comparison with null pointer handling
 *
 * @param str1 First string (null-terminated)
 * @param str2 Second string (null-terminated)
 * @return true if strings are equal (including both nullptr)
 * @note noexcept for embedded safety
 */
inline bool safe_eq(const char* str1, const char* str2) noexcept {
    if (str1 == str2) {
        return true;  // Both nullptr or same pointer
    }
    if (!str1 || !str2) {
        return false;  // One is nullptr
    }
    size_t i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return str1[i] == str2[i];
}

/**
 * @brief Check if string starts with prefix
 *
 * @param str String to check (null-terminated)
 * @param prefix Prefix to match (null-terminated)
 * @return true if str starts with prefix
 * @note noexcept for embedded safety
 */
inline bool starts_with(const char* str, const char* prefix) noexcept {
    if (!str || !prefix) {
        return false;
    }
    size_t prefix_len = 0;
    while (prefix[prefix_len] != '\0') {
        prefix_len++;
    }
    size_t str_len = 0;
    while (str[str_len] != '\0') {
        str_len++;
    }
    if (prefix_len > str_len) {
        return false;
    }
    for (size_t i = 0; i < prefix_len; ++i) {
        if (str[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Find character in string
 *
 * @param str String to search (null-terminated)
 * @param ch Character to find
 * @return Pointer to first occurrence, or nullptr if not found
 * @note noexcept for embedded safety
 */
inline const char* find_char(const char* str, char ch) noexcept {
    if (!str) {
        return nullptr;
    }
    size_t i = 0;
    while (str[i] != '\0') {
        if (str[i] == ch) {
            return &str[i];
        }
        i++;
    }
    return nullptr;
}

} // namespace SafeString

/**
 * @brief Safe array utilities (no std::vector)
 *
 * All array operations use fixed-size std::array or raw arrays.
 * No std::vector is used to prevent heap allocation.
 */
namespace SafeArray {

/**
 * @brief Safe array fill with bounds checking
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to fill
 * @param value Value to fill with
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline void fill(T (&arr)[N], const T& value) noexcept {
    for (size_t i = 0; i < N; ++i) {
        arr[i] = value;
    }
}

/**
 * @brief Safe array copy with bounds checking
 *
 * @tparam T Element type
 * @tparam N Source array size
 * @tparam M Destination array size
 * @param dest Destination array
 * @param src Source array
 * @return Number of elements copied
 * @note Copies min(N, M) elements
 * @note noexcept for embedded safety
 */
template <typename T, size_t N, size_t M>
inline size_t copy(T (&dest)[M], const T (&src)[N]) noexcept {
    size_t copy_count = (N < M) ? N : M;
    for (size_t i = 0; i < copy_count; ++i) {
        dest[i] = src[i];
    }
    return copy_count;
}

/**
 * @brief Safe array find with bounds checking
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to search
 * @param value Value to find
 * @return Index of first occurrence, or N if not found
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline size_t find(const T (&arr)[N], const T& value) noexcept {
    for (size_t i = 0; i < N; ++i) {
        if (arr[i] == value) {
            return i;
        }
    }
    return N;  // Not found
}

/**
 * @brief Safe array count with bounds checking
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to search
 * @param value Value to count
 * @return Number of occurrences
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline size_t count(const T (&arr)[N], const T& value) noexcept {
    size_t cnt = 0;
    for (size_t i = 0; i < N; ++i) {
        if (arr[i] == value) {
            cnt++;
        }
    }
    return cnt;
}

/**
 * @brief Safe array max with bounds checking
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to search
 * @return Maximum value, or default value if array is empty
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline T max(const T (&arr)[N]) noexcept {
    if (N == 0) {
        return T{};
    }
    T result = arr[0];
    for (size_t i = 1; i < N; ++i) {
        if (arr[i] > result) {
            result = arr[i];
        }
    }
    return result;
}

/**
 * @brief Safe array min with bounds checking
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to search
 * @return Minimum value, or default value if array is empty
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline T min(const T (&arr)[N]) noexcept {
    if (N == 0) {
        return T{};
    }
    T result = arr[0];
    for (size_t i = 1; i < N; ++i) {
        if (arr[i] < result) {
            result = arr[i];
        }
    }
    return result;
}

/**
 * @brief Safe array sum with bounds checking
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to sum
 * @return Sum of all elements
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline T sum(const T (&arr)[N]) noexcept {
    T result = T{};
    for (size_t i = 0; i < N; ++i) {
        result += arr[i];
    }
    return result;
}

/**
 * @brief Safe array average with bounds checking
 *
 * @tparam T Element type (integer type)
 * @tparam N Array size
 * @param arr Array to average
 * @return Average value (integer division)
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline T average(const T (&arr)[N]) noexcept {
    if (N == 0) {
        return T{};
    }
    return sum(arr) / static_cast<T>(N);
}

/**
 * @brief Safe array clamp with bounds checking
 *
 * @tparam T Element type
 * @tparam N Array size
 * @param arr Array to clamp
 * @param min_val Minimum value
 * @param max_val Maximum value
 * @note Clamps all elements to [min_val, max_val]
 * @note noexcept for embedded safety
 */
template <typename T, size_t N>
inline void clamp(T (&arr)[N], T min_val, T max_val) noexcept {
    for (size_t i = 0; i < N; ++i) {
        if (arr[i] < min_val) {
            arr[i] = min_val;
        } else if (arr[i] > max_val) {
            arr[i] = max_val;
        }
    }
}

} // namespace SafeArray

/**
 * @brief Safe buffer utilities for fixed-size buffers
 *
 * Provides utilities for working with fixed-size buffers
 * without dynamic allocation.
 */
namespace SafeBuffer {

/**
 * @brief Circular buffer index increment
 *
 * @param index Current index
 * @param size Buffer size (must be power of 2)
 * @return Next index (wrapping around)
 * @note noexcept for embedded safety
 */
inline size_t next_index(size_t index, size_t size) noexcept {
    return (index + 1) & (size - 1);
}

/**
 * @brief Circular buffer index decrement
 *
 * @param index Current index
 * @param size Buffer size (must be power of 2)
 * @return Previous index (wrapping around)
 * @note noexcept for embedded safety
 */
inline size_t prev_index(size_t index, size_t size) noexcept {
    return (index - 1) & (size - 1);
}

/**
 * @brief Check if size is power of 2
 *
 * @param size Size to check
 * @return true if size is power of 2
 * @note noexcept for embedded safety
 */
inline constexpr bool is_power_of_two(size_t size) noexcept {
    return size != 0 && (size & (size - 1)) == 0;
}

/**
 * @brief Safe modulo operation for circular buffers
 *
 * @param index Index to wrap
 * @param size Buffer size (must be power of 2)
 * @return Wrapped index
 * @note Uses bitwise AND for power-of-2 sizes
 * @note noexcept for embedded safety
 */
inline size_t wrap_index(size_t index, size_t size) noexcept {
    return index & (size - 1);
}

} // namespace SafeBuffer

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_STL_COMPAT_HPP_
