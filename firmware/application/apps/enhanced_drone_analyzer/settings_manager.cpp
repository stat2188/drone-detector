#include <cstdint>
#include <cstring>

#include "settings_manager.hpp"
#include "scanner.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "receiver_model.hpp"
#include "portapack.hpp"

namespace drone_analyzer {

SettingsStruct::SettingsStruct() noexcept
    : scanning_mode(DEFAULT_SCANNING_MODE)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , scan_sensitivity(85)
    , alert_rssi_threshold_dbm(RSSI_DETECTION_THRESHOLD_DBM)
    , threat_low_dbm(DEFAULT_THREAT_LOW_DBM)
    , threat_medium_dbm(DEFAULT_THREAT_MEDIUM_DBM)
    , threat_high_dbm(RSSI_HIGH_THREAT_THRESHOLD_DBM)
    , threat_critical_dbm(RSSI_CRITICAL_THREAT_THRESHOLD_DBM)
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
    , spectrum_flatness(DEFAULT_SPECTRUM_FLATNESS)
    , spectrum_symmetry(DEFAULT_SPECTRUM_SYMMETRY)
    , cfar_mode(DEFAULT_CFAR_MODE)
    , cfar_ref_cells(DEFAULT_CFAR_REF_CELLS)
    , cfar_guard_cells(DEFAULT_CFAR_GUARD_CELLS)
    , cfar_threshold_x10(DEFAULT_CFAR_THRESHOLD_X10)
    , cfar_hybrid_alpha(DEFAULT_CFAR_HYBRID_ALPHA)
    , cfar_hybrid_beta(DEFAULT_CFAR_HYBRID_BETA)
    , cfar_hybrid_gamma(DEFAULT_CFAR_HYBRID_GAMMA)
    , os_cfar_k_percent(DEFAULT_OS_CFAR_K_PERCENT)
    , vi_cfar_threshold_x10(DEFAULT_VI_CFAR_THRESHOLD_X10)
    , neighbor_margin_db(DEFAULT_NEIGHBOR_MARGIN_DB)
    , rssi_variance_enabled(true)
    , confirm_count(DEFAULT_CONFIRM_COUNT)
    , mahalanobis_enabled(true)
    , mahalanobis_threshold_x10(DEFAULT_MAHALOBIS_THRESHOLD_X10)
    , pattern_matching_enabled(true)
    , sweep_start_freq(SWEEP_DEFAULT_START_HZ)
    , sweep_end_freq(SWEEP_DEFAULT_END_HZ)
    , sweep_step_freq(17813000)
    , sweep2_start_freq(2400000000ULL)
    , sweep2_end_freq(2500000000ULL)
    , sweep2_step_freq(17813000)
    , sweep2_enabled(false)
    , sweep3_start_freq(900000000ULL)
    , sweep3_end_freq(1000000000ULL)
    , sweep3_step_freq(17813000)
    , sweep3_enabled(false)
    , sweep4_start_freq(1200000000ULL)
    , sweep4_end_freq(1300000000ULL)
    , sweep4_step_freq(17813000)
    , sweep4_enabled(false) {
}

// Compact key=value parser
static void parse_line(const uint8_t* buf, size_t len, SettingsStruct& s) noexcept {
    if (len == 0 || buf[0] == '#') return;
    size_t eq = 0;
    for (size_t i = 0; i < len; ++i) { if (buf[i] == '=') { eq = i; break; } }
    if (eq == 0 || eq >= len - 1) return;

    char key[32];
    size_t kl = (eq > 31) ? 31 : eq;
    for (size_t i = 0; i < kl; ++i) key[i] = static_cast<char>(buf[i]);
    key[kl] = '\0';
    const uint8_t* val = buf + eq + 1;
    size_t vl = len - eq - 1;

    auto km = [key, kl](const char* e) -> bool {
        const size_t el = __builtin_strlen(e);
        return (kl == el) && __builtin_memcmp(key, e, el) == 0;
    };
    auto pint = [val, vl]() -> uint64_t {
        uint64_t v = 0;
        for (size_t i = 0; i < vl; ++i)
            if (val[i] >= '0' && val[i] <= '9') v = v * 10 + (val[i] - '0');
        return v;
    };
    auto pbool = [val, vl]() -> bool {
        return vl == 4 && val[0] == 't' && val[1] == 'r' && val[2] == 'u' && val[3] == 'e';
    };
    auto psint = [val, vl]() -> int32_t {
        const bool neg = vl > 0 && val[0] == '-';
        const uint8_t* ns = neg ? val + 1 : val;
        size_t nl = neg ? vl - 1 : vl;
        uint32_t v = 0;
        for (size_t i = 0; i < nl; ++i)
            if (ns[i] >= '0' && ns[i] <= '9') v = v * 10 + (ns[i] - '0');
        return neg ? -static_cast<int32_t>(v) : static_cast<int32_t>(v);
    };

    // Scanning
    if (km("scan_interval_ms")) { auto v = pint(); s.scan_interval_ms = static_cast<uint32_t>((v < 10) ? 10 : (v > 10000 ? 10000 : v)); }
    else if (km("sensitivity")) { auto v = static_cast<int32_t>(pint()); s.scan_sensitivity = static_cast<uint8_t>(v > 100 ? 100 : (v < 0 ? 0 : v)); s.alert_rssi_threshold_dbm = -20 - s.scan_sensitivity; }
    else if (km("rssi_threshold_db")) { s.alert_rssi_threshold_dbm = psint(); const int32_t sens = -20 - s.alert_rssi_threshold_dbm; s.scan_sensitivity = static_cast<uint8_t>(sens > 100 ? 100 : (sens < 0 ? 0 : sens)); }
    else if (km("threat_low_db")) { auto v = psint(); s.threat_low_dbm = (v < RSSI_MIN_DBM) ? RSSI_MIN_DBM : (v > RSSI_MAX_DBM) ? RSSI_MAX_DBM : v; }
    else if (km("threat_medium_db")) { auto v = psint(); s.threat_medium_dbm = (v < RSSI_MIN_DBM) ? RSSI_MIN_DBM : (v > RSSI_MAX_DBM) ? RSSI_MAX_DBM : v; }
    else if (km("threat_high_db")) { auto v = psint(); s.threat_high_dbm = (v < RSSI_MIN_DBM) ? RSSI_MIN_DBM : (v > RSSI_MAX_DBM) ? RSSI_MAX_DBM : v; }
    else if (km("threat_critical_db")) { auto v = psint(); s.threat_critical_dbm = (v < RSSI_MIN_DBM) ? RSSI_MIN_DBM : (v > RSSI_MAX_DBM) ? RSSI_MAX_DBM : v; }
    // Audio/Display
    else if (km("enable_audio_alerts")) { s.audio_alerts_enabled = pbool(); }
    else if (km("volume")) { auto v = pint(); s.volume = static_cast<uint8_t>((v > 99) ? 99 : v); }
    else if (km("show_spectrum")) { s.spectrum_visible = pbool(); }
    else if (km("show_histogram")) { s.histogram_visible = pbool(); }
    // Detection
    else if (km("spectrum_detection")) { s.spectrum_detection_enabled = pbool(); }
    else if (km("dwell_enabled")) { s.dwell_enabled = pbool(); }
    else if (km("confirm_count_enabled")) { s.confirm_count_enabled = pbool(); }
    else if (km("noise_blacklist_enabled")) { s.noise_blacklist_enabled = pbool(); }
    else if (km("median_enabled")) { s.median_enabled = pbool(); }
    // Shape filter
    else if (km("spectrum_margin")) { auto v = pint(); s.spectrum_margin = static_cast<uint8_t>((v < 5) ? 5 : (v > 200 ? 200 : v)); }
    else if (km("spectrum_min_width")) { auto v = pint(); s.spectrum_min_width = static_cast<uint8_t>((v < 1) ? 1 : (v > 100 ? 100 : v)); }
    else if (km("spectrum_max_width")) { auto v = pint(); s.spectrum_max_width = static_cast<uint8_t>((v < 2) ? 2 : (v > 255 ? 255 : v)); }
    else if (km("spectrum_peak_sharpness")) { auto v = pint(); s.spectrum_peak_sharpness = static_cast<uint8_t>((v < 50) ? 50 : (v > 250 ? 250 : v)); }
    else if (km("spectrum_peak_ratio")) { auto v = pint(); s.spectrum_peak_ratio = static_cast<uint8_t>((v > 255) ? 255 : v); }
    else if (km("spectrum_valley_depth")) { auto v = pint(); s.spectrum_valley_depth = static_cast<uint8_t>((v > 255) ? 255 : v); }
    else if (km("spectrum_flatness")) { auto v = pint(); s.spectrum_flatness = static_cast<uint8_t>((v > 100) ? 100 : v); }
    else if (km("spectrum_symmetry")) { auto v = pint(); s.spectrum_symmetry = static_cast<uint8_t>((v > 100) ? 100 : v); }
    // Anti-false-positive
    else if (km("neighbor_margin_db")) { auto v = static_cast<int32_t>(pint()); s.neighbor_margin_db = (v < 0) ? 0 : (v > 15 ? 15 : v); }
    else if (km("rssi_variance_enabled")) { s.rssi_variance_enabled = pbool(); }
    else if (km("confirm_count")) { auto v = pint(); s.confirm_count = static_cast<uint8_t>((v < CONFIRM_COUNT_MIN) ? CONFIRM_COUNT_MIN : (v > CONFIRM_COUNT_MAX ? CONFIRM_COUNT_MAX : v)); }
    // Sweep window 1
    else if (km("sweep_start_mhz")) { s.sweep_start_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep_end_mhz")) { s.sweep_end_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep_step_khz")) { s.sweep_step_freq = static_cast<uint64_t>(pint()) * 1000ULL; }
    // Sweep window 2
    else if (km("sweep2_start_mhz")) { s.sweep2_start_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep2_end_mhz")) { s.sweep2_end_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep2_step_khz")) { s.sweep2_step_freq = static_cast<uint64_t>(pint()) * 1000ULL; }
    else if (km("sweep2_enabled")) { s.sweep2_enabled = pbool(); }
    // Sweep window 3
    else if (km("sweep3_start_mhz")) { s.sweep3_start_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep3_end_mhz")) { s.sweep3_end_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep3_step_khz")) { s.sweep3_step_freq = static_cast<uint64_t>(pint()) * 1000ULL; }
    else if (km("sweep3_enabled")) { s.sweep3_enabled = pbool(); }
    // Sweep window 4
    else if (km("sweep4_start_mhz")) { s.sweep4_start_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep4_end_mhz")) { s.sweep4_end_freq = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sweep4_step_khz")) { s.sweep4_step_freq = static_cast<uint64_t>(pint()) * 1000ULL; }
    else if (km("sweep4_enabled")) { s.sweep4_enabled = pbool(); }
    // Sweep exceptions (4 windows × 5 slots)
    else if (km("sw1_exc0_mhz")) { s.sweep_exceptions[0][0] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw1_exc1_mhz")) { s.sweep_exceptions[0][1] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw1_exc2_mhz")) { s.sweep_exceptions[0][2] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw1_exc3_mhz")) { s.sweep_exceptions[0][3] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw1_exc4_mhz")) { s.sweep_exceptions[0][4] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw2_exc0_mhz")) { s.sweep_exceptions[1][0] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw2_exc1_mhz")) { s.sweep_exceptions[1][1] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw2_exc2_mhz")) { s.sweep_exceptions[1][2] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw2_exc3_mhz")) { s.sweep_exceptions[1][3] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw2_exc4_mhz")) { s.sweep_exceptions[1][4] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw3_exc0_mhz")) { s.sweep_exceptions[2][0] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw3_exc1_mhz")) { s.sweep_exceptions[2][1] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw3_exc2_mhz")) { s.sweep_exceptions[2][2] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw3_exc3_mhz")) { s.sweep_exceptions[2][3] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw3_exc4_mhz")) { s.sweep_exceptions[2][4] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw4_exc0_mhz")) { s.sweep_exceptions[3][0] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw4_exc1_mhz")) { s.sweep_exceptions[3][1] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw4_exc2_mhz")) { s.sweep_exceptions[3][2] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw4_exc3_mhz")) { s.sweep_exceptions[3][3] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    else if (km("sw4_exc4_mhz")) { s.sweep_exceptions[3][4] = static_cast<uint64_t>(pint()) * 1000000ULL; }
    // Misc
    else if (km("exception_radius_mhz")) { auto r = static_cast<int32_t>(pint()); s.exception_radius_mhz = static_cast<uint8_t>(r > 100 ? 100 : (r < 1 ? 1 : r)); }
    else if (km("rssi_decrease_cycles")) { auto c = static_cast<int32_t>(pint()); s.rssi_decrease_cycles = static_cast<uint8_t>(c > 50 ? 50 : (c < 1 ? 1 : c)); }
    else if (km("cfar_mode")) { auto m = static_cast<int32_t>(pint()); s.cfar_mode = static_cast<CFARMode>((m < 0 || m > 6) ? 0 : m); }
    else if (km("cfar_ref_cells")) { auto v = pint(); s.cfar_ref_cells = static_cast<uint8_t>((v < CFAR_REF_CELLS_MIN) ? CFAR_REF_CELLS_MIN : (v > CFAR_REF_CELLS_MAX ? CFAR_REF_CELLS_MAX : v)); }
    else if (km("cfar_guard_cells")) { auto v = pint(); s.cfar_guard_cells = static_cast<uint8_t>((v < CFAR_GUARD_CELLS_MIN) ? CFAR_GUARD_CELLS_MIN : (v > CFAR_GUARD_CELLS_MAX ? CFAR_GUARD_CELLS_MAX : v)); }
    else if (km("cfar_threshold_x10")) { auto v = pint(); s.cfar_threshold_x10 = static_cast<uint8_t>((v < CFAR_THRESHOLD_MIN_X10) ? CFAR_THRESHOLD_MIN_X10 : (v > CFAR_THRESHOLD_MAX_X10 ? CFAR_THRESHOLD_MAX_X10 : v)); }
    else if (km("cfar_hybrid_alpha")) { auto v = pint(); s.cfar_hybrid_alpha = static_cast<uint8_t>((v > 100) ? 100 : v); }
    else if (km("cfar_hybrid_beta")) { auto v = pint(); s.cfar_hybrid_beta = static_cast<uint8_t>((v > 100) ? 100 : v); }
    else if (km("cfar_hybrid_gamma")) { auto v = pint(); s.cfar_hybrid_gamma = static_cast<uint8_t>((v > 100) ? 100 : v); }
    else if (km("os_cfar_k_percent")) { auto v = pint(); s.os_cfar_k_percent = static_cast<uint8_t>((v < OS_CFAR_K_PERCENT_MIN) ? OS_CFAR_K_PERCENT_MIN : (v > OS_CFAR_K_PERCENT_MAX ? OS_CFAR_K_PERCENT_MAX : v)); }
    else if (km("vi_cfar_threshold_x10")) { auto v = pint(); s.vi_cfar_threshold_x10 = static_cast<uint8_t>((v < VI_CFAR_THRESHOLD_MIN_X10) ? VI_CFAR_THRESHOLD_MIN_X10 : (v > VI_CFAR_THRESHOLD_MAX_X10 ? VI_CFAR_THRESHOLD_MAX_X10 : v)); }
    else if (km("mahalanobis_enabled")) { s.mahalanobis_enabled = pbool(); }
    else if (km("mahalanobis_threshold_x10")) { auto v = pint(); s.mahalanobis_threshold_x10 = static_cast<uint8_t>((v < MAHALANOBIS_THRESHOLD_MIN_X10) ? MAHALANOBIS_THRESHOLD_MIN_X10 : (v > MAHALANOBIS_THRESHOLD_MAX_X10 ? MAHALANOBIS_THRESHOLD_MAX_X10 : v)); }
    else if (km("pattern_matching_enabled")) { s.pattern_matching_enabled = pbool(); }
}

