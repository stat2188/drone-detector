#include "drone_sweep_view.hpp"
#include "scanner.hpp"
#include "constants.hpp"
#include "ui_receiver.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include <cstring>

namespace drone_analyzer {

// ============================================================================
// Sweep1View
// ============================================================================

Sweep1View::Sweep1View(Rect parent_rect) noexcept
    : View(parent_rect) {
    hidden(true);
    add_children({
        &labels_,
        &field_start_,
        &field_end_,
        &field_step_,
    });

    field_start_.set_value(START_MHZ_DEFAULT);
    field_end_.set_value(END_MHZ_DEFAULT);
    field_step_.set_value(STEP_KHZ_DEFAULT);
}

void Sweep1View::focus() {
    field_start_.focus();
}

void Sweep1View::set_values(FreqHz start, FreqHz end, FreqHz step) noexcept {
    field_start_.set_value(static_cast<int32_t>(start / 1000000ULL));
    field_end_.set_value(static_cast<int32_t>(end / 1000000ULL));
    field_step_.set_value(static_cast<int32_t>(step / 1000ULL));
}

FreqHz Sweep1View::get_start() const noexcept {
    return static_cast<FreqHz>(field_start_.value()) * 1000000ULL;
}

FreqHz Sweep1View::get_end() const noexcept {
    return static_cast<FreqHz>(field_end_.value()) * 1000000ULL;
}

FreqHz Sweep1View::get_step() const noexcept {
    return static_cast<FreqHz>(field_step_.value()) * 1000ULL;
}

// ============================================================================
// Sweep2View
// ============================================================================

Sweep2View::Sweep2View(Rect parent_rect) noexcept
    : View(parent_rect) {
    hidden(true);
    add_children({
        &check_enabled_,
        &labels_,
        &field_start_,
        &field_end_,
        &field_step_,
    });

    field_start_.set_value(START_MHZ_DEFAULT);
    field_end_.set_value(END_MHZ_DEFAULT);
    field_step_.set_value(STEP_KHZ_DEFAULT);
}

void Sweep2View::focus() {
    check_enabled_.focus();
}

void Sweep2View::set_values(bool enabled, FreqHz start, FreqHz end, FreqHz step) noexcept {
    check_enabled_.set_value(enabled);
    field_start_.set_value(static_cast<int32_t>(start / 1000000ULL));
    field_end_.set_value(static_cast<int32_t>(end / 1000000ULL));
    field_step_.set_value(static_cast<int32_t>(step / 1000ULL));
}

bool Sweep2View::get_enabled() const noexcept {
    return check_enabled_.value();
}

FreqHz Sweep2View::get_start() const noexcept {
    return static_cast<FreqHz>(field_start_.value()) * 1000000ULL;
}

FreqHz Sweep2View::get_end() const noexcept {
    return static_cast<FreqHz>(field_end_.value()) * 1000000ULL;
}

FreqHz Sweep2View::get_step() const noexcept {
    return static_cast<FreqHz>(field_step_.value()) * 1000ULL;
}

// ============================================================================
// DroneSweepView
// ============================================================================

DroneSweepView::DroneSweepView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr) noexcept
    : ui::View()
    , nav_(nav)
    , scanner_ptr_(scanner_ptr)
    , original_config_(config) {
    add_children({
        &tab_view_,
        &view_sw1_,
        &view_sw2_,
        &button_defaults_,
        &button_save_,
    });

    // Populate SW1 tab from config
    view_sw1_.set_values(config.sweep_start_freq, config.sweep_end_freq, config.sweep_step_freq);

    // Populate SW2 tab from config
    view_sw2_.set_values(config.sweep2_enabled, config.sweep2_start_freq, config.sweep2_end_freq, config.sweep2_step_freq);

    button_save_.on_select = [this](ui::Button&) {
        save_settings();
        nav_.pop();
    };

    button_defaults_.on_select = [this](ui::Button&) {
        apply_defaults();
    };
}

DroneSweepView::~DroneSweepView() noexcept {
}

void DroneSweepView::focus() {
    tab_view_.focus();
}

