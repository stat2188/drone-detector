#include <cstdint>
#include <cstddef>
#include "sweep_processor.hpp"
#include "message.hpp"
#include "constants.hpp"
#include "scanner.hpp"

namespace drone_analyzer {

bool SweepProcessor::is_detection_window_allowed(
    const FreqRangeHz* slots,
    uint8_t num_slots,
    FreqHz freq
) noexcept {
    if (slots == nullptr || num_slots == 0) return true;
    bool any_enabled = false;
    for (uint8_t i = 0; i < num_slots; ++i) {
        const FreqHz lo = slots[i].start_hz;
        const FreqHz hi = slots[i].end_hz;
        if (lo == 0 || hi == 0 || lo > hi) continue;  // inactive slot (0/0 or inverted)
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
    if (pixel_step_hz == 0 || effective_bin_size == 0 || f_max <= f_min) {
        return pixel_index;
    }
    const FreqHz range = f_max - f_min;

    for (uint8_t bin = 0; bin < SWEEP_PIXELS_PER_SLICE; ++bin) {
        if (bin >= UPPER_PIXEL_END) continue;

        const uint8_t fft_bin = (bin < 118)
            ? (bin + 2)   // upper sideband: fft_bin 2..119
            : (bin + 16); // lower sideband: fft_bin 134..253

        // Skip DC spike bins (120-135) — contain DC leakage, not real signal.
        // Do NOT advance bins_hz_acc here: advancing without power creates
        // "dead pixels" in the composite when a pixel boundary is crossed.
        if (fft_bin >= FFT_DC_SPIKE_START && fft_bin < FFT_DC_SPIKE_END) {
            continue;
        }

        // PROGRESS ACCUMULATOR — drives the scan-head marker and the
        // line_full completion check only. Each of the 236 signal bins
        // contributes effective_bin_size (= step_hz / 236), so a slice
        // advances pixel_index by exactly step_hz / pixel_step_hz pixels,
        // identical to the pre-true-position pacing.
        bins_hz_acc += effective_bin_size;
        while (bins_hz_acc >= pixel_step_hz && pixel_index < COMPOSITE_SIZE) {
            ++pixel_index;
            bins_hz_acc -= pixel_step_hz;
        }

        // TRUE-POSITION PAINTING: write this bin's power at the pixel its
        // REAL RF frequency occupies on the linear window scale — the same
        // mapping as the band title and the tracked drones' frequencies.
        // Sequential slot placement drew each slice compressed ~2.4x and
        // duplicated signals across overlapping slices (drawn peaks did not
        // match tracked frequencies).
        const FreqHz freq = DroneScanner::fft_bin_to_freq(f_center, fft_bin);
        if (freq == 0 || freq < f_min || freq >= f_max) continue;
        // NOTE: no pixel masking here — the sweep window is drawn FULLY.
        // Detection-window gating is applied in the scanner only
        // (DroneScanner::is_detection_window_allowed).

        // 64-bit intermediate: (freq - f_min) up to ~5 GHz x 240 overflows
        // uint32_t. One UMULL + UDIV per bin on the UI thread — the old code
        // already called fft_bin_to_freq() per bin, same cost class.
        const uint16_t px = static_cast<uint16_t>(
            (static_cast<uint64_t>(freq - f_min) * COMPOSITE_SIZE) / range);
        if (px >= COMPOSITE_SIZE) continue;

        const uint8_t power = spectrum.db[fft_bin];
        // Max-hold: overlapping slices cover the same column up to
        // ceil(slice_RF_span / step) + 1 times per pass — keep the strongest.
        if (power > composite[px]) {
            composite[px] = power;
        }
    }

    // Legacy sequential-flush state: obsolete with true-position painting
    // (each bin targets its own column; no per-pixel running max exists).
    pixel_max = 0;
    return pixel_index;
}

} // namespace drone_analyzer
