# Stage 4: Unified Database Architecture Design

## Executive Summary

This document presents a comprehensive architecture for unifying the database subsystem in the Enhanced Drone Analyzer (EDA) app. The design addresses four critical issues:

1. **Two Separate Database Loaders** - Inconsistent data access patterns
2. **No Real-Time Database Sync** - Stale data between UI and scanner
3. **Entry Limit Mismatch** - 100 vs 150 vs 300+ entries
4. **Missing Frequency Validation** - Invalid frequencies in UI editor

---

## 1. Current State Analysis

### 1.1 Database Loading Mechanisms

#### Scanner Path (DroneScanner::load_frequency_database)
```
Location: ui_enhanced_drone_analyzer.cpp:295-344
Loader: FreqmanDB (freqman_db.hpp)
Format: freqman format - f=freq,d=desc
Max Entries: 150 (hardcoded in options.max_entries)
Storage: FreqmanDB* freq_db_ptr_ (placement new on static storage)
```

#### UI Path (DroneDatabaseManager::load_database)
```
Location: ui_enhanced_drone_settings.cpp:895-941
Loader: Custom CSV parser
Format: CSV - frequency,description
Max Entries: 100 (MAX_DATABASE_ENTRIES constant)
Storage: DatabaseView (stack-allocated struct)
```

### 1.2 Entry Limit Mismatch Analysis

| Component | Limit | Location |
|-----------|-------|----------|
| DroneDatabaseManager | 100 | `MAX_DATABASE_ENTRIES` in ui_enhanced_drone_settings.hpp:188 |
| DroneScanner | 150 | `options.max_entries` in ui_enhanced_drone_analyzer.cpp:299 |
| freqman_default_max_entries | 150 | freqman_db.hpp:162 |
| DRONES.TXT actual | 300+ | User database file |

**Risk**: Silent truncation leads to missing frequencies in scans.

### 1.3 Memory Constraints (STM32F405)

```
Total RAM: 128 KB
Available for EDA: ~32 KB (after system overhead)
Current Database Memory:
  - FreqmanDB storage: 4 KB (FREQ_DB_STORAGE_SIZE)
  - TrackedDrones storage: ~800 bytes
  - DatabaseView: ~6.4 KB (100 entries × 64 bytes each)
```

---

## 2. Unified Database Interface Design

### 2.1 Design Goals

1. **Single Source of Truth** - One database instance shared by scanner and UI
2. **Zero-Overhead Abstraction** - No virtual functions in hot path
3. **Heap-Free Operation** - All storage is static/fixed-size
4. **Thread-Safe Access** - Mutex protection for concurrent access
5. **Observable Changes** - Callback mechanism for UI updates

### 2.2 Core Interface (IDroneDatabase)

