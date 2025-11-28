// ui_spectrum_settings.hpp - Advanced spectrum settings for EDA (Migrated from Looking Glass)
// Provides presets, IQ calibration, and amplifier controls

#ifndef __UI_SPECTRUM_SETTINGS_HPP__
#define __UI_SPECTRUM_SETTINGS_HPP__

#include <string>
#include <vector>
#include <cstdint>
#include "../application/freqman.hpp"
#include "ui_drone_common_types.hpp"
#include "radio.hpp"

using rf::Frequency;

namespace ui::external_app::enhanced_drone_analyzer {

// Enhanced frequency range preset system (migrated from Looking Glass)
struct FrequencyPreset {
    Frequency min_freq_hz = 0;
    Frequency max_freq_hz = 0;
    char label[16] = {0}; // Static buffer
    SpectrumMode spectrum_mode = SpectrumMode::MEDIUM;
    ThreatLevel default_threat_level = ThreatLevel::NONE;

    bool is_valid() const {
        return min_freq_hz > 0 && max_freq_hz > min_freq_hz && label[0] != '\0';
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

    void set_lna_gain(uint8_t gain) {
        if (gain <= 63) lna_gain_db = gain;
    }

    void set_vga_gain(uint8_t gain) {
        if (gain <= 62) vga_gain_db = gain;
    }
};

// Range locking mechanism (migrated from Looking Glass)
struct RangeLockSettings {
    bool locked_range = false;  // Prevent changes during scanning
    Frequency locked_min_freq = 0;
    Frequency locked_max_freq = 0;

    void lock_range(Frequency min, Frequency max) {
        locked_range = true;
        locked_min_freq = min;
        locked_max_freq = max;
    }

    void unlock_range() {
        locked_range = false;
        locked_min_freq = 0;
        locked_max_freq = 0;
    }

    bool is_in_locked_range() const {
        return locked_range;
    }
};

static constexpr size_t MAX_PRESETS = 10;

// Unified spectrum settings container
struct SpectrumAnalyzerSettings {
    // Initialize in default constructor to satisfy -Weffc++
    SpectrumAnalyzerSettings() :
        current_preset_index(0),
        preset_count(0),
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
    FrequencyPreset preset_ranges[MAX_PRESETS];
    size_t current_preset_index;
    size_t preset_count;

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
        if (preset.is_valid() && preset_count < MAX_PRESETS) {
            preset_ranges[preset_count++] = preset;
        }
    }

    void set_frequency_range(Frequency min, Frequency max) {
        if (!range_lock.is_in_locked_range()) {
            current_min_freq = min;
            current_max_freq = max;
        }
    }

    bool load_preset(size_t index) {
        if (index < preset_count) {
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
        if (current_preset_index < preset_count) {
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
    size_t get_preset_count() const { return settings_.preset_count; }
    const FrequencyPreset* get_preset(size_t index) const;
    bool add_custom_preset(const FrequencyPreset& preset);

    SpectrumAnalyzerSettings& get_settings() { return settings_; }
    const SpectrumAnalyzerSettings& get_settings() const { return settings_; }

private:
    std::string preset_file_path_;
    SpectrumAnalyzerSettings settings_;

    void initialize_default_presets();
    bool parse_preset_line(const std::string& line, FrequencyPreset& preset);
    std::string serialize_preset(const FrequencyPreset& preset) const;
    std::string get_preset_filename() const;
};

} // namespace ui::external_app::enhanced_drone_analyzer

#endif // __UI_SPECTRUM_SETTINGS_HPP__
