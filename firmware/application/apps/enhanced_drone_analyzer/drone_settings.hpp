#ifndef DRONE_SETTINGS_HPP
#define DRONE_SETTINGS_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "drone_types.hpp"
#include "constants.hpp"
#include "error_handler.hpp"

#include "ui_painter.hpp"
#include "ui_widget.hpp"

namespace drone_analyzer {

/**
 * @brief Settings structure for drone analyzer
 * @note Simplified - removed translation system, preset system, about dialog
 */
struct DroneSettings {
    // Scanning settings
    ScanningMode scanning_mode;
    uint32_t scan_interval_ms;
    uint8_t scan_sensitivity;
    
    // Display settings
    bool spectrum_visible;
    bool histogram_visible;
    bool drone_list_visible;
    bool status_bar_visible;
    
    // Alert settings
    bool audio_alerts_enabled;
    int32_t alert_rssi_threshold_dbm;
    
    // Threat settings
    ThreatLevel min_threat_level;
    
    /**
     * @brief Default constructor
     */
    DroneSettings() noexcept;
    
    /**
     * @brief Reset to defaults
     */
    void reset_to_defaults() noexcept;
};

/**
 * @brief Settings UI component for drone analyzer
 * @note Inherits from ui::View
 * @note Simplified - removed translation system, preset system, about dialog
 */
class DroneSettingsView : public ui::View {
public:
    /**
     * @brief Default constructor
     */
    DroneSettingsView() noexcept;

    /**
     * @brief Destructor
     */
    ~DroneSettingsView() noexcept override;

/**
 * @brief Paint method - render settings UI
 * @param painter Painter instance for drawing
 */
void paint(ui::Painter& painter) override;

    /**
     * @brief Setup settings UI components
     * @note Creates UI controls for all settings
     */
    void setup_settings_ui() noexcept;

    /**
     * @brief Handle setting change
     * @param setting_id Setting identifier
     * @param value New value
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     */
    ErrorCode on_setting_change(uint32_t setting_id, int32_t value) noexcept;

    /**
     * @brief Save settings to file
     * @return ErrorCode::SUCCESS if saved, error code otherwise
     */
    ErrorCode save_settings() noexcept;

    /**
     * @brief Load settings from file
     * @return ErrorCode::SUCCESS if loaded, error code otherwise
     */
    ErrorCode load_settings() noexcept;

    /**
     * @brief Get current settings
     * @return Reference to current settings
     */
    [[nodiscard]] const DroneSettings& get_settings() const noexcept;

    /**
     * @brief Set settings
     * @param settings New settings
     */
    void set_settings(const DroneSettings& settings) noexcept;

    /**
     * @brief Reset settings to defaults
     */
    void reset_settings() noexcept;

    /**
     * @brief Validate settings
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_settings() const noexcept;

private:
    /**
     * @brief Draw settings header
     * @param painter Painter instance for drawing
     */
    void draw_settings_header(ui::Painter& painter) noexcept;

    /**
     * @brief Draw scanning settings
     * @param painter Painter instance for drawing
     * @param start_y Starting Y coordinate
     */
    void draw_scanning_settings(ui::Painter& painter, uint16_t start_y) noexcept;

    /**
     * @brief Draw display settings
     * @param painter Painter instance for drawing
     * @param start_y Starting Y coordinate
     */
    void draw_display_settings(ui::Painter& painter, uint16_t start_y) noexcept;

    /**
     * @brief Draw alert settings
     * @param painter Painter instance for drawing
     * @param start_y Starting Y coordinate
     */
    void draw_alert_settings(ui::Painter& painter, uint16_t start_y) noexcept;

    /**
     * @brief Draw threat settings
     * @param painter Painter instance for drawing
     * @param start_y Starting Y coordinate
     */
    void draw_threat_settings(ui::Painter& painter, uint16_t start_y) noexcept;

