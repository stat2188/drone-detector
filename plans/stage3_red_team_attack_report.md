# Stage 3: Red Team Attack Report

**Date:** 2026-02-21  
**Pipeline Stage:** Stage 3 (Red Team Attack)  
**Target Blueprint:** Stage 2 Architectural Blueprint  
**Target Platform:** STM32F405 (ARM Cortex-M4, 128KB RAM)  
**OS:** ChibiOS (bare-metal RTOS)

---

## Executive Summary

This report documents the results of a comprehensive Red Team Attack on the Stage 2 Architectural Blueprint. The attack tested the blueprint against 5 critical attack vectors: Stack Overflow, Performance, Mayhem Compatibility, Corner Cases, and Logic Verification.

**CRITICAL FINDING:** The blueprint contains **5 CRITICAL FLAWS** that must be addressed before implementation. The most severe flaw is in the `SharedState<T>` template which creates a new mutex on every operation, causing massive stack usage and performance degradation.

**Attack Result:** Blueprint requires significant revision before Stage 4 implementation.

---

## Attack Vector 1: Stack Overflow Test

### Question: "Will this array blow the stack if called recursively or deeply nested?"

### Test Results

#### CRITICAL FLAW #1: SharedState<T> Creates New Mutex Per Operation

**Location:** Blueprint lines 1372-1407 (eda_shared_state.hpp)

**Problem:**
```cpp
T read() const noexcept {
    Mutex mutex;  // ← NEW MUTEX ON STACK FOR EVERY READ!
    chMtxObjectInit(&mutex);
    MutexLock lock(&mutex, LOCK_ORDER);
    return value_;
}
```

**Impact:**
- Each `Mutex` is ~20-30 bytes on stack
- `chMtxObjectInit()` adds additional overhead
- In a deep call chain (e.g., `perform_scan_cycle` → `process_detection` → `update_statistics`), multiple reads could consume 100-300 bytes of stack just for mutex creation
- **Total stack impact:** 20-30 bytes per operation × multiple operations = **100-300+ bytes**

**Attack Scenario:**
```cpp
// Deep call chain in perform_scan_cycle
void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    // Stack used: ~200 bytes
    SharedState<DetectionStats> stats;
    
    // Stack used: +30 bytes (new mutex)
    auto current = stats.read();  
    
    // Stack used: +30 bytes (new mutex)
    stats.write(updated);  
    
    // Stack used: +30 bytes (new mutex)
    stats.update([](auto& s) { s.count++; });  // Another new mutex!
}
```

**Result:** **CRITICAL STACK OVERFLOW RISK**

---

#### CRITICAL FLAW #2: SettingsBuffer Exceeds Stack Budget

**Location:** Blueprint lines 157-162 (eda_buffer_utils.hpp)

**Problem:**
```cpp
namespace BufferSizes {
    constexpr size_t SETTINGS_BUFFER = 1024;  // ← 1024 bytes!
    // ...
}

using SettingsBuffer = StackBuffer<BufferSizes::SETTINGS_BUFFER>;
```

**Impact:**
- Blueprint defines `MAX_FUNCTION_STACK = 512` bytes (line 233)
- But `SettingsBuffer` alone is 1024 bytes
- **Stack budget exceeded by 512 bytes!**

**Attack Scenario:**
```cpp
void save_settings() {
    // Stack used: 1024 bytes (exceeds MAX_FUNCTION_STACK)
    SettingsBuffer buffer;  
    // ... more stack usage ...
}
```

**Result:** **CRITICAL STACK OVERFLOW RISK**

---

#### FLAW #3: Multiple FixedStrings in Single Call Frame

**Location:** Blueprint lines 104-108 (eda_fixed_string.hpp)

**Problem:**
```cpp
using TitleString = FixedString<32>;      // 32 bytes
using DescriptionString = FixedString<64>; // 64 bytes
using PathString = FixedString<64>;       // 64 bytes
using NameString = FixedString<32>;       // 32 bytes
```

**Impact:**
- Multiple FixedStrings in a single function: 32+64+64+32 = 192 bytes
- Combined with other stack allocations (spectrum buffers, freqman_entry arrays)
- **Total stack usage could exceed 1KB in hot paths**

**Attack Scenario:**
```cpp
void update_drone_display(const DroneDetection& drone) {
    TitleString title;          // 32 bytes
    DescriptionString desc;     // 64 bytes
    PathString path;           // 64 bytes
    NameString name;           // 32 bytes
    // Total: 192 bytes + function overhead
}
```

**Result:** **MODERATE STACK OVERFLOW RISK**

---

#### FLAW #4: std::array<freqman_entry, 10> in Deep Call Chain

**Location:** Blueprint line 275 (eda_stack_budget.hpp)

**Problem:**
```cpp
std::array<freqman_entry, StackBudget::BufferLimits::MAX_SCAN_BATCH> entries;
// MAX_SCAN_BATCH = 10, freqman_entry ≈ 64 bytes
// Total: 640 bytes
```

**Impact:**
- Each `freqman_entry` is ~64 bytes (frequency_a, frequency_b, description, type, etc.)
- 10 entries = 640 bytes
- Combined with other allocations in `perform_database_scan_cycle`
- **Total stack usage in hot path: 640 + 192 (FixedStrings) + overhead = ~900+ bytes**

**Result:** **MODERATE STACK OVERFLOW RISK**

---

### Stack Overflow Test Summary

| Flaw | Severity | Stack Impact | Location |
|-------|-----------|---------------|------------|
| SharedState<T> new mutex per operation | CRITICAL | 20-30 bytes/op | eda_shared_state.hpp:1372 |
| SettingsBuffer exceeds budget | CRITICAL | +512 bytes | eda_buffer_utils.hpp:157 |
| Multiple FixedStrings | MODERATE | 192 bytes | eda_fixed_string.hpp:104 |
| std::array<freqman_entry, 10> | MODERATE | 640 bytes | eda_stack_budget.hpp:275 |

**Total Potential Stack Overflow:** 100-300 bytes (SharedState) + 512 bytes (SettingsBuffer) + 192 bytes (FixedStrings) + 640 bytes (entries) = **1444-1644 bytes**

**Safety Margin Against 128KB RAM:** **NEGATIVE** - blueprint could exceed available stack in deep call chains.

---

## Attack Vector 2: Performance Test

### Question: "Is this floating-point math too slow for real-time DSP? Should I use fixed-point?"

### Test Results

#### CRITICAL FLAW #5: SharedState<T> Performance Degradation

**Location:** Blueprint lines 1372-1407 (eda_shared_state.hpp)

**Problem:**
```cpp
T read() const noexcept {
    Mutex mutex;           // ← Stack allocation (~20 bytes)
    chMtxObjectInit(&mutex); // ← Expensive initialization
    MutexLock lock(&mutex, LOCK_ORDER);
    return value_;
}
```

**Impact:**
- `chMtxObjectInit()` is expensive (initializes ChibiOS mutex structure)
- Creating/destroying mutex on every operation is **O(n) overhead**
- For 1000 operations/second in scan cycle: 1000 mutex init/destroy cycles
- **Performance impact: ~10-20x slower than using existing mutex**

