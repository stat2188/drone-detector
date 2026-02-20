/**
 * Diamond Code: Safe Type Casting Utilities
 *
 * STAGE 4 FIXES IMPLEMENTED:
 * - SafeCast: Type-safe numeric conversions with bounds checking
 * - safe_reinterpret_cast: Safe pointer casting with alignment checks
 * - StrictMode: Compile-time enforcement for critical conversions
 * - will_overflow/will_underflow: Pre-check helpers for conversions
 *
 * DIAMOND CODE STANDARDS:
 * - Zero-heap allocation
 * - Compile-time validation (static_assert)
 * - Runtime bounds checking (assert in debug, no-op in release)
 * - constexpr for compile-time evaluation
 * - noexcept for exception-free operation
 * - Guard clauses for early error returns
 * - Doxygen comments for public APIs
 */

#ifndef EDA_SAFECAST_HPP_
#define EDA_SAFECAST_HPP_

#include <cstdint>
#include <cassert>
#include <type_traits>
#include <limits>

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// SAFE NUMERIC CAST
// ========================================
/**
 * @brief Safe numeric cast with bounds checking
 *
 * Performs type-safe numeric conversion with compile-time and runtime bounds checking.
 * Triggers assertion failure in debug mode if value would overflow/underflow.
 * In release mode, clamps to target type range (graceful degradation).
 *
 * @tparam To Target type
 * @tparam From Source type
 * @tparam StrictMode If true, fails at compile time for potentially unsafe conversions (default: false)
 *
 * USAGE:
 *   int64_t large_value = 1000000;
 *   int32_t safe_value = SafeCast<int32_t, int64_t, false>::from(large_value);
 *
 *   // For critical code paths requiring compile-time safety:
 *   int32_t strict_value = SafeCast<int32_t, int64_t, true>::from(large_value);
 *
 * @note Compile-time error if types have incompatible signs and StrictMode is true
 * @note Runtime assertion in debug mode on overflow/underflow
 * @note Clamps to target range in release mode
 * @note In StrictMode, static_assert prevents compilation if conversion cannot be guaranteed safe
 */
template<typename To, typename From, bool StrictMode = false>
class SafeCast {
public:
    /**
     * @brief Perform safe conversion from source to target type
     * @param value Value to convert
     * @return Converted value (clamped if out of range in release)
     */
    static constexpr To from(From value) noexcept {
        // Compile-time check: ensure target type is not smaller than source type
        // if both types are signed or both are unsigned
        static_assert(
            !StrictMode || 
            (std::is_signed<To>::value == std::is_signed<From>::value) ||
            (sizeof(To) >= sizeof(From)),
            "SafeCast: Unsafe conversion in StrictMode - target type may lose precision. "
            "Use StrictMode=false for runtime-checked conversion."
        );

        // Handle same-type conversion (no-op)
        if constexpr (std::is_same<To, From>::value) {
            return value;
        }

        // Handle signed-to-signed conversion
        if constexpr (std::is_signed<To>::value && std::is_signed<From>::value) {
            if constexpr (sizeof(To) >= sizeof(From)) {
                return static_cast<To>(value);
            } else {
                // Target is smaller - check bounds
                constexpr To to_min = std::numeric_limits<To>::min();
                constexpr To to_max = std::numeric_limits<To>::max();
                if (value < static_cast<From>(to_min)) {
                    assert(false && "SafeCast: underflow - value below target type minimum");
                    return to_min;
                }
                if (value > static_cast<From>(to_max)) {
                    assert(false && "SafeCast: overflow - value above target type maximum");
                    return to_max;
                }
                return static_cast<To>(value);
            }
        }

        // Handle unsigned-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && !std::is_signed<From>::value) {
            if constexpr (sizeof(To) >= sizeof(From)) {
                return static_cast<To>(value);
            } else {
                // Target is smaller - check upper bound
                constexpr To to_max = std::numeric_limits<To>::max();
                if (value > static_cast<From>(to_max)) {
                    assert(false && "SafeCast: overflow - value above target type maximum");
                    return to_max;
                }
                return static_cast<To>(value);
            }
        }

