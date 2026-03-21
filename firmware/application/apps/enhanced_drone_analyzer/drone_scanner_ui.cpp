#include "drone_scanner_ui.hpp"
#include "ui.hpp"
#include "ui_fileman.hpp"
#include "scanner.hpp"
#include "scanner_thread.hpp"
#include "drone_settings.hpp"
#include "database.hpp"
#include "hardware_controller.hpp"
#include "audio_alerts.hpp"
#include "constants.hpp"
#include "string_format.hpp"
#include "baseband_api.hpp"
#include "portapack_persistent_memory.hpp"
#include "file_path.hpp"
#include "ch.h"
#include <new>
#include <array>

namespace drone_analyzer {

alignas(HardwareController) static uint8_t s_hardware_buffer[sizeof(HardwareController)];
static_assert(sizeof(HardwareController) <= sizeof(s_hardware_buffer), "HardwareController buffer overflow");

alignas(DatabaseManager) static uint8_t s_database_buffer[sizeof(DatabaseManager)];
static_assert(sizeof(DatabaseManager) <= sizeof(s_database_buffer), "DatabaseManager buffer overflow");

alignas(DroneScanner) static uint8_t s_scanner_buffer[sizeof(DroneScanner)];
static_assert(sizeof(DroneScanner) <= sizeof(s_scanner_buffer), "DroneScanner buffer overflow");

alignas(ScannerThread) static uint8_t s_scanner_thread_buffer[sizeof(ScannerThread)];
static_assert(sizeof(ScannerThread) <= sizeof(s_scanner_thread_buffer), "ScannerThread buffer overflow");

DroneScannerUI::DroneScannerUI(NavigationView& nav) noexcept
    : View()
    , nav_(nav)
    , big_display_{{BIG_FREQUENCY_X, BIG_FREQUENCY_Y, BIG_FREQUENCY_WIDTH, 52}, 0}  // Y moved to 1*16=16
    , message_handler_spectrum_config{
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }
    }
    , message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            if (this->spectrum_fifo_ != nullptr) {
                ChannelSpectrum spectrum;
                if (this->spectrum_fifo_->out(spectrum)) {
                    this->on_channel_spectrum(spectrum);
                }
            }
            this->refresh_ui();
        }
    }
    , message_handler_retune{
        Message::ID::Retune,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const RetuneMessage*>(p);
            this->on_retune(message.freq, message.range);
        }
    } {
    add_children({
        &labels_,
        &field_lna_,
        &field_vga_,
        &field_rf_amp_,
        &field_volume_,
        &big_display_,
        &drone_display_,
        &button_start_stop_,
        &button_mode_,
        &button_load_,
        &button_settings_
    });

    // Register button callbacks BEFORE any early returns
    // Buttons must always respond, even if init fails
    button_start_stop_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        if (scanning_) {
            scanner_thread_->set_scanning(false);
            (void)scanner_ptr_->stop_scanning();
            baseband::spectrum_streaming_stop();
            scanning_ = false;
            button_start_stop_.set_text("Start");
        } else {
            baseband::spectrum_streaming_start();
            (void)scanner_ptr_->start_scanning();
            scanner_thread_->set_scanning(true);
            scanning_ = true;
            button_start_stop_.set_text("Stop");
        }
    };

    button_mode_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        ScanConfig config = scanner_ptr_->get_config();
        const uint8_t current = static_cast<uint8_t>(config.mode);
        config.mode = static_cast<ScanningMode>((current + 1) % SCANNING_MODE_COUNT);
        const ErrorCode err = scanner_ptr_->set_config(config);
        if (err != ErrorCode::SUCCESS) {
            show_error(err, ERROR_DURATION_MS);
            return;
        }
        scanning_mode_ = config.mode;

        // Update button text to show current mode
        static const char* mode_names[] = {"Single", "Hopping", "Sequential", "Targeted", "Spectrometer"};
        button_mode_.set_text(mode_names[static_cast<uint8_t>(scanning_mode_)]);
    };

    button_load_.on_select = [this, &nav](ui::Button&) {
        auto open_view = nav.push<FileLoadView>(".TXT");
        open_view->push_dir(freqman_dir);
        open_view->on_changed = [this, &nav](std::filesystem::path new_file_path) {
            // Stop scanning if active
            if (scanning_) {
                scanner_thread_->set_scanning(false);
                (void)scanner_ptr_->stop_scanning();
                baseband::spectrum_streaming_stop();
                scanning_ = false;
                button_start_stop_.set_text("Start");
            }

            // Extract filename without extension (e.g., "DRONES" from "/FREQMAN/DRONES.TXT")
            const auto stem = new_file_path.stem().string();
            char filename[32];
            size_t i = 0;
            while (i < 31 && i < stem.length() && stem[i] != '\0') {
                filename[i] = stem[i];
                ++i;
            }
            filename[i] = '\0';

            // Set new database file and reload
            database_ptr_->set_database_file(filename);

            // Clear existing tracked drones
            scanner_ptr_->clear_tracked_drones();

            // Load new database
            const ErrorCode err = database_ptr_->load_frequency_database();
            db_loaded_ = (err == ErrorCode::SUCCESS);
            db_entry_count_ = database_ptr_->get_database_size();
            if (err == ErrorCode::SUCCESS) {
                char msg[32];
                snprintf(msg, sizeof(msg), "Loaded %lu entries", (unsigned long)db_entry_count_);
                show_alert(msg, 2000);
            } else {
                show_error(err, ERROR_DURATION_MS);
            }
        };
    };

    button_settings_.on_select = [this, &nav](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        ScanConfig config = scanner_ptr_->get_config();
        nav.push<DroneSettingsView>(config, scanner_ptr_, &drone_display_);
    };

    // Hardware initialization (callbacks are already set, safe to early-return)
    construct_objects();

    if (scanner_ptr_ == nullptr) {
        show_error(ErrorCode::INITIALIZATION_FAILED, ERROR_DURATION_MS);
        initialization_failed_ = true;
        return;
    }

    // Database load is non-fatal — scanner works on default frequency if DB missing
    const auto db_result = database_ptr_->load_frequency_database();
    db_loaded_ = (db_result == ErrorCode::SUCCESS);
    db_entry_count_ = database_ptr_->get_database_size();
    if (db_result != ErrorCode::SUCCESS) {
        show_error(db_result, ERROR_DURATION_MS);
    }

    const ErrorCode init_err = scanner_ptr_->initialize();
    if (init_err != ErrorCode::SUCCESS) {
        show_error(init_err, ERROR_DURATION_MS);
    }

    baseband::run_image(portapack::spi_flash::image_tag_wideband_spectrum);
    baseband::set_spectrum(DEFAULT_SAMPLE_RATE_HZ, 31);
    portapack::receiver_model.enable();

    scanner_thread_->start();

    scanning_ = false;

    scanner_ptr_->set_alert_callback([](ThreatLevel level) {
        AudioAlertManager::play_alert(level);
    });

    ScanConfig config;
    config.mode = scanning_mode_;
    config.rssi_threshold_dbm = RSSI_DETECTION_THRESHOLD_DBM;
    config.scan_interval_ms = SCAN_CYCLE_INTERVAL_MS;
    const ErrorCode config_err = scanner_ptr_->set_config(config);
    if (config_err != ErrorCode::SUCCESS) {
        show_error(config_err, ERROR_DURATION_MS);
    }
}

