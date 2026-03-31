#include <cstdint>
#include <cstring>
#include <cstdio>

#include "settings_manager.hpp"
#include "scanner.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "receiver_model.hpp"
#include "portapack.hpp"

namespace drone_analyzer {

// ============================================================================
// SettingsStruct Implementation
// ============================================================================

SettingsStruct::SettingsStruct() noexcept
    : scanning_mode(DEFAULT_SCANNING_MODE)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , scan_sensitivity(70)
    , alert_rssi_threshold_dbm(RSSI_DETECTION_THRESHOLD_DBM)
    , spectrum_visible(true)
    , histogram_visible(true)
    , audio_alerts_enabled(true)
    , dwell_enabled(true)
    , confirm_count_enabled(true)
    , noise_blacklist_enabled(true)
    , spectrum_detection_enabled(true)
    , median_enabled(true)
    , spectrum_margin(DEFAULT_SPECTRUM_MARGIN)
    , spectrum_min_width(DEFAULT_SPECTRUM_MIN_WIDTH)
    , spectrum_max_width(DEFAULT_SPECTRUM_MAX_WIDTH)
    , spectrum_peak_sharpness(DEFAULT_SPECTRUM_PEAK_SHARPNESS)
    , spectrum_peak_ratio(DEFAULT_SPECTRUM_PEAK_RATIO)
    , spectrum_valley_depth(DEFAULT_SPECTRUM_VALLEY_DEPTH)
    , neighbor_margin_db(DEFAULT_NEIGHBOR_MARGIN_DB)
    , rssi_variance_enabled(false)
    , confirm_count(DEFAULT_CONFIRM_COUNT)
    , sweep_start_freq(SWEEP_DEFAULT_START_HZ)
    , sweep_end_freq(SWEEP_DEFAULT_END_HZ)
    , sweep_step_freq(20000000)
    , sweep2_start_freq(2400000000ULL)
    , sweep2_end_freq(2500000000ULL)
    , sweep2_step_freq(20000000)
    , sweep2_enabled(false)
    , sweep3_start_freq(900000000ULL)
    , sweep3_end_freq(1000000000ULL)
    , sweep3_step_freq(20000000)
    , sweep3_enabled(false)
    , sweep4_start_freq(1200000000ULL)
    , sweep4_end_freq(1300000000ULL)
    , sweep4_step_freq(20000000)
    , sweep4_enabled(false) {
}

// ============================================================================
// Unified Parser — single implementation for all key=value parsing
// ============================================================================

static void parse_settings_line(
    const uint8_t* buf,
    size_t len,
    SettingsStruct& s
) noexcept {
    if (len == 0 || buf[0] == '#') return;

    size_t eq_pos = 0;
    for (size_t i = 0; i < len; ++i) {
        if (buf[i] == '=') { eq_pos = i; break; }
    }
    if (eq_pos == 0 || eq_pos >= len - 1) return;

    char key[32];
    size_t key_len = eq_pos;
    if (key_len > 31) key_len = 31;
    for (size_t i = 0; i < key_len; ++i) {
        key[i] = static_cast<char>(buf[i]);
    }
    key[key_len] = '\0';

    const uint8_t* val_start = buf + eq_pos + 1;
    size_t val_len = len - eq_pos - 1;

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

    // --- Scanning ---
    if (key_matches("scan_interval_ms")) {
        s.scan_interval_ms = static_cast<uint32_t>(parse_int());
    } else if (key_matches("sensitivity")) {
        const int32_t sens = parse_int();
        s.scan_sensitivity = static_cast<uint8_t>(sens > 100 ? 100 : (sens < 0 ? 0 : sens));
        s.alert_rssi_threshold_dbm = -20 - s.scan_sensitivity;
    } else if (key_matches("rssi_threshold_db")) {
        bool negative = (val_len > 0 && val_start[0] == '-');
        const uint8_t* num_start = negative ? val_start + 1 : val_start;
        size_t num_len = negative ? val_len - 1 : val_len;
        int32_t val = 0;
        for (size_t i = 0; i < num_len; ++i) {
            if (num_start[i] >= '0' && num_start[i] <= '9')
                val = val * 10 + (num_start[i] - '0');
        }
        s.alert_rssi_threshold_dbm = negative ? -val : val;

    // --- Audio / Display ---
    } else if (key_matches("enable_audio_alerts")) {
        s.audio_alerts_enabled = parse_bool();
    } else if (key_matches("volume")) {
        const uint8_t vol = static_cast<uint8_t>(parse_int());
        if (vol <= 99) {
            portapack::receiver_model.set_normalized_headphone_volume(vol);
        }
    } else if (key_matches("show_spectrum")) {
        s.spectrum_visible = parse_bool();
    } else if (key_matches("show_histogram")) {
        s.histogram_visible = parse_bool();

    // --- Detection features ---
    } else if (key_matches("spectrum_detection")) {
        s.spectrum_detection_enabled = parse_bool();
    } else if (key_matches("dwell_enabled")) {
        s.dwell_enabled = parse_bool();
    } else if (key_matches("confirm_count_enabled")) {
        s.confirm_count_enabled = parse_bool();
    } else if (key_matches("noise_blacklist_enabled")) {
        s.noise_blacklist_enabled = parse_bool();
    } else if (key_matches("median_enabled")) {
        s.median_enabled = parse_bool();

    // --- Spectrum shape filter ---
    } else if (key_matches("spectrum_margin")) {
        s.spectrum_margin = static_cast<uint8_t>(parse_int());
    } else if (key_matches("spectrum_min_width")) {
        s.spectrum_min_width = static_cast<uint8_t>(parse_int());
    } else if (key_matches("spectrum_max_width")) {
        s.spectrum_max_width = static_cast<uint8_t>(parse_int());
    } else if (key_matches("spectrum_peak_sharpness")) {
        s.spectrum_peak_sharpness = static_cast<uint8_t>(parse_int());
    } else if (key_matches("spectrum_peak_ratio")) {
        s.spectrum_peak_ratio = static_cast<uint8_t>(parse_int());
    } else if (key_matches("spectrum_valley_depth")) {
        s.spectrum_valley_depth = static_cast<uint8_t>(parse_int());

    // --- Anti-false-positive ---
    } else if (key_matches("neighbor_margin_db")) {
        s.neighbor_margin_db = parse_int();
    } else if (key_matches("rssi_variance_enabled")) {
        s.rssi_variance_enabled = parse_bool();
    } else if (key_matches("confirm_count")) {
        s.confirm_count = static_cast<uint8_t>(parse_int());

    // --- Sweep window 1 ---
    } else if (key_matches("sweep_start_mhz")) {
        s.sweep_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep_end_mhz")) {
        s.sweep_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep_step_khz")) {
        s.sweep_step_freq = static_cast<uint64_t>(parse_int()) * 1000ULL;

    // --- Sweep window 2 ---
    } else if (key_matches("sweep2_start_mhz")) {
        s.sweep2_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep2_end_mhz")) {
        s.sweep2_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep2_step_khz")) {
        s.sweep2_step_freq = static_cast<uint64_t>(parse_int()) * 1000ULL;
    } else if (key_matches("sweep2_enabled")) {
        s.sweep2_enabled = parse_bool();

    // --- Sweep window 3 ---
    } else if (key_matches("sweep3_start_mhz")) {
        s.sweep3_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep3_end_mhz")) {
        s.sweep3_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep3_step_khz")) {
        s.sweep3_step_freq = static_cast<uint64_t>(parse_int()) * 1000ULL;
    } else if (key_matches("sweep3_enabled")) {
        s.sweep3_enabled = parse_bool();

    // --- Sweep window 4 ---
    } else if (key_matches("sweep4_start_mhz")) {
        s.sweep4_start_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep4_end_mhz")) {
        s.sweep4_end_freq = static_cast<uint64_t>(parse_int()) * 1000000ULL;
    } else if (key_matches("sweep4_step_khz")) {
        s.sweep4_step_freq = static_cast<uint64_t>(parse_int()) * 1000ULL;
    } else if (key_matches("sweep4_enabled")) {
        s.sweep4_enabled = parse_bool();
    }
}

// ============================================================================
// SettingsFileManager::load
// ============================================================================

ErrorCode SettingsFileManager::load(SettingsStruct& out) noexcept {
    File file;
    const auto open_result = file.open(settings_dir / u"eda_settings.txt", true, false);
    if (!open_result) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }

    constexpr size_t READ_CHUNK_SIZE = 256;
    uint8_t chunk[READ_CHUNK_SIZE];
    uint8_t line_buf[128];
    size_t line_len = 0;

    while (true) {
        const auto read_result = file.read(chunk, READ_CHUNK_SIZE);
        if (!read_result.is_ok() || read_result.value() == 0) break;

        const size_t bytes_read = read_result.value();
        for (size_t i = 0; i < bytes_read; ++i) {
            const char c = static_cast<char>(chunk[i]);
            if (c == '\r' || c == '\n') {
                parse_settings_line(line_buf, line_len, out);
                line_len = 0;
            } else if (line_len < sizeof(line_buf) - 1) {
                line_buf[line_len++] = chunk[i];
            }
        }
    }
    parse_settings_line(line_buf, line_len, out);

    file.close();
    return ErrorCode::SUCCESS;
}

// ============================================================================
// SettingsFileManager::save
// ============================================================================

ErrorCode SettingsFileManager::save(
    DroneScanner* scanner_ptr,
    const SettingsStruct& s
) noexcept {
    // Get sweep config from scanner if available
    ScanConfig sweep_cfg;
    if (scanner_ptr != nullptr) {
        sweep_cfg = scanner_ptr->get_config();
    } else {
        // Use sweep values from settings struct
        sweep_cfg.sweep_start_freq = s.sweep_start_freq;
        sweep_cfg.sweep_end_freq = s.sweep_end_freq;
        sweep_cfg.sweep_step_freq = s.sweep_step_freq;
        sweep_cfg.sweep2_start_freq = s.sweep2_start_freq;
        sweep_cfg.sweep2_end_freq = s.sweep2_end_freq;
        sweep_cfg.sweep2_step_freq = s.sweep2_step_freq;
        sweep_cfg.sweep2_enabled = s.sweep2_enabled;
        sweep_cfg.sweep3_start_freq = s.sweep3_start_freq;
        sweep_cfg.sweep3_end_freq = s.sweep3_end_freq;
        sweep_cfg.sweep3_step_freq = s.sweep3_step_freq;
        sweep_cfg.sweep3_enabled = s.sweep3_enabled;
        sweep_cfg.sweep4_start_freq = s.sweep4_start_freq;
        sweep_cfg.sweep4_end_freq = s.sweep4_end_freq;
        sweep_cfg.sweep4_step_freq = s.sweep4_step_freq;
        sweep_cfg.sweep4_enabled = s.sweep4_enabled;
    }

    File file;
    ensure_directory(settings_dir);
    const auto open_result = file.create(settings_dir / u"eda_settings.txt");
    if (!open_result) {
        return ErrorCode::INITIALIZATION_FAILED;
    }

    char buffer[1024];
    size_t offset = 0;

    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "# Enhanced Drone Analyzer Settings\n");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "# Auto-generated — do not edit while app is running\n\n");

    // Scanning
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_mode=SEQUENTIAL\n");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "scan_interval_ms=%lu\n", (unsigned long)s.scan_interval_ms);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sensitivity=%u\n", (unsigned)s.scan_sensitivity);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "rssi_threshold_db=%ld\n", (long)s.alert_rssi_threshold_dbm);

    // Audio / Display
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "enable_audio_alerts=%s\n", s.audio_alerts_enabled ? "true" : "false");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "volume=%u\n", (unsigned)portapack::receiver_model.normalized_headphone_volume());
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "show_spectrum=%s\n", s.spectrum_visible ? "true" : "false");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "show_histogram=%s\n", s.histogram_visible ? "true" : "false");

    // Detection features
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_detection=%s\n", s.spectrum_detection_enabled ? "true" : "false");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "dwell_enabled=%s\n", s.dwell_enabled ? "true" : "false");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "confirm_count_enabled=%s\n", s.confirm_count_enabled ? "true" : "false");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "noise_blacklist_enabled=%s\n", s.noise_blacklist_enabled ? "true" : "false");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "median_enabled=%s\n", s.median_enabled ? "true" : "false");

    // Spectrum shape filter
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_margin=%u\n", (unsigned)s.spectrum_margin);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_min_width=%u\n", (unsigned)s.spectrum_min_width);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_max_width=%u\n", (unsigned)s.spectrum_max_width);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_peak_sharpness=%u\n", (unsigned)s.spectrum_peak_sharpness);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_peak_ratio=%u\n", (unsigned)s.spectrum_peak_ratio);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "spectrum_valley_depth=%u\n", (unsigned)s.spectrum_valley_depth);

    // Anti-false-positive
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "neighbor_margin_db=%d\n", (int)s.neighbor_margin_db);
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "rssi_variance_enabled=%s\n", s.rssi_variance_enabled ? "true" : "false");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "confirm_count=%u\n", (unsigned)s.confirm_count);

    // Sweep window 1
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep_start_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep_start_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep_end_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep_end_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep_step_khz=%lu\n", (unsigned long)(sweep_cfg.sweep_step_freq / 1000ULL));

    // Sweep window 2
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep2_start_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep2_start_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep2_end_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep2_end_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep2_step_khz=%lu\n", (unsigned long)(sweep_cfg.sweep2_step_freq / 1000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep2_enabled=%s\n", sweep_cfg.sweep2_enabled ? "true" : "false");

    // Sweep window 3
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep3_start_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep3_start_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep3_end_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep3_end_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep3_step_khz=%lu\n", (unsigned long)(sweep_cfg.sweep3_step_freq / 1000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep3_enabled=%s\n", sweep_cfg.sweep3_enabled ? "true" : "false");

    // Sweep window 4
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep4_start_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep4_start_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep4_end_mhz=%lu\n", (unsigned long)(sweep_cfg.sweep4_end_freq / 1000000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep4_step_khz=%lu\n", (unsigned long)(sweep_cfg.sweep4_step_freq / 1000ULL));
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "sweep4_enabled=%s\n", sweep_cfg.sweep4_enabled ? "true" : "false");

    // Metadata
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "freqman_path=DRONES\n");
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
        "settings_version=1.1\n");

    const auto write_result = file.write(buffer, offset);
    if (write_result.is_ok()) {
        (void)file.sync();
    }
    file.close();

    return write_result.is_ok() ? ErrorCode::SUCCESS : ErrorCode::INITIALIZATION_FAILED;
}

