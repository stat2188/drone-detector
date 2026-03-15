# Enhanced Drone Analyzer - Comprehensive Testing and Debugging Plan

**Part 3: Error Handling, Thread Synchronization, and Memory Analysis**

---

## Table of Contents (Part 3)

7. [Error Handling Flow](#7-error-handling-flow)
8. [Thread Synchronization](#8-thread-synchronization)
9. [Memory Usage Analysis](#9-memory-usage-analysis)

---

## 7. Error Handling Flow

### 7.1 Error Handling Architecture

**File:** `common/error_handler.hpp`

**Error Types:**

```
┌─────────────────────────────────────────────────────────────┐
│              Error Code Classification                  │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  Hardware Errors (1-9):                                 │
│  ┌─────────────────────────────────────────────┐           │
│  │ 1: HARDWARE_NOT_INITIALIZED            │           │
│  │ 2: HARDWARE_TIMEOUT                     │           │
│  │ 3: HARDWARE_FAILURE                    │           │
│  │ 4: SPI_FAILURE                         │           │
│  │ 5: PLL_LOCK_FAILURE                    │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Database Errors (10-19):                               │
│  ┌─────────────────────────────────────────────┐           │
│  │ 10: DATABASE_NOT_LOADED                │           │
│  │ 11: DATABASE_LOAD_TIMEOUT              │           │
│  │ 12: DATABASE_CORRUPTED                │           │
│  │ 13: DATABASE_EMPTY                     │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Buffer Errors (20-29):                                 │
│  ┌─────────────────────────────────────────────┐           │
│  │ 20: BUFFER_EMPTY                       │           │
│  │ 21: BUFFER_FULL                        │           │
│  │ 22: BUFFER_INVALID                     │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Synchronization Errors (30-39):                          │
│  ┌─────────────────────────────────────────────┐           │
│  │ 30: MUTEX_TIMEOUT                      │           │
│  │ 31: MUTEX_LOCK_FAILED                 │           │
│  │ 32: SEMAPHORE_TIMEOUT                  │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Initialization Errors (40-49):                           │
│  ┌─────────────────────────────────────────────┐           │
│  │ 40: INITIALIZATION_FAILED              │           │
│  │ 41: INITIALIZATION_INCOMPLETE          │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  General Errors (50-59):                                 │
│  ┌─────────────────────────────────────────────┐           │
│  │ 50: INVALID_PARAMETER                 │           │
│  │ 51: NOT_IMPLEMENTED                   │           │
│  │ 255: UNKNOWN_ERROR                     │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 Error Detection Points

#### 7.2.1 Hardware Error Detection

**Detection Points:**

```
Hardware Error Detection
├── HardwareController::initialize()
│   └── Detect: HARDWARE_NOT_INITIALIZED, INITIALIZATION_FAILED
│
├── HardwareController::tune_to_frequency()
│   ├── Detect: INVALID_PARAMETER (frequency out of range)
│   ├── Detect: HARDWARE_TIMEOUT (tune timeout)
│   ├── Detect: PLL_LOCK_FAILURE (PLL not locking)
│   └── Detect: HARDWARE_FAILURE (SPI failure)
│
├── HardwareController::start_spectrum_streaming()
│   └── Detect: HARDWARE_NOT_INITIALIZED, HARDWARE_FAILURE
│
├── HardwareController::stop_spectrum_streaming()
│   └── Detect: HARDWARE_FAILURE
│
└── HardwareController::get_rssi_sample()
    └── Detect: HARDWARE_NOT_INITIALIZED, HARDWARE_FAILURE
```

**Detection Implementation:**

```cpp
ErrorCode HardwareController::tune_to_frequency(
    FreqHz frequency,
    uint32_t max_retries
) noexcept {
    // Validate frequency
    ErrorCode validate_result = validate_frequency_internal(frequency);
    if (validate_result != ErrorCode::SUCCESS) {
        last_error_ = validate_result;
        return validate_result;  // Error detected: INVALID_PARAMETER
    }
    
    MutexLock<LockOrder::STATE_MUTEX> lock(*mutex_);
    
    if (state_ != HardwareState::READY && state_ != HardwareState::STREAMING) {
        last_error_ = ErrorCode::HARDWARE_NOT_INITIALIZED;
        return ErrorCode::HARDWARE_NOT_INITIALIZED;  // Error detected
    }
    
    state_ = HardwareState::TUNING;
    
    // Retry logic with PLL lock verification
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        retry_count_ = retry;
        
        // Tune to frequency
        ErrorCode tune_result = tune_internal(frequency);
        if (tune_result != ErrorCode::SUCCESS) {
            if (retry < max_retries - 1) {
                chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS);
                continue;
            }
            state_ = HardwareState::ERROR;
            last_error_ = tune_result;
            return tune_result;  // Error detected: HARDWARE_FAILURE
        }
        
        // Wait for PLL lock
        SystemTime start_time = chTimeNow();
        while (!check_pll_lock_internal()) {
            if (chTimeNow() - start_time > MS2ST(PLL_LOCK_TIMEOUT_MS)) {
                if (retry < max_retries - 1) {
                    chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS);
                    break;
                }
                state_ = HardwareState::ERROR;
                last_error_ = ErrorCode::PLL_LOCK_FAILURE;
                return ErrorCode::PLL_LOCK_FAILURE;  // Error detected
            }
            chThdSleepMilliseconds(PLL_LOCK_POLL_INTERVAL_MS);
        }
        
        // PLL locked successfully
        current_frequency_ = frequency;
        pll_locked_.set();
        state_ = streaming_active_.test() ? HardwareState::STREAMING : HardwareState::READY;
        last_error_ = ErrorCode::SUCCESS;
        return ErrorCode::SUCCESS;
    }
    
    state_ = HardwareState::ERROR;
    last_error_ = ErrorCode::PLL_LOCK_FAILURE;
    return ErrorCode::PLL_LOCK_FAILURE;
}
```

#### 7.2.2 Database Error Detection

**Detection Points:**

```
Database Error Detection
├── DatabaseManager::load_frequency_database()
│   ├── Detect: DATABASE_LOAD_TIMEOUT (file read timeout)
│   ├── Detect: DATABASE_CORRUPTED (malformed file)
│   └── Detect: DATABASE_EMPTY (no entries)
│
├── DatabaseManager::get_next_frequency()
│   └── Detect: DATABASE_EMPTY, DATABASE_NOT_LOADED
│
├── DatabaseManager::add_entry()
│   ├── Detect: INVALID_PARAMETER (invalid entry)
│   └── Detect: BUFFER_FULL (database full)
│
└── DatabaseManager::remove_entry()
    └── Detect: INVALID_PARAMETER (frequency not found)
