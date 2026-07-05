#ifndef DRONE_SETTINGS_HPP
#define DRONE_SETTINGS_HPP

#include <cstdint>
#include <cstddef>
#include "drone_types.hpp"
#include "constants.hpp"
#include "scanner.hpp"
#include "settings_manager.hpp"

#include "ui_painter.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"

namespace drone_analyzer {

class DroneScanner;
class DroneDisplay;

class DroneSettingsView : public ui::View {
public:
    explicit DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display = nullptr) noexcept;
    ~DroneSettingsView() noexcept override;
    DroneSettingsView(const DroneSettingsView&) = delete;
    DroneSettingsView& operator=(const DroneSettingsView&) = delete;
    void paint(ui::Painter& painter) override;
    void focus() override;
    std::string title() const override {
        static const std::string t = "EDA Settings";
        return t;
    }

private:
    NavigationView& nav_;
    DroneScanner* scanner_ptr_;
    DroneDisplay* display_ptr_;
    ScanConfig original_config_;
    SettingsStruct settings_;
    bool settings_dirty_;

    // Row 0: CFAR
    ui::Labels labels_cfar_{{{UI_POS_X(0), UI_POS_Y(0)}, "CFAR:", Color::white()}};
    ui::OptionsField field_cfar_mode_{{UI_POS_X(4), UI_POS_Y(0)}, 7, {
        {"OFF", 0}, {"CA", 1}, {"GO", 2}, {"SO", 3}, {"HYBRID", 4}, {"OS", 5}, {"VI", 6},
    }};
    ui::NumberField field_cfar_threshold_{{UI_POS_X(12), UI_POS_Y(0)}, 3, {10, 100}, 5, ' '};

    // Row 1: Scan interval + volume + mahalanobis
    ui::Labels labels_row1_{{{UI_POS_X(0), UI_POS_Y(1)}, "Int(ms):", Color::white()}};
    ui::NumberField field_scan_interval_{{UI_POS_X(1), UI_POS_Y(2)}, 4, {10, 1000}, 10, ' '};
    ui::Labels labels_sens_{{{UI_POS_X(1), UI_POS_Y(3)}, "Sens:", Color::white()}};
    ui::NumberField field_rssi_threshold_{{UI_POS_X(1), UI_POS_Y(4)}, 3, {0, 100}, 1, ' '};
    ui::Labels labels_vol_{{{UI_POS_X(13), UI_POS_Y(2)}, "Vol:", Color::white()}};
    ui::NumberField field_volume_{{UI_POS_X(17), UI_POS_Y(2)}, 2, {0, 99}, 1, ' '};
    ui::Labels labels_cyc_{{{UI_POS_X(13), UI_POS_Y(3)}, "Cyc:", Color::white()}};
    ui::NumberField field_rssi_dec_cyc_{{UI_POS_X(17), UI_POS_Y(3)}, 2, {1, 50}, 1, ' '};
    ui::Checkbox check_mahalanobis_{{UI_POS_X(20), UI_POS_Y(2)}, 3, "MG", false};
    ui::NumberField field_mahalanobis_threshold_{{UI_POS_X(22), UI_POS_Y(1)}, 3,
        {MAHALANOBIS_THRESHOLD_MIN_X10, MAHALANOBIS_THRESHOLD_MAX_X10},
        DEFAULT_MAHALOBIS_THRESHOLD_X10, ' '};

    // Row 3-4: Shape filter
    ui::Labels labels_shape_{
        {{UI_POS_X(0), UI_POS_Y(5)}, "MaxW:", Color::white()},
        {{UI_POS_X(0), UI_POS_Y(6)}, "Shrp:", Color::white()},
        {{UI_POS_X(10), UI_POS_Y(5)}, "Rat:", Color::white()},
        {{UI_POS_X(10), UI_POS_Y(6)}, "Vly:", Color::white()},
        {{UI_POS_X(10), UI_POS_Y(4)}, "Flat:", Color::white()},
        {{UI_POS_X(17), UI_POS_Y(4)}, "Sym:", Color::white()},
        {{UI_POS_X(17), UI_POS_Y(5)}, "Mar:", Color::white()},
        {{UI_POS_X(17), UI_POS_Y(6)}, "Min:", Color::white()},
    };
    ui::NumberField field_spectrum_max_width_{{UI_POS_X(6), UI_POS_Y(5)}, 3, {2, 255}, 1, ' '};
    ui::NumberField field_spectrum_peak_sharpness_{{UI_POS_X(6), UI_POS_Y(6)}, 3, {50, 250}, 5, ' '};
    ui::NumberField field_spectrum_peak_ratio_{{UI_POS_X(13), UI_POS_Y(5)}, 3, {0, 255}, 5, ' '};
    ui::NumberField field_spectrum_valley_depth_{{UI_POS_X(13), UI_POS_Y(6)}, 3, {0, 200}, 5, ' '};
    ui::NumberField field_spectrum_flatness_{{UI_POS_X(14), UI_POS_Y(4)}, 3, {0, 100}, 5, ' '};
    ui::NumberField field_spectrum_symmetry_{{UI_POS_X(20), UI_POS_Y(4)}, 3, {0, 100}, 5, ' '};
    ui::NumberField field_spectrum_margin_{{UI_POS_X(20), UI_POS_Y(5)}, 3, {5, 200}, 5, ' '};
    ui::NumberField field_spectrum_min_width_{{UI_POS_X(20), UI_POS_Y(6)}, 3, {1, 100}, 1, ' '};

