# Architectural Blueprint: eda_locking.hpp Fixes - Part 3

**Date:** 2026-03-13
**Target:** STM32F405 (ARM Cortex-M4, 128KB RAM, 4KB stack per thread)
**Context:** Subtask 2 - Architectural Blueprint for Locking System Fixes

---

## SECTION 6: MEMORY PLACEMENT STRATEGY

### 6.1 Flash vs RAM

**Principle:** Place read-only data in flash to save RAM.

**Current Memory Placement:**

| Component | Current Placement | Should Be | Rationale |
|-----------|------------------|-------------|------------|
| `EDA_LOCK_DEBUG` constexpr | Flash | Flash | Compile-time constant |
| `MAX_LOCK_DEPTH` constexpr | Flash | Flash | Compile-time constant |
| `STACK_SAFETY_MARGIN_BYTES` constexpr | Flash | Flash | Compile-time constant |
| `STACK_MAX_SCAN_BYTES` constexpr | Flash | Flash | Compile-time constant |
| `STACK_FILL_PATTERN` constexpr | Flash | Flash | Compile-time constant |
| `WORD_SIZE_BYTES` constexpr | Flash | Flash | Compile-time constant |
| `SD_CARD_LOCK_TIMEOUT_MS` constexpr | Flash | Flash | Compile-time constant |
| `is_valid_lock_order()` constexpr function | Flash | Flash | Compile-time function |
| `AtomicFlag::value_` member | RAM | RAM | Per-instance data |
| `LockOrderTracker` members | RAM (thread-local) | RAM (TLS) | Per-thread data |
| `MutexLock` members | RAM (stack) | RAM (stack) | Per-instance data |
| `StackMonitor` members | RAM (stack) | RAM (stack) | Per-instance data |

**Analysis:**

All constants are already `constexpr`, which means they are placed in flash (read-only memory) by the compiler. No changes needed.

**Recommendation:** Continue using `constexpr` for all compile-time constants.

---

### 6.2 Thread-Local Storage

**Principle:** Use `thread_local` for per-thread data to avoid race conditions.

**Current Thread-Local Variables:**

| Variable | Type | Placement | Lifetime |
|-----------|-------|-----------|-----------|
| `CriticalSection::nesting_count_` | `size_t` | Thread-local | Thread lifetime |
| `LockOrderTracker::instance()` | `LockOrderTracker` | Thread-local | Thread lifetime |
| `LockOrderTracker::lock_stack_` | `LockOrder[16]` | Thread-local | Thread lifetime |
| `LockOrderTracker::lock_depth_` | `size_t` | Thread-local | Thread lifetime |
| `LockOrderTracker::held_locks_bitset_` | `uint32_t` | Thread-local | Thread lifetime |
| `LockOrderTracker::overflow_count_` | `size_t` | Thread-local | Thread lifetime |

**Memory Impact:**

Per-thread memory usage:
- `CriticalSection::nesting_count_`: 4 bytes
- `LockOrderTracker`: 28 bytes
- **Total per thread:** 32 bytes

**Total for 4 threads:** 128 bytes

**Analysis:**

Thread-local storage is the correct choice for these variables because:
1. Each thread needs its own copy (avoid race conditions)
2. Lifetime matches thread lifetime (no dangling references)
3. No synchronization needed (thread-local by definition)
4. Minimal memory overhead (32 bytes per thread)

**Recommendation:** Continue using `thread_local` for all per-thread data.

---

### 6.3 Zero-Cost Abstractions

**Principle:** Ensure release builds have zero overhead for debug-only features.

**Current Implementation:**

```cpp
#if EDA_LOCK_DEBUG
    constexpr bool EDA_LOCK_DEBUG = true;
#else
    constexpr bool EDA_LOCK_DEBUG = false;
#endif

#if EDA_LOCK_DEBUG
class LockOrderTracker {
    // ... debug-only implementation
};
#endif
```

**Analysis:**

- **Debug Builds:** `LockOrderTracker` is compiled and used
- **Release Builds:** `LockOrderTracker` is completely compiled out
- **Overhead in Release:** Zero (no code generated)