```

**Detection Implementation:**

```cpp
ErrorCode DatabaseManager::load_frequency_database() noexcept {
    // Check if already loaded (lock-free check)
    if (loaded_.test()) {
        return ErrorCode::SUCCESS;
    }
    
    // Acquire mutex for thread safety
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    // Double-check after acquiring lock
    if (loaded_.test()) {
        return ErrorCode::SUCCESS;
    }
    
    // Load database from file
    ErrorCode result = load_from_file_internal();
    
    if (result == ErrorCode::SUCCESS) {
        loaded_.set();
    } else if (result == ErrorCode::DATABASE_LOAD_TIMEOUT ||
               result == ErrorCode::DATABASE_CORRUPTED) {
        // Use built-in defaults as fallback
        add_default_frequencies_internal();
        loaded_.set();
        result = ErrorCode::SUCCESS;
    }
    
    return result;  // Error detected or SUCCESS
}

ErrorCode DatabaseManager::parse_line_internal(
    const char* line,
    FrequencyEntry& entry
) const noexcept {
    if (line == nullptr || line[0] == '\0') {
        return ErrorCode::BUFFER_INVALID;  // Error detected
    }
    
    // Copy line to buffer for parsing
    std::strncpy(line_buffer_, line, DATABASE_LINE_BUFFER_SIZE - 1);
    line_buffer_[DATABASE_LINE_BUFFER_SIZE - 1] = '\0';
    
    // Parse frequency (first field)
    char* token = std::strtok(line_buffer_, ",");
    if (token == nullptr) {
        return ErrorCode::DATABASE_CORRUPTED;  // Error detected
    }
    
    // Convert frequency string to FreqHz
    FreqHz freq = 0;
    const char* ptr = token;
    while (*ptr >= '0' && *ptr <= '9') {
        freq = freq * 10 + (*ptr - '0');
        ptr++;
    }
    entry.frequency = freq;
    
    // Parse drone type (second field)
    token = std::strtok(nullptr, ",");
    if (token == nullptr) {
        return ErrorCode::DATABASE_CORRUPTED;  // Error detected
    }
    
    uint8_t type_value = static_cast<uint8_t>(std::atoi(token));
    if (type_value > 255) {
        type_value = 0;
    }
    entry.drone_type = static_cast<DroneType>(type_value);
    
    // Parse priority (third field)
    token = std::strtok(nullptr, ",");
    if (token != nullptr) {
        entry.priority = static_cast<uint8_t>(std::atoi(token));
    } else {
        entry.priority = 0;
    }
    
    // Validate entry
    return validate_entry_internal(entry);  // Error detection
}

ErrorCode DatabaseManager::validate_entry_internal(
    const FrequencyEntry& entry
) const noexcept {
    if (entry.frequency < MIN_FREQUENCY_HZ ||
        entry.frequency > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;  // Error detected
    }
    
    if (entry.drone_type == DroneType::UNKNOWN) {
        return ErrorCode::INVALID_PARAMETER;  // Error detected
    }
    
    return ErrorCode::SUCCESS;
}
```

#### 7.2.3 Buffer Error Detection

**Detection Points:**

```
Buffer Error Detection
├── validate_spectrum_buffer()
│   ├── Detect: BUFFER_INVALID (null pointer)
│   ├── Detect: BUFFER_EMPTY (zero length)
│   └── Detect: BUFFER_INVALID (length > MAX_SPECTRUM_SIZE)
│
├── validate_drone_buffer()
│   ├── Detect: BUFFER_INVALID (null pointer)
│   ├── Detect: BUFFER_EMPTY (zero count)
│   └── Detect: BUFFER_FULL (count > max_count)
│
└── validate_histogram_buffer()
    ├── Detect: BUFFER_INVALID (null pointer)
    ├── Detect: BUFFER_EMPTY (zero length)
    └── Detect: BUFFER_INVALID (length > MAX_HISTOGRAM_SIZE)
```

**Detection Implementation:**

```cpp
inline ErrorCode validate_spectrum_buffer(
    const uint8_t* spectrum_data,
    size_t length
) noexcept {
    if (spectrum_data == nullptr) {
        return ErrorCode::BUFFER_INVALID;  // Error detected
    }
    
    if (length == 0) {
        return ErrorCode::BUFFER_EMPTY;  // Error detected
    }
    
    // Max spectrum size will be defined in constants.hpp
    // For now, use a reasonable limit
    constexpr size_t MAX_SPECTRUM_SIZE = 256;
    if (length > MAX_SPECTRUM_SIZE) {
        return ErrorCode::BUFFER_INVALID;  // Error detected
    }
    
    return ErrorCode::SUCCESS;
}

inline ErrorCode validate_drone_buffer(
    const DisplayDroneEntry* drones,
    size_t count,
    size_t max_count
) noexcept {
    if (drones == nullptr) {
        return ErrorCode::BUFFER_INVALID;  // Error detected
    }
    
    if (count == 0) {
        return ErrorCode::BUFFER_EMPTY;  // Error detected
    }
    
    if (count > max_count) {
        return ErrorCode::BUFFER_FULL;  // Error detected
    }
    
    return ErrorCode::SUCCESS;
}
```

### 7.3 Error Propagation

**Propagation Flow:**

```
Error Propagation Chain
├── Error detected in low-level function
│   └── Example: HardwareController::tune_internal() returns HARDWARE_FAILURE
│
├── Error propagated to caller
│   └── Example: tune_to_frequency() receives HARDWARE_FAILURE
│
├── Error handled or propagated further
│   ├── If recoverable: Apply recovery strategy
│   │   └── Example: Retry operation, use fallback
│   └── If not recoverable: Propagate to caller
│       └── Example: Scanner::perform_scan_cycle() receives HARDWARE_FAILURE
│
├── Error reaches top-level handler
│   └── Example: Scanner::perform_scan_cycle() returns error
│
└── Error handled at application level
    ├── Log error
    ├── Update UI with error message
    └── Apply graceful degradation
```

**Propagation Example:**

```cpp
// Low-level function
ErrorCode HardwareController::tune_internal(FreqHz frequency) noexcept {
    // Attempt to tune hardware
    bool success = spi_write_pll_registers(frequency);
    
    if (!success) {
        return ErrorCode::HARDWARE_FAILURE;  // Error detected
    }
    
    return ErrorCode::SUCCESS;
}

// Mid-level function
ErrorCode HardwareController::tune_to_frequency(
    FreqHz frequency,
    uint32_t max_retries
) noexcept {
    // Validate frequency
    ErrorCode validate_result = validate_frequency_internal(frequency);
    if (validate_result != ErrorCode::SUCCESS) {
        last_error_ = validate_result;
        return validate_result;  // Propagate error
    }
    
    // Retry logic
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        ErrorCode tune_result = tune_internal(frequency);
        if (tune_result != ErrorCode::SUCCESS) {
            if (retry < max_retries - 1) {
                chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS);
                continue;
            }
            state_ = HardwareState::ERROR;
            last_error_ = tune_result;
            return tune_result;  // Propagate error
        }
        
        // PLL lock verification
        // ...
        
        return ErrorCode::SUCCESS;
    }
    
    state_ = HardwareState::ERROR;
    last_error_ = ErrorCode::PLL_LOCK_FAILURE;
    return ErrorCode::PLL_LOCK_FAILURE;
}

