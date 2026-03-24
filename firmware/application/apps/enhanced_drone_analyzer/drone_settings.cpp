#include "drone_settings.hpp"
#include "scanner.hpp"
#include "audio_alerts.hpp"
#include "drone_display.hpp"
#include "ui_receiver.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "receiver_model.hpp"
#include <cstring>

namespace drone_analyzer {

// ============================================================================
// DroneSettings Implementation
// ============================================================================

DroneSettings::DroneSettings() noexcept
    : scanning_mode(DEFAULT_SCANNING_MODE)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , scan_sensitivity(38)
    , spectrum_visible(true)
    , histogram_visible(true)
    , drone_list_visible(true)
    , status_bar_visible(true)
    , audio_alerts_enabled(true)
    , alert_rssi_threshold_dbm(RSSI_HIGH_THREAT_THRESHOLD_DBM)
    , min_threat_level(ThreatLevel::LOW)
    , dwell_enabled(false)
    , confirm_count_enabled(false)
    , noise_blacklist_enabled(false)
    , sweep_start_freq(SWEEP_DEFAULT_START_HZ)
    , sweep_end_freq(SWEEP_DEFAULT_END_HZ)
    , sweep_step_freq(20000000) {  // Default 20 MHz (matches SWEEP_BANDWIDTH)
}

void DroneSettings::reset_to_defaults() noexcept {
    scanning_mode = DEFAULT_SCANNING_MODE;
    scan_interval_ms = SCAN_CYCLE_INTERVAL_MS;
    scan_sensitivity = 38;
    
    spectrum_visible = true;
    histogram_visible = true;
    drone_list_visible = true;
    status_bar_visible = true;
    
    audio_alerts_enabled = true;
    alert_rssi_threshold_dbm = RSSI_HIGH_THREAT_THRESHOLD_DBM;
    
    min_threat_level = ThreatLevel::LOW;
    
    dwell_enabled = true;
    confirm_count_enabled = false;
    noise_blacklist_enabled = false;
    
    sweep_start_freq = SWEEP_DEFAULT_START_HZ;
    sweep_end_freq = SWEEP_DEFAULT_END_HZ;
    sweep_step_freq = 20000000;  // 20 MHz
}

// ============================================================================
// DroneSettingsView Constructor / Destructor
// ============================================================================

DroneSettingsView::DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display) noexcept
    : ui::View()
    , labels_({
        {{UI_POS_X(1), UI_POS_Y(1)}, "Int(ms):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(3)}, "Sens:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(13), UI_POS_Y(3)}, "Vol:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(5)}, "Sweep(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(9), UI_POS_Y(5)}, "-", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(7)}, "Step(kHz):", Theme::getInstance()->fg_light->foreground},
    })
    , field_scan_mode_({UI_POS_X(0), UI_POS_Y(0)}, 1, {
        {"-", 0},
    })
    , field_scan_interval_({UI_POS_X(1), UI_POS_Y(2)}, 4, {10, 1000}, 10, ' ')
    , field_rssi_threshold_({UI_POS_X(1), UI_POS_Y(4)}, 3, {0, 100}, 1, ' ')
    , field_volume_({UI_POS_X(17), UI_POS_Y(3)}, 2, {0, 99}, 1, ' ')
    , check_audio_alerts_({UI_POS_X(1), UI_POS_Y(10)}, 6, "Audio", false)
    , check_spectrum_visible_({UI_POS_X(15), UI_POS_Y(10)}, 5, "Spec", false)
    , check_histogram_visible_({UI_POS_X(15), UI_POS_Y(11)}, 5, "Hist", false)
    , check_dwell_enabled_({UI_POS_X(1), UI_POS_Y(11)}, 6, "Dwell", false)
    , check_confirm_count_({UI_POS_X(1), UI_POS_Y(12)}, 8, "Confirm", false)
    , check_noise_blacklist_({UI_POS_X(1), UI_POS_Y(13)}, 8, "Blklist", false)
    , field_sweep_start_({UI_POS_X(1), UI_POS_Y(6)}, 5, {100, 7200}, 1, ' ')
    , field_sweep_end_({UI_POS_X(10), UI_POS_Y(6)}, 5, {100, 7200}, 1, ' ')
    , field_sweep_step_({UI_POS_X(1), UI_POS_Y(8)}, 5, {1000, 99999}, 1000, ' ')
    , button_defaults_({UI_POS_X(0), UI_POS_Y_BOTTOM(5), UI_POS_WIDTH(14), 28}, "DEFAULT")
    , button_save_({UI_POS_X(16), UI_POS_Y_BOTTOM(5), UI_POS_WIDTH(14), 28}, "SAVE")
    , button_cancel_({UI_POS_X(16), UI_POS_Y_BOTTOM(3), UI_POS_WIDTH(14), 28}, "CANCEL")
    , button_about_({UI_POS_X(0), UI_POS_Y_BOTTOM(3), UI_POS_WIDTH(14), 28}, "ABOUT")
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , display_ptr_(display)
    , original_config_(config)
    , settings_()
    , settings_dirty_(false) {
    settings_.scanning_mode = config.mode;
    settings_.scan_interval_ms = config.scan_interval_ms;
    settings_.alert_rssi_threshold_dbm = config.rssi_threshold_dbm;
    settings_.dwell_enabled = config.dwell_enabled;
    settings_.confirm_count_enabled = config.confirm_count_enabled;
    settings_.noise_blacklist_enabled = config.noise_blacklist_enabled;

    add_children({
        &labels_,
        &field_scan_interval_,
        &field_rssi_threshold_,
        &field_volume_,
        &check_audio_alerts_,
        &check_spectrum_visible_,
        &check_histogram_visible_,
        &check_dwell_enabled_,
        &check_confirm_count_,
        &check_noise_blacklist_,
        &field_sweep_start_,
        &field_sweep_end_,
        &field_sweep_step_,
        &button_defaults_,
        &button_save_,
        &button_cancel_,
        &button_about_
    });

    field_scan_interval_.set_value(settings_.scan_interval_ms);
    // Convert dBm threshold to sensitivity: sens = (dbm + 120) * 5 / 4
    {
        const int32_t sens = (settings_.alert_rssi_threshold_dbm + 120) * 5 / 4;
        field_rssi_threshold_.set_value(sens < 0 ? 0 : (sens > 100 ? 100 : sens));
    }
    field_volume_.set_value(portapack::receiver_model.normalized_headphone_volume());
    check_audio_alerts_.set_value(settings_.audio_alerts_enabled);
    check_spectrum_visible_.set_value(settings_.spectrum_visible);
    check_histogram_visible_.set_value(settings_.histogram_visible);
    check_dwell_enabled_.set_value(settings_.dwell_enabled);
    check_confirm_count_.set_value(settings_.confirm_count_enabled);
    check_noise_blacklist_.set_value(settings_.noise_blacklist_enabled);

    // Sweep frequency range fields (GHz → MHz for display)
    field_sweep_start_.set_value(static_cast<int32_t>(settings_.sweep_start_freq / 1000000));
    field_sweep_end_.set_value(static_cast<int32_t>(settings_.sweep_end_freq / 1000000));

    // Sweep step field (kHz)
    field_sweep_step_.set_value(static_cast<int32_t>(settings_.sweep_step_freq / 1000));

    button_save_.on_select = [this](ui::Button&) {
        if (scanner_ptr_ != nullptr) {
            ScanConfig updated_config = original_config_;
            updated_config.mode = settings_.scanning_mode;
            updated_config.scan_interval_ms = settings_.scan_interval_ms;
            updated_config.rssi_threshold_dbm = settings_.alert_rssi_threshold_dbm;
            updated_config.sweep_start_freq = settings_.sweep_start_freq;
            updated_config.sweep_end_freq = settings_.sweep_end_freq;
            updated_config.sweep_step_freq = settings_.sweep_step_freq;
            updated_config.dwell_enabled = settings_.dwell_enabled;
            updated_config.confirm_count_enabled = settings_.confirm_count_enabled;
            updated_config.noise_blacklist_enabled = settings_.noise_blacklist_enabled;

            const ErrorCode err = scanner_ptr_->set_config(updated_config);
            if (err != ErrorCode::SUCCESS) {
                return;
            }
        }

        // Save settings to SD card file
        File file;
        ensure_directory(settings_dir);
        const auto open_result = file.create(settings_dir / u"eda_settings.txt");
        if (!open_result) {
            char buffer[1024];
            size_t offset = 0;

            // Write header
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "# Enhanced Drone Analyzer Settings\n");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "# Auto-generated by EDA Settings\n\n");

            // Write scanning settings
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "spectrum_mode=SEQUENTIAL\n");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "scan_interval_ms=%lu\n", (unsigned long)settings_.scan_interval_ms);
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "sensitivity=%u\n", (unsigned)settings_.scan_sensitivity);
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "rssi_threshold_db=%ld\n", (long)settings_.alert_rssi_threshold_dbm);
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "enable_audio_alerts=%s\n", settings_.audio_alerts_enabled ? "true" : "false");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "volume=%u\n", (unsigned)portapack::receiver_model.normalized_headphone_volume());
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "show_spectrum=%s\n", settings_.spectrum_visible ? "true" : "false");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "show_histogram=%s\n", settings_.histogram_visible ? "true" : "false");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "sweep_start_mhz=%lu\n", (unsigned long)(settings_.sweep_start_freq / 1000000));
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "sweep_end_mhz=%lu\n", (unsigned long)(settings_.sweep_end_freq / 1000000));
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "sweep_step_khz=%lu\n", (unsigned long)(settings_.sweep_step_freq / 1000));
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "freqman_path=DRONES\n");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "settings_version=1.0\n");

            file.write(buffer, offset);
            file.close();
        }

        nav_.pop();
    };

    button_cancel_.on_select = [this](ui::Button&) {
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        settings_.reset_to_defaults();
        portapack::receiver_model.set_normalized_headphone_volume(70);
        settings_dirty_ = true;
        apply_settings();
    };

    button_about_.on_select = [this](ui::Button&) {
        static constexpr const char* kAboutMessage =
            "Author: Kuznetsov Maxim\n"
            "Orenburg branch\n"
            "Greetings to everyone who risks\n"
            "Support the author:\n"
            "Card: 2202 20202 5787 1695\n"
            "YooMoney: 41001810704697\n"
            "TON: UQCdtMxQB5zbQBOICkY90l\n"
            "TQQqcs8V-V28Bf2AGvl8xOc5HR\n"
            "Telegram: @max_ai_master\n"
            "TM PowerHamster2188";
        nav_.display_modal("About Author", kAboutMessage);
    };

    field_scan_interval_.on_change = [this](int32_t v) {
        settings_.scan_interval_ms = static_cast<uint32_t>(v);
        settings_dirty_ = true;
    };

    field_rssi_threshold_.on_change = [this](int32_t v) {
        // Convert sensitivity (0-100) to dBm threshold (-120 to -40)
        settings_.alert_rssi_threshold_dbm = -120 + (v * 4 / 5);
        settings_.scan_sensitivity = static_cast<uint8_t>(v);
        settings_dirty_ = true;
    };

    field_volume_.on_change = [this](int32_t v) {
        portapack::receiver_model.set_normalized_headphone_volume(static_cast<uint8_t>(v));
    };

    check_audio_alerts_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.audio_alerts_enabled = v;
        AudioAlertManager::set_enabled(v);
        settings_dirty_ = true;
    };

    check_spectrum_visible_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.spectrum_visible = v;
        if (display_ptr_ != nullptr) {
            display_ptr_->set_spectrum_visible(v);
        }
        settings_dirty_ = true;
    };

    check_histogram_visible_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.histogram_visible = v;
        if (display_ptr_ != nullptr) {
            display_ptr_->set_histogram_visible(v);
        }
        settings_dirty_ = true;
    };

    check_dwell_enabled_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.dwell_enabled = v;
        settings_dirty_ = true;
    };
    check_confirm_count_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.confirm_count_enabled = v;
        settings_dirty_ = true;
    };
    check_noise_blacklist_.on_select = [this](ui::Checkbox&, bool v) {
        settings_.noise_blacklist_enabled = v;
        settings_dirty_ = true;
    };

    // Sweep frequency range callbacks
    field_sweep_start_.on_change = [this](int32_t v) {
        settings_.sweep_start_freq = static_cast<FreqHz>(v) * 1000000ULL;
        settings_dirty_ = true;
    };
    field_sweep_end_.on_change = [this](int32_t v) {
        settings_.sweep_end_freq = static_cast<FreqHz>(v) * 1000000ULL;
        settings_dirty_ = true;
    };

    // Sweep step callback (kHz → Hz)
    field_sweep_step_.on_change = [this](int32_t v) {
        settings_.sweep_step_freq = static_cast<FreqHz>(v) * 1000ULL;
        settings_dirty_ = true;
    };

    // Frequency keypad callbacks — open on touch
    auto open_keypad = [this](ui::NumberField& field) {
        auto keypad = nav_.push<ui::FrequencyKeypadView>(static_cast<rf::Frequency>(field.value()) * 1000000ULL);
        keypad->on_changed = [&field](rf::Frequency f) {
            field.set_value(static_cast<int32_t>(f / 1000000));
        };
    };
    field_sweep_start_.on_select = open_keypad;
    field_sweep_end_.on_select = open_keypad;
}

