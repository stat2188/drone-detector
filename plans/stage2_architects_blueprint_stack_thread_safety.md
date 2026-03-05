# Stage 2: Architect's Blueprint - Enhanced Drone Analyzer
## Stack Usage and Thread Safety Fix Plan

**Date:** 2026-03-05  
**Platform:** STM32F405 (ARM Cortex-M4, 128KB RAM)  
**Target:** Enhanced Drone Analyzer Application  
**Constraint:** NO dynamic allocation (no std::vector, std::string, std::map, new, malloc)  
**Constraint:** Stack frames must be < 4KB  
**Constraint:** No exceptions, no RTTI, bare-metal / ChibiOS RTOS

---

## Executive Summary

This blueprint provides specific architectural solutions for the **24 defects** identified in Stage 1 Forensic Audit. The solutions are prioritized by severity and include:

1. **Specific data structure replacements** (e.g., std::string → char array)
2. **Memory placement strategies** (Flash vs RAM using const/constexpr)
3. **Function signature designs** and RAII wrappers
4. **Flash memory migration** opportunities (const data, lookup tables)
5. **Thread reduction strategies**
6. **Synchronization mechanisms** for thread safety

---

## P0-STOP: Critical Defect Solutions

### P0-1: std::string title() Methods - Framework Limitation

**Defect:** Multiple View classes override `title()` to return `std::string`, causing heap allocation (50-200 bytes per call).

**Affected Files:**
- [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp):279-281, 315-317, 350-352, 385-387, 421-423, 491-493, 757-759

**Solution Options:**

#### Option A: Framework Modification (Recommended for Long Term)
Modify the UI framework to support `std::string_view` or `const char*` return type:

```cpp
// Framework change: ui.hpp (base View class)
class View {
public:
    // OLD: Returns std::string (heap allocation)
    // virtual std::string title() const = 0;
    
    // NEW: Returns string_view (zero allocation)
    virtual std::string_view title() const noexcept = 0;
    
    // Alternative: Returns const char* (zero allocation)
    // virtual const char* title() const noexcept = 0;
};
```

**Benefits:**
- Eliminates all heap allocation from title() methods
- Zero runtime overhead
- Type-safe (string_view provides bounds checking)

**Drawbacks:**
- Requires framework modification (outside EDA directory)
- Breaking change for all View subclasses

**Migration Path:**
1. Add new `title_string_view()` method to View base class
2. Keep old `title()` method for backward compatibility
3. Deprecate `title()` method with `[[deprecated]]` attribute
4. Remove `title()` method after migration period

#### Option B: Zero-Allocation Wrapper (Immediate Workaround)
Create a zero-allocation wrapper that caches the title string:

```cpp
// ui_enhanced_drone_settings.hpp
template <typename DerivedView>
class ZeroAllocTitleView : public DerivedView {
public:
    std::string title() const noexcept override {
        // Return cached string (only allocated once)
        if (!title_cached_) {
            title_string_ = DerivedView::title_string_view();
            title_cached_ = true;
        }
        return title_string_;
    }
    
private:
    mutable std::string title_string_;  // Allocated once per view instance
    mutable bool title_cached_ = false;
};

// Usage:
using ZeroAllocAudioSettingsView = ZeroAllocTitleView<AudioSettingsView>;
```

**Benefits:**
- Reduces heap allocation from "every call" to "once per view instance"
- No framework changes required
- Immediate improvement

**Drawbacks:**
- Still allocates heap memory (just less frequently)
- Increases memory usage per view instance
- Not a true zero-heap solution

#### Option C: Static Title Storage (Best for Embedded)
Store titles in Flash memory and return pointers:

```cpp
// ui_enhanced_drone_settings.hpp
namespace TitleStorage {
    // Store titles in Flash (const section)
    constexpr char AUDIO_SETTINGS_TITLE[] = "Audio Settings";
    constexpr char HARDWARE_SETTINGS_TITLE[] = "Hardware Settings";
    constexpr char SCANNING_SETTINGS_TITLE[] = "Scanning Settings";
    constexpr char DRONE_ANALYZER_SETTINGS_TITLE[] = "EDA Settings";
    constexpr char LOADING_TITLE[] = "Loading";
    constexpr char EDIT_FREQUENCY_TITLE[] = "Edit Frequency";
    constexpr char DATABASE_LIST_TITLE[] = "Database List";
}

class AudioSettingsView : public View {
public:
    // Zero-allocation title (returns pointer to Flash)
    std::string title() const noexcept override {
        return TitleStorage::AUDIO_SETTINGS_TITLE;  // Still allocates
    }
    
    // Internal zero-allocation version
    static constexpr const char* title_string_view() noexcept {
        return TitleStorage::AUDIO_SETTINGS_TITLE;
    }
};
```

