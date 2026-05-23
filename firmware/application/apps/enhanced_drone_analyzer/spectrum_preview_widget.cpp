#include <cstdint>
#include <algorithm>

#include "spectrum_preview_widget.hpp"

namespace drone_analyzer {

namespace {

constexpr int PEAK1_CENTER = 72;
constexpr int PEAK2_CENTER = 172;
constexpr int NOISE_FLOOR_OFFSET = 4;

int32_t peak_height_at(
    int32_t x,
    int32_t center,
    int32_t peak_width,
    int32_t max_peak_h) noexcept {
    int32_t dx = x - center;
    int32_t pw2 = peak_width * peak_width;
    return (max_peak_h * pw2) / (pw2 + dx * dx);
}

} // namespace

SpectrumPreviewWidget::SpectrumPreviewWidget(ui::Rect parent_rect) noexcept
    : Widget{parent_rect} {
}

void SpectrumPreviewWidget::set_params(
    uint8_t margin,
    uint8_t min_width,
    uint8_t max_width,
    uint8_t sharpness,
    uint8_t peak_ratio) noexcept {
    margin_ = margin;
    min_width_ = min_width;
    max_width_ = max_width;
    sharpness_ = sharpness;
    peak_ratio_ = peak_ratio;
    set_dirty();
}

ui::Color SpectrumPreviewWidget::amplitude_color(int32_t h, int32_t max_h) noexcept {
    if (max_h <= 0) return ui::Color::green();
    uint32_t scaled = static_cast<uint32_t>(h) * 768u / static_cast<uint32_t>(max_h);
    if (scaled < 256u) {
        return ui::Color(0, scaled & 0xFFu, 0);
    } else if (scaled < 512u) {
        return ui::Color(scaled - 256u, 255u, 0);
    } else {
        uint32_t g = std::min<uint32_t>(255u, 255u - (scaled - 512u));
        return ui::Color(255u, g & 0xFFu, 0);
    }
}

void SpectrumPreviewWidget::paint(ui::Painter& painter) {
    const auto r = screen_rect();
    const int x0 = r.location().x();
    const int y0 = r.location().y();
    const int w = r.size().width();
    const int h = r.size().height();

    painter.fill_rectangle(r, ui::Color::black());

    const int floor_local = h - NOISE_FLOOR_OFFSET;
    const int floor_abs_y = y0 + floor_local;
    const int max_peak_h = h - 8;

    painter.draw_hline({x0, floor_abs_y}, w, ui::Color::darker_grey());

    int peak_width = 120 - (static_cast<int>(sharpness_) - 50) * 100 / 200;
    peak_width = std::max(15, peak_width);

    int effective_max_h = max_peak_h * (128 + static_cast<int>(peak_ratio_)) / 256;
    effective_max_h = std::min(max_peak_h, std::max(4, effective_max_h));

    int margin_h = 2 + static_cast<int>(margin_) * (floor_local - 4) / 200;
    margin_h = std::min(floor_local - 2, margin_h);

    int min_w_px = 2 + static_cast<int>(min_width_) * 58 / 100;
    int max_w_px = 4 + static_cast<int>(max_width_) * 116 / 255;

    for (int column = 0; column < w; column++) {
        int h1 = peak_height_at(column, PEAK1_CENTER, peak_width, effective_max_h);
        int h2 = peak_height_at(column, PEAK2_CENTER, peak_width, effective_max_h);
        int combined = std::max(h1, h2);
        if (combined > 0) {
            combined = std::min(combined, floor_local - 2);
            painter.draw_vline(
                {x0 + column, floor_abs_y - combined},
                combined,
                amplitude_color(combined, effective_max_h));
        }
    }

    for (int pi = 0; pi < 2; pi++) {
        int cx = (pi == 0) ? PEAK1_CENTER : PEAK2_CENTER;

        int l_min = cx - min_w_px;
        int r_min = cx + min_w_px;
        int l_max = cx - max_w_px;
        int r_max = cx + max_w_px;

        if (l_min >= 0) {
            painter.draw_vline({x0 + l_min, y0 + 2}, 3, ui::Color::cyan());
            painter.draw_hline({x0 + l_min, y0 + 2}, 3, ui::Color::cyan());
        }
        if (r_min < w) {
            painter.draw_vline({x0 + r_min, y0 + 2}, 3, ui::Color::cyan());
            painter.draw_hline({x0 + r_min - 2, y0 + 2}, 3, ui::Color::cyan());
        }
        if (l_max >= 0) {
            painter.draw_vline({x0 + l_max, y0 + 2}, 3, ui::Color::grey());
            painter.draw_hline({x0 + l_max, y0 + 2}, 3, ui::Color::grey());
        }
        if (r_max < w) {
            painter.draw_vline({x0 + r_max, y0 + 2}, 3, ui::Color::grey());
            painter.draw_hline({x0 + r_max - 2, y0 + 2}, 3, ui::Color::grey());
        }
    }

    if (margin_h > 2) {
        int margin_abs_y = floor_abs_y - margin_h;
        for (int x = x0; x < x0 + w; x += 5) {
            painter.draw_hline({x, margin_abs_y}, 3, ui::Color::grey());
        }
    }
}

} // namespace drone_analyzer
