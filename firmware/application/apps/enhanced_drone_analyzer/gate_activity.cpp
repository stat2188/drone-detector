#include "gate_activity.hpp"

namespace drone_analyzer {

// ============================================================================
// Gate evaluation helpers — integer-only, no heap, no FP.
// These mirror the shape-chain logic in scanner.cpp::apply_shape_filters()
// so the UI can show the SAME verdicts the detector computed.
// ============================================================================

/**
 * @brief FFT bin → absolute RF (Hz) for one sweep slice.
 * @note Byte-for-byte mirror of DroneScanner::fft_bin_to_freq()
 *       (scanner.hpp) — kept local so this UI-only translation unit does not
 *       drag scanner.hpp (and its ch.h / locking / database / hardware
 *       include chain) into a pure render module. The GEOMETRY itself is
 *       shared through constants.hpp (SWEEP_BIAS_UPPER, SWEEP_BIAS_LOWER,
 *       SWEEP_BIN_SIZE, FFT_DC_SPIKE_*), so only the two-way branch is
 *       duplicated: if the detector's formula ever changes, change it here
 *       too — the same contract sweep_processor.cpp already mirrors for its
 *       bin→pixel mapping.
 * @note Subtract-bias-first form avoids the unsigned underflow a
 *       `(bin − bias)` form would produce below bias·bin_size.
 * @return 0 when bin lies in the DC spike (no valid RF) or f_center is too
 *         low for the resulting frequency to exist.
 */
static FreqHz sweep_bin_to_freq(FreqHz f_center, size_t bin) noexcept {
    if (bin >= FFT_DC_SPIKE_START && bin < FFT_DC_SPIKE_END) return 0;
    const uint32_t bias = (bin >= FFT_DC_SPIKE_END)
        ? static_cast<uint32_t>(SWEEP_BIAS_LOWER)
        : static_cast<uint32_t>(SWEEP_BIAS_UPPER);
    const FreqHz offset = static_cast<FreqHz>(bias) * SWEEP_BIN_SIZE;
    const FreqHz freq = static_cast<FreqHz>(bin) * SWEEP_BIN_SIZE;
    if (f_center + freq < offset) return 0;
    return f_center - offset + freq;
}

/**
 * @brief Mirror of DroneScanner::VALLEY_PROBE_MAX_BINS (scanner.hpp).
 * @note The original is a PRIVATE static member, so it cannot be referenced
 *       here without including scanner.hpp — which would defeat the include
 *       isolation above. Keep the two in lockstep: the probe window must
 *       stay below CFAR_MIN_PEAK_SEPARATION (see the static_assert in
 *       scanner.hpp) or a distinct neighbour could hide inside the window.
 */
constexpr uint8_t VALLEY_PROBE_BINS = 6;

/** @brief Evaluate the shape chain for ONE candidate and record the verdict.
 *
 *  Mirrors scanner.cpp::apply_shape_filters() Steps 3-11 in the SAME reject
 *  order, so `reject_reason` names the step the detector would have rejected
 *  at first, and gate_step_label() turns it into the on-screen glyph.
 *
 *  @param spectrum    256-bin raw FFT power of ONE slice (read-only).
 *  @param bin         Candidate peak bin (0-255).
 *  @param f_center    Slice center of that frame — the only input that turns
 *                     `bin` into the absolute RF stored in verdict.freq.
 *  @param noise_floor p25 shelf of the usable bins.
 *  @param overlay     Gate limits — RAW ScanConfig values, 0 = gate off.
 *  @param verdict     Output: pass/fail per step + metrics + absolute RF.
 *  @return false when a step rejected (verdict.reject_reason = slot index),
 *          true when the candidate cleared Steps 3-11.
 *  @note  ENFORCED, not assumed: a bin in the DC spike or the edge rolloff is
 *         reported 0x7F "not evaluated" instead of measuring a width across
 *         the notch — the detector's CFAR selection guarantees this upstream,
 *         a UI caller may not.
 *  @note  Stack: ~56 bytes (two flank probes + accumulators, no arrays).
 *         Flash: ~450 bytes. No FP, no heap, no locks — UI-thread safe.
 *  @note  KNOWN DIVERGENCES from the detector (deliberate, documented):
 *         (1) Sens-mode easing (eased_*_limit, w²) is NOT applied — limits
 *             are the raw user values, i.e. exactly what Sens OFF judges.
 *             With Sens ON the detector is GENTLER near the gate, so this
 *             mirror can only be STRICTER, never looser: a red marker here
 *             with a green detection on the same frame means easing, not a
 *             bug in this file.
 *             STEP-3 EXCEPTION — the ONE case where the mirror is LOOSER:
 *             with Sens > 75 and Sens-mode OFF the detector scales its
 *             entry gate UP by (Sens−75)/2 (shape_gate_margin()), so a
 *             candidate whose raw margin sits in [raw, raw+scaling) is
 *             REJECTED by the detector but judged here against the raw
 *             value. Close it by populating overlay.margin_gate with
 *             shape_gate_margin_try() instead of config_.spectrum_margin.
 *         (2) Arming / abstention predicates (run_sharp, run_valley,
 *             run_flat, very_strong, dual-peak skip, A-1 raw fallback) are
 *             not mirrored — a step the detector would have skipped is judged
 *             here. Same direction: never a false "passed".
 *         (3) Step 6b (emission extent) and Step 6c (edge-clip guard) are not
 *             mirrored; only the Step-6 fragment width is.
 *         (4) width_floor ≈ noise_floor: the scanner demotes to p10 only in
 *             occupied frames (audit W1); the UI has only the p25 shelf.
 *  @pre    spectrum != nullptr is checked; the bin's sideband is checked.
 */
[[nodiscard]] bool evaluate_candidate_gates(
    const uint8_t* spectrum,
    size_t bin,
    FreqHz f_center,
    uint8_t noise_floor,
    const GateOverlay& overlay,
    GateVerdict& verdict
) noexcept {
    if (spectrum == nullptr) {
        verdict = GateVerdict{};
        verdict.reject_reason = 0x7F;  // not evaluated
        return false;
    }

    // Structural preconditions the detector guarantees upstream: enforce them
    // here too, so a malformed caller gets a grey "not evaluated" verdict
    // instead of a width measured across the DC notch or the edge rolloff.
    if (bin < FFT_EDGE_SKIP_NARROW ||
        bin >= (FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW) ||
        (bin >= FFT_DC_SPIKE_START && bin < FFT_DC_SPIKE_END)) {
        verdict = GateVerdict{};
        verdict.bin = static_cast<uint8_t>(bin);
        verdict.reject_reason = 0x7F;  // not evaluated
        return false;
    }

    const uint8_t peak = spectrum[bin];
    verdict.bin = static_cast<uint8_t>(bin);
    verdict.power = peak;
    verdict.margin = (peak > noise_floor) ? static_cast<uint8_t>(peak - noise_floor) : 0;
    verdict.width = 0;  // filled by the Step-4 walk below; 0 = never measured
    verdict.gates_passed_mask = 0;
    verdict.reject_reason = -1;
    // Absolute RF of the peak — the renderer's ONLY placement source.
    // (bin × SWEEP_BIN_SIZE is NOT the offset from f_min: a sweep window
    // spans many 20 MHz slices, so bin 0 belongs to whichever slice was
    // captured, never to the window's left edge.)
    verdict.freq = sweep_bin_to_freq(f_center, bin);

    // ---- Step 3: Peak Margin (entry gate) ----
    if (overlay.margin_gate > 0 && verdict.margin < overlay.margin_gate) {
        verdict.reject_reason = 0;  // slot 0 = Step3_Margin
        return false;
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step3_Margin);

    // ---- Step 4: measure emission width (NOT a gate — Steps 5-11 share it) --
    // Scanner bar (audit W1): width_floor + peak_margin / 3, where the /3 (vs
    // /4) keeps the bar above the 1-bin quantization noise for weak signals
    // and /2 is used only for very-strong peaks (not mirrored — divergence
    // (2)); the width_floor vs noise_floor anchor is divergence (4).
    // The anchor is the MEASURED margin of THIS peak; the configured entry
    // gate is a different quantity and made the walk depend on a knob the
    // width has nothing to do with.
    const uint16_t elevated_sum =
        static_cast<uint16_t>(noise_floor) +
        static_cast<uint16_t>(verdict.margin / 3u);
    const uint8_t elevated_threshold =
        (elevated_sum > 255) ? 255 : static_cast<uint8_t>(elevated_sum);

    constexpr size_t edge_skip = FFT_EDGE_SKIP_NARROW;                  // 6
    constexpr size_t upper_limit = FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW; // 250 (exclusive)

    // DC spike (bins 120-135) is a HARD WALL: the walk stops at it and never
    // bridges — ADC offset energy would otherwise inflate width by up to 16
    // bins (1.25 MHz) and glue two unrelated sidebands into one emission.
    size_t left = bin;
    while (left > edge_skip) {
        const size_t prev = left - 1;
        if (prev >= FFT_DC_SPIKE_START && prev < FFT_DC_SPIKE_END) break;
        if (spectrum[prev] < elevated_threshold) break;
        --left;
    }
    size_t right = bin;
    while (right < upper_limit - 1) {  // last readable bin is 249; 250-255 is rolloff
        const size_t next = right + 1;
        if (next >= FFT_DC_SPIKE_START && next < FFT_DC_SPIKE_END) break;
        if (spectrum[next] < elevated_threshold) break;
        ++right;
    }
    const size_t width = right - left + 1;
    verdict.width = static_cast<uint8_t>((width > 255) ? 255 : width);

    // Walk-stop classification (audit D8): a flank stopped by the window edge
    // or the DC wall is an OBSERVATION GAP, not a measurement — Steps 9 and 11
    // must not read a boundary artifact as shape evidence.
    const bool left_clipped =
        (left <= edge_skip) ||
        (bin >= FFT_DC_SPIKE_END && left == FFT_DC_SPIKE_END);
    const bool right_clipped =
        (right >= upper_limit - 1) ||
        (bin < FFT_DC_SPIKE_START && right == FFT_DC_SPIKE_START - 1);

    // ---- Step 5: Minimum width ----
    // The detector enforces config_.spectrum_min_width UNCONDITIONALLY (audit:
    // MinW is a user band edge, not a noise-adaptive threshold — no margin
    // gate participates). 0 rejects nothing, exactly like a config value of 0.
    if (overlay.minw_limit > 0 && width < overlay.minw_limit) {
        verdict.reject_reason = 1;  // slot 1 = Step5_MinWidth
        return false;
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step5_MinWidth);

    // ---- Step 6: Maximum width ----
    // Only the Step-6 fragment check lives here; 6b (emission extent) and 6c
    // (edge-clip guard) need emission_extent() and are divergence (3).
    if (overlay.maxw_limit > 0 && width > overlay.maxw_limit) {
        verdict.reject_reason = 2;  // slot 2 = Step6_MaxWidth
        return false;
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step6_MaxWidth);

    // ---- Step 7: Peak sharpness (enforce inverted-V shape) ----
    // README: sharpness = (peak_margin * 100) / avg_margin over [left..right]
    // and "Must exceed spectrum_peak_sharpness" → REJECT when BELOW the limit
    // (direction is the OPPOSITE of Flat: bigger = stricter here).
    // avg_margin averages POSITIVE MARGINS over the fragment (bins at or below
    // the shelf are not counted, DC bins skipped) — averaging raw power would
    // read the shelf into the denominator and understate sharpness by ~25-40 %,
    // rejecting clean inverted-V peaks the detector accepts.
    if (overlay.sharpness_limit > 0) {
        int32_t margin_sum = 0;
        size_t margin_count = 0;
        for (size_t i = left; i <= right; ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;  // defensive
            if (spectrum[i] > noise_floor) {
                margin_sum += static_cast<int32_t>(spectrum[i]) - static_cast<int32_t>(noise_floor);
                ++margin_count;
            }
        }
        const int32_t avg_margin =
            (margin_count > 0) ? (margin_sum / static_cast<int32_t>(margin_count)) : 0;
        // avg_margin == 0 is a REJECT in the legacy semantics, not an abstain.
        const int32_t sharp = (avg_margin > 0)
            ? (static_cast<int32_t>(verdict.margin) * 100) / avg_margin
            : 0;
        if (sharp < static_cast<int32_t>(overlay.sharpness_limit)) {
            verdict.reject_reason = 3;  // slot 3 = Step7_Sharpness
            return false;
        }
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step7_Sharpness);

    // ---- Step 8: Peak-to-width ratio (the RAT setting) ----
    // README: ratio = (peak_margin * 10) / signal_width, "Must exceed
    // spectrum_peak_ratio (default 0 = disabled)". Its own limit field — the
    // chain reads flatness_limit here, which would judge Rat against the
    // Flatness knob and light the "R" label for the wrong reason.
    if (overlay.ratio_limit > 0) {
        const uint32_t ratio =
            (static_cast<uint32_t>(verdict.margin) * 10u) / static_cast<uint32_t>(width);
        if (ratio < static_cast<uint32_t>(overlay.ratio_limit)) {
            verdict.reject_reason = 4;  // slot 4 = Step8_PeakRatio
            return false;
        }
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step8_PeakRatio);

    // ---- Step 9: Valley depth (deep valleys flanking peak = V-shape) ----
    // WINDOWED PROBE OUTSIDE THE FRAGMENT (audit D-VALLEY-PROBE): each flank
    // reads up to VALLEY_PROBE_BINS usable bins BEYOND the walk edge and takes
    // the maximum margin. Probing around the peak instead would read the
    // emission itself, measure ~0 and reject every well-formed V-shape.
    // Boundary-clipped sides contribute 0 (no veto), like the scanner.
    if (overlay.valley_limit > 0) {
        uint8_t left_flank = 0;
        uint8_t right_flank = 0;
        if (left > edge_skip) {
            size_t i = left - 1;
            for (uint8_t n = 0; n < VALLEY_PROBE_BINS; ++n) {
                if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) break;  // DC wall
                if (spectrum[i] > noise_floor) {
                    const uint8_t m = static_cast<uint8_t>(spectrum[i] - noise_floor);
                    if (m > left_flank) left_flank = m;
                }
                if (i <= edge_skip) break;
                --i;
            }
        }
        if (right < upper_limit - 1) {
            size_t i = right + 1;
            for (uint8_t n = 0; n < VALLEY_PROBE_BINS; ++n) {
                if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) break;
                if (spectrum[i] > noise_floor) {
                    const uint8_t m = static_cast<uint8_t>(spectrum[i] - noise_floor);
                    if (m > right_flank) right_flank = m;
                }
                if (i + 1 >= upper_limit) break;
                ++i;
            }
        }
        const uint8_t max_flank =
            (left_flank > right_flank) ? left_flank : right_flank;
        // Scanner: ok = (max_valley < valley_limit) — the flank must STAY
        // BELOW the depth (a deep valley is a LOW flank), so >= is the reject.
        if (max_flank >= overlay.valley_limit) {
            verdict.reject_reason = 5;  // slot 5 = Step9_Valley
            return false;
        }
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step9_Valley);

    // ---- Step 10: Flatness (reject flat-top signals like WiFi/BT) ----
    // Threshold is NOISE-RELATIVE: noise_floor + 0.9 * peak_margin (audit F2).
    // 0.9 × raw_peak — the pre-F2 absolute form — shrinks the effective
    // elevation as the shelf rises (65 % of margin at noise 100, 50 % at 160)
    // and lets the verdict drift with gain while every other step is
    // noise-relative. Numerator counts ALL fragment bins >= the bar.
    // Denominator guard: FLATNESS_MIN_SIGNAL_WIDTH (= 4), same as arming.
    if (overlay.flatness_limit > 0 && width > FLATNESS_MIN_SIGNAL_WIDTH) {
        const uint16_t hp_sum =
            static_cast<uint16_t>(noise_floor) +
            static_cast<uint16_t>((static_cast<uint16_t>(verdict.margin) * 9u) / 10u);
        const uint8_t hp_threshold = (hp_sum > 255) ? 255 : static_cast<uint8_t>(hp_sum);
        uint16_t high_power = 0;
        for (size_t i = left; i <= right; ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;  // defensive
            if (spectrum[i] >= hp_threshold) ++high_power;
        }
        const uint8_t flat_pct = static_cast<uint8_t>(
            (static_cast<uint32_t>(high_power) * 100u) / static_cast<uint32_t>(width));
        // Scanner: ok = (pct <= limit) → strictly above the bar rejects.
        // (Flat's direction is the OPPOSITE of Sharp's: smaller = stricter.)
        if (flat_pct > overlay.flatness_limit) {
            verdict.reject_reason = 6;  // slot 6 = Step10_Flatness
            return false;
        }
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step10_Flatness);

    // ---- Step 11: Symmetry (V-shape must have similar left/right width) ----
    // WIDTH-based, not energy-based: left_width = bin − left,
    // right_width = right − bin, sym = min*100/max (README Step 11). Summing
    // power over the two halves measures energy balance instead of geometry
    // and disagrees with the detector on every asymmetric-but-narrow peak.
    // A boundary-clipped flank is an observation gap → the scanner ABSTAINS;
    // mirroring that as "did not reject" keeps a signal parked at a window or
    // DC wall from being painted red for an asymmetry nobody observed (D8).
    if (overlay.symmetry_limit > 0 && width > 1 && !left_clipped && !right_clipped) {
        // width > 1 ⇒ left_width + right_width ≥ 1 ⇒ max_side ≥ 1 (no /0).
        const size_t left_width = bin - left;
        const size_t right_width = right - bin;
        const size_t max_side = (left_width > right_width) ? left_width : right_width;
        const size_t min_side = (left_width < right_width) ? left_width : right_width;
        const uint8_t sym_pct = static_cast<uint8_t>((min_side * 100u) / max_side);
        if (sym_pct < overlay.symmetry_limit) {
            verdict.reject_reason = 7;  // slot 7 = Step11_Symmetry
            return false;
        }
    }
    verdict.gates_passed_mask |= static_cast<uint16_t>(ShapeGateBit::Step11_Symmetry);

    // ---- Steps 12-15: Kurtosis, Neighbor, Mahalanobis, RSSI-variance ----
    // Not evaluated here: Step 12 needs SpectralKurtosis over the fragment and
    // Steps 13-15 are POST-filter trackers holding multi-frame state that does
    // not exist on the UI thread. Recorded as "did not reject" so the overlay
    // never paints a false red for them — their real verdicts simply are not
    // reachable from a single frame.
    verdict.gates_passed_mask |=
        static_cast<uint16_t>(ShapeGateBit::Step12_Kurtosis) |
        static_cast<uint16_t>(ShapeGateBit::Step13_Neighbor) |
        static_cast<uint16_t>(ShapeGateBit::Step14_Mahalanobis) |
        static_cast<uint16_t>(ShapeGateBit::Step15_RSSIVar);

    return true;
}

