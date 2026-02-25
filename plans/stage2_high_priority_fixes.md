# Stage 2: Architect's Blueprint - High Priority Fixes (Priority 2)

**Diamond Code Pipeline - Stage 2: Architect's Blueprint**  
**Enhanced Drone Analyzer Codebase**  
**Target Platform: STM32F405 (ARM Cortex-M4, 128KB RAM)**

---

## Document Overview

This document contains **Priority 2 (High Priority) fixes** that SHOULD be implemented to improve code quality, maintainability, and performance. These fixes don't cause immediate failures but violate best practices and Diamond Code principles.

**Related Documents:**
- `stage2_critical_fixes.md` - Priority 1 critical fixes
- `stage2_medium_low_priority_fixes.md` - Priority 3 and 4 improvements

---

## Section 2: High Priority Fixes (Priority 2 - SHOULD FIX)

### Fix #H1: Magic Numbers Throughout Codebase

**Defect ID:** #7, #12  
**Severity:** HIGH  
**Constraint Violation:** Diamond Code requires named constants  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:442,1559,1567,1816,2636,3293,3423,3618,3848)

**Description:**
Hundreds of magic numbers scattered throughout the codebase make the code difficult to understand, maintain, and modify. Magic numbers violate the Diamond Code principle of explicit intent.

**Current Code Examples:**
```cpp
// ui_enhanced_drone_analyzer.cpp:442 - Magic number 39
uint32_t cycles_clamped = (cycles_value < 39) ? cycles_value : 39;

// ui_enhanced_drone_analyzer.cpp:1559,1567 - Magic frequency 433000000
return 433000000;  // Default fallback frequency

// ui_enhanced_drone_analyzer.cpp:1816 - Magic timeout 1000ms
constexpr systime_t SD_WRITE_TIMEOUT_MS = 1000;

// ui_enhanced_drone_analyzer.cpp:2636 - Magic progress 50%
uint32_t progress_percent = 50;

// ui_enhanced_drone_analyzer.cpp:3293 - Logic error with magic numbers
if (new_max > new_max + new_min) {  // Always false!

// ui_enhanced_drone_analyzer.cpp:3423 - Magic stack size 1792
constexpr size_t PAINT_STACK_REQUIRED = 1792;

// ui_enhanced_drone_analyzer.cpp:3618 - Undefined constant reference
if (elapsed > MS2ST(InitTiming::TIMEOUT_MS)) {

// ui_enhanced_drone_analyzer.cpp:3848 - Magic timeout
constexpr systime_t SETTINGS_LOAD_TIMEOUT_MS = MS2ST(EDA::Constants::SETTINGS_LOAD_TIMEOUT_MS);
```

**Proposed Solution:**

#### 1.1 Named Constants Organization

