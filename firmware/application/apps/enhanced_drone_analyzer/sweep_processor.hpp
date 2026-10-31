#ifndef SWEEP_PROCESSOR_HPP
#define SWEEP_PROCESSOR_HPP

#include <cstdint>
#include <cstddef>
#include "drone_types.hpp"
#include "constants.hpp"
// Forward declaration ONLY — ChannelSpectrum is defined in message.hpp (which
// pulls ch.h). Keeping this header free of that include makes the pure gate
// logic host-testable (firmware/test/enhanced_drone_analyzer compiles this
// header with plain g++, no ChibiOS): process_frame()'s parameter is a
// reference, so the declaration suffices here; sweep_processor.cpp includes
// message.hpp for the definition it dereferences.
struct ChannelSpectrum;

namespace drone_analyzer {

/**
 * @brief Maps FFT bins to composite sweep pixels (frequency-ascending order).
 * @note Pure function — no UI dependencies, no state, no heap.
 * @note Iterates bins in frequency-ascending order for correct sweep composite:
 * @note   bin 0..117   → FFT bins 2..119   (upper sideband, lower frequencies)
 * @note   bin 118..119 → FFT bins 134..135 (DC spike — skipped entirely, no Hz)
 * @note   bin 120..237 → FFT bins 136..253 (lower sideband, higher frequencies)
 * @note   bin 238..239 unused (end of slice)
 * @note M0 baseband handles FFT; M4 only maps bins to pixels.
 */
class SweepProcessor {
public:
    static constexpr uint8_t UPPER_PIXEL_END = SWEEP_PIXELS_PER_SLICE - 2;

    /**
     * @brief Process one FFT frame into a composite pixel buffer.
     * @param spectrum       256-bin FFT power values from baseband
     * @param composite      Output pixel buffer (COMPOSITE_SIZE bytes)
     * @param pixel_index    In/out: progress position (scan-head marker +
     *                       line_full completion bookkeeping). Advanced by the
     *                       internal accumulator, NOT by data placement.
     * @param pixel_max      Unused since true-position painting (kept for API
     *                       stability; the old sequential flush used it).
     * @param bins_hz_acc    In/out: accumulated Hz remainder (integer remainder
     *                       prevents drift) driving pixel_index.
     * @param pixel_step_hz  Hz per pixel on the window scale (range / 240)
     * @param f_center       FFT slice center frequency (Hz)
     * @param effective_bin_size Hz contributed per FFT bin to the progress
     *        accumulator. Must equal step_hz / FFT_SWEEP_USABLE_BINS (236 =
     *        the bins this function visits: {2..119} ∪ {136..253} =
     *        256 FFT bins − 4 edge − 16 DC-spike; equivalently the 240
     *        composite pixels − 2 DC-spike pixels − 2 unused tail pixels,
     *        mapped 1:1 to bins) so each slice advances the accumulator by
     *        exactly step_hz. DC spike bins are skipped (no Hz, no power).
     * @param f_min          Window lower bound (Hz) — linear scale anchor
     * @param f_max          Window upper bound (Hz) — linear scale anchor
     * @return Updated pixel_index
     * @note The spectrum is drawn FULLY — the old exception-radius pixel
     *       masking was removed (detection-window gating lives exclusively in
     *       the scanner, see DroneScanner::is_detection_window_allowed()).
     * @note DATA PLACEMENT — TRUE POSITION, not sequential: every bin is
     *       written at composite[(freq - f_min) * 240 / range] — the exact
     *       pixel its RF frequency occupies on the linear window scale, i.e.
     *       the same mapping the band title (f_min..f_max) and every tracked
     *       drone's frequency use. The previous sequential placement squeezed
     *       each slice's ~20 MHz of RF content into its step_hz pixel slot
     *       (~2.4x compression at the gapless 8.83 MHz step), drawing peaks at
     *       columns that did not match the tracked frequency, and duplicating
     *       every signal once per overlapping slice (ghost peaks).
     * @note Overlapping slices (gapless step < slice RF span) now write the
     *       SAME column — max-hold (composite[px] = max(old, power)) makes
     *       them reinforce instead of ghosting, and the 1.8 MHz DC notch of
     *       slice k is covered by slices k-1/k+1. Stack: ~16 bytes.
     */
    static uint16_t process_frame(
        const ChannelSpectrum& spectrum,
        uint8_t* composite,
        uint16_t& pixel_index,
        uint8_t& pixel_max,
        FreqHz& bins_hz_acc,
        FreqHz pixel_step_hz,
        FreqHz f_center,
        FreqHz effective_bin_size,
        FreqHz f_min,
        FreqHz f_max
    ) noexcept;