/** @brief Apply the display-side gate floor to a sweep composite.
 *  Subtracts half the Step-3 margin gate from every column, so bars the gates
 *  would not even consider fall away visually (the detector's entry gate and
 *  the display's own noise-floor subtraction are independent; this is the
 *  bridge between them).
 *
 *  @param composite In/out 240-byte composite pixel buffer (true-position mapped).
 *  @param size      Composite size (must be <= COMPOSITE_SIZE).
 *  @param overlay   Gate limits — only margin_gate is consumed here.
 *  @note Stack: ~8 bytes. Flash: ~90 bytes. No heap, no FP.
 *  @note NO bin-index blanking: `composite` is a PIXEL axis (column i sits at
 *        f_min + i·range/240), not an FFT-bin axis. The mapper never writes
 *        DC or edge-rolloff bins into any column, so erasing "bins 120-135"
 *        here would blank six real columns at the LOW end of the window
 *        (columns 0-5 are the window's first detection bins — f_min lands on
 *        bin 6 by construction) plus sixteen unrelated columns in the middle,
 *        while leaving the actual notch untouched. Blank nothing; subtract.
 *  @note NOT CALLED from anywhere yet.
 *  @warning The subtraction is NOT invertible — never pass a max-hold
 *           accumulator (SweepWindow::composite): the bars would be
 *           lowered for the rest of the pass and the next max-hold would
 *           re-raise them, oscillating. Apply to a render-time copy, or
 *           drop this helper in favour of subtracting at draw time.
 */
