# Enhanced Drone Analyzer - Comprehensive Testing and Debugging Plan

**Part 1: Initialization, Database, and Hardware Flows**

---

## Document Overview

This document provides a comprehensive testing and debugging plan for the refactored Enhanced Drone Analyzer (EDA) application. The plan covers all application operation cycles from initialization to audio output, with detailed flow analysis, potential issues, testing checklists, and debugging instrumentation recommendations.

**Target Hardware:** STM32F405 (ARM Cortex-M4, 64KB RAM)
**Project:** Mayhem firmware for HackRF One / PortaPack
**Refactored Code Location:** `firmware/application/apps/enhanced_drone_analyzer/`
**Legacy Code Location:** `firmware/application/apps/enhanced_drone_analyzer/overengineering_LEGACY/`

---

## Table of Contents (Part 1)

1. [Initialization Flow Analysis](#1-initialization-flow-analysis)
2. [Database Integration with Freqman](#2-database-integration-with-freqman)
3. [Hardware Access Flow](#3-hardware-access-flow)

---

## 1. Initialization Flow Analysis

### 1.1 Initialization Architecture

The EDA uses a **3-phase initialization strategy** to minimize startup time and reduce memory pressure:

```
┌─────────────────────────────────────────────────────────────┐
│           EDA Initialization Sequence                      │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  Phase 1: CRITICAL (Immediate)                           │
│  ┌─────────────────────────────────────────────┐           │
│  │ • Mutex initialization                     │           │
│  │ • Atomic flags initialization              │           │
│  │ • State variables reset                   │           │
│  └─────────────────────────────────────────────┘           │
│                          ↓                                 │
│  Phase 2: CORE SERVICES (Deferred)                         │
│  ┌─────────────────────────────────────────────┐           │
│  │ • HardwareController::initialize()         │           │
│  │   - SPI communication setup               │           │
│  │   - RF frontend configuration            │           │
│  │   - Default gain settings               │           │
│  │                                         │           │
│  │ • DatabaseManager::load_frequency_database()│           │
│  │   - Lazy loading (first access only)     │           │
│  │   - Parse freqman format                │           │
│  │   - Validate entries                    │           │
│  └─────────────────────────────────────────────┘           │
│                          ↓                                 │
│  Phase 3: LAZY (On-Demand)                                │
│  ┌─────────────────────────────────────────────┐           │
│  │ • DSP processors (SpectrumProcessor,        │           │
│  │   RSSIDetector, HistogramProcessor)       │           │
│  │ • UI components (DroneScannerUI,          │           │
│  │   DroneDisplay, DroneSettings)            │           │
│  │ • Scanner state machine                  │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Component Initialization Order

#### 1.2.1 DatabaseManager Initialization

**File:** `core/database.cpp`

**Initialization Flow:**

```
DatabaseManager Constructor
├── Initialize entries_ array (MAX_DATABASE_ENTRIES = 500)
├── Set current_index_ = 0
├── Set entry_count_ = 0
├── Clear loaded_ flag (AtomicFlag)
├── Initialize mutex_ (LockOrder::DATA_MUTEX)
└── Clear line_buffer_ (DATABASE_LINE_BUFFER_SIZE = 128)

↓ (Lazy initialization on first access)

load_frequency_database() [Called by Scanner::initialize()]
├── Check loaded_.test() → Return SUCCESS if true
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Double-check loaded_.test() → Return SUCCESS if true
├── Call load_from_file_internal()
│   ├── Open DATABASE_FILE_PATH = "/FREQMAN/DRONES.TXT"
│   ├── Read line by line
│   ├── Parse each line with parse_line_internal()
│   │   ├── Validate format: "FREQUENCY,TYPE,PRIORITY"
│   │   ├── Parse frequency (uint64_t)
│   │   ├── Parse drone type (uint8_t)
│   │   ├── Parse priority (uint8_t)
│   │   └── Validate entry with validate_entry_internal()
│   └── On error: Call add_default_frequencies_internal()
│       └── Add 18 default 2.4 GHz frequencies
├── Set loaded_.set() on success
└── Return ErrorCode::SUCCESS
```

**Potential Issues:**

1. **Race Condition in Lazy Loading**
   - **Issue:** Multiple threads may call `load_frequency_database()` simultaneously
   - **Detection:** Check if `loaded_.test()` returns true before mutex acquisition
   - **Fix:** Double-checked locking pattern is implemented (lines 60-70)
   - **Test:** Create concurrent access test with multiple threads

2. **File Not Found**
   - **Issue:** `/FREQMAN/DRONES.TXT` doesn't exist
   - **Detection:** `load_from_file_internal()` fails
   - **Fix:** Fallback to `add_default_frequencies_internal()` (lines 79-82)
   - **Test:** Test with missing file, verify default frequencies loaded

3. **Malformed Database File**
   - **Issue:** Invalid format in database file
   - **Detection:** `parse_line_internal()` returns `DATABASE_CORRUPTED`
   - **Fix:** Skip malformed lines, continue parsing
   - **Test:** Test with corrupted file, verify graceful degradation

4. **Buffer Overflow in Parsing**
   - **Issue:** Line longer than `DATABASE_LINE_BUFFER_SIZE`
   - **Detection:** `std::strncpy` truncates line
   - **Fix:** Buffer size check before parsing
   - **Test:** Test with extremely long lines

**Debug Instrumentation:**

```cpp
// Add to load_frequency_database()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("DatabaseManager: Loading database...");
    log_info("DatabaseManager: File path: %s", DATABASE_FILE_PATH);
}
#endif

// Add to load_from_file_internal()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("DatabaseManager: Loaded %zu entries", entry_count_);
    log_info("DatabaseManager: First freq: %llu Hz", entries_[0].frequency);
}
#endif

// Add to parse_line_internal()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("DatabaseManager: Parsed entry: %llu Hz, type=%u, prio=%u",
             entry.frequency, entry.drone_type, entry.priority);
}
#endif
```

#### 1.2.2 HardwareController Initialization

**File:** `core/hardware_controller.cpp`

**Initialization Flow:**

```
HardwareController Constructor
├── Set state_ = HardwareState::UNINITIALIZED
├── Initialize config_ with defaults
│   ├── center_frequency = DEFAULT_SCAN_FREQUENCY_HZ (2.432 GHz)
│   ├── sample_rate = 2000000 (2 MHz)
│   ├── gain = 20
│   ├── lna_gain = 8
│   └── vga_gain = 12
├── Set current_frequency_ = 0
├── Set last_error_ = ErrorCode::SUCCESS
├── Set retry_count_ = 0
├── Clear pll_locked_ flag (AtomicFlag)
├── Clear streaming_active_ flag (AtomicFlag)
└── Initialize mutex_ (LockOrder::STATE_MUTEX)