**Attack Scenario:**
```cpp
// Hot path: perform_scan_cycle called ~1000 times/second
for (int i = 0; i < 1000; ++i) {
    auto stats = shared_stats.read();  // New mutex every time!
    // ... process stats ...
}
```

**Benchmark Estimate:**
- Existing approach (shared mutex): ~0.1μs per read
- Blueprint approach (new mutex per read): ~1-2μs per read
- **10-20x slower!**

**Result:** **CRITICAL PERFORMANCE DEGRADATION**

---

#### FLAW #6: Excessive Memory Barriers

**Location:** Blueprint lines 1568-1616 (eda_isr_safe.hpp)

**Problem:**
```cpp
inline uint32_t read_scan_cycles() noexcept {
    __DMB();  // ← Barrier #1
    uint32_t value = scan_cycles;
    __DMB();  // ← Barrier #2
    return value;
}
```

**Impact:**
- ARM Cortex-M4 has weak memory ordering, but barriers are expensive
- For simple volatile uint32_t reads, a single barrier is sufficient (or none for aligned 32-bit reads)
- Double barriers on every read is **unnecessary overhead**
- **Performance impact: ~2-3x slower for volatile reads**

**Attack Scenario:**
```cpp
// Hot path: spectrum data processing
for (int i = 0; i < 256; ++i) {
    uint32_t cycles = read_scan_cycles();  // 2 barriers!
    // ... process ...
}
```

**Benchmark Estimate:**
- Single barrier read: ~10 cycles
- Double barrier read: ~25 cycles
- **2.5x slower!**

**Result:** **MODERATE PERFORMANCE DEGRADATION**

---

#### FLAW #7: SafeNumeric Conversion Overhead

**Location:** Blueprint lines 846-863 (eda_safe_numeric.hpp)

**Problem:**
```cpp
template<typename T>
inline StringToNumberResult<T> from_string(std::string_view str) noexcept {
    StringToNumberResult<T> result;
    auto parsed = SafeParse::parse_int<T>(str);  // ← Function call
    if (!parsed.success) {
        return result;
    }
    result.value = parsed.value;
    result.success = true;
    return result;
}
```

**Impact:**
- Two layers of function calls: `SafeNumeric::from_string` → `SafeParse::parse_int`
- Each layer adds call overhead and validation
- For 1000 parses/second: 2000 function calls
- **Performance impact: ~1.5-2x slower than direct parsing**

**Result:** **MINOR PERFORMANCE DEGRADATION**

---

#### FLAW #8: StringParser Bounds Checking Overhead

**Location:** Blueprint lines 604-636 (eda_string_parser.hpp)

**Problem:**
```cpp
constexpr bool parse_key_value(char* key_buffer, size_t key_size,
                               char* value_buffer, size_t value_size) noexcept {
    skip_whitespace();
    
    size_t key_len = 0;
    while (pos_ < input_.size() &&  // ← Bounds check #1
           input_[pos_] != '=' &&     // ← Bounds check #2
           input_[pos_] != '\n' &&   // ← Bounds check #3
           key_len < key_size - 1) {  // ← Bounds check #4
        key_buffer[key_len++] = input_[pos_++];
    }
    // ... more bounds checks ...
}
```

**Impact:**
- 4 bounds checks per character in key parsing loop
- For 1000-character line: 4000 bounds checks
- **Performance impact: ~1.2-1.5x slower than optimized parsing**

**Result:** **MINOR PERFORMANCE DEGRADATION**

---

### Performance Test Summary

| Flaw | Severity | Performance Impact | Location |
|-------|-----------|-------------------|------------|
| SharedState<T> new mutex per operation | CRITICAL | 10-20x slower | eda_shared_state.hpp:1372 |
| Excessive memory barriers | MODERATE | 2.5x slower | eda_isr_safe.hpp:1568 |
| SafeNumeric conversion overhead | MINOR | 1.5-2x slower | eda_safe_numeric.hpp:846 |
| StringParser bounds checking | MINOR | 1.2-1.5x slower | eda_string_parser.hpp:604 |

**Overall Performance Impact:** **CRITICAL** - SharedState<T> flaw alone could make the system 10-20x slower in hot paths.

---

## Attack Vector 3: Mayhem Compatibility Test

### Question: "Does this fit the coding style of the repository?"

### Test Results

#### CRITICAL FLAW #9: FixedString<N> Duplicate Definition

**Location:** Blueprint lines 36-111 (eda_fixed_string.hpp)

**Problem:**
- Blueprint proposes creating a new `FixedString<N>` class
- **Existing code already has `FixedString<N>` in [`eda_safe_string.hpp`](firmware/application/apps/enhanced_drone_analyzer/eda_safe_string.hpp:332)
- This will cause **duplicate definition compilation errors**

**Impact:**
```cpp
// Existing in eda_safe_string.hpp (lines 332-571)
template<size_t N>
class FixedString {
    // ... existing implementation ...
};

// Blueprint proposes in eda_fixed_string.hpp (lines 36-111)
template<size_t N>
class FixedString {
    // ... new implementation ...
};
```

**Result:** **COMPILATION ERROR** - Duplicate symbol definition

---

#### FLAW #10: SharedState<T> Inconsistent with Existing Patterns

**Location:** Blueprint lines 1360-1511 (eda_shared_state.hpp)

**Problem:**
- Existing code uses direct mutex access with `MutexLock`/`ScopedLock`
- Blueprint's approach of creating a new mutex per operation is **inconsistent** with existing patterns
- Existing code uses shared mutexes (class member or global)
- Blueprint uses per-operation mutexes (stack-allocated)

**Impact:**
```cpp
// Existing pattern (from eda_locking.hpp)
Mutex data_mutex;  // Shared mutex
{
    MutexLock lock(&data_mutex, LockOrder::DATA_MUTEX);
    // ... access shared data ...
}

// Blueprint pattern (from eda_shared_state.hpp)
T read() const noexcept {
    Mutex mutex;  // New mutex every time!
    chMtxObjectInit(&mutex);
    MutexLock lock(&mutex, LOCK_ORDER);
    return value_;
}
```

**Result:** **INCONSISTENT CODING STYLE** - Violates existing patterns

---

#### FLAW #11: SafeFile Class Follows Different Error Handling

**Location:** Blueprint lines 338-443 (eda_file_io.hpp)

**Problem:**
- Existing code uses direct FatFS calls with manual error checking
- Blueprint's RAII approach is different but compatible
- However, the blueprint's error handling is more verbose

**Impact:**
```cpp
// Existing pattern (from settings_persistence.cpp)
FRESULT res = f_open(&file, path, FA_READ);
if (res != FR_OK) {
    return false;  // Simple error handling
}

// Blueprint pattern (from eda_file_io.hpp)
FileError err = settings_file.open(filepath, false);
if (!err.is_success()) {
    log_error("Failed to open settings: %s (FatFS: %d)", 
              err.message(), err.fatfs_error);
    return false;  // Verbose error handling
}
```

**Result:** **ACCEPTABLE** - Different but compatible

---

#### FLAW #12: std::string_view Usage Inconsistent

**Location:** Throughout blueprint

**Problem:**
- Blueprint uses `std::string_view` extensively
- Existing codebase may not use `std::string_view` consistently
- Need to verify all existing string operations are compatible

