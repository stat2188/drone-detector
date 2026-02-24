// Unified Database Implementation for Enhanced Drone Analyzer
// Stage 4: Single Source of Truth for Drone Frequency Database

#include "eda_unified_database.hpp"
#include "eda_database_parser.hpp"
#include "file.hpp"
#include "sd_card.hpp"
#include <cstdio>
#include <cstring>

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// Constructor
// ============================================================================

UnifiedDroneDatabase::UnifiedDroneDatabase() noexcept 
    : entry_count_(0)
    , initialized_(false) {
    // Initialize mutex
    chMtxInit(&mutex_);
    
    // Clear all entries
    for (auto& entry : entries_) {
        entry.clear();
    }
    
    // Initialize stats
    stats_.entry_count = 0;
    stats_.capacity = DatabaseConfig::MAX_ENTRIES;
    stats_.load_count = 0;
    stats_.save_count = 0;
    stats_.change_count = 0;
    stats_.validation_errors = 0;
}

// ============================================================================
// Lifecycle Methods
// ============================================================================

bool UnifiedDroneDatabase::initialize() noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (initialized_) {
        return true;  // Already initialized
    }
    
    // Clear all entries
    for (auto& entry : entries_) {
        entry.clear();
    }
    entry_count_ = 0;
    initialized_ = true;
    
    return true;
}

bool UnifiedDroneDatabase::load(const char* path) noexcept {
    MutexLock lock(mutex_, LockOrder::SD_CARD_MUTEX);
    
    if (!initialized_) {
        return false;
    }
    
    // Open file for reading
    File file;
    auto error = file.open(path, true, false);
    if (error) {
        // File doesn't exist - not an error, just empty database
        return false;
    }
    
    // Clear existing entries
    for (auto& entry : entries_) {
        entry.clear();
    }
    entry_count_ = 0;
    
    // Read file line by line
    constexpr size_t LINE_BUFFER_SIZE = 128;
    char line_buffer[LINE_BUFFER_SIZE];
    size_t line_pos = 0;
    
    while (true) {
        // Read one byte at a time to find line endings
        char ch;
        auto result = file.read(&ch, 1);
        
        if (!result.is_ok() || result.value() == 0) {
            // End of file or error
            break;
        }
        
        // Accumulate characters until newline or buffer full
        if (ch == '\n' || ch == '\r') {
            if (line_pos > 0) {
                line_buffer[line_pos] = '\0';
                
                // Parse the line
                UnifiedDroneEntry entry;
                if (DatabaseParser::parse_line(line_buffer, entry)) {
                    if (entry_count_ < DatabaseConfig::MAX_ENTRIES) {
                        entries_[entry_count_] = entry;
                        entry_count_++;
                    }
                }
                
                line_pos = 0;
            }
        } else if (line_pos < LINE_BUFFER_SIZE - 1) {
            line_buffer[line_pos++] = ch;
        }
    }
    
    // Process last line if no trailing newline
    if (line_pos > 0) {
        line_buffer[line_pos] = '\0';
        
        UnifiedDroneEntry entry;
        if (DatabaseParser::parse_line(line_buffer, entry)) {
            if (entry_count_ < DatabaseConfig::MAX_ENTRIES) {
                entries_[entry_count_] = entry;
                entry_count_++;
            }
        }
    }
    
    file.close();
    
    // Update stats
    stats_.entry_count = entry_count_;
    stats_.load_count++;
    
    // Notify observers of database reload
    DatabaseChangeEvent event{
        DatabaseEventType::DATABASE_RELOADED,
        0,
        entry_count_,
        nullptr
    };
    notify_observers(event);
    
    return true;
}

bool UnifiedDroneDatabase::save(const char* path) noexcept {
    MutexLock lock(mutex_, LockOrder::SD_CARD_MUTEX);
    
    if (!initialized_) {
        return false;
    }
    
    // Create file for writing
    File file;
    auto error = file.create(path);
    if (error) {
        return false;
    }
    
    // Write each entry in freqman format
    constexpr size_t LINE_BUFFER_SIZE = 128;
    char line_buffer[LINE_BUFFER_SIZE];
    
    for (size_t i = 0; i < entry_count_; ++i) {
        const auto& entry = entries_[i];
        
        // Format: f=frequency,d=description
        DatabaseParser::write_freqman_line(entry, line_buffer, LINE_BUFFER_SIZE);
        
        // Write line
        auto write_result = file.write(line_buffer, std::strlen(line_buffer));
        if (!write_result.is_ok()) {
            file.close();
            return false;
        }
        
        // Write newline
        char newline = '\n';
        write_result = file.write(&newline, 1);
        if (!write_result.is_ok()) {
            file.close();
            return false;
        }
    }
    
    file.close();
    
    // Update stats
    stats_.save_count++;
    
    return true;
}

