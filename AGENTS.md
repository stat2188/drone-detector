# AGENTS.md - Mayhem Firmware Developer Guide

This file provides guidelines for AI agents working on the PortaPack Mayhem firmware codebase.

---

## 1. Build Commands

**1.1 Full Build (in-tree)**
```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

**1.2 Faster Build with Ninja**
```bash
cmake -G Ninja ..
ninja
```

**1.3 Build Firmware Only**
```bash
cd build && make firmware
```

**1.4 Build All Tests**
```bash
cd build && make build_tests
```

**1.5 Run All Tests**
```bash
cd build && ctest --output-on-failure
```

**1.6 Run Single Test Executable**
```bash
cd build
./firmware/test/application/application_test
./firmware/test/baseband/baseband_test
./firmware/test/enhanced_drone_analyzer/enhanced_drone_analyzer_test
```

**1.7 Run Specific Test Case**
```bash
cd build
./firmware/test/application/application_test "test_name_filter"
```

---

## 2. Code Formatting

**2.1 Format Code Automatically**
```bash
./format-code.sh
```
Uses clang-format-18 with Chromium style (4-space indent, no column limit).

**2.2 Clang-Format Rules**
- BasedOnStyle: Chromium
- IndentWidth: 4
- ColumnLimit: 0 (unlimited line length)
- Cpp11BracedListStyle: true
- AllowShortLoopsOnASingleLine: true
- AllowShortIfStatementsOnASingleLine: true
- SortIncludes: false

**2.3 Pre-commit Checks**
- No tabs in source code (allowed in comments)
- Use LF line endings (not CRLF)
- No trailing whitespace
- Header guards required on all .hpp files

---

## 3. Embedded Constraints (CRITICAL)

This is an embedded ARM Cortex-M4 firmware with only 128KB RAM.

**3.1 FORBIDDEN - Never Use**
- `std::vector` - dynamic allocation
- `std::string` - dynamic allocation
- `std::map`, `std::unordered_map` - dynamic allocation
- `std::atomic` - not needed, causes bloat
- `new`, `malloc` - heap allocation
- Exceptions - not supported
- RTTI (typeid, dynamic_cast) - not supported
- Stack allocations > 4KB

**3.2 PERMITTED - Safe to Use**
- `std::array<T, N>` - fixed-size, stack allocated
- `std::string_view` - no allocation, just a pointer+size
- Fixed-size character buffers
- C++11/14/17 standard
- Memory pools (pre-allocated)
- `constexpr`, `enum class`

**3.3 Memory Placement**
- Use `const` for data in flash (read-only)
- Use `constexpr` for compile-time constants
- Avoid global objects with constructors

---

## 4. Types and Integer Types

**4.1 Always Use Explicit Fixed-Width Types**
```cpp
#include <cstdint>

using int8_t = std::int8_t;
using uint32_t = std::uint32_t;
using size_t = std::size_t;
```

**4.2 Use Type Aliases for Clarity**
```cpp
using FreqHz = uint32_t;
using FreqKHz = uint32_t;
using BytesPerSec = uint32_t;
```

**4.3 Use Enum Class for Type-Safe Enums**
```cpp
enum class Mode : uint8_t {
    Idle,
    Receive,
    Transmit
};
```

**4.4 Avoid Magic Numbers**
```cpp
// BAD: if (value > 1000)
constexpr uint32_t TIMEOUT_MS = 1000;
if (value > TIMEOUT_MS)
```

---

## 5. Naming Conventions

**5.1 Files**
- Use lowercase with underscores: `string_format.hpp`, `ui_button.cpp`

**5.2 Classes**
- Use PascalCase: `class MyClass`, `class UiView`

**5.3 Functions**
- Use snake_case: `do_something()`, `calculate_checksum()`

**5.4 Constants/Enums**
- Use UPPER_SNAKE_CASE: `constexpr uint32_t BUFFER_SIZE = 256;`
- Or enum class values: `Mode::Receive`

**5.5 Member Variables**
- Use trailing underscore: `member_name_` or `m_member_name`

---

## 6. Header Files

**6.1 Include Guards (Required)**
```cpp
#ifndef __STRING_FORMAT_H__
#define __STRING_FORMAT_H__
// ... content ...
#endif
```
Do NOT use `#pragma once` - this codebase uses include guards.

**6.2 Include Order**
1. Standard library (`<cstdint>`, `<array>`)
2. Third-party (`ch.h`, `doctest.h`)
3. Project headers (use quotes)

**6.3 Minimal Headers**
- Only include what you use
- Use forward declarations when possible
- Include related functionality together

---

## 7. Error Handling

**7.1 No Exceptions**
- Never throw exceptions (not supported)
- Return error codes or use `optional<T>`

**7.2 Use Optional for Nullable Values**
```cpp
#include <optional>
std::optional<Config> load_config();
```

**7.3 Check Return Values**
- Always handle error returns
- No silent failures

---

## 8. Thread Safety (ChibiOS RTOS)

**8.1 Use RAII Mutex Wrappers**
```cpp
class MutexLock {
public:
    MutexLock(mutex_t& m) : mutex_(m) { chMtxLock(&mutex_); }
    ~MutexLock() { chMtxUnlock(&mutex_); }
private:
    mutex_t& mutex_;
};
```

**8.2 NEVER Use chMtxDeinit**
- This function does not exist in the ChibiOS version used
- Will cause linker errors

**8.3 Non-Blocking Access**
- Use `try_get()` methods for UI updates
- Never block in ISR or time-critical code

**8.4 Document Thread Safety**
- Document `_internal` methods with `@pre`, `@note`, `@invariant`
- Specify locking requirements

---

## 9. License Headers

**9.1 All Source Files Must Have GPL Header**
```cpp
/*
 * Copyright (C) YEAR Author Name
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
```

---

## 10. Architecture

**10.1 Directory Structure**
- `firmware/application/` - Main application code
- `firmware/application/ui/` - UI components
- `firmware/baseband/` - DSP/baseband processing
- `firmware/common/` - Shared utilities
- `firmware/test/` - Unit tests
- `firmware/chibios/` - RTOS and HAL

**10.2 UI Components**
- Inherit from `ui::View` base class
- Use portapack display/input APIs

**10.3 Baseband Processing**
- Runs on separate ARM core (M0)
- Use message passing for communication

---

## 11. Linting

**11.1 Cppcheck**
```bash
./run_cppcheck.sh
```
Suppressions are in `.cppcheck_suppressions.txt`.

**11.2 Clang-Tidy**
```bash
clang-tidy file.cpp -checks=-*,thread-*,bugprone-* -- -std=c++17
```

---

## 12. CI/CD

GitHub Actions workflows are in `.github/workflows/`:
- `code_quality.yml` - Static analysis, formatting, dependency checks
- `check_formatting.yml` - Code formatting validation

---

## 13. Key Configuration Files

- `.clang-format` - Code formatting rules
- `.clangd` - Language server configuration for clangd
- `.cppcheck_suppressions.txt` - Cppcheck suppressions
- `CMakeLists.txt` - Root build configuration
- `firmware/CMakeLists.txt` - Firmware build rules
- `firmware/test/CMakeLists.txt` - Test build configuration
