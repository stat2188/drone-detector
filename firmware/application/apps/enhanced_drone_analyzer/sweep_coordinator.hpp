#ifndef SWEEP_COORDINATOR_HPP
#define SWEEP_COORDINATOR_HPP

#include <cstdint>
#include <cstddef>
#include <array>

#include "ch.h"
#include "message.hpp"
#include "drone_types.hpp"
#include "constants.hpp"
#include "locking.hpp"
#include "sweep_processor.hpp"

namespace drone_analyzer {

/**
 * @brief Sweep window state for Looking Glass pattern scanning.
 * @note Each window covers a frequency range with configurable step size.
 * @note Pure data structure with no UI dependencies.
 */
struct SweepWindow {
    uint8_t composite[COMPOSITE_SIZE]{};
    FreqHz f_min{0};
    FreqHz f_max{0};
    FreqHz f_center{0};
    FreqHz f_center_ini{0};
    FreqHz pixel_step_hz{0};
    FreqHz step_hz{0};
    FreqHz bins_hz_acc{0};
    FreqHz exceptions[EXCEPTIONS_PER_WINDOW]{};
    FreqHz exception_radius_hz{3000000ULL};
    uint16_t pixel_index{0};
    uint8_t pixel_max{0};
    bool enabled{false};

    void init(FreqHz start, FreqHz end, FreqHz step = 0) noexcept;
    void reset() noexcept;
    void process_bins(const ChannelSpectrum& spectrum) noexcept;
    [[nodiscard]] bool is_exception(FreqHz hz) const noexcept;
};

/**
 * @brief Sweep coordinator — manages Looking Glass sweep state machine.
 * @note Encapsulates all sweep logic: window management, round-robin, display data.
 * @note Thread-safe with atomic guards for mode transitions.
 * @note No UI dependencies — pure business logic.
 */
class SweepCoordinator {
public:
    SweepCoordinator() noexcept;
    ~SweepCoordinator() noexcept;

    SweepCoordinator(const SweepCoordinator&) = delete;
    SweepCoordinator& operator=(const SweepCoordinator&) = delete;
    SweepCoordinator(SweepCoordinator&&) = delete;
    SweepCoordinator& operator=(SweepCoordinator&&) = delete;

    [[nodiscard]] bool is_active() const noexcept { return active_; }

    /**
     * @brief Initialize sweep mode from configuration.
     * @param cfg Scan configuration with sweep ranges.
     * @return true if at least one window was enabled.
     */
    [[nodiscard]] bool start_sweep(const ScanConfig& cfg) noexcept;

    /**
     * @brief Stop sweep mode and disable all windows.
     */
    void stop_sweep() noexcept;

    /**
     * @brief Process incoming spectrum data from baseband.
     * @param spectrum Channel spectrum from baseband.
     * @param current_freq Current tuned frequency when spectrum was captured.
     * @return true if display should be updated.
     * @note Updates internal state and determines if retune is needed.
     */
    [[nodiscard]] bool process_spectrum(const ChannelSpectrum& spectrum, FreqHz current_freq) noexcept;

    /**
     * @brief Get the frequency the next retune should target.
     * @return Frequency for next tune operation, or 0 if sweep complete.
     */
    [[nodiscard]] FreqHz get_retune_frequency() const noexcept;

    /**
     * @brief Get index of currently active sweep window (0-3).
     * @return Active window index.
     */
    [[nodiscard]] uint8_t get_active_window_index() const noexcept { return active_idx_; }

    /**
     * @brief Get current pair index for display (0 or 2).
     * @return Pair first window index.
     */
    [[nodiscard]] uint8_t get_current_pair_index() const noexcept { return pair_idx_; }

    /**
     * @brief Get composite data for display region.
     * @param region Region index (0 or 2 for pair display).
     * @param out_data Output buffer for composite data.
     * @param max_size Maximum buffer size.
     * @return true if region has valid data.
     */
    [[nodiscard]] bool get_composite_data(uint8_t region, uint8_t* out_data, size_t max_size) const noexcept;

    /**
     * @brief Get frequency range for a window.
     * @param idx Window index (0-3).
     * @param out_min Output for start frequency.
     * @param out_max Output for end frequency.
     * @return true if window is enabled.
     */
    [[nodiscard]] bool get_window_range(uint8_t idx, FreqHz& out_min, FreqHz& out_max) const noexcept;

    /**
     * @brief Get histogram data for current pair.
     * @param out_hist Output buffer for histogram data.
     * @param max_size Maximum buffer size.
     * @return Number of histogram entries written.
     */
    [[nodiscard]] size_t get_histogram_data(uint16_t* out_hist, size_t max_size) const noexcept;

    /**
     * @brief Check if sweep mode is in auto progression.
     * @return true if auto mode active.
     */
    [[nodiscard]] bool is_auto_mode() const noexcept { return auto_mode_; }

    /**
     * @brief Set auto mode (for DB-driven sweep).
     * @param auto_mode true for auto progression.
     */
    void set_auto_mode(bool auto_mode) noexcept { auto_mode_ = auto_mode; }

    /**
     * @brief Signal that sweep pair is complete (for triggering exit in auto mode).
     * @return true if full cycle complete (all windows visited).
     */
    [[nodiscard]] bool advance_to_next_pair() noexcept;

private:
    [[nodiscard]] uint8_t pair_first(uint8_t idx) const noexcept;
    void retune_active_window() noexcept;

    std::array<SweepWindow, MAX_SWEEP_WINDOWS> windows_{};
    bool active_{false};
    bool auto_mode_{false};
    uint8_t active_idx_{0};
    uint8_t pair_idx_{0};
    uint8_t pair_complete_count_{0};
    FreqHz last_tuned_freq_{0};
    bool skip_next_fft_{true};
    AtomicFlag transition_guard_;
};

FreqHz SweepWindow_center(SweepWindow& win) noexcept;

} // namespace drone_analyzer

#endif // SWEEP_COORDINATOR_HPP