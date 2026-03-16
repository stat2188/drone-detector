#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include <cstring>
#include <new>
#include "drone_types.hpp"
#include "error_handler.hpp"
#include "locking.hpp"
#include "constants.hpp"

// Forward declarations for ChibiOS types
struct mutex_t;

namespace drone_analyzer {

/**
 * @brief Frequency database entry (16 bytes)
 * @note POD type, no vtable
 */
struct FrequencyEntry {
    FreqHz frequency;           // 8 bytes
    DroneType drone_type;       // 1 byte
    uint8_t priority;           // 1 byte
    uint16_t reserved;          // 2 bytes (padding)
    uint32_t flags;             // 4 bytes
    
    // Total: 16 bytes
    
    /**
     * @brief Default constructor
     */
    FrequencyEntry() noexcept;
    
    /**
     * @brief Constructor with values
     */
    FrequencyEntry(FreqHz freq, DroneType type, uint8_t prio) noexcept;
    
    /**
     * @brief Check if entry is valid
     */
    [[nodiscard]] bool is_valid() const noexcept;
};

/**
 * @brief Database manager for frequency database
 * @note Lazy initialization - loads on first access
 * @note No observer pattern (simplified from legacy)
 * @note Thread-safe with mutex protection
 */
class DatabaseManager {
public:
    /**
     * @brief Constructor
     */
    DatabaseManager() noexcept;
    
    /**
     * @brief Destructor
     */
    ~DatabaseManager() noexcept;
    
    // Delete copy and move operations
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) = delete;
    DatabaseManager& operator=(DatabaseManager&&) = delete;
    
    /**
     * @brief Load frequency database (lazy initialization)
     * @note Only loads on first call, subsequent calls return cached result
     * @note Thread-safe with mutex protection
     * @return ErrorCode::SUCCESS if loaded, error code otherwise
     */
    [[nodiscard]] ErrorCode load_frequency_database() noexcept;
    
    /**
     * @brief Get next frequency from database
     * @param current_freq Current frequency (0 for first)
     * @return ErrorResult containing next frequency or error
     */
    [[nodiscard]] ErrorResult<FreqHz> get_next_frequency(FreqHz current_freq) noexcept;
    
    /**
     * @brief Reset database iterator
     * @note Does not reload database, just resets index
     */
    void reset_database() noexcept;
    
    /**
     * @brief Get database size
     * @return Number of entries in database
     */
    [[nodiscard]] size_t get_database_size() const noexcept;
    
    /**
     * @brief Check if database is loaded
     * @return true if loaded, false otherwise
     */
    [[nodiscard]] bool is_loaded() const noexcept;
    
    /**
     * @brief Get frequency entry by index
     * @param index Entry index
     * @return ErrorResult containing entry or error
     */
    [[nodiscard]] ErrorResult<FrequencyEntry> get_entry(size_t index) const noexcept;
    
    /**
     * @brief Find entry by frequency
     * @param frequency Frequency to find
     * @return ErrorResult containing entry or error
     */
    [[nodiscard]] ErrorResult<FrequencyEntry> find_entry(FreqHz frequency) const noexcept;
    
    /**
     * @brief Add entry to database
     * @param entry Entry to add
     * @return ErrorCode::SUCCESS if added, error code otherwise
     */
    [[nodiscard]] ErrorCode add_entry(const FrequencyEntry& entry) noexcept;

    /**
     * @brief Remove entry from database
     * @param frequency Frequency to remove
     * @return ErrorCode::SUCCESS if removed, error code otherwise
     */
    [[nodiscard]] ErrorCode remove_entry(FreqHz frequency) noexcept;

    // ========================================================================
    // FreqmanDB Integration Methods (no std::vector, no std::string)
    // ========================================================================

    /**
     * @brief Load frequencies from Freqman file
     * @param filename Filename (without extension, e.g., "DRONES")
     * @return ErrorCode::SUCCESS if loaded, error code otherwise
     * @note Uses freqman_entry_fixed (embedded-compatible, no heap)
     */
    [[nodiscard]] ErrorCode load_from_freqman_file(const char* filename) noexcept;

