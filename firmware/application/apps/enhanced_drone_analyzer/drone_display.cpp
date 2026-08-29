#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string_view>

#include "drone_display.hpp"

namespace drone_analyzer {

// ============================================================================
// Drone list layout constants (single source for render, draw, and hit-test)
// ============================================================================
namespace {

constexpr uint16_t LIST_PAD = 3;                 // horizontal padding around text
constexpr uint16_t THREAT_SHIFT_CHARS = 3;       // threat nudged left of Col 1's right edge
constexpr uint16_t GAP_CHARS = 1;                // min gap between type text and threat level
constexpr uint16_t TYPE_MAX_CHARS = 7;           // type name truncated to this for layout
constexpr uint16_t THREAT_MAX_CHARS = 4;         // longest threat label ("CRIT")
constexpr uint16_t ENTRY_MIN_H = 26;             // min row height — still fits 5 entries
constexpr uint16_t ENTRY_MAX_H = 40;             // cap so tall rows don't waste space
constexpr size_t DUAL_COLUMN_MIN_COUNT = 6;      // split into 2 columns when >= 6 detections

struct EntryLayout {
    uint16_t col_w;   // cell width (list width when single column)
    uint16_t rows;    // row count (drone_count when single column)
    uint16_t entry_h; // height per row
};

EntryLayout compute_entry_layout(
    size_t drone_count,
    uint16_t width,
    uint16_t height,
    bool dual_column
) noexcept {
    EntryLayout layout;
    if (dual_column && drone_count >= DUAL_COLUMN_MIN_COUNT) {
        layout.rows = static_cast<uint16_t>((drone_count + 1) / 2);
        layout.col_w = width / 2;
    } else {
        layout.rows = static_cast<uint16_t>(drone_count);
        layout.col_w = width;
    }

    uint16_t entry_h = (layout.rows > 0) ? height / layout.rows : height;
    if (entry_h > ENTRY_MAX_H) entry_h = ENTRY_MAX_H;
    if (entry_h < ENTRY_MIN_H) entry_h = ENTRY_MIN_H;
    layout.entry_h = entry_h;
    return layout;
}

} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

DroneDisplay::DroneDisplay(const Rect parent_rect) noexcept
    : ui::View()
    , display_data_()
    , spectrum_buffer_{}
    , timeline_{}
    , status_text_{0}
    , spectrum_data_size_(0)
    , spectrum_visible_(true)
    , timeline_visible_(true)
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

DroneDisplay::LayoutMetrics DroneDisplay::calculate_layout() const noexcept {
    constexpr uint16_t SPECTRUM_H = 50;
    constexpr uint16_t TIMELINE_H = 24;

    const uint16_t total_h = parent_rect().size().height();

    uint16_t remaining = total_h;

    const bool show_spec = (spectrum_visible_ && spectrum_data_size_ > 0) ||
                           (composite_mode_ && composite_data_ != nullptr && composite_data_size_ > 0);
    const bool show_tl = timeline_visible_;

    const uint16_t spec_h = show_spec ? SPECTRUM_H : 0;
    if (spec_h <= remaining) remaining -= spec_h; else remaining = 0;

    const uint16_t tl_h = show_tl ? TIMELINE_H : 0;
    if (tl_h <= remaining) remaining -= tl_h; else remaining = 0;

    // No status bar reserved — the drone list stretches to the window's bottom edge.
    const uint16_t status_h = 0;

    const uint16_t drone_h = remaining;

    uint16_t list_start_y = 0;
    if (show_spec) list_start_y += spec_h;
    if (show_tl) list_start_y += tl_h;

    return LayoutMetrics{spec_h, tl_h, status_h, drone_h, list_start_y};
}

void DroneDisplay::paint(Painter& painter) {
    const auto sr = screen_rect();
    const uint16_t ox = sr.location().x();
    const uint16_t oy = sr.location().y();
    const uint16_t w = sr.size().width();

    const auto layout = calculate_layout();
    uint16_t y_offset = oy;

    const bool show_spec = (spectrum_visible_ && spectrum_data_size_ > 0) ||
                           (composite_mode_ && composite_data_ != nullptr && composite_data_size_ > 0);
    const bool show_tl = timeline_visible_;
    const bool show_list = (drone_list_visible_ && display_data_.drone_count > 0);

    if (show_spec && (dirty_flags_ & DIRTY_SPEC)) {
        if (composite_mode_ && composite_data_ != nullptr && composite_data_size_ > 0) {
            if (dual_sweep_mode_ && sweep2_data_ != nullptr && sweep2_data_size_ > 0) {
                render_dual_composite(painter, ox, y_offset, w, layout.spec_h);
            } else if (multi_zone_count_ > 1) {
                render_multi_zone(painter, ox, y_offset, w, layout.spec_h);
            } else {
                render_composite(painter, composite_data_, composite_data_size_,
                                ox, y_offset, w, layout.spec_h, scan_head_position_[0],
                                composite_noise_floor_valid_ ? composite_noise_floor_ : 0);
            }
        } else {
            render_spectrum(painter, spectrum_buffer_.data(), spectrum_data_size_,
                            ox, y_offset, w, layout.spec_h);
        }
    }
    if (show_spec) y_offset += layout.spec_h;

    if (show_tl && (dirty_flags_ & DIRTY_WATERFALL) && timeline_.count() > 0) {
        render_waterfall(painter, timeline_,
                         ox, y_offset, w, layout.timeline_h);
    }
    if (show_tl) y_offset += layout.timeline_h;

    if (show_list && layout.drone_h > 0 && (dirty_flags_ & DIRTY_DRONES)) {
        render_drone_list(painter, display_data_.drones, display_data_.drone_count,
                          ox, y_offset, w, layout.drone_h);
    }
    if (show_list && layout.drone_h > 0) y_offset += layout.drone_h;

    if (layout.status_h > 0 && (dirty_flags_ & DIRTY_STATUS)) {
        render_status_bar(painter, status_text_, ox, y_offset, w, layout.status_h);
    }

    dirty_flags_ = 0;  // All sections painted — clear flags
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
    draw_text(painter, "SPECTRUM +/-1MHz", start_x + 2, start_y + 2, COLOR_TEXT);

    constexpr uint16_t MIN_BAR_WIDTH = 2;
    const uint16_t usable_width = width - 4;
    const uint16_t bar_count = static_cast<uint16_t>(spectrum_size);
    uint16_t bar_width = usable_width / bar_count;
    if (bar_width < MIN_BAR_WIDTH) bar_width = MIN_BAR_WIDTH;

    const uint16_t chart_start_x = start_x + 2;
    const uint16_t chart_start_y = start_y + 12;
    const uint16_t chart_height = height - 14;
    if (chart_height < 4) return;

    // Noise floor and margin filtering are pre-computed in set_spectrum_data()
    // Here we only draw — pure paint, no DSP
    for (size_t i = 0; i < spectrum_size; ++i) {
        if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;

        const uint8_t value = spectrum_data[i];
        if (value < min_color_power_) continue;

        const uint16_t bar_height = (static_cast<uint16_t>(value) * chart_height) >> 8;
        const uint16_t x = chart_start_x + static_cast<uint16_t>(i) * bar_width;
        const uint16_t y = chart_start_y + chart_height - bar_height;

        uint32_t color = COLOR_LOW_THREAT;
        if (value > 200) color = COLOR_CRITICAL_THREAT;
        else if (value > 150) color = COLOR_HIGH_THREAT;
        else if (value > 100) color = COLOR_MEDIUM_THREAT;

        if (bar_height > 0) {
            draw_rectangle(painter, x, y, bar_width, bar_height, color);
        }
    }
}

void DroneDisplay::render_waterfall(
    Painter& painter,
    const MiniWaterfall& waterfall,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    if (width < 10 || height < 4) return;

    // Background + label
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);
    draw_text(painter, "WF", start_x + 2, start_y + 2, COLOR_TEXT);

