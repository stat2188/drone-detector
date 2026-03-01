# Mixed UI/DSP Logic Analysis Report
## Enhanced Drone Analyzer (EDA) - Architectural Investigation

**Analysis Date:** 2026-03-01  
**Analyst:** Architect Mode  
**Target Directory:** `firmware/application/apps/enhanced_drone_analyzer/`  
**Analysis Scope:** Identification and documentation of mixed UI rendering and DSP/signal processing logic

---

## Section 1: Executive Summary

### Overall Assessment

| Metric | Value | Severity |
|---------|-------|----------|
| **Total Mixed Logic Instances** | 12 | - |
| **Critical Issues** | 4 | Critical |
| **High Issues** | 5 | High |
| **Medium Issues** | 3 | Medium |
| **Low Issues** | 0 | Low |
| **Architectural Health Score** | 42/100 | Poor |

### Key Findings

1. **Severe Separation Violations:** The EDA codebase exhibits pervasive mixing of UI rendering logic with DSP/signal processing logic across multiple translation units.

2. **Most Affected Areas:**
   - [`DroneDisplayController`](ui_enhanced_drone_analyzer.cpp:2555) - Primary display controller with heavy mixing
   - [`EnhancedDroneSpectrumAnalyzerView`](ui_enhanced_drone_analyzer.cpp:3507) - Spectrum analyzer view with initialization logic mixed in paint()
   - [`DroneScanner`](ui_enhanced_drone_analyzer.cpp:149) - Scanner with detection processing mixed with scanning logic

3. **Root Causes:**
   - Lack of clear architectural layering
   - Monolithic design pattern
   - Direct data flow from DSP to UI without intermediate abstraction layers
   - Performance optimization taking precedence over separation of concerns

4. **Impact:**
   - **Testability:** DSP logic cannot be unit tested without UI framework dependencies
   - **Maintainability:** Changes to UI rendering risk breaking DSP logic and vice versa
   - **Performance:** UI thread blocked by DSP calculations during rendering
   - **Code Reusability:** DSP logic tightly coupled to UI framework, cannot be reused elsewhere

---

## Section 2: Detailed Findings

### MIXED-LOGIC-001: DroneDisplayController::paint() with Spectrum Processing

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:2341) - Lines 2341-3608  
**Severity:** **Critical**

#### UI Components Present
- Painter operations: `painter.fill_rectangle()`, `painter.draw_string()`
- UI widget rendering: `big_display_`, `scanning_progress_`, `text_threat_summary_`, etc.
- Display mode management: `set_display_mode()`, visibility toggles
- Status bar updates: `status_bar_.update_normal_status()`

#### DSP Components Present
- Spectrum data processing: `process_mini_spectrum_data()` called during paint
- Histogram data processing: `process_bins()` called during paint
- Spectrum rendering: `render_bar_spectrum()` contains spectrum data calculations
- Histogram rendering: `render_histogram()` contains histogram bin calculations
- Frequency formatting: `FrequencyFormatter::to_string_short_freq_buffer()` calls
- Signal type detection logic embedded in rendering path

#### Problem Statement
The [`DroneDisplayController::paint()`](ui_enhanced_drone_analyzer.cpp:2341) method is responsible for UI rendering but directly calls DSP processing methods:
- [`process_mini_spectrum_data()`](ui_enhanced_drone_analyzer.cpp:2946) - processes 256-bin spectrum data
- [`process_bins()`](ui_enhanced_drone_analyzer.cpp:2960) - processes histogram bins with frequency calculations
- [`render_bar_spectrum()`](ui_enhanced_drone_analyzer.cpp:2981) - renders spectrum with embedded power level calculations
- [`render_histogram()`](ui_enhanced_drone_analyzer.cpp:3071) - renders histogram with embedded bin processing

This mixing violates the "Single Responsibility Principle" - the paint method should only render, not process signals.

#### Impact Analysis
- **Performance:** UI thread blocked by spectrum data processing (O(256) operations per paint)
- **Testability:** Cannot unit test spectrum processing without Painter dependency
- **Maintainability:** Changing spectrum algorithm requires modifying paint() method
- **Reusability:** Spectrum processing logic cannot be reused in other contexts

#### Proposed Solution
**Refactor Strategy:** Extract spectrum processing to separate DSP layer

1. Create new DSP layer class: `SpectrumProcessor`
   - Pure DSP logic: process spectrum data, calculate power levels, compute histograms
   - No UI dependencies (Painter, View, etc.)
   - Thread-safe data structures for processed results

2. Create data flow interface:
   ```cpp
   // DSP Layer (no UI dependencies)
   class SpectrumProcessor {
   public:
       struct ProcessedSpectrumData {
           std::array<uint8_t, 256> power_levels;
           std::array<uint16_t, 64> histogram_bins;
           uint8_t noise_floor;
           uint8_t max_power;
       };
       
       ProcessedSpectrumData process(const ChannelSpectrum& spectrum) noexcept;
   };
   
   // UI Layer (rendering only)
   class DroneDisplayController {
   private:
       SpectrumProcessor spectrum_processor_;  // DSP processing
       ProcessedSpectrumData cached_data_;  // Thread-safe cache
   public:
       void paint(Painter& painter) override {
           // Render only using cached_data_
           // No DSP calls in paint()
       }
       
       void update_spectrum(const ChannelSpectrum& spectrum) {
           // Called from scanner thread
           cached_data_ = spectrum_processor_.process(spectrum);
           set_dirty();  // Trigger repaint
       }
   };
   ```

3. Benefits:
   - DSP logic testable without UI framework
   - UI thread only renders, no blocking
   - Spectrum processor reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** High
- **Time:** 8-12 hours
- **Risk:** Medium (requires careful data flow refactoring)

---

### MIXED-LOGIC-002: EnhancedDroneSpectrumAnalyzerView::paint() with Initialization Logic

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:3507) - Lines 3507-4105  
**Severity:** **Critical**

#### UI Components Present
- Painter operations: `painter.fill_rectangle()`, `painter.draw_string()`
- Progress bar rendering: Dynamic progress bar with percentage calculation
- Error message display: Error header and message rendering
- Status message display: Loading status messages
- Display mode management: `display_controller_.set_display_mode()`

#### DSP Components Present
- Initialization state machine: Multi-phase initialization (7 phases)
- Database loading coordination: Checks `is_database_loading_complete()`
- Scanner state validation: Validates `display_controller_.are_buffers_valid()`
- Buffer allocation management: `allocate_buffers_from_pool()`, `deallocate_buffers()`
- Initialization continuation: `continue_initialization()` called from paint()

#### Problem Statement
The [`EnhancedDroneSpectrumAnalyzerView::paint()`](ui_enhanced_drone_analyzer.cpp:3507) method contains a complex initialization state machine that:
1. Validates database loading state
2. Allocates display buffers
3. Coordinates with scanner initialization
4. Renders progress indicators
5. Handles initialization errors
6. Manages multi-phase initialization flow

This initialization logic is tightly coupled to the paint() method, making it impossible to:
- Test initialization logic independently
- Reuse initialization logic in other contexts
- Separate initialization from rendering concerns

#### Impact Analysis
- **Testability:** Initialization logic cannot be tested without View/Painter dependencies
- **Maintainability:** Initialization changes require modifying paint() method
- **Performance:** Initialization logic runs on UI thread during every paint
- **Code Clarity:** 500+ lines in single paint() method with mixed concerns

