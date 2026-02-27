PART 2: ARCHITECT'S BLUEPRINT
========================================

FIX PRIORITY CLASSIFICATION:
============================

CRITICAL FIXES (Must fix - safety/memory violations):
-----------------------------------------------------
1. Create eda_locking.hpp - Missing header file causing compilation errors
2. Replace OrderedScopedLock with MutexLock in ui_drone_audio.hpp - API inconsistency
3. Remove OrderedScopedLock reference from ui_enhanced_drone_analyzer.hpp - API inconsistency
4. Fix std::string description_buffer_ in ui_enhanced_drone_settings.hpp:461 - Heap allocation
5. Create eda_stl_compat.hpp - Missing header file mentioned in audit

HIGH PRIORITY FIXES (Should fix - race conditions, API inconsistencies):
------------------------------------------------------------------------
6. Fix potential race condition in ui_enhanced_drone_analyzer.cpp:2897 - Semaphore signaling
7. Ensure mutex protection for volatile variables - Missing mutex protection
8. Fix double-read of volatile variables without version check - TOCTOU vulnerability
9. Fix non-atomic compound operations on volatile variables - Race condition
10. Fix potential TOCTOU in histogram update - Double memory access pattern

MEDIUM PRIORITY FIXES (Consider fixing - code quality, maintainability):
------------------------------------------------------------------------
11. Replace placement new with static std::array in ui_enhanced_drone_analyzer.cpp - Code clarity
12. Replace reinterpret_cast pattern with static storage - Code clarity
13. Separate UI from DSP in DroneDisplayController - Mixed logic concerns
14. Separate UI from hardware control in DroneHardwareController - Mixed logic concerns

LOW PRIORITY FIXES (Optional - magic numbers, minor issues):
-------------------------------------------------------------
15. Extract magic numbers to named constants (30 occurrences) - Code maintainability
16. Add const/constexpr to constant data - Flash optimization

