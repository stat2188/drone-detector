# STAGE 2: The Architect's Blueprint - Enhanced Drone Analyzer Diamond Code Refactoring

**Date:** 2026-03-02
**Based on:** Stage 1 Forensic Audit (24 critical defects)
**Target Architecture:** STM32F405 (ARM Cortex-M4, 128KB RAM)
**Principles:** Zero-Overhead, Data-Oriented Design, Diamond Code Standard

---

## Executive Summary

The Stage 2 Architect's Blueprint presents a comprehensive refactoring strategy for the Enhanced Drone Analyzer (EDA) module, addressing 24 defects identified in the forensic audit. The refactoring follows Diamond Code principles with zero-heap allocation, deterministic memory usage, and strict separation of concerns. Key architectural changes include eliminating heap allocations through static storage patterns, separating UI and DSP logic into distinct layers with thread-safe communication via snapshot objects, and implementing six critical fixes for singleton access, static storage protection, thread-local stack canaries, ring buffer bounds protection, unified lock ordering, and stack size unification. The UI/DSP separation uses a two-phase rendering pattern where UI threads fetch immutable snapshots from DSP threads without blocking, ensuring real-time responsiveness. Data structures use fixed-size arrays with compile-time bounds validation, and all functions follow noexcept specifications with RAII wrappers for resource management. The memory layout allocates static storage in the BSS segment (~16KB), constants in Flash, and validates stack usage per thread (<4KB). Expected outcomes include zero heap allocations, deterministic real-time performance, elimination of all identified error codes, improved testability through separation of concerns, and compliance with Diamond Code constraints. The implementation sequence prioritizes critical fixes first (P0), followed by core extraction (P1), then UI refactoring (P2), with comprehensive testing at each phase.

---

## UI/DSP Separation Strategy

### Separation Boundaries

The UI and DSP layers must be strictly separated with clear boundaries:

```
┌─────────────────────────────────────────────────────────────────┐
│                    UI LAYER (Presentation)                   │
│  - View classes only (rendering, user input)                  │
│  - No DSP logic, no scanning, no threading                     │
│  - Thread: Main UI Thread (priority: NORMAL)                  │
│  - Access: Read-only snapshots from DSP layer                  │
└─────────────────────────────────────────────────────────────────┘
                             │
                             │ (immutable snapshot exchange)
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│              DSP LAYER (Signal Processing)                    │
│  - DroneScanner (scanning logic)                               │
│  - SpectralAnalyzer (signal analysis)                          │
│  - Thread: Scanning Thread (priority: HIGH)                   │
│  - Access: Write-only snapshots to UI layer                    │
└─────────────────────────────────────────────────────────────────┘
```

### Thread-Safe Communication Data Structures

```cpp
// DisplayData: UI rendering data (immutable snapshot)
struct DisplayData {
    // Spectrum data (256 bins, 8-bit dB values)
    std::array<uint8_t, 256> spectrum_db;
    
    // Tracked drones (max 10)
    struct TrackedDroneDisplay {
        uint64_t frequency_hz;
        int32_t rssi_db;
        uint8_t snr;
        DroneType drone_type;
        ThreatLevel threat_level;
        uint32_t last_detection_age_ms;
    };
    std::array<TrackedDroneDisplay, 10> tracked_drones;
    uint8_t tracked_drone_count;
    
    // Scanning status
    bool scanning_active;
    uint32_t scan_progress_percent;
    
    // Timestamp for snapshot validity
    uint32_t snapshot_timestamp_ms;
    
    // Snapshot version (for detecting updates)
    uint32_t snapshot_version;
};

// DroneSnapshot: Atomic data exchange structure
struct DroneSnapshot {
    // Atomic flag for snapshot validity
    volatile bool valid;
    
    // Snapshot data (must be aligned for atomic operations)
    alignas(4) DisplayData data;
    
    // Snapshot version counter
    std::atomic<uint32_t> version;
    
    // Thread ID of producer
    uint8_t producer_thread_id;
    
    // Thread ID of last consumer
    uint8_t last_consumer_thread_id;
};

// SpectrumBuffer: Thread-safe spectrum data
class SpectrumBuffer {
public:
    static constexpr size_t SPECTRUM_BINS = 256;
    
    // Producer: DSP thread writes spectrum data
    void write_spectrum(const std::array<uint8_t, SPECTRUM_BINS>& spectrum) noexcept;
    
    // Consumer: UI thread reads spectrum data (copy to local buffer)
    void read_spectrum(std::array<uint8_t, SPECTRUM_BINS>& out) const noexcept;
    
    // Check if new data available
    bool has_new_data() const noexcept;
    
private:
    std::array<uint8_t, SPECTRUM_BINS> buffer_;
    mutable Mutex buffer_mutex_;
    volatile bool data_updated_;
    uint32_t update_timestamp_ms_;
};
```

### Snapshot Pattern for Atomic Data Exchange

```cpp
// Two-phase snapshot exchange (lock-free read, mutex-protected write)
class SnapshotManager {
public:
    // Producer (DSP thread): Write snapshot with mutex protection
    void write_snapshot(const DisplayData& data) noexcept {
        MutexLock lock(snapshot_mutex_, LockOrder::SNAPSHOT_MUTEX);
        
        // Update snapshot data
        snapshot_.data = data;
        snapshot_.data.snapshot_timestamp_ms = get_system_time_ms();
        snapshot_.data.snapshot_version = ++version_counter_;
        
        // Set valid flag after data is complete (memory barrier)
        chSysLock();
        snapshot_.valid = true;
        chSysUnlock();
    }
    
    // Consumer (UI thread): Read snapshot without blocking
    bool read_snapshot(DisplayData& out) const noexcept {
        // Check validity first (memory barrier for volatile read)
        chSysLock();
        bool valid = snapshot_.valid;
        chSysUnlock();
        
        if (!valid) {
            return false;
        }
        
        // Copy snapshot data (fast, no blocking)
        out = snapshot_.data;
        
        return true;
    }
    
private:
    DroneSnapshot snapshot_;
    mutable Mutex snapshot_mutex_;
    uint32_t version_counter_;
};
```

### Two-Phase Rendering Pattern

```cpp
// Phase 1: Fetch snapshot (non-blocking, fast)
class EnhancedDroneSpectrumAnalyzerView : public View {
private:
    // Local copy of display data (UI thread owns this)
    DisplayData display_data_;
    
    // Fetch snapshot from DSP layer (called at start of paint)
    void fetch_display_data() noexcept {
        auto& coordinator = ui::apps::enhanced_drone_analyzer::thread::ScanningCoordinator::instance();
        coordinator.read_snapshot(display_data_);
    }
    
public:
    // Phase 2: Render using local copy (no blocking, no mutex)
    void paint(Painter& painter) override {
        // Phase 1: Fetch snapshot (fast, non-blocking)
        fetch_display_data();
        
        // Phase 2: Render using local copy (no blocking, no mutex)
        paint_spectrum(painter);
        paint_drone_list(painter);
        paint_status_bar(painter);
    }
    
private:
    // Spectrum rendering (uses display_data_ only)
    void paint_spectrum(Painter& painter) {
        // Render 256 bins from display_data_.spectrum_db
        for (size_t i = 0; i < 256; ++i) {
            uint8_t db = display_data_.spectrum_db[i];
            // ... rendering code
        }
    }
    
    // Drone list rendering (uses display_data_ only)
    void paint_drone_list(Painter& painter) {
        for (uint8_t i = 0; i < display_data_.tracked_drone_count; ++i) {
            const auto& drone = display_data_.tracked_drones[i];
            // ... rendering code
        }
    }
};
```

---

## Fix #1: Safe Singleton Access Pattern

### Design Requirements

- Eliminate heap allocation in singleton initialization
- Provide safe initialization check mechanism
- Define is_initialized() static method
- Specify error handling for uninitialized singleton
- Memory barrier strategy using chSysLock/unlock

### Implementation

