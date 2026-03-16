#include "hardware_controller.hpp"
#include "receiver_model.hpp"
#include "rf_path.hpp"

#include "ch.h"

namespace drone_analyzer {

// ============================================================================
// HardwareConfig Implementation
// ============================================================================

HardwareConfig::HardwareConfig() noexcept
    : center_frequency(DEFAULT_SCAN_FREQUENCY_HZ)
    , sample_rate(2000000)  // 2 MHz sample rate
    , gain(20)              // Default gain
    , lna_gain(8)
    , vga_gain(12) {
}

HardwareConfig::HardwareConfig(FreqHz freq, uint32_t rate, uint16_t g) noexcept
    : center_frequency(freq)
    , sample_rate(rate)
    , gain(g)
    , lna_gain(8)
    , vga_gain(12) {
}

// ============================================================================
// RssiSample Implementation
// ============================================================================

RssiSample::RssiSample() noexcept
    : rssi(RSSI_NOISE_FLOOR_DBM)
    , timestamp(0)
    , frequency(0) {
}

RssiSample::RssiSample(RssiValue r, SystemTime t, FreqHz f) noexcept
    : rssi(r)
    , timestamp(t)
    , frequency(f) {
}

// ============================================================================
// HardwareController Implementation
// ============================================================================

HardwareController::HardwareController() noexcept
    : state_(HardwareState::UNINITIALIZED)
    , config_()
    , current_frequency_(0)
    , last_error_(ErrorCode::SUCCESS)
    , retry_count_(0)
    , pll_locked_()
    , streaming_active_()
    , mutex_() {
    
    chMtxObjectInit(&mutex_);
}

HardwareController::~HardwareController() noexcept {
    // Shutdown if still initialized
    if (state_ != HardwareState::UNINITIALIZED) {
        shutdown();
    }
    
    // Note: Do NOT call chMtxDeinit - it doesn't exist in ChibiOS
}

ErrorCode HardwareController::initialize() noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (state_ != HardwareState::UNINITIALIZED) {
        return ErrorCode::INITIALIZATION_INCOMPLETE;
    }
    
    state_ = HardwareState::INITIALIZING;
    
    ErrorCode result = initialize_internal();
    
    if (result == ErrorCode::SUCCESS) {
        state_ = HardwareState::READY;
        current_frequency_ = config_.center_frequency;
        last_error_ = ErrorCode::SUCCESS;
    } else {
        state_ = HardwareState::ERROR;
        last_error_ = result;
    }
    
    return result;
}

ErrorCode HardwareController::initialize_internal() noexcept {
    // Placeholder for hardware initialization
    // In actual implementation, this would:
    // 1. Initialize SPI communication with HackRF One
    // 2. Configure RF frontend
    // 3. Set default gain
    // 4. Enable RF path
    
    // Apply default configuration
    return apply_config_internal(config_);
}

ErrorCode HardwareController::shutdown() noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    // Stop streaming if active
    if (streaming_active_.test()) {
        stop_streaming_internal();
    }
    
    // Placeholder for hardware shutdown
    // In actual implementation, this would:
    // 1. Disable RF path
    // 2. Put hardware in low-power mode
    
    state_ = HardwareState::UNINITIALIZED;
    pll_locked_.clear();
    streaming_active_.clear();
    
    return ErrorCode::SUCCESS;
}

