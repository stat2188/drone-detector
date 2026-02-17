# Enhanced Drone Analyzer - Architectural Analysis Report

## Executive Summary

This report provides a comprehensive architectural analysis of the `enhanced_drone_analyzer` application for PortaPack H2 (resource-constrained MCU). The analysis identifies memory bottlenecks, CPU hot paths, flash usage patterns, architecture issues, and UI performance concerns.

**Total Codebase Size:** ~166KB .cpp + ~71KB .hpp = ~237KB source code
**Target Platform:** PortaPack H2 (STM32F405, Cortex-M4F, 192KB RAM, 1MB Flash)

---

## 1. File-by-File Analysis

### 1.1 [`ui_enhanced_drone_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp)

**Size:** 70,826 bytes (1,706 lines)

**Classes Defined:**
- `ScopedLock<MutexType, TryLock>` - RAII mutex wrapper
- `TrackedDrone` - Drone tracking with RSSI history
- `DisplayDroneEntry` - Display data structure
- `DroneDetectionLogger` - Async SD card logging
- `DroneScanner` - Core scanning engine
- `DroneHardwareController` - Radio hardware abstraction
- `SmartThreatHeader` - UI header component
- `ThreatCard` - UI card component
- `ConsoleStatusBar` - Status bar component
- `CompactFrequencyRuler` - Frequency display
- `DroneDisplayController` - Display management
- `DroneUIController` - UI event handling
- `EnhancedDroneSpectrumAnalyzerView` - Main view

**Memory Usage Analysis:**

| Structure | Size (bytes) | Storage Type | Issue |
|-----------|--------------|--------------|-------|
| `TrackedDrone` | ~48 | Stack/Static | MAX_HISTORY=4 entries |
| `DroneScanner::freq_db_storage_` | 2,048 | Static .bss | Placement new target |
| `DroneScanner::tracked_drones_storage_` | ~192 | Static .bss | 4 drones max |
| `DroneScanner::db_loading_wa_` | 4,096 | Static .bss | Thread stack |
| `DroneDetectionLogger::worker_wa_` | 4,096 | Static .bss | Worker thread stack |
| `DroneDetectionLogger::ring_buffer_` | ~1,024 | Member | 32 entries |
| `DroneDisplayController::spectrum_row_buffer_storage_` | 480 | Static .bss | 240 Color entries |
| `DroneDisplayController::render_line_buffer_storage_` | 480 | Static .bss | 240 Color entries |
| `DroneDisplayController::spectrum_power_levels_storage_` | 200 | Static .bss | 200 uint8_t |
| `DroneDisplayController::detected_drones_storage_` | ~240 | Static .bss | 3 DisplayDroneEntry |

**Critical Observations:**
- Extensive use of `static inline` storage for zero-heap allocation
- Placement new pattern for FreqmanDB and TrackedDrones
- Multiple thread stacks (total ~12KB static)
- `DroneScanner` stores `const DroneAnalyzerSettings&` reference (lifetime dependency)

### 1.2 [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp)

**Size:** 166,097 bytes (~3,500+ lines)

**Key Components:**
- Static member definitions
- `BUILTIN_DRONE_DB` (17 entries, ~600 bytes Flash)
- `DroneScanner` implementation
- `DroneHardwareController` implementation
- `DroneDisplayController` implementation
- `EnhancedDroneSpectrumAnalyzerView` implementation
- Deferred initialization state machine

**CPU Hot Paths:**

```cpp
// Line 397-400: perform_scan_cycle - called every scan interval
void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    // Calls SCAN_FUNCTIONS[mode] via function pointer LUT
    (this->*SCAN_FUNCTIONS[static_cast<uint8_t>(scanning_mode_)]) (hardware);
}
```

**Threading Model:**
- Main UI thread (ChibiOS)
- Scanning thread (created by ScanningCoordinator)
- Database loading thread (async initialization)
- Detection logger worker thread

### 1.3 [`ui_signal_processing.hpp/cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.hpp)

**Size:** 90 lines / 87 lines

**Classes:**
- `DetectionRingBuffer` - Lock-free ring buffer for detections

**Memory Usage:**
- `entries_`: 16 × ~24 bytes = 384 bytes (reduced from 32 in Phase 3)

**Algorithm Analysis:**
- Linear probe hash table (O(n) worst case)
- Open addressing with linear probing
- No mutex (lock-free for single-writer/single-reader)

**Issue:** Hash collisions cause O(n) lookup when buffer is full.

### 1.4 [`scanning_coordinator.hpp/cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.hpp)

**Size:** 64 lines / 106 lines

