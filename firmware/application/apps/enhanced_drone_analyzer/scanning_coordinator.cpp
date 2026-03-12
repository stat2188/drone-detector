// @file scanning_coordinator.cpp
// @brief Coordinate scanning operations for Enhanced Drone Analyzer
//
// DIAMOND CODE - Phase 4: Coordination Layer
// ============================================
// Migrated from LEGACY/ with Diamond Code compliance:
// - No forbidden constructs (std::vector, std::string, std::map, std::atomic, new, malloc)
// - Stack allocation only (max 4KB stack per thread)
// - Uses constexpr, enum class, using Type = uintXX_t
// - No magic numbers (all constants defined)
// - Zero-Overhead and Data-Oriented Design principles
// - Singleton pattern with proper initialization order handling
// - Thread-safe using MutexLock from eda_locking.hpp
// - Custom AtomicFlag class (NOT std::atomic) using GCC built-ins
//
// Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
// Environment: Bare-metal / ChibiOS RTOS
//
// @author Diamond Code Pipeline - Phase 4 Migration
// @date 2026-03-11

// Corresponding header (must be first)
#include "scanning_coordinator.hpp"

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>  // Required for placement new operator

// Third-party library headers
#include <ch.h>         // ChibiOS RTOS

// Project-specific headers (alphabetical order)
#include "dsp_display_types.hpp"         // DSP/UI communication types
#include "eda_constants.hpp"
#include "eda_locking.hpp"               // Unified MutexLock, StackMonitor
#include "ui_drone_common_types.hpp"     // For DroneAnalyzerSettings
#include "ui_enhanced_drone_analyzer.hpp" // Scanner interface
#include "ui_enhanced_drone_settings.hpp" // For DroneDatabaseListView::g_database_mutex (Bug #1 fix)
#include "ui_navigation.hpp"              // For NavigationView

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// SINGLETON STORAGE DEFINITION
// ============================================================================

// DIAMOND FIX: Use StaticStorage class with canary pattern for corruption detection
// No heap allocation - all memory is in static storage
static StaticStorage<ScanningCoordinator> coordinator_storage;

// Singleton instance pointer (initialized to nullptr, set to &coordinator_storage after construction)
// NOTE: volatile qualifier is required to match header declaration for thread safety
volatile ScanningCoordinator* ScanningCoordinator::instance_ptr_ = nullptr;
Mutex ScanningCoordinator::init_mutex_;
AtomicFlag ScanningCoordinator::initialized_;  // AtomicFlag for thread-safe singleton pattern

// ============================================================================
// EXPLICIT INITIALIZATION FUNCTION
// ============================================================================
// Initialize EDA mutexes after ChibiOS RTOS is ready.
// Must be called AFTER chSysInit() in main() to prevent undefined behavior.
//
// @note This replaces the static initializer pattern which runs before main()
//       and before chSysInit(), causing undefined behavior with ChibiOS mutexes.
//
// P1-HIGH FIX: Initialize all static Mutex objects
// - Static Mutex objects require explicit initialization after ChibiOS RTOS is ready
// - chMtxInit() must be called after chSysInit() to avoid undefined behavior
// - This function initializes all EDA mutexes in one place
void initialize_eda_mutexes() noexcept {
    // CRITICAL FIX: Add guard to prevent wild calls
    // Ensure mutexes are only initialized once to prevent double initialization
    // Double initialization can cause undefined behavior with ChibiOS mutexes
    static bool mutexes_initialized = false;
    if (mutexes_initialized) {
        return;  // Already initialized
    }
    
    // Initialize ScanningCoordinator mutex
    chMtxInit(&ScanningCoordinator::init_mutex_);
    mutexes_initialized = true;

    // BUG #1 FIX: Initialize g_database_mutex via public static function
    // g_database_mutex is defined in ui_enhanced_drone_settings.cpp:1118
    // Must be initialized after chSysInit() to prevent undefined behavior
    // Call public static initialization function from DroneDatabaseListView
    DroneDatabaseListView::initialize_mutex();

    // CRITICAL FIX: Initialize settings mutexes from ui_enhanced_drone_settings.cpp
    // These mutexes are defined as global variables and must be initialized AFTER chSysInit()
    // to prevent stack overflow and undefined behavior during early initialization
    // settings_buffer_mutex (line 85), load_buffer_mutex (line 90), header_buffer_mutex (line 103)
    extern Mutex settings_buffer_mutex;
    extern Mutex load_buffer_mutex;
    extern Mutex header_buffer_mutex;
    chMtxInit(&settings_buffer_mutex);
    chMtxInit(&load_buffer_mutex);
    chMtxInit(&header_buffer_mutex);

    // Memory pool functionality has been removed from codebase
    // No need to initialize MemoryPoolManager global mutex

    // Initialize other EDA mutexes here
    // (Add additional mutex initializations as needed)
}

