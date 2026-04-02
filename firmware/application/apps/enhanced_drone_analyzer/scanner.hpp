#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "ch.h"
#include "drone_types.hpp"
#include "locking.hpp"
#include "constants.hpp"
#include "database.hpp"
#include "hardware_controller.hpp"
#include "audio_alerts.hpp"
#include "rssi_detector.hpp"
#include "histogram_processor.hpp"
#include "median_filter.hpp"
#include "message.hpp"

namespace drone_analyzer {

/**
 * @brief Scan configuration
 * @note ScannerState is defined in drone_types.hpp
 */
struct ScanConfig {
    ScanningMode mode;
    FreqHz start_frequency;
    FreqHz end_frequency;
    uint32_t scan_interval_ms;
    int32_t rssi_threshold_dbm;
    uint32_t stale_timeout_ms;
    
    // Sweep range (Hz) — window 1
    FreqHz sweep_start_freq;
    FreqHz sweep_end_freq;
    FreqHz sweep_step_freq;

    // Sweep range (Hz) — window 2 (independent, optional)
    FreqHz sweep2_start_freq{2400000000ULL};
    FreqHz sweep2_end_freq{2500000000ULL};
    FreqHz sweep2_step_freq{20000000};
    bool sweep2_enabled{false};

    // Sweep range (Hz) — window 3 (independent, optional, disabled by default)
    FreqHz sweep3_start_freq{900000000ULL};
    FreqHz sweep3_end_freq{1000000000ULL};
    FreqHz sweep3_step_freq{20000000};
    bool sweep3_enabled{false};

    // Sweep range (Hz) — window 4 (independent, optional, disabled by default)
    FreqHz sweep4_start_freq{1200000000ULL};
    FreqHz sweep4_end_freq{1300000000ULL};
    FreqHz sweep4_step_freq{20000000};
    bool sweep4_enabled{false};

    // Advanced detection features (OFF by default)
    bool dwell_enabled{false};           // Stay on frequency when signal detected
    bool confirm_count_enabled{false};   // Require multiple confirmations before creating drone
    bool noise_blacklist_enabled{false}; // Skip frequencies with persistent noise
    bool spectrum_detection_enabled{false}; // Detect drone signals by spectrum shape (U/V peaks)
    bool median_enabled{false};             // Median filter for RSSI spike rejection
    uint8_t spectrum_margin{15};            // Peak margin above noise (0-200, default 15 ≈ 5 dB)
    uint8_t spectrum_min_width{1};          // Min signal width in bins (1-20, default 1)
    uint8_t spectrum_max_width{DEFAULT_SPECTRUM_MAX_WIDTH};            // Max signal width (reject flat U/I shapes)
    uint8_t spectrum_peak_sharpness{DEFAULT_SPECTRUM_PEAK_SHARPNESS};  // Min peak sharpness ratio (enforce V-shape)
    uint8_t spectrum_peak_ratio{DEFAULT_SPECTRUM_PEAK_RATIO};          // Peak-to-width ratio (inverted-V filter)
    uint8_t spectrum_valley_depth{DEFAULT_SPECTRUM_VALLEY_DEPTH};      // Valley depth threshold (V-shape flanks)

    // New anti-false-positive features
    int32_t neighbor_margin_db{DEFAULT_NEIGHBOR_MARGIN_DB};  // 0=disabled, 3=default
    bool rssi_variance_enabled{false};                        // RSSI variance noise rejection
    uint8_t confirm_count{DEFAULT_CONFIRM_COUNT};             // Configurable confirm count

    // Sweep exception frequencies (per window, 0 = unused slot)
    FreqHz sweep_exceptions[4][EXCEPTIONS_PER_WINDOW]{};
    uint8_t exception_radius_mhz{DEFAULT_EXCEPTION_RADIUS_MHZ};  // 1-100, configurable exclusion radius
    uint8_t rssi_decrease_cycles{5};  // sweep cycles of RSSI decrease before threat decay

    /**
     * @brief Default constructor
     */
    ScanConfig() noexcept;
    
    /**
     * @brief Constructor with values
     */
    ScanConfig(ScanningMode m, FreqHz start, FreqHz end) noexcept;
};

/**
 * @brief Scan statistics
 */
struct ScanStatistics {
    uint32_t total_scan_cycles;
    uint32_t successful_cycles;
    uint32_t failed_cycles;
    uint32_t drones_detected;
    uint32_t max_rssi_dbm;
    
