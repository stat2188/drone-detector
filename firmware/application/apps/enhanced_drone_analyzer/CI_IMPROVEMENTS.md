# Enhanced Drone Analyzer - CI/CD Improvements

## Overview

This document describes the improvements made to the CI/CD pipeline for the Enhanced Drone Analyzer application.

## What Was Added

### 1. **Clang-Format Check** (replaces formatting_check)
- **Job:** `clang_format_check`
- **Purpose:** Automated code formatting verification using `clang-format`
- **Files:**
  - `.clang-format` - Configuration file for code style
- **Changes:**
  - Replaced manual grep-based formatting checks
  - Set `ColumnLimit: 120`
  - Enabled consistent indentation and spacing

### 2. **Stack Usage Analysis**
- **Job:** `stack_usage_check`
- **Purpose:** Detects functions that consume excessive stack memory
- **What it checks:**
  - Large `std::array` allocations (>512 bytes)
  - Large char buffers (>128 bytes)
  - Multi-dimensional arrays (like waterfall buffer)
- **Why important:** Prevents stack overflow in embedded systems

### 3. **Heap Usage Analysis**
- **Job:** `heap_usage_check`
- **Purpose:** Detects heap-allocating types that may cause fragmentation
- **What it checks:**
  - `std::vector` usage
  - `std::map` usage
  - `std::string` members
  - Smart pointers (`unique_ptr`, `shared_ptr`)
  - Manual `new/delete` operations
- **Why important:** Heap fragmentation is a critical issue in embedded systems

### 4. **Magic Numbers Check**
- **Job:** `magic_numbers_check`
- **Purpose:** Detects hardcoded constants that should use named constants
- **What it checks:**
  - Hardcoded frequency values (>1,000,000 Hz)
  - Hardcoded RSSI threshold values
  - Hardcoded time intervals
- **Files:**
  - `drone_constants.hpp` - Centralized constant definitions
- **Why important:** Improves maintainability and prevents typos

### 5. **Binary Size Analysis**
- **Job:** `binary_size_check`
- **Purpose:** Monitors binary file size to prevent exceeding flash limits
- **What it checks:**
  - Size of individual .o files
  - Total size (>50KB warning, >500KB failure)
- **Why important:** Portapack has limited flash memory

### 6. **Host Build Check**
- **Job:** `host_build`
- **Purpose:** Compiles pure logic classes on host system (Ubuntu)
- **What it builds:**
  - Pure logic classes without hardware dependencies
  - Uses mocks for ChibiOS and Portapack
- **Why important:** Catches syntax errors early without ARM toolchain

### 7. **Unit Tests**
- **Job:** `unit_tests`
- **Purpose:** Automated testing of pure logic classes
- **Test files:**
  - `test_median_filter.cpp` - FastMedianFilter tests
  - `test_spectral_analyzer.cpp` - SpectralAnalyzer tests
  - `test_drone_validation.cpp` - SimpleDroneValidation tests
- **Mock files:**
  - `mocks/chibios_mock.hpp` - ChibiOS threading mocks
  - `mocks/portapack_mock.hpp` - Portapack UI type mocks
- **Why important:** Verifies correctness of algorithms independently

### 8. **CodeQL Configuration**
- **File:** `.github/codeql-config.yml`
- **Purpose:** Configure CodeQL security analysis
- **Queries:**
  - `cpp-security-and-quality.qls` - Extended security queries
- **Note:** CodeQL job should be added separately with GitHub Advanced Security

## File Structure

```
firmware/application/apps/enhanced_drone_analyzer/
├── drone_constants.hpp          # New: Centralized constant definitions
└── tests/                       # New: Unit test suite
    ├── CMakeLists.txt
    ├── README.md
    ├── mocks/
    │   ├── chibios_mock.hpp
    │   └── portapack_mock.hpp
    ├── test_median_filter.cpp
    ├── test_spectral_analyzer.cpp
    └── test_drone_validation.cpp

.github/
├── workflows/
│   └── enhanced_drone_analyzer.yml  # Updated: New jobs added
├── codeql-config.yml              # New: CodeQL configuration
└── workflows/codeql/              # (optional) CodeQL query customization

.clang-format                      # Updated: Enhanced configuration
```

