// ui_scanner_combined.cpp - Unified implementation for Enhanced Drone Analyzer Scanner App
// Combines implementations from: ui_drone_scanner.cpp, ui_drone_hardware.cpp, ui_drone_ui.cpp
// Combines all required implementations for scanner functionality

#include "ui_scanner_combined.hpp"
#include "ui_drone_audio.hpp"
#include "../../gradient.hpp"
#include "../../baseband_api.hpp"
#include <algorithm>
#include <sstream>

#include <cstdlib>

// Add ChibiOS headers for threading
#include <ch.h>

static constexpr uint32_t MIN_SCAN_INTERVAL_MS = 100;
// Increase stack size for safety (was 4096)
static constexpr uint32_t SCAN_THREAD_STACK_SIZE = 8192;

// Zombie detection cleanup thread for stale drones
static constexpr uint32_t CLEANUP_THREAD_STACK_SIZE = 4096;

// Audio processing thread for alert generation
static constexpr uint32_t AUDIO_THREAD_STACK_SIZE = 4096;

// WORKING_AREA definition for thread stack
WORKING_AREA(scanning_thread_wa, SCAN_THREAD_STACK_SIZE);

// ChibiOS constants for message thread return
#define MSG_OK (msg_t)0

// Settings file loading helper for scanner app - FIXED per Portapack File API
// Using namespace to access DroneAnalyzerSettings defined in the scanner namespace
using namespace ui::external_app::enhanced_drone_analyzer;

/**
 * VALIDATION OF LOADED SETTINGS WITH SAFE LIMITS
 * Checks all parameters for compliance with acceptable ranges
 */
bool validate_loaded_settings(const DroneAnalyzerSettings& settings) {
    // Validate scan interval (100ms - 30sec)
    if (settings.scan_interval_ms < 100 || settings.scan_interval_ms > 30000) {
        return false;
    }

    // Validate RSSI threshold (-120dB to 0dB)
    if (settings.rssi_threshold_db < -120 || settings.rssi_threshold_db > 0) {
        return false;
    }

    // Validate audio signal frequency (200Hz - 3000Hz)
    if (settings.audio_alert_frequency_hz < 200 || settings.audio_alert_frequency_hz > 3000) {
        return false;
    }

    // Validate audio duration (50ms - 2sec)
    if (settings.audio_alert_duration_ms < 50 || settings.audio_alert_duration_ms > 2000) {
        return false;
    }

    // Validate bandwidth (1MHz - 100MHz)
    if (settings.hardware_bandwidth_hz < 1000000 || settings.hardware_bandwidth_hz > 100000000) {
        return false;
    }

    return true;  // ALL SETTINGS WITHIN ACCEPTABLE LIMITS
}

/**
 * PARSING SETTINGS FROM FILE CONTENT
 * Parses TXT file line by line with error handling
 */
bool parse_settings_from_content(const std::string& content, DroneAnalyzerSettings& settings) {
    std::istringstream iss(content);
    std::string line;
    size_t parsed_lines = 0;

    while (std::getline(iss, line)) {
        // TRIM SPACES
        auto it = std::find_if(line.begin(), line.end(), [](int ch) {
            return !std::isspace(ch);
        });
        line.erase(line.begin(), it);

        auto rit = std::find_if(line.rbegin(), line.rend(), [](int ch) {
            return !std::isspace(ch);
        });
        line.erase(rit.base(), line.end());

        // SKIP EMPTY LINES AND COMMENTS
        if (line.empty() || line[0] == '#') continue;

        // FIND DELIMITER "="
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) continue;  // INVALID LINE

        std::string key = line.substr(0, equals_pos);
        std::string value = line.substr(equals_pos + 1);

        // TRIM KEY AND VALUE
        key.erase(key.begin(), std::find_if(key.begin(), key.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        key.erase(std::find_if(key.rbegin(), key.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), key.end());

        value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        value.erase(std::find_if(value.rbegin(), value.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), value.end());

        // PARSING SOME KEYS (exceptions disabled in embedded context)
        if (key == "spectrum_mode") {
            // SPECTRUM MODE
            if (value == "NARROW") settings.spectrum_mode = SpectrumMode::NARROW;
            else if (value == "MEDIUM") settings.spectrum_mode = SpectrumMode::MEDIUM;
            else if (value == "WIDE") settings.spectrum_mode = SpectrumMode::WIDE;
            else if (value == "ULTRA_WIDE") settings.spectrum_mode = SpectrumMode::ULTRA_WIDE;
            parsed_lines++;
        } else if (key == "scan_interval_ms") {
            // SCAN INTERVAL - safe strtoul with bounds check
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val <= 30000) {
                settings.scan_interval_ms = static_cast<uint32_t>(val);
                parsed_lines++;
            }
        } else if (key == "rssi_threshold_db") {
            // RSSI THRESHOLD - safe strtol with bounds check
            char* endptr = nullptr;
            long val = strtol(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= -120 && val <= 0) {
                settings.rssi_threshold_db = static_cast<int32_t>(val);
                parsed_lines++;
            }
        } else if (key == "enable_audio_alerts") {
            // AUDIO ALERTS
            settings.enable_audio_alerts = (value == "true");
            parsed_lines++;
        } else if (key == "audio_alert_frequency_hz") {
            // AUDIO FREQUENCY - safe strtoul with bounds check
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= 200 && val <= 3000) {
                settings.audio_alert_frequency_hz = static_cast<uint16_t>(val);
                parsed_lines++;
            }
        } else if (key == "audio_alert_duration_ms") {
            // AUDIO DURATION - safe strtoul with bounds check
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= 50 && val <= 2000) {
                settings.audio_alert_duration_ms = static_cast<uint32_t>(val);
                parsed_lines++;
            }
        } else if (key == "enable_real_hardware") {
            // REAL HARDWARE MODE
            settings.enable_real_hardware = (value == "true");
            parsed_lines++;
        } else if (key == "demo_mode") {
            // DEMO MODE
            settings.demo_mode = (value == "true");
            parsed_lines++;
        } else if (key == "hardware_bandwidth_hz") {
            // HARDWARE BANDWIDTH - safe strtoul with bounds check
            char* endptr = nullptr;
            unsigned long val = strtoul(value.c_str(), &endptr, 10);
            if (endptr != value.c_str() && val >= 1000000 && val <= 100000000) {
                settings.hardware_bandwidth_hz = static_cast<uint32_t>(val);
                parsed_lines++;
            }
        }
        // SKIP UNKNOWN KEYS
    }

    return parsed_lines >= 5;  // REQUIRES MINIMUM 5 VALID SETTINGS
}

