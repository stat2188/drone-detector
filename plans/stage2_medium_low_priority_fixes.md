# Stage 2: Architect's Blueprint - Medium & Low Priority Improvements

**Diamond Code Pipeline - Stage 2: Architect's Blueprint**  
**Enhanced Drone Analyzer Codebase**  
**Target Platform: STM32F405 (ARM Cortex-M4, 128KB RAM)**

---

## Document Overview

This document contains **Priority 3 (Medium Priority)** and **Priority 4 (Low Priority)** improvements that enhance code quality, maintainability, and user experience. These fixes are nice to have but don't cause immediate failures.

**Related Documents:**
- `stage2_critical_fixes.md` - Priority 1 critical fixes
- `stage2_high_priority_fixes.md` - Priority 2 high priority fixes

---

## Section 3: Medium Priority Improvements (Priority 3 - NICE TO FIX)

### Fix #M1: Dead Code (Commented-Out Code)

**Defect ID:** #14  
**Severity:** MEDIUM  
**Constraint Violation:** Diamond Code requires clean code without dead code  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:2138-3971)

**Description:**
Large blocks of commented-out code clutter the codebase, making it difficult to read and maintain. Dead code should be removed or properly documented.

**Current Code Examples:**
```cpp
// Example of commented-out code (actual locations vary)
// void old_function() {
//     // This code is no longer used
//     // but kept for reference
// }
```

**Proposed Solution:**

#### 1.1 Dead Code Removal Strategy

**Strategy 1: Remove Dead Code**
```cpp
// Remove commented-out code entirely
// If the code might be needed in the future, use version control instead
```

**Strategy 2: Document Reason for Keeping**
```cpp
// If code must be kept, document why:
// TODO: Re-enable this feature when hardware supports XYZ
// Disabled due to issue #123
// void old_function() {
//     // Implementation...
// }
```

**Strategy 3: Move to Separate File**
```cpp
// Create ui_enhanced_drone_analyzer_deprecated.hpp
// Keep deprecated code there for reference
// with clear deprecation notices
```

**Memory Impact Analysis:**
- **Before:** Dead code in ROM (wasted space)
- **After:** Clean codebase
- **Savings:** ~1-2KB ROM (estimated)

**Performance Impact Analysis:**
- **Before:** No impact (code not executed)
- **After:** No impact
- **Improvement:** Better code readability

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (removing dead code)
- **Migration Effort:** Low (remove commented blocks)
- **Test Coverage:** No tests needed (dead code not executed)

---

### Fix #M2: Stack Overflow Risk in paint()

**Defect ID:** #15  
**Severity:** MEDIUM  
**Constraint Violation:** Stack usage monitoring needed  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:3418-3492)

**Description:**
The `paint()` method uses significant stack (~1.6KB) and could cause stack overflow on constrained systems. While the code already includes stack monitoring, additional safeguards are recommended.

**Current Code:**
```cpp
// ui_enhanced_drone_analyzer.cpp:3418-3492
void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
    // Stack monitoring to prevent stack overflow
    // paint() method uses ~1.6KB of stack per call
    // STACK OPTIMIZATION: Reduced stack requirement to 1792 bytes (1.75KB)
    StackMonitor stack_monitor;
    constexpr size_t PAINT_STACK_REQUIRED = 1792;  // 1.75KB for paint() method (reduced from 2KB)
    
    // Guard clause: Return early if insufficient stack
    if (!stack_monitor.is_stack_safe(PAINT_STACK_REQUIRED)) {
        // Cannot safely render - skip this frame
        return;
    }
    
    // ... rest of paint() code ...
}
```

**Proposed Solution:**

#### 2.1 Enhanced Stack Safety

**Strategy 1: Stack Budget Analysis**
```cpp
// Create stack budget per function
namespace StackBudget {
    // Maximum stack usage per function (measured with stack usage analysis tools)
    constexpr size_t PAINT_STACK_BUDGET = 2048;      // 2KB
    constexpr size_t UPDATE_STACK_BUDGET = 1024;     // 1KB
    constexpr size_t SCAN_STACK_BUDGET = 1536;        // 1.5KB
    constexpr size_t INIT_STACK_BUDGET = 3072;        // 3KB
    
    // Safety margin (20%)
    constexpr float SAFETY_MARGIN = 0.2f;
    
    // Calculate required stack with safety margin
    constexpr size_t paint_stack_required() {
        return static_cast<size_t>(PAINT_STACK_BUDGET * (1.0f + SAFETY_MARGIN));
    }
}
```

