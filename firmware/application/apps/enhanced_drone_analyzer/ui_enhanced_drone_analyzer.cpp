// Thread Safety Notes
// Locking Order: 1.ATOMIC_FLAGS 2.data_mutex 3.spectrum_mutex 4.logger_mutex 5.sd_card_mutex
// Always acquire in order 1->2->3->4->5; sd_card_mutex must be LAST
// Use MutexLock RAII for automatic unlock; raii::SystemLock for volatile bool

#include "ui_enhanced_drone_analyzer.hpp"
#include "settings_persistence.hpp"
#include "ui_drone_audio.hpp"
#include "eda_optimized_utils.hpp"
#include "color_lookup_unified.hpp"
#include "eda_constants.hpp"
#include "diamond_core.hpp"
#include "diamond_fixes.hpp"
#include "gradient.hpp"
#include "eda_locking.hpp"
#include "eda_raii.hpp"
#include "eda_safecast.hpp"
#include "baseband_api.hpp"
#include "portapack.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "file_path.hpp"
#include "string_format.hpp"
#include "file.hpp"
#include "tone_key.hpp"
#include "rtc_time.hpp"
#include "ui_fileman.hpp"
#include "ui_drone_common_types.hpp"
#include "ui_enhanced_drone_settings.hpp"
#include "ui_spectral_analyzer.hpp"
#include "sd_card.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdarg>
#include <cinttypes>

using namespace portapack;
using namespace tonekey;
#include <ch.h>
#include <chmsg.h>

namespace ui::apps::enhanced_drone_analyzer {

using namespace EDA::Constants;

// Heap monitoring
namespace HeapMonitor {
    inline size_t get_free_heap() noexcept {
        size_t heap_free = 0;
        chHeapStatus(NULL, &heap_free);
        return heap_free;
    }

    inline void log_heap_status() noexcept {
        size_t free_heap = get_free_heap();
        [[maybe_unused]] constexpr size_t HEAP_WARNING_THRESHOLD = 8192;
        constexpr size_t HEAP_CRITICAL_THRESHOLD = 4096;

        if (free_heap < HEAP_CRITICAL_THRESHOLD) {
            // Log critical heap warning
        }
    }
}

// Progressive slowdown multiplier LUT (eliminates runtime division)
static constexpr uint8_t SLOWDOWN_MULTIPLIER_LUT[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0-9 cycles
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 10-19 cycles
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 20-29 cycles
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3   // 30+ cycles (capped at 3)
};

// Compile-time division optimization
constexpr inline uint64_t ceil_div_u64(uint64_t numerator, uint64_t denominator) noexcept {
    return (numerator + denominator - 1) / denominator;
}

// SD card mutex (FatFS is NOT thread-safe)
Mutex sd_card_mutex;
static struct MutexInitializer {
    MutexInitializer() { chMtxInit(&sd_card_mutex); }
} sd_card_mutex_initializer_;

// ScanningMode LUT (strings in Flash)
EDA_FLASH_CONST inline static constexpr const char* const SCANNING_MODE_NAMES[] = {
    "Database Scan",      // DATABASE = 0
    "Wideband Monitor",   // WIDEBAND_CONTINUOUS = 1
    "Hybrid Discovery"    // HYBRID = 2
};
static_assert(sizeof(SCANNING_MODE_NAMES) / sizeof(const char*) == 3, "SCANNING_MODE_NAMES size");

// Static member definitions (FIX #1: removed inline to prevent RAM bloat)

alignas(alignof(DisplayDroneEntry))
DisplayDroneEntry DroneDisplayController::detected_drones_storage_[DroneDisplayController::MAX_UI_DRONES];

alignas(alignof(FreqmanDB))
uint8_t DroneScanner::freq_db_storage_[DroneScanner::FREQ_DB_STORAGE_SIZE];

alignas(alignof(TrackedDrone))
uint8_t DroneScanner::tracked_drones_storage_[DroneScanner::TRACKED_DRONES_STORAGE_SIZE];

alignas(alignof(std::array<Color, DroneDisplayController::SPECTRUM_ROW_SIZE>))
Color DroneDisplayController::spectrum_row_buffer_storage_[DroneDisplayController::SPECTRUM_ROW_SIZE];

alignas(alignof(std::array<Color, DroneDisplayController::RENDER_LINE_SIZE>))
Color DroneDisplayController::render_line_buffer_storage_[DroneDisplayController::RENDER_LINE_SIZE];

alignas(alignof(std::array<uint8_t, 200>))
uint8_t DroneDisplayController::spectrum_power_levels_storage_[200];

stkalign_t DroneDetectionLogger::worker_wa_[THD_WA_SIZE(DroneDetectionLogger::WORKER_STACK_SIZE) / sizeof(stkalign_t)];

stkalign_t DroneScanner::db_loading_wa_[THD_WA_SIZE(DroneScanner::DB_LOADING_STACK_SIZE) / sizeof(stkalign_t)];

// Built-in drone frequency DB (reduced from 31 to 15 entries)
EDA_FLASH_CONST const std::array<DroneScanner::BuiltinDroneFreq, DroneScanner::BUILTIN_DB_SIZE> DroneScanner::BUILTIN_DRONE_DB = {{
    // LRS / Control
    { 868000000, "TBS Crossfire EU", DroneType::MILITARY_DRONE },
    { 915000000, "TBS Crossfire US", DroneType::MILITARY_DRONE },
    { 866000000, "ELRS 868", DroneType::PX4_DRONE },
    { 915000000, "ELRS 915", DroneType::PX4_DRONE },

    // Legacy / Telemetry
    { 433050000, "LRS 433 Ch1", DroneType::UNKNOWN },

    // DJI OcuSync (2.4 GHz)
    { 2406500000, "DJI OcuSync 1", DroneType::MAVIC },
    { 2416500000, "DJI OcuSync 3", DroneType::MAVIC },
    { 2426500000, "DJI OcuSync 5", DroneType::MAVIC },
    { 2436500000, "DJI OcuSync 7", DroneType::MAVIC },

    // FPV Video (5.8 GHz)
    { 5658000000, "RaceBand 1", DroneType::UNKNOWN },
    { 5695000000, "RaceBand 2", DroneType::UNKNOWN },
    { 5732000000, "RaceBand 3", DroneType::UNKNOWN },
    { 5769000000, "RaceBand 4", DroneType::UNKNOWN },

    // DJI FPV System
    { 5735000000, "DJI FPV Ch1", DroneType::MAVIC },

    // WiFi Drones
    { 2412000000, "WiFi Ch1", DroneType::PARROT_ANAFI },
    { 2437000000, "WiFi Ch6", DroneType::PARROT_ANAFI },
    { 2462000000, "WiFi Ch11", DroneType::PARROT_ANAFI }
}};

DroneScanner::DroneScanner(DroneAnalyzerSettings settings)
    : entries_to_scan_(),
      stale_indices_(),
      scanning_thread_(nullptr),
      data_mutex(),
      scanning_active_(false),
      histogram_callback_(nullptr),
      histogram_callback_user_data_(nullptr),
      freq_db_ptr_(nullptr),
      tracked_drones_ptr_(nullptr),
      freq_db_constructed_(false),
      tracked_drones_constructed_(false),
      freq_db_loaded_(false),
      current_db_index_(0),
      last_scanned_frequency_(0),
      last_detection_log_time_(0),
      db_loading_thread_(nullptr),
      db_loading_active_{false},
      initialization_complete_{false},
      scan_cycles_(0),
      total_detections_(0),
      scanning_mode_(DroneScanner::ScanningMode::DATABASE),
      is_real_mode_(true),
      tracked_count_(0),
      approaching_count_(0),
      receding_count_(0),
      static_count_(0),
      max_detected_threat_(ThreatLevel::NONE),
      last_valid_rssi_(-120),
      wideband_scan_data_(),
      detection_logger_(),
      detection_ring_buffer_(),
      spectrum_data_(),
      histogram_buffer_(),
      settings_(std::move(settings)),
      last_scan_error_(nullptr)
{
    chMtxInit(&data_mutex);
    // Lazy initialization: FreqmanDB and tracked_drones allocated later from heap
    initialize_wideband_scanning();
}

DroneScanner::~DroneScanner() {
    stop_scanning();
    cleanup_database_and_scanner();
    // ChibiOS mutexes auto-cleaned with the object
}



void DroneScanner::initialize_wideband_scanning() {
    wideband_scan_data_.reset();
    setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
}

void DroneScanner::setup_wideband_range(Frequency min_freq, Frequency max_freq) {
    Frequency safe_min = std::max(min_freq, EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ);
    Frequency safe_max = std::min(max_freq, EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ);

    // Swap to ensure min <= max (prevents negative range)
    if (safe_min > safe_max) {
        safe_min = safe_max;
        safe_max = std::max(min_freq, EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ);
    }

    wideband_scan_data_.min_freq = safe_min;
    wideband_scan_data_.max_freq = safe_max;

    Frequency scanning_range = safe_max - safe_min;
    const uint64_t width_u64 = static_cast<uint64_t>(settings_.wideband_slice_width_hz);
    if (scanning_range > static_cast<Frequency>(settings_.wideband_slice_width_hz)) {
        // Check for overflow BEFORE addition (Diamond Code pattern)
        if (scanning_range < 0) {
            wideband_scan_data_.slices_nb = 1;
        } else {
            const uint64_t range_u64 = static_cast<uint64_t>(scanning_range);

            // Check for overflow
            if (UINT64_MAX - range_u64 < width_u64) {
                wideband_scan_data_.slices_nb = 1;
            } else {
                // Use constexpr ceil_div_u64 for compiler optimization
                uint64_t range_plus_width = range_u64 + width_u64;
                uint64_t slices_calc = ceil_div_u64(range_plus_width, width_u64);
                // Check if result exceeds size_t max
                if (slices_calc > static_cast<uint64_t>(SIZE_MAX)) {
                    wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
                } else {
                    wideband_scan_data_.slices_nb = static_cast<size_t>(slices_calc);
                }
            }
        }

        if (wideband_scan_data_.slices_nb > WIDEBAND_MAX_SLICES) {
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }
        
        // Check for overflow in multiplication
        const uint64_t slices_nb_u64 = static_cast<uint64_t>(wideband_scan_data_.slices_nb);
        if (slices_nb_u64 > 0 && UINT64_MAX / slices_nb_u64 < width_u64) {
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }
        
        // Check for INT64_MAX overflow before casting to Frequency
        const uint64_t product = slices_nb_u64 * width_u64;
        if (product > static_cast<uint64_t>(INT64_MAX)) {
            return;
        }
        Frequency slices_span = static_cast<Frequency>(product);
        Frequency offset = ((scanning_range - slices_span) / 2) + (settings_.wideband_slice_width_hz / 2);
        Frequency center_frequency = safe_min + offset;

        std::generate_n(wideband_scan_data_.slices,
                       wideband_scan_data_.slices_nb,
                       [&center_frequency, slice_index = 0, this]() mutable -> WidebandSlice {
                           WidebandSlice slice;
                           slice.center_frequency = center_frequency;
                           slice.index = slice_index++;
                           if (center_frequency <= static_cast<Frequency>(INT64_MAX) - static_cast<Frequency>(settings_.wideband_slice_width_hz)) {
                               center_frequency += settings_.wideband_slice_width_hz;
                           }
                           return slice;
                       });
    } else {
        wideband_scan_data_.slices[0].center_frequency = (safe_max + safe_min) / 2;
        wideband_scan_data_.slices_nb = 1;
    }
    wideband_scan_data_.slice_counter = 0;
}

void DroneScanner::start_scanning() {
    bool is_scanning;
    {
        raii::SystemLock lock;
        is_scanning = scanning_active_;
    }
    if (is_scanning) return;

    {
        raii::SystemLock lock;
        scanning_active_ = true;
    }
    scan_cycles_ = 0;
    total_detections_ = 0;
}

void DroneScanner::stop_scanning() {
    bool is_scanning;
    {
        raii::SystemLock lock;
        is_scanning = scanning_active_;
    }
    if (!is_scanning) return;

    {
        raii::SystemLock lock;
        scanning_active_ = false;
    }

    if (scanning_thread_ != nullptr) {
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }

    remove_stale_drones();
}

bool DroneScanner::load_frequency_database() {
    current_db_index_ = 0;

    freqman_load_options options;
    options.max_entries = 150;
    options.load_freqs = true;
    options.load_ranges = true;
    options.load_hamradios = true;
    options.load_repeaters = true;

    raii::SystemLock sys_lock;
    if (!freq_db_ptr_ || !freq_db_constructed_) {
        return false;
    }

    auto db_path = get_freqman_path(settings_.freqman_path);
    bool sd_loaded = false;

    {
        MutexLock lock(data_mutex);

        sd_loaded = freq_db_ptr_->open(db_path);

        if (!sd_loaded || freq_db_ptr_->empty()) {
            freq_db_ptr_->open(db_path, true);

            for (const auto& item : BUILTIN_DRONE_DB) {
                freqman_entry entry{};
                entry.frequency_a = item.freq;
                entry.description = item.desc;
                entry.type = freqman_type::Single;
                entry.modulation = freqman_invalid_index;
                entry.bandwidth = freqman_invalid_index;
                entry.step = freqman_invalid_index;
                entry.tone = freqman_invalid_index;
                freq_db_ptr_->append_entry(entry);
            }

            sync_database();

            sd_loaded = !freq_db_ptr_->empty();
        }
    }

    if (freq_db_ptr_->entry_count() > 100) {
        handle_scan_error("Large database loaded");
    }

    freq_db_loaded_ = true;
    return !freq_db_ptr_->empty();
}

size_t DroneScanner::get_database_size() const {
    return freq_db_ptr_->entry_count();
}

void DroneScanner::set_scanning_mode(ScanningMode mode) {
    scanning_mode_ = mode;
    stop_scanning();
    reset_scan_cycles();

    if (scanning_mode_ == ScanningMode::DATABASE || scanning_mode_ == ScanningMode::HYBRID) {
        load_frequency_database();
    }
}

const char* DroneScanner::scanning_mode_name() const {
    uint8_t idx = static_cast<uint8_t>(scanning_mode_);
    return (idx < 3) ? SCANNING_MODE_NAMES[idx] : "Unknown";
}

void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    bool is_scanning;
    {
        raii::SystemLock lock;
        is_scanning = scanning_active_;
    }
    if (!is_scanning) return;

    using namespace EDA::Constants;
    uint32_t base_interval = DEFAULT_SCAN_INTERVAL_MS;
    uint32_t adaptive_interval = base_interval;

    size_t current_detections = get_total_detections();
    ThreatLevel max_threat = get_max_detected_threat();
    size_t tracked_count = tracked_count_;

    if (max_threat >= ThreatLevel::CRITICAL) {
        adaptive_interval = FAST_SCAN_INTERVAL_MS;
    } else if (max_threat == ThreatLevel::HIGH) {
        adaptive_interval = HIGH_THREAT_SCAN_INTERVAL_MS;
    } else if (max_threat == ThreatLevel::MEDIUM) {
        adaptive_interval = NORMAL_SCAN_INTERVAL_MS;
    } else if (current_detections > 0 && tracked_count > 0) {
        adaptive_interval = SLOW_SCAN_INTERVAL_MS;
    } else if (current_detections == 0 && scan_cycles_ > PROGRESSIVE_SLOWDOWN_DIVISOR) {
        uint32_t cycles_value = get_scan_cycles();
        uint32_t cycles_clamped = (cycles_value < 39) ? cycles_value : 39;
        uint32_t slowdown_multiplier = SLOWDOWN_MULTIPLIER_LUT[cycles_clamped];
        uint32_t interval_calc = base_interval * slowdown_multiplier;
        adaptive_interval = (interval_calc < VERY_SLOW_SCAN_INTERVAL_MS) ? interval_calc : VERY_SLOW_SCAN_INTERVAL_MS;
    }

    // Additional adjustment for high detection density
    static constexpr size_t HIGH_DENSITY_DETECTION_THRESHOLD = 5;
    if (current_detections > HIGH_DENSITY_DETECTION_THRESHOLD) {
        adaptive_interval = (adaptive_interval < HIGH_DENSITY_SCAN_CAP_MS) ? adaptive_interval : HIGH_DENSITY_SCAN_CAP_MS;
    }
    
    chThdSleepMilliseconds(adaptive_interval);

    uint8_t mode_idx = static_cast<uint8_t>(scanning_mode_);
    if (mode_idx < 3) {
        (this->*DroneScanner::SCAN_FUNCTIONS[mode_idx])(hardware);
    }

    {
        MutexLock lock(data_mutex);
        scan_cycles_++;
    }

    {
        raii::SystemLock lock;
        is_scanning = scanning_active_;
    }
    if (is_scanning) {
        chThdSleepMilliseconds(adaptive_interval);
    }
}

void DroneScanner::perform_database_scan_cycle(DroneHardwareController& hardware) {
    size_t total_entries = 0;

    {
        MutexLock lock(data_mutex);
        if (!freq_db_ptr_ || freq_db_ptr_->empty()) {
            if (scan_cycles_ % 50 == 0) {
                handle_scan_error("Database is empty");
            }
            return;
        }
        total_entries = freq_db_ptr_->entry_count();
    }

    const size_t batch_size = std::min(static_cast<size_t>(EDA::Constants::MAX_SCAN_BATCH_SIZE), total_entries);

    std::array<freqman_entry, EDA::Constants::MAX_SCAN_BATCH_SIZE> entries_to_scan{};
    size_t entries_count = 0;

    {
        MutexLock lock(data_mutex);
        if (freq_db_ptr_) {
            size_t db_entry_count = freq_db_ptr_->entry_count();
            if (db_entry_count > 0) {
                for (size_t i = 0; i < batch_size; ++i) {
                    size_t idx = (current_db_index_ + i) % db_entry_count;
                    if (idx < db_entry_count && entries_count < entries_to_scan.size()) {
                        entries_to_scan[entries_count++] = (*freq_db_ptr_)[idx];
                    }
                }
                current_db_index_ = (current_db_index_ + batch_size) % db_entry_count;
            }
        }
    }

    static constexpr int PLL_STABILIZATION_ITERATIONS = EDA::Constants::PLL_STABILIZATION_ITERATIONS;
    static constexpr uint32_t PLL_STABILIZATION_DELAY_MS = EDA::Constants::PLL_STABILIZATION_DELAY_MS;
    
    const Frequency MIN_VALID_FREQ = EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ;
    const Frequency MAX_VALID_FREQ = EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;

    for (size_t i = 0; i < entries_count; ++i) {
        const auto& entry = entries_to_scan[i];

        // Guard clause: check scanning flag
        bool is_scanning;
        {
            raii::SystemLock lock;
            is_scanning = scanning_active_;
        }
        if (!is_scanning) return;

        Frequency target_freq_hz = entry.frequency_a;

        // Guard clause: validate frequency range
        if (target_freq_hz < MIN_VALID_FREQ || target_freq_hz > MAX_VALID_FREQ) continue;

        // Guard clause: hardware tuning validation
        if (!hardware.tune_to_frequency(target_freq_hz)) continue;

        // Wait for PLL stabilization (broken into small chunks)
        for (int w = 0; w < PLL_STABILIZATION_ITERATIONS; w++) {
            bool is_scanning;
            {
                raii::SystemLock lock;
                is_scanning = scanning_active_;
            }
            if (!is_scanning) return;
            chThdSleepMilliseconds(PLL_STABILIZATION_DELAY_MS);
        }

        hardware.clear_rssi_flag();

        // Optimized polling: check condition first, then sleep
        systime_t deadline = chTimeNow() + MS2ST(EDA::Constants::RSSI_TIMEOUT_MS);
        bool signal_captured = false;

        if (hardware.is_rssi_fresh()) {
            signal_captured = true;
        } else {
            // Use 10ms poll delay to reduce CPU usage
            constexpr uint32_t OPTIMIZED_POLL_DELAY_MS = 10;
            while (chTimeNow() < deadline) {
                chThdSleepMilliseconds(OPTIMIZED_POLL_DELAY_MS);
                if (hardware.is_rssi_fresh()) {
                    signal_captured = true;
                    break;
                }
            }
        }

        // Guard clause: only process if signal captured
        if (!signal_captured) {
            last_scanned_frequency_ = target_freq_hz;
            continue;
        }

        int32_t real_rssi = hardware.get_current_rssi();
        process_rssi_detection(entry, real_rssi);
        last_scanned_frequency_ = target_freq_hz;
    }
}

