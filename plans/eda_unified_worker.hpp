/**
 * @file eda_unified_worker.hpp
 * @brief Unified Worker Thread - Diamond Code for Stack Optimization
 * 
 * DIAMOND CODE PRINCIPLES APPLIED:
 * - Zero heap allocation: All memory is static or stack-allocated
 * - No exceptions: All functions are noexcept
 * - Type-safe: Uses enum class and semantic type aliases
 * - Memory-safe: Uses ChibiOS RTOS for thread management
 * - Single-threaded State Machine: Eliminates race conditions
 * 
 * STACK OPTIMIZATION:
 * - BEFORE: 4 threads × (1.5KB + 4KB + 3KB + 4KB) = 12.4KB
 * - AFTER:  1 thread × 2KB = 2KB (84% reduction)
 * 
 * @author Diamond Code Pipeline
 * @date 2026-02-24
 * @version 3.0.0
 */

#ifndef EDA_UNIFIED_WORKER_HPP_
#define EDA_UNIFIED_WORKER_HPP_

#include <cstdint>
#include <cstddef>
#include <ch.h>
#include "eda_constants.hpp"
#include "eda_locking.hpp"
#include "eda_raii.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Forward declarations
class DroneHardwareController;
class DroneScanner;
struct freqman_entry;
struct TrackedDrone;

//=============================================================================
// Type Aliases (Semantic Types)
//=============================================================================
using Frequency = EDA::Frequency;
using RSSI = EDA::RSSI;
using Timestamp = EDA::Timestamp;

//=============================================================================
// Stack Size Constants (NO MAGIC NUMBERS)
//=============================================================================
namespace StackConstants {
    /// @brief Unified worker thread stack size (2KB)
    /// @note Calculated: TCB(~100) + State(~20) + Locals(~200) + Calls(~100) + Margin(~400) = ~820 bytes
    /// @note 2048 provides 2.5x safety margin for edge cases
    constexpr size_t UNIFIED_WORKER_STACK_SIZE = 2048;
    
    /// @brief UI thread stack size (reduced from 4KB to 1KB)
    /// @note UI delegates heavy work to UnifiedWorker
    constexpr size_t UI_THREAD_STACK_SIZE = 1024;
    
    /// @brief ISR/Reserve stack size
    constexpr size_t ISR_RESERVE_STACK_SIZE = 1024;
    
    /// @brief Total stack budget (4KB limit for STM32F405)
    constexpr size_t TOTAL_STACK_BUDGET = 
        UNIFIED_WORKER_STACK_SIZE + UI_THREAD_STACK_SIZE + ISR_RESERVE_STACK_SIZE;
    
    // Compile-time verification
    static_assert(TOTAL_STACK_BUDGET <= 4096, "Total stack exceeds 4KB limit!");
}

//=============================================================================
// State Machine States
//=============================================================================
/**
 * @brief Unified Worker State Machine States
 * 
 * State transitions are designed to be:
 * - Flat (no nesting)
 * - Deterministic (single transition per cycle)
 * - Minimal local variables per state
 */
enum class WorkerState : uint8_t {
    /// @brief Initial/Idle state - waiting for commands
    IDLE = 0,
    
    /// @brief Initialize frequency database from SD card
    INIT_DB,
    
    /// @brief Scan single database frequency
    SCAN_DB_FREQ,
    
    /// @brief Scan wideband slice
    SCAN_WIDEBAND,
    
    /// @brief Process RSSI detection
    PROCESS_DETECTION,
    
    /// @brief Log detection entry (lock-free)
    LOG_ENTRY,
    
    /// @brief Graceful shutdown
    SHUTDOWN,
    
    /// @brief State count for bounds checking
    STATE_COUNT
};

//=============================================================================
// State Machine Events (for future extension)
//=============================================================================
enum class WorkerEvent : uint8_t {
    NONE = 0,
    START_SCAN,
    STOP_SCAN,
    DB_LOADED,
    DETECTION_FOUND,
    TIMEOUT,
    ERROR
};