```cpp
// thread_coordinator.hpp
namespace ui::apps::enhanced_drone_analyzer::thread {

class ScanningCoordinator {
public:
    // Check if singleton is initialized (thread-safe)
    static bool is_initialized() noexcept {
        // Memory barrier before reading volatile flag
        chSysLock();
        bool initialized = initialized_;
        chSysUnlock();
        return initialized;
    }
    
    // Get singleton instance (with error handling)
    static ScanningCoordinator& instance() noexcept {
        // Check initialization first
        if (!is_initialized()) {
            // Error: singleton not initialized
            // In production: return null reference or trigger error handler
            // For now: trigger hard fault with error code
            trigger_hard_fault(0x20001E38);  // Heap allocation failure error code
            __builtin_unreachable();
        }
        
        // Return reference to static storage
        return *reinterpret_cast<ScanningCoordinator*>(instance_storage_);
    }
    
    // Initialize singleton (call once at startup)
    static bool initialize(
        NavigationView& nav,
        DroneHardwareController& hardware,
        DroneScanner& scanner,
        DroneDisplayController& display_controller,
        DroneAudioController& audio_controller
    ) noexcept {
        // Double-checked locking pattern
        chSysLock();
        if (initialized_) {
            chSysUnlock();
            return false;  // Already initialized
        }
        chSysUnlock();
        
        // Acquire mutex for initialization
        MutexLock lock(init_mutex_, LockOrder::INIT_MUTEX);
        
        // Check again (another thread may have initialized)
        chSysLock();
        if (initialized_) {
            chSysUnlock();
            return false;
        }
        chSysUnlock();
        
        // Validate storage size (compile-time)
        static_assert(sizeof(ScanningCoordinator) <= sizeof(instance_storage_),
                      "instance_storage_ too small for ScanningCoordinator");
        
        // Construct object in static storage (placement new, NO heap allocation)
        new (instance_storage_) ScanningCoordinator(
            nav, hardware, scanner, display_controller, audio_controller
        );
        
        // Memory barrier after writing volatile flag
        chSysLock();
        initialized_ = true;
        chSysUnlock();
        
        return true;
    }
    
private:
    // Static storage in BSS segment (zero-initialized at boot)
    // CRITICAL: This is NOT heap allocation - memory allocated at link time
    alignas(alignof(ScanningCoordinator))
    static uint8_t instance_storage_[sizeof(ScanningCoordinator)];
    
    // Initialization flag (volatile for thread safety)
    static volatile bool initialized_;
    
    // Initialization mutex
    static Mutex init_mutex_;
    
    // Private constructor (singleton pattern)
    ScanningCoordinator(
        NavigationView& nav,
        DroneHardwareController& hardware,
        DroneScanner& scanner,
        DroneDisplayController& display_controller,
        DroneAudioController& audio_controller
    );
    
    // Prevent copying
    ScanningCoordinator(const ScanningCoordinator&) = delete;
    ScanningCoordinator& operator=(const ScanningCoordinator&) = delete;
};

// Static member definitions
alignas(alignof(ScanningCoordinator))
uint8_t ScanningCoordinator::instance_storage_[sizeof(ScanningCoordinator)];
volatile bool ScanningCoordinator::initialized_ = false;
Mutex ScanningCoordinator::init_mutex_;

} // namespace ui::apps::enhanced_drone_analyzer::thread
```

### Error Handling Strategy

```cpp
// Error handling for uninitialized singleton access
enum class SingletonError {
    NOT_INITIALIZED = 0x20001E38,
    INITIALIZATION_FAILED = 0x20001E39,
    DOUBLE_INITIALIZATION = 0x20001E3A
};

// Safe instance access with error handling
template <typename T>
class SafeSingleton {
public:
    static T& instance() noexcept {
        if (!T::is_initialized()) {
            handle_singleton_error(SingletonError::NOT_INITIALIZED);
            __builtin_unreachable();
        }
        return T::instance();
    }
    
private:
    static void handle_singleton_error(SingletonError error) noexcept {
        // Log error
        log_error("Singleton error: 0x%08X", static_cast<uint32_t>(error));
        
        // Trigger hard fault with error code
        trigger_hard_fault(static_cast<uint32_t>(error));
    }
};
```

---

## Fix #2: Static Storage Protection

### Design Requirements

- Design memory barrier implementation
- Eliminate double access vulnerability
- Specify critical section usage
- Alignment requirements

### Implementation

```cpp
// thread_coordinator.hpp (continued)
namespace ui::apps::enhanced_drone_analyzer::thread {

class ScanningCoordinator {
public:
    // Thread-safe state access with memory barriers
    bool is_scanning_active() const noexcept {
        // Memory barrier before reading volatile flag
        chSysLock();
        bool active = scanning_active_;
        chSysUnlock();
        return active;
    }
    
    void set_scanning_active(bool active) noexcept {
        // Acquire mutex for state modification
        MutexLock lock(state_mutex_, LockOrder::STATE_MUTEX);
        
        // Modify state
        scanning_active_ = active;
        
        // Memory barrier after writing volatile flag
        chSysLock();
        // (volatile write already has memory barrier)
        chSysUnlock();
    }
    
private:
    // Volatile state flags (protected by mutex)
    volatile bool scanning_active_;
    volatile bool coordinator_running_;
    
    // State mutex (LockOrder::STATE_MUTEX)
    mutable Mutex state_mutex_;
};

// Static storage with alignment and protection
class StaticStorageManager {
public:
    // Template for static storage with validation
    template <typename T, size_t StorageSize>
    class StaticStorage {
    public:
        // Construct object in static storage
        template <typename... Args>
        void construct(Args&&... args) noexcept {
            // Validate storage size at compile time
            static_assert(sizeof(T) <= StorageSize,
                          "StaticStorage too small for type T");
            static_assert(StorageSize % alignof(T) == 0,
                          "StaticStorage not properly aligned");
            
            // Validate storage not already constructed
            if (constructed_) {
                // Error: double construction
                trigger_hard_fault(0x0080013);  // Memory access violation
                __builtin_unreachable();
            }
            
            // Memory barrier before construction
            chSysLock();
            
            // Construct object (placement new, NO heap allocation)
            new (storage_) T(std::forward<Args>(args)...);
            
            // Mark as constructed
            constructed_ = true;
            
            // Memory barrier after construction
            chSysUnlock();
        }
        
        // Get reference to object
        T& get() noexcept {
            // Validate object is constructed
            if (!constructed_) {
                trigger_hard_fault(0x0080013);  // Memory access violation
                __builtin_unreachable();
            }
            return *reinterpret_cast<T*>(storage_);
        }
        
        // Check if object is constructed
        bool is_constructed() const noexcept {
            // Memory barrier before reading volatile flag
            chSysLock();
            bool constructed = constructed_;
            chSysUnlock();
            return constructed;
        }
        
        // Destroy object
        void destroy() noexcept {
            // Validate object is constructed
            if (!constructed_) {
                return;
            }
            
            // Memory barrier before destruction
            chSysLock();
            
            // Call destructor
            get().~T();
            
            // Mark as not constructed
            constructed_ = false;
            
            // Memory barrier after destruction
            chSysUnlock();
        }
        
    private:
        // Storage buffer (aligned for type T)
        alignas(alignof(T)) uint8_t storage_[StorageSize];
        
        // Construction flag (volatile for thread safety)
        volatile bool constructed_ = false;
    };
};

} // namespace ui::apps::enhanced_drone_analyzer::thread
```

### Critical Section Usage

```cpp
// Critical section wrapper for memory barriers
class CriticalSection {
public:
    CriticalSection() noexcept {
        // Enter critical section (disable interrupts, lock scheduler)
        chSysLock();
    }
    
    ~CriticalSection() noexcept {
        // Exit critical section (re-enable interrupts, unlock scheduler)
        chSysUnlock();
    }
    
    // Prevent copying
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
};

// Usage example
void ScanningCoordinator::update_state() noexcept {
    // Critical section for volatile flag access
    {
        CriticalSection cs;
        scanning_active_ = true;
    }  // Memory barrier on scope exit
    
    // Critical section for another volatile flag
    {
        CriticalSection cs;
        coordinator_running_ = true;
    }  // Memory barrier on scope exit
}
```

---

