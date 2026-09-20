#ifndef DRONE_SCANNER_UI_HPP
#define DRONE_SCANNER_UI_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_receiver.hpp"
#include "ui_rssi.hpp"
#include "message.hpp"
#include "portapack.hpp"
#include "radio_state.hpp"
#include "app_settings.hpp"

#include "drone_types.hpp"
#include "constants.hpp"
#include "scanner.hpp"
#include "scanner_thread.hpp"
#include "drone_display.hpp"
#include "sweep_processor.hpp"
#include "settings_manager.hpp"
#include "auto_gain_control.hpp"

namespace drone_analyzer {

enum class BigDisplayColor : int8_t {
    GREY = -2,
    YELLOW = -1,
    GREEN = 0,
    RED = 1
};

class DroneScanner;
class DatabaseManager;
class HardwareController;
struct ScanConfig;

class DroneScannerUI : public ui::View {
public:
    explicit DroneScannerUI(NavigationView& nav) noexcept;
    ~DroneScannerUI() noexcept override;

    DroneScannerUI(const DroneScannerUI&) = delete;
    DroneScannerUI& operator=(const DroneScannerUI&) = delete;

    std::string title() const override {
        static const std::string t = "EDA";  // SSO — no heap allocation for ≤15 chars
        return t;
    }
    void paint(Painter& painter) override;
    void focus() override;
    void on_show() override;
    void on_hide() override;

    void show_alert(const char* message, uint32_t duration_ms) noexcept;
    void show_error(ErrorCode error, uint32_t duration_ms) noexcept;

private:

    static constexpr uint16_t BIG_FREQUENCY_X = 4;
    static constexpr uint16_t BIG_FREQUENCY_Y = 1 * 16;
    static constexpr uint16_t BIG_FREQUENCY_WIDTH = 28 * 8;
    static constexpr uint16_t DRONE_TYPE_SPACING = 5;
    static constexpr uint16_t DRONE_TYPE_Y_OFFSET = 20;

    static constexpr uint32_t ERROR_DURATION_MS = 3000;

    void construct_objects() noexcept;
    void destruct_objects() noexcept;

    NavigationView& nav_;

    RxRadioState radio_state_{ReceiverModel::Mode::SpectrumAnalysis};

    ui::BigFrequency big_display_;

    HardwareController* hardware_ptr_{nullptr};
    DatabaseManager* database_ptr_{nullptr};
    DroneScanner* scanner_ptr_{nullptr};
    ScannerThread* scanner_thread_{nullptr};

    ui::Labels labels_{
        // 15 chars — exactly the SSO limit. The old "LNA   VGA   AMP  " (17)
        // forced a heap allocation inside std::string on EVERY app open.
        // Trailing pad spaces removed: they were invisible and pushed the
        // literal over the SSO boundary. Field columns are untouched.
        {{UI_POS_X(0), UI_POS_Y(0)}, "LNA   VGA   AMP", Color::white()},
    };

    ui::LNAGainField field_lna_{{UI_POS_X(4), 0}};
    ui::VGAGainField field_vga_{{UI_POS_X(11), 0}};
    ui::RFAmpField field_rf_amp_{{UI_POS_X(18), 0}};
    ui::AudioVolumeField field_volume_{{UI_POS_X_RIGHT(2), UI_POS_Y(0)}};
    // "cy" = rssi_decrease_cycles (1..50): seconds without RSSI growth before a
    // tracked drone's threat decays (normal mode; sweep mode uses the hardcoded
    // MAX_SWEEP_CYCLES_MISSED instead — see scanner.hpp:apply_rssi_decay).
    // on_change writes through to the scanner config (get→mutate→set); initial
    // value is pushed from the config at the end of the constructor.
    ui::NumberField field_rssi_dec_cyc_{{UI_POS_X_RIGHT(6), UI_POS_Y(0)}, 2, {1, 50}, 1, ' '};
    ui::Labels labels_cyc_{
        {{UI_POS_X_RIGHT(9), UI_POS_Y(0)}, "cy", Color::white()},
    };

