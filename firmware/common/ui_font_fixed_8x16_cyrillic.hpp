/*
 * Copyright (C) 2024 Enhanced Drone Analyzer
 *
 * Cyrillic 8x16 fixed-width font for PortaPack Mayhem
 * Supports Russian alphabet (А-Я а-я Ё ё)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#ifndef __UI_FONT_FIXED_8X16_CYRILLIC_H__
#define __UI_FONT_FIXED_8X16_CYRILLIC_H__

#include "ui_text.hpp"

namespace ui {
namespace font {

/**
 * @brief Cyrillic 8x16 fixed-width font
 * @note Covers Russian alphabet: А-Я (0x80-0x9F), а-я (0xA0-0xBF), Ёё (0xC0-0xC1)
 * @note Total 66 characters, 16 bytes each = 1056 bytes
 */
extern const ui::Font fixed_8x16_cyrillic;

} /* namespace font */
} /* namespace ui */

#endif /*__UI_FONT_FIXED_8X16_CYRILLIC_H__*/