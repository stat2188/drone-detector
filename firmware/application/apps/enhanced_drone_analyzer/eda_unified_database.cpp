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
#include <cstring>

// Third-party library headers (alphabetical order)
#include <ch.h>

// Project-specific headers (alphabetical order)
#include "eda_constants.hpp"
#include "eda_locking.hpp"
#include "eda_validation.hpp"
#include "file.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// P1-HIGH FIX #3: chMtxUnlock() API Compatibility
// ============================================================================
// PROBLEM: chMtxUnlock() called without mutex parameter
//   - ChibiOS API requires mutex pointer parameter for unlock
//   - Original code: chMtxUnlock(); (missing parameter)
//   - Causes undefined behavior or incorrect mutex unlock
//
// SOLUTION: Use correct API with mutex pointer parameter
//   - Fixed: chMtxUnlock(&mutex_);
//   - Ensures proper mutex unlock for ChibiOS version compatibility
//
// API COMPATIBILITY:
// - ChibiOS 20.x: chMtxUnlock(mutex_t *mp)
// - ChibiOS 21.x+: chMtxUnlock(mutex_t *mp)
// - Both versions require mutex pointer parameter
// ============================================================================

// Static member definitions (ODR compliance)
UnifiedDroneDatabase::DatabaseEntry UnifiedDroneDatabase::database_entries_[UnifiedDroneDatabase::MAX_DATABASE_ENTRIES];
size_t UnifiedDroneDatabase::database_entry_count_ = 0;
Mutex UnifiedDroneDatabase::mutex_;
volatile bool UnifiedDroneDatabase::initialized_ = false;

// Observer pattern support
static constexpr size_t MAX_OBSERVERS = 4;
static UnifiedDroneDatabase::ObserverCallback observer_callbacks_[MAX_OBSERVERS] = {nullptr};
static void* observer_user_data_[MAX_OBSERVERS] = {nullptr};
size_t UnifiedDroneDatabase::observer_count_ = 0;

// ============================================================================
// Constructor / Destructor
// ============================================================================

UnifiedDroneDatabase::UnifiedDroneDatabase() noexcept {
    chMtxObjectInit(&mutex_);
    initialized_.store(false);
    database_entry_count_ = 0;
    observer_count_ = 0;
    
    // Initialize all entries to zero
    for (size_t i = 0; i < MAX_DATABASE_ENTRIES; ++i) {
        database_entries_[i] = DatabaseEntry{};
    }
}

UnifiedDroneDatabase::~UnifiedDroneDatabase() noexcept {
    // Cleanup is handled by clear()
    // Mutex is automatically cleaned up by ChibiOS
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
    database_entry_count_ = 0;
    
    return true;
}

void UnifiedDroneDatabase::clear() noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    // Reset all entries to zero
    for (size_t i = 0; i < MAX_DATABASE_ENTRIES; ++i) {
        database_entries_[i] = DatabaseEntry{};
    }
    
    database_entry_count_ = 0;
}

// ============================================================================
// Thread Safety
// ============================================================================

void UnifiedDroneDatabase::lock() noexcept {
    chMtxLock(&mutex_);
}

void UnifiedDroneDatabase::unlock() noexcept {
    // P1-HIGH FIX #3: Use correct API with mutex pointer parameter
    // ChibiOS requires mutex pointer parameter for unlock operation
    // This ensures proper mutex unlock for version compatibility
    chMtxUnlock(&mutex_);
}

// ============================================================================
// Database Operations
// ============================================================================

bool UnifiedDroneDatabase::add_entry(const DatabaseEntry& entry) noexcept {
    // Guard clause: validate entry before adding
    if (!entry.is_valid()) {
        return false;
    }
    
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    // Check if database is full
    if (database_entry_count_ >= MAX_DATABASE_ENTRIES) {
        return false;
    }
    
    // Find empty slot
    size_t empty_index = MAX_DATABASE_ENTRIES;
    for (size_t i = 0; i < MAX_DATABASE_ENTRIES; ++i) {
        if (database_entries_[i].frequency_hz == 0) {
            empty_index = i;
            break;
        }
    }
    
    if (empty_index >= MAX_DATABASE_ENTRIES) {
        return false;  // No empty slot found
    }
    
    // Add entry
    database_entries_[empty_index] = entry;
    database_entry_count_++;
    
    // Notify observers
    notify_observers(DatabaseEventType::ENTRY_ADDED);
    
    return true;
}

bool UnifiedDroneDatabase::remove_entry(uint64_t frequency_hz) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    bool found = false;
    for (size_t i = 0; i < database_entry_count_; ++i) {
        if (database_entries_[i].frequency_hz == frequency_hz) {
            // Remove entry by shifting remaining entries
            for (size_t j = i; j < database_entry_count_ - 1; ++j) {
                database_entries_[j] = database_entries_[j + 1];
            }
            // Clear last entry
            database_entries_[database_entry_count_ - 1] = DatabaseEntry{};
            database_entry_count_--;
            found = true;
            break;
        }
    }
    
    if (found) {
        notify_observers(DatabaseEventType::ENTRY_DELETED);
    }
    
    return found;
}

const UnifiedDroneDatabase::DatabaseEntry* UnifiedDroneDatabase::get_entry(size_t index) const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (index >= database_entry_count_) {
        return nullptr;
    }
    
    return &database_entries_[index];
}

size_t UnifiedDroneDatabase::size() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return database_entry_count_;
}

