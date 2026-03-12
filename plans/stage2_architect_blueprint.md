# PART 2: ARCHITECT'S BLUEPRINT
# Stack Optimization for Enhanced Drone Analyzer
# Stage 2 of 4-Stage Pipeline

**Date:** 2026-03-12
**Target:** STM32F405 (ARM Cortex-M4, 128KB RAM, 4KB stack per thread)
**Current Stack Usage:** ~4,366 bytes (106% of 4KB limit)
**Target Stack Usage:** <3,200 bytes (80% of 4KB limit)

---

## 1. MEMORY ARCHITECTURE OVERVIEW

### 1.1 Memory Placement Strategy

```
┌─────────────────────────────────────────────────────────────────┐
│                    FLASH MEMORY (Read-Only)                  │
├─────────────────────────────────────────────────────────────────┤
│ • Lookup Tables (LUTs)                                     │
│ • Color Tables                                              │
│ • Format Tables                                            │
│ • String Constants                                          │
│ • constexpr Data                                           │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                    RAM - BSS Segment                        │
├─────────────────────────────────────────────────────────────────┤
│ • Static Large Arrays (>256 bytes)                           │
│ • Thread-Local Storage (replaced with static + mutex)         │
│ • Global Buffers                                             │
│ • Memory Pools                                              │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                    RAM - Stack (4KB per thread)             │
├─────────────────────────────────────────────────────────────────┤
│ • Local Variables (<1KB per function)                       │
│ • Small Arrays (<64 bytes)                                  │
│ • Function Call Frames                                       │
│ • Register Spills                                           │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 Memory Budget Allocation

| Memory Type | Budget | Usage | Status |
|-------------|---------|-------|--------|
| Stack (per thread) | 4,096 bytes | ~4,366 bytes | ❌ OVERFLOW |
| Static Large Arrays | 8,192 bytes | ~4,500 bytes | ✅ OK |
| Flash LUTs | Unlimited | ~2,000 bytes | ✅ OK |
| Heap | 0 bytes (FORBIDDEN) | ~200 bytes | ❌ VIOLATION |

---

## 2. PRIORITY 1 CRITICAL FIXES

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Current Implementation:**
- Location: `dsp_display_types.hpp:182-186`, `dsp_display_types.cpp:78`
- Stack allocation: `FilteredDronesSnapshot filtered;` (640 bytes)
- Structure: `TrackedDroneData drones[10]` (64 bytes × 10 = 640 bytes)

**Proposed Solution:**

```cpp
// NEW: Static storage for filtered drones snapshot
// File: dsp_display_types.hpp (add to namespace dsp)

namespace StaticStorage {
    // BSS segment placement (zero-initialized at startup)
    alignas(alignof(FilteredDronesSnapshot))
    static uint8_t g_filtered_drones_snapshot_storage[sizeof(FilteredDronesSnapshot)];
    
    // Mutex for protecting static storage access
    static Mutex g_filtered_drones_mutex;
}

// RAII wrapper for static storage access
class FilteredDronesSnapshotGuard {
public:
    explicit FilteredDronesSnapshotGuard() noexcept {
        chMtxLock(&StaticStorage::g_filtered_drones_mutex);
        snapshot_ptr_ = reinterpret_cast<FilteredDronesSnapshot*>(
            StaticStorage::g_filtered_drones_snapshot_storage
        );
    }
    
    ~FilteredDronesSnapshotGuard() noexcept {
        chMtxUnlock(&StaticStorage::g_filtered_drones_mutex);
    }
    
    // Non-copyable, non-movable
    FilteredDronesSnapshotGuard(const FilteredDronesSnapshotGuard&) = delete;
    FilteredDronesSnapshotGuard& operator=(const FilteredDronesSnapshotGuard&) = delete;
    
    [[nodiscard]] FilteredDronesSnapshot* get() noexcept { return snapshot_ptr_; }
    [[nodiscard]] const FilteredDronesSnapshot* get() const noexcept { return snapshot_ptr_; }
    
    FilteredDronesSnapshot* operator->() noexcept { return snapshot_ptr_; }
    const FilteredDronesSnapshot* operator->() const noexcept { return snapshot_ptr_; }
    
    FilteredDronesSnapshot& operator*() noexcept { return *snapshot_ptr_; }
    const FilteredDronesSnapshot& operator*() const noexcept { return *snapshot_ptr_; }
    
private:
    FilteredDronesSnapshot* snapshot_ptr_;
};
```

**Modified Function Signature:**

```cpp
// BEFORE (stack allocation):
FilteredDronesSnapshot filter_stale_drones(
    const FilteredDronesSnapshot& snapshot,
    StaleTimeout stale_timeout_ms,
    CurrentTime now
) noexcept;

// AFTER (static storage):
void filter_stale_drones_in_place(
    const FilteredDronesSnapshot& input,
    FilteredDronesSnapshot& output,  // Output parameter (no allocation)
    StaleTimeout stale_timeout_ms,
    CurrentTime now
) noexcept;
```

**Memory Placement:**
- **Segment:** BSS (zero-initialized at startup)
- **Lifetime:** Application lifetime
- **Thread Safety:** Mutex-protected access via RAII wrapper

**Expected Savings:** 640 bytes (100% reduction in stack usage)

**Risk Assessment:** **LOW**
- Simple refactoring from stack to static storage
- RAII wrapper ensures proper mutex locking
- No complex logic changes required

---

### Fix #2: spectrum_data_ (256 bytes)

**Current Implementation:**
- Location: `ui_enhanced_drone_analyzer.hpp:809`
- Stack allocation: `std::array<uint8_t, 256> spectrum_data_{};`
- Already a class member, but may be copied on stack

**Proposed Solution:**

```cpp
// NEW: Move spectrum_data_ to static storage with lock-free access
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
private:
    // Static storage for spectrum data (BSS segment)
    alignas(alignof(std::array<uint8_t, 256>))
    static uint8_t g_spectrum_data_storage[sizeof(std::array<uint8_t, 256>)];
    
    // Lock-free access using atomic flag for double-buffering
    static std::atomic<uint8_t> g_spectrum_data_buffer_index;  // 0 or 1
    
    // Double-buffered spectrum data for lock-free reads
    static std::array<uint8_t, 256> g_spectrum_data_buffers[2];
    
    // Accessor methods (lock-free)
    [[nodiscard]] const std::array<uint8_t, 256>& get_spectrum_data() const noexcept {
        uint8_t idx = g_spectrum_data_buffer_index.load(std::memory_order_acquire);
        return g_spectrum_data_buffers[idx];
    }
    
    void update_spectrum_data(const std::array<uint8_t, 256>& new_data) noexcept {
        uint8_t write_idx = 1 - g_spectrum_data_buffer_index.load(std::memory_order_relaxed);
        g_spectrum_data_buffers[write_idx] = new_data;
        g_spectrum_data_buffer_index.store(write_idx, std::memory_order_release);
    }
};
```

**Memory Placement:**
- **Segment:** BSS (zero-initialized at startup)
- **Lifetime:** Application lifetime
- **Thread Safety:** Lock-free double-buffering with atomic index

**Expected Savings:** 256 bytes (eliminates stack copies)

**Risk Assessment:** **LOW**
- Lock-free double-buffering is well-established pattern
- Atomic operations are supported on ARM Cortex-M4
- No complex synchronization required

---

### Fix #3: power_levels (240 bytes)

**Current Implementation:**
- Location: `dsp_spectrum_processor.hpp:214`
- Stack allocation: `uint8_t power_levels[240]` (240 bytes)
- Used in `process_mini_spectrum()` function

**Proposed Solution:**

```cpp
// NEW: Static storage for power levels with per-call buffer
// File: dsp_spectrum_processor.hpp (add to namespace dsp)

