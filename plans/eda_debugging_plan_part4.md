# Enhanced Drone Analyzer - Comprehensive Testing and Debugging Plan

**Part 4: Testing Plan**

---

## Table of Contents (Part 4)

10. [Testing Plan](#10-testing-plan)

---

## 10. Testing Plan

### 10.1 Unit Tests for Each Component

#### 10.1.1 DatabaseManager Unit Tests

**Test File:** `firmware/test/database/database_test.cpp`

**Test Cases:**

```cpp
// DatabaseManager Unit Tests
TEST_CASE("DatabaseManager initialization", "[database]") {
    DatabaseManager db;
    
    // Verify initial state
    REQUIRE(db.is_loaded() == false);
    REQUIRE(db.get_database_size() == 0);
    REQUIRE(db.get_current_index().has_value() == false);
}

TEST_CASE("DatabaseManager load default frequencies", "[database]") {
    DatabaseManager db;
    
    // Load database
    ErrorCode result = db.load_frequency_database();
    REQUIRE(result == ErrorCode::SUCCESS);
    REQUIRE(db.is_loaded() == true);
    
    // Verify default frequencies loaded
    REQUIRE(db.get_database_size() == 18);
    
    // Verify first frequency
    ErrorResult<FreqHz> freq_result = db.get_next_frequency(0);
    REQUIRE(freq_result.has_value() == true);
    REQUIRE(freq_result.value() == 2'400'000'000ULL);
}

TEST_CASE("DatabaseManager add entry", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Add new entry
    FrequencyEntry entry(2'410'000'000ULL, DroneType::DJI, 1);
    ErrorCode result = db.add_entry(entry);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify entry added
    REQUIRE(db.get_database_size() == 19);
    
    // Verify entry can be retrieved
    ErrorResult<FrequencyEntry> entry_result = db.find_entry(2'410'000'000ULL);
    REQUIRE(entry_result.has_value() == true);
    REQUIRE(entry_result.value().frequency == 2'410'000'000ULL);
}

TEST_CASE("DatabaseManager add duplicate entry", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Add entry
    FrequencyEntry entry1(2'410'000'000ULL, DroneType::DJI, 1);
    db.add_entry(entry1);
    
    // Add duplicate entry
    FrequencyEntry entry2(2'410'000'000ULL, DroneType::PARROT, 2);
    ErrorCode result = db.add_entry(entry2);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify entry updated (not duplicated)
    REQUIRE(db.get_database_size() == 19);
    
    // Verify entry updated
    ErrorResult<FrequencyEntry> entry_result = db.find_entry(2'410'000'000ULL);
    REQUIRE(entry_result.has_value() == true);
    REQUIRE(entry_result.value().drone_type == DroneType::PARROT);
}

TEST_CASE("DatabaseManager remove entry", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Remove existing entry
    ErrorCode result = db.remove_entry(2'400'000'000ULL);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify entry removed
    REQUIRE(db.get_database_size() == 17);
    
    // Verify entry not found
    ErrorResult<FrequencyEntry> entry_result = db.find_entry(2'400'000'000ULL);
    REQUIRE(entry_result.has_value() == false);
}

TEST_CASE("DatabaseManager remove non-existent entry", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Try to remove non-existent entry
    ErrorCode result = db.remove_entry(9'999'999'999ULL);
    REQUIRE(result == ErrorCode::INVALID_PARAMETER);
    
    // Verify database unchanged
    REQUIRE(db.get_database_size() == 18);
}

TEST_CASE("DatabaseManager clear entries", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Clear all entries
    db.clear_entries();
    
    // Verify database empty
    REQUIRE(db.get_database_size() == 0);
}

TEST_CASE("DatabaseManager get next frequency", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Get first frequency
    ErrorResult<FreqHz> freq1 = db.get_next_frequency(0);
    REQUIRE(freq1.has_value() == true);
    REQUIRE(freq1.value() == 2'400'000'000ULL);
    
    // Get next frequency
    ErrorResult<FreqHz> freq2 = db.get_next_frequency(freq1.value());
    REQUIRE(freq2.has_value() == true);
    REQUIRE(freq2.value() == 2'405'000'000ULL);
    
    // Get next frequency (wrap around)
    ErrorResult<FreqHz> freq_last = db.get_next_frequency(2'480'000'000ULL);
    REQUIRE(freq_last.has_value() == true);
    REQUIRE(freq_last.value() == 2'400'000'000ULL);  // Wrap to first
}

TEST_CASE("DatabaseManager invalid frequency", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Try to get next frequency with invalid current frequency
    ErrorResult<FreqHz> freq = db.get_next_frequency(9'999'999'999ULL);
    REQUIRE(freq.has_value() == true);  // Should start from first
    REQUIRE(freq.value() == 2'400'000'000ULL);
}

TEST_CASE("DatabaseManager thread safety", "[database]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Test concurrent access
    std::thread t1([&db]() {
        for (int i = 0; i < 100; ++i) {
            db.get_next_frequency(0);
        }
    });
    
    std::thread t2([&db]() {
        for (int i = 0; i < 100; ++i) {
            db.add_entry(FrequencyEntry(2'500'000'000ULL + i, DroneType::DJI, 1));
        }
    });
    
    t1.join();
    t2.join();
    
    // Verify no corruption
    REQUIRE(db.get_database_size() <= MAX_DATABASE_ENTRIES);
}
```

**Testing Checklist:**

- [ ] Test initialization
- [ ] Test load default frequencies
- [ ] Test add entry
- [ ] Test add duplicate entry (update)
- [ ] Test remove entry
- [ ] Test remove non-existent entry
- [ ] Test clear entries
- [ ] Test get next frequency
- [ ] Test get next frequency with wrap-around
- [ ] Test invalid frequency
- [ ] Test thread safety (concurrent access)
- [ ] Test lazy loading (double-checked locking)
- [ ] Test database full (MAX_DATABASE_ENTRIES)
- [ ] Test malformed line parsing
- [ ] Test invalid frequency validation
- [ ] Test invalid drone type validation

#### 10.1.2 HardwareController Unit Tests

**Test File:** `firmware/test/hardware/hardware_test.cpp`

**Test Cases:**

```cpp
// HardwareController Unit Tests
TEST_CASE("HardwareController initialization", "[hardware]") {
    HardwareController hw;
    
    // Verify initial state
    REQUIRE(hw.get_state() == HardwareState::UNINITIALIZED);
    REQUIRE(hw.is_ready() == false);
    REQUIRE(hw.is_streaming() == false);
}

TEST_CASE("HardwareController initialize success", "[hardware]") {
    HardwareController hw;
    
    // Initialize
    ErrorCode result = hw.initialize();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify state
    REQUIRE(hw.get_state() == HardwareState::READY);
    REQUIRE(hw.is_ready() == true);
    REQUIRE(hw.is_streaming() == false);
}

TEST_CASE("HardwareController tune to frequency", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    
    // Tune to frequency
    ErrorCode result = hw.tune_to_frequency(2'432'000'000ULL);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify frequency
    ErrorResult<FreqHz> freq = hw.get_current_frequency();
    REQUIRE(freq.has_value() == true);
    REQUIRE(freq.value() == 2'432'000'000ULL);
    
    // Verify PLL locked
    REQUIRE(hw.is_pll_locked() == true);
}

TEST_CASE("HardwareController tune invalid frequency", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    
    // Try to tune to invalid frequency
    ErrorCode result = hw.tune_to_frequency(1'000'000'000ULL);  // Below min
    REQUIRE(result == ErrorCode::INVALID_PARAMETER);
    
    // Verify state unchanged
    REQUIRE(hw.get_state() == HardwareState::READY);
}

TEST_CASE("HardwareController tune retry logic", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    
    // Simulate PLL lock failure
    // (This would need mocking of hardware)
    
    // Tune with retry
    ErrorCode result = hw.tune_to_frequency(2'432'000'000ULL, 3);
    
    // Verify retry logic
    // REQUIRE(result == ErrorCode::SUCCESS || result == ErrorCode::PLL_LOCK_FAILURE);
}

TEST_CASE("HardwareController start streaming", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    
    // Start streaming
    ErrorCode result = hw.start_spectrum_streaming();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify state
    REQUIRE(hw.get_state() == HardwareState::STREAMING);
    REQUIRE(hw.is_streaming() == true);
}

TEST_CASE("HardwareController stop streaming", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    hw.start_spectrum_streaming();
    
    // Stop streaming
    ErrorCode result = hw.stop_spectrum_streaming();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify state
    REQUIRE(hw.get_state() == HardwareState::READY);
    REQUIRE(hw.is_streaming() == false);
}

TEST_CASE("HardwareController get RSSI sample", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    hw.start_spectrum_streaming();
    
    // Get RSSI sample
    ErrorResult<RssiSample> sample_result = hw.get_rssi_sample();
    REQUIRE(sample_result.has_value() == true);
    
    // Verify sample
    const RssiSample& sample = sample_result.value();
    REQUIRE(sample.rssi >= RSSI_MIN_DBM);
    REQUIRE(sample.rssi <= RSSI_MAX_DBM);
    REQUIRE(sample.frequency > 0);
}

TEST_CASE("HardwareController set gain", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    
    // Set gain
    ErrorCode result = hw.set_gain(30);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify gain
    REQUIRE(hw.get_gain() == 30);
}

TEST_CASE("HardwareController set invalid gain", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    
    // Try to set invalid gain
    ErrorCode result = hw.set_gain(100);  // Above max
    REQUIRE(result == ErrorCode::INVALID_PARAMETER);
    
    // Verify gain unchanged
    REQUIRE(hw.get_gain() == 20);  // Default
}

TEST_CASE("HardwareController reset", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    hw.start_spectrum_streaming();
    hw.set_gain(30);
    
    // Reset
    ErrorCode result = hw.reset();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify state reset
    REQUIRE(hw.get_state() == HardwareState::READY);
    REQUIRE(hw.is_streaming() == false);
    REQUIRE(hw.get_gain() == 20);  // Default
}

TEST_CASE("HardwareController thread safety", "[hardware]") {
    HardwareController hw;
    hw.initialize();
    
    // Test concurrent operations
    std::thread t1([&hw]() {
        for (int i = 0; i < 100; ++i) {
            hw.tune_to_frequency(2'400'000'000ULL + i);
        }
    });
    
    std::thread t2([&hw]() {
        for (int i = 0; i < 100; ++i) {
            hw.get_rssi_sample();
        }
    });
    
    t1.join();
    t2.join();
    
    // Verify no corruption
    REQUIRE(hw.get_state() == HardwareState::READY);
}
```

**Testing Checklist:**

- [ ] Test initialization
- [ ] Test initialize success
- [ ] Test initialize failure (mock hardware failure)
- [ ] Test tune to frequency
- [ ] Test tune invalid frequency
- [ ] Test tune retry logic (3 retries)
- [ ] Test PLL lock verification
- [ ] Test PLL lock timeout
- [ ] Test start streaming
- [ ] Test stop streaming
- [ ] Test get RSSI sample
- [ ] Test RSSI value range (-120 to -20 dBm)
- [ ] Test set gain
- [ ] Test set invalid gain
- [ ] Test reset
- [ ] Test thread safety (concurrent operations)
- [ ] Test state transitions
- [ ] Test error codes
- [ ] Test last error tracking

#### 10.1.3 DroneScanner Unit Tests

**Test File:** `firmware/test/scanner/scanner_test.cpp`

**Test Cases:**

```cpp
// DroneScanner Unit Tests
TEST_CASE("DroneScanner initialization", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Verify initial state
    REQUIRE(scanner.get_state() == ScannerState::IDLE);
    REQUIRE(scanner.is_scanning() == false);
    REQUIRE(scanner.get_tracked_count() == 0);
}

TEST_CASE("DroneScanner initialize", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Initialize
    ErrorCode result = scanner.initialize();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify state
    REQUIRE(scanner.get_state() == ScannerState::IDLE);
    REQUIRE(scanner.get_tracked_count() == 0);
}

TEST_CASE("DroneScanner start scanning", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Start scanning
    ErrorCode result = scanner.start_scanning();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify state
    REQUIRE(scanner.get_state() == ScannerState::SCANNING);
    REQUIRE(scanner.is_scanning() == true);
}

TEST_CASE("DroneScanner stop scanning", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    scanner.start_scanning();
    
    // Stop scanning
    ErrorCode result = scanner.stop_scanning();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify state
    REQUIRE(scanner.get_state() == ScannerState::IDLE);
    REQUIRE(scanner.is_scanning() == false);
}

TEST_CASE("DroneScanner pause/resume scanning", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    scanner.start_scanning();
    
    // Pause scanning
    ErrorCode pause_result = scanner.pause_scanning();
    REQUIRE(pause_result == ErrorCode::SUCCESS);
    REQUIRE(scanner.get_state() == ScannerState::PAUSED);
    
    // Resume scanning
    ErrorCode resume_result = scanner.resume_scanning();
    REQUIRE(resume_result == ErrorCode::SUCCESS);
    REQUIRE(scanner.get_state() == ScannerState::SCANNING);
}

TEST_CASE("DroneScanner perform scan cycle", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    scanner.start_scanning();
    
    // Perform scan cycle
    ErrorCode result = scanner.perform_scan_cycle();
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify statistics
    ScanStatistics stats = scanner.get_statistics();
    REQUIRE(stats.total_scan_cycles > 0);
}

TEST_CASE("DroneScanner add tracked drone", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Add tracked drone
    SystemTime now = 0;
    ErrorCode result = scanner.update_tracked_drones(
        2'432'000'000ULL,
        -60,
        now
    );
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify drone added
    REQUIRE(scanner.get_tracked_count() == 1);
    
    // Verify drone data
    TrackedDrone drones[1];
    size_t count = scanner.get_tracked_drones(drones, 1);
    REQUIRE(count == 1);
    REQUIRE(drones[0].frequency == 2'432'000'000ULL);
    REQUIRE(drones[0].rssi == -60);
}

TEST_CASE("DroneScanner update existing drone", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Add drone
    SystemTime now = 0;
    scanner.update_tracked_drones(2'432'000'000ULL, -60, now);
    
    // Update drone
    now = 100;
    ErrorCode result = scanner.update_tracked_drones(2'432'000'000ULL, -50, now);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify drone updated
    TrackedDrone drones[1];
    scanner.get_tracked_drones(drones, 1);
    REQUIRE(drones[0].rssi == -50);
    REQUIRE(drones[0].last_seen == 100);
}

TEST_CASE("DroneScanner tracked drones overflow", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Add maximum drones
    for (size_t i = 0; i < MAX_TRACKED_DRONES; ++i) {
        scanner.update_tracked_drones(
            2'400'000'000ULL + i,
            -60,
            i
        );
    }
    
    REQUIRE(scanner.get_tracked_count() == MAX_TRACKED_DRONES);
    
    // Try to add one more
    ErrorCode result = scanner.update_tracked_drones(
        2'500'000'000ULL,
        -60,
        MAX_TRACKED_DRONES
    );
    REQUIRE(result == ErrorCode::BUFFER_FULL);
    
    // Verify count unchanged
    REQUIRE(scanner.get_tracked_count() == MAX_TRACKED_DRONES);
}

TEST_CASE("DroneScanner remove stale drones", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Add drone
    SystemTime now = 0;
    scanner.update_tracked_drones(2'432'000'000ULL, -60, now);
    
    // Wait for stale timeout
    now = DRONE_STALE_TIMEOUT_MS + 100;
    
    // Remove stale drones
    scanner.remove_stale_drones(now);
    
    // Verify drone removed
    REQUIRE(scanner.get_tracked_count() == 0);
}

TEST_CASE("DroneScanner get display data", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Add some drones
    for (size_t i = 0; i < 5; ++i) {
        scanner.update_tracked_drones(
            2'400'000'000ULL + i,
            -60,
            i
        );
    }
    
    // Get display data
    DisplayData display_data;
    ErrorCode result = scanner.get_display_data(display_data);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Verify display data
    REQUIRE(display_data.drone_count == 5);
    REQUIRE(display_data.state.scanning_active == false);
}

TEST_CASE("DroneScanner thread safety", "[scanner]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    scanner.start_scanning();
    
    // Test concurrent operations
    std::thread t1([&scanner]() {
        for (int i = 0; i < 100; ++i) {
            scanner.perform_scan_cycle();
        }
    });
    
    std::thread t2([&scanner]() {
        for (int i = 0; i < 100; ++i) {
            scanner.get_display_data();
        }
    });
    
    t1.join();
    t2.join();
    
    // Verify no corruption
    REQUIRE(scanner.get_state() == ScannerState::SCANNING);
}
```

**Testing Checklist:**

- [ ] Test initialization
- [ ] Test initialize success
- [ ] Test initialize failure (database/hardware)
- [ ] Test start scanning
- [ ] Test stop scanning
- [ ] Test pause/resume scanning
- [ ] Test perform scan cycle
- [ ] Test add tracked drone
- [ ] Test update existing drone
- [ ] Test tracked drones overflow
- [ ] Test remove stale drones
- [ ] Test get display data
- [ ] Test drone type classification
- [ ] Test threat level calculation
- [ ] Test statistics tracking
- [ ] Test thread safety (concurrent operations)
- [ ] Test state transitions
- [ ] Test error codes

#### 10.1.4 DSP Processors Unit Tests

**Test File:** `firmware/test/dsp/dsp_test.cpp`

**Test Cases:**

```cpp
// SpectrumProcessor Unit Tests
TEST_CASE("SpectrumProcessor initialization", "[spectrum]") {
    SpectrumProcessor processor;
    
    // Initialize
    ErrorCode result = processor.initialize();
    REQUIRE(result == ErrorCode::SUCCESS);
}

TEST_CASE("SpectrumProcessor process spectrum data", "[spectrum]") {
    SpectrumProcessor processor;
    processor.initialize();
    
    // Create test spectrum data
    uint8_t spectrum_data[128];
    for (size_t i = 0; i < 128; ++i) {
        spectrum_data[i] = i * 2;  // Linear ramp
    }
    
    // Process spectrum data
    SpectrumResult result;
    ErrorCode code = processor.process_spectrum_data(
        spectrum_data,
        128,
        2'400'000'000ULL,
        1'000'000ULL,
        result
    );
    REQUIRE(code == ErrorCode::SUCCESS);
    
    // Verify result
    REQUIRE(result.signal_detected == true);
    REQUIRE(result.peak_amplitude > 0);
}

TEST_CASE("SpectrumProcessor calculate peak frequency", "[spectrum]") {
    SpectrumProcessor processor;
    processor.initialize();
    
    // Create test spectrum data with peak at index 64
    uint8_t spectrum_data[128];
    for (size_t i = 0; i < 128; ++i) {
        spectrum_data[i] = (i == 64) ? 255 : 0;
    }
    
    // Calculate peak frequency
    FreqHz peak_freq;
    uint16_t peak_amp;
    ErrorCode code = processor.calculate_peak_frequency(
        spectrum_data,
        128,
        2'400'000'000ULL,
        1'000'000ULL,
        peak_freq,
        peak_amp
    );
    REQUIRE(code == ErrorCode::SUCCESS);
    
    // Verify peak
    REQUIRE(peak_freq == 2'400'000'000ULL + 64 * 1'000'000ULL);
    REQUIRE(peak_amp == 255);
}

TEST_CASE("SpectrumProcessor calculate average power", "[spectrum]") {
    SpectrumProcessor processor;
    processor.initialize();
    
    // Create test spectrum data
    uint8_t spectrum_data[128];
    for (size_t i = 0; i < 128; ++i) {
        spectrum_data[i] = 128;  // Constant
    }
    
    // Calculate average power
    uint16_t avg_power;
    ErrorCode code = processor.calculate_average_power(spectrum_data, 128, avg_power);
    REQUIRE(code == ErrorCode::SUCCESS);
    
    // Verify average
    REQUIRE(avg_power == 128);
}

// RSSIDetector Unit Tests
TEST_CASE("RSSIDetector initialization", "[rssi]") {
    RSSIDetector detector;
    
    // Initialize
    ErrorCode result = detector.initialize(-90);
    REQUIRE(result == ErrorCode::SUCCESS);
}

TEST_CASE("RSSIDetector process RSSI sample", "[rssi]") {
    RSSIDetector detector;
    detector.initialize(-90);
    
    // Process RSSI samples
    SystemTime now = 0;
    detector.process_rssi_sample(-60, now);
    now += 100;
    detector.process_rssi_sample(-55, now);
    now += 100;
    detector.process_rssi_sample(-50, now);
    
    // Detect drone
    RSSIDetectionResult result;
    ErrorCode code = detector.detect_drone(result);
    REQUIRE(code == ErrorCode::SUCCESS);
    
    // Verify detection
    REQUIRE(result.drone_detected == true);
    REQUIRE(result.threat_level == ThreatLevel::HIGH);
}

TEST_CASE("RSSIDetector calculate threat level", "[rssi]") {
    RSSIDetector detector;
    detector.initialize(-90);
    
    // Test critical threat
    ThreatLevel threat1 = detector.calculate_threat_level(-40, -45);
    REQUIRE(threat1 == ThreatLevel::CRITICAL);
    
    // Test high threat
    ThreatLevel threat2 = detector.calculate_threat_level(-60, -65);
    REQUIRE(threat2 == ThreatLevel::HIGH);
    
    // Test medium threat
    ThreatLevel threat3 = detector.calculate_threat_level(-80, -85);
    REQUIRE(threat3 == ThreatLevel::MEDIUM);
    
    // Test low threat
    ThreatLevel threat4 = detector.calculate_threat_level(-95, -100);
    REQUIRE(threat4 == ThreatLevel::LOW);
}

TEST_CASE("RSSIDetector movement trend", "[rssi]") {
    RSSIDetector detector;
    detector.initialize(-90);
    
    // Process approaching drone
    SystemTime now = 0;
    detector.process_rssi_sample(-80, now);
    now += 100;
    detector.process_rssi_sample(-70, now);
    now += 100;
    detector.process_rssi_sample(-60, now);
    
    // Get movement trend
    MovementTrend trend = detector.get_movement_trend();
    REQUIRE(trend == MovementTrend::APPROACHING);
}
```

**Testing Checklist:**

- [ ] Test SpectrumProcessor initialization
- [ ] Test SpectrumProcessor process spectrum data
- [ ] Test SpectrumProcessor calculate peak frequency
- [ ] Test SpectrumProcessor calculate average power
- [ ] Test SpectrumProcessor detect signal
- [ ] Test SpectrumProcessor calculate noise floor
- [ ] Test RSSIDetector initialization
- [ ] Test RSSIDetector process RSSI sample
- [ ] Test RSSIDetector detect drone
- [ ] Test RSSIDetector calculate threat level
- [ ] Test RSSIDetector movement trend
- [ ] Test RSSIDetector statistics
- [ ] Test no floating-point operations
- [ ] Test integer arithmetic accuracy

### 10.2 Integration Tests for Component Interactions

#### 10.2.1 Database + Hardware Integration

**Test File:** `firmware/test/integration/database_hardware_test.cpp`

**Test Cases:**

```cpp
// Database + Hardware Integration Tests
TEST_CASE("Database + Hardware: Initialize and scan", "[integration]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Initialize
    ErrorCode init_result = scanner.initialize();
    REQUIRE(init_result == ErrorCode::SUCCESS);
    
    // Start scanning
    ErrorCode start_result = scanner.start_scanning();
    REQUIRE(start_result == ErrorCode::SUCCESS);
    
    // Perform scan cycles
    for (int i = 0; i < 10; ++i) {
        ErrorCode scan_result = scanner.perform_scan_cycle();
        REQUIRE(scan_result == ErrorCode::SUCCESS);
    }
    
    // Stop scanning
    ErrorCode stop_result = scanner.stop_scanning();
    REQUIRE(stop_result == ErrorCode::SUCCESS);
}

TEST_CASE("Database + Hardware: Database empty", "[integration]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Clear database
    db.clear_entries();
    
    // Initialize
    ErrorCode result = scanner.initialize();
    REQUIRE(result == ErrorCode::DATABASE_EMPTY);
    
    // Verify scanner uses default frequency
    ErrorResult<FreqHz> freq = scanner.get_current_frequency();
    REQUIRE(freq.has_value() == true);
    REQUIRE(freq.value() == DEFAULT_SCAN_FREQUENCY_HZ);
}

TEST_CASE("Database + Hardware: Hardware failure", "[integration]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Initialize
    scanner.initialize();
    
    // Simulate hardware failure
    // (This would need mocking of hardware)
    
    // Perform scan cycle
    ErrorCode scan_result = scanner.perform_scan_cycle();
    
    // Verify error handling
    // REQUIRE(scan_result == ErrorCode::HARDWARE_FAILURE);
}
```

#### 10.2.2 Scanner + UI Integration

**Test File:** `firmware/test/integration/scanner_ui_test.cpp`

**Test Cases:**

```cpp
// Scanner + UI Integration Tests
TEST_CASE("Scanner + UI: Display data flow", "[integration]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    DroneDisplay display;
    
    // Initialize
    scanner.initialize();
    scanner.start_scanning();
    
    // Add some drones
    for (size_t i = 0; i < 5; ++i) {
        scanner.update_tracked_drones(
            2'400'000'000ULL + i,
            -60,
            i
        );
    }
    
    // Get display data from scanner
    DisplayData display_data;
    ErrorCode result = scanner.get_display_data(display_data);
    REQUIRE(result == ErrorCode::SUCCESS);
    
    // Update display
    ErrorCode update_result = display.update_display_data(display_data);
    REQUIRE(update_result == ErrorCode::SUCCESS);
    
    // Verify display data
    REQUIRE(display.get_display_data().drone_count == 5);
}

TEST_CASE("Scanner + UI: Real-time updates", "[integration]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    DroneDisplay display;
    
    // Initialize
    scanner.initialize();
    scanner.start_scanning();
    
    // Simulate real-time updates
    for (int i = 0; i < 60; ++i) {  // 1 second at 60 FPS
        // Perform scan cycle
        scanner.perform_scan_cycle();
        
        // Get display data
        DisplayData display_data;
        scanner.get_display_data(display_data);
        
        // Update display
        display.update_display_data(display_data);
        
        // Verify no errors
        REQUIRE(display.get_display_data().drone_count <= MAX_DISPLAYED_DRONES);
    }
}

TEST_CASE("Scanner + UI: Thread safety", "[integration]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    DroneDisplay display;
    
    // Initialize
    scanner.initialize();
    scanner.start_scanning();
    
    // Test concurrent scanner and UI updates
    std::thread scanner_thread([&scanner]() {
        for (int i = 0; i < 100; ++i) {
            scanner.perform_scan_cycle();
        }
    });
    
    std::thread ui_thread([&scanner, &display]() {
        for (int i = 0; i < 100; ++i) {
            DisplayData display_data;
            scanner.get_display_data(display_data);
            display.update_display_data(display_data);
        }
    });
    
    scanner_thread.join();
    ui_thread.join();
    
    // Verify no corruption
    REQUIRE(scanner.get_tracked_count() <= MAX_TRACKED_DRONES);
}
```

**Testing Checklist:**

- [ ] Test Database + Hardware integration
- [ ] Test Database + Hardware with empty database
- [ ] Test Database + Hardware with hardware failure
- [ ] Test Scanner + UI integration
- [ ] Test Scanner + UI real-time updates
- [ ] Test Scanner + UI thread safety
- [ ] Test component interaction error handling
- [ ] Test component interaction recovery
- [ ] Test component interaction graceful degradation

### 10.3 System Tests for Full Application Flow

#### 10.3.1 Full Application Lifecycle Test

**Test File:** `firmware/test/system/lifecycle_test.cpp`

**Test Cases:**

```cpp
// Full Application Lifecycle Tests
TEST_CASE("Application lifecycle: Initialize to shutdown", "[system]") {
    // Create components
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    DroneDisplay display;
    
    // Phase 1: Critical initialization
    // (Mutexes and atomic flags)
    
    // Phase 2: Core services initialization
    ErrorCode db_result = db.load_frequency_database();
    REQUIRE(db_result == ErrorCode::SUCCESS);
    
    ErrorCode hw_result = hw.initialize();
    REQUIRE(hw_result == ErrorCode::SUCCESS);
    
    ErrorCode scanner_result = scanner.initialize();
    REQUIRE(scanner_result == ErrorCode::SUCCESS);
    
    // Phase 3: Lazy initialization
    // (DSP processors and UI components)
    
    // Start scanning
    ErrorCode start_result = scanner.start_scanning();
    REQUIRE(start_result == ErrorCode::SUCCESS);
    
    // Run for some time
    for (int i = 0; i < 100; ++i) {
        scanner.perform_scan_cycle();
        
        DisplayData display_data;
        scanner.get_display_data(display_data);
        display.update_display_data(display_data);
    }
    
    // Stop scanning
    ErrorCode stop_result = scanner.stop_scanning();
    REQUIRE(stop_result == ErrorCode::SUCCESS);
    
    // Shutdown
    ErrorCode shutdown_result = hw.shutdown();
    REQUIRE(shutdown_result == ErrorCode::SUCCESS);
}

TEST_CASE("Application lifecycle: Error recovery", "[system]") {
    // Create components
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Initialize
    scanner.initialize();
    
    // Simulate error during scan cycle
    // (This would need mocking of hardware)
    
    // Verify error recovery
    // REQUIRE(scanner.get_state() == ScannerState::ERROR);
    
    // Verify graceful degradation
    // REQUIRE(scanner.is_scanning() == false);
}

TEST_CASE("Application lifecycle: Stress test", "[system]") {
    // Create components
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    DroneDisplay display;
    
    // Initialize
    scanner.initialize();
    scanner.start_scanning();
    
    // Stress test: Run for extended period
    for (int i = 0; i < 10000; ++i) {  // ~1000 seconds
        scanner.perform_scan_cycle();
        
        DisplayData display_data;
        scanner.get_display_data(display_data);
        display.update_display_data(display_data);
        
        // Verify no memory leaks
        // REQUIRE(scanner.get_tracked_count() <= MAX_TRACKED_DRONES);
    }
    
    // Stop scanning
    scanner.stop_scanning();
}
```

**Testing Checklist:**

- [ ] Test full application lifecycle
- [ ] Test initialization sequence
- [ ] Test startup flow
- [ ] Test normal operation
- [ ] Test shutdown sequence
- [ ] Test error recovery
- [ ] Test graceful degradation
- [ ] Test stress test (extended operation)
- [ ] Test memory stability
- [ ] Test no memory leaks
- [ ] Test no deadlocks

#### 10.3.2 Real-Time Operation Test

**Test File:** `firmware/test/system/realtime_test.cpp`

**Test Cases:**

```cpp
// Real-Time Operation Tests
TEST_CASE("Real-time: Scan cycle timing", "[realtime]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Initialize
    scanner.initialize();
    scanner.start_scanning();
    
    // Measure scan cycle time
    SystemTime start = chTimeNow();
    scanner.perform_scan_cycle();
    SystemTime end = chTimeNow();
    
    SystemTime duration = end - start;
    
    // Verify scan cycle < 100 ms
    REQUIRE(duration <= SCAN_CYCLE_INTERVAL_MS);
}

TEST_CASE("Real-time: UI update timing", "[realtime]") {
    DroneDisplay display;
    
    // Measure UI update time
    SystemTime start = chTimeNow();
    
    DisplayData display_data;
    display.update_display_data(display_data);
    
    SystemTime end = chTimeNow();
    
    SystemTime duration = end - start;
    
    // Verify UI update < 16 ms (60 FPS)
    REQUIRE(duration <= UI_REFRESH_INTERVAL_MS);
}

TEST_CASE("Real-time: Frequency tuning timing", "[realtime]") {
    HardwareController hw;
    hw.initialize();
    
    // Measure tuning time
    SystemTime start = chTimeNow();
    hw.tune_to_frequency(2'432'000'000ULL);
    SystemTime end = chTimeNow();
    
    SystemTime duration = end - start;
    
    // Verify tuning < 50 ms (including PLL lock)
    REQUIRE(duration <= 50);
}

TEST_CASE("Real-time: Continuous operation", "[realtime]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    DroneDisplay display;
    
    // Initialize
    scanner.initialize();
    scanner.start_scanning();
    
    // Run continuous operation
    for (int i = 0; i < 1000; ++i) {  // ~100 seconds
        SystemTime cycle_start = chTimeNow();
        
        scanner.perform_scan_cycle();
        
        SystemTime cycle_end = chTimeNow();
        SystemTime cycle_duration = cycle_end - cycle_start;
        
        // Verify real-time constraint
        REQUIRE(cycle_duration <= SCAN_CYCLE_INTERVAL_MS);
        
        // Update UI
        DisplayData display_data;
        scanner.get_display_data(display_data);
        display.update_display_data(display_data);
    }
    
    scanner.stop_scanning();
}
```

**Testing Checklist:**

- [ ] Test scan cycle timing (< 100 ms)
- [ ] Test UI update timing (< 16 ms)
- [ ] Test frequency tuning timing (< 50 ms)
- [ ] Test RSSI sampling timing
- [ ] Test continuous operation
- [ ] Verify real-time constraints
- [ ] Test frame rate stability
- [ ] Test no frame drops

### 10.4 Performance Tests for Real-Time Constraints

#### 10.4.1 Scan Rate Performance

**Test File:** `firmware/test/performance/scan_rate_test.cpp`

**Test Cases:**

```cpp
// Scan Rate Performance Tests
TEST_CASE("Performance: Scan rate", "[performance]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Initialize
    scanner.initialize();
    scanner.start_scanning();
    
    // Measure scan rate
    uint32_t total_cycles = 0;
    SystemTime start = chTimeNow();
    
    for (int i = 0; i < 1000; ++i) {
        scanner.perform_scan_cycle();
        total_cycles++;
    }
    
    SystemTime end = chTimeNow();
    SystemTime duration_ms = (end - start) / 1000;  // Convert to ms
    
    // Calculate scan rate
    float scan_rate = (float)total_cycles / (duration_ms / 1000.0f);
    
    // Verify scan rate >= 10 Hz
    REQUIRE(scan_rate >= TARGET_SCAN_RATE_HZ);
}

TEST_CASE("Performance: Database iteration", "[performance]") {
    DatabaseManager db;
    db.load_frequency_database();
    
    // Measure iteration time
    SystemTime start = chTimeNow();
    
    for (size_t i = 0; i < db.get_database_size(); ++i) {
        ErrorResult<FreqHz> freq = db.get_next_frequency(0);
        (void)freq;
    }
    
    SystemTime end = chTimeNow();
    
    // Verify iteration time reasonable
    SystemTime duration = end - start;
    REQUIRE(duration < 10);  // < 10 ms for full iteration
}

TEST_CASE("Performance: Drone tracking", "[performance]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    
    // Initialize
    scanner.initialize();
    
    // Add maximum drones
    for (size_t i = 0; i < MAX_TRACKED_DRONES; ++i) {
        scanner.update_tracked_drones(
            2'400'000'000ULL + i,
            -60,
            i
        );
    }
    
    // Measure tracking performance
    SystemTime start = chTimeNow();
    
    for (int i = 0; i < 100; ++i) {
        scanner.update_tracked_drones(
            2'400'000'000ULL,
            -60,
            i
        );
    }
    
    SystemTime end = chTimeNow();
    
    // Verify tracking time reasonable
    SystemTime duration = end - start;
    REQUIRE(duration < 50);  // < 50 ms for 100 updates
}
```

**Testing Checklist:**

- [ ] Test scan rate performance
- [ ] Test database iteration performance
- [ ] Test drone tracking performance
- [ ] Test display update performance
- [ ] Test spectrum processing performance
- [ ] Test histogram processing performance
- [ ] Verify real-time constraints
- [ ] Test with maximum load
- [ ] Test with minimum load

### 10.5 Memory Usage Tests

#### 10.5.1 Static Memory Usage Test

**Test File:** `firmware/test/memory/static_memory_test.cpp`

**Test Cases:**

```cpp
// Static Memory Usage Tests
TEST_CASE("Memory: DatabaseManager static usage", "[memory]") {
    // Measure DatabaseManager static storage
    constexpr size_t expected_size = 
        sizeof(std::array<FrequencyEntry, MAX_DATABASE_ENTRIES>) +
        sizeof(size_t) * 2 +  // current_index_, entry_count_
        sizeof(AtomicFlag) +
        sizeof(mutex_t*) +
        DATABASE_LINE_BUFFER_SIZE;
    
    // Verify size within budget
    REQUIRE(expected_size <= 8200);  // Approximate budget
}

TEST_CASE("Memory: HardwareController static usage", "[memory]") {
    // Measure HardwareController static storage
    constexpr size_t expected_size = 
        sizeof(HardwareState) +
        sizeof(HardwareConfig) +
        sizeof(FreqHz) +
        sizeof(ErrorCode) +
        sizeof(uint32_t) +
        sizeof(AtomicFlag) * 2 +
        sizeof(mutex_t*);
    
    // Verify size within budget
    REQUIRE(expected_size <= 100);  // Approximate budget
}

TEST_CASE("Memory: DroneScanner static usage", "[memory]") {
    // Measure DroneScanner static storage
    constexpr size_t expected_size = 
        sizeof(DatabaseManager&) +
        sizeof(HardwareController&) +
        sizeof(ScannerState) +
        sizeof(ScanConfig) +
        sizeof(ScanStatistics) +
        sizeof(std::array<TrackedDrone, MAX_TRACKED_DRONES>) +
        sizeof(size_t) * 2 +
        sizeof(FreqHz) +
        sizeof(SystemTime) +
        sizeof(AtomicFlag) +
        sizeof(mutex_t*);
    
    // Verify size within budget
    REQUIRE(expected_size <= 1000);  // Approximate budget
}

TEST_CASE("Memory: DroneDisplay static usage", "[memory]") {
    // Measure DroneDisplay static storage
    constexpr size_t expected_size = 
        sizeof(DisplayData) +
        sizeof(std::array<uint8_t, SPECTRUM_BUFFER_SIZE>) +
        sizeof(std::array<uint16_t, HISTOGRAM_BUFFER_SIZE>) +
        MAX_TEXT_LENGTH +
        sizeof(size_t) * 2 +
        sizeof(bool) * 4 +
        sizeof(uint16_t) * 4;
    
    // Verify size within budget
    REQUIRE(expected_size <= 1500);  // Approximate budget
}
```

#### 10.5.2 Stack Usage Test

**Test File:** `firmware/test/memory/stack_usage_test.cpp`

**Test Cases:**

```cpp
// Stack Usage Tests
extern "C" size_t get_stack_usage();

TEST_CASE("Memory: Stack usage - DatabaseManager", "[memory]") {
    DatabaseManager db;
    
    // Measure stack usage
    size_t stack_before = get_stack_usage();
    db.load_frequency_database();
    size_t stack_after = get_stack_usage();
    
    size_t stack_used = stack_after - stack_before;
    
    // Verify stack usage within budget
    REQUIRE(stack_used <= 512);  // MAX_STACK_PER_FUNCTION
}

TEST_CASE("Memory: Stack usage - HardwareController", "[memory]") {
    HardwareController hw;
    hw.initialize();
    
    // Measure stack usage
    size_t stack_before = get_stack_usage();
    hw.tune_to_frequency(2'432'000'000ULL);
    size_t stack_after = get_stack_usage();
    
    size_t stack_used = stack_after - stack_before;
    
    // Verify stack usage within budget
    REQUIRE(stack_used <= 512);
}

TEST_CASE("Memory: Stack usage - DroneScanner", "[memory]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    scanner.start_scanning();
    
    // Measure stack usage
    size_t stack_before = get_stack_usage();
    scanner.perform_scan_cycle();
    size_t stack_after = get_stack_usage();
    
    size_t stack_used = stack_after - stack_before;
    
    // Verify stack usage within budget
    REQUIRE(stack_used <= 512);
}

TEST_CASE("Memory: Stack usage - DroneDisplay", "[memory]") {
    DroneDisplay display;
    
    // Create test display data
    DisplayData display_data;
    display_data.drone_count = 5;
    
    // Measure stack usage
    size_t stack_before = get_stack_usage();
    display.update_display_data(display_data);
    size_t stack_after = get_stack_usage();
    
    size_t stack_used = stack_after - stack_before;
    
    // Verify stack usage within budget
    REQUIRE(stack_used <= 512);
}
```

**Testing Checklist:**

- [ ] Test DatabaseManager static usage
- [ ] Test HardwareController static usage
- [ ] Test DroneScanner static usage
- [ ] Test DroneDisplay static usage
- [ ] Test stack usage per function
- [ ] Verify total static < 4504 bytes
- [ ] Verify stack < 512 bytes per function
- [ ] Test no heap allocation
- [ ] Test memory alignment

### 10.6 Stress Tests for Edge Cases

#### 10.6.1 Edge Case Tests

**Test File:** `firmware/test/stress/edge_case_test.cpp`

**Test Cases:**

```cpp
// Edge Case Tests
TEST_CASE("Stress: Empty database", "[stress]") {
    DatabaseManager db;
    db.clear_entries();
    
    // Initialize scanner with empty database
    HardwareController hw;
    DroneScanner scanner(db, hw);
    ErrorCode result = scanner.initialize();
    
    // Verify graceful degradation
    REQUIRE(result == ErrorCode::DATABASE_EMPTY);
    
    // Verify default frequency used
    ErrorResult<FreqHz> freq = scanner.get_current_frequency();
    REQUIRE(freq.has_value() == true);
    REQUIRE(freq.value() == DEFAULT_SCAN_FREQUENCY_HZ);
}

TEST_CASE("Stress: Maximum database size", "[stress]") {
    DatabaseManager db;
    db.clear_entries();
    
    // Add maximum entries
    for (size_t i = 0; i < MAX_DATABASE_ENTRIES; ++i) {
        FrequencyEntry entry(2'400'000'000ULL + i, DroneType::DJI, 1);
        db.add_entry(entry);
    }
    
    REQUIRE(db.get_database_size() == MAX_DATABASE_ENTRIES);
    
    // Try to add one more
    FrequencyEntry extra_entry(2'500'000'000ULL, DroneType::DJI, 1);
    ErrorCode result = db.add_entry(extra_entry);
    
    // Verify buffer full
    REQUIRE(result == ErrorCode::BUFFER_FULL);
}

TEST_CASE("Stress: Maximum tracked drones", "[stress]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Add maximum drones
    for (size_t i = 0; i < MAX_TRACKED_DRONES; ++i) {
        scanner.update_tracked_drones(
            2'400'000'000ULL + i,
            -60,
            i
        );
    }
    
    REQUIRE(scanner.get_tracked_count() == MAX_TRACKED_DRONES);
    
    // Try to add one more
    ErrorCode result = scanner.update_tracked_drones(
        2'500'000'000ULL,
        -60,
        MAX_TRACKED_DRONES
    );
    
    // Verify buffer full
    REQUIRE(result == ErrorCode::BUFFER_FULL);
}

TEST_CASE("Stress: Rapid frequency changes", "[stress]") {
    HardwareController hw;
    hw.initialize();
    
    // Rapid frequency changes
    for (int i = 0; i < 100; ++i) {
        ErrorCode result = hw.tune_to_frequency(2'400'000'000ULL + i);
        REQUIRE(result == ErrorCode::SUCCESS || result == ErrorCode::PLL_LOCK_FAILURE);
    }
}

TEST_CASE("Stress: All drones stale", "[stress]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Add drones
    for (size_t i = 0; i < 5; ++i) {
        scanner.update_tracked_drones(
            2'400'000'000ULL + i,
            -60,
            i
        );
    }
    
    // Wait for stale timeout
    SystemTime now = DRONE_STALE_TIMEOUT_MS + 100;
    
    // Remove stale drones
    scanner.remove_stale_drones(now);
    
    // Verify all removed
    REQUIRE(scanner.get_tracked_count() == 0);
}

TEST_CASE("Stress: Concurrent operations", "[stress]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    scanner.start_scanning();
    
    // Concurrent operations
    std::thread t1([&scanner]() {
        for (int i = 0; i < 1000; ++i) {
            scanner.perform_scan_cycle();
        }
    });
    
    std::thread t2([&scanner]() {
        for (int i = 0; i < 1000; ++i) {
            scanner.get_display_data();
        }
    });
    
    std::thread t3([&scanner]() {
        for (int i = 0; i < 1000; ++i) {
            scanner.update_tracked_drones(
                2'400'000'000ULL,
                -60,
                i
            );
        }
    });
    
    t1.join();
    t2.join();
    t3.join();
    
    // Verify no corruption
    REQUIRE(scanner.get_state() == ScannerState::SCANNING);
    REQUIRE(scanner.get_tracked_count() <= MAX_TRACKED_DRONES);
}

TEST_CASE("Stress: Extreme RSSI values", "[stress]") {
    RSSIDetector detector;
    detector.initialize(-90);
    
    // Test extreme RSSI values
    detector.process_rssi_sample(RSSI_MIN_DBM, 0);      // -120 dBm
    detector.process_rssi_sample(RSSI_MAX_DBM, 100);    // -20 dBm
    detector.process_rssi_sample(-100, 200);           // Below threshold
    detector.process_rssi_sample(-30, 300);            // Above critical
    
    // Detect drone
    RSSIDetectionResult result;
    ErrorCode code = detector.detect_drone(result);
    REQUIRE(code == ErrorCode::SUCCESS);
    
    // Verify threat level
    REQUIRE(result.threat_level == ThreatLevel::CRITICAL);
}

TEST_CASE("Stress: Invalid parameters", "[stress]") {
    DatabaseManager db;
    HardwareController hw;
    DroneScanner scanner(db, hw);
    scanner.initialize();
    
    // Test invalid parameters
    ErrorCode result1 = hw.tune_to_frequency(1'000'000'000ULL);  // Below min
    REQUIRE(result1 == ErrorCode::INVALID_PARAMETER);
    
    ErrorCode result2 = hw.tune_to_frequency(3'000'000'000ULL);  // Above max
    REQUIRE(result2 == ErrorCode::INVALID_PARAMETER);
    
    ErrorCode result3 = hw.set_gain(100);  // Above max
    REQUIRE(result3 == ErrorCode::INVALID_PARAMETER);
}
```

**Testing Checklist:**

- [ ] Test empty database
- [ ] Test maximum database size
- [ ] Test maximum tracked drones
- [ ] Test rapid frequency changes
- [ ] Test all drones stale
- [ ] Test concurrent operations
- [ ] Test extreme RSSI values
- [ ] Test invalid parameters
- [ ] Test boundary conditions
- [ ] Test overflow conditions
- [ ] Test underflow conditions

### 10.7 Debug Instrumentation Recommendations

#### 10.7.1 Logging Strategy

**Logging Levels:**

```cpp
enum class LogLevel : uint8_t {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3,
    TRACE = 4
};

#ifdef DEBUG_LOGGING_ENABLED
#define LOG_ERROR(fmt, ...) \
    if (DEBUG_LOGGING_ENABLED) { \
        log_printf("[ERROR] " fmt, ##__VA_ARGS__); \
    }

#define LOG_WARNING(fmt, ...) \
    if (DEBUG_LOGGING_ENABLED) { \
        log_printf("[WARN] " fmt, ##__VA_ARGS__); \
    }

#define LOG_INFO(fmt, ...) \
    if (DEBUG_LOGGING_ENABLED) { \
        log_printf("[INFO] " fmt, ##__VA_ARGS__); \
    }

#define LOG_DEBUG(fmt, ...) \
    if (DEBUG_LOGGING_ENABLED) { \
        log_printf("[DEBUG] " fmt, ##__VA_ARGS__); \
    }

#define LOG_TRACE(fmt, ...) \
    if (DEBUG_LOGGING_ENABLED) { \
        log_printf("[TRACE] " fmt, ##__VA_ARGS__); \
    }
#else
#define LOG_ERROR(fmt, ...) ((void)0)
#define LOG_WARNING(fmt, ...) ((void)0)
#define LOG_INFO(fmt, ...) ((void)0)
#define LOG_DEBUG(fmt, ...) ((void)0)
#define LOG_TRACE(fmt, ...) ((void)0)
#endif
```

**Logging Points:**

```
Logging Strategy
├── Initialization
│   ├── LOG_INFO("DatabaseManager: Loading database...")
│   ├── LOG_INFO("HardwareController: Initializing...")
│   └── LOG_INFO("DroneScanner: Initializing...")
│
├── Errors
│   ├── LOG_ERROR("HardwareController: PLL lock failed, retry %u/%u", retry, max_retries)
│   ├── LOG_ERROR("DatabaseManager: Failed to load database: %s", error_to_string(error))
│   └── LOG_ERROR("DroneScanner: Scan cycle failed: %s", error_to_string(error))
│
├── State Changes
│   ├── LOG_DEBUG("HardwareController: State %d -> %d", old_state, new_state)
│   ├── LOG_DEBUG("DroneScanner: State %d -> %d", old_state, new_state)
│   └── LOG_DEBUG("Scanner: Scanning active: %s", scanning ? "true" : "false")
│
├── Performance
│   ├── LOG_DEBUG("Scanner: Scan cycle duration: %u ms", duration)
│   ├── LOG_DEBUG("UI: Update duration: %u ms", duration)
│   └── LOG_DEBUG("Hardware: Tune duration: %u ms", duration)
│
└── Statistics
    ├── LOG_INFO("Scanner: Total cycles: %u, Success: %u, Failed: %u", total, success, failed)
    ├── LOG_INFO("Scanner: Drones detected: %u", drones_detected)
    └── LOG_INFO("Scanner: Max RSSI: %d dBm", max_rssi_dbm)
```

#### 10.7.2 Performance Monitoring

**Performance Counters:**

```cpp
struct PerformanceCounters {
    uint32_t scan_cycles_total;
    uint32_t scan_cycles_success;
    uint32_t scan_cycles_failed;
    uint32_t scan_cycle_max_duration_ms;
    uint32_t scan_cycle_min_duration_ms;
    uint32_t scan_cycle_avg_duration_ms;
    
    uint32_t ui_updates_total;
    uint32_t ui_update_max_duration_ms;
    uint32_t ui_update_min_duration_ms;
    uint32_t ui_update_avg_duration_ms;
    
    uint32_t hardware_tunes_total;
    uint32_t hardware_tunes_success;
    uint32_t hardware_tunes_failed;
    uint32_t hardware_tune_max_duration_ms;
    
    uint32_t mutex_lock_timeouts;
    uint32_t mutex_contention_count;
};

PerformanceCounters g_perf_counters;

void update_scan_cycle_stats(SystemTime duration_ms) {
    g_perf_counters.scan_cycles_total++;
    
    if (duration_ms > g_perf_counters.scan_cycle_max_duration_ms) {
        g_perf_counters.scan_cycle_max_duration_ms = duration_ms;
    }
    
    if (duration_ms < g_perf_counters.scan_cycle_min_duration_ms ||
        g_perf_counters.scan_cycle_min_duration_ms == 0) {
        g_perf_counters.scan_cycle_min_duration_ms = duration_ms;
    }
    
    // Update running average
    g_perf_counters.scan_cycle_avg_duration_ms =
        (g_perf_counters.scan_cycle_avg_duration_ms * (g_perf_counters.scan_cycles_total - 1) + duration_ms) /
        g_perf_counters.scan_cycles_total;
}

void print_performance_stats() {
    LOG_INFO("=== Performance Statistics ===");
    LOG_INFO("Scan Cycles: Total=%u, Success=%u, Failed=%u",
             g_perf_counters.scan_cycles_total,
             g_perf_counters.scan_cycles_success,
             g_perf_counters.scan_cycles_failed);
    LOG_INFO("Scan Cycle Duration: Max=%u ms, Min=%u ms, Avg=%u ms",
             g_perf_counters.scan_cycle_max_duration_ms,
             g_perf_counters.scan_cycle_min_duration_ms,
             g_perf_counters.scan_cycle_avg_duration_ms);
    LOG_INFO("UI Updates: Total=%u, Max=%u ms, Min=%u ms, Avg=%u ms",
             g_perf_counters.ui_updates_total,
             g_perf_counters.ui_update_max_duration_ms,
             g_perf_counters.ui_update_min_duration_ms,
             g_perf_counters.ui_update_avg_duration_ms);
    LOG_INFO("Hardware Tunes: Total=%u, Success=%u, Failed=%u",
             g_perf_counters.hardware_tunes_total,
             g_perf_counters.hardware_tunes_success,
             g_perf_counters.hardware_tunes_failed);
    LOG_INFO("Mutex Timeouts: %u", g_perf_counters.mutex_lock_timeouts);
    LOG_INFO("Mutex Contention: %u", g_perf_counters.mutex_contention_count);
}
```

#### 10.7.3 Stack Usage Monitoring

**Stack Canary:**

```cpp
#ifdef DEBUG_STACK_MONITORING
class StackCanary {
public:
    StackCanary() {
        // Initialize canary at stack bottom
        canary_value_ = STACK_CANARY_MAGIC;
    }
    
    ~StackCanary() {
        // Verify canary at stack top
        if (canary_value_ != STACK_CANARY_MAGIC) {
            LOG_ERROR("Stack overflow detected!");
        }
    }
    
private:
    static constexpr uint32_t STACK_CANARY_MAGIC = 0xDEADBEEF;
    volatile uint32_t canary_value_;
};

#define STACK_CANARY() StackCanary stack_canary_##__LINE__
#else
#define STACK_CANARY() ((void)0)
#endif

// Usage in function
ErrorCode some_function() {
    STACK_CANARY();  // Place at beginning of function
    
    // Function code...
    
    return ErrorCode::SUCCESS;
    // Canary destructor checks for stack overflow
}
```

#### 10.7.4 Memory Leak Detection

**Memory Leak Tracker:**

```cpp
#ifdef DEBUG_MEMORY_TRACKING
struct AllocationTracker {
    size_t total_allocations;
    size_t total_deallocations;
    size_t current_allocations;
    size_t peak_allocations;
    
    void track_allocation(size_t size) {
        total_allocations++;
        current_allocations++;
        if (current_allocations > peak_allocations) {
            peak_allocations = current_allocations;
        }
        LOG_DEBUG("Allocation: %zu bytes, Total: %zu, Current: %zu, Peak: %zu",
                 size, total_allocations, current_allocations, peak_allocations);
    }
    
    void track_deallocation(size_t size) {
        total_deallocations++;
        current_allocations--;
        LOG_DEBUG("Deallocation: %zu bytes, Total: %zu, Current: %zu",
                 size, total_deallocations, current_allocations);
    }
    
    void check_leaks() {
        if (current_allocations > 0) {
            LOG_ERROR("Memory leak detected: %zu allocations not freed",
                      current_allocations);
        }
    }
};

AllocationTracker g_alloc_tracker;

void* operator new(size_t size) {
    void* ptr = malloc(size);
    g_alloc_tracker.track_allocation(size);
    return ptr;
}

void operator delete(void* ptr) {
    g_alloc_tracker.track_deallocation(0);
    free(ptr);
}
#endif
```

### 10.8 Final Testing Checklist

#### Comprehensive Testing Checklist

**Unit Tests:**
- [ ] DatabaseManager unit tests (20+ test cases)
- [ ] HardwareController unit tests (15+ test cases)
- [ ] DroneScanner unit tests (20+ test cases)
- [ ] SpectrumProcessor unit tests (10+ test cases)
- [ ] RSSIDetector unit tests (10+ test cases)
- [ ] HistogramProcessor unit tests (5+ test cases)
- [ ] Error handling unit tests (15+ test cases)
- [ ] Locking mechanism unit tests (10+ test cases)

**Integration Tests:**
- [ ] Database + Hardware integration tests (5+ test cases)
- [ ] Scanner + UI integration tests (5+ test cases)
- [ ] DSP + UI integration tests (5+ test cases)
- [ ] Component interaction tests (10+ test cases)
- [ ] Error recovery integration tests (5+ test cases)

**System Tests:**
- [ ] Full application lifecycle test
- [ ] Initialization flow test
- [ ] Normal operation test
- [ ] Shutdown flow test
- [ ] Error recovery test
- [ ] Graceful degradation test

**Performance Tests:**
- [ ] Scan rate performance test
- [ ] Database iteration performance test
- [ ] Drone tracking performance test
- [ ] Display update performance test
- [ ] Real-time constraints test
- [ ] Frame rate stability test

**Memory Tests:**
- [ ] Static memory usage test
- [ ] Stack usage test
- [ ] Memory leak detection test
- [ ] Memory alignment test
- [ ] No heap allocation verification

**Stress Tests:**
- [ ] Empty database test
- [ ] Maximum database size test
- [ ] Maximum tracked drones test
- [ ] Rapid frequency changes test
- [ ] All drones stale test
- [ ] Concurrent operations test
- [ ] Extreme RSSI values test
- [ ] Invalid parameters test
- [ ] Boundary conditions test

**Thread Safety Tests:**
- [ ] Lock ordering verification
- [ ] Deadlock detection test
- [ ] Race condition test
- [ ] Mutex timeout test
- [ ] Atomic flag test

**Debug Instrumentation:**
- [ ] Logging implementation
- [ ] Performance monitoring implementation
- [ ] Stack usage monitoring implementation
- [ ] Memory leak detection implementation
- [ ] Debug mode compilation

---

**End of Part 4 - Complete Testing Plan**

---

## Summary

This comprehensive testing and debugging plan covers all aspects of the Enhanced Drone Analyzer application:

1. **Initialization Flow Analysis** - 3-phase initialization with race condition detection
2. **Database Integration with Freqman** - File format, parsing, editing, error handling
3. **Hardware Access Flow** - Tuning, PLL lock, streaming, RSSI sampling, error recovery
4. **Scanning Logic Flow** - Scan cycles, database iteration, drone detection, tracking
5. **Display and Output Flow** - UI updates, spectrum/histogram/list rendering
6. **Audio Alert Flow** - Triggering, playback, priority handling
7. **Error Handling Flow** - Detection, propagation, recovery, user notification
8. **Thread Synchronization** - Lock ordering, mutex usage, atomic flags, deadlock prevention
9. **Memory Usage Analysis** - Static storage, stack usage, allocation/deallocation, leak risks
10. **Testing Plan** - Unit tests, integration tests, system tests, performance tests, stress tests

Each section includes:
- Detailed flow diagrams
- Step-by-step analysis
- Identified potential issues and risks
- Testing checklists
- Debug instrumentation recommendations
- Logging strategy

This plan provides a complete roadmap for testing and debugging the refactored Enhanced Drone Analyzer application.
