#include <cstdint>
#include <algorithm>

#include "spectrum_preview_widget.hpp"

namespace drone_analyzer {

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
        return ui::Color(0, static_cast<uint8_t>(scaled & 0xFFu), 0);
    } else if (scaled < 512u) {
        return ui::Color(static_cast<uint8_t>(scaled - 256u), 255u, 0);
    } else {
        const uint32_t clamped = std::min(scaled, 767u);
        const uint32_t g = 255u - (clamped - 512u);
        return ui::Color(255u, static_cast<uint8_t>(g & 0xFFu), 0);
    }
}

void SpectrumPreviewWidget::paint(ui::Painter& painter) {
    const auto r = screen_rect();
    const int x0 = r.location().x();
    const int y0 = r.location().y();
    const int w = r.size().width();
    const int h = r.size().height();

    painter.fill_rectangle(r, ui::Color::black());

    // Layout constants
    const int floor_y = y0 + h - 2;
    const int peak_h = h - 6;

    // Noise floor line
    painter.draw_hline({x0, floor_y}, w, ui::Color::darker_grey());

    // Half-width in pixels: peak shows maximum acceptable signal width.
    int half_px = static_cast<int>(max_width_);
    half_px = std::max(5, std::min(w / 3, half_px * w / 512));

    // Peak ratio narrows the peak: high ratio = tall + narrow (selective).
    // Mirrors scanner logic: ratio = peak_margin * 10 / signal_width.
    half_px = half_px * (255 - static_cast<int>(peak_ratio_)) / 255;
    half_px = std::max(1, half_px);

    // Valley floor height (pixels above baseline)
    int valley_px = static_cast<int>(valley_depth_) * peak_h / 400;
    valley_px = std::min(valley_px, peak_h / 3);

    // Flat top width (pixels)
    int flat_px = 0;
    if (flatness_ > 0) {
        flat_px = half_px * static_cast<int>(flatness_) / 200;
    }

    // Symmetry offset: shows MAXIMUM allowed asymmetry
    // higher symmetry_ = stricter filter = more centered peak
    // symmetry_ = 100 (perfect required) → centered (0 offset)
    // symmetry_ = 0   (disabled)         → max offset
    int sym_off = (100 - static_cast<int>(symmetry_)) * half_px / 400;
    const int center = w / 2 + sym_off;

    // Sharpness factor (0-250, matches original)
    const int sharp_factor = std::min(static_cast<int>(sharpness_), 250);

    // Peak ratio height: maps 0-255 linearly across available vertical space.
    const int height_range = h - 2 - peak_h;
    const int peak_height = peak_h + (static_cast<int>(peak_ratio_) * height_range / 255);

    const int peak_above = peak_height - valley_px;
    const int max_slope = std::max(1, half_px - flat_px);

    // Render peak at pixel resolution (240 columns = 240 bins)
    for (int col = 0; col < w; col++) {
        int dx = col - center;
        if (dx < 0) dx = -dx;

        int col_h;
        if (flat_px > 0 && dx <= flat_px) {
            col_h = peak_height;
        } else if (dx <= half_px) {
            const int slope_dx = dx - flat_px;
            const int drop = slope_dx * sharp_factor * peak_above / (200 * max_slope);
            col_h = peak_height - std::min(drop, peak_above);
        } else {
            const int v_dist = dx - half_px;
            if (v_dist < 10) {
                col_h = valley_px * (10 - v_dist) / 10;
            } else {
                col_h = 0;
            }
        }

        col_h = std::max(0, std::min(peak_height, col_h));
        if (col_h > 0) {
            painter.draw_vline({x0 + col, floor_y - col_h}, col_h,
                               amplitude_color(col_h, peak_height));
        }
    }

    // Elevated threshold (solid grey at 25% of peak_h)
    const int elev_px = std::max(2, peak_h / 4);
    const int elev_y = floor_y - elev_px;
    painter.draw_hline({x0, elev_y}, w, ui::Color::dark_grey());

    // Margin threshold (orange dashed)
    const int margin_px = std::min(peak_h, static_cast<int>(margin_) * peak_h / 255);
    const int margin_y = floor_y - margin_px;
    if (margin_px > 2) {
        for (int x = x0; x < x0 + w; x += 5) {
            painter.draw_hline({x, margin_y}, 3, ui::Color::orange());
        }
    }

    // Min width markers (red vertical lines — signal must be at least this wide)
    const int min_px = static_cast<int>(min_width_) * w / 512;
    // Max width markers (grey vlines at elevated line) — use same base as half_px (incl. w/3 clamp)
    const int base_max = static_cast<int>(max_width_) * w / 512;
    const int max_px = std::max(5, std::min(w / 3, base_max));

    for (int side = -1; side <= 1; side += 2) {
        const int ml = center + side * min_px;
        if (ml >= 0 && ml < w && min_px > 0) {
            painter.draw_vline({x0 + ml, y0 + 1}, h - 3, ui::Color::red());
        }
        const int mr = center + side * max_px;
        if (mr >= 0 && mr < w) {
            painter.draw_vline({x0 + mr, elev_y - 2}, 5, ui::Color::grey());
        }
    }

    // Valley indicators (filled bars at half-width + 1)
    for (int side = -1; side <= 1; side += 2) {
        const int vx = center + side * (half_px + 1);
        if (vx >= 0 && vx < w && valley_px > 0) {
            const int vh = std::min(valley_px, h - 4);
            const ui::Color vc = (valley_depth_ == 0) ? ui::Color::green() : ui::Color::red();
            painter.fill_rectangle({x0 + vx - 1, floor_y - vh, 3, vh}, vc);
        }
    }
}

} // namespace drone_analyzer
