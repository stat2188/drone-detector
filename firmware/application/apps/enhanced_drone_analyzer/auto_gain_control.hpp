#ifndef AUTO_GAIN_CONTROL_HPP
#define AUTO_GAIN_CONTROL_HPP

#include <cstdint>
#include <cstddef>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Automatic gain control feedback loop for HackRF One RF frontend.
 *
 * Monitors spectrum saturation and noise floor to dynamically adjust
 * LNA/VGA/RF-AMP gains. Prevents ADC saturation (intermod products
 * mimicking drone signals) while maintaining maximum sensitivity for
 * weak signal detection.
 *
 * Gain stepping follows hardware constraints:
 * - LNA (MAX2837): 0-40 dB, step 8 dB
 * - VGA (MAX2837): 0-62 dB, step 2 dB
 * - RF AMP (HMC627A): 0 or 14 dB (binary)
 *
 * Rate-limited: max 1 gain change per 500ms to prevent oscillation.
 *
 * SRAM: ~16 bytes (state only)
 * Stack: ~4 bytes per call
 * Flash: ~160 bytes code
 */
class AutoGainControl {
public:
    /**
     * @brief Optimal gain settings computed by AGC.
     */
    struct GainSetting {
        uint8_t lna;       ///< LNA gain (0, 8, 16, 24, 32, 40)
        uint8_t vga;       ///< VGA gain (0-62, step 2)
        bool rf_amp;       ///< RF amplifier enabled (0 or 14 dB)
    };

    /**
     * @brief Compute optimal gain from spectrum data.
     * @param spectrum_256 256-bin FFT data (0-255 power values)
     * @param current_lna Current LNA gain setting
     * @param current_vga Current VGA gain setting
     * @param current_rf_amp Current RF amplifier state
     * @param now_ms Current system time in milliseconds (for rate limiting)
     * @return Recommended gain settings
     *
     * @note Algorithm:
     *   1. Count saturated bins (power ≥ 250) — indicates ADC overflow
     *   2. Count dead bins (power ≤ 5) — indicates insufficient gain
     *   3. If >5% saturated: reduce gain (VGA first, then LNA, then RF amp)
     *   4. If <1% dead AND max_power < 180: increase gain (RF amp, then VGA, then LNA)
     *   5. Rate limit: skip if <500ms since last change
     */
    [[nodiscard]] GainSetting compute_optimal(
        const uint8_t* spectrum_256,
        uint8_t current_lna,
        uint8_t current_vga,
        bool current_rf_amp,
        uint32_t now_ms
    ) noexcept {
        GainSetting result{current_lna, current_vga, current_rf_amp};

        // Rate limiting
        if ((now_ms - last_change_ms_) < MIN_CHANGE_INTERVAL_MS) {
            return result;
        }

        if (spectrum_256 == nullptr) return result;

        // Analyze spectrum
        uint32_t saturated_count = 0;
        uint32_t dead_count = 0;
        uint8_t max_power = 0;

        for (size_t i = FFT_EDGE_SKIP; i < FFT_BIN_COUNT - FFT_EDGE_SKIP; ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            const uint8_t val = spectrum_256[i];
            if (val >= SATURATION_THRESHOLD) saturated_count++;
            if (val <= DEAD_THRESHOLD) dead_count++;
            if (val > max_power) max_power = val;
        }

        const uint32_t usable_bins = FFT_BIN_COUNT
            - (FFT_DC_SPIKE_END - FFT_DC_SPIKE_START)
            - (2 * FFT_EDGE_SKIP);
        const uint32_t sat_percent = (saturated_count * 100) / usable_bins;
        const uint32_t dead_percent = (dead_count * 100) / usable_bins;

        bool changed = false;

        // Action 1: Reduce gain if saturated
        if (sat_percent > SATURATION_PERCENT_THRESHOLD) {
            changed = reduce_gain(result);
        }
        // Action 2: Increase gain if signal too weak
        else if (dead_percent < DEAD_PERCENT_THRESHOLD && max_power < WEAK_SIGNAL_THRESHOLD) {
            changed = increase_gain(result);
        }

        if (changed) {
            last_change_ms_ = now_ms;
        }

        return result;
    }

    /**
     * @brief Reset AGC state (call on mode change or frequency hop).
     */
    void reset() noexcept {
        last_change_ms_ = 0;
    }

private:
    static constexpr uint32_t MIN_CHANGE_INTERVAL_MS = 500;  // Max 1 change per 500ms
    static constexpr uint8_t SATURATION_THRESHOLD = 250;      // Bins at ≥250 = saturated
    static constexpr uint8_t DEAD_THRESHOLD = 5;              // Bins at ≤5 = dead
    static constexpr uint32_t SATURATION_PERCENT_THRESHOLD = 5;  // >5% saturated → reduce
    static constexpr uint32_t DEAD_PERCENT_THRESHOLD = 1;        // <1% dead → can increase
    static constexpr uint8_t WEAK_SIGNAL_THRESHOLD = 180;     // Max power < 180 → weak

    /**
     * @brief Reduce gain by one step (priority: VGA → LNA → RF amp).
     * @return true if gain was changed
     */
    static bool reduce_gain(GainSetting& g) noexcept {
        if (g.vga >= 4) {
            g.vga -= 4;  // Step down 4 dB (2 steps of 2 dB)
            return true;
        }
        if (g.lna >= 8) {
            g.lna -= 8;  // Step down 8 dB
            return true;
        }
        if (g.rf_amp) {
            g.rf_amp = false;  // Disable RF amp (-14 dB)
            return true;
        }
        return false;
    }

    /**
     * @brief Increase gain by one step (priority: RF amp → VGA → LNA).
     * @return true if gain was changed
     */
    static bool increase_gain(GainSetting& g) noexcept {
        if (!g.rf_amp) {
            g.rf_amp = true;  // Enable RF amp (+14 dB)
            return true;
        }
        if (g.vga <= 58) {
            g.vga += 4;  // Step up 4 dB
            return true;
        }
        if (g.lna <= 32) {
            g.lna += 8;  // Step up 8 dB
            return true;
        }
        return false;
    }

    uint32_t last_change_ms_{0};
};

} // namespace drone_analyzer

#endif // AUTO_GAIN_CONTROL_HPP
