#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>
#include "../types/drone_types.hpp"

namespace drone_analyzer {

/**
 * @brief Error handling policy
 * 
 * FAIL_FAST: Return immediately on error, stop operation
 * GRACEFUL_DEGRADATION: Continue with fallback behavior
 * LOG_AND_CONTINUE: Log error and continue operation
 */
enum class ErrorPolicy : uint8_t {
    FAIL_FAST,
    GRACEFUL_DEGRADATION,
    LOG_AND_CONTINUE
};

/**
 * @brief Result type similar to std::optional<T> with error code
 * @note No heap allocation, uses static storage
 * @tparam T Type of value to store
 */
template<typename T>
class ErrorResult {
public:
    /**
     * @brief Default constructor - error state
     */
    ErrorResult() noexcept
        : has_value_(false), error_(ErrorCode::UNKNOWN_ERROR) {}
    
    /**
     * @brief Constructor with value - success state
     */
    explicit ErrorResult(const T& value) noexcept
        : has_value_(true), error_(ErrorCode::SUCCESS), value_(value) {}
    
    /**
     * @brief Constructor with error code - error state
     */
    explicit ErrorResult(ErrorCode error) noexcept
        : has_value_(false), error_(error) {}
    
    /**
     * @brief Check if result has a value
     */
    [[nodiscard]] bool has_value() const noexcept {
        return has_value_;
    }
    
    /**
     * @brief Check if result is valid (has value or error is SUCCESS)
     */
    [[nodiscard]] bool is_valid() const noexcept {
        return has_value_ || error_ == ErrorCode::SUCCESS;
    }
    
    /**
     * @brief Get error code
     */
    [[nodiscard]] ErrorCode error() const noexcept {
        return error_;
    }
    
    /**
     * @brief Get value (undefined if no value)
     * @pre has_value() must be true
     */
    [[nodiscard]] const T& value() const noexcept {
        return value_;
    }
    
    /**
     * @brief Get value or default if no value
     */
    [[nodiscard]] T value_or(const T& default_value) const noexcept {
        return has_value_ ? value_ : default_value;
    }
    
    /**
     * @brief Create success result
     */
    [[nodiscard]] static ErrorResult<T> success(const T& value) noexcept {
        return ErrorResult<T>(value);
    }
    
    /**
     * @brief Create error result
     */
    [[nodiscard]] static ErrorResult<T> failure(ErrorCode error) noexcept {
        return ErrorResult<T>(error);
    }
    
private:
    bool has_value_;
    ErrorCode error_;
    T value_;
};

/**
 * @brief Specialization for void operations
 */
template<>
class ErrorResult<void> {
public:
    /**
     * @brief Default constructor - success state
     */
    ErrorResult() noexcept
        : error_(ErrorCode::SUCCESS) {}
    
    /**
     * @brief Constructor with error code - error state
     */
    explicit ErrorResult(ErrorCode error) noexcept
        : error_(error) {}
    
    /**
     * @brief Check if result is valid (no error)
     */
    [[nodiscard]] bool is_valid() const noexcept {
        return error_ == ErrorCode::SUCCESS;
    }
    
    /**
     * @brief Get error code
     */
    [[nodiscard]] ErrorCode error() const noexcept {
        return error_;
    }
    
    /**
     * @brief Create success result
     */
    [[nodiscard]] static ErrorResult<void> success() noexcept {
        return ErrorResult<void>();
    }
    
