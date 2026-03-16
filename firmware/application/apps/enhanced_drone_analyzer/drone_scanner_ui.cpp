#include "drone_scanner_ui.hpp"
#include "ui.hpp"  // Will provide ui::View, Painter, etc.

namespace drone_analyzer {

constexpr uint32_t KEY_START_STOP = 0x01;
constexpr uint32_t KEY_MODE = 0x02;
constexpr uint32_t KEY_SETTINGS = 0x03;

// ============================================================================
// Constructor / Destructor
// ============================================================================

DroneScannerUI::DroneScannerUI(NavigationView& nav) noexcept
    : ui::View()
    , nav_(nav)
    , display_data_()
    , scanning_(false)
    , scanning_mode_(DEFAULT_SCANNING_MODE)
    , alert_active_(false)
    , alert_message_{0}
    , alert_start_time_(0)
    , alert_duration_ms_(0)
    , error_active_(false)
    , last_error_(ErrorCode::SUCCESS)
    , error_start_time_(0)
    , error_duration_ms_(0)
    , selected_button_(BUTTON_ID_START_STOP)
    , settings_visible_(false)
    , header_height_(30)
    , status_height_(40)
    , threat_summary_height_(50)
    , control_buttons_height_(40)
    , button_width_(70)
    , button_height_(30)
    , button_spacing_(10) {
    display_data_.clear();
}

DroneScannerUI::~DroneScannerUI() noexcept {
    // Destructor - no dynamic memory to free
}

// ============================================================================
// Paint Method
// ============================================================================

void DroneScannerUI::paint(Painter& painter) {
    uint16_t y_offset = 5;
    
    draw_scanner_header(painter);
    y_offset += header_height_;
    
    draw_scanner_status(painter, y_offset);
    y_offset += status_height_;
    
    draw_threat_summary(painter, y_offset);
    y_offset += threat_summary_height_;
    
    draw_control_buttons(painter, y_offset);
    
    if (is_alert_active()) {
        draw_alert_overlay(painter);
    }
    
    if (is_error_active()) {
        draw_error_overlay(painter);
    }
}

// ============================================================================
// Display Update
// ============================================================================

ErrorCode DroneScannerUI::update_display(const DisplayData& display_data) noexcept {
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
    
    // Update status text
    update_status_text();
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// User Input Handling
// ============================================================================

ErrorCode DroneScannerUI::handle_user_input(uint32_t key) noexcept {
    // Process key input
    return process_key_input(key);
}

ErrorCode DroneScannerUI::process_key_input(uint32_t key) noexcept {
    switch (key) {
        case KEY_START_STOP:
            return handle_start_stop_key();
        
        case KEY_MODE:
            return handle_mode_key();
        
        case KEY_SETTINGS:
            return handle_settings_key();
        
        default:
            return ErrorCode::INVALID_PARAMETER;
    }
}

ErrorCode DroneScannerUI::handle_start_stop_key() noexcept {
    if (scanning_) {
        return stop_scanning();
    } else {
        return start_scanning();
    }
}

ErrorCode DroneScannerUI::handle_mode_key() noexcept {
    const uint8_t current_mode = static_cast<uint8_t>(scanning_mode_);
    const uint8_t next_mode = (current_mode + 1) % SCANNING_MODE_COUNT;
    scanning_mode_ = static_cast<ScanningMode>(next_mode);
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScannerUI::handle_settings_key() noexcept {
    // Toggle settings visibility
    settings_visible_ = !settings_visible_;
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// Alert / Error Handling
// ============================================================================

void DroneScannerUI::show_alert(const char* message, uint32_t duration_ms) noexcept {
    if (message == nullptr) {
        return;
    }
    
    size_t i = 0;
    while (i < MAX_TEXT_LENGTH - 1 && message[i] != '\0') {
        alert_message_[i] = message[i];
        ++i;
    }
    alert_message_[i] = '\0';
    
    alert_active_ = true;
    alert_start_time_ = chTimeNow();
    alert_duration_ms_ = duration_ms;
}

void DroneScannerUI::show_error(ErrorCode error, uint32_t duration_ms) noexcept {
    error_active_ = true;
    last_error_ = error;
    error_start_time_ = chTimeNow();
    error_duration_ms_ = duration_ms;
}

void DroneScannerUI::clear_alert() noexcept {
    alert_active_ = false;
    alert_message_[0] = '\0';
}

void DroneScannerUI::clear_error() noexcept {
    error_active_ = false;
    last_error_ = ErrorCode::SUCCESS;
}

// ============================================================================
// Audio Alert Handling
// ============================================================================

void DroneScannerUI::on_alert(ThreatLevel threat_level) noexcept {
    const char* alert_message = nullptr;
    uint32_t alert_duration_ms = 2000;

    switch (threat_level) {
        case ThreatLevel::CRITICAL:
            alert_message = "CRITICAL";
            alert_duration_ms = 3000;
            break;
        case ThreatLevel::HIGH:
            alert_message = "HIGH THREAT";
            alert_duration_ms = 2500;
            break;
        case ThreatLevel::MEDIUM:
            alert_message = "THREAT";
            alert_duration_ms = 2000;
            break;
        case ThreatLevel::LOW:
        case ThreatLevel::NONE:
        default:
            return;
    }

    AudioAlertManager::play_alert(threat_level);

    if (alert_message != nullptr) {
        show_alert(alert_message, alert_duration_ms);
    }
}

// ============================================================================
// Scanning Control
// ============================================================================

ErrorCode DroneScannerUI::start_scanning() noexcept {
    if (scanning_) {
        return ErrorCode::SUCCESS;
    }
    
    // Start scanning (will be implemented when scanner coordinator is available)
    scanning_ = true;
    update_status_text();
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneScannerUI::stop_scanning() noexcept {
    if (!scanning_) {
        return ErrorCode::SUCCESS;
    }
    
    // Stop scanning (will be implemented when scanner coordinator is available)
    scanning_ = false;
    update_status_text();
    
    return ErrorCode::SUCCESS;
}

bool DroneScannerUI::is_scanning() const noexcept {
    return scanning_;
}

ErrorCode DroneScannerUI::set_scanning_mode(ScanningMode mode) noexcept {
    if (mode > ScanningMode::TARGETED) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    scanning_mode_ = mode;
    return ErrorCode::SUCCESS;
}

ScanningMode DroneScannerUI::get_scanning_mode() const noexcept {
    return scanning_mode_;
}

const DisplayData& DroneScannerUI::get_display_data() const noexcept {
    return display_data_;
}

// ============================================================================
// Drawing Methods
// ============================================================================

void DroneScannerUI::draw_scanner_header(Painter& painter) noexcept {
    // Will use ui::Painter when available
    (void)painter;
}

void DroneScannerUI::draw_scanner_status(Painter& painter, uint16_t start_y) noexcept {
    // Will use ui::Painter when available
    (void)painter;
    (void)start_y;
}

void DroneScannerUI::draw_threat_summary(Painter& painter, uint16_t start_y) noexcept {
    // Will use ui::Painter when available
    (void)painter;
    (void)start_y;
}

void DroneScannerUI::draw_control_buttons(Painter& painter, uint16_t start_y) noexcept {
    // Calculate button positions
    const uint16_t total_width = (3 * button_width_) + (2 * button_spacing_);
    const uint16_t start_x = (DISPLAY_WIDTH - total_width) / 2;
    
    // Draw Start/Stop button
    draw_button(
        painter,
        scanning_ ? "Stop" : "Start",
        start_x,
        start_y,
        button_width_,
        button_height_,
        true,
        selected_button_ == BUTTON_ID_START_STOP
    );
    
    // Draw Mode button
    draw_button(
        painter,
        "Mode",
        start_x + button_width_ + button_spacing_,
        start_y,
        button_width_,
        button_height_,
        true,
        selected_button_ == BUTTON_ID_MODE
    );
    
    // Draw Settings button
    draw_button(
        painter,
        "Setup",
        start_x + (2 * button_width_) + (2 * button_spacing_),
        start_y,
        button_width_,
        button_height_,
        true,
        selected_button_ == BUTTON_ID_SETTINGS
    );
}

void DroneScannerUI::draw_alert_overlay(Painter& painter) noexcept {
    // Will use ui::Painter when available
    (void)painter;
}

void DroneScannerUI::draw_error_overlay(Painter& painter) noexcept {
    // Will use ui::Painter when available
    (void)painter;
}

void DroneScannerUI::draw_button(
    Painter& painter,
    const char* label,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    bool enabled,
    bool selected
) noexcept {
    // Will use ui::Painter when available
    (void)painter;
    (void)label;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)enabled;
    (void)selected;
}

void DroneScannerUI::draw_text(
    Painter& painter,
    const char* text,
    uint16_t x,
    uint16_t y,
    uint32_t color
) noexcept {
    // Will use ui::Painter when available
    (void)painter;
    (void)text;
    (void)x;
    (void)y;
    (void)color;
}

void DroneScannerUI::draw_rectangle(
    Painter& painter,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint32_t color,
    bool fill
) noexcept {
    // Will use ui::Painter when available
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

void DroneScannerUI::update_status_text() noexcept {
    // Update status text based on scanning state
    // Will be implemented when text rendering is available
}

bool DroneScannerUI::is_alert_active() const noexcept {
    return alert_active_;
}

bool DroneScannerUI::is_error_active() const noexcept {
    return error_active_;
}

const char* DroneScannerUI::get_alert_message() const noexcept {
    return alert_message_;
}

const char* DroneScannerUI::get_error_message() const noexcept {
    return error_to_string(last_error_);
}

void DroneScannerUI::update_alert_timer(uint32_t elapsed_ms) noexcept {
    if (!alert_active_) {
        return;
    }
    
    // Check if alert duration has elapsed
    if (elapsed_ms >= alert_start_time_ + alert_duration_ms_) {
        clear_alert();
    }
}

void DroneScannerUI::update_error_timer(uint32_t elapsed_ms) noexcept {
    if (!error_active_) {
        return;
    }
    
    // Check if error duration has elapsed
    if (elapsed_ms >= error_start_time_ + error_duration_ms_) {
        clear_error();
    }
}

} // namespace drone_analyzer