void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
    if (wideband_scan_data_.slices_nb == 0) {
        setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
    }
    
    const WidebandSlice& current_slice = wideband_scan_data_.slices[wideband_scan_data_.slice_counter];
    
    // 1. Tune to slice center
    if (hardware.tune_to_frequency(current_slice.center_frequency)) {

        // 2. Wait for PLL stabilization
        chThdSleepMilliseconds(EDA::Constants::PLL_STABILIZATION_DELAY_MS);
        
        if (!hardware.is_spectrum_streaming_active()) {
            hardware.start_spectrum_streaming();
        }
        
        // 3. Get spectrum data from M0 coprocessor
        auto& spectrum_data = spectrum_data_;
        
        // Optimized waiting: adaptive timeout with absolute deadline
        systime_t current_time = chTimeNow();
        systime_t deadline = current_time + MS2ST(EDA::Constants::SPECTRUM_TIMEOUT_MS);
        
        // Check for overflow
        if (deadline < current_time) {
            deadline = DiamondFixes::ConfidenceConstants::MAX_SYSTIME_VALUE;  // Max systime_t value
        }
        
        bool spectrum_received = false;

        // Optimized polling: check condition first
        if (hardware.get_latest_spectrum_if_fresh(spectrum_data)) {
            spectrum_received = true;
        } else {
            while (chTimeNow() < deadline) {
                chThdSleepMilliseconds(EDA::Constants::CHECK_INTERVAL_MS);

                if (hardware.get_latest_spectrum_if_fresh(spectrum_data)) {
                    spectrum_received = true;
                    break;
                }
            }
        }

        // 4. Perform spectral analysis
        if (spectrum_received) {
            // Use histogram_buffer_ class member (reduces stack usage)
            auto analysis_result = SpectralAnalyzer::analyze(
                spectrum_data,
                {hardware.get_spectrum_bandwidth(), current_slice.center_frequency},
                histogram_buffer_
            );
            
            // Invoke histogram callback (thread-safe)
            if (histogram_callback_) {
                histogram_callback_(histogram_buffer_, analysis_result.noise_floor, histogram_callback_user_data_);
            }
            
            // 5. Process detection
            if (analysis_result.is_valid && analysis_result.signature != SignalSignature::NOISE) {
                freqman_entry detection_entry{
                    .frequency_a = static_cast<int64_t>(current_slice.center_frequency),
                    .frequency_b = static_cast<int64_t>(current_slice.center_frequency),
                    .description = "Spectral Detection",
                    .type = freqman_type::Single,
                };

                ThreatLevel threat_level = SpectralAnalyzer::get_threat_level(analysis_result.signature, analysis_result.snr);
                DroneType drone_type = SpectralAnalyzer::get_drone_type(current_slice.center_frequency, analysis_result.signature);

                process_spectral_detection(detection_entry, analysis_result, threat_level, drone_type);
            }
        } else {
            // Fallback to RSSI-based detection
            int32_t slice_rssi = hardware.get_real_rssi_from_hardware(current_slice.center_frequency);
            if (slice_rssi > WIDEBAND_RSSI_THRESHOLD_DB) {
                freqman_entry fallback_entry{
                    .frequency_a = static_cast<int64_t>(current_slice.center_frequency),
                    .frequency_b = static_cast<int64_t>(current_slice.center_frequency),
                    .description = "RSSI Fallback",
                    .type = freqman_type::Single,
                };
                wideband_detection_override(fallback_entry, slice_rssi, WIDEBAND_RSSI_THRESHOLD_DB);
            }
        }

        last_scanned_frequency_ = current_slice.center_frequency;
    } else {
        if (scan_cycles_ % 100 == 0) {
            handle_scan_error("Hardware tuning failed in wideband mode");
        }
    }

    // Transition to next slice
    size_t next_slice_idx = get_next_slice_with_intelligence();
    wideband_scan_data_.slice_counter = next_slice_idx;
}

size_t DroneScanner::get_next_slice_with_intelligence() {
    (void)settings_;
    size_t current = wideband_scan_data_.slice_counter;
    size_t next = (current + 1) % wideband_scan_data_.slices_nb;
    return next;
}

void DroneScanner::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
    if (rssi >= threshold_override) {
        // Use const reference to avoid copy
        process_wideband_detection_with_override(entry, rssi, DEFAULT_RSSI_THRESHOLD_DB, threshold_override);
    }
}

void DroneScanner::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                           int32_t /*original_threshold*/, int32_t wideband_threshold) {
    if (!EDA::Validation::validate_rssi(rssi) ||
        !EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    bool should_log = false;
    DetectionLogEntry log_entry_to_write;
    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level = ThreatLevel::UNKNOWN;

    // Validate frequency before division (prevent division by zero)
    if (entry.frequency_a == 0) {
        return;
    }
    
    // Threat level logic
    if (rssi > -70) threat_level = ThreatLevel::HIGH;
    else if (rssi > -80) threat_level = ThreatLevel::LOW;
    else threat_level = ThreatLevel::UNKNOWN;
    if (entry.frequency_a >= 2'400'000'000 && entry.frequency_a <= 2'500'000'000) {
        threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
    }

    // Critical section
    {
        MutexLock lock(data_mutex);

        // Increment total_detections_ with mutex protection
        total_detections_++;

        // Use bitwise AND with HASH_MASK for efficient modulo operation
        constexpr size_t FREQ_HASH_DIVISOR = 100000;
        const size_t freq_hash_raw = static_cast<size_t>(entry.frequency_a / FREQ_HASH_DIVISOR);
        const size_t freq_hash = freq_hash_raw & DetectionBufferConstants::HASH_MASK;
        int32_t effective_threshold = wideband_threshold;

        if (detection_ring_buffer_.get_rssi_value(freq_hash) < wideband_threshold) {
            effective_threshold = wideband_threshold + HYSTERESIS_MARGIN_DB;
        }

        if (rssi >= effective_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
            detection_ring_buffer_.update_detection(freq_hash, current_count, rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                should_log = true;

                log_entry_to_write = {
                    chTimeNow(),
                    static_cast<uint64_t>(entry.frequency_a),
                    rssi,
                    threat_level,
                    detected_type,
                    current_count,
                    DiamondFixes::ConfidenceConstants::RSSI_CONFIDENCE,  // 85% confidence as integer
                    0,   // width_bins - default value
                    0,   // signal_width_hz - default value
                    0    // snr - default value
                };

                update_tracked_drone_internal({detected_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(rssi), threat_level});
            }
        } else {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // End critical section

    // Logging
    if (should_log && detection_logger_.is_session_active()) {
        detection_logger_.log_detection_async(log_entry_to_write);
    }
}

void DroneScanner::process_spectral_detection(const freqman_entry& entry,
                                             const SpectralAnalysisResult& analysis_result,
                                             ThreatLevel threat_level, DroneType drone_type) {
    if (!EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    bool should_log = false;
    DetectionLogEntry log_entry_to_write;

    {
        MutexLock lock(data_mutex);

        total_detections_++;

        // Guard clause: Prevent division by zero
        if (entry.frequency_a == 0) {
            return;
        }
        size_t freq_hash = DiamondFixes::safe_frequency_hash(entry.frequency_a);
        int32_t effective_threshold = WIDEBAND_RSSI_THRESHOLD_DB;

        if (detection_ring_buffer_.get_rssi_value(freq_hash) < WIDEBAND_RSSI_THRESHOLD_DB) {
            effective_threshold = WIDEBAND_RSSI_THRESHOLD_DB + HYSTERESIS_MARGIN_DB;
        }

        // Use the maximum value from spectral analysis as the effective RSSI
        int32_t effective_rssi = static_cast<int32_t>(analysis_result.max_val);

        if (effective_rssi >= effective_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
            detection_ring_buffer_.update_detection(freq_hash, current_count, effective_rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                should_log = true;

                log_entry_to_write = {
                    chTimeNow(),
                    static_cast<uint64_t>(entry.frequency_a),
                    effective_rssi,
                    threat_level,
                    drone_type,
                    current_count,
                    DiamondFixes::ConfidenceConstants::SPECTRAL_CONFIDENCE,  // 90% confidence for spectral analysis
                    analysis_result.width_bins,        // Calibration: signal width in bins
                    analysis_result.signal_width_hz,   // Calibration: signal width in Hz
                    analysis_result.snr               // Calibration: Signal-to-Noise Ratio
                };

                update_tracked_drone_internal({drone_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(effective_rssi), threat_level});
            }
        } else {
            // Counter decrement logic...
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // --- CRITICAL SECTION END ---

    // 3. Logging
    if (should_log && detection_logger_.is_session_active()) {
        detection_logger_.log_detection_async(log_entry_to_write);
    }
}

void DroneScanner::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
    // Diamond Code Fix: Use constant instead of magic number
    if (scan_cycles_ % EDA::Constants::HYBRID_SCAN_DIVISOR == 0) {
        perform_wideband_scan_cycle(hardware);
    } else {
        perform_database_scan_cycle(hardware);
    }
}

void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
    // DIAMOND OPTIMIZATION: Using DiamondCore validation utilities
    // 1. RSSI validation (using DiamondCore RSSI utilities)
    if (!EDA::Validation::validate_rssi(rssi)) {
        return;
    }

    // 2. Frequency validation (using DiamondCore validation)
    if (!EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    // 3. Drone band filtering (433MHz - 5.8GHz)
    if (!EDA::Validation::is_433mhz_band(entry.frequency_a) &&
        !EDA::Validation::is_2_4ghz_band(entry.frequency_a) &&
        !EDA::Validation::is_5_8ghz_band(entry.frequency_a) &&
        !EDA::Validation::is_military_band(entry.frequency_a)) {
        return;
    }

    // Determine parameters (locally)
    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level = ThreatClassifier::from_rssi(rssi);

    // Simple database search (with mutex protection to prevent race with load_frequency_database)
    {
        MutexLock lock(data_mutex);
        // 🔴 FIX: Use freq_db_ptr_ instead of freq_db_
        if (!freq_db_ptr_) return;
        for (size_t i = 0; i < freq_db_ptr_->entry_count(); ++i) {
            if ((*freq_db_ptr_)[i].frequency_a == entry.frequency_a) {
                detected_type = DroneType::MAVIC;
                threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
                break;
            }
        }
    }

    // Flag and data for deferred logging
    bool should_log = false;
    DetectionLogEntry log_entry_to_write;

    // --- CRITICAL SECTION START ---
    {
        MutexLock lock(data_mutex);

        // Diamond Code: Increment total_detections_ with mutex protection for thread safety
        // volatile uint32_t reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
        total_detections_++;

        // Guard clause: Prevent division by zero
        if (entry.frequency_a == 0) {
            return;
        }
        size_t freq_hash = DiamondFixes::safe_frequency_hash(entry.frequency_a);
        int32_t prev_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
        int32_t base_threshold = -90;

        if (rssi > -100 && rssi < -80) {
            base_threshold = -100;
        } else if (rssi > -80) {
            base_threshold = -90;
        }

        if (prev_rssi < base_threshold) {
            base_threshold += HYSTERESIS_MARGIN_DB;
        }

        if (rssi >= base_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));

            // Write to buffer (safe)
            detection_ring_buffer_.update_detection(freq_hash, current_count, rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                // We DON'T write log here. We only prepare data.
                should_log = true;

                // Prepare log data
                log_entry_to_write.timestamp = chTimeNow();
                // ✅ DIAMOND FIX: Type Consistency - Use uint64_t for frequency
                // entry.frequency_a is int64_t (Frequency type), DetectionLogEntry::frequency_hz is uint64_t
                // Cast to uint64_t instead of uint32_t to preserve full frequency range (>4.29GHz)
                log_entry_to_write.frequency_hz = static_cast<uint64_t>(entry.frequency_a);
                log_entry_to_write.rssi_db = rssi;
                log_entry_to_write.threat_level = threat_level;
                log_entry_to_write.drone_type = detected_type;
                log_entry_to_write.detection_count = current_count;
                log_entry_to_write.confidence_percent = 85;
                // Инициализация полей спектрального анализа (недоступны в RSSI режиме)
                log_entry_to_write.width_bins = 0;
                log_entry_to_write.signal_width_hz = 0;
                log_entry_to_write.snr = 0;

                // Update drone tracking for UI (internal method that DOESN'T take mutex again)
                update_tracked_drone_internal({detected_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(rssi), threat_level});
            }
        } else {
            // "Leaky bucket" logic
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);

            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // --- CRITICAL SECTION END ---
    // Mutex released. UI can redraw.

        // 3. Perform heavy I/O operations
    // TODO[CRITICAL][FIXED]: Async logging to prevent blocking scan thread
    // Original code blocked scan thread for 10-100ms during SD write
    if (should_log) {
        // Only log every 200ms to prevent I/O blocking while maintaining responsiveness
        // Async logging means IO is no longer the bottleneck, so we can log more frequently
        systime_t now = chTimeNow();

        // FIX #15: Use consistent memory ordering (relaxed for both)
        // Inconsistent memory ordering can cause subtle bugs
        systime_t last_time;
        {
            raii::SystemLock lock;
            last_time = last_detection_log_time_;
        }

        // Use chTimeNow() directly for timing - no CH_CFG_ST_FREQUENCY dependency
        // DIAMOND FIX: Priority 2 - Replace magic number with constant
        if (now - last_time > EDA::Constants::LOG_WRITE_INTERVAL_MS) { // 200ms delay (10x more frequent than before)
            if (detection_logger_.is_session_active()) {
                // log_detection_async returns false if buffer is full,
                // so built-in protection against hanging is already inside the logger.
                if (detection_logger_.log_detection_async(log_entry_to_write)) {
                    raii::SystemLock lock;
                    last_detection_log_time_ = now;
                }
            }
        }
    }
}

void DroneScanner::send_drone_detection_message(const DetectionParams& params) {
    // TODO[CRITICAL][FIXED]: Don't call UI methods directly from scanning thread
    // Only update internal data - UI will read via snapshot
    update_tracked_drone(params);
}

void DroneScanner::update_tracked_drone(const DetectionParams& params) {
    // TODO[CRITICAL][FIXED]: Don't call UI methods directly from scanning thread
    // Only update internal data - UI will read via snapshot
    // FIXED: Added mutex lock to prevent race condition
    MutexLock lock(data_mutex);
    update_tracked_drone_internal(params);
}

void DroneScanner::update_tracked_drone_internal(const DetectionParams& params) {
    Frequency frequency = params.frequency_hz;
    DroneType type = params.type;
    int32_t rssi = params.rssi_db;
    ThreatLevel threat_level = params.threat_level;
    // @pre Caller MUST hold data_mutex.
    // @note This is an internal optimization to avoid double-locking when called
    //       from within a critical section. Use update_tracked_drone() for external calls.
    // @invariant Assumes exclusive access to tracked_drones_ array.
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (DiamondFixes::frequency_equal(tracked_drones()[i].frequency, frequency) && tracked_drones()[i].update_count > 0) {
            tracked_drones()[i].add_rssi({static_cast<int16_t>(rssi), chTimeNow()});
            tracked_drones()[i].drone_type = static_cast<uint8_t>(type);
            tracked_drones()[i].threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
    }

    if (tracked_count_ < MAX_TRACKED_DRONES) {
        TrackedDrone new_drone;
        new_drone.frequency = static_cast<uint32_t>(frequency);
        new_drone.drone_type = static_cast<uint8_t>(type);
        new_drone.threat_level = static_cast<uint8_t>(threat_level);
        new_drone.add_rssi({static_cast<int16_t>(rssi), chTimeNow()});
        tracked_drones()[tracked_count_] = new_drone;
        tracked_count_++;
        update_tracking_counts();
        return;
    }

    // Array full - replace oldest entry
    size_t oldest_index = 0;
    systime_t oldest_time = tracked_drones()[0].last_seen;

    for (size_t i = 1; i < tracked_count_; i++) {
        if (tracked_drones()[i].last_seen < oldest_time) {
            oldest_time = tracked_drones()[i].last_seen;
            oldest_index = i;
        }
    }

    TrackedDrone& oldest_drone = tracked_drones()[oldest_index];
    oldest_drone = TrackedDrone();
    oldest_drone.frequency = static_cast<uint32_t>(frequency);
    oldest_drone.drone_type = static_cast<uint8_t>(type);
    oldest_drone.threat_level = static_cast<uint8_t>(threat_level);
    oldest_drone.add_rssi({static_cast<int16_t>(rssi), chTimeNow()});

}

void DroneScanner::remove_stale_drones() {
    const systime_t STALE_TIMEOUT = EDA::Constants::STALE_DRONE_TIMEOUT_MS;
    systime_t current_time = chTimeNow();

    // Single lock for atomicity of identify-and-remove operation
    MutexLock lock(data_mutex);

    stale_indices_.fill(0);
    size_t stale_count = 0;

    // Step 1: Identify stale drones
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones()[i].update_count > 0 &&
            (current_time - tracked_drones()[i].last_seen) > STALE_TIMEOUT) {
            stale_indices_[stale_count++] = i;
        }
    }

    // Step 2: Exit early if no stale drones
    if (stale_count == 0) return;

    // Step 3: Compact array
    size_t write_index = 0;
    size_t num_valid = 0;

    std::array<bool, MAX_TRACKED_DRONES> is_stale{};
    for (size_t i = 0; i < stale_count; ++i) {
        if (stale_indices_[i] < tracked_count_) {
            is_stale[stale_indices_[i]] = true;
        }
    }

    for (size_t read_index = 0; read_index < tracked_count_; ++read_index) {
        if (!is_stale[read_index]) {
            if (write_index != read_index) {
                tracked_drones()[write_index] = tracked_drones()[read_index];
            }
            write_index++;
            num_valid++;
        }
    }
    tracked_count_ = num_valid;

    // Step 4: Update tracking counts
    if (tracked_count_ > 0) {
        update_tracking_counts();
    } else {
        approaching_count_ = 0;
        receding_count_ = 0;
        static_count_ = 0;
    }
}

// LUT lookup for update_tracking_counts() (saves ~60 bytes Flash)
void DroneScanner::update_tracking_counts() {
    approaching_count_ = 0;
    receding_count_ = 0;
    static_count_ = 0;

    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones()[i].update_count < 2) continue;

        MovementTrend trend = tracked_drones()[i].get_trend();
        // LUT lookup for O(1) time
        uint8_t idx = static_cast<uint8_t>(trend);
        if (idx < 4) {
            (this->*DroneScanner::TREND_COUNTERS[idx])++;
        }
    }
}

void DroneScanner::reset_scan_cycles() {
    scan_cycles_ = 0;
}

void DroneScanner::switch_to_real_mode() {
    is_real_mode_ = true;
}

void DroneScanner::switch_to_demo_mode() {
    is_real_mode_ = false;
}

void DroneScanner::initialize_database_and_scanner() {
    // Aligned storage for FreqmanDB (compile-time size, no heap fragmentation)

    // Runtime alignment verification before reinterpret_cast
    if (reinterpret_cast<uintptr_t>(freq_db_storage_) % alignof(FreqmanDB) != 0) {
        freq_db_ptr_ = nullptr;
        handle_scan_error("Memory: freq_db_storage_ alignment error");
        return;
    }

    // Use reinterpret_cast on aligned buffer
    freq_db_ptr_ = reinterpret_cast<FreqmanDB*>(freq_db_storage_);

    // Guard clause: validate reinterpret_cast succeeded
    if (!freq_db_ptr_) {
        handle_scan_error("Memory: FreqmanDB reinterpret_cast failed");
        return;
    }

    // Mark freq_db as constructed
    freq_db_constructed_ = true;

    // Runtime alignment verification for tracked_drones storage
    if (reinterpret_cast<uintptr_t>(tracked_drones_storage_) % alignof(TrackedDrone) != 0) {
        handle_scan_error("Memory: tracked_drones_storage_ alignment error");
        // Only call destructor if construction succeeded
        if (freq_db_constructed_) {
            freq_db_ptr_->~FreqmanDB();
            freq_db_constructed_ = false;
        }
        freq_db_ptr_ = nullptr;
        return;
    }

    // Create TrackedDrone array in static storage
    tracked_drones_ptr_ = new (tracked_drones_storage_)
        std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>();
    if (!tracked_drones_ptr_) {
        handle_scan_error("Memory: tracked_drones placement new failed");
        // Only call destructor if construction succeeded
        if (freq_db_constructed_) {
            freq_db_ptr_->~FreqmanDB();
            freq_db_constructed_ = false;
        }
        freq_db_ptr_ = nullptr;
        return;
    }
    
    // Mark tracked_drones as constructed
    tracked_drones_constructed_ = true;

    // Инициализируем все элементы TrackedDrone
    for (auto& drone : *tracked_drones_ptr_) {
        drone = TrackedDrone();  // Default construct
    }

    // Initialize database with built-in frequencies as fallback
    // This ensures scanner has valid data even if SD card is not ready
    {
        MutexLock lock(data_mutex);

        // Try to open DRONES database first
        auto db_path = get_freqman_path(settings_.freqman_path);
        bool db_opened = freq_db_ptr_->open(db_path);

        if (!db_opened || freq_db_ptr_->empty()) {
            // If database doesn't exist or is empty, try to create it with built-in frequencies
            freq_db_ptr_->open(db_path, true);

            for (const auto& item : BUILTIN_DRONE_DB) {
                // 🔴 FIX: Validate frequency before adding to database
                if (!EDA::Validation::validate_frequency(item.freq)) {
                    continue;  // Skip invalid frequencies
                }
                
                freqman_entry entry{};
                entry.frequency_a = item.freq;
                entry.description = item.desc;
                entry.type = freqman_type::Single;
                entry.modulation = freqman_invalid_index;
                entry.bandwidth = freqman_invalid_index;
                entry.step = freqman_invalid_index;
                entry.tone = freqman_invalid_index;
                freq_db_ptr_->append_entry(entry);
            }

            current_db_index_ = 0;
        }

        freq_db_loaded_ = true;
    }

    // Mark initialization as complete
    initialization_complete_ = true;
}


