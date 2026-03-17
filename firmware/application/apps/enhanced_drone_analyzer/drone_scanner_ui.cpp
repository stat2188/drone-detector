#include "drone_scanner_ui.hpp"
#include "ui.hpp"
#include "scanner.hpp"
#include "database.hpp"
#include "string_format.hpp"
#include "baseband_api.hpp"
#include <cstring>
#include <cinttypes>
#include <cstdio>
#include <new>
#include <type_traits>
#include "message.hpp"
#include "drone_settings.hpp"

namespace drone_analyzer {

alignas(HardwareController) static uint8_t s_hardware_buffer[sizeof(HardwareController)];
static_assert(sizeof(HardwareController) <= sizeof(s_hardware_buffer), "HardwareController buffer overflow risk");

alignas(DatabaseManager) static uint8_t s_database_buffer[sizeof(DatabaseManager)];
static_assert(sizeof(DatabaseManager) <= sizeof(s_database_buffer), "DatabaseManager buffer overflow risk");

alignas(DroneScanner) static uint8_t s_scanner_buffer[sizeof(DroneScanner)];
static_assert(sizeof(DroneScanner) <= sizeof(s_scanner_buffer), "DroneScanner buffer overflow risk");

alignas(DisplayData) static uint8_t s_display_data_buffer[sizeof(DisplayData)];
static_assert(sizeof(DisplayData) <= sizeof(s_display_data_buffer), "DisplayData buffer overflow risk");

alignas(MessageHandlerRegistration) static uint8_t s_message_handler_spectrum_buffer[sizeof(MessageHandlerRegistration)];
static_assert(sizeof(MessageHandlerRegistration) <= sizeof(s_message_handler_spectrum_buffer), "MessageHandlerRegistration buffer overflow risk");

alignas(MessageHandlerRegistration) static uint8_t s_message_handler_frame_buffer[sizeof(MessageHandlerRegistration)];
static_assert(sizeof(MessageHandlerRegistration) <= sizeof(s_message_handler_frame_buffer), "MessageHandlerRegistration buffer overflow risk");

void DroneScannerUI::construct_objects() noexcept {
    hardware_ptr_ = new(&s_hardware_buffer[0]) HardwareController();
    database_ptr_ = new(&s_database_buffer[0]) DatabaseManager();
    scanner_ptr_ = new(&s_scanner_buffer[0]) DroneScanner(*database_ptr_, *hardware_ptr_);
    display_data_ptr_ = new(&s_display_data_buffer[0]) DisplayData();
    
    init_message_handlers();
}

void DroneScannerUI::destruct_objects() noexcept {
    destruct_message_handlers();
    
    if (display_data_ptr_ != nullptr) {
        display_data_ptr_->~DisplayData();
        display_data_ptr_ = nullptr;
    }
    
    if (scanner_ptr_ != nullptr) {
        scanner_ptr_->~DroneScanner();
        scanner_ptr_ = nullptr;
    }
    
    if (database_ptr_ != nullptr) {
        database_ptr_->~DatabaseManager();
        database_ptr_ = nullptr;
    }
    
    if (hardware_ptr_ != nullptr) {
        hardware_ptr_->~HardwareController();
        hardware_ptr_ = nullptr;
    }
}

void DroneScannerUI::init_message_handlers() noexcept {
    message_handler_spectrum_ptr_ = new(&s_message_handler_spectrum_buffer[0]) MessageHandlerRegistration(
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }
    );
    
    message_handler_frame_ptr_ = new(&s_message_handler_frame_buffer[0]) MessageHandlerRegistration(
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            if (this->spectrum_fifo_) {
                ChannelSpectrum spectrum;
                while (this->spectrum_fifo_->out(spectrum)) {
                    this->on_channel_spectrum(spectrum);
                }
            }
            this->update_ui_state();
        }
    );
}

void DroneScannerUI::destruct_message_handlers() noexcept {
    if (message_handler_frame_ptr_ != nullptr) {
        message_handler_frame_ptr_->~MessageHandlerRegistration();
        message_handler_frame_ptr_ = nullptr;
    }
    
    if (message_handler_spectrum_ptr_ != nullptr) {
        message_handler_spectrum_ptr_->~MessageHandlerRegistration();
        message_handler_spectrum_ptr_ = nullptr;
    }
}

