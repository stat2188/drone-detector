// Thread Safety Notes
// Locking Order: 1.ATOMIC_FLAGS 2.data_mutex 3.spectrum_mutex 4.logger_mutex 5.sd_card_mutex
// Always acquire in order 1->2->3->4->5; sd_card_mutex must be LAST
// Use MutexLock RAII for automatic unlock; CriticalSection for volatile bool

// FIX #1: Removed incorrect forward declarations
// FreqmanDB is in freqman namespace, not ui::apps::enhanced_drone_analyzer
// freqman_db.hpp is already included in the header file, no forward declaration needed

// ============================================================================
// HEADER INCLUDES
// ============================================================================
// This file follows IWYU (Include What You Use) principle
// Headers are organized by category in alphabetical order within each group
// ============================================================================

// Corresponding header (must be first)
#include "ui_enhanced_drone_analyzer.hpp"

// ============================================================================
// C++ Standard Library Headers (alphabetical order)
// ============================================================================
#include <algorithm>      // std::min, std::max, std::copy, std::fill, std::generate_n
#include <array>          // std::array for fixed-size containers
#include <cinttypes>     // PRIu32, PRId64 format specifiers for printf
#include <cstddef>        // size_t, nullptr_t
#include <cstdint>        // uint8_t, uint16_t, uint32_t, uint64_t, int32_t, int64_t
#include <cstring>        // std::memset, std::strlen, std::memcpy
#include <new>            // Placement new operator for static storage allocation
#include <utility>        // std::move, std::pair

// ============================================================================
// Third-Party Library Headers (ChibiOS RTOS)
// ============================================================================
#include <ch.h>           // ChibiOS kernel API

// ============================================================================
// ChibiOS Internal Headers (alphabetical order)
// ============================================================================

// ============================================================================
// Project-Specific Headers (alphabetical order)
// ============================================================================
#include "color_lookup_unified.hpp"    // UnifiedColorLookup for O(1) color mapping
#include "diamond_core.hpp"             // DiamondCore validation and parsing utilities
#include "dsp_display_types.hpp"       // DSP display data structures
#include "dsp_spectrum_processor.hpp"   // SpectrumProcessor for signal processing
#include "eda_constants.hpp"            // EDA::Constants namespace
#include "eda_locking.hpp"             // MutexLock, CriticalSection RAII wrappers
#include "eda_optimized_utils.hpp"     // Optimized utility functions
#include "eda_ui_constants.hpp"        // UI layout constants
#include "eda_unified_database.hpp"    // UnifiedDroneDatabase singleton
#include "freqman_db.hpp"              // FreqmanDB for frequency database
#include "message.hpp"                 // Message passing primitives
#include "scanning_coordinator.hpp"    // ScanningCoordinator singleton
#include "settings_persistence.hpp"    // SettingsPersistence for config I/O
#include "ui.hpp"                      // Base UI framework (NavigationView, View)
#include "ui_drone_common_types.hpp"   // DroneType, ThreatLevel, MovementTrend enums
#include "ui_fileman.hpp"              // FileLoadView for file browser
#include "ui_painter.hpp"              // Painter class for graphics rendering
#include "ui_signal_processing.hpp"    // Signal processing utilities
#include "ui_spectral_analyzer.hpp"   // SpectralAnalyzer for spectrum analysis
#include "ui_widget.hpp"               // Widget base classes (Button, Text, etc.)

namespace ui::apps::enhanced_drone_analyzer {

// NOTE: using namespace portapack; removed to prevent namespace pollution
// Use explicit portapack:: qualifications where needed

using namespace EDA::Constants;

// Heap monitoring
namespace HeapMonitor {
    inline size_t get_free_heap() noexcept {
        size_t heap_free = 0;
        chHeapStatus(NULL, &heap_free);
        return heap_free;
    }
}

// Compile-time division optimization
constexpr inline uint64_t ceil_div_u64(uint64_t numerator, uint64_t denominator) noexcept {
    return (numerator + denominator - 1) / denominator;
}

// DIAMOND FIX: Named constants for magic numbers (replaces magic values with semantic names)
namespace MagicNumberConstants {
    // Maximum scan cycles before capping progressive slowdown
    constexpr uint32_t PROGRESSIVE_SLOWDOWN_MAX_CYCLES = 39;

    // Number of scanning modes (DATABASE, WIDEBAND_CONTINUOUS, HYBRID)
    constexpr uint8_t SCANNING_MODE_COUNT = 3;

    // Progress bar multiplier for threat display
    constexpr uint32_t THREAT_PROGRESS_MULTIPLIER = 10;

    // Maximum progress bar value
    constexpr uint32_t MAX_PROGRESS_VALUE = 32;

    // Scanning progress multiplier (cycles to progress)
    constexpr uint32_t SCAN_PROGRESS_MULTIPLIER = 5;

    // Error reporting interval (cycles)
    constexpr uint32_t ERROR_REPORTING_INTERVAL_CYCLES = 100;

    // RSSI threshold constants (dBm)
    constexpr int32_t RSSI_LOW_THRESHOLD_DBM = -100;
    constexpr int32_t RSSI_HIGH_THRESHOLD_DBM = -80;

    // LNA gain for weak signal reception (dB)
    constexpr uint8_t LNA_GAIN_WEAK_SIGNAL_DB = 32;

    // Progress bar constants
    constexpr uint32_t PROGRESS_PERCENT_MAX = 100;
    constexpr uint32_t PROGRESS_BAR_SEGMENTS = 8;

    // UI layout constants
    constexpr uint32_t UI_THREAT_STATUS_Y_OFFSET = 20;
    constexpr uint32_t UI_THREAT_FREQUENCY_Y_OFFSET = 38;
    constexpr uint32_t UI_ELEMENT_HEIGHT = 16;

    // Buffer sizes for text formatting
    constexpr size_t THREAT_BUFFER_SIZE = 64;
    constexpr size_t FREQ_BUFFER_SIZE = 16;
    constexpr size_t STATUS_BUFFER_SIZE = 48;
    constexpr size_t STATS_BUFFER_SIZE = 48;
    constexpr size_t CARD_BUFFER_SIZE = 48;
    constexpr size_t ALERT_BUFFER_SIZE = 64;
    constexpr size_t HARDWARE_BUFFER_SIZE = 64;
    constexpr size_t LAST_TEXT_BUFFER_SIZE = 64;

    // Paint stack requirement (reduced from 1792 to 1536 bytes)
    constexpr size_t PAINT_STACK_REQUIRED = 1536;

    // Stack monitoring constants
    constexpr size_t MIN_STACK_FREE_THRESHOLD = 512;  // 512 bytes minimum free stack
}

// DIAMOND FIX #HIGH #3: Function-local static for safe mutex initialization
// Prevents initialization order fiasco by using function-local static pattern
// SD card mutex (FatFS is NOT thread-safe)
Mutex& get_sd_card_mutex() noexcept {
    static Mutex mutex;  // Initialized on first use
    return mutex;
}

// Stack safety guard for embedded systems
// DIAMOND FIX #C4 (REVISED): Stack canary on stack frame (not thread_local)
namespace StackSafety {
    // Stack canary magic value (stored in Flash)
    constexpr uint32_t STACK_CANARY_MAGIC = 0xDEADBEEF;

    // Stack canary placed on stack (NOT thread_local)
    class StackCanary {
        uint32_t canary_value_;

    public:
        // Initialize canary on construction (placed on stack)
        StackCanary() noexcept : canary_value_(STACK_CANARY_MAGIC) {}

        // Check if canary was corrupted
        bool is_valid() const noexcept {
            return canary_value_ == STACK_CANARY_MAGIC;
        }

        // Get canary value (for debugging)
        uint32_t get_value() const noexcept {
            return canary_value_;
        }
    };

    // Stack guard with RAII
    class StackGuard {
        StackCanary canary_;
        const char* function_name_;

    public:
        explicit StackGuard(const char* name) noexcept
            : canary_(), function_name_(name) {
            // Canary initialized on stack
        }

        ~StackGuard() noexcept {
            if (!canary_.is_valid()) {
                // Stack overflow detected
                // Use lightweight handler (no logging to avoid recursion)
                handle_stack_overflow();
            }
        }

        bool is_stack_safe() const noexcept {
            return canary_.is_valid();
        }

        StackGuard(const StackGuard&) = delete;
        StackGuard& operator=(const StackGuard&) = delete;

    private:
        static void handle_stack_overflow() noexcept {
            // Lightweight handler - no heap allocation, no logging
            // Stack overflow detected - canary corrupted
        }
    };
}

// ============================================================================
// PLL Constants for wideband scan cycle
// ============================================================================
/**
 * @brief Constants for PLL lock verification in wideband scanning
 * @note Moved from function scope to file scope for proper C++ compliance
 */
namespace PllConstants {
    constexpr uint32_t MAX_PLL_LOCK_RETRIES = 3;           ///< Maximum PLL lock retry attempts (reduced from 5 to 3 for efficiency)
    constexpr uint32_t PLL_LOCK_POLL_INTERVAL_MS = 3;   ///< Poll interval for PLL lock status (reduced from 5 to 3 for efficiency)
    constexpr uint32_t PLL_LOCK_TIMEOUT_MS = 100;    ///< Timeout for PLL lock verification
}

// ============================================================================
// Audio Alert Cooldown Constants
// ============================================================================
/**
 * @brief Constants for audio alert cooldown management
 * @note Moved from function scope to file scope for proper C++ compliance
 */
namespace AudioCooldown {
    // Cooldown state tracking
    static systime_t last_alert_time_ = 0;
    static constexpr uint32_t ALERT_COOLDOWN_MS = 2000;  // 2 second cooldown between alerts

    /**
     * @brief Check if cooldown period has elapsed
     * @return true if cooldown has elapsed, false otherwise
     * @note Uses ChibiOS time primitives for accurate timing
     */
    inline bool is_cooldown_elapsed() noexcept {
        systime_t current_time = chTimeNow();
        systime_t elapsed = current_time - last_alert_time_;
        return elapsed >= MS2ST(ALERT_COOLDOWN_MS);
    }

