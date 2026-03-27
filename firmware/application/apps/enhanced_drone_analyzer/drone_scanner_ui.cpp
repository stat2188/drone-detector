#include "drone_scanner_ui.hpp"
#include "ui.hpp"
#include "ui_fileman.hpp"
#include "scanner.hpp"
#include "scanner_thread.hpp"
#include "drone_settings.hpp"
#include "database.hpp"
#include "hardware_controller.hpp"
#include "audio_alerts.hpp"
#include "audio.hpp"
#include "constants.hpp"
#include "string_format.hpp"
#include "baseband_api.hpp"
#include "portapack_persistent_memory.hpp"
#include "portapack.hpp"
#include "file_path.hpp"
#include "radio.hpp"
#include "ch.h"
#include <array>

namespace drone_analyzer {

static HardwareController s_hardware;
static DatabaseManager s_database;
static DroneScanner s_scanner(s_database, s_hardware);
static ScannerThread s_scanner_thread(s_scanner);

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
                    if (this->composite_active_) {
                        this->on_sweep_spectrum(spectrum);
                        // Auto-exit sweep after pass completes (only in interleaved mode)
                        // sweep_pixel_index_ is reset to 0 at end of each pass
                        if (this->sweep_auto_mode_ && this->sweep_pixel_index_ == 0) {
                            this->exit_sweep_mode();
                        }
                    } else {
                        this->on_channel_spectrum(spectrum);
                        // Count DB scans, auto-trigger sweep after DB_SCANS_PER_SWEEP cycles
                        if (this->scanning_) {
                            this->db_scan_count_++;
                            if (this->db_scan_count_ >= this->DB_SCANS_PER_SWEEP) {
                                this->db_scan_count_ = 0;
                                this->sweep_auto_mode_ = true;
                                this->enter_sweep_mode();
                            }
                        }
                    }
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
    }
    , message_handler_channel_stats{
        Message::ID::ChannelStatistics,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelStatisticsMessage*>(p);
            this->latest_max_db_ = message.statistics.max_db;
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
        &button_median_,
        &button_start_stop_,
        &button_mode_,
        &button_load_,
        &button_settings_,
        &button_swp_
    });

    // Filter callback (Looking Glass style: OFF/MID/HIGH)
    field_filter_.on_change = [this](size_t, int32_t v) {
        min_color_power_ = static_cast<uint8_t>(v);
        drone_display_.set_spectrum_filter(min_color_power_);
    };
    field_filter_.set_by_value(DEFAULT_SPECTRUM_FILTER);

    // Median filter toggle (spike rejection on RSSI samples)
    button_median_.on_select = [this](ui::Button&) {
        median_enabled_ = !median_enabled_;
        if (scanner_ptr_ != nullptr) {
            scanner_ptr_->set_median_filter_enabled(median_enabled_);
        }
        button_median_.set_text(median_enabled_ ? "Md+" : "MED");
    };

    // Register button callbacks BEFORE any early returns
    // Buttons must always respond, even if init fails
    button_start_stop_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        if (scanning_) {
            // Stop
            if (composite_active_) {
                // Stop sweep streaming
                baseband::spectrum_streaming_stop();
                scanning_ = false;
                button_start_stop_.set_text("Start");
            } else {
                // Stop normal scanning
                scanner_thread_->set_scanning(false);
                (void)scanner_ptr_->stop_scanning();
                baseband::spectrum_streaming_stop();
                scanning_ = false;
                button_start_stop_.set_text("Start");
            }
        } else {
            // Start
            if (composite_active_) {
                // Start sweep streaming
                baseband::spectrum_streaming_start();
                scanning_ = true;
                button_start_stop_.set_text("Stop");
            } else {
                // Start normal scanning
                baseband::spectrum_streaming_start();
                (void)scanner_ptr_->start_scanning();
                scanner_thread_->set_scanning(true);
                scanning_ = true;
                button_start_stop_.set_text("Stop");
            }
        }
    };

    button_mode_.on_select = [this](ui::Button&) {
        if (!composite_active_) {
            // Enter manual sweep mode
            this->sweep_auto_mode_ = false;
            this->enter_sweep_mode();
            button_mode_.set_text("Sweep");
        } else {
            // Exit sweep mode back to normal DB scanning
            this->exit_sweep_mode();
            button_mode_.set_text("Mode");
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

    // Set default headphone volume to 70/99 if not already set higher
    if (portapack::receiver_model.normalized_headphone_volume() < 70) {
        portapack::receiver_model.set_normalized_headphone_volume(70);
    }

    // Initialize audio codec for beep playback (WM8731/AK4951)
    audio::set_rate(audio::Rate::Hz_24000);
    audio::output::start();

    scanner_thread_->start();

    scanning_ = false;

    scanner_ptr_->set_alert_callback([](ThreatLevel level) {
        AudioAlertManager::play_alert(level);
    });

    ScanConfig config;
    config.mode = scanning_mode_;
    config.rssi_threshold_dbm = RSSI_DETECTION_THRESHOLD_DBM;
    config.scan_interval_ms = SCAN_CYCLE_INTERVAL_MS;
    load_startup_settings(config);
    const ErrorCode config_err = scanner_ptr_->set_config(config);
    if (config_err != ErrorCode::SUCCESS) {
        show_error(config_err, ERROR_DURATION_MS);
    }
}

DroneScannerUI::~DroneScannerUI() noexcept {
    destruct_objects();

    audio::output::stop();
    portapack::receiver_model.disable();
    baseband::shutdown();
}

void DroneScannerUI::focus() {
    button_start_stop_.focus();
}

void DroneScannerUI::construct_objects() noexcept {
    hardware_ptr_ = &s_hardware;
    database_ptr_ = &s_database;
    scanner_ptr_ = &s_scanner;
    scanner_thread_ = &s_scanner_thread;
}

void DroneScannerUI::destruct_objects() noexcept {
    if (scanner_thread_ != nullptr) {
        scanner_thread_->stop();
        scanner_thread_ = nullptr;
    }
    scanner_ptr_ = nullptr;
    database_ptr_ = nullptr;
    hardware_ptr_ = nullptr;
}

void DroneScannerUI::on_show() {
    if (scanner_thread_ != nullptr && !scanner_thread_->is_active()) {
        scanner_thread_->start();
    }

    // If in sweep mode, reload sweep range from config (Settings may have changed it)
    if (composite_active_ && scanner_ptr_ != nullptr) {
        ScanConfig cfg = scanner_ptr_->get_config();
        sweep_f_min_ = cfg.sweep_start_freq;
        sweep_f_max_ = cfg.sweep_end_freq;

        // Validate: start must be < end
        if (sweep_f_min_ >= sweep_f_max_) {
            sweep_f_max_ = sweep_f_min_ + SWEEP_SLICE_BW;
        }

        // Recalculate LG-style parameters
        sweep_pixel_step_hz_ = (sweep_f_max_ - sweep_f_min_) / COMPOSITE_SIZE;
        sweep_step_hz_ = 244 * EACH_BIN_SIZE;
        sweep_f_center_ini_ = sweep_f_min_ - (2 * EACH_BIN_SIZE) + (SWEEP_SLICE_BW / 2);
        sweep_f_center_ = sweep_f_center_ini_;
        sweep_pixel_index_ = 0;
        sweep_pixel_max_ = 0;
        sweep_bins_hz_acc_ = 0;

        // Clear composite and update display
        for (uint16_t i = 0; i < COMPOSITE_SIZE; ++i) {
            composite_buffer_[i] = 0;
        }
        drone_display_.set_sweep_range(sweep_f_min_, sweep_f_max_);
        drone_display_.set_composite_data(composite_buffer_, COMPOSITE_SIZE);

        // Retune to start frequency (direct radio write, bypasses model)
        radio::set_tuning_frequency(rf::Frequency(sweep_f_center_));
        chThdSleepMilliseconds(5);
        current_frequency_ = sweep_f_center_;

        // Restart streaming (on_hide() stopped it when entering Settings)
        baseband::spectrum_streaming_start();
        scanning_ = true;
    }
}

void DroneScannerUI::on_hide() {
    if (scanning_) {
        scanner_thread_->set_scanning(false);
        if (scanner_ptr_ != nullptr) {
            (void)scanner_ptr_->stop_scanning();
        }
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

    // Remove stale drones (not seen for DRONE_STALE_TIMEOUT_MS)
    scanner_ptr_->remove_stale_drones(chTimeNow());

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

    // Stop looping SOS alert if no HIGH/CRITICAL threats remain
    // (MEDIUM uses one-shot alerts — don't kill those)
    if (AudioAlertManager::is_sos_looping()) {
        bool has_high_threat = false;
        for (size_t i = 0; i < display_data.drone_count; ++i) {
            if (display_data.drones[i].threat >= ThreatLevel::HIGH) {
                has_high_threat = true;
                break;
            }
        }
        if (!has_high_threat) {
            AudioAlertManager::stop_alert();
        }
    }

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

    // Drive SOS/multi-beep pattern at ~60Hz
    AudioAlertManager::update();

    set_dirty();
}

void DroneScannerUI::on_retune(FreqHz freq, uint32_t range) noexcept {
    (void)range;
    current_frequency_ = freq;
}

void DroneScannerUI::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    if (scanner_ptr_ != nullptr && scanning_) {
        // Capture frequency BEFORE processing — scanner thread may have moved
        // to the next frequency by the time this callback runs
        const FreqHz freq = scanner_ptr_->get_spectrum_frequency();
        if (freq != 0) {
            (void)scanner_ptr_->process_spectrum_message(spectrum, freq);
        }

        // Feed spectrum to DroneDisplay for visualization only when scanning
        drone_display_.set_spectrum_data(spectrum.db.data(), spectrum.db.size());
        drone_display_.set_dirty();
    }
}

void DroneScannerUI::enter_sweep_mode() noexcept {
    composite_active_ = true;
    drone_display_.set_composite_mode(true);

    // Count enabled zones
    enabled_zone_count_ = 0;
    for (uint8_t z = 0; z < SWEEP_ZONE_COUNT; ++z) {
        if (sweep_zones_config_.zones[z].enabled &&
            sweep_zones_config_.zones[z].start_freq < sweep_zones_config_.zones[z].end_freq) {
            enabled_zone_count_++;
        }
    }

    // Initialize all 4 zones
    for (uint8_t z = 0; z < SWEEP_ZONE_COUNT; ++z) {
        const auto& zone_cfg = sweep_zones_config_.zones[z];
        if (zone_cfg.enabled && zone_cfg.start_freq < zone_cfg.end_freq) {
            sweep_zones_runtime_[z].init(zone_cfg);
        }
        // Clear composite buffer for this zone
        for (uint16_t i = 0; i < COMPOSITE_SIZE; ++i) {
            composite_buffers_[z][i] = 0;
        }
    }

    // Start with first enabled zone
    active_zone_ = find_next_enabled_zone();

    // Stop scanner thread (UI drives tuning during sweep)
    if (scanner_thread_ != nullptr) {
        scanner_thread_->set_scanning(false);
    }
    if (scanner_ptr_ != nullptr) {
        (void)scanner_ptr_->stop_scanning();
    }

    // Wait for scanner thread to finish (avoid radio write race)
    chThdSleepMilliseconds(25);
    scanning_ = false;

    // Configure baseband for sweep bandwidth (LG pattern)
    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, 31);
    spectrum_fifo_ = nullptr;

    // Set display range (use zone 0 range as primary)
    drone_display_.set_sweep_range(
        sweep_zones_config_.zones[0].start_freq,
        sweep_zones_config_.zones[0].end_freq);

    // Tune to first active zone's start
    if (active_zone_ < SWEEP_ZONE_COUNT) {
        radio::set_tuning_frequency(rf::Frequency(sweep_zones_runtime_[active_zone_].current_center));
        chThdSleepMilliseconds(5);
        current_frequency_ = sweep_zones_runtime_[active_zone_].current_center;
    }

    // Start streaming
    baseband::spectrum_streaming_start();
    scanning_ = true;
    button_start_stop_.set_text("Stop");
}

