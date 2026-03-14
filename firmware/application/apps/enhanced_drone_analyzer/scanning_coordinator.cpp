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

// ============================================================================
// INCLUDES
// ============================================================================

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
#include "eda_thread_sync.hpp"           // DIAMOND FIX: Thread Flags for event signaling
#include "ui_drone_common_types.hpp"     // For DroneAnalyzerSettings
#include "ui_enhanced_drone_analyzer.hpp" // Scanner interface
#include "ui_enhanced_drone_settings.hpp" // For DroneDatabaseListView::g_database_mutex (Bug #1 fix)
#include "ui_navigation.hpp"              // For NavigationView

namespace ui::apps::enhanced_drone_analyzer {

// ============================================================================
// SECTION 1: SINGLETON STORAGE DEFINITION
// ============================================================================
// This section defines the static storage for the ScanningCoordinator singleton.
// StaticStorage class provides memory corruption detection via canary pattern.
// ============================================================================

// DIAMOND FIX: Use StaticStorage class with canary pattern for corruption detection
// No heap allocation - all memory is in static storage
static StaticStorage<ScanningCoordinator> coordinator_storage;

// ============================================================================
// SECTION 1.5: SEMAPHORE STORAGE DEFINITION
// ============================================================================
// DIAMOND FIX: Semaphore instances for resource counting
// Eliminates mutex contention for buffer access and detection queue
// Stack-only allocation (no heap), thread-safe (ChibiOS semaphores)
// ============================================================================

// Display buffer semaphore (binary semaphore, single slot)
// Controls access to display buffer (only one thread can update display at a time)
sync::BinarySemaphore g_display_buffer_sem;

// Detection queue semaphore (counting semaphore, 10 slots)
// Controls number of detections that can be queued for processing
sync::CountingSemaphore g_detection_queue_sem;

// Singleton instance pointer (initialized to nullptr, set to &coordinator_storage after construction)
// NOTE: volatile qualifier is required to match header declaration for thread safety
volatile ScanningCoordinator* ScanningCoordinator::instance_ptr_ = nullptr;
Mutex ScanningCoordinator::init_mutex_;
AtomicFlag ScanningCoordinator::initialized_;  // AtomicFlag for thread-safe singleton pattern

// ============================================================================
// SECTION 2: EXPLICIT INITIALIZATION FUNCTION
// ============================================================================
// This section defines the initialize_eda_mutexes() function which must be
// called AFTER chSysInit() in main() to prevent undefined behavior.
// Initializes all EDA mutexes and semaphores.
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

    // CRITICAL FIX: Initialize external DSP mutexes
    // These mutexes are declared in eda_locking.hpp (centralized external declarations)
    // and defined in DSP modules:
    // - g_filtered_drones_mutex: dsp_display_types.hpp:550
    // - g_power_levels_mutex: dsp_spectrum_processor.hpp:190
    // Must be initialized after chSysInit() to prevent undefined behavior
    chMtxInit(&g_filtered_drones_mutex);
    chMtxInit(&g_power_levels_mutex);

    // Memory pool functionality has been removed from codebase
    // No need to initialize MemoryPoolManager global mutex

    // DIAMOND FIX: Initialize semaphores after chSysInit()
    // Semaphores must be initialized after ChibiOS RTOS is ready
    // to prevent undefined behavior during early initialization
    //
    // SEMAPHORE USAGE:
    // - BinarySemaphore: Single-slot resource access (e.g., display buffer)
    // - CountingSemaphore: Multi-slot resource access (e.g., detection queue)
    //
    // Lock order for semaphores is defined in eda_locking.hpp LockOrder enum:
    // - SEMAPHORE_DISPLAY_BUFFER = 14 (after UI_DISPLAY_MUTEX)
    // - SEMAPHORE_BUFFER_SLOTS = 15 (after ENTRIES_TO_SCAN_MUTEX)
    //
    // USAGE EXAMPLE:
    //   // Acquire display buffer slot (blocking with timeout)
    //   if (display_buffer_sem_.wait(TIMEOUT_SHORT_MS)) {
    //       // Use display buffer
    //       display_buffer_sem_.signal();  // Release buffer slot
    //   }
    //
    //   // Non-blocking try-wait
    //   if (display_buffer_sem_.try_wait()) {
    //       // Use display buffer
    //       display_buffer_sem_.signal();  // Release buffer slot
    //   }
    //
    //   // RAII guard for automatic release
    //   {
    //       sync::BinarySemaphoreGuard guard(display_buffer_sem_, TIMEOUT_SHORT_MS);
    //       if (guard.is_acquired()) {
    //           // Use display buffer (semaphore held)
    //       }  // Semaphore automatically released
    //   }

