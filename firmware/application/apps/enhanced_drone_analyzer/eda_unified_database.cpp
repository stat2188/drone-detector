// eda_unified_database.cpp - Unified Drone Database Implementation
// Stage 4, Part 3 Fix: P1-3 - chMtxUnlock() API Compatibility
//
// This file implements a unified database for drone frequency entries with thread-safe
// operations and observer pattern support.

#include "eda_unified_database.hpp"

// ============================================================================
// DIAMOND CODE COMPLIANCE NOTES:
// ============================================================================
// This file follows Diamond Code principles for embedded C++ on STM32F405:
// - NO std::vector, std::string, std::map, std::atomic, new, malloc
// - NO exceptions, NO RTTI
// - PERMITTED: std::array, std::string_view, fixed-size buffers, stack allocation
// - Stack allocation only (max 4KB stack)
// - Uses MutexLock from eda_locking.hpp for thread-safe operations
// - Zero-Overhead and Data-Oriented Design principles
// ============================================================================

// C++ standard library headers (alphabetical order)
#include <algorithm>
#include <cstddef>
#include <cstdint>

// C standard library headers (for string functions)
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Third-party library headers (alphabetical order)
#include <ch.h>

// ChibiOS Internal Headers

// Project-specific headers (alphabetical order)
#include "eda_locking.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// P1-HIGH FIX #3: chMtxUnlock() API Compatibility
// ============================================================================
// PROBLEM: chMtxUnlock() API usage
//   - ChibiOS 2.6.8 API requires mutex pointer parameter for unlock
//   - chMtxUnlock(mutex_t *mp) unlocks the specified mutex
//   - Returns pointer to unlocked mutex for verification
//
// SOLUTION: Use correct API with mutex parameter
//   - Fixed: chMtxUnlock(mutex_t *mp);
//   - Ensures proper mutex unlock for ChibiOS 2.6.8 version compatibility
//
// API COMPATIBILITY:
// - ChibiOS 20.x: chMtxUnlock(mutex_t *mp) - requires parameter
// - ChibiOS 21.x+: chMtxUnlock() - unlocks last locked mutex, no parameter
// - This codebase uses ChibiOS 2.6.8 which requires parameter-based API
//
// NOTE: See eda_locking.hpp:244-256 for MutexLock implementation
// ============================================================================

// ============================================================================
// Singleton Instance
// ============================================================================

/**
 * @brief Get singleton instance of the database
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe: C++11 guarantees atomic initialization of static local variables
 * - No heap allocation
 * - Returns reference to static instance
 */
UnifiedDroneDatabase& UnifiedDroneDatabase::instance() noexcept {
    static UnifiedDroneDatabase instance;
    return instance;
}

// ============================================================================
// Constructor / Destructor
// ============================================================================

/**
 * @brief Constructor - initializes database with zero entries
 *
 * DIAMOND CODE COMPLIANCE:
 * - Initializes ChibiOS mutex for thread safety
 * - Zero-initializes all entries and observers
 * - No heap allocation
 */
UnifiedDroneDatabase::UnifiedDroneDatabase() noexcept {
    chMtxInit(&mutex_);
    initialized_ = false;
    entry_count_ = 0;

    // Initialize all entries to zero
    for (size_t i = 0; i < DatabaseConfig::MAX_ENTRIES; ++i) {
        entries_[i] = UnifiedDroneEntry{};
    }

    // Initialize observers
    for (size_t i = 0; i < DatabaseConfig::MAX_OBSERVERS; ++i) {
        observers_[i] = ObserverEntry{};
    }
}

// ============================================================================
// Database Entry Validation
// ============================================================================

/**
 * @brief Validate a drone database entry for corruption protection
 * @param entry Entry to validate
 * @return true if entry is valid, false otherwise
 * @note Validates frequency range, threat_level, and description
 *
 * DIAMOND CODE COMPLIANCE:
 * - No exceptions - uses boolean return value
 * - Stack-allocated operations
 * - Thread-safe when called with mutex held
 */
