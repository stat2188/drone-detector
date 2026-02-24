// Unified Database Interface for Enhanced Drone Analyzer
// Stage 4: Single Source of Truth for Drone Frequency Database

#ifndef EDA_UNIFIED_DATABASE_HPP_
#define EDA_UNIFIED_DATABASE_HPP_

#include <cstdint>
#include <cstddef>
#include <array>
#include <cstring>
#include <ch.h>
#include <chtypes.h>
#include "eda_constants.hpp"
#include "eda_locking.hpp"
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// ============================================================================
// Unified Drone Entry Structure (48 bytes)
// ============================================================================

struct UnifiedDroneEntry {
    Frequency frequency_hz = 0;       // 8 bytes
    char description[32] = "";        // 32 bytes  
    uint8_t threat_level = 0;         // 1 byte (ThreatLevel enum)
    uint8_t frequency_band = 0;       // 1 byte (frequency band identifier)
    uint16_t flags = 0;               // 2 bytes (reserved for future use)
    uint8_t reserved[4] = {};         // 4 bytes (padding/reserved)
    
    // Total: 48 bytes
    
    // [[nodiscard]] - Validation result must be used by caller
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return frequency_hz > 0;
    }
    
    // [[nodiscard]] - Description check result must be used by caller
    [[nodiscard]] constexpr bool has_description() const noexcept {
        return description[0] != '\0';
    }
    
    constexpr void clear() noexcept {
        frequency_hz = 0;
        description[0] = '\0';
        threat_level = 0;
        frequency_band = 0;
        flags = 0;
        // reserved array is zero-initialized
    }
};

// Compile-time size verification
static_assert(sizeof(UnifiedDroneEntry) == 48, "UnifiedDroneEntry must be exactly 48 bytes");

// ============================================================================
// Database Configuration Constants
// ============================================================================

namespace DatabaseConfig {
    // Unified limit based on memory analysis (5,760 bytes total)
    constexpr size_t MAX_ENTRIES = 120;
    
    // Entry and database sizes
    constexpr size_t ENTRY_SIZE = sizeof(UnifiedDroneEntry);
    constexpr size_t DATABASE_SIZE = MAX_ENTRIES * ENTRY_SIZE;
    
    // Maximum number of observers (fixed-size array, no heap)
    constexpr size_t MAX_OBSERVERS = 4;
    
    // Maximum description length (matches freqman format)
    constexpr size_t MAX_DESCRIPTION_LENGTH = 32;
    
    // Validation thresholds
    constexpr Frequency MIN_FREQ = EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ;
    constexpr Frequency MAX_FREQ = EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;
}

// ============================================================================
// Database Change Event Types
// ============================================================================

enum class DatabaseEventType : uint8_t {
    ENTRY_ADDED = 0,
    ENTRY_MODIFIED = 1,
    ENTRY_DELETED = 2,
    DATABASE_CLEARED = 3,
    DATABASE_RELOADED = 4
};

// Database change event for observer notifications
struct DatabaseChangeEvent {
    DatabaseEventType type;
    size_t index;                       // Affected entry index
    size_t count;                       // Number of affected entries
    const UnifiedDroneEntry* entry;     // Pointer to entry (nullptr for bulk ops)
};

// Observer callback type - function pointer (zero heap)
using DatabaseObserverCallback = void(*)(const DatabaseChangeEvent& event, void* user_data);

// ============================================================================
// Database Statistics
// ============================================================================

struct DatabaseStats {
    size_t entry_count;
    size_t capacity;
    uint32_t load_count;
    uint32_t save_count;
    uint32_t change_count;
    uint32_t validation_errors;
};

// ============================================================================
// Non-Owning Database View (Zero-Copy Iteration)
// ============================================================================

class DatabaseView {
public:
    constexpr DatabaseView(const UnifiedDroneEntry* entries, size_t count) noexcept
        : entries_(entries), count_(count) {}
    