// ============================================================================
// Load / Save
// ============================================================================

ErrorCode SettingsFileManager::load(SettingsStruct& out) noexcept {
    File file;
    const auto error = file.open(settings_dir / u"eda_settings.txt", true, false);
    if (error) return ErrorCode::DATABASE_NOT_LOADED;

    constexpr size_t CHUNK = 256;
    uint8_t buf[CHUNK];
    uint8_t line[128];
    size_t llen = 0;

    while (true) {
        const auto r = file.read(buf, CHUNK);
        if (!r.is_ok() || r.value() == 0) break;
        for (size_t i = 0; i < r.value(); ++i) {
            const char c = static_cast<char>(buf[i]);
            if (c == '\r' || c == '\n') { parse_line(line, llen, out); llen = 0; }
            else if (llen < sizeof(line) - 1) { line[llen++] = buf[i]; }
        }
    }
    parse_line(line, llen, out);

    // Post-load validation
    if (out.spectrum_min_width > out.spectrum_max_width) out.spectrum_max_width = out.spectrum_min_width;
    if (out.threat_low_dbm > out.threat_medium_dbm) out.threat_medium_dbm = out.threat_low_dbm;
    if (out.threat_medium_dbm > out.threat_high_dbm) out.threat_high_dbm = out.threat_medium_dbm;
    if (out.threat_high_dbm > out.threat_critical_dbm) out.threat_critical_dbm = out.threat_high_dbm;
    if (out.cfar_ref_cells < out.cfar_guard_cells + 2)
        out.cfar_guard_cells = (out.cfar_ref_cells > 2) ? out.cfar_ref_cells - 2 : 0;
    const uint16_t hs = static_cast<uint16_t>(out.cfar_hybrid_alpha + out.cfar_hybrid_beta + out.cfar_hybrid_gamma);
    if (hs != 100 && hs > 0) {
        out.cfar_hybrid_alpha = static_cast<uint8_t>((out.cfar_hybrid_alpha * 100) / hs);
        out.cfar_hybrid_beta = static_cast<uint8_t>((out.cfar_hybrid_beta * 100) / hs);
        out.cfar_hybrid_gamma = static_cast<uint8_t>(100 - out.cfar_hybrid_alpha - out.cfar_hybrid_beta);
    }
    file.close();
    return ErrorCode::SUCCESS;
}

