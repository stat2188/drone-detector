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
    const bool show_composite = (composite_mode_ && composite_data_ != nullptr && composite_data_size_ > 0);
    const bool show_hist = (histogram_visible_ && histogram_data_size_ > 0);
    const bool show_list = (drone_list_visible_ && display_data_.drone_count > 0);

    const uint16_t spec_h = (show_spec || show_composite) ? SPECTRUM_H : 0;
    if (spec_h <= remaining) remaining -= spec_h; else remaining = 0;

    const uint16_t hist_h = show_hist ? HISTOGRAM_H : 0;
    if (hist_h <= remaining) remaining -= hist_h; else remaining = 0;

    const uint16_t status_h = (remaining >= STATUS_H) ? STATUS_H : 0;
    if (status_h <= remaining) remaining -= status_h; else remaining = 0;

    const uint16_t drone_h = remaining;

    uint16_t y_offset = oy;

    if (show_spec || show_composite) {
        if (show_composite) {
            render_composite(painter, composite_data_, composite_data_size_,
                            ox, y_offset, w, spec_h);
        } else {
            render_spectrum(painter, spectrum_buffer_.data(), spectrum_data_size_,
                            ox, y_offset, w, spec_h);
        }
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
    if (spectrum_data == nullptr || spectrum_size == 0 || height < 4) {
        return;
    }

    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);
    draw_text(painter, "SPECTRUM", start_x + 2, start_y + 2, COLOR_TEXT);

    constexpr uint16_t MIN_BAR_WIDTH = 2;
    const uint16_t usable_width = width - 4;
    const uint16_t bar_count = static_cast<uint16_t>(spectrum_size);
    uint16_t bar_width = usable_width / bar_count;
    if (bar_width < MIN_BAR_WIDTH) bar_width = MIN_BAR_WIDTH;

    const uint16_t chart_start_x = start_x + 2;
    const uint16_t chart_start_y = start_y + 12;
    const uint16_t chart_height = height - 14;
    if (chart_height < 4) return;

    for (size_t i = 0; i < spectrum_size; ++i) {
        // Skip DC spike bins
        if (i >= 120 && i < 136) continue;

        const uint8_t value = spectrum_data[i];
        if (value < min_color_power_) continue;

        const uint16_t bar_height = (static_cast<uint16_t>(value) * chart_height) / 255;
        const uint16_t x = chart_start_x + static_cast<uint16_t>(i) * bar_width;
        const uint16_t y = chart_start_y + chart_height - bar_height;

        uint32_t color = COLOR_LOW_THREAT;
        if (value > 200) color = COLOR_CRITICAL_THREAT;
        else if (value > 150) color = COLOR_HIGH_THREAT;
        else if (value > 100) color = COLOR_MEDIUM_THREAT;

        if (bar_height > 0) {
            draw_rectangle(painter, x, y, bar_width - 1, bar_height, color);
        }
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
    if (histogram_data == nullptr || histogram_size == 0 || width < 10 || height < 4) {
        return;
    }
    
    // Draw background with border
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);
    draw_text(painter, "HISTOGRAM", start_x + 2, start_y + 2, COLOR_TEXT);

    // Subsample to fit screen (128 bins → max 60 bars)
    constexpr size_t MAX_BARS = 60;
    const size_t step = (histogram_size > MAX_BARS) ? (histogram_size / MAX_BARS) : 1;
    const size_t visible_count = histogram_size / step;

    // Find max value for scaling (include all visible bins)
    uint16_t max_value = 0;
    for (size_t i = 0; i < histogram_size; i += step) {
        if (histogram_data[i] > max_value) {
            max_value = histogram_data[i];
        }
    }

    if (max_value == 0) {
        draw_text(painter, "No data", start_x + 2, start_y + 12, COLOR_UNKNOWN_THREAT);
        return;
    }

    constexpr uint16_t MIN_BAR_WIDTH = 2;
    const uint16_t usable_width = width - 4;
    uint16_t bar_width = usable_width / static_cast<uint16_t>(visible_count);
    if (bar_width < MIN_BAR_WIDTH) bar_width = MIN_BAR_WIDTH;

    const uint16_t chart_start_x = start_x + 2;
    const uint16_t chart_start_y = start_y + 12;
    const uint16_t chart_height = height - 14;
    if (chart_height < 4) return;

    for (size_t bar = 0; bar < visible_count; ++bar) {
        const size_t idx = bar * step;
        const uint16_t value = histogram_data[idx];
        const uint16_t bar_height = (value * chart_height) / max_value;
        const uint16_t x = chart_start_x + static_cast<uint16_t>(bar) * bar_width;
        const uint16_t y = chart_start_y + chart_height - bar_height;

        if (bar_height > 0) {
            draw_rectangle(painter, x, y, bar_width - 1, bar_height, COLOR_MEDIUM_THREAT);
        }
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
        draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
        draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);
        draw_text(painter, "DETECTED DRONES", start_x + 2, start_y + 2, COLOR_TEXT);
        draw_text(painter, STATUS_NO_DRONES, start_x + 2, start_y + 14, COLOR_UNKNOWN_THREAT);
        return;
    }
    
    // Draw background with border
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);  // Top border
    
    // Draw header
    draw_text(painter, "DETECTED DRONES", start_x + 2, start_y + 2, COLOR_TEXT);
    
    // Draw column headers
    constexpr uint16_t HEADER_H = 12;
    constexpr uint16_t ENTRY_MIN_H = 22;  // Minimum height per entry
    const uint16_t list_start_y = start_y + HEADER_H;
    const uint16_t available_h = height - HEADER_H;
    
    // Calculate entry height (max 4 entries visible)
    uint16_t entry_height = available_h / static_cast<uint16_t>(drone_count);
    if (entry_height > 40) entry_height = 40;
    if (entry_height < ENTRY_MIN_H) entry_height = ENTRY_MIN_H;
    
    // Draw drone entries
    for (size_t i = 0; i < drone_count; ++i) {
        const uint16_t y = list_start_y + static_cast<uint16_t>(i) * entry_height;
        if (y + entry_height > start_y + height) break;  // Don't overflow
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
    if (status_text == nullptr || height < 4) {
        return;
    }
    
    // Draw background with top border
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);
    
    // Draw status text centered vertically
    const uint16_t text_y = start_y + (height > 10 ? 4 : 1);
    draw_text(painter, status_text, start_x + 4, text_y, COLOR_TEXT);
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
    const ErrorCode error = validate_spectrum_data(spectrum_data, spectrum_size);
    if (error != ErrorCode::SUCCESS) {
        return error;
    }

    const size_t count = (spectrum_size < spectrum_buffer_.size()) ? spectrum_size : spectrum_buffer_.size();
    spectrum_data_size_ = count;
    for (size_t i = 0; i < count; ++i) {
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
    // Draw entry separator line
    draw_rectangle(painter, x, y + height - 1, width, 1, COLOR_UNKNOWN_THREAT);
    
    // Format frequency
    char freq_buffer[16];
    format_frequency(drone.frequency, freq_buffer, sizeof(freq_buffer));
    
    // Format RSSI
    char rssi_buffer[16];
    format_rssi(drone.rssi, rssi_buffer, sizeof(rssi_buffer));
    
    // Format threat level string
    const char* threat_str = "";
    switch (drone.threat) {
        case ThreatLevel::CRITICAL: threat_str = "CRIT"; break;
        case ThreatLevel::HIGH:     threat_str = "HIGH"; break;
        case ThreatLevel::MEDIUM:   threat_str = "MED";  break;
        case ThreatLevel::LOW:      threat_str = "LOW";  break;
        default:                    threat_str = "---";  break;
    }
    
    // Get movement trend symbol
    char trend_symbol = MOVEMENT_TREND_SYMBOL_UNKNOWN;
    switch (drone.trend) {
        case MovementTrend::APPROACHING: trend_symbol = '<'; break;  // Approaching
        case MovementTrend::RECEDING:    trend_symbol = '>'; break;  // Receding
        case MovementTrend::STATIC:      trend_symbol = '~'; break;  // Static
        default:                         trend_symbol = '-'; break;
    }
    
    // Layout: Use proportional columns
    // Col 1 (0-40%): Type name (colored)
    // Col 2 (40-60%): Threat level
    // Col 3 (60-75%): Frequency
    // Col 4 (75-90%): RSSI
    // Col 5 (90-100%): Trend
    
    constexpr uint16_t PAD = 3;
    const uint16_t col1_end = (width * 40) / 100;
    const uint16_t col3_end = (width * 75) / 100;
    const uint16_t col4_end = (width * 90) / 100;
    
    // Line 1: Type | Threat
    draw_text(painter, drone.get_type_name(), x + PAD, y + 2, drone.display_color);
    draw_text(painter, threat_str, x + col1_end + PAD, y + 2, drone.display_color);
    
    // Line 2: Frequency | RSSI | Trend
    draw_text(painter, freq_buffer, x + PAD, y + 12, COLOR_TEXT);
    draw_text(painter, rssi_buffer, x + col3_end + PAD, y + 12, COLOR_TEXT);
    
    // Trend symbol at far right
    char trend_buffer[2] = {trend_symbol, '\0'};
    draw_text(painter, trend_buffer, x + col4_end + PAD, y + 12, COLOR_TEXT);
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

void DroneDisplay::set_composite_data(const uint8_t* data, size_t size) noexcept {
    composite_data_ = data;
    composite_data_size_ = size;
}

void DroneDisplay::render_composite(
    Painter& painter,
    const uint8_t* composite_data,
    size_t composite_size,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    if (composite_data == nullptr || composite_size == 0 || height < 4) {
        return;
    }

    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);
    draw_text(painter, "BAND SWEEP", start_x + 2, start_y + 2, COLOR_TEXT);

    constexpr uint16_t MIN_BAR_WIDTH = 2;
    const uint16_t usable_width = width - 4;
    const uint16_t bar_count = static_cast<uint16_t>(composite_size);
    uint16_t bar_width = usable_width / bar_count;
    if (bar_width < MIN_BAR_WIDTH) bar_width = MIN_BAR_WIDTH;

    const uint16_t chart_start_x = start_x + 2;
    const uint16_t chart_start_y = start_y + 12;
    const uint16_t chart_height = height - 14;
    if (chart_height < 4) return;

    for (uint16_t i = 0; i < bar_count; ++i) {
        const uint8_t power = composite_data[i];
        if (power == 0) continue;
        if (power < min_color_power_) continue;

        const uint16_t bar_height = (static_cast<uint16_t>(power) * chart_height) / 255;
        if (bar_height == 0) continue;

        const uint16_t x = chart_start_x + i * bar_width;
        const uint16_t y = chart_start_y + chart_height - bar_height;

        uint32_t color = COLOR_LOW_THREAT;
        if (power > 200) color = COLOR_CRITICAL_THREAT;
        else if (power > 150) color = COLOR_HIGH_THREAT;
        else if (power > 100) color = COLOR_MEDIUM_THREAT;

        draw_rectangle(painter, x, y, bar_width - 1, bar_height, color);
    }
}

} // namespace drone_analyzer
