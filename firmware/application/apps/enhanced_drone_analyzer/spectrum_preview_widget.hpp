#ifndef SPECTRUM_PREVIEW_WIDGET_HPP
#define SPECTRUM_PREVIEW_WIDGET_HPP

#include <cstdint>
#include <array>
#include "ui_widget.hpp"
#include "ui_painter.hpp"
#include "spectrum_shape.hpp"
#include "constants.hpp"

namespace drone_analyzer {

class SpectrumPreviewWidget : public ui::Widget {
public:
    explicit SpectrumPreviewWidget(ui::Rect parent_rect) noexcept;

    SpectrumPreviewWidget(const SpectrumPreviewWidget&) = delete;
    SpectrumPreviewWidget& operator=(const SpectrumPreviewWidget&) = delete;

    void paint(ui::Painter& painter) override;

    void set_params(
        uint8_t margin,
        uint8_t min_width,
        uint8_t max_width,
        uint8_t sharpness,
        uint8_t peak_ratio,
        uint8_t valley_depth,
        uint8_t flatness,
        uint8_t symmetry) noexcept;

private:
    struct Params {
        uint8_t margin{DEFAULT_SPECTRUM_MARGIN};
        uint8_t min_width{DEFAULT_SPECTRUM_MIN_WIDTH};
        uint8_t max_width{DEFAULT_SPECTRUM_MAX_WIDTH};
        uint8_t sharpness{DEFAULT_SPECTRUM_PEAK_SHARPNESS};
        uint8_t peak_ratio{DEFAULT_SPECTRUM_PEAK_RATIO};
        uint8_t valley_depth{DEFAULT_SPECTRUM_VALLEY_DEPTH};
        uint8_t flatness{DEFAULT_SPECTRUM_FLATNESS};
        uint8_t symmetry{DEFAULT_SPECTRUM_SYMMETRY};
    };

    Params params_;
    std::array<uint8_t, FFT_BIN_COUNT> synthetic_spectrum_{};
    std::array<uint8_t, FFT_BIN_COUNT> sort_buf_{};
    SpectrumShape::Config shape_config_{};
    SpectrumShape::AnalysisResult result_{};
    bool dirty_{true};

    void recompute() noexcept;
    static ui::Color amplitude_color(uint8_t value, uint8_t min_val, uint8_t max_val) noexcept;
};

} // namespace drone_analyzer

#endif // SPECTRUM_PREVIEW_WIDGET_HPP