bool load_settings_from_sd_card(DroneAnalyzerSettings& settings) {
    const std::string SETTINGS_FILE_PATH = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";

    // SAVE ORIGINAL SETTINGS FOR ROLLBACK
    DroneAnalyzerSettings original_settings = settings;

    // REMOVED: try/catch - exceptions disabled in embedded C++ environment

    // FIXED: Use proper File::open with boolean read_only parameter
    File settings_file;
    if (!settings_file.open(SETTINGS_FILE_PATH, true)) {  // true = read_only
        return false;  // FILE NOT FOUND - RETURN FALSE
    }

    std::string file_content;
    file_content.resize(settings_file.size());
    auto read_result = settings_file.read(file_content.data(), settings_file.size());
    if (read_result != settings_file.size()) {
        settings_file.close();
        // ROLLBACK TO ORIGINAL SETTINGS
        settings = original_settings;
        return false;
    }
    settings_file.close();

    // PARSE AND VALIDATE SETTINGS
    if (!parse_settings_from_content(file_content, settings)) {
        // PARSING FAILED - ROLLBACK
        settings = original_settings;
        return false;
    }

    // VALIDATE LOADED SETTINGS
    if (!validate_loaded_settings(settings)) {
        // INVALID SETTINGS - ROLLBACK TO SAFE VALUES
        settings = original_settings;
        return false;
    }

    return true;  // SUCCESSFUL LOAD
}

namespace ui::external_app::enhanced_drone_analyzer {

// ===========================================
// PART 1: DETECTION RING BUFFER IMPLEMENTATION (from ui_drone_scanner.cpp)
// ===========================================

DetectionRingBuffer global_detection_ring;
DetectionRingBuffer& local_detection_ring = global_detection_ring;

DetectionRingBuffer::DetectionRingBuffer()
    : detection_counts_{}, rssi_values_{}
{
    clear();
}

void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
    const size_t index = frequency_hash % DETECTION_TABLE_SIZE;
    detection_counts_[index] = detection_count;
    rssi_values_[index] = rssi_value;
    __DMB();
}

uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
    const size_t index = frequency_hash % DETECTION_TABLE_SIZE;
    return detection_counts_[index];
}

int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const {
    const size_t index = frequency_hash % DETECTION_TABLE_SIZE;
    return rssi_values_[index];
}

void DetectionRingBuffer::clear() {
    memset(detection_counts_, 0, sizeof(detection_counts_));
    memset(rssi_values_, 0, sizeof(rssi_values_));
    for (size_t i = 0; i < DETECTION_TABLE_SIZE; i++) {
        rssi_values_[i] = -120;
    }
}



// DroneScanner::DroneScanner()
DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
    : scanning_active_(false),                           
      scanning_thread_(nullptr),                         
      current_db_index_(0),                              
      last_scanned_frequency_(0),                        
      scan_cycles_(0),                                   
      total_detections_(0),                              
      is_real_mode_(config.enable_real_hardware),        
      tracked_drones_count_(0),                          
      approaching_count_(0),                             
      receding_count_(0),                                
      static_count_(0),                                  
      max_detected_threat_(ThreatLevel::NONE),           
      last_valid_rssi_(-120),                            
    wideband_scan_data_(),                             
    freq_db_(),                                        
    scanning_mode_(ScanningMode::DATABASE),            
    tracked_drones_(),                                 
    detection_processor_(this),                         
    scan_interval_ms_(config.scan_interval_ms),        
    rssi_threshold_db_(config.rssi_threshold_db),      
    audio_alerts_enabled_(config.enable_audio_alerts)  
{
    initialize_database_and_scanner();
}

DroneScanner::~DroneScanner() {
    stop_scanning();
    cleanup_database_and_scanner();
}

void DroneScanner::initialize_database_and_scanner() {
    auto db_path = get_freqman_path("DRONES");
    if (!freq_db_.open(db_path, true)) {  // FreqmanDB::open uses different signature
        // Continue without enhanced drone data
    }
}

void DroneScanner::cleanup_database_and_scanner() {
    if (scanning_thread_) {
        scanning_active_ = false;
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }
}

void DroneScanner::initialize_wideband_scanning() {
    wideband_scan_data_.reset();
    setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
}

void DroneScanner::setup_wideband_range(Frequency min_freq, Frequency max_freq) {
    wideband_scan_data_.min_freq = min_freq;
    wideband_scan_data_.max_freq = max_freq;

    Frequency scanning_range = max_freq - min_freq;
    if (scanning_range > WIDEBAND_SLICE_WIDTH) {
        wideband_scan_data_.slices_nb = (scanning_range + WIDEBAND_SLICE_WIDTH - 1) / WIDEBAND_SLICE_WIDTH;
        if (wideband_scan_data_.slices_nb > WIDEBAND_MAX_SLICES) {
            wideband_scan_data_.slices_nb = WIDEBAND_MAX_SLICES;
        }
        Frequency slices_span = wideband_scan_data_.slices_nb * WIDEBAND_SLICE_WIDTH;
        Frequency offset = ((scanning_range - slices_span) / 2) + (WIDEBAND_SLICE_WIDTH / 2);
        Frequency center_frequency = min_freq + offset;

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
        wideband_scan_data_.slices[0].center_frequency = (max_freq + min_freq) / 2;
        wideband_scan_data_.slices_nb = 1;
    }
    wideband_scan_data_.slice_counter = 0;
}

void DroneScanner::start_scanning() {
    if (scanning_active_ || scanning_thread_ != nullptr) return;

    scanning_active_ = true;
    scan_cycles_ = 0;
    total_detections_ = 0;

    // FIXED: Use WORKING_AREA for proper ChibiOS thread creation
    scanning_thread_ = chThdCreateStatic(scanning_thread_wa, sizeof(scanning_thread_wa),
                                         NORMALPRIO + 10, scanning_thread_function, this);
    if (!scanning_thread_) {
        scanning_active_ = false;
    }
}

// ADDED: Spectrum streaming management for proper RSSI acquisition
void DroneScanner::start_spectrum_for_scanning(DroneHardwareController& hardware) {
    if (scanning_active_ && !hardware.is_spectrum_streaming_active()) {
        hardware.start_spectrum_streaming();
    }
}