ErrorCode HardwareController::tune_to_frequency(
    FreqHz frequency,
    uint32_t max_retries
) noexcept {
    // Validate frequency
    ErrorCode validate_result = validate_frequency_internal(frequency);
    if (validate_result != ErrorCode::SUCCESS) {
        last_error_ = validate_result;
        return validate_result;
    }
    
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (state_ != HardwareState::READY && state_ != HardwareState::STREAMING) {
        last_error_ = ErrorCode::HARDWARE_NOT_INITIALIZED;
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    state_ = HardwareState::TUNING;
    
    // Retry logic with PLL lock verification
    for (uint32_t retry = 0; retry < max_retries; ++retry) {
        retry_count_ = retry;
        
        // Tune to frequency
        ErrorCode tune_result = tune_internal(frequency);
        if (tune_result != ErrorCode::SUCCESS) {
            if (retry < max_retries - 1) {
                // Wait before retry
                // chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS);
                continue;
            }
            state_ = HardwareState::ERROR;
            last_error_ = tune_result;
            return tune_result;
        }
        
        // Wait for PLL lock
        SystemTime start_time = chTimeNow();
        while (!check_pll_lock_internal()) {
            if (chTimeNow() - start_time > MS2ST(PLL_LOCK_TIMEOUT_MS)) {
                if (retry < max_retries - 1) {
                    chThdSleepMilliseconds(HARDWARE_RETRY_DELAY_MS);
                    break;
                }
                state_ = HardwareState::ERROR;
                last_error_ = ErrorCode::PLL_LOCK_FAILURE;
                return ErrorCode::PLL_LOCK_FAILURE;
            }
            chThdSleepMilliseconds(PLL_LOCK_POLL_INTERVAL_MS);
        }
        
        // PLL locked successfully
        current_frequency_ = frequency;
        pll_locked_.set();
        state_ = streaming_active_.test() ? HardwareState::STREAMING : HardwareState::READY;
        last_error_ = ErrorCode::SUCCESS;
        return ErrorCode::SUCCESS;
    }
    
    state_ = HardwareState::ERROR;
    last_error_ = ErrorCode::PLL_LOCK_FAILURE;
    return ErrorCode::PLL_LOCK_FAILURE;
}

ErrorCode HardwareController::tune_internal(FreqHz frequency) noexcept {
    // Set target frequency using receiver_model
    rf::Frequency rf_freq = rf::Frequency(frequency);
    receiver_model.set_target_frequency(rf_freq);

    // Set gain values
    receiver_model.set_lna(config_.lna_gain);
    receiver_model.set_vga(config_.vga_gain);

    // Set sampling rate for spectrum processing
    receiver_model.set_sampling_rate(config_.sample_rate);

    // Enable RF amplifier for better sensitivity
    receiver_model.set_rf_amp(config_.gain > 20);

    // Set baseband bandwidth for spectrum analysis
    receiver_model.set_baseband_bandwidth(config_.sample_rate);

    return ErrorCode::SUCCESS;
}

bool HardwareController::check_pll_lock_internal() const noexcept {
    // Placeholder for PLL lock check
    // In actual implementation, this would read PLL lock status from hardware
    return pll_locked_.test();
}

ErrorCode HardwareController::start_spectrum_streaming() noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (state_ != HardwareState::READY && state_ != HardwareState::TUNING) {
        last_error_ = ErrorCode::HARDWARE_NOT_INITIALIZED;
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    if (streaming_active_.test()) {
        return ErrorCode::SUCCESS;  // Already streaming
    }
    
    ErrorCode result = start_streaming_internal();
    
    if (result == ErrorCode::SUCCESS) {
        streaming_active_.set();
        state_ = HardwareState::STREAMING;
        last_error_ = ErrorCode::SUCCESS;
    } else {
        last_error_ = result;
    }
    
    return result;
}

ErrorCode HardwareController::start_streaming_internal() noexcept {
    // Enable spectrum streaming mode in receiver_model
    receiver_model.set_modulation(ReceiverModel::Mode::SpectrumAnalysis);

    // Enable RF path for spectrum processing
    rf_path::rf::enable();

    return ErrorCode::SUCCESS;
}

ErrorCode HardwareController::stop_spectrum_streaming() noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (!streaming_active_.test()) {
        return ErrorCode::SUCCESS;  // Not streaming
    }
    
    ErrorCode result = stop_streaming_internal();
    
    if (result == ErrorCode::SUCCESS) {
        streaming_active_.clear();
        state_ = HardwareState::READY;
        last_error_ = ErrorCode::SUCCESS;
    } else {
        last_error_ = result;
    }
    
    return result;
}

ErrorCode HardwareController::stop_streaming_internal() noexcept {
    // Disable RF path
    rf_path::rf::disable();

    return ErrorCode::SUCCESS;
}

ErrorResult<RssiSample> HardwareController::get_rssi_sample() noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (state_ != HardwareState::STREAMING) {
        return ErrorResult<RssiSample>::failure(ErrorCode::HARDWARE_NOT_INITIALIZED);
    }
    
    return read_rssi_internal();
}

ErrorResult<RssiSample> HardwareController::read_rssi_internal() noexcept {
    // Placeholder for reading RSSI
    // In actual implementation, this would:
    // 1. Read RSSI from hardware
    // 2. Get current timestamp
    // 3. Return sample
    
    // Generate sample with current frequency
    RssiSample sample;
    sample.rssi = RSSI_NOISE_FLOOR_DBM;
    sample.timestamp = 0;  // Will be chTimeNow()
    sample.frequency = current_frequency_;
    
    return ErrorResult<RssiSample>::success(sample);
}

