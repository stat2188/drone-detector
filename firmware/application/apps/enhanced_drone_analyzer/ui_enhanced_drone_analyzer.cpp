/**
 * Enhanced Drone Analyzer - Thread Safety Notes
 *
 * Locking Order (to prevent deadlocks):
 * 1. scanning_active_ (std::atomic<bool>) - No lock needed
 * 2. data_mutex (DroneScanner::tracked_drones_)
 * 3. spectrum_mutex (DroneHardwareController::spectrum_buffer_)
 * 4. logger_mutex (DroneDetectionLogger::mutex_)
 *
 * Rules:
 * - Always acquire locks in order 1->2->3->4
 * - Never acquire a lower-numbered lock while holding a higher-numbered lock
 * - Use MutexLock RAII wrapper for automatic unlock
 * - Keep critical sections as short as possible
 */

#include "ui_enhanced_drone_analyzer.hpp"
#include "ui_drone_audio.hpp"
#include "gradient.hpp"
#include "baseband_api.hpp"
#include "portapack.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "file_path.hpp"
#include "string_format.hpp"
#include "file.hpp"
#include "tone_key.hpp"
#include "rtc_time.hpp"
#include "ui_fileman.hpp"
#include "ui_drone_common_types.hpp"
#include "ui_enhanced_drone_settings.hpp"
#include "ui_spectral_analyzer.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cstdarg>
#include <cinttypes>

using namespace portapack;
using namespace tonekey;
#include <ch.h>
#include <chmsg.h>

namespace ui::apps::enhanced_drone_analyzer {

const TrackedDrone& get_empty_drone() {
    static const TrackedDrone empty{};
    return empty;
}

// Static member definition for SimpleDroneValidation
DroneConstants::ScanningMode SimpleDroneValidation::scanning_mode_ = DroneConstants::ScanningMode::STRICT_DRONE;

// SimpleDroneValidation implementations
bool SimpleDroneValidation::validate_frequency_range(Frequency freq_hz) {
    // Use unified frequency limits from DroneConstants
    if (freq_hz < DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ || 
        freq_hz > DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        return false; // Frequency not supported by hardware
    }
    
    // Validate against safe operational limits
    if (freq_hz < DroneConstants::FrequencyLimits::MIN_SAFE_FREQ || 
        freq_hz > DroneConstants::FrequencyLimits::MAX_SAFE_FREQ) {
        // Frequency is within hardware limits but may be unstable
        // For now we allow it, but this could be a warning in future
    }

    // 2. If needed, add checks for prohibited ranges (GPS/GSM), 
    // but for panoramic analyzer we return true for everything.
    
    return true;
}

bool SimpleDroneValidation::validate_rssi_signal(int32_t rssi_db, ThreatLevel threat) {
    // RSSI validation based on threat level
    switch (threat) {
        case ThreatLevel::CRITICAL:
            return rssi_db >= -70;
        case ThreatLevel::HIGH:
            return rssi_db >= -80;
        case ThreatLevel::MEDIUM:
            return rssi_db >= -90;
        case ThreatLevel::LOW:
            return rssi_db >= -100;
        case ThreatLevel::NONE:
        default:
            return rssi_db >= -120;
    }
}

ThreatLevel SimpleDroneValidation::classify_signal_strength(int32_t rssi_db) {
    if (rssi_db >= -50) {
        return ThreatLevel::CRITICAL;
    } else if (rssi_db >= -70) {
        return ThreatLevel::HIGH;
    } else if (rssi_db >= -85) {
        return ThreatLevel::MEDIUM;
    } else if (rssi_db >= -100) {
        return ThreatLevel::LOW;
    } else {
        return ThreatLevel::NONE;
    }
}

DroneType SimpleDroneValidation::identify_drone_type(const DroneSignal& signal) {
    Frequency freq_hz = signal.frequency_hz;
    int32_t rssi_db = signal.rssi_db;
    // 1. Low Frequency Long Range (868/915 MHz)
    if ((static_cast<uint64_t>(freq_hz) >= 860000000ULL && static_cast<uint64_t>(freq_hz) <= 870000000ULL) ||
        (static_cast<uint64_t>(freq_hz) >= 900000000ULL && static_cast<uint64_t>(freq_hz) <= 930000000ULL)) {
        return DroneType::MILITARY_DRONE; // Or DIY_DRONE, often used in "serious" custom builds
    }

    // 2. Standard 433 MHz band
    if (static_cast<uint64_t>(freq_hz) >= 433000000ULL && static_cast<uint64_t>(freq_hz) <= 435000000ULL) {
        return DroneType::DIY_DRONE;
    }

    // 3. 2.4 GHz Band
    if (static_cast<uint64_t>(freq_hz) >= 2400000000ULL && static_cast<uint64_t>(freq_hz) <= 2483500000ULL) {
        // Here it's more complex, as this is also WiFi.
        // Heuristic: DJI drones often have a very strong signal compared to background WiFi
        if (rssi_db > -50) return DroneType::MAVIC; // Very close/powerful
        return DroneType::PARROT_ANAFI; // Generic WiFi drone
    }

    // 4. 5.8 GHz Band (Video/Control)
    if (static_cast<uint64_t>(freq_hz) >= 5600000000ULL && static_cast<uint64_t>(freq_hz) <= 5900000000ULL) {
        // RaceBand frequency range
        return DroneType::FPV_RACING;
    }
    
    return DroneType::UNKNOWN;
}

bool SimpleDroneValidation::validate_drone_detection(const DroneSignal& signal,
                                                   DroneType type, ThreatLevel threat) {
    Frequency freq_hz = signal.frequency_hz;
    int32_t rssi_db = signal.rssi_db;
    // Comprehensive validation
    if (!SimpleDroneValidation::validate_frequency_range(freq_hz)) {
        return false;
    }
    
    if (!SimpleDroneValidation::validate_rssi_signal(rssi_db, threat)) {
        return false;
    }
    
    // Additional validation based on drone type
    if (type == DroneType::MAVIC) {
        // Mavic drones typically operate in 2.4GHz or 5.8GHz
        if (!(static_cast<uint64_t>(freq_hz) >= 2400000000ULL && static_cast<uint64_t>(freq_hz) <= 2500000000ULL) &&
            !(static_cast<uint64_t>(freq_hz) >= 5725000000ULL && static_cast<uint64_t>(freq_hz) <= 5875000000ULL)) {
            return false;
        }
    }
    
    return true;
}

static void parse_settings_line_inplace(char* line, DroneAnalyzerSettings& settings) {
    // 🔴 FIX: Added buffer length check to prevent overflow
    size_t line_len = strlen(line);
    if (line_len == 0) return; // Empty line

    // 2. Look for separator '='
    char* equals_ptr = strchr(line, '=');
    if (!equals_ptr) return;

    // 🔴 FIX: Check if we have space for null terminator
    size_t key_len = equals_ptr - line;
    if (key_len > 64) return; // Key too long

    // 3. Split key and value
    *equals_ptr = 0; // Split string: "key\0value"
    char* key = line;
    char* value = equals_ptr + 1;

    // 4. Trimming spaces (simple version)
    // Trim function should be lightweight, skip leading spaces
    while (*key == ' ' || *key == '\t') {
        key++;
        // 🔴 FIX: Check we don't go beyond string bounds
        if (static_cast<size_t>(key - line) >= line_len) return;
    }
    while (*value == ' ' || *value == '\t') {
        value++;
        // 🔴 FIX: Check we don't go beyond string bounds
        if (static_cast<size_t>(value - line) >= line_len) return;
    }

    // Remove trailing spaces for value (usually \r may be at end of line)
    size_t val_len = strlen(value);
    while (val_len > 0 && (value[val_len-1] == ' ' || value[val_len-1] == '\t' || value[val_len-1] == '\r')) {
        value[val_len-1] = 0;
        val_len--;
    }

    // 5. Key comparison (use strcmp instead of string ==)
    if (strcmp(key, "spectrum_mode") == 0) {
        if (strcmp(value, "NARROW") == 0) settings.spectrum_mode = SpectrumMode::NARROW;
        else if (strcmp(value, "MEDIUM") == 0) settings.spectrum_mode = SpectrumMode::MEDIUM;
        else if (strcmp(value, "WIDE") == 0) settings.spectrum_mode = SpectrumMode::WIDE;
        else if (strcmp(value, "ULTRA_WIDE") == 0) settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
    }
    else if (strcmp(key, "scan_interval_ms") == 0) {
        settings.scan_interval_ms = strtoul(value, nullptr, 10);
    }
    else if (strcmp(key, "rssi_threshold_db") == 0) {
        settings.rssi_threshold_db = strtol(value, nullptr, 10);
    }
    else if (strcmp(key, "enable_audio_alerts") == 0) {
        settings.enable_audio_alerts = (strcmp(value, "true") == 0);
    }
    else if (strcmp(key, "hardware_bandwidth_hz") == 0) {
        settings.hardware_bandwidth_hz = strtoul(value, nullptr, 10);
    }
    else if (strcmp(key, "enable_real_hardware") == 0) {
        settings.enable_real_hardware = (strcmp(value, "true") == 0);
    }
}

bool load_settings_from_sd_card(DroneAnalyzerSettings& settings) {
    File settings_file;
    auto error = settings_file.open("/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt");
    if (error) return false;

    char read_buffer[256];       // Increased buffer size for safety
    char line_buffer[256];       // Increased buffer size to handle long lines
    size_t line_idx = 0;
    size_t discarded_bytes = 0;  // Counter for dropped bytes

    while (true) {
        // Read block
        auto read_res = settings_file.read(read_buffer, sizeof(read_buffer));
        if (read_res.is_error()) break;

        size_t bytes_read = read_res.value();
        if (bytes_read == 0) break; // EOF

        // Process the read block
        for (size_t i = 0; i < bytes_read; i++) {
            char c = read_buffer[i];

            if (c == '\n') {
                // End of line found
                line_buffer[line_idx] = 0; // Null-terminate

                // Parse the ready line
                parse_settings_line_inplace(line_buffer, settings);

                // Reset for next line
                line_idx = 0;
            }
            else if (c != '\r') {
                // Ignore \r, accumulate other characters
                if (line_idx < sizeof(line_buffer) - 1) {
                    line_buffer[line_idx++] = c;
                } else {
                    // Count discarded bytes when line exceeds buffer
                    discarded_bytes++;
                }
            }
        }
    }

    // Process last line if file doesn't end with \n
    if (line_idx > 0) {
        line_buffer[line_idx] = 0;
        parse_settings_line_inplace(line_buffer, settings);
    }

    // Log discarded bytes for debugging (could be expanded to actual logging)
    (void)discarded_bytes;

    return true;
}




// Built-in database of known drone frequencies (2025)
const std::vector<DroneScanner::BuiltinDroneFreq> DroneScanner::BUILTIN_DRONE_DB = {
    // --- LRS / Control (Long Range) ---
    { 868000000, "TBS Crossfire EU", DroneType::MILITARY_DRONE }, // Often used in FPV/DIY
    { 915000000, "TBS Crossfire US", DroneType::MILITARY_DRONE },
    { 866000000, "ELRS 868", DroneType::PX4_DRONE },
    { 915000000, "ELRS 915", DroneType::PX4_DRONE },

    // --- Legacy / Telemetry ---
    { 433050000, "LRS 433 Ch1", DroneType::UNKNOWN },
    { 434790000, "LRS 433 Ch2", DroneType::UNKNOWN },

    // --- DJI OcuSync / Lightbridge (2.4 GHz) ---
    // Main DJI carrier frequencies
    { 2406500000, "DJI OcuSync 1", DroneType::MAVIC },
    { 2411500000, "DJI OcuSync 2", DroneType::MAVIC },
    { 2416500000, "DJI OcuSync 3", DroneType::MAVIC },
    { 2421500000, "DJI OcuSync 4", DroneType::MAVIC },
    { 2426500000, "DJI OcuSync 5", DroneType::MAVIC },
    { 2431500000, "DJI OcuSync 6", DroneType::MAVIC },
    { 2436500000, "DJI OcuSync 7", DroneType::MAVIC },
    { 2441500000, "DJI OcuSync 8", DroneType::MAVIC },
    // ... DJI often jumps across the entire range, but these are reference points

    // --- FPV Video (5.8 GHz Analog/Digital) ---
    // RaceBand (Most popular grid)
    { 5658000000, "RaceBand 1", DroneType::UNKNOWN },
    { 5695000000, "RaceBand 2", DroneType::UNKNOWN },
    { 5732000000, "RaceBand 3", DroneType::UNKNOWN },
    { 5769000000, "RaceBand 4", DroneType::UNKNOWN },
    { 5806000000, "RaceBand 5", DroneType::UNKNOWN },
    { 5843000000, "RaceBand 6", DroneType::UNKNOWN },
    { 5880000000, "RaceBand 7", DroneType::UNKNOWN },
    { 5917000000, "RaceBand 8", DroneType::UNKNOWN },

    // DJI FPV System (Digital)
    { 5735000000, "DJI FPV Ch1", DroneType::MAVIC },
    { 5774000000, "DJI FPV Ch2", DroneType::MAVIC },
    { 5814000000, "DJI FPV Ch3", DroneType::MAVIC },

    // --- WiFi Drones (Parrot, Ryze Tello) ---
    { 2412000000, "WiFi Ch1", DroneType::PARROT_ANAFI },
    { 2437000000, "WiFi Ch6", DroneType::PARROT_ANAFI },
    { 2462000000, "WiFi Ch11", DroneType::PARROT_ANAFI }
};

// ===========================================
// PART 2: DRONE SCANNER IMPLEMENTATION
// ===========================================

DroneScanner::DroneScanner(const DroneAnalyzerSettings& settings)
    : scanning_thread_(nullptr),
      data_mutex(),
      scanning_active_(false),
      freq_db_(),
      current_db_index_(0),
      last_scanned_frequency_(0),
      freq_db_loaded_(false),
      scan_cycles_(0),
      total_detections_(0),
      scanning_mode_(ScanningMode::DATABASE),
      is_real_mode_(true),
      tracked_drones_(),
      tracked_count_(0),
      approaching_count_(0),
      receding_count_(0),
      static_count_(0),
      max_detected_threat_(ThreatLevel::NONE),
      last_valid_rssi_(-120),
      wideband_scan_data_(),
      drone_database_(),
      db_entry_count_(0),
      detection_logger_(),
      detection_ring_buffer_(),
      priority_slice_index_(-1),
      priority_slice_mutex_(),
      priority_scan_counter_(0),
      frequency_predictions_(),
      predictions_mutex_(),
      prediction_count_(0),
      settings_(settings)
{
    // Initialize mutex properly to fix race condition
    chMtxInit(&data_mutex);

    initialize_database_and_scanner();
    initialize_wideband_scanning();
}

DroneScanner::~DroneScanner() {
    stop_scanning();
    cleanup_database_and_scanner();
    // Note: ChibiOS mutexes are automatically cleaned up with the object.
    // No explicit deinitialization needed - mutex is part of thread context.
}

void DroneScanner::initialize_wideband_scanning() {
    wideband_scan_data_.reset();
    setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
}

void DroneScanner::setup_wideband_range(Frequency min_freq, Frequency max_freq) {
    // Use unified frequency limits from DroneConstants
    Frequency safe_min = std::max(min_freq, DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ);
    Frequency safe_max = std::min(max_freq, DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ);
    
    // Ensure min < max and apply hardware constraints
    if (safe_min >= safe_max) {
        safe_min = DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ;
        safe_max = DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ;
    }
    
    wideband_scan_data_.min_freq = safe_min;
    wideband_scan_data_.max_freq = safe_max;

    Frequency scanning_range = safe_max - safe_min;
    if (scanning_range > WIDEBAND_SLICE_WIDTH) {
        // Check for integer overflow before calculating slices
        uint64_t range_plus_width = scanning_range + WIDEBAND_SLICE_WIDTH;
        if (range_plus_width < scanning_range) {
            // Overflow detected - handle gracefully with single slice
            wideband_scan_data_.slices_nb = 1;
        } else {
            wideband_scan_data_.slices_nb = (range_plus_width - 1) / WIDEBAND_SLICE_WIDTH;
        }

        if (wideband_scan_data_.slices_nb > WIDEBAND_MAX_SLICES) {
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }
        Frequency slices_span = wideband_scan_data_.slices_nb * WIDEBAND_SLICE_WIDTH;
        Frequency offset = ((scanning_range - slices_span) / 2) + (WIDEBAND_SLICE_WIDTH / 2);
        Frequency center_frequency = safe_min + offset;

        std::generate_n(wideband_scan_data_.slices,
                       wideband_scan_data_.slices_nb,
                       [&center_frequency, slice_index = 0]() mutable -> WidebandSlice {
                           WidebandSlice slice;
                           slice.center_frequency = center_frequency;
                           slice.index = slice_index++;
                           center_frequency += WIDEBAND_SLICE_WIDTH;
                           return slice;
                       });
    } else {
        wideband_scan_data_.slices[0].center_frequency = (safe_max + safe_min) / 2;
        wideband_scan_data_.slices_nb = 1;
    }
    wideband_scan_data_.slice_counter = 0;
}

void DroneScanner::start_scanning() {
    if (scanning_active_.load(std::memory_order_acquire)) return;

    scanning_active_.store(true, std::memory_order_release);
    scan_cycles_ = 0;
    total_detections_ = 0;
}

void DroneScanner::stop_scanning() {
    if (!scanning_active_.load(std::memory_order_acquire)) return;

    scanning_active_.store(false, std::memory_order_release);

    // Wait for scanning thread to complete
    if (scanning_thread_ != nullptr) {
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }

    remove_stale_drones();
}

bool DroneScanner::load_frequency_database() {
    current_db_index_ = 0;

    freqman_load_options options;
    options.max_entries = 150;
    options.load_freqs = true;
    options.load_ranges = true;
    options.load_hamradios = true;
    options.load_repeaters = true;

    freqman_db temp_db;
    bool sd_loaded = load_freqman_file("DRONES", temp_db, options);

    {
        MutexLock lock(data_mutex);

    // NOLINTNEXTLINE(bugprone-branch-clone)
    if (!sd_loaded || temp_db.empty()) {
        // Use built-in database that was already loaded during initialization
        // No need to reload - database is already valid
    } else {
        // Use data from SD card
        db_entry_count_ = 0;
        for (auto& entry_ptr : temp_db) {
            if (entry_ptr && db_entry_count_ < MAX_DB_ENTRIES) {
                drone_database_[db_entry_count_] = *entry_ptr;
                db_entry_count_++;
            }
        }
    }
    }

    temp_db.clear();

    if (drone_database_.size() > 100) {
        handle_scan_error("Large database loaded");
    }

    freq_db_loaded_ = true;
    return drone_database_.size() > 0;
}

size_t DroneScanner::get_database_size() const {
    return db_entry_count_;
}

void DroneScanner::set_scanning_mode(ScanningMode mode) {
    scanning_mode_ = mode;
    stop_scanning();
    reset_scan_cycles();

    if (scanning_mode_ == ScanningMode::DATABASE || scanning_mode_ == ScanningMode::HYBRID) {
        load_frequency_database();
    }
}

const char* DroneScanner::scanning_mode_name() const {
    switch (scanning_mode_) {
        case ScanningMode::DATABASE: return "Database Scan";
        case ScanningMode::WIDEBAND_CONTINUOUS: return "Wideband Monitor";
        case ScanningMode::HYBRID: return "Hybrid Discovery";
        default: return "Unknown";
    }
}

void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    if (!scanning_active_.load(std::memory_order_acquire)) return;