void DroneScannerUI::exit_sweep_mode() noexcept {
    const bool was_auto = sweep_auto_mode_;
    composite_active_ = false;
    sweep_auto_mode_ = false;
    drone_display_.set_composite_mode(false);
    spectrum_fifo_ = nullptr;
    db_scan_count_ = 0;

    // Stop streaming
    if (scanning_) {
        baseband::spectrum_streaming_stop();
        scanning_ = false;
    }
    button_start_stop_.set_text("Start");

    // Restore normal bandwidth
    portapack::receiver_model.set_sampling_rate(DEFAULT_SAMPLE_RATE_HZ);
    portapack::receiver_model.set_baseband_bandwidth(DEFAULT_SAMPLE_RATE_HZ);
    baseband::set_spectrum(DEFAULT_SAMPLE_RATE_HZ, 31);

    // In auto mode, immediately resume DB scanning
    // In manual mode, stay stopped — user presses Start
    if (was_auto) {
        if (scanner_thread_ != nullptr) {
            scanner_thread_->set_scanning(true);
        }
        if (scanner_ptr_ != nullptr) {
            (void)scanner_ptr_->start_scanning();
        }
        baseband::spectrum_streaming_start();
        scanning_ = true;
        button_start_stop_.set_text("Stop");
    }
}