**Strategy 2: Stack Usage Profiling**
```cpp
// Stack usage profiler
class StackProfiler {
    static thread_local uintptr_t min_stack_ptr_;
    static thread_local uintptr_t max_stack_ptr_;
    static thread_local const char* current_function_;
    
public:
    static void enter_function(const char* name) {
        current_function_ = name;
        volatile uint8_t stack_var;
        uintptr_t current_sp = reinterpret_cast<uintptr_t>(&stack_var);
        
        if (current_sp < min_stack_ptr_) min_stack_ptr_ = current_sp;
        if (current_sp > max_stack_ptr_) max_stack_ptr_ = current_sp;
    }
    
    static void exit_function() {
        current_function_ = nullptr;
    }
    
    static size_t get_max_stack_usage() {
        return max_stack_ptr_ - min_stack_ptr_;
    }
    
    static const char* get_current_function() {
        return current_function_;
    }
};

// RAII wrapper for automatic profiling
class ScopedStackProfile {
    const char* function_name_;
    
public:
    explicit ScopedStackProfile(const char* name) : function_name_(name) {
        StackProfiler::enter_function(name);
    }
    
    ~ScopedStackProfile() {
        StackProfiler::exit_function();
    }
};
```

**Strategy 3: Paint() Optimization**
```cpp
// Optimized paint() with reduced stack usage
void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
    // Stack safety check
    if (!StackSafety::StackMonitor::is_stack_safe(StackBudget::paint_stack_required())) {
        // Insufficient stack - skip this frame
        return;
    }
    
    // Profile stack usage (debug builds only)
    #ifdef DEBUG
    ScopedStackProfile profiler("paint");
    #endif
    
    // Early returns for error states
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        render_error_state(painter);
        return;
    }
    
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        render_loading_state(painter);
        return;
    }
    
    // Main rendering (only if fully initialized)
    if (display_controller_.are_buffers_valid()) {
        display_controller_.render_bar_spectrum(painter);
        
        if (display_controller_.get_display_mode() == 
            DroneDisplayController::DisplayRenderMode::HISTOGRAM) {
            display_controller_.render_histogram(painter);
        } else {
            display_controller_.clear_histogram_area(painter);
        }
    }
    
    // Continue initialization after rendering
    continue_initialization();
}

// Separate rendering functions to reduce stack depth
void EnhancedDroneSpectrumAnalyzerView::render_error_state(Painter& painter) {
    painter.fill_rectangle({0, 0, screen_width, screen_height}, Color::black());
    painter.draw_string({10, 80}, Style{font::fixed_8x16, Color::red(), Color::black()}, 
                       "INIT ERROR");
    painter.draw_string({10, 100}, Style{font::fixed_8x16, Color::white(), Color::black()}, 
                       ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]);
    painter.draw_string({10, 130}, Style{font::fixed_8x16, Color::yellow(), Color::black()}, 
                       "Press BACK to exit");
}

void EnhancedDroneSpectrumAnalyzerView::render_loading_state(Painter& painter) {
    size_t phase_idx = static_cast<size_t>(init_state_);
    if (phase_idx < 7) {
        painter.draw_string({10, 80}, Style{font::fixed_8x16, Color::white(), Color::black()}, 
                           "Loading...");
        painter.draw_string({10, 100}, Style{font::fixed_8x16, Color::green(), Color::black()}, 
                           INIT_STATUS_MESSAGES[phase_idx]);
        
        // Progress bar
        uint8_t progress = static_cast<uint8_t>(phase_idx * 16);
        if (progress > 100) progress = 100;
        
        painter.fill_rectangle({10, 120, 100, 10}, Color::dark_grey());
        painter.fill_rectangle({10, 120, progress, 10}, Color::green());
    }
}
```

**Memory Impact Analysis:**
- **Before:** ~1.6KB stack per paint() call
- **After:** Same stack usage, better monitoring
- **Savings:** 0 bytes (improved safety)

**Performance Impact Analysis:**
- **Before:** ~1.6KB stack usage
- **After:** ~1.6KB stack usage (same)
- **Improvement:** Better stack safety monitoring

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (internal refactoring)
- **Migration Effort:** Medium (requires stack analysis)
- **Test Coverage:** Requires stack usage testing