↓

initialize() [Called by Scanner::initialize()]
├── Acquire MutexLock<LockOrder::STATE_MUTEX>
├── Check state_ != UNINITIALIZED → Return INITIALIZATION_INCOMPLETE
├── Set state_ = HardwareState::INITIALIZING
├── Call initialize_internal()
│   ├── Initialize SPI communication with HackRF One
│   ├── Configure RF frontend
│   ├── Set default gain
│   ├── Enable RF path
│   └── Call apply_config_internal(config_)
│       ├── Set sample rate
│       ├── Set center frequency
│       └── Configure gain stages
├── On success:
│   ├── Set state_ = HardwareState::READY
│   ├── Set current_frequency_ = config_.center_frequency
│   └── Set last_error_ = ErrorCode::SUCCESS
├── On error:
│   ├── Set state_ = HardwareState::ERROR
│   └── Set last_error_ = error code
└── Return error code
```

**Potential Issues:**

1. **SPI Communication Failure**
   - **Issue:** HackRF One not responding on SPI
   - **Detection:** `initialize_internal()` returns `SPI_FAILURE`
   - **Fix:** Retry logic with exponential backoff
   - **Test:** Simulate SPI failure, verify retry behavior

2. **PLL Lock Timeout**
   - **Issue:** PLL fails to lock within timeout
   - **Detection:** `check_pll_lock_internal()` times out
   - **Fix:** Retry with different frequency
   - **Test:** Test with invalid frequency, verify timeout handling

3. **Gain Setting Failure**
   - **Issue:** Gain value out of range
   - **Detection:** `validate_gain_internal()` returns `INVALID_PARAMETER`
   - **Fix:** Clamp gain to valid range (0-40)
   - **Test:** Test with gain > 40, verify clamping

4. **State Machine Corruption**
   - **Issue:** State transitions violate expected sequence
   - **Detection:** State goes from UNINITIALIZED to ERROR without INITIALIZING
   - **Fix:** Add state transition validation
   - **Test:** Add state logging, verify transitions

**Debug Instrumentation:**

```cpp
// Add to initialize()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("HardwareController: Initializing...");
    log_info("HardwareController: Config - freq=%llu Hz, rate=%u Hz, gain=%u",
             config_.center_frequency, config_.sample_rate, config_.gain);
}
#endif

// Add to initialize_internal()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("HardwareController: SPI initialized");
    log_info("HardwareController: RF frontend configured");
    log_info("HardwareController: Gain set to %u", config_.gain);
}
#endif