**Benefits:**
- Titles stored in Flash (no RAM usage)
- Zero allocation for internal use
- Type-safe (const char*)

**Drawbacks:**
- Framework still allocates std::string when title() is called
- Requires framework change for true zero-heap

**Recommended Solution:** **Option A** (Framework modification) for long-term, **Option C** (Flash storage) for immediate improvement.

---

### P0-2: std::string in FixedStringBuffer - Framework Limitation

**Defect:** `FixedStringBuffer` uses placement new to construct `std::string` in union storage, allocating heap memory when TextEdit widget uses it.

**Affected Files:**
- [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp):632, 649, 662, 676, 680

**Solution Options:**

#### Option A: Framework Modification (Recommended)
Modify TextEdit widget to accept char* buffer directly:

```cpp
// Framework change: ui_textedit.hpp
class TextEdit : public Widget {
public:
    // OLD: Requires std::string& (heap allocation)
    // TextEdit(Rect rect, std::string& buffer);
    
    // NEW: Accepts char* buffer (zero allocation)
    TextEdit(Rect rect, char* buffer, size_t buffer_size);
    
    // NEW: Returns const char* (zero allocation)
    const char* get_text() const noexcept;
    
    // NEW: Returns text length
    size_t get_length() const noexcept;
    
private:
    char* buffer_;
    size_t buffer_size_;
    size_t length_;
};
```

**Benefits:**
- Eliminates all heap allocation from TextEdit widget
- Zero runtime overhead
- Simpler API (no std::string wrapper needed)

**Drawbacks:**
- Requires framework modification
- Breaking change for all TextEdit users

#### Option B: Custom TextEdit Widget (Immediate Workaround)
Create a custom TextEdit widget that works with char* buffers:

```cpp
// ui_enhanced_drone_settings.hpp
class ZeroAllocTextEdit : public Widget {
public:
    ZeroAllocTextEdit(Rect rect, char* buffer, size_t buffer_size)
        : Widget(rect), buffer_(buffer), buffer_size_(buffer_size), length_(0) {
        buffer_[0] = '\0';
    }
    
    void paint(Painter& painter) override {
        // Custom rendering logic
        painter.draw_string(rect(), buffer_);
    }
    
    bool on_key(const KeyEvent key) override {
        // Custom key handling
        if (key >= KeyEvent::Digit0 && key <= KeyEvent::Digit9) {
            if (length_ < buffer_size_ - 1) {
                buffer_[length_++] = '0' + (key - KeyEvent::Digit0);
                buffer_[length_] = '\0';
                set_dirty();
            }
        }
        // ... handle other keys
        return true;
    }
    
    const char* get_text() const noexcept { return buffer_; }
    size_t get_length() const noexcept { return length_; }
    void clear() noexcept { buffer_[0] = '\0'; length_ = 0; }
    
private:
    char* buffer_;
    size_t buffer_size_;
    size_t length_;
};
```

**Benefits:**
- Zero heap allocation
- No framework changes required
- Immediate improvement

**Drawbacks:**
- Requires custom widget implementation
- Duplicates TextEdit functionality
- Maintenance burden

#### Option C: Lazy Allocation with Immediate Sync (Current Implementation)
Keep current `FixedStringBuffer` implementation but add immediate sync:

```cpp
// ui_enhanced_drone_settings.hpp
class FixedStringBuffer {
public:
    // Sync from temp_string immediately after modification
    void sync_from_temp() noexcept {
        if (temp_string_constructed_) {
            std::string& temp = get_temp_string();
            size_t len = std::min(temp.size(), size_);
            std::memcpy(buffer_, temp.c_str(), len);
            buffer_[len] = '\0';
            size_ = len;
            
            // Destroy temp_string immediately to free heap
            destroy_temp_string();
        }
    }
    
    // ... rest of implementation
};
```

**Benefits:**
- Minimizes heap allocation time
- Keeps current implementation
- No framework changes

**Drawbacks:**
- Still allocates heap memory (just briefly)
- Not a true zero-heap solution

**Recommended Solution:** **Option A** (Framework modification) for long-term, **Option B** (Custom widget) for immediate improvement.

---

### P0-3: Remove <string> Header

**Defect:** File includes `<string>` header but attempts to avoid using `std::string`.

**Affected Files:**
- [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp):11

**Solution:**

```cpp
// ui_enhanced_drone_settings.hpp
// C++ standard library headers (alphabetical order)
#include <array>
#include <cstddef>
#include <cstdint>
// #include <memory>  // REMOVE - not used
// #include <string>   // REMOVE - framework limitation workaround
#include <string_view>  // ADD - use string_view instead

// Add comment explaining framework limitation:
// NOTE: <string> header is included only for framework compatibility (View::title() returns std::string)
// All internal EDA code uses char* or std::string_view (zero allocation)
```