    /**
     * @brief Default constructor
     */
    ScanStatistics() noexcept;
    
    /**
     * @brief Reset statistics
     */
    void reset() noexcept;
};

// ============================================================================
// Multi-Zone Sweep Structures
// ============================================================================

/**
 * @brief Number of sweep zones (4 horizontal display bands)
 */
constexpr uint8_t SWEEP_ZONE_COUNT = 4;

/**
 * @brief Per-zone sweep configuration
 */
struct SweepZoneConfig {
    FreqHz start_freq{0};
    FreqHz end_freq{0};
    bool enabled{false};
};

/**
 * @brief Multi-zone sweep configuration (stored in SD card settings)
 */
struct SweepZonesConfig {
    SweepZoneConfig zones[SWEEP_ZONE_COUNT];

    SweepZonesConfig() noexcept {
        // Default: zone 0 = 2.4 GHz ISM, others disabled
        zones[0] = {2400000000ULL, 2500000000ULL, true};
        zones[1] = {5700000000ULL, 5800000000ULL, false};
        zones[2] = {5800000000ULL, 5900000000ULL, false};
        zones[3] = {1000000000ULL, 1100000000ULL, false};
    }
};

/**
 * @brief Per-zone sweep runtime state
 */
struct SweepZoneRuntime {
    FreqHz current_center{0};
    FreqHz pixel_step_hz{0};
    FreqHz step_hz{0};
    FreqHz bins_hz_acc{0};
    FreqHz center_ini{0};
    uint16_t pixel_index{0};
    uint8_t pixel_max{0};

    void init(const SweepZoneConfig& cfg) noexcept {
        constexpr FreqHz SLICE_BW = 20000000;
        constexpr FreqHz BIN_SIZE = SLICE_BW / FFT_BIN_COUNT;

        const FreqHz range = cfg.end_freq - cfg.start_freq;
        pixel_step_hz = (range > 0) ? range / SWEEP_PIXELS_PER_SLICE : 0;
        step_hz = SWEEP_BINS_PER_STEP * BIN_SIZE;
        center_ini = cfg.start_freq - (2 * BIN_SIZE) + (SLICE_BW / 2);
        current_center = center_ini;
        pixel_index = 0;
        pixel_max = 0;
        bins_hz_acc = 0;
    }

    void reset_pass() noexcept {
        current_center = center_ini;
        pixel_index = 0;
        pixel_max = 0;
        bins_hz_acc = 0;
    }

    [[nodiscard]] bool is_complete() const noexcept {
        return pixel_index >= SWEEP_PIXELS_PER_SLICE;
    }
};

/**
 * @brief Neighbor frequency margin checker (anti-false-positive)
 * @note Stores last N frequency/RSSI pairs in circular buffer
 * @note When signal detected, checks if current freq is stronger than neighbors
 * @note Eliminates wideband noise false positives (WiFi, BT, microwave)
 * @note Inspired by FPV detect app's MIN_NEIGHBOR_MARGIN_FOR_LOCK_DB
 */
class NeighborMarginChecker {
public:
    static constexpr size_t WINDOW = 5;

    /**
     * @brief Add frequency/RSSI sample
     * @param freq Tuned frequency
     * @param rssi RSSI in dBm
     */
    void add(FreqHz freq, int32_t rssi) noexcept {
        history_[head_] = {freq, rssi};
        head_ = (head_ + 1) % WINDOW;
        if (count_ < WINDOW) count_++;
    }

    /**
     * @brief Check if current frequency is stronger than neighbors
     * @param current_freq Current tuned frequency
     * @param current_rssi Current RSSI in dBm
     * @param min_margin_db Minimum dB margin over strongest neighbor
     * @return true if current freq dominates neighbors, false if wideband noise
     */
    [[nodiscard]] bool check_margin(FreqHz current_freq, int32_t current_rssi, int32_t min_margin_db) const noexcept {
        if (count_ < 2) return true;  // Not enough data — pass through
        int32_t best_neighbor_rssi = -120;
        for (uint8_t i = 0; i < count_; ++i) {
            if (history_[i].freq != current_freq && history_[i].rssi > best_neighbor_rssi) {
                best_neighbor_rssi = history_[i].rssi;
            }
        }
        return (current_rssi - best_neighbor_rssi) >= min_margin_db;
    }