// Add state transition logging
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("HardwareController: State %d -> %d", old_state, new_state);
}
#endif
```

#### 1.2.3 Scanner Initialization

**File:** `core/scanner.cpp`

**Initialization Flow:**

```
DroneScanner Constructor
├── Store references to database_ and hardware_
├── Set state_ = ScannerState::IDLE
├── Initialize config_ with defaults
│   ├── mode = DEFAULT_SCANNING_MODE (SEQUENTIAL)
│   ├── start_frequency = MIN_FREQUENCY_HZ (2.4 GHz)
│   ├── end_frequency = MAX_FREQUENCY_HZ (2.4835 GHz)
│   ├── scan_interval_ms = SCAN_CYCLE_INTERVAL_MS (100 ms)
│   ├── rssi_threshold_dbm = RSSI_DETECTION_THRESHOLD_DBM (-90 dBm)
│   └── stale_timeout_ms = DRONE_STALE_TIMEOUT_MS (5000 ms)
├── Initialize statistics_
│   ├── total_scan_cycles = 0
│   ├── successful_cycles = 0
│   ├── failed_cycles = 0
│   ├── drones_detected = 0
│   └── max_rssi_dbm = RSSI_NOISE_FLOOR_DBM (-100 dBm)
├── Initialize tracked_drones_ array (MAX_TRACKED_DRONES = 20)
├── Set tracked_count_ = 0
├── Set current_frequency_ = 0
├── Set last_scan_time_ = 0
├── Clear scanning_active_ flag (AtomicFlag)
└── Initialize mutex_ (LockOrder::DATA_MUTEX)

↓

initialize() [Called by application startup]
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Check state_ != IDLE → Return INITIALIZATION_INCOMPLETE
├── Call database_.load_frequency_database()
│   ├── On SUCCESS or DATABASE_EMPTY: Continue
│   └── On other errors: Return error
├── Call hardware_.initialize()
│   ├── On SUCCESS: Continue
│   └── On error: Return error
├── Get first frequency from database
│   └── Call database_.get_next_frequency(0)
│       ├── On success: Set current_frequency_
│       └── On error: Set current_frequency_ = DEFAULT_SCAN_FREQUENCY_HZ
├── Set state_ = ScannerState::IDLE
├── Reset statistics_.reset()
└── Return ErrorCode::SUCCESS
```

**Potential Issues:**

1. **Dependency Initialization Order**
   - **Issue:** Database not loaded before hardware initialization
   - **Detection:** `database_.load_frequency_database()` fails
   - **Fix:** Initialize database before hardware
   - **Test:** Test with corrupted database, verify error handling

2. **Frequency Database Empty**
   - **Issue:** No frequencies in database
   - **Detection:** `get_next_frequency(0)` returns `DATABASE_EMPTY`
   - **Fix:** Use default frequency (2.432 GHz)
   - **Test:** Test with empty database, verify default frequency used

3. **Tracked Drones Array Overflow**
   - **Issue:** More than 20 drones detected
   - **Detection:** `tracked_count_ >= MAX_TRACKED_DRONES`
   - **Fix:** Reject new drones when array full
   - **Test:** Simulate 20+ drones, verify overflow handling

4. **Statistics Corruption**
   - **Issue:** Statistics counters overflow
   - **Detection:** `total_scan_cycles` wraps around
   - **Fix:** Use uint32_t, handle overflow gracefully
   - **Test:** Run for extended period, verify no corruption

**Debug Instrumentation:**

```cpp
// Add to initialize()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("DroneScanner: Initializing...");
    log_info("DroneScanner: Config - mode=%u, start=%llu Hz, end=%llu Hz",
             config_.mode, config_.start_frequency, config_.end_frequency);
    log_info("DroneScanner: Threshold - %d dBm, timeout=%u ms",
             config_.rssi_threshold_dbm, config_.stale_timeout_ms);
}
#endif

// Add to get_next_frequency()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("DroneScanner: Next frequency: %llu Hz", current_frequency_);
}
#endif

// Add to add_tracked_drone_internal()
#ifdef DEBUG_LOGGING_ENABLED
if (DEBUG_LOGGING_ENABLED) {
    log_info("DroneScanner: Added drone - freq=%llu Hz, type=%u, threat=%u, rssi=%d dBm",
             frequency, type, threat, rssi);
    log_info("DroneScanner: Tracked count: %zu / %zu", tracked_count_, MAX_TRACKED_DRONES);
}
#endif
```

### 1.3 Initialization Flow Testing Checklist

#### Phase 1: Critical Initialization

- [ ] Verify mutex initialization doesn't deadlock
- [ ] Verify AtomicFlag operations are lock-free
- [ ] Verify state variables initialized to correct defaults
- [ ] Test with concurrent initialization attempts

#### Phase 2: Core Services Initialization

**DatabaseManager:**
- [ ] Test with valid database file
- [ ] Test with missing database file (use defaults)
- [ ] Test with corrupted database file (graceful degradation)
- [ ] Test with malformed entries (skip invalid lines)
- [ ] Test lazy loading (only load on first access)
- [ ] Test thread-safe lazy loading (double-checked locking)
- [ ] Verify database size limit (MAX_DATABASE_ENTRIES = 500)
- [ ] Verify line buffer overflow protection

**HardwareController:**
- [ ] Test successful initialization
- [ ] Test SPI communication failure
- [ ] Test PLL lock timeout
- [ ] Test gain setting out of range
- [ ] Test state machine transitions
- [ ] Verify retry logic (MAX_HARDWARE_RETRIES = 3)
- [ ] Verify default configuration values

**DroneScanner:**
- [ ] Test successful initialization
- [ ] Test database load failure
- [ ] Test hardware initialization failure
- [ ] Test empty database (use default frequency)
- [ ] Test tracked drones array overflow
- [ ] Verify statistics initialization
- [ ] Verify configuration defaults

#### Phase 3: Lazy Initialization

**DSP Processors:**
- [ ] Test SpectrumProcessor initialization
- [ ] Test RSSIDetector initialization
- [ ] Test HistogramProcessor initialization
- [ ] Verify no heap allocation
- [ ] Verify stack usage < 512 bytes

**UI Components:**
- [ ] Test DroneScannerUI initialization
- [ ] Test DroneDisplay initialization
- [ ] Test DroneSettings initialization
- [ ] Verify static buffer usage
- [ ] Verify display dimensions

### 1.4 Initialization Race Conditions

#### 1.4.1 Database Lazy Loading Race

**Scenario:** Two threads call `load_frequency_database()` simultaneously

**Race Condition:**
```
Thread 1                          Thread 2
─────────                          ─────────
Check loaded_.test() → false
                                   Check loaded_.test() → false