## Fix #3: Thread-Local Stack Canary

### Design Requirements

- Design thread-local storage pattern
- Thread ID prefix for debugging
- Reinitialization strategy
- Check on function entry/exit

### Implementation

```cpp
// stack_canary.hpp
namespace eda::thread_safety {

// Thread-local stack canary
class ThreadLocalStackCanary {
public:
    // Initialize stack canary for current thread
    static void initialize(uint8_t thread_id) noexcept {
        // Get thread-local storage
        auto& tls = get_tls();
        
        // Set thread ID
        tls.thread_id = thread_id;
        
        // Generate canary value (pseudo-random based on thread ID and time)
        tls.canary_value = generate_canary(thread_id, get_system_time_ms());
        
        // Write canary to stack bottom
        tls.stack_canary_bottom = tls.canary_value;
        
        // Write canary to stack top
        tls.stack_canary_top = tls.canary_value;
        
        // Set initialized flag
        tls.initialized = true;
    }
    
    // Check stack canary (call on function entry)
    static void check_on_entry() noexcept {
        auto& tls = get_tls();
        
        if (!tls.initialized) {
            // Error: stack canary not initialized
            trigger_hard_fault(0x00000328);  // Stack corruption
            __builtin_unreachable();
        }
        
        // Check bottom canary
        if (tls.stack_canary_bottom != tls.canary_value) {
            // Error: stack overflow detected
            log_error("Stack overflow detected in thread %u", tls.thread_id);
            trigger_hard_fault(0x00000328);  // Stack corruption
            __builtin_unreachable();
        }
        
        // Check top canary
        if (tls.stack_canary_top != tls.canary_value) {
            // Error: stack underflow detected
            log_error("Stack underflow detected in thread %u", tls.thread_id);
            trigger_hard_fault(0x00000328);  // Stack corruption
            __builtin_unreachable();
        }
    }
    
    // Check stack canary (call on function exit)
    static void check_on_exit() noexcept {
        check_on_entry();  // Same check as entry
    }
    
    // Get thread ID
    static uint8_t get_thread_id() noexcept {
        return get_tls().thread_id;
    }
    
    // Reinitialize stack canary (call after stack overflow recovery)
    static void reinitialize() noexcept {
        auto& tls = get_tls();
        
        if (!tls.initialized) {
            return;  // Not initialized, nothing to reinitialize
        }
        
        // Generate new canary value
        tls.canary_value = generate_canary(tls.thread_id, get_system_time_ms());
        
        // Update canary values
        tls.stack_canary_bottom = tls.canary_value;
        tls.stack_canary_top = tls.canary_value;
    }
    
private:
    // Thread-local storage structure
    struct TLS {
        uint8_t thread_id;
        uint32_t canary_value;
        uint32_t stack_canary_bottom;
        uint32_t stack_canary_top;
        bool initialized;
    };
    
    // Get thread-local storage
    static TLS& get_tls() noexcept {
        // ChibiOS thread-local storage
        static thread_local TLS tls = {0, 0, 0, 0, false};
        return tls;
    }
    
    // Generate canary value
    static uint32_t generate_canary(uint8_t thread_id, uint32_t time_ms) noexcept {
        // Simple pseudo-random generator
        uint32_t seed = (thread_id << 24) | time_ms;
        seed = seed * 1103515245 + 12345;
        return seed;
    }
};

// RAII wrapper for stack canary checking
class StackCanaryGuard {
public:
    explicit StackCanaryGuard(const char* function_name) noexcept
        : function_name_(function_name) {
        // Check canary on entry
        ThreadLocalStackCanary::check_on_entry();
    }
    
    ~StackCanaryGuard() noexcept {
        // Check canary on exit
        ThreadLocalStackCanary::check_on_exit();
    }
    
    // Prevent copying
    StackCanaryGuard(const StackCanaryGuard&) = delete;
    StackCanaryGuard& operator=(const StackCanaryGuard&) = delete;
    
private:
    const char* function_name_;
};

// Macro for automatic stack canary checking
#define STACK_CANARY_GUARD() \
    eda::thread_safety::StackCanaryGuard _stack_canary_guard(__func__)

} // namespace eda::thread_safety
```

### Usage Example

```cpp
// drone_scanner_core.cpp
namespace ui::apps::enhanced_drone_analyzer::core {

DroneScanner::ScanResult DroneScanner::perform_scan_cycle(
    const HardwareInterface& hw
) noexcept {
    // Stack canary guard (automatic check on entry/exit)
    STACK_CANARY_GUARD();
    
    // ... scan logic ...
    
    return result;
}

} // namespace ui::apps::enhanced_drone_analyzer::core
```

---

## Fix #4: Ring Buffer Bounds Protection

### Design Requirements

- Compile-time bounds validation with static_assert
- Runtime bounds checking strategy
- Hash function design
- Modulo fallback for overflow

### Implementation

```cpp
// ring_buffer.hpp
namespace eda::data_structures {

// Hash function for index calculation
namespace detail {
    // FNV-1a hash (32-bit)
    constexpr uint32_t fnv1a_hash_32(const uint8_t* data, size_t length) noexcept {
        uint32_t hash = 2166136261u;  // FNV offset basis
        
        for (size_t i = 0; i < length; ++i) {
            hash ^= data[i];
            hash *= 16777619u;  // FNV prime
        }
        
        return hash;
    }
    
    // Compile-time hash for string literals
    constexpr uint32_t compile_time_hash(const char* str, size_t length) noexcept {
        uint32_t hash = 2166136261u;
        
        for (size_t i = 0; i < length; ++i) {
            hash ^= static_cast<uint32_t>(str[i]);
            hash *= 16777619u;
        }
        
        return hash;
    }
}

// Ring buffer with bounds protection
template <typename T, size_t Capacity>
class RingBuffer {
public:
    static_assert(Capacity > 0, "RingBuffer capacity must be > 0");
    static_assert(Capacity <= 256, "RingBuffer capacity must be <= 256");
    static_assert((Capacity & (Capacity - 1)) == 0, "RingBuffer capacity must be power of 2");
    
    RingBuffer() noexcept : head_(0), tail_(0), count_(0) {}
    
    // Write element (with bounds checking)
    bool write(const T& value) noexcept {
        // Check if buffer is full
        if (count_ >= Capacity) {
            return false;  // Buffer full
        }
        
        // Write element
        buffer_[head_] = value;
        
        // Update head (with modulo)
        head_ = (head_ + 1) % Capacity;
        
        // Update count
        ++count_;
        
        return true;
    }
    
    // Read element (with bounds checking)
    bool read(T& out) noexcept {
        // Check if buffer is empty
        if (count_ == 0) {
            return false;  // Buffer empty
        }
        
        // Read element
        out = buffer_[tail_];
        
        // Update tail (with modulo)
        tail_ = (tail_ + 1) % Capacity;
        
        // Update count
        --count_;
        
        return true;
    }
    
    // Peek element (without removing)
    bool peek(size_t index, T& out) const noexcept {
        // Check bounds
        if (index >= count_) {
            return false;  // Index out of bounds
        }
        
        // Calculate actual index (with modulo)
        size_t actual_index = (tail_ + index) % Capacity;
        
        // Read element
        out = buffer_[actual_index];
        
        return true;
    }
    
    // Clear buffer
    void clear() noexcept {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
    }
    
    // Get count
    size_t count() const noexcept {
        return count_;
    }
    
    // Check if buffer is full
    bool is_full() const noexcept {
        return count_ >= Capacity;
    }
    
    // Check if buffer is empty
    bool is_empty() const noexcept {
        return count_ == 0;
    }
    
private:
    // Buffer storage
    std::array<T, Capacity> buffer_;
    
    // Head index (next write position)
    size_t head_;
    
    // Tail index (next read position)
    size_t tail_;
    
    // Element count
    size_t count_;
};

// Detection ring buffer (specialized for drone detection)
class DetectionRingBuffer {
public:
    static constexpr size_t MAX_ENTRIES = 64;
    
    // Detection entry
    struct DetectionEntry {
        uint64_t frequency_hz;
        int32_t rssi_db;
        uint8_t snr;
        uint32_t timestamp_ms;
    };
    
    DetectionRingBuffer() noexcept = default;
    
    // Update detection (with bounds checking)
    bool update_detection(const DetectionEntry& entry) noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
        
        // Check for overflow
        if (ring_buffer_.is_full()) {
            // Buffer full, drop oldest entry
            DetectionEntry dropped;
            ring_buffer_.read(dropped);
            
            // Log warning
            log_warning("Detection ring buffer overflow, dropped entry at %llu Hz",
                       dropped.frequency_hz);
        }
        
        // Write new entry
        return ring_buffer_.write(entry);
    }
    
    // Get detection count for frequency hash
    uint8_t get_detection_count(uint32_t frequency_hash) const noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
        
        uint8_t count = 0;
        
        // Iterate through buffer
        for (size_t i = 0; i < ring_buffer_.count(); ++i) {
            DetectionEntry entry;
            if (ring_buffer_.peek(i, entry)) {
                // Calculate frequency hash
                uint32_t entry_hash = hash_frequency(entry.frequency_hz);
                
                // Compare hashes
                if (entry_hash == frequency_hash) {
                    ++count;
                }
            }
        }
        
        return count;
    }
    
    // Get RSSI value for frequency hash
    int32_t get_rssi_value(uint32_t frequency_hash) const noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
        
        // Find most recent entry with matching hash
        for (size_t i = ring_buffer_.count(); i > 0; --i) {
            DetectionEntry entry;
            if (ring_buffer_.peek(i - 1, entry)) {
                uint32_t entry_hash = hash_frequency(entry.frequency_hz);
                
                if (entry_hash == frequency_hash) {
                    return entry.rssi_db;
                }
            }
        }
        
        return RSSI_INVALID;  // Not found
    }
    
    // Clear buffer
    void clear() noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
        ring_buffer_.clear();
    }
    
private:
    // Ring buffer storage
    RingBuffer<DetectionEntry, MAX_ENTRIES> ring_buffer_;
    
    // Buffer mutex
    mutable Mutex buffer_mutex_;
    
    // Hash frequency (compile-time validated)
    static constexpr uint32_t hash_frequency(uint64_t frequency_hz) noexcept {
        // Simple hash function (modulo prime)
        constexpr uint32_t HASH_PRIME = 65537;  // Prime number
        return static_cast<uint32_t>(frequency_hz % HASH_PRIME);
    }
};

} // namespace eda::data_structures
```