**Create Constants Header File:**
```cpp
// ui_drone_constants.hpp
#ifndef UI_DRONE_CONSTANTS_HPP_
#define UI_DRONE_CONSTANTS_HPP_

#include <cstdint>
#include <cstddef>

namespace EDA::UIConstants {

// ============================================================================
// FREQUENCY CONSTANTS
// ============================================================================

namespace Frequency {
    // Default fallback frequency (433 MHz ISM band)
    constexpr uint64_t DEFAULT_FALLBACK_HZ = 433'000'000ULL;
    
    // 2.4 GHz Wi-Fi band center
    constexpr uint64_t WIFI_24GHZ_CENTER_HZ = 2'450'000'000ULL;
    
    // 5.8 GHz band center
    constexpr uint64_t BAND_58GHZ_CENTER_HZ = 5'800'000'000ULL;
}

// ============================================================================
// TIMING CONSTANTS
// ============================================================================

namespace Timing {
    // SD card operations
    constexpr uint32_t SD_WRITE_TIMEOUT_MS = 1000;
    constexpr uint32_t SD_CARD_MOUNT_TIMEOUT_MS = 5000;
    constexpr uint32_t SD_CARD_POLL_INTERVAL_SHORT_MS = 50;
    constexpr uint32_t SD_CARD_POLL_INTERVAL_LONG_MS = 200;
    
    // Settings persistence
    constexpr uint32_t SETTINGS_LOAD_TIMEOUT_MS = 2000;
    constexpr uint32_t SETTINGS_SAVE_TIMEOUT_MS = 1000;
    
    // Initialization
    constexpr uint32_t INIT_TIMEOUT_MS = 10000;
    constexpr uint32_t INIT_PHASE_DELAY_MS = 100;
    
    // Scanning
    constexpr uint32_t SCAN_CYCLE_CHECK_INTERVAL = 50;
    constexpr uint32_t PLL_STABILIZATION_DELAY_MS = 10;
    constexpr uint32_t RSSI_TIMEOUT_MS = 60;
    constexpr uint32_t RSSI_POLL_DELAY_MS = 2;
    
    // Logging
    constexpr uint32_t LOG_WRITE_INTERVAL_MS = 200;
    constexpr uint32_t LOG_FLUSH_INTERVAL_MS = 1000;
}

// ============================================================================
// UI DIMENSIONS
// ============================================================================

namespace Dimensions {
    // Screen dimensions
    constexpr uint32_t SCREEN_WIDTH = 240;
    constexpr uint32_t SCREEN_HEIGHT = 320;
    
    // Spectrum display
    constexpr uint32_t SPECTRUM_WIDTH = 228;
    constexpr uint32_t SPECTRUM_HEIGHT = 120;
    constexpr uint32_t MINI_SPECTRUM_WIDTH = 228;
    constexpr uint32_t MINI_SPECTRUM_HEIGHT = 48;
    
    // Histogram
    constexpr uint32_t HISTOGRAM_WIDTH = 228;
    constexpr uint32_t HISTOGRAM_HEIGHT = 26;
    constexpr uint32_t HISTOGRAM_BAR_WIDTH = 4;
    
    // Progress bar
    constexpr uint32_t PROGRESS_BAR_WIDTH = 228;
    constexpr uint32_t PROGRESS_BAR_HEIGHT = 8;
    
    // Text buffers
    constexpr uint32_t MAX_TEXT_BUFFER_SIZE = 64;
    constexpr uint32_t MAX_FREQ_BUFFER_SIZE = 32;
    constexpr uint32_t MAX_STATUS_BUFFER_SIZE = 48;
    
    // Margins and spacing
    constexpr uint32_t MARGIN_SMALL = 4;
    constexpr uint32_t MARGIN_MEDIUM = 8;
    constexpr uint32_t MARGIN_LARGE = 16;
}

// ============================================================================
// STACK ALLOCATION LIMITS
// ============================================================================

namespace StackLimits {
    // Maximum safe stack allocation per function
    constexpr size_t MAX_STACK_PER_FUNCTION = 4096;  // 4KB
    
    // Specific function limits
    constexpr size_t PAINT_STACK_REQUIRED = 1792;     // 1.75KB for paint()
    constexpr size_t INIT_STACK_REQUIRED = 2048;      // 2KB for initialization
    constexpr size_t SCAN_STACK_REQUIRED = 1024;     // 1KB for scanning
    
    // Safety margin
    constexpr size_t STACK_SAFETY_MARGIN = 512;
}

// ============================================================================
// SCANNING CONSTANTS
// ============================================================================

namespace Scanning {
    // Adaptive scan intervals
    constexpr uint32_t FAST_SCAN_INTERVAL_MS = 250;
    constexpr uint32_t HIGH_THREAT_SCAN_INTERVAL_MS = 400;
    constexpr uint32_t NORMAL_SCAN_INTERVAL_MS = 750;
    constexpr uint32_t SLOW_SCAN_INTERVAL_MS = 1000;
    constexpr uint32_t VERY_SLOW_SCAN_INTERVAL_MS = 2000;
    
    // Progressive slowdown
    constexpr uint32_t PROGRESSIVE_SLOWDOWN_DIVISOR = 10;
    constexpr uint32_t MAX_SLOWDOWN_CYCLES = 39;
    
    // Batch processing
    constexpr uint32_t MAX_SCAN_BATCH_SIZE = 10;
    constexpr uint32_t PLL_STABILIZATION_ITERATIONS = 3;
    
    // Detection thresholds
    constexpr uint32_t MIN_DETECTION_COUNT = 3;
    constexpr uint32_t ALERT_PERSISTENCE_THRESHOLD = 3;
    constexpr uint32_t HIGH_DENSITY_DETECTION_THRESHOLD = 10;
}

// ============================================================================
// DISPLAY CONSTANTS
// ============================================================================

namespace Display {
    // Drone display limits
    constexpr size_t MAX_TRACKED_DRONES = 4;
    constexpr size_t MAX_DISPLAYED_DRONES = 3;
    constexpr size_t MAX_UI_DRONES = 3;
    
    // Spectrum display
    constexpr uint8_t SPECTRUM_BIN_COUNT = 256;
    constexpr size_t SPECTRAL_VALID_BIN_START = 8;
    constexpr size_t SPECTRAL_VALID_BIN_END = 240;
    
    // Noise filtering
    constexpr uint8_t NOISE_THRESHOLD = 10;
    constexpr uint8_t SPECTRAL_SNR_THRESHOLD = 10;
    constexpr uint8_t SPECTRAL_PEAK_THRESHOLD_DB = 6;
    
    // Progress display
    constexpr uint32_t DEFAULT_PROGRESS_PERCENT = 50;
    constexpr uint8_t PROGRESS_BAR_MAX_VALUE = 100;
}

// ============================================================================
// ERROR CODES
// ============================================================================

namespace ErrorCodes {
    constexpr int SUCCESS = 0;
    constexpr int INVALID_FREQUENCY = -1;
    constexpr int INVALID_RANGE = -2;
    constexpr int TIMEOUT = -3;
    constexpr int ALLOCATION_FAILED = -4;
    constexpr int DATABASE_ERROR = -5;
    constexpr int HARDWARE_ERROR = -6;
}

// ============================================================================
// MOVEMENT TRACKING
// ============================================================================

namespace Movement {
    // RSSI change thresholds
    constexpr int32_t TREND_THRESHOLD_APPROACHING = 3;
    constexpr int32_t TREND_THRESHOLD_RECEEDING = -3;
    constexpr int32_t TREND_MIN_HISTORY = 4;
    constexpr int32_t TREND_SILENCE_THRESHOLD = -110;
    
    // Stale drone timeout
    constexpr uint32_t STALE_DRONE_TIMEOUT_MS = 30000;
}

// ============================================================================
// AUDIO CONSTANTS
// ============================================================================

namespace Audio {
    // Frequency range
    constexpr uint32_t MIN_FREQ = 200;
    constexpr uint32_t MAX_FREQ = 20000;
    constexpr uint32_t MCU_MAX_FREQ_HZ = 4000;
    constexpr uint32_t GENERAL_MAX_FREQ_HZ = 20000;
    
    // Duration range
    constexpr uint32_t MIN_DURATION_MS = 50;
    constexpr uint32_t MAX_DURATION_MS = 5000;
    constexpr uint32_t DEFAULT_DURATION_MS = 500;
    
    // Volume range
    constexpr uint32_t MIN_VOLUME = 0;
    constexpr uint32_t MAX_VOLUME = 100;
    constexpr uint32_t DEFAULT_VOLUME = 50;
    
    // Default alert
    constexpr uint32_t DEFAULT_FREQ_HZ = 800;
    constexpr uint32_t DEFAULT_BEEP_DURATION_MS = 50;
    constexpr uint32_t DEFAULT_BEEP_FREQUENCY_HZ = 800;
    
    // Cooldown
    constexpr uint32_t DEFAULT_COOLDOWN_MS = 100;
    constexpr uint32_t ALERT_COOLDOWN_MS = 100;
}

// ============================================================================
// VALIDATION CONSTANTS
// ============================================================================

namespace Validation {
    // Frequency validation
    constexpr uint64_t MIN_HARDWARE_FREQ = 1'000'000ULL;
    constexpr uint64_t MAX_HARDWARE_FREQ = 7'200'000'000ULL;
    
    // RSSI validation
    constexpr int32_t MIN_VALID_RSSI = -110;
    constexpr int32_t MAX_VALID_RSSI = 10;
    constexpr int32_t NOISE_FLOOR_RSSI = -110;
    
    // Failure thresholds
    constexpr int32_t MAX_CONSECUTIVE_FAILURES = 10;
}

} // namespace EDA::UIConstants

#endif // UI_DRONE_CONSTANTS_HPP_
```