DroneSettingsView::~DroneSettingsView() noexcept {
}

void DroneSettingsView::paint(ui::Painter& painter) {
    (void)painter;
}

void DroneSettingsView::focus() {
    field_scan_interval_.focus();
}

bool DroneSettingsView::on_touch(TouchEvent event) {
    if (event.type == TouchEvent::Type::End) {
        // Let child widgets (buttons, checkboxes, options) handle touch first
        if (View::on_touch(event)) {
            return true;
        }

        // No child handled it — check if touch is on a frequency field
        struct FreqField { ui::NumberField* field; };
        FreqField fields[] = {
            {&field_sweep_start_}, {&field_sweep_end_},
            {&field_sweep_step_},
        };
        for (auto& ff : fields) {
            if (ff.field->screen_rect().contains(event.point)) {
                auto keypad = nav_.push<ui::FrequencyKeypadView>(
                    static_cast<rf::Frequency>(ff.field->value()) * 1000000ULL);
                keypad->on_changed = [field = ff.field](rf::Frequency f) {
                    field->set_value(static_cast<int32_t>(f / 1000000));
                };
                return true;
            }
        }
    }
    return View::on_touch(event);
}

void DroneSettingsView::reset_settings() noexcept {
    settings_.reset_to_defaults();
    settings_dirty_ = true;
    apply_settings();
}

