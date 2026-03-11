# EDA Initialization Logic Fix Report

**Date:** 2026-03-11  
**Target:** STM32F405 (ARM Cortex-M4, 128KB RAM)  
**Issue:** Wild calls (поблема диких визовов) in the stack - components accessed before proper initialization

---

## Executive Summary

This report identifies and documents critical initialization logic issues in the Enhanced Drone Analyzer (EDA) that can cause "wild calls" - components being accessed before they are properly initialized. These issues can lead to crashes, undefined behavior, and data corruption.

**Key Finding:** The codebase has a sophisticated phase-based initialization system with `PhaseCompletion` tracking, but several code paths bypass the initialization checks and access components prematurely.

---

## Issues Identified

### Issue 1: Wild Calls in Stack - Missing Guards

**Severity:** CRITICAL  
**Location:** Multiple locations in `ui_enhanced_drone_analyzer.cpp`

#### 1.1 Coordinator Access Without Null Check (Line 4985)

```cpp
// BEFORE (UNSAFE):
scanning_coordinator_->update_runtime_parameters(settings_);

// AFTER (SAFE):
if (scanning_coordinator_ == nullptr) {
    status_bar_.update_normal_status("ERROR", "Coordinator not ready");
    return;
}
scanning_coordinator_->update_runtime_parameters(settings_);
```

**Problem:** `scanning_coordinator_` is accessed without checking if it's initialized. This can happen if:
- Constructor initialization fails
- Singleton initialization is not complete
- The pointer is still nullptr

**Impact:** Null pointer dereference → Crash/Undefined behavior

---

#### 1.2 Scanner Update Without Initialization Check (Line 5280)

```cpp
// BEFORE (UNSAFE):
void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
    if (!display_controller_.are_buffers_valid()) {
        return;
    }
    // ... rest of function accesses scanner_ without checking init state
}

// AFTER (SAFE):
void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
    // CRITICAL FIX: Guard against wild calls - check initialization state
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        return;
    }
    
    if (!display_controller_.are_buffers_valid()) {
        return;
    }
    // ... rest of function
}
```

**Problem:** `handle_scanner_update()` is called from multiple code paths:
- `message_handler_frame_sync_` (line 2041)
- `init_phase_finalize()` (line 5034)

If called before `FULLY_INITIALIZED` state, scanner methods are invoked on an uninitialized scanner.

**Impact:** Access to uninitialized scanner data → Wild calls, crashes, undefined behavior

---

### Issue 2: Phase Race Conditions - Incomplete Verification

**Severity:** HIGH  
**Location:** `step_deferred_initialization()` (lines 4648-4730)

**Status:** PARTIALLY FIXED - Phase completion verification exists but incomplete

#### 2.1 Existing Phase Completion Checks (Lines 4721-4730)

```cpp
// EXISTING CODE (GOOD):
if (phase_idx >= 1 && !phase_completion_.buffers_allocated) {
    initialization_in_progress_ = false;
    return;
}
if (phase_idx >= 2 && !phase_completion_.database_loaded) {
    initialization_in_progress_ = false;
    return;
}
```

**Good:** Phases 1 and 2 have completion verification.

**Missing:** Phases 3, 4, 5 do not have verification checks.

#### 2.2 Missing Phase Verification for Phases 3-5

```cpp
// ADD AFTER LINE 4730:
// Phase 3 (init_hardware): Verify Phase 2 completed
if (phase_idx >= 3 && !phase_completion_.database_loaded) {
    initialization_in_progress_ = false;
    return;
}

// Phase 4 (setup_ui): Verify Phase 3 completed
if (phase_idx >= 4 && !phase_completion_.hardware_ready) {
    initialization_in_progress_ = false;
    return;
}

// Phase 5 (load_settings): Verify Phase 4 completed
if (phase_idx >= 5 && !phase_completion_.ui_layout_ready) {
    initialization_in_progress_ = false;
    return;
}
```

**Impact:** Without these checks, phases can execute even when previous phases failed, creating race conditions where:
- Hardware init runs before database loads
- UI setup runs before hardware is ready
- Settings load runs before UI is ready

---

### Issue 3: Singleton Access Before Initialization

**Severity:** HIGH  
**Location:** `scanning_coordinator.cpp`

#### 3.1 Double Initialization Risk (Line 78-95)

