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

#ifndef ANALOG_VIDEO_VIEW_HPP
#define ANALOG_VIDEO_VIEW_HPP

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_tv.hpp"
#include "drone_types.hpp"

namespace drone_analyzer {

class AnalogVideoView : public ui::View {
public:
    static constexpr uint16_t HEADER_H = 16;
    static constexpr uint16_t SCALE_H = 20;
    static constexpr uint16_t VIDEO_H = 284;

    explicit AnalogVideoView(NavigationView& nav, FreqHz frequency) noexcept;
    ~AnalogVideoView() noexcept override;

    AnalogVideoView(const AnalogVideoView&) = delete;
    AnalogVideoView& operator=(const AnalogVideoView&) = delete;

    void on_show() override;
    void on_hide() override;
    void paint(Painter& painter) override;
    void focus() override;

    std::string title() const override { return "FPV Video"; }

private:
    NavigationView& nav_;
    FreqHz frequency_{0};

    ui::external_app::analogtv::tv::TVWidget tv_widget_{};

    void setup_video_receiver() noexcept;
    void restore_receiver() noexcept;
};

}  // namespace drone_analyzer

#endif  // ANALOG_VIDEO_VIEW_HPP