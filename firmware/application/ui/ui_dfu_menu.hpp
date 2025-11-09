/*
 * Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
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

#ifndef __UI_DFU_MENU_H__
#define __UI_DFU_MENU_H__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"

namespace ui {

class DFUMenuView : public View {
   public:
    DFUMenuView(NavigationView& nav);
    ~DFUMenuView() = default;

    void focus() override;
    std::string title() const override { return "DFU Menu"; };

   private:
    NavigationView& nav_;

    Button button_enter_dfu{
        {screen_width / 2 - 60, screen_height / 2 - 16, 120, 32},
        "Enter DFU Mode"
    };

    Text text_warning{
        {8, screen_height / 2 - 48, screen_width - 16, 32},
        "Warning: Entering DFU mode will\nrestart the device for firmware\nupdate. Ensure update file is ready."
    };
};

} /* namespace ui */

#endif /*__UI_DFU_MENU_H__*/
