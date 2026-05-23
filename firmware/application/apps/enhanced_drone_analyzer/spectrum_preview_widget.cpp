#include <cstdint>
#include <algorithm>

#include "spectrum_preview_widget.hpp"

namespace drone_analyzer {

namespace {

constexpr uint8_t NOISE_FLOOR = 100;
constexpr uint8_t PEAK_VALUE = 200;
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
    // Fill with noise floor
    synthetic_spectrum_.fill(NOISE_FLOOR);

    // Average min/max width as design half-width, clamped to fit before DC spike
    size_t hw = (static_cast<size_t>(params_.min_width) + static_cast<size_t>(params_.max_width)) / 2;
    const size_t right_room = FFT_DC_SPIKE_START - 1 - PEAK_BIN;
    const size_t left_room = PEAK_BIN - FFT_EDGE_SKIP_NARROW;
    hw = std::max<size_t>(2, std::min(hw, std::min(left_room, right_room)));

    // Symmetry: generate signal with ~70% of config_symmetry asymmetry
    // so the user sees the effect of tightening/loosening this filter
    size_t left_w = hw;
    size_t right_w = hw;
    {
        // Make the generated signal somewhat asymmetric to give symmetry filter meaningful feedback
        const uint32_t gen_sym = (static_cast<uint32_t>(params_.symmetry) + 100u) / 2u;
        if (gen_sym < 100u && params_.symmetry > 0) {
            right_w = std::max<size_t>(1, left_w * gen_sym / 100u);
        }
    }

    // Flat top: signal bins at peak power
    size_t flat_bins = 0;
    if (params_.flatness > 0 && params_.flatness < 100) {
        const size_t total_w = left_w + right_w;
        flat_bins = std::max<size_t>(1, total_w * params_.flatness / 200);
    } else if (params_.flatness >= 100) {
        flat_bins = std::max(left_w, right_w);
    }

    // Peak ratio: ensure enough peak margin to pass the ratio check
    uint8_t peak_val = PEAK_VALUE;
    if (params_.peak_ratio > 0) {
        const size_t total_w = left_w + right_w + 1;
        const int32_t needed_margin = (static_cast<int32_t>(params_.peak_ratio) * static_cast<int32_t>(total_w)) / 10;
        const int32_t needed_peak = static_cast<int32_t>(NOISE_FLOOR) + needed_margin;
        if (needed_peak > static_cast<int32_t>(PEAK_VALUE) && needed_peak < 250) {
            peak_val = static_cast<uint8_t>(needed_peak);
        }
    }

