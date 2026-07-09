# Plan: Remove Pattern Comparison Logic from EDA

## Goal
Completely remove all pattern comparison (PTR) logic from the Enhanced Drone Analyzer application. The feature is broken and needs to be cleaned out entirely.

## Summary
- Delete 8 pattern files
- Modify 13 source files to remove pattern-related code
- Update CMakeLists.txt to remove 4 source files from build
- Net savings: ~960B SRAM, ~1,400 lines of dead code removed

---

## Files to DELETE (8 files)

1. `firmware/application/apps/enhanced_drone_analyzer/pattern_types.hpp`
2. `firmware/application/apps/enhanced_drone_analyzer/pattern_types.cpp`
3. `firmware/application/apps/enhanced_drone_analyzer/pattern_matcher.hpp`
4. `firmware/application/apps/enhanced_drone_analyzer/pattern_matcher.cpp`
5. `firmware/application/apps/enhanced_drone_analyzer/pattern_manager.hpp`
6. `firmware/application/apps/enhanced_drone_analyzer/pattern_manager.cpp`
7. `firmware/application/apps/enhanced_drone_analyzer/pattern_manager_view.hpp`
8. `firmware/application/apps/enhanced_drone_analyzer/pattern_manager_view.cpp`

---

## Files to MODIFY (13 files)

### 1. `scanner.hpp`

**Remove includes (line 19-20):**
```cpp
// DELETE these two lines:
#include "pattern_matcher.hpp"
#include "pattern_manager.hpp"
```

**Remove from ScanConfig (lines 106-108):**
```cpp
// DELETE these lines:
    // Pattern matching settings
    bool pattern_matching_enabled{true};              // Enable/disable pattern matching
    uint16_t pattern_similarity_threshold{DEFAULT_PATTERN_SIMILARITY_THRESHOLD};  // 0-1000
```

**Remove public methods (lines 1114-1144):**
```cpp
// DELETE get_pattern_manager(), get_pattern_count(), get_patterns(), refresh_patterns()
```

**Remove from private section (lines 1347-1361):**
```cpp
// DELETE try_match_pattern_internal() declaration
```

**Remove member variables (lines 1488-1496):**
```cpp
// DELETE these lines:
    // Pattern matcher for signal pattern recognition
    PatternMatcher pattern_matcher_;

    // Pattern manager for loading/saving patterns from SD card
    PatternManager pattern_manager_;

    // Matched pattern index (-1 if no pattern matched in sweep)
    int8_t matched_pattern_index_{-1};
    size_t matched_pattern_bin_{0};
```

**Remove public pattern methods (lines 1500-1503):**
```cpp
// DELETE these lines:
    [[nodiscard]] bool is_pattern_matched() const noexcept { return matched_pattern_index_ >= 0; }
    [[nodiscard]] size_t get_matched_pattern_bin() const noexcept { return matched_pattern_bin_; }
    void clear_matched_pattern() noexcept { matched_pattern_index_ = -1; matched_pattern_bin_ = 0; }
```

**Update `apply_sweep_tracking()` signature** — remove last 3 params:
```cpp
// BEFORE:
void apply_sweep_tracking(
    FreqHz peak_freq, int32_t peak_rssi, FreqHz center_freq,
    FreqHz f_min, FreqHz f_max, size_t highlight_bin,
    int8_t pattern_index, uint16_t pattern_correlation, bool pattern_matched
) noexcept;

// AFTER:
void apply_sweep_tracking(
    FreqHz peak_freq, int32_t peak_rssi, FreqHz center_freq,
    FreqHz f_min, FreqHz f_max
) noexcept;
```

---

### 2. `scanner.cpp`

**Remove `refresh_patterns()` method (lines 1371-1380):**
```cpp
// DELETE entire method:
void DroneScanner::refresh_patterns() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (config_.pattern_matching_enabled) {
        pattern_matcher_.set_patterns(
            pattern_manager_.get_patterns_array(),
            pattern_manager_.get_pattern_count()
        );
    }
}
```

**Remove `try_match_pattern_internal()` method (lines 1640-1653):**
```cpp
// DELETE entire method:
PatternMatchResult DroneScanner::try_match_pattern_internal(...) noexcept { ... }
```

**In `process_spectrum_message()` (lines 768-772) — remove pattern matching call:**
```cpp
// DELETE these lines:
    // Pattern matching in normal scan mode: run after shape analysis passes
    PatternMatchResult normal_pattern_result = PatternMatchResult::no_match();
    if (signal_detected && config_.pattern_matching_enabled && pattern_manager_.get_pattern_count() > 0) {
        normal_pattern_result = try_match_pattern_internal(spectrum.db.data(), frequency);
    }
```

**In `process_spectrum_message()` (lines 861-873) — remove pattern propagation:**
```cpp
// DELETE these lines:
        // Propagate pattern match result to tracked drone entry (normal mode)
        if (normal_pattern_result.matched && should_update && drone_idx.has_value()) {
            const SignalPattern* p = pattern_manager_.get_pattern(normal_pattern_result.pattern_index);
            if (p != nullptr && p->name[0] != '\0') {
                auto& d = tracked_drones_[drone_idx.value()];
                d.set_pattern_match(normal_pattern_result.score, p->name);
            }
        }
```

