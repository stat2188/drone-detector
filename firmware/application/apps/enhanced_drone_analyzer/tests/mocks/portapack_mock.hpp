#ifndef PORTAPACK_MOCK_HPP_
#define PORTAPACK_MOCK_HPP_

#include <cstdint>
#include <array>
#include <string>

using Frequency = uint64_t;

struct Rect {
    int16_t x, y;
    uint16_t w, h;
};

struct Color {
    uint8_t r, g, b;

    static Color red() { return {255, 0, 0}; }
    static Color green() { return {0, 255, 0}; }
    static Color blue() { return {0, 0, 255}; }
    static Color white() { return {255, 255, 255}; }
    static Color black() { return {0, 0, 0}; }
    static Color yellow() { return {255, 255, 0}; }
    static Color orange() { return {255, 165, 0}; }
    static Color dark_grey() { return {64, 64, 64}; }
};

struct Style {
    void* font;
    Color fg;
    Color bg;
};

constexpr int screen_width = 240;
constexpr int screen_height = 320;

#endif // PORTAPACK_MOCK_HPP_
