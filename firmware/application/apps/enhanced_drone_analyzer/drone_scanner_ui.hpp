#ifndef DRONE_SCANNER_UI_HPP
#define DRONE_SCANNER_UI_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "ui_widget.hpp"
#include "drone_types.hpp"
#include "audio_alerts.hpp"
#include "constants.hpp"
#include "error_handler.hpp"
#include "locking.hpp"

namespace ui {
class NavigationView;
}

namespace drone_analyzer {

/**
 * @brief Scanner UI component for drone analyzer
 * @note Inherits from ui::View
 * @note Simplified - removed wideband, hybrid, panoramic modes
 */
class DroneScannerUI : public ui::View {
public:
    /**
     * @brief Constructor
     * @param nav Navigation view reference
     */
    explicit DroneScannerUI(NavigationView& nav) noexcept;

    /**
     * @brief Destructor
     */
    ~DroneScannerUI() noexcept override;

    /**
     * @brief Paint method - render scanner UI
     * @param painter Painter instance for drawing
     */
    void paint(Painter& painter) override;

    /**
     * @brief Update display with new data
     * @param display_data New display data
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     */
    ErrorCode update_display(const DisplayData& display_data) noexcept;

    /**
     * @brief Handle user input
     * @param key Key code
     * @return ErrorCode::SUCCESS if handled, error code otherwise
     */
    ErrorCode handle_user_input(uint32_t key) noexcept;

    /**
     * @brief Show alert message
     * @param message Alert message
     * @param duration_ms Alert duration in milliseconds
     */
    void show_alert(const char* message, uint32_t duration_ms) noexcept;

    /**
     * @brief Show error message
     * @param error Error code
     * @param duration_ms Error duration in milliseconds
     */
    void show_error(ErrorCode error, uint32_t duration_ms) noexcept;

    /**
     * @brief Start scanning
     * @return ErrorCode::SUCCESS if started, error code otherwise
     */
    ErrorCode start_scanning() noexcept;

    /**
     * @brief Stop scanning
     * @return ErrorCode::SUCCESS if stopped, error code otherwise
     */
    ErrorCode stop_scanning() noexcept;

    /**
     * @brief Get scanning state
     * @return true if scanning, false otherwise
     */
    [[nodiscard]] bool is_scanning() const noexcept;

    /**
     * @brief Set scanning mode
     * @param mode Scanning mode
     * @return ErrorCode::SUCCESS if set, error code otherwise
     */
    ErrorCode set_scanning_mode(ScanningMode mode) noexcept;

    /**
     * @brief Get scanning mode
     * @return Current scanning mode
     */
    [[nodiscard]] ScanningMode get_scanning_mode() const noexcept;

    /**
     * @brief Get display data
     * @return Reference to current display data
     */
    [[nodiscard]] const DisplayData& get_display_data() const noexcept;

    /**
     * @brief Clear alert
     */
    void clear_alert() noexcept;

    /**
     * @brief Clear error
     */
    void clear_error() noexcept;
    
    /**
     * @brief Handle audio alert
     * @param alert_type Alert type
     * @param priority Alert priority (0=LOW, 1=MEDIUM, 2=HIGH, 3=CRITICAL)
     */
    void on_alert(AlertType alert_type, uint8_t priority) noexcept;
    
private:
    /**
     * @brief Draw scanner header
     * @param painter Painter instance for drawing
     */
    void draw_scanner_header(Painter& painter) noexcept;

    /**
     * @brief Draw scanner status
     * @param painter Painter instance for drawing
     * @param start_y Starting Y coordinate
     */
    void draw_scanner_status(Painter& painter, uint16_t start_y) noexcept;

    /**
     * @brief Draw threat summary
     * @param painter Painter instance for drawing
     * @param start_y Starting Y coordinate
     */
    void draw_threat_summary(Painter& painter, uint16_t start_y) noexcept;

    /**
     * @brief Draw control buttons
     * @param painter Painter instance for drawing
     * @param start_y Starting Y coordinate
     */
    void draw_control_buttons(Painter& painter, uint16_t start_y) noexcept;

    /**
     * @brief Draw alert overlay
     * @param painter Painter instance for drawing
     */
    void draw_alert_overlay(Painter& painter) noexcept;

