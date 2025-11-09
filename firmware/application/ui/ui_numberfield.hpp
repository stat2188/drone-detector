/*
 * Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2016 Furrtek
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

#ifndef __UI_NUMBERFIELD_H__
#define __UI_NUMBERFIELD_H__

#include "ui_widget.hpp"

namespace ui {

class NumberField : public Widget {
   public:
    std::function<void(NumberField&)> on_select{};
    std::function<void(int32_t)> on_change{};
    std::function<void(int32_t)> on_wrap{};

    using range_t = std::pair<int32_t, int32_t>;

    NumberField(Point parent_pos, int length, range_t range, int32_t step, char fill_char, bool can_loop);

    NumberField(Point parent_pos, int length, range_t range, int32_t step, char fill_char)
        : NumberField{parent_pos, length, range, step, fill_char, false} {
    }

    NumberField()
        : NumberField{{0, 0}, 1, {0, 1}, 1, ' ', false} {
    }

    NumberField(const NumberField&) = delete;
    NumberField(NumberField&&) = delete;

    int32_t value() const;
    void set_value(int32_t new_value, bool trigger_change = true);
    void set_range(const int32_t min, const int32_t max);
    void set_step(const int32_t new_step);

    void paint(Painter& painter) override;

    bool on_key(const KeyEvent key) override;
    bool on_encoder(const EncoderEvent delta) override;
    bool on_touch(const TouchEvent event) override;
    bool on_keyboard(const KeyboardEvent event) override;

    void getAccessibilityText(std::string& result) override;
    void getWidgetName(std::string& result) override;

   private:
    range_t range;
    int32_t step;
    const int length_;
    const char fill_char;
    int32_t value_{0};
    bool can_loop{};
};

} /* namespace ui */

#endif /*__UI_NUMBERFIELD_H__*/