    // Generate the V-shape across usable bins
    for (size_t i = FFT_EDGE_SKIP_NARROW; i < FFT_BIN_COUNT - FFT_EDGE_SKIP_NARROW; ++i) {
        if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;

        int32_t val = NOISE_FLOOR;

        if (i == PEAK_BIN) {
            val = peak_val;
        } else if (i > PEAK_BIN) {
            const size_t dist = i - PEAK_BIN;
            if (dist <= flat_bins) {
                val = peak_val;
            } else if (dist <= right_w) {
                const size_t slope_dist = dist - flat_bins;
                const size_t max_slope_dist = right_w - std::min(flat_bins, right_w);
                if (max_slope_dist > 0) {
                    const uint32_t factor = std::min<uint32_t>(params_.sharpness, 250u);
                    val = static_cast<int32_t>(peak_val)
                        - static_cast<int32_t>(static_cast<uint32_t>(slope_dist) * factor
                            * static_cast<uint32_t>(peak_val - NOISE_FLOOR)
                            / (250u * static_cast<uint32_t>(std::max<size_t>(1, max_slope_dist))));
                    val = std::max<int32_t>(NOISE_FLOOR, val);
                }
            } else {
                const size_t v_dist = dist - right_w;
                const uint32_t depth = 60u / std::max<uint32_t>(1, static_cast<uint32_t>(v_dist));
                val = NOISE_FLOOR + static_cast<int32_t>(std::min<uint32_t>(depth, 155u));
            }
        } else {
            const size_t dist = PEAK_BIN - i;
            if (dist <= flat_bins) {
                val = peak_val;
            } else if (dist <= left_w) {
                const size_t slope_dist = dist - flat_bins;
                const size_t max_slope_dist = left_w - std::min(flat_bins, left_w);
                if (max_slope_dist > 0) {
                    const uint32_t factor = std::min<uint32_t>(params_.sharpness, 250u);
                    val = static_cast<int32_t>(peak_val)
                        - static_cast<int32_t>(static_cast<uint32_t>(slope_dist) * factor
                            * static_cast<uint32_t>(peak_val - NOISE_FLOOR)
                            / (250u * static_cast<uint32_t>(std::max<size_t>(1, max_slope_dist))));
                    val = std::max<int32_t>(NOISE_FLOOR, val);
                }
            } else {
                const size_t v_dist = dist - left_w;
                const uint32_t depth = 60u / std::max<uint32_t>(1, static_cast<uint32_t>(v_dist));
                val = NOISE_FLOOR + static_cast<int32_t>(std::min<uint32_t>(depth, 155u));
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

    // Find display range
    uint8_t min_val = 255, max_val = 0;
    for (const auto v : synthetic_spectrum_) {
        if (v > max_val) max_val = v;
        if (v < min_val) min_val = v;
    }
    if (max_val <= min_val) { min_val = 0; max_val = 255; }

    const int graph_h = h - 2;
    const int floor_y = y0 + h - 1;

    // Draw spectrum vertical bars (256 bins → 240 pixels)
    for (int px = 0; px < w; px++) {
        const size_t bin = static_cast<size_t>(px) * FFT_BIN_COUNT / static_cast<size_t>(w);
        const uint8_t val = synthetic_spectrum_[std::min(bin, FFT_BIN_COUNT - 1)];
        const int bar_h = static_cast<int>(static_cast<int32_t>(val - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        if (bar_h > 0) {
            const int bar_y = floor_y - bar_h;
            painter.draw_vline({x0 + px, bar_y}, bar_h, amplitude_color(val, min_val, max_val));
        }
    }

    // PASS/FAIL indicator (top-left corner)
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

    // Peak marker (white vertical line)
    if (result_.peak_value > 0) {
        const int peak_px = x0 + static_cast<int>(result_.peak_index) * w / static_cast<int>(FFT_BIN_COUNT);
        const int p_px = static_cast<int>(static_cast<int32_t>(result_.peak_value - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        painter.draw_vline({peak_px, floor_y - p_px}, std::max(1, p_px), ui::Color::white());
    }

    // Elevated threshold line (solid darker grey)
    if (result_.peak_margin > 0) {
        const uint8_t elev_val = result_.noise_floor + result_.peak_margin / 4;
        const int e_px = static_cast<int>(static_cast<int32_t>(elev_val - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        const int e_y = floor_y - e_px;
        painter.draw_hline({x0, e_y}, w, ui::Color::darker_grey());
    }

    // Width markers: min=cyan, max=grey (only with valid peak)
    if (result_.peak_margin > 0) {
        const int peak_px = x0 + static_cast<int>(result_.peak_index) * w / static_cast<int>(FFT_BIN_COUNT);
        const int e_px = static_cast<int>(static_cast<int32_t>(result_.noise_floor + result_.peak_margin / 4 - min_val) * graph_h / static_cast<int32_t>(max_val - min_val + 1));
        const int e_y = floor_y - e_px;
        const int min_w_px = static_cast<int>(shape_config_.min_width) * w / static_cast<int>(FFT_BIN_COUNT);
        const int max_w_px = static_cast<int>(shape_config_.max_width) * w / static_cast<int>(FFT_BIN_COUNT);
        for (int side = -1; side <= 1; side += 2) {
            const int l = peak_px + side * min_w_px;
            if (l >= x0 && l < x0 + w) {
                painter.draw_vline({l, e_y - 2}, 5, ui::Color::cyan());
            }
            const int r = peak_px + side * max_w_px;
            if (r >= x0 && r < x0 + w) {
                painter.draw_vline({r, e_y - 2}, 5, ui::Color::grey());
            }
        }
    }
}

} // namespace drone_analyzer