## CI/CD Pipeline Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                      Enhanced Drone Analyzer CI                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────────┐  ┌──────────────────┐                    │
│  │  static_analysis │  │ clang_format_   │                    │
│  │  (Cppcheck,      │  │  check          │                    │
│  │   Clang-Tidy)    │  │                 │                    │
│  └────────┬─────────┘  └────────┬─────────┘                    │
│           │                      │                              │
│           ▼                      ▼                              │
│  ┌──────────────────┐  ┌──────────────────┐                    │
│  │ dependency_check │  │ stack_usage_     │                    │
│  │                  │  │  check           │                    │
│  └────────┬─────────┘  └────────┬─────────┘                    │
│           │                      │                              │
│           ▼                      ▼                              │
│  ┌──────────────────┐  ┌──────────────────┐                    │
│  │ heap_usage_     │  │ magic_numbers_   │                    │
│  │  check          │  │  check           │                    │
│  └────────┬─────────┘  └────────┬─────────┘                    │
│           │                      │                              │
│           ▼                      ▼                              │
│  ┌──────────────────┐  ┌──────────────────┐                    │
│  │ binary_size_    │  │ host_build       │                    │
│  │  check          │  │                  │                    │
│  └────────┬─────────┘  └────────┬─────────┘                    │
│           │                      │                              │
│           ▼                      ▼                              │
│  ┌──────────────────┐                                    │
│  │ unit_tests       │                                    │
│  │                  │                                    │
│  └────────┬─────────┘                                    │
│           │                                              │
│           ▼                                              │
│  ┌──────────────────────────────────────┐                │
│  │ final_status                         │                │
│  │ (aggregates all results)            │                │
│  └──────────────────────────────────────┘                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Running Tests Locally

### Unit Tests

```bash
cd firmware/application/apps/enhanced_drone_analyzer/tests

# Build and run test_median_filter
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra test_median_filter.cpp -o build/test_median_filter
./build/test_median_filter

# Build and run test_spectral_analyzer
g++ -std=c++17 -O2 -Wall -Wextra test_spectral_analyzer.cpp -o build/test_spectral_analyzer
./build/test_spectral_analyzer

# Build and run test_drone_validation
g++ -std=c++17 -O2 -Wall -Wextra test_drone_validation.cpp -o build/test_drone_validation
./build/test_drone_validation
```

### Clang-Format Check

```bash
# Check formatting (returns non-zero if issues found)
clang-format -dry-run -Werror \
  firmware/application/apps/enhanced_drone_analyzer/*.cpp \
  firmware/application/apps/enhanced_drone_analyzer/*.hpp

# Fix formatting automatically
clang-format -i \
  firmware/application/apps/enhanced_drone_analyzer/*.cpp \
  firmware/application/apps/enhanced_drone_analyzer/*.hpp
```

## Next Steps

### Recommended Refactoring

1. **Replace hardcoded constants** with `DroneConstants::` values:
   ```cpp
   // Before:
   if (frequency_hz >= 5725000000ULL && frequency_hz <= 5875000000ULL)
   
   // After:
   if (frequency_hz >= DroneConstants::MIN_58GHZ &&
       frequency_hz <= DroneConstants::MAX_58GHZ)
   ```

2. **Consider using `std::array` instead of `std::vector`** for fixed-size containers

3. **Add `#include "drone_constants.hpp"`** to files that use constants

### Future Enhancements

- Add CodeQL job (requires GitHub Advanced Security)
- Add code coverage reporting
- Add integration tests on actual hardware
- Add performance benchmarking

## Troubleshooting

### Unit Tests Fail to Compile

If tests fail to compile on your system:
1. Ensure you have a C++17 compatible compiler (GCC 8+, Clang 7+)
2. Check that all mock files are present in `tests/mocks/`
3. Verify that `drone_constants.hpp` is in the parent directory

### Clang-Format Issues

If you see formatting warnings:
1. Run `clang-format -i <file>` to fix formatting automatically
2. Check `.clang-format` configuration
3. Ensure your clang-format version supports all options used

### Magic Numbers Warnings

If you see magic number warnings:
1. Check if the number is already defined in `drone_constants.hpp`
2. Add missing constants to `drone_constants.hpp`
3. Replace hardcoded values with constant references

## Summary

| Job | Status | Purpose |
|-----|--------|---------|
| `static_analysis` | Existing | Cppcheck + Clang-Tidy |
| `clang_format_check` | **New** | Code formatting verification |
| `dependency_check` | Existing | Header includes check |
| `stack_usage_check` | **New** | Stack memory analysis |
| `heap_usage_check` | **New** | Heap allocation detection |
| `magic_numbers_check` | **New** | Hardcoded constant detection |
| `binary_size_check` | **New** | Binary size monitoring |
| `host_build` | **New** | Host system compilation |
| `unit_tests` | **New** | Automated unit testing |
| `final_status` | Updated | Aggregates all results |

**Total:** 4 existing jobs → 10 jobs (+6 new, 1 replaced)

## References

- [Clang-Format Documentation](https://clang.llvm.org/docs/ClangFormat.html)
- [Cppcheck Manual](https://cppcheck.sourceforge.io/manual.pdf)
- [ChibiOS Documentation](http://www.chibios.org/dokuwiki/doku.php)
- [GoogleTest Documentation](https://google.github.io/googletest/)
- [CodeQL Documentation](https://codeql.github.com/docs/)