FRAMEWORK-LEVEL ISSUES (Document but don't fix):
-------------------------------------------------
F1. std::string title() methods - Requires UI framework changes (8 occurrences)
F2. std::function callbacks - Requires UI framework changes (1 occurrence)
F3. std::string throughout ui_enhanced_drone_analyzer.cpp - Requires comprehensive refactoring

DETAILED DESIGN FOR EACH FIX:
===========================

FIX #1: Create eda_locking.hpp
------------------------------
Problem Statement:
- Missing header file causing compilation errors
- Files reference OrderedScopedLock which doesn't exist
- Inconsistent mutex API across codebase

Proposed Solution:
Create a unified locking header that provides:
- Simple MutexLock RAII wrapper (already in ui_signal_processing.hpp)
- Remove OrderedScopedLock entirely (unsafe for embedded systems)
- Define lock ordering constants for documentation

Data Structures:
```cpp
// Simple RAII Mutex Lock (ChibiOS wrapper)
class MutexLock {
public:
    explicit MutexLock(Mutex& mtx) noexcept;
    ~MutexLock() noexcept;
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
private:
    Mutex& mtx_;
    bool locked_;
};

// Lock ordering constants (for documentation only)
enum class LockOrder : uint8_t {
    SD_CARD = 1,
    AUDIO = 2,
    DATABASE = 3,
    DETECTION_BUFFER = 4,
    TRACKED_DRONES = 5,
    UI_STATE = 6,
    SETTINGS = 7
};
```

Memory Placement:
- Header-only file (no code in object file)
- All functions inline constexpr where possible

Function Signatures and RAII Wrappers:
```cpp
// Constructor
explicit MutexLock(Mutex& mtx) noexcept : mtx_(mtx), locked_(false) {
    chMtxLock(&mtx_);
    locked_ = true;
}

// Destructor
~MutexLock() noexcept {
    if (locked_) {
        chMtxUnlock();
    }
}
```

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.hpp, ui_drone_audio.hpp
- Files to create: eda_locking.hpp
- Breaking changes: OrderedScopedLock usage must be replaced with MutexLock
- Risk: Low - simple mechanical replacement

---

FIX #2: Replace OrderedScopedLock with MutexLock in ui_drone_audio.hpp
------------------------------------------------------------------------
Problem Statement:
- ui_drone_audio.hpp uses OrderedScopedLock which doesn't exist
- Inconsistent with MutexLock used in ui_signal_processing.hpp
- Potential deadlock risk with complex lock ordering

Proposed Solution:
Replace all OrderedScopedLock<Mutex> instances with MutexLock

Data Structures:
- No new data structures needed
- Use existing MutexLock from eda_locking.hpp

Memory Placement:
- No memory impact (header-only)

Function Signatures and RAII Wrappers:
```cpp
// Before:
OrderedScopedLock<Mutex> lock(get_mutex());

// After:
MutexLock lock(get_mutex());
```

Impact Analysis:
- Files to modify: ui_drone_audio.hpp
- Lines to change: 52, 74, 80, 93
- Breaking changes: None (same semantics)
- Risk: Low - simple mechanical replacement

---

FIX #3: Remove OrderedScopedLock reference from ui_enhanced_drone_analyzer.hpp
------------------------------------------------------------------------------
Problem Statement:
- Comment at line 58 references OrderedScopedLock which doesn't exist
- Misleading documentation
- Inconsistent with actual code using MutexLock

Proposed Solution:
Update comment to reference MutexLock instead

Data Structures:
- No new data structures needed

Memory Placement:
- No memory impact (comment only)

Function Signatures and RAII Wrappers:
- No code changes, comment only

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.hpp
- Lines to change: 58-59
- Breaking changes: None (comment only)
- Risk: None

---

FIX #4: Fix std::string description_buffer_ in ui_enhanced_drone_settings.hpp:461
---------------------------------------------------------------------------------
Problem Statement:
- std::string description_buffer_ member causes heap allocation
- Violates constraint: FORBIDDEN: std::string, malloc
- Comment at line 453-461 documents migration path

Proposed Solution:
Replace std::string with fixed-size char array

Data Structures:
```cpp
// Before:
std::string description_buffer_;

// After:
char description_buffer_[64];
```

Memory Placement:
- RAM: 64 bytes (down from heap allocation)
- No Flash impact

Function Signatures and RAII Wrappers:
```cpp
// Update constructor to initialize buffer:
EditFrequencyView(NavigationView& nav, DroneDbEntry entry, Callback on_save_fn)
    : nav_(nav),
      entry_(std::move(entry)),
      on_save_fn_(std::move(on_save_fn)),
      field_desc_{description_buffer_, sizeof(description_buffer_)} {
    // Initialize buffer with existing description
    safe_strcpy(description_buffer_, entry_.description, sizeof(description_buffer_));
}

// Update on_save() to use buffer:
void on_save() {
    DroneDbEntry new_entry = entry_;
    safe_strcpy(new_entry.description, description_buffer_, sizeof(new_entry.description));
    // ... rest of save logic
}
```

Impact Analysis:
- Files to modify: ui_enhanced_drone_settings.hpp, ui_enhanced_drone_settings.cpp
- Breaking changes: Requires TextEdit widget to support set_buffer() method
- Risk: Medium - depends on TextEdit widget API
- Note: Comment at lines 453-461 already documents this migration path

---

FIX #5: Create eda_stl_compat.hpp
---------------------------------
Problem Statement:
- Missing header file mentioned in Stage 1 audit
- May contain STL compatibility utilities for embedded systems

Proposed Solution:
Create a compatibility header with safe STL alternatives

Data Structures:
```cpp
// Safe string utilities (no heap allocation)
namespace eda {
    // Safe string copy with bounds checking
    inline void safe_strcpy(char* dest, const char* src, size_t dest_size) noexcept {
        if (dest_size == 0) return;
        size_t i = 0;
        while (i < dest_size - 1 && src[i] != '\0') {
            dest[i] = src[i];
            i++;
        }
        dest[i] = '\0';
    }

    // Safe string concatenation with bounds checking
    inline void safe_strcat(char* dest, const char* src, size_t dest_size) noexcept {
        size_t dest_len = 0;
        while (dest_len < dest_size && dest[dest_len] != '\0') {
            dest_len++;
        }
        if (dest_len >= dest_size) return;
        safe_strcpy(dest + dest_len, src, dest_size - dest_len);
    }

    // String length with bounds checking
    inline size_t safe_strlen(const char* str, size_t max_len) noexcept {
        size_t len = 0;
        while (len < max_len && str[len] != '\0') {
            len++;
        }
        return len;
    }
}
```

Memory Placement:
- Header-only file (no code in object file)
- All functions inline constexpr

Function Signatures and RAII Wrappers:
- All functions inline noexcept

Impact Analysis:
- Files to create: eda_stl_compat.hpp
- Breaking changes: None (new header)
- Risk: Low - simple utility functions

---

FIX #6: Fix potential race condition in ui_enhanced_drone_analyzer.cpp:2897
---------------------------------------------------------------------------
Problem Statement:
- Semaphore signaling without proper synchronization
- Potential missed wakeups or spurious wakeups
- Line 2897 in ui_enhanced_drone_analyzer.cpp

Proposed Solution:
Ensure proper semaphore signaling with mutex protection

Data Structures:
- No new data structures needed

Memory Placement:
- No memory impact

Function Signatures and RAII Wrappers:
```cpp
// Before (hypothetical):
chSemSignal(&semaphore_);

// After - ensure proper ordering:
{
    MutexLock lock(mutex_);
    // Update shared state
    chSemSignal(&semaphore_);
}
```

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.cpp
- Lines to change: ~2897 (needs verification)
- Breaking changes: None (same semantics, safer)
- Risk: Low - adds safety, doesn't change behavior

---

FIX #7: Ensure mutex protection for volatile variables
-------------------------------------------------------
Problem Statement:
- Volatile variables accessed without mutex protection
- Missing mutex protection for volatile variables
- Lines 631, 636, 639, 642, 650 in ui_enhanced_drone_analyzer.hpp

Proposed Solution:
Add mutex protection for all volatile variable access

Data Structures:
- No new data structures needed

Memory Placement:
- No memory impact

Function Signatures and RAII Wrappers:
```cpp
// Example for scan_cycles_ (line 650):
// Before:
volatile uint32_t scan_cycles_{0};

// After:
mutable Mutex scan_cycles_mutex_;
volatile uint32_t scan_cycles_{0};

// Access pattern:
uint32_t get_scan_cycles() const noexcept {
    MutexLock lock(scan_cycles_mutex_);
    return scan_cycles_;
}

void increment_scan_cycles() noexcept {
    MutexLock lock(scan_cycles_mutex_);
    scan_cycles_++;
}
```

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.hpp, ui_enhanced_drone_analyzer.cpp
- Breaking changes: None (adds safety)
- Risk: Low - adds safety, doesn't change behavior
- Note: May impact performance slightly due to mutex overhead

---

FIX #8: Fix double-read of volatile variables without version check
--------------------------------------------------------------------
Problem Statement:
- Double read of volatile variable without version check
- Potential TOCTOU (Time-Of-Check-Time-Of-Use) vulnerability
- Double memory access pattern #1 in audit

Proposed Solution:
Use versioned data structures or single read pattern

Data Structures:
```cpp
// Versioned data structure for safe reads
template<typename T, size_t SIZE>
struct VersionedBuffer {
    T data[SIZE];
    volatile uint32_t version;

    // Writer updates version after data
    void write(const T* new_data, size_t count) noexcept {
        std::copy(new_data, new_data + count, data);
        version++;  // Release semantics
    }

    // Reader checks version before and after
    bool read(T* out_data, size_t count) const noexcept {
        uint32_t v1 = version;  // Acquire semantics
        std::copy(data, data + count, out_data);
        uint32_t v2 = version;
        return v1 == v2;  // Consistent read if versions match
    }
};
```

Memory Placement:
- RAM: sizeof(T) * SIZE + sizeof(uint32_t)
- Flash: None

Function Signatures and RAII Wrappers:
- Template class with inline methods

Impact Analysis:
- Files to modify: Multiple files with volatile access patterns
- Breaking changes: Requires refactoring of volatile access patterns
- Risk: Medium - requires careful analysis of all volatile access
- Note: ChibiOS provides atomic operations that may be simpler

---

FIX #9: Fix non-atomic compound operations on volatile variables
-----------------------------------------------------------------
Problem Statement:
- Non-atomic compound operations on volatile variables
- Double memory access pattern #3 in audit
- Example: scan_cycles_++ is not atomic

Proposed Solution:
Use ChibiOS atomic operations or mutex protection

Data Structures:
- No new data structures needed

Memory Placement:
- No memory impact

Function Signatures and RAII Wrappers:
```cpp
// Option 1: ChibiOS atomic operations
#include <chatomic.h>

// Before:
scan_cycles_++;

// After:
chAtomicInc(&scan_cycles_);

// Option 2: Mutex protection
void increment_scan_cycles() noexcept {
    MutexLock lock(scan_cycles_mutex_);
    scan_cycles_++;
}
```

Impact Analysis:
- Files to modify: Multiple files with volatile compound operations
- Breaking changes: None (same semantics, safer)
- Risk: Low - ChibiOS atomic operations are well-tested
- Note: ChibiOS atomic operations preferred over mutex for simple counters

---

FIX #10: Fix potential TOCTOU in histogram update
--------------------------------------------------
Problem Statement:
- Potential TOCTOU in histogram update
- Double memory access pattern #4 in audit
- Histogram update may have race conditions

Proposed Solution:
Ensure atomic histogram updates with proper locking

Data Structures:
- No new data structures needed

Memory Placement:
- No memory impact

Function Signatures and RAII Wrappers:
```cpp
// Before (hypothetical):
if (histogram[index] < max_value) {
    histogram[index]++;
}

// After:
{
    MutexLock lock(histogram_mutex_);
    if (histogram[index] < max_value) {
        histogram[index]++;
    }
}
```

Impact Analysis:
- Files to modify: ui_signal_processing.hpp, ui_signal_processing.cpp
- Breaking changes: None (same semantics, safer)
- Risk: Low - adds safety, doesn't change behavior

---

FIX #11: Replace placement new with static std::array
------------------------------------------------------
Problem Statement:
- Placement new usage violates "no dynamic memory" constraint
- Line 1205 in ui_enhanced_drone_analyzer.cpp
- Lines 610-611 in ui_enhanced_drone_analyzer.hpp

Proposed Solution:
Replace placement new with static std::array member

Data Structures:
```cpp
// Before:
alignas(alignof(TrackedDrone))
static uint8_t tracked_drones_storage_[TRACKED_DRONES_STORAGE_SIZE];
std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>* tracked_drones_ptr_;

// After:
std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES> tracked_drones_;
```

Memory Placement:
- RAM: Same size (~800 bytes)
- Flash: None

Function Signatures and RAII Wrappers:
```cpp
// Remove placement new code:
// tracked_drones_ptr_ = new (tracked_drones_storage_)
//     std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>();

// Use direct member access:
tracked_drones_[index] = ...;
```

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.hpp, ui_enhanced_drone_analyzer.cpp
- Breaking changes: Requires updating all tracked_drones_ptr_ access
- Risk: Medium - requires careful refactoring
- Note: Simplifies code significantly

---

FIX #12: Replace reinterpret_cast pattern with static storage
--------------------------------------------------------------
Problem Statement:
- reinterpret_cast on aligned buffer (line 1181)
- Placement new alternative pattern
- Violates "no dynamic memory" constraint

Proposed Solution:
Replace reinterpret_cast with static member

Data Structures:
```cpp
// Before:
static uint8_t freq_db_storage_[FREQ_DB_STORAGE_SIZE];
FreqmanDB* freq_db_ptr_;

// After:
FreqmanDB freq_db_;
```

Memory Placement:
- RAM: Same size (~8KB for FreqmanDB)
- Flash: None

Function Signatures and RAII Wrappers:
```cpp
// Remove reinterpret_cast code:
// freq_db_ptr_ = reinterpret_cast<FreqmanDB*>(freq_db_storage_);

// Use direct member access:
freq_db_.load(...);
```

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.hpp, ui_enhanced_drone_analyzer.cpp
- Breaking changes: Requires updating all freq_db_ptr_ access
- Risk: Medium - requires careful refactoring
- Note: Simplifies code significantly

---

FIX #13: Separate UI from DSP in DroneDisplayController
--------------------------------------------------------
Problem Statement:
- UI mixed with DSP in DroneDisplayController class
- Mixed logic issue #1 in audit
- Violates separation of concerns principle

Proposed Solution:
Extract DSP logic into separate DroneSignalProcessor class

Data Structures:
```cpp
// New DSP-only class
class DroneSignalProcessor {
public:
    struct ProcessedSignal {
        Frequency frequency;
        int32_t rssi;
        ThreatLevel threat;
        uint32_t confidence;
    };

    ProcessedSignal process_signal(const complex16_t* samples, size_t count) noexcept;
    void update_threshold(int32_t new_threshold) noexcept;
    int32_t get_noise_floor() const noexcept;

private:
    int32_t noise_floor_{EDA::Constants::NOISE_FLOOR_RSSI};
    int32_t threshold_{EDA::Constants::DEFAULT_RSSI_THRESHOLD_DB};
    // ... DSP-specific state
};

// UI-only controller
class DroneDisplayController {
public:
    void update_display(const DroneSignalProcessor::ProcessedSignal& signal) noexcept;
    void set_display_mode(DisplayMode mode) noexcept;

private:
    DroneSignalProcessor processor_;
    // ... UI-specific state
};
```

Memory Placement:
- RAM: Split between DSP and UI classes
- Flash: None

Function Signatures and RAII Wrappers:
- Clear separation of concerns

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.hpp, ui_enhanced_drone_analyzer.cpp
- Breaking changes: Requires refactoring of DroneDisplayController
- Risk: High - major architectural change
- Note: Significant refactoring effort

---

FIX #14: Separate UI from hardware control in DroneHardwareController
----------------------------------------------------------------------
Problem Statement:
- UI mixed with hardware control in DroneHardwareController
- Mixed logic issue #3 in audit
- Violates separation of concerns principle

Proposed Solution:
Extract hardware control into separate DroneRadioController class

Data Structures:
```cpp
// New hardware-only class
class DroneRadioController {
public:
    bool set_frequency(Frequency freq) noexcept;
    bool start_rx() noexcept;
    bool stop_rx() noexcept;
    Frequency get_current_frequency() const noexcept;

private:
    rf::Frequency current_freq_{0};
    bool rx_active_{false};
    // ... hardware-specific state
};

// UI-only controller
class DroneHardwareController {
public:
    void on_frequency_changed(Frequency freq) noexcept;
    void on_scan_start() noexcept;
    void on_scan_stop() noexcept;

private:
    DroneRadioController radio_;
    // ... UI-specific state
};
```

Memory Placement:
- RAM: Split between hardware and UI classes
- Flash: None

Function Signatures and RAII Wrappers:
- Clear separation of concerns

Impact Analysis:
- Files to modify: ui_enhanced_drone_analyzer.hpp, ui_enhanced_drone_analyzer.cpp
- Breaking changes: Requires refactoring of DroneHardwareController
- Risk: High - major architectural change
- Note: Significant refactoring effort

---

FIX #15: Extract magic numbers to named constants
-------------------------------------------------
Problem Statement:
- 30 occurrences of magic numbers in code
- Reduces code maintainability
- Makes code harder to understand

Proposed Solution:
Extract magic numbers to named constants in eda_constants.hpp

Data Structures:
```cpp
namespace MagicNumberConstants {
    // Display constants
    constexpr size_t DISPLAY_WIDTH = 240;
    constexpr size_t DISPLAY_HEIGHT = 320;
    constexpr size_t MARGIN = 4;
    constexpr size_t PADDING = 8;

    // Timing constants
    constexpr uint32_t DEBOUNCE_MS = 50;
    constexpr uint32_t LONG_PRESS_MS = 1000;
    constexpr uint32_t BLINK_INTERVAL_MS = 500;

    // Buffer sizes
    constexpr size_t TEMP_BUFFER_SIZE = 64;
    constexpr size_t ERROR_BUFFER_SIZE = 128;
    constexpr size_t PATH_BUFFER_SIZE = 256;

    // Thresholds
    constexpr int32_t MIN_RSSI_FOR_DISPLAY = -100;
    constexpr uint32_t MIN_SAMPLES_FOR_VALID = 10;
}
```

Memory Placement:
- Flash: All constants (EDA_FLASH_CONST)
- RAM: None

Function Signatures and RAII Wrappers:
- No functions, just constants

Impact Analysis:
- Files to modify: Multiple .cpp and .hpp files
- Breaking changes: None (semantic equivalent)
- Risk: Low - mechanical replacement
- Note: Requires identifying all magic numbers first

---

FIX #16: Add const/constexpr to constant data
---------------------------------------------
Problem Statement:
- Constant data not marked as const/constexpr
- Misses Flash optimization opportunity
- Wastes RAM

Proposed Solution:
Add EDA_FLASH_CONST and constexpr to constant data

Data Structures:
```cpp
// Before:
static const char* DRONE_NAMES[] = { "DJI", "Parrot", "Skydio" };

// After:
EDA_FLASH_CONST inline static constexpr const char* DRONE_NAMES[] = {
    "DJI", "Parrot", "Skydio"
};
```

Memory Placement:
- Flash: All constant data (EDA_FLASH_CONST)
- RAM: None

Function Signatures and RAII Wrappers:
- No functions, just attributes

Impact Analysis:
- Files to modify: Multiple .hpp files
- Breaking changes: None (semantic equivalent)
- Risk: Low - adds attributes only
- Note: Saves RAM by moving constants to Flash

---

SPECIFIC DESIGN DECISIONS:
==========================

1. MUTEX API UNIFICATION:
--------------------------
Design a unified mutex wrapper:

```cpp
// File: eda_locking.hpp
#ifndef EDA_LOCKING_HPP_
#define EDA_LOCKING_HPP_

#include <ch.h>

namespace ui::apps::enhanced_drone_analyzer {

// Simple RAII Mutex Lock (ChibiOS wrapper)
class MutexLock {
public:
    explicit MutexLock(Mutex& mtx) noexcept : mtx_(mtx), locked_(false) {
        chMtxLock(&mtx_);
        locked_ = true;
    }

    ~MutexLock() noexcept {
        if (locked_) {
            chMtxUnlock();
        }
    }

    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;

private:
    Mutex& mtx_;
    bool locked_;
};

// Lock ordering constants (for documentation only)
// Always acquire locks in ascending order (1 < 2 < 3 < 4 < 5 < 6 < 7)
// Never acquire a lower-numbered lock while holding a higher-numbered lock
enum class LockOrder : uint8_t {
    SD_CARD = 1,          // FatFS protection
    AUDIO = 2,            // Audio alert state
    DATABASE = 3,         // Drone database access
    DETECTION_BUFFER = 4, // Detection ring buffer
    TRACKED_DRONES = 5,   // Tracked drone list
    UI_STATE = 6,         // UI widget state
    SETTINGS = 7          // Settings persistence
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_LOCKING_HPP_
```

Define lock ordering constants:
- SD_CARD = 1 (lowest priority, acquired first)
- AUDIO = 2
- DATABASE = 3
- DETECTION_BUFFER = 4
- TRACKED_DRONES = 5
- UI_STATE = 6
- SETTINGS = 7 (highest priority, acquired last)

Handle OrderedScopedLock vs MutexLock inconsistency:
- Remove all OrderedScopedLock references
- Replace with MutexLock
- Update documentation comments

2. VOLATILE VARIABLE ATOMICITY:
-------------------------------
Design for atomic compound operations:

```cpp
// Option 1: ChibiOS atomic operations (preferred for simple counters)
#include <chatomic.h>

class AtomicCounter {
public:
    void increment() noexcept {
        chAtomicInc(&value_);
    }

    void decrement() noexcept {
        chAtomicDec(&value_);
    }

    uint32_t get() const noexcept {
        return chAtomicRead(&value_);
    }

    void set(uint32_t new_value) noexcept {
        chAtomicWrite(&value_, new_value);
    }

private:
    atomic_flag_t value_;
};

// Option 2: Mutex protection (for complex compound operations)
class MutexProtectedCounter {
public:
    void increment() noexcept {
        MutexLock lock(mutex_);
        value_++;
    }

    uint32_t get() const noexcept {
        MutexLock lock(mutex_);
        return value_;
    }

private:
    mutable Mutex mutex_;
    uint32_t value_{0};
};
```

Use ChibiOS atomic operations or proper locking:
- Use chAtomicInc/Dec/Read/Write for simple counters
- Use mutex protection for complex compound operations
- Never rely on volatile for atomicity

3. DOUBLE MEMORY ACCESS PREVENTION:
-----------------------------------
Design versioned data structures:

```cpp
// Versioned buffer for safe concurrent reads
template<typename T, size_t SIZE>
class VersionedBuffer {
public:
    // Writer updates version after data (release semantics)
    void write(const T* new_data, size_t count) noexcept {
        std::copy(new_data, new_data + count, data_);
        version_++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }

    // Reader checks version before and after
    bool read(T* out_data, size_t count) const noexcept {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        uint32_t v1 = version_;
        std::copy(data_, data_ + count, out_data);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        uint32_t v2 = version_;
        return v1 == v2;  // Consistent read if versions match
    }

private:
    T data_[SIZE];
    volatile uint32_t version_{0};
};
```

Use memory barriers where needed:
- Use __atomic_thread_fence for acquire/release semantics
- Use ChibiOS chSysLock() for critical sections
- Never double-read volatile variables without version check

4. SEPARATION OF CONCERNS:
--------------------------
Design separation between UI, DSP, and hardware control:

```cpp
// Layer 1: Hardware Abstraction (no UI, no business logic)
class DroneRadioHardware {
public:
    bool set_frequency(Frequency freq) noexcept;
    bool start_rx() noexcept;
    bool stop_rx() noexcept;
    Frequency get_current_frequency() const noexcept;
    int32_t get_rssi() const noexcept;
};

// Layer 2: Signal Processing (no UI, no hardware control)
class DroneSignalProcessor {
public:
    struct ProcessedSignal {
        Frequency frequency;
        int32_t rssi;
        ThreatLevel threat;
        uint32_t confidence;
    };

    ProcessedSignal process_signal(const complex16_t* samples, size_t count) noexcept;
    void update_threshold(int32_t new_threshold) noexcept;
    int32_t get_noise_floor() const noexcept;
};

// Layer 3: Business Logic (no UI, no hardware, no DSP)
class DroneDetectionEngine {
public:
    void process_detection(const DroneSignalProcessor::ProcessedSignal& signal) noexcept;
    std::vector<TrackedDrone> get_active_drones() const noexcept;
};

// Layer 4: UI Controller (no hardware, no DSP, no business logic)
class DroneDisplayController {
public:
    void update_display(const std::vector<TrackedDrone>& drones) noexcept;
    void set_display_mode(DisplayMode mode) noexcept;
};
```

Define clear interfaces:
- Hardware layer: Radio control only
- DSP layer: Signal processing only
- Business logic layer: Detection rules only
- UI layer: Display only

---

FRAMEWORK-LEVEL ISSUES (Document but don't fix):
=================================================

F1. std::string title() methods:
--------------------------------
Problem:
- 8 title() methods return std::string causing heap allocation
- Lines: 232, 262, 291, 320, 350, 415, 497 in ui_enhanced_drone_settings.hpp
- Line: 1534 in ui_enhanced_drone_analyzer.hpp
- All have comments: "Framework-level change required to use std::string_view"

Why cannot fix in this scope:
- Requires UI framework changes (ui_widget.hpp)
- May require changes to NavigationView and other UI components
- Affects multiple apps beyond enhanced_drone_analyzer
- Requires coordination with framework maintainers

Migration path (when framework is ready):
1. Change title() return type from std::string to std::string_view
2. Update all NavigationView implementations
3. Update all Widget subclasses
4. Remove heap allocation from title() methods

F2. std::function callbacks:
-----------------------------
Problem:
- std::function may allocate on heap
- Line 1741 in ui_enhanced_drone_analyzer.hpp
- Comment: "std::function may allocate on heap, but this is unavoidable with the Mayhem UI framework's callback design"

Why cannot fix in this scope:
- Requires UI framework changes
- May require changes to Button, TextEdit, and other widgets
- Affects multiple apps beyond enhanced_drone_analyzer
- Requires coordination with framework maintainers

Migration path (when framework is ready):
1. Replace std::function with template-based callbacks
2. Update all widget callback interfaces
3. Use function pointers with user_data parameter
4. Remove heap allocation from callback storage

F3. std::string throughout ui_enhanced_drone_analyzer.cpp:
-------------------------------------------------------------
Problem:
- Multiple std::string instances throughout ui_enhanced_drone_analyzer.cpp
- Comprehensive refactoring required
- Many are related to UI framework integration

Why cannot fix in this scope:
- Requires comprehensive refactoring of entire file
- Many instances are framework-related
- Requires coordination with framework maintainers
- Risk of introducing bugs is high

Migration path (when framework is ready):
1. Audit all std::string usage in the file
2. Replace with char arrays or std::string_view where possible
3. Update all string handling code
4. Test thoroughly for buffer overflows

---

SUMMARY:
========

Total Fixes: 16
- Critical: 5
- High Priority: 5
- Medium Priority: 4
- Low Priority: 2

Framework-Level Issues: 3
- Cannot fix in this scope
- Documented for future reference

Key Design Principles:
1. Zero heap allocation (no std::string, std::function, malloc, new)
2. RAII for resource management
3. noexcept for all methods where possible
4. Thread-safe with proper mutex protection
5. Separation of concerns (UI vs DSP vs Hardware)
6. Flash optimization with EDA_FLASH_CONST
7. Use ChibiOS primitives (atomic operations, mutexes, semaphores)

Memory Impact:
- RAM: Minimal (mostly reorganization)
- Flash: Savings from EDA_FLASH_CONST on constants

Risk Assessment:
- Low risk: Fixes 1-10, 15-16
- Medium risk: Fixes 11-12
- High risk: Fixes 13-14 (architectural changes)

Recommended Implementation Order:
1. Fix #1 (Create eda_locking.hpp) - Foundation for other fixes
2. Fix #2-3 (Replace OrderedScopedLock) - Unify mutex API
3. Fix #4-5 (Fix std::string issues) - Remove heap allocations
4. Fix #6-10 (Fix race conditions) - Thread safety
5. Fix #11-12 (Replace placement new) - Code clarity
6. Fix #15-16 (Extract constants) - Code maintainability
7. Fix #13-14 (Separation of concerns) - Architecture (optional, defer if time constrained)