    /**
     * @brief Get freqman entry by index
     * @param index Entry index
     * @return Pointer to entry or nullptr if out of range
     */
    [[nodiscard]] const freqman_entry_fixed* get_freqman_entry(size_t index) const noexcept;

    /**
     * @brief Get total number of freqman entries
     * @return Number of entries
     */
    [[nodiscard]] size_t get_freqman_entry_count() const noexcept;
    
    /**
     * @brief Clear all entries
     */
    void clear_entries() noexcept;
    
    /**
     * @brief Get current iterator position
     * @return Current index or error
     */
    [[nodiscard]] ErrorResult<size_t> get_current_index() const noexcept;
    
    /**
     * @brief Set iterator position
     * @param index New index
     * @return ErrorCode::SUCCESS if set, error code otherwise
     */
    [[nodiscard]] ErrorCode set_current_index(size_t index) noexcept;
    
private:
    /**
     * @brief Internal: Load database from file
     * @note Called by load_frequency_database() with mutex held
     * @return ErrorCode::SUCCESS if loaded, error code otherwise
     */
    [[nodiscard]] ErrorCode load_from_file_internal() noexcept;

    // ========================================================================
    // FreqmanDB Storage (embedded-compatible, aligned, no heap)
    // ========================================================================

    /**
     * @brief Freqman entries storage (aligned array, no heap allocation)
     * @note Uses freqman_entry_fixed (no std::string)
     * @note Properly aligned for ARM Cortex-M4 (8-byte alignment)
     */
    alignas(alignof(freqman_entry_fixed))
    std::array<freqman_entry_fixed, MAX_DATABASE_ENTRIES> freqman_storage_;

    /**
     * @brief Number of loaded freqman entries
     */
    size_t freqman_entry_count_{0};

    /**
     * @brief Freqman database loaded flag
     */
    bool freqman_loaded_{false};
    
    /**
     * @brief Internal: Parse line from database file
     * @param line Line to parse
     * @param entry Output entry
     * @return ErrorCode::SUCCESS if parsed, error code otherwise
     */
    [[nodiscard]] ErrorCode parse_line_internal(
        const char* line,
        FrequencyEntry& entry
    ) const noexcept;
    
    /**
     * @brief Internal: Validate frequency entry
     * @param entry Entry to validate
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_entry_internal(
        const FrequencyEntry& entry
    ) const noexcept;
    
    /**
     * @brief Internal: Find entry index by frequency
     * @param frequency Frequency to find
     * @return ErrorResult containing index or error
     */
    [[nodiscard]] ErrorResult<size_t> find_entry_index_internal(
        FreqHz frequency
    ) const noexcept;

    /**
      * @brief Internal: Load drone legend from DRONES.TXT
      * @note Populates entries_ with drone types and threat levels
      */
    ErrorCode load_drone_legend_internal() noexcept;

    // ========================================================================
    // FreqmanDB Internal Methods
    // ========================================================================

    /**
     * @brief Parse drone type from freqman description
     * @param description Description string from freqman entry
     * @return Drone type (UNKNOWN if not recognized)
     * @note Supports: DJI, FPV, PARROT, YUNEEC (max 4 chars)
     */
    [[nodiscard]] DroneType parse_drone_type_from_description(
        const char* description
    ) const noexcept;
    
    // Database storage (fixed-size array, no heap allocation)
    std::array<FrequencyEntry, MAX_DATABASE_ENTRIES> entries_;
    
    // Current iterator position
    size_t current_index_;
    
    // Number of valid entries
    size_t entry_count_;
    
    // Database loaded flag (AtomicFlag for lock-free access)
    AtomicFlag loaded_;
    
    // Mutex for thread safety (LockOrder::DATABASE_MUTEX)
    mutable mutex_t mutex_;
    
    // Line buffer for parsing (mutable for use in const methods)
    mutable char line_buffer_[DATABASE_LINE_BUFFER_SIZE];
};

} // namespace drone_analyzer

#endif // DATABASE_HPP