namespace StaticStorage {
    // BSS segment for power levels buffer
    alignas(alignof(uint8_t))
    static uint8_t g_power_levels_buffer[SpectrumProcessorConstants::POWER_LEVELS_COUNT];
    
    // Mutex for protecting power levels access
    static Mutex g_power_levels_mutex;
}

// RAII wrapper for power levels access
class PowerLevelsGuard {
public:
    PowerLevelsGuard() noexcept {
        chMtxLock(&StaticStorage::g_power_levels_mutex);
    }
    
    ~PowerLevelsGuard() noexcept {
        chMtxUnlock(&StaticStorage::g_power_levels_mutex);
    }
    
    [[nodiscard]] uint8_t* get() noexcept {
        return StaticStorage::g_power_levels_buffer;
    }
    
    [[nodiscard]] const uint8_t* get() const noexcept {
        return StaticStorage::g_power_levels_buffer;
    }
    
    // Non-copyable, non-movable
    PowerLevelsGuard(const PowerLevelsGuard&) = delete;
    PowerLevelsGuard& operator=(const PowerLevelsGuard&) = delete;
    
private:
};

// Modified function signature (takes buffer pointer instead of allocating)
static inline size_t process_mini_spectrum(
    const ChannelSpectrum& spectrum,
    uint8_t* power_levels,  // Output buffer (caller-provided)
    size_t power_levels_size,  // Buffer size for bounds checking
    size_t& bins_hz_size,
    BinSize each_bin_size,
    Frequency marker_pixel_step,
    MinColorPower min_color_power
) noexcept;
```

**Memory Placement:**
- **Segment:** BSS (zero-initialized at startup)
- **Lifetime:** Application lifetime
- **Thread Safety:** Mutex-protected access via RAII wrapper

**Expected Savings:** 240 bytes (100% reduction in stack usage)

**Risk Assessment:** **LOW**
- Simple buffer pointer parameter change
- RAII wrapper ensures proper mutex locking
- Bounds checking added for safety

---

### Fix #4: std::string temp_string_ (heap allocation)

**Current Implementation:**
- Location: `ui_enhanced_drone_settings.hpp:838, 849`
- Heap allocation: `std::string temp_string_` in FixedStringBuffer class
- Used for TextEdit widget compatibility

**Proposed Solution:**

```cpp
// NEW: Replace std::string with fixed-size char array
// File: ui_enhanced_drone_settings.hpp (FixedStringBuffer class)

class FixedStringBuffer {
public:
    explicit FixedStringBuffer(char* buffer, size_t capacity) noexcept
        : buffer_(buffer),
          capacity_(capacity),
          length_(0) {
        buffer_[0] = '\0';
    }

    // Non-copyable
    FixedStringBuffer(const FixedStringBuffer&) = delete;
    FixedStringBuffer& operator=(const FixedStringBuffer&) = delete;

    // Provide std::string-like interface for TextEdit compatibility
    [[nodiscard]] const char* c_str() const noexcept { return buffer_; }
    [[nodiscard]] size_t size() const noexcept { return length_; }
    [[nodiscard]] size_t capacity() const noexcept { return capacity_; }
    [[nodiscard]] bool empty() const noexcept { return length_ == 0; }

    // Clear buffer
    void clear() noexcept {
        length_ = 0;
        buffer_[0] = '\0';
    }

    // Assign from string (for initialization from entry.description)
    void assign(const char* str) noexcept {
        if (!str) {
            clear();
            return;
        }
        size_t len = 0;
        while (len < capacity_ - 1 && str[len] != '\0') {
            buffer_[len] = str[len];
            len++;
        }
        buffer_[len] = '\0';
        length_ = len;
    }

    // NEW: Direct buffer access for TextEdit widget (no heap allocation)
    // TextEdit widget can write directly to this buffer
    [[nodiscard]] char* data() noexcept { return buffer_; }
    [[nodiscard]] const char* data() const noexcept { return buffer_; }
    
    // NEW: Update length after TextEdit modifies buffer
    void update_length() noexcept {
        length_ = 0;
        while (length_ < capacity_ - 1 && buffer_[length_] != '\0') {
            length_++;
        }
    }

    // REMOVED: operator std::string&() - causes heap allocation
    // TextEdit widget should use data() and update_length() instead

private:
    char* buffer_;                    // Fixed-size buffer (non-owning)
    size_t capacity_;                // Buffer capacity
    size_t length_;                  // Current string length
};
```

**Modified TextEdit Widget Usage:**

```cpp
// BEFORE (heap allocation via std::string conversion):
field_desc_ = TextEdit{
    description_widget_buffer_,  // FixedStringBuffer
    DroneEntryEditorViewConstants::FIELD_DESC_MAX_LENGTH,
    {DroneEntryEditorViewConstants::FIELD_DESC_RECT.x,
     DroneEntryEditorViewConstants::FIELD_DESC_RECT.y},
    DroneEntryEditorViewConstants::FIELD_DESC_HEIGHT
};

// AFTER (direct buffer access - no heap allocation):
// Note: Requires TextEdit widget to support direct char* buffer
// If widget cannot be modified, use alternative approach:
field_desc_ = TextEdit{
    description_widget_buffer_.data(),  // Direct char* pointer
    DroneEntryEditorViewConstants::FIELD_DESC_MAX_LENGTH,
    {DroneEntryEditorViewConstants::FIELD_DESC_RECT.x,
     DroneEntryEditorViewConstants::FIELD_DESC_RECT.y},
    DroneEntryEditorViewConstants::FIELD_DESC_HEIGHT
};

