# Enhanced Drone Analyzer - Comprehensive Testing and Debugging Plan

**Part 2: Scanning Logic, Display, and Audio Flows**

---

## Table of Contents (Part 2)

4. [Scanning Logic Flow](#4-scanning-logic-flow)
5. [Display and Output Flow](#5-display-and-output-flow)
6. [Audio Alert Flow](#6-audio-alert-flow)

---

## 4. Scanning Logic Flow

### 4.1 Scanner Architecture

**File:** `core/scanner.hpp`

**Scanner State Machine:**

```
┌─────────────────────────────────────────────────────────────┐
│              DroneScanner State Machine                    │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  States:                                                 │
│  ┌─────────────────────────────────────────────┐           │
│  │ IDLE → SCANNING → PAUSED → ERROR     │           │
│  │   ↑       ↓         ↓         ↓          │           │
│  │   └───────┴─────────┴─────────┘          │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Transitions:                                             │
│  • initialize(): IDLE → IDLE (or ERROR)                  │
│  • start_scanning(): IDLE/PAUSED → SCANNING              │
│  • stop_scanning(): SCANNING/PAUSED → IDLE               │
│  • pause_scanning(): SCANNING → PAUSED                    │
│  • resume_scanning(): PAUSED → SCANNING                   │
│  • error: Any state → ERROR                              │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Perform Scan Cycle Flow

**File:** `core/scanner.cpp`

**Flow Diagram:**

```
perform_scan_cycle() [Called periodically by scanner thread]
├── Check scanning_active_.test()
│   └── Return SUCCESS if false (not scanning)
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Check state_ == SCANNING
│   └── Return HARDWARE_NOT_INITIALIZED if false
├── Call perform_scan_cycle_internal()
│   ├── Update statistics.total_scan_cycles++
│   │
│   ├── Get next frequency from database
│   │   └── Call database_.get_next_frequency(current_frequency_)
│   │       ├── On success: Set current_frequency_ = result
│   │       └── On error:
│   │           ├── Increment statistics.failed_cycles++
│   │           └── Return error
│   │
│   ├── Tune to frequency
│   │   └── Call hardware_.tune_to_frequency(current_frequency_)
│   │       ├── On success: Continue
│   │       └── On error:
│   │           ├── Increment statistics.failed_cycles++
│   │           └── Return error
│   │
│   ├── Get RSSI sample
│   │   └── Call hardware_.get_rssi_sample()
│   │       ├── On success: Get sample
│   │       └── On error:
│   │           ├── Increment statistics.failed_cycles++
│   │           └── Return error
│   │
│   ├── Check if signal detected
│   │   └── if (sample.rssi >= config_.rssi_threshold_dbm)
│   │       └── Update tracked drones
│   │           └── Call update_tracked_drone_internal(
│   │                   sample.frequency,
│   │                   sample.rssi,
│   │                   sample.timestamp)
│   │               ├── Find existing drone by frequency
│   │               │   └── Call find_drone_by_frequency_internal(frequency)
│   │               │       ├── On found: Update drone
│   │               │       │   └── tracked_drones_[index].update_rssi(rssi, timestamp)
│   │               │       └── On not found: Add new drone
│   │               │           └── Call add_tracked_drone_internal(frequency, rssi, timestamp)
│   │               │               ├── Check tracked_count_ < MAX_TRACKED_DRONES
│   │               │               ├── Determine drone type
│   │               │               │   └── Call determine_drone_type_internal(frequency)
│   │               │               ├── Determine threat level
│   │               │               │   └── Call determine_threat_level_internal(rssi)
│   │               │               ├── Create TrackedDrone
│   │               │               │   └── tracked_drones_[tracked_count_] = TrackedDrone(...)
│   │               │               ├── Update RSSI history
│   │               │               ├── tracked_count_++
│   │               │               └── Increment statistics.drones_detected++
│   │               ├── On SUCCESS:
│   │               │   ├── Increment statistics.successful_cycles++
│   │               │   └── Update max RSSI if needed
│   │               │       └── if (sample.rssi > statistics.max_rssi_dbm)
│   │               │           └── statistics.max_rssi_dbm = sample.rssi
│   │               └── On error:
│   │                   └── Increment statistics.failed_cycles++
│   │
│   ├── Update last_scan_time_
│   │   └── last_scan_time_ = chTimeNow()
│   │
│   └── Return ErrorCode::SUCCESS
│
└── Return result
```

### 4.3 Database Iteration Logic

**File:** `core/database.cpp`

**Iteration Flow:**

```
get_next_frequency(current_freq)
├── Ensure database is loaded
│   └── Call load_frequency_database()
│       ├── Check loaded_.test() → Return SUCCESS if true
│       ├── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Double-check loaded_.test()
│       ├── Load from file if not loaded
│       └── Set loaded_.set()
│
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
│
├── If current_freq == 0:
│   ├── Set current_index_ = 0
│   └── Return entries_[current_index_].frequency
│
├── Find current frequency in database
│   └── Call find_entry_index_internal(current_freq)
│       ├── For i = 0 to entry_count_-1:
│       │   └── if (entries_[i].frequency == current_freq)
│       │       └── Return i
│       └── Return INVALID_PARAMETER if not found
│
├── If not found:
│   ├── Set current_index_ = 0
│   └── Return entries_[current_index_].frequency
│
├── If found:
│   ├── Set current_index_ = index
│   ├── Increment current_index_++
│   ├── If current_index_ >= entry_count_:
│   │   └── Set current_index_ = 0 (wrap around)
│   └── Return entries_[current_index_].frequency
│
└── Return result
```

**Iteration Behavior:**

- **Sequential Mode:** Iterate through all frequencies in order
- **Wrap Around:** After last frequency, return to first
- **Start Point:** If `current_freq == 0`, start from first entry
- **Missing Frequency:** If frequency not found, start from first entry

### 4.4 Hardware Tuning Per Frequency

**File:** `core/hardware_controller.cpp`

**Tuning Flow:**

```
tune_to_frequency(frequency, max_retries=3)
├── Validate frequency range
│   ├── Check frequency >= MIN_FREQUENCY_HZ (2.4 GHz)
│   ├── Check frequency <= MAX_FREQUENCY_HZ (2.4835 GHz)
│   └── Return INVALID_PARAMETER if invalid
│
├── Acquire MutexLock<LockOrder::STATE_MUTEX>
│
├── Check state_ in {READY, STREAMING}
│   └── Return HARDWARE_NOT_INITIALIZED if false
│
├── Set state_ = HardwareState::TUNING
│
├── For retry = 0 to max_retries-1:
│   ├── Set retry_count_ = retry
│   │
│   ├── Call tune_internal(frequency)
│   │   ├── Calculate PLL parameters
│   │   │   ├── Calculate N divider
│   │   │   ├── Calculate R divider
│   │   │   └── Calculate reference frequency
│   │   ├── Write to PLL registers via SPI
│   │   │   ├── Write N divider register
│   │   │   ├── Write R divider register
│   │   │   ├── Write control register
│   │   │   └── Wait for SPI transaction complete
│   │   ├── Wait for PLL to settle
│   │   │   └── chThdSleepMilliseconds(2)
│   │   └── Return SUCCESS or HARDWARE_FAILURE
│   │
│   ├── On SUCCESS:
│   │   ├── Wait for PLL lock
│   │   │   ├── SystemTime start_time = chTimeNow()
│   │   │   ├── While !check_pll_lock_internal():
│   │   │   │   ├── Check timeout
│   │   │   │   │   └── if (chTimeNow() - start_time > MS2ST(PLL_LOCK_TIMEOUT_MS))
│   │   │   │       ├── If retry < max_retries-1:
│   │   │   │       │   ├── chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS)
│   │   │   │       │   └── break (retry loop)
│   │   │   │       └── If retry == max_retries-1:
│   │   │   │           └── Return PLL_LOCK_FAILURE
│   │   │   └── chThdSleepMilliseconds(PLL_LOCK_POLL_INTERVAL_MS)
│   │   ├── PLL locked successfully
│   │   ├── Set current_frequency_ = frequency
│   │   ├── Set pll_locked_.set()
│   │   ├── Set state_ = streaming_active_.test() ? STREAMING : READY
│   │   ├── Set last_error_ = SUCCESS
│   │   └── Return SUCCESS
│   │
│   └── On HARDWARE_FAILURE:
│       ├── If retry < max_retries-1:
│       │   ├── chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS)
│       │   └── continue (retry loop)
│       └── If retry == max_retries-1:
│           ├── Set state_ = ERROR
│           ├── Set last_error_ = HARDWARE_FAILURE
│           └── Return HARDWARE_FAILURE
│
└── Return PLL_LOCK_FAILURE (if all retries failed)
```

### 4.5 RSSI Detection Flow

**File:** `core/scanner.cpp`

**Detection Flow:**

```
Signal Detection in perform_scan_cycle_internal()
├── Get RSSI sample from hardware
│   └── ErrorResult<RssiSample> rssi_result = hardware_.get_rssi_sample()
│
├── Check if result has value
│   └── if (rssi_result.has_value())
│       └── const RssiSample& sample = rssi_result.value()
│
├── Check if signal detected
│   └── if (sample.rssi >= config_.rssi_threshold_dbm)
│       └── Signal detected above threshold
│           └── Update tracked drones
│               └── Call update_tracked_drone_internal(
│                       sample.frequency,
│                       sample.rssi,
│                       sample.timestamp)
│
└── else
    └── Signal below threshold
        └── Increment successful_cycles (no drone detected)
```

**Detection Thresholds:**

- **RSSI_DETECTION_THRESHOLD_DBM:** -90 dBm (default)
- **RSSI_HIGH_THREAT_THRESHOLD_DBM:** -60 dBm
- **RSSI_CRITICAL_THREAT_THRESHOLD_DBM:** -40 dBm
- **RSSI_NOISE_FLOOR_DBM:** -100 dBm

### 4.6 Drone Detection Logic

**File:** `core/scanner.cpp`

**Detection Flow:**

```
update_tracked_drone_internal(frequency, rssi, timestamp)
├── Find existing drone by frequency
│   └── Call find_drone_by_frequency_internal(frequency)
│       ├── For i = 0 to tracked_count_-1:
│       │   └── if (tracked_drones_[i].frequency == frequency)
│       │       └── Return i
│       └── Return INVALID_PARAMETER if not found
│
├── If found:
│   ├── Update existing drone
│   │   └── tracked_drones_[index].update_rssi(rssi, timestamp)
│   │       ├── Update RSSI history
│   │       │   ├── rssi_history_[history_index_] = rssi
│   │       │   ├── timestamp_history_[history_index_] = timestamp
│   │       │   └── history_index_ = (history_index_ + 1) % 3
│   │       └── Increment update_count
│   └── Return SUCCESS
│
└── If not found:
    └── Add new drone
        └── Call add_tracked_drone_internal(frequency, rssi, timestamp)
            ├── Check tracked_count_ < MAX_TRACKED_DRONES
            │   └── Return BUFFER_FULL if false
            │
            ├── Determine drone type
            │   └── Call determine_drone_type_internal(frequency)
            │       ├── if (frequency >= 2'400'000'000 && frequency <= 2'405'000'000)
            │       │   └── return DroneType::DJI
            │       ├── else if (frequency >= 2'470'000'000 && frequency <= 2'483'500'000)
            │       │   └── return DroneType::PARROT
            │       └── else
            │           └── return DroneType::UNKNOWN
            │
            ├── Determine threat level
            │   └── Call determine_threat_level_internal(rssi)
            │       ├── if (rssi >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM)
            │       │   └── return ThreatLevel::CRITICAL
            │       ├── else if (rssi >= RSSI_HIGH_THREAT_THRESHOLD_DBM)
            │       │   └── return ThreatLevel::HIGH
            │       ├── else if (rssi >= RSSI_DETECTION_THRESHOLD_DBM)
            │       │   └── return ThreatLevel::MEDIUM
            │       └── else
            │           └── return ThreatLevel::LOW
            │
            ├── Create new tracked drone
            │   └── tracked_drones_[tracked_count_] = TrackedDrone(frequency, type, threat)
            │       ├── Initialize frequency, type, threat
            │       ├── Initialize RSSI history
            │       ├── Initialize timestamp history
            │       └── Set update_count = 0
            │
            ├── Update RSSI history
            │   └── tracked_drones_[tracked_count_].update_rssi(rssi, timestamp)
            │
            ├── Increment tracked_count_
            │   └── tracked_count_++
            │
            └── Increment statistics
                └── statistics_.drones_detected++
```

### 4.7 Tracked Drone Management

#### 4.7.1 Drone Type Classification

**Frequency-Based Classification:**

```cpp
DroneType DroneScanner::determine_drone_type_internal(FreqHz frequency) const noexcept {
    // DJI drones often use specific frequency bands
    if (frequency >= 2'400'000'000ULL && frequency <= 2'405'000'000ULL) {
        return DroneType::DJI;
    }
    
    // Parrot drones often use different bands
    if (frequency >= 2'470'000'000ULL && frequency <= 2'483'500'000ULL) {
        return DroneType::PARROT;
    }
    
    // Default to unknown
    return DroneType::UNKNOWN;
}
```

**Classification Ranges:**

- **DJI:** 2.400 - 2.405 GHz (5 MHz bandwidth)
- **Parrot:** 2.470 - 2.4835 GHz (13.5 MHz bandwidth)
- **Unknown:** All other frequencies

#### 4.7.2 Threat Level Calculation

**RSSI-Based Classification:**

```cpp
ThreatLevel DroneScanner::determine_threat_level_internal(RssiValue rssi) const noexcept {
    // Classify threat based on RSSI
    if (rssi >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM) {  // -40 dBm
        return ThreatLevel::CRITICAL;
    } else if (rssi >= RSSI_HIGH_THREAT_THRESHOLD_DBM) {  // -60 dBm
        return ThreatLevel::HIGH;
    } else if (rssi >= RSSI_DETECTION_THRESHOLD_DBM) {  // -90 dBm
        return ThreatLevel::MEDIUM;
    } else {
        return ThreatLevel::LOW;
    }
}
```

**Threat Level Ranges:**

- **CRITICAL:** RSSI >= -40 dBm (very close, < 10 meters)
- **HIGH:** -40 dBm > RSSI >= -60 dBm (close, 10-50 meters)
- **MEDIUM:** -60 dBm > RSSI >= -90 dBm (moderate, 50-200 meters)
- **LOW:** RSSI < -90 dBm (far, > 200 meters)

### 4.8 Stale Drone Removal

**File:** `core/scanner.cpp`

**Removal Flow:**

```
remove_stale_drones(current_time)
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Call remove_stale_drones_internal(current_time)
│   ├── Initialize write_index = 0
│   │
│   ├── For read_index = 0 to tracked_count_-1:
│   │   └── if (!tracked_drones_[read_index].is_stale(current_time, config_.stale_timeout_ms))
│   │       └── Keep this drone (not stale)
│   │           ├── if (write_index != read_index)
│   │           │   └── tracked_drones_[write_index] = tracked_drones_[read_index]
│   │           └── write_index++
│   │
│   └── Set tracked_count_ = write_index
│
└── Return
```

**Stale Detection:**

```cpp
bool TrackedDrone::is_stale(SystemTime current_time, SystemTime timeout_ms) const noexcept {
    // Check if drone hasn't been seen recently
    return (current_time - last_seen) > timeout_ms;
}
```

**Stale Timeout:**

- **DRONE_STALE_TIMEOUT_MS:** 5000 ms (5 seconds)
- **DRONE_REMOVAL_TIMEOUT_MS:** 30000 ms (30 seconds)

**Removal Behavior:**

- Drones not seen for 5 seconds are marked as stale
- Stale drones are removed from tracked list
- Array is compacted (no gaps left)
- Tracking continues with remaining drones

### 4.9 Threat Level Calculation

**Threat Level Factors:**

1. **RSSI Strength:** Primary factor
2. **Movement Trend:** Approaching drones are higher threat
3. **Drone Type:** Some types are higher priority

**Calculation Flow:**

```
Threat Level Calculation
├── Base threat from RSSI
│   ├── RSSI >= -40 dBm → CRITICAL
│   ├── RSSI >= -60 dBm → HIGH
│   ├── RSSI >= -90 dBm → MEDIUM
│   └── RSSI < -90 dBm → LOW
│
├── Adjust for movement trend
│   ├── APPROACHING → Increase threat by 1 level
│   ├── STATIC → No change
│   └── RECEDING → Decrease threat by 1 level
│
└── Clamp to valid range (LOW to CRITICAL)
```

### 4.10 Scanning Logic Testing Checklist

#### Scan Cycle

- [ ] Test perform_scan_cycle() when scanning
- [ ] Test perform_scan_cycle() when not scanning
- [ ] Test perform_scan_cycle() when paused
- [ ] Test perform_scan_cycle() when in error state
- [ ] Verify statistics.total_scan_cycles incremented
- [ ] Verify statistics.successful_cycles incremented
- [ ] Verify statistics.failed_cycles incremented
- [ ] Verify max RSSI tracking

#### Database Iteration

- [ ] Test get_next_frequency() with current_freq = 0
- [ ] Test get_next_frequency() with valid current_freq
- [ ] Test get_next_frequency() with invalid current_freq
- [ ] Test wrap-around after last frequency
- [ ] Test database empty
- [ ] Test database with single entry
- [ ] Test database with multiple entries
- [ ] Verify current_index_ updated correctly

#### Hardware Tuning

- [ ] Test tune_to_frequency() with valid frequency
- [ ] Test tune_to_frequency() with invalid frequency
- [ ] Test PLL lock success
- [ ] Test PLL lock timeout
- [ ] Test retry logic (3 retries)
- [ ] Verify retry delay (10 ms)
- [ ] Verify state transitions
- [ ] Verify current_frequency_ updated
- [ ] Verify pll_locked_ flag set

#### RSSI Detection

- [ ] Test signal detection above threshold
- [ ] Test signal detection below threshold
- [ ] Test RSSI at threshold (-90 dBm)
- [ ] Test RSSI at critical threshold (-40 dBm)
- [ ] Test RSSI at high threshold (-60 dBm)
- [ ] Verify RSSI clamping (-120 to -20 dBm)
- [ ] Verify timestamp accuracy
- [ ] Verify frequency accuracy

#### Drone Detection

- [ ] Test new drone detection
- [ ] Test existing drone update
- [ ] Test drone type classification (DJI)
- [ ] Test drone type classification (Parrot)
- [ ] Test drone type classification (Unknown)
- [ ] Test threat level calculation (CRITICAL)
- [ ] Test threat level calculation (HIGH)
- [ ] Test threat level calculation (MEDIUM)
- [ ] Test threat level calculation (LOW)
- [ ] Test tracked drones array overflow
- [ ] Verify RSSI history update
- [ ] Verify timestamp history update

#### Stale Drone Removal

- [ ] Test remove_stale_drones() with fresh drones
- [ ] Test remove_stale_drones() with stale drones
- [ ] Test stale timeout (5 seconds)
- [ ] Test array compaction
- [ ] Test removal of all drones
- [ ] Test removal of some drones
- [ ] Verify tracked_count_ updated

#### Thread Safety

- [ ] Test concurrent scan cycles
- [ ] Test concurrent drone updates
- [ ] Test concurrent stale removal
- [ ] Verify mutex protection
- [ ] Test deadlock scenarios
- [ ] Verify data consistency

#### Performance

- [ ] Measure scan cycle duration
- [ ] Verify scan cycle < 100 ms
- [ ] Measure frequency tuning time
- [ ] Measure RSSI sampling time
- [ ] Verify real-time constraints
- [ ] Test with maximum database size (500 entries)
- [ ] Test with maximum tracked drones (20)

---

## 5. Display and Output Flow

### 5.1 Display Architecture

**File:** `ui/drone_display.hpp`

**Display Components:**

```
┌─────────────────────────────────────────────────────────────┐
│                 DroneDisplay (ui::View)                   │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  Display Layout (240x320 pixels):                         │
│  ┌─────────────────────────────────────────────┐           │
│  │ Status Bar (20px)                       │           │
│  │ "Scanning... | 3 drones | MAX: -45 dBm"│           │
│  ├─────────────────────────────────────────────┤           │
│  │ Spectrum Display (100px)                  │           │
│  │ ╔═════════════════════════════════╗ │           │
│  │ ║ Signal strength visualization      ║ │           │
│  │ ╚═════════════════════════════════╝ │           │
│  ├─────────────────────────────────────────────┤           │
│  │ Histogram Display (80px)                  │           │
│  │ ╔═════════════════════════════════╗ │           │
│  │ ║ Signal distribution histogram    ║ │           │
│  │ ╚═════════════════════════════════╝ │           │
│  ├─────────────────────────────────────────────┤           │
│  │ Drone List (100px)                       │           │
│  │ ┌─────────────────────────────────────┐   │           │
│  │ │ DJI @ 2.405 GHz [-45 dBm] HIGH│   │           │
│  │ │ Parrot @ 2.470 GHz [-60 dBm] MED │   │           │
│  │ │ Unknown @ 2.432 GHz [-75 dBm] LOW│   │           │
│  │ └─────────────────────────────────────┘   │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Static Storage:                                          │
│  • display_data_ (DisplayData, ~820 bytes)               │
│  • spectrum_buffer_ (uint8_t[240], 240 bytes)           │
│  • histogram_buffer_ (uint16_t[128], 256 bytes)         │
│  • status_text_ (char[32], 32 bytes)                    │
│                                                          │
│  Total: ~1348 bytes static storage                          │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 UI Update Cycle

**File:** `ui/drone_scanner_ui.cpp`

**Update Flow:**

```
UI Update Cycle [Called at 60 FPS]
├── Get current time
│   └── SystemTime now = chTimeNow()
│
├── Get display data from scanner
│   └── Call scanner_.get_display_data(display_data_)
│       ├── Acquire MutexLock<LockOrder::DATA_MUTEX>
│       ├── Call update_display_data_internal(display_data_)
│       │   ├── Clear display data
│       │   │   └── display_data.clear()
│       │   ├── Copy tracked drones to display entries
│       │   │   ├── copy_count = min(tracked_count_, MAX_DISPLAYED_DRONES)
│       │   │   └── for i = 0 to copy_count-1:
│       │   │       └── display_data.drones[i] = DisplayDroneEntry(tracked_drones_[i])
│       │   ├── Set display_data.drone_count = copy_count
│       │   ├── Update state snapshot
│       │   │   ├── display_data.state.scanning_active = scanning_active_.test()
│       │   │   ├── display_data.state.is_fresh = true
│       │   │   ├── Calculate max threat
│       │   │   ├── Count movement trends
│       │   │   └── Update statistics
│       │   └── Return SUCCESS
│       └── Release mutex
│
├── Update display components
│   ├── Update spectrum display
│   │   └── Call render_spectrum(painter, spectrum_data_, ...)
│   ├── Update histogram display
│   │   └── Call render_histogram(painter, histogram_data_, ...)
│   ├── Update drone list
│   │   └── Call render_drone_list(painter, display_data_.drones, ...)
│   └── Update status bar
│       └── Call render_status_bar(painter, status_text_, ...)
│
├── Update timers
│   ├── Update alert timer
│   │   └── update_alert_timer(elapsed_ms)
│   └── Update error timer
│       └── update_error_timer(elapsed_ms)
│
├── Call paint() to render
│   └── paint(painter)
│       ├── Draw background
│       ├── Draw spectrum
│       ├── Draw histogram
│       ├── Draw drone list
│       ├── Draw status bar
│       ├── Draw alert overlay (if active)
│       └── Draw error overlay (if active)
│
└── Schedule next update (16 ms = 60 FPS)
```

### 5.3 Spectrum Rendering Flow

**File:** `ui/drone_display.cpp`

**Rendering Flow:**

```
render_spectrum(painter, spectrum_data, spectrum_size, start_x, start_y, width, height)
├── Validate spectrum data
│   └── Call validate_spectrum_data(spectrum_data, spectrum_size)
│       ├── Check spectrum_data != nullptr
│       ├── Check spectrum_size > 0
│       ├── Check spectrum_size <= SPECTRUM_BUFFER_SIZE (240)
│       └── Return INVALID_PARAMETER if invalid
│
├── Draw spectrum background
│   └── draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND, true)
│
├── For i = 0 to spectrum_size-1:
│   ├── Calculate x position
│   │   └── x = start_x + (i * width / spectrum_size)
│   ├── Calculate bar height
│   │   └── bar_height = map_rssi_to_height(spectrum_data[i], height)
│   │       ├── Normalize RSSI to 0-255 range
│   │       ├── Scale to display height
│   │       └── Clamp to valid range
│   ├── Get color based on amplitude
│   │   └── color = get_spectrum_color(spectrum_data[i])
│   │       ├── Low amplitude: Green
│   │       ├── Medium amplitude: Yellow
│   │       ├── High amplitude: Orange
│   │       └── Critical amplitude: Red
│   └── Draw spectrum line
│       └── draw_spectrum_line(painter, x, start_y + height - bar_height, color)
│
└── Return
```

**Color Mapping:**

```cpp
uint32_t get_spectrum_color(uint8_t amplitude) {
    if (amplitude < 64) {
        return COLOR_LOW_THREAT;        // Green
    } else if (amplitude < 128) {
        return COLOR_MEDIUM_THREAT;     // Yellow
    } else if (amplitude < 192) {
        return COLOR_HIGH_THREAT;       // Orange
    } else {
        return COLOR_CRITICAL_THREAT;   // Red
    }
}
```

### 5.4 Histogram Rendering Flow

**File:** `ui/drone_display.cpp`

**Rendering Flow:**

```
render_histogram(painter, histogram_data, histogram_size, start_x, start_y, width, height)
├── Validate histogram data
│   └── Call validate_histogram_data(histogram_data, histogram_size)
│       ├── Check histogram_data != nullptr
│       ├── Check histogram_size > 0
│       ├── Check histogram_size <= HISTOGRAM_BUFFER_SIZE (128)
│       └── Return INVALID_PARAMETER if invalid
│
├── Draw histogram background
│   └── draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND, true)
│
├── Find maximum value
│   ├── max_value = 0
│   └── For i = 0 to histogram_size-1:
│       └── if (histogram_data[i] > max_value)
│           └── max_value = histogram_data[i]
│
├── Calculate bar width
│   └── bar_width = width / histogram_size
│
├── For i = 0 to histogram_size-1:
│   ├── Calculate x position
│   │   └── x = start_x + (i * bar_width)
│   ├── Calculate bar height
│   │   └── bar_height = (histogram_data[i] * height) / max_value
│   ├── Get color based on bin value
│   │   └── color = get_histogram_color(histogram_data[i], max_value)
│   └── Draw histogram bar
│       └── draw_histogram_bar(painter, x, start_y + height - bar_height, bar_width, bar_height, color)
│
└── Return
```

### 5.5 Drone List Rendering Flow

**File:** `ui/drone_display.cpp`

**Rendering Flow:**

```
render_drone_list(painter, drones, drone_count, start_x, start_y, width, height)
├── Validate drone data
│   └── Call validate_drone_buffer(drones, drone_count, MAX_DISPLAYED_DRONES)
│       ├── Check drones != nullptr
│       ├── Check drone_count > 0
│       ├── Check drone_count <= MAX_DISPLAYED_DRONES (20)
│       └── Return INVALID_PARAMETER if invalid
│
├── Draw list background
│   └── draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND, true)
│
├── Calculate entry height
│   └── entry_height = height / min(drone_count, MAX_DISPLAYED_DRONES)
│
├── For i = 0 to drone_count-1:
│   ├── Calculate y position
│   │   └── y = start_y + (i * entry_height)
│   ├── Get drone color based on threat level
│   │   └── color = get_threat_color(drones[i].threat)
│   ├── Draw drone entry background
│   │   └── draw_rectangle(painter, start_x, y, width, entry_height, color, true)
│   ├── Draw drone type name
│   │   └── draw_text(painter, drones[i].type_name, start_x + 5, y + 5, COLOR_TEXT)
│   ├── Draw frequency
│   │   ├── Format frequency string
│   │   │   └── format_frequency(drones[i].frequency, freq_buffer, 32)
│   │   └── draw_text(painter, freq_buffer, start_x + 80, y + 5, COLOR_TEXT)
│   ├── Draw RSSI
│   │   ├── Format RSSI string
│   │   │   └── format_rssi(drones[i].rssi, rssi_buffer, 32)
│   │   └── draw_text(painter, rssi_buffer, start_x + 160, y + 5, COLOR_TEXT)
│   └── Draw threat level
│       └── draw_text(painter, threat_string, start_x + 200, y + 5, COLOR_TEXT)
│
└── Return
```

**Threat Color Mapping:**

```cpp
uint32_t get_threat_color(ThreatLevel threat) {
    switch (threat) {
        case ThreatLevel::LOW:
            return COLOR_LOW_THREAT;        // Green
        case ThreatLevel::MEDIUM:
            return COLOR_MEDIUM_THREAT;     // Yellow
        case ThreatLevel::HIGH:
            return COLOR_HIGH_THREAT;       // Orange
        case ThreatLevel::CRITICAL:
            return COLOR_CRITICAL_THREAT;   // Red
        default:
            return COLOR_UNKNOWN_THREAT;   // Gray
    }
}
```

### 5.6 Status Bar Rendering Flow

**File:** `ui/drone_scanner_ui.cpp`

**Rendering Flow:**

```
render_status_bar(painter, status_text, start_x, start_y, width, height)
├── Draw status bar background
│   └── draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND, true)
│
├── Draw status text
│   └── draw_text(painter, status_text, start_x + 5, start_y + 5, COLOR_TEXT)
│
├── Draw scanning state
│   ├── if (is_scanning())
│   │   └── draw_text(painter, "Scanning", start_x + 100, start_y + 5, COLOR_TEXT)
│   └── else
│       └── draw_text(painter, "Idle", start_x + 100, start_y + 5, COLOR_TEXT)
│
├── Draw drone count
│   ├── Format count string
│   │   └── snprintf(count_buffer, 32, "%zu drones", tracked_count_)
│   └── draw_text(painter, count_buffer, start_x + 160, start_y + 5, COLOR_TEXT)
│
└── Draw max RSSI
    ├── Format RSSI string
    │   └── snprintf(rssi_buffer, 32, "MAX: %d dBm", statistics_.max_rssi_dbm)
    └── draw_text(painter, rssi_buffer, start_x + 220, start_y + 5, COLOR_TEXT)
```

### 5.7 Display Data Generation from Scanner

**File:** `core/scanner.cpp`

**Data Generation Flow:**

```
get_display_data(display_data)
├── Acquire MutexLock<LockOrder::DATA_MUTEX>
├── Call update_display_data_internal(display_data)
│   ├── Clear display data
│   │   └── display_data.clear()
│   │       ├── Set drone_count = 0
│   │       ├── Clear all drone entries
│   │       └── Reset state snapshot
│   │
│   ├── Copy tracked drones to display entries
│   │   ├── copy_count = min(tracked_count_, MAX_DISPLAYED_DRONES)
│   │   └── for i = 0 to copy_count-1:
│   │       └── display_data.drones[i] = DisplayDroneEntry(tracked_drones_[i])
│   │           ├── Copy frequency
│   │           ├── Copy drone type
│   │           ├── Copy threat level
│   │           ├── Copy RSSI
│   │           ├── Copy last seen timestamp
│   │           ├── Set type name string
│   │           ├── Set display color from threat
│   │           └── Calculate movement trend
│   │
│   ├── Set display_data.drone_count = copy_count
│   │
│   ├── Update state snapshot
│   │   ├── display_data.state.scanning_active = scanning_active_.test()
│   │   ├── display_data.state.is_fresh = true
│   │   ├── Calculate max detected threat
│   │   │   └── max_threat = max(display_data.drones[0..copy_count-1].threat)
│   │   ├── Count movement trends
│   │   │   ├── approaching_count = count(trend == APPROACHING)
│   │   │   ├── static_count = count(trend == STATIC)
│   │   │   └── receding_count = count(trend == RECEDING)
│   │   └── Update state counts
│   │
│   └── Return SUCCESS
│
└── Release mutex
```

### 5.8 Display and Output Testing Checklist

#### UI Update Cycle

- [ ] Test UI update at 60 FPS
- [ ] Test UI update with no drones
- [ ] Test UI update with 1 drone
- [ ] Test UI update with 10 drones
- [ ] Test UI update with 20 drones (max)
- [ ] Verify display data retrieval
- [ ] Verify mutex protection
- [ ] Verify non-blocking updates

#### Spectrum Rendering

- [ ] Test spectrum rendering with valid data
- [ ] Test spectrum rendering with empty data
- [ ] Test spectrum rendering with invalid data
- [ ] Verify color mapping
- [ ] Verify bar height calculation
- [ ] Verify background rendering
- [ ] Test with maximum spectrum size (240)
- [ ] Verify no heap allocation

#### Histogram Rendering

- [ ] Test histogram rendering with valid data
- [ ] Test histogram rendering with empty data
- [ ] Test histogram rendering with invalid data
- [ ] Verify color mapping
- [ ] Verify bar width calculation
- [ ] Verify bar height calculation
- [ ] Test with maximum histogram size (128)
- [ ] Verify no heap allocation

#### Drone List Rendering

- [ ] Test drone list rendering with 1 drone
- [ ] Test drone list rendering with 10 drones
- [ ] Test drone list rendering with 20 drones (max)
- [ ] Verify threat color mapping
- [ ] Verify frequency formatting
- [ ] Verify RSSI formatting
- [ ] Verify threat level display
- [ ] Test with different drone types
- [ ] Test with different threat levels

#### Status Bar Rendering

- [ ] Test status bar with scanning state
- [ ] Test status bar with idle state
- [ ] Test status bar with error state
- [ ] Verify drone count display
- [ ] Verify max RSSI display
- [ ] Verify status text display

#### Display Data Generation

- [ ] Test display data retrieval
- [ ] Test with no tracked drones
- [ ] Test with maximum tracked drones
- [ ] Verify drone copy
- [ ] Verify threat calculation
- [ ] Verify movement trend calculation
- [ ] Verify state snapshot update
- [ ] Verify mutex protection

#### Thread Safety

- [ ] Test concurrent UI updates
- [ ] Test concurrent display data retrieval
- [ ] Test concurrent spectrum updates
- [ ] Test concurrent histogram updates
- [ ] Verify mutex protection
- [ ] Test deadlock scenarios
- [ ] Verify data consistency

#### Performance

- [ ] Measure UI update duration
- [ ] Verify UI update < 16 ms (60 FPS)
- [ ] Measure spectrum rendering time
- [ ] Measure histogram rendering time
- [ ] Measure drone list rendering time
- [ ] Verify no frame drops
- [ ] Test with maximum display data

#### Memory Constraints

- [ ] Verify display_data_ fits in static RAM
- [ ] Verify spectrum_buffer_ fits in static RAM
- [ ] Verify histogram_buffer_ fits in static RAM
- [ ] Verify status_text_ fits in static RAM
- [ ] Verify no heap allocation
- [ ] Verify stack usage < 512 bytes
- [ ] Verify memory leak free

---

## 6. Audio Alert Flow

### 6.1 Audio Alert Architecture

**Audio Alert System:**

```
┌─────────────────────────────────────────────────────────────┐
│              Audio Alert System                           │
├─────────────────────────────────────────────────────────────┤
│                                                          │
│  Alert Conditions:                                        │
│  ┌─────────────────────────────────────────────┐           │
│  │ • Critical threat detected               │           │
│  │ • High threat approaching                │           │
│  │ • New drone detected                    │           │
│  │ • System error                         │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Alert Priority:                                           │
│  ┌─────────────────────────────────────────────┐           │
│  │ 1. CRITICAL (highest)                  │           │
│  │ 2. HIGH                              │           │
│  │ 3. MEDIUM                            │           │
│  │ 4. LOW (lowest)                       │           │
│  └─────────────────────────────────────────────┘           │
│                                                          │
│  Alert Parameters:                                         │
│  • Duration: 500 ms (AUDIO_ALERT_DURATION_MS)             │
│  • Frequency: 1000 Hz (AUDIO_ALERT_FREQUENCY_HZ)          │
│  • Volume: Configurable via settings                       │
│                                                          │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 Audio Alert Triggering Mechanism

**Triggering Flow:**

```
Audio Alert Triggering
├── Monitor scanner state
│   └── Call scanner_.get_display_data(display_data)
│       └── Analyze display_data for alert conditions
│
├── Check alert conditions
│   ├── Check for critical threat
│   │   └── if (display_data.state.max_detected_threat == ThreatLevel::CRITICAL)
│   │       └── Trigger critical alert
│   │           └── play_audio_alert(AudioPriority::CRITICAL)
│   │
│   ├── Check for high threat approaching
│   │   └── if (display_data.state.max_detected_threat == ThreatLevel::HIGH &&
│   │           display_data.state.approaching_count > 0)
│   │       └── Trigger high alert
│   │           └── play_audio_alert(AudioPriority::HIGH)
│   │
│   ├── Check for new drone
│   │   └── if (display_data.drone_count > previous_drone_count)
│   │       └── Trigger new drone alert
│   │           └── play_audio_alert(AudioPriority::MEDIUM)
│   │
│   └── Check for system error
│       └── if (scanner_.get_state() == ScannerState::ERROR)
│           └── Trigger error alert
│               └── play_audio_alert(AudioPriority::HIGH)
│
└── Update previous_drone_count
    └── previous_drone_count = display_data.drone_count
```

### 6.3 Alert Conditions

**Condition 1: Critical Threat Detected**

```cpp
bool check_critical_threat(const DisplayData& display_data) {
    return display_data.state.max_detected_threat == ThreatLevel::CRITICAL;
}
```

**Condition 2: High Threat Approaching**

```cpp
bool check_high_threat_approaching(const DisplayData& display_data) {
    return (display_data.state.max_detected_threat == ThreatLevel::HIGH) &&
           (display_data.state.approaching_count > 0);
}
```

**Condition 3: New Drone Detected**

```cpp
bool check_new_drone(const DisplayData& display_data, size_t previous_count) {
    return display_data.drone_count > previous_count;
}
```

**Condition 4: System Error**

```cpp
bool check_system_error(ScannerState state) {
    return state == ScannerState::ERROR;
}
```

### 6.4 Audio Playback Flow

**Playback Flow:**

```
play_audio_alert(AudioPriority priority)
├── Check if alert already playing
│   └── if (alert_active_)
│       └── Check if new priority > current priority
│           └── if (new_priority > current_priority)
│               └── Override current alert
│           └── else
│               └── Ignore new alert (lower priority)
│   └── else
│       └── Continue with new alert
│
├── Set alert parameters
│   ├── alert_active_ = true
│   ├── alert_priority_ = priority
│   ├── alert_start_time_ = chTimeNow()
│   └── alert_duration_ms_ = AUDIO_ALERT_DURATION_MS
│
├── Configure audio output
│   ├── Set frequency = AUDIO_ALERT_FREQUENCY_HZ (1000 Hz)
│   ├── Set volume = settings_.alert_volume
│   └── Enable audio output
│
├── Start audio playback
│   └── Call audio_output.start_tone(frequency, volume)
│       ├── Generate tone waveform
│       │   └── Sine wave at 1000 Hz
│       ├── Start DMA transfer to audio DAC
│       └── Return SUCCESS
│
└── Schedule alert stop
    └── Set timer for alert_duration_ms (500 ms)
        └── On timer expiry:
            └── stop_audio_alert()
```

### 6.5 Alert Priority Handling

**Priority System:**

```cpp
enum class AudioPriority : uint8_t {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
    CRITICAL = 3
};
```

**Priority Handling Logic:**

```
Alert Priority Handling
├── New alert arrives
├── Check if alert already playing
│   └── if (alert_active_)
│       ├── Compare priorities
│       │   └── if (new_priority > current_priority)
│       │       └── Override current alert
│       │           ├── Stop current alert
│       │           ├── Play new alert
│       │           └── Set current_priority = new_priority
│       │   └── else
│       │       └── Ignore new alert
│   └── else
│       └── Play new alert
│           └── Set current_priority = new_priority
│
└── Continue
```

**Priority Override Examples:**

- **CRITICAL alert playing:** LOW/MEDIUM/HIGH alerts are ignored
- **HIGH alert playing:** LOW/MEDIUM alerts are ignored
- **MEDIUM alert playing:** LOW alerts are ignored
- **No alert playing:** Any alert plays

### 6.6 Audio Alert Testing Checklist

#### Alert Triggering

- [ ] Test critical threat alert
- [ ] Test high threat approaching alert
- [ ] Test new drone detected alert
- [ ] Test system error alert
- [ ] Test multiple alerts simultaneously
- [ ] Verify alert condition detection
- [ ] Verify alert priority assignment

#### Audio Playback

- [ ] Test audio playback start
- [ ] Test audio playback stop
- [ ] Test audio frequency (1000 Hz)
- [ ] Test audio duration (500 ms)
- [ ] Test audio volume settings
- [ ] Verify audio waveform generation
- [ ] Verify DMA transfer

#### Priority Handling

- [ ] Test priority override (CRITICAL > HIGH > MEDIUM > LOW)
- [ ] Test lower priority ignored when higher playing
- [ ] Test higher priority overrides lower
- [ ] Test alert queue (if implemented)
- [ ] Verify current_priority tracking

#### Thread Safety

- [ ] Test concurrent alert triggers
- [ ] Test concurrent alert playback
- [ ] Test concurrent alert stop
- [ ] Verify alert_active_ flag protection
- [ ] Verify priority consistency

#### Performance

- [ ] Measure alert trigger latency
- [ ] Measure audio start latency
- [ ] Verify alert duration accuracy
- [ ] Test with rapid alert triggers
- [ ] Verify no audio glitches

#### Memory Constraints

- [ ] Verify no heap allocation
- [ ] Verify stack usage < 512 bytes
- [ ] Verify audio buffer fits in RAM
- [ ] Verify memory leak free

---

**End of Part 2**

**Continue to Part 3:** Error Handling Flow, Thread Synchronization, Memory Usage Analysis