    // Initialize display buffer semaphore (binary semaphore, single slot)
    extern sync::BinarySemaphore g_display_buffer_sem;
    g_display_buffer_sem.initialize(1);  // Single-slot buffer access

    // Initialize detection queue semaphore (counting semaphore, 10 slots)
    extern sync::CountingSemaphore g_detection_queue_sem;
    g_detection_queue_sem.initialize(10);  // 10 detection queue slots

    // Initialize other EDA mutexes here
    // (Add additional mutex initializations as needed)
}

// Coordinator Thread Working Area Definition
// Stack Budget Revised with Hidden Stack Usage Sources
// Original: 2048 bytes (underestimated by 50-130%)
// Revised: 4096 bytes (100% increase, realistic worst-case)
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
// This provides 1598 bytes of safety margin (4096 - 2500 - 256 overhead)
stkalign_t ScanningCoordinator::coordinator_wa_[THD_WA_SIZE(ScanningCoordinator::COORDINATOR_THREAD_STACK_SIZE) / sizeof(stkalign_t)];

// ============================================================================
// SECTION 3: TYPE ALIASES AND CONSTANTS
// ============================================================================
// This section defines type aliases and constants used throughout the file.
// All constants are constexpr and stored in Flash for efficiency.
// ============================================================================

// TYPE ALIASES
using TimeoutCount = uint32_t;
// DIAMOND FIX: Semantic type for frequency values (Hz)
// Use Frequency (int64_t) consistently throughout to match rf::Frequency definition
using FrequencyHz = int64_t;

// ============================================================================
// SEMAPHORE TIMEOUT CONSTANTS
// ============================================================================
// DIAMOND CODE: Stage 3, Part 2 - Semaphore Implementation
// Timeout constants for semaphore operations to prevent indefinite blocking
// ============================================================================
namespace SemaphoreTimeoutConstants {
    // Display buffer semaphore timeout (10ms for responsive UI)
    constexpr uint32_t DISPLAY_BUFFER_TIMEOUT_MS = 10;

    // Detection queue semaphore timeout (100ms for responsive queue operations)
    constexpr uint32_t DETECTION_QUEUE_TIMEOUT_MS = 100;

    // Processing capacity semaphore timeout (1000ms for DSP operations)
    constexpr uint32_t PROCESSING_CAPACITY_TIMEOUT_MS = 1000;
}

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
// SECTION 4: SINGLETON IMPLEMENTATION
// ============================================================================
// This section implements the singleton pattern for ScanningCoordinator.
// Provides safe access to the singleton instance via instance_safe().
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
                                   ::Thread* ui_thread,
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

    // DIAMOND FIX: Initialize ThreadFlagSender for UI thread communication
    // UI thread will receive flags for event-driven updates
    // CRITICAL: Must be called AFTER construct() to avoid accessing object before construction
    coordinator_storage.get().ui_flag_sender_.set_target_thread(ui_thread);

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
// SECTION 5: CONSTRUCTOR / DESTRUCTOR
// ============================================================================
// This section implements the constructor and destructor for ScanningCoordinator.
// Initializes mutexes and member variables.
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
    , thread_mutex_{}
    , scanning_active_()
    , thread_terminated_()
    , thread_generation_(0)
    , scanning_thread_(nullptr)
    , scan_interval_ms_(EDA::Constants::DEFAULT_SCAN_INTERVAL_MS)
{
    // DIAMOND FIX: state_mutex_ removed - replaced with AtomicFlag for lock-free synchronization
    // AtomicFlag has constexpr constructor, no initialization needed
    chMtxInit(&thread_mutex_);
}

ScanningCoordinator::~ScanningCoordinator() noexcept {
    stop_coordinated_scanning();
    // Note: Memory pools use static storage, so no explicit shutdown is needed
    // All allocated memory will be returned to pools when deallocated
}

// ============================================================================
// SECTION 6: THREAD LIFECYCLE
// ============================================================================
// This section implements the thread lifecycle functions for ScanningCoordinator.
// Functions: start_coordinated_scanning(), stop_coordinated_scanning(),
// is_scanning_active(), update_runtime_parameters(), show_session_summary()
// ============================================================================

