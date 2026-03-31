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
#include "video_renderer.hpp"
#include "receiver_model.hpp"
#include "baseband_api.hpp"

namespace ui::external_app::analogtv {

/**
 * @brief Fullscreen Analog TV view with LUT-optimized rendering
 *
 * Provides true 240x320 fullscreen video display with automatic
 * video carrier scanning. Video rendering uses pre-computed LUTs
 * to eliminate all per-pixel arithmetic (zero division, zero multiply).
 *
 * @note No dynamic memory allocation.
 * @note Max stack usage per render: ~488 bytes.
 * @note Flash overhead: ~1.3 KB for LUTs.
 */
class FullscreenTvView : public View {
public:
    FullscreenTvView(NavigationView& nav);
    ~FullscreenTvView() override;

    FullscreenTvView(const FullscreenTvView&) = delete;
    FullscreenTvView& operator=(const FullscreenTvView&) = delete;
    FullscreenTvView(FullscreenTvView&&) = delete;
    FullscreenTvView& operator=(FullscreenTvView&&) = delete;

    std::string title() const override { return "Analog TV"; }
    void paint(Painter& painter) override;
    void focus() override;
    void on_show() override;
    void on_hide() override;

private:
    void initialize();
    void start_auto_scan();
    void stop_auto_scan();
    void scan_next_frequency();
    void on_carrier_found(uint64_t freq_hz);
    void update_scan_status();
    void on_channel_spectrum(const ChannelSpectrum& spectrum);
    void draw_frequency_info(Painter& painter);
    bool on_key(const KeyEvent event) override;
    bool on_encoder(const EncoderEvent delta) override;
    void toggle_scan_mode();
    void frequency_up();
    void frequency_down();

    NavigationView& nav_;

    // LUT-optimized video renderer (owns 13KB buffer)
    VideoRenderer renderer_;

    // Scan state
    ScanState scan_state_{ScanState::IDLE};
    ScanMode scan_mode_{ScanMode::AUTO_SCAN};
    uint64_t current_scan_frequency_{SCAN_START_HZ};
    uint32_t dwell_timer_{0};
    bool carrier_detected_{false};

    // UI state
    uint64_t displayed_frequency_{0};
    bool video_active_{false};

    // UI Buttons (overlay on bottom of fullscreen video)
    Button button_scan{{0, 292, 80, 26}, "Scan"};
    Button button_mode{{85, 292, 70, 26}, "Auto"};
    Button button_up{{160, 292, 35, 26}, "+"};
    Button button_down{{200, 292, 35, 26}, "-"};

    // Spectrum FIFO
    ChannelSpectrumFIFO* spectrum_fifo_{nullptr};

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
            portapack::receiver_model.set_target_frequency(message->freq);
        }};
};

}  // namespace ui::external_app::analogtv

#endif /* FULLSCREEN_TV_VIEW_HPP */