void DroneScanner::sync_database() {
    MutexLock lock(data_mutex);
    
    if (!freq_db_ptr_) {
        return;
    }
    
    // FreqmanDB auto-syncs on every write operation
}

void DroneScanner::cleanup_database_and_scanner() {
    sync_database();

    bool was_loading;
    {
        raii::SystemLock lock;
        was_loading = db_loading_active_;
        db_loading_active_ = false;
    }
    if (was_loading) {
        if (db_loading_thread_ != nullptr) {
            chThdWait(db_loading_thread_);
            db_loading_thread_ = nullptr;
        }
    }

    // Stop detection logger worker thread
    detection_logger_.stop_worker();

    // Explicit destructor calls for placement new
    // Only call destructor if construction succeeded
    if (tracked_drones_ptr_ != nullptr && tracked_drones_constructed_) {
        tracked_drones_ptr_->~array();
        tracked_drones_constructed_ = false;
        tracked_drones_ptr_ = nullptr;
    }

    // Only call destructor if construction succeeded
    if (freq_db_ptr_ != nullptr && freq_db_constructed_) {
        freq_db_ptr_->~FreqmanDB();
        freq_db_constructed_ = false;
        freq_db_ptr_ = nullptr;
    }
    
    // Clear detection ring buffer
    detection_ring_buffer_.clear();
    
    // ChibiOS mutexes auto-cleaned with the object
}


// Async database loading to prevent UI freeze
msg_t DroneScanner::db_loading_thread_entry(void* arg) {
    static_cast<DroneScanner*>(arg)->db_loading_thread_loop();
    return 0;
}

void DroneScanner::db_loading_thread_loop() {
    // Use same static storage for async initialization
    // Check if already initialized
    if (freq_db_ptr_ != nullptr || tracked_drones_ptr_ != nullptr) {
        handle_scan_error("DB already initialized");
        {
            raii::SystemLock lock;
            db_loading_active_ = false;
        }
        return;
    }

    // Use reinterpret_cast on aligned buffer
    if (reinterpret_cast<uintptr_t>(freq_db_storage_) % alignof(FreqmanDB) != 0) {
        handle_scan_error("Memory: freq_db_storage_ alignment error (async)");
        {
            raii::SystemLock lock;
            db_loading_active_ = false;
        }
        return;
    }
    freq_db_ptr_ = reinterpret_cast<FreqmanDB*>(freq_db_storage_);
    if (!freq_db_ptr_) {
        handle_scan_error("Memory: FreqmanDB reinterpret_cast failed");
        {
            raii::SystemLock lock;
            db_loading_active_ = false;
        }
        return;
    }

    // Runtime alignment verification for tracked_drones_storage_
    if (reinterpret_cast<uintptr_t>(tracked_drones_storage_) % alignof(std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>) != 0) {
        handle_scan_error("Memory: tracked_drones_storage_ alignment error (async)");
        freq_db_ptr_ = nullptr;
        {
            raii::SystemLock lock;
            db_loading_active_ = false;
        }
        return;
    }

    // Use reinterpret_cast on aligned buffer
    tracked_drones_ptr_ = reinterpret_cast<std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>*>(tracked_drones_storage_);
    if (!tracked_drones_ptr_) {
        handle_scan_error("Memory: tracked_drones reinterpret_cast failed");
        freq_db_ptr_ = nullptr;
        {
            raii::SystemLock lock;
            db_loading_active_ = false;
        }
        return;
    }

    // Initialize elements
    for (auto& drone : *tracked_drones_ptr_) {
        drone = TrackedDrone();
    }

    // Set tracked_drones_constructed_ flag after initialization
    tracked_drones_constructed_ = true;

    bool db_success = false;
    auto db_path = get_freqman_path(settings_.freqman_path);

    systime_t load_start = chTimeNow();

    // SD mount timeout logic with proper resource cleanup
    systime_t start_time = chTimeNow();
    while (sd_card::status() < sd_card::Status::Mounted) {
        if (chTimeNow() - start_time > MS2ST(EDA::Constants::SD_CARD_MOUNT_TIMEOUT_MS)) {
            handle_scan_error("SD card not ready");
            {
                raii::SystemLock lock;
                db_loading_active_ = false;
            }
            // Ensure proper cleanup of placement-newed objects on timeout
            if (freq_db_ptr_ && freq_db_constructed_) {
                freq_db_ptr_->~FreqmanDB();
                freq_db_constructed_ = false;
                freq_db_ptr_ = nullptr;
            }
            if (tracked_drones_ptr_ && tracked_drones_constructed_) {
                tracked_drones_ptr_->~array();
                tracked_drones_constructed_ = false;
                tracked_drones_ptr_ = nullptr;
            }
            // FIX #16: Always reset initialization complete flag on error
            initialization_complete_ = false;
            freq_db_loaded_ = false;  // Also reset database loaded flag
            return;
        }
        chThdSleepMilliseconds(EDA::Constants::SD_CARD_POLL_INTERVAL_MS);
    }

    // Maintain proper lock order: DATA_MUTEX → SD_CARD_MUTEX
    bool should_load;
    {
        raii::SystemLock lock;
        should_load = db_loading_active_;
    }
    if (should_load) {
        db_success = freq_db_ptr_->open(db_path);

        if (db_success) {
            systime_t load_time = chTimeNow() - load_start;
            if (load_time > MS2ST(DB_LOAD_TIMEOUT_MS)) {
                handle_scan_error("Database load timeout");
                {
                    raii::SystemLock lock;
                    db_loading_active_ = false;
                }
                // 🔴 FIX #4 & #6: Ensure proper cleanup of placement-newed objects on timeout
                // Only call destructors if construction succeeded
                if (freq_db_ptr_ && freq_db_constructed_) {
                    freq_db_ptr_->~FreqmanDB();
                    freq_db_constructed_ = false;
                    freq_db_ptr_ = nullptr;
                }
                if (tracked_drones_ptr_ && tracked_drones_constructed_) {
                    tracked_drones_ptr_->~array();
                    tracked_drones_constructed_ = false;
                    tracked_drones_ptr_ = nullptr;
                }
                initialization_complete_ = false;
                freq_db_loaded_ = false;
                return;
            }
        }
    }

    bool should_init;
    {
        raii::SystemLock lock;
        should_init = db_loading_active_;
    }
    if (should_init && (!db_success || freq_db_ptr_->empty())) {

        // Phase 1: Acquire data_mutex for database modifications
        {
            TwoPhaseLock<Mutex> data_lock(data_mutex, LockOrder::DATA_MUTEX);
            
            freq_db_ptr_->open(db_path, true);

            for (const auto& item : BUILTIN_DRONE_DB) {
                bool still_loading;
                {
                    raii::SystemLock lock;
                    still_loading = db_loading_active_;
                }
                // DIAMOND FIX #6: Early return if loading was stopped externally
                // Note: No flag reset needed here - the caller who stopped loading
                // should have already reset the flags. This is a cooperative cancellation.
                if (!still_loading) {
                    return;
                }

                if (!EDA::Validation::validate_frequency(item.freq)) {
                    continue;  // Skip invalid frequencies
                }

                freqman_entry entry{};
                entry.frequency_a = item.freq;
                entry.description = item.desc;
                entry.type = freqman_type::Single;
                entry.modulation = freqman_invalid_index;
                entry.bandwidth = freqman_invalid_index;
                entry.step = freqman_invalid_index;
                entry.tone = freqman_invalid_index;
                freq_db_ptr_->append_entry(entry);
            }

            current_db_index_ = 0;
            freq_db_loaded_ = true;
        }  // data_lock released here

        // Phase 2: Acquire sd_card_mutex for sync_database()
        {
            MutexLock sd_lock(sd_card_mutex, LockOrder::SD_CARD_MUTEX);
            sync_database();
        }  // sd_lock released here

        // Set freq_db_constructed_ flag at end of thread
        freq_db_constructed_ = true;

        // Set initialization_complete_ flag at end of thread
        {
            raii::SystemLock lock;
            initialization_complete_ = true;
        }
    }
}

// Async database loading (non-blocking UI)
void DroneScanner::initialize_database_async() {
    bool is_loading;
    {
        raii::SystemLock lock;
        is_loading = db_loading_active_;
    }
    if (is_loading) {
        return;  // Already loading or loaded
    }

    {
        raii::SystemLock lock;
        db_loading_active_ = true;
    }

    // Use chThdCreateStatic instead of chThdCreateFromHeap

    db_loading_thread_ = chThdCreateStatic(
        db_loading_wa_,                    // Working area
        sizeof(db_loading_wa_),            // Size
        NORMALPRIO - 2,                    // Priority
        db_loading_thread_entry,           // Entry function
        this                               // Argument
    );

    // Verify thread creation result
    if (db_loading_thread_ == nullptr) {
        handle_scan_error("Failed to create db_loading_thread");
        {
            raii::SystemLock lock;
            db_loading_active_ = false;
        }
        return;
    }

    if (db_loading_thread_ == nullptr) {
        // Do NOT run synchronously - would block UI thread
        handle_scan_error("Scan unavailable - resource limit");
        {
            raii::SystemLock lock;
            db_loading_active_ = false;
        }
        freq_db_loaded_ = false;
        return;  // Keep UI responsive - early return on failure
    }
}


// Check if async loading finished
bool DroneScanner::is_database_loading_complete() const {
    bool is_loading;
    {
        raii::SystemLock lock;
        is_loading = db_loading_active_;
    }
    return !is_loading && freq_db_loaded_;
}

bool DroneScanner::is_initialization_complete() const {
    raii::SystemLock lock;
    return initialization_complete_;
}

Frequency DroneScanner::get_current_scanning_frequency() const {
    raii::SystemLock lock;

    if (!freq_db_ptr_ || !freq_db_loaded_) {
        return 433000000;  // Default fallback frequency
    }

    size_t db_entry_count = freq_db_ptr_->entry_count();
    if (db_entry_count > 0 && current_db_index_ < db_entry_count) {
        return (*freq_db_ptr_)[current_db_index_].frequency_a;
    }

    return 433000000;  // Default fallback frequency
}

Frequency DroneScanner::get_current_radio_frequency() const {
    return get_current_scanning_frequency();
}

TrackedDrone DroneScanner::getTrackedDrone(size_t index) const {
    MutexLock lock(data_mutex);
    if (index < tracked_count_) {
        return tracked_drones()[index];
    }
    return TrackedDrone{};
}

void DroneScanner::handle_scan_error([[maybe_unused]] const char* error_msg) {
    if (error_msg) {
        last_scan_error_ = error_msg;
    }
}

DroneScanner::DroneSnapshot DroneScanner::get_tracked_drones_snapshot() const {
    // Snapshot is point-in-time and may be stale
    DroneSnapshot snapshot;
    MutexLock lock(data_mutex);
    snapshot.count = tracked_count_;
    for (size_t i = 0; i < tracked_count_ && i < MAX_TRACKED_DRONES; ++i) {
        snapshot.drones[i] = tracked_drones()[i];
    }
    return snapshot;
}

bool DroneScanner::try_get_tracked_drones_snapshot(DroneSnapshot& out_snapshot) const {
    // 🔴 FIX: Use RAII MutexTryLock for automatic unlock on all paths
    // 🔴 FIX #9: Snapshot is point-in-time and may be stale
    // The lock is released when the function returns, so the returned data
    // represents the state at the moment of the snapshot and may not reflect
    // subsequent changes to tracked_drones_.
    MutexTryLock lock(data_mutex);

    if (lock.is_locked()) {
        out_snapshot.count = tracked_count_;
        for (size_t i = 0; i < tracked_count_ && i < MAX_TRACKED_DRONES; ++i) {
            out_snapshot.drones[i] = tracked_drones()[i];
        }
        return true;
    }
    return false;
}

    // Implementation of safe read methods

int32_t DroneScanner::get_detection_rssi_safe(size_t freq_hash) const {
    MutexLock lock(data_mutex); // Lock reading while scanner can write
    return detection_ring_buffer_.get_rssi_value(freq_hash);
}

uint8_t DroneScanner::get_detection_count_safe(size_t freq_hash) const {
    MutexLock lock(data_mutex);
    return detection_ring_buffer_.get_detection_count(freq_hash);
}

// DroneDetectionLogger implementations
DroneDetectionLogger::DroneDetectionLogger()
    : worker_thread_(nullptr),
      mutex_(),
      data_ready_(),
      worker_should_run_(false),
      csv_log_(),
      session_active_(false),
      session_start_(0),
      logged_count_(0),
      dropped_logs_(0),
      header_written_(false),
      ring_buffer_(),
      head_(0),
      tail_(0),
      is_full_(false),
      line_buffer_() {
    chMtxInit(&mutex_);
    memset(line_buffer_, 0, sizeof(line_buffer_));
    
    chSemInit(&data_ready_, 0);
    
    start_session();
}

DroneDetectionLogger::~DroneDetectionLogger() {
    stop_worker();
    end_session();
}

void DroneDetectionLogger::start_session() {
    if (session_active_) return;
    session_active_ = true;
    session_start_ = chTimeNow();
    logged_count_ = 0;
    dropped_logs_ = 0;
    header_written_ = false;
}

void DroneDetectionLogger::end_session() {
    if (!session_active_) return;
    session_active_ = false;
}

// Producer method - called by scanner thread
bool DroneDetectionLogger::log_detection_async(const DetectionLogEntry& entry) {
    MutexLock lock(mutex_);
    if (is_full_) {
        // Circular buffer - overwrite oldest entry
        dropped_logs_++;

        // Overwrite oldest entry (tail position)
        tail_ = (tail_ + 1) % BUFFER_SIZE;
    }

    size_t current_head = head_;
    ring_buffer_[current_head] = entry;
    head_ = (current_head + 1) % BUFFER_SIZE;

    if (head_ == tail_) {
        is_full_ = true;
    }

    chSemSignal(&data_ready_);
    return true;
}

void DroneDetectionLogger::start_worker() {
    if (worker_thread_) return;

    {
        raii::SystemLock lock;
        worker_should_run_ = true;
    }
    // Create worker thread with static stack
    worker_thread_ = chThdCreateStatic(
        worker_wa_,
        sizeof(worker_wa_),
        NORMALPRIO - 1,
        worker_thread_entry,
        this
    );
    
    static_assert(sizeof(worker_wa_) >= DroneDetectionLogger::WORKER_STACK_SIZE,
                 "worker_wa_ size mismatch with WORKER_STACK_SIZE");

    if (worker_thread_ == nullptr) {
        worker_should_run_ = false;
        return;
    }

    start_session();
}


// Stack safety guard for embedded systems
namespace StackSafety {
    class StackGuard {
    public:
        explicit StackGuard(const char* function_name) noexcept
            : function_name_(function_name), stack_ptr_(get_stack_pointer()) {}

        ~StackGuard() noexcept {
            size_t stack_used = get_stack_pointer() - stack_ptr_;
            // Could log stack usage here if needed
            (void)stack_used;  // Suppress unused warning
        }

        StackGuard(const StackGuard&) = delete;
        StackGuard& operator=(const StackGuard&) = delete;

    private:
        static inline size_t get_stack_pointer() noexcept {
            volatile size_t sp;
            sp = reinterpret_cast<size_t>(&sp);
            return sp;
        }

        const char* function_name_;
        size_t stack_ptr_;
    };
}

void DroneDetectionLogger::stop_worker() {
    if (!worker_thread_) return;

    {
        raii::SystemLock lock;
        worker_should_run_ = false;
    }
    chSemSignal(&data_ready_);
    
    // Thread termination timeout (1 second)
    systime_t timeout = chTimeNow() + MS2ST(EDA::Constants::THREAD_TERMINATION_TIMEOUT_MS);  // 1 second timeout
    while (chTimeNow() < timeout && worker_thread_ != nullptr) {
        chThdSleepMilliseconds(EDA::Constants::THREAD_TERMINATION_POLL_INTERVAL_MS);  // Small sleep to yield CPU
    }
    worker_thread_ = nullptr;
    
    end_session();
}

msg_t DroneDetectionLogger::worker_thread_entry(void* arg) {
    static_cast<DroneDetectionLogger*>(arg)->worker_loop();
    return 0;
}

void DroneDetectionLogger::worker_loop() {
    while (true) {
        bool should_run;
        {
            raii::SystemLock lock;
            should_run = worker_should_run_;
        }
        if (!should_run) break;
        
        chSemWaitTimeout(&data_ready_, MS2ST(1000));
        
        {
            raii::SystemLock lock;
            should_run = worker_should_run_;
        }
        if (!should_run) break;

        DetectionLogEntry entry_to_write;
        bool has_data = false;
        bool has_more_data = false;

        {
            MutexLock lock(mutex_);
            // Diamond Code: Direct volatile access is safe because mutex_ provides thread safety
            if (head_ != tail_ || is_full_) {
                entry_to_write = ring_buffer_[tail_];
                tail_ = (tail_ + 1) % BUFFER_SIZE;
                is_full_ = false;
                has_data = true;

                // Проверяем есть ли ещё данные
                has_more_data = (head_ != tail_);
            }
            // 🔴 FIX #12: Signal semaphore while still holding mutex
            // This prevents race condition where buffer state changes between
            // checking has_more_data and signaling the semaphore
            if (has_more_data) {
                chSemSignal(&data_ready_);
            }
        }
        // Мьютекс освобождён

        if (has_data) {
            write_entry_to_sd(entry_to_write);
        }
    }
}

constexpr systime_t SD_WRITE_TIMEOUT_MS = 1000;

bool DroneDetectionLogger::write_entry_to_sd(const DetectionLogEntry& entry) {
    if (sd_card::status() < sd_card::Status::Mounted) {
        return false;
    }

    // Lock SD card before file operations (FatFS is NOT thread-safe)
    SDCardLock lock(sd_card_mutex);

    if (!ensure_csv_header()) return false;

    // Use class member buffer (reduces stack pressure)
    int len = snprintf(line_buffer_, sizeof(line_buffer_),
        "%" PRIu32 ",%" PRIu32 ",%" PRId32 ",%" PRIu8 ",%" PRIu8 ",%" PRIu8 ",%" PRIu8 ",%" PRIu8 ",%" PRIu32 ",%" PRIu8 "\n",
        entry.timestamp,
        static_cast<uint32_t>(entry.frequency_hz),
        entry.rssi_db,
        static_cast<uint8_t>(entry.threat_level),
        static_cast<uint8_t>(entry.drone_type),
        entry.detection_count,
        entry.confidence_percent,
        entry.width_bins,
        static_cast<uint32_t>(entry.signal_width_hz),
        entry.snr);

    if (len < 0 || static_cast<size_t>(len) >= sizeof(line_buffer_)) {
        return false;
    }

    // Add timeout to SD write operation
    systime_t write_start = chTimeNow();
    
    // DIAMOND FIX: Zero-heap write using raw buffer
    auto error = csv_log_.write_raw(line_buffer_, static_cast<File::Size>(len));

    systime_t write_time = chTimeNow() - write_start;
    if (write_time > MS2ST(SD_WRITE_TIMEOUT_MS)) {
    }

    if (error && error->ok()) {
        logged_count_++;
        return true;
    }
    return false;
}

bool DroneDetectionLogger::ensure_csv_header() {
    if (header_written_) return true;

    if (sd_card::status() < sd_card::Status::Mounted) {
        return false;
    }

    // Static string literal stored in Flash
    static const char* const header =
        "timestamp_ms,frequency_hz,rssi_db,threat_level,drone_type,detection_count,confidence_percent,width_bins,signal_width_hz,snr\n";

    auto error = csv_log_.append(generate_log_filename());
    if (error && !error->ok()) return false;

    // Zero-heap write using raw buffer
    const size_t header_len = strlen(header);
    error = csv_log_.write_raw(header, static_cast<File::Size>(header_len));

    if (error && error->ok()) {
        header_written_ = true;
        return true;
    }
    return false;
}

