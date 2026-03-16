#include "drone_scanner_ui.hpp"
#include "ui.hpp"
#include "scanner.hpp"
#include "database.hpp"
#include <cstring>
#include <cstdio>
#include <cinttypes>

namespace drone_analyzer {

constexpr uint32_t KEY_START_STOP = 0x01;
constexpr uint32_t KEY_MODE = 0x02;
constexpr uint32_t KEY_SETTINGS = 0x03;

constexpr uint16_t LNA_X = 4;
constexpr uint16_t LNA_Y = 0;
constexpr uint16_t VGA_X = 11;
constexpr uint16_t VGA_Y = 0;
constexpr uint16_t RFAMP_X = 18;
constexpr uint16_t RFAMP_Y = 0;

// ============================================================================
// Constructor / Destructor
// ============================================================================

DroneScannerUI::DroneScannerUI(NavigationView& nav) noexcept
    : ui::View()
    , nav_(nav)
    , hardware_()
    , database_()
    , scanner_(database_, hardware_)
    , radio_state_{ReceiverModel::Mode::SpectrumAnalysis}
    , big_display_{{4, 6 * 16, 28 * 8, 52}, 0}
    , field_lna_{Point{LNA_X, LNA_Y}}
    , field_vga_{Point{VGA_X, VGA_Y}}
    , field_rf_amp_{Point{RFAMP_X, RFAMP_Y}}
    , message_handler_retune_{
          Message::ID::Retune,
          [this](Message* const p) {
              const auto msg = *reinterpret_cast<const RetuneMessage*>(p);
              this->handle_retune(msg.freq, msg.range);
          }}
    , message_handler_stats_{
          Message::ID::ChannelStatistics,
          [this](Message* const p) {
              const auto msg = *reinterpret_cast<const ChannelStatisticsMessage*>(p);
              this->handle_statistics(msg.statistics);
          }}
    , current_frequency_(0)
    , current_rssi_(-120)
    , current_scanner_state_(ScannerState::IDLE)
    , displayed_drone_type_{'\0'}
    , drone_type_display_timer_(0)
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

    // Load DRONES.TXT from freqman
    ErrorCode err = database_.load_from_freqman_file("DRONES");
    if (err != ErrorCode::SUCCESS) {
        show_error(err, 3000);
    }

    // Initialize scanner
    err = scanner_.initialize();
    if (err != ErrorCode::SUCCESS) {
        show_error(err, 3000);
        return;
    }

