#include "drone_scanner_ui.hpp"
#include "ui.hpp"
#include "scanner.hpp"
#include "database.hpp"
#include <cstring>
#include <cinttypes>
#include <cstdio>
#include <new>
#include <type_traits>

namespace drone_analyzer {

// Static buffers for large objects (off-stack, file scope)
// These are constructed once at startup and reused for the lifetime of the application
// Size verification ensures buffer overflow protection at compile time
alignas(HardwareController) static uint8_t s_hardware_buffer[sizeof(HardwareController)];
static_assert(sizeof(HardwareController) <= sizeof(s_hardware_buffer), "HardwareController buffer overflow risk");

alignas(DatabaseManager) static uint8_t s_database_buffer[sizeof(DatabaseManager)];
static_assert(sizeof(DatabaseManager) <= sizeof(s_database_buffer), "DatabaseManager buffer overflow risk");

alignas(DroneScanner) static uint8_t s_scanner_buffer[sizeof(DroneScanner)];
static_assert(sizeof(DroneScanner) <= sizeof(s_scanner_buffer), "DroneScanner buffer overflow risk");

alignas(DisplayData) static uint8_t s_display_data_buffer[sizeof(DisplayData)];
static_assert(sizeof(DisplayData) <= sizeof(s_display_data_buffer), "DisplayData buffer overflow risk");

void DroneScannerUI::construct_objects() noexcept {
    hardware_ptr_ = new(&s_hardware_buffer[0]) HardwareController();
    database_ptr_ = new(&s_database_buffer[0]) DatabaseManager();
    scanner_ptr_ = new(&s_scanner_buffer[0]) DroneScanner(*database_ptr_, *hardware_ptr_);
    display_data_ptr_ = new(&s_display_data_buffer[0]) DisplayData();
}

void DroneScannerUI::destruct_objects() noexcept {
    if (display_data_ptr_ != nullptr) {
        display_data_ptr_->~DisplayData();
        display_data_ptr_ = nullptr;
    }
    if (scanner_ptr_ != nullptr) {
        scanner_ptr_->~DroneScanner();
        scanner_ptr_ = nullptr;
    }
    if (database_ptr_ != nullptr) {
        database_ptr_->~DatabaseManager();
        database_ptr_ = nullptr;
    }
    if (hardware_ptr_ != nullptr) {
        hardware_ptr_->~HardwareController();
        hardware_ptr_ = nullptr;
    }
}

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
    , hardware_ptr_(nullptr)
    , database_ptr_(nullptr)
    , scanner_ptr_(nullptr)
    , radio_state_{ReceiverModel::Mode::SpectrumAnalysis}
    , big_display_{{4, 6 * 16, 28 * 8, 52}, 0}
    , field_lna_{Point{LNA_X, LNA_Y}}
    , field_vga_{Point{VGA_X, VGA_Y}}
    , field_rf_amp_{Point{RFAMP_X, RFAMP_Y}}
    , current_frequency_(0)
    , current_rssi_(RSSI_NOISE_FLOOR_DBM)
    , current_scanner_state_(ScannerState::IDLE)
    , displayed_drone_type_{'\0'}
    , drone_type_display_timer_(0)
    , display_data_ptr_(nullptr)
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

    construct_objects();

    if (display_data_ptr_ != nullptr) {
        display_data_ptr_->clear();
    }

