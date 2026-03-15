#include "spectrum_processor.hpp"

namespace drone_analyzer {

// ============================================================================
// Constructor / Destructor
// ============================================================================

SpectrumProcessor::SpectrumProcessor() noexcept
    : total_processed_(0)
    , signals_detected_(0)
    , initialized_(false)
    , peak_buffer_{} {
}

// ============================================================================
// Initialization
// ============================================================================

ErrorCode SpectrumProcessor::initialize() noexcept {
    // Reset statistics
    total_processed_ = 0;
    signals_detected_ = 0;

    // Clear peak buffer
    for (size_t i = 0; i < MAX_PEAKS; ++i) {
        peak_buffer_[i] = 0;
    }

    initialized_ = true;
    return ErrorCode::SUCCESS;
}

// ============================================================================
// Public Methods
// ============================================================================

ErrorCode SpectrumProcessor::process_spectrum_data(
    const uint8_t* spectrum_data,
    size_t length,
    FreqHz start_frequency,
    FreqHz frequency_step,
    SpectrumResult& result
) noexcept {
    // Validate parameters
    ErrorCode error = validate_spectrum_params(spectrum_data, length);
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    // Initialize result
    result.peak_frequency = 0;
    result.peak_amplitude = 0;
    result.average_power = 0;
    result.signal_detected = false;
    result.signal_count = 0;
    result.noise_floor = 0;

    // Calculate peak frequency and amplitude
    error = calculate_peak_frequency(
        spectrum_data,
        length,
        start_frequency,
        frequency_step,
        result.peak_frequency,
        result.peak_amplitude
    );
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    // Calculate average power
    error = calculate_average_power(
        spectrum_data,
        length,
        result.average_power
    );
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    // Calculate noise floor
    error = calculate_noise_floor(
        spectrum_data,
        length,
        result.noise_floor
    );
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    // Detect signals using threshold (noise floor + margin)
    uint8_t detection_threshold = static_cast<uint8_t>(
        (result.noise_floor > 20) ? (result.noise_floor - 20) : 10
    );

    error = detect_signal(
        spectrum_data,
        length,
        detection_threshold,
        result.signal_detected,
        result.signal_count
    );
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    // Update statistics
    total_processed_++;
    if (result.signal_detected) {
        signals_detected_++;
    }

    return ErrorCode::SUCCESS;
}

ErrorCode SpectrumProcessor::calculate_peak_frequency(
    const uint8_t* spectrum_data,
    size_t length,
    FreqHz start_frequency,
    FreqHz frequency_step,
    FreqHz& peak_frequency,
    uint16_t& peak_amplitude
) const noexcept {
    // Validate parameters
    if (spectrum_data == nullptr || length == 0) {
        return ErrorCode::BUFFER_INVALID;
    }

    // Find peak amplitude and index
    uint8_t max_amplitude = 0;
    size_t peak_index = 0;

    for (size_t i = 0; i < length; ++i) {
        if (spectrum_data[i] > max_amplitude) {
            max_amplitude = spectrum_data[i];
            peak_index = i;
        }
    }

    // Calculate peak frequency using integer arithmetic
    // peak_frequency = start_frequency + (peak_index * frequency_step)
    peak_frequency = start_frequency + (static_cast<FreqHz>(peak_index) * frequency_step);
    peak_amplitude = static_cast<uint16_t>(max_amplitude);

    return ErrorCode::SUCCESS;
}

ErrorCode SpectrumProcessor::calculate_average_power(
    const uint8_t* spectrum_data,
    size_t length,
    uint16_t& average_power
) const noexcept {
    // Validate parameters
    if (spectrum_data == nullptr || length == 0) {
        return ErrorCode::BUFFER_INVALID;
    }

    // Calculate sum using uint32_t to avoid overflow
    uint32_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        sum += static_cast<uint32_t>(spectrum_data[i]);
    }

    // Calculate average using bit shift (divide by power of 2 approximation)
    // For non-power-of-2 lengths, use multiplication approximation
    if (length >= 256) {
        // Divide by 256 using bit shift
        average_power = static_cast<uint16_t>(sum >> 8);
    } else if (length >= 128) {
        // Divide by 128 using bit shift
        average_power = static_cast<uint16_t>(sum >> 7);
    } else if (length >= 64) {
        // Divide by 64 using bit shift
        average_power = static_cast<uint16_t>(sum >> 6);
    } else if (length >= 32) {
        // Divide by 32 using bit shift
        average_power = static_cast<uint16_t>(sum >> 5);
    } else if (length >= 16) {
        // Divide by 16 using bit shift
        average_power = static_cast<uint16_t>(sum >> 4);
    } else if (length >= 8) {
        // Divide by 8 using bit shift
        average_power = static_cast<uint16_t>(sum >> 3);
    } else if (length >= 4) {
        // Divide by 4 using bit shift
        average_power = static_cast<uint16_t>(sum >> 2);
    } else if (length >= 2) {
        // Divide by 2 using bit shift
        average_power = static_cast<uint16_t>(sum >> 1);
    } else {
        // length == 1, no division needed
        average_power = static_cast<uint16_t>(sum);
    }

    return ErrorCode::SUCCESS;
}