```cpp
// File: eda_unified_database.hpp

#ifndef EDA_UNIFIED_DATABASE_HPP_
#define EDA_UNIFIED_DATABASE_HPP_

#include <cstdint>
#include <array>
#include <cstddef>
#include "eda_constants.hpp"
#include "ui_drone_common_types.hpp"
#include <ch.h>
#include <chtypes.h>

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// Unified entry structure - compatible with both formats
struct UnifiedDroneEntry {
    Frequency frequency_hz = 0;
    char description[32] = "";  // Match freqman_max_desc_size
    uint8_t drone_type = 0;     // DroneType enum value
    uint8_t threat_level = 0;   // ThreatLevel enum value
    uint8_t flags = 0;          // Reserved for future use
    
    constexpr bool is_valid() const noexcept {
        return frequency_hz > 0;
    }
    
    constexpr bool has_description() const noexcept {
        return description[0] != '\0';
    }
};

// Compile-time configuration
namespace DatabaseConfig {
    // Unified limit based on memory analysis
    // See Section 4 for memory calculations
    constexpr size_t MAX_ENTRIES = 120;
    
    // Memory-mapped regions for zero-copy access
    constexpr size_t ENTRY_SIZE = sizeof(UnifiedDroneEntry);
    constexpr size_t DATABASE_SIZE = MAX_ENTRIES * ENTRY_SIZE;
    
    // Validation thresholds
    constexpr Frequency MIN_FREQ = EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ;
    constexpr Frequency MAX_FREQ = EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;
}

// Database change event types
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
    size_t index;           // Affected entry index
    size_t count;           // Number of affected entries
    const UnifiedDroneEntry* entry;  // Pointer to entry (nullptr for bulk ops)
};

// Observer callback type - function pointer (zero heap)
using DatabaseObserverCallback = void(*)(const DatabaseChangeEvent& event, void* user_data);

// Maximum number of observers (fixed-size array)
constexpr size_t MAX_DATABASE_OBSERVERS = 4;

// Forward declaration
class UnifiedDroneDatabase;

// Database statistics for monitoring
struct DatabaseStats {
    size_t entry_count;
    size_t capacity;
    uint32_t load_count;
    uint32_t save_count;
    uint32_t change_count;
    uint32_t validation_errors;
};

// Non-owning view for iteration (zero-copy)
class DatabaseView {
public:
    constexpr DatabaseView(const UnifiedDroneEntry* entries, size_t count) noexcept
        : entries_(entries), count_(count) {}
    
    constexpr const UnifiedDroneEntry& operator[](size_t index) const noexcept {
        return entries_[index];
    }
    
    constexpr size_t size() const noexcept { return count_; }
    constexpr bool empty() const noexcept { return count_ == 0; }
    
    // Iterator support for range-based for
    const UnifiedDroneEntry* begin() const noexcept { return entries_; }
    const UnifiedDroneEntry* end() const noexcept { return entries_ + count_; }
    
private:
    const UnifiedDroneEntry* entries_;
    size_t count_;
};

// Main unified database class
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
    bool initialize() noexcept;
    
    // Load from file (replaces current content)
    bool load_from_file(const char* path = "/FREQMAN/DRONES.TXT") noexcept;
    
    // Save to file
    bool save_to_file(const char* path = "/FREQMAN/DRONES.TXT") noexcept;
    
    // Clear all entries
    void clear() noexcept;
    
    // === Entry Access ===
    
    // Get read-only view (thread-safe snapshot)
    DatabaseView get_view() const noexcept;
    
    // Get entry count
    size_t size() const noexcept;
    size_t capacity() const noexcept { return DatabaseConfig::MAX_ENTRIES; }
    bool empty() const noexcept;
    
    // Get single entry (bounds-checked)
    const UnifiedDroneEntry* get_entry(size_t index) const noexcept;
    
    // === Modification (triggers observer notifications) ===
    
    // Add entry (returns index or -1 if full)
    int add_entry(const UnifiedDroneEntry& entry) noexcept;
    
    // Update entry at index
    bool update_entry(size_t index, const UnifiedDroneEntry& entry) noexcept;
    
    // Delete entry at index
    bool delete_entry(size_t index) noexcept;
    
    // === Observer Pattern ===
    
    // Register observer (returns false if max observers reached)
    bool register_observer(DatabaseObserverCallback callback, void* user_data) noexcept;
    
    // Unregister observer
    void unregister_observer(DatabaseObserverCallback callback) noexcept;
    
    // === Validation ===
    
    // Validate entry before adding
    bool validate_entry(const UnifiedDroneEntry& entry) const noexcept;
    
    // Get statistics
    DatabaseStats get_stats() const noexcept;
    
    // === Thread Safety ===
    
    // Acquire mutex for bulk operations
    void lock() noexcept;
    void unlock() noexcept;
    
private:
    UnifiedDroneDatabase() noexcept = default;
    ~UnifiedDroneDatabase() = default;
    
    // Notify all observers
    void notify_observers(const DatabaseChangeEvent& event) noexcept;
    
    // Parse freqman format line
    bool parse_freqman_line(const char* line, UnifiedDroneEntry& entry) noexcept;
    
    // Parse CSV format line
    bool parse_csv_line(const char* line, UnifiedDroneEntry& entry) noexcept;
    
    // Format entry for file output
    void format_entry(const UnifiedDroneEntry& entry, char* buffer, size_t size) noexcept;
    
    // Storage (static allocation)
    alignas(alignof(UnifiedDroneEntry))
    static uint8_t storage_[DatabaseConfig::DATABASE_SIZE];
    
    // Entry array (placement new on storage_)
    std::array<UnifiedDroneEntry, DatabaseConfig::MAX_ENTRIES> entries_;
    
    // State
    size_t entry_count_ = 0;
    bool initialized_ = false;
    
    // Thread synchronization
    mutable Mutex mutex_;
    
    // Observers (fixed-size array, no heap)
    struct Observer {
        DatabaseObserverCallback callback = nullptr;
        void* user_data = nullptr;
        bool active = false;
    };
    std::array<Observer, MAX_DATABASE_OBSERVERS> observers_;
    
    // Statistics
    mutable DatabaseStats stats_{};
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_UNIFIED_DATABASE_HPP_
```