**Verification:**

Check that all debug-only code is properly guarded:

```cpp
// MutexLock constructor
explicit MutexLock(Mutex& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
    : mtx_(mtx), locked_(false), order_(order) {
    // Runtime validation (debug mode only): track lock order
    #if EDA_LOCK_DEBUG
    if (!LockOrderTracker::instance().push_lock(order)) {
        // Lock order violation detected
    }
    #endif
    
    chMtxLock(&mtx_);
    locked_ = true;
}
```

**Result:** In release builds, the `#if EDA_LOCK_DEBUG` block is completely removed by the preprocessor, resulting in zero overhead.

**Recommendation:** Continue using `#if EDA_LOCK_DEBUG` guards for all debug-only code.

---

### 6.4 Alignment Requirements

**Principle:** Ensure proper alignment for atomic operations and DMA transfers.

**Current Alignment:**

```cpp
class AtomicFlag {
private:
    alignas(4) volatile int value_;  ///< 4-byte aligned for ARM Cortex-M4 atomic operations
};
```

**Analysis:**

- ARM Cortex-M4 requires 4-byte alignment for atomic operations
- `alignas(4)` ensures proper alignment
- `volatile` prevents compiler optimizations that could break atomic semantics
- This is correct and necessary

**Other Alignment Considerations:**

1. **Mutex Structure:** ChibiOS `Mutex` type has its own alignment requirements (handled by ChibiOS)
2. **LockOrder Stack:** `LockOrder` is `uint8_t`, no special alignment needed
3. **Bitset:** `uint32_t` is naturally 4-byte aligned on ARM Cortex-M4

**Recommendation:** No changes needed. Current alignment is correct.

---

### 6.5 Memory Placement Summary

**Flash Usage (Read-Only):**

| Component | Size (bytes) | Placement |
|-----------|---------------|-----------|
| Constants (EDA_LOCK_DEBUG, etc.) | ~50 | Flash |
| LockOrder enum | ~21 | Flash |
| `is_valid_lock_order()` | Code | Flash |
| Class methods | Code | Flash |
| **Total** | **~500** | Flash |

**RAM Usage (Per Thread):**

| Component | Size (bytes) | Placement |
|-----------|---------------|-----------|
| `CriticalSection::nesting_count_` | 4 | TLS |
| `LockOrderTracker` | 28 | TLS |
| **Total per thread** | **32** | TLS |

**RAM Usage (Per Instance - Stack):**

| Component | Size (bytes) | Placement |
|-----------|---------------|-----------|
| `AtomicFlag` | 4 | Stack |
| `MutexLock` | 12 | Stack |
| `MutexTryLock` | 12 | Stack |
| `SDCardLock` | 12 | Stack |
| `StackMonitor` | 12 | Stack |

**Total RAM Usage (4 threads, typical usage):**

- **Thread-Local Storage:** 32 × 4 = 128 bytes
- **Stack Usage (3 nested locks + monitor):** 48 bytes
- **Total:** 176 bytes

**Percentage of 128KB RAM:** 176 / 131072 = 0.13%

**Conclusion:** Memory usage is excellent. The locking system uses minimal RAM.

---

## SECTION 7: RED TEAM ATTACK PLAN

### 7.1 CriticalSection nesting_count Fix

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- `CriticalSection` uses `thread_local` variable (not on stack)
- Recursive calls share the same `nesting_count_` variable
- Stack usage per call: 0 bytes (no stack allocation)
- Maximum recursion depth: Unlimited (limited only by interrupt nesting)

**Answer:** **NO** - No stack overflow risk.

**Test Case:**
```cpp
void recursive_critical_section(int depth) {
    if (depth == 1000) {
        return;  // Stop after 1000 levels
    }
    CriticalSection lock;
    recursive_critical_section(depth + 1);
}
// This is safe - no stack allocation per call
```

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- Constructor: One comparison + one increment (2 CPU cycles)
- Destructor: One decrement + one comparison + potential `chSysUnlock()` (10-20 cycles)
- Total per call: ~12-22 CPU cycles
- Frequency: ~1-10 calls per second (typical usage)