---

### Fix #M3: Inconsistent Naming Convention

**Defect ID:** #16  
**Severity:** MEDIUM  
**Constraint Violation:** Diamond Code requires consistent naming  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp) (throughout)

**Description:**
Inconsistent naming conventions make the codebase difficult to read and maintain. Mixed use of snake_case, camelCase, and other styles.

**Current Code Examples:**
```cpp
// Inconsistent naming examples
scanning_active_           // snake_case (good)
last_scan_error_            // snake_case (good)
freq_db_ptr_               // snake_case (good)
display_controller_         // snake_case (good)
ui_controller_             // snake_case (good)

// Potential inconsistencies
init_state_                // snake_case (good)
init_error_                // snake_case (good)
init_start_time_           // snake_case (good)
initialization_in_progress_ // snake_case (good)
```

**Proposed Solution:**

#### 3.1 Naming Convention Guidelines

**Establish Consistent Naming Rules:**
```cpp
// NAMING_CONVENTION.md

// 1. Member variables: snake_case with trailing underscore
class MyClass {
    int member_variable_;
    bool is_active_;
    std::string_view name_;
};

// 2. Local variables: snake_case without underscore
void my_function() {
    int local_variable;
    bool is_valid;
    std::string_view name;
};

// 3. Function names: snake_case
void do_something();
bool is_valid();
std::string_view get_name();

// 4. Constants: SCREAMING_SNAKE_CASE
constexpr int MAX_VALUE = 100;
constexpr float PI = 3.14159f;

// 5. Type names: PascalCase
class MyClass {};
struct MyStruct {};
enum class MyEnum {};

// 6. Enum values: SCREAMING_SNAKE_CASE
enum class MyEnum {
    VALUE_ONE,
    VALUE_TWO,
    VALUE_THREE
};

// 7. Template parameters: PascalCase
template<typename T>
class Container {};

// 8. Macro names: SCREAMING_SNAKE_CASE
#define MY_MACRO(x) ((x) * 2)

// 9. File names: snake_case
// my_class.hpp
// my_class.cpp

// 10. Namespace names: snake_case
namespace my_namespace {}
```

**Refactoring Script:**
```cpp
// Example refactoring checklist
// - Scan all member variables for consistent trailing underscore
// - Scan all function names for snake_case
// - Scan all constants for SCREAMING_SNAKE_CASE
// - Scan all type names for PascalCase
// - Scan all enum values for SCREAMING_SNAKE_CASE
```

**Memory Impact Analysis:**
- **Before:** Inconsistent naming
- **After:** Consistent naming
- **Savings:** 0 bytes (naming only)

**Performance Impact Analysis:**
- **Before:** No impact
- **After:** No impact
- **Improvement:** Better code readability

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (naming only)
- **Migration Effort:** Medium (find and rename)
- **Test Coverage:** No tests needed (naming only)

---

### Fix #M4: Missing Documentation

**Defect ID:** #17  
**Severity:** MEDIUM  
**Constraint Violation:** Diamond Code requires comprehensive documentation  
**Files Affected:**
- [`ui_enhanced_drone_analyzer.cpp`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp) (throughout)

**Description:**
Lack of comprehensive documentation makes the codebase difficult to understand, maintain, and extend. Diamond Code requires clear documentation for all public interfaces and complex algorithms.

**Proposed Solution:**

#### 4.1 Documentation Standards

