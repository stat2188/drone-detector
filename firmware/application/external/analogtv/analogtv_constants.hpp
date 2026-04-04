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

#ifndef ANALOGTV_CONSTANTS_HPP
#define ANALOGTV_CONSTANTS_HPP

#include <cstdint>
#include <cstddef>

namespace ui::external_app::analogtv {

/**
 * @brief Video display width (source pixels per line)
 */
constexpr uint16_t VIDEO_WIDTH = 128;

/**
 * @brief Source lines per frame (PAL half-frame, doubled for display)
 */
constexpr uint16_t SOURCE_LINES = 104;

/**
 * @brief Display height after line doubling (104 * 2 = 208)
 */
constexpr uint16_t VIDEO_HEIGHT = 208;

/**
 * @brief Spectrum bins per callback from M0
 */
constexpr size_t SPECTRUM_BINS = 256;

/**
 * @brief Callbacks needed per frame (256 bins * 52 = 13312 = 128 * 104)
 */
constexpr uint32_t CALLBACKS_PER_FRAME = 52;

/**
 * @brief Default horizontal alignment offset
 */
constexpr uint8_t DEFAULT_X_CORRECTION = 10;

/**
 * @brief Maximum horizontal correction value
 */
constexpr uint8_t MAX_X_CORRECTION = 128;

}  // namespace ui::external_app::analogtv

#endif /* ANALOGTV_CONSTANTS_HPP */