void DroneSweepView::save_settings() noexcept {
    // Read values from UI tabs
    FreqHz sw1_start = view_sw1_.get_start();
    FreqHz sw1_end = view_sw1_.get_end();
    FreqHz sw1_step = view_sw1_.get_step();

    bool sw2_enabled = view_sw2_.get_enabled();
    FreqHz sw2_start = view_sw2_.get_start();
    FreqHz sw2_end = view_sw2_.get_end();
    FreqHz sw2_step = view_sw2_.get_step();

    // Validate: start must be < end
    if (sw1_start >= sw1_end) {
        sw1_end = sw1_start + 20000000;
    }
    if (sw2_enabled && sw2_start >= sw2_end) {
        sw2_end = sw2_start + 20000000;
    }

    // Update scanner config
    if (scanner_ptr_ != nullptr) {
        ScanConfig updated_config = original_config_;
        updated_config.sweep_start_freq = sw1_start;
        updated_config.sweep_end_freq = sw1_end;
        updated_config.sweep_step_freq = sw1_step;
        updated_config.sweep2_start_freq = sw2_start;
        updated_config.sweep2_end_freq = sw2_end;
        updated_config.sweep2_step_freq = sw2_step;
        updated_config.sweep2_enabled = sw2_enabled;
        (void)scanner_ptr_->set_config(updated_config);
    }

    // Read existing file, update sweep keys, write back
    File file;
    ensure_directory(settings_dir);

    // Read existing file content into static buffer
    static char file_buf[2048];
    size_t file_len = 0;
    const auto open_result = file.open(settings_dir / u"eda_settings.txt", true, false);
    if (open_result) {
        // File exists, read it
        constexpr size_t CHUNK = 256;
        uint8_t chunk[CHUNK];
        while (true) {
            const auto read_result = file.read(chunk, CHUNK);
            if (!read_result.is_ok() || read_result.value() == 0) break;
            const size_t bytes = read_result.value();
            for (size_t i = 0; i < bytes && file_len < sizeof(file_buf) - 1; ++i) {
                file_buf[file_len++] = static_cast<char>(chunk[i]);
            }
        }
        file.close();
    }
    file_buf[file_len] = '\0';

    // Create/overwrite file
    const auto create_result = file.create(settings_dir / u"eda_settings.txt");
    if (!create_result) return;

    // Write output: non-sweep lines from existing file, plus sweep keys
    static char out_buf[2048];
    size_t out_offset = 0;

    // If file didn't exist, write defaults for non-sweep settings
    if (file_len == 0) {
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "# Enhanced Drone Analyzer Settings\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "# Auto-generated by EDA\n\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "spectrum_mode=SEQUENTIAL\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "scan_interval_ms=%lu\n", (unsigned long)SCAN_CYCLE_INTERVAL_MS);
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "sensitivity=70\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "rssi_threshold_db=%ld\n", (long)RSSI_DETECTION_THRESHOLD_DBM);
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "enable_audio_alerts=true\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "volume=70\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "show_spectrum=true\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "show_histogram=true\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "spectrum_detection=false\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "spectrum_margin=15\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "spectrum_min_width=1\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "dwell_enabled=false\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "confirm_count_enabled=false\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "noise_blacklist_enabled=false\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "median_enabled=false\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "freqman_path=DRONES\n");
        out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
            "settings_version=1.0\n");
    } else {
        // Parse existing file and copy non-sweep lines
        size_t line_start = 0;
        for (size_t i = 0; i <= file_len; ++i) {
            if (i == file_len || file_buf[i] == '\n' || file_buf[i] == '\r') {
                if (i > line_start) {
                    // Check if this is a sweep line to skip
                    bool is_sweep_line = false;
                    const char* sweep_keys[] = {
                        "sweep_start_mhz=", "sweep_end_mhz=", "sweep_step_khz=",
                        "sweep2_start_mhz=", "sweep2_end_mhz=", "sweep2_step_khz=",
                        "sweep2_enabled=",
                    };
                    for (const auto& key : sweep_keys) {
                        const size_t klen = __builtin_strlen(key);
                        if ((i - line_start) >= klen &&
                            __builtin_memcmp(file_buf + line_start, key, klen) == 0) {
                            is_sweep_line = true;
                            break;
                        }
                    }
                    if (!is_sweep_line && (i - line_start) > 0) {
                        const size_t copy_len = i - line_start;
                        if (out_offset + copy_len + 1 < sizeof(out_buf)) {
                            __builtin_memcpy(out_buf + out_offset, file_buf + line_start, copy_len);
                            out_offset += copy_len;
                            out_buf[out_offset++] = '\n';
                        }
                    }
                }
                // Skip \r\n pairs
                if (i < file_len && file_buf[i] == '\r' && (i + 1) < file_len && file_buf[i + 1] == '\n') {
                    i++;
                }
                line_start = i + 1;
            }
        }
    }

    // Append sweep keys
    out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
        "sweep_start_mhz=%lu\n", (unsigned long)(sw1_start / 1000000));
    out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
        "sweep_end_mhz=%lu\n", (unsigned long)(sw1_end / 1000000));
    out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
        "sweep_step_khz=%lu\n", (unsigned long)(sw1_step / 1000));
    out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
        "sweep2_start_mhz=%lu\n", (unsigned long)(sw2_start / 1000000));
    out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
        "sweep2_end_mhz=%lu\n", (unsigned long)(sw2_end / 1000000));
    out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
        "sweep2_step_khz=%lu\n", (unsigned long)(sw2_step / 1000));
    out_offset += snprintf(out_buf + out_offset, sizeof(out_buf) - out_offset,
        "sweep2_enabled=%s\n", sw2_enabled ? "true" : "false");

    file.write(out_buf, out_offset);
    file.close();
}

void DroneSweepView::apply_defaults() noexcept {
    view_sw1_.set_values(
        SWEEP_DEFAULT_START_HZ,
        SWEEP_DEFAULT_END_HZ,
        20000000
    );
    view_sw2_.set_values(
        false,
        2400000000ULL,
        2500000000ULL,
        20000000
    );
}

} // namespace drone_analyzer