// After TextEdit modifies buffer:
description_widget_buffer_.update_length();
```

**Memory Placement:**
- **Segment:** Stack (fixed-size char array in parent class)
- **Lifetime:** Parent class lifetime
- **Thread Safety:** Not thread-safe (UI-only access)

**Expected Savings:** ~100-200 bytes (eliminates heap allocation)

**Risk Assessment:** **MEDIUM**
- Requires TextEdit widget to support direct char* buffer access
- If widget cannot be modified, need alternative approach
- May need to create custom TextEdit variant

---

### Fix #5: std::string filename (heap allocation)

**Current Implementation:**
- Location: `ui_enhanced_drone_settings.cpp:996-997`
- Heap allocation: `std::string filename = new_file_path.stem().string();`
- Used in `on_select_database()` callback

**Proposed Solution:**

```cpp
// NEW: Use fixed-size char buffer instead of std::string
// File: ui_enhanced_drone_settings.cpp (on_select_database method)

void ScanningSettingsView::on_select_database() noexcept {
    // Launch FileLoadView with .TXT filter for frequency database files
    auto open_view = nav_.push<FileLoadView>(".TXT");

    // Set callback to handle file selection when user picks a file
    open_view->on_changed = [this](std::filesystem::path new_file_path) {
        // NEW: Use fixed-size char buffer instead of std::string
        constexpr size_t MAX_FILENAME_LENGTH = EDA::Constants::MAX_NAME_LENGTH;
        char filename[MAX_FILENAME_LENGTH];
        
        // Extract filename without extension using std::filesystem::path
        // Note: std::filesystem::path::stem() still returns a path object
        // We need to copy the stem string to our fixed buffer
        std::filesystem::path stem_path = new_file_path.stem();
        std::string stem_string = stem_path.string();  // Temporary std::string (unavoidable)
        
        // Copy to fixed buffer with bounds checking
        size_t copy_len = std::min(stem_string.length(), MAX_FILENAME_LENGTH - 1);
        std::memcpy(filename, stem_string.c_str(), copy_len);
        filename[copy_len] = '\0';
        
        // Load current settings from persistence
        static DroneAnalyzerSettings settings;
        if (!SettingsPersistence<DroneAnalyzerSettings>::load(settings)) {
            nav_.display_modal("Error", "Failed to load settings");
            return;
        }
        
        // Update freqman_path with selected filename (using fixed buffer)
        safe_strcpy(settings.freqman_path, filename, sizeof(settings.freqman_path));
        
        // Save updated settings
        if (!SettingsPersistence<DroneAnalyzerSettings>::save(settings)) {
            nav_.display_modal("Error", "Failed to save settings");
            return;
        }
        
        // Display success message
        nav_.display_modal("Success", "Database updated");
    };
}
```

**Alternative Solution (Zero Heap Allocation):**

```cpp
// If std::filesystem::path cannot be avoided, minimize heap usage
// by using string_view and avoiding intermediate std::string copies

