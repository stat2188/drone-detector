#ifndef HEAP_GUARD_HPP
#define HEAP_GUARD_HPP

#include <cstddef>

#include "ch.h"

namespace drone_analyzer {

/**
 * @brief Heap pre-flight check before large UI allocations.
 *
 * operator new() (chibios_cpp.cpp) calls chDbgPanic("Out of Memory") when
 * chHeapAlloc() returns nullptr — on device this manifests as a HardFault.
 * This probe converts that fatal path into a recoverable check: allocate the
 * worst-case block, free it immediately, report the result.
 *
 * @note UI-thread only (same thread that performs the subsequent push).
 * @note No retention, no exceptions, no globals. Stack: ~12 bytes.
 *
 * Guard budgets are DELIBERATELY above the estimated real need (view object +
 * Labels/options heap vectors). A false positive merely refuses to open a
 * sub-view (recoverable); a false negative is a HardFault.
 */
// FrequencyKeypadView: std::array<Button, 12> + 4 buttons + Text (~1.6 KB) +
// margin for the nav-stack bookkeeping.
constexpr size_t KEYPAD_HEAP_GUARD_BYTES = 2'600;
// DroneSettingsView: ~35 widgets + original POD members + Labels vector
// (21 entries) + CFAR OptionsField vector (7 entries).
constexpr size_t SETTINGS_HEAP_GUARD_BYTES = 5'200;
// DroneSweepView: object + nested SweepWindowView + windows_[4] + window
// selector vector (4 entries) + range-name options vector (18 entries).
constexpr size_t SWEEP_HEAP_GUARD_BYTES = 4'800;

/**
 * @brief Probe the ChibiOS memcore heap for a contiguous block
 * @param bytes Worst-case contiguous block size required
 * @return true if a contiguous block of `bytes` was available at call time
 * @note chHeapAlloc returning nullptr is NOT a panic — only operator new
 *       panics. This probe never panics.
 */
[[nodiscard]] inline bool heap_can_allocate(const size_t bytes) noexcept {
    void* probe = chHeapAlloc(nullptr, bytes);
    if (probe == nullptr) {
        return false;
    }
    chHeapFree(probe);
    return true;
}

} // namespace drone_analyzer

#endif // HEAP_GUARD_HPP