DroneScannerUI::DroneScannerUI(NavigationView& nav) noexcept
    : ui::View()
    , nav_(nav)
    , hardware_ptr_(nullptr)
    , database_ptr_(nullptr)
    , scanner_ptr_(nullptr)
    , display_data_ptr_(nullptr)
    , scanning_(false)
    , scanning_mode_(DEFAULT_SCANNING_MODE)
    , alert_active_(false)
    , error_active_(false)
    , last_error_(ErrorCode::SUCCESS)
    , message_handler_spectrum_ptr_(nullptr)
    , message_handler_frame_ptr_(nullptr)
    , big_display_({BIG_FREQUENCY_X, BIG_FREQUENCY_Y, BIG_FREQUENCY_WIDTH, 52}, 0)
    , field_lna_({4 * 8, 0})
    , field_vga_({11 * 8, 0})
    , field_rf_amp_({18 * 8, 0})
    , button_start_stop_({0, 16 * 16, 8 * 8, 28}, "Start")
    , button_mode_({9 * 8, 16 * 16, 7 * 8, 28}, "Mode")
    , button_settings_({17 * 8, 16 * 16, 7 * 8, 28}, "Setup")
    , current_frequency_(0)
    , current_rssi_(RSSI_NOISE_FLOOR_DBM)
    , current_scanner_state_(ScannerState::IDLE)
    , displayed_drone_type_{'\0', '\0', '\0', '\0', '\0'}
    , drone_type_display_timer_(0)
    , alert_start_time_(0)
    , alert_duration_ms_(0)
    , error_start_time_(0)
    , error_duration_ms_(0)
    , selected_button_(1)
    , settings_visible_(false) {

    construct_objects();

    if (display_data_ptr_ != nullptr) {
        display_data_ptr_->clear();
    }

    if (scanner_ptr_ != nullptr) {
        ErrorCode err = scanner_ptr_->initialize();
        if (err != ErrorCode::SUCCESS) {
            show_error(err, ERROR_DURATION_MS);
            destruct_objects();
            return;
        }
    }

    add_children({
        &field_lna_,
        &field_vga_,
        &field_rf_amp_,
        &big_display_,
        &button_start_stop_,
        &button_mode_,
        &button_settings_
    });

    button_start_stop_.on_select = [this](ui::Button&) {
        if (scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        
        if (scanning_) {
            stop_scanning();
            button_start_stop_.set_text("Start");
        } else {
            start_scanning();
            button_start_stop_.set_text("Stop");
        }
    };

    button_mode_.on_select = [this](ui::Button&) {
        const uint8_t current_mode = static_cast<uint8_t>(scanning_mode_);
        const uint8_t next_mode = (current_mode + 1) % SCANNING_MODE_COUNT;
        scanning_mode_ = static_cast<ScanningMode>(next_mode);
    };

    button_settings_.on_select = [this, &nav](ui::Button&) {
        if (scanner_ptr_ != nullptr) {
            auto config = scanner_ptr_->get_config();
            nav.push<DroneSettingsView>(config);
        }
    };
}

DroneScannerUI::~DroneScannerUI() noexcept {
    destruct_objects();
    
    portapack::receiver_model.set_sampling_rate(3072000);
    portapack::receiver_model.disable();
    baseband::shutdown();
}

void DroneScannerUI::focus() {
    button_start_stop_.focus();
}

void DroneScannerUI::on_show() {
    if (scanner_ptr_ != nullptr) {
        update_ui_state();
    }
}

void DroneScannerUI::on_hide() {
    if (scanner_ptr_ != nullptr) {
        stop_scanning();
    }
    
    baseband::spectrum_streaming_stop();
}

void DroneScannerUI::paint(Painter& painter) {
    uint16_t y_offset = 5;

    draw_scanner_header(painter);
    y_offset += 30;

    draw_scanner_status(painter, y_offset);
    y_offset += 40;

    draw_threat_summary(painter, y_offset);

    if (is_alert_active()) {
        draw_alert_overlay(painter);
    }

    if (is_error_active()) {
        draw_error_overlay(painter);
    }

    SystemTime current_time = chTimeNow();
    update_alert_timer(current_time);
    update_error_timer(current_time);
}

ErrorCode DroneScannerUI::update_display(const DisplayData& display_data) noexcept {
    if (display_data_ptr_ == nullptr) {
        return ErrorCode::INVALID_PARAMETER;
    }

    const ErrorCode error = validate_drone_buffer(
        display_data.drones,
        display_data.drone_count,
        MAX_DISPLAYED_DRONES
    );
    
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    *display_data_ptr_ = display_data;

    update_status_text();

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScannerUI::handle_user_input(uint32_t key) noexcept {
    switch (key) {
        case 1:
            return handle_start_stop_key();

        case 2:
            return handle_mode_key();

        case 3:
            return handle_settings_key();

        default:
            return ErrorCode::INVALID_PARAMETER;
    }
}

ErrorCode DroneScannerUI::handle_start_stop_key() noexcept {
    if (scanner_ptr_ == nullptr) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }

    if (scanning_) {
        return stop_scanning();
    } else {
        return start_scanning();
    }
}

ErrorCode DroneScannerUI::handle_mode_key() noexcept {
    const uint8_t current_mode = static_cast<uint8_t>(scanning_mode_);
    const uint8_t next_mode = (current_mode + 1) % SCANNING_MODE_COUNT;
    scanning_mode_ = static_cast<ScanningMode>(next_mode);
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScannerUI::handle_settings_key() noexcept {
    settings_visible_ = !settings_visible_;
    
    return ErrorCode::SUCCESS;
}

void DroneScannerUI::show_alert(const char* message, uint32_t duration_ms) noexcept {
    if (message == nullptr) {
        return;
    }

    const ErrorCode copy_err = safe_str_copy(alert_message_, MAX_TEXT_LENGTH, message);
    if (copy_err != ErrorCode::SUCCESS) {
        alert_message_[0] = '\0';
    }

    alert_active_ = true;
    alert_start_time_ = chTimeNow();
    alert_duration_ms_ = duration_ms;
}

void DroneScannerUI::show_error(ErrorCode error, uint32_t duration_ms) noexcept {
    error_active_ = true;
    last_error_ = error;
    error_start_time_ = chTimeNow();
    error_duration_ms_ = duration_ms;
}

void DroneScannerUI::clear_alert() noexcept {
    alert_active_ = false;
    alert_message_[0] = '\0';
}

void DroneScannerUI::clear_error() noexcept {
    error_active_ = false;
    last_error_ = ErrorCode::SUCCESS;
}

ErrorCode DroneScannerUI::start_scanning() noexcept {
    if (scanner_ptr_ == nullptr) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }

    if (scanning_) {
        return ErrorCode::SUCCESS;
    }

    ErrorCode err = scanner_ptr_->start_scanning();
    if (err == ErrorCode::SUCCESS) {
        scanning_ = true;
        update_status_text();
    }

    return err;
}

ErrorCode DroneScannerUI::stop_scanning() noexcept {
    if (scanner_ptr_ == nullptr) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }

    if (!scanning_) {
        return ErrorCode::SUCCESS;
    }

    ErrorCode err = scanner_ptr_->stop_scanning();
    if (err == ErrorCode::SUCCESS) {
        scanning_ = false;
        update_status_text();
    }

    return err;
}