**File Header Template:**
```cpp
// * * @file ui_enhanced_drone_analyzer.cpp
// * * @brief Enhanced Drone Analyzer UI Implementation
// * * 
// * * DESCRIPTION:
// * * This file implements the main UI for the Enhanced Drone Analyzer application.
// * * It provides real-time spectrum analysis, drone detection, and threat assessment.
// * * 
// * * FEATURES:
// * * - Real-time spectrum display with waterfall visualization
// * * - Automatic drone detection and tracking
// * * - Threat level assessment (NONE, LOW, MEDIUM, HIGH, CRITICAL)
// * * - Movement trend analysis (APPROACHING, STATIC, RECEIVING)
// * * - Configurable scanning parameters
// * * - Detection logging to SD card
// * * - Audio alerts for detected threats
// * * 
// * * MEMORY:
// * * - Stack: ~4KB maximum (with safety margin)
// * * - Heap: 0 bytes (no dynamic allocation)
// * * - Flash: ~50KB (code + constants)
// * * 
// * * PERFORMANCE:
// * * - Scan cycle: ~100-2000ms (adaptive based on threat level)
// * * - Paint cycle: ~16ms (60 FPS)
// * * - Detection latency: ~50-100ms
// * * 
// * * THREADS:
// * * - Main UI thread (ChibiOS)
// * * - Scanning thread (background)
// * * - Database loading thread (async)
// * * 
// * * SYNCHRONIZATION:
// * * - data_mutex: Protects shared scanner state
// * * - sd_card_mutex: Protects SD card operations
// * * - spectrum_mutex: Protects spectrum data
// * * - histogram_mutex: Protects histogram data
// * * 
// * * LOCK ORDERING:
// * * 1. data_mutex (LEVEL_0)
// * * 2. sd_card_mutex (LEVEL_1)
// * * 3. spectrum_mutex (LEVEL_2)
// * * 4. histogram_mutex (LEVEL_3)
// * * 
// * * @author Enhanced Drone Analyzer Team
// * * @version 2.0.0
// * * @date 2025-02-25
```

**Class Documentation Template:**
```cpp
// * * @class DroneScanner
// * * @brief Core scanning engine for drone detection
// * * 
// * * DESCRIPTION:
// * * The DroneScanner class implements the core scanning logic for detecting
// * * drones across multiple frequency bands. It manages frequency database
// * * scanning, signal processing, and drone tracking.
// * * 
// * * RESPONSIBILITIES:
// * * - Manage scanning state (start/stop/pause)
// * * - Scan frequency database entries
// * * - Process RSSI measurements
// * * - Track detected drones
// * * - Calculate movement trends
// * * - Assess threat levels
// * * 
// * * THREAD SAFETY:
// * * - Public methods are thread-safe when called from scanning thread
// * * - Shared state protected by data_mutex
// * * - Use getTrackedDrone() for thread-safe drone access
// * * 
// * * MEMORY:
// * * - Stack: ~1KB per scan cycle
// * * - Heap: 0 bytes (no dynamic allocation)
// * * - Static: ~2KB (tracked drones, buffers)
// * * 
// * * USAGE EXAMPLE:
// * * @code
// * * DroneScanner scanner(settings);
// * * scanner.initialize_database_async();
// * * // Wait for initialization...
// * * scanner.start_scanning();
// * * while (scanner.is_scanning()) {
// * *     auto drones = scanner.get_all_tracked_drones();
// * *     // Process drones...
// * * }
// * * scanner.stop_scanning();
// * * @endcode
class DroneScanner {
    // ... implementation ...
};
```

**Function Documentation Template:**
```cpp
// * * @brief Perform a single database scan cycle
// * * 
// * * DESCRIPTION:
// * * Scans a batch of frequencies from the database, measures RSSI,
// * * and processes detections. This method is called repeatedly by the
// * * scanning thread.
// * * 
// * * ALGORITHM:
// * * 1. Check stack canary for overflow detection
// * * 2. Acquire database entries under lock
// * * 3. For each entry in batch:
// * *    a. Tune radio to target frequency
// * *    b. Wait for PLL stabilization
// * *    c. Acquire RSSI measurement
// * *    d. Process detection if signal captured
// * * 4. Update tracked drones
// * * 5. Remove stale drones
// * * 6. Log detections if needed
// * * 
// * * THREAD SAFETY:
// * * - Thread-safe (called from scanning thread only)
// * * - Acquires data_mutex for database access
// * * - Updates shared state under lock
// * * 
// * * PARAMETERS:
// * * @param hardware Reference to hardware controller for radio operations
// * * 
// * * SIDE EFFECTS:
// * * - Updates tracked_drones_ array
// * * - Updates scan_cycles_ counter
// * * - Updates total_detections_ counter
// * * - May write detection logs to SD card
// * * 
// * * EXCEPTION SAFETY:
// * * - noexcept (no exceptions thrown)
// * * 
// * * PERFORMANCE:
// * * - Time: O(N) where N = batch size (typically 10)
// * * - Stack: ~1KB
// * * - Memory: 0 bytes heap
// * * 
// * * @note This method should only be called from the scanning thread
// * * @see start_scanning(), stop_scanning()
void DroneScanner::perform_database_scan_cycle(DroneHardwareController& hardware) noexcept {
    // ... implementation ...
}
```

