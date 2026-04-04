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

#include "fullscreen_tv_view.hpp"
#include "portapack.hpp"

#include <cstdio>

using namespace portapack;

namespace ui::external_app::analogtv {

FullscreenTvView::FullscreenTvView(NavigationView& nav)
    : nav_(nav), renderer_{} {
    set_parent_rect({0, 0, DISPLAY_W, DISPLAY_H});
    add_children({&button_up, &button_down, &button_xcorr});

    button_up.on_select = [this](Button&) {
        frequency_up();
    };

    button_down.on_select = [this](Button&) {
        frequency_down();
    };

    button_xcorr.on_select = [this](Button&) {
        uint8_t cur = renderer_.get_x_correction();
        renderer_.set_x_correction((cur + 10) > 128 ? 0 : cur + 10);
    };
}

FullscreenTvView::~FullscreenTvView() {
    baseband::shutdown();
    receiver_model.disable();
}

void FullscreenTvView::paint(Painter& painter) {
    draw_frequency_info(painter);
}

void FullscreenTvView::focus() {
}

void FullscreenTvView::on_show() {
    initialize();
    baseband::spectrum_streaming_start();
    receiver_model.enable();
}

void FullscreenTvView::on_hide() {
    baseband::spectrum_streaming_stop();
    receiver_model.disable();
}

void FullscreenTvView::initialize() {
    baseband::run_prepared_image(portapack::memory::map::m4_code.base());

    receiver_model.set_modulation(ReceiverModel::Mode::WidebandFMAudio);
    receiver_model.set_sampling_rate(SAMPLE_RATE_HZ);
    receiver_model.set_baseband_bandwidth(BASEBAND_BW_HZ);

    renderer_.clear_buffer();
    current_frequency_ = FREQ_MIN_HZ;
    video_active_ = false;
}

void FullscreenTvView::on_channel_spectrum(const ChannelSpectrum& spectrum) {
    if (!spectrum_fifo_) {
        return;
    }

    video_active_ = renderer_.process_spectrum(spectrum);
    set_dirty();
}

void FullscreenTvView::draw_frequency_info(Painter& painter) {
    char freq_buf[24];
    const auto freq_mhz = current_frequency_ / 1000000;
    const auto freq_frac = (current_frequency_ % 1000000) / 10000;

    std::snprintf(freq_buf, sizeof(freq_buf), "%lu.%02lu MHz",
                  static_cast<unsigned long>(freq_mhz),
                  static_cast<unsigned long>(freq_frac));

    painter.fill_rectangle({0, 0, DISPLAY_W, INFO_BAR_H}, Color::black());
    painter.draw_string({4, 4}, *Theme::getInstance()->fg_yellow, freq_buf);

    const char* state_str = video_active_ ? "VIDEO" : "TUNE";
    painter.draw_string({DISPLAY_W - 60, 4},
                        video_active_ ? *Theme::getInstance()->fg_green : *Theme::getInstance()->fg_yellow,
                        state_str);
}

bool FullscreenTvView::on_key(const KeyEvent event) {
    switch (event) {
        case KeyEvent::Left:
            frequency_down();
            return true;
        case KeyEvent::Right:
            frequency_up();
            return true;
        case KeyEvent::Select:
            renderer_.set_x_correction(10);
            return true;
        default:
            return false;
    }
}

bool FullscreenTvView::on_encoder(const EncoderEvent delta) {
    if (delta > 0) {
        frequency_up();
    } else {
        frequency_down();
    }
    return true;
}

void FullscreenTvView::frequency_up() {
    current_frequency_ += FREQ_STEP_HZ;
    if (current_frequency_ > FREQ_MAX_HZ) {
        current_frequency_ = FREQ_MIN_HZ;
    }

    receiver_model.set_target_frequency(current_frequency_);
    renderer_.clear_buffer();
    video_active_ = false;
    set_dirty();
}

void FullscreenTvView::frequency_down() {
    if (current_frequency_ < FREQ_MIN_HZ + FREQ_STEP_HZ) {
        current_frequency_ = FREQ_MAX_HZ;
    } else {
        current_frequency_ -= FREQ_STEP_HZ;
    }

    receiver_model.set_target_frequency(current_frequency_);
    renderer_.clear_buffer();
    video_active_ = false;
    set_dirty();
}

}  // namespace ui::external_app::analogtv