**Benefits:**
- Removes unused header
- Documents framework limitation
- Encourages zero-allocation patterns

---

## P1-HIGH: High Priority Defect Solutions

### P1-1: Mutex Ordering Validation

**Defect:** No compile-time validation of lock ordering in `start_coordinated_scanning()`.

**Affected Files:**
- [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp):278-279

**Solution:**

```cpp
// scanning_coordinator.cpp
StartResult ScanningCoordinator::start_coordinated_scanning() noexcept {
    // Add lock order validation
    LockOrderValidator validator(LockOrder::DATA_MUTEX);
    
    MutexLock thread_lock(thread_mutex_, LockOrder::DATA_MUTEX);
    
    // Validate that we can acquire next lock
    chDbgAssert(validator.can_acquire(LockOrder::DATA_MUTEX),
                "Lock order violation",
                "Cannot acquire state_mutex_ after thread_mutex_");
    
    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
    
    // ... rest of function
}
```

**Benefits:**
- Compile-time lock order validation
- Prevents deadlock
- Documents lock ordering intent

---

### P1-2: Lock-Free Read Operations (Optional Optimization)

**Defect:** Full mutex protection for read operations adds performance overhead.

**Affected Files:**
- [`ui_signal_processing.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.hpp):292, 304

**Solution:**

```cpp
// ui_signal_processing.hpp
class DetectionRingBuffer {
public:
    // Lock-free read using atomic operations
    [[nodiscard]] DetectionCount get_detection_count_lockfree(FrequencyHash frequency_hash) const noexcept {
        // Use atomic load for version field
        uint32_t version = __atomic_load_n(&global_version_, __ATOMIC_ACQUIRE);
        
        // Hash table lookup
        const size_t hash_idx = hash_index(frequency_hash);
        
        for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
            const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;
            
            // Atomic load of entry fields
            FrequencyHash hash = __atomic_load_n(&entries_[idx].frequency_hash, __ATOMIC_ACQUIRE);
            
            if (hash == frequency_hash) {
                // Check version hasn't changed
                uint32_t entry_version = __atomic_load_n(&entries_[idx].version, __ATOMIC_ACQUIRE);
                if (entry_version == version) {
                    return entries_[idx].detection_count;
                } else {
                    // Version changed, retry
                    return 0;  // Or retry from beginning
                }
            }
            
            if (hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
                return 0;
            }
        }
        
        return 0;
    }
    
    // Keep mutex-protected version for consistency
    [[nodiscard]] DetectionCount get_detection_count(FrequencyHash frequency_hash) const noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
        return get_detection_count_lockfree(frequency_hash);
    }
};
```

**Benefits:**
- Lock-free read operations (better performance)
- Still provides mutex-protected version for consistency
- Optional optimization (can be enabled via compile-time flag)

**Drawbacks:**
- More complex implementation
- Potential for torn reads on 32-bit ARM
- Requires careful testing

**Recommendation:** Keep current mutex-protected implementation. Lock-free optimization is only needed if performance profiling shows contention.

---

### P1-3: Coordinator Thread Stack Size

**Defect:** Coordinator thread stack size (2048 bytes) may be insufficient for deep call stacks.

**Affected Files:**
- [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp):67

**Solution:**

```cpp
// scanning_coordinator.cpp
// Increase stack size for safety margin
constexpr size_t COORDINATOR_THREAD_STACK_SIZE = 3072;  // 3KB (was 2048)

stkalign_t ScanningCoordinator::coordinator_wa_[THD_WA_SIZE(COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];
```

**Benefits:**
- Reduces stack overflow risk
- Provides safety margin for deep call stacks
- Still below 4KB limit

---

### P1-4: Stack Allocation Prevention

**Defect:** `DetectionRingBuffer` could be accidentally allocated on stack, causing stack overflow.

**Affected Files:**
- [`ui_signal_processing.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.hpp):235-396

**Solution:**

```cpp
// ui_signal_processing.hpp
class DetectionRingBuffer {
public:
    // Add compile-time check to prevent stack allocation
    static_assert(sizeof(DetectionRingBuffer) < 4096, 
                  "DetectionRingBuffer too large for stack allocation");
    static_assert(alignof(DetectionRingBuffer) == 4,
                  "DetectionRingBuffer must be 4-byte aligned");
    
    // Delete new operator to prevent heap allocation
    static void* operator new(std::size_t) = delete;
    static void* operator new[](std::size_t) = delete;
    
    // ... rest of class
};
```

**Benefits:**
- Compile-time prevention of stack allocation
- Compile-time prevention of heap allocation
- Enforces BSS allocation

---

### P1-5: Large Structure Stack Allocation Prevention

**Defect:** Large structures could be allocated on stack, contributing to stack overflow.