**In `apply_sweep_tracking()` — remove pattern parameters and propagation (lines 1660-1669, 1743-1754):**
- Remove params: `int8_t pattern_index, uint16_t pattern_correlation, bool pattern_matched`
- Remove from function body: the block that propagates pattern match to tracked drone
- Remove `highlight_bin` param too (only used for pattern bin tracking)

**In `process_spectrum_sweep()` (lines 1762-1901) — remove pattern matching code:**
- Remove `clear_matched_pattern()` call (line 1771)
- Remove lines 1860-1876 (early pattern matching block)
- Update `apply_sweep_tracking()` call to remove pattern params (line 1895-1900)

---

### 3. `drone_scanner_ui.hpp`

**Remove button_ptr_ declaration (line 115):**
```cpp
// DELETE this line:
    ui::Button button_ptr_{{UI_POS_X(27), 284, UI_POS_WIDTH(3), 28}, "PTR"};
```

---

### 4. `drone_scanner_ui.cpp`

**Remove button_ptr_ from children list (line 149):**
```cpp
// DELETE:        &button_ptr_
```

**Remove PTR button handler (lines 390-423):**
```cpp
// DELETE entire block:
    // PTR button: open pattern manager view
    button_ptr_.on_select = [this](ui::Button&) { ... };
```

**Remove pattern highlight in sweep (lines 1138-1157):**
```cpp
// DELETE entire block:
        // Update pattern match highlight (red frame) if pattern matching is enabled
        if (scanner_ptr_->is_pattern_matched()) {
            ...
        } else {
            drone_display_.set_matched_pattern_bin(-1);
        }
```

**Remove `#include "pattern_manager_view.hpp"` if present.**

---

### 5. `drone_types.hpp`

**Remove PATTERN_NAME_MAX_LEN (line 16):**
```cpp
// DELETE:
constexpr size_t PATTERN_NAME_MAX_LEN = 28;
```

**Remove pattern fields from TrackedDrone (lines 275-281):**
```cpp
// DELETE:
    // Pattern match state
    bool pattern_matched_{false};
    uint16_t pattern_score_{0};
    char pattern_name_[PATTERN_NAME_MAX_LEN];
```

**Remove set_pattern_match() method (lines 373-382):**
```cpp
// DELETE entire method:
    void set_pattern_match(uint16_t score, const char* name) noexcept { ... }
```

**Remove pattern fields from DisplayDroneEntry (lines 449-451):**
```cpp
// DELETE:
    bool pattern_matched;
    uint16_t pattern_score;
    char pattern_name[16];
```

**Update DisplayDroneEntry comment (line 453):**
```cpp
// BEFORE: // Total: 47 bytes (no vtable, POD type)
// AFTER:  // Total: 28 bytes (no vtable, POD type)
```

---

### 6. `drone_types.cpp`

**Remove pattern fields from TrackedDrone default constructor:**
The constructor at line 70 doesn't explicitly init pattern fields (uses default member initializers), so removing the fields is sufficient.

**Remove pattern fields from DisplayDroneEntry default constructor (lines 271-283):**
```cpp
// DELETE these initializers:
    , pattern_matched{false}
    , pattern_score{0}
    , pattern_name{'\0'}
```

**Remove pattern copy from DisplayDroneEntry(TrackedDrone) constructor (lines 294-313):**
```cpp
// DELETE:
    , pattern_matched(drone.pattern_matched_)
    , pattern_score(drone.pattern_score_)
    , pattern_name{'\0'}
// AND the copy block:
    if (drone.pattern_matched_) {
        size_t j = 0;
        while (j < 15 && drone.pattern_name_[j] != '\0') {
            pattern_name[j] = drone.pattern_name_[j];
            ++j;
        }
        pattern_name[j] = '\0';
    }
```

---

### 7. `drone_display.hpp`

**Remove set_matched_pattern_bin() (line 575):**
```cpp
// DELETE:
    void set_matched_pattern_bin(int16_t bin) noexcept { matched_pattern_bin_ = bin; dirty_flags_ |= DIRTY_SPEC; set_dirty(); }
```

**Remove matched_pattern_bin_ member (lines 616-617):**
```cpp
// DELETE:
    // Pattern match highlight (red frame in sweep)
    int16_t matched_pattern_bin_{-1};
```

---

### 8. `drone_display.cpp`

**Remove "PTR:name" text display (lines 528-538):**
```cpp
// DELETE entire block:
    // Line 3 (optional): Pattern match info — only when pattern is matched
    if (drone.pattern_matched && drone.pattern_name[0] != '\0') {
        char pattern_buf[22];
        ...
    }
```