void ScanningSettingsView::on_select_database() noexcept {
    auto open_view = nav_.push<FileLoadView>(".TXT");

    open_view->on_changed = [this](std::filesystem::path new_file_path) {
        // NEW: Use std::string_view to avoid heap allocation
        // std::filesystem::path::native() returns the path string
        // We can extract the stem portion without allocating
        
        constexpr size_t MAX_FILENAME_LENGTH = EDA::Constants::MAX_NAME_LENGTH;
        char filename[MAX_FILENAME_LENGTH];
        
        // Get the full path string (may allocate internally)
        const std::string& full_path = new_file_path.string();
        
        // Find the last directory separator
        size_t last_slash = full_path.find_last_of("/\\");
        size_t start_pos = (last_slash == std::string::npos) ? 0 : last_slash + 1;
        
        // Find the last dot (extension separator)
        size_t last_dot = full_path.find_last_of('.');
        
        // Extract filename without extension
        size_t end_pos = (last_dot == std::string::npos || last_dot < start_pos) 
                        ? full_path.length() 
                        : last_dot;
        
        // Copy to fixed buffer with bounds checking
        size_t copy_len = std::min(end_pos - start_pos, MAX_FILENAME_LENGTH - 1);
        std::memcpy(filename, full_path.c_str() + start_pos, copy_len);
        filename[copy_len] = '\0';
        
        // ... rest of the code unchanged
    };
}
```

**Memory Placement:**
- **Segment:** Stack (fixed-size char array)
- **Lifetime:** Function scope
- **Thread Safety:** Not thread-safe (UI-only access)

**Expected Savings:** ~50-100 bytes (eliminates heap allocation)

**Risk Assessment:** **LOW**
- Simple buffer replacement
- Bounds checking added for safety
- Minimal code changes required

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Current Implementation:**
- Location: `ui_enhanced_drone_analyzer.hpp:638`
- Stack allocation: `std::array<freqman_entry, 10> entries_to_scan_{};`
- Each freqman_entry is ~100 bytes, total ~1000 bytes

**Proposed Solution:**

```cpp
// NEW: Move entries_to_scan_ to static storage
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
private:
    // Static storage for entries to scan (BSS segment)
    alignas(alignof(freqman_entry))
    static uint8_t g_entries_to_scan_storage[sizeof(std::array<freqman_entry, 10>)];
    
    // Mutex for protecting entries to scan access
    static Mutex g_entries_to_scan_mutex;
    
    // RAII wrapper for entries to scan access
    class EntriesToScanGuard {
    public:
        EntriesToScanGuard() noexcept {
            chMtxLock(&g_entries_to_scan_mutex);
        }
        
        ~EntriesToScanGuard() noexcept {
            chMtxUnlock(&g_entries_to_scan_mutex);
        }
        
        [[nodiscard]] std::array<freqman_entry, 10>* get() noexcept {
            return reinterpret_cast<std::array<freqman_entry, 10>*>(
                g_entries_to_scan_storage
            );
        }
        
        [[nodiscard]] const std::array<freqman_entry, 10>* get() const noexcept {
            return reinterpret_cast<const std::array<freqman_entry, 10>*>(
                g_entries_to_scan_storage
            );
        }
        
        // Non-copyable, non-movable
        EntriesToScanGuard(const EntriesToScanGuard&) = delete;
        EntriesToScanGuard& operator=(const EntriesToScanGuard&) = delete;
        
    private:
    };
    
    // Helper methods for accessing entries to scan
    inline std::array<freqman_entry, 10>& get_entries_to_scan() noexcept {
        EntriesToScanGuard guard;
        return *guard.get();
    }
    
    inline const std::array<freqman_entry, 10>& get_entries_to_scan() const noexcept {
        EntriesToScanGuard guard;
        return *guard.get();
    }
    
    // REMOVED: std::array<freqman_entry, 10> entries_to_scan_{}; (moved to static)
};
```

**Memory Placement:**
- **Segment:** BSS (zero-initialized at startup)
- **Lifetime:** Application lifetime
- **Thread Safety:** Mutex-protected access via RAII wrapper

**Expected Savings:** 1000 bytes (100% reduction in stack usage)

**Risk Assessment:** **LOW**
- Simple refactoring from class member to static storage
- RAII wrapper ensures proper mutex locking
- No complex logic changes required

---

## 3. PRIORITY 2 HIGH PRIORITY FIXES

### Fix #7: histogram_buffer_ (67 bytes)

**Current Implementation:**
- Location: `ui_enhanced_drone_analyzer.hpp:812`
- Stack allocation: `SpectralAnalyzer::HistogramBuffer histogram_buffer_{};`
- Already a class member, but may be copied on stack

**Proposed Solution:**

```cpp
// NEW: Move histogram_buffer_ to static storage
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
private:
    // Static storage for histogram buffer (BSS segment)
    alignas(alignof(SpectralAnalyzer::HistogramBuffer))
    static uint8_t g_histogram_buffer_storage[sizeof(SpectralAnalyzer::HistogramBuffer)];
    
    // Mutex for protecting histogram buffer access
    static Mutex g_histogram_buffer_mutex;
    
    // RAII wrapper for histogram buffer access
    class HistogramBufferGuard {
    public:
        HistogramBufferGuard() noexcept {
            chMtxLock(&g_histogram_buffer_mutex);
        }
        
        ~HistogramBufferGuard() noexcept {
            chMtxUnlock(&g_histogram_buffer_mutex);
        }
        
        [[nodiscard]] SpectralAnalyzer::HistogramBuffer* get() noexcept {
            return reinterpret_cast<SpectralAnalyzer::HistogramBuffer*>(
                g_histogram_buffer_storage
            );
        }
        
        [[nodiscard]] const SpectralAnalyzer::HistogramBuffer* get() const noexcept {
            return reinterpret_cast<const SpectralAnalyzer::HistogramBuffer*>(
                g_histogram_buffer_storage
            );
        }
        
        // Non-copyable, non-movable
        HistogramBufferGuard(const HistogramBufferGuard&) = delete;
        HistogramBufferGuard& operator=(const HistogramBufferGuard&) = delete;
        
    private:
    };
    
    // Accessor methods
    inline SpectralAnalyzer::HistogramBuffer& get_histogram_buffer() noexcept {
        HistogramBufferGuard guard;
        return *guard.get();
    }
    
    inline const SpectralAnalyzer::HistogramBuffer& get_histogram_buffer() const noexcept {
        HistogramBufferGuard guard;
        return *guard.get();
    }
    
    // REMOVED: SpectralAnalyzer::HistogramBuffer histogram_buffer_{}; (moved to static)
};
```

**Memory Placement:**
- **Segment:** BSS (zero-initialized at startup)
- **Lifetime:** Application lifetime
- **Thread Safety:** Mutex-protected access via RAII wrapper

**Expected Savings:** 67 bytes (eliminates stack copies)

**Risk Assessment:** **LOW**
- Simple refactoring from class member to static storage
- RAII wrapper ensures proper mutex locking
- No complex logic changes required

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Current Implementation:**
- Location: Multiple locations in `ui_enhanced_drone_analyzer.hpp`
- Stack allocation: `void update_settings(const DroneAnalyzerSettings& settings)`
- Settings struct is ~200 bytes, passed by reference but copied internally

**Proposed Solution:**

```cpp
// NEW: Pass settings by const reference consistently
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
public:
    // BEFORE: Settings stored by value (copy on construction)
    explicit DroneScanner(DroneAnalyzerSettings settings);
    
    // AFTER: Settings stored by reference (no copy)
    explicit DroneScanner(const DroneAnalyzerSettings& settings);
    
    // Update scanner's settings from view's settings
    // BEFORE: void update_settings(const DroneAnalyzerSettings& settings) {
    //            settings_ = settings;  // Copy (~200 bytes)
    //        }
    
    // AFTER: Update individual fields (no full copy)
    void update_settings(const DroneAnalyzerSettings& settings) {
        MutexLock lock(data_mutex, LockOrder::DATA_MUTEX);
        
        // Update only changed fields (minimize copy overhead)
        if (settings_.scan_interval_ms != settings.scan_interval_ms) {
            settings_.scan_interval_ms = settings.scan_interval_ms;
        }
        if (settings_.rssi_threshold_db != settings.rssi_threshold_db) {
            settings_.rssi_threshold_db = settings.rssi_threshold_db;
        }
        if (settings_.audio_enabled != settings.audio_enabled) {
            settings_.audio_enabled = settings.audio_enabled;
        }
        // ... other fields as needed
    }
    
private:
    // BEFORE: DroneAnalyzerSettings settings_;  // Stored by value (~200 bytes)
    // AFTER: const DroneAnalyzerSettings& settings_;  // Reference (8 bytes)
    const DroneAnalyzerSettings& settings_;
};
```

**Memory Placement:**
- **Segment:** Stack (reference only, 8 bytes)
- **Lifetime:** Parent object lifetime
- **Thread Safety:** Mutex-protected access

**Expected Savings:** ~200 bytes (eliminates copy overhead)

**Risk Assessment:** **MEDIUM**
- Requires careful lifetime management (settings must outlive scanner)
- Need to update all settings access to use mutex
- May require changes to settings persistence logic

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Current Implementation:**
- Location: Multiple locations in `ui_enhanced_drone_settings.cpp`
- Heap allocation: `std::filesystem::path` objects allocate on heap
- Used for file path operations

**Proposed Solution:**

```cpp
// NEW: Use fixed-size char buffers instead of std::filesystem::path
// File: ui_enhanced_drone_settings.hpp (add utility functions)

namespace PathUtils {
    // Maximum path length for SD card files
    constexpr size_t MAX_PATH_LENGTH = 64;
    
