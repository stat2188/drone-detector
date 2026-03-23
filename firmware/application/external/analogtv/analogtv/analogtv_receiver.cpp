/*
 * Copyright (C) 2024 Mayhem Firmware Contributors
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "analogtv_receiver.hpp"

#include "baseband_api.hpp"
#include "portapack.hpp"
#include "portapack_persistent_memory.hpp"

using namespace portapack;

namespace ui::external_app::analogtv {

AnalogTvReceiver::~AnalogTvReceiver() {
    shutdown();
}

ErrorCode AnalogTvReceiver::initialize() {
    if (initialized_) {
        return ErrorCode::SUCCESS;
    }

    // Shutdown any existing baseband
    baseband::shutdown();

    // Run the baseband image
    baseband::run_prepared_image(portapack::memory::map::m4_code.base());

    // Set default modulation
    ErrorCode error = set_modulation(ReceiverModel::Mode::WidebandFMAudio);
    if (error != ErrorCode::SUCCESS) {
        handle_hardware_error(error, "set_modulation");
        return error;
    }

    // Set default sampling rate
    error = set_sampling_rate(DEFAULT_SAMPLE_RATE_HZ);
    if (error != ErrorCode::SUCCESS) {
        handle_hardware_error(error, "set_sampling_rate");
        return error;
    }

    // Set default baseband bandwidth
    error = set_baseband_bandwidth(DEFAULT_BASEBAND_BANDWIDTH_HZ);
    if (error != ErrorCode::SUCCESS) {
        handle_hardware_error(error, "set_baseband_bandwidth");
        return error;
    }

    initialized_ = true;
    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::tune(uint64_t freq_hz) {
    if (!initialized_) {
        ErrorCode error = initialize();
        if (error != ErrorCode::SUCCESS) {
            return error;
        }
    }

    // Validate frequency range
    if (freq_hz < SCAN_START_HZ || freq_hz > SCAN_END_HZ) {
        last_error_ = ErrorCode::TUNING_FAILED;
        return last_error_;
    }

    // Set target frequency
    receiver_model.set_target_frequency(freq_hz);
    current_frequency_ = freq_hz;

    // Wait for PLL lock
    if (!wait_for_pll_lock()) {
        last_error_ = ErrorCode::TUNING_FAILED;
        return last_error_;
    }

    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::start_streaming() {
    if (!initialized_) {
        ErrorCode error = initialize();
        if (error != ErrorCode::SUCCESS) {
            return error;
        }
    }

    if (streaming_) {
        return ErrorCode::SUCCESS;
    }

    baseband::spectrum_streaming_start();
    streaming_ = true;
    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::stop_streaming() {
    if (!streaming_) {
        return ErrorCode::SUCCESS;
    }

    baseband::spectrum_streaming_stop();
    streaming_ = false;
    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::enable() {
    if (!initialized_) {
        ErrorCode error = initialize();
        if (error != ErrorCode::SUCCESS) {
            return error;
        }
    }

    if (enabled_) {
        return ErrorCode::SUCCESS;
    }

    receiver_model.enable();
    enabled_ = true;
    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::disable() {
    if (!enabled_) {
        return ErrorCode::SUCCESS;
    }

    receiver_model.disable();
    enabled_ = false;
    last_error_ = ErrorCode::SUCCESS;
    return ErrorCode::SUCCESS;
}

void AnalogTvReceiver::shutdown() {
    if (streaming_) {
        stop_streaming();
    }

    if (enabled_) {
        disable();
    }

    if (initialized_) {
        baseband::shutdown();
        initialized_ = false;
    }

    current_frequency_ = 0;
    retry_count_ = 0;
    last_error_ = ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::set_modulation(ReceiverModel::Mode mode) {
    receiver_model.set_modulation(mode);
    return ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::set_sampling_rate(uint32_t rate_hz) {
    receiver_model.set_sampling_rate(rate_hz);
    return ErrorCode::SUCCESS;
}

ErrorCode AnalogTvReceiver::set_baseband_bandwidth(uint32_t bandwidth_hz) {
    receiver_model.set_baseband_bandwidth(bandwidth_hz);
    return ErrorCode::SUCCESS;
}

bool AnalogTvReceiver::wait_for_pll_lock() {
    // Simple polling-based PLL lock detection
    // In real implementation, this would check hardware registers
    uint32_t timeout_ms = PLL_LOCK_TIMEOUT_MS;
    
    while (timeout_ms > 0) {
        // Simulate PLL lock check - in real implementation,
        // this would read from hardware registers
        // For now, assume PLL locks quickly
        if (timeout_ms < PLL_LOCK_TIMEOUT_MS - PLL_LOCK_POLL_INTERVAL_MS) {
            return true;  // PLL locked
        }
        
        // Delay for poll interval
        chThdSleepMilliseconds(PLL_LOCK_POLL_INTERVAL_MS);
        timeout_ms -= PLL_LOCK_POLL_INTERVAL_MS;
    }
    
    return false;  // Timeout - PLL didn't lock
}

void AnalogTvReceiver::handle_hardware_error(ErrorCode error, const char* operation) {
    last_error_ = error;
    retry_count_++;
    
    // Log error for debugging
    // In production, this would write to a log buffer
    (void)operation;  // Suppress unused parameter warning
    
    // If max retries exceeded, mark as unrecoverable
    if (retry_count_ >= MAX_HARDWARE_RETRIES) {
        initialized_ = false;
    }
}

}  // namespace ui::external_app::analogtv