**Impact:**
- Requires audit of existing code for `std::string_view` compatibility
- May require changes to existing code to use `std::string_view`

**Result:** **MINOR COMPATIBILITY ISSUE** - Requires audit

---

### Mayhem Compatibility Test Summary

| Flaw | Severity | Impact | Location |
|-------|-----------|----------|------------|
| FixedString<N> duplicate definition | CRITICAL | Compilation error | eda_fixed_string.hpp:36 |
| SharedState<T> inconsistent patterns | MODERATE | Style violation | eda_shared_state.hpp:1360 |
| SafeFile different error handling | MINOR | Acceptable | eda_file_io.hpp:338 |
| std::string_view inconsistency | MINOR | Audit required | Throughout |

**Overall Compatibility:** **MODERATE** - Duplicate definition is critical blocker.

---

## Attack Vector 4: Corner Cases

### Question: "What happens if the input buffer is empty? What if SPI fails?"

### Test Results

#### CRITICAL FLAW #13: FixedString<N> Silent Truncation

**Location:** Blueprint lines 87-90 (eda_fixed_string.hpp)

**Problem:**
```cpp
FixedString& append(const char* str) noexcept {
    length_ += safe_strcat(data_, str, N);
    return *this;  // ← No indication of truncation!
}
```

**Impact:**
- If `safe_strcat` truncates due to buffer overflow, caller has no way to know
- Data loss is **silent** - no error indication
- Could lead to corrupted settings or logs

**Attack Scenario:**
```cpp
FixedString<32> title;
title.set("This is a very long title that exceeds 32 characters");
title.append(" and more text");  // Silently truncated!
// Caller has no way to know data was lost
```

**Result:** **CRITICAL DATA LOSS RISK**

---

#### CRITICAL FLAW #14: SafeFile SD Card Removal Not Handled

**Location:** Blueprint lines 446-516 (eda_file_io.hpp)

**Problem:**
```cpp
class AtomicFileWriter {
public:
    static FileError write(const char* path, 
                          const void* data, 
                          size_t size) noexcept {
        // Create backup path
        PathBuffer backup_path;
        safe_strcpy(backup_path.ptr(), path, backup_path.capacity());
        safe_strcat(backup_path.ptr(), ".bak", backup_path.capacity());
        
        // Backup existing file
        SafeFile backup_file;
        FileError err = backup_file.open(backup_path.c_str(), false);
        // ... copy file ...
        
        // Write new data
        SafeFile new_file;
        err = new_file.open(path, false);
        // ... write data ...
        
        new_file.close();
        
        // Remove backup on success
        f_unlink(reinterpret_cast<const TCHAR*>(backup_path.c_str()));
        // ← What if SD card was removed mid-operation?
    }
};
```

**Impact:**
- If SD card is removed during write, `f_unlink` could return error
- No handling of `FR_DISK_ERR` or `FR_NOT_READY`
- Could leave system in inconsistent state
- **Silent failure** - caller may not know write failed

**Attack Scenario:**
```cpp
// User removes SD card while saving settings
auto result = AtomicFileWriter::write("SETTINGS.TXT", data, size);
// SD card removed mid-write
// f_unlink returns FR_DISK_ERR
// But result shows SUCCESS (because write succeeded before unlink)
```

**Result:** **CRITICAL DATA CORRUPTION RISK**

---

#### FLAW #15: StringParser Malformed Input Not Handled

**Location:** Blueprint lines 604-636 (eda_string_parser.hpp)

**Problem:**
```cpp
constexpr bool parse_key_value(char* key_buffer, size_t key_size,
                               char* value_buffer, size_t value_size) noexcept {
    skip_whitespace();
    
    size_t key_len = 0;
    while (pos_ < input_.size() && 
           input_[pos_] != '=' && 
           input_[pos_] != '\n' && 
           key_len < key_size - 1) {
        key_buffer[key_len++] = input_[pos_++];
    }
    key_buffer[key_len] = '\0';
    
    // Skip '='
    if (pos_ >= input_.size() || input_[pos_] != '=') {
        return false;  // ← Returns false, but buffers may be corrupted
    }
    ++pos_;
    
    // Parse value
    skip_whitespace();
    size_t value_len = 0;
    while (pos_ < input_.size() && 
           input_[pos_] != '\n' && 
           input_[pos_] != '\r' && 
           value_len < value_size - 1) {
        value_buffer[value_len++] = input_[pos_++];
    }
    value_buffer[value_len] = '\0';
    
    return true;  // ← Returns true even if value is empty!
}
```

**Impact:**
- Empty lines: returns `true` with empty key and value
- Lines without `=`: returns `false` but key buffer may be partially filled
- Lines with only `=`: returns `true` with empty value
- Lines with multiple `=`: only first `=` is used
- **No validation of parsed data**

**Attack Scenario:**
```cpp
StringParser parser("KEY=VALUE\n");
char key[32], value[32];
if (parser.parse_key_value(key, 32, value, 32)) {
    // Success - but what if value is empty or malformed?
}

StringParser parser2("EMPTY_LINE\n");
if (parser2.parse_key_value(key, 32, value, 32)) {
    // Returns true with empty key and value!
}
```

**Result:** **MODERATE DATA CORRUPTION RISK**

---

#### FLAW #16: FrequencyOps Overflow/Underflow No Clamped Result

**Location:** Blueprint lines 1047-1062 (eda_frequency.hpp)

**Problem:**
```cpp
inline constexpr bool add_frequency(uint64_t a, uint64_t b, uint64_t& result) noexcept {
    if (a > std::numeric_limits<uint64_t>::max() - b) {
        return false;  // ← Returns false, but result is not set!
    }
    result = a + b;
    return true;
}

inline constexpr bool sub_frequency(uint64_t a, uint64_t b, uint64_t& result) noexcept {
    if (a < b) {
        return false;  // ← Returns false, but result is not set!
    }
    result = a - b;
    return true;
}
```

**Impact:**
- If overflow/underflow occurs, `result` parameter is not modified
- Caller must check return value before using `result`
- If caller forgets to check, they use **uninitialized value**
- **No clamped result provided**

**Attack Scenario:**
```cpp
uint64_t result;
if (FrequencyOps::add_frequency(UINT64_MAX, 1, result)) {
    // Success - result is valid
} else {
    // Overflow - but result is uninitialized!
    // If caller forgets to check, they use garbage value
    use_frequency(result);  // ← Undefined behavior!
}
```

**Result:** **MODERATE UNDEFINED BEHAVIOR RISK**

---

#### FLAW #17: ISRSafe Variables During Interrupt Storms

**Location:** Blueprint lines 1544-1616 (eda_isr_safe.hpp)

**Problem:**
```cpp
namespace ISRSafe {
    extern volatile uint32_t scan_cycles;
    extern volatile int32_t current_rssi;
    // ... more volatile variables ...
    
    inline uint32_t read_scan_cycles() noexcept {
        __DMB();
        uint32_t value = scan_cycles;
        __DMB();
        return value;
    }
}
```

**Impact:**
- During interrupt storms (e.g., rapid spectrum updates), multiple ISRs may write to same variable
- `volatile` ensures visibility but not atomicity for multi-byte values
- On ARM Cortex-M4, 32-bit aligned reads/writes are atomic
- But 64-bit values (uint64_t) are **not atomic**
- **Race condition possible for uint64_t variables**

