# PART 3: RED TEAM ATTACK RESULTS
# Stack Optimization for Enhanced Drone Analyzer
# Stage 3 of 4-Stage Pipeline

**Date:** 2026-03-12
**Target:** STM32F405 (ARM Cortex-M4, 128KB RAM, 4KB stack per thread)
**Blueprint:** plans/stage2_architect_blueprint.md
**Expected Stack Reduction:** 4,366 bytes → 1,428 bytes (35% utilization)

---

## EXECUTIVE SUMMARY

This document presents the results of a systematic Red Team Attack on the Stage 2 Architect's Blueprint. The verification process examined each proposed fix from 7 different angles to identify potential flaws, risks, and implementation issues.

**Critical Finding:** Fix #2 (spectrum_data_ lock-free double-buffering) uses `std::atomic<uint8_t>` which is **FORBIDDEN** per the embedded constraints. This is a CRITICAL issue that must be resolved before implementation.

**Total Issues Found:** 12
- Critical Issues: 2
- High Priority Issues: 4
- Medium Priority Issues: 4
- Low Priority Issues: 2

---

## ATTACK #1: STACK OVERFLOW TEST

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Analysis:**
- Moving 640-byte structure from stack to static storage (BSS segment)
- RAII wrapper (`FilteredDronesSnapshotGuard`) allocates only a pointer (~8 bytes) on stack
- Maximum call depth for functions using this structure: ~5-6 frames
- Hidden stack allocations: Constructor/destructor of RAII wrapper are trivial (no heap allocation)
- Sequential allocation risk: Multiple guards cannot be nested due to mutex locking

**Risk Level:** LOW

**Issues Found:**
- None

**Recommendations:**
- Add static_assert to verify RAII wrapper size < 32 bytes

---

### Fix #2: spectrum_data_ (256 bytes) - Lock-free double-buffering

**Analysis:**
- Moving 256-byte array from stack to static storage (BSS segment)
- Double-buffering uses 512 bytes total (2×256)
- RAII wrapper not used (lock-free design)
- Maximum call depth: ~3-4 frames for spectrum processing
- Hidden stack allocations: None in lock-free design

**Risk Level:** CRITICAL (due to std::atomic usage)

**Issues Found:**
- **CRITICAL:** Uses `std::atomic<uint8_t>` which is FORBIDDEN per embedded constraints
- **CRITICAL:** std::atomic causes bloat and is not needed for this use case

**Recommendations:**
- **MUST FIX:** Replace std::atomic with ChibiOS mutex-protected double-buffering
- Use RAII guard pattern consistent with other fixes
- See "Revised Blueprint" section for corrected implementation

---

### Fix #3: power_levels (240 bytes)

**Analysis:**
- Moving 240-byte array from stack to static storage (BSS segment)
- RAII wrapper (`PowerLevelsGuard`) allocates only a pointer (~8 bytes) on stack
- Maximum call depth: ~4-5 frames for spectrum processing
- Hidden stack allocations: Constructor/destructor are trivial (no heap allocation)
- Sequential allocation risk: Can be nested with other guards (may cause deadlock)

**Risk Level:** LOW

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add static_assert to verify RAII wrapper size < 32 bytes

---

### Fix #4: std::string temp_string_ (heap allocation)

**Analysis:**
- Replacing std::string with fixed-size char array in FixedStringBuffer
- FixedStringBuffer stores pointer to external buffer (no stack allocation)
- Maximum call depth: ~3-4 frames for UI rendering
- Hidden stack allocations: None (FixedStringBuffer is non-owning)
- Sequential allocation risk: None (UI-only access)

**Risk Level:** MEDIUM

**Issues Found:**
- **HIGH:** Requires TextEdit widget to support direct char* buffer access
- **HIGH:** If TextEdit widget cannot be modified, this fix may not be feasible
- **MEDIUM:** FixedStringBuffer::data() returns non-const pointer, allowing external modification without update_length() call

**Recommendations:**
- Verify TextEdit widget API before implementation
- Consider creating custom TextEdit variant if needed
- Add documentation warning about calling update_length() after external modification

---

### Fix #5: std::string filename (heap allocation)

**Analysis:**
- Replacing std::string with fixed-size char array on stack
- Char array size: MAX_FILENAME_LENGTH (from EDA::Constants::MAX_NAME_LENGTH)
- Maximum call depth: ~3-4 frames for file operations
- Hidden stack allocations: std::filesystem::path::string() still allocates on heap
- Sequential allocation risk: None (UI-only access)

**Risk Level:** LOW

**Issues Found:**
- **MEDIUM:** Alternative solution still uses std::string temporarily (line 417 in blueprint)
- **LOW:** std::filesystem::path may allocate internally (unavoidable with current API)

**Recommendations:**
- Accept temporary std::string allocation (unavoidable with std::filesystem::path)
- Document that heap allocation is unavoidable for filesystem operations
- Consider implementing custom path parsing to eliminate std::filesystem dependency

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Analysis:**
- Moving 1000-byte array from stack to static storage (BSS segment)
- RAII wrapper (`EntriesToScanGuard`) allocates only a pointer (~8 bytes) on stack
- Maximum call depth: ~5-6 frames for scanning operations
- Hidden stack allocations: Constructor/destructor are trivial (no heap allocation)
- Sequential allocation risk: Can be nested with other guards (may cause deadlock)

**Risk Level:** LOW

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add static_assert to verify RAII wrapper size < 32 bytes

---

### Fix #7: histogram_buffer_ (67 bytes)

**Analysis:**
- Moving 67-byte structure from stack to static storage (BSS segment)
- RAII wrapper (`HistogramBufferGuard`) allocates only a pointer (~8 bytes) on stack
- Maximum call depth: ~4-5 frames for spectrum analysis
- Hidden stack allocations: Constructor/destructor are trivial (no heap allocation)
- Sequential allocation risk: Can be nested with other guards (may cause deadlock)

**Risk Level:** LOW

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add static_assert to verify RAII wrapper size < 32 bytes

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Analysis:**
- Changing from value copy to reference passing
- Reference size: 8 bytes (pointer)
- Maximum call depth: ~5-6 frames for settings operations
- Hidden stack allocations: None (reference is just a pointer)
- Sequential allocation risk: None (settings are read-only in most cases)