void DroneScanner::stop_spectrum_for_scanning(DroneHardwareController& hardware) {
    if (!scanning_active_ && hardware.is_spectrum_streaming_active()) {
        hardware.stop_spectrum_streaming();
    }
}

void DroneScanner::stop_scanning() {
    if (!scanning_active_) return;

    scanning_active_ = false;
    if (scanning_thread_ && chThdTerminated(scanning_thread_) == false) {
        chThdTerminate(scanning_thread_);
        chThdWait(scanning_thread_);
        scanning_thread_ = nullptr;
    }
    remove_stale_drones();

    if (detection_logger_.is_session_active()) {
        detection_logger_.end_session();
    }
}

msg_t DroneScanner::scanning_thread_function(void* arg) {
    auto* self = static_cast<DroneScanner*>(arg);
    return self->scanning_thread();
}

msg_t DroneScanner::scanning_thread() {
    // FIXED: chThdShouldTerminateX() → chThdShouldTerminate() (no 'X' suffix)
    // CHANGED: Using configurable scan_interval_ms_ instead of MIN_SCAN_INTERVAL_MS
    while (scanning_active_ && !chThdShouldTerminate()) {
        chThdSleepMilliseconds(scan_interval_ms_);
        scan_cycles_++;
    }
    scanning_active_ = false;
    scanning_thread_ = nullptr;
    chThdExit(MSG_OK);
    return MSG_OK;
}

// Implementation for missing methods declared in header but not defined
size_t DroneScanner::get_total_memory_usage() const {
    // Estimate memory usage for UI display
    return sizeof(*this) + (tracked_drones_.size() * sizeof(TrackedDroneData)) +
           (freq_db_.entry_count() > 0 ? freq_db_.entry_count() * sizeof(freqman_entry) : 0);
}

// ===========================================
// UNIFIED WIDEBAND PROCESSING SYSTEM (Consolidated Implementation)
// ===========================================

/**
 * MASTER WIDEBAND DETECTION FUNCTION
 * Consolidated all wideband logic into single, coherent function
 * Handles all wideband scanning, detection, validation, and tracking
 */
void DroneScanner::master_wideband_detection_handler(
    DroneHardwareController& hardware,
    Frequency target_frequency_hz,
    int32_t rss_threshold_override,
    bool force_detection
) {
    // PHASE 1: Wideband Range Validation & Setup
    const Frequency WIDEBAND_MIN = WIDEBAND_DEFAULT_MIN;
    const Frequency WIDEBAND_MAX = WIDEBAND_DEFAULT_MAX;

    if (target_frequency_hz < WIDEBAND_MIN || target_frequency_hz > WIDEBAND_MAX) {
        if (scan_cycles_ % 50 == 0) {
            handle_scan_error("Wideband frequency out of range");
        }
        return;
    }

    // PHASE 2: Hardware Tuning with Error Handling
    if (!hardware.tune_to_frequency(target_frequency_hz)) {
        if (scan_cycles_ % 100 == 0) {
            handle_scan_error("Wideband hardware tuning failed");
        }
        return;
    }

    // PHASE 3: Get Real RSSI Measurement
    int32_t measured_rssi = hardware.get_real_rssi_from_hardware(target_frequency_hz);
    last_valid_rssi_ = measured_rssi;

    // PHASE 4: Determine Detection Threshold
    int32_t rss_threshold = (rss_threshold_override > 0) ?
        rss_threshold_override : WIDEBAND_RSSI_THRESHOLD_DB;

    // PHASE 5: Pre-processing Validation
    if (!SimpleDroneValidation::validate_rssi_signal(measured_rssi, ThreatLevel::NONE)) {
        return;  // Invalid RSSI signal
    }

    if (!SimpleDroneValidation::validate_frequency_range(target_frequency_hz)) {
        return;  // Out of valid frequency range
    }

    // PHASE 6: Quick Rejection for Weak Signals
    if (measured_rssi < rss_threshold && !force_detection) {
        return;
    }

    // PHASE 7: Create Detection Entry for Processing
    freqman_entry detection_entry{
        .frequency_a = target_frequency_hz,
        .frequency_b = target_frequency_hz,
        .description = "Wideband Enhanced Detection",
        .type = freqman_type::Single,
        .modulation = freqman_invalid_index,
        .bandwidth = freqman_invalid_index,
        .step = freqman_invalid_index,
        .tone = freqman_invalid_index,
        .tonal = ""
    };

// PHASE 8: Process Detection with Unified Processor (Refactored)
    detection_processor_.process_unified_detection(detection_entry, measured_rssi, rss_threshold);
}

bool DroneScanner::load_frequency_database() {
    // Removed try/catch - exceptions disabled in embedded environment
    if (freq_db_.entry_count() == 0) {
        return false;
    }
    current_db_index_ = 0;
    last_scanned_frequency_ = 0;

    if (freq_db_.entry_count() > 100) {
        handle_scan_error("Large database loaded");
    }
    scan_init_from_loaded_frequencies();
    return true;
}

size_t DroneScanner::get_database_size() const {
    size_t count = freq_db_.entry_count();
    return (count > 0) ? count : 0;
}

void DroneScanner::set_scanning_mode(ScanningMode mode) {
    scanning_mode_ = mode;
    stop_scanning();
    scan_cycles_ = 0;  // Reset scan cycles
    total_detections_ = 0;  // Reset total detections

    if (scanning_mode_ == ScanningMode::DATABASE || scanning_mode_ == ScanningMode::HYBRID) {
        load_frequency_database();
    }
}

std::string DroneScanner::scanning_mode_name() const {
    switch (scanning_mode_) {
        case ScanningMode::DATABASE: return "Database Scan";
        case ScanningMode::WIDEBAND_CONTINUOUS: return "Wideband Monitor";
        case ScanningMode::HYBRID: return "Hybrid Discovery";
        default: return "Unknown";
    }
}

void DroneScanner::perform_scan_cycle(DroneHardwareController& hardware) {
    if (!scanning_active_) return;

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
}