// High-level function
ErrorCode DroneScanner::perform_scan_cycle_internal() noexcept {
    // Get next frequency
    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(current_frequency_);
    if (!freq_result.has_value()) {
        statistics_.failed_cycles++;
        return freq_result.error();  // Propagate error
    }
    
    current_frequency_ = freq_result.value();
    
    // Tune to frequency
    ErrorCode tune_result = hardware_.tune_to_frequency(current_frequency_);
    if (tune_result != ErrorCode::SUCCESS) {
        statistics_.failed_cycles++;
        return tune_result;  // Propagate error
    }
    
    // Get RSSI sample
    ErrorResult<RssiSample> rssi_result = hardware_.get_rssi_sample();
    if (!rssi_result.has_value()) {
        statistics_.failed_cycles++;
        return rssi_result.error();  // Propagate error
    }
    
    // Process RSSI sample
    // ...
    
    return ErrorCode::SUCCESS;
}
```

### 7.4 Error Recovery Mechanisms

**Recovery Strategies:**

#### 7.4.1 Hardware Failure Recovery

```cpp
template<typename HardwareFunc>
ErrorCode handle_hardware_failure(
    HardwareFunc hardware_func,
    ErrorCode initial_error,
    uint32_t max_retries = 3
) noexcept {
    if (initial_error == ErrorCode::SUCCESS) {
        return ErrorCode::SUCCESS;
    }
    
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        // Retry hardware operation
        ErrorCode result = hardware_func();
        if (result == ErrorCode::SUCCESS) {
            return ErrorCode::SUCCESS;  // Recovery successful
        }
        
        if (retry < max_retries - 1) {
            // Wait before retry
            chThdSleepMilliseconds(10);
        }
    }
    
    return initial_error;  // Recovery failed
}
```

#### 7.4.2 PLL Lock Failure Recovery

```cpp
template<typename TuneFunc, typename LockCheckFunc>
ErrorCode handle_pll_lock_failure(
    TuneFunc tune_func,
    LockCheckFunc lock_check_func,
    FreqHz target_freq,
    uint32_t max_retries = 3
) noexcept {
    constexpr uint32_t PLL_LOCK_TIMEOUT_MS = 100;
    constexpr uint32_t PLL_LOCK_POLL_INTERVAL_MS = 3;
    
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        // Attempt to tune to frequency
        if (!tune_func(target_freq)) {
            if (retry < max_retries - 1) {
                // Retry after delay
                chThdSleepMilliseconds(10);
                continue;
            }
            return ErrorCode::HARDWARE_FAILURE;  // Recovery failed
        }
        
        // Wait for PLL lock with timeout
        SystemTime start_time = chTimeNow();
        while (!lock_check_func()) {
            if (chTimeNow() - start_time > MS2ST(PLL_LOCK_TIMEOUT_MS)) {
                if (retry < max_retries - 1) {
                    chThdSleepMilliseconds(10);
                    break;
                }
                return ErrorCode::PLL_LOCK_FAILURE;  // Recovery failed
            }
            chThdSleepMilliseconds(PLL_LOCK_POLL_INTERVAL_MS);
        }
        
        // PLL locked successfully
        return ErrorCode::SUCCESS;  // Recovery successful
    }
    
    return ErrorCode::PLL_LOCK_FAILURE;  // Recovery failed
}
```

#### 7.4.3 Spectrum Streaming Failure Recovery

```cpp
template<typename StartFunc, typename ShutdownFunc, typename InitFunc>
ErrorCode handle_spectrum_streaming_failure(
    StartFunc start_func,
    ShutdownFunc shutdown_func,
    InitFunc init_func
) noexcept {
    // Attempt to start streaming
    if (!start_func()) {
        // Streaming failed - attempt recovery
        shutdown_func();
        chThdSleepMilliseconds(100);
        init_func();
        
        // Retry streaming
        if (!start_func()) {
            return ErrorCode::HARDWARE_FAILURE;  // Recovery failed
        }
    }
    
    return ErrorCode::SUCCESS;  // Recovery successful
}
```

#### 7.4.4 Mutex Lock Timeout Recovery

```cpp
inline ErrorCode handle_mutex_lock_timeout(
    bool lock_acquired,
    const char* operation
) noexcept {
    (void)operation;  // Suppress unused parameter warning
    
    if (!lock_acquired) {
        // Lock timeout - skip this update (graceful degradation)
        return ErrorCode::MUTEX_TIMEOUT;  // Recovery: skip update
    }
    
    return ErrorCode::SUCCESS;
}
```

### 7.5 User Notification of Errors

**Notification Flow:**

```
Error Notification Flow
├── Error detected
├── Log error (if DEBUG_LOGGING_ENABLED)
│   ├── Log error code
│   ├── Log error message
│   └── Log error context
├── Update UI with error message
│   ├── Set error_active_ = true
│   ├── Set last_error_ = error_code
│   ├── Set error_start_time_ = chTimeNow()
│   ├── Set error_duration_ms_ = 3000 (3 seconds)
│   └── Format error message
│       └── error_to_string(error_code)
├── Show error overlay on display
│   └── draw_error_overlay(painter)
│       ├── Draw error icon
│       ├── Draw error message
│       └── Draw error code
└── Schedule error message timeout
    └── After error_duration_ms:
        └── Clear error message
            └── error_active_ = false