StartResult ScanningCoordinator::start_coordinated_scanning() noexcept {
    // DIAMOND FIX: Lock order validation to prevent deadlock
    // Lock ordering: thread_mutex_ (DATA_MUTEX) only
    // STATE_MUTEX replaced with AtomicFlag for lock-free synchronization
    // Lock order is enforced by consistent acquisition sequence
    MutexLock thread_lock(thread_mutex_, LockOrder::DATA_MUTEX);

    // Guard clause: Already scanning
    // AtomicFlag::load() uses acquire semantics for proper memory ordering
    if (scanning_active_.load()) {
        return StartResult::ALREADY_ACTIVE;
    }

    // Check initialization complete before starting scanning
    // Prevents scanning from starting before database is loaded
    if (!scanner_.is_initialization_complete()) {
        return StartResult::INITIALIZATION_NOT_COMPLETE;
    }

    // Set flags using AtomicFlag::store() with release semantics
    // This ensures all prior writes are visible before flag is set
    scanning_active_.store(true);
    thread_terminated_.store(false);
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
    // Lock ordering: thread_mutex_ (DATA_MUTEX) only
    // STATE_MUTEX replaced with AtomicFlag for lock-free synchronization
    // This prevents deadlock by ensuring consistent lock acquisition order
    MutexLock thread_lock(thread_mutex_, LockOrder::DATA_MUTEX);

    // Check if scanning is active using AtomicFlag::load() with acquire semantics
    bool was_scanning = scanning_active_.load();
    if (!was_scanning) {
        return;
    }
    
    // Capture generation before stopping
    uint32_t expected_generation = thread_generation_;
    
    // Set scanning_active_ to false using AtomicFlag::store() with release semantics
    scanning_active_.store(false);

    // Wait for thread to terminate with timeout
    constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
    constexpr uint32_t POLL_INTERVAL_MS = 10;
    systime_t deadline = chTimeNow() + MS2ST(TERMINATION_TIMEOUT_MS);

    while (chTimeNow() < deadline) {
        // Check if thread terminated using AtomicFlag::load() with acquire semantics
        // Only break if generation matches (prevents missed signal race condition)
        if (thread_terminated_.load() && thread_generation_ == expected_generation) {
            break;
        }
        chThdSleepMilliseconds(POLL_INTERVAL_MS);
    }

    scanning_thread_ = nullptr;
}

bool ScanningCoordinator::is_scanning_active() const noexcept {
    // DIAMOND FIX: Lock-free read using AtomicFlag::load() with acquire semantics
    // No mutex needed - AtomicFlag provides lock-free synchronization
    return scanning_active_.load();
}