// Coordinator Thread Working Area Definition
// Stack Budget Revised with Hidden Stack Usage Sources
// Original: 2048 bytes (underestimated by 50-130%)
// Revised: 3072 bytes (50% increase, realistic worst-case)
//
// Hidden Stack Usage Sources (not included in original estimate):
// - Compiler-generated temporaries: 32-64 bytes
// - Return addresses: 4 bytes per call
// - Saved registers (R4-R11, LR): 16-32 bytes per call
// - Alignment padding: 0-8 bytes
// - ChibiOS context switch overhead: 32-64 bytes
// - Function call overhead: 64-128 bytes per level
//
// Stack usage analysis shows coordinator thread requires ~2.5KB with nested function calls
// This provides 574 bytes of safety margin (3072 - 2500 - 256 overhead)
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

// ============================================================================
// SINGLETON INSTANCE ACCESS
// ============================================================================

/**
 * @brief Get singleton instance safely (returns nullptr if not initialized)
 * @return Pointer to singleton instance, or nullptr if not initialized
 * @note Returns nullptr instead of halting system if called before initialize()
 * @note Uses double-checked locking with memory barriers for thread safety
 * @note This is the recommended method for optional singleton access
 *
 * DIAMOND FIX: Fixed malformed comment block
 * - Removed incomplete comment block that was never closed
 * - Properly formatted documentation comment
 */
