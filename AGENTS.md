# AGENTS.md - Mayhem Firmware Developer Guide

This file provides guidelines for AI agents working on the PortaPack Mayhem firmware codebase.

---

## 1. Build Commands

**Compiler**: ARM GCC toolchain at `C:\SysGCC\arm-eabi\bin`

```bash
# Full build (in-tree)
mkdir -p build && cd build
cmake .. && cmake --build . -j$(nproc)

# Faster build with Ninja
cmake -G Ninja .. && ninja

# Build firmware only
cmake --build . --target firmware

# Build and run tests
cmake --build . --target build_tests && cd build && ctest --output-on-failure

# Run single test executable (after build)
./firmware/test/application/application_test
./firmware/test/baseband/baseband_test
./firmware/test/enhanced_drone_analyzer/enhanced_drone_analyzer_test

# Run specific test case
./firmware/test/application/application_test "test_name_filter"
```

---

## 2. Code Style

**Formatting**: Run `./format-code.sh` (clang-format-18, Chromium style, 4-space indent, no column limit).

**Pre-commit Checks**:
- No tabs in source code (allowed in comments)
- Use LF line endings (not CRLF)
- No trailing whitespace
- Header guards required on all .hpp files (NOT `#pragma once`)

**Include Order**: 1) Standard lib (`<cstdint>`), 2) Third-party (`ch.h`, `doctest.h`), 3) Project headers (quotes).

---

## 3. Embedded Constraints (CRITICAL)

This is ARM Cortex-M4 firmware with only 128KB RAM.

**FORBIDDEN**: `std::vector`, `std::string`, `std::map`, `std::atomic`, `new`, `malloc`, exceptions, RTTI, stack >4KB.

**PERMITTED**: `std::array<T,N>`, `std::string_view`, fixed-size buffers, C++17, memory pools, `constexpr`, `enum class`.

**Memory**: Use `const` for flash (read-only), `constexpr` for compile-time constants, avoid global constructors.

---

## 4. Types & Naming

**Types**: Use explicit fixed-width types (`uint32_t`, `int8_t`). Use type aliases: `using FreqHz = uint32_t;`. Use `enum class`.

**Naming**:
- Files: lowercase_underscores.hpp
- Classes: PascalCase
- Functions: snake_case
- Constants: UPPER_SNAKE_CASE
- Members: trailing_underscore_

**No magic numbers**: Use `constexpr uint32_t TIMEOUT_MS = 1000;`

---

## 5. Error Handling

- No exceptions (not supported)
- Return error codes or use `std::optional<T>`
- Always handle return values, no silent failures

---

## 6. Thread Safety (ChibiOS RTOS)

- Use RAII mutex wrappers
- NEVER call `chMtxDeinit` (does not exist, causes linker errors)
- Use `try_get()` for non-blocking UI updates
- Never block in ISR or time-critical code
- Document `_internal` methods with `@pre`, `@note`, `@invariant`

---

## 7. Architecture

- `firmware/application/` - Main app code
- `firmware/application/ui/` - UI components (inherit from `ui::View`)
- `firmware/baseband/` - DSP processing (separate M0 core)
- `firmware/common/` - Shared utilities
- `firmware/test/` - Unit tests

---

## 8. Linting & CI

```bash
./run_cppcheck.sh
clang-tidy file.cpp -checks=-*,thread-*,bugprone-* -- -std=c++17
```

Workflows in `.github/workflows/`: `code_quality.yml`, `check_formatting.yml`.

---

## 9. Code Refinement Pipeline (For AI Agents)

When modifying code, follow this 4-stage pipeline:

### Stage 1: Forensic Audit
- Identify heap allocations (std::vector, std::string, new/malloc)
- Find STL containers, race conditions, unsafe ISR interactions
- Check for magic numbers, type ambiguity, UI/DSP mixing

### Stage 2: Architect's Blueprint
- Replace dynamic containers with `std::array<T, N>`
- Define memory placement (Flash vs RAM)
- Plan RAII wrappers and function signatures

### Stage 3: Red Team Attack
- Test: Will array blow stack in recursive calls?
- Test: Is floating-point too slow for real-time DSP? Use fixed-point.
- Test: Does code fit repository style?
- Test: Handle empty buffers and SPI failures

### Stage 4: Diamond Code Synthesis
- Apply clean, flat hierarchy
- Use guard clauses
- Add Doxygen comments
- Verify no mixed logic (separate UI from DSP)

---

## 10. Key Files

- `.clang-format` - Formatting rules
- `.clangd` - Language server config
- `.cppcheck_suppressions.txt` - Cppcheck suppressions
- `CMakeLists.txt` - Root build config
- `firmware/test/CMakeLists.txt` - Test build config
