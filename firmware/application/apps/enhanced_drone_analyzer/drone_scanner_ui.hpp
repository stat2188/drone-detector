#ifndef DRONE_SCANNER_UI_HPP
#define DRONE_SCANNER_UI_HPP

#include <cstdint>
#include <cstddef>
#include <string>

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
        return std::string("EDA");
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
    app_settings::SettingsManager settings_{
        "rx_eda"sv, app_settings::Mode::RX};

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

    ui::Labels filter_labels_{
        {{UI_POS_X(0), 274}, "FILT:", Theme::getInstance()->fg_light->foreground},
    };

    ui::OptionsField field_filter_{
        {UI_POS_X(5), 274},
        4,
        {
            {"OFF ", SPECTRUM_FILTER_OFF},
            {"MID ", SPECTRUM_FILTER_MID},
            {"HIGH", SPECTRUM_FILTER_HIGH},
        }};

    ui::Button button_start_stop_{{UI_POS_X(0), 290, UI_POS_WIDTH(6), 28}, "Start"};
    ui::Button button_mode_{{UI_POS_X(7), 290, UI_POS_WIDTH(5), 28}, "Mode"};
    ui::Button button_load_{{UI_POS_X(13), 290, UI_POS_WIDTH(5), 28}, "Load"};
    ui::Button button_settings_{{UI_POS_X(19), 290, UI_POS_WIDTH(5), 28}, "Setup"};

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

    DroneDisplay drone_display_{{0, 68, DISPLAY_WIDTH, 206}};

    void bigdisplay_update(BigDisplayColor color) noexcept;
    void refresh_ui() noexcept;
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;
    void on_retune(FreqHz freq, uint32_t range) noexcept;

    // Band sweep state (Looking Glass pattern: UI-thread driven, independent of scanner)
    static constexpr uint16_t COMPOSITE_SIZE = DISPLAY_WIDTH;  // 240 pixels
    uint8_t composite_buffer_[COMPOSITE_SIZE]{};
    bool composite_active_{false};
    bool sweep_was_scanning_{false};       // restore scanner state on exit

    FreqHz sweep_start_{5700000000};
    FreqHz sweep_end_{5900000000};
    FreqHz sweep_step_hz_{2000000};
    FreqHz sweep_f_center_{0};             // current tuned frequency in sweep
    FreqHz sweep_f_center_ini_{0};         // first slice center
    uint16_t sweep_pixel_index_{0};        // current pixel position (0..239)
    static constexpr FreqHz SWEEP_SLICE_BW = 2000000;  // 2 MHz baseband per slice

    void enter_sweep_mode() noexcept;
    void exit_sweep_mode() noexcept;
    void sweep_process_frame(const ChannelSpectrum& spectrum) noexcept;

    // Spectrum filter threshold (OFF/MID/HIGH)
    uint8_t min_color_power_{DEFAULT_SPECTRUM_FILTER};

    MessageHandlerRegistration message_handler_spectrum_config;
    MessageHandlerRegistration message_handler_frame_sync;
    MessageHandlerRegistration message_handler_retune;
};

} // namespace drone_analyzer

#endif // DRONE_SCANNER_UI_HPP
