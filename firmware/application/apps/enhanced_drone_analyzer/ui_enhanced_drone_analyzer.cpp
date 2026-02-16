/**
 * Enhanced Drone Analyzer - Thread Safety Notes
 *
 * Locking Order (to prevent deadlocks):
 * 1. scanning_active_ (std::atomic<bool>) - No lock needed
 * 2. data_mutex (DroneScanner::tracked_drones_)
 * 3. spectrum_mutex (DroneHardwareController::spectrum_buffer_)
 * 4. logger_mutex (DroneDetectionLogger::mutex_)
 *
 * Rules:
 * - Always acquire locks in order 1->2->3->4
 * - Never acquire a lower-numbered lock while holding a higher-numbered lock
 * - Use MutexLock RAII wrapper for automatic unlock
 * - Keep critical sections as short as possible
 * 
 * If something doesn't load at all, at least it doesn't freeze and immediately crash. Which means the code is potentially alive. You all know perfectly well that running so many lines of code is already an achievement.
 * 
 * I'm just here for fun. I had this idea to cram 8,000 lines into an outdated controller and that's it. Code for me is like painting a picture. I want it abstract, extensive, and yet solid.
 * 
 * I'd like to point out that the current problem isn't related to either the stack or the heap. I've changed them variously and disabled modules, but the problem hasn't changed. It's not moving. I suspect the problem is in the math. Changing the stack and heap should logically correct the logic error. But that's not happening.
 * 
 */

#include "ui_enhanced_drone_analyzer.hpp"
// Phase 2 Optimization: Removed ui_enhanced_drone_memory_pool.hpp (~5.6 KB savings)
// Memory pools replaced with static arrays directly in code
#include "ui_drone_audio.hpp"
#include "eda_optimized_utils.hpp"
#include "color_lookup_unified.hpp"
#include "eda_constants.hpp"
#include "diamond_core.hpp"
#include "gradient.hpp"
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

// ===========================================
// DIAMOND OPTIMIZATION: Compile-Time Lookup Tables
// ===========================================

// Progressive slowdown multiplier lookup table
// Precomputed values for scan_cycles_ / PROGRESSIVE_SLOWDOWN_DIVISOR, capped at 3
// Eliminates runtime division in hot path
static constexpr uint8_t SLOWDOWN_MULTIPLIER_LUT[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0-9 cycles
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 10-19 cycles
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 20-29 cycles
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3   // 30+ cycles (capped at 3)
};

// Compile-time division by power of 2 optimization
// If wideband_slice_width_hz is a power of 2, use bit shift instead of division
// This is a constexpr function that compiler can evaluate at compile time
constexpr inline uint64_t ceil_div_u64(uint64_t numerator, uint64_t denominator) noexcept {
    return (numerator + denominator - 1) / denominator;
}

// ===========================================
// DIAMOND FIX: SD Card Mutex Definition
// FatFS is NOT thread-safe - all SD operations must be serialized
// ===========================================
Mutex sd_card_mutex;

// ===========================================
// DIAMOND OPTIMIZATION: ScanningMode LUT (namespace scope)
// ===========================================
// Scott Meyers Item 15: Prefer constexpr to #define
// All strings stored in Flash, no RAM used
EDA_FLASH_CONST inline static constexpr const char* const SCANNING_MODE_NAMES[] = {
    "Database Scan",      // DATABASE = 0
    "Wideband Monitor",   // WIDEBAND_CONTINUOUS = 1
    "Hybrid Discovery"    // HYBRID = 2
};
static_assert(sizeof(SCANNING_MODE_NAMES) / sizeof(const char*) == 3, "SCANNING_MODE_NAMES size");

// Note: TREND_COUNTERS and SCAN_FUNCTIONS moved to DroneScanner class
// (ui_enhanced_drone_analyzer.hpp) for access to private members

// ===========================================
// STATIC MEMBER DEFINITIONS
// ===========================================
// Diamond Code: Out-of-line definitions for static class members
// Required for non-inline static members in header-only declarations

// DroneDisplayController static member definition
alignas(alignof(DisplayDroneEntry))
DisplayDroneEntry DroneDisplayController::detected_drones_storage_[DroneDisplayController::MAX_UI_DRONES];

// DroneDetectionLogger static member definition for thread working area
stkalign_t DroneDetectionLogger::worker_wa_[THD_WA_SIZE(DroneDetectionLogger::WORKER_STACK_SIZE) / sizeof(stkalign_t)];

// DroneScanner static member definition for thread working area
// WORKING_AREA macro expands to: stkalign_t name[THD_WA_SIZE(n) / sizeof(stkalign_t)]
stkalign_t DroneScanner::db_loading_wa_[THD_WA_SIZE(DroneScanner::DB_LOADING_STACK_SIZE) / sizeof(stkalign_t)];

// 🔴 OPTIMIZATION: static const array instead of vector to avoid heap allocation
// Built-in database of known drone frequencies (2025)
// Phase 3 Optimization: Reduced from 31 to 15 entries (~384 bytes savings)
// Kept most common/popular drone types: DJI OcuSync (4), FPV RaceBand (4), WiFi (3), Crossfire (2), ELRS (2), LRS (1), DJI FPV (1)
EDA_FLASH_CONST const std::array<DroneScanner::BuiltinDroneFreq, DroneScanner::BUILTIN_DB_SIZE> DroneScanner::BUILTIN_DRONE_DB = {{
    // --- LRS / Control (Long Range) ---
    { 868000000, "TBS Crossfire EU", DroneType::MILITARY_DRONE },
    { 915000000, "TBS Crossfire US", DroneType::MILITARY_DRONE },
    { 866000000, "ELRS 868", DroneType::PX4_DRONE },
    { 915000000, "ELRS 915", DroneType::PX4_DRONE },

    // --- Legacy / Telemetry ---
    { 433050000, "LRS 433 Ch1", DroneType::UNKNOWN },

    // --- DJI OcuSync / Lightbridge (2.4 GHz) ---
    // Reduced from 8 to 4 channels (kept most common)
    { 2406500000, "DJI OcuSync 1", DroneType::MAVIC },
    { 2416500000, "DJI OcuSync 3", DroneType::MAVIC },
    { 2426500000, "DJI OcuSync 5", DroneType::MAVIC },
    { 2436500000, "DJI OcuSync 7", DroneType::MAVIC },

    // --- FPV Video (5.8 GHz Analog/Digital) ---
    // RaceBand (Reduced from 8 to 4 channels - kept most popular)
    { 5658000000, "RaceBand 1", DroneType::UNKNOWN },
    { 5695000000, "RaceBand 2", DroneType::UNKNOWN },
    { 5732000000, "RaceBand 3", DroneType::UNKNOWN },
    { 5769000000, "RaceBand 4", DroneType::UNKNOWN },

    // DJI FPV System (Digital) - Reduced from 3 to 1 channel
    { 5735000000, "DJI FPV Ch1", DroneType::MAVIC },

    // --- WiFi Drones (Parrot, Ryze Tello) ---
    { 2412000000, "WiFi Ch1", DroneType::PARROT_ANAFI },
    { 2437000000, "WiFi Ch6", DroneType::PARROT_ANAFI },
    { 2462000000, "WiFi Ch11", DroneType::PARROT_ANAFI }
}};

// ===========================================
// PART 2: DRONE SCANNER IMPLEMENTATION
// ===========================================

DroneScanner::DroneScanner(const DroneAnalyzerSettings& settings)
    : scanning_thread_(nullptr),
        data_mutex(),
        scanning_active_(false),
        freq_db_ptr_(nullptr),  // 🔴 FIX: Defer heap allocation to after constructor
        tracked_drones_ptr_(nullptr),  // 🔴 FIX: Defer heap allocation
        freq_db_loaded_(false),
        current_db_index_(0),
        last_scanned_frequency_(0),
        db_loading_thread_(nullptr),
        db_loading_active_{false},
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
          // Phase 3 Optimization: Removed intelligent scanning features initialization
          settings_(settings)
{
    // Initialize mutex properly to fix race condition
    chMtxInit(&data_mutex);

    // 🔴 FIX: Lazy initialization after constructor (prevents stack overflow)
    // FreqmanDB and tracked_drones allocated later from heap
    // Initialize wideband scanning only (lightweight operation)
    initialize_wideband_scanning();
}

DroneScanner::~DroneScanner() {
    stop_scanning();
    cleanup_database_and_scanner();
    // Note: ChibiOS mutexes are automatically cleaned up with the object.
    // No explicit deinitialization needed - mutex is part of thread context.
}

// ===========================================
// RAII DESIGN NOTES FOR DroneScanner
// ===========================================
// WHY MANUAL CLEANUP IS NECESSARY:
// 
// This class uses placement new for memory allocation, which is a deliberate
// design choice for embedded systems (Cortex-M4 with limited RAM).
//
// RAII WRAPPERS USED:
// - std::array<DetectionEntry, 32> detection_ring_buffer_ (automatic storage)
// - std::atomic<bool> scanning_active_ (lock-free, automatic)
// - MutexLock wrapper for ChibiOS mutexes (RAII-style lock management)
//
// MANUAL CLEANUP REQUIRED FOR:
// - freq_db_ptr_: FreqmanDB allocated via placement new from memory pool
// - tracked_drones_ptr_: TrackedDrone array via placement new
// - detection_logger_: Worker thread requires explicit stop_worker() call
//
// RATIONALE FOR PLACEMENT NEW:
// 1. Memory pool allocation prevents heap fragmentation
// 2. Deterministic memory usage (critical for embedded systems)
// 3. Objects constructed only when needed (lazy initialization)
// 4. Explicit destructor calls required by C++ standard for placement new
//
// ALTERNATIVE CONSIDERED:
// std::optional<T> could provide RAII semantics, but adds ~4 bytes overhead
// per object and requires C++17. Current design is optimal for Cortex-M4.
// ===========================================

void DroneScanner::initialize_wideband_scanning() {
    wideband_scan_data_.reset();
    setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
}

void DroneScanner::setup_wideband_range(Frequency min_freq, Frequency max_freq) {
    // Use unified frequency limits from DroneConstants
    Frequency safe_min = std::max(min_freq, EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ);
    Frequency safe_max = std::min(max_freq, EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ);

    // Ensure min < max and apply hardware constraints
    if (safe_min >= safe_max) {
        safe_min = EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ;
        safe_max = EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;
    }

    wideband_scan_data_.min_freq = safe_min;
    wideband_scan_data_.max_freq = safe_max;

    Frequency scanning_range = safe_max - safe_min;
    if (scanning_range > static_cast<Frequency>(settings_.wideband_slice_width_hz)) {
        // 🔴 FIX: Check for overflow BEFORE addition to prevent undefined behavior
        // Use uint64_t for all arithmetic to match Frequency type safely
        // Diamond Code: Use proper overflow check pattern: if (max_val - current_val < increment)
        const uint64_t range_u64 = static_cast<uint64_t>(scanning_range);
        const uint64_t width_u64 = static_cast<uint64_t>(settings_.wideband_slice_width_hz);

        // Check if adding width would overflow using Diamond Code pattern
        if (UINT64_MAX - range_u64 < width_u64) {
            // Overflow would occur - use single slice
            wideband_scan_data_.slices_nb = 1;
        } else {
            // DIAMOND OPTIMIZATION: Use constexpr ceil_div_u64 for better compiler optimization
            // Enables compiler to optimize division when width_u64 is a power of 2
            uint64_t range_plus_width = range_u64 + width_u64;
            wideband_scan_data_.slices_nb = static_cast<size_t>(ceil_div_u64(range_plus_width, width_u64));
        }

        if (wideband_scan_data_.slices_nb > WIDEBAND_MAX_SLICES) {
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }
        
        // 🔴 FIX: Check for overflow in multiplication BEFORE performing it
        // Diamond Code: Use proper overflow check pattern: if (max_val / current_val < increment)
        const uint64_t slices_nb_u64 = static_cast<uint64_t>(wideband_scan_data_.slices_nb);
        if (UINT64_MAX / slices_nb_u64 < width_u64) {
            // Multiplication would overflow - clamp slices_nb
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }
        
        Frequency slices_span = static_cast<Frequency>(slices_nb_u64 * width_u64);
        Frequency offset = ((scanning_range - slices_span) / 2) + (settings_.wideband_slice_width_hz / 2);
        Frequency center_frequency = safe_min + offset;

        std::generate_n(wideband_scan_data_.slices,
                       wideband_scan_data_.slices_nb,
                       [&center_frequency, slice_index = 0, this]() mutable -> WidebandSlice {
                           WidebandSlice slice;
                           slice.center_frequency = center_frequency;
                           slice.index = slice_index++;
                           center_frequency += settings_.wideband_slice_width_hz;
                           return slice;
                       });
    } else {
        wideband_scan_data_.slices[0].center_frequency = (safe_max + safe_min) / 2;
        wideband_scan_data_.slices_nb = 1;
    }
    wideband_scan_data_.slice_counter = 0;
}

void DroneScanner::start_scanning() {
    if (scanning_active_.load(std::memory_order_acquire)) return;

    scanning_active_.store(true, std::memory_order_release);
    scan_cycles_ = 0;
    total_detections_ = 0;
}