    // [[nodiscard]] - Entry access must be used
    [[nodiscard]] constexpr const UnifiedDroneEntry& operator[](size_t index) const noexcept {
        return entries_[index];
    }
    
    // [[nodiscard]] - Size must be used
    [[nodiscard]] constexpr size_t size() const noexcept { return count_; }
    // [[nodiscard]] - Empty check must be used
    [[nodiscard]] constexpr bool empty() const noexcept { return count_ == 0; }
    
    // Iterator support for range-based for
    const UnifiedDroneEntry* begin() const noexcept { return entries_; }
    const UnifiedDroneEntry* end() const noexcept { return entries_ + count_; }
    
private:
    const UnifiedDroneEntry* entries_;
    size_t count_;
};

// ============================================================================
// Frequency Validation Extension
// ============================================================================

namespace FrequencyValidation {

// Validation result structure
struct ValidationResult {
    bool valid;
    const char* error_message;
    uint8_t error_code;
};

// Validation error codes
enum class ValidationErrorCode : uint8_t {
    OK = 0,
    FREQUENCY_OUT_OF_RANGE = 1,
    FREQUENCY_UNSAFE = 2,
    DESCRIPTION_TOO_LONG = 3,
    INVALID_DRONE_TYPE = 4,
    INVALID_THREAT_LEVEL = 5,
    DUPLICATE_FREQUENCY = 6
};

// Validate complete entry
// [[nodiscard]] - Validation result must be used by caller
[[nodiscard]] inline ValidationResult validate_entry(const UnifiedDroneEntry& entry) noexcept {
    // Check frequency range
    if (!is_valid(entry.frequency_hz)) {
        return {false, "Frequency out of hardware range", 
                static_cast<uint8_t>(ValidationErrorCode::FREQUENCY_OUT_OF_RANGE)};
    }
    
    // Check frequency safety
    if (!is_safe(entry.frequency_hz)) {
        return {false, "Frequency outside safe operating range",
                static_cast<uint8_t>(ValidationErrorCode::FREQUENCY_UNSAFE)};
    }
    
    // Check description length (must be null-terminated within buffer)
    if (entry.description[31] != '\0') {
        // Check if description fills entire buffer without null terminator
        bool has_null = false;
        for (size_t i = 0; i < 32; ++i) {
            if (entry.description[i] == '\0') {
                has_null = true;
                break;
            }
        }
        if (!has_null) {
            return {false, "Description too long (max 31 chars)",
                    static_cast<uint8_t>(ValidationErrorCode::DESCRIPTION_TOO_LONG)};
        }
    }
    
    // Check threat level (max valid is UNKNOWN = 5)
    if (entry.threat_level > static_cast<uint8_t>(ThreatLevel::UNKNOWN)) {
        return {false, "Invalid threat level",
                static_cast<uint8_t>(ValidationErrorCode::INVALID_THREAT_LEVEL)};
    }
    
    return {true, nullptr, static_cast<uint8_t>(ValidationErrorCode::OK)};
}

// Check for duplicate frequency in entry array
// [[nodiscard]] - Duplicate check result must be used by caller
[[nodiscard]] inline bool is_duplicate_frequency(Frequency freq, 
                                    const UnifiedDroneEntry* entries,
                                    size_t count,
                                    size_t exclude_index = SIZE_MAX) noexcept {
    if (entries == nullptr) return false;
    
    for (size_t i = 0; i < count; ++i) {
        if (i == exclude_index) continue;
        if (entries[i].frequency_hz == freq) return true;
    }
    return false;
}

} // namespace FrequencyValidation

// ============================================================================
// Main Unified Database Class (Singleton)
// ============================================================================

class UnifiedDroneDatabase {
public:
    // Singleton access (static storage, no heap)
    static UnifiedDroneDatabase& instance() noexcept {
        static UnifiedDroneDatabase instance;
        return instance;
    }
    
    // Disable copy/move
    UnifiedDroneDatabase(const UnifiedDroneDatabase&) = delete;
    UnifiedDroneDatabase& operator=(const UnifiedDroneDatabase&) = delete;
    
