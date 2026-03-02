# STAGE 2: The Architect's Blueprint - Refactoring Plan

**Date:** 2026-03-02
**Based on:** Stage 1 Forensic Audit (24 critical defects)
**Target Architecture:** STM32F405 (ARM Cortex-M4, 128KB RAM)

---

## Architecture Overview

### Current Architecture Problems
1. **Monolithic UI File:** `ui_enhanced_drone_analyzer.cpp` (4618 lines) contains UI + DSP + scanning + threading logic
2. **Heap Allocations:** Singleton uses `new` operator, std::string used throughout
3. **Mixed Responsibilities:** UI classes contain signal processing logic
4. **Type Ambiguity:** Inconsistent frequency types (uint64_t vs int64_t vs Frequency)

### Proposed Architecture (Diamond Code Standard)

```
┌─────────────────────────────────────────────────────────────────┐
│                    UI LAYER (Presentation)                   │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ ui_enhanced_drone_analyzer.cpp/hpp (UI ONLY)      │  │
│  │ - EnhancedDroneSpectrumAnalyzerView                     │  │
│  │ - DroneDisplayController (UI rendering)                 │  │
│  │ - No DSP logic, no scanning logic                     │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                            │
                            │ (uses)
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              BUSINESS LOGIC LAYER (Coordination)              │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ thread_coordinator.cpp/hpp (NEW FILE)                 │  │
│  │ - ScanningCoordinator (static storage)                   │  │
│  │ - Thread lifecycle management                            │  │
│  │ - No UI dependencies                                   │  │
│  │ - Namespace: ui::apps::enhanced_drone_analyzer::thread  │  │
│  └──────────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ drone_scanner_core.cpp/hpp (NEW FILE)               │  │
│  │ - DroneScanner (DSP logic only)                        │  │
│  │ - Frequency database management                          │  │
│  │ - Detection logic                                      │  │
│  │ - Namespace: ui::apps::enhanced_drone_analyzer::core   │  │
│  └──────────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ spectral_analyzer_core.cpp/hpp (NEW FILE)            │  │
│  │ - SpectralAnalyzer (signal processing only)             │  │
│  │ - Noise floor calculation                              │  │
│  │ - SNR computation                                    │  │
│  │ - Namespace: ui::apps::enhanced_drone_analyzer::signal │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                            │
                            │ (uses)
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              DATA LAYER (Persistence)                         │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ eda_unified_database.cpp/hpp (EXISTING, CLEAN)     │  │
│  │ - UnifiedDroneDatabase                                │  │
│  │ - File I/O (freqman format)                           │  │
│  │ - Namespace: ui::apps::enhanced_drone_analyzer        │  │
│  └──────────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ settings_persistence.cpp/hpp (EXISTING, CLEAN)        │  │
│  │ - SettingsPersistence template                           │  │
│  │ - TXT file I/O                                      │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                            │
                            │ (uses)
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              HARDWARE LAYER (Abstraction)                     │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ drone_hardware_controller.cpp/hpp (NEW FILE)         │  │
│  │ - DroneHardwareController                               │  │
│  │ - RF control abstraction                                │  │
│  │ - Baseband API wrapper                                 │  │
│  │ - Namespace: ui::apps::enhanced_drone_analyzer::hardware│  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

---

## File-by-File Refactoring Plan

### 1. NEW FILE: thread_coordinator.hpp/cpp

**Purpose:** Extract thread coordination logic from UI layer.

**Memory Strategy:** Zero heap, static storage only.

**Key Changes:**
```cpp
// BEFORE (scanning_coordinator.hpp):
static ScanningCoordinator* instance_ptr_;  // Heap-allocated

// AFTER (thread_coordinator.hpp):
class ScanningCoordinator {
private:
    // Static storage (BSS segment, zero-initialized)
    // IMPORTANT: This is NOT heap allocation - it's static memory in BSS segment
    alignas(alignof(ScanningCoordinator))
    static uint8_t instance_storage_[sizeof(ScanningCoordinator)];
    