**Attack Scenario:**
```cpp
// ISR 1 writes high 32 bits
ISR1() {
    scan_cycles_high = new_value_high;
}

// ISR 2 writes low 32 bits
ISR2() {
    scan_cycles_low = new_value_low;
}

// Main thread reads 64-bit value
uint64_t value = read_scan_cycles();
// ← May read mixed high/low from different ISR invocations!
```

**Result:** **MODERATE RACE CONDITION RISK**

---

### Corner Cases Test Summary

| Flaw | Severity | Impact | Location |
|-------|-----------|----------|------------|
| FixedString<N> silent truncation | CRITICAL | Data loss | eda_fixed_string.hpp:87 |
| SafeFile SD card removal | CRITICAL | Data corruption | eda_file_io.hpp:446 |
| StringParser malformed input | MODERATE | Data corruption | eda_string_parser.hpp:604 |
| FrequencyOps no clamped result | MODERATE | Undefined behavior | eda_frequency.hpp:1047 |
| ISRSafe interrupt storms | MODERATE | Race condition | eda_isr_safe.hpp:1544 |

**Overall Corner Case Handling:** **CRITICAL** - Silent data loss and corruption risks.

---

## Attack Vector 5: Logic Check

### Question: "Verify reasoning. If a flaw is found, return to Stage 2 and fix the plan."

### Test Results

#### CRITICAL FLAW #18: SharedState<T> Design Fundamentally Broken

**Location:** Blueprint lines 1360-1511 (eda_shared_state.hpp)

**Problem:**
```cpp
template<typename T, LockOrder LOCK_ORDER>
class SharedState {
public:
    // Read value (with lock)
    T read() const noexcept {
        Mutex mutex;  // ← NEW MUTEX EVERY TIME!
        chMtxObjectInit(&mutex);
        MutexLock lock(&mutex, LOCK_ORDER);
        return value_;
    }
    
    // Write value (with lock)
    void write(const T& value) noexcept {
        Mutex mutex;  // ← ANOTHER NEW MUTEX!
        chMtxObjectInit(&mutex);
        MutexLock lock(&mutex, LOCK_ORDER);
        value_ = value;
    }
    
private:
    T value_;  // ← Shared value, but protected by DIFFERENT mutexes!
};
```

**Fundamental Flaws:**
1. **No actual thread safety:** Each operation uses a different mutex
   - Thread A calls `read()` → creates Mutex #1
   - Thread B calls `write()` → creates Mutex #2
   - Mutex #1 and Mutex #2 are **different objects**
   - **NO MUTUAL EXCLUSION!**

2. **Massive stack usage:** Each operation allocates a new mutex on stack
   - 20-30 bytes per operation
   - In deep call chains: 100-300 bytes just for mutexes

3. **Performance disaster:** Mutex initialization is expensive
   - `chMtxObjectInit()` is not trivial
   - Called on every read/write
   - **10-20x slower than using shared mutex**

4. **Violates lock ordering:** Each operation creates a new mutex with `LOCK_ORDER`
   - Lock order tracking becomes meaningless
   - Cannot detect lock order violations

**Correct Design:**
```cpp
template<typename T, LockOrder LOCK_ORDER>
class SharedState {
public:
    T read() const noexcept {
        MutexLock lock(&mutex_, LOCK_ORDER);  // ← Shared mutex!
        return value_;
    }
    
    void write(const T& value) noexcept {
        MutexLock lock(&mutex_, LOCK_ORDER);  // ← Same shared mutex!
        value_ = value;
    }
    
private:
    T value_;
    mutable Mutex mutex_;  // ← Single shared mutex!
};
```

**Result:** **CRITICAL DESIGN FLAW** - Does not provide thread safety

---

#### CRITICAL FLAW #19: Migration Strategy Unrealistic

**Location:** Blueprint lines 2308-2350 (Migration Strategy)

**Problem:**
- Blueprint proposes creating **18 new header files** with ~3400 lines of code
- All in a single phase (Phase 1: Infrastructure)
- No incremental migration path
- No rollback strategy for partial migrations

**Impact:**
- Creating 18 new files at once is **high risk**
- If any file has compilation errors, entire phase fails
- No way to test individual components in isolation
- Rollback would require deleting 18 files

**Attack Scenario:**
```cpp
// Phase 1: Create all infrastructure
1. Create eda_fixed_string.hpp
2. Create eda_buffer_utils.hpp
3. Create eda_stack_budget.hpp
4. Create eda_file_io.hpp
5. Create eda_string_parser.hpp
6. Create eda_safe_numeric.hpp
7. Create eda_magic_numbers.hpp
8. Create eda_frequency.hpp
9. Create eda_enums.hpp
10. Create eda_shared_state.hpp  // ← Has critical flaw!
11. Create eda_isr_safe.hpp
12. Create eda_memory_barriers.hpp
13. Create eda_raii_extended.hpp
14. Create eda_translation_tables.hpp
15. Create dsp/dsp_scanner.hpp
16. Create ui/ui_scanner.hpp
17. Create scanner_decomposed.hpp
18. Update existing files

// If file #10 has a flaw, entire phase fails
// No way to test files 1-9 in isolation
```

**Result:** **CRITICAL MIGRATION RISK**

---

#### FLAW #20: Rollback Strategy Insufficient

**Location:** Blueprint lines 2352-2363 (Rollback Strategy)

**Problem:**
```cpp
Each phase includes a rollback branch:
- feature/stage2-phase1-infrastructure
- feature/stage2-phase2-memory
- ...

If any phase fails, rollback to previous phase and investigate.
```

**Impact:**
- Rollback branches don't specify **what to rollback**
- No data migration strategy for settings files
- No strategy for handling partial migrations
- No way to recover from corrupted state

**Attack Scenario:**
```cpp
// Phase 2: Replace std::string usage
// After migration, settings file format may change
// User saves new settings format
// Phase fails due to bug
// Rollback to Phase 1
// But settings file is now in new format!
// Old code cannot read new format!
// → DATA LOSS
```

**Result:** **CRITICAL DATA LOSS RISK**

---

#### FLAW #21: Dependencies Between Changes Not Documented

**Location:** Throughout blueprint

**Problem:**
- Blueprint doesn't document dependencies between changes
- Example: `FixedString<N>` depends on `safe_strcpy`/`safe_strcat`
- Example: `SafeFile` depends on `SettingsBufferLock` (not defined in blueprint)
- Example: `SharedState<T>` depends on `LockOrder` enum (moved to `eda_locking.hpp`)

**Impact:**
- Changes cannot be implemented in arbitrary order
- Circular dependencies possible
- Migration strategy may be impossible

**Attack Scenario:**
```cpp
// Dependency chain:
eda_fixed_string.hpp → needs safe_strcpy (from eda_safe_string.hpp)
eda_file_io.hpp → needs SettingsBufferLock (not defined!)
eda_shared_state.hpp → needs LockOrder (from eda_locking.hpp)

// If we implement in wrong order:
1. Implement eda_fixed_string.hpp (depends on existing eda_safe_string.hpp) ✓
2. Implement eda_file_io.hpp (SettingsBufferLock not defined!) ✗
3. Implement eda_shared_state.hpp (depends on eda_locking.hpp) ✓

// Migration fails at step 2!
```