**Purpose:** Coordinates scanning between hardware, scanner, and display

**Memory Usage:**
- `coordinator_wa_`: 6,144 bytes (reduced from 12KB in Phase 2)

**Threading:**
- Creates dedicated scanning thread
- Thread priority: NORMALPRIO

### 1.5 [`ui_drone_common_types.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_drone_common_types.hpp)

**Size:** 259 lines

**Key Structures:**
- `DroneAnalyzerSettings` - Packed struct (<=512 bytes)
- `DetectionLogEntry` - Packed struct for logging
- Enums: `ThreatLevel`, `DroneType`, `MovementTrend`, `SpectrumMode`

**Optimization Applied:**
```cpp
#pragma pack(push, 1)
struct DroneAnalyzerSettings { /* ... */ };
#pragma pack(pop)
static_assert(sizeof(DroneAnalyzerSettings) <= 512, "...");
```

### 1.6 [`color_lookup_unified.hpp`](firmware/application/apps/enhanced_drone_analyzer/color_lookup_unified.hpp)

**Size:** 233 lines

**Purpose:** Unified color and string lookup tables (SSOT)

**Flash Usage:**
- `ThreatColorLUT::COLORS`: 6 × 2 bytes = 12 bytes
- `ThreatColorLUT::CARD_STYLES`: 5 × 4 bytes = 20 bytes
- `DroneColorLUT::COLORS`: 11 × 2 bytes = 22 bytes
- `UnifiedStringLookup::THREAT_NAMES`: 6 pointers
- `UnifiedStringLookup::DRONE_TYPE_NAMES`: 11 pointers

**All stored in Flash via `constexpr`**

### 1.7 [`ui_spectral_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_spectral_analyzer.hpp)

**Size:** 216 lines

**Purpose:** Spectral analysis using M0 FFT data

**Key Algorithm:**
- 64-bin histogram for noise floor estimation
- O(n) median calculation (replaces rolling median)
- Signal classification by width

**Memory Usage:**
- `HistogramBuffer`: 64 × 2 = 128 bytes (caller-provided)

### 1.8 [`eda_constants.hpp`](firmware/application/apps/enhanced_drone_analyzer/eda_constants.hpp)

**Size:** ~500 lines

**Purpose:** Single source of truth for all constants

**Key Constants:**
- Frequency limits (1MHz - 7.2GHz)
- RSSI thresholds (-120 to 10 dBm)
- Buffer sizes (reduced in Phase 3)
- Stack sizes (reduced in Phase 2)

### 1.9 [`eda_optimized_utils.hpp`](firmware/application/apps/enhanced_drone_analyzer/eda_optimized_utils.hpp)

**Size:** ~450 lines

**Templates:**
- `MedianFilter<T, N>` - Unified median filter
- `CachedValue<T>` - Check-before-update helper
- `FrequencyValidator` - Constexpr validation

### 1.10 [`settings_persistence.hpp`](firmware/application/apps/enhanced_drone_analyzer/settings_persistence.hpp)

**Size:** ~600 lines

**Purpose:** Settings serialization/deserialization

**Key Feature:**
- LUT-based parsing (52 settings)
- Single-pass buffer serialization
- Zero heap allocation

---

## 2. Critical Issues Ranked by Severity

### CRITICAL (P0) - Must Fix

#### 2.1 Stack Overflow Risk from Static Thread Stacks

**Location:** Multiple files

**Problem:** Thread working areas are allocated statically but may not fit in available RAM.

```cpp
// ui_enhanced_drone_analyzer.hpp:367
static constexpr size_t WORKER_STACK_SIZE = 4096;
static WORKING_AREA(worker_wa_, WORKER_STACK_SIZE);

// ui_enhanced_drone_analyzer.hpp:635
static constexpr size_t DB_LOADING_STACK_SIZE = 4096;
static WORKING_AREA(db_loading_wa_, DB_LOADING_STACK_SIZE);

// scanning_coordinator.hpp:58
static constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 6144;
static WORKING_AREA(coordinator_wa_, COORDINATOR_THREAD_STACK_SIZE);
```

**Total Static Thread Stacks:** 4KB + 4KB + 6KB = 14KB

**Impact:** Potential stack overflow during heavy I/O operations.

#### 2.2 Lifetime Dependency on Settings Reference

**Location:** [`ui_enhanced_drone_analyzer.hpp:660`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:660)

```cpp
// LIFETIME: settings_ must outlive this instance; it references parent class member
const DroneAnalyzerSettings& settings_;
```

**Problem:** If parent object is destroyed before `DroneScanner`, dangling reference causes UB.