    if (scanner_ptr_ != nullptr) {
        ErrorCode err = scanner_ptr_->initialize();
        if (err != ErrorCode::SUCCESS) {
            show_error(err, ERROR_DURATION_MS);
            destruct_objects();
            return;
        }
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
    destruct_objects();
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

    SystemTime current_time = chTimeNow();
    update_alert_timer(current_time);
    update_error_timer(current_time);
}

// ============================================================================
// Display Update
// ============================================================================

ErrorCode DroneScannerUI::update_display(const DisplayData& display_data) noexcept {
    if (display_data_ptr_ == nullptr) {
        return ErrorCode::INVALID_PARAMETER;
    }

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
    *display_data_ptr_ = display_data;

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
        case BUTTON_ID_START_STOP:
            return handle_start_stop_key();

        case BUTTON_ID_MODE:
            return handle_mode_key();

        case BUTTON_ID_SETTINGS:
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

    const ErrorCode copy_err = safe_str_copy(alert_message_, MAX_TEXT_LENGTH, message);
    if (copy_err != ErrorCode::SUCCESS) {
        alert_message_[0] = '\0';
    }

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
// Message Handlers
// ============================================================================

ErrorCode DroneScannerUI::start_scanning() noexcept {
    if (scanner_ptr_ == nullptr) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }

    if (scanning_) {
        return ErrorCode::SUCCESS;
    }

    ErrorCode err = scanner_ptr_->start_scanning();
    if (err == ErrorCode::SUCCESS) {
        scanning_ = true;
        update_status_text();
    }

    return err;
}

ErrorCode DroneScannerUI::stop_scanning() noexcept {
    if (scanner_ptr_ == nullptr) {
        return ErrorCode::HARDWARE_NOT_INITIALIZED;
    }

    if (!scanning_) {
        return ErrorCode::SUCCESS;
    }

    ErrorCode err = scanner_ptr_->stop_scanning();
    if (err == ErrorCode::SUCCESS) {
        scanning_ = false;
        update_status_text();
    }

    return err;
}

bool DroneScannerUI::is_scanning() const noexcept {
    return scanning_;
}

ErrorCode DroneScannerUI::set_scanning_mode(ScanningMode mode) noexcept {
    if (static_cast<uint8_t>(mode) < static_cast<uint8_t>(ScanningMode::SINGLE) ||
        static_cast<uint8_t>(mode) > static_cast<uint8_t>(ScanningMode::TARGETED)) {
        return ErrorCode::INVALID_PARAMETER;
    }

    scanning_mode_ = mode;
    return ErrorCode::SUCCESS;
}

ScanningMode DroneScannerUI::get_scanning_mode() const noexcept {
    return scanning_mode_;
}

const DisplayData& DroneScannerUI::get_display_data() const noexcept {
    static DisplayData empty_data;
    
    if (scanner_ptr_ == nullptr || display_data_ptr_ == nullptr) {
        return empty_data;
    }
    
    display_data_ptr_->clear();
    
    TrackedDrone tracked_drones[MAX_TRACKED_DRONES];
    const size_t drone_count = scanner_ptr_->get_tracked_drones(tracked_drones, MAX_TRACKED_DRONES);
    
    const size_t copy_count = drone_count < MAX_DISPLAYED_DRONES ? drone_count : MAX_DISPLAYED_DRONES;
    
    for (size_t i = 0; i < copy_count; ++i) {
        display_data_ptr_->drones[i] = DisplayDroneEntry(tracked_drones[i]);
    }
    
    display_data_ptr_->drone_count = copy_count;
    
    display_data_ptr_->state.scanning_active = scanner_ptr_->is_scanning();
    display_data_ptr_->state.is_fresh = true;
    
    display_data_ptr_->state.max_detected_threat = ThreatLevel::NONE;
    display_data_ptr_->state.approaching_count = 0;
    display_data_ptr_->state.static_count = 0;
    display_data_ptr_->state.receding_count = 0;
    
    for (size_t i = 0; i < copy_count; ++i) {
        if (display_data_ptr_->drones[i].threat > display_data_ptr_->state.max_detected_threat) {
            display_data_ptr_->state.max_detected_threat = display_data_ptr_->drones[i].threat;
        }
        
        switch (display_data_ptr_->drones[i].trend) {
            case MovementTrend::APPROACHING:
                display_data_ptr_->state.approaching_count++;
                break;
            case MovementTrend::STATIC:
                display_data_ptr_->state.static_count++;
                break;
            case MovementTrend::RECEDING:
                display_data_ptr_->state.receding_count++;
                break;
            default:
                break;
        }
    }
    
    return *display_data_ptr_;
}

// ============================================================================
// UI Updates
// ============================================================================

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
    if (scanner_ptr_ == nullptr) {
        current_scanner_state_ = ScannerState::IDLE;
        current_rssi_ = RSSI_NOISE_FLOOR_DBM;
        current_frequency_ = 0;
        bigdisplay_update(BigDisplayColor::GREY);
        displayed_drone_type_[0] = '\0';
        drone_type_display_timer_ = 0;
        return;
    }
    
    current_scanner_state_ = scanner_ptr_->get_state();
    
    current_rssi_ = RSSI_NOISE_FLOOR_DBM;
    current_frequency_ = 0;
    
    TrackedDrone tracked_drones[MAX_TRACKED_DRONES];
    const size_t drone_count = scanner_ptr_->get_tracked_drones(tracked_drones, MAX_TRACKED_DRONES);
    
    if (drone_count > 0) {
        current_rssi_ = tracked_drones[0].rssi;
        current_frequency_ = tracked_drones[0].frequency;
    } else {
        const ErrorResult<FreqHz> freq_result = scanner_ptr_->get_current_frequency();
        if (freq_result.has_value()) {
            current_frequency_ = freq_result.value();
        }
    }
    
    BigDisplayColor color = BigDisplayColor::GREY;
    switch (current_scanner_state_) {
        case ScannerState::SCANNING:
            color = BigDisplayColor::GREY;
            break;
        case ScannerState::LOCKING:
            color = BigDisplayColor::YELLOW;
            break;
        case ScannerState::TRACKING:
            color = (current_rssi_ >= RSSI_CRITICAL_THREAT_THRESHOLD_DBM) 
                  ? BigDisplayColor::RED 
                  : BigDisplayColor::GREEN;
            break;
        default:
            color = BigDisplayColor::GREY;
            break;
    }
    
    bigdisplay_update(color);
    