---

## Fix #5: Unified Lock Order

### Design Requirements

- Define global lock order enum
- Lock acquisition hierarchy
- Debug verification mechanism
- Documentation requirements

### Implementation

```cpp
// lock_order.hpp
namespace eda::thread_safety {

// Global lock order enum (must be acquired in ascending order)
enum class LockOrder : uint8_t {
    // Level 0: Atomic flags (no mutex, volatile bool only)
    ATOMIC_FLAGS = 0,
    
    // Level 1: Initialization mutex (singleton initialization)
    INIT_MUTEX = 1,
    
    // Level 2: Data mutex (detection data, frequency DB)
    DATA_MUTEX = 2,
    
    // Level 3: Spectrum mutex (spectrum buffer)
    SPECTRUM_MUTEX = 3,
    
    // Level 4: State mutex (coordinator state, flags)
    STATE_MUTEX = 4,
    
    // Level 5: Snapshot mutex (snapshot exchange)
    SNAPSHOT_MUTEX = 5,
    
    // Level 6: Logger mutex (log file access)
    LOGGER_MUTEX = 6,
    
    // Level 7: SD card mutex (FatFS access - MUST BE LAST)
    SD_CARD_MUTEX = 7,
    
    // Invalid lock order
    INVALID = 255
};

// Lock order tracker (for debugging)
class LockOrderTracker {
public:
    // Register lock acquisition
    static void acquire_lock(LockOrder order) noexcept {
        // Validate lock order
        if (order < current_lock_order_) {
            // Error: lock order violation
            log_error("Lock order violation: trying to acquire lock %u while holding lock %u",
                      static_cast<uint8_t>(order), static_cast<uint8_t>(current_lock_order_));
            trigger_hard_fault(0x000177ae);  // Data abort (lock order violation)
            __builtin_unreachable();
        }
        
        // Update current lock order
        previous_lock_order_ = current_lock_order_;
        current_lock_order_ = order;
        
        // Increment lock depth
        ++lock_depth_;
    }
    
    // Register lock release
    static void release_lock(LockOrder order) noexcept {
        // Validate lock order
        if (order != current_lock_order_) {
            // Error: releasing wrong lock
            log_error("Lock release order violation: releasing lock %u but current is %u",
                      static_cast<uint8_t>(order), static_cast<uint8_t>(current_lock_order_));
            trigger_hard_fault(0x000177ae);  // Data abort (lock order violation)
            __builtin_unreachable();
        }
        
        // Decrement lock depth
        --lock_depth_;
        
        // Restore previous lock order
        current_lock_order_ = previous_lock_order_;
    }
    
    // Get current lock order
    static LockOrder get_current_lock_order() noexcept {
        return current_lock_order_;
    }
    
    // Get lock depth
    static uint8_t get_lock_depth() noexcept {
        return lock_depth_;
    }
    
    // Check if lock order is valid
    static bool is_valid_lock_order(LockOrder order) noexcept {
        return order >= LockOrder::ATOMIC_FLAGS && order <= LockOrder::SD_CARD_MUTEX;
    }
    
private:
    // Current lock order
    static LockOrder current_lock_order_;
    
    // Previous lock order
    static LockOrder previous_lock_order_;
    
    // Lock depth (number of locks held)
    static uint8_t lock_depth_;
};

// RAII mutex lock with lock order tracking
class MutexLock {
public:
    explicit MutexLock(Mutex& mutex, LockOrder order) noexcept
        : mutex_(mutex), order_(order), locked_(false) {
        // Validate lock order
        if (!LockOrderTracker::is_valid_lock_order(order)) {
            log_error("Invalid lock order: %u", static_cast<uint8_t>(order));
            trigger_hard_fault(0x000177ae);  // Data abort (invalid lock order)
            __builtin_unreachable();
        }
        
        // Register lock acquisition
        LockOrderTracker::acquire_lock(order);
        
        // Acquire mutex
        mutex_.lock();
        locked_ = true;
    }
    
    ~MutexLock() noexcept {
        if (locked_) {
            // Release mutex
            mutex_.unlock();
            
            // Register lock release
            LockOrderTracker::release_lock(order_);
        }
    }
    
    // Prevent copying
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    
private:
    Mutex& mutex_;
    LockOrder order_;
    bool locked_;
};

} // namespace eda::thread_safety
```

### Lock Order Documentation

```cpp
// lock_order_documentation.md
/*
 * LOCK ORDER DOCUMENTATION
 * =======================
 *
 * Locks must ALWAYS be acquired in ascending order (0 < 1 < 2 < ... < 7).
 * Never acquire a lower-numbered lock while holding a higher-numbered lock.
 *
 * Lock Order Hierarchy:
 * --------------------
 * 0. ATOMIC_FLAGS:    Volatile bool flags (no mutex, memory barriers only)
 * 1. INIT_MUTEX:      Singleton initialization
 * 2. DATA_MUTEX:      Detection data, frequency database
 * 3. SPECTRUM_MUTEX:  Spectrum buffer
 * 4. STATE_MUTEX:     Coordinator state, flags
 * 5. SNAPSHOT_MUTEX:  Snapshot exchange
 * 6. LOGGER_MUTEX:    Log file access
 * 7. SD_CARD_MUTEX:   FatFS access (MUST BE LAST)
 *
 * Examples:
 * ---------
 * ✅ CORRECT: Acquire DATA_MUTEX (2) then SPECTRUM_MUTEX (3)
 *    MutexLock data_lock(data_mutex, LockOrder::DATA_MUTEX);
 *    MutexLock spectrum_lock(spectrum_mutex, LockOrder::SPECTRUM_MUTEX);
 *
 * ❌ INCORRECT: Acquire SPECTRUM_MUTEX (3) then DATA_MUTEX (2)
 *    MutexLock spectrum_lock(spectrum_mutex, LockOrder::SPECTRUM_MUTEX);
 *    MutexLock data_lock(data_mutex, LockOrder::DATA_MUTEX);  // VIOLATION!
 *
 * ✅ CORRECT: Acquire SD_CARD_MUTEX (7) alone (highest lock)
 *    MutexLock sd_lock(sd_card_mutex, LockOrder::SD_CARD_MUTEX);
 *
 * ❌ INCORRECT: Acquire SD_CARD_MUTEX (7) then any other lock
 *    MutexLock sd_lock(sd_card_mutex, LockOrder::SD_CARD_MUTEX);
 *    MutexLock data_lock(data_mutex, LockOrder::DATA_MUTEX);  // VIOLATION!
 *
 * Debug Verification:
 * -------------------
 * In debug builds, LockOrderTracker validates lock order at runtime.
 * Violations trigger a hard fault with error code 0x000177ae.
 *
 * ISR Safety:
 * -----------
 * Mutex operations CANNOT be used in ISR context (ChibiOS limitation).
 * Use volatile bool flags (ATOMIC_FLAGS level) for ISR communication.
 */
```

