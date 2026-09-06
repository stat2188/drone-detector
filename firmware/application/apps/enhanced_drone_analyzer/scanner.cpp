#include <cstdint>
#include <algorithm>

#include "ch.h"

#include "scanner.hpp"
#include "receiver_model.hpp"
#include "portapack.hpp"
#include "portapack_persistent_memory.hpp"

namespace drone_analyzer {

/**
 * @brief Looking Glass edge skip (pixels) used by analyze_spectrum_shape_lg()
 *        and by the TBD narrowband guard in process_spectrum_sweep().
 * @note File-scope so both call sites share one definition (was a function
 *       local — duplicated magic number if the guard re-declared it).
 */
constexpr size_t LG_EDGE_SKIP_PX = 4;

/**
 * @brief Convert a spectrum.db uint8_t value to calibrated dBm (with cached gain)
 * @param value Raw spectrum.db value (0-255, from baseband)
 * @param total_gain LNA + VGA + (RF_AMP enabled ? RF_AMP_GAIN_DB : 0) in dB
 * @return RSSI in dBm, gain-compensated
 * @note Encoding: spectrum.db = clamp(dBV*5 + 255, 0, 255) => dBV_norm = (value - 255) / 5
 * @note dBm = dBV_norm - total_gain
 */
static int32_t spectrum_value_to_dbm(const uint8_t value, int32_t total_gain) noexcept {
    // Round toward nearest instead of truncating toward zero.
    // Raw encoding: spectrum.db = clamp(dBV*5 + 255, 0, 255).
    // Inverse: dBV = (value - 255) / 5. Without rounding, truncation loses
    // up to 0.2 dB per sample (e.g., value=254 gives 0 instead of -0.2).
    // Rounding recovers ~0.1 dB average precision at zero cost (no float).
    const int32_t raw = static_cast<int32_t>(value) - 255;
    const int32_t dbv_norm = (raw >= 0) ? ((raw + 2) / 5) : ((raw - 2) / 5);
    int32_t rssi = dbv_norm - total_gain;
    if (rssi < RSSI_MIN_DBM) rssi = RSSI_MIN_DBM;
    if (rssi > RSSI_MAX_DBM) rssi = RSSI_MAX_DBM;
    return rssi;
}

/**
 * @brief Read total gain from hardware (SPI access).
 * @note Call once and pass to spectrum_value_to_dbm() to avoid repeated SPI reads.
 */
static int32_t get_current_total_gain() noexcept {
    return static_cast<int32_t>(portapack::receiver_model.lna()) +
           static_cast<int32_t>(portapack::receiver_model.vga()) +
           (portapack::receiver_model.rf_amp() ? RF_AMP_GAIN_DB : 0);
}

/**
 * @brief RSSI extraction — scan usable bins with edge skip
 * @param spectrum Channel spectrum
 * @param out_peak Optional output for the usable-bin peak power (used to fill
 *        last_peak_power_ without a second full spectrum scan). Excludes the DC
 *        spike region and edge bins — same cells used for RSSI, so the returned
 *        value is the power that actually determines signal detection.
 */
static int32_t extract_rssi(const ChannelSpectrum& spectrum, uint8_t* out_peak = nullptr) noexcept {
    uint8_t peak = 0;

    for (size_t i = FFT_EDGE_SKIP; i < FFT_DC_SPIKE_START; ++i) {
        if (spectrum.db[i] > peak) peak = spectrum.db[i];
    }
    for (size_t i = FFT_DC_SPIKE_END; i < (FFT_BIN_COUNT - FFT_EDGE_SKIP); ++i) {
        if (spectrum.db[i] > peak) peak = spectrum.db[i];
    }

    if (out_peak != nullptr) *out_peak = peak;

    const int32_t total_gain = get_current_total_gain();
    return spectrum_value_to_dbm(peak, total_gain);
}

/**
 * @brief Compute k-th percentile via quickselect (O(n) average, in-place)
 * @param buf Buffer to partition (will be modified)
 * @param count Number of elements in buffer
 * @param percentile Percentile to compute (0-100, where 25 = 25th percentile)
 * @return Percentile value (0 if count == 0)
 * @note 25th percentile is more robust than median for noise floor estimation
 *       when signal occupies >50% of bins (WiFi-dense environments).
 *       Median becomes signal-biased in that case; 25th percentile stays near
 *       the true noise floor.
 */
static uint8_t quickselect_percentile(uint8_t* buf, size_t count, uint8_t percentile) noexcept {
    if (count == 0) return 0;
    const size_t k = (count * static_cast<size_t>(percentile)) / 100;
    const size_t k_safe = (k < count) ? k : count - 1;
    size_t left = 0;
    size_t right = count - 1;
    while (left < right) {
        const size_t pivot_idx = left + (right - left) / 2;
        const uint8_t pivot = buf[pivot_idx];
        buf[pivot_idx] = buf[right];
        buf[right] = pivot;
        size_t store = left;
        for (size_t i = left; i < right; ++i) {
            if (buf[i] < pivot) {
                const uint8_t t = buf[store];
                buf[store] = buf[i];
                buf[i] = t;
                ++store;
            }
        }
        buf[right] = buf[store];
        buf[store] = pivot;
        if (store == k_safe) break;
        if (store < k_safe) left = store + 1;
        else right = store - 1;
    }
    return buf[k_safe];
}

// ============================================================================
// ScanConfig Implementation
// ============================================================================

ScanConfig::ScanConfig() noexcept
    : mode(DEFAULT_SCANNING_MODE)
    , start_frequency(MIN_FREQUENCY_HZ)
    , end_frequency(MAX_FREQUENCY_HZ)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , rssi_threshold_dbm(RSSI_DETECTION_THRESHOLD_DBM)
    , stale_timeout_ms(DRONE_REMOVAL_TIMEOUT_MS)
    , sweep_start_freq(SWEEP_DEFAULT_START_HZ)
    , sweep_end_freq(SWEEP_DEFAULT_END_HZ)
    , sweep_step_freq(17813000)
    , dwell_enabled(true)
    , confirm_count_enabled(true)
    , noise_blacklist_enabled(true)
    , spectrum_detection_enabled(true)
    , median_enabled(false)
    , spectrum_margin(DEFAULT_SPECTRUM_MARGIN)
    , spectrum_min_width(DEFAULT_SPECTRUM_MIN_WIDTH)
    , spectrum_max_width(DEFAULT_SPECTRUM_MAX_WIDTH)
    , spectrum_peak_sharpness(DEFAULT_SPECTRUM_PEAK_SHARPNESS)
    , spectrum_peak_ratio(DEFAULT_SPECTRUM_PEAK_RATIO)
    , spectrum_valley_depth(DEFAULT_SPECTRUM_VALLEY_DEPTH)
    , spectrum_flatness(DEFAULT_SPECTRUM_FLATNESS)
    , spectrum_symmetry(DEFAULT_SPECTRUM_SYMMETRY)
    , neighbor_margin_db(DEFAULT_NEIGHBOR_MARGIN_DB)
    , rssi_variance_enabled(true)
    , confirm_count(DEFAULT_CONFIRM_COUNT)
    , sensitive_mode(false) {
    // sweep2/3/4 fields use in-class defaults: disabled
    // mahalanobis_enabled uses in-class default (true, FPV-optimized)
}

ScanConfig::ScanConfig(ScanningMode m, FreqHz start, FreqHz end) noexcept
    : mode(m)
    , start_frequency(start)
    , end_frequency(end)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , rssi_threshold_dbm(RSSI_DETECTION_THRESHOLD_DBM)
    , stale_timeout_ms(DRONE_REMOVAL_TIMEOUT_MS)
    , sweep_start_freq(SWEEP_DEFAULT_START_HZ)
    , sweep_end_freq(SWEEP_DEFAULT_END_HZ)
    , sweep_step_freq(17813000)
    , dwell_enabled(true)
    , confirm_count_enabled(true)
    , noise_blacklist_enabled(true)
    , spectrum_detection_enabled(true)
    , median_enabled(false)
    , spectrum_margin(DEFAULT_SPECTRUM_MARGIN)
    , spectrum_min_width(DEFAULT_SPECTRUM_MIN_WIDTH)
    , spectrum_max_width(DEFAULT_SPECTRUM_MAX_WIDTH)
    , spectrum_peak_sharpness(DEFAULT_SPECTRUM_PEAK_SHARPNESS)
    , spectrum_peak_ratio(DEFAULT_SPECTRUM_PEAK_RATIO)
    , spectrum_valley_depth(DEFAULT_SPECTRUM_VALLEY_DEPTH)
    , spectrum_flatness(DEFAULT_SPECTRUM_FLATNESS)
    , spectrum_symmetry(DEFAULT_SPECTRUM_SYMMETRY)
    , neighbor_margin_db(DEFAULT_NEIGHBOR_MARGIN_DB)
    , rssi_variance_enabled(true)
    , confirm_count(DEFAULT_CONFIRM_COUNT)
    , sensitive_mode(false) {
    // sweep2/3/4 fields use in-class defaults: disabled
    // mahalanobis_enabled uses in-class default (true, FPV-optimized)
}



// ========================================================================
// ScanStatistics Implementation
// ============================================================================

ScanStatistics::ScanStatistics() noexcept
    : total_scan_cycles(0)
    , successful_cycles(0)
    , failed_cycles(0)
    , drones_detected(0)
    , max_rssi_dbm(RSSI_NOISE_FLOOR_DBM) {
}

void ScanStatistics::reset() noexcept {
    total_scan_cycles = 0;
    successful_cycles = 0;
    failed_cycles = 0;
    drones_detected = 0;
    max_rssi_dbm = RSSI_NOISE_FLOOR_DBM;
}

// ============================================================================
// DroneScanner Implementation
// ============================================================================

DroneScanner::DroneScanner(DatabaseManager& database, HardwareController& hardware) noexcept
    : database_(database)
    , hardware_(hardware)
    , state_(ScannerState::IDLE)
    , config_()
    , freq_lock_count_{0}
    , locked_frequency_{0}
    , track_start_time_{0}
    , current_drone_type_{'\0', '\0', '\0', '\0', '\0'}
    , drone_type_valid_{false}
    , statistics_()
    , tracked_drones_()
    , tracked_count_{0}
    , current_frequency_{0}
    , pending_frequency_{0}
    , pending_count_{0}
    , missed_lock_count_{0}
    , noise_blacklist_{}
    , last_scan_time_{0}
    , scanning_active_()
    , alert_callback_(nullptr)
    , mutex_()
    , state_transition_allowed_()
    , force_resume_flag_()
    , dwell_request_()
    , dwell_cycles_{0}
    , lock_start_time_{0}
    , confirm_start_time_{0}
    , spectrum_sort_buf_{}
    , lock_timeout_count_{0}
    , sweep_usable_buf_{}
    , alert_callback_in_progress_()
    , rssi_detector_()
    , rssi_median_filter_()
    , median_filter_enabled_{false}
    , neighbor_margin_checker_()
    , mahalanobis_detector_()
    , waterfall_history_()
    , adaptive_threshold_() {

    // Initialize mutex
    chMtxInit(&mutex_);

    (void)rssi_detector_.initialize(RSSI_DETECTION_THRESHOLD_DBM);
}

DroneScanner::~DroneScanner() noexcept {
    // Stop scanning if active
    if (scanning_active_.test()) {
        (void)stop_scanning();
    }

    // Note: Do NOT call chMtxDeinit - it doesn't exist in ChibiOS
}

ErrorCode DroneScanner::initialize() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (state_ != ScannerState::IDLE) {
        return ErrorCode::INITIALIZATION_INCOMPLETE;
    }

    ErrorCode hw_result = hardware_.initialize();
    if (hw_result != ErrorCode::SUCCESS) {
        return hw_result;
    }

    // Try to get first frequency from database
    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(0);
    if (freq_result.has_value()) {
        current_frequency_ = freq_result.value();
    } else {
        // Database empty or not loaded — scanner still works on default freq
        current_frequency_ = MIN_FREQUENCY_HZ;
    }

    state_ = ScannerState::IDLE;
    statistics_.reset();

    return ErrorCode::SUCCESS;
}

// ============================================================================
// Fast Scanner Integration Getters
// ============================================================================

uint32_t DroneScanner::get_freq_lock_count() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return __atomic_load_n(&freq_lock_count_, __ATOMIC_RELAXED);
    }
    return freq_lock_count_;
}

void DroneScanner::set_freq_lock_count(uint32_t count) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    freq_lock_count_ = count;
}

FreqHz DroneScanner::get_locked_frequency() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    return locked_frequency_;
}

// ============================================================================
// Scanner Control Methods (Restored for functionality)
// ============================================================================

ErrorCode DroneScanner::start_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (state_ == ScannerState::SCANNING) {
        return ErrorCode::SUCCESS;
    }

    ErrorCode hw_result = hardware_.start_spectrum_streaming();
    if (hw_result != ErrorCode::SUCCESS) {
        return hw_result;
    }

    state_ = ScannerState::SCANNING;
    scanning_active_.set();

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::stop_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (state_ == ScannerState::IDLE) {
        return ErrorCode::SUCCESS;
    }

    state_ = ScannerState::IDLE;
    scanning_active_.clear();

    (void)hardware_.stop_spectrum_streaming();

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::pause_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ != ScannerState::SCANNING) {
        return ErrorCode::SUCCESS;
    }
    
    state_ = ScannerState::PAUSED;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::resume_scanning() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ != ScannerState::PAUSED) {
        return ErrorCode::SUCCESS;
    }
    
    state_ = ScannerState::SCANNING;
    return ErrorCode::SUCCESS;
}