    // === Lifecycle ===
    
    // Initialize database (call once at app startup)
    // [[nodiscard]] - Initialization success must be checked
    [[nodiscard]] bool initialize() noexcept;
    
    // Load from file (replaces current content)
    // [[nodiscard]] - Load success must be checked
    [[nodiscard]] bool load(const char* path = "/FREQMAN/DRONES.TXT") noexcept;
    
    // Save to file
    // [[nodiscard]] - Save success must be checked
    [[nodiscard]] bool save(const char* path = "/FREQMAN/DRONES.TXT") noexcept;
    
    // Clear all entries
    void clear() noexcept;
    
    // === Entry Access ===
    
    // Get read-only view (thread-safe snapshot)
    // [[nodiscard]] - View must be used
    [[nodiscard]] DatabaseView get_view() const noexcept;
    
    // Get entry count
    // [[nodiscard]] - Size must be used
    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] size_t capacity() const noexcept { return DatabaseConfig::MAX_ENTRIES; }
    // [[nodiscard]] - Empty check must be used
    [[nodiscard]] bool empty() const noexcept;
    
    // Get single entry (bounds-checked)
    // [[nodiscard]] - Entry pointer must be used
    [[nodiscard]] const UnifiedDroneEntry* get_entry(size_t index) const noexcept;
    
    // === Modification (triggers observer notifications) ===
    
    // Add entry (returns index or -1 if full)
    // [[nodiscard]] - Index must be used
    [[nodiscard]] int add_entry(const UnifiedDroneEntry& entry) noexcept;
    
    // Update entry at index
    // [[nodiscard]] - Success must be checked
    [[nodiscard]] bool update_entry(size_t index, const UnifiedDroneEntry& entry) noexcept;
    
    // Delete entry at index
    // [[nodiscard]] - Success must be checked
    [[nodiscard]] bool delete_entry(size_t index) noexcept;
    
    // === Observer Pattern ===
    
    // Register observer (returns false if max observers reached)
    // [[nodiscard]] - Registration success must be checked
    [[nodiscard]] bool add_observer(DatabaseObserverCallback callback, void* user_data) noexcept;
    
    // Remove observer
    void remove_observer(DatabaseObserverCallback callback) noexcept;
    
    // === Validation ===
    
    // Validate entry before adding
    // [[nodiscard]] - Validation result must be used
    [[nodiscard]] bool validate_entry(const UnifiedDroneEntry& entry) const noexcept;
    
    // Get statistics
    // [[nodiscard]] - Stats must be used
    [[nodiscard]] DatabaseStats get_stats() const noexcept;
    
    // === Thread Safety ===
    
    // Acquire mutex for bulk operations
    void lock() noexcept;
    void unlock() noexcept;
    
private:
    UnifiedDroneDatabase() noexcept;
    ~UnifiedDroneDatabase() noexcept = default;
    
    // Notify all observers
    void notify_observers(const DatabaseChangeEvent& event) noexcept;
    
    // Observer entry structure
    struct ObserverEntry {
        DatabaseObserverCallback callback = nullptr;
        void* user_data = nullptr;
        bool active = false;
    };
    
    // Storage (static allocation)
    std::array<UnifiedDroneEntry, DatabaseConfig::MAX_ENTRIES> entries_{};
    
    // State
    size_t entry_count_ = 0;
    bool initialized_ = false;
    
    // Thread synchronization
    mutable Mutex mutex_{};
    
    // Observers (fixed-size array, no heap)
    std::array<ObserverEntry, DatabaseConfig::MAX_OBSERVERS> observers_{};
    
    // Statistics
    mutable DatabaseStats stats_{};
};

// ============================================================================
// Compile-Time Size Checks
// ============================================================================

static_assert(DatabaseConfig::DATABASE_SIZE <= 8192, "Database storage exceeds 8KB limit");

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_UNIFIED_DATABASE_HPP_