---

## Fix #6: Stack Size Unification

### Design Requirements

- Consolidate stack size definitions
- Minimum stack size validation
- Compile-time assertions
- Per-thread stack allocation

### Implementation

```cpp
// thread_stack_sizes.hpp
namespace eda::threading {

// Unified stack size definitions
namespace StackSizes {
    // Minimum stack size (for any thread)
    constexpr size_t MINIMUM_STACK_SIZE = 1024;  // 1KB
    
    // Maximum stack size (per thread limit)
    constexpr size_t MAXIMUM_STACK_SIZE = 8192;  // 8KB
    
    // Main UI thread stack size
    constexpr size_t MAIN_UI_THREAD = 2048;  // 2KB
    
    // Scanning thread stack size
    constexpr size_t SCANNING_THREAD = 4096;  // 4KB
    
    // Coordinator thread stack size
    constexpr size_t COORDINATOR_THREAD = 2048;  // 2KB
    
    // Logger worker thread stack size
    constexpr size_t LOGGER_WORKER_THREAD = 4096;  // 4KB
    
    // Audio worker thread stack size
    constexpr size_t AUDIO_WORKER_THREAD = 2048;  // 2KB
    
    // Default stack size (for new threads)
    constexpr size_t DEFAULT = MAIN_UI_THREAD;
}

// Compile-time validation
namespace Validation {
    // Validate minimum stack sizes
    static_assert(StackSizes::MAIN_UI_THREAD >= StackSizes::MINIMUM_STACK_SIZE,
                  "MAIN_UI_THREAD stack size below minimum");
    static_assert(StackSizes::SCANNING_THREAD >= StackSizes::MINIMUM_STACK_SIZE,
                  "SCANNING_THREAD stack size below minimum");
    static_assert(StackSizes::COORDINATOR_THREAD >= StackSizes::MINIMUM_STACK_SIZE,
                  "COORDINATOR_THREAD stack size below minimum");
    static_assert(StackSizes::LOGGER_WORKER_THREAD >= StackSizes::MINIMUM_STACK_SIZE,
                  "LOGGER_WORKER_THREAD stack size below minimum");
    static_assert(StackSizes::AUDIO_WORKER_THREAD >= StackSizes::MINIMUM_STACK_SIZE,
                  "AUDIO_WORKER_THREAD stack size below minimum");
    
    // Validate maximum stack sizes
    static_assert(StackSizes::MAIN_UI_THREAD <= StackSizes::MAXIMUM_STACK_SIZE,
                  "MAIN_UI_THREAD stack size exceeds maximum");
    static_assert(StackSizes::SCANNING_THREAD <= StackSizes::MAXIMUM_STACK_SIZE,
                  "SCANNING_THREAD stack size exceeds maximum");
    static_assert(StackSizes::COORDINATOR_THREAD <= StackSizes::MAXIMUM_STACK_SIZE,
                  "COORDINATOR_THREAD stack size exceeds maximum");
    static_assert(StackSizes::LOGGER_WORKER_THREAD <= StackSizes::MAXIMUM_STACK_SIZE,
                  "LOGGER_WORKER_THREAD stack size exceeds maximum");
    static_assert(StackSizes::AUDIO_WORKER_THREAD <= StackSizes::MAXIMUM_STACK_SIZE,
                  "AUDIO_WORKER_THREAD stack size exceeds maximum");
    
    // Validate stack sizes are multiples of 64 (for alignment)
    static_assert(StackSizes::MAIN_UI_THREAD % 64 == 0,
                  "MAIN_UI_THREAD stack size not aligned to 64 bytes");
    static_assert(StackSizes::SCANNING_THREAD % 64 == 0,
                  "SCANNING_THREAD stack size not aligned to 64 bytes");
    static_assert(StackSizes::COORDINATOR_THREAD % 64 == 0,
                  "COORDINATOR_THREAD stack size not aligned to 64 bytes");
    static_assert(StackSizes::LOGGER_WORKER_THREAD % 64 == 0,
                  "LOGGER_WORKER_THREAD stack size not aligned to 64 bytes");
    static_assert(StackSizes::AUDIO_WORKER_THREAD % 64 == 0,
                  "AUDIO_WORKER_THREAD stack size not aligned to 64 bytes");
}

// Thread stack allocation
class ThreadStackAllocator {
public:
    // Allocate stack for main UI thread
    static void* allocate_main_ui_stack() noexcept {
        return allocate_stack(StackSizes::MAIN_UI_THREAD, "MainUI");
    }
    
    // Allocate stack for scanning thread
    static void* allocate_scanning_stack() noexcept {
        return allocate_stack(StackSizes::SCANNING_THREAD, "Scanning");
    }
    
    // Allocate stack for coordinator thread
    static void* allocate_coordinator_stack() noexcept {
        return allocate_stack(StackSizes::COORDINATOR_THREAD, "Coordinator");
    }
    
    // Allocate stack for logger worker thread
    static void* allocate_logger_worker_stack() noexcept {
        return allocate_stack(StackSizes::LOGGER_WORKER_THREAD, "LoggerWorker");
    }
    
    // Allocate stack for audio worker thread
    static void* allocate_audio_worker_stack() noexcept {
        return allocate_stack(StackSizes::AUDIO_WORKER_THREAD, "AudioWorker");
    }
    
    // Allocate stack with custom size
    static void* allocate_custom_stack(size_t size, const char* thread_name) noexcept {
        // Validate stack size
        if (size < StackSizes::MINIMUM_STACK_SIZE) {
            log_error("Custom stack size %zu below minimum %zu for thread %s",
                      size, StackSizes::MINIMUM_STACK_SIZE, thread_name);
            return nullptr;
        }
        
        if (size > StackSizes::MAXIMUM_STACK_SIZE) {
            log_error("Custom stack size %zu exceeds maximum %zu for thread %s",
                      size, StackSizes::MAXIMUM_STACK_SIZE, thread_name);
            return nullptr;
        }
        
        // Allocate stack
        return allocate_stack(size, thread_name);
    }
    
private:
    // Allocate stack (internal)
    static void* allocate_stack(size_t size, const char* thread_name) noexcept {
        // Validate alignment
        if (size % 64 != 0) {
            log_error("Stack size %zu not aligned to 64 bytes for thread %s",
                      size, thread_name);
            return nullptr;
        }
        
        // Allocate stack memory (static storage, NO heap)
        // Note: In ChibiOS, thread stacks are allocated from a memory pool
        // This is a simplified example
        static uint8_t stack_pool[StackSizes::MAXIMUM_STACK_SIZE * 8];  // Pool for 8 threads
        static size_t pool_offset = 0;
        
        if (pool_offset + size > sizeof(stack_pool)) {
            log_error("Stack pool exhausted for thread %s", thread_name);
            return nullptr;
        }
        
        void* stack = &stack_pool[pool_offset];
        pool_offset += size;
        
        // Initialize stack canary
        eda::thread_safety::ThreadLocalStackCanary::initialize(
            get_thread_id_for_name(thread_name)
        );
        
        return stack;
    }
    
    // Get thread ID for thread name
    static uint8_t get_thread_id_for_name(const char* thread_name) noexcept {
        // Simple hash of thread name
        uint32_t hash = 0;
        for (size_t i = 0; thread_name[i] != '\0'; ++i) {
            hash = hash * 31 + thread_name[i];
        }
        return static_cast<uint8_t>(hash % 256);
    }
};

} // namespace eda::threading
```

