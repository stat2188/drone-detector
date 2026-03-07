// * @file ui_signal_processing.cpp * @brief Implementation of signal processing utilities for Enhanced Drone Analyzer

// Corresponding header (must be first)
#include "ui_signal_processing.hpp"

// C++ standard library headers (alphabetical order)
#include <algorithm>
#include <new>

// Third-party library headers
#include <ch.h>

namespace ui::apps::enhanced_drone_analyzer {

// Implementation: update_detection()
// / @brief Update detection entry (mutex-protected writer)
// / @note Updates or creates a detection entry for given frequency hash.
// / @note Uses linear probing with simple modulo-based hash for collision resolution.
// / @note Evicts oldest entry when hash table is full (ring buffer eviction).
// / @note SYNCHRONIZATION:
// /   - Acquires buffer_mutex_ for exclusive access
// /   - Increments global_version_ for concurrent update detection
// /   - Stores head_ with release semantics
// / @note RED TEAM FIX: Recursion detection prevents deadlock when called recursively.
// / @note DIAMOND FIX: Uses DetectionUpdate struct to prevent parameter swapping
// / @param update Detection update parameters (frequency_hash, detection_count, rssi_value)

void DetectionRingBuffer::update_detection(const DetectionUpdate& update) noexcept {
    const FrequencyHash frequency_hash = update.frequency_hash;
    const DetectionCount detection_count = update.detection_count;
    const RSSIValue rssi_value = update.rssi_value;
    const Timestamp current_time = static_cast<Timestamp>(chTimeNow());

    // FIX: Use thread-local storage for recursion detection to avoid race condition
    // Previous implementation checked recursion_depth_ after acquiring mutex, which
    // could allow two threads to both check and increment before either sees the change.
    // Thread-local storage ensures each thread has its own recursion counter.
    static thread_local int tls_recursion_depth = 0;

    // Check recursion depth before acquiring mutex (no race condition with TLS)
    if (tls_recursion_depth > 0) {
        // Recursive call detected - return early to prevent deadlock
        return;
    }

    // Increment recursion depth (TLS is thread-safe)
    tls_recursion_depth++;

    // DIAMOND FIX #2-3: Replace OrderedScopedLock with MutexLock from eda_locking.hpp
    // Lock order: DATA_MUTEX (level 1) for detection data and frequency database
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Increment global version for this update
    global_version_++;

    // Hash table lookup (using simple modulo-based hash)
    const size_t hash_idx = hash_index(frequency_hash);

    // Linear probe (bounded by HASH_TABLE_SIZE)
    for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
        const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;

        // Check for existing entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            // Update entry with new version
            entries_[idx].version = global_version_;
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].detection_count = detection_count;
            entries_[idx].timestamp = current_time;

            // Decrement thread-local recursion depth
            tls_recursion_depth--;
            return;
        }

        // Check for empty slot
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            entries_[idx].version = global_version_;
            entries_[idx].frequency_hash = frequency_hash;
            entries_[idx].detection_count = detection_count;
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].timestamp = current_time;

            tls_recursion_depth--;
            return;
        }
    }

    // Table full, evict oldest entry (ring buffer eviction)
    const size_t evict_idx = head_;
    entries_[evict_idx].version = global_version_;
    entries_[evict_idx].frequency_hash = frequency_hash;
    entries_[evict_idx].detection_count = detection_count;
    entries_[evict_idx].rssi_value = rssi_value;
    entries_[evict_idx].timestamp = current_time;
    head_ = (head_ + 1) & DetectionBufferConstants::HASH_MASK;

    tls_recursion_depth--;
}

// IMPLEMENTATION: get_detection_count()
// * * @brief Get detection count (mutex-protected reader)
// * * FIX #RC-2: Full mutex protection (was lock-free)
// * * SYNCHRONIZATION:
// * * - Acquires buffer_mutex_ for exclusive access
// * * - Ensures consistent read of entry state
// * * - Eliminates torn reads and inconsistent state
// * * @param frequency_hash Hash of frequency to query
// * @return Detection count (0 if not found)
DetectionCount DetectionRingBuffer::get_detection_count(FrequencyHash frequency_hash) const noexcept {
    // FIX #RC-2: Full mutex protection (was lock-free)
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Hash table lookup
    const size_t hash_idx = hash_index(frequency_hash);

    // Linear probe (bounded by HASH_TABLE_SIZE)
    for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
        const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;

        // Check for matching entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].detection_count;
        }

        // Check for empty slot (not found)
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            return 0;
        }
    }

    return 0;
}

