# Enhanced Drone Analyzer - Unit Tests

## Overview

This directory contains unit tests for the Enhanced Drone Analyzer application. These tests verify the correctness of pure logic classes that do not depend on embedded hardware.

## Test Structure

```
tests/
├── CMakeLists.txt          # CMake build configuration
├── mocks/                  # Mock implementations for embedded dependencies
│   ├── chibios_mock.hpp   # Mock ChibiOS threading primitives
│   └── portapack_mock.hpp # Mock Portapack UI types
├── test_median_filter.cpp         # Tests for FastMedianFilter
├── test_spectral_analyzer.cpp    # Tests for SpectralAnalyzer
└── test_drone_validation.cpp      # Tests for SimpleDroneValidation
```

## Running Tests Locally

### Prerequisites

- GCC/Clang with C++17 support
- CMake (optional, for building with CMake)
- Make

### Using g++ directly

```bash
cd tests

# Create build directory
mkdir -p build

# Compile test_median_filter
g++ -std=c++17 -O2 -Wall -Wextra test_median_filter.cpp -o build/test_median_filter
./build/test_median_filter

# Compile test_spectral_analyzer
g++ -std=c++17 -O2 -Wall -Wextra test_spectral_analyzer.cpp -o build/test_spectral_analyzer
./build/test_spectral_analyzer

# Compile test_drone_validation
g++ -std=c++17 -O2 -Wall -Wextra test_drone_validation.cpp -o build/test_drone_validation
./build/test_drone_validation
```

### Using CMake (when GoogleTest is available)

```bash
cd tests
mkdir -p build
cd build
cmake ..
make -j$(nproc)
ctest --output-on-failure --verbose
```

## Test Descriptions

### test_median_filter.cpp

Tests the `FastMedianFilter` class:

- `test_empty_filter_returns_zero` - Verifies empty filter returns 0
- `test_full_window_calculates_correct_median` - Tests median calculation for 11 samples
- `test_odd_number_of_samples` - Tests with even-spaced samples
- `test_reset_clears_window` - Verifies reset functionality
- `test_circular_buffer_behavior` - Tests circular buffer wraparound

### test_spectral_analyzer.cpp

Tests the `SpectralAnalyzer` class:

- `test_noise_detection` - Verifies noise signal classification
- `test_narrowband_drone_detection` - Tests narrowband drone signal detection
- `test_wideband_wifi_detection` - Tests wideband WiFi signal detection
- `test_digital_fpv_detection` - Tests digital FPV signal detection
- `test_threat_level_calculation` - Verifies threat level calculation
- `test_drone_type_from_frequency` - Tests drone type identification

### test_drone_validation.cpp

Tests the `SimpleDroneValidation` class:

- `test_valid_frequency_ranges` - Tests valid frequency range detection
- `test_invalid_frequency_ranges` - Tests invalid frequency range detection
- `test_rssi_signal_validation` - Tests RSSI signal validation
- `test_signal_strength_classification` - Tests signal strength classification
- `test_drone_type_identification` - Tests drone type identification

## Mocks

The `mocks/` directory contains simplified implementations of embedded dependencies:

### chibios_mock.hpp

Provides mock implementations for:
- `systime_t` type
- `Thread`, `Mutex`, `Semaphore` structures
- `chMtxLock()`, `chMtxUnlock()`
- `chThdSleepMilliseconds()`
- `chThdCreateStatic()`, `chThdTerminate()`, `chThdWait()`

### portapack_mock.hpp

Provides mock implementations for:
- `Rect`, `Color`, `Style` structures
- `Color` factory methods (red(), green(), etc.)
- `screen_width`, `screen_height` constants

## Adding New Tests

To add a new test:

1. Create a new `.cpp` file in the `tests/` directory
2. Include the header file of the class you want to test
3. Write test functions using `assert()` or a testing framework
4. Add the test to `CMakeLists.txt` (if using CMake)
5. Run the test to verify it passes

## CI/CD Integration

These tests are automatically run in GitHub Actions as part of the `unit_tests` job in the workflow:

```yaml
unit_tests:
  name: Unit Tests
  runs-on: ubuntu-latest
  ...
```

## Notes

- These tests are designed for "pure logic" classes that do not depend on hardware
- For hardware-dependent tests, integration tests on the actual device are recommended
- All tests use `assert()` for simplicity, but can be adapted to GoogleTest or other frameworks