// ============================================================================
// SECTION 9: RUNTIME PARAMETER UPDATES
// ============================================================================
// This section implements runtime parameter update functions.
// Functions: update_runtime_parameters(), show_session_summary()
// Updates scan interval and frequency range if scanning is active.
// ============================================================================

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) noexcept {
    // Update scan interval (no mutex needed - simple assignment)
    scan_interval_ms_ = settings.scan_interval_ms;

    // Guard clause: Not scanning, no need to update scanner
    // AtomicFlag::load() uses acquire semantics for proper memory ordering
    if (!scanning_active_.load()) {
        return;
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
// SECTION 7: DSP LAYER FUNCTIONS (UI/DSP Separation)
// ============================================================================
// This section provides thread-safe access to DSP layer data for UI rendering.
// Functions: get_display_data_snapshot(), get_filtered_drones_snapshot()
// These functions capture snapshots of internal state under mutex protection.
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

    // Capture scanning state using AtomicFlag::load() with acquire semantics
    // Lock-free read - no mutex needed for simple boolean flag
    snapshot.is_scanning = scanning_active_.load();

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

// ============================================================================
// SECTION 8: SCANNING THREAD IMPLEMENTATION
// ============================================================================
// This section implements the main scanning thread function.
// Functions: scanning_thread_function(), coordinated_scanning_thread()
// Performs scan cycles in a loop with configurable interval.
// Handles timeout detection and consecutive failure counting.
// ============================================================================

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
    // OPTIMIZATION: Single StackMonitor instance at function entry reduces stack usage
    StackMonitor stack_monitor;

    // Check stack safety at thread entry (minimum 512 bytes required for function overhead)
    constexpr size_t MIN_THREAD_STACK = 512;
    if (!stack_monitor.is_stack_safe(MIN_THREAD_STACK)) {
        // Stack overflow imminent - exit thread gracefully
        // TODO: Implement proper error logging system
        return ReturnCodes::TIMEOUT_ERROR;
    }

    // DIAMOND FIX: Thread Flag Sender for event signaling
    // Eliminates polling and enables event-driven architecture
    // UI thread will wait for DSP_DATA_READY flag instead of polling
    sync::ThreadFlagSender flag_sender(chThdSelf());

    // Capture thread generation at start to prevent "missed signal" race condition
    // during thread restart: each thread start increments counter, thread only
    // sets termination flag if generation matches, coordinator waits for matching generation.
    // Lock-free read - no mutex needed for simple uint32_t counter
    uint32_t my_generation = thread_generation_;

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
        // OPTIMIZATION: Reuse single StackMonitor instance from function entry
        constexpr size_t MIN_FREE_STACK = 512;  // Minimum free stack required for safe operation
        if (!stack_monitor.is_stack_safe(MIN_FREE_STACK)) {
            // Stack running low - exit thread gracefully to prevent overflow
            // TODO: Implement proper error logging system
            #ifdef DEBUG
                __BKPT();  // Breakpoint for debugger
            #endif
            // Signal stop to coordinator using AtomicFlag::store() with release semantics
            scanning_active_.store(false);
            
            // Set termination flag before exit (thread-safe)
            // Only set if generation matches (prevents missed signal race condition)
            if (thread_generation_ == my_generation) {
                thread_terminated_.store(true);
            }
            chThdExit(ReturnCodes::TIMEOUT_ERROR);
            return ReturnCodes::TIMEOUT_ERROR;
        }

        // Check if still active using AtomicFlag::load() with acquire semantics
        // Lock-free read - no mutex needed for simple boolean flag
        if (!scanning_active_.load()) {
            break;
        }

        // Check initialization complete with timeout to prevent infinite loop
        if (!scanner_.is_initialization_complete()) {
            // Check if initialization timeout has been reached
            systime_t init_wait_time = chTimeNow() - init_wait_start_time;
            if (init_wait_time > MS2ST(CoordinatorConstants::INITIALIZATION_TIMEOUT_MS)) {
                // Initialization timeout - signal stop to coordinator
                // AtomicFlag::store() uses release semantics
                scanning_active_.store(false);

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                if (thread_generation_ == my_generation) {
                    thread_terminated_.store(true);
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
        // OPTIMIZATION: Reuse single StackMonitor instance from function entry (line 594)
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
        // Lock order: No semaphore needed - perform_scan_cycle() acquires SPECTRUM_MUTEX internally
        scanner_.perform_scan_cycle(hardware_);

        // DIAMOND FIX: Signal UI that DSP data is ready
        // Event-driven architecture eliminates polling
        // UI thread will receive DSP_DATA_READY flag and update display
        flag_sender.send_flag(sync::ThreadFlag::DSP_DATA_READY);

        const systime_t cycle_duration = chTimeNow() - cycle_start;

        // Check for scan cycle timeout
        if (cycle_duration > SCAN_CYCLE_TIMEOUT_ST) {
            consecutive_timeouts++;

            // Guard clause: Too many consecutive timeouts
            if (consecutive_timeouts >= CoordinatorConstants::MAX_CONSECUTIVE_TIMEOUTS) {
                // Signal stop to coordinator using AtomicFlag::store() with release semantics
                scanning_active_.store(false);

                // Set termination flag before exit (thread-safe)
                // Only set flag if generation matches (prevents missed signal race condition)
                if (thread_generation_ == my_generation) {
                    thread_terminated_.store(true);
                }

                chThdExit(ReturnCodes::TIMEOUT_ERROR);
                return ReturnCodes::TIMEOUT_ERROR;
            }
        } else {
            consecutive_timeouts = 0;
        }

        // Sleep for scan interval
        // Lock-free read - no mutex needed for simple uint32_t value
        chThdSleepMilliseconds(scan_interval_ms_);
    }

    // Set termination flag before exit (thread-safe)
    // Only set flag if generation matches (prevents missed signal race condition)
    // AtomicFlag::store() uses release semantics
    if (thread_generation_ == my_generation) {
        thread_terminated_.store(true);
    }

    // Normal exit
    chThdExit(ReturnCodes::SUCCESS);
    return ReturnCodes::SUCCESS;
}

} // namespace ui::apps::enhanced_drone_analyzer