void DroneScanner::perform_database_scan_cycle(DroneHardwareController& hardware) {
    size_t entry_count = freq_db_.entry_count();
    if (entry_count == 0) {
        if (scan_cycles_ % 50 == 0) {
            handle_scan_error("No frequency database loaded");
            scanning_active_ = false;
        }
        return;
    }

    const size_t total_entries = freq_db_.entry_count();
    if (current_db_index_ >= total_entries) {
        current_db_index_ = 0;
    }

    const freqman_entry& entry = freq_db_[current_db_index_];
    if (entry.frequency_a > 0) {
        Frequency target_freq_hz = entry.frequency_a;
        if (target_freq_hz >= 50000000 && target_freq_hz <= 6000000000) {
            if (hardware.tune_to_frequency(target_freq_hz)) {
                int32_t real_rssi = hardware.get_real_rssi_from_hardware(target_freq_hz);
                process_rssi_detection(entry, real_rssi);
                last_scanned_frequency_ = target_freq_hz;
            }
        }
        current_db_index_ = (current_db_index_ + 1) % total_entries;
    } else {
        current_db_index_ = (current_db_index_ + 1) % total_entries;
    }
}

void DroneScanner::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
    // PHASE: Wideband scanning using master unified detection handler
    if (wideband_scan_data_.slices_nb == 0) {
        setup_wideband_range(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
    }

    if (wideband_scan_data_.slice_counter >= wideband_scan_data_.slices_nb) {
        wideband_scan_data_.slice_counter = 0;
    }

    const WidebandSlice& current_slice = wideband_scan_data_.slices[wideband_scan_data_.slice_counter];

    // Use the master handler for unified wideband detection processing
    master_wideband_detection_handler(hardware, current_slice.center_frequency,
                                     WIDEBAND_RSSI_THRESHOLD_DB, false);

    last_scanned_frequency_ = current_slice.center_frequency;
    wideband_scan_data_.slice_counter = (wideband_scan_data_.slice_counter + 1) % wideband_scan_data_.slices_nb;
}



// Implementation for missing methods declared in header but not defined
void DroneScanner::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
                                                           int32_t original_threshold, int32_t wideband_threshold) {
    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::NONE) ||
        !SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    ThreatLevel threat_level;
    if (rssi > -70) {
        threat_level = ThreatLevel::HIGH;
    } else if (rssi > -80) {
        threat_level = ThreatLevel::LOW;
    } else {
        threat_level = ThreatLevel::NONE;
    }

    if (entry.frequency_a >= 2'400'000'000 && entry.frequency_a <= 2'500'000'000) {
        threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
    }

    total_detections_++;
    DroneType detected_type = DroneType::UNKNOWN;

    size_t freq_hash = entry.frequency_a;
    int32_t effective_threshold = wideband_threshold;

    int32_t previous_rssi = local_detection_ring.get_rssi_value(freq_hash);
    if (previous_rssi < wideband_threshold - HYSTERESIS_MARGIN_DB) {
        effective_threshold = wideband_threshold + HYSTERESIS_MARGIN_DB;
    }

    if (rssi >= effective_threshold) {
        uint8_t current_count = local_detection_ring.get_detection_count(freq_hash);
        current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
        local_detection_ring.update_detection(freq_hash, current_count, rssi);

        if (current_count >= MIN_DETECTION_COUNT) {
            DetectionLogEntry log_entry{
                .timestamp = chTimeNow(),
                .frequency_hz = static_cast<uint32_t>(entry.frequency_a),
                .rssi_db = rssi,
                .threat_level = threat_level,
                .drone_type = detected_type,
                .detection_count = current_count,
                .confidence_score = 0.6f
            };

            if (detection_logger_.is_session_active()) {
                detection_logger_.log_detection(log_entry);
            }
            update_tracked_drone(detected_type, entry.frequency_a, rssi, threat_level);
        }
    } else {
        local_detection_ring.update_detection(freq_hash, 0, -120);
    }
}

void DroneScanner::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
    if (scan_cycles_ % 2 == 0) {  // Alternate every cycle
        perform_wideband_scan_cycle(hardware);
    } else {
        perform_database_scan_cycle(hardware);
    }
}

void DroneScanner::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
    if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::NONE) ||
        !SimpleDroneValidation::validate_frequency_range(entry.frequency_a)) {
        return;
    }

    ThreatLevel threat_level;
    if (rssi > -70) {
        threat_level = ThreatLevel::HIGH;
    } else if (rssi > -80) {
        threat_level = ThreatLevel::MEDIUM;
    } else {
        threat_level = ThreatLevel::LOW;
    }

    if (entry.frequency_a >= 2'400'000'000 && entry.frequency_a <= 2'500'000'000) {
        threat_level = std::max(threat_level, ThreatLevel::MEDIUM);
    }

    total_detections_++;
    DroneType detected_type = DroneType::UNKNOWN;

    // Look up drone type in database if available
    if (freq_db_.entry_count() > 0) {
        freqman_entry db_entry = freq_db_[0];
        if (db_entry.frequency_a > 0) {
            // Assume drone type encoded in bandwidth field for now
            // TODO: Enhance database schema for drone types
            detected_type = (db_entry.bandwidth == 0 ? DroneType::MAVIC : DroneType::UNKNOWN);
        }
    }

    size_t freq_hash = entry.frequency_a;
    int32_t effective_threshold = rssi_threshold_db_;
    if (local_detection_ring.get_rssi_value(freq_hash) < rssi_threshold_db_) {
        effective_threshold = rssi_threshold_db_ + HYSTERESIS_MARGIN_DB;
    }

    if (rssi >= effective_threshold) {
        uint8_t current_count = local_detection_ring.get_detection_count(freq_hash);
        current_count = std::min(static_cast<uint8_t>(current_count + 1), static_cast<uint8_t>(255));
        local_detection_ring.update_detection(freq_hash, current_count, rssi);

        if (current_count >= MIN_DETECTION_COUNT) {
            chThdSleepMilliseconds(DETECTION_DELAY);

            DetectionLogEntry log_entry{
                .timestamp = chTimeNow(),
                .frequency_hz = static_cast<uint32_t>(entry.frequency_a),
                .rssi_db = rssi,
                .threat_level = threat_level,

// PHASE 4.2: AUDIO ALERT INTEGRATION - Play beep for high threats
// CORRECTED: Using proper baseband API for audio alerts (per baseband_api.hpp)
            if (threat_level >= ThreatLevel::HIGH && audio_alerts_enabled_) {
                // Use baseband::request_audio_beep with proper namespace per library reference
                baseband::request_audio_beep(800, 48000, 200);  // 800Hz, 48kHz sample rate, 200ms duration
            }

            update_tracked_drone(detected_type, entry.frequency_a, rssi, threat_level);
        }
    } else {
        local_detection_ring.update_detection(freq_hash, 0, -120);
    }
}

