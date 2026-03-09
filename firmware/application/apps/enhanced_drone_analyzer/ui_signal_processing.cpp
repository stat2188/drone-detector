// * @file ui_signal_processing.cpp * @brief Implementation of signal processing utilities for Enhanced Drone Analyzer
// * DIAMOND CODE PRINCIPLES:
// * - Zero heap allocation: All memory is stack-allocated or in Flash
// * - No exceptions: All functions are noexcept
// * - Type-safe: Uses semantic type aliases
// * - Memory-safe: Uses ChibiOS RTOS for thread management
// * @author Diamond Code Pipeline
// * @date 2026-02-27

// Corresponding header (must be first)
#include "ui_signal_processing.hpp"

// C++ standard library headers (alphabetical order)
#include <algorithm>
#include <cstddef>
#include <new>

// Third-party library headers
#include "chmtx.h"
#include "chvt.h"
#include "eda_locking.hpp"

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

            // Decrement thread-local recursion depth
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

    // Decrement thread-local recursion depth
    tls_recursion_depth--;
}

// Implementation: get_rssi_value()
// / @brief Get RSSI value for given frequency hash (mutex-protected reader)
// / @param frequency_hash Frequency hash to look up
// / @return RSSI value in dBm, or default value if not found
// / @note Thread-safe: Acquires buffer_mutex_ for exclusive access
// / @note ISR-safe: Marked noexcept
RSSIValue DetectionRingBuffer::get_rssi_value(FrequencyHash frequency_hash) const noexcept {
    // DIAMOND FIX #2-3: Replace OrderedScopedLock with MutexLock from eda_locking.hpp
    // Lock order: DATA_MUTEX (level 1) for detection data and frequency database
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Hash table lookup (using simple modulo-based hash)
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

    // Not found - return default value
    return DetectionBufferConstants::DEFAULT_RSSI_DBM;
}

// Implementation: get_detection_count()
// / @brief Get detection count for given frequency hash (mutex-protected reader)
// / @param frequency_hash Frequency hash to look up
// / @return Detection count, or 0 if not found
// / @note Thread-safe: Acquires buffer_mutex_ for exclusive access
// / @note ISR-safe: Marked noexcept
DetectionCount DetectionRingBuffer::get_detection_count(FrequencyHash frequency_hash) const noexcept {
    // DIAMOND FIX #2-3: Replace OrderedScopedLock with MutexLock from eda_locking.hpp
    // Lock order: DATA_MUTEX (level 1) for detection data and frequency database
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Hash table lookup (using simple modulo-based hash)
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

    // Not found - return 0
    return 0;
}

// Implementation: get_timestamp()
// / @brief Get timestamp for given frequency hash (mutex-protected reader)
// / @param frequency_hash Frequency hash to look up
// / @return Timestamp of last detection, or 0 if not found
// / @note Thread-safe: Acquires buffer_mutex_ for exclusive access
// / @note ISR-safe: Marked noexcept
Timestamp DetectionRingBuffer::get_timestamp(FrequencyHash frequency_hash) const noexcept {
    // DIAMOND FIX #2-3: Replace OrderedScopedLock with MutexLock from eda_locking.hpp
    // Lock order: DATA_MUTEX (level 1) for detection data and frequency database
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Hash table lookup (using simple modulo-based hash)
    const size_t hash_idx = hash_index(frequency_hash);

    // Linear probe (bounded by HASH_TABLE_SIZE)
    for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
        const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;

        // Check for matching entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            return entries_[idx].timestamp;
        }

        // Check for empty slot (not found)
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            return 0;
        }
    }

    // Not found - return 0
    return 0;
}

// Implementation: clear()
// * @brief Clear all entries (mutex-protected)
// * @note Resets all entries to empty state and resets head index.
// * @note SYNCHRONIZATION:
// *   - Acquires buffer_mutex_ for exclusive access
// *   - Increments global_version_ for concurrent update detection
// *   - Resets head_ with release semantics
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

// Implementation: get_version()
// / @brief Get current global version (for concurrent update detection)
// / @return Global version number
// / @note Thread-safe: Acquires buffer_mutex_ for exclusive access
// / @note ISR-safe: Marked noexcept
uint32_t DetectionRingBuffer::get_version() const noexcept {
    // DIAMOND FIX #2-3: Replace OrderedScopedLock with MutexLock from eda_locking.hpp
    // Lock order: DATA_MUTEX (level 1) for detection data and frequency database
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
    return global_version_;
}

// Implementation: init_entry()
// / @brief Initialize a single entry with default values
// / @param entry Entry to initialize
// / @param version Version number to assign
void DetectionRingBuffer::init_entry(Entry& entry, uint32_t version) noexcept {
    entry.frequency_hash = DetectionBufferConstants::EMPTY_HASH_MARKER;
    entry.rssi_value = DetectionBufferConstants::DEFAULT_RSSI_DBM;
    entry.detection_count = 0;
    entry.timestamp = 0;
    entry.version = version;
}

// Implementation: hash_index()
// / @brief Compute hash index for frequency hash
// / @param frequency_hash Frequency hash value
// / @return Hash index (modulo HASH_TABLE_SIZE)
constexpr size_t DetectionRingBuffer::hash_index(FrequencyHash frequency_hash) noexcept {
    return static_cast<size_t>(frequency_hash) & DetectionBufferConstants::HASH_MASK;
}

// ============================================================================
// NOTE: Memory pool allocation methods have been removed
// ============================================================================
// DetectionRingBuffer should be stack-allocated or use StaticStorage pattern
// Example: DetectionRingBuffer buffer{};  // Stack allocation

} // namespace ui::apps::enhanced_drone_analyzer