    if (waterfall.count() == 0) {
        draw_text(painter, "Waiting...", start_x + 2, start_y + 12, COLOR_UNKNOWN_THREAT);
        return;
    }

    constexpr uint16_t LABEL_H = 12;
    const uint16_t chart_start_x = start_x + 2;
    const uint16_t chart_start_y = start_y + LABEL_H;
    const uint16_t chart_h = (height > LABEL_H + 2) ? (height - LABEL_H - 2) : 4;
    const uint16_t chart_w = (width > 4) ? (width - 4) : width;

    const uint16_t col_count = waterfall.count();
    const uint8_t win_count = waterfall.get_active_windows();

    // Single-window rendering (fast path)
    if (win_count <= 1) {
        for (uint16_t col = 0; col < col_count; ++col) {
            const uint16_t px = chart_start_x
                + static_cast<uint16_t>((static_cast<uint32_t>(col) * chart_w) / col_count);
            const uint16_t bar_w = chart_w / col_count;
            if (bar_w == 0) continue;

            for (uint8_t row = 0; row < MiniWaterfall::HEIGHT; ++row) {
                const uint8_t pixel = waterfall.get_pixel(col, row);
                if (pixel == 0) continue;

                const uint32_t color = MiniWaterfall::PALETTE[pixel];
                const uint16_t py = chart_start_y + chart_h - 1 - row;
                draw_rectangle(painter, px, py, bar_w, 1, color);
            }
        }
        return;
    }