void DroneScanner::force_resume_scanning() noexcept {
    // Set flag — actual state transition happens inside perform_scan_cycle()
    // under mutex protection. This avoids data race on state_ between
    // scanner thread and UI thread.
    force_resume_flag_.set();
}

void DroneScanner::request_dwell() noexcept {
    // Set dwell request — scanner thread will check this BEFORE hopping frequency.
    // This is the critical link: UI detects signal → requests dwell → scanner holds.
    dwell_request_.set();
}

bool DroneScanner::try_consume_force_resume_flag() noexcept {
    if (!force_resume_flag_.test_and_set()) {
        return false;
    }
    force_resume_flag_.clear();
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING) {
        state_ = ScannerState::SCANNING;
        // Clear lock timing when force-resuming (consistent with perform_scan_cycle_internal)
        lock_start_time_ = 0;
        confirm_start_time_ = 0;
        return true;
    }
    return false;
}

void DroneScanner::remove_drone_on_frequency(FreqHz frequency) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones_[i].frequency == frequency) {
            tracked_count_--;
            if (i < tracked_count_) {
                tracked_drones_[i] = tracked_drones_[tracked_count_];
            }
            return;
        }
    }
}

void DroneScanner::increment_noise_count(FreqHz frequency) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    increment_noise_count_internal(frequency);
}

void DroneScanner::increment_noise_count_internal(FreqHz frequency) noexcept {
    // Find existing entry or first empty slot
    // @pre Caller holds DATA_MUTEX
    size_t empty_slot = MAX_NOISE_ENTRIES;
    for (size_t i = 0; i < MAX_NOISE_ENTRIES; ++i) {
        if (noise_blacklist_[i].freq == frequency) {
            if (noise_blacklist_[i].count < 255) {
                noise_blacklist_[i].count++;
            }
            return;
        }
        if (noise_blacklist_[i].freq == 0 && empty_slot == MAX_NOISE_ENTRIES) {
            empty_slot = i;
        }
    }
    // New entry
    if (empty_slot < MAX_NOISE_ENTRIES) {
        noise_blacklist_[empty_slot].freq = frequency;
        noise_blacklist_[empty_slot].count = 1;
    }
}

void DroneScanner::reset_noise_count(FreqHz frequency) noexcept {
    for (size_t i = 0; i < MAX_NOISE_ENTRIES; ++i) {
        if (noise_blacklist_[i].freq == frequency) {
            noise_blacklist_[i].freq = 0;
            noise_blacklist_[i].count = 0;
            return;
        }
    }
}

bool DroneScanner::is_blacklisted(FreqHz frequency) const noexcept {
    for (size_t i = 0; i < MAX_NOISE_ENTRIES; ++i) {
        if (noise_blacklist_[i].freq == frequency) {
            return noise_blacklist_[i].count >= NOISE_BLACKLIST_THRESHOLD;
        }
    }
    return false;
}

bool DroneScanner::is_exception_frequency(FreqHz freq) const noexcept {
    const FreqHz exc_radius = static_cast<FreqHz>(config_.exception_radius_mhz) * 1000000ULL;
    for (uint8_t w = 0; w < 4; ++w) {
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            const FreqHz exc = config_.sweep_exceptions[w][i];
            if (exc == 0) continue;
            // Absolute difference — avoids unsigned underflow when exc < exc_radius
            const FreqHz diff = (freq > exc) ? (freq - exc) : (exc - freq);
            if (diff <= exc_radius) return true;
        }
    }
    return false;
}

ErrorCode DroneScanner::perform_scan_cycle() noexcept {
    if (!scanning_active_.test()) {
        return ErrorCode::SUCCESS;
    }
    
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (state_ == ScannerState::PAUSED || state_ == ScannerState::IDLE) {
        return ErrorCode::SUCCESS;
    }
    
    return perform_scan_cycle_internal();
}

ErrorCode DroneScanner::perform_scan_cycle_internal() noexcept {
    // Check force-resume flag (set when max dwell expires)
    if (force_resume_flag_.test_and_set()) {
        force_resume_flag_.clear();
        if (state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING) {
            state_ = ScannerState::SCANNING;
            // Clear lock timing when force-resuming
            lock_start_time_ = 0;
            confirm_start_time_ = 0;
        }
        dwell_cycles_ = 0;
        return ErrorCode::SUCCESS;
    }

    // ABSOLUTE LOCK TIMEOUT: Force exit from LOCKING/TRACKING after MAX_LOCK_DURATION_MS
    // This prevents infinite lock on noisy frequencies where signal intermittently exceeds threshold
    if (state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING) {
        if (lock_start_time_ != 0) {
            const uint32_t lock_duration = chTimeNow() - lock_start_time_;
            if (lock_duration >= MAX_LOCK_DURATION_MS) {
                // Lock timeout exceeded - force transition to SCANNING
                state_ = ScannerState::SCANNING;
                freq_lock_count_ = 0;
                locked_frequency_ = 0;
                lock_start_time_ = 0;
                confirm_start_time_ = 0;
                dwell_cycles_ = 0;
                lock_timeout_count_++;
                // Continue to frequency hop (fall through to dwell check)
            }
        } else {
            // Just entered LOCKING/TRACKING state - start timing
            lock_start_time_ = chTimeNow();
        }
    } else {
        // Not locked - clear timing
        lock_start_time_ = 0;
        confirm_start_time_ = 0;
    }

    // Check dwell request from UI thread (signal detected, hold frequency)
    if (dwell_request_.test_and_set()) {
        dwell_request_.clear();
        dwell_cycles_ = 1;  // Start at 1 so should_dwell triggers immediately
    }

    // Dwell: if UI requested hold or state is LOCKING/TRACKING, skip frequency hop
    const bool should_dwell = config_.dwell_enabled &&
        (dwell_cycles_ > 0 || state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING);

    if (should_dwell) {
        dwell_cycles_++;

        // Max dwell: 300ms total (6 cycles × 50ms).
        // Enough for median filter to warm up (4 samples) + 2 more frames for
        // reliable lock accumulation. At 60fps, 6 frames ≈ 6 increments of
        // freq_lock_count_ toward MAX_FREQ_LOCK (10) → TRACKING transition.
        // Reduced from 8 cycles (400ms) to allow faster return to scanning
        // when multiple FPV transmitters are present in the band.
        static constexpr uint8_t LOCAL_MAX_DWELL_CYCLES = 6;
        const uint8_t max_dwell = config_.confirm_count_enabled
            ? LOCAL_MAX_DWELL_CYCLES : (LOCAL_MAX_DWELL_CYCLES / 2);

        if (dwell_cycles_ >= max_dwell) {
            // Max dwell reached — force resume scanning
            if (config_.noise_blacklist_enabled) {
                const FreqHz locked_freq = locked_frequency_;
                // Find or add noise entry
                size_t empty_slot = MAX_NOISE_ENTRIES;
                for (size_t i = 0; i < MAX_NOISE_ENTRIES; ++i) {
                    if (noise_blacklist_[i].freq == locked_freq) {
                        if (noise_blacklist_[i].count < 255) {
                            noise_blacklist_[i].count++;
                        }
                        break;
                    }
                    if (noise_blacklist_[i].freq == 0 && empty_slot == MAX_NOISE_ENTRIES) {
                        empty_slot = i;
                    }
                }
                if (empty_slot < MAX_NOISE_ENTRIES && locked_freq != 0) {
                    noise_blacklist_[empty_slot].freq = locked_freq;
                    noise_blacklist_[empty_slot].count = 1;
                }
                // Remove drone on this frequency
                for (size_t i = 0; i < tracked_count_; ++i) {
                    if (tracked_drones_[i].frequency == locked_freq) {
                        tracked_count_--;
                        if (i < tracked_count_) {
                            tracked_drones_[i] = tracked_drones_[tracked_count_];
                        }
                        break;
                    }
                }
            }
            force_resume_flag_.set();
            dwell_cycles_ = 0;
        }

        // Stay on current frequency — do NOT hop
        statistics_.successful_cycles++;
        return ErrorCode::SUCCESS;
    }

    // No dwell — normal frequency hop
    dwell_cycles_ = 0;
    statistics_.total_scan_cycles++;

    apply_rssi_decay();

    // Try to get next frequency from database
    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(current_frequency_);

    if (freq_result.has_value()) {
        current_frequency_ = freq_result.value();

        // Skip blacklisted frequencies
        if (config_.noise_blacklist_enabled) {
            bool found_clean_freq = false;
            for (size_t skip_count = 0; skip_count < MAX_NOISE_ENTRIES; ++skip_count) {
                if (!is_blacklisted(current_frequency_)) {
                    found_clean_freq = true;
                    break;
                }
                freq_result = database_.get_next_frequency(current_frequency_);
                if (!freq_result.has_value()) {
                    break;  // Database exhausted
                }
                current_frequency_ = freq_result.value();
            }
            
            if (!found_clean_freq) {
                // All frequencies are blacklisted - fallback to minimum frequency
                current_frequency_ = MIN_FREQUENCY_HZ;
            }
        }
    } else {
        // Database empty — sweep through frequency range
        if (current_frequency_ < MIN_FREQUENCY_HZ || current_frequency_ >= MAX_FREQUENCY_HZ) {
            current_frequency_ = MIN_FREQUENCY_HZ;
        } else {
            current_frequency_ += FREQUENCY_STEP_HZ;
            if (current_frequency_ > MAX_FREQUENCY_HZ) {
                current_frequency_ = MIN_FREQUENCY_HZ;
            }
        }
    }

    ErrorCode tune_result = hardware_.tune_to_frequency(current_frequency_);
    if (tune_result != ErrorCode::SUCCESS) {
        statistics_.failed_cycles++;
        return tune_result;
    }

    rssi_median_filter_.reset();
    neighbor_margin_checker_.reset();

    if (current_frequency_ != pending_frequency_) {
        pending_frequency_ = 0;
        pending_count_ = 0;
    }

    statistics_.successful_cycles++;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::update_tracked_drones(
    FreqHz frequency,
    RssiValue rssi,
    SystemTime timestamp
) noexcept {
    if (frequency < MIN_FREQUENCY_HZ || frequency > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;
    }

    if (rssi < RSSI_MIN_DBM || rssi > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }

    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    return update_tracked_drone_internal(frequency, rssi, timestamp);
}

FreqHz DroneScanner::get_spectrum_frequency() noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (lock.is_locked()) {
        return current_frequency_;
    }
    return 0;
}

