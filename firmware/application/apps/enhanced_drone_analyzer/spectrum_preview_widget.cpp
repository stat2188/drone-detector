#include <cstdint>
#include <algorithm>

#include "spectrum_preview_widget.hpp"
#include "theme.hpp"

namespace drone_analyzer {

// Scaling factor: maps parameter range (0-255) to pixel space.
// For a 240px-wide widget: max_width_=200 -> 200*240/512 ~ 94px (~39% width).
constexpr int PARAM_TO_PX = 512;

// Color gradient segments for amplitude_color(): green->yellow->red across 768 steps.
constexpr uint32_t COLOR_SEGMENTS = 3u;
constexpr uint32_t COLOR_SEGMENT_SIZE = 256u;

// Valley slope decay distance in pixels beyond signal half-width edge.
constexpr int VALLEY_DECAY_PX = 10;

// Dashed line parameters for margin threshold visualization.
constexpr int DASH_SEGMENT_PX = 3;
constexpr int DASH_PERIOD_PX = 5;

// Marker line heights (pixels).
constexpr int MARKER_HEIGHT = 5;

// Ratio threshold line dash pattern.
constexpr int RATIO_DASH_PX = 2;
constexpr int RATIO_DASH_PERIOD_PX = 4;

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

// Three-segment color gradient: green (0-33%) -> yellow (33-67%) -> red (67-100%).
// Maps normalized height h/max_h to a heat-map color for the spectrum peak waveform.
ui::Color SpectrumPreviewWidget::amplitude_color(int32_t h, int32_t max_h) noexcept {
    if (max_h <= 0) return ui::Color::green();
    const uint32_t scaled = static_cast<uint32_t>(h) * (COLOR_SEGMENTS * COLOR_SEGMENT_SIZE) / static_cast<uint32_t>(max_h);
    if (scaled < COLOR_SEGMENT_SIZE) {
        return ui::Color(0, static_cast<uint8_t>(scaled & 0xFFu), 0);
    } else if (scaled < COLOR_SEGMENT_SIZE * 2u) {
        return ui::Color(static_cast<uint8_t>(scaled - COLOR_SEGMENT_SIZE), 255u, 0);
    } else {
        const uint32_t clamped = std::min<uint32_t>(scaled, COLOR_SEGMENTS * COLOR_SEGMENT_SIZE - 1u);
        const uint32_t g = COLOR_SEGMENT_SIZE - 1u - (clamped - COLOR_SEGMENT_SIZE * 2u);
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

    // Layout: 2px bottom margin for noise floor, 4px top margin for labels.
    const int floor_y = y0 + h - 2;
    const int peak_h = h - 6;

    // --- Noise floor line (always visible) ---
    painter.draw_hline({x0, floor_y}, w, ui::Color::darker_grey());

    // --- Peak shape: controlled by MaxW (base width), Sharpness, Flatness, Valley, Symmetry ---
    // MaxW sets the base half-width. Rat does NOT affect the peak shape.
    int half_px = static_cast<int>(max_width_);
    half_px = std::max(5, std::min(w / 3, half_px * w / PARAM_TO_PX));

    // Valley floor height (pixels above baseline). Maps 0-200 to 0-50% of peak_h.
    int valley_px = static_cast<int>(valley_depth_) * peak_h / 400;
    valley_px = std::min(valley_px, peak_h / 3);

    // Flat top width (pixels). Maps flatness 0-100 to 0-50% of half_px.
    int flat_px = 0;
    if (flatness_ > 0) {
        flat_px = half_px * static_cast<int>(flatness_) / 200;
    }

    // Symmetry offset: shows maximum allowed asymmetry.
    // symmetry_=100 -> centered (0 offset), symmetry_=0 -> max offset.
    int sym_off = (100 - static_cast<int>(symmetry_)) * half_px / 400;
    const int center = w / 2 + sym_off;

    // Sharpness factor (0-250, matches scanner range).
    const int sharp_factor = std::min(static_cast<int>(sharpness_), 250);

    // Peak height: fixed at peak_h (Rat no longer affects width or height).
    const int peak_height = peak_h;

    const int peak_above = peak_height - valley_px;
    const int max_slope = std::max(1, half_px - flat_px);

    // Render peak at pixel resolution (one column per pixel).
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
            if (v_dist < VALLEY_DECAY_PX) {
                col_h = valley_px * (VALLEY_DECAY_PX - v_dist) / VALLEY_DECAY_PX;
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

    // --- Margin threshold: orange dashed horizontal line ---
    // Maps margin 0-255 to 0-peak_h pixels. Labeled "M" at left edge.
    const int margin_px = std::min(peak_h, static_cast<int>(margin_) * peak_h / 255);
    const int margin_y = floor_y - margin_px;
    if (margin_px > 2) {
        for (int x = x0; x < x0 + w; x += DASH_PERIOD_PX) {
            painter.draw_hline({x, margin_y}, DASH_SEGMENT_PX, ui::Color::orange());
        }
    }

    // --- Width markers: red = min_width, grey = max_width ---
    // These markers show the ABSOLUTE width boundaries. MaxW markers do NOT
    // respond to Rat — they only move when MaxW changes.
    const int min_px = static_cast<int>(min_width_) * w / PARAM_TO_PX;
    const int max_px = std::max(5, std::min(w / 3, static_cast<int>(max_width_) * w / PARAM_TO_PX));

    for (int side = -1; side <= 1; side += 2) {
        const int ml = center + side * min_px;
        if (ml >= 0 && ml < w && min_px > 0) {
            painter.draw_vline({x0 + ml, y0 + 1}, h - 3, ui::Color::red());
        }
        const int mr = center + side * max_px;
        if (mr >= 0 && mr < w) {
            painter.draw_vline({x0 + mr, y0 + 1}, h - 3, ui::Color::grey());
        }
    }

    // --- Valley indicators: small filled bars at signal edge ---
    // Red if valley_depth > 0 (filter active), green if disabled.
    for (int side = -1; side <= 1; side += 2) {
        const int vx = center + side * (half_px + 1);
        if (vx >= 0 && vx < w && valley_px > 0) {
            const int vh = std::min(valley_px, h - 4);
            const ui::Color vc = (valley_depth_ == 0) ? ui::Color::green() : ui::Color::red();
            painter.fill_rectangle({x0 + vx - 1, floor_y - vh, 3, vh}, vc);
        }
    }

    // --- Ratio threshold: cyan diagonal line from bottom-left to peak ---
    // Shows the height/width ratio boundary. Higher Rat = steeper line = harder to pass.
    // ratio = peak_margin * 10 / signal_width. Line slope = peak_ratio / 10.
    if (peak_ratio_ > 0) {
        const int ratio_slope = static_cast<int>(peak_ratio_) * peak_h / (255 * 10);
        // Draw from left edge toward center, stopping at the peak.
        for (int x = x0; x < x0 + center && x < x0 + w; x += RATIO_DASH_PERIOD_PX) {
            const int dist_from_center = center - (x - x0);
            const int line_y = floor_y - (dist_from_center * ratio_slope / std::max(1, half_px));
            if (line_y >= y0 && line_y <= floor_y) {
                painter.draw_hline({x, line_y}, RATIO_DASH_PX, ui::Color::cyan());
            }
        }
        // Mirror on right side.
        for (int x = x0 + w - 1; x > x0 + center && x >= x0; x -= RATIO_DASH_PERIOD_PX) {
            const int dist_from_center = x - (x0 + center);
            const int line_y = floor_y - (dist_from_center * ratio_slope / std::max(1, half_px));
            if (line_y >= y0 && line_y <= floor_y) {
                painter.draw_hline({x - RATIO_DASH_PX + 1, line_y}, RATIO_DASH_PX, ui::Color::cyan());
            }
        }
    }

    // --- Inline parameter labels ---
    // Each label is drawn at the visual element it describes.
    // Font: 8x16 default font, positioned to avoid overlap.
    const auto& font = ui::Theme::getInstance()->fg_light->font;

    // "M" near margin dashed line (orange) — identifies the margin threshold.
    if (margin_px > 4) {
        painter.draw_string({x0 + 1, margin_y - 14}, font,
                            ui::Color::orange(), ui::Color::black(), "M");
    }

    // "mn" near left red marker — identifies minimum width boundary.
    if (min_px > 8 && min_px < w - 24) {
        painter.draw_string({x0 + center - min_px - 9, y0 + 1}, font,
                            ui::Color::red(), ui::Color::black(), "mn");
    }

    // "mx" near left grey marker — identifies maximum width boundary.
    if (max_px > 16 && max_px < w - 24) {
        painter.draw_string({x0 + center - max_px - 9, y0 + 1}, font,
                            ui::Color::grey(), ui::Color::black(), "mx");
    }

    // "R" at top-right when peak_ratio > 0 — identifies ratio threshold line.
    if (peak_ratio_ > 0) {
        painter.draw_string({x0 + w - 9, y0 + 1}, font,
                            ui::Color::cyan(), ui::Color::black(), "R");
    }
}

} // namespace drone_analyzer