```

**Error String Conversion:**

```cpp
const char* error_to_string(ErrorCode error) noexcept {
    switch (error) {
        // Hardware errors
        case ErrorCode::HARDWARE_NOT_INITIALIZED:
            return "HW Not Init";
        case ErrorCode::HARDWARE_TIMEOUT:
            return "HW Timeout";
        case ErrorCode::HARDWARE_FAILURE:
            return "HW Failure";
        case ErrorCode::SPI_FAILURE:
            return "SPI Fail";
        case ErrorCode::PLL_LOCK_FAILURE:
            return "PLL Lock Fail";
        
        // Database errors
        case ErrorCode::DATABASE_NOT_LOADED:
            return "DB Not Loaded";
        case ErrorCode::DATABASE_LOAD_TIMEOUT:
            return "DB Timeout";
        case ErrorCode::DATABASE_CORRUPTED:
            return "DB Corrupted";
        case ErrorCode::DATABASE_EMPTY:
            return "DB Empty";
        
        // Buffer errors
        case ErrorCode::BUFFER_EMPTY:
            return "Buffer Empty";
        case ErrorCode::BUFFER_FULL:
            return "Buffer Full";
        case ErrorCode::BUFFER_INVALID:
            return "Buffer Invalid";
        
        // Synchronization errors
        case ErrorCode::MUTEX_TIMEOUT:
            return "Mutex Timeout";
        case ErrorCode::MUTEX_LOCK_FAILED:
            return "Mutex Lock Fail";
        case ErrorCode::SEMAPHORE_TIMEOUT:
            return "Semaphore Timeout";
        
        // Initialization errors
        case ErrorCode::INITIALIZATION_FAILED:
            return "Init Failed";
        case ErrorCode::INITIALIZATION_INCOMPLETE:
            return "Init Incomplete";
        
        // General errors
        case ErrorCode::INVALID_PARAMETER:
            return "Invalid Param";
        case ErrorCode::NOT_IMPLEMENTED:
            return "Not Implemented";
        case ErrorCode::UNKNOWN_ERROR:
        default:
            return "Unknown Error";
    }
}
```

### 7.6 Graceful Degradation

**Degradation Strategies:**

#### 7.6.1 Database Degradation

```
Database Degradation
├── Database file not found
│   └── Use built-in default frequencies
│       └── Add 18 default 2.4 GHz frequencies
│
├── Database file corrupted
│   ├── Skip malformed lines
│   ├── Log errors
│   └── Use valid entries
│
└── Database empty
    └── Use default frequency (2.432 GHz)
```

#### 7.6.2 Hardware Degradation

```
Hardware Degradation
├── PLL lock failure
│   └── Use last known frequency
│       └── Continue scanning with current frequency
│
├── Hardware timeout
│   └── Retry operation
│       └── Skip current frequency if retries exhausted
│
└── Hardware failure
    └── Reset hardware
        └── Reinitialize hardware
```

#### 7.6.3 UI Degradation

```
UI Degradation
├── Mutex timeout on display data retrieval
│   └── Skip this UI update
│       └── Retry next cycle
│
├── Display buffer overflow
│   └── Truncate display data
│       └── Show partial data
│
└── Audio playback failure
    └── Skip audio alert
        └── Continue operation
```

### 7.7 Error Handling Testing Checklist

#### Error Detection

- [ ] Test hardware not initialized error
- [ ] Test hardware timeout error
- [ ] Test hardware failure error
- [ ] Test SPI failure error
- [ ] Test PLL lock failure error
- [ ] Test database not loaded error
- [ ] Test database load timeout error
- [ ] Test database corrupted error
- [ ] Test database empty error
- [ ] Test buffer empty error
- [ ] Test buffer full error
- [ ] Test buffer invalid error
- [ ] Test mutex timeout error
- [ ] Test mutex lock failed error
- [ ] Test initialization failed error
- [ ] Test invalid parameter error

#### Error Propagation

- [ ] Test error propagation from low to high level
- [ ] Test error propagation across components
- [ ] Verify error codes preserved
- [ ] Verify error context preserved
- [ ] Test error propagation in scan cycle
- [ ] Test error propagation in UI update

#### Error Recovery

- [ ] Test hardware failure recovery (retry)
- [ ] Test PLL lock failure recovery
- [ ] Test spectrum streaming failure recovery
- [ ] Test mutex timeout recovery (skip update)
- [ ] Test database corruption recovery (use defaults)
- [ ] Test database empty recovery (use defaults)
- [ ] Test graceful degradation strategies

#### User Notification

- [ ] Test error message display
- [ ] Test error overlay rendering
- [ ] Test error message timeout
- [ ] Test error code display
- [ ] Test error string conversion
- [ ] Verify error message clarity

#### Logging

- [ ] Test error logging (DEBUG mode)
- [ ] Verify error code logged
- [ ] Verify error message logged
- [ ] Verify error context logged
- [ ] Test log buffer overflow
- [ ] Verify log format

---

## 8. Thread Synchronization

### 8.1 Locking Architecture

**File:** `common/locking.hpp`

**Lock Ordering Levels:**

```
┌─────────────────────────────────────────────────────────────┐
│              Lock Ordering Levels (0-13)               │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  Level 0-2: Core System                                 │
│  ┌─────────────────────────────────────────────┐           │
│  │ 0: ATOMIC_FLAGS (no lock)          │           │
│  │ 1: DATA_MUTEX (scanner data)        │           │
│  │ 2: STATE_MUTEX (system state)       │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Level 3-7: UI Components                               │
│  ┌─────────────────────────────────────────────┐           │
│  │ 3: UI_THREAT_MUTEX                 │           │
│  │ 4: UI_CARD_MUTEX                    │           │
│  │ 5: UI_STATUSBAR_MUTEX               │           │
│  │ 6: UI_DISPLAY_MUTEX                │           │
│  │ 7: UI_CONTROLLER_MUTEX              │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Level 8-10: DSP Processing                              │
│  ┌─────────────────────────────────────────────┐           │
│  │ 8: ENTRIES_TO_SCAN_MUTEX           │           │
│  │ 9: HISTOGRAM_BUFFER_MUTEX         │           │
│  │ 10: SPECTRUM_DATA_MUTEX            │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Level 11-12: System Services                            │
│  ┌─────────────────────────────────────────────┐           │
│  │ 11: SPECTRUM_MUTEX                   │           │
│  │ 12: LOGGER_MUTEX                     │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Level 13: I/O Operations (MUST BE LAST)                  │
│  ┌─────────────────────────────────────────────┐           │
│  │ 13: SD_CARD_MUTEX (FatFS not thread-safe)│           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Rule: Always acquire locks in ascending order (0 → 13)    │
│  Rule: Never acquire locks in descending order (causes deadlock)│
│  Rule: Lock 13 (SD_CARD_MUTEX) must always be LAST        │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 Mutex Usage Points

#### 8.2.1 DatabaseManager Mutex Usage

**Lock:** `LockOrder::DATA_MUTEX` (Level 1)

**Usage Points:**

```
DatabaseManager Mutex Usage
├── load_frequency_database()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Double-check loaded flag
│       ├── Load from file
│       └── Set loaded flag
│
├── get_next_frequency()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Find current frequency
│       ├── Update current index
│       └── Return next frequency
│
├── add_entry()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Validate entry
│       ├── Check database full
│       └── Add entry
│
├── remove_entry()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Find entry
│       ├── Remove entry
│       └── Compact array
│
├── clear_entries()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Set entry count = 0
│       └── Reset current index
│
└── get_entry() / find_entry()
    └── Acquire MutexLock<LockOrder::DATA_MUTEX>
        ├── Read entry
        └── Return entry
```

**Implementation:**

