#ifndef SPECTRUM_PREVIEW_WIDGET_HPP
#define SPECTRUM_PREVIEW_WIDGET_HPP

#include <cstdint>
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
        uint8_t peak_ratio) noexcept;

private:
    uint8_t margin_{20};
    uint8_t min_width_{3};
    uint8_t max_width_{200};
    uint8_t sharpness_{80};
    uint8_t peak_ratio_{0};

    static ui::Color amplitude_color(int32_t h, int32_t max_h) noexcept;
};

} // namespace drone_analyzer

#endif // SPECTRUM_PREVIEW_WIDGET_HPP