    /**
     * @brief Detection-window gate (pure, no state, no I/O).
     * @param start_mhz Array of num_slots range lower bounds (MHz; 0 = unset)
     * @param end_mhz   Array of num_slots range upper bounds (MHz; 0 = unset)
     * @param num_slots Number of slots (DETECTION_WINDOWS_PER_WINDOW)
     * @param freq      Candidate peak frequency (Hz)
     * @return true if freq MAY be tracked:
     *         - when NO slot is active → everywhere (full sweep range), or
     *         - when >= 1 slot is active → freq inside an active range
     * @note A slot is active iff start_mhz[i] > 0 && end_mhz[i] > 0 &&
     *       start_mhz[i] <= end_mhz[i]; inverted ranges count as inactive
     *       (guard against a malformed "kills the whole band" configuration).
     * @note MHz→Hz expansion per slot: one UMULL per slot (u32 × 1e6 → u64).
     * @note O(num_slots), integer-only, no heap, no stack beyond registers.
     */
    [[nodiscard]] static bool is_detection_window_allowed(
        const uint32_t* start_mhz,
        const uint32_t* end_mhz,
        uint8_t num_slots,
        FreqHz freq
    ) noexcept;

    /**
     * @brief Entry-center detectability probe (pure, no state, no I/O).
     * @param start_mhz Array of num_slots range lower bounds (MHz; 0 = unset)
     * @param end_mhz   Array of num_slots range upper bounds (MHz; 0 = unset)
     * @param num_slots Number of slots (DETECTION_WINDOWS_PER_WINDOW)
     * @param center    Candidate slice center (Hz)
     * @return true if a slice CENTERED at `center` may yield a detection:
     *         its detectable span [center − SWEEP_FIRST_SLICE_LEAD_HZ,
     *         center + SWEEP_SLICE_REACH_HIGH_HZ] intersects at least one
     *         ACTIVE slot ⇔ center lies inside the union of the slots'
     *         useful-center intervals [lo − REACH_HIGH, hi + LEAD].
     *         Returns true when NO slot is active (detector fully open —
     *         same rule as is_detection_window_allowed) and on malformed
     *         args (nullptr / num_slots == 0), so callers stay fail-open.
     * @note WHY THIS EXISTS (CRITICAL): next_detectable_center() searches
     *       STRICTLY ABOVE `from_center`. When the center under test is
     *       itself detectable but no grid point ABOVE it lands inside the
     *       slot's useful interval, next_detectable_center() honestly
     *       reports the pass-completion anchor — which the O1 ENTRY SKIP in
     *       on_sweep_spectrum() would misread as "the entry center is
     *       guard-zone" and jump away, permanently eliding the only slice
     *       that covers the slot. Concrete trigger: sweep [2400, 2700] MHz,
     *       slot [2400, 2405] MHz → f_center_ini = f_min + 9.375 MHz sees
     *       [2400.000, 2419.45] ⊇ slot, yet next(f_center_ini) = anchor.
     *       This probe answers the question next() structurally cannot:
     *       "is THIS center detectable?" — the entry-skip consults it and
     *       stays disarmed exactly when the entry center is signal-bearing.
     * @note Single source of truth: shares slot_useful_interval() with
     *       next_detectable_center(), so the probe and the walk can never
     *       drift apart. O(num_slots), integer-only, no heap, no FP, no
     *       locks. Stack: ~16 B. Flash: ~90 B.
     */
    [[nodiscard]] static bool span_intersects_active_slot(
        const uint32_t* start_mhz,
        const uint32_t* end_mhz,
        uint8_t num_slots,
        FreqHz center
    ) noexcept;