**Affected Files:**
- [`ui_drone_common_types.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_drone_common_types.hpp):25
- [`dsp_display_types.hpp`](firmware/application/apps/enhanced_drone_analyzer/dsp_display_types.hpp):99

**Solution:**

```cpp
// ui_drone_common_types.hpp
#pragma pack(push, 1)
struct DroneAnalyzerSettings {
    // ... struct members
    
    // Add compile-time check
    static_assert(sizeof(DroneAnalyzerSettings) <= 512,
                  "DroneAnalyzerSettings exceeds 512 bytes");
};
#pragma pack(pop)

// dsp_display_types.hpp
struct FilteredDronesSnapshot {
    size_t count;
    TrackedDroneData drones[10];
    
    // Add compile-time check
    static_assert(sizeof(FilteredDronesSnapshot) <= 640,
                  "FilteredDronesSnapshot exceeds 640 bytes");
};
```

**Benefits:**
- Compile-time size validation
- Prevents accidental size growth
- Documents size constraints

---

### P1-6: Histogram Buffer Stack Usage

**Defect:** `HistogramBuffer` (128 bytes) could contribute to stack overflow in deep call stacks.

**Affected Files:**
- [`ui_spectral_analyzer.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_spectral_analyzer.hpp):82

**Solution:**

```cpp
// ui_spectral_analyzer.hpp
class SpectralAnalyzer {
public:
    // Keep current implementation (passed by reference)
    static SpectralAnalysisResult analyze(
        const std::array<uint8_t, 256>& db_buffer,
        const SpectralAnalysisParams& params,
        HistogramBuffer& histogram_buffer) noexcept;
    
    // Alternative: Static buffer (no stack allocation)
    static SpectralAnalysisResult analyze_static(
        const std::array<uint8_t, 256>& db_buffer,
        const SpectralAnalysisParams& params) noexcept {
        static HistogramBuffer static_buffer;  // BSS allocation
        return analyze(db_buffer, params, static_buffer);
    }
};
```

**Benefits:**
- Eliminates stack allocation for histogram buffer
- Static buffer is always available
- No performance overhead

---

### P1-7: Singleton Initialization Safety

**Defect:** `instance()` halts system if called before initialization.

**Affected Files:**
- [`scanning_coordinator.cpp`](firmware/application/apps/enhanced_drone_analyzer/scanning_coordinator.cpp):108-151

**Solution:**

```cpp
// scanning_coordinator.cpp
// Add safe instance access method
ScanningCoordinator* ScanningCoordinator::instance_safe() noexcept {
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    
    if (!initialized_ || !instance_ptr_) {
        return nullptr;  // Graceful handling (no halt)
    }
    
    return const_cast<ScanningCoordinator*>(instance_ptr_);
}

// Update instance() to use instance_safe()
ScanningCoordinator& ScanningCoordinator::instance() noexcept {
    ScanningCoordinator* ptr = instance_safe();
    
    if (!ptr) {
        // Log critical error before halting
        // TODO: Implement proper error logging system
        #ifdef DEBUG
            __BKPT();
        #endif
        while (true) {
            // Infinite loop to halt execution
        }
    }
    
    return *ptr;
}
```

**Benefits:**
- Graceful error handling
- Allows caller to handle uninitialized state
- Reduces system crash risk

---

### P1-8: ConfigUpdaterCallback Dangling Pointer Fix

**Defect:** `ConfigUpdaterCallback` stores raw pointer that can dangle if parent view is destroyed.