ErrorCode DroneSettingsView::load_settings() noexcept {
    File file;
    const auto open_result = file.open(settings_dir / u"eda_settings.txt", true, false);
    if (!open_result) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }

    constexpr size_t READ_CHUNK_SIZE = 256;
    uint8_t chunk[READ_CHUNK_SIZE];
    uint8_t line_buf[128];
    size_t line_len = 0;

    auto parse_line = [&](const uint8_t* buf, size_t len) {
        if (len == 0 || buf[0] == '#') {
            return;  // Skip empty lines and comments
        }

        // Find '=' separator
        size_t eq_pos = 0;
        for (size_t i = 0; i < len; ++i) {
            if (buf[i] == '=') {
                eq_pos = i;
                break;
            }
        }

        if (eq_pos == 0 || eq_pos >= len - 1) {
            return;  // Invalid line
        }

        // Parse key
        char key[32];
        size_t key_len = eq_pos;
        if (key_len > 31) key_len = 31;
        for (size_t i = 0; i < key_len; ++i) {
            key[i] = static_cast<char>(buf[i]);
        }
        key[key_len] = '\0';

        // Parse value
        const uint8_t* val_start = buf + eq_pos + 1;
        size_t val_len = len - eq_pos - 1;

        // Key matching with length guard: memcmp only called when lengths match.
        // This eliminates prefix collisions and buffer overread from str_eq.
        auto key_matches = [key, key_len](const char* expected) -> bool {
            const size_t elen = __builtin_strlen(expected);
            return (key_len == elen) && __builtin_memcmp(key, expected, elen) == 0;
        };

        auto parse_int = [val_start, val_len]() -> int32_t {
            int32_t val = 0;
            for (size_t i = 0; i < val_len; ++i) {
                if (val_start[i] >= '0' && val_start[i] <= '9')
                    val = val * 10 + (val_start[i] - '0');
            }
            return val;
        };

        auto parse_bool = [val_start, val_len]() -> bool {
            return (val_len == 4 &&
                    val_start[0] == 't' && val_start[1] == 'r' &&
                    val_start[2] == 'u' && val_start[3] == 'e');
        };

        if (key_matches("sweep_start_mhz")) {
            settings_.sweep_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("sweep_end_mhz")) {
            settings_.sweep_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("sweep_step_khz")) {
            settings_.sweep_step_freq = static_cast<uint64_t>(parse_int()) * 1000ULL;
        } else if (key_matches("scan_interval_ms")) {
            settings_.scan_interval_ms = static_cast<uint32_t>(parse_int());
        } else if (key_matches("sensitivity")) {
            const int32_t sens = parse_int();
            settings_.scan_sensitivity = static_cast<uint8_t>(sens > 100 ? 100 : (sens < 0 ? 0 : sens));
            // Convert sensitivity to dBm: dbm = -120 + sens * 4 / 5
            settings_.alert_rssi_threshold_dbm = -120 + (settings_.scan_sensitivity * 4 / 5);
        } else if (key_matches("rssi_threshold_db")) {
            bool negative = (val_len > 0 && val_start[0] == '-');
            const uint8_t* num_start = negative ? val_start + 1 : val_start;
            size_t num_len = negative ? val_len - 1 : val_len;
            int32_t val = 0;
            for (size_t i = 0; i < num_len; ++i) {
                if (num_start[i] >= '0' && num_start[i] <= '9')
                    val = val * 10 + (num_start[i] - '0');
            }
            settings_.alert_rssi_threshold_dbm = negative ? -val : val;
        } else if (key_matches("enable_audio_alerts")) {
            settings_.audio_alerts_enabled = parse_bool();
        } else if (key_matches("volume")) {
            const uint8_t vol = static_cast<uint8_t>(parse_int());
            if (vol <= 99) {
                portapack::receiver_model.set_normalized_headphone_volume(vol);
            }
        } else if (key_matches("show_spectrum")) {
            settings_.spectrum_visible = parse_bool();
        } else if (key_matches("show_histogram")) {
            settings_.histogram_visible = parse_bool();
        }
    };

    while (true) {
        const auto read_result = file.read(chunk, READ_CHUNK_SIZE);
        if (!read_result.is_ok() || read_result.value() == 0) {
            break;
        }

        const size_t bytes_read = read_result.value();

        for (size_t i = 0; i < bytes_read; ++i) {
            const char c = static_cast<char>(chunk[i]);

            if (c == '\r' || c == '\n') {
                parse_line(line_buf, line_len);
                line_len = 0;
            } else if (line_len < sizeof(line_buf) - 1) {
                line_buf[line_len++] = chunk[i];
            }
        }
    }

    // Process final line if file doesn't end with newline
    parse_line(line_buf, line_len);

    file.close();

    // Apply loaded settings to UI
    apply_settings();

    // Update scanner config if scanner is available
    if (scanner_ptr_ != nullptr) {
        ScanConfig updated_config = original_config_;
        updated_config.mode = settings_.scanning_mode;
        updated_config.scan_interval_ms = settings_.scan_interval_ms;
        updated_config.rssi_threshold_dbm = settings_.alert_rssi_threshold_dbm;
        updated_config.sweep_start_freq = settings_.sweep_start_freq;
        updated_config.sweep_end_freq = settings_.sweep_end_freq;
        updated_config.sweep_step_freq = settings_.sweep_step_freq;
        updated_config.dwell_enabled = settings_.dwell_enabled;
        updated_config.confirm_count_enabled = settings_.confirm_count_enabled;
        updated_config.noise_blacklist_enabled = settings_.noise_blacklist_enabled;

        const ErrorCode err = scanner_ptr_->set_config(updated_config);
        if (err != ErrorCode::SUCCESS) {
            return err;
        }
    }

    return ErrorCode::SUCCESS;
}