ErrorCode DroneScanner::process_spectrum_message(const ChannelSpectrum& spectrum, FreqHz frequency) noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);

    if (!lock.is_locked()) {
        return ErrorCode::MUTEX_LOCK_FAILED;
    }

    if (frequency == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }

    // ChannelSpectrum has fixed-size array (256 bins), no empty check needed
    // Track peak power for timeline display, fused with the RSSI scan below
    // (single pass over usable bins — DC spike/edge bins carry no real signal
    // energy and would crush timeline scaling; mirrors sweep path at :2206).
    const int32_t raw_rssi = extract_rssi(spectrum, &last_peak_power_);

    // Feed temporal history for multi-frame analysis (TBD, FHSS, Doppler)
    waterfall_history_.push(spectrum.db.data());

    // Median filter: reject single-sample noise spikes
    // Feed every sample; use median only when enabled and filter is warm
    rssi_median_filter_.add(raw_rssi);
    const int32_t rssi = (median_filter_enabled_ && rssi_median_filter_.is_warm())
        ? rssi_median_filter_.get_median()
        : raw_rssi;

    const SystemTime now = chTimeNow();

    // ----- RSSI Hysteresis (Schmitt trigger) -----
    // Reset waterfall history on frequency change — old frames are stale
    if (frequency != last_hysteresis_freq_) {
        last_hysteresis_freq_ = frequency;
        waterfall_history_.reset();
    }

    // Effective threshold: 2 dB harder to turn ON, 2 dB easier to stay ON
    // Prevents signal toggling at the threshold boundary (8-bit FFT quantization
    // causes ±1 bin fluctuation = ~0.19 dB, enough to toggle detection)
    //
    // FIX: Do NOT reset signal_present_ on frequency change. When the signal
    // persists across frequency hops (scanner still in detection zone), hysteresis
    // carries over — first frame on new freq uses the easier threshold. This is
    // correct: the signal was confirmed present on the previous frequency.
    // signal_present_ only resets when signal actually drops (lines 761/770).
    const int32_t rssi_threshold = signal_present_
        ? (config_.rssi_threshold_dbm - RSSI_HYSTERESIS_DB)
        : (config_.rssi_threshold_dbm + RSSI_HYSTERESIS_DB);

    int32_t effective_rssi = rssi;
    bool signal_detected = false;

    // Multi-peak shape analysis: find ALL independent signals in the 20 MHz frame.
    // Primary detection (index 0 = strongest) drives hysteresis, TBD, and lock state.
    // Secondary detections (index 1..N) are tracked as additional drones.
    ShapeDetectionResult shape_result{};
    bool has_shape_result = false;

    if (config_.spectrum_detection_enabled) {
        has_shape_result = analyze_spectrum_shape_multi(spectrum, shape_result, frequency);

        if (has_shape_result && shape_result.count > 0) {
            // Primary detection: strongest peak that passed shape filters
            const int32_t spectrum_rssi = shape_result.detections[0].rssi;
            // Shape passed — enforce RSSI threshold with hysteresis
            if (rssi > rssi_threshold) {
                signal_detected = true;
                signal_present_ = true;
                // Use shape-analysis RSSI if it's stronger than the simple peak.
                // Safety bound: don't allow spectrum_rssi to exceed rssi by more than
                // SHAPE_RSSI_MAX_EXCESS_DB — prevents single-sample noise spikes that
                // happen to pass shape filters from inflating effective_rssi.
                constexpr int32_t SHAPE_RSSI_MAX_EXCESS_DB = 10;
                if (spectrum_rssi > effective_rssi
                    && spectrum_rssi <= rssi + SHAPE_RSSI_MAX_EXCESS_DB) {
                    effective_rssi = spectrum_rssi;
                }
            } else if (signal_present_) {
                signal_present_ = false;
            }
        } else {
            // Shape analysis failed — no drone signal on this frequency.
            // Must reset signal_present_ so hysteresis doesn't carry over
            // from a previous frequency (prevents false easy threshold on
            // next frequency when no signal was detected here).
            signal_present_ = false;
        }
    } else {
        signal_detected = (rssi > rssi_threshold);
        if (signal_detected) {
            signal_present_ = true;
        } else if (signal_present_) {
            signal_present_ = false;
        }
    }

    // ---- Track-Before-Detect (TBD) Pass ----
    // When single-frame detection fails, check if multi-frame integration
    // confirms a weak signal. Uses WaterfallHistory to accumulate power
    // across recent frames. Provides ~9 dB SNR gain (8 frames = 3× sensitivity).
    // Only runs when waterfall_history_ has enough frames and single-frame failed.
    if (!signal_detected && waterfall_history_.is_warm(TBD_MIN_FRAMES)
        && config_.spectrum_detection_enabled) {

        // Find peak bin from newest frame
        size_t tbd_peak_bin = FFT_EDGE_SKIP;
        uint8_t tbd_peak_power = 0;
        for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            const uint8_t max_power = waterfall_history_.get_max_across_frames(i);
            if (max_power > tbd_peak_power) {
                tbd_peak_power = max_power;
                tbd_peak_bin = i;
            }
        }

        // Check if this bin was active in enough recent frames
        // FIX: Use correct inverse of spectrum_value_to_dbm encoding.
        // Forward:  dBm = (value - 255) / 5 - total_gain
        // Inverse:  value = (dBm + total_gain) * 5 + 255
        // Previously used legacy approximation (rssi_threshold + 120) which
        // produced threshold ~37 vs actual ~170, making TBD pass everything.
        const int32_t total_gain_tbd = get_current_total_gain();
        const int32_t tbd_raw_threshold = (rssi_threshold + total_gain_tbd) * 5 + 255;
        const uint8_t threshold = static_cast<uint8_t>(
            (tbd_raw_threshold < 0) ? 0 : (tbd_raw_threshold > 255 ? 255 : tbd_raw_threshold));
        const uint8_t active_frames = waterfall_history_.count_above_threshold(tbd_peak_bin, threshold);

        if (active_frames >= TBD_MIN_FRAMES) {
            // Multi-frame confirmed — compute integrated RSSI
            const uint16_t integrated = waterfall_history_.get_integrated_power(tbd_peak_bin);
            const uint8_t avg_power = static_cast<uint8_t>(integrated / waterfall_history_.size());
            const int32_t tbd_rssi = spectrum_value_to_dbm(avg_power, total_gain_tbd);

            // NARROWBAND GUARD (MaxW): TBD integrates power only and never
            // runs apply_shape_filters(), so a persistent WiFi/BT flat-top
            // rejected by MaxW single-frame was re-confirmed after 3 frames
            // and tracked as a drone — the user's MaxW setting was silently
            // ignored with bypass OFF. Re-enforce the MaxW width semantics on
            // the current frame: wideband targets must stay rejected.
            if (tbd_rssi > rssi_threshold &&
                tbd_peak_is_narrowband(
                    spectrum.db.data(), FFT_BIN_COUNT, tbd_peak_bin,
                    shape_result.noise_floor, FFT_EDGE_SKIP, /*has_dc_gap=*/true)) {
                signal_detected = true;
                signal_present_ = true;
                effective_rssi = tbd_rssi;
            }
        }
    }

    if (signal_detected) {
        // Exception check: suppress drones at configured exclusion frequencies
        // Applies to both normal scanning and sweep detection paths
        if (is_exception_frequency(frequency)) return ErrorCode::SUCCESS;

        // Neighbor margin check (if enabled): center freq must dominate neighbors
        // This eliminates wideband noise false positives (WiFi, BT, microwave)
        // SKIPPED for shape-validated detections: the shape filters already
        // reject wideband noise, and the margin check would wrongly reject the
        // weaker peak of a dual-carrier emitter (FPV video + audio subcarrier
        // scanned as separate DB channels) whenever the other carrier is
        // stronger. The check still guards RSSI-only and TBD detections.
        const bool shape_validated = has_shape_result;
        if (config_.neighbor_margin_db > 0 && !shape_validated) {
            neighbor_margin_checker_.add(frequency, effective_rssi);
            if (!neighbor_margin_checker_.check_margin(frequency, effective_rssi, config_.neighbor_margin_db)) {
                // Current frequency not stronger than neighbors — wideband noise
                return ErrorCode::SUCCESS;
            }
        }

        // Feed RSSI detector only with above-threshold samples
        // to prevent noise from polluting trend analysis
        (void)rssi_detector_.process_rssi_sample(effective_rssi, now);

        bool should_update = true;

        if (config_.confirm_count_enabled) {
            // Confirm count: require configurable N detections on same frequency
            // before creating a drone. Prevents noise spikes from adding phantom drones.
            //
            // FIX: Added CONFIRM_TIMEOUT_MS to prevent infinite waiting on noisy frequencies.
            // On 2400 MHz with intermittent noise, pending_count_ may never reach
            // config_.confirm_count, causing scanner to wait forever.

            if (frequency != pending_frequency_) {
                // New frequency - start fresh confirmation
                pending_frequency_ = frequency;
                pending_count_ = 1;
                confirm_start_time_ = now;  // Start confirmation timer
            } else if (pending_count_ < config_.confirm_count) {
                // Still accumulating confirmations on same frequency
                pending_count_++;
            }

            // CONFIRM TIMEOUT: Force continue if taking too long to gather confirmations
            // This prevents indefinite blocking on frequencies with intermittent noise.
            if (confirm_start_time_ != 0) {
                const uint32_t confirm_elapsed = now - confirm_start_time_;
                if (confirm_elapsed >= CONFIRM_TIMEOUT_MS) {
                    // Timeout exceeded - give up on confirmation, allow frequency hop
                    // Blacklist this frequency to prevent re-triggering on intermittent noise
                    // NOTE: Must use _internal variant — DATA_MUTEX is already held by caller
                    if (config_.noise_blacklist_enabled && frequency != 0) {
                        increment_noise_count_internal(frequency);
                    }
                    pending_frequency_ = 0;
                    pending_count_ = 0;
                    confirm_start_time_ = 0;
                    // Continue to next part of logic (fall through to existing drone check)
                }
            }

            ErrorResult<size_t> existing = find_nearest_drone_internal(
                frequency, static_cast<FreqHz>(config_.freq_match_radius_mhz) * 1'000'000ULL);
            if (!existing.has_value() && pending_count_ < config_.confirm_count) {
                should_update = false;  // waiting for more confirmations
            }
        }

        // Reuse the index returned by update_tracked_drone_internal (via out_index)
        // instead of a second find_nearest_drone_internal() O(n) search.
        // out_index is set on every SUCCESS path (existing drone, new drone, and
        // the RSSI-variance rejection early return) at scanner.cpp:1037/1072.
        // When should_update is false (confirm count still accumulating), no drone
        // exists and every consumer below is gated on should_update, so the stale
        // SIZE_MAX index is never dereferenced.
        size_t drone_idx = SIZE_MAX;
        if (should_update) {
            const ErrorCode err = update_tracked_drone_internal(
                frequency,
                effective_rssi,
                now,
                &drone_idx
            );
            if (err != ErrorCode::SUCCESS) {
                return err;
            }
        }

        // Reset decay counters for detected drone — signal confirmed present
        if (should_update && drone_idx < tracked_count_) {
            tracked_drones_[drone_idx].reset_missed();
            tracked_drones_[drone_idx].rssi_decrease_counter_ = 0;
        }

        // Store bin-corrected frequency for display (primary detection).
        // NOTE: lookup key (`frequency`) stays the center — identity, DB type,
        // confirm/lock state must remain exact on the tune frequency. The
        // measured value only refines what the UI shows.
        if (has_shape_result && shape_result.count > 0 && should_update && drone_idx < tracked_count_) {
            tracked_drones_[drone_idx].set_measured_frequency(
                shape_result.detections[0].frequency);
        }

        // Update max RSSI statistic
        if (effective_rssi > statistics_.max_rssi_dbm) {
            statistics_.max_rssi_dbm = effective_rssi;
        }

        // Frequency lock state machine
        if (frequency == locked_frequency_) {
            // Same frequency as locked — accumulate persistence count
            missed_lock_count_ = 0;  // Reset miss counter on successful detection
            if (freq_lock_count_ < MAX_FREQ_LOCK) {
                freq_lock_count_++;
            }
            // Transition: LOCKING → TRACKING after sustained lock
            if (freq_lock_count_ >= MAX_FREQ_LOCK && state_ == ScannerState::LOCKING) {
                state_ = ScannerState::TRACKING;
            }
        } else {
            // Different frequency detected
            if (state_ == ScannerState::SCANNING) {
                // No active lock — jump to new frequency immediately
                locked_frequency_ = frequency;
                freq_lock_count_ = 1;
                missed_lock_count_ = 0;
                state_ = ScannerState::LOCKING;
                lock_start_time_ = chTimeNow();  // Start lock timing
                confirm_start_time_ = 0;  // Reset confirm timer (not applicable for new lock)
                // CRITICAL: Request scanner thread to hold frequency.
                // Without this, the scanner thread hops to the next DB entry
                // before the UI thread can transition state to LOCKING.
                dwell_request_.set();
            } else if (state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING) {
                // Already locked — don't jump to transient signal.
                // The current lock is more valuable than a momentary spike.
                // Continue accumulating on the locked frequency.
                // Note: The "different frequency" here is likely noise or interference
                // on the locked frequency, not a genuine signal on another frequency.
            }
            // No action needed — stay in current state
        }
    } else {
        // No signal on this frequency
        // Decay is handled by apply_rssi_decay() every CYC cycles.
        // Here we only handle lock cleanup.

        // Only break lock if we're tuned to the locked freq and it's gone
        if (locked_frequency_ != 0 && frequency == locked_frequency_) {
            // Use configurable miss_tolerance (independent of confirm_count).
            // Default 4 consecutive misses (~66ms) before breaking lock.
            // Higher values tolerate more FHSS/burst fading without dropping lock.
            missed_lock_count_++;
            if (missed_lock_count_ >= config_.miss_tolerance) {
                freq_lock_count_ = 0;
                locked_frequency_ = 0;
                missed_lock_count_ = 0;
                if (state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING) {
                    state_ = ScannerState::SCANNING;
                }
            }
        } else {
            missed_lock_count_ = 0;
        }
        // If tuned to a non-locked freq and no signal: normal scanning, ignore
    }

    // ---- Secondary detection tracking (multi-peak) ----
    // Track additional independent signals found in the same 20 MHz FFT frame.
    // Primary detection (index 0) was already handled above with full hysteresis,
    // TBD, confirm count, and lock state machine. Secondary detections bypass
    // these safeguards (already validated by shape filters) and are tracked directly.
    if (has_shape_result && shape_result.count > 1 && signal_detected) {
        for (size_t d = 1; d < shape_result.count; ++d) {
            const FreqHz det_freq = shape_result.detections[d].frequency;
            const int32_t det_rssi = shape_result.detections[d].rssi;

            // Skip if frequency is out of valid range
            if (det_freq < MIN_FREQUENCY_HZ || det_freq > MAX_FREQUENCY_HZ) continue;

            // Exception check (same as primary)
            if (is_exception_frequency(det_freq)) continue;

            // Track secondary detection directly (no confirm count needed —
            // shape analysis already validated the signal)
            if (tracked_count_ < MAX_TRACKED_DRONES) {
                (void)update_tracked_drone_internal(det_freq, det_rssi, now);
            }
        }
    }

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::update_tracked_drone_internal(
    FreqHz frequency,
    RssiValue rssi,
    SystemTime timestamp,
    size_t* out_index
) noexcept {
    if (out_index != nullptr) *out_index = SIZE_MAX;
    // Nearest-match lookup WITH duplicate consolidation ("a close frequency
    // corrects the tracked entry"): a detection landing inside the match
    // radius updates the nearest existing tracker instead of spawning a
    // duplicate, and ALL other in-radius entries are absorbed into the
    // survivor (highest threat level, ties → oldest) — their RSSI history,
    // sweep cycle peaks and trend state merge, so the trend applies as if
    // every detection had always hit one ("current") entry. Lower-severity
    // threats NEVER displace higher-severity ones during merge. The returned index is the post-compaction
    // survivor; the re-centering below then pins it onto the latest
    // observation so slow RF drift (FHSS wander, TX/RX offset, 78 kHz
    // FFT-bin quantization, overlapping sweep windows) can never walk
    // outside the radius.
    // NOTE: in normal (DB scan) mode `frequency` IS the tune center, so
    // the re-centering below is a no-op there — the key stays pinned to
    // the database channel; only sweep-mode bin measurements drift it.
    ErrorResult<size_t> index_result = match_and_consolidate_drone_internal(
        frequency, static_cast<FreqHz>(config_.freq_match_radius_mhz) * 1'000'000ULL);

    if (index_result.has_value()) {
        // Existing drone — update and alert on threat increase
        size_t index = index_result.value();
        if (out_index != nullptr) *out_index = index;

        // Re-center the survivor's tracking key onto the latest observation
        // (already consolidated above) and refresh the display frequency.
        if (tracked_drones_[index].frequency != frequency) {
            tracked_drones_[index].frequency = frequency;
        }
        tracked_drones_[index].set_measured_frequency(frequency);
        
        // RSSI variance rejection: noise has chaotic fluctuations
        // Real drones have stable signal (variance < 25), noise > 100
        // FIX: record the observation WITHOUT threat reclassification
        // (observe_rssi). Previously the sample was dropped entirely: the
        // history never refreshed, so the variance stayed frozen above the
        // threshold forever, threat never escalated, and the tracker could
        // go stale while the (noisy) signal was still present.
        if (config_.rssi_variance_enabled) {
            const uint32_t variance = tracked_drones_[index].calculate_rssi_variance();
            if (variance > static_cast<uint32_t>(DEFAULT_RSSI_VARIANCE_THRESHOLD)) {
                tracked_drones_[index].observe_rssi(rssi, timestamp);
                // RSSI too chaotic — likely noise, don't upgrade threat
                return ErrorCode::SUCCESS;
            }
        }

        // Update drone type from DB if it was UNKNOWN (DB may have loaded after first detection)
        if (tracked_drones_[index].drone_type == DroneType::UNKNOWN) {
            tracked_drones_[index].drone_type = determine_drone_type_internal(frequency);
        }

        // After merge, absorb_from() correctly set threat = max(survivor, absorbed).
        // Capture this BEFORE update_rssi() reclassifies from current RSSI.
        const ThreatLevel post_merge_threat = tracked_drones_[index].get_threat();
        const bool merge_occurred = last_merge_absorbed_;
        last_merge_absorbed_ = false;

        ThreatLevel old_threat = post_merge_threat;
        tracked_drones_[index].update_rssi(rssi, timestamp, ThreatThresholds{
            config_.threat_low_dbm, config_.threat_medium_dbm,
            config_.threat_high_dbm, config_.threat_critical_dbm});
        ThreatLevel new_threat = tracked_drones_[index].get_threat();

        // MRG safety: when a merge occurred, preserve the merged (higher) threat
        // level if update_rssi() would downgrade it. In sweep mode, update_rssi()
        // classifies from direct new_rssi (not peak of history), so a weak
        // detection merging into a CRITICAL drone must not drop its threat.
        // In normal mode, peak-based classification already prevents this, but
        // this guard is a cheap safety net.
        if (merge_occurred && post_merge_threat > new_threat) {
            tracked_drones_[index].threat_level = post_merge_threat;
            new_threat = post_merge_threat;
        }
        
        if (new_threat > old_threat) {
            trigger_alert(new_threat);
            // Real signal confirmed — clear noise blacklist for this frequency
            reset_noise_count(frequency);
        }
    } else {
        // New drone — add and alert for its initial threat
        const size_t new_index = tracked_count_;
        ErrorCode add_result = add_tracked_drone_internal(frequency, rssi, timestamp);
        if (add_result != ErrorCode::SUCCESS) {
            return add_result;
        }
        if (out_index != nullptr) *out_index = new_index;
        // First detection: mark as increasing to prevent immediate decay
        // update_rssi() already sets last_rssi_ to the current RSSI
        tracked_drones_[new_index].rssi_increased_ = true;
        // Alert for newly added drone's actual threat level
        ThreatLevel new_threat = tracked_drones_[new_index].get_threat();
        if (new_threat > ThreatLevel::NONE) {
            trigger_alert(new_threat);
        }
    }

    return ErrorCode::SUCCESS;
}