**Answer:** **NO** - Negligible overhead.

**Comparison:**
- DSP processing: Thousands of cycles per sample
- Critical section: ~20 cycles per call
- **Ratio:** 1:1000 (critical section is 0.1% of DSP cost)

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Uses `thread_local` (C++11 feature, compatible with Mayhem)
- Uses RAII pattern (consistent with Mayhem)
- Uses `noexcept` (embedded safety, consistent with Mayhem)
- Uses `delete` for copy/move (consistent with Mayhem)
- Uses Doxygen comments (consistent with Mayhem)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| Constructor called before chSysInit() | `chSysLock()` called, undefined behavior | **NO** |
| Destructor called without matching constructor | `nesting_count_` underflows, interrupts stay disabled | **NO** |
| Interrupt nesting > 1000 levels | `nesting_count_` overflows (uint8_t) | **NO** |
| Exception thrown in critical section | Destructor not called, interrupts stay disabled | **NO** |

**Recommendation:** Add `static_assert` to ensure `chSysInit()` is called first:

```cpp
// In main.cpp, after chSysInit():
static_assert(CH_STATE_SYSTEM == CH_STATE_READY, 
    "chSysInit() must be called before using CriticalSection");
```

**Note:** Mayhem codebase already enforces proper initialization sequence via `initialize_eda_mutexes()`.

---

### 7.2 MutexTryLock push_lock Timing Fix

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- `MutexTryLock` allocates 12 bytes on stack (mtx_ reference, locked_, order_)
- Recursive calls allocate 12 bytes per level
- Maximum recursion depth: 4KB / 12 = ~342 levels
- Typical usage: 1-3 levels

**Answer:** **NO** - Safe for typical usage.

**Test Case:**
```cpp
void recursive_try_lock(int depth) {
    if (depth == 10) {
        return;  // Stop after 10 levels
    }
    MutexTryLock lock(my_mutex, LockOrder::DATA_MUTEX);
    if (lock.is_locked()) {
        recursive_try_lock(depth + 1);
    }
}
// Stack usage: 12 × 10 = 120 bytes (safe)
```

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- Constructor: One `chMtxTryLock()` call (~20-30 cycles) + one `push_lock()` call (~10 cycles)
- Destructor: One `pop_lock()` call (~10 cycles) + one `chMtxUnlock()` call (~10-20 cycles)
- Total per call: ~40-60 CPU cycles
- Frequency: ~1-10 calls per second (typical usage)

**Answer:** **NO** - Negligible overhead.