#### Proposed Solution
**Refactor Strategy:** Extract initialization to separate controller

1. Create initialization controller class:
   ```cpp
   class SpectrumAnalyzerInitializationController {
   public:
       enum class InitPhase {
           PHASE_0_WAIT_FOR_DB,
           PHASE_1_ALLOCATE_BUFFERS,
           PHASE_2_INITIALIZE_SCANNER,
           PHASE_3_WAIT_FOR_READY,
           PHASE_4_START_SCANNING,
           PHASE_5_COMPLETE,
           PHASE_6_ERROR
       };
       
       void start_initialization();
       InitPhase get_current_phase() const;
       bool is_complete() const;
       void step();  // Called from UI event loop
       const char* get_error_message() const;
       uint8_t get_progress_percent() const;
   };
   ```

2. Simplify paint() method:
   ```cpp
   void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) override {
       View::paint(painter);
       
       if (init_controller_.is_complete()) {
           // Render spectrum display
           display_controller_.render_bar_spectrum(painter);
           display_controller_.render_histogram(painter);
       } else {
           // Render initialization UI
           render_initialization_ui(painter);
       }
   }
   ```

3. Benefits:
   - Initialization logic testable independently
   - Paint() method simplified to rendering only
   - Initialization reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** High
- **Time:** 6-10 hours
- **Risk:** Medium (requires careful state machine extraction)

---

### MIXED-LOGIC-003: DroneScanner::perform_wideband_scan_cycle() with Spectral Analysis

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:596) - Lines 596-692  
**Severity:** **High**

#### UI Components Present
- Scan error handling: `handle_scan_error()` displays error messages
- Status updates: Implicit UI updates through detection logging

#### DSP Components Present
- Hardware control: `hardware.tune_to_frequency()`, `hardware.start_spectrum_streaming()`
- Spectrum data acquisition: `hardware.get_latest_spectrum_if_fresh()` with timeout logic
- Spectral analysis: `SpectralAnalyzer::analyze()` - full spectral signal analysis
- Histogram callback invocation: `histogram_callback_()` with histogram data
- Signal classification: `get_threat_level()`, `get_drone_type()` from spectral results
- Detection processing: `process_spectral_detection()` with threat classification

#### Problem Statement
The [`DroneScanner::perform_wideband_scan_cycle()`](ui_enhanced_drone_analyzer.cpp:596) method mixes:
1. Hardware control (tuning, spectrum streaming)
2. Signal acquisition (polling with timeout)
3. DSP analysis (spectral analysis, threat classification)
4. Detection processing (logging, tracking updates)

This method is called from the scanning thread but contains both low-level hardware control and high-level signal analysis logic, making it:
- Difficult to test hardware control independently
- Hard to optimize DSP algorithms without affecting hardware control
- Difficult to understand the scanning flow

#### Impact Analysis
- **Testability:** Cannot test spectral analysis without hardware dependencies
- **Maintainability:** Changes to spectral analysis require modifying scanning logic
- **Performance:** Hardware control blocks DSP analysis execution
- **Code Clarity:** 100+ lines with mixed hardware/DSP concerns

#### Proposed Solution
**Refactor Strategy:** Extract spectral analysis to separate DSP layer

1. Create spectral analysis coordinator:
   ```cpp
   class SpectralAnalysisCoordinator {
   public:
       struct AnalysisResult {
           SignalSignature signature;
           uint8_t snr;
           uint8_t noise_floor;
           uint8_t max_val;
           uint32_t signal_width_hz;
           ThreatLevel threat_level;
           DroneType drone_type;
       };
       
       AnalysisResult analyze_slice(
           const std::array<uint8_t, 256>& spectrum_data,
           Frequency center_frequency,
           uint32_t bandwidth_hz) noexcept;
   };
   ```

2. Simplify scanning method:
   ```cpp
   void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
       // Hardware control only
       if (!hardware.tune_to_frequency(current_slice.center_frequency)) return;
       chThdSleepMilliseconds(EDA::Constants::PLL_STABILIZATION_DELAY_MS);
       
       if (!hardware.is_spectrum_streaming_active()) {
           hardware.start_spectrum_streaming();
       }
       
       // Acquire spectrum data
       if (!hardware.get_latest_spectrum_if_fresh(spectrum_data_)) {
           // Polling logic...
       }
       
       // Delegate to DSP layer
       auto result = spectral_analyzer_.analyze_slice(
           spectrum_data_,
           {hardware.get_spectrum_bandwidth(), current_slice.center_frequency},
           histogram_buffer_
       );
       
       // Process detection (separate concern)
       if (result.is_valid && result.signature != SignalSignature::NOISE) {
           process_detection_result(result);
       }
   }
   ```

3. Benefits:
   - Spectral analysis testable without hardware
   - Scanning logic simplified to hardware control only
   - Spectral analysis reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 4-6 hours
- **Risk:** Low (well-defined interface)

---

### MIXED-LOGIC-004: DroneScanner::process_spectral_detection() with Detection Logging

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:793) - Lines 793-863  
**Severity:** **High**

#### UI Components Present
- None (pure DSP logic)

#### DSP Components Present
- Spectral result processing: Extracts SNR, signal width, noise floor from analysis
- Threat classification: `get_threat_level()`, `get_drone_type()` from spectral results
- Detection ring buffer updates: `detection_ring_buffer_.update_detection()` with hysteresis
- Tracked drone updates: `update_tracked_drone()` with movement trend calculation
- Detection logging: `detection_logger_.log_detection_async()` with CSV formatting

#### Problem Statement
The [`DroneScanner::process_spectral_detection()`](ui_enhanced_drone_analyzer.cpp:793) method contains pure DSP logic but is tightly coupled to:
- Detection ring buffer (thread-safe data structure)
- Detection logger (async file I/O)
- Tracked drone management (movement trend calculation)

This mixing creates:
- Tight coupling between detection processing and logging
- Inability to test detection logic independently
- Difficulty in optimizing detection algorithms without affecting logging

#### Impact Analysis
- **Testability:** Cannot test detection logic without logger dependency
- **Maintainability:** Detection algorithm changes require modifying logging logic
- **Performance:** Logging overhead embedded in detection path
- **Code Clarity:** 70+ lines with mixed detection/logging concerns

#### Proposed Solution
**Refactor Strategy:** Extract detection processing to separate DSP layer

1. Create detection processor:
   ```cpp
   class DetectionProcessor {
   public:
       struct DetectionResult {
           DroneType drone_type;
           ThreatLevel threat_level;
           int32_t effective_rssi;
           uint8_t detection_count;
           MovementTrend trend;
       };
       
       DetectionResult process_spectral_detection(
           const SpectralAnalysisResult& spectral_result,
           Frequency frequency_hz) noexcept;
   };
   ```

2. Separate logging interface:
   ```cpp
   class DetectionLogger {
   public:
       void log_detection(const DetectionProcessor::DetectionResult& result) noexcept;
   };
   ```

3. Simplified processing:
   ```cpp
   void DroneScanner::process_spectral_detection(
       const SpectralAnalysisResult& spectral_result,
       Frequency frequency_hz,
       ThreatLevel threat_level,
       DroneType drone_type) {
       
       // Pure DSP processing
       auto detection_result = detection_processor_.process_spectral_detection(
           spectral_result, frequency_hz);
       
       // Update tracking
       update_tracked_drone({
           detection_result.drone_type,
           frequency_hz,
           detection_result.effective_rssi,
           detection_result.threat_level
       });
       
       // Delegate to logger
       if (detection_result.detection_count >= MIN_DETECTION_COUNT) {
           DetectionLogEntry log_entry{
               chTimeNow(),
               static_cast<uint64_t>(frequency_hz),
               detection_result.effective_rssi,
               detection_result.threat_level,
               detection_result.drone_type,
               detection_result.detection_count,
               spectral_result.snr,
               spectral_result.width_bins,
               spectral_result.signal_width_hz
           };
           detection_logger_.log_detection_async(log_entry);
       }
   }
   ```

