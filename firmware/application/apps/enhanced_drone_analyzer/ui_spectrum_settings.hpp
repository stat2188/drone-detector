/*
 * ========================================
 * LEGACY FILE - DEPRECATED
 * ========================================
 * This file has been DEPRECATED.
 *
 * All features from this file have been merged
 * into DroneAnalyzerSettings:
 * - IQ calibration: iq_calibration_enabled, rx_phase_value
 * - Amplifier control: lna_gain_db, vga_gain_db, rf_amp_enabled
 * - Frequency presets: Use DroneFrequencyPresets instead
 *
 * Status: Fully deprecated - all code disabled
 * ========================================
 */

#if 0  // DEPRECATED - All code disabled

// ui_spectrum_settings.hpp - Advanced spectrum settings for EDA (Migrated from Looking Glass)
// Provides presets, IQ calibration, and amplifier controls

#ifndef UI_SPECTRUM_SETTINGS_HPP_
#define UI_SPECTRUM_SETTINGS_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include "freqman.hpp"
#include "ui_drone_common_types.hpp"
#include "radio.hpp"

namespace ui::apps::enhanced_drone_analyzer {

using rf::Frequency;

// Enhanced frequency range preset system (migrated from Looking Glass)
struct FrequencyPreset {
    Frequency min_freq_hz = 0;
    Frequency max_freq_hz = 0;
    std::string label = "";
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    ThreatLevel default_threat_level = ThreatLevel::NONE;

    bool is_valid() const {
        return min_freq_hz > 0 && max_freq_hz > min_freq_hz && !label.empty();
    }
};

// IQ Phase calibration settings (migrated from Looking Glass)
struct IQCalibrationSettings {
    uint8_t rx_phase_value = 15;  // Default from Looking Glass
    bool iq_calibration_enabled = false;
    uint8_t min_calibration_value = 0;
    uint8_t max_calibration_value = 63;  // For 5 or 6 bit IQ supports

    void set_phase_value(uint8_t value) {
        if (value >= min_calibration_value && value <= max_calibration_value) {
            rx_phase_value = value;
        }
    }

    uint8_t get_phase_value() const { return rx_phase_value; }
};

// Amplifier control settings (LNA/VGA/RF amp)
struct AmplifierControl {
    // LNA amplification levels (0-63 dB typical)
    uint8_t lna_gain_db = 32;
    // Variable gain amplifier (0-62 dB)
    uint8_t vga_gain_db = 20;
    // RF pre-amplifier enable/disable
    bool rf_amp_enabled = false;

    bool is_valid_lna_gain() const { return lna_gain_db <= 63; }
    bool is_valid_vga_gain() const { return vga_gain_db <= 62; }

    // 🔴 FIXED: Removed meaningless unsigned negative check - unsigned values are always >= 0
    void set_lna_gain(uint8_t gain) {
        lna_gain_db = gain;  // Already validated in is_valid_lna_gain()
    }

    // 🔴 FIXED: Removed meaningless unsigned negative check - unsigned values are always >= 0
    void set_vga_gain(uint8_t gain) {
        vga_gain_db = gain;  // Already validated in is_valid_vga_gain()
    }
};

// Range locking mechanism (migrated from Looking Glass)
struct FrequencyRange {
    Frequency min_hz = 0;
    Frequency max_hz = 0;

    bool is_valid() const {
        return min_hz > 0 && max_hz > min_hz;
    }
};

struct RangeLockSettings {
    RangeLockSettings()
        : locked_range(false),
          locked_range_values{}
    {}

    bool locked_range;
    FrequencyRange locked_range_values;

    void lock_range(const FrequencyRange& range) {
        locked_range = true;
        locked_range_values = range;
    }

    void unlock_range() {
        locked_range = false;
        locked_range_values = {0, 0};
    }

    bool is_in_locked_range() const {
        return locked_range;
    }
};

// Unified spectrum settings container
struct SpectrumAnalyzerSettings {
    // Initialize in default constructor to satisfy -Weffc++
    SpectrumAnalyzerSettings() :
        preset_ranges(),
        current_preset_index(0),
        iq_calibration(),
        amplifiers(),
        range_lock(),
        live_frequency_view_display(true),
        trigger_level(32),
        scan_mode(0),
        current_min_freq(2400000000ULL),
        current_max_freq(2500000000ULL)
    {
    }

    // Preset management
    std::vector<FrequencyPreset> preset_ranges;
    size_t current_preset_index;

    // IQ calibration (from Looking Glass)
    IQCalibrationSettings iq_calibration;

    // Amplifier controls (from Looking Glass)
    AmplifierControl amplifiers;

    // Range locking
    RangeLockSettings range_lock;

    // UI state
    bool live_frequency_view_display;
    uint8_t trigger_level;
    uint8_t scan_mode;  // FASTSCAN from Looking Glass

    // Frequency range (default drone spectrum)
    Frequency current_min_freq;
    Frequency current_max_freq;

    void add_preset(const FrequencyPreset& preset) {
        if (preset.is_valid()) {
            preset_ranges.push_back(preset);
        }
    }

    void set_frequency_range(const FrequencyRange& range) {
        if (!range_lock.is_in_locked_range()) {
            current_min_freq = range.min_hz;
            current_max_freq = range.max_hz;
        }
    }

    bool load_preset(size_t index) {
        if (index < preset_ranges.size()) {
            const auto& preset = preset_ranges[index];
            if (!range_lock.is_in_locked_range()) {
                current_min_freq = preset.min_freq_hz;
                current_max_freq = preset.max_freq_hz;
                current_preset_index = index;
                return true;
            }
        }
        return false;
    }

    const FrequencyPreset* get_current_preset() const {
        if (current_preset_index < preset_ranges.size()) {
            return &preset_ranges[current_preset_index];
        }
        return nullptr;
    }
};

// Preset file loader (adapted for EDA drone directory)
class SpectrumPresetLoader {
public:
    explicit SpectrumPresetLoader(const std::string& preset_path = "EDA_PRESETS");
    ~SpectrumPresetLoader();

    bool load_presets_from_file();
    bool save_presets_to_file() const;
    size_t get_preset_count() const { return settings_.preset_ranges.size(); }
    const FrequencyPreset* get_preset(size_t index) const;
    bool add_custom_preset(const FrequencyPreset& preset);

    SpectrumAnalyzerSettings& get_settings() { return settings_; }
    const SpectrumAnalyzerSettings& get_settings() const { return settings_; }

private:
    std::string preset_file_path_;
    SpectrumAnalyzerSettings settings_;

    void initialize_default_presets();
    // 🔴 REMOVED: Unused private methods removed
    // - parse_preset_line
    // - serialize_preset
    // - get_preset_filename
};

} // namespace ui::apps::enhanced_drone_analyzer

#endif // UI_SPECTRUM_SETTINGS_HPP_

#endif // 0 - DEPRECATED