ErrorCode DroneSettingsView::validate_settings() const noexcept {
    if (static_cast<uint8_t>(settings_.scanning_mode) >= SCANNING_MODE_COUNT) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (settings_.scan_interval_ms < 10 || settings_.scan_interval_ms > 1000) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (settings_.scan_sensitivity > 100) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (settings_.alert_rssi_threshold_dbm < RSSI_MIN_DBM ||
        settings_.alert_rssi_threshold_dbm > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (settings_.min_threat_level > ThreatLevel::CRITICAL) {
        return ErrorCode::INVALID_PARAMETER;
    }

    if (settings_.sweep_start_freq < MIN_FREQUENCY_HZ ||
        settings_.sweep_end_freq > MAX_FREQUENCY_HZ ||
        settings_.sweep_start_freq >= settings_.sweep_end_freq) {
        return ErrorCode::INVALID_PARAMETER;
    }

    return ErrorCode::SUCCESS;
}

void DroneSettingsView::apply_settings() noexcept {
    field_scan_interval_.set_value(settings_.scan_interval_ms);
    // Convert dBm threshold to sensitivity: sens = (dbm + 120) * 5 / 4
    {
        const int32_t sens = (settings_.alert_rssi_threshold_dbm + 120) * 5 / 4;
        field_rssi_threshold_.set_value(sens < 0 ? 0 : (sens > 100 ? 100 : sens));
    }
    field_volume_.set_value(portapack::receiver_model.normalized_headphone_volume());
    check_audio_alerts_.set_value(settings_.audio_alerts_enabled);
    check_spectrum_visible_.set_value(settings_.spectrum_visible);
    check_histogram_visible_.set_value(settings_.histogram_visible);
    check_dwell_enabled_.set_value(settings_.dwell_enabled);
    check_confirm_count_.set_value(settings_.confirm_count_enabled);
    check_noise_blacklist_.set_value(settings_.noise_blacklist_enabled);
    field_sweep_start_.set_value(static_cast<int32_t>(settings_.sweep_start_freq / 1000000));
    field_sweep_end_.set_value(static_cast<int32_t>(settings_.sweep_end_freq / 1000000));
    field_sweep_step_.set_value(static_cast<int32_t>(settings_.sweep_step_freq / 1000));
}

} // namespace drone_analyzer