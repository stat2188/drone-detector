#include <cstdint>
#include <cstring>
#include <cstdio>
#include <array>

#include "ui.hpp"
#include "ui_fileman.hpp"
#include "scanner.hpp"
#include "scanner_thread.hpp"
#include "drone_settings.hpp"
#include "settings_manager.hpp"
#include "drone_sweep_view.hpp"
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

#include "drone_scanner_ui.hpp"

namespace drone_analyzer {

static HardwareController s_hardware;
static DatabaseManager s_database;
static DroneScanner s_scanner(s_database, s_hardware);
static ScannerThread s_scanner_thread(s_scanner);

DroneScannerUI::DroneScannerUI(NavigationView& nav) noexcept
    : View()
    , nav_(nav)
    , big_display_{{BIG_FREQUENCY_X, BIG_FREQUENCY_Y, BIG_FREQUENCY_WIDTH, 52}, 0}
    , pattern_capture_state_(PatternCaptureState::IDLE)
    , pattern_select_start_(0)
    , pattern_select_end_(0)
    , pattern_capture_frames_(0)
    , pattern_waveform_sum_{}
    , pattern_capture_freq_{0}
    , pattern_capture_rssi_(0)
    , pattern_match_counter_(0)
    , fifo_state_{}
    , sweep_transition_guard_()
    , message_handler_spectrum_config{
        Message::ID::ChannelSpectrumConfig,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);

            this->safe_set_fifo(message.fifo);

            this->spectrum_fifo_ = message.fifo;
        }}
    , message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](const Message* const p) {
            (void)p;
            if (!this->scanning_) return;

            if (this->spectrum_fifo_ != nullptr) {
                ChannelSpectrum spectrum;
                if (this->spectrum_fifo_->out(spectrum)) {
                    if (this->composite_active_) {
                        this->on_sweep_spectrum(spectrum);
                    } else {
                        this->on_channel_spectrum(spectrum);
                        this->db_scan_count_++;
                        if (this->db_scan_count_ >= this->DB_SCANS_PER_SWEEP) {
                            this->db_scan_count_ = 0;
                            this->sweep_auto_mode_ = true;
                            this->enter_sweep_mode();
                        }
                    }

                    this->fifo_state_.mark_access(chTimeNow());
                }
            }
            this->refresh_ui();
        }}
    , message_handler_retune{
        Message::ID::Retune,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const RetuneMessage*>(p);
            this->on_retune(message.freq, message.range);
        }}
    , message_handler_channel_stats{
        Message::ID::ChannelStatistics,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const ChannelStatisticsMessage*>(p);
            this->latest_max_db_ = message.statistics.max_db;
        }}
    , add_children({
        &labels_,
        &field_lna_,
        &field_vga_,
        &field_rf_amp_,
        &field_volume_,
        &labels_cyc_,
        &field_rssi_dec_cyc_,
        &big_display_,
        &drone_display_,
        &filter_labels_,
        &field_filter_,
        &button_median_,
        &button_start_stop_,
        &button_mode_,
        &button_load_,
        &button_settings_,
        &button_swp_,
        &button_ptr_
    })
{
    // Filter callback (Looking Glass style: OFF/MID/HIGH)
    field_filter_.on_change = [this](size_t, int32_t v) {
        min_color_power_ = static_cast<uint8_t>(v);
        drone_display_.set_spectrum_filter(min_color_power_);
    };
    field_filter_.set_by_value(DEFAULT_SPECTRUM_FILTER);

    // Sync RSSI decrease cycles from scanner config to UI field
    // NOTE: spectrum shape params NOT synced here to preserve display_margin_=0 default
    // (display and detection filtering are now separate)
    if (scanner_ptr_ != nullptr) {
        const ScanConfig cfg = scanner_ptr_->get_config();
        field_rssi_dec_cyc_.set_value(static_cast<int32_t>(cfg.rssi_decrease_cycles));
    }

    // Median filter toggle (spike rejection on RSSI samples)
    button_median_.on_select = [this](ui::Button&) {
        median_enabled_ = !median_enabled_;
        if (scanner_ptr_ != nullptr) {
            scanner_ptr_->set_median_filter_enabled(median_enabled_);
        }
        button_median_.set_text(median_enabled_ ? "Md+" : "OFF");
        // Persist to SD card so state survives app restart
        SettingsStruct persist_settings;
        (void)SettingsFileManager::load(persist_settings);
        persist_settings.median_enabled = median_enabled_;
        (void)SettingsFileManager::save(scanner_ptr_, persist_settings);
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
            // Enter manual sweep mode (enter_sweep_mode handles re-entrancy guard)
            this->sweep_auto_mode_ = false;
            this->enter_sweep_mode();
            if (composite_active_) {
                button_mode_.set_text("Sweep");
            }
        } else {
            // Exit sweep mode back to normal DB scanning
            this->exit_sweep_mode();
            button_mode_.set_text("Mode");
        }
    };

    button_load_.on_select = [this](ui::Button&) {
        auto open_view = nav_.push<FileLoadView>(".TXT");
        open_view->push_dir(freqman_dir);
        open_view->on_changed = [this](const std::filesystem::path& new_file_path) {
            const bool was_scanning = scanning_;

            // Stop scanning if active
            if (scanning_) {
                scanner_thread_->set_scanning(false);
                (void)scanner_ptr_->stop_scanning();
                baseband::spectrum_streaming_stop();
                scanning_ = false;
                button_start_stop_.set_text("Start");
            }

            // Extract filename stem from path — iterate native() chars directly
            // Avoids path::string() which allocates std::string on heap
            // native() returns const std::u16string& (reference, no copy)
            char filename[32];
            const auto& native_path = new_file_path.native();
            size_t last_slash = 0;
            size_t last_dot = 0;
            for (size_t j = 0; j < native_path.size(); ++j) {
                const char c = static_cast<char>(native_path[j]);
                if (c == '/' || c == '\\') last_slash = j + 1;
                if (c == '.') last_dot = j;
            }
            size_t i = 0;
            const size_t stem_start = last_slash;
            const size_t stem_end = (last_dot > last_slash) ? last_dot : last_slash + 31;
            for (; i < 31 && (stem_start + i) < stem_end && (stem_start + i) < native_path.size(); ++i) {
                filename[i] = static_cast<char>(native_path[stem_start + i]);
            }
            filename[i] = '\0';

            // Set new database file and reload
            database_ptr_->set_database_file(filename);
            scanner_ptr_->clear_tracked_drones();

            const ErrorCode err = database_ptr_->load_frequency_database();
            db_loaded_ = (err == ErrorCode::SUCCESS);
            db_entry_count_ = database_ptr_->get_database_size();

            if (err == ErrorCode::SUCCESS && db_entry_count_ > 0) {
                char status_buf[MAX_TEXT_LENGTH];
                snprintf(status_buf, sizeof(status_buf), "%s (%lu)",
                         filename, (unsigned long)db_entry_count_);
                drone_display_.set_status_text(status_buf);

                char alert_buf[MAX_TEXT_LENGTH];
                snprintf(alert_buf, sizeof(alert_buf), "DB: %s loaded", filename);
                show_alert(alert_buf, 2000);

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
                char err_buf[MAX_TEXT_LENGTH];
                snprintf(err_buf, sizeof(err_buf), "Failed: %s (err %d)",
                         filename, static_cast<int>(err));
                show_alert(err_buf, 3000);

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
        // Refresh config from scanner (SWP view may have changed sweep settings)
        ScanConfig config = scanner_ptr_->get_config();
        nav_.push<DroneSettingsView>(config, scanner_ptr_, &drone_display_);
    };

    // SWP button: open sweep settings view
    button_swp_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        ScanConfig config = scanner_ptr_->get_config();
        nav_.push<DroneSweepView>(config, scanner_ptr_);
    };
    
    // PTR button: toggle pattern capture mode
    button_ptr_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        on_pattern_capture_toggle();
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
    baseband::set_spectrum(DEFAULT_SAMPLE_RATE_HZ, SWEEP_FFT_TRIGGER);
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

    // Load all settings from SD card via centralized manager
    SettingsStruct startup_settings;
    (void)SettingsFileManager::load(startup_settings);
    SettingsFileManager::apply_to_config(startup_settings, config);

    const ErrorCode config_err = scanner_ptr_->set_config(config);
    if (config_err != ErrorCode::SUCCESS) {
        show_error(config_err, ERROR_DURATION_MS);
    }

    // Sync UI median_enabled from loaded config
    median_enabled_ = config.median_enabled;
    button_median_.set_text(median_enabled_ ? "Md+" : "OFF");
}