void UnifiedDroneDatabase::clear() noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    for (auto& entry : entries_) {
        entry.clear();
    }
    entry_count_ = 0;
    
    // Update stats
    stats_.entry_count = 0;
    
    // Notify observers
    DatabaseChangeEvent event{
        DatabaseEventType::DATABASE_CLEARED,
        0,
        0,
        nullptr
    };
    notify_observers(event);
}

// ============================================================================
// Entry Access Methods
// ============================================================================

DatabaseView UnifiedDroneDatabase::get_view() const noexcept {
    return DatabaseView(entries_.data(), entry_count_);
}

size_t UnifiedDroneDatabase::size() const noexcept {
    return entry_count_;
}

bool UnifiedDroneDatabase::empty() const noexcept {
    return entry_count_ == 0;
}

const UnifiedDroneEntry* UnifiedDroneDatabase::get_entry(size_t index) const noexcept {
    if (index >= entry_count_) {
        return nullptr;
    }
    return &entries_[index];
}

// ============================================================================
// Modification Methods
// ============================================================================

int UnifiedDroneDatabase::add_entry(const UnifiedDroneEntry& entry) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (!initialized_) {
        return -1;
    }
    
    // Validate entry
    if (!validate_entry(entry)) {
        stats_.validation_errors++;
        return -1;
    }
    
    // Check capacity
    if (entry_count_ >= DatabaseConfig::MAX_ENTRIES) {
        return -1;
    }
    
    // Add entry
    size_t index = entry_count_;
    entries_[index] = entry;
    entry_count_++;
    
    // Update stats
    stats_.entry_count = entry_count_;
    stats_.change_count++;
    
    // Notify observers
    DatabaseChangeEvent event{
        DatabaseEventType::ENTRY_ADDED,
        index,
        1,
        &entries_[index]
    };
    notify_observers(event);
    
    return static_cast<int>(index);
}

bool UnifiedDroneDatabase::update_entry(size_t index, const UnifiedDroneEntry& entry) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (!initialized_) {
        return false;
    }
    
    // Bounds check
    if (index >= entry_count_) {
        return false;
    }
    
    // Validate entry
    if (!validate_entry(entry)) {
        stats_.validation_errors++;
        return false;
    }
    
    // Update entry
    entries_[index] = entry;
    
    // Update stats
    stats_.change_count++;
    
    // Notify observers
    DatabaseChangeEvent event{
        DatabaseEventType::ENTRY_MODIFIED,
        index,
        1,
        &entries_[index]
    };
    notify_observers(event);
    
    return true;
}

bool UnifiedDroneDatabase::delete_entry(size_t index) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (!initialized_) {
        return false;
    }
    
    // Bounds check
    if (index >= entry_count_) {
        return false;
    }
    
    // Shift entries down
    for (size_t i = index; i < entry_count_ - 1; ++i) {
        entries_[i] = entries_[i + 1];
    }
    
    // Clear last entry
    entries_[entry_count_ - 1].clear();
    entry_count_--;
    
    // Update stats
    stats_.entry_count = entry_count_;
    stats_.change_count++;
    
    // Notify observers
    DatabaseChangeEvent event{
        DatabaseEventType::ENTRY_DELETED,
        index,
        1,
        nullptr
    };
    notify_observers(event);
    
    return true;
}

// ============================================================================
// Observer Pattern
// ============================================================================

bool UnifiedDroneDatabase::add_observer(DatabaseObserverCallback callback, void* user_data) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    if (callback == nullptr) {
        return false;
    }
    
    // Find empty slot
    for (auto& observer : observers_) {
        if (!observer.active) {
            observer.callback = callback;
            observer.user_data = user_data;
            observer.active = true;
            return true;
        }
    }
    
    // No empty slots
    return false;
}

void UnifiedDroneDatabase::remove_observer(DatabaseObserverCallback callback) noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    
    for (auto& observer : observers_) {
        if (observer.callback == callback) {
            observer.callback = nullptr;
            observer.user_data = nullptr;
            observer.active = false;
        }
    }
}

void UnifiedDroneDatabase::notify_observers(const DatabaseChangeEvent& event) noexcept {
    // Note: Called while mutex is held - observers must not call back into database
    for (const auto& observer : observers_) {
        if (observer.active && observer.callback != nullptr) {
            observer.callback(event, observer.user_data);
        }
    }
}

// ============================================================================
// Validation
// ============================================================================

bool UnifiedDroneDatabase::validate_entry(const UnifiedDroneEntry& entry) const noexcept {
    auto result = FrequencyValidation::validate_entry(entry);
    return result.valid;
}

// ============================================================================
// Statistics
// ============================================================================

DatabaseStats UnifiedDroneDatabase::get_stats() const noexcept {
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    return stats_;
}

// ============================================================================
// Thread Safety
// ============================================================================

void UnifiedDroneDatabase::lock() noexcept {
    chMtxLock(&mutex_);
}

void UnifiedDroneDatabase::unlock() noexcept {
    chMtxUnlock();
}

} // namespace ui::apps::enhanced_drone_analyzer
