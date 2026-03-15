#include "drone_display.hpp"
#include "ui.hpp"  // Will provide ui::View, Painter, etc.

namespace drone_analyzer {

// ============================================================================
// Constructor / Destructor
// ============================================================================

DroneDisplay::DroneDisplay() noexcept
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
    , status_bar_visible_(true)
    , spectrum_height_(80)
    , histogram_height_(60)
    , drone_list_height_(150)
    , status_bar_height_(20) {
    // Initialize status text
    set_status_text(STATUS_READY);
}

DroneDisplay::~DroneDisplay() noexcept {
    // Destructor - no dynamic memory to free
}

// ============================================================================
// Paint Method
// ============================================================================

void DroneDisplay::paint(Painter& painter) {
    // Use static storage for large buffers to reduce stack usage
    static std::array<uint8_t, SPECTRUM_BUFFER_SIZE> local_spectrum;
    static std::array<uint16_t, HISTOGRAM_BUFFER_SIZE> local_histogram;
    
    // Clear display
    clear_display(painter);
    
    uint16_t y_offset = 0;
    
    // Render spectrum if visible
    if (spectrum_visible_ && spectrum_data_size_ > 0) {
        render_spectrum(
            painter,
            spectrum_buffer_.data(),
            spectrum_data_size_,
            0,
            y_offset,
            DISPLAY_WIDTH,
            spectrum_height_
        );
        y_offset += spectrum_height_;
    }
    
    // Render histogram if visible
    if (histogram_visible_ && histogram_data_size_ > 0) {
        render_histogram(
            painter,
            histogram_buffer_.data(),
            histogram_data_size_,
            0,
            y_offset,
            DISPLAY_WIDTH,
            histogram_height_
        );
        y_offset += histogram_height_;
    }
    
    // Render drone list if visible
    if (drone_list_visible_ && display_data_.drone_count > 0) {
        render_drone_list(
            painter,
            display_data_.drones,
            display_data_.drone_count,
            0,
            y_offset,
            DISPLAY_WIDTH,
            drone_list_height_
        );
        y_offset += drone_list_height_;
    }
    
    // Render status bar if visible
    if (status_bar_visible_) {
        render_status_bar(
            painter,
            status_text_,
            0,
            y_offset,
            DISPLAY_WIDTH,
            status_bar_height_
        );
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
        const uint16_t bar_height = (value * height) / 255;
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
    if (histogram_data == nullptr || histogram_size == 0) {
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
    
    // Draw histogram bars
    const uint16_t bar_width = width / static_cast<uint16_t>(histogram_size);
    for (size_t i = 0; i < histogram_size; ++i) {
        const uint16_t value = histogram_data[i];
        uint16_t bar_height = 0;
        if (max_value > 0) {
            bar_height = (value * height) / max_value;
        }
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
    
    // Copy spectrum data
    spectrum_data_size_ = spectrum_size;
    for (size_t i = 0; i < spectrum_size && i < spectrum_buffer_.size(); ++i) {
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
    while (i < status_text_.size() - 1 && status_text[i] != '\0') {
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
    
    // Draw drone info
    draw_text(painter, drone.get_type_name(), x + 5, y + 2, drone.display_color);
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
    // Will use ui::Painter::draw_string when available
    (void)painter;
    (void)x;
    (void)y;
    (void)color;
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
    // Will use ui::Painter::draw_rectangle when available
    (void)painter;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)color;
    (void)fill;
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

void DroneDisplay::format_frequency(
    FreqHz frequency,
    char* buffer,
    size_t buffer_size
) const noexcept {
    if (buffer == nullptr || buffer_size < 16) {
        return;
    }
    
    // Format as MHz with 3 decimal places
    const uint32_t freq_mhz = static_cast<uint32_t>(frequency / 1'000'000);
    const uint32_t freq_khz = static_cast<uint32_t>((frequency % 1'000'000) / 1'000);
    
    // Simple formatting (will use proper function when available)
    buffer[0] = '0' + static_cast<char>((freq_mhz / 1000) % 10);
    buffer[1] = '0' + static_cast<char>((freq_mhz / 100) % 10);
    buffer[2] = '0' + static_cast<char>((freq_mhz / 10) % 10);
    buffer[3] = '0' + static_cast<char>(freq_mhz % 10);
    buffer[4] = '.';
    buffer[5] = '0' + static_cast<char>((freq_khz / 100) % 10);
    buffer[6] = '0' + static_cast<char>((freq_khz / 10) % 10);
    buffer[7] = '0' + static_cast<char>(freq_khz % 10);
    buffer[8] = ' ';
    buffer[9] = 'M';
    buffer[10] = 'H';
    buffer[11] = 'z';
    buffer[12] = '\0';
}

void DroneDisplay::format_rssi(
    RssiValue rssi,
    char* buffer,
    size_t buffer_size
) const noexcept {
    if (buffer == nullptr || buffer_size < 8) {
        return;
    }
    
    // Simple formatting (will use proper function when available)
    buffer[0] = '-';
    if (rssi < -100) {
        buffer[1] = '1';
        buffer[2] = '0';
        buffer[3] = '0';
        buffer[4] = '+';
    } else if (rssi < -10) {
        buffer[1] = '0' + static_cast<char>((-rssi / 10) % 10);
        buffer[2] = '0' + static_cast<char>(-rssi % 10);
        buffer[3] = ' ';
    } else {
        buffer[1] = '0' + static_cast<char>(-rssi % 10);
        buffer[2] = ' ';
    }
    buffer[5] = 'd';
    buffer[6] = 'B';
    buffer[7] = 'm';
    buffer[8] = '\0';
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