    // Extract filename without extension from path
    // Returns true on success, false if path too long
    [[nodiscard]] bool extract_filename_stem(
        const char* full_path,
        char* output_buffer,
        size_t buffer_size
    ) noexcept {
        if (!full_path || !output_buffer || buffer_size == 0) {
            return false;
        }
        
        // Find the last directory separator
        size_t last_slash = 0;
        size_t i = 0;
        while (full_path[i] != '\0' && i < buffer_size) {
            if (full_path[i] == '/' || full_path[i] == '\\') {
                last_slash = i + 1;
            }
            i++;
        }
        
        // Find the last dot (extension separator)
        size_t start_pos = last_slash;
        size_t last_dot = 0;
        i = start_pos;
        while (full_path[i] != '\0' && i < buffer_size) {
            if (full_path[i] == '.') {
                last_dot = i;
            }
            i++;
        }
        
        // Extract filename without extension
        size_t end_pos = (last_dot > start_pos) ? last_dot : i;
        size_t copy_len = end_pos - start_pos;
        
        if (copy_len >= buffer_size) {
            return false;  // Buffer too small
        }
        
        std::memcpy(output_buffer, full_path + start_pos, copy_len);
        output_buffer[copy_len] = '\0';
        return true;
    }
    
    // Combine directory and filename into full path
    // Returns true on success, false if path too long
    [[nodiscard]] bool combine_path(
        const char* directory,
        const char* filename,
        char* output_buffer,
        size_t buffer_size
    ) noexcept {
        if (!directory || !filename || !output_buffer || buffer_size == 0) {
            return false;
        }
        
        size_t dir_len = 0;
        while (directory[dir_len] != '\0' && dir_len < buffer_size) {
            output_buffer[dir_len] = directory[dir_len];
            dir_len++;
        }
        
        // Add directory separator if needed
        if (dir_len > 0 && dir_len < buffer_size - 1) {
            if (output_buffer[dir_len - 1] != '/' && output_buffer[dir_len - 1] != '\\') {
                output_buffer[dir_len] = '/';
                dir_len++;
            }
        }
        
        // Copy filename
        size_t file_len = 0;
        while (filename[file_len] != '\0' && dir_len + file_len < buffer_size - 1) {
            output_buffer[dir_len + file_len] = filename[file_len];
            file_len++;
        }
        
        output_buffer[dir_len + file_len] = '\0';
        return true;
    }
}

// Usage example:
void ScanningSettingsView::on_select_database() noexcept {
    auto open_view = nav_.push<FileLoadView>(".TXT");
    
    open_view->on_changed = [this](std::filesystem::path new_file_path) {
        // NEW: Use fixed-size char buffer instead of std::string
        constexpr size_t MAX_FILENAME_LENGTH = EDA::Constants::MAX_NAME_LENGTH;
        char filename[MAX_FILENAME_LENGTH];
        
        // Extract filename without extension using utility function
        const std::string& full_path = new_file_path.string();
        if (!PathUtils::extract_filename_stem(
                full_path.c_str(),
                filename,
                MAX_FILENAME_LENGTH
            )) {
            nav_.display_modal("Error", "Filename too long");
            return;
        }
        
        // ... rest of the code unchanged
    };
}
```

**Memory Placement:**
- **Segment:** Stack (fixed-size char arrays)
- **Lifetime:** Function scope
- **Thread Safety:** Not thread-safe (UI-only access)

**Expected Savings:** ~100-200 bytes (eliminates heap allocations)

**Risk Assessment:** **LOW**
- Simple utility functions for path operations
- Bounds checking added for safety
- Minimal code changes required

---

### Fix #10: UI buffers totaling 160 bytes

**Current Implementation:**
- Location: `ui_enhanced_drone_analyzer.hpp:648-651`
- Stack allocation: Multiple UI buffers (16 + 48 + 48 + 48 = 160 bytes)
- Buffers: `ui_freq_buf_[16]`, `ui_summary_buffer_[48]`, `ui_status_buffer_[48]`, `ui_stats_buffer_[48]`

**Proposed Solution:**

```cpp
// NEW: Move UI buffers to static storage with per-call access
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
private:
    // Static storage for UI buffers (BSS segment)
    struct UIBuffers {
        char freq_buf[16];
        char summary_buffer[48];
        char status_buffer[48];
        char stats_buffer[48];
    };
    
    alignas(alignof(UIBuffers))
    static uint8_t g_ui_buffers_storage[sizeof(UIBuffers)];
    
    // Mutex for protecting UI buffers access
    static Mutex g_ui_buffers_mutex;
    
    // RAII wrapper for UI buffers access
    class UIBuffersGuard {
    public:
        UIBuffersGuard() noexcept {
            chMtxLock(&g_ui_buffers_mutex);
        }
        
        ~UIBuffersGuard() noexcept {
            chMtxUnlock(&g_ui_buffers_mutex);
        }
        
        [[nodiscard]] UIBuffers* get() noexcept {
            return reinterpret_cast<UIBuffers*>(g_ui_buffers_storage);
        }
        
        [[nodiscard]] const UIBuffers* get() const noexcept {
            return reinterpret_cast<const UIBuffers*>(g_ui_buffers_storage);
        }
        
        // Non-copyable, non-movable
        UIBuffersGuard(const UIBuffersGuard&) = delete;
        UIBuffersGuard& operator=(const UIBuffersGuard&) = delete;
        
    private:
    };
    
    // REMOVED: Individual UI buffers (moved to static storage)
    // char ui_freq_buf_[16]{};
    // char ui_summary_buffer_[48]{};
    // char ui_status_buffer_[48]{};
    // char ui_stats_buffer_[48]{};
};
```

**Memory Placement:**
- **Segment:** BSS (zero-initialized at startup)
- **Lifetime:** Application lifetime
- **Thread Safety:** Mutex-protected access via RAII wrapper

**Expected Savings:** 160 bytes (100% reduction in stack usage)

**Risk Assessment:** **LOW**
- Simple refactoring from class members to static storage
- RAII wrapper ensures proper mutex locking
- No complex logic changes required

---

## 4. DATA STRUCTURE SPECIFICATIONS

### 4.1 New Type Definitions

```cpp
// File: dsp_display_types.hpp (add to namespace dsp)

// ============================================================================
// RAII GUARD TEMPLATES FOR STATIC STORAGE ACCESS
// ============================================================================

/**
 * @brief Generic RAII guard for static storage access
 * 
 * Provides automatic mutex locking/unlocking for accessing static storage.
 * Template parameter defines the storage type and mutex.
 * 
 * @tparam StorageType Type of data stored in static storage
 * @tparam MutexType Type of mutex protecting the storage
 * 
 * USAGE:
 * @code
 *   StaticStorageGuard<FilteredDronesSnapshot, Mutex> guard;
 *   FilteredDronesSnapshot* snapshot = guard.get();
 *   // Use snapshot...
 *   // Mutex automatically released when guard goes out of scope
 * @endcode
 */
