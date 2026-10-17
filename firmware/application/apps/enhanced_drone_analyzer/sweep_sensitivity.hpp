#ifndef SWEEP_SENSITIVITY_HPP
#define SWEEP_SENSITIVITY_HPP

#include <cstdint>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Single-source cheap pre-gate before heavy shape/CFAR/kurtosis.
 * @param shape_detect_on ScanConfig::spectrum_detection_enabled snapshot.
 *        FALSE (RSSI-only mode) => ALWAYS analyze: the shape margin does
 *        not apply there — threshold tracking owns every frame (parity
 *        with the detector-side bypass in process_spectrum_sweep).
 * @param peak_power Max usable-bin power of the slice (already scanned).
 * @param noise_floor Noise shelf of the SAME slice: clamp(min + off) at the UI
 *        drain level, clamp(p25 + off) inside the detector — BOTH carry the
 *        SAME ScanConfig::sweep_shelf_offset (SWEEP_SHELF_OFFSET_*). The clamp
 *        is monotone, so min <= p25 implies clamp(min+off) <= clamp(p25+off)
 *        and the UI gate is a strict SUBSET of the detector gate for EVERY
 *        offset (with off = 0 this reduces to min <= p25): it can never drop
 *        a frame the detector would have accepted.
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

/**
 * @brief TBD confirmation threshold in raw spectrum.db units (RSSI term only).
 * @param rssi_threshold_dbm ScanConfig::rssi_threshold_dbm (Sens-derived).
 *        DB scan passes its HYSTERESIS-ADJUSTED local value so the DB confirm
 *        gate keeps its ±RSSI_HYSTERESIS_DB behaviour byte-for-byte.
 * @param total_gain_db LNA + VGA (+ RF amp) of the SAME frame.
 * @return clamp((rssi_threshold_dbm + total_gain_db) * 5 + 255, 0, 255) —
 *         inverse of spectrum_value_to_dbm() (dBm = (v-255)/5 - gain), i.e.
 *         the raw value a bin must reach to count as a TBD vote.
 * @note SINGLE SOURCE OF TRUTH: the DB confirm gate, the sweep confirm gate
 *       and the UI drain prefilter all consumed this derivation separately
 *       (three copies); they now share this function.
 * @note The result is an ABSOLUTE level and is NOT usable as a weak-signal
 *       criterion on its own — it clamps to 0 for any total gain <= 54 dB at
 *       the default Sens (-105 dBm). Always pass it through
 *       sweep_tbd_effective_threshold() before gating anything.
 * Stack: 0 B (inlined). Flash: ~32 B. SRAM: 0. No heap, no FP, no locks.
 */
[[nodiscard]] inline uint8_t sweep_tbd_threshold_units(
    int32_t rssi_threshold_dbm, int32_t total_gain_db) noexcept {
    const int32_t raw = (rssi_threshold_dbm + total_gain_db) * 5 + 255;
    return static_cast<uint8_t>((raw < 0) ? 0 : ((raw > 255) ? 255 : raw));
}

/**
 * @brief TBD confirm threshold WITH the noise-relative floor applied.
 * @param rssi_threshold_units Output of sweep_tbd_threshold_units().
 * @param noise_floor Current frame's 25th-percentile shelf (raw units).
 * @return max(rssi_threshold_units, noise_floor + TBD_MIN_ELEVATION_UNITS),
 *         saturated to 255.
 * @note This floor is what keeps "confirmed in TBD_MIN_FRAMES of the window"
 *       a NOISE-RELATIVE statement. Without it the absolute RSSI term
 *       collapses to 0 (see sweep_tbd_threshold_units) and any non-zero bin
 *       counts as a vote — the discrimination hole that the frame-drop
 *       prefilter used to mask by discarding the frame outright.
 * @note MUST be consumed by every TBD decision (DB confirm, sweep confirm,
 *       frame reachability) so all three agree by construction.
 * @note uint16 arithmetic: noise_floor (<=255) + floor (6) cannot wrap; the
 *       saturation is defense-in-depth for a future floor increase.
 * Stack: 0 B (inlined). Flash: ~24 B. SRAM: 0. No heap, no FP, no locks.
 */
[[nodiscard]] inline uint8_t sweep_tbd_effective_threshold(
    uint8_t rssi_threshold_units, uint8_t noise_floor) noexcept {
    const uint16_t floor_sum = static_cast<uint16_t>(noise_floor) +
        static_cast<uint16_t>(TBD_MIN_ELEVATION_UNITS);
    const uint8_t floor_units =
        (floor_sum > 255) ? 255 : static_cast<uint8_t>(floor_sum);
    return (rssi_threshold_units > floor_units) ? rssi_threshold_units
                                                : floor_units;
}

/**
 * @brief Exact (provably sensitivity-neutral) TBD reachability of one frame.
 * @details A TBD confirmation requires a PRESENT-NOW vote from this very frame
 *   (spectrum.db[tbd_peak_bin] >= threshold), and spectrum.db[b] never exceeds
 *   the frame's usable-bin peak. A frame whose peak lies below the effective
 *   threshold therefore cannot satisfy present-now, cannot contribute a vote
 *   to count_above_threshold() either, and may be discarded whole: skipping
 *   the integration pass for it changes no decision. This is an EQUIVALENCE,
 *   not a heuristic — it is the only admissible way to cut a frame.
 * @param shape_detect_on ScanConfig::spectrum_detection_enabled snapshot.
 *        FALSE (RSSI-only mode) => never gate: threshold tracking owns it.
 * @param frame_peak_power Max usable-bin power of the slice (already scanned).
 * @param noise_floor Noise shelf of the SAME slice: clamp(min + off) at the UI
 *        drain level, clamp(p25 + off) inside the detector — SAME
 *        SWEEP_SHELF_OFFSET on both sides (see sweep_fast_prefilter above), so
 *        the monotone clamp preserves min <= p25 and the UI-side test is a
 *        strict SUBSET of the detector-side one for EVERY offset: it can never
 *        drop a frame the detector would have offered to TBD.
 * @param rssi_threshold_units Output of sweep_tbd_threshold_units(); 0 on
 *        mutex contention (fail-open: the noise floor still applies).
 * @return true iff the frame must still be offered to the TBD integrator.
 * Stack: 0 B (inlined). Flash: ~32 B. SRAM: 0. No heap, no FP, no locks.
 */
[[nodiscard]] inline bool sweep_tbd_frame_reachable(
    bool shape_detect_on,
    uint8_t frame_peak_power,
    uint8_t noise_floor,
    uint8_t rssi_threshold_units) noexcept {
    if (!shape_detect_on) return true;        // RSSI-only: never gate
    if (frame_peak_power == 0) return false;  // dead frame: nothing to integrate
    return frame_peak_power >=
        sweep_tbd_effective_threshold(rssi_threshold_units, noise_floor);
}

}  // namespace drone_analyzer

#endif  // SWEEP_SENSITIVITY_HPP