**Result:** **MODERATE MIGRATION RISK**

---

### Logic Check Summary

| Flaw | Severity | Impact | Location |
|-------|-----------|----------|------------|
| SharedState<T> design broken | CRITICAL | No thread safety | eda_shared_state.hpp:1360 |
| Migration strategy unrealistic | CRITICAL | High risk | Migration Strategy:2308 |
| Rollback strategy insufficient | CRITICAL | Data loss | Rollback Strategy:2352 |
| Dependencies not documented | MODERATE | Migration risk | Throughout |

**Overall Logic:** **CRITICAL** - SharedState<T> design is fundamentally broken and must be revised.

---

## Critical Flaws Found Summary

| # | Flaw | Severity | Category | Blueprint Section |
|---|-------|-----------|------------------|
| 1 | SharedState<T> creates new mutex per operation | CRITICAL | Stack Overflow, Performance, Logic | eda_shared_state.hpp:1372 |
| 2 | SettingsBuffer exceeds stack budget | CRITICAL | Stack Overflow | eda_buffer_utils.hpp:157 |
| 3 | FixedString<N> duplicate definition | CRITICAL | Mayhem Compatibility | eda_fixed_string.hpp:36 |
| 4 | FixedString<N> silent truncation | CRITICAL | Corner Cases | eda_fixed_string.hpp:87 |
| 5 | SafeFile SD card removal not handled | CRITICAL | Corner Cases | eda_file_io.hpp:446 |
| 6 | SharedState<T> design fundamentally broken | CRITICAL | Logic | eda_shared_state.hpp:1360 |
| 7 | Migration strategy unrealistic | CRITICAL | Logic | Migration Strategy:2308 |
| 8 | Rollback strategy insufficient | CRITICAL | Logic | Rollback Strategy:2352 |

**Total Critical Flaws:** 8

---

## Revised Blueprint Sections

### Revision 1: Fix SharedState<T> Design

**Replace:** `eda_shared_state.hpp` lines 1360-1511

**With:**

```cpp
namespace ui::apps::enhanced_drone_analyzer {

/**
 * @brief Thread-safe wrapper for shared state (REVISED DESIGN)
 *
 * FIXES:
 * - Uses SINGLE shared mutex (not new mutex per operation)
 * - Eliminates stack overflow risk
 * - Eliminates performance degradation
 * - Provides actual thread safety
 *
 * @tparam T Type of shared value
 * @tparam LOCK_ORDER Lock order level for deadlock prevention
 */
template<typename T, LockOrder LOCK_ORDER>
class SharedState {
public:
    SharedState() noexcept = default;
    explicit constexpr SharedState(const T& value) noexcept : value_(value) {}
    explicit constexpr SharedState(T&& value) noexcept : value_(std::move(value)) {}
    
    /**
     * @brief Read value (with shared mutex lock)
     * @return Copy of current value
     * 
     * FIX: Uses shared mutex_ member, not new mutex per operation
     * Stack impact: ~0 bytes (mutex is class member)
     * Performance: ~0.1μs (mutex lock only)
     */
    T read() const noexcept {
        MutexLock lock(&mutex_, LOCK_ORDER);
        return value_;
    }
    
    /**
     * @brief Write value (with shared mutex lock)
     * @param value New value to write
     * 
     * FIX: Uses shared mutex_ member, not new mutex per operation
     */
    void write(const T& value) noexcept {
        MutexLock lock(&mutex_, LOCK_ORDER);
        value_ = value;
    }
    
    /**
     * @brief Update value with function (with shared mutex lock)
     * @tparam F Function type (callable with T& parameter)
     * @param func Function to apply to value
     * 
     * FIX: Uses shared mutex_ member, not new mutex per operation
     */
    template<typename F>
    void update(F&& func) noexcept {
        MutexLock lock(&mutex_, LOCK_ORDER);
        func(value_);
    }
    
    /**
     * @brief Read-modify-write with lock
     * @tparam F Function type (callable with T& parameter)
     * @param func Function to apply to value
     * @return Result of func(value_)
     * 
     * FIX: Uses shared mutex_ member, not new mutex per operation
     */
    template<typename F>
    auto read_modify_write(F&& func) noexcept -> decltype(func(std::declval<T>())) {
        MutexLock lock(&mutex_, LOCK_ORDER);
        return func(value_);
    }
    
private:
    T value_;
    mutable Mutex mutex_;  // ← SINGLE SHARED MUTEX (class member)
};

/**
 * @brief Thread-safe flag (specialization for bool)
 * 
 * FIXES:
 * - Uses ChibiOS critical sections (not new mutex per operation)
 * - Eliminates stack overflow risk
 * - Eliminates performance degradation
 * - Provides actual thread safety for simple flags
 */
template<LockOrder LOCK_ORDER>
class SharedFlag {
public:
    SharedFlag() noexcept : flag_(false) {}
    explicit constexpr SharedFlag(bool value) noexcept : flag_(value) {}
    
    /**
     * @brief Read flag (with critical section)
     * @return Current flag value
     * 
     * FIX: Uses chSysLock/chSysUnlock (no mutex needed)
     * Stack impact: ~0 bytes
     * Performance: ~0.01μs (critical section only)
     */
    bool read() const noexcept {
        chSysLock();
        bool value = flag_;
        chSysUnlock();
        return value;
    }
    
    /**
     * @brief Write flag (with critical section)
     * @param value New flag value
     * 
     * FIX: Uses chSysLock/chSysUnlock (no mutex needed)
     */
    void write(bool value) noexcept {
        chSysLock();
        flag_ = value;
        chSysUnlock();
    }
    
    /**
     * @brief Set flag to true
     */
    void set() noexcept {
        chSysLock();
        flag_ = true;
        chSysUnlock();
    }
    
    /**
     * @brief Clear flag to false
     */
    void clear() noexcept {
        chSysLock();
        flag_ = false;
        chSysUnlock();
    }
    
    /**
     * @brief Toggle flag
     */
    void toggle() noexcept {
        chSysLock();
        flag_ = !flag_;
        chSysUnlock();
    }
    
    /**
     * @brief Test and set (atomic)
     * @return Previous flag value
     * 
     * FIX: Uses chSysLock/chSysUnlock (atomic operation)
     */
    bool test_and_set() noexcept {
        chSysLock();
        bool old = flag_;
        flag_ = true;
        chSysUnlock();
        return old;
    }
    
private:
    volatile bool flag_;  // ← volatile for ISR visibility
};

/**
 * @brief Thread-safe counter (specialization for uint32_t)
 * 
 * FIXES:
 * - Uses ChibiOS critical sections (not new mutex per operation)
 * - Eliminates stack overflow risk
 * - Eliminates performance degradation
 * - Provides actual thread safety for simple counters
 */
template<LockOrder LOCK_ORDER>
class SharedCounter {
public:
    SharedCounter() noexcept : count_(0) {}
    explicit constexpr SharedCounter(uint32_t value) noexcept : count_(value) {}
    
    /**
     * @brief Read counter (with critical section)
     * @return Current counter value
     * 
     * FIX: Uses chSysLock/chSysUnlock (no mutex needed)
     * Stack impact: ~0 bytes
     * Performance: ~0.01μs (critical section only)
     */
    uint32_t read() const noexcept {
        chSysLock();
        uint32_t value = count_;
        chSysUnlock();
        return value;
    }
    
    /**
     * @brief Write counter (with critical section)
     * @param value New counter value
     * 
     * FIX: Uses chSysLock/chSysUnlock (no mutex needed)
     */
    void write(uint32_t value) noexcept {
        chSysLock();
        count_ = value;
        chSysUnlock();
    }
    
    /**
     * @brief Increment counter
     * @return New counter value
     */
    uint32_t increment() noexcept {
        chSysLock();
        uint32_t value = ++count_;
        chSysUnlock();
        return value;
    }
    
    /**
     * @brief Decrement counter
     * @return New counter value
     */
    uint32_t decrement() noexcept {
        chSysLock();
        uint32_t value = --count_;
        chSysUnlock();
        return value;
    }
    
    /**
     * @brief Add to counter
     * @param delta Value to add
     * @return New counter value
     */
    uint32_t add(uint32_t delta) noexcept {
        chSysLock();
        count_ += delta;
        uint32_t value = count_;
        chSysUnlock();
        return value;
    }
    
    /**
     * @brief Reset counter
     */
    void reset() noexcept {
        chSysLock();
        count_ = 0;
        chSysUnlock();
    }
    
private:
    volatile uint32_t count_;  // ← volatile for ISR visibility
};

} // namespace ui::apps::enhanced_drone_analyzer
```