Acquire mutex
                                   Acquire mutex (blocked)
Double-check loaded_.test() → false
Call load_from_file_internal()
                                   (blocked)
Set loaded_.set()
                                   (blocked)
Release mutex
                                   Acquire mutex
                                   Double-check loaded_.test() → true
                                   Release mutex
                                   Return SUCCESS
```

**Detection:** Add logging to track concurrent access
**Fix:** Double-checked locking pattern already implemented
**Test:** Create test with 10 concurrent threads

#### 1.4.2 Hardware State Transition Race

**Scenario:** One thread tunes while another shuts down

**Race Condition:**
```
Thread 1 (tune)                Thread 2 (shutdown)
─────────────────                ─────────────────
Acquire STATE_MUTEX
                                   Acquire STATE_MUTEX (blocked)
Set state_ = TUNING
                                   (blocked)
Call tune_internal()
                                   (blocked)
Set state_ = READY
                                   (blocked)
Release mutex
                                   Acquire mutex
                                   Set state_ = UNINITIALIZED
                                   Release mutex
```

**Detection:** Add state transition logging
**Fix:** Mutex protection already implemented
**Test:** Create test with concurrent tune and shutdown

---

## 2. Database Integration with Freqman

### 2.1 Freqman File Format Specification

**File Location:** `/FREQMAN/DRONES.TXT`

**File Format (from freqman_db.hpp):**

```
# Comment lines start with #
# Each entry on a separate line
# Format: f=frequency,d=description,m=modulation,b=bandwidth,s=step,t=tone

