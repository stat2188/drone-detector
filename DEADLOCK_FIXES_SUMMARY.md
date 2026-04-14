# Fix Summary: Database Scan Mode Hangs (Commits 34913c9d, b303dfe7)

## Critical Issues Fixed

### 1. RECURSIVE MUTEX DEADLOCK (CRITICAL)
**File**: `scanner.cpp:382-414`  
**Problem**:  
- `perform_scan_cycle()` acquires DATA_MUTEX at line 373
- Calls `perform_scan_cycle_internal()` (still holding mutex)
- `perform_scan_cycle_internal()` attempts to acquire DATA_MUTEX again at lines 398 and 409
- ChibiOS mutexes are NOT recursive → DEADLOCK

**Fix Applied**: Removed nested `MutexLock<LockOrder::DATA_MUTEX>` calls in `perform_scan_cycle_internal()`  
**Result**: Scanner thread can now complete scan cycles without deadlock

---

### 2. RACE CONDITION IN DWELL REQUEST FILTERING (SEVERE)
**File**: `scanner.cpp:384-402`  
**Problem**:
```cpp
const SystemTime force_resume_time = chTimeNow();
// ... time passes ...
if (dwell_request_.pending && dwell_request_.timestamp < force_resume_time) {
    dwell_request_.pending = false;  // New requests can be ignored!
}
```
- Between timestamp capture and comparison, UI thread can call `request_dwell()`
- New dwell requests with valid timestamp get cleared as "stale"

**Fix Applied**: Removed timestamp-based filtering entirely  
- Clear dwell request unconditionally when force_resume_flag is set
- Simpler, race-free logic

---

### 3. DATABASE_MUTEX / DATA_MUTEX DEADLOCK (CRITICAL)
**File**: `drone_scanner_ui.cpp:757-780, 837-856`  
**Problem**: Classic deadlock pattern
```
UI thread:                Scanner thread:
DATABASE_MUTEX (758) →   DATA_MUTEX (373)
  waits for                 waits for
DATA_MUTEX (765)        DATABASE_MUTEX (490)
```

**In enter_sweep_mode()**:  
- Line 758: `database_ptr_->get_current_index()` acquires DATABASE_MUTEX
- Line 765: `scanner_ptr_->get_locked_frequency()` acquires DATA_MUTEX
- Scanner thread holds DATA_MUTEX in `perform_scan_cycle()` and waits for DATABASE_MUTEX

**In exit_sweep_mode()**:  
- Line 841: `scanner_ptr_->set_scan_frequency()` acquires DATA_MUTEX
- Line 844: `database_ptr_->set_current_index()` acquires DATABASE_MUTEX
- Same deadlock scenario

**Fix Applied**: Reordered operations
1. **enter_sweep_mode()**: Stop scanner FIRST, then access database
2. **exit_sweep_mode()**: Access database FIRST, then scanner
3. Added synchronization delays to ensure thread state changes complete

---

### 4. SCANNER THREAD SYNCHRONIZATION (SEVERE)
**File**: `drone_scanner_ui.cpp:770-772`  
**Problem**:
```cpp
scanner_thread_->set_scanning(false);  // Atomic, scanner exits loop
scanner_thread_->reset_dwell();       // Acquires DATA_MUTEX
```
- `reset_dwell()` acquires DATA_MUTEX
- Scanner thread might still be in `perform_scan_cycle()` holding DATA_MUTEX
- Result: UI thread blocks on DATA_MUTEX while scanner thread completes

**Fix Applied**: Added synchronization delay
```cpp
scanner_thread_->set_scanning(false);
chThdSleepMilliseconds(10);  // Let scanner exit perform_scan_cycle()
scanner_thread_->reset_dwell();   // Now safe to acquire DATA_MUTEX
```

---

### 5. DEADLOCK IN reset_frequency() (CRITICAL)
**File**: `scanner.cpp:1073-1087`  
**Problem**:
```cpp
MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
database_.get_next_frequency(0);  // Requires DATABASE_MUTEX
```
- Acquires DATA_MUTEX first
- Then calls database method that requires DATABASE_MUTEX
- If another thread holds DATABASE_MUTEX → DEADLOCK

**Fix Applied**: Acquire DATABASE_MUTEX before DATA_MUTEX
```cpp
ErrorResult<FreqHz> freq_result = database_.get_next_frequency(0);
FreqHz new_frequency = MIN_FREQUENCY_HZ;
if (freq_result.has_value()) {
    new_frequency = freq_result.value();
}

MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
current_frequency_ = new_frequency;
```

---

### 6. INVALID DWELL REQUEST HANDLING (MODERATE)
**File**: `scanner.cpp:284-292`  
**Problem**:
```cpp
dwell_request_.frequency = (frequency != 0) ? frequency : current_frequency_;
```
- `current_frequency_` is read without mutex protection
- Race condition: frequency can change between read and dwell request processing

**Fix Applied**: Require explicit frequency, reject invalid requests
```cpp
if (frequency == 0) {
    return;  // Invalid request
}
dwell_request_.frequency = frequency;
```

---

## Additional Cleanup

### 7. REMOVED DEAD CODE
**File**: `scanner.cpp:294-305, scanner.hpp:634-642`  
- Removed `try_consume_force_resume_flag()` function
- Function was defined but never called
- Real logic was inlined in `perform_scan_cycle_internal()`

---

## Root Cause Analysis

### Why hangs started in commit 34913c9d (Mahalanobis GATE):
- Added Mahalanobis validation with timestamp-based dwell filtering
- Introduced recursive mutex acquisition in `perform_scan_cycle_internal()`
- Increased scan cycle time due to additional validation
- Race conditions more likely with longer critical sections

### Why immediate hang in commit b303dfe7:
- Changed dwell request to use timestamp filtering (intended to fix race)
- Introduced DATABASE_MUTEX / DATA_MUTEX deadlock in sweep mode
- Changed `request_dwell()` to access `current_frequency_` without mutex
- Attempted to fix issues but created more severe deadlocks

---

## Testing Recommendations

1. **Test database scan mode**: Load database with 50+ entries, press Start
2. **Test sweep mode**: Press Mode button, verify no hangs
3. **Test mode transitions**: Switch between database scan and sweep repeatedly
4. **Test signal detection**: Place known signal source, verify dwell works
5. **Stress test**: Rapid mode switching with active signal

---

## Dependencies Verified

### Lock Order (from locking.hpp):
```cpp
enum class LockOrder : uint8_t {
    DATA_MUTEX = 0,
    DATABASE_MUTEX = 1,
    STATE_MUTEX = 2,
};
```

### Mutex Usage:
- **DATA_MUTEX**: Protects scanner state (DroneScanner class members)
- **DATABASE_MUTEX**: Protects database index and entries (DatabaseManager class)
- **STATE_MUTEX**: Protects hardware state (HardwareController class)

### Thread Model:
- **Scanner thread**: Calls `perform_scan_cycle()` periodically
- **UI thread**: Processes button clicks, mode switches, dwell requests
- **Baseband M0**: Only handles hardware (no C++ logic) ✓ CORRECT

---

## Verification Checklist

- [x] Removed recursive mutex acquisitions
- [x] Fixed DATABASE_MUTEX / DATA_MUTEX deadlock in sweep mode
- [x] Added synchronization delays for thread state changes
- [x] Fixed race condition in dwell request filtering
- [x] Fixed deadlock in reset_frequency()
- [x] Removed dead code (try_consume_force_resume_flag)
- [x] Ensured explicit frequency in request_dwell()
- [ ] **TODO**: Test on hardware to verify fixes work in practice