```cpp
// BEFORE (UNSAFE):
void initialize_eda_mutexes() noexcept {
    chMtxInit(&ScanningCoordinator::init_mutex_);
    // ...
}

// AFTER (SAFE):
void initialize_eda_mutexes() noexcept {
    // CRITICAL FIX: Add guard to prevent wild calls
    // Ensure mutexes are only initialized once to prevent double initialization
    // Double initialization can cause undefined behavior with ChibiOS mutexes
    static bool mutexes_initialized = false;
    if (mutexes_initialized) {
        return;  // Already initialized
    }
    
    chMtxInit(&ScanningCoordinator::init_mutex_);
    mutexes_initialized = true;
    // ...
}
```

**Problem:** If `initialize_eda_mutexes()` is called multiple times, it re-initializes the mutex, which can cause:
- Lost lock state
- Deadlock
- Undefined behavior with ChibiOS

**Impact:** Mutex corruption → Deadlock, crashes

---

#### 3.2 Function Signature Mismatch

**Problem:** The function declaration in `eda_locking.hpp` (line 31) doesn't have `noexcept`, but the implementation in `scanning_coordinator.cpp` (line 78) has `noexcept`.

**Fix Applied:** Added `noexcept` to declaration in `eda_locking.hpp` (line 31).

---

### Issue 4: Constructor Initialization Order

**Severity:** MEDIUM  
**Location:** `EnhancedDroneSpectrumAnalyzerView` constructor (lines 4262-4322)

#### 4.1 Current Initialization Order (CORRECT)

```cpp
EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      settings_(),
      hardware_(SpectrumMode::MEDIUM),      // 1. No dependencies
      scanner_(get_default_scanner_settings()), // 2. No dependencies
      audio_(),                              // 3. No dependencies
      ui_controller_(nav, hardware_, scanner_, audio_), // 4. Depends on 1-3
      display_controller_({0, 60, screen_width, screen_height - 80}), // 5. No dependencies
      // ...
```

**Analysis:** The initialization order is CORRECT:
1. `hardware_` - No dependencies ✓
2. `scanner_` - No dependencies ✓
3. `audio_` - No dependencies ✓
4. `ui_controller_` - Depends on 1-3 ✓
5. `display_controller_` - No dependencies ✓
6. `scanning_coordinator_` - Depends on all above ✓

**Status:** No fix needed for constructor order.

---

## Recommended Fixes

### Fix 1: Add `is_phase_complete()` Helper Method

**File:** `ui_enhanced_drone_analyzer.hpp`  
**Location:** PhaseCompletion struct (after line 2148)

**Status:** ALREADY IMPLEMENTED ✓

The `is_phase_complete()` method is already present (lines 2156-2173):

```cpp
[[nodiscard]] bool is_phase_complete(InitState phase) const noexcept {
    switch (phase) {
        case InitState::BUFFERS_ALLOCATED:
            return buffers_allocated;
        case InitState::DATABASE_LOADED:
            return database_loaded;
        case InitState::HARDWARE_READY:
            return hardware_ready;
        case InitState::UI_LAYOUT_READY:
            return ui_layout_ready;
        case InitState::SETTINGS_LOADED:
            return settings_loaded;
        case InitState::FULLY_INITIALIZED:
            return finalized;
        default:
            return false;
    }
}
```

This method should be used throughout the codebase to prevent wild calls.

---

### Fix 2: Add Guards in `init_phase_load_settings()`

**File:** `ui_enhanced_drone_analyzer.cpp`  
**Location:** Line 4984-4985

```cpp
// ADD BEFORE LINE 4985:
// FIX: Update coordinator parameters after settings load
// CRITICAL FIX: Add null check to prevent wild call before coordinator is initialized
if (scanning_coordinator_ == nullptr) {
    status_bar_.update_normal_status("ERROR", "Coordinator not ready");
    return;
}
scanning_coordinator_->update_runtime_parameters(settings_);
```

---

### Fix 3: Add Initialization Check in `handle_scanner_update()`

**File:** `ui_enhanced_drone_analyzer.cpp`  
**Location:** Line 5280-5284

```cpp
// ADD AFTER LINE 5280:
// CRITICAL FIX: Guard against wild calls - check initialization state
// Prevents access to scanner before initialization is complete
if (init_state_ != InitState::FULLY_INITIALIZED) {
    return;
}
```