**Algorithm Documentation Template:**
```cpp
// * * @brief Calculate movement trend from RSSI history
// * * 
// * * ALGORITHM:
// * * The movement trend is calculated by analyzing the RSSI history
// * * of a tracked drone over time. The algorithm uses a weighted
// * * average of recent RSSI changes to determine if the drone is
// * * approaching, static, or receding.
// * * 
// * * STEPS:
// * * 1. Collect last N RSSI measurements (N = MOVEMENT_TREND_MIN_HISTORY)
// * * 2. Calculate average RSSI change per measurement
// * * 3. Compare change to thresholds:
// * *    - If change > TREND_THRESHOLD_APPROACHING: APPROACHING
// * *    - If change < TREND_THRESHOLD_RECEEDING: RECEIVING
// * *    - Otherwise: STATIC
// * * 4. Return trend
// * * 
// * * MATHEMATICAL MODEL:
// * * Let R[i] be the RSSI at time i, where i = 0 is the oldest
// * * measurement and i = N-1 is the newest.
// * * 
// * * Average change = (R[N-1] - R[0]) / (N-1)
// * * 
// * * If average change > APPROACHING_THRESHOLD: APPROACHING
// * * If average change < RECEIVING_THRESHOLD: RECEIVING
// * * Otherwise: STATIC
// * * 
// * * EXAMPLE:
// * * RSSI history: [-85, -82, -79, -76, -73]
// * * Average change = (-73 - (-85)) / 4 = 12 / 4 = 3
// * * Since 3 > APPROACHING_THRESHOLD (3): APPROACHING
// * * 
// * * PARAMETERS:
// * * @param rssi_history Array of RSSI measurements
// * * @param history_size Number of measurements in array
// * * 
// * * @return Movement trend (APPROACHING, STATIC, or RECEIVING)
// * * 
// * * @note Requires at least MOVEMENT_TREND_MIN_HISTORY measurements
// * * @see MovementTrend enum
MovementTrend calculate_movement_trend(const int16_t* rssi_history, 
                                         size_t history_size) noexcept {
    // ... implementation ...
}
```

**Memory Impact Analysis:**
- **Before:** Minimal documentation
- **After:** Comprehensive documentation
- **Savings:** 0 bytes (documentation in comments)

**Performance Impact Analysis:**
- **Before:** No impact
- **After:** No impact
- **Improvement:** Better code maintainability

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (documentation only)
- **Migration Effort:** High (requires significant documentation effort)
- **Test Coverage:** No tests needed (documentation only)

---

## Section 4: Low Priority Improvements (Priority 4 - NICE TO HAVE)

### Fix #L1: Code Style and Formatting

**Defect ID:** N/A  
**Severity:** LOW  
**Constraint Violation:** Diamond Code recommends consistent code style  
**Files Affected:**
- All files in the codebase

**Description:**
Inconsistent code formatting and style make the codebase difficult to read. Automated formatting tools should be used to ensure consistency.

**Proposed Solution:**

#### 1.1 Code Style Configuration

**.clang-format Configuration:**
```yaml
# .clang-format for Enhanced Drone Analyzer
BasedOnStyle: Google
Language: Cpp

# Indentation
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100

# Braces
BreakBeforeBraces: Attach
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: False

# Spacing
SpaceAfterCStyleCast: false
SpaceBeforeParens: ControlStatements
SpacesInParentheses: false
SpacesInSquareBrackets: false

# Alignment
AlignAfterOpenBracket: Align
AlignConsecutiveAssignments: false
AlignConsecutiveDeclarations: false
AlignOperands: true

# Includes
SortIncludes: true
IncludeBlocks: Regroup

# Comments
ReflowComments: true

# Namespaces
NamespaceIndentation: None

# Pointers
DerivePointerAlignment: false
PointerAlignment: Left
```

**Pre-commit Hook:**
```bash
#!/bin/bash
# .git/hooks/pre-commit

# Run clang-format
echo "Running clang-format..."
clang-format --dry-run --Werror firmware/application/apps/enhanced_drone_analyzer/*.{hpp,cpp}

if [ $? -ne 0 ]; then
    echo "Code formatting issues found. Run 'clang-format -i' to fix."
    exit 1
fi

# Run clang-tidy
echo "Running clang-tidy..."
clang-tidy firmware/application/apps/enhanced_drone_analyzer/*.{hpp,cpp} -- -I firmware/application

if [ $? -ne 0 ]; then
    echo "Code quality issues found."
    exit 1
fi

exit 0
```

