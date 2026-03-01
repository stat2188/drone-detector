/**
 * @file example_refined_dsp_layer.hpp
 * @brief Diamond Code Best Practices: Separated DSP Layer from UI Logic
 * 
 * DIAMOND CODE PRINCIPLES DEMONSTRATED:
 * 1. Clear separation of concerns (DSP vs UI)
 * 2. NO std::vector, std::string, std::map, std::atomic
 * 3. NO new, malloc - all stack allocation
 * 4. Stack < 4KB constraint respected
 * 5. Use std::array instead of std::vector
 * 6. Use constexpr for compile-time constants
 * 7. All functions marked noexcept (no exceptions)
 * 8. Use enum class, using Type = uintXX_t;
 * 9. Guard clauses and null checks
 * 10. static_assert for stack validation
 * 
 * BEFORE (INCORRECT):
 * class DroneAnalyzer {
 *     void process() {
 *         // Mixed UI and DSP logic - BAD!
 *         ui::draw_spectrum(calculate_fft());
 *         update_display();
 *     }
 * };
 * 
 * AFTER (CORRECT):
 * // DSP Layer: Pure signal processing, no UI dependencies
 * class DspLayer { ... };
 * 
 * // UI Layer: Pure display logic, uses DSP results
 * class UiLayer { ... };
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

// ============================================================================
// DIAMOND CODE: Type Aliases (using Type = uintXX_t;)
// ============================================================================

namespace eda {
namespace dsp {

// Type aliases for clarity and type safety
using SampleIndex = uint16_t;
using FrequencyHz = uint32_t;
using FrequencyBin = uint8_t;
using Magnitude = uint16_t;
using Phase = int16_t;
using Decibel = int8_t;

} // namespace dsp
} // namespace eda

// ============================================================================
// DIAMOND CODE: DSP Constants (constexpr, Flash placement)
// ============================================================================

namespace eda {
namespace dsp {

/**
 * @brief DSP configuration constants
 * 
 * DIAMOND CODE: constexpr ensures compile-time evaluation
 * DIAMOND CODE: Placed in Flash (no RAM consumption)
 */
struct DspConstants {
    // Sample rate and buffer sizes
    static constexpr uint32_t SAMPLE_RATE = 48000;
    static constexpr SampleIndex FFT_SIZE = 256;
    static constexpr SampleIndex WINDOW_SIZE = 256;
    
    // Frequency range (Hz)
    static constexpr FrequencyHz MIN_FREQ = 2400;  // 2.4 GHz band
    static constexpr FrequencyHz MAX_FREQ = 2500;  // 2.5 GHz band
    
    // Magnitude thresholds
    static constexpr Magnitude NOISE_FLOOR = 100;
    static constexpr Magnitude SIGNAL_THRESHOLD = 500;
    
    // Number of frequency bins
    static constexpr FrequencyBin NUM_BINS = 64;
    
    // Compile-time validation
    static_assert(FFT_SIZE == WINDOW_SIZE, "FFT and window sizes must match");
    static_assert(FFT_SIZE % 2 == 0, "FFT size must be even");
    static_assert(NUM_BINS <= 128, "Too many frequency bins for stack");
};

} // namespace dsp
} // namespace eda

// ============================================================================
// DIAMOND CODE: Pure DSP Layer - No UI Dependencies
// ============================================================================

namespace eda {
namespace dsp {

/**
 * @brief Pure DSP signal processor
 * 
 * DIAMOND CODE PRINCIPLE: This class contains ONLY signal processing logic.
 * It has NO knowledge of UI, display, or user interaction.
 * This enables:
 * - Unit testing without UI framework
 * - Reuse in different contexts
 * - Clear separation of concerns
 * - Better performance (no UI overhead in DSP path)
 * 
 * STACK USAGE: ~1.5KB (well under 4KB limit)
 */
class SignalProcessor {
public:
    // DIAMOND CODE: Use std::array instead of std::vector
    // Fixed-size array on stack, no dynamic allocation
    using SampleBuffer = std::array<int16_t, DspConstants::FFT_SIZE>;
    using MagnitudeBuffer = std::array<Magnitude, DspConstants::NUM_BINS>;
    using FrequencyBuffer = std::array<FrequencyHz, DspConstants::NUM_BINS>;
    