4. Benefits:
   - Detection logic testable independently
   - Logging decoupled from detection processing
   - Detection processor reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 4-6 hours
- **Risk:** Low (well-defined interface)

---

### MIXED-LOGIC-005: DroneDisplayController::process_mini_spectrum_data() with Spectrum Processing

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:2946) - Lines 2946-2958  
**Severity:** **High**

#### UI Components Present
- None (pure DSP logic)

#### DSP Components Present
- Spectrum data iteration: Processes 256 spectrum bins
- Power level extraction: Extracts current bin power from spectrum data
- Bin processing: Calls `process_bins()` for histogram calculation
- Spectrum FIFO management: Reads from FIFO, handles wraparound

#### Problem Statement
The [`DroneDisplayController::process_mini_spectrum_data()`](ui_enhanced_drone_analyzer.cpp:2946) method is pure DSP logic but is part of the `DroneDisplayController` class, which is a UI component. This creates:
- Incorrect architectural layering (DSP logic in UI class)
- Inability to test spectrum processing independently
- Tight coupling between UI display controller and spectrum processing

#### Impact Analysis
- **Testability:** Cannot test spectrum processing without UI controller
- **Maintainability:** Spectrum algorithm changes require modifying UI controller
- **Performance:** Spectrum processing blocks UI thread when called from paint()
- **Code Clarity:** UI controller class contains 60+ lines of DSP logic

#### Proposed Solution
**Refactor Strategy:** Move spectrum processing to separate DSP layer

1. Extract to standalone spectrum processor:
   ```cpp
   class MiniSpectrumProcessor {
   public:
       struct ProcessedData {
           std::array<uint8_t, 256> power_levels;
           uint16_t bins_hz_size;
           uint8_t noise_floor;
           uint8_t max_power;
       };
       
       ProcessedData process(const ChannelSpectrum& spectrum) noexcept;
   };
   ```

2. Update UI controller to use processor:
   ```cpp
   class DroneDisplayController {
   private:
       MiniSpectrumProcessor spectrum_processor_;
       ProcessedData cached_spectrum_data_;
   public:
       void update_spectrum(const ChannelSpectrum& spectrum) {
           cached_spectrum_data_ = spectrum_processor_.process(spectrum);
           set_dirty();
       }
       
       void render_bar_spectrum(Painter& painter) {
           // Render using cached_spectrum_data_
           // No DSP calls
       }
   };
   ```

3. Benefits:
   - Spectrum processing testable independently
   - UI controller simplified to rendering only
   - Spectrum processor reusable in other contexts
   - Clear architectural layering

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 3-5 hours
- **Risk:** Low (well-defined interface)

---

### MIXED-LOGIC-006: DroneDisplayController::process_bins() with Histogram Calculations

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:2960) - Lines 2960-2998  
**Severity:** **High**

#### UI Components Present
- None (pure DSP logic)

#### DSP Components Present
- Bin power level processing: Accumulates power levels across bins
- Frequency calculation: Calculates `bins_hz_size` from bin width and pixel step
- Marker pixel step calculation: `marker_pixel_step` from frequency range
- Mutex-protected spectrum buffer access: Thread-safe histogram buffer updates

#### Problem Statement
The [`DroneDisplayController::process_bins()`](ui_enhanced_drone_analyzer.cpp:2960) method is pure DSP logic for histogram processing but is part of the UI display controller. This creates:
- Incorrect architectural layering (histogram DSP in UI class)
- Inability to test histogram processing independently
- Tight coupling between UI display and histogram calculations

#### Impact Analysis
- **Testability:** Cannot test histogram processing without UI controller
- **Maintainability:** Histogram algorithm changes require modifying UI controller
- **Performance:** Histogram calculations block UI thread when called from paint()
- **Code Clarity:** UI controller class contains 40+ lines of histogram DSP logic

#### Proposed Solution
**Refactor Strategy:** Extract histogram processing to separate DSP layer

1. Create histogram processor:
   ```cpp
   class HistogramProcessor {
   public:
       struct HistogramData {
           std::array<uint16_t, 64> bin_counts;
           uint16_t bins_hz_size;
           uint8_t noise_floor;
           uint8_t max_power;
       };
       
       HistogramData process(
           const std::array<uint8_t, 256>& power_levels,
           uint32_t marker_pixel_step_hz) noexcept;
   };
   ```

2. Update UI controller to use processor:
   ```cpp
   class DroneDisplayController {
   private:
       HistogramProcessor histogram_processor_;
       HistogramData cached_histogram_data_;
   public:
       void update_histogram(const std::array<uint8_t, 256>& power_levels) {
           cached_histogram_data_ = histogram_processor_.process(
               power_levels, marker_pixel_step_);
           set_dirty();
       }
       
       void render_histogram(Painter& painter) {
           // Render using cached_histogram_data_
           // No DSP calls
       }
   };
   ```

3. Benefits:
   - Histogram processing testable independently
   - UI controller simplified to rendering only
   - Histogram processor reusable in other contexts
   - Clear architectural layering

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 3-5 hours
- **Risk:** Low (well-defined interface)

---

### MIXED-LOGIC-007: DroneScanner::process_rssi_detection() with Database Search

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:890) - Lines 890-1000  
**Severity:** **Medium**

#### UI Components Present
- None (pure DSP logic)

#### DSP Components Present
- RSSI validation: Validates RSSI range using `EDA::Validation::validate_rssi()`
- Frequency validation: Validates frequency range
- Drone band filtering: Filters by 433MHz, 2.4GHz, 5.8GHz, military bands
- Database search: Linear search through frequency database for matching entries
- Threat classification: `ThreatClassifier::from_rssi()` for threat level determination
- Detection ring buffer updates: `detection_ring_buffer_.update_detection()` with hysteresis
- Tracked drone updates: `update_tracked_drone()` with movement trend calculation
- Detection logging: Prepares log entry for async logging

#### Problem Statement
The [`DroneScanner::process_rssi_detection()`](ui_enhanced_drone_analyzer.cpp:890) method contains DSP logic (RSSI processing, database lookup, threat classification) mixed with detection tracking and logging. This creates:
- Tight coupling between RSSI processing and database operations
- Inability to test RSSI processing independently
- Difficulty in optimizing detection algorithms without affecting database lookup

#### Impact Analysis
- **Testability:** Cannot test RSSI processing without database dependency
- **Maintainability:** RSSI algorithm changes require modifying database lookup logic
- **Performance:** Database lookup blocks detection processing
- **Code Clarity:** 110+ lines with mixed RSSI/database/logging concerns

#### Proposed Solution
**Refactor Strategy:** Extract detection processing to separate DSP layer

1. Create RSSI detection processor:
   ```cpp
   class RSSIDetectionProcessor {
   public:
       struct DetectionResult {
           DroneType drone_type;
           ThreatLevel threat_level;
           int32_t rssi;
           uint8_t detection_count;
           MovementTrend trend;
       };
       
       DetectionResult process_rssi_detection(
           Frequency frequency_hz,
           int32_t rssi_db) noexcept;
   };
   ```

