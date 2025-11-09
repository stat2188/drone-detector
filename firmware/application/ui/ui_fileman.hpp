/*
 * Copyright (C) 2015 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2017 Furrtek
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

#ifndef __UI_FILEMAN_H__
#define __UI_FILEMAN_H__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"

namespace ui {

class FileBrowserView : public View {
   public:
    FileBrowserView(NavigationView& nav, const std::string& path, const std::string& file);

    std::function<void(std::string)> on_save{};

   private:
    void refresh_widgets();

    NavigationView& nav_;
    std::string path_;
    std::string file_;
    std::string buffer_{};

    Labels labels{
        {{0 * 8, 1 * 16}, "Path:", Theme::getInstance()->fg_light->foreground},
        {{0 * 8, 6 * 16}, "Filename:", Theme::getInstance()->fg_light->foreground},
    };

    Text text_path{
        {0 * 8, 2 * 16, screen_width, 16},
        "",
    };

    Button button_edit_path{
        {18 * 8, 3 * 16, 11 * 8, 32},
        "Edit Path"
    };

    Text text_name{
        {0 * 8, 7 * 16, screen_width, 16},
        "",
    };

    Button button_edit_name{
        {18 * 8, 8 * 16, 11 * 8, 32},
        "Edit Name"
    };

    Button button_save{
        {10 * 8, 16 * 16, 9 * 8, 32},
        "Save"
    };

    Button button_cancel{
        {20 * 8, 16 * 16, 9 * 8, 32},
        "Cancel"
    };
};

} /* namespace ui */

#endif /*__UI_FILEMAN_H__*/