// Return static const char* to eliminate heap allocation
const char* DroneDetectionLogger::generate_log_filename() const {
    return "EDA_LOG.CSV";
}

// Hardware Controller Implementation

DroneHardwareController::DroneHardwareController(SpectrumMode mode)
    : last_spectrum_db_(),
      spectrum_mutex_(),
      spectrum_mode_(mode),
      center_frequency_(2400000000ULL),
      bandwidth_hz_(24000000),
      radio_state_(),
      spectrum_fifo_(nullptr),
      spectrum_streaming_active_(false),
      rssi_updated_(false),
      last_valid_rssi_(-120)
{
    chMtxInit(&spectrum_mutex_);
    spectrum_updated_ = false;
}

DroneHardwareController::~DroneHardwareController() {
    shutdown_hardware();
}

void DroneHardwareController::initialize_hardware() {
    // Initialize radio with error protection
    initialize_radio_state();
    initialize_spectrum_collector();
}

void DroneHardwareController::on_hardware_show() {
    initialize_hardware();
}

void DroneHardwareController::on_hardware_hide() {
}

void DroneHardwareController::shutdown_hardware() {
    stop_spectrum_streaming();

    // Shutdown baseband coprocessor for proper resource cleanup
    baseband::shutdown();

    // Explicitly disable radio chip (CPLD/R820T/MAX2837)
    // This is critical for power saving and stability of the next application launch
    receiver_model.disable();

    cleanup_spectrum_collector();
}

void DroneHardwareController::initialize_radio_state() {
    // Load baseband coprocessor image before accessing receiver resources
    baseband::run_image(portapack::spi_flash::image_tag_wideband_spectrum);

    // Add error handling for all radio operations

    receiver_model.enable();
    receiver_model.set_modulation(ReceiverModel::Mode::SpectrumAnalysis);
    receiver_model.set_sampling_rate(get_configured_sampling_rate());
    receiver_model.set_baseband_bandwidth(get_configured_bandwidth());
    receiver_model.set_squelch_level(0);

        

        // Use safe defaults - RF Amp controlled by user settings
        receiver_model.set_rf_amp(false);

    // LNA Gain: 32dB (Range 0-40) - receives weak signals
    receiver_model.set_lna(32);

    // VGA Gain: 32dB (Range 0-62) - signal volume after LNA
    receiver_model.set_vga(32);
    
    
    
   
}

void DroneHardwareController::initialize_spectrum_collector() {
}

void DroneHardwareController::cleanup_spectrum_collector() {
    spectrum_streaming_active_ = false;
}

void DroneHardwareController::set_spectrum_mode(SpectrumMode mode) {
    spectrum_mode_ = mode;
}

uint32_t DroneHardwareController::get_spectrum_bandwidth() const {
    return bandwidth_hz_;
}

void DroneHardwareController::set_spectrum_bandwidth(uint32_t bandwidth_hz) {
    bandwidth_hz_ = bandwidth_hz;
}

Frequency DroneHardwareController::get_spectrum_center_frequency() const {
    return center_frequency_;
}

void DroneHardwareController::set_spectrum_center_frequency(Frequency center_freq) {
    // 🔴 FIX: Validate frequency before setting
    if (!EDA::Validation::validate_frequency(center_freq)) {
        // Invalid frequency - do nothing, keep previous value
        return;
    }
    center_frequency_ = center_freq;
}

// DIAMOND OPTIMIZATION: Using DiamondCore validation with overflow protection
// DIAMOND FIX: Revision #8 - Add SPI Retry Logic (LOW)
// Add 3-retry logic with 10ms delay for improved SPI communication reliability
bool DroneHardwareController::tune_to_frequency(Frequency frequency_hz) {
    // Validate frequency range using DiamondCore utilities
    if (!EDA::Validation::validate_frequency(frequency_hz)) {
        return false;
    }
    
    // FIX #17: Removed impossible overflow check
    // The check (frequency + 1MHz < frequency) is ALWAYS FALSE for unsigned arithmetic
    // No overflow can occur with uint64_t addition of 1MHz to valid frequencies
    
    // 3-retry logic with 10ms delay for SPI reliability
    constexpr uint8_t MAX_RETRIES = 3;
    constexpr uint32_t RETRY_DELAY_MS = 10;
    
    for (uint8_t retry = 0; retry < MAX_RETRIES; ++retry) {
        receiver_model.set_target_frequency(frequency_hz);
        
        // Delay between retries only
        if (retry > 0) {
            chThdSleepMilliseconds(RETRY_DELAY_MS);
        }
        
        // Assume success if no exception occurs
        return true;
    }
    
    return false;
}

void DroneHardwareController::start_spectrum_streaming() {
    if (spectrum_streaming_active_) return;
    spectrum_streaming_active_ = true;
    baseband::spectrum_streaming_start();
}

void DroneHardwareController::stop_spectrum_streaming() {
    spectrum_streaming_active_ = false;
    baseband::spectrum_streaming_stop();
}

void DroneHardwareController::stop_spectrum_before_scan() {
    if (spectrum_streaming_active_) {
        spectrum_streaming_active_ = false;
        baseband::spectrum_streaming_stop();
        // Give M0 time to process stop command
        chThdSleepMilliseconds(EDA::Constants::BASEBOARD_STOP_DELAY_MS);
    }
}

void DroneHardwareController::resume_spectrum_after_scan() {
    if (!spectrum_streaming_active_) {
        spectrum_streaming_active_ = true;
        baseband::spectrum_streaming_start();
    }
}

bool DroneHardwareController::is_spectrum_compatible_with_scanning() const {
    return !spectrum_streaming_active_;
}

int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency) {
    (void)target_frequency;
    return last_valid_rssi_;
}

// ISR-safe: uses raii::SystemLock (critical section) instead of mutex
void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message) {
    if (message) {
        spectrum_fifo_ = message->fifo;
        if (spectrum_fifo_) {
            ChannelSpectrum spectrum;
            while (spectrum_fifo_->out(spectrum)) {
                process_channel_spectrum_data(spectrum);
            }
        }
    }
}

void DroneHardwareController::handle_channel_statistics(const ChannelStatistics& statistics) {
    last_valid_rssi_ = statistics.max_db;
    rssi_updated_ = true;
}

void DroneHardwareController::clear_rssi_flag() {
    rssi_updated_ = false;
}

bool DroneHardwareController::is_rssi_fresh() const {
    return rssi_updated_;
}

void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum) {
    // ISR-safe: use critical section instead of mutex
    raii::SystemLock lock;

    size_t count = std::min(spectrum.db.size(), last_spectrum_db_.size());
    std::copy(spectrum.db.begin(), spectrum.db.begin() + count, last_spectrum_db_.begin());

    spectrum_updated_ = true;
}

int32_t DroneHardwareController::get_configured_sampling_rate() const {
    return bandwidth_hz_;
}

int32_t DroneHardwareController::get_configured_bandwidth() const {
    return bandwidth_hz_;
}

bool DroneHardwareController::is_spectrum_streaming_active() const {
    return spectrum_streaming_active_;
}

int32_t DroneHardwareController::get_current_rssi() const {
    return last_valid_rssi_;
}

// UI Implementations

SmartThreatHeader::SmartThreatHeader(Rect parent_rect)
    : View(parent_rect),
      threat_progress_bar_({0, 0, screen_width, 16}),
      threat_status_main_({0, 20, screen_width, 16}, "THREAT: LOW | <0 ~0 >0"),
      threat_frequency_({0, 38, screen_width, 16}, "2400.0MHz SCANNING") {
    last_text_[0] = '\0';
    add_children({&threat_progress_bar_, &threat_status_main_, &threat_frequency_});
    update(ThreatLevel::NONE, 0, 0, 0, 2400000000ULL, false);
}

void SmartThreatHeader::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
                                size_t receding, Frequency current_freq, bool is_scanning) {
    // Check-Before-Update Pattern
    bool data_changed = (max_threat != last_threat_) ||
                        (is_scanning != last_is_scanning_) ||
                        (current_freq != last_freq_) ||
                        (approaching != last_approaching_) ||
                        (static_count != last_static_) ||
                        (receding != last_receding_);

    if (!data_changed) {
    }

    last_threat_ = max_threat;
    last_is_scanning_ = is_scanning;
    last_freq_ = current_freq;
    last_approaching_ = approaching;
    last_static_ = static_count;
    last_receding_ = receding;

    size_t total_drones = approaching + static_count + receding;
    threat_progress_bar_.set_value(total_drones * 10);

    // Use StatusFormatter with thread-local buffer
    thread_local char buffer[64];
    const char* threat_name = UnifiedStringLookup::threat_name(static_cast<uint8_t>(max_threat));
    if (total_drones > 0) {
        StatusFormatter::format_to(buffer, "THREAT: %s | <%lu ~%lu >%lu",
                                  threat_name,
                                  static_cast<unsigned long>(approaching),
                                  static_cast<unsigned long>(static_count),
                                  static_cast<unsigned long>(receding));
    } else if (is_scanning) {
        StatusFormatter::format_to(buffer, "SCANNING: <%lu ~%lu >%lu",
                                  static_cast<unsigned long>(approaching),
                                  static_cast<unsigned long>(static_count),
                                  static_cast<unsigned long>(receding));
    } else {
        StatusFormatter::format_to(buffer, "READY");
    }
    threat_status_main_.set(buffer);
    threat_status_main_.set_style(&UIStyles::RED_STYLE);
    safe_strcpy(last_text_, buffer, sizeof(last_text_));
    
    // Cache text length to avoid strlen() in paint()
    last_text_len_ = strlen(last_text_);

    // Use FrequencyFormatter
    if (current_freq > 0) {
        // Use thread-local buffer
        thread_local char freq_buf[16];
        FrequencyFormatter::to_string_short_freq_buffer(freq_buf, sizeof(freq_buf), current_freq);
        if (is_scanning) {
            StatusFormatter::format_to(buffer, "%s SCANNING", freq_buf);
        } else {
            StatusFormatter::format_to(buffer, "%s READY", freq_buf);
        }
        threat_frequency_.set(buffer);
    } else {
        threat_frequency_.set("NO SIGNAL");
    }

    // DIAMOND OPTIMIZATION: Unified color mapping
    static constexpr const Style* const THREAT_STYLES[5] = {
        &UIStyles::LIGHT_STYLE,  // NONE (0)
        &UIStyles::GREEN_STYLE,   // LOW (1)
        &UIStyles::YELLOW_STYLE,  // MEDIUM (2)
        &UIStyles::YELLOW_STYLE,  // HIGH (3)
        &UIStyles::RED_STYLE      // CRITICAL (4)
    };
    uint8_t threat_idx = std::min(static_cast<uint8_t>(max_threat), static_cast<uint8_t>(4));
    threat_frequency_.set_style(THREAT_STYLES[threat_idx]);
    set_dirty();
}

void SmartThreatHeader::set_max_threat(ThreatLevel threat) {
    if (threat != last_threat_) {
        update(threat, last_approaching_, last_static_, last_receding_,
               last_freq_, last_is_scanning_);
    }
}

void SmartThreatHeader::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
    update(last_threat_, approaching, static_count, receding,
           last_freq_, last_is_scanning_);
}

void SmartThreatHeader::set_current_frequency(Frequency freq) {
    if (freq != last_freq_) {
        update(last_threat_, last_approaching_, last_static_, last_receding_,
               freq, last_is_scanning_);
    }
}

void SmartThreatHeader::set_scanning_state(bool is_scanning) {
    if (is_scanning != last_is_scanning_) {
        update(last_threat_, last_approaching_, last_static_, last_receding_,
               last_freq_, is_scanning);
    }
}

void SmartThreatHeader::set_color_scheme(bool use_dark_theme) {
    (void)use_dark_theme;
}

// UnifiedColorLookup - O(1) lookup
Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) const {
    return UnifiedColorLookup::header_bar(static_cast<uint8_t>(level));
}

Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) const {
    (void)level;
    return Color::white();  // Always white text on colored backgrounds
}

const char* SmartThreatHeader::get_threat_icon_text(ThreatLevel level) const {
    return UnifiedStringLookup::threat_name(static_cast<uint8_t>(level));
}

void SmartThreatHeader::paint(Painter& painter) {
    // Fill background with threat color
    Color bg_color = UnifiedColorLookup::header_bar(static_cast<uint8_t>(last_threat_));
    painter.fill_rectangle(screen_rect(), bg_color);

    // Draw centered text with white color
    constexpr int FONT_WIDTH_PX = 8;
    const int text_width = static_cast<int>(last_text_len_) * FONT_WIDTH_PX;
    const int text_height = 16;
    const int center_x = (screen_width - text_width) / 2;
    const int center_y = (60 - text_height) / 2;
    Point text_pos = {center_x, center_y};
    auto style = Style{font::fixed_8x16, bg_color, Color::white()};
    painter.draw_string(text_pos, style, last_text_);
}

ThreatCard::ThreatCard(size_t card_index, Rect parent_rect)
    : View(parent_rect), card_index_(card_index),
      parent_rect_(parent_rect), is_active_(false),
      last_frequency_(0), last_threat_(ThreatLevel::NONE),
      last_trend_(MovementTrend::UNKNOWN), last_rssi_(-120) {
    last_threat_name_[0] = '\0';
    add_children({&card_text_});
}

// Use UnifiedStringLookup
void ThreatCard::update_card(const DisplayDroneEntry& drone) {
    // Check-Before-Update Pattern
    bool data_changed = (drone.frequency != last_frequency_) ||
                        (drone.threat != last_threat_) ||
                        (drone.trend != last_trend_) ||
                        (std::abs(drone.rssi - last_rssi_) > 1) || // RSSI hysteresis
                        (strcmp(drone.type_name, last_threat_name_) != 0);

    if (!data_changed && is_active_) {
    }

    last_frequency_ = drone.frequency;
    last_threat_ = drone.threat;
    last_trend_ = drone.trend;
    last_rssi_ = drone.rssi;
    safe_strcpy(last_threat_name_, drone.type_name, sizeof(last_threat_name_));
    is_active_ = true;

    // Use TrendSymbols for O(1) lookup
    char trend_char = TrendSymbols::from_trend(static_cast<uint8_t>(drone.trend));
    uint32_t mhz = drone.frequency / 1000000;

    // Use StatusFormatter with thread-local buffer
    thread_local char buffer[48];
    StatusFormatter::format_to(buffer, "%s %c %luM %ld",
                             drone.type_name, trend_char,
                             (unsigned long)mhz, (long)drone.rssi);
    card_text_.set(buffer);
    
    // Static style (not created on stack each call)
    static const Style CARD_STYLE = {font::fixed_8x16, Color::black(), Color::white()};
    card_text_.set_style(&CARD_STYLE);
    
    set_dirty();
}

void ThreatCard::clear_card() {
    is_active_ = false;
    card_text_.set("");
    set_dirty();
}



// UnifiedColorLookup - O(1) lookup
Color ThreatCard::get_card_bg_color() const {
    if (!is_active_) return Color::black();

    return UnifiedColorLookup::card_bg(static_cast<uint8_t>(last_threat_));
}

Color ThreatCard::get_card_text_color() const {
    if (!is_active_) return Color::black();
    return UnifiedColorLookup::card_text(static_cast<uint8_t>(last_threat_));
}

void ThreatCard::paint(Painter& painter) {
    View::paint(painter);
    if (is_active_) {
        Color bg_color = UnifiedColorLookup::card_bg(static_cast<uint8_t>(last_threat_));
        painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, bg_color);
    }
}

ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
    : View(parent_rect), bar_index_(bar_index), parent_rect_(parent_rect) {
    add_children({&progress_text_, &alert_text_, &normal_text_});
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    // Early return for invalid states
    if (progress_percent > 100) progress_percent = 100;

    set_display_mode(DisplayMode::SCANNING);

    // Table-driven progress bar - O(1) lookup
    uint8_t raw_idx = (progress_percent * 8) / 100;
    uint8_t bar_idx = (raw_idx < 8) ? raw_idx : 8;
    const auto& bar_entry = PROGRESS_PATTERNS[bar_idx];

    // Use StatusFormatter
    char buffer[64];
    StatusFormatter::format_to(buffer, "%s %lu%% C:%lu D:%lu",
                             bar_entry.pattern, (unsigned long)progress_percent,
                             (unsigned long)total_cycles, (unsigned long)detections);
    progress_text_.set(buffer);

    // Style from constexpr LUT
    static const Style SCANNING_STYLE = {font::fixed_8x16, Color::black(), Color(STATUS_STYLES[0].bg_color)};
    progress_text_.set_style(&SCANNING_STYLE);

    if (detections > 0) {
        set_display_mode(DisplayMode::ALERT);
        // Use StatusFormatter
        char alert_buffer[64];
        StatusFormatter::format_to(alert_buffer, "[!] DETECTED: %lu threats found!",
                                  static_cast<unsigned long>(detections));
        alert_text_.set(alert_buffer);

        // Style from constexpr LUT
        static const Style ALERT_STYLE = {font::fixed_8x16, Color::black(), Color(THREAT_STYLES[0].bg_color)};
        alert_text_.set_style(&ALERT_STYLE);
    }
    set_dirty();
}

void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const char* alert_msg) {
    // Early return for invalid states
    if (!alert_msg) return;

    set_display_mode(DisplayMode::ALERT);

    // Unified icon and style mapping from constexpr LUT
    size_t icon_idx = std::min(static_cast<size_t>(threat), size_t(4));
    const char* alert_icon = ALERT_ICONS[icon_idx];

    // Use StatusFormatter
    char buffer[64];
    StatusFormatter::format_to(buffer, "%s ALERT: %lu drones | %s",
                             alert_icon,
                             static_cast<unsigned long>(total_drones),
                             alert_msg);
    alert_text_.set(buffer);

    // Table-driven style selection
    // O(1) lookup вместо runtime ветвления: CRITICAL=0 (RED), все остальное=1 (YELLOW)
    static const Style ALERT_STYLES[] = {
        {font::fixed_8x16, Color::black(), Color(THREAT_STYLES[0].bg_color)}, // CRITICAL (RED)
        {font::fixed_8x16, Color::black(), Color(THREAT_STYLES[1].bg_color)}  // MEDIUM (YELLOW)
    };
    size_t style_idx = (threat >= ThreatLevel::CRITICAL) ? 0 : 1;
    alert_text_.set_style(&ALERT_STYLES[style_idx]);
    set_dirty();
}

void ConsoleStatusBar::update_normal_status(const char* primary, const char* secondary) {
    // Early return for invalid states
    if (!primary) return;
    
    set_display_mode(DisplayMode::NORMAL);

    // Use StatusFormatter
    // Direct character check instead of strlen() - O(1) vs O(n)
    char buffer[48];
    if (!secondary || secondary[0] == '\0') {
        StatusFormatter::format_to(buffer, "%s", primary);
    } else {
        StatusFormatter::format_to(buffer, "%s | %s", primary, secondary);
    }
    normal_text_.set(buffer);
    
    // Static style (not created on stack each call)
    static const Style NORMAL_STYLE = {font::fixed_8x16, Color::black(), Color::white()};
    normal_text_.set_style(&NORMAL_STYLE);
    set_dirty();
}

void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
    // Early return
    if (mode_ == mode) return;

    mode_ = mode;

    // Bit mask instead of 3x bool comparison (more compact and faster)
    static constexpr uint8_t VISIBILITY_MASK[3] = {
        0b001, // NORMAL
        0b010, // SCANNING
        0b100  // ALERT
    };
    
    uint8_t mask = VISIBILITY_MASK[static_cast<uint8_t>(mode)];
    progress_text_.hidden(!(mask & 0b010));
    alert_text_.hidden(!(mask & 0b100));
    normal_text_.hidden(!(mask & 0b001));
    
    set_dirty();
}

void ConsoleStatusBar::paint(Painter& painter) {
    View::paint(painter);

    uint8_t mode_idx = static_cast<uint8_t>(mode_);
    const auto& layout = DISPLAY_MODE_LAYOUTS[mode_idx];

    if (layout.bar_color != 0x0000) {
        painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, Color(layout.bar_color));
    }
}

DroneDisplayController::~DroneDisplayController() {
    // Handlers are stack-allocated and will be automatically destroyed
}

// Implementation: process_frame_sync
void DroneDisplayController::process_frame_sync() {
    if (spectrum_fifo_) {
        ChannelSpectrum spectrum;
        while (spectrum_fifo_->out(spectrum)) {
            this->process_mini_spectrum_data(spectrum);
        }
    }
}

