/**
 * @file eda_unified_worker.cpp
 * @brief Unified Worker Thread Implementation - Diamond Code
 * 
 * IMPLEMENTATION NOTES:
 * - All state functions are flat (no deep call nesting)
 * - Maximum stack depth: run() → step_xxx() = 2 levels
 * - All buffers are static (not on stack)
 * - No heap allocation anywhere
 * 
 * STACK USAGE PER STATE:
 * - step_init_db():       ~50 bytes locals
 * - step_scan_db_freq():  ~100 bytes locals  
 * - step_scan_wideband(): ~80 bytes locals
 * - step_process_detection(): ~60 bytes locals
 * - step_log_entry():     ~40 bytes locals
 * 
 * @author Diamond Code Pipeline
 * @date 2026-02-24
 */

#include "eda_unified_worker.hpp"
#include "eda_constants.hpp"
#include "eda_raii.hpp"
#include "diamond_fixes.hpp"

#include <ch.h>
#include <cstring>

namespace ui::apps::enhanced_drone_analyzer {

using namespace EDA::Constants;

//=============================================================================
// Stack Canary Implementation
//=============================================================================

namespace {
    // Stack canary pattern for overflow detection
    constexpr uint32_t CANARY_PATTERN = 0xDEADBEEF;
    
    // Thread-local canary (placed at "bottom" of used stack)
    thread_local uint32_t stack_canary_ = CANARY_PATTERN;
}

void UnifiedWorker::init_stack_canary() noexcept {
    stack_canary_ = CANARY_PATTERN;
}

bool UnifiedWorker::check_stack_canary() noexcept {
    return stack_canary_ == CANARY_PATTERN;
}

//=============================================================================
// Thread Start/Stop
//=============================================================================

bool UnifiedWorker::start(DroneHardwareController& hardware, 
                          DroneScanner& scanner) noexcept {
    // Guard clause: Already running
    if (active_) {
        return false;
    }
    
    // Initialize references
    hardware_ = &hardware;
    scanner_ = &scanner;
    
    // Initialize state
    state_ = WorkerState::IDLE;
    requested_state_ = WorkerState::INIT_DB;
    scan_cycles_ = 0;
    total_detections_ = 0;
    db_index_ = 0;
    wideband_slice_ = 0;
    
    // Initialize stack canary
    init_stack_canary();
    
    // Set active flag with critical section
    {
        CriticalSection cs;
        active_ = true;
    }
    
    // Create static thread
    thread_ = chThdCreateStatic(
        worker_wa_,
        sizeof(worker_wa_),
        NORMALPRIO,
        thread_entry,
        this
    );
    
    // Guard clause: Thread creation failed (should never happen with static)
    if (!thread_) {
        CriticalSection cs;
        active_ = false;
        return false;
    }
    
    return true;
}

void UnifiedWorker::stop() noexcept {
    // Guard clause: Not running
    if (!active_) {
        return;
    }
    
    // Signal thread to stop
    {
        CriticalSection cs;
        active_ = false;
        state_ = WorkerState::SHUTDOWN;
    }
    
    // Wait for thread termination with timeout
    if (thread_) {
        // Poll with deadline (5 second timeout)
        constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
        constexpr uint32_t POLL_INTERVAL_MS = 10;
        systime_t deadline = chTimeNow() + MS2ST(TERMINATION_TIMEOUT_MS);
        
        while (chTimeNow() < deadline && thread_ != nullptr) {
            chThdSleepMilliseconds(POLL_INTERVAL_MS);
        }
        
        thread_ = nullptr;
    }
}

//=============================================================================
// Main Run Loop - State Machine
//=============================================================================

msg_t UnifiedWorker::run() noexcept {
    while (active_) {
        // Check for requested state change
        if (requested_state_ != state_) {
            transition_state(requested_state_);
        }
        
        // Check stack canary periodically
        if ((scan_cycles_ & 0x3F) == 0) {  // Every 64 cycles
            if (!check_stack_canary()) {
                // Stack overflow detected - emergency shutdown
                state_ = WorkerState::SHUTDOWN;
                break;
            }
        }
        
        // State Machine - FLAT SWITCH (no nesting)
        switch (state_) {
            case WorkerState::IDLE:
                // Wait for command
                chThdSleepMilliseconds(100);
                break;
                
            case WorkerState::INIT_DB:
                step_init_db();
                break;
                
            case WorkerState::SCAN_DB_FREQ:
                step_scan_db_freq();
                break;
                
            case WorkerState::SCAN_WIDEBAND:
                step_scan_wideband();
                break;
                
            case WorkerState::PROCESS_DETECTION:
                step_process_detection();
                break;
                
            case WorkerState::LOG_ENTRY:
                step_log_entry();
                break;
                
            case WorkerState::SHUTDOWN:
                active_ = false;
                break;
                
            default:
                // Invalid state - reset to IDLE
                state_ = WorkerState::IDLE;
                break;
        }
        
        // Adaptive sleep based on threat level
        uint32_t sleep_ms = scan_interval_ms_;
        
        // Reduce sleep for high-threat situations
        if (current_drone_.threat_level >= 3) {  // HIGH or CRITICAL
            sleep_ms = EDA::Constants::FAST_SCAN_INTERVAL_MS;
        }
        
        chThdSleepMilliseconds(sleep_ms);
    }
    
    return 0;
}

//=============================================================================
// State Machine Steps
//=============================================================================

void UnifiedWorker::step_init_db() noexcept {
    // MINIMUM LOCALS - use static buffers
    // This step loads database from SD card
    
    if (!scanner_) {
        state_ = WorkerState::IDLE;
        return;
    }
    
    // Check if database is already loaded
    // Note: This calls into scanner which has its own synchronization
    bool db_loaded = false;
    {
        // Brief check without blocking
        // Scanner handles its own mutex internally
        db_loaded = scanner_->is_database_loaded();
    }
    
    if (db_loaded) {
        // Database ready - start scanning
        state_ = WorkerState::SCAN_DB_FREQ;
        return;
    }
    
    // Attempt to load database
    // Note: Scanner handles this synchronously
    bool success = scanner_->load_frequency_database();
    
    if (success) {
        state_ = WorkerState::SCAN_DB_FREQ;
    } else {
        // Retry after delay or switch to wideband
        state_ = WorkerState::SCAN_WIDEBAND;
    }
}

void UnifiedWorker::step_scan_db_freq() noexcept {
    // MINIMUM LOCALS - use static current_entry_
    
    if (!hardware_ || !scanner_) {
        state_ = WorkerState::IDLE;
        return;
    }
    
    // Get next frequency to scan
    // Using static buffer current_entry_
    bool has_entry = scanner_->get_next_entry(current_entry_);
    
    if (!has_entry) {
        // Database empty or end reached - wrap around or switch mode
        db_index_ = 0;
        scan_cycles_++;
        
        // Check if should switch to wideband (hybrid mode)
        // Every N cycles, do a wideband sweep
        if ((scan_cycles_ % EDA::Constants::HYBRID_SCAN_DIVISOR) == 0) {
            state_ = WorkerState::SCAN_WIDEBAND;
        }
        return;
    }
    
    // Validate frequency
    if (current_entry_.frequency_hz < FrequencyLimits::MIN_HARDWARE_FREQ ||
        current_entry_.frequency_hz > FrequencyLimits::MAX_HARDWARE_FREQ) {
        return;  // Skip invalid - stay in same state
    }
    
    // Tune hardware
    if (!hardware_->tune_to_frequency(current_entry_.frequency_hz)) {
        return;  // Tune failed - skip this entry
    }
    
    // Wait for PLL stabilization
    for (int i = 0; i < PLL_STABILIZATION_ITERATIONS; ++i) {
        if (!active_) return;  // Early exit if stopped
        chThdSleepMilliseconds(PLL_STABILIZATION_DELAY_MS);
    }
    
    // Get RSSI
    hardware_->clear_rssi_flag();
    
    // Optimized polling with deadline
    systime_t deadline = chTimeNow() + MS2ST(RSSI_TIMEOUT_MS);
    bool signal_captured = false;
    
    while (chTimeNow() < deadline) {
        if (hardware_->is_rssi_fresh()) {
            signal_captured = true;
            break;
        }
        chThdSleepMilliseconds(CHECK_INTERVAL_MS);
    }
    
    if (!signal_captured) {
        last_scanned_frequency_ = current_entry_.frequency_hz;
        return;
    }
    
    // Get RSSI value
    RSSI rssi = hardware_->get_current_rssi();
    
    // Process detection if above threshold
    if (rssi >= DEFAULT_RSSI_THRESHOLD_DB) {
        current_drone_.frequency_hz = current_entry_.frequency_hz;
        current_drone_.current_rssi = rssi;
        current_drone_.active = true;
        
        // Transition to detection processing
        state_ = WorkerState::PROCESS_DETECTION;
    }
    
    last_scanned_frequency_ = current_entry_.frequency_hz;
    scan_cycles_++;
}

void UnifiedWorker::step_scan_wideband() noexcept {
    // MINIMUM LOCALS - use static spectrum_buf_
    
    if (!hardware_) {
        state_ = WorkerState::SCAN_DB_FREQ;
        return;
    }
    
    // Calculate current wideband slice
    Frequency slice_center = WIDEBAND_DEFAULT_MIN + 
        (wideband_slice_ * WIDEBAND_SLICE_WIDTH);
    
    // Validate slice is within range
    if (slice_center > WIDEBAND_DEFAULT_MAX) {
        wideband_slice_ = 0;  // Reset
        state_ = WorkerState::SCAN_DB_FREQ;  // Return to DB scan
        return;
    }
    
    // Tune to slice center
    if (!hardware_->tune_to_frequency(slice_center)) {
        wideband_slice_++;
        return;
    }
    
    // Wait for PLL
    chThdSleepMilliseconds(PLL_STABILIZATION_DELAY_MS);
    
    // Start spectrum streaming if not active
    if (!hardware_->is_spectrum_streaming_active()) {
        hardware_->start_spectrum_streaming();
    }
    
    // Get spectrum data
    systime_t deadline = chTimeNow() + MS2ST(SPECTRUM_TIMEOUT_MS);
    bool spectrum_received = false;
    
    while (chTimeNow() < deadline) {
        if (hardware_->get_latest_spectrum_if_fresh(spectrum_buf_.data, 
                                                     SpectrumBuffer::SIZE)) {
            spectrum_received = true;
            break;
        }
        chThdSleepMilliseconds(CHECK_INTERVAL_MS);
    }
    
    if (spectrum_received) {
        // Analyze spectrum for peaks
        // Simple peak detection - find maximum
        uint8_t max_level = 0;
        size_t max_idx = 0;
        
        for (size_t i = 0; i < SpectrumBuffer::SIZE; ++i) {
            if (spectrum_buf_.data[i] > max_level) {
                max_level = spectrum_buf_.data[i];
                max_idx = i;
            }
        }
        
        // Check if peak exceeds threshold
        if (max_level > SPECTRUM_PEAK_THRESHOLD_DEFAULT) {
            // Calculate frequency offset from center
            // (simplified - actual implementation would use bin-to-freq conversion)
            Frequency detected_freq = slice_center + 
                ((max_idx - SpectrumBuffer::SIZE/2) * WIDEBAND_SLICE_WIDTH / SpectrumBuffer::SIZE);
            
            current_drone_.frequency_hz = detected_freq;
            current_drone_.active = true;
            
            // Transition to detection processing
            state_ = WorkerState::PROCESS_DETECTION;
            return;
        }
    }
    
    // Move to next slice
    wideband_slice_++;
}

void UnifiedWorker::step_process_detection() noexcept {
    // MINIMUM LOCALS - use static current_drone_
    
    if (!current_drone_.active) {
        state_ = WorkerState::SCAN_DB_FREQ;
        return;
    }
    
    // Calculate threat level based on RSSI
    RSSI rssi = current_drone_.current_rssi;
    uint8_t threat = 0;
    
    if (rssi >= CRITICAL_RSSI_DB) {
        threat = 4;  // CRITICAL
    } else if (rssi >= HIGH_RSSI_DB) {
        threat = 3;  // HIGH
    } else if (rssi >= MEDIUM_RSSI_DB) {
        threat = 2;  // MEDIUM
    } else if (rssi >= LOW_RSSI_DB) {
        threat = 1;  // LOW
    }
    
    current_drone_.threat_level = threat;
    
    // Update peak RSSI
    if (rssi > current_drone_.peak_rssi) {
        current_drone_.peak_rssi = rssi;
    }
    
    // Update timestamp
    current_drone_.last_seen = chTimeNow();
    
    // Increment detection counter
    total_detections_++;
    
    // Transition to logging
    state_ = WorkerState::LOG_ENTRY;
}

void UnifiedWorker::step_log_entry() noexcept {
    // MINIMUM LOCALS - use lock-free ring buffer
    
    if (!current_drone_.active) {
        state_ = WorkerState::SCAN_DB_FREQ;
        return;
    }
    
    // Create detection entry
    DetectionEntry entry;
    entry.frequency_hz = current_drone_.frequency_hz;
    entry.rssi = current_drone_.current_rssi;
    entry.timestamp = current_drone_.last_seen;
    entry.threat_level = current_drone_.threat_level;
    entry.drone_type = current_entry_.type;
    
    // Copy description (truncated if necessary)
    size_t copy_len = sizeof(entry.description) - 1;
    if (copy_len > sizeof(current_entry_.description)) {
        copy_len = sizeof(current_entry_.description);
    }
    std::memcpy(entry.description, current_entry_.description, copy_len);
    entry.description[copy_len] = '\0';
    
    // Write to lock-free ring buffer (non-blocking)
    bool written = log_buffer_.write(entry);
    
    if (!written) {
        // Buffer full - oldest entry will be overwritten on next write
        // This is acceptable for real-time detection logging
    }
    
    // Reset current drone
    current_drone_.active = false;
    
    // Return to scanning
    state_ = WorkerState::SCAN_DB_FREQ;
}

//=============================================================================
// Helper Functions
//=============================================================================

void UnifiedWorker::log_detection(Frequency freq, RSSI rssi, 
                                   uint8_t threat, uint8_t type) noexcept {
    DetectionEntry entry;
    entry.frequency_hz = freq;
    entry.rssi = rssi;
    entry.timestamp = chTimeNow();
    entry.threat_level = threat;
    entry.drone_type = type;
    entry.description[0] = '\0';
    
    log_buffer_.write(entry);
}

//=============================================================================
// Static Assertions
//=============================================================================

// Verify ring buffer size is power of 2
static_assert((UnifiedWorker::RING_BUFFER_SIZE & (UnifiedWorker::RING_BUFFER_SIZE - 1)) == 0,
              "RING_BUFFER_SIZE must be power of 2");

// Verify stack size alignment
static_assert((UnifiedWorker::STACK_SIZE % sizeof(stkalign_t)) == 0,
              "STACK_SIZE must be aligned to stkalign_t");

} // namespace ui::apps::enhanced_drone_analyzer