    /**
     * @brief Reset checker state
     */
    void reset() noexcept {
        head_ = 0;
        count_ = 0;
    }

private:
    struct Entry { FreqHz freq; int32_t rssi; };
    Entry history_[WINDOW]{};
    uint8_t head_{0};
    uint8_t count_{0};
};

/**
 * @brief Alert callback function type
 * @param threat_level Threat level based on RSSI
 * @note Audio tone varies by threat: CRITICAL=1500Hz, HIGH=1200Hz, MEDIUM=1000Hz
 */
using ThreatAlertCallback = void(*)(ThreatLevel threat_level);

/**
 * @brief Drone scanner
 * @note Main scanning logic for drone detection
 * @note Simplified: removed wideband/hybrid/panoramic modes, removed FHSS detection
 * @note Thread-safe with mutex protection
 */
class DroneScanner {
public:
    /**
     * @brief Constructor
     * @param database Reference to database manager
     * @param hardware Reference to hardware controller
     */
    DroneScanner(DatabaseManager& database, HardwareController& hardware) noexcept;
    
    /**
     * @brief Destructor
     */
    ~DroneScanner() noexcept;
    
    // Delete copy and move operations
    DroneScanner(const DroneScanner&) = delete;
    DroneScanner& operator=(const DroneScanner&) = delete;
    DroneScanner(DroneScanner&&) = delete;
    DroneScanner& operator=(DroneScanner&&) = delete;
    
    /**
     * @brief Initialize scanner
     * @return ErrorCode::SUCCESS if initialized, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode initialize() noexcept;
    
    /**
     * @brief Start scanning
     * @return ErrorCode::SUCCESS if started, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode start_scanning() noexcept;
    
    /**
     * @brief Stop scanning
     * @return ErrorCode::SUCCESS if stopped, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode stop_scanning() noexcept;
    
    /**
     * @brief Pause scanning
     * @return ErrorCode::SUCCESS if paused, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode pause_scanning() noexcept;
    
    /**
     * @brief Resume scanning from paused state
     * @return ErrorCode::SUCCESS if resumed, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode resume_scanning() noexcept;

    /**
     * @brief Force resume scanning from LOCKING/TRACKING state
     * @note Called by scanner thread when max dwell time expires
     * @note Uses AtomicFlag trick to avoid mutex deadlock
     *       (scanner thread already can't hold the mutex)
     */
    void force_resume_scanning() noexcept;

    /**
     * @brief Consume force-resume flag and transition to SCANNING
     * @return true if flag was set and state transitioned, false otherwise
     * @note Thread-safe: uses AtomicFlag test_and_clear + mutex for state
     * @note Called by scanner thread BEFORE dwell logic to break out of LOCKING
     */
    bool try_consume_force_resume_flag() noexcept;
    
    /**
     * @brief Remove tracked drone on a specific frequency (no mutex)
     * @note Called by scanner thread after force-resume
     */
    void remove_drone_on_frequency(FreqHz frequency) noexcept;

    /**
     * @brief Increment noise count for a frequency (blacklist tracking)
     * @note Called by scanner thread when force-resuming from noise
     */
    void increment_noise_count(FreqHz frequency) noexcept;

    /**
     * @brief Reset noise count for a frequency (real signal confirmed)
     */
    void reset_noise_count(FreqHz frequency) noexcept;

    /**
     * @brief Check if frequency is blacklisted (persistent noise)
     */
    bool is_blacklisted(FreqHz frequency) const noexcept;
    
    /**
     * @brief Perform single scan cycle (frequency hop)
     * @note Called periodically by scanner thread
     * @return ErrorCode::SUCCESS if cycle completed, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @note ChibiOS mutexes are recursive per-thread: nested calls from the
     *       same thread (perform_scan_cycle → perform_scan_cycle_internal)
     *       succeed without deadlock.
     * @note This method only advances the frequency; RSSI detection is done
     *       by the UI thread via process_spectrum_message().
     */
    [[nodiscard]] ErrorCode perform_scan_cycle() noexcept;
    