**Remove red frame drawing (lines 993-1003):**
```cpp
// DELETE entire block:
    // Draw red frame for matched pattern
    if (matched_pattern_bin_ >= 0) {
        const uint16_t frame_x = ...;
        painter.draw_rectangle({...}, Color::red());
    }
```

---

### 9. `drone_settings.hpp`

**Remove check_pattern_matching_ (line 75):**
```cpp
// DELETE:
    ui::Checkbox check_pattern_matching_;
```

---

### 10. `drone_settings.cpp`

**Remove from initializer list (line 66):**
```cpp
// DELETE:
    , check_pattern_matching_({UI_POS_X(10), UI_POS_Y(8)}, 5, "Ptr", false)
```

**Remove from children list (line 129):**
```cpp
// DELETE:         &check_pattern_matching_,
```

**Remove handler (lines 445-449):**
```cpp
// DELETE:
    // Pattern matching toggle
    check_pattern_matching_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.pattern_matching_enabled = v;
        settings_dirty_ = true;
    };
```

**Remove set_value call (line 564):**
```cpp
// DELETE:
    check_pattern_matching_.set_value(settings_.pattern_matching_enabled);
```

---

### 11. `settings_manager.hpp`

**Remove pattern_matching_enabled field (lines 78-79):**
```cpp
// DELETE:
    // Pattern matching
    bool pattern_matching_enabled;
```

---

### 12. `settings_manager.cpp`

**Remove from constructor (line 56):**
```cpp
// DELETE:
    , pattern_matching_enabled(true)
```

**Remove from parse (lines 331-332):**
```cpp
// DELETE:
    } else if (key_matches("pattern_matching_enabled")) {
        s.pattern_matching_enabled = parse_bool();
```

**Remove from save (line 586):**
```cpp
// DELETE:
    wbool(file, "pattern_matching_enabled", s.pattern_matching_enabled);
```

**Remove from apply_to_config (line 628):**
```cpp
// DELETE:
    config.pattern_matching_enabled = s.pattern_matching_enabled;
```

**Remove from save_to_config (line 777):**
```cpp
// DELETE:
    s.pattern_matching_enabled = config.pattern_matching_enabled;
```

---

### 13. `constants.hpp`

**Remove all PATTERN_* constants (lines 870-917):**
```cpp
// DELETE entire block from "Maximum number of patterns" through "PATTERN_BIN_SCALE_FACTOR":
constexpr size_t MAX_PATTERNS = 10;
constexpr size_t PATTERN_WAVEFORM_SIZE = 16;
constexpr uint16_t DEFAULT_PATTERN_SIMILARITY_THRESHOLD = 600;
constexpr uint16_t SIMILARITY_EXCELLENT = 800;
constexpr uint16_t SIMILARITY_STRONG = 600;
constexpr uint16_t SIMILARITY_MODERATE = 400;
constexpr uint8_t PATTERN_MIN_AMPLITUDE_RATIO = 5;
constexpr size_t PATTERN_NORM_EDGE_SKIP = 10;
constexpr uint8_t PATTERN_BIN_SCALE_FACTOR = FFT_BIN_COUNT / PATTERN_WAVEFORM_SIZE;
```

---

### 14. `CMakeLists.txt`

**Remove 4 source file entries (lines 330-333):**
```cmake
# DELETE:
	apps/enhanced_drone_analyzer/pattern_types.cpp
	apps/enhanced_drone_analyzer/pattern_matcher.cpp
	apps/enhanced_drone_analyzer/pattern_manager.cpp
	apps/enhanced_drone_analyzer/pattern_manager_view.cpp
```

---

## Execution Order

1. Delete 8 pattern files (no dependencies)
2. Edit `constants.hpp` (remove PATTERN_* constants)
3. Edit `drone_types.hpp` (remove PATTERN_NAME_MAX_LEN, pattern fields)
4. Edit `drone_types.cpp` (remove pattern init/copy)
5. Edit `scanner.hpp` (remove includes, config fields, members, methods)
6. Edit `scanner.cpp` (remove pattern calls and methods)
7. Edit `drone_scanner_ui.hpp` (remove button_ptr_)
8. Edit `drone_scanner_ui.cpp` (remove PTR handler, highlight code)
9. Edit `drone_display.hpp` (remove matched_pattern_bin_)
10. Edit `drone_display.cpp` (remove red frame, PTR text)
11. Edit `drone_settings.hpp` (remove checkbox)
12. Edit `drone_settings.cpp` (remove checkbox init, handler, placement)
13. Edit `settings_manager.hpp` (remove field)
14. Edit `settings_manager.cpp` (remove all pattern_matching_enabled references)
15. Edit `CMakeLists.txt` (remove source files)

## Verification

After all changes:
- Grep for "pattern" (case-insensitive) in all modified files — should find 0 matches in EDA-specific code
- Grep for "PTR" in all modified files — should find 0 matches
- Verify no broken includes (scanner.hpp no longer includes pattern_matcher.hpp/pattern_manager.hpp)
- Verify no broken references (all method calls to removed methods are gone)
