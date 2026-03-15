#ifndef SPECTRUM_PROCESSOR_HPP
#define SPECTRUM_PROCESSOR_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "drone_types.hpp"
#include "constants.hpp"
#include "error_handler.hpp"

namespace drone_analyzer {

/**
 * @brief Spectrum data processing result
 * @note Fixed-size structure, no heap allocation
 */
struct SpectrumResult {
    FreqHz peak_frequency;              // Peak frequency in Hz
    uint16_t peak_amplitude;             // Peak amplitude (0-255)
    uint16_t average_power;              // Average power (0-255)
    bool signal_detected;                // Signal detected flag
    uint8_t signal_count;                // Number of signals detected
    uint16_t noise_floor;                // Noise floor level
    uint8_t reserved[7];                 // Reserved for future use (padding)
};

/**
 * @brief Spectrum processor for drone signal analysis
 * @note No floating-point operations, uses integer arithmetic
 * @note No heap allocation, uses fixed-size arrays
 * @note Separated from UI layer (no UI dependencies)
 * @note Removed: wideband spectrum processing, FHSS detection
 */
class SpectrumProcessor {
public:
    /**
     * @brief Default constructor
     */
    SpectrumProcessor() noexcept;

    /**
     * @brief Initialize spectrum processor
     * @return ErrorCode::SUCCESS if initialized, error code otherwise
     */
    [[nodiscard]] ErrorCode initialize() noexcept;

    /**
     * @brief Process spectrum data buffer
     * @param spectrum_data Input spectrum data buffer
     * @param length Length of spectrum data buffer
     * @param start_frequency Start frequency in Hz
     * @param frequency_step Frequency step in Hz
     * @param result Output spectrum result
     * @return ErrorCode::SUCCESS if processed, error code otherwise
     * @pre spectrum_data must not be nullptr
     * @pre length must be <= SPECTRUM_BUFFER_SIZE
     * @note Uses integer arithmetic, no floating-point operations
     * @note Minimizes stack usage with static storage
     */
    [[nodiscard]] ErrorCode process_spectrum_data(
        const uint8_t* spectrum_data,
        size_t length,
        FreqHz start_frequency,
        FreqHz frequency_step,
        SpectrumResult& result
    ) noexcept;

    /**
     * @brief Calculate peak frequency from spectrum data
     * @param spectrum_data Input spectrum data buffer
     * @param length Length of spectrum data buffer
     * @param start_frequency Start frequency in Hz
     * @param frequency_step Frequency step in Hz
     * @param peak_frequency Output peak frequency in Hz
     * @param peak_amplitude Output peak amplitude
     * @return ErrorCode::SUCCESS if calculated, error code otherwise
     * @pre spectrum_data must not be nullptr
     * @pre length must be > 0
     * @note Uses linear scan, no floating-point operations
     */
    [[nodiscard]] ErrorCode calculate_peak_frequency(
        const uint8_t* spectrum_data,
        size_t length,
        FreqHz start_frequency,
        FreqHz frequency_step,
        FreqHz& peak_frequency,
        uint16_t& peak_amplitude
    ) const noexcept;

    /**
     * @brief Calculate average power from spectrum data
     * @param spectrum_data Input spectrum data buffer
     * @param length Length of spectrum data buffer
     * @param average_power Output average power
     * @return ErrorCode::SUCCESS if calculated, error code otherwise
     * @pre spectrum_data must not be nullptr
     * @pre length must be > 0
     * @note Uses integer arithmetic with bit shifts for division
     */
    [[nodiscard]] ErrorCode calculate_average_power(
        const uint8_t* spectrum_data,
        size_t length,
        uint16_t& average_power
    ) const noexcept;

    /**
     * @brief Detect signal presence in spectrum data
     * @param spectrum_data Input spectrum data buffer
     * @param length Length of spectrum data buffer
     * @param threshold Detection threshold (0-255)
     * @param signal_detected Output signal detected flag
     * @param signal_count Output number of signals detected
     * @return ErrorCode::SUCCESS if detected, error code otherwise
     * @pre spectrum_data must not be nullptr
     * @pre length must be > 0
     * @note Uses simple threshold detection, no complex algorithms
     */
    [[nodiscard]] ErrorCode detect_signal(
        const uint8_t* spectrum_data,
        size_t length,
        uint8_t threshold,
        bool& signal_detected,
        uint8_t& signal_count
    ) const noexcept;

    /**
     * @brief Calculate noise floor from spectrum data
     * @param spectrum_data Input spectrum data buffer
     * @param length Length of spectrum data buffer
     * @param noise_floor Output noise floor level
     * @return ErrorCode::SUCCESS if calculated, error code otherwise
     * @pre spectrum_data must not be nullptr
     * @pre length must be > 0
     * @note Uses percentile-based estimation (25th percentile)
     */
    [[nodiscard]] ErrorCode calculate_noise_floor(
        const uint8_t* spectrum_data,
        size_t length,
        uint16_t& noise_floor
    ) const noexcept;

    /**
     * @brief Reset spectrum processor state
     */
    void reset() noexcept;

    /**
     * @brief Get statistics
     * @param total_processed Output total spectra processed
     * @param signals_detected Output total signals detected
     */
    void get_statistics(
        uint32_t& total_processed,
        uint32_t& signals_detected
    ) const noexcept;

private:
    /**
     * @brief Validate spectrum data parameters
     * @param spectrum_data Spectrum data buffer
     * @param length Buffer length
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_spectrum_params(
        const uint8_t* spectrum_data,
        size_t length
    ) const noexcept;

    /**
     * @brief Find peaks in spectrum data
     * @param spectrum_data Input spectrum data buffer
     * @param length Length of spectrum data buffer
     * @param threshold Peak detection threshold
     * @param peaks Output peak indices
     * @param max_peaks Maximum number of peaks to find
     * @return Number of peaks found
     * @note Uses simple peak detection algorithm
     */
    [[nodiscard]] size_t find_peaks(
        const uint8_t* spectrum_data,
        size_t length,
        uint8_t threshold,
        uint16_t* peaks,
        size_t max_peaks
    ) const noexcept;

    /**
     * @brief Calculate signal-to-noise ratio
     * @param signal_power Signal power level
     * @param noise_floor Noise floor level
     * @return SNR in dB (fixed-point, 8.8 format)
     * @note Uses logarithm approximation for integer arithmetic
     */
    [[nodiscard]] int16_t calculate_snr(
        uint16_t signal_power,
        uint16_t noise_floor
    ) const noexcept;

    // Statistics
    uint32_t total_processed_;
    uint32_t signals_detected_;

    // State
    bool initialized_;

    // Static storage for processing (no heap allocation)
    static constexpr size_t MAX_PEAKS = 16;
    std::array<uint16_t, MAX_PEAKS> peak_buffer_;
};

} // namespace drone_analyzer

#endif // SPECTRUM_PROCESSOR_HPP