#### 1.2 Refactored Code Examples

**Before:**
```cpp
// Magic numbers
uint32_t cycles_clamped = (cycles_value < 39) ? cycles_value : 39;
return 433000000;
uint32_t progress_percent = 50;
```

**After:**
```cpp
// Named constants
using namespace EDA::UIConstants;
uint32_t cycles_clamped = (cycles_value < Scanning::MAX_SLOWDOWN_CYCLES) 
                          ? cycles_value 
                          : Scanning::MAX_SLOWDOWN_CYCLES;
return Frequency::DEFAULT_FALLBACK_HZ;
uint32_t progress_percent = Display::DEFAULT_PROGRESS_PERCENT;
```

**Memory Impact Analysis:**
- **Before:** Magic numbers embedded in code
- **After:** Constants in Flash (0 RAM)
- **Savings:** 0 bytes (same code size, better readability)

**Performance Impact Analysis:**
- **Before:** Direct constant use
- **After:** Same (compiler optimizes to direct constants)
- **Improvement:** Zero runtime overhead

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (internal refactoring)
- **Migration Effort:** Medium (find and replace magic numbers)
- **Test Coverage:** Existing tests verify functionality

---

### Fix #H2: Type Ambiguity Throughout Codebase

**Defect ID:** #8, #13  
**Severity:** HIGH  
**Constraint Violation:** Diamond Code requires explicit type aliases  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:2149,2278,2749,2751,3139,3141,3142,3173,3187,3269,3270,3315,3317)

**Description:**
Implicit types and ambiguous integer types make the code difficult to understand and prone to errors. Diamond Code requires explicit type aliases with semantic meaning.

