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

#ifndef ANALOGTV_RECEIVER_HPP
#define ANALOGTV_RECEIVER_HPP

#include "radio_state.hpp"
#include "receiver_model.hpp"
#include "analogtv_constants.hpp"

namespace ui::external_app::analogtv {

/**
 * @brief Hardware abstraction layer for Analog TV receiver
 * 
 * This class handles all hardware interactions including:
 * - Baseband initialization and control
 * - Frequency tuning
 * - Spectrum streaming control
 * - Hardware state management
 * 
 * @note All methods are non-blocking and safe to call from UI thread
 * @note No dynamic memory allocation
 */
class AnalogTvReceiver {
public:
    /**
     * @brief Construct a new Analog TV Receiver
     */
    AnalogTvReceiver() = default;

    /**
     * @brief Destructor - ensures proper cleanup
     */
    ~AnalogTvReceiver();

    // Disable copy and move
    AnalogTvReceiver(const AnalogTvReceiver&) = delete;
    AnalogTvReceiver& operator=(const AnalogTvReceiver&) = delete;
    AnalogTvReceiver(AnalogTvReceiver&&) = delete;
    AnalogTvReceiver& operator=(AnalogTvReceiver&&) = delete;

    /**
     * @brief Initialize the receiver hardware
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode initialize();

    /**
     * @brief Tune to specified frequency
     * @param freq_hz Frequency in Hz
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode tune(uint64_t freq_hz);

    /**
     * @brief Start spectrum streaming
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode start_streaming();

    /**
     * @brief Stop spectrum streaming
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode stop_streaming();

    /**
     * @brief Enable receiver model
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode enable();

    /**
     * @brief Disable receiver model
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode disable();

    /**
     * @brief Shutdown baseband
     */
    void shutdown();

    /**
     * @brief Get current tuned frequency
     * @return uint64_t Current frequency in Hz
     */
    uint64_t get_current_frequency() const { return current_frequency_; }

    /**
     * @brief Check if receiver is streaming
     * @return bool True if streaming
     */
    bool is_streaming() const { return streaming_; }

    /**
     * @brief Check if receiver is enabled
     * @return bool True if enabled
     */
    bool is_enabled() const { return enabled_; }

    /**
     * @brief Get last error code
     * @return ErrorCode Last error
     */
    ErrorCode get_last_error() const { return last_error_; }

private:
    /**
     * @brief Set modulation mode
     * @param mode Receiver model mode
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode set_modulation(ReceiverModel::Mode mode);

    /**
     * @brief Set sampling rate
     * @param rate_hz Sampling rate in Hz
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode set_sampling_rate(uint32_t rate_hz);

    /**
     * @brief Set baseband bandwidth
     * @param bandwidth_hz Bandwidth in Hz
     * @return ErrorCode SUCCESS or error code
     */
    ErrorCode set_baseband_bandwidth(uint32_t bandwidth_hz);

    /**
     * @brief Wait for PLL lock with timeout
     * @return bool True if PLL locked within timeout
     */
    bool wait_for_pll_lock();

    /**
     * @brief Handle hardware error with retry logic
     * @param error Error code
     * @param operation Description of failed operation
     */
    void handle_hardware_error(ErrorCode error, const char* operation);

    // Member variables
    RxRadioState radio_state_{ReceiverModel::Mode::WidebandFMAudio};
    uint64_t current_frequency_{0};
    bool streaming_{false};
    bool enabled_{false};
    bool initialized_{false};
    ErrorCode last_error_{ErrorCode::SUCCESS};
    uint8_t retry_count_{0};
};

}  // namespace ui::external_app::analogtv

#endif /* ANALOGTV_RECEIVER_HPP */