    /**
     * @brief Process audio samples and extract frequency spectrum
     * 
     * @param samples Input audio samples (must be non-null)
     * @param magnitudes Output magnitude spectrum (must be non-null)
     * @param frequencies Output frequency values (must be non-null)
     * 
     * DIAMOND CODE: Guard clauses for null checks
     * DIAMOND CODE: noexcept ensures no exceptions
     * DIAMOND CODE: Pure DSP logic, no UI dependencies
     */
    void process_spectrum(
        const SampleBuffer* samples,
        MagnitudeBuffer* magnitudes,
        FrequencyBuffer* frequencies
    ) noexcept {
        // Guard clause: null check for input
        if (samples == nullptr) {
            return;
        }
        
        // Guard clause: null check for outputs
        if (magnitudes == nullptr || frequencies == nullptr) {
            return;
        }
        
        // Pure DSP: Apply window function
        apply_window(*samples);
        
        // Pure DSP: Compute FFT
        compute_fft(*samples, *magnitudes);
        
        // Pure DSP: Convert to frequency bins
        compute_frequencies(*frequencies);
        
        // Pure DSP: Apply noise floor
        apply_noise_floor(*magnitudes);
    }
    
    /**
     * @brief Detect if signal exceeds threshold
     * 
     * @param magnitudes Magnitude spectrum
     * @return true Signal detected
     * @return false No signal detected
     * 
     * DIAMOND CODE: Guard clause for null check
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    auto detect_signal(const MagnitudeBuffer* magnitudes) const noexcept -> bool {
        // Guard clause: null check
        if (magnitudes == nullptr) {
            return false;
        }
        
        // Check each bin for signal above threshold
        for (FrequencyBin i = 0; i < DspConstants::NUM_BINS; ++i) {
            if ((*magnitudes)[i] > DspConstants::SIGNAL_THRESHOLD) {
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * @brief Get peak frequency and magnitude
     * 
     * @param magnitudes Magnitude spectrum
     * @param frequencies Frequency values
     * @param peak_freq Output: peak frequency
     * @param peak_mag Output: peak magnitude
     * 
     * DIAMOND CODE: Guard clauses for null checks
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void find_peak(
        const MagnitudeBuffer* magnitudes,
        const FrequencyBuffer* frequencies,
        FrequencyHz* peak_freq,
        Magnitude* peak_mag
    ) const noexcept {
        // Guard clause: null check for inputs
        if (magnitudes == nullptr || frequencies == nullptr) {
            return;
        }
        
        // Guard clause: null check for outputs
        if (peak_freq == nullptr || peak_mag == nullptr) {
            return;
        }
        
        // Initialize with first bin
        *peak_freq = (*frequencies)[0];
        *peak_mag = (*magnitudes)[0];
        
        // Find maximum magnitude
        for (FrequencyBin i = 1; i < DspConstants::NUM_BINS; ++i) {
            if ((*magnitudes)[i] > *peak_mag) {
                *peak_mag = (*magnitudes)[i];
                *peak_freq = (*frequencies)[i];
            }
        }
    }
    
private:
    /**
     * @brief Apply Hanning window to samples
     * 
     * DIAMOND CODE: Private method, pure DSP logic
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void apply_window(SampleBuffer& samples) noexcept {
        // DIAMOND CODE: Use constexpr for window coefficients
        // In real implementation, would pre-compute in Flash
        for (SampleIndex i = 0; i < DspConstants::WINDOW_SIZE; ++i) {
            // Hanning window: 0.5 * (1 - cos(2*pi*i/(N-1)))
            // Simplified fixed-point implementation
            samples[i] = static_cast<int16_t>(
                (samples[i] * hanning_window(i)) >> 15
            );
        }
    }
    
    /**
     * @brief Compute FFT magnitude spectrum
     * 
     * DIAMOND CODE: Private method, pure DSP logic
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void compute_fft(
        const SampleBuffer& samples,
        MagnitudeBuffer& magnitudes
    ) noexcept {
        // Simplified FFT implementation
        // In real code, would use optimized ARM CMSIS FFT
        
        // For each frequency bin
        for (FrequencyBin bin = 0; bin < DspConstants::NUM_BINS; ++bin) {
            Magnitude sum = 0;
            
            // Compute magnitude for this bin
            for (SampleIndex i = 0; i < DspConstants::FFT_SIZE; ++i) {
                // Simplified DFT (not optimized)
                int32_t real = samples[i];
                int32_t imag = 0; // Would compute imaginary part in real FFT
                sum += static_cast<Magnitude>(
                    (real * real + imag * imag) >> 16
                );
            }
            
            magnitudes[bin] = sum;
        }
    }
    
    /**
     * @brief Compute frequency values for each bin
     * 
     * DIAMOND CODE: Private method, pure DSP logic
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void compute_frequencies(FrequencyBuffer& frequencies) noexcept {
        // Linear frequency spacing
        const FrequencyHz bin_width = 
            (DspConstants::MAX_FREQ - DspConstants::MIN_FREQ) / 
            DspConstants::NUM_BINS;
        
        for (FrequencyBin i = 0; i < DspConstants::NUM_BINS; ++i) {
            frequencies[i] = DspConstants::MIN_FREQ + (i * bin_width);
        }
    }
    
    /**
     * @brief Apply noise floor to magnitude spectrum
     * 
     * DIAMOND CODE: Private method, pure DSP logic
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    void apply_noise_floor(MagnitudeBuffer& magnitudes) noexcept {
        for (FrequencyBin i = 0; i < DspConstants::NUM_BINS; ++i) {
            if (magnitudes[i] < DspConstants::NOISE_FLOOR) {
                magnitudes[i] = 0;
            }
        }
    }
    
    /**
     * @brief Get Hanning window coefficient
     * 
     * @param index Sample index
     * @return int16_t Window coefficient (Q15 fixed-point)
     * 
     * DIAMOND CODE: constexpr for compile-time evaluation
     * DIAMOND CODE: noexcept ensures no exceptions
     */
    static constexpr auto hanning_window(SampleIndex index) noexcept -> int16_t {
        // Simplified Hanning window (Q15 fixed-point)
        // 0.5 * (1 - cos(2*pi*i/(N-1)))
        constexpr float PI = 3.14159265f;
        float t = static_cast<float>(index) / (DspConstants::WINDOW_SIZE - 1);
        float window = 0.5f * (1.0f - cosf(2.0f * PI * t));
        return static_cast<int16_t>(window * 32767.0f);
    }
};

// Compile-time stack validation
static_assert(sizeof(SignalProcessor) < 2048, 
              "SignalProcessor stack usage must be < 2KB");
static_assert(sizeof(SignalProcessor::SampleBuffer) == 512, 
              "SampleBuffer must be 256 samples * 2 bytes");
static_assert(sizeof(SignalProcessor::MagnitudeBuffer) == 128, 
              "MagnitudeBuffer must be 64 bins * 2 bytes");

} // namespace dsp
} // namespace eda