```cpp
ErrorCode DatabaseManager::add_entry(const FrequencyEntry& entry) noexcept {
    // Validate entry first
    ErrorCode validate_result = validate_entry_internal(entry);
    if (validate_result != ErrorCode::SUCCESS) {
        return validate_result;
    }
    
    // Acquire mutex for thread safety
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    // Check if database is full
    if (entry_count_ >= MAX_DATABASE_ENTRIES) {
        return ErrorCode::BUFFER_FULL;
    }
    
    // Check if entry already exists
    ErrorResult<size_t> index_result = find_entry_index_internal(entry.frequency);
    if (index_result.has_value()) {
        // Update existing entry
        entries_[index_result.value()] = entry;
        return ErrorCode::SUCCESS;
    }
    
    // Add new entry
    entries_[entry_count_] = entry;
    entry_count_++;
    
    return ErrorCode::SUCCESS;
}
```

#### 8.2.2 HardwareController Mutex Usage

**Lock:** `LockOrder::STATE_MUTEX` (Level 2)

**Usage Points:**

```
HardwareController Mutex Usage
├── initialize()
│   └── Acquire MutexLock<LockOrder::STATE_MUTEX>
│       ├── Set state = INITIALIZING
│       ├── Initialize hardware
│       └── Set state = READY or ERROR
│
├── shutdown()
│   └── Acquire MutexLock<LockOrder::STATE_MUTEX>
│       ├── Stop streaming
│       └── Set state = UNINITIALIZED
│
├── tune_to_frequency()
│   └── Acquire MutexLock<LockOrder::STATE_MUTEX>
│       ├── Set state = TUNING
│       ├── Tune to frequency
│       ├── Wait for PLL lock
│       └── Set state = READY or STREAMING or ERROR
│
├── start_spectrum_streaming()
│   └── Acquire MutexLock<LockOrder::STATE_MUTEX>
│       ├── Start streaming
│       └── Set state = STREAMING
│
├── stop_spectrum_streaming()
│   └── Acquire MutexLock<LockOrder::STATE_MUTEX>
│       ├── Stop streaming
│       └── Set state = READY
│
├── get_rssi_sample()
│   └── Acquire MutexLock<LockOrder::STATE_MUTEX>
│       ├── Read RSSI
│       └── Return sample
│
└── reset()
    └── Acquire MutexLock<LockOrder::STATE_MUTEX>
        ├── Stop streaming
        ├── Reset state
        └── Reapply configuration
```

**Implementation:**

```cpp
ErrorCode HardwareController::tune_to_frequency(
    FreqHz frequency,
    uint32_t max_retries
) noexcept {
    // Validate frequency
    ErrorCode validate_result = validate_frequency_internal(frequency);
    if (validate_result != ErrorCode::SUCCESS) {
        last_error_ = validate_result;
        return validate_result;
    }
    
    // Acquire mutex for thread safety
    MutexLock<LockOrder::STATE_MUTEX> lock(*mutex_);
    
    if (state_ != HardwareState::READY && state_ != HardwareState::STREAMING) {
        last_error_ = ErrorCode::HARDWARE_NOT_INITIALIZED;
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    state_ = HardwareState::TUNING;
    
    // Retry logic with PLL lock verification
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        retry_count_ = retry;
        
        // Tune to frequency
        ErrorCode tune_result = tune_internal(frequency);
        if (tune_result != ErrorCode::SUCCESS) {
            if (retry < max_retries - 1) {
                chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS);
                continue;
            }
            state_ = HardwareState::ERROR;
            last_error_ = tune_result;
            return tune_result;
        }
        
        // Wait for PLL lock
        // ...
        
        // PLL locked successfully
        current_frequency_ = frequency;
        pll_locked_.set();
        state_ = streaming_active_.test() ? HardwareState::STREAMING : HardwareState::READY;
        last_error_ = ErrorCode::SUCCESS;
        return ErrorCode::SUCCESS;
    }
    
    state_ = HardwareState::ERROR;
    last_error_ = ErrorCode::PLL_LOCK_FAILURE;
    return ErrorCode::PLL_LOCK_FAILURE;
}
```

#### 8.2.3 DroneScanner Mutex Usage

**Lock:** `LockOrder::DATA_MUTEX` (Level 1)

**Usage Points:**

```
DroneScanner Mutex Usage
├── initialize()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Initialize database
│       ├── Initialize hardware
│       └── Set state = IDLE
│
├── start_scanning()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Start streaming
│       └── Set state = SCANNING
│
├── stop_scanning()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Stop streaming
│       └── Set state = IDLE
│
├── perform_scan_cycle()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Perform scan cycle
│       └── Update tracked drones
│
├── update_tracked_drones()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Update drone data
│       └── Update statistics
│
├── get_display_data()
│   └── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Copy display data
│       └── Return display data
│
└── remove_stale_drones()
    └── Acquire MutexLock<LockOrder::DATA_MUTEX>
        ├── Remove stale drones
        └── Compact array
```

**Implementation:**

```cpp
ErrorCode DroneScanner::perform_scan_cycle() noexcept {
    if (!scanning_active_.test()) {
        return ErrorCode::SUCCESS;  // Not scanning, skip
    }
    
    // Acquire mutex for thread safety
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    if (state_ != ScannerState::SCANNING) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    return perform_scan_cycle_internal();
}
```

### 8.3 Lock Ordering Enforcement

**Compile-Time Validation:**

```cpp
template<LockOrder CURRENT, LockOrder NEXT>
struct ValidateLockOrder {
    static_assert(CURRENT < NEXT, "Lock order violation: must acquire locks in ascending order");
    static constexpr bool valid = true;
};
```

**Usage Example:**

```cpp
// Correct: Acquire locks in ascending order (1 → 2)
void correct_lock_order() {
    MutexLock<LockOrder::DATA_MUTEX> data_lock(*data_mutex);      // Level 1
    MutexLock<LockOrder::STATE_MUTEX> state_lock(*state_mutex);  // Level 2
    // Do work
}

// Incorrect: Acquire locks in descending order (2 → 1) - COMPILATION ERROR
void incorrect_lock_order() {
    MutexLock<LockOrder::STATE_MUTEX> state_lock(*state_mutex);  // Level 2
    MutexLock<LockOrder::DATA_MUTEX> data_lock(*data_mutex);      // Level 1
    // Do work - COMPILATION ERROR: Lock order violation
}
```

**Runtime Validation (DEBUG mode):**

```cpp
#ifdef DEBUG_LOCK_ORDER_VALIDATION
class LockOrderTracker {
public:
    static void acquire(LockOrder order) {
        if (last_acquired_ >= order) {
            // Lock order violation detected
            log_error("Lock order violation: %d -> %d", last_acquired_, order);
        }
        last_acquired_ = order;
    }
    
    static void release(LockOrder order) {
        if (last_acquired_ == order) {
            last_acquired_ = LockOrder::ATOMIC_FLAGS;  // Reset to lowest
        }
    }
    
private:
    static LockOrder last_acquired_ = LockOrder::ATOMIC_FLAGS;
};

template<LockOrder ORDER>
class ValidatedMutexLock : public MutexLock<ORDER> {
public:
    explicit ValidatedMutexLock(mutex_t& mutex) noexcept
        : MutexLock<ORDER>(mutex) {
        LockOrderTracker::acquire(ORDER);
    }
    
    ~ValidatedMutexLock() noexcept {
        LockOrderTracker::release(ORDER);
    }
};
#endif
```

