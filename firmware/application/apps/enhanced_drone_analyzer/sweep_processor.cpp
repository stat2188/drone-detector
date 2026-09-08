#include <cstdint>
#include <cstddef>
#include "sweep_processor.hpp"
#include "message.hpp"
#include "constants.hpp"
#include "scanner.hpp"

namespace drone_analyzer {

bool SweepProcessor::is_exception_freq(
    FreqHz hz,
    FreqHz exception_radius_hz,
    const FreqHz* exceptions,
    uint8_t num_exceptions
) noexcept {
    for (uint8_t i = 0; i < num_exceptions; ++i) {
        if (exceptions[i] == 0) continue;
        const FreqHz lo = (exceptions[i] > exception_radius_hz)
            ? (exceptions[i] - exception_radius_hz) : 0;
        const FreqHz hi = exceptions[i] + exception_radius_hz;
        if (hz >= lo && hz <= hi) return true;
    }
    return false;
}

uint16_t SweepProcessor::process_frame(
    const ChannelSpectrum& spectrum,
    uint8_t* composite,
    uint16_t& pixel_index,
    uint8_t& pixel_max,
    FreqHz& bins_hz_acc,
    FreqHz pixel_step_hz,
    FreqHz f_center,
    FreqHz exception_radius_hz,
    const FreqHz* exceptions,
    uint8_t num_exceptions,
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
        if (is_exception_freq(freq, exception_radius_hz, exceptions, num_exceptions)) {
            continue;
        }

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

void SweepProcessor::reorder_frame(
    const ChannelSpectrum& spectrum,
    uint8_t* lg_buffer
) noexcept {
    for (uint8_t px = 0; px < COMPOSITE_SIZE; ++px) {
        if (px < SWEEP_FFT_MAP_CROSSOVER) {
            lg_buffer[px] = spectrum.db[SWEEP_FFT_MAP_START + px];
        } else if (px < UPPER_PIXEL_END) {
            lg_buffer[px] = spectrum.db[px - UPPER_OFFSET];
        } else {
            lg_buffer[px] = 0;  // pixels 238-239: DC spike region, zero
        }
    }
}

} // namespace drone_analyzer