### 2.3 Class Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    UnifiedDroneDatabase                         │
│                      (Singleton)                                │
├─────────────────────────────────────────────────────────────────┤
│ - entries_: std::array<UnifiedDroneEntry, 120>                 │
│ - entry_count_: size_t                                          │
│ - mutex_: Mutex                                                 │
│ - observers_: std::array<Observer, 4>                          │
│ - stats_: DatabaseStats                                         │
├─────────────────────────────────────────────────────────────────┤
│ + instance(): UnifiedDroneDatabase&                            │
│ + initialize(): bool                                            │
│ + load_from_file(path): bool                                    │
│ + save_to_file(path): bool                                      │
│ + get_view(): DatabaseView                                      │
│ + add_entry(entry): int                                         │
│ + update_entry(index, entry): bool                              │
│ + delete_entry(index): bool                                     │
│ + register_observer(callback, user_data): bool                  │
│ + unregister_observer(callback): void                           │
│ + validate_entry(entry): bool                                   │
│ + lock() / unlock(): void                                       │
└─────────────────────────────────────────────────────────────────┘
                    │
                    │ notifies
                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Observer Pattern                           │
├─────────────────────────────────────────────────────────────────┤
│ DatabaseObserverCallback = void(*)(DatabaseChangeEvent&, void*)│
│                                                                 │
│ Observers:                                                      │
│   1. DroneScanner - reloads frequency list on change           │
│   2. DroneDatabaseListView - refreshes UI display              │
│   3. DroneDetectionLogger - logs database changes              │
│   4. (Reserved for future use)                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 3. Real-Time Sync Mechanism

### 3.1 Observer Pattern (Zero Heap)

The observer pattern uses function pointers and fixed-size arrays to avoid heap allocation:

```cpp
// Observer registration example
class DroneScanner {
public:
    void on_database_change(const DatabaseChangeEvent& event, void* user_data) {
        auto* scanner = static_cast<DroneScanner*>(user_data);
        switch (event.type) {
            case DatabaseEventType::ENTRY_ADDED:
            case DatabaseEventType::ENTRY_DELETED:
            case DatabaseEventType::DATABASE_RELOADED:
                scanner->schedule_database_reload();
                break;
            default:
                break;
        }
    }
    
    void register_database_observer() {
        UnifiedDroneDatabase::instance().register_observer(
            &DroneScanner::on_database_change,
            this
        );
    }
};
```

### 3.2 Thread Synchronization Strategy