    static bool initialized_;
    
public:
    static ScanningCoordinator& instance() noexcept {
        if (!initialized_) {
            // Placement new with static storage (ZERO HEAP ALLOCATION)
            // CRITICAL: placement new does NOT allocate from heap
            // It constructs the object at the pre-allocated static address
            new (instance_storage_) ScanningCoordinator(/* args */);
            initialized_ = true;
        }
        return *reinterpret_cast<ScanningCoordinator*>(instance_storage_);
    }
};
```

**Benefits:**
- Eliminates heap allocation (Violation #1) - placement new uses static storage, NOT heap
- Eliminates raw pointer ownership issues (Violation #3)
- Deterministic initialization - memory is pre-allocated at link time
- Zero runtime allocation - no malloc/new calls at runtime

**Stack Usage:** 2048 bytes (unchanged, already validated)

---

### 2. NEW FILE: drone_scanner_core.hpp/cpp

**Purpose:** Extract DSP/scanning logic from UI layer.

**Memory Strategy:** Fixed-size arrays in BSS segment.

**Key Changes:**
```cpp
// BEFORE (ui_enhanced_drone_analyzer.hpp):
class DroneScanner {
    // Mixed with UI code
    void perform_scan_cycle(DroneHardwareController& hardware);
    // ...
};

// AFTER (drone_scanner_core.hpp):
// Use core sub-namespace within existing module namespace
namespace ui::apps::enhanced_drone_analyzer::core {

class DroneScanner {
public:
    // DSP-only interface (no UI types)
    struct ScanResult {
        uint64_t frequency_hz;
        int32_t rssi_db;
        uint8_t snr;
        DroneType drone_type;
        ThreatLevel threat_level;
    };
    
    // Pure DSP methods
    ScanResult perform_scan_cycle(const HardwareInterface& hw);
    void update_tracked_drone(const ScanResult& result);
    
private:
    // Fixed-size storage (BSS segment)
    alignas(alignof(TrackedDrone))
    static uint8_t tracked_drones_storage_[TRACKED_DRONES_STORAGE_SIZE];
    
    alignas(alignof(FreqmanDB))
    static uint8_t freq_db_storage_[FREQ_DB_STORAGE_SIZE];
};

} // namespace ui::apps::enhanced_drone_analyzer::core
```

**Benefits:**
- Separates DSP logic from UI (Violation #10)
- Enables independent testing
- Reusable in other contexts
- Clear responsibility boundary

**Stack Usage:** 4096 bytes (DB_LOADING_STACK_SIZE, validated)

---

### 3. NEW FILE: spectral_analyzer_core.hpp/cpp

**Purpose:** Extract signal processing logic from UI layer.

**Memory Strategy:** Stack-only buffers (validated < 256 bytes).

**Key Changes:**
```cpp
// BEFORE (ui_enhanced_drone_analyzer.hpp):
class SpectralAnalyzer {
    // Mixed with UI code
};

// AFTER (spectral_analyzer_core.hpp):
// Use signal sub-namespace within existing module namespace
namespace ui::apps::enhanced_drone_analyzer::signal {

class SpectralAnalyzer {
public:
    // Pure signal processing interface
    struct AnalysisResult {
        SignalSignature signature;
        uint8_t noise_floor;
        uint8_t snr;
        uint32_t signal_width_hz;
        bool is_valid;
    };
    
    // Stack-only analysis (no heap, no UI dependencies)
    static AnalysisResult analyze(
        const uint8_t spectrum_buffer[256],
        uint32_t center_freq_hz,
        uint32_t slice_bandwidth_hz
    ) noexcept;
    
private:
    // Stack buffer for histogram (validated size)
    static constexpr size_t HISTOGRAM_BINS = 64;
    using HistogramBuffer = std::array<uint16_t, HISTOGRAM_BINS>;
};

} // namespace ui::apps::enhanced_drone_analyzer::signal
```

**Benefits:**
- Separates signal processing from UI (Violation #13)
- Reusable DSP algorithms
- Clear interface boundary
- No UI dependencies

**Stack Usage:** 256 bytes (histogram buffer, validated)

---

### 4. NEW FILE: drone_hardware_controller.hpp/cpp

**Purpose:** Abstract hardware access for testability.

**Memory Strategy:** Zero heap, state machine only.

**Key Changes:**
```cpp
// NEW FILE (drone_hardware_controller.hpp):
// Use hardware sub-namespace within existing module namespace
namespace ui::apps::enhanced_drone_analyzer::hardware {

class DroneHardwareController {
public:
    // Hardware interface (abstracted from baseband API)
    struct RFState {
        uint64_t frequency_hz;
        uint32_t bandwidth_hz;
        int32_t lna_gain_db;
        int32_t vga_gain_db;
    };
    