**Memory Impact Analysis:**
- **Before:** Inconsistent formatting
- **After:** Consistent formatting
- **Savings:** 0 bytes (formatting only)

**Performance Impact Analysis:**
- **Before:** No impact
- **After:** No impact
- **Improvement:** Better code readability

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (formatting only)
- **Migration Effort:** Low (automated)
- **Test Coverage:** No tests needed (formatting only)

---

### Fix #L2: Unit Test Coverage

**Defect ID:** N/A  
**Severity:** LOW  
**Constraint Violation:** Diamond Code recommends comprehensive testing  
**Files Affected:**
- Test files in `firmware/test/enhanced_drone_analyzer/`

**Description:**
Limited unit test coverage reduces confidence in code changes. Diamond Code recommends comprehensive testing for all critical components.

**Proposed Solution:**

#### 2.1 Test Coverage Goals

**Target Coverage:**
- Line coverage: 80%
- Branch coverage: 70%
- Function coverage: 90%

**Critical Components to Test:**
1. DroneScanner core logic
2. Frequency database operations
3. Settings persistence
4. Stack safety mechanisms
5. Thread synchronization
6. Movement trend calculation
7. Threat level assessment

**Test Structure:**
```cpp
// test_drone_scanner.cpp
#include <doctest.h>
#include "ui_enhanced_drone_analyzer.hpp"

TEST_SUITE("DroneScanner") {
    
    TEST_CASE("Initialization") {
        // Test scanner initialization
        DroneAnalyzerSettings settings;
        DroneScanner scanner(settings);
        
        CHECK(scanner.is_initialization_complete() == false);
        CHECK(scanner.is_scanning() == false);
    }
    
    TEST_CASE("Start/Stop Scanning") {
        DroneAnalyzerSettings settings;
        DroneScanner scanner(settings);
        
        // Initialize scanner
        scanner.initialize_database_async();
        while (!scanner.is_initialization_complete()) {
            chThdSleepMilliseconds(10);
        }
        
        // Start scanning
        scanner.start_scanning();
        CHECK(scanner.is_scanning() == true);
        
        // Stop scanning
        scanner.stop_scanning();
        CHECK(scanner.is_scanning() == false);
    }
    
    TEST_CASE("Tracked Drones") {
        DroneAnalyzerSettings settings;
        DroneScanner scanner(settings);
        
        // Initialize scanner
        scanner.initialize_database_async();
        while (!scanner.is_initialization_complete()) {
            chThdSleepMilliseconds(10);
        }
        
        // Add tracked drone
        scanner.add_tracked_drone(2400000000ULL, DroneType::DJI, 
                                  ThreatLevel::HIGH, -70);
        
        // Get tracked drone
        auto drone = scanner.getTrackedDrone(0);
        CHECK(drone.frequency == 2400000000ULL);
        CHECK(drone.drone_type == static_cast<uint8_t>(DroneType::DJI));
        CHECK(drone.threat_level == static_cast<uint8_t>(ThreatLevel::HIGH));
    }
    
    TEST_CASE("Movement Trend Calculation") {
        // Test approaching trend
        int16_t approaching_history[] = {-85, -82, -79, -76, -73};
        auto trend = calculate_movement_trend(approaching_history, 5);
        CHECK(trend == MovementTrend::APPROACHING);
        
        // Test static trend
        int16_t static_history[] = {-80, -80, -80, -80, -80};
        trend = calculate_movement_trend(static_history, 5);
        CHECK(trend == MovementTrend::STATIC);
        
        // Test receiving trend
        int16_t receiving_history[] = {-73, -76, -79, -82, -85};
        trend = calculate_movement_trend(receiving_history, 5);
        CHECK(trend == MovementTrend::RECEIVING);
    }
    
    TEST_CASE("Stack Safety") {
        // Test stack canary
        StackSafety::init_stack_canary();
        CHECK(StackSafety::check_stack_canary() == true);
        
        // Simulate stack overflow
        StackSafety::stack_canary_value_ = 0xDEADBEEF + 1;
        CHECK(StackSafety::check_stack_canary() == false);
        
        // Reset canary
        StackSafety::reset_stack_canary();
        CHECK(StackSafety::check_stack_canary() == true);
    }
}
```