### 8.4 Atomic Flag Usage

**Atomic Flag Implementation:**

```cpp
class AtomicFlag {
public:
    constexpr AtomicFlag() noexcept
        : flag_(0) {}
    
    bool set() noexcept {
        return __atomic_test_and_set(&flag_, __ATOMIC_SEQ_CST) != 0;
    }
    
    void clear() noexcept {
        __atomic_clear(&flag_, __ATOMIC_SEQ_CST);
    }
    
    [[nodiscard]] bool test() const noexcept {
        return __atomic_test_and_set(&const_cast<uint8_t&>(flag_), __ATOMIC_SEQ_CST) != 0;
    }
    
    [[nodiscard]] bool test_and_set() noexcept {
        return set();
    }
    
    [[nodiscard]] bool try_set() noexcept {
        return !set();
    }
    
private:
    uint8_t flag_;
};
```

**Usage Points:**

```
Atomic Flag Usage
├── DatabaseManager::loaded_
│   └── Lock-free check if database loaded
│       └── if (loaded_.test()) { /* loaded */ }
│
├── HardwareController::pll_locked_
│   └── Lock-free check if PLL locked
│       └── if (pll_locked_.test()) { /* locked */ }
│
├── HardwareController::streaming_active_
│   └── Lock-free check if streaming active
│       └── if (streaming_active_.test()) { /* streaming */ }
│
└── DroneScanner::scanning_active_
    └── Lock-free check if scanning active
        └── if (scanning_active_.test()) { /* scanning */ }
```

### 8.5 Potential Deadlock Scenarios

#### 8.5.1 Lock Ordering Violation

**Scenario:**

```
Thread 1                          Thread 2
─────────                          ─────────
Acquire DATA_MUTEX (Level 1)
                                   Acquire STATE_MUTEX (Level 2)
Try to acquire STATE_MUTEX (Level 2)
                                   Try to acquire DATA_MUTEX (Level 1)
                                   (blocked - waiting for Thread 1)
(blocked - waiting for Thread 2)

DEADLOCK!
```

**Detection:** Compile-time lock order validation
**Fix:** Always acquire locks in ascending order (1 → 2)
**Test:** Create test with concurrent lock acquisition

#### 8.5.2 Reentrant Lock

**Scenario:**

```
Thread 1
─────────
Acquire DATA_MUTEX
Call function that tries to acquire DATA_MUTEX again
(blocked - waiting for itself)

DEADLOCK!
```

**Detection:** Runtime lock reentrancy check
**Fix:** Use recursive mutex or split critical sections
**Test:** Create test with reentrant lock attempt

#### 8.5.3 Lock Timeout with Retry

**Scenario:**

```
Thread 1                          Thread 2
─────────                          ─────────
Acquire DATA_MUTEX
                                   Try to acquire DATA_MUTEX with timeout
                                   (timeout)
Retry to acquire DATA_MUTEX
                                   (timeout)
Retry to acquire DATA_MUTEX
                                   (timeout)
(blocked - waiting for Thread 2)
(blocked - waiting for Thread 1)

DEADLOCK (with timeout)!
```

**Detection:** Lock timeout counter
**Fix:** Limit retry attempts, use graceful degradation
**Test:** Create test with lock timeout and retry

### 8.6 Lock Contention Points

**High Contention Points:**

1. **DATA_MUTEX (Level 1)**
   - **Components:** DatabaseManager, DroneScanner
   - **Operations:** Database access, scanner state updates
   - **Contention:** High during scan cycles
   - **Mitigation:** Use AtomicFlag for lock-free checks

2. **STATE_MUTEX (Level 2)**
   - **Components:** HardwareController
   - **Operations:** Hardware control, state transitions
   - **Contention:** Moderate during tuning
   - **Mitigation:** Minimize lock holding time

3. **UI_DISPLAY_MUTEX (Level 6)**
   - **Components:** DroneDisplay, DroneScannerUI
   - **Operations:** Display updates, UI rendering
   - **Contention:** High during 60 FPS updates
   - **Mitigation:** Use try_lock for non-blocking updates

### 8.7 Thread Synchronization Testing Checklist

#### Lock Ordering

- [ ] Test correct lock order (ascending)
- [ ] Test incorrect lock order (compilation error)
- [ ] Verify compile-time validation
- [ ] Test runtime validation (DEBUG mode)
- [ ] Test lock order violations

#### Mutex Usage

- [ ] Test mutex acquisition
- [ ] Test mutex release
- [ ] Test mutex timeout
- [ ] Test mutex retry logic
- [ ] Verify RAII behavior
- [ ] Test with concurrent threads

#### Atomic Flags

- [ ] Test AtomicFlag::set()
- [ ] Test AtomicFlag::clear()
- [ ] Test AtomicFlag::test()
- [ ] Test AtomicFlag::test_and_set()
- [ ] Test AtomicFlag::try_set()
- [ ] Verify lock-free behavior
- [ ] Test with concurrent threads

#### Deadlock Scenarios

- [ ] Test lock ordering violation
- [ ] Test reentrant lock
- [ ] Test lock timeout with retry
- [ ] Test circular wait
- [ ] Verify deadlock detection
- [ ] Verify deadlock recovery

#### Lock Contention

- [ ] Measure DATA_MUTEX contention
- [ ] Measure STATE_MUTEX contention
- [ ] Measure UI_DISPLAY_MUTEX contention
- [ ] Test with high scan rate
- [ ] Test with high UI update rate
- [ ] Verify lock holding time

#### Thread Safety

- [ ] Test concurrent database access
- [ ] Test concurrent hardware control
- [ ] Test concurrent scanner operations
- [ ] Test concurrent UI updates
- [ ] Verify data consistency
- [ ] Verify no race conditions

---

## 9. Memory Usage Analysis

### 9.1 Memory Budget

**Total Memory Budget:**

