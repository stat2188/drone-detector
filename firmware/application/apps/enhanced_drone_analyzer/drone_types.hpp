#ifndef DRONE_TYPES_HPP
#define DRONE_TYPES_HPP

#include <cstdint>
#include <cstddef>

namespace drone_analyzer {

/**
 * @brief Type alias for frequency in Hz
 */
using FreqHz = uint64_t;

/**
 * @brief Type alias for signal strength (RSSI)
 */
using RssiValue = int32_t;

/**
 * @brief Type alias for system time (ChibiOS)
 */
using SystemTime = uint32_t;

/**
 * @brief Drone type classification
 */
enum class DroneType : uint8_t {
    UNKNOWN = 0,
    DJI = 1,
    PARROT = 2,
    YUNEEC = 3,
    DR_3DR = 4,
    AUTEL = 5,
    HOBBY = 6,
    FPV = 7,
    CUSTOM = 8,
    OTHER = 255
};

/**
 * @brief Threat level classification
 */
enum class ThreatLevel : uint8_t {
    NONE = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

/**
 * @brief Scanning mode (simplified - removed WIDEBAND, HYBRID, PANORAMIC)
 */
enum class ScanningMode : uint8_t {
    SINGLE = 0,
    HOPPING = 1,
    SEQUENTIAL = 2,
    TARGETED = 3
};

/**
 * @brief Movement trend for tracked drones
 */
enum class MovementTrend : uint8_t {
    UNKNOWN = 0,
    STATIC = 1,
    APPROACHING = 2,
    RECEDING = 3
};

/**
 * @brief Error codes for EDA operations
 * @note All errors are recoverable or have fallback behavior
 */
enum class ErrorCode : uint8_t {
    SUCCESS = 0,
    
    // Hardware errors (1-9)
    HARDWARE_NOT_INITIALIZED = 1,
    HARDWARE_TIMEOUT = 2,
    HARDWARE_FAILURE = 3,
    SPI_FAILURE = 4,
    PLL_LOCK_FAILURE = 5,
    
    // Database errors (10-19)
    DATABASE_NOT_LOADED = 10,
    DATABASE_LOAD_TIMEOUT = 11,
    DATABASE_CORRUPTED = 12,
    DATABASE_EMPTY = 13,
    
    // Buffer errors (20-29)
    BUFFER_EMPTY = 20,
    BUFFER_FULL = 21,
    BUFFER_INVALID = 22,
    
    // Synchronization errors (30-39)
    MUTEX_TIMEOUT = 30,
    MUTEX_LOCK_FAILED = 31,
    SEMAPHORE_TIMEOUT = 32,
    
    // Initialization errors (40-49)
    INITIALIZATION_FAILED = 40,
    INITIALIZATION_INCOMPLETE = 41,
    
    // General errors (50-59)
    INVALID_PARAMETER = 50,
    NOT_IMPLEMENTED = 51,
    UNKNOWN_ERROR = 255
};

/**
 * @brief Result type similar to std::optional<T> with error code
 * @note No heap allocation, uses static storage
 * @tparam T Type of value to store
 */
template<typename T>
class ErrorResult {
public:
    ErrorResult() noexcept
        : has_value_(false), error_(ErrorCode::UNKNOWN_ERROR) {}

    explicit ErrorResult(const T& value) noexcept
        : has_value_(true), error_(ErrorCode::SUCCESS), value_(value) {}

    explicit ErrorResult(ErrorCode error) noexcept
        : has_value_(false), error_(error) {}

    [[nodiscard]] bool has_value() const noexcept {
        return has_value_;
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return has_value_ || error_ == ErrorCode::SUCCESS;
    }

    [[nodiscard]] ErrorCode error() const noexcept {
        return error_;
    }

    [[nodiscard]] const T& value() const noexcept {
        return value_;
    }

    [[nodiscard]] T value_or(const T& default_value) const noexcept {
        return has_value_ ? value_ : default_value;
    }

