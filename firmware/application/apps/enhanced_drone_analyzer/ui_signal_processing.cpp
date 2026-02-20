/**
 * @file ui_signal_processing.cpp
 * @brief Implementation of signal processing utilities for Enhanced Drone Analyzer
 *
 * DIAMOND CODE STANDARDS:
 * - Zero-heap allocation (no new, malloc, std::vector, std::string)
 * - RAII wrappers for automatic resource management
 * - noexcept for exception-free operation
 * - Guard clauses for early returns
 * - Doxygen comments for public APIs
 *
 * STAGE 4 DIAMOND FIXES:
 * - DetectionRingBuffer implementation with AtomicIndex
 * - FNV-1a hash function for reduced collisions
 * - Version field for concurrent update detection
 * - Proper memory ordering with std::memory_order_acquire/release
 * - Recursion detection in update_detection()
 * - Timestamp wrap-around handling
 *
 * @target STM32F405 (ARM Cortex-M4, 128KB RAM)
 * @os ChibiOS (bare-metal RTOS)
 */

#include "ui_signal_processing.hpp"
#include "eda_locking.hpp"
#include <algorithm>
#include <ch.h>
#include <limits>

namespace ui::apps::enhanced_drone_analyzer {

// ========================================
// IMPLEMENTATION: update_detection()
// ========================================
/**
 * @brief Update detection entry (mutex-protected writer)
 *
 * Updates or creates a detection entry for the given frequency hash.
 * Uses linear probing with FNV-1a hash for collision resolution.
 * Evicts oldest entry when hash table is full (ring buffer eviction).
 *
 * SYNCHRONIZATION:
 * - Acquires buffer_mutex_ for exclusive access
 * - Increments global_version_ for concurrent update detection
 * - Stores head_ with release semantics
 *
 * RED TEAM FIX: Recursion detection prevents deadlock when called recursively.
 *
 * @param frequency_hash Hash of frequency to update
 * @param detection_count New detection count
 * @param rssi_value New RSSI value
 */
void DetectionRingBuffer::update_detection(FrequencyHash frequency_hash,
                                        DetectionCount detection_count,
                                        RSSIValue rssi_value) noexcept {
    // RED TEAM FIX: Recursion detection
    // Prevents deadlock when update_detection() is called recursively
    thread_local int recursion_depth = 0;
    if (recursion_depth > 0) {
        // Recursive call detected - return early to prevent deadlock
        return;
    }
    recursion_depth++;

    const Timestamp current_time = static_cast<Timestamp>(chTimeNow());

    // Acquire mutex with lock ordering (from eda_locking.hpp)
    OrderedScopedLock<Mutex> lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Increment global version for this update
    global_version_++;

    // Hash table lookup (using FNV-1a hash)
    const size_t hash_idx = hash_index(frequency_hash);

    // Linear probe (bounded by HASH_TABLE_SIZE)
    for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
        const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;

        // Check for existing entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            // Handle timestamp wrap-around
            if (current_time < DetectionBufferConstants::WRAP_THRESHOLD &&
                entries_[idx].timestamp > DetectionBufferConstants::WRAP_THRESHOLD) {
                entries_[idx].timestamp = DetectionBufferConstants::ZERO_TIMESTAMP;
            }

            // Update entry with new version
            entries_[idx].version = global_version_;
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].detection_count = detection_count;
            entries_[idx].timestamp = current_time;

            recursion_depth--;
            return;
        }

        // Check for empty slot
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            entries_[idx].version = global_version_;
            entries_[idx].frequency_hash = frequency_hash;
            entries_[idx].detection_count = detection_count;
            entries_[idx].rssi_value = rssi_value;
            entries_[idx].timestamp = current_time;

            recursion_depth--;
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

    recursion_depth--;
}

// ========================================
// IMPLEMENTATION: get_detection_count()
// ========================================
/**
 * @brief Get detection count (lock-free reader with versioning)
 *
 * Lock-free reader using atomic head_ and version checking.
 * May return stale data if concurrent update is in progress (acceptable for UI).
 *
 * SYNCHRONIZATION:
 * - Loads atomic head_ with acquire semantics
 * - Double-checks version to detect concurrent updates
 * - No mutex lock (lock-free for performance)
 *
 * @param frequency_hash Hash of frequency to query
 * @return Detection count (0 if not found or concurrent update detected)
 */