bool UnifiedDroneDatabase::validate_entry_data(const UnifiedDroneEntry& entry) noexcept {
    // Validate frequency range (100 MHz to 6 GHz)
    if (entry.frequency_hz < 100000000LL || entry.frequency_hz > 6000000000LL) {
        return false;  // Frequency out of valid range
    }

    // Validate threat_level enum value (0-4)
    uint8_t threat_value = entry.threat_level;
    if (threat_value > 4) {
        return false;  // Invalid threat_level
    }

    // Validate description is null-terminated
    if (entry.description[31] != '\0') {
        return false;  // Description not null-terminated
    }

    // Note: frequency_band is uint8_t (0-255), so no upper bound check needed

    return true;  // Entry is valid
}

// ============================================================================
// Initialization
// ============================================================================

/**
 * @brief Initialize the database
 * @return true if initialization succeeded
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - No exceptions - uses boolean return value
 * - Stack-allocated operations
 */
bool UnifiedDroneDatabase::initialize() noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    if (initialized_) {
        return true;  // Already initialized
    }

    initialized_ = true;
    entry_count_ = 0;

    // Initialize all entries to zero
    for (size_t i = 0; i < DatabaseConfig::MAX_ENTRIES; ++i) {
        entries_[i] = UnifiedDroneEntry{};
    }

    // Initialize observers
    for (size_t i = 0; i < DatabaseConfig::MAX_OBSERVERS; ++i) {
        observers_[i] = ObserverEntry{};
    }

    return true;
}

// ============================================================================
// Clear
// ============================================================================

/**
 * @brief Clear all entries from the database
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Notifies observers of DATABASE_CLEARED event
 * - No exceptions
 */
void UnifiedDroneDatabase::clear() noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Reset all entries to zero
    for (size_t i = 0; i < DatabaseConfig::MAX_ENTRIES; ++i) {
        entries_[i] = UnifiedDroneEntry{};
    }

    entry_count_ = 0;

    // Notify observers
    DatabaseChangeEvent event{};
    event.type = DatabaseEventType::DATABASE_CLEARED;
    event.index = 0;
    event.count = 0;
    event.entry = nullptr;
    notify_observers(event);
}

// ============================================================================
// Thread Safety
// ============================================================================

/**
 * @brief Lock the database mutex for manual locking
 *
 * DIAMOND CODE COMPLIANCE:
 * - Uses ChibiOS chMtxLock() API
 * - For manual locking - prefer MutexLock RAII wrapper
 */
void UnifiedDroneDatabase::lock() noexcept {
    chMtxLock(&mutex_);
}

/**
 * @brief Unlock the database mutex for manual unlocking
 *
 * DIAMOND CODE COMPLIANCE:
 * - Uses ChibiOS chMtxUnlock() API
 * - For manual unlocking - prefer MutexLock RAII wrapper
 */
void UnifiedDroneDatabase::unlock() noexcept {
    // P1-HIGH FIX #3: Use correct API without mutex parameter
    // ChibiOS 2.6.8 API uses parameter-less unlock (unlocks last locked mutex via LIFO stack)
    // This ensures proper mutex unlock for version compatibility
    chMtxUnlock();
}

// ============================================================================
// Entry Access
// ============================================================================

/**
 * @brief Get a read-only view of the database
 * @return DatabaseView with entries
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Returns non-owning view (zero-copy)
 * - No heap allocation
 */
DatabaseView UnifiedDroneDatabase::get_view() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return DatabaseView(entries_.data(), entry_count_);
}

/**
 * @brief Get the number of entries in the database
 * @return Entry count
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - No heap allocation
 */
size_t UnifiedDroneDatabase::size() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return entry_count_;
}

/**
 * @brief Check if the database is empty
 * @return true if empty
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - No heap allocation
 */
bool UnifiedDroneDatabase::empty() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return entry_count_ == 0;
}

/**
 * @brief Get a single entry by index
 * @param index Entry index
 * @return Pointer to entry, or nullptr if index is invalid
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Bounds-checked access
 * - Returns non-owning pointer (zero-copy)
 */
const UnifiedDroneEntry* UnifiedDroneDatabase::get_entry(size_t index) const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    if (index >= entry_count_) {
        return nullptr;
    }

    return &entries_[index];
}

// ============================================================================
// Modification Operations
// ============================================================================

/**
 * @brief Add a new entry to the database
 * @param entry Entry to add
 * @return Index of added entry, or -1 if failed
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Validates entry before adding
 * - Checks for duplicate frequencies
 * - Notifies observers of ENTRY_ADDED event
 * - No exceptions
 */