**Memory Impact Analysis:**
- **Before:** Limited test coverage
- **After:** Comprehensive test coverage
- **Savings:** 0 bytes (tests in separate build)

**Performance Impact Analysis:**
- **Before:** No impact (tests not in production)
- **After:** No impact (tests not in production)
- **Improvement:** Better code quality assurance

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (tests only)
- **Migration Effort:** High (requires significant test development)
- **Test Coverage:** New tests

---

### Fix #L3: Performance Profiling and Optimization

**Defect ID:** N/A  
**Severity:** LOW  
**Constraint Violation:** Diamond Code recommends performance optimization  
**Files Affected:**
- Performance-critical code paths

**Description:**
Performance profiling can identify bottlenecks and optimization opportunities. Diamond Code recommends continuous performance monitoring.

**Proposed Solution:**

#### 3.1 Performance Profiling Strategy

**Performance Profiling Tools:**
```cpp
// performance_profiler.hpp
#ifndef PERFORMANCE_PROFILER_HPP_
#define PERFORMANCE_PROFILER_HPP_

#include <cstdint>
#include <array>

namespace Performance {

// Profiling data
struct ProfilingData {
    const char* function_name;
    uint32_t call_count;
    uint32_t total_cycles;
    uint32_t min_cycles;
    uint32_t max_cycles;
};

// Profiler
class Profiler {
    static constexpr size_t MAX_FUNCTIONS = 64;
    static std::array<ProfilingData, MAX_FUNCTIONS> data_;
    static size_t data_count_;
    
public:
    // Start profiling
    static uint32_t start() {
        return chSysGetRealtimeCounterX();
    }
    
    // End profiling
    static void end(const char* function_name, uint32_t start_cycles) {
        uint32_t end_cycles = chSysGetRealtimeCounterX();
        uint32_t elapsed = end_cycles - start_cycles;
        
        // Find or create entry
        for (size_t i = 0; i < data_count_; ++i) {
            if (data_[i].function_name == function_name) {
                data_[i].call_count++;
                data_[i].total_cycles += elapsed;
                data_[i].min_cycles = std::min(data_[i].min_cycles, elapsed);
                data_[i].max_cycles = std::max(data_[i].max_cycles, elapsed);
                return;
            }
        }
        
        // Create new entry
        if (data_count_ < MAX_FUNCTIONS) {
            data_[data_count_] = {
                .function_name = function_name,
                .call_count = 1,
                .total_cycles = elapsed,
                .min_cycles = elapsed,
                .max_cycles = elapsed
            };
            data_count_++;
        }
    }
    
    // Get profiling data
    static const ProfilingData* get_data(size_t index) {
        if (index >= data_count_) return nullptr;
        return &data_[index];
    }
    
    static size_t get_data_count() {
        return data_count_;
    }
    
    // Reset profiling data
    static void reset() {
        data_count_ = 0;
    }
};

// RAII profiling wrapper
class ScopedProfile {
    const char* function_name_;
    uint32_t start_cycles_;
    
public:
    explicit ScopedProfile(const char* name) 
        : function_name_(name), start_cycles_(Profiler::start()) {}
    
    ~ScopedProfile() {
        Profiler::end(function_name_, start_cycles_);
    }
};

} // namespace Performance

#endif // PERFORMANCE_PROFILER_HPP_
```

**Usage Example:**
```cpp
void DroneScanner::perform_database_scan_cycle(DroneHardwareController& hardware) noexcept {
    // Profile this function
    #ifdef ENABLE_PROFILING
    Performance::ScopedProfile profile("perform_database_scan_cycle");
    #endif
    
    // ... function code ...
}
```

**Performance Optimization Targets:**
1. Reduce paint() stack usage below 1.5KB
2. Optimize spectrum rendering for < 10ms per frame
3. Reduce scan cycle latency to < 50ms for high-threat scenarios
4. Minimize mutex lock contention
5. Optimize string formatting operations

**Memory Impact Analysis:**
- **Before:** No performance profiling
- **After:** Performance profiling enabled
- **Savings:** 0 bytes (profiling in debug builds only)

**Performance Impact Analysis:**
- **Before:** Unknown performance characteristics
- **After:** Measured and optimized performance
- **Improvement:** Better performance

**Risk Assessment:**
- **Risk Level:** LOW
- **Breaking Changes:** None (profiling only)
- **Migration Effort:** Medium (requires profiling setup)
- **Test Coverage:** Performance tests