    /**
     * @brief Create error result
     */
    [[nodiscard]] static ErrorResult<void> failure(ErrorCode error) noexcept {
        return ErrorResult<void>(error);
    }
    
private:
    ErrorCode error_;
};

/**
 * @brief Validate spectrum data buffer
 * @param spectrum_data Spectrum data buffer
 * @param length Buffer length
 * @return ErrorCode::SUCCESS if valid, error code otherwise
 */
[[nodiscard]] inline ErrorCode validate_spectrum_buffer(
    const uint8_t* spectrum_data,
    size_t length
) noexcept {
    if (spectrum_data == nullptr) {
        return ErrorCode::BUFFER_INVALID;
    }
    
    if (length == 0) {
        return ErrorCode::BUFFER_EMPTY;
    }
    
    // Max spectrum size will be defined in constants.hpp
    // For now, use a reasonable limit
    constexpr size_t MAX_SPECTRUM_SIZE = 256;
    if (length > MAX_SPECTRUM_SIZE) {
        return ErrorCode::BUFFER_INVALID;
    }
    
    return ErrorCode::SUCCESS;
}

/**
 * @brief Validate drone list buffer
 * @param drones Drone list buffer
 * @param count Number of drones
 * @param max_count Maximum allowed count
 * @return ErrorCode::SUCCESS if valid, error code otherwise
 */
[[nodiscard]] inline ErrorCode validate_drone_buffer(
    const DisplayDroneEntry* drones,
    size_t count,
    size_t max_count
) noexcept {
    if (drones == nullptr) {
        return ErrorCode::BUFFER_INVALID;
    }
    
    if (count == 0) {
        return ErrorCode::BUFFER_EMPTY;
    }
    
    if (count > max_count) {
        return ErrorCode::BUFFER_FULL;
    }
    
    return ErrorCode::SUCCESS;
}

/**
 * @brief Validate histogram buffer
 * @param histogram Histogram buffer
 * @param length Buffer length
 * @return ErrorCode::SUCCESS if valid, error code otherwise
 */
[[nodiscard]] inline ErrorCode validate_histogram_buffer(
    const uint16_t* histogram,
    size_t length
) noexcept {
    if (histogram == nullptr) {
        return ErrorCode::BUFFER_INVALID;
    }
    
    if (length == 0) {
        return ErrorCode::BUFFER_EMPTY;
    }
    
    constexpr size_t MAX_HISTOGRAM_SIZE = 128;
    if (length > MAX_HISTOGRAM_SIZE) {
        return ErrorCode::BUFFER_INVALID;
    }
    
    return ErrorCode::SUCCESS;
}

/**
 * @brief Handle hardware initialization failure
 * @param error Error code from hardware operation
 * @param max_retries Maximum retry attempts
 * @return ErrorCode::SUCCESS if recovered, error code otherwise
 */
template<typename HardwareFunc>
ErrorCode handle_hardware_failure(
    HardwareFunc hardware_func,
    ErrorCode initial_error,
    uint32_t max_retries = 3
) noexcept {
    if (initial_error == ErrorCode::SUCCESS) {
        return ErrorCode::SUCCESS;
    }
    
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        // Retry hardware operation
        ErrorCode result = hardware_func();
        if (result == ErrorCode::SUCCESS) {
            return ErrorCode::SUCCESS;
        }
        
        if (retry < max_retries - 1) {
            // Wait before retry
            // chThdSleepMilliseconds(10);  // ChibiOS call - will be available
        }
    }
    
    return initial_error;
}

/**
 * @brief Handle PLL lock failure with retry
 * @param tune_func Function to tune to frequency
 * @param lock_check_func Function to check PLL lock
 * @param target_freq Target frequency
 * @param max_retries Maximum retry attempts
 * @return ErrorCode::SUCCESS if locked, error code otherwise
 */
template<typename TuneFunc, typename LockCheckFunc>
ErrorCode handle_pll_lock_failure(
    TuneFunc tune_func,
    LockCheckFunc lock_check_func,
    FreqHz target_freq,
    uint32_t max_retries = 3
) noexcept {
    constexpr uint32_t PLL_LOCK_TIMEOUT_MS = 100;
    constexpr uint32_t PLL_LOCK_POLL_INTERVAL_MS = 3;
    
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        // Attempt to tune to frequency
        if (!tune_func(target_freq)) {
            if (retry < max_retries - 1) {
                // Retry after delay
                // chThdSleepMilliseconds(10);
                continue;
            }
            return ErrorCode::HARDWARE_FAILURE;
        }
        
        // Wait for PLL lock with timeout
        // SystemTime start_time = chTimeNow();
        // while (!lock_check_func()) {
        //     if (chTimeNow() - start_time > MS2ST(PLL_LOCK_TIMEOUT_MS)) {
        //         if (retry < max_retries - 1) {
        //             chThdSleepMilliseconds(10);
        //             break;
        //         }
        //         return ErrorCode::PLL_LOCK_FAILURE;
        //     }
        //     chThdSleepMilliseconds(PLL_LOCK_POLL_INTERVAL_MS);
        // }
        
        // PLL locked successfully
        return ErrorCode::SUCCESS;
    }
    
    return ErrorCode::PLL_LOCK_FAILURE;
}