    /**
     * @brief Update tracked drones with new data
     * @param frequency Frequency of detected signal
     * @param rssi RSSI value
     * @param timestamp Timestamp of detection
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode update_tracked_drones(
        FreqHz frequency,
        RssiValue rssi,
        SystemTime timestamp
    ) noexcept;

    /**
     * @brief Process spectrum data and extract RSSI
     * @param spectrum Channel spectrum data
     * @param current_frequency Current tuned frequency (for tracking)
     * @return ErrorResult containing RSSI value or error
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @note Extracts maximum power from spectrum and converts to dBm
     * @note Updates tracked drones if RSSI above threshold
     */
    [[nodiscard]] ErrorResult<RssiValue> process_spectrum_data(
        const ChannelSpectrum& spectrum,
        FreqHz current_frequency
    ) noexcept;
    
    /**
     * @brief Process ChannelSpectrum message directly
     * @param spectrum Channel spectrum data
     * @return ErrorCode::SUCCESS if processed, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @note Uses scanner's internal current_frequency
     * @note Updates tracked drones if RSSI above threshold
     */
    [[nodiscard]] ErrorCode process_spectrum_message(const ChannelSpectrum& spectrum) noexcept;

    /**
     * @brief Process spectrum with explicit frequency (avoids race with scanner thread)
     * @param spectrum Channel spectrum data
     * @param frequency Frequency this spectrum corresponds to
     * @return ErrorCode::SUCCESS if processed, error code otherwise
     */
    [[nodiscard]] ErrorCode process_spectrum_message(const ChannelSpectrum& spectrum, FreqHz frequency) noexcept;

    /**
     * @brief Get current frequency for spectrum association (thread-safe)
     * @return Current frequency under mutex lock
     */
    [[nodiscard]] FreqHz get_spectrum_frequency() noexcept;

    [[nodiscard]] size_t get_tracked_drones(
        TrackedDrone* drones,
        size_t max_count
    ) const noexcept;
    
    /**
     * @brief Get scanner state
     * @return Current scanner state
     */
    [[nodiscard]] ScannerState get_state() const noexcept;
    
    /**
     * @brief Check if scanning is active
     * @return true if scanning, false otherwise
     */
    [[nodiscard]] bool is_scanning() const noexcept;
    
    /**
     * @brief Get scan configuration
     * @return Current scan configuration
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ScanConfig get_config() const noexcept;
    
    /**
     * @brief Set scan configuration
     * @param config Configuration to apply
     * @return ErrorCode::SUCCESS if set, error code otherwise
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode set_config(const ScanConfig& config) noexcept;
    
    /**
     * @brief Get scan statistics
     * @return Current scan statistics
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ScanStatistics get_statistics() const noexcept;
    
    /**
     * @brief Reset scan statistics
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void reset_statistics() noexcept;
    
    /**
     * @brief Get current scan frequency
     * @return ErrorResult containing current frequency or error
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorResult<FreqHz> get_current_frequency() const noexcept;

    /**
     * @brief Set scan frequency directly (for continue after sweep)
     * @param frequency Frequency to set
     * @note After sweep exit, continue scanning from this frequency
     */
    void set_scan_frequency(FreqHz frequency) noexcept;

    /**
     * @brief Clear lock state (LOCKING/TRACKING → SCANNING, reset lock counters)
     * @note Called before entering sweep mode to prevent stale lock after resume
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void clear_lock_state() noexcept;
    
    /**
     * @brief Get number of tracked drones
     * @return Number of tracked drones
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] size_t get_tracked_count() const noexcept;

    // ========================================================================
    // Fast Scanner Integration Methods
    // ========================================================================

    /**
     * @brief Get frequency lock count
     * @return Current lock count
     * @note Thread-safe: acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] uint32_t get_freq_lock_count() const noexcept;

    /**
     * @brief Set frequency lock count
     * @param count New lock count
     * @note Thread-safe: acquires mutex (LockOrder::DATA_MUTEX)
     */
    void set_freq_lock_count(uint32_t count) noexcept;

    /**
     * @brief Get locked frequency
     * @return Currently locked frequency (0 if not locked)
     * @note Thread-safe: acquires mutex (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] FreqHz get_locked_frequency() const noexcept;

    /**
     * @brief Get current drone type string
     * @param buffer Destination buffer for drone type string
     * @param buffer_size Size of destination buffer (must be >= 2)
     * @return ErrorCode::SUCCESS if copied, error otherwise
     * @note Only valid during LOCKING state
     * @note Thread-safe: acquires mutex (LockOrder::DATA_MUTEX)
     * @note Copies to caller's buffer while holding mutex to prevent race conditions
     */
    [[nodiscard]] ErrorCode get_current_drone_type(char* buffer, size_t buffer_size) const noexcept;