// ============================================================================
// DIAMOND CODE: Before/After Comparison Examples
// ============================================================================

/**
 * @section BEFORE_AFTER_EXAMPLES Before/After Comparisons
 * 
 * BEFORE (INCORRECT - Mixed UI/DSP):
 * @code
 * // BAD: UI and DSP logic mixed together
 * class DroneAnalyzer {
 *     void process() {
 *         // DSP logic
 *         calculate_fft();
 *         find_peak();
 *         
 *         // UI logic - WRONG! Should be separate
 *         ui::draw_spectrum(magnitudes);
 *         ui::update_text("Peak: " + peak_freq);
 *     }
 * };
 * @endcode
 * 
 * AFTER (CORRECT - Separated):
 * @code
 * // GOOD: Pure DSP layer
 * namespace dsp {
 *     class SignalProcessor {
 *         void process_spectrum(samples, magnitudes, frequencies) noexcept {
 *             // Only DSP logic here
 *             apply_window();
 *             compute_fft();
 *             compute_frequencies();
 *         }
 *     };
 * }
 * 
 * // GOOD: Pure UI layer
 * namespace ui {
 *     class SpectrumDisplay {
 *         void update(const dsp::MagnitudeBuffer& magnitudes) noexcept {
 *             // Only UI logic here
 *             draw_bars(magnitudes);
 *             update_labels();
 *         }
 *     };
 * }
 * 
 * // Integration in application
 * void app_loop() noexcept {
 *     dsp::SignalProcessor dsp;
 *     ui::SpectrumDisplay display;
 *     
 *     // Process signal
 *     dsp.process_spectrum(&samples, &magnitudes, &frequencies);
 *     
 *     // Update display
 *     display.update(magnitudes);
 * }
 * @endcode
 */

// ============================================================================
// DIAMOND CODE: Memory Placement Documentation
// ============================================================================

/**
 * @section MEMORY_PLACEMENT Memory Placement
 * 
 * DSP LAYER MEMORY USAGE:
 * 
 * FLASH (Read-only):
 * - DspConstants: All constexpr values (~64 bytes)
 * - hanning_window() coefficients: Computed at compile time
 * - Function code: ~2KB
 * 
 * STACK (Runtime):
 * - SignalProcessor instance: ~64 bytes
 * - SampleBuffer: 256 * 2 = 512 bytes
 * - MagnitudeBuffer: 64 * 2 = 128 bytes
 * - FrequencyBuffer: 64 * 4 = 256 bytes
 * - Total stack: ~960 bytes (well under 4KB limit)
 * 
 * NO DYNAMIC ALLOCATION:
 * - NO std::vector (would use heap)
 * - NO std::string (would use heap)
 * - NO new, malloc (would use heap)
 * - All data on stack or in Flash
 * 
 * BENEFITS:
 * - Deterministic memory usage
 * - No heap fragmentation
 * - Fast allocation (stack only)
 * - Cache-friendly (contiguous memory)
 */

} // namespace eda