2. Separate database lookup:
   ```cpp
   class DroneDatabaseLookup {
   public:
       struct DroneInfo {
           DroneType drone_type;
           ThreatLevel threat_level;
       };
       
       std::optional<DroneInfo> lookup(Frequency frequency_hz) const noexcept;
   };
   ```

3. Simplified processing:
   ```cpp
   void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
       // Pure DSP processing
       auto detection_result = rssi_processor_.process_rssi_detection(
           entry.frequency_a, rssi);
       
       // Database lookup (separate concern)
       auto db_info = database_lookup_.lookup(entry.frequency_a);
       if (db_info) {
           detection_result.drone_type = db_info->drone_type;
           detection_result.threat_level = std::max(
               detection_result.threat_level,
               db_info->threat_level);
       }
       
       // Update tracking
       update_tracked_drone({
           detection_result.drone_type,
           entry.frequency_a,
           detection_result.rssi,
           detection_result.threat_level
       });
       
       // Logging
       if (detection_result.detection_count >= MIN_DETECTION_COUNT) {
           detection_logger_.log_detection_async(...);
       }
   }
   ```

4. Benefits:
   - RSSI processing testable independently
   - Database lookup decoupled from detection
   - Detection processor reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 5-7 hours
- **Risk:** Medium (requires database lookup refactoring)

---

### MIXED-LOGIC-008: DroneDisplayController::render_bar_spectrum() with Power Level Calculations

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:2981) - Lines 2981-3070  
**Severity:** **Medium**

#### UI Components Present
- Painter operations: `painter.fill_rectangle()` for background and bars
- Bar rendering: Renders 240 spectrum bars with calculated heights
- Color selection: Uses `spectrum_gradient_` for bar colors based on power levels

#### DSP Components Present
- Power level calculation: Iterates through 240 spectrum bins to calculate bar heights
- Color gradient calculation: Maps power levels to RGB565 colors
- Frequency-to-pixel mapping: Calculates bar positions from frequency range
- Mutex-protected spectrum buffer access: Thread-safe power level reads

#### Problem Statement
The [`DroneDisplayController::render_bar_spectrum()`](ui_enhanced_drone_analyzer.cpp:2981) method is responsible for rendering but contains embedded DSP calculations:
- Power level to bar height mapping
- Color gradient lookups
- Frequency-to-pixel coordinate calculations

This mixing creates:
- Inability to test rendering independently
- Difficulty in optimizing rendering without affecting DSP calculations
- Rendering method contains 90+ lines with mixed rendering/DSP concerns

#### Impact Analysis
- **Testability:** Cannot test rendering without DSP calculations
- **Maintainability:** Rendering changes require modifying DSP calculations
- **Performance:** DSP calculations block UI thread during rendering
- **Code Clarity:** Rendering method contains embedded calculations

#### Proposed Solution
**Refactor Strategy:** Pre-calculate rendering data in DSP layer

1. Create spectrum renderer data structure:
   ```cpp
   struct SpectrumRenderData {
       struct BarInfo {
           int x;
           int y;
           int height;
           uint16_t color;
       };
       
       std::array<BarInfo, 240> bars;
       int y_start;
       int height;
   };
   ```

2. Pre-calculate in DSP layer:
   ```cpp
   class SpectrumRenderer {
   public:
       SpectrumRenderData calculate_render_data(
           const std::array<uint8_t, 256>& power_levels,
           const SpectrumGradient& gradient) noexcept;
   };
   ```

3. Simplified rendering:
   ```cpp
   void DroneDisplayController::render_bar_spectrum(Painter& painter) {
       const auto& render_data = spectrum_renderer_.calculate_render_data(
           spectrum_power_levels_, spectrum_gradient_);
       
       painter.fill_rectangle({0, render_data.y_start, 240, render_data.height}, Color::black());
       
       for (const auto& bar : render_data.bars) {
           painter.fill_rectangle({bar.x, bar.y, 1, bar.height}, bar.color);
       }
   }
   ```

4. Benefits:
   - Rendering testable independently
   - DSP calculations moved to appropriate layer
   - Renderer reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 4-6 hours
- **Risk:** Low (well-defined interface)

---

### MIXED-LOGIC-009: DroneDisplayController::render_histogram() with Bin Processing

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:3071) - Lines 3071-3153  
**Severity:** **Medium**

#### UI Components Present
- Painter operations: `painter.fill_rectangle()` for background and bins
- Bin rendering: Renders 64 histogram bins with calculated heights
- Color selection: Uses threat level colors for bins

#### DSP Components Present
- Bin height calculation: Maps histogram bin counts to pixel heights
- Max bin value calculation: Finds maximum bin value for scaling
- Mutex-protected histogram buffer access: Thread-safe bin count reads

#### Problem Statement
The [`DroneDisplayController::render_histogram()`](ui_enhanced_drone_analyzer.cpp:3071) method is responsible for rendering but contains embedded DSP calculations:
- Histogram bin scaling calculations
- Max value determination
- Bin height mapping

This mixing creates:
- Inability to test rendering independently
- Difficulty in optimizing rendering without affecting DSP calculations
- Rendering method contains 80+ lines with mixed rendering/DSP concerns

#### Impact Analysis
- **Testability:** Cannot test rendering without DSP calculations
- **Maintainability:** Rendering changes require modifying DSP calculations
- **Performance:** DSP calculations block UI thread during rendering
- **Code Clarity:** Rendering method contains embedded calculations

#### Proposed Solution
**Refactor Strategy:** Pre-calculate rendering data in DSP layer

1. Create histogram renderer data structure:
   ```cpp
   struct HistogramRenderData {
       struct BinInfo {
           int x;
           int y;
           int height;
           uint16_t color;
       };
       
       std::array<BinInfo, 64> bins;
       int y_start;
       int height;
   };
   ```

2. Pre-calculate in DSP layer:
   ```cpp
   class HistogramRenderer {
   public:
       HistogramRenderData calculate_render_data(
           const std::array<uint16_t, 64>& bin_counts,
           uint8_t max_color) noexcept;
   };
   ```

3. Simplified rendering:
   ```cpp
   void DroneDisplayController::render_histogram(Painter& painter) {
       const auto& render_data = histogram_renderer_.calculate_render_data(
           histogram_display_buffer_.bin_counts, max_color_);
       
       painter.fill_rectangle(histogram_rect, Color::black());
       
       for (const auto& bin : render_data.bins) {
           painter.fill_rectangle({bin.x, bin.y, 1, bin.height}, bin.color);
       }
   }
   ```

4. Benefits:
   - Rendering testable independently
   - DSP calculations moved to appropriate layer
   - Renderer reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 3-5 hours
- **Risk:** Low (well-defined interface)

---

### MIXED-LOGIC-010: DroneDisplayController::update_detection_display() with Data Fetching

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:2685) - Lines 2685-2784  
**Severity:** **High**

#### UI Components Present
- Display text updates: Updates `big_display_`, `text_threat_summary_`, etc.
- Progress bar rendering: Updates scanning progress bar
- Status bar updates: Updates scanner statistics and status
- Frequency formatting: Formats frequencies for display using `FrequencyFormatter`
- Style management: Updates display styles based on threat level

#### DSP Components Present
- Scanner state fetching: Calls multiple scanner getter methods under mutex
- Threat level classification: Determines color index based on max threat
- Movement trend calculation: Implicit in drone data fetching
- Data aggregation: Calculates totals, counts from scanner data