**Risk Level:** MEDIUM

**Issues Found:**
- **CRITICAL:** Lifetime management risk - settings reference may outlive parent object
- **HIGH:** Blueprint shows `const DroneAnalyzerSettings& settings_` as member variable (line 1124)
- **HIGH:** If parent object is destroyed while scanner is still using settings reference, use-after-free occurs
- **MEDIUM:** Blueprint shows both constructor and update_settings taking const reference, but doesn't show how settings_ is initialized

**Recommendations:**
- **MUST FIX:** Revert to storing settings by value (copy) - the 200 bytes is acceptable trade-off for safety
- Alternative: Use static storage for settings with mutex protection (similar to other fixes)
- Document lifetime requirements clearly if reference approach is kept

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Analysis:**
- Replacing std::filesystem::path with fixed-size char buffers
- Char array size: MAX_PATH_LENGTH (64 bytes)
- Maximum call depth: ~3-4 frames for file operations
- Hidden stack allocations: None in PathUtils functions
- Sequential allocation risk: None (UI-only access)

**Risk Level:** LOW

**Issues Found:**
- **MEDIUM:** PathUtils::extract_filename_stem() uses size_t for loop variables (OK)
- **LOW:** PathUtils functions don't validate that output buffer is null-terminated after copy

**Recommendations:**
- Ensure output buffer is always null-terminated (current code does this correctly)
- Add unit tests for edge cases (empty path, path with no extension, path with multiple dots)

---

### Fix #10: UI buffers totaling 160 bytes

**Analysis:**
- Moving 160-byte struct from stack to static storage (BSS segment)
- RAII wrapper (`UIBuffersGuard`) allocates only a pointer (~8 bytes) on stack
- Maximum call depth: ~4-5 frames for UI rendering
- Hidden stack allocations: Constructor/destructor are trivial (no heap allocation)
- Sequential allocation risk: Can be nested with other guards (may cause deadlock)

**Risk Level:** LOW

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add static_assert to verify RAII wrapper size < 32 bytes

---

## ATTACK #2: PERFORMANCE TEST

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Analysis:**
- Mutex locking overhead: ~10-20 cycles per lock/unlock
- Function call frequency: ~1-10 times per second (drone filtering)
- Cache locality: Moving to BSS may reduce cache locality (data not in stack)
- Double-buffering: Not used (single buffer with mutex)

**Performance Impact:** NEUTRAL (slight overhead from mutex, acceptable)

**Issues Found:**
- **LOW:** Moving to BSS may reduce cache locality slightly

**Recommendations:**
- Profile mutex contention to ensure acceptable performance
- Consider lock-free approach if contention is high (but use ChibiOS atomic, not std::atomic)

---

### Fix #2: spectrum_data_ (256 bytes) - Lock-free double-buffering

**Analysis:**
- Lock-free overhead: ~5-10 cycles per atomic load/store
- Function call frequency: ~30-60 times per second (spectrum display)
- Cache locality: Good (double-buffering allows writer to modify inactive buffer)
- Double-buffering: Used (512 bytes total, 256 bytes per buffer)

**Performance Impact:** POSITIVE (lock-free is efficient)

**Issues Found:**
- **CRITICAL:** Uses std::atomic which is FORBIDDEN

**Recommendations:**
- Replace with mutex-protected double-buffering (slightly slower but compliant)
- See "Revised Blueprint" section for corrected implementation

---

### Fix #3: power_levels (240 bytes)

**Analysis:**
- Mutex locking overhead: ~10-20 cycles per lock/unlock
- Function call frequency: ~30-60 times per second (spectrum processing)
- Cache locality: Moving to BSS may reduce cache locality slightly
- Double-buffering: Not used (single buffer with mutex)

**Performance Impact:** NEUTRAL (slight overhead from mutex, acceptable)

**Issues Found:**
- **LOW:** Moving to BSS may reduce cache locality slightly

**Recommendations:**
- Profile mutex contention to ensure acceptable performance
- Consider lock-free approach if contention is high (but use ChibiOS atomic, not std::atomic)

---

### Fix #4: std::string temp_string_ (heap allocation)

**Analysis:**
- Heap allocation overhead: Eliminated (good)
- Fixed-size buffer overhead: None (non-owning wrapper)
- Function call frequency: ~10-30 times per second (UI rendering)
- Cache locality: Improved (no heap allocation, no pointer chasing)

**Performance Impact:** POSITIVE (eliminates heap allocation overhead)

**Issues Found:**
- **HIGH:** Requires TextEdit widget modification (may not be feasible)

**Recommendations:**
- Verify TextEdit widget API before implementation
- Consider creating custom TextEdit variant if needed

---

### Fix #5: std::string filename (heap allocation)

**Analysis:**
- Heap allocation overhead: Reduced (but not eliminated due to std::filesystem::path)
- Fixed-size buffer overhead: None (stack allocation)
- Function call frequency: ~1-5 times per user interaction (file selection)
- Cache locality: Improved (no heap allocation for filename itself)

**Performance Impact:** POSITIVE (reduces heap allocation overhead)

**Issues Found:**
- **LOW:** std::filesystem::path still allocates internally (unavoidable)

**Recommendations:**
- Accept temporary heap allocation (unavoidable with std::filesystem::path)
- Document that heap allocation is unavoidable for filesystem operations

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Analysis:**
- Mutex locking overhead: ~10-20 cycles per lock/unlock
- Function call frequency: ~1-10 times per second (scanning)
- Cache locality: Moving to BSS may reduce cache locality slightly
- Double-buffering: Not used (single buffer with mutex)

**Performance Impact:** NEUTRAL (slight overhead from mutex, acceptable)

**Issues Found:**
- **LOW:** Moving to BSS may reduce cache locality slightly

**Recommendations:**
- Profile mutex contention to ensure acceptable performance
- Consider lock-free approach if contention is high (but use ChibiOS atomic, not std::atomic)

---

### Fix #7: histogram_buffer_ (67 bytes)

**Analysis:**
- Mutex locking overhead: ~10-20 cycles per lock/unlock
- Function call frequency: ~30-60 times per second (spectrum analysis)
- Cache locality: Moving to BSS may reduce cache locality slightly
- Double-buffering: Not used (single buffer with mutex)