bool DroneScannerUI::is_scanning() const noexcept {
    return scanning_;
}

ErrorCode DroneScannerUI::set_scanning_mode(ScanningMode mode) noexcept {
    if (static_cast<uint8_t>(mode) >= SCANNING_MODE_COUNT) {
        return ErrorCode::INVALID_PARAMETER;
    }

    scanning_mode_ = mode;
    return ErrorCode::SUCCESS;
}

ScanningMode DroneScannerUI::get_scanning_mode() const noexcept {
    return scanning_mode_;
}

const DisplayData& DroneScannerUI::get_display_data() const noexcept {
    static DisplayData empty_data;
    
    if (scanner_ptr_ == nullptr || display_data_ptr_ == nullptr) {
        return empty_data;
    }
    
    return *display_data_ptr_;
}

void DroneScannerUI::draw_scanner_header(Painter& painter) noexcept {
    if (drone_type_display_timer_ != 0) {
        SystemTime now = chTimeNow();
        
        const uint32_t elapsed = now - drone_type_display_timer_;
        if (elapsed >= DRONE_TYPE_DISPLAY_DURATION_MS) {
            displayed_drone_type_[0] = '\0';
            drone_type_display_timer_ = 0;
        }
    }

    if (displayed_drone_type_[0] != '\0') {
        const auto& style = *Theme::getInstance()->fg_red;
        
        uint16_t drone_type_x = BIG_FREQUENCY_X + BIG_FREQUENCY_WIDTH + DRONE_TYPE_SPACING;
        uint16_t drone_type_y = BIG_FREQUENCY_Y + DRONE_TYPE_Y_OFFSET;

        draw_text(painter, displayed_drone_type_, drone_type_x, drone_type_y, style);
    }
}