---

## Data Structures Design

### DisplayData Struct

```cpp
// display_data.hpp
namespace eda::data_structures {

// Display data structure for UI rendering
struct DisplayData {
    // Spectrum data (256 bins, 8-bit dB values)
    std::array<uint8_t, 256> spectrum_db;
    
    // Tracked drone display data
    struct TrackedDroneDisplay {
        uint64_t frequency_hz;           // Frequency in Hz
        int32_t rssi_db;                 // RSSI in dBm
        uint8_t snr;                     // Signal-to-noise ratio in dB
        DroneType drone_type;            // Drone type (DJI, Parrot, etc.)
        ThreatLevel threat_level;        // Threat level (LOW, MEDIUM, HIGH)
        uint32_t last_detection_age_ms;  // Age of last detection in ms
    };
    
    // Tracked drones (max 10)
    std::array<TrackedDroneDisplay, 10> tracked_drones;
    uint8_t tracked_drone_count;
    
    // Scanning status
    bool scanning_active;
    uint32_t scan_progress_percent;
    
    // Timestamp for snapshot validity
    uint32_t snapshot_timestamp_ms;
    
    // Snapshot version (for detecting updates)
    uint32_t snapshot_version;
    
    // Default constructor
    DisplayData() noexcept
        : spectrum_db{},
          tracked_drones{},
          tracked_drone_count(0),
          scanning_active(false),
          scan_progress_percent(0),
          snapshot_timestamp_ms(0),
          snapshot_version(0) {}
    
    // Clear display data
    void clear() noexcept {
        spectrum_db.fill(0);
        tracked_drones.fill({});
        tracked_drone_count = 0;
        scanning_active = false;
        scan_progress_percent = 0;
        snapshot_timestamp_ms = 0;
        snapshot_version = 0;
    }
    
    // Validate display data
    bool is_valid() const noexcept {
        // Validate tracked drone count
        if (tracked_drone_count > 10) {
            return false;
        }
        
        // Validate scan progress
        if (scan_progress_percent > 100) {
            return false;
        }
        
        return true;
    }
};

// Compile-time validation
static_assert(sizeof(DisplayData) <= 4096,
              "DisplayData exceeds 4KB limit");
static_assert(alignof(DisplayData) <= 8,
              "DisplayData alignment exceeds 8 bytes");

} // namespace eda::data_structures
```

### DroneSnapshot Struct

```cpp
// drone_snapshot.hpp
namespace eda::data_structures {

// Drone snapshot for atomic data exchange
struct DroneSnapshot {
    // Atomic flag for snapshot validity
    volatile bool valid;
    
    // Snapshot data (must be aligned for atomic operations)
    alignas(4) DisplayData data;
    
    // Snapshot version counter
    std::atomic<uint32_t> version;
    
    // Thread ID of producer
    uint8_t producer_thread_id;
    
    // Thread ID of last consumer
    uint8_t last_consumer_thread_id;
    
    // Default constructor
    DroneSnapshot() noexcept
        : valid(false),
          data{},
          version(0),
          producer_thread_id(0),
          last_consumer_thread_id(0) {}
    
    // Initialize snapshot
    void initialize(uint8_t producer_id) noexcept {
        valid = false;
        data.clear();
        version.store(0, std::memory_order_relaxed);
        producer_thread_id = producer_id;
        last_consumer_thread_id = 0;
    }
    
    // Mark snapshot as valid
    void mark_valid() noexcept {
        // Memory barrier before writing volatile flag
        chSysLock();
        valid = true;
        chSysUnlock();
    }
    
    // Mark snapshot as invalid
    void mark_invalid() noexcept {
        // Memory barrier before writing volatile flag
        chSysLock();
        valid = false;
        chSysUnlock();
    }
    
    // Check if snapshot is valid
    bool is_valid() const noexcept {
        // Memory barrier before reading volatile flag
        chSysLock();
        bool v = valid;
        chSysUnlock();
        return v;
    }
    
    // Increment version
    void increment_version() noexcept {
        version.fetch_add(1, std::memory_order_relaxed);
    }
    
    // Get version
    uint32_t get_version() const noexcept {
        return version.load(std::memory_order_relaxed);
    }
};

// Compile-time validation
static_assert(sizeof(DroneSnapshot) <= 4160,
              "DroneSnapshot exceeds 4KB limit");
static_assert(alignof(DroneSnapshot) <= 8,
              "DroneSnapshot alignment exceeds 8 bytes");

} // namespace eda::data_structures
```

### SpectrumBuffer Class

```cpp
// spectrum_buffer.hpp
namespace eda::data_structures {

// Thread-safe spectrum buffer
class SpectrumBuffer {
public:
    static constexpr size_t SPECTRUM_BINS = 256;
    
    SpectrumBuffer() noexcept
        : buffer_{},
          buffer_mutex_(),
          data_updated_(false),
          update_timestamp_ms_(0) {}
    
    // Producer: DSP thread writes spectrum data
    void write_spectrum(const std::array<uint8_t, SPECTRUM_BINS>& spectrum) noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);
        
        // Copy spectrum data
        buffer_ = spectrum;
        
        // Update timestamp
        update_timestamp_ms_ = get_system_time_ms();
        
        // Mark data as updated
        data_updated_ = true;
    }
    
    // Consumer: UI thread reads spectrum data (copy to local buffer)
    void read_spectrum(std::array<uint8_t, SPECTRUM_BINS>& out) const noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);
        
        // Copy spectrum data
        out = buffer_;
    }
    
    // Check if new data available
    bool has_new_data() const noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);
        return data_updated_;
    }
    
    // Clear data updated flag
    void clear_data_updated() noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);
        data_updated_ = false;
    }
    
    // Get update timestamp
    uint32_t get_update_timestamp() const noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);
        return update_timestamp_ms_;
    }
    
    // Clear buffer
    void clear() noexcept {
        MutexLock lock(buffer_mutex_, LockOrder::SPECTRUM_MUTEX);
        buffer_.fill(0);
        data_updated_ = false;
        update_timestamp_ms_ = 0;
    }
    
private:
    // Spectrum buffer (256 bins, 8-bit dB values)
    std::array<uint8_t, SPECTRUM_BINS> buffer_;
    
    // Buffer mutex
    mutable Mutex buffer_mutex_;
    
    // Data updated flag
    volatile bool data_updated_;
    
    // Update timestamp
    uint32_t update_timestamp_ms_;
};

// Compile-time validation
static_assert(sizeof(SpectrumBuffer) <= 512,
              "SpectrumBuffer exceeds 512 bytes");
static_assert(alignof(SpectrumBuffer) <= 8,
              "SpectrumBuffer alignment exceeds 8 bytes");

} // namespace eda::data_structures
```

---

## Function Signatures

### New Function Signatures for Separated Logic

```cpp
// drone_scanner_core.hpp
namespace ui::apps::enhanced_drone_analyzer::core {

class DroneScanner {
public:
    // Scan result structure
    struct ScanResult {
        uint64_t frequency_hz;
        int32_t rssi_db;
        uint8_t snr;
        DroneType drone_type;
        ThreatLevel threat_level;
        bool is_valid;
    };
    
    // Perform scan cycle (pure DSP, no UI dependencies)
    [[nodiscard]] ScanResult perform_scan_cycle(
        const HardwareInterface& hw
    ) noexcept;
    
    // Update tracked drone
    void update_tracked_drone(const ScanResult& result) noexcept;
    
    // Remove stale drones
    void remove_stale_drones(uint32_t stale_threshold_ms) noexcept;
    
    // Get tracked drone count
    [[nodiscard]] uint8_t get_tracked_drone_count() const noexcept;
    
    // Get tracked drone by index
    [[nodiscard]] bool get_tracked_drone(
        uint8_t index,
        ScanResult& out
    ) const noexcept;
    
    // Clear all tracked drones
    void clear_tracked_drones() noexcept;
    
private:
    // Private constructor (singleton pattern)
    DroneScanner() noexcept = default;
    
    // Prevent copying
    DroneScanner(const DroneScanner&) = delete;
    DroneScanner& operator=(const DroneScanner&) = delete;
    
    // Friend for singleton access
    friend class DroneScannerSingleton;
};

} // namespace ui::apps::enhanced_drone_analyzer::core
```

