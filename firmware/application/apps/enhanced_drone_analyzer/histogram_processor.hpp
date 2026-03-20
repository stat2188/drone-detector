#ifndef HISTOGRAM_PROCESSOR_HPP
#define HISTOGRAM_PROCESSOR_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "drone_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Frequency-indexed histogram bin with RSSI delta tracking
 * @note Maps to actual frequency, tracks RSSI changes between scans
 * @note Color: RED (RSSI increased), GREEN (decreased), YELLOW (stable)
 */
struct FrequencyBin {
    FreqHz frequency;            // Center frequency of bin (Hz)
    RssiValue current_rssi;      // Current scan RSSI (dBm)
    RssiValue previous_rssi;     // Previous scan RSSI (dBm)
    uint8_t display_height;      // Normalized height (0-255) for UI
    uint32_t color;              // RGBA color: RED, GREEN, or YELLOW
    bool has_data;               // True if bin contains valid data
    
    /**
     * @brief Default constructor
     */
    FrequencyBin() noexcept
        : frequency(0)
        , current_rssi(RSSI_NOISE_FLOOR_DBM)
        , previous_rssi(RSSI_NOISE_FLOOR_DBM)
        , display_height(0)
        , color(COLOR_UNKNOWN_THREAT)
        , has_data(false) {}
    
    /**
     * @brief Update bin with new RSSI value
     * @param new_rssi New RSSI reading (dBm)
     * @return Color based on RSSI delta
     */
    uint32_t update(RssiValue new_rssi) noexcept {
        previous_rssi = current_rssi;
        current_rssi = new_rssi;
        has_data = true;
        
        // Calculate RSSI delta
        const int32_t delta = current_rssi - previous_rssi;
        
        // Determine color based on delta
        // RED: Signal increased (approaching drone)
        // GREEN: Signal decreased (receding drone)
        // YELLOW: Stable (within ±2dB)
        if (delta > 2) {
            color = COLOR_CRITICAL_THREAT;  // RED
        } else if (delta < -2) {
            color = COLOR_LOW_THREAT;       // GREEN
        } else {
            color = COLOR_MEDIUM_THREAT;    // YELLOW
        }
        
        // Normalize RSSI to display height (0-255)
        // RSSI range: -120dBm to -20dBm
        const int32_t rssi_range = RSSI_MAX_DBM - RSSI_MIN_DBM;
        const int32_t rssi_clamped = (current_rssi < RSSI_MIN_DBM) ? RSSI_MIN_DBM :
                                     (current_rssi > RSSI_MAX_DBM) ? RSSI_MAX_DBM : current_rssi;
        const int32_t rssi_normalized = rssi_clamped - RSSI_MIN_DBM;
        display_height = static_cast<uint8_t>((rssi_normalized * 255) / rssi_range);
        
        return color;
    }
    
    /**
     * @brief Clear bin data
     */
    void clear() noexcept {
        current_rssi = RSSI_NOISE_FLOOR_DBM;
        previous_rssi = RSSI_NOISE_FLOOR_DBM;
        display_height = 0;
        color = COLOR_UNKNOWN_THREAT;
        has_data = false;
    }
};

/**
 * @brief Histogram statistics
 */
struct HistogramStatistics {
    uint32_t total_samples;      // Total number of samples processed
    uint16_t peak_bin;           // Bin with highest RSSI
    RssiValue peak_rssi;         // RSSI at peak bin
    uint16_t active_bins;        // Number of bins with data
};

/**
 * @brief Frequency-indexed histogram processor
 * @note Maps spectrum bins to actual frequencies
 * @note Tracks RSSI changes between scan cycles
 * @note Provides color-coded display data
 */
class HistogramProcessor {
public:
    /**
     * @brief Default constructor
     */
    HistogramProcessor() noexcept;

    /**
     * @brief Initialize histogram processor
     * @param bin_count Number of frequency bins (max 128)
     * @return ErrorCode::SUCCESS if initialized, error code otherwise
     */
    [[nodiscard]] ErrorCode initialize(size_t bin_count) noexcept;

    /**
     * @brief Update histogram with spectrum data for a frequency range
     * @param spectrum_data Spectrum power values (0-255)
     * @param spectrum_size Number of spectrum bins (typically 256)
     * @param center_freq Center frequency of this spectrum slice (Hz)
     * @param bandwidth Bandwidth of spectrum slice (Hz)
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     * @note Maps spectrum bins to frequency bins and updates RSSI
     */
    [[nodiscard]] ErrorCode update_histogram(
        const uint8_t* spectrum_data,
        size_t spectrum_size,
        FreqHz center_freq,
        FreqHz bandwidth
    ) noexcept;

    /**
     * @brief Get frequency bins for display
     * @param bins Output buffer for frequency bins
     * @param max_bins Maximum number of bins to return
     * @return Number of bins populated
     */
    [[nodiscard]] size_t get_frequency_bins(
        FrequencyBin* bins,
        size_t max_bins
    ) const noexcept;

    /**
     * @brief Get histogram data for legacy display (backward compatibility)
     * @param histogram Output buffer for histogram data
     * @param max_length Maximum buffer length
     * @return Number of entries copied
     */
    [[nodiscard]] size_t get_histogram_data(
        uint16_t* histogram,
        size_t max_length
    ) const noexcept;

    /**
     * @brief Reset histogram data (hold values until next scan)
     * @note Call this at the start of each scan cycle
     */
    void begin_scan_cycle() noexcept;

    /**
     * @brief Finalize scan cycle
     * @note Call this at the end of each scan cycle
     */
    void end_scan_cycle() noexcept;

    /**
     * @brief Get histogram statistics
     * @param stats Output statistics structure
     */
    void get_statistics(HistogramStatistics& stats) const noexcept;

    /**
     * @brief Get active bin count (bins with data)
     * @return Number of active bins
     */
    [[nodiscard]] uint16_t get_active_bin_count() const noexcept;

    /**
     * @brief Set frequency range for histogram
     * @param start_freq Start frequency (Hz)
     * @param end_freq End frequency (Hz)
     */
    void set_frequency_range(FreqHz start_freq, FreqHz end_freq) noexcept;

private:
    /**
     * @brief Find bin index for a frequency
     * @param frequency Frequency to find
     * @return Bin index, or -1 if not in range
     */
    [[nodiscard]] int find_bin_for_frequency(FreqHz frequency) const noexcept;

    /**
     * @brief Convert RSSI raw value to dBm
     * @param rssi_raw Raw RSSI value (0-255)
     * @return RSSI in dBm
     */
    [[nodiscard]] RssiValue raw_to_dbm(uint8_t rssi_raw) const noexcept;

    // Histogram configuration
    size_t bin_count_;
    FreqHz start_frequency_;
    FreqHz end_frequency_;
    FreqHz bin_width_;  // Frequency range per bin

    // Frequency bins (fixed-size array, no heap)
    std::array<FrequencyBin, HISTOGRAM_BUFFER_SIZE> bins_;

    // Histogram statistics
    HistogramStatistics statistics_;

    // Scan cycle tracking
    bool scan_cycle_active_;

    // Initialization flag
    bool initialized_;
};

} // namespace drone_analyzer

#endif // HISTOGRAM_PROCESSOR_HPP