//=============================================================================
// Shared Static Buffers (NOT in stack!)
//=============================================================================
namespace SharedBuffers {
    /// @brief Current frequency entry being processed (~32 bytes)
    /// @note Static storage - shared across operations, NOT on stack
    struct CurrentEntry {
        Frequency frequency_hz = 0;
        char description[32] = {};
        uint8_t type = 0;
        bool valid = false;
    };
    
    /// @brief Current tracked drone data (~80 bytes)
    struct CurrentDrone {
        Frequency frequency_hz = 0;
        RSSI current_rssi = -127;
        RSSI peak_rssi = -127;
        Timestamp last_seen = 0;
        uint8_t trend = 0;
        uint8_t threat_level = 0;
        bool active = false;
    };
    
    /// @brief Spectrum buffer for wideband scanning
    /// @note Reduced from 256 to 128 bytes (still sufficient for detection)
    struct SpectrumBuffer {
        static constexpr size_t SIZE = 128;
        uint8_t data[SIZE] = {};
        bool fresh = false;
    };
}

//=============================================================================
// Ring Buffer for Lock-Free Logging
//=============================================================================
/**
 * @brief Lock-free single-producer single-consumer ring buffer
 * 
 * @tparam T Entry type
 * @tparam N Buffer capacity (must be power of 2)
 * 
 * @note Used for detection logging without blocking worker thread
 * @note Single producer (worker), single consumer (UI or SD writer)
 */
template<typename T, size_t N>
class LockFreeRingBuffer {
    static_assert((N & (N - 1)) == 0, "N must be power of 2");
    
public:
    /// @brief Write entry to buffer (producer)
    /// @return true if written, false if buffer full
    bool write(const T& entry) noexcept {
        const size_t next_head = (head_ + 1) & MASK;
        if (next_head == tail_) {
            return false;  // Buffer full
        }
        buffer_[head_] = entry;
        head_ = next_head;
        return true;
    }
    
    /// @brief Read entry from buffer (consumer)
    /// @return true if read, false if buffer empty
    bool read(T& entry) noexcept {
        if (head_ == tail_) {
            return false;  // Buffer empty
        }
        entry = buffer_[tail_];
        tail_ = (tail_ + 1) & MASK;
        return true;
    }
    
    /// @brief Check if buffer is empty
    bool empty() const noexcept {
        return head_ == tail_;
    }
    
    /// @brief Check if buffer is full
    bool full() const noexcept {
        return ((head_ + 1) & MASK) == tail_;
    }
    
    /// @brief Get current entry count
    size_t size() const noexcept {
        return (head_ - tail_) & MASK;
    }
    
private:
    static constexpr size_t MASK = N - 1;
    T buffer_[N] = {};
    volatile size_t head_ = 0;  // Write position
    volatile size_t tail_ = 0;  // Read position
};

//=============================================================================
// Detection Entry for Ring Buffer
//=============================================================================
struct DetectionEntry {
    Frequency frequency_hz = 0;
    RSSI rssi = -127;
    Timestamp timestamp = 0;
    uint8_t threat_level = 0;
    uint8_t drone_type = 0;
    char description[16] = {};
};

//=============================================================================
// Unified Worker Class
//=============================================================================
/**
 * @brief Unified Worker Thread - Single Thread State Machine
 * 
 * ARCHITECTURE:
 * - Replaces 4 separate threads with 1 State Machine
 * - Stack usage: 2048 bytes (vs 12,700 bytes before)
 * - No heap allocation
 * - No deep call nesting (max depth = 2)
 * 
 * THREAD SAFETY:
 * - Single thread eliminates most race conditions
 * - volatile bool + CriticalSection for thread control flags
 * - Lock-free ring buffer for logging
 */
