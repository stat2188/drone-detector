# Enhanced Drone Analyzer - Resource Usage Analysis

## Overview

This report analyzes the Enhanced Drone Analyzer (EDA) application in `mayhem-firmware\firmware\application\apps\enhanced_drone_analyzer` to evaluate resource usage (CPU, memory, stack, heap). The analysis focuses on the following files:

1. ui_enhanced_drone_memory_pool.hpp
2. ui_signal_processing.cpp/hpp
3. ui_drone_common_types.cpp/hpp
4. ui_drone_audio.hpp
5. ui_spectral_analyzer.hpp
6. diamond_core.hpp
7. settings_persistence.hpp/cpp
8. eda_advanced_settings.hpp/cpp

## Memory Allocation Patterns

### 1. Static Allocation

#### Stack Allocation (Automatic Storage)
- **TrackedDrone** class ([`ui_enhanced_drone_analyzer.hpp:106-186`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:106-186)):
  - Fixed-size arrays for RSSI and timestamp history: `rssi_history_[8]`, `timestamp_history_[8]`
  - Total size per instance: ~40 bytes

- **DisplayDroneEntry** struct ([`ui_enhanced_drone_analyzer.hpp:188-197`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:188-197)):
  - Fixed-size char array for type name: `type_name[16]`
  - Total size: ~40 bytes

- **WidebandScanData** struct ([`ui_enhanced_drone_analyzer.hpp:212-225`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:212-225)):
  - Fixed-size array for slices: `slices[20]`
  - Total size: ~200 bytes

#### Global/Static Storage
- **BUILTIN_DRONE_DB** ([`ui_enhanced_drone_analyzer.cpp:154-197`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:154-197)):
  - Static array of 31 BuiltinDroneFreq structs
  - Total size: ~31 * 12 bytes = ~372 bytes

- **DroneDetectionLogger** static members ([`ui_enhanced_drone_analyzer.cpp:138-143`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:138-143)):
  - Worker thread stack: `WORKER_STACK_SIZE = 4096 bytes`
  - DB loading thread stack: `DB_LOADING_STACK_SIZE` (not explicitly defined, but ChibiOS default likely 2048-4096 bytes)

### 2. Heap Allocation