    // Row 5: Detection toggles
    ui::Checkbox check_audio_alerts_{{UI_POS_X(1), UI_POS_Y(9)}, 6, "Audio", false};
    ui::Checkbox check_spectrum_visible_{{UI_POS_X(20), UI_POS_Y(9)}, 5, "SpVis", false};
    ui::Checkbox check_histogram_visible_{{UI_POS_X(20), UI_POS_Y(13)}, 5, "Hist", false};
    ui::Checkbox check_dwell_enabled_{{UI_POS_X(1), UI_POS_Y(11)}, 6, "Dwell", false};
    ui::Checkbox check_confirm_count_{{UI_POS_X(1), UI_POS_Y(13)}, 8, "Confirm", false};
    ui::NumberField field_confirm_count_{{UI_POS_X(13), UI_POS_Y(13)}, 2, {1, 10}, 1, ' '};
    ui::Checkbox check_spectrum_detection_{{UI_POS_X(20), UI_POS_Y(11)}, 5, "SpDet", false};
    ui::Checkbox check_neighbor_margin_{{UI_POS_X(20), UI_POS_Y(15)}, 4, "NB", false};
    ui::NumberField field_neighbor_margin_{{UI_POS_X(17), UI_POS_Y(15)}, 2, {0, 15}, 1, ' '};
    ui::Checkbox check_noise_blacklist_{{UI_POS_X(1), UI_POS_Y(15)}, 8, "Blklist", false};
    ui::Checkbox check_rssi_variance_{{UI_POS_X(20), UI_POS_Y(7)}, 5, "RVar", false};
    ui::Checkbox check_pattern_matching_{{UI_POS_X(10), UI_POS_Y(8)}, 5, "Ptr", false};

    // Row 7: Threat thresholds
    ui::Labels labels_threat_{
        {{UI_POS_X(0), UI_POS_Y(16)}, "Lo:", Color::white()},
        {{UI_POS_X(8), UI_POS_Y(16)}, "Md:", Color::white()},
        {{UI_POS_X(16), UI_POS_Y(16)}, "Hi:", Color::white()},
        {{UI_POS_X(24), UI_POS_Y(16)}, "Cr:", Color::white()},
    };
    ui::NumberField field_threat_low_{{UI_POS_X(3), UI_POS_Y(16)}, 3, {RSSI_MIN_DBM, RSSI_MAX_DBM}, DEFAULT_THREAT_LOW_DBM, ' '};
    ui::NumberField field_threat_medium_{{UI_POS_X(11), UI_POS_Y(16)}, 3, {RSSI_MIN_DBM, RSSI_MAX_DBM}, DEFAULT_THREAT_MEDIUM_DBM, ' '};
    ui::NumberField field_threat_high_{{UI_POS_X(19), UI_POS_Y(16)}, 3, {RSSI_MIN_DBM, RSSI_MAX_DBM}, RSSI_HIGH_THREAT_THRESHOLD_DBM, ' '};
    ui::NumberField field_threat_critical_{{UI_POS_X(27), UI_POS_Y(16)}, 3, {RSSI_MIN_DBM, RSSI_MAX_DBM}, RSSI_CRITICAL_THREAT_THRESHOLD_DBM, ' '};

    // Bottom buttons
    ui::Button button_defaults_{{UI_POS_X(0), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(13), 20}, "DEFAULT"};
    ui::Button button_about_{{UI_POS_X(13), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(2), 20}, "!"};
    ui::Button button_save_{{UI_POS_X(15), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH(14), 20}, "SAVE"};

    void apply_settings_to_ui() noexcept;
    void set_shape_filter_visibility(bool visible) noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SETTINGS_HPP