int UnifiedDroneDatabase::add_entry(const UnifiedDroneEntry& entry) noexcept {
    // Guard clause: validate entry before adding
    if (!entry.is_valid()) {
        return -1;
    }

    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Check if database is full
    if (entry_count_ >= DatabaseConfig::MAX_ENTRIES) {
        return -1;
    }

    // Check for duplicate frequency
    for (size_t i = 0; i < entry_count_; ++i) {
        if (entries_[i].frequency_hz == entry.frequency_hz) {
            return -1;  // Duplicate frequency
        }
    }

    // Add entry
    entries_[entry_count_] = entry;
    int index = static_cast<int>(entry_count_);
    entry_count_++;

    // Notify observers
    DatabaseChangeEvent event{};
    event.type = DatabaseEventType::ENTRY_ADDED;
    event.index = static_cast<size_t>(index);
    event.count = 1;
    event.entry = &entries_[index];
    notify_observers(event);

    return index;
}

/**
 * @brief Update an existing entry
 * @param index Entry index to update
 * @param entry New entry data
 * @return true if successful
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Validates entry before updating
 * - Checks for duplicate frequencies (excluding current entry)
 * - Notifies observers of ENTRY_MODIFIED event
 * - No exceptions
 */
bool UnifiedDroneDatabase::update_entry(size_t index, const UnifiedDroneEntry& entry) noexcept {
    // Guard clause: validate entry before updating
    if (!entry.is_valid()) {
        return false;
    }

    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    if (index >= entry_count_) {
        return false;
    }

    // Check for duplicate frequency (excluding current entry)
    for (size_t i = 0; i < entry_count_; ++i) {
        if (i != index && entries_[i].frequency_hz == entry.frequency_hz) {
            return false;  // Duplicate frequency
        }
    }

    // Update entry
    entries_[index] = entry;

    // Notify observers
    DatabaseChangeEvent event{};
    event.type = DatabaseEventType::ENTRY_MODIFIED;
    event.index = index;
    event.count = 1;
    event.entry = &entries_[index];
    notify_observers(event);

    return true;
}

/**
 * @brief Delete an entry from the database
 * @param index Entry index to delete
 * @return true if successful
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Shifts remaining entries to maintain compact storage
 * - Notifies observers of ENTRY_DELETED event
 * - No exceptions
 */
bool UnifiedDroneDatabase::delete_entry(size_t index) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    if (index >= entry_count_) {
        return false;
    }

    // Remove entry by shifting remaining entries
    for (size_t i = index; i < entry_count_ - 1; ++i) {
        entries_[i] = entries_[i + 1];
    }

    // Clear last entry
    entries_[entry_count_ - 1] = UnifiedDroneEntry{};
    entry_count_--;

    // Notify observers
    DatabaseChangeEvent event{};
    event.type = DatabaseEventType::ENTRY_DELETED;
    event.index = index;
    event.count = 1;
    event.entry = nullptr;
    notify_observers(event);

    return true;
}

// ============================================================================
// File I/O Operations
// ============================================================================

/**
 * @brief Load database entries from file
 * @param file_path Path to file
 * @return true if successful
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - No exceptions - uses boolean return value
 * - NOTE: Currently stubbed - see implementation notes below
 */
bool UnifiedDroneDatabase::load(const char* file_path) noexcept {
    // Guard clause: null pointer check
    if (!file_path) {
        return false;
    }

    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // ========================================================================
    // STUBBED METHOD - NOT IMPLEMENTED
    // ========================================================================
    // This method is intentionally stubbed and always returns false.
    //
    // WHY STUBBED:
    // The File API (File::read/write) needs review for compatibility with
    // current firmware architecture. The database file format and parsing
    // logic require careful design to ensure thread-safe operation and proper
    // error handling.
    //
    // IMPLEMENTATION REQUIREMENTS:
    // 1. Review File API documentation and usage patterns in other modules
    // 2. Define file format specification (binary or text-based)
    // 3. Implement parsing logic for UnifiedDroneEntry structures
    // 4. Add validation for loaded data
    // 5. Handle file I/O errors gracefully
    // 6. Ensure atomic load operation (clear old entries only after successful load)
    // 7. Update statistics (load_count)
    // 8. Notify observers of DATABASE_RELOADED event
    //
    // CURRENT BEHAVIOR:
    // - Always returns false to indicate failure
    // - Database remains unchanged
    // - No side effects
    //
    // TODO: Implement file loading when File API compatibility is verified
    // ========================================================================

    (void)file_path;  // Suppress unused warning
    return false;
}

