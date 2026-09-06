================================================================================
  ENHANCED DRONE ANALYZER (EDA) — COMPREHENSENSIVE DOCUMENTATION
  Firmware Application for HackRF One / PortaPack Mayhem
  Author: Kuznetsov Maxim (Orenburg)
  Version: Current codebase (49 source files)
================================================================================

TABLE OF CONTENTS
-----------------
  1. Overview
  2. Architecture & Layer Diagram
  3. File Inventory (49 files)
  4. Hardware Constraints & Memory Budget
  5. Core Data Types
  6. Detection Pipeline
  7. Sweep Mode (Looking Glass Pattern)
  8. Normal Mode (DB Sequential Scan)
  9. UI Framework
  10. Settings & Persistence
  11. Thread Model & Synchronization
  12. Audio Alert System
  13. Pattern Matching (RF Fingerprinting)
  14. Detection Filter Chain
  15. CFAR Detection (All 7 Modes)
  16. Mahalanobis Gate Filter
  17. Auto Gain Control (AGC)
  18. Memory Map & Stack Budgets
  19. Error Handling Strategy
  20. SD Card File Layout
  21. Build & Test Instructions
  22. Known Limitations
  23. Configuration Reference

================================================================================
1. OVERVIEW
================================================================================

The Enhanced Drone Analyzer (EDA) is a comprehensive RF drone detection application
for the HackRF One hardware platform running PortaPack Mayhem firmware. It detects
and classifies radio-frequency signals from consumer and FPV drones by analyzing
spectrum shape, signal strength, movement trends, and statistical properties.