    if (current_scanner_state_ == ScannerState::LOCKING) {
        const char* drone_type = scanner_ptr_->get_current_drone_type();
        
        if (drone_type != nullptr && drone_type[0] != '\0') {
            const ErrorCode copy_err = safe_str_copy(displayed_drone_type_, MAX_DRONE_TYPE_DISPLAY, drone_type);
            if (copy_err == ErrorCode::SUCCESS) {
                drone_type_display_timer_ = chTimeNow();
            }
        }
    } else {
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
        uint16_t drone_type_x = BIG_FREQUENCY_X + BIG_FREQUENCY_WIDTH + DRONE_TYPE_SPACING;
        uint16_t drone_type_y = BIG_FREQUENCY_Y + DRONE_TYPE_Y_OFFSET;

        // Red color for drone type
        const auto& drone_type_style = *Theme::getInstance()->fg_red;

        draw_text(painter, displayed_drone_type_, drone_type_x, drone_type_y, drone_type_style);
    }
}

void DroneScannerUI::draw_scanner_status(Painter& painter, uint16_t start_y) noexcept {
    const auto& text_style = *Theme::getInstance()->bg_darkest_small;

    draw_text(painter, "RSSI:", RSSI_TEXT_X, start_y + RSSI_TEXT_Y_OFFSET, text_style);

    const auto rssi_str = to_string_dec_int(current_rssi_, 5);
    draw_text(painter, rssi_str.c_str(), RSSI_VALUE_X, start_y + RSSI_TEXT_Y_OFFSET, text_style);
    draw_text(painter, "dBm", RSSI_DBM_X_BASE + rssi_str.length() * FONT_WIDTH, start_y + RSSI_TEXT_Y_OFFSET, text_style);

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

    draw_text(painter, state_text, STATE_TEXT_X, start_y + STATE_TEXT_Y_OFFSET, text_style);

    if (scanner_ptr_ != nullptr && scanner_ptr_->is_scanning()) {
        draw_text(painter, "Scanning...", STATE_TEXT_X, start_y + SCANNING_TEXT_Y_OFFSET, text_style);
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
    if (!alert_active_) {
        return;
    }

    const auto& style = *Theme::getInstance()->fg_red;

    draw_rectangle(painter, ALERT_X, ALERT_Y, ALERT_W, ALERT_H, COLOR_CRITICAL_THREAT, true);

    draw_text(painter, "ALERT", ALERT_X + ALERT_TEXT_OFFSET_X, ALERT_Y + ALERT_TEXT_OFFSET_Y, style);
    draw_text(painter, alert_message_, ALERT_X + ALERT_TEXT_OFFSET_X, ALERT_Y + ALERT_MESSAGE_TEXT_OFFSET_Y, style);
}

void DroneScannerUI::draw_error_overlay(Painter& painter) noexcept {
    if (!error_active_) {
        return;
    }

    const auto& style = *Theme::getInstance()->fg_yellow;

    draw_rectangle(painter, ERROR_X, ERROR_Y, ERROR_W, ERROR_H, COLOR_HIGH_THREAT, true);

    draw_text(painter, "ERROR", ERROR_X + ALERT_TEXT_OFFSET_X, ERROR_Y + ALERT_TEXT_OFFSET_Y, style);
    draw_text(painter, error_to_string(last_error_), ERROR_X + ALERT_TEXT_OFFSET_X, ERROR_Y + ALERT_MESSAGE_TEXT_OFFSET_Y, style);
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
    if (label == nullptr) {
        return;
    }

    const uint32_t bg_color = selected ? BUTTON_BG_SELECTED : (enabled ? BUTTON_BG_ENABLED : BUTTON_BG_DISABLED);

    draw_rectangle(painter, x, y, width, height, bg_color, true);

    if (enabled) {
        const auto& style = *Theme::getInstance()->bg_darkest_small;

        size_t label_len = 0;
        while (label[label_len] != '\0') {
            label_len++;
        }

        const uint16_t text_x = x + (width - static_cast<uint16_t>(label_len) * FONT_WIDTH) / 2;
        const uint16_t text_y = y + (height - FONT_HEIGHT) / 2;
        draw_text(painter, label, text_x, text_y, style);
    }
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
    if (width == 0 || height == 0) {
        return;
    }

    if (fill) {
        painter.fill_rectangle({x, y, width, height}, color);
    } else {
        painter.draw_rectangle({x, y, width, height}, color);
    }
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

void DroneScannerUI::update_alert_timer(uint32_t current_time_ms) noexcept {
    if (!alert_active_) {
        return;
    }

    const uint32_t elapsed = current_time_ms - alert_start_time_;
    if (elapsed >= alert_duration_ms_) {
        clear_alert();
    }
}

void DroneScannerUI::update_error_timer(uint32_t current_time_ms) noexcept {
    if (!error_active_) {
        return;
    }

    const uint32_t elapsed = current_time_ms - error_start_time_;
    if (elapsed >= error_duration_ms_) {
        clear_error();
    }
}

} // namespace drone_analyzer
