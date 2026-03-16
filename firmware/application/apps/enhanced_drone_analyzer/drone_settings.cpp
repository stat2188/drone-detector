#include "drone_settings.hpp"

namespace drone_analyzer {

// ============================================================================
// DroneSettings Implementation
// ============================================================================

DroneSettings::DroneSettings() noexcept
    : scanning_mode(DEFAULT_SCANNING_MODE)
    , scan_interval_ms(SCAN_CYCLE_INTERVAL_MS)
    , scan_sensitivity(50)
    , spectrum_visible(true)
    , histogram_visible(true)
    , drone_list_visible(true)
    , status_bar_visible(true)
    , audio_alerts_enabled(false)
    , alert_rssi_threshold_dbm(RSSI_HIGH_THREAT_THRESHOLD_DBM)
    , min_threat_level(ThreatLevel::LOW) {
}

void DroneSettings::reset_to_defaults() noexcept {
    scanning_mode = DEFAULT_SCANNING_MODE;
    scan_interval_ms = SCAN_CYCLE_INTERVAL_MS;
    scan_sensitivity = 50;
    
    spectrum_visible = true;
    histogram_visible = true;
    drone_list_visible = true;
    status_bar_visible = true;
    
    audio_alerts_enabled = false;
    alert_rssi_threshold_dbm = RSSI_HIGH_THREAT_THRESHOLD_DBM;
    
    min_threat_level = ThreatLevel::LOW;
}

// ============================================================================
// DroneSettingsView Constructor / Destructor
// ============================================================================

DroneSettingsView::DroneSettingsView() noexcept
    : ui::View()
    , settings_()
    , settings_file_path_(SETTINGS_FILE_PATH)
    , settings_loaded_(false)
    , settings_dirty_(false)
    , header_height_(30)
    , section_spacing_(20)
    , row_height_(25)
    , checkbox_size_(20)
    , button_height_(30) {
}

DroneSettingsView::~DroneSettingsView() noexcept {
    // Destructor - no dynamic memory to free
}

// ============================================================================
// Paint Method
// ============================================================================

void DroneSettingsView::paint(ui::Painter& painter) {
    // Clear display
    // draw_rectangle(painter, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, COLOR_BACKGROUND);
    
    uint16_t y_offset = 10;
    
    // Draw header
    draw_settings_header(painter);
    y_offset += header_height_ + section_spacing_;
    
    // Draw scanning settings
    draw_scanning_settings(painter, y_offset);
    y_offset += (3 * row_height_) + section_spacing_;
    
    // Draw display settings
    draw_display_settings(painter, y_offset);
    y_offset += (4 * row_height_) + section_spacing_;
    
    // Draw alert settings
    draw_alert_settings(painter, y_offset);
    y_offset += (2 * row_height_) + section_spacing_;
    
    // Draw threat settings
    draw_threat_settings(painter, y_offset);
}

// ============================================================================
// Setup Settings UI
// ============================================================================

void DroneSettingsView::setup_settings_ui() noexcept {
    // UI components will be created when UI framework is available
    // For now, this is a placeholder
}

// ============================================================================
// Setting Change Handler
// ============================================================================

ErrorCode DroneSettingsView::on_setting_change(uint32_t setting_id, int32_t value) noexcept {
    switch (setting_id) {
        case SETTING_ID_SCANNING_MODE:
            if (value >= 0 && value <= 3) {
                settings_.scanning_mode = static_cast<ScanningMode>(value);
                settings_dirty_ = true;
            } else {
                return ErrorCode::INVALID_PARAMETER;
            }
            break;
        
        case SETTING_ID_SCAN_INTERVAL:
            if (value >= 10 && value <= 1000) {
                settings_.scan_interval_ms = static_cast<uint32_t>(value);
                settings_dirty_ = true;
            } else {
                return ErrorCode::INVALID_PARAMETER;
            }
            break;
        
        case SETTING_ID_SCAN_SENSITIVITY:
            if (value >= 0 && value <= 100) {
                settings_.scan_sensitivity = static_cast<uint8_t>(value);
                settings_dirty_ = true;
            } else {
                return ErrorCode::INVALID_PARAMETER;
            }
            break;
        
        case SETTING_ID_SPECTRUM_VISIBLE:
            settings_.spectrum_visible = (value != 0);
            settings_dirty_ = true;
            break;
        
        case SETTING_ID_HISTOGRAM_VISIBLE:
            settings_.histogram_visible = (value != 0);
            settings_dirty_ = true;
            break;
        
        case SETTING_ID_DRONE_LIST_VISIBLE:
            settings_.drone_list_visible = (value != 0);
            settings_dirty_ = true;
            break;
        
        case SETTING_ID_STATUS_BAR_VISIBLE:
            settings_.status_bar_visible = (value != 0);
            settings_dirty_ = true;
            break;
        
        case SETTING_ID_AUDIO_ALERTS:
            settings_.audio_alerts_enabled = (value != 0);
            settings_dirty_ = true;
            break;
        
        case SETTING_ID_ALERT_RSSI_THRESHOLD:
            if (value >= RSSI_MIN_DBM && value <= RSSI_MAX_DBM) {
                settings_.alert_rssi_threshold_dbm = value;
                settings_dirty_ = true;
            } else {
                return ErrorCode::INVALID_PARAMETER;
            }
            break;
        
        case SETTING_ID_MIN_THREAT_LEVEL:
            if (value >= 0 && value <= 4) {
                settings_.min_threat_level = static_cast<ThreatLevel>(value);
                settings_dirty_ = true;
            } else {
                return ErrorCode::INVALID_PARAMETER;
            }
            break;
        
        default:
            return ErrorCode::INVALID_PARAMETER;
    }
    
    // Apply settings after change
    apply_settings();
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// Save / Load Settings
// ============================================================================

ErrorCode DroneSettingsView::save_settings() noexcept {
    // Placeholder - will use FatFS when available
    // For now, just mark as saved
    settings_dirty_ = false;
    return ErrorCode::SUCCESS;
}

ErrorCode DroneSettingsView::load_settings() noexcept {
    // Placeholder - will use FatFS when available
    // For now, just use defaults
    settings_.reset_to_defaults();
    settings_loaded_ = true;
    settings_dirty_ = false;
    return ErrorCode::SUCCESS;
}

// ============================================================================
// Settings Accessors
// ============================================================================

const DroneSettings& DroneSettingsView::get_settings() const noexcept {
    return settings_;
}

void DroneSettingsView::set_settings(const DroneSettings& settings) noexcept {
    settings_ = settings;
    settings_dirty_ = true;
}

void DroneSettingsView::reset_settings() noexcept {
    settings_.reset_to_defaults();
    settings_dirty_ = true;
    apply_settings();
}

ErrorCode DroneSettingsView::validate_settings() const noexcept {
    // Validate scanning mode
    if (settings_.scanning_mode > ScanningMode::TARGETED) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Validate scan interval
    if (settings_.scan_interval_ms < 10 || settings_.scan_interval_ms > 1000) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Validate scan sensitivity
    if (settings_.scan_sensitivity > 100) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Validate RSSI threshold
    if (settings_.alert_rssi_threshold_dbm < RSSI_MIN_DBM ||
        settings_.alert_rssi_threshold_dbm > RSSI_MAX_DBM) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Validate threat level
    if (settings_.min_threat_level > ThreatLevel::CRITICAL) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// Drawing Methods
// ============================================================================

void DroneSettingsView::draw_settings_header(ui::Painter& painter) noexcept {
    // Will use ui::Painter when available
    (void)painter;
}

void DroneSettingsView::draw_scanning_settings(ui::Painter& painter, uint16_t start_y) noexcept {
    char value_buffer[16];
    
    // Scanning mode
    format_value_string(static_cast<int32_t>(settings_.scanning_mode), value_buffer, sizeof(value_buffer));
    draw_setting_row(
        painter,
        "Scanning Mode",
        get_scanning_mode_name(settings_.scanning_mode),
        10,
        start_y,
        DISPLAY_WIDTH - 20,
        row_height_
    );
    
    // Scan interval
    format_value_string(static_cast<int32_t>(settings_.scan_interval_ms), value_buffer, sizeof(value_buffer));
    draw_setting_row(
        painter,
        "Scan Interval",
        value_buffer,
        10,
        start_y + row_height_,
        DISPLAY_WIDTH - 20,
        row_height_
    );
    
    // Scan sensitivity
    format_value_string(static_cast<int32_t>(settings_.scan_sensitivity), value_buffer, sizeof(value_buffer));
    draw_setting_row(
        painter,
        "Sensitivity",
        value_buffer,
        10,
        start_y + (2 * row_height_),
        DISPLAY_WIDTH - 20,
        row_height_
    );
}

void DroneSettingsView::draw_display_settings(ui::Painter& painter, uint16_t start_y) noexcept {
    // Spectrum visible
    draw_checkbox(painter, settings_.spectrum_visible, 10, start_y + 5, checkbox_size_);
    draw_setting_row(
        painter,
        "Show Spectrum",
        "",
        40,
        start_y,
        DISPLAY_WIDTH - 50,
        row_height_
    );
    
    // Histogram visible
    draw_checkbox(painter, settings_.histogram_visible, 10, start_y + row_height_ + 5, checkbox_size_);
    draw_setting_row(
        painter,
        "Show Histogram",
        "",
        40,
        start_y + row_height_,
        DISPLAY_WIDTH - 50,
        row_height_
    );
    
    // Drone list visible
    draw_checkbox(painter, settings_.drone_list_visible, 10, start_y + (2 * row_height_) + 5, checkbox_size_);
    draw_setting_row(
        painter,
        "Show Drone List",
        "",
        40,
        start_y + (2 * row_height_),
        DISPLAY_WIDTH - 50,
        row_height_
    );
    
    // Status bar visible
    draw_checkbox(painter, settings_.status_bar_visible, 10, start_y + (3 * row_height_) + 5, checkbox_size_);
    draw_setting_row(
        painter,
        "Show Status Bar",
        "",
        40,
        start_y + (3 * row_height_),
        DISPLAY_WIDTH - 50,
        row_height_
    );
}

void DroneSettingsView::draw_alert_settings(ui::Painter& painter, uint16_t start_y) noexcept {
    char value_buffer[16];
    
    // Audio alerts
    draw_checkbox(painter, settings_.audio_alerts_enabled, 10, start_y + 5, checkbox_size_);
    draw_setting_row(
        painter,
        "Audio Alerts",
        "",
        40,
        start_y,
        DISPLAY_WIDTH - 50,
        row_height_
    );
    
    // Alert RSSI threshold
    format_value_string(settings_.alert_rssi_threshold_dbm, value_buffer, sizeof(value_buffer));
    draw_setting_row(
        painter,
        "Alert Threshold",
        value_buffer,
        10,
        start_y + row_height_,
        DISPLAY_WIDTH - 20,
        row_height_
    );
}

void DroneSettingsView::draw_threat_settings(ui::Painter& painter, uint16_t start_y) noexcept {
    draw_setting_row(
        painter,
        "Min Threat Level",
        get_threat_level_name(settings_.min_threat_level),
        10,
        start_y,
        DISPLAY_WIDTH - 20,
        row_height_
    );
}

void DroneSettingsView::draw_setting_row(
    ui::Painter& painter,
    const char* label,
    const char* value,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height
) noexcept {
    // Will use ui::Painter when available
    (void)painter;
    (void)label;
    (void)value;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
}

void DroneSettingsView::draw_checkbox(
    ui::Painter& painter,
    bool checked,
    uint16_t x,
    uint16_t y,
    uint16_t size
) noexcept {
    // Will use ui::Painter when available
    (void)painter;
    (void)checked;
    (void)x;
    (void)y;
    (void)size;
}

void DroneSettingsView::draw_button(
    ui::Painter& painter,
    const char* label,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    bool enabled
) noexcept {
    // Will use ui::Painter when available
    (void)painter;
    (void)label;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)enabled;
}

// ============================================================================
// Utility Methods
// ============================================================================

const char* DroneSettingsView::get_scanning_mode_name(ScanningMode mode) const noexcept {
    switch (mode) {
        case ScanningMode::SINGLE:
            return "Single";
        case ScanningMode::HOPPING:
            return "Hopping";
        case ScanningMode::SEQUENTIAL:
            return "Sequential";
        case ScanningMode::TARGETED:
            return "Targeted";
        default:
            return "Unknown";
    }
}

const char* DroneSettingsView::get_threat_level_name(ThreatLevel threat) const noexcept {
    switch (threat) {
        case ThreatLevel::NONE:
            return "None";
        case ThreatLevel::LOW:
            return "Low";
        case ThreatLevel::MEDIUM:
            return "Medium";
        case ThreatLevel::HIGH:
            return "High";
        case ThreatLevel::CRITICAL:
            return "Critical";
        default:
            return "Unknown";
    }
}

void DroneSettingsView::format_value_string(
    int32_t value,
    char* buffer,
    size_t buffer_size
) const noexcept {
    if (buffer == nullptr || buffer_size < 16) {
        return;
    }
    
    // Simple formatting (will use proper function when available)
    if (value < 0) {
        buffer[0] = '-';
        value = -value;
    } else {
        buffer[0] = ' ';
    }
    
    if (value >= 1000) {
        buffer[1] = '0' + static_cast<char>((value / 1000) % 10);
        buffer[2] = '0' + static_cast<char>((value / 100) % 10);
        buffer[3] = '0' + static_cast<char>((value / 10) % 10);
        buffer[4] = '0' + static_cast<char>(value % 10);
        buffer[5] = '\0';
    } else if (value >= 100) {
        buffer[1] = '0' + static_cast<char>((value / 100) % 10);
        buffer[2] = '0' + static_cast<char>((value / 10) % 10);
        buffer[3] = '0' + static_cast<char>(value % 10);
        buffer[4] = '\0';
    } else if (value >= 10) {
        buffer[1] = '0' + static_cast<char>((value / 10) % 10);
        buffer[2] = '0' + static_cast<char>(value % 10);
        buffer[3] = '\0';
    } else {
        buffer[1] = '0' + static_cast<char>(value % 10);
        buffer[2] = '\0';
    }
}

bool DroneSettingsView::parse_settings_line(
    const char* line,
    char* key,
    char* value
) const noexcept {
    if (line == nullptr || key == nullptr || value == nullptr) {
        return false;
    }
    
    // Find '=' separator
    size_t i = 0;
    while (line[i] != '\0' && line[i] != '=') {
        key[i] = line[i];
        ++i;
    }
    key[i] = '\0';
    
    if (line[i] != '=') {
        return false;
    }
    
    // Copy value
    ++i;
    size_t j = 0;
    while (line[i] != '\0' && line[i] != '\n' && line[i] != '\r') {
        value[j] = line[i];
        ++i;
        ++j;
    }
    value[j] = '\0';
    
    return true;
}

void DroneSettingsView::write_settings_line(
    const char* key,
    const char* value,
    char* buffer,
    size_t buffer_size
) const noexcept {
    if (key == nullptr || value == nullptr || buffer == nullptr || buffer_size == 0) {
        return;
    }
    
    size_t i = 0;
    
    // Copy key
    size_t j = 0;
    while (key[j] != '\0' && i < buffer_size - 1) {
        buffer[i] = key[j];
        ++i;
        ++j;
    }
    
    // Add '='
    if (i < buffer_size - 1) {
        buffer[i] = '=';
        ++i;
    }
    
    // Copy value
    j = 0;
    while (value[j] != '\0' && i < buffer_size - 1) {
        buffer[i] = value[j];
        ++i;
        ++j;
    }
    
    // Add newline
    if (i < buffer_size - 1) {
        buffer[i] = '\n';
        ++i;
    }
    
    buffer[i] = '\0';
}

void DroneSettingsView::apply_settings() noexcept {
    // Apply settings to system
    // This will be implemented when system integration is available
}

void DroneSettingsView::notify_settings_changed() noexcept {
    // Notify listeners of settings change
    // This will be implemented when observer pattern is available
}

} // namespace drone_analyzer
