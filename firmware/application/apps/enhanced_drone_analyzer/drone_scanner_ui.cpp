#include <cstdint>
#include <cstring>
#include <array>
#include <new>

#include "ch.h"

#include "ui.hpp"
#include "ui_fileman.hpp"
#include "scanner.hpp"
#include "scanner_thread.hpp"
#include "drone_settings.hpp"
#include "settings_manager.hpp"
#include "drone_sweep_view.hpp"
#include "pattern_manager_view.hpp"
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

// ============================================================================
// Handler registration — placement-new to allow manual lifetime control.
// Prevents DBLREG hard fault when PatternManagerView is pushed.
// ============================================================================
void DroneScannerUI::register_handlers() noexcept {
    if (handlers_active_) return;

    auto* const h = reinterpret_cast<HandlerStorage*>(handler_storage_);

    new (&h->spectrum_config) MessageHandlerRegistration{
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }
    };
    new (&h->frame_sync) MessageHandlerRegistration{
        Message::ID::DisplayFrameSync,
        [this](Message* const) {
            if (!this->scanning_) return;
            if (this->spectrum_fifo_ != nullptr) {
                ChannelSpectrum& spectrum = this->spectrum_buffer_;
                if (this->spectrum_fifo_->out(spectrum)) {
                    if (this->composite_active_) {
                        this->on_sweep_spectrum(spectrum, this->current_frequency_);
                    } else {
                        this->on_channel_spectrum(spectrum);
                        this->db_scan_count_++;
                        if (this->db_scan_count_ >= this->DB_SCANS_PER_SWEEP) {
                            this->db_scan_count_ = 0;
                            this->sweep_auto_mode_ = true;
                            this->enter_sweep_mode();
                        }
                    }
                }
            }
            this->refresh_ui();
        }
    };
    new (&h->retune) MessageHandlerRegistration{
        Message::ID::Retune,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const RetuneMessage*>(p);
            this->on_retune(message.freq, message.range);
        }
    };
    new (&h->channel_stats) MessageHandlerRegistration{
        Message::ID::ChannelStatistics,
        [this](Message* const p) {
            const auto message = *reinterpret_cast<const ChannelStatisticsMessage*>(p);
            this->latest_max_db_ = message.statistics.max_db;
        }
    };

    handlers_active_ = true;
}

void DroneScannerUI::unregister_handlers() noexcept {
    if (!handlers_active_) return;

    auto* const h = reinterpret_cast<HandlerStorage*>(handler_storage_);
    h->spectrum_config.~MessageHandlerRegistration();
    h->frame_sync.~MessageHandlerRegistration();
    h->retune.~MessageHandlerRegistration();
    h->channel_stats.~MessageHandlerRegistration();

    handlers_active_ = false;
}