    [[nodiscard]] static ErrorResult<T> success(const T& value) noexcept {
        return ErrorResult<T>(value);
    }

    [[nodiscard]] static ErrorResult<T> failure(ErrorCode error) noexcept {
        return ErrorResult<T>(error);
    }

private:
    bool has_value_;
    ErrorCode error_;
    T value_;
};

template<>
class ErrorResult<void> {
public:
    ErrorResult() noexcept
        : error_(ErrorCode::SUCCESS) {}

    explicit ErrorResult(ErrorCode error) noexcept
        : error_(error) {}

    [[nodiscard]] bool is_valid() const noexcept {
        return error_ == ErrorCode::SUCCESS;
    }

    [[nodiscard]] ErrorCode error() const noexcept {
        return error_;
    }

    [[nodiscard]] static ErrorResult<void> success() noexcept {
        return ErrorResult<void>();
    }

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
    if (length > 256) {
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
    if (length > 128) {
        return ErrorCode::BUFFER_INVALID;
    }
    return ErrorCode::SUCCESS;
}

/**
 * @brief Convert error code to human-readable string
 * @param error Error code to convert
 * @return Static string (Flash storage)
 */
[[nodiscard]] inline const char* error_to_string(ErrorCode error) noexcept {
    switch (error) {
        case ErrorCode::SUCCESS:
            return "Success";
        case ErrorCode::HARDWARE_NOT_INITIALIZED:
            return "Hardware not initialized";
        case ErrorCode::HARDWARE_TIMEOUT:
            return "Hardware timeout";
        case ErrorCode::HARDWARE_FAILURE:
            return "Hardware failure";
        case ErrorCode::SPI_FAILURE:
            return "SPI failure";
        case ErrorCode::PLL_LOCK_FAILURE:
            return "PLL lock failure";
        case ErrorCode::DATABASE_NOT_LOADED:
            return "Database not loaded";
        case ErrorCode::DATABASE_LOAD_TIMEOUT:
            return "Database load timeout";
        case ErrorCode::DATABASE_CORRUPTED:
            return "Database corrupted";
        case ErrorCode::DATABASE_EMPTY:
            return "Database empty";
        case ErrorCode::BUFFER_EMPTY:
            return "Buffer empty";
        case ErrorCode::BUFFER_FULL:
            return "Buffer full";
        case ErrorCode::BUFFER_INVALID:
            return "Buffer invalid";
        case ErrorCode::MUTEX_TIMEOUT:
            return "Mutex timeout";
        case ErrorCode::MUTEX_LOCK_FAILED:
            return "Mutex lock failed";
        case ErrorCode::SEMAPHORE_TIMEOUT:
            return "Semaphore timeout";
        case ErrorCode::INITIALIZATION_FAILED:
            return "Initialization failed";
        case ErrorCode::INITIALIZATION_INCOMPLETE:
            return "Initialization incomplete";
        case ErrorCode::INVALID_PARAMETER:
            return "Invalid parameter";
        case ErrorCode::NOT_IMPLEMENTED:
            return "Not implemented";
        case ErrorCode::UNKNOWN_ERROR:
        default:
            return "Unknown error";
    }
}

/**
 * @brief Safe string copy with bounds checking
 * @param dest Destination buffer
 * @param dest_size Destination buffer size (must be >= 2)
 * @param src Source string
 * @return ErrorCode::SUCCESS if copied, error otherwise
 * @note Always null-terminates destination
 * @note No heap allocation, uses inline implementation
 */
[[nodiscard]] inline ErrorCode safe_str_copy(
    char* dest,
    size_t dest_size,
    const char* src
) noexcept {
    if (dest == nullptr || src == nullptr || dest_size < 2) {
        return ErrorCode::BUFFER_INVALID;
    }

    size_t i = 0;
    for (; i < dest_size - 1 && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return ErrorCode::SUCCESS;
}

/**
 * @brief Tracked drone data structure (41 bytes)
 * @note No virtual functions, no vtable
 * @note Memory layout optimized for cache efficiency
 */
struct TrackedDrone {
    FreqHz frequency;                    // 8 bytes (uint64_t)
    DroneType drone_type;              // 1 byte (uint8_t)
    ThreatLevel threat_level;           // 1 byte (uint8_t)
    uint8_t update_count;              // 1 byte
    SystemTime last_seen;              // 4 bytes (uint32_t on ChibiOS)
    RssiValue rssi;                    // 4 bytes
    int16_t rssi_history_[6];          // 12 bytes (6 × int16_t)
    SystemTime timestamp_history_[6];   // 24 bytes (6 × uint32_t)
    uint8_t history_index_;            // 1 byte (was size_t, optimized to uint8_t)
    
    // Total: 56 bytes (no vtable, no virtual functions)
    
    /**
     * @brief Default constructor
     */
    TrackedDrone() noexcept;
    
    /**
     * @brief Constructor with initial values
     */
    TrackedDrone(
        FreqHz freq,
        DroneType type,
        ThreatLevel threat
    ) noexcept;
    
    /**
     * @brief Update drone with new RSSI reading
     */
    void update_rssi(RssiValue new_rssi, SystemTime timestamp) noexcept;
    
    /**
     * @brief Get average RSSI from history
     */
    [[nodiscard]] RssiValue get_average_rssi() const noexcept;
    
    /**
     * @brief Get movement trend based on RSSI history
     */
    [[nodiscard]] MovementTrend get_movement_trend() const noexcept;

    /**
     * @brief Get current threat level
     */
    [[nodiscard]] ThreatLevel get_threat() const noexcept {
        return threat_level;
    }

    /**
     * @brief Check if drone is stale (not seen recently)
     */
    [[nodiscard]] bool is_stale(SystemTime current_time, SystemTime timeout_ms) const noexcept;
};

/**
 * @brief Display drone entry for UI (39 bytes)
 * @note POD type, no vtable
 */
struct DisplayDroneEntry {
    FreqHz frequency;           // 8 bytes
    DroneType type;            // 1 byte (uint8_t)
    ThreatLevel threat;         // 1 byte (uint8_t)
    RssiValue rssi;             // 4 bytes
    SystemTime last_seen;       // 4 bytes
    char type_name[16];         // 16 bytes
    uint32_t display_color;     // 4 bytes (RGBA)
    MovementTrend trend;        // 1 byte (uint8_t)
    
    // Total: 39 bytes (no vtable, POD type)
    
    /**
     * @brief Default constructor
     */
    DisplayDroneEntry() noexcept;
    
    /**
     * @brief Constructor from TrackedDrone
     */
    explicit DisplayDroneEntry(const TrackedDrone& drone) noexcept;
    
    /**
     * @brief Set display color based on threat level
     */
    void set_color_from_threat() noexcept;
    
    /**
     * @brief Get type name string
     */
    [[nodiscard]] const char* get_type_name() const noexcept;
};

/**
 * @brief Phase completion tracking (6 bytes)
 */
struct PhaseCompletion {
    uint8_t buffers_allocated : 1;   // 1 bit
    uint8_t database_loaded : 1;    // 1 bit
    uint8_t hardware_ready : 1;     // 1 bit
    uint8_t ui_layout_ready : 1;    // 1 bit
    uint8_t settings_loaded : 1;    // 1 bit
    uint8_t finalized : 1;          // 1 bit
    uint8_t reserved : 2;           // 2 bits
    
    // Total: 1 byte (bitfield)
    
    /**
     * @brief Default constructor
     */
    PhaseCompletion() noexcept;
    
    /**
     * @brief Check if all phases complete
     */
    [[nodiscard]] bool is_complete() const noexcept;
    
    /**
     * @brief Reset all phases
     */
    void reset() noexcept;
};

/**
 * @brief Frequency hop detector (32 bytes)
 */
struct FrequencyHopDetector {
    FreqHz base_frequency;       // 8 bytes
    FreqHz last_frequency;        // 8 bytes
    uint8_t hop_count;            // 1 byte
    uint8_t hop_threshold;        // 1 byte
    SystemTime last_hop_time;     // 4 bytes
    uint32_t hop_interval_ms;     // 4 bytes
    uint8_t reserved[6];          // 6 bytes (padding for alignment)
    
    // Total: 32 bytes
    
    /**
     * @brief Default constructor
     */
    FrequencyHopDetector() noexcept;
    
    /**
     * @brief Check if frequency hop detected
     */
    [[nodiscard]] bool detect_hop(FreqHz current_freq, SystemTime current_time) noexcept;
    
    /**
     * @brief Reset detector
     */
    void reset() noexcept;
};

/**
 * @brief Wideband scan data (200 bytes)
 */
struct WidebandScanData {
    uint8_t spectrum_buffer[128];    // 128 bytes
    uint8_t noise_floor;             // 1 byte
    uint8_t signal_threshold;        // 1 byte
    uint16_t peak_index;             // 2 bytes
    uint32_t peak_frequency;         // 4 bytes
    uint16_t peak_amplitude;         // 2 bytes
    SystemTime scan_time;             // 4 bytes
    uint8_t scan_complete;           // 1 byte
    uint8_t reserved[57];            // 57 bytes (padding)
    
    // Total: 200 bytes
    
    /**
     * @brief Default constructor
     */
    WidebandScanData() noexcept;
    
    /**
     * @brief Clear scan data
     */
    void clear() noexcept;
    
    /**
     * @brief Check if scan is complete
     */
    [[nodiscard]] bool is_complete() const noexcept;
};

/**
 * @brief Scanner state snapshot
 */
struct ScannerStateSnapshot {
    ThreatLevel max_detected_threat;
    size_t approaching_count;
    size_t static_count;
    size_t receding_count;
    bool scanning_active;
    bool is_fresh;
    
    /**
     * @brief Default constructor
     */
    ScannerStateSnapshot() noexcept;
};

/**
 * @brief Display data for UI updates
 */
struct DisplayData {
    DisplayDroneEntry drones[20];     // 20 × 39 = 780 bytes
    size_t drone_count;
    ScannerStateSnapshot state;
    
    /**
     * @brief Default constructor
     */
    DisplayData() noexcept;
    
/**
 * @brief Clear display data
 */
void clear() noexcept;
};

// ============================================================================
// FreqmanDB Embedded-Compatible Types
// ============================================================================

/**
 * @brief Forward declaration of freqman_type from FreqmanDB
 */
enum class freqman_type : uint8_t;

/**
 * @brief Embedded-compatible freqman entry (no std::string, no heap)
 * @note 80 bytes total, aligned for 8-byte access
 * @note Replaces std::string-based freqman_entry from Mayhem core
 */
struct freqman_entry_fixed {
    int64_t frequency_a{0};
    int64_t frequency_b{0};
    char description[32];  // Fixed-size array, no std::string
    freqman_type type{static_cast<freqman_type>(0)};
    uint8_t modulation{255};  // freqman_invalid_index
    uint8_t bandwidth{255};
    uint8_t step{255};
    uint8_t tone{255};
    
    // Total: 8 + 8 + 32 + 1 + 1 + 1 + 1 + 1 = 53 bytes (padded to 56)
    
    /**
     * @brief Default constructor
     */
    freqman_entry_fixed() noexcept;
    
    /**
     * @brief Constructor with values
     */
    freqman_entry_fixed(
        int64_t freq_a,
        int64_t freq_b,
        const char* desc,
        freqman_type t
    ) noexcept;
    
    /**
     * @brief Copy description safely
     * @param src Source string
     * @return ErrorCode::SUCCESS if copied, error otherwise
     */
    [[nodiscard]] ErrorCode set_description(const char* src) noexcept;
    
    /**
     * @brief Check if entry is valid
     */
    [[nodiscard]] bool is_valid() const noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_TYPES_HPP
