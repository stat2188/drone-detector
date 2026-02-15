# EDA Project Fix - Multiple Definition Errors

## Problem

The build was failing with multiple redefinition errors in `ui_navigation.cpp`:

```
error: redefinition of 'struct ui::fileman_entry'
error: multiple definition of 'enum class ui::EmptyReason'
error: multiple definition of 'enum class ui::ClipboardMode'
error: redefinition of 'class ui::FileManBaseView'
error: redefinition of 'class ui::FileLoadView'
error: redefinition of 'class ui::FileManagerView'
```

## Root Cause

`ui_fileman.hpp` was being included **twice** in `ui_navigation.cpp` through different paths:

1. **Direct inclusion** at line 39:
   ```cpp
   #include "ui_fileman.hpp"
   ```

2. **Transitive inclusion** at line 65 via `ui_enhanced_drone_analyzer.hpp`:
   ```cpp
   #include "apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp"
   ```
   Which itself included `ui_fileman.hpp` at line 26.

Since `ui_fileman.hpp` lacks include guards (pragma once or #ifndef/#define/#endif), the preprocessor processed its contents twice, causing all definitions to be duplicated.

## Solution

Removed the unnecessary `#include "ui_fileman.hpp"` from the EDA project header file:

**File Modified:** `firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp`
- **Line 26** removed: `#include "ui_fileman.hpp"`

## Why This Works

The header `ui_enhanced_drone_analyzer.hpp` does not use any classes from `ui_fileman.hpp` (it only had the include without any usage). The `.cpp` file still includes `ui_fileman.hpp` (line 40) because it uses `FileLoadView` for CSV file operations:

```cpp
auto open_view = nav_.push<FileLoadView>(".CSV");
```

## Dependency Chain After Fix

```
BEFORE:
ui_navigation.cpp
  ├─> ui_fileman.hpp (Line 39) ───┐
  │                                  │
  └─> ui_enhanced_drone_analyzer.hpp (Line 65)
        └─> ui_fileman.hpp (Line 26) ┘
  ❌ Double inclusion -> MULTIPLE DEFINITION ERRORS

AFTER:
ui_navigation.cpp
  ├─> ui_fileman.hpp (Line 39) ✓
  │
  └─> ui_enhanced_drone_analyzer.hpp (Line 65)
        └─> [NO ui_fileman.hpp anymore]
  ✅ Single inclusion -> NO ERRORS
```

## Files Changed

1. `firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp`
   - Removed: `#include "ui_fileman.hpp"` (line 26)

## Files NOT Changed (per requirement)

- `firmware/application/apps/ui_fileman.hpp` - Firmware file, left unchanged
- `firmware/application/ui_navigation.cpp` - Firmware file, left unchanged
- `firmware/application/ui_navigation.hpp` - Firmware file, left unchanged

## Verification

The fix was verified by:
1. Checking that `ui_enhanced_drone_analyzer.hpp` no longer includes `ui_fileman.hpp`
2. Confirming that `ui_enhanced_drone_analyzer.cpp` still includes `ui_fileman.hpp` (needed for `FileLoadView`)
3. Verifying that `ui_navigation.cpp` includes `ui_fileman.hpp` directly
4. Confirming that `ui_enhanced_drone_analyzer.hpp` does not use any `FileMan` classes
5. Verifying that only the EDA project file was modified (not firmware files)

## Result

The double inclusion chain is broken, and `ui_fileman.hpp` is now included only once in `ui_navigation.cpp`. This eliminates all multiple definition errors without changing any firmware files.