    // Add children to UI tree
    add_children({
        &field_lna_,
        &field_vga_,
        &field_rf_amp_,
        &big_display_,
    });
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
// Message Handlers
// ============================================================================

void DroneScannerUI::handle_retune(int64_t freq, uint32_t range) noexcept {
    (void)range;  // Unused parameter (matches RetuneMessage structure)
    current_frequency_ = freq;

    // Update scanner thread state
    scanner_.set_freq_lock_count(0);  // Reset lock for new frequency

    // Update UI state
    update_ui_state();
}

void DroneScannerUI::handle_statistics(const ChannelStatistics& statistics) noexcept {
    current_rssi_ = statistics.max_db;

    // Update UI state based on scanner state and RSSI
    update_ui_state();
}

void DroneScannerUI::bigdisplay_update(BigDisplayColor color) noexcept {
    // Set BigFrequency style based on color
    switch (color) {
        case BigDisplayColor::GREY:
            big_display_.set_style(Theme::getInstance()->fg_medium);
            break;
        case BigDisplayColor::YELLOW:
            big_display_.set_style(Theme::getInstance()->fg_yellow);
            break;
        case BigDisplayColor::GREEN:
            big_display_.set_style(Theme::getInstance()->fg_green);
            break;
        case BigDisplayColor::RED:
            big_display_.set_style(Theme::getInstance()->fg_red);
            break;
        default:
            break;
    }

    // Update frequency display
    big_display_.set(current_frequency_);
}

void DroneScannerUI::update_ui_state() noexcept {
    // Get current scanner state
    current_scanner_state_ = scanner_.get_state();

    // Determine BigFrequency color
    BigDisplayColor color = BigDisplayColor::GREY;

    switch (current_scanner_state_) {
        case ScannerState::SCANNING:
            color = BigDisplayColor::GREY;
            break;
        case ScannerState::LOCKING:
            color = BigDisplayColor::YELLOW;
            break;
        case ScannerState::TRACKING:
            // Determine color by RSSI
            if (current_rssi_ >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM) {
                color = BigDisplayColor::RED;
            } else {
                color = BigDisplayColor::GREEN;
            }
            break;
        default:
            color = BigDisplayColor::GREY;
            break;
    }

    bigdisplay_update(color);

    // Show drone type if in LOCKING state
    if (current_scanner_state_ == ScannerState::LOCKING) {
        // Get drone type from scanner
        const char* drone_type = scanner_.get_current_drone_type();

        if (drone_type[0] != '\0') {
            // Safe copy with bounds check
            size_t copy_len = 0;
            while (copy_len < MAX_DRONE_TYPE_DISPLAY && drone_type[copy_len] != '\0') {
                displayed_drone_type_[copy_len] = drone_type[copy_len];
                copy_len++;
            }
            displayed_drone_type_[copy_len] = '\0';

            // Start display timer (500ms)
            drone_type_display_timer_ = chTimeNow();
        }
    } else {
        // Clear drone type in other states
        displayed_drone_type_[0] = '\0';
        drone_type_display_timer_ = 0;
    }
}

// ============================================================================
// Drawing Methods
// ============================================================================

void DroneScannerUI::draw_scanner_header(Painter& painter) noexcept {
    // BigFrequency already added as child widget, no need to draw
    // Check drone type display timer (uint32_t overflow-safe)
    if (drone_type_display_timer_ != 0) {
        SystemTime now = chTimeNow();
        
        // Use uint32_t wrap-safe subtraction
        const uint32_t elapsed = now - drone_type_display_timer_;
        if (elapsed >= DRONE_TYPE_DISPLAY_DURATION_MS) {
            // Timer expired, clear drone type
            displayed_drone_type_[0] = '\0';
            drone_type_display_timer_ = 0;
        }
    }

    // Draw drone type next to BigFrequency if present
    if (displayed_drone_type_[0] != '\0') {
        // Position: right of BigFrequency
        // BigFrequency position: {4, 6 * 16, 28 * 8, 52}
        // Width = 28 * 8 = 224 pixels
        // X position: 4 + 224 + 5 = 233

        uint16_t drone_type_x = 4 + (28 * 8) + 5;
        uint16_t drone_type_y = 6 * 16 + 20;  // Shift down 20px

        // Red color for drone type
        const auto& drone_type_style = *Theme::getInstance()->fg_red;

        draw_text(painter, displayed_drone_type_, drone_type_x, drone_type_y, drone_type_style);
    }
}

void DroneScannerUI::draw_scanner_status(Painter& painter, uint16_t start_y) noexcept {
    // Get text style from theme
    const auto& text_style = *Theme::getInstance()->bg_darkest_small;

    // Draw RSSI
    char rssi_text[32];
    snprintf(rssi_text, sizeof(rssi_text), "RSSI: %d dBm", static_cast<int>(current_rssi_));
    draw_text(painter, rssi_text, 5, start_y + 100, text_style);

    // Draw scanner state
    const char* state_text = "IDLE";
    switch (current_scanner_state_) {
        case ScannerState::SCANNING:
            state_text = "SCANNING";
            break;
        case ScannerState::LOCKING:
            state_text = "LOCKING";
            break;
        case ScannerState::TRACKING:
            state_text = "TRACKING";
            break;
        case ScannerState::PAUSED:
            state_text = "PAUSED";
            break;
        default:
            state_text = "IDLE";
    }

    draw_text(painter, state_text, 5, start_y + 120, text_style);

    // Draw progress (index/total)
    if (scanner_.is_scanning()) {
        char freq_info[64];
        size_t total = database_.get_database_size();

        snprintf(freq_info, sizeof(freq_info), "?/%zu", total);
        draw_text(painter, freq_info, 5, start_y + 140, text_style);
    }
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
    const ui::Style& style
) noexcept {
    if (text == nullptr || text[0] == '\0') {
        return;
    }
    // Draw string directly (no std::string_view needed for C-string)
    painter.draw_string(Point{x, y}, style, text);
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
