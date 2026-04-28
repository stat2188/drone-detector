#include <cstring>

#include "sweep_coordinator.hpp"
#include "constants.hpp"

namespace drone_analyzer {

static constexpr FreqHz EACH_BIN_SIZE = SWEEP_SLICE_BW / 256;
static constexpr FreqHz BIN_SIZE = SWEEP_SLICE_BW / 256;

void SweepWindow::init(FreqHz start, FreqHz end, FreqHz step) noexcept {
    f_min = start;
    f_max = end;
    if (f_min >= f_max) {
        f_max = f_min + SWEEP_SLICE_BW;
    }
    pixel_step_hz = (f_max - f_min) / SWEEP_PIXELS_PER_SLICE;
    step_hz = (step > 0) ? step : (SWEEP_BINS_PER_STEP * EACH_BIN_SIZE);
    f_center_ini = f_min - (2 * BIN_SIZE) + (SWEEP_SLICE_BW / 2);
    reset();
}

void SweepWindow::reset() noexcept {
    memset(composite, 0, COMPOSITE_SIZE);
    f_center = f_center_ini;
    pixel_index = 0;
    pixel_max = 0;
    bins_hz_acc = 0;
}

bool SweepWindow::is_exception(FreqHz hz) const noexcept {
    for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
        if (exceptions[i] == 0) continue;
        const FreqHz lo = (exceptions[i] > exception_radius_hz) ? (exceptions[i] - exception_radius_hz) : 0;
        const FreqHz hi = exceptions[i] + exception_radius_hz;
        if (hz >= lo && hz <= hi) return true;
    }
    return false;
}

void SweepWindow::process_bins(const ChannelSpectrum& spectrum) noexcept {
    SweepProcessor::process_frame(
        spectrum,
        composite,
        pixel_index,
        pixel_max,
        bins_hz_acc,
        pixel_step_hz,
        f_min,
        exception_radius_hz,
        exceptions,
        EXCEPTIONS_PER_WINDOW
    );
}

FreqHz SweepWindow_center(SweepWindow& win) noexcept {
    return win.f_center;
}

SweepCoordinator::SweepCoordinator() noexcept
    : active_(false)
    , auto_mode_(false)
    , active_idx_(0)
    , pair_idx_(0)
    , pair_complete_count_(0)
    , last_tuned_freq_(0)
    , skip_next_fft_(true)
    , transition_guard_() {
}

SweepCoordinator::~SweepCoordinator() noexcept {
    stop_sweep();
}

bool SweepCoordinator::start_sweep(const ScanConfig& cfg) noexcept {
    if (active_) return true;
    if (!transition_guard_.try_set()) return false;

    active_ = true;
    last_tuned_freq_ = 0;
    skip_next_fft_ = true;

    windows_[0].init(cfg.sweep_start_freq, cfg.sweep_end_freq, cfg.sweep_step_freq);
    windows_[0].enabled = true;
    windows_[1].init(cfg.sweep2_start_freq, cfg.sweep2_end_freq, cfg.sweep2_step_freq);
    windows_[1].enabled = cfg.sweep2_enabled;
    windows_[2].init(cfg.sweep3_start_freq, cfg.sweep3_end_freq, cfg.sweep3_step_freq);
    windows_[2].enabled = cfg.sweep3_enabled;
    windows_[3].init(cfg.sweep4_start_freq, cfg.sweep4_end_freq, cfg.sweep4_step_freq);
    windows_[3].enabled = cfg.sweep4_enabled;

    const FreqHz exc_radius_hz = static_cast<FreqHz>(cfg.exception_radius_mhz) * 1000000ULL;
    for (uint8_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
        for (uint8_t i = 0; i < EXCEPTIONS_PER_WINDOW; ++i) {
            windows_[w].exceptions[i] = cfg.sweep_exceptions[w][i];
        }
        windows_[w].exception_radius_hz = exc_radius_hz;
    }

    active_idx_ = MAX_SWEEP_WINDOWS;
    for (uint8_t i = 0; i < MAX_SWEEP_WINDOWS; ++i) {
        if (windows_[i].enabled) {
            active_idx_ = i;
            break;
        }
    }

    if (active_idx_ >= MAX_SWEEP_WINDOWS) {
        active_ = false;
        transition_guard_.clear();
        return false;
    }

    pair_idx_ = pair_first(active_idx_);
    transition_guard_.clear();
    return true;
}

void SweepCoordinator::stop_sweep() noexcept {
    if (!active_) return;
    if (!transition_guard_.try_set()) return;

    active_ = false;
    auto_mode_ = false;
    active_idx_ = 0;
    pair_idx_ = 0;
    pair_complete_count_ = 0;
    last_tuned_freq_ = 0;
    skip_next_fft_ = true;

    for (auto& win : windows_) {
        win.enabled = false;
    }

    transition_guard_.clear();
}

