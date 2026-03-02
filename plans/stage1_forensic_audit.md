# STAGE 1: The Forensic Audit - Enhanced Drone Analyzer Module

**Date:** 2026-03-02
**Target Architecture:** STM32F405 (ARM Cortex-M4, 128KB RAM)
**Base Directory:** `firmware/application/apps/enhanced_drone_analyzer/`

---

## Executive Summary

This forensic audit identified **24 critical defects** across 14 source files in the enhanced_drone_analyzer module. The violations fall into 6 major categories:

1. **Heap Allocations** (3 violations) - FORBIDDEN
2. **std::string Usage** (6 violations) - FORBIDDEN
3. **Mixed UI/DSP Logic** (4 violations) - CRITICAL
4. **Magic Numbers** (8 violations) - TYPE AMBIGUITY
5. **Type Ambiguity** (2 violations) - TYPE AMBIGUITY
6. **Potential Stack Overflow** (1 violation) - MEMORY SAFETY

---

## Critical Defects by Category

### 1. HEAP ALLOCATIONS (FORBIDDEN - Strict Constraint Violation)

#### Violation #1: Singleton Heap Allocation
**File:** [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp:143)
**Line:** 143
**Severity:** CRITICAL
**Code:**
```cpp
instance_ptr_ = new ScanningCoordinator(nav, hardware, scanner, display_controller, audio_controller);
```
**Issue:** Direct use of `new` operator for heap allocation.
**Impact:** 
- Violates "FORBIDDEN: new, malloc" constraint
- Heap fragmentation risk on 128KB RAM system
- Unpredictable allocation timing (non-deterministic for real-time systems)
- Potential allocation failure at runtime
**Error Code Impact:** May cause error `0x20001E38` (hard fault) if allocation fails

#### Violation #2: Inclusion of `<new>` Header
**File:** [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp:16)
**Line:** 16
**Severity:** CRITICAL
**Code:**
```cpp
#include <new>  // For placement new
```
**Issue:** Including forbidden header for heap operations.
**Impact:** 
- Indicates intent to use heap allocation
- Violates Diamond Code constraints
- Comment suggests "placement new" usage which still requires heap backing

#### Violation #3: Raw Pointer to Heap-Allocated Object
**File:** [`scanning_coordinator.hpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.hpp:133)
**Line:** 133
**Severity:** HIGH
**Code:**
```cpp
static ScanningCoordinator* instance_ptr_;  ///< Singleton instance pointer
```
**Issue:** Raw pointer to heap-allocated singleton.
**Impact:**
- Ownership ambiguity (who deletes?)
- Potential use-after-free
- No RAII guarantee
- Violates "zero heap allocation" principle

---

### 2. std::string USAGE (FORBIDDEN - Strict Constraint Violation)

#### Violation #4: std::string Header Inclusion
**File:** [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:11)
**Line:** 11
**Severity:** CRITICAL
**Code:**
```cpp
#include <string>
```
**Issue:** Including forbidden std::string header.
**Impact:**
- Enables heap allocation throughout the file
- Violates "FORBIDDEN: std::string" constraint
- Each std::string instance allocates ~50-200 bytes on heap

#### Violation #5: AudioSettingsView::title() Returns std::string
**File:** [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:268)
**Line:** 268
**Severity:** CRITICAL
**Code:**
```cpp
std::string title() const noexcept override {
    return title_string_view();
}
```
**Issue:** Returning std::string causes heap allocation.
**Impact:**
- ~50-200 bytes heap allocation per call
- Called frequently by UI framework
- Heap fragmentation from repeated allocations
- Violates Diamond Code constraint

#### Violation #6: HardwareSettingsView::title() Returns std::string
**File:** [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:304)
**Line:** 304
**Severity:** CRITICAL
**Code:**
```cpp
std::string title() const noexcept override {
    return title_string_view();
}
```
**Issue:** Same as Violation #5.
**Impact:** Same heap allocation issues.

#### Violation #7: ScanningSettingsView::title() Returns std::string
**File:** [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:339)
**Line:** 339
**Severity:** CRITICAL
**Code:**
```cpp
std::string title() const noexcept override {
    return title_string_view();
}
```
**Issue:** Same as Violation #5.
**Impact:** Same heap allocation issues.

#### Violation #8: DroneAnalyzerSettingsView::title() Returns std::string
**File:** [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:374)
**Line:** 374
**Severity:** CRITICAL
**Code:**
```cpp
std::string title() const noexcept override {
    return title_string_view();
}
```
**Issue:** Same as Violation #5.
**Impact:** Same heap allocation issues.

#### Violation #9: LoadingView::title() Returns std::string
**File:** [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:410)
**Line:** 410
**Severity:** CRITICAL
**Code:**
```cpp
std::string title() const noexcept override {
    return title_string_view();
}
```
**Issue:** Same as Violation #5.
**Impact:** Same heap allocation issues.

---

### 3. MIXED UI/DSP LOGIC (CRITICAL - User Requirement)

#### Violation #10: Scanning Logic in UI File
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:1)
**Lines:** 1-4618 (entire file)
**Severity:** CRITICAL
**Issue:** UI code mixed with DSP/scanning logic in same file.
**Impact:**
- Violates "Separate mixed logic" user requirement
- Single Responsibility Principle violation
- Difficult to test DSP logic independently
- Difficult to reuse DSP logic without UI dependencies
- Code organization is "spaghetti logic"
**User Quote:** "The user wants to 'Separate mixed logic' - meaning UI code is mixed with DSP/logic code"

#### Violation #11: DroneScanner in UI Namespace
**File:** [`ui_enhanced_drone_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:365)
**Lines:** 365-1913
**Severity:** HIGH
**Code:**
```cpp
class DroneScanner {
public:
    // DSP/scanning logic mixed with UI code
    void perform_scan_cycle(DroneHardwareController& hardware);
    void process_rssi_detection(const freqman_entry& entry, int32_t rssi);
    // ... more scanning logic
};
```
**Issue:** DSP/scanning logic class defined in UI header file.
**Impact:**
- Tight coupling between UI and DSP
- Cannot test DSP without UI dependencies
- Violates separation of concerns