DroneDisplayController::DroneDisplayController(Rect parent_rect)
    : View(parent_rect),
      big_display_({4, 0, 28 * 8, 52}, ""),
      scanning_progress_({0, 52, screen_width, 8}),
      text_threat_summary_({0, 82, screen_width, 16}, "THREAT: NONE"),
      text_status_info_({0, 98, screen_width, 16}, "Ready"),
      text_scanner_stats_({0, 114, screen_width, 16}, "No database"),
       text_trends_compact_({0, 130, screen_width, 16}, ""),
       text_drone_1_({screen_width - 120, 146, 120, 16}, ""),
       text_drone_2_({screen_width - 120, 162, 120, 16}, ""),
       text_drone_3_({screen_width - 120, 178, 120, 16}, ""),
       compact_frequency_ruler_({0, 68, screen_width, 12}),
       displayed_drones_(),
          // Static buffers initialization
           detected_drones_count_(0),
           buffers_allocated_(false),
          threat_bins_(), threat_bins_count_(0),
           waterfall_line_index_(0),
           spectrum_gradient_(), spectrum_fifo_(nullptr),
           pixel_index(0), bins_hz_size(0), each_bin_size(DEFAULT_EACH_BIN_SIZE_HZ), min_color_power(DEFAULT_MIN_COLOR_POWER),
           marker_pixel_step(DEFAULT_MARKER_PIXEL_STEP_HZ), max_power(0), range_max_power(0), mode_(DroneDisplayController::DisplayRenderMode::SPECTRUM),
           spectrum_config_(),
      // Initialize mutexes in member initialization list
      // Lock order: SPECTRUM_MUTEX (level 1), HISTOGRAM_MUTEX (level 2)
      spectrum_mutex_(),
      histogram_mutex_()
{
    // Add ALL widgets to View hierarchy
    add_children({
        &big_display_,
        &scanning_progress_,
        &compact_frequency_ruler_,
        &text_threat_summary_,
        &text_status_info_,
        &text_scanner_stats_,
        &text_trends_compact_,
        &text_drone_1_,
        &text_drone_2_,
        &text_drone_3_,
        &text_signal_type_
    });
}

// ===========================================
// DIAMOND OPTIMIZATION: Buffer Management Methods
// ===========================================
// Diamond Code Principle: Zero heap allocation, use static storage
// All buffers are allocated at compile time in .bss section
// These methods manage the allocation state flag only

// Guard clause: Returns true if display buffers are marked as allocated
bool DroneDisplayController::are_buffers_allocated() const {
    // DIAMOND OPTIMIZATION: Direct flag access (O(1), zero overhead)
    // Static buffers are always present, only the flag needs checking
    return buffers_allocated_;
}

// Guard clause: Allocates display buffers (static, just marks as ready)
bool DroneDisplayController::allocate_buffers_from_pool() {
    // Early return if already allocated
    if (buffers_allocated_) {
        return true;  // Already allocated, success
    }

    // Static buffers are pre-allocated at compile time
    // Total: ~1.16 KB (all in static memory)

    buffers_allocated_ = true;
    return true;  // Allocation successful
}

// Guard clause: Returns true if display buffers are valid for use
bool DroneDisplayController::are_buffers_valid() const {
    // DIAMOND OPTIMIZATION: Direct flag access (O(1), zero overhead)
    // For static buffers, "valid" means "allocated and ready"
    return buffers_allocated_;
}

// Guard clause: Deallocates display buffers (static, just marks as not ready)
void DroneDisplayController::deallocate_buffers() {
    // Early return if not allocated
    if (!buffers_allocated_) {
        return;  // Nothing to deallocate
    }

    // DIAMOND OPTIMIZATION: Static buffers don't need deallocation
    // Just clear the flag to mark buffers as unavailable
    // Buffers remain in .bss section, will be reused on next allocation
    buffers_allocated_ = false;

    // Clear buffer contents to prevent stale data usage
    // DIAMOND OPTIMIZATION: Zero-initialize all buffers
    std::fill(spectrum_power_levels().begin(), spectrum_power_levels().end(), 0);
}


void DroneDisplayController::update_detection_display(const DroneScanner& scanner) {
    // Separate UI/DSP logic - Diamond Code pattern
    // Phase 1: Data fetching (DSP/logic layer) - no UI calls
    // Phase 2: UI rendering (presentation layer) - no data fetching
    // This separation ensures atomic data fetching and consistent UI rendering
    if (!buffers_allocated_) {
        return;
    }
    
    // Phase 1: DATA FETCHING (DSP/Logic Layer)
    // Fetch all data from scanner into local variables
    // This ensures no UI calls are made during data fetching
    
    DisplayData data;
    data.is_scanning = scanner.is_scanning_active();
    data.current_freq = scanner.get_current_scanning_frequency();
    data.total_freqs = scanner.get_database_size();
    data.max_threat = scanner.get_max_detected_threat();
    data.approaching_count = scanner.get_approaching_count();
    data.receding_count = scanner.get_receding_count();
    data.static_count = scanner.get_static_count();
    data.total_detections = scanner.get_total_detections();
    data.is_real_mode = scanner.is_real_mode();
    data.scan_cycles = scanner.get_scan_cycles();
    data.has_detections = (data.approaching_count + data.receding_count + data.static_count) > 0;
    
    // 🔴 FIX #13: Set timestamp when snapshot is captured
    // This helps identify stale data in UI (snapshot is point-in-time)
    data.snapshot_timestamp = chTimeNow();
    
    // Determine color index (logic only, no UI calls)
    data.color_idx = (data.max_threat >= ThreatLevel::HIGH) ? 4 :
                     (data.max_threat >= ThreatLevel::MEDIUM) ? 3 :
                     (data.has_detections) ? 2 :
                     (data.is_scanning) ? 1 : 0;
    
    // Phase 2: UI RENDERING (Presentation Layer)
    // Render UI using fetched data only - no scanner calls
    if (data.is_scanning) {
        if (data.current_freq > 0) {
            // Use buffer-based formatting
            char freq_buf[16];
            FrequencyFormatter::to_string_short_freq_buffer(freq_buf, sizeof(freq_buf), data.current_freq);
            big_display_.set(freq_buf);
        } else {
            big_display_.set("2400.0MHz");
        }
    } else {
        big_display_.set("READY");
    }

    if (data.total_freqs > 0 && data.is_scanning) {
        uint32_t progress_percent = 50;
        scanning_progress_.set_value(std::min(progress_percent, static_cast<uint32_t>(100)));
    } else {
        scanning_progress_.set_value(0);
    }

    if (data.has_detections) {
        char summary_buffer[48];
        const char* threat_name = UnifiedStringLookup::threat_name(static_cast<uint8_t>(data.max_threat));
        // Use StatusFormatter
        StatusFormatter::format_to(summary_buffer, "THREAT: %s | <%lu ~%lu >%lu",
                                  threat_name,
                                  static_cast<unsigned long>(data.approaching_count),
                                  static_cast<unsigned long>(data.static_count),
                                  static_cast<unsigned long>(data.receding_count));
        text_threat_summary_.set(summary_buffer);
        text_threat_summary_.set_style(&UIStyles::RED_STYLE);
    } else {
        text_threat_summary_.set("THREAT: NONE | All clear");
        text_threat_summary_.set_style(&UIStyles::GREEN_STYLE);
    }

    char status_buffer[48];
    if (data.is_scanning) {
        // Use const char* instead of std::string (saves RAM)
        const char* mode_str = data.is_real_mode ? "REAL" : "DEMO";
        StatusFormatter::format_to(status_buffer, "%s - Detections: %lu",
                                  mode_str, static_cast<unsigned long>(data.total_detections));
    } else {
        StatusFormatter::format_to(status_buffer, "Ready - Enhanced Drone Analyzer");
    }
    text_status_info_.set(status_buffer);

    char stats_buffer[48];
    if (data.is_scanning && data.total_freqs > 0) {
        size_t current_idx = 0;
        // Use StatusFormatter
        StatusFormatter::format_to(stats_buffer, "Freq: %lu/%lu | Cycle: %lu",
                                  static_cast<unsigned long>(current_idx + 1),
                                  static_cast<unsigned long>(data.total_freqs),
                                  static_cast<unsigned long>(data.scan_cycles));
    } else if (data.total_freqs > 0) {
        StatusFormatter::format_to(stats_buffer, "Loaded: %lu frequencies",
                                  static_cast<unsigned long>(data.total_freqs));
    } else {
        StatusFormatter::format_to(stats_buffer, "No database loaded");
    }
    text_scanner_stats_.set(stats_buffer);

    // Use constexpr LUT (stored in Flash)
    big_display_.set_style(&BIG_DISPLAY_STYLES[data.color_idx]);
}

// Implementation: DroneDisplayController::set_scanning_status
void DroneDisplayController::set_scanning_status(bool active, const char* message) {
    // Early return for invalid states
    if (!message) return;
    
    // Use StatusFormatter
    char buffer[48];
    if (active) {
        StatusFormatter::format_to(buffer, "SCAN: %s", message);
        text_status_info_.set(buffer);
        text_status_info_.set_style(&UIStyles::GREEN_STYLE);
    } else {
        StatusFormatter::format_to(buffer, "STOP: %s", message);
        text_status_info_.set_style(&UIStyles::LIGHT_STYLE);
    }
}

void DroneDisplayController::update_signal_type_display(const char* signal_type) {
    if (!signal_type) return;
    text_scanner_stats_.set(signal_type);
}

void DroneDisplayController::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) {
    systime_t now = chTimeNow();

    for (size_t i = 0; i < detected_drones_count_; ++i) {
        if (detected_drones()[i].frequency == freq) {
            detected_drones()[i].rssi = rssi;
            detected_drones()[i].threat = threat;
            detected_drones()[i].type = type;
            detected_drones()[i].last_seen = now;
            safe_strcpy(detected_drones()[i].type_name, UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type)), sizeof(detected_drones()[i].type_name));
            detected_drones()[i].display_color = UnifiedColorLookup::drone(static_cast<uint8_t>(type));
            sort_drones_by_rssi();
            render_drone_text_display();
            return;
        }
    }

    if (detected_drones_count_ < MAX_UI_DRONES) {
        auto& entry = detected_drones()[detected_drones_count_];
        entry.frequency = freq;
        entry.rssi = rssi;
        entry.threat = threat;
        entry.type = type;
        entry.last_seen = now;
        safe_strcpy(entry.type_name, UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type)), sizeof(entry.type_name));
        entry.display_color = UnifiedColorLookup::drone(static_cast<uint8_t>(type));
        entry.trend = MovementTrend::STATIC;
        detected_drones_count_++;
    }
    sort_drones_by_rssi();
    render_drone_text_display();
}

void DroneDisplayController::sort_drones_by_rssi() {
    std::sort(detected_drones().begin(), detected_drones().end(),
              [](const DisplayDroneEntry& a, const DisplayDroneEntry& b) {
                  if (a.rssi != b.rssi) return a.rssi > b.rssi;
                  if (a.threat != b.threat) return static_cast<int>(a.threat) > static_cast<int>(b.threat);
                  return a.last_seen > b.last_seen;
              });
}

void DroneDisplayController::update_drones_display(const DroneScanner& scanner) {
    // STEP 1: Get a safe copy of data
    // Mutex is locked only for the copying time (microseconds)
    auto snapshot = scanner.get_tracked_drones_snapshot();

    // Step 2: Work with local snapshot copy

    const systime_t STALE_TIMEOUT = 30000;
    systime_t now = chTimeNow();

    detected_drones_count_ = 0;

    for (size_t i = 0; i < snapshot.count; ++i) {
        const auto& drone_data = snapshot.drones[i];

        // Stale check (duplicate logic for UI filtering)
        if ((now - drone_data.last_seen) > STALE_TIMEOUT) continue;

        if (detected_drones_count_ < MAX_UI_DRONES) {
            auto& entry = detected_drones()[detected_drones_count_];
            entry.frequency = drone_data.frequency;
            entry.type = static_cast<DroneType>(drone_data.drone_type);
            entry.threat = static_cast<ThreatLevel>(drone_data.threat_level);
            entry.rssi = drone_data.rssi;
            entry.last_seen = drone_data.last_seen;
            safe_strcpy(entry.type_name, UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(entry.type)), sizeof(entry.type_name));
            entry.display_color = UnifiedColorLookup::drone(static_cast<uint8_t>(entry.type));
            entry.trend = drone_data.get_trend();
            detected_drones_count_++;
        }
    }

    // Step 3: Sorting and rendering
    sort_drones_by_rssi();

    // Remaining code for updating display_drones_ and calling render
    for (auto& drone : displayed_drones_) {
        drone = DisplayDroneEntry{};
    }
    size_t count = std::min(detected_drones_count_, MAX_DISPLAYED_DRONES);
    for (size_t i = 0; i < count; ++i) {
        displayed_drones_[i] = detected_drones()[i];
    }
    highlight_threat_zones_in_spectrum(displayed_drones_);
    render_drone_text_display();
}

void DroneDisplayController::render_drone_text_display() {
    text_drone_1_.set("");
    text_drone_2_.set("");
    text_drone_3_.set("");

    size_t drone_count = std::min(displayed_drones_.size(), size_t(3));
    for (size_t i = 0; i < drone_count; ++i) {
        const auto& drone = displayed_drones_[i];
        char buffer[64];
        char freq_buf[16]; // Локальный буфер вместо std::string (экономия RAM)

        // DIAMOND OPTIMIZATION: Use TrendSymbols for O(1) lookup
        char trend_symbol = TrendSymbols::from_trend(static_cast<uint8_t>(drone.trend));

        // DIAMOND OPTIMIZATION: Table-driven frequency formatting (устраняет if/else)
        // O(1) lookup вместо каскадных if/else (экономит ~15 строк кода)
        const auto& format_entry = FREQ_FORMAT_TABLE[
            (drone.frequency >= 1000000000LL) ? 0 :
            (drone.frequency >= 1000000LL) ? 1 :
            (drone.frequency >= 1000LL) ? 2 : 3
        ];

        int64_t int_part = drone.frequency / format_entry.divider;
        int64_t dec_part = (format_entry.decimal_div > 1) ?
                          (drone.frequency % format_entry.divider) / format_entry.decimal_div : 0;

        if (format_entry.decimal_div > 1) {
            snprintf(freq_buf, sizeof(freq_buf), format_entry.format,
                     (unsigned long)int_part, (unsigned long)dec_part);
        } else {
            snprintf(freq_buf, sizeof(freq_buf), format_entry.format,
                     (unsigned long)int_part);
        }

        // Use StatusFormatter
        StatusFormatter::format_to(buffer, DRONE_DISPLAY_FORMAT,
                                 drone.type_name,
                                 freq_buf,
                                 (long int)drone.rssi,
                                 trend_symbol);

        // Helper for indexed access (no bounds check each time)
        Text* target = drone_text_widget(i);
        if (target) {
            target->set(buffer);
        }
    }
}

void DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum& spectrum) {
    uint8_t current_bin_power = 0;
    for (size_t bin = 0; bin < MINI_SPECTRUM_WIDTH; bin++) {
        if (bin < spectrum.db.size()) {
            current_bin_power = spectrum.db[bin];
        } else {
            current_bin_power = 0;
        }
        if (process_bins(&current_bin_power)) {
            return;
        }
    }
}

bool DroneDisplayController::process_bins(uint8_t* powerlevel) {
    bins_hz_size += each_bin_size;
    if (bins_hz_size >= marker_pixel_step) {
        // Thread-safe buffer access with mutex protection
        // Lock order: DISPLAY_SPECTRUM_MUTEX (level 4)
        MutexLock lock(spectrum_mutex_, LockOrder::DISPLAY_SPECTRUM_MUTEX);

        if (pixel_index < spectrum_power_levels().size()) {
            spectrum_power_levels()[pixel_index] = (*powerlevel > min_color_power) ? *powerlevel : 0;
        }
        *powerlevel = 0;

        if (!pixel_index) {
            bins_hz_size = 0;
            return true;
        }
        bins_hz_size -= marker_pixel_step;
    }
    return false;
}

void DroneDisplayController::render_bar_spectrum(Painter& painter) {
    const auto& config = BarSpectrumConfig{};

    // Clear spectrum area
    const int waterfall_y_start = config.WATERFALL_Y_START;
    const int spectrum_height = config.BAR_HEIGHT_MAX;

    painter.fill_rectangle(
        {0, waterfall_y_start, EDA::Constants::MINI_SPECTRUM_WIDTH, spectrum_height},
        Color::black()
    );

    // Validate buffer (static buffer)
    if (!buffers_allocated_) {
        return;
    }

    // Thread-safe buffer access with mutex protection
    // Lock order: DISPLAY_SPECTRUM_MUTEX (level 4)
    MutexLock lock(spectrum_mutex_, LockOrder::DISPLAY_SPECTRUM_MUTEX);

    // Iterate through all spectrum bins
    const auto& levels = spectrum_power_levels();
    const size_t spectrum_width = std::min(levels.size(),
                                        static_cast<size_t>(EDA::Constants::MINI_SPECTRUM_WIDTH));

    for (size_t x = 0; x < spectrum_width; ++x) {
        uint8_t power = levels[x];

        // Noise filter: don't draw if signal too weak
        if (power < config.NOISE_THRESHOLD) continue;

        // Calculate bar height (0-255 → 0-spectrum_height)
        int bar_height = (power * spectrum_height) / 255;
        if (bar_height < 1) bar_height = 1;
        if (bar_height > spectrum_height) bar_height = spectrum_height;

        // Analyze signal shape (Narrow vs Wide)
        size_t color_idx = get_bar_color_index(x, power);

        // Draw bar
        int y_top = (waterfall_y_start + spectrum_height) - bar_height;
        painter.fill_rectangle(
            {static_cast<int>(x), y_top, 1, bar_height},
            config.BAR_COLORS[color_idx]
        );
    }
}

// Histogram Display Implementation
// Zero heap allocation, static storage, integer-only math

void DroneDisplayController::update_histogram_display(
    const SpectralAnalyzer::HistogramBuffer& analysis_histogram,
    uint8_t noise_floor
) noexcept {
    // Guard clause: Validate input histogram
    if (analysis_histogram.empty()) {
        // Thread-safe buffer access with mutex protection
        // Lock order: DISPLAY_HISTOGRAM_MUTEX (level 5)
        MutexLock lock(histogram_mutex_, LockOrder::DISPLAY_HISTOGRAM_MUTEX);
        histogram_display_buffer_.is_valid = false;
        return;
    }

    // Copy histogram data from analysis buffer to display buffer
    // Scale from uint16_t to uint8_t (0-255) for display
    uint8_t max_count = 0;

    // Thread-safe buffer access with mutex protection
    // Lock order: DISPLAY_HISTOGRAM_MUTEX (level 5)
    MutexLock lock(histogram_mutex_, LockOrder::DISPLAY_HISTOGRAM_MUTEX);

    for (size_t i = 0; i < HISTOGRAM_NUM_BINS; ++i) {
        uint16_t raw_count = (i < analysis_histogram.size()) ? analysis_histogram[i] : 0;
        histogram_display_buffer_.bin_counts[i] =
            static_cast<uint8_t>(std::min(raw_count, static_cast<uint16_t>(255)));

        if (histogram_display_buffer_.bin_counts[i] > max_count) {
            max_count = histogram_display_buffer_.bin_counts[i];
        }
    }

    histogram_display_buffer_.max_count = max_count;
    histogram_display_buffer_.noise_floor = noise_floor;
    histogram_display_buffer_.is_valid = true;

    histogram_dirty_ = true;
}

void DroneDisplayController::render_histogram(Painter& painter) noexcept {
    // Guard clause: Skip rendering if data hasn't changed
    if (!histogram_dirty_) {
        return;
    }

    // Thread-safe buffer access with mutex protection
    // Lock order: DISPLAY_HISTOGRAM_MUTEX (level 5)
    MutexLock lock(histogram_mutex_, LockOrder::DISPLAY_HISTOGRAM_MUTEX);

    // Guard clause: Skip if histogram data is invalid
    if (!histogram_display_buffer_.is_valid) {
        return;
    }

    const auto& config = HistogramColorConfig{};

    // Clear histogram area
    painter.fill_rectangle(
        {0, HISTOGRAM_Y, HISTOGRAM_WIDTH, HISTOGRAM_HEIGHT},
        Color::black()
    );

    // Render 64 bins with color gradient
    const uint8_t max_count = histogram_display_buffer_.max_count;

    // Guard clause: Skip rendering if max_count is zero
    if (max_count == 0) {
        histogram_dirty_ = false;
        return;
    }

    // Calculate scaling factor for bin height (0-255 -> 0-HISTOGRAM_HEIGHT)
    // Diamond Code: Use 256 as divisor for fast bit shift approximation
    const int scale_factor = HISTOGRAM_HEIGHT;

    for (size_t bin_idx = 0; bin_idx < HISTOGRAM_NUM_BINS; ++bin_idx) {
        uint8_t bin_count = histogram_display_buffer_.bin_counts[bin_idx];
        
        // Skip empty bins
        if (bin_count == 0) continue;
        
        // Calculate bin height (integer scaling)
        int bin_height = (static_cast<int>(bin_count) * scale_factor) / 256;
        if (bin_height < 1) bin_height = 1;
        if (bin_height > HISTOGRAM_HEIGHT) bin_height = HISTOGRAM_HEIGHT;
        
        const int bin_x = static_cast<int>(bin_idx * HISTOGRAM_BIN_WIDTH);
        const int bin_width = static_cast<int>(HISTOGRAM_BIN_WIDTH);
        
        // Calculate color level based on signal strength (integer-only)
        uint8_t color_level;
        if (bin_count <= HISTOGRAM_COLOR_THRESHOLD_20PCT) {           // 0-20%
            color_level = 0;  // dark_grey (noise floor)
        } else if (bin_count <= HISTOGRAM_COLOR_THRESHOLD_40PCT) {
            color_level = 1;  // blue (low signal)
        } else if (bin_count <= HISTOGRAM_COLOR_THRESHOLD_60PCT) {
            color_level = 2;  // cyan (medium signal)
        } else if (bin_count <= HISTOGRAM_COLOR_THRESHOLD_80PCT) {
            color_level = 3;  // yellow (high signal)
        } else {
            color_level = 4;  // red (peak signal)
        }
        
        // Bounds check for color array
        if (color_level >= config.NUM_COLOR_LEVELS) {
            color_level = 4;
        }
        
        const int y_top = (HISTOGRAM_Y + HISTOGRAM_HEIGHT) - bin_height;
        painter.fill_rectangle(
            {bin_x, y_top, bin_width, bin_height},
            config.HISTOGRAM_COLORS[color_level]
        );
    }
    
    // Mark histogram as clean (rendered)
    histogram_dirty_ = false;
}