```
┌─────────────────────────────────────────────────────────────────┐
│                    Thread Architecture                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐     ┌─────────────┐     ┌─────────────┐       │
│  │  UI Thread  │     │Scanner Thread│    │ Logger Thread│       │
│  │  (Main)     │     │ (Background) │    │ (Background) │       │
│  └──────┬──────┘     └──────┬──────┘     └──────┬──────┘       │
│         │                   │                   │               │
│         │    ┌──────────────┴───────────────┐   │               │
│         │    │     UnifiedDroneDatabase     │   │               │
│         │    │         ┌───────┐            │   │               │
│         ├────┼────────►│ Mutex │◄───────────┼───┤               │
│         │    │         └───────┘            │   │               │
│         │    │                              │   │               │
│         │    │  ┌────────────────────────┐  │   │               │
│         │    │  │   Observer Callbacks   │  │   │               │
│         │    │  │  ┌─────┐ ┌─────┐       │  │   │               │
│         │    │  │  │Scanner│ │ UI │ ...   │  │   │               │
│         │    │  │  └─────┘ └─────┘       │  │   │               │
│         │    │  └────────────────────────┘  │   │               │
│         │    └──────────────────────────────┘   │               │
│         │                   │                   │               │
│         ▼                   ▼                   ▼               │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    SD Card (FatFS)                       │   │
│  │                  Protected by sd_card_mutex              │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 3.3 Lock Ordering (Deadlock Prevention)

```cpp
// Lock order (always acquire in ascending order):
// 1. sd_card_mutex (global)
// 2. UnifiedDroneDatabase::mutex_
// 3. DroneScanner::data_mutex
// 4. DroneHardwareController::spectrum_mutex_

// Example: UI modifying database
void DroneDatabaseListView::save_changes() noexcept {
    // Order: sd_card_mutex -> database mutex
    MutexLock sd_lock(sd_card_mutex, LockOrder::SD_CARD_MUTEX);
    UnifiedDroneDatabase::instance().lock();
    
    // ... perform file I/O and database updates ...
    
    UnifiedDroneDatabase::instance().unlock();
    // sd_lock released automatically
}
```

### 3.4 ChibiOS Synchronization Primitives

```cpp
// Mutex wrapper for RAII
class ScopedMutexLock {
public:
    explicit ScopedMutexLock(Mutex& mutex) noexcept : mutex_(mutex) {
        chMtxLock(&mutex_);
    }
    ~ScopedMutexLock() noexcept {
        chMtxUnlock();
    }
private:
    Mutex& mutex_;
};

// Semaphore for async notifications
class DatabaseReloadSignal {
public:
    void signal() noexcept {
        chSemSignal(&semaphore_);
    }
    bool wait(sysinterval_t timeout = TIME_INFINITE) noexcept {
        return chSemWaitTimeout(&semaphore_, timeout) == RDY_OK;
    }
private:
    Semaphore semaphore_;
};
```

---

## 4. Memory Layout Calculations

### 4.1 Entry Size Analysis

```cpp
struct UnifiedDroneEntry {
    Frequency frequency_hz;      // 8 bytes (uint64_t)
    char description[32];        // 32 bytes
    uint8_t drone_type;          // 1 byte
    uint8_t threat_level;        // 1 byte
    uint8_t flags;               // 1 byte
    // Padding:                   // 5 bytes (align to 8)
};  // Total: 48 bytes per entry
```

### 4.2 Database Memory Budget

| Component | Size | Calculation |
|-----------|------|-------------|
| UnifiedDroneEntry | 48 bytes | struct size |
| MAX_ENTRIES | 120 | Configured limit |
| **Total Entry Storage** | **5,760 bytes** | 48 × 120 |
| Observer array | 64 bytes | 4 × 16 bytes |
| Mutex | ~40 bytes | ChibiOS Mutex |
| Stats | 20 bytes | DatabaseStats |
| **Total Database** | **~5,884 bytes** | ~5.75 KB |

### 4.3 Comparison with Current Implementation

| Component | Current | Proposed | Savings |
|-----------|---------|----------|---------|
| FreqmanDB storage | 4,096 bytes | - | -4,096 |
| DatabaseView (UI) | 6,400 bytes | - | -6,400 |
| UnifiedDroneDatabase | - | 5,884 bytes | +5,884 |
| **Net Change** | 10,496 bytes | 5,884 bytes | **-4,612 bytes** |

### 4.4 Memory Map

```
┌─────────────────────────────────────────────────────────────────┐
│                    EDA Memory Layout                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  .bss Section (Static Storage)                                  │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ UnifiedDroneDatabase::storage_        5,760 bytes       │   │
│  │ TrackedDrones storage                   800 bytes       │   │
│  │ DetectionRingBuffer                    512 bytes       │   │
│  │ Spectrum buffers                     1,024 bytes       │   │
│  │ Thread stacks (3 threads)           12,288 bytes       │   │
│  │ ─────────────────────────────────────────────────────   │   │
│  │ Subtotal                            20,384 bytes       │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  .data Section (Initialized Data)                               │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ DroneScanner members                  ~2,000 bytes       │   │
│  │ DroneHardwareController               ~800 bytes       │   │
│  │ View members                        ~1,500 bytes       │   │
│  │ ─────────────────────────────────────────────────────   │   │
│  │ Subtotal                             4,300 bytes       │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  Stack (Main Thread)                      4,096 bytes          │
│                                                                 │
│  ─────────────────────────────────────────────────────────────  │
│  Total EDA Memory                       ~28,780 bytes          │
│  Available RAM                          128 KB                  │
│  Headroom                               ~99 KB                  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 5. Frequency Validation Design