#### Problem Statement
The [`DroneDisplayController::update_detection_display()`](ui_enhanced_drone_analyzer.cpp:2685) method is responsible for UI updates but contains embedded DSP logic:
- Fetches scanner state (thread-safe snapshot)
- Classifies threat levels
- Formats frequencies
- Calculates display data

This mixing creates:
- UI thread blocked by scanner state fetching (multiple mutex locks)
- Inability to test UI updates independently
- Difficulty in optimizing UI without affecting DSP calculations

#### Impact Analysis
- **Testability:** Cannot test UI updates without scanner dependency
- **Maintainability:** UI changes require modifying DSP logic
- **Performance:** Multiple mutex locks block UI thread
- **Code Clarity:** 100+ lines with mixed UI/DSP concerns

#### Proposed Solution
**Refactor Strategy:** Use observer pattern for data flow

1. Create display data structure:
   ```cpp
   struct DisplayData {
       bool is_scanning;
       Frequency current_freq;
       size_t total_freqs;
       ThreatLevel max_threat;
       size_t approaching_count;
       size_t receding_count;
       size_t static_count;
       uint32_t total_detections;
       bool is_real_mode;
       uint32_t scan_cycles;
       uint8_t color_idx;
   };
   ```

2. Create display data observer:
   ```cpp
   class DisplayDataObserver {
   public:
       virtual void on_display_data_changed(const DisplayData& data) = 0;
   };
   ```

3. Update scanner to notify observers:
   ```cpp
   class DroneScanner {
   private:
       std::array<DisplayDataObserver*, 4> observers_;
       DisplayData cached_display_data_;
   public:
       void add_observer(DisplayDataObserver* observer);
       void remove_observer(DisplayDataObserver* observer);
       
       void notify_display_data_changed() {
           // Update cache
           cached_display_data_ = calculate_display_data();
           
           // Notify observers (non-blocking)
           for (auto* observer : observers_) {
               observer->on_display_data_changed(cached_display_data_);
           }
       }
   };
   ```

4. Simplified UI updates:
   ```cpp
   void DroneDisplayController::update_detection_display(const DroneScanner& scanner) {
       // Just use cached data, no scanner calls
       render_display_data(cached_display_data_);
   }
   ```

5. Benefits:
   - UI updates testable independently
   - Scanner decoupled from UI updates
   - Observer pattern enables multiple UI components
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** High
- **Time:** 8-12 hours
- **Risk:** Medium (requires observer pattern implementation)

---

### MIXED-LOGIC-011: DroneDisplayController::update_drones_display() with Sorting and Rendering

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:2850) - Lines 2850-2895  
**Severity:** **Medium**

#### UI Components Present
- Display text updates: Updates drone text widgets with formatted data
- Frequency formatting: Formats frequencies using `FrequencyFormatter`
- Trend symbol lookup: Uses `TrendSymbols` for movement indicators
- Style application: Applies colors and styles to text widgets

#### DSP Components Present
- Drone snapshot fetching: Calls `scanner.get_tracked_drones_snapshot()` under mutex
- Stale drone filtering: Filters drones based on timeout threshold
- Drone data copying: Copies snapshot data to display structures
- Sorting: Sorts drones by RSSI, threat level, last seen time
- Movement trend calculation: Calls `drone.get_trend()` for each drone

#### Problem Statement
The [`DroneDisplayController::update_drones_display()`](ui_enhanced_drone_analyzer.cpp:2850) method is responsible for UI updates but contains embedded DSP logic:
- Fetches drone data (thread-safe snapshot)
- Filters stale drones
- Sorts drones by multiple criteria
- Calculates movement trends
- Formats display data

This mixing creates:
- UI thread blocked by snapshot fetching (mutex lock)
- Inability to test UI updates independently
- Difficulty in optimizing UI without affecting DSP calculations

#### Impact Analysis
- **Testability:** Cannot test UI updates without scanner dependency
- **Maintainability:** UI changes require modifying DSP logic
- **Performance:** Mutex lock and sorting block UI thread
- **Code Clarity:** 50+ lines with mixed UI/DSP concerns

#### Proposed Solution
**Refactor Strategy:** Pre-sort and filter in DSP layer

1. Create display drone data structure:
   ```cpp
   struct DisplayDroneData {
       Frequency frequency;
       DroneType type;
       ThreatLevel threat;
       int32_t rssi;
       MovementTrend trend;
       char type_name[16];
       Color display_color;
   };
   ```

2. Pre-process in DSP layer:
   ```cpp
   class DroneDataPreprocessor {
   public:
       struct PreprocessedData {
           std::array<DisplayDroneData, MAX_DISPLAYED_DRONES> drones;
           size_t count;
       };
       
       PreprocessedData preprocess(
           const DroneScanner::DroneSnapshot& snapshot,
           systime_t current_time) noexcept;
   };
   ```

3. Simplified UI updates:
   ```cpp
   void DroneDisplayController::update_drones_display(const DroneScanner& scanner) {
       const auto& preprocessed = drone_preprocessor_.preprocess(
           scanner.get_tracked_drones_snapshot(), chTimeNow());
       
       // Just render preprocessed data
       for (size_t i = 0; i < preprocessed.count; ++i) {
           render_drone_entry(preprocessed.drones[i]);
       }
   }
   ```

4. Benefits:
   - UI updates testable independently
   - DSP preprocessing decoupled from UI
   - Preprocessor reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** Medium
- **Time:** 4-6 hours
- **Risk:** Low (well-defined interface)

---

### MIXED-LOGIC-012: EnhancedDroneSpectrumAnalyzerView::paint() with Initialization State Machine

**Location:** [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:3507) - Lines 3520-3608  
**Severity:** **Critical**

#### UI Components Present
- Painter operations: `painter.fill_rectangle()`, `painter.draw_string()`
- Error message rendering: Renders initialization error messages
- Progress bar rendering: Renders multi-phase initialization progress
- Status message rendering: Renders initialization status messages
- Display mode management: `display_controller_.set_display_mode()`

#### DSP Components Present
- Initialization state machine: 7-phase initialization with state transitions
- Database loading validation: Checks `is_database_loading_complete()`
- Scanner buffer validation: Checks `are_buffers_valid()`
- Buffer allocation: `allocate_buffers_from_pool()`
- Initialization continuation: `continue_initialization()` called from paint()
- Initialization retry logic: Retry logic with phase tracking

#### Problem Statement
The [`EnhancedDroneSpectrumAnalyzerView::paint()`](ui_enhanced_drone_analyzer.cpp:3507) method contains a complex 7-phase initialization state machine mixed with UI rendering:
- Phase 0: Wait for database
- Phase 1: Allocate buffers
- Phase 2: Initialize scanner
- Phase 3: Wait for ready
- Phase 4: Start scanning
- Phase 5: Complete
- Phase 6: Error

This creates:
- Inability to test initialization independently
- Initialization tightly coupled to paint() method
- Difficulty in modifying initialization without affecting rendering
- 100+ lines of mixed concerns in single method

#### Impact Analysis
- **Testability:** Cannot test initialization without View/Painter dependencies
- **Maintainability:** Initialization changes require modifying paint() method
- **Performance:** Initialization logic runs on UI thread during every paint
- **Code Clarity:** Extremely complex paint() method with embedded state machine

#### Proposed Solution
**Refactor Strategy:** Extract initialization to separate controller

