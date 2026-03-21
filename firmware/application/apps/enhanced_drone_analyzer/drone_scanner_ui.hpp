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
        static const std::string title_str = "EDA";
        return title_str;
    }
    void paint(Painter& painter) override;
    void focus() override;
    void on_show() override;
    void on_hide() override;

    void show_alert(const char* message, uint32_t duration_ms) noexcept;
    void show_error(ErrorCode error, uint32_t duration_ms) noexcept;

private:
    static constexpr uint16_t BIG_FREQUENCY_X = 4;
    static constexpr uint16_t BIG_FREQUENCY_Y = 1 * 16;  // Moved higher (1 row from top)
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
    void update_big_frequency_only() noexcept;
    void refresh_ui() noexcept;
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;
    void on_retune(FreqHz freq, uint32_t range) noexcept;

    // Spectrum display cycle counter (update visual every N scan cycles)
    uint8_t spectrum_cycle_counter_{0};
    static constexpr uint8_t SPECTRUM_UPDATE_INTERVAL = 3;

    // Frame counter for periodic full refresh (update threat levels without new spectrum)
    uint8_t display_frame_count_{0};
    static constexpr uint8_t DISPLAY_REFRESH_INTERVAL = 60;  // ~1 second at 60 FPS

    // Spectrum filter threshold (OFF/MID/HIGH)
    uint8_t min_color_power_{DEFAULT_SPECTRUM_FILTER};

    MessageHandlerRegistration message_handler_spectrum_config{
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }
    };

    MessageHandlerRegistration message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            bool has_new_spectrum = false;
            if (this->spectrum_fifo_ != nullptr) {
                ChannelSpectrum spectrum;
                if (this->spectrum_fifo_->out(spectrum)) {
                    this->on_channel_spectrum(spectrum);
                    has_new_spectrum = true;
                }
            }
            this->display_frame_count_++;
            if (has_new_spectrum || this->display_frame_count_ >= DISPLAY_REFRESH_INTERVAL) {
                this->display_frame_count_ = 0;
                this->refresh_ui();
            } else {
                this->update_big_frequency_only();
            }
        }
    };

    MessageHandlerRegistration message_handler_retune{
        Message::ID::Retune,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const RetuneMessage*>(p);
            this->on_retune(message.freq, message.range);
            this->spectrum_cycle_counter_++;
        }
    };
};

} // namespace drone_analyzer

#endif // DRONE_SCANNER_UI_HPP