// Compact write helpers
static void ws(File& f, const char* s) noexcept { f.write(s, __builtin_strlen(s)); }
static void wb(File& f, bool v) noexcept { ws(f, v ? "true\n" : "false\n"); }
static void wi(File& f, int64_t val) noexcept {
    char buf[24]; uint8_t pos = 0;
    if (val < 0) { buf[pos++] = '-'; val = -val; }
    char dig[20]; uint8_t n = 0;
    uint64_t uv = static_cast<uint64_t>(val);
    if (uv == 0) { dig[n++] = '0'; } else { while (uv > 0) { dig[n++] = '0' + static_cast<char>(uv % 10); uv /= 10; } }
    for (uint8_t i = n; i > 0; --i) buf[pos++] = dig[i - 1];
    buf[pos++] = '\n';
    f.write(buf, pos);
}
static void wl(File& f, const char* k, int64_t v) noexcept { ws(f, k); ws(f, "="); wi(f, v); }
static void wbool(File& f, const char* k, bool v) noexcept { ws(f, k); ws(f, "="); wb(f, v); }
static void wexc(File& f, const char* k, uint64_t hz) noexcept { if (hz) wl(f, k, static_cast<int64_t>(hz / 1000000ULL)); }

ErrorCode SettingsFileManager::save(DroneScanner* scanner_ptr, const SettingsStruct& s) noexcept {
    static ScanConfig sweep_cfg;
    if (scanner_ptr) {
        scanner_ptr->get_config_to(sweep_cfg);
    } else {
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
        for (uint8_t w = 0; w < 4; ++w)
            for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i)
                sweep_cfg.sweep_exceptions[w][i] = s.sweep_exceptions[w][i];
    }

    File file;
    ensure_directory(settings_dir);
    if (file.create(settings_dir / u"eda_settings.txt.tmp")) return ErrorCode::INITIALIZATION_FAILED;

    ws(file, "# Enhanced Drone Analyzer Settings\n# Auto-generated\n\n");
    ws(file, "spectrum_mode=SEQUENTIAL\n");
    wl(file, "scan_interval_ms", s.scan_interval_ms);
    wl(file, "sensitivity", s.scan_sensitivity);
    wl(file, "rssi_threshold_db", s.alert_rssi_threshold_dbm);
    wl(file, "threat_low_db", s.threat_low_dbm);
    wl(file, "threat_medium_db", s.threat_medium_dbm);
    wl(file, "threat_high_db", s.threat_high_dbm);
    wl(file, "threat_critical_db", s.threat_critical_dbm);
    wbool(file, "enable_audio_alerts", s.audio_alerts_enabled);
    wl(file, "volume", s.volume);
    wbool(file, "show_spectrum", s.spectrum_visible);
    wbool(file, "show_histogram", s.histogram_visible);
    wbool(file, "spectrum_detection", s.spectrum_detection_enabled);
    wbool(file, "dwell_enabled", s.dwell_enabled);
    wbool(file, "confirm_count_enabled", s.confirm_count_enabled);
    wbool(file, "noise_blacklist_enabled", s.noise_blacklist_enabled);
    wbool(file, "median_enabled", s.median_enabled);
    wl(file, "spectrum_margin", s.spectrum_margin);
    wl(file, "spectrum_min_width", s.spectrum_min_width);
    wl(file, "spectrum_max_width", s.spectrum_max_width);
    wl(file, "spectrum_peak_sharpness", s.spectrum_peak_sharpness);
    wl(file, "spectrum_peak_ratio", s.spectrum_peak_ratio);
    wl(file, "spectrum_valley_depth", s.spectrum_valley_depth);
    wl(file, "spectrum_flatness", s.spectrum_flatness);
    wl(file, "spectrum_symmetry", s.spectrum_symmetry);
    wl(file, "neighbor_margin_db", s.neighbor_margin_db);
    wbool(file, "rssi_variance_enabled", s.rssi_variance_enabled);
    wl(file, "confirm_count", s.confirm_count);
    wl(file, "sweep_start_mhz", static_cast<int64_t>(sweep_cfg.sweep_start_freq / 1000000ULL));
    wl(file, "sweep_end_mhz", static_cast<int64_t>(sweep_cfg.sweep_end_freq / 1000000ULL));
    wl(file, "sweep_step_khz", static_cast<int64_t>(sweep_cfg.sweep_step_freq / 1000ULL));
    wl(file, "sweep2_start_mhz", static_cast<int64_t>(sweep_cfg.sweep2_start_freq / 1000000ULL));
    wl(file, "sweep2_end_mhz", static_cast<int64_t>(sweep_cfg.sweep2_end_freq / 1000000ULL));
    wl(file, "sweep2_step_khz", static_cast<int64_t>(sweep_cfg.sweep2_step_freq / 1000ULL));
    wbool(file, "sweep2_enabled", sweep_cfg.sweep2_enabled);
    wl(file, "sweep3_start_mhz", static_cast<int64_t>(sweep_cfg.sweep3_start_freq / 1000000ULL));
    wl(file, "sweep3_end_mhz", static_cast<int64_t>(sweep_cfg.sweep3_end_freq / 1000000ULL));
    wl(file, "sweep3_step_khz", static_cast<int64_t>(sweep_cfg.sweep3_step_freq / 1000ULL));
    wbool(file, "sweep3_enabled", sweep_cfg.sweep3_enabled);
    wl(file, "sweep4_start_mhz", static_cast<int64_t>(sweep_cfg.sweep4_start_freq / 1000000ULL));
    wl(file, "sweep4_end_mhz", static_cast<int64_t>(sweep_cfg.sweep4_end_freq / 1000000ULL));
    wl(file, "sweep4_step_khz", static_cast<int64_t>(sweep_cfg.sweep4_step_freq / 1000ULL));
    wbool(file, "sweep4_enabled", sweep_cfg.sweep4_enabled);

    static const char* exc_keys[4][EXCEPTIONS_PER_WINDOW] = {
        {"sw1_exc0_mhz", "sw1_exc1_mhz", "sw1_exc2_mhz", "sw1_exc3_mhz", "sw1_exc4_mhz"},
        {"sw2_exc0_mhz", "sw2_exc1_mhz", "sw2_exc2_mhz", "sw2_exc3_mhz", "sw2_exc4_mhz"},
        {"sw3_exc0_mhz", "sw3_exc1_mhz", "sw3_exc2_mhz", "sw3_exc3_mhz", "sw3_exc4_mhz"},
        {"sw4_exc0_mhz", "sw4_exc1_mhz", "sw4_exc2_mhz", "sw4_exc3_mhz", "sw4_exc4_mhz"},
    };
    for (uint8_t w = 0; w < 4; ++w)
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i)
            wexc(file, exc_keys[w][i], sweep_cfg.sweep_exceptions[w][i]);

    wl(file, "exception_radius_mhz", s.exception_radius_mhz);
    wl(file, "rssi_decrease_cycles", s.rssi_decrease_cycles);
    wl(file, "cfar_mode", static_cast<int64_t>(static_cast<uint8_t>(s.cfar_mode)));
    wl(file, "cfar_ref_cells", s.cfar_ref_cells);
    wl(file, "cfar_guard_cells", s.cfar_guard_cells);
    wl(file, "cfar_threshold_x10", s.cfar_threshold_x10);
    wl(file, "cfar_hybrid_alpha", s.cfar_hybrid_alpha);
    wl(file, "cfar_hybrid_beta", s.cfar_hybrid_beta);
    wl(file, "cfar_hybrid_gamma", s.cfar_hybrid_gamma);
    wl(file, "os_cfar_k_percent", s.os_cfar_k_percent);
    wl(file, "vi_cfar_threshold_x10", s.vi_cfar_threshold_x10);
    wbool(file, "mahalanobis_enabled", s.mahalanobis_enabled);
    wl(file, "mahalanobis_threshold_x10", s.mahalanobis_threshold_x10);
    wbool(file, "pattern_matching_enabled", s.pattern_matching_enabled);
    ws(file, "freqman_path=DRONES\nsettings_version=1.2\n");

    (void)file.sync();
    file.close();
    (void)delete_file(settings_dir / u"eda_settings.txt");
    (void)rename_file(settings_dir / u"eda_settings.txt.tmp", settings_dir / u"eda_settings.txt");
    return ErrorCode::SUCCESS;
}