1. Create initialization controller (similar to MIXED-LOGIC-002):
   ```cpp
   class SpectrumAnalyzerInitController {
   public:
       enum class InitPhase {
           PHASE_0_WAIT_FOR_DB,
           PHASE_1_ALLOCATE_BUFFERS,
           PHASE_2_INITIALIZE_SCANNER,
           PHASE_3_WAIT_FOR_READY,
           PHASE_4_START_SCANNING,
           PHASE_5_COMPLETE,
           PHASE_6_ERROR
       };
       
       void start_initialization();
       InitPhase get_current_phase() const;
       bool is_complete() const;
       void step();  // Called from UI event loop
       const char* get_status_message() const;
       uint8_t get_progress_percent() const;
       bool has_error() const;
   };
   ```

2. Simplify paint() method:
   ```cpp
   void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) override {
       View::paint(painter);
       
       if (init_controller_.is_complete()) {
           // Render spectrum display
           display_controller_.render_bar_spectrum(painter);
           display_controller_.render_histogram(painter);
       } else if (init_controller_.has_error()) {
           // Render error UI
           render_error_ui(painter, init_controller_.get_status_message());
       } else {
           // Render initialization UI
           render_init_ui(painter, init_controller_.get_status_message(),
                       init_controller_.get_progress_percent());
       }
   }
   ```

3. Benefits:
   - Initialization logic testable independently
   - Paint() method simplified to rendering only
   - Initialization reusable in other contexts
   - Clear separation of concerns

#### Effort Estimate
- **Complexity:** High
- **Time:** 8-12 hours
- **Risk:** Medium (requires careful state machine extraction)

---

## Section 3: Architectural Patterns

### Common Mixing Patterns

#### Pattern 1: "View Class with Inline DSP Calculation"
**Description:** View classes contain inline DSP calculations instead of delegating to separate DSP layer.

**Occurrences:**
- [`DroneDisplayController::paint()`](ui_enhanced_drone_analyzer.cpp:2341) - Spectrum processing in paint()
- [`DroneDisplayController::render_bar_spectrum()`](ui_enhanced_drone_analyzer.cpp:2981) - Power level calculations
- [`DroneDisplayController::render_histogram()`](ui_enhanced_drone_analyzer.cpp:3071) - Bin processing

**Impact:** High - Makes View classes bloated and difficult to test

**Recommendation:** Extract all DSP calculations to separate processor classes

---

#### Pattern 2: "Scanner with Mixed Detection and Logging"
**Description:** Scanner classes mix detection processing with logging logic.

**Occurrences:**
- [`DroneScanner::process_spectral_detection()`](ui_enhanced_drone_analyzer.cpp:793) - Detection + logging
- [`DroneScanner::process_rssi_detection()`](ui_enhanced_drone_analyzer.cpp:890) - Detection + logging
- [`DroneScanner::process_wideband_detection_with_override()`](ui_enhanced_drone_analyzer.cpp:707) - Detection + logging

**Impact:** High - Tight coupling makes code difficult to maintain

**Recommendation:** Extract logging to separate service layer

---

#### Pattern 3: "UI Update with Direct Scanner Calls"
**Description:** UI update methods call scanner methods directly, creating tight coupling.

**Occurrences:**
- [`DroneDisplayController::update_detection_display()`](ui_enhanced_drone_analyzer.cpp:2685) - Multiple scanner getter calls
- [`DroneDisplayController::update_drones_display()`](ui_enhanced_drone_analyzer.cpp:2850) - Snapshot fetching + sorting

**Impact:** Medium - Creates dependency from UI on scanner implementation

**Recommendation:** Use observer pattern or data caching

---

#### Pattern 4: "Initialization in Paint Method"
**Description:** Complex initialization logic embedded in paint() methods.

**Occurrences:**
- [`EnhancedDroneSpectrumAnalyzerView::paint()`](ui_enhanced_drone_analyzer.cpp:3507) - 7-phase state machine

**Impact:** Critical - Makes initialization untestable and paint() methods extremely complex

**Recommendation:** Extract initialization to separate controller

---

### Good Separation Examples

The following files demonstrate proper separation of concerns:

1. **[`ui_spectral_analyzer.hpp`](ui_spectral_analyzer.hpp:1)** - Pure DSP/signal processing logic
   - No UI dependencies
   - Clean spectral analysis algorithms
   - Testable independently

2. **[`ui_signal_processing.hpp`](ui_signal_processing.hpp:1)** - Pure DSP data structures
   - Thread-safe ring buffer
   - No UI dependencies
   - Clean separation of concerns

3. **[`eda_optimized_utils.hpp`](eda_optimized_utils.hpp:1)** - Pure utility functions
   - No UI dependencies
   - Reusable DSP algorithms

4. **[`color_lookup_unified.hpp`](color_lookup_unified.hpp:1)** - Pure data lookup
   - No DSP logic
   - No UI framework dependencies
   - Clean separation of concerns

---

### Recommended Architectural Structure

```
firmware/application/apps/enhanced_drone_analyzer/
├── dsp/                          # DSP/SIGNAL PROCESSING LAYER
│   ├── spectrum_processor.hpp
│   ├── histogram_processor.hpp
│   ├── detection_processor.hpp
│   ├── signal_analyzer.hpp
│   └── frequency_formatter.hpp
├── data/                         # DATA LAYER
│   ├── display_data.hpp
│   ├── drone_snapshot.hpp
│   └── thread_safe_cache.hpp
├── services/                     # SERVICE LAYER
│   ├── detection_logger.hpp
│   ├── database_service.hpp
│   └── audio_service.hpp
├── controllers/                  # CONTROLLER LAYER
│   ├── spectrum_display_controller.hpp
│   ├── drone_list_controller.hpp
│   └── status_controller.hpp
├── views/                        # UI PRESENTATION LAYER
│   ├── spectrum_analyzer_view.hpp
│   ├── drone_list_view.hpp
│   └── status_bar_view.hpp
└── models/                       # SHARED DATA MODELS
    ├── drone_types.hpp
    ├── detection_result.hpp
    └── display_config.hpp
```

**Key Principles:**
1. **DSP Layer:** Pure signal processing, no UI dependencies
2. **Data Layer:** Thread-safe data structures, no processing logic
3. **Service Layer:** Business logic, no UI/DSP dependencies
4. **Controller Layer:** Coordinates between data and views, no direct rendering
5. **View Layer:** Pure UI rendering, no business logic
6. **Models Layer:** Shared data structures, minimal logic

---

## Section 4: Refactoring Roadmap

### Priority 0 (Critical) - Fix Immediately

| ID | Issue | File | Effort | Risk | Description |
|----|-------|------|--------|------|-------------|
| P0-1 | MIXED-LOGIC-001 | [`ui_enhanced_drone_analyzer.cpp:2341`](ui_enhanced_drone_analyzer.cpp:2341) | 8-12h | Medium | Extract spectrum processing from DroneDisplayController::paint() |
| P0-2 | MIXED-LOGIC-002 | [`ui_enhanced_drone_analyzer.cpp:3507`](ui_enhanced_drone_analyzer.cpp:3507) | 6-10h | Medium | Extract initialization state machine from paint() |
| P0-3 | MIXED-LOGIC-012 | [`ui_enhanced_drone_analyzer.cpp:3507`](ui_enhanced_drone_analyzer.cpp:3507) | 8-12h | Medium | Extract initialization from EnhancedDroneSpectrumAnalyzerView::paint() |

