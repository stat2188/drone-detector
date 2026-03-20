#include "histogram_processor.hpp"

namespace drone_analyzer {

// ============================================================================
// Constructor
// ============================================================================

HistogramProcessor::HistogramProcessor() noexcept
    : bin_count_(HISTOGRAM_BIN_COUNT)
    , start_frequency_(DEFAULT_HISTOGRAM_START_HZ)
    , end_frequency_(DEFAULT_HISTOGRAM_END_HZ)
    , bin_width_(0)
    , bins_{}
    , statistics_{}
    , scan_cycle_active_(false)
    , initialized_(false) {
}

// ============================================================================
// Initialization
// ============================================================================

ErrorCode HistogramProcessor::initialize(size_t bin_count) noexcept {
    // Validate bin count
    if (bin_count == 0 || bin_count > HISTOGRAM_BUFFER_SIZE) {
        return ErrorCode::INVALID_PARAMETER;
    }

    bin_count_ = bin_count;

    // Calculate bin width
    if (end_frequency_ > start_frequency_) {
        bin_width_ = (end_frequency_ - start_frequency_) / bin_count_;
    }

    // Initialize all bins with frequencies
    for (size_t i = 0; i < bin_count_; ++i) {
        bins_[i].frequency = start_frequency_ + (i * bin_width_) + (bin_width_ / 2);
        bins_[i].clear();
    }

    // Reset statistics
    statistics_.total_samples = 0;
    statistics_.peak_bin = 0;
    statistics_.peak_rssi = RSSI_NOISE_FLOOR_DBM;
    statistics_.active_bins = 0;

    scan_cycle_active_ = false;
    initialized_ = true;
    return ErrorCode::SUCCESS;
}

// ============================================================================
// Public Methods
// ============================================================================

ErrorCode HistogramProcessor::update_histogram(
    const uint8_t* spectrum_data,
    size_t spectrum_size,
    FreqHz center_freq,
    FreqHz bandwidth
) noexcept {
    if (!initialized_) {
        return ErrorCode::INITIALIZATION_INCOMPLETE;
    }

    if (spectrum_data == nullptr || spectrum_size == 0) {
        return ErrorCode::BUFFER_INVALID;
    }

    if (bandwidth == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }

    // Calculate frequency range of this spectrum slice
    const FreqHz slice_start = center_freq - (bandwidth / 2);
    const FreqHz slice_end = center_freq + (bandwidth / 2);
    const FreqHz slice_bin_width = bandwidth / spectrum_size;

    // Map spectrum bins to frequency bins
    for (size_t i = 0; i < spectrum_size; ++i) {
        // Calculate frequency for this spectrum bin
        const FreqHz bin_freq = slice_start + (i * slice_bin_width) + (slice_bin_width / 2);

        // Find corresponding frequency bin
        const int bin_index = find_bin_for_frequency(bin_freq);
        if (bin_index >= 0 && bin_index < static_cast<int>(bin_count_)) {
            // Convert raw RSSI to dBm and update bin
            const RssiValue rssi_dbm = raw_to_dbm(spectrum_data[i]);

            // Only update if RSSI is above noise floor
            if (rssi_dbm > RSSI_NOISE_FLOOR_DBM) {
                bins_[bin_index].update(rssi_dbm);
                statistics_.total_samples++;
            }
        }
    }

    // Update statistics
    RssiValue max_rssi = RSSI_NOISE_FLOOR_DBM;
    uint16_t peak_bin = 0;
    uint16_t active_bins = 0;

    for (size_t i = 0; i < bin_count_; ++i) {
        if (bins_[i].has_data) {
            active_bins++;
            if (bins_[i].current_rssi > max_rssi) {
                max_rssi = bins_[i].current_rssi;
                peak_bin = static_cast<uint16_t>(i);
            }
        }
    }

    statistics_.peak_bin = peak_bin;
    statistics_.peak_rssi = max_rssi;
    statistics_.active_bins = active_bins;

    return ErrorCode::SUCCESS;
}

size_t HistogramProcessor::get_frequency_bins(
    FrequencyBin* bins,
    size_t max_bins
) const noexcept {
    if (bins == nullptr || max_bins == 0 || !initialized_) {
        return 0;
    }

    const size_t num_bins = (bin_count_ < max_bins) ? bin_count_ : max_bins;

    for (size_t i = 0; i < num_bins; ++i) {
        bins[i] = bins_[i];
    }

    return num_bins;
}

size_t HistogramProcessor::get_histogram_data(
    uint16_t* histogram,
    size_t max_length
) const noexcept {
    if (histogram == nullptr || max_length == 0 || !initialized_) {
        return 0;
    }

    const size_t num_entries = (bin_count_ < max_length) ? bin_count_ : max_length;

    for (size_t i = 0; i < num_entries; ++i) {
        // Convert RSSI to display height (0-65535 for uint16_t)
        if (bins_[i].has_data) {
            // Scale display_height (0-255) to uint16_t range
            histogram[i] = static_cast<uint16_t>(bins_[i].display_height) * 256;
        } else {
            histogram[i] = 0;
        }
    }

    return num_entries;
}

void HistogramProcessor::begin_scan_cycle() noexcept {
    scan_cycle_active_ = true;
    // Note: We do NOT clear bin data here - values are held until next scan
    // This implements the "hold until next scan update" requirement
}

void HistogramProcessor::end_scan_cycle() noexcept {
    scan_cycle_active_ = false;
}

void HistogramProcessor::get_statistics(HistogramStatistics& stats) const noexcept {
    stats = statistics_;
}

uint16_t HistogramProcessor::get_active_bin_count() const noexcept {
    return statistics_.active_bins;
}

void HistogramProcessor::set_frequency_range(FreqHz start_freq, FreqHz end_freq) noexcept {
    if (end_freq <= start_freq || !initialized_) {
        return;
    }

    start_frequency_ = start_freq;
    end_frequency_ = end_freq;
    bin_width_ = (end_frequency_ - start_frequency_) / bin_count_;

    // Update bin frequencies
    for (size_t i = 0; i < bin_count_; ++i) {
        bins_[i].frequency = start_frequency_ + (i * bin_width_) + (bin_width_ / 2);
    }
}

// ============================================================================
// Private Methods
// ============================================================================

int HistogramProcessor::find_bin_for_frequency(FreqHz frequency) const noexcept {
    if (frequency < start_frequency_ || frequency >= end_frequency_) {
        return -1;
    }

    const FreqHz offset = frequency - start_frequency_;
    const size_t bin_index = static_cast<size_t>(offset / bin_width_);

    if (bin_index >= bin_count_) {
        return -1;
    }

    return static_cast<int>(bin_index);
}

RssiValue HistogramProcessor::raw_to_dbm(uint8_t rssi_raw) const noexcept {
    // Convert raw RSSI (0-255) to dBm
    // RSSI range: -120dBm (0) to -20dBm (255)
    // Formula: dBm = -120 + (raw * 100 / 255)
    // Simplified integer math: dBm = -120 + ((raw * 100) >> 8)
    // More accurate: dBm = -120 + ((raw * 100) / 255)
    const int32_t scaled = (static_cast<int32_t>(rssi_raw) * 100) / 255;
    return RSSI_MIN_DBM + scaled;
}

} // namespace drone_analyzer