**Current Code Examples:**
```cpp
// ui_enhanced_drone_analyzer.cpp:2149 - Implicit size_t
size_t total_drones = approaching + static_count + receding;

// ui_enhanced_drone_analyzer.cpp:2278 - Implicit int
(std::abs(drone.rssi - last_rssi_) > 1)

// ui_enhanced_drone_analyzer.cpp:2749-2751 - Implicit comparisons
if (a.rssi != b.rssi) return a.rssi > b.rssi;
if (a.threat != b.threat) return static_cast<int>(a.threat) > static_cast<int>(b.threat);
return a.last_seen > b.last_seen;

// ui_enhanced_drone_analyzer.cpp:3139 - Implicit char array
char buffer[32];

// ui_enhanced_drone_analyzer.cpp:3141-3142 - Implicit bool
bool current = audio_get_enable_alerts(settings_);
audio_set_enable_alerts(settings_, !current);

// ui_enhanced_drone_analyzer.cpp:3173 - Implicit size_t
size_t current_len = strlen(buffer);

// ui_enhanced_drone_analyzer.cpp:3187 - Implicit uint32_t
uint32_t band_mhz = hardware_.get_spectrum_bandwidth() / 1000000ULL;

// ui_enhanced_drone_analyzer.cpp:3269-3270 - Implicit Frequency
Frequency new_min = static_cast<Frequency>(DiamondCore::FrequencyParser::parse_mhz_string(min_str));
Frequency new_max = static_cast<Frequency>(DiamondCore::FrequencyParser::parse_mhz_string(max_str));

// ui_enhanced_drone_analyzer.cpp:3315-3317 - Implicit types
StatusFormatter::format_to(buffer, "Range updated:\n%s - %s\nBW: %lu MHz",
                           min_freq_buf, max_freq_buf,
                           (unsigned long)(new_slice_width / 1000000));
```

**Proposed Solution:**

#### 2.1 Type Aliases Organization

**Create Type Aliases Header File:**
```cpp
// ui_drone_types.hpp
#ifndef UI_DRONE_TYPES_HPP_
#define UI_DRONE_TYPES_HPP_

#include <cstdint>
#include <cstddef>
#include <array>

namespace EDA::Types {

// ============================================================================
// SEMANTIC TYPE ALIASES
// ============================================================================

// Frequency types
using FrequencyHz = uint64_t;
using FrequencyMHz = uint32_t;
using FrequencyKHz = uint32_t;

// RSSI types
using RSSIValue = int32_t;
using RSSIDB = int32_t;

// Time types
using TimestampMs = uint32_t;
using TimestampSystime = uint32_t;
using DurationMs = uint32_t;

// Count types
using DroneCount = size_t;
using DetectionCount = size_t;
using CycleCount = uint32_t;

// Index types
using DroneIndex = uint8_t;
using SpectrumBinIndex = uint8_t;
using ThreatIndex = uint8_t;
using TrendIndex = uint8_t;

// Buffer types
using TextBufferSize = size_t;
using FreqBufferSize = size_t;

// Progress types
using ProgressPercent = uint8_t;
using ProgressValue = uint32_t;

// Coordinate types
using ScreenX = uint16_t;
using ScreenY = uint16_t;
using Width = uint16_t;
using Height = uint16_t;

// Color types
using ColorValue = uint32_t;
using AlphaValue = uint8_t;

// ============================================================================
// FIXED-SIZE BUFFER TYPES
// ============================================================================

// Text buffers
using TextBuffer64 = std::array<char, 64>;
using TextBuffer48 = std::array<char, 48>;
using TextBuffer32 = std::array<char, 32>;
using TextBuffer16 = std::array<char, 16>;

// Frequency display buffers
using FreqBuffer32 = std::array<char, 32>;
using FreqBuffer16 = std::array<char, 16>;

// ============================================================================
// ENUM CLASS TYPES
// ============================================================================

// Threat levels
enum class ThreatLevel : uint8_t {
    NONE = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

// Movement trends
enum class MovementTrend : uint8_t {
    STATIC = 0,
    APPROACHING = 1,
    RECEDING = 2,
    UNKNOWN = 3
};

// Drone types
enum class DroneType : uint8_t {
    UNKNOWN = 0,
    DJI = 1,
    PARROT = 2,
    YUNEEC = 3,
    AUTEL = 4,
    CUSTOM = 255
};

// Display modes
enum class DisplayMode : uint8_t {
    NORMAL = 0,
    ALERT = 1,
    ERROR = 2
};

// Initialization states
enum class InitState : uint8_t {
    CONSTRUCTED = 0,
    BUFFERS_ALLOCATED = 1,
    DATABASE_LOADING = 2,
    DATABASE_LOADED = 3,
    HARDWARE_READY = 4,
    UI_LAYOUT_READY = 5,
    SETTINGS_LOADED = 6,
    COORDINATOR_READY = 7,
    FULLY_INITIALIZED = 8,
    INITIALIZATION_ERROR = 9
};

// Error codes
enum class InitError : uint8_t {
    NONE = 0,
    GENERAL_TIMEOUT = 1,
    ALLOCATION_FAILED = 2,
    DATABASE_ERROR = 3,
    HARDWARE_ERROR = 4,
    SD_CARD_ERROR = 5,
    SETTINGS_ERROR = 6
};

// ============================================================================
// CONSTEXPR TYPE VALIDATORS
// ============================================================================

namespace Validators {
    // Frequency validation
    constexpr bool is_valid_frequency(FrequencyHz freq) noexcept {
        return freq >= Validation::MIN_HARDWARE_FREQ && 
               freq <= Validation::MAX_HARDWARE_FREQ;
    }
    
    // RSSI validation
    constexpr bool is_valid_rssi(RSSIValue rssi) noexcept {
        return rssi >= Validation::MIN_VALID_RSSI && 
               rssi <= Validation::MAX_VALID_RSSI;
    }
    
    // Drone count validation
    constexpr bool is_valid_drone_count(DroneCount count) noexcept {
        return count <= Display::MAX_TRACKED_DRONES;
    }
    
    // Progress validation
    constexpr bool is_valid_progress(ProgressPercent progress) noexcept {
        return progress <= 100;
    }
}

// ============================================================================
// CONSTEXPR TYPE CONVERTERS
// ============================================================================

namespace Converters {
    // Frequency conversions
    constexpr FrequencyMHz hz_to_mhz(FrequencyHz hz) noexcept {
        return static_cast<FrequencyMHz>(hz / 1'000'000);
    }
    
    constexpr FrequencyHz mhz_to_hz(FrequencyMHz mhz) noexcept {
        return static_cast<FrequencyHz>(mhz) * 1'000'000;
    }
    
    // Time conversions
    constexpr TimestampMs systime_to_ms(TimestampSystime systime) noexcept {
        return static_cast<TimestampMs>(systime * (1000 / CH_CFG_ST_FREQUENCY));
    }
    
    // RSSI conversions
    constexpr RSSIDB rssi_to_db(RSSIValue rssi) noexcept {
        return rssi;
    }
    
    // Progress conversions
    constexpr ProgressValue percent_to_value(ProgressPercent percent, 
                                            ProgressValue max_value) noexcept {
        return static_cast<ProgressValue>(percent) * max_value / 100;
    }
}

} // namespace EDA::Types

#endif // UI_DRONE_TYPES_HPP_
```