ErrorCode SpectrumProcessor::detect_signal(
    const uint8_t* spectrum_data,
    size_t length,
    uint8_t threshold,
    bool& signal_detected,
    uint8_t& signal_count
) const noexcept {
    // Validate parameters
    if (spectrum_data == nullptr || length == 0) {
        return ErrorCode::BUFFER_INVALID;
    }

    signal_detected = false;
    signal_count = 0;

    // Simple threshold detection
    // Count samples above threshold
    size_t above_threshold = 0;
    for (size_t i = 0; i < length; ++i) {
        if (spectrum_data[i] >= threshold) {
            above_threshold++;
        }
    }

    // Signal detected if enough samples above threshold
    // Use 5% of length as minimum signal requirement
    const size_t min_signal_samples = (length + 19) / 20;  // ceil(length / 20)
    if (above_threshold >= min_signal_samples) {
        signal_detected = true;

        // Count distinct signals using peak finding
        size_t peaks = find_peaks(spectrum_data, length, threshold, peak_buffer_.data(), MAX_PEAKS);
        signal_count = static_cast<uint8_t>((peaks > 255) ? 255 : peaks);
    }

    return ErrorCode::SUCCESS;
}

ErrorCode SpectrumProcessor::calculate_noise_floor(
    const uint8_t* spectrum_data,
    size_t length,
    uint16_t& noise_floor
) const noexcept {
    // Validate parameters
    if (spectrum_data == nullptr || length == 0) {
        return ErrorCode::BUFFER_INVALID;
    }

    // Use percentile-based estimation (25th percentile)
    // Sort a subset of samples to estimate noise floor
    constexpr size_t SAMPLE_SIZE = 32;
    uint8_t samples[SAMPLE_SIZE];

    // Sample every Nth element to get representative samples
    const size_t step = (length + SAMPLE_SIZE - 1) / SAMPLE_SIZE;
    for (size_t i = 0; i < SAMPLE_SIZE; ++i) {
        const size_t index = (i * step) < length ? (i * step) : (length - 1);
        samples[i] = spectrum_data[index];
    }

    // Simple insertion sort (small array, no heap allocation)
    for (size_t i = 1; i < SAMPLE_SIZE; ++i) {
        const uint8_t key = samples[i];
        size_t j = i;
        while (j > 0 && samples[j - 1] > key) {
            samples[j] = samples[j - 1];
            j--;
        }
        samples[j] = key;
    }

    // 25th percentile is at index 8 (32 * 0.25 = 8)
    noise_floor = static_cast<uint16_t>(samples[8]);

    return ErrorCode::SUCCESS;
}

void SpectrumProcessor::reset() noexcept {
    total_processed_ = 0;
    signals_detected_ = 0;

    for (size_t i = 0; i < MAX_PEAKS; ++i) {
        peak_buffer_[i] = 0;
    }
}

void SpectrumProcessor::get_statistics(
    uint32_t& total_processed,
    uint32_t& signals_detected
) const noexcept {
    total_processed = total_processed_;
    signals_detected = signals_detected_;
}

// ============================================================================
// Private Methods
// ============================================================================

ErrorCode SpectrumProcessor::validate_spectrum_params(
    const uint8_t* spectrum_data,
    size_t length
) const noexcept {
    if (spectrum_data == nullptr) {
        return ErrorCode::BUFFER_INVALID;
    }

    if (length == 0) {
        return ErrorCode::BUFFER_EMPTY;
    }

    if (length > SPECTRUM_BUFFER_SIZE) {
        return ErrorCode::BUFFER_INVALID;
    }

    return ErrorCode::SUCCESS;
}

size_t SpectrumProcessor::find_peaks(
    const uint8_t* spectrum_data,
    size_t length,
    uint8_t threshold,
    uint16_t* peaks,
    size_t max_peaks
) const noexcept {
    if (spectrum_data == nullptr || length < 3 || peaks == nullptr || max_peaks == 0) {
        return 0;
    }

    size_t peak_count = 0;

    // Simple peak detection: sample > threshold and > neighbors
    for (size_t i = 1; i < length - 1; ++i) {
        if (spectrum_data[i] >= threshold &&
            spectrum_data[i] >= spectrum_data[i - 1] &&
            spectrum_data[i] >= spectrum_data[i + 1]) {

            if (peak_count < max_peaks) {
                peaks[peak_count] = static_cast<uint16_t>(i);
                peak_count++;
            } else {
                break;  // Buffer full
            }
        }
    }

    return peak_count;
}

int16_t SpectrumProcessor::calculate_snr(
    uint16_t signal_power,
    uint16_t noise_floor
) const noexcept {
    // Avoid division by zero
    if (noise_floor == 0) {
        noise_floor = 1;
    }

    // Calculate SNR in dB using fixed-point (8.8 format)
    // SNR = 20 * log10(signal / noise)
    // Use approximation: log10(x) ≈ (x - 1) / (x + 1) * 3.321928 (for x near 1)
    // Or use lookup table for common values

    // Simple approximation for embedded systems
    // SNR (dB) ≈ (signal_power - noise_floor) / 2.5
    const int16_t diff = static_cast<int16_t>(signal_power) - static_cast<int16_t>(noise_floor);
    if (diff <= 0) {
        return 0;
    }

    // Use bit shift for division by 2.5 (multiply by 0.4 ≈ 102/256)
    const int16_t snr = (diff * 102) >> 8;
    return snr;
}

} // namespace drone_analyzer
