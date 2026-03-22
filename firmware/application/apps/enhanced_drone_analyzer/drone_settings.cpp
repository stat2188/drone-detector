#include "drone_settings.hpp"
#include "scanner.hpp"
#include "audio_alerts.hpp"
#include "drone_display.hpp"
#include "ui_receiver.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include <cstring>

namespace drone_analyzer {

// ============================================================================
// DroneSettings Implementation
// ============================================================================

DroneSettings::DroneSettings() noexcept
    : scanning_mode(DEFAULT_SCANNING_MODE)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , scan_sensitivity(50)
    , spectrum_visible(true)
    , histogram_visible(true)
    , drone_list_visible(true)
    , status_bar_visible(true)
    , audio_alerts_enabled(false)
    , alert_rssi_threshold_dbm(RSSI_HIGH_THREAT_THRESHOLD_DBM)
    , min_threat_level(ThreatLevel::LOW)
    , spectrum_start_freq(DEFAULT_SPECTRUM_START_HZ)
    , spectrum_end_freq(DEFAULT_SPECTRUM_END_HZ)
    , histogram_start_freq(DEFAULT_HISTOGRAM_START_HZ)
    , histogram_end_freq(DEFAULT_HISTOGRAM_END_HZ)
    , sweep_start_freq(SWEEP_DEFAULT_START_HZ)
    , sweep_end_freq(SWEEP_DEFAULT_END_HZ)
    , sweep_step_freq(1000000) {  // Default 1 MHz step
}

void DroneSettings::reset_to_defaults() noexcept {
    scanning_mode = DEFAULT_SCANNING_MODE;
    scan_interval_ms = SCAN_CYCLE_INTERVAL_MS;
    scan_sensitivity = 50;
    
    spectrum_visible = true;
    histogram_visible = true;
    drone_list_visible = true;
    status_bar_visible = true;
    
    audio_alerts_enabled = false;
    alert_rssi_threshold_dbm = RSSI_HIGH_THREAT_THRESHOLD_DBM;
    
    min_threat_level = ThreatLevel::LOW;
    
    spectrum_start_freq = DEFAULT_SPECTRUM_START_HZ;
    spectrum_end_freq = DEFAULT_SPECTRUM_END_HZ;
    histogram_start_freq = DEFAULT_HISTOGRAM_START_HZ;
    histogram_end_freq = DEFAULT_HISTOGRAM_END_HZ;
    sweep_start_freq = SWEEP_DEFAULT_START_HZ;
    sweep_end_freq = SWEEP_DEFAULT_END_HZ;
    sweep_step_freq = 1000000;  // 1 MHz
}

// ============================================================================
// DroneSettingsView Constructor / Destructor
// ============================================================================

DroneSettingsView::DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display) noexcept
    : ui::View()
    , labels_({
        {{UI_POS_X(1), UI_POS_Y(1)}, "Scan Mode:", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(3)}, "Interval (ms):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(5)}, "RSSI (dBm):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(9)}, "Spec(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(7), UI_POS_Y(9)}, "-", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(11)}, "Hist(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(7), UI_POS_Y(11)}, "-", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(13)}, "Sweep(MHz):", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(8), UI_POS_Y(13)}, "-", Theme::getInstance()->fg_light->foreground},
        {{UI_POS_X(1), UI_POS_Y(15)}, "Step(kHz):", Theme::getInstance()->fg_light->foreground},
    })
    , field_scan_mode_({UI_POS_X(1), UI_POS_Y(2)}, 14, {
        {"Single", 0},
        {"Hopping", 1},
        {"Sequential", 2},
        {"Targeted", 3}
    })
    , field_scan_interval_({UI_POS_X(1), UI_POS_Y(4)}, 4, {10, 1000}, 10, ' ')
    , field_rssi_threshold_({UI_POS_X(1), UI_POS_Y(6)}, 4, {-90, -20}, 1, ' ')
    , check_audio_alerts_({UI_POS_X(1), UI_POS_Y(7)}, 15, "Audio Alerts", false)
    , check_spectrum_visible_({UI_POS_X(13), UI_POS_Y(7)}, 10, "Spectrum", false)
    , check_histogram_visible_({UI_POS_X(1), UI_POS_Y(8)}, 15, "Histogram", false)
    , field_spectrum_start_({UI_POS_X(1), UI_POS_Y(10)}, 5, {1, 7200}, 1, ' ')
    , field_spectrum_end_({UI_POS_X(8), UI_POS_Y(10)}, 5, {1, 7200}, 1, ' ')
    , field_histogram_start_({UI_POS_X(1), UI_POS_Y(12)}, 5, {1, 7200}, 1, ' ')
    , field_histogram_end_({UI_POS_X(8), UI_POS_Y(12)}, 5, {1, 7200}, 1, ' ')
    , field_sweep_start_({UI_POS_X(1), UI_POS_Y(14)}, 5, {100, 7200}, 1, ' ')
    , field_sweep_end_({UI_POS_X(9), UI_POS_Y(14)}, 5, {100, 7200}, 1, ' ')
    , field_sweep_step_({UI_POS_X(1), UI_POS_Y(16)}, 5, {10, 9999}, 10, ' ')
    , button_defaults_({UI_POS_X(0), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(10), 28}, "DEFAULTS")
    , button_save_({UI_POS_X_CENTER(7), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(7), 28}, "SAVE")
    , button_cancel_({UI_POS_X_RIGHT(7), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(7), 28}, "CANCEL")
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , display_ptr_(display)
    , original_config_(config)
    , settings_()
    , settings_dirty_(false) {
    settings_.scanning_mode = config.mode;
    settings_.scan_interval_ms = config.scan_interval_ms;
    settings_.alert_rssi_threshold_dbm = config.rssi_threshold_dbm;

    add_children({
        &labels_,
        &field_scan_mode_,
        &field_scan_interval_,
        &field_rssi_threshold_,
        &check_audio_alerts_,
        &check_spectrum_visible_,
        &check_histogram_visible_,
        &field_spectrum_start_,
        &field_spectrum_end_,
        &field_histogram_start_,
        &field_histogram_end_,
        &field_sweep_start_,
        &field_sweep_end_,
        &field_sweep_step_,
        &button_defaults_,
        &button_save_,
        &button_cancel_
    });

    field_scan_mode_.set_by_value(static_cast<int32_t>(settings_.scanning_mode));
    field_scan_interval_.set_value(settings_.scan_interval_ms);
    field_rssi_threshold_.set_value(settings_.alert_rssi_threshold_dbm);
    check_audio_alerts_.set_value(settings_.audio_alerts_enabled);
    check_spectrum_visible_.set_value(settings_.spectrum_visible);
    check_histogram_visible_.set_value(settings_.histogram_visible);
    
    // Set frequency range fields (convert Hz to MHz)
    field_spectrum_start_.set_value(static_cast<int32_t>(settings_.spectrum_start_freq / 1000000));
    field_spectrum_end_.set_value(static_cast<int32_t>(settings_.spectrum_end_freq / 1000000));
    field_histogram_start_.set_value(static_cast<int32_t>(settings_.histogram_start_freq / 1000000));
    field_histogram_end_.set_value(static_cast<int32_t>(settings_.histogram_end_freq / 1000000));

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
            updated_config.spectrum_start_freq = settings_.spectrum_start_freq;
            updated_config.spectrum_end_freq = settings_.spectrum_end_freq;
            updated_config.histogram_start_freq = settings_.histogram_start_freq;
            updated_config.histogram_end_freq = settings_.histogram_end_freq;
            updated_config.sweep_start_freq = settings_.sweep_start_freq;
            updated_config.sweep_end_freq = settings_.sweep_end_freq;
            updated_config.sweep_step_freq = settings_.sweep_step_freq;

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
                "rssi_threshold_db=%ld\n", (long)settings_.alert_rssi_threshold_dbm);
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "enable_audio_alerts=%s\n", settings_.audio_alerts_enabled ? "true" : "false");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "show_spectrum=%s\n", settings_.spectrum_visible ? "true" : "false");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "show_histogram=%s\n", settings_.histogram_visible ? "true" : "false");
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "spectrum_start_mhz=%lu\n", (unsigned long)(settings_.spectrum_start_freq / 1000000));
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "spectrum_end_mhz=%lu\n", (unsigned long)(settings_.spectrum_end_freq / 1000000));
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "histogram_start_mhz=%lu\n", (unsigned long)(settings_.histogram_start_freq / 1000000));
            offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                "histogram_end_mhz=%lu\n", (unsigned long)(settings_.histogram_end_freq / 1000000));
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
        settings_dirty_ = true;
        apply_settings();
    };

    field_scan_mode_.on_change = [this](size_t, int32_t v) {
        settings_.scanning_mode = static_cast<ScanningMode>(v);
        settings_dirty_ = true;
    };

    field_scan_interval_.on_change = [this](int32_t v) {
        settings_.scan_interval_ms = static_cast<uint32_t>(v);
        settings_dirty_ = true;
    };

    field_rssi_threshold_.on_change = [this](int32_t v) {
        settings_.alert_rssi_threshold_dbm = v;
        settings_dirty_ = true;
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
    
    field_spectrum_start_.on_change = [this](int32_t v) {
        settings_.spectrum_start_freq = static_cast<FreqHz>(v) * 1000000;
        settings_dirty_ = true;
    };
    
    field_spectrum_end_.on_change = [this](int32_t v) {
        settings_.spectrum_end_freq = static_cast<FreqHz>(v) * 1000000;
        settings_dirty_ = true;
    };
    
    field_histogram_start_.on_change = [this](int32_t v) {
        settings_.histogram_start_freq = static_cast<FreqHz>(v) * 1000000;
        settings_dirty_ = true;
    };
    
    field_histogram_end_.on_change = [this](int32_t v) {
        settings_.histogram_end_freq = static_cast<FreqHz>(v) * 1000000ULL;
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
    field_spectrum_start_.on_select = open_keypad;
    field_spectrum_end_.on_select = open_keypad;
    field_histogram_start_.on_select = open_keypad;
    field_histogram_end_.on_select = open_keypad;
    field_sweep_start_.on_select = open_keypad;
    field_sweep_end_.on_select = open_keypad;
}

DroneSettingsView::~DroneSettingsView() noexcept {
}

void DroneSettingsView::paint(ui::Painter& painter) {
    (void)painter;
}

void DroneSettingsView::focus() {
    field_scan_mode_.focus();
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
            {&field_spectrum_start_}, {&field_spectrum_end_},
            {&field_histogram_start_}, {&field_histogram_end_},
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

        if (key_matches("spectrum_start_mhz")) {
            settings_.spectrum_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("spectrum_end_mhz")) {
            settings_.spectrum_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("histogram_start_mhz")) {
            settings_.histogram_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("histogram_end_mhz")) {
            settings_.histogram_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("sweep_start_mhz")) {
            settings_.sweep_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("sweep_end_mhz")) {
            settings_.sweep_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
        } else if (key_matches("sweep_step_khz")) {
            settings_.sweep_step_freq = static_cast<uint64_t>(parse_int()) * 1000ULL;
        } else if (key_matches("scan_interval_ms")) {
            settings_.scan_interval_ms = static_cast<uint32_t>(parse_int());
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
        updated_config.spectrum_start_freq = settings_.spectrum_start_freq;
        updated_config.spectrum_end_freq = settings_.spectrum_end_freq;
        updated_config.histogram_start_freq = settings_.histogram_start_freq;
        updated_config.histogram_end_freq = settings_.histogram_end_freq;
        updated_config.sweep_start_freq = settings_.sweep_start_freq;
        updated_config.sweep_end_freq = settings_.sweep_end_freq;
        updated_config.sweep_step_freq = settings_.sweep_step_freq;

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
    
    if (settings_.spectrum_start_freq < MIN_FREQUENCY_HZ ||
        settings_.spectrum_start_freq > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (settings_.spectrum_end_freq < MIN_FREQUENCY_HZ ||
        settings_.spectrum_end_freq > MAX_FREQUENCY_HZ ||
        settings_.spectrum_end_freq <= settings_.spectrum_start_freq) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (settings_.histogram_start_freq < MIN_FREQUENCY_HZ ||
        settings_.histogram_start_freq > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (settings_.histogram_end_freq < MIN_FREQUENCY_HZ ||
        settings_.histogram_end_freq > MAX_FREQUENCY_HZ ||
        settings_.histogram_end_freq <= settings_.histogram_start_freq) {
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
    field_scan_mode_.set_by_value(static_cast<int32_t>(settings_.scanning_mode));
    field_scan_interval_.set_value(settings_.scan_interval_ms);
    field_rssi_threshold_.set_value(settings_.alert_rssi_threshold_dbm);
    check_audio_alerts_.set_value(settings_.audio_alerts_enabled);
    check_spectrum_visible_.set_value(settings_.spectrum_visible);
    check_histogram_visible_.set_value(settings_.histogram_visible);
    field_spectrum_start_.set_value(static_cast<int32_t>(settings_.spectrum_start_freq / 1000000));
    field_spectrum_end_.set_value(static_cast<int32_t>(settings_.spectrum_end_freq / 1000000));
    field_histogram_start_.set_value(static_cast<int32_t>(settings_.histogram_start_freq / 1000000));
    field_histogram_end_.set_value(static_cast<int32_t>(settings_.histogram_end_freq / 1000000));
    field_sweep_start_.set_value(static_cast<int32_t>(settings_.sweep_start_freq / 1000000));
    field_sweep_end_.set_value(static_cast<int32_t>(settings_.sweep_end_freq / 1000000));
    field_sweep_step_.set_value(static_cast<int32_t>(settings_.sweep_step_freq / 1000));
}

} // namespace drone_analyzer