    // Multi-window rendering: divide chart width among windows.
    // push_multi_window() interleaves columns: [w0, w1, w2, w3, w0, w1, ...]
    // Each window section renders only its own columns (every win_count-th).
    const uint16_t win_w = chart_w / win_count;
    for (uint8_t w = 0; w < win_count; ++w) {
        const uint16_t wx = chart_start_x + w * win_w;

        // Separator between windows
        if (w > 0) {
            draw_rectangle(painter, wx - 1, chart_start_y, 1, chart_h, COLOR_UNKNOWN_THREAT);
        }

        // Count columns belonging to this window
        const uint16_t win_cols = col_count / win_count;
        if (win_cols == 0) continue;

        for (uint16_t i = 0; i < win_cols; ++i) {
            const uint16_t col = i * win_count + w;
            if (col >= col_count) break;

            const uint16_t px = wx
                + static_cast<uint16_t>((static_cast<uint32_t>(i) * (win_w - 1)) / win_cols);

            for (uint8_t row = 0; row < MiniWaterfall::HEIGHT; ++row) {
                const uint8_t pixel = waterfall.get_pixel(col, row);
                if (pixel == 0) continue;

                const uint32_t color = MiniWaterfall::PALETTE[pixel];
                const uint16_t py = chart_start_y + chart_h - 1 - row;
                draw_rectangle(painter, px, py, 1, 1, color);
            }
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
        draw_text(painter, STATUS_NO_DRONES, start_x + 2, start_y + 4, COLOR_UNKNOWN_THREAT);
        return;
    }
    
    // Draw background with border
    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);  // Top border
    
    // No header row — the drone entry list fills the full section height.
    // Dual column kicks in when enabled and >= DUAL_COLUMN_MIN_COUNT detections.
    const uint16_t list_start_y = start_y;
    const EntryLayout layout = compute_entry_layout(drone_count, width, height, dual_column_mode_);

    if (layout.col_w < width) {
        // Dual column: left cell = row index, right cell = row + rows.
        for (uint16_t row = 0; row < layout.rows; ++row) {
            const uint16_t y = list_start_y + row * layout.entry_h;
            if (y + layout.entry_h > start_y + height) break;  // Don't overflow
            draw_drone_entry(painter, drones[row], start_x, y, layout.col_w, layout.entry_h);
            const size_t right_idx = static_cast<size_t>(row) + layout.rows;
            if (right_idx < drone_count) {
                draw_drone_entry(painter, drones[right_idx], start_x + layout.col_w, y,
                                 layout.col_w, layout.entry_h);
            }
        }
    } else {
        // Single column (default)
        for (size_t i = 0; i < drone_count; ++i) {
            const uint16_t y = list_start_y + static_cast<uint16_t>(i) * layout.entry_h;
            if (y + layout.entry_h > start_y + height) break;  // Don't overflow
            draw_drone_entry(painter, drones[i], start_x, y, width, layout.entry_h);
        }
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
    const ErrorCode error = validate_drone_buffer(
        display_data.drones,
        display_data.drone_count,
        MAX_DISPLAYED_DRONES
    );
    if (error != ErrorCode::SUCCESS) {
        return error;
    }
    display_data_ = display_data;
    dirty_flags_ |= DIRTY_DRONES;
    set_dirty();
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

    // Pre-filter: apply display margin filter during copy (not during paint)
    // This moves noise floor computation out of the hot paint() path
    // display_margin_ is separate from spectrum_shape_margin_ (detection margin)
    // Default: 0 = show full spectrum (no filtering)
    if (display_margin_ > 0) {
        // Quickselect median — O(n) vs O(n²) insertion sort
        // Use class member buffer to avoid stack allocation (was: uint8_t sorted[240])
        size_t sort_count = 0;
        for (size_t i = 0; i < count && sort_count < spectrum_sort_buffer_.size(); ++i) {
            if (i >= FFT_DC_SPIKE_START && i < FFT_DC_SPIKE_END) continue;
            spectrum_sort_buffer_[sort_count++] = spectrum_data[i];
        }
        if (sort_count > 0) {
            const size_t k = sort_count / 2;
            size_t qs_left = 0;
            size_t qs_right = sort_count - 1;
            while (qs_left < qs_right) {
                const size_t pivot_idx = qs_left + (qs_right - qs_left) / 2;
                const uint8_t pivot = spectrum_sort_buffer_[pivot_idx];
                spectrum_sort_buffer_[pivot_idx] = spectrum_sort_buffer_[qs_right];
                spectrum_sort_buffer_[qs_right] = pivot;
                size_t store = qs_left;
                for (size_t i = qs_left; i < qs_right; ++i) {
                    if (spectrum_sort_buffer_[i] < pivot) {
                        const uint8_t t = spectrum_sort_buffer_[store];
                        spectrum_sort_buffer_[store] = spectrum_sort_buffer_[i];
                        spectrum_sort_buffer_[i] = t;
                        ++store;
                    }
                }
                {
                    const uint8_t t = spectrum_sort_buffer_[store];
                    spectrum_sort_buffer_[store] = spectrum_sort_buffer_[qs_right];
                    spectrum_sort_buffer_[qs_right] = t;
                }
                if (store == k) break;
                if (store < k) qs_left = store + 1;
                else qs_right = store - 1;
            }
            const uint8_t noise_floor = spectrum_sort_buffer_[k];
            const uint8_t display_threshold = noise_floor + display_margin_;
            for (size_t i = 0; i < count; ++i) {
                const uint8_t val = spectrum_data[i];
                spectrum_buffer_[i] = (val >= display_threshold) ? val : 0;
            }
        } else {
            for (size_t i = 0; i < count; ++i) {
                spectrum_buffer_[i] = spectrum_data[i];
            }
        }
    } else {
        for (size_t i = 0; i < count; ++i) {
            spectrum_buffer_[i] = spectrum_data[i];
        }
    }

    dirty_flags_ |= DIRTY_SPEC;
    set_dirty();
    return ErrorCode::SUCCESS;
}

void DroneDisplay::push_timeline_value(uint8_t peak_power) noexcept {
    timeline_.set_active_windows(1);
    timeline_.push_single_value(peak_power);
    dirty_flags_ |= DIRTY_WATERFALL;
    set_dirty();
}

void DroneDisplay::push_waterfall_from_sweep(
    const uint8_t* composite_240,
    uint8_t window_count,
    const uint8_t* composite2_240,
    const uint8_t* composite3_240,
    const uint8_t* composite4_240
) noexcept {
    timeline_.set_active_windows(window_count);
    timeline_.push_multi_window(
        composite_240, composite2_240,
        composite3_240, composite4_240
    );
    dirty_flags_ |= DIRTY_WATERFALL;
    set_dirty();
}

void DroneDisplay::reset_timeline() noexcept {
    timeline_.reset();
    dirty_flags_ |= DIRTY_WATERFALL;
    set_dirty();
}

void DroneDisplay::set_status_text(const char* status_text) noexcept {
    if (status_text == nullptr) {
        return;
    }
    size_t i = 0;
    while (i < MAX_TEXT_LENGTH - 1 && status_text[i] != '\0') {
        status_text_[i] = status_text[i];
        ++i;
    }
    status_text_[i] = '\0';
    dirty_flags_ |= DIRTY_STATUS;
    set_dirty();
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
    
    // Format RSSI (no "dBm" unit)
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
        case MovementTrend::APPROACHING: trend_symbol = MOVEMENT_TREND_SYMBOL_APPROACHING; break;
        case MovementTrend::RECEDING:    trend_symbol = MOVEMENT_TREND_SYMBOL_RECEEDING; break;
        case MovementTrend::STATIC:      trend_symbol = MOVEMENT_TREND_SYMBOL_STATIC; break;
        default:                         trend_symbol = MOVEMENT_TREND_SYMBOL_UNKNOWN; break;
    }
    