DroneScannerUI::~DroneScannerUI() noexcept {
    destruct_objects();

    audio::output::stop();
    portapack::receiver_model.disable();
    baseband::shutdown();
}

void DroneScannerUI::safe_clear_fifo() noexcept {
    if (!fifo_state_.fifo_changing_.try_set()) {
        return;
    }
    
    SystemTime now = chTimeNow();
    
    if (!fifo_state_.is_safe_to_clear(now, 100)) {
        fifo_state_.fifo_changing_.clear();
        return;
    }
    
    fifo_state_.fifo = nullptr;
    fifo_state_.fifo_active_.clear();
    fifo_state_.fifo_changing_.clear();
}

void DroneScannerUI::safe_set_fifo(ChannelSpectrumFIFO* fifo) noexcept {
    if (!fifo_state_.fifo_changing_.try_set()) {
        return;
    }
    
    SystemTime now = chTimeNow();
    
    ChannelSpectrumFIFO* old_fifo = fifo_state_.fifo;
    fifo_state_.fifo = fifo;
    fifo_state_.mark_access(now);
    
    if (fifo != nullptr) {
        fifo_state_.fifo_active_.set();
    } else {
        fifo_state_.fifo_active_.clear();
    }
    
    fifo_state_.fifo_changing_.clear();
}

