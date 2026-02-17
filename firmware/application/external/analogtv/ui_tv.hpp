/*
 * Copyright (C) 2015 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2020 Shao
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

#ifndef __UI_TV_H__
#define __UI_TV_H__

#include "ui.hpp"
#include "ui_widget.hpp"

#include "event_m0.hpp"

#include "message.hpp"

#include <cstdint>
#include <cstddef>
#include <array>
#include "tv_signal_detector.hpp"

namespace ui::external_app::analogtv {

class TVSignalDetector;

namespace tv {

// Named constants for TV rendering
namespace RenderConstants {
    // Audio spectrum constants
    static constexpr int AUDIO_SPECTRUM_SIZE = 128;
    static constexpr int CURSOR_BAND_HEIGHT = 4;
    static constexpr int WAVEFORM_HEIGHT = 2 * 16;
    
    // TV display constants
    static constexpr int TV_LINE_WIDTH = 128;
    static constexpr int SAMPLES_PER_PACKET = 256;
    static constexpr int X_CORRECTION_RANGE = 128;
    
    // Buffer optimization: reduced from 16 to 8 lines (50% RAM reduction)
    static constexpr int LINE_BUFFER_SIZE = 8;
    static constexpr int RENDER_THRESHOLD = 8;
    
    // Audio spectrum view dimensions
    static constexpr Dim AUDIO_SPECTRUM_HEIGHT = 16 * 2 + 20;
    static constexpr Dim SCALE_HEIGHT = 20;
    
    // Signal detection constants
    static constexpr int DETECTION_SKIP_FRAMES = 8;
}

class TimeScopeView : public View {
   public:
    TimeScopeView(const Rect parent_rect);

    void paint(Painter& painter) override;

    void on_audio_spectrum(const AudioSpectrum* spectrum);

   private:
    std::array<int16_t, RenderConstants::AUDIO_SPECTRUM_SIZE> audio_spectrum{};

    Waveform waveform{
        {0, 1 * 16 + RenderConstants::CURSOR_BAND_HEIGHT, screen_width, RenderConstants::WAVEFORM_HEIGHT},
        audio_spectrum.data(),
        RenderConstants::AUDIO_SPECTRUM_SIZE,
        0,
        false,
        Theme::getInstance()->bg_darkest->foreground,
        true};
};

class TVView : public Widget {
   public:
    TVView() : scan_line(0), x_correction_(0), line_buffer_{}, buffer_line_count(0) {
        for (auto& line : line_buffer_) {
            line.fill(Color::black());
        }
    }

    void on_show() override;
    void on_hide() override;

    void paint(Painter& painter) override;
    void on_channel_spectrum(const ChannelSpectrum& spectrum);
    void set_x_correction(int32_t value);

   private:
    int scan_line;
    int32_t x_correction_;

    // Optimized line buffer: reduced from 16 to 8 lines (50% RAM reduction)
    std::array<std::array<ui::Color, RenderConstants::TV_LINE_WIDTH>, RenderConstants::LINE_BUFFER_SIZE> line_buffer_;
    int buffer_line_count;

    void add_line_to_buffer(const ChannelSpectrum& spectrum, int offset_idx);
    void render_buffer_batch();
    void process_buffer_overflow();

   private:
    void clear();
};

class TVWidget : public View {
   public:
    std::function<void(int32_t offset)> on_select{};
    std::function<void(const TVSignalDetector::DetectionResult&)> on_tv_signal_detected{};

    TVWidget();

    TVWidget(const TVWidget&) = delete;
    TVWidget(TVWidget&&) = delete;
    TVWidget& operator=(const TVWidget&) = delete;
    TVWidget& operator=(TVWidget&&) = delete;

    void on_show() override;
    void on_hide() override;

    void set_parent_rect(const Rect new_parent_rect) override;

    void show_audio_spectrum_view(const bool show);

    void paint(Painter& painter) override;
    NumberField field_xcorr{
        {UI_POS_X(0), UI_POS_Y(0)},
        5,
        {0, RenderConstants::X_CORRECTION_RANGE},
        1,
        ' '};

   private:
    void update_widgets_rect();

    const Rect audio_spectrum_view_rect{UI_POS_X(0), UI_POS_Y(0), screen_width, 2 * 16 + 20};

    TVView tv_view{};
    TVSignalDetector signal_detector{};

    int frame_counter{0};
    TVSignalDetector::DetectionResult cached_detection{};
    bool has_cached_detection{false};

    ChannelSpectrumFIFO* channel_fifo{nullptr};
    AudioSpectrum* audio_spectrum_data{nullptr};
    bool audio_spectrum_update{false};

    std::unique_ptr<TimeScopeView> audio_spectrum_view{};

    int sampling_rate{0};
    int32_t cursor_position{0};
    ui::Rect tv_normal_rect{};
    ui::Rect tv_reduced_rect{};

    MessageHandlerRegistration message_handler_channel_spectrum_config{
        Message::ID::ChannelSpectrumConfig,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const ChannelSpectrumConfigMessage*>(p);
            this->channel_fifo = message.fifo;
        }};
    MessageHandlerRegistration message_handler_audio_spectrum{
        Message::ID::AudioSpectrum,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const AudioSpectrumMessage*>(p);
            this->audio_spectrum_data = message.data;
            this->audio_spectrum_update = true;
        }};
    MessageHandlerRegistration message_handler_frame_sync{
        Message::ID::DisplayFrameSync,
        [this](const Message* const) {
            if (this->channel_fifo) {
                ChannelSpectrum channel_spectrum;
                while (channel_fifo->out(channel_spectrum)) {
                    this->on_channel_spectrum(channel_spectrum);
                }
            }
            if (this->audio_spectrum_update) {
                this->audio_spectrum_update = false;
                this->on_audio_spectrum();
            }
        }};

    void on_channel_spectrum(const ChannelSpectrum& spectrum);
    void on_audio_spectrum();
};

} /* namespace tv */
}  // namespace ui::external_app::analogtv

#endif /*__UI_TV_H__*/