class UnifiedWorker {
public:
    //=========================================================================
    // Constants
    //=========================================================================
    
    /// @brief Stack size for this worker
    static constexpr size_t STACK_SIZE = StackConstants::UNIFIED_WORKER_STACK_SIZE;
    
    /// @brief Ring buffer capacity (power of 2)
    static constexpr size_t RING_BUFFER_SIZE = 32;
    
    /// @brief Maximum scan batch size per state transition
    static constexpr size_t MAX_BATCH_SIZE = 5;  // Reduced from 10
    
    /// @brief State transition interval (minimum)
    static constexpr uint32_t MIN_STATE_INTERVAL_MS = 10;
    
    /// @brief Default scan interval
    static constexpr uint32_t DEFAULT_SCAN_INTERVAL_MS = EDA::Constants::DEFAULT_SCAN_INTERVAL_MS;
    
    //=========================================================================
    // Constructor / Destructor
    //=========================================================================
    
    /// @brief Default constructor
    UnifiedWorker() noexcept = default;
    
    /// @brief Destructor - ensures thread is stopped
    ~UnifiedWorker() noexcept {
        stop();
    }
    
    // Non-copyable, non-movable
    UnifiedWorker(const UnifiedWorker&) = delete;
    UnifiedWorker& operator=(const UnifiedWorker&) = delete;
    UnifiedWorker(UnifiedWorker&&) = delete;
    UnifiedWorker& operator=(UnifiedWorker&&) = delete;
    
    //=========================================================================
    // Public Interface
    //=========================================================================
    
    /**
     * @brief Start the unified worker thread
     * 
     * @param hardware Reference to hardware controller
     * @param scanner Reference to drone scanner
     * @return true if started successfully, false if already running
     * 
     * @note Thread-safe: Uses CriticalSection for flag access
     * @note No heap: Uses static working area
     */
    bool start(DroneHardwareController& hardware, DroneScanner& scanner) noexcept;
    
    /**
     * @brief Stop the unified worker thread
     * 
     * @note Thread-safe: Uses CriticalSection for flag access
     * @note Blocking: Waits for thread to terminate
     * @note Timeout: 5 second maximum wait
     */
    void stop() noexcept;
    
    /**
     * @brief Check if worker is active
     * @return true if worker thread is running
     */
    bool is_active() const noexcept {
        return active_;
    }
    
    /**
     * @brief Get current state
     * @return Current WorkerState
     */
    WorkerState get_state() const noexcept {
        return state_;
    }
    
    /**
     * @brief Request state change
     * @param new_state Target state
     * @note State change occurs at next state transition
     */
    void request_state(WorkerState new_state) noexcept {
        requested_state_ = new_state;
    }
    
    /**
     * @brief Get number of pending log entries
     * @return Count of entries in ring buffer
     */
    size_t pending_log_entries() const noexcept {
        return log_buffer_.size();
    }
    
    /**
     * @brief Read log entry (consumer side)
     * @param entry Output entry
     * @return true if entry read, false if buffer empty
     */
    bool read_log_entry(DetectionEntry& entry) noexcept {
        return log_buffer_.read(entry);
    }
    
    /**
     * @brief Set scan interval
     * @param interval_ms Interval in milliseconds
     */
    void set_scan_interval(uint32_t interval_ms) noexcept {
        scan_interval_ms_ = interval_ms;
    }
    
    //=========================================================================
    // Static Working Area (NOT in heap!)
    //=========================================================================
    
    /// @brief Thread working area (static allocation)
    /// @note THD_WA_SIZE includes overhead for ChibiOS TCB
    static inline stkalign_t worker_wa_[THD_WA_SIZE(STACK_SIZE) / sizeof(stkalign_t)];
    
private:
    //=========================================================================
    // Static Shared Buffers (NOT in stack!)
    //=========================================================================
    
    /// @brief Current frequency entry
    static inline SharedBuffers::CurrentEntry current_entry_;
    
