#ifndef SPECTRUM_ENVELOPE_HPP
#define SPECTRUM_ENVELOPE_HPP

#include <cstdint>
#include <cstddef>

namespace drone_analyzer {

/**
 * @brief 3-tap upper envelope (max-of-neighbours) over one contiguous run of
 *        FFT bins — the minimal "VBW"-style spatial smoother for the shape
 *        detector.
 *
 * WHY THIS FILTER AND NOT MEAN/MEDIAN/EMA (filters-lie analysis):
 * Every smoother fabricates values that were never measured, so the question
 * is not "does it lie" (all do) but "does it lie in a DIRECTION that cannot
 * destroy a real detection". Mean/boxcar LOWERS the peak (a weak narrowband
 * target — ELRS/burst — falls under the Step-3 gate: sensitivity loss).
 * Median-3 DELETES a lone single-bin peak (median[n,P,n] == n: the very class
 * of transient bursts this detector exists for). EMA SHIFTS the peak ~0.5 bin
 * (~39 kHz frequency error). This envelope ONLY RAISES valleys: by construction
 * dst[i] >= src[i], so a peak's amplitude and the Step-3 margin can never
 * shrink, and a crest's frequency bin never moves. Its single lie — filling a
 * one-bin notch on a jagged slope — is exactly the lie that stops the raw
 * staircase from spawning phantom crests and truncated Step-4 edges.
 *
 * INVARIANTS (relied on by the scanner):
 *  - MONOTONE-RAISING: dst[i] >= src[i] for every i. A margin gate measured on
 *    the envelope is always >= the same gate on raw => no peak can be lost.
 *  - NOTCH-ONLY: a valley of width >= 2 bins keeps its floor (dst uses only the
 *    3-tap window), so two genuinely separate emitters are never merged.
 *  - SEGMENT-LOCAL: the caller invokes this once per sideband; it never reads
 *    across the DC-spike gap or the window edge, so bin indices, the DC
 *    boundary and all frequency mappings stay valid.
 *
 * @param src Raw power bins (0..255 dB-compressed units). Read-only.
 * @param dst Output envelope. MUST NOT alias src. Same length as src.
 * @param lo  First index of the contiguous run (inclusive).
 * @param hi  Last  index of the contiguous run (inclusive), lo <= hi.
 * @note Edge policy: the first and last bin of the run replicate their only
 *       valid neighbour (no out-of-range read).
 * @note Stack: ~16 bytes (3 registers). SRAM: caller-owned dst. Flash: ~80 B.
 *       No float, no allocation, O(hi-lo+1), single pass.
 */
inline void compute_envelope_max3(
    const uint8_t* src,
    uint8_t* dst,
    size_t lo,
    size_t hi
) noexcept {
    if (src == nullptr || dst == nullptr || lo > hi) return;

    // Single-bin run: envelope == the bin itself.
    if (lo == hi) {
        dst[lo] = src[lo];
        return;
    }

    // Left edge: replicate the right neighbour (only valid 3-tap member).
    dst[lo] = (src[lo] > src[lo + 1]) ? src[lo] : src[lo + 1];

    // Interior: dst[i] = max(src[i-1], src[i], src[i+1]) via two max compares.
    for (size_t i = lo + 1; i < hi; ++i) {
        const uint8_t left = src[i - 1];
        const uint8_t mid = src[i];
        const uint8_t right = src[i + 1];
        const uint8_t max_lm = (left > mid) ? left : mid;
        dst[i] = (max_lm > right) ? max_lm : right;
    }

    // Right edge: replicate the left neighbour.
    dst[hi] = (src[hi] > src[hi - 1]) ? src[hi] : src[hi - 1];
}

/**
 * @brief Fill the envelope for a full 256-bin FFT frame, processing the two
 *        sidebands as INDEPENDENT runs so the DC-spike gap (bins
 *        [FFT_DC_SPIKE_START, FFT_DC_SPIKE_END)) is never bridged.
 *
 * This is the scanner-facing convenience wrapper: it encodes the DC-gap
 * policy in ONE place so neither process_spectrum_message nor
 * process_spectrum_sweep can accidentally smooth across the ADC-offset hole.
 *
 * @param src Raw 256-bin spectrum (read-only).
 * @param dst Output 256-bin envelope. MUST NOT alias src.
 * @param edge_skip Lowest/highest usable bin index (FFT_EDGE_SKIP for DB,
 *        FFT_EDGE_SKIP_NARROW for sweep). Bins outside [edge_skip,
 *        256-edge_skip) are left untouched (they carry no signal).
 * @param dc_start First DC-spike bin (inclusive).
 * @param dc_end   First bin after the DC spike (exclusive).
 * @note Stack: ~16 bytes. Flash: ~40 B on top of compute_envelope_max3.
 */
inline void compute_envelope_frame(
    const uint8_t* src,
    uint8_t* dst,
    size_t edge_skip,
    size_t dc_start,
    size_t dc_end
) noexcept {
    if (src == nullptr || dst == nullptr) return;
    if (edge_skip >= dc_start || dc_end >= (256 - edge_skip)) return;

    // Lower sideband [edge_skip, dc_start-1].
    compute_envelope_max3(src, dst, edge_skip, dc_start - 1);
    // Upper sideband [dc_end, 255-edge_skip].
    compute_envelope_max3(src, dst, dc_end, 255 - edge_skip);
}

} // namespace drone_analyzer

#endif // SPECTRUM_ENVELOPE_HPP