    ui::Button button_start_stop_{{UI_POS_X(0), 284, UI_POS_WIDTH(6), 28}, "Start"};
    ui::Button button_mode_{{UI_POS_X(7), 284, UI_POS_WIDTH(5), 28}, "Mode"};
    ui::Button button_load_{{UI_POS_X(13), 284, UI_POS_WIDTH(4), 28}, "Load"};
    ui::Button button_settings_{{UI_POS_X(18), 284, UI_POS_WIDTH(4), 28}, "Set"};
    ui::Button button_swp_{{UI_POS_X(23), 284, UI_POS_WIDTH(5), 28}, "SWP"};

    FreqHz current_frequency_{0};
    void set_current_frequency_safe(FreqHz freq) noexcept;
    [[nodiscard]] FreqHz get_current_frequency_safe() const noexcept;
    int32_t current_rssi_{RSSI_NOISE_FLOOR_DBM};
    ScannerState current_scanner_state_{ScannerState::IDLE};

    char displayed_drone_type_[MAX_DRONE_TYPE_DISPLAY + 1]{};
    uint32_t drone_type_display_timer_{0};

    // Cache for bigdisplay_update() — skip big_display_.set() when frequency unchanged.
    FreqHz bigdisplay_last_freq_{0};
    BigDisplayColor bigdisplay_last_color_{BigDisplayColor::GREY};

    bool scanning_{false};
    ScanningMode scanning_mode_{DEFAULT_SCANNING_MODE};

    bool alert_active_{false};
    char alert_message_[MAX_TEXT_LENGTH]{};
    uint32_t alert_start_time_{0};
    uint32_t alert_duration_ms_{0};

    bool error_active_{false};
    ErrorCode last_error_{ErrorCode::SUCCESS};
    uint32_t error_start_time_{0};
    uint32_t error_duration_ms_{0};

    ChannelSpectrumFIFO* spectrum_fifo_{nullptr};
    bool initialization_failed_{false};
    bool db_loaded_{false};
    size_t db_entry_count_{0};
    FreqHz last_tuned_freq_{0};  // exact freq radio was tuned to when FFT was captured
    bool skip_next_fft_{false};  // skip first FFT after sweep entry (may be stale)

    // Reusable buffer to prevent stack overflow in message handler
    // ChannelSpectrum is 256 bytes - moved from local stack to BSS
    ChannelSpectrum spectrum_buffer_{};
    // C1 drain scratch: sweep-mode drain-to-newest pops up to 3 stale frames
    // per tick into this BSS buffer (NOT the stack — ~272 bytes). Never read
    // except for the empty-FIFO fallback copy in the frame-sync handler.
    ChannelSpectrum spectrum_discard_{};

    DroneDisplay drone_display_{{0, 68, DISPLAY_WIDTH, 206}};

    void bigdisplay_update(BigDisplayColor color) noexcept;
    void refresh_ui() noexcept;
    void apply_agc(const uint8_t* spectrum_data) noexcept;
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;
    void on_retune(FreqHz freq, uint32_t range) noexcept;

    // Band sweep — Looking Glass pattern: stop → process → retune → start
    // COMPOSITE_SIZE, SWEEP_SLICE_BW, MAX_SWEEP_WINDOWS defined in constants.hpp
    // DB frames before the auto-cycle switches to sweep mode. The DB FFT frame
    // is ~16.5 ms (DB_FFT_TRIGGER @ DB_CAPTURE_RATE_HZ), so 200 frames ≈ 3.3 s
    // of DB airtime per cycle — the same rhythm the legacy 2 MHz capture gave
    // (50 × 65.5 ms). Sweep runs one full cycle, then auto-returns to DB scan.
    static constexpr uint8_t DB_SCANS_PER_SWEEP = 200;
    // EACH_BIN_SIZE removed — use SWEEP_BIN_SIZE from constants.hpp instead