ErrorResult<size_t> DroneScanner::find_nearest_drone_internal(
    FreqHz frequency,
    FreqHz radius_hz
) const noexcept {
    // Stack: ~32 bytes. FreqHz is uint64_t — unsigned difference is computed
    // by branch, never by signed cast, so no underflow is possible.
    ErrorResult<size_t> best_match = ErrorResult<size_t>::failure(ErrorCode::INVALID_PARAMETER);
    FreqHz best_diff = 0;

    for (size_t i = 0; i < tracked_count_; ++i) {
        const FreqHz entry_freq = tracked_drones_[i].frequency;
        const FreqHz diff = (frequency > entry_freq) ? (frequency - entry_freq)
                                                     : (entry_freq - frequency);
        if (diff > radius_hz) {
            continue;  // Outside match radius — not the same emitter
        }
        if (!best_match.has_value() || (diff < best_diff)) {
            best_match = ErrorResult<size_t>::success(i);
            best_diff = diff;
            if (diff == 0) {
                break;  // Exact match — cannot improve, stop scanning
            }
        }
    }

    return best_match;
}

ErrorResult<size_t> DroneScanner::match_and_consolidate_drone_internal(
    FreqHz frequency,
    FreqHz radius_hz
) noexcept {
    // Stack: ~28 bytes (bool in_radius[20] + scalars). O(n²) worst case with
    // n <= 20 (190 pair checks); the absorb path is rare (duplicate healing),
    // the common path is a single O(n) scan with zero absorptions.
    if (tracked_count_ == 0) {
        return ErrorResult<size_t>::failure(ErrorCode::INVALID_PARAMETER);
    }

    // Pass 1: mark every entry within the match radius of the detection.
    // FreqHz is uint64_t — unsigned difference is computed by branch, never
    // by signed cast, so no underflow is possible.
    bool in_radius[MAX_TRACKED_DRONES];
    size_t in_radius_count = 0;
    for (size_t i = 0; i < tracked_count_; ++i) {
        const FreqHz entry_freq = tracked_drones_[i].frequency;
        const FreqHz diff = (frequency > entry_freq) ? (frequency - entry_freq)
                                                     : (entry_freq - frequency);
        in_radius[i] = (diff <= radius_hz);
        if (in_radius[i]) {
            ++in_radius_count;
        }
    }

    // No candidate within radius — nothing to match, nothing to consolidate.
    if (in_radius_count == 0) {
        return ErrorResult<size_t>::failure(ErrorCode::INVALID_PARAMETER);
    }

    // Pass 2: survivor = highest threat_level in-radius entry. Higher-severity
    // threats must NOT be absorbed into lower-severity ones — a CRITICAL drone
    // must survive over a LOW drone regardless of age. Ties broken by oldest
    // created_time_ (longest trend history), then lowest index.
    size_t survivor = 0;
    ThreatLevel best_threat = ThreatLevel::NONE;
    SystemTime oldest_created = 0;
    bool survivor_set = false;
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (!in_radius[i]) {
            continue;
        }
        const ThreatLevel threat = tracked_drones_[i].threat_level;
        const SystemTime created = tracked_drones_[i].created_time_;
        if (!survivor_set
            || static_cast<uint8_t>(threat) > static_cast<uint8_t>(best_threat)
            || (threat == best_threat && created < oldest_created)) {
            survivor = i;
            best_threat = threat;
            oldest_created = created;
            survivor_set = true;
        }
    }

    // Pass 3: absorb every OTHER in-radius entry into the survivor. Each
    // duplicate's RSSI history, cycle peaks and decay state merge into the
    // survivor, so the trend sees one continuous sample stream.
    last_merge_absorbed_ = (in_radius_count > 1);
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (in_radius[i] && i != survivor) {
            tracked_drones_[survivor].absorb_from(tracked_drones_[i]);
        }
    }

    // Pass 4: compact the array (same pattern as remove_stale_drones_internal),
    // dropping the absorbed duplicates, and record the survivor's new index.
    size_t write_index = 0;
    size_t survivor_new_index = SIZE_MAX;
    for (size_t read_index = 0; read_index < tracked_count_; ++read_index) {
        if (in_radius[read_index] && read_index != survivor) {
            continue;  // absorbed duplicate — drop
        }
        if (read_index == survivor) {
            survivor_new_index = write_index;
        }
        if (write_index != read_index) {
            tracked_drones_[write_index] = tracked_drones_[read_index];
        }
        ++write_index;
    }
    tracked_count_ = write_index;

    return ErrorResult<size_t>::success(survivor_new_index);
}

ErrorCode DroneScanner::get_current_drone_type(char* buffer, size_t buffer_size) const noexcept {
    if (buffer == nullptr || buffer_size < 2) {
        return ErrorCode::INVALID_PARAMETER;
    }

    size_t copy_len = 0;
    for (size_t i = 0; i < 4 && i < (buffer_size - 1); ++i) {
        const char c = current_drone_type_[i];
        if (c != '\0') {
            buffer[i] = c;
            copy_len++;
        } else {
            break;
        }
    }

    buffer[copy_len] = '\0';
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::add_tracked_drone_internal(
    FreqHz frequency_hz,
    RssiValue rssi_dbm,
    SystemTime timestamp_ms
) noexcept {
    if (tracked_count_ >= MAX_TRACKED_DRONES) {
        return ErrorCode::BUFFER_FULL;
    }

    DroneType type = determine_drone_type_internal(frequency_hz);

    tracked_drones_[tracked_count_] = TrackedDrone(frequency_hz, type, ThreatLevel::NONE);
    tracked_drones_[tracked_count_].created_time_ = timestamp_ms;
    tracked_drones_[tracked_count_].last_increase_time_ = timestamp_ms;
    tracked_drones_[tracked_count_].update_rssi(rssi_dbm, timestamp_ms, ThreatThresholds{
        config_.threat_low_dbm, config_.threat_medium_dbm,
        config_.threat_high_dbm, config_.threat_critical_dbm});

    tracked_count_++;
    statistics_.drones_detected++;

    // NOTE: Do NOT call trigger_alert() here.
    // update_rssi() set the initial threat level inside the drone.
    // update_tracked_drone_internal() will compare and trigger the alert.

    return ErrorCode::SUCCESS;
}

DroneType DroneScanner::determine_drone_type_internal(FreqHz frequency) const noexcept {
    ErrorResult<FrequencyEntry> entry_result = database_.find_entry(frequency);
    
    if (entry_result.has_value()) {
        const FrequencyEntry& entry = entry_result.value();
        return entry.drone_type;
    }
    
    return DroneType::UNKNOWN;
}

size_t DroneScanner::get_tracked_drones(
    TrackedDrone* drones,
    size_t max_count
) const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    
    if (drones == nullptr || max_count == 0) {
        return 0;
    }
    
    size_t copy_count = tracked_count_ < max_count ? tracked_count_ : max_count;
    
    for (size_t i = 0; i < copy_count; ++i) {
        // BUGFIX (trend flicker / '-' reset): advance the trend hysteresis on
        // the PERSISTENT tracked_drones_[i] object BEFORE copying.
        //
        // get_movement_trend() is const but mutates mutable hysteresis state
        // (cached_trend_ / trend_hold_count_). It is only ever invoked from
        // DisplayDroneEntry's constructor, which historically ran on a COPY
        // (refresh_drones_[i] here). The copy is discarded after painting, so
        // the hysteresis never accumulated across ticks: cached_trend_ on the
        // persistent object stayed UNKNOWN forever.
        //
        // Consequence in sweep mode: finalize_sweep_cycle() resets
        // last_cycle_peak_rssi_ to the sentinel at every pair boundary, so raw
        // trend is UNKNOWN until the drone is re-detected in the next pass.
        // The hysteresis branch "raw UNKNOWN + cached real → hold last real
        // trend" was designed to cover exactly this gap, but it never fired —
        // every evaluation started from a fresh copy with cached UNKNOWN — and
        // ALL trends collapsed to '-' (UNKNOWN) after the second sweep window
        // completed its pass (pair boundary = finalize).
        //
        // Evaluating on the original makes cached_trend_ persist, so during the
        // sentinel gap the UI holds the last real trend ('<' / '>' / '~').
        // Stack: ~8 bytes. Runs under DATA_MUTEX (UI thread; scanner thread is
        // stopped in sweep mode and locks the same mutex in normal mode).
        (void)tracked_drones_[i].get_movement_trend();
        drones[i] = tracked_drones_[i];
    }
    
    return copy_count;
}