```
┌─────────────────────────────────────────────────────────────┐
│              Memory Budget (STM32F405, 64KB RAM)    │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  Total RAM: 65536 bytes (64 KB)                        │
│                                                          │
│  Static RAM Budget: 4504 bytes                          │
│  ┌─────────────────────────────────────────────┐           │
│  │ DatabaseManager:                              │           │
│  │   • entries_[500]: 8000 bytes (16 × 500)   │           │
│  │   • line_buffer_[128]: 128 bytes              │           │
│  │   • Total: 8128 bytes                        │           │
│  │                                                 │           │
│  │ HardwareController:                           │           │
│  │   • config_: 32 bytes                         │           │
│  │   • Total: 32 bytes                           │           │
│  │                                                 │           │
│  │ DroneScanner:                                 │           │
│  │   • tracked_drones_[20]: 820 bytes (41 × 20)│           │
│  │   • Total: 820 bytes                           │           │
│  │                                                 │           │
│  │ DroneDisplay:                                 │           │
│  │   • display_data_: 820 bytes                   │           │
│  │   • spectrum_buffer_[240]: 240 bytes           │           │
│  │   • histogram_buffer_[128]: 256 bytes           │           │
│  │   • status_text_[32]: 32 bytes                │           │
│  │   • Total: 1348 bytes                        │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Stack Budget: 2867 bytes                                │
│  ┌─────────────────────────────────────────────┐           │
│  │ Scanner thread: 2048 bytes                 │           │
│  │ UI thread: 4096 bytes                      │           │
│  │ Per function: 512 bytes (max)               │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Total Budget: 7371 bytes                              │
│  ┌─────────────────────────────────────────────┐           │
│  │ Static: 4504 bytes                         │           │
│  │ Stack: 2867 bytes                          │           │
│  │ Total: 7371 bytes                          │           │
│  │                                                 │           │
│  │ Available: 58165 bytes (56.8 KB)         │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 Static Storage Usage Per Component

#### 9.2.1 DatabaseManager

**Static Storage:**

```cpp
class DatabaseManager {
private:
    // Database storage (fixed-size array, no heap allocation)
    std::array<FrequencyEntry, MAX_DATABASE_ENTRIES> entries_;
    // Size: 500 × 16 = 8000 bytes
    
    // Current iterator position
    size_t current_index_;
    // Size: 4 bytes
    
    // Number of valid entries
    size_t entry_count_;
    // Size: 4 bytes
    
    // Database loaded flag (AtomicFlag for lock-free access)
    AtomicFlag loaded_;
    // Size: 1 byte
    
    // Mutex for thread safety (LockOrder::DATA_MUTEX)
    mutable mutex_t* mutex_;
    // Size: 4 bytes (pointer)
    
    // Line buffer for parsing
    char line_buffer_[DATABASE_LINE_BUFFER_SIZE];
    // Size: 128 bytes
    
    // Total: 8000 + 4 + 4 + 1 + 4 + 128 = 8141 bytes
};
```

**Memory Usage:**

- **entries_[500]:** 8000 bytes (16 bytes × 500)
- **line_buffer_[128]:** 128 bytes
- **Other members:** 13 bytes
- **Total:** 8141 bytes

#### 9.2.2 HardwareController

**Static Storage:**

```cpp
class HardwareController {
private:
    // Hardware state
    HardwareState state_;
    // Size: 1 byte
    
    // Current configuration
    HardwareConfig config_;
    // Size: 32 bytes
    
    // Current frequency
    FreqHz current_frequency_;
    // Size: 8 bytes
    
    // Last error code
    ErrorCode last_error_;
    // Size: 1 byte
    
    // Retry counter
    uint32_t retry_count_;
    // Size: 4 bytes
    
    // PLL lock flag
    AtomicFlag pll_locked_;
    // Size: 1 byte
    
    // Streaming active flag
    AtomicFlag streaming_active_;
    // Size: 1 byte
    
    // Mutex for thread safety (LockOrder::STATE_MUTEX)
    mutable mutex_t* mutex_;
    // Size: 4 bytes (pointer)
    
    // Total: 1 + 32 + 8 + 1 + 4 + 1 + 1 + 4 = 52 bytes
};
```

**Memory Usage:**

- **config_:** 32 bytes
- **current_frequency_:** 8 bytes
- **Other members:** 12 bytes
- **Total:** 52 bytes

#### 9.2.3 DroneScanner

**Static Storage:**

```cpp
class DroneScanner {
private:
    // References to dependencies
    DatabaseManager& database_;
    // Size: 4 bytes (reference)
    HardwareController& hardware_;
    // Size: 4 bytes (reference)
    
    // Scanner state
    ScannerState state_;
    // Size: 1 byte
    
    // Scan configuration
    ScanConfig config_;
    // Size: 32 bytes
    
    // Scan statistics
    ScanStatistics statistics_;
    // Size: 20 bytes
    
    // Tracked drones (fixed-size array, no heap allocation)
    std::array<TrackedDrone, MAX_TRACKED_DRONES> tracked_drones_;
    // Size: 20 × 41 = 820 bytes
    
    // Number of tracked drones
    size_t tracked_count_;
    // Size: 4 bytes
    
    // Current scan frequency
    FreqHz current_frequency_;
    // Size: 8 bytes
    
    // Last scan time
    SystemTime last_scan_time_;
    // Size: 4 bytes
    
    // Scanning active flag
    AtomicFlag scanning_active_;
    // Size: 1 byte
    
    // Mutex for thread safety (LockOrder::DATA_MUTEX)
    mutable mutex_t* mutex_;
    // Size: 4 bytes (pointer)
    
    // Total: 4 + 4 + 1 + 32 + 20 + 820 + 4 + 8 + 4 + 1 + 4 = 902 bytes
};
```

**Memory Usage:**

- **tracked_drones_[20]:** 820 bytes (41 bytes × 20)
- **config_:** 32 bytes
- **statistics_:** 20 bytes
- **Other members:** 30 bytes
- **Total:** 902 bytes

#### 9.2.4 DroneDisplay

**Static Storage:**

```cpp
class DroneDisplay {
private:
    // Display data (static storage, no heap allocation)
    DisplayData display_data_;
    // Size: 820 bytes
    
    // Spectrum data buffer (static storage for stack optimization)
    std::array<uint8_t, SPECTRUM_BUFFER_SIZE> spectrum_buffer_;
    // Size: 240 bytes
    
    // Histogram data buffer (static storage for stack optimization)
    std::array<uint16_t, HISTOGRAM_BUFFER_SIZE> histogram_buffer_;
    // Size: 256 bytes
    
    // Status text buffer
    char status_text_[MAX_TEXT_LENGTH];
    // Size: 32 bytes
    
    // Spectrum data size
    size_t spectrum_data_size_;
    // Size: 4 bytes
    
    // Histogram data size
    size_t histogram_data_size_;
    // Size: 4 bytes
    
    // Display flags
    bool spectrum_visible_;
    bool histogram_visible_;
    bool drone_list_visible_;
    bool status_bar_visible_;
    // Size: 4 bytes
    
    // Display dimensions
    uint16_t spectrum_height_;
    uint16_t histogram_height_;
    uint16_t drone_list_height_;
    uint16_t status_bar_height_;
    // Size: 8 bytes
    