    /**
     * @brief Gate-aware slice skip (pure, no state, no I/O) — O1 optimization.
     * @param start_mhz Array of num_slots range lower bounds (MHz; 0 = unset)
     * @param end_mhz   Array of num_slots range upper bounds (MHz; 0 = unset)
     * @param num_slots Number of slots (DETECTION_WINDOWS_PER_WINDOW)
     * @param window_lo Sweep window lower bound (Hz) — anchor for the
     *        degenerate-range completion-jump below
     * @param window_hi Sweep window upper bound (Hz) — anchor for the
     *        degenerate-range completion-jump below
     * @param step_hz   Gapless pitch (Hz); MUST be > 0 (caller short-circuits
     *        the degenerate step_hz == 0 case before this call)
     * @param from_center Current slice center the sweep is advancing FROM (Hz);
     *        the returned center is the first DETECTABLE center STRICTLY
     *        greater than from_center
     * @return The next slice center to tune to:
     *         - the first candidate center > from_center whose detectable span
     *           [c − SWEEP_FIRST_SLICE_LEAD_HZ, c + SWEEP_SLICE_REACH_HIGH_HZ]
     *           intersects at least one ACTIVE detection slot, or
     *         - the completion anchor (window_hi − SWEEP_SLICE_BW/2) when NO
     *           such center exists (every remaining center is guard-zone), or
     *         - window_lo when the anchor itself would be degenerate
     *           (window narrower than one slice — guard against an anchor
     *           below window_lo that would walk the sweep backwards).
     * @note SLOT RULE: a slot is active iff is_det_win_slot_active() holds
     *       (SINGLE SOURCE OF TRUTH, constants.hpp). When NO slot is active,
     *       the detector allows EVERY frequency, so this returns from_center +
     *       step_hz unchanged (identity — the skip is disarmed).
     * @note The useful-center interval per active slot is
     *       [lo − REACH_HIGH, hi + LEAD]: a center c is skipped iff its whole
     *       detectable span lies inside the guard gap between two consecutive
     *       active intervals (or outside all of them). This mirrors
     *       is_detection_window_allowed exactly: a skipped c satisfies
     *       !is_detection_window_allowed(...) for EVERY frequency in
     *       [c − LEAD, c + REACH_HIGH], so the detector would have rejected
     *       every peak from that slice — detection-neutral by construction.
     * @note Drift-free: the walk is analytic (slot intervals computed once in
     *       Hz, no per-step O(frames²) scan), and the completion anchor uses
     *       the SAME last_c the composite accumulator would have reached, so
     *       pixel_index credit stays exact (see SweepWindow::credit_span).
     * @see  span_intersects_active_slot() for the complementary query
     *       "is THIS center detectable?" — required by callers that test the
     *       CURRENT center (this function only ever looks strictly above it).
     * @note O(num_slots) integer-only arithmetic, no heap, no FP, no locks.
     *       Stack: ~32 B. Flash: ~180 B.
     */
    [[nodiscard]] static FreqHz next_detectable_center(
        const uint32_t* start_mhz,
        const uint32_t* end_mhz,
        uint8_t num_slots,
        FreqHz window_lo,
        FreqHz window_hi,
        FreqHz step_hz,
        FreqHz from_center
    ) noexcept;

private:
    /**
     * @brief Useful-center interval [a_hz, b_hz] of ONE slot: a center c is
     *        detectable iff c ∈ [lo − SWEEP_SLICE_REACH_HIGH_HZ,
     *        hi + SWEEP_FIRST_SLICE_LEAD_HZ] (⇔ the slice's detectable span
     *        [c − LEAD, c + REACH] intersects [lo, hi]).
     * @param start_mhz Slot lower bound (MHz)
     * @param end_mhz   Slot upper bound (MHz)
     * @param a_hz      OUT: interval floor, clamped to ≥ 0 (a low slot edge
     *                  can push lo − REACH below zero)
     * @param b_hz      OUT: interval ceiling (hi + LEAD, always positive)
     * @pre  is_det_win_slot_active(start_mhz, end_mhz) — the caller filters
     *       inactive slots first (the slot rule lives in constants.hpp and
     *       stays visible at each call site).
     * @note SINGLE SOURCE OF TRUTH shared by next_detectable_center() and
     *       span_intersects_active_slot() — the walk and the entry-skip
     *       guard can never disagree about a slot's geometry.
     * @note Integer-only (u32 MHz → int64 Hz, one UMULL), no heap, no FP.
     *       Stack: ~16 B.
     */
    static void slot_useful_interval(
        uint32_t start_mhz,
        uint32_t end_mhz,
        int64_t& a_hz,
        int64_t& b_hz
    ) noexcept;
};

// ============================================================================
// Gate implementations — INLINE in this header (same pattern as
// shape_gate_logic.hpp / sweep_sensitivity.hpp): the logic is pure,
// integer-only and free of ChannelSpectrum, so the host test target compiles
// it with plain g++ while firmware TUs share one COMDAT copy per function
// (linker-deduplicated — no flash bloat). process_frame() — the only member
// needing ChannelSpectrum — stays in sweep_processor.cpp.
// ============================================================================

inline bool SweepProcessor::is_detection_window_allowed(
    const uint32_t* start_mhz,
    const uint32_t* end_mhz,
    const uint8_t num_slots,
    const FreqHz freq
) noexcept {
    if (start_mhz == nullptr || end_mhz == nullptr || num_slots == 0) return true;
    bool any_enabled = false;
    for (uint8_t i = 0; i < num_slots; ++i) {
        // Single source of truth for the slot rule: constants.hpp — checked
        // in the MHz mirror BEFORE expansion, so inactive slots skip the
        // UMULL entirely (was: expand both, then test lo/hi in Hz).
        if (!is_det_win_slot_active(start_mhz[i], end_mhz[i])) continue;
        // MHz → Hz expansion (one UMULL per ACTIVE slot; u32 max 7200 M
        // cannot overflow u64).
        const FreqHz lo = static_cast<FreqHz>(start_mhz[i]) * MHZ;
        const FreqHz hi = static_cast<FreqHz>(end_mhz[i]) * MHZ;
        any_enabled = true;
        if (freq >= lo && freq <= hi) return true;    // inside an active window
    }
    return !any_enabled;  // all slots disabled → entire sweep range allowed
}

inline void SweepProcessor::slot_useful_interval(
    const uint32_t start_mhz,
    const uint32_t end_mhz,
    int64_t& a_hz,
    int64_t& b_hz
) noexcept {
    // MHz → Hz expansion (u64; one UMULL — caller has already filtered the
    // slot through is_det_win_slot_active, see the @pre on the declaration).
    const int64_t lo_hz = static_cast<int64_t>(static_cast<FreqHz>(start_mhz) * MHZ);
    const int64_t hi_hz = static_cast<int64_t>(static_cast<FreqHz>(end_mhz) * MHZ);

    // Useful-center interval: a center c is DETECTABLE iff its span
    // [c − reach, c + lead] intersects [lo_hz, hi_hz] ⇔
    // c ∈ [lo_hz − reach, hi_hz + lead].
    int64_t a = lo_hz - static_cast<int64_t>(SWEEP_SLICE_REACH_HIGH_HZ);  // may go negative for a low slot → clamp
    if (a < 0) a = 0;
    a_hz = a;
    b_hz = hi_hz + static_cast<int64_t>(SWEEP_FIRST_SLICE_LEAD_HZ);
}

inline bool SweepProcessor::span_intersects_active_slot(
    const uint32_t* start_mhz,
    const uint32_t* end_mhz,
    const uint8_t num_slots,
    const FreqHz center
) noexcept {
    // Malformed args / no slots ⇒ the detector allows every frequency
    // (is_detection_window_allowed returns true unconditionally) ⇒ the
    // center counts as detectable ⇒ the caller's skip stays disarmed.
    if (start_mhz == nullptr || end_mhz == nullptr || num_slots == 0) return true;

    bool any_enabled = false;
    const int64_t c = static_cast<int64_t>(center);
    for (uint8_t i = 0; i < num_slots; ++i) {
        // Single source of truth for the slot rule: constants.hpp (mirrors
        // is_detection_window_allowed / next_detectable_center exactly).
        if (!is_det_win_slot_active(start_mhz[i], end_mhz[i])) continue;
        any_enabled = true;
        int64_t a;
        int64_t b;
        slot_useful_interval(start_mhz[i], end_mhz[i], a, b);
        if (c >= a && c <= b) return true;  // entry center is signal-bearing
    }
    // All slots inactive → whole band allowed (same rule as
    // is_detection_window_allowed): treat the center as detectable.
    return !any_enabled;
}

inline FreqHz SweepProcessor::next_detectable_center(
    const uint32_t* start_mhz,
    const uint32_t* end_mhz,
    const uint8_t num_slots,
    const FreqHz window_lo,
    const FreqHz window_hi,
    const FreqHz step_hz,
    const FreqHz from_center
) noexcept {
    // The advance-by-one pitch, returned unchanged whenever the skip is
    // disarmed (malformed args or NO active slot) — identity ⇒ behavior of the
    // historical `f_center += step_hz` is preserved bit-for-bit.
    const FreqHz advance = from_center + step_hz;

    // Guards: malformed pointers/counts and the degenerate step (caller is
    // contractually required to short-circuit step_hz == 0 before this call,
    // but a zero pitch here would make every k·step collapse to from_center
    // and stall the sweep — never return from_center itself).
    if (start_mhz == nullptr || end_mhz == nullptr || num_slots == 0 || step_hz == 0) {
        return advance;
    }

    // int64_t working domain: a slot's low edge (MHz) can sit below
    // SWEEP_SLICE_REACH_HIGH_HZ, so `lo − REACH_HIGH` (computed inside
    // slot_useful_interval) may go NEGATIVE; the high edge `hi + LEAD` at
    // 7200 MHz is ~7.21e9 and fits comfortably.
    const int64_t from = static_cast<int64_t>(from_center);
    const int64_t step = static_cast<int64_t>(step_hz);

    int64_t best = 0;  // 0 = "no detectable candidate found yet"

    for (uint8_t i = 0; i < num_slots; ++i) {
        // Single source of truth for the slot rule (constants.hpp) — mirror the
        // detector gate so a skipped center is provably one it would reject.
        if (!is_det_win_slot_active(start_mhz[i], end_mhz[i])) continue;

        // MHz → Hz expansion + useful-center interval via the SHARED helper
        // (single source of truth with span_intersects_active_slot()).
        int64_t a;
        int64_t b;
        slot_useful_interval(start_mhz[i], end_mhz[i], a, b);

        if (b <= from) continue;  // whole useful interval is behind us

        // Smallest k ≥ 1 with from + k·step ≥ a, i.e. the first grid point at
        // or above the interval's useful floor. For delta ≤ step this is k=1
        // (covers a ≤ from, i.e. the interval already reached behind us).
        const int64_t delta = a - from;
        int64_t k;
        if (delta <= step) {
            k = 1;
        } else {
            k = (delta + step - 1) / step;  // ceil(delta/step)
        }
        const int64_t c = from + k * step;
        if (c > b) continue;  // grid jumped over a narrow interval → guard zone
        if (best == 0 || c < best) best = c;
    }

    // Completion anchor: the first grid center at/after window_hi − SLICE_BW/2
    // — the point where freq_covered flips true and the pass completes. This
    // is also the CAP on the skip: a "detectable" center lying PAST the anchor
    // carries no useful content before the pass ends (its range-gated peaks
    // would be rejected by apply_sweep_tracking's [f_min, f_max] bound anyway),
    // so tuning there would overshoot the window instead of finishing it.
    const int64_t anchor_raw = static_cast<int64_t>(window_hi)
        - static_cast<int64_t>(SWEEP_SLICE_BW / 2);
    int64_t c_anchor;
    if (anchor_raw <= from) {
        c_anchor = from + step;
    } else {
        const int64_t ad = anchor_raw - from;
        const int64_t ka = (ad + step - 1) / step;  // ceil(ad/step)
        c_anchor = from + ka * step;
    }
    // Degenerate-range guard: a window narrower than one slice yields an
    // anchor below window_lo — never walk the sweep backwards.
    if (c_anchor < static_cast<int64_t>(window_lo)) return advance;

    // Detectable center strictly before the pass-completion point wins;
    // otherwise finish the pass at the anchor (guard-zone tail, or the only
    // detectable content sits past the completion point). Both branches
    // return a center >= from + step, so the sweep always advances.
    if (best != 0 && best < c_anchor) return static_cast<FreqHz>(best);
    return static_cast<FreqHz>(c_anchor);
}

} // namespace drone_analyzer

#endif // SWEEP_PROCESSOR_HPP
