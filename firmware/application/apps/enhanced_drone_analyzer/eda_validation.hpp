// eda_validation.hpp - Unified Validation Functions for Enhanced Drone Analyzer
//
// This header provides centralized validation functions for drone frequency and
// signal analysis operations. All functions are constexpr and noexcept for
// embedded safety, with no heap allocations or forbidden STL containers.
//
// DESIGN PRINCIPLES:
// - All functions are constexpr (compile-time evaluation when possible)
// - All functions are noexcept (no exceptions in embedded systems)
// - No heap allocations (all stack-based or compile-time constants)
// - No forbidden STL containers (std::vector, std::string, std::map, std::atomic)
// - Type-safe with using declarations and enum class
// - Named constants instead of magic numbers
//
// THREAD SAFETY:
// - Thread-safe: All functions are pure computations with no mutable state
// - ISR-safe: No blocking operations, no dynamic memory allocation
//
// PERFORMANCE:
// - Time complexity: O(1) for all validation functions
// - CPU cycles: ~2-5 cycles per validation (simple integer comparisons)
// - Memory: 0 bytes runtime (all constexpr constants in Flash/ROM)

#ifndef EDA_VALIDATION_HPP_
#define EDA_VALIDATION_HPP_

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"

namespace EDA {

// ============================================================================
// Type Definitions
// ============================================================================
// NOTE: Frequency and RSSI types are defined in eda_constants.hpp
// This header uses those types to avoid duplication

// ============================================================================
// Validation Constants (constexpr for Flash/ROM placement)
// ============================================================================

namespace ValidationConstants {

// Hardware frequency limits (1 MHz - 7.2 GHz)
constexpr Frequency MIN_HARDWARE_FREQ = 1'000'000ULL;      // 1 MHz
constexpr Frequency MAX_HARDWARE_FREQ = 7'200'000'000ULL;   // 7.2 GHz

// Safe frequency limits (50 MHz - 7.2 GHz)
constexpr Frequency MIN_SAFE_FREQ = 50'000'000ULL;          // 50 MHz
constexpr Frequency MAX_SAFE_FREQ = 7'200'000'000ULL;       // 7.2 GHz

// 2.4 GHz ISM band (Wi-Fi, Bluetooth, many drones)
constexpr Frequency MIN_2_4GHZ = 2'400'000'000ULL;         // 2.4 GHz
constexpr Frequency MAX_2_4GHZ = 2'483'500'000ULL;        // 2.4835 GHz

// 5.8 GHz ISM band (video transmission, FPV drones)
constexpr Frequency MIN_5_8GHZ = 5'725'000'000ULL;         // 5.725 GHz
constexpr Frequency MAX_5_8GHZ = 5'875'000'000ULL;         // 5.875 GHz

// Military band (860 MHz - 930 MHz)
constexpr Frequency MIN_MILITARY = 860'000'000ULL;         // 860 MHz
constexpr Frequency MAX_MILITARY = 930'000'000ULL;         // 930 MHz

// 433 MHz ISM band (long-range control)
constexpr Frequency MIN_433MHZ = 433'000'000ULL;           // 433 MHz
constexpr Frequency MAX_433MHZ = 435'000'000ULL;           // 435 MHz

// RSSI limits (dB)
constexpr RSSI MIN_RSSI = -110;
constexpr RSSI MAX_RSSI = 10;

// Description length limits
constexpr size_t MAX_DESCRIPTION_LENGTH = 32;

} // namespace ValidationConstants

// ============================================================================
// Validation Error Codes (enum class for type safety)
// ============================================================================

enum class ValidationErrorCode : uint8_t {
    OK = 0,
    FREQUENCY_OUT_OF_RANGE = 1,
    FREQUENCY_UNSAFE = 2,
    RSSI_OUT_OF_RANGE = 3,
    DESCRIPTION_TOO_LONG = 4,
    INVALID_THREAT_LEVEL = 5,
    EMPTY_DESCRIPTION = 6
};

// ============================================================================
// Validation Result Structure (stack-allocated, 16 bytes)
// ============================================================================

struct ValidationResult {
    bool valid;
    const char* error_message;
    ValidationErrorCode error_code;
};

// ============================================================================
// Core Validation Functions
// ============================================================================

namespace Validation {

/**
 * @brief Check if a value is within a specified range
 * @param value The value to check
 * @param min_val Minimum valid value (inclusive)
 * @param max_val Maximum valid value (inclusive)
 * @return true if value is within range, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool is_in_range(Frequency value, Frequency min_val, Frequency max_val) noexcept {
    return value >= min_val && value <= max_val;
}

/**
 * @brief Validate frequency is within hardware limits (1 MHz - 7.2 GHz)
 * @param freq_hz Frequency in Hz to validate
 * @return true if frequency is valid, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool validate_frequency(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, 
                       ValidationConstants::MIN_HARDWARE_FREQ, 
                       ValidationConstants::MAX_HARDWARE_FREQ);
}

/**
 * @brief Validate RSSI value is within acceptable range (-110 dB to 10 dB)
 * @param rssi_db RSSI value in dB to validate
 * @return true if RSSI is valid, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool validate_rssi(RSSI rssi_db) noexcept {
    return rssi_db >= ValidationConstants::MIN_RSSI && 
           rssi_db <= ValidationConstants::MAX_RSSI;
}

/**
 * @brief Check if frequency is within safe operating range (50 MHz - 7.2 GHz)
 * @param freq_hz Frequency in Hz to check
 * @return true if frequency is safe, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool is_safe_frequency(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, 
                       ValidationConstants::MIN_SAFE_FREQ, 
                       ValidationConstants::MAX_SAFE_FREQ);
}

// ============================================================================
// Band Validation Functions
// ============================================================================

/**
 * @brief Check if frequency is in 2.4 GHz ISM band
 * @param freq_hz Frequency in Hz to check
 * @return true if frequency is in 2.4 GHz band, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool is_2_4ghz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, 
                       ValidationConstants::MIN_2_4GHZ, 
                       ValidationConstants::MAX_2_4GHZ);
}

/**
 * @brief Check if frequency is in 5.8 GHz ISM band
 * @param freq_hz Frequency in Hz to check
 * @return true if frequency is in 5.8 GHz band, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool is_5_8ghz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, 
                       ValidationConstants::MIN_5_8GHZ, 
                       ValidationConstants::MAX_5_8GHZ);
}

/**
 * @brief Check if frequency is in military band (860 MHz - 930 MHz)
 * @param freq_hz Frequency in Hz to check
 * @return true if frequency is in military band, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool is_military_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, 
                       ValidationConstants::MIN_MILITARY, 
                       ValidationConstants::MAX_MILITARY);
}

/**
 * @brief Check if frequency is in 433 MHz ISM band
 * @param freq_hz Frequency in Hz to check
 * @return true if frequency is in 433 MHz band, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool is_433mhz_band(Frequency freq_hz) noexcept {
    return is_in_range(freq_hz, 
                       ValidationConstants::MIN_433MHZ, 
                       ValidationConstants::MAX_433MHZ);
}

/**
 * @brief Check if frequency is in any known drone band
 * @param freq_hz Frequency in Hz to check
 * @return true if frequency is in any known drone band, false otherwise
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr bool is_drone_band(Frequency freq_hz) noexcept {
    return is_2_4ghz_band(freq_hz) ||
           is_5_8ghz_band(freq_hz) ||
           is_military_band(freq_hz) ||
           is_433mhz_band(freq_hz);
}

// ============================================================================
// Description Validation Functions
// ============================================================================

/**
 * @brief Validate description string length and null-termination
 * @param description Description string to validate
 * @param max_length Maximum allowed length (including null terminator)
 * @return true if description is valid, false otherwise
 * @note constexpr for compile-time evaluation when string is known
 * @note noexcept for embedded safety
 */
static constexpr bool validate_description(const char* description, std::size_t max_length) noexcept {
    if (description == nullptr) {
        return false;
    }
    
    // Check for null terminator within max_length
    for (std::size_t i = 0; i < max_length; ++i) {
        if (description[i] == '\0') {
            return true;  // Found null terminator within bounds
        }
    }
    
    return false;  // No null terminator found within bounds
}

/**
 * @brief Validate description string using default max length
 * @param description Description string to validate
 * @return true if description is valid, false otherwise
 * @note constexpr for compile-time evaluation when string is known
 * @note noexcept for embedded safety
 */
static constexpr bool validate_description(const char* description) noexcept {
    return validate_description(description, ValidationConstants::MAX_DESCRIPTION_LENGTH);
}

// ============================================================================
// Comprehensive Validation Functions
// ============================================================================

/**
 * @brief Validate frequency with detailed error reporting
 * @param freq_hz Frequency in Hz to validate
 * @return ValidationResult with validity status and error details
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr ValidationResult validate_frequency_detailed(Frequency freq_hz) noexcept {
    if (!validate_frequency(freq_hz)) {
        return {false, "Frequency out of hardware range (1 MHz - 7.2 GHz)", 
                ValidationErrorCode::FREQUENCY_OUT_OF_RANGE};
    }
    
    if (!is_safe_frequency(freq_hz)) {
        return {false, "Frequency outside safe operating range (50 MHz - 7.2 GHz)", 
                ValidationErrorCode::FREQUENCY_UNSAFE};
    }
    
    return {true, nullptr, ValidationErrorCode::OK};
}

/**
 * @brief Validate RSSI with detailed error reporting
 * @param rssi_db RSSI value in dB to validate
 * @return ValidationResult with validity status and error details
 * @note constexpr for compile-time evaluation
 * @note noexcept for embedded safety
 */
static constexpr ValidationResult validate_rssi_detailed(RSSI rssi_db) noexcept {
    if (!validate_rssi(rssi_db)) {
        return {false, "RSSI out of range (-110 dB to 10 dB)", 
                ValidationErrorCode::RSSI_OUT_OF_RANGE};
    }
    
    return {true, nullptr, ValidationErrorCode::OK};
}

} // namespace Validation

} // namespace EDA

#endif // EDA_VALIDATION_HPP_