    /**
     * @brief Clear all tracked drones
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void clear_tracked_drones() noexcept;

    /**
     * @brief Reset scanner frequency to first database entry
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void reset_frequency() noexcept;

    /**
     * @brief Remove drones not seen since stale timeout
     * @param current_time Current system time
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void remove_stale_drones(SystemTime current_time) noexcept;

    /**
     * @brief Set the alert callback function
     * @param callback Function to call when alerts are triggered
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void set_alert_callback(ThreatAlertCallback callback) noexcept;

    /**
     * @brief Enable or disable median filter for RSSI spike rejection
     * @param enabled true to enable, false to disable
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     */
    void set_median_filter_enabled(bool enabled) noexcept;

    /**
     * @brief Reset neighbor margin checker state
     * @note Called on frequency change to prevent stale neighbor data
     */
    void reset_neighbor_checker() noexcept;

    /**
     * @brief Get filtered RSSI through median filter
     * @return Filtered or raw RSSI
     */
    [[nodiscard]] int32_t get_filtered_rssi() const noexcept {
        return median_filter_enabled_ ? rssi_median_filter_.get_median() : 0;
    }

    /**
     * @brief Set current frequency for sweep mode
     * @note Used by UI sweep loop to keep scanner frequency in sync
     * @note No mutex — called from UI thread during sweep (scanner thread stopped)
     */
    void set_sweep_frequency(FreqHz freq) noexcept {
        current_frequency_ = freq;
    }

    /**
     * @brief Convert FFT bin index (Looking Glass reordering) to actual RF frequency
     * @param f_center Slice center frequency (Hz)
     * @param bin FFT bin index (0-255, after Looking Glass reordering)
     * @return Actual RF frequency for this bin (Hz)
     * @note Looking Glass reordering: bin 0 = Nyquist, bin 128 = DC.
     *       Bins 134-253 (lower sideband): freq = f_center + (bin-256)*BIN_SIZE
     *       Bins 2-119 (upper sideband):   freq = f_center + (bin-126)*BIN_SIZE
     *       Bins 120-133 and 0-1, 254-255 are DC spike / edge (should be skipped)
     */
    static FreqHz fft_bin_to_freq(FreqHz f_center, size_t bin) noexcept {
        constexpr FreqHz SLICE_BW = 20000000;
        constexpr FreqHz BIN_SIZE = SLICE_BW / FFT_BIN_COUNT;  // 78125
        // Looking Glass bin reordering: bin 0 = Nyquist, bin 128 = DC.
        // Lower sideband (bin >= 136): freq = f_center - 120*BIN_SIZE + bin*BIN_SIZE
        //   Avoids negative cast: (bin-256) would overflow uint64_t.
        // Upper sideband (bin < 120):  freq = f_center - 126*BIN_SIZE + bin*BIN_SIZE
        //   Avoids negative cast: (bin-126) would overflow uint64_t.
        if (bin >= FFT_DC_SPIKE_END) {
            return f_center - 120 * BIN_SIZE + static_cast<FreqHz>(bin) * BIN_SIZE;
        }
        return f_center - 126 * BIN_SIZE + static_cast<FreqHz>(bin) * BIN_SIZE;
    }

    /**
     * @brief Lightweight spectrum processing for sweep mode
     * @param spectrum Channel spectrum data (256 bins)
     * @param center_freq Current slice center frequency
     * @note Uses UNIFIED 40-bin window (100-119, 136-155) — same as spectrum mode.
     *       This ensures LNA/VGA tuning affects both modes identically.
     * @note Full spectrum shape analysis: margin + min_width + max_width + peak_sharpness + peak_ratio + valley_depth
     * @note Called from UI thread during sweep (scanner thread stopped, no mutex needed)
     * @note Implementation in scanner.cpp — NOT inline (200+ lines, too large for header)
     */
    void process_spectrum_sweep(const ChannelSpectrum& spectrum, FreqHz center_freq) noexcept;