    // Layout: proportional columns
    // Col 1 (0-40%): Type name; below it frequency + RSSI grouped (no MHz suffix)
    // Threat level: shifted THREAT_SHIFT_CHARS left of Col 1's right edge, then
    //   clamped to always sit GAP_CHARS right of the type text — so a 7-char
    //   type name and a 4-char threat never overlap, even at width=120.
    // Trend symbol: right-aligned at the row's right edge (x + width - PAD - 1ch).
    const uint16_t char_w = Theme::getInstance()->fg_light->font.char_width();
    const uint16_t col1_end = (width * 40) / 100;
    // A third (optional) pattern row must fit: y+22+16 within the entry height.
    constexpr uint16_t PATTERN_ROW_MIN_H = 38;

    // Type name — truncated to TYPE_MAX_CHARS so the threat clamp below always
    // has a known upper bound, even for names longer than 7 chars.
    const char* type_name = drone.get_type_name();
    const size_t type_len = std::strlen(type_name);
    const size_t type_chars = (type_len > TYPE_MAX_CHARS) ? TYPE_MAX_CHARS : type_len;
    const uint16_t type_end = x + LIST_PAD + static_cast<uint16_t>(type_chars) * char_w;

    // Threat level: default position, clamped right of the type text and left of
    // the entry edge so a 4-char label always stays fully visible (even at 120px).
    const uint16_t threat_x_default = x + col1_end + LIST_PAD - THREAT_SHIFT_CHARS * char_w;
    const uint16_t threat_x_min = type_end + GAP_CHARS * char_w;
    const uint16_t threat_x_max = x + width - THREAT_MAX_CHARS * char_w - LIST_PAD;
    const uint16_t threat_x = (threat_x_default > threat_x_min)
        ? ((threat_x_default < threat_x_max) ? threat_x_default : threat_x_max)
        : threat_x_min;

    // Line 1: Type | Threat
    draw_text(painter, std::string_view(type_name, type_chars), x + LIST_PAD, y + 2, drone.display_color);
    draw_text(painter, threat_str, threat_x, y + 2, drone.display_color);

    // Line 2: Frequency (no unit) | RSSI | Trend
    draw_text(painter, freq_buffer, x + LIST_PAD, y + 12, COLOR_TEXT);
    draw_text(painter, rssi_buffer,
              x + LIST_PAD + static_cast<uint16_t>(std::strlen(freq_buffer)) * char_w + char_w,
              y + 12, COLOR_TEXT);

    // Trend symbol right-aligned to the row's right edge.
    char trend_buffer[2] = {trend_symbol, '\0'};
    const uint16_t trend_x = x + width - LIST_PAD - char_w;
    draw_text(painter, trend_buffer, trend_x, y + 12, COLOR_TEXT);