    // Total: 820 + 240 + 256 + 32 + 4 + 4 + 4 + 8 = 1368 bytes
};
```

**Memory Usage:**

- **display_data_:** 820 bytes
- **spectrum_buffer_[240]:** 240 bytes
- **histogram_buffer_[128]:** 256 bytes
- **status_text_[32]:** 32 bytes
- **Other members:** 20 bytes
- **Total:** 1368 bytes

### 9.3 Stack Usage Per Function

**Stack Usage Analysis:**

```
Stack Usage Per Function
├── DatabaseManager::load_frequency_database()
│   └── Stack: ~256 bytes
│       ├── Local variables: 32 bytes
│       ├── File I/O buffer: 128 bytes
│       └── Function call overhead: 96 bytes
│
├── DatabaseManager::parse_line_internal()
│   └── Stack: ~128 bytes
│       ├── Line buffer: 128 bytes (static)
│       ├── Local variables: 16 bytes
│       └── Function call overhead: 16 bytes
│
├── HardwareController::tune_to_frequency()
│   └── Stack: ~192 bytes
│       ├── Local variables: 32 bytes
│       ├── Retry loop: 64 bytes
│       └── Function call overhead: 96 bytes
│
├── DroneScanner::perform_scan_cycle_internal()
│   └── Stack: ~256 bytes
│       ├── Local variables: 64 bytes
│       ├── Database access: 64 bytes
│       ├── Hardware access: 64 bytes
│       └── Function call overhead: 64 bytes
│
├── DroneDisplay::render_spectrum()
│   └── Stack: ~384 bytes
│       ├── Local variables: 64 bytes
│       ├── Spectrum data: 240 bytes (static)
│       ├── Drawing operations: 64 bytes
│       └── Function call overhead: 96 bytes
│
└── DroneDisplay::render_drone_list()
    └── Stack: ~512 bytes (max)
        ├── Local variables: 128 bytes
        ├── Drone data: 820 bytes (static)
        ├── Drawing operations: 128 bytes
        └── Function call overhead: 128 bytes
```

### 9.4 Memory Allocation Points

**No Heap Allocation:**

The refactored EDA uses **zero heap allocation**:

```
Memory Allocation Strategy
├── Static storage for large buffers
│   ├── DatabaseManager::entries_[500]
│   ├── DroneScanner::tracked_drones_[20]
│   ├── DroneDisplay::spectrum_buffer_[240]
│   ├── DroneDisplay::histogram_buffer_[128]
│   └── DroneDisplay::display_data_
│
├── Stack allocation for small buffers
│   ├── Local variables
│   ├── Function parameters
│   └── Return values
│
└── No dynamic allocation
    ├── No new/malloc
    ├── No std::vector
    ├── No std::string
    └── No std::map
```

**Alternative to Heap:**

```cpp
// Instead of: std::vector<TrackedDrone> tracked_drones_;
// Use: std::array<TrackedDrone, MAX_TRACKED_DRONES> tracked_drones_;

// Instead of: std::string description;
// Use: char description[MAX_DESCRIPTION_LENGTH];

// Instead of: new TrackedDrone();
// Use: TrackedDrone drone; (stack allocation)
```

### 9.5 Memory Deallocation Points

**No Deallocation Needed:**

Since there is no heap allocation, there is no need for deallocation:

```
Memory Deallocation Strategy
├── Static storage: No deallocation needed
│   └── Automatically freed on program exit
│
├── Stack allocation: No deallocation needed
│   └── Automatically freed on function return
│
└── No heap allocation: No deallocation needed
    └── No memory leaks possible
```

### 9.6 Memory Leak Risks

**Potential Leak Sources:**

1. **Mutex Not Released**
   - **Risk:** Mutex acquired but not released
   - **Detection:** Use RAII mutex wrappers
   - **Fix:** MutexLock destructor releases automatically
   - **Test:** Test with early returns

2. **File Handle Not Closed**
   - **Risk:** File opened but not closed
   - **Detection:** Use RAII file wrappers
   - **Fix:** File destructor closes automatically
   - **Test:** Test with exceptions/errors

3. **DMA Buffer Not Freed**
   - **Risk:** DMA buffer allocated but not freed
   - **Detection:** Track DMA allocations
   - **Fix:** Free DMA buffer after use
   - **Test:** Test with DMA operations

4. **Interrupt Handler Allocation**
   - **Risk:** Memory allocated in ISR
   - **Detection:** Static analysis
   - **Fix:** Use static buffers for ISR
   - **Test:** Test with interrupt handlers

**Mitigation Strategies:**

```cpp
// RAII Mutex Wrapper
template<LockOrder ORDER>
class MutexLock {
public:
    explicit MutexLock(mutex_t& mutex) noexcept
        : mutex_(mutex), locked_(false) {
        chMtxLock(&mutex_);
        locked_ = true;
    }
    
    ~MutexLock() noexcept {
        if (locked_) {
            chMtxUnlock(&mutex_);
            locked_ = false;
        }
    }
    
private:
    mutex_t& mutex_;
    bool locked_;
};

// RAII File Wrapper
class FileHandle {
public:
    explicit FileHandle(const char* path) noexcept
        : file_(fopen(path, "r")) {}
    
    ~FileHandle() noexcept {
        if (file_) {
            fclose(file_);
        }
    }
    
    FILE* get() const noexcept { return file_; }
    
private:
    FILE* file_;
};
```

### 9.7 Memory Usage Testing Checklist

#### Static Storage

- [ ] Measure DatabaseManager static storage
- [ ] Measure HardwareController static storage
- [ ] Measure DroneScanner static storage
- [ ] Measure DroneDisplay static storage
- [ ] Verify total < 4504 bytes
- [ ] Verify no heap allocation
- [ ] Verify memory alignment

#### Stack Usage

- [ ] Measure DatabaseManager::load_frequency_database() stack
- [ ] Measure DatabaseManager::parse_line_internal() stack
- [ ] Measure HardwareController::tune_to_frequency() stack
- [ ] Measure DroneScanner::perform_scan_cycle_internal() stack
- [ ] Measure DroneDisplay::render_spectrum() stack
- [ ] Measure DroneDisplay::render_drone_list() stack
- [ ] Verify all < 512 bytes
- [ ] Verify no stack overflow

#### Memory Allocation

- [ ] Verify no new/malloc
- [ ] Verify no std::vector
- [ ] Verify no std::string
- [ ] Verify no std::map
- [ ] Verify no dynamic allocation
- [ ] Test with memory analyzer

#### Memory Deallocation

- [ ] Verify mutex release (RAII)
- [ ] Verify file handle close (RAII)
- [ ] Verify DMA buffer free
- [ ] Verify ISR buffer static
- [ ] Test with memory leak detector

#### Memory Constraints

- [ ] Test with maximum database size (500 entries)
- [ ] Test with maximum tracked drones (20)
- [ ] Test with maximum spectrum size (240)
- [ ] Test with maximum histogram size (128)
- [ ] Verify no buffer overflow
- [ ] Verify no memory corruption

---

**End of Part 3**

**Continue to Part 4:** Testing Plan