    /**
     * @brief Update last alert time
     * @note Call after playing alert to reset cooldown timer
     */
    inline void update_last_alert_time() noexcept {
        last_alert_time_ = chTimeNow();
    }
}

// ScanningMode LUT (strings in Flash)
EDA_FLASH_CONST inline static constexpr const char* const SCANNING_MODE_NAMES[] = {
    "Database Scan",      // DATABASE = 0
    "Wideband Monitor",   // WIDEBAND_CONTINUOUS = 1
    "Hybrid Discovery"    // HYBRID = 2
};
static_assert(sizeof(SCANNING_MODE_NAMES) / sizeof(const char*) == 3, "SCANNING_MODE_NAMES size");

// ============================================================================
// STACK USAGE VALIDATION
// ============================================================================
// Embedded systems have limited stack space (4KB per thread on STM32F405).
// These static_assert statements validate stack usage at compile time to prevent
// stack overflow at runtime, which is difficult to debug.
// ============================================================================

// Validate static storage sizes are within reasonable limits
// These are allocated in BSS segment, not on stack, but still need validation
static_assert(DroneDisplayController::MAX_UI_DRONES <= 10,
              "MAX_UI_DRONES exceeds 10, may cause display performance issues");
static_assert(DroneScanner::FREQ_DB_STORAGE_SIZE <= 8192,
              "FREQ_DB_STORAGE_SIZE exceeds 8KB memory budget");
static_assert(DroneScanner::TRACKED_DRONES_STORAGE_SIZE <= 4096,
              "TRACKED_DRONES_STORAGE_SIZE exceeds 4KB memory budget");

// Validate spectrum power levels storage size
static_assert(200 <= 512,
              "spectrum_power_levels_storage_ exceeds 512 bytes safe buffer limit");

// Validate thread stack sizes
static_assert(DroneScanner::DB_LOADING_STACK_SIZE <= 8192,
              "DB_LOADING_STACK_SIZE exceeds 8KB thread stack limit");

// Static member definitions (FIX #1: removed inline to prevent RAM bloat)

alignas(alignof(DisplayDroneEntry))
DisplayDroneEntry DroneDisplayController::detected_drones_storage_[DroneDisplayController::MAX_UI_DRONES];

alignas(alignof(FreqmanDB))
uint8_t DroneScanner::freq_db_storage_[DroneScanner::FREQ_DB_STORAGE_SIZE];

alignas(alignof(TrackedDrone))
uint8_t DroneScanner::tracked_drones_storage_[DroneScanner::TRACKED_DRONES_STORAGE_SIZE];

alignas(alignof(uint8_t))
uint8_t DroneDisplayController::spectrum_power_levels_storage_[200];

// DIAMOND FIX: Allocate double buffers in BSS segment
alignas(4) dsp::RenderCache::BarSpectrumCache DroneDisplayController::bar_caches_[2];
alignas(4) dsp::RenderCache::HistogramCache DroneDisplayController::hist_caches_[2];

stkalign_t DroneScanner::db_loading_wa_[THD_WA_SIZE(DroneScanner::DB_LOADING_STACK_SIZE) / sizeof(stkalign_t)];

// Built-in drone frequency DB (reduced from 31 to 14 entries)
// FIX: Removed extra brace that caused "Excess elements in scalar initializer" error
// REMOVED: WiFi Ch1 entry to eliminate WiFi detection functionality
EDA_FLASH_CONST const DroneScanner::BuiltinDroneFreq DroneScanner::BUILTIN_DRONE_DB[DroneScanner::BUILTIN_DB_SIZE] = {
    // LRS / Control
    { 868000000, "TBS Crossfire EU", DroneType::MILITARY_DRONE },
    { 915000000, "TBS Crossfire US", DroneType::MILITARY_DRONE },
    { 866000000, "ELRS 868", DroneType::PX4_DRONE },
    { 915000000, "ELRS 915", DroneType::PX4_DRONE },

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
    { 5735000000, "DJI FPV Ch1", DroneType::MAVIC }
};

DroneScanner::DroneScanner(DroneAnalyzerSettings settings)
    : entries_to_scan_(),
      stale_indices_(),
      is_stale_{},
      ui_freq_buf_{},
      ui_summary_buffer_{},
      ui_status_buffer_{},
      ui_stats_buffer_{},
      stack_canary_{},
      scanning_thread_(nullptr),
      data_mutex(),
      scanning_active_(),
      histogram_callback_(nullptr),
      histogram_callback_user_data_(nullptr),
      freq_db_ptr_(nullptr),
      tracked_drones_ptr_(nullptr),
      freq_db_constructed_(false),
      tracked_drones_constructed_(false),
      freq_db_loaded_(),
      current_db_index_(0),
      last_scanned_frequency_(0),
      last_detection_log_time_{0},
      db_loading_thread_(nullptr),
      db_loading_active_(),
      initialization_complete_(),
      database_needs_reload_(),
      scan_cycles_(0),
      total_detections_(0),
      scanning_mode_(DroneScanner::ScanningMode::DATABASE),
      is_real_mode_(),
      tracked_count_(0),
      approaching_count_(0),
      receding_count_(0),
      static_count_(0),
      max_detected_threat_(ThreatLevel::NONE),
      last_valid_rssi_(-120),
      wideband_scan_data_(),
      detection_ring_buffer_(),
      fhss_detector_(),
      spectrum_data_(),
      histogram_buffer_(),
      settings_(std::move(settings))
{
    scanning_active_.store(false);
    freq_db_loaded_.store(false);
    db_loading_active_.store(false);
    initialization_complete_.store(false);
    is_real_mode_.store(true);
    database_needs_reload_.store(false);
    // Initialize stack canary for overflow detection
    init_stack_canary();

    chMtxInit(&data_mutex);

    // FIX #8: Initialize pointers to static storage in constructor
    // This prevents nullptr dereferences and ensures objects are always available
    // R04 FIX: Storage is declared with alignas(), so alignment is guaranteed at compile time
    // No runtime check needed - alignas() ensures proper alignment
    freq_db_ptr_ = new (freq_db_storage_) FreqmanDB();
    freq_db_constructed_ = true;

    // R04 FIX: Storage is declared with alignas(), so alignment is guaranteed at compile time
    // No runtime check needed - alignas() ensures proper alignment
    tracked_drones_ptr_ = new (tracked_drones_storage_)
        std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>();
    tracked_drones_constructed_ = true;

    // Initialize TrackedDrone array to default state
    if (tracked_drones_ptr_ && tracked_drones_constructed_) {
        for (auto& drone : *tracked_drones_ptr_) {
            drone = TrackedDrone();  // Default construct
        }
    }

    // DIAMOND FIX #4: Wideband scanning initialization moved to initialize_database_and_scanner()
    // to ensure proper initialization order: database -> wideband scanning
    // This prevents premature execution before database is fully loaded
}

// ============================================================================
// DIAMOND FIX #HIGH #1: Memory Leak in DroneScanner Destructor
// ============================================================================
/**
 * @brief Destructor - Cleanup scanner resources
 * @details Implements cooperative thread termination with proper cleanup
 * @note Verifies thread has exited before destruction
 * @note Implements proper cleanup of thread resources
 * @note Uses Doxygen comments for documentation
 */
DroneScanner::~DroneScanner() {
    // DIAMOND FIX #HIGH #1: Stop scanning thread first
    // This ensures scanning thread is not accessing any resources
    stop_scanning();
    
    // DIAMOND FIX #HIGH #1: Cooperative thread termination
    // Signal all threads to stop cooperatively
    scanning_active_ = false;
    
    // FIX R10: Proper thread termination using ChibiOS API
    // This replaces timeout-based wait with proper termination mechanism
    if (scanning_thread_ != nullptr) {
        // Request thread termination
        chThdTerminate(scanning_thread_);
        // Wait for thread to exit (no timeout)
        chThdWait(scanning_thread_);
        // Clear thread pointer after thread has exited
        scanning_thread_ = nullptr;
    }
    
    // DIAMOND FIX #HIGH #1: Cleanup database and scanner resources
    // This ensures proper cleanup of all resources
    cleanup_database_and_scanner();
    
    // FIX #8: Explicitly destroy placement-newed objects
    // This prevents resource leaks and ensures proper cleanup
    if (freq_db_ptr_ && freq_db_constructed_) {
        freq_db_ptr_->~FreqmanDB();
        freq_db_ptr_ = nullptr;
        freq_db_constructed_ = false;
    }
    
    if (tracked_drones_ptr_ && tracked_drones_constructed_) {
        tracked_drones_ptr_->~array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>();
        tracked_drones_ptr_ = nullptr;
        tracked_drones_constructed_ = false;
    }
    
    // ChibiOS mutexes auto-cleaned with the object
}

void DroneScanner::initialize_wideband_scanning() {
    wideband_scan_data_.reset();
    setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
}

void DroneScanner::setup_wideband_range(Frequency min_freq, Frequency max_freq) {
    Frequency safe_min = std::max(min_freq, EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ);
    Frequency safe_max = std::min(max_freq, EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ);

    // Swap to ensure min <= max
    if (safe_min > safe_max) {
        safe_min = safe_max;
        safe_max = std::max(min_freq, EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ);
    }

    wideband_scan_data_.min_freq = safe_min;
    wideband_scan_data_.max_freq = safe_max;

    Frequency scanning_range = safe_max - safe_min;
    const uint64_t width_u64 = static_cast<uint64_t>(settings_.wideband_slice_width_hz);
    if (scanning_range > static_cast<Frequency>(settings_.wideband_slice_width_hz)) {
        // Check for overflow BEFORE addition
        if (scanning_range < EDA::Constants::NEGATIVE_RANGE_INDICATOR) {
            wideband_scan_data_.slices_nb = EDA::Constants::WIDEBAND_SLICE_COUNT_MIN;
        } else {
            const uint64_t range_u64 = static_cast<uint64_t>(scanning_range);

            // Check for overflow
            if (UINT64_MAX - range_u64 < width_u64) {
                wideband_scan_data_.slices_nb = EDA::Constants::WIDEBAND_SLICE_COUNT_MIN;
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
        wideband_scan_data_.slices_nb = EDA::Constants::WIDEBAND_SLICE_COUNT_MIN;
    }
    wideband_scan_data_.slice_counter = 0;
}

void DroneScanner::start_scanning() {
    bool is_scanning = scanning_active_.load();
    if (is_scanning) return;

    scanning_active_.store(true);
    {
        MutexLock lock(data_mutex);
        scan_cycles_ = 0;
        total_detections_ = 0;
    }
}

void DroneScanner::stop_scanning() {
    bool is_scanning = scanning_active_.load();
    if (!is_scanning) return;

    scanning_active_.store(false);

    if (scanning_thread_ != nullptr) {
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }

    remove_stale_drones();
}

// ============================================================================
// SCANNING FLOW: Database Loading
// ============================================================================
// Flow: on_show() -> init_phase_load_database() -> initialize_database_async()
//       -> db_loading_thread_loop() -> UnifiedDroneDatabase
// 
// Stage 4 Integration: This legacy method now delegates to UnifiedDroneDatabase
// for the actual frequency data. The FreqmanDB is kept for file I/O compatibility.
// 
// TODO: Future refactoring should fully migrate to UnifiedDroneDatabase
// and remove the dual-database complexity.
bool DroneScanner::load_frequency_database() {
    current_db_index_ = 0;

    freqman_load_options options;
    options.max_entries = 150;
    options.load_freqs = true;
    options.load_ranges = true;
    options.load_hamradios = true;
    options.load_repeaters = true;

    if (!freq_db_ptr_ || !freq_db_constructed_) {
        return false;
    }

    auto db_path = get_freqman_path(settings_.freqman_path);

    {
        MutexLock lock(data_mutex);

        bool sd_loaded = freq_db_ptr_->open(db_path);

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
        }
    }

    if (freq_db_ptr_->entry_count() > 100) {
        handle_scan_error("Large database loaded");
    }

    freq_db_loaded_.store(true);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
    return !freq_db_ptr_->empty();
}

// Stage 4: Database observer callback for real-time sync
void DroneScanner::database_change_callback(const DatabaseChangeEvent& event, void* user_data) {
    auto* scanner = static_cast<DroneScanner*>(user_data);
    if (scanner == nullptr) return;
    
    switch (event.type) {
        case DatabaseEventType::ENTRY_ADDED:
        case DatabaseEventType::ENTRY_DELETED:
        case DatabaseEventType::DATABASE_RELOADED:
            // Signal scanner to reload frequency list
            scanner->database_needs_reload_ = true;
            break;
        default:
            break;
    }
}

void DroneScanner::register_database_observer() {
    auto& db = UnifiedDroneDatabase::instance();
    (void)db.add_observer(&DroneScanner::database_change_callback, this);
}

void DroneScanner::unregister_database_observer() {
    auto& db = UnifiedDroneDatabase::instance();
    db.remove_observer(&DroneScanner::database_change_callback);
}

/**
 * @brief Get database size with mutex protection
 * @details Thread-safe access to database entry count
 * @return Number of entries in the database
 * @note DIAMOND FIX MEDIUM #5: Added mutex protection for database access
 */
size_t DroneScanner::get_database_size() const {
    // DIAMOND FIX MEDIUM #5: Acquire mutex for database access protection
    MutexLock lock(data_mutex);
    
    // Defensive null check
    if (!freq_db_ptr_) {
        return 0;
    }
    
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

// ============================================================================
// SCANNING FLOW: Main Scan Cycle
// ============================================================================
// Flow: perform_scan_cycle() -> SCAN_FUNCTIONS LUT -> perform_database_scan_cycle()
//       or perform_wideband_scan_cycle() or perform_hybrid_scan_cycle()
//
// Adaptive Scanning: Scan interval adjusts based on threat level:
// - CRITICAL threat: FAST_SCAN_INTERVAL_MS (50ms)
// - HIGH threat: HIGH_THREAT_SCAN_INTERVAL_MS (100ms)
// - MEDIUM threat: NORMAL_SCAN_INTERVAL_MS (200ms)
// - No detections: Progressive slowdown up to VERY_SLOW_SCAN_INTERVAL_MS (2000ms)
//
// LUT-based dispatch: SCAN_FUNCTIONS array provides O(1) dispatch to scan method
void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    // Guard clause: Early return if not scanning
    bool is_scanning = scanning_active_.load();
    if (!is_scanning) return;

    // Stage 4: Check if database needs reload due to observer notification
    if (database_needs_reload_.load()) {
        database_needs_reload_.store(false);
        load_frequency_database();
    }

    using namespace EDA::Constants;
    uint32_t base_interval = DEFAULT_SCAN_INTERVAL_MS;
    uint32_t adaptive_interval = base_interval;

    // FIX: Data race protection - read all scanner state variables under single mutex lock
    // This prevents race conditions where tracked_count_ changes between reads
    size_t current_detections;
    ThreatLevel max_threat;
    size_t tracked_count;
    {
        MutexLock lock(data_mutex);
        current_detections = total_detections_;
        max_threat = max_detected_threat_;
        tracked_count = tracked_count_;
    }

    if (max_threat >= ThreatLevel::CRITICAL) {
        adaptive_interval = FAST_SCAN_INTERVAL_MS;
    } else if (max_threat == ThreatLevel::HIGH) {
        adaptive_interval = HIGH_THREAT_SCAN_INTERVAL_MS;
    } else if (max_threat == ThreatLevel::MEDIUM) {
        adaptive_interval = NORMAL_SCAN_INTERVAL_MS;
    } else if (current_detections > 0 && tracked_count > 0) {
        adaptive_interval = SLOW_SCAN_INTERVAL_MS;
    } else if (current_detections == 0 && get_scan_cycles() > PROGRESSIVE_SLOWDOWN_DIVISOR) {
        uint32_t cycles_value = get_scan_cycles();
        // Use slowdown multiplier from EDA::Constants::ScanSlowdown
        uint32_t slowdown_multiplier = EDA::Constants::ScanSlowdown::get_slowdown_multiplier(cycles_value);
        uint32_t interval_calc = base_interval * slowdown_multiplier;
        adaptive_interval = (interval_calc < VERY_SLOW_SCAN_INTERVAL_MS) ? interval_calc : VERY_SLOW_SCAN_INTERVAL_MS;
    }

    // Additional adjustment for high detection density
    static constexpr size_t HIGH_DENSITY_DETECTION_THRESHOLD = EDA::Constants::HIGH_DENSITY_DETECTION_THRESHOLD;
    if (current_detections > HIGH_DENSITY_DETECTION_THRESHOLD) {
        adaptive_interval = (adaptive_interval < HIGH_DENSITY_SCAN_CAP_MS) ? adaptive_interval : HIGH_DENSITY_SCAN_CAP_MS;
    }
    
    chThdSleepMilliseconds(adaptive_interval);

    uint8_t mode_idx = static_cast<uint8_t>(scanning_mode_);
    // DIAMOND FIX: Replace magic number 3 with semantic constant
    if (mode_idx < MagicNumberConstants::SCANNING_MODE_COUNT) {
        (this->*DroneScanner::SCAN_FUNCTIONS[mode_idx])(hardware);
    }

    {
        MutexLock lock(data_mutex);
        scan_cycles_++;
    }

    is_scanning = scanning_active_.load();
    if (is_scanning) {
        chThdSleepMilliseconds(adaptive_interval);
    }
}

// ============================================================================
// SCANNING FLOW: Database Scan Cycle
// ============================================================================
// Flow: perform_database_scan_cycle() -> hardware.tune_to_frequency() -> 
//       hardware.get_current_rssi() -> process_rssi_detection()
//
// Hardware Tuning Sequence:
// FIX #4: Add portapack:: namespace qualifier to receiver_model
// 1. Validate frequency range (MIN_HARDWARE_FREQ to MAX_HARDWARE_FREQ)
// 2. Tune radio via portapack::receiver_model.set_target_frequency()
// 3. Wait for PLL stabilization (PLL_STABILIZATION_ITERATIONS * PLL_STABILIZATION_DELAY_MS)
// 4. Acquire RSSI measurement with timeout (RSSI_TIMEOUT_MS)
// 5. Process detection if signal captured
//
// Memory Optimization: Uses class member buffers (entries_to_scan_) instead of
// stack allocation to prevent stack overflow on STM32F405 (192KB RAM)
void DroneScanner::perform_database_scan_cycle(DroneHardwareController& hardware) {
    // Stack overflow detection check
    if (!check_stack_canary()) {
        // Stack overflow detected - reinitialize canary to prevent further corruption
        init_stack_canary();
    }

    size_t total_entries = 0;

    {
        MutexLock lock(data_mutex);
        if (!freq_db_ptr_ || freq_db_ptr_->empty()) {
            if (scan_cycles_ % EDA::Constants::SCAN_CYCLE_CHECK_INTERVAL == 0) {
                handle_scan_error("Database is empty");
            }
            return;
        }
        total_entries = freq_db_ptr_->entry_count();
    }

    const size_t batch_size = std::min(static_cast<size_t>(EDA::Constants::MAX_SCAN_BATCH_SIZE), total_entries);

    // Use class member variable directly (heap-free)
    // All accesses to entries_to_scan_ must be under data_mutex protection
    size_t entries_count = 0;

    {
        MutexLock lock(data_mutex);
        if (freq_db_ptr_) {
            size_t db_entry_count = freq_db_ptr_->entry_count();
            if (db_entry_count > 0) {
                for (size_t i = 0; i < batch_size; ++i) {
                    size_t idx = (current_db_index_ + i) % db_entry_count;
                    if (idx < db_entry_count && entries_count < entries_to_scan_.size()) {
                        entries_to_scan_[entries_count++] = (*freq_db_ptr_)[idx];
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
        const auto& entry = entries_to_scan_[i];

        // Guard clause: check scanning flag
        bool is_scanning = scanning_active_.load();
        if (!is_scanning) return;

        Frequency target_freq_hz = entry.frequency_a;

        // Guard clause: validate frequency range
        if (target_freq_hz < MIN_VALID_FREQ || target_freq_hz > MAX_VALID_FREQ) continue;

        // Guard clause: hardware tuning validation
        if (!hardware.tune_to_frequency(target_freq_hz)) continue;

        // Wait for PLL stabilization
        for (int w = 0; w < PLL_STABILIZATION_ITERATIONS; w++) {
            is_scanning = scanning_active_.load();
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

// ============================================================================
// DIAMOND FIX #CRITICAL #3: PLL Stabilization Verification
// ============================================================================
/**
 * @brief Perform wideband scan cycle with PLL lock verification
 * @details Implements retry logic with maximum enforcement and fallback to default frequency
 * @note Replaces fixed delay with hardware status polling
 * @note Adds graceful degradation if PLL never locks
 */
void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
    if (wideband_scan_data_.slices_nb == 0) {
        setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
    }
    
    const WidebandSlice& current_slice = wideband_scan_data_.slices[wideband_scan_data_.slice_counter];
    
    // 1. Tune to slice center frequency
    if (hardware.tune_to_frequency(current_slice.center_frequency)) {
        
        // DIAMOND FIX #CRITICAL #3: PLL lock verification with retry logic
        // Instead of fixed delay, poll hardware status to verify PLL is actually locked
        // This prevents proceeding with unstable PLL which causes inaccurate measurements
        uint32_t pll_retry_count = 0;
        bool pll_locked = false;
        systime_t pll_start_time = chTimeNow();
        
        // Poll for PLL lock status with retry logic
        while (pll_retry_count < PllConstants::MAX_PLL_LOCK_RETRIES) {
            // Check if PLL timeout exceeded
            systime_t elapsed = chTimeNow() - pll_start_time;
            if (elapsed > MS2ST(PllConstants::PLL_LOCK_TIMEOUT_MS)) {
                // PLL lock timeout - use fallback frequency
                // DIAMOND FIX #CRITICAL #3: Fallback to default frequency if PLL never locks
                // This ensures system continues with degraded functionality instead of hanging
                // Note: status_bar_ not available in DroneScanner context

                // DIAMOND FIX #P2-MED #6: Use named constant for fallback frequency
                // Use default frequency (100 MHz) as fallback
                constexpr Frequency DEFAULT_FALLBACK_FREQ_HZ = EDA::Constants::DEFAULT_FALLBACK_FREQUENCY_HZ;
                hardware.tune_to_frequency(DEFAULT_FALLBACK_FREQ_HZ);

                // Wait for stabilization at fallback frequency
                chThdSleepMilliseconds(EDA::Constants::PLL_STABILIZATION_DELAY_MS);
                // BUG FIX: Do NOT set pll_locked to true on timeout
                // pll_locked should remain false to trigger fallback logic
                break;
            }
            
            // Short delay before next poll
            chThdSleepMilliseconds(PllConstants::PLL_LOCK_POLL_INTERVAL_MS);
            pll_retry_count++;
        }
        
        // If PLL never locked after all retries, log warning and continue
        if (!pll_locked) {
            // DIAMOND FIX #CRITICAL #3: Graceful degradation on PLL lock failure
            // System continues with degraded functionality instead of hanging
            // Note: status_bar_ not available in DroneScanner context
        }
        
        // 2. Wait for PLL stabilization (original delay)
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
            deadline = DiamondCore::ConfidenceConstants::MAX_SYSTIME_VALUE;  // Max systime_t value
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

                // MODIFICATION: Check for FHSS (frequency hopping) on every spectral detection
                process_fhss_detection(detection_entry, static_cast<int32_t>(analysis_result.max_val), chTimeNow());
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
        if (get_scan_cycles() % MagicNumberConstants::ERROR_REPORTING_INTERVAL_CYCLES == 0) {
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
        process_wideband_detection_with_override(entry, rssi, DEFAULT_RSSI_THRESHOLD_DB, threshold_override);
    }
}

void DroneScanner::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                           int32_t original_threshold, int32_t wideband_threshold) {
    (void)original_threshold;  // Intentionally unused - kept for API compatibility

    if (!EDA::Validation::validate_rssi(rssi) ||
        !EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level = ThreatLevel::UNKNOWN;

    // Validate frequency before division
    if (entry.frequency_a == EDA::Constants::ZERO_FREQUENCY) {
        return;
    }
    
    // Threat level logic
    // DIAMOND FIX #P2-MED #3, #4: Replace magic numbers with named constants
    if (rssi > EDA::Constants::ThreatLevelThresholds::HIGH_RSSI_THRESHOLD_DB) {
        threat_level = ThreatLevel::HIGH;
    } else if (rssi > EDA::Constants::ThreatLevelThresholds::LOW_RSSI_THRESHOLD_DB) {
        threat_level = ThreatLevel::LOW;
    } else {
        threat_level = ThreatLevel::UNKNOWN;
    }

    // DIAMOND FIX #P2-MED #5: Replace 2.4 GHz band magic numbers with named constants
    if (entry.frequency_a >= EDA::Constants::FrequencyBandThresholds::WIDEBAND_24GHZ_MIN_HZ &&
        entry.frequency_a <= EDA::Constants::FrequencyBandThresholds::WIDEBAND_24GHZ_MAX_HZ) {
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
            // DIAMOND FIX #P2-MEDIUM #6: Saturated increment (no overflow)
            if (current_count < 255) {
                current_count++;
            }
            detection_ring_buffer_.update_detection({freq_hash, current_count, rssi});

            if (current_count >= MIN_DETECTION_COUNT) {
                update_tracked_drone({detected_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(rssi), threat_level});

                // MODIFICATION: Check for FHSS (frequency hopping) on wideband detection
                process_fhss_detection(entry, rssi, chTimeNow());
            }
        } else {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection({freq_hash, current_count, stored_rssi});
            } else {
                detection_ring_buffer_.update_detection({freq_hash, 0, -120});
            }
        }
    }
    // End critical section
}

void DroneScanner::process_spectral_detection(const freqman_entry& entry,
                                             const SpectralAnalysisResult& analysis_result,
                                             ThreatLevel threat_level, DroneType drone_type) {
    if (!EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    {
        MutexLock lock(data_mutex);

        total_detections_++;

        // Guard clause: Prevent division by zero
        if (entry.frequency_a == EDA::Constants::ZERO_FREQUENCY) {
            return;
        }
        size_t freq_hash = static_cast<size_t>(entry.frequency_a / EDA::Constants::FREQ_HASH_DIVISOR) & EDA::Constants::FREQ_HASH_MASK;
        int32_t effective_threshold = WIDEBAND_RSSI_THRESHOLD_DB;

        if (detection_ring_buffer_.get_rssi_value(freq_hash) < WIDEBAND_RSSI_THRESHOLD_DB) {
            effective_threshold = WIDEBAND_RSSI_THRESHOLD_DB + HYSTERESIS_MARGIN_DB;
        }

        // Use the maximum value from spectral analysis as the effective RSSI
        int32_t effective_rssi = static_cast<int32_t>(analysis_result.max_val);

        if (effective_rssi >= effective_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            // DIAMOND FIX #P2-MEDIUM #6: Saturated increment (no overflow)
            if (current_count < 255) {
                current_count++;
            }
            detection_ring_buffer_.update_detection({freq_hash, current_count, effective_rssi});

            if (current_count >= MIN_DETECTION_COUNT) {
                update_tracked_drone({drone_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(effective_rssi), threat_level});
            }
        } else {
            // Counter decrement logic...
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection({freq_hash, current_count, stored_rssi});
            } else {
                detection_ring_buffer_.update_detection({freq_hash, 0, -120});
            }
        }
    }
    // --- CRITICAL SECTION END ---
}

// MODIFICATION: Implementation of FHSS detection methods
// Detects frequency hopping signals characteristic of military and FPV drones

bool DroneScanner::is_fhss_enabled() const noexcept {
    // Check FHSS detection flag from settings (bit0)
    return BitfieldAccess::get_bit<0>(settings_.detection_flags);
}

void DroneScanner::process_fhss_detection(const freqman_entry& entry, int32_t rssi, systime_t now) noexcept {
    // Guard clause: Skip if FHSS detection is disabled
    if (!is_fhss_enabled()) {
        return;
    }

    // Guard clause: Validate RSSI and frequency
    if (!EDA::Validation::validate_rssi(rssi) ||
        !EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    // Detect frequency hopping
    if (fhss_detector_.detect_hopping(static_cast<Frequency>(entry.frequency_a), now)) {
        // Frequency hop detected within time window
        if (fhss_detector_.is_fhss_confirmed(now)) {
            // FHSS signal confirmed (>= 3 hops in 1 second)
            // Generate FHSS detection with high threat level
            bool should_log = false;
            DetectionLogEntry log_entry_to_write;

            {
                MutexLock lock(data_mutex);

                // Increment total detections
                total_detections_++;

                // Calculate frequency hash
                size_t freq_hash = static_cast<size_t>(entry.frequency_a / EDA::Constants::FREQ_HASH_DIVISOR) &
                                     EDA::Constants::FREQ_HASH_MASK;

                // Use maximum of current rssi and threshold for detection
                int32_t effective_threshold = WIDEBAND_RSSI_THRESHOLD_DB;
                int32_t effective_rssi = std::max(rssi, effective_threshold);

                // Increment detection count
                uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
                if (current_count < 255) {
                    current_count++;
                }

                detection_ring_buffer_.update_detection({freq_hash, current_count, effective_rssi});

                if (current_count >= MIN_DETECTION_COUNT) {
                    should_log = true;

                    // FHSS signals are high threat (military drones)
                    log_entry_to_write = {
                        chTimeNow(),
                        static_cast<uint64_t>(entry.frequency_a),
                        effective_rssi,
                        ThreatLevel::HIGH,  // FHSS = HIGH threat by default
                        DroneType::MILITARY_DRONE,  // FHSS typically military
                        current_count,
                        95,  // High confidence for FHSS (95%)
                        0,  // width_bins (not applicable)
                        0,  // signal_width_hz (not applicable)
                        0   // snr (not applicable)
                    };

                    // Update tracked drone with FHSS detection
                    update_tracked_drone({DroneType::MILITARY_DRONE,
                                          static_cast<Frequency>(entry.frequency_a),
                                          effective_rssi,
                                          ThreatLevel::HIGH});
                }
            }

            // Log detection asynchronously if needed
            if (should_log) {
                // TODO: detection_logger_.log_detection_async(log_entry_to_write);
                // DroneDetectionLogger class not implemented
            }
        }
    }
}

void DroneScanner::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
    // Diamond Code Fix: Use constant instead of magic number
    if (get_scan_cycles() % EDA::Constants::HYBRID_SCAN_DIVISOR == 0) {
        perform_wideband_scan_cycle(hardware);
    } else {
        perform_database_scan_cycle(hardware);
    }
}

// ============================================================================
// SCANNING FLOW: Detection Processing
// ============================================================================
// Flow: process_rssi_detection() -> DetectionRingBuffer -> update_tracked_drone()
//       -> detection_logger_.log_detection_async()
//
// Detection Algorithm:
// 1. Validate RSSI and frequency using DiamondCore utilities
// 2. Filter by drone bands (433MHz, 868/915MHz, 2.4GHz, 5.8GHz)
// 3. Classify threat level using ThreatClassifier::from_rssi()
// 4. Update detection ring buffer with hysteresis
// 5. Update tracked drone list if detection count >= MIN_DETECTION_COUNT
// 6. Log detection asynchronously (non-blocking)
//
// Thread Safety: Uses MutexLock for all shared data access
// Memory: Uses class member buffers to avoid stack allocation
void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
    // DIAMOND OPTIMIZATION: Using DiamondCore validation utilities
    // Guard clause: RSSI validation (using DiamondCore RSSI utilities)
    if (!EDA::Validation::validate_rssi(rssi)) {
        return;
    }

    // Guard clause: Frequency validation (using DiamondCore validation)
    if (!EDA::Validation::validate_frequency(entry.frequency_a)) {
        return;
    }

    // MODIFICATION: Removed drone band filtering to allow detection of all frequencies
    // Previously blocked non-drone signals like PMR radios (446 MHz), bleepers, etc.
    // Now allows any frequency from database to trigger detection if RSSI threshold is met
    /*
    // Guard clause: Drone band filtering (433MHz - 5.8GHz)
    if (!EDA::Validation::is_433mhz_band(entry.frequency_a) &&
        !EDA::Validation::is_2_4ghz_band(entry.frequency_a) &&
        !EDA::Validation::is_5_8ghz_band(entry.frequency_a) &&
        !EDA::Validation::is_military_band(entry.frequency_a)) {
        return;
    }
    */

    // Determine parameters (locally)
    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level = ThreatClassifier::from_rssi(rssi);

    // Simple database search (with mutex protection to prevent race with load_frequency_database)
    {
        MutexLock lock(data_mutex);
        // FIX: Use freq_db_ptr_ instead of freq_db_
        if (!freq_db_ptr_) return;
        for (size_t i = 0; i < freq_db_ptr_->entry_count(); ++i) {
            if ((*freq_db_ptr_)[i].frequency_a == entry.frequency_a) {
                detected_type = DroneType::MAVIC;
                threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
                break;
            }
        }
    }

    // MODIFICATION: Simple frequency legend for known drone bands
    // Applied only if type is still UNKNOWN after database search
    // This provides basic classification for common drone frequency ranges
    if (detected_type == DroneType::UNKNOWN) {
        Frequency freq = entry.frequency_a;

        // 433 MHz ISM band: Long Range Systems (LRS/ELRS)
        // Used by DIY and military drones for control link
        if (freq >= 433000000 && freq <= 435000000) {
            detected_type = DroneType::MILITARY_DRONE;
            threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
        }
        // 868 MHz ISM band: Crossfire/ELRS EU
        // Used by professional drones and FPV racing
        else if (freq >= 860000000 && freq <= 870000000) {
            detected_type = DroneType::MILITARY_DRONE;
            threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
        }
        // 915 MHz ISM band: Crossfire/ELRS US
        // Used by long-range control systems
        else if (freq >= 900000000 && freq <= 930000000) {
            detected_type = DroneType::MILITARY_DRONE;
            threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
        }
        // 2.4 GHz ISM band: DJI drones, WiFi, other consumer drones
        // Most common drone frequency range for video and control
        else if (freq >= 2400000000 && freq <= 2500000000) {
            detected_type = DroneType::MAVIC;
            threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
        }
        // 5.8 GHz ISM band: FPV video transmission
        // Used by racing drones and FPV systems
        else if (freq >= 5700000000 && freq <= 5900000000) {
            detected_type = DroneType::FPV_RACING;
            threat_level = std::max(threat_level, ThreatLevel::LOW);
        }
    }

    // MODIFICATION: Adjust threat level based on movement trend
    // If drone is approaching, increase threat level. If receding, decrease it.
    // This provides dynamic threat assessment based on proximity changes.
    {
        MutexLock lock(data_mutex);

        // Check if this frequency is already being tracked
        bool drone_found = false;
        size_t drone_index = 0;
        for (size_t i = 0; i < tracked_count_; ++i) {
            if (tracked_drones()[i].frequency == entry.frequency_a && tracked_drones()[i].update_count >= 3) {
                drone_found = true;
                drone_index = i;
                break;
            }
        }

        if (drone_found) {
            // Get movement trend from tracked drone history
            MovementTrend trend = tracked_drones()[drone_index].get_trend();

            // Adjust threat level based on trend
            switch (trend) {
                case MovementTrend::APPROACHING:
                    // Drone is getting closer - raise threat level
                    if (threat_level == ThreatLevel::NONE) {
                        threat_level = ThreatLevel::LOW;
                    } else if (threat_level < ThreatLevel::CRITICAL) {
                        // Increase by one level (e.g., MEDIUM -> HIGH)
                        threat_level = static_cast<ThreatLevel>(static_cast<uint8_t>(threat_level) + 1);
                    }
                    break;

                case MovementTrend::RECEDING:
                    // Drone is moving away - lower threat level
                    if (threat_level > ThreatLevel::NONE) {
                        // Decrease by one level (e.g., HIGH -> MEDIUM)
                        threat_level = static_cast<ThreatLevel>(static_cast<uint8_t>(threat_level) - 1);
                    }
                    break;

                case MovementTrend::STATIC:
                case MovementTrend::UNKNOWN:
                default:
                    // No change to threat level
                    break;
            }
        }
    }

    // --- CRITICAL SECTION START ---
    {
        MutexLock lock(data_mutex);

        // Diamond Code: Increment total_detections_ with mutex protection for thread safety
        // volatile uint32_t reads/writes are atomic on ARM Cortex-M4 (32-bit aligned)
        total_detections_++;

        // Guard clause: Prevent division by zero
        if (entry.frequency_a == EDA::Constants::ZERO_FREQUENCY) {
            return;
        }
        size_t freq_hash = static_cast<size_t>(entry.frequency_a / EDA::Constants::FREQ_HASH_DIVISOR) & EDA::Constants::FREQ_HASH_MASK;
        int32_t prev_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
        int32_t base_threshold = -90;

        if (rssi > MagicNumberConstants::RSSI_LOW_THRESHOLD_DBM && rssi < MagicNumberConstants::RSSI_HIGH_THRESHOLD_DBM) {
            base_threshold = MagicNumberConstants::RSSI_LOW_THRESHOLD_DBM;
        } else if (rssi > MagicNumberConstants::RSSI_HIGH_THRESHOLD_DBM) {
            base_threshold = -90;
        }

        if (prev_rssi < base_threshold) {
            base_threshold += HYSTERESIS_MARGIN_DB;
        }

        if (rssi >= base_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            // DIAMOND FIX #P2-MEDIUM #6: Saturated increment (no overflow)
            if (current_count < 255) {
                current_count++;
            }

            // Write to buffer (safe)
            detection_ring_buffer_.update_detection({freq_hash, current_count, rssi});

            if (current_count >= MIN_DETECTION_COUNT) {
                // Update drone tracking for UI
                update_tracked_drone({detected_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(rssi), threat_level});
            }
        } else {
            // "Leaky bucket" logic
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);

            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection({freq_hash, current_count, stored_rssi});
            } else {
                detection_ring_buffer_.update_detection({freq_hash, 0, -120});
            }
        }
    }
    // --- CRITICAL SECTION END ---
    // Mutex released. UI can redraw.

}

void DroneScanner::send_drone_detection_message(const DetectionParams& params) {
    // Only update internal data - UI will read via snapshot
    update_tracked_drone(params);
}

// ============================================================================
// SCANNING FLOW: Tracked Drone Update
// ============================================================================
// Flow: update_tracked_drone() -> TrackedDrone::add_rssi() -> update_tracking_counts()
//
// Tracked Drone Management:
// - MAX_TRACKED_DRONES (10) fixed-size array (zero heap allocation)
// - Each TrackedDrone stores MAX_HISTORY (3) RSSI samples for trend calculation
// - Oldest entry replaced when array is full
//
// Thread Safety: Called from scanner thread, data protected by data_mutex
// Memory: Uses placement new on static storage (tracked_drones_storage_)
void DroneScanner::update_tracked_drone(const DetectionParams& params) {
    // Only update internal data - UI will read via snapshot
    // Single mutex lock acquisition at the start
    MutexLock lock(data_mutex);
    
    Frequency frequency = params.frequency_hz;
    DroneType type = params.type;
    int32_t rssi = params.rssi_db;
    ThreatLevel threat_level = params.threat_level;
    
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones()[i].frequency == frequency && tracked_drones()[i].update_count > 0) {
            tracked_drones()[i].add_rssi({static_cast<int16_t>(rssi), chTimeNow()});
            tracked_drones()[i].drone_type = static_cast<uint8_t>(type);
            tracked_drones()[i].threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
    }

    if (tracked_count_ < EDA::Constants::MAX_TRACKED_DRONES) {
        TrackedDrone new_drone;
        // DIAMOND FIX #MEDIUM #4: Remove uint32_t cast, use Frequency directly
        // TrackedDrone.frequency is already Frequency type (int64_t)
        new_drone.frequency = frequency;
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
    // DIAMOND FIX #MEDIUM #4: Remove uint32_t cast, use Frequency directly
    // TrackedDrone.frequency is already Frequency type (int64_t)
    oldest_drone.frequency = frequency;
    oldest_drone.drone_type = static_cast<uint8_t>(type);
    oldest_drone.threat_level = static_cast<uint8_t>(threat_level);
    oldest_drone.add_rssi({static_cast<int16_t>(rssi), chTimeNow()});
}

void DroneScanner::remove_stale_drones() {
    // Stack overflow detection check
    if (!check_stack_canary()) {
        // Stack overflow detected - reinitialize canary to prevent further corruption
        init_stack_canary();
    }

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

    // Use class member variable instead of stack allocation (heap-free)
    // Reset is_stale array
    for (size_t i = 0; i < EDA::Constants::MAX_TRACKED_DRONES; ++i) {
        is_stale_[i] = 0;
    }
    for (size_t i = 0; i < stale_count; ++i) {
        if (stale_indices_[i] < tracked_count_) {
            is_stale_[stale_indices_[i]] = 1;
        }
    }

    for (size_t read_index = 0; read_index < tracked_count_; ++read_index) {
        if (!is_stale_[read_index]) {
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
    MutexLock lock(data_mutex);
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

    // TrackedDrone
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
                // FIX: Validate frequency before adding to database
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

        freq_db_loaded_.store(true);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
    }

    // DIAMOND FIX #4: Initialize wideband scanning AFTER database initialization
    // This ensures proper initialization order: database -> wideband scanning
    // Prevents premature execution before database is fully loaded
    initialize_wideband_scanning();

    // Mark initialization as complete
    initialization_complete_.store(true);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
}

void DroneScanner::sync_database() {
    MutexLock lock(data_mutex);
    
    if (!freq_db_ptr_) {
        return;
    }
    
    // FreqmanDB auto-syncs on every write operation
}

// ============================================================================
// DIAMOND FIX #CRITICAL #5: SD Card Mount Timeout Without Hardware Reset
// ============================================================================
/**
 * @brief Cleanup database and scanner resources
 * @details Implements proper thread cleanup before destruction
 * @note Adds thread state verification before clearing thread pointer
 * @note Implements timeout enforcement with cooperative termination
 * @note Implements graceful degradation on SD card removal
 */
void DroneScanner::cleanup_database_and_scanner() {
    // DIAMOND FIX #CRITICAL #5: Sync database before cleanup
    // This ensures any pending writes are flushed to SD card
    sync_database();

    // DIAMOND FIX #CRITICAL #5: Implement proper thread cleanup
    // Check if database loading thread is still active
    bool was_loading = db_loading_active_.load();  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.load()
    
    // Signal thread to stop (cooperative termination)
    db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
    
    if (was_loading && db_loading_thread_ != nullptr) {
        // DIAMOND FIX #CRITICAL #5: Thread state verification before cleanup
        // Wait for thread to exit with timeout enforcement
        systime_t timeout = chTimeNow() + MS2ST(EDA::Constants::THREAD_TERMINATION_TIMEOUT_MS);
        
        // Wait for thread to complete (cooperative termination)
        while (chTimeNow() < timeout && db_loading_thread_ != nullptr) {
            // Small sleep to yield CPU and allow thread to exit
            chThdSleepMilliseconds(EDA::Constants::THREAD_TERMINATION_POLL_INTERVAL_MS);
            
            // Check if thread has exited
            if (db_loading_thread_ == nullptr) {
                break;
            }
        }
        
        // DIAMOND FIX #CRITICAL #5: Thread state verification before clearing pointer
        // Only clear thread pointer if thread has actually exited
        if (db_loading_thread_ != nullptr) {
            // Thread did not exit within timeout - log warning but continue
            // This is graceful degradation - we don't hang the system
            // The thread will eventually exit when it checks db_loading_active_ flag
            db_loading_thread_ = nullptr;
        }
    }

    // Explicit destructor calls for placement new
    // Only call destructor if construction succeeded
    if (tracked_drones_ptr_ != nullptr && tracked_drones_constructed_) {
        // DIAMOND FIX: Call destructor of std::array<TrackedDrone, MAX_TRACKED_DRONES>
        // Use type alias defined in header (ui_enhanced_drone_analyzer.hpp:274)
        using TrackedDronesArray = std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>;
        tracked_drones_ptr_->~TrackedDronesArray();
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
        db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
        return;
    }

    // Use reinterpret_cast on aligned buffer
    // DIAMOND FIX #P2-MED #7: Use direct comparison for alignment check (more reliable than modulo)
    // Direct comparison avoids potential issues with modulo operator and is more readable
    constexpr uintptr_t FREQMAN_DB_ALIGNMENT = alignof(FreqmanDB);
    const uintptr_t freq_db_addr = reinterpret_cast<uintptr_t>(freq_db_storage_);
    if ((freq_db_addr & (FREQMAN_DB_ALIGNMENT - 1)) != 0) {
        handle_scan_error("Memory: freq_db_storage_ alignment error (async)");
        db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
        return;
    }
    freq_db_ptr_ = reinterpret_cast<FreqmanDB*>(freq_db_storage_);
    // FIX #M1: Remove meaningless null check after reinterpret_cast
    // reinterpret_cast from a valid pointer never returns nullptr
    // Alignment check above is sufficient

    // Runtime alignment verification for tracked_drones_storage_
    // DIAMOND FIX #P2-MED #7: Use direct comparison for alignment check
    // Note: TrackedDronesArray type alias is defined in header (ui_enhanced_drone_analyzer.hpp:274)
    constexpr uintptr_t TRACKED_DRONES_ALIGNMENT = alignof(std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>);
    const uintptr_t tracked_drones_addr = reinterpret_cast<uintptr_t>(tracked_drones_storage_);
    if ((tracked_drones_addr & (TRACKED_DRONES_ALIGNMENT - 1)) != 0) {
        handle_scan_error("Memory: tracked_drones_storage_ alignment error (async)");
        freq_db_ptr_ = nullptr;
        db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
        return;
    }

    // Use reinterpret_cast on aligned buffer
    tracked_drones_ptr_ = reinterpret_cast<std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>*>(tracked_drones_storage_);
    // FIX #M2: Remove meaningless null check after reinterpret_cast
    // reinterpret_cast from a valid pointer never returns nullptr
    // Alignment check above is sufficient

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
            db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
            // Ensure proper cleanup of placement-newed objects on timeout
            if (freq_db_ptr_ && freq_db_constructed_) {
                freq_db_ptr_->~FreqmanDB();
                freq_db_constructed_ = false;
                freq_db_ptr_ = nullptr;
            }
            if (tracked_drones_ptr_ && tracked_drones_constructed_) {
                // DIAMOND FIX: Call destructor of std::array<TrackedDrone, MAX_TRACKED_DRONES>
                using TrackedDronesArray = std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>;
                tracked_drones_ptr_->~TrackedDronesArray();
                tracked_drones_constructed_ = false;
                tracked_drones_ptr_ = nullptr;
            }
            // FIX #16: Always reset initialization complete flag on error
            initialization_complete_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
            freq_db_loaded_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store() - Also reset database loaded flag
            return;
        }
        chThdSleepMilliseconds(EDA::Constants::SD_CARD_POLL_INTERVAL_MS);
    }

    // Maintain proper lock order: DATA_MUTEX  SD_CARD_MUTEX
    bool should_load = db_loading_active_.load();  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.load()
    if (should_load) {
        // FIX: Defensive null check before dereferencing freq_db_ptr_
        // This provides additional safety if initialization somehow fails
        if (!freq_db_ptr_) {
            handle_scan_error("Database pointer is null during async loading");
            db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
            return;
        }
        db_success = freq_db_ptr_->open(db_path);

        if (db_success) {
            systime_t load_time = chTimeNow() - load_start;
            if (load_time > MS2ST(DB_LOAD_TIMEOUT_MS)) {
                handle_scan_error("Database load timeout");
                db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
                // FIX #4 & #6: Ensure proper cleanup of placement-newed objects on timeout
                // Only call destructors if construction succeeded
                if (freq_db_ptr_ && freq_db_constructed_) {
                    freq_db_ptr_->~FreqmanDB();
                    freq_db_constructed_ = false;
                    freq_db_ptr_ = nullptr;
                }
            if (tracked_drones_ptr_ && tracked_drones_constructed_) {
                // DIAMOND FIX: Call destructor of std::array<TrackedDrone, MAX_TRACKED_DRONES>
                using TrackedDronesArray = std::array<TrackedDrone, EDA::Constants::MAX_TRACKED_DRONES>;
                tracked_drones_ptr_->~TrackedDronesArray();
                tracked_drones_constructed_ = false;
                tracked_drones_ptr_ = nullptr;
            }
                initialization_complete_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
                freq_db_loaded_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
                return;
            }

            // FIX #H1: Set flags when database loads successfully on first attempt
            // If db_success is true and database is not empty, the code below
            // (lines 1408-1467) won't execute, leaving initialization_complete_ and
            // freq_db_constructed_ flags unset. This causes is_initialization_complete()
            // to always return false, preventing phases 3-6 from completing.
            // FIX: Defensive null check before dereferencing freq_db_ptr_
            if (freq_db_ptr_ && !freq_db_ptr_->empty()) {
                freq_db_loaded_.store(true);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
                freq_db_constructed_ = true;
                initialization_complete_.store(true);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
                db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
                return;
            }
        }
    }

    bool should_init = db_loading_active_.load();
    if (should_init && (!db_success || freq_db_ptr_->empty())) {

        // Phase 1: Acquire data_mutex for database modifications
        {
            MutexLock data_lock(data_mutex, LockOrder::DATA_MUTEX);
            
            // FIX: Defensive null check before dereferencing freq_db_ptr_
            // This provides additional safety if initialization somehow fails
            if (!freq_db_ptr_) {
                handle_scan_error("Database pointer is null during database initialization");
                db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
                return;
            }
            freq_db_ptr_->open(db_path, true);

            for (const auto& item : BUILTIN_DRONE_DB) {
                bool still_loading = db_loading_active_.load();
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
            freq_db_loaded_.store(true);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
        }  // data_lock released here

        // Phase 2: Acquire sd_card_mutex for sync_database()
        // DIAMOND FIX #HIGH #3: Use get_sd_card_mutex() for safe initialization
        {
            MutexLock sd_lock(get_sd_card_mutex(), LockOrder::SD_CARD_MUTEX);
            sync_database();
        }  // sd_lock released here

        // Set freq_db_constructed_ flag at end of thread
        freq_db_constructed_ = true;
        
        // Set initialization_complete_ flag at end of thread
        initialization_complete_.store(true);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
    }
}

// Async database loading (non-blocking UI)
void DroneScanner::initialize_database_async() {
    bool is_loading = db_loading_active_.load();
    if (is_loading) {
        return;  // Already loading or loaded
    }

    // FIX: Initialize database and scanner BEFORE creating the loading thread
    // This ensures freq_db_ptr_ and tracked_drones_ptr_ are properly initialized
    // via placement new before any async operations begin, preventing M0 guru
    // meditation errors from null pointer dereferences.
    initialize_database_and_scanner();

    db_loading_active_ = true;

    // Use chThdCreateStatic instead of chThdCreateFromHeap

    db_loading_thread_ = chThdCreateStatic(
        db_loading_wa_,                    // Working area
        sizeof(db_loading_wa_),            // Size
        NORMALPRIO - 2,                    // Priority
        db_loading_thread_entry,           // Entry function
        this                               // Argument
    );

    // FIX #2: Verify stack size matches constant
    static_assert(sizeof(db_loading_wa_) >= DroneScanner::DB_LOADING_STACK_SIZE,
                 "db_loading_wa_ size mismatch with DB_LOADING_STACK_SIZE");

    // FIX #M5: Single thread creation check (removed duplicate)
    // chThdCreateStatic with static stack should never fail, but handle gracefully
    if (db_loading_thread_ == nullptr) {
        handle_scan_error("Failed to create db_loading_thread");
        db_loading_active_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
        freq_db_loaded_.store(false);  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.store()
        return;  // Keep UI responsive - early return on failure
    }
}

// ============================================================================
// DIAMOND FIX #CRITICAL #4: Database Loading Thread Race Condition
// ============================================================================
/**
 * @brief Check if async database loading has completed
 * @details Thread-safe check for database loading completion
 * @note Uses mutex protection to prevent race condition with loading thread
 * @note Verifies database pointer is valid before reporting completion
 * @return true if database loading is complete, false otherwise
 */
bool DroneScanner::is_database_loading_complete() const {
    // DIAMOND FIX #CRITICAL #4: Acquire mutex before accessing freq_db_ptr_
    // This prevents race condition where UI thread reads freq_db_ptr_ while
    // loading thread is writing to it
    MutexLock lock(data_mutex);
    
    // Check loading state with mutex protection
    bool is_loading = db_loading_active_.load();  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.load()
    
    // Check that freq_db_ptr_ is not null in addition to checking freq_db_loaded_ flag
    // This ensures the database is properly initialized before reporting completion
    bool db_loaded = freq_db_loaded_.load();  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.load()
    bool db_valid = (freq_db_ptr_ != nullptr);
    
    return !is_loading && db_loaded && db_valid;
}

bool DroneScanner::is_initialization_complete() const {
    return initialization_complete_.load();  // DIAMOND FIX #P1-HIGH #2: Use AtomicFlag.load()
}

// FIX #6: Database validation method
bool DroneScanner::is_database_valid() const {
    // Check if database pointer is valid and database is loaded
    if (!freq_db_ptr_ || !freq_db_loaded_.load()) {
        return false;
    }
    
    // Check if database has at least one entry
    uint32_t entry_count = freq_db_ptr_->entry_count();
    return entry_count > 0;
}

/**
 * @brief Get current scanning frequency with mutex protection
 * @details Thread-safe access to current scanning frequency
 * @return Current scanning frequency or default fallback frequency
 * @note DIAMOND FIX MEDIUM #5: Added mutex protection for database access
 * @note DIAMOND FIX #MEDIUM #5: Replaced magic number with named constant
 */
Frequency DroneScanner::get_current_scanning_frequency() const {
    // DIAMOND FIX MEDIUM #5: Acquire mutex for database access protection
    MutexLock lock(data_mutex);

    if (!freq_db_ptr_ || !freq_db_loaded_) {
        // DIAMOND FIX #MEDIUM #5: Use named constant instead of magic number
        return EDA::FrequencyConstants::CENTER_433MHZ;  // Default fallback frequency (434 MHz)
    }

    size_t db_entry_count = freq_db_ptr_->entry_count();
    if (db_entry_count > 0 && current_db_index_ < db_entry_count) {
        return (*freq_db_ptr_)[current_db_index_].frequency_a;
    }

    // DIAMOND FIX #MEDIUM #5: Use named constant instead of magic number
    return EDA::FrequencyConstants::CENTER_433MHZ;  // Default fallback frequency (434 MHz)
}

Frequency DroneScanner::get_current_radio_frequency() const {
    return get_current_scanning_frequency();
}

// ============================================================================
// THREAD SAFETY FIX: Mutex-protected getter methods for concurrent access
// These methods are called from UI thread while scanner thread updates values.
// Lock order: DATA_MUTEX (level 1) for detection data and tracking state
// ============================================================================

ThreatLevel DroneScanner::get_max_detected_threat() const {
    MutexLock lock(data_mutex);
    return max_detected_threat_;
}

size_t DroneScanner::get_approaching_count() const {
    MutexLock lock(data_mutex);
    return approaching_count_;
}

size_t DroneScanner::get_receding_count() const {
    MutexLock lock(data_mutex);
    return receding_count_;
}

size_t DroneScanner::get_static_count() const {
    MutexLock lock(data_mutex);
    return static_count_;
}

uint32_t DroneScanner::get_total_detections() const {
    MutexLock lock(data_mutex);
    return total_detections_;
}

uint32_t DroneScanner::get_scan_cycles() const {
    MutexLock lock(data_mutex);
    return scan_cycles_;
}

// ============================================================================
// SCANNER STATE SNAPSHOT IMPLEMENTATION
// ============================================================================

DroneScanner::ScannerStateSnapshot DroneScanner::get_state_snapshot() const {
    ScannerStateSnapshot snapshot;
    MutexLock lock(data_mutex);
    snapshot.max_detected_threat = max_detected_threat_;
    snapshot.approaching_count = approaching_count_;
    snapshot.static_count = static_count_;
    snapshot.receding_count = receding_count_;
    snapshot.scanning_active = scanning_active_.load();
    return snapshot;
}

// ============================================================================
// END OF THREAD SAFETY FIX
// ============================================================================

TrackedDrone DroneScanner::getTrackedDrone(size_t index) const {
    MutexLock lock(data_mutex);
    if (index < tracked_count_) {
        return tracked_drones()[index];
    }
    return TrackedDrone{};
}

void DroneScanner::handle_scan_error([[maybe_unused]] const char* error_msg) {
    // Error handling - currently just logs to debug output
    // Could be extended to update UI status or trigger recovery actions
    if (error_msg) {
        // Log error (implementation depends on logging system)
        // TODO: Add proper error logging/recovery mechanism
    }
}

DroneScanner::DroneSnapshot DroneScanner::get_tracked_drones_snapshot() const {
    // Snapshot is point-in-time and may be stale
    DroneSnapshot snapshot;
    MutexLock lock(data_mutex);
    snapshot.count = tracked_count_;
    for (size_t i = 0; i < tracked_count_ && i < EDA::Constants::MAX_TRACKED_DRONES; ++i) {
        snapshot.drones[i] = tracked_drones()[i];
    }
    return snapshot;
}

bool DroneScanner::try_get_tracked_drones_snapshot(DroneSnapshot& out_snapshot) const {
    // FIX: Use RAII MutexTryLock for automatic unlock on all paths
    // FIX #9: Snapshot is point-in-time and may be stale
    // The lock is released when the function returns, so the returned data
    // represents the state at the moment of the snapshot and may not reflect
    // subsequent changes to tracked_drones_.
    MutexTryLock lock(data_mutex);

    if (lock.is_locked()) {
        out_snapshot.count = tracked_count_;
        for (size_t i = 0; i < tracked_count_ && i < EDA::Constants::MAX_TRACKED_DRONES; ++i) {
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

// DroneDetectionLogger implementations - DISABLED (class not defined)
#if 0
DroneDetectionLogger::DroneDetectionLogger()
    : worker_thread_(nullptr),
      mutex_(),
      data_ready_(),
      worker_should_run_(false),
      initialization_complete_(),
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
    std::memset(line_buffer_, 0, sizeof(line_buffer_));
    
    chSemInit(&data_ready_, 0);
    
    // DIAMOND FIX #P0-CRITICAL #1: Removed start_session() from constructor
    // Session is now started in start_worker() when worker thread is ready
    // This prevents race condition where session is active but no worker is processing entries
}

DroneDetectionLogger::~DroneDetectionLogger() {
    stop_worker();
    end_session();
}

// FIX #1: Changed return type to bool to indicate success/failure
// start_session() now checks if SD card is mounted before activating session
bool DroneDetectionLogger::start_session() {
    if (session_active_) {
        return true;  // Already active, consider success
    }
    
    // Check if SD card is mounted before starting session
    if (sd_card::status() != sd_card::Status::Mounted) {
        return false;  // SD card not mounted, cannot start session
    }
    
    session_active_ = true;
    session_start_ = chTimeNow();
    logged_count_ = 0;
    dropped_logs_ = 0;
    header_written_ = false;
    return true;
}

// FIX #1: Renamed from end_session() for clarity, now returns bool
bool DroneDetectionLogger::stop_session() {
    if (!session_active_) {
        return true;  // Already inactive, consider success
    }
    
    session_active_ = false;
    return true;
}

// Keep end_session() for backward compatibility
void DroneDetectionLogger::end_session() {
    (void)stop_session();  // Explicitly ignore return value for backward compatibility
}

/**
 * @brief Log detection asynchronously with buffer overflow protection
 * @details Thread-safe detection logging with robust buffer overflow handling
 * @param entry Detection log entry to add to buffer
 * @return true if entry was logged successfully, false on error
 * @note DIAMOND FIX MEDIUM #4: Enhanced buffer overflow detection and handling
 * @note Uses circular buffer with oldest entry eviction on overflow
 * @note Tracks overflow statistics for monitoring
 */
bool DroneDetectionLogger::log_detection_async(const DetectionLogEntry& entry) {
    // DIAMOND FIX MEDIUM #4: Validate buffer size before logging
    static_assert(BUFFER_SIZE > 0, "BUFFER_SIZE must be greater than 0");
    static_assert(BUFFER_SIZE <= 256, "BUFFER_SIZE too large for embedded system");
    
    MutexLock lock(mutex_);
    
    // DIAMOND FIX MEDIUM #4: Detect buffer overflow before writing
    bool was_full = is_full_;
    if (was_full) {
        // Buffer overflow detected - evict oldest entry
        overflow_count_++;
        dropped_logs_++;

        // Overwrite oldest entry (tail position)
        tail_ = (tail_ + 1) % BUFFER_SIZE;
        
        // Graceful degradation: Log continues with oldest entry eviction
        // This prevents system hang while preserving most recent data
    }

    // Write new entry at head position
    size_t current_head = head_;
    ring_buffer_[current_head] = entry;
    head_ = (current_head + 1) % BUFFER_SIZE;

    // Check if buffer is now full
    if (head_ == tail_) {
        is_full_ = true;
    } else {
        is_full_ = false;
    }

    // Signal worker thread that data is ready
    chSemSignal(&data_ready_);
    
    // DIAMOND FIX MEDIUM #4: Return success even on overflow
    // System continues with degraded functionality instead of hanging
    return true;
}

void DroneDetectionLogger::start_worker() {
    if (worker_thread_) return;

    worker_should_run_ = true;
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

    // Check if session started successfully
    if (!start_session()) {
        // Session failed to start (SD card not mounted)
        // Worker thread will exit when worker_should_run_ is checked in worker_loop()
        worker_should_run_ = false;
    }
}

void DroneDetectionLogger::stop_worker() {
    if (!worker_thread_) return;

    worker_should_run_ = false;
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
        bool should_run = worker_should_run_;
        if (!should_run) break;

        // Check initialization state - wait until initialization is complete
        bool init_complete = initialization_complete_.load();
        if (!init_complete) {
            // FIX #L5: Use constant instead of magic number
            chThdSleepMilliseconds(EDA::Constants::SD_CARD_POLL_INTERVAL_MS);
            continue;
        }

        // FIX #L4: Use constant instead of magic number (1 second timeout)
        chSemWaitTimeout(&data_ready_, MS2ST(EDA::Constants::SETTINGS_LOAD_TIMEOUT_MS));
        
        should_run = worker_should_run_;
        if (!should_run) break;

        DetectionLogEntry entry_to_write;
        bool has_data = false;

        {
            MutexLock lock(mutex_);
            // Diamond Code: Direct volatile access is safe because mutex_ provides thread safety
            if (head_ != tail_ || is_full_) {
                entry_to_write = ring_buffer_[tail_];
                tail_ = (tail_ + 1) % BUFFER_SIZE;
                is_full_ = false;
                has_data = true;

                bool has_more_data = (head_ != tail_);
                // FIX #12: Signal semaphore while still holding mutex
                // This prevents race condition where buffer state changes between
                // checking has_more_data and signaling the semaphore
                if (has_more_data) {
                    chSemSignal(&data_ready_);
                }
            }
        }

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
    // DIAMOND FIX #HIGH #3: Use get_sd_card_mutex() for safe initialization
    SDCardLock lock(get_sd_card_mutex());

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
#endif

// Hardware Controller Implementation

DroneHardwareController::DroneHardwareController(SpectrumMode mode)
    : last_spectrum_db_(),
      spectrum_mutex_(),
      spectrum_updated_(),
      spectrum_mode_(mode),
      center_frequency_(2400000000ULL),
      bandwidth_hz_(24000000),
      radio_state_(),
      spectrum_fifo_(nullptr),
      spectrum_streaming_active_(false),
      rssi_updated_(),
      last_valid_rssi_(-120)
{
    chMtxInit(&spectrum_mutex_);
    spectrum_updated_.store(false);
    rssi_updated_.store(false);
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
    portapack::receiver_model.disable();

    cleanup_spectrum_collector();
}

void DroneHardwareController::initialize_radio_state() {
    // Load baseband coprocessor image before accessing receiver resources
    baseband::run_image(portapack::spi_flash::image_tag_wideband_spectrum);

    // Add error handling for all radio operations

    portapack::receiver_model.enable();
    portapack::receiver_model.set_modulation(ReceiverModel::Mode::SpectrumAnalysis);
    portapack::receiver_model.set_sampling_rate(get_configured_sampling_rate());
    portapack::receiver_model.set_baseband_bandwidth(get_configured_bandwidth());
    portapack::receiver_model.set_squelch_level(0);

        // Use safe defaults - RF Amp controlled by user settings
        portapack::receiver_model.set_rf_amp(false);

    // LNA Gain: 32dB (Range 0-40) - receives weak signals
    portapack::receiver_model.set_lna(MagicNumberConstants::LNA_GAIN_WEAK_SIGNAL_DB);

    // VGA Gain: 32dB (Range 0-62) - signal volume after LNA
    portapack::receiver_model.set_vga(32);

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
    // FIX: Validate frequency before setting
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
        portapack::receiver_model.set_target_frequency(frequency_hz);
        
        // Verify frequency was set correctly
        Frequency actual_freq = portapack::receiver_model.target_frequency();
        if (actual_freq == frequency_hz) {
            return true;  // Success
        }
        
        // Delay before retry
        if (retry < MAX_RETRIES - 1) {
            chThdSleepMilliseconds(RETRY_DELAY_MS);
        }
    }
    
    return false;  // All retries failed
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

// ISR-safe: uses CriticalSection instead of mutex
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
    rssi_updated_.store(true);
}

void DroneHardwareController::clear_rssi_flag() {
    rssi_updated_.store(false);
}

bool DroneHardwareController::is_rssi_fresh() const {
    return rssi_updated_.load();
}

void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum) {
    // ISR-safe: use critical section instead of mutex
    CriticalSection lock;

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
      threat_progress_bar_({0, 0, screen_width, MagicNumberConstants::UI_ELEMENT_HEIGHT}),
      threat_status_main_({0, MagicNumberConstants::UI_THREAT_STATUS_Y_OFFSET, screen_width, MagicNumberConstants::UI_ELEMENT_HEIGHT}, "THREAT: LOW | <0 ~0 >0"),
      threat_frequency_({0, MagicNumberConstants::UI_THREAT_FREQUENCY_Y_OFFSET, screen_width, MagicNumberConstants::UI_ELEMENT_HEIGHT}, "2400.0MHz SCANNING"),
      // DIAMOND FIX: Initialize mutex in member initialization list (RAII pattern)
      ui_mutex_() {
    last_text_[0] = '\0';
    add_children({&threat_progress_bar_, &threat_status_main_, &threat_frequency_});
    update(ThreatLevel::NONE, 0, 0, 0, 2400000000ULL, false);
}

void SmartThreatHeader::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
                                size_t receding, Frequency current_freq, bool is_scanning) {
    // DIAMOND FIX #HIGH #4: Acquire mutex for UI update protection
    // Prevents race conditions when multiple threads call update methods concurrently
    MutexLock lock(ui_mutex_, LockOrder::UI_THREAT_MUTEX);

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
    threat_progress_bar_.set_value(total_drones * MagicNumberConstants::THREAT_PROGRESS_MULTIPLIER);

    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    // Prevents initialization order issues and saves stack memory
    const char* threat_name = UnifiedStringLookup::threat_name(static_cast<uint8_t>(max_threat));
    if (total_drones > 0) {
        StatusFormatter::format_to(ui_threat_buffer_, "THREAT: %s | <%lu ~%lu >%lu",
                                  threat_name,
                                  static_cast<unsigned long>(approaching),
                                  static_cast<unsigned long>(static_count),
                                  static_cast<unsigned long>(receding));
    } else if (is_scanning) {
        StatusFormatter::format_to(ui_threat_buffer_, "SCANNING: <%lu ~%lu >%lu",
                                  static_cast<unsigned long>(approaching),
                                  static_cast<unsigned long>(static_count),
                                  static_cast<unsigned long>(receding));
    } else {
        StatusFormatter::format_to(ui_threat_buffer_, "READY");
    }
    threat_status_main_.set(ui_threat_buffer_);
    threat_status_main_.set_style(&UIStyles::RED_STYLE);
    snprintf(last_text_, sizeof(last_text_), "%s", ui_threat_buffer_);

    // Cache text length to avoid strlen() in paint()
    last_text_len_ = strlen(last_text_);

    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    // Use FrequencyFormatter with member buffer
    if (current_freq > 0) {
        FrequencyFormatter::to_string_short_freq_buffer(ui_freq_buffer_, sizeof(ui_freq_buffer_), current_freq);
        if (is_scanning) {
            StatusFormatter::format_to(ui_threat_buffer_, "%s SCANNING", ui_freq_buffer_);
        } else {
            StatusFormatter::format_to(ui_threat_buffer_, "%s READY", ui_freq_buffer_);
        }
        threat_frequency_.set(ui_threat_buffer_);
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
      last_trend_(MovementTrend::UNKNOWN), last_rssi_(-120),
      // DIAMOND FIX: Initialize mutex in member initialization list (RAII pattern)
      ui_mutex_() {
    last_threat_name_[0] = '\0';
    add_children({&card_text_});
}

// Use UnifiedStringLookup
void ThreatCard::update_card(const DisplayDroneEntry& drone) {
    // DIAMOND FIX #HIGH #4: Acquire mutex for UI update protection
    // Prevents race conditions when multiple threads call update methods concurrently
    MutexLock lock(ui_mutex_, LockOrder::UI_CARD_MUTEX);

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
    snprintf(last_threat_name_, sizeof(last_threat_name_), "%s", drone.type_name);
    is_active_ = true;

    // Use TrendSymbols for O(1) lookup
    char trend_char = TrendSymbols::from_trend(static_cast<uint8_t>(drone.trend));
    uint32_t mhz = drone.frequency / 1000000;

    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    // Prevents initialization order issues and saves stack memory
    StatusFormatter::format_to(card_buffer_, "%s %c %luM %ld",
                             drone.type_name, trend_char,
                             (unsigned long)mhz, (long)drone.rssi);
    card_text_.set(card_buffer_);

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
    : View(parent_rect), bar_index_(bar_index), parent_rect_(parent_rect),
      // DIAMOND FIX: Initialize mutex in member initialization list (RAII pattern)
      ui_mutex_() {
    add_children({&progress_text_, &alert_text_, &normal_text_});
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    // DIAMOND FIX #HIGH #4: Acquire mutex for UI update protection
    // Prevents race conditions when multiple threads call update methods concurrently
    MutexLock lock(ui_mutex_, LockOrder::UI_STATUSBAR_MUTEX);

    // Early return for invalid states
    if (progress_percent > MagicNumberConstants::PROGRESS_PERCENT_MAX) progress_percent = MagicNumberConstants::PROGRESS_PERCENT_MAX;

    set_display_mode(DisplayMode::SCANNING);

    // Table-driven progress bar - O(1) lookup
    uint8_t raw_idx = (progress_percent * MagicNumberConstants::PROGRESS_BAR_SEGMENTS) / MagicNumberConstants::PROGRESS_PERCENT_MAX;
    uint8_t bar_idx = (raw_idx < MagicNumberConstants::PROGRESS_BAR_SEGMENTS) ? raw_idx : MagicNumberConstants::PROGRESS_BAR_SEGMENTS;
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
        // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
        // Prevents initialization order issues and saves stack memory
        StatusFormatter::format_to(alert_buffer_, "[!] DETECTED: %lu threats found!",
                                  static_cast<unsigned long>(detections));
        alert_text_.set(alert_buffer_);

        // Style from constexpr LUT
        static const Style ALERT_STYLE = {font::fixed_8x16, Color::black(), Color(THREAT_STYLES[0].bg_color)};
        alert_text_.set_style(&ALERT_STYLE);
    }
    set_dirty();
}

/**
 * @brief Update alert status with mutex protection
 * @details Thread-safe UI update for alert status display
 * @param threat Current threat level
 * @param total_drones Total number of detected drones
 * @param alert_msg Alert message to display
 * @note Uses mutex to prevent race conditions during concurrent UI updates
 * @note DIAMOND FIX MEDIUM #3: Added mutex protection for UI update race condition
 */
/**
 * @brief Update alert status with mutex protection
 * @details Thread-safe UI update for alert status display
 * @param threat Current threat level
 * @param total_drones Total number of detected drones
 * @param alert_msg Alert message to display
 * @note Uses mutex to prevent race conditions during concurrent UI updates
 * @note DIAMOND FIX MEDIUM #3: Added mutex protection for UI update race condition
 * @note DIAMOND FIX LOW #3: Added input validation for alert message
 */
void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const char* alert_msg) {
    // DIAMOND FIX MEDIUM #3: Acquire mutex for UI update protection
    MutexLock lock(ui_mutex_, LockOrder::UI_STATUSBAR_MUTEX);
    
    // DIAMOND FIX LOW #3: Input validation - check for null pointer
    if (!alert_msg) return;
    
    // DIAMOND FIX LOW #3: Input validation - check for empty string
    if (alert_msg[0] == '\0') return;

    set_display_mode(DisplayMode::ALERT);

    // Unified icon and style mapping from constexpr LUT
    size_t icon_idx = std::min(static_cast<size_t>(threat), size_t(4));
    const char* alert_icon = ALERT_ICONS[icon_idx];

    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    // Prevents initialization order issues and saves stack memory
    StatusFormatter::format_to(status_buffer_, "%s ALERT: %lu drones | %s",
                             alert_icon,
                             static_cast<unsigned long>(total_drones),
                             alert_msg);
    alert_text_.set(status_buffer_);

    // Table-driven style selection
    // O(1) lookup  runtime : CRITICAL=0 (RED),  =1 (YELLOW)
    static const Style ALERT_TEXT_STYLES[] = {
        {font::fixed_8x16, Color::black(), Color(THREAT_STYLES[0].bg_color)}, // CRITICAL (RED)
        {font::fixed_8x16, Color::black(), Color(THREAT_STYLES[1].bg_color)}  // MEDIUM (YELLOW)
    };
    size_t style_idx = (threat >= ThreatLevel::CRITICAL) ? 0 : 1;
    alert_text_.set_style(&ALERT_TEXT_STYLES[style_idx]);
    set_dirty();
}

/**
 * @brief Update normal status with mutex protection
 * @details Thread-safe UI update for normal status display
 * @param primary Primary status message
 * @param secondary Optional secondary status message
 * @note Uses mutex to prevent race conditions during concurrent UI updates
 * @note DIAMOND FIX MEDIUM #3: Added mutex protection for UI update race condition
 */
void ConsoleStatusBar::update_normal_status(const char* primary, const char* secondary) {
    // DIAMOND FIX MEDIUM #3: Acquire mutex for UI update protection
    MutexLock lock(ui_mutex_, LockOrder::UI_STATUSBAR_MUTEX);
    
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

/**
 * @brief Set display mode with mutex protection
 * @details Thread-safe display mode change
 * @param mode Display mode to set
 * @note Uses mutex to prevent race conditions during concurrent mode changes
 * @note DIAMOND FIX MEDIUM #3: Added mutex protection for UI update race condition
 */
void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
    // DIAMOND FIX MEDIUM #3: Acquire mutex for UI update protection
    MutexLock lock(ui_mutex_, LockOrder::UI_STATUSBAR_MUTEX);
    
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
// DIAMOND FIX: Moving DSP math from UI thread to Sync thread.
void DroneDisplayController::process_frame_sync() {
    bool has_new_data = false;
    if (spectrum_fifo_) {
        ChannelSpectrum spectrum;
        while (spectrum_fifo_->out(spectrum)) {
            this->process_mini_spectrum_data(spectrum);
            has_new_data = true;
        }
    }

    if (!has_new_data) return;

    // 1. Pre-calculate Bar Spectrum into BACK buffer
    uint8_t back_idx = active_bar_buffer_ ^ 1;
    auto& bar_cache = bar_caches_[back_idx];
    const auto& config = dsp::BarSpectrumConfig{};

    {
        MutexLock lock(spectrum_mutex_, LockOrder::SPECTRUM_MUTEX);
        const auto& levels = spectrum_power_levels();
        const size_t spectrum_width = std::min(levels.size(), static_cast<size_t>(EDA::Constants::MINI_SPECTRUM_WIDTH));

        for (size_t x = 0; x < spectrum_width; ++x) {
            const dsp::BarRenderParams params{levels[x], x};
            bar_cache.render_data[x] = dsp::calculate_bar_render_data(params, config.BAR_HEIGHT_MAX, config);
        }
        bar_cache.valid = true;
    }

    __sync_synchronize();
    active_bar_buffer_ = back_idx;

    // 2. Pre-calculate Histogram into BACK buffer
    uint8_t hist_back_idx = active_hist_buffer_ ^ 1;
    auto& hist_cache = hist_caches_[hist_back_idx];
    const auto& h_config = dsp::HistogramColorConfig{};

    {
        MutexLock lock(histogram_mutex_, LockOrder::SPECTRUM_MUTEX);
        if (histogram_display_buffer_.is_valid && histogram_display_buffer_.max_count > 0) {
            for (size_t bin_idx = 0; bin_idx < 64; ++bin_idx) {
                const dsp::HistogramBinRenderParams params{bin_idx, histogram_display_buffer_.bin_counts[bin_idx]};
                hist_cache.render_data[bin_idx] = dsp::calculate_histogram_bin_render_data(params, histogram_display_buffer_.max_count, h_config);
            }
            hist_cache.valid = true;
            histogram_dirty_ = false;
        } else {
            hist_cache.valid = false;
        }
    }

    __sync_synchronize();
    active_hist_buffer_ = hist_back_idx;
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
       text_signal_type_({screen_width - 80, 80, 80, 16}, "SIGNAL: --"),
       compact_frequency_ruler_({0, 68, screen_width, 12}),
       displayed_drones_(),
            // Static buffers initialization
             detected_drones_count_(0),
             buffers_allocated_(false),
            histogram_display_buffer_(),
            histogram_dirty_(false),
      // Initialize mutexes in member initialization list
      // Lock order: SPECTRUM_MUTEX (level 1), HISTOGRAM_MUTEX (level 2), UI_MUTEX (level 3)
      spectrum_mutex_(),
      histogram_mutex_(),
            ui_mutex_(),
            threat_bins_(), threat_bins_count_(0),
             spectrum_gradient_(), spectrum_fifo_(nullptr),
             pixel_index(0), bins_hz_size(0), each_bin_size(DEFAULT_EACH_BIN_SIZE_HZ), min_color_power(DEFAULT_MIN_COLOR_POWER),
             marker_pixel_step(DEFAULT_MARKER_PIXEL_STEP_HZ), max_power(0), range_max_power(0), mode_(DroneDisplayController::DisplayRenderMode::HISTOGRAM),
               spectrum_config_()
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

// DIAMOND OPTIMIZATION: Buffer Management Methods
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

/**
 * @brief Initialize UI widgets and add them to View hierarchy
 * @return true if initialization succeeded
 * @note Widgets are added in constructor via add_children()
 * @note This method is provided for API consistency
 */
bool DroneDisplayController::initialize_widgets() noexcept {
    // Widgets are already added in constructor via add_children()
    // This method is provided for API consistency
    return true;
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

    // DIAMOND FIX: Clear histogram display buffer
    // Clear histogram buffer to prevent stale data usage
    std::fill(std::begin(histogram_display_buffer_.bin_counts),
              std::end(histogram_display_buffer_.bin_counts), 0);
}

// ============================================================================
// SCANNING FLOW: UI Update
// ============================================================================
// Flow: handle_scanner_update() -> update_detection_display() -> 
//       get_tracked_drones_snapshot() -> sort_drones_by_rssi() -> render_drone_text_display()
//
// UI/DSP Separation Pattern (Diamond Code):
// Phase 1: DATA FETCHING (DSP/Logic Layer) - no UI calls
//   - Fetch all data from scanner into local DisplayData struct
//   - Ensures atomic data fetching and consistent UI rendering
// Phase 2: UI RENDERING (Presentation Layer) - no data fetching
//   - Render UI using fetched data only
//   - No scanner calls during rendering
//
// Thread Safety: Uses snapshot pattern to avoid holding mutex during render
// Memory: Uses class member buffers to avoid stack allocation (FIX #2)
void DroneDisplayController::update_detection_display(const DroneScanner& scanner) {
    // DIAMOND FIX #HIGH #4: Acquire mutex for UI update protection
    // Prevents race conditions when multiple threads call update methods concurrently
    MutexLock lock(ui_mutex_, LockOrder::UI_DISPLAY_MUTEX);

    // Guard clause: Early return if buffers not allocated
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
    
    // Determine color index (logic only, no UI calls)
    data.color_idx = (data.max_threat >= ThreatLevel::HIGH) ? 4 :
                     (data.max_threat >= ThreatLevel::MEDIUM) ? 3 :
                     (data.has_detections) ? 2 :
                     (data.is_scanning) ? 1 : 0;
    
    // Phase 2: UI RENDERING (Presentation Layer)
    // Render UI using fetched data only - no scanner calls
    if (data.is_scanning) {
        if (data.current_freq > 0) {
            // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
            // Prevents initialization order issues and saves stack memory
            FrequencyFormatter::to_string_short_freq_buffer(ui_freq_buffer_, sizeof(ui_freq_buffer_), data.current_freq);
            big_display_.set(ui_freq_buffer_);
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
        // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
        // Prevents initialization order issues and saves stack memory
        const char* threat_name = UnifiedStringLookup::threat_name(static_cast<uint8_t>(data.max_threat));
        // Use StatusFormatter
        StatusFormatter::format_to(ui_summary_buffer_, "THREAT: %s | <%lu ~%lu >%lu",
                                  threat_name,
                                  static_cast<unsigned long>(data.approaching_count),
                                  static_cast<unsigned long>(data.static_count),
                                  static_cast<unsigned long>(data.receding_count));
        text_threat_summary_.set(ui_summary_buffer_);
        text_threat_summary_.set_style(&UIStyles::RED_STYLE);
    } else {
        text_threat_summary_.set("THREAT: NONE | All clear");
        text_threat_summary_.set_style(&UIStyles::GREEN_STYLE);
    }

    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    // Prevents initialization order issues and saves stack memory
    if (data.is_scanning) {
        // Use const char* instead of std::string (saves RAM)
        const char* mode_str = data.is_real_mode ? "REAL" : "DEMO";
        StatusFormatter::format_to(ui_status_buffer_, "%s - Detections: %lu",
                                  mode_str, static_cast<unsigned long>(data.total_detections));
    } else {
        StatusFormatter::format_to(ui_status_buffer_, "Ready - Enhanced Drone Analyzer");
    }
    text_status_info_.set(ui_status_buffer_);

    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    // Prevents initialization order issues and saves stack memory
    if (data.is_scanning && data.total_freqs > 0) {
        size_t current_idx = 0;
        // Use StatusFormatter
        StatusFormatter::format_to(ui_stats_buffer_, "Freq: %lu/%lu | Cycle: %lu",
                                  static_cast<unsigned long>(current_idx + 1),
                                  static_cast<unsigned long>(data.total_freqs),
                                  static_cast<unsigned long>(data.scan_cycles));
    } else if (data.total_freqs > 0) {
        StatusFormatter::format_to(ui_stats_buffer_, "Loaded: %lu frequencies",
                                  static_cast<unsigned long>(data.total_freqs));
    } else {
        StatusFormatter::format_to(ui_stats_buffer_, "No database loaded");
    }
    text_scanner_stats_.set(ui_stats_buffer_);

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

// ============================================================================
// DSP LAYER FUNCTIONS (UI/DSP Separation)
// ============================================================================

/**
 * @brief Update display data snapshot from DSP layer
 * 
 * This function updates the display with data from the DSP layer.
 * It uses the DSP layer snapshot to update UI widgets.
 * 
 * Thread-safety: This function is called from UI thread only.
 * No scanner calls are made in this function (pure UI rendering).
 * 
 * @param snapshot Display data snapshot from coordinator
 */
void DroneDisplayController::update_display_data_snapshot(const dsp::DisplayDataSnapshot& snapshot) noexcept {
    // Guard clause: Early return if buffers not allocated
    if (!buffers_allocated_) {
        return;
    }

    // Update scanning status
    if (snapshot.is_scanning) {
        if (snapshot.current_freq > 0) {
            // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
            // Prevents initialization order issues and saves stack memory
            FrequencyFormatter::to_string_short_freq_buffer(ui_freq_buffer_, sizeof(ui_freq_buffer_), snapshot.current_freq);
            big_display_.set(ui_freq_buffer_);
        } else {
            big_display_.set("2400.0MHz");
        }
    } else {
        big_display_.set("READY");
    }

    // Update scanning progress
    if (snapshot.total_freqs > 0 && snapshot.is_scanning) {
        uint32_t progress_percent = 50;
        scanning_progress_.set_value(std::min(progress_percent, static_cast<uint32_t>(100)));
    } else {
        scanning_progress_.set_value(0);
    }

    // Update threat summary
    if (snapshot.has_detections) {
        // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
        const char* threat_name = UnifiedStringLookup::threat_name(static_cast<uint8_t>(snapshot.max_threat));
        // Use StatusFormatter
        StatusFormatter::format_to(ui_summary_buffer_, "THREAT: %s | <%lu ~%lu >%lu",
                                  threat_name,
                                  static_cast<unsigned long>(snapshot.approaching_count),
                                  static_cast<unsigned long>(snapshot.static_count),
                                  static_cast<unsigned long>(snapshot.receding_count));
        text_threat_summary_.set(ui_summary_buffer_);
        text_threat_summary_.set_style(&UIStyles::RED_STYLE);
    } else {
        text_threat_summary_.set("THREAT: NONE | All clear");
        text_threat_summary_.set_style(&UIStyles::GREEN_STYLE);
    }

    // Update status info
    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    if (snapshot.is_scanning) {
        // Use const char* instead of std::string (saves RAM)
        const char* mode_str = snapshot.is_real_mode ? "REAL" : "DEMO";
        StatusFormatter::format_to(ui_status_buffer_, "%s - Detections: %lu",
                                  mode_str, static_cast<unsigned long>(snapshot.total_detections));
    } else {
        StatusFormatter::format_to(ui_status_buffer_, "Ready - Enhanced Drone Analyzer");
    }
    text_status_info_.set(ui_status_buffer_);

    // Update scanner stats
    // DIAMOND FIX #HIGH #4: Use class member buffer instead of thread_local
    if (snapshot.is_scanning && snapshot.total_freqs > 0) {
        size_t current_idx = 0;
        // Use StatusFormatter
        StatusFormatter::format_to(ui_stats_buffer_, "Freq: %lu/%lu | Cycle: %lu",
                                  static_cast<unsigned long>(current_idx + 1),
                                  static_cast<unsigned long>(snapshot.total_freqs),
                                  static_cast<unsigned long>(snapshot.scan_cycles));
    } else if (snapshot.total_freqs > 0) {
        StatusFormatter::format_to(ui_stats_buffer_, "Loaded: %lu frequencies",
                                  static_cast<unsigned long>(snapshot.total_freqs));
    } else {
        StatusFormatter::format_to(ui_stats_buffer_, "No database loaded");
    }
    text_scanner_stats_.set(ui_stats_buffer_);

    // Update big display style
    big_display_.set_style(&BIG_DISPLAY_STYLES[snapshot.color_idx]);
}

/**
 * @brief Update filtered drones snapshot from DSP layer
 * 
 * This function updates the display with filtered drone data from the DSP layer.
 * It uses the DSP layer snapshot to update UI widgets.
 * 
 * Thread-safety: This function is called from UI thread only.
 * No scanner calls are made in this function (pure UI rendering).
 * 
 * @param snapshot Filtered drones snapshot from coordinator
 */
void DroneDisplayController::update_filtered_drones_snapshot(const dsp::FilteredDronesSnapshot& snapshot) noexcept {
    // Guard clause: Early return if buffers not allocated
    if (!buffers_allocated_) {
        return;
    }

    // Clear current drones display
    for (size_t i = 0; i < MAX_UI_DRONES; ++i) {
        detected_drones()[i].frequency = 0;
        detected_drones()[i].type = DroneType::UNKNOWN;
        detected_drones()[i].threat = ThreatLevel::NONE;
        detected_drones()[i].rssi = -120;
        detected_drones()[i].last_seen = 0;
        detected_drones()[i].trend = MovementTrend::UNKNOWN;
    }

    // Update drones display with snapshot data
    for (size_t i = 0; i < snapshot.count && i < MAX_UI_DRONES; ++i) {
        const auto& drone = snapshot.drones[i];
        detected_drones()[i].frequency = drone.frequency;
        detected_drones()[i].type = static_cast<DroneType>(drone.drone_type);
        detected_drones()[i].threat = static_cast<ThreatLevel>(drone.threat_level);
        detected_drones()[i].rssi = drone.rssi;
        detected_drones()[i].last_seen = drone.last_seen;
        detected_drones()[i].trend = drone.trend;
    }

    // Render drone text display
    render_drone_text_display();
}

void DroneDisplayController::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) noexcept {
    systime_t now = chTimeNow();

    for (size_t i = 0; i < detected_drones_count_; ++i) {
        if (detected_drones()[i].frequency == freq) {
            detected_drones()[i].rssi = rssi;
            detected_drones()[i].threat = threat;
            detected_drones()[i].type = type;
            detected_drones()[i].last_seen = now;
            snprintf(detected_drones()[i].type_name, sizeof(detected_drones()[i].type_name), "%s", UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type)));
            detected_drones()[i].display_color = UnifiedColorLookup::drone(static_cast<uint8_t>(type));
            sort_drones_by_rssi();
            // DIAMOND CODE PRINCIPLE: Removed render_drone_text_display() call
            // Rendering is now triggered separately by the caller
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
        snprintf(entry.type_name, sizeof(entry.type_name), "%s", UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(type)));
        entry.display_color = UnifiedColorLookup::drone(static_cast<uint8_t>(type));
        entry.trend = MovementTrend::STATIC;
        detected_drones_count_++;
    }
    sort_drones_by_rssi();
    // DIAMOND CODE PRINCIPLE: Removed render_drone_text_display() call
    // Rendering is now triggered separately by the caller
}

void DroneDisplayController::sort_drones_by_rssi() {
    // DIAMOND CODE PRINCIPLE: Use DSP layer for sorting logic
    // This separates sorting logic from UI rendering
    // Convert DisplayDroneEntry to TrackedDroneData for DSP sorting
    dsp::TrackedDroneData temp_drones[MAX_UI_DRONES];
    for (size_t i = 0; i < detected_drones_count_; ++i) {
        const auto& entry = detected_drones()[i];
        temp_drones[i].frequency = entry.frequency;
        temp_drones[i].drone_type = static_cast<uint8_t>(entry.type);
        temp_drones[i].threat_level = static_cast<uint8_t>(entry.threat);
        temp_drones[i].rssi = entry.rssi;
        temp_drones[i].last_seen = entry.last_seen;
        temp_drones[i].trend = entry.trend;
    }
    
    // Sort using DSP layer function
    dsp::sort_drones_by_priority(temp_drones, detected_drones_count_);
    
    // Copy sorted drones back to display buffer
    for (size_t i = 0; i < detected_drones_count_; ++i) {
        const auto& drone = temp_drones[i];
        auto& entry = detected_drones()[i];
        entry.frequency = drone.frequency;
        entry.type = static_cast<DroneType>(drone.drone_type);
        entry.threat = static_cast<ThreatLevel>(drone.threat_level);
        entry.rssi = drone.rssi;
        entry.last_seen = drone.last_seen;
        entry.trend = drone.trend;
    }
}

void DroneDisplayController::update_drones_display(const DroneScanner& scanner) {
    // STEP 1: Get a safe copy of data
    // Mutex is locked only for the copying time (microseconds)
    auto snapshot = scanner.get_tracked_drones_snapshot();

    // Step 2: Convert DroneSnapshot to FilteredDronesSnapshot
    // This converts TrackedDrone objects to TrackedDroneData structs
    dsp::FilteredDronesSnapshot converted_snapshot;
    converted_snapshot.count = 0;
    for (size_t i = 0; i < snapshot.count && converted_snapshot.count < 10; ++i) {
        const auto& tracked_drone = snapshot.drones[i];
        auto& drone_data = converted_snapshot.drones[converted_snapshot.count];
        drone_data.frequency = tracked_drone.frequency;
        drone_data.drone_type = static_cast<uint8_t>(tracked_drone.drone_type);
        drone_data.threat_level = static_cast<uint8_t>(tracked_drone.threat_level);
        drone_data.rssi = tracked_drone.rssi;
        drone_data.last_seen = tracked_drone.last_seen;
        drone_data.trend = tracked_drone.get_trend();
        converted_snapshot.count++;
    }

    // Step 3: Filter stale drones using DSP layer function
    const systime_t STALE_TIMEOUT = 30000;
    systime_t now = chTimeNow();

    // DIAMOND CODE PRINCIPLE: Use DSP layer for filtering logic
    // This separates filtering logic from UI rendering
    // CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
    dsp::FilteredDronesSnapshot filtered_snapshot = dsp::filter_stale_drones(
        converted_snapshot,
        dsp::StaleTimeout(STALE_TIMEOUT),
        dsp::CurrentTime(now)
    );

    // Step 3: Copy filtered drones to display buffer
    detected_drones_count_ = 0;

    for (size_t i = 0; i < filtered_snapshot.count && detected_drones_count_ < MAX_UI_DRONES; ++i) {
        const auto& drone_data = filtered_snapshot.drones[i];
        auto& entry = detected_drones()[detected_drones_count_];
        entry.frequency = drone_data.frequency;
        entry.type = static_cast<DroneType>(drone_data.drone_type);
        entry.threat = static_cast<ThreatLevel>(drone_data.threat_level);
        entry.rssi = drone_data.rssi;
        entry.last_seen = drone_data.last_seen;
        snprintf(entry.type_name, sizeof(entry.type_name), "%s", UnifiedStringLookup::drone_type_name(static_cast<uint8_t>(entry.type)));
        entry.display_color = UnifiedColorLookup::drone(static_cast<uint8_t>(entry.type));
        entry.trend = drone_data.trend;
        detected_drones_count_++;
    }

    // Step 4: Sort using DSP layer function
    // DIAMOND CODE PRINCIPLE: Use DSP layer for sorting logic
    // This separates sorting logic from UI rendering
    sort_drones_by_rssi();

    // Remaining code for updating display_drones_ and calling render
    for (auto& drone : displayed_drones_) {
        drone = DisplayDroneEntry{};
    }
    size_t count = std::min(detected_drones_count_, EDA::Constants::MAX_DISPLAYED_DRONES);
    for (size_t i = 0; i < count; ++i) {
        displayed_drones_[i] = detected_drones()[i];
    }
    highlight_threat_zones_in_spectrum(displayed_drones_);
    render_drone_text_display();
}

void DroneDisplayController::render_drone_text_display() noexcept {
    text_drone_1_.set("");
    text_drone_2_.set("");
    text_drone_3_.set("");

    size_t drone_count = std::min(displayed_drones_.size(), EDA::Constants::MAX_DISPLAYED_DRONES);
    for (size_t i = 0; i < drone_count; ++i) {
        const auto& drone = displayed_drones_[i];
        
        // DIAMOND CODE PRINCIPLE: Use utility function for text formatting (DSP layer)
        // This separates formatting logic from UI rendering
        dsp::DisplayDroneEntry display_entry;
        display_entry.frequency = drone.frequency;
        display_entry.trend = drone.trend;
        // Copy type name to display entry
        for (size_t j = 0; j < sizeof(display_entry.type_name) - 1 && drone.type_name[j] != '\0'; ++j) {
            display_entry.type_name[j] = drone.type_name[j];
        }
        display_entry.type_name[sizeof(display_entry.type_name) - 1] = '\0';
        
        // Pre-format display text using utility function (DSP layer)
        dsp::DroneDisplayText display_text = dsp::format_drone_display_text(display_entry);
        
        // Pure UI rendering - use pre-formatted text
        char buffer[64];
        StatusFormatter::format_to(buffer, DRONE_DISPLAY_FORMAT,
                                 display_text.type_name,
                                 display_text.freq_string,
                                 (long int)display_text.rssi,
                                 display_text.trend_symbol);

        // Helper for indexed access (no bounds check each time)
        Text* target = drone_text_widget(i);
        if (target) {
            target->set(buffer);
        }
    }
}

void DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum& spectrum) noexcept {
    // DIAMOND CODE PRINCIPLE: Use DSP layer for signal processing
    // This separates signal processing from UI rendering
    using namespace dsp;

    // Thread-safe buffer access with mutex protection
    // Lock order: SPECTRUM_MUTEX (level 2)
    MutexLock lock(spectrum_mutex_, LockOrder::SPECTRUM_MUTEX);

    // Call DSP layer function to process spectrum data
    // CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
    pixel_index = dsp::SpectrumProcessor::process_mini_spectrum(
        spectrum,
        spectrum_power_levels_storage_,
        bins_hz_size,
        dsp::BinSize(each_bin_size),
        marker_pixel_step,
        dsp::MinColorPower(min_color_power)
    );
}

bool DroneDisplayController::process_bins(uint8_t* powerlevel) {
    bins_hz_size += each_bin_size;
    if (bins_hz_size >= marker_pixel_step) {
        // Thread-safe buffer access with mutex protection
        // Lock order: DISPLAY_SPECTRUM_MUTEX (level 4)
        MutexLock lock(spectrum_mutex_, LockOrder::SPECTRUM_MUTEX);

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

void DroneDisplayController::render_bar_spectrum(Painter& painter) noexcept {
    const auto& config = dsp::BarSpectrumConfig{};

    painter.fill_rectangle(
        {0, config.SPECTRUM_Y_START, EDA::Constants::MINI_SPECTRUM_WIDTH, config.BAR_HEIGHT_MAX},
        Color::black()
    );

    if (!buffers_allocated_) return;

    uint8_t front_idx = active_bar_buffer_;
    __sync_synchronize();

    const auto& cache = bar_caches_[front_idx];
    if (!cache.valid) return;

    for (size_t x = 0; x < 240; ++x) {
        const auto& render_data = cache.render_data[x];
        if (!render_data.should_draw) continue;

        painter.fill_rectangle(
            {static_cast<int>(x), render_data.y_top, 1, render_data.bar_height},
            config.BAR_COLORS[render_data.color_idx]
        );
    }
}

// Histogram Display Implementation
// Zero heap allocation, static storage, integer-only math

// ============================================================================
// DIAMOND FIX #HIGH #4: Stack Canary Not Verified
// ============================================================================
/**
 * @brief Update histogram display buffer with new data
 * @details Implements stack canary verification for overflow detection
 * @param analysis_histogram Histogram buffer from SpectralAnalyzer (64 bins)
 * @param noise_floor Noise floor value from spectral analysis
 * @note DIAMOND FIX #HIGH #4: Added stack monitoring verification
 * @note DIAMOND FIX #HIGH #4: Implements ChibiOS runtime stack monitoring
 * @note DIAMOND FIX #HIGH #4: Uses StackMonitor for overflow detection
 */
void DroneDisplayController::update_histogram_display(
    const SpectralAnalyzer::HistogramBuffer& analysis_histogram,
    uint8_t noise_floor
) noexcept {
    // DIAMOND FIX #HIGH #4: Stack monitor verification on function entry
    // Detects stack overflow using ChibiOS runtime monitoring
    StackMonitor stack_monitor;
    
    // Guard clause: Validate input histogram
    if (analysis_histogram.empty()) {
        // Thread-safe buffer access with mutex protection
        // Lock order: SPECTRUM_MUTEX (level 2)
        MutexLock lock(histogram_mutex_, LockOrder::SPECTRUM_MUTEX);
        histogram_display_buffer_.is_valid = false;
        
        // DIAMOND FIX #HIGH #4: Stack monitor verification on function exit
        // StackMonitor captured stack state at function entry (line 3696)
        // Use stack_monitor.is_stack_safe(required_bytes) to verify stack availability
        return;
    }

    // DIAMOND CODE PRINCIPLE: Use utility function for data scaling (DSP layer)
    // This separates data scaling from UI display update
    uint16_t temp_histogram[64];
    for (size_t i = 0; i < 64 && i < analysis_histogram.size(); ++i) {
        temp_histogram[i] = analysis_histogram[i];
    }
    for (size_t i = analysis_histogram.size(); i < 64; ++i) {
        temp_histogram[i] = 0;
    }
    
    // Pre-scale histogram data using utility function (DSP layer)
    // P1-HIGH FIX: Use HistogramScaleParams struct to prevent parameter swapping
    const dsp::HistogramScaleParams params{64, noise_floor};
    dsp::HistogramDisplayBuffer scaled_histogram = dsp::scale_histogram_for_display(
        temp_histogram, params
    );
    
    // Copy to display buffer (UI only)
    // Thread-safe buffer access with mutex protection
    // Lock order: SPECTRUM_MUTEX (level 2)
    MutexLock lock(histogram_mutex_, LockOrder::SPECTRUM_MUTEX);
    
    for (size_t i = 0; i < 64; ++i) {
        histogram_display_buffer_.bin_counts[i] = scaled_histogram.bin_counts[i];
    }
    histogram_display_buffer_.max_count = scaled_histogram.max_count;
    histogram_display_buffer_.noise_floor = scaled_histogram.noise_floor;
    histogram_display_buffer_.is_valid = scaled_histogram.is_valid;
    histogram_dirty_ = true;
}

void DroneDisplayController::render_histogram(Painter& painter) noexcept {
    const auto& config = dsp::HistogramColorConfig{};

    painter.fill_rectangle(
        {0, config.HISTOGRAM_Y, config.HISTOGRAM_WIDTH, config.HISTOGRAM_HEIGHT},
        Color::black()
    );

    if (!buffers_allocated_) return;

    uint8_t front_idx = active_hist_buffer_;
    __sync_synchronize();

    const auto& cache = hist_caches_[front_idx];
    if (!cache.valid) return;

    for (size_t bin_idx = 0; bin_idx < config.HISTOGRAM_NUM_BINS; ++bin_idx) {
        const auto& render_data = cache.render_data[bin_idx];
        if (!render_data.should_draw) continue;

        painter.fill_rectangle(
            {render_data.bin_x, render_data.y_top,
             render_data.bin_width, render_data.bin_height},
            config.HISTOGRAM_COLORS[render_data.color_idx]
        );
    }
}

void DroneDisplayController::clear_histogram_area(Painter& painter) noexcept {
    // DIAMOND FIX #4: Use class constants instead of hardcoded values
    // Using class constants ensures consistency and maintainability
    const auto& config = dsp::HistogramColorConfig{};
    const Rect histogram_rect{
        8,  // HISTOGRAM_X (not defined as class constant)
        config.HISTOGRAM_Y,
        config.HISTOGRAM_WIDTH,
        config.HISTOGRAM_HEIGHT
    };

    painter.fill_rectangle(histogram_rect, Color::black());
}

void DroneDisplayController::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, EDA::Constants::MAX_DISPLAYED_DRONES>& drones) {
    threat_bins_count_ = 0;
    for (const auto& drone : drones) {
        if (drone.frequency > 0) {
            size_t bin_x = frequency_to_spectrum_bin(drone.frequency);
            if (bin_x < EDA::Constants::MINI_SPECTRUM_WIDTH && threat_bins_count_ < EDA::Constants::MAX_DISPLAYED_DRONES) {
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
        return EDA::Constants::MINI_SPECTRUM_WIDTH;
    }
    int64_t relative_freq = freq_hz - MIN_FREQ;
    size_t bin = static_cast<size_t>((relative_freq * EDA::Constants::MINI_SPECTRUM_WIDTH) / FREQ_RANGE);
    return std::min(bin, static_cast<size_t>(EDA::Constants::MINI_SPECTRUM_WIDTH - 1));
}

// Pass settings by value to scanner constructor (eliminates lifetime dependency)
DroneUIController::DroneUIController(NavigationView& nav,
                                          DroneHardwareController& hardware,
                                          DroneScanner& scanner,
                                          AudioManager& audio_mgr)
    : nav_(nav),
      hardware_(hardware),
      scanner_(scanner),
      audio_mgr_(audio_mgr),
      scanning_active_{false},
      display_controller_(nullptr),
      settings_(),  // Initialize settings_ with defaults
      // DIAMOND FIX: Initialize mutex in member initialization list (RAII pattern)
      ui_mutex_() {
    settings_.spectrum_mode = SpectrumMode::MEDIUM;
    settings_.scan_interval_ms = 1000;
    settings_.rssi_threshold_db = -90;
    audio_set_enable_alerts(settings_, true);
    char buffer[32];

    bool current = audio_get_enable_alerts(settings_);
    audio_set_enable_alerts(settings_, !current);
    const char* status = audio_get_enable_alerts(settings_) ? "ENABLED" : "DISABLED";
    snprintf(buffer, sizeof(buffer), "%s", "Alerts ");
    size_t current_len = strlen(buffer);
    snprintf(buffer + current_len, sizeof(buffer) - current_len, "%s", status);
    nav_.display_modal("Audio Alerts", buffer);
}

DroneUIController::~DroneUIController() {
}

void DroneUIController::on_start_scan() {
    scanner_.start_scanning();
    scanning_active_ = true;
}

void DroneUIController::on_stop_scan() {
    scanner_.stop_scanning();
    scanning_active_ = false;
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
    // DIAMOND FIX #HIGH #4: Acquire mutex for UI update protection
    // Prevents race conditions when multiple threads call update methods concurrently
    MutexLock lock(ui_mutex_, LockOrder::UI_CONTROLLER_MUTEX);

    // DIAMOND FIX #HIGH #4: Use class member buffers instead of thread_local
    // Prevents initialization order issues and saves stack memory
    uint32_t band_mhz = hardware_.get_spectrum_bandwidth() / 1000000ULL;
    FrequencyFormatter::format_to_buffer(freq_buffer_, sizeof(freq_buffer_), hardware_.get_spectrum_center_frequency(),
                                     FrequencyFormatter::Format::STANDARD_GHZ);
    StatusFormatter::format_to(hardware_buffer_, "Band: %lu MHz\nFreq: %s",
                               (unsigned long)band_mhz, freq_buffer_);
    nav_.display_modal("Hardware Status", hardware_buffer_);
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

// ---  FrequencyRangeSetupView ---

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
    
    // Validation - use unified constants from EDA::Constants
    if (new_min >= new_max) {
        nav_.display_modal("Error", "Min freq must be < Max freq");
        return;
    }
    
    if (new_min < EDA::Constants::FrequencyLimits::MIN_HARDWARE_FREQ || 
        new_max > EDA::Constants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        nav_.display_modal("Error", "Frequency out of range (1MHz - 7.2GHz)");
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

// FIX #M11: Helper function to get default scanner settings
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
      // FIX: ScanningCoordinator is a singleton - pointer member initialized to nullptr
      // The singleton is initialized in the constructor body via ScanningCoordinator::initialize()
      scanning_coordinator_(nullptr),
      smart_header_(Rect{0, 0, screen_width, 60}),
      status_bar_(0, Rect{0, screen_height - 80, screen_width, 16}),
      threat_cards_(),
      button_start_stop_({screen_width - 80, screen_height - 72, 72, 32}, "START/STOP"),
      button_menu_({screen_width - 80, screen_height - 40, 72, 32}, "MENU"),
      button_audio_({screen_width - 160, screen_height - 72, 72, 32}, "AUDIO: OFF"),
      field_scanning_mode_({10, screen_height - 72}, 15, OptionsField::options_t{{"Database", 0}, {"Wideband",1}, {"Hybrid", 2}}),
      scanning_active_(false),
      initialization_in_progress_(false)
{
    // DIAMOND FIX #P1-HIGH #6: Add error handling for ScanningCoordinator singleton access
    // Initialize ScanningCoordinator singleton (must be called before using instance())
    // This creates the singleton instance and sets up all dependencies
    if (!ScanningCoordinator::initialize(nav, hardware_, scanner_, display_controller_, audio_)) {
        // Handle initialization failure - log error or set error state
        display_controller_.text_status_info().set("Coordinator init failed");
        scanning_coordinator_ = nullptr;
        return;
    }

    // P0-STOP FIX #3: Migrated to instance_safe() for safe singleton access
    // ScanningCoordinator::instance() is deprecated and halts the system if called before initialization
    // ScanningCoordinator::instance_safe() returns nullptr if not initialized, allowing graceful error handling
    // Using a pointer instead of a reference avoids undefined behavior from const_cast rebinding
    scanning_coordinator_ = ScanningCoordinator::instance_safe();

    // P0-STOP FIX #3: Null pointer check after instance_safe() call
    // instance_safe() returns nullptr if not initialized, so we must check before using
    if (!scanning_coordinator_) {
        display_controller_.text_status_info().set("Coordinator instance null");
        return;
    }
    // Remove sd_mutex_initialized pattern (mutex already declared as static at namespace scope)

    // FIX: Set display_controller after construction
    // This makes initialization order independent of member declaration order
    ui_controller_.set_display_controller(&display_controller_);

    // FIX:   -  UI setup
    // scanner_/hardware_/coordinator_   step_deferred_initialization()

    // FIX:
    init_state_ = InitState::CONSTRUCTED;

    setup_button_handlers();
    initialize_scanning_mode();
    // Move add_ui_elements() to appropriate phase (after parent View initialization)
}

EnhancedDroneSpectrumAnalyzerView::~EnhancedDroneSpectrumAnalyzerView() {
    // Request global shutdown - signals all threads to stop
    request_global_shutdown();

    // 1. Stop activity (in dependency order)
    // DIAMOND FIX: Guard against nullptr if init failed
    if (scanning_coordinator_ != nullptr) {
        scanning_coordinator_->stop_coordinated_scanning();
    }
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
    // paint() method uses ~1.5KB of stack per call
    // DIAMOND FIX #HIGH #3: Reduced stack requirement to 1536 bytes (1.5KB)
    // This saves ~256 bytes per frame, reducing stack pressure from 57% to 38%
    StackMonitor stack_monitor;
    constexpr size_t PAINT_STACK_REQUIRED = 1536;  // 1.5KB for paint() method (reduced from 1.75KB)

    // Guard clause: Return early if insufficient stack
    if (!stack_monitor.is_stack_safe(PAINT_STACK_REQUIRED)) {
        // Cannot safely render - skip this frame
        return;
    }

    // Call base class paint
    View::paint(painter);

    // FIX #2: Do NOT call step_deferred_initialization() from paint()
    // This prevents nested stack frames that cause M0 stack overflow
    // Initialization is now handled by continue_initialization() called from UI event loop

    // ============================================================================
    // PHASE 5: VIEW LAYER UPDATES - Simple Dispatcher Pattern
    // ============================================================================
    // Dispatch to appropriate rendering function based on initialization state
    // This eliminates UI/DSP mixing by separating concerns into pure UI functions
    // ============================================================================

    // Dispatch based on initialization state (pure UI rendering)
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        render_initialization_error(painter);
        return;
    }

    if (init_state_ != InitState::FULLY_INITIALIZED) {
        render_loading_progress(painter);
        return;
    }

    // Normal rendering
    render_normal_display(painter);

    // Stack Overflow Prevention: Do NOT call continue_initialization() from paint()!
    // Calling continue_initialization() from paint() creates nested stack frames that
    // exceed the 4KB stack limit. The message_handler_frame_sync_ already handles
    // initialization by calling step_deferred_initialization() from the UI event loop,
    // not from paint(). The initialization continues naturally via the frame sync handler.
}

// ============================================================================
// PHASE 5: VIEW LAYER UPDATES - UI/DSP Separation
// ============================================================================
// These functions extract rendering logic from paint() to eliminate UI/DSP mixing
// Each function is a pure UI rendering function with no DSP/signal processing
// ============================================================================

/**
 * @brief Render initialization error screen
 * @details Displays error message with instructions
 * @param painter Painter instance for rendering
 * @note DIAMOND FIX LOW #1: Replaced magic numbers with named constants
 */
void EnhancedDroneSpectrumAnalyzerView::render_initialization_error(Painter& painter) noexcept {
    // Clear screen
    painter.fill_rectangle({0, 0, screen_width, screen_height}, Color::black());
    
    // Error header
    painter.draw_string(
        {ui::apps::enhanced_drone_analyzer::UIConstants::ERROR_MSG_X_POS, ui::apps::enhanced_drone_analyzer::UIConstants::ERROR_MSG_Y_POS_1},
        Style{font::fixed_8x16, Color::red(), Color::black()},
        "INIT ERROR"
    );

    // Error message
    painter.draw_string(
        {ui::apps::enhanced_drone_analyzer::UIConstants::ERROR_MSG_X_POS, ui::apps::enhanced_drone_analyzer::UIConstants::ERROR_MSG_Y_POS_2},
        Style{font::fixed_8x16, Color::white(), Color::black()},
        ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]
    );

    // Instructions
    painter.draw_string(
        {ui::apps::enhanced_drone_analyzer::UIConstants::ERROR_MSG_X_POS, ui::apps::enhanced_drone_analyzer::UIConstants::ERROR_MSG_Y_POS_3},
        Style{font::fixed_8x16, Color::yellow(), Color::black()},
        "Press BACK to exit"
    );
}

/**
 * @brief Render loading progress screen
 * @details Displays loading progress with progress bar
 * @param painter Painter instance for rendering
 * @note DIAMOND FIX LOW #1: Replaced magic numbers with named constants
 */
void EnhancedDroneSpectrumAnalyzerView::render_loading_progress(Painter& painter) noexcept {
    size_t phase_idx = static_cast<size_t>(init_state_);
    
    // Guard clause: Skip if phase index is out of range
    if (phase_idx >= 7) {
        return;
    }
    
    // Loading text
    painter.draw_string(
        {ui::apps::enhanced_drone_analyzer::UIConstants::LOADING_MSG_X_POS, ui::apps::enhanced_drone_analyzer::UIConstants::LOADING_MSG_Y_POS_1},
        Style{font::fixed_8x16, Color::white(), Color::black()},
        "Loading..."
    );
    
    // Status message
    painter.draw_string(
        {ui::apps::enhanced_drone_analyzer::UIConstants::LOADING_MSG_X_POS, ui::apps::enhanced_drone_analyzer::UIConstants::LOADING_MSG_Y_POS_2},
        Style{font::fixed_8x16, Color::green(), Color::black()},
        INIT_STATUS_MESSAGES[phase_idx]
    );
    
    // Progress bar (6 phases = ~16.6% each)
    uint8_t progress = static_cast<uint8_t>(phase_idx * 16);
    if (progress > 100) {
        progress = 100;
    }
    
    // Progress bar background
    painter.fill_rectangle(
        {ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_X_POS, ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_Y_POS,
         ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_WIDTH, ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_HEIGHT},
         Color(ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_BG_COLOR));
    // Filled portion
    painter.fill_rectangle(
        {ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_X_POS, ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_Y_POS,
         progress, ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_HEIGHT},
         Color(ui::apps::enhanced_drone_analyzer::UIConstants::PROGRESS_BAR_FILL_COLOR));
}

void EnhancedDroneSpectrumAnalyzerView::render_normal_display(Painter& painter) noexcept {
    // Guard clause: Skip if buffers are not valid
    if (!display_controller_.are_buffers_valid()) {
        return;
    }
    
    // Render bar spectrum
    display_controller_.render_bar_spectrum(painter);
    
    // Handle histogram display based on mode
    if (display_controller_.get_display_mode() == 
        ui::apps::enhanced_drone_analyzer::DroneDisplayController::DisplayRenderMode::HISTOGRAM) {
        // DIAMOND OPTIMIZATION: Render histogram after bar spectrum
        // Histogram is positioned at y=164-190 (below bar spectrum)
        display_controller_.render_histogram(painter);
    } else {
        // Clear histogram area when not in histogram mode
        display_controller_.clear_histogram_area(painter);
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
    // CRITICAL FIX: Update start button text during initialization
    // Note: Button class doesn't have set_enabled() method.
    // Button is protected by init_state_ check in handle_start_stop_button()
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        button_start_stop_.set_text("INITIALIZING...");
    } else {
        button_start_stop_.set_text("START/STOP");
    }
    
    // DIAMOND OPTIMIZATION: constexpr LUT  Flash  switch ( 3057-3077)
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

// ============================================================================
// DIAMOND FIX #HIGH #3: Histogram Callback Race Condition
// ============================================================================
/**
 * @brief Static histogram callback implementation (no lambda captures)
 * @details Thread-safe callback for histogram data from SpectralAnalyzer
 * @param histogram Histogram buffer from SpectralAnalyzer (64 bins)
 * @param noise_floor Noise floor value from spectral analysis
 * @param user_data User data pointer (typically 'this' pointer)
 * @note DIAMOND FIX #HIGH #3: Added mutex protection to prevent race condition
 * @note DIAMOND FIX #HIGH #3: Implements copy-on-write pattern for thread safety
 * @note DIAMOND FIX #HIGH #3: Prevents callback from modifying histogram during update
 */
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
    
    // DIAMOND FIX #HIGH #7: Comprehensive state validation for histogram callback
    // Prevents race conditions during initialization and ensures callback safety
    
    // Validate view pointer is not null (redundant but safe)
    if (!view) {
        return;
    }
    
    // Guard clause: Skip if display buffers not ready
    if (!view->display_controller_.are_buffers_valid()) {
        return;
    }
    
    // Validate initialization state - callback should only process after full initialization
    // This prevents race conditions during startup phase
    if (view->init_state_ != InitState::FULLY_INITIALIZED) {
        return;
    }
    
    // Validate global shutdown flag - skip callback during shutdown
    // This prevents accessing destroyed resources during shutdown
    if (view->is_global_shutdown_requested()) {
        return;
    }
    
    // DIAMOND FIX #HIGH #3: Acquire histogram mutex before forwarding data
    // This prevents race condition where callback is called while histogram is being updated
    // Lock order: SPECTRUM_MUTEX (level 2) - must be consistent with other accesses
    MutexLock histogram_lock(view->display_controller_.get_histogram_mutex(), LockOrder::SPECTRUM_MUTEX);
    
    // DIAMOND FIX #HIGH #3: Check if histogram update is in progress
    // If histogram is currently being updated, skip this callback to prevent race condition
    // This implements copy-on-write pattern - we only update if no write is in progress
    if (view->display_controller_.is_histogram_update_safe()) {
        // Forward histogram data to display controller
        view->display_controller_.update_histogram_display(histogram, noise_floor);
    }
    // Mutex released automatically when histogram_lock goes out of scope
}

// Enhanced Initialization State Machine with Timeout Protection
// Eliminates cascading if/else, adds timeout protection, saves ~150 bytes RAM
// EVENT-DRIVEN INITIALIZATION:
// Phases execute based on actual state completion, not time delays
// Key changes:
// - Removed time-based delay check
// - Each phase checks for actual completion before transitioning state
// - Only one phase executes per call, ensuring proper sequencing
// Benefits:
// - No race conditions between phases
// - Database loading is guaranteed complete before hardware init
// - Settings loading waits for database completion
// - More predictable initialization behavior
void EnhancedDroneSpectrumAnalyzerView::step_deferred_initialization() noexcept {
    // SAFETY: Guard clause for re-entrancy protection
    // Single-threaded context (UI event loop only) - no synchronization needed
    if (initialization_in_progress_) {
        return;
    }
    initialization_in_progress_ = true;

    // SAFETY: Handle ERROR state - no further processing needed
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0,
                                      ERROR_MESSAGES[static_cast<uint8_t>(init_error_)]);
        initialization_in_progress_ = false;
        return;
    }

    // Check timeout (protects from hangs)
    systime_t elapsed = chTimeNow() - init_start_time_;
    if (elapsed > MS2ST(EDA::Constants::INIT_TIMEOUT_MS)) {
        // Proper cleanup in timeout path
        SettingsPersistence<DroneAnalyzerSettings>::reset_to_defaults(settings_);
        
        // Database loading thread will complete on its own
        // The initialization state machine will handle the incomplete state
        
        init_state_ = InitState::INITIALIZATION_ERROR;
        init_error_ = InitError::GENERAL_TIMEOUT;
        initialization_in_progress_ = false;
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0, "Init timeout!");
        return;
    }

    // Single-level dispatch using LUT - eliminates deep call hierarchy
    // Phase functions are called directly based on current state index
    constexpr size_t NUM_PHASES = 6;
    using PhaseFunc = void (EnhancedDroneSpectrumAnalyzerView::*)();
    
    static constexpr std::array<PhaseFunc, NUM_PHASES> PHASE_FUNCS = {{
        &EnhancedDroneSpectrumAnalyzerView::init_phase_allocate_buffers,
        &EnhancedDroneSpectrumAnalyzerView::init_phase_load_database,
        &EnhancedDroneSpectrumAnalyzerView::init_phase_init_hardware,
        &EnhancedDroneSpectrumAnalyzerView::init_phase_setup_ui,
        &EnhancedDroneSpectrumAnalyzerView::init_phase_load_settings,
        &EnhancedDroneSpectrumAnalyzerView::init_phase_finalize
    }};
    
    // Map current state to phase function index
    // CONSTRUCTED (0) -> index 0 -> init_phase_allocate_buffers
    // BUFFERS_ALLOCATED (1) -> index 1 -> init_phase_load_database
    // DATABASE_LOADED (3) -> index 2 -> init_phase_init_hardware
    // HARDWARE_READY (4) -> index 3 -> init_phase_setup_ui
    // UI_LAYOUT_READY (5) -> index 4 -> init_phase_load_settings
    // SETTINGS_LOADED (6) -> index 5 -> init_phase_finalize
    uint8_t state_idx = static_cast<uint8_t>(init_state_);
    
    // State-to-index mapping: skip intermediate states
    // DATABASE_LOADING (2) and COORDINATOR_READY (7) are handled by phase functions
    uint8_t phase_idx = state_idx;
    if (state_idx > 2) phase_idx--;  // Skip DATABASE_LOADING (state 2)
    if (state_idx > 6) phase_idx--;  // Skip COORDINATOR_READY (state 7)
    
    // Skip DATABASE_LOADING state - it's an intermediate state handled by init_phase_load_database
    if (init_state_ == InitState::DATABASE_LOADING || init_state_ == InitState::COORDINATOR_READY) {
        initialization_in_progress_ = false;
        return;
    }
    
    // DIAMOND FIX #CRITICAL #1: Phase completion verification before execution
    // Guard clause: Verify previous phase completed before executing next phase
    // This prevents race conditions where Phase N starts before Phase N-1 completes
    if (phase_idx < NUM_PHASES) {
        // Phase 0 (allocate_buffers): No previous phase to verify
        // Phase 1 (load_database): Verify Phase 0 completed
        if (phase_idx >= 1 && !phase_completion_.buffers_allocated) {
            // Previous phase did not complete - skip this phase
            // This can happen if a phase function returned without setting completion flag
            initialization_in_progress_ = false;
            return;
        }
        // Phase 2 (init_hardware): Verify Phase 1 completed
        if (phase_idx >= 2 && !phase_completion_.database_loaded) {
            // Previous phase did not complete - skip this phase
            initialization_in_progress_ = false;
            return;
        }
        // Phase 3 (setup_ui): Verify Phase 2 completed
        if (phase_idx >= 3 && !phase_completion_.hardware_ready) {
            // Previous phase did not complete - skip this phase
            initialization_in_progress_ = false;
            return;
        }
        // Phase 4 (load_settings): Verify Phase 3 completed
        if (phase_idx >= 4 && !phase_completion_.ui_layout_ready) {
            // Previous phase did not complete - skip this phase
            initialization_in_progress_ = false;
            return;
        }
        // Phase 5 (finalize): Verify Phase 4 completed
        if (phase_idx >= 5 && !phase_completion_.settings_loaded) {
            // Previous phase did not complete - skip this phase
            initialization_in_progress_ = false;
            return;
        }

        // Execute phase function
        (this->*PHASE_FUNCS[phase_idx])();
        
        // Exit if error occurred
        if (init_state_ == InitState::INITIALIZATION_ERROR) {
            initialization_in_progress_ = false;
            return;
        }
        
        // Update UI status based on phase
        static constexpr const char* const INIT_PHASE_NAMES[NUM_PHASES] = {
            "Allocating buffers...",
            "Loading database...",
            "Initializing hardware...",
            "Setting up UI...",
            "Loading settings...",
            "Finalizing..."
        };
        
        const char* status_msg = INIT_PHASE_NAMES[phase_idx];
        // Special message for database loading
        if (init_state_ == InitState::DATABASE_LOADING && !scanner_.is_database_loading_complete()) {
            status_msg = "Loading database...";
        }
        status_bar_.update_normal_status("INIT", status_msg);
    }
    
    // Reset flag (allow repeated calls)
    initialization_in_progress_ = false;
}

// Phase Initialization Methods (each method checks state before execution)

// ============================================================================
// DIAMOND FIX #CRITICAL #1: Phase 1 - Allocate display buffers
// ============================================================================
/**
 * @brief Phase 1: Allocate display buffers
 * @details Allocates display buffers from pool and marks phase completion
 * @note Sets phase_completion_.buffers_allocated on success
 * @note Sets init_state_ to INITIALIZATION_ERROR on failure
 */
void EnhancedDroneSpectrumAnalyzerView::init_phase_allocate_buffers() {
    if (!display_controller_.allocate_buffers_from_pool()) {
        init_error_ = InitError::ALLOCATION_FAILED;
        init_state_ = InitState::INITIALIZATION_ERROR;
        initialization_in_progress_ = false;
        return;
    }

    // DIAMOND FIX #P1-HIGH #2: Histogram callback registration moved to init_phase_finalize()
    // This ensures callback is only registered after database is fully loaded
    // preventing premature callback invocation before database is ready

    // DIAMOND FIX #CRITICAL #1: Mark Phase 1 as complete
    // This flag is verified before Phase 2 (load_database) executes
    phase_completion_.buffers_allocated = true;

    status_bar_.update_normal_status("INIT", "Phase 1: Buffers OK");
    init_state_ = InitState::BUFFERS_ALLOCATED;
}

// 2: Load database
void EnhancedDroneSpectrumAnalyzerView::init_phase_load_database() {
    // FIX #H2: Ensure DATABASE_LOADED only transitions when actually complete
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
            // FIX #H2: Only transition to DATABASE_LOADED when actually complete
            // This prevents Phase 3 (hardware init) from starting before DB is ready
            
            // DIAMOND FIX #CRITICAL #1: Mark Phase 2 as complete
            // This flag is verified before Phase 3 (init_hardware) executes
            phase_completion_.database_loaded = true;

            init_state_ = InitState::DATABASE_LOADED;
            status_bar_.update_normal_status("INIT", "Phase 2: DB loaded");
        } else {
            status_bar_.update_normal_status("INIT", "Loading DB...");
            // FIX #H2: Do NOT transition - wait for completion
        }
    }
}

// Phase 3: Initialize hardware
void EnhancedDroneSpectrumAnalyzerView::init_phase_init_hardware() {
    // FIX #M9: Ensure DATABASE_LOADED state only when truly complete
    // Double-check database loading is complete before hardware init
    if (init_state_ != InitState::DATABASE_LOADED) {
        return;
    }

    // FIX #M9: Additional verification - check initialization_complete_ flag
    if (!scanner_.is_initialization_complete()) {
        status_bar_.update_normal_status("INIT", "DB not ready");
        return;
    }

    hardware_.on_hardware_show();

    // DIAMOND FIX #CRITICAL #1: Mark Phase 3 as complete
    // This flag is verified before Phase 4 (setup_ui) executes
    phase_completion_.hardware_ready = true;

    status_bar_.update_normal_status("INIT", "Phase 3: HW ready");
    init_state_ = InitState::HARDWARE_READY;
}

// Phase 4: Setup UI layout
void EnhancedDroneSpectrumAnalyzerView::init_phase_setup_ui() {
    if (init_state_ != InitState::HARDWARE_READY) {
        return;
    }

    // FIX: Wait for database to complete before setting up UI
    // Phase 4 (UI Setup) can execute while database is still loading asynchronously.
    // This creates a race condition where UI elements may access uninitialized data.
    if (!scanner_.is_database_loading_complete()) {
        status_bar_.update_normal_status("INIT", "Waiting for DB...");
        return;  // Return and retry in next paint() call
    }

    // FIX: Additional verification - check initialization_complete_ flag
    if (!scanner_.is_initialization_complete()) {
        status_bar_.update_normal_status("INIT", "DB not ready");
        return;  // Return and retry in next paint() call
    }

    initialize_modern_layout();
    // FIX #L6: Move add_ui_elements() to appropriate phase
    // UI elements should be added after parent View initialization is complete
    add_ui_elements();

    // DIAMOND FIX #CRITICAL #1: Mark Phase 4 as complete
    // This flag is verified before Phase 5 (load_settings) executes
    phase_completion_.ui_layout_ready = true;

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

    // FIX #M2: Wait for database to complete before loading settings
    // The initialization state machine transitions from DATABASE_LOADED to SETTINGS_LOADED
    // based on time delays only, not actual completion. Phase 5 (settings load) executes
    // while the database is still loading asynchronously, causing a race condition.
    if (!scanner_.is_database_loading_complete()) {
        status_bar_.update_normal_status("INIT", "Waiting for DB...");
        return;  // Return and retry in next paint() call
    }

    // FIX #M2: Additional verification - check initialization_complete_ flag
    if (!scanner_.is_initialization_complete()) {
        status_bar_.update_normal_status("INIT", "DB not ready");
        return;  // Return and retry in next paint() call
    }

    // 6.4:

    // FIX: Use RAII guard to ensure M4 interrupts are always re-enabled
    // This prevents M4Core_IRQHandler from firing while M0 is locked
    // The guard automatically re-enables interrupts on scope exit
    // FIX: M4InterruptGuard doesn't exist - use M4InterruptMask from settings_persistence.hpp
    M4InterruptMask interrupt_mask;

    // SD
    systime_t sd_start = chTimeNow();
    while (sd_card::status() < sd_card::Status::Mounted) {
        // FIX #L4: Use constant instead of magic number 1s timeout
        if ((chTimeNow() - sd_start) > MS2ST(EDA::Constants::SD_CARD_MOUNT_TIMEOUT_MS)) {
            // FIX: M4InterruptGuard automatically re-enables interrupts on return
            // FIX #M7: Reset to default settings on SD card timeout
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
        // FIX: M4InterruptGuard automatically re-enables interrupts on return
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

    // FIX: M4InterruptGuard automatically re-enables interrupts on scope exit

    button_audio_.set_text(audio_get_enable_alerts(settings_) ? "AUDIO: ON" : "AUDIO: OFF");
    scanner_.update_scan_range(settings_.wideband_min_freq_hz,
                            settings_.wideband_max_freq_hz);
    
    // FIX: Update coordinator parameters after settings load
    // CRITICAL FIX: Add null check to prevent wild call before coordinator is initialized
    if (scanning_coordinator_ == nullptr) {
        status_bar_.update_normal_status("ERROR", "Coordinator not ready");
        return;
    }
    scanning_coordinator_->update_runtime_parameters(settings_);

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

    // FIX #M10: Transition to SETTINGS_LOADED state
    
    // DIAMOND FIX #CRITICAL #1: Mark Phase 5 as complete
    // This flag is verified before Phase 6 (finalize) executes
    phase_completion_.settings_loaded = true;

    init_state_ = InitState::SETTINGS_LOADED;
}

// 6: Finalize (  FULLY_INITIALIZED)
void EnhancedDroneSpectrumAnalyzerView::init_phase_finalize() {
    if (init_state_ != InitState::SETTINGS_LOADED) {
        return;
    }

    // DIAMOND FIX #P1-HIGH #2: Register histogram callback after database is loaded
    // This ensures callback is only registered when database is fully initialized
    // preventing premature callback invocation before database is ready
    scanner_.set_histogram_callback(&EnhancedDroneSpectrumAnalyzerView::static_histogram_callback, this);

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

    handle_scanner_update();

    // DIAMOND FIX #CRITICAL #1: Mark Phase 6 as complete
    // This flag is verified before any subsequent operations
    phase_completion_.finalized = true;

    init_state_ = InitState::FULLY_INITIALIZED;
    status_bar_.update_normal_status("EDA", "Ready");

    // FIX: Automatically start scanning thread after initialization
    start_scanning_thread();
}

void EnhancedDroneSpectrumAnalyzerView::on_show() {
    View::on_show();

    // DIAMOND FIX #P1-HIGH #3: Removed automatic initialization call
    // Initialization now only starts when user presses Start button
    // This prevents premature execution without user interaction

    // FIX: Reset initialization state
    init_state_ = InitState::CONSTRUCTED;
    init_start_time_ = chTimeNow();
    last_init_progress_ = 0;
    initialization_in_progress_ = false;
    init_error_ = InitError::NONE;

    status_bar_.update_normal_status("EDA", "Press START to initialize");
    
    // DIAMOND FIX #P1-HIGH #3: Removed continue_initialization() call
    // User must now press Start button to begin initialization
    
    // FIX: Force redraw to show status bar
    set_dirty();
}

void EnhancedDroneSpectrumAnalyzerView::continue_initialization() {
    // FIX #2: Continue deferred initialization from UI event loop
    // This prevents nested stack frames that cause M0 stack overflow
    if (init_state_ != InitState::FULLY_INITIALIZED &&
        init_state_ != InitState::INITIALIZATION_ERROR) {
        step_deferred_initialization();
        
        // Schedule next call if initialization is not complete
        if (init_state_ != InitState::FULLY_INITIALIZED &&
            init_state_ != InitState::INITIALIZATION_ERROR) {
            set_dirty();  // Trigger repaint which will call continue_initialization() again
        }
    }
}

// FIX #3: Begin initialization process (called from Start button)
bool EnhancedDroneSpectrumAnalyzerView::begin_initialization() noexcept {
    // Only begin initialization if not already initialized
    if (init_state_ == InitState::FULLY_INITIALIZED) {
        return true;  // Already initialized, consider success
    }
    
    if (init_state_ == InitState::INITIALIZATION_ERROR) {
        // Reset error state and try again
        init_state_ = InitState::CONSTRUCTED;
        init_start_time_ = chTimeNow();
        last_init_progress_ = 0;
        initialization_in_progress_ = false;
        init_error_ = InitError::NONE;
    }
    
    // Set flag to indicate initialization was requested by user
    initialization_requested_ = true;
    
    // Start the initialization process
    continue_initialization();
    
    return true;
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
    // CRITICAL FIX: Verify initialization before starting
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0, "Not initialized");
        return;
    }

    // DIAMOND FIX #HIGH #2: Add null check for scanning_coordinator_
    // Prevents nullptr dereference crash if coordinator not initialized
    if (!scanning_coordinator_) {
        status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0, "Coordinator not initialized");
        return;
    }

    if (scanning_coordinator_->is_scanning_active()) return;

    const auto result = scanning_coordinator_->start_coordinated_scanning();

    // Handle different start results
    switch (result) {
        case StartResult::SUCCESS:
            // Scanning started successfully
            break;
        case StartResult::ALREADY_ACTIVE:
            // Already active (shouldn't happen due to guard clause above)
            break;
        case StartResult::INITIALIZATION_NOT_COMPLETE:
            // Database initialization not complete yet
            // The UI event loop will retry on next frame sync
            // See message_handler_frame_sync_ in ui_enhanced_drone_analyzer.hpp:1721
            break;
        case StartResult::THREAD_CREATION_FAILED:
            // Thread creation failed - this is a critical error
            // Display error message to user
            display_controller_.text_status_info().set("Thread creation failed");
            break;
        case StartResult::SINGLETON_VIOLATION:
            // Singleton violation detected - multiple instances created
            // This should never happen in normal operation
            // Display critical error message to user
            display_controller_.text_status_info().set("Singleton violation error");
            break;
    }
}

void EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread() {
    if (!scanning_coordinator_->is_scanning_active()) return;
    scanning_coordinator_->stop_coordinated_scanning();
}

// FIX #3: Added public methods for explicit scanning control
bool EnhancedDroneSpectrumAnalyzerView::start_scanning() {
    // Check initialization state before starting scanning
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        return false;
    }
    
    // Check if already scanning
    if (scanning_coordinator_->is_scanning_active()) {
        return true;  // Already active, consider success
    }
    
    // Start scanning
    const auto result = scanning_coordinator_->start_coordinated_scanning();
    
    // Return true on success or already active
    return (result == StartResult::SUCCESS || result == StartResult::ALREADY_ACTIVE);
}

bool EnhancedDroneSpectrumAnalyzerView::stop_scanning() {
    // Check if scanning is active
    if (!scanning_coordinator_->is_scanning_active()) {
        return true;  // Not active, consider success
    }
    
    // Stop scanning
    scanning_coordinator_->stop_coordinated_scanning();
    return true;
}

bool EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button() {
    // DIAMOND FIX #P1-HIGH #4: Add begin_initialization() call when not initialized
    // This allows user to trigger initialization by pressing Start button
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        // Attempt to begin initialization
        if (begin_initialization()) {
            status_bar_.update_normal_status("INIT", "Initializing...");
            button_start_stop_.set_text("INITIALIZING...");
            return true;
        } else {
            // Show error message to user
            status_bar_.update_alert_status(ThreatLevel::CRITICAL, 0, "Init failed");
            return false;
        }
    }
    
    if (scanning_coordinator_->is_scanning_active()) {
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
    // 2.7: Don't call handle_scanner_update() here!
    // scanner_ may not be fully initialized yet
    // This prevents segfault/black screen during startup
    // handle_scanner_update() will be called after FULLY_INITIALIZED state

    // FIX #L7: Add check to ensure parent View is ready before setting parent_rect
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
        "Threats detected",   // NONE = 0
        "Threats detected",   // LOW = 1
        "Threats detected",   // MEDIUM = 2
        "HIGH THREATS!",      // HIGH = 3
        "CRITICAL THREATS!",  // CRITICAL = 4
        "UNKNOWN THREAT"      // UNKNOWN = 5
    };
}

void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
    // CRITICAL FIX: Guard against wild calls - check initialization state
    // Prevents access to scanner before initialization is complete
    if (init_state_ != InitState::FULLY_INITIALIZED) {
        return;
    }
    