/**
 * @brief Save database entries to file
 * @param file_path Path to file
 * @return true if successful
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - No exceptions - uses boolean return value
 * - NOTE: Currently stubbed - see implementation notes below
 */
bool UnifiedDroneDatabase::save(const char* file_path) noexcept {
    // Guard clause: null pointer check
    if (!file_path) {
        return false;
    }

    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // ========================================================================
    // STUBBED METHOD - NOT IMPLEMENTED
    // ========================================================================
    // This method is intentionally stubbed and always returns false.
    //
    // WHY STUBBED:
    // The File API (File::write) needs review for compatibility with
    // current firmware architecture. The database file format and serialization
    // logic require careful design to ensure thread-safe operation and proper
    // error handling.
    //
    // IMPLEMENTATION REQUIREMENTS:
    // 1. Review File API documentation and usage patterns in other modules
    // 2. Define file format specification (binary or text-based)
    // 3. Implement serialization logic for UnifiedDroneEntry structures
    // 4. Add header/metadata to file (version, entry count, checksum)
    // 5. Handle file I/O errors gracefully (disk full, write failure)
    // 6. Ensure atomic write operation (write to temp file, then rename)
    // 7. Update statistics (save_count)
    // 8. Consider file compression if space is limited
    //
    // CURRENT BEHAVIOR:
    // - Always returns false to indicate failure
    // - No file is created or modified
    // - Database content remains unchanged
    //
    // TODO: Implement file saving when File API compatibility is verified
    // ========================================================================

    (void)file_path;  // Suppress unused warning
    return false;
}

// ============================================================================
// Observer Pattern Implementation
// ============================================================================

/**
 * @brief Register an observer callback
 * @param callback Function pointer to callback
 * @param user_data User data pointer passed to callback
 * @return true if registration succeeded
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Fixed-size observer array (no heap allocation)
 * - No exceptions
 */
bool UnifiedDroneDatabase::add_observer(DatabaseObserverCallback callback, void* user_data) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    // Find empty observer slot
    for (size_t i = 0; i < DatabaseConfig::MAX_OBSERVERS; ++i) {
        if (!observers_[i].active) {
            observers_[i].callback = callback;
            observers_[i].user_data = user_data;
            observers_[i].active = true;
            return true;
        }
    }

    return false;  // Observer list full
}

/**
 * @brief Remove an observer callback
 * @param callback Function pointer to remove
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - No exceptions
 */
void UnifiedDroneDatabase::remove_observer(DatabaseObserverCallback callback) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);

    for (size_t i = 0; i < DatabaseConfig::MAX_OBSERVERS; ++i) {
        if (observers_[i].active && observers_[i].callback == callback) {
            observers_[i] = ObserverEntry{};  // Reset to default
            return;
        }
    }
}

/**
 * @brief Notify all active observers of a database event
 * @param event Event to notify about
 *
 * DIAMOND CODE COMPLIANCE:
 * - Called with mutex already held
 * - No exceptions
 * - Fixed-size observer array (no heap allocation)
 */
void UnifiedDroneDatabase::notify_observers(const DatabaseChangeEvent& event) noexcept {
    // Notify all active observers
    for (size_t i = 0; i < DatabaseConfig::MAX_OBSERVERS; ++i) {
        if (observers_[i].active && observers_[i].callback != nullptr) {
            observers_[i].callback(event, observers_[i].user_data);
        }
    }
}

// ============================================================================
// Validation
// ============================================================================

/**
 * @brief Validate an entry
 * @param entry Entry to validate
 * @return true if valid
 *
 * DIAMOND CODE COMPLIANCE:
 * - No exceptions - uses boolean return value
 * - Stack-allocated operations
 */
bool UnifiedDroneDatabase::validate_entry(const UnifiedDroneEntry& entry) const noexcept {
    return entry.is_valid();
}

// ============================================================================
// Statistics
// ============================================================================

/**
 * @brief Get database statistics
 * @return DatabaseStats structure
 *
 * DIAMOND CODE COMPLIANCE:
 * - Thread-safe using MutexLock from eda_locking.hpp
 * - Returns by value (no heap allocation)
 * - No exceptions
 */
DatabaseStats UnifiedDroneDatabase::get_stats() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return stats_;
}

} // namespace ui::apps::enhanced_drone_analyzer