#### 2.2 Refactored Code Examples

**Before:**
```cpp
// Implicit types
size_t total_drones = approaching + static_count + receding;
(std::abs(drone.rssi - last_rssi_) > 1)
char buffer[32];
bool current = audio_get_enable_alerts(settings_);
uint32_t band_mhz = hardware_.get_spectrum_bandwidth() / 1000000ULL;
```

**After:**
```cpp
// Explicit type aliases
using namespace EDA::Types;
DroneCount total_drones = approaching + static_count + receding;
(RSSIValue)std::abs(drone.rssi - last_rssi_) > 1
TextBuffer32 buffer;
bool current = audio_get_enable_alerts(settings_);
FrequencyMHz band_mhz = Converters::hz_to_mhz(hardware_.get_spectrum_bandwidth());
```

**Memory Impact Analysis:**
- **Before:** Same memory footprint
- **After:** Same memory footprint (type aliases are zero-cost)
- **Savings:** 0 bytes (improved type safety)

**Performance Impact Analysis:**
- **Before:** Same performance
- **After:** Same performance (compiler optimizes away aliases)
- **Improvement:** Better compile-time type checking

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (type aliases are transparent)
- **Migration Effort:** Medium (find and replace implicit types)
- **Test Coverage:** Existing tests verify functionality

---

### Fix #H3: Logic Error (Always False Condition)

**Defect ID:** #9  
**Severity:** HIGH  
**Constraint Violation:** Logic error causes unreachable code  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:3293)

**Description:**
The condition `if (new_max > new_max + new_min)` is always false because `new_max` cannot be greater than itself plus a positive value.

**Current Code:**
```cpp
// ui_enhanced_drone_analyzer.cpp:3293
if (new_max > new_max + new_min) {
    nav_.display_modal("Error", "Frequency range overflow detected");
    return;
}
```

**Analysis:**
- `new_max` is a Frequency (uint64_t)
- `new_min` is a Frequency (uint64_t)
- `new_max + new_min` is always >= `new_max` (assuming non-negative values)
- The condition is always false → unreachable code

**Proposed Solution:**

#### 3.1 Corrected Logic

**Intended Logic:**
The code likely meant to check for integer overflow when adding `new_min` and `new_max`, or to validate that the range is valid (max > min).

**Option A: Check for Range Validity**
```cpp
// Check that max > min (valid range)
if (new_max <= new_min) {
    nav_.display_modal("Error", "Invalid frequency range (max must be > min)");
    return;
}
```

**Option B: Check for Integer Overflow**
```cpp
// Check for overflow when calculating range
FrequencyHz range = 0;
if (__builtin_add_overflow(new_max, new_min, &range)) {
    nav_.display_modal("Error", "Frequency range overflow detected");
    return;
}
```

**Option C: Check for Slice Width Validity**
```cpp
// Check that slice width fits within range
FrequencyHz range = new_max - new_min;
FrequencyHz slice_width = controller_.settings().wideband_slice_width_hz;

if (slice_width > range) {
    nav_.display_modal("Error", "Slice width exceeds frequency range");
    return;
}
```

**Option D: Check for Maximum Range**
```cpp
// Check that range doesn't exceed hardware limits
FrequencyHz range = new_max - new_min;
constexpr FrequencyHz MAX_RANGE_HZ = 1'000'000'000ULL;  // 1 GHz max range

if (range > MAX_RANGE_HZ) {
    nav_.display_modal("Error", "Frequency range too large (max 1 GHz)");
    return;
}
```