bool DroneScannerUI::verify_baseband_stopped(SystemTime now, uint32_t timeout_ms) const noexcept {
    if (scanning_) {
        return false;
    }
    
    if (now < fifo_state_.last_access_time_ms) {
        return (UINT32_MAX - fifo_state_.last_access_time_ms + now) >= timeout_ms;
    }
    
    const uint32_t elapsed_ms = now - fifo_state_.last_access_time_ms;
    return elapsed_ms >= timeout_ms;
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

        // Reinit all windows from config
        last_tuned_freq_ = 0;
        skip_next_fft_ = true;
        sweep_[0].init(cfg.sweep_start_freq, cfg.sweep_end_freq, cfg.sweep_step_freq);
        sweep_[0].enabled = true;  // Window 0 always enabled
        sweep_[1].init(cfg.sweep2_start_freq, cfg.sweep2_end_freq, cfg.sweep2_step_freq);
        sweep_[1].enabled = cfg.sweep2_enabled;
        sweep_[2].init(cfg.sweep3_start_freq, cfg.sweep3_end_freq, cfg.sweep3_step_freq);
        sweep_[2].enabled = cfg.sweep3_enabled;
        sweep_[3].init(cfg.sweep4_start_freq, cfg.sweep4_end_freq, cfg.sweep4_step_freq);
        sweep_[3].enabled = cfg.sweep4_enabled;

        // Copy exception frequencies to each sweep window
        const FreqHz on_show_exc_radius = static_cast<FreqHz>(cfg.exception_radius_mhz) * 1000000ULL;
        for (uint8_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
            for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
                sweep_[w].exceptions[i] = cfg.sweep_exceptions[w][i];
            }
            sweep_[w].exception_radius_hz = on_show_exc_radius;
        }

        // Find first enabled window
        active_sweep_idx_ = 0;
        for (uint8_t i = 0; i < MAX_SWEEP_WINDOWS; ++i) {
            if (sweep_[i].enabled) {
                active_sweep_idx_ = i;
                break;
            }
        }

        current_pair_ = pair_first(active_sweep_idx_);
        update_sweep_pair_display();

        radio::set_tuning_frequency(rf::Frequency(sweep_[active_sweep_idx_].f_center));
        current_frequency_ = sweep_[active_sweep_idx_].f_center;

        if (!scanning_) {
            baseband::spectrum_streaming_start();
            scanning_ = true;
        }
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

    // Remove stale drones (not seen for DRONE_REMOVAL_TIMEOUT_MS)
    // Works in both normal and sweep mode
    scanner_ptr_->remove_stale_drones(chTimeNow());

    // Build display data from tracked drones
    // Use single get_tracked_drones call for consistency (one lock, one snapshot)
    refresh_display_data_.clear();

    {
        const size_t count = scanner_ptr_->get_tracked_drones(refresh_drones_, MAX_DISPLAYED_DRONES);
        refresh_display_data_.drone_count = count;
        for (size_t i = 0; i < count; ++i) {
            refresh_display_data_.drones[i] = DisplayDroneEntry(refresh_drones_[i]);
        }
        // Clear stale entries beyond current count
        for (size_t i = count; i < MAX_DISPLAYED_DRONES; ++i) {
            refresh_display_data_.drones[i] = DisplayDroneEntry();
        }

        // Sort by threat level descending (CRITICAL first, NONE last)
        // Simple insertion sort — O(n²) but n ≤ MAX_DISPLAYED_DRONES (small)
        for (size_t i = 1; i < count; ++i) {
            const DisplayDroneEntry key = refresh_display_data_.drones[i];
            size_t j = i;
            while (j > 0 && refresh_display_data_.drones[j - 1].threat < key.threat) {
                refresh_display_data_.drones[j] = refresh_display_data_.drones[j - 1];
                --j;
            }
            refresh_display_data_.drones[j] = key;
        }
    }

    // Update status text based on state and database status
    // Always show database entry count so user can verify DB changed after Load

    if (!db_loaded_) {
        drone_display_.set_status_text("No DB");
    } else if (db_entry_count_ == 0) {
        drone_display_.set_status_text("DB empty");
    } else {
        switch (current_scanner_state_) {
            case ScannerState::SCANNING:
                snprintf(refresh_status_buf_, sizeof(refresh_status_buf_), "Scan (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(refresh_status_buf_);
                break;
            case ScannerState::LOCKING:
                snprintf(refresh_status_buf_, sizeof(refresh_status_buf_), "Lock (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(refresh_status_buf_);
                break;
            case ScannerState::TRACKING:
                snprintf(refresh_status_buf_, sizeof(refresh_status_buf_), "Track %lu (%lu)",
                         (unsigned long)refresh_display_data_.drone_count, (unsigned long)db_entry_count_);
                drone_display_.set_status_text(refresh_status_buf_);
                break;
            case ScannerState::PAUSED:
                snprintf(refresh_status_buf_, sizeof(refresh_status_buf_), "Pause (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(refresh_status_buf_);
                break;
            default:
                snprintf(refresh_status_buf_, sizeof(refresh_status_buf_), "Ready (%lu)", (unsigned long)db_entry_count_);
                drone_display_.set_status_text(refresh_status_buf_);
                break;
        }
    }

    drone_display_.update_display_data(refresh_display_data_);

    // Stop looping SOS alert if no HIGH/CRITICAL threats remain
    // (MEDIUM uses one-shot alerts — don't kill those)
    if (AudioAlertManager::is_sos_looping()) {
        bool has_high_threat = false;
        for (size_t i = 0; i < refresh_display_data_.drone_count; ++i) {
            if (refresh_display_data_.drones[i].threat >= ThreatLevel::HIGH) {
                has_high_threat = true;
                break;
            }
        }
        if (!has_high_threat) {
            AudioAlertManager::stop_alert();
        }
    }

    // Feed histogram data
    // Normal mode: from scanner histogram processor
    // Sweep mode: from composite buffer (live power distribution)
    if (!composite_active_) {
        const size_t hist_count = scanner_ptr_->get_histogram_snapshot(
            refresh_hist_data_, HISTOGRAM_BUFFER_SIZE
        );
        if (hist_count > 0) {
            drone_display_.set_histogram_data(refresh_hist_data_, hist_count);
        }
    } else {
        // Feed displayed pair's composite power levels as histogram bins
        const uint8_t w0 = pair_first(active_sweep_idx_);
        const uint8_t w1 = w0 + 1;
        size_t hist_idx = 0;

        if (sweep_[w0].enabled) {
            const size_t n0 = (COMPOSITE_SIZE < HISTOGRAM_BUFFER_SIZE)
                ? COMPOSITE_SIZE : HISTOGRAM_BUFFER_SIZE;
            for (size_t i = 0; i < n0 && hist_idx < HISTOGRAM_BUFFER_SIZE; ++i) {
                refresh_hist_data_[hist_idx++] = static_cast<uint16_t>(sweep_[w0].composite[i]) * COMPOSITE_TO_HIST_SCALE;
            }
        }
        if (w1 < MAX_SWEEP_WINDOWS && sweep_[w1].enabled) {
            for (size_t i = 0; i < COMPOSITE_SIZE && hist_idx < HISTOGRAM_BUFFER_SIZE; ++i) {
                refresh_hist_data_[hist_idx++] = static_cast<uint16_t>(sweep_[w1].composite[i]) * COMPOSITE_TO_HIST_SCALE;
            }
        }
        if (hist_idx > 0) {
            drone_display_.set_histogram_data(refresh_hist_data_, hist_idx);
        }
    }

    // Update big frequency display
    {
        RssiValue drone_rssi = RSSI_NOISE_FLOOR_DBM;
        if (refresh_display_data_.drone_count > 0) {
            drone_rssi = refresh_display_data_.drones[0].rssi;
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
    }

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

bool DroneScannerUI::on_touch(const ui::TouchEvent event) noexcept {
    if (pattern_capture_state_ != PatternCaptureState::SELECTING) {
        return false;  // Let parent handle touch events
    }
    
    // Only process Start events (user taps screen)
    if (event.type != ui::TouchEvent::Type::Start) {
        return false;
    }
    
    // Check if touch is within spectrum display area
    // Spectrum display: x=0-239, y=68-273 (from DroneDisplay rect)
    const uint16_t display_x = event.point.x();
    const uint16_t display_y = event.point.y();
    
    if (display_x < 240 && display_y >= 68 && display_y < 274) {
        // Convert screen X to composite pixel (0-239)
        const uint16_t pixel_x = display_x;
        on_touch_spectrum(pixel_x);
        return true;  // Event handled
    }
    
    return false;  // Let parent handle touch events outside spectrum area
}

void DroneScannerUI::on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept {
    if (scanner_ptr_ != nullptr && scanning_) {
        // Use frequency from on_retune() — this matches the hardware tuning
        // when this spectrum was captured. Do NOT read scanner's current_frequency_
        // because the scanner thread may have already moved to the next frequency.
        const FreqHz freq = current_frequency_;
        if (freq != 0) {
            (void)scanner_ptr_->process_spectrum_message(spectrum, freq);
        }

        // Feed spectrum to DroneDisplay for visualization only when scanning
        drone_display_.set_spectrum_data(spectrum.db.data(), spectrum.db.size());
        drone_display_.set_dirty();
    }
}

void DroneScannerUI::enter_sweep_mode() noexcept {
    // Prevent re-entrant entry (double-tap on Mode button)
    if (composite_active_) return;
    if (!sweep_transition_guard_.try_set()) return;

    composite_active_ = true;
    last_tuned_freq_ = 0;
    skip_next_fft_ = true;  // first FFT after entry may be stale
    drone_display_.set_composite_mode(true);

    ScanConfig cfg;
    if (scanner_ptr_ != nullptr) {
        cfg = scanner_ptr_->get_config();
    }

    // Initialize all 4 sweep windows from config
    sweep_[0].init(cfg.sweep_start_freq, cfg.sweep_end_freq, cfg.sweep_step_freq);
    sweep_[0].enabled = true;  // Window 0 always enabled
    sweep_[1].init(cfg.sweep2_start_freq, cfg.sweep2_end_freq, cfg.sweep2_step_freq);
    sweep_[1].enabled = cfg.sweep2_enabled;
    sweep_[2].init(cfg.sweep3_start_freq, cfg.sweep3_end_freq, cfg.sweep3_step_freq);
    sweep_[2].enabled = cfg.sweep3_enabled;
    sweep_[3].init(cfg.sweep4_start_freq, cfg.sweep4_end_freq, cfg.sweep4_step_freq);
    sweep_[3].enabled = cfg.sweep4_enabled;

    // Copy exception frequencies to each sweep window
    const FreqHz exc_radius_hz = static_cast<FreqHz>(cfg.exception_radius_mhz) * 1000000ULL;
    for (uint8_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            sweep_[w].exceptions[i] = cfg.sweep_exceptions[w][i];
        }
        sweep_[w].exception_radius_hz = exc_radius_hz;
    }

    // Find first enabled window for round-robin
    active_sweep_idx_ = MAX_SWEEP_WINDOWS;  // invalid sentinel
    for (uint8_t i = 0; i < MAX_SWEEP_WINDOWS; ++i) {
        if (sweep_[i].enabled) {
            active_sweep_idx_ = i;
            break;
        }
    }
    if (active_sweep_idx_ >= MAX_SWEEP_WINDOWS) {
        // No enabled windows — abort sweep, return to normal mode
        composite_active_ = false;
        drone_display_.set_composite_mode(false);
        sweep_transition_guard_.clear();
        return;
    }

    // Initialize pair tracking (pairs: 0=[w0,w1], 2=[w2,w3])
    current_pair_ = pair_first(active_sweep_idx_);

    // Save DB index BEFORE stopping scanner, then derive frequency from DB entry.
    // This ensures last_db_frequency_ == entries[last_db_index_].frequency,
    // so get_next_frequency() finds the exact resume point after sweep.
    if (database_ptr_ != nullptr) {
        last_db_index_ = database_ptr_->get_current_index();
        // Use current frequency from scanner (thread-safe via atomic read)
        // This is the frequency the scanner was tuned to before sweep entry
        if (current_frequency_ != 0) {
            last_db_frequency_ = current_frequency_;
        } else {
            // Fallback: try to get from scanner's locked frequency
            last_db_frequency_ = scanner_ptr_->get_locked_frequency();
        }
    }

    // Stop scanner thread FIRST — UI drives tuning during sweep
    if (scanner_thread_ != nullptr) {
        scanner_thread_->set_scanning(false);
        scanner_thread_->reset_dwell();
    }
    if (scanner_ptr_ != nullptr) {
        (void)scanner_ptr_->stop_scanning();
        // Clear lock state to prevent stale LOCKING/TRACKING after resume
        scanner_ptr_->clear_lock_state();
    }
    scanning_ = false;
    
    // CRITICAL: Verify baseband has stopped before touching FIFO
    SystemTime start_time = chTimeNow();
    constexpr uint32_t STOP_TIMEOUT_MS = 200;
    while (!verify_baseband_stopped(chTimeNow(), STOP_TIMEOUT_MS) && (chTimeNow() - start_time) < STOP_TIMEOUT_MS) {
        chThdSleepMilliseconds(5);
    }
    
    // CRITICAL: Safe FIFO clear using new method
    safe_clear_fifo();
    
    // Configure baseband for sweep bandwidth
    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, SWEEP_FFT_TRIGGER);
    // Note: spectrum_fifo_ remains nullptr until new mode starts streaming

    // Set up display for current pair (2+2 pagination)
    update_sweep_pair_display();

    radio::set_tuning_frequency(rf::Frequency(sweep_[active_sweep_idx_].f_center));
    current_frequency_ = sweep_[active_sweep_idx_].f_center;

    baseband::spectrum_streaming_start();
    scanning_ = true;
    button_start_stop_.set_text("Stop");
}

void DroneScannerUI::exit_sweep_mode() noexcept {
    // Prevent re-entrant exit
    if (!composite_active_) return;

    const bool was_auto = sweep_auto_mode_;
    composite_active_ = false;
    sweep_auto_mode_ = false;
    active_sweep_idx_ = 0;
    current_pair_ = 0;
    sweep_[0].enabled = false;
    sweep_[1].enabled = false;
    sweep_[2].enabled = false;
    sweep_[3].enabled = false;
    drone_display_.set_composite_mode(false);
    drone_display_.set_dual_sweep_mode(false);

    // Stop streaming BEFORE nulling fifo — prevents stale data in frame_sync
    if (scanning_) {
        baseband::spectrum_streaming_stop();
        scanning_ = false;
    }
    
    // CRITICAL: Verify baseband has stopped before touching FIFO
    SystemTime start_time = chTimeNow();
    constexpr uint32_t STOP_TIMEOUT_MS = 200;
    while (!verify_baseband_stopped(chTimeNow(), STOP_TIMEOUT_MS) && (chTimeNow() - start_time) < STOP_TIMEOUT_MS) {
        chThdSleepMilliseconds(5);
    }
    
    // CRITICAL: Safe FIFO clear using new method
    safe_clear_fifo();
    db_scan_count_ = 0;
    button_start_stop_.set_text("Start");

    // Restore baseband to normal bandwidth immediately
    portapack::receiver_model.set_sampling_rate(DEFAULT_SAMPLE_RATE_HZ);
    portapack::receiver_model.set_baseband_bandwidth(DEFAULT_SAMPLE_RATE_HZ);
    baseband::set_spectrum(DEFAULT_SAMPLE_RATE_HZ, SWEEP_FFT_TRIGGER);

    if (was_auto && scanner_ptr_ != nullptr) {
        // Continue scanning from last DB position (skip already-scanned)
        // Restore both frequency AND database index for exact resume
        if (last_db_frequency_ != 0) {
            scanner_ptr_->set_scan_frequency(last_db_frequency_);
        }
        if (database_ptr_ != nullptr) {
            database_ptr_->set_current_index(last_db_index_);
        }

        if (scanner_thread_ != nullptr) {
            scanner_thread_->set_scanning(true);
        }
        (void)scanner_ptr_->start_scanning();
        baseband::spectrum_streaming_start();
        scanning_ = true;
        button_start_stop_.set_text("Stop");
    }

    sweep_transition_guard_.clear();
}

void DroneScannerUI::on_sweep_spectrum(const ChannelSpectrum& spectrum) noexcept {
    baseband::spectrum_streaming_stop();
    auto& win = sweep_[active_sweep_idx_];
    win.process_bins(spectrum);

    // Skip first FFT after sweep entry — may be stale from previous mode
    // Restart retune at same frequency to get a clean FFT
    if (skip_next_fft_) {
        skip_next_fft_ = false;
        retune_sweep_window(win, nullptr);
        return;
    }

    if (scanner_ptr_ != nullptr) {
        // Use the exact frequency to radio was tuned to when this FFT was captured.
        // f_center may have already been incremented from the previous frame's step.
        const FreqHz fft_freq = (last_tuned_freq_ != 0) ? last_tuned_freq_ : win.f_center;
        // Pass sweep range boundaries to prevent false positives outside of range
        scanner_ptr_->process_spectrum_sweep(spectrum, fft_freq, win.f_min, win.f_max);
    }
    
    // Pattern capture: accumulate frames when in capture mode
    if (pattern_capture_state_ == PatternCaptureState::CAPTURING) {
        capture_pattern_frame(spectrum);
    }
    
    // Pattern matching: check for patterns periodically
    if (pattern_capture_state_ != PatternCaptureState::CAPTURING) {
        const FreqHz fft_freq = (last_tuned_freq_ != 0) ? last_tuned_freq_ : win.f_center;
        match_patterns_in_sweep(spectrum, fft_freq);
    }

    // Live display update: show current pair data every frame
    update_sweep_pair_display();

    if (win.pixel_index < COMPOSITE_SIZE) {
        // Normal step: advance frequency within current window
        // Use < instead of <= to allow final step that slightly overshoots f_max.
        // The peak frequency range check in process_spectrum_sweep() rejects
        // any detections outside [f_min, f_max], so overshoot is safe.
        // This ensures all 240 composite pixels are filled.
        if (win.f_center < win.f_max) {
            win.f_center += win.step_hz;
            retune_sweep_window(win, nullptr);
            return;
        }
        // Reached end of range — force completion and clear accumulators
        // to prevent stale values from affecting the next sweep pass.
        win.pixel_index = COMPOSITE_SIZE;
        win.bins_hz_acc = 0;
        win.pixel_max = 0;
    }

    // Current window sweep pass complete
    // Check if its pair is fully complete (both enabled windows in the pair done)
    const uint8_t w0 = current_pair_;
    const uint8_t w1 = w0 + 1;
    const bool w0_done = !sweep_[w0].enabled || sweep_[w0].pixel_index >= COMPOSITE_SIZE;
    const bool w1_done = (w1 >= MAX_SWEEP_WINDOWS) || !sweep_[w1].enabled || sweep_[w1].pixel_index >= COMPOSITE_SIZE;
    const bool pair_complete = w0_done && w1_done;

    if (pair_complete) {
        // Display final pair data before reset
        update_sweep_pair_display();
        drone_display_.set_dirty();

        // Reset windows in this pair for next scan pass
        if (sweep_[w0].enabled) sweep_[w0].reset();
        if (w1 < MAX_SWEEP_WINDOWS && sweep_[w1].enabled) sweep_[w1].reset();

        // Advance to next pair (pairs: 0=[w0,w1], 2=[w2,w3])
        const uint8_t next_pair = (current_pair_ + 2 < MAX_SWEEP_WINDOWS) ? current_pair_ + 2 : 0;

        // Full cycle wrap: all pairs have been visited.
        // Exit auto-mode before round-robin changes active_sweep_idx_,
        // but apply decay first (see wrap detection block below for rationale).
        if (next_pair == 0 && sweep_auto_mode_) {
            if (scanner_ptr_ != nullptr) {
                scanner_ptr_->apply_rssi_decay();
            }
            exit_sweep_mode();
            return;
        }

        current_pair_ = next_pair;
    }

    // Round-robin to next enabled window (GLOBAL cycling through all 4 windows)
    // This is critical: cycling must go through ALL windows, not just the current pair.
    // Otherwise, with 1-2 enabled windows, the round-robin wraps within the pair
    // and triggers premature pair_complete + auto-exit.
    uint8_t next = active_sweep_idx_;
    do {
        next = (next + 1) % MAX_SWEEP_WINDOWS;
    } while (!sweep_[next].enabled && next != active_sweep_idx_);

    // Wrap detection: if we cycled back to the first window of the pair
    // that just completed, a full round-robin pass is done.
    // This is the correct trigger for RSSI decay — it fires when ALL enabled
    // windows have been scanned once, regardless of how many windows are active.
    // A drone detected in the first window may not be re-seen for several seconds
    // while remaining windows are scanned — premature decay would remove it.
    if (pair_complete && next == w0) {
        if (scanner_ptr_ != nullptr) {
            scanner_ptr_->apply_rssi_decay();
        }

        if (sweep_auto_mode_) {
            exit_sweep_mode();
            return;
        }
        // Manual mode: continue sweeping (pair already advanced above)
    }

    active_sweep_idx_ = next;
    last_tuned_freq_ = sweep_[next].f_center;  // freq for first FFT of new window
    retune_sweep_window(sweep_[next], nullptr);
}

void DroneScannerUI::update_sweep_pair_display() noexcept {
    const uint8_t w0 = current_pair_;
    const uint8_t w1 = w0 + 1;

    if (w0 >= MAX_SWEEP_WINDOWS) return;
    if (!sweep_[w0].enabled && (w1 >= MAX_SWEEP_WINDOWS || !sweep_[w1].enabled)) return;

    if (w1 < MAX_SWEEP_WINDOWS && sweep_[w0].enabled && sweep_[w1].enabled) {
        // Dual mode: upper = w0, lower = w1
        drone_display_.set_sweep_range(sweep_[w0].f_min, sweep_[w0].f_max);
        drone_display_.set_composite_data(sweep_[w0].composite, COMPOSITE_SIZE);
        drone_display_.set_dual_sweep_mode(true);
        drone_display_.set_sweep2_range(sweep_[w1].f_min, sweep_[w1].f_max);
        drone_display_.set_sweep2_data(sweep_[w1].composite, COMPOSITE_SIZE);
    } else if (sweep_[w0].enabled) {
        // Single mode: only w0
        drone_display_.set_sweep_range(sweep_[w0].f_min, sweep_[w0].f_max);
        drone_display_.set_composite_data(sweep_[w0].composite, COMPOSITE_SIZE);
        drone_display_.set_dual_sweep_mode(false);
    } else if (w1 < MAX_SWEEP_WINDOWS && sweep_[w1].enabled) {
        // Only w1 enabled (non-contiguous): show w1 as single
        drone_display_.set_sweep_range(sweep_[w1].f_min, sweep_[w1].f_max);
        drone_display_.set_composite_data(sweep_[w1].composite, COMPOSITE_SIZE);
        drone_display_.set_dual_sweep_mode(false);
    }
}

uint8_t DroneScannerUI::pair_first(uint8_t idx) const noexcept {
    if (idx >= MAX_SWEEP_WINDOWS) return 0;
    return (idx / 2) * 2;
}

// ============================================================================
// SweepWindow implementations
// ============================================================================

void DroneScannerUI::SweepWindow::init(FreqHz start, FreqHz end, FreqHz step) noexcept {
    f_min = start;
    f_max = end;
    if (f_min >= f_max) {
        f_max = f_min + SWEEP_SLICE_BW;
    }
    pixel_step_hz = (f_max - f_min) / SWEEP_PIXELS_PER_SLICE;
    // Use config step if provided, otherwise fall back to FFT-based constant
    step_hz = (step > 0) ? step : (SWEEP_BINS_PER_STEP * EACH_BIN_SIZE);
    // f_center_ini positioned so pixel 239 maps to f_max.
    // The -2*BIN_SIZE offset prevents FFT overshoot beyond f_max which causes
    // false positives at frequencies outside the sweep range.
    // Looking Glass: f_center at f_min + SLICE_BW/2 - 2*BIN_SIZE → last pixel ≈ f_max.
    constexpr FreqHz BIN_SIZE = SWEEP_SLICE_BW / 256;
    f_center_ini = f_min - (2 * BIN_SIZE) + (SWEEP_SLICE_BW / 2);
    reset();
}

void DroneScannerUI::SweepWindow::reset() noexcept {
    memset(composite, 0, COMPOSITE_SIZE);
    f_center = f_center_ini;
    pixel_index = 0;
    pixel_max = 0;
    bins_hz_acc = 0;
}

bool DroneScannerUI::SweepWindow::is_exception(FreqHz hz) const noexcept {
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        if (exceptions[i] == 0) continue;
        const FreqHz lo = (exceptions[i] > exception_radius_hz) ? (exceptions[i] - exception_radius_hz) : 0;
        const FreqHz hi = exceptions[i] + exception_radius_hz;
        if (hz >= lo && hz <= hi) return true;
    }
    return false;
}

void DroneScannerUI::SweepWindow::process_bins(const ChannelSpectrum& spectrum) noexcept {
    SweepProcessor::process_frame(
        spectrum,
        composite,
        pixel_index,
        pixel_max,
        bins_hz_acc,
        pixel_step_hz,
        f_min,
        exception_radius_hz,
        exceptions,
        EXCEPTIONS_PER_WINDOW
    );
}

void DroneScannerUI::retune_sweep_window(SweepWindow& win, const char* prefix) noexcept {
    radio::set_tuning_frequency(rf::Frequency(win.f_center));
    current_frequency_ = win.f_center;
    last_tuned_freq_ = win.f_center;
    (void)prefix;
    baseband::spectrum_streaming_start();
}

void DroneScannerUI::on_pattern_capture_toggle() noexcept {
    if (pattern_capture_state_ == PatternCaptureState::IDLE) {
        // Enter pattern capture mode
        if (!composite_active_) {
            show_alert("Pattern capture needs Sweep mode", 2000);
            return;
        }
        pattern_capture_state_ = PatternCaptureState::SELECTING;
        pattern_select_start_ = 0;
        pattern_select_end_ = 0;
        button_ptr_.set_text("SEL");
        drone_display_.set_status_text("Tap spectrum to select region");
    } else if (pattern_capture_state_ == PatternCaptureState::SELECTING) {
        // Cancel selection
        pattern_capture_state_ = PatternCaptureState::IDLE;
        button_ptr_.set_text("PTR");
        drone_display_.set_status_text("Pattern capture cancelled");
    } else {
        // Cancel capture
        pattern_capture_state_ = PatternCaptureState::IDLE;
        button_ptr_.set_text("PTR");
        drone_display_.set_status_text("Pattern capture cancelled");
    }
}

void DroneScannerUI::on_touch_spectrum(uint16_t pixel_x) noexcept {
    if (pattern_capture_state_ != PatternCaptureState::SELECTING) return;
    
    if (pixel_x >= COMPOSITE_SIZE) return;
    
    if (pattern_select_start_ == 0) {
        // First touch - set start
        pattern_select_start_ = pixel_x;
        char status_buf[MAX_TEXT_LENGTH];
        snprintf(status_buf, sizeof(status_buf), "Start: %d, tap end", pixel_x);
        drone_display_.set_status_text(status_buf);
    } else {
        // Second touch - set end and start capture
        pattern_select_end_ = pixel_x;
        
        // Ensure start < end
        if (pattern_select_end_ < pattern_select_start_) {
            const uint16_t temp = pattern_select_start_;
            pattern_select_start_ = pattern_select_end_;
            pattern_select_end_ = temp;
        }
        
        // Validate selection (must be at least 10 pixels wide)
        if ((pattern_select_end_ - pattern_select_start_) < 10) {
            show_alert("Selection too small (min 10 pixels)", 2000);
            pattern_capture_state_ = PatternCaptureState::IDLE;
            button_ptr_.set_text("PTR");
            drone_display_.set_status_text("Selection cancelled");
            return;
        }
        
        // Start capturing frames
        pattern_capture_state_ = PatternCaptureState::CAPTURING;
        pattern_capture_frames_ = 0;
        pattern_waveform_sum_.fill(0);
        button_ptr_.set_text("CAP");
        drone_display_.set_status_text("Capturing pattern...");
    }
}

void DroneScannerUI::capture_pattern_frame(const ChannelSpectrum& spectrum) noexcept {
    if (pattern_capture_state_ != PatternCaptureState::CAPTURING) return;
    
    // Calculate FFT bin range from pixel selection
    // Composite pixels map to FFT bins via Looking Glass pattern
    // Lower sideband: pixels 0-119 → bins 134-253
    // Upper sideband: pixels 120-239 → bins 2-119
    
    constexpr size_t FFT_BINS_PER_PIXEL = SWEEP_SLICE_BW / SWEEP_PIXELS_PER_SLICE;
    
    // Normalize spectrum to 16-bin waveform
    uint8_t wave_16[PATTERN_WAVEFORM_SIZE];
    for (size_t i = 0; i < PATTERN_WAVEFORM_SIZE; ++i) {
        const size_t pixel_pos = pattern_select_start_ + (i * (pattern_select_end_ - pattern_select_start_) / PATTERN_WAVEFORM_SIZE;
        if (pixel_pos >= COMPOSITE_SIZE) break;
        
        // Map pixel to FFT bin
        uint8_t fft_bin;
        if (pixel_pos < SWEEP_FFT_MAP_CROSSOVER) {
            // Lower sideband: pixel → bin 134-253
            fft_bin = SWEEP_FFT_MAP_START + pixel_pos;
            if (fft_bin >= FFT_BIN_COUNT) fft_bin = FFT_BIN_COUNT - 1;
        } else {
            // Upper sideband: pixel → bin 2-119
            fft_bin = pixel_pos - SWEEP_FFT_MAP_CROSSOVER + 2;
            if (fft_bin >= FFT_DC_SPIKE_START) fft_bin = FFT_DC_SPIKE_END;
        }
        
        if (fft_bin < FFT_BIN_COUNT) {
            wave_16[i] = spectrum.db[fft_bin];
        } else {
            wave_16[i] = 0;
        }
    }
    
    // Accumulate waveform
    for (size_t i = 0; i < PATTERN_WAVEFORM_SIZE; ++i) {
        pattern_waveform_sum_[i] += wave_16[i];
    }
    
    pattern_capture_frames_++;
    
    if (pattern_capture_frames_ >= PATTERN_CAPTURE_FRAMES) {
        finalize_pattern_capture();
    }
}

void DroneScannerUI::finalize_pattern_capture() noexcept {
    if (pattern_capture_frames_ < PATTERN_CAPTURE_FRAMES) {
        return;
    }
    
    // Average accumulated waveform
    SignalPattern pattern;
    pattern.match_count = 0;
    pattern.created_time = chTimeNow();
    pattern.flags = SignalPattern::Flags::ENABLED;
    pattern.match_threshold = DEFAULT_PATTERN_CORRELATION_THRESHOLD;
    
    for (size_t i = 0; i < PATTERN_WAVEFORM_SIZE; ++i) {
        pattern.waveform[i] = pattern_waveform_sum_[i] / PATTERN_CAPTURE_FRAMES;
    }

    // Extract features from current spectrum
    // Use SpectrumShape to analyze the selected region

    // Get current frequency from active sweep window
    if (scanner_ptr_ != nullptr) {
        ScanConfig cfg = scanner_ptr_->get_config();
        SpectrumShape::Config config{
            cfg.spectrum_margin,
            cfg.spectrum_min_width,
            cfg.spectrum_max_width,
            cfg.spectrum_peak_sharpness,
            cfg.spectrum_peak_ratio,
            cfg.spectrum_valley_depth,
            cfg.spectrum_flatness,
            cfg.spectrum_symmetry,
            false
        };
        (void)config;  // Will be used for feature extraction in future
    }
    
    // Create pattern name with timestamp
    char name_buf[PATTERN_NAME_MAX_LEN];
    const FreqHz center_freq = sweep_[active_sweep_idx_].f_center;
    const uint32_t freq_mhz = static_cast<uint32_t>(center_freq / 1000000);
    snprintf(name_buf, sizeof(name_buf), "PAT_%luMHz", (unsigned long)freq_mhz);
    safe_str_copy(pattern.name, PATTERN_NAME_MAX_LEN, name_buf);
    
    // Save pattern via PatternManager
    if (scanner_ptr_ != nullptr) {
        const ErrorCode err = scanner_ptr_->save_pattern(pattern);
        if (err == ErrorCode::SUCCESS) {
            char success_msg[MAX_TEXT_LENGTH];
            snprintf(success_msg, sizeof(success_msg), "Saved: %s", name_buf);
            show_alert(success_msg, 2000);
            drone_display_.set_status_text(success_msg);
        } else {
            char error_msg[MAX_TEXT_LENGTH];
            snprintf(error_msg, sizeof(error_msg), "Save failed: err %d", (int)err);
            show_alert(error_msg, 3000);
            drone_display_.set_status_text("Pattern save failed");
        }
    }
    
    // Reset capture state
    pattern_capture_state_ = PatternCaptureState::IDLE;
    button_ptr_.set_text("PTR");
    pattern_select_start_ = 0;
    pattern_select_end_ = 0;
    pattern_capture_frames_ = 0;
}

void DroneScannerUI::match_patterns_in_sweep(const ChannelSpectrum& spectrum, FreqHz freq) noexcept {
    if (scanner_ptr_ == nullptr) return;
    
    // Only match every PATTERN_MATCH_INTERVAL frames to save CPU
    pattern_match_counter_++;
    if (pattern_match_counter_ < PATTERN_MATCH_INTERVAL) return;
    pattern_match_counter_ = 0;
    
    // Check if pattern matching is enabled
    ScanConfig cfg = scanner_ptr_->get_config();
    if (!cfg.pattern_matching_enabled) return;
    
    // Analyze spectrum shape
    SpectrumShape::Config shape_config{
        cfg.spectrum_margin,
        cfg.spectrum_min_width,
        cfg.spectrum_max_width,
        cfg.spectrum_peak_sharpness,
        cfg.spectrum_peak_ratio,
        cfg.spectrum_valley_depth,
        cfg.spectrum_flatness,
        cfg.spectrum_symmetry,
        false
    };
    
    const SpectrumShape::AnalysisResult shape = SpectrumShape::analyze(
        spectrum.db.data(),
        spectrum_shape_sort_buf_,
        shape_config
    );
    
    if (!shape.signal_detected) return;
    
    // Match against loaded patterns
    PatternMatcher& matcher = scanner_ptr_->get_pattern_matcher();
    const PatternMatchResult result = matcher.match(spectrum.db.data(), shape);
    
    if (result.matched && result.correlation_score >= cfg.pattern_min_correlation) {
        // Pattern matched!
        const SignalPattern* patterns = scanner_ptr_->get_patterns();
        const size_t pattern_count = scanner_ptr_->get_pattern_count();
        
        if (result.pattern_index < pattern_count) {
            const SignalPattern& matched_pattern = patterns[result.pattern_index];
            
            // Show match notification in status
            char match_msg[MAX_TEXT_LENGTH];
            const uint32_t freq_mhz = static_cast<uint32_t>(freq / 1000000);
            snprintf(match_msg, sizeof(match_msg), "MATCH: %s @ %luMHz",
                     matched_pattern.name, (unsigned long)freq_mhz);
            drone_display_.set_status_text(match_msg);
            
            // Optionally update display to show matched pattern name
            // This could be integrated with drone display or shown as overlay
        }
    }
}

} // namespace drone_analyzer