void prepare_sweep_for_gates(
    uint8_t* composite,
    size_t size,
    const GateOverlay& overlay
) noexcept {
    if (composite == nullptr || size == 0 || size > COMPOSITE_SIZE) return;

    const uint8_t floor_sub = (overlay.margin_gate > 0)
        ? static_cast<uint8_t>(overlay.margin_gate / 2u)
        : 0;
    if (floor_sub == 0) return;

    for (size_t i = 0; i < size; ++i) {
        const uint8_t val = composite[i];
        composite[i] = (val > floor_sub)
            ? static_cast<uint8_t>(val - floor_sub)
            : static_cast<uint8_t>(0);
    }
}

/** @brief Render gate-activity verdicts as a compact overlay on the spectrum.
 *  Draws one glyph per verdict: a 2-px marker at the candidate's true RF
 *  position and height, green when it cleared the chain, red when a step
 *  rejected it (with that step's single-char label above it), grey when the
 *  chain could not evaluate it — plus a dim reference line at the shelf.
 *
 *  @param painter   UI painter.
 *  @param verdicts  Array of gate verdicts (from evaluate_candidate_gates).
 *  @param count     Number of valid verdicts (0..MAX_GATE_VERDICTS).
 *  @param x0,y0     TOP-LEFT of the CHART area — the plotting rectangle
 *                   render_composite_* uses, i.e. already past the band
 *                   header. Passing the band rect would offset every glyph
 *                   by the header height.
 *  @param w,h       Chart width/height in pixels. `h` must be the same
 *                   chart_height the bars were drawn with, so the height
 *                   mapping below is byte-identical to theirs.
 *  @param f_min,f_max Linear frequency bounds of the chart.
 *  @param noise_floor Current shelf — markers are shifted by it exactly as
 *                   the bars are, so both share one vertical scale.
 *  @note Stack: ~32 bytes. Flash: ~350 bytes. No heap, no FP.
 *  @note Column index uses the bars' OWN index space, not a w-rescale:
 *           px = (freq − f_min) * COMPOSITE_SIZE / range
 *        Rescaling by w instead would drift a marker away from its bar on
 *        any chart whose drawable width ≠ 240.
 *  @note NOT CALLED from anywhere yet — DroneDisplay::render_gate_overlay()
 *        is the intended entry point and paint() has no call site for it.
 */