// ============================================================================
// apply_to_config / extract_from_config
// ============================================================================

void SettingsFileManager::apply_to_config(const SettingsStruct& s, ScanConfig& c) noexcept {
    c.mode = s.scanning_mode;
    c.scan_interval_ms = s.scan_interval_ms;
    c.rssi_threshold_dbm = s.alert_rssi_threshold_dbm;
    c.threat_low_dbm = s.threat_low_dbm;
    c.threat_medium_dbm = s.threat_medium_dbm;
    c.threat_high_dbm = s.threat_high_dbm;
    c.threat_critical_dbm = s.threat_critical_dbm;
    c.dwell_enabled = s.dwell_enabled;
    c.confirm_count_enabled = s.confirm_count_enabled;
    c.noise_blacklist_enabled = s.noise_blacklist_enabled;
    c.spectrum_detection_enabled = s.spectrum_detection_enabled;
    c.median_enabled = s.median_enabled;
    c.mahalanobis_enabled = s.mahalanobis_enabled;
    c.mahalanobis_threshold_x10 = s.mahalanobis_threshold_x10;
    c.pattern_matching_enabled = s.pattern_matching_enabled;
    c.neighbor_margin_db = s.neighbor_margin_db;
    c.rssi_variance_enabled = s.rssi_variance_enabled;
    c.confirm_count = s.confirm_count;
    c.spectrum_margin = s.spectrum_margin;
    c.spectrum_min_width = s.spectrum_min_width;
    c.spectrum_max_width = s.spectrum_max_width;
    c.spectrum_peak_sharpness = s.spectrum_peak_sharpness;
    c.spectrum_peak_ratio = s.spectrum_peak_ratio;
    c.spectrum_valley_depth = s.spectrum_valley_depth;
    c.spectrum_flatness = s.spectrum_flatness;
    c.spectrum_symmetry = s.spectrum_symmetry;
    c.cfar_mode = s.cfar_mode;
    c.cfar_ref_cells = s.cfar_ref_cells;
    c.cfar_guard_cells = s.cfar_guard_cells;
    c.cfar_threshold_x10 = s.cfar_threshold_x10;
    c.cfar_hybrid_alpha = s.cfar_hybrid_alpha;
    c.cfar_hybrid_beta = s.cfar_hybrid_beta;
    c.cfar_hybrid_gamma = s.cfar_hybrid_gamma;
    c.os_cfar_k_percent = s.os_cfar_k_percent;
    c.vi_cfar_threshold_x10 = s.vi_cfar_threshold_x10;
    c.sweep_start_freq = s.sweep_start_freq;
    c.sweep_end_freq = s.sweep_end_freq;
    c.sweep_step_freq = s.sweep_step_freq;
    c.sweep2_start_freq = s.sweep2_start_freq;
    c.sweep2_end_freq = s.sweep2_end_freq;
    c.sweep2_step_freq = s.sweep2_step_freq;
    c.sweep2_enabled = s.sweep2_enabled;
    c.sweep3_start_freq = s.sweep3_start_freq;
    c.sweep3_end_freq = s.sweep3_end_freq;
    c.sweep3_step_freq = s.sweep3_step_freq;
    c.sweep3_enabled = s.sweep3_enabled;
    c.sweep4_start_freq = s.sweep4_start_freq;
    c.sweep4_end_freq = s.sweep4_end_freq;
    c.sweep4_step_freq = s.sweep4_step_freq;
    c.sweep4_enabled = s.sweep4_enabled;
    for (uint8_t w = 0; w < 4; ++w)
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i)
            c.sweep_exceptions[w][i] = s.sweep_exceptions[w][i];
    c.exception_radius_mhz = s.exception_radius_mhz;
    c.rssi_decrease_cycles = s.rssi_decrease_cycles;
}