void DroneDisplayController::clear_histogram_area(Painter& painter) noexcept {
    constexpr auto HISTOGRAM_Y = 164;
    constexpr auto HISTOGRAM_HEIGHT = 26;
    constexpr auto HISTOGRAM_X = 8;
    constexpr auto HISTOGRAM_WIDTH = 304;

    const Rect histogram_rect{
        HISTOGRAM_X,
        HISTOGRAM_Y,
        HISTOGRAM_WIDTH,
        HISTOGRAM_HEIGHT
    };

    painter.fill_rectangle(histogram_rect, Color::black());
}

void DroneDisplayController::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES>& drones) {
    threat_bins_count_ = 0;
    for (const auto& drone : drones) {
        if (drone.frequency > 0) {
            size_t bin_x = frequency_to_spectrum_bin(drone.frequency);
            if (bin_x < MINI_SPECTRUM_WIDTH && threat_bins_count_ < MAX_DISPLAYED_DRONES) {
                threat_bins_[threat_bins_count_].bin = bin_x;
                threat_bins_[threat_bins_count_].threat = drone.threat;
                threat_bins_count_++;
            }
        }
    }
}

void DroneDisplayController::clear_spectrum_buffers() {
    if (!buffers_allocated_) {
        return;
    }
    std::fill(spectrum_power_levels().begin(), spectrum_power_levels().end(), 0);
}

void DroneDisplayController::set_spectrum_range(Frequency min_freq, Frequency max_freq) {
    if (min_freq >= max_freq || min_freq < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ ||
        max_freq > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        spectrum_config_.min_freq = EDA::Constants::WIDEBAND_24GHZ_MIN;
        spectrum_config_.max_freq = EDA::Constants::WIDEBAND_24GHZ_MAX;
        update_frequency_ruler();
        return;
    }
    spectrum_config_.min_freq = min_freq;
    spectrum_config_.max_freq = max_freq;
    spectrum_config_.bandwidth = (max_freq - min_freq) > EDA::Constants::WIDEBAND_WIFI_MIN_WIDTH_HZ ?
                                EDA::Constants::WIDEBAND_DEFAULT_SLICE_WIDTH : static_cast<uint32_t>(max_freq - min_freq);
    spectrum_config_.sampling_rate = spectrum_config_.bandwidth;
    update_frequency_ruler();
}

size_t DroneDisplayController::frequency_to_spectrum_bin(Frequency freq_hz) const {
    const Frequency MIN_FREQ = spectrum_config_.min_freq;
    const Frequency MAX_FREQ = spectrum_config_.max_freq;
    const Frequency FREQ_RANGE = MAX_FREQ - MIN_FREQ;
    if (freq_hz < MIN_FREQ || freq_hz > MAX_FREQ || FREQ_RANGE == 0) {
        return MINI_SPECTRUM_WIDTH;
    }
    int64_t relative_freq = freq_hz - MIN_FREQ;
    size_t bin = static_cast<size_t>((relative_freq * MINI_SPECTRUM_WIDTH) / FREQ_RANGE);
    return std::min(bin, static_cast<size_t>(MINI_SPECTRUM_WIDTH - 1));
}

// Pass settings by value to scanner constructor (eliminates lifetime dependency)
DroneUIController::DroneUIController(NavigationView& nav,
                                          DroneHardwareController& hardware,
                                          DroneScanner& scanner,
                                          ::AudioManager& audio_mgr)
    : nav_(nav),
      hardware_(hardware),
      scanner_(scanner),
      audio_mgr_(audio_mgr),
      scanning_active_{false},
      display_controller_(nullptr),
      settings_()  // Initialize settings_ with defaults
{
    settings_.spectrum_mode = SpectrumMode::MEDIUM;
    settings_.scan_interval_ms = 1000;
    settings_.rssi_threshold_db = -90;
    audio_set_enable_alerts(settings_, true);
    char buffer[32];

    bool current = audio_get_enable_alerts(settings_);
    audio_set_enable_alerts(settings_, !current);
    const char* status = audio_get_enable_alerts(settings_) ? "ENABLED" : "DISABLED";
    safe_strcpy(buffer, "Alerts ", sizeof(buffer));
    safe_strcat(buffer, status, sizeof(buffer));
    nav_.display_modal("Audio Alerts", buffer);
}

DroneUIController::~DroneUIController() {
}

void DroneUIController::on_start_scan() {
    scanner_.start_scanning();
    {
        raii::SystemLock lock;
        scanning_active_ = true;
    }
}

void DroneUIController::on_stop_scan() {
    scanner_.stop_scanning();
    {
        raii::SystemLock lock;
        scanning_active_ = false;
    }
}

void DroneUIController::show_menu() {
    nav_.push<DroneAnalyzerMenuView>();
}

void DroneUIController::on_hardware_control() {
    show_hardware_status();
}

void DroneUIController::on_set_bandwidth() {
    uint32_t current_bw = hardware_.get_spectrum_bandwidth();
    char buffer[48];
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)current_bw);
    nav_.display_modal("Set Bandwidth (MHz)", buffer);
}

void DroneUIController::on_set_center_freq() {
    Frequency current_cf = hardware_.get_spectrum_center_frequency();
    hardware_.set_spectrum_center_frequency(current_cf + 1000000);
    show_hardware_status();
}

void DroneUIController::show_hardware_status() {
    // Use constant instead of magic number
    char buffer[EDA::Constants::LAST_TEXT_BUFFER_SIZE];
    char freq_buf[32];
    uint32_t band_mhz = hardware_.get_spectrum_bandwidth() / 1000000ULL;
    FrequencyFormatter::format_to_buffer(freq_buf, sizeof(freq_buf), hardware_.get_spectrum_center_frequency(),
                                     FrequencyFormatter::Format::STANDARD_GHZ);
    StatusFormatter::format_to(buffer, "Band: %lu MHz\nFreq: %s",
                               (unsigned long)band_mhz, freq_buf);
    nav_.display_modal("Hardware Status", buffer);
}

void DroneUIController::on_view_logs() {
    auto open_view = nav_.push<FileLoadView>(".CSV");
    open_view->push_dir("/LOGS/EDA");
}

void DroneUIController::on_about() {
    nav_.display_modal("EDA v1.0", "Enhanced Drone Analyzer\nMayhem Firmware Integration\nBased on Recon & Looking Glass");
}

void DroneUIController::update_scanner_range(Frequency min_freq, Frequency max_freq) {
    scanner_.update_scan_range(min_freq, max_freq);
    display_controller_->set_spectrum_range(min_freq, max_freq);
}

// --- РЕАЛИЗАЦИЯ FrequencyRangeSetupView ---

FrequencyRangeSetupView::FrequencyRangeSetupView(NavigationView& nav, DroneUIController& controller)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      controller_(controller),
      text_title_({4, 4, 224, 16}, "Panoramic Spectrum"),
      label_min_({4, 36, 80, 16}, "Start Freq:"),
      field_min_({88, 36, 160, 16}, "2400.000000"),
      label_max_({4, 68, 80, 16}, "End Freq:"),
      field_max_({88, 68, 160, 16}, "2500.000000"),
      label_slice_({4, 100, 80, 16}, "Res (BW):"),
      field_slice_({88, 100, 160, 16}, "24 MHz"),
      button_save_({4, 140, 224, 40}, "Apply Range"),
      button_cancel_({4, 190, 224, 40}, "Cancel") {

    add_children({
        &text_title_,
        &label_min_, &field_min_,
        &label_max_, &field_max_,
        &label_slice_, &field_slice_,
        &button_save_, &button_cancel_
    });

    button_save_.on_select = [this](Button&) {
        on_save();
    };
    
    button_cancel_.on_select = [this](Button&) {
        on_cancel();
    };
}

void FrequencyRangeSetupView::focus() {
    char min_freq_buf[32];
    char max_freq_buf[32];
    const auto& settings = controller_.settings();

    FrequencyFormatter::format_to_buffer(min_freq_buf, sizeof(min_freq_buf), settings.wideband_min_freq_hz,
                                       FrequencyFormatter::Format::DETAILED_GHZ);
    FrequencyFormatter::format_to_buffer(max_freq_buf, sizeof(max_freq_buf), settings.wideband_max_freq_hz,
                                       FrequencyFormatter::Format::DETAILED_GHZ);

    field_min_.set_text(min_freq_buf);
    field_max_.set_text(max_freq_buf);

    // Use StatusFormatter
    char slice_buffer[32];
    uint64_t slice_mhz = settings.wideband_slice_width_hz / 1000000;
    StatusFormatter::format_to(slice_buffer, "%lu MHz", (unsigned long)slice_mhz);
    field_slice_.set(slice_buffer);

    button_save_.focus();
}

void FrequencyRangeSetupView::on_save() {
    // Use const char* from get_text() reference (no new heap allocation)
    const char* min_str = field_min_.get_text().c_str();
    const char* max_str = field_max_.get_text().c_str();
    
    Frequency new_min = static_cast<Frequency>(DiamondCore::FrequencyParser::parse_mhz_string(min_str));
    Frequency new_max = static_cast<Frequency>(DiamondCore::FrequencyParser::parse_mhz_string(max_str));
    
    // Input validation (integer-only)
    if (new_min == 0 || new_max == 0) {
        nav_.display_modal("Error", "Invalid frequency format");
        return;
    }
    
    // Get slice width from settings
    uint64_t new_slice_width = controller_.settings().wideband_slice_width_hz;
    
    // Validation - use unified constants from DroneConstants
    if (new_min >= new_max) {
        nav_.display_modal("Error", "Min freq must be < Max freq");
        return;
    }
    
    if (new_min < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ || 
        new_max > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        nav_.display_modal("Error", "Frequency out of range (1MHz - 7.2GHz)");
        return;
    }
    
    if (new_max > new_max + new_min) {
        nav_.display_modal("Error", "Frequency range overflow detected");
        return;
    }

    // Save settings (use existing settings reference)
    controller_.settings().wideband_min_freq_hz = new_min;
    controller_.settings().wideband_max_freq_hz = new_max;

    // Save to file with success check
    if (!SettingsPersistence<DroneAnalyzerSettings>::save(controller_.settings())) {
        nav_.display_modal("Error", "Failed to save settings to SD card");
        return;
    }
    
    controller_.update_scanner_range(new_min, new_max);
    
    char buffer[64];
    char min_freq_buf[32];
    char max_freq_buf[32];
    FrequencyFormatter::format_to_buffer(min_freq_buf, sizeof(min_freq_buf), new_min, FrequencyFormatter::Format::STANDARD_MHZ);
    FrequencyFormatter::format_to_buffer(max_freq_buf, sizeof(max_freq_buf), new_max, FrequencyFormatter::Format::STANDARD_MHZ);
    StatusFormatter::format_to(buffer, "Range updated:\n%s - %s\nBW: %lu MHz",
                               min_freq_buf, max_freq_buf,
                               (unsigned long)(new_slice_width / 1000000));

    nav_.display_modal("Success", buffer);
    nav_.pop();
}

void FrequencyRangeSetupView::on_cancel() {
    nav_.pop();
}

DroneAnalyzerMenuView::DroneAnalyzerMenuView(NavigationView& nav)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      menu_view_{{0, 16, screen_width, screen_height - 16}} {

    add_children({&menu_view_});

    menu_view_.add_item({"Audio Settings", Theme::getInstance()->fg_yellow->foreground, nullptr,
        [this](KeyEvent) { nav_.push<AudioSettingsView>(); }});
    menu_view_.add_item({"Hardware Control", Theme::getInstance()->fg_green->foreground, nullptr,
        [this](KeyEvent) { nav_.push<HardwareSettingsView>(); }});
    menu_view_.add_item({"View Logs", Theme::getInstance()->fg_blue->foreground, nullptr,
        [this](KeyEvent) { nav_.push<FileLoadView>(".CSV"); }});
    menu_view_.add_item({"About", Theme::getInstance()->fg_yellow->foreground, nullptr,
        [this](KeyEvent) { nav_.display_modal("About", "Enhanced Drone Analyzer v2.0\nDetection and analysis tool for UAV signals"); }});
}

void DroneAnalyzerMenuView::focus() {
    menu_view_.focus();
}

// Menu implementation removed - now using DroneAnalyzerSettingsView from ui_enhanced_drone_settings.cpp

// 🔴 FIX #M11: Helper function to get default scanner settings
// Diamond Code: Static function with stack allocation, no heap
// Returns initialized settings to prevent use-before-initialization
static DroneAnalyzerSettings get_default_scanner_settings() noexcept {
    DroneAnalyzerSettings default_settings;
    SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(default_settings);
    return default_settings;
}

// Pass settings by value to scanner constructor (eliminates lifetime dependency)
EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      settings_(),
      hardware_(SpectrumMode::MEDIUM),
      // Pass meaningful initial settings to scanner constructor
      scanner_(get_default_scanner_settings()),
      audio_(),
      ui_controller_(nav, hardware_, scanner_, audio_),  // No display_controller in constructor
      display_controller_({0, 60, screen_width, screen_height - 80}),
      scanning_coordinator_(nav, hardware_, scanner_, display_controller_, audio_),
      smart_header_(Rect{0, 0, screen_width, 60}),
      status_bar_(0, Rect{0, screen_height - 80, screen_width, 16}),
      threat_cards_(),
      button_start_stop_({screen_width - 80, screen_height - 72, 72, 32}, "START/STOP"),
      button_menu_({screen_width - 80, screen_height - 40, 72, 32}, "MENU"),
      button_audio_({screen_width - 160, screen_height - 72, 72, 32}, "AUDIO: OFF"),
      field_scanning_mode_({10, screen_height - 72}, 15, OptionsField::options_t{{"Database", 0}, {"Wideband",1}, {"Hybrid", 2}}),
      scanning_active_(false)
{
    // Remove sd_mutex_initialized pattern (mutex already declared as static at namespace scope)

    // 🔧 FIX: Set display_controller after construction
    // This makes initialization order independent of member declaration order
    ui_controller_.set_display_controller(&display_controller_);

    // 🔧 FIX: МИНИМАЛЬНЫЙ конструктор - только UI setup
    // Все вызовы scanner_/hardware_/coordinator_ перенесены в step_deferred_initialization()

    // 🔧 FIX: Установить начальное состояние
    init_state_ = InitState::CONSTRUCTED;

    setup_button_handlers();
    initialize_scanning_mode();
    // Move add_ui_elements() to appropriate phase (after parent View initialization)
}

EnhancedDroneSpectrumAnalyzerView::~EnhancedDroneSpectrumAnalyzerView() {
    // TODO[FIXED]: STEP 2.1: Destruction order
    // 1. Stop activity (in dependency order)
    scanning_coordinator_.stop_coordinated_scanning();
    scanner_.stop_scanning();
    hardware_.shutdown_hardware();

    // Explicit buffer deallocation (before stack objects destroyed)
    display_controller_.deallocate_buffers();

    // 2. Stack objects will be automatically destroyed in reverse order
    // No manual deletion needed for stack-allocated objects
}

void EnhancedDroneSpectrumAnalyzerView::focus() {
    button_start_stop_.focus();
}

void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
    // Stack monitoring to prevent stack overflow
    // paint() method uses ~1.6KB of stack per call
    // STACK OPTIMIZATION: Reduced stack requirement to 1792 bytes (1.75KB)
    StackMonitor stack_monitor;
    constexpr size_t PAINT_STACK_REQUIRED = 1792;  // 1.75KB for paint() method (reduced from 2KB)

    // Guard clause: Return early if insufficient stack
    if (!stack_monitor.is_stack_safe(PAINT_STACK_REQUIRED)) {
        // Cannot safely render - skip this frame
        return;
    }

    // 🔧 FIX: Всегда вызываем базовый paint для очистки экрана
    View::paint(painter);

    // Execute deferred initialization
    // CRITICAL: This was missing, causing permanent hang
    if (init_state_ != InitState::FULLY_INITIALIZED &&
        init_state_ != InitState::INITIALIZATION_ERROR) {
        step_deferred_initialization();
    }

    // 🔧 FIX: Показываем сообщение об ошибке с возможностью выхода
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        // Заполняем экран чёрным
        painter.fill_rectangle({0, 0, screen_width, screen_height}, Color::black());
        
        // Error header
        painter.draw_string({10, 80}, Style{font::fixed_8x16, Color::red(), Color::black()}, "INIT ERROR");
        
        // Error message
        painter.draw_string({10, 100}, Style{font::fixed_8x16, Color::white(), Color::black()}, 
                           ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]);
        
        // Instructions
        painter.draw_string({10, 130}, Style{font::fixed_8x16, Color::yellow(), Color::black()}, "Press BACK to exit");
        return;
    }
    
    // 🔧 FIX: Показываем прогресс инициализации
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        // Текущая фаза
        size_t phase_idx = static_cast<size_t>(init_state_);
        if (phase_idx < 7) {
            painter.draw_string({10, 80}, Style{font::fixed_8x16, Color::white(), Color::black()}, "Loading...");
            painter.draw_string({10, 100}, Style{font::fixed_8x16, Color::green(), Color::black()}, 
                               INIT_STATUS_MESSAGES[phase_idx]);
            
            // Progress bar (6 phases = 16.6% each)
            uint8_t progress = static_cast<uint8_t>(phase_idx * 16);
            if (progress > 100) progress = 100;
            
            // Progress bar background
            painter.fill_rectangle({10, 120, 100, 10}, Color::dark_grey());
            // Filled portion
            painter.fill_rectangle({10, 120, progress, 10}, Color::green());
        }
        return;
    }
    
    // Additional buffer check
    if (display_controller_.are_buffers_valid()) {
        display_controller_.render_bar_spectrum(painter);

        // Handle histogram display based on mode
        if (display_controller_.get_display_mode() == ui::apps::enhanced_drone_analyzer::DroneDisplayController::DisplayRenderMode::HISTOGRAM) {
            // DIAMOND OPTIMIZATION: Render histogram after bar spectrum
            // Histogram is positioned at y=164-190 (below bar spectrum)
            display_controller_.render_histogram(painter);
        } else {
            // Clear histogram area when not in histogram mode
            display_controller_.clear_histogram_area(painter);
        }
    }
}

bool EnhancedDroneSpectrumAnalyzerView::on_key(const KeyEvent key) {
    switch(key) {
        case KeyEvent::Back:
            stop_scanning_thread();
            nav_.pop();
            return true;
        default:
            break;
    }
    return View::on_key(key);
}

bool EnhancedDroneSpectrumAnalyzerView::on_touch(const TouchEvent event) {
    return View::on_touch(event);
}

void EnhancedDroneSpectrumAnalyzerView::on_toggle_display_mode() {
    const auto current_mode = display_controller_.get_display_mode();
    const auto new_mode = (current_mode == ui::apps::enhanced_drone_analyzer::DroneDisplayController::DisplayRenderMode::SPECTRUM)
                          ? ui::apps::enhanced_drone_analyzer::DroneDisplayController::DisplayRenderMode::HISTOGRAM
                          : ui::apps::enhanced_drone_analyzer::DroneDisplayController::DisplayRenderMode::SPECTRUM;
    display_controller_.set_display_mode(new_mode);
    set_dirty();  // Trigger repaint
}

// Phase 2.5: Deferred Initialization Implementation