ErrorResult<FreqHz> HardwareController::get_current_frequency() const noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (state_ == HardwareState::UNINITIALIZED) {
        return ErrorResult<FreqHz>::failure(ErrorCode::HARDWARE_NOT_INITIALIZED);
    }
    
    return ErrorResult<FreqHz>::success(current_frequency_);
}

HardwareState HardwareController::get_state() const noexcept {
    return state_;
}

bool HardwareController::is_ready() const noexcept {
    return state_ == HardwareState::READY || state_ == HardwareState::STREAMING;
}

bool HardwareController::is_streaming() const noexcept {
    return streaming_active_.test();
}

ErrorCode HardwareController::set_config(const HardwareConfig& config) noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (state_ != HardwareState::READY && state_ != HardwareState::STREAMING) {
        last_error_ = ErrorCode::HARDWARE_NOT_INITIALIZED;
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    ErrorCode result = apply_config_internal(config);
    
    if (result == ErrorCode::SUCCESS) {
        config_ = config;
        last_error_ = ErrorCode::SUCCESS;
    } else {
        last_error_ = result;
    }
    
    return result;
}

HardwareConfig HardwareController::get_config() const noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    return config_;
}

ErrorCode HardwareController::set_gain(uint16_t gain) noexcept {
    ErrorCode validate_result = validate_gain_internal(gain);
    if (validate_result != ErrorCode::SUCCESS) {
        last_error_ = validate_result;
        return validate_result;
    }
    
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    if (state_ != HardwareState::READY && state_ != HardwareState::STREAMING) {
        last_error_ = ErrorCode::HARDWARE_NOT_INITIALIZED;
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }
    
    // Update gain in config
    config_.gain = gain;
    
    // Apply to hardware
    // Placeholder for gain setting
    // In actual implementation, this would write gain registers
    
    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}

uint16_t HardwareController::get_gain() const noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    return config_.gain;
}

bool HardwareController::is_pll_locked() const noexcept {
    return pll_locked_.test();
}

ErrorCode HardwareController::reset() noexcept {
    MutexLock<LockOrder::STATE_MUTEX> lock(mutex_);
    
    // Stop streaming if active
    if (streaming_active_.test()) {
        stop_streaming_internal();
    }
    
    // Reset state
    state_ = HardwareState::READY;
    pll_locked_.clear();
    streaming_active_.clear();
    retry_count_ = 0;
    
    // Reapply default configuration
    HardwareConfig default_config;
    ErrorCode result = apply_config_internal(default_config);
    if (result == ErrorCode::SUCCESS) {
        config_ = default_config;
        current_frequency_ = config_.center_frequency;
        last_error_ = ErrorCode::SUCCESS;
    } else {
        last_error_ = result;
    }
    
    return result;
}

ErrorCode HardwareController::get_last_error() const noexcept {
    return last_error_;
}

ErrorCode HardwareController::validate_frequency_internal(FreqHz frequency) const noexcept {
    if (frequency < MIN_FREQUENCY_HZ || frequency > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode HardwareController::validate_gain_internal(uint16_t gain) const noexcept {
    constexpr uint16_t MAX_GAIN = 40;
    if (gain > MAX_GAIN) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode HardwareController::apply_config_internal(const HardwareConfig& config) noexcept {
    // Apply sample rate for spectrum processing
    receiver_model.set_sampling_rate(config.sample_rate);

    // Set baseband bandwidth
    receiver_model.set_baseband_bandwidth(config.sample_rate);

    // Configure gain stages
    receiver_model.set_lna(config.lna_gain);
    receiver_model.set_vga(config.vga_gain);

    // Enable RF amplifier if gain is high
    receiver_model.set_rf_amp(config.gain > 20);

    // Set frequency
    rf::Frequency rf_freq = rf::Frequency(config.center_frequency);
    receiver_model.set_target_frequency(rf_freq);

    // Enable receiver
    receiver_model.enable();

    return ErrorCode::SUCCESS;
}

ErrorCode HardwareController::handle_error_internal(ErrorCode error) noexcept {
    // Handle error based on type
    switch (error) {
        case ErrorCode::PLL_LOCK_FAILURE:
            // Fallback: use last known frequency
            pll_locked_.clear();
            return ErrorCode::SUCCESS;
        
        case ErrorCode::HARDWARE_TIMEOUT:
            // Retry operation
            return ErrorCode::SUCCESS;
        
        case ErrorCode::HARDWARE_FAILURE:
            // Reset hardware
            return reset();
        
        default:
            return error;
    }
}

} // namespace drone_analyzer