### 5.1 Validation Interface

```cpp
// Extended FrequencyValidation namespace
namespace FrequencyValidation {

// Existing functions (from ui_drone_common_types.hpp)
inline constexpr bool is_valid(uint64_t freq_hz) noexcept;
inline constexpr bool is_safe(uint64_t freq_hz) noexcept;
inline constexpr bool is_range_valid(uint64_t min_freq, uint64_t max_freq) noexcept;
inline constexpr uint64_t clamp(uint64_t freq_hz) noexcept;

// New validation for database entries
struct ValidationResult {
    bool valid;
    const char* error_message;
    uint8_t error_code;
};

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
inline ValidationResult validate_entry(const UnifiedDroneEntry& entry) noexcept {
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
    
    // Check description length
    if (strnlen(entry.description, 32) >= 32) {
        return {false, "Description too long (max 31 chars)",
                static_cast<uint8_t>(ValidationErrorCode::DESCRIPTION_TOO_LONG)};
    }
    
    // Check drone type
    if (entry.drone_type > static_cast<uint8_t>(DroneType::FPV_RACING)) {
        return {false, "Invalid drone type",
                static_cast<uint8_t>(ValidationErrorCode::INVALID_DRONE_TYPE)};
    }
    
    // Check threat level
    if (entry.threat_level > static_cast<uint8_t>(ThreatLevel::UNKNOWN)) {
        return {false, "Invalid threat level",
                static_cast<uint8_t>(ValidationErrorCode::INVALID_THREAT_LEVEL)};
    }
    
    return {true, nullptr, 0};
}

// Check for duplicate frequency
inline bool is_duplicate_frequency(Frequency freq, 
                                    const UnifiedDroneEntry* entries,
                                    size_t count,
                                    size_t exclude_index = SIZE_MAX) noexcept {
    for (size_t i = 0; i < count; ++i) {
        if (i == exclude_index) continue;
        if (entries[i].frequency_hz == freq) return true;
    }
    return false;
}

} // namespace FrequencyValidation
```

### 5.2 UI Integration

```cpp
// In DroneEntryEditorView
template <typename Callback>
class DroneEntryEditorView : public View {
private:
    void on_save() noexcept {
        DroneDbEntry new_entry;
        new_entry.freq = field_freq_.value();
        
        // Validate frequency before saving
        auto validation = FrequencyValidation::validate_entry(
            UnifiedDroneEntry{new_entry.freq, "", 0, 0, 0}
        );
        
        if (!validation.valid) {
            // Show error message to user
            nav_.display_modal("Validation Error", validation.error_message);
            return;
        }
        
        // Check for duplicates
        auto& db = UnifiedDroneDatabase::instance();
        DatabaseView view = db.get_view();
        if (FrequencyValidation::is_duplicate_frequency(
                new_entry.freq, 
                view.begin(), 
                view.size(),
                editing_index_)) {
            nav_.display_modal("Validation Error", "Frequency already exists");
            return;
        }
        
        snprintf(new_entry.description, sizeof(new_entry.description), 
                 "%s", description_buffer_);
        on_save_fn_(new_entry);
        nav_.pop();
    }
    
    size_t editing_index_ = SIZE_MAX;
};
```

---

## 6. File Structure Changes

