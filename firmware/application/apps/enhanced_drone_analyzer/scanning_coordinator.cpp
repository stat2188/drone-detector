// * @file scanning_coordinator.cpp * @brief Coordinate scanning operations for Enhanced Drone Analyzer
// * DIAMOND CODE PRINCIPLES:
// * - Zero heap allocation: All memory is stack-allocated or in Flash
// * - No exceptions: All functions are noexcept
// * - Type-safe: Uses semantic type aliases
// * - Memory-safe: Uses ChibiOS RTOS for thread management
// * @author Diamond Code Pipeline
// * @date 2026-02-27

// Corresponding header (must be first)
#include "scanning_coordinator.hpp"

// C++ standard library headers (alphabetical order)
#include <cstdint>
#include <cstring>
#include <new>  // Required for placement new operator

// Third-party library headers
#include <ch.h>         // ChibiOS RTOS
#include "chthreads.h"  // ChibiOS threading functions
#include "chmtx.h"      // ChibiOS mutex functions

// Project-specific headers (alphabetical order)
#include "dsp_display_types.hpp"         // DSP/UI communication types
#include "eda_locking.hpp"               // Unified MutexLock
#include "radio.hpp"                     // For rf::Frequency type
#include "ui_drone_common_types.hpp"     // For DroneAnalyzerSettings
#include "ui_enhanced_drone_analyzer.hpp" // Scanner interface
#include "ui_navigation.hpp"              // For NavigationView

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// SINGLETON STORAGE DEFINITION
// ============================================================================

// FIX #1 & #2: Static storage with canary pattern for corruption detection
// DIAMOND FIX: Use StaticStorage class with canary values and memory barriers
// No heap allocation - all memory is in static storage
static StaticStorage<ScanningCoordinator> coordinator_storage;

// Singleton instance pointer (initialized to nullptr, set to &coordinator_storage after construction)
// NOTE: volatile qualifier is required to match header declaration for thread safety
volatile ScanningCoordinator* ScanningCoordinator::instance_ptr_ = nullptr;
Mutex ScanningCoordinator::init_mutex_;
volatile bool ScanningCoordinator::initialized_ = false;  // volatile for thread-safe singleton pattern
volatile bool ScanningCoordinator::instance_constructed_ = false;  // tracks if instance was constructed

// ============================================================================
// RED TEAM FIX #CRITICAL FLAW #5: Explicit initialization function
// ============================================================================
// Initialize EDA mutexes after ChibiOS RTOS is ready.
// Must be called AFTER chSysInit() in main() to prevent undefined behavior.
//
// @note This replaces the static initializer pattern which runs before main()
//       and before chSysInit(), causing undefined behavior with ChibiOS mutexes.
void initialize_eda_mutexes() noexcept {
    // Initialize ScanningCoordinator mutex
    chMtxInit(&ScanningCoordinator::init_mutex_);
    
    // Initialize other EDA mutexes here
    // (Add additional mutex initializations as needed)
}