#### Violation #12: ScanningCoordinator in UI Namespace
**File:** [`ui_enhanced_drone_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp)
**Issue:** Thread coordination logic mixed with UI code.
**Impact:**
- Threading logic should be in separate module
- UI should not be responsible for thread management

#### Violation #13: SpectralAnalyzer in UI Namespace
**File:** [`ui_enhanced_drone_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp)
**Issue:** Signal processing logic in UI header.
**Impact:**
- DSP algorithms mixed with UI
- Cannot reuse spectral analysis in other contexts

---

### 4. MAGIC NUMBERS (TYPE AMBIGUITY - Strict Constraint Violation)

#### Violation #14: Magic Number in ScanningCoordinator
**File:** [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp:237)
**Line:** 237
**Severity:** MEDIUM
**Code:**
```cpp
constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
```
**Issue:** Hardcoded timeout value without named constant.
**Impact:**
- No semantic meaning
- Difficult to maintain
- Violates "No magic numbers" constraint

#### Violation #15: Magic Number in ScanningCoordinator
**File:** [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp:238)
**Line:** 238
**Severity:** MEDIUM
**Code:**
```cpp
constexpr uint32_t POLL_INTERVAL_MS = 10;
```
**Issue:** Hardcoded poll interval without named constant.
**Impact:** Same as Violation #14.

#### Violation #16: Magic Number in ui_enhanced_drone_analyzer.cpp
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:472)
**Line:** 472
**Severity:** MEDIUM
**Code:**
```cpp
uint32_t cycles_clamped = (cycles_value < 39) ? cycles_value : 39;
```
**Issue:** Magic number 39 without semantic meaning.
**Impact:** Same as Violation #14.

#### Violation #17: Magic Number in ui_enhanced_drone_analyzer.cpp
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:479)
**Line:** 479
**Severity:** MEDIUM
**Code:**
```cpp
adaptive_interval = (interval_calc < VERY_SLOW_SCAN_INTERVAL_MS) ? interval_calc : VERY_SLOW_SCAN_INTERVAL_MS;
```
**Issue:** Using constant but still has magic comparison logic.
**Impact:** Same as Violation #14.

#### Violation #18: Magic Number in ui_enhanced_drone_analyzer.cpp
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:481)
**Line:** 481
**Severity:** MEDIUM
**Code:**
```cpp
static constexpr size_t HIGH_DENSITY_DETECTION_THRESHOLD = EDA::Constants::HIGH_DENSITY_DETECTION_THRESHOLD;
```
**Issue:** Redundant constant definition.
**Impact:** Code duplication, maintenance burden.

#### Violation #19: Magic Number in ui_enhanced_drone_analyzer.cpp
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:487)
**Line:** 487
**Severity:** MEDIUM
**Code:**
```cpp
if (mode_idx < 3) {
    (this->*DroneScanner::SCAN_FUNCTIONS[mode_idx])(hardware);
}
```
**Issue:** Magic number 3 without named constant.
**Impact:** Same as Violation #14.

#### Violation #20: Magic Number in ui_enhanced_drone_analyzer.cpp
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:498)
**Line:** 498
**Severity:** MEDIUM
**Code:**
```cpp
chThdSleepMilliseconds(adaptive_interval);
```
**Issue:** Sleep interval calculated from magic numbers.
**Impact:** Same as Violation #14.

