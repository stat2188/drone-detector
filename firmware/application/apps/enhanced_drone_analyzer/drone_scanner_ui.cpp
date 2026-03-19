#include "drone_scanner_ui.hpp"
#include "ui.hpp"
#include "scanner.hpp"
#include "database.hpp"
#include "hardware_controller.hpp"
#include "constants.hpp"
#include "string_format.hpp"
#include "baseband_api.hpp"
#include "portapack_persistent_memory.hpp"
#include "ch.h"
#include <new>

namespace drone_analyzer {

alignas(HardwareController) static uint8_t s_hardware_buffer[sizeof(HardwareController)];
static_assert(sizeof(HardwareController) <= sizeof(s_hardware_buffer), "HardwareController buffer overflow");

alignas(DatabaseManager) static uint8_t s_database_buffer[sizeof(DatabaseManager)];
static_assert(sizeof(DatabaseManager) <= sizeof(s_database_buffer), "DatabaseManager buffer overflow");

alignas(DroneScanner) static uint8_t s_scanner_buffer[sizeof(DroneScanner)];
static_assert(sizeof(DroneScanner) <= sizeof(s_scanner_buffer), "DroneScanner buffer overflow");

void DroneScannerUI::construct_objects() noexcept {
    hardware_ptr_ = new (&s_hardware_buffer[0]) HardwareController();
    database_ptr_ = new (&s_database_buffer[0]) DatabaseManager();
    scanner_ptr_ = new (&s_scanner_buffer[0]) DroneScanner(*database_ptr_, *hardware_ptr_);
}

void DroneScannerUI::destruct_objects() noexcept {
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

DroneScannerUI::DroneScannerUI(NavigationView& nav) noexcept
    : ui::View()
    , nav_(nav)
    , big_display_({BIG_FREQUENCY_X, BIG_FREQUENCY_Y, BIG_FREQUENCY_WIDTH, 52}, 0) {

    baseband::run_image(portapack::spi_flash::image_tag_wideband_spectrum);

    add_children({
        &field_lna_,
        &field_vga_,
        &field_rf_amp_,
        &big_display_,
        &button_start_stop_,
        &button_mode_,
        &button_settings_
    });

    construct_objects();

    if (scanner_ptr_ == nullptr) {
        show_error(ErrorCode::INITIALIZATION_FAILED, ERROR_DURATION_MS);
        initialization_failed_ = true;
        return;
    }

    ErrorCode err = scanner_ptr_->initialize();
    if (err != ErrorCode::SUCCESS) {
        destruct_objects();
        show_error(err, ERROR_DURATION_MS);
        initialization_failed_ = true;
        return;
    }

    button_start_stop_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        if (scanning_) {
            scanner_ptr_->stop_scanning();
            scanning_ = false;
            button_start_stop_.set_text("Start");
        } else {
            scanner_ptr_->start_scanning();
            scanning_ = true;
            button_start_stop_.set_text("Stop");
        }
    };

    button_mode_.on_select = [this](ui::Button&) {
        if (initialization_failed_) return;
        const uint8_t current = static_cast<uint8_t>(scanning_mode_);
        scanning_mode_ = static_cast<ScanningMode>((current + 1) % SCANNING_MODE_COUNT);
    };

    button_settings_.on_select = [this, &nav](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        ScanConfig config = scanner_ptr_->get_config();
        nav.push<DroneSettingsView>(config, scanner_ptr_);
    };
}

DroneScannerUI::~DroneScannerUI() noexcept {
    if (scanner_ptr_ != nullptr && scanning_) {
        scanner_ptr_->stop_scanning();
    }
    destruct_objects();

    portapack::receiver_model.set_sampling_rate(3072000);
    portapack::receiver_model.disable();
    baseband::shutdown();
}

void DroneScannerUI::focus() {
    button_start_stop_.focus();
}

void DroneScannerUI::on_show() {
    baseband::set_spectrum(DEFAULT_SAMPLE_RATE_HZ, 31);
    baseband::spectrum_streaming_start();
}

void DroneScannerUI::on_hide() {
    if (scanner_ptr_ != nullptr && scanning_) {
        scanner_ptr_->stop_scanning();
        scanning_ = false;
    }
    baseband::spectrum_streaming_stop();
}

void DroneScannerUI::paint(Painter& painter) {
    (void)painter;

    if (alert_active_) {
        const SystemTime now = chTimeNow();
        if ((now - alert_start_time_) >= alert_duration_ms_) {
            alert_active_ = false;
        }
    }

    if (error_active_) {
        const SystemTime now = chTimeNow();
        if ((now - error_start_time_) >= error_duration_ms_) {
            error_active_ = false;
        }
    }
}

void DroneScannerUI::show_alert(const char* message, uint32_t duration_ms) noexcept {
    if (message == nullptr) return;
    safe_str_copy(alert_message_, MAX_TEXT_LENGTH, message);
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

void DroneScannerUI::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    if (scanner_ptr_ == nullptr) return;

    uint8_t max_power = 0;
    for (size_t i = 0; i < 256; ++i) {
        if (spectrum.db[i] > max_power) {
            max_power = spectrum.db[i];
        }
    }

    current_rssi_ = static_cast<int32_t>(max_power) - 120;

    if (current_rssi_ > RSSI_DETECTION_THRESHOLD_DBM) {
        scanner_ptr_->process_spectrum_message(spectrum);
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
    } else {
        current_rssi_ = RSSI_NOISE_FLOOR_DBM;
    }

    ErrorResult<FreqHz> freq_result = scanner_ptr_->get_current_frequency();
    current_frequency_ = freq_result.has_value() ? freq_result.value() : 0;

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
        char drone_type[5]{'\0'};
        ErrorCode err = scanner_ptr_->get_current_drone_type(drone_type, 5);
        if (err == ErrorCode::SUCCESS && drone_type[0] != '\0') {
            safe_str_copy(displayed_drone_type_, MAX_DRONE_TYPE_DISPLAY, drone_type);
            drone_type_display_timer_ = chTimeNow();
        }
    } else {
        displayed_drone_type_[0] = '\0';
        drone_type_display_timer_ = 0;
    }

    set_dirty();
}

} // namespace drone_analyzer