void DroneScannerUI::on_sweep_spectrum(const ChannelSpectrum& spectrum) noexcept {
    // Multi-zone sweep: process spectrum for current active zone
    baseband::spectrum_streaming_stop();

    if (active_zone_ >= SWEEP_ZONE_COUNT) {
        active_zone_ = find_next_enabled_zone();
        multi_zone_sweep_retune();
        return;
    }

    SweepZoneRuntime& rt = sweep_zones_runtime_[active_zone_];

    // Bin-to-pixel mapping (LG FASTSCAN pattern)
    for (uint8_t bin = 0; bin < 240; ++bin) {
        const uint8_t fft_bin = (bin < 120) ? (134 + bin) : (bin - 118);
        const uint8_t power = spectrum.db[fft_bin];

        if (power > rt.pixel_max) rt.pixel_max = power;

        rt.bins_hz_acc += EACH_BIN_SIZE;
        while (rt.bins_hz_acc >= rt.pixel_step_hz && rt.pixel_index < COMPOSITE_SIZE) {
            if (rt.pixel_max > composite_buffers_[active_zone_][rt.pixel_index]) {
                composite_buffers_[active_zone_][rt.pixel_index] = rt.pixel_max;
            }
            rt.pixel_index++;
            rt.pixel_max = 0;
            rt.bins_hz_acc -= rt.pixel_step_hz;
        }
    }

    // Drone detection on current zone
    if (scanner_ptr_ != nullptr) {
        scanner_ptr_->process_spectrum_sweep(spectrum, rt.current_center);
    }

    // Check if this zone's pass is complete
    if (rt.is_complete()) {
        // Display all zones with frequency labels
        FreqHz freq_starts[SWEEP_ZONE_COUNT];
        FreqHz freq_ends[SWEEP_ZONE_COUNT];
        for (uint8_t z = 0; z < SWEEP_ZONE_COUNT; ++z) {
            freq_starts[z] = sweep_zones_config_.zones[z].start_freq;
            freq_ends[z] = sweep_zones_config_.zones[z].end_freq;
        }
        drone_display_.set_multi_zone_data(composite_buffers_, SWEEP_ZONE_COUNT, COMPOSITE_SIZE,
                                           freq_starts, freq_ends);
        drone_display_.set_dirty();

        // Reset this zone for next pass
        rt.reset_pass();

        // Move to next enabled zone
        active_zone_ = find_next_enabled_zone();
        if (active_zone_ >= SWEEP_ZONE_COUNT) {
            // All zones complete — wrap to first
            active_zone_ = find_next_enabled_zone();
        }
    }

    // Advance to next slice
    multi_zone_sweep_retune();
}