// Coordinator Thread Working Area Definition
// FIX #SO-1: Increased from 1536 to 2048 bytes
stkalign_t ScanningCoordinator::coordinator_wa_[THD_WA_SIZE(ScanningCoordinator::COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

// TYPE ALIASES
using TimeoutCount = uint32_t;
// DIAMOND FIX: Semantic type for frequency values (Hz)
// Use Frequency (int64_t) consistently throughout to match rf::Frequency definition
using FrequencyHz = int64_t;

// CONSTANTS
namespace CoordinatorConstants {
    // Scan cycle timeout in milliseconds
    constexpr uint32_t SCAN_CYCLE_TIMEOUT_MS = 10000;

    // Maximum consecutive timeouts before aborting
    constexpr TimeoutCount MAX_CONSECUTIVE_TIMEOUTS = 3;

    // Maximum consecutive scanner failures before aborting
    constexpr TimeoutCount MAX_CONSECUTIVE_SCANNER_FAILURES = 5;

    // Initialization timeout: maximum time to wait for initialization to complete (30 seconds)
    constexpr uint32_t INITIALIZATION_TIMEOUT_MS = 30000;
}

// RETURN CODES
namespace ReturnCodes {
    constexpr msg_t SUCCESS = 0;
    constexpr msg_t TIMEOUT_ERROR = -1;
    constexpr msg_t SCANNER_ERROR = -2;
    constexpr msg_t INITIALIZATION_TIMEOUT = -3;
}

// ============================================================================
// SINGLETON IMPLEMENTATION
// ============================================================================

/**
 * @brief Get singleton instance
 * @return Reference to singleton instance
 * @note Must call initialize() before using instance
 * @note CRITICAL: This method will halt the system if called before initialize()
 */
ScanningCoordinator& ScanningCoordinator::instance() noexcept {
    // RED TEAM FIX #CRITICAL FLAW #1: Use hardware memory barrier for thread-safe singleton access
    // NOTE: Using __atomic_thread_fence(__ATOMIC_SEQ_CST) for full memory barrier
    //       This ensures proper memory ordering across all CPU cores and prevents
    //       race conditions in the double-checked locking pattern.
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    // CRITICAL: Instance must be initialized before use
    // Dereferencing a null pointer causes undefined behavior (system crash)
    if (!instance_ptr_) {
        // Log critical error before halting
        // TODO: Implement proper error logging system
        // For now, trigger breakpoint for debugging (in debug builds)
        // and infinite loop as last resort (in release builds)
        #ifdef DEBUG
            __BKPT();  // Breakpoint for debugger
        #endif
        while (true) {
            // Infinite loop to halt execution
            // System watchdog will trigger reset if configured
        }
    }

    // FIX #1 & #2: Check for memory corruption using canary pattern
    if (coordinator_storage.is_corrupted()) {
        // Memory corruption detected - critical error
        // TODO: Implement proper error logging system
        #ifdef DEBUG
            __BKPT();  // Breakpoint for debugger
        #endif
        while (true) {
            // Infinite loop to halt execution
            // System watchdog will trigger reset if configured
        }
    }

    // Cast volatile pointer to non-volatile for return
    // SAFETY: This is safe because:
    // 1. The pointer is only modified during initialization (protected by mutex and memory barriers)
    // 2. After initialization, the pointer itself is not modified - only read
    // 3. The object it points to is not volatile - only the pointer is volatile for thread-safe publication
    // 4. The memory barrier before reading ensures visibility of the pointer value
    return *const_cast<ScanningCoordinator*>(instance_ptr_);
}

// ============================================================================
// DIAMOND FIX #CRITICAL #2: Safe instance access with cooperative termination
// ============================================================================
/**
 * @brief Get singleton instance safely (returns nullptr if not initialized)
 * @return Pointer to singleton instance, or nullptr if not initialized
 * @note Returns nullptr instead of halting system if called before initialize()
 * @note Uses double-checked locking with memory barriers for thread safety
 * @note This is the recommended method for optional singleton access
 */
ScanningCoordinator* ScanningCoordinator::instance_safe() noexcept {
    // Double-checked locking pattern with memory barriers
    // First check without lock (fast path for already-initialized case)
    if (initialized_ && instance_ptr_) {
        // Memory barrier ensures visibility of initialized flag and pointer
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        return const_cast<ScanningCoordinator*>(instance_ptr_);
    }

    // Slow path: acquire mutex and check again
    MutexLock lock(init_mutex_, LockOrder::DATA_MUTEX);
    
    // Memory barrier after acquiring lock ensures proper memory ordering
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    
    // Second check under lock protection
    if (!initialized_ || !instance_ptr_) {
        // Cooperative termination: return nullptr instead of halting system
        // This allows caller to handle the error gracefully
        return nullptr;
    }

    // Memory barrier before returning ensures visibility of pointer
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    
    // Return pointer (not reference) to allow nullptr return
    return const_cast<ScanningCoordinator*>(instance_ptr_);
}


/**
 * @brief Initialize singleton instance
 * @param nav Navigation view reference
 * @param hardware Hardware controller reference
 * @param scanner Scanner reference
 * @param display_controller Display controller reference
 * @param audio_controller Audio controller reference
 * @return true if initialization successful, false if already initialized
 * @note Can only be called once per program lifetime
 */
bool ScanningCoordinator::initialize(NavigationView& nav,
                                   DroneHardwareController& hardware,
                                   DroneScanner& scanner,
                                   DroneDisplayController& display_controller,
                                   AudioManager& audio_controller) noexcept {
    // RED TEAM FIX #CRITICAL FLAW #1: Use hardware memory barrier for thread-safe singleton access
    // NOTE: Using __atomic_thread_fence(__ATOMIC_SEQ_CST) for full memory barrier
    //       This ensures proper memory ordering across all CPU cores and prevents
    //       race conditions in the double-checked locking pattern.
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    MutexLock lock(init_mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Already initialized
    if (initialized_) {
        return false;
    }

    // FIX #1 & #2: Use StaticStorage class with canary pattern (no heap allocation)
    // StaticStorage provides memory corruption detection via canary values
    // and uses compiler intrinsic memory barriers for thread safety
    coordinator_storage.construct(nav, hardware, scanner, display_controller, audio_controller);

    // Check for memory corruption after construction
    if (coordinator_storage.is_corrupted()) {
        // Memory corruption detected - critical error
        // TODO: Implement proper error logging system
        return false;
    }

    // Set instance pointer to constructed object
    instance_ptr_ = &coordinator_storage.get();
   instance_constructed_ = true;

    // RED TEAM FIX #CRITICAL FLAW #1: Use hardware memory barrier for thread-safe singleton access
    // NOTE: Using __atomic_thread_fence(__ATOMIC_SEQ_CST) for full memory barrier
    //       This ensures proper memory ordering across all CPU cores and prevents
    //       race conditions in the double-checked locking pattern.
    initialized_ = true;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    return true;
}

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
                                         DroneHardwareController& hardware,
                                         DroneScanner& scanner,
                                         DroneDisplayController& display_controller,
                                         AudioManager& audio_controller) noexcept
    : nav_(nav)
    , hardware_(hardware)
    , scanner_(scanner)
    , display_controller_(display_controller)
    , audio_controller_(audio_controller)
    , state_mutex_{}
    , thread_mutex_{}
    , scanning_active_(false)
    , thread_terminated_(false)
    , thread_generation_(0)
    , scanning_thread_(nullptr)
    , scan_interval_ms_(EDA::Constants::DEFAULT_SCAN_INTERVAL_MS)
{
    chMtxInit(&state_mutex_);
    chMtxInit(&thread_mutex_);
}

ScanningCoordinator::~ScanningCoordinator() noexcept {
    stop_coordinated_scanning();
}

StartResult ScanningCoordinator::start_coordinated_scanning() noexcept {
    MutexLock thread_lock(thread_mutex_, LockOrder::DATA_MUTEX);
    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Already scanning
    if (scanning_active_) {
        return StartResult::ALREADY_ACTIVE;
    }

    // Check initialization complete before starting scanning
    // Prevents scanning from starting before database is loaded
    if (!scanner_.is_initialization_complete()) {
        return StartResult::INITIALIZATION_NOT_COMPLETE;
    }

    scanning_active_ = true;
    thread_terminated_ = false;
    thread_generation_++;

    // Create static thread with stack-based working area
    // ChibiOS chThdCreateStatic() returns nullptr on failure
    scanning_thread_ = chThdCreateStatic(
        coordinator_wa_,
        sizeof(coordinator_wa_),
        NORMALPRIO,
        scanning_thread_function,
        this
    );

    // Guard clause: Thread creation failed
    if (!scanning_thread_) {
        scanning_active_ = false;
        thread_terminated_ = false;
        return StartResult::THREAD_CREATION_FAILED;
    }

    return StartResult::SUCCESS;
}

void ScanningCoordinator::stop_coordinated_scanning() noexcept {
    MutexLock thread_lock(thread_mutex_, LockOrder::DATA_MUTEX);

    bool was_scanning;
    uint32_t expected_generation;
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        was_scanning = scanning_active_;
        if (!was_scanning) {
            return;
        }
        expected_generation = thread_generation_;
        scanning_active_ = false;
    }

    // Wait for thread to terminate with timeout
    constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
    constexpr uint32_t POLL_INTERVAL_MS = 10;
    systime_t deadline = chTimeNow() + MS2ST(TERMINATION_TIMEOUT_MS);

    while (chTimeNow() < deadline) {
        {
            MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
            if (thread_terminated_ && thread_generation_ == expected_generation) {
                break;
            }
        }
        chThdSleepMilliseconds(POLL_INTERVAL_MS);
    }

    scanning_thread_ = nullptr;
}

bool ScanningCoordinator::is_scanning_active() const noexcept {
    // FIX #RC-1: Full mutex protection for state access
    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
    return scanning_active_;
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept {
    // Update scan interval
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        scan_interval_ms_ = settings.scan_interval_ms;
    }

    // Guard clause: Not scanning, no need to update scanner
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        if (!scanning_active_) {
            return;
        }
    }

    // Update scanner frequency range
    // DIAMOND FIX: Type-safe frequency comparison using consistent FrequencyHz (int64_t) type
    // Prevents overflow when converting from uint64_t settings to int64_t Frequency type
    constexpr uint64_t INT64_MAX_U64 = 9223372036854775807ULL;

    // Clamp frequency values to int64_t range if needed (prevent overflow)
    // Use FrequencyHz (int64_t) consistently to match rf::Frequency definition
    FrequencyHz min_freq = (settings.wideband_min_freq_hz > INT64_MAX_U64) ?
                          static_cast<FrequencyHz>(INT64_MAX_U64) :
                          static_cast<FrequencyHz>(settings.wideband_min_freq_hz);
    FrequencyHz max_freq = (settings.wideband_max_freq_hz > INT64_MAX_U64) ?
                          static_cast<FrequencyHz>(INT64_MAX_U64) :
                          static_cast<FrequencyHz>(settings.wideband_max_freq_hz);

    scanner_.update_scan_range(min_freq, max_freq);
}