    /**
     * @brief Encapsulates all state for a single sweep window (Meyers: replace duplication with data)
     * @note SRAM cost: ~300B per window (240 bytes composite + ~50 bytes metadata)
     * @note Total for MAX_SWEEP_WINDOWS (4) windows: ~1,200B BSS
     * @note Within 128KB SRAM budget but significant for resource-constrained system
     */
    struct SweepWindow {
        uint8_t composite[COMPOSITE_SIZE]{};  // pixel buffer
        FreqHz f_min{0};
        FreqHz f_max{0};
        FreqHz f_center{0};
        FreqHz f_center_ini{0};
        FreqHz pixel_step_hz{0};
        FreqHz step_hz{0};
        // Cached Hz-per-usable-bin for the progress accumulator
        // (step_hz / FFT_SWEEP_USABLE_BINS, computed once in init(); the
        // per-frame UDIV in process_bins() is the P0-3 hot-path saving).
        FreqHz effective_bin_size{0};
        FreqHz bins_hz_acc{0};
        uint16_t pixel_index{0};
        uint8_t pixel_max{0};
        uint8_t settle_frames_remaining_{0};  // frames to skip after retune
        bool enabled{false};

        /**
         * @brief Derive the gapless sweep pitch and reset the window.
         * @note Contract: step_hz == 0 after init() ⇔ the range is degenerate
         *       (unsweepable after hardware clamps — f_min at the ceiling).
         *       Callers MUST gate enabled via init_sweep_window(), and
         *       on_sweep_spectrum() short-circuits such windows to the normal
         *       pass-completion path (no retune loop, no detection).
         */
        void init(FreqHz start, FreqHz end, FreqHz step = 0) noexcept;
        void reset() noexcept;
        /**
         * @brief Integrate one FFT frame into this window's composite buffer.
         * @param spectrum        256-bin FFT frame from baseband
         * @param credit_progress true  — full path: progress accumulator and
         *                        settle counter are consumed (freshest frame
         *                        per tick, via on_sweep_spectrum);
         *                        false — display-only integration for the
         *                        extra drained live frames (frame-sync
         *                        handler): composite max-hold is reinforced
         *                        at true RF positions, while pixel_index /
         *                        bins_hz_acc pacing stays one step per retune
         *                        (line_full/freq_covered logic unchanged).
         * @return true if the frame was processed, false if it was discarded
         *         as a settle (retune-straddler) frame.
         * @note Stack: ~16 bytes (dummy accumulators in the display-only path).
         */
        [[nodiscard]] bool process_bins(const ChannelSpectrum& spectrum, bool credit_progress = true) noexcept;
        // NOTE: rendering is never masked — the sweep window is always drawn
        // fully. Detection-window gating is applied in the scanner only
        // (DroneScanner::is_detection_window_allowed()).
    };

    SweepWindow sweep_[MAX_SWEEP_WINDOWS]{};

    bool composite_active_{false};
    bool sweep_auto_mode_{false};
    uint8_t active_sweep_idx_{0};         // 0-3, round-robin index
    uint8_t first_enabled_sweep_idx_{0};  // First enabled window (cycle boundary for decay)
    uint8_t current_pair_{0};             // Current displayed pair index (0 or 2)
    uint8_t db_scan_count_{0};
    FreqHz last_db_frame_freq_{0};        // Capture freq of last processed DB frame (retune-straddle guard)
    AtomicFlag sweep_transition_guard_;   // Prevents concurrent enter/exit
    uint32_t sweep_guard_timestamp_{0};   // Tick when guard was last set (timeout safety)
    AtomicFlag button_debounce_guard_;     // Debounces button_mode_/start_stop rapid taps
    FreqHz last_db_frequency_{0};         // Last DB frequency before sweep
    size_t last_db_index_{0};             // Last DB index before sweep (for exact restore)

    // Cached display state for dirty-checking in update_sweep_pair_display().
    // Avoids calling set_dual_sweep_mode(), set_sweep_range(), set_sweep2_range()
    // every frame when values haven't changed — each call forced DIRTY_ALL/DIRTY_SPEC
    // and triggered full composite+waterfall+drone list repaints at 60 Hz.
    FreqHz last_sweep_range_start_{0};
    FreqHz last_sweep_range_end_{0};
    FreqHz last_sweep2_range_start_{0};
    FreqHz last_sweep2_range_end_{0};
    bool last_dual_sweep_mode_{false};