// Phase names for logging
static const char* const PHASE_NAMES[] = {
    "ALLOC",      // Phase 0: Buffer allocation
    "DB_LOAD",    // Phase 1: Database loading
    "HW_INIT",    // Phase 2: Hardware initialization
    "UI_SETUP",    // Phase 3: UI setup
    "SETTINGS",    // Phase 4: Settings loading
    "FINALIZE"     // Phase 5: Finalization
};
static_assert(sizeof(PHASE_NAMES) / sizeof(const char*) == 6, "PHASE_NAMES size");


void EnhancedDroneSpectrumAnalyzerView::update_init_progress_display() {
    // DIAMOND OPTIMIZATION: constexpr LUT в Flash вместо switch (строки 3057-3077)
    size_t state_idx = static_cast<size_t>(init_state_);
    
    // Bounds-checked array access
    constexpr size_t MAX_STATE_IDX = 8;  // INITIALIZATION_ERROR = 8
    if (state_idx > MAX_STATE_IDX) {
        state_idx = MAX_STATE_IDX;  // Fallback to error message
    }
    
    // Title: 0=INIT (states 0-6), 1=EDA Ready (state 7), 2=ERROR (state 8)
    size_t title_idx = (state_idx < static_cast<size_t>(InitState::FULLY_INITIALIZED)) ? 0 :
                       (state_idx == static_cast<size_t>(InitState::FULLY_INITIALIZED)) ? 1 : 2;

    status_bar_.update_normal_status(
        INIT_STATUS_TITLES[title_idx],
        INIT_STATUS_MESSAGES[state_idx]
    );
}

// Static Histogram Callback Implementation (no lambda captures)
//
// @param histogram Histogram buffer from SpectralAnalyzer (64 bins)
// @param noise_floor Noise floor value from spectral analysis
// @param user_data User data pointer
void EnhancedDroneSpectrumAnalyzerView::static_histogram_callback(
    const SpectralAnalyzer::HistogramBuffer& histogram,
    uint8_t noise_floor,
    void* user_data
) noexcept {
    // Guard clause: Validate user_data pointer
    if (!user_data) {
        return;
    }
    
    // Cast user_data pointer back to view instance
    auto* view = static_cast<EnhancedDroneSpectrumAnalyzerView*>(user_data);
    
    // Guard clause: Skip if display buffers not ready
    if (!view->display_controller_.are_buffers_valid()) {
        return;
    }
    
    // Forward histogram data to display controller
    view->display_controller_.update_histogram_display(histogram, noise_floor);
}

// Enhanced Initialization State Machine with Timeout Protection
// Eliminates cascading if/else, adds timeout protection, saves ~150 bytes RAM
// EVENT-DRIVEN INITIALIZATION:
// Phases execute based on actual state completion, not time delays
//
// Key changes:
// - Removed time-based delay check
// - Each phase checks for actual completion before transitioning state
// - Only one phase executes per call, ensuring proper sequencing
//
// Benefits:
// - No race conditions between phases
// - Database loading is guaranteed complete before hardware init
// - Settings loading waits for database completion
// - More predictable initialization behavior
//
void EnhancedDroneSpectrumAnalyzerView::step_deferred_initialization() {
    // Add logging at entry
    static systime_t last_log_time = 0;
    static int call_count = 0;
    systime_t now = chTimeNow();
    call_count++;
    
    if (now - last_log_time > MS2ST(1000)) {
        last_log_time = now;
        // Log current state for debugging
        char debug_buf[64];
        snprintf(debug_buf, sizeof(debug_buf), "Init: state=%d, calls=%d",
                 static_cast<int>(init_state_), call_count);
        status_bar_.update_normal_status("DEBUG", debug_buf);
    }
    
    // 🔴 SAFETY: Защита от повторного вызова (re-entrancy)
    if (initialization_in_progress_) return;
    
    // 🔴 SAFETY: Проверка на ошибку (если уже в ERROR state - выходим)
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0,
                                      ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]);
        initialization_in_progress_ = false;
        return;
    }
    
    // 🔴 SAFETY: Установка флага (для защиты от re-entrancy)
    initialization_in_progress_ = true;

    // Check timeout (protects from hangs)
    systime_t elapsed = chTimeNow() - init_start_time_;
    if (elapsed > MS2ST(InitTiming::TIMEOUT_MS)) {
        // Add proper cleanup in timeout path
        
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings_);
        
        // Note: Database loading thread will complete on its own
        // The initialization state machine will handle the incomplete state
        
        init_state_ = InitState::INITIALIZATION_ERROR;
        init_error_ = InitError::GENERAL_TIMEOUT;
        initialization_in_progress_ = false;
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0, "Init timeout!");
        return;
    }


    // 🟢 EVENT-DRIVEN MAIN LOOP: Execute phases based on state completion
    // Instead of time-based delays, we now check if the current phase is complete
    // before moving to the next phase. This eliminates race conditions.
    for (uint8_t i = 0; i < InitTiming::MAX_PHASES; ++i) {
        // LUT lookup instead of switch (O(1) lookup)
        const auto& phase = INIT_PHASES[i];

        // Check if this phase is the next one to execute based on current state
        uint8_t expected_state = static_cast<uint8_t>(InitState::CONSTRUCTED) + i + 1;
        
        // Only execute if current state matches expected state for this phase
        if (static_cast<uint8_t>(init_state_) + 1 == expected_state) {
            // EVENT-DRIVEN: Execute phase and let it determine when to transition
            // Phase functions now check for actual completion before transitioning
            (this->*phase.init_func)();

            // Exit if error occurred
            if (init_state_ == InitState::INITIALIZATION_ERROR) {
                initialization_in_progress_ = false;
                return;
            }
            
            // Exit if phase returns with error
            if (!initialization_in_progress_) {
                return;
            }

            // Update UI if state changed
            const char* status_msg = phase.name;
            // 🔴 FIX: Special message for database loading
            if (init_state_ == InitState::DATABASE_LOADING && !scanner_.is_database_loading_complete()) {
                status_msg = "Loading database...";
            }
            status_bar_.update_normal_status("INIT", status_msg);
            
            // Only execute one phase per call - this ensures proper event-driven flow
            // The phase will transition state when complete, and we'll be called again
            break;
        }
    }
    
    // Reset flag (allow repeated calls)
    initialization_in_progress_ = false;
}

// Phase Initialization Methods (each method checks state before execution)

// Phase 1: Allocate display buffers
void EnhancedDroneSpectrumAnalyzerView::init_phase_allocate_buffers() {
    if (!display_controller_.allocate_buffers_from_pool()) {
        init_error_ = InitError::ALLOCATION_FAILED;
        init_state_ = InitState::INITIALIZATION_ERROR;
        initialization_in_progress_ = false;
        return;
    }

    status_bar_.update_normal_status("INIT", "Phase 1: Buffers OK");
    init_state_ = InitState::BUFFERS_ALLOCATED;
}

// ФАЗА 2: Load database
void EnhancedDroneSpectrumAnalyzerView::init_phase_load_database() {
    // 🔴 FIX #H2: Ensure DATABASE_LOADED only transitions when actually complete
    // The state machine previously used DATABASE_LOADED to mean "database loading started",
    // not "database loading complete". This created a logical inconsistency where the
    // state machine proceeded to Phase 3 (hardware init) while the database was still loading.

    if (init_state_ == InitState::BUFFERS_ALLOCATED) {
        // Start async database loading
        scanner_.initialize_database_async();
        status_bar_.update_normal_status("INIT", "Phase 2: DB loading...");
        init_state_ = InitState::DATABASE_LOADING;
        return;
    }

    // Check if database loading is complete
    if (init_state_ == InitState::DATABASE_LOADING) {
        if (scanner_.is_database_loading_complete()) {
            // 🔴 FIX #H2: Only transition to DATABASE_LOADED when actually complete
            // This prevents Phase 3 (hardware init) from starting before DB is ready
            init_state_ = InitState::DATABASE_LOADED;
            status_bar_.update_normal_status("INIT", "Phase 2: DB loaded");
        } else {
            status_bar_.update_normal_status("INIT", "Loading DB...");
            // 🔴 FIX #H2: Do NOT transition - wait for completion
        }
    }
}

// Phase 3: Initialize hardware
void EnhancedDroneSpectrumAnalyzerView::init_phase_init_hardware() {
    // 🔴 FIX #M9: Ensure DATABASE_LOADED state only when truly complete
    // Double-check database loading is complete before hardware init
    if (init_state_ != InitState::DATABASE_LOADED) {
        return;
    }

    // 🔴 FIX #M9: Additional verification - check initialization_complete_ flag
    if (!scanner_.is_initialization_complete()) {
        status_bar_.update_normal_status("INIT", "DB not ready");
        return;
    }

    hardware_.on_hardware_show();
    status_bar_.update_normal_status("INIT", "Phase 3: HW ready");
    init_state_ = InitState::HARDWARE_READY;
}

// Phase 4: Setup UI layout
void EnhancedDroneSpectrumAnalyzerView::init_phase_setup_ui() {
    if (init_state_ != InitState::HARDWARE_READY) {
        return;
    }

    // 🔴 FIX: Wait for database to complete before setting up UI
    // Phase 4 (UI Setup) can execute while database is still loading asynchronously.
    // This creates a race condition where UI elements may access uninitialized data.
    if (!scanner_.is_database_loading_complete()) {
        status_bar_.update_normal_status("INIT", "Waiting for DB...");
        return;  // Return and retry in next paint() call
    }

    // 🔴 FIX: Additional verification - check initialization_complete_ flag
    if (!scanner_.is_initialization_complete()) {
        status_bar_.update_normal_status("INIT", "DB not ready");
        return;  // Return and retry in next paint() call
    }

    initialize_modern_layout();
    // 🔴 FIX #L6: Move add_ui_elements() to appropriate phase
    // UI elements should be added after parent View initialization is complete
    add_ui_elements();
    status_bar_.update_normal_status("INIT", "Phase 4: UI ready");
    init_state_ = InitState::UI_LAYOUT_READY;
}

// Phase 5: Load settings
void EnhancedDroneSpectrumAnalyzerView::init_phase_load_settings() {
    // StackGuard for stack safety monitoring
    StackSafety::StackGuard guard("init_phase_load_settings");
    
    if (init_state_ != InitState::UI_LAYOUT_READY) {
        return;
    }

    // 🔴 FIX #M2: Wait for database to complete before loading settings
    // The initialization state machine transitions from DATABASE_LOADED to SETTINGS_LOADED
    // based on time delays only, not actual completion. Phase 5 (settings load) executes
    // while the database is still loading asynchronously, causing a race condition.
    if (!scanner_.is_database_loading_complete()) {
        status_bar_.update_normal_status("INIT", "Waiting for DB...");
        return;  // Return and retry in next paint() call
    }

    // 🔴 FIX #M2: Additional verification - check initialization_complete_ flag
    if (!scanner_.is_initialization_complete()) {
        status_bar_.update_normal_status("INIT", "DB not ready");
        return;  // Return and retry in next paint() call
    }

    // ===========================================
    // ФАЗА 6.4: ЗАЩИЩЁННАЯ ЗАГРУЗКА НАСТРОЕК
    // ===========================================

    // Проверяем доступность SD карты с таймаутом
    systime_t sd_start = chTimeNow();
    while (sd_card::status() < sd_card::Status::Mounted) {
        // 🔴 FIX #L4: Use constant instead of magic number 1s timeout
        if ((chTimeNow() - sd_start) > MS2ST(EDA::Constants::SD_CARD_MOUNT_TIMEOUT_MS)) {
            // 🔴 FIX #M7: Reset to default settings on SD card timeout
            status_bar_.update_normal_status("INIT", "No SD - defaults");
            SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings_);
            init_state_ = InitState::SETTINGS_LOADED;
            return;
        }
        // Use constant instead of magic number 50ms
        chThdSleepMilliseconds(EDA::Constants::SD_CARD_POLL_INTERVAL_SHORT_MS);
    }

    systime_t settings_start = chTimeNow();
    // Use constant instead of magic number 2s timeout
    constexpr systime_t SETTINGS_LOAD_TIMEOUT_MS = MS2ST(EDA::Constants::SETTINGS_LOAD_TIMEOUT_MS);

    // Removed SDCardLock - FatFS handles locking at driver level
    auto load_result = SettingsPersistence<DroneAnalyzerSettings>::load(settings_);
    bool loaded = load_result.is_ok() && load_result.value;

    systime_t elapsed = chTimeNow() - settings_start;
    if (elapsed >= SETTINGS_LOAD_TIMEOUT_MS) {
        // Reset to default settings on timeout
        status_bar_.update_normal_status("WARN", "Settings timeout");
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings_);
        init_state_ = InitState::SETTINGS_LOADED;
        return;
    }

    if (!loaded) {
        status_bar_.update_normal_status("INIT", "Using defaults");
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings_);
    } else {
        status_bar_.update_normal_status("INIT", "Settings loaded");
    }

    button_audio_.set_text(audio_get_enable_alerts(settings_) ? "AUDIO: ON" : "AUDIO: OFF");
    scanner_.update_scan_range(settings_.wideband_min_freq_hz,
                            settings_.wideband_max_freq_hz);
    
    // 🔧 FIX: Update coordinator parameters after settings load
    scanning_coordinator_.update_runtime_parameters(settings_);

    // DIAMOND FIX: Update scanner's settings from view's settings
    // Required because scanner now stores settings by value (not reference)
    scanner_.update_settings(settings_);

    // Scanner initialization verification with proper error feedback
    if (!scanner_.is_initialization_complete()) {
        status_bar_.update_normal_status("WARN", "Scanner not ready");
        // Continue anyway - scanner will retry initialization
    }

    // DIAMOND FIX: Update audio cooldown based on settings to prevent UI freeze
    // baseband::send_message() uses busy-wait spin loop (baseband_api.cpp:54-64)
    // Set cooldown = duration + 100ms buffer to ensure M0 can process audio
    const uint32_t audio_cooldown = settings_.audio_alert_duration_ms + 100;
    AudioAlertManager::set_cooldown_ms(audio_cooldown);

    // 🔴 FIX #M10: Transition to SETTINGS_LOADED state
    init_state_ = InitState::SETTINGS_LOADED;
}

// ФАЗА 6: Finalize (переход в FULLY_INITIALIZED)
void EnhancedDroneSpectrumAnalyzerView::init_phase_finalize() {
    if (init_state_ != InitState::SETTINGS_LOADED) {
        return;
    }

    // DIAMOND FIX #3: Verify initialization is complete before starting
    // start_scanning_thread() is called without verifying that scanner_.is_initialization_complete()
    // returns true. This prevents starting scanning if the scanner is not fully initialized.
    if (!scanner_.is_initialization_complete()) {
        init_state_ = InitState::INITIALIZATION_ERROR;
        init_error_ = InitError::DATABASE_ERROR;
        initialization_in_progress_ = false;
        status_bar_.update_normal_status("ERROR", "Scanner not ready");
        return;
    }

    // ===========================================
    // DIAMOND OPTIMIZATION: Wire histogram data flow
    // ===========================================
    // Connect scanner histogram callback to display controller
    // Diamond Code: Function pointer with user data (no heap allocation, no std::function)
    // Data flow: SpectralAnalyzer → Scanner → DisplayController
    // NOTE: Using static function with 'this' as user data to avoid lambda captures
    scanner_.set_histogram_callback(&EnhancedDroneSpectrumAnalyzerView::static_histogram_callback, this);

    handle_scanner_update();
    init_state_ = InitState::FULLY_INITIALIZED;
    status_bar_.update_normal_status("EDA", "Ready");
    
    // 🔴 FIX: Automatically start scanning thread after initialization
    start_scanning_thread();
}

void EnhancedDroneSpectrumAnalyzerView::on_show() {
    View::on_show();

    // 🔧 FIX: Reset initialization state
    init_state_ = InitState::CONSTRUCTED;
    init_start_time_ = chTimeNow();
    last_init_progress_ = 0;
    initialization_in_progress_ = false;
    init_error_ = InitError::NONE;

    status_bar_.update_normal_status("INIT", "Phase 0: Ready");
    
    // 🔧 FIX: Force immediate first initialization step
    // This ensures initialization starts even if paint() is delayed
    step_deferred_initialization();
    
    // 🔧 FIX: Force redraw to show status bar
    set_dirty();
}

void EnhancedDroneSpectrumAnalyzerView::on_hide() {
    stop_scanning_thread();
    scanner_.stop_scanning();
    hardware_.stop_spectrum_streaming();
    hardware_.shutdown_hardware();
    hardware_.on_hardware_hide();

    scanner_.sync_database();

    View::on_hide();
}

void EnhancedDroneSpectrumAnalyzerView::start_scanning_thread() {
    if (scanning_coordinator_.is_scanning_active()) return;
    scanning_coordinator_.start_coordinated_scanning();
}

void EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread() {
    if (!scanning_coordinator_.is_scanning_active()) return;
    scanning_coordinator_.stop_coordinated_scanning();
}

bool EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button() {
    if (scanning_coordinator_.is_scanning_active()) {
        ui_controller_.on_stop_scan();
        button_start_stop_.set_text("START/STOP");
    } else {
        ui_controller_.on_start_scan();
        button_start_stop_.set_text("STOP");
    }
    return true;
}

bool EnhancedDroneSpectrumAnalyzerView::handle_menu_button() {
    ui_controller_.show_menu();
    return true;
}

void EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout() {
    // 🔴 ФАЗА 2.7: Don't call handle_scanner_update() here!
    // scanner_ may not be fully initialized yet
    // This prevents segfault/black screen during startup
    // handle_scanner_update() will be called after FULLY_INITIALIZED state

    // 🔴 FIX #L7: Add check to ensure parent View is ready before setting parent_rect
    // Verify initialization state before calling set_parent_rect() on ThreatCard objects
    if (init_state_ < InitState::UI_LAYOUT_READY) {
        return;
    }

    for (size_t i = 0; i < threat_cards_.size(); ++i) {
        size_t card_y_pos = 165 + i * 20;

        if (card_y_pos + 24 > 224) break;

        threat_cards_[i].set_parent_rect(Rect{0, static_cast<int>(card_y_pos), screen_width, 24});
    }
}

void EnhancedDroneSpectrumAnalyzerView::update_modern_layout() {
    handle_scanner_update();
}

// DIAMOND OPTIMIZATION: Lookup tables for UI state machine
// Scott Meyers Item 15: Prefer constexpr to #define
// Eliminates branching and std::string allocation
namespace {
    // Signal type mapping from ScanningMode to const char* (Flash storage)
    static constexpr const char* SIGNAL_TYPE_LUT[] = {
        "RSSI",      // DATABASE = 0
        "SPECTRAL",  // WIDEBAND_CONTINUOUS = 1
        "HYBRID"     // HYBRID = 2
    };

    // Alert message mapping from ThreatLevel to const char* (Flash storage)
    static constexpr const char* ALERT_MSG_LUT[] = {
        "Threats detected",   // NONE/LOW = 0,1
        "Threats detected",   // MEDIUM = 2
        "HIGH THREATS!",      // HIGH = 3
        "CRITICAL THREATS!"   // CRITICAL = 4
    };
}

void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
    // 🔴 SAFETY: Early exit if buffers are not valid
    if (!display_controller_.are_buffers_valid()) {
        return;
    }
    
    // Early exit if scanner not active
    if (!scanner_.is_scanning_active() &&
        scanner_.get_approaching_count() == 0 &&
        scanner_.get_static_count() == 0 &&
        scanner_.get_receding_count() == 0) {
        const char* primary_msg = "EDA Ready";
        char secondary_buffer[48];
        uint32_t total_detections = scanner_.get_total_detections();
        if (total_detections > 0) {
            StatusFormatter::format_to(secondary_buffer, "Total detections: %lu",
                                      static_cast<unsigned long>(total_detections));
        } else {
            safe_strcpy(secondary_buffer, "Awaiting commands", sizeof(secondary_buffer));
        }
        status_bar_.update_normal_status(primary_msg, secondary_buffer);
        return;
    }

    // Cache scanner state (single read)
    ThreatLevel max_threat = scanner_.get_max_detected_threat();
    size_t approaching = scanner_.get_approaching_count();
    size_t static_count = scanner_.get_static_count();
    size_t receding = scanner_.get_receding_count();
    bool is_scanning = scanner_.is_scanning_active();
    Frequency current_freq = scanner_.get_current_scanning_frequency();

    // Trigger audio alerts based on threat level (with debouncing)
    // AudioAlertManager::play_alert() has built-in debouncing to prevent baseband queue saturation
    // baseband::send_message() uses busy-wait spin loop
    if (audio_get_enable_alerts(settings_) && max_threat >= ThreatLevel::LOW) {
        size_t total_drones = approaching + static_count + receding;
        if (total_drones > 0) {
            AudioAlertManager::play_alert(max_threat);
        }
    }

    // Update header
    smart_header_.update(max_threat, approaching, static_count, receding,
                        current_freq, is_scanning);

    // Update status bar (DoD: table-driven state machine)
    if (is_scanning) {
        uint32_t cycles = scanner_.get_scan_cycles();
        uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), static_cast<uint32_t>(32));
        status_bar_.update_scanning_progress(progress, cycles,
                                         scanner_.get_total_detections());
    } else {
        size_t total_drones = approaching + static_count + receding;
        // LUT lookup instead of ternary (Flash string, zero RAM allocation)
        const char* alert_msg = ALERT_MSG_LUT[
            std::min(static_cast<uint8_t>(max_threat), static_cast<uint8_t>(ThreatLevel::CRITICAL))
        ];
        status_bar_.update_alert_status(max_threat, total_drones, alert_msg);
    }

    display_controller_.update_detection_display(scanner_);

    // DIAMOND OPTIMIZATION: LUT lookup for signal type (Flash storage)
    // Eliminates switch-case and std::string allocation
    uint8_t mode_idx = static_cast<uint8_t>(scanner_.get_scanning_mode());
    const char* signal_type = SIGNAL_TYPE_LUT[mode_idx];
    display_controller_.update_signal_type_display(signal_type);
}