// ============================================================================
// SettingsFileManager::apply_to_config
// ============================================================================

void SettingsFileManager::apply_to_config(
    const SettingsStruct& s,
    ScanConfig& config
) noexcept {
    config.mode = s.scanning_mode;
    config.scan_interval_ms = s.scan_interval_ms;
    config.rssi_threshold_dbm = s.alert_rssi_threshold_dbm;
    config.dwell_enabled = s.dwell_enabled;
    config.confirm_count_enabled = s.confirm_count_enabled;
    config.noise_blacklist_enabled = s.noise_blacklist_enabled;
    config.spectrum_detection_enabled = s.spectrum_detection_enabled;
    config.spectrum_margin = s.spectrum_margin;
    config.spectrum_min_width = s.spectrum_min_width;
    config.spectrum_max_width = s.spectrum_max_width;
    config.spectrum_peak_sharpness = s.spectrum_peak_sharpness;
    config.spectrum_peak_ratio = s.spectrum_peak_ratio;
    config.spectrum_valley_depth = s.spectrum_valley_depth;
    config.median_enabled = s.median_enabled;
    config.neighbor_margin_db = s.neighbor_margin_db;
    config.rssi_variance_enabled = s.rssi_variance_enabled;
    config.confirm_count = s.confirm_count;

    // Sweep window 1
    config.sweep_start_freq = s.sweep_start_freq;
    config.sweep_end_freq = s.sweep_end_freq;
    config.sweep_step_freq = s.sweep_step_freq;

    // Sweep window 2
    config.sweep2_start_freq = s.sweep2_start_freq;
    config.sweep2_end_freq = s.sweep2_end_freq;
    config.sweep2_step_freq = s.sweep2_step_freq;
    config.sweep2_enabled = s.sweep2_enabled;

    // Sweep window 3
    config.sweep3_start_freq = s.sweep3_start_freq;
    config.sweep3_end_freq = s.sweep3_end_freq;
    config.sweep3_step_freq = s.sweep3_step_freq;
    config.sweep3_enabled = s.sweep3_enabled;

    // Sweep window 4
    config.sweep4_start_freq = s.sweep4_start_freq;
    config.sweep4_end_freq = s.sweep4_end_freq;
    config.sweep4_step_freq = s.sweep4_step_freq;
    config.sweep4_enabled = s.sweep4_enabled;
}