bool SweepCoordinator::process_spectrum(const ChannelSpectrum& spectrum, FreqHz current_freq) noexcept {
    if (!active_) return false;
    if (active_idx_ >= MAX_SWEEP_WINDOWS) return false;

    auto& win = windows_[active_idx_];
    win.process_bins(spectrum);

    if (skip_next_fft_) {
        skip_next_fft_ = false;
        retune_active_window();
        return true;
    }

    if (win.pixel_index < COMPOSITE_SIZE) {
        if (win.f_center < win.f_max) {
            win.f_center += win.step_hz;
            retune_active_window();
            return true;
        }
        win.pixel_index = COMPOSITE_SIZE;
        win.bins_hz_acc = 0;
        win.pixel_max = 0;
    }

    const uint8_t w0 = pair_idx_;
    const uint8_t w1 = w0 + 1;
    const bool w0_done = !windows_[w0].enabled || windows_[w0].pixel_index >= COMPOSITE_SIZE;
    const bool w1_done = (w1 >= MAX_SWEEP_WINDOWS) || !windows_[w1].enabled || windows_[w1].pixel_index >= COMPOSITE_SIZE;
    const bool pair_complete = w0_done && w1_done;

    if (pair_complete) {
        if (windows_[w0].enabled) windows_[w0].reset();
        if (w1 < MAX_SWEEP_WINDOWS && windows_[w1].enabled) windows_[w1].reset();

        const uint8_t next_pair = (pair_idx_ + 2 < MAX_SWEEP_WINDOWS) ? pair_idx_ + 2 : 0;
        if (next_pair == 0 && auto_mode_) {
            return false;
        }
        pair_idx_ = next_pair;
    }

    uint8_t next = active_idx_;
    do {
        next = (next + 1) % MAX_SWEEP_WINDOWS;
    } while (!windows_[next].enabled && next != active_idx_);

    if (pair_complete && next == w0) {
        pair_complete_count_++;
    }

    active_idx_ = next;
    last_tuned_freq_ = windows_[next].f_center;
    retune_active_window();
    return true;
}

FreqHz SweepCoordinator::get_retune_frequency() const noexcept {
    if (!active_ || active_idx_ >= MAX_SWEEP_WINDOWS) return 0;
    return windows_[active_idx_].f_center;
}

bool SweepCoordinator::advance_to_next_pair() noexcept {
    if (!active_) return false;

    const uint8_t next = (pair_idx_ + 2 < MAX_SWEEP_WINDOWS) ? pair_idx_ + 2 : 0;
    if (next == 0) {
        pair_complete_count_++;
        if (pair_complete_count_ >= 2) {
            return true;
        }
    }
    pair_idx_ = next;
    return false;
}

bool SweepCoordinator::get_composite_data(uint8_t region, uint8_t* out_data, size_t max_size) const noexcept {
    if (region >= MAX_SWEEP_WINDOWS || out_data == nullptr || max_size < COMPOSITE_SIZE) {
        return false;
    }
    if (!windows_[region].enabled) return false;

    for (size_t i = 0; i < COMPOSITE_SIZE; ++i) {
        out_data[i] = windows_[region].composite[i];
    }
    return true;
}

bool SweepCoordinator::get_window_range(uint8_t idx, FreqHz& out_min, FreqHz& out_max) const noexcept {
    if (idx >= MAX_SWEEP_WINDOWS) return false;
    if (!windows_[idx].enabled) return false;
    out_min = windows_[idx].f_min;
    out_max = windows_[idx].f_max;
    return true;
}

size_t SweepCoordinator::get_histogram_data(uint16_t* out_hist, size_t max_size) const noexcept {
    if (out_hist == nullptr || max_size < COMPOSITE_SIZE * 2) return 0;

    size_t count = 0;
    const uint8_t w0 = pair_idx_;
    const uint8_t w1 = w0 + 1;

    if (windows_[w0].enabled) {
        const size_t n0 = (COMPOSITE_SIZE < max_size) ? COMPOSITE_SIZE : max_size;
        for (size_t i = 0; i < n0; ++i) {
            out_hist[count++] = static_cast<uint16_t>(windows_[w0].composite[i]) * COMPOSITE_TO_HIST_SCALE;
        }
    }
    if (w1 < MAX_SWEEP_WINDOWS && windows_[w1].enabled) {
        for (size_t i = 0; i < COMPOSITE_SIZE && count < max_size; ++i) {
            out_hist[count++] = static_cast<uint16_t>(windows_[w1].composite[i]) * COMPOSITE_TO_HIST_SCALE;
        }
    }
    return count;
}

void SweepCoordinator::retune_active_window() noexcept {
}

uint8_t SweepCoordinator::pair_first(uint8_t idx) const noexcept {
    if (idx >= MAX_SWEEP_WINDOWS) return 0;
    return (idx / 2) * 2;
}

} // namespace drone_analyzer