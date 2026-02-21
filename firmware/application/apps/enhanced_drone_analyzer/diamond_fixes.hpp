/**
 * @file diamond_fixes.hpp
 * @brief Critical fixes for Enhanced Drone Analyzer - Diamond Code Pipeline Results
 */

#ifndef DIAMOND_FIXES_HPP_
#define DIAMOND_FIXES_HPP_

#include <cstdint>
#include "eda_constants.hpp"
#include "eda_locking.hpp"
#include "eda_raii.hpp"  // For raii::SystemLock

namespace ui::apps::enhanced_drone_analyzer {
namespace DiamondFixes {

// Fix #1: Division by Zero Protection

/**
 * @brief Safe frequency hash calculation with zero check
 * @param frequency_hz Frequency in Hz (must be non-zero)
 * @return Frequency hash value (0-31)
 */
inline constexpr size_t safe_frequency_hash(EDA::Frequency frequency_hz) noexcept {
    // Guard clause: Prevent division by zero
    if (frequency_hz == 0) {
        return 0;
    }
    
    // Use named constant instead of magic number 100000
    return static_cast<size_t>(frequency_hz / EDA::Constants::FREQ_HASH_DIVISOR) 
           & EDA::Constants::FREQ_HASH_MASK;
}

// Fix #2: Magic Number Constants

namespace ConfidenceConstants {
    /// @brief Confidence percentage for RSSI-based detections
    constexpr uint8_t RSSI_CONFIDENCE = 85;
    
    /// @brief Confidence percentage for spectral analysis detections
    constexpr uint8_t SPECTRAL_CONFIDENCE = 90;
    
    /// @brief Maximum systime_t value (for overflow handling)
    constexpr uint32_t MAX_SYSTIME_VALUE = 0xFFFFFFFFUL;
}

namespace SpectrumConstants {
    /// @brief Spectrum width in pixels
    constexpr int SPEC_WIDTH = 240;
    
    /// @brief Spectrum height in pixels
    constexpr int SPEC_HEIGHT = 40;
}

// Fix #3: Type-Safe Frequency Comparison

/**
 * @brief Type-safe frequency comparison
 * @param freq1 First frequency (EDA::Frequency)
 * @param freq2 Second frequency (EDA::Frequency)
 * @return true if frequencies are equal, false otherwise
 */
inline constexpr bool frequency_equal(EDA::Frequency freq1, EDA::Frequency freq2) noexcept {
    return freq1 == freq2;
}

// Fix #4: Safe Deadline Calculation

/**
 * @brief Safe deadline calculation with overflow protection
 * @param current_time Current system time
 * @param timeout_ms Timeout in milliseconds
 * @return Deadline time (with overflow handling)
 */
inline constexpr systime_t safe_deadline(systime_t current_time, uint32_t timeout_ms) noexcept {
    uint32_t timeout_ticks = MS2ST(timeout_ms);
    
    // Check for overflow before addition
    if (current_time > ConfidenceConstants::MAX_SYSTIME_VALUE - timeout_ticks) {
        // Overflow would occur - return max value
        return ConfidenceConstants::MAX_SYSTIME_VALUE;
    }
    
    return current_time + timeout_ticks;
}

} // namespace DiamondFixes

} // namespace ui::apps::enhanced_drone_analyzer

#endif // DIAMOND_FIXES_HPP_
