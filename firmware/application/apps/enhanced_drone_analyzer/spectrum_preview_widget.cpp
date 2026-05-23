#include <cstdint>
#include <algorithm>

#include "spectrum_preview_widget.hpp"

namespace drone_analyzer {

namespace {

constexpr uint8_t NOISE_FLOOR = 100;
constexpr uint8_t PEAK_MARGIN = 100;
constexpr size_t PEAK_BIN = FFT_BIN_COUNT / 2 - 12;

} // namespace

SpectrumPreviewWidget::SpectrumPreviewWidget(ui::Rect parent_rect) noexcept
    : Widget{parent_rect}
    , params_{}
    , synthetic_spectrum_{}
    , sort_buf_{}
    , shape_config_{}
    , result_{}
    , dirty_{true} {
    recompute();
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
    params_.margin = margin;
    params_.min_width = min_width;
    params_.max_width = max_width;
    params_.sharpness = sharpness;
    params_.peak_ratio = peak_ratio;
    params_.valley_depth = valley_depth;
    params_.flatness = flatness;
    params_.symmetry = symmetry;
    shape_config_.margin = margin;
    shape_config_.min_width = min_width;
    shape_config_.max_width = max_width;
    shape_config_.peak_sharpness = sharpness;
    shape_config_.peak_ratio = peak_ratio;
    shape_config_.valley_depth = valley_depth;
    shape_config_.flatness = flatness;
    shape_config_.symmetry = symmetry;
    shape_config_.use_cfar = false;
    dirty_ = true;
    set_dirty();
}

void SpectrumPreviewWidget::recompute() noexcept {
    synthetic_spectrum_.fill(NOISE_FLOOR);

    // Signal width derived from min_width.
    // Symmetry maps to right/left ratio:
    //   sym=0 (disabled) → symmetric via else branch
    //   sym=50 → right = 75% of left (moderate asymmetry)
    //   sym=100 → right = 100% of left (perfect symmetry)
    size_t left_w = 1, right_w = 1;

    if (params_.symmetry > 0 && params_.symmetry < 100) {
        // Ensure (left + right + 1) >= min_width where right = left * ratio/100
        const uint32_t right_ratio = 50u + (static_cast<uint32_t>(params_.symmetry) + 1u) / 2u;
        const size_t denom = 100u + right_ratio;
        const size_t min_left = ((static_cast<size_t>(params_.min_width) - 1) * 100u + denom - 1u) / denom;
        left_w = std::max<size_t>(min_left, 2);
        right_w = std::max<size_t>(1, left_w * right_ratio / 100u);
    } else {
        left_w = static_cast<size_t>(params_.min_width) / 2;
        if (left_w < 1) left_w = 1;
        right_w = static_cast<size_t>(params_.min_width) - left_w - 1;
        if (right_w < 1) right_w = 1;
    }

    // Clamp to fit within usable spectrum range (DC spike at 120-135, edge skip 6)
    const size_t right_room = FFT_DC_SPIKE_START - 1 - PEAK_BIN;
    const size_t left_room = PEAK_BIN - FFT_EDGE_SKIP_NARROW;
    const size_t max_total = left_room + right_room + 1;
    if (left_w + right_w + 1 > max_total) {
        const size_t max_pair = max_total - 1;
        const size_t total_hw = left_w + right_w;
        if (total_hw > 0) {
            left_w = std::max<size_t>(1, left_w * max_pair / total_hw);
            right_w = max_pair - left_w;
            if (right_w < 1) right_w = 1;
        }
    }

    // Flat top: bins at peak power
    size_t flat_bins = 0;
    if (params_.flatness == 0) {
        flat_bins = 0;
    } else if (params_.flatness >= 100) {
        flat_bins = std::max(left_w, right_w);
    } else {
        flat_bins = (left_w + right_w) * static_cast<size_t>(params_.flatness) / 200;
    }

    // Peak value: PEAK_MARGIN above noise floor, increased by peak_ratio if needed
    uint8_t peak_val = NOISE_FLOOR + PEAK_MARGIN;
    if (params_.peak_ratio > 0) {
        const int32_t effective_width = static_cast<int32_t>(left_w + right_w + 1);
        const int32_t needed_margin = (static_cast<int32_t>(params_.peak_ratio) * effective_width) / 10;
        const int32_t needed_peak = static_cast<int32_t>(NOISE_FLOOR) + needed_margin;
        if (needed_peak > 250) {
            peak_val = 250;
        } else if (needed_peak > static_cast<int32_t>(peak_val)) {
            peak_val = static_cast<uint8_t>(needed_peak);
        }
    }

    const uint32_t sharp_factor = std::min<uint32_t>(static_cast<uint32_t>(params_.sharpness), 250u);
    const int32_t peak_above_noise = static_cast<int32_t>(peak_val) - static_cast<int32_t>(NOISE_FLOOR);

    for (size_t i = FFT_EDGE_SKIP_NARROW; i < FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW; ++i) {
        if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;

        int32_t val = NOISE_FLOOR;

        if (i == PEAK_BIN) {
            val = peak_val;
        } else if (i > PEAK_BIN) {
            const size_t dist = i - PEAK_BIN;
            if (flat_bins > 0 && dist <= flat_bins) {
                val = peak_val;
            } else if (dist <= right_w) {
                const size_t slope_dist = dist - std::min(flat_bins, dist);
                const size_t max_slope_dist = right_w - std::min(flat_bins, right_w);
                if (max_slope_dist > 0) {
                    val = static_cast<int32_t>(peak_val)
                        - static_cast<int32_t>(static_cast<uint32_t>(slope_dist) * sharp_factor
                            * static_cast<uint32_t>(peak_above_noise)
                            / (200u * static_cast<uint32_t>(max_slope_dist)));
                    val = std::max<int32_t>(NOISE_FLOOR, val);
                }
            } else {
                // Valley height depends on flatness:
                // flatness=0  → NOISE_FLOOR+2  (deep V, passes valley check easily)
                // flatness=50 → NOISE_FLOOR+24 (moderate, tests valley check)
                // flatness=100 → NOISE_FLOOR+24 (capped so valley < elevated threshold ~125)
                const uint32_t raw_extra = 2u + static_cast<uint32_t>(params_.flatness) * 58u / 100u;
                const uint32_t valley_extra = std::min<uint32_t>(raw_extra, 24u);
                val = NOISE_FLOOR + static_cast<int32_t>(valley_extra);
            }
        } else {
            const size_t dist = PEAK_BIN - i;
            if (flat_bins > 0 && dist <= flat_bins) {
                val = peak_val;
            } else if (dist <= left_w) {
                const size_t slope_dist = dist - std::min(flat_bins, dist);
                const size_t max_slope_dist = left_w - std::min(flat_bins, left_w);
                if (max_slope_dist > 0) {
                    val = static_cast<int32_t>(peak_val)
                        - static_cast<int32_t>(static_cast<uint32_t>(slope_dist) * sharp_factor
                            * static_cast<uint32_t>(peak_above_noise)
                            / (200u * static_cast<uint32_t>(max_slope_dist)));
                    val = std::max<int32_t>(NOISE_FLOOR, val);
                }
            } else {
                const uint32_t raw_extra = 2u + static_cast<uint32_t>(params_.flatness) * 58u / 100u;
                const uint32_t valley_extra = std::min<uint32_t>(raw_extra, 24u);
                val = NOISE_FLOOR + static_cast<int32_t>(valley_extra);
            }
        }

        val = std::max<int32_t>(0, std::min<int32_t>(255, val));
        synthetic_spectrum_[i] = static_cast<uint8_t>(val);
    }

    result_ = SpectrumShape::analyze(synthetic_spectrum_.data(), sort_buf_.data(), shape_config_);
    dirty_ = false;
}

ui::Color SpectrumPreviewWidget::amplitude_color(uint8_t value, uint8_t min_val, uint8_t max_val) noexcept {
    if (value <= min_val || max_val <= min_val) return ui::Color::black();
    const uint32_t scaled = (static_cast<uint32_t>(value - min_val) * 768u) / static_cast<uint32_t>(max_val - min_val);
    if (scaled < 256u) {
        return ui::Color(0, static_cast<uint8_t>(scaled & 0xFFu), 0);
    } else if (scaled < 512u) {
        return ui::Color(static_cast<uint8_t>(scaled - 256u), 255u, 0);
    } else {
        const uint32_t g = std::min<uint32_t>(255u, 255u - (scaled - 512u));
        return ui::Color(255u, static_cast<uint8_t>(g & 0xFFu), 0);
    }
}

void SpectrumPreviewWidget::paint(ui::Painter& painter) {
    if (dirty_) recompute();

    const auto r = screen_rect();
    const int x0 = r.location().x();
    const int y0 = r.location().y();
    const int w = r.size().width();
    const int h = r.size().height();

    painter.fill_rectangle(r, ui::Color::black());

    uint8_t min_val = 255, max_val = 0;
    for (const auto v : synthetic_spectrum_) {
        if (v > max_val) max_val = v;
        if (v < min_val) min_val = v;
    }
    if (max_val <= min_val) { min_val = 0; max_val = 255; }

    const int graph_h = h - 2;
    const int floor_y = y0 + h - 1;

    // Spectrum vertical bars (256 bins -> 240 pixels)
    for (int px = 0; px < w; px++) {
        const size_t bin = static_cast<size_t>(px) * FFT_BIN_COUNT / static_cast<size_t>(w);
        const uint8_t val = synthetic_spectrum_[std::min(bin, FFT_BIN_COUNT - 1)];
        const int bar_h = static_cast<int>(static_cast<int32_t>(val - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        if (bar_h > 0) {
            const int bar_y = floor_y - bar_h;
            painter.draw_vline({x0 + px, bar_y}, bar_h, amplitude_color(val, min_val, max_val));
        }
    }

    // PASS/FAIL indicator (top-left)
    {
        const auto indicator_color = result_.signal_detected ? ui::Color::green() : ui::Color::red();
        painter.fill_rectangle({x0 + 2, y0 + 2, 6, 6}, indicator_color);
    }

    // Noise floor line (dashed dark grey)
    {
        const int nf_px = static_cast<int>(static_cast<int32_t>(result_.noise_floor - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        const int nf_y = floor_y - nf_px;
        for (int px = 0; px < w; px += 4) {
            painter.draw_hline({x0 + px, nf_y}, 2, ui::Color::dark_grey());
        }
    }

    // Margin threshold line (orange dashed)
    if (result_.noise_floor > 0 && shape_config_.margin > 0) {
        const uint8_t margin_val = result_.noise_floor + shape_config_.margin;
        const int m_px = static_cast<int>(static_cast<int32_t>(margin_val - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        const int m_y = floor_y - m_px;
        for (int px = 0; px < w; px += 5) {
            painter.draw_hline({x0 + px, m_y}, 3, ui::Color::orange());
        }
    }

    if (result_.peak_value == 0) return;

    const int peak_px = x0 + static_cast<int>(result_.peak_index) * w / static_cast<int>(FFT_BIN_COUNT);

    // Peak marker (white vertical line)
    {
        const int p_px = static_cast<int>(static_cast<int32_t>(result_.peak_value - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        painter.draw_vline({peak_px, floor_y - p_px}, std::max(1, p_px), ui::Color::white());
    }

    // Elevated threshold line (solid darker grey)
    if (result_.peak_margin > 0) {
        const uint8_t elev_val = result_.noise_floor + result_.peak_margin / 4;
        const int e_px = static_cast<int>(static_cast<int32_t>(elev_val - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        const int e_y = floor_y - e_px;
        painter.draw_hline({x0, e_y}, w, ui::Color::darker_grey());

        if (result_.signal_width > 0) {
            // Measured signal boundaries (cyan) — where signal drops below elevated threshold
            const int sig_left_px = x0 + static_cast<int>(result_.sig_left) * w / static_cast<int>(FFT_BIN_COUNT);
            const int sig_right_px = x0 + static_cast<int>(result_.sig_right) * w / static_cast<int>(FFT_BIN_COUNT);
            painter.draw_vline({sig_left_px, e_y - 2}, 5, ui::Color::cyan());
            painter.draw_vline({sig_right_px, e_y - 2}, 5, ui::Color::cyan());

            // Max width rejection boundary (grey) — signals wider than this boundary fail
            if (shape_config_.max_width < FFT_BIN_COUNT) {
                const int max_hw_px = static_cast<int>(shape_config_.max_width) * w / (2 * static_cast<int>(FFT_BIN_COUNT));
                if (max_hw_px > 0) {
                    const int max_l = peak_px - max_hw_px;
                    const int max_r = peak_px + max_hw_px;
                    if (max_l >= x0)
                        painter.draw_vline({max_l, e_y - 2}, 5, ui::Color::grey());
                    if (max_r < x0 + w)
                        painter.draw_vline({max_r, e_y - 2}, 5, ui::Color::grey());
                }
            }
        }
    }
}

} // namespace drone_analyzer