### 6.1 New Files

```
firmware/application/apps/enhanced_drone_analyzer/
├── eda_unified_database.hpp      # Unified database interface
├── eda_unified_database.cpp      # Implementation
└── eda_database_parser.hpp       # File format parsers
```

### 6.2 Modified Files

```
firmware/application/apps/enhanced_drone_analyzer/
├── ui_enhanced_drone_analyzer.hpp    # Remove FreqmanDB member
├── ui_enhanced_drone_analyzer.cpp    # Use UnifiedDroneDatabase
├── ui_enhanced_drone_settings.hpp    # Remove DroneDatabaseManager
├── ui_enhanced_drone_settings.cpp    # Use UnifiedDroneDatabase
├── ui_drone_common_types.hpp         # Add FrequencyValidation extensions
└── eda_constants.hpp                 # Update MAX_DB_ENTRIES
```

### 6.3 File Dependencies

```
                    ┌─────────────────────┐
                    │  eda_constants.hpp  │
                    └──────────┬──────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                    ui_drone_common_types.hpp                    │
│              (FrequencyValidation, DroneType, etc.)            │
└─────────────────────────────────────────────────────────────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │eda_unified_database │
                    │        .hpp         │
                    └──────────┬──────────┘
                               │
          ┌────────────────────┼────────────────────┐
          │                    │                    │
          ▼                    ▼                    ▼
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│ui_enhanced_drone│  │ui_enhanced_drone│  │   Other EDA     │
│   _analyzer.hpp │  │   _settings.hpp │  │   components    │
└─────────────────┘  └─────────────────┘  └─────────────────┘
```

---

## 7. Implementation Steps

### Phase 1: Foundation (No Breaking Changes)

1. **Create `eda_database_parser.hpp`**
   - Implement freqman format parser
   - Implement CSV format parser
   - Add unit tests for parsers

2. **Create `eda_unified_database.hpp`**
   - Define UnifiedDroneEntry struct
   - Define DatabaseConfig constants
   - Define UnifiedDroneDatabase interface

3. **Create `eda_unified_database.cpp`**
   - Implement static storage allocation
   - Implement basic CRUD operations
   - Implement file I/O (both formats)

### Phase 2: Integration

4. **Update `ui_drone_common_types.hpp`**
   - Add FrequencyValidation extensions
   - Add ValidationResult struct

5. **Update `eda_constants.hpp`**
   - Add `constexpr size_t MAX_DB_ENTRIES = 120;`
   - Document memory calculations

6. **Update `ui_enhanced_drone_settings.cpp`**
   - Replace DroneDatabaseManager with UnifiedDroneDatabase
   - Add observer registration
   - Add frequency validation

### Phase 3: Scanner Integration

7. **Update `ui_enhanced_drone_analyzer.hpp`**
   - Remove FreqmanDB member
   - Add UnifiedDroneDatabase reference
   - Add observer callback

8. **Update `ui_enhanced_drone_analyzer.cpp`**
   - Replace load_frequency_database() with UnifiedDroneDatabase
   - Implement observer callback
   - Add database reload on change notification

### Phase 4: Testing & Validation

9. **Add Integration Tests**
   - Test concurrent access (UI + Scanner)
   - Test file format compatibility
   - Test memory usage

10. **Update Documentation**
    - Update architecture diagrams
    - Document API changes
    - Add migration guide

---

## 8. Code Snippets for Key Interfaces

### 8.1 Database Initialization

```cpp
// In EnhancedDroneSpectrumAnalyzerView constructor
void EnhancedDroneSpectrumAnalyzerView::init_phase_load_database() {
    auto& db = UnifiedDroneDatabase::instance();
    
    // Initialize database
    if (!db.initialize()) {
        handle_init_error("Database init failed");
        return;
    }
    
    // Register scanner as observer
    scanner_.register_database_observer();
    
    // Load from file
    if (!db.load_from_file()) {
        // Load built-in defaults
        load_builtin_frequencies();
    }
    
    init_state_ = InitState::DATABASE_LOADED;
}
```

### 8.2 Scanner Observer Callback