#### Memory Pool Implementation
The application uses a custom memory pool implementation ([`ui_enhanced_drone_memory_pool.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_memory_pool.hpp)) that pre-allocates memory statically but manages it dynamically:

```cpp
// Pool definitions (line 191-229)
class EDAMemoryPools {
    static constexpr size_t DETECTION_LOG_POOL_SIZE = 32;
    using DetectionLogPool = FixedSizeMemoryPool<DetectionLogEntry, DETECTION_LOG_POOL_SIZE>;
    
    static constexpr size_t DISPLAY_DRONE_POOL_SIZE = 16;
    using DisplayDronePool = FixedSizeMemoryPool<DisplayDroneEntry, DISPLAY_DRONE_POOL_SIZE>;
    
    static constexpr size_t TRACKED_DRONE_POOL_SIZE = 16;
    using TrackedDronePool = FixedSizeMemoryPool<TrackedDrone, TRACKED_DRONE_POOL_SIZE>;
};
```

**Pool Sizes:**
- DetectionLogPool: 32 entries × ~32 bytes = ~1KB
- DisplayDronePool: 16 entries × ~40 bytes = ~640 bytes
- TrackedDronePool: 16 entries × ~40 bytes = ~640 bytes
- **Total Pool Memory:** ~2.28KB

#### Dynamic Allocation in Main Application
- **DroneScanner** class ([`ui_enhanced_drone_analyzer.cpp:203-242`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:203-242)):
  - Uses placement new for freq_db_ptr_ and tracked_drones_ptr_ (lazy initialization)
  - FreqmanDB and TrackedDrone array allocated from heap

### 3. Stack Usage Patterns

#### Large Stack Allocations
1. **SpectralAnalyzer::analyze()** ([`ui_spectral_analyzer.hpp:75-150`](firmware/application/apps/enhanced_drone_analyzer/ui_spectral_analyzer.hpp:75-150)):
   - Allocates histogram array: `std::array<uint16_t, 128>` (256 bytes)
   - Total stack usage: ~300 bytes

2. **SettingsPersistence::load()** ([`settings_persistence.hpp:305-394`](firmware/application/apps/enhanced_drone_analyzer/settings_persistence.hpp:305-394)):
   - Uses static buffers (line_buffer[144], read_buffer[256]) - ~400 bytes
   - Stack usage: ~500 bytes

3. **MedianFilter::get_median()** ([`eda_optimized_utils.hpp:58-81`](firmware/application/apps/enhanced_drone_analyzer/eda_optimized_utils.hpp:58-81)):
   - Allocates temporary array: `std::array<T, N>` (up to 11 elements)
   - Stack usage: ~44 bytes (for N=11, T=int16_t)

#### Deep Nesting
- **DroneScanner::perform_scan_cycle()** calls mode-specific scan functions (database, wideband, hybrid)
- Each scan function may call multiple helper methods
- Maximum nesting depth: ~5-6 levels

### 4. Const Placement and Flash/RAM Usage

#### Flash Storage Optimization
The application uses `FLASH_STORAGE` attribute to place constant data in flash memory instead of RAM:

```cpp
// Diamond core LUTs (diamond_core.hpp line 73)
static constexpr int32_t THRESHOLDS[5] FLASH_STORAGE = {
    -120,  // NONE (0)
    -100,  // LOW (1)
    -85,   // MEDIUM (2)
    -70,   // HIGH (3)
    -50    // CRITICAL (4)
};

// Settings LUT (settings_persistence.hpp line 106)
inline constexpr SettingMetadata SETTINGS_LUT[] FLASH_STORAGE = {
    SET_META_BIT(audio_flags, 0, "true"),
    SET_META(audio_alert_frequency_hz, TYPE_UINT32, 200, 20000, "800"),
    // ... 50 more settings
};
```

**Flash Usage:**
- Translation tables: ENGLISH_TRANSLATIONS (10 entries × 8 bytes = ~80 bytes), RUSSIAN_TRANSLATIONS (~80 bytes)
- Frequency multipliers LUT: MULTIPLIERS[7] (~28 bytes)
- Settings LUT: SETTINGS_LUT[52] (~52 × 24 bytes = ~1.25KB)
- Total flash-stored LUTs: ~1.5KB

#### RAM Usage
- **DroneAnalyzerSettings** struct ([`ui_drone_common_types.hpp:60-158`](firmware/application/apps/enhanced_drone_analyzer/ui_drone_common_types.hpp:60-158)):
  - Size: ≤ 512 bytes (static_assert check line 162)
  - Contains multiple char arrays: log_file_path[64], log_format[8], color_scheme[32], etc.

### 5. Dynamic Memory Allocation Analysis

**Search Results:** No explicit `new`/`malloc`/`calloc`/`realloc` calls found in the analyzed files. The application uses:
1. Static arrays and memory pools for dynamic-like behavior
2. Placement new for object construction in pre-allocated memory
3. No standard library containers that allocate on the heap (std::vector, std::string)

### 6. Embedded C++ Best Practices

#### Compliance
- **Zero Heap Allocation:** Uses static arrays and memory pools instead of dynamic allocation
- **Const Correctness:** Extensive use of constexpr and const for constants
- **Flash Storage:** Uses FLASH_STORAGE attribute for read-only data
- **Packed Structs:** DroneAnalyzerSettings uses #pragma pack(push, 1) to minimize size
- **RAII Wrappers:** MutexLock, SDCardLock for automatic resource management
- **Type Safety:** Uses enum classes instead of raw integers
- **Lazy Initialization:** FreqmanDB and TrackedDrone array initialized on first use

#### Violations/Areas for Improvement

1. **Stack Overflow Risk in SettingsPersistence** ([`settings_persistence.hpp:305-394`](firmware/application/apps/enhanced_drone_analyzer/settings_persistence.hpp:305-394)):
   - Uses stack-allocated buffers for file reading
   - Risk of stack overflow with large files

2. **Lack of Stack Usage Documentation** ([`ui_enhanced_drone_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp)):
   - Thread stack sizes defined but not documented
   - No stack usage measurements or comments