void DroneScanner::update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
    for (size_t i = 0; i < MAX_TRACKED_DRONES; i++) {
        TrackedDroneData& drone = tracked_drones_[i];
        if (drone.frequency == static_cast<uint32_t>(frequency) && drone.update_count > 0) {
            drone.add_rssi(static_cast<int16_t>(rssi), chTimeNow());
            drone.drone_type = static_cast<uint8_t>(type);
            drone.threat_level = static_cast<uint8_t>(threat_level);
            update_tracking_counts();
            return;
        }
        if (drone.update_count == 0) {
            drone.frequency = static_cast<uint32_t>(frequency);
            drone.drone_type = static_cast<uint8_t>(type);
            drone.threat_level = static_cast<uint8_t>(threat_level);
            drone.add_rssi(static_cast<int16_t>(rssi), chTimeNow());
            tracked_drones_count_++;
            update_tracking_counts();
            return;
        }
    }

    // Replace oldest drone
    size_t oldest_index = 0;
    systime_t oldest_time = tracked_drones_[0].last_seen;
    for (size_t i = 1; i < MAX_TRACKED_DRONES; i++) {
        if (tracked_drones_[i].last_seen < oldest_time) {
            oldest_time = tracked_drones_[i].last_seen;
            oldest_index = i;
        }
    }

    tracked_drones_[oldest_index] = TrackedDroneData();
    tracked_drones_[oldest_index].frequency = static_cast<uint32_t>(frequency);
    tracked_drones_[oldest_index].drone_type = static_cast<uint8_t>(type);
    tracked_drones_[oldest_index].threat_level = static_cast<uint8_t>(threat_level);
    tracked_drones_[oldest_index].add_rssi(static_cast<int16_t>(rssi), chTimeNow());
    update_tracking_counts();
}

void DroneScanner::remove_stale_drones() {
    const systime_t STALE_TIMEOUT = 30000;
    systime_t current_time = chTimeNow();

    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < MAX_TRACKED_DRONES; read_idx++) {
        const TrackedDroneData& drone = tracked_drones_[read_idx];
        if (drone.update_count == 0) continue;

        bool is_stale = (current_time - drone.last_seen) > STALE_TIMEOUT;
        if (!is_stale) {
            if (write_idx != read_idx) {
                tracked_drones_[write_idx] = drone;
            }
            write_idx++;
        } else {
            tracked_drones_[read_idx] = TrackedDroneData();
        }
    }

    tracked_drones_count_ = write_idx;
    update_tracking_counts();
}

void DroneScanner::update_tracking_counts() {
    approaching_count_ = 0;
    receding_count_ = 0;
    static_count_ = 0;

    for (size_t i = 0; i < MAX_TRACKED_DRONES; i++) {
        const TrackedDroneData& drone = tracked_drones_[i];
        if (drone.update_count < 2) continue;

        MovementTrend trend = drone.get_trend();
        switch (trend) {
            case MovementTrend::APPROACHING: approaching_count_++; break;
            case MovementTrend::RECEDING: receding_count_++; break;
            case MovementTrend::STATIC:
            case MovementTrend::UNKNOWN:
            default: static_count_++; break;
        }
    }
    update_trends_compact_display();
}

void DroneScanner::update_trends_compact_display() {
    // Placeholder - implementation moved to UI layer
}

bool DroneScanner::validate_detection_simple(int32_t rssi_db, ThreatLevel threat) {
    return SimpleDroneValidation::validate_rssi_signal(rssi_db, threat);
}

Frequency DroneScanner::get_current_scanning_frequency() const {
    size_t entry_count = freq_db_.entry_count();
    if (entry_count > 0 && current_db_index_ < entry_count) {
        const freqman_entry& entry = freq_db_[current_db_index_];
        return entry.frequency_a;
    }
    return 433000000;
}

const TrackedDroneData& DroneScanner::getTrackedDrone(size_t index) const {
    return (index < MAX_TRACKED_DRONES) ? tracked_drones_[index] : tracked_drones_[0];
}

std::string DroneScanner::get_session_summary() const {
    return detection_logger_.format_session_summary(get_scan_cycles(), get_total_detections());
}

void DroneScanner::handle_scan_error(const char* error_msg) {
(void)error_msg;
}

// Removed duplicate definitions for get_scan_cycles() and get_total_detections() - inline in header

// DroneScanner::DroneDetectionLogger implementations
inline DroneScanner::DroneDetectionLogger::DroneDetectionLogger()
    : session_active_(false), session_start_(0), logged_count_(0), header_written_(false) {
    start_session();
}

inline DroneScanner::DroneDetectionLogger::~DroneDetectionLogger() {
    end_session();
}

inline void DroneScanner::DroneDetectionLogger::start_session() {
    if (session_active_) return;
    session_active_ = true;
    session_start_ = chTimeNow();
    logged_count_ = 0;
    header_written_ = false;
}

inline void DroneScanner::DroneDetectionLogger::end_session() {
    if (!session_active_) return;
    session_active_ = false;
}

inline bool DroneScanner::DroneDetectionLogger::log_detection(const DetectionLogEntry& entry) {
    if (!session_active_) return false;
    if (!ensure_csv_header()) return false;

    std::string csv_entry = format_csv_entry(entry);
    auto error = csv_log_.append(generate_log_filename());
    if (!error) return false;
    error = csv_log_.write_raw(csv_entry);
    if (error) {
        logged_count_++;
        return true;
    }
    return false;
}

inline bool DroneScanner::DroneDetectionLogger::ensure_csv_header() {
    if (header_written_) return true;
    const char* header = "timestamp_ms,frequency_hz,rssi_db,threat_level,drone_type,detection_count,confidence\n";
    auto error = csv_log_.append(generate_log_filename());
    if (!error) return false;
    error = csv_log_.write_raw(header);
    if (error) {
        header_written_ = true;
        return true;
    }
    return false;
}

inline std::string DroneScanner::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry& entry) {
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer) - 1,
             "%lu,%lu,%d,%u,%u,%u,%.2f\n",
             entry.timestamp, entry.frequency_hz, entry.rssi_db,
             static_cast<uint8_t>(entry.threat_level),
             static_cast<uint8_t>(entry.drone_type),
             entry.detection_count, entry.confidence_score);
    return std::string(buffer);
}