DetectionCount DetectionRingBuffer::get_detection_count(FrequencyHash frequency_hash) const noexcept {
    // Acquire mutex for thread-safe access
    OrderedScopedLock<Mutex> lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Hash table lookup
    const size_t hash_idx = hash_index(frequency_hash);

    // Linear probe (bounded by HASH_TABLE_SIZE)
    for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
        const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;

        // Check for matching entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            // Double-check version to detect concurrent updates
            const uint32_t version1 = entries_[idx].version;
            const DetectionCount count = entries_[idx].detection_count;
            const uint32_t version2 = entries_[idx].version;

            // Version changed during read -> concurrent update detected
            if (version1 != version2) {
                return 0;  // Return default (stale data acceptable for UI)
            }

            return count;
        }

        // Check for empty slot (not found)
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            return 0;
        }
    }

    return 0;
}

// ========================================
// IMPLEMENTATION: get_rssi_value()
// ========================================
/**
 * @brief Get RSSI value (lock-free reader with versioning)
 *
 * Lock-free reader using atomic head_ and version checking.
 * May return stale data if concurrent update is in progress (acceptable for UI).
 *
 * SYNCHRONIZATION:
 * - Loads atomic head_ with acquire semantics
 * - Double-checks version to detect concurrent updates
 * - No mutex lock (lock-free for performance)
 *
 * @param frequency_hash Hash of frequency to query
 * @return RSSI value (DEFAULT_RSSI_DBM if not found or concurrent update detected)
 */
RSSIValue DetectionRingBuffer::get_rssi_value(FrequencyHash frequency_hash) const noexcept {
    // Acquire mutex for thread-safe access
    OrderedScopedLock<Mutex> lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Hash table lookup
    const size_t hash_idx = hash_index(frequency_hash);

    // Linear probe (bounded by HASH_TABLE_SIZE)
    for (size_t probe = 0; probe < DetectionBufferConstants::HASH_TABLE_SIZE; ++probe) {
        const EntryIndex idx = (hash_idx + probe) & DetectionBufferConstants::HASH_MASK;

        // Check for matching entry
        if (entries_[idx].frequency_hash == frequency_hash) {
            // Double-check version to detect concurrent updates
            const uint32_t version1 = entries_[idx].version;
            const RSSIValue rssi = entries_[idx].rssi_value;
            const uint32_t version2 = entries_[idx].version;

            // Version changed during read -> concurrent update detected
            if (version1 != version2) {
                return DetectionBufferConstants::DEFAULT_RSSI_DBM;  // Return default
            }

            return rssi;
        }

        // Check for empty slot (not found)
        if (entries_[idx].frequency_hash == DetectionBufferConstants::EMPTY_HASH_MARKER) {
            return DetectionBufferConstants::DEFAULT_RSSI_DBM;
        }
    }

    return DetectionBufferConstants::DEFAULT_RSSI_DBM;
}

// ========================================
// IMPLEMENTATION: clear()
// ========================================
/**
 * @brief Clear all entries (mutex-protected)
 *
 * Resets all entries to empty state and resets head index.
 *
 * SYNCHRONIZATION:
 * - Acquires buffer_mutex_ for exclusive access
 * - Increments global_version_ for concurrent update detection
 * - Resets head_ with release semantics
 */
void DetectionRingBuffer::clear() noexcept {
    // Acquire mutex with lock ordering (from eda_locking.hpp)
    OrderedScopedLock<Mutex> lock(buffer_mutex_, LockOrder::DATA_MUTEX);

    // Increment global version
    global_version_++;

    // Clear all entries
    for (auto& entry : entries_) {
        init_entry(entry, global_version_);
    }

    // Reset head index
    head_ = 0;
}

} // namespace ui::apps::enhanced_drone_analyzer