    /**
     * @brief Apply RSSI-based threat decay (unified for both normal and sweep modes)
     * @note For each drone: if RSSI decreased or drone not detected this cycle,
     *       increment decrease counter. If counter reaches rssi_decrease_cycles (CYC), decay threat.
     *       If RSSI increased, reset counter.
     * @note Updates last_rssi_ to current rssi as new baseline for next cycle comparison.
     * @note Called from perform_scan_cycle_internal() (normal mode) and on_sweep_spectrum() (sweep mode)
     */
    void apply_rssi_decay() noexcept {
        const uint8_t threshold = config_.rssi_decrease_cycles;
        size_t write_idx = 0;
        for (size_t read_idx = 0; read_idx < tracked_count_; ++read_idx) {
            auto& drone = tracked_drones_[read_idx];
            if (drone.rssi_increased_) {
                drone.rssi_decrease_counter_ = 0;
                drone.rssi_increased_ = false;
            } else {
                if (drone.rssi_decrease_counter_ < 255) {
                    drone.rssi_decrease_counter_++;
                }
            }
            // Update baseline for next cycle comparison
            drone.last_rssi_ = static_cast<int16_t>(drone.rssi);
            if (drone.rssi_decrease_counter_ >= threshold) {
                drone.rssi_decrease_counter_ = 0;
                if (drone.decay_threat()) {
                    continue;  // drone removed (threat = NONE)
                }
            }
            if (write_idx != read_idx) {
                tracked_drones_[write_idx] = tracked_drones_[read_idx];
            }
            ++write_idx;
        }
        tracked_count_ = static_cast<uint8_t>(write_idx);
    }

    [[nodiscard]] HistogramProcessor& get_histogram_processor() noexcept {
        return histogram_processor_;
    }

    /**
     * @brief Get histogram data snapshot (thread-safe)
     * @param buffer Output buffer for histogram data
     * @param max_length Maximum buffer length
     * @return Number of histogram entries copied
     * @note Acquires mutex (LockOrder::DATA_MUTEX)
     * @note Use this from UI thread instead of get_histogram_processor().get_histogram_data()
     */
    [[nodiscard]] size_t get_histogram_snapshot(
        uint16_t* buffer,
        size_t max_length
    ) noexcept {
        MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
        if (!lock.is_locked()) {
            return 0;
        }
        return histogram_processor_.get_histogram_data(buffer, max_length);
    }

private:
    /**
     * @brief Internal: Perform scan cycle
     * @note Called by perform_scan_cycle() with mutex held
     * @return ErrorCode::SUCCESS if cycle completed, error code otherwise
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode perform_scan_cycle_internal() noexcept;
    
    /**
     * @brief Internal: Update tracked drone
     * @note Called by update_tracked_drones() with mutex held
     * @param frequency Frequency of detected signal
     * @param rssi RSSI value
     * @param timestamp Timestamp of detection
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode update_tracked_drone_internal(
        FreqHz frequency,
        RssiValue rssi,
        SystemTime timestamp
    ) noexcept;
    
    /**
     * @brief Internal: Find drone by frequency
     * @param frequency Frequency to find
     * @return ErrorResult containing index or error
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorResult<size_t> find_drone_by_frequency_internal(
        FreqHz frequency
    ) const noexcept;
    
    /**
     * @brief Internal: Add new tracked drone
     * @param frequency_hz Frequency of detected signal (Hz)
     * @param rssi_dbm RSSI value (dBm)
     * @param timestamp_ms Timestamp of detection (ms)
     * @return ErrorCode::SUCCESS if added, error code otherwise
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] ErrorCode add_tracked_drone_internal(
        FreqHz frequency_hz,
        RssiValue rssi_dbm,
        SystemTime timestamp_ms
    ) noexcept;
    
    /**
     * @brief Internal: Remove stale drones
     * @note Called by remove_stale_drones() with mutex held
     * @param current_time Current system time
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    void remove_stale_drones_internal(SystemTime current_time) noexcept;
    
    /**
     * @brief Internal: Determine drone type from frequency
     * @param frequency Frequency to analyze
     * @return Drone type
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     * @note Uses database_.find_entry() to look up drone type from freqman DB
     */
    [[nodiscard]] DroneType determine_drone_type_internal(FreqHz frequency) const noexcept;

    /**
     * @brief Internal: Validate scan configuration
     * @param config Configuration to validate
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_config_internal(const ScanConfig& config) const noexcept;

    /**
     * @brief Internal: Analyze spectrum shape for U/V signal peaks
     * @param spectrum Channel spectrum data (256 bins, 0-255 each)
     * @param out_rssi Estimated RSSI in dBm if signal detected
     * @return true if drone-like signal detected (elevated peak with width)
     * @note Noise floor = flat line. Signal = elevated U/V peak above noise.
     * @pre Mutex must be held (LockOrder::DATA_MUTEX)
     */
    [[nodiscard]] bool analyze_spectrum_shape(const ChannelSpectrum& spectrum, int32_t& out_rssi) noexcept;

