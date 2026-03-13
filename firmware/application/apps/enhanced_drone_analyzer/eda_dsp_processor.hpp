/**
 * @file eda_dsp_processor.hpp
 * @brief Pure DSP processing layer for Enhanced Drone Analyzer
 *
 * DIAMOND CODE COMPLIANCE:
 * - Stack allocation only (no heap allocation)
 * - Pure DSP processing (no UI code mixed in)
 * - Uses Thread Flags for event signaling
 * - Zero-Overhead Abstraction (inline functions, constexpr)
 * - Thread-safe communication with UI layer
 *
 * DATA FLOW:
 * - DSP Processor processes spectrum data from M0 coprocessor
 * - Generates DisplayDataSnapshot for UI rendering
 * - Signals UI via Thread Flags when data is ready
 *
 * Target: STM32F405 (ARM Cortex-M4, 128KB RAM)
 * Environment: ChibiOS RTOS
 */

#ifndef EDA_DSP_PROCESSOR_HPP_
#define EDA_DSP_PROCESSOR_HPP_

// ============================================================================
// STANDARD LIBRARY HEADERS
// ============================================================================
#include <cstdint>
#include <cstddef>

// ============================================================================
// THIRD-PARTY LIBRARY HEADERS
// ============================================================================
#include <ch.h>

// ============================================================================
// PROJECT HEADERS
// ============================================================================
#include "eda_thread_sync.hpp"
#include "eda_runtime_constants.hpp"
#include "dsp_display_types.hpp"
#include "dsp_spectrum_processor.hpp"
#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer::dsp {

// ============================================================================
// DSP PROCESSOR STATE
// ============================================================================

/**
 * @brief DSP processor state
 * @details Tracks processing state and configuration
 */
class DSPProcessor {
public:
    /**
     * @brief Constructor
     */
    DSPProcessor() noexcept;

    /**
     * @brief Destructor
     */
    ~DSPProcessor() noexcept;

    // Non-copyable, non-movable
    DSPProcessor(const DSPProcessor&) = delete;
    DSPProcessor& operator=(const DSPProcessor&) = delete;
    DSPProcessor(DSPProcessor&&) = delete;
    DSPProcessor& operator=(DSPProcessor&&) = delete;

    // ========================================================================
    // INITIALIZATION
    // ========================================================================

    /**
     * @brief Initialize DSP processor
     * @return true if initialization succeeded
     */
    [[nodiscard]] bool initialize() noexcept;

    /**
     * @brief Deinitialize DSP processor
     */
    void deinitialize() noexcept;

    // ========================================================================
    // PROCESSING
    // ========================================================================

    /**
     * @brief Process spectrum data
     * @param spectrum_data Raw spectrum data from M0 coprocessor
     * @param data_size Size of spectrum data
     * @note Generates DisplayDataSnapshot for UI rendering
     */
    void process_spectrum_data(const uint8_t* spectrum_data, size_t data_size) noexcept;

    /**
     * @brief Process detection event
     * @param frequency Detection frequency (Hz)
     * @param rssi Detection RSSI (dBm)
     * @param drone_type Detected drone type
     * @param threat_level Detected threat level
     */
    void process_detection_event(uint64_t frequency, int rssi,
                               DroneType drone_type, ThreatLevel threat_level) noexcept;

    /**
     * @brief Process FHSS detection event
     * @param hop_frequency Hop frequency (Hz)
     * @param hop_count Number of hops detected
     */
    void process_fhss_detection(uint64_t hop_frequency, unsigned int hop_count) noexcept;

    // ========================================================================
    // DATA ACCESS
    // ========================================================================

    /**
     * @brief Get display data snapshot
     * @return Current display data snapshot
     * @note Thread-safe access (uses internal mutex)
     */
    [[nodiscard]] DisplayDataSnapshot get_display_snapshot() const noexcept;

    /**
     * @brief Try to get display data snapshot (non-blocking)
     * @param out_snapshot Output snapshot
     * @return true if snapshot was acquired, false if locked
     */
    [[nodiscard]] bool try_get_display_snapshot(DisplayDataSnapshot& out_snapshot) const noexcept;