**Recommended Solution (Combination):**
```cpp
// ui_enhanced_drone_analyzer.cpp:3288-3296
// Validate frequency range
if (new_min < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ || 
    new_max > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
    nav_.display_modal("Error", "Frequency out of range (1MHz - 7.2GHz)");
    return;
}

// Check that max > min (valid range)
if (new_max <= new_min) {
    nav_.display_modal("Error", "Invalid frequency range (max must be > min)");
    return;
}

// Check that range is reasonable
FrequencyHz range = new_max - new_min;
constexpr FrequencyHz MIN_RANGE_HZ = 1'000'000ULL;      // 1 MHz minimum
constexpr FrequencyHz MAX_RANGE_HZ = 1'000'000'000ULL;  // 1 GHz maximum

if (range < MIN_RANGE_HZ) {
    nav_.display_modal("Error", "Frequency range too small (min 1 MHz)");
    return;
}

if (range > MAX_RANGE_HZ) {
    nav_.display_modal("Error", "Frequency range too large (max 1 GHz)");
    return;
}

// Check that slice width is valid
FrequencyHz slice_width = controller_.settings().wideband_slice_width_hz;
if (slice_width > range) {
    nav_.display_modal("Error", "Slice width exceeds frequency range");
    return;
}
```

**Memory Impact Analysis:**
- **Before:** Unreachable code (wasted ROM)
- **After:** Valid range checking
- **Savings:** ~50 bytes ROM (removed dead code)

**Performance Impact Analysis:**
- **Before:** Always false check (1-2 cycles)
- **After:** Valid range checks (5-10 cycles)
- **Improvement:** Actually validates input

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (fixes bug)
- **Migration Effort:** Low (single line change)
- **Test Coverage:** Requires range validation tests

---

### Fix #H4: Undefined Constant Reference

**Defect ID:** #10  
**Severity:** HIGH  
**Constraint Violation:** Reference to undefined constant  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:3618)

**Description:**
The code references `InitTiming::TIMEOUT_MS` which is not defined, causing a compilation error or undefined behavior.

**Current Code:**
```cpp
// ui_enhanced_drone_analyzer.cpp:3618
if (elapsed > MS2ST(InitTiming::TIMEOUT_MS)) {
    // InitTiming::TIMEOUT_MS is not defined!
```

**Proposed Solution:**

#### 4.1 Define Missing Constants

**Create Initialization Timing Constants:**
```cpp
// ui_drone_init_timing.hpp
#ifndef UI_DRONE_INIT_TIMING_HPP_
#define UI_DRONE_INIT_TIMING_HPP_

#include <cstdint>

namespace EDA::InitTiming {

// Initialization timeout (10 seconds total)
constexpr uint32_t TIMEOUT_MS = 10000;

// Phase-specific timeouts
constexpr uint32_t PHASE_ALLOCATE_TIMEOUT_MS = 1000;    // 1 second
constexpr uint32_t PHASE_DATABASE_TIMEOUT_MS = 5000;    // 5 seconds
constexpr uint32_t PHASE_HARDWARE_TIMEOUT_MS = 2000;     // 2 seconds
constexpr uint32_t PHASE_UI_TIMEOUT_MS = 1000;          // 1 second
constexpr uint32_t PHASE_SETTINGS_TIMEOUT_MS = 2000;    // 2 seconds
constexpr uint32_t PHASE_FINALIZE_TIMEOUT_MS = 1000;    // 1 second

// Retry intervals
constexpr uint32_t RETRY_INTERVAL_MS = 100;             // 100ms between retries
constexpr uint32_t MAX_RETRIES = 10;                     // Maximum retry attempts

// Progress update interval
constexpr uint32_t PROGRESS_UPDATE_INTERVAL_MS = 100;    // 100ms

} // namespace EDA::InitTiming

#endif // UI_DRONE_INIT_TIMING_HPP_
```

**Or Add to Existing Constants File:**
```cpp
// eda_constants.hpp (add to existing file)
namespace EDA::Constants::InitTiming {
    constexpr uint32_t TIMEOUT_MS = 10000;
    constexpr uint32_t PHASE_ALLOCATE_TIMEOUT_MS = 1000;
    constexpr uint32_t PHASE_DATABASE_TIMEOUT_MS = 5000;
    constexpr uint32_t PHASE_HARDWARE_TIMEOUT_MS = 2000;
    constexpr uint32_t PHASE_UI_TIMEOUT_MS = 1000;
    constexpr uint32_t PHASE_SETTINGS_TIMEOUT_MS = 2000;
    constexpr uint32_t PHASE_FINALIZE_TIMEOUT_MS = 1000;
}
```

**Usage:**
```cpp
// ui_enhanced_drone_analyzer.cpp:3618
if (elapsed > MS2ST(EDA::InitTiming::TIMEOUT_MS)) {
    // Now properly defined
```

**Memory Impact Analysis:**
- **Before:** Undefined reference (compilation error)
- **After:** Constants in Flash (0 RAM)
- **Savings:** Enables compilation

**Performance Impact Analysis:**
- **Before:** N/A (compilation error)
- **After:** Direct constant use
- **Improvement:** Enables code to compile

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (defines missing constant)
- **Migration Effort:** Low (add constant definition)
- **Test Coverage:** Existing tests verify functionality