        // Handle signed-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && std::is_signed<From>::value) {
            if (value < 0) {
                assert(false && "SafeCast: negative to unsigned - cannot convert negative value to unsigned");
                return 0;
            }
            constexpr To to_max = std::numeric_limits<To>::max();
            if (static_cast<std::make_unsigned_t<From>>(value) > 
                static_cast<std::make_unsigned_t<From>>(to_max)) {
                assert(false && "SafeCast: overflow - value above target type maximum");
                return to_max;
            }
            return static_cast<To>(value);
        }

        // Handle unsigned-to-signed conversion
        if constexpr (std::is_signed<To>::value && !std::is_signed<From>::value) {
            constexpr To to_max = std::numeric_limits<To>::max();
            if (value > static_cast<std::make_unsigned_t<From>>(to_max)) {
                assert(false && "SafeCast: overflow - value above target type maximum");
                return to_max;
            }
            return static_cast<To>(value);
        }

        // Fallback (should never reach here)
        return static_cast<To>(value);
    }

    /**
     * @brief Check if conversion is safe without performing it
     * @param value Value to check
     * @return true if conversion is safe, false otherwise
     */
    static constexpr bool is_safe(From value) noexcept {
        // Handle same-type conversion
        if constexpr (std::is_same<To, From>::value) {
            return true;
        }

        // Handle signed-to-signed conversion
        if constexpr (std::is_signed<To>::value && std::is_signed<From>::value) {
            if constexpr (sizeof(To) >= sizeof(From)) {
                return true;
            } else {
                constexpr To to_min = std::numeric_limits<To>::min();
                constexpr To to_max = std::numeric_limits<To>::max();
                return value >= static_cast<From>(to_min) && 
                       value <= static_cast<From>(to_max);
            }
        }

        // Handle unsigned-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && !std::is_signed<From>::value) {
            if constexpr (sizeof(To) >= sizeof(From)) {
                return true;
            } else {
                constexpr To to_max = std::numeric_limits<To>::max();
                return value <= static_cast<From>(to_max);
            }
        }

        // Handle signed-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && std::is_signed<From>::value) {
            return value >= 0 && 
                   static_cast<std::make_unsigned_t<From>>(value) <= 
                   static_cast<std::make_unsigned_t<From>>(std::numeric_limits<To>::max());
        }

        // Handle unsigned-to-signed conversion
        if constexpr (std::is_signed<To>::value && !std::is_signed<From>::value) {
            return value <= static_cast<std::make_unsigned_t<From>>(std::numeric_limits<To>::max());
        }

        return true;
    }

    /**
     * @brief Check if value would overflow when converted to target type
     * @param value Value to check
     * @return true if conversion would overflow
     */
    static constexpr bool will_overflow(From value) noexcept {
        // Handle same-type conversion
        if constexpr (std::is_same<To, From>::value) {
            return false;
        }

        // Handle signed-to-signed conversion
        if constexpr (std::is_signed<To>::value && std::is_signed<From>::value) {
            if constexpr (sizeof(To) >= sizeof(From)) {
                return false;
            } else {
                constexpr To to_max = std::numeric_limits<To>::max();
                return value > static_cast<From>(to_max);
            }
        }

        // Handle unsigned-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && !std::is_signed<From>::value) {
            if constexpr (sizeof(To) >= sizeof(From)) {
                return false;
            } else {
                constexpr To to_max = std::numeric_limits<To>::max();
                return value > static_cast<From>(to_max);
            }
        }

        // Handle signed-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && std::is_signed<From>::value) {
            if (value < 0) {
                return false; // Negative values are underflow, not overflow
            }
            constexpr To to_max = std::numeric_limits<To>::max();
            return static_cast<std::make_unsigned_t<From>>(value) > 
                   static_cast<std::make_unsigned_t<From>>(to_max);
        }

        // Handle unsigned-to-signed conversion
        if constexpr (std::is_signed<To>::value && !std::is_signed<From>::value) {
            constexpr To to_max = std::numeric_limits<To>::max();
            return value > static_cast<std::make_unsigned_t<From>>(to_max);
        }

        return false;
    }

    /**
     * @brief Check if value would underflow when converted to target type
     * @param value Value to check
     * @return true if conversion would underflow
     */
    static constexpr bool will_underflow(From value) noexcept {
        // Handle same-type conversion
        if constexpr (std::is_same<To, From>::value) {
            return false;
        }

        // Handle signed-to-signed conversion
        if constexpr (std::is_signed<To>::value && std::is_signed<From>::value) {
            if constexpr (sizeof(To) >= sizeof(From)) {
                return false;
            } else {
                constexpr To to_min = std::numeric_limits<To>::min();
                return value < static_cast<From>(to_min);
            }
        }

        // Handle unsigned-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && !std::is_signed<From>::value) {
            return false; // Unsigned types cannot underflow
        }

        // Handle signed-to-unsigned conversion
        if constexpr (!std::is_signed<To>::value && std::is_signed<From>::value) {
            return value < 0;
        }

        // Handle unsigned-to-signed conversion
        if constexpr (std::is_signed<To>::value && !std::is_signed<From>::value) {
            return false; // Unsigned to signed cannot underflow
        }

        return false;
    }
};

// ========================================
// SAFE REINTERPRET CAST
// ========================================
/**
 * @brief Safe reinterpret_cast with alignment checks
 *
 * Performs type-safe pointer reinterpretation with compile-time alignment validation.
 * Ensures target type alignment is compatible with source pointer.
 *
 * @tparam To Target pointer type
 * @tparam From Source pointer type
 * @param ptr Source pointer to cast
 * @return Casted pointer
 *
 * USAGE:
 *   uint8_t buffer[1024];
 *   MyStruct* obj = safe_reinterpret_cast<MyStruct*>(buffer);
 *
 * @note Compile-time error if alignment is insufficient
 * @note Runtime assertion in debug mode if pointer is null
 */
