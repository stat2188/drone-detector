#ifndef SPECTRUM_PREVIEW_WIDGET_HPP
#define SPECTRUM_PREVIEW_WIDGET_HPP

#include <cstdint>

#include "constants.hpp"
#include "ui_widget.hpp"
#include "ui_painter.hpp"

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
    // Initializers ARE the constants.hpp defaults (single source of truth —
    // a default change there updates this preview automatically; the old
    // hardcoded set once said sharpness=120 / valley=80 and was left behind
    // by later default moves — it now reads 165 / 0, i.e. VLY OFF); runtime
    // values always arrive via set_params() from DroneSettingsView.
    // NOTE valley_depth_ is an ABSOLUTE flank-margin threshold (higher =
    // more permissive) and the preview normalizes the useful band 0..84
    // (SPECTRUM_VALLEY_SMOOTH_REACH) onto its pedestal — see
    // spectrum_preview_widget.cpp.
    uint8_t margin_{DEFAULT_SPECTRUM_MARGIN};
    uint8_t min_width_{DEFAULT_SPECTRUM_MIN_WIDTH};
    uint8_t max_width_{DEFAULT_SPECTRUM_MAX_WIDTH};
    uint8_t sharpness_{DEFAULT_SPECTRUM_PEAK_SHARPNESS};
    uint8_t peak_ratio_{DEFAULT_SPECTRUM_PEAK_RATIO};
    uint8_t valley_depth_{DEFAULT_SPECTRUM_VALLEY_DEPTH};
    uint8_t flatness_{DEFAULT_SPECTRUM_FLATNESS};
    uint8_t symmetry_{DEFAULT_SPECTRUM_SYMMETRY};

    static ui::Color amplitude_color(int32_t h, int32_t max_h) noexcept;
};

} // namespace drone_analyzer

#endif // SPECTRUM_PREVIEW_WIDGET_HPP