    // Line 3 (optional): Pattern match info — on its own row so it never
    // collides with the RSSI now adjacent to the frequency.
    if (drone.pattern_matched && drone.pattern_name[0] != '\0' && height >= PATTERN_ROW_MIN_H) {
        char pattern_buf[22];
        const uint16_t score_pct = (drone.pattern_score * 100) / 1000;
        snprintf(pattern_buf, sizeof(pattern_buf), "PTR:%.12s(%u%%)",
                 drone.pattern_name,
                 static_cast<unsigned>(score_pct));
        const uint32_t pm_color = (drone.pattern_score >= SIMILARITY_STRONG)
            ? COLOR_MEDIUM_THREAT : COLOR_LOW_THREAT;
        draw_text(painter, pattern_buf, x + col1_end + LIST_PAD, y + 22, pm_color);
    }
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
    *buf = '\0';
}

ErrorCode DroneDisplay::validate_spectrum_data(
    const uint8_t* spectrum_data,
    size_t spectrum_size
) const noexcept {
    return validate_spectrum_buffer(spectrum_data, spectrum_size);
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
    if (data == nullptr || size == 0) {
        composite_data_ = nullptr;
        composite_data_size_ = 0;
        return;
    }

    const size_t copy_n = (size < COMPOSITE_SIZE) ? size : COMPOSITE_SIZE;

    // Apply EMA persistence: keep old data unless new data has a higher peak.
    // Effectively max(raw, old * DECAY), which holds peaks steady between passes
    // while letting noise floor decay toward zero.
    if (!composite_persist_initialized_) {
        for (size_t i = 0; i < copy_n; ++i) {
            composite_persist_buf_[i] = data[i];
        }
        composite_persist_initialized_ = true;
    } else {
        for (size_t i = 0; i < copy_n; ++i) {
            const uint16_t decayed = (static_cast<uint16_t>(composite_persist_buf_[i])
                                      * SWEEP_PERSISTENCE_DECAY_Q8) >> 8;
            composite_persist_buf_[i] = (data[i] > static_cast<uint8_t>(decayed))
                ? data[i]
                : static_cast<uint8_t>(decayed);
        }
    }

    composite_data_ = composite_persist_buf_;
    composite_data_size_ = copy_n;
    dirty_flags_ |= DIRTY_SPEC;
    set_dirty();
}

void DroneDisplay::reset_composite_persistence() noexcept {
    // Drop EMA state so the next set_composite_data() rebuilds the buffer from
    // scratch (it enters the "not initialized" branch and copies the raw data).
    std::memset(composite_persist_buf_, 0, COMPOSITE_SIZE);
    composite_persist_initialized_ = false;
    composite_noise_floor_ = 0;
    composite_noise_floor_valid_ = false;
    // Also reset band 2 persistence (dual-sweep mode).
    std::memset(sweep2_persist_buf_, 0, COMPOSITE_SIZE);
    sweep2_persist_initialized_ = false;
    sweep2_noise_floor_ = 0;
    sweep2_noise_floor_valid_ = false;
    dirty_flags_ = DIRTY_ALL;
    set_dirty();
    // DO NOT null composite_data_ or composite_data_size_ here.
    // Nulling them causes calculate_layout() to collapse the spectrum area
    // (show_spec = false), letting the timeline take over the display —
    // this is the root cause of "sweep does 1 pass, then hangs, replaced
    // by timeline" bug. The zeroed persist buffer renders as empty bars
    // (invisible) until the next set_composite_data() fills it with fresh data.
}

// ============================================================================
// Noise floor computation (called once per sweep-pass, not per frame)
// ============================================================================

static void quickselect_pctile(
    uint8_t* buf,
    size_t n,
    size_t k,
    uint8_t& output
) noexcept {
    if (n == 0) return;
    if (k >= n) k = n - 1;
    size_t ql = 0;
    size_t qr = n - 1;
    while (ql < qr) {
        const size_t pv = ql + (qr - ql) / 2;
        const uint8_t pivot = buf[pv];
        buf[pv] = buf[qr];
        buf[qr] = pivot;
        size_t st = ql;
        for (size_t i = ql; i < qr; ++i) {
            if (buf[i] < pivot) {
                const uint8_t t = buf[st];
                buf[st] = buf[i];
                buf[i] = t;
                ++st;
            }
        }
        {
            const uint8_t t = buf[st];
            buf[st] = buf[qr];
            buf[qr] = t;
        }
        if (st == k) break;
        if (st < k) ql = st + 1;
        else qr = st - 1;
    }
    output = buf[k];
}