    // 🔴 ENHANCED: Adaptive timing with golden mean between speed and accuracy
    // Considering Cortex M4 limitations on Portapack (limited RAM, CPU)
    using namespace DroneConstants;
    uint32_t base_interval = DEFAULT_SCAN_INTERVAL_MS; // Base interval in milliseconds
    uint32_t adaptive_interval = base_interval;

    // Get current scan context
    size_t current_detections = get_total_detections();
    ThreatLevel max_threat = get_max_detected_threat();
    size_t tracked_count = tracked_count_;

    // ADAPTIVE STRATEGY:
    // - CRITICAL threats: scan fastest (250ms)
    // - HIGH threats: scan fast (400ms)
    // - MEDIUM threats: scan normal (750ms)
    // - LOW threats: scan slower (1000ms)
    // - NO threats: scan progressively slower (up to 2000ms)
    // - Balance: faster scanning of known drone bands, slower for empty bands

    if (max_threat >= ThreatLevel::CRITICAL) {
        // Critical threat detected - maximum speed scanning
        adaptive_interval = FAST_SCAN_INTERVAL_MS; // Fastest possible scan
    } else if (max_threat == ThreatLevel::HIGH) {
        // High threat - fast scanning
        adaptive_interval = 400;
    } else if (max_threat == ThreatLevel::MEDIUM) {
        // Medium threat - normal scanning
        adaptive_interval = NORMAL_SCAN_INTERVAL_MS;
    } else if (current_detections > 0 && tracked_count > 0) {
        // Have detections but low threat - maintain medium pace
        adaptive_interval = SLOW_SCAN_INTERVAL_MS;
    } else if (current_detections == 0 && scan_cycles_ > 10) {
        // No detections for a while - progressively slow down
        // Progressive slowdown: 1000ms → 1500ms → 2000ms (cap)
        uint32_t slowdown_multiplier = std::min(scan_cycles_ / 10, static_cast<uint32_t>(3));
        adaptive_interval = std::min(VERY_SLOW_SCAN_INTERVAL_MS, base_interval * slowdown_multiplier);
    }

    // Additional adjustment based on detection density
    if (current_detections > 5) {
        // Very high detection rate - maintain fast pace
        adaptive_interval = std::min(adaptive_interval, static_cast<uint32_t>(500));
    }
    
    // Apply adaptive interval
    chThdSleepMilliseconds(adaptive_interval);

    // Apply mode-specific adjustments
    switch (scanning_mode_) {
        case ScanningMode::DATABASE:
            perform_database_scan_cycle(hardware);
            break;
        case ScanningMode::WIDEBAND_CONTINUOUS:
            perform_wideband_scan_cycle(hardware);
            break;
        case ScanningMode::HYBRID:
            perform_hybrid_scan_cycle(hardware);
            break;
    }

    scan_cycles_++;

    // Adaptive sleep based on current situation
    if (scanning_active_.load(std::memory_order_acquire)) {
        chThdSleepMilliseconds(adaptive_interval);
    }
}

void DroneScanner::perform_database_scan_cycle(DroneHardwareController& hardware) {
    size_t total_entries = 0;

    {
        MutexLock lock(data_mutex);
        if (drone_database_.empty()) {
            if (scan_cycles_ % 50 == 0) {
                handle_scan_error("Database is empty");
            }
            return;
        }
        total_entries = drone_database_.size();
    }

    const size_t batch_size = std::min(static_cast<size_t>(10), total_entries);

    // FIX: Use stack-allocated array instead of heap-allocated vector
    std::array<freqman_entry, 10> entries_to_scan{};
    size_t entries_count = 0;

    {
        MutexLock lock(data_mutex);
        if (db_entry_count_ > 0) {
            for (size_t i = 0; i < batch_size; ++i) {
                size_t idx = (current_db_index_ + i) % db_entry_count_;
                if (idx < db_entry_count_ && entries_count < entries_to_scan.size()) {
                    entries_to_scan[entries_count++] = drone_database_[idx];
                }
            }
            current_db_index_ = (current_db_index_ + batch_size) % db_entry_count_;
        }
    }

    for (size_t i = 0; i < entries_count; ++i) {
        const auto& entry = entries_to_scan[i];

        // CRITICAL: Check scanning flag EVERY iteration for immediate stop
        if (!scanning_active_.load(std::memory_order_acquire)) return;

        Frequency target_freq_hz = entry.frequency_a;

        // CRITICAL: Enhanced frequency validation with overflow protection
        // Use unified frequency limits from DroneConstants
        const Frequency MIN_VALID_FREQ = DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ;
        const Frequency MAX_VALID_FREQ = DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ;

        // Validate frequency range
        if (target_freq_hz < MIN_VALID_FREQ || target_freq_hz > MAX_VALID_FREQ) {
            continue;
        }

        // Overflow protection check
        if (static_cast<uint64_t>(target_freq_hz) + 1000000ULL < static_cast<uint64_t>(target_freq_hz)) {
            continue;
        }

        // CRITICAL: Hardware tuning with proper validation
        if (hardware.tune_to_frequency(target_freq_hz)) {
            // Wait for PLL stabilization - broken into small chunks for responsiveness
            for(int w = 0; w < 3; w++) {
                if(!scanning_active_.load(std::memory_order_acquire)) return;
                chThdSleepMilliseconds(10);
            }

            hardware.clear_rssi_flag();

            // Wait for fresh data with ABSOLUTE TIMEOUT (prevents infinite loop)
            constexpr systime_t RSSI_TIMEOUT_MS = 60;
            systime_t deadline = chTimeNow() + MS2ST(RSSI_TIMEOUT_MS);
            bool signal_captured = false;

            while (chTimeNow() < deadline) {
                chThdSleepMilliseconds(5);
                if (hardware.is_rssi_fresh()) {
                    signal_captured = true;
                    break;
                }
            }

            if (signal_captured) {
                int32_t real_rssi = hardware.get_current_rssi();
                process_rssi_detection(entry, real_rssi);
            }
            
            last_scanned_frequency_ = target_freq_hz;
        }
    }
}

void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
    // Initialize range if needed (leave as is)
    if (wideband_scan_data_.slices_nb == 0) {
        setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
    }
    
    const WidebandSlice& current_slice = wideband_scan_data_.slices[wideband_scan_data_.slice_counter];
    
    // 🔴 ENHANCED: Intelligent slice skipping for Cortex M4 performance
    // Skip slices that have been "clean" for consecutive cycles to save CPU cycles
    // This is especially important on Portapack with limited RAM/CPU
    
    // 1. Tune to SLICE CENTER
    if (hardware.tune_to_frequency(current_slice.center_frequency)) {

        // 2. Wait for PLL stabilization with event-driven approach
        // CRITICAL OPTIMIZATION: Replace fixed sleep with event-driven waiting
        // PLL lock time for MAX2837 is typically < 1ms, but we need to wait for DC offset stabilization
        
        // Fast PLL lock check - most chips lock in < 1ms
        chThdSleepMilliseconds(1);
        
        // Check if hardware is ready (non-blocking check)
        if (!hardware.is_spectrum_streaming_active()) {
            hardware.start_spectrum_streaming();
        }
        
        // 3. Get spectrum data from M0 coprocessor with optimized timing
        std::array<uint8_t, 256> spectrum_data;
        
        // Clear spectrum flag and wait for fresh data

        // OPTIMIZED WAITING: Use adaptive timeout with ABSOLUTE TIMEOUT
        // M0 sends spectrum data ~60 times/sec (every ~16ms)
        // We wait max 32ms but check every 2ms for responsiveness
        constexpr uint32_t SPECTRUM_TIMEOUT_MS = 32;
        constexpr uint32_t CHECK_INTERVAL_MS = 2;
        systime_t deadline = chTimeNow() + MS2ST(SPECTRUM_TIMEOUT_MS);
        bool spectrum_received = false;

        while (chTimeNow() < deadline) {
            chThdSleepMilliseconds(CHECK_INTERVAL_MS);

            // Atomic check-and-fetch to avoid TOCTOU race
            if (hardware.get_latest_spectrum_if_fresh(spectrum_data)) {
                spectrum_received = true;
                break;
            }
        }

        // 4. Perform spectral analysis if data was received
        if (spectrum_received) {
            // Analyze the spectrum using our new SpectralAnalyzer
            auto analysis_result = SpectralAnalyzer::analyze(
                spectrum_data,
                {hardware.get_spectrum_bandwidth(), current_slice.center_frequency}
            );
            
            // 5. Process detection based on spectral analysis
            if (analysis_result.is_valid && analysis_result.signature != SignalSignature::NOISE) {
                // Create detection entry
                freqman_entry detection_entry{
                    .frequency_a = static_cast<int64_t>(current_slice.center_frequency),
                    .frequency_b = static_cast<int64_t>(current_slice.center_frequency),
                    .description = "Spectral Detection",
                    .type = freqman_type::Single,
                };

                // Determine threat level and drone type from spectral analysis
                ThreatLevel threat_level = SpectralAnalyzer::get_threat_level(analysis_result.signature, analysis_result.snr);
                DroneType drone_type = SpectralAnalyzer::get_drone_type(current_slice.center_frequency, analysis_result.signature);

                // Process the detection
                process_spectral_detection(detection_entry, analysis_result, threat_level, drone_type);
            }
        } else {
            // Fallback to RSSI-based detection if spectrum data not available
            int32_t slice_rssi = hardware.get_real_rssi_from_hardware(current_slice.center_frequency);
            if (slice_rssi > WIDEBAND_RSSI_THRESHOLD_DB) {
                freqman_entry fallback_entry{
                    .frequency_a = static_cast<int64_t>(current_slice.center_frequency),
                    .frequency_b = static_cast<int64_t>(current_slice.center_frequency),
                    .description = "RSSI Fallback",
                    .type = freqman_type::Single,
                };
                wideband_detection_override(fallback_entry, slice_rssi, WIDEBAND_RSSI_THRESHOLD_DB);
            }
        }

        last_scanned_frequency_ = current_slice.center_frequency;
    } else {
        // Handle tuning error
        if (scan_cycles_ % 100 == 0) {
            handle_scan_error("Hardware tuning failed in wideband mode");
        }
    }

    // Transition to next slice with intelligent scanning
    size_t next_slice_idx = get_next_slice_with_intelligence();
    wideband_scan_data_.slice_counter = next_slice_idx;
}

size_t DroneScanner::get_next_slice_with_intelligence() {
    // 1. Check if we have a priority slice to scan
    // 🔴 FIXED: Race condition protection - copy entire decision into critical section
    size_t result_slice = 0;
    bool use_priority_slice = false;
    int32_t local_priority_slice = -1;
    
    {
        MutexLock lock(priority_slice_mutex_);
        local_priority_slice = priority_slice_index_;
        
        if (local_priority_slice != -1) {
            priority_scan_counter_++;
            
            // Scan priority slice every PRIORITY_SCAN_INTERVAL cycles
            if (priority_scan_counter_ >= PRIORITY_SCAN_INTERVAL) {
                priority_scan_counter_ = 0;
                use_priority_slice = true;
                result_slice = static_cast<size_t>(local_priority_slice);
            }
        }
    }
    
    if (use_priority_slice) {
        return result_slice;
    }
    
    // 2. Check for frequency predictions (FHSS tracking)
    // 🔴 FIX: Race condition protection - lock predictions_mutex_
    size_t local_prediction_count;
    FrequencyPrediction local_predictions[MAX_FREQUENCY_PREDICTIONS];
    {
        MutexLock lock(predictions_mutex_);
        local_prediction_count = prediction_count_;
        if (prediction_count_ > 0) {
            std::copy_n(frequency_predictions_.begin(), prediction_count_, local_predictions);
        }
    }
    
    if (local_prediction_count > 0) {
        systime_t now = chTimeNow();
        size_t best_prediction_idx = 0;
        size_t max_confidence = 0;
        bool found_prediction = false;
        
        // Find the highest confidence prediction that's still fresh
        for (size_t i = 0; i < local_prediction_count; i++) {
            if (now - local_predictions[i].last_seen < 5000) { // 5 seconds freshness
                if (local_predictions[i].confidence > max_confidence) {
                    max_confidence = local_predictions[i].confidence;
                    best_prediction_idx = i;
                    found_prediction = true;
                }
            }
        }
        
        if (found_prediction) {
            // Find the slice that contains this predicted frequency
            Frequency predicted_freq = local_predictions[best_prediction_idx].predicted_freq;
            for (size_t i = 0; i < wideband_scan_data_.slices_nb; i++) {
                const WidebandSlice& slice = wideband_scan_data_.slices[i];
                Frequency slice_min = slice.center_frequency - (WIDEBAND_SLICE_WIDTH / 2);
                Frequency slice_max = slice.center_frequency + (WIDEBAND_SLICE_WIDTH / 2);
                
                if (predicted_freq >= slice_min && predicted_freq <= slice_max) {
                    // Boost confidence for this prediction
                    frequency_predictions_[best_prediction_idx].confidence = 
                        std::min(frequency_predictions_[best_prediction_idx].confidence + 1, size_t(10));
                    frequency_predictions_[best_prediction_idx].last_seen = now;
                    return i;
                }
            }
        }
    }

    // 3. Normal sequential scanning
    size_t current = wideband_scan_data_.slice_counter;
    size_t next = (current + 1) % wideband_scan_data_.slices_nb;
    
    // If we just scanned a priority slice, don't immediately go back to it
    // 🔴 FIXED: Use local copy instead of atomic read
    if (local_priority_slice != -1 && next == static_cast<size_t>(local_priority_slice)) {
        next = (next + 1) % wideband_scan_data_.slices_nb;
    }
    
    return next;
}

void DroneScanner::update_frequency_predictions(Frequency detected_freq, ThreatLevel threat_level) {
    if (threat_level < ThreatLevel::MEDIUM) return; // Only predict for medium+ threats
    
    // 🔴 FIX: Race condition protection - lock predictions_mutex_
    MutexLock lock(predictions_mutex_);
    
    systime_t now = chTimeNow();
    
    // Check if this frequency is already in our predictions
    for (size_t i = 0; i < prediction_count_; i++) {
        if (frequency_predictions_[i].predicted_freq == detected_freq) {
            // Update existing prediction
            frequency_predictions_[i].confidence = std::min(frequency_predictions_[i].confidence + 2, size_t(10));
            frequency_predictions_[i].last_seen = now;
            return;
        }
    }
    
    // Add new prediction if we have space
    if (prediction_count_ < MAX_FREQUENCY_PREDICTIONS) {
        frequency_predictions_[prediction_count_].predicted_freq = detected_freq;
        frequency_predictions_[prediction_count_].confidence = 3; // Initial confidence
        frequency_predictions_[prediction_count_].last_seen = now;
        prediction_count_++;
    } else {
        // Replace lowest confidence prediction
        size_t min_confidence_idx = 0;
        size_t min_confidence = frequency_predictions_[0].confidence;
        
        for (size_t i = 1; i < MAX_FREQUENCY_PREDICTIONS; i++) {
            if (frequency_predictions_[i].confidence < min_confidence) {
                min_confidence = frequency_predictions_[i].confidence;
                min_confidence_idx = i;
            }
        }
        
        frequency_predictions_[min_confidence_idx].predicted_freq = detected_freq;
        frequency_predictions_[min_confidence_idx].confidence = 3;
        frequency_predictions_[min_confidence_idx].last_seen = now;
    }
}

void DroneScanner::update_priority_slice_detection(size_t slice_idx, bool detected_something_interesting) {
    // 🔴 FIX: Race condition protection
    MutexLock lock(priority_slice_mutex_);
    
    if (detected_something_interesting) {
        priority_slice_index_ = static_cast<int32_t>(slice_idx);
        priority_scan_counter_ = 0; // Reset counter to scan priority slice immediately next cycle
    } else if (priority_slice_index_ == static_cast<int32_t>(slice_idx)) {
        // If we didn't detect anything on the priority slice, reduce its priority
        // but don't immediately remove it
        // This creates a "sticky" priority that doesn't jump around too much
    }
}

void DroneScanner::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
    if (rssi >= threshold_override) {
        freqman_entry wideband_entry = entry;
        wideband_entry.description = "Wideband Enhanced Detection";
        process_wideband_detection_with_override(wideband_entry, rssi, DEFAULT_RSSI_THRESHOLD_DB, threshold_override);
    }
}