ScannerState DroneScanner::get_state() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (lock.is_locked()) {
        return state_;
    }
    // Fallback: return last known state (atomic word-sized read is safe on Cortex-M4)
    return state_;
}

bool DroneScanner::is_scanning() const noexcept {
    return scanning_active_.test();
}

void DroneScanner::get_config(ScanConfig& out) const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    out = config_;
}

int32_t DroneScanner::get_threat_critical_dbm() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return RSSI_CRITICAL_THREAT_THRESHOLD_DBM;
    }
    return config_.threat_critical_dbm;
}

bool DroneScanner::get_median_enabled() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return false;
    }
    return config_.median_enabled;
}

uint32_t DroneScanner::get_scan_interval_ms() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return SCANNER_SLEEP_MS;
    }
    return config_.scan_interval_ms;
}

bool DroneScanner::is_adaptive_cfar_enabled() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return false;
    }
    return config_.adaptive_cfar_enabled;
}

ErrorCode DroneScanner::set_config(const ScanConfig& config) noexcept {
    ErrorCode validate_result = validate_config_internal(config);
    if (validate_result != ErrorCode::SUCCESS) {
        return validate_result;
    }
    
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    config_ = config;
    median_filter_enabled_ = config.median_enabled;

    // Propagate configurable threat thresholds to RSSI detector
    rssi_detector_.set_threat_thresholds(ThreatThresholds{
        config.threat_low_dbm, config.threat_medium_dbm,
        config.threat_high_dbm, config.threat_critical_dbm});

    return ErrorCode::SUCCESS;
}

ErrorCode DroneScanner::validate_config_internal(const ScanConfig& config) const noexcept {
    // Validate against HARDWARE limits (not theoretical MAX_FREQUENCY_HZ)
    // HackRF One RFFC5072 mixer practical limit: 6 GHz
    if (config.start_frequency < HARDWARE_MIN_FREQ_HZ ||
        config.end_frequency > HARDWARE_MAX_FREQ_HZ ||
        config.start_frequency >= config.end_frequency) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (config.rssi_threshold_dbm < RSSI_MIN_DBM ||
        config.rssi_threshold_dbm > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Validate scanning mode
    const uint8_t mode_value = static_cast<uint8_t>(config.mode);
    if (mode_value >= SCANNING_MODE_COUNT) {
        return ErrorCode::INVALID_PARAMETER;
    }

    // Validate sweep windows against hardware limits
    if (config.sweep_start_freq < HARDWARE_MIN_FREQ_HZ ||
        config.sweep_end_freq > HARDWARE_MAX_FREQ_HZ ||
        config.sweep_start_freq >= config.sweep_end_freq) {
        return ErrorCode::INVALID_PARAMETER;
    }
    if (config.sweep2_enabled) {
        if (config.sweep2_start_freq < HARDWARE_MIN_FREQ_HZ ||
            config.sweep2_end_freq > HARDWARE_MAX_FREQ_HZ ||
            config.sweep2_start_freq >= config.sweep2_end_freq) {
            return ErrorCode::INVALID_PARAMETER;
        }
    }
    if (config.sweep3_enabled) {
        if (config.sweep3_start_freq < HARDWARE_MIN_FREQ_HZ ||
            config.sweep3_end_freq > HARDWARE_MAX_FREQ_HZ ||
            config.sweep3_start_freq >= config.sweep3_end_freq) {
            return ErrorCode::INVALID_PARAMETER;
        }
    }
    if (config.sweep4_enabled) {
        if (config.sweep4_start_freq < HARDWARE_MIN_FREQ_HZ ||
            config.sweep4_end_freq > HARDWARE_MAX_FREQ_HZ ||
            config.sweep4_start_freq >= config.sweep4_end_freq) {
            return ErrorCode::INVALID_PARAMETER;
        }
    }

    // Validate spectrum shape filter parameters (prevent impossible detection configs)
    if (config.spectrum_min_width > config.spectrum_max_width) {
        return ErrorCode::INVALID_PARAMETER;
    }
    if (config.spectrum_min_width < 1 || config.spectrum_max_width < 2) {
        return ErrorCode::INVALID_PARAMETER;
    }
    if (config.confirm_count < CONFIRM_COUNT_MIN || config.confirm_count > CONFIRM_COUNT_MAX) {
        return ErrorCode::INVALID_PARAMETER;
    }
    if (config.miss_tolerance < MISS_TOLERANCE_MIN || config.miss_tolerance > MISS_TOLERANCE_MAX) {
        return ErrorCode::INVALID_PARAMETER;
    }
    if (config.cfar_ref_cells < CFAR_REF_CELLS_MIN || config.cfar_ref_cells > CFAR_REF_CELLS_MAX) {
        return ErrorCode::INVALID_PARAMETER;
    }
    if (config.cfar_guard_cells > CFAR_GUARD_CELLS_MAX) {
        return ErrorCode::INVALID_PARAMETER;
    }
    // CFAR ref cells must be >= guard cells + 2 for valid reference window
    if (config.cfar_ref_cells < config.cfar_guard_cells + 2) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

ScanStatistics DroneScanner::get_statistics() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    return statistics_;
}

void DroneScanner::reset_statistics() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    statistics_.reset();
}

ErrorResult<FreqHz> DroneScanner::get_current_frequency() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (current_frequency_ == 0) {
        return ErrorResult<FreqHz>::failure(ErrorCode::HARDWARE_NOT_INITIALIZED);
    }
    return ErrorResult<FreqHz>::success(current_frequency_);
}

void DroneScanner::set_scan_frequency(FreqHz frequency) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    current_frequency_ = frequency;
}

void DroneScanner::clear_lock_state() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    freq_lock_count_ = 0;
    locked_frequency_ = 0;
    missed_lock_count_ = 0;
    // Clear lock timing timers for consistency
    lock_start_time_ = 0;
    confirm_start_time_ = 0;
    // Reset sweep frequency tracking so first sweep frame always resets median filter
    last_sweep_freq_ = 0;
    if (state_ == ScannerState::LOCKING || state_ == ScannerState::TRACKING) {
        state_ = ScannerState::SCANNING;
    }
}

size_t DroneScanner::get_tracked_count() const noexcept {
    MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
    if (!lock.is_locked()) {
        return tracked_count_;  // Atomic word-read fallback on Cortex-M4
    }
    return tracked_count_;
}

void DroneScanner::clear_tracked_drones() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    tracked_count_ = 0;
}

void DroneScanner::reset_frequency() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);

    // Get first frequency from new database
    ErrorResult<FreqHz> freq_result = database_.get_next_frequency(0);
    if (freq_result.has_value()) {
        current_frequency_ = freq_result.value();
    } else {
        current_frequency_ = MIN_FREQUENCY_HZ;
    }

    // Reset tracking state
    freq_lock_count_ = 0;
    locked_frequency_ = 0;
    missed_lock_count_ = 0;
    // Reset lock timing for consistency
    lock_start_time_ = 0;
    confirm_start_time_ = 0;
}

void DroneScanner::remove_stale_drones(SystemTime current_time) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    remove_stale_drones_internal(current_time);
}

void DroneScanner::remove_stale_drones_internal(SystemTime current_time) noexcept {
    size_t write_index = 0;
    
    for (size_t read_index = 0; read_index < tracked_count_; ++read_index) {
        if (!tracked_drones_[read_index].is_stale(current_time, config_.stale_timeout_ms)) {
            if (write_index != read_index) {
                tracked_drones_[write_index] = tracked_drones_[read_index];
            }
            write_index++;
        }
    }
    
    tracked_count_ = write_index;
}

/**
 * @brief Sets the alert callback function
 * @param callback Function to call when alerts are triggered
 * @note The callback function MUST be thread-safe and reentrant-safe
 * @note The callback MUST NOT acquire any mutexes or perform blocking operations
 * @note The callback MUST execute quickly (preferably < 1ms) to avoid delaying scanner thread
 * @note Failure to meet timing requirements may result in missed alerts or buffer overflows
 * @warning Violating these constraints can cause system instability
 */
void DroneScanner::set_alert_callback(ThreatAlertCallback callback) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    alert_callback_ = callback;
}

void DroneScanner::trigger_alert(ThreatLevel threat_level) noexcept {
    // Never alert for NONE level — this fires on threat decrease or initial NONE state
    if (threat_level == ThreatLevel::NONE) {
        return;
    }

    ThreatAlertCallback local_callback = alert_callback_;

    if (local_callback == nullptr) {
        return;
    }

    // Re-entrancy guard (AtomicFlag is lock-free)
    if (alert_callback_in_progress_.test_and_set()) {
        return;  // Already in progress
    }

    // Invoke callback outside any lock
    local_callback(threat_level);

    alert_callback_in_progress_.clear();
}

void DroneScanner::set_median_filter_enabled(bool enabled) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    median_filter_enabled_ = enabled;
    config_.median_enabled = enabled;
    rssi_median_filter_.reset();
}

void DroneScanner::reset_neighbor_checker() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(mutex_);
    neighbor_margin_checker_.reset();
}

// ============================================================================
// Spectrum Shape Analysis — detect U/V peaks above flat noise floor
// ============================================================================

bool DroneScanner::analyze_spectrum_shape(const ChannelSpectrum& spectrum, int32_t& out_rssi) noexcept {
    // Step 1: Find noise floor via 25th percentile of usable bins.
    // 25th percentile (not median) is more robust when signal occupies >50% of
    // usable bins (WiFi-dense 2.4 GHz environment). Median becomes signal-biased
    // in that case, inflating the noise floor estimate and causing real drone
    // signals to fail the peak_margin check.
    uint8_t* sorted = spectrum_sort_buf_;
    size_t sort_count = 0;
    for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
        if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
        sorted[sort_count++] = spectrum.db[i];
    }
    const uint8_t noise_floor = (sort_count > 0)
        ? quickselect_percentile(sorted, sort_count, 25)
        : 0;

    // Step 2: Find peak bin and peak value
    uint8_t peak_value = noise_floor;
    size_t peak_index = FFT_EDGE_SKIP;

    if (config_.cfar_mode != CFARMode::OFF) {
        // Use adaptive threshold ONLY when explicitly enabled.
        // When disabled, use the user-configured fixed threshold directly.
        // Prevents silent threshold drift from AdaptiveThreshold's internal state
        // when the user has not opted into auto-tuning.
        const uint8_t effective_threshold = config_.adaptive_cfar_enabled
            ? adaptive_threshold_.get_optimal_threshold()
            : config_.cfar_threshold_x10;

        const size_t cfar_peak = CFARDetector::find_peak_cfar(
            spectrum.db.data(), FFT_BIN_COUNT,
            config_.cfar_mode, config_.cfar_ref_cells, config_.cfar_guard_cells,
            effective_threshold, FFT_EDGE_SKIP, FFT_EDGE_SKIP,
            config_.cfar_hybrid_alpha, config_.cfar_hybrid_beta, config_.cfar_hybrid_gamma,
            config_.os_cfar_k_percent, config_.vi_cfar_threshold_x10
        );

        if (cfar_peak >= FFT_BIN_COUNT) {
            // No detection — feed negative result to adaptive threshold
            // only when adaptive mode is active (avoids polluting internal state).
            if (config_.adaptive_cfar_enabled) {
                adaptive_threshold_.update(
                    false, RSSI_MIN_DBM,
                    spectrum_value_to_dbm(noise_floor, get_current_total_gain()),
                    config_.cfar_threshold_x10);
            }
            return false;
        }

        // Detection found — feed positive result to adaptive threshold
        if (config_.adaptive_cfar_enabled) {
            adaptive_threshold_.update(
                true,
                spectrum_value_to_dbm(spectrum.db[cfar_peak], get_current_total_gain()),
                spectrum_value_to_dbm(noise_floor, get_current_total_gain()),
                config_.cfar_threshold_x10);
        }

        peak_index = cfar_peak;
        peak_value = spectrum.db[cfar_peak];
    } else {
        for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            if (spectrum.db[i] > peak_value) {
                peak_value = spectrum.db[i];
                peak_index = i;
            }
        }
    }

    const int32_t total_gain = get_current_total_gain();
    return analyze_spectrum_shape_impl(spectrum, peak_index, peak_value, noise_floor, out_rssi, FFT_EDGE_SKIP, total_gain);
}

bool DroneScanner::analyze_spectrum_shape_impl(
    const ChannelSpectrum& spectrum,
    size_t peak_index,
    uint8_t raw_peak,
    uint8_t noise_floor,
    int32_t& out_rssi,
    size_t edge_skip,
    int32_t total_gain
) noexcept {
    return apply_shape_filters(
        spectrum.db.data(), peak_index, raw_peak, noise_floor,
        out_rssi, edge_skip, /*has_dc_gap=*/true, total_gain);
}

// ============================================================================
// Multi-peak shape analysis: find ALL CFAR peaks, try shape filters on each
// Critical for detecting multiple FPV transmitters in the same 20 MHz frame.
// ============================================================================