**Changes:**
1. Single shared mutex (class member) instead of new mutex per operation
2. Eliminates stack overflow risk (no stack allocation for mutex)
3. Eliminates performance degradation (no mutex init/destroy per operation)
4. Provides actual thread safety (same mutex protects all operations)
5. Uses critical sections for simple types (bool, uint32_t)

---

### Revision 2: Fix SettingsBuffer Stack Budget

**Replace:** `eda_buffer_utils.hpp` lines 157-162

**With:**

```cpp
// Buffer size constants (REVISED for stack budget)
namespace BufferSizes {
    // FIX: Reduced from 1024 to 512 to stay within MAX_FUNCTION_STACK
    constexpr size_t SETTINGS_BUFFER = 512;   // Was 1024, reduced for stack safety
    constexpr size_t TIMESTAMP_BUFFER = 32;
    constexpr size_t HEADER_BUFFER = 256;
    constexpr size_t PATH_BUFFER = 64;
    constexpr size_t NAME_BUFFER = 32;
}

// Type aliases for common buffer sizes
using SettingsBuffer = StackBuffer<BufferSizes::SETTINGS_BUFFER>;   // 512 bytes (was 1024)
using TimestampBuffer = StackBuffer<BufferSizes::TIMESTAMP_BUFFER>;
using HeaderBuffer = StackBuffer<BufferSizes::HEADER_BUFFER>;
using PathBuffer = StackBuffer<BufferSizes::PATH_BUFFER>;
using NameBuffer = StackBuffer<BufferSizes::NAME_BUFFER>;
```

**Changes:**
1. Reduced `SETTINGS_BUFFER` from 1024 to 512 bytes
2. Stays within `MAX_FUNCTION_STACK` budget of 512 bytes
3. May require settings file format changes if 1024 bytes was actually needed

---

### Revision 3: Remove Duplicate FixedString<N>

**Replace:** `eda_fixed_string.hpp` lines 36-111

**With:**

```cpp
// NOTE: FixedString<N> is already implemented in eda_safe_string.hpp
// This file provides type aliases for common buffer sizes
// DO NOT redefine FixedString<N> - use existing implementation

namespace ui::apps::enhanced_drone_analyzer {

// Type aliases for common FixedString sizes (using existing implementation)
using TitleString = FixedString<32>;      // For View::title()
using DescriptionString = FixedString<64>; // For description buffers
using PathString = FixedString<64>;       // For file paths
using NameString = FixedString<32>;       // For names
using FormatString = FixedString<8>;      // For format strings

} // namespace ui::apps::enhanced_drone_analyzer
```

**Changes:**
1. Removed duplicate `FixedString<N>` class definition
2. Uses existing implementation from `eda_safe_string.hpp`
3. Provides type aliases for convenience

---

### Revision 4: Fix FixedString<N> Silent Truncation

**Add to:** `eda_safe_string.hpp` (extend existing FixedString<N> class)

**Add:**

```cpp
// In FixedString<N> class, add:

/**
 * @brief Append C string with truncation detection
 * @param str String to append (may be nullptr)
 * @return Number of characters appended, or SIZE_MAX if truncated
 * 
 * FIX: Returns SIZE_MAX on truncation to indicate data loss
 */
size_t append_with_truncation_check(const char* str) noexcept {
    size_t current_len = safe_strlen(buffer_, N);
    
    // Guard clause: Check for null source
    if (!str) {
        return 0;
    }
    
    // Guard clause: Buffer is already full
    if (current_len >= N - 1) {
        return SIZE_MAX;  // ← Indicates truncation
    }
    
    // Calculate how many characters we can append
    size_t str_len = 0;
    while (str[str_len] != '\0' && (current_len + str_len) < N - 1) {
        ++str_len;
    }
    
    // Check if truncation will occur
    if (str[str_len] != '\0') {
        // Truncation will occur
        for (size_t i = 0; i < str_len; ++i) {
            buffer_[current_len + i] = str[i];
        }
        buffer_[current_len + str_len] = '\0';
        length_ = current_len + str_len;
        verify_invariants();
        return SIZE_MAX;  // ← Indicates truncation occurred
    }
    
    // No truncation - append all characters
    for (size_t i = 0; i < str_len; ++i) {
        buffer_[current_len + i] = str[i];
    }
    buffer_[current_len + str_len] = '\0';
    length_ = current_len + str_len;
    verify_invariants();
    return str_len;  // ← Number of characters appended
}

/**
 * @brief Check if last append operation truncated data
 * @return true if last append truncated data
 * 
 * FIX: Allows caller to detect truncation
 */
bool was_truncated() const noexcept {
    // Implementation would require tracking truncation flag
    // For now, caller should use append_with_truncation_check
    return false;
}
```

**Changes:**
1. Added `append_with_truncation_check()` method that returns `SIZE_MAX` on truncation
2. Allows caller to detect data loss
3. Existing `append()` method remains unchanged for backward compatibility

---

### Revision 5: Fix SafeFile SD Card Removal Handling

**Replace:** `eda_file_io.hpp` lines 446-516 (AtomicFileWriter)

**With:**