/**
 * @brief Handle spectrum streaming failure with recovery
 * @param start_func Function to start streaming
 * @param shutdown_func Function to shutdown hardware
 * @param init_func Function to initialize hardware
 * @return ErrorCode::SUCCESS if started, error code otherwise
 */
template<typename StartFunc, typename ShutdownFunc, typename InitFunc>
ErrorCode handle_spectrum_streaming_failure(
    StartFunc start_func,
    ShutdownFunc shutdown_func,
    InitFunc init_func
) noexcept {
    // Attempt to start streaming
    if (!start_func()) {
        // Streaming failed - attempt recovery
        shutdown_func();
        // chThdSleepMilliseconds(100);
        init_func();
        
        // Retry streaming
        if (!start_func()) {
            return ErrorCode::HARDWARE_FAILURE;
        }
    }
    
    return ErrorCode::SUCCESS;
}

/**
 * @brief Handle mutex lock timeout gracefully
 * @param lock_result Result from lock attempt
 * @param operation Operation that failed
 * @return ErrorCode::SUCCESS if operation succeeded, error code otherwise
 */
inline ErrorCode handle_mutex_lock_timeout(
    bool lock_acquired,
    const char* operation
) noexcept {
    (void)operation;  // Suppress unused parameter warning
    
    if (!lock_acquired) {
        // Lock timeout - skip this update (graceful degradation)
        return ErrorCode::MUTEX_TIMEOUT;
    }
    
    return ErrorCode::SUCCESS;
}

/**
 * @brief Process error according to policy
 * @param error Error code to handle
 * @param policy Error handling policy
 * @return true if operation should continue, false otherwise
 */
inline bool handle_error(
    ErrorCode error,
    ErrorPolicy policy
) noexcept {
    switch (policy) {
        case ErrorPolicy::FAIL_FAST:
            // Stop operation on any error
            return (error == ErrorCode::SUCCESS);
        
        case ErrorPolicy::GRACEFUL_DEGRADATION:
            // Continue with fallback behavior
            return true;  // Always continue
        
        case ErrorPolicy::LOG_AND_CONTINUE:
            // Log error and continue
            // log_error(error);  // Will be implemented later
            return true;
    }
    
    return false;
}

/**
 * @brief Check if error is recoverable
 * @param error Error code to check
 * @return true if recoverable, false otherwise
 */
[[nodiscard]] inline bool is_recoverable_error(ErrorCode error) noexcept {
    switch (error) {
        case ErrorCode::BUFFER_EMPTY:
        case ErrorCode::MUTEX_TIMEOUT:
        case ErrorCode::DATABASE_LOAD_TIMEOUT:
        case ErrorCode::HARDWARE_TIMEOUT:
            return true;
        
        default:
            return false;
    }
}

/**
 * @brief Get fallback behavior for error
 * @param error Error code
 * @return Description of fallback behavior
 */
[[nodiscard]] inline const char* get_fallback_behavior(ErrorCode error) noexcept {
    switch (error) {
        case ErrorCode::BUFFER_EMPTY:
            return "Use empty state";
        
        case ErrorCode::MUTEX_TIMEOUT:
            return "Skip update, retry next cycle";
        
        case ErrorCode::DATABASE_LOAD_TIMEOUT:
            return "Use built-in defaults";
        
        case ErrorCode::HARDWARE_TIMEOUT:
            return "Retry operation";
        
        case ErrorCode::PLL_LOCK_FAILURE:
            return "Use last known frequency";
        
        default:
            return "No fallback available";
    }
}

} // namespace drone_analyzer

#endif // ERROR_HANDLER_HPP