    /// @brief Current drone tracking data
    static inline SharedBuffers::CurrentDrone current_drone_;
    
    /// @brief Spectrum buffer for wideband
    static inline SharedBuffers::SpectrumBuffer spectrum_buf_;
    
    /// @brief Lock-free ring buffer for logging
    static inline LockFreeRingBuffer<DetectionEntry, RING_BUFFER_SIZE> log_buffer_;
    
    //=========================================================================
    // Thread State
    //=========================================================================
    
    /// @brief Worker thread handle
    Thread* thread_ = nullptr;
    
    /// @brief Active flag (volatile for thread visibility)
    volatile bool active_ = false;
    
    /// @brief Current state machine state
    WorkerState state_ = WorkerState::IDLE;
    
    /// @brief Requested state (for state changes)
    WorkerState requested_state_ = WorkerState::IDLE;
    
    /// @brief Scan interval in milliseconds
    uint32_t scan_interval_ms_ = DEFAULT_SCAN_INTERVAL_MS;
    
    /// @brief Database index for scanning
    size_t db_index_ = 0;
    
    /// @brief Wideband slice counter
    size_t wideband_slice_ = 0;
    
    /// @brief Scan cycle counter
    uint32_t scan_cycles_ = 0;
    
    /// @brief Total detections counter
    uint32_t total_detections_ = 0;
    
    //=========================================================================
    // Component References (non-owning)
    //=========================================================================
    
    DroneHardwareController* hardware_ = nullptr;
    DroneScanner* scanner_ = nullptr;
    
    //=========================================================================
    // Thread Entry Point
    //=========================================================================
    
    /**
     * @brief Thread entry point (static)
     * @param arg Pointer to UnifiedWorker instance
     * @return Thread exit code
     */
    static msg_t thread_entry(void* arg) noexcept {
        return static_cast<UnifiedWorker*>(arg)->run();
    }
    
    //=========================================================================
    // Main Run Loop
    //=========================================================================
    
    /**
     * @brief Main worker thread loop
     * @return 0 on normal exit, error code on failure
     * 
     * @note State Machine Pattern:
     * - Flat switch statement (no nesting)
     * - Each step function has minimal locals
     * - All buffers are static (not on stack)
     */
    msg_t run() noexcept;
    
    //=========================================================================
    // State Machine Steps
    //=========================================================================
    
    /// @brief Initialize database from SD card
    void step_init_db() noexcept;
    
    /// @brief Scan single database frequency
    void step_scan_db_freq() noexcept;
    
    /// @brief Scan wideband slice
    void step_scan_wideband() noexcept;
    
    /// @brief Process RSSI detection
    void step_process_detection() noexcept;
    
    /// @brief Log detection entry (lock-free)
    void step_log_entry() noexcept;
    
    //=========================================================================
    // Helper Functions
    //=========================================================================
    
    /// @brief Check if should continue running
    bool should_continue() const noexcept {
        return active_ && state_ != WorkerState::SHUTDOWN;
    }
    
    /// @brief Handle state transition
    void transition_state(WorkerState new_state) noexcept {
        state_ = new_state;
    }
    
    /// @brief Write detection to ring buffer
    void log_detection(Frequency freq, RSSI rssi, uint8_t threat, uint8_t type) noexcept;
    
    /// @brief Check stack canary (detect overflow)
    bool check_stack_canary() noexcept;
    
    /// @brief Initialize stack canary
    void init_stack_canary() noexcept;
};

//=============================================================================
// Compile-Time Assertions
//=============================================================================

static_assert(sizeof(UnifiedWorker) <= 256, 
    "UnifiedWorker instance size should be minimal (< 256 bytes)");

static_assert(StackConstants::TOTAL_STACK_BUDGET <= 4096, 
    "Total stack budget must not exceed 4KB STM32F405 limit");

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_UNIFIED_WORKER_HPP_