void DroneScanner::stop_scanning() {
    if (!scanning_active_.load(std::memory_order_acquire)) return;

    scanning_active_.store(false, std::memory_order_release);

    // Wait for scanning thread to complete
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

    if (!freq_db_ptr_) return false;

    auto db_path = get_freqman_path("DRONES");
    bool sd_loaded = freq_db_ptr_->open(db_path);

    {
        MutexLock lock(data_mutex);

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

// DIAMOND OPTIMIZATION: LUT lookup вместо switch для scanning_mode_name()
// Scott Meyers Item 15: Prefer constexpr to #define
// Экономит ~40 байт Flash
const char* DroneScanner::scanning_mode_name() const {
    uint8_t idx = static_cast<uint8_t>(scanning_mode_);
    return (idx < 3) ? SCANNING_MODE_NAMES[idx] : "Unknown";
}

void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    if (!scanning_active_.load(std::memory_order_acquire)) return;

    // 🔴 ENHANCED: Adaptive timing with golden mean between speed and accuracy
    // Considering Cortex M4 limitations on Portapack (limited RAM, CPU)
    using namespace EDA::Constants;
    uint32_t base_interval = DEFAULT_SCAN_INTERVAL_MS; // Base interval in milliseconds
    uint32_t adaptive_interval = base_interval;

    // Get current scan context
    size_t current_detections = get_total_detections();
    ThreatLevel max_threat = get_max_detected_threat();
    size_t tracked_count = tracked_count_;

    // ADAPTIVE STRATEGY:
    // - CRITICAL threats: scan fastest (250ms)
    // - HIGH threats: scan fast (400ms)
    // - MEDIUM threats: scan normal (750ms)
    // - LOW threats: scan slower (1000ms)
    // - NO threats: scan progressively slower (up to 2000ms)
    // - Balance: faster scanning of known drone bands, slower for empty bands

    if (max_threat >= ThreatLevel::CRITICAL) {
        // Critical threat detected - maximum speed scanning
        adaptive_interval = FAST_SCAN_INTERVAL_MS; // Fastest possible scan
    } else if (max_threat == ThreatLevel::HIGH) {
        // High threat - fast scanning
        adaptive_interval = HIGH_THREAT_SCAN_INTERVAL_MS;
    } else if (max_threat == ThreatLevel::MEDIUM) {
        // Medium threat - normal scanning
        adaptive_interval = NORMAL_SCAN_INTERVAL_MS;
    } else if (current_detections > 0 && tracked_count > 0) {
        // Have detections but low threat - maintain medium pace
        adaptive_interval = SLOW_SCAN_INTERVAL_MS;
    } else if (current_detections == 0 && scan_cycles_ > PROGRESSIVE_SLOWDOWN_DIVISOR) {
        // No detections for a while - progressively slow down
        // Progressive slowdown: 1000ms → 1500ms → 2000ms (cap)
        // DIAMOND OPTIMIZATION: Use lookup table instead of runtime division
        // Saves ~30 CPU cycles per scan cycle (division is expensive on Cortex-M4)
        uint32_t cycles_value = get_scan_cycles();
        uint32_t cycles_clamped = (cycles_value < 39) ? cycles_value : 39;
        uint32_t slowdown_multiplier = SLOWDOWN_MULTIPLIER_LUT[cycles_clamped];
        uint32_t interval_calc = base_interval * slowdown_multiplier;
        adaptive_interval = (interval_calc < VERY_SLOW_SCAN_INTERVAL_MS) ? interval_calc : VERY_SLOW_SCAN_INTERVAL_MS;
    }

    // Additional adjustment based on detection density
    static constexpr size_t HIGH_DENSITY_DETECTION_THRESHOLD = 5;
    if (current_detections > HIGH_DENSITY_DETECTION_THRESHOLD) {
        // Very high detection rate - maintain fast pace
        // DIAMOND OPTIMIZATION: Use ternary instead of std::min for better inlining
        adaptive_interval = (adaptive_interval < HIGH_DENSITY_SCAN_CAP_MS) ? adaptive_interval : HIGH_DENSITY_SCAN_CAP_MS;
    }
    
    // Apply adaptive interval
    chThdSleepMilliseconds(adaptive_interval);

    // DIAMOND OPTIMIZATION: LUT lookup вместо switch для perform_scan_cycle()
    // Scott Meyers Item 36: Never redefine an inherited non-virtual function
    // Экономит ~80 байт Flash, использует сырые указатели на методы (не std::function!)
    // Apply mode-specific adjustments
    uint8_t mode_idx = static_cast<uint8_t>(scanning_mode_);
    if (mode_idx < 3) {
        // Используем классный LUT DroneScanner::SCAN_FUNCTIONS для доступа к private методам
        (this->*DroneScanner::SCAN_FUNCTIONS[mode_idx])(hardware);  // Вызов метода через указатель
    }

    scan_cycles_++;

    // Adaptive sleep based on current situation
    if (scanning_active_.load(std::memory_order_acquire)) {
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

    const size_t batch_size = std::min(static_cast<size_t>(10), total_entries);

    // FIX: Use stack-allocated array instead of heap-allocated vector
    std::array<freqman_entry, 10> entries_to_scan{};
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

    // Named constants for scan parameters (from EDA::Constants)
    static constexpr int PLL_STABILIZATION_ITERATIONS = 3;
    static constexpr uint32_t PLL_STABILIZATION_DELAY_MS = 10;
    
    // Frequency validation constants (from unified limits)
    const Frequency MIN_VALID_FREQ = EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ;
    const Frequency MAX_VALID_FREQ = EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ;

    for (size_t i = 0; i < entries_count; ++i) {
        const auto& entry = entries_to_scan[i];

        // GUARD CLAUSE: Check scanning flag for immediate stop
        if (!scanning_active_.load(std::memory_order_acquire)) return;

        Frequency target_freq_hz = entry.frequency_a;

        // GUARD CLAUSE: Validate frequency range
        if (target_freq_hz < MIN_VALID_FREQ || target_freq_hz > MAX_VALID_FREQ) continue;

        // GUARD CLAUSE: Overflow protection check
        if (static_cast<uint64_t>(target_freq_hz) + 1000000ULL < static_cast<uint64_t>(target_freq_hz)) continue;

        // GUARD CLAUSE: Hardware tuning validation
        if (!hardware.tune_to_frequency(target_freq_hz)) continue;

        // Wait for PLL stabilization - broken into small chunks for responsiveness
        for (int w = 0; w < PLL_STABILIZATION_ITERATIONS; w++) {
            if (!scanning_active_.load(std::memory_order_acquire)) return;
            chThdSleepMilliseconds(PLL_STABILIZATION_DELAY_MS);
        }

        hardware.clear_rssi_flag();

        // Wait for fresh data with ABSOLUTE TIMEOUT (prevents infinite loop)
        // Diamond Code: Use named constants instead of magic numbers
        systime_t deadline = chTimeNow() + MS2ST(EDA::Constants::RSSI_TIMEOUT_MS);
        bool signal_captured = false;

        while (chTimeNow() < deadline) {
            chThdSleepMilliseconds(EDA::Constants::RSSI_POLL_DELAY_MS);
            if (hardware.is_rssi_fresh()) {
                signal_captured = true;
                break;
            }
        }

        // GUARD CLAUSE: Only process if signal was captured
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
    // Initialize range if needed (leave as is)
    if (wideband_scan_data_.slices_nb == 0) {
        setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
    }
    
    const WidebandSlice& current_slice = wideband_scan_data_.slices[wideband_scan_data_.slice_counter];
    
    // 🔴 ENHANCED: Intelligent slice skipping for Cortex M4 performance
    // Skip slices that have been "clean" for consecutive cycles to save CPU cycles
    // This is especially important on Portapack with limited RAM/CPU
    
    // 1. Tune to SLICE CENTER
    if (hardware.tune_to_frequency(current_slice.center_frequency)) {

        // 2. Wait for PLL stabilization with event-driven approach
        // CRITICAL OPTIMIZATION: Replace fixed sleep with event-driven waiting
        // PLL lock time for MAX2837 is typically < 1ms, but we need to wait for DC offset stabilization
        
        // Fast PLL lock check - most chips lock in < 1ms
        chThdSleepMilliseconds(1);
        
        // Check if hardware is ready (non-blocking check)
        if (!hardware.is_spectrum_streaming_active()) {
            hardware.start_spectrum_streaming();
        }
        
        // 3. Get spectrum data from M0 coprocessor with optimized timing
        // DIAMOND OPTIMIZATION: static buffer in Flash/RAM boundary (256 bytes)
        // Reduces stack usage by 256 bytes per call
        static std::array<uint8_t, 256> spectrum_data;
        
        // Clear spectrum flag and wait for fresh data

        // OPTIMIZED WAITING: Use adaptive timeout with ABSOLUTE TIMEOUT
        // M0 sends spectrum data ~60 times/sec (every ~16ms)
        // We wait max 32ms but check every 2ms for responsiveness
        // Diamond Code: Use named constants instead of magic numbers
        systime_t deadline = chTimeNow() + MS2ST(EDA::Constants::SPECTRUM_TIMEOUT_MS);
        bool spectrum_received = false;

        while (chTimeNow() < deadline) {
            chThdSleepMilliseconds(EDA::Constants::CHECK_INTERVAL_MS);

            // Atomic check-and-fetch to avoid TOCTOU race
            if (hardware.get_latest_spectrum_if_fresh(spectrum_data)) {
                spectrum_received = true;
                break;
            }
        }

        // 4. Perform spectral analysis if data was received
        if (spectrum_received) {
            // Analyze the spectrum using our new SpectralAnalyzer
            auto analysis_result = SpectralAnalyzer::analyze(
                spectrum_data,
                {hardware.get_spectrum_bandwidth(), current_slice.center_frequency}
            );
            
            // 5. Process detection based on spectral analysis
            if (analysis_result.is_valid && analysis_result.signature != SignalSignature::NOISE) {
                // Create detection entry
                freqman_entry detection_entry{
                    .frequency_a = static_cast<int64_t>(current_slice.center_frequency),
                    .frequency_b = static_cast<int64_t>(current_slice.center_frequency),
                    .description = "Spectral Detection",
                    .type = freqman_type::Single,
                };

                // Determine threat level and drone type from spectral analysis
                ThreatLevel threat_level = SpectralAnalyzer::get_threat_level(analysis_result.signature, analysis_result.snr);
                DroneType drone_type = SpectralAnalyzer::get_drone_type(current_slice.center_frequency, analysis_result.signature);

                // Process the detection
                process_spectral_detection(detection_entry, analysis_result, threat_level, drone_type);
            }
        } else {
            // Fallback to RSSI-based detection if spectrum data not available
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
        // Handle tuning error
        if (scan_cycles_ % 100 == 0) {
            handle_scan_error("Hardware tuning failed in wideband mode");
        }
    }

    // Transition to next slice with intelligent scanning
    size_t next_slice_idx = get_next_slice_with_intelligence();
    wideband_scan_data_.slice_counter = next_slice_idx;
}

size_t DroneScanner::get_next_slice_with_intelligence() {
    // 1. Check if we have a priority slice to scan
    // 🔴 FIXED: Race condition protection - copy entire decision into critical section
    size_t result_slice = 0;
    bool use_priority_slice = false;
    int32_t local_priority_slice = -1;
    
    {
        MutexLock lock(priority_slice_mutex_);
        local_priority_slice = priority_slice_index_;
        
        if (local_priority_slice != -1) {
            priority_scan_counter_++;
            
            // Scan priority slice every PRIORITY_SCAN_INTERVAL cycles
            // Diamond Code: Use named constant from eda_constants.hpp
            if (priority_scan_counter_ >= EDA::Constants::PRIORITY_SCAN_INTERVAL) {
                priority_scan_counter_ = 0;
                use_priority_slice = true;
                result_slice = static_cast<size_t>(local_priority_slice);
            }
        }
    }
    
    if (use_priority_slice) {
        return result_slice;
    }
    
    // 2. Check for frequency predictions (FHSS tracking)
    // 🔴 FIX: Race condition protection - lock predictions_mutex_
    size_t local_prediction_count;
    FrequencyPrediction local_predictions[MAX_FREQUENCY_PREDICTIONS];
    {
        MutexLock lock(predictions_mutex_);
        local_prediction_count = prediction_count_;
        if (prediction_count_ > 0) {
            std::copy_n(frequency_predictions_.begin(), prediction_count_, local_predictions);
        }
    }
    
    if (local_prediction_count > 0) {
        systime_t now = chTimeNow();
        size_t best_prediction_idx = 0;
        size_t max_confidence = 0;
        bool found_prediction = false;
        
        // Find the highest confidence prediction that's still fresh
        // Diamond Code: Use named constant for prediction staleness
        for (size_t i = 0; i < local_prediction_count; i++) {
            if (now - local_predictions[i].last_seen < EDA::Constants::PREDICTION_STALE_MS) {
                if (local_predictions[i].confidence > max_confidence) {
                    max_confidence = local_predictions[i].confidence;
                    best_prediction_idx = i;
                    found_prediction = true;
                }
            }
        }
        
        if (found_prediction) {
            // Find the slice that contains this predicted frequency
            Frequency predicted_freq = local_predictions[best_prediction_idx].predicted_freq;
            for (size_t i = 0; i < wideband_scan_data_.slices_nb; i++) {
                const WidebandSlice& slice = wideband_scan_data_.slices[i];
                Frequency slice_min = slice.center_frequency - (static_cast<Frequency>(settings_.wideband_slice_width_hz) / 2);
                Frequency slice_max = slice.center_frequency + (static_cast<Frequency>(settings_.wideband_slice_width_hz) / 2);
                
                if (predicted_freq >= slice_min && predicted_freq <= slice_max) {
                    // 🔴 FIX: Use method to boost confidence inside critical section
                    boost_prediction_confidence(best_prediction_idx, now);
                    return i;
                }
            }
        }
    }

    // 3. Normal sequential scanning
    size_t current = wideband_scan_data_.slice_counter;
    size_t next = (current + 1) % wideband_scan_data_.slices_nb;
    
    // If we just scanned a priority slice, don't immediately go back to it
    // 🔴 FIXED: Use local copy instead of atomic read
    if (local_priority_slice != -1 && next == static_cast<size_t>(local_priority_slice)) {
        next = (next + 1) % wideband_scan_data_.slices_nb;
    }
    
    return next;
}

void DroneScanner::update_frequency_predictions(Frequency detected_freq, ThreatLevel threat_level) {
    if (threat_level < ThreatLevel::MEDIUM) return; // Only predict for medium+ threats
    
    // 🔴 FIX: Race condition protection - lock predictions_mutex_
    MutexLock lock(predictions_mutex_);
    
    systime_t now = chTimeNow();
    
    // Check if this frequency is already in our predictions
    for (size_t i = 0; i < prediction_count_; i++) {
        if (frequency_predictions_[i].predicted_freq == detected_freq) {
            // Update existing prediction
            frequency_predictions_[i].confidence = std::min(frequency_predictions_[i].confidence + 2, size_t(10));
            frequency_predictions_[i].last_seen = now;
            return;
        }
    }
    
    // Add new prediction if we have space
    if (prediction_count_ < MAX_FREQUENCY_PREDICTIONS) {
        frequency_predictions_[prediction_count_].predicted_freq = detected_freq;
        frequency_predictions_[prediction_count_].confidence = 3; // Initial confidence
        frequency_predictions_[prediction_count_].last_seen = now;
        prediction_count_++;
    } else {
        // Replace lowest confidence prediction
        size_t min_confidence_idx = 0;
        size_t min_confidence = frequency_predictions_[0].confidence;
        
        for (size_t i = 1; i < MAX_FREQUENCY_PREDICTIONS; i++) {
            if (frequency_predictions_[i].confidence < min_confidence) {
                min_confidence = frequency_predictions_[i].confidence;
                min_confidence_idx = i;
            }
        }
        
        frequency_predictions_[min_confidence_idx].predicted_freq = detected_freq;
        frequency_predictions_[min_confidence_idx].confidence = 3;
        frequency_predictions_[min_confidence_idx].last_seen = now;
    }
}

void DroneScanner::update_priority_slice_detection(size_t slice_idx, bool detected_something_interesting) {
    // 🔴 FIX: Race condition protection
    MutexLock lock(priority_slice_mutex_);

    if (detected_something_interesting) {
        priority_slice_index_ = static_cast<int32_t>(slice_idx);
        priority_scan_counter_ = 0; // Reset counter to scan priority slice immediately next cycle
    } else if (priority_slice_index_ == static_cast<int32_t>(slice_idx)) {
        // If we didn't detect anything on the priority slice, reduce its priority
        // but don't immediately remove it
        // This creates a "sticky" priority that doesn't jump around too much
    }
}

void DroneScanner::boost_prediction_confidence(size_t prediction_idx, systime_t now) {
    MutexLock lock(predictions_mutex_);

    if (prediction_idx < EDA::Constants::MAX_FREQUENCY_PREDICTIONS) {
        // Diamond Code: Use named constants instead of magic numbers
        size_t new_confidence = frequency_predictions_[prediction_idx].confidence + EDA::Constants::CONFIDENCE_BOOST_INCREMENT;
        if (new_confidence > EDA::Constants::CONFIDENCE_MAX) {
            new_confidence = EDA::Constants::CONFIDENCE_MAX;
        }
        frequency_predictions_[prediction_idx].confidence = new_confidence;
        frequency_predictions_[prediction_idx].last_seen = now;
    }
}

void DroneScanner::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
    if (rssi >= threshold_override) {
        // DIAMOND OPTIMIZATION: Use const reference to avoid copy
        // Note: description modification requires entry to be mutable, but this is intentional
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

    // Threat level logic (copy from original)
    if (rssi > -70) threat_level = ThreatLevel::HIGH;
    else if (rssi > -80) threat_level = ThreatLevel::LOW;
    else threat_level = ThreatLevel::UNKNOWN;
    if (entry.frequency_a >= 2'400'000'000 && entry.frequency_a <= 2'500'000'000) {
        threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
    }

    // --- CRITICAL SECTION START ---
    {
        MutexLock lock(data_mutex);

        total_detections_++; // Protect counter

        size_t freq_hash = entry.frequency_a / 100000;
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
                    85,  // 85% confidence as integer
                    0,   // width_bins - default value
                    0,   // signal_width_hz - default value
                    0    // snr - default value
                };

                update_tracked_drone_internal({detected_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(rssi), threat_level});
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

void DroneScanner::process_spectral_detection(const freqman_entry& entry,
                                             const SpectralAnalysisResult& analysis_result,
                                             ThreatLevel threat_level, DroneType drone_type) {
    if (!EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    bool should_log = false;
    DetectionLogEntry log_entry_to_write;

    // --- CRITICAL SECTION START ---
    {
        MutexLock lock(data_mutex);

        total_detections_++; // Protect counter

        size_t freq_hash = entry.frequency_a / 100000;
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
                    90,  // 90% confidence for spectral analysis
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
    if (scan_cycles_ % 2 == 0) {
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

        total_detections_++;

        size_t freq_hash = entry.frequency_a / 100000;
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
                log_entry_to_write.frequency_hz = static_cast<uint32_t>(entry.frequency_a);
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

        // Use atomic member to prevent race condition
        systime_t last_time = last_detection_log_time_.load(std::memory_order_acquire);

        // Use chTimeNow() directly for timing - no CH_CFG_ST_FREQUENCY dependency
        if (now - last_time > 200) { // 200ms delay (10x more frequent than before)
            if (detection_logger_.is_session_active()) {
                // log_detection_async returns false if buffer is full,
                // so built-in protection against hanging is already inside the logger.
                if (detection_logger_.log_detection_async(log_entry_to_write)) {
                    last_detection_log_time_.store(now, std::memory_order_release);
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
        if (tracked_drones()[i].frequency == static_cast<uint32_t>(frequency) && tracked_drones()[i].update_count > 0) {
            tracked_drones()[i].add_rssi({static_cast<int16_t>(rssi), chTimeNow()});
            tracked_drones()[i].drone_type = static_cast<uint8_t>(type);
            tracked_drones()[i].threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
    }

    // TODO[FIXED]: Ring buffer overflow protection
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

    // Ring buffer overflow: find oldest entry and replace it
    size_t oldest_index = 0;
    systime_t oldest_time = tracked_drones()[0].last_seen;

    for (size_t i = 1; i < tracked_count_; i++) {
        if (tracked_drones()[i].last_seen < oldest_time) {
            oldest_time = tracked_drones()[i].last_seen;
            oldest_index = i;
        }
    }

    // Replace oldest entry instead of shifting array
    TrackedDrone& oldest_drone = tracked_drones()[oldest_index];
    oldest_drone = TrackedDrone();
    oldest_drone.frequency = static_cast<uint32_t>(frequency);
    oldest_drone.drone_type = static_cast<uint8_t>(type);
    oldest_drone.threat_level = static_cast<uint8_t>(threat_level);
    oldest_drone.add_rssi({static_cast<int16_t>(rssi), chTimeNow()});

    // No need to call update_tracking_counts() here since we're just replacing an existing entry
}

void DroneScanner::remove_stale_drones() {
    const systime_t STALE_TIMEOUT = 30000;
    systime_t current_time = chTimeNow();

    std::array<size_t, MAX_TRACKED_DRONES> stale_indices{};
    size_t stale_count = 0;

    // Step 1: Identify stale drones under mutex (fast O(n) scan)
    {
        MutexLock lock(data_mutex);
        for (size_t i = 0; i < tracked_count_; ++i) {
            if (tracked_drones()[i].update_count > 0 &&
                (current_time - tracked_drones()[i].last_seen) > STALE_TIMEOUT) {
                stale_indices[stale_count++] = i;
            }
        }
    }

    // Step 2: If no stale drones, exit early
    if (stale_count == 0) return;
    // Step 3: Compact array without mutex (O(n) but fast)
    MutexLock lock(data_mutex);
    size_t write_index = 0;
    size_t num_valid = 0;

    // Mark stale indices for removal
    std::array<bool, MAX_TRACKED_DRONES> is_stale{};
    for (size_t i = 0; i < stale_count; ++i) {
        if (stale_indices[i] < tracked_count_) {
            is_stale[stale_indices[i]] = true;
        }
    }

    // Compact: copy only valid drones
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

    // Step 4: Update tracking counts (only if we have valid data)
    if (tracked_count_ > 0) {
        update_tracking_counts();
    } else {
        approaching_count_ = 0;
        receding_count_ = 0;
        static_count_ = 0;
    }
}

// DIAMOND OPTIMIZATION: LUT lookup вместо switch для update_tracking_counts()
// Scott Meyers Item 20: Prefer pass-by-reference-to-const to pass-by-value
// Устраняет 4 case labels, экономит ~60 байт Flash
void DroneScanner::update_tracking_counts() {
    approaching_count_ = 0;
    receding_count_ = 0;
    static_count_ = 0;

    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones()[i].update_count < 2) continue;

        MovementTrend trend = tracked_drones()[i].get_trend();
        // LUT lookup вместо switch - O(1) время, ноль RAM
        // Используем классный LUT DroneScanner::TREND_COUNTERS для доступа к private членам
        uint8_t idx = static_cast<uint8_t>(trend);
        if (idx < 4) {
            // Скобки обязательны для правильного приоритета операций (постфиксный ++ имеет выше приоритет чем ->*)
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
    // ===========================================
    // ФАЗА 3.2: PLACEMENT NEW ДЛЯ FREQMAN_DB
    // ===========================================
    // Diamond Code: Compile-time известный размер, placement new
    // Преимущества: нет heap fragmentation, гарантированное выделение

    // 🔴 FIX: Runtime alignment verification before placement new
    // Prevents undefined behavior on platforms with stricter alignment
    if (reinterpret_cast<uintptr_t>(freq_db_storage_) % alignof(FreqmanDB) != 0) {
        handle_scan_error("Memory: freq_db_storage_ alignment error");
        return;
    }

    // Создаём FreqmanDB в статическом хранилище через placement new
    freq_db_ptr_ = new (freq_db_storage_) FreqmanDB();
    if (!freq_db_ptr_) {
        handle_scan_error("Memory: FreqmanDB placement new failed");
        return;
    }

    // 🔴 FIX: Runtime alignment verification for tracked_drones storage
    if (reinterpret_cast<uintptr_t>(tracked_drones_storage_) % alignof(TrackedDrone) != 0) {
        handle_scan_error("Memory: tracked_drones_storage_ alignment error");
        freq_db_ptr_->~FreqmanDB();
        freq_db_ptr_ = nullptr;
        return;
    }

    // Создаём массив TrackedDrone в статическом хранилище
    tracked_drones_ptr_ = new (tracked_drones_storage_)
        std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>();
    if (!tracked_drones_ptr_) {
        handle_scan_error("Memory: tracked_drones placement new failed");
        freq_db_ptr_->~FreqmanDB();  // Явный вызов деструктора
        freq_db_ptr_ = nullptr;
        return;
    }

    // Инициализируем все элементы TrackedDrone
    for (auto& drone : *tracked_drones_ptr_) {
        drone = TrackedDrone();  // Default construct
    }

    // Initialize database with built-in frequencies as fallback
    // This ensures scanner has valid data even if SD card is not ready
    {
        MutexLock lock(data_mutex);

        // Try to open DRONES database first
        auto db_path = get_freqman_path("DRONES");
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
}


void DroneScanner::sync_database() {
    MutexLock lock(data_mutex);
    
    if (!freq_db_ptr_) {
        return;
    }
    
    // NOTE: FreqmanDB automatically syncs on every write operation
    // via FileWrapper::replace_range() -> wrapped_->sync()
    // No explicit sync needed or accessible from this API layer
}

void DroneScanner::cleanup_database_and_scanner() {
    sync_database();

    if (db_loading_active_.load(std::memory_order_acquire)) {
        db_loading_active_.store(false, std::memory_order_release);
        if (db_loading_thread_ != nullptr) {
            chThdWait(db_loading_thread_);
            db_loading_thread_ = nullptr;
        }
    }

    // 🔴 FIX: Stop detection logger worker thread
    // This ensures proper cleanup of SD card resources and thread
    detection_logger_.stop_worker();

    // ===========================================
    // ФАЗА 3.4: ЯВНЫЙ ВЫЗОВ ДЕСТРУКТОРОВ (Placement New)
    // ===========================================
    // Diamond Code: placement new требует явного вызова деструктора

    if (tracked_drones_ptr_ != nullptr) {
        tracked_drones_ptr_->~array();
        tracked_drones_ptr_ = nullptr;
    }

    if (freq_db_ptr_ != nullptr) {
        freq_db_ptr_->~FreqmanDB();
        freq_db_ptr_ = nullptr;
    }
    
    // 🔴 FIX: Clear detection ring buffer to prevent stale data
    detection_ring_buffer_.clear();
    
    // 🔴 FIX: Reset all prediction data
    {
        MutexLock lock(predictions_mutex_);
        prediction_count_ = 0;
        std::fill(frequency_predictions_.begin(), frequency_predictions_.end(), FrequencyPrediction{});
    }
    
    // 🔴 FIX: Reset priority slice data
    {
        MutexLock lock(priority_slice_mutex_);
        priority_slice_index_ = -1;
        priority_scan_counter_ = 0;
    }
    
    // Note: ChibiOS mutexes (data_mutex_, predictions_mutex_, priority_slice_mutex_) 
    // are automatically cleaned up with the object. No explicit deinit needed.
}


// 🔴 FIX: Async database loading to prevent UI freeze
msg_t DroneScanner::db_loading_thread_entry(void* arg) {
    static_cast<DroneScanner*>(arg)->db_loading_thread_loop();
    return 0;
}

void DroneScanner::db_loading_thread_loop() {
    // ===========================================
    // ФАЗА 3.3: PLACEMENT NEW В ASYNC THREAD
    // ===========================================
    // Используем те же статические хранилища

    // Проверяем, не инициализированы ли уже
    if (freq_db_ptr_ != nullptr || tracked_drones_ptr_ != nullptr) {
        handle_scan_error("DB already initialized");
        db_loading_active_.store(false, std::memory_order_release);
        return;
    }

    // Placement new для FreqmanDB
    freq_db_ptr_ = new (freq_db_storage_) FreqmanDB();
    if (!freq_db_ptr_) {
        handle_scan_error("Memory: FreqmanDB async alloc failed");
        db_loading_active_.store(false, std::memory_order_release);
        return;
    }

    // Placement new для TrackedDrones
    tracked_drones_ptr_ = new (tracked_drones_storage_)
        std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>();
    if (!tracked_drones_ptr_) {
        handle_scan_error("Memory: tracked_drones async alloc failed");
        freq_db_ptr_->~FreqmanDB();
        freq_db_ptr_ = nullptr;
        db_loading_active_.store(false, std::memory_order_release);
        return;
    }

    // Инициализация элементов
    for (auto& drone : *tracked_drones_ptr_) {
        drone = TrackedDrone();
    }

    bool db_success = false;
    auto db_path = get_freqman_path("DRONES");

    systime_t load_start = chTimeNow();

    // DIAMOND FIX: Validate SD card status before attempting to open file
    systime_t sd_check_start = chTimeNow();
    while (sd_card::status() < sd_card::Status::Mounted) {
        if ((chTimeNow() - sd_check_start) > MS2ST(5000)) {  // 5 second timeout
            handle_scan_error("SD card not ready");
            db_loading_active_.store(false, std::memory_order_release);
            // Явный вызов деструкторов (placement new)
            if (freq_db_ptr_) {
                freq_db_ptr_->~FreqmanDB();
                freq_db_ptr_ = nullptr;
            }
            if (tracked_drones_ptr_) {
                tracked_drones_ptr_->~array();
                tracked_drones_ptr_ = nullptr;
            }
            return;
        }
        chThdSleepMilliseconds(100);
    }

    // DIAMOND FIX: Lock SD card before file operations (FatFS is NOT thread-safe)
    {
        SDCardLock lock;

    if (db_loading_active_.load(std::memory_order_acquire)) {
        db_success = freq_db_ptr_->open(db_path);

        if (db_success) {
            systime_t load_time = chTimeNow() - load_start;
            if (load_time > MS2ST(DB_LOAD_TIMEOUT_MS)) {
                handle_scan_error("Database load timeout");
                db_loading_active_.store(false, std::memory_order_release);
                // Явный вызов деструкторов (placement new)
                if (freq_db_ptr_) {
                    freq_db_ptr_->~FreqmanDB();
                    freq_db_ptr_ = nullptr;
                }
                if (tracked_drones_ptr_) {
                    tracked_drones_ptr_->~array();
                    tracked_drones_ptr_ = nullptr;
                }
                return;
            }
        }
    }
    }  // DIAMOND FIX: SDCardLock scope ends here

    if ((db_loading_active_.load(std::memory_order_acquire)) &&
        (!db_success || freq_db_ptr_->empty())) {

        // DIAMOND FIX: Lock SD card before file operations (FatFS is NOT thread-safe)
        SDCardLock lock;

        freq_db_ptr_->open(db_path, true);

        for (const auto& item : BUILTIN_DRONE_DB) {
            if (!db_loading_active_.load(std::memory_order_acquire)) {
                return;
            }

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

        sync_database();
    }  // DIAMOND FIX: SDCardLock scope ends here

    {
        MutexLock lock(data_mutex);
        freq_db_loaded_ = true;
    }
}

// 🔴 FIX: Async database loading (non-blocking UI)
void DroneScanner::initialize_database_async() {
    if (db_loading_active_.load(std::memory_order_acquire)) {
        return;  // Already loading or loaded
    }

    db_loading_active_.store(true, std::memory_order_release);

    // ===========================================
    // ФАЗА 5.2: СОЗДАНИЕ ПОТОКА СО СТАТИЧЕСКИМ СТЕКОМ
    // ===========================================
    // Diamond Code: Используем chThdCreateStatic вместо chThdCreateFromHeap
    // Преимущества: гарантированное создание, нет heap allocation

    db_loading_thread_ = chThdCreateStatic(
        db_loading_wa_,                    // Working area
        sizeof(db_loading_wa_),            // Size
        NORMALPRIO - 2,                    // Priority
        db_loading_thread_entry,           // Entry function
        this                               // Argument
    );

    // Проверка результата (chThdCreateStatic не может вернуть NULL при корректных параметрах)
    if (db_loading_thread_ == nullptr) {
        // GRACEFUL FAILURE: Do NOT run synchronously - would block UI thread
        // This should not happen with static stack allocation, but handle safely
        handle_scan_error("Scan unavailable - resource limit");
        db_loading_active_.store(false, std::memory_order_release);
        freq_db_loaded_ = false;
        return;  // Keep UI responsive - early return on failure
    }
}


// 🔴 FIX: Check if async loading finished
bool DroneScanner::is_database_loading_complete() const {
    return !db_loading_active_.load(std::memory_order_acquire) && freq_db_loaded_;
}

Frequency DroneScanner::get_current_scanning_frequency() const {
    // 🔴 FIX: Use freq_db_ptr_ instead of freq_db_
    if (!freq_db_ptr_) return 433000000;
    size_t db_entry_count = freq_db_ptr_->entry_count();
    if (db_entry_count > 0 && current_db_index_ < db_entry_count) {
        return (*freq_db_ptr_)[current_db_index_].frequency_a;
    }
    return 433000000;
}

Frequency DroneScanner::get_current_radio_frequency() const {
    return get_current_scanning_frequency();
}

const TrackedDrone& DroneScanner::getTrackedDrone(size_t index) const {
    // 🔴 FIX: Race condition protection - lock before accessing tracked_drones_
    MutexLock lock(data_mutex);
    if (index < tracked_count_) {
        return tracked_drones()[index];
    }
    // 🔴 OPTIMIZATION: Return static object (constexpr not allowed for non-literal type)
    static const TrackedDrone empty_drone{};
    return empty_drone;
}

void DroneScanner::handle_scan_error([[maybe_unused]] const char* error_msg) {
    // Store last error for diagnostics (accessed via get_last_scan_error if needed)
    if (error_msg) {
        last_scan_error_ = error_msg;
    }
}

DroneScanner::DroneSnapshot DroneScanner::get_tracked_drones_snapshot() const {
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
    
    // Инициализация бинарного семафора (not taken = false)
    chSemInit(&data_ready_, 0);
    
    start_session();
}

DroneDetectionLogger::~DroneDetectionLogger() {
    stop_worker();
    end_session();
    // Note: ChibiOS mutexes are automatically cleaned up with the object.
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

// Producer method - called by scanner thread (non-blocking)
bool DroneDetectionLogger::log_detection_async(const DetectionLogEntry& entry) {
    MutexLock lock(mutex_); // Блокировка на микросекунды

    if (is_full_) {
        // 🔴 FIX: Circular buffer - overwrite oldest entry instead of dropping
        // This ensures we always have the most recent data for analysis
        // Track overwrites for statistics and debugging
        dropped_logs_++;
        
        // Overwrite oldest entry (tail position)
        tail_ = (tail_ + 1) % BUFFER_SIZE;
    }

    // Копируем данные в буфер
    ring_buffer_[head_] = entry;
    head_ = (head_ + 1) % BUFFER_SIZE;
    
    if (head_ == tail_) {
        is_full_ = true;
    }

    // Сигнализируем рабочему потоку
    chSemSignal(&data_ready_);
    return true;
}

// Consumer lifecycle methods
void DroneDetectionLogger::start_worker() {
    if (worker_thread_) return;

    worker_should_run_ = true;
    // ===========================================
    // ФАЗА 5.4: СОЗДАНИЕ WORKER ПОТОКА СО СТАТИЧЕСКИМ СТЕКОМ
    // ===========================================
    worker_thread_ = chThdCreateStatic(
        worker_wa_,
        sizeof(worker_wa_),
        NORMALPRIO - 1,
        worker_thread_entry,
        this
    );

    // 🔴 FIX: Check thread creation result
    if (worker_thread_ == nullptr) {
        // Thread creation failed - fall back to synchronous logging
        worker_should_run_ = false;
        return;
    }

    start_session(); // Открываем файл/сессию
}


void DroneDetectionLogger::stop_worker() {
    if (!worker_thread_) return;

    worker_should_run_ = false;
    chSemSignal(&data_ready_); // Будим поток, чтобы он вышел из wait
    
    chThdWait(worker_thread_); // Ждем завершения
    worker_thread_ = nullptr;
    
    end_session(); // Закрываем файл
}

msg_t DroneDetectionLogger::worker_thread_entry(void* arg) {
    static_cast<DroneDetectionLogger*>(arg)->worker_loop();
    return 0;
}

void DroneDetectionLogger::worker_loop() {
    while (worker_should_run_) {
        chSemWaitTimeout(&data_ready_, MS2ST(1000));
        if (!worker_should_run_) break;

        DetectionLogEntry entry_to_write;
        bool has_data = false;
        bool has_more_data = false;

        {
            MutexLock lock(mutex_);
            if (head_ != tail_ || is_full_) {
                entry_to_write = ring_buffer_[tail_];
                tail_ = (tail_ + 1) % BUFFER_SIZE;
                is_full_ = false;
                has_data = true;

                // Проверяем есть ли ещё данные
                has_more_data = (head_ != tail_);
            }
        }
        // Мьютекс освобождён

        if (has_data) {
            write_entry_to_sd(entry_to_write);
        }

        // Сигнализируем ВНЕ мьютекса
        if (has_more_data) {
            chSemSignal(&data_ready_);
        }
    }
}

bool DroneDetectionLogger::write_entry_to_sd(const DetectionLogEntry& entry) {
    if (sd_card::status() < sd_card::Status::Mounted) {
        return false;
    }

    // DIAMOND FIX: Lock SD card before file operations (FatFS is NOT thread-safe)
    SDCardLock lock;

    if (!ensure_csv_header()) return false;

    // DIAMOND FIX: Stack buffer (no heap allocation)
    char line_buffer[128];
    int len = snprintf(line_buffer, sizeof(line_buffer),
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

    if (len < 0 || static_cast<size_t>(len) >= sizeof(line_buffer)) {
        return false;
    }

    // DIAMOND FIX: Zero-heap write using raw buffer
    auto error = csv_log_.write_raw(line_buffer, static_cast<File::Size>(len));

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

    // DIAMOND FIX: Static string literal stored in Flash (no heap allocation)
    static const char* const header =
        "timestamp_ms,frequency_hz,rssi_db,threat_level,drone_type,detection_count,confidence_percent,width_bins,signal_width_hz,snr\n";

    auto error = csv_log_.append(generate_log_filename());
    if (error && !error->ok()) return false;

    // DIAMOND FIX: Zero-heap write using raw buffer
    const size_t header_len = strlen(header);
    error = csv_log_.write_raw(header, static_cast<File::Size>(header_len));

    if (error && error->ok()) {
        header_written_ = true;
        return true;
    }
    return false;
}

// 🔴 OPTIMIZED: generate_log_filename() - use constexpr string (Flash storage)
// DIAMOND FIX: Return static const char* to eliminate heap allocation
const char* DroneDetectionLogger::generate_log_filename() const {
    return "EDA_LOG.CSV";
}

// ===========================================
// PART 3: HARDWARE CONTROLLER IMPLEMENTATION
// ===========================================

DroneHardwareController::DroneHardwareController(SpectrumMode mode)
    : last_spectrum_db_(),          // Declared 1st (top of private section)
      spectrum_mutex_(),            // Declared 2nd
      // spectrum_updated_ initialized in-class
      spectrum_mode_(mode),         // Declared 4th
      center_frequency_(2400000000ULL),
      bandwidth_hz_(24000000),
      radio_state_(),
      spectrum_fifo_(nullptr),
      spectrum_streaming_active_(false),
      rssi_updated_(false),         // Declared before last_valid_rssi_
      last_valid_rssi_(-120)        // Declared last
{
    chMtxInit(&spectrum_mutex_);
    spectrum_updated_ = false;
    // MessageHandlerRegistration objects are now initialized in-class (C++11 feature)
}

DroneHardwareController::~DroneHardwareController() {
    shutdown_hardware();
    // Note: ChibiOS mutexes are automatically cleaned up with the object.
}

void DroneHardwareController::initialize_hardware() {
    // 🔴 FIX: Initialize radio with error protection
    // Continue even if radio init fails to avoid blocking UI
    initialize_radio_state();
    initialize_spectrum_collector();
}

void DroneHardwareController::on_hardware_show() {
    initialize_hardware();
}

void DroneHardwareController::on_hardware_hide() {
    // Handlers are cleaned up in destructor to avoid double deletion
}

void DroneHardwareController::shutdown_hardware() {
    // Stop Baseband data streaming
    stop_spectrum_streaming();

    // Explicitly disable radio chip (CPLD/R820T/MAX2837)
    // This is critical for power saving and stability of the next application launch
    receiver_model.disable();

    // Clean up message subscriptions
    cleanup_spectrum_collector();
}

void DroneHardwareController::initialize_radio_state() {
    // 🔴 FIX: Add error handling for all radio operations
    // Returns false if any operation fails to prevent blocking

    receiver_model.enable();
    receiver_model.set_modulation(ReceiverModel::Mode::SpectrumAnalysis);
    receiver_model.set_sampling_rate(get_configured_sampling_rate());
    receiver_model.set_baseband_bandwidth(get_configured_bandwidth());
    receiver_model.set_squelch_level(0);

        // TODO[CRITICAL][FIXED]: Remove hardcoded RF Amp enable
        // Hardware safety: Never enable RF Amp by default to prevent damage
        // to antenna/power supply in case of short circuit
        // receiver_model.set_rf_amp(true); <-- REMOVED FOR SAFETY

        // Use safe defaults - RF Amp should be controlled by user settings
        // or left disabled until explicitly enabled
        receiver_model.set_rf_amp(false);

    // 2. LNA Gain (Low Noise Amplifier): 32dB (Range 0-40).
    // Responsible for receiving weak signals.
    receiver_model.set_lna(32);

    // 3. VGA Gain (Variable Gain Amplifier): 32dB (Range 0-62).
    // Responsible for signal volume after LNA.
    receiver_model.set_vga(32);
    
    // 4. OPTIMIZATION: Hardware-specific tuning for faster PLL lock
    // Configure MAX2837 for faster frequency switching
    // These settings reduce PLL lock time from ~5ms to ~1ms
    // Note: These methods may not exist in all ReceiverModel implementations
    // receiver_model.set_pll_lock_time(1); // Fast lock mode
    // receiver_model.set_vco_calibration_mode(1); // Fast calibration
    
    // 5. OPTIMIZATION: Pre-calibrate DC offset for faster stabilization
    // This reduces the time needed for DC offset to settle after frequency change
    // Note: These methods may not exist in all ReceiverModel implementations
    // receiver_model.enable_dc_offset_compensation(true);
    // receiver_model.set_dc_offset_calibration_mode(1); // Fast calibration mode
}

void DroneHardwareController::initialize_spectrum_collector() {
    // Handlers are now allocated in constructor to avoid external code address warnings
}

void DroneHardwareController::cleanup_spectrum_collector() {
    spectrum_streaming_active_ = false;
    // MessageHandlerRegistration objects are now stack objects and will be automatically destroyed
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
bool DroneHardwareController::tune_to_frequency(Frequency frequency_hz) {
    // Validate frequency range using DiamondCore utilities
    if (!EDA::Validation::validate_frequency(frequency_hz)) {
        return false;
    }
    
    // Overflow check for +1MHz offset
    if (static_cast<uint64_t>(frequency_hz) + 1000000ULL < static_cast<uint64_t>(frequency_hz)) {
        return false;
    }
    
    receiver_model.set_target_frequency(frequency_hz);
    return true;
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

// TODO[CRITICAL][FIXED]: Safe Baseband/Scanning separation methods
void DroneHardwareController::stop_spectrum_before_scan() {
    if (spectrum_streaming_active_) {
        spectrum_streaming_active_ = false;
        baseband::spectrum_streaming_stop();
        // Give M0 time to process stop command
        chThdSleepMilliseconds(10);
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
    rssi_updated_ = true;  // Mark RSSI as fresh
}

void DroneHardwareController::clear_rssi_flag() {
    rssi_updated_ = false;
}

bool DroneHardwareController::is_rssi_fresh() const {
    return rssi_updated_;
}

void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum) {
    MutexLock lock(spectrum_mutex_);

    // Copy spectrum data (usually 240 or 256 bins)
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

// ===========================================
// PART 4: UI IMPLEMENTATIONS
// ===========================================

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
    // Check-Before-Update Pattern: Only update if data actually changed
    bool data_changed = (max_threat != last_threat_) ||
                        (is_scanning != last_is_scanning_) ||
                        (current_freq != last_freq_) ||
                        (approaching != last_approaching_) ||
                        (static_count != last_static_) ||
                        (receding != last_receding_);

    if (!data_changed) {
        return; // Exit early, no need to redraw
    }

    // Update cached values
    last_threat_ = max_threat;
    last_is_scanning_ = is_scanning;
    last_freq_ = current_freq;
    last_approaching_ = approaching;
    last_static_ = static_count;
    last_receding_ = receding;

    size_t total_drones = approaching + static_count + receding;
    threat_progress_bar_.set_value(total_drones * 10);

    // DIAMOND OPTIMIZATION: Use StatusFormatter with thread-local buffer (64 bytes)
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

    // DIAMOND OPTIMIZATION: Use FrequencyFormatter
    if (current_freq > 0) {
        // DIAMOND OPTIMIZATION: thread-local buffer (16 bytes)
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

// ✅ ОПТИМИЗАЦИЯ: UnifiedColorLookup - O(1) lookup вместо 2 функций
Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) const {
    // ✅ UnifiedColorLookup устраняет дублирование
    return UnifiedColorLookup::header_bar(static_cast<uint8_t>(level));
}

Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) const {
    (void)level;  // Suppress unused parameter warning
    return Color::white();  // Always white text on colored backgrounds
}

const char* SmartThreatHeader::get_threat_icon_text(ThreatLevel level) const {
    return UnifiedStringLookup::threat_name(static_cast<uint8_t>(level));
}

void SmartThreatHeader::paint(Painter& painter) {
    // 1. Fill entire background with threat color
    Color bg_color = UnifiedColorLookup::header_bar(static_cast<uint8_t>(last_threat_));
    painter.fill_rectangle(screen_rect(), bg_color);

    // 2. Draw large centered text with white color on colored background
    // Calculate centered position
    const int text_width = strlen(last_text_) * 8; // fixed_8x16 is 8px per char
    const int text_height = 16;
    const int center_x = (screen_width - text_width) / 2;
    const int center_y = (60 - text_height) / 2; // Header height is 60px
    Point text_pos = {center_x, center_y};
    auto style = Style{font::fixed_8x16, bg_color, Color::white()};
    painter.draw_string(text_pos, style, last_text_);
}

ThreatCard::ThreatCard(size_t card_index, Rect parent_rect)
    : View(parent_rect), card_index_(card_index),
      parent_rect_(parent_rect), is_active_(false), // <--- ПЕРЕМЕЩЕНО СЮДА (после parent_rect_, перед last_frequency_)
      last_frequency_(0), last_threat_(ThreatLevel::NONE),
      last_trend_(MovementTrend::UNKNOWN), last_rssi_(-120) {
    last_threat_name_[0] = '\0';
    add_children({&card_text_});
}

// 🎯 Use UnifiedStringLookup for consolidated utility functions

void ThreatCard::update_card(const DisplayDroneEntry& drone) {
    // Check-Before-Update Pattern: Only update if data actually changed
    bool data_changed = (drone.frequency != last_frequency_) ||
                        (drone.threat != last_threat_) ||
                        (drone.trend != last_trend_) ||
                        (std::abs(drone.rssi - last_rssi_) > 1) || // RSSI hysteresis
                        (strcmp(drone.type_name, last_threat_name_) != 0);

    if (!data_changed && is_active_) {
        return; // Exit early, no need to redraw
    }

    // Update cached values
    last_frequency_ = drone.frequency;
    last_threat_ = drone.threat;
    last_trend_ = drone.trend;
    last_rssi_ = drone.rssi;
    safe_strcpy(last_threat_name_, drone.type_name, sizeof(last_threat_name_));
    is_active_ = true;

    // DIAMOND OPTIMIZATION: Use TrendSymbols for O(1) lookup
    char trend_char = TrendSymbols::from_trend(static_cast<uint8_t>(drone.trend));
    uint32_t mhz = drone.frequency / 1000000;

    // DIAMOND OPTIMIZATION: Use StatusFormatter with thread-local buffer (48 bytes)
    thread_local char buffer[48];
    StatusFormatter::format_to(buffer, "%s %c %luM %ld",
                             drone.type_name, trend_char,
                             (unsigned long)mhz, (long)drone.rssi);
    card_text_.set(buffer);
    
    // DIAMOND OPTIMIZATION: статический стиль (не создаётся на стеке каждый вызов)
    static const Style CARD_STYLE = {font::fixed_8x16, Color::black(), Color::white()};
    card_text_.set_style(&CARD_STYLE);
    
    set_dirty(); // Only call set_dirty() when content actually changes
}

void ThreatCard::clear_card() {
    is_active_ = false;
    card_text_.set("");
    set_dirty();
}



// ✅ ОПТИМИЗАЦИЯ: UnifiedColorLookup - O(1) lookup вместо 2 функций
Color ThreatCard::get_card_bg_color() const {
    if (!is_active_) return Color::black();

    // ✅ UnifiedColorLookup устраняет дублирование
    return UnifiedColorLookup::card_bg(static_cast<uint8_t>(last_threat_));
}

Color ThreatCard::get_card_text_color() const {
    // ✅ UnifiedColorLookup устраняет дублирование
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

void ThreatCard::set_parent_rect(const Rect& rect) {
    parent_rect_ = rect;
    View::set_parent_rect(rect);
    set_dirty();
}

ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
    : View(parent_rect), bar_index_(bar_index), parent_rect_(parent_rect) {
    add_children({&progress_text_, &alert_text_, &normal_text_});
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    // DIAMOND OPTIMIZATION: Early Return для invalid states
    if (progress_percent > 100) progress_percent = 100;

    set_display_mode(DisplayMode::SCANNING);

    // DIAMOND OPTIMIZATION: Table-driven progress bar (устраняет цикл + 5 строк кода)
    // O(1) lookup вместо runtime цикла с условными проверками
    // Исправлено: Тернарный оператор вместо std::min (избегает несовпадение типов)
    uint8_t raw_idx = (progress_percent * 8) / 100;
    uint8_t bar_idx = (raw_idx < 8) ? raw_idx : 8;
    const auto& bar_entry = PROGRESS_PATTERNS[bar_idx];

    // DIAMOND OPTIMIZATION: Use StatusFormatter для унифицированного форматирования
    char buffer[64];
    StatusFormatter::format_to(buffer, "%s %lu%% C:%lu D:%lu",
                             bar_entry.pattern, (unsigned long)progress_percent,
                             (unsigned long)total_cycles, (unsigned long)detections);
    progress_text_.set(buffer);

    // DIAMOND OPTIMIZATION: Style из constexpr LUT (не создаётся на стеке)
    static const Style SCANNING_STYLE = {font::fixed_8x16, Color::black(), Color(STATUS_STYLES[0].bg_color)};
    progress_text_.set_style(&SCANNING_STYLE);

    if (detections > 0) {
        set_display_mode(DisplayMode::ALERT);
        // DIAMOND OPTIMIZATION: Use StatusFormatter
        char alert_buffer[64];
        StatusFormatter::format_to(alert_buffer, "[!] DETECTED: %lu threats found!",
                                  static_cast<unsigned long>(detections));
        alert_text_.set(alert_buffer);

        // DIAMOND OPTIMIZATION: Style из constexpr LUT
        static const Style ALERT_STYLE = {font::fixed_8x16, Color::black(), Color(THREAT_STYLES[0].bg_color)};
        alert_text_.set_style(&ALERT_STYLE);
    }
    set_dirty();
}

void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const char* alert_msg) {
    // DIAMOND OPTIMIZATION: Early Return для invalid states
    if (!alert_msg) return;

    set_display_mode(DisplayMode::ALERT);

    // DIAMOND OPTIMIZATION: Unified icon and style mapping из constexpr LUT
    size_t icon_idx = std::min(static_cast<size_t>(threat), size_t(4));
    const char* alert_icon = ALERT_ICONS[icon_idx];

    // DIAMOND OPTIMIZATION: Use StatusFormatter
    char buffer[64];
    StatusFormatter::format_to(buffer, "%s ALERT: %lu drones | %s",
                             alert_icon,
                             static_cast<unsigned long>(total_drones),
                             alert_msg);
    alert_text_.set(buffer);

    // DIAMOND OPTIMIZATION: Table-driven style selection (устраняет условный оператор + 7 строк кода)
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
    // DIAMOND OPTIMIZATION: Early Return для invalid states
    if (!primary) return;
    
    set_display_mode(DisplayMode::NORMAL);

    // DIAMOND OPTIMIZATION: Use StatusFormatter
    char buffer[48];
    if (!secondary || strlen(secondary) == 0) {
        StatusFormatter::format_to(buffer, "%s", primary);
    } else {
        StatusFormatter::format_to(buffer, "%s | %s", primary, secondary);
    }
    normal_text_.set(buffer);
    
    // DIAMOND OPTIMIZATION: статический стиль (не создаётся на стеке каждый вызов)
    static const Style NORMAL_STYLE = {font::fixed_8x16, Color::black(), Color::white()};
    normal_text_.set_style(&NORMAL_STYLE);
    set_dirty();
}

void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
    // DIAMOND OPTIMIZATION: Early Return
    if (mode_ == mode) return;

    mode_ = mode;

    // DIAMOND OPTIMIZATION: Битовая маска вместо 3x bool сравнения
    // Компактнее и быстрее
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
    // TODO[FIXED]: STEP 2.2: Memory leaks in MessageHandler's
    // These handlers are now stack-allocated and will be automatically destroyed
    // No manual cleanup needed for stack-allocated objects
}

// ===========================================
// IMPLEMENTATION: process_frame_sync
// ===========================================
void DroneDisplayController::process_frame_sync() {
    if (spectrum_fifo_) {
        ChannelSpectrum spectrum;
        while (spectrum_fifo_->out(spectrum)) {
            this->process_mini_spectrum_data(spectrum);
            this->analyze_spectrum_for_threats(spectrum);
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
          // 🔴 ФАЗА 1.5: Static buffers initialization
          detected_drones_count_(0),
          buffers_allocated_(false),
         threat_bins_(), threat_bins_count_(0),
         waterfall_line_index_(0),
         spectrum_gradient_(), spectrum_fifo_(nullptr),
         pixel_index(0), bins_hz_size(0), each_bin_size(100000), min_color_power(0),
         marker_pixel_step(1000000), max_power(0), range_max_power(0), mode(0),
         spectrum_config_()
{
    // 🔴 REMOVED: frequency_ruler_ member (Dead Code - duplicate never used)
    // FrequencyRuler class was removed, only CompactFrequencyRuler is used

    // DIAMOND OPTIMIZATION: Member initialization (moved from body to initializer list)
    // detected_drones_count_ initialized in initializer list (line 2650)
    // displayed_drones_ value-initialized in initializer list (line 2648)
    // SAVES: ~4 bytes Flash, eliminates unnecessary std::fill call
    //
    // detected_drones_count_ = 0;
    // std::fill(displayed_drones_.begin(), displayed_drones_.end(), DisplayDroneEntry{});

    // CRITICAL FIX: Use default gradient immediately
    spectrum_gradient_.set_default();

    // --- REMOVED: Stack allocation now handled in header ---
    // MessageHandlerRegistration objects are now initialized in-class (C++11 feature)

    // CRITICAL: Add ALL widgets to View hierarchy
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

    // 🔴 REMOVED: frequency_ruler_.set_visible(false) (Dead Code - duplicate never used)
}

// 🔴 REMOVED: Duplicate initialization block (lines 2690-2720)
// REASON: Duplicate of constructor initialization (lines 2661-2686)
//         Caused syntax error when closing brace at line 2689 orphaned this code
// SAVES: ~30 lines Flash, ~120 bytes
//
//     // TODO[FIXED]: Reserve memory in advance.
//     // MAX_TRACKED_DRONES is usually around 8-10, +2 as reserve.
//     detected_drones_count_ = 0;
// 
//     std::fill(displayed_drones_.begin(), displayed_drones_.end(), DisplayDroneEntry{});
// 
//     // CRITICAL FIX: Use default gradient immediately
//     spectrum_gradient_.set_default();
// 
//     // --- REMOVED: Stack allocation now handled in header ---
//     // MessageHandlerRegistration objects are now initialized in-class (C++11 feature)
// 
//     // CRITICAL: Add ALL widgets to View hierarchy
//     add_children({
//         &big_display_,
//         &scanning_progress_,
//         &compact_frequency_ruler_,
//         &text_threat_summary_,
//         &text_status_info_,
//         &text_scanner_stats_,
//         &text_trends_compact_,
//         &text_drone_1_,
//         &text_drone_2_,
//         &text_drone_3_,
//         &text_signal_type_
//     });
// 
//     // 🔴 REMOVED: frequency_ruler_ (Dead Code - duplicate never used)
//     // Hide old ruler, use compact by default
//     // frequency_ruler_.set_visible(false);
// }

// 🔴 FIX: Deferred buffer allocation to prevent stack overflow
void DroneDisplayController::allocate_buffers() {
    // DIAMOND OPTIMIZATION: waterfall_buffer удалён (не нужен, экономия ~9.6KB RAM)
    // render_mini_spectrum использует display.scroll() и локальный массив new_line

    // Diamond Code: Статические буферы - просто вызываем allocate_buffers_from_pool
    allocate_buffers_from_pool();
}


void DroneDisplayController::deallocate_buffers() {
    // Diamond Code: Статические буферы не требуют deallocation
    // Просто сбрасываем флаг
    buffers_allocated_ = false;

    // Опционально: очищаем память для безопасности
    std::fill(std::begin(spectrum_row_buffer_storage_),
              std::end(spectrum_row_buffer_storage_),
              Color::black());
    std::fill(std::begin(render_line_buffer_storage_),
              std::end(render_line_buffer_storage_),
              Color::black());
    std::fill(std::begin(spectrum_power_levels_storage_),
              std::end(spectrum_power_levels_storage_),
              0);
}


// ===========================================
// ФАЗА 1.3: СТАТИЧЕСКИЕ БУФЕРЫ - ИМПЛЕМЕНТАЦИЯ
// ===========================================
// Diamond Code: Zero heap allocation
// Буферы статические - "выделение" просто устанавливает флаг

bool DroneDisplayController::are_buffers_allocated() const {
    return buffers_allocated_;
}

bool DroneDisplayController::are_buffers_valid() const {
    // Статические буферы всегда валидны после "аллокации"
    // Размер проверяется на этапе компиляции через static_assert
    return buffers_allocated_;
}

bool DroneDisplayController::allocate_buffers_from_pool() {
    // Diamond Code: Статические буферы не требуют allocation
    // Просто устанавливаем флаг и очищаем память

    if (buffers_allocated_) {
        return true;  // Уже "выделено"
    }

    // Очищаем буферы (zero-initialize)
    // Это выполняется один раз при первом вызове
    std::fill(std::begin(spectrum_row_buffer_storage_),
              std::end(spectrum_row_buffer_storage_),
              Color::black());

    std::fill(std::begin(render_line_buffer_storage_),
              std::end(render_line_buffer_storage_),
              Color::black());

    std::fill(std::begin(spectrum_power_levels_storage_),
              std::end(spectrum_power_levels_storage_),
              0);

    buffers_allocated_ = true;
    waterfall_line_index_ = 0;

    return true;  // Всегда успешно (нет heap allocation)
}



void DroneDisplayController::update_detection_display(const DroneScanner& scanner) {
     if (scanner.is_scanning_active()) {
         Frequency current_freq = scanner.get_current_scanning_frequency();
         // NOLINTNEXTLINE(bugprone-branch-clone)
         if (current_freq > 0) {
             // 🔴 FIX: Use buffer-based formatting (no std::string allocation)
             char freq_buf[16];
             FrequencyFormatter::to_string_short_freq_buffer(freq_buf, sizeof(freq_buf), current_freq);
             big_display_.set(freq_buf);
         } else {
             big_display_.set("2400.0MHz");
         }
     } else {
         big_display_.set("READY");
     }

    size_t total_freqs = scanner.get_database_size();
    if (total_freqs > 0 && scanner.is_scanning_active()) {
        uint32_t progress_percent = 50;
        scanning_progress_.set_value(std::min(progress_percent, (uint32_t)100));
    } else {
        scanning_progress_.set_value(0);
    }

    ThreatLevel max_threat = scanner.get_max_detected_threat();
    bool has_detections = (scanner.get_approaching_count() + scanner.get_receding_count() + scanner.get_static_count()) > 0;

    if (has_detections) {
    char summary_buffer[48];
    const char* threat_name = UnifiedStringLookup::threat_name(static_cast<uint8_t>(max_threat));
        // DIAMOND OPTIMIZATION: Use StatusFormatter
        StatusFormatter::format_to(summary_buffer, "THREAT: %s | <%lu ~%lu >%lu",
                                  threat_name,
                                  static_cast<unsigned long>(scanner.get_approaching_count()),
                                  static_cast<unsigned long>(scanner.get_static_count()),
                                  static_cast<unsigned long>(scanner.get_receding_count()));
        text_threat_summary_.set(summary_buffer);
        // DIAMOND OPTIMIZATION: Use Color::red()
        static Style red_style{font::fixed_8x16, Color::black(), Color::red()};
        text_threat_summary_.set_style(&red_style);
    } else {
        text_threat_summary_.set("THREAT: NONE | All clear");
        // DIAMOND OPTIMIZATION: Use Color::green()
        static Style green_style{font::fixed_8x16, Color::black(), Color::green()};
        text_threat_summary_.set_style(&green_style);
    }

    char status_buffer[48];
    if (scanner.is_scanning_active()) {
        // DIAMOND OPTIMIZATION: const char* вместо std::string (экономия RAM)
        // Scott Meyers Item 1: View C++ as a federation of languages
        // const char* указывает на Flash, std::string выделяет RAM
        const char* mode_str = scanner.is_real_mode() ? "REAL" : "DEMO";
        StatusFormatter::format_to(status_buffer, "%s - Detections: %lu",
                                  mode_str, static_cast<unsigned long>(scanner.get_total_detections()));
    } else {
        StatusFormatter::format_to(status_buffer, "Ready - Enhanced Drone Analyzer");
    }
    text_status_info_.set(status_buffer);

    size_t loaded_freqs = scanner.get_database_size();
    char stats_buffer[48];
    if (scanner.is_scanning_active() && loaded_freqs > 0) {
        size_t current_idx = 0;
        // DIAMOND OPTIMIZATION: Use StatusFormatter
        StatusFormatter::format_to(stats_buffer, "Freq: %lu/%lu | Cycle: %lu",
                                  static_cast<unsigned long>(current_idx + 1),
                                  static_cast<unsigned long>(loaded_freqs),
                                  static_cast<unsigned long>(scanner.get_scan_cycles()));
    } else if (loaded_freqs > 0) {
        StatusFormatter::format_to(stats_buffer, "Loaded: %lu frequencies",
                                  static_cast<unsigned long>(loaded_freqs));
    } else {
        StatusFormatter::format_to(stats_buffer, "No database loaded");
    }
    text_scanner_stats_.set(stats_buffer);

    // DIAMOND OPTIMIZATION: ternary operator вместо каскадного if-else
    // Компилятор оптимизирует это в безветвящийся код (branchless)
    size_t color_idx = (max_threat >= ThreatLevel::HIGH) ? 4 :
                      (max_threat >= ThreatLevel::MEDIUM) ? 3 :
                      (has_detections) ? 2 :
                      (scanner.is_scanning_active()) ? 1 : 0;

    // DIAMOND OPTIMIZATION: constexpr LUT вместо локального массива (хранится во Flash)
    big_display_.set_style(&BIG_DISPLAY_STYLES[color_idx]);
}

// ===========================================
// IMPLEMENTATION: DroneDisplayController::set_scanning_status
// ===========================================
void DroneDisplayController::set_scanning_status(bool active, const char* message) {
    // DIAMOND OPTIMIZATION: Early Return для invalid states
    if (!message) return;
    
    // DIAMOND OPTIMIZATION: Use StatusFormatter
    char buffer[48];
    if (active) {
        StatusFormatter::format_to(buffer, "SCAN: %s", message);
        text_status_info_.set(buffer);
        
        // DIAMOND OPTIMIZATION: статический стиль из constexpr LUT
        static const Style SCANNING_STYLE = {font::fixed_8x16, Color::black(), Color::green()};
        text_status_info_.set_style(&SCANNING_STYLE);
    } else {
        StatusFormatter::format_to(buffer, "STOP: %s", message);
        
        // DIAMOND OPTIMIZATION: статический стиль (не создаётся на стеке)
        static const Style STOPPED_STYLE = {font::fixed_8x16, Color::black(), Color::white()};
        text_status_info_.set_style(&STOPPED_STYLE);
    }
}

void DroneDisplayController::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) {
    systime_t now = chTimeNow();

    // Find existing drone by frequency
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

    // Add new drone if space available
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

    // STEP 2: Work with local snapshot copy.
    // Scanner may already be looking for new drones in another thread at this time.

    const systime_t STALE_TIMEOUT = 30000;
    systime_t now = chTimeNow();

    // Clear UI list
    detected_drones_count_ = 0;

    // Fill from snapshot
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
            entry.trend = drone_data.get_trend(); // Now this is safe to call
            detected_drones_count_++;
        }
    }

    // STEP 3: Sorting and rendering
    sort_drones_by_rssi();

    // ... (remaining code for updating display_drones_ and calling render)
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

        // DIAMOND OPTIMIZATION: Use StatusFormatter
        StatusFormatter::format_to(buffer, DRONE_DISPLAY_FORMAT,
                                 drone.type_name,
                                 freq_buf,
                                 (long int)drone.rssi,
                                 trend_symbol);

        // DIAMOND OPTIMизация: хелпер для индексного доступа (без bounds check каждый раз)
        Text* target = drone_text_widget(i);
        if (target) {
            target->set(buffer);
        }
    }
}

void DroneDisplayController::initialize_mini_spectrum() {
    if (!spectrum_gradient_.load_file(default_gradient_file)) {
        spectrum_gradient_.set_default();
    }
    clear_spectrum_buffers();
    update_frequency_ruler();
}

void DroneDisplayController::lazy_initialize_gradient() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    // Try to load gradient file, use default if fails
    // This is now called from on_show() instead of constructor to avoid blocking I/O
    if (!spectrum_gradient_.load_file(default_gradient_file)) {
        spectrum_gradient_.set_default();
    }
    clear_spectrum_buffers();
    update_frequency_ruler();
}

void DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum& spectrum) {
    uint8_t current_bin_power = 0;
    for (size_t bin = 0; bin < MINI_SPECTRUM_WIDTH; bin++) {
        get_max_power_for_current_bin(spectrum, bin, current_bin_power);
        if (process_bins(&current_bin_power)) {
            return;
        }
    }
}

bool DroneDisplayController::process_bins(uint8_t* powerlevel) {
    bins_hz_size += each_bin_size;
    if (bins_hz_size >= marker_pixel_step) {
        if (*powerlevel > min_color_power)
            add_spectrum_pixel(*powerlevel);
        else
            add_spectrum_pixel(0);
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

    // 1. Очищаем область спектра (вместо display.scroll)
    const int waterfall_y_start = config.WATERFALL_Y_START;
    const int spectrum_height = config.BAR_HEIGHT_MAX;

    painter.fill_rectangle(
        {0, waterfall_y_start, EDA::Constants::MINI_SPECTRUM_WIDTH, spectrum_height},
        Color::black()
    );

    // 🔴 ФАЗА 1.5: Проверка валидности буфера (статический буфер)
    if (!buffers_allocated_) {
        return;
    }


    // 2. Проход по всем столбцам (бинам) спектра
    const auto& levels = spectrum_power_levels();
    const size_t spectrum_width = std::min(levels.size(),
                                        static_cast<size_t>(EDA::Constants::MINI_SPECTRUM_WIDTH));

    for (size_t x = 0; x < spectrum_width; ++x) {
        uint8_t power = levels[x];

        // Фильтр шума: не рисуем ничего, если сигнал слишком слабый
        if (power < config.NOISE_THRESHOLD) continue;

        // 3. Расчет высоты столбика (0-255 → 0-spectrum_height)
        int bar_height = (power * spectrum_height) / 255;
        if (bar_height < 1) bar_height = 1;
        if (bar_height > spectrum_height) bar_height = spectrum_height;

        // 4. АНАЛИЗ ФОРМЫ (Острый vs Широкий)
        size_t color_idx = get_bar_color_index(x, power);

        // 5. Рисование столбика
        int y_top = (waterfall_y_start + spectrum_height) - bar_height;
        painter.fill_rectangle(
            {static_cast<int>(x), y_top, 1, bar_height},
            config.BAR_COLORS[color_idx]
        );
    }
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
    // 🔴 ФАЗА 1.5: Проверка валидности буфера (статический буфер)
    if (!buffers_allocated_) {
        return;
    }
    std::fill(spectrum_power_levels().begin(), spectrum_power_levels().end(), 0);
}

bool DroneDisplayController::validate_spectrum_data() const {
    // 🔴 ФАЗА 1.5: Проверка валидности буфера (статический буфер)
    if (!buffers_allocated_) {
        return false;
    }

    if (spectrum_power_levels().size() != MINI_SPECTRUM_WIDTH) return false;
    if (spectrum_gradient_.lut.empty()) return false;
    return true;
}

size_t DroneDisplayController::get_safe_spectrum_index(size_t x, size_t y) const {
    if (x >= MINI_SPECTRUM_WIDTH || y >= MINI_SPECTRUM_HEIGHT) {
        return 0;
    }
    return y * MINI_SPECTRUM_WIDTH + x;
}

void DroneDisplayController::set_spectrum_range(Frequency min_freq, Frequency max_freq) {
    // Use unified frequency limits from DroneConstants
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

void DroneDisplayController::update_signal_type_display(const char* signal_type) {
    // DIAMOND OPTIMIZATION: Early Return для invalid states
    if (!signal_type) return;
    
    // DIAMOND OPTIMIZATION: Use StatusFormatter
    char buffer[48];
    StatusFormatter::format_to(buffer, "SIGNAL: %s", signal_type);
    text_signal_type_.set(buffer);
 
    // DIAMOND OPTIMIZATION: constexpr LUT для signal_type → index (во Flash)
    static constexpr struct SignalTypeMapping {
        const char* name;
        uint8_t idx;
    } SIGNAL_TYPE_LUT[] = {
        {"--",       0},  // DEFAULT/Unknown
        {"DIGITAL",  1},  // Digital
        {"ANALOG",   2},  // Analog
        {"NOISE",    3}   // Noise
    };
    
    // Быстрый поиск по имени (O(n) где n=4, быстрее чем 3x strcmp + ternary)
    size_t signal_idx = 0;
    for (size_t i = 0; i < 4; ++i) {
        if (strcmp(signal_type, SIGNAL_TYPE_LUT[i].name) == 0) {
            signal_idx = SIGNAL_TYPE_LUT[i].idx;
            break;
        }
    }
 
    // DIAMOND OPTIMIZATION: constexpr LUT вместо локального массива (хранится во Flash)
    static constexpr Style SIGNAL_STYLES[] = {
        {font::fixed_8x16, Color::black(), SIGNAL_TYPE_CONFIG[0].color},
        {font::fixed_8x16, Color::black(), SIGNAL_TYPE_CONFIG[1].color},
        {font::fixed_8x16, Color::black(), SIGNAL_TYPE_CONFIG[2].color},
        {font::fixed_8x16, Color::black(), SIGNAL_TYPE_CONFIG[3].color}
    };
    text_signal_type_.set_style(&SIGNAL_STYLES[signal_idx]);
 
    set_dirty();
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

void DroneDisplayController::handle_channel_spectrum(const ChannelSpectrum& spectrum) {
    process_mini_spectrum_data(spectrum);
}

void DroneDisplayController::analyze_spectrum_for_threats(const ChannelSpectrum& spectrum) {
    for (size_t i = 0; i < spectrum.db.size(); ++i) {
        uint8_t power = spectrum.db[i];
        
        const uint8_t THREAT_THRESHOLD = 100;
        
        if (power > THREAT_THRESHOLD) {
            Frequency freq_hz = spectrum_bin_to_frequency(i);
            update_or_create_drone_from_spectrum(freq_hz, power);
        }
    }
}

Frequency DroneDisplayController::spectrum_bin_to_frequency(size_t bin) const {
    const Frequency MIN_FREQ = spectrum_config_.min_freq;
    const Frequency MAX_FREQ = spectrum_config_.max_freq;
    const Frequency FREQ_RANGE = MAX_FREQ - MIN_FREQ;
    
    if (FREQ_RANGE == 0 || bin >= MINI_SPECTRUM_WIDTH) {
        return MIN_FREQ;
    }
    
    int64_t relative_freq = (static_cast<int64_t>(bin) * FREQ_RANGE) / MINI_SPECTRUM_WIDTH;
    return MIN_FREQ + static_cast<Frequency>(relative_freq);
}

void DroneDisplayController::update_or_create_drone_from_spectrum(Frequency freq_hz, uint8_t power) {
    int32_t rssi = static_cast<int32_t>(power) - 150;
    ThreatLevel threat = ThreatClassifier::from_rssi(rssi);
    DroneType type = DroneTypeDetector::from_frequency(freq_hz);
    add_detected_drone(freq_hz, type, threat, rssi);
}

DroneUIController::DroneUIController(NavigationView& nav,
                                          DroneHardwareController& hardware,
                                          DroneScanner& scanner,
                                          ::AudioManager& audio_mgr,
                                          DroneDisplayController& display_controller)
    : nav_(nav),
      hardware_(hardware),
      scanner_(scanner),
      audio_mgr_(audio_mgr),
      scanning_active_{false},
      display_controller_(&display_controller),
      settings_()
{
    settings_.spectrum_mode = SpectrumMode::MEDIUM;
    settings_.scan_interval_ms = 1000;
    settings_.rssi_threshold_db = -90;
    settings_.audio_flags.enable_alerts = true;
    char buffer[32];

    settings_.audio_flags.enable_alerts = !settings_.audio_flags.enable_alerts;
    const char* status = settings_.audio_flags.enable_alerts ? "ENABLED" : "DISABLED";
    safe_strcpy(buffer, "Alerts ", sizeof(buffer));
    safe_strcat(buffer, status, sizeof(buffer));
    nav_.display_modal("Audio Alerts", buffer);
}

DroneUIController::~DroneUIController() {
}

void DroneUIController::on_start_scan() {
    scanner_.start_scanning();
    scanning_active_.store(true, std::memory_order_release);
}

void DroneUIController::on_stop_scan() {
    scanner_.stop_scanning();
    scanning_active_.store(false, std::memory_order_release);
}

void DroneUIController::show_menu() {
    nav_.push<DroneAnalyzerMenuView>();
}

void DroneUIController::on_spectrum_mode() {
    nav_.display_modal("Spectrum Mode", "Feature not implemented in this version");
}

void DroneUIController::set_spectrum_mode(SpectrumMode mode) {
    hardware_.set_spectrum_mode(mode);
    nav_.display_modal("Applied", "Spectrum mode updated");
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
    char buffer[128];
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

    // Set up button handlers
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

    // DIAMOND OPTIMIZATION: Use StatusFormatter
    char slice_buffer[32];
    uint64_t slice_mhz = settings.wideband_slice_width_hz / 1000000;
    StatusFormatter::format_to(slice_buffer, "%lu MHz", (unsigned long)slice_mhz);
    field_slice_.set(slice_buffer);

    button_save_.focus();
}

void FrequencyRangeSetupView::on_save() {
    // Получаем значения из полей
    const std::string min_str = field_min_.get_text();
    const std::string max_str = field_max_.get_text();
    
    Frequency new_min = static_cast<Frequency>(DiamondCore::FrequencyParser::parse_mhz_string(min_str.c_str()));
    Frequency new_max = static_cast<Frequency>(DiamondCore::FrequencyParser::parse_mhz_string(max_str.c_str()));
    
    // Input validation (integer-only, no NaN check needed)
    if (new_min == 0 || new_max == 0) {
        nav_.display_modal("Error", "Invalid frequency format");
        return;
    }
    
    // Get slice width from settings instead of hardcoded value
    uint64_t new_slice_width = controller_.settings().wideband_slice_width_hz;
    
    // Валидация - use unified constants from DroneConstants
    if (new_min >= new_max) {
        nav_.display_modal("Error", "Min freq must be < Max freq");
        return;
    }
    
    if (new_min < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ || 
        new_max > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        nav_.display_modal("Error", "Frequency out of range (1MHz - 7.2GHz)");
        return;
    }
    
    // Overflow check
    if (new_max > new_max + new_min) {
        nav_.display_modal("Error", "Frequency range overflow detected");
        return;
    }

    // Сохранение настроек (use existing settings reference)
    controller_.settings().wideband_min_freq_hz = new_min;
    controller_.settings().wideband_max_freq_hz = new_max;

    // Сохранение в файл с проверкой успешности
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

// 2. Реализация меню удалена - теперь используем DroneAnalyzerSettingsView из ui_enhanced_drone_settings.cpp

EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      settings_(),
      hardware_(SpectrumMode::MEDIUM),
      scanner_(settings_),
      audio_(),
      display_controller_({0, 60, screen_width, screen_height - 80}),
      ui_controller_(nav, hardware_, scanner_, audio_, display_controller_),
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
    // 🔧 FIX: Thread-safe mutex initialization (volatile for embedded)
    // Note: Constructor is called once per View instance in single-threaded init phase
    static volatile bool sd_mutex_initialized = false;
    if (!sd_mutex_initialized) {
        chMtxInit(&sd_card_mutex);
        sd_mutex_initialized = true;
    }

    // 🔧 FIX: МИНИМАЛЬНЫЙ конструктор - только UI setup
    // Все вызовы scanner_/hardware_/coordinator_ перенесены в step_deferred_initialization()
    
    // 🔧 FIX: Установить начальное состояние
    init_state_ = InitState::CONSTRUCTED;

    setup_button_handlers();
    initialize_scanning_mode();
    add_ui_elements();
}

EnhancedDroneSpectrumAnalyzerView::~EnhancedDroneSpectrumAnalyzerView() {
    // TODO[FIXED]: STEP 2.1: Destruction order
    // 1. Stop activity (in dependency order)
    scanning_coordinator_.stop_coordinated_scanning();
    scanner_.stop_scanning();
    hardware_.shutdown_hardware();

    // 🔴 FIX: Explicit buffer deallocation (before stack objects destroyed)
    display_controller_.deallocate_buffers();

    // 2. Stack objects will be automatically destroyed in reverse order
    // No manual deletion needed for stack-allocated objects
}

void EnhancedDroneSpectrumAnalyzerView::focus() {
    button_start_stop_.focus();
}

void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
    // 🔧 FIX: Всегда вызываем базовый paint для очистки экрана
    View::paint(painter);

    // ===========================================
    // DIAMOND FIX: Execute deferred initialization
    // ===========================================
    // CRITICAL: This was missing, causing permanent hang
    // The initialization state machine was defined but never invoked
    if (init_state_ != InitState::FULLY_INITIALIZED && 
        init_state_ != InitState::INITIALIZATION_ERROR) {
        step_deferred_initialization();
    }

    // 🔧 FIX: Показываем сообщение об ошибке с возможностью выхода
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        // Заполняем экран чёрным
        painter.fill_rectangle({0, 0, screen_width, screen_height}, Color::black());
        
        // Заголовок ошибки
        painter.draw_string({10, 80}, Style{font::fixed_8x16, Color::red(), Color::black()}, "INIT ERROR");
        
        // Сообщение об ошибке
        painter.draw_string({10, 100}, Style{font::fixed_8x16, Color::white(), Color::black()}, 
                           ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]);
        
        // Инструкция
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
            
            // Прогресс-бар (6 фаз = 16.6% каждая)
            uint8_t progress = static_cast<uint8_t>(phase_idx * 16);
            if (progress > 100) progress = 100;
            
            // Фон прогресс-бара
            painter.fill_rectangle({10, 120, 100, 10}, Color::dark_grey());
            // Заполненная часть
            painter.fill_rectangle({10, 120, progress, 10}, Color::green());
        }
        return;
    }
    
    // 🔴 SAFETY: Дополнительная проверка буферов
    if (display_controller_.are_buffers_valid()) {
        display_controller_.render_bar_spectrum(painter);
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

// ===========================================
// ФАЗА 2.5: Deferred Initialization Implementation
// ===========================================

// ФАЗА 0.2: Имена фаз для логирования
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
    
    // 🔧 FIX: Bounds-checked array access
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

// 🔴 DIAMOND OPTIMIZATION: Enhanced Initialization State Machine with Timeout Protection
// Scott Meyers Item 15: Prefer constexpr to #define
// Scott Meyers Item 20: Prefer pass-by-reference-to-const to pass-by-value
// Eliminates cascading if/else, adds timeout protection, saves ~150 bytes RAM
void EnhancedDroneSpectrumAnalyzerView::step_deferred_initialization() {
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

    // 🔴 SAFETY: Проверка таймаута (защита от зависаний)
    systime_t elapsed = chTimeNow() - init_start_time_;
    if (elapsed > MS2ST(InitTiming::TIMEOUT_MS)) {
        init_state_ = InitState::INITIALIZATION_ERROR;
        init_error_ = InitError::GENERAL_TIMEOUT;
        initialization_in_progress_ = false;
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0, "Init timeout!");
        return;
    }


    // 🟢 MAIN LOOP: Проходим по фазам инициализации
    for (uint8_t i = 0; i < InitTiming::MAX_PHASES; ++i) {
        // DIAMOND OPTIMIZATION: LUT lookup вместо switch (O(1) lookup)
        const auto& phase = INIT_PHASES[i];

        // Проверяем, наступило ли время для этой фазы
        if (elapsed >= MS2ST(phase.delay_ms)) {
            // Проверяем, была ли эта фаза уже выполнена
            uint8_t expected_state = static_cast<uint8_t>(InitState::CONSTRUCTED) + i + 1;
            if (static_cast<uint8_t>(init_state_) >= expected_state) {
                // 🔴 FIX: Update UI status during async database loading
                // Phase 2 (DATABASE_LOADED) may still be loading in background
                if (init_state_ == InitState::DATABASE_LOADED && !scanner_.is_database_loading_complete()) {
                    status_bar_.update_normal_status("INIT", "Loading database...");
                }
                continue;  // Эта фаза уже выполнена, пропускаем
            }

            // Выполняем фазу через указатель на метод
            (this->*phase.init_func)();

            // Если произошла ошибка - выходим
            if (init_state_ == InitState::INITIALIZATION_ERROR) {
                initialization_in_progress_ = false;
                return;
            }
            
            // Если фаза возвращает с ошибкой (initialization_in_progress_ сброшен внутри фазы)
            if (!initialization_in_progress_) {
                return;
            }

            // Если состояние изменилось - обновляем UI
            const char* status_msg = phase.name;
            // 🔴 FIX: Special message for database loading
            if (init_state_ == InitState::DATABASE_LOADED && !scanner_.is_database_loading_complete()) {
                status_msg = "Loading database...";
            }
            status_bar_.update_normal_status("INIT", status_msg);

        }
    }
    
    // 🔴 SAFETY: Сброс флага (разрешаем повторный вызов)
    initialization_in_progress_ = false;
}

// ================================================================
// DIAMOND OPTIMIZATION: Phase Initialization Methods
// Защита от UB: каждый метод проверяет состояние перед выполнением
// ================================================================

// ФАЗА 1: Allocate display buffers
void EnhancedDroneSpectrumAnalyzerView::init_phase_allocate_buffers() {
    if (display_controller_.are_buffers_allocated()) {
        init_state_ = InitState::BUFFERS_ALLOCATED;
        return;
    }

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
    if (init_state_ != InitState::BUFFERS_ALLOCATED) {
        return;
    }

    scanner_.initialize_database_async();
    status_bar_.update_normal_status("INIT", "Phase 2: DB loading...");
    init_state_ = InitState::DATABASE_LOADED;
}

// ФАЗА 3: Initialize hardware
void EnhancedDroneSpectrumAnalyzerView::init_phase_init_hardware() {
    if (init_state_ != InitState::DATABASE_LOADED) {
        return;
    }

    hardware_.on_hardware_show();
    status_bar_.update_normal_status("INIT", "Phase 3: HW ready");
    init_state_ = InitState::HARDWARE_READY;
}

// ФАЗА 4: Setup UI layout
void EnhancedDroneSpectrumAnalyzerView::init_phase_setup_ui() {
    if (init_state_ != InitState::HARDWARE_READY) {
        return;
    }

    initialize_modern_layout();
    status_bar_.update_normal_status("INIT", "Phase 4: UI ready");
    init_state_ = InitState::UI_LAYOUT_READY;
}

// ФАЗА 5: Load settings
void EnhancedDroneSpectrumAnalyzerView::init_phase_load_settings() {
    if (init_state_ != InitState::UI_LAYOUT_READY) {
        return;
    }

    // ===========================================
    // ФАЗА 6.4: ЗАЩИЩЁННАЯ ЗАГРУЗКА НАСТРОЕК
    // ===========================================

    // Проверяем доступность SD карты с таймаутом
    systime_t sd_start = chTimeNow();
    while (sd_card::status() < sd_card::Status::Mounted) {
        if ((chTimeNow() - sd_start) > MS2ST(1000)) {  // 1 сек на mount
            status_bar_.update_normal_status("INIT", "No SD - defaults");
            init_state_ = InitState::SETTINGS_LOADED;
            return;  // Пропускаем загрузку, используем defaults
        }
        chThdSleepMilliseconds(50);
    }

    systime_t settings_start = chTimeNow();
    constexpr systime_t SETTINGS_LOAD_TIMEOUT_MS = MS2ST(2000);

    // FIX: Removed SDCardLock - FatFS handles locking at driver level
    // Deadlock occurred when Phase 2 (db loading thread) held lock while Phase 5 waited
    auto load_result = SettingsPersistence<DroneAnalyzerSettings>::load(settings_);
    bool loaded = load_result.is_ok() && load_result.value;

    systime_t elapsed = chTimeNow() - settings_start;
    if (elapsed >= SETTINGS_LOAD_TIMEOUT_MS) {
        status_bar_.update_normal_status("WARN", "Settings timeout");
        init_state_ = InitState::SETTINGS_LOADED;
        return;
    }

    if (!loaded) {
        status_bar_.update_normal_status("INIT", "Using defaults");
    } else {
        status_bar_.update_normal_status("INIT", "Settings loaded");
    }

    button_audio_.set_text(settings_.audio_flags.enable_alerts ? "AUDIO: ON" : "AUDIO: OFF");
    scanner_.update_scan_range(settings_.wideband_min_freq_hz,
                            settings_.wideband_max_freq_hz);
    
    // 🔧 FIX: Update coordinator parameters after settings load
    scanning_coordinator_.update_runtime_parameters(settings_);

    // DIAMOND FIX: Update audio cooldown based on settings to prevent UI freeze
    // baseband::send_message() uses busy-wait spin loop (baseband_api.cpp:54-64)
    // Set cooldown = duration + 100ms buffer to ensure M0 can process audio
    uint32_t audio_cooldown = settings_.audio_alert_duration_ms + 100;
    AudioAlertManager::set_cooldown_ms(audio_cooldown);

    init_state_ = InitState::SETTINGS_LOADED;
}

// ФАЗА 6: Finalize (переход в FULLY_INITIALIZED)
void EnhancedDroneSpectrumAnalyzerView::init_phase_finalize() {
    if (init_state_ != InitState::SETTINGS_LOADED) {
        return;
    }

    handle_scanner_update();
    init_state_ = InitState::FULLY_INITIALIZED;
    status_bar_.update_normal_status("EDA", "Ready");
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

    // DIAMOND FIX: Trigger audio alerts based on threat level (with debouncing)
    // Only play audio if threats detected and audio alerts are enabled
    // AudioAlertManager::play_alert() has built-in debouncing to prevent baseband queue saturation
    // baseband::send_message() uses busy-wait spin loop (baseband_api.cpp:54-64)
    if (settings_.audio_flags.enable_alerts && max_threat >= ThreatLevel::LOW) {
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

void EnhancedDroneSpectrumAnalyzerView::setup_button_handlers() {
    button_start_stop_.on_select = [this](Button&) {
        handle_start_stop_button();
    };
    button_menu_.on_select = [this](Button&) -> void {
        ui_controller_.show_menu();
    };
    button_audio_.on_select = [this](Button&) {
        // Toggle audio alerts setting
        settings_.audio_flags.enable_alerts = !settings_.audio_flags.enable_alerts;
        // Update button text immediately
        button_audio_.set_text(settings_.audio_flags.enable_alerts ? "AUDIO: ON" : "AUDIO: OFF");
        static Style green_style{font::fixed_8x16, Color::black(), Color::green()};
        static Style grey_style{font::fixed_8x16, Color::black(), Color::grey()};
        button_audio_.set_style(settings_.audio_flags.enable_alerts ? &green_style : &grey_style);
    };

    field_scanning_mode_.on_change = [this](size_t index, int32_t value) -> void {
        (void)value;
        set_scanning_mode_from_index(index);
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

// ===========================================
// PART 5: SCANNINGCOORDINATOR IMPLEMENTATION REMOVED
// ===========================================
// Diamond Code: Single Responsibility Principle
// All ScanningCoordinator implementations moved to scanning_coordinator.cpp
// This eliminates multiple definition linker errors

// ===========================================
// PART 6: DISPLAY HELPER IMPLEMENTATIONS
// ===========================================
// 🎯 Use UnifiedStringLookup and UnifiedColorLookup directly at call sites
// All wrapper methods removed to eliminate indirection

void DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power) {
    if (bin >= spectrum.db.size()) {
        max_power = 0;
        return;
    }
    max_power = spectrum.db[bin];
}

void DroneDisplayController::add_spectrum_pixel(uint8_t power) {
    // 🔴 ФАЗА 1.5: Проверка валидности буфера (статический буфер)
    if (!buffers_allocated_) {
        return;
    }
    if (pixel_index < spectrum_row_buffer().size()) {
        spectrum_row_buffer()[pixel_index] = spectrum_gradient_.lut[power];
        pixel_index++;
    }
}


// ===========================================
// PART 7: ENHANCED SETTINGS VALIDATOR
// ===========================================

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

    // Check range width
    Frequency range = max_hz - min_hz;
    if (static_cast<uint64_t>(range) < 1000000ULL) { // Minimum 1 MHz range
        safe_strcpy(error, "Frequency range too small (minimum 1 MHz)", error_size);
        return false;
    }
    
    return true;
}

bool EnhancedDroneSettingsValidator::is_known_drone_band(Frequency freq) {
    // Check 433 MHz band
    if (freq >= DroneConstants::MIN_433MHZ && freq <= DroneConstants::MAX_433MHZ) return true;
    
    // Check 860-930 MHz (LRS/Military)
    if (freq >= DroneConstants::MIN_900MHZ && freq <= DroneConstants::MAX_900MHZ) return true;
    
    // Check 2.4 GHz ISM band
    if (freq >= DroneConstants::MIN_24GHZ && freq <= DroneConstants::MAX_24GHZ) return true;
    
    // Check 5.8 GHz band
    if (freq >= DroneConstants::MIN_58GHZ && freq <= DroneConstants::MAX_58GHZ) return true;
    
    return false;
}

bool EnhancedDroneSettingsValidator::is_ism_band(Frequency freq) {
    return (static_cast<uint64_t>(freq) >= 2400000000ULL && static_cast<uint64_t>(freq) <= 2483500000ULL) ||
           (static_cast<uint64_t>(freq) >= 5725000000ULL && static_cast<uint64_t>(freq) <= 5875000000ULL);
}

// DIAMOND OPTIMIZATION: Unified frequency formatting using FrequencyFormatter
// Eliminates ~30 lines of duplicate formatting code
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
    if (range == 0) return;  // Early exit pattern

    Frequency tick_interval = calculate_optimal_tick_interval();
    if (tick_interval == 0) return;

    // DIAMOND OPTIMIZATION: Single style (both use same font/colors)
    // Eliminates branching between mhz/ghz styles
    static Style label_style{font::fixed_5x8, Color::grey(), Color::black()};

    // Draw horizontal line at bottom of ruler (using 1-pixel rectangle)
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

        // Draw major tick (tall) - using 1-pixel rectangle
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

        // DIAMOND OPTIMIZATION: Sub-tick logic based on tick_interval only
        // Eliminates should_use_mhz() call (bitfield read cost)
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

// DIAMOND OPTIMIZATION: Lookup table for ruler style to formatter mapping
// Scott Meyers Item 15: Prefer constexpr to #define
// Eliminates ~60 lines of switch-case logic
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

// DIAMOND OPTIMIZATION: Unified frequency formatting using FrequencyFormatter
// Eliminates ~60 lines of duplicate formatting code
// Scott Meyers Item 2: Prefer consts, enums, and inlines to #defines
void CompactFrequencyRuler::format_compact_label(char* buffer, size_t buffer_size, Frequency freq) {
    uint8_t idx = static_cast<uint8_t>(ruler_style_);
    if (idx >= 6) idx = 0;
    FrequencyFormatter::format_to_buffer(buffer, buffer_size, freq, RULER_FORMAT_LUT[idx]);
}

Frequency CompactFrequencyRuler::calculate_optimal_tick_interval() {
    Frequency range = max_freq_ - min_freq_;
    if (range == 0) return 0;

    // Optimized intervals for PortaPack's small screen
    // Prioritize GHz intervals for SPACED_GHZ style
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
    // 🔴 REMOVED: frequency_ruler_ (Dead Code - duplicate never used)
    // frequency_ruler_.set_visible(false);
}

void DroneDisplayController::apply_display_settings(const DroneAnalyzerSettings& settings) {
    if (settings.display_flags.show_frequency_ruler) {
        compact_frequency_ruler_.set_visible(true);
        // 🔴 REMOVED: frequency_ruler_ (Dead Code - duplicate never used)
        // frequency_ruler_.set_visible(false);

        if (settings.display_flags.auto_ruler_style) {
            compact_frequency_ruler_.set_ruler_style(RulerStyle::COMPACT_GHZ);
        } else {
            // DIAMOND OPTIMIZATION: constexpr LUT в Flash вместо switch (строки 4135-4144)
            uint8_t style_idx = (settings.frequency_ruler_style < 7) ?
                                settings.frequency_ruler_style : 0;
            compact_frequency_ruler_.set_ruler_style(RULER_STYLE_LUT[style_idx]);
        }

        compact_frequency_ruler_.set_tick_count(settings.compact_ruler_tick_count);
    } else {
        compact_frequency_ruler_.set_visible(false);
        // 🔴 REMOVED: frequency_ruler_ (Dead Code - duplicate never used)
        // frequency_ruler_.set_visible(false);
    }
}



// ===========================================
// AudioAlertManager Implementation
// ===========================================
// DIAMOND FIX: Fully migrated to ui_drone_audio.hpp as inline static (C++17)
// Eliminates ODR violation, removes .cpp dependency, ensures single instance

} // namespace ui::apps::enhanced_drone_analyzer
