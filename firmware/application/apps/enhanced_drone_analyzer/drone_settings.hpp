#ifndef DRONE_SETTINGS_HPP
#define DRONE_SETTINGS_HPP

#include <cstdint>
#include <cstddef>
#include <string>
#include "drone_types.hpp"
#include "constants.hpp"
#include "scanner.hpp"

#include "ui_painter.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"

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

    // Advanced detection features (OFF by default)
    bool dwell_enabled;
    bool confirm_count_enabled;
    bool noise_blacklist_enabled;
    
    // Sweep frequency range (Hz)
    FreqHz sweep_start_freq;
    FreqHz sweep_end_freq;

    // Sweep step (Hz)
    FreqHz sweep_step_freq;
    
    /**
      * @brief Default constructor
      */
    DroneSettings() noexcept;

    /**
      * @brief Reset settings to defaults
      */
    void reset_to_defaults() noexcept;
};

/**
 * @brief Settings UI component for drone analyzer
 * @note Inherits from ui::View
 * @note Simplified - removed translation system, preset system, about dialog
 */
class DroneScanner;
class DroneDisplay;

class DroneSettingsView : public ui::View {
public:
    explicit DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display = nullptr) noexcept;

    /**
     * @brief Destructor
     */
    ~DroneSettingsView() noexcept override;

    /**
     * @brief Delete copy constructor (non-copyable)
     */
    DroneSettingsView(const DroneSettingsView&) = delete;

    /**
     * @brief Delete copy assignment (non-copyable)
     */
    DroneSettingsView& operator=(const DroneSettingsView&) = delete;

    /**
     * @brief Paint method - render settings UI
     * @param painter Painter instance for drawing
     */
    void paint(ui::Painter& painter) override;

    /**
     * @brief Reset settings to defaults
     */
    void reset_settings() noexcept;

    /**
     * @brief Load settings from SD card file
     * @return ErrorCode::SUCCESS if loaded, error code otherwise
     */
    [[nodiscard]] ErrorCode load_settings() noexcept;

    /**
     * @brief Validate settings
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_settings() const noexcept;

    /**
     * @brief Focus handler - sets focus to first field
     */
    void focus() override;

    bool on_touch(TouchEvent event) override;

    std::string title() const override { return "EDA Settings"; }

private:
    ui::Labels labels_;
    ui::OptionsField field_scan_mode_;
    ui::NumberField field_scan_interval_;
    ui::NumberField field_rssi_threshold_;
    ui::NumberField field_volume_;
    ui::Checkbox check_audio_alerts_;
    ui::Checkbox check_spectrum_visible_;
    ui::Checkbox check_histogram_visible_;

    // Advanced detection features
    ui::Checkbox check_dwell_enabled_;
    ui::Checkbox check_confirm_count_;
    ui::Checkbox check_noise_blacklist_;

    // Sweep frequency range fields (MHz)
    ui::NumberField field_sweep_start_;
    ui::NumberField field_sweep_end_;

    // Sweep step field (kHz)
    ui::NumberField field_sweep_step_;

    ui::Button button_defaults_;
    ui::Button button_save_;
    ui::Button button_cancel_;
    ui::Button button_about_;

    NavigationView& nav_;
    DroneScanner* scanner_ptr_;
    DroneDisplay* display_ptr_;

    ScanConfig original_config_;
    DroneSettings settings_;
    bool settings_dirty_;

    void apply_settings() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SETTINGS_HPP
