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

#ifndef FULLSCREEN_TV_VIEW_HPP
#define FULLSCREEN_TV_VIEW_HPP

#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_receiver.hpp"
#include "ui_navigation.hpp"
#include "analogtv_constants.hpp"
#include "analogtv_receiver.hpp"
#include "video_processor.hpp"

#include <string>

namespace ui::external_app::analogtv {

/**
 * @brief Fullscreen Analog TV view with auto-scan capability
 * 
 * This class provides:
 * - Fullscreen video display (240x320 pixels)
 * - Automatic video carrier search
 * - Manual frequency tuning
 * - Minimal UI controls (hidden by default)
 * 
 * @note Follows Diamond Code standard - clean, flat hierarchy
 * @note No dynamic memory allocation
 * @note Minimal stack usage
 */
class FullscreenTvView : public View {
public:
    /**
     * @brief Construct a new Fullscreen TV View
     * @param nav Navigation view reference
     */
    FullscreenTvView(NavigationView& nav);

    /**
     * @brief Destructor - ensures proper cleanup
     */
    ~FullscreenTvView() override;

    // Disable copy and move
    FullscreenTvView(const FullscreenTvView&) = delete;
    FullscreenTvView& operator=(const FullscreenTvView&) = delete;
    FullscreenTvView(FullscreenTvView&&) = delete;
    FullscreenTvView& operator=(FullscreenTvView&&) = delete;

    /**
     * @brief Get view title
     * @return std::string View title
     */
    std::string title() const override { return "Analog TV"; }

    /**
     * @brief Paint the view
     * @param painter Painter instance
     */
    void paint(Painter& painter) override;

    /**
     * @brief Handle focus gained
     */
    void focus() override;

    /**
     * @brief Handle view show
     */
    void on_show() override;

    /**
     * @brief Handle view hide
     */
    void on_hide() override;

    /**
     * @brief Set parent rectangle
     * @param new_parent_rect New parent rectangle
     */
    void set_parent_rect(const Rect new_parent_rect) override;

private:
    /**
     * @brief Initialize the view
     */
    void initialize();

    /**
     * @brief Start automatic frequency scan
     */
    void start_auto_scan();

    /**
     * @brief Stop automatic frequency scan
     */
    void stop_auto_scan();

    /**
     * @brief Scan next frequency in sequence
     */
    void scan_next_frequency();

    /**
     * @brief Handle video carrier found
     * @param freq_hz Frequency where carrier was found
     */
    void on_carrier_found(uint64_t freq_hz);

    /**
     * @brief Update scan status display
     */
    void update_scan_status();

    /**
     * @brief Handle channel spectrum data
     * @param spectrum Channel spectrum data
     */
    void on_channel_spectrum(const ChannelSpectrum& spectrum);

    /**
     * @brief Render video frame to display
     */
    void render_video_frame();

    /**
     * @brief Draw scan status overlay
     * @param painter Painter instance
     */
    void draw_scan_status(Painter& painter);

    /**
     * @brief Draw frequency information
     * @param painter Painter instance
     */
    void draw_frequency_info(Painter& painter);

    /**
     * @brief Handle key events
     * @param event Key event
     * @return true if event was handled
     */
    bool on_key(const KeyEvent event);

    /**
     * @brief Handle encoder events
     * @param delta Encoder delta
     * @return true if event was handled
     */
    bool on_encoder(const EncoderEvent delta);

    /**
     * @brief Toggle scan mode (auto/manual)
     */
    void toggle_scan_mode();

    /**
     * @brief Increase frequency by step
     */
    void frequency_up();

    /**
     * @brief Decrease frequency by step
     */
    void frequency_down();

    // Navigation reference
    NavigationView& nav_;

    // Hardware abstraction
    AnalogTvReceiver receiver_;

    // Video processing
    VideoProcessor video_processor_;

    // Scan state
    ScanState scan_state_{ScanState::IDLE};
    ScanMode scan_mode_{ScanMode::AUTO_SCAN};
    uint64_t current_scan_frequency_{SCAN_START_HZ};
    uint32_t scan_timer_{0};
    uint32_t dwell_timer_{0};
    bool carrier_detected_{false};

    // UI state
    uint64_t displayed_frequency_{0};
    bool video_active_{false};

    // UI Buttons
    Button button_scan{
        {0, 290, 80, 28},
        "Scan"};
    
    Button button_mode{
        {85, 290, 70, 28},
        "Auto"};
    
    Button button_up{
        {160, 290, 35, 28},
        "+"};
    
    Button button_down{
        {200, 290, 35, 28},
        "-"};

    // Spectrum data
    ChannelSpectrumFIFO* spectrum_fifo_{nullptr};
    bool spectrum_update_{false};

    // Message handlers
    MessageHandlerRegistration message_handler_spectrum_config{
        Message::ID::ChannelSpectrumConfig,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        }};

    MessageHandlerRegistration message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](const Message* const) {
            if (this->spectrum_fifo_) {
                ChannelSpectrum channel_spectrum;
                while (spectrum_fifo_->out(channel_spectrum)) {
                    this->on_channel_spectrum(channel_spectrum);
                }
            }
        }};

    MessageHandlerRegistration message_handler_freqchg{
        Message::ID::FreqChangeCommand,
        [this](Message* const p) {
            const auto message = static_cast<const FreqChangeCommandMessage*>(p);
            this->receiver_.tune(message->freq);
        }};
};

}  // namespace ui::external_app::analogtv

#endif /* FULLSCREEN_TV_VIEW_HPP */