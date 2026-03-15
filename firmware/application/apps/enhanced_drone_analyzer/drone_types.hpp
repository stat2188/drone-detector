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
    3DR = 4,
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
 * @brief Convert error code to human-readable string
 * @param error Error code to convert
 * @return Static string (Flash storage)
 */
[[nodiscard]] const char* error_to_string(ErrorCode error) noexcept;

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
    int16_t rssi_history_[3];          // 6 bytes (3 × int16_t)
    SystemTime timestamp_history_[3];   // 12 bytes (3 × uint32_t)
    size_t history_index_;             // 4 bytes
    
    // Total: 41 bytes (no vtable, no virtual functions)
    
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

} // namespace drone_analyzer

#endif // DRONE_TYPES_HPP