void DroneDisplay::update_noise_floor() noexcept {
    // Band 1: quickselect on composite persistence buffer.
    if (composite_data_size_ > 0) {
        for (size_t i = 0; i < composite_data_size_; ++i) {
            composite_sort_buf_[i] = composite_persist_buf_[i];
        }
        size_t k = (composite_data_size_ * SWEEP_NOISE_FLOOR_PERCENTILE) / 100;
        if (k >= composite_data_size_) k = composite_data_size_ - 1;
        quickselect_pctile(composite_sort_buf_, composite_data_size_, k, composite_noise_floor_);
        composite_noise_floor_valid_ = true;
    }

    // Band 2: quickselect on sweep2 persistence buffer.
    if (sweep2_data_size_ > 0) {
        for (size_t i = 0; i < sweep2_data_size_; ++i) {
            sweep2_sort_buf_[i] = sweep2_persist_buf_[i];
        }
        size_t k2 = (sweep2_data_size_ * SWEEP_NOISE_FLOOR_PERCENTILE) / 100;
        if (k2 >= sweep2_data_size_) k2 = sweep2_data_size_ - 1;
        quickselect_pctile(sweep2_sort_buf_, sweep2_data_size_, k2, sweep2_noise_floor_);
        sweep2_noise_floor_valid_ = true;
    }
    dirty_flags_ |= DIRTY_SPEC;
    set_dirty();
}

// ============================================================================
// Shared envelope bar drawing (DRY: used by render_composite + render_multi_zone)
// ============================================================================

void DroneDisplay::draw_bar_with_envelope(
    Painter& painter,
    uint16_t x,
    uint16_t y,
    uint16_t bar_height,
    uint32_t color,
    EnvelopeState& state
) noexcept {
    if (color == COLOR_BACKGROUND || bar_height == 0) {
        state.gap_count++;
        if (state.gap_count > EnvelopeState::MAX_GAP) {
            state.prev_valid = false;
        }
        return;
    }

    draw_rectangle(painter, x, y, 1, bar_height, color);

    // Envelope: draw connector from previous active pixel, then dot.
    if (state.prev_valid && state.gap_count == 0 && y != state.prev_y) {
        const uint16_t lo = (y < state.prev_y) ? y : state.prev_y;
        const uint16_t hi = (y < state.prev_y) ? state.prev_y : y;
        draw_rectangle(painter, x, lo, 1, hi - lo + 1, COLOR_TEXT);
    } else if (state.prev_valid && state.gap_count > 0
               && state.gap_count <= EnvelopeState::MAX_GAP) {
        const uint16_t lo = (y < state.prev_y) ? y : state.prev_y;
        const uint16_t hi = (y < state.prev_y) ? state.prev_y : y;
        draw_rectangle(painter, x, lo, 1, hi - lo + 1, COLOR_TEXT);
    }
    draw_rectangle(painter, x, y, 1, 1, COLOR_TEXT);

    state.prev_y = y;
    state.prev_valid = true;
    state.gap_count = 0;
}

void DroneDisplay::render_composite(
    Painter& painter,
    const uint8_t* composite_data,
    size_t composite_size,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height,
    int16_t scan_head,
    uint8_t noise_floor
) noexcept {
    if (composite_data == nullptr || composite_size == 0 || height < 4) {
        return;
    }

    draw_rectangle(painter, start_x, start_y, width, height, COLOR_BACKGROUND);
    draw_rectangle(painter, start_x, start_y, width, 1, COLOR_UNKNOWN_THREAT);

    // Title: compact frequency range (e.g. "5700M-5900M")
    if (sweep_freq_start_ < sweep_freq_end_) {
        char title_buf[20];
        const uint32_t mhz_lo = static_cast<uint32_t>(sweep_freq_start_ / 1000000ULL);
        const uint32_t mhz_hi = static_cast<uint32_t>(sweep_freq_end_ / 1000000ULL);
        char* dst = title_buf;
        size_t rem = sizeof(title_buf);
        write_uint(dst, rem, mhz_lo);
        write_str(dst, rem, "M-");
        write_uint(dst, rem, mhz_hi);
        write_str(dst, rem, "M");
        *dst = '\0';
        draw_text(painter, title_buf, start_x + 2, start_y + 2, COLOR_TEXT);
    } else {
        draw_text(painter, "SWEEP", start_x + 2, start_y + 2, COLOR_TEXT);
    }

    const uint16_t bar_count = static_cast<uint16_t>(composite_size);

    const uint16_t chart_start_x = start_x + 2;
    const uint16_t chart_start_y = start_y + 12;
    const uint16_t chart_height = height - 14;
    if (chart_height < 4) return;

    // Compute display threshold: subtract noise floor + margin from all power values.
    // This eliminates the visible noise baseline while preserving signal peaks.
    // noise_floor parameter allows per-band noise floor in dual-sweep mode.
    const uint8_t display_threshold = min_color_power_;

    // White envelope state: connects bar tops (classical spectrum analyzer look).
    // Envelope only draws on above-threshold bars — eliminates the sub-threshold
    // white dot line that previously appeared at the chart bottom.
    // Stack: ~6 bytes (EnvelopeState struct).
    const uint16_t envelope_y0 = static_cast<uint16_t>(chart_start_y + chart_height);
    EnvelopeState envelope{envelope_y0, false, 0};

    for (uint16_t i = 0; i < bar_count; ++i) {
        uint8_t power = composite_data[i];

        // Subtract noise floor — reused for both bar color and envelope.
        if (power > noise_floor) {
            power -= noise_floor;
        } else {
            power = 0;
        }

        const uint16_t x = chart_start_x + i;

        // Bar color from noise-subtracted power
        uint32_t color = COLOR_BACKGROUND;
        if (power >= display_threshold) {
            if (power > 200) color = COLOR_CRITICAL_THREAT;
            else if (power > 150) color = COLOR_HIGH_THREAT;
            else if (power > 100) color = COLOR_MEDIUM_THREAT;
            else color = COLOR_LOW_THREAT;
        }

        const uint16_t bar_height = (static_cast<uint16_t>(power) * chart_height) >> 8;
        const uint16_t final_height = (bar_height > 0) ? bar_height : 1;
        const uint16_t y = chart_start_y + chart_height - final_height;

        draw_bar_with_envelope(painter, x, y, final_height, color, envelope);
    }

    // Real-time scan-head marker: 1-px white vertical line at current pixel_index.
    // Shows where the sweep is right now, independent of pixel-value filtering.
    if (scan_head >= 0 && static_cast<uint16_t>(scan_head) < bar_count) {
        const uint16_t hx = chart_start_x + static_cast<uint16_t>(scan_head);
        painter.draw_rectangle({hx, chart_start_y, 1, chart_height}, Color::white());
    }

    // Draw red frame for matched pattern
    if (matched_pattern_bin_ >= 0) {
        const uint16_t frame_x = chart_start_x + static_cast<uint16_t>(matched_pattern_bin_) - 3;
        const uint16_t frame_w = 7;
        painter.draw_rectangle({
            frame_x,
            chart_start_y,
            frame_w,
            chart_height
        }, Color::red());
    }
}