// IMPLEMENTATION: get_rssi_value()
// * * @brief Get RSSI value (mutex-protected reader)
// * * FIX #RC-2: Full mutex protection (was lock-free)
// * * SYNCHRONIZATION:
// * * - Acquires buffer_mutex_ for exclusive access
// * * - Ensures consistent read of entry state
// * * - Eliminates torn reads and inconsistent state
// * * @param frequency_hash Hash of frequency to query
// * @return RSSI value (DEFAULT_RSSI_DBM if not found)
RSSIValue DetectionRingBuffer::get_rssi_value(FrequencyHash frequency_hash) const noexcept {
    // FIX #RC-2: Full mutex protection (was lock-free)
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Hash table lookup
    const size_t hash_idx = hash_index(frequency_hash);

    // Linear probe (bounded by HASH_TABLE_SIZE)
    for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
        const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;

        // Check for matching entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].rssi_value;
        }

        // Check for empty slot (not found)
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            return DetectionBufferConstants::DEFAULT_RSSI_DBM;
        }
    }

    return DetectionBufferConstants::DEFAULT_RSSI_DBM;
}

// IMPLEMENTATION: clear()
// * * @brief Clear all entries (mutex-protected)
// * * Resets all entries to empty state and resets head index.
// * * SYNCHRONIZATION:
// * * - Acquires buffer_mutex_ for exclusive access
// * * - Increments global_version_ for concurrent update detection
// * * - Resets head_ with release semantics
void DetectionRingBuffer::clear() noexcept {
    // DIAMOND FIX #2-3: Replace OrderedScopedLock with MutexLock from eda_locking.hpp
    // Lock order: DATA_MUTEX (level 1) for detection data and frequency database
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Increment global version
    global_version_++;

    // Clear all entries
    for (auto& entry : entries_) {
        init_entry(entry, global_version_);
    }

    // Reset head index
    head_ = 0;
}

// ============================================================================
// MEMORY POOL FACTORY METHOD IMPLEMENTATIONS
// ============================================================================

/**
 * @brief Factory method to create DetectionRingBuffer from memory pool
 * @return Pointer to allocated DetectionRingBuffer, or nullptr if pool exhausted
 * @note Thread-safe (mutex-protected)
 * @note Returns nullptr if pool is exhausted (overflow protection)
 * @note Caller is responsible for deallocating using MemoryPoolManager::deallocate()
 *
 * MEMORY POOL INTEGRATION:
 * - This method allocates DetectionRingBuffer from memory pool instead of stack
 * - Prevents stack overflow when creating DetectionRingBuffer instances
 * - Uses RAII wrapper for automatic deallocation (recommended)
 * - Thread-safe allocation and deallocation
 */
DetectionRingBuffer* DetectionRingBuffer::allocate_from_pool() noexcept {
    // Allocate from memory pool
    void* ptr = ui::apps::enhanced_drone_analyzer::MemoryPoolManager::allocate(PoolType::DETECTION_RING_BUFFER);
    
    // Guard clause: Allocation failed
    if (!ptr) {
        return nullptr;
    }
    
    // Manually construct DetectionRingBuffer in allocated memory
    // This avoids placement new which can cause compiler issues
    DetectionRingBuffer* buffer = static_cast<DetectionRingBuffer*>(ptr);
    
    // Initialize canaries
    buffer->canary_before_ = DetectionRingBuffer::CANARY_VALUE;
    buffer->canary_after_ = DetectionRingBuffer::CANARY_VALUE;
    
    // Initialize other members
    buffer->head_ = 0;
    buffer->global_version_ = 0;
    
    // Initialize mutex using ChibiOS API
    chMtxInit(&buffer->buffer_mutex_);
    
    // Initialize all entries
    for (size_t i = 0; i < DetectionBufferConstants::MAX_ENTRIES; ++i) {
        buffer->init_entry(buffer->entries_[i], 0);
    }
    
    return buffer;
}

/**
 * @brief Deallocate DetectionRingBuffer back to memory pool
 * @param ptr Pointer to DetectionRingBuffer to deallocate
 * @note Thread-safe (mutex-protected)
 * @note Safe to call with nullptr (no-op)
 *
 * MEMORY POOL INTEGRATION:
 * - This method deallocates DetectionRingBuffer back to memory pool
 * - Calls destructor before deallocating
 * - Thread-safe deallocation
 *
 * NOTE: ChibiOS mutexes do not require explicit deinitialization.
 * The Mutex structure is automatically cleaned up when the object is destroyed.
 */
void DetectionRingBuffer::deallocate_to_pool(DetectionRingBuffer* ptr) noexcept {
    // Guard clause: Null pointer (no-op)
    if (!ptr) {
        return;
    }

    // Call destructor to properly clean up DetectionRingBuffer
    // ChibiOS mutexes do not require explicit deinitialization
    ptr->~DetectionRingBuffer();

    // Deallocate to memory pool
    MemoryPoolManager::deallocate(PoolType::DETECTION_RING_BUFFER, ptr);
}

} // namespace ui::apps::enhanced_drone_analyzer