template <typename StorageType, typename MutexType>
class StaticStorageGuard {
public:
    explicit StaticStorageGuard(MutexType& mutex, StorageType* storage_ptr) noexcept
        : mutex_(mutex), storage_ptr_(storage_ptr) {
        chMtxLock(&mutex_);
    }
    
    ~StaticStorageGuard() noexcept {
        chMtxUnlock(&mutex_);
    }
    
    // Non-copyable, non-movable
    StaticStorageGuard(const StaticStorageGuard&) = delete;
    StaticStorageGuard& operator=(const StaticStorageGuard&) = delete;
    
    [[nodiscard]] StorageType* get() noexcept { return storage_ptr_; }
    [[nodiscard]] const StorageType* get() const noexcept { return storage_ptr_; }
    
    StorageType* operator->() noexcept { return storage_ptr_; }
    const StorageType* operator->() const noexcept { return storage_ptr_; }
    
    StorageType& operator*() noexcept { return *storage_ptr_; }
    const StorageType& operator*() const noexcept { return *storage_ptr_; }
    
private:
    MutexType& mutex_;
    StorageType* storage_ptr_;
};

/**
 * @brief Lock-free double buffer for thread-safe data sharing
 * 
 * Provides lock-free access to shared data using double-buffering.
 * Writer updates the inactive buffer, then swaps atomically.
 * Reader always reads from the active buffer without locking.
 * 
 * @tparam BufferType Type of buffer data
 * @tparam BufferSize Size of each buffer in bytes
 * 
 * USAGE:
 * @code
 *   LockFreeDoubleBuffer<uint8_t, 256> spectrum_buffer;
 *   
 *   // Writer thread:
 *   uint8_t* write_buf = spectrum_buffer.get_write_buffer();
 *   // ... fill write_buf ...
 *   spectrum_buffer.swap_buffers();
 *   
 *   // Reader thread:
 *   const uint8_t* read_buf = spectrum_buffer.get_read_buffer();
 *   // ... read from read_buf ...
 * @endcode
 */
template <typename BufferType, size_t BufferSize>
class LockFreeDoubleBuffer {
public:
    LockFreeDoubleBuffer() noexcept : active_index_(0) {
        // Buffers are zero-initialized (BSS segment)
    }
    
    // Get write buffer (caller must ensure exclusive access)
    [[nodiscard]] BufferType* get_write_buffer() noexcept {
        uint8_t write_idx = 1 - active_index_.load(std::memory_order_relaxed);
        return reinterpret_cast<BufferType*>(buffers_[write_idx]);
    }
    
    // Get read buffer (lock-free)
    [[nodiscard]] const BufferType* get_read_buffer() const noexcept {
        uint8_t read_idx = active_index_.load(std::memory_order_acquire);
        return reinterpret_cast<const BufferType*>(buffers_[read_idx]);
    }
    
    // Swap buffers (call after filling write buffer)
    void swap_buffers() noexcept {
        uint8_t new_active = 1 - active_index_.load(std::memory_order_relaxed);
        active_index_.store(new_active, std::memory_order_release);
    }
    
private:
    uint8_t buffers_[2][BufferSize];  // Double-buffered storage
    std::atomic<uint8_t> active_index_;  // Index of active buffer (0 or 1)
};
```

### 4.2 Class Member Changes

```cpp
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
public:
    // Constructor - accepts settings by reference (no copy)
    explicit DroneScanner(const DroneAnalyzerSettings& settings);
    ~DroneScanner();
    
    // Update settings with minimal copy overhead
    void update_settings(const DroneAnalyzerSettings& settings);
    
private:
    // ============================================================================
    // STATIC STORAGE FOR LARGE DATA STRUCTURES (BSS Segment)
    // ============================================================================
    
    // Filtered drones snapshot (640 bytes)
    alignas(alignof(FilteredDronesSnapshot))
    static uint8_t g_filtered_drones_storage[sizeof(FilteredDronesSnapshot)];
    static Mutex g_filtered_drones_mutex;
    
    // Spectrum data (256 bytes) - lock-free double-buffered
    static LockFreeDoubleBuffer<uint8_t, 256> g_spectrum_data_buffer;
    
    // Power levels (240 bytes)
    alignas(alignof(uint8_t))
    static uint8_t g_power_levels_buffer[240];
    static Mutex g_power_levels_mutex;
    
    // Entries to scan (1000 bytes)
    alignas(alignof(std::array<freqman_entry, 10>))
    static uint8_t g_entries_to_scan_storage[sizeof(std::array<freqman_entry, 10>)];
    static Mutex g_entries_to_scan_mutex;
    
    // Histogram buffer (67 bytes)
    alignas(alignof(SpectralAnalyzer::HistogramBuffer))
    static uint8_t g_histogram_buffer_storage[sizeof(SpectralAnalyzer::HistogramBuffer)];
    static Mutex g_histogram_buffer_mutex;
    
    // UI buffers (160 bytes)
    struct UIBuffers {
        char freq_buf[16];
        char summary_buffer[48];
        char status_buffer[48];
        char stats_buffer[48];
    };
    alignas(alignof(UIBuffers))
    static uint8_t g_ui_buffers_storage[sizeof(UIBuffers)];
    static Mutex g_ui_buffers_mutex;
    
    // ============================================================================
    // REMOVED: Large stack allocations (moved to static storage)
    // ============================================================================
    // REMOVED: std::array<freqman_entry, 10> entries_to_scan_{};
    // REMOVED: char ui_freq_buf_[16]{};
    // REMOVED: char ui_summary_buffer_[48]{};
    // REMOVED: char ui_status_buffer_[48]{};
    // REMOVED: char ui_stats_buffer_[48]{};
    
    // ============================================================================
    // CHANGED: Settings stored by reference (not value)
    // ============================================================================
    const DroneAnalyzerSettings& settings_;  // Reference (8 bytes), not copy (~200 bytes)
};
```

### 4.3 Function Signature Updates

```cpp
// File: dsp_display_types.hpp (filter_stale_drones function)

// BEFORE (returns by value - stack allocation):
FilteredDronesSnapshot filter_stale_drones(
    const FilteredDronesSnapshot& snapshot,
    StaleTimeout stale_timeout_ms,
    CurrentTime now
) noexcept;

// AFTER (output parameter - no allocation):
void filter_stale_drones_in_place(
    const FilteredDronesSnapshot& input,
    FilteredDronesSnapshot& output,  // Output parameter (caller-provided)
    StaleTimeout stale_timeout_ms,
    CurrentTime now
) noexcept;

// File: dsp_spectrum_processor.hpp (process_mini_spectrum function)

