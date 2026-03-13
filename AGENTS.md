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

Only run linting when:
- User explicitly asks for it
- After making changes and before committing (optional)

```bash
./run_cppcheck.sh
clang-tidy file.cpp -checks=-*,thread-*,bugprone-* -- -std=c++17
```

Workflows in `.github/workflows/`: `code_quality.yml`, `check_formatting.yml`.

---

## 9. Workflow

When asked to make a change:
1. Understand what needs to be done (ask if unclear)
2. Make the change
3. Done. Do NOT run build commands unless user explicitly asks.

DO NOT:
- Run build commands to "verify" changes unless asked
- Run grep/cat/head/tail commands to inspect code - use Read/Grep tools instead
- Run abstract filtering like `grep -E "(eda_locking|scanning_coordinator|...)"` - this is meaningless
- Second-guess the requirements

---

## 10. Key Files

- `.clang-format` - Formatting rules
- `.clangd` - Language server config
- `.cppcheck_suppressions.txt` - Cppcheck suppressions
- `CMakeLists.txt` - Root build config
- `firmware/test/CMakeLists.txt` - Test build config
