#include <cstdint>
#include <algorithm>

#include "spectrum_preview_widget.hpp"

namespace drone_analyzer {

namespace {

constexpr int CENTER = 120;
constexpr int NOISE_FLOOR_OFFSET = 4;
constexpr int VALLEY_ZONE = 5;

int32_t v_shape_height(
    int32_t dx,
    int32_t half_width,
    int32_t peak_height,
    int32_t valley_margin_px) noexcept {
    if (dx <= half_width) {
        return peak_height - dx * (peak_height * 3 / 4) / std::max<int32_t>(1, half_width);
    }
    int32_t v_dist = dx - half_width;
    if (v_dist < VALLEY_ZONE) {
        return valley_margin_px * (VALLEY_ZONE - v_dist) / VALLEY_ZONE;
    }
    return 0;
}

int32_t flat_top_width(int32_t flatness, int32_t half_width) noexcept {
    if (flatness == 0) return 0;
    return half_width * flatness / 100;
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
    uint8_t peak_ratio,
    uint8_t valley_depth,
    uint8_t flatness,
    uint8_t symmetry) noexcept {
    margin_ = margin;
    min_width_ = min_width;
    max_width_ = max_width;
    sharpness_ = sharpness;
    peak_ratio_ = peak_ratio;
    valley_depth_ = valley_depth;
    flatness_ = flatness;
    symmetry_ = symmetry;
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

    const int floor_y = y0 + h - NOISE_FLOOR_OFFSET;
    const int peak_h = h - 8;
    const int elevated_h = peak_h / 4;

    painter.draw_hline({x0, floor_y}, w, ui::Color::darker_grey());

    const int elevated_abs_y = floor_y - elevated_h;

    int half_width = (static_cast<int>(min_width_) + static_cast<int>(max_width_)) / 2;
    half_width = std::max(5, std::min(w / 3, half_width * w / 512));

    int valley_margin_px = static_cast<int>(valley_depth_) * 10 / 255;

    int flat_top_px = flat_top_width(flatness_, half_width);

    int sym_offset_x = 0;
    if (symmetry_ > 0) {
        sym_offset_x = (static_cast<int>(symmetry_) - 50) * half_width / 200;
    }
    int actual_center = CENTER + sym_offset_x;

    for (int col = 0; col < w; col++) {
        int dx = std::abs(col - actual_center);
        int ft_dx = std::abs(col - actual_center);

        int h_px;
        if (flat_top_px > 0 && ft_dx <= flat_top_px) {
            h_px = peak_h;
        } else {
            int valley_display_h = valley_margin_px;
            h_px = v_shape_height(dx, half_width, peak_h, valley_display_h);
        }

        h_px = std::max(0, std::min(peak_h, h_px));
        if (h_px > 0) {
            ui::Color c = amplitude_color(h_px, peak_h);
            painter.draw_vline({x0 + col, floor_y - h_px}, h_px, c);
        }
    }

    int margin_px = static_cast<int>(margin_) * peak_h / 255;
    int margin_abs_y = floor_y - margin_px;
    if (margin_px > 2) {
        for (int x = x0; x < x0 + w; x += 5) {
            painter.draw_hline({x, margin_abs_y}, 3, ui::Color::orange());
        }
    }

    painter.draw_hline({x0, elevated_abs_y}, w, ui::Color::dark_grey());

    int min_w_px = static_cast<int>(min_width_) * w / 512;
    int max_w_px = static_cast<int>(max_width_) * w / 256;

    for (int side = -1; side <= 1; side += 2) {
        int l = actual_center + side * min_w_px;
        if (l >= 0 && l < w) {
            painter.draw_vline({x0 + l, elevated_abs_y - 2}, 5, ui::Color::cyan());
        }
        int r = actual_center + side * max_w_px;
        if (r >= 0 && r < w) {
            painter.draw_vline({x0 + r, elevated_abs_y - 2}, 5, ui::Color::grey());
        }
    }

    int half_sig = half_width;
    for (int side = -1; side <= 1; side += 2) {
        int vx = actual_center + side * (half_sig + 1);
        if (vx >= 0 && vx < w && valley_margin_px > 0) {
            int vh = std::min(valley_margin_px, h - NOISE_FLOOR_OFFSET - 2);
            if (vh > 0) {
                ui::Color vc = (valley_depth_ < 10) ? ui::Color::green() : ui::Color::red();
                painter.fill_rectangle({x0 + vx - 1, floor_y - vh, 3, vh}, vc);
            }
        }
    }
}

} // namespace drone_analyzer