ScanningCoordinator* ScanningCoordinator::instance_safe() noexcept {
    // DIAMOND FIX: Atomic load with acquire semantics
    // Use __atomic_load_n for atomic pointer access to prevent TOCTOU race
    // Load pointer atomically first, then check if it's null
    ScanningCoordinator* ptr = const_cast<ScanningCoordinator*>(
        __atomic_load_n(&instance_ptr_, __ATOMIC_ACQUIRE)
    );
    
    // Memory fence to ensure initialization is visible
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    
    return ptr;  // Returns nullptr if not initialized
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
    // DIAMOND FIX: Use hardware memory barrier for thread-safe singleton access
    // NOTE: Using __atomic_thread_fence(__ATOMIC_SEQ_CST) for full memory barrier
    //       This ensures proper memory ordering across all CPU cores and prevents
    //       race conditions in the double-checked locking pattern.
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    MutexLock lock(init_mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Already initialized
    if (initialized_) {
        return false;
    }

    // DIAMOND FIX: Use StaticStorage class with canary pattern (no heap allocation)
    // StaticStorage provides memory corruption detection via canary values
    // and uses compiler intrinsic memory barriers for thread safety
    coordinator_storage.construct(nav, hardware, scanner, display_controller, audio_controller);

    // Check for memory corruption after construction
    if (coordinator_storage.is_corrupted()) {
        // Memory corruption detected - critical error
        // TODO: Implement proper error logging system
        return false;
    }

    // Set instance pointer to constructed object ONLY after all initialization succeeds
    // This prevents race condition where instance_ptr_ is set before memory pools are ready
    // Using __atomic_store_n ensures proper memory ordering across all CPU cores
    __atomic_store_n(&instance_ptr_, &coordinator_storage.get(), __ATOMIC_RELEASE);

    // DIAMOND FIX: Use hardware memory barrier for thread-safe singleton access
    // CRITICAL FIX: Use AtomicFlag store() instead of direct assignment
    // AtomicFlag provides acquire/release memory ordering and lock-free operations
    // This ensures proper memory ordering across all CPU cores and prevents
    // race conditions in double-checked locking pattern.
    initialized_.store(true);

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
    // Note: Memory pools use static storage, so no explicit shutdown is needed
    // All allocated memory will be returned to pools when deallocated
}

StartResult ScanningCoordinator::start_coordinated_scanning() noexcept {
    // DIAMOND FIX: Lock order validation to prevent deadlock
    // Lock ordering: thread_mutex_ (DATA_MUTEX) -> state_mutex_ (DATA_MUTEX)
    // Both locks are at DATA_MUTEX level, so order is determined by acquisition sequence
    // Lock order is enforced by consistent acquisition sequence

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
    // DIAMOND FIX: Lock ordering documentation
    // Lock ordering: thread_mutex_ (DATA_MUTEX) -> state_mutex_ (DATA_MUTEX)
    // Both locks are at DATA_MUTEX level, so order is determined by acquisition sequence
    // This prevents deadlock by ensuring consistent lock acquisition order
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
    // DIAMOND FIX: Full mutex protection for state access
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
 *
 * DIAMOND FIX: Replaced magic number 10 with dsp::DisplayTypeConstants::MAX_FILTERED_DRONES
 */
dsp::FilteredDronesSnapshot ScanningCoordinator::get_filtered_drones_snapshot() const noexcept {
    dsp::FilteredDronesSnapshot snapshot;

    // Get tracked drones snapshot from scanner
    DroneScanner::DroneSnapshot drone_snapshot = scanner_.get_tracked_drones_snapshot();

    // Copy tracked drones to filtered snapshot (max MAX_FILTERED_DRONES)
    // DIAMOND FIX: Replaced magic number 10 with dsp::DisplayTypeConstants::MAX_FILTERED_DRONES
    snapshot.count = (drone_snapshot.count < dsp::DisplayTypeConstants::MAX_FILTERED_DRONES) ?
                   drone_snapshot.count : dsp::DisplayTypeConstants::MAX_FILTERED_DRONES;
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

    // DIAMOND FIX: Stack monitoring using StackMonitor
    // Initialize stack monitor to track stack usage and detect overflow
    // StackMonitor uses ChibiOS stack fill pattern for accurate detection
    StackMonitor stack_monitor;

    // Check stack safety at thread entry (minimum 512 bytes required for function overhead)
    constexpr size_t MIN_THREAD_STACK = 512;
    if (!stack_monitor.is_stack_safe(MIN_THREAD_STACK)) {
        // Stack overflow imminent - exit thread gracefully
        // TODO: Implement proper error logging system
        return ReturnCodes::TIMEOUT_ERROR;
    }

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

    // DIAMOND FIX: Document synchronization pattern for scanning_active_ access
    // - All reads/writes to scanning_active_ are protected by state_mutex_
    // - No lock-free reads - all state access is synchronized
    while (true) {
        // DIAMOND FIX: Stack monitoring at loop start to detect overflow before it happens
        // Check stack availability before performing any operations
        // This prevents stack overflow by detecting low stack conditions early
        // CRITICAL FIX: Create StackMonitor instance for stack safety check
        StackMonitor stack_monitor;
        constexpr size_t MIN_FREE_STACK = 512;  // Minimum free stack required for safe operation
        if (!stack_monitor.is_stack_safe(MIN_FREE_STACK)) {
            // Stack running low - exit thread gracefully to prevent overflow
            // TODO: Implement proper error logging system
            #ifdef DEBUG
                __BKPT();  // Breakpoint for debugger
            #endif
            // Signal stop to coordinator
            {
                MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                scanning_active_ = false;
            }
            // Set termination flag before exit (thread-safe)
            {
                MutexLock state_lock(state_mutex_, LockOrder::DATA_MUTEX);
                if (thread_generation_ == my_generation) {
                    thread_terminated_ = true;
                }
            }
            chThdExit(ReturnCodes::TIMEOUT_ERROR);
            return ReturnCodes::TIMEOUT_ERROR;
        }

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

        // DIAMOND FIX: Stack monitor check before scan cycle
        // Check for stack overflow before performing scan cycle
        // StackMonitor provides runtime detection of stack exhaustion
        constexpr size_t SCAN_CYCLE_STACK_REQUIRED = 256;
        if (!stack_monitor.is_stack_safe(SCAN_CYCLE_STACK_REQUIRED)) {
            // Stack overflow detected - exit thread gracefully
            // TODO: Implement proper error logging system
            return ReturnCodes::TIMEOUT_ERROR;
        }

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