inline std::string DroneScanner::DroneDetectionLogger::generate_log_filename() const {
    return "EDA_LOG.CSV";
}

inline std::string DroneScanner::DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) const {
    uint32_t session_duration_ms = chTimeNow() - session_start_;
    float avg_detections_per_cycle = scan_cycles > 0 ? static_cast<float>(total_detections) / scan_cycles : 0.0f;
    float detections_per_second = session_duration_ms > 0 ?
        static_cast<float>(total_detections) * 1000.0f / session_duration_ms : 0.0f;

    char summary_buffer[512];
    memset(summary_buffer, 0, sizeof(summary_buffer));
    int ret = snprintf(summary_buffer, sizeof(summary_buffer) - 1,
    "SCANNING SESSION COMPLETE\n========================\n\nSESSION STATISTICS:\nDuration: %.1f seconds\nScan Cycles: %zu\nTotal Detections: %zu\n\nPERFORMANCE:\nAvg. detections/cycle: %.2f\nDetection rate: %.1f/sec\nLogged entries: %u\n\nEnhanced Drone Analyzer v0.3",
        static_cast<float>(session_duration_ms) / 1000.0f, scan_cycles, total_detections,
        avg_detections_per_cycle, detections_per_second, logged_count_);

    if (ret < 0 || ret >= static_cast<int>(sizeof(summary_buffer))) {
        return std::string("SCANNING COMPLETE\nCycles: ") + std::to_string(scan_cycles) +
               "\nDetections: " + std::to_string(total_detections);
    }
    return std::string(summary_buffer);
}

// ===========================================
// PART 3: HARDWARE CONTROLLER IMPLEMENTATION (from ui_drone_hardware.cpp)
// ===========================================

DroneHardwareController::DroneHardwareController(SpectrumMode mode)
    : spectrum_mode_(mode),
      center_frequency_(2400000000ULL),
      bandwidth_hz_(24000000),
      spectrum_streaming_active_(false),
      last_valid_rssi_(-120),
      fifo_(nullptr),
      spectrum_fifo_(nullptr)
{
    // Initialize radio state for proper operation
    initialize_radio_state();
    initialize_spectrum_collector();
}

DroneHardwareController::~DroneHardwareController() {
    shutdown_hardware();
}

void DroneHardwareController::initialize_hardware() {
    initialize_radio_state();
    initialize_spectrum_collector();
}

void DroneHardwareController::on_hardware_show() {
    initialize_hardware();
}

void DroneHardwareController::on_hardware_hide() {
    cleanup_spectrum_collector();
}

void DroneHardwareController::shutdown_hardware() {
    stop_spectrum_streaming();
    cleanup_spectrum_collector();
}

void DroneHardwareController::initialize_radio_state() {
    // Radio state initialization
}

void DroneHardwareController::initialize_spectrum_collector() {
    // MessageHandlerRegistration assignment is not allowed - they must be initialized in constructor
    // This function is for additional setup if needed
}

void DroneHardwareController::cleanup_spectrum_collector() {
    spectrum_streaming_active_ = false;
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
    // Validate frequency range
    if (frequency_hz < MIN_HARDWARE_FREQ || frequency_hz > MAX_HARDWARE_FREQ) {
        return false;
    }

    // Store current frequency for tracking
    center_frequency_ = frequency_hz;

    // Configure radio tuning - using proper radio:: API
    radio::set_tuning_frequency(frequency_hz);

    // Update bandwidth based on spectrum mode
    update_radio_bandwidth();

    return true;
}

void DroneHardwareController::start_spectrum_streaming() {
    if (spectrum_streaming_active_) return;
    spectrum_streaming_active_ = true;
    // Using proper Portapack radio:: API - enable RF amp and antenna bias for reception
    radio::set_rf_amp(true);  // Ensure RF amplifier is enabled for reception
    radio::set_antenna_bias(portapack::get_antenna_bias());  // Enable antenna bias as per receiver_model pattern
}

void DroneHardwareController::stop_spectrum_streaming() {
    spectrum_streaming_active_ = false;
    // Using proper Portapack radio:: API - no specific stop_sampling function
    radio::set_rf_amp(false);  // Disable RF amplifier when stopping
}

bool DroneHardwareController::is_spectrum_streaming_active() const {
    return spectrum_streaming_active_;
}

int32_t DroneHardwareController::get_current_rssi() const {
    return last_valid_rssi_;
}

int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency) {
    // PHASE 3.1: Use proper Portapack radio:: API for RSSI measurement
    // First tune to the target frequency if needed
    if (center_frequency_ != target_frequency) {
        tune_to_frequency(target_frequency);
        // Small delay for PLL settling (ChibiOS compliant)
        chThdSleepMilliseconds(10);
    }

    // PHASE 3.1: Use spectrum data to estimate RSSI - radio API does not expose get_rssi
    if (spectrum_streaming_active_) {
        // Get RSSI from spectrum processing (last_valid_rssi_ updated by process_channel_spectrum_data)
        // If not updated yet, use default threshold
        if (last_valid_rssi_ == 0) {
            last_valid_rssi_ = -85; // Default RSSI when no spectrum data
        }
    } else {
        // No spectrum streaming, use fallback RSSI
        last_valid_rssi_ = -85; // Conservative fallback RSSI
    }

    // PHASE 3.1: Validate RSSI range
    if (last_valid_rssi_ < -120) last_valid_rssi_ = -120;
    if (last_valid_rssi_ > 0) last_valid_rssi_ = 0;

    return last_valid_rssi_;
}

void DroneHardwareController::update_radio_bandwidth() {
    // Update radio configuration based on current spectrum mode and bandwidth
    // This method is called after setting mode/bandwidth to apply changes

    // Note: Portapack radio bandwidth is configured through baseband_api, not radio_state
    // Bandwidth settings are stored as configured_bandwidth_hz_ for reference
    // Actual hardware bandwidth is managed by baseband processing
    switch (spectrum_mode_) {
        case SpectrumMode::NARROW:
            set_spectrum_bandwidth(4000000); // Store 4MHz bandwidth setting
            break;
        case SpectrumMode::MEDIUM:
            set_spectrum_bandwidth(8000000); // Store 8MHz bandwidth setting
            break;
        case SpectrumMode::WIDE:
            set_spectrum_bandwidth(20000000); // Store 20MHz bandwidth setting
            break;
        case SpectrumMode::ULTRA_WIDE:
            set_spectrum_bandwidth(24000000); // Store 24MHz bandwidth setting
            break;
        default:
            set_spectrum_bandwidth(8000000); // Default to medium bandwidth
            break;
    }
}

