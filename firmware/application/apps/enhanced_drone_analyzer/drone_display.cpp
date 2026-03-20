#include "drone_display.hpp"

#include <string_view>

namespace drone_analyzer {

// ============================================================================
// Constructor / Destructor
// ============================================================================

DroneDisplay::DroneDisplay(const Rect parent_rect) noexcept
    : ui::View()
    , display_data_()
    , spectrum_buffer_{}
    , histogram_buffer_{}
    , status_text_{0}
    , spectrum_data_size_(0)
    , histogram_data_size_(0)
    , spectrum_visible_(true)
    , histogram_visible_(true)
    , drone_list_visible_(true)
    , status_bar_visible_(true) {
    set_parent_rect(parent_rect);
    set_status_text(STATUS_READY);
}

DroneDisplay::~DroneDisplay() noexcept {
    // Destructor - no dynamic memory to free
}

// ============================================================================
// Paint Method
// ============================================================================

void DroneDisplay::paint(Painter& painter) {
    const auto sr = screen_rect();
    const uint16_t ox = sr.location().x();
    const uint16_t oy = sr.location().y();
    const uint16_t w = sr.size().width();
    const uint16_t total_h = sr.size().height();

    // Clear widget area
    draw_rectangle(painter, ox, oy, w, total_h, COLOR_BACKGROUND);

    // Calculate section heights dynamically
    constexpr uint16_t STATUS_H = 16;
    constexpr uint16_t SPECTRUM_H = 50;
    constexpr uint16_t HISTOGRAM_H = 30;

    uint16_t remaining = total_h;

    const bool show_spec = (spectrum_visible_ && spectrum_data_size_ > 0);
    const bool show_hist = (histogram_visible_ && histogram_data_size_ > 0);
    const bool show_list = (drone_list_visible_ && display_data_.drone_count > 0);

    const uint16_t spec_h = show_spec ? SPECTRUM_H : 0;
    if (spec_h <= remaining) remaining -= spec_h; else remaining = 0;

    const uint16_t hist_h = show_hist ? HISTOGRAM_H : 0;
    if (hist_h <= remaining) remaining -= hist_h; else remaining = 0;

    const uint16_t status_h = (remaining >= STATUS_H) ? STATUS_H : 0;
    if (status_h <= remaining) remaining -= status_h; else remaining = 0;

    const uint16_t drone_h = remaining;

    uint16_t y_offset = oy;

    if (show_spec) {
        render_spectrum(painter, spectrum_buffer_.data(), spectrum_data_size_,
                        ox, y_offset, w, spec_h);
        y_offset += spec_h;
    }

    if (show_hist) {
        render_histogram(painter, histogram_buffer_.data(), histogram_data_size_,
                         ox, y_offset, w, hist_h);
        y_offset += hist_h;
    }

    if (show_list && drone_h > 0) {
        render_drone_list(painter, display_data_.drones, display_data_.drone_count,
                          ox, y_offset, w, drone_h);
        y_offset += drone_h;
    }

    if (status_h > 0) {
        render_status_bar(painter, status_text_, ox, y_offset, w, status_h);
    }
}

// ============================================================================
// Render Methods
// ============================================================================

void DroneDisplay::render_spectrum(
    Painter& painter,
    const uint8_t* spectrum_data,
    size_t spectrum_size,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    // Validate input
    if (spectrum_data == nullptr || spectrum_size == 0) {
        return;
    }
    
    // Draw background
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    
    // Draw spectrum
    const uint16_t step_x = width / static_cast<uint16_t>(spectrum_size);
    for (size_t i = 0; i < spectrum_size; ++i) {
        const uint8_t value = spectrum_data[i];
        const uint16_t bar_height = (value * height) / HISTOGRAM_MAX_VALUE;
        const uint16_t x = start_x + static_cast<uint16_t>(i * step_x);
        const uint16_t y = start_y + height - bar_height;
        
        // Color based on signal strength
        uint32_t color = COLOR_LOW_THREAT;
        if (value > 200) {
            color = COLOR_CRITICAL_THREAT;
        } else if (value > 150) {
            color = COLOR_HIGH_THREAT;
        } else if (value > 100) {
            color = COLOR_MEDIUM_THREAT;
        }
        
        draw_spectrum_line(painter, x, y, color);
    }
}

void DroneDisplay::render_histogram(
    Painter& painter,
    const uint16_t* histogram_data,
    size_t histogram_size,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    // Validate input
    if (histogram_data == nullptr || histogram_size == 0 || width == 0) {
        return;
    }
    
    // Draw background
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    
    // Find max value for scaling
    uint16_t max_value = 0;
    for (size_t i = 0; i < histogram_size; ++i) {
        if (histogram_data[i] > max_value) {
            max_value = histogram_data[i];
        }
    }
    
    if (max_value == 0) {
        return;  // All zeros, nothing to draw
    }
    
    // Draw histogram bars
    const uint16_t bar_width = width / static_cast<uint16_t>(histogram_size);
    if (bar_width == 0) {
        return;  // Width too small for any bars
    }
    for (size_t i = 0; i < histogram_size; ++i) {
        const uint16_t value = histogram_data[i];
        const uint16_t bar_height = (value * height) / max_value;
        const uint16_t x = start_x + static_cast<uint16_t>(i * bar_width);
        const uint16_t y = start_y + height - bar_height;
        
        draw_histogram_bar(painter, x, y, bar_width, bar_height, COLOR_MEDIUM_THREAT);
    }
}

void DroneDisplay::render_drone_list(
    Painter& painter,
    const DisplayDroneEntry* drones,
    size_t drone_count,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    // Validate input
    if (drones == nullptr || drone_count == 0) {
        draw_text(painter, STATUS_NO_DRONES, start_x + 5, start_y + 10, COLOR_TEXT);
        return;
    }
    
    // Draw background
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    
    // Draw drone entries
    const uint16_t entry_height = height / static_cast<uint16_t>(drone_count);
    for (size_t i = 0; i < drone_count; ++i) {
        const uint16_t y = start_y + static_cast<uint16_t>(i * entry_height);
        draw_drone_entry(painter, drones[i], start_x, y, width, entry_height);
    }
}

void DroneDisplay::render_status_bar(
    Painter& painter,
    const char* status_text,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    // Validate input
    if (status_text == nullptr) {
        return;
    }
    
    // Draw background
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    
    // Draw status text
    draw_text(painter, status_text, start_x + 5, start_y + 5, COLOR_TEXT);
}

// ============================================================================
// Data Management
// ============================================================================

ErrorCode DroneDisplay::update_display_data(const DisplayData& display_data) noexcept {
    // Validate input
    const ErrorCode error = validate_drone_buffer(
        display_data.drones,
        display_data.drone_count,
        MAX_DISPLAYED_DRONES
    );
    if (error != ErrorCode::SUCCESS) {
        return error;
    }
    
    // Copy display data
    display_data_ = display_data;
    
    return ErrorCode::SUCCESS;
}

const DisplayData& DroneDisplay::get_display_data() const noexcept {
    return display_data_;
}

void DroneDisplay::clear_display(Painter& painter) noexcept {
    draw_rectangle(painter, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, COLOR_BACKGROUND);
}

ErrorCode DroneDisplay::set_spectrum_data(
    const uint8_t* spectrum_data,
    size_t spectrum_size
) noexcept {
    // Validate input
    const ErrorCode error = validate_spectrum_data(spectrum_data, spectrum_size);
    if (error != ErrorCode::SUCCESS) {
        return error;
    }
    
    // Copy spectrum data (clamp to buffer capacity)
    spectrum_data_size_ = (spectrum_size < spectrum_buffer_.size()) ? spectrum_size : spectrum_buffer_.size();
    for (size_t i = 0; i < spectrum_data_size_; ++i) {
        spectrum_buffer_[i] = spectrum_data[i];
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneDisplay::set_histogram_data(
    const uint16_t* histogram_data,
    size_t histogram_size
) noexcept {
    // Validate input
    const ErrorCode error = validate_histogram_data(histogram_data, histogram_size);
    if (error != ErrorCode::SUCCESS) {
        return error;
    }
    
    // Copy histogram data
    histogram_data_size_ = histogram_size;
    for (size_t i = 0; i < histogram_size && i < histogram_buffer_.size(); ++i) {
        histogram_buffer_[i] = histogram_data[i];
    }
    
    return ErrorCode::SUCCESS;
}

void DroneDisplay::set_status_text(const char* status_text) noexcept {
    if (status_text == nullptr) {
        return;
    }
    
    // Copy status text
    size_t i = 0;
    while (i < MAX_TEXT_LENGTH - 1 && status_text[i] != '\0') {
        status_text_[i] = status_text[i];
        ++i;
    }
    status_text_[i] = '\0';
}

const char* DroneDisplay::get_status_text() const noexcept {
    return status_text_;
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void DroneDisplay::draw_spectrum_line(
    Painter& painter,
    uint16_t x,
    uint16_t y,
    uint32_t color
) noexcept {
    draw_rectangle(painter, x, y, 1, 1, color);
}

void DroneDisplay::draw_histogram_bar(
    Painter& painter,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color
) noexcept {
    if (width == 0 || height == 0) {
        return;
    }
    draw_rectangle(painter, x, y, width, height, color);
}

void DroneDisplay::draw_drone_entry(
    Painter& painter,
    const DisplayDroneEntry& drone,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
) noexcept {
    // Draw entry background
    draw_rectangle(painter, x, y, width, height, COLOR_BACKGROUND, false);
    
    // Format frequency
    char freq_buffer[16];
    format_frequency(drone.frequency, freq_buffer, sizeof(freq_buffer));
    
    // Format RSSI
    char rssi_buffer[16];
    format_rssi(drone.rssi, rssi_buffer, sizeof(rssi_buffer));
    
    // Format threat level string
    const char* threat_str = "";
    switch (drone.threat) {
        case ThreatLevel::CRITICAL:
            threat_str = "CRITICAL";
            break;
        case ThreatLevel::HIGH:
            threat_str = "HIGH";
            break;
        case ThreatLevel::MEDIUM:
            threat_str = "MEDIUM";
            break;
        case ThreatLevel::LOW:
            threat_str = "LOW";
            break;
        case ThreatLevel::NONE:
        default:
            threat_str = "NONE";
            break;
    }
    
    // Get movement trend symbol
    char trend_symbol = MOVEMENT_TREND_SYMBOL_UNKNOWN;
    switch (drone.trend) {
        case MovementTrend::APPROACHING:
            trend_symbol = MOVEMENT_TREND_SYMBOL_APPROACHING;
            break;
        case MovementTrend::RECEDING:
            trend_symbol = MOVEMENT_TREND_SYMBOL_RECEEDING;
            break;
        case MovementTrend::STATIC:
            trend_symbol = MOVEMENT_TREND_SYMBOL_STATIC;
            break;
        default:
            trend_symbol = MOVEMENT_TREND_SYMBOL_UNKNOWN;
            break;
    }
    
    // Draw drone info
    // Line 1: Type | Threat | Trend
    draw_text(painter, drone.get_type_name(), x + 5, y + 2, drone.display_color);
    draw_text(painter, threat_str, x + 65, y + 2, drone.display_color);
    
    // Draw trend symbol - FIXED: moved to avoid overlap
    char trend_buffer[2] = {trend_symbol, '\0'};
    draw_text(painter, trend_buffer, x + 130, y + 2, drone.display_color);
    
    // Line 2: Frequency | RSSI
    draw_text(painter, freq_buffer, x + 5, y + 12, COLOR_TEXT);
    draw_text(painter, rssi_buffer, x + 100, y + 12, COLOR_TEXT);
}

void DroneDisplay::draw_text(
    Painter& painter,
    const char* text,
    uint16_t x,
    uint16_t y,
    uint32_t color
) noexcept {
    if (text == nullptr) {
        return;
    }

    const Color fg_color = Color::RGB(color);
    const Color bg_color = Color::black();

    painter.draw_string(
        Point{x, y},
        Theme::getInstance()->fg_light->font,
        fg_color,
        bg_color,
        std::string_view(text)
    );
}

void DroneDisplay::draw_text(
    Painter& painter,
    std::string_view text,
    uint16_t x,
    uint16_t y,
    uint32_t color
) noexcept {
    if (text.empty()) {
        return;
    }

    const Color fg_color = Color::RGB(color);
    const Color bg_color = Color::black();

    painter.draw_string(
        Point{x, y},
        Theme::getInstance()->fg_light->font,
        fg_color,
        bg_color,
        text
    );
}

void DroneDisplay::draw_rectangle(
    Painter& painter,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color,
    bool fill
) noexcept {
    const Color rect_color = Color::RGB(color);
    const Rect rect{x, y, width, height};

    if (fill) {
        painter.fill_rectangle(rect, rect_color);
    } else {
        painter.draw_rectangle(rect, rect_color);
    }
}

// ============================================================================
// Utility Methods
// ============================================================================

uint16_t DroneDisplay::map_rssi_to_height(
    RssiValue rssi,
    uint16_t max_height
) const noexcept {
    const int32_t rssi_range = RSSI_MAX_DBM - RSSI_MIN_DBM;
    const int32_t rssi_clamped = clamp(rssi, RSSI_MIN_DBM, RSSI_MAX_DBM);
    const int32_t rssi_normalized = rssi_clamped - RSSI_MIN_DBM;
    return static_cast<uint16_t>((rssi_normalized * max_height) / rssi_range);
}

uint32_t DroneDisplay::get_threat_color(ThreatLevel threat) const noexcept {
    switch (threat) {
        case ThreatLevel::LOW:
            return COLOR_LOW_THREAT;
        case ThreatLevel::MEDIUM:
            return COLOR_MEDIUM_THREAT;
        case ThreatLevel::HIGH:
            return COLOR_HIGH_THREAT;
        case ThreatLevel::CRITICAL:
            return COLOR_CRITICAL_THREAT;
        default:
            return COLOR_UNKNOWN_THREAT;
    }
}

static void write_uint(char*& buf, size_t& remaining, uint32_t value) noexcept {
    char tmp[10];
    int len = 0;
    if (value == 0) {
        tmp[len++] = '0';
    } else {
        while (value > 0 && len < 10) {
            tmp[len++] = '0' + static_cast<char>(value % 10);
            value /= 10;
        }
    }
    for (int i = len - 1; i >= 0 && remaining > 1; --i) {
        *buf++ = tmp[i];
        --remaining;
    }
}

static void write_uint_pad(char*& buf, size_t& remaining, uint32_t value, int pad) noexcept {
    char tmp[10];
    int len = 0;
    while (len < pad) {
        tmp[len++] = '0' + static_cast<char>(value % 10);
        value /= 10;
    }
    for (int i = len - 1; i >= 0 && remaining > 1; --i) {
        *buf++ = tmp[i];
        --remaining;
    }
}

static void write_str(char*& buf, size_t& remaining, const char* s) noexcept {
    while (*s != '\0' && remaining > 1) {
        *buf++ = *s++;
        --remaining;
    }
}

void DroneDisplay::format_frequency(
    FreqHz frequency,
    char* buffer,
    size_t buffer_size
) const noexcept {
    if (buffer == nullptr || buffer_size < 16) {
        return;
    }

    const uint32_t mhz = static_cast<uint32_t>(frequency / 1'000'000ULL);
    const uint32_t khz = static_cast<uint32_t>((frequency % 1'000'000ULL) / 1'000ULL);

    char* buf = buffer;
    size_t remaining = buffer_size;
    write_uint(buf, remaining, mhz);
    if (remaining > 1) { *buf++ = '.'; --remaining; }
    write_uint_pad(buf, remaining, khz, 3);
    write_str(buf, remaining, " MHz");
    *buf = '\0';
}

void DroneDisplay::format_rssi(
    RssiValue rssi,
    char* buffer,
    size_t buffer_size
) const noexcept {
    if (buffer == nullptr || buffer_size < 8) {
        return;
    }

    char* buf = buffer;
    size_t remaining = buffer_size;

    if (rssi < 0) {
        if (remaining > 1) { *buf++ = '-'; --remaining; }
        rssi = -rssi;
    }

    write_uint(buf, remaining, static_cast<uint32_t>(rssi));
    write_str(buf, remaining, " db");
    *buf = '\0';
}

ErrorCode DroneDisplay::validate_spectrum_data(
    const uint8_t* spectrum_data,
    size_t spectrum_size
) const noexcept {
    return validate_spectrum_buffer(spectrum_data, spectrum_size);
}

ErrorCode DroneDisplay::validate_histogram_data(
    const uint16_t* histogram_data,
    size_t histogram_size
) const noexcept {
    return validate_histogram_buffer(histogram_data, histogram_size);
}

uint16_t DroneDisplay::clamp(
    int32_t value,
    int32_t min,
    int32_t max
) const noexcept {
    if (value < min) {
        return static_cast<uint16_t>(min);
    }
    if (value > max) {
        return static_cast<uint16_t>(max);
    }
    return static_cast<uint16_t>(value);
}

} // namespace drone_analyzer