void SettingsFileManager::extract_from_config(const ScanConfig& c, SettingsStruct& s) noexcept {
    s.scanning_mode = c.mode;
    s.scan_interval_ms = c.scan_interval_ms;
    s.alert_rssi_threshold_dbm = c.rssi_threshold_dbm;
    s.threat_low_dbm = c.threat_low_dbm;
    s.threat_medium_dbm = c.threat_medium_dbm;
    s.threat_high_dbm = c.threat_high_dbm;
    s.threat_critical_dbm = c.threat_critical_dbm;
    s.scan_sensitivity = static_cast<uint8_t>(
        (c.rssi_threshold_dbm > -20) ? 0 : (c.rssi_threshold_dbm < -120) ? 100 : (-20 - c.rssi_threshold_dbm));
    s.dwell_enabled = c.dwell_enabled;
    s.confirm_count_enabled = c.confirm_count_enabled;
    s.noise_blacklist_enabled = c.noise_blacklist_enabled;
    s.spectrum_detection_enabled = c.spectrum_detection_enabled;
    s.median_enabled = c.median_enabled;
    s.spectrum_margin = c.spectrum_margin;
    s.spectrum_min_width = c.spectrum_min_width;
    s.spectrum_max_width = c.spectrum_max_width;
    s.spectrum_peak_sharpness = c.spectrum_peak_sharpness;
    s.spectrum_peak_ratio = c.spectrum_peak_ratio;
    s.spectrum_valley_depth = c.spectrum_valley_depth;
    s.spectrum_flatness = c.spectrum_flatness;
    s.spectrum_symmetry = c.spectrum_symmetry;
    s.neighbor_margin_db = c.neighbor_margin_db;
    s.rssi_variance_enabled = c.rssi_variance_enabled;
    s.confirm_count = c.confirm_count;
    s.cfar_mode = c.cfar_mode;
    s.cfar_ref_cells = c.cfar_ref_cells;
    s.cfar_guard_cells = c.cfar_guard_cells;
    s.cfar_threshold_x10 = c.cfar_threshold_x10;
    s.cfar_hybrid_alpha = c.cfar_hybrid_alpha;
    s.cfar_hybrid_beta = c.cfar_hybrid_beta;
    s.cfar_hybrid_gamma = c.cfar_hybrid_gamma;
    s.os_cfar_k_percent = c.os_cfar_k_percent;
    s.vi_cfar_threshold_x10 = c.vi_cfar_threshold_x10;
    s.sweep_start_freq = c.sweep_start_freq;
    s.sweep_end_freq = c.sweep_end_freq;
    s.sweep_step_freq = c.sweep_step_freq;
    s.sweep2_start_freq = c.sweep2_start_freq;
    s.sweep2_end_freq = c.sweep2_end_freq;
    s.sweep2_step_freq = c.sweep2_step_freq;
    s.sweep2_enabled = c.sweep2_enabled;
    s.sweep3_start_freq = c.sweep3_start_freq;
    s.sweep3_end_freq = c.sweep3_end_freq;
    s.sweep3_step_freq = c.sweep3_step_freq;
    s.sweep3_enabled = c.sweep3_enabled;
    s.sweep4_start_freq = c.sweep4_start_freq;
    s.sweep4_end_freq = c.sweep4_end_freq;
    s.sweep4_step_freq = c.sweep4_step_freq;
    s.sweep4_enabled = c.sweep4_enabled;
    for (uint8_t w = 0; w < 4; ++w)
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i)
            s.sweep_exceptions[w][i] = c.sweep_exceptions[w][i];
    s.exception_radius_mhz = c.exception_radius_mhz;
    s.rssi_decrease_cycles = c.rssi_decrease_cycles;
    s.mahalanobis_enabled = c.mahalanobis_enabled;
    s.mahalanobis_threshold_x10 = c.mahalanobis_threshold_x10;
    s.pattern_matching_enabled = c.pattern_matching_enabled;
}

} // namespace drone_analyzer