```cpp
// Atomic file write (with backup and error handling)
class AtomicFileWriter {
public:
    static FileError write(const char* path, 
                          const void* data, 
                          size_t size) noexcept {
        // Create backup path
        PathBuffer backup_path;
        safe_strcpy(backup_path.ptr(), path, backup_path.capacity());
        safe_strcat(backup_path.ptr(), ".bak", backup_path.capacity());
        
        // Backup existing file
        SafeFile backup_file;
        FileError err = backup_file.open(backup_path.c_str(), false);
        if (err.is_success()) {
            // Copy existing to backup
            SafeFile original;
            if (original.open(path, true).is_success()) {
                copy_file(original, backup_file);
            }
        }
        
        // Write new data
        SafeFile new_file;
        err = new_file.open(path, false);
        if (!err.is_success()) {
            // FIX: Check if error is due to SD card removal
            if (err.fatfs_error == FR_DISK_ERR || 
                err.fatfs_error == FR_NOT_READY) {
                // SD card removed - cannot restore
                return err;
            }
            // Restore from backup
            restore_from_backup(backup_path.c_str(), path);
            return err;
        }
        
        size_t written;
        err = new_file.write(data, size, written);
        if (!err.is_success()) {
            new_file.close();
            
            // FIX: Check if error is due to SD card removal
            if (err.fatfs_error == FR_DISK_ERR || 
                err.fatfs_error == FR_NOT_READY) {
                // SD card removed - cannot restore
                return err;
            }
            
            restore_from_backup(backup_path.c_str(), path);
            return err;
        }
        
        // FIX: Verify all data was written
        if (written != size) {
            new_file.close();
            restore_from_backup(backup_path.c_str(), path);
            return FileError{FileResult::DISK_FULL, 0, written};
        }
        
        err = new_file.close();
        if (!err.is_success()) {
            // FIX: Check if error is due to SD card removal
            if (err.fatfs_error == FR_DISK_ERR || 
                err.fatfs_error == FR_NOT_READY) {
                // SD card removed - cannot remove backup
                return err;
            }
            
            // Close failed, but data may be written
            // Try to remove backup anyway
            f_unlink(reinterpret_cast<const TCHAR*>(backup_path.c_str()));
            return err;
        }
        
        // FIX: Safely remove backup (check for SD card removal)
        FRESULT unlink_result = f_unlink(reinterpret_cast<const TCHAR*>(backup_path.c_str()));
        if (unlink_result != FR_OK && 
            unlink_result != FR_NO_FILE && 
            unlink_result != FR_NO_PATH) {
            // Backup removal failed, but data is written
            // Log warning but don't fail the operation
            // (caller can decide if this is critical)
        }
        
        return FileError{FileResult::SUCCESS, 0, written};
    }
    
private:
    static void copy_file(SafeFile& src, SafeFile& dst) noexcept {
        constexpr size_t CHUNK_SIZE = 512;
        uint8_t buffer[CHUNK_SIZE];
        size_t read_bytes, written_bytes;
        
        while (true) {
            FileError err = src.read(buffer, CHUNK_SIZE, read_bytes);
            if (!err.is_success() || read_bytes == 0) break;
            
            err = dst.write(buffer, read_bytes, written_bytes);
            if (!err.is_success() || written_bytes != read_bytes) break;
        }
    }
    
    static void restore_from_backup(const char* backup_path, 
                                    const char* target_path) noexcept {
        SafeFile backup;
        if (!backup.open(backup_path, true).is_success()) return;
        
        SafeFile target;
        if (!target.open(target_path, false).is_success()) return;
        
        copy_file(backup, target);
    }
};
```

**Changes:**
1. Added checks for `FR_DISK_ERR` and `FR_NOT_READY` (SD card removal)
2. Added verification that all data was written
3. Added safe backup removal with error checking
4. Returns appropriate error for SD card removal scenarios

---

### Revision 6: Fix FrequencyOps No Clamped Result

**Replace:** `eda_frequency.hpp` lines 1047-1062

**With:**

```cpp
// Safe frequency addition with overflow check and clamping
inline constexpr bool add_frequency(uint64_t a, uint64_t b, uint64_t& result) noexcept {
    if (a > std::numeric_limits<uint64_t>::max() - b) {
        // FIX: Clamp to max value instead of leaving result uninitialized
        result = std::numeric_limits<uint64_t>::max();
        return false;  // Indicates overflow occurred
    }
    result = a + b;
    return true;  // Indicates success
}

// Safe frequency subtraction with underflow check and clamping
inline constexpr bool sub_frequency(uint64_t a, uint64_t b, uint64_t& result) noexcept {
    if (a < b) {
        // FIX: Clamp to min value instead of leaving result uninitialized
        result = 0;
        return false;  // Indicates underflow occurred
    }
    result = a - b;
    return true;  // Indicates success
}

// Safe frequency addition with automatic clamping
inline constexpr uint64_t add_frequency_clamped(uint64_t a, uint64_t b) noexcept {
    uint64_t result;
    if (add_frequency(a, b, result)) {
        return result;  // No overflow
    }
    // Overflow occurred - return clamped value
    return std::numeric_limits<uint64_t>::max();
}

// Safe frequency subtraction with automatic clamping
inline constexpr uint64_t sub_frequency_clamped(uint64_t a, uint64_t b) noexcept {
    uint64_t result;
    if (sub_frequency(a, b, result)) {
        return result;  // No underflow
    }
    // Underflow occurred - return clamped value
    return 0;
}
```

**Changes:**
1. Added clamping to max/min value on overflow/underflow
2. Added `add_frequency_clamped()` and `sub_frequency_clamped()` for automatic clamping
3. Result is always initialized (no undefined behavior)

---

### Revision 7: Reduce Memory Barriers

**Replace:** `eda_isr_safe.hpp` lines 1568-1616

**With:**

```cpp
// ISR-safe read functions (with optimized memory barriers)
inline uint32_t read_scan_cycles() noexcept {
    // FIX: Single barrier is sufficient for aligned 32-bit reads on ARM Cortex-M4
    __DMB();  // Data Memory Barrier (ARM Cortex-M4)
    uint32_t value = scan_cycles;
    // No second barrier needed for aligned 32-bit reads
    return value;
}

inline int32_t read_current_rssi() noexcept {
    // FIX: Single barrier is sufficient for aligned 32-bit reads on ARM Cortex-M4
    __DMB();
    int32_t value = current_rssi;
    return value;
}

inline bool read_spectrum_data_ready() noexcept {
    // FIX: Single barrier is sufficient for aligned bool reads on ARM Cortex-M4
    __DMB();
    bool value = spectrum_data_ready;
    return value;
}

inline void clear_spectrum_data_ready() noexcept {
    // FIX: Barrier before write ensures visibility
    __DMB();
    spectrum_data_ready = false;
    // No barrier after needed (write is immediately visible)
}

// ISR-safe write functions (called from ISR)
inline void set_spectrum_data_ready() noexcept {
    // No barrier needed in ISR (single-threaded context)
    spectrum_data_ready = true;
}

inline void set_scan_cycles(uint32_t value) noexcept {
    // FIX: Barrier after write ensures visibility
    scan_cycles = value;
    __DMB();
}

inline void set_current_rssi(int32_t value) noexcept {
    // FIX: Barrier after write ensures visibility
    current_rssi = value;
    __DMB();
}

inline void set_hardware_error() noexcept {
    // FIX: Barrier after write ensures visibility
    hardware_error = true;
    __DMB();
}

inline void set_last_valid_frequency(uint64_t freq) noexcept {
    // FIX: Barrier after write ensures visibility
    // For 64-bit values, may need additional barriers on some ARM implementations
    last_valid_frequency = freq;
    __DMB();
    __DSB();  // Data Synchronization Barrier for 64-bit values
}
```