DroneScannerUI::DroneScannerUI(NavigationView& nav) noexcept
    : View()
    , nav_(nav)
    , big_display_({BIG_FREQUENCY_X, BIG_FREQUENCY_Y, BIG_FREQUENCY_WIDTH, 52}, 0)
    , drone_display_({{0, 68, DISPLAY_WIDTH, 206}})
    , sweep_transition_guard_()
    , button_debounce_guard_() {
    add_children({
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
    });

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
        // Debounce rapid taps that could cause race conditions
        if (!button_debounce_guard_.try_set()) return;

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
        // Clear debounce guard after handling completes
        button_debounce_guard_.clear();
    };

    button_mode_.on_select = [this](ui::Button&) {
        // Debounce rapid taps that could cause race conditions
        if (!button_debounce_guard_.try_set()) return;

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
        // Clear debounce guard after state transition completes
        button_debounce_guard_.clear();
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

    // PTR button: open pattern manager view
    button_ptr_.on_select = [this](ui::Button&) {
        if (initialization_failed_ || scanner_ptr_ == nullptr) {
            show_error(ErrorCode::HARDWARE_NOT_INITIALIZED, ERROR_DURATION_MS);
            return;
        }
        // CRITICAL: Unregister message handlers BEFORE pushing PatternManagerView.
        // Both views register handlers for ChannelSpectrumConfig / DisplayFrameSync.
        // Without explicit unregister, MessageHandlerMap hits chDbgPanic("MsgDblReg").
        unregister_handlers();

        // Stop streaming and scanner thread
        if (scanning_) {
            baseband::spectrum_streaming_stop();
            scanning_ = false;
        }
        if (scanner_thread_ != nullptr && scanner_thread_->is_scanning()) {
            scanner_thread_->set_scanning(false);
        }
        // Exit sweep mode if active - this also restores bandwidth.
        if (composite_active_) {
            exit_sweep_mode();
        }
        nav_.push<PatternManagerView>();
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
    // Unregister handlers before destroying anything else.
    unregister_handlers();

    // CRITICAL: Exit sweep mode first to stop baseband streaming
    // Prevents DBLREG hard fault when PatternManagerView is pushed after sweep
    if (composite_active_) {
        exit_sweep_mode();
    }

    destruct_objects();

    audio::output::stop();
    portapack::receiver_model.disable();
    baseband::shutdown();

    // Ensure atomic flags are cleared on destruction
    button_debounce_guard_.clear();
    sweep_transition_guard_.clear();
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
    register_handlers();

    if (scanner_ptr_ != nullptr) {
        scanner_ptr_->refresh_patterns();
    }

    if (scanner_thread_ != nullptr && !scanner_thread_->is_active()) {
        scanner_thread_->start();
    }

    if (composite_active_ && scanner_ptr_ != nullptr) {
        ScanConfig cfg = scanner_ptr_->get_config();
        (void)sweep_coordinator_.start_sweep(cfg);

        update_sweep_pair_display();

        const FreqHz tune_freq = sweep_coordinator_.get_retune_frequency();
        radio::set_tuning_frequency(rf::Frequency(tune_freq));
        current_frequency_ = tune_freq;
        last_tuned_freq_ = tune_freq;

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
        const size_t hist_count = sweep_coordinator_.get_histogram_data(
            refresh_hist_data_, HISTOGRAM_BUFFER_SIZE
        );
        if (hist_count > 0) {
            drone_display_.set_histogram_data(refresh_hist_data_, hist_count);
        }
    }

    // Update big frequency display (only if frequency or color changed)
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

        // Only update BigFrequency widget if frequency or color actually changed
        // This prevents redundant widget repaints that cause flicker
        if (current_frequency_ != last_displayed_freq_ || color != last_displayed_color_) {
            bigdisplay_update(color);
            last_displayed_freq_ = current_frequency_;
            last_displayed_color_ = color;
        }
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

    // Only mark UI dirty if DroneDisplay has actual changes to render
    // This eliminates redundant full-view repaints that cause screen flicker
    if (drone_display_.has_pending_updates()) {
        set_dirty();
    }
}

void DroneScannerUI::on_retune(FreqHz freq, uint32_t range) noexcept {
    (void)range;
    current_frequency_ = freq;
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
    if (composite_active_) return;
    if (!sweep_transition_guard_.try_set()) return;

    ScanConfig cfg;
    if (scanner_ptr_ != nullptr) {
        cfg = scanner_ptr_->get_config();
    }

    if (!sweep_coordinator_.start_sweep(cfg)) {
        composite_active_ = false;
        drone_display_.set_composite_mode(false);
        sweep_transition_guard_.clear();
        return;
    }

    composite_active_ = true;
    sweep_coordinator_.set_auto_mode(false);
    drone_display_.set_composite_mode(true);

    // Save database state before entering sweep mode
    // Priority: 1) current_frequency from UI, 2) locked frequency from scanner, 3) scanner's current
    if (database_ptr_ != nullptr) {
        last_db_index_ = database_ptr_->get_current_index();
        FreqHz save_freq = 0;
        if (current_frequency_ != 0) {
            save_freq = current_frequency_;
        } else if (scanner_ptr_ != nullptr) {
            save_freq = scanner_ptr_->get_locked_frequency();
            if (save_freq == 0 && scanner_ptr_->get_current_frequency().has_value()) {
                save_freq = scanner_ptr_->get_current_frequency().value();
            }
        }
        last_db_frequency_ = save_freq;
    }

    if (scanner_thread_ != nullptr) {
        scanner_thread_->set_scanning(false);
        scanner_thread_->reset_dwell();
    }
    if (scanner_ptr_ != nullptr) {
        (void)scanner_ptr_->stop_scanning();
        scanner_ptr_->clear_lock_state();
    }
    scanning_ = false;

    portapack::receiver_model.set_sampling_rate(SWEEP_SLICE_BW);
    portapack::receiver_model.set_baseband_bandwidth(SWEEP_SLICE_BW);
    baseband::set_spectrum(SWEEP_SLICE_BW, SWEEP_FFT_TRIGGER);
    spectrum_fifo_ = nullptr;

    update_sweep_pair_display();

    const FreqHz tune_freq = sweep_coordinator_.get_retune_frequency();
    radio::set_tuning_frequency(rf::Frequency(tune_freq));
    current_frequency_ = tune_freq;
    last_tuned_freq_ = tune_freq;

    baseband::spectrum_streaming_start();
    scanning_ = true;
    button_start_stop_.set_text("Stop");

    sweep_transition_guard_.clear();
}

void DroneScannerUI::exit_sweep_mode() noexcept {
    if (!composite_active_) return;
    if (!sweep_transition_guard_.try_set()) return;

    const bool was_auto = sweep_auto_mode_;
    composite_active_ = false;
    sweep_auto_mode_ = false;
    sweep_coordinator_.stop_sweep();

    drone_display_.set_composite_mode(false);
    drone_display_.set_dual_sweep_mode(false);

    if (scanning_) {
        baseband::spectrum_streaming_stop();
        scanning_ = false;
    }
    spectrum_fifo_ = nullptr;
    db_scan_count_ = 0;
    button_start_stop_.set_text("Start");

    portapack::receiver_model.set_sampling_rate(DEFAULT_SAMPLE_RATE_HZ);
    portapack::receiver_model.set_baseband_bandwidth(DEFAULT_SAMPLE_RATE_HZ);
    baseband::set_spectrum(DEFAULT_SAMPLE_RATE_HZ, SWEEP_FFT_TRIGGER);

    // Restore database scanning state - check if we have valid DB state to restore
    // This replaces the old was_auto check which could fail to restore in some cases
    if (scanner_ptr_ != nullptr && database_ptr_ != nullptr && db_loaded_) {
        // Always restore database index and frequency when exiting sweep to DB mode
        // Use set_current_index for direct index restoration (more reliable than frequency match)
        if (last_db_index_ < db_entry_count_) {
            database_ptr_->set_current_index(last_db_index_);
        } else {
            database_ptr_->set_current_index(0);  // Safe fallback
        }

        // Set scanner frequency - if last_db_frequency_ is 0, scanner will get next from DB
        if (last_db_frequency_ != 0) {
            scanner_ptr_->set_scan_frequency(last_db_frequency_);
        } else {
            // Reset to first DB entry if no valid frequency was saved
            scanner_ptr_->reset_frequency();
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

void DroneScannerUI::on_sweep_spectrum(const ChannelSpectrum& spectrum, FreqHz current_freq) noexcept {
    if (!composite_active_) return;

    baseband::spectrum_streaming_stop();

    const bool needs_update = sweep_coordinator_.process_spectrum(spectrum, current_freq);
    const FreqHz retune_freq = sweep_coordinator_.get_retune_frequency();

    if (needs_update && retune_freq != 0) {
        last_tuned_freq_ = retune_freq;
        current_frequency_ = retune_freq;
        radio::set_tuning_frequency(rf::Frequency(retune_freq));
        baseband::spectrum_streaming_start();

        if (scanner_ptr_ != nullptr) {
            FreqHz f_min = 0, f_max = 0;
            const uint8_t active_idx = sweep_coordinator_.get_active_window_index();
            if (sweep_coordinator_.get_window_range(active_idx, f_min, f_max)) {
                scanner_ptr_->process_spectrum_sweep(spectrum, current_freq, f_min, f_max);
            }

            if (scanner_ptr_->is_pattern_matched()) {
                drone_display_.set_matched_pattern_bin(static_cast<int16_t>(scanner_ptr_->get_matched_pattern_bin()));
            } else {
                drone_display_.set_matched_pattern_bin(-1);
            }
        }
    }

    update_sweep_pair_display();
    drone_display_.set_dirty();
}

void DroneScannerUI::update_sweep_pair_display() noexcept {
    const uint8_t pair_idx = sweep_coordinator_.get_current_pair_index();
    const uint8_t w0 = pair_idx;
    const uint8_t w1 = w0 + 1;

    FreqHz f_min0 = 0, f_max0 = 0, f_min1 = 0, f_max1 = 0;

    const bool has0 = sweep_coordinator_.get_composite_data(w0, sweep_data0_, COMPOSITE_SIZE);
    const bool has1 = sweep_coordinator_.get_composite_data(w1, sweep_data1_, COMPOSITE_SIZE);

    if (has0) (void)sweep_coordinator_.get_window_range(w0, f_min0, f_max0);
    if (has1) (void)sweep_coordinator_.get_window_range(w1, f_min1, f_max1);

    if (has0 && has1) {
        drone_display_.set_sweep_range(f_min0, f_max0);
        drone_display_.set_composite_data(sweep_data0_, COMPOSITE_SIZE);
        drone_display_.set_dual_sweep_mode(true);
        drone_display_.set_sweep2_range(f_min1, f_max1);
        drone_display_.set_sweep2_data(sweep_data1_, COMPOSITE_SIZE);
    } else if (has0) {
        drone_display_.set_sweep_range(f_min0, f_max0);
        drone_display_.set_composite_data(sweep_data0_, COMPOSITE_SIZE);
        drone_display_.set_dual_sweep_mode(false);
    } else if (has1) {
        drone_display_.set_sweep_range(f_min1, f_max1);
        drone_display_.set_composite_data(sweep_data1_, COMPOSITE_SIZE);
        drone_display_.set_dual_sweep_mode(false);
    }
}

DroneScanner* get_scanner_ptr() noexcept {
    return &s_scanner;
}

DroneScanner& get_scanner_instance() noexcept {
    return s_scanner;
}

} // namespace drone_analyzer