# Example entries for drone detection
f=2400000000,d=DJI Control,m=0,b=0,s=0,t=0
f=2405000000,d=Parrot Control,m=0,b=0,s=0,t=0
f=2410000000,d=Yuneec Control,m=0,b=0,s=0,t=0
f=2415000000,d=3DR Control,m=0,b=0,s=0,t=0
f=2420000000,d=Autel Control,m=0,b=0,s=0,t=0
f=2432000000,d=DJI Video,m=0,b=0,s=0,t=0
f=2472000000,d=Parrot Video,m=0,b=0,s=0,t=0
f=2484000000,d=Yuneec Video,m=0,b=0,s=0,t=0
```

**Entry Structure (from freqman_db.hpp):**

```cpp
struct freqman_entry {
    int64_t frequency_a{0};        // 'f=freq' or 'a=freq_start'
    int64_t frequency_b{0};        // 'b=freq_end' for ranges
    std::string description{};      // 'd=desc' (max 30 chars)
    freqman_type type{freqman_type::Unknown};
    freqman_index_t modulation{freqman_invalid_index};
    freqman_index_t bandwidth{freqman_invalid_index};
    freqman_index_t step{freqman_invalid_index};
    freqman_index_t tone{freqman_invalid_index};
};
```

**Freqman Types:**

```cpp
enum class freqman_type : uint8_t {
    Single,    // f=frequency
    Range,     // a=start,b=end
    HamRadio,  // Special format
    Repeater,  // Special format
    Raw,       // Raw frequency
    Unknown
};
```

### 2.2 Database Parsing Logic

**File:** `core/database.cpp`

**Parsing Flow:**

```
load_from_file_internal()
├── Open DATABASE_FILE_PATH = "/FREQMAN/DRONES.TXT"
├── For each line in file:
│   ├── Skip empty lines
│   ├── Skip comment lines (starting with #)
│   ├── Call parse_line_internal(line, entry)
│   │   ├── Copy line to line_buffer_ (128 bytes)
│   │   ├── Parse frequency (first field)
│   │   │   ├── Use std::strtok to split by ','
│   │   │   ├── Convert string to uint64_t
│   │   │   └── Store in entry.frequency
│   │   ├── Parse drone type (second field)
│   │   │   ├── Use std::strtok to get next token
│   │   │   ├── Convert to uint8_t
│   │   │   └── Store in entry.drone_type
│   │   ├── Parse priority (third field)
│   │   │   ├── Use std::strtok to get next token
│   │   │   ├── Convert to uint8_t
│   │   │   └── Store in entry.priority
│   │   └── Call validate_entry_internal(entry)
│   │       ├── Check frequency in valid range (2.4-2.4835 GHz)
│   │       └── Check drone_type != UNKNOWN
│   ├── On success: Add entry to entries_[entry_count_++]
│   └── On error: Skip line, log error
├── Close file
└── Return ErrorCode::SUCCESS
```

**Simplified Format Used by EDA:**

The EDA uses a simplified format for drone frequencies:

```
FREQUENCY,TYPE,PRIORITY
```

Example:
```
2400000000,1,0
2405000000,2,0
2410000000,3,0
```

Where:
- **FREQUENCY:** Frequency in Hz (uint64_t)
- **TYPE:** Drone type (1=DJI, 2=Parrot, 3=Yuneec, etc.)
- **PRIORITY:** Scan priority (0=normal, 1=high, etc.)

### 2.3 Database Compatibility with Freqman

#### 2.3.1 Format Compatibility

**Freqman Full Format:**
```
f=2400000000,d=DJI Control,m=0,b=0,s=0,t=0
```

**EDA Simplified Format:**
```
2400000000,1,0
```

**Compatibility Layer:**

The EDA's `parse_line_internal()` function supports both formats:

1. **Full Freqman Format:** Parses `f=`, `d=`, `m=`, `b=`, `s=`, `t=` tags
2. **Simplified Format:** Parses comma-separated values

**Implementation:**

```cpp
ErrorCode DatabaseManager::parse_line_internal(
    const char* line,
    FrequencyEntry& entry
) const noexcept {
    // Copy line to buffer for parsing
    std::strncpy(line_buffer_, line, DATABASE_LINE_BUFFER_SIZE - 1);
    line_buffer_[DATABASE_LINE_BUFFER_SIZE - 1] = '\0';
    
    // Check if line starts with 'f=' (full freqman format)
    if (line_buffer_[0] == 'f' && line_buffer_[1] == '=') {
        // Parse full freqman format
        // f=2400000000,d=DJI Control,m=0,b=0,s=0,t=0
        char* token = std::strtok(line_buffer_ + 2, ",");
        // Parse frequency, description, modulation, etc.
    } else {
        // Parse simplified format
        // 2400000000,1,0
        char* token = std::strtok(line_buffer_, ",");
        // Parse frequency, type, priority
    }
    
    // Validate entry
    return validate_entry_internal(entry);
}
```

#### 2.3.2 Entry Mapping

**Freqman Entry to EDA FrequencyEntry:**

```cpp
// Freqman entry
struct freqman_entry {
    int64_t frequency_a;        // Maps to FrequencyEntry.frequency
    int64_t frequency_b;        // Not used (single frequency only)
    std::string description;    // Not used (type field instead)
    freqman_type type;          // Not used (DroneType enum instead)
    freqman_index_t modulation; // Not used
    freqman_index_t bandwidth;  // Not used
    freqman_index_t step;       // Not used
    freqman_index_t tone;       // Not used
};

// EDA FrequencyEntry
struct FrequencyEntry {
    FreqHz frequency;           // Mapped from freqman_entry.frequency_a
    DroneType drone_type;       // Mapped from type field in simplified format
    uint8_t priority;           // Mapped from priority field in simplified format
    uint16_t reserved;
    uint32_t flags;
};
```

### 2.4 Database Editing Mechanism

**File:** `core/database.cpp`

**Editing Operations:**

#### 2.4.1 Add Entry

```
add_entry(const FrequencyEntry& entry)
├── Validate entry with validate_entry_internal()
│   ├── Check frequency in valid range
│   └── Check drone_type != UNKNOWN
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Check if entry_count_ >= MAX_DATABASE_ENTRIES
│   └── Return BUFFER_FULL if true
├── Find existing entry by frequency
│   ├── Call find_entry_index_internal(entry.frequency)
│   └── If found: Update existing entry
├── If not found: Add new entry
│   ├── entries_[entry_count_] = entry
│   └── entry_count_++
└── Return ErrorCode::SUCCESS
```

#### 2.4.2 Remove Entry

```
remove_entry(FreqHz frequency)
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Find entry index by frequency
│   └── Call find_entry_index_internal(frequency)
├── If not found: Return INVALID_PARAMETER
├── Remove entry by shifting remaining entries
│   ├── for (i = index; i < entry_count_ - 1; ++i)
│   │   └── entries_[i] = entries_[i + 1]
│   └── entry_count_--
├── Adjust current_index_ if needed
│   └── If current_index_ >= entry_count_: Set to entry_count_ - 1
└── Return ErrorCode::SUCCESS
```

#### 2.4.3 Clear All Entries

```
clear_entries()
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Set entry_count_ = 0
├── Set current_index_ = 0
└── Return
```

### 2.5 Error Handling for Malformed Database Files

#### 2.5.1 Malformed Line Detection

**Detection Points:**

1. **Empty Line:**
   - Check: `line[0] == '\0'`
   - Action: Skip line
   - Return: `BUFFER_INVALID`

2. **Invalid Frequency:**
   - Check: `frequency < MIN_FREQUENCY_HZ || frequency > MAX_FREQUENCY_HZ`
   - Action: Skip line
   - Return: `INVALID_PARAMETER`

3. **Invalid Drone Type:**
   - Check: `drone_type == DroneType::UNKNOWN`
   - Action: Skip line
   - Return: `INVALID_PARAMETER`

4. **Missing Fields:**
   - Check: `std::strtok() == nullptr`
   - Action: Skip line
   - Return: `DATABASE_CORRUPTED`

5. **Buffer Overflow:**
   - Check: Line length > `DATABASE_LINE_BUFFER_SIZE`
   - Action: Truncate line
   - Return: `BUFFER_INVALID`

#### 2.5.2 Graceful Degradation Strategy

```
Malformed Database File
├── Open file
├── For each line:
│   ├── Try to parse line
│   ├── On success: Add entry
│   ├── On error:
│   │   ├── Log error (if DEBUG_LOGGING_ENABLED)
│   │   ├── Increment error count
│   │   └── Continue to next line
│   └── Continue
├── After parsing all lines:
│   ├── If error_count > 0:
│   │   └── Log warning: "Database had X malformed lines"
│   ├── If entry_count_ == 0:
│   │   └── Add default frequencies
│   └── Set loaded_.set()
└── Return ErrorCode::SUCCESS
```

### 2.6 Database Testing Checklist

#### File Loading

- [ ] Test with valid database file
- [ ] Test with missing database file
- [ ] Test with empty database file
- [ ] Test with corrupted database file
- [ ] Test with malformed entries
- [ ] Test with extremely long lines (> 128 bytes)
- [ ] Test with comment lines (starting with #)
- [ ] Test with empty lines
- [ ] Test with duplicate frequencies
- [ ] Test with frequencies outside valid range

#### Format Compatibility

- [ ] Test full freqman format parsing
- [ ] Test simplified format parsing
- [ ] Test mixed format file
- [ ] Verify frequency mapping
- [ ] Verify drone type mapping
- [ ] Verify priority mapping

#### Editing Operations

- [ ] Test add_entry() with valid entry
- [ ] Test add_entry() with invalid frequency
- [ ] Test add_entry() with invalid drone type
- [ ] Test add_entry() with duplicate frequency (update)
- [ ] Test add_entry() when database full
- [ ] Test remove_entry() with existing frequency
- [ ] Test remove_entry() with non-existent frequency
- [ ] Test clear_entries()
- [ ] Test concurrent add/remove operations

#### Thread Safety

- [ ] Test concurrent load operations
- [ ] Test concurrent add operations
- [ ] Test concurrent remove operations
- [ ] Test concurrent add/remove
- [ ] Test concurrent read/write
- [ ] Verify mutex protection
- [ ] Verify double-checked locking

#### Memory Constraints

- [ ] Verify database fits in static RAM (500 entries × 16 bytes = 8KB)
- [ ] Verify line buffer doesn't overflow stack
- [ ] Verify no heap allocation
- [ ] Verify memory leak free

---

## 3. Hardware Access Flow

### 3.1 Hardware Controller Architecture

**File:** `core/hardware_controller.hpp`

**Component Overview:**

```
┌─────────────────────────────────────────────────────────────┐
│              HardwareController                             │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  State Machine:                                           │
│  ┌─────────────────────────────────────────────┐           │
│  │ UNINITIALIZED → INITIALIZING → READY     │           │
│  │               ↓                   ↓         │           │
│  │             ERROR ←─────── TUNING ←───┘           │
│  │                   ↓                              │           │
│  │               STREAMING                        │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Configuration:                                           │
│  • center_frequency: 2.432 GHz (default)                  │
│  • sample_rate: 2 MHz                                      │
│  • gain: 20 (0-40)                                       │
│  • lna_gain: 8                                            │
│  • vga_gain: 12                                           │
│                                                          │
│  Flags (AtomicFlag):                                       │
│  • pll_locked_: PLL lock status                              │
│  • streaming_active_: Spectrum streaming active               │
│                                                          │
│  Mutex: LockOrder::STATE_MUTEX                              │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Tune to Frequency Flow

**File:** `core/hardware_controller.cpp`

**Flow Diagram:**

```
tune_to_frequency(frequency, max_retries=3)
├── Validate frequency with validate_frequency_internal()
│   ├── Check frequency >= MIN_FREQUENCY_HZ (2.4 GHz)
│   ├── Check frequency <= MAX_FREQUENCY_HZ (2.4835 GHz)
│   └── Return INVALID_PARAMETER if invalid
├── Acquire MutexLock<LockOrder::STATE_MUTEX>
├── Check state_ in {READY, STREAMING}
│   └── Return HARDWARE_NOT_INITIALIZED if false
├── Set state_ = HardwareState::TUNING
├── For retry = 0 to max_retries-1:
│   ├── Set retry_count_ = retry
│   ├── Call tune_internal(frequency)
│   │   ├── Calculate PLL parameters
│   │   ├── Write to PLL registers via SPI
│   │   ├── Wait for PLL to settle
│   │   └── Return SUCCESS or HARDWARE_FAILURE
│   ├── On SUCCESS:
│   │   ├── Wait for PLL lock
│   │   │   ├── SystemTime start_time = chTimeNow()
│   │   │   ├── While !check_pll_lock_internal():
│   │   │   │   ├── Check timeout (chTimeNow() - start_time > MS2ST(PLL_LOCK_TIMEOUT_MS))
│   │   │   │   ├── On timeout:
│   │   │   │   │   ├── If retry < max_retries-1:
│   │   │   │   │   │   ├── chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS)
│   │   │   │   │   │   └── break (retry loop)
│   │   │   │   │   └── If retry == max_retries-1:
│   │   │   │   │       └── Return PLL_LOCK_FAILURE
│   │   │   │   └── chThdSleepMilliseconds(PLL_LOCK_POLL_INTERVAL_MS)
│   │   │   └── PLL locked successfully
│   │   ├── Set current_frequency_ = frequency
│   │   ├── Set pll_locked_.set()
│   │   ├── Set state_ = streaming_active_.test() ? STREAMING : READY
│   │   ├── Set last_error_ = SUCCESS
│   │   └── Return SUCCESS
│   └── On HARDWARE_FAILURE:
│       ├── If retry < max_retries-1:
│       │   ├── chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS)
│       │   └── continue (retry loop)
│       └── If retry == max_retries-1:
│           ├── Set state_ = ERROR
│           ├── Set last_error_ = HARDWARE_FAILURE
│           └── Return HARDWARE_FAILURE
└── Return PLL_LOCK_FAILURE (if all retries failed)
```

### 3.3 PLL Lock Verification Mechanism

**Implementation:**

```cpp
bool HardwareController::check_pll_lock_internal() const noexcept {
    // Placeholder for PLL lock check
    // In actual implementation, this would read PLL lock status from hardware
    
    // Read PLL lock register via SPI
    // uint8_t lock_status = spi_read_pll_lock_register();
    
    // Check if PLL is locked
    // bool locked = (lock_status & PLL_LOCK_MASK) != 0;
    
    // For now, return flag value
    return pll_locked_.test();
}
```

**Verification Sequence:**

```
PLL Lock Verification
├── Tune to frequency (tune_internal)
├── Wait for PLL to settle (typically 1-2 ms)
├── Start polling loop:
│   ├── Read PLL lock register via SPI
│   ├── Check if PLL_LOCK bit is set
│   ├── If locked: Return true
│   ├── If not locked:
│   │   ├── Check timeout (100 ms)
│   │   ├── If timeout: Return false
│   │   └── If not timeout: Wait 3 ms, retry
└── Return lock status
```

**Timeout Values:**

- **PLL_LOCK_TIMEOUT_MS:** 100 ms (from constants.hpp)
- **PLL_LOCK_POLL_INTERVAL_MS:** 3 ms (from constants.hpp)
- **HARDWARE_RETRY_DELAY_MS:** 10 ms (from constants.hpp)

### 3.4 Spectrum Streaming Start/Stop Sequence

#### 3.4.1 Start Streaming

```
start_spectrum_streaming()
├── Acquire MutexLock<LockOrder::STATE_MUTEX>
├── Check state_ in {READY, TUNING}
│   └── Return HARDWARE_NOT_INITIALIZED if false
├── Check streaming_active_.test()
│   └── Return SUCCESS if already streaming
├── Call start_streaming_internal()
│   ├── Enable spectrum processor
│   │   ├── Configure spectrum collector
│   │   ├── Start RSSI DMA
│   │   └── Enable spectrum streaming
│   └── Return SUCCESS
├── On success:
│   ├── Set streaming_active_.set()
│   ├── Set state_ = HardwareState::STREAMING
│   └── Set last_error_ = SUCCESS
├── On error:
│   ├── Set last_error_ = error
│   └── Return error
└── Return result
```

#### 3.4.2 Stop Streaming

```
stop_spectrum_streaming()
├── Acquire MutexLock<LockOrder::STATE_MUTEX>
├── Check !streaming_active_.test()
│   └── Return SUCCESS if not streaming
├── Call stop_streaming_internal()
│   ├── Stop RSSI DMA
│   ├── Disable spectrum processor
│   └── Disable spectrum streaming
│   └── Return SUCCESS
├── On success:
│   ├── Set streaming_active_.clear()
│   ├── Set state_ = HardwareState::READY
│   └── Set last_error_ = SUCCESS
├── On error:
│   ├── Set last_error_ = error
│   └── Return error
└── Return result
```

### 3.5 RSSI Sampling Mechanism

**File:** `core/hardware_controller.cpp`

**Sampling Flow:**

```
get_rssi_sample()
├── Acquire MutexLock<LockOrder::STATE_MUTEX>
├── Check state_ == STREAMING
│   └── Return HARDWARE_NOT_INITIALIZED if false
├── Call read_rssi_internal()
│   ├── Read RSSI from hardware
│   │   ├── Read RSSI register via SPI
│   │   ├── Convert to dBm
│   │   └── Clamp to valid range (-120 to -20 dBm)
│   ├── Get current timestamp
│   │   └── SystemTime timestamp = chTimeNow()
│   ├── Get current frequency
│   │   └── FreqHz frequency = current_frequency_
│   ├── Create RssiSample
│   │   ├── sample.rssi = RSSI value
│   │   ├── sample.timestamp = timestamp
│   │   └── sample.frequency = frequency
│   └── Return ErrorResult<RssiSample>::success(sample)
└── Return result
```

**RSSI Value Range:**

- **Minimum:** -120 dBm (RSSI_MIN_DBM)
- **Maximum:** -20 dBm (RSSI_MAX_DBM)
- **Noise Floor:** -100 dBm (RSSI_NOISE_FLOOR_DBM)
- **Detection Threshold:** -90 dBm (RSSI_DETECTION_THRESHOLD_DBM)

### 3.6 Error Handling for Hardware Failures

**File:** `core/hardware_controller.cpp`

**Error Handling Flow:**

```
handle_error_internal(error)
├── Switch on error type:
│   ├── PLL_LOCK_FAILURE:
│   │   ├── Clear pll_locked_.clear()
│   │   ├── Use last known frequency
│   │   └── Return SUCCESS
│   │
│   ├── HARDWARE_TIMEOUT:
│   │   ├── Retry operation
│   │   └── Return SUCCESS
│   │
│   ├── HARDWARE_FAILURE:
│   │   ├── Call reset()
│   │   │   ├── Stop streaming if active
│   │   │   ├── Reset state to READY
│   │   │   ├── Clear flags
│   │   │   └── Reapply default configuration
│   │   └── Return result
│   │
│   └── default:
│       └── Return error (no recovery)
└── Return result
```

**Retry Logic:**

```cpp
// Tune with retry logic
for (uint32_t retry = 0; retry < max_retries; ++retry) {
    retry_count_ = retry;
    
    ErrorCode tune_result = tune_internal(frequency);
    if (tune_result != ErrorCode::SUCCESS) {
        if (retry < max_retries - 1) {
            // Wait before retry
            chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS);
            continue;
        }
        state_ = HardwareState::ERROR;
        last_error_ = tune_result;
        return tune_result;
    }
    
    // Wait for PLL lock
    // ... (PLL lock verification)
    
    // PLL locked successfully
    current_frequency_ = frequency;
    pll_locked_.set();
    state_ = streaming_active_.test() ? HardwareState::STREAMING : HardwareState::READY;
    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}
```

### 3.7 Hardware Access Testing Checklist

#### Tune to Frequency

- [ ] Test tune to valid frequency (2.4 GHz)
- [ ] Test tune to invalid frequency (< 2.4 GHz)
- [ ] Test tune to invalid frequency (> 2.4835 GHz)
- [ ] Test PLL lock success
- [ ] Test PLL lock timeout
- [ ] Test retry logic (3 retries)
- [ ] Test retry delay (10 ms)
- [ ] Verify state transitions (READY → TUNING → READY)
- [ ] Verify current_frequency_ updated
- [ ] Verify pll_locked_ flag set

#### Spectrum Streaming

- [ ] Test start streaming success
- [ ] Test start streaming when already streaming
- [ ] Test start streaming when not ready
- [ ] Test stop streaming success
- [ ] Test stop streaming when not streaming
- [ ] Verify streaming_active_ flag
- [ ] Verify state transitions (READY → STREAMING → READY)
- [ ] Test concurrent start/stop

#### RSSI Sampling

- [ ] Test get RSSI sample success
- [ ] Test get RSSI when not streaming
- [ ] Verify RSSI value range (-120 to -20 dBm)
- [ ] Verify timestamp accuracy
- [ ] Verify frequency accuracy
- [ ] Test RSSI clamping

#### Error Handling

- [ ] Test SPI communication failure
- [ ] Test PLL lock failure
- [ ] Test hardware timeout
- [ ] Test hardware failure recovery
- [ ] Test reset functionality
- [ ] Verify error codes
- [ ] Verify last_error_ updated
- [ ] Verify graceful degradation

#### Thread Safety

- [ ] Test concurrent tune operations
- [ ] Test concurrent start/stop streaming
- [ ] Test concurrent RSSI sampling
- [ ] Verify mutex protection
- [ ] Verify state machine consistency
- [ ] Test deadlock scenarios

#### Memory Constraints

- [ ] Verify no heap allocation
- [ ] Verify stack usage < 512 bytes
- [ ] Verify static RAM usage
- [ ] Verify memory leak free

---

**End of Part 1**

**Continue to Part 2:** Scanning Logic Flow, Display and Output Flow, Audio Alert Flow