    void enter_sweep_mode() noexcept;
    void exit_sweep_mode(bool suppress_auto_restart = false) noexcept;
    void on_sweep_spectrum(const ChannelSpectrum& spectrum) noexcept;
    void retune_sweep_window(SweepWindow& win, const char* prefix = nullptr) noexcept;
    void update_sweep_pair_display() noexcept;
    [[nodiscard]] uint8_t pair_first(uint8_t idx) const noexcept;

    /**
     * @brief Initialize one sweep window from config and gate its enabled flag.
     * @param idx          Window index (0..MAX_SWEEP_WINDOWS-1, guarded)
     * @param start        Configured range start (Hz)
     * @param end          Configured range end (Hz)
     * @param step         User pitch (ignored — gapless auto-derivation)
     * @param want_enabled Desired enabled state from the workspace config
     * @note DEGENERATE-WINDOW GUARD: SweepWindow::init() leaves step_hz == 0
     *       for unsweepable ranges (f_min clamped to the hardware ceiling).
     *       Enabling such a window would hang the pass (process_frame() is a
     *       no-op, the pass never completes) — it is kept disabled instead so
     *       the pair/round-robin logic skips it. Used by enter_sweep_mode()
     *       and on_show() — the only two init() call sites.
     */
    void init_sweep_window(uint8_t idx, FreqHz start, FreqHz end, FreqHz step, bool want_enabled) noexcept;

    // Latest ChannelStatistics.max_db from baseband (full-bandwidth RSSI)
    int32_t latest_max_db_{RSSI_NOISE_FLOOR_DBM};

    // Reusable buffers for refresh_ui() (class members instead of static locals)
    DisplayData refresh_display_data_{};
    TrackedDrone refresh_drones_[MAX_DISPLAYED_DRONES]{};  // BSS, not stack (saves 2,304B stack)
    char refresh_status_buf_[MAX_TEXT_LENGTH]{};

    /**
     * @brief RF frontend automatic gain controller.
     * @note Analyzes each spectrum frame for saturation/dead bins and adjusts
     *       LNA/VGA/RF amplifiers via hardware_controller. Rate-limited to
     *       prevent gain oscillation. Default OFF — opt-in via ScanConfig.
     */
    AutoGainControl auto_gain_control_{};

    /**
     * @brief Storage layout for message handlers.
     * @note No constructor — members are placement-new'd manually.
     */
    struct HandlerStorage {
        MessageHandlerRegistration spectrum_config;
        MessageHandlerRegistration frame_sync;
        MessageHandlerRegistration retune;
        MessageHandlerRegistration channel_stats;
    };
    // handler_storage_ is explicitly sized to sizeof(HandlerStorage) — guaranteed safe.
    // Prevents DBLREG hard fault via placement new at runtime.
    static_assert(sizeof(HandlerStorage) <= 256, "HandlerStorage unexpectedly large (>256 bytes)");
    alignas(alignof(HandlerStorage)) uint8_t handler_storage_[sizeof(HandlerStorage)];
    bool handlers_active_{false};

    void register_handlers() noexcept;
    void unregister_handlers() noexcept;

    bool on_touch(const ui::TouchEvent event) override;
    bool baseband_needs_restore_{false};
    bool scanning_needs_restore_{false};
};

[[nodiscard]] DroneScanner& get_scanner_instance() noexcept;
[[nodiscard]] DroneScanner* get_scanner_ptr() noexcept;

// ============================================================================
// Shared workspace buffers (UI-thread only, no concurrency)
// Replaces 5 separate file-level statics (~1,824 B) with 2 shared ones (~728 B)
// Used by DroneSweepView, DroneSettingsView, and DroneScannerUI
// ============================================================================
extern ScanConfig g_workspace_cfg;
extern SettingsStruct g_workspace_settings;

// ============================================================================
// g_workspace_cfg ownership invariant:
//   - UI thread ONLY (message handlers, widget callbacks, keypad lambdas).
//   - The scanner thread NEVER reads g_workspace_cfg: DroneScanner uses its
//     internal config_ guarded by DATA_MUTEX (get_config/set_config).
//   - No locking is therefore needed. Any FUTURE cross-thread access must
//     protect ALL readers AND writers with one consistent lock scheme.
// ============================================================================
} // namespace drone_analyzer

#endif // DRONE_SCANNER_UI_HPP
