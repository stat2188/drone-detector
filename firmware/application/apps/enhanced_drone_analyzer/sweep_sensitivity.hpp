#ifndef SWEEP_SENSITIVITY_HPP
#define SWEEP_SENSITIVITY_HPP

#include <cstdint>
#include <cstddef>

#include <array>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Non-coherent multi-pass RF-pixel integrator for sweep mode.
 * @details Fixes structural TBD blindness: WaterfallHistory resets on
 *   every 8.83 MHz hop (TOL 4.4 MHz), so weak signals need ~3 passes
 *   (~9-36 s) to confirm. This integrator accumulates EVERY live slice
 *   at its TRUE RF pixel ((freq-f_min)*240/range, same map as
 *   SweepProcessor::process_frame) and NEVER resets on hop.
 * @note SRAM: 720 B BSS (240x u16 + 240x u8). One per window (pixels
 *   are window-relative) or reset() on window switch.
 * @note Stack: 0 B lifetime, <= 24 B per accumulate(). UI-thread only.
 * @invariant energy_[px] <= ENERGY_SAT, hits_[px] <= HIT_MAX.
 */
class SweepPixelIntegrator {
public:
    static constexpr uint16_t ENERGY_SAT = 64000;
    static constexpr uint8_t HIT_CONFIRM = 2;
    static constexpr uint8_t HIT_MAX = 254;
    static constexpr uint8_t DECAY_SHIFT = 2;

    constexpr SweepPixelIntegrator() noexcept = default;

    SweepPixelIntegrator(const SweepPixelIntegrator&) = delete;
    SweepPixelIntegrator& operator=(const SweepPixelIntegrator&) = delete;

    uint16_t accumulate(
        const uint8_t* powers,
        FreqHz f_center,
        FreqHz f_min,
        FreqHz range) noexcept {
        if (powers == nullptr) return 0;
        if (range == 0 || f_center == 0) return 0;
        // DETECTOR-USABLE bin ranges (CFAR/shape domain), NOT the
        // display-paint range 2..119 / 136..253 of SweepProcessor::
        // process_frame: edge bins 0..5 and 250..255 are excluded
        // everywhere the detection chain runs, so they must not vote
        // here either (parity with process_spectrum_sweep Step 1).
        const auto first_hi = static_cast<uint16_t>(FFT_EDGE_SKIP_NARROW);
        const auto last_hi =
            static_cast<uint16_t>(FFT_DC_SPIKE_START - 1);
        const auto first_lo = static_cast<uint16_t>(FFT_DC_SPIKE_END);
        const auto last_lo = static_cast<uint16_t>(
            FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW - 1);
        uint16_t touched = accumulate_run(powers, f_center, f_min, range,
                                          first_hi, last_hi,
                                          SWEEP_BIAS_UPPER);
        touched = static_cast<uint16_t>(touched + accumulate_run(
            powers, f_center, f_min, range,
            first_lo, last_lo, SWEEP_BIAS_LOWER));
        return touched;
    }

    [[nodiscard]] bool is_ready(uint16_t px) const noexcept {
        if (px >= COMPOSITE_SIZE) return false;
        return hits_[px] >= HIT_CONFIRM;
    }

    [[nodiscard]] uint16_t energy(uint16_t px) const noexcept {
        if (px >= COMPOSITE_SIZE) return 0;
        return energy_[px];
    }

    void decay_pass() noexcept {
        for (uint16_t i = 0; i < COMPOSITE_SIZE; ++i) {
            energy_[i] >>= DECAY_SHIFT;
            if (hits_[i] > 0) --hits_[i];
        }
    }

    void reset() noexcept {
        energy_.fill(0);
        hits_.fill(0);
    }

private:
    std::array<uint16_t, COMPOSITE_SIZE> energy_{};
    std::array<uint8_t, COMPOSITE_SIZE> hits_{};

    [[gnu::hot]] uint16_t accumulate_run(
        const uint8_t* powers,
        FreqHz f_center,
        FreqHz f_min,
        FreqHz range,
        uint16_t bin_first,
        uint16_t bin_last,
        uint32_t bias) noexcept {
        if (bin_first > bin_last || bin_last >= FFT_BIN_COUNT) return 0;
        const uint64_t bias_hz = static_cast<uint64_t>(bias) * SWEEP_BIN_SIZE;
        const uint64_t first_hz =
            static_cast<uint64_t>(f_center) +
            static_cast<uint64_t>(bin_first) * SWEEP_BIN_SIZE;
        if (first_hz < bias_hz) return 0;
        const uint64_t base = static_cast<uint64_t>(f_center) - bias_hz;
        uint16_t n = 0;
        for (uint16_t bin = bin_first; bin <= bin_last; ++bin) {
            const uint8_t pw = powers[bin];
            if (pw == 0) continue;
            const uint64_t freq =
                base + static_cast<uint64_t>(bin) * SWEEP_BIN_SIZE;
            if (freq < f_min) continue;
            const uint64_t off = freq - f_min;
            const uint32_t px =
                static_cast<uint32_t>((off * COMPOSITE_SIZE) / range);
            if (px >= COMPOSITE_SIZE) continue;
            const uint32_t add = static_cast<uint32_t>(energy_[px]) + pw;
            energy_[px] = (add > ENERGY_SAT)
                ? ENERGY_SAT
                : static_cast<uint16_t>(add);
            if (hits_[px] < HIT_MAX) ++hits_[px];
            ++n;
        }
        return n;
    }
};

/**
 * @brief Single-source cheap pre-gate before heavy shape/CFAR/kurtosis.
 * @param shape_detect_on ScanConfig::spectrum_detection_enabled snapshot.
 *        FALSE (RSSI-only mode) => ALWAYS analyze: the shape margin does
 *        not apply there — threshold tracking owns every frame (parity
 *        with the detector-side bypass in process_spectrum_sweep).
 * @param peak_power Max usable-bin power of the slice (already scanned).
 * @param noise_floor Noise shelf of the SAME slice: min at the UI drain
 *        level, 25th-percentile inside the detector. min <= p25 always
 *        holds, so the UI gate is a strict SUBSET of the detector gate —
 *        it can never drop a frame the detector would have accepted.
 * @param shape_margin Full Step-3 gate (shape_gate_margin()); 0 on mutex
 *        contention or when the user disabled the margin => fail-open.
 * @return true iff slice deserves heavy analysis or TBD integration.
 * @details Threshold is HALF the shape gate (~30 cycles vs ~2000+ for
 *   the full chain): genuine weak (below-gate TBD) targets stay alive
 *   and go to the integrator; only dead-flat noise is cut early.
 * @note Byte-parity with the former inline detector gate: margin 0 or 1
 *   (sensitive floor) => gate_half 0 => everything passes.
 * @note Stack: ~8 B. Flash: ~64 B. No heap, no FP, no locks.
 */
[[nodiscard]] inline bool sweep_fast_prefilter(
    bool shape_detect_on,
    uint8_t peak_power,
    uint8_t noise_floor,
    uint8_t shape_margin) noexcept {
    if (!shape_detect_on) return true;  // RSSI-only: never gate
    const uint8_t margin = (peak_power > noise_floor)
        ? static_cast<uint8_t>(peak_power - noise_floor)
        : 0;
    // shape_margin 0 => disabled / contention fail-open (0/2 = 0,
    // margin >= 0 always true). shape_margin 1 => half = 0 => passes.
    return margin >= static_cast<uint8_t>(shape_margin / 2U);
}

}  // namespace drone_analyzer

#endif  // SWEEP_SENSITIVITY_HPP
