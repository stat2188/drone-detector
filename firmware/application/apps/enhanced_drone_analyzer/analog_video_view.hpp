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

#include <string>
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_tv.hpp"
#include "drone_types.hpp"
#include "constants.hpp"  // For MIN_FREQUENCY_HZ, MAX_FREQUENCY_HZ

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

    std::string title() const override {
        static const std::string title_str = "FPV Video";
        return title_str;
    }

    /**
     * @brief Check if video receiver is properly configured
     * @return true if frequency valid and hardware ready
     */
    [[nodiscard]] bool is_valid() const noexcept;

private:
    NavigationView& nav_;
    FreqHz frequency_{0};

    ui::tv::TVWidget tv_widget_{};

    // Guard against double-initialization
    bool receiver_active_{false};

    void setup_video_receiver() noexcept;
    void restore_receiver() noexcept;
    
    /**
     * @brief Validate frequency against hardware limits
     * @return true if frequency is within HackRF operational range
     */
    [[nodiscard]] static bool is_frequency_valid(FreqHz freq) noexcept;
};

}  // namespace drone_analyzer

#endif  // ANALOG_VIDEO_VIEW_HPP