**Mitigation:** Documented but not enforced by compiler.

### HIGH (P1) - Should Fix

#### 2.3 Lock-Free Ring Buffer Assumptions

**Location:** [`ui_signal_processing.hpp:39-65`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.hpp:39)

**Problem:** Lock-free design assumes single-writer/single-reader but doesn't enforce it.

```cpp
// THREAD SAFETY DOCUMENTATION:
// WRITER THREAD: DroneScanner::scan_thread (baseband/M0 context)
// READER THREAD: UI thread (main application context)
// WARNING: If multi-writer access is needed in future, add MutexLock protection
```

**Risk:** If threading model changes, race conditions will occur.

#### 2.4 Linear Search in Detection Ring Buffer

**Location:** [`ui_signal_processing.cpp:46-61`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.cpp:46)

```cpp
uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const noexcept {
    const size_t start_idx = frequency_hash % MAX_ENTRIES;
    for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
        // Linear probe - O(n) worst case
    }
}
```

**Impact:** O(n) lookup when buffer is near full. With n=16, acceptable but not optimal.

#### 2.5 Reinterpret_cast for Static Storage Access

**Location:** [`ui_enhanced_drone_analyzer.hpp:1054-1059`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:1054)

```cpp
std::array<DisplayDroneEntry, MAX_UI_DRONES>& detected_drones() {
    return *reinterpret_cast<std::array<DisplayDroneEntry, MAX_UI_DRONES>*>(detected_drones_storage_);
}
```

**Problem:** Relies on alignment guarantees. While `alignas` is used, this is fragile.

### MEDIUM (P2) - Consider Fixing

#### 2.6 Large Main View Class

**Location:** [`ui_enhanced_drone_analyzer.hpp:1425`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:1425)

**Problem:** `EnhancedDroneSpectrumAnalyzerView` contains many member objects:

```cpp
DroneHardwareController hardware_;
DroneScanner scanner_;
::AudioManager audio_;
DroneDisplayController display_controller_;
DroneUIController ui_controller_;
ScanningCoordinator scanning_coordinator_;
SmartThreatHeader smart_header_;
ConsoleStatusBar status_bar_;
std::array<ThreatCard, 3> threat_cards_;
// ... plus buttons, fields, etc.
```

**Impact:** Large stack footprint when view is instantiated.

#### 2.7 Deferred Initialization Complexity

**Location:** [`ui_enhanced_drone_analyzer.hpp:1446-1456`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:1446)

```cpp
enum class InitState : uint8_t {
    CONSTRUCTED = 0,
    BUFFERS_ALLOCATED,
    DATABASE_LOADED,
    HARDWARE_READY,
    UI_LAYOUT_READY,
    SETTINGS_LOADED,
    COORDINATOR_READY,
    FULLY_INITIALIZED = 7,
    INITIALIZATION_ERROR = 8
};
```

**Problem:** 8-state initialization machine adds complexity and potential for partial initialization states.

#### 2.8 std::string in Settings Editor

**Location:** [`ui_enhanced_drone_settings.hpp:383`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:383)

```cpp
// DIAMOND OPTIMIZATION: Use std::string buffer for TextEdit (requires std::string&), sync to char array on save
// This is the minimal heap usage compromise - TextEdit widget requires std::string reference
std::string description_buffer_;
```

**Problem:** `TextEdit` widget requires `std::string&`, forcing heap allocation.

### LOW (P3) - Minor Issues

#### 2.9 Template Bloat from MedianFilter

**Location:** [`eda_optimized_utils.hpp:46-111`](firmware/application/apps/enhanced_drone_analyzer/eda_optimized_utils.hpp:46)

**Problem:** Template instantiated for multiple types may cause code bloat.

#### 2.10 Magic Numbers in Phase Delays

**Location:** [`ui_enhanced_drone_analyzer.hpp:1485-1490`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:1485)

```cpp
static constexpr uint32_t PHASE_DELAY_0_MS = 50;
static constexpr uint32_t PHASE_DELAY_1_MS = 300;
// ... etc
```

**Problem:** Numbers appear arbitrary without clear derivation.

---

## 3. Optimization Recommendations

### 3.1 Memory Optimizations

| Current | Recommended | Savings |
|---------|-------------|---------|
| Thread stacks: 14KB total | Profile actual usage, reduce if possible | 2-4KB potential |
| `DetectionRingBuffer`: 16 entries | Use power-of-2 hash function | Better cache behavior |
| `TrackedDrone::MAX_HISTORY`: 4 | Already optimized | N/A |
| Static storage for buffers | Already optimized | N/A |

### 3.2 CPU Optimizations