void DroneScannerUI::draw_scanner_status(Painter& painter, uint16_t start_y) noexcept {
    const auto& text_style = *Theme::getInstance()->bg_darkest_small;

    draw_text(painter, "RSSI:", RSSI_TEXT_X, start_y + RSSI_TEXT_Y_OFFSET, text_style);

    char rssi_str[16];
    snprintf(rssi_str, sizeof(rssi_str), "%ld", static_cast<int32_t>(current_rssi_));
    draw_text(painter, rssi_str, RSSI_VALUE_X, start_y + RSSI_TEXT_Y_OFFSET, text_style);
    draw_text(painter, "dBm", RSSI_DBM_X_BASE + strlen(rssi_str) * 5, start_y + RSSI_TEXT_Y_OFFSET, text_style);

    const char* state_text = "IDLE";
    switch (current_scanner_state_) {
        case ScannerState::SCANNING:
            state_text = "SCANNING";
            break;
        case ScannerState::LOCKING:
            state_text = "LOCKING";
            break;
        case ScannerState::TRACKING:
            state_text = "TRACKING";
            break;
        case ScannerState::PAUSED:
            state_text = "PAUSED";
            break;
        default:
            state_text = "IDLE";
    }

    draw_text(painter, state_text, STATE_TEXT_X, start_y + STATE_TEXT_Y_OFFSET, text_style);

    if (scanner_ptr_ != nullptr && scanner_ptr_->is_scanning()) {
        draw_text(painter, "Scanning...", STATE_TEXT_X, start_y + SCANNING_TEXT_Y_OFFSET, text_style);
    }
}

void DroneScannerUI::draw_threat_summary(Painter& painter, uint16_t start_y) noexcept {
    (void)painter;
    (void)start_y;
}

void DroneScannerUI::draw_alert_overlay(Painter& painter) noexcept {
    if (!alert_active_) {
        return;
    }

    const auto& style = *Theme::getInstance()->fg_red;

    draw_rectangle(painter, ALERT_X, ALERT_Y, ALERT_W, ALERT_H, COLOR_CRITICAL_THREAT, true);

    draw_text(painter, "ALERT", ALERT_X + ALERT_TEXT_OFFSET_X, ALERT_Y + ALERT_TEXT_OFFSET_Y, style);
    draw_text(painter, alert_message_, ALERT_X + ALERT_TEXT_OFFSET_X, ALERT_Y + ALERT_MESSAGE_TEXT_OFFSET_Y, style);
}

void DroneScannerUI::draw_error_overlay(Painter& painter) noexcept {
    if (!error_active_) {
        return;
    }

    const auto& style = *Theme::getInstance()->fg_yellow;

    draw_rectangle(painter, ERROR_X, ERROR_Y, ERROR_W, ERROR_H, COLOR_HIGH_THREAT, true);

    draw_text(painter, "ERROR", ERROR_X + ALERT_TEXT_OFFSET_X, ERROR_Y + ALERT_TEXT_OFFSET_Y, style);
    draw_text(painter, error_to_string(last_error_), ERROR_X + ALERT_TEXT_OFFSET_X, ERROR_Y + ALERT_MESSAGE_TEXT_OFFSET_Y, style);
}

void DroneScannerUI::draw_text(
    Painter& painter,
    const char* text,
    uint16_t x,
    uint16_t y,
    const ui::Style& style
) noexcept {
    if (text == nullptr || text[0] == '\0') {
        return;
    }
    
    painter.draw_string(Point{x, y}, style, text);
}

void DroneScannerUI::draw_rectangle(
    Painter& painter,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color,
    bool fill
) noexcept {
    if (width == 0 || height == 0) {
        return;
    }

    if (fill) {
        painter.fill_rectangle({x, y, width, height}, color);
    } else {
        painter.draw_rectangle({x, y, width, height}, color);
    }
}

void DroneScannerUI::update_status_text() noexcept {
}

bool DroneScannerUI::is_alert_active() const noexcept {
    return alert_active_;
}