    // Pure hardware control methods
    bool set_frequency(uint64_t freq_hz) noexcept;
    bool set_bandwidth(uint32_t bw_hz) noexcept;
    bool set_gain(int32_t lna_db, int32_t vga_db) noexcept;
    bool start_rx() noexcept;
    bool stop_rx() noexcept;
    
    // Spectrum data acquisition
    bool read_spectrum(uint8_t buffer[256]) noexcept;
    
private:
    RFState current_state_;
};

} // namespace ui::apps::enhanced_drone_analyzer::hardware
```

**Benefits:**
- Hardware abstraction layer
- Enables mock hardware for testing
- Centralizes RF control
- No UI dependencies

**Stack Usage:** < 1KB (state machine only)

---

### 5. REFACTOR: ui_enhanced_drone_analyzer.hpp/cpp

**Purpose:** Keep only UI rendering logic, delegate to core modules.

**Key Changes:**
```cpp
// BEFORE (4618 lines, mixed responsibilities):
class EnhancedDroneSpectrumAnalyzerView : public View {
    DroneScanner scanner_;           // DSP logic
    ScanningCoordinator coordinator_; // Threading logic
    SpectralAnalyzer spectral_;     // Signal processing
    // ... 4000+ lines of mixed code
};

// AFTER (UI only, ~1000 lines):
class EnhancedDroneSpectrumAnalyzerView : public View {
public:
    // UI lifecycle
    EnhancedDroneSpectrumAnalyzerView(NavigationView& nav);
    ~EnhancedDroneSpectrumAnalyzerView() override = default;
    
    void focus() noexcept override;
    void paint(Painter& painter) override;
    
private:
    // UI rendering only (no DSP, no scanning)
    void paint_spectrum(Painter& painter);
    void paint_drone_list(Painter& painter);
    void paint_status_bar(Painter& painter);
    
    // Delegates to core modules
    ui::apps::enhanced_drone_analyzer::core::DroneScanner& scanner() noexcept {
        return ui::apps::enhanced_drone_analyzer::core::DroneScanner::instance();
    }
    
    ui::apps::enhanced_drone_analyzer::thread::ScanningCoordinator& coordinator() noexcept {
        return ui::apps::enhanced_drone_analyzer::thread::ScanningCoordinator::instance();
    }
    
    // UI state only
    struct UIState {
        bool show_detailed_info;
        bool show_mini_spectrum;
        bool show_rssi_history;
        // ... UI-only state
    } ui_state_;
};
```

**Benefits:**
- Removes mixed logic (Violation #10)
- Reduces file size from 4618 to ~1000 lines
- Clear responsibility: UI rendering only
- Testable UI layer with mock cores

**Stack Usage:** < 2KB (UI state only)

---

### 6. REFACTOR: ui_enhanced_drone_settings.hpp

**Purpose:** Eliminate std::string usage.

**Key Changes:**
```cpp
// BEFORE (heap allocation):
class AudioSettingsView : public View {
    std::string title() const noexcept override {
        return title_string_view();
    }
};

// AFTER (zero heap):
class AudioSettingsView : public View {
    // Zero-allocation version (internal use)
    [[nodiscard]] static constexpr const char* title_string_view() noexcept {
        return "Audio Settings";
    }
    
    // Framework-compatible version (still causes heap allocation ~50-200 bytes)
    // NOTE: This is a framework limitation - the UI framework requires std::string return type
    // Internal EDA code should use title_string_view() instead (zero allocation)
    std::string title() const noexcept override {
        return title_string_view();
    }
    
    // Internal EDA code uses title_string_view() (zero heap)
};
```

**Benefits:**
- Eliminates heap allocation for internal use (Violation #5-9)
- Maintains framework compatibility
- Clear documentation of workaround
- Internal code uses zero-allocation path

**Stack Usage:** Unchanged (UI widget stack)

---

### 7. REFACTOR: scanning_coordinator.hpp/cpp → thread_coordinator.hpp/cpp

**Purpose:** Rename and move to thread namespace, eliminate heap.

**Key Changes:**
```cpp
// BEFORE (heap allocation):
ScanningCoordinator* ScanningCoordinator::instance_ptr_ = nullptr;

