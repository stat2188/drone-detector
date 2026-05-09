/*
 * Copyright (C) 2025 PortaPack Mayhem Edition Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "analog_video_view.hpp"

#include "baseband_api.hpp"
#include "portapack.hpp"
#include "audio.hpp"

namespace drone_analyzer {

AnalogVideoView::AnalogVideoView(NavigationView& nav, FreqHz frequency) noexcept
    : nav_(nav)
    , frequency_(frequency) {

    tv_widget_.show_audio_spectrum_view(false);

    add_child(&tv_widget_);

    tv_widget_.set_parent_rect({0, HEADER_H, 240, 320 - HEADER_H});
}

AnalogVideoView::~AnalogVideoView() noexcept {
    restore_receiver();
}

void AnalogVideoView::on_show() {
    setup_video_receiver();
    tv_widget_.on_show();
}

void AnalogVideoView::on_hide() {
    tv_widget_.on_hide();
    restore_receiver();
}

void AnalogVideoView::paint(Painter& painter) {
    painter.fill_rectangle({0, 0, 240, HEADER_H}, Color::black());

    char freq_str[24];
    const uint32_t mhz = static_cast<uint32_t>(frequency_ / 1'000'000ULL);
    const uint32_t khz = static_cast<uint32_t>((frequency_ % 1'000'000ULL) / 1'000ULL);
    snprintf(freq_str, sizeof(freq_str), "%lu.%03lu MHz",
             static_cast<unsigned long>(mhz),
             static_cast<unsigned long>(khz));

    painter.draw_string(
        Point{4, 4},
        Theme::getInstance()->fg_light->font,
        Theme::getInstance()->fg_light->foreground,
        Color::black(),
        freq_str);

    View::paint(painter);
}

void AnalogVideoView::focus() {
}

void AnalogVideoView::setup_video_receiver() noexcept {
    audio::output::mute();
    baseband::shutdown();

    baseband::run_prepared_image(portapack::memory::map::m4_code.base());

    portapack::receiver_model.set_modulation(ReceiverModel::Mode::WidebandFMAudio);
    portapack::receiver_model.set_sampling_rate(2000000);
    portapack::receiver_model.set_baseband_bandwidth(2000000);
    portapack::receiver_model.set_target_frequency(frequency_);
    portapack::receiver_model.enable();
}

void AnalogVideoView::restore_receiver() noexcept {
    audio::output::mute();
    portapack::receiver_model.disable();
    baseband::shutdown();
}

}  // namespace drone_analyzer