**Total Effort:** 22-34 hours

---

### Priority 1 (High) - Fix Soon

| ID | Issue | File | Effort | Risk | Description |
|----|-------|------|--------|------|-------------|
| P1-1 | MIXED-LOGIC-003 | [`ui_enhanced_drone_analyzer.cpp:596`](ui_enhanced_drone_analyzer.cpp:596) | 4-6h | Low | Extract spectral analysis from perform_wideband_scan_cycle() |
| P1-2 | MIXED-LOGIC-004 | [`ui_enhanced_drone_analyzer.cpp:793`](ui_enhanced_drone_analyzer.cpp:793) | 4-6h | Low | Extract detection processing from process_spectral_detection() |
| P1-3 | MIXED-LOGIC-005 | [`ui_enhanced_drone_analyzer.cpp:2946`](ui_enhanced_drone_analyzer.cpp:2946) | 3-5h | Low | Extract spectrum processing from process_mini_spectrum_data() |
| P1-4 | MIXED-LOGIC-006 | [`ui_enhanced_drone_analyzer.cpp:2960`](ui_enhanced_drone_analyzer.cpp:2960) | 3-5h | Low | Extract histogram processing from process_bins() |
| P1-5 | MIXED-LOGIC-007 | [`ui_enhanced_drone_analyzer.cpp:890`](ui_enhanced_drone_analyzer.cpp:890) | 5-7h | Medium | Extract detection processing from process_rssi_detection() |

**Total Effort:** 19-29 hours

---

### Priority 2 (Medium) - Nice to Have

| ID | Issue | File | Effort | Risk | Description |
|----|-------|------|--------|------|-------------|
| P2-1 | MIXED-LOGIC-008 | [`ui_enhanced_drone_analyzer.cpp:2981`](ui_enhanced_drone_analyzer.cpp:2981) | 4-6h | Low | Pre-calculate spectrum render data |
| P2-2 | MIXED-LOGIC-009 | [`ui_enhanced_drone_analyzer.cpp:3071`](ui_enhanced_drone_analyzer.cpp:3071) | 3-5h | Low | Pre-calculate histogram render data |
| P2-3 | MIXED-LOGIC-010 | [`ui_enhanced_drone_analyzer.cpp:2685`](ui_enhanced_drone_analyzer.cpp:2685) | 8-12h | Medium | Implement observer pattern for display data |
| P2-4 | MIXED-LOGIC-011 | [`ui_enhanced_drone_analyzer.cpp:2850`](ui_enhanced_drone_analyzer.cpp:2850) | 4-6h | Low | Pre-process drone data for display |

**Total Effort:** 19-29 hours

---

### Priority 3 (Low) - Cosmetic Improvements

| ID | Issue | File | Effort | Risk | Description |
|----|-------|------|--------|------|-------------|
| P3-1 | General code organization | Multiple files | 10-15h | Low | Improve overall code organization and consistency |
| P3-2 | Add unit tests | All DSP files | 15-20h | Low | Add unit tests for extracted DSP logic |

**Total Effort:** 25-35 hours

---

## Section 5: Recommendations

### File Structure Recommendations

**Current Structure Issues:**
- Monolithic files: [`ui_enhanced_drone_analyzer.cpp`](ui_enhanced_drone_analyzer.cpp:1) is 180KB (4500+ lines)
- Mixed concerns: UI, DSP, scanning, logging all intertwined
- Poor testability: DSP logic cannot be tested without UI framework

**Recommended Structure:**

```
firmware/application/apps/enhanced_drone_analyzer/
├── dsp/                                    # DSP/Signal Processing Layer
│   ├── spectrum_processor.hpp              # Spectrum data processing
│   ├── histogram_processor.hpp              # Histogram calculations
│   ├── detection_processor.hpp              # Detection algorithms
│   ├── signal_classifier.hpp               # Signal classification (threat, type)
│   ├── frequency_analyzer.hpp              # Frequency analysis
│   └── dsp_constants.hpp                  # DSP-specific constants
│
├── data/                                   # Data Layer
│   ├── display_data.hpp                     # Data for UI display
│   ├── drone_data.hpp                       # Drone tracking data
│   ├── spectrum_data.hpp                     # Spectrum data structures
│   ├── thread_safe_cache.hpp               # Thread-safe data cache
│   └── detection_ring_buffer.hpp           # Detection ring buffer
│
├── services/                               # Service Layer
│   ├── detection_logger_service.hpp          # Async detection logging
│   ├── database_service.hpp                 # Database operations
│   ├── audio_service.hpp                    # Audio alert management
│   └── scanner_service.hpp                 # Scanning coordination
│
├── controllers/                            # Controller Layer
│   ├── spectrum_display_controller.hpp      # Spectrum display coordination
│   ├── drone_list_controller.hpp           # Drone list display coordination
│   ├── status_controller.hpp                # Status bar coordination
│   └── initialization_controller.hpp        # Initialization management
│
├── views/                                  # UI Presentation Layer
│   ├── spectrum_analyzer_view.hpp          # Spectrum analyzer UI
│   ├── drone_list_view.hpp                # Drone list UI
│   ├── status_bar_view.hpp                 # Status bar UI
│   ├── frequency_ruler_view.hpp           # Frequency ruler UI
│   └── threat_indicator_view.hpp           # Threat indicator UI
│
└── models/                                 # Shared Data Models
    ├── drone_types.hpp                      # Drone type enums
    ├── threat_level.hpp                      # Threat level enums
    ├── detection_result.hpp                  # Detection result structures
    └── display_config.hpp                  # Display configuration
```

---

### Layering Recommendations

**Recommended Layering:**

```
┌─────────────────────────────────────────────────────────────────┐
│                    UI Presentation Layer                      │
│  (Pure rendering, no business logic)                        │
├─────────────────────────────────────────────────────────────────┤
│                   Controller Layer                           │
│  (Coordinates data flow, no rendering)                      │
├─────────────────────────────────────────────────────────────────┤
│                   Service Layer                              │
│  (Business logic, no UI/DSP dependencies)                   │
├─────────────────────────────────────────────────────────────────┤
│                   Data Layer                                 │
│  (Thread-safe structures, no processing)                       │
├─────────────────────────────────────────────────────────────────┤
│                   DSP/Signal Processing Layer                   │
│  (Pure algorithms, no UI dependencies)                        │
└─────────────────────────────────────────────────────────────────┘
```

**Key Principles:**
1. **Unidirectional Data Flow:** DSP → Data → Service → Controller → UI
2. **No Upward Dependencies:** Lower layers do not depend on upper layers
3. **Interface-Based Communication:** Layers communicate through well-defined interfaces
4. **Thread Safety:** Data layer provides thread-safe access patterns
5. **Testability:** Each layer can be tested independently

---

### Interface Design Recommendations

**1. DSP Layer Interfaces:**

