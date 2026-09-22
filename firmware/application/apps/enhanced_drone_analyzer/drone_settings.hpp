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
#include "spectrum_preview_widget.hpp"

namespace drone_analyzer {

/**
 * @brief Settings UI component for drone analyzer
 * @note Inherits from ui::View
 * @note Uses SettingsFileManager for all SD card I/O (no duplicated parser)
 */
class DroneScanner;
class DroneDisplay;

/**
 * @brief Zero-heap CFAR-mode selector over a compile-time options table
 * @note Drop-in replacement for ui::OptionsField with the 7 CFAR options.
 *       ui::OptionsField stores its options as
 *       std::vector<std::pair<std::string, int32_t>> (~196 B of heap for 7
 *       entries, plus a transient reallocation inside set_options()). The
 *       CFAR option table is compile-time Flash data, so this selector keeps
 *       a single index and resolves the label string at paint time.
 * @note API mirrors ui::OptionsField (on_change(size_t, int32_t),
 *       set_by_value(), value()) so the existing wiring compiles unchanged.
 * @note Heap: 0 B. SRAM: ~44 B (member of DroneSettingsView). Flash: ~250 B.
 * @note UI-thread only (paint / encoder / touch) — same contract as every
 *       other selector widget in this view.
 */
class CfarModeSelector : public ui::Widget {
public:
    using value_t = int32_t;

    // OptionsField-compatible callback: (selected_index, option_value).
    std::function<void(size_t, value_t)> on_change{};

    CfarModeSelector(ui::Point parent_pos, size_t length) noexcept
        : ui::Widget{{parent_pos, {8 * static_cast<int>(length), 16}}},
          length_{length} {
        set_focusable(true);
    }

    CfarModeSelector(const CfarModeSelector&) = delete;
    CfarModeSelector& operator=(const CfarModeSelector&) = delete;

    [[nodiscard]] value_t value() const noexcept {
        return static_cast<value_t>(selected_index_);
    }

    void set_selected_index(size_t new_index, bool trigger_change = true) noexcept;
    void set_by_value(value_t v) noexcept;

    void paint(ui::Painter& painter) override;
    bool on_encoder(const ui::EncoderEvent delta) override;
    bool on_keyboard(const ui::KeyboardEvent key) override;
    bool on_touch(const ui::TouchEvent event) override;
    void on_focus() override;
    void on_blur() override;

private:
    const size_t length_;
    size_t selected_index_{0};
};

class DroneSettingsView : public ui::View {
public:
    explicit DroneSettingsView(NavigationView& nav, const ScanConfig& config, DroneScanner* scanner_ptr, DroneDisplay* display = nullptr) noexcept;

    ~DroneSettingsView() noexcept override;

    DroneSettingsView(const DroneSettingsView&) = delete;
    DroneSettingsView& operator=(const DroneSettingsView&) = delete;

    void paint(ui::Painter& painter) override;

    void focus() override;

    std::string title() const override {
        static const std::string t = "EDA Settings";  // SSO — no heap allocation
        return t;
    }

private:
    ui::Labels labels_;
    ui::NumberField field_scan_interval_;
    ui::NumberField field_rssi_threshold_;
    ui::NumberField field_volume_;
    ui::NumberField field_rssi_dec_cyc_;
    ui::NumberField field_freq_match_radius_;
    ui::Checkbox check_audio_alerts_;
    ui::Checkbox check_spectrum_visible_;
    ui::Checkbox check_timeline_visible_;


    // Detection features
    ui::Checkbox check_dwell_enabled_;
    ui::Checkbox check_confirm_count_;
    ui::NumberField field_confirm_count_;
    ui::NumberField field_miss_tolerance_;  // Independent of confirm_count
    ui::Checkbox check_spectrum_detection_;

    // Neighbor margin
    ui::NumberField field_neighbor_margin_;
    ui::Checkbox check_neighbor_margin_;

    ui::Checkbox check_noise_blacklist_;
    ui::Checkbox check_rssi_variance_;

    SpectrumPreviewWidget preview_;

    // Mahalanobis gate controls
    ui::Checkbox check_mahalanobis_;
    ui::NumberField field_mahalanobis_threshold_;

    // Sensitive mode toggle
    ui::Checkbox check_sensitive_mode_;

    // Spectrum detection params
    ui::NumberField field_spectrum_margin_;
    ui::NumberField field_spectrum_min_width_;
    ui::NumberField field_spectrum_max_width_;
    ui::NumberField field_spectrum_peak_sharpness_;
    ui::NumberField field_spectrum_peak_ratio_;
    ui::NumberField field_spectrum_valley_depth_;
    ui::NumberField field_spectrum_flatness_;
    ui::NumberField field_spectrum_symmetry_;

    ui::Button button_defaults_;
    ui::Button button_about_;
    ui::Button button_save_;

    // Very-strong shape-filter bypass toggle (default OFF — MaxW always enforced)
    ui::Checkbox check_shape_bypass_;

    // Median filter toggle
    ui::Checkbox check_median_enabled_;

    // Threat threshold controls
    ui::NumberField field_threat_low_;
    ui::NumberField field_threat_medium_;
    ui::NumberField field_threat_high_;
    ui::NumberField field_threat_critical_;

    // CFAR detection controls (field_cfar_mode_ is a zero-heap selector over
    // the Flash CFAR_OPTIONS table — see CfarModeSelector in this header)
    CfarModeSelector field_cfar_mode_;
    ui::NumberField field_cfar_ref_cells_;
    ui::NumberField field_cfar_guard_cells_;
    ui::NumberField field_cfar_threshold_;

    NavigationView& nav_;
    DroneScanner* scanner_ptr_;
    DroneDisplay* display_ptr_;

    ScanConfig original_config_;
    SettingsStruct settings_;
    bool settings_dirty_;

    // Last detection gate (Sens) in dBm seen by normalize_threat_ladder().
    // Used to translate the WHOLE threat ladder by the gate delta when the
    // user changes Sensitivity, so all shelves move coherently and the
    // tuned gaps between shelves are preserved.
    int32_t prev_gate_dbm_{RSSI_DETECTION_THRESHOLD_DBM};

    void apply_settings_to_ui() noexcept;
    void update_preview() noexcept;
    void save_settings_to_sd() noexcept;
    void set_shape_filter_visibility(bool visible) noexcept;
    void normalize_threat_ladder(uint8_t edited_field) noexcept;

    /**
     * @brief Stage settings_ into g_workspace_cfg and push it to the scanner.
     * @return true iff the scanner accepted the config (validation passed);
     *         on false the scanner keeps its previous config (fail-safe).
     * @note Shared by the SAVE button and ~DroneSettingsView (apply-on-exit):
     *       the field callbacks only STAGE values into settings_ (plus
     *       settings_dirty_) — without this commit they never reach
     *       DroneScanner::config_, so edits tested via the Back button were
     *       silently discarded and every shape filter appeared dead.
     * @note Sweep-field stash mirrors the historical SAVE dance: SWP view may
     *       have changed sweep fields after this view's ctor snapshot
     *       (original_config_), so live values are re-read first.
     * Stack: ~48 bytes (stash writes hit settings_ directly, no temporaries).
     */
    [[nodiscard]] bool commit_to_scanner() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_SETTINGS_HPP
