#ifndef DRONE_SCANNER_UI_HPP
#define DRONE_SCANNER_UI_HPP

#include <cstdint>
#include <cstddef>

#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_receiver.hpp"
#include "message.hpp"
#include "portapack.hpp"

#include "drone_types.hpp"
#include "constants.hpp"

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

    void paint(Painter& painter) override;
    void focus() override;
    void on_show() override;
    void on_hide() override;

    void show_alert(const char* message, uint32_t duration_ms) noexcept;
    void show_error(ErrorCode error, uint32_t duration_ms) noexcept;

 private:
    static constexpr uint16_t BIG_FREQUENCY_X = 4;
    static constexpr uint16_t BIG_FREQUENCY_Y = 6 * 16;
    static constexpr uint16_t BIG_FREQUENCY_WIDTH = 28 * 8;
    static constexpr uint16_t DRONE_TYPE_SPACING = 5;
    static constexpr uint16_t DRONE_TYPE_Y_OFFSET = 20;

    static constexpr uint32_t ERROR_DURATION_MS = 3000;

    void construct_objects() noexcept;
    void destruct_objects() noexcept;

    NavigationView& nav_;

    ui::BigFrequency big_display_;

    HardwareController* hardware_ptr_{nullptr};
    DatabaseManager* database_ptr_{nullptr};
    DroneScanner* scanner_ptr_{nullptr};

    ui::LNAGainField field_lna_{{4 * 8, 0}};
    ui::VGAGainField field_vga_{{11 * 8, 0}};
    ui::RFAmpField field_rf_amp_{{18 * 8, 0}};

    ui::Button button_start_stop_{{0, 16 * 16, 8 * 8, 28}, "Start"};
    ui::Button button_mode_{{9 * 8, 16 * 16, 7 * 8, 28}, "Mode"};
    ui::Button button_settings_{{17 * 8, 16 * 16, 7 * 8, 28}, "Setup"};

    FreqHz current_frequency_{0};
    int32_t current_rssi_{RSSI_NOISE_FLOOR_DBM};
    ScannerState current_scanner_state_{ScannerState::IDLE};

    char displayed_drone_type_[MAX_DRONE_TYPE_DISPLAY + 1]{};
    uint32_t drone_type_display_timer_{0};

    TrackedDrone temp_drone_{};

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

    void bigdisplay_update(BigDisplayColor color) noexcept;
    void update_ui_state() noexcept;
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;

    MessageHandlerRegistration message_handler_spectrum_config{
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }};

    MessageHandlerRegistration message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            if (this->spectrum_fifo_ != nullptr) {
                ChannelSpectrum spectrum;
                if (this->spectrum_fifo_->out(spectrum)) {
                    this->on_channel_spectrum(spectrum);
                }
            }
            this->update_ui_state();
        }};
};

} // namespace drone_analyzer

#endif // DRONE_SCANNER_UI_HPP