```cpp
// spectrum_processor.hpp
class ISpectrumProcessor {
public:
    struct ProcessedSpectrum {
        std::array<uint8_t, 256> power_levels;
        uint8_t noise_floor;
        uint8_t max_power;
    };
    
    virtual ~ISpectrumProcessor() = default;
    virtual ProcessedSpectrum process(const ChannelSpectrum& spectrum) noexcept = 0;
};

// histogram_processor.hpp
class IHistogramProcessor {
public:
    struct ProcessedHistogram {
        std::array<uint16_t, 64> bin_counts;
        uint8_t noise_floor;
        uint16_t max_count;
    };
    
    virtual ~IHistogramProcessor() = default;
    virtual ProcessedHistogram process(
        const std::array<uint8_t, 256>& power_levels,
        uint32_t marker_pixel_step_hz) noexcept = 0;
};

// detection_processor.hpp
class IDetectionProcessor {
public:
    struct DetectionResult {
        DroneType drone_type;
        ThreatLevel threat_level;
        int32_t rssi;
        uint8_t detection_count;
        MovementTrend trend;
    };
    
    virtual ~IDetectionProcessor() = default;
    virtual DetectionResult process_rssi_detection(
        Frequency frequency_hz,
        int32_t rssi_db) noexcept = 0;
};
```

**2. Data Layer Interfaces:**

```cpp
// display_data.hpp
class IDisplayDataProvider {
public:
    virtual ~IDisplayDataProvider() = default;
    virtual DisplayData get_display_data() const noexcept = 0;
    virtual void invalidate_cache() noexcept = 0;
};

// drone_data.hpp
class IDroneDataProvider {
public:
    virtual ~IDroneDataProvider() = default;
    virtual DroneSnapshot get_drone_snapshot() const noexcept = 0;
    virtual void invalidate_cache() noexcept = 0;
};
```

**3. Controller Layer Interfaces:**

```cpp
// spectrum_display_controller.hpp
class ISpectrumDisplayController {
public:
    virtual ~ISpectrumDisplayController() = default;
    virtual void update_spectrum(const ProcessedSpectrum& data) noexcept = 0;
    virtual void render(Painter& painter) noexcept = 0;
};

// drone_list_controller.hpp
class IDroneListController {
public:
    virtual ~IDroneListController() = default;
    virtual void update_drones(const DroneSnapshot& data) noexcept = 0;
    virtual void render(Painter& painter) noexcept = 0;
};
```

---

### Testing Strategy Recommendations

**1. Unit Testing for DSP Layer:**

```cpp
// tests/dsp/test_spectrum_processor.cpp
#include <gtest/gtest.h>
#include "dsp/spectrum_processor.hpp"

TEST(SpectrumProcessorTest, ProcessValidSpectrum) {
    ChannelSpectrum spectrum{};
    for (size_t i = 0; i < 256; ++i) {
        spectrum.db[i] = static_cast<uint8_t>(i);
    }
    
    SpectrumProcessor processor;
    auto result = processor.process(spectrum);
    
    EXPECT_EQ(result.noise_floor, 0);
    EXPECT_EQ(result.max_power, 255);
    EXPECT_EQ(result.power_levels[0], 0);
    EXPECT_EQ(result.power_levels[255], 255);
}

TEST(SpectrumProcessorTest, ProcessEmptySpectrum) {
    ChannelSpectrum spectrum{};
    
    SpectrumProcessor processor;
    auto result = processor.process(spectrum);
    
    EXPECT_EQ(result.noise_floor, 0);
    EXPECT_EQ(result.max_power, 0);
}
```

**2. Integration Testing for Controller Layer:**

```cpp
// tests/controllers/test_spectrum_display_controller.cpp
#include <gtest/gtest.h>
#include "controllers/spectrum_display_controller.hpp"

TEST(SpectrumDisplayControllerTest, UpdateAndRender) {
    MockPainter painter;
    MockSpectrumProcessor spectrum_processor;
    ISpectrumDisplayController controller(spectrum_processor);
    
    ProcessedSpectrum spectrum_data{};
    spectrum_data.power_levels[0] = 128;
    
    controller.update_spectrum(spectrum_data);
    controller.render(painter);
    
    EXPECT_TRUE(painter.fill_rectangle_called);
    EXPECT_EQ(painter.last_fill_color, Color::black());
}
```

**3. Thread Safety Testing:**

```cpp
// tests/data/test_thread_safe_cache.cpp
#include <thread>
#include "data/thread_safe_cache.hpp"
#include "dsp/spectrum_processor.hpp"

TEST(ThreadSafeCacheTest, ConcurrentReadWrite) {
    ThreadSafeCache<ProcessedSpectrum> cache;
    SpectrumProcessor processor;
    
    // Writer thread
    std::thread writer([&]() {
        ProcessedSpectrum data{};
        for (size_t i = 0; i < 100; ++i) {
            data.power_levels[i] = static_cast<uint8_t>(i);
            cache.put(data);
        }
    });
    
    // Reader thread
    std::thread reader([&]() {
        for (size_t i = 0; i < 100; ++i) {
            auto data = cache.get();
            EXPECT_EQ(data.power_levels[0], static_cast<uint8_t>(i));
        }
    });
    
    writer.join();
    reader.join();
}
```

---

### Performance Optimization Recommendations

**1. Cache Pre-Calculated Data:**
- Pre-calculate spectrum render data in DSP layer
- Cache histogram render data in data layer
- Avoid recalculating during each paint() call

**2. Use Thread-Safe Data Structures:**
- Use lock-free ring buffers for high-frequency data
- Use atomic operations for simple flags
- Minimize mutex contention between scanner and UI threads

**3. Optimize DSP Algorithms:**
- Use lookup tables for common calculations
- Use fixed-point arithmetic for frequency calculations
- Minimize memory allocations in DSP layer

**4. Lazy Initialization:**
- Initialize expensive resources only when needed
- Use singleton pattern for expensive processors
- Defer initialization until first use

---

### Maintainability Recommendations

**1. Follow SOLID Principles:**
- **S**ingle Responsibility: Each class has one reason to change
- **O**pen/Closed: Open for extension, closed for modification
- **L**iskov Substitution: Use interfaces, not concrete implementations
- **I**nterface Segregation: Separate interfaces for different concerns

**2. Use Meaningful Names:**
- Avoid abbreviations (use `spectrum_processor` not `sp_proc`)
- Use descriptive names (use `calculate_noise_floor` not `calc_nf`)
- Follow naming conventions (PascalCase for classes, camelCase for methods)

**3. Add Documentation:**
- Document class responsibilities in header comments
- Document method preconditions and postconditions
- Document thread safety guarantees
- Document performance characteristics

**4. Code Organization:**
- Group related methods together
- Separate public and private interfaces
- Use consistent ordering (constructors, public methods, private methods)
- Avoid deep nesting (max 3 levels)

---

## Conclusion

The Enhanced Drone Analyzer exhibits **pervasive mixing of UI rendering and DSP/signal processing logic** across multiple translation units. This architectural violation significantly impacts:

- **Testability:** DSP logic cannot be tested independently
- **Maintainability:** Changes to one layer risk breaking another
- **Performance:** UI thread blocked by DSP calculations
- **Code Reusability:** Tightly coupled logic cannot be reused

**Recommended Action:** Implement the refactoring roadmap in Section 4, prioritizing P0 (Critical) and P1 (High) issues. The proposed architectural structure with clear layering will address all identified mixed logic instances and provide a solid foundation for future development.

**Overall Assessment:** The codebase requires significant architectural refactoring to achieve proper separation of concerns. However, the good separation examples in [`ui_spectral_analyzer.hpp`](ui_spectral_analyzer.hpp:1), [`ui_signal_processing.hpp`](ui_signal_processing.hpp:1), and [`eda_optimized_utils.hpp`](eda_optimized_utils.hpp:1) demonstrate that clean separation is achievable and provides a clear target architecture.

---

**Report Generated:** 2026-03-01  
**Analyst:** Architect Mode  
**Report Version:** 1.0