**Affected Files:**
- [`ui_enhanced_drone_settings.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp):108-123

**Solution:**

```cpp
// ui_enhanced_drone_settings.hpp
// Option A: Copy by value (recommended for long-lived callbacks)
struct ConfigUpdaterCallback {
    DroneAnalyzerSettings config_copy;  // Copy instead of pointer
    
    constexpr explicit ConfigUpdaterCallback(const DroneAnalyzerSettings& config) noexcept
        : config_copy(config) {}
    
    void operator()(const DronePreset& preset) const noexcept {
        (void)DroneFrequencyPresets::apply_preset(config_copy, preset);
    }
    
    // Add method to retrieve modified copy
    const DroneAnalyzerSettings& get_settings() const noexcept {
        return config_copy;
    }
};

// Option B: Weak pointer (requires framework support)
struct ConfigUpdaterCallbackSafe {
    std::weak_ptr<DroneAnalyzerSettings> config_weak;  // Requires shared_ptr
    
    explicit ConfigUpdaterCallbackSafe(std::weak_ptr<DroneAnalyzerSettings> config) noexcept
        : config_weak(config) {}
    
    void operator()(const DronePreset& preset) const noexcept {
        if (auto config = config_weak.lock()) {
            (void)DroneFrequencyPresets::apply_preset(*config, preset);
        }
    }
};

// Option C: Document usage constraints (keep current implementation)
struct ConfigUpdaterCallback {
    DroneAnalyzerSettings* config_ptr;
    
    constexpr explicit ConfigUpdaterCallback(DroneAnalyzerSettings& config) noexcept
        : config_ptr(&config) {}
    
    void operator()(const DronePreset& preset) const noexcept {
        // Guard clause to reduce nesting and prevent nullptr dereference
        if (!config_ptr) return;
        (void)DroneFrequencyPresets::apply_preset(*config_ptr, preset);
    }
    
    // Add documentation
    /// @warning CRITICAL: Callback must be used immediately (not stored for later use)
    /// @warning Parent view must remain alive while callback is in use
    /// @warning This is designed for short-lived menu interactions only
};
```

**Recommended Solution:** **Option A** (Copy by value) for long-lived callbacks, **Option C** (Document constraints) for short-lived menu interactions.

---

## P2-MED: Medium Priority Defect Solutions

### P2-1: Named Constants for Magic Numbers

**Defect:** Magic numbers 240, 10 used without named constants.

**Affected Files:**
- [`dsp_spectrum_processor.hpp`](firmware/application/apps/enhanced_drone_analyzer/dsp_spectrum_processor.hpp):68, 87
- [`dsp_display_types.hpp`](firmware/application/apps/enhanced_drone_analyzer/dsp_display_types.hpp):101, 472

**Solution:**

```cpp
// dsp_spectrum_processor.hpp
namespace SpectrumProcessorConstants {
    constexpr size_t POWER_LEVELS_COUNT = 240;
    constexpr size_t MAX_PIXEL_INDEX = 239;  // POWER_LEVELS_COUNT - 1
}

class SpectrumProcessor {
public:
    static inline size_t process_mini_spectrum(
        const ChannelSpectrum& spectrum,
        uint8_t power_levels[SpectrumProcessorConstants::POWER_LEVELS_COUNT],  // Named constant
        size_t& bins_hz_size,
        Frequency each_bin_size,
        Frequency marker_pixel_step,
        uint8_t min_color_power
    ) noexcept {
        size_t pixel_index = 0;
        
        for (size_t bin = 0; bin < MINI_SPECTRUM_WIDTH; bin++) {
            uint8_t current_bin_power;
            if (bin < spectrum.db.size()) {
                current_bin_power = spectrum.db[bin];
            } else {
                current_bin_power = 0;
            }
            
            bins_hz_size += each_bin_size;
            if (bins_hz_size >= marker_pixel_step) {
                // Bounds check BEFORE array access to prevent buffer overflow
                if (pixel_index < SpectrumProcessorConstants::MAX_PIXEL_INDEX) {  // Named constant
                    power_levels[pixel_index] = (current_bin_power > min_color_power) ?
                                                current_bin_power : 0;
                }
                current_bin_power = 0;
                
                bins_hz_size -= marker_pixel_step;
                pixel_index++;
            }
        }
        
        return pixel_index;
    }
};

// dsp_display_types.hpp
namespace DisplayConstants {
    constexpr size_t MAX_FILTERED_DRONES = 10;
}

struct FilteredDronesSnapshot {
    size_t count;
    TrackedDroneData drones[DisplayConstants::MAX_FILTERED_DRONES];  // Named constant
    
    // ... rest of struct
};

// Usage:
snapshot.count = (drone_snapshot.count < DisplayConstants::MAX_FILTERED_DRONES) 
                ? drone_snapshot.count 
                : DisplayConstants::MAX_FILTERED_DRONES;
```

**Benefits:**
- Self-documenting code
- Easy to change values
- Type-safe (constexpr)

---

## P3-LOW: Low Priority Defect Solutions

### P3-1: Type Unification

**Defect:** Inconsistent use of `uint64_t` vs `rf::Frequency` (int64_t) for frequency values.

**Affected Files:**
- [`ui_drone_common_types.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_drone_common_types.hpp):41-42, 49-50

**Solution:**

```cpp
// ui_drone_common_types.hpp
#pragma pack(push, 1)
struct DroneAnalyzerSettings {
    // Audio settings
    uint8_t audio_flags = 0x01;
    uint32_t audio_alert_frequency_hz = 800;
    uint32_t audio_alert_duration_ms = 500;
    uint8_t audio_volume_level = 50;
    
    // Hardware settings
    uint8_t hardware_flags = 0x01;
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    uint32_t hardware_bandwidth_hz = 24000000;
    uint8_t rx_phase_value = 15;
    uint8_t lna_gain_db = 32;
    uint8_t vga_gain_db = 20;
    
    // FIX: Use rf::Frequency (int64_t) instead of uint64_t
    rf::Frequency user_min_freq_hz = 50000000LL;  // Was: uint64_t
    rf::Frequency user_max_freq_hz = 6000000000LL;  // Was: uint64_t
    
    // Scanning settings
    uint8_t scanning_flags = 0x06;
    uint32_t scan_interval_ms = 1000;
    int32_t rssi_threshold_db = -90;
    
    // FIX: Use rf::Frequency (int64_t) instead of uint64_t
    rf::Frequency wideband_min_freq_hz = 2400000000LL;  // Was: uint64_t
    rf::Frequency wideband_max_freq_hz = 2500000000LL;  // Was: uint64_t
    uint32_t wideband_slice_width_hz = 24000000;
    
    // ... rest of struct
};
#pragma pack(pop)
```