bool DroneScanner::analyze_spectrum_shape_multi(
    const ChannelSpectrum& spectrum,
    ShapeDetectionResult& out_result,
    FreqHz center_freq
) noexcept {
    out_result.count = 0;

    // Step 1: Noise floor (25th percentile of usable bins)
    uint8_t* sorted = spectrum_sort_buf_;
    size_t sort_count = 0;
    for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
        if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
        sorted[sort_count++] = spectrum.db[i];
    }
    const uint8_t noise_floor = (sort_count > 0)
        ? quickselect_percentile(sorted, sort_count, 25)
        : 0;
    out_result.noise_floor = noise_floor;

    const int32_t total_gain = get_current_total_gain();

    if (config_.cfar_mode != CFARMode::OFF) {
        // Find ALL CFAR-passing peaks (up to MAX_SHAPE_DETECTIONS)
        CFARPeak cfar_peaks[MAX_SHAPE_DETECTIONS];
        const uint8_t effective_threshold = config_.adaptive_cfar_enabled
            ? adaptive_threshold_.get_optimal_threshold()
            : config_.cfar_threshold_x10;

        const size_t peak_count = CFARDetector::find_peaks(
            spectrum.db.data(), FFT_BIN_COUNT,
            cfar_peaks, MAX_SHAPE_DETECTIONS,
            config_.cfar_mode, config_.cfar_ref_cells, config_.cfar_guard_cells,
            effective_threshold, FFT_EDGE_SKIP, FFT_EDGE_SKIP,
            config_.cfar_hybrid_alpha, config_.cfar_hybrid_beta, config_.cfar_hybrid_gamma,
            config_.os_cfar_k_percent, config_.vi_cfar_threshold_x10
        );

        if (peak_count == 0) {
            // No CFAR detection — feed negative result only when adaptive active
            if (config_.adaptive_cfar_enabled) {
                adaptive_threshold_.update(
                    false, RSSI_MIN_DBM,
                    spectrum_value_to_dbm(noise_floor, total_gain),
                    config_.cfar_threshold_x10);
            }
            return false;
        }

        // Feed first (strongest) peak to adaptive threshold (only when active)
        if (config_.adaptive_cfar_enabled) {
            adaptive_threshold_.update(
                true,
                spectrum_value_to_dbm(cfar_peaks[0].power, total_gain),
                spectrum_value_to_dbm(noise_floor, total_gain),
                config_.cfar_threshold_x10);
        }

        // Try shape filters on each CFAR peak independently
        for (size_t i = 0; i < peak_count && out_result.count < MAX_SHAPE_DETECTIONS; ++i) {
            int32_t peak_rssi = RSSI_MIN_DBM;
            if (apply_shape_filters(
                    spectrum.db.data(), cfar_peaks[i].bin, cfar_peaks[i].power,
                    noise_floor, peak_rssi, FFT_EDGE_SKIP, /*has_dc_gap=*/true, total_gain)) {
                ShapeDetection& det = out_result.detections[out_result.count];
                det.frequency = normal_bin_to_freq(center_freq, cfar_peaks[i].bin, spectrum.sampling_rate);
                det.rssi = peak_rssi;
                det.bin_index = static_cast<uint16_t>(cfar_peaks[i].bin);
                det.peak_power = cfar_peaks[i].power;
                out_result.count++;
            }
        }
    } else {
        // No CFAR — find top N peaks by simple maximum (non-CFAR fallback)
        struct SimplePeak { size_t bin; uint8_t power; };
        SimplePeak candidates[MAX_SHAPE_DETECTIONS * 2];
        size_t cand_count = 0;

        uint8_t max_val = noise_floor;
        size_t max_bin = FFT_EDGE_SKIP;
        for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            if (spectrum.db[i] > max_val) {
                max_val = spectrum.db[i];
                max_bin = i;
            }
        }

        if (max_val <= noise_floor) return false;

        // Collect the global max and immediate neighbors as candidates
        candidates[cand_count++] = {max_bin, max_val};
        // Check bins ±5 around the maximum for secondary peaks
        for (int32_t offset = -5; offset <= 5; offset += 10) {
            const size_t check_bin = static_cast<size_t>(static_cast<int32_t>(max_bin) + offset);
            if (check_bin >= FFT_BIN_COUNT || check_bin == max_bin) continue;
            if (check_bin >= FFT_DC_SPIKE_START && check_bin < FFT_DC_SPIKE_END) continue;
            if (spectrum.db[check_bin] > noise_floor + config_.spectrum_margin) {
                candidates[cand_count++] = {check_bin, spectrum.db[check_bin]};
            }
        }

        for (size_t i = 0; i < cand_count && out_result.count < MAX_SHAPE_DETECTIONS; ++i) {
            int32_t peak_rssi = RSSI_MIN_DBM;
            if (apply_shape_filters(
                    spectrum.db.data(), candidates[i].bin, candidates[i].power,
                    noise_floor, peak_rssi, FFT_EDGE_SKIP, /*has_dc_gap=*/true, total_gain)) {
                ShapeDetection& det = out_result.detections[out_result.count];
                det.frequency = normal_bin_to_freq(center_freq, candidates[i].bin, spectrum.sampling_rate);
                det.rssi = peak_rssi;
                det.bin_index = static_cast<uint16_t>(candidates[i].bin);
                det.peak_power = candidates[i].power;
                out_result.count++;
            }
        }
    }

    return out_result.count > 0;
}

bool DroneScanner::analyze_spectrum_shape_lg(
    const uint8_t* lg_buffer,
    size_t peak_pixel,
    uint8_t noise_floor,
    int32_t& out_rssi,
    int32_t total_gain
) noexcept {
    if (peak_pixel >= COMPOSITE_SIZE) return false;

    const uint8_t raw_peak = lg_buffer[peak_pixel];
    if (raw_peak <= noise_floor) return false;

    return apply_shape_filters(
        lg_buffer, peak_pixel, raw_peak, noise_floor,
        out_rssi, LG_EDGE_SKIP_PX, /*has_dc_gap=*/false, total_gain);
}

uint8_t DroneScanner::effective_spectrum_margin() const noexcept {
    // Single source of truth for the sensitive-mode margin relaxation.
    // Applied identically by the sweep candidate gate (process_spectrum_sweep)
    // and by apply_shape_filters Step 3, in BOTH normal and sweep mode.
    if (config_.sensitive_mode) {
        return (config_.spectrum_margin > 2)
            ? static_cast<uint8_t>(config_.spectrum_margin - 2)
            : 1;
    }
    return config_.spectrum_margin;
}