    /**
     * @brief Get detection count
     * @return Number of detections processed
     */
    [[nodiscard]] unsigned int get_detection_count() const noexcept;

    /**
     * @brief Reset detection count
     */
    void reset_detection_count() noexcept;

    // ========================================================================
    // CONFIGURATION
    // ========================================================================

    /**
     * @brief Set detection threshold
     * @param threshold_dbm Detection threshold (dBm)
     */
    void set_detection_threshold(int threshold_dbm) noexcept;

    /**
     * @brief Get detection threshold
     * @return Detection threshold (dBm)
     */
    [[nodiscard]] int get_detection_threshold() const noexcept;

    /**
     * @brief Enable/disable FHSS detection
     * @param enabled true to enable FHSS detection
     */
    void set_fhss_enabled(bool enabled) noexcept;

    /**
     * @brief Check if FHSS detection is enabled
     * @return true if FHSS detection is enabled
     */
    [[nodiscard]] bool is_fhss_enabled() const noexcept;

    // ========================================================================
    // THREAD FLAG SENDER
    // ========================================================================

    /**
     * @brief Get thread flag sender
     * @return Thread flag sender for signaling UI
     */
    [[nodiscard]] sync::ThreadFlagSender& get_flag_sender() noexcept;

    /**
     * @brief Set target thread for flag signaling
     * @param thread Target thread (UI thread)
     */
    void set_target_thread(::Thread* thread) noexcept;

private:
    // ========================================================================
    // INTERNAL STATE
    // ========================================================================

    /**
     * @brief Internal processing state
     */
    struct InternalState {
        bool initialized;
        bool fhss_enabled;
        int detection_threshold_dbm;
        unsigned int detection_count;
        DisplayDataSnapshot display_snapshot;
        sync::ThreadFlagSender flag_sender;
        mutable Mutex data_mutex;
    };

    InternalState state_;

    // ========================================================================
    // INTERNAL PROCESSING
    // ========================================================================

    /**
     * @brief Update display snapshot from spectrum data
     * @param spectrum_data Raw spectrum data
     * @param data_size Size of spectrum data
     */
    void update_display_snapshot(const uint8_t* spectrum_data, size_t data_size) noexcept;

    /**
     * @brief Detect drone from spectrum data
     * @param spectrum_data Raw spectrum data
     * @param data_size Size of spectrum data
     * @return Detection result (frequency, rssi, type, threat)
     */
    struct DetectionResult {
        uint64_t frequency;
        int rssi;
        DroneType drone_type;
        ThreatLevel threat_level;
    };
    [[nodiscard]] DetectionResult detect_drone(const uint8_t* spectrum_data, size_t data_size) noexcept;

    /**
     * @brief Detect FHSS from spectrum data
     * @param spectrum_data Raw spectrum data
     * @param data_size Size of spectrum data
     * @return FHSS detection result (frequency, hop_count)
     */
    struct FHSSResult {
        uint64_t hop_frequency;
        unsigned int hop_count;
    };
    [[nodiscard]] FHSSResult detect_fhss(const uint8_t* spectrum_data, size_t data_size) noexcept;

    /**
     * @brief Calculate noise floor
     * @param spectrum_data Raw spectrum data
     * @param data_size Size of spectrum data
     * @return Noise floor level
     */
    [[nodiscard]] int calculate_noise_floor(const uint8_t* spectrum_data, size_t data_size) const noexcept;

    /**
     * @brief Find peak in spectrum data
     * @param spectrum_data Raw spectrum data
     * @param data_size Size of spectrum data
     * @return Peak index and value
     */
    struct PeakResult {
        size_t peak_index;
        uint8_t peak_value;
    };
    [[nodiscard]] PeakResult find_peak(const uint8_t* spectrum_data, size_t data_size) const noexcept;
};

} // namespace ui::apps::enhanced_drone_analyzer::dsp

#endif // EDA_DSP_PROCESSOR_HPP_