### RAII Wrapper Patterns

```cpp
// raii_wrappers.hpp
namespace eda::raii {

// RAII mutex lock wrapper
class MutexLock {
public:
    explicit MutexLock(Mutex& mutex, LockOrder order) noexcept
        : mutex_(mutex), order_(order), locked_(false) {
        LockOrderTracker::acquire_lock(order);
        mutex_.lock();
        locked_ = true;
    }
    
    ~MutexLock() noexcept {
        if (locked_) {
            mutex_.unlock();
            LockOrderTracker::release_lock(order_);
        }
    }
    
    // Prevent copying
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    
private:
    Mutex& mutex_;
    LockOrder order_;
    bool locked_;
};

// RAII critical section wrapper
class CriticalSection {
public:
    CriticalSection() noexcept {
        chSysLock();
    }
    
    ~CriticalSection() noexcept {
        chSysUnlock();
    }
    
    // Prevent copying
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
};

// RAII stack canary guard
class StackCanaryGuard {
public:
    explicit StackCanaryGuard(const char* function_name) noexcept
        : function_name_(function_name) {
        ThreadLocalStackCanary::check_on_entry();
    }
    
    ~StackCanaryGuard() noexcept {
        ThreadLocalStackCanary::check_on_exit();
    }
    
    // Prevent copying
    StackCanaryGuard(const StackCanaryGuard&) = delete;
    StackCanaryGuard& operator=(const StackCanaryGuard&) = delete;
    
private:
    const char* function_name_;
};

// RAII scope timer (for performance measurement)
class ScopeTimer {
public:
    explicit ScopeTimer(const char* name) noexcept
        : name_(name), start_time_(get_system_time_us()) {}
    
    ~ScopeTimer() noexcept {
        uint32_t elapsed_us = get_system_time_us() - start_time_;
        log_debug("%s: %lu us", name_, elapsed_us);
    }
    
    // Prevent copying
    ScopeTimer(const ScopeTimer&) = delete;
    ScopeTimer& operator=(const ScopeTimer&) = delete;
    
private:
    const char* name_;
    uint32_t start_time_;
};

} // namespace eda::raii
```

### noexcept Specifications

```cpp
// All functions should be noexcept unless they can throw
namespace ui::apps::enhanced_drone_analyzer::core {

// DSP functions (always noexcept)
[[nodiscard]] DroneScanner::ScanResult
DroneScanner::perform_scan_cycle(const HardwareInterface& hw) noexcept {
    STACK_CANARY_GUARD();
    // ... scan logic ...
    return result;
}

void DroneScanner::update_tracked_drone(const ScanResult& result) noexcept {
    STACK_CANARY_GUARD();
    // ... update logic ...
}

} // namespace ui::apps::enhanced_drone_analyzer::core
```

### Memory Placement (Flash vs RAM)

```cpp
// Constants in Flash (using const/constexpr)
namespace eda::constants {

// Thread coordination constants (Flash)
namespace ThreadCoordination {
    constexpr uint32_t TERMINATION_TIMEOUT_MS = 5000;
    constexpr uint32_t POLL_INTERVAL_MS = 10;
    constexpr uint32_t MAX_CONSECUTIVE_TIMEOUTS = 3;
    constexpr uint32_t MAX_CONSECUTIVE_SCANNER_FAILURES = 5;
}

// Scanning constants (Flash)
namespace Scanning {
    constexpr uint8_t MAX_SCAN_MODES = 3;
    constexpr uint32_t CYCLES_CLAMP_MAX = 39;
    constexpr size_t HIGH_DENSITY_DETECTION_THRESHOLD = 20;
}

// UI constants (Flash)
namespace UI {
    constexpr size_t SPECTRUM_POWER_LEVELS_SIZE = 200;
    constexpr size_t MAX_UI_DRONES = 10;
}

// Compile-time validation
static_assert(ThreadCoordination::TERMINATION_TIMEOUT_MS <= 10000,
              "TERMINATION_TIMEOUT_MS exceeds 10 seconds");
static_assert(UI::SPECTRUM_POWER_LEVELS_SIZE <= 512,
              "SPECTRUM_POWER_LEVELS_SIZE exceeds 512 bytes");

} // namespace eda::constants

// Static data in BSS segment (RAM, zero-initialized)
namespace eda::static_storage {

// Static storage for singleton
alignas(alignof(ScanningCoordinator))
static uint8_t scanning_coordinator_storage_[sizeof(ScanningCoordinator)];

// Static storage for spectrum buffer
alignas(alignof(SpectrumBuffer))
static uint8_t spectrum_buffer_storage_[sizeof(SpectrumBuffer)];

} // namespace eda::static_storage
```

---

## Implementation Sequence

### Phase 1: Critical Fixes (P0 - High Risk, High Priority)

| Fix | Risk | Dependencies | Testing Strategy |
|-----|------|--------------|------------------|
| Fix #1: Safe Singleton Access Pattern | HIGH | None | Unit tests for initialization, multi-threaded access |
| Fix #2: Static Storage Protection | HIGH | Fix #1 | Memory corruption tests, race condition tests |
| Fix #3: Thread-Local Stack Canary | MEDIUM | None | Stack overflow simulation tests |
| Fix #4: Ring Buffer Bounds Protection | MEDIUM | None | Overflow/underflow tests, hash collision tests |
| Fix #5: Unified Lock Order | HIGH | None | Deadlock detection tests, lock order validation |
| Fix #6: Stack Size Unification | LOW | None | Stack usage analysis, static_assert validation |

**Order:**
1. Fix #6 (Stack Size Unification) - Low risk, foundational
2. Fix #5 (Unified Lock Order) - High risk, but foundational
3. Fix #3 (Thread-Local Stack Canary) - Medium risk, enables other fixes
4. Fix #4 (Ring Buffer Bounds Protection) - Medium risk, independent
5. Fix #2 (Static Storage Protection) - High risk, depends on Fix #1
6. Fix #1 (Safe Singleton Access Pattern) - High risk, depends on Fix #2

### Phase 2: Core Extraction (P1 - Medium Risk, Medium Priority)

| Component | Risk | Dependencies | Testing Strategy |
|-----------|------|--------------|------------------|
| drone_scanner_core.hpp/cpp | MEDIUM | Phase 1 | Unit tests for DSP logic, integration tests |
| spectral_analyzer_core.hpp/cpp | MEDIUM | Phase 1 | Unit tests for signal processing |
| thread_coordinator.hpp/cpp | HIGH | Phase 1, Fix #1 | Multi-threaded tests, race condition tests |
| drone_hardware_controller.hpp/cpp | LOW | Phase 1 | Hardware abstraction tests, mock tests |

**Order:**
1. drone_hardware_controller.hpp/cpp - Low risk, foundational
2. drone_scanner_core.hpp/cpp - Medium risk, depends on hardware abstraction
3. spectral_analyzer_core.hpp/cpp - Medium risk, independent
4. thread_coordinator.hpp/cpp - High risk, depends on all core components

### Phase 3: UI Refactoring (P2 - Low Risk, Low Priority)

| Component | Risk | Dependencies | Testing Strategy |
|-----------|------|--------------|------------------|
| ui_enhanced_drone_analyzer.cpp | LOW | Phase 1, Phase 2 | UI rendering tests, regression tests |
| ui_enhanced_drone_settings.hpp | LOW | Phase 1 | UI widget tests, regression tests |
| Data structure replacements | LOW | Phase 1, Phase 2 | Memory usage tests, performance tests |