bool DroneScanner::apply_shape_filters(
    const uint8_t* data,
    size_t peak_idx,
    uint8_t raw_peak,
    uint8_t noise_floor,
    int32_t& out_rssi,
    size_t edge_skip,
    bool has_dc_gap,
    int32_t total_gain
) const noexcept {
    const uint8_t peak_margin = raw_peak - noise_floor;

    // VERY STRONG SIGNAL BYPASS (opt-in via config_.shape_bypass_enabled):
    // When enabled AND peak_margin > 80 (~16 dB above noise, e.g. drone
    // within ~50m), three effects activate:
    //   1) Switch elevated_threshold from margin/3 to margin/2, narrowing the
    //      width measurement to prevent inflation from elevated flanking bins.
    //   2) Skip valley depth, symmetry, and kurtosis checks (unreliable when
    //      all bins in the signal band are elevated).
    //   3) At peak_margin > 96 (~19 dB), also skip max_width check because
    //      even the narrowed threshold may still capture the full bandwidth.
    // Default (shape_bypass_enabled == false): very_strong is always false —
    // max_width, valley, symmetry and kurtosis are enforced on EVERY signal
    // regardless of strength, so a powerful flat WiFi/BT carrier can never
    // bypass MaxW (bypass used to be implicit and always-on, which let
    // close-range wideband signals through even with MaxW=5).
    // Stack: 1 byte (local bool). Flash: 0 (inline).
    const bool very_strong = config_.shape_bypass_enabled
        && (peak_margin > VERY_STRONG_SIGNAL_MARGIN);

    // Sensitivity-adaptive filter scaling:
    // At high sensitivity (low threshold), the RSSI gate is wide open and shape
    // filters must work harder to reject noise. We derive a sensitivity factor
    // from the RSSI threshold: 0 at default (-95 dBm), positive at high sensitivity.
    // At low sensitivity (strict threshold), the RSSI gate does most of the work
    // so shape filters stay at defaults (no loosening).
    const int32_t rssi_sens = -(config_.rssi_threshold_dbm + 95);

    // Step 3: Peak must be significantly above noise floor
    // Sensitive mode relaxes the margin by 2 (min 1) via
    // effective_spectrum_margin() — the SAME reduction the sweep candidate
    // gate applies. Previously the reduction was nullified here (this step
    // re-checked the full config margin), so peaks with margin 3-4 passed
    // CFAR and were then rejected — the sensitive relaxation was dead code.
    // Non-sensitive mode scales the margin UP at high RSSI sensitivity
    // (+1 unit per 2 sensitivity points above the -95 dBm default): the RSSI
    // gate is wide open there, so shape filters must work harder.
    // At sens=75 (default): effective_margin = spectrum_margin (no change).
    // At sens=87 (rssi_sens=12): effective_margin = spectrum_margin + 6.
    // In sensitive mode the scaling is NOT applied — the user explicitly opts
    // for maximum weak-signal sensitivity, and the scaling would partially
    // cancel the relaxation.
    const uint8_t base_margin = effective_spectrum_margin();
    const uint8_t effective_margin = (!config_.sensitive_mode && rssi_sens > 0)
        ? static_cast<uint8_t>(base_margin + rssi_sens / 2)
        : base_margin;
    if (peak_margin < effective_margin) return false;

    // Step 4: Count elevated bins around peak (signal width)
    // /3 instead of /4: for weak signals (peak_margin=20), /4 gives 5 units above
    // noise (~1 dB) where 1-bin quantization noise dominates width measurement.
    // /3 gives 7 units (~1.4 dB), providing more stable width for marginal signals.
    // Very strong signals use /2 instead: at close range, ALL bins in the signal
    // band are elevated, so /3 captures too many bins and inflates width beyond
    // max_width, causing false rejection. /2 narrows the threshold to the actual
    // peak region, producing a reliable width measurement.
    const uint8_t elevated_divisor = very_strong ? 2 : 3;
    const uint8_t elevated_threshold = noise_floor + (peak_margin / elevated_divisor);

    // Upper bound: FFT_BIN_COUNT for raw spectrum, COMPOSITE_SIZE for LG buffer
    const size_t data_size = has_dc_gap ? FFT_BIN_COUNT : COMPOSITE_SIZE;
    const size_t upper_limit = data_size - edge_skip;

    // Width expansion: extend left/right while bins are above elevated_threshold.
    // DC spike (bins 120-135) is a hard boundary — expansion stops, never bridges.
    // The DC spike contains ADC offset energy (not real signal), so including it
    // inflates width by up to 16 bins (1.25 MHz), causing false rejects (max_width)
    // or false accepts (min_width) for signals near DC.
    size_t left = peak_idx;
    while (left > edge_skip) {
        const size_t prev = left - 1;
        if (has_dc_gap && prev >= FFT_DC_SPIKE_START && prev < FFT_DC_SPIKE_END) {
            break;
        }
        if (data[prev] < elevated_threshold) break;
        --left;
    }

    size_t right = peak_idx;
    while (right < upper_limit - 1) {
        const size_t next = right + 1;
        if (has_dc_gap && next >= FFT_DC_SPIKE_START && next < FFT_DC_SPIKE_END) {
            break;
        }
        if (data[next] < elevated_threshold) break;
        ++right;
    }

    const size_t signal_width = right - left + 1;

    // Step 5: Minimum width
    if (signal_width < config_.spectrum_min_width) return false;

    // Step 6: Maximum width
    // Opt-in bypass only (shape_bypass_enabled): at peak_margin > 96 (~19 dB),
    // ALL bins in the signal band are elevated, making width measurement
    // unreliable even with the narrowed /2 threshold. Skip max_width check.
    // At peak_margin 81-96: use config max_width with the narrowed /2
    // threshold — width is now reliable enough to enforce.
    // Default (bypass disabled): max_width is ALWAYS enforced — the MaxW
    // value from Settings is authoritative at any signal strength.
    if (!very_strong || peak_margin <= EXTREME_SIGNAL_MARGIN) {
        if (signal_width > config_.spectrum_max_width) return false;
    }

    // Step 7: Peak sharpness (enforce inverted-V shape)
    // Sensitive mode: skip — at low SNR noise bins inflate avg_margin and the
    // peak/avg ratio becomes unreliable (same rationale as valley/flatness).
    int32_t avg_margin = 0;
    if (config_.spectrum_peak_sharpness > 50 && !config_.sensitive_mode) {
        int32_t margin_sum = 0;
        size_t count = 0;
        for (size_t i = left; i <= right; ++i) {
            if (has_dc_gap && i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            if (data[i] > noise_floor) {
                margin_sum += (data[i] - noise_floor);
                ++count;
            }
        }
        if (count > 0) {
            avg_margin = margin_sum / static_cast<int32_t>(count);
        }

        if (avg_margin <= 0) return false;
        const int32_t sharpness = (static_cast<int32_t>(peak_margin) * 100) / avg_margin;
        if (sharpness < config_.spectrum_peak_sharpness) return false;
    }

    // Step 8: Peak ratio (tall+narrow = inverted-V)
    if (config_.spectrum_peak_ratio > 0) {
        const int32_t ratio = (static_cast<int32_t>(peak_margin) * 10) / static_cast<int32_t>(signal_width);
        if (ratio < config_.spectrum_peak_ratio) return false;
    }

    // Step 9: Valley depth (deep valleys flanking peak = V-shape)
    // Skip for dual-peak signals (FPV video + audio subcarrier): the valley
    // between two legitimate peaks is NOT a rejection criterion. Detect by
    // checking if any bin within the signal width exceeds half peak power.
    // Very strong signal bypass: flanking bins ARE the signal at close range.
    // Sensitive mode bypass: valley depth is unreliable for weak signals.
    if (config_.spectrum_valley_depth > 0 && !very_strong && !config_.sensitive_mode) {
        bool has_secondary_peak = false;
        const uint8_t secondary_threshold = noise_floor + (peak_margin / 2);
        for (size_t i = left; i <= right && !has_secondary_peak; ++i) {
            if (has_dc_gap && i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            if (i == peak_idx) continue;
            if (data[i] >= secondary_threshold) {
                has_secondary_peak = true;
            }
        }

        if (!has_secondary_peak) {
            uint8_t left_valley_margin = 0;
            uint8_t right_valley_margin = 0;

            if (left > edge_skip) {
                const size_t lv = left - 1;
                const bool dc_blocked = has_dc_gap && lv >= FFT_DC_SPIKE_START && lv < FFT_DC_SPIKE_END;
                if (!dc_blocked && data[lv] > noise_floor) {
                    left_valley_margin = data[lv] - noise_floor;
                }
            }
            if (right < upper_limit - 1) {
                const size_t rv = right + 1;
                const bool dc_blocked = has_dc_gap && rv >= FFT_DC_SPIKE_START && rv < FFT_DC_SPIKE_END;
                if (!dc_blocked && data[rv] > noise_floor) {
                    right_valley_margin = data[rv] - noise_floor;
                }
            }

            const uint8_t max_valley = (left_valley_margin > right_valley_margin)
                ? left_valley_margin : right_valley_margin;
            if (max_valley >= config_.spectrum_valley_depth) return false;
        }
    }

    // Step 10: Flatness (reject flat-top signals like WiFi/BT)
    // Guard: skip for weak signals where flatness measurement is unreliable.
    // At high sensitivity, lower the guard so flatness activates earlier —
    // this is critical because flatness is the primary WiFi/BT rejection filter.
    // At default sensitivity: guard = FLATNESS_MIN_PEAK_MARGIN (40, ~8 dB).
    // At sens=87 (rssi_sens=12): guard = max(15, 40-12) = 28 (~5.6 dB).
    // At sens=95 (rssi_sens=20): guard = max(15, 40-20) = 20 (~4 dB).
    const uint8_t effective_flatness_min = (rssi_sens > 0)
        ? static_cast<uint8_t>((FLATNESS_MIN_PEAK_MARGIN > static_cast<uint8_t>(rssi_sens))
            ? (FLATNESS_MIN_PEAK_MARGIN - rssi_sens)
            : 15)
        : FLATNESS_MIN_PEAK_MARGIN;
    // Sensitive mode skips flatness for WEAK signals (#1 cause of missed weak
    // FPV) but KEEPS it for very_strong peaks: with max_width/valley/symmetry
    // all bypassed at very_strong, disabling flatness too would let a
    // close-range WiFi/BT flat-top pass unfiltered (sharpness ≈ 100 passes
    // the default threshold).
    if (config_.spectrum_flatness > 0 && peak_margin >= effective_flatness_min
        && (!config_.sensitive_mode || very_strong)) {
        // Denominator: signal width excluding DC spike bins (if present)
        size_t effective_width = right - left + 1;
        if (has_dc_gap && left < FFT_DC_SPIKE_END && right >= FFT_DC_SPIKE_START) {
            const size_t dc_start = (left > FFT_DC_SPIKE_START) ? left : FFT_DC_SPIKE_START;
            const size_t dc_end = (right < FFT_DC_SPIKE_END) ? right : (FFT_DC_SPIKE_END - 1);
            effective_width -= (dc_end - dc_start + 1);
        }

        // Narrowband guard: flatness is meaningless for very narrow signals.
        // A 2-4 bin burst (ELRS/FrSky in sweep mode) is inherently "flat"
        // (1-2 high-power bins out of 2-4 = 50-100%) and was previously
        // rejected whenever peak_margin >= FLATNESS_MIN_PEAK_MARGIN — making
        // spectrum_min_width=2 unreachable at moderate+ SNR. WiFi/BT
        // flat-tops span dozens of bins and are unaffected by this guard.
        if (effective_width > FLATNESS_MIN_SIGNAL_WIDTH) {
            const uint8_t high_power_threshold = raw_peak * 9 / 10;
            size_t high_power_count = 0;

            for (size_t i = peak_idx; i > left && i > edge_skip; --i) {
                if (has_dc_gap && i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
                if (data[i] >= high_power_threshold) ++high_power_count;
                else break;
            }
            for (size_t i = peak_idx + 1; i <= right && i < upper_limit; ++i) {
                if (has_dc_gap && i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
                if (data[i] >= high_power_threshold) ++high_power_count;
                else break;
            }

            if (effective_width > 0) {
                const uint8_t flatness_pct = static_cast<uint8_t>((high_power_count * 100) / effective_width);
                if (flatness_pct > config_.spectrum_flatness) return false;
            }
        }
    }

    // Step 11: Symmetry (V-shape must have similar left/right width)
    // Signal is real regardless of asymmetry at strong levels.
    // Sensitive mode: skip symmetry — asymmetric shapes are common in weak/multipath signals.
    if (config_.spectrum_symmetry > 0 && signal_width > 1 && !very_strong && !config_.sensitive_mode) {
        const size_t left_width = peak_idx - left;
        const size_t right_width = right - peak_idx;
        const size_t max_side = (left_width > right_width) ? left_width : right_width;
        const size_t min_side = (left_width < right_width) ? left_width : right_width;
        if (max_side > 0) {
            const uint8_t sym_pct = static_cast<uint8_t>((min_side * 100) / max_side);
            if (sym_pct < config_.spectrum_symmetry) return false;
        }
    }

    // Step 12: Spectral Kurtosis (higher-order statistics)
    // Distinguishes Gaussian noise (kurtosis ≈ 0) from non-Gaussian drone signals
    // (kurtosis > 3, leptokurtic). WiFi flat-top has kurtosis < 0 (platykurtic).
    // Only runs when explicitly enabled (opt-in, default OFF).
    // Very strong bypass: kurtosis is unreliable when signal fills >50% of bins.
    // Sensitive mode: skip kurtosis — unreliable for weak signals with low SNR.
    if (config_.kurtosis_enabled && peak_margin >= FLATNESS_MIN_PEAK_MARGIN && !very_strong && !config_.sensitive_mode) {
        const size_t data_size = has_dc_gap ? FFT_BIN_COUNT : COMPOSITE_SIZE;
        const auto kurt_result = SpectralKurtosis::compute(
            data, edge_skip, data_size - edge_skip,
            spectrum_sort_buf_, SPECTRUM_SORT_BUF_SIZE,
            has_dc_gap ? FFT_DC_SPIKE_START : 0,
            has_dc_gap ? FFT_DC_SPIKE_END : 0
        );
        if (kurt_result.kurtosis_x10 < config_.kurtosis_min_x10) {
            return false;
        }
    }

    out_rssi = spectrum_value_to_dbm(raw_peak, total_gain);
    return true;
}

// ============================================================================
// tbd_peak_is_narrowband — MaxW guard for Track-Before-Detect (TBD)
// ============================================================================

bool DroneScanner::tbd_peak_is_narrowband(
    const uint8_t* data,
    size_t data_size,
    size_t peak_idx,
    uint8_t noise_floor,
    size_t edge_skip,
    bool has_dc_gap
) const noexcept {
    // TBD confirms signals purely by multi-frame power integration and NEVER
    // runs apply_shape_filters(). Consequently a persistent WiFi/BT flat-top
    // that MaxW rejected in the single-frame chain was re-confirmed after
    // TBD_MIN_FRAMES frames and tracked as a drone — the MaxW setting was
    // silently ignored (bypass OFF or ON made no difference). This guard
    // re-applies the exact MaxW width semantics of apply_shape_filters()
    // Steps 4+6 around the TBD-confirmed bin on the CURRENT frame: if the
    // contiguous elevated width exceeds spectrum_max_width, the target is
    // wideband and must stay rejected.
    // Stack: 16 bytes (safe on 4KB task stack). Flash: ~64 bytes.
    if (peak_idx >= data_size || edge_skip >= data_size / 2) return false;
    if (peak_idx < edge_skip || peak_idx >= data_size - edge_skip) return false;

    const uint8_t raw_peak = data[peak_idx];
    if (raw_peak <= noise_floor) return false;

    // Same elevated threshold as apply_shape_filters Step 4 (non-bypass path).
    // peak_margin/3 in uint8 arithmetic: raw_peak > noise_floor guarantees a
    // positive margin, and raw_peak <= 255 caps the sum well below 256+.
    const uint8_t peak_margin = raw_peak - noise_floor;
    const uint8_t elevated_threshold = noise_floor + static_cast<uint8_t>(peak_margin / 3);

    const size_t upper_limit = data_size - edge_skip;

    size_t left = peak_idx;
    while (left > edge_skip) {
        const size_t prev = left - 1;
        if (has_dc_gap && prev >= FFT_DC_SPIKE_START && prev < FFT_DC_SPIKE_END) break;
        if (data[prev] < elevated_threshold) break;
        --left;
    }

    size_t right = peak_idx;
    while (right < upper_limit - 1) {
        const size_t next = right + 1;
        if (has_dc_gap && next >= FFT_DC_SPIKE_START && next < FFT_DC_SPIKE_END) break;
        if (data[next] < elevated_threshold) break;
        ++right;
    }

    const size_t signal_width = right - left + 1;
    return signal_width <= config_.spectrum_max_width;
}

// ============================================================================
// apply_sweep_tracking — range check, exception filter, Mahalanobis gate,
//                        and drone tracking
// ============================================================================

void DroneScanner::apply_sweep_tracking(
    FreqHz peak_freq,
    int32_t peak_rssi,
    FreqHz center_freq,
    FreqHz f_min,
    FreqHz f_max
) noexcept {
    const int32_t cfg_rssi_thresh = config_.rssi_threshold_dbm;
    if (peak_rssi <= cfg_rssi_thresh) return;

    // Range check — prevent false positives outside sweep boundaries
    const FreqHz range_min = (f_min != 0) ? f_min : config_.sweep_start_freq;
    const FreqHz range_max = (f_max != 0) ? f_max : config_.sweep_end_freq;
    if (range_min != 0 && range_max != 0) {
        if (peak_freq < range_min || peak_freq > range_max) return;
    }

    // Exception frequency filter
    if (is_exception_frequency(peak_freq)) return;

    // Mahalanobis gate
    bool mahalanobis_rejected = false;
    bool drone_created_here = false;
    if (config_.mahalanobis_enabled) {
        size_t drone_idx = 0;
        bool drone_found = false;
        // Nearest-match lookup (same policy as update_tracked_drone_internal):
        // bin-quantized peak frequencies drift by 1-3 bins between sweep
        // cycles, so exact equality never accumulated per-drone statistics.
        const ErrorResult<size_t> match = find_nearest_drone_internal(
            peak_freq, static_cast<FreqHz>(config_.freq_match_radius_mhz) * 1'000'000ULL);
        if (match.has_value()) {
            drone_idx = match.value();
            drone_found = true;
        }

        if (drone_found) {
            MahalanobisStatistics& stats = tracked_drones_[drone_idx].get_mahalanobis_stats();
            if (!mahalanobis_detector_.validate(
                peak_rssi, center_freq, stats, config_.mahalanobis_threshold_x10
            )) {
                mahalanobis_rejected = true;
            } else {
                mahalanobis_detector_.update_statistics(stats, peak_rssi, center_freq, peak_freq);
            }
        } else if (tracked_count_ < MAX_TRACKED_DRONES) {
            const size_t new_idx = tracked_count_;
            tracked_drones_[new_idx] = TrackedDrone(peak_freq, DroneType::UNKNOWN, ThreatLevel::NONE);
            auto& drone = tracked_drones_[new_idx];
            drone.created_time_ = chTimeNow();
            drone.last_increase_time_ = chTimeNow();
            // First detection: always mark as increasing to prevent immediate decay
            drone.rssi_increased_ = true;
            drone.sweep_cycles_missed_ = 0;
            drone.last_seen_time_ = chTimeNow();
            // Set sweep mode BEFORE update_rssi() so threat classification
            // uses the direct RSSI (not contaminated rssi_history_).
            drone.sweep_mode_active_ = true;
            drone.update_rssi(peak_rssi, chTimeNow(), ThreatThresholds{
                config_.threat_low_dbm, config_.threat_medium_dbm,
                config_.threat_high_dbm, config_.threat_critical_dbm});
            drone.update_cycle_peak(peak_rssi);
            drone.get_mahalanobis_stats().last_tuned_frequency = peak_freq;
            tracked_count_++;
            statistics_.drones_detected++;
            if (drone.threat_level > ThreatLevel::NONE) {
                trigger_alert(drone.threat_level);
            }
            drone_created_here = true;
        }
    }

    // Skip update_tracked_drone_internal when drone was just created above
    // to avoid double update_rssi() + double trigger_alert()
    if (!mahalanobis_rejected && !drone_created_here) {
        size_t drone_idx = SIZE_MAX;
        (void)update_tracked_drone_internal(peak_freq, peak_rssi, chTimeNow(), &drone_idx);

        // Use returned index — avoids redundant find_nearest_drone_internal() O(n) search
        if (drone_idx < tracked_count_) {
            tracked_drones_[drone_idx].update_cycle_peak(peak_rssi);
            tracked_drones_[drone_idx].mark_seen(chTimeNow(), peak_rssi);
        }
    }
}

void DroneScanner::process_spectrum_sweep(
    const ChannelSpectrum& spectrum,
    const uint8_t* lg_buffer,
    FreqHz center_freq,
    FreqHz f_min,
    FreqHz f_max
) noexcept {
    current_frequency_ = center_freq;

    // Per-frequency median filter reset: only reset when the tuned frequency changes.
    // In sweep mode, each FFT frame is a different frequency, so resetting every frame
    // makes the filter useless (never reaches warm state of 7 samples). By tracking
    // the last frequency, the filter accumulates across sweep CYCLES for the same freq,
    // providing meaningful RSSI smoothing after ~7 passes (~11 seconds at 1.6s/pass).
    if (center_freq != last_sweep_freq_) {
        rssi_median_filter_.reset();
        // Reset temporal history on frequency change. Previously the waterfall
        // was NOT reset in sweep mode, so TBD "confirmed" targets by
        // integrating the last 8 frames (~8 slices ≈ 70 MHz of DIFFERENT RF
        // frequencies) — statistically meaningless and a source of phantom
        // detections on the current slice's frequency. TBD now integrates
        // only revisit frames of the SAME slice (requires 3 sweep cycles).
        // NOTE: per-frequency RSSI smoothing (median, window 7, warm at 4)
        // likewise needs several revisit cycles per frequency, so both
        // median_enabled and sweep TBD are slow integrators on wide windows.
        waterfall_history_.reset();
        last_sweep_freq_ = center_freq;
    }

    // Hysteresis reset: only when frequency changes (matches normal mode pattern at :730)
    // Preserves signal_present_ state when revisiting the same frequency in sweep cycles
    if (center_freq != last_hysteresis_freq_) {
        signal_present_ = false;
        last_hysteresis_freq_ = center_freq;
    }

    // Feed waterfall history for sweep mode (runs without mutex — scanner thread stopped)
    waterfall_history_.push(spectrum.db.data());

    // Sensitive mode: reduce the spectrum margin by 2 (min 1) for maximum
    // weak-signal detection — single source of truth via
    // effective_spectrum_margin(); apply_shape_filters Step 3 applies the
    // identical reduction in BOTH scan modes, so the relaxation is no longer
    // nullified downstream.
    // Default margin=5 (~1 dB above noise); sensitive: margin=3 (~0.6 dB).
    const uint8_t cfg_margin = effective_spectrum_margin();

    // Step 1: Compute noise floor (25th percentile of usable bins).
    // Shared for all peaks in this frame — computed once.
    uint8_t* usable = sweep_usable_buf_;
    size_t idx = 0;
    for (size_t i = FFT_EDGE_SKIP_NARROW; i < FFT_DC_SPIKE_START; ++i) {
        usable[idx++] = spectrum.db[i];
    }
    for (size_t i = FFT_DC_SPIKE_END; i < (FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW); ++i) {
        usable[idx++] = spectrum.db[i];
    }
    if (idx == 0) return;
    const uint8_t noise_floor = quickselect_percentile(usable, idx, 25);

    // Step 2: Find ALL candidate peaks (CFAR multi-peak or fixed-threshold).
    // Stack: CFARPeak × 8 = 16 bytes. Well within 512B limit.
    static constexpr size_t MAX_SWEEP_PEAKS = CFAR_MAX_CONCURRENT_PEAKS;
    CFARPeak cfar_peaks[MAX_SWEEP_PEAKS]{};
    size_t peak_count = 0;

    if (config_.cfar_mode != CFARMode::OFF) {
        // Multi-peak CFAR: finds ALL peaks passing the adaptive threshold.
        // Critical for detecting dual-peak FPV (video + audio subcarrier)
        // or multiple drones in the same 20 MHz window.
        // Sensitive mode: reduce CFAR threshold by 1.0 unit (~0.2 dB) for max sensitivity.
        const uint8_t effective_cfar_threshold = config_.sensitive_mode
            ? ((config_.cfar_threshold_x10 > CFAR_THRESHOLD_MIN_X10 + 10)
                ? (config_.cfar_threshold_x10 - 10) : CFAR_THRESHOLD_MIN_X10)
            : config_.cfar_threshold_x10;
        peak_count = CFARDetector::find_peaks(
            spectrum.db.data(),
            FFT_BIN_COUNT,
            cfar_peaks, MAX_SWEEP_PEAKS,
            config_.cfar_mode,
            config_.cfar_ref_cells,
            config_.cfar_guard_cells,
            effective_cfar_threshold,
            FFT_EDGE_SKIP_NARROW,
            FFT_EDGE_SKIP_NARROW,
            config_.cfar_hybrid_alpha,
            config_.cfar_hybrid_beta,
            config_.cfar_hybrid_gamma,
            config_.os_cfar_k_percent,
            config_.vi_cfar_threshold_x10
        );
    } else {
        // Fixed-threshold: collect all bins above noise_floor + margin,
        // sort by power descending, take top N.
        struct SimplePeak { size_t bin; uint8_t power; };
        SimplePeak candidates[MAX_SWEEP_PEAKS * 2]{};
        size_t cand_count = 0;

        for (size_t i = FFT_EDGE_SKIP_NARROW; i < FFT_DC_SPIKE_START; ++i) {
            const int32_t bin_val = spectrum.db[i];
            const int32_t margin = bin_val - static_cast<int32_t>(noise_floor);
            if (margin >= static_cast<int32_t>(cfg_margin) && cand_count < MAX_SWEEP_PEAKS * 2) {
                candidates[cand_count++] = {i, spectrum.db[i]};
            }
        }
        for (size_t i = FFT_DC_SPIKE_END; i < (FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW); ++i) {
            const int32_t bin_val = spectrum.db[i];
            const int32_t margin = bin_val - static_cast<int32_t>(noise_floor);
            if (margin >= static_cast<int32_t>(cfg_margin) && cand_count < MAX_SWEEP_PEAKS * 2) {
                candidates[cand_count++] = {i, spectrum.db[i]};
            }
        }

        // Sort by power descending (insertion sort — small array)
        for (size_t i = 1; i < cand_count; ++i) {
            const SimplePeak key = candidates[i];
            size_t j = i;
            while (j > 0 && candidates[j - 1].power < key.power) {
                candidates[j] = candidates[j - 1];
                --j;
            }
            candidates[j] = key;
        }

        // Non-maximum suppression: reject peaks within CFAR_MIN_PEAK_SEPARATION
        peak_count = 0;
        for (size_t i = 0; i < cand_count && peak_count < MAX_SWEEP_PEAKS; ++i) {
            bool suppressed = false;
            for (size_t j = 0; j < peak_count; ++j) {
                const size_t bin_diff = (candidates[i].bin > cfar_peaks[j].bin)
                    ? (candidates[i].bin - cfar_peaks[j].bin)
                    : (cfar_peaks[j].bin - candidates[i].bin);
                if (bin_diff < CFAR_MIN_PEAK_SEPARATION) {
                    suppressed = true;
                    break;
                }
            }
            if (!suppressed) {
                cfar_peaks[peak_count++] = {candidates[i].bin, candidates[i].power};
            }
        }
    }

    // Update peak power for timeline display (strongest peak)
    // Do NOT early-return on peak_count == 0: multi-frame TBD below must still
    // run when single-frame detection found no peaks (documented at :2256).
    if (peak_count > 0) {
        last_peak_power_ = cfar_peaks[0].power;
    }

    const int32_t total_gain = get_current_total_gain();

    // Tracks whether any peak survived shape analysis this frame. TBD below must
    // be gated on this: once single-frame detection tracked a signal, the
    // multi-frame integration must NOT run (documented at :2256).
    bool any_peak_passed_shape = false;

    // Step 3: Process each CFAR peak through shape analysis + tracking.
    // Only the strongest peak feeds the median filter (prevents cross-frequency
    // contamination of the per-frequency smoothing accumulator).
    for (size_t p = 0; p < peak_count; ++p) {
        const size_t peak_index = cfar_peaks[p].bin;

        // Skip peaks on DC spike (bins 120-135) — no real signal energy
        const size_t peak_pixel = fft_bin_to_lg_pixel(peak_index);
        if (peak_pixel >= COMPOSITE_SIZE) continue;

        FreqHz peak_freq = fft_bin_to_freq(center_freq, peak_index);

        // Shape analysis on LG-reordered buffer (continuous, no DC gap).
        int32_t shape_rssi = RSSI_MIN_DBM;
        if (!analyze_spectrum_shape_lg(lg_buffer, peak_pixel, noise_floor, shape_rssi, total_gain)) {
            continue;  // This peak rejected by shape filter — try next peak
        }
        any_peak_passed_shape = true;

        // Median filter: only for primary peak (p==0) to avoid cross-frequency contamination.
        int32_t peak_rssi = shape_rssi;
        if (p == 0) {
            rssi_median_filter_.add(peak_rssi);
            if (median_filter_enabled_ && rssi_median_filter_.is_warm()) {
                peak_rssi = rssi_median_filter_.get_median();
            }
        }

        apply_sweep_tracking(
            peak_freq,
            peak_rssi, center_freq, f_min, f_max
        );
    }

    // ---- Sweep-mode Track-Before-Detect (TBD) ----
    // When single-frame detection fails for all peaks, check if multi-frame
    // integration confirms a weak signal. Uses waterfall_history_ to accumulate
    // power across recent frames at the same tuned frequency.
    // Provides ~9 dB SNR gain (8 frames = 3× sensitivity improvement).
    // Only runs when waterfall_history_ has enough frames and single-frame
    // detection found nothing (peak_count == 0 or all peaks rejected by shape).
    if (!any_peak_passed_shape &&
        waterfall_history_.is_warm(TBD_MIN_FRAMES) && config_.spectrum_detection_enabled) {
        // Find the peak bin from multi-frame integration across usable bins
        size_t tbd_peak_bin = FFT_EDGE_SKIP_NARROW;
        uint8_t tbd_peak_power = 0;
        for (size_t i = FFT_EDGE_SKIP_NARROW; i < FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW; ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            const uint8_t max_power = waterfall_history_.get_max_across_frames(i);
            if (max_power > tbd_peak_power) {
                tbd_peak_power = max_power;
                tbd_peak_bin = i;
            }
        }

        // Check if this bin was active in enough recent frames
        const int32_t total_gain_tbd = get_current_total_gain();
        const int32_t tbd_raw_threshold = (config_.rssi_threshold_dbm + total_gain_tbd) * 5 + 255;
        const uint8_t threshold = static_cast<uint8_t>(
            (tbd_raw_threshold < 0) ? 0 : (tbd_raw_threshold > 255 ? 255 : tbd_raw_threshold));
        const uint8_t active_frames = waterfall_history_.count_above_threshold(tbd_peak_bin, threshold);

        if (active_frames >= TBD_MIN_FRAMES) {
            // Multi-frame confirmed — compute integrated RSSI
            const uint16_t integrated = waterfall_history_.get_integrated_power(tbd_peak_bin);
            const uint8_t avg_power = static_cast<uint8_t>(integrated / waterfall_history_.size());
            const int32_t tbd_rssi = spectrum_value_to_dbm(avg_power, total_gain_tbd);

            // NARROWBAND GUARD (MaxW): mirror of the normal-mode TBD guard.
            // TBD must never resurrect a wideband flat-top (WiFi/BT) that the
            // single-frame shape chain (MaxW step of apply_shape_filters)
            // already rejected — enforce MaxW on the LG buffer too.
            const size_t tbd_peak_pixel = fft_bin_to_lg_pixel(tbd_peak_bin);
            if (tbd_rssi > config_.rssi_threshold_dbm &&
                tbd_peak_pixel < COMPOSITE_SIZE &&
                tbd_peak_is_narrowband(
                    lg_buffer, COMPOSITE_SIZE, tbd_peak_pixel, noise_floor,
                    LG_EDGE_SKIP_PX, /*has_dc_gap=*/false)) {
                const FreqHz tbd_freq = fft_bin_to_freq(center_freq, tbd_peak_bin);
                apply_sweep_tracking(
                    tbd_freq, tbd_rssi, center_freq, f_min, f_max
                );
            }
        }
    }
}

} // namespace drone_analyzer