**Performance Impact:** NEUTRAL (slight overhead from mutex, acceptable)

**Issues Found:**
- **LOW:** Moving to BSS may reduce cache locality slightly

**Recommendations:**
- Profile mutex contention to ensure acceptable performance
- Consider lock-free approach if contention is high (but use ChibiOS atomic, not std::atomic)

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Analysis:**
- Copy overhead: Eliminated (reference passing)
- Mutex locking overhead: Added (for thread-safe access)
- Function call frequency: ~1-10 times per second (settings updates)
- Cache locality: Improved (no 200-byte copy)

**Performance Impact:** POSITIVE (eliminates copy overhead)

**Issues Found:**
- **CRITICAL:** Lifetime management risk (use-after-free)
- **MEDIUM:** Mutex locking overhead added

**Recommendations:**
- **MUST FIX:** Revert to storing settings by value (copy) - the 200 bytes is acceptable trade-off for safety
- Alternative: Use static storage for settings with mutex protection (similar to other fixes)

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Analysis:**
- Heap allocation overhead: Eliminated (good)
- Fixed-size buffer overhead: None (stack allocation)
- Function call frequency: ~1-5 times per user interaction (file operations)
- Cache locality: Improved (no heap allocation, no pointer chasing)

**Performance Impact:** POSITIVE (eliminates heap allocation overhead)

**Issues Found:**
- **LOW:** PathUtils functions add slight overhead compared to std::filesystem::path

**Recommendations:**
- Profile PathUtils functions to ensure acceptable performance
- Optimize if necessary (e.g., use pointer arithmetic instead of size_t loops)

---

### Fix #10: UI buffers totaling 160 bytes

**Analysis:**
- Mutex locking overhead: ~10-20 cycles per lock/unlock
- Function call frequency: ~10-30 times per second (UI rendering)
- Cache locality: Moving to BSS may reduce cache locality slightly
- Double-buffering: Not used (single buffer with mutex)

**Performance Impact:** NEUTRAL (slight overhead from mutex, acceptable)

**Issues Found:**
- **LOW:** Moving to BSS may reduce cache locality slightly

**Recommendations:**
- Profile mutex contention to ensure acceptable performance
- Consider lock-free approach if contention is high (but use ChibiOS atomic, not std::atomic)

---

## ATTACK #3: MAYHEM COMPATIBILITY TEST

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Analysis:**
- Coding style: Follows Mayhem conventions (snake_case, trailing underscore for members)
- ChibiOS mutex: Uses chMtxLock/chMtxUnlock (compatible)
- RAII pattern: Compatible with Mayhem codebase (similar to existing MutexLock)
- Type aliases: Uses existing types (FilteredDronesSnapshot, Mutex)

**Compatibility:** COMPATIBLE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #2: spectrum_data_ (256 bytes) - Lock-free double-buffering

**Analysis:**
- Coding style: Follows Mayhem conventions
- std::atomic: **NOT COMPATIBLE** - FORBIDDEN per embedded constraints
- Lock-free pattern: Good concept, but uses forbidden std::atomic

**Compatibility:** INCOMPATIBLE (due to std::atomic)

**Issues Found:**
- **CRITICAL:** Uses std::atomic which is FORBIDDEN per embedded constraints

**Recommendations:**
- **MUST FIX:** Replace with mutex-protected double-buffering
- See "Revised Blueprint" section for corrected implementation

---

### Fix #3: power_levels (240 bytes)

**Analysis:**
- Coding style: Follows Mayhem conventions
- ChibiOS mutex: Uses chMtxLock/chMtxUnlock (compatible)
- RAII pattern: Compatible with Mayhem codebase
- Type aliases: Uses existing types (uint8_t, Mutex)

**Compatibility:** COMPATIBLE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #4: std::string temp_string_ (heap allocation)

**Analysis:**
- Coding style: Follows Mayhem conventions
- Fixed-size buffers: Compatible with Mayhem constraints
- TextEdit widget: Requires modification (unknown compatibility)

**Compatibility:** NEEDS ADJUSTMENT (TextEdit widget compatibility unknown)

**Issues Found:**
- **HIGH:** Requires TextEdit widget to support direct char* buffer access
- **HIGH:** If TextEdit widget cannot be modified, this fix may not be feasible

**Recommendations:**
- Verify TextEdit widget API before implementation
- Consider creating custom TextEdit variant if needed

---

### Fix #5: std::string filename (heap allocation)

**Analysis:**
- Coding style: Follows Mayhem conventions
- Fixed-size buffers: Compatible with Mayhem constraints
- std::filesystem::path: Used in existing codebase (acceptable)

**Compatibility:** COMPATIBLE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Analysis:**
- Coding style: Follows Mayhem conventions
- ChibiOS mutex: Uses chMtxLock/chMtxUnlock (compatible)
- RAII pattern: Compatible with Mayhem codebase
- Type aliases: Uses existing types (freqman_entry, Mutex)

**Compatibility:** COMPATIBLE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #7: histogram_buffer_ (67 bytes)

**Analysis:**
- Coding style: Follows Mayhem conventions
- ChibiOS mutex: Uses chMtxLock/chMtxUnlock (compatible)
- RAII pattern: Compatible with Mayhem codebase
- Type aliases: Uses existing types (SpectralAnalyzer::HistogramBuffer, Mutex)

**Compatibility:** COMPATIBLE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Analysis:**
- Coding style: Follows Mayhem conventions
- Reference passing: Compatible with Mayhem codebase
- Lifetime management: **NOT COMPATIBLE** - use-after-free risk

**Compatibility:** INCOMPATIBLE (due to lifetime management risk)

**Issues Found:**
- **CRITICAL:** Lifetime management risk - settings reference may outlive parent object
- **HIGH:** Blueprint shows `const DroneAnalyzerSettings& settings_` as member variable

**Recommendations:**
- **MUST FIX:** Revert to storing settings by value (copy) - the 200 bytes is acceptable trade-off for safety
- Alternative: Use static storage for settings with mutex protection (similar to other fixes)

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Analysis:**
- Coding style: Follows Mayhem conventions
- Fixed-size buffers: Compatible with Mayhem constraints
- PathUtils namespace: Compatible with Mayhem codebase

**Compatibility:** COMPATIBLE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #10: UI buffers totaling 160 bytes)

