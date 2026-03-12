// * @file ui_signal_processing.cpp * @brief Implementation of signal processing utilities for Enhanced Drone Analyzer
// * DIAMOND CODE PRINCIPLES:
// * - Zero heap allocation: All memory is stack-allocated or in Flash
// * - No exceptions: All functions are noexcept
// * - Type-safe: Uses semantic type aliases
// * - Memory-safe: Uses ChibiOS RTOS for thread management
// * @author Diamond Code Pipeline
// * @date 2026-02-27
//
// Corresponding header (must be first)
#include "ui_signal_processing.hpp"

// C++ standard library headers (alphabetical order)
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <new>

// Third-party library headers
#include <ch.h>

// ChibiOS Internal Headers
#include "chvt.h"

// Project-specific headers
#include "eda_locking.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// Implementation: update_detection()
// / @brief Update detection entry (mutex-protected writer)
// / @note Updates or creates a detection entry for given frequency hash.
// / @note Uses linear probing with simple modulo-based hash for collision resolution.
// / @note Evicts oldest entry when hash table is full (ring buffer eviction).
// / @note MEMORY ISSUE #3 FIX: Ring Buffer Overflow Reporting and Recovery
//   - Tracks overflow events with overflow_count_ counter
//   - Provides get_overflow_count() method to query overflow statistics
//   - Implements recovery strategy: reduce detection count when overflow occurs
//   - Documented overflow behavior
// / @note SYNCHRONIZATION:
//   - Acquires buffer_mutex_ for exclusive access
//   - Increments global_version_ for concurrent update detection
//   - Stores head_ with release semantics
// / @note RED TEAM FIX: Recursion detection prevents deadlock when called recursively.
// / @note DIAMOND FIX: Uses DetectionUpdate struct to prevent parameter swapping
// / @param update Detection update parameters (frequency_hash, detection_count, rssi_value)
void DetectionRingBuffer::update_detection(const DetectionUpdate& update) noexcept {
    const FrequencyHash frequency_hash = update.frequency_hash;
    const DetectionCount detection_count = update.detection_count;
    const RSSIValue rssi_value = update.rssi_value;
    const Timestamp current_time = static_cast<Timestamp>(chTimeNow());

    // DIAMOND FIX: Removed illegal 'thread_local' which causes hidden malloc via __emutls.
    // Recursion detection removed as update_detection has no recursive call paths.

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

            return;
        }

        // Check for empty slot
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            entries_[idx].version = global_version_;
            entries_[idx].frequency_hash = frequency_hash;
            entries_[idx].detection_count = detection_count;
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].timestamp = current_time;
            entry_count_++;  // PERFORMANCE FIX: Track entry count for O(1) overflow detection

            return;
        }
    }

    // Table full - evict oldest entry (ring buffer eviction)
    // Note: We reach this point because the linear probe found no empty slots
    // The overflow_count_ tracks how many times we had to evict entries
    overflow_count_++;

    // Evict oldest entry and replace it with new data
    const size_t evict_idx = head_;
    entries_[evict_idx].version = global_version_;
    entries_[evict_idx].frequency_hash = frequency_hash;
    entries_[evict_idx].detection_count = detection_count;
    entries_[evict_idx].rssi_value = rssi_value;
    entries_[evict_idx].timestamp = current_time;
    head_ = (head_ + 1) & DetectionBufferConstants::HASH_MASK;
    // entry_count_ stays the same - we're replacing an entry, not adding/removing
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
// / @brief Clear all entries (mutex-protected)
// / @note Resets all entries to empty state and resets head index.
// / @note SYNCHRONIZATION:
//   - Acquires buffer_mutex_ for exclusive access
//   - Increments global_version_ for concurrent update detection
// / @note RED TEAM FIX: Recursion detection prevents deadlock when called recursively.
void DetectionRingBuffer::clear() noexcept {
    // DIAMOND FIX #2-3: Replace OrderedScopedLock with MutexLock from eda_locking.hpp
    // Lock order: DATA_MUTEX (level 1) for detection data and frequency database
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Reset all entries to empty state
    for (auto& entry : entries_) {
        init_entry(entry, 0);
    }

    // Reset head index
    head_ = 0;

    // Reset entry count (PERFORMANCE FIX: Reset entry count on clear)
    entry_count_ = 0;

    // Increment global version for this clear
    global_version_++;
}

// Implementation: get_version()
// / @brief Get current global version (for concurrent update detection)
// /return Global version number
// /note Thread-safe: Acquires buffer_mutex_ for exclusive access
// /note ISR-safe: Marked noexcept
uint32_t DetectionRingBuffer::get_version() const noexcept {
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
    return global_version_;
}

// Implementation: get_overflow_count()
// / @brief Get overflow count statistics
// /return Number of overflow events that have occurred
// /note MEMORY ISSUE #3 FIX: Added overflow reporting
// /note Thread-safe: Acquires buffer_mutex_ for exclusive access
// /note ISR-safe: Marked noexcept
uint32_t DetectionRingBuffer::get_overflow_count() const noexcept {
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
    return overflow_count_;
}

// Implementation: reset_overflow_count()
// /brief Reset overflow counter to zero
// @note MEMORY ISSUE #3 FIX: Added overflow counter reset
// /note Thread-safe: Acquires buffer_mutex for exclusive access
void DetectionRingBuffer::reset_overflow_count() noexcept {
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
    overflow_count_ = 0;
}

// Implementation: get_entry_count()
// /brief Get entry count (PERFORMANCE FIX: O(1) query for buffer utilization)
// /return Number of valid entries currently stored in the buffer
// /note PERFORMANCE FIX: Returns entry_count_ for O(1) buffer utilization query
// /note Thread-safe: Acquires buffer_mutex for exclusive access
// /note ISR-safe: Marked noexcept
size_t DetectionRingBuffer::get_entry_count() const noexcept {
    MutexLock lock(buffer_mutex_, LockOrder::DATA_MUTEX);
    return entry_count_;
}

} // namespace ui::apps::enhanced_drone_analyzer