bool UnifiedDroneDatabase::empty() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return database_entry_count_ == 0;
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
    
    File db_file;
    if (!db_file.open(file_path, true)) {  // read_only
        return false;
    }
    
    // Read file content
    static constexpr size_t BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    auto read_result = db_file.read(buffer, BUFFER_SIZE);
    
    if (read_result.is_error() || read_result.value() == 0) {
        db_file.close();
        return false;
    }
    
    // Null-terminate buffer
    size_t bytes_read = read_result.value();
    if (bytes_read < BUFFER_SIZE) {
        buffer[bytes_read] = '\0';
    } else {
        buffer[BUFFER_SIZE - 1] = '\0';
    }
    
    db_file.close();
    
    // Parse file content (simple line-by-line format)
    // Format: "frequency_hz,description\n"
    clear();
    
    const char* line_start = buffer;
    while (*line_start != '\0' && database_entry_count_ < MAX_DATABASE_ENTRIES) {
        // Find end of line
        const char* line_end = line_start;
        while (*line_end != '\0' && *line_end != '\n' && *line_end != '\r') {
            line_end++;
        }
        
        // Skip empty lines
        if (line_end == line_start) {
            line_start = (*line_end == '\0') ? line_end : line_end + 1;
            continue;
        }
        
        // Parse frequency and description
        char temp_line[128];
        size_t line_len = line_end - line_start;
        if (line_len >= sizeof(temp_line)) {
            line_len = sizeof(temp_line) - 1;
        }
        std::memcpy(temp_line, line_start, line_len);
        temp_line[line_len] = '\0';
        
        // Parse frequency
        uint64_t freq = 0;
        char* comma = std::strchr(temp_line, ',');
        if (comma != nullptr) {
            *comma = '\0';
            freq = std::strtoull(temp_line, nullptr, 10);
        }
        
        // Parse description
        const char* desc = (comma != nullptr) ? (comma + 1) : "";
        
        // Validate and add entry
        if (freq > 0 && EDA::Validation::validate_frequency(freq)) {
            DatabaseEntry entry{};
            entry.frequency_hz = freq;
            
            // Copy description safely
            size_t desc_len = 0;
            while (desc[desc_len] != '\0' && desc_len < sizeof(entry.description) - 1) {
                entry.description[desc_len++] = desc[desc_len];
            }
            entry.description[desc_len] = '\0';
            
            if (entry.is_valid()) {
                database_entries_[database_entry_count_++] = entry;
            }
        }
        
        // Move to next line
        line_start = (*line_end == '\0') ? line_end : line_end + 1;
        while (*line_start == '\n' || *line_start == '\r') {
            line_start++;
        }
    }
    
    if (database_entry_count_ > 0) {
        notify_observers(DatabaseEventType::DATABASE_RELOADED);
    }
    
    return database_entry_count_ > 0;
}

bool UnifiedDroneDatabase::save(const char* file_path) noexcept {
    // Guard clause: null pointer check
    if (!file_path) {
        return false;
    }
    
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    File db_file;
    if (!db_file.open(file_path, false)) {  // write mode
        return false;
    }
    
    // Write header
    static constexpr const char* HEADER = "# Unified Drone Database\n# Format: frequency_hz,description\n\n";
    auto header_result = db_file.write(HEADER, std::strlen(HEADER));
    if (header_result.is_error() || header_result.value() != std::strlen(HEADER)) {
        db_file.close();
        return false;
    }
    
    // Write entries
    for (size_t i = 0; i < database_entry_count_; ++i) {
        const auto& entry = database_entries_[i];
        
        // Format line
        static constexpr size_t LINE_BUFFER_SIZE = 128;
        char line_buffer[LINE_BUFFER_SIZE];
        int written = std::snprintf(line_buffer, LINE_BUFFER_SIZE,
                                       "%llu,%s\n",
                                       static_cast<unsigned long long>(entry.frequency_hz),
                                       entry.description);
        
        if (written < 0 || written >= LINE_BUFFER_SIZE) {
            db_file.close();
            return false;
        }
        
        auto write_result = db_file.write(line_buffer, static_cast<size_t>(written));
        if (write_result.is_error() || write_result.value() != static_cast<size_t>(written)) {
            db_file.close();
            return false;
        }
    }
    
    db_file.close();
    return true;
}

// ============================================================================
// Observer Pattern Implementation
// ============================================================================

bool UnifiedDroneDatabase::add_observer(ObserverCallback callback, void* user_data) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (observer_count_ >= MAX_OBSERVERS) {
        return false;  // Observer list full
    }
    
    observer_callbacks_[observer_count_] = callback;
    observer_user_data_[observer_count_] = user_data;
    observer_count_++;
    
    return true;
}

void UnifiedDroneDatabase::remove_observer(ObserverCallback callback) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    for (size_t i = 0; i < observer_count_; ++i) {
        if (observer_callbacks_[i] == callback) {
            // Shift remaining observers
            for (size_t j = i; j < observer_count_ - 1; ++j) {
                observer_callbacks_[j] = observer_callbacks_[j + 1];
                observer_user_data_[j] = observer_user_data_[j + 1];
            }
            observer_count_--;
            return;
        }
    }
}

void UnifiedDroneDatabase::notify_observers(DatabaseEventType event) noexcept {
    // Create event structure
    DatabaseChangeEvent change_event{};
    change_event.type = event;
    
    // Notify all observers
    for (size_t i = 0; i < observer_count_; ++i) {
        if (observer_callbacks_[i] != nullptr) {
            observer_callbacks_[i](change_event, observer_user_data_[i]);
        }
    }
}

// ============================================================================
// DatabaseEntry Implementation
// ============================================================================

bool UnifiedDroneDatabase::DatabaseEntry::is_valid() const noexcept {
    return frequency_hz > 0 &&
           EDA::Validation::validate_frequency(frequency_hz) &&
           description[0] != '\0';
}

} // namespace ui::apps::enhanced_drone_analyzer