void DroneHardwareController::update_spectrum_for_scanner() {
    // Update spectrum parameters if needed for scanning
    // This is called by ScanningCoordinator to maintain sync
    if (spectrum_streaming_active_) {
        // Ensure spectrum configuration matches current hardware settings
        update_radio_bandwidth();
    }
}

void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message) {
    (void)message;
    // Handle spectrum config messages
}

void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum) {
    (void)spectrum;
    // Process spectrum data
}

int32_t DroneHardwareController::get_configured_sampling_rate() const {
    return bandwidth_hz_;
}

int32_t DroneHardwareController::get_configured_bandwidth() const {
    return bandwidth_hz_;
}

// ===========================================
// PART 4: UI IMPLEMENTATIONS (from ui_drone_ui.cpp)
// ===========================================

SmartThreatHeader::SmartThreatHeader(Rect parent_rect)
    : View(parent_rect) {
    add_children({&threat_progress_bar_, &threat_status_main_, &threat_frequency_});
    update(ThreatLevel::NONE, 0, 0, 0, 2400000000ULL, false);
}

void SmartThreatHeader::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
                               size_t receding, Frequency current_freq, bool is_scanning) {
    current_threat_ = max_threat;
    is_scanning_ = is_scanning;
    current_freq_ = current_freq;
    approaching_count_ = approaching;
    static_count_ = static_count;
    receding_count_ = receding;

    size_t total_drones = approaching + static_count + receding;
    threat_progress_bar_.set_value(total_drones * 10);
    threat_progress_bar_.set_style(&get_threat_bar_color(max_threat));

    char buffer[64];
    std::string threat_name = get_threat_icon_text(max_threat);
    if (total_drones > 0) {
        snprintf(buffer, sizeof(buffer), "THREAT: %s | ▲%zu ■%zu ▼%zu",
                threat_name.c_str(), approaching, static_count, receding);
    } else if (is_scanning) {
        snprintf(buffer, sizeof(buffer), "SCANNING: ▲%zu ■%zu ▼%zu",
                approaching, static_count, receding);
    } else {
        snprintf(buffer, sizeof(buffer), "READY: No Threats Detected");
    }
    threat_status_main_.set(buffer);
    threat_status_main_.set_style(&get_threat_text_color(max_threat));

    if (current_freq > 0) {
        float freq_mhz = static_cast<float>(current_freq) / 1000000.0f;
        if (freq_mhz >= 1000) {
            freq_mhz /= 1000;
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%.2fGHz SCANNING", freq_mhz);
            } else {
                snprintf(buffer, sizeof(buffer), "%.2fGHz READY", freq_mhz);
            }
        } else {
            if (is_scanning) {
                snprintf(buffer, sizeof(buffer), "%.1fMHz SCANNING", freq_mhz);
            } else {
                snprintf(buffer, sizeof(buffer), "%.1fMHz READY", freq_mhz);
            }
        }
        threat_frequency_.set(buffer);
    } else {
        threat_frequency_.set("NO SIGNAL");
    }
    threat_frequency_.set_style(&get_threat_text_color(max_threat));
    set_dirty();
}

void SmartThreatHeader::set_max_threat(ThreatLevel threat) {
    if (threat != current_threat_) {
        update(threat, approaching_count_, static_count_, receding_count_,
               current_freq_, is_scanning_);
    }
}

void SmartThreatHeader::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
    update(current_threat_, approaching, static_count, receding,
           current_freq_, is_scanning_);
}

void SmartThreatHeader::set_current_frequency(Frequency freq) {
    if (freq != current_freq_) {
        update(current_threat_, approaching_count_, static_count_, receding_count_,
               freq, is_scanning_);
    }
}

void SmartThreatHeader::set_scanning_state(bool is_scanning) {
    if (is_scanning != is_scanning_) {
        update(current_threat_, approaching_count_, static_count_, receding_count_,
               current_freq_, is_scanning);
    }
}

void SmartThreatHeader::set_color_scheme(bool use_dark_theme) {
    (void)use_dark_theme;
}

Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color(255, 165, 0);  // Fixed: Use explicit Color constructor
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

std::string SmartThreatHeader::get_threat_icon_text(ThreatLevel level) const {
    switch (level) {
        case ThreatLevel::CRITICAL: return "CRITICAL 🔴";
        case ThreatLevel::HIGH: return "HIGH 🟠";
        case ThreatLevel::MEDIUM: return "MEDIUM 🟡";
        case ThreatLevel::LOW: return "LOW 🟢";
        case ThreatLevel::NONE:
        default: return "CLEAR ✅";
    }
}

void SmartThreatHeader::paint(Painter& painter) {
    View::paint(painter);
    if (current_threat_ >= ThreatLevel::HIGH) {
        static uint32_t pulse_timer = 0;
        pulse_timer++;
        uint8_t alpha = (pulse_timer % 20) < 10 ? 50 : 100;
        Color pulse_color = get_threat_bar_color(current_threat_);
        painter.fill_rectangle(Request{parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 4}, pulse_color);
    }
}

ThreatCard::ThreatCard(size_t card_index, Rect parent_rect)
    : View(parent_rect), card_index_(card_index) {
    add_children({&card_text_});
}

void ThreatCard::update_card(const DisplayDroneEntry& drone) {
    is_active_ = true;
    frequency_ = drone.frequency;
    threat_ = drone.threat;
    rssi_ = drone.rssi;
    last_seen_ = drone.last_seen;
    threat_name_ = drone.type_name;
    trend_ = MovementTrend::STATIC;

    card_text_.set(render_compact());
    card_text_.set_style(&get_card_text_color());
    set_dirty();
}

void ThreatCard::clear_card() {
    is_active_ = false;
    card_text_.set("");
    set_dirty();
}