    /**
     * @brief Internal: Trigger alert callback if set
     * @param threat_level Threat level to report
     * @note Re-entrant safe via AtomicFlag guard
     * @pre Mutex must NOT be held (callback must be lock-free)
     */
    void trigger_alert(ThreatLevel threat_level) noexcept;

    // References to dependencies
    DatabaseManager& database_;
    HardwareController& hardware_;
    
    // Scanner state
    ScannerState state_;
    
    // Scan configuration
    ScanConfig config_;

    // Fast scanner state (protected by mutex_)
    uint32_t freq_lock_count_{0};              // Frequency lock counter (0-10)
    FreqHz locked_frequency_{0};                // Locked frequency for tracking
    SystemTime track_start_time_{0};           // Tracking start time
    char current_drone_type_[5]{'\0', '\0', '\0', '\0', '\0'};  // All bytes initialized
    bool drone_type_valid_{false};              // Drone type valid flag
    
    // Scan statistics
    ScanStatistics statistics_;
    
    // Tracked drones (fixed-size array, no heap allocation)
    std::array<TrackedDrone, MAX_TRACKED_DRONES> tracked_drones_;
    
    // Number of tracked drones (uint8_t sufficient for MAX_TRACKED_DRONES=16)
    uint8_t tracked_count_;
    
    // Current scan frequency
    FreqHz current_frequency_;

    // Pending detection hysteresis (prevent noise from adding phantom drones)
    FreqHz pending_frequency_{0};
    uint8_t pending_count_{0};
    static constexpr uint8_t DETECT_CONFIRM_COUNT = 2;

    // Decay: run once per frequency change (not every frame)
    FreqHz last_decay_freq_{0};

    // RSSI-based decay cycle counter (normal mode): triggers apply_rssi_decay() every CYC cycles
    uint8_t rssi_decay_cycle_counter_{0};

    // Noise blacklist: track force-resume count per frequency
    // If we force-resume from a freq 3+ times without threat upgrade → skip it
    static constexpr size_t MAX_NOISE_ENTRIES = 8;
    struct NoiseEntry { FreqHz freq; uint8_t count; };
    NoiseEntry noise_blacklist_[MAX_NOISE_ENTRIES]{};

    // Last scan time
    SystemTime last_scan_time_;
    
    // Scanning active flag
    AtomicFlag scanning_active_;
    
    // Alert callback
    ThreatAlertCallback alert_callback_;
    
    // Mutex for thread safety (LockOrder::DATA_MUTEX)
    mutable Mutex mutex_;

    // State transition control flag
    AtomicFlag state_transition_allowed_;

    // Force-resume flag (set by scanner thread, cleared inside mutex-protected scan cycle)
    AtomicFlag force_resume_flag_;

    // Sort buffer for analyze_spectrum_shape (class member to avoid static in method)
    static constexpr size_t SPECTRUM_SORT_BUF_SIZE = 256;
    uint8_t spectrum_sort_buf_[SPECTRUM_SORT_BUF_SIZE];

    // Usable bins buffer for process_spectrum_sweep (class member to avoid static in method)
    // (FFT_DC_SPIKE_START - FFT_EDGE_SKIP_NARROW) + (FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW - FFT_DC_SPIKE_END)
    // = (120 - 6) + (256 - 6 - 136) = 114 + 114 = 228
    static constexpr size_t SWEEP_USABLE_BINS = 228;
    uint8_t sweep_usable_buf_[SWEEP_USABLE_BINS];

    // Alert callback in progress flag (prevents re-entrant calls)
    AtomicFlag alert_callback_in_progress_;

    // RSSI detector for signal analysis and threat classification
    RSSIDetector rssi_detector_;

    // Histogram processor for spectrum analysis
    HistogramProcessor histogram_processor_;

    // Median filter for RSSI spike rejection (window=7 samples)
    MedianFilter<int32_t, 7> rssi_median_filter_;
    bool median_filter_enabled_{false};

    // Neighbor margin checker for anti-false-positive detection
    NeighborMarginChecker neighbor_margin_checker_;
};

} // namespace drone_analyzer

#endif // SCANNER_HPP