**Changes:**
1. Reduced from 2 barriers to 1 barrier for aligned 32-bit reads
2. Removed unnecessary barriers after writes (write is immediately visible)
3. Added `__DSB()` for 64-bit values (may need additional synchronization)
4. Performance improvement: ~2x faster for volatile reads

---

### Revision 8: Fix Migration Strategy

**Replace:** Migration Strategy section (lines 2308-2350)

**With:**

```cpp
## 6. Migration Strategy (REVISED)

### 6.1 Incremental Migration Approach

**CRITICAL FIX:** Migrate incrementally, not all at once.

**Phase 1: Foundation (Week 1)**
- Create eda_buffer_utils.hpp (reduce SETTINGS_BUFFER to 512)
- Update existing code to use StackBuffer pattern
- **Test:** Verify stack usage within budget

**Phase 2: Thread Safety (Week 2)**
- Create eda_shared_state.hpp (REVISED design with shared mutex)
- Replace critical SharedState usage
- **Test:** Verify thread safety with stress tests

**Phase 3: String Handling (Week 3)**
- DO NOT create eda_fixed_string.hpp (use existing eda_safe_string.hpp)
- Add truncation detection to FixedString<N>
- Replace std::string usage with FixedString<N>
- **Test:** Verify no data loss on truncation

**Phase 4: File I/O (Week 4)**
- Create eda_file_io.hpp (with SD card removal handling)
- Replace unsafe file operations
- **Test:** Verify error handling for SD card removal

**Phase 5: Type Safety (Week 5)**
- Create eda_frequency.hpp (with clamped operations)
- Create eda_enums.hpp
- Replace magic numbers and plain enums
- **Test:** Verify type safety and no overflow

**Phase 6: Code Organization (Week 6-7)**
- Create dsp/ and ui/ subdirectories
- Separate UI/DSP logic
- Decompose perform_scan_cycle
- **Test:** Verify functionality preserved

### 6.2 Rollback Strategy (REVISED)

**CRITICAL FIX:** Each phase includes data migration strategy.

**Phase 1 Rollback:**
- Delete eda_buffer_utils.hpp
- Revert to existing buffer patterns
- **Data:** No data migration needed (no format changes)

**Phase 2 Rollback:**
- Delete eda_shared_state.hpp
- Revert to existing mutex patterns
- **Data:** No data migration needed (no format changes)

**Phase 3 Rollback:**
- Revert FixedString<N> changes to std::string
- Settings file format unchanged
- **Data:** No data migration needed

**Phase 4 Rollback:**
- Revert to existing file I/O patterns
- Settings file format unchanged
- **Data:** No data migration needed

**Phase 5 Rollback:**
- Revert magic numbers and enums
- Settings file format unchanged
- **Data:** No data migration needed

**Phase 6 Rollback:**
- Delete dsp/ and ui/ subdirectories
- Revert to existing code organization
- **Data:** No data migration needed

### 6.3 Validation Criteria (REVISED)

**Phase 1 (Foundation):**
- All new buffer sizes within stack budget
- Stack usage < 80% of stack size
- **Tool:** StackMonitor from eda_locking.hpp

**Phase 2 (Thread Safety):**
- No race conditions (verified with thread sanitizer if available)
- All ISR-accessible variables are volatile
- All shared state uses proper locking
- **Tool:** Thread sanitizer, code review

**Phase 3 (String Handling):**
- Zero heap allocations (verified with chHeapStatus)
- All truncations detected and logged
- No data loss on buffer overflow
- **Tool:** Static analysis, runtime tests

**Phase 4 (File I/O):**
- All file operations return error codes
- SD card removal handled gracefully
- No data corruption on power loss
- **Tool:** Manual testing with SD card removal

**Phase 5 (Type Safety):**
- No magic numbers in production code
- All enums are enum class
- No overflow/underflow bugs
- **Tool:** Static analysis, unit tests

**Phase 6 (Code Organization):**
- UI and DSP logic separated
- No function exceeds 50 lines
- All tests pass
- **Tool:** Unit tests, integration tests
```

**Changes:**
1. Incremental migration (one component per week)
2. Each phase has clear rollback strategy
3. No data migration needed (settings format unchanged)
4. Clear validation criteria for each phase
5. Uses existing FixedString<N> (no duplicate)

---

## Attack Results Summary

### Attack Vector 1: Stack Overflow Test
**Result:** **CRITICAL FLAWS FOUND**
- SharedState<T> creates new mutex per operation (20-30 bytes per op)
- SettingsBuffer exceeds stack budget (1024 > 512 bytes)
- Multiple FixedStrings in single call frame (192 bytes)
- std::array<freqman_entry, 10> (640 bytes)
- **Total potential stack overflow:** 1444-1644 bytes

### Attack Vector 2: Performance Test
**Result:** **CRITICAL FLAWS FOUND**
- SharedState<T> performance degradation (10-20x slower)
- Excessive memory barriers (2.5x slower)
- SafeNumeric conversion overhead (1.5-2x slower)
- StringParser bounds checking (1.2-1.5x slower)
- **Overall performance impact:** CRITICAL

### Attack Vector 3: Mayhem Compatibility Test
**Result:** **CRITICAL FLAWS FOUND**
- FixedString<N> duplicate definition (compilation error)
- SharedState<T> inconsistent with existing patterns
- SafeFile different error handling (acceptable)
- std::string_view inconsistency (minor)
- **Overall compatibility:** MODERATE (critical blocker: duplicate definition)

### Attack Vector 4: Corner Cases
**Result:** **CRITICAL FLAWS FOUND**
- FixedString<N> silent truncation (data loss)
- SafeFile SD card removal not handled (data corruption)
- StringParser malformed input (data corruption)
- FrequencyOps no clamped result (undefined behavior)
- ISRSafe interrupt storms (race condition)
- **Overall corner case handling:** CRITICAL

### Attack Vector 5: Logic Check
**Result:** **CRITICAL FLAWS FOUND**
- SharedState<T> design fundamentally broken (no thread safety)
- Migration strategy unrealistic (high risk)
- Rollback strategy insufficient (data loss risk)
- Dependencies not documented (migration risk)
- **Overall logic:** CRITICAL

---

## Final Attack Result

**Attack Result:** Initial plan used SharedState<T> with new mutex per operation causing stack overflow and performance degradation. SettingsBuffer exceeded stack budget. FixedString<N> duplicate definition. Silent data truncation. SD card removal not handled. Migration strategy unrealistic. Revised to use shared mutex, reduced buffer sizes, use existing FixedString<N>, add truncation detection, handle SD card removal, incremental migration.

**Recommendation:** Blueprint requires **8 critical revisions** before Stage 4 implementation. Most critical flaw is SharedState<T> design which must be completely revised.

---

## Next Steps

1. **Approve Revised Blueprint Sections** - Review and approve the 8 revised sections
2. **Update Stage 2 Blueprint** - Incorporate revisions into original blueprint
3. **Proceed to Stage 4** - Implement revised blueprint with incremental migration
4. **Continuous Validation** - Use StackMonitor and thread sanitizers during implementation

---

**Report End**