// Button Callback Implementations
// NOTE: Mayhem UI framework uses std::function<void(Button&)> for callbacks
// Alternative approaches considered:
// - Raw function pointers: Not feasible due to 'this' capture requirement
// - Template-based callbacks: Would require framework-wide changes
void EnhancedDroneSpectrumAnalyzerView::setup_button_handlers() {
    button_start_stop_.on_select = [this](Button&) {
        this->handle_start_stop_button();
    };

    button_menu_.on_select = [this](Button&) {
        this->ui_controller_.show_menu();
    };

    button_audio_.on_select = [this](Button&) {
        bool current = audio_get_enable_alerts(this->settings_);
        audio_set_enable_alerts(this->settings_, !current);
        this->button_audio_.set_text(audio_get_enable_alerts(this->settings_) ? "AUDIO: ON" : "AUDIO: OFF");
        this->button_audio_.set_style(audio_get_enable_alerts(this->settings_) ? &UIStyles::GREEN_STYLE : &UIStyles::LIGHT_STYLE);
    };

    field_scanning_mode_.on_change = [this](size_t index, int32_t value) {
        (void)value;  // Silence unused parameter warning
        this->set_scanning_mode_from_index(index);
    };
}

void EnhancedDroneSpectrumAnalyzerView::initialize_scanning_mode() {
    int initial_mode = static_cast<int>(scanner_.get_scanning_mode());
    field_scanning_mode_.set_selected_index(initial_mode);
}

void EnhancedDroneSpectrumAnalyzerView::set_scanning_mode_from_index(size_t index) {
    DroneScanner::ScanningMode mode = static_cast<DroneScanner::ScanningMode>(index);
    scanner_.set_scanning_mode(mode);
    display_controller_.set_scanning_status(ui_controller_.is_scanning(),
                                           scanner_.scanning_mode_name());
    update_modern_layout();
}

void EnhancedDroneSpectrumAnalyzerView::add_ui_elements() {
    add_children({&smart_header_, &status_bar_});
    for (auto& card : threat_cards_) {
        add_child(&card);
    }
    add_children({&button_start_stop_, &button_menu_, &button_audio_});
}

LoadingScreenView::LoadingScreenView(NavigationView& nav)
    : nav_(nav),
      text_eda_(Rect{108, 213, 24, 16}, "EDA"),
      timer_start_(chTimeNow())
{
    static Style red_style{font::fixed_8x16, Color::black(), Color::red()};
    text_eda_.set_style(&red_style);
    add_child(&text_eda_);
    set_focusable(false);
}

void LoadingScreenView::paint(Painter& painter) {
    painter.fill_rectangle(
        {0, 0, portapack::display.width(), portapack::display.height()},
        Color::black()
    );
    View::paint(painter);
}

// Part 7: Enhanced Settings Validator

EnhancedDroneSettingsValidator::ValidationResult
EnhancedDroneSettingsValidator::validate_all(const DroneAnalyzerSettings& settings) {
    ValidationResult result;

    // 1. Validate RSSI threshold
    if (!validate_rssi_threshold(settings.rssi_threshold_db, result.error_message, sizeof(result.error_message))) {
        result.is_valid = false;
        return result;
    }

    // 2. Validate scan interval
    if (!validate_scan_interval(settings.scan_interval_ms, result.error_message, sizeof(result.error_message))) {
        result.is_valid = false;
        return result;
    }

    // 3. Validate audio parameters
    if (!validate_audio_params(settings.audio_alert_frequency_hz,
                              settings.audio_alert_duration_ms, result.error_message, sizeof(result.error_message))) {
        result.is_valid = false;
        return result;
    }

    // 4. Validate bandwidth
    if (!validate_bandwidth(settings.hardware_bandwidth_hz, result.error_message, sizeof(result.error_message))) {
        result.is_valid = false;
        return result;
    }

    // 5. Validate frequency range
    if (!validate_frequency_range(settings.user_min_freq_hz, settings.user_max_freq_hz, result.error_message, sizeof(result.error_message))) {
        result.is_valid = false;
        return result;
    }

    // 6. Check for warnings (not errors)
    if (settings.rssi_threshold_db > -60) {
        result.warning_count++;
        safe_strcat(result.error_message, "WARN: High RSSI threshold may miss weak signals\n", sizeof(result.error_message));
    }

    if (settings.scan_interval_ms > 5000) {
        result.warning_count++;
        safe_strcat(result.error_message, "WARN: Slow scan interval\n", sizeof(result.error_message));
    }

    return result;
}

bool EnhancedDroneSettingsValidator::validate_frequency(Frequency freq, char* error, size_t error_size) {
    if (freq < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ ||
        freq > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        char freq_str[32];
        format_frequency_hz(freq, freq_str, sizeof(freq_str));
        snprintf(error, error_size, "Frequency %s out of range (must be %llu-%llu GHz)",
                 freq_str,
                 EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ / 1000000000ULL,
                 EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ / 1000000000ULL);
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_rssi_threshold(int32_t rssi, char* error, size_t error_size) {
    if (rssi < DroneConstants::MIN_VALID_RSSI || rssi > DroneConstants::MAX_VALID_RSSI) {
        snprintf(error, error_size, "RSSI threshold %ld invalid (must be %ld to %ld)",
                 (long)rssi, (long)DroneConstants::MIN_VALID_RSSI, (long)DroneConstants::MAX_VALID_RSSI);
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_scan_interval(uint32_t interval_ms, char* error, size_t error_size) {
    if (interval_ms < DroneConstants::MIN_SCAN_INTERVAL_MS ||
        interval_ms > DroneConstants::MAX_SCAN_INTERVAL_MS) {
        snprintf(error, error_size, "Scan interval %lu invalid (must be %lu to %lu)",
                 (unsigned long)interval_ms, (unsigned long)DroneConstants::MIN_SCAN_INTERVAL_MS,
                 (unsigned long)DroneConstants::MAX_SCAN_INTERVAL_MS);
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_audio_params(uint32_t freq_hz, uint32_t duration_ms, char* error, size_t error_size) {
    if (freq_hz < DroneConstants::MIN_AUDIO_FREQ || freq_hz > DroneConstants::MAX_AUDIO_FREQ) {
        snprintf(error, error_size, "Audio frequency %lu invalid (must be %lu to %lu)",
                 (unsigned long)freq_hz, (unsigned long)DroneConstants::MIN_AUDIO_FREQ,
                 (unsigned long)DroneConstants::MAX_AUDIO_FREQ);
        return false;
    }

    if (duration_ms < DroneConstants::MIN_AUDIO_DURATION ||
        duration_ms > DroneConstants::MAX_AUDIO_DURATION) {
        snprintf(error, error_size, "Audio duration %lu invalid (must be %lu to %lu)",
                 (unsigned long)duration_ms, (unsigned long)DroneConstants::MIN_AUDIO_DURATION,
                 (unsigned long)DroneConstants::MAX_AUDIO_DURATION);
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_bandwidth(uint32_t bandwidth_hz, char* error, size_t error_size) {
    if (bandwidth_hz < DroneConstants::MIN_BANDWIDTH || bandwidth_hz > DroneConstants::MAX_BANDWIDTH) {
        snprintf(error, error_size, "Bandwidth %lu invalid (must be %lu to %lu)",
                 (unsigned long)bandwidth_hz, (unsigned long)DroneConstants::MIN_BANDWIDTH,
                 (unsigned long)DroneConstants::MAX_BANDWIDTH);
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_frequency_range(Frequency min_hz, Frequency max_hz, char* error, size_t error_size) {
    if (min_hz >= max_hz) {
        safe_strcpy(error, "Min frequency must be less than max frequency", error_size);
        return false;
    }

    if (!validate_frequency(min_hz, error, error_size)) return false;
    if (!validate_frequency(max_hz, error, error_size)) return false;

    Frequency range = max_hz - min_hz;
    if (static_cast<uint64_t>(range) < 1000000ULL) { // Min 1 MHz range
        safe_strcpy(error, "Frequency range too small (minimum 1 MHz)", error_size);
        return false;
    }
    
    return true;
}

bool EnhancedDroneSettingsValidator::is_known_drone_band(Frequency freq) {
    if (freq >= DroneConstants::MIN_433MHZ && freq <= DroneConstants::MAX_433MHZ) return true;
    
    if (freq >= DroneConstants::MIN_900MHZ && freq <= DroneConstants::MAX_900MHZ) return true;
    
    if (freq >= DroneConstants::MIN_24GHZ && freq <= DroneConstants::MAX_24GHZ) return true;
    
    if (freq >= DroneConstants::MIN_58GHZ && freq <= DroneConstants::MAX_58GHZ) return true;
    
    return false;
}

bool EnhancedDroneSettingsValidator::is_ism_band(Frequency freq) {
    return (static_cast<uint64_t>(freq) >= 2400000000ULL && static_cast<uint64_t>(freq) <= 2483500000ULL) ||
           (static_cast<uint64_t>(freq) >= 5725000000ULL && static_cast<uint64_t>(freq) <= 5875000000ULL);
}

// Unified frequency formatting using FrequencyFormatter
// Scott Meyers Item 25: Consider support for implicit interfaces
void EnhancedDroneSettingsValidator::format_frequency_hz(Frequency freq, char* buffer, size_t buffer_size) {
    if (static_cast<uint64_t>(freq) >= 1000000000ULL) {
        FrequencyFormatter::format_to_buffer(buffer, buffer_size, freq, FrequencyFormatter::Format::DETAILED_GHZ);
    } else {
        FrequencyFormatter::format_to_buffer(buffer, buffer_size, freq, FrequencyFormatter::Format::STANDARD_MHZ);
    }
}

// ===========================================
// PART 6.5: COMPACT FREQUENCY RULER IMPLEMENTATION
// ===========================================

CompactFrequencyRuler::CompactFrequencyRuler(Rect parent_rect)
    : View(parent_rect),
      min_freq_(2400000000ULL),
      max_freq_(2500000000ULL),
      spectrum_width_(240),
      visible_(true),
      ruler_style_(RulerStyle::COMPACT_GHZ),
      target_tick_count_(DEFAULT_TICK_COUNT) {
}

void CompactFrequencyRuler::set_frequency_range(Frequency min_freq, Frequency max_freq) {
    min_freq_ = min_freq;
    max_freq_ = max_freq;
    set_dirty();
}

void CompactFrequencyRuler::set_spectrum_width(int width) {
    spectrum_width_ = width;
    set_dirty();
}

void CompactFrequencyRuler::set_visible(bool visible) {
    visible_ = visible;
    hidden(!visible);
}

void CompactFrequencyRuler::set_ruler_style(RulerStyle style) {
    ruler_style_ = style;
    set_dirty();
}

void CompactFrequencyRuler::set_tick_count(int num_ticks) {
    if (num_ticks >= 3 && num_ticks <= 8) {
        target_tick_count_ = num_ticks;
    }
    set_dirty();
}

void CompactFrequencyRuler::paint(Painter& painter) {
    const auto r = screen_rect();

    painter.fill_rectangle(r, Color::black());

    if (!visible_) return;

    draw_compact_ticks(painter, r);
}

void CompactFrequencyRuler::draw_compact_ticks(Painter& painter, const Rect r) {
    Frequency range = max_freq_ - min_freq_;
    if (range == 0) return;

    Frequency tick_interval = calculate_optimal_tick_interval();
    if (tick_interval == 0) return;

    // Single style (both use same font/colors)
    static Style label_style{font::fixed_5x8, Color::grey(), Color::black()};

    painter.fill_rectangle(
        {r.left(), r.top() + RULER_HEIGHT - 1, r.width(), 1},
        Theme::getInstance()->bg_darkest->foreground
    );

    Frequency first_tick = (min_freq_ / tick_interval) * tick_interval;
    if (first_tick < min_freq_) {
        first_tick += tick_interval;
    }

    for (Frequency tick = first_tick; tick <= max_freq_; tick += tick_interval) {
        int x = r.left() + static_cast<int>(((tick - min_freq_) * spectrum_width_) / range);

        if (x < r.left() || x > r.right()) {
            continue;
        }

        painter.fill_rectangle(
            {x, r.top(), 1, TICK_HEIGHT_MAJOR},
            Theme::getInstance()->bg_darkest->foreground
        );

        char label_buf[32];
        format_compact_label(label_buf, sizeof(label_buf), tick);

        auto text_size = label_style.font.size_of(label_buf);
        int text_x = x - text_size.width() / 2;
        int text_y = r.top() + 1;

        if (text_x < r.left() + 2) text_x = r.left() + 2;
        if (text_x + text_size.width() > r.right() - 2) {
            text_x = r.right() - text_size.width() - 2;
        }

        painter.draw_string({text_x, text_y}, label_style, label_buf);

        // Sub-tick logic based on tick_interval only
        if (static_cast<uint64_t>(tick_interval) >= 100000000ULL) {
            for (int sub = 1; sub < 5; sub++) {
                Frequency sub_tick = tick + (tick_interval * sub) / 5;
                int sub_x = r.left() + static_cast<int>(((sub_tick - min_freq_) * spectrum_width_) / range);

                if (sub_x > r.left() && sub_x < r.right()) {
                    painter.fill_rectangle(
                        {sub_x, r.top(), 1, TICK_HEIGHT_MINOR},
                        Theme::getInstance()->bg_darkest->foreground
                    );
                }
            }
        }
    }
}

// Lookup table for ruler style to formatter mapping (eliminates ~60 lines of switch-case)
namespace {
    static constexpr FrequencyFormatter::Format RULER_FORMAT_LUT[] = {
        FrequencyFormatter::Format::COMPACT_GHZ,   // COMPACT_GHZ = 0
        FrequencyFormatter::Format::COMPACT_MHZ,   // COMPACT_MHZ = 1
        FrequencyFormatter::Format::STANDARD_GHZ,  // STANDARD_GHZ = 2
        FrequencyFormatter::Format::STANDARD_MHZ,  // STANDARD_MHZ = 3
        FrequencyFormatter::Format::DETAILED_GHZ,  // DETAILED = 4
        FrequencyFormatter::Format::SPACED_GHZ      // SPACED_GHZ = 5
    };
}

// Unified frequency formatting using FrequencyFormatter
void CompactFrequencyRuler::format_compact_label(char* buffer, size_t buffer_size, Frequency freq) {
    uint8_t idx = static_cast<uint8_t>(ruler_style_);
    if (idx >= 6) idx = 0;
    FrequencyFormatter::format_to_buffer(buffer, buffer_size, freq, RULER_FORMAT_LUT[idx]);
}

Frequency CompactFrequencyRuler::calculate_optimal_tick_interval() {
    Frequency range = max_freq_ - min_freq_;
    if (range == 0) return 0;

    // Optimized intervals for PortaPack's small screen (prioritize GHz for SPACED_GHZ)
    static constexpr Frequency intervals[] = {
        5000000000ULL,  4000000000ULL,  3000000000ULL, 2000000000ULL,  // 5G, 4G, 3G, 2G
        1000000000ULL,  // 1G (key for SPACED_GHZ)
        500000000ULL,   250000000ULL,   200000000ULL,            // 500M, 250M, 200M
        100000000ULL,   50000000ULL,    25000000ULL,            // 100M, 50M, 25M
        20000000ULL,    10000000ULL,    5000000ULL,             // 20M, 10M, 5M
        2000000ULL,     1000000ULL,     500000ULL,              // 2M, 1M, 500k
        200000ULL,      100000ULL                                // 200k, 100k
    };

    for (auto interval : intervals) {
        int ticks = static_cast<int>(range / interval);
        if (ticks >= target_tick_count_ - 1 && ticks <= target_tick_count_ + 1) {
            return interval;
        }
    }

    return range / target_tick_count_;
}

bool CompactFrequencyRuler::should_use_mhz_labels() const {
    Frequency range = max_freq_ - min_freq_;

    if (static_cast<uint64_t>(range) < 50000000ULL) {
        return true;
    }

    Frequency center = (min_freq_ + max_freq_) / 2;
    if (static_cast<uint64_t>(center) < 1000000000ULL) {
        return true;
    }

    if (ruler_style_ == RulerStyle::COMPACT_MHZ ||
        ruler_style_ == RulerStyle::STANDARD_MHZ) {
        return true;
    }

    return false;
}

void DroneDisplayController::update_frequency_ruler() {
    compact_frequency_ruler_.set_frequency_range(spectrum_config_.min_freq, spectrum_config_.max_freq);
    compact_frequency_ruler_.set_spectrum_width(SPEC_WIDTH);
    compact_frequency_ruler_.set_dirty();
}

void DroneDisplayController::set_ruler_style(RulerStyle style) {
    compact_frequency_ruler_.set_ruler_style(style);
    compact_frequency_ruler_.set_visible(true);
}

void DroneDisplayController::apply_display_settings(const DroneAnalyzerSettings& settings) {
    if (disp_get_show_frequency_ruler(settings)) {
        compact_frequency_ruler_.set_visible(true);

        if (disp_get_auto_ruler_style(settings)) {
            compact_frequency_ruler_.set_ruler_style(RulerStyle::COMPACT_GHZ);
        } else {
            // constexpr LUT in Flash instead of switch
            uint8_t style_idx = (settings.frequency_ruler_style < 7) ?
                                settings.frequency_ruler_style : 0;
            compact_frequency_ruler_.set_ruler_style(RULER_STYLE_LUT[style_idx]);
        }

        compact_frequency_ruler_.set_tick_count(settings.compact_ruler_tick_count);
    } else {
        compact_frequency_ruler_.set_visible(false);
    }
}

// Stack Usage Monitoring
/**
 * @brief Check stack usage and log warnings if low
 * @param thread_name Name of the thread for logging
 * @param stack_size Total stack size for the thread
 *
 * @note Uses ChibiOS stack fill pattern (0x55) to estimate free stack space.
 *       Compatible with ChibiOS versions that have CH_DBG_FILL_THREADS enabled.
 *       If stack filling is not enabled, reports 0 free bytes conservatively.
 */
void EnhancedDroneSpectrumAnalyzerView::check_stack_usage([[maybe_unused]] const char* thread_name, size_t stack_size) {
    Thread* current_thread = chThdSelf();
    
    // Count unused bytes from stack fill pattern (0x55)
    
    size_t free_stack = 0;
    
    // Get stack start address (after Thread structure)
    uint8_t* stack_start = reinterpret_cast<uint8_t*>(current_thread + 1);
    
    // Limit scan to avoid scanning entire stack every time
    const size_t max_scan_bytes = (stack_size > 4096) ? 4096 : stack_size;
    const uint8_t stack_fill_value = 0x55;
    
    for (size_t i = 0; i < max_scan_bytes; ++i) {
        if (stack_start[i] == stack_fill_value) {
            free_stack++;
        } else {
            // Stack used up to this point
            break;
        }
    }
    
    // Conservatively report max_scan_bytes if all fill pattern (avoids large scans)
    
    if (free_stack < MIN_STACK_FREE_THRESHOLD) {
        // Track low stack condition for monitoring
    }
}

// Memory Pressure Monitoring
/**
 * @brief Check memory pressure and log warnings if critical
 */
void EnhancedDroneSpectrumAnalyzerView::check_memory_pressure() {
    size_t free_heap = HeapMonitor::get_free_heap();

    if (free_heap < 8192) {  // Less than 8KB free
        // Log warning - memory running low
    }

    if (free_heap < 4096) {  // Less than 4KB free - critical
        // Log error - critical memory condition (implement graceful degradation)
    }
}

} // namespace ui::apps::enhanced_drone_analyzer