---

### Fix #H5: Spaghetti Logic (325 Lines Initialization)

**Defect ID:** #11  
**Severity:** HIGH  
**Constraint Violation:** Diamond Code requires modular, testable code  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:3600-3925)

**Description:**
The `step_deferred_initialization()` function is 325 lines long with deeply nested conditionals and complex state machine logic. This makes the code difficult to understand, test, and maintain.

**Current Code Structure:**
```cpp
// ui_enhanced_drone_analyzer.cpp:3600-3925
void EnhancedDroneSpectrumAnalyzerView::step_deferred_initialization() noexcept {
    // 325 lines of initialization logic
    // - Guard clauses
    // - Timeout checks
    // - State machine dispatch
    // - Phase functions (6 phases)
    // - Error handling
    // - Status updates
}
```

**Proposed Solution:**

#### 5.1 Refactoring Strategy

**Strategy 1: State Machine Pattern**
```cpp
// ui_drone_init_state_machine.hpp
#ifndef UI_DRONE_INIT_STATE_MACHINE_HPP_
#define UI_DRONE_INIT_STATE_MACHINE_HPP_

#include <functional>
#include <array>
#include "ui_drone_types.hpp"

namespace EDA::Initialization {

// Phase result
enum class PhaseResult : uint8_t {
    SUCCESS = 0,
    PENDING = 1,
    ERROR = 2
};

// Phase context
struct PhaseContext {
    InitState current_state;
    InitError error_code;
    uint32_t elapsed_ms;
    bool timeout;
};

// Phase function signature
using PhaseFunc = std::function<PhaseResult(const PhaseContext&)>;

// Phase descriptor
struct PhaseDescriptor {
    const char* name;
    const char* status_message;
    PhaseFunc execute;
    uint32_t timeout_ms;
};

// State machine
class InitializationStateMachine {
public:
    InitializationStateMachine();
    
    // Execute current phase
    PhaseResult execute_phase(const PhaseContext& context);
    
    // Get current phase descriptor
    const PhaseDescriptor* get_current_phase() const;
    
    // Transition to next phase
    bool transition_to_next();
    
    // Handle error
    void handle_error(InitError error);
    
private:
    std::array<PhaseDescriptor, 6> phases_;
    size_t current_phase_index_;
    InitState current_state_;
    InitError last_error_;
};

} // namespace EDA::Initialization

#endif // UI_DRONE_INIT_STATE_MACHINE_HPP_
```

**Strategy 2: Separate Phase Classes**
```cpp
// ui_drone_init_phases.hpp
#ifndef UI_DRONE_INIT_PHASES_HPP_
#define UI_DRONE_INIT_PHASES_HPP_

#include "ui_drone_init_state_machine.hpp"

namespace EDA::Initialization::Phases {

// Base phase class
class BasePhase {
protected:
    EnhancedDroneSpectrumAnalyzerView& view_;
    
public:
    explicit BasePhase(EnhancedDroneSpectrumAnalyzerView& view);
    virtual ~BasePhase() = default;
    
    virtual PhaseResult execute(const PhaseContext& context) = 0;
    virtual const char* get_name() const = 0;
    virtual const char* get_status_message() const = 0;
    virtual uint32_t get_timeout_ms() const = 0;
};

// Phase 1: Allocate buffers
class AllocateBuffersPhase : public BasePhase {
public:
    explicit AllocateBuffersPhase(EnhancedDroneSpectrumAnalyzerView& view);
    PhaseResult execute(const PhaseContext& context) override;
    const char* get_name() const override { return "Allocate Buffers"; }
    const char* get_status_message() const override { return "Allocating buffers..."; }
    uint32_t get_timeout_ms() const override { return InitTiming::PHASE_ALLOCATE_TIMEOUT_MS; }
};

// Phase 2: Load database
class LoadDatabasePhase : public BasePhase {
public:
    explicit LoadDatabasePhase(EnhancedDroneSpectrumAnalyzerView& view);
    PhaseResult execute(const PhaseContext& context) override;
    const char* get_name() const override { return "Load Database"; }
    const char* get_status_message() const override { return "Loading database..."; }
    uint32_t get_timeout_ms() const override { return InitTiming::PHASE_DATABASE_TIMEOUT_MS; }
};

// Phase 3: Initialize hardware
class InitHardwarePhase : public BasePhase {
public:
    explicit InitHardwarePhase(EnhancedDroneSpectrumAnalyzerView& view);
    PhaseResult execute(const PhaseContext& context) override;
    const char* get_name() const override { return "Initialize Hardware"; }
    const char* get_status_message() const override { return "Initializing hardware..."; }
    uint32_t get_timeout_ms() const override { return InitTiming::PHASE_HARDWARE_TIMEOUT_MS; }
};

// Phase 4: Setup UI
class SetupUIPhase : public BasePhase {
public:
    explicit SetupUIPhase(EnhancedDroneSpectrumAnalyzerView& view);
    PhaseResult execute(const PhaseContext& context) override;
    const char* get_name() const override { return "Setup UI"; }
    const char* get_status_message() const override { return "Setting up UI..."; }
    uint32_t get_timeout_ms() const override { return InitTiming::PHASE_UI_TIMEOUT_MS; }
};

// Phase 5: Load settings
class LoadSettingsPhase : public BasePhase {
public:
    explicit LoadSettingsPhase(EnhancedDroneSpectrumAnalyzerView& view);
    PhaseResult execute(const PhaseContext& context) override;
    const char* get_name() const override { return "Load Settings"; }
    const char* get_status_message() const override { return "Loading settings..."; }
    uint32_t get_timeout_ms() const override { return InitTiming::PHASE_SETTINGS_TIMEOUT_MS; }
};

// Phase 6: Finalize
class FinalizePhase : public BasePhase {
public:
    explicit FinalizePhase(EnhancedDroneSpectrumAnalyzerView& view);
    PhaseResult execute(const PhaseContext& context) override;
    const char* get_name() const override { return "Finalize"; }
    const char* get_status_message() const override { return "Finalizing..."; }
    uint32_t get_timeout_ms() const override { return InitTiming::PHASE_FINALIZE_TIMEOUT_MS; }
};

} // namespace EDA::Initialization::Phases

#endif // UI_DRONE_INIT_PHASES_HPP_
```