**Analysis:**
- Coding style: Follows Mayhem conventions
- ChibiOS mutex: Uses chMtxLock/chMtxUnlock (compatible)
- RAII pattern: Compatible with Mayhem codebase
- Type aliases: Uses existing types (Mutex)

**Compatibility:** COMPATIBLE

**Issues Found:**
- None

**Recommendations:**
- None

---

## ATTACK #4: CORNER CASES

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Analysis:**
- Empty input buffer: Handled by filter_stale_drones_in_place() logic
- SPI failure during data transfer: Not applicable (this is in-memory data)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: RAII wrapper ensures mutex is always released
- Memory initialization failure: BSS segment is zero-initialized at startup

**Corner Cases:**
- Mutex already locked: RAII constructor will block until available (may cause priority inversion)
- Nested guards: Potential deadlock (see Attack #1)

**Issues Found:**
- **MEDIUM:** Potential priority inversion if mutex is held by low-priority thread

**Recommendations:**
- Use chMtxLockTimeout() with reasonable timeout to prevent indefinite blocking
- Add documentation about priority inversion risk

---

### Fix #2: spectrum_data_ (256 bytes) - Lock-free double-buffering

**Analysis:**
- Empty input buffer: Handled by update_spectrum_data() logic
- SPI failure during data transfer: Not applicable (this is in-memory data)
- Double-buffer swap during ISR: Lock-free design handles this correctly
- Thread synchronization failure: Lock-free design avoids synchronization issues
- Memory initialization failure: BSS segment is zero-initialized at startup

**Corner Cases:**
- Writer updates buffer while reader is reading: Lock-free design handles this correctly
- Atomic operations not supported: **CRITICAL ISSUE** - std::atomic is forbidden

**Issues Found:**
- **CRITICAL:** Uses std::atomic which is FORBIDDEN per embedded constraints

**Recommendations:**
- **MUST FIX:** Replace with mutex-protected double-buffering
- See "Revised Blueprint" section for corrected implementation

---

### Fix #3: power_levels (240 bytes)

**Analysis:**
- Empty input buffer: Handled by process_mini_spectrum() logic
- SPI failure during data transfer: Not applicable (this is in-memory data)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: RAII wrapper ensures mutex is always released
- Memory initialization failure: BSS segment is zero-initialized at startup

**Corner Cases:**
- Buffer size mismatch: Bounds checking added (power_levels_size parameter)
- Mutex already locked: RAII constructor will block until available (may cause priority inversion)
- Nested guards: Potential deadlock (see Attack #1)

**Issues Found:**
- **MEDIUM:** Potential priority inversion if mutex is held by low-priority thread

**Recommendations:**
- Use chMtxLockTimeout() with reasonable timeout to prevent indefinite blocking
- Add documentation about priority inversion risk

---

### Fix #4: std::string temp_string_ (heap allocation)

**Analysis:**
- Empty input buffer: FixedStringBuffer handles empty strings correctly
- SPI failure during data transfer: Not applicable (this is UI-only)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: Not applicable (UI-only access)
- Memory initialization failure: FixedStringBuffer initializes buffer to empty string

**Corner Cases:**
- String too long for buffer: FixedStringBuffer::assign() truncates with bounds checking
- TextEdit modifies buffer without update_length() call: Length becomes stale
- TextEdit writes past buffer end: **CRITICAL ISSUE** - no bounds checking in TextEdit

**Issues Found:**
- **HIGH:** TextEdit may write past buffer end (no bounds checking)
- **MEDIUM:** TextEdit may modify buffer without update_length() call (length becomes stale)

**Recommendations:**
- Verify TextEdit widget API before implementation
- If TextEdit cannot be modified, consider creating custom variant
- Add documentation warning about calling update_length() after external modification

---

### Fix #5: std::string filename (heap allocation)

**Analysis:**
- Empty input buffer: PathUtils::extract_filename_stem() handles empty paths correctly
- SPI failure during data transfer: Not applicable (this is UI-only)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: Not applicable (UI-only access)
- Memory initialization failure: Char array is on stack (initialized at function entry)

**Corner Cases:**
- Filename too long for buffer: Bounds checking added (std::min with MAX_FILENAME_LENGTH - 1)
- Path with no extension: PathUtils::extract_filename_stem() handles this correctly
- Path with multiple dots: PathUtils::extract_filename_stem() finds last dot correctly
- std::filesystem::path::string() allocation failure: **CRITICAL ISSUE** - no error handling

**Issues Found:**
- **MEDIUM:** std::filesystem::path::string() allocation failure not handled

**Recommendations:**
- Add try-catch for std::bad_alloc (but exceptions are forbidden)
- Alternative: Use static buffer for path string and check for truncation
- Document that heap allocation is unavoidable for filesystem operations

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Analysis:**
- Empty input buffer: Handled by scanning logic
- SPI failure during data transfer: Not applicable (this is in-memory data)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: RAII wrapper ensures mutex is always released
- Memory initialization failure: BSS segment is zero-initialized at startup

**Corner Cases:**
- Mutex already locked: RAII constructor will block until available (may cause priority inversion)
- Nested guards: Potential deadlock (see Attack #1)
- Array accessed out of bounds: No bounds checking in blueprint

**Issues Found:**
- **MEDIUM:** Potential priority inversion if mutex is held by low-priority thread
- **LOW:** No bounds checking for array access

**Recommendations:**
- Use chMtxLockTimeout() with reasonable timeout to prevent indefinite blocking
- Add bounds checking for array access
- Add documentation about priority inversion risk

---

### Fix #7: histogram_buffer_ (67 bytes)

**Analysis:**
- Empty input buffer: Handled by spectrum analysis logic
- SPI failure during data transfer: Not applicable (this is in-memory data)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: RAII wrapper ensures mutex is always released
- Memory initialization failure: BSS segment is zero-initialized at startup

**Corner Cases:**
- Mutex already locked: RAII constructor will block until available (may cause priority inversion)
- Nested guards: Potential deadlock (see Attack #1)

**Issues Found:**
- **MEDIUM:** Potential priority inversion if mutex is held by low-priority thread

**Recommendations:**
- Use chMtxLockTimeout() with reasonable timeout to prevent indefinite blocking
- Add documentation about priority inversion risk

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Analysis:**
- Empty input buffer: Not applicable (settings struct)
- SPI failure during data transfer: Not applicable (this is in-memory data)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: Mutex-protected access
- Memory initialization failure: Settings loaded from persistence

**Corner Cases:**
- Settings object destroyed while scanner still using reference: **CRITICAL ISSUE** - use-after-free
- Settings modified while scanner is reading: Mutex-protected access handles this
- Settings reference is null: **CRITICAL ISSUE** - no null check

**Issues Found:**
- **CRITICAL:** Lifetime management risk - settings reference may outlive parent object
- **CRITICAL:** No null check for settings reference

**Recommendations:**
- **MUST FIX:** Revert to storing settings by value (copy) - the 200 bytes is acceptable trade-off for safety
- Alternative: Use static storage for settings with mutex protection (similar to other fixes)

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Analysis:**
- Empty input buffer: PathUtils::extract_filename_stem() handles empty paths correctly
- SPI failure during data transfer: Not applicable (this is UI-only)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: Not applicable (UI-only access)
- Memory initialization failure: Char arrays are on stack (initialized at function entry)

**Corner Cases:**
- Path too long for buffer: Bounds checking added (return false if buffer too small)
- Path with no extension: PathUtils::extract_filename_stem() handles this correctly
- Path with multiple dots: PathUtils::extract_filename_stem() finds last dot correctly
- Path with no directory separator: PathUtils::extract_filename_stem() handles this correctly
- Null pointer input: PathUtils::extract_filename_stem() checks for null

**Issues Found:**
- None

**Recommendations:**
- Add unit tests for edge cases (empty path, path with no extension, path with multiple dots)

---

### Fix #10: UI buffers totaling 160 bytes)

**Analysis:**
- Empty input buffer: Handled by UI rendering logic
- SPI failure during data transfer: Not applicable (this is UI-only)
- Double-buffer swap during ISR: Not applicable (no double-buffering)
- Thread synchronization failure: RAII wrapper ensures mutex is always released
- Memory initialization failure: BSS segment is zero-initialized at startup

**Corner Cases:**
- Mutex already locked: RAII constructor will block until available (may cause priority inversion)
- Nested guards: Potential deadlock (see Attack #1)
- Buffer overflow: No bounds checking in blueprint

**Issues Found:**
- **MEDIUM:** Potential priority inversion if mutex is held by low-priority thread
- **LOW:** No bounds checking for buffer access

**Recommendations:**
- Use chMtxLockTimeout() with reasonable timeout to prevent indefinite blocking
- Add bounds checking for buffer access
- Add documentation about priority inversion risk

---

## ATTACK #5: LOGIC CHECK

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Analysis:**
- Reasoning: Moving large stack allocation to static storage reduces stack usage
- Logic: Sound - static storage is zero-initialized at startup, RAII wrapper ensures thread safety
- Problem solving: Correctly addresses stack overflow risk
- Side effects: None identified

**Logic Status:** SOUND

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #2: spectrum_data_ (256 bytes) - Lock-free double-buffering

**Analysis:**
- Reasoning: Lock-free double-buffering avoids mutex overhead for high-frequency data
- Logic: Sound - lock-free pattern is well-established
- Problem solving: Correctly addresses stack overflow risk
- Side effects: Uses std::atomic which is FORBIDDEN

**Logic Status:** FLAWED (due to std::atomic usage)

**Issues Found:**
- **CRITICAL:** Uses std::atomic which is FORBIDDEN per embedded constraints

**Recommendations:**
- **MUST FIX:** Replace with mutex-protected double-buffering
- See "Revised Blueprint" section for corrected implementation

---

### Fix #3: power_levels (240 bytes)

**Analysis:**
- Reasoning: Moving large stack allocation to static storage reduces stack usage
- Logic: Sound - static storage is zero-initialized at startup, RAII wrapper ensures thread safety
- Problem solving: Correctly addresses stack overflow risk
- Side effects: None identified

**Logic Status:** SOUND

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #4: std::string temp_string_ (heap allocation)

**Analysis:**
- Reasoning: Replacing std::string with fixed-size buffer eliminates heap allocation
- Logic: Sound - FixedStringBuffer provides std::string-like interface without heap allocation
- Problem solving: Correctly addresses heap allocation violation
- Side effects: Requires TextEdit widget modification

**Logic Status:** SOUND (with caveat)

**Issues Found:**
- **HIGH:** Requires TextEdit widget to support direct char* buffer access

**Recommendations:**
- Verify TextEdit widget API before implementation
- Consider creating custom TextEdit variant if needed

---

### Fix #5: std::string filename (heap allocation)

**Analysis:**
- Reasoning: Replacing std::string with fixed-size buffer reduces heap allocation
- Logic: Sound - char array on stack eliminates heap allocation for filename
- Problem solving: Correctly addresses heap allocation violation
- Side effects: std::filesystem::path still allocates (unavoidable)

**Logic Status:** SOUND (with caveat)

**Issues Found:**
- **LOW:** std::filesystem::path still allocates internally (unavoidable)

**Recommendations:**
- Accept temporary heap allocation (unavoidable with std::filesystem::path)
- Document that heap allocation is unavoidable for filesystem operations

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Analysis:**
- Reasoning: Moving large stack allocation to static storage reduces stack usage
- Logic: Sound - static storage is zero-initialized at startup, RAII wrapper ensures thread safety
- Problem solving: Correctly addresses stack overflow risk
- Side effects: None identified

**Logic Status:** SOUND

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #7: histogram_buffer_ (67 bytes)

**Analysis:**
- Reasoning: Moving stack allocation to static storage reduces stack usage
- Logic: Sound - static storage is zero-initialized at startup, RAII wrapper ensures thread safety
- Problem solving: Correctly addresses stack overflow risk
- Side effects: None identified

**Logic Status:** SOUND

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Analysis:**
- Reasoning: Passing by reference eliminates copy overhead
- Logic: **FLAWED** - reference lifetime management is complex and error-prone
- Problem solving: Addresses stack usage but introduces use-after-free risk
- Side effects: Lifetime management risk

**Logic Status:** FLAWED (due to lifetime management risk)

**Issues Found:**
- **CRITICAL:** Lifetime management risk - settings reference may outlive parent object
- **HIGH:** Blueprint shows `const DroneAnalyzerSettings& settings_` as member variable

**Recommendations:**
- **MUST FIX:** Revert to storing settings by value (copy) - the 200 bytes is acceptable trade-off for safety
- Alternative: Use static storage for settings with mutex protection (similar to other fixes)

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Analysis:**
- Reasoning: Replacing std::filesystem::path with fixed-size buffers eliminates heap allocation
- Logic: Sound - PathUtils functions provide path operations without heap allocation
- Problem solving: Correctly addresses heap allocation violation
- Side effects: None identified

**Logic Status:** SOUND

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #10: UI buffers totaling 160 bytes)

**Analysis:**
- Reasoning: Moving stack allocations to static storage reduces stack usage
- Logic: Sound - static storage is zero-initialized at startup, RAII wrapper ensures thread safety
- Problem solving: Correctly addresses stack overflow risk
- Side effects: None identified

**Logic Status:** SOUND

**Issues Found:**
- None

**Recommendations:**
- None

---

## ATTACK #6: THREAD SAFETY ANALYSIS

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Analysis:**
- Race conditions: RAII wrapper ensures exclusive access via mutex
- Locking strategy: Correct - lock on construction, unlock on destruction
- Deadlock risk: Low - single mutex, no nesting in current design
- Memory ordering: Not applicable (mutex provides full barrier)

**Thread Safety:** SAFE

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add documentation about lock ordering requirements

---

### Fix #2: spectrum_data_ (256 bytes) - Lock-free double-buffering

**Analysis:**
- Race conditions: Lock-free design handles concurrent access correctly
- Locking strategy: Not applicable (lock-free)
- Deadlock risk: None (no locks)
- Memory ordering: Uses std::memory_order_acquire/release (correct for double-buffering)

**Thread Safety:** SAFE (but uses forbidden std::atomic)

**Issues Found:**
- **CRITICAL:** Uses std::atomic which is FORBIDDEN per embedded constraints

**Recommendations:**
- **MUST FIX:** Replace with mutex-protected double-buffering
- See "Revised Blueprint" section for corrected implementation

---

### Fix #3: power_levels (240 bytes)

**Analysis:**
- Race conditions: RAII wrapper ensures exclusive access via mutex
- Locking strategy: Correct - lock on construction, unlock on destruction
- Deadlock risk: Low - single mutex, no nesting in current design
- Memory ordering: Not applicable (mutex provides full barrier)

**Thread Safety:** SAFE

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add documentation about lock ordering requirements

---

### Fix #4: std::string temp_string_ (heap allocation)

**Analysis:**
- Race conditions: Not applicable (UI-only access)
- Locking strategy: Not applicable (UI-only access)
- Deadlock risk: None (no locks)
- Memory ordering: Not applicable (single-threaded access)

**Thread Safety:** SAFE (UI-only access)

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #5: std::string filename (heap allocation)

**Analysis:**
- Race conditions: Not applicable (UI-only access)
- Locking strategy: Not applicable (UI-only access)
- Deadlock risk: None (no locks)
- Memory ordering: Not applicable (single-threaded access)

**Thread Safety:** SAFE (UI-only access)

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Analysis:**
- Race conditions: RAII wrapper ensures exclusive access via mutex
- Locking strategy: Correct - lock on construction, unlock on destruction
- Deadlock risk: Low - single mutex, no nesting in current design
- Memory ordering: Not applicable (mutex provides full barrier)

**Thread Safety:** SAFE

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add documentation about lock ordering requirements

---

### Fix #7: histogram_buffer_ (67 bytes)

**Analysis:**
- Race conditions: RAII wrapper ensures exclusive access via mutex
- Locking strategy: Correct - lock on construction, unlock on destruction
- Deadlock risk: Low - single mutex, no nesting in current design
- Memory ordering: Not applicable (mutex provides full barrier)

**Thread Safety:** SAFE

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add documentation about lock ordering requirements

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Analysis:**
- Race conditions: Mutex-protected access to settings
- Locking strategy: Correct - lock on access, unlock after access
- Deadlock risk: Low - single mutex, no nesting in current design
- Memory ordering: Not applicable (mutex provides full barrier)

**Thread Safety:** SAFE (with lifetime caveat)

**Issues Found:**
- **CRITICAL:** Lifetime management risk - settings reference may outlive parent object

**Recommendations:**
- **MUST FIX:** Revert to storing settings by value (copy) - the 200 bytes is acceptable trade-off for safety
- Alternative: Use static storage for settings with mutex protection (similar to other fixes)

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Analysis:**
- Race conditions: Not applicable (UI-only access)
- Locking strategy: Not applicable (UI-only access)
- Deadlock risk: None (no locks)
- Memory ordering: Not applicable (single-threaded access)

**Thread Safety:** SAFE (UI-only access)

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #10: UI buffers totaling 160 bytes)

**Analysis:**
- Race conditions: RAII wrapper ensures exclusive access via mutex
- Locking strategy: Correct - lock on construction, unlock on destruction
- Deadlock risk: Low - single mutex, no nesting in current design
- Memory ordering: Not applicable (mutex provides full barrier)

**Thread Safety:** SAFE

**Issues Found:**
- **MEDIUM:** Potential deadlock if nested with other guards without lock ordering

**Recommendations:**
- Define lock ordering constants to prevent deadlock
- Add documentation about lock ordering requirements

---

## ATTACK #7: MEMORY LIFETIME ANALYSIS

### Fix #1: FilteredDronesSnapshot (640 bytes)

**Analysis:**
- Lifetime: Application lifetime (static storage in BSS segment)
- Initialization order: Zero-initialized at startup (no static initialization fiasco)
- Cleanup: No cleanup needed (BSS segment is freed at application exit)
- Thread safety: RAII wrapper ensures safe access

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #2: spectrum_data_ (256 bytes) - Lock-free double-buffering

**Analysis:**
- Lifetime: Application lifetime (static storage in BSS segment)
- Initialization order: Zero-initialized at startup (no static initialization fiasco)
- Cleanup: No cleanup needed (BSS segment is freed at application exit)
- Thread safety: Lock-free design ensures safe access

**Lifetime Management:** SAFE (but uses forbidden std::atomic)

**Issues Found:**
- **CRITICAL:** Uses std::atomic which is FORBIDDEN per embedded constraints

**Recommendations:**
- **MUST FIX:** Replace with mutex-protected double-buffering
- See "Revised Blueprint" section for corrected implementation

---

### Fix #3: power_levels (240 bytes)

**Analysis:**
- Lifetime: Application lifetime (static storage in BSS segment)
- Initialization order: Zero-initialized at startup (no static initialization fiasco)
- Cleanup: No cleanup needed (BSS segment is freed at application exit)
- Thread safety: RAII wrapper ensures safe access

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #4: std::string temp_string_ (heap allocation)

**Analysis:**
- Lifetime: Parent class lifetime (FixedStringBuffer is non-owning)
- Initialization order: Initialized in parent class constructor (no static initialization fiasco)
- Cleanup: No cleanup needed (FixedStringBuffer is non-owning)
- Thread safety: UI-only access (single-threaded)

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #5: std::string filename (heap allocation)

**Analysis:**
- Lifetime: Function scope (char array on stack)
- Initialization order: Initialized at function entry (no static initialization fiasco)
- Cleanup: Stack frame is freed at function exit (no cleanup needed)
- Thread safety: UI-only access (single-threaded)

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #6: entries_to_scan_ (1000 bytes)

**Analysis:**
- Lifetime: Application lifetime (static storage in BSS segment)
- Initialization order: Zero-initialized at startup (no static initialization fiasco)
- Cleanup: No cleanup needed (BSS segment is freed at application exit)
- Thread safety: RAII wrapper ensures safe access

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #7: histogram_buffer_ (67 bytes)

**Analysis:**
- Lifetime: Application lifetime (static storage in BSS segment)
- Initialization order: Zero-initialized at startup (no static initialization fiasco)
- Cleanup: No cleanup needed (BSS segment is freed at application exit)
- Thread safety: RAII wrapper ensures safe access

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #8: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Analysis:**
- Lifetime: **UNSAFE** - reference lifetime depends on parent object
- Initialization order: Initialized in constructor (no static initialization fiasco)
- Cleanup: **UNSAFE** - reference becomes dangling if parent object is destroyed
- Thread safety: Mutex-protected access (safe if reference is valid)

**Lifetime Management:** UNSAFE (due to use-after-free risk)

**Issues Found:**
- **CRITICAL:** Lifetime management risk - settings reference may outlive parent object
- **HIGH:** Blueprint shows `const DroneAnalyzerSettings& settings_` as member variable

**Recommendations:**
- **MUST FIX:** Revert to storing settings by value (copy) - the 200 bytes is acceptable trade-off for safety
- Alternative: Use static storage for settings with mutex protection (similar to other fixes)

---

### Fix #9: Multiple std::filesystem::path uses (heap allocation)

**Analysis:**
- Lifetime: Function scope (char arrays on stack)
- Initialization order: Initialized at function entry (no static initialization fiasco)
- Cleanup: Stack frames are freed at function exit (no cleanup needed)
- Thread safety: UI-only access (single-threaded)

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

### Fix #10: UI buffers totaling 160 bytes)

**Analysis:**
- Lifetime: Application lifetime (static storage in BSS segment)
- Initialization order: Zero-initialized at startup (no static initialization fiasco)
- Cleanup: No cleanup needed (BSS segment is freed at application exit)
- Thread safety: RAII wrapper ensures safe access

**Lifetime Management:** SAFE

**Issues Found:**
- None

**Recommendations:**
- None

---

## SUMMARY OF ATTACK RESULTS

### Total Issues Found: 12

**Critical Issues: 2**
1. Fix #2: Uses std::atomic which is FORBIDDEN per embedded constraints
2. Fix #8: Lifetime management risk - settings reference may outlive parent object

**High Priority Issues: 4**
1. Fix #4: Requires TextEdit widget to support direct char* buffer access
2. Fix #4: TextEdit may write past buffer end (no bounds checking)
3. Fix #8: Blueprint shows `const DroneAnalyzerSettings& settings_` as member variable
4. Fix #8: No null check for settings reference

**Medium Priority Issues: 4**
1. Fix #3: Potential deadlock if nested with other guards without lock ordering
2. Fix #5: std::filesystem::path::string() allocation failure not handled
3. Fix #6: Potential deadlock if nested with other guards without lock ordering
4. Fix #7: Potential deadlock if nested with other guards without lock ordering

**Low Priority Issues: 2**
1. Fix #6: No bounds checking for array access
2. Fix #10: No bounds checking for buffer access

---

## REVISED BLUEPRINT

### Critical Fix #1: spectrum_data_ (256 bytes) - Replace std::atomic with mutex

**Original Issue:**
- Uses `std::atomic<uint8_t>` which is FORBIDDEN per embedded constraints

**Revised Solution:**

```cpp
// REVISED: Move spectrum_data_ to static storage with mutex-protected double-buffering
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
private:
    // Static storage for spectrum data (BSS segment) - mutex-protected
    struct SpectrumDataBuffer {
        std::array<uint8_t, 256> buffers[2];  // Double-buffered
        uint8_t active_index;  // Index of active buffer (0 or 1)
    };
    
    alignas(alignof(SpectrumDataBuffer))
    static uint8_t g_spectrum_data_storage[sizeof(SpectrumDataBuffer)];
    
    // Mutex for protecting spectrum data access
    static Mutex g_spectrum_data_mutex;
    
    // RAII wrapper for spectrum data access
    class SpectrumDataGuard {
    public:
        SpectrumDataGuard() noexcept {
            chMtxLock(&g_spectrum_data_mutex);
        }
        
        ~SpectrumDataGuard() noexcept {
            chMtxUnlock(&g_spectrum_data_mutex);
        }
        
        // Non-copyable, non-movable
        SpectrumDataGuard(const SpectrumDataGuard&) = delete;
        SpectrumDataGuard& operator=(const SpectrumDataGuard&) = delete;
        
        [[nodiscard]] SpectrumDataBuffer* get() noexcept {
            return reinterpret_cast<SpectrumDataBuffer*>(g_spectrum_data_storage);
        }
        
        [[nodiscard]] const SpectrumDataBuffer* get() const noexcept {
            return reinterpret_cast<const SpectrumDataBuffer*>(g_spectrum_data_storage);
        }
        
    private:
    };
    
    // Accessor methods
    inline const std::array<uint8_t, 256>& get_spectrum_data() const noexcept {
        SpectrumDataGuard guard;
        const SpectrumDataBuffer* buffer = guard.get();
        return buffer->buffers[buffer->active_index];
    }
    
    inline void update_spectrum_data(const std::array<uint8_t, 256>& new_data) noexcept {
        SpectrumDataGuard guard;
        SpectrumDataBuffer* buffer = guard.get();
        uint8_t write_idx = 1 - buffer->active_index;
        buffer->buffers[write_idx] = new_data;
        buffer->active_index = write_idx;
    }
};
```

**Changes from Original:**
- Replaced `std::atomic<uint8_t>` with mutex-protected access
- Added RAII wrapper for thread-safe access
- Maintained double-buffering pattern for performance

---

### Critical Fix #2: DroneAnalyzerSettings passed by value (~200 bytes copy)

**Original Issue:**
- Lifetime management risk - settings reference may outlive parent object

**Revised Solution (Option 1 - Revert to Value Copy):**

```cpp
// REVISED: Keep settings by value (accept 200 bytes copy overhead)
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
public:
    // Constructor - accepts settings by value (copy)
    explicit DroneScanner(DroneAnalyzerSettings settings);
    
    // Update scanner's settings from view's settings
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
    // Keep settings by value (200 bytes) - safe lifetime management
    DroneAnalyzerSettings settings_;
};
```

**Revised Solution (Option 2 - Static Storage):**

```cpp
// REVISED: Move settings to static storage with mutex protection
// File: ui_enhanced_drone_analyzer.hpp (DroneScanner class)

class DroneScanner {
private:
    // Static storage for settings (BSS segment)
    alignas(alignof(DroneAnalyzerSettings))
    static uint8_t g_settings_storage[sizeof(DroneAnalyzerSettings)];
    
    // Mutex for protecting settings access
    static Mutex g_settings_mutex;
    
    // RAII wrapper for settings access
    class SettingsGuard {
    public:
        SettingsGuard() noexcept {
            chMtxLock(&g_settings_mutex);
        }
        
        ~SettingsGuard() noexcept {
            chMtxUnlock(&g_settings_mutex);
        }
        
        // Non-copyable, non-movable
        SettingsGuard(const SettingsGuard&) = delete;
        SettingsGuard& operator=(const SettingsGuard&) = delete;
        
        [[nodiscard]] DroneAnalyzerSettings* get() noexcept {
            return reinterpret_cast<DroneAnalyzerSettings*>(g_settings_storage);
        }
        
        [[nodiscard]] const DroneAnalyzerSettings* get() const noexcept {
            return reinterpret_cast<const DroneAnalyzerSettings*>(g_settings_storage);
        }
        
    private:
    };
    
    // Accessor methods
    inline DroneAnalyzerSettings& get_settings() noexcept {
        SettingsGuard guard;
        return *guard.get();
    }
    
    inline const DroneAnalyzerSettings& get_settings() const noexcept {
        SettingsGuard guard;
        return *guard.get();
    }
};
```

**Recommendation:**
- Use Option 1 (Revert to Value Copy) for simplicity and safety
- The 200 bytes copy overhead is acceptable given the safety trade-off
- Option 2 (Static Storage) adds complexity and may not be worth the effort

---

### Additional Recommendations

**Lock Ordering Constants:**

```cpp
// File: eda_locking.hpp (add to namespace EDA)

namespace LockOrder {
    constexpr int DATA_MUTEX = 1;
    constexpr int FILTERED_DRONES_MUTEX = 2;
    constexpr int SPECTRUM_DATA_MUTEX = 3;
    constexpr int POWER_LEVELS_MUTEX = 4;
    constexpr int ENTRIES_TO_SCAN_MUTEX = 5;
    constexpr int HISTOGRAM_BUFFER_MUTEX = 6;
    constexpr int UI_BUFFERS_MUTEX = 7;
    constexpr int SETTINGS_MUTEX = 8;
}

// Use lock ordering in RAII guards to prevent deadlock
// Example:
class FilteredDronesSnapshotGuard {
public:
    FilteredDronesSnapshotGuard() noexcept {
        // Lock with timeout to prevent priority inversion
        if (!chMtxLockTimeout(&StaticStorage::g_filtered_drones_mutex, TIME_MS2I(100))) {
            // Handle timeout (log error, use default values, etc.)
        }
    }
    
    ~FilteredDronesSnapshotGuard() noexcept {
        chMtxUnlock(&StaticStorage::g_filtered_drones_mutex);
    }
    
    // ... rest of the implementation
};
```

**Bounds Checking:**

```cpp
// Add bounds checking for array access
// Example:
inline std::array<freqman_entry, 10>& get_entries_to_scan() noexcept {
    EntriesToScanGuard guard;
    std::array<freqman_entry, 10>* entries = guard.get();
    // Bounds checking is implicit (std::array provides at() method)
    return *entries;
}

// Use at() method for bounds-checked access
freqman_entry& entry = entries->at(index);  // Throws std::out_of_range if index >= 10
```

**TextEdit Widget Compatibility:**

```cpp
// Before implementing Fix #4, verify TextEdit widget API
// Check if TextEdit supports direct char* buffer access

// If TextEdit does NOT support direct char* buffer access:
// Option 1: Create custom TextEdit variant
// Option 2: Keep std::string (accept heap allocation for this specific case)
// Option 3: Use different UI widget that supports fixed-size buffers

// Recommendation: Verify TextEdit widget API before implementing Fix #4
```

---

## FINAL VERIFICATION STATUS

**Blueprint Status:** NEEDS REVISION

**Critical Issues Requiring Fix:**
1. Fix #2: Replace std::atomic with mutex-protected double-buffering
2. Fix #8: Revert to storing settings by value (copy) or use static storage

**Ready for Implementation:** NO

**Additional Recommendations:**
1. Add lock ordering constants to prevent deadlock
2. Add bounds checking for array access
3. Verify TextEdit widget API before implementing Fix #4
4. Add timeout to mutex locks to prevent priority inversion
5. Add unit tests for edge cases
6. Profile performance impact of mutex locking

---

**End of Stage 3: Red Team Attack**

Next Stage: Stage 4 - Diamond Code Synthesis (after critical issues are resolved)