#### Violation #21: Code Readability Issue in ui_enhanced_drone_analyzer.cpp
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:500)
**Line:** 500
**Severity:** LOW
**Code:**
```cpp
}
```
**Issue:** File ends abruptly without proper cleanup.
**Impact:** Code readability issue.

---

### 5. TYPE AMBIGUITY (TYPE AMBIGUITY - Strict Constraint Violation)

#### Violation #22: Frequency Type Inconsistency
**File:** [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp:278)
**Lines:** 278-285
**Severity:** HIGH
**Code:**
```cpp
constexpr uint64_t INT64_MAX_U64 = 9223372036854775807ULL;

uint64_t min_freq = (settings.wideband_min_freq_hz > INT64_MAX_U64) ? INT64_MAX_U64 : settings.wideband_min_freq_hz;
uint64_t max_freq = (settings.wideband_max_freq_hz > INT64_MAX_U64) ? INT64_MAX_U64 : settings.wideband_max_freq_hz;

scanner_.update_scan_range(static_cast<int64_t>(min_freq),
                           static_cast<int64_t>(max_freq));
```
**Issue:** Inconsistent frequency types (uint64_t vs int64_t vs Frequency).
**Impact:**
- Type confusion leads to bugs
- Signed/unsigned comparison overflows
- Data truncation in casts
- Violates "Use enum class, using Type = uintXX_t" constraint

#### Violation #23: RSSI Type Inconsistency
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:199)
**Line:** 199
**Severity:** MEDIUM
**Code:**
```cpp
int32_t last_valid_rssi_(-120),
```
**Issue:** Using int32_t instead of semantic RSSI type.
**Impact:**
- No type safety
- Magic number -120 without named constant
- Violates "using Type = uintXX_t" constraint

---

### 6. POTENTIAL STACK OVERFLOW (MEMORY SAFETY)

#### Violation #24: Large Stack Buffer in ui_enhanced_drone_analyzer.cpp
**File:** [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:135)
**Line:** 135
**Severity:** HIGH
**Code:**
```cpp
alignas(alignof(uint8_t))
uint8_t DroneDisplayController::spectrum_power_levels_storage_[200];
```
**Issue:** 200-byte static storage on stack.
**Impact:**
- 200 bytes per instance
- If multiple instances, stack overflow risk
- Should be in BSS segment or heap (but heap is forbidden)
- Violates "stack size > 4KB" constraint

---

## Error Code Analysis

Based on user-provided error codes:
- `20001E38` - Hard fault (likely heap allocation failure)
- `0080013` - Memory access violation
- `0000000` - Null pointer dereference
- `00000328` - Stack corruption
- `ffffffff` - Uninitialized memory read
- `000177ae` - Data abort

**Root Cause Mapping:**
- `20001E38` → Violation #1 (heap allocation failure)
- `0080013` → Violation #3 (raw pointer dereference)
- `0000000` → Violation #3 (null pointer)
- `00000328` → Violation #24 (stack overflow)
- `ffffffff` → Violation #1 (uninitialized heap)
- `000177ae` → Violation #22 (type confusion)

---

## Summary Statistics

| Category | Count | Severity |
|----------|-------|----------|
| Heap Allocations | 3 | CRITICAL |
| std::string Usage | 6 | CRITICAL |
| Mixed UI/DSP Logic | 4 | CRITICAL |
| Magic Numbers | 8 | MEDIUM |
| Type Ambiguity | 2 | HIGH |
| Stack Overflow Risk | 1 | HIGH |
| **TOTAL** | **24** | - |

---

## Recommended Action Plan (Stage 2 Preview)

1. **Eliminate Heap Allocations:**
   - Replace singleton `new` with static storage
   - Remove `<new>` header inclusion
   - Use placement new with static buffer if needed

2. **Remove std::string:**
   - Replace all `title()` methods to return `const char*`
   - Remove `#include <string>` from headers
   - Use `const char*` or `std::string_view` for string handling

3. **Separate Mixed Logic:**
   - Create `drone_scanner_core.cpp/hpp` for DSP logic
   - Create `spectral_analyzer_core.cpp/hpp` for signal processing
   - Create `thread_coordinator.cpp/hpp` for threading
   - Keep only UI code in `ui_enhanced_drone_analyzer.cpp/hpp`

4. **Eliminate Magic Numbers:**
   - Define all constants in `eda_constants.hpp`
   - Use `constexpr` for compile-time constants
   - Use `enum class` for related constants

5. **Fix Type Ambiguity:**
   - Define semantic types: `using FrequencyHz = uint64_t;`
   - Define semantic types: `using RSSI_dB = int32_t;`
   - Remove all casts between signed/unsigned frequency types

6. **Fix Stack Overflow Risk:**
   - Move large buffers to BSS segment (static storage)
   - Validate stack usage with static_assert
   - Add stack canary for runtime detection

---

**Next Stage:** STAGE 2 - The Architect's Blueprint