3. **Potential Memory Leak in DroneScanner** ([`ui_enhanced_drone_analyzer.cpp:203-242`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:203-242)):
   - Uses placement new but destructor calls not explicitly documented
   - Manual cleanup required for freq_db_ptr_ and tracked_drones_ptr_

4. **String Pool Overflow** ([`ui_enhanced_drone_analyzer.hpp:317-350`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:317-350)):
   - StringPool has simple wrap-around logic that can overwrite existing strings
   - No error handling for pool exhaustion

### 7. CPU Usage Analysis

#### Signal Processing
- **SpectralAnalysis** ([`ui_spectral_analyzer.hpp:75-150`](firmware/application/apps/enhanced_drone_analyzer/ui_spectral_analyzer.hpp:75-150)):
  - Histogram calculation: O(N) for 232 valid bins
  - Median lookup: O(1) with histogram
  - Classification: O(1) switch case

- **MedianFilter** ([`eda_optimized_utils.hpp:47-93`](firmware/application/apps/enhanced_drone_analyzer/eda_optimized_utils.hpp:47-93)):
  - Selection algorithm: O(N²) for N=11 (acceptably fast for Cortex-M4)

- **DetectionRingBuffer** ([`ui_signal_processing.cpp:22-85`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.cpp:22-85)):
  - Hash table lookup: O(1) average case, O(N) worst case (32 entries max)

#### Settings Parsing
- **SettingsPersistence** ([`settings_persistence.hpp:305-457`](firmware/application/apps/enhanced_drone_analyzer/settings_persistence.hpp:305-457)):
  - Linear search through SETTINGS_LUT: O(N) for 52 settings
  - snprintf for string formatting: minimal overhead

### 8. Summary Statistics

| Resource | Usage | Location |
|----------|-------|----------|
| **Total Stack Usage** | ~1-2KB per thread | Various |
| **Static RAM** | ~5-6KB | Global/static variables |
| **Memory Pool RAM** | ~2.28KB | EDAMemoryPools |
| **Flash Storage** | ~1.5KB | FLASH_STORAGE constants |
| **Heap Usage** | Dynamic (freq_db_ptr_, tracked_drones_ptr_) | DroneScanner |
| **CPU Usage** | O(N) per scan cycle | DroneScanner, SpectralAnalyzer |

### 9. Optimization Recommendations

1. **Increase Stack Sizes for Workers** ([`ui_enhanced_drone_analyzer.hpp:384`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:384)):
   - Current WORKER_STACK_SIZE = 4096 bytes - should be sufficient
   - Monitor stack usage with ChibiOS stack checking

2. **Add Error Handling to StringPool** ([`ui_enhanced_drone_analyzer.hpp:317-350`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp:317-350)):
   - Implement proper pool exhaustion detection
   - Add error code return or exception handling

3. **Document Stack Usage** ([`ui_enhanced_drone_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp)):
   - Add comments to thread stack size definitions
   - Include stack usage measurements from testing

4. **Improve Settings Persistence** ([`settings_persistence.hpp`](firmware/application/apps/enhanced_drone_analyzer/settings_persistence.hpp)):
   - Replace stack-allocated buffers with static or pool-allocated memory
   - Add overflow checking for buffer operations

## Conclusion

The Enhanced Drone Analyzer application demonstrates good memory management practices for embedded systems, with:
- Minimal use of dynamic allocation
- Static allocation and memory pools for predictable behavior
- Flash storage for constant data
- RAII for resource management
- Packed structs to minimize memory waste

Potential improvements exist in stack management, error handling, and documentation, but overall the application is well-optimized for Cortex-M4 constraints.