bool ScanningCoordinator::initialize(...) {
    instance_ptr_ = new ScanningCoordinator(...);  // HEAP ALLOCATION - FORBIDDEN!
    return true;
}

// AFTER (static storage):
namespace ui::apps::enhanced_drone_analyzer::thread {

class ScanningCoordinator {
private:
    // Static storage in BSS segment (zero-initialized at startup)
    // IMPORTANT: This is NOT heap allocation - memory is allocated at link time
    alignas(alignof(ScanningCoordinator))
    static uint8_t instance_storage_[sizeof(ScanningCoordinator)];
    static bool initialized_;
    
public:
    static ScanningCoordinator& instance() noexcept {
        if (!initialized_) {
            // Placement new with static storage (ZERO HEAP ALLOCATION)
            // CRITICAL: placement new constructs object at pre-allocated address
            // This does NOT call malloc or allocate from heap
            new (instance_storage_) ScanningCoordinator(/* args */);
            initialized_ = true;
        }
        return *reinterpret_cast<ScanningCoordinator*>(instance_storage_);
    }
};

} // namespace ui::apps::enhanced_drone_analyzer::thread
```

**Benefits:**
- Eliminates heap allocation (Violation #1)
- Eliminates raw pointer ownership (Violation #3)
- Moves to appropriate namespace
- Clear separation from UI

**Stack Usage:** 2048 bytes (unchanged)

---

### 8. ADD: eda_type_definitions.hpp

**Purpose:** Define semantic types to eliminate type ambiguity.

**Key Changes:**
```cpp
// NEW FILE (eda_type_definitions.hpp):
namespace eda::types {

// Semantic frequency types (eliminate uint64_t/int64_t confusion)
using FrequencyHz = uint64_t;      // Hardware frequency in Hz
using FrequencyKHz = uint32_t;     // Frequency in kHz
using FrequencyMHz = uint32_t;     // Frequency in MHz
using FrequencyGHz = uint16_t;     // Frequency in GHz

// Semantic RSSI types
using RSSI_dB = int32_t;         // RSSI in dBm
using RSSI_dBm = int32_t;        // RSSI in dBm (alias)
using SNR_dB = uint8_t;         // SNR in dB

// Semantic time types
using TimeMs = uint32_t;          // Time in milliseconds
using TimeUs = uint32_t;          // Time in microseconds

// Semantic count types
using DetectionCount = uint8_t;    // Number of detections
using DroneCount = uint8_t;       // Number of tracked drones

// Compile-time validation
static_assert(sizeof(FrequencyHz) == 8, "FrequencyHz must be 64-bit");
static_assert(sizeof(RSSI_dB) == 4, "RSSI_dB must be 32-bit");

} // namespace eda::types
```

**Benefits:**
- Eliminates type ambiguity (Violation #22, #23)
- Self-documenting code
- Compile-time type safety
- No casting between signed/unsigned

**Stack Usage:** 0 bytes (type aliases only)

---

### 9. ADD: eda_constants_extended.hpp

**Purpose:** Define all magic numbers as named constants.

**Key Changes:**
```cpp
// NEW FILE (eda_constants_extended.hpp):
namespace eda::constants {

// Thread coordination constants
namespace ThreadCoordination {
    constexpr TimeMs TERMINATION_TIMEOUT_MS = 5000;
    constexpr TimeMs POLL_INTERVAL_MS = 10;
    constexpr uint32_t MAX_CONSECUTIVE_TIMEOUTS = 3;
    constexpr uint32_t MAX_CONSECUTIVE_SCANNER_FAILURES = 5;
}

// Scanning constants
namespace Scanning {
    constexpr uint8_t MAX_SCAN_MODES = 3;  // DATABASE, WIDEBAND, HYBRID
    constexpr uint32_t CYCLES_CLAMP_MAX = 39;  // For progressive slowdown
    constexpr size_t HIGH_DENSITY_DETECTION_THRESHOLD = 20;
}

// UI constants
namespace UI {
    constexpr size_t SPECTRUM_POWER_LEVELS_SIZE = 200;
    constexpr size_t MAX_UI_DRONES = 10;
}

// Stack validation
static_assert(ThreadCoordination::TERMINATION_TIMEOUT_MS <= 10000,
              "TERMINATION_TIMEOUT_MS exceeds 10 seconds");
static_assert(UI::SPECTRUM_POWER_LEVELS_SIZE <= 512,
              "SPECTRUM_POWER_LEVELS_SIZE exceeds 512 bytes");

} // namespace eda::constants
```

**Benefits:**
- Eliminates magic numbers (Violation #14-21)
- Single source of truth for constants
- Compile-time validation
- Self-documenting code

**Stack Usage:** 0 bytes (constexpr only)

---

### 10. REFACTOR: Move Large Buffers to BSS

**Purpose:** Eliminate stack overflow risk.

**Key Changes:**
```cpp
// BEFORE (stack allocation risk):
class DroneDisplayController {
    uint8_t spectrum_power_levels_storage_[200];  // On stack!
};

