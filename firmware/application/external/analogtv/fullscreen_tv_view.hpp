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
#include "video_renderer.hpp"
#include "receiver_model.hpp"
#include "baseband_api.hpp"

#include <string>

namespace ui::external_app::analogtv {

/**
 * @brief Fullscreen Analog TV view
 *
 * Receives spectrum data from M0 via FIFO, renders analog TV image.
 * Manual frequency tuning only — no auto-scanning.
 *
 * @note M0 only streams raw spectrum — zero DSP on M0
 * @note Video buffer: ~13.5KB in .bss (M4 RAM)
 * @note Stack per frame: 256 bytes (line buffer)
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
    void on_channel_spectrum(const ChannelSpectrum& spectrum);
    void draw_frequency_info(Painter& painter);
    bool on_key(const KeyEvent event) override;
    bool on_encoder(const EncoderEvent delta) override;
    void frequency_up();
    void frequency_down();

    static constexpr ui::Coord DISPLAY_W = 240;
    static constexpr ui::Coord DISPLAY_H = 320;
    static constexpr ui::Coord INFO_BAR_H = 20;
    static constexpr ui::Coord BUTTON_Y = 292;
    static constexpr uint32_t FREQ_STEP_HZ = 1000000;
    static constexpr uint64_t FREQ_MIN_HZ = 50000000ULL;
    static constexpr uint64_t FREQ_MAX_HZ = 7200000000ULL;
    static constexpr uint32_t SAMPLE_RATE_HZ = 2000000;
    static constexpr uint32_t BASEBAND_BW_HZ = 2000000;

    NavigationView& nav_;
    VideoRenderer renderer_;
    uint64_t current_frequency_{FREQ_MIN_HZ};
    bool video_active_{false};

    // UI Buttons
    Button button_up{{0, BUTTON_Y, 80, 26}, "+"};
    Button button_down{{85, BUTTON_Y, 80, 26}, "-"};
    Button button_xcorr{{170, BUTTON_Y, 65, 26}, "XCorr"};

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