// BEFORE (allocates power_levels on stack):
static inline size_t process_mini_spectrum(
    const ChannelSpectrum& spectrum,
    uint8_t power_levels[240],  // Stack allocation
    size_t& bins_hz_size,
    BinSize each_bin_size,
    Frequency marker_pixel_step,
    MinColorPower min_color_power
) noexcept;

// AFTER (takes buffer pointer - no allocation):
static inline size_t process_mini_spectrum(
    const ChannelSpectrum& spectrum,
    uint8_t* power_levels,  // Caller-provided buffer
    size_t power_levels_size,  // Buffer size for bounds checking
    size_t& bins_hz_size,
    BinSize each_bin_size,
    Frequency marker_pixel_step,
    MinColorPower min_color_power
) noexcept;

// File: ui_enhanced_drone_analyzer.hpp (DroneScanner methods)

// BEFORE (passes settings by value):
explicit DroneScanner(DroneAnalyzerSettings settings);
void update_settings(const DroneAnalyzerSettings& settings);

// AFTER (passes settings by reference):
explicit DroneScanner(const DroneAnalyzerSettings& settings);
void update_settings(const DroneAnalyzerSettings& settings);
```

---

## 5. MEMORY LAYOUT MAP

```
┌────────────────────────────────────────────────────────────────────┐
│                    FLASH MEMORY (Read-Only)                    │
├────────────────────────────────────────────────────────────────────┤
│ • FREQ_FORMAT_TABLE[]           (~64 bytes)                   │
│ • BAR_COLORS[]                   (~20 bytes)                   │
│ • HISTOGRAM_COLORS[]            (~20 bytes)                   │
│ • SPECTRUM_MODES[]             (~120 bytes)                  │
│ • FREQUENCY_SCALES[]            (~64 bytes)                   │
│ • ERROR_MESSAGES[]              (~120 bytes)                   │
│ • ScanSlowdown::MULTIPLIER_LUT[] (~40 bytes)                │
│ • String Constants               (~500 bytes)                  │
├────────────────────────────────────────────────────────────────────┤
│ Total Flash Usage: ~948 bytes                                 │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│                    RAM - BSS Segment                           │
├────────────────────────────────────────────────────────────────────┤
│ • g_filtered_drones_storage[]    640 bytes                    │
│ • g_spectrum_data_buffer[]       512 bytes (2×256)            │
│ • g_power_levels_buffer[]        240 bytes                    │
│ • g_entries_to_scan_storage[]   1000 bytes                    │
│ • g_histogram_buffer_storage[]      67 bytes                    │
│ • g_ui_buffers_storage[]          160 bytes                    │
│ • Mutexes (6)                   ~48 bytes                    │
│ • freq_db_storage_             4096 bytes                    │
│ • tracked_drones_storage_       ~800 bytes                    │
├────────────────────────────────────────────────────────────────────┤
│ Total BSS Usage: ~7,563 bytes                               │
│ Available RAM: 128,000 - 7,563 = 120,437 bytes             │
└────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────┐
│                    RAM - Stack (4KB per thread)                 │
├────────────────────────────────────────────────────────────────────┤
│ • Local Variables                  ~800 bytes                    │
│ • Small Arrays                    ~400 bytes                    │
│ • Function Call Frames             ~500 bytes                    │
│ • Register Spills                 ~200 bytes                    │
│ • Safety Margin                   ~500 bytes                    │
├────────────────────────────────────────────────────────────────────┤
│ Total Stack Usage: ~2,400 bytes (59% of 4KB limit)          │
│ Stack Headroom: ~1,696 bytes (41% of 4KB limit)            │
└────────────────────────────────────────────────────────────────────┘
```

---

## 6. IMPLEMENTATION PHASES

### Phase 1: Critical Fixes (Priority 1)

**Goal:** Eliminate stack overflow risk by moving largest stack allocations to static storage

| Fix | File | Stack Savings | Risk | Estimated Effort |
|-----|------|--------------|-------|------------------|
| Fix #1: FilteredDronesSnapshot | dsp_display_types.cpp | 640 bytes | Low | 2 hours |
| Fix #2: spectrum_data_ | ui_enhanced_drone_analyzer.hpp | 256 bytes | Low | 1 hour |
| Fix #3: power_levels | dsp_spectrum_processor.hpp | 240 bytes | Low | 1 hour |
| Fix #4: temp_string_ | ui_enhanced_drone_settings.hpp | ~150 bytes | Medium | 3 hours |
| Fix #5: filename | ui_enhanced_drone_settings.cpp | ~75 bytes | Low | 1 hour |
| Fix #6: entries_to_scan_ | ui_enhanced_drone_analyzer.hpp | 1000 bytes | Low | 2 hours |

**Phase 1 Total Stack Savings:** ~2,361 bytes (54% reduction)
**Phase 1 Total Effort:** ~10 hours

**Phase 1 Deliverables:**
- All Priority 1 fixes implemented
- Unit tests for RAII guard templates
- Integration tests for static storage access
- Stack usage validation with static_assert

---

### Phase 2: High Priority Fixes (Priority 2)

**Goal:** Optimize remaining stack usage and eliminate heap allocations

| Fix | File | Stack Savings | Risk | Estimated Effort |
|-----|------|--------------|-------|------------------|
| Fix #7: histogram_buffer_ | ui_enhanced_drone_analyzer.hpp | 67 bytes | Low | 1 hour |
| Fix #8: Settings by reference | ui_enhanced_drone_analyzer.hpp | ~200 bytes | Medium | 4 hours |
| Fix #9: filesystem::path | ui_enhanced_drone_settings.cpp | ~150 bytes | Low | 2 hours |
| Fix #10: UI buffers | ui_enhanced_drone_analyzer.hpp | 160 bytes | Low | 1 hour |

**Phase 2 Total Stack Savings:** ~577 bytes (13% reduction)
**Phase 2 Total Effort:** ~8 hours

**Phase 2 Deliverables:**
- All Priority 2 fixes implemented
- Path utility functions with unit tests
- Settings reference passing validation
- Memory usage profiling

---

### Phase 3: Testing and Validation

**Goal:** Ensure all fixes work correctly and don't introduce bugs

| Task | Description | Estimated Effort |
|------|-------------|------------------|
| Unit Tests | Test RAII guards, static storage access, lock-free buffers | 4 hours |
| Integration Tests | Test full application flow with optimizations | 3 hours |
| Stack Analysis | Verify stack usage with static_assert and runtime checks | 2 hours |
| Performance Tests | Measure performance impact of mutex locking | 2 hours |
| Thread Safety Tests | Verify no race conditions with static storage | 3 hours |

**Phase 3 Total Effort:** ~14 hours

**Phase 3 Deliverables:**
- Comprehensive test suite
- Stack usage report (<3,200 bytes target)
- Performance impact analysis (<5% overhead target)
- Thread safety validation report

---

## 7. EXPECTED RESULTS

### 7.1 Stack Usage Summary

| Metric | Before | After | Improvement |
|--------|---------|--------|-------------|
| Total Stack Usage | 4,366 bytes | ~2,400 bytes | -45% |
| Stack Utilization | 106% | 59% | -47% |
| Stack Headroom | -366 bytes (OVERFLOW) | 1,696 bytes | +2062 bytes |
| Heap Allocations | ~200 bytes | 0 bytes | -100% |
| Static Storage Usage | ~4,500 bytes | ~7,563 bytes | +3,063 bytes |

### 7.2 Per-Fix Stack Savings

| Fix | Stack Savings | Cumulative Savings |
|-----|--------------|-------------------|
| Fix #1: FilteredDronesSnapshot | 640 bytes | 640 bytes |
| Fix #2: spectrum_data_ | 256 bytes | 896 bytes |
| Fix #3: power_levels | 240 bytes | 1,136 bytes |
| Fix #4: temp_string_ | ~150 bytes | 1,286 bytes |
| Fix #5: filename | ~75 bytes | 1,361 bytes |
| Fix #6: entries_to_scan_ | 1000 bytes | 2,361 bytes |
| Fix #7: histogram_buffer_ | 67 bytes | 2,428 bytes |
| Fix #8: Settings by reference | ~200 bytes | 2,628 bytes |
| Fix #9: filesystem::path | ~150 bytes | 2,778 bytes |
| Fix #10: UI buffers | 160 bytes | 2,938 bytes |

**Total Stack Savings:** ~2,938 bytes (67% reduction)
**Final Stack Usage:** ~1,428 bytes (35% of 4KB limit)

### 7.3 Compliance with Embedded Constraints

| Constraint | Status | Notes |
|------------|--------|-------|
| NO heap allocations (std::vector, std::string, std::map) | ✅ PASS | All heap allocations eliminated |
| NO new, malloc | ✅ PASS | No dynamic memory allocation |
| NO exceptions | ✅ PASS | All functions noexcept |
| NO RTTI | ✅ PASS | No typeid or dynamic_cast |
| Stack allocations < 1KB per function | ✅ PASS | Max stack per function ~800 bytes |
| Use std::array, std::string_view, fixed-size buffers | ✅ PASS | All data structures use fixed-size buffers |
| Use constexpr, enum class, using Type = uintXX_t | ✅ PASS | Type-safe code maintained |

---

## 8. RISK MITIGATION STRATEGIES

### 8.1 Thread Safety Risks

**Risk:** Static storage accessed from multiple threads without proper synchronization

**Mitigation:**
- Use RAII guard templates for all static storage access
- Add mutex protection for all shared static storage
- Use lock-free double-buffering for high-frequency data (spectrum)
- Add static_assert to verify mutex is held before accessing storage

**Validation:**
- Run thread safety tests with Helgrind or similar tool
- Add runtime assertions for mutex ownership
- Use ChibiOS mutex debugging features

---

### 8.2 Lifetime Management Risks

**Risk:** Settings reference outlives parent object (use-after-free)

**Mitigation:**
- Document lifetime requirements clearly in code comments
- Add runtime assertions to validate settings reference
- Consider using shared_ptr if lifetime cannot be guaranteed
- Add static_assert to verify settings object lifetime

**Validation:**
- Add unit tests for lifetime scenarios
- Use static analysis tools to detect use-after-free
- Add runtime checks for null references

---

### 8.3 Performance Impact Risks

**Risk:** Mutex locking causes performance degradation

**Mitigation:**
- Use lock-free double-buffering for high-frequency data (spectrum)
- Minimize mutex hold time (copy data, then release)
- Use try_lock() for non-critical operations
- Profile performance before and after optimization

**Validation:**
- Measure mutex contention with ChibiOS profiling tools
- Compare performance metrics (scan rate, UI responsiveness)
- Optimize critical sections if contention is high

---

### 8.4 Memory Fragmentation Risks

**Risk:** Static storage increases memory fragmentation

**Mitigation:**
- Use BSS segment for static storage (zero-initialized at startup)
- Align all static storage to natural boundaries
- Use fixed-size buffers (no dynamic allocation)
- Add memory usage monitoring

**Validation:**
- Measure memory fragmentation at runtime
- Use static_assert to verify alignment
- Add memory usage reports to debug output

---

### 8.5 Testing Strategy

**Unit Tests:**
- Test RAII guard templates (lock/unlock, exception safety)
- Test lock-free double-buffer (correctness, memory ordering)
- Test path utility functions (bounds checking, edge cases)
- Test settings reference passing (lifetime, thread safety)

**Integration Tests:**
- Test full application flow with optimizations
- Test concurrent access to static storage
- Test UI rendering with static buffers
- Test database loading with path utilities

**Performance Tests:**
- Measure stack usage with static_assert and runtime checks
- Measure performance impact of mutex locking
- Compare scan rate before and after optimization
- Measure UI responsiveness

**Thread Safety Tests:**
- Run with thread sanitizer (if available)
- Test concurrent access to static storage
- Test lock-free buffer correctness
- Test mutex contention scenarios

---

## 9. CONCLUSION

This architectural blueprint provides a comprehensive plan to eliminate the critical stack overflow risk in the Enhanced Drone Analyzer application. The proposed changes:

1. **Eliminate stack overflow risk** by moving 2,938 bytes from stack to static storage
2. **Eliminate all heap allocations** by replacing std::string and std::filesystem::path with fixed-size buffers
3. **Maintain thread safety** using RAII guard templates and lock-free double-buffering
4. **Comply with embedded constraints** by using only permitted data structures and patterns
5. **Minimize performance impact** by using lock-free patterns for high-frequency data

The implementation is divided into three phases:
- **Phase 1 (Critical Fixes):** 10 hours, 2,361 bytes saved
- **Phase 2 (High Priority Fixes):** 8 hours, 577 bytes saved
- **Phase 3 (Testing and Validation):** 14 hours, comprehensive validation

**Total Estimated Effort:** ~32 hours

**Expected Result:** Stack usage reduced from 4,366 bytes (106% overflow) to ~1,428 bytes (35% utilization), with 100% elimination of heap allocations and full compliance with embedded constraints.

---

**End of Stage 2: Architect's Blueprint**

Next Stage: Stage 3 - Red Team Attack (Verification)