DroneScannerUI::~DroneScannerUI() noexcept {
    destruct_objects();

    portapack::receiver_model.disable();
    baseband::shutdown();
}

void DroneScannerUI::focus() {
    button_start_stop_.focus();
}

void DroneScannerUI::construct_objects() noexcept {
    hardware_ptr_ = new (&s_hardware_buffer[0]) HardwareController();
    database_ptr_ = new (&s_database_buffer[0]) DatabaseManager();
    scanner_ptr_ = new (&s_scanner_buffer[0]) DroneScanner(*database_ptr_, *hardware_ptr_);
    scanner_thread_ = new (&s_scanner_thread_buffer[0]) ScannerThread(*scanner_ptr_);
}

void DroneScannerUI::destruct_objects() noexcept {
    if (scanner_thread_ != nullptr) {
        scanner_thread_->stop();
        scanner_thread_->~ScannerThread();
        scanner_thread_ = nullptr;
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

void DroneScannerUI::on_show() {
    if (scanner_thread_ != nullptr && !scanner_thread_->is_active()) {
        scanner_thread_->start();
    }
}

void DroneScannerUI::on_hide() {
    if (scanning_) {
        scanner_thread_->set_scanning(false);
        baseband::spectrum_streaming_stop();
        scanning_ = false;
        button_start_stop_.set_text("Start");
    }
    if (scanner_thread_ != nullptr) {
        scanner_thread_->stop();
    }
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
    (void)safe_str_copy(alert_message_, MAX_TEXT_LENGTH, message);
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
    
    // BigFrequency::set() only accepts rf::Frequency (numeric), not strings
    // When frequency is 0 (uninitialized), display 0 Hz which formats as "0.000"
    big_display_.set(current_frequency_);
}

void DroneScannerUI::on_retune(FreqHz freq, uint32_t range) noexcept {
    (void)range;
    current_frequency_ = freq;
}

void DroneScannerUI::refresh_ui() noexcept {
    if (scanner_ptr_ == nullptr || initialization_failed_) {
        current_scanner_state_ = ScannerState::IDLE;
        current_rssi_ = RSSI_NOISE_FLOOR_DBM;
        current_frequency_ = 0;
        bigdisplay_update(BigDisplayColor::GREY);
        displayed_drone_type_[0] = '\0';
        drone_type_display_timer_ = 0;
        drone_display_.set_status_text(STATUS_ERROR);
        return;
    }

    current_scanner_state_ = scanner_ptr_->get_state();

    // Build display data from tracked drones
    // Use single get_tracked_drones call for consistency (one lock, one snapshot)
    // Static to save ~1500 bytes stack per frame (called at 60 FPS from DisplayFrameSync)
    static DisplayData display_data;
    display_data.clear();

    {
        static TrackedDrone drones[MAX_DISPLAYED_DRONES];
        const size_t count = scanner_ptr_->get_tracked_drones(drones, MAX_DISPLAYED_DRONES);
        display_data.drone_count = count;
        for (size_t i = 0; i < count; ++i) {
            display_data.drones[i] = DisplayDroneEntry(drones[i]);
        }
    }

    // Update status text based on state and database status
    switch (current_scanner_state_) {
        case ScannerState::SCANNING:
            drone_display_.set_status_text(STATUS_SCANNING);
            break;
        case ScannerState::LOCKING:
        case ScannerState::TRACKING:
            if (display_data.drone_count > 0) {
                drone_display_.set_status_text(STATUS_READY);
            } else {
                drone_display_.set_status_text(STATUS_NO_DRONES);
            }
            break;
        case ScannerState::PAUSED:
            drone_display_.set_status_text(STATUS_READY);
            break;
        default:
            // IDLE or unknown: show database-aware status
            if (!db_loaded_) {
                drone_display_.set_status_text("No DB");
            } else if (db_entry_count_ == 0) {
                drone_display_.set_status_text("DB empty");
            } else {
                // Show "Ready (N)" so user knows DB is loaded with N entries
                static char ready_buf[MAX_TEXT_LENGTH];
                snprintf(ready_buf, sizeof(ready_buf), "Ready (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(ready_buf);
            }
            break;
    }

    drone_display_.update_display_data(display_data);

    // Feed histogram data from scanner (thread-safe snapshot)
    {
        static uint16_t hist_data[HISTOGRAM_BUFFER_SIZE];
        const size_t hist_count = scanner_ptr_->get_histogram_snapshot(
            hist_data, HISTOGRAM_BUFFER_SIZE
        );
        if (hist_count > 0) {
            drone_display_.set_histogram_data(hist_data, hist_count);
        }
    }

    // Update big frequency display (use already-fetched drone data)
    RssiValue drone_rssi = RSSI_NOISE_FLOOR_DBM;
    if (display_data.drone_count > 0) {
        drone_rssi = display_data.drones[0].rssi;
    }
    current_rssi_ = drone_rssi;

    BigDisplayColor color = BigDisplayColor::GREY;
    switch (current_scanner_state_) {
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
        char drone_type[5]{'\0', '\0', '\0', '\0'};
        ErrorCode err = scanner_ptr_->get_current_drone_type(drone_type, 5);
        if (err == ErrorCode::SUCCESS && drone_type[0] != '\0') {
            (void)safe_str_copy(displayed_drone_type_, MAX_DRONE_TYPE_DISPLAY, drone_type);
            drone_type_display_timer_ = chTimeNow();
        }
    } else {
        displayed_drone_type_[0] = '\0';
        drone_type_display_timer_ = 0;
    }

    set_dirty();
}

void DroneScannerUI::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    if (scanner_ptr_ != nullptr && scanning_) {
        (void)scanner_ptr_->process_spectrum_message(spectrum);

        // Feed spectrum to DroneDisplay for visualization only when scanning
        drone_display_.set_spectrum_data(spectrum.db.data(), spectrum.db.size());
        drone_display_.set_dirty();
    }
}

} // namespace drone_analyzer
