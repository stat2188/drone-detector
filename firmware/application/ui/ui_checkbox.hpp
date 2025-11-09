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

#ifndef __UI_CHECKBOX_H__
#define __UI_CHECKBOX_H__

#include "ui_widget.hpp"

namespace ui {

class Checkbox : public Widget {
   public:
    std::function<void(Checkbox&, bool)> on_select{};

    Checkbox(Point parent_pos, size_t length, std::string text, bool small);
    Checkbox(
        Point parent_pos,
        size_t length,
        std::string text)
        : Checkbox{parent_pos, length, text, false} {
    }

    Checkbox()
        : Checkbox{{}, {}, {}} {
    }

    Checkbox(const Checkbox&) = delete;
    Checkbox(Checkbox&&) = delete;
    Checkbox& operator=(const Checkbox&) = delete;
    Checkbox& operator=(const Checkbox&&) = delete;

    void set_text(const std::string value);
    bool set_value(const bool value);
    bool value() const;

    void paint(Painter& painter) override;

    bool on_key(const KeyEvent key) override;
    bool on_keyboard(const KeyboardEvent key) override;
    bool on_touch(const TouchEvent event) override;
    void getAccessibilityText(std::string& result) override;
    void getWidgetName(std::string& result) override;

   private:
    std::string text_;
    bool small_{false};
    bool value_{false};
    const Style* style_{nullptr};
};

} /* namespace ui */

#endif /*__UI_CHECKBOX_H__*/