Key Capabilities:
  - Wideband spectrum analysis across 1 MHz - 7.2 GHz
  - 4-window sweep mode covering up to 300 MHz per window
  - Real-time drone tracking with threat classification (NONE/LOW/MEDIUM/HIGH/CRITICAL)
  - CFAR (Constant False Alarm Rate) detection with 7 algorithm variants
  - Mahalanobis gate filter for statistical outlier rejection
  - Spectrum shape analysis (V-shape, sharpness, valley depth, flatness, symmetry)
  - Automatic RF frontend gain control
  - Audio alerts with SOS patterns for critical threats
  - Pattern matching via SAD-based RF fingerprinting
  - RSSI history with movement trend detection (approaching/receding/static)
  - Persistent settings on SD card (/EDA/SETTINGS.TXT)
  - Database scanning from freqman-format files (/FREQMAN/*.TXT)

Target Platform:
  - MCU: STM32F405RG (ARM Cortex-M4F, 168 MHz)
  - RAM: 128 KB SRAM (usable ~64 KB contiguous for application)
  - Flash: 1 MB
  - RTOS: ChibiOS (cooperative + preemptive threads)
  - Display: 240x320 pixels, 16-bit RGB565
  - RF: HackRF One (Si5351, MAX2837, RFFC5072)

================================================================================
2. ARCHITECTURE & LAYER DIAGRAM
================================================================================

  ┌─────────────────────────────────────────────────────────────┐
  │                    UI LAYER (M4 Core)                       │
  │  DroneScannerUI  │  DroneDisplay  │  DroneSettingsView     │
  │  DroneSweepView  │  PatternManagerView                     │
  │  SpectrumPreviewWidget │ MiniWaterfall                      │
  ├─────────────────────────────────────────────────────────────┤
  │                   LOGIC LAYER (M4 Core)                     │
  │  DroneScanner (core logic) │ ScannerThread                  │
  │  CFARDetector │ PeakDetector │ PatternMatcher               │
  │  MahalanobisDetector │ RSSIDetector                         │
  │  AdaptiveThreshold │ SpectralKurtosis                       │
  │  MedianFilter<T,N> │ SweepProcessor                        │
  │  AutoGainControl │ NeighborMarginChecker                    │
  ├─────────────────────────────────────────────────────────────┤
  │                   DATA LAYER (M4 Core)                      │
  │  DatabaseManager │ PatternManager │ SettingsFileManager     │
  │  TrackedDrone[16] │ DisplayDroneEntry[16]                   │
  │  ScanConfig │ SettingsStruct │ WaterfallHistory              │
  ├─────────────────────────────────────────────────────────────┤
  │                   HAL LAYER                                 │
  │  HardwareController │ portapack::spi │ baseband::api        │
  │  radio::set_tuning_frequency │ audio::*                     │
  ├─────────────────────────────────────────────────────────────┤
  │                   HARDWARE                                  │
  │  Si5351 (PLL) │ MAX2837 (LNA/VGA) │ RFFC5072 (Mixer)     │
  │  CPLD (ADC/DAC) │ WM8731/AK4951 (Audio Codec)             │
  └─────────────────────────────────────────────────────────────┘

Thread Architecture:
  ┌──────────────────────────────────────┐
  │  UI Thread (Main, Priority 5)        │
  │  - All widget callbacks              │
  │  - Message handlers                  │
  │  - Sweep frequency stepping          │
  │  - Display rendering                 │
  │  Stack: 4096 bytes                   │
  ├──────────────────────────────────────┤
  │  Scanner Thread (Priority 10)        │
  │  - perform_scan_cycle()              │
  │  - DB frequency hopping              │
  │  - Dwell timer management            │
  │  Stack: 2048 bytes                   │
  ├──────────────────────────────────────┤
  │  Baseband (M0 Core, separate binary) │
  │  - FFT computation                   │
  │  - Spectrum streaming                │
  │  - Channel statistics                │
  └──────────────────────────────────────┘

Lock Ordering (deadlock prevention):
  PATTERN_MUTEX(0) → DATA_MUTEX(1) → DATABASE_MUTEX(2) → STATE_MUTEX(3)
  MUST be acquired in ascending order only.

================================================================================
3. FILE INVENTORY (49 files)
================================================================================

  File                          Lines   Purpose
  ───────────────────────────── ─────── ──────────────────────────────────────
  constants.hpp                 ~1500   All compile-time constants, thresholds
  drone_types.hpp               ~820    Core types, enums, TrackedDrone struct
  drone_types.cpp               ~615    TrackedDrone methods, DisplayDroneEntry
  drone_scanner_ui.hpp          ~287    Main EDA view (entry point)
  drone_scanner_ui.cpp          ~1250+  Main view implementation, sweep logic
  drone_display.hpp             ~706    Display widget (spectrum, drone list)
  drone_display.cpp             ~1000+  Rendering implementation
  drone_settings.hpp            ~133    Settings view declaration
  drone_settings.cpp            ~711    Settings view with all callbacks
  drone_sweep_view.hpp          ~161    Sweep settings view
  drone_sweep_view.cpp          ~378    Sweep settings implementation
  scanner.hpp                   ~1400+  DroneScanner class, CFAR detector
  scanner.cpp                   ~1000+  Scanner logic, spectrum processing
  scanner_thread.hpp            ~52     ScannerThread declaration
  scanner_thread.cpp            ~50+    Thread implementation
  database.hpp                  ~84     DatabaseManager declaration
  database.cpp                  ~200+   Database file loading
  hardware_controller.hpp       ~304    HardwareController declaration
  hardware_controller.cpp       ~200+   Hardware abstraction implementation
  settings_manager.hpp          ~179    SettingsStruct + SettingsFileManager
  settings_manager.cpp          ~400+   SD card load/save/apply
  locking.hpp                   ~119    RAII mutex wrappers, AtomicFlag
  audio_alerts.hpp              ~131    Audio alert system declaration
  audio_alerts.cpp              ~200+   Beep generation, SOS patterns
  rssi_detector.hpp             ~120    RSSIDetector declaration
  rssi_detector.cpp             ~200+   RSSI processing
  pattern_matcher.hpp           ~103    SAD pattern matching declaration
  pattern_matcher.cpp           ~200+   Pattern matching implementation
  pattern_types.hpp             ~99     SignalPattern, PatternFeatures structs
  pattern_types.cpp             ~50+    Pattern type implementations
  pattern_manager.hpp           ~108    PatternManager declaration
  pattern_manager.cpp           ~400+   SD card pattern CRUD
  pattern_manager_view.hpp      ~143    Pattern capture UI declaration
  pattern_manager_view.cpp      ~400+   Pattern capture UI implementation
  mahalanobis_gate.hpp          ~150    Mahalanobis gate declaration
  mahalanobis_gate.cpp          ~200+   Q8.8 fixed-point statistics
  median_filter.hpp             ~146    Stack-based median filter (template)
  peak_detector.hpp             ~90     PeakDetector declaration
  peak_detector.cpp             ~200+   Quickselect percentile + peak scan
  adaptive_threshold.hpp        ~124    Self-tuning CFAR threshold
  auto_gain_control.hpp         ~170    AGC for HackRF RF frontend
  spectral_kurtosis.hpp         ~202    Higher-order statistics computation
  sweep_processor.hpp           ~89     FFT bin → composite pixel mapping
  sweep_processor.cpp           ~200+   Process frame, reorder frame
  mini_waterfall.hpp            ~166    Compact scrolling waterfall
  waterfall_history.hpp         ~174    Multi-frame FFT ring buffer
  spectrum_preview_widget.hpp   ~44     Settings preview widget
  spectrum_preview_widget.cpp   ~100+   Preview rendering
  freqman_types.hpp             ~23     freqman_type enum

Total: ~49 files, ~8,500+ lines of C++ code.

================================================================================
4. HARDWARE CONSTRAINTS & MEMORY BUDGET
================================================================================

Memory Budget (from constants.hpp):
  ─────────────────────────────────────────────
  Component                    Bytes     Location
  ─────────────────────────────────────────────
  Database entries[100]        1,152     BSS (12B each)
  Tracked drones[16]           2,560     BSS (~160B each)
  Display drones[16]           1,024     BSS (~64B each)
  Spectrum buffer              256       BSS (ChannelSpectrum)
  Signal timeline              62        BSS
  RSSI detector                ~108      BSS
  Scanner thread stack         2,048     BSS (THD_WA_SIZE)
  Mini waterfalls[5]           2,250     BSS (450B each)
  Sweep windows[4]             ~1,200    BSS (300B each)
  LG frame buffer              240       BSS
  Workspace buffers            ~728      BSS (cfg + settings)
  Other structures             ~200      BSS
  ─────────────────────────────────────────────
  TOTAL STATIC RAM             ~8,550    bytes
  STACK BUDGET                 4,096     bytes (UI thread)
  SCANNER THREAD STACK         2,048     bytes
  TOTAL MEMORY BUDGET          ~12,646   bytes
  ─────────────────────────────────────────────

Forbidden (will cause HardFault or OOM):
  ✗ std::vector, std::string, std::map, std::unordered_map
  ✗ std::atomic (no SMP, single-core M4F)
  ✗ new, delete, malloc, free, calloc, realloc
  ✗ std::thread, std::mutex, std::future
  ✗ Dynamic memory allocation of ANY kind
  ✗ Stack frames > 512 bytes per function
  ✗ Recursion deeper than 2 levels
  ✗ VLA (Variable Length Arrays)
  ✗ Exceptions, RTTI, dynamic_cast

Permitted:
  ✓ std::array<T, N> — compile-time fixed size
  ✓ std::string_view — non-owning reference
  ✓ C-style arrays with constexpr size
  ✓ Memory Pools (pre-allocated blocks)
  ✓ Stack allocation for primitives and small structs (<64 bytes)
  ✓ static const/constexpr for flash placement
  ✓ Placement new ONLY for memory pool initialization

================================================================================
5. CORE DATA TYPES
================================================================================

Type Aliases (drone_types.hpp):
  FreqHz        = uint64_t    // Frequency in Hz
  RssiValue     = int32_t     // Signal strength in dBm
  SystemTime    = uint32_t    // ChibiOS tick count (ms)

Enums:
  DroneType     : DJI, Parrot, Yuneec, 3DR, Autel, Hobby, FPV, Custom, Other
  ThreatLevel   : NONE, LOW, MEDIUM, HIGH, CRITICAL
  ScanningMode  : SEQUENTIAL
  ScannerState  : IDLE, SCANNING, LOCKING, TRACKING, PAUSED, ERROR
  MovementTrend : UNKNOWN, STATIC, APPROACHING, RECEIVING
  ErrorCode     : SUCCESS + 25 error codes (HW, DB, buffer, sync, init)
  CFARMode      : OFF, CA, GO, SO, HYBRID, OS, VI

Key Structures:

  TrackedDrone (~160 bytes, 16 instances = 2,560B BSS):
    FreqHz frequency                      // 8B - tracking identity
    FreqHz measured_frequency_            // 8B - bin-corrected display freq
    DroneType drone_type                  // 1B
    ThreatLevel threat_level              // 1B
    uint8_t update_count                  // 1B
    SystemTime last_seen                  // 4B
    RssiValue rssi                        // 4B - current RSSI
    int16_t rssi_history_[6]              // 12B - circular buffer
    SystemTime timestamp_history_[6]      // 24B
    uint16_t history_index_               // 2B - monotonic write cursor
    uint8_t missed_cycles_                // 1B
    int16_t last_rssi_                    // 2B
    uint8_t rssi_decrease_counter_        // 1B
    bool rssi_increased_                  // 1B
    SystemTime last_increase_time_        // 4B
    SystemTime created_time_              // 4B
    SystemTime last_seen_time_            // 4B
    uint8_t sweep_cycles_missed_          // 1B
    bool sweep_mode_active_               // 1B
    int16_t last_cycle_peak_rssi_         // 2B - sentinel if invalid
    int16_t prev_cycle_peak_rssi_         // 2B
    bool has_prev_cycle_peak_             // 1B
    mutable MovementTrend cached_trend_   // 1B - hysteresis state
    mutable uint8_t trend_hold_count_     // 1B
    MahalanobisStatistics mahalanobis_    // 48B - Q8.8 fixed-point

  DisplayDroneEntry (~48 bytes, 16 instances = 768B BSS):
    FreqHz frequency                      // 8B
    DroneType type                        // 1B
    ThreatLevel threat                    // 1B
    RssiValue rssi                        // 4B
    SystemTime last_seen                  // 4B
    char type_name[16]                    // 16B
    uint32_t display_color                // 4B (RGBA)
    MovementTrend trend                   // 1B

  ScanConfig (~368 bytes):
    ScanningMode mode
    FreqHz start_frequency, end_frequency
    uint32_t scan_interval_ms
    int32_t rssi_threshold_dbm
    int32_t threat_low/medium/high/critical_dbm
    uint32_t stale_timeout_ms
    FreqHz sweep_start/end/step_freq (×4 windows)
    bool sweep2/3/4_enabled
    FreqHz sweep_exceptions[4][5]
    bool dwell_enabled, confirm_count_enabled, noise_blacklist_enabled
    bool spectrum_detection_enabled, median_enabled
    uint8_t spectrum_margin/min_width/max_width/sharpness/ratio/valley/flatness/symmetry
    bool mahalanobis_enabled
    uint8_t mahalanobis_threshold_x10
    int32_t neighbor_margin_db
    bool rssi_variance_enabled
    uint8_t confirm_count, miss_tolerance
    CFARMode cfar_mode
    uint8_t cfar_ref_cells/guard_cells/threshold_x10
    uint8_t cfar_hybrid_alpha/beta/gamma, os_cfar_k_percent, vi_cfar_threshold_x10
    bool shape_bypass_enabled, sensitive_mode
    bool kurtosis_enabled, adaptive_cfar_enabled
    uint8_t exception_radius_mhz, rssi_decrease_cycles, freq_match_radius_mhz

  SettingsStruct (~360 bytes):
    Mirrors ScanConfig fields + display-only settings (spectrum_visible, etc.)
    + SD card persistence metadata

================================================================================
6. DETECTION PIPELINE
================================================================================

The detection pipeline processes each FFT frame through multiple stages:

  Spectrum Frame (256 bins, from baseband M0)
       │
       ▼
  ┌─────────────────────────┐
  │ Stage 1: DC Spike Blank │ Skip bins 120-135 (FFT center)
  └─────────────────────────┘
       │
       ▼
  ┌─────────────────────────┐
  │ Stage 2: Edge Skip      │ Skip bins 0-9 and 246-255 (filter rolloff)
  └─────────────────────────┘
       │
       ▼
  ┌─────────────────────────┐
  │ Stage 3: CFAR Detection │ Adaptive threshold detection (7 modes)
  │   OFF/CA/GO/SO/HYBRID/OS/VI │
  └─────────────────────────┘
       │
       ▼
  ┌─────────────────────────┐
  │ Stage 4: Peak Detection │ Find strongest peak, compute margin/width
  └─────────────────────────┘
       │
       ▼
  ┌─────────────────────────┐
  │ Stage 5: Shape Filters  │ 12-step filter chain:
  │   1. Peak margin (SNR)  │
  │   2. Min width          │
  │   3. Max width          │
  │   4. Peak sharpness     │
  │   5. Peak-to-width ratio│
  │   6. Valley depth       │
  │   7. Flatness           │
  │   8. Symmetry           │
  │   9. Neighbor margin    │
  │  10. RSSI variance      │
  │  11. Mahalanobis gate   │
  │  12. Spectral kurtosis  │
  └─────────────────────────┘
       │
       ▼
  ┌─────────────────────────┐
  │ Stage 6: Drone Tracking │ Match to existing or create new TrackedDrone
  │   - Frequency proximity │ (DRONE_FREQ_MATCH_RADIUS_HZ = 1 MHz)
  │   - Duplicate consolidation (absorb_from)
  │   - Threat classification from RSSI thresholds
  │   - Movement trend calculation (cycle-peak or split-buffer)
  └─────────────────────────┘
       │
       ▼
  ┌─────────────────────────┐
  │ Stage 7: UI Update      │ DisplayDroneEntry copy + sort + render
  └─────────────────────────┘

================================================================================
7. SWEEP MODE (Looking Glass Pattern)
================================================================================

Sweep mode scans wide frequency ranges (up to 4 windows × 300 MHz) by stepping
the radio through 20 MHz slices, similar to the Looking Glass app.

Sweep Architecture:
  ┌──────────────────────────────────────────────────────┐
  │  SweepWindow[0..3] — each window has:                │
  │    composite[240]     — pixel buffer (one pass)       │
  │    f_min, f_max       — frequency range               │
  │    f_center           — current tune frequency        │
  │    pixel_index        — current pixel position        │
  │    pixel_step_hz      — Hz per pixel                  │
  │    step_hz            — Hz per FFT step               │
  │    exceptions[5]      — exclusion frequencies         │
  │    exception_radius_hz— exclusion radius              │
  └──────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────┐
  │  Looking Glass Bin Reordering:                        │
  │    FFT bins 134-253 → screen pixels 0-119 (lower SB)  │
  │    FFT bins 2-119   → screen pixels 120-238 (upper SB)│
  │    Bins 119-136     → DC spike (skipped, no Hz)       │
  └──────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────┐
  │  Sweep Cycle:                                         │
  │    1. Tune to f_center                                │
  │    2. Wait for PLL settle (5ms)                       │
  │    3. Discard stale frames (STALE_FIFO_FRAMES=3)     │
  │    4. Process FFT → composite[240] via SweepProcessor │
  │    5. Feed to scanner for drone detection             │
  │    6. Push to per-window MiniWaterfall                │
  │    7. Advance f_center by step_hz                     │
  │    8. When pixel_index >= 240: pass complete          │
  │    9. Round-robin to next enabled window              │
  │   10. On full cycle: finalize_sweep_cycle()           │
  └──────────────────────────────────────────────────────┘

Key Constants:
  SWEEP_SLICE_BW          = 20 MHz (bandwidth per slice)
  FFT_BIN_COUNT           = 256 bins
  SWEEP_BIN_SIZE          = 78,125 Hz/bin (20 MHz / 256)
  SWEEP_PIXELS_PER_SLICE  = 240 (= DISPLAY_WIDTH)
  SWEEP_BINS_PER_STEP     = 228 (= FFT_USABLE_BINS_NARROW)
  SWEEP_GAPLESS_STEP_MAX  = ~8.8 MHz (max step for zero blind frequencies)
  SWEEP_FFT_TRIGGER       = 63 (phase decimation, ~6.5ms integration)
  SWEEP_PERSISTENCE_DECAY = 224/256 = 0.875 (EMA decay factor)
  SWEEP_SETTLE_FRAMES     = 1 (discard after retune)
  MAX_SWEEP_WINDOWS       = 4

Composite Persistence (EMA):
  new_val = max(raw_val, (persist_val * DECAY) >> 8)
  Signal decays to ~59% after 4 passes (~3s at 1.6s/pass)

Noise Floor Subtraction:
  Auto-computed from 15th percentile of EMA persistence buffer
  Display only bins above noise_floor + SWEEP_DISPLAY_NOISE_MARGIN (8)

================================================================================
8. NORMAL MODE (DB Sequential Scan)
================================================================================

Normal mode scans a database of known drone frequencies.

  ┌──────────────────────────────────────────────────────┐
  │  Database (freqman format):                           │
  │    /FREQMAN/DRONES.TXT (default)                      │
  │    Format: f=frequency,d=description                  │
  │    Max entries: 100 (1,152 bytes BSS)                 │
  │    Entry: frequency_a (int64) + description[32]       │
  └──────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────┐
  │  Scan Cycle:                                          │
  │    1. Scanner thread calls perform_scan_cycle()       │
  │    2. Get next frequency from database                │
  │    3. Tune radio via baseband API                     │
  │    4. Wait for scan_interval_ms (default 50ms)        │
  │    5. Repeat from step 2                              │
  │                                                       │
  │  Detection (UI thread):                               │
  │    1. Receive ChannelSpectrum via FIFO                │
  │    2. Call process_spectrum_message(spectrum, freq)   │
  │    3. Run detection pipeline (Section 6)              │
  │    4. Update tracked drones                           │
  │    5. After DB_SCANS_PER_SWEEP=50 cycles:             │
  │       auto-enter sweep mode                           │
  └──────────────────────────────────────────────────────┘

Dwell Behavior:
  - When signal detected: dwell_enabled=true holds frequency
  - max_dwell_cycles derived from confirm_count + miss_tolerance
  - After max dwell: force_resume_scanning() breaks out
  - Noise blacklist: 3+ consecutive noise events → skip frequency

================================================================================
9. UI FRAMEWORK
================================================================================

The EDA follows the standard Mayhem UI pattern:

  DroneScannerUI (main view, inherits ui::View)
    ├── BigFrequency big_display_        — large 7-segment frequency
    ├── DroneDisplay drone_display_      — spectrum + drone list + waterfall
    ├── Labels labels_                   — LNA/VGA/AMP labels
    ├── LNAGainField field_lna_          — LNA gain control
    ├── VGAGainField field_vga_          — VGA gain control
    ├── RFAmpField field_rf_amp_         — RF amplifier toggle
    ├── AudioVolumeField field_volume_   — headphone volume
    ├── NumberField field_rssi_dec_cyc_  — RSSI decrease cycles
    ├── Button button_start_stop_        — Start/Stop scanning
    ├── Button button_mode_              — Toggle sweep mode
    ├── Button button_load_              — Load database file
    ├── Button button_settings_          — Open settings view
    └── Button button_swp_               — Open sweep settings

  DroneDisplay (custom view, inherits ui::View)
    ├── Spectrum/Composite rendering     — 240px wide
    ├── Per-window sweep waterfalls      — MiniWaterfall[4]
    ├── Drone list                       — sorted by threat level
    ├── Signal timeline                  — MiniWaterfall (realtime)
    └── Status bar                       — state + DB count

  DroneSettingsView (inherits ui::View)
    ├── ~40 UI widgets (NumberField, Checkbox, OptionsField)
    ├── SpectrumPreviewWidget            — live filter preview
    ├── Threat threshold ladder          — normalize_threat_ladder()
    └── Save/Defaults/About buttons

  DroneSweepView (inherits ui::View)
    ├── SweepWindowView (reusable, 1 instance)
    │   ├── Start/End NumberFields
    │   ├── Enabled Checkbox
    │   └── 5 Exception NumberFields
    ├── OptionsField window_select_      — switch window 1-4
    └── Save/Defaults buttons

  PatternManagerView (inherits ui::View)
    ├── Spectrum display                 — 240px with bin selection
    ├── Pattern list (OptionsField)
    ├── Capture/Save/Delete/Toggle buttons
    └── Frequency input (NumberField)

Widget Update Pattern:
  void on_tick() {  // Called by DisplayFrameSync message
      const auto result = logic_layer_.get_detection();
      if (result.is_valid) {
          drone_list_.add_entry(result);
          set_dirty();  // Mark for repaint
      }
      // NEVER call paint() directly!
  }

Message Handler Registration:
  MessageHandlerRegistration message_handler_{
      Message::ID::ChannelSpectrum,
      [this](const Message* const p) {
          this->on_spectrum(static_cast<const ChannelSpectrumMessage*>(p));
      }
  };

================================================================================
10. SETTINGS & PERSISTENCE
================================================================================

Settings File: /EDA/SETTINGS.TXT
Settings Version: 1

SettingsStruct fields (all persisted):
  // Scanning
  scanning_mode, scan_interval_ms, scan_sensitivity
  alert_rssi_threshold_dbm
  threat_low_dbm, threat_medium_dbm, threat_high_dbm, threat_critical_dbm

  // Display
  spectrum_visible, timeline_visible

  // Alerts
  audio_alerts_enabled, volume

  // Detection
  dwell_enabled, confirm_count_enabled, noise_blacklist_enabled
  spectrum_detection_enabled, median_enabled
  spectrum_margin, spectrum_min_width, spectrum_max_width
  spectrum_peak_sharpness, spectrum_peak_ratio
  spectrum_valley_depth, spectrum_flatness, spectrum_symmetry

  // CFAR
  cfar_mode, cfar_ref_cells, cfar_guard_cells, cfar_threshold_x10
  cfar_hybrid_alpha/beta/gamma, os_cfar_k_percent, vi_cfar_threshold_x10

  // Anti-false-positive
  neighbor_margin_db, rssi_variance_enabled
  confirm_count, miss_tolerance

  // Mahalanobis
  mahalanobis_enabled, mahalanobis_threshold_x10

  // Advanced
  sensitive_mode, shape_bypass_enabled
  kurtosis_enabled, kurtosis_min_x10, adaptive_cfar_enabled

  // Sweep Window 1
  sweep_start_freq, sweep_end_freq, sweep_step_freq

  // Sweep Window 2-4
  sweep2/3/4_start/end/step_freq, sweep2/3/4_enabled

  // Sweep Exceptions
  sweep_exceptions[4][5], exception_radius_mhz

  // Misc
  rssi_decrease_cycles, freq_match_radius_mhz

SettingsManager Flow:
  1. Load: SettingsFileManager::load(settings) → read /EDA/SETTINGS.TXT
  2. Apply: SettingsFileManager::apply_to_config(settings, config) → ScanConfig
  3. Extract: SettingsFileManager::extract_from_config(config, settings) → reverse
  4. Save: SettingsFileManager::save(scanner_ptr, settings) → write SD card

Threat Ladder Normalization:
  Enforces: low <= medium <= high <= critical
  AND: medium > detection_threshold + RSSI_MIN_MEDIUM_ABOVE_DETECTION_DB (3 dB)
  Ensures LOW band is always reachable.

================================================================================
11. THREAD MODEL & SYNCHRONIZATION
================================================================================

ChibiOS RTOS Threading:

  Thread          Priority  Stack    Purpose
  ─────────────── ───────── ──────── ────────────────────────────────
  UI (Main)       5         4096 B   Widget callbacks, message handlers
  Scanner         10        2048 B   DB frequency hopping, dwell timer
  Baseband (M0)   -         -        FFT, spectrum streaming

Synchronization Primitives:

  AtomicFlag (locking.hpp):
    - Lock-free using __atomic_test_and_set
    - Used for: sweep_transition_guard_, button_debounce_guard_,
                pll_locked_, streaming_active_, loaded_

  MutexLock<ORDER> (RAII wrapper):
    - ChibiOS mutex with automatic unlock
    - Template parameter enforces lock ordering at compile time
    - Used for: DATA_MUTEX, DATABASE_MUTEX, STATE_MUTEX, PATTERN_MUTEX

  MutexTryLock<ORDER> (non-blocking):
    - Returns is_locked() flag
    - Used where blocking would cause deadlock

Message Flow (baseband → UI):
  1. M0 baseband produces ChannelSpectrum via FIFO
  2. DisplayFrameSync message triggers UI thread processing
  3. UI thread reads spectrum_buffer_ (class member, not stack)
  4. Calls process_spectrum_message() or on_sweep_spectrum()
  5. Detection pipeline runs (see Section 6)
  6. Tracked drones updated under DATA_MUTEX
  7. refresh_ui() copies drone list to DisplayData at 10 Hz

Shared Workspace Buffers:
  ScanConfig g_workspace_cfg        — UI-thread only, no locking needed
  SettingsStruct g_workspace_settings — UI-thread only, no locking needed
  Invariant: scanner thread NEVER reads g_workspace_cfg

================================================================================
12. AUDIO ALERT SYSTEM
================================================================================

AudioAlertManager (static class):
  - Enabled/disabled via settings
  - Plays tones through WM8731/AK4951 audio codec
  - Sample rate: 24 kHz

Alert Types:
  NEW_DRONE        — 1000 Hz, 150ms
  THREAT_INCREASED — 1000 Hz, 100ms
  THREAT_CRITICAL  — 1500 Hz, 80ms (SOS loop)
  DRONE_APPROACHING — 1200 Hz, 200ms
  DRONE_RECEDING   — 800 Hz, 150ms

Threat-Level Audio:
  NONE     — silence
  LOW      — single beep (800 Hz, 150ms)
  MEDIUM   — triple beep pattern (1000 Hz, 80ms × 3)
  HIGH     — SOS loop (1200 Hz)
  CRITICAL — SOS loop (1500 Hz, faster)

SOS Pattern:
  • • • — — — • • • (short-long-short-long-short)
  SHORT_BEEP_MS = 80
  LONG_BEEP_MS = 200
  BEEP_GAP_MS = 80
  LETTER_GAP_MS = 400
  CYCLE_GAP_MS = 1000

update() called at ~60 Hz from refresh_ui():
  - Drives multi-beep patterns
  - Handles continuous (SOS) vs one-shot (MEDIUM) modes
  - Stops looping when no HIGH/CRITICAL threats remain

================================================================================
13. PATTERN MATCHING (RF FINGERPRINTING)
================================================================================

PatternMatcher: SAD-based (Sum of Absolute Differences)
  - Normalizes 256-bin FFT to 16-bin waveform
  - Compares against saved patterns via SAD score (0-1000)
  - Frequency-proximity pre-filter (center_freq + range_width)

Pattern Storage:
  Directory: /EDA/PATTERNS/
  Format: <name>.TXT (one CSV line per file)
  CSV: name,wave[16],features[4],threshold,flags,center_freq,range_width
  Max patterns: MAX_PATTERNS (10)

PatternFeatures (4 bytes):
  peak_position  — 16-bin space (0..15)
  peak_value     — captured peak amplitude (0-255)
  noise_floor    — captured noise floor (0-255)
  margin         — peak_value - noise_floor

Match Threshold:
  Auto-tuned at save time from captured SNR margin
  Higher margin → higher threshold (stricter matching)

Capture Flow:
  1. User captures live spectrum via PatternManagerView
  2. PeakDetector::find() identifies peak + noise floor
  3. PatternMatcher::normalize() → 16-bin waveform
  4. match_threshold = f(margin) at save time
  5. Saved to /EDA/PATTERNS/<name>.TXT

Matching Flow:
  1. normalize(fft_256) → wave_16
  2. For each enabled pattern:
     a. Frequency proximity check (if center_freq set)
     b. compute_similarity(wave_16, pattern_wave) → score
     c. If score >= pattern.match_threshold → match
  3. Return best match (highest score)

================================================================================
14. DETECTION FILTER CHAIN (12 Steps)
================================================================================

The spectrum shape filter chain is the primary false-positive rejection mechanism.
Each step can independently reject a detection.

  Step 1: Peak Margin (SNR)
    peak_margin = peak_value - noise_floor
    Must exceed spectrum_margin (default 20 ≈ 4 dB)
    → Rejects sub-CFAR noise spikes

  Step 2: Minimum Width
    signal_width must exceed spectrum_min_width (default 9 bins ≈ 700 kHz)
    → Rejects single-bin noise spikes

  Step 3: Maximum Width
    signal_width must be below spectrum_max_width (default 40 bins ≈ 3.1 MHz)
    → Rejects flat U/I noise (WiFi, BT)

  Step 4: Peak Sharpness
    sharpness = (peak_margin * 100) / avg_margin
    Must exceed spectrum_peak_sharpness (default 150)
    → Rejects flat-top signals, accepts dual-peak FPV

  Step 5: Peak-to-Width Ratio
    ratio = (peak_margin * 10) / signal_width
    Must exceed spectrum_peak_ratio (default 0 = disabled)
    → Optional: rejects wide, low signals

  Step 6: Valley Depth
    max_valley_margin = max margin of bins flanking the signal
    Must be below spectrum_valley_depth (default 80)
    → Rejects flat-top WiFi/BT (deep valleys = drone V-shape)

  Step 7: Flatness
    flatness_pct = (high_power_bins * 100) / signal_width
    Must be below spectrum_flatness (default 0% = disabled)
    → Rejects WiFi flat-top (>50%), accepts drone V-shape (<20%)
    → Only applied when peak_margin >= 40 AND signal_width > 4

  Step 8: Symmetry
    symmetry = min(left_width, right_width) * 100 / max(left_width, right_width)
    Must exceed spectrum_symmetry (default 0 = disabled)
    → Optional: rejects asymmetric noise

  Step 9: Neighbor Margin
    center bin must exceed strongest neighbor within 10 MHz by neighbor_margin_db
    Default 2 dB
    → Rejects wideband noise (WiFi, BT, microwave)

  Step 10: RSSI Variance
    calculate_rssi_variance() must be < 100
    → Rejects chaotic noise (real drones have stable RSSI)

  Step 11: Mahalanobis Gate
    D²_M must be < mahalanobis_threshold_x10 / 10
    → Rejects statistical outliers (noise, spurs)

  Step 12: Spectral Kurtosis
    kurtosis_x10 must exceed kurtosis_min_x10 (default 20 = 2.0)
    → Rejects Gaussian noise (kurtosis ≈ 0), accepts drone peaks (> 3)
    → Default OFF (opt-in)

Very-Strong Signal Bypass:
  When peak_margin > 80 (~16 dB): skip valley/symmetry/kurtosis
  When peak_margin > 96 (~19 dB): additionally skip max_width
  → Handles close-range wideband FPV signals

Sensitive Mode:
  When ON: reduces spectrum_margin by 2, skips sharpness/valley/symmetry/kurtosis
  → For weak/long-range signals

================================================================================
15. CFAR DETECTION (All 7 Modes)
================================================================================

CFARDetector (scanner.hpp):

  CA-CFAR (Cell Averaging):
    noise_estimate = average of left + right reference windows
    Best for: homogeneous noise environments

  GO-CFAR (Greatest Of):
    noise_estimate = max(left_avg, right_avg)
    Best for: noise edges (sweep slice boundaries)

  SO-CFAR (Smallest Of):
    noise_estimate = min(left_avg, right_avg)
    Best for: cluttered environments

  HYBRID CFAR:
    noise_estimate = (α × CA + β × GO + γ × SO) / 100
    Default: α=50, β=30, γ=20

  OS-CFAR (Ordered Statistic):
    Sort reference cells, pick k-th order statistic
    k = (N_ref × os_k_percent) / 100 (default 75%)
    Uses histogram-based selection (O(256) instead of O(n²) sort)
    Best for: multi-target environments (FPV swarm)

  VI-CFAR (Variability Index):
    VI = variance / mean²
    VI < threshold → homogeneous → use CA-CFAR
    VI > threshold → clutter edge → use GO/SO-CFAR
    Default threshold: 2.0 (×10 = 20)

  OFF:
    CFAR disabled, use fixed threshold

Parameters:
  cfar_ref_cells     — reference window size (4-64, default 32)
  cfar_guard_cells   — guard cells (0-8, default 3)
  cfar_threshold_x10 — threshold offset ×10 (10-100, default 60)

Multi-Target Detection (find_peaks):
  - Collects all CFAR-passing bins
  - Sorts by power descending
  - Non-maximum suppression within CFAR_MIN_PEAK_SEPARATION (10 bins)
  - Returns up to CFAR_MAX_CONCURRENT_PEAKS (8) peaks

================================================================================
16. MAHALANOBIS GATE FILTER
================================================================================

MahalanobisDetector (mahalanobis_gate.hpp):

  Purpose: Statistical outlier detection using Mahalanobis distance.
  Formula: D²_M = Σ((x_i - μ_i)² / σ_i²)
  Assumes diagonal covariance (uncorrelated features).

  Feature Vector (2D):
    [0] = RSSI normalized (Q8.8, range 0-255)
    [1] = Frequency stability (Q8.8, 0-256 = 0-100%)

  Fixed-Point Arithmetic:
    Q8.8 format: 8 integer bits, 8 fractional bits
    Q_SCALE = 256
    q_multiply_safe(): int64_t intermediate, clamped to INT32_MAX/MIN

  Validation:
    If sample_count < MAHALANOBIS_HISTORY_SIZE/2 (4): pass through
    If threshold_x10 == 0: disabled (pass through)
    Otherwise: D²_M < threshold_x10/10 → valid signal

  Statistics Update (Welford's algorithm):
    Running mean and variance per drone
    Variance decay: every 64 samples, decay by 31/32 (3.125%)
    → Prevents overly aggressive gate tightening during long scans

  History:
    MAHALANOBIS_HISTORY_SIZE = 8 samples
    Circular buffer in MahalanobisStatistics struct

================================================================================
17. AUTO GAIN CONTROL (AGC)
================================================================================

AutoGainControl (auto_gain_control.hpp):

  Purpose: Prevent ADC saturation while maintaining max sensitivity.
  Rate: max 1 gain change per 500ms (prevents oscillation)

  Algorithm:
    1. Count saturated bins (power >= 250) — ADC overflow
    2. Count dead bins (power <= 5) — insufficient gain
    3. If >5% saturated: reduce gain
    4. If <1% dead AND max_power < 180: increase gain

  Gain Stepping:
    Reduce: VGA first (−4 dB) → LNA (−8 dB) → RF amp (−14 dB)
    Increase: RF amp (+14 dB) → VGA (+4 dB) → LNA (+8 dB)

  Hardware Constraints:
    LNA (MAX2837): 0-40 dB, step 8 dB
    VGA (MAX2837): 0-62 dB, step 2 dB
    RF AMP (HMC627A): 0 or 14 dB (binary)

  Usage:
    Enabled via adaptive_cfar_enabled in ScanConfig
    Called from apply_agc() in DroneScannerUI::on_channel_spectrum()

================================================================================
18. MEMORY MAP & STACK BUDGETS
================================================================================

BSS Allocation:
  Static RAM Budget: 8,550 bytes
  Stack Budget: 4,096 bytes (UI thread)
  Scanner Thread: 2,048 bytes
  Total: ~12,646 bytes

Key BSS Structures:
  s_hardware          — HardwareController
  s_database          — DatabaseManager (entries[100])
  s_scanner           — DroneScanner (tracked_drones_[16])
  s_scanner_thread    — ScannerThread (wa_[512])
  g_workspace_cfg     — ScanConfig (368 bytes)
  g_workspace_settings — SettingsStruct (360 bytes)
  spectrum_buffer_    — ChannelSpectrum (256 bytes)
  refresh_drones_[16] — TrackedDrone array (2,560 bytes)
  sweep_[4]           — SweepWindow array (~1,200 bytes)
  lg_frame_buf_[240]  — Looking Glass reorder buffer
  auto_gain_control_  — AutoGainControl (~16 bytes)

Stack-Heavy Functions (monitored):
  CFARDetector::find_peaks()   — ~32 bytes (candidates[16])
  OS-CFAR detect()             — ~256 bytes (histogram)
  SpectralKurtosis::compute()  — ~32 bytes (accumulators)
  absorb_from()                — ~128 bytes (merged[12])
  refresh_ui()                 — uses BSS for refresh_drones_[]
  DroneSettingsView ctor       — ~480B (all widgets in class)
  PatternManagerView::capture_and_save() — ~288B (PeakDetector sort_buf)

================================================================================
19. ERROR HANDLING STRATEGY
================================================================================

Error Codes (drone_types.hpp):
  SUCCESS = 0
  Hardware: HARDWARE_NOT_INITIALIZED, HARDWARE_TIMEOUT, HARDWARE_FAILURE,
            SPI_FAILURE, PLL_LOCK_FAILURE
  Database: DATABASE_NOT_LOADED, DATABASE_LOAD_TIMEOUT, DATABASE_CORRUPTED,
            DATABASE_EMPTY, DATABASE_FORMAT_INVALID
  Buffer: BUFFER_EMPTY, BUFFER_FULL, BUFFER_INVALID
  Sync: MUTEX_TIMEOUT, MUTEX_LOCK_FAILED, SEMAPHORE_TIMEOUT
  Init: INITIALIZATION_FAILED, INITIALIZATION_INCOMPLETE
  General: INVALID_PARAMETER, NOT_IMPLEMENTED, FILE_SYSTEM_ERROR

Pattern:
  - All public methods return ErrorCode or ErrorResult<T>
  - [[nodiscard]] on all return values
  - No exceptions (not supported on this platform)
  - Fallback behavior on errors (never crash)
  - UI shows error via show_error() for 3 seconds

ErrorResult<T> (optional-like):
  has_value() / is_valid() / error() / value() / value_or()
  Static factories: success(value) / failure(error)
  Specialization for void (no value, just error code)

================================================================================
20. SD CARD FILE LAYOUT
================================================================================

  /FREQMAN/
    DRONES.TXT           — Default drone frequency database
    *.TXT                — User-uploaded freqman files

  /EDA/
    SETTINGS.TXT         — EDA settings (all fields)
    PATTERNS/
      *.TXT              — RF fingerprint patterns (CSV)
      DJI_Mavic.TXT      — Example pattern
      FPV_Analog.TXT     — Example pattern

File Format — SETTINGS.TXT:
  Version: 1
  Key=Value pairs (one per line)
  Keys match SettingsStruct field names

File Format — PATTERNS/*.TXT:
  CSV: name,wave[16],features[4],threshold,flags,center_freq,range_width
  25 fields (new format), 29 fields (old format, still supported)

File Format — DRONES.TXT:
  freqman format: f=frequency,d=description
  OR CSV format: frequency,description

================================================================================
21. BUILD & TEST INSTRUCTIONS
================================================================================

Build:
  # From repository root
  mkdir -p build && cd build
  cmake -G Ninja ..
  ninja

  # Or target-specific
  cmake --build . --target firmware

Test:
  # Build test executable
  cmake --build . --target enhanced_drone_analyzer_test

  # Run tests
  cd build
  ctest --output-on-failure

  # Run specific test
  ./firmware/test/enhanced_drone_analyzer/enhanced_drone_analyzer_test

Test Coverage (3 test files):
  test_database_parser.cpp (842 lines):
    - safe_strlen, safe_strcpy, skip_whitespace
    - parse_uint64, freqman/CSV format parsing
    - Auto-detection, comment skipping
    - Edge cases: null pointers, small buffers, overflow

  test_settings_persistence.cpp (982 lines):
    - strnlen_wrapper, safe_strcpy
    - safe_str_to_uint64/int64/bool
    - dispatch_by_type for all setting types
    - serialize_setting for all types

  test_stack_usage.cpp (730 lines):
    - Validates struct sizes (TrackedDrone, SettingsStruct)
    - Validates buffer sizes (parser buffers, signal processing)
    - Validates UI component sizes
    - Validates total RAM < 128KB

Code Quality:
  ./format-code.sh          — clang-format-18
  ./run_cppcheck.sh         — static analysis
  clang-tidy file.cpp -checks=-*,thread-*,bugprone-*

================================================================================
22. KNOWN LIMITATIONS
================================================================================

  1. Single scanning mode (SEQUENTIAL only)
     - Wideband/hybrid/panoramic modes removed for simplicity
     - Sweep mode covers the wideband use case

  2. Pattern matching limited to 16-bin SAD
     - No frequency-dependent features in comparison
     - Frequency proximity filter is coarse (±range_width)

  3. Database limited to 100 entries
     - Fixed array, no dynamic expansion
     - Large databases need manual trimming

  4. No FHSS detection
     - Removed in current version
     - Sweep mode partially addresses hopping signals

  5. Audio alerts block during DAC write
     - WM8731/AK4951 DAC write is blocking
     - Brief audio glitches possible during heavy UI updates

  6. Sweep mode disables scanner thread
     - DB scanning stops during sweep
     - Resume from exact DB position after sweep

  7. Settings UI limited to ~40 widgets
     - Screen real estate constrains visible options
     - Some settings only accessible via SD card editing

  8. No real-time FFT overlay in normal mode
     - Only sweep mode shows composite spectrum
     - Normal mode shows drone list + big frequency

================================================================================
23. CONFIGURATION REFERENCE
================================================================================

RSSI Thresholds (dBm):
  RSSI_DETECTION_THRESHOLD_DBM     = -95   (minimum detection)
  DEFAULT_THREAT_LOW_DBM           = -95   (weak but active)
  DEFAULT_THREAT_MEDIUM_DBM        = -89   (moderate signal)
  RSSI_HIGH_THREAT_THRESHOLD_DBM   = -85   (strong signal)
  RSSI_CRITICAL_THREAT_THRESHOLD_DBM = -82  (very strong, close range)
  RSSI_NOISE_FLOOR_DBM             = -100  (noise floor)
  RSSI_MIN_MEDIUM_ABOVE_DETECTION_DB = 3   (LOW band guarantee)

Timing (ms):
  SCAN_CYCLE_INTERVAL_MS     = 50    (DB scan interval)
  DRONE_STALE_TIMEOUT_MS     = 5000  (5 seconds)
  DRONE_REMOVAL_TIMEOUT_MS   = 30000 (30 seconds)
  PLL_LOCK_TIMEOUT_MS        = 100
  HARDWARE_RETRY_DELAY_MS    = 10
  SD_CARD_TIMEOUT_MS         = 1000
  DATABASE_LOAD_TIMEOUT_MS   = 2000
  TARGET_UI_FPS              = 60
  UI_REFRESH_INTERVAL_MS     = 16

FFT Bin Layout:
  FFT_BIN_COUNT              = 256
  FFT_DC_SPIKE_START         = 120
  FFT_DC_SPIKE_END           = 136
  FFT_EDGE_SKIP              = 10
  FFT_EDGE_SKIP_NARROW       = 6
  FFT_USABLE_BINS            = 220
  FFT_USABLE_BINS_NARROW     = 228
  SWEEP_BIN_SIZE             = 78,125 Hz

Spectrum Shape Defaults:
  DEFAULT_SPECTRUM_MARGIN         = 20    (≈4 dB, analog FPV)
  DEFAULT_SPECTRUM_MIN_WIDTH      = 9     (≈700 kHz, analog FPV)
  DEFAULT_SPECTRUM_MAX_WIDTH      = 40    (≈3.1 MHz, analog FPV)
  DEFAULT_SPECTRUM_PEAK_SHARPNESS = 150
  DEFAULT_SPECTRUM_PEAK_RATIO     = 0     (disabled)
  DEFAULT_SPECTRUM_VALLEY_DEPTH   = 80
  DEFAULT_SPECTRUM_FLATNESS       = 0     (disabled, analog FPV)
  DEFAULT_SPECTRUM_SYMMETRY       = 0     (disabled)

CFAR Defaults:
  DEFAULT_CFAR_MODE           = CFARMode::OS
  DEFAULT_CFAR_REF_CELLS      = 32
  DEFAULT_CFAR_GUARD_CELLS    = 3
  DEFAULT_CFAR_THRESHOLD_X10  = 60 (6.0 ≈ 1.2 dB)

Mahalanobis Defaults:
  DEFAULT_MAHALOBIS_THRESHOLD_X10 = 40 (4.0)
  MAHALANOBIS_DIMENSIONS          = 2
  MAHALANOBIS_HISTORY_SIZE        = 8
  MAHALANOBIS_Q_FORMAT            = 8 (Q8.8)

Movement Trend:
  MOVEMENT_TREND_THRESHOLD_APPROACHING_DB = 3
  MOVEMENT_TREND_THRESHOLD_RECEEDING_DB   = -3
  SWEEP_TREND_THRESHOLD_DB                = 2
  MOVEMENT_TREND_MIN_HISTORY              = 3
  TREND_HYSTERESIS_COUNT                  = 3

Thread Priorities:
  SCANNER_THREAD_PRIORITY  = 10
  UI_THREAD_PRIORITY       = 5

Stack Sizes:
  SCANNER_THREAD_STACK_SIZE = 2048
  UI_THREAD_STACK_SIZE      = 4096

Colors (RGBA):
  COLOR_LOW_THREAT      = 0xFF00FF00 (green)
  COLOR_MEDIUM_THREAT   = 0xFFFFFF00 (yellow)
  COLOR_HIGH_THREAT     = 0xFFFF8000 (orange)
  COLOR_CRITICAL_THREAT = 0xFFFF0000 (red)
  COLOR_UNKNOWN_THREAT  = 0xFF808080 (gray)
  COLOR_BACKGROUND      = 0xFF000000 (black)
  COLOR_TEXT            = 0xFFFFFFFF (white)

================================================================================
  END OF DOCUMENTATION
  Generated: 2026-09-06
  Codebase: 49 files, ~8,500+ lines C++
  Platform: HackRF One / PortaPack Mayhem / STM32F405RG
================================================================================