**Strategy 3: Simplified Initialization Function**
```cpp
// ui_enhanced_drone_analyzer.cpp (refactored)
void EnhancedDroneSpectrumAnalyzerView::step_deferred_initialization() noexcept {
    // Guard clause for re-entrancy protection
    if (initialization_in_progress_) {
        return;
    }
    initialization_in_progress_ = true;
    
    // Handle ERROR state
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0,
                                      ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]);
        initialization_in_progress_ = false;
        return;
    }
    
    // Check timeout
    PhaseContext context{
        .current_state = init_state_,
        .error_code = init_error_,
        .elapsed_ms = chTimeNow() - init_start_time_,
        .timeout = false
    };
    
    if (context.elapsed_ms > InitTiming::TIMEOUT_MS) {
        handle_init_timeout();
        initialization_in_progress_ = false;
        return;
    }
    
    // Execute current phase
    PhaseResult result = state_machine_.execute_phase(context);
    
    // Handle phase result
    switch (result) {
        case PhaseResult::SUCCESS:
            // Transition to next phase
            if (!state_machine_.transition_to_next()) {
                // All phases complete
                init_state_ = InitState::FULLY_INITIALIZED;
                status_bar_.update_normal_status("EDA", "Ready");
                start_scanning_thread();
            } else {
                // Update status for next phase
                const auto* phase = state_machine_.get_current_phase();
                status_bar_.update_normal_status("INIT", phase->status_message);
            }
            break;
            
        case PhaseResult::PENDING:
            // Phase still in progress, retry next time
            break;
            
        case PhaseResult::ERROR:
            // Handle error
            state_machine_.handle_error(init_error_);
            status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0,
                                          ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]);
            break;
    }
    
    initialization_in_progress_ = false;
}

void EnhancedDroneSpectrumAnalyzerView::handle_init_timeout() {
    // Proper cleanup in timeout path
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings_);
    
    // Set error state
    init_state_ = InitState::INITIALIZATION_ERROR;
    init_error_ = InitError::GENERAL_TIMEOUT;
    
    // Update UI
    status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0, "Init timeout!");
}
```

**Memory Impact Analysis:**
- **Before:** Single 325-line function
- **After:** Multiple small functions (~50 lines each)
- **Savings:** 0 bytes (same code size, better organization)

**Performance Impact Analysis:**
- **Before:** Single function call
- **After:** Multiple small function calls (inline optimization)
- **Improvement:** Better code organization, same performance

**Risk Assessment:**
- **Risk Level:** MEDIUM
- **Breaking Changes:** None (internal refactoring)
- **Migration Effort:** High (requires significant refactoring)
- **Test Coverage:** Requires unit tests for each phase

---

## Summary of High Priority Fixes

| Fix ID | Description | Priority | Risk | Effort |
|--------|-------------|----------|------|--------|
| #H1 | Magic numbers throughout codebase | 2 | LOW | Medium |
| #H2 | Type ambiguity throughout codebase | 2 | LOW | Medium |
| #H3 | Logic error (always false condition) | 2 | LOW | Low |
| #H4 | Undefined constant reference | 2 | LOW | Low |
| #H5 | Spaghetti logic (325 lines initialization) | 2 | MEDIUM | High |

**Total High Priority Fixes:** 5  
**Estimated Implementation Effort:** Medium-High  
**Risk Mitigation:** All fixes improve code quality and maintainability

---

## Next Steps

1. Review this blueprint document
2. Refer to `stage2_critical_fixes.md` for Priority 1 critical fixes
3. Refer to `stage2_medium_low_priority_fixes.md` for Priority 3 and 4 improvements

**Note:** This is Stage 2 of a 4-stage Diamond Code pipeline. No code changes should be made until Stage 3 (Implementation) begins.