void ScanningCoordinator::show_session_summary([[maybe_unused]] const char* summary) noexcept {
    // Guard clause: null pointer check for summary parameter
    if (!summary) {
        return;
    }
    // Placeholder for future implementation
    // DIAMOND OPTIMIZATION: Uses const char* instead of std::string (zero heap allocation)
}

// ============================================================================
// DSP LAYER FUNCTIONS (UI/DSP Separation)
// ============================================================================

/**
 * @brief Get display data snapshot from DSP layer
 * 
 * This function captures a snapshot of all display data from the DSP layer
 * for UI rendering. It includes scanning state, threat levels, drone counts,
 * and pre-calculated color indices.
 * 
 * Thread-safety: Acquires state_mutex_ before accessing data.
 * No UI widget calls are made in this function (pure DSP logic).
 * 
 * @return DisplayDataSnapshot containing current scanning state and statistics
 */
dsp::DisplayDataSnapshot ScanningCoordinator::get_display_data_snapshot() const noexcept {
    dsp::DisplayDataSnapshot snapshot;

    // Acquire mutex to access scanning state
    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);

    // Capture scanning state
    snapshot.is_scanning = scanning_active_;

    // Get current frequency from scanner
    snapshot.current_freq = scanner_.get_current_scanning_frequency();

    // Get total frequencies from database
    snapshot.total_freqs = scanner_.get_database_size();

    // Get threat level from scanner
    snapshot.max_threat = scanner_.get_max_detected_threat();

    // Get drone counts from scanner
    snapshot.approaching_count = scanner_.get_approaching_count();
    snapshot.receding_count = scanner_.get_receding_count();
    snapshot.static_count = scanner_.get_static_count();
    snapshot.total_detections = scanner_.get_total_detections();

    // Get scan cycles from scanner
    snapshot.scan_cycles = scanner_.get_scan_cycles();

    // Get real mode flag from scanner
    snapshot.is_real_mode = scanner_.is_real_mode();

    // Set has_detections flag
    snapshot.has_detections = (snapshot.total_detections > 0);

    // Pre-calculate color index based on threat level
    // Clamp to valid range (0-4) to prevent out-of-bounds array access
    // BIG_DISPLAY_STYLES has 5 elements (indices 0-4)
    uint8_t threat_value = static_cast<uint8_t>(snapshot.max_threat);
    snapshot.color_idx = (threat_value > 4) ? 4 : threat_value;

    return snapshot;
}