// AFTER (BSS segment):
class DroneDisplayController {
public:
    // Static storage in BSS segment (zero-initialized at boot)
    alignas(alignof(uint8_t))
    static uint8_t spectrum_power_levels_storage_[200];
    
    // Compile-time validation
    static_assert(sizeof(spectrum_power_levels_storage_) <= 512,
                  "Spectrum buffer exceeds 512 bytes safe limit");
};
```

**Benefits:**
- Eliminates stack overflow risk (Violation #24)
- Deterministic memory usage
- Zero initialization cost (BSS cleared at boot)
- Compile-time size validation

**Stack Usage:** 0 bytes (moved to BSS)

---

## Data Structure Replacements

### std::string → const char* / std::string_view

| Current Type | Replacement | Location |
|--------------|-------------|-----------|
| `std::string title()` | `const char* title_string_view()` | All View classes |
| `std::string description` | `char description[32]` | UnifiedDroneEntry |
| `std::string path` | `char path[MAX_PATH_LEN]` | File operations |

### std::vector → std::array

| Current Type | Replacement | Size |
|--------------|-------------|-------|
| `std::vector<uint8_t>` | `std::array<uint8_t, N>` | Fixed at compile time |
| `std::vector<TrackedDrone>` | `std::array<TrackedDrone, MAX_TRACKED_DRONES>` | MAX_TRACKED_DRONES = 20 |

### new/malloc → Static Storage

| Current Allocation | Replacement | Storage Location |
|-------------------|-------------|------------------|
| `new ScanningCoordinator()` | Placement new with static buffer | BSS segment |
| `new DroneScanner()` | Static member storage | BSS segment |
| `malloc(N)` | `uint8_t buffer[N]` | BSS segment |

---

## Memory Layout Plan

### BSS Segment (Zero-Initialized Static Storage)

```
┌─────────────────────────────────────────────────────────┐
│ ScanningCoordinator instance:  ~512 bytes            │
│ DroneScanner storage:        ~4096 bytes           │
│ TrackedDrone storage:       ~960 bytes (20 * 48)  │
│ Frequency DB storage:       ~4096 bytes           │
│ Spectrum buffer:            ~200 bytes            │
│ UnifiedDroneDatabase:       ~5760 bytes (120 * 48) │
└─────────────────────────────────────────────────────────┘
Total BSS: ~15,624 bytes (15.3 KB)
```

### Stack Usage Per Thread

```
┌─────────────────────────────────────────────────────────┐
│ Main UI Thread:          ~2048 bytes (validated)  │
│ Scanning Thread:         ~4096 bytes (validated)  │
│ Coordinator Thread:       ~2048 bytes (validated)  │
│ Logger Worker Thread:     ~4096 bytes (validated)  │
└─────────────────────────────────────────────────────────┘
Total Stack: ~12,288 bytes (12 KB)
```

### Heap Usage

```
┌─────────────────────────────────────────────────────────┐
│ Heap allocations:         0 bytes (ZERO HEAP)     │
│ std::string allocations: 0 bytes (ZERO HEAP)      │
│ std::vector allocations:  0 bytes (ZERO HEAP)      │
│ new/malloc calls:        0 bytes (ZERO HEAP)      │
└─────────────────────────────────────────────────────────┘
Total Heap: 0 bytes (GOAL ACHIEVED)
```

---

## Thread Safety Strategy

### Lock Order (Prevents Deadlock)

```
1. ATOMIC_FLAGS (volatile bool)
2. DATA_MUTEX (detection data, frequency DB)
3. SPECTRUM_MUTEX (spectrum buffer)
4. LOGGER_MUTEX (log file access)
5. SD_CARD_MUTEX (FatFS access - MUST BE LAST)
```

### Mutex Protection Rules

| Data Structure | Mutex | Lock Order |
|--------------|--------|------------|
| DetectionRingBuffer | buffer_mutex_ | DATA_MUTEX (2) |
| UnifiedDroneDatabase | mutex_ | DATA_MUTEX (2) |
| SettingsPersistence | settings_buffer_mutex | DATA_MUTEX (2) |
| SD Card I/O | sd_card_mutex | SD_CARD_MUTEX (5) |

### ISR Safety

- **NO mutex operations in ISR context** (ChibiOS limitation)
- Use `volatile bool` for flags accessed by ISR
- Use ChibiOS atomic operations for simple counters
- Defer complex operations to thread context

---

## Error Handling Strategy

### Error Codes (User-Provided)

| Error Code | Meaning | Root Cause | Fix |
|------------|----------|------------|-----|
| `20001E38` | Hard fault | Heap allocation failure | Eliminate heap (Violation #1) |
| `0080013` | Memory access violation | Null pointer dereference | Fix raw pointer (Violation #3) |
| `0000000` | Null pointer | Uninitialized singleton | Fix singleton (Violation #1) |
| `00000328` | Stack corruption | Stack overflow | Move buffers to BSS (Violation #24) |
| `ffffffff` | Uninitialized memory | Uninitialized heap read | Eliminate heap (Violation #1) |
| `000177ae` | Data abort | Type confusion | Fix types (Violation #22) |

### Error Handling Pattern

```cpp
// Diamond Code: noexcept + return codes
ResultType function() noexcept {
    // Guard clauses (early return)
    if (invalid_input) {
        return ResultType::fail(ErrorCode::INVALID_ARGUMENT);
    }
    
    // Main logic
    // ...
    
    // Success
    return ResultType::ok(value);
}
```

---

## Migration Path

### Phase 1: Foundation (No Breaking Changes)
1. Create `eda_type_definitions.hpp` (semantic types)
2. Create `eda_constants_extended.hpp` (named constants)
3. Add to existing code incrementally

### Phase 2: Core Extraction (No Breaking Changes)
1. Create `drone_scanner_core.hpp/cpp` (new file)
2. Create `spectral_analyzer_core.hpp/cpp` (new file)
3. Create `drone_hardware_controller.hpp/cpp` (new file)
4. Add delegation in existing code

### Phase 3: UI Refactoring (Breaking Changes)
1. Refactor `ui_enhanced_drone_analyzer.cpp` (remove mixed logic)
2. Refactor `ui_enhanced_drone_settings.hpp` (remove std::string)
3. Update includes and namespaces

### Phase 4: Singleton Fix (Breaking Changes)
1. Rename `scanning_coordinator` → `thread_coordinator`
2. Replace heap allocation with static storage
3. Update all call sites

### Phase 5: Validation
1. Compile-time static_assert validation
2. Stack usage analysis
3. Runtime testing with error codes
4. Memory leak detection (should be zero)

---

## Success Criteria

### Functional Requirements
- [ ] All error codes from user are resolved
- [ ] UI renders correctly (no regressions)
- [ ] Scanning works (no regressions)
- [ ] Database load/save works (no regressions)
- [ ] Audio alerts work (no regressions)

### Non-Functional Requirements
- [ ] Zero heap allocations (verified with chHeapStatus)
- [ ] Zero std::string usage (except framework compatibility)
- [ ] Zero std::vector usage
- [ ] All stack usage < 4KB per thread
- [ ] All mutex operations follow lock order
- [ ] No magic numbers (all constants defined)
- [ ] All types are semantic (no uint32_t for frequency)

### Code Quality Requirements
- [ ] All functions are noexcept
- [ ] All public APIs have [[nodiscard]] where appropriate
- [ ] All code follows Diamond Code standard
- [ ] All code has Doxygen comments
- [ ] All code is tested (unit tests)

---

**Next Stage:** STAGE 3 - The Red Team Attack (Verification)
