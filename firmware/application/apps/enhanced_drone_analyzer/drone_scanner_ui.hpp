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

    std::string title() const override { return "EDA"; }
    void paint(Painter& painter) override;
    void focus() override;
    void on_show() override;
    void on_hide() override;
    bool on_touch(const ui::TouchEvent event) override;

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
        {{UI_POS_X(0), UI_POS_Y(0)}, "LNA   VGA   AMP  ", Theme::getInstance()->fg_light->foreground},
    };

    ui::LNAGainField field_lna_{{UI_POS_X(4), 0}};
    ui::VGAGainField field_vga_{{UI_POS_X(11), 0}};
    ui::RFAmpField field_rf_amp_{{UI_POS_X(18), 0}};
    ui::AudioVolumeField field_volume_{{UI_POS_X_RIGHT(2), UI_POS_Y(0)}};
    ui::NumberField field_rssi_dec_cyc_{{UI_POS_X_RIGHT(6), UI_POS_Y(0)}, 2, {1, 50}, 1, ' '};
    ui::Labels labels_cyc_{
        {{UI_POS_X_RIGHT(9), UI_POS_Y(0)}, "cy", Theme::getInstance()->fg_light->foreground},
    };

    ui::Labels filter_labels_{
        {{UI_POS_X(0), 268}, "FILT:", Theme::getInstance()->fg_light->foreground},
    };

    ui::OptionsField field_filter_{
        {UI_POS_X(5), 268},
        4,
        {
            {"OFF ", SPECTRUM_FILTER_OFF},
            {"MID ", SPECTRUM_FILTER_MID},
            {"HIGH", SPECTRUM_FILTER_HIGH},
        }};

    ui::Button button_median_{{UI_POS_X(10), 268, UI_POS_WIDTH(3), 16}, "OFF"};

    ui::Button button_start_stop_{{UI_POS_X(0), 284, UI_POS_WIDTH(6), 28}, "Start"};
    ui::Button button_mode_{{UI_POS_X(7), 284, UI_POS_WIDTH(5), 28}, "Mode"};
    ui::Button button_load_{{UI_POS_X(13), 284, UI_POS_WIDTH(4), 28}, "Load"};
    ui::Button button_settings_{{UI_POS_X(18), 284, UI_POS_WIDTH(4), 28}, "Set"};
    ui::Button button_swp_{{UI_POS_X(23), 284, UI_POS_WIDTH(3), 28}, "SWP"};
    ui::Button button_ptr_{{UI_POS_X(27), 284, UI_POS_WIDTH(3), 28}, "PTR"};
    
    FreqHz current_frequency_{0};
    int32_t current_rssi_{RSSI_NOISE_FLOOR_DBM};
    ScannerState current_scanner_state_{ScannerState::IDLE};

    char displayed_drone_type_[MAX_DRONE_TYPE_DISPLAY + 1]{};
    uint32_t drone_type_display_timer_{0};

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

    DroneDisplay drone_display_{{0, 68, DISPLAY_WIDTH, 206}};

    void bigdisplay_update(BigDisplayColor color) noexcept;
    void refresh_ui() noexcept;
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;
    void on_retune(FreqHz freq, uint32_t range) noexcept;

    // Band sweep — Looking Glass pattern: stop → process → retune → start
    // COMPOSITE_SIZE, SWEEP_SLICE_BW, MAX_SWEEP_WINDOWS defined in constants.hpp
    static constexpr uint8_t DB_SCANS_PER_SWEEP = 50;
    static constexpr FreqHz EACH_BIN_SIZE = SWEEP_SLICE_BW / 256;

    /**
     * @brief Encapsulates all state for a single sweep window (Meyers: replace duplication with data)
     */
    struct SweepWindow {
        uint8_t composite[COMPOSITE_SIZE]{};  // pixel buffer
        FreqHz f_min{0};
        FreqHz f_max{0};
        FreqHz f_center{0};
        FreqHz f_center_ini{0};
        FreqHz pixel_step_hz{0};
        FreqHz step_hz{0};
        FreqHz bins_hz_acc{0};
        FreqHz exceptions[EXCEPTIONS_PER_WINDOW]{};  // exception frequencies (0 = unused)
        FreqHz exception_radius_hz{3000000ULL};       // configurable exclusion radius (Hz)
        uint16_t pixel_index{0};
        uint8_t pixel_max{0};
        bool enabled{false};

        void init(FreqHz start, FreqHz end, FreqHz step = 0) noexcept;
        void reset() noexcept;
        void process_bins(const ChannelSpectrum& spectrum) noexcept;

        /**
         * @brief Check if a frequency falls within ±EXCEPTION_RADIUS_HZ of any exception
         * @param hz Frequency to check
         * @return true if frequency should be suppressed
         */
        [[nodiscard]] bool is_exception(FreqHz hz) const noexcept;
    };

    SweepWindow sweep_[MAX_SWEEP_WINDOWS]{};

    bool composite_active_{false};
    bool sweep_auto_mode_{false};
    uint8_t active_sweep_idx_{0};         // 0-3, round-robin index
    uint8_t current_pair_{0};             // Current displayed pair index (0 or 2)
    uint8_t db_scan_count_{0};
    
    // Pattern capture state
    PatternCaptureState pattern_capture_state_{PatternCaptureState::IDLE};
    uint16_t pattern_select_start_{0};
    uint16_t pattern_select_end_{0};
    uint8_t pattern_capture_frames_{0};
    static constexpr uint8_t PATTERN_CAPTURE_FRAMES = 5;
    static constexpr uint16_t PATTERN_MATCH_INTERVAL = 10;  // Match every 10th frame
    
    // FIFO lifecycle management - prevents M0 use-after-free crashes
    struct SpectrumFIFOState {
        ChannelSpectrumFIFO* fifo{nullptr};
        AtomicFlag fifo_active_;        // M0 is using this FIFO
        AtomicFlag fifo_changing_;      // Prevents concurrent changes
        uint32_t last_access_time_ms{0};   // ChibiOS timestamp
        
        [[nodiscard]] bool is_safe_to_clear(SystemTime now, uint32_t timeout_ms = 100) const noexcept {
            if (fifo == nullptr) return true;
            if (fifo_active_.test()) return false;           // M0 still using
            if (fifo_changing_.test()) return false;        // Change in progress
            const uint32_t elapsed_ms = (now >= last_access_time_ms) 
                ? (now - last_access_time_ms) 
                : (UINT32_MAX - last_access_time_ms + now);
            return elapsed_ms >= timeout_ms;
        }
        
        void mark_access(SystemTime now) noexcept {
            last_access_time_ms = now;
        }
    };
    
    // Safe FIFO management methods - prevent M0 memory overflow crashes
    void safe_clear_fifo() noexcept;
    void safe_set_fifo(ChannelSpectrumFIFO* fifo) noexcept;
    [[nodiscard]] bool verify_baseband_stopped(SystemTime now, uint32_t timeout_ms = 200) const noexcept;
    
    uint8_t pattern_match_counter_{0};  // Counter for frame interval matching (init first)
    SpectrumFIFOState fifo_state_;
    AtomicFlag sweep_transition_guard_;   // Prevents concurrent enter/exit
    FreqHz last_db_frequency_{0};         // Last DB frequency before sweep
    size_t last_db_index_{0};             // Last DB index before sweep (for exact restore)

    void enter_sweep_mode() noexcept;
    void exit_sweep_mode() noexcept;
    void on_sweep_spectrum(const ChannelSpectrum& spectrum) noexcept;
    void retune_sweep_window(SweepWindow& win, const char* prefix = nullptr) noexcept;
    void update_sweep_pair_display() noexcept;
    [[nodiscard]] uint8_t pair_first(uint8_t idx) const noexcept;
    
    // Pattern capture methods
    void on_pattern_capture_toggle() noexcept;
    void on_touch_spectrum(uint16_t pixel_x) noexcept;
    void capture_pattern_frame(const ChannelSpectrum& spectrum) noexcept;
    void finalize_pattern_capture() noexcept;
    void match_patterns_in_sweep(const ChannelSpectrum& spectrum, FreqHz freq) noexcept;

    // Spectrum filter threshold (OFF/MID/HIGH)
    uint8_t min_color_power_{DEFAULT_SPECTRUM_FILTER};

    // Median filter toggle state
    bool median_enabled_{true};

    // Latest ChannelStatistics.max_db from baseband (full-bandwidth RSSI)
    int32_t latest_max_db_{RSSI_NOISE_FLOOR_DBM};

    // Reusable buffers for refresh_ui() (class members instead of static locals)
    DisplayData refresh_display_data_{};
    TrackedDrone refresh_drones_[MAX_DISPLAYED_DRONES]{};
    char refresh_status_buf_[MAX_TEXT_LENGTH]{};
    uint16_t refresh_hist_data_[HISTOGRAM_BUFFER_SIZE]{};
    
    // Pattern capture buffer (accumulates multiple frames)
    std::array<uint8_t, PATTERN_WAVEFORM_SIZE> pattern_waveform_sum_{};
    FreqHz pattern_capture_freq_{0};
    RssiValue pattern_capture_rssi_{0};

    // Spectrum shape sort buffer (for SpectrumShape::analyze in sweep mode)
    uint8_t spectrum_shape_sort_buf_[256];
    
    // Reusable buffer for pattern capture (prevents stack allocation)
    uint8_t pattern_capture_buffer_[PATTERN_WAVEFORM_SIZE];
    
    MessageHandlerRegistration message_handler_spectrum_config;
    MessageHandlerRegistration message_handler_frame_sync;
    MessageHandlerRegistration message_handler_retune;
    MessageHandlerRegistration message_handler_channel_stats;
};

} // namespace drone_analyzer

#endif // DRONE_SCANNER_UI_HPP