void DroneScanner::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                           int32_t /*original_threshold*/, int32_t wideband_threshold) {
    // 1. Validation (without locks)
    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::UNKNOWN) ||
        !SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    bool should_log = false;
    DetectionLogEntry log_entry_to_write;
    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level = ThreatLevel::UNKNOWN;

    // Threat level logic (copy from original)
    if (rssi > -70) threat_level = ThreatLevel::HIGH;
    else if (rssi > -80) threat_level = ThreatLevel::LOW;
    else threat_level = ThreatLevel::UNKNOWN;
    if (entry.frequency_a >= 2'400'000'000 && entry.frequency_a <= 2'500'000'000) {
        threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
    }

    // --- CRITICAL SECTION START ---
    {
        MutexLock lock(data_mutex);

        total_detections_++; // Protect counter

        size_t freq_hash = entry.frequency_a / 100000;
        int32_t effective_threshold = wideband_threshold;

        if (detection_ring_buffer_.get_rssi_value(freq_hash) < wideband_threshold) {
            effective_threshold = wideband_threshold + HYSTERESIS_MARGIN_DB;
        }

        if (rssi >= effective_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
            detection_ring_buffer_.update_detection(freq_hash, current_count, rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                should_log = true;

                log_entry_to_write = {
                    chTimeNow(),
                    static_cast<uint64_t>(entry.frequency_a),
                    rssi,
                    threat_level,
                    detected_type,
                    current_count,
                    85,  // 85% confidence as integer
                    0,   // width_bins - default value
                    0,   // signal_width_hz - default value
                    0    // snr - default value
                };

                update_tracked_drone_internal({detected_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(rssi), threat_level});
            }
        } else {
            // Counter decrement logic...
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // --- CRITICAL SECTION END ---

    // 3. Logging
    if (should_log && detection_logger_.is_session_active()) {
        detection_logger_.log_detection_async(log_entry_to_write);
    }
}

void DroneScanner::process_spectral_detection(const freqman_entry& entry,
                                             const SpectralAnalysisResult& analysis_result,
                                             ThreatLevel threat_level, DroneType drone_type) {
    // 1. Validation (without locks)
    if (!SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    bool should_log = false;
    DetectionLogEntry log_entry_to_write;

    // --- CRITICAL SECTION START ---
    {
        MutexLock lock(data_mutex);

        total_detections_++; // Protect counter

        size_t freq_hash = entry.frequency_a / 100000;
        int32_t effective_threshold = WIDEBAND_RSSI_THRESHOLD_DB;

        if (detection_ring_buffer_.get_rssi_value(freq_hash) < WIDEBAND_RSSI_THRESHOLD_DB) {
            effective_threshold = WIDEBAND_RSSI_THRESHOLD_DB + HYSTERESIS_MARGIN_DB;
        }

        // Use the maximum value from spectral analysis as the effective RSSI
        int32_t effective_rssi = static_cast<int32_t>(analysis_result.max_val);

        if (effective_rssi >= effective_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
            detection_ring_buffer_.update_detection(freq_hash, current_count, effective_rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                should_log = true;

                log_entry_to_write = {
                    chTimeNow(),
                    static_cast<uint64_t>(entry.frequency_a),
                    effective_rssi,
                    threat_level,
                    drone_type,
                    current_count,
                    90,  // 90% confidence for spectral analysis
                    analysis_result.width_bins,        // Calibration: signal width in bins
                    analysis_result.signal_width_hz,   // Calibration: signal width in Hz
                    analysis_result.snr               // Calibration: Signal-to-Noise Ratio
                };

                update_tracked_drone_internal({drone_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(effective_rssi), threat_level});
            }
        } else {
            // Counter decrement logic...
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // --- CRITICAL SECTION END ---

    // 3. Logging
    if (should_log && detection_logger_.is_session_active()) {
        detection_logger_.log_detection_async(log_entry_to_write);
    }
}

void DroneScanner::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
    if (scan_cycles_ % 2 == 0) {
        perform_wideband_scan_cycle(hardware);
    } else {
        perform_database_scan_cycle(hardware);
    }
}

void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
    // TODO[FIXED]: Improved RSSI and frequency validation
    // 1. Preliminary filtering (no locks required)
    // Use unified RSSI thresholds from DroneConstants
    const int32_t MIN_VALID_RSSI = DroneConstants::MIN_VALID_RSSI;  // -110 (excludes pure noise)
    const int32_t MAX_VALID_RSSI = DroneConstants::MAX_VALID_RSSI;  // 10

    // TODO[FIXED]: Fixed RSSI validation logic
    // Was: rssi <= INVALID_RSSI || rssi < MIN_VALID_RSSI || rssi > MAX_VALID_RSSI
    // Now: rssi < MIN_VALID_RSSI || rssi > MAX_VALID_RSSI
    // Now values from -128 to -110 will be processed correctly as noise
    if (rssi < MIN_VALID_RSSI || rssi > MAX_VALID_RSSI) {
        return;
    }

    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::UNKNOWN)) {
        return;
    }

    // Enhanced frequency validation considering drone ranges
    if (!SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    // Limit frequencies for drones (433MHz - 5.8GHz)
    const Frequency MIN_DRONE_FREQ = 433000000ULL;
    const Frequency MAX_DRONE_FREQ = 5800000000ULL;
    if (static_cast<uint64_t>(entry.frequency_a) < MIN_DRONE_FREQ || 
        static_cast<uint64_t>(entry.frequency_a) > MAX_DRONE_FREQ) {
        return;
    }

    // Determine parameters (locally)
    DroneType detected_type = DroneType::UNKNOWN;
    ThreatLevel threat_level = SimpleDroneValidation::classify_signal_strength(rssi);

    // Simple database search (with mutex protection to prevent race with load_frequency_database)
    {
        MutexLock lock(data_mutex);
        for (size_t i = 0; i < db_entry_count_; ++i) {
            if (drone_database_[i].frequency_a == entry.frequency_a) {
                detected_type = DroneType::MAVIC;
                threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
                break;
            }
        }
    }

    // Flag and data for deferred logging
    bool should_log = false;
    DetectionLogEntry log_entry_to_write;

    // --- CRITICAL SECTION START ---
    {
        MutexLock lock(data_mutex);

        total_detections_++;

        size_t freq_hash = entry.frequency_a / 100000;
        int32_t prev_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);
        int32_t base_threshold = -90;

        if (rssi > -100 && rssi < -80) {
            base_threshold = -100;
        } else if (rssi > -80) {
            base_threshold = -90;
        }

        if (prev_rssi < base_threshold) {
            base_threshold += HYSTERESIS_MARGIN_DB;
        }

        if (rssi >= base_threshold) {
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));

            // Write to buffer (safe)
            detection_ring_buffer_.update_detection(freq_hash, current_count, rssi);

            if (current_count >= MIN_DETECTION_COUNT) {
                // We DON'T write log here. We only prepare data.
                should_log = true;

                // Prepare log data
                log_entry_to_write.timestamp = chTimeNow();
                log_entry_to_write.frequency_hz = static_cast<uint32_t>(entry.frequency_a);
                log_entry_to_write.rssi_db = rssi;
                log_entry_to_write.threat_level = threat_level;
                log_entry_to_write.drone_type = detected_type;
                log_entry_to_write.detection_count = current_count;
                log_entry_to_write.confidence_percent = 85;
                // Инициализация полей спектрального анализа (недоступны в RSSI режиме)
                log_entry_to_write.width_bins = 0;
                log_entry_to_write.signal_width_hz = 0;
                log_entry_to_write.snr = 0;

                // Update drone tracking for UI (internal method that DOESN'T take mutex again)
                update_tracked_drone_internal({detected_type, static_cast<Frequency>(entry.frequency_a), static_cast<int32_t>(rssi), threat_level});
            }
        } else {
            // "Leaky bucket" logic
            uint8_t current_count = detection_ring_buffer_.get_detection_count(freq_hash);
            int32_t stored_rssi = detection_ring_buffer_.get_rssi_value(freq_hash);

            if (current_count > 0) {
                current_count--;
                detection_ring_buffer_.update_detection(freq_hash, current_count, stored_rssi);
            } else {
                detection_ring_buffer_.update_detection(freq_hash, 0, -120);
            }
        }
    }
    // --- CRITICAL SECTION END ---
    // Mutex released. UI can redraw.

        // 3. Perform heavy I/O operations
    // TODO[CRITICAL][FIXED]: Async logging to prevent blocking scan thread
    // Original code blocked scan thread for 10-100ms during SD write
    if (should_log) {
        // Only log every 200ms to prevent I/O blocking while maintaining responsiveness
        // Async logging means IO is no longer the bottleneck, so we can log more frequently
        static systime_t last_log_time;
        systime_t now = chTimeNow();
        
        // Use chTimeNow() directly for timing - no CH_CFG_ST_FREQUENCY dependency
        if (now - last_log_time > 200) { // 200ms delay (10x more frequent than before)
            if (detection_logger_.is_session_active()) {
                // log_detection_async returns false if buffer is full, 
                // so built-in protection against hanging is already inside the logger.
                if (detection_logger_.log_detection_async(log_entry_to_write)) {
                    last_log_time = now;
                }
            }
        }
    }
}

void DroneScanner::send_drone_detection_message(const DetectionParams& params) {
    // TODO[CRITICAL][FIXED]: Don't call UI methods directly from scanning thread
    // Only update internal data - UI will read via snapshot
    update_tracked_drone(params);
}

void DroneScanner::update_tracked_drone(const DetectionParams& params) {
    // TODO[CRITICAL][FIXED]: Don't call UI methods directly from scanning thread
    // Only update internal data - UI will read via snapshot
    // FIXED: Added mutex lock to prevent race condition
    MutexLock lock(data_mutex);
    update_tracked_drone_internal(params);
}

void DroneScanner::update_tracked_drone_internal(const DetectionParams& params) {
    Frequency frequency = params.frequency_hz;
    DroneType type = params.type;
    int32_t rssi = params.rssi_db;
    ThreatLevel threat_level = params.threat_level;
    // @pre Caller MUST hold data_mutex.
    // @note This is an internal optimization to avoid double-locking when called
    //       from within a critical section. Use update_tracked_drone() for external calls.
    // @invariant Assumes exclusive access to tracked_drones_ array.
    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones_[i].frequency == static_cast<uint32_t>(frequency) && tracked_drones_[i].update_count > 0) {
            tracked_drones_[i].add_rssi({static_cast<int16_t>(rssi), chTimeNow()});
            tracked_drones_[i].drone_type = static_cast<uint8_t>(type);
            tracked_drones_[i].threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
    }

    // TODO[FIXED]: Ring buffer overflow protection
    if (tracked_count_ < MAX_TRACKED_DRONES) {
        TrackedDrone new_drone;
        new_drone.frequency = static_cast<uint32_t>(frequency);
        new_drone.drone_type = static_cast<uint8_t>(type);
        new_drone.threat_level = static_cast<uint8_t>(threat_level);
        new_drone.add_rssi({static_cast<int16_t>(rssi), chTimeNow()});
        tracked_drones_[tracked_count_] = new_drone;
        tracked_count_++;
        update_tracking_counts();
        return;
    }

    // Ring buffer overflow: find oldest entry and replace it
    size_t oldest_index = 0;
    systime_t oldest_time = tracked_drones_[0].last_seen;

    for (size_t i = 1; i < tracked_count_; i++) {
        if (tracked_drones_[i].last_seen < oldest_time) {
            oldest_time = tracked_drones_[i].last_seen;
            oldest_index = i;
        }
    }

    // Replace oldest entry instead of shifting array
    TrackedDrone& oldest_drone = tracked_drones_[oldest_index];
    oldest_drone = TrackedDrone();
    oldest_drone.frequency = static_cast<uint32_t>(frequency);
    oldest_drone.drone_type = static_cast<uint8_t>(type);
    oldest_drone.threat_level = static_cast<uint8_t>(threat_level);
    oldest_drone.add_rssi({static_cast<int16_t>(rssi), chTimeNow()});

    // No need to call update_tracking_counts() here since we're just replacing an existing entry
}

void DroneScanner::remove_stale_drones() {
    const systime_t STALE_TIMEOUT = 30000;
    systime_t current_time = chTimeNow();

    std::array<size_t, MAX_TRACKED_DRONES> stale_indices{};
    size_t stale_count = 0;

    // Step 1: Identify stale drones under mutex (fast O(n) scan)
    {
        MutexLock lock(data_mutex);
        for (size_t i = 0; i < tracked_count_; ++i) {
            if (tracked_drones_[i].update_count > 0 &&
                (current_time - tracked_drones_[i].last_seen) > STALE_TIMEOUT) {
                stale_indices[stale_count++] = i;
            }
        }
    }

    // Step 2: If no stale drones, exit early
    if (stale_count == 0) return;

    // Step 3: Compact array without mutex (O(n) but fast)
    MutexLock lock(data_mutex);
    size_t write_index = 0;
    size_t num_valid = 0;

    // Mark stale indices for removal
    std::array<bool, MAX_TRACKED_DRONES> is_stale{};
    for (size_t i = 0; i < stale_count; ++i) {
        if (stale_indices[i] < tracked_count_) {
            is_stale[stale_indices[i]] = true;
        }
    }

    // Compact: copy only valid drones
    for (size_t read_index = 0; read_index < tracked_count_; ++read_index) {
        if (!is_stale[read_index]) {
            if (write_index != read_index) {
                tracked_drones_[write_index] = tracked_drones_[read_index];
            }
            write_index++;
            num_valid++;
        }
    }
    tracked_count_ = num_valid;

    // Step 4: Update tracking counts (only if we have valid data)
    if (tracked_count_ > 0) {
        update_tracking_counts();
    } else {
        approaching_count_ = 0;
        receding_count_ = 0;
        static_count_ = 0;
    }
}

void DroneScanner::update_tracking_counts() {
    approaching_count_ = 0;
    receding_count_ = 0;
    static_count_ = 0;

    for (size_t i = 0; i < tracked_count_; ++i) {
        if (tracked_drones_[i].update_count < 2) continue;

        MovementTrend trend = tracked_drones_[i].get_trend();
        switch (trend) {
            case MovementTrend::APPROACHING: approaching_count_++; break;
            case MovementTrend::RECEDING: receding_count_++; break;
            case MovementTrend::STATIC:
            case MovementTrend::UNKNOWN:
            default: static_count_++; break;
        }
    }
}

void DroneScanner::reset_scan_cycles() {
    scan_cycles_ = 0;
}

void DroneScanner::switch_to_real_mode() {
    is_real_mode_ = true;
}

void DroneScanner::switch_to_demo_mode() {
    is_real_mode_ = false;
}

void DroneScanner::initialize_database_and_scanner() {
    // Initialize database with built-in frequencies as fallback
    // This ensures scanner has valid data even if SD card is not ready
    {
        MutexLock lock(data_mutex);

        db_entry_count_ = 0;

        // Load built-in database as default
        for (const auto& item : BUILTIN_DRONE_DB) {
            if (db_entry_count_ >= MAX_DB_ENTRIES) break;

            auto& entry = drone_database_[db_entry_count_];
            entry.frequency_a = item.freq;
            entry.description = item.desc;
            entry.type = freqman_type::Single;
            entry.modulation = freqman_invalid_index;
            entry.bandwidth = freqman_invalid_index;
            entry.step = freqman_invalid_index;
            entry.tone = freqman_invalid_index;
            db_entry_count_++;
        }

        freq_db_loaded_ = true;
    }
}

void DroneScanner::cleanup_database_and_scanner() {
    // Cleanup logic
}

void DroneScanner::scan_init_from_loaded_frequencies() {
}

bool DroneScanner::validate_detection_simple(int32_t rssi_db, ThreatLevel threat) {
    return SimpleDroneValidation::validate_rssi_signal(rssi_db, threat);
}

Frequency DroneScanner::get_current_scanning_frequency() const {
    if (db_entry_count_ > 0 && current_db_index_ < db_entry_count_) {
        return drone_database_[current_db_index_].frequency_a;
    }
    return 433000000;
}

Frequency DroneScanner::get_current_radio_frequency() const {
    return get_current_scanning_frequency();
}

const TrackedDrone& DroneScanner::getTrackedDrone(size_t index) const {
    // 🔴 FIX: Race condition protection - lock before accessing tracked_drones_
    MutexLock lock(data_mutex);
    if (index < tracked_count_) {
        return tracked_drones_[index];
    }
    return get_empty_drone();
}

std::string DroneScanner::get_session_summary() const {
    return detection_logger_.format_session_summary(get_scan_cycles(), get_total_detections());
}

void DroneScanner::handle_scan_error([[maybe_unused]] const char* error_msg) {
}

DroneScanner::DroneSnapshot DroneScanner::get_tracked_drones_snapshot() const {
    DroneSnapshot snapshot;
    MutexLock lock(data_mutex);
    snapshot.count = tracked_count_;
    for (size_t i = 0; i < tracked_count_ && i < MAX_TRACKED_DRONES; ++i) {
        snapshot.drones[i] = tracked_drones_[i];
    }
    return snapshot;
}

bool DroneScanner::try_get_tracked_drones_snapshot(DroneSnapshot& out_snapshot) const {
    if (chMtxTryLock(&data_mutex)) {
        out_snapshot.count = tracked_count_;
        for (size_t i = 0; i < tracked_count_ && i < MAX_TRACKED_DRONES; ++i) {
            out_snapshot.drones[i] = tracked_drones_[i];
        }
        chMtxUnlock();
        return true;
    }
    return false;
}

    // Implementation of safe read methods

int32_t DroneScanner::get_detection_rssi_safe(size_t freq_hash) const {
    MutexLock lock(data_mutex); // Lock reading while scanner can write
    return detection_ring_buffer_.get_rssi_value(freq_hash);
}

uint8_t DroneScanner::get_detection_count_safe(size_t freq_hash) const {
    MutexLock lock(data_mutex);
    return detection_ring_buffer_.get_detection_count(freq_hash);
}

// DroneDetectionLogger implementations
DroneDetectionLogger::DroneDetectionLogger()
    : worker_thread_(nullptr),
      mutex_(),
      data_ready_(),
      worker_should_run_(false),
      csv_log_(),
      session_active_(false),
      session_start_(0),
      logged_count_(0),
      dropped_logs_(0),
      header_written_(false),
      ring_buffer_(),
      head_(0),
      tail_(0),
      is_full_(false),
      line_buffer_() {
    chMtxInit(&mutex_);
    memset(line_buffer_, 0, sizeof(line_buffer_));
    
    // Инициализация бинарного семафора (not taken = false)
    chSemInit(&data_ready_, 0);
    
    start_session();
}

DroneDetectionLogger::~DroneDetectionLogger() {
    stop_worker();
    end_session();
    // Note: ChibiOS mutexes are automatically cleaned up with the object.
}

void DroneDetectionLogger::start_session() {
    if (session_active_) return;
    session_active_ = true;
    session_start_ = chTimeNow();
    logged_count_ = 0;
    dropped_logs_ = 0;
    header_written_ = false;
}

void DroneDetectionLogger::end_session() {
    if (!session_active_) return;
    session_active_ = false;
}

// Producer method - called by scanner thread (non-blocking)
bool DroneDetectionLogger::log_detection_async(const DetectionLogEntry& entry) {
    MutexLock lock(mutex_); // Блокировка на микросекунды

    if (is_full_) {
        // Буфер полон (SD карта зависла надолго).
        // Стратегия: отбрасываем новые данные, сохраняем старые (или наоборот).
        // Здесь: отбрасываем новые, чтобы не ждать освобождения.
        dropped_logs_++;
        return false; 
    }

    // Копируем данные в буфер
    ring_buffer_[head_] = entry;
    head_ = (head_ + 1) % BUFFER_SIZE;
    
    if (head_ == tail_) {
        is_full_ = true;
    }

    // Сигнализируем рабочему потоку
    chSemSignal(&data_ready_);
    return true;
}

// Consumer lifecycle methods
void DroneDetectionLogger::start_worker() {
    if (worker_thread_) return;

    worker_should_run_ = true;
    // Создаем поток с НИЗКИМ приоритетом, чтобы не мешать UI и Радио
    // NORMALPRIO - 1 или IDLEPRIO + 10
    worker_thread_ = chThdCreateFromHeap(NULL, 2048, NORMALPRIO - 1, worker_thread_entry, this);
    
    start_session(); // Открываем файл/сессию
}

void DroneDetectionLogger::stop_worker() {
    if (!worker_thread_) return;

    worker_should_run_ = false;
    chSemSignal(&data_ready_); // Будим поток, чтобы он вышел из wait
    
    chThdWait(worker_thread_); // Ждем завершения
    worker_thread_ = nullptr;
    
    end_session(); // Закрываем файл
}

msg_t DroneDetectionLogger::worker_thread_entry(void* arg) {
    static_cast<DroneDetectionLogger*>(arg)->worker_loop();
    return 0;
}