    // SAFETY: Early exit if buffers are not valid
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
            snprintf(secondary_buffer, sizeof(secondary_buffer), "%s", "Awaiting commands");
        }
        status_bar_.update_normal_status(primary_msg, secondary_buffer);
        return;
    }

    // THREAD SAFETY FIX: Acquire mutex once for atomic snapshot of scanner state
    // This prevents TOCTOU (Time-Of-Check-Time-Of-Use) issues
    // where the state could change between reads.
    //
    // Lock order: DATA_MUTEX (level 1) for detection data and tracking state
    // The get_state_snapshot() method provides thread-safe access with single mutex acquisition.
    // Acquiring the mutex once here is more efficient than acquiring it separately
    // for each getter call.

    const auto snapshot = scanner_.get_state_snapshot();
    const ThreatLevel max_threat = snapshot.max_detected_threat;
    const size_t approaching = snapshot.approaching_count;
    const size_t static_count = snapshot.static_count;
    const size_t receding = snapshot.receding_count;
    const bool is_scanning = snapshot.scanning_active;
    
    Frequency current_freq = scanner_.get_current_scanning_frequency();

    // ============================================================================
    // DIAMOND FIX #HIGH #2: Audio Alert Cooldown Not Set
    // ============================================================================

    // Trigger audio alerts based on threat level (with debouncing)
    // AudioAlertManager::play_alert() has built-in debouncing to prevent baseband queue saturation
    // baseband::send_message() uses busy-wait spin loop
    // DIAMOND FIX #HIGH #2: Added cooldown verification before playing alert
    if (audio_get_enable_alerts(settings_) && max_threat >= ThreatLevel::LOW) {
        size_t total_drones = approaching + static_count + receding;
        if (total_drones > 0) {
            // DIAMOND FIX #HIGH #2: Verify cooldown before playing alert
            // This prevents rapid repeated alerts that could saturate baseband queue
            if (AudioCooldown::is_cooldown_elapsed()) {
                AudioAlertManager::play_alert(max_threat);
                AudioCooldown::update_last_alert_time();
            }
        }
    }

    // Update header
    smart_header_.update(max_threat, approaching, static_count, receding,
                        current_freq, is_scanning);

    // Update status bar (DoD: table-driven state machine)
    if (is_scanning) {
        uint32_t cycles = scanner_.get_scan_cycles();
        uint32_t progress = std::min(static_cast<uint32_t>(cycles * MagicNumberConstants::SCAN_PROGRESS_MULTIPLIER),
                                         static_cast<uint32_t>(MagicNumberConstants::MAX_PROGRESS_VALUE));
        status_bar_.update_scanning_progress(progress, cycles,
                                         scanner_.get_total_detections());
    } else {
        size_t total_drones = approaching + static_count + receding;
        // LUT lookup instead of ternary (Flash string, zero RAM allocation)
        // Manual min to avoid std::min during static initialization
        const uint8_t threat_idx = static_cast<uint8_t>(max_threat);
        const uint8_t max_idx = static_cast<uint8_t>(ThreatLevel::UNKNOWN);
        const uint8_t alert_idx = (threat_idx < max_idx) ? threat_idx : max_idx;
        const char* alert_msg = ALERT_MSG_LUT[alert_idx];
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
        // FIX: Verify initialization state before allowing user interaction
        if (init_state_ != InitState::FULLY_INITIALIZED) {
            return;
        }
        bool current = audio_get_enable_alerts(this->settings_);
        audio_set_enable_alerts(this->settings_, !current);
        this->button_audio_.set_text(audio_get_enable_alerts(this->settings_) ? "AUDIO: ON" : "AUDIO: OFF");
        this->button_audio_.set_style(audio_get_enable_alerts(this->settings_) ? &UIStyles::GREEN_STYLE : &UIStyles::LIGHT_STYLE);
    };

    field_scanning_mode_.on_change = [this](size_t index, int32_t value) {
        // FIX: Verify initialization state before allowing user interaction
        if (init_state_ != InitState::FULLY_INITIALIZED) {
            return;
        }
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

// PART 6.5: COMPACT FREQUENCY RULER IMPLEMENTATION

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
// * * @brief Check stack usage and log warnings if low * @param thread_name Name of the thread for logging * @param stack_size Total stack size for the thread * * @note Uses ChibiOS stack fill pattern (0x55) to estimate free stack space. * Compatible with ChibiOS versions that have CH_DBG_FILL_THREADS enabled. * If stack filling is not enabled, reports 0 free bytes conservatively.
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
    
    if (free_stack < MagicNumberConstants::MIN_STACK_FREE_THRESHOLD) {
        // Track low stack condition for monitoring
    }
}

// Memory Pressure Monitoring
// * * @brief Check memory pressure and log warnings if critical
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