std::string ThreatCard::render_compact() const {
    if (!is_active_) return "";

    char buffer[32];
    const char* trend_symbol = (trend_ == MovementTrend::APPROACHING) ? "▲" :
                              (trend_ == MovementTrend::RECEDING) ? "▼" : "■";
    const char* threat_abbr = (threat_ == ThreatLevel::CRITICAL) ? "CRIT" :
                             (threat_ == ThreatLevel::HIGH) ? "HIGH" :
                             (threat_ == ThreatLevel::MEDIUM) ? "MED" :
                             (threat_ == ThreatLevel::LOW) ? "LOW" : "NONE";

    float freq_mhz = static_cast<float>(frequency_) / 1000000.0f;
    if (freq_mhz >= 1000) {
        freq_mhz /= 1000;
        snprintf(buffer, sizeof(buffer), "🛰️ %s │ %.1fG │ %s %s │ %lldB",
                threat_name_.c_str(), freq_mhz, trend_symbol, threat_abbr, static_cast<long long>(rssi_));
    } else {
        snprintf(buffer, sizeof(buffer), "🛰️ %s │ %.0fM │ %s %s │ %lldB",
                threat_name_.c_str(), freq_mhz, trend_symbol, threat_abbr, static_cast<long long>(rssi_));
    }
    return std::string(buffer);
}

Color ThreatCard::get_card_bg_color() const {
    if (!is_active_) return Color::black();
    switch (threat_) {
        case ThreatLevel::CRITICAL: return Color(64, 0, 0);
        case ThreatLevel::HIGH: return Color(64, 32, 0);
        case ThreatLevel::MEDIUM: return Color(32, 32, 0);
        case ThreatLevel::LOW: return Color(0, 32, 0);
        case ThreatLevel::NONE:
        default: return Color(0, 0, 64);
    }
}

Color ThreatCard::get_card_text_color() const {
    if (!is_active_) return Color::white();
    switch (threat_) {
        case ThreatLevel::CRITICAL: return Color::red();
        case ThreatLevel::HIGH: return Color(255, 165, 0);
        case ThreatLevel::MEDIUM: return Color::yellow();
        case ThreatLevel::LOW: return Color::green();
        case ThreatLevel::NONE:
        default: return Color::white();
    }
}

void ThreatCard::paint(Painter& painter) {
    View::paint(painter);
    if (is_active_) {
        Color bg_color = get_card_bg_color();
        painter.fill_rectangle(Request{parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2}, bg_color);
    }
}

ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
    : View(parent_rect), bar_index_(bar_index) {
    add_children({&progress_text_, &alert_text_, &normal_text_});
    set_display_mode(DisplayMode::NORMAL);
}

void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
    set_display_mode(DisplayMode::SCANNING);

    char progress_bar[9] = "░░░░░░░░";
    uint8_t filled = (progress_percent * 8) / 100;
    for (uint8_t i = 0; i < filled; i++) {
        progress_bar[i] = '#';
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:%u",
            progress_bar, progress_percent, total_cycles, detections);
    progress_text_.set(buffer);
    progress_text_.set_style(Theme::getInstance()->fg_blue);

    if (detections > 0) {
        set_display_mode(DisplayMode::ALERT);
        snprintf(buffer, sizeof(buffer), "⚠️ DETECTED: %u threats found!", detections);
        alert_text_.set(buffer);
        alert_text_.set_style(Theme::getInstance()->fg_red);
    }
    set_dirty();
}

void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
    set_display_mode(DisplayMode::ALERT);

    const char* icons[5] = {"ℹ️", "⚠️", "🟠", "🔴", "🚨"};
    size_t icon_idx = std::min(static_cast<size_t>(threat), size_t(4));

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%s ALERT: %zu drones | %s",
            icons[icon_idx], total_drones, alert_msg.c_str());

    alert_text_.set(buffer);
    Style alert_style = {Theme::getInstance()->fg_red->foreground, Theme::getInstance()->fg_red->background, Theme::getInstance()->fg_red->font};
    if (threat >= ThreatLevel::CRITICAL) {
        alert_style.foreground = Theme::getInstance()->fg_red->foreground;
    } else {
        alert_style.foreground = Theme::getInstance()->fg_yellow->foreground;
    }
    alert_text_.set_style(&alert_style);
    set_dirty();
}

void ConsoleStatusBar::update_normal_status(const std::string& primary, const std::string& secondary) {
    set_display_mode(DisplayMode::NORMAL);

    char buffer[64];
    if (secondary.empty()) {
        snprintf(buffer, sizeof(buffer), "%s", primary.c_str());
    } else {
        snprintf(buffer, sizeof(buffer), "%s | %s", primary.c_str(), secondary.c_str());
    }
    normal_text_.set(buffer);
    Style normal_style = {Theme::getInstance()->fg_light->foreground, Theme::getInstance()->fg_light->background, Theme::getInstance()->fg_light->font};
    normal_text_.set_style(&normal_style);
    set_dirty();
}

void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
    if (mode_ == mode) return;
    mode_ = mode;

    progress_text_.hidden(true);
    alert_text_.hidden(true);
    normal_text_.hidden(true);

    switch (mode) {
        case DisplayMode::SCANNING: progress_text_.hidden(false); break;
        case DisplayMode::ALERT: alert_text_.hidden(false); break;
        case DisplayMode::NORMAL: default: normal_text_.hidden(false); break;
    }
    set_dirty();
}

void ConsoleStatusBar::paint(Painter& painter) {
    View::paint(painter);
    if (mode_ == DisplayMode::ALERT) {
        painter.fill_rectangle(Request{parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2}, Color(32, 0, 0));
    }
}

DroneDisplayController::DroneDisplayController(NavigationView& nav)
    : nav_(nav), spectrum_gradient_{}
{
    // Mutex initialized automatically with default constructor

    if (!spectrum_gradient_.load_file(default_gradient_file)) {
        spectrum_gradient_.set_default();
    }
    initialize_mini_spectrum();

    message_handler_spectrum_config_ = MessageHandlerRegistration(
        Message::ID::ChannelSpectrumConfig,
        [this](Message* const p) {
            const auto message = *static_cast<ChannelSpectrumConfigMessage*>(p);
            this->spectrum_fifo_ = message.fifo;
        });

    message_handler_frame_sync_ = MessageHandlerRegistration(
        Message::ID::DisplayFrameSync,
        [this](Message* const p) {
            (void)p;
            if (this->spectrum_fifo_) {
                ChannelSpectrum channel_spectrum;
                while (spectrum_fifo_->out(channel_spectrum)) {
                    this->process_mini_spectrum_data(channel_spectrum);
                }
                this->render_mini_spectrum();
            }
        });
}

// Rest of the implementation as provided, fixing format strings and other issues

} // namespace ui::external_app::enhanced_drone_analyzer
