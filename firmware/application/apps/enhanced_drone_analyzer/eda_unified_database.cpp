// eda_unified_database.cpp - Unified Drone Database Implementation
// Stage 4, Part 3 Fix: P1-3 - chMtxUnlock() API Compatibility
//
// This file implements a unified database for drone frequency entries with thread-safe
// operations and observer pattern support.

#include "eda_unified_database.hpp"

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

// Project-specific headers (alphabetical order)
#include "chmtx.h"
#include "eda_locking.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// P1-HIGH FIX #3: chMtxUnlock() API Compatibility
// ============================================================================
// PROBLEM: chMtxUnlock() API usage
//   - ChibiOS 21.x+ API does NOT require mutex pointer parameter for unlock
//   - chMtxUnlock() unlocks the last locked mutex automatically
//   - Returns pointer to unlocked mutex for verification
//
// SOLUTION: Use correct API without mutex parameter
//   - Fixed: chMtxUnlock();
//   - Ensures proper mutex unlock for ChibiOS 21.x+ version compatibility
//
// API COMPATIBILITY:
// - ChibiOS 20.x: chMtxUnlock(mutex_t *mp) - requires parameter
// - ChibiOS 21.x+: chMtxUnlock() - unlocks last locked mutex, no parameter
// - This codebase uses ChibiOS 21.x+ API pattern
//
// NOTE: See eda_locking.hpp:244-256 for MutexLock implementation
// ============================================================================

// ============================================================================
// Singleton Instance
// ============================================================================

UnifiedDroneDatabase& UnifiedDroneDatabase::instance() noexcept {
    static UnifiedDroneDatabase instance;
    return instance;
}

// ============================================================================
// Constructor / Destructor
// ============================================================================

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
// Initialization
// ============================================================================

bool UnifiedDroneDatabase::initialize() noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (initialized_) {
        return true;  // Already initialized
    }
    
    initialized_ = true;
    entry_count_ = 0;
    
    return true;
}

// ============================================================================
// Clear
// ============================================================================

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

void UnifiedDroneDatabase::lock() noexcept {
    chMtxLock(&mutex_);
}

void UnifiedDroneDatabase::unlock() noexcept {
    // P1-HIGH FIX #3: Use correct API without mutex parameter
    // ChibiOS API (>=21.x) unlocks last locked mutex without parameter
    // This ensures proper mutex unlock for version compatibility
    chMtxUnlock();
}

// ============================================================================
// Entry Access
// ============================================================================

DatabaseView UnifiedDroneDatabase::get_view() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return DatabaseView(entries_.data(), entry_count_);
}

size_t UnifiedDroneDatabase::size() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return entry_count_;
}

bool UnifiedDroneDatabase::empty() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return entry_count_ == 0;
}

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
    // The File API (File::read/write) needs review for compatibility with the
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
    // The File API (File::write) needs review for compatibility with the
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

void UnifiedDroneDatabase::remove_observer(DatabaseObserverCallback callback) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    for (size_t i = 0; i < DatabaseConfig::MAX_OBSERVERS; ++i) {
        if (observers_[i].active && observers_[i].callback == callback) {
            observers_[i] = ObserverEntry{};  // Reset to default
            return;
        }
    }
}

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

bool UnifiedDroneDatabase::validate_entry(const UnifiedDroneEntry& entry) const noexcept {
    return entry.is_valid();
}

// ============================================================================
// Statistics
// ============================================================================

DatabaseStats UnifiedDroneDatabase::get_stats() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return stats_;
}

} // namespace ui::apps::enhanced_drone_analyzer