uint8_t DroneScannerUI::find_next_enabled_zone() noexcept {
    for (uint8_t i = 1; i <= SWEEP_ZONE_COUNT; ++i) {
        uint8_t candidate = (active_zone_ + i) % SWEEP_ZONE_COUNT;
        if (sweep_zones_config_.zones[candidate].enabled) {
            return candidate;
        }
    }
    return active_zone_;  // fallback: stay on current
}

void DroneScannerUI::multi_zone_sweep_retune() noexcept {
    if (active_zone_ >= SWEEP_ZONE_COUNT) return;

    SweepZoneRuntime& rt = sweep_zones_runtime_[active_zone_];
    rt.current_center += sweep_zones_runtime_[active_zone_].step_hz;

    // Check if we've swept past the zone's end frequency
    const auto& zone_cfg = sweep_zones_config_.zones[active_zone_];
    if (rt.current_center > zone_cfg.end_freq + SWEEP_SLICE_BW) {
        // Wrap back to start of this zone
        rt.current_center = rt.center_ini;
    }

    radio::set_tuning_frequency(rf::Frequency(rt.current_center));
    chThdSleepMilliseconds(5);  // PLL settle
    current_frequency_ = rt.current_center;
    baseband::spectrum_streaming_start();
}

} // namespace drone_analyzer