```cpp
// In DroneScanner
void DroneScanner::register_database_observer() {
    UnifiedDroneDatabase::instance().register_observer(
        [](const DatabaseChangeEvent& event, void* user_data) {
            auto* scanner = static_cast<DroneScanner*>(user_data);
            
            switch (event.type) {
                case DatabaseEventType::ENTRY_ADDED:
                case DatabaseEventType::ENTRY_DELETED:
                case DatabaseEventType::DATABASE_RELOADED:
                    // Signal scanner to reload frequency list
                    scanner->database_needs_reload_ = true;
                    break;
                default:
                    break;
            }
        },
        this
    );
}

// In scanner thread loop
void DroneScanner::check_database_reload() {
    if (database_needs_reload_.exchange(false)) {
        MutexLock lock(data_mutex);
        reload_frequency_cache();
    }
}
```

### 8.3 UI Database Save with Validation

```cpp
// In DroneDatabaseListView
void DroneDatabaseListView::save_changes() noexcept {
    auto& db = UnifiedDroneDatabase::instance();
    
    // Lock database for bulk update
    db.lock();
    
    // Validate all entries
    for (size_t i = 0; i < database_view_.count; ++i) {
        UnifiedDroneEntry entry;
        entry.frequency_hz = database_view_.entries[i].freq;
        strncpy(entry.description, database_view_.entries[i].description, 31);
        entry.description[31] = '\0';
        
        auto result = FrequencyValidation::validate_entry(entry);
        if (!result.valid) {
            db.unlock();
            nav_.display_modal("Validation Error", result.error_message);
            return;
        }
    }
    
    // Clear and re-add all entries
    db.clear();
    for (size_t i = 0; i < database_view_.count; ++i) {
        UnifiedDroneEntry entry;
        entry.frequency_hz = database_view_.entries[i].freq;
        strncpy(entry.description, database_view_.entries[i].description, 31);
        db.add_entry(entry);
    }
    
    // Save to file
    if (!db.save_to_file()) {
        nav_.display_modal("Error", "Failed to save database");
    }
    
    db.unlock();
}
```

---

## 9. Risk Assessment

### 9.1 Technical Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Thread deadlock | Medium | High | Strict lock ordering, timeout on locks |
| Memory overflow | Low | High | Compile-time size checks, static_assert |
| File format incompatibility | Medium | Medium | Support both formats, auto-detect |
| Observer callback crash | Low | High | Validate callback before invoke |

### 9.2 Mitigation Strategies

```cpp
// Compile-time size verification
static_assert(sizeof(UnifiedDroneEntry) == 48, "Entry size mismatch");
static_assert(DatabaseConfig::DATABASE_SIZE <= 8192, "Database too large");

// Runtime lock timeout
bool UnifiedDroneDatabase::try_lock(uint32_t timeout_ms) noexcept {
    return chMtxLockTimeout(&mutex_, MS2ST(timeout_ms)) == RDY_OK;
}

// Safe callback invocation
void UnifiedDroneDatabase::notify_observers(const DatabaseChangeEvent& event) noexcept {
    for (auto& observer : observers_) {
        if (observer.active && observer.callback != nullptr) {
            // Use try-catch equivalent for embedded (watchdog)
            observer.callback(event, observer.user_data);
        }
    }
}
```

---

## 10. Summary

This architecture provides:

1. **Single Source of Truth**: UnifiedDroneDatabase singleton
2. **Real-Time Sync**: Observer pattern with zero heap allocation
3. **Unified Entry Limits**: 120 entries (memory-optimized)
4. **Frequency Validation**: Integrated validation in UI and database

### Memory Savings
- **Before**: ~10.5 KB (FreqmanDB + DatabaseView)
- **After**: ~5.9 KB (UnifiedDroneDatabase)
- **Savings**: ~4.6 KB

### Thread Safety
- Mutex-protected database access
- Lock ordering prevents deadlocks
- Observer callbacks are thread-safe

### Compatibility
- Supports both freqman and CSV formats
- Backward-compatible with existing DRONES.TXT files
- No breaking changes to external APIs