#include <cstdint>
#include <cstddef>
#include "sweep_processor.hpp"
#include "message.hpp"
#include "constants.hpp"

namespace drone_analyzer {

bool SweepProcessor::is_detection_window_allowed(
    const uint32_t* start_mhz,
    const uint32_t* end_mhz,
    uint8_t num_slots,
    FreqHz freq
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

uint16_t SweepProcessor::process_frame(
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
) noexcept {
    if (composite == nullptr) {
        return pixel_index;
    }
    if (pixel_step_hz == 0 || effective_bin_size == 0 || f_max <= f_min) {
        return pixel_index;
    }
    const FreqHz range = f_max - f_min;

    // H1: division-free true-position mapping (BIT-EXACT vs the old u64 div).
    // Old per-bin math: px(bin) = (freq(bin) - f_min) * 240 / range with
    // freq(bin) = f_center + (bin - BIAS) * SWEEP_BIN_SIZE (BIAS = 126 upper /
    // 120 lower sideband — the same (bin-120)/(bin-126) Looking-Glass offsets
    // the detection path uses in fft_bin_to_freq()). K(bin) is an ARITHMETIC
    // PROGRESSION in bin, so the pre-division numerator N(bin) advances by a
    // CONSTANT 64-bit step P = 240 * SWEEP_BIN_SIZE (= 18'750'000 Hz) and the
    // per-bin truncating division is replaced by remainder accumulation
    // (Bresenham): floor((N+P)/range) from (N, N%range) — bit-identical, zero
    // divisions per bin (~4 cycles/bin vs ~40-100 for __aeabi_uldivmod).
    // Sensitivity/display equivalence: the px SEQUENCE is identical for every
    // bin (same numerator, same truncation, same range) and all continue
    // conditions fire in the same order — same cells get the same max-hold
    // writes; bins_hz_acc / pixel_index accounting is untouched.
    // Two sidebands ⇒ two monotonic runs (2..119, 136..253); the DC spike
    // (120..135) breaks monotonicity so the lower sideband re-anchors O(1).
    // NOTE: bins 134..135 are DC-spike members (old code skipped them via the
    // fft_bin>=120 check) and bins 254..255 were never visited (loop bound 238
    // ⇒ fft_bin<=253). The runs below visit EXACTLY {2..119} ∪ {136..253}.
    const uint64_t range64 = static_cast<uint64_t>(range);
    const uint64_t step_num =
        static_cast<uint64_t>(COMPOSITE_SIZE) * static_cast<uint64_t>(SWEEP_BIN_SIZE);

    // Paints one monotonic run; anchors quotient+remainder once (O(1) divs).
    // @pre bin_first <= bin_last, both < FFT_BIN_COUNT.
    // Stack: u64 anchor state in registers + the caller's accumulator refs;
    // the lambda captures by reference ([&]) and is invoked synchronously —
    // no heap, no thread hop, no lifetime escape.
    const auto paint_run = [&](uint32_t bias_bins, uint8_t bin_first, uint8_t bin_last) noexcept {
        // Reproduce the detection path's underflow guard (freq(bin) == 0 iff
        // f_center + bin*BIN < bias — same test the detector applies).
        // Freq is monotonic in bin within a run, so if the FIRST bin underflows,
        // every bin does — skip the run (the old code hit `freq == 0 → continue`
        // on each of them). Progress
        // accounting MUST still advance: the old code advanced bins_hz_acc
        // BEFORE the freq==0 check, so replicate that exactly here.
        const uint64_t bias_hz = static_cast<uint64_t>(bias_bins) *
                                 static_cast<uint64_t>(SWEEP_BIN_SIZE);
        const uint64_t first_hz = static_cast<uint64_t>(bin_first) *
                                  static_cast<uint64_t>(SWEEP_BIN_SIZE);
        if (static_cast<uint64_t>(f_center) + first_hz < bias_hz) {
            for (uint8_t bin = bin_first; bin <= bin_last; ++bin) {
                bins_hz_acc += effective_bin_size;
                while (bins_hz_acc >= pixel_step_hz && pixel_index < COMPOSITE_SIZE) {
                    ++pixel_index;
                    bins_hz_acc -= pixel_step_hz;
                }
            }
            return;
        }

        // Anchor: N(bin_first) = (f_center + bin_first*BIN - bias - f_min)*240.
        // u64-safe: f_center <= 7.2e9, bin*BIN <= 2e7, ×240 products <= ~2e12.
        // Underflow ORDER matches the detection path exactly (bias test first,
        // already passed above), but the f_min subtraction needs its own
        // guard: when freq(bin_first) < f_min the old code hit
        // `freq < f_min → continue` WITHOUT computing px. The monotonic run
        // is therefore split: the below-f_min prefix advances ONLY the
        // progress accumulator (exactly like the old `continue`), then the
        // run re-anchors at the first in-range bin with a non-negative
        // numerator. The common case (anchor already in range) keeps a single
        // O(1) anchor division.
        uint64_t n_first;
        uint8_t bin_anchor = bin_first;
        {
            const uint64_t freq_first = static_cast<uint64_t>(f_center) + first_hz - bias_hz;
            if (freq_first >= static_cast<uint64_t>(f_min)) {
                n_first = (freq_first - static_cast<uint64_t>(f_min)) *
                          static_cast<uint64_t>(COMPOSITE_SIZE);
            } else {
                // Below-range prefix: freq(bin) < f_min. Step in bin units:
                // need (freq_first + k*BIN - f_min) >= 0 ⇒ k >= (f_min - freq)/BIN.
                const uint64_t deficit = static_cast<uint64_t>(f_min) - freq_first;
                const uint64_t bin_size = static_cast<uint64_t>(SWEEP_BIN_SIZE);
                uint64_t k = deficit / bin_size;
                if ((deficit % bin_size) != 0) ++k;  // ceil
                // k bins (or the whole run) are skipped write-wise but MUST
                // advance bins_hz_acc — same as the old per-bin `continue`.
                // Bound analysis (no wraparound): bin_first <= bin_last <= 253
                // and k can exceed the run only when the deficit spans the
                // whole sideband; skip_end is clamped to bin_last + 1, and
                // bin_last + 1 <= 254 < 255 fits uint8_t. The loop below runs
                // at most (bin_last - bin_first + 1) <= 118 iterations.
                uint8_t bin = bin_first;
                // NOTE: (bin_first + k) is compared WITHOUT narrowing: k is u64
                // and can be astronomically large when f_min >> freq_first; the
                // test below keeps every operand at full width, so no
                // truncation can alias the comparison. (bin_last <= 253, so
                // bin_last + 1 <= 254 always fits uint8_t for the assignment.)
                const uint8_t skip_end =
                    (k > static_cast<uint64_t>(static_cast<uint32_t>(bin_last) - bin_first))
                        ? static_cast<uint8_t>(static_cast<uint32_t>(bin_last) + 1u)
                        : static_cast<uint8_t>(static_cast<uint32_t>(bin_first) + k);
                for (; bin < skip_end; ++bin) {
                    bins_hz_acc += effective_bin_size;
                    while (bins_hz_acc >= pixel_step_hz && pixel_index < COMPOSITE_SIZE) {
                        ++pixel_index;
                        bins_hz_acc -= pixel_step_hz;
                    }
                }
                if (bin > bin_last) return;  // whole run below f_min
                bin_anchor = bin;
                const uint64_t freq_anchor = freq_first +
                    static_cast<uint64_t>(static_cast<uint32_t>(bin - bin_first)) * bin_size;
                n_first = (freq_anchor - static_cast<uint64_t>(f_min)) *
                          static_cast<uint64_t>(COMPOSITE_SIZE);
            }
        }
        uint64_t px64 = n_first / range64;
        uint64_t rem = n_first % range64;
        // px is the u16 view of px64 used ONLY for the composite write gate.
        // px64 itself never wraps (see carry block below), so the write gate
        // stays exact even when a bin spans thousands of pixels.
        uint16_t px = (px64 >= COMPOSITE_SIZE) ? COMPOSITE_SIZE : static_cast<uint16_t>(px64);

        for (uint8_t bin = bin_anchor; /* bounded below */; ++bin) {
            // PROGRESS ACCUMULATOR — scan-head marker + line_full bookkeeping
            // only (byte-identical accounting to the old per-bin loop).
            bins_hz_acc += effective_bin_size;
            while (bins_hz_acc >= pixel_step_hz && pixel_index < COMPOSITE_SIZE) {
                ++pixel_index;
                bins_hz_acc -= pixel_step_hz;
            }

            // TRUE-POSITION PAINTING at the bin's real RF pixel.
            // Old guards in run form: freq==0 impossible (pre-checked above);
            // freq<f_min impossible from the anchor on (below-range prefix was
            // consumed above with accumulator-only advance); only the f_max
            // exit remains — px>=240 ⇒ skip the write (accumulator already
            // advanced, same order as the old `continue`).
            // NOTE: no pixel masking — the window is drawn FULLY; detection
            // gating lives exclusively in the scanner (is_detection_window_allowed).
            if (px < COMPOSITE_SIZE) {
                const uint8_t power = spectrum.db[bin];
                // Max-hold: overlapping slices reinforce the same column.
                if (power > composite[px]) {
                    composite[px] = power;
                }
            }

            if (bin >= bin_last) break;
            // Exact next-bin step: floor((N+P)/range) from (px64, rem) with
            // remainder carry — bit-identical to a fresh truncating u64 div.
            // Division form (NOT repeated subtraction): a single bin can span
            // THOUSANDS of pixels on degenerate narrow windows (range < 78 kHz
            // ⇒ step P/range > 240 px/bin), where a `while (rem >= range)`
            // subtraction loop would spin thousands of iterations per bin and
            // stall the UI thread. One UDIV per bin in that rare path is
            // bounded and safe; the COMMON path (windows >= ~78 kHz, i.e. all
            // real configs) takes the single-subtraction fast lane below with
            // zero divisions. px64 is monotonic u64 (never wraps); the u16 px
            // view saturates at COMPOSITE_SIZE so the write gate above stays
            // exact — writes are skipped for px >= 240 exactly as the old
            // `if (px >= COMPOSITE_SIZE) continue` did.
            rem += step_num;
            if (rem < range64) {
                // No carry — px unchanged (common case for wide windows).
            } else if (rem < range64 * 2u) {
                rem -= range64;
                ++px64;
            } else {
                const uint64_t carry = rem / range64;
                rem -= carry * range64;
                px64 += carry;
            }
            px = (px64 >= COMPOSITE_SIZE) ? COMPOSITE_SIZE : static_cast<uint16_t>(px64);
        }
    };

    // Upper sideband: FFT bins 2..119 (BIAS = SWEEP_BIAS_UPPER = 126, ascending freq).
    paint_run(SWEEP_BIAS_UPPER, 2, 119);
    // DC spike 120..135: no RF frequency, no Hz credit (old `continue` before
    // the accumulator — reproduced by simply not visiting these bins).
    // Lower sideband: FFT bins 136..253 (BIAS = SWEEP_BIAS_LOWER = 120, ascending freq).
    paint_run(SWEEP_BIAS_LOWER, 136, 253);

    // Legacy sequential-flush state: obsolete with true-position painting
    // (each bin targets its own column; no per-pixel running max exists).
    pixel_max = 0;
    return pixel_index;
}

} // namespace drone_analyzer
