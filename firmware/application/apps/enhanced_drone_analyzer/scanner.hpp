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
    
    // Sweep range (Hz)
    FreqHz sweep_start_freq;
    FreqHz sweep_end_freq;
    FreqHz sweep_step_freq;

    // Advanced detection features (OFF by default)
    bool dwell_enabled{false};           // Stay on frequency when signal detected
    bool confirm_count_enabled{false};   // Require multiple confirmations before creating drone
    bool noise_blacklist_enabled{false}; // Skip frequencies with persistent noise
    bool spectrum_detection_enabled{false}; // Detect drone signals by spectrum shape (U/V peaks)
    bool median_enabled{false};             // Median filter for RSSI spike rejection
    uint8_t spectrum_margin{15};            // Peak margin above noise (0-200, default 15 ≈ 5 dB)
    uint8_t spectrum_min_width{1};          // Min signal width in bins (1-20, default 1)
    
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
        constexpr FreqHz BIN_SIZE = SLICE_BW / 256;

        const FreqHz range = cfg.end_freq - cfg.start_freq;
        pixel_step_hz = (range > 0) ? range / 240 : 0;
        step_hz = 244 * BIN_SIZE;
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
        return pixel_index >= 240;
    }
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
     * @brief Lightweight spectrum processing for sweep mode
     * @param spectrum Channel spectrum data (256 bins)
     * @param center_freq Current slice center frequency
     * @note Uses UNIFIED 40-bin window (100-119, 136-155) — same as spectrum mode.
     *       This ensures LNA/VGA tuning affects both modes identically.
     * @note Median filter + margin gate for spike/noise rejection.
     * @note Called from UI thread during sweep (scanner thread stopped, no mutex needed)
     */
    void process_spectrum_sweep(const ChannelSpectrum& spectrum, FreqHz center_freq) noexcept {
        current_frequency_ = center_freq;

        // Expanded bin window: EDGE_SKIP=6 on both sides, skip DC spike
        // Matches display coverage (240 bins) minus edge artifacts
        // Display: bins 2-121 + 134-253 (240 bins)
        // Detection: bins 6-119 + 136-250 (228 bins)
        constexpr size_t EDGE_SKIP = 6;
        constexpr size_t DC_SPIKE_START = 120;
        constexpr size_t DC_SPIKE_END = 136;
        constexpr size_t BIN_COUNT = 228;  // (120-6) + (256-6-136)

        // Step 1: Collect bins + find peak
        // Static: called only from UI thread (DisplayFrameSync callback)
        static uint8_t usable[BIN_COUNT];
        size_t idx = 0;
        uint8_t raw_peak = 0;

        for (size_t i = EDGE_SKIP; i < DC_SPIKE_START; ++i) {
            usable[idx++] = spectrum.db[i];
            if (spectrum.db[i] > raw_peak) raw_peak = spectrum.db[i];
        }
        for (size_t i = DC_SPIKE_END; i < (256 - EDGE_SKIP); ++i) {
            usable[idx++] = spectrum.db[i];
            if (spectrum.db[i] > raw_peak) raw_peak = spectrum.db[i];
        }

        // Step 2: Insertion sort for median (O(n²), n=228)
        for (size_t i = 1; i < idx; ++i) {
            const uint8_t key = usable[i];
            size_t j = i;
            while (j > 0 && usable[j - 1] > key) {
                usable[j] = usable[j - 1];
                --j;
            }
            usable[j] = key;
        }

        // Step 3: Noise floor = median, margin check rejects flat noise
        const uint8_t noise_floor = usable[idx / 2];
        int32_t peak_rssi = static_cast<int32_t>(raw_peak) - 120;

        if (raw_peak <= noise_floor + config_.spectrum_margin) {
            return;  // Signal too weak above noise floor — reject
        }

        // Step 4: Median filter for spike rejection
        rssi_median_filter_.add(peak_rssi);
        if (median_filter_enabled_ && rssi_median_filter_.is_warm()) {
            peak_rssi = rssi_median_filter_.get_median();
        }

        // Step 5: Update drone tracker only for above-threshold signals
        if (peak_rssi > config_.rssi_threshold_dbm) {
            (void)update_tracked_drone_internal(center_freq, peak_rssi, chTimeNow());
        }
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
     * @brief Unified RSSI extraction for both spectrum and sweep modes
     * @param spectrum Channel spectrum data (256 bins, 0-255 each)
     * @param out_rssi Extracted RSSI in dBm (peak from focused bin window)
     * @param out_noise_floor Noise floor estimate (0-255 scale, median of usable bins)
     * @return true if signal detected above noise floor + spectrum_margin
     * @note Uses focused 40-bin window (100-119, 136-155) centered on DC spike.
     *       This matches the tuned frequency's actual energy location.
     * @note Consistent bin selection ensures LNA/VGA tuning affects both modes identically.
     * @pre Mutex must NOT be held (static buffers, no shared state)
     */
    [[nodiscard]] bool extract_rssi_unified(
        const ChannelSpectrum& spectrum,
        int32_t& out_rssi,
        uint8_t& out_noise_floor
    ) const noexcept;

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

    // Alert callback in progress flag (prevents re-entrant calls)
    AtomicFlag alert_callback_in_progress_;

    // RSSI detector for signal analysis and threat classification
    RSSIDetector rssi_detector_;

    // Histogram processor for spectrum analysis
    HistogramProcessor histogram_processor_;

    // Median filter for RSSI spike rejection (window=7 samples)
    MedianFilter<int32_t, 7> rssi_median_filter_;
    bool median_filter_enabled_{false};
};

} // namespace drone_analyzer

#endif // SCANNER_HPP