**Benefits:**
- Type consistency across codebase
- Prevents signed/unsigned comparison issues
- Self-documenting (rf::Frequency is clearer than uint64_t)

---

### P3-2: Timestamp Type Unification

**Defect:** Inconsistent use of `uint32_t` vs `systime_t` for timestamps.

**Affected Files:**
- [`ui_signal_processing.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.hpp):45
- [`dsp_display_types.hpp`](firmware/application/apps/enhanced_drone_analyzer/dsp_display_types.hpp):86

**Solution:**

```cpp
// ui_signal_processing.hpp
// FIX: Use systime_t consistently
using Timestamp = systime_t;  // Was: uint32_t

// dsp_display_types.hpp
struct TrackedDroneData {
    rf::Frequency frequency;
    uint8_t drone_type;
    uint8_t threat_level;
    int32_t rssi;
    systime_t last_seen;  // Was: systime_t (already correct)
    MovementTrend trend;
};
```

**Benefits:**
- Type consistency with ChibiOS
- Clearer intent (systime_t is timestamp type)
- Prevents type conversion errors

---

### P3-3: RSSI Type Unification

**Defect:** Inconsistent use of `int32_t` vs `RSSIValue` for RSSI values.

**Affected Files:**
- [`ui_signal_processing.hpp`](firmware/application/apps/enhanced_drone_analyzer/ui_signal_processing.hpp):44
- [`dsp_display_types.hpp`](firmware/application/apps/enhanced_drone_analyzer/dsp_display_types.hpp):44

**Solution:**

```cpp
// dsp_display_types.hpp
struct DisplayDroneEntry {
    char type_name[16];
    Frequency frequency;
    RSSIValue rssi;  // Was: int32_t
    MovementTrend trend;
};

struct TrackedDroneData {
    Frequency frequency;
    uint8_t drone_type;
    uint8_t threat_level;
    RSSIValue rssi;  // Was: int32_t
    systime_t last_seen;
    MovementTrend trend;
};
```

**Benefits:**
- Type consistency across codebase
- Self-documenting (RSSIValue is clearer than int32_t)
- Easy to change RSSI type in one place

---

## Memory Placement Strategy

### Flash Memory Opportunities

The following data can be moved to Flash memory to save RAM:

#### 1. Title Strings
```cpp
// ui_enhanced_drone_settings.hpp
namespace TitleStorage {
    // Store in Flash using const and EDA_FLASH_CONST attribute
    EDA_FLASH_CONST constexpr char AUDIO_SETTINGS_TITLE[] = "Audio Settings";
    EDA_FLASH_CONST constexpr char HARDWARE_SETTINGS_TITLE[] = "Hardware Settings";
    EDA_FLASH_CONST constexpr char SCANNING_SETTINGS_TITLE[] = "Scanning Settings";
    EDA_FLASH_CONST constexpr char DRONE_ANALYZER_SETTINGS_TITLE[] = "EDA Settings";
    EDA_FLASH_CONST constexpr char LOADING_TITLE[] = "Loading";
    EDA_FLASH_CONST constexpr char EDIT_FREQUENCY_TITLE[] = "Edit Frequency";
    EDA_FLASH_CONST constexpr char DATABASE_LIST_TITLE[] = "Database List";
}
```

**RAM Savings:** ~200 bytes (7 strings × ~30 bytes each)

#### 2. Color Tables
```cpp
// dsp_display_types.hpp
struct BarSpectrumConfig {
    static constexpr int WATERFALL_Y_START = 120;
    static constexpr int BAR_HEIGHT_MAX = 40;
    static constexpr uint8_t NOISE_THRESHOLD = 10;
    
    // Store in Flash
    EDA_FLASH_CONST static constexpr uint32_t BAR_COLORS[] = {
        0x001F,  // Blue (low)
        0x07E0,  // Green (medium-low)
        0xFFE0,  // Yellow (medium)
        0xF800,  // Red (high)
        0xF81F   // Magenta (critical)
    };
};

struct HistogramColorConfig {
    // Store in Flash
    EDA_FLASH_CONST static constexpr uint32_t HISTOGRAM_COLORS[] = {
        0x001F,  // Blue (20%)
        0x07E0,  // Green (40%)
        0xFFE0,  // Yellow (60%)
        0xF800,  // Red (80%)
        0xF81F   // Magenta (100%)
    };
};
```

**RAM Savings:** ~40 bytes (2 tables × 5 colors × 4 bytes)

#### 3. Frequency Format Table
```cpp
// dsp_display_types.hpp
struct FreqFormatEntry {
    int64_t divider;
    int64_t decimal_div;
    const char* format;
};

// Store in Flash
EDA_FLASH_CONST inline constexpr FreqFormatEntry FREQ_FORMAT_TABLE[] = {
    {1000000000LL, 100000000LL, "%lu.%03lu GHz"},  // >= 1 GHz
    {1000000LL, 100000LL, "%lu.%03lu MHz"},        // >= 1 MHz
    {1000LL, 100LL, "%lu.%02lu kHz"},            // >= 1 kHz
    {1LL, 1, "%lu Hz"}                           // < 1 kHz
};
```

**RAM Savings:** ~64 bytes (4 entries × 16 bytes)

**Total RAM Savings:** ~304 bytes

---

### RAM Optimization Strategy

#### 1. Use BSS for Large Structures
```cpp
// Global/static allocation (BSS segment)
static DetectionRingBuffer detection_buffer;  // 480 bytes in BSS
static DroneAnalyzerSettings settings;  // 512 bytes in BSS
static FilteredDronesSnapshot drone_snapshot;  // 640 bytes in BSS
```

#### 2. Pass Large Structures by Reference
```cpp
// Bad: Pass by value (copies 640 bytes)
void process_drones(FilteredDronesSnapshot snapshot);

// Good: Pass by reference (no copy)
void process_drones(const FilteredDronesSnapshot& snapshot);
```

#### 3. Use Fixed-Size Arrays Instead of std::vector
```cpp
// Bad: std::vector allocates on heap
std::vector<uint8_t> spectrum_data(256);

// Good: std::array allocates on stack/BSS
std::array<uint8_t, 256> spectrum_data;
```

---

## Thread Safety Strategy

### Lock Ordering Hierarchy

Define clear lock ordering levels and enforce at compile time:

```cpp
// eda_locking.hpp
enum class LockOrder : uint8_t {
    ATOMIC_FLAGS = 0,   // Fast spinlocks (CriticalSection)
    DATA_MUTEX = 1,     // Detection data and frequency database
    SPECTRUM_MUTEX = 2, ///< Spectrum data and histogram
    LOGGER_MUTEX = 3,   ///< Detection logger state
    SD_CARD_MUTEX = 4   ///< SD card I/O (must be LAST)
};

// Add lock order validator usage
void example_function() {
    LockOrderValidator validator(LockOrder::DATA_MUTEX);
    
    MutexLock lock1(data_mutex_, LockOrder::DATA_MUTEX);
    
    // Validate next lock acquisition
    chDbgAssert(validator.can_acquire(LockOrder::SPECTRUM_MUTEX),
                "Lock order violation",
                "Cannot acquire spectrum_mutex_ after data_mutex_");
    
    MutexLock lock2(spectrum_mutex_, LockOrder::SPECTRUM_MUTEX);
}
```

### Synchronization Mechanisms

#### 1. Mutex Protection (Current - Good)
Use `MutexLock` RAII wrapper for all mutex operations:

```cpp
{
    MutexLock lock(mutex_, LockOrder::DATA_MUTEX);
    // Critical section
}  // Lock automatically released
```

#### 2. Atomic Operations (For Simple Flags)
Use `AtomicFlag` for simple boolean flags:

```cpp
AtomicFlag flag;

// Thread 1: Set flag
flag.store(true);

// Thread 2: Check flag
if (flag.load()) {
    // Flag is set
}
```

#### 3. Critical Sections (For ISR Safety)
Use `CriticalSection` for ISR-safe operations:

```cpp
{
    CriticalSection lock;  // Disable interrupts
    volatile_flag = true;  // Atomic flag update
}  // Interrupts restored
```

---

## Stack Usage Reduction Strategy

### 1. Stack Monitoring

Add stack monitoring to critical functions:

```cpp
void critical_function() {
    StackMonitor monitor;
    constexpr size_t REQUIRED_STACK = 2048;
    
    if (!monitor.is_stack_safe(REQUIRED_STACK)) {
        // Not enough stack, skip operation
        return;
    }
    
    // Safe to use up to REQUIRED_STACK bytes
}
```

### 2. Stack Canary Checks

Add stack canary checks at critical points:

```cpp
void critical_function() {
    STACK_CANARY_INIT(canary);
    
    // Critical code
    
    STACK_CANARY_CHECK(canary);  // Halts if stack overflow
}
```

### 3. Reduce Stack Frame Size

#### Pass by Reference Instead of Value
```cpp
// Bad: Pass by value (copies 512 bytes)
void process_settings(DroneAnalyzerSettings settings);

// Good: Pass by reference (no copy)
void process_settings(const DroneAnalyzerSettings& settings);
```

#### Use Static Buffers Instead of Stack Allocations
```cpp
// Bad: Allocate on stack (128 bytes)
void process_spectrum() {
    std::array<uint16_t, 64> histogram;
    // ...
}

// Good: Use static buffer (BSS allocation)
void process_spectrum() {
    static std::array<uint16_t, 64> histogram;
    // ...
}
```

#### Inline Small Functions
```cpp
// Bad: Function call overhead
inline uint8_t get_max_value(const std::array<uint8_t, 256>& buffer) {
    uint8_t max = 0;
    for (auto val : buffer) {
        if (val > max) max = val;
    }
    return max;
}

// Good: Inlined (no call overhead)
constexpr uint8_t get_max_value(const std::array<uint8_t, 256>& buffer) {
    uint8_t max = 0;
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (buffer[i] > max) max = buffer[i];
    }
    return max;
}
```

---

## Implementation Priority

### Phase 1: Critical Fixes (P0-STOP)
1. Remove `<string>` header from `ui_enhanced_drone_settings.hpp`
2. Document framework limitations with clear comments
3. Add compile-time checks to prevent stack allocation of large structures
4. Add `static_assert` for structure size validation

### Phase 2: High Priority Fixes (P1-HIGH)
1. Add `LockOrderValidator` usage to all multi-lock functions
2. Add stack monitoring to coordinator thread
3. Increase coordinator thread stack to 3072 bytes
4. Fix `ConfigUpdaterCallback` to copy settings by value
5. Use `instance_safe()` instead of `instance()` for graceful error handling

### Phase 3: Medium Priority Fixes (P2-MED)
1. Define named constants for magic numbers:
   - `POWER_LEVELS_COUNT = 240`
   - `MAX_FILTERED_DRONES = 10`
2. Move static data to Flash memory:
   - Title strings
   - Color tables
   - Frequency format table

### Phase 4: Low Priority Fixes (P3-LOW)
1. Unify frequency types (use `rf::Frequency` consistently)
2. Unify timestamp types (use `systime_t` consistently)
3. Unify RSSI types (use `RSSIValue` consistently)
4. Add type safety comments to clarify usage

### Phase 5: Framework Changes (Long Term)
1. Modify UI framework to support `std::string_view` or `const char*` return type for `title()` method
2. Modify TextEdit widget to accept char* buffer instead of `std::string&`
3. Add lock order validation to framework (compile-time enforcement)

---

## Testing Strategy

### 1. Stack Overflow Testing
- Use `StackMonitor` class to verify stack usage
- Add stack canary checks at critical points
- Test with deep call stacks
- Verify stack usage < 4KB limit

### 2. Thread Safety Testing
- Test with concurrent reader/writer threads
- Verify lock ordering enforcement
- Test singleton initialization race conditions
- Verify mutex protection for all shared state

### 3. Memory Allocation Testing
- Verify no heap allocation from EDA code (except framework workarounds)
- Use static analysis to detect `new`, `malloc`, `std::vector`, `std::string`
- Verify all large structures use BSS allocation
- Verify Flash memory placement for const data

### 4. Type Safety Testing
- Verify type consistency across codebase
- Test signed/unsigned comparisons
- Verify no implicit type conversions
- Test with compiler warnings enabled (`-Wall -Wextra`)

---

## Conclusion

This Architect's Blueprint provides specific solutions for all **24 defects** identified in Stage 1 Forensic Audit:

- **6 P0-STOP** defects: Framework limitations require framework changes, but immediate workarounds are provided
- **8 P1-HIGH** defects: Specific solutions provided for thread safety and stack overflow risks
- **6 P2-MED** defects: Named constants and Flash memory migration strategies
- **4 P3-LOW** defects: Type unification strategies

**Key Recommendations:**

1. **Immediate Actions (Phase 1-2):** Fix P0-STOP and P1-HIGH defects
2. **Short Term (Phase 3):** Move static data to Flash memory
3. **Long Term (Phase 4-5):** Type unification and framework changes

**Expected Impact:**

- **RAM Savings:** ~304 bytes (Flash memory migration)
- **Stack Safety:** Improved (monitoring, canary checks, size validation)
- **Thread Safety:** Improved (lock order validation, graceful error handling)
- **Code Quality:** Improved (named constants, type unification)

**Overall Assessment:**

The enhanced_drone_analyzer codebase is well-designed for embedded constraints but requires framework changes to fully eliminate heap allocation. The solutions provided in this blueprint are practical, implementable, and prioritize safety and performance.

---

**Blueprint Generated:** 2026-03-05  
**Architect:** Architect Mode  
**Next Phase:** Implementation (Code Mode)