    /**
     * @brief Draw setting row
     * @param painter Painter instance for drawing
     * @param label Setting label
     * @param value Setting value
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Row width
     * @param height Row height
     */
    void draw_setting_row(
        ui::Painter& painter,
        const char* label,
        const char* value,
        uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height
    ) noexcept;

    /**
     * @brief Draw checkbox
     * @param painter Painter instance for drawing
     * @param checked Checkbox state
     * @param x X coordinate
     * @param y Y coordinate
     * @param size Checkbox size
     */
    void draw_checkbox(
        ui::Painter& painter,
        bool checked,
        uint16_t x,
        uint16_t y,
        uint16_t size
    ) noexcept;

    /**
     * @brief Draw button
     * @param painter Painter instance for drawing
     * @param label Button label
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Button width
     * @param height Button height
     * @param enabled Button enabled state
     */
    void draw_button(
        ui::Painter& painter,
        const char* label,
        uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height,
        bool enabled
    ) noexcept;

    /**
     * @brief Get scanning mode name
     * @param mode Scanning mode
     * @return Mode name string
     */
    [[nodiscard]] const char* get_scanning_mode_name(ScanningMode mode) const noexcept;

    /**
     * @brief Get threat level name
     * @param threat Threat level
     * @return Threat level name string
     */
    [[nodiscard]] const char* get_threat_level_name(ThreatLevel threat) const noexcept;

    /**
     * @brief Format value string
     * @param value Value to format
     * @param buffer Buffer to store formatted string
     * @param buffer_size Buffer size
     */
    void format_value_string(
        int32_t value,
        char* buffer,
        size_t buffer_size
    ) const noexcept;

    /**
     * @brief Parse settings file line
     * @param line Line to parse
     * @param key Output key
     * @param value Output value
     * @return true if parsed successfully, false otherwise
     */
    [[nodiscard]] bool parse_settings_line(
        const char* line,
        char* key,
        char* value
    ) const noexcept;

    /**
     * @brief Write settings file line
     * @param key Setting key
     * @param value Setting value
     * @param buffer Buffer to store formatted line
     * @param buffer_size Buffer size
     */
    void write_settings_line(
        const char* key,
        const char* value,
        char* buffer,
        size_t buffer_size
    ) const noexcept;

    /**
     * @brief Apply settings
     * @note Called after settings are loaded or changed
     */
    void apply_settings() noexcept;

    /**
     * @brief Notify settings changed
     * @note Callback for settings change notification
     */
    void notify_settings_changed() noexcept;

private:
    // Current settings
    DroneSettings settings_;

    // Settings file path
    const char* settings_file_path_;

    // Settings loaded flag
    bool settings_loaded_;

    // Settings dirty flag
    bool settings_dirty_;

    // UI component positions
    uint16_t header_height_;
    uint16_t section_spacing_;
    uint16_t row_height_;
    uint16_t checkbox_size_;
    uint16_t button_height_;

    // Setting IDs
    static constexpr uint32_t SETTING_ID_SCANNING_MODE = 1;
    static constexpr uint32_t SETTING_ID_SCAN_INTERVAL = 2;
    static constexpr uint32_t SETTING_ID_SCAN_SENSITIVITY = 3;
    static constexpr uint32_t SETTING_ID_SPECTRUM_VISIBLE = 4;
    static constexpr uint32_t SETTING_ID_HISTOGRAM_VISIBLE = 5;
    static constexpr uint32_t SETTING_ID_DRONE_LIST_VISIBLE = 6;
    static constexpr uint32_t SETTING_ID_STATUS_BAR_VISIBLE = 7;
    static constexpr uint32_t SETTING_ID_AUDIO_ALERTS = 8;
    static constexpr uint32_t SETTING_ID_ALERT_RSSI_THRESHOLD = 9;
    static constexpr uint32_t SETTING_ID_MIN_THREAT_LEVEL = 10;
};

} // namespace drone_analyzer

#endif // DRONE_SETTINGS_HPP
