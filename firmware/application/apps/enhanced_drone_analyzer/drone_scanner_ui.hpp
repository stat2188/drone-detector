#ifndef DRONE_SCANNER_UI_HPP
#define DRONE_SCANNER_UI_HPP

#include <cstdint>
#include <cstddef>
#include <functional>

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

    [[nodiscard]] ErrorCode initialize() noexcept;

    void paint(Painter& painter) override;
    void focus() override;
    void on_show() override;
    void on_hide() override;

    ErrorCode update_display(const DisplayData& display_data) noexcept;
    ErrorCode handle_user_input(uint32_t key) noexcept;

    void show_alert(const char* message, uint32_t duration_ms) noexcept;
    void show_error(ErrorCode error, uint32_t duration_ms) noexcept;

    ErrorCode start_scanning() noexcept;
    ErrorCode stop_scanning() noexcept;

    [[nodiscard]] bool is_scanning() const noexcept;
    ErrorCode set_scanning_mode(ScanningMode mode) noexcept;
    [[nodiscard]] ScanningMode get_scanning_mode() const noexcept;

    [[nodiscard]] const DisplayData& get_display_data() const noexcept;

    void clear_alert() noexcept;
    void clear_error() noexcept;

private:
    static constexpr uint16_t BIG_FREQUENCY_X = 4;
    static constexpr uint16_t BIG_FREQUENCY_Y = 6 * 16;
    static constexpr uint16_t BIG_FREQUENCY_WIDTH = 28 * 8;
    static constexpr uint16_t DRONE_TYPE_SPACING = 5;
    static constexpr uint16_t DRONE_TYPE_Y_OFFSET = 20;

    static constexpr uint16_t RSSI_TEXT_X = 5;
    static constexpr uint16_t RSSI_TEXT_Y_OFFSET = 100;
    static constexpr uint16_t RSSI_VALUE_X = 50;
    static constexpr uint16_t RSSI_DBM_X_BASE = 55;

    static constexpr uint16_t STATE_TEXT_X = 5;
    static constexpr uint16_t STATE_TEXT_Y_OFFSET = 120;
    static constexpr uint16_t SCANNING_TEXT_Y_OFFSET = 140;

    static constexpr uint16_t ALERT_X = 10;
    static constexpr uint16_t ALERT_Y = 100;
    static constexpr uint16_t ALERT_W = 220;
    static constexpr uint16_t ALERT_H = 50;
    static constexpr uint16_t ALERT_TEXT_OFFSET_X = 20;
    static constexpr uint16_t ALERT_TEXT_OFFSET_Y = 10;
    static constexpr uint16_t ALERT_MESSAGE_TEXT_OFFSET_Y = 30;

    static constexpr uint16_t ERROR_X = 10;
    static constexpr uint16_t ERROR_Y = 200;
    static constexpr uint16_t ERROR_W = 220;
    static constexpr uint16_t ERROR_H = 50;

    static constexpr uint32_t ERROR_DURATION_MS = 3000;

    void construct_objects() noexcept;
    void destruct_objects() noexcept;

    void on_spectrum_config(Message* const p) noexcept;
    void on_frame_sync(Message* const p) noexcept;

    private:
    alignas(MessageHandlerRegistration) static uint8_t s_message_handler_spectrum_buffer[sizeof(MessageHandlerRegistration)];
    alignas(MessageHandlerRegistration) static uint8_t s_message_handler_frame_buffer[sizeof(MessageHandlerRegistration)];
    NavigationView& nav_;

    ui::BigFrequency big_display_;

    HardwareController* hardware_ptr_;
    DatabaseManager* database_ptr_;
    DroneScanner* scanner_ptr_;

    ui::LNAGainField field_lna_{
        {4 * 8, 0}};
    ui::VGAGainField field_vga_{
        {11 * 8, 0}};
    ui::RFAmpField field_rf_amp_{
        {18 * 8, 0}};

    ui::Button button_start_stop_{
        {0, 16 * 16, 8 * 8, 28},
        "Start"};
    ui::Button button_mode_{
        {9 * 8, 16 * 16, 7 * 8, 28},
        "Mode"};
    ui::Button button_settings_{
        {17 * 8, 16 * 16, 7 * 8, 28},
        "Setup"};

    uint64_t current_frequency_{0};
    int32_t current_rssi_{RSSI_NOISE_FLOOR_DBM};
    ScannerState current_scanner_state_{ScannerState::IDLE};

    char displayed_drone_type_[5] = {'\0', '\0', '\0', '\0', '\0'};
    uint32_t drone_type_display_timer_{0};

    DisplayData* display_data_ptr_;

    bool scanning_{false};
    ScanningMode scanning_mode_{DEFAULT_SCANNING_MODE};

    bool alert_active_{false};
    char alert_message_[MAX_TEXT_LENGTH] = {0};
    uint32_t alert_start_time_{0};
    uint32_t alert_duration_ms_{0};

    bool error_active_{false};
    ErrorCode last_error_{ErrorCode::SUCCESS};
    uint32_t error_start_time_{0};
    uint32_t error_duration_ms_{0};

    uint16_t selected_button_{1};
    bool settings_visible_{false};

    ChannelSpectrumFIFO* spectrum_fifo_{nullptr};

    MessageHandlerRegistration* message_handler_spectrum_{nullptr};
    MessageHandlerRegistration* message_handler_frame_{nullptr};

    void draw_scanner_header(Painter& painter) noexcept;
    void draw_scanner_status(Painter& painter, uint16_t start_y) noexcept;
    void draw_alert_overlay(Painter& painter) noexcept;
    void draw_error_overlay(Painter& painter) noexcept;
    void draw_text(
        Painter& painter,
        const char* text,
        uint16_t x,
        uint16_t y,
        const ui::Style& style
    ) noexcept;

    [[nodiscard]] ErrorCode handle_start_stop_key() noexcept;
    [[nodiscard]] ErrorCode handle_mode_key() noexcept;
    [[nodiscard]] ErrorCode handle_settings_key() noexcept;

    [[nodiscard]] bool is_alert_active() const noexcept;
    [[nodiscard]] bool is_error_active() const noexcept;
    [[nodiscard]] const char* get_alert_message() const noexcept;
    [[nodiscard]] const char* get_error_message() const noexcept;

    void update_alert_timer(uint32_t current_time_ms) noexcept;
    void update_error_timer(uint32_t current_time_ms) noexcept;

    void update_status_text() noexcept;
    void bigdisplay_update(BigDisplayColor color) noexcept;
    void update_ui_state() noexcept;
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SCANNER_UI_HPP