1. **Replace linear probe with power-of-2 hash mask:**
```cpp
// Current: frequency_hash % MAX_ENTRIES
// Recommended: frequency_hash & (MAX_ENTRIES - 1)  // MAX_ENTRIES must be power of 2
```

2. **Pre-compute frequency formatting:**
```cpp
// Current: snprintf in display hot path
// Recommended: Pre-format at detection time, cache result
```

3. **Inline critical path methods:**
- Already done for `get_trend()`, `add_rssi()`
- Consider inlining `get_bar_color_index()`

### 3.3 Flash Optimizations

1. **All LUTs already in Flash via `constexpr`** - Good
2. **EDA_FLASH_CONST attribute used** - Good
3. **Template bloat:** Audit instantiated templates

### 3.4 Architecture Improvements

1. **Replace settings reference with copy:**
```cpp
// Current: const DroneAnalyzerSettings& settings_;
// Recommended: const DroneAnalyzerSettings settings_;  // ~512 bytes, safer lifetime
```

2. **Add compile-time lifetime enforcement:**
```cpp
static_assert(std::is_trivially_destructible<DroneScanner>::value,
              "DroneScanner must not outlive settings");
```

3. **Consider SoA for TrackedDrone:**
```cpp
// Current: Array of structures
std::array<TrackedDrone, 4> drones;
// Alternative: Structure of arrays (better cache for batch operations)
struct TrackedDronesSoA {
    std::array<uint32_t, 4> frequencies;
    std::array<uint8_t, 4> drone_types;
    // ...
};
```

---

## 4. UI Performance Analysis

### 4.1 Rendering Pipeline

```
DisplayFrameSync Message
    -> step_deferred_initialization() [if not FULLY_INITIALIZED]
    -> display_controller_.process_frame_sync()
    -> handle_scanner_update()
```

### 4.2 Check-Before-Update Pattern

**Implemented in:**
- `SmartThreatHeader::last_threat_`, `last_freq_`, etc.
- `ThreatCard::last_frequency_`, `last_threat_`, etc.
- `DroneDisplayController::last_drone_text_0_`, etc.

**Benefit:** Avoids redundant `Text::set()` calls when content unchanged.

### 4.3 Paint Methods

**Concern:** String formatting in `paint()` methods:

```cpp
// SmartThreatHeader::paint()
snprintf(buffer, sizeof(buffer), "THREAT: %s | %c%zu %c%zu %c%zu",
         UnifiedStringLookup::threat_name(static_cast<uint8_t>(last_threat_)),
         // ...
);
```

**Recommendation:** Pre-format strings during `update()`, not `paint()`.

---

## 5. Compliance with Embedded Constraints

| Constraint | Status | Notes |
|------------|--------|-------|
| No dynamic allocation | **Partial** | `std::string` in settings editor |
| constexpr/const for Flash | **Good** | All LUTs in Flash |
| Compile-time > Runtime | **Good** | Templates, constexpr |
| Data-Oriented Design | **Partial** | Some SoA, mostly AoS |
| No virtual functions in hot paths | **Good** | Function pointer LUTs instead |
| Strong typing | **Good** | enum class everywhere |
| No magic numbers | **Partial** | Phase delays need documentation |

---

## 6. Code Quality Observations

### 6.1 Positive Patterns

1. **Scott Meyers Item 15 compliance:** `constexpr` preferred over `#define`
2. **RAII mutex wrappers:** `ScopedLock`, `MutexLock`, `SDCardLock`
3. **Static assertions:** Size and alignment checks throughout
4. **Thread safety documentation:** Clear locking order documented
5. **Packed structs:** `#pragma pack` for memory efficiency

### 6.2 Areas for Improvement

1. **Error handling:** Many `return;` statements on error without logging
2. **Magic numbers:** Phase delay values need derivation comments
3. **Complexity:** 8-state initialization machine could be simplified
4. **Coupling:** `DroneScanner` tightly coupled to `DroneHardwareController`

---

## 7. Summary

The Enhanced Drone Analyzer codebase demonstrates strong embedded C++ practices with:
- Zero-heap allocation design (mostly achieved)
- Flash-resident constant data
- Thread-safe synchronization primitives
- Check-before-update UI optimization

**Primary Concerns:**
1. Large static thread stacks (14KB total)
2. Lifetime dependency on settings reference
3. Complexity of deferred initialization

**Recommended Priority:**
1. Profile actual stack usage to validate 4KB/6KB sizes
2. Consider copying settings instead of referencing
3. Simplify initialization state machine

---

*Report generated: 2026-02-17*
*Analyzer: Architect Mode*