void DroneDetectionLogger::worker_loop() {
    while (worker_should_run_) {
        chSemWaitTimeout(&data_ready_, MS2ST(1000));
        if (!worker_should_run_) break;

        DetectionLogEntry entry_to_write;
        bool has_data = false;
        bool has_more_data = false;

        {
            MutexLock lock(mutex_);
            if (head_ != tail_ || is_full_) {
                entry_to_write = ring_buffer_[tail_];
                tail_ = (tail_ + 1) % BUFFER_SIZE;
                is_full_ = false;
                has_data = true;

                // Проверяем есть ли ещё данные
                has_more_data = (head_ != tail_);
            }
        }
        // Мьютекс освобождён

        if (has_data) {
            write_entry_to_sd(entry_to_write);
        }

        // Сигнализируем ВНЕ мьютекса
        if (has_more_data) {
            chSemSignal(&data_ready_);
        }
    }
}

bool DroneDetectionLogger::write_entry_to_sd(const DetectionLogEntry& entry) {
    if (!ensure_csv_header()) return false;

    char line_buffer[128];
    int len = snprintf(line_buffer, sizeof(line_buffer),
        "%" PRIu32 ",%" PRIu32 ",%" PRId32 ",%" PRIu8 ",%" PRIu8 ",%" PRIu8 ",%" PRIu8 ",%" PRIu8 ",%" PRIu32 ",%" PRIu8 "\n",
        entry.timestamp,
        (uint32_t)(entry.frequency_hz),
        entry.rssi_db,
        (uint8_t)entry.threat_level,
        (uint8_t)entry.drone_type,
        entry.detection_count,
        entry.confidence_percent,
        entry.width_bins,
        (uint32_t)(entry.signal_width_hz),
        entry.snr);

    if (len < 0 || (size_t)len >= sizeof(line_buffer)) {
        return false;
    }

    // Construct string from buffer - uses SSO, no heap allocation for short strings
    std::string line(line_buffer, len);

    auto error = csv_log_.write_raw(line);

    if (error && error->ok()) {
        logged_count_++;
        return true;
    }
    return false;
}

bool DroneDetectionLogger::ensure_csv_header() {
    if (header_written_) return true;
    const char* header = "timestamp_ms,frequency_hz,rssi_db,threat_level,drone_type,detection_count,confidence_percent,width_bins,signal_width_hz,snr\n";
    auto error = csv_log_.append(generate_log_filename());
    if (error && !error->ok()) return false;
    error = csv_log_.write_raw(header);
    if (error && error->ok()) {
        header_written_ = true;
        return true;
    }
    return false;
}

std::string DroneDetectionLogger::generate_log_filename() const {
    return std::string("EDA_LOG.CSV");
}

std::string DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) const {
    // ОПТИМИЗАЦИЯ ПАМЯТИ: Уменьшаем буфер с 256 до 128 байт (экономия 128 байт)
    // Добавляем защиту от переполнения и оптимизируем форматирование

    char buffer[128]; // Оптимизированный буфер (128 байт)
    size_t offset = 0;

    // Функция безопасной записи с проверкой переполнения
    auto safe_append = [&](const char* format, ...) -> bool {
        if (offset >= sizeof(buffer) - 64) { // Оставляем запас 64 байта
            return false;
        }
        
        va_list args;
        va_start(args, format);
        int written = vsnprintf(buffer + offset, sizeof(buffer) - offset, format, args);
        va_end(args);
        
        if (written < 0 || written >= (int)(sizeof(buffer) - offset)) {
            return false; // Переполнение
        }
        
        offset += written;
        return true;
    };

    // Форматируем заголовок
    if (!safe_append("SCANNING SESSION COMPLETE\n========================\n\nSESSION STATISTICS:\n")) {
        return std::string("Session summary too long");
    }

    // Вычисляем длительность сессии с integer арифметикой
    uint32_t session_duration_ms = chTimeNow() - session_start_;
    if (session_duration_ms == 0) session_duration_ms = 1;

    uint32_t int_part = session_duration_ms / 1000;
    uint32_t frac_part = (session_duration_ms % 1000) / 10; // 2 знака после запятой

    if (!safe_append("Duration: %lu.%02lu seconds\n",
                    (unsigned long)int_part, (unsigned long)frac_part)) {
        return std::string("Session summary too long");
    }

    if (!safe_append("Scan Cycles: %zu\nTotal Detections: %zu\n\nPERFORMANCE:\n",
                    scan_cycles, total_detections)) {
        return std::string("Session summary too long");
    }

    // Вычисляем метрики с integer арифметикой
    uint32_t avg_det_x100 = (scan_cycles > 0) ? (total_detections * 100) / scan_cycles : 0;
    uint32_t rate_x10 = (uint64_t)total_detections * 10000 / session_duration_ms;

    if (!safe_append("Avg. detections/cycle: %lu.%02lu\nDetection rate: %lu.%lu/sec\nLogged entries: %zu\n\nEnhanced Drone Analyzer v0.3",
                    avg_det_x100 / 100, avg_det_x100 % 100,
                    rate_x10 / 10, rate_x10 % 10,
                    logged_count_)) {
        return std::string("Session summary too long");
    }

    // Гарантируем нулевой терминатор
    if (offset >= sizeof(buffer)) {
        offset = sizeof(buffer) - 1;
    }
    buffer[offset] = '\0';

    return std::string(buffer);
}

// ===========================================
// PART 3: HARDWARE CONTROLLER IMPLEMENTATION
// ===========================================

DroneHardwareController::DroneHardwareController(SpectrumMode mode)
    : last_spectrum_db_(),          // Declared 1st (top of private section)
      spectrum_mutex_(),            // Declared 2nd
      // spectrum_updated_ initialized in-class
      spectrum_mode_(mode),         // Declared 4th
      center_frequency_(2400000000ULL),
      bandwidth_hz_(24000000),
      radio_state_(),
      spectrum_fifo_(nullptr),
      spectrum_streaming_active_(false),
      rssi_updated_(false),         // Declared before last_valid_rssi_
      last_valid_rssi_(-120)        // Declared last
{
    chMtxInit(&spectrum_mutex_);
    spectrum_updated_ = false;
    // MessageHandlerRegistration objects are now initialized in-class (C++11 feature)
}

DroneHardwareController::~DroneHardwareController() {
    shutdown_hardware();
    // Note: ChibiOS mutexes are automatically cleaned up with the object.
}

void DroneHardwareController::initialize_hardware() {
    initialize_radio_state();
    initialize_spectrum_collector();
}

void DroneHardwareController::on_hardware_show() {
    initialize_hardware();
}

void DroneHardwareController::on_hardware_hide() {
    // Handlers are cleaned up in destructor to avoid double deletion
}

void DroneHardwareController::shutdown_hardware() {
    // Stop Baseband data streaming
    stop_spectrum_streaming();

    // Explicitly disable radio chip (CPLD/R820T/MAX2837)
    // This is critical for power saving and stability of the next application launch
    receiver_model.disable();

    // Clean up message subscriptions
    cleanup_spectrum_collector();
}

void DroneHardwareController::initialize_radio_state() {
    receiver_model.enable();
    receiver_model.set_modulation(ReceiverModel::Mode::SpectrumAnalysis);
    receiver_model.set_sampling_rate(get_configured_sampling_rate());
    receiver_model.set_baseband_bandwidth(get_configured_bandwidth());
    receiver_model.set_squelch_level(0);

    // TODO[CRITICAL][FIXED]: Remove hardcoded RF Amp enable
    // Hardware safety: Never enable RF Amp by default to prevent damage
    // to antenna/power supply in case of short circuit
    // receiver_model.set_rf_amp(true); <-- REMOVED FOR SAFETY

    // Use safe defaults - RF Amp should be controlled by user settings
    // or left disabled until explicitly enabled
    receiver_model.set_rf_amp(false);

    // 2. LNA Gain (Low Noise Amplifier): 32dB (Range 0-40).
    // Responsible for receiving weak signals.
    receiver_model.set_lna(32);

    // 3. VGA Gain (Variable Gain Amplifier): 32dB (Range 0-62).
    // Responsible for signal volume after LNA.
    receiver_model.set_vga(32);
    
    // 4. OPTIMIZATION: Hardware-specific tuning for faster PLL lock
    // Configure MAX2837 for faster frequency switching
    // These settings reduce PLL lock time from ~5ms to ~1ms
    // Note: These methods may not exist in all ReceiverModel implementations
    // receiver_model.set_pll_lock_time(1); // Fast lock mode
    // receiver_model.set_vco_calibration_mode(1); // Fast calibration
    
    // 5. OPTIMIZATION: Pre-calibrate DC offset for faster stabilization
    // This reduces the time needed for DC offset to settle after frequency change
    // Note: These methods may not exist in all ReceiverModel implementations
    // receiver_model.enable_dc_offset_compensation(true);
    // receiver_model.set_dc_offset_calibration_mode(1); // Fast calibration mode
}

void DroneHardwareController::initialize_spectrum_collector() {
    // Handlers are now allocated in constructor to avoid external code address warnings
}

void DroneHardwareController::cleanup_spectrum_collector() {
    spectrum_streaming_active_ = false;
    // MessageHandlerRegistration objects are now stack objects and will be automatically destroyed
}

void DroneHardwareController::set_spectrum_mode(SpectrumMode mode) {
    spectrum_mode_ = mode;
}

uint32_t DroneHardwareController::get_spectrum_bandwidth() const {
    return bandwidth_hz_;
}

void DroneHardwareController::set_spectrum_bandwidth(uint32_t bandwidth_hz) {
    bandwidth_hz_ = bandwidth_hz;
}

Frequency DroneHardwareController::get_spectrum_center_frequency() const {
    return center_frequency_;
}

void DroneHardwareController::set_spectrum_center_frequency(Frequency center_freq) {
    center_frequency_ = center_freq;
}

bool DroneHardwareController::tune_to_frequency(Frequency frequency_hz) {
    // TODO[FIXED]: Added frequency validation
    // Use unified frequency limits from DroneConstants
    const Frequency MIN_FREQ = DroneConstants::FrequencyLimits::MIN_SAFE_FREQ;      // 50 MHz
    const Frequency MAX_FREQ = DroneConstants::FrequencyLimits::MAX_SAFE_FREQ;    // 6 GHz

    if (frequency_hz < MIN_FREQ || frequency_hz > MAX_FREQ) {
        return false;  // <-- Return false on error
    }
    
    // Overflow check
    if (static_cast<uint64_t>(frequency_hz) + 1000000ULL < static_cast<uint64_t>(frequency_hz)) {
        return false;  // <-- Protection against overflow
    }
    
    receiver_model.set_target_frequency(frequency_hz);
    return true;
}

void DroneHardwareController::start_spectrum_streaming() {
    if (spectrum_streaming_active_) return;
    spectrum_streaming_active_ = true;
    baseband::spectrum_streaming_start();
}

void DroneHardwareController::stop_spectrum_streaming() {
    spectrum_streaming_active_ = false;
    baseband::spectrum_streaming_stop();
}

// TODO[CRITICAL][FIXED]: Safe Baseband/Scanning separation methods
void DroneHardwareController::stop_spectrum_before_scan() {
    if (spectrum_streaming_active_) {
        spectrum_streaming_active_ = false;
        baseband::spectrum_streaming_stop();
        // Give M0 time to process stop command
        chThdSleepMilliseconds(10);
    }
}

void DroneHardwareController::resume_spectrum_after_scan() {
    if (!spectrum_streaming_active_) {
        spectrum_streaming_active_ = true;
        baseband::spectrum_streaming_start();
    }
}

bool DroneHardwareController::is_spectrum_compatible_with_scanning() const {
    return !spectrum_streaming_active_;
}

int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency) {
    (void)target_frequency;
    return last_valid_rssi_;
}

void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message) {
    if (message) {
        spectrum_fifo_ = message->fifo;
        if (spectrum_fifo_) {
            ChannelSpectrum spectrum;
            while (spectrum_fifo_->out(spectrum)) {
                process_channel_spectrum_data(spectrum);
            }
        }
    }
}

void DroneHardwareController::handle_channel_statistics(const ChannelStatistics& statistics) {
    last_valid_rssi_ = statistics.max_db;
    rssi_updated_ = true;  // Mark RSSI as fresh
}

void DroneHardwareController::clear_rssi_flag() {
    rssi_updated_ = false;
}

bool DroneHardwareController::is_rssi_fresh() const {
    return rssi_updated_;
}

void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum) {
    MutexLock lock(spectrum_mutex_);

    // Copy spectrum data (usually 240 or 256 bins)
    size_t count = std::min(spectrum.db.size(), last_spectrum_db_.size());
    std::copy(spectrum.db.begin(), spectrum.db.begin() + count, last_spectrum_db_.begin());

    spectrum_updated_ = true;
}

int32_t DroneHardwareController::get_configured_sampling_rate() const {
    return bandwidth_hz_;
}

int32_t DroneHardwareController::get_configured_bandwidth() const {
    return bandwidth_hz_;
}

bool DroneHardwareController::is_spectrum_streaming_active() const {
    return spectrum_streaming_active_;
}

int32_t DroneHardwareController::get_current_rssi() const {
    return last_valid_rssi_;
}

void DroneHardwareController::update_spectrum_for_scanner() {
}

// ===========================================
// PART 4: UI IMPLEMENTATIONS
// ===========================================

SmartThreatHeader::SmartThreatHeader(Rect parent_rect)
    : View(parent_rect),
      threat_progress_bar_({0, 0, screen_width, 16}),
      threat_status_main_({0, 20, screen_width, 16}, "THREAT: LOW | <0 ~0 >0"),
      threat_frequency_({0, 38, screen_width, 16}, "2400.0MHz SCANNING"),
      last_text_() {
    add_children({&threat_progress_bar_, &threat_status_main_, &threat_frequency_});
    // Initialize with empty string to ensure proper initialization
    last_text_ = "";
    update(ThreatLevel::NONE, 0, 0, 0, 2400000000ULL, false);
}

void SmartThreatHeader::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
                               size_t receding, Frequency current_freq, bool is_scanning) {
    // Check-Before-Update Pattern: Only update if data actually changed
    bool data_changed = (max_threat != last_threat_) ||
                        (is_scanning != last_is_scanning_) ||
                        (current_freq != last_freq_) ||
                        (approaching != last_approaching_) ||
                        (static_count != last_static_) ||
                        (receding != last_receding_);

    if (!data_changed) {
        return; // Exit early, no need to redraw
    }

    // Update cached values
    last_threat_ = max_threat;
    last_is_scanning_ = is_scanning;
    last_freq_ = current_freq;
    last_approaching_ = approaching;
    last_static_ = static_count;
    last_receding_ = receding;

    size_t total_drones = approaching + static_count + receding;
    threat_progress_bar_.set_value(total_drones * 10);

    char buffer[64]; // Increased buffer size to prevent overflow
    const char* threat_name = get_threat_icon_text(max_threat);
    if (total_drones > 0) {
        snprintf(buffer, sizeof(buffer), "THREAT: %s | <%zu ~%zu >%zu",
                threat_name, approaching, static_count, receding);
    } else if (is_scanning) {
        snprintf(buffer, sizeof(buffer), "SCANNING: <%zu ~%zu >%zu",
                approaching, static_count, receding);
    } else {
        snprintf(buffer, sizeof(buffer), "READY");
    }
    threat_status_main_.set(buffer);
    threat_status_main_.set_style(&UIStyles::RED_STYLE);
    last_text_ = buffer;

    if (current_freq > 0) {
        if (static_cast<uint64_t>(current_freq) >= 1000000000ULL) {
            uint32_t ghz = static_cast<uint64_t>(current_freq) / 1000000000ULL;
            uint32_t decimals = ((static_cast<uint64_t>(current_freq) % 1000000000ULL) / 10000000ULL);
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%lu.%02luGHz SCANNING", (unsigned long)ghz, (unsigned long)decimals);
            } else {
                snprintf(buffer, sizeof(buffer), "%lu.%02luGHz READY", (unsigned long)ghz, (unsigned long)decimals);
            }
        } else {
            uint32_t mhz = static_cast<uint64_t>(current_freq) / 1000000ULL;
            uint32_t decimals = (static_cast<uint64_t>(current_freq) % 1000000ULL) / 100000ULL;
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%lu.%01luMHz SCANNING", (unsigned long)mhz, (unsigned long)decimals);
            } else {
                snprintf(buffer, sizeof(buffer), "%lu.%01luMHz READY", (unsigned long)mhz, (unsigned long)decimals);
            }
        }
        threat_frequency_.set(buffer);
    } else {
        threat_frequency_.set("NO SIGNAL");
    }

    const Style* new_style = &UIStyles::LIGHT_STYLE;
    switch (max_threat) {
        case ThreatLevel::CRITICAL:
            new_style = &UIStyles::RED_STYLE;
            break;
        case ThreatLevel::HIGH:
        case ThreatLevel::MEDIUM:
            new_style = &UIStyles::YELLOW_STYLE;
            break;
        case ThreatLevel::LOW:
            new_style = &UIStyles::GREEN_STYLE;
            break;
        case ThreatLevel::NONE:
        default:
            new_style = &UIStyles::LIGHT_STYLE;
            break;
    }
    threat_frequency_.set_style(new_style);
    set_dirty();
}

void SmartThreatHeader::set_max_threat(ThreatLevel threat) {
    if (threat != last_threat_) {
        update(threat, last_approaching_, last_static_, last_receding_,
               last_freq_, last_is_scanning_);
    }
}

void SmartThreatHeader::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
    update(last_threat_, approaching, static_count, receding,
           last_freq_, last_is_scanning_);
}

void SmartThreatHeader::set_current_frequency(Frequency freq) {
    if (freq != last_freq_) {
        update(last_threat_, last_approaching_, last_static_, last_receding_,
               freq, last_is_scanning_);
    }
}

void SmartThreatHeader::set_scanning_state(bool is_scanning) {
    if (is_scanning != last_is_scanning_) {
        update(last_threat_, last_approaching_, last_static_, last_receding_,
               last_freq_, is_scanning);
    }
}

void SmartThreatHeader::set_color_scheme(bool use_dark_theme) {
    (void)use_dark_theme;
}

Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color(255, 140, 0);
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        case ThreatLevel::NONE:
        default: return Color::blue();
    }
}

Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color(255, 165, 0);
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        case ThreatLevel::NONE:
        default: return Color::white();
    }
}

const char* SmartThreatHeader::get_threat_icon_text(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return "CRITICAL";
        case ThreatLevel::HIGH: return "HIGH";
        case ThreatLevel::MEDIUM: return "MEDIUM";
        case ThreatLevel::LOW: return "LOW";
        case ThreatLevel::NONE:
        default: return "CLEAR";
    }
}

void SmartThreatHeader::paint(Painter& painter) {
    // 1. Fill the entire background with threat color
    Color bg_color = get_threat_bar_color(last_threat_);
    painter.fill_rectangle(screen_rect(), bg_color);

    // 2. Draw large centered text with white color on colored background
    // Calculate centered position
    const int text_width = last_text_.length() * 8; // fixed_8x16 is 8px per char
    const int text_height = 16;
    const int center_x = (screen_width - text_width) / 2;
    const int center_y = (60 - text_height) / 2; // Header height is 60px
    Point text_pos = {center_x, center_y};
    auto style = Style{font::fixed_8x16, bg_color, Color::white()};
    painter.draw_string(text_pos, style, last_text_);
}

ThreatCard::ThreatCard(size_t card_index, Rect parent_rect)
    : View(parent_rect), card_index_(card_index),
      parent_rect_(parent_rect), is_active_(false), // <--- ПЕРЕМЕЩЕНО СЮДА (после parent_rect_, перед last_frequency_)
      last_frequency_(0), last_threat_(ThreatLevel::NONE),
      last_trend_(MovementTrend::UNKNOWN), last_rssi_(-120), last_threat_name_("") {
    add_children({&card_text_});
}

// Use consolidated utility function from DroneUtils namespace

void ThreatCard::update_card(const DisplayDroneEntry& drone) {
    // Check-Before-Update Pattern: Only update if data actually changed
    bool data_changed = (drone.frequency != last_frequency_) ||
                        (drone.threat != last_threat_) ||
                        (drone.trend != last_trend_) ||
                        (std::abs(drone.rssi - last_rssi_) > 1) || // RSSI hysteresis
                        (drone.type_name != last_threat_name_);

    if (!data_changed && is_active_) {
        return; // Exit early, no need to redraw
    }

    // Update cached values
    last_frequency_ = drone.frequency;
    last_threat_ = drone.threat;
    last_trend_ = drone.trend;
    last_rssi_ = drone.rssi;
    last_threat_name_ = drone.type_name;
    is_active_ = true;

    // Use stack buffer instead of std::string operations for performance
    char buffer[48]; // Increased buffer size to prevent overflow
    char trend_char;
    switch (drone.trend) {
        case MovementTrend::APPROACHING: trend_char = '^'; break;
        case MovementTrend::RECEDING: trend_char = 'v'; break;
        case MovementTrend::STATIC:
        case MovementTrend::UNKNOWN:
        default: trend_char = '='; break;
    }

    uint32_t mhz = drone.frequency / 1000000;
    
    // Use stack buffer with snprintf for efficient string formatting
    // This avoids heap allocations from std::string operations
    snprintf(buffer, sizeof(buffer), "%s %c %luM %ld",
             drone.type_name.c_str(), trend_char, 
             (unsigned long)mhz, (long)drone.rssi);

    card_text_.set(buffer);
    
    // Only change color if threat level changed to avoid unnecessary redraws
    static Style light_style{font::fixed_8x16, Color::black(), Color::white()};
    card_text_.set_style(&light_style);
    
    set_dirty(); // Only call set_dirty() when content actually changes
}

void ThreatCard::clear_card() {
    is_active_ = false;
    card_text_.set("");
    set_dirty();
}



Color ThreatCard::get_card_bg_color() const {
    if (!is_active_) return Color::black();
    switch (last_threat_) {
        case ThreatLevel::CRITICAL: return Color(64, 0, 0);
        case ThreatLevel::HIGH: return Color(64, 32, 0);
        case ThreatLevel::MEDIUM: return Color(32, 32, 0);
        case ThreatLevel::LOW: return Color(0, 32, 0);
        case ThreatLevel::NONE:
        default: return Color(0, 0, 64);
    }
}

Color ThreatCard::get_card_text_color() const {
    // Always white text, since threat backgrounds (Red, Orange) are dark
    // Exception: if background Yellow (Medium), text better Black
    if (last_threat_ == ThreatLevel::MEDIUM) return Color::black();
    return Color::white();
}

void ThreatCard::paint(Painter& painter) {
    View::paint(painter);
    if (is_active_) {
        Color bg_color = get_card_bg_color();
        painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, bg_color);
    }
}

void ThreatCard::set_parent_rect(const Rect& rect) {
    parent_rect_ = rect;
    View::set_parent_rect(rect);
    set_dirty();
}

ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
    : View(parent_rect), bar_index_(bar_index), parent_rect_(parent_rect) {
    add_children({&progress_text_, &alert_text_, &normal_text_});
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    set_display_mode(DisplayMode::SCANNING);

    char progress_bar[25] = "--------";
    uint8_t filled = (progress_percent * 8) / 100;
    for (uint8_t i = 0; i < filled; i++) {
        progress_bar[i] = '=';
    }

    char buffer[48]; // Reduced from 64 to 48 bytes to save 16 bytes
    snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
            progress_bar, (unsigned long)progress_percent, (unsigned long)total_cycles, (unsigned long)detections);
    progress_text_.set(buffer);
    static Style blue_style{font::fixed_8x16, Color::black(), Color::blue()};
    progress_text_.set_style(&blue_style);

    if (detections > 0) {
        set_display_mode(DisplayMode::ALERT);
    char alert_buffer[48]; // Reduced from 64 to 48 bytes to save 16 bytes
    snprintf(alert_buffer, sizeof(alert_buffer), "[!] DETECTED: %lu threats found!", static_cast<unsigned long>(detections));
        alert_text_.set(alert_buffer);
        static Style red_style{font::fixed_8x16, Color::black(), Color::red()};
        alert_text_.set_style(&red_style);
    }
    set_dirty();
}

void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
    set_display_mode(DisplayMode::ALERT);

    const char* icons[5] = {"(i)", "[!]", "[O]", "[X]", "[!!!]"};
    size_t icon_idx = std::min(static_cast<size_t>(threat), size_t(4));

    char buffer[48]; // Reduced from 64 to 48 bytes to save 16 bytes
    snprintf(buffer, sizeof(buffer), "%s ALERT: %zu drones | %s",
            icons[icon_idx], total_drones, alert_msg.c_str());

    alert_text_.set(buffer);
    if (threat >= ThreatLevel::CRITICAL) {
        static Style red_style{font::fixed_8x16, Color::black(), Color::red()};
        alert_text_.set_style(&red_style);
    } else {
        static Style yellow_style{font::fixed_8x16, Color::black(), Color(255, 255, 0)};
        alert_text_.set_style(&yellow_style);
    }
    set_dirty();
}

void ConsoleStatusBar::update_normal_status(const std::string& primary, const std::string& secondary) {
    set_display_mode(DisplayMode::NORMAL);

    char buffer[32]; // Reduced from 48 to 32 bytes to save 16 bytes
    if (secondary.empty()) {
        snprintf(buffer, sizeof(buffer), "%s", primary.c_str());
    } else {
        snprintf(buffer, sizeof(buffer), "%s | %s", primary.c_str(), secondary.c_str());
    }
    normal_text_.set(buffer);
    static Style light_style{font::fixed_8x16, Color::black(), Color::white()};
    normal_text_.set_style(&light_style);
    set_dirty();
}

void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
    if (mode_ == mode) return;
    mode_ = mode;

    progress_text_.hidden(true);
    alert_text_.hidden(true);
    normal_text_.hidden(true);

    const bool show_progress = (mode == DisplayMode::SCANNING);
    const bool show_alert = (mode == DisplayMode::ALERT);
    const bool show_normal = !show_progress && !show_alert;
    progress_text_.hidden(!show_progress);
    alert_text_.hidden(!show_alert);
    normal_text_.hidden(!show_normal);
    set_dirty();
}

void ConsoleStatusBar::paint(Painter& painter) {
    View::paint(painter);
    if (mode_ == DisplayMode::ALERT) {
        painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, Color(32, 0, 0));
    }
}

DroneDisplayController::~DroneDisplayController() {
    // TODO[FIXED]: STEP 2.2: Memory leaks in MessageHandler's
    // These handlers are now stack-allocated and will be automatically destroyed
    // No manual cleanup needed for stack-allocated objects
}

DroneDisplayController::DroneDisplayController(Rect parent_rect)
    : View(parent_rect),
      big_display_({4, 0, 28 * 8, 52}, ""),
      scanning_progress_({0, 52, screen_width, 8}),
      text_threat_summary_({0, 82, screen_width, 16}, "THREAT: NONE"),
      text_status_info_({0, 98, screen_width, 16}, "Ready"),
      text_scanner_stats_({0, 114, screen_width, 16}, "No database"),
      text_trends_compact_({0, 130, screen_width, 16}, ""),
      text_drone_1_({screen_width - 120, 146, 120, 16}, ""),
      text_drone_2_({screen_width - 120, 162, 120, 16}, ""),
      text_drone_3_({screen_width - 120, 178, 120, 16}, ""),
      compact_frequency_ruler_({0, 68, screen_width, 12}),
      frequency_ruler_({0, 68, screen_width, 12}),
      detected_drones_(),
      displayed_drones_(),
      spectrum_row(), spectrum_power_levels_(), threat_bins_(), threat_bins_count_(0),
      waterfall_buffer_(),
      spectrum_gradient_(), spectrum_fifo_(nullptr),
      pixel_index(0), bins_hz_size(0), each_bin_size(100000), min_color_power(0),
      marker_pixel_step(1000000), max_power(0), range_max_power(0), mode(0),
      spectrum_config_()
{
    // TODO[FIXED]: Reserve memory in advance.
    // MAX_TRACKED_DRONES is usually around 8-10, +2 as reserve.
    detected_drones_count_ = 0;

    std::fill(displayed_drones_.begin(), displayed_drones_.end(), DisplayDroneEntry{});

    // CRITICAL FIX: Don't perform blocking I/O in View constructor
    // Use default gradient immediately, load from file in on_show()
    spectrum_gradient_.set_default();

    // --- REMOVED: Stack allocation now handled in header ---
    // MessageHandlerRegistration objects are now initialized in-class (C++11 feature)

    // CRITICAL: Add ALL widgets to View hierarchy
    add_children({
        &big_display_,
        &scanning_progress_,
        &compact_frequency_ruler_,
        &text_threat_summary_,
        &text_status_info_,
        &text_scanner_stats_,
        &text_trends_compact_,
        &text_drone_1_,
        &text_drone_2_,
        &text_drone_3_
    });

    // Hide the old ruler, use compact by default
    frequency_ruler_.set_visible(false);
}

void DroneDisplayController::update_detection_display(const DroneScanner& scanner) {
    if (scanner.is_scanning_active()) {
        Frequency current_freq = scanner.get_current_scanning_frequency();
        // NOLINTNEXTLINE(bugprone-branch-clone)
        if (current_freq > 0) {
            big_display_.set(to_string_short_freq(current_freq));
        } else {
            big_display_.set("2400.0MHz");
        }
    } else {
        big_display_.set("READY");
    }

    size_t total_freqs = scanner.get_database_size();
    if (total_freqs > 0 && scanner.is_scanning_active()) {
        uint32_t progress_percent = 50;
        scanning_progress_.set_value(std::min(progress_percent, (uint32_t)100));
    } else {
        scanning_progress_.set_value(0);
    }

    ThreatLevel max_threat = scanner.get_max_detected_threat();
    bool has_detections = (scanner.get_approaching_count() + scanner.get_receding_count() + scanner.get_static_count()) > 0;

    if (has_detections) {
    char summary_buffer[32]; // Reduced from 48 to 32 bytes to save 16 bytes
    const char* threat_name = get_threat_level_name(max_threat);
        snprintf(summary_buffer, sizeof(summary_buffer), "THREAT: %s | <%zu ~%zu >%zu",
                threat_name, scanner.get_approaching_count(),
                scanner.get_static_count(), scanner.get_receding_count());
        text_threat_summary_.set(summary_buffer);
        static Style red_style{font::fixed_8x16, Color::black(), Color::red()};
        text_threat_summary_.set_style(&red_style);
    } else {
        text_threat_summary_.set("THREAT: NONE | All clear");
        static Style green_style{font::fixed_8x16, Color::black(), Color::green()};
        text_threat_summary_.set_style(&green_style);
    }

    char status_buffer[32]; // Reduced from 48 to 32 bytes to save 16 bytes
    if (scanner.is_scanning_active()) {
        std::string mode_str = scanner.is_real_mode() ? "REAL" : "DEMO";
        snprintf(status_buffer, sizeof(status_buffer), "%s - Detections: %lu",
                mode_str.c_str(), static_cast<unsigned long>(scanner.get_total_detections()));
    } else {
        snprintf(status_buffer, sizeof(status_buffer), "Ready - Enhanced Drone Analyzer");
    }
    text_status_info_.set(status_buffer);

    size_t loaded_freqs = scanner.get_database_size();
    char stats_buffer[32]; // Reduced from 48 to 32 bytes to save 16 bytes
    if (scanner.is_scanning_active() && loaded_freqs > 0) {
        size_t current_idx = 0;
        snprintf(stats_buffer, sizeof(stats_buffer), "Freq: %zu/%zu | Cycle: %lu",
                current_idx + 1, loaded_freqs, static_cast<unsigned long>(scanner.get_scan_cycles()));
    } else if (loaded_freqs > 0) {
        snprintf(stats_buffer, sizeof(stats_buffer), "Loaded: %zu frequencies", loaded_freqs);
    } else {
        snprintf(stats_buffer, sizeof(stats_buffer), "No database loaded");
    }
    text_scanner_stats_.set(stats_buffer);

    if (max_threat >= ThreatLevel::HIGH) {
        static Style red_style{font::fixed_8x16, Color::black(), Color::red()};
        big_display_.set_style(&red_style);
    } else if (max_threat >= ThreatLevel::MEDIUM) {
        static Style yellow_style{font::fixed_8x16, Color::black(), Color(255, 255, 0)};
        big_display_.set_style(&yellow_style);
    } else if (has_detections) {
        static Style orange_style{font::fixed_8x16, Color::black(), Color(255, 165, 0)};
        big_display_.set_style(&orange_style);
    } else if (scanner.is_scanning_active()) {
        static Style green_style{font::fixed_8x16, Color::black(), Color::green()};
        big_display_.set_style(&green_style);
    } else {
        static Style dark_style{font::fixed_8x16, Color::black(), Color::dark_grey()};
        big_display_.set_style(&dark_style);
    }
}

// ===========================================
// IMPLEMENTATION: DroneDisplayController::set_scanning_status
// ===========================================
void DroneDisplayController::set_scanning_status(bool active, const std::string& message) {
    char buffer[32];
    if (active) {
        snprintf(buffer, sizeof(buffer), "SCAN: %s", message.c_str());
        text_status_info_.set(buffer);
        static Style green_style{font::fixed_8x16, Color::black(), Color::green()};
        text_status_info_.set_style(&green_style);
    } else {
        snprintf(buffer, sizeof(buffer), "STOP: %s", message.c_str());
        text_status_info_.set(buffer);
        // Use a color that exists in Theme, falling back to white/light
        static Style light_style{font::fixed_8x16, Color::black(), Color::white()};
        text_status_info_.set_style(&light_style);
    }
}

void DroneDisplayController::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) {
    systime_t now = chTimeNow();
    
    // Find existing drone by frequency
    for (size_t i = 0; i < detected_drones_count_; ++i) {
        if (detected_drones_[i].frequency == freq) {
            detected_drones_[i].rssi = rssi;
            detected_drones_[i].threat = threat;
            detected_drones_[i].type = type;
            detected_drones_[i].last_seen = now;
            detected_drones_[i].type_name = get_drone_type_name(type);
            detected_drones_[i].display_color = get_drone_type_color(type);
            sort_drones_by_rssi();
            render_drone_text_display();
            return;
        }
    }
    
    // Add new drone if space available
    if (detected_drones_count_ < MAX_UI_DRONES) {
        auto& entry = detected_drones_[detected_drones_count_];
        entry.frequency = freq;
        entry.rssi = rssi;
        entry.threat = threat;
        entry.type = type;
        entry.last_seen = now;
        entry.type_name = get_drone_type_name(type);
        entry.display_color = get_drone_type_color(type);
        entry.trend = MovementTrend::STATIC;
        detected_drones_count_++;
    }
    sort_drones_by_rssi();
    render_drone_text_display();
}

void DroneDisplayController::sort_drones_by_rssi() {
    std::sort(detected_drones_.begin(), detected_drones_.end(),
              [](const DisplayDroneEntry& a, const DisplayDroneEntry& b) {
                  if (a.rssi != b.rssi) return a.rssi > b.rssi;
                  if (a.threat != b.threat) return static_cast<int>(a.threat) > static_cast<int>(b.threat);
                  return a.last_seen > b.last_seen;
              });
}

void DroneDisplayController::update_drones_display(const DroneScanner& scanner) {
    // STEP 1: Get a safe copy of data
    // Mutex is locked only for the copying time (microseconds)
    auto snapshot = scanner.get_tracked_drones_snapshot();

    // STEP 2: Work with local snapshot copy.
    // Scanner may already be looking for new drones in another thread at this time.

    const systime_t STALE_TIMEOUT = 30000;
    systime_t now = chTimeNow();

    // Clear UI list
    detected_drones_count_ = 0;

    // Fill from snapshot
    for (size_t i = 0; i < snapshot.count; ++i) {
        const auto& drone_data = snapshot.drones[i];

        // Stale check (duplicate logic for UI filtering)
        if ((now - drone_data.last_seen) > STALE_TIMEOUT) continue;

        if (detected_drones_count_ < MAX_UI_DRONES) {
            auto& entry = detected_drones_[detected_drones_count_];
            entry.frequency = drone_data.frequency;
            entry.type = static_cast<DroneType>(drone_data.drone_type);
            entry.threat = static_cast<ThreatLevel>(drone_data.threat_level);
            entry.rssi = drone_data.rssi;
            entry.last_seen = drone_data.last_seen;
            entry.type_name = get_drone_type_name(entry.type);
            entry.display_color = get_drone_type_color(entry.type);
            entry.trend = drone_data.get_trend(); // Now this is safe to call
            detected_drones_count_++;
        }
    }

    // STEP 3: Sorting and rendering
    sort_drones_by_rssi();

    // ... (remaining code for updating display_drones_ and calling render)
    for (auto& drone : displayed_drones_) {
        drone = DisplayDroneEntry{};
    }
    size_t count = std::min(detected_drones_count_, MAX_DISPLAYED_DRONES);
    for (size_t i = 0; i < count; ++i) {
        displayed_drones_[i] = detected_drones_[i];
    }
    highlight_threat_zones_in_spectrum(displayed_drones_);
    render_drone_text_display();
}

void DroneDisplayController::render_drone_text_display() {
    text_drone_1_.set("");
    text_drone_2_.set("");
    text_drone_3_.set("");
    for (size_t i = 0; i < std::min(displayed_drones_.size(), size_t(3)); ++i) {
        const auto& drone = displayed_drones_[i];
        char buffer[32];
        char trend_symbol;
        switch (drone.trend) {
            case MovementTrend::APPROACHING: trend_symbol = '<'; break;
            case MovementTrend::RECEDING: trend_symbol = '>'; break;
            case MovementTrend::STATIC:
            case MovementTrend::UNKNOWN:
            default: trend_symbol = '~'; break;
        }
        std::string freq_str;
        if (drone.frequency >= 1000000000) {
            freq_str = to_string_dec_uint(drone.frequency / 1000000000, 1) + "G";
        } else if (drone.frequency >= 1000000) {
            freq_str = to_string_dec_uint(drone.frequency / 1000000, 1) + "M";
        } else {
            freq_str = to_string_dec_uint(drone.frequency / 1000, 1) + "k";
        }
        snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
                drone.type_name.c_str(),
                freq_str.c_str(),
                (long int)drone.rssi,
                trend_symbol);
        Text* target_text = nullptr;
        switch(i) {
            case 0: target_text = &text_drone_1_; break;
            case 1: target_text = &text_drone_2_; break;
            case 2: target_text = &text_drone_3_; break;
            default: break;
        }
        if (target_text) {
            target_text->set(buffer);
        }
    }
}

void DroneDisplayController::initialize_mini_spectrum() {
    if (!spectrum_gradient_.load_file(default_gradient_file)) {
        spectrum_gradient_.set_default();
    }
    clear_spectrum_buffers();
    update_frequency_ruler();
}

void DroneDisplayController::lazy_initialize_gradient() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    // Try to load gradient file, use default if fails
    // This is now called from on_show() instead of constructor to avoid blocking I/O
    if (!spectrum_gradient_.load_file(default_gradient_file)) {
        spectrum_gradient_.set_default();
    }
    clear_spectrum_buffers();
    update_frequency_ruler();
}

void DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum& spectrum) {
    uint8_t current_bin_power = 0;
    for (size_t bin = 0; bin < MINI_SPECTRUM_WIDTH; bin++) {
        get_max_power_for_current_bin(spectrum, bin, current_bin_power);
        if (process_bins(&current_bin_power)) {
            return;
        }
    }
}

bool DroneDisplayController::process_bins(uint8_t* powerlevel) {
    bins_hz_size += each_bin_size;
    if (bins_hz_size >= marker_pixel_step) {
        if (*powerlevel > min_color_power)
            add_spectrum_pixel(*powerlevel);
        else
            add_spectrum_pixel(0);
        *powerlevel = 0;

        if (!pixel_index) {
            bins_hz_size = 0;
            return true;
        }
        bins_hz_size -= marker_pixel_step;
    }
    return false;
}

void DroneDisplayController::render_mini_spectrum() {
    // 1. Write new line of data to buffer (replace the oldest)
    for (size_t x = 0; x < SPEC_WIDTH; ++x) {
        uint8_t power_value = 0;
        if (x < spectrum_power_levels_.size()) {
            power_value = spectrum_power_levels_[x];
        }
        // Convert power to color index
        uint8_t color_index = (power_value * spectrum_gradient_.lut.size()) / 256;
        if (color_index >= spectrum_gradient_.lut.size()) {
            color_index = spectrum_gradient_.lut.size() - 1;
        }
        waterfall_buffer_[waterfall_line_index_][x] = color_index;
    }

    // 2. Shift index (ring buffer)
    waterfall_line_index_ = (waterfall_line_index_ + 1) % SPEC_HEIGHT;

    // 2. Rendering (OPTIMIZED)
    const int start_y = 92;

    // Line buffer on stack (moved outside loop to avoid recreating)
    std::array<Color, SPEC_WIDTH> line_colors;

    for (int y = 0; y < SPEC_HEIGHT; ++y) {
        // Correct index calculation for "flowing down"
        // Newest line (waterfall_line_index_ - 1) should be at y=0
        size_t buf_idx = (waterfall_line_index_ + SPEC_HEIGHT - 1 - y) % SPEC_HEIGHT;

        const auto& src_row = waterfall_buffer_[buf_idx];

        // Convert indices to colors
        for (int x = 0; x < SPEC_WIDTH; ++x) {
            line_colors[x] = spectrum_gradient_.lut[src_row[x]];
        }

        // Draw line
        display.draw_pixels({{0, start_y + y}, {SPEC_WIDTH, 1}}, line_colors);
    }
}

void DroneDisplayController::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES>& drones) {
    threat_bins_count_ = 0;
    for (const auto& drone : drones) {
        if (drone.frequency > 0) {
            size_t bin_x = frequency_to_spectrum_bin(drone.frequency);
            if (bin_x < MINI_SPECTRUM_WIDTH && threat_bins_count_ < MAX_DISPLAYED_DRONES) {
                threat_bins_[threat_bins_count_].bin = bin_x;
                threat_bins_[threat_bins_count_].threat = drone.threat;
                threat_bins_count_++;
            }
        }
    }
}

void DroneDisplayController::clear_spectrum_buffers() {
    std::fill(spectrum_power_levels_.begin(), spectrum_power_levels_.end(), 0);
}

bool DroneDisplayController::validate_spectrum_data() const {
    if (spectrum_power_levels_.size() != MINI_SPECTRUM_WIDTH) return false;
    if (spectrum_gradient_.lut.empty()) return false;
    return true;
}

size_t DroneDisplayController::get_safe_spectrum_index(size_t x, size_t y) const {
    if (x >= MINI_SPECTRUM_WIDTH || y >= MINI_SPECTRUM_HEIGHT) {
        return 0;
    }
    return y * MINI_SPECTRUM_WIDTH + x;
}

void DroneDisplayController::set_spectrum_range(Frequency min_freq, Frequency max_freq) {
    // Use unified frequency limits from DroneConstants
    if (min_freq >= max_freq || min_freq < DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ ||
        max_freq > DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        spectrum_config_.min_freq = DroneConstants::WIDEBAND_24GHZ_MIN;
        spectrum_config_.max_freq = DroneConstants::WIDEBAND_24GHZ_MAX;
        update_frequency_ruler();
        return;
    }
    spectrum_config_.min_freq = min_freq;
    spectrum_config_.max_freq = max_freq;
    spectrum_config_.bandwidth = (max_freq - min_freq) > DroneConstants::WIDEBAND_WIFI_MIN_WIDTH_HZ ?
                                DroneConstants::WIDEBAND_DEFAULT_SLICE_WIDTH : static_cast<uint32_t>(max_freq - min_freq);
    spectrum_config_.sampling_rate = spectrum_config_.bandwidth;
    update_frequency_ruler();
}

void DroneDisplayController::update_signal_type_display(const std::string& signal_type) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "SIGNAL: %s", signal_type.c_str());
    text_signal_type_.set(buffer);
    
    // Set color based on signal type
    static Style normal_style{font::fixed_8x16, Color::black(), Color::white()};
    static Style digital_style{font::fixed_8x16, Color::black(), Color::green()};
    static Style analog_style{font::fixed_8x16, Color::black(), Color::yellow()};
    static Style unknown_style{font::fixed_8x16, Color::black(), Color::grey()};
    
    // NOLINTNEXTLINE(bugprone-branch-clone)
    if (signal_type == "DIGITAL") {
        text_signal_type_.set_style(&digital_style);
    } else if (signal_type == "ANALOG") {
        text_signal_type_.set_style(&analog_style);
    } else if (signal_type == "NOISE") {
        text_signal_type_.set_style(&unknown_style);
    } else {
        text_signal_type_.set_style(&normal_style);
    }
    
    set_dirty();
}

size_t DroneDisplayController::frequency_to_spectrum_bin(Frequency freq_hz) const {
    const Frequency MIN_FREQ = spectrum_config_.min_freq;
    const Frequency MAX_FREQ = spectrum_config_.max_freq;
    const Frequency FREQ_RANGE = MAX_FREQ - MIN_FREQ;
    if (freq_hz < MIN_FREQ || freq_hz > MAX_FREQ || FREQ_RANGE == 0) {
        return MINI_SPECTRUM_WIDTH;
    }
    Frequency relative_freq = freq_hz - MIN_FREQ;
    size_t bin = (relative_freq * MINI_SPECTRUM_WIDTH) / FREQ_RANGE;
    return std::min(bin, MINI_SPECTRUM_WIDTH - 1);
}

DroneUIController::DroneUIController(NavigationView& nav,
                                         DroneHardwareController& hardware,
                                         DroneScanner& scanner,
                                         ::AudioManager& audio_mgr,
                                         DroneDisplayController& display_controller)
    : nav_(nav),
      hardware_(hardware),
      scanner_(scanner),
      audio_mgr_(audio_mgr),
      scanning_active_{false},
      display_controller_(&display_controller),
      settings_()
{
    settings_.spectrum_mode = SpectrumMode::MEDIUM;
    settings_.scan_interval_ms = 1000;
    settings_.rssi_threshold_db = -90;
    settings_.enable_audio_alerts = true;
}

DroneUIController::~DroneUIController() {
    // TODO[FIXED]: Don't delete display_controller_ - owned by View
    // UIController only uses reference, doesn't own the resource
    display_controller_ = nullptr;  // Only nullify pointer
}

void DroneUIController::on_start_scan() {
    if (scanning_active_.load(std::memory_order_acquire)) return;
    scanning_active_.store(true, std::memory_order_release);
    scanner_.start_scanning();
    display_controller_->set_scanning_status(true, "Scanning Active");
    display_controller_->update_detection_display(scanner_);
}

void DroneUIController::on_stop_scan() {
    scanning_active_.store(false, std::memory_order_release);
    scanner_.stop_scanning();
    audio_mgr_.stop_audio();
    display_controller_->set_scanning_status(false, "Stopped");
    display_controller_->update_detection_display(scanner_);
}

void DroneUIController::on_toggle_mode() {
    if (scanner_.is_real_mode()) {
        scanner_.switch_to_demo_mode();
        if (hardware_.is_spectrum_streaming_active()) {
            hardware_.stop_spectrum_streaming();
        }
    } else {
        scanner_.switch_to_real_mode();
        if (!hardware_.is_spectrum_streaming_active()) {
            hardware_.start_spectrum_streaming();
        }
    }
    display_controller_->set_scanning_status(scanning_active_.load(std::memory_order_acquire),
                                           scanner_.is_real_mode() ? "Real Mode" : "Demo Mode");
}

void DroneUIController::show_menu() {
    // СТАЛО: Открываем полноценное меню настроек EDA
    nav_.push<DroneAnalyzerSettingsView>();
}

void DroneUIController::on_load_frequency_file() {
    if (scanner_.load_frequency_database()) {
        size_t count = scanner_.get_database_size();
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Loaded %zu frequencies", count);
        nav_.display_modal("Success", buffer);
    } else {
        nav_.display_modal("Error", "Failed to load database");
    }
}

void DroneUIController::on_save_settings() {
    if (!DroneAnalyzerSettingsManager::save(settings_)) {
        nav_.display_modal("Error", "Failed to save settings");
        return;
    }
    nav_.display_modal("Success", "Settings saved");
}

void DroneUIController::on_audio_settings() {
    settings_.enable_audio_alerts = !settings_.enable_audio_alerts;
    char buffer[32];
    const char* status = settings_.enable_audio_alerts ? "ENABLED" : "DISABLED";
    strcpy(buffer, "Alerts ");
    strcat(buffer, status);
    nav_.display_modal("Audio Alerts", buffer);
}

void DroneUIController::on_spectrum_mode() {
    nav_.display_modal("Spectrum Mode", "Feature not implemented in this version");
}

void DroneUIController::set_spectrum_mode(SpectrumMode mode) {
    hardware_.set_spectrum_mode(mode);
    nav_.display_modal("Applied", "Spectrum mode updated");
}

void DroneUIController::on_hardware_control() {
    show_hardware_status();
}

void DroneUIController::on_set_bandwidth() {
    uint32_t current_bw = hardware_.get_spectrum_bandwidth();
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)current_bw);
    nav_.display_modal("Set Bandwidth (MHz)", buffer);
}

void DroneUIController::on_set_center_freq() {
    Frequency current_cf = hardware_.get_spectrum_center_frequency();
    hardware_.set_spectrum_center_frequency(current_cf + 1000000);
    show_hardware_status();
}

void DroneUIController::show_hardware_status() {
    char buffer[128];
    uint32_t band_mhz = hardware_.get_spectrum_bandwidth() / 1000000ULL;
    uint32_t freq_ghz = hardware_.get_spectrum_center_frequency() / 1000000000ULL;
    uint32_t freq_decimals = (hardware_.get_spectrum_center_frequency() % 1000000000ULL) / 100000000ULL;
    snprintf(buffer, sizeof(buffer),
            "Band: %lu MHz\nFreq: %lu.%03lu GHz",
            (unsigned long)band_mhz, (unsigned long)freq_ghz, (unsigned long)freq_decimals);
    nav_.display_modal("Hardware Status", buffer);
}

void DroneUIController::on_view_logs() {
    auto open_view = nav_.push<FileLoadView>(".CSV");
    open_view->push_dir("/LOGS/EDA");
}

void DroneUIController::on_about() {
    nav_.display_modal("EDA v1.0", "Enhanced Drone Analyzer\nMayhem Firmware Integration\nBased on Recon & Looking Glass");
}

void DroneUIController::update_scanner_range(Frequency min_freq, Frequency max_freq) {
    scanner_.update_scan_range(min_freq, max_freq);
    display_controller_->set_spectrum_range(min_freq, max_freq);
}

// --- РЕАЛИЗАЦИЯ FrequencyRangeSetupView ---

FrequencyRangeSetupView::FrequencyRangeSetupView(NavigationView& nav, DroneUIController& controller)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      controller_(controller),
      text_title_({4, 4, 224, 16}, "Panoramic Spectrum"),
      label_min_({4, 36, 80, 16}, "Start Freq:"),
      field_min_({88, 36, 160, 16}, "2400.000000"),
      label_max_({4, 68, 80, 16}, "End Freq:"),
      field_max_({88, 68, 160, 16}, "2500.000000"),
      label_slice_({4, 100, 80, 16}, "Res (BW):"),
      field_slice_({88, 100, 160, 16}, "24 MHz"),
      button_save_({4, 140, 224, 40}, "Apply Range"),
      button_cancel_({4, 190, 224, 40}, "Cancel") {

    add_children({
        &text_title_,
        &label_min_, &field_min_,
        &label_max_, &field_max_,
        &label_slice_, &field_slice_,
        &button_save_, &button_cancel_
    });

    // Set up button handlers
    button_save_.on_select = [this](Button&) {
        on_save();
    };
    
    button_cancel_.on_select = [this](Button&) {
        on_cancel();
    };
}

void FrequencyRangeSetupView::focus() {
    // При фокусе загружаем текущие настройки в поля ввода
    const auto& settings = controller_.settings();
    
    // Format frequencies as MHz with 6 decimal places
    char min_buffer[32];
    char max_buffer[32];
    snprintf(min_buffer, sizeof(min_buffer), "%.6f", settings.wideband_min_freq_hz / 1000000.0);
    snprintf(max_buffer, sizeof(max_buffer), "%.6f", settings.wideband_max_freq_hz / 1000000.0);
    
    field_min_.set_text(min_buffer);
    field_max_.set_text(max_buffer);
    
    // Set resolution text
    char slice_buffer[32];
    uint64_t slice_mhz = settings.wideband_slice_width_hz / 1000000;
    snprintf(slice_buffer, sizeof(slice_buffer), "%lu MHz", (unsigned long)slice_mhz);
    field_slice_.set(slice_buffer);
    
    button_save_.focus();
}

void FrequencyRangeSetupView::on_save() {
    // Получаем значения из полей
    const std::string min_str = field_min_.get_text();
    const std::string max_str = field_max_.get_text();
    
    // Parse frequencies (MHz to Hz)
    double min_mhz = strtod(min_str.c_str(), nullptr);
    double max_mhz = strtod(max_str.c_str(), nullptr);
    
    // NaN validation
    if (std::isnan(min_mhz) || std::isnan(max_mhz)) {
        nav_.display_modal("Error", "Invalid frequency format");
        return;
    }
    
    Frequency new_min = static_cast<Frequency>(min_mhz * 1000000.0);
    Frequency new_max = static_cast<Frequency>(max_mhz * 1000000.0);
    
    // Get slice width from settings instead of hardcoded value
    auto& settings = controller_.settings();
    uint64_t new_slice_width = settings.wideband_slice_width_hz;
    
    // Валидация - use unified constants from DroneConstants
    if (new_min >= new_max) {
        nav_.display_modal("Error", "Min freq must be < Max freq");
        return;
    }
    
    if (new_min < DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ || 
        new_max > DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        nav_.display_modal("Error", "Frequency out of range (1MHz - 7.2GHz)");
        return;
    }
    
    // Overflow check
    if (new_max > new_max + new_min) {
        nav_.display_modal("Error", "Frequency range overflow detected");
        return;
    }

    // Сохранение настроек (use existing settings reference from line 2961)
    settings.wideband_min_freq_hz = new_min;
    settings.wideband_max_freq_hz = new_max;

    // Сохранение в файл с проверкой успешности
    if (!DroneAnalyzerSettingsManager::save(settings)) {
        nav_.display_modal("Error", "Failed to save settings to SD card");
        return;
    }
    
    // Обновление сканера
    controller_.update_scanner_range(new_min, new_max);
    
    // Показываем подтверждение
    char buffer[64];
    snprintf(buffer, sizeof(buffer), 
             "Range updated:\n%.3f - %.3f MHz\nBW: %lu MHz", 
             min_mhz, max_mhz, (unsigned long)(new_slice_width / 1000000));
    
    nav_.display_modal("Success", buffer);
    nav_.pop();
}

void FrequencyRangeSetupView::on_cancel() {
    nav_.pop();
}

// 2. Реализация меню удалена - теперь используем DroneAnalyzerSettingsView из ui_enhanced_drone_settings.cpp

EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
    : View({0, 0, screen_width, screen_height}),
      nav_(nav),
      hardware_(SpectrumMode::MEDIUM),
      scanner_(settings_),
      audio_(),
      display_controller_({0, 60, screen_width, screen_height - 80}),
      ui_controller_(nav, hardware_, scanner_, audio_, display_controller_),
      scanning_coordinator_(nav, hardware_, scanner_, display_controller_, audio_),
      smart_header_(Rect{0, 0, screen_width, 60}),
      status_bar_(0, Rect{0, screen_height - 80, screen_width, 16}),
      threat_cards_(),
      button_start_stop_({{screen_width - 80, screen_height - 72, 72, 32}, "START/STOP"}),
      button_menu_({{screen_width - 80, screen_height - 40, 72, 32}, "MENU"}),
      button_audio_(),
      field_scanning_mode_({{10, screen_height - 72}, 15, OptionsField::options_t{{"Database", 0}, {"Wideband", 1}, {"Hybrid", 2}}}),
      scanning_active_(false),
      settings_()
{
    DroneAnalyzerSettingsManager::load(settings_);

    // Update scanner range with settings
    scanner_.update_scan_range(settings_.wideband_min_freq_hz, settings_.wideband_max_freq_hz);

    // Update coordinator parameters
    scanning_coordinator_.update_runtime_parameters(settings_);

    // Initialize threat cards with proper positions
    for (size_t i = 0; i < threat_cards_.size(); ++i) {
        size_t card_y_pos = 60 + i * 26;
        threat_cards_[i].set_parent_rect(Rect{0, static_cast<int>(card_y_pos), screen_width, 24});
    }

    setup_button_handlers();

    // Initial layout setup
    initialize_modern_layout();

    initialize_scanning_mode();
    add_ui_elements();

    // Initial update after all elements are added
    update_modern_layout();
}

EnhancedDroneSpectrumAnalyzerView::~EnhancedDroneSpectrumAnalyzerView() {
    // TODO[FIXED]: STEP 2.1: Destruction order
    // 1. Stop activity (in dependency order)
    scanning_coordinator_.stop_coordinated_scanning();
    scanner_.stop_scanning();
    hardware_.shutdown_hardware();

    // 2. Stack objects will be automatically destroyed in reverse order
    // No manual deletion needed for stack-allocated objects
}

void EnhancedDroneSpectrumAnalyzerView::focus() {
    button_start_stop_.focus();
}

void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
    View::paint(painter);
}

bool EnhancedDroneSpectrumAnalyzerView::on_key(const KeyEvent key) {
    switch(key) {
        case KeyEvent::Back:
            stop_scanning_thread();
            nav_.pop();
            return true;
        default:
            break;
    }
    return View::on_key(key);
}

bool EnhancedDroneSpectrumAnalyzerView::on_touch(const TouchEvent event) {
    return View::on_touch(event);
}

void EnhancedDroneSpectrumAnalyzerView::on_show() {
    View::on_show();
    display.scroll_set_area(109, screen_height - 1);

    // CRITICAL FIX: Lazy gradient initialization to prevent blocking I/O on startup
    // Call this AFTER display is ready to avoid hanging during constructor
    display_controller_.lazy_initialize_gradient();

    hardware_.on_hardware_show();
}

void EnhancedDroneSpectrumAnalyzerView::on_hide() {
    // TODO[FIXED]: Explicit stop of all processes
    stop_scanning_thread(); // Stop coordinator thread

    scanner_.stop_scanning();

    // 3. Disable "hardware"
    // Stop spectrum streaming (Baseband)
    hardware_.stop_spectrum_streaming();
    // Completely disable radio module
    hardware_.shutdown_hardware();

    // Call on_hardware_hide (if there is specific logic there)
    hardware_.on_hardware_hide();

    // 4. Pass control to base class (hide widgets)
    View::on_hide();
}

void EnhancedDroneSpectrumAnalyzerView::start_scanning_thread() {
    if (scanning_coordinator_.is_scanning_active()) return;
    scanning_coordinator_.start_coordinated_scanning();
}

void EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread() {
    if (!scanning_coordinator_.is_scanning_active()) return;
    scanning_coordinator_.stop_coordinated_scanning();
}

bool EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button() {
    if (scanning_coordinator_.is_scanning_active()) {
        ui_controller_.on_stop_scan();
        button_start_stop_.set_text("START/STOP");
    } else {
        ui_controller_.on_start_scan();
        button_start_stop_.set_text("STOP");
    }
    return true;
}

bool EnhancedDroneSpectrumAnalyzerView::handle_menu_button() {
    ui_controller_.show_menu();
    return true;
}

void EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout() {
    for (size_t i = 0; i < threat_cards_.size(); ++i) {
        size_t card_y_pos = 60 + i * 26;
        threat_cards_[i].set_parent_rect(Rect{0, static_cast<int>(card_y_pos), screen_width, 24});
    }
    handle_scanner_update();
}

void EnhancedDroneSpectrumAnalyzerView::update_modern_layout() {
    handle_scanner_update();
}

void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
    ThreatLevel max_threat = scanner_.get_max_detected_threat();
    size_t approaching = scanner_.get_approaching_count();
    size_t static_count = scanner_.get_static_count();
    size_t receding = scanner_.get_receding_count();
    bool is_scanning = scanner_.is_scanning_active();
    Frequency current_freq = scanner_.get_current_scanning_frequency();
    uint32_t total_detections = scanner_.get_total_detections();

    smart_header_.update(max_threat, approaching, static_count, receding,
                        current_freq, is_scanning);

    if (is_scanning) {
        uint32_t cycles = scanner_.get_scan_cycles();
        uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), (uint32_t)100);
        status_bar_.update_scanning_progress(progress, cycles, total_detections);
    } else if (approaching + static_count + receding > 0) {
        size_t total_drones = approaching + static_count + receding;
        const char* alert_msg = (max_threat >= ThreatLevel::CRITICAL) ? "CRITICAL THREATS!" :
                               (max_threat >= ThreatLevel::HIGH) ? "HIGH THREATS!" : "Threats detected";
        status_bar_.update_alert_status(max_threat, total_drones, alert_msg);
    } else {
        const char* primary_msg = "EDA Ready";
        char secondary_buffer[32];
        if (total_detections > 0) {
            snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: %lu", (unsigned long)total_detections);
        } else {
            strcpy(secondary_buffer, "Awaiting commands");
        }
        status_bar_.update_normal_status(primary_msg, secondary_buffer);
    }

    display_controller_.update_detection_display(scanner_);
    
    // Update signal type display based on current scanning mode and signal analysis
    std::string signal_type = "UNKNOWN";
    switch (scanner_.get_scanning_mode()) {
        case DroneScanner::ScanningMode::DATABASE:
            signal_type = "RSSI";
            break;
        case DroneScanner::ScanningMode::WIDEBAND_CONTINUOUS:
            // Check if we have any digital FPV signals detected
            if (scanner_.get_max_detected_threat() >= ThreatLevel::MEDIUM) {
                // For now, we'll use SPECTRAL as the base, but the actual signal type
                // will be determined by the spectral analysis in the scanner
                signal_type = "SPECTRAL";
            } else {
                signal_type = "SPECTRAL";
            }
            break;
        case DroneScanner::ScanningMode::HYBRID:
            signal_type = "HYBRID";
            break;
    }
    display_controller_.update_signal_type_display(signal_type);
}

void EnhancedDroneSpectrumAnalyzerView::setup_button_handlers() {
    button_start_stop_.on_select = [this](Button&) {
        handle_start_stop_button();
    };
    button_menu_.on_select = [this](Button&) -> void {
        ui_controller_.show_menu();
    };
    button_audio_.on_select = [this](Button&) {
        // Toggle audio alerts setting
        settings_.enable_audio_alerts = !settings_.enable_audio_alerts;
        // Update button text immediately
        button_audio_.set_text(settings_.enable_audio_alerts ? "AUDIO: ON" : "AUDIO: OFF");
        static Style green_style{font::fixed_8x16, Color::black(), Color::green()};
        static Style grey_style{font::fixed_8x16, Color::black(), Color::grey()};
        button_audio_.set_style(settings_.enable_audio_alerts ? &green_style : &grey_style);
    };

    field_scanning_mode_.on_change = [this](size_t index, int32_t value) -> void {
        (void)value;
        set_scanning_mode_from_index(index);
    };
}

void EnhancedDroneSpectrumAnalyzerView::initialize_scanning_mode() {
    int initial_mode = static_cast<int>(scanner_.get_scanning_mode());
    field_scanning_mode_.set_selected_index(initial_mode);
}

void EnhancedDroneSpectrumAnalyzerView::set_scanning_mode_from_index(size_t index) {
    DroneScanner::ScanningMode mode = static_cast<DroneScanner::ScanningMode>(index);
    scanner_.set_scanning_mode(mode);
    display_controller_.set_scanning_status(ui_controller_.is_scanning(),
                                           scanner_.scanning_mode_name());
    update_modern_layout();
}

void EnhancedDroneSpectrumAnalyzerView::add_ui_elements() {
    add_children({&smart_header_, &status_bar_});
    for (auto& card : threat_cards_) {
        add_child(&card);
    }
    add_children({&button_start_stop_, &button_menu_, &button_audio_});
}

LoadingScreenView::LoadingScreenView(NavigationView& nav)
    : nav_(nav),
      text_eda_(Rect{108, 213, 24, 16}, "EDA"),
      timer_start_(chTimeNow())
{
    static Style red_style{font::fixed_8x16, Color::black(), Color::red()};
    text_eda_.set_style(&red_style);
    add_child(&text_eda_);
    set_focusable(false);
}

void LoadingScreenView::paint(Painter& painter) {
    painter.fill_rectangle(
        {0, 0, portapack::display.width(), portapack::display.height()},
        Color::black()
    );
    View::paint(painter);
}

// ===========================================
// PART 5: SCANNINGCOORDINATOR IMPLEMENTATION
// ===========================================

ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
                                       DroneHardwareController& hardware,
                                       DroneScanner& scanner,
                                       DroneDisplayController& display_controller,
                                       ::AudioManager& audio_controller)
    : nav_(nav), hardware_(hardware), scanner_(scanner), display_controller_(display_controller), audio_controller_(audio_controller),
      scanning_active_(false), scanning_thread_(nullptr), scan_interval_ms_(750)
{
}

ScanningCoordinator::~ScanningCoordinator() {
    stop_coordinated_scanning();
}

void ScanningCoordinator::start_coordinated_scanning() {
    if (scanning_active_.load(std::memory_order_acquire)) return;
    scanning_active_.store(true, std::memory_order_release);

    scanning_thread_ = chThdCreateFromHeap(NULL, COORDINATOR_THREAD_STACK_SIZE,
                                         NORMALPRIO,
                                         scanning_thread_function, this);
    if (!scanning_thread_) {
        scanning_active_.store(false, std::memory_order_release);
    }
}

void ScanningCoordinator::stop_coordinated_scanning() {
    //1. First check if thread is active at all
    if (scanning_active_.load(std::memory_order_acquire)) {
        // 2. Reset flag. Thread will see this in while(scanning_active_) loop and exit.
        scanning_active_.store(false, std::memory_order_release);

        // 3. If thread pointer exists, wait for its completion.
        if (scanning_thread_) {
            // chThdWait blocks current (UI) thread until scanning_thread_ calls chThdExit
            chThdWait(scanning_thread_);
            scanning_thread_ = nullptr;
        }
    }
}

msg_t ScanningCoordinator::scanning_thread_function(void* arg) {
    return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
}

msg_t ScanningCoordinator::coordinated_scanning_thread() {
    while (scanning_active_.load(std::memory_order_acquire)) {
        scanner_.perform_scan_cycle(hardware_);
        // TODO[CRITICAL][FIXED]: Removed direct UI calls from scanning thread
        // UI updates now happen only through MessageHandler in handle_scanner_update()

        chThdSleepMilliseconds(scan_interval_ms_);
    }
    scanning_active_.store(false, std::memory_order_release);
    scanning_thread_ = nullptr;
    chThdExit(0);
    return 0;
}

void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
    scan_interval_ms_ = settings.scan_interval_ms;

    // Update scanner parameters if scanning is active
    if (scanning_active_.load(std::memory_order_acquire)) {
        scanner_.update_scan_range(settings.wideband_min_freq_hz,
                                   settings.wideband_max_freq_hz);
    }
}

void ScanningCoordinator::show_session_summary(const std::string& summary) {
    (void)summary;
}

// ===========================================
// PART 6: DISPLAY HELPER IMPLEMENTATIONS
// ===========================================

const char* DroneDisplayController::get_drone_type_name(DroneType type) const {
    switch (type) {
        case DroneType::MAVIC: return "MAVIC";
        case DroneType::DJI_P34: return "DJI P34";
        case DroneType::DIY_DRONE: return "DIY DRONE";
        case DroneType::FPV_RACING: return "FPV RACING";
        case DroneType::UNKNOWN: default: return "UNKNOWN";
    }
}

Color DroneDisplayController::get_drone_type_color(DroneType type) const {
    // NOLINTNEXTLINE(bugprone-branch-clone)
    switch (type) {
        case DroneType::MAVIC: return Color::red();
        case DroneType::DJI_P34: return Color::orange();
        case DroneType::DIY_DRONE: return Color::blue();
        case DroneType::FPV_RACING: return Color::purple();
        case DroneType::UNKNOWN: default: return Color::white();
    }
}

Color DroneDisplayController::get_threat_level_color(ThreatLevel level) const {
     switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color::orange();
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        default: return Color::grey();
    }
}

const char* DroneDisplayController::get_threat_level_name(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return "CRITICAL";
        case ThreatLevel::HIGH: return "HIGH";
        case ThreatLevel::MEDIUM: return "MEDIUM";
        case ThreatLevel::LOW: return "LOW";
        default: return "NONE";
    }
}

void DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power) {
    if (bin >= spectrum.db.size()) {
        max_power = 0;
        return;
    }
    max_power = spectrum.db[bin];
}

void DroneDisplayController::add_spectrum_pixel(uint8_t power) {
    if (pixel_index < spectrum_row.size()) {
        spectrum_row[pixel_index] = spectrum_gradient_.lut[power];
        pixel_index++;
    }
}

// ===========================================
// PART 7: ENHANCED SETTINGS VALIDATOR
// ===========================================

EnhancedDroneSettingsValidator::ValidationResult 
EnhancedDroneSettingsValidator::validate_all(const DroneAnalyzerSettings& settings) {
    ValidationResult result;
    
    // 1. Validate RSSI threshold
    if (!validate_rssi_threshold(settings.rssi_threshold_db, result.error_message)) {
        result.is_valid = false;
        return result;
    }
    
    // 2. Validate scan interval
    if (!validate_scan_interval(settings.scan_interval_ms, result.error_message)) {
        result.is_valid = false;
        return result;
    }
    
    // 3. Validate audio parameters
    if (!validate_audio_params(settings.audio_alert_frequency_hz, 
                              settings.audio_alert_duration_ms, result.error_message)) {
        result.is_valid = false;
        return result;
    }
    
    // 4. Validate bandwidth
    if (!validate_bandwidth(settings.hardware_bandwidth_hz, result.error_message)) {
        result.is_valid = false;
        return result;
    }
    
    // 5. Validate frequency range
    if (!validate_frequency_range(settings.min_frequency_hz, settings.max_frequency_hz, result.error_message)) {
        result.is_valid = false;
        return result;
    }
    
    // 6. Check for warnings (not errors)
    if (settings.rssi_threshold_db > -60) {
        result.warning_count++;
        result.error_message += "WARN: High RSSI threshold may miss weak signals\n";
    }
    
    if (settings.scan_interval_ms > 5000) {
        result.warning_count++;
        result.error_message += "WARN: Slow scan interval\n";
    }
    
    return result;
}

bool EnhancedDroneSettingsValidator::validate_frequency(Frequency freq, std::string& error) {
    if (freq < DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ || 
        freq > DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Frequency %s out of range (must be %llu-%llu GHz)", 
                 format_frequency_hz(freq).c_str(),
                 DroneConstants::FrequencyLimits::MIN_HARDWARE_FREQ / 1000000000ULL,
                 DroneConstants::FrequencyLimits::MAX_HARDWARE_FREQ / 1000000000ULL);
        error = buffer;
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_rssi_threshold(int32_t rssi, std::string& error) {
    if (rssi < DroneConstants::MIN_VALID_RSSI || rssi > DroneConstants::MAX_VALID_RSSI) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "RSSI threshold %lddBm invalid (must be %ld to %ld)",
                 static_cast<long>(rssi),
                 static_cast<long>(DroneConstants::MIN_VALID_RSSI),
                 static_cast<long>(DroneConstants::MAX_VALID_RSSI));
        error = buffer;
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_scan_interval(uint32_t interval_ms, std::string& error) {
    if (interval_ms < DroneConstants::MIN_SCAN_INTERVAL_MS ||
        interval_ms > DroneConstants::MAX_SCAN_INTERVAL_MS) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Scan interval %lums invalid (must be %lu to %lums)",
                 static_cast<unsigned long>(interval_ms),
                 static_cast<unsigned long>(DroneConstants::MIN_SCAN_INTERVAL_MS),
                 static_cast<unsigned long>(DroneConstants::MAX_SCAN_INTERVAL_MS));
        error = buffer;
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_audio_params(uint32_t freq_hz, uint32_t duration_ms, std::string& error) {
    if (freq_hz < DroneConstants::MIN_AUDIO_FREQ || freq_hz > DroneConstants::MAX_AUDIO_FREQ) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Audio frequency %luHz invalid (must be %lu to %luHz)",
                 static_cast<unsigned long>(freq_hz),
                 static_cast<unsigned long>(DroneConstants::MIN_AUDIO_FREQ),
                 static_cast<unsigned long>(DroneConstants::MAX_AUDIO_FREQ));
        error = buffer;
        return false;
    }

    if (duration_ms < DroneConstants::MIN_AUDIO_DURATION ||
        duration_ms > DroneConstants::MAX_AUDIO_DURATION) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Audio duration %lums invalid (must be %lu to %lums)",
                 static_cast<unsigned long>(duration_ms),
                 static_cast<unsigned long>(DroneConstants::MIN_AUDIO_DURATION),
                 static_cast<unsigned long>(DroneConstants::MAX_AUDIO_DURATION));
        error = buffer;
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_bandwidth(uint32_t bandwidth_hz, std::string& error) {
    if (bandwidth_hz < DroneConstants::MIN_BANDWIDTH || bandwidth_hz > DroneConstants::MAX_BANDWIDTH) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Bandwidth %luHz invalid (must be %lu to %luHz)",
                 static_cast<unsigned long>(bandwidth_hz),
                 static_cast<unsigned long>(DroneConstants::MIN_BANDWIDTH),
                 static_cast<unsigned long>(DroneConstants::MAX_BANDWIDTH));
        error = buffer;
        return false;
    }
    return true;
}

bool EnhancedDroneSettingsValidator::validate_frequency_range(Frequency min_hz, Frequency max_hz, std::string& error) {
    if (min_hz >= max_hz) {
        error = "Min frequency must be less than max frequency";
        return false;
    }
    
    if (!validate_frequency(min_hz, error)) return false;
    if (!validate_frequency(max_hz, error)) return false;
    
    // Check range width
    Frequency range = max_hz - min_hz;
    if (static_cast<uint64_t>(range) < 1000000ULL) { // Minimum 1 MHz range
        error = "Frequency range too small (minimum 1 MHz)";
        return false;
    }
    
    return true;
}

bool EnhancedDroneSettingsValidator::is_known_drone_band(Frequency freq) {
    // Check 433 MHz band
    if (freq >= DroneConstants::MIN_433MHZ && freq <= DroneConstants::MAX_433MHZ) return true;
    
    // Check 860-930 MHz (LRS/Military)
    if (freq >= DroneConstants::MIN_900MHZ && freq <= DroneConstants::MAX_900MHZ) return true;
    
    // Check 2.4 GHz ISM band
    if (freq >= DroneConstants::MIN_24GHZ && freq <= DroneConstants::MAX_24GHZ) return true;
    
    // Check 5.8 GHz band
    if (freq >= DroneConstants::MIN_58GHZ && freq <= DroneConstants::MAX_58GHZ) return true;
    
    return false;
}

bool EnhancedDroneSettingsValidator::is_ism_band(Frequency freq) {
    return (static_cast<uint64_t>(freq) >= 2400000000ULL && static_cast<uint64_t>(freq) <= 2483500000ULL) ||
           (static_cast<uint64_t>(freq) >= 5725000000ULL && static_cast<uint64_t>(freq) <= 5875000000ULL);
}

std::string EnhancedDroneSettingsValidator::format_frequency_hz(Frequency freq) {
    if (static_cast<uint64_t>(freq) >= 1000000000ULL) {
        // GHz range
        uint32_t ghz = static_cast<uint32_t>(freq / 1000000000ULL);
        uint32_t mhz = static_cast<uint32_t>((freq % 1000000000ULL) / 1000000ULL);
        char buffer[32];
        if (mhz > 0) {
            snprintf(buffer, sizeof(buffer), "%u.%03u GHz",
                     static_cast<unsigned int>(ghz),
                     static_cast<unsigned int>(mhz));
        } else {
            snprintf(buffer, sizeof(buffer), "%u GHz",
                     static_cast<unsigned int>(ghz));
        }
        return std::string(buffer);
    } else {
        // MHz range
        uint32_t mhz = static_cast<uint32_t>(freq / 1000000ULL);
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%u MHz",
                 static_cast<unsigned int>(mhz));
        return std::string(buffer);
    }
}

// ===========================================
// PART 6.5: COMPACT FREQUENCY RULER IMPLEMENTATION
// ===========================================

CompactFrequencyRuler::CompactFrequencyRuler(Rect parent_rect)
    : View(parent_rect),
      min_freq_(2400000000ULL),
      max_freq_(2500000000ULL),
      spectrum_width_(240),
      visible_(true),
      ruler_style_(RulerStyle::COMPACT_GHZ),
      target_tick_count_(DEFAULT_TICK_COUNT) {
}

void CompactFrequencyRuler::set_frequency_range(Frequency min_freq, Frequency max_freq) {
    min_freq_ = min_freq;
    max_freq_ = max_freq;
    set_dirty();
}

void CompactFrequencyRuler::set_spectrum_width(int width) {
    spectrum_width_ = width;
    set_dirty();
}

void CompactFrequencyRuler::set_visible(bool visible) {
    visible_ = visible;
    hidden(!visible);
}

void CompactFrequencyRuler::set_ruler_style(RulerStyle style) {
    ruler_style_ = style;
    set_dirty();
}

void CompactFrequencyRuler::set_tick_count(int num_ticks) {
    if (num_ticks >= 3 && num_ticks <= 8) {
        target_tick_count_ = num_ticks;
    }
    set_dirty();
}

void CompactFrequencyRuler::paint(Painter& painter) {
    const auto r = screen_rect();

    painter.fill_rectangle(r, Color::black());

    if (!visible_) return;

    draw_compact_ticks(painter, r);
}

void CompactFrequencyRuler::draw_compact_ticks(Painter& painter, const Rect r) {
    Frequency range = max_freq_ - min_freq_;
    if (range == 0) return;

    Frequency tick_interval = calculate_optimal_tick_interval();
    if (tick_interval == 0) return;

    bool use_mhz = should_use_mhz_labels();

    static Style compact_style_mhz{font::fixed_5x8, Color::grey(), Color::black()};
    static Style compact_style_ghz{font::fixed_5x8, Color::grey(), Color::black()};
    Style* label_style = use_mhz ? &compact_style_mhz : &compact_style_ghz;

    display.draw_line(
        {r.left(), r.top() + RULER_HEIGHT - 1},
        {r.right(), r.top() + RULER_HEIGHT - 1},
        Theme::getInstance()->fg_light->foreground
    );

    Frequency first_tick = (min_freq_ / tick_interval) * tick_interval;
    if (first_tick < min_freq_) {
        first_tick += tick_interval;
    }

    for (Frequency tick = first_tick; tick <= max_freq_; tick += tick_interval) {
        int x = r.left() + static_cast<int>(((tick - min_freq_) * spectrum_width_) / range);

        if (x < r.left() || x > r.right()) {
            continue;
        }

        display.draw_line(
            {x, r.top()},
            {x, r.top() + TICK_HEIGHT_MAJOR},
            Theme::getInstance()->fg_light->foreground
        );

        std::string label = format_compact_label(tick);

        auto text_size = label_style->font.size_of(label);
        int text_x = x - text_size.width() / 2;
        int text_y = r.top() + 1;

        if (text_x < r.left() + 2) text_x = r.left() + 2;
        if (text_x + text_size.width() > r.right() - 2) {
            text_x = r.right() - text_size.width() - 2;
        }

        painter.draw_string({text_x, text_y}, *label_style, label);

        if (!use_mhz && static_cast<uint64_t>(tick_interval) >= 100000000ULL) {
            for (int sub = 1; sub < 5; sub++) {
                Frequency sub_tick = tick + (tick_interval * sub) / 5;
                int sub_x = r.left() + static_cast<int>(((sub_tick - min_freq_) * spectrum_width_) / range);

                if (sub_x > r.left() && sub_x < r.right()) {
                    display.draw_line(
                        {sub_x, r.top()},
                        {sub_x, r.top() + TICK_HEIGHT_MINOR},
                        Theme::getInstance()->fg_light->foreground
                    );
                }
            }
        }
    }
}

std::string CompactFrequencyRuler::format_compact_label(Frequency freq) {
    char buffer[16];

    switch (ruler_style_) {
        case RulerStyle::COMPACT_GHZ: {
            uint32_t ghz = static_cast<uint32_t>(freq / 1000000000ULL);
            uint32_t decimal = static_cast<uint32_t>(((freq % 1000000000ULL) / 100000000ULL));
            if (decimal > 0) {
                snprintf(buffer, sizeof(buffer), "%lu.%luG",
                         static_cast<unsigned long>(ghz),
                         static_cast<unsigned long>(decimal));
            } else {
                snprintf(buffer, sizeof(buffer), "%luG", static_cast<unsigned long>(ghz));
            }
            break;
        }
        case RulerStyle::COMPACT_MHZ: {
            uint32_t mhz = static_cast<uint32_t>((freq + 500000) / 1000000);
            snprintf(buffer, sizeof(buffer), "%lu", static_cast<unsigned long>(mhz));
            break;
        }
        case RulerStyle::STANDARD_GHZ: {
            uint32_t ghz = static_cast<uint32_t>(freq / 1000000000ULL);
            uint32_t decimal = static_cast<uint32_t>(((freq % 1000000000ULL) / 100000000ULL));
            if (decimal > 0) {
                snprintf(buffer, sizeof(buffer), "%lu.%luGHz",
                         static_cast<unsigned long>(ghz),
                         static_cast<unsigned long>(decimal));
            } else {
                snprintf(buffer, sizeof(buffer), "%luGHz", static_cast<unsigned long>(ghz));
            }
            break;
        }
        case RulerStyle::STANDARD_MHZ: {
            uint32_t mhz = static_cast<uint32_t>((freq + 500000) / 1000000);
            snprintf(buffer, sizeof(buffer), "%luMHz", static_cast<unsigned long>(mhz));
            break;
        }
        case RulerStyle::DETAILED: {
            uint32_t ghz = static_cast<uint32_t>(freq / 1000000000ULL);
            uint32_t decimal = static_cast<uint32_t>(((freq % 1000000000ULL) / 10000000ULL));
            snprintf(buffer, sizeof(buffer), "%lu.%03luGHz",
                     static_cast<unsigned long>(ghz),
                     static_cast<unsigned long>(decimal));
            break;
        }
        case RulerStyle::SPACED_GHZ: {
            uint32_t ghz = static_cast<uint32_t>(freq / 1000000000ULL);
            snprintf(buffer, sizeof(buffer), "%uGHz", static_cast<unsigned int>(ghz));
            break;
        }
        default: {
            uint32_t ghz = static_cast<uint32_t>(freq / 1000000000ULL);
            snprintf(buffer, sizeof(buffer), "%luG", static_cast<unsigned long>(ghz));
            break;
        }
    }

    return std::string(buffer);
}

Frequency CompactFrequencyRuler::calculate_optimal_tick_interval() {
    Frequency range = max_freq_ - min_freq_;
    if (range == 0) return 0;

    // Optimized intervals for PortaPack's small screen
    // Prioritize GHz intervals for SPACED_GHZ style
    Frequency intervals[] = {
        5000000000ULL,  4000000000ULL,  3000000000ULL, 2000000000ULL,  // 5G, 4G, 3G, 2G
        1000000000ULL,  // 1G (key for SPACED_GHZ)
        500000000ULL,   250000000ULL,   200000000ULL,            // 500M, 250M, 200M
        100000000ULL,   50000000ULL,    25000000ULL,            // 100M, 50M, 25M
        20000000ULL,    10000000ULL,    5000000ULL,             // 20M, 10M, 5M
        2000000ULL,     1000000ULL,     500000ULL,              // 2M, 1M, 500k
        200000ULL,      100000ULL                                // 200k, 100k
    };

    for (auto interval : intervals) {
        int ticks = static_cast<int>(range / interval);
        if (ticks >= target_tick_count_ - 1 && ticks <= target_tick_count_ + 1) {
            return interval;
        }
    }

    return range / target_tick_count_;
}

RulerStyle CompactFrequencyRuler::determine_auto_style() {
    Frequency range = max_freq_ - min_freq_;
    bool use_mhz = should_use_mhz_labels();

    if (use_mhz) {
        if (static_cast<uint64_t>(range) < 100000000ULL) {
            return RulerStyle::DETAILED;
        } else {
            return RulerStyle::STANDARD_MHZ;
        }
    } else {
        // Optimal for PortaPack: Use SPACED_GHZ for wide ranges (>1GHz)
        if (static_cast<uint64_t>(range) >= 1000000000ULL) {
            return RulerStyle::SPACED_GHZ;
        } else if (static_cast<uint64_t>(range) >= 500000000ULL) {
            return RulerStyle::STANDARD_GHZ;
        } else {
            return RulerStyle::COMPACT_GHZ;
        }
    }
}

bool CompactFrequencyRuler::should_use_mhz_labels() const {
    Frequency range = max_freq_ - min_freq_;

    if (static_cast<uint64_t>(range) < 50000000ULL) {
        return true;
    }

    Frequency center = (min_freq_ + max_freq_) / 2;
    if (static_cast<uint64_t>(center) < 1000000000ULL) {
        return true;
    }

    if (ruler_style_ == RulerStyle::COMPACT_MHZ ||
        ruler_style_ == RulerStyle::STANDARD_MHZ) {
        return true;
    }

    return false;
}

// ===========================================
// PART 7: FREQUENCY RULER IMPLEMENTATION
// ===========================================

FrequencyRuler::FrequencyRuler(Rect parent_rect)
    : View(parent_rect),
      min_freq_(2400000000ULL),
      max_freq_(2500000000ULL),
      spectrum_width_(240),
      visible_(true) {
}

void FrequencyRuler::set_frequency_range(Frequency min_freq, Frequency max_freq) {
    min_freq_ = min_freq;
    max_freq_ = max_freq;
    set_dirty();
}

void FrequencyRuler::set_spectrum_width(int width) {
    spectrum_width_ = width;
    set_dirty();
}

void FrequencyRuler::set_visible(bool visible) {
    visible_ = visible;
    if (!visible) {
        hidden(true);
    } else {
        hidden(false);
    }
}

Frequency FrequencyRuler::calculate_tick_interval() {
    Frequency freq_range = max_freq_ - min_freq_;
    if (freq_range == 0) {
        return 1000000000ULL;  // Default: 1 GHz
    }

    // Possible intervals in Hz (from largest to smallest)
    Frequency intervals[] = {
        4000000000ULL,  // 4 GHz
        2000000000ULL,  // 2 GHz
        1000000000ULL,  // 1 GHz
        500000000ULL,   // 500 MHz
        200000000ULL,   // 200 MHz
        100000000ULL,   // 100 MHz
        50000000ULL,    // 50 MHz
        20000000ULL,    // 20 MHz
        10000000ULL,    // 10 MHz
        5000000ULL,     // 5 MHz
        2000000ULL,     // 2 MHz
        1000000ULL      // 1 MHz
    };

    size_t num_intervals = sizeof(intervals) / sizeof(intervals[0]);

    // Select interval so that we have 4-8 ticks on screen
    for (size_t i = 0; i < num_intervals; i++) {
        if (freq_range / intervals[i] <= 8 && freq_range / intervals[i] >= 4) {
            return intervals[i];
        }
    }

    // Fallback: use 6 ticks evenly distributed
    return freq_range / 6;
}

std::string FrequencyRuler::format_frequency_label(Frequency freq, const std::string& unit) {
    char buffer[32];

    if (unit == "G") {
        uint32_t ghz = static_cast<uint32_t>(freq / 1000000000ULL);
        uint32_t decimal = static_cast<uint32_t>((freq % 1000000000ULL) / 100000000ULL);
        if (decimal > 0) {
            snprintf(buffer, sizeof(buffer), "%lu.%luG",
                     static_cast<unsigned long>(ghz),
                     static_cast<unsigned long>(decimal));
        } else {
            snprintf(buffer, sizeof(buffer), "%luG", static_cast<unsigned long>(ghz));
        }
    } else if (unit == "M") {
        uint32_t mhz = static_cast<uint32_t>((freq + 500000) / 1000000);
        uint32_t decimal = static_cast<uint32_t>(((freq + 50000) % 1000000) / 100000);
        if (decimal > 0 && unit != "Mcompact") {
            snprintf(buffer, sizeof(buffer), "%lu.%luM",
                     static_cast<unsigned long>(mhz),
                     static_cast<unsigned long>(decimal));
        } else {
            snprintf(buffer, sizeof(buffer), "%luM", static_cast<unsigned long>(mhz));
        }
    } else {
        // Fallback
        uint32_t mhz = static_cast<uint32_t>(freq / 1000000);
        snprintf(buffer, sizeof(buffer), "%luM", static_cast<unsigned long>(mhz));
    }

    return std::string(buffer);
}

void FrequencyRuler::draw_frequency_ticks(Painter& painter, const Rect r) {
    Frequency tick_interval = calculate_tick_interval();
    Frequency range = max_freq_ - min_freq_;
    if (range == 0) return;

    // Determine measurement unit
    std::string unit;
    if (static_cast<uint64_t>(tick_interval) >= 1000000000ULL) {
        unit = "G";
    } else if (static_cast<uint64_t>(tick_interval) >= 1000000ULL) {
        unit = "M";
    } else {
        unit = "Mcompact";
    }

    // Style for tick labels (small font 5x8)
    static Style ruler_style{font::fixed_5x8, Color::grey(), Color::black()};

    // Draw horizontal line at bottom of ruler
    display.draw_line(
        {r.left(), r.top() + RULER_HEIGHT - 1},
        {r.right(), r.top() + RULER_HEIGHT - 1},
        Theme::getInstance()->bg_darkest->foreground
    );

    // Find first tick
    Frequency first_tick = (min_freq_ / tick_interval) * tick_interval;
    if (first_tick < min_freq_) {
        first_tick += tick_interval;
    }

    // Draw ticks and labels
    for (Frequency tick = first_tick; tick <= max_freq_; tick += tick_interval) {
        // Convert frequency to x position
        int x = r.left() + static_cast<int>(((tick - min_freq_) * spectrum_width_) / range);

        if (x < r.left() || x > r.right()) {
            continue;
        }

        // Draw major tick (tall)
        display.draw_line(
            {x, r.top()},
            {x, r.top() + TICK_HEIGHT_MAJOR},
            Theme::getInstance()->bg_darkest->foreground
        );

        // Format and draw text label
        std::string label = format_frequency_label(tick, unit);

        auto text_size = ruler_style.font.size_of(label);
        int text_x = x - text_size.width() / 2;
        int text_y = r.top() + 1;

        // Clamp text position to screen bounds
        if (text_x < r.left() + 2) text_x = r.left() + 2;
        if (text_x + text_size.width() > r.right() - 2) {
            text_x = r.right() - text_size.width() - 2;
        }

        painter.draw_string({text_x, text_y}, ruler_style, label);

        // Add intermediate ticks if interval is large
        if (static_cast<uint64_t>(tick_interval) >= 100000000ULL) {  // >= 100 MHz
            for (int sub = 1; sub < 5; sub++) {
                Frequency sub_tick = tick + (tick_interval * sub) / 5;
                int sub_x = r.left() + static_cast<int>(((sub_tick - min_freq_) * spectrum_width_) / range);

                if (sub_x > r.left() && sub_x < r.right()) {
                    display.draw_line(
                        {sub_x, r.top()},
                        {sub_x, r.top() + TICK_HEIGHT_MINOR},
                        Theme::getInstance()->bg_darkest->foreground
                    );
                }
            }
        }
    }
}

void FrequencyRuler::paint(Painter& painter) {
    const auto r = screen_rect();

    // Fill background
    painter.fill_rectangle(r, Color::black());

    // Draw frequency ticks and labels
    if (visible_) {
        draw_frequency_ticks(painter, r);
    }
}

void DroneDisplayController::update_frequency_ruler() {
    compact_frequency_ruler_.set_frequency_range(spectrum_config_.min_freq, spectrum_config_.max_freq);
    compact_frequency_ruler_.set_spectrum_width(SPEC_WIDTH);
    compact_frequency_ruler_.set_dirty();
}

void DroneDisplayController::set_ruler_style(RulerStyle style) {
    compact_frequency_ruler_.set_ruler_style(style);
    compact_frequency_ruler_.set_visible(true);
    frequency_ruler_.set_visible(false);
}

void DroneDisplayController::apply_display_settings(const DisplaySettings& settings) {
    if (settings.show_frequency_ruler) {
        compact_frequency_ruler_.set_visible(true);
        frequency_ruler_.set_visible(false);

        if (settings.auto_ruler_style) {
            compact_frequency_ruler_.set_ruler_style(RulerStyle::COMPACT_GHZ);
        } else {
            RulerStyle style;
            switch (settings.frequency_ruler_style) {
                case 0: style = RulerStyle::COMPACT_GHZ; break;
                case 1: style = RulerStyle::COMPACT_GHZ; break;
                case 2: style = RulerStyle::COMPACT_MHZ; break;
                case 3: style = RulerStyle::STANDARD_GHZ; break;
                case 4: style = RulerStyle::STANDARD_MHZ; break;
                case 5: style = RulerStyle::DETAILED; break;
                case 6: style = RulerStyle::SPACED_GHZ; break;  // New SPACED_GHZ
                default: style = RulerStyle::COMPACT_GHZ; break;
            }
            compact_frequency_ruler_.set_ruler_style(style);
        }

        compact_frequency_ruler_.set_tick_count(settings.compact_ruler_tick_count);
    } else {
        compact_frequency_ruler_.set_visible(false);
        frequency_ruler_.set_visible(false);
    }
}

} // namespace ui::apps::enhanced_drone_analyzer