/**
 * @brief Get filtered drones snapshot from DSP layer
 * 
 * This function captures a snapshot of filtered drone data from the DSP layer
 * for UI rendering. It includes only active drones that have been seen recently.
 * 
 * Thread-safety: Acquires data_mutex_ before accessing drone data.
 * No UI widget calls are made in this function (pure DSP logic).
 * 
 * @return FilteredDronesSnapshot containing active drones for display
 */
dsp::FilteredDronesSnapshot ScanningCoordinator::get_filtered_drones_snapshot() const noexcept {
    dsp::FilteredDronesSnapshot snapshot;

    // Get tracked drones snapshot from scanner
    DroneScanner::DroneSnapshot drone_snapshot = scanner_.get_tracked_drones_snapshot();

    // Copy tracked drones to filtered snapshot (max 10)
    snapshot.count = (drone_snapshot.count < 10) ? drone_snapshot.count : 10;
    for (size_t i = 0; i < snapshot.count; ++i) {
        // Copy TrackedDrone to TrackedDroneData structure
        snapshot.drones[i].frequency = drone_snapshot.drones[i].frequency;
        snapshot.drones[i].drone_type = drone_snapshot.drones[i].drone_type;
        snapshot.drones[i].threat_level = drone_snapshot.drones[i].threat_level;
        snapshot.drones[i].rssi = drone_snapshot.drones[i].rssi;
        snapshot.drones[i].last_seen = drone_snapshot.drones[i].last_seen;
        snapshot.drones[i].trend = drone_snapshot.drones[i].get_trend();
    }

    return snapshot;
}