---

## Summary of Medium and Low Priority Improvements

| Fix ID | Description | Priority | Risk | Effort |
|--------|-------------|----------|------|--------|
| #M1 | Dead code (commented-out code) | 3 | LOW | Low |
| #M2 | Stack overflow risk in paint() | 3 | LOW | Medium |
| #M3 | Inconsistent naming convention | 3 | LOW | Medium |
| #M4 | Missing documentation | 3 | LOW | High |
| #L1 | Code style and formatting | 4 | LOW | Low |
| #L2 | Unit test coverage | 4 | LOW | High |
| #L3 | Performance profiling and optimization | 4 | LOW | Medium |

**Total Medium Priority Improvements:** 4  
**Total Low Priority Improvements:** 3  
**Estimated Implementation Effort:** Medium-High  
**Risk Mitigation:** All improvements enhance code quality

---

## Complete Fix Summary Across All Documents

### Priority 1 (Critical) - MUST FIX
| Fix ID | Description | Risk | Effort |
|--------|-------------|------|--------|
| #C1 | std::string return types | LOW | Low |
| #C2 | Unsafe ISR mutex usage | MEDIUM | Medium |
| #C3 | Lock ordering violation | MEDIUM | Medium |
| #C4 | Uninitialized stack canary | LOW | Low |
| #C5 | Stack allocation exceeds 4KB | MEDIUM | Medium |
| #C6 | Race condition on shared state | MEDIUM | High |

### Priority 2 (High) - SHOULD FIX
| Fix ID | Description | Risk | Effort |
|--------|-------------|------|--------|
| #H1 | Magic numbers throughout codebase | LOW | Medium |
| #H2 | Type ambiguity throughout codebase | LOW | Medium |
| #H3 | Logic error (always false condition) | LOW | Low |
| #H4 | Undefined constant reference | LOW | Low |
| #H5 | Spaghetti logic (325 lines initialization) | MEDIUM | High |

### Priority 3 (Medium) - NICE TO FIX
| Fix ID | Description | Risk | Effort |
|--------|-------------|------|--------|
| #M1 | Dead code (commented-out code) | LOW | Low |
| #M2 | Stack overflow risk in paint() | LOW | Medium |
| #M3 | Inconsistent naming convention | LOW | Medium |
| #M4 | Missing documentation | LOW | High |

### Priority 4 (Low) - NICE TO HAVE
| Fix ID | Description | Risk | Effort |
|--------|-------------|------|--------|
| #L1 | Code style and formatting | LOW | Low |
| #L2 | Unit test coverage | LOW | High |
| #L3 | Performance profiling and optimization | LOW | Medium |

---

## Implementation Roadmap

### Phase 1: Critical Fixes (Priority 1)
1. Fix #C1: std::string return types
2. Fix #C2: Unsafe ISR mutex usage
3. Fix #C3: Lock ordering violation
4. Fix #C4: Uninitialized stack canary
5. Fix #C5: Stack allocation exceeds 4KB
6. Fix #C6: Race condition on shared state

### Phase 2: High Priority Fixes (Priority 2)
1. Fix #H1: Magic numbers throughout codebase
2. Fix #H2: Type ambiguity throughout codebase
3. Fix #H3: Logic error (always false condition)
4. Fix #H4: Undefined constant reference
5. Fix #H5: Spaghetti logic (325 lines initialization)

### Phase 3: Medium Priority Improvements (Priority 3)
1. Fix #M1: Dead code (commented-out code)
2. Fix #M2: Stack overflow risk in paint()
3. Fix #M3: Inconsistent naming convention
4. Fix #M4: Missing documentation

### Phase 4: Low Priority Improvements (Priority 4)
1. Fix #L1: Code style and formatting
2. Fix #L2: Unit test coverage
3. Fix #L3: Performance profiling and optimization

---

## Next Steps

1. Review all three blueprint documents:
   - `stage2_critical_fixes.md` - Priority 1 critical fixes
   - `stage2_high_priority_fixes.md` - Priority 2 high priority fixes
   - `stage2_medium_low_priority_fixes.md` - This document

2. Approve the blueprint documents

3. Proceed to Stage 3 (Implementation) to apply the fixes

**Note:** This is Stage 2 of a 4-stage Diamond Code pipeline. No code changes should be made until Stage 3 (Implementation) begins.