**Order:**
1. Data structure replacements - Low risk, incremental
2. ui_enhanced_drone_settings.hpp - Low risk, independent
3. ui_enhanced_drone_analyzer.cpp - Low risk, final step

### Risk Assessment

| Risk Category | Mitigation Strategy |
|---------------|---------------------|
| Heap allocation failure | Use static storage, compile-time validation |
| Stack overflow | Stack canary, stack size validation, static_assert |
| Race conditions | Mutex protection, lock order enforcement, memory barriers |
| Deadlock | Lock order enforcement, lock order tracker |
| Memory corruption | Stack canary, bounds checking, static_assert |
| Performance regression | Benchmark before/after, optimize critical paths |
| UI regressions | Comprehensive UI tests, visual regression tests |

### Testing Strategy

1. **Unit Tests:** Test individual components in isolation
2. **Integration Tests:** Test component interactions
3. **Multi-threaded Tests:** Test thread safety, race conditions
4. **Performance Tests:** Benchmark critical paths, measure latency
5. **Memory Tests:** Validate zero heap, stack usage, memory leaks
6. **Regression Tests:** Ensure no regressions in functionality
7. **Error Injection Tests:** Test error handling, recovery paths

---

## Memory Layout Plan

### BSS Segment (Zero-Initialized Static Storage)

```
┌─────────────────────────────────────────────────────────────────┐
│ ScanningCoordinator instance:        ~512 bytes                 │
│   - Static storage for singleton                               │
│   - Zero-initialized at boot                                   │
├─────────────────────────────────────────────────────────────────┤
│ DroneScanner storage:               ~4096 bytes                │
│   - Tracked drones (20 * 48 bytes)                             │
│   - Frequency database (4096 bytes)                             │
│   - Detection ring buffer (64 * 24 bytes)                       │
├─────────────────────────────────────────────────────────────────┤
│ SpectralAnalyzer storage:             ~256 bytes                │
│   - Histogram buffer (64 * 2 bytes)                           │
│   - Analysis parameters                                         │
├─────────────────────────────────────────────────────────────────┤
│ SpectrumBuffer storage:               ~256 bytes                │
│   - Spectrum data (256 bytes)                                  │
│   - Mutex and flags                                              │
├─────────────────────────────────────────────────────────────────┤
│ UnifiedDroneDatabase storage:         ~5760 bytes                │
│   - 120 drone entries (120 * 48 bytes)                         │
│   - Index structures                                             │
├─────────────────────────────────────────────────────────────────┤
│ DisplayData storage:                 ~4096 bytes                │
│   - Spectrum data (256 bytes)                                  │
│   - Tracked drones (10 * 32 bytes)                              │
│   - Snapshot metadata                                           │
├─────────────────────────────────────────────────────────────────┤
│ Thread stacks (static pool):          ~32768 bytes               │
│   - Main UI thread (2048 bytes)                                │
│   - Scanning thread (4096 bytes)                                │
│   - Coordinator thread (2048 bytes)                              │
│   - Logger worker thread (4096 bytes)                           │
│   - Audio worker thread (2048 bytes)                            │
│   - Reserve for future threads (20480 bytes)                    │
└─────────────────────────────────────────────────────────────────┘
Total BSS: ~51,744 bytes (50.5 KB)
```

### Flash Storage (Constants, Code)

```
┌─────────────────────────────────────────────────────────────────┐
│ Code segment:                      ~128 KB                     │
│   - Core DSP logic                                             │
│   - Signal processing algorithms                                │
│   - UI rendering code                                            │
│   - Thread coordination                                          │
├─────────────────────────────────────────────────────────────────┤
│ Constants (Flash):                  ~2 KB                       │
│   - Thread coordination constants                               │
│   - Scanning constants                                          │
│   - UI constants                                                │
│   - Magic number replacements                                   │
├─────────────────────────────────────────────────────────────────┤
│ String literals:                    ~1 KB                       │
│   - Error messages                                               │
│   - Log messages                                                │
│   - UI labels                                                   │
└─────────────────────────────────────────────────────────────────┘
Total Flash: ~131 KB (within 1MB Flash budget)
```

### Stack Usage Per Function

```
┌─────────────────────────────────────────────────────────────────┐
│ DroneScanner::perform_scan_cycle():      ~256 bytes            │
│   - Local variables (result, temporary buffers)                 │
│   - Function call depth: ~3 levels                              │
├─────────────────────────────────────────────────────────────────┤
│ SpectralAnalyzer::analyze():            ~512 bytes            │
│   - Histogram buffer (64 * 2 bytes)                             │
│   - Temporary arrays                                             │
│   - Function call depth: ~4 levels                              │
├─────────────────────────────────────────────────────────────────┤
│ ScanningCoordinator::run():             ~384 bytes            │
│   - Local variables                                               │
│   - Function call depth: ~2 levels                              │
├─────────────────────────────────────────────────────────────────┤
│ UI paint functions:                   ~128 bytes            │
│   - Local variables (painter state)                             │
│   - Function call depth: ~2 levels                              │
└─────────────────────────────────────────────────────────────────┘
Maximum stack usage per function: ~512 bytes
```

### Static Storage Requirements

```
┌─────────────────────────────────────────────────────────────────┐
│ Singleton instances:                  ~512 bytes                │
│   - ScanningCoordinator                                         │
│   - DroneScanner                                                │
│   - SpectralAnalyzer                                            │
├─────────────────────────────────────────────────────────────────┤
│ Data structures:                    ~10240 bytes               │
│   - Detection ring buffer (64 * 24 bytes)                      │
│   - Tracked drones (20 * 48 bytes)                              │
│   - Frequency database (4096 bytes)                             │
│   - UnifiedDroneDatabase (120 * 48 bytes)                       │
├─────────────────────────────────────────────────────────────────┤
│ Snapshot buffers:                    ~4160 bytes                │
│   - DisplayData (4096 bytes)                                    │
│   - DroneSnapshot (64 bytes)                                    │
├─────────────────────────────────────────────────────────────────┤
│ Thread-local storage:                 ~64 bytes                 │
│   - Stack canary per thread (4 threads * 16 bytes)             │
└─────────────────────────────────────────────────────────────────┘
Total static storage: ~14,976 bytes (14.6 KB)
```

### Memory Budget Summary

| Segment | Size | Percentage | Status |
|---------|------|------------|--------|
| Total RAM | 128 KB | 100% | ✅ Within budget |
| BSS Segment | ~50.5 KB | 39.5% | ✅ Within budget |
| Stack Usage | ~12.3 KB | 9.6% | ✅ Within budget |
| Static Storage | ~14.6 KB | 11.4% | ✅ Within budget |
| Heap Available | ~76.8 KB | 60.0% | ✅ Healthy headroom |
| Headroom | ~12.0 KB | 9.4% | ✅ Safety margin |

---

## Success Criteria

### Functional Requirements
- [ ] All error codes from Stage 1 are resolved
- [ ] UI renders correctly (no regressions)
- [ ] Scanning works (no regressions)
- [ ] Database load/save works (no regressions)
- [ ] Audio alerts work (no regressions)

### Non-Functional Requirements
- [ ] Zero heap allocations (verified with chHeapStatus)
- [ ] Zero std::string usage (except framework compatibility)
- [ ] Zero std::vector usage
- [ ] All stack usage < 4KB per thread
- [ ] All mutex operations follow lock order
- [ ] No magic numbers (all constants defined)
- [ ] All types are semantic (no uint32_t for frequency)

### Code Quality Requirements
- [ ] All functions are noexcept
- [ ] All public APIs have [[nodiscard]] where appropriate
- [ ] All code follows Diamond Code standard
- [ ] All code has Doxygen comments
- [ ] All code is tested (unit tests)

### Performance Requirements
- [ ] UI paint latency < 16ms (60 FPS)
- [ ] Scan cycle time < 100ms
- [ ] Spectrum analysis time < 10ms
- [ ] No frame drops during scanning

---

**Next Stage:** STAGE 3 - The Red Team Attack (Verification)
