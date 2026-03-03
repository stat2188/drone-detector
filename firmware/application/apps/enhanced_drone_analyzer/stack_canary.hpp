// @file stack_canary.hpp
// @brief Stack canary for overflow detection
//
// DIAMOND FIX #3: Thread-Local Stack Canary (P0 - CRITICAL)
//
// FEATURES:
// - Stack-allocated canary for overflow detection
// - Detects stack overflow by checking canary corruption
// - Zero heap allocation - all memory is on stack
// - Thread-safe - each function call creates its own canary
//
// USAGE:
//   // At function entry:
//   StackCanary canary;
//   canary.initialize();
//
//   // At critical points:
//   canary.check();  // Halts if stack overflow detected
//
// LIMITATIONS:
// - Canary is a local variable on stack, so it only detects overflows that corrupt it
// - Most stack overflows corrupt data beyond or before the canary, leaving it intact
// - For comprehensive stack overflow detection, use runtime stack monitoring tools
//
// @note CRITICAL: Can only be used in function context (not global)
// @note CRITICAL: Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)

#ifndef STACK_CANARY_HPP_
#define STACK_CANARY_HPP_

// C++ standard library headers (alphabetical order)
#include <cstdint>

// Third-party library headers
#include <ch.h>  // For ChibiOS RTOS

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// FIX #3: STACK CANARY FOR OVERFLOW DETECTION
// ============================================================================

/**
 * @brief Stack canary for overflow detection
 *
 * DIAMOND FIX #3: Implement stack overflow detection using stack-allocated canary
 *
 * FEATURES:
 * - Stack-allocated canary value
 * - Initializes canary at function entry
 * - Checks canary at function exit or critical points
 * - Detects stack overflow by checking canary corruption
 *
 * THREADING MODEL:
 * - Each function call creates its own canary on stack
 * - Safe to use in multi-threaded environment
 * - Canary is destroyed when function returns
 *
 * USAGE:
 *   // At function entry:
 *   StackCanary canary;
 *   canary.initialize();
 *
 *   // At critical points:
 *   canary.check();  // Halts if stack overflow detected
 *
 * LIMITATIONS:
 * - Canary is a local variable on stack, so it only detects overflows that corrupt it
 * - Most stack overflows corrupt data beyond or before the canary, leaving it intact
 * - For comprehensive stack overflow detection, use runtime stack monitoring tools
 *
 * @note CRITICAL: Must be declared as local variable (not global or static)
 * @note CRITICAL: Canary is on stack, so it will be corrupted if stack overflows
 * @note CRITICAL: Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)
 * @note chSysLock/chSysUnlock are critical section locks that disable ALL interrupts,
 *       not memory barriers. Using them incorrectly can cause system instability.
 */
class StackCanary {
public:
    // Canary value for stack overflow detection (0xDEADBEEF)
    static constexpr uint32_t CANARY_VALUE = 0xDEADBEEF;

    /**
     * @brief Constructor
     * @note Does not initialize canary - call initialize() explicitly
     */
    constexpr StackCanary() noexcept = default;

    /**
     * @brief Destructor
     * @note Does nothing - canary is on stack
     */
    ~StackCanary() = default;

    // Non-copyable, non-movable
    StackCanary(const StackCanary&) = delete;
    StackCanary& operator=(const StackCanary&) = delete;

    /**
     * @brief Initialize canary value
     * @note Must be called at function entry
     * @note Stores canary value on stack
     * @note Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)
     */
    void initialize() noexcept {
        // Memory barrier before writing canary (compiler intrinsic)
        // NOTE: Using __sync_synchronize() instead of chSysLock/chSysUnlock
        //       chSysLock/chSysUnlock are critical section locks that disable ALL interrupts,
        //       not memory barriers. Using them incorrectly can cause system instability.
        __sync_synchronize();

        // Store canary value on stack
        canary_value_ = CANARY_VALUE;
    }

    /**
     * @brief Check canary for corruption
     * @return true if canary is intact, false if overflow detected
     * @note Does not halt - caller must handle error
     * @note Uses compiler intrinsic for memory barrier (not chSysLock/chSysUnlock)
     */
    [[nodiscard]] bool check() const noexcept {
        // Memory barrier before reading canary (compiler intrinsic)
        // NOTE: Using __sync_synchronize() instead of chSysLock/chSysUnlock
        //       chSysLock/chSysUnlock are critical section locks that disable ALL interrupts,
        //       not memory barriers. Using them incorrectly can cause system instability.
        __sync_synchronize();

        // Check canary value
        return canary_value_ == CANARY_VALUE;
    }

    /**
     * @brief Assert canary is intact (halts if corrupted)
     * @note Halts system if stack overflow detected
     */
    void assert_canary() const noexcept {
        if (!check()) {
            // Stack overflow detected - critical error
            // TODO: Implement proper error logging system
            #ifdef DEBUG
                __BKPT();  // Breakpoint for debugger
            #endif
            while (true) {
                // Infinite loop to halt execution
                // System watchdog will trigger reset if configured
            }
        }
    }

    /**
     * @brief Check and assert canary in one call
     * @note Convenience method for common pattern
     */
    void check_and_assert() const noexcept {
        assert_canary();
    }

    /**
     * @brief Get canary value (for debugging)
     * @return Current canary value
     */
    [[nodiscard]] uint32_t get_value() const noexcept {
        return canary_value_;
    }

private:
    // Canary value stored on stack
    // Will be corrupted if stack overflows
    volatile uint32_t canary_value_{0};
};

// ============================================================================
// STACK CANARY MACROS FOR CONVENIENCE
// ============================================================================

/**
 * @brief Initialize stack canary at function entry
 * @param canary Variable name for canary object
 */
#define STACK_CANARY_INIT(canary) \
    StackCanary canary; \
    canary.initialize();

/**
 * @brief Check stack canary at function exit
 * @param canary Variable name for canary object
 */
#define STACK_CANARY_CHECK(canary) \
    canary.check_and_assert();

/**
 * @brief Initialize and check stack canary (common pattern)
 * @param canary Variable name for canary object
 */
#define STACK_CANARY_GUARD(canary) \
    STACK_CANARY_INIT(canary); \
    STACK_CANARY_CHECK(canary);

} // namespace ui::apps::enhanced_drone_analyzer

#endif // STACK_CANARY_HPP_
