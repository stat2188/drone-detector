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
    , big_display_{{BIG_FREQUENCY_X, BIG_FREQUENCY_Y, BIG_FREQUENCY_WIDTH, 52}, 0}
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
                    if (this->composite_active_ && this->scanning_) {
                        this->update_composite(
                            static_cast<FreqHz>(this->current_frequency_), spectrum);
                    }
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
        &filter_labels_,
        &field_filter_,
        &button_start_stop_,
        &button_mode_,
        &button_load_,
        &button_settings_
    });

    // Filter callback (Looking Glass style: OFF/MID/HIGH)
    field_filter_.on_change = [this](size_t, int32_t v) {
        min_color_power_ = static_cast<uint8_t>(v);
        drone_display_.set_spectrum_filter(min_color_power_);
    };
    field_filter_.set_by_value(DEFAULT_SPECTRUM_FILTER);

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
        composite_active_ = !composite_active_;
        if (composite_active_) {
            // Use sweep range from ScanConfig
            if (scanner_ptr_ != nullptr) {
                const ScanConfig cfg = scanner_ptr_->get_config();
                sweep_display_start_ = cfg.sweep_start_freq;
                sweep_display_end_ = cfg.sweep_end_freq;
            } else {
                sweep_display_start_ = SWEEP_DEFAULT_START_HZ;
                sweep_display_end_ = SWEEP_DEFAULT_END_HZ;
            }
            sweep_display_range_ = sweep_display_end_ - sweep_display_start_;

            if (sweep_display_range_ == 0 || sweep_display_range_ > 10000000000ULL) {
                composite_active_ = false;
                show_alert("Bad sweep range", 2000);
                return;
            }

            FreqHz step = sweep_display_range_ / COMPOSITE_SIZE;
            if (step < 100000) step = 100000;  // Min 100 kHz step

            // Use configured step if available
            if (scanner_ptr_ != nullptr) {
                const ScanConfig cfg = scanner_ptr_->get_config();
                // Step can be overridden from settings (stored temporarily in histogram fields)
                // For now, use auto-calculated step
                (void)cfg;
            }

            clear_composite();
            sweep_frame_count_ = 0;
            if (scanner_thread_ != nullptr) {
                scanner_thread_->set_sweep_range(
                    sweep_display_start_, sweep_display_end_, step);
                scanner_thread_->set_sweep_enabled(true);
            }
            drone_display_.set_composite_mode(true);
            button_mode_.set_text("Spec");
        } else {
            if (scanner_thread_ != nullptr) {
                scanner_thread_->set_sweep_enabled(false);
            }
            drone_display_.set_composite_mode(false);
            button_mode_.set_text("Mode");
            show_alert("Scanner mode", 1000);
        }
    };

    button_load_.on_select = [this](ui::Button&) {
        auto open_view = nav_.push<FileLoadView>(".TXT");
        open_view->push_dir(freqman_dir);
        open_view->on_changed = [this](std::filesystem::path new_file_path) {
            const bool was_scanning = scanning_;

            // Stop scanning if active
            if (scanning_) {
                scanner_thread_->set_scanning(false);
                (void)scanner_ptr_->stop_scanning();
                baseband::spectrum_streaming_stop();
                scanning_ = false;
                button_start_stop_.set_text("Start");
            }

            // Extract filename stem from path (avoid std::string heap)
            // Path format: "FREQMAN/DRONES.TXT" → stem = "DRONES"
            char filename[32];
            const auto path_str = new_file_path.string();
            size_t last_slash = 0;
            size_t last_dot = 0;
            for (size_t j = 0; path_str[j] != '\0'; ++j) {
                if (path_str[j] == '/' || path_str[j] == '\\') last_slash = j + 1;
                if (path_str[j] == '.') last_dot = j;
            }
            size_t i = 0;
            const size_t stem_start = last_slash;
            const size_t stem_end = (last_dot > last_slash) ? last_dot : last_slash + 31;
            for (; i < 31 && (stem_start + i) < stem_end && path_str[stem_start + i] != '\0'; ++i) {
                filename[i] = path_str[stem_start + i];
            }
            filename[i] = '\0';

            // Set new database file and reload
            database_ptr_->set_database_file(filename);
            scanner_ptr_->clear_tracked_drones();

            const ErrorCode err = database_ptr_->load_frequency_database();
            db_loaded_ = (err == ErrorCode::SUCCESS);
            db_entry_count_ = database_ptr_->get_database_size();

            if (err == ErrorCode::SUCCESS && db_entry_count_ > 0) {
                // Build status message with filename and entry count
                static char status_buf[MAX_TEXT_LENGTH];
                snprintf(status_buf, sizeof(status_buf), "%s (%lu)",
                         filename, (unsigned long)db_entry_count_);
                drone_display_.set_status_text(status_buf);

                // Show alert with confirmation
                static char msg[MAX_TEXT_LENGTH];
                snprintf(msg, sizeof(msg), "DB: %s loaded", filename);
                show_alert(msg, 2000);

                // Reset scanner frequency to first entry in new database
                scanner_ptr_->reset_frequency();

                // Resume scanning if it was active before
                if (was_scanning) {
                    baseband::spectrum_streaming_start();
                    (void)scanner_ptr_->start_scanning();
                    scanner_thread_->set_scanning(true);
                    scanning_ = true;
                    button_start_stop_.set_text("Stop");
                }

                // Force UI refresh
                drone_display_.set_dirty();
                set_dirty();
            } else {
                // Show error with filename for debugging
                static char err_msg[MAX_TEXT_LENGTH];
                snprintf(err_msg, sizeof(err_msg), "Failed: %s (err %d)",
                         filename, static_cast<int>(err));
                show_alert(err_msg, 3000);

                // Show error in status
                drone_display_.set_status_text("DB load error");
                drone_display_.set_dirty();
                set_dirty();
            }
        };
    };

    button_settings_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        ScanConfig config = scanner_ptr_->get_config();
        nav_.push<DroneSettingsView>(config, scanner_ptr_, &drone_display_);
    };

    // Hardware initialization (callbacks are already set, safe to early-return)
    construct_objects();

    if (scanner_ptr_ == nullptr) {
        show_error(ErrorCode::INITIALIZATION_FAILED, ERROR_DURATION_MS);
        initialization_failed_ = true;
        return;
    }

    // Database load — SD card is already mounted at boot (portapack.cpp:569)
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
    portapack::receiver_model.set_sampling_rate(DEFAULT_SAMPLE_RATE_HZ);
    portapack::receiver_model.set_baseband_bandwidth(DEFAULT_SAMPLE_RATE_HZ);
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
        // Clear stale entries beyond current count
        for (size_t i = count; i < MAX_DISPLAYED_DRONES; ++i) {
            display_data.drones[i] = DisplayDroneEntry();
        }

        // Sort by threat level descending (CRITICAL first, NONE last)
        // Simple insertion sort — O(n²) but n ≤ MAX_DISPLAYED_DRONES (small)
        for (size_t i = 1; i < count; ++i) {
            const DisplayDroneEntry key = display_data.drones[i];
            size_t j = i;
            while (j > 0 && display_data.drones[j - 1].threat < key.threat) {
                display_data.drones[j] = display_data.drones[j - 1];
                --j;
            }
            display_data.drones[j] = key;
        }
    }

    // Update status text based on state and database status
    // Always show database entry count so user can verify DB changed after Load
    static char status_buf[MAX_TEXT_LENGTH];

    if (!db_loaded_) {
        drone_display_.set_status_text("No DB");
    } else if (db_entry_count_ == 0) {
        drone_display_.set_status_text("DB empty");
    } else {
        switch (current_scanner_state_) {
            case ScannerState::SCANNING:
                snprintf(status_buf, sizeof(status_buf), "Scan (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(status_buf);
                break;
            case ScannerState::LOCKING:
                snprintf(status_buf, sizeof(status_buf), "Lock (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(status_buf);
                break;
            case ScannerState::TRACKING:
                snprintf(status_buf, sizeof(status_buf), "Track %lu (%lu)",
                         (unsigned long)display_data.drone_count, (unsigned long)db_entry_count_);
                drone_display_.set_status_text(status_buf);
                break;
            case ScannerState::PAUSED:
                snprintf(status_buf, sizeof(status_buf), "Pause (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(status_buf);
                break;
            default:
                snprintf(status_buf, sizeof(status_buf), "Ready (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(status_buf);
                break;
        }
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

void DroneScannerUI::on_retune(FreqHz freq, uint32_t range) noexcept {
    (void)range;
    current_frequency_ = freq;
}

void DroneScannerUI::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    if (scanner_ptr_ != nullptr && scanning_) {
        (void)scanner_ptr_->process_spectrum_message(spectrum);

        // Feed spectrum to DroneDisplay for visualization only when scanning
        drone_display_.set_spectrum_data(spectrum.db.data(), spectrum.db.size());
        drone_display_.set_dirty();
    }
}

void DroneScannerUI::update_composite(FreqHz center_freq, const ChannelSpectrum& spectrum) noexcept {
    if (sweep_display_range_ == 0) return;

    constexpr FreqHz half_bw = 1000000;      // 1 MHz from center to edge
    constexpr FreqHz bin_bw = 2000000 / 256;  // ~7812 Hz per bin (2MHz baseband)
    FreqHz frame_start = center_freq - half_bw;

    for (size_t bin = 0; bin < 256; ++bin) {
        // Skip DC spike bins
        if (bin >= 120 && bin < 136) continue;

        uint8_t power = spectrum.db[bin];
        if (power == 0) continue;

        FreqHz bin_freq = frame_start + bin * bin_bw;
        if (bin_freq < sweep_display_start_ || bin_freq >= sweep_display_end_) continue;

        // Map frequency to pixel
        uint32_t pixel = static_cast<uint32_t>(
            ((bin_freq - sweep_display_start_) * COMPOSITE_SIZE) / sweep_display_range_);
        if (pixel >= COMPOSITE_SIZE) continue;

        // Max accumulation
        if (power > composite_buffer_[pixel]) {
            composite_buffer_[pixel] = power;
        }
    }

    // Pass to display for rendering
    drone_display_.set_composite_data(composite_buffer_, COMPOSITE_SIZE);
    drone_display_.set_dirty();
}

void DroneScannerUI::clear_composite() noexcept {
    for (uint16_t i = 0; i < COMPOSITE_SIZE; ++i) {
        composite_buffer_[i] = 0;
    }
    drone_display_.set_composite_data(composite_buffer_, COMPOSITE_SIZE);
}

} // namespace drone_analyzer