void render_gate_activity(
    Painter& painter,
    const GateVerdict* verdicts,
    size_t count,
    uint16_t x0,
    uint16_t y0,
    uint16_t w,
    uint16_t h,
    FreqHz f_min,
    FreqHz f_max,
    uint8_t noise_floor
) noexcept {
    if (verdicts == nullptr || count == 0 || w == 0 || h == 0) return;
    if (f_max <= f_min) return;

    const FreqHz range = f_max - f_min;
    // Same drawable-column rule as render_composite_*: the composite is
    // truncated to the chart, and a column beyond it has no bar to sit on.
    const uint16_t bar_count = (static_cast<uint16_t>(COMPOSITE_SIZE) < w)
        ? static_cast<uint16_t>(COMPOSITE_SIZE)
        : w;
    const uint16_t floor_y = static_cast<uint16_t>(y0 + h - 1);  // 0-height baseline

    // Noise-floor reference line: the bars are drawn shelf-subtracted, so the
    // shelf itself IS the chart's bottom row.
    painter.draw_hline({x0, floor_y}, bar_count, Color::darker_grey());

    for (size_t i = 0; i < count && i < MAX_GATE_VERDICTS; ++i) {
        const GateVerdict& v = verdicts[i];
        // No placeable RF (DC spike / underflow) or outside this chart — the
        // same frequencies the bars never paint.
        if (v.freq == 0) continue;
        if (v.freq < f_min || v.freq >= f_max) continue;

        const uint64_t px64 =
            (static_cast<uint64_t>(v.freq - f_min) * static_cast<uint64_t>(COMPOSITE_SIZE)) /
            static_cast<uint64_t>(range);
        if (px64 >= static_cast<uint64_t>(bar_count)) continue;
        const uint16_t px = static_cast<uint16_t>(px64);

        // Height on the bars' own scale: shelf-subtracted, ×h, ÷256 — and a
        // 0-height marker still occupies the bottom row, exactly as
        // draw_bar_with_envelope() floors final_height at 1.
        const uint8_t p = (v.power > noise_floor)
            ? static_cast<uint8_t>(v.power - noise_floor)
            : static_cast<uint8_t>(0);
        const uint16_t bar_h = static_cast<uint16_t>(
            (static_cast<uint32_t>(p) * static_cast<uint32_t>(h)) >> 8);
        const uint16_t drawn_h = (bar_h > 0) ? bar_h : 1;
        const uint16_t bar_y = static_cast<uint16_t>(y0 + h - drawn_h);

        // Green = cleared the chain, red = a step rejected, grey = not evaluated.
        const uint32_t color = (v.reject_reason == -1)  ? COLOR_LOW_THREAT :
                               (v.reject_reason == 0x7F) ? COLOR_UNKNOWN_THREAT :
                                                           COLOR_CRITICAL_THREAT;

        painter.fill_rectangle({x0 + px, bar_y, 2, drawn_h}, Color::RGB(color));

        // Single-char label of the rejecting step, one 8-px glyph box above
        // the marker — skipped when it would climb out of the chart.
        if (v.reject_reason >= 0 && v.reject_reason < 12 && bar_y >= y0 + 8) {
            const ShapeGateBit bit =
                static_cast<ShapeGateBit>(1u << static_cast<uint16_t>(v.reject_reason));
            const char* label = gate_step_label(bit);
            painter.draw_string({x0 + px, static_cast<int>(bar_y) - 8},
                                ui::font::fixed_5x8,
                                Color::RGB(color), Color::black(),
                                std::string_view(label, 1));
        }
    }
}

} // namespace drone_analyzer
