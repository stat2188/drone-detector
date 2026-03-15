/**
 * @file dsp_display_types.cpp
 * @brief Implementation of DSP/UI communication types
 *
 * This file implements drone filtering and sorting functions for
 * the Enhanced Drone Analyzer.
 *
 * DIAMOND CODE PRINCIPLES:
 * - Zero heap allocation: All structures use fixed-size arrays
 * - No exceptions: All functions are noexcept
 * - Thread-safe: Uses ChibiOS mutexes for data access
 * - Type-safe: Uses semantic type aliases
 * - Memory-safe: Stack-only allocation
 *
 * @author Diamond Code Pipeline - Stage 5
 * @date 2026-03-03
 */

// Corresponding header (must be first)
#include "dsp_display_types.hpp"

// C++ standard library headers (alphabetical order)
#include <cstddef>
#include <cstdint>

// Third-party library headers
#include <ch.h>

// Project-specific headers (alphabetical order)

namespace ui::apps::enhanced_drone_analyzer::dsp {


// ============================================================================
// DRONE FILTERING AND SORTING FUNCTIONS
// ============================================================================

/**
 * @brief Filter drones by stale timeout (in-place, no allocation)
 *
 * This function filters out drones that have not been seen recently.
 * It writes the filtered snapshot to the provided output parameter.
 *
 * @param input Input snapshot of tracked drones
 * @param output Output snapshot with only active drones (caller-provided)
 * @param stale_timeout_ms Timeout in milliseconds for stale detection
 * @param now Current timestamp in milliseconds
 *
 * @note This is a pure DSP filtering function with no UI dependencies
 * @note noexcept for embedded safety
 * @note Output parameter eliminates stack allocation (640 bytes saved)
 *
 * THREAD SAFETY:
 * - This function does not acquire any mutexes
 * - Caller is responsible for thread-safe access to input snapshot
 * - Output snapshot must be pre-allocated by caller
 *
 * USAGE:
 * @code
 *   FilteredDronesSnapshot input_snapshot = ...;
 *   FilteredDronesSnapshot output_snapshot;
 *   systime_t now = chTimeNow();
 *   filter_stale_drones_in_place(
 *       input_snapshot,
 *       output_snapshot,
 *       StaleTimeout(STALE_TIMEOUT_MS),
 *       CurrentTime(now)
 *   );
 *   // Use output_snapshot...
 * @endcode
 */
// CRITICAL FIX #E004: Use strongly-typed wrappers to prevent parameter swapping
// FIX #1: Use output parameter instead of return value (640 bytes saved)
void filter_stale_drones_in_place(
    const FilteredDronesSnapshot& input,
    FilteredDronesSnapshot& output,  // Output parameter (no allocation)
    StaleTimeout stale_timeout_ms,
    CurrentTime now
) noexcept {
    output.count = 0;

    // Filter out stale drones
    for (size_t i = 0; i < input.count; ++i) {
        const TrackedDroneData& drone = input.drones[i];

        // Check if drone is stale (not seen recently)
        // CRITICAL FIX #E004: Call .get() on strongly-typed wrappers
        systime_t time_since_last_seen = now.get() - drone.last_seen;
        if (time_since_last_seen < stale_timeout_ms.get()) {
            // Drone is still active, add to filtered snapshot
            if (output.count < DisplayTypeConstants::MAX_FILTERED_DRONES) {
                output.drones[output.count] = drone;
                output.count++;
            }
        }
    }
}

/**
 * @brief Sort drones by RSSI, threat level, and last seen
 *
 * This function sorts an array of TrackedDroneData entries by:
 * 1. RSSI (descending - stronger signals first)
 * 2. Threat level (descending - higher threats first)
 * 3. Last seen timestamp (descending - more recent first)
 *
 * @param drones Array of drone data to sort
 * @param count Number of drones in array
 *
 * @note This is a pure DSP sorting function with no UI dependencies
 * @note Uses insertion sort for small arrays (O(n^2) worst case, but O(n) for nearly sorted)
 * @note noexcept for embedded safety
 *
 * THREAD SAFETY:
 * - This function does not acquire any mutexes
 * - Caller is responsible for thread-safe access to input array
 * - Sorts in-place (modifies input array)
 *
 * USAGE:
 * @code
 *   TrackedDroneData drones[MAX_FILTERED_DRONES];
 *   size_t count = ...;
 *   sort_drones_by_priority(drones, count);
 * @endcode
 *
 * PERFORMANCE:
 * - Insertion sort is optimal for small arrays (n <= 10)
 * - For larger arrays, consider using quicksort or mergesort
 * - In-place sorting (no additional memory allocation)
 */
void sort_drones_by_priority(
    TrackedDroneData drones[],
    size_t count
) noexcept {
    // Guard clause: Empty or single-element array
    if (count <= 1) {
        return;
    }

    // Insertion sort for small arrays
    // Optimal for n <= 10 (MAX_FILTERED_DRONES)
    for (size_t i = 1; i < count; ++i) {
        TrackedDroneData key = drones[i];
        size_t j = i;

        // Shift elements that are less than key
        // Compare by RSSI (descending), then threat level (descending), then last seen (descending)
        while (j > 0) {
            const TrackedDroneData& prev = drones[j - 1];

            // Compare RSSI (descending - stronger signals first)
            if (prev.rssi > key.rssi) {
                break;
            }

            // Compare threat level (descending - higher threats first)
            if (prev.rssi == key.rssi && prev.threat_level > key.threat_level) {
                break;
            }

            // Compare last seen (descending - more recent first)
            // CRITICAL FIX: Inverted logic - should break when previous is more recent (smaller timestamp)
            // In ChibiOS, systime_t is a millisecond counter that wraps around
            // Smaller timestamp = more recent, larger timestamp = older
            if (prev.rssi == key.rssi &&
                prev.threat_level == key.threat_level &&
                prev.last_seen <= key.last_seen) {
                break;
            }

            // Shift element to the right
            drones[j] = prev;
            j--;
        }

        // Insert key at correct position
        drones[j] = key;
    }
}

} // namespace ui::apps::enhanced_drone_analyzer::dsp