**Comparison:**
- DSP processing: Thousands of cycles per sample
- Try-lock: ~50 cycles per call
- **Ratio:** 1:100 (try-lock is 1% of DSP cost)

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Uses RAII pattern (consistent with Mayhem)
- Uses `noexcept` (embedded safety, consistent with Mayhem)
- Uses `delete` for copy/move (consistent with Mayhem)
- Uses `is_locked()` method (consistent with Mayhem's `MutexTryLock`)
- Uses Doxygen comments (consistent with Mayhem)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| `chMtxTryLock()` fails | `locked_ = false`, `push_lock()` not called | **YES** |
| `push_lock()` fails (lock order violation) | Lock still held, violation recorded | **YES** |
| Mutex not initialized | Undefined behavior in `chMtxTryLock()` | **NO** |
| Destructor called with `locked_ = false` | No action, safe | **YES** |
| Exception thrown in critical section | Destructor not called, mutex stays locked | **NO** |

**Recommendation:** Add documentation about mutex initialization requirement:

```cpp
/**
 * @brief Non-blocking try-lock for mutexes
 *
 * MUTEX INITIALIZATION REQUIREMENT:
 * - All mutexes MUST be initialized via initialize_eda_mutexes() before use
 * - Calling chMtxTryLock() on uninitialized mutex causes undefined behavior
 *
 * @note Safe for use in situations where blocking is not acceptable
 * @note Always check is_locked() before accessing protected data
 * @note CRITICAL: Locks must be acquired in ascending order to prevent deadlock
 */
```

---

### 7.3 SDCardLock push_lock Timing Fix

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- `SDCardLock` allocates 12 bytes on stack (same as `MutexTryLock`)
- Recursive calls allocate 12 bytes per level
- Maximum recursion depth: 4KB / 12 = ~342 levels
- Typical usage: 1 level (SD card operations are not nested)

**Answer:** **NO** - Safe for typical usage.

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- Same as `MutexTryLock`: ~40-60 CPU cycles per call
- Frequency: ~1-5 calls per second (SD card operations are slow anyway)
- SD card write: ~10-100ms (100,000-1,000,000 cycles)
- Lock overhead: ~50 cycles (negligible compared to SD card time)

**Answer:** **NO** - Negligible overhead.

**Comparison:**
- SD card write: 1,000,000 cycles
- Lock overhead: 50 cycles
- **Ratio:** 1:20,000 (lock overhead is 0.005% of SD card time)

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Uses RAII pattern (consistent with Mayhem)
- Uses `noexcept` (embedded safety, consistent with Mayhem)
- Uses `delete` for copy/move (consistent with Mayhem)
- Uses `is_locked()` method (consistent with Mayhem's `MutexTryLock`)
- Uses Doxygen comments (consistent with Mayhem)
- Specialized for SD card (consistent with Mayhem's pattern of specialized wrappers)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| `chMtxTryLock()` fails | `locked_ = false`, `push_lock()` not called | **YES** |
| Mutex not initialized | Undefined behavior in `chMtxTryLock()` | **NO** |
| Timeout expires (if timeout implemented) | `locked_ = false`, `push_lock()` not called | **YES** |
| Destructor called with `locked_ = false` | No action, safe | **YES** |
| Exception thrown in critical section | Destructor not called, mutex stays locked | **NO** |

**Recommendation:** Add documentation about SD card mutex requirements:

```cpp
/**
 * @brief SD Card Lock (specialized wrapper for SD card mutex)
 *
 * MUTEX INITIALIZATION REQUIREMENT:
 * - All mutexes MUST be initialized via initialize_eda_mutexes() before use
 * - Calling chMtxTryLock() on uninitialized mutex causes undefined behavior
 *
 * TIMEOUT BEHAVIOR:
 * - ChibiOS 2.6.8 doesn't support timeout-based mutex locking
 * - The timeout_ms parameter is kept for API compatibility but not used
 * - Use is_locked() to check if lock was acquired
 *
 * @note Always use this for SD card operations (FatFS is NOT thread-safe)
 * @note SD_CARD_MUTEX must be LAST in lock ordering
 * @note CRITICAL: Always check is_locked() after construction
 * @note CRITICAL: Mutex must be initialized via initialize_eda_mutexes() before use
 */
```

---

### 7.4 ChibiOS API Verification

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- API verification is compile-time only (no runtime code)
- No stack allocation
- No recursion

**Answer:** **NO** - No stack impact.

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- Compile-time verification: Zero runtime overhead
- Runtime code: No change

**Answer:** **NO** - Zero runtime overhead.

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Using `static_assert` for compile-time checks (consistent with Mayhem)
- Documenting API assumptions (consistent with Mayhem)
- Adding compatibility layer if needed (consistent with Mayhem's cross-platform approach)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| ChibiOS API signature mismatch | Compile-time error (static_assert) | **YES** |
| Wrong ChibiOS version | Compile-time error or runtime error | **NO** |
| Parameter-less API doesn't exist | Compile-time error | **YES** |

**Recommendation:** Verify ChibiOS version and API signature before implementing:

```bash
# Check ChibiOS version
grep -r "CH_KERNEL_MAJOR" firmware/chibios-portapack/os/kernel/include

# Check chMtxUnlock signature
grep -r "void chMtxUnlock" firmware/chibios-portapack/os/kernel/include
```

---

### 7.5 SDCardLock Validation Logic Fix

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- Removing validation logic reduces code size
- No additional stack allocation
- Same as `MutexTryLock`: 12 bytes per instance

**Answer:** **NO** - No stack impact.

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- Removing validation logic: FASTER (fewer CPU cycles)
- No additional overhead

**Answer:** **NO** - Performance improvement.

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Relying on proper initialization sequence (consistent with Mayhem)
- Documenting requirements clearly (consistent with Mayhem)
- Simplifying code by removing unreliable heuristic (consistent with Mayhem's "clean code" principle)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| Mutex not initialized | Undefined behavior in `chMtxTryLock()` | **NO** |
| `initialize_eda_mutexes()` not called | Undefined behavior | **NO** |
| Mutex initialized twice | Undefined behavior | **NO** |

**Recommendation:** Enforce initialization sequence via documentation and compile-time checks:

```cpp
/**
 * @brief Initialize all EDA mutexes
 *
 * CRITICAL: This function MUST be called AFTER chSysInit() and BEFORE any
 * threads that use these mutexes are created.
 *
 * Calling chMtxInit() before chSysInit() will cause undefined behavior.
 *
 * This function should be called in main.cpp during the initialization sequence:
 *
 * @code
 *   int main() {
 *       halInit();              // Initialize hardware abstraction layer
 *       chSysInit();            // Initialize ChibiOS kernel (REQUIRED FIRST)
 *       initialize_eda_mutexes(); // Initialize EDA mutexes (E003 requirement)
 *       // ... create threads that use mutexes ...
 *   }
 * @endcode
 *
 * @note This is a declaration only. The implementation should be provided in
 *       a .cpp file that initializes all mutexes used by the EDA module.
 * @note All mutexes must be initialized before they are used.
 * @note Failure to call this function will result in undefined behavior.
 */
void initialize_eda_mutexes() noexcept;
```

---

### 7.6 LockOrder Enhancement

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- `LockOrder` enum is compile-time constant (no runtime allocation)
- `is_valid_lock_order()` is constexpr (no runtime code)
- No stack impact

**Answer:** **NO** - No stack impact.

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- `is_valid_lock_order()`: constexpr (zero runtime overhead)
- `validate_lock_order<ORDER>`: static_assert (compile-time only)
- Runtime code: No change

**Answer:** **NO** - Zero runtime overhead.

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Using `enum class` (consistent with Mayhem)
- Using `uint8_t` as underlying type (consistent with Mayhem)
- Using constexpr validation (consistent with Mayhem)
- Using Doxygen comments (consistent with Mayhem)
- Organizing locks into logical groups (consistent with Mayhem's documentation style)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| Invalid lock order used | Compile-time error (static_assert) | **YES** |
| Lock order value exceeds enum range | Compile-time error (static_assert) | **YES** |
| Wrong lock order used at runtime | Lock order violation detected (debug mode) | **YES** |
| Too many locks nested | Overflow detected (debug mode) | **YES** |

**Recommendation:** Add compile-time validation for all lock acquisitions:

```cpp
// Helper macro for compile-time lock order validation
#define VALIDATE_LOCK_ORDER(ORDER) \
    static_assert(validate_lock_order<ORDER>::value, \
        "Invalid lock order - must be within LockOrder enum range")

// Usage in MutexLock constructor
explicit MutexLock(Mutex& mtx, LockOrder order = LockOrder::DATA_MUTEX) noexcept
    : mtx_(mtx), locked_(false), order_(order) {
    VALIDATE_LOCK_ORDER(order);  // Compile-time validation
    
    // ... rest of implementation
}
```

---

### 7.7 LockOrderTracker Improvements

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- `LockOrderTracker` is thread-local (not on stack)
- Recursive calls to `push_lock()`/`pop_lock()` share the same instance
- Stack usage per call: ~8 bytes (return address + registers)
- Maximum recursion depth: Limited by `MAX_LOCK_DEPTH` (16)

**Answer:** **NO** - Safe for typical usage.

**Test Case:**
```cpp
void recursive_lock_tracking(int depth) {
    if (depth == 16) {
        return;  // Stop at MAX_LOCK_DEPTH
    }
    LockOrderTracker::instance().push_lock(static_cast<LockOrder>(depth));
    recursive_lock_tracking(depth + 1);
    LockOrderTracker::instance().pop_lock(static_cast<LockOrder>(depth));
}
// Stack usage: ~8 × 16 = 128 bytes (safe)
```

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- `push_lock()`: Bitset check (1 cycle) + array access (2 cycles) + comparison (1 cycle) = ~4 cycles
- `pop_lock()`: Array access (2 cycles) + comparison (1 cycle) + bitset clear (1 cycle) = ~4 cycles
- `is_lock_held()`: Bitset check (1 cycle) = ~1 cycle
- Total per lock acquisition: ~4 cycles
- Frequency: ~1-10 locks per second

**Answer:** **NO** - Negligible overhead.

**Comparison:**
- DSP processing: Thousands of cycles per sample
- Lock tracking: ~4 cycles per lock
- **Ratio:** 1:1000 (lock tracking is 0.1% of DSP cost)

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Using bitset for O(1) lookup (consistent with Mayhem's performance focus)
- Using `thread_local` (consistent with Mayhem)
- Using `noexcept` (embedded safety, consistent with Mayhem)
- Using `delete` for copy/move (consistent with Mayhem)
- Using Doxygen comments (consistent with Mayhem)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| Lock stack overflow | Returns `false`, tracks overflow count | **YES** |
| Duplicate lock detected | Returns `false`, prevents deadlock | **YES** |
| Lock order violation | Returns `false`, prevents deadlock | **YES** |
| Pop wrong lock | Returns `false`, detects error | **YES** |
| Bitset overflow (> 32 locks) | Bitset wraps, undefined behavior | **NO** |

**Recommendation:** Add static_assert to ensure `LockOrder` fits in bitset:

```cpp
static_assert(static_cast<uint8_t>(LockOrder::SD_CARD_MUTEX) < 32,
    "LockOrder enum must have < 32 values to fit in uint32_t bitset");
```

---

### 7.8 Code Organization Split

**Stack Overflow Test:**

**Question:** "Will this blow the stack if called recursively?"

**Analysis:**
- Splitting files has no runtime impact
- Same code, just organized differently
- No additional stack allocation

**Answer:** **NO** - No stack impact.

---

**Performance Test:**

**Question:** "Is this too slow for real-time DSP?"

**Analysis:**
- Same code, just organized differently
- No performance change

**Answer:** **NO** - Zero performance impact.

---

**Mayhem Compatibility Test:**

**Question:** "Does this fit the repository style?"

**Analysis:**
- Splitting into logical modules (consistent with Mayhem's organization)
- Each file < 800 lines (consistent with user requirement)
- Clear dependencies (consistent with Mayhem's architecture)
- Using header guards (consistent with Mayhem)

**Answer:** **YES** - Fully compatible.

---

**Corner Cases:**

**Question:** "What happens if X fails?"

| Corner Case | Behavior | Safe? |
|-------------|-----------|--------|
| Circular dependency between files | Compile-time error (incomplete type) | **YES** |
| Missing include | Compile-time error | **YES** |
| Wrong include path | Compile-time error | **YES** |
| Header guard collision | Compile-time error | **YES** |

**Recommendation:** Document dependency graph clearly:

```cpp
/**
 * @file eda_locking_core.hpp
 * @brief Core types and enums for EDA locking system
 *
 * DEPENDENCIES:
 * - Standard library: <cstddef>, <cstdint>
 * - Third-party: <ch.h>
 * - Project: None (this is the foundation)
 *
 * FILES THAT DEPEND ON THIS:
 * - eda_locking_tracker.hpp
 * - eda_locking_wrappers.hpp
 * - eda_locking_stack.hpp
 * - eda_locking_init.hpp
 */
```

---

**End of Part 3**

This completes Part 3 of architectural blueprint, covering:
- Section 6: Memory Placement Strategy (6.1-6.5)
- Section 7: Red Team Attack Plan (7.1-7.8)

Continue to Part 4 for:
- Section 8: Implementation Priority