void DroneDisplay::set_multi_zone_data(const uint8_t buffers[][240], uint8_t zone_count, size_t /*buffer_size*/,
                                       const FreqHz* freq_starts, const FreqHz* freq_ends) noexcept {
    if (zone_count > MAX_ZONES) zone_count = MAX_ZONES;
    multi_zone_count_ = zone_count;
    for (uint8_t z = 0; z < zone_count; ++z) {
        multi_zone_data_[z] = buffers[z];
        zone_freq_start_[z] = freq_starts[z];
        zone_freq_end_[z] = freq_ends[z];
    }
    dirty_flags_ |= DIRTY_SPEC;
    set_dirty();
}

void DroneDisplay::render_multi_zone(
    Painter& painter,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    if (multi_zone_count_ == 0 || height < 20) return;

    // Divide height into equal zones with 1px separator
    const uint16_t zone_h = height / multi_zone_count_;
    const uint16_t sep_h = 1;

    for (uint8_t z = 0; z < multi_zone_count_; ++z) {
        const uint16_t zone_y = start_y + z * (zone_h + sep_h);
        const uint8_t* data = multi_zone_data_[z];

        // Background
        draw_rectangle(painter, start_x, zone_y, width, zone_h, COLOR_BACKGROUND);

        // Separator line at top (except first zone)
        if (z > 0) {
            draw_rectangle(painter, start_x, zone_y - sep_h, width, sep_h, COLOR_UNKNOWN_THREAT);
        }

        // Title: zone frequency range
        if (data != nullptr) {
            char title[16];
            const uint32_t mhz_lo = static_cast<uint32_t>(zone_freq_start_[z] / 1000000ULL);
            const uint32_t mhz_hi = static_cast<uint32_t>(zone_freq_end_[z] / 1000000ULL);
            char* dst = title;
            size_t rem = sizeof(title);
            write_uint(dst, rem, mhz_lo);
            write_str(dst, rem, "-");
            write_uint(dst, rem, mhz_hi);
            write_str(dst, rem, "M");
            *dst = '\0';
            draw_text(painter, title, start_x + 2, zone_y + 2, COLOR_TEXT);

            // Bar chart + white envelope via shared method (DRY).
            // Stack: ~6 bytes (EnvelopeState).
            constexpr uint16_t bar_width = 1;
            const uint16_t chart_y = zone_y + 12;
            const uint16_t chart_h = zone_h - 14;
            if (chart_h < 4) continue;

            const uint16_t envelope_y0 = static_cast<uint16_t>(chart_y + chart_h);
            EnvelopeState envelope{envelope_y0, false, 0};

            for (uint16_t i = 0; i < width - 4 && i < 240; ++i) {
                const uint8_t power = data[i];

                // Skip below-threshold bins (consistent with render_composite).
                uint32_t color = COLOR_BACKGROUND;
                if (power >= min_color_power_) {
                    if (power > 200) color = COLOR_CRITICAL_THREAT;
                    else if (power > 150) color = COLOR_HIGH_THREAT;
                    else if (power > 100) color = COLOR_MEDIUM_THREAT;
                    else color = COLOR_LOW_THREAT;
                }

                const uint16_t bar_h = (static_cast<uint16_t>(power) * chart_h) >> 8;
                if (bar_h == 0 && color == COLOR_BACKGROUND) continue;

                const uint16_t x = start_x + 2 + i * bar_width;
                const uint16_t y = chart_y + chart_h - bar_h;

                draw_bar_with_envelope(painter, x, y, bar_h, color, envelope);
            }
        }
    }
}