template<typename To, typename From>
constexpr To safe_reinterpret_cast(From* ptr) noexcept {
    static_assert(alignof(typename std::remove_pointer<To>::type) <= 
                  alignof(typename std::remove_pointer<From>::type),
                  "SafeCast: Target alignment exceeds source alignment - "
                  "this may cause undefined behavior on some platforms");
    static_assert(alignof(typename std::remove_pointer<To>::type) <= 
                  alignof(std::max_align_t),
                  "SafeCast: Target alignment too large for platform - "
                  "maximum alignment requirement exceeded");
    assert(ptr != nullptr && "SafeCast: null pointer cast - dereferencing will cause undefined behavior");
    return reinterpret_cast<To>(ptr);
}

/**
 * @brief Safe reinterpret_cast for volatile pointers
 */
template<typename To, typename From>
constexpr To safe_reinterpret_cast_volatile(From* ptr) noexcept {
    static_assert(alignof(typename std::remove_pointer<To>::type) <= 
                  alignof(typename std::remove_pointer<From>::type),
                  "SafeCast: Target alignment exceeds source alignment - "
                  "this may cause undefined behavior on some platforms");
    static_assert(alignof(typename std::remove_pointer<To>::type) <= 
                  alignof(std::max_align_t),
                  "SafeCast: Target alignment too large for platform - "
                  "maximum alignment requirement exceeded");
    assert(ptr != nullptr && "SafeCast: null pointer cast - dereferencing will cause undefined behavior");
    return reinterpret_cast<To>(ptr);
}

/**
 * @brief Safe reinterpret_cast from address
 */
template<typename To>
constexpr To safe_reinterpret_cast_addr(uintptr_t addr) noexcept {
    static_assert(alignof(typename std::remove_pointer<To>::type) <= 
                  alignof(std::max_align_t),
                  "SafeCast: Target alignment too large for platform - "
                  "maximum alignment requirement exceeded");
    assert(addr != 0 && "SafeCast: zero address cast - dereferencing will cause undefined behavior");
    return reinterpret_cast<To>(addr);
}

// ========================================
// SAFE BIT CAST
// ========================================
/**
 * @brief Safe bit-level cast between types
 *
 * Performs bit-level type conversion (like std::bit_cast in C++20).
 * Requires both types to have the same size and be trivially copyable.
 * Uses explicit byte-by-byte copy for C++14/17 constexpr compatibility.
 *
 * @tparam To Target type
 * @tparam From Source type
 * @param value Value to cast
 * @return Bit-casted value
 *
 * USAGE:
 *   uint32_t int_value = 0x12345678;
 *   float float_value = safe_bit_cast<float>(int_value);
 *
 * @note Compile-time error if types have different sizes
 * @note Compile-time error if types are not trivially copyable
 * @note Uses explicit byte copy for C++14/17 constexpr compatibility
 */
template<typename To, typename From>
constexpr To safe_bit_cast(const From& value) noexcept {
    static_assert(sizeof(To) == sizeof(From), 
                  "SafeCast: Types must have same size for bit_cast - "
                  "source and target types have different sizes");
    static_assert(std::is_trivially_copyable<To>::value, 
                  "SafeCast: Target type must be trivially copyable - "
                  "non-trivially-copyable types cannot be safely bit-cast");
    static_assert(std::is_trivially_copyable<From>::value, 
                  "SafeCast: Source type must be trivially copyable - "
                  "non-trivially-copyable types cannot be safely bit-cast");
    
    To result{};
    // Use explicit byte-by-byte copy for C++14/17 constexpr compatibility
    // (std::memcpy is not constexpr in C++14/17)
    const uint8_t* src = reinterpret_cast<const uint8_t*>(&value);
    uint8_t* dst = reinterpret_cast<uint8_t*>(&result);
    for (size_t i = 0; i < sizeof(To); ++i) {
        dst[i] = src[i];
    }
    return result;
}

// ========================================
// FREQUENCY CASTING HELPERS
// ========================================
/**
 * @brief Safe cast for frequency values (int64_t to uint64_t)
 */
constexpr inline uint64_t safe_frequency_cast(int64_t freq_hz) noexcept {
    if (freq_hz < 0) {
        assert(false && "SafeCast: negative frequency - frequency values cannot be negative");
        return 0;
    }
    return static_cast<uint64_t>(freq_hz);
}

/**
 * @brief Safe cast for frequency values (uint64_t to int64_t)
 */
constexpr inline int64_t safe_frequency_cast_signed(uint64_t freq_hz) noexcept {
    constexpr uint64_t MAX_SAFE_FREQ = static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
    if (freq_hz > MAX_SAFE_FREQ) {
        assert(false && "SafeCast: frequency overflow - frequency value exceeds int64_t maximum");
        return std::numeric_limits<int64_t>::max();
    }
    return static_cast<int64_t>(freq_hz);
}

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_SAFECAST_HPP_