    /**
     * @brief Draw error overlay
     * @param painter Painter instance for drawing
     */
    void draw_error_overlay(Painter& painter) noexcept;

    /**
     * @brief Draw button
     * @param painter Painter instance for drawing
     * @param label Button label
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Button width
     * @param height Button height
     * @param enabled Button enabled state
     * @param selected Button selected state
     */
    void draw_button(
        Painter& painter,
        const char* label,
        uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height,
        bool enabled,
        bool selected
    ) noexcept;

    /**
     * @brief Draw text
     * @param painter Painter instance for drawing
     * @param text Text to draw
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Text color
     */
    void draw_text(
        Painter& painter,
        const char* text,
        uint16_t x,
        uint16_t y,
        uint32_t color
    ) noexcept;

    /**
     * @brief Draw rectangle
     * @param painter Painter instance for drawing
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Rectangle width
     * @param height Rectangle height
     * @param color Rectangle color
     * @param fill Fill rectangle if true, outline only if false
     */
    void draw_rectangle(
        Painter& painter,
        uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height,
        uint32_t color,
        bool fill = true
    ) noexcept;

    /**
     * @brief Process key input
     * @param key Key code
     * @return ErrorCode::SUCCESS if processed, error code otherwise
     */
    [[nodiscard]] ErrorCode process_key_input(uint32_t key) noexcept;

    /**
     * @brief Handle start/stop key
     * @return ErrorCode::SUCCESS if handled, error code otherwise
     */
    [[nodiscard]] ErrorCode handle_start_stop_key() noexcept;

    /**
     * @brief Handle mode key
     * @return ErrorCode::SUCCESS if handled, error code otherwise
     */
    [[nodiscard]] ErrorCode handle_mode_key() noexcept;

    /**
     * @brief Handle settings key
     * @return ErrorCode::SUCCESS if handled, error code otherwise
     */
    [[nodiscard]] ErrorCode handle_settings_key() noexcept;

    /**
     * @brief Update status text based on scanning state
     */
    void update_status_text() noexcept;

    /**
     * @brief Check if alert is active
     * @return true if alert active, false otherwise
     */
    [[nodiscard]] bool is_alert_active() const noexcept;

    /**
     * @brief Check if error is active
     * @return true if error active, false otherwise
     */
    [[nodiscard]] bool is_error_active() const noexcept;

    /**
     * @brief Get alert message
     * @return Alert message string
     */
    [[nodiscard]] const char* get_alert_message() const noexcept;

    /**
     * @brief Get error message
     * @return Error message string
     */
    [[nodiscard]] const char* get_error_message() const noexcept;

    /**
     * @brief Update alert timer
     * @param elapsed_ms Elapsed time in milliseconds
     */
    void update_alert_timer(uint32_t elapsed_ms) noexcept;

    /**
     * @brief Update error timer
     * @param elapsed_ms Elapsed time in milliseconds
     */
    void update_error_timer(uint32_t elapsed_ms) noexcept;

private:
    // Display data
    DisplayData display_data_;

    // Scanning state
    bool scanning_;
    ScanningMode scanning_mode_;

    // Alert state
    bool alert_active_;
    char alert_message_[MAX_TEXT_LENGTH];
    uint32_t alert_start_time_;
    uint32_t alert_duration_ms_;

    // Error state
    bool error_active_;
    ErrorCode last_error_;
    uint32_t error_start_time_;
    uint32_t error_duration_ms_;

    // UI state
    uint16_t selected_button_;
    bool settings_visible_;

    // UI dimensions
    uint16_t header_height_;
    uint16_t status_height_;
    uint16_t threat_summary_height_;
    uint16_t control_buttons_height_;
    uint16_t button_width_;
    uint16_t button_height_;
    uint16_t button_spacing_;

    // Button IDs
    static constexpr uint32_t BUTTON_ID_START_STOP = 1;
    static constexpr uint32_t BUTTON_ID_MODE = 2;
    static constexpr uint32_t BUTTON_ID_SETTINGS = 3;
};

} // namespace drone_analyzer

#endif // DRONE_SCANNER_UI_HPP