void DroneDisplay::set_sweep2_data(const uint8_t* data, size_t size) noexcept {
    if (data == nullptr || size == 0) {
        sweep2_data_ = nullptr;
        sweep2_data_size_ = 0;
        return;
    }

    const size_t copy_n = (size < COMPOSITE_SIZE) ? size : COMPOSITE_SIZE;

    // Apply EMA persistence for band 2 (same formula as band 1).
    if (!sweep2_persist_initialized_) {
        for (size_t i = 0; i < copy_n; ++i) {
            sweep2_persist_buf_[i] = data[i];
        }
        sweep2_persist_initialized_ = true;
    } else {
        for (size_t i = 0; i < copy_n; ++i) {
            const uint16_t decayed = (static_cast<uint16_t>(sweep2_persist_buf_[i])
                                      * SWEEP_PERSISTENCE_DECAY_Q8) >> 8;
            sweep2_persist_buf_[i] = (data[i] > static_cast<uint8_t>(decayed))
                ? data[i]
                : static_cast<uint8_t>(decayed);
        }
    }

    sweep2_data_ = sweep2_persist_buf_;
    sweep2_data_size_ = copy_n;
    dirty_flags_ |= DIRTY_SPEC;
    set_dirty();
}

void DroneDisplay::render_dual_composite(
    Painter& painter,
    uint16_t start_x,
    uint16_t start_y,
    uint16_t width,
    uint16_t height
) noexcept {
    if (composite_data_ == nullptr || sweep2_data_ == nullptr || height < 8) {
        return;
    }

    // Split height between two sweep bands
    const uint16_t band_h = height / 2;
    if (band_h < 4) return;

    // Render sweep 1 (top half) — upper band scan head
    // Pass band 1's noise floor for correct per-band noise subtraction.
    const uint8_t nf1 = composite_noise_floor_valid_ ? composite_noise_floor_ : 0;
    render_composite(painter, composite_data_, composite_data_size_,
                     start_x, start_y, width, band_h, scan_head_position_[0], nf1);

    // Render sweep 2 (bottom half) — temporarily swap freq range
    // Pass band 2's independently computed noise floor.
    const FreqHz saved_start = sweep_freq_start_;
    const FreqHz saved_end = sweep_freq_end_;
    sweep_freq_start_ = sweep2_freq_start_;
    sweep_freq_end_ = sweep2_freq_end_;
    const uint8_t nf2 = sweep2_noise_floor_valid_ ? sweep2_noise_floor_ : 0;
    render_composite(painter, sweep2_data_, sweep2_data_size_,
                     start_x, start_y + band_h, width, band_h, scan_head_position_[1], nf2);
    sweep_freq_start_ = saved_start;
    sweep_freq_end_ = saved_end;
}

int16_t DroneDisplay::hit_test(uint16_t x, uint16_t y) const noexcept {
    if (!drone_list_visible_ || display_data_.drone_count == 0) {
        return -1;
    }

    const uint16_t width = parent_rect().size().width();
    if (x >= width) return -1;

    // SINGLE SOURCE OF TRUTH: Use calculate_layout() — same as paint() uses
    const auto layout = calculate_layout();

    if (y < layout.list_start_y || y >= layout.list_start_y + layout.drone_h) return -1;

    // No header row — entry rows begin at the section top (mirrors render_drone_list).
    // SINGLE SOURCE OF TRUTH: compute_entry_layout() — the same helper paint() uses.
    const uint16_t entry_y = y - layout.list_start_y;
    const EntryLayout el = compute_entry_layout(display_data_.drone_count, width,
                                                layout.drone_h, dual_column_mode_);
    // Rows that actually fit — mirrors the "Don't overflow" break in render_drone_list.
    const uint16_t visible_rows = (el.entry_h > 0) ? (layout.drone_h / el.entry_h) : 0;

    if (el.col_w < width) {
        // Dual column: pick the column by X, the row by Y. Odd count means the
        // right column's last cell is empty → that tap returns -1.
        const uint16_t col = (x >= el.col_w) ? 1 : 0;
        const uint16_t row = entry_y / el.entry_h;
        if (row >= visible_rows) return -1;
        const size_t idx = (col == 0) ? row : (static_cast<size_t>(row) + el.rows);
        if (idx >= display_data_.drone_count) return -1;
        return static_cast<int16_t>(idx);
    }

    const uint16_t idx = entry_y / el.entry_h;
    if (idx >= visible_rows) return -1;
    if (idx >= display_data_.drone_count) return -1;

    return static_cast<int16_t>(idx);
}

} // namespace drone_analyzer