// ============================================================================
// SettingsFileManager::extract_from_config
// ============================================================================

void SettingsFileManager::extract_from_config(
    const ScanConfig& config,
    SettingsStruct& s
) noexcept {
    s.scanning_mode = config.mode;
    s.scan_interval_ms = config.scan_interval_ms;
    s.alert_rssi_threshold_dbm = config.rssi_threshold_dbm;
    s.dwell_enabled = config.dwell_enabled;
    s.confirm_count_enabled = config.confirm_count_enabled;
    s.noise_blacklist_enabled = config.noise_blacklist_enabled;
    s.spectrum_detection_enabled = config.spectrum_detection_enabled;
    s.spectrum_margin = config.spectrum_margin;
    s.spectrum_min_width = config.spectrum_min_width;
    s.spectrum_max_width = config.spectrum_max_width;
    s.spectrum_peak_sharpness = config.spectrum_peak_sharpness;
    s.spectrum_peak_ratio = config.spectrum_peak_ratio;
    s.spectrum_valley_depth = config.spectrum_valley_depth;
    s.median_enabled = config.median_enabled;
    s.neighbor_margin_db = config.neighbor_margin_db;
    s.rssi_variance_enabled = config.rssi_variance_enabled;
    s.confirm_count = config.confirm_count;

    // Sweep window 1
    s.sweep_start_freq = config.sweep_start_freq;
    s.sweep_end_freq = config.sweep_end_freq;
    s.sweep_step_freq = config.sweep_step_freq;

    // Sweep window 2
    s.sweep2_start_freq = config.sweep2_start_freq;
    s.sweep2_end_freq = config.sweep2_end_freq;
    s.sweep2_step_freq = config.sweep2_step_freq;
    s.sweep2_enabled = config.sweep2_enabled;

    // Sweep window 3
    s.sweep3_start_freq = config.sweep3_start_freq;
    s.sweep3_end_freq = config.sweep3_end_freq;
    s.sweep3_step_freq = config.sweep3_step_freq;
    s.sweep3_enabled = config.sweep3_enabled;

    // Sweep window 4
    s.sweep4_start_freq = config.sweep4_start_freq;
    s.sweep4_end_freq = config.sweep4_end_freq;
    s.sweep4_step_freq = config.sweep4_step_freq;
    s.sweep4_enabled = config.sweep4_enabled;
}

} // namespace drone_analyzer