---

### Fix 4: Complete Phase Verification in `step_deferred_initialization()`

**File:** `ui_enhanced_drone_analyzer.cpp`  
**Location:** After line 4730

```cpp
// ADD AFTER LINE 4730:
// Phase 3 (init_hardware): Verify Phase 2 completed
if (phase_idx >= 3 && !phase_completion_.database_loaded) {
    // Previous phase did not complete - skip this phase
    initialization_in_progress_ = false;
    return;
}

// Phase 4 (setup_ui): Verify Phase 3 completed
if (phase_idx >= 4 && !phase_completion_.hardware_ready) {
    // Previous phase did not complete - skip this phase
    initialization_in_progress_ = false;
    return;
}

// Phase 5 (load_settings): Verify Phase 4 completed
if (phase_idx >= 5 && !phase_completion_.ui_layout_ready) {
    // Previous phase did not complete - skip this phase
    initialization_in_progress_ = false;
    return;
}
```

---

### Fix 5: Add Double Initialization Guard in `initialize_eda_mutexes()`

**File:** `scanning_coordinator.cpp`  
**Location:** Line 78-86

```cpp
// REPLACE LINES 78-86:
void initialize_eda_mutexes() noexcept {
    // CRITICAL FIX: Add guard to prevent wild calls
    // Ensure mutexes are only initialized once to prevent double initialization
    // Double initialization can cause undefined behavior with ChibiOS mutexes
    static bool mutexes_initialized = false;
    if (mutexes_initialized) {
        return;  // Already initialized
    }
    
    // Initialize ScanningCoordinator mutex
    chMtxInit(&ScanningCoordinator::init_mutex_);
    mutexes_initialized = true;
    // ... rest of function
}
```

---

### Fix 6: Fix Function Signature Mismatch

**File:** `eda_locking.hpp`  
**Location:** Line 31

**Status:** ALREADY FIXED ✓

Changed from:
```cpp
void initialize_eda_mutexes();
```

To:
```cpp
void initialize_eda_mutexes() noexcept;
```

---

## Implementation Priority

### Critical (Must Fix Immediately)
1. **Fix 2** - Coordinator null check (prevents crash on line 4985)
2. **Fix 3** - Initialization check in `handle_scanner_update()` (prevents wild calls from multiple code paths)

### High (Should Fix Soon)
3. **Fix 4** - Complete phase verification (prevents phase race conditions)
4. **Fix 5** - Double initialization guard (prevents mutex corruption)

### Medium (Fix When Possible)
5. **Fix 1** - Already implemented ✓
6. **Fix 6** - Already fixed ✓

---

## Testing Recommendations

After applying these fixes, test the following scenarios:

1. **Cold Start Test:**
   - Start EDA from fresh state
   - Verify all phases complete in order
   - Verify no wild calls occur

2. **Interrupted Initialization Test:**
   - Start EDA, interrupt during database load
   - Resume and verify proper state recovery

3. **Error Path Test:**
   - Simulate database load failure
   - Verify error state is handled correctly
   - Verify no wild calls occur in error state

4. **Multi-Thread Test:**
   - Start scanning thread
   - Verify coordinator access is safe from UI thread
   - Verify no race conditions occur

---

## Summary

The EDA initialization system has a solid foundation with `PhaseCompletion` tracking, but several code paths bypass the initialization checks. The recommended fixes add defensive guards to prevent wild calls and ensure proper phase sequencing.

**Key Improvements:**
1. Null pointer checks for coordinator access
2. Initialization state verification before scanner access
3. Complete phase completion verification for all 6 phases
4. Double initialization guard for mutex initialization

**Estimated Impact:**
- **Safety:** Eliminates wild calls, prevents crashes
- **Reliability:** Ensures proper initialization sequencing
- **Debuggability:** Makes initialization issues easier to diagnose
- **Code Size:** Minimal increase (< 100 bytes total)

---

## Notes

- The codebase already has good defensive programming practices with `PhaseCompletion` tracking
- The `is_phase_complete()` helper method is already implemented
- Most phase functions properly set completion flags
- The main issues are missing guards at call sites and incomplete phase verification

---

**Report Generated:** 2026-03-11  
**Author:** EDA Initialization Fix Task