msg_t ScanningCoordinator::scanning_thread_function(void* arg) noexcept {
    auto coordinator = static_cast<ScanningCoordinator*>(arg);
    return coordinator->coordinated_scanning_thread();
}

msg_t ScanningCoordinator::coordinated_scanning_thread() noexcept {
    // No exceptions - perform scan cycle directly
    // Error handling is managed via return codes

    // Capture thread generation at start to prevent "missed signal" race condition
    // during thread restart: each thread start increments counter, thread only
    // sets termination flag if generation matches, coordinator waits for matching generation.
    uint32_t my_generation;
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        my_generation = thread_generation_;
    }

    // Convert timeout constant to ChibiOS systime_t
    constexpr systime_t SCAN_CYCLE_TIMEOUT_ST = MS2ST(CoordinatorConstants::SCAN_CYCLE_TIMEOUT_MS);

    // Counters for error detection
    TimeoutCount consecutive_timeouts = 0;

    // Track initialization wait time outside loop to ensure timeout is
    // calculated correctly across loop iterations
    systime_t init_wait_start_time = chTimeNow();

    // FIX #RC-1: Document synchronization pattern for scanning_active_ access
    // - All reads/writes to scanning_active_ are protected by state_mutex_
    // - No lock-free reads - all state access is synchronized
    while (true) {
        // Check if still active (with mutex protection)
        bool active;
        {
            MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
            active = scanning_active_;
        }
        if (!active) {
            break;
        }

        // Check initialization complete with timeout to prevent infinite loop
        if (!scanner_.is_initialization_complete()) {
            // Check if initialization timeout has been reached
            systime_t init_wait_time = chTimeNow() - init_wait_start_time;
            if (init_wait_time > MS2ST(CoordinatorConstants::INITIALIZATION_TIMEOUT_MS)) {
                // Initialization timeout - signal stop to coordinator
                {
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                    scanning_active_ = false;
                }

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                {
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                    if (thread_generation_ == my_generation) {
                        thread_terminated_ = true;
                    }
                }

                chThdExit(ReturnCodes::INITIALIZATION_TIMEOUT);
            }

            // Wait before retrying
            chThdSleepMilliseconds(EDA::Constants::SD_CARD_POLL_INTERVAL_MS);
            continue;
        }

        // Reset initialization wait timer when initialization completes
        // This ensures timeout is calculated correctly if initialization fails later
        init_wait_start_time = chTimeNow();

        const systime_t cycle_start = chTimeNow();

        // Perform scan cycle
        // Note: We don't check return value here as scanner handles its own errors
        // Future enhancement: Add scanner error handling if needed
        scanner_.perform_scan_cycle(hardware_);

        const systime_t cycle_duration = chTimeNow() - cycle_start;

        // Check for scan cycle timeout
        if (cycle_duration > SCAN_CYCLE_TIMEOUT_ST) {
            consecutive_timeouts++;

            // Guard clause: Too many consecutive timeouts
            if (consecutive_timeouts >= CoordinatorConstants::MAX_CONSECUTIVE_TIMEOUTS) {
                // Signal stop to coordinator
                {
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                    scanning_active_ = false;
                }

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                {
                    MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                    if (thread_generation_ == my_generation) {
                        thread_terminated_ = true;
                    }
                }

                chThdExit(ReturnCodes::TIMEOUT_ERROR);
                return ReturnCodes::TIMEOUT_ERROR;
            }
        } else {
            consecutive_timeouts = 0;
        }

        // Sleep for scan interval
        uint32_t interval_ms;
        {
            MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
            interval_ms = scan_interval_ms_;
        }
        chThdSleepMilliseconds(interval_ms);
    }

    // Set termination flag before exit (thread-safe)
    // Only set flag if generation matches (prevents missed signal race condition)
    {
        MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
        if (thread_generation_ == my_generation) {
            thread_terminated_ = true;
        }
    }

    // Normal exit
    chThdExit(ReturnCodes::SUCCESS);
    return ReturnCodes::SUCCESS;
}

} // namespace ui::apps::enhanced_drone_analyzer