bool DroneScannerUI::is_error_active() const noexcept {
    return error_active_;
}

const char* DroneScannerUI::get_alert_message() const noexcept {
    return alert_message_;
}

const char* DroneScannerUI::get_error_message() const noexcept {
    return error_to_string(last_error_);
}

void DroneScannerUI::update_alert_timer(uint32_t current_time_ms) noexcept {
    if (!alert_active_) {
        return;
    }

    const uint32_t elapsed = current_time_ms - alert_start_time_;
    if (elapsed >= alert_duration_ms_) {
        clear_alert();
    }
}

void DroneScannerUI::update_error_timer(uint32_t current_time_ms) noexcept {
    if (!error_active_) {
        return;
    }

    const uint32_t elapsed = current_time_ms - error_start_time_;
    if (elapsed >= error_duration_ms_) {
        clear_error();
    }
}

void DroneScannerUI::bigdisplay_update(BigDisplayColor color) noexcept {
    switch (color) {
        case BigDisplayColor::GREY:
            big_display_.set_style(Theme::getInstance()->fg_medium);
            break;
        case BigDisplayColor::YELLOW:
            big_display_.set_style(Theme::getInstance()->fg_yellow);
            break;
        case BigDisplayColor::GREEN:
            big_display_.set_style(Theme::getInstance()->fg_green);
            break;
        case BigDisplayColor::RED:
            big_display_.set_style(Theme::getInstance()->fg_red);
            break;
    }

    big_display_.set(current_frequency_);
}

void DroneScannerUI::update_ui_state() noexcept {
    if (scanner_ptr_ == nullptr) {
        current_scanner_state_ = ScannerState::IDLE;
        current_rssi_ = RSSI_NOISE_FLOOR_DBM;
        current_frequency_ = 0;
        bigdisplay_update(BigDisplayColor::GREY);
        displayed_drone_type_[0] = '\0';
        drone_type_display_timer_ = 0;
        return;
    }
    
    current_scanner_state_ = scanner_ptr_->get_state();
    
    TrackedDrone tracked_drones[MAX_TRACKED_DRONES];
    const size_t drone_count = scanner_ptr_->get_tracked_drones(tracked_drones, MAX_TRACKED_DRONES);
    
    if (drone_count > 0) {
        current_rssi_ = tracked_drones[0].rssi;
        current_frequency_ = tracked_drones[0].frequency;
    } else {
        current_rssi_ = RSSI_NOISE_FLOOR_DBM;
        current_frequency_ = 0;
    }
    
    BigDisplayColor color = BigDisplayColor::GREY;
    switch (current_scanner_state_) {
        case ScannerState::SCANNING:
            color = BigDisplayColor::GREY;
            break;
        case ScannerState::LOCKING:
            color = BigDisplayColor::YELLOW;
            break;
        case ScannerState::TRACKING:
            color = (current_rssi_ >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM) 
                  ? BigDisplayColor::RED 
                  : BigDisplayColor::GREEN;
            break;
        default:
            color = BigDisplayColor::GREY;
            break;
    }
    
    bigdisplay_update(color);
    
    if (current_scanner_state_ == ScannerState::LOCKING) {
        const char* drone_type = scanner_ptr_->get_current_drone_type();
        
        if (drone_type != nullptr && drone_type[0] != '\0') {
            const ErrorCode copy_err = safe_str_copy(displayed_drone_type_, MAX_DRONE_TYPE_DISPLAY, drone_type);
            if (copy_err == ErrorCode::SUCCESS) {
                drone_type_display_timer_ = chTimeNow();
            }
        }
    } else {
        displayed_drone_type_[0] = '\0';
        drone_type_display_timer_ = 0;
    }
    
    set_dirty();
}

void DroneScannerUI::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    if (scanner_ptr_ == nullptr) {
        return;
    }
    
    uint8_t max_power = 0;
    for (size_t i = 0; i < 256; i++) {
        if (spectrum.db[i] > max_power) {
            max_power = spectrum.db[i];
        }
    }
    
    const int32_t rssi = static_cast<int32_t>(max_power) - 120;
    
    if (rssi > RSSI_DETECTION_THRESHOLD_DBM) {
        scanner_ptr_->update_tracked_drones(
            current_frequency_,
            rssi,
            chTimeNow()
        );
    }
}

} // namespace drone_analyzer
