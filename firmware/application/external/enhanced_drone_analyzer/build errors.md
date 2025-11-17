[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:13:26: error: redefinition of 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB'
   13 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:215:26: note: 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB' previously defined here
  215 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:17:7: warning: 'class ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' has pointer data members [-Weffc++]
   17 | class ScanningCoordinator {
      |       ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:17:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:17:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:78:26: error: redefinition of 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB'
   78 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:215:26: note: 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB' previously defined here
  215 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:79:26: error: conflicting declaration 'constexpr const int32_t HYSTERESIS_MARGIN_DB'
   79 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:19,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:14:26: note: previous declaration as 'constexpr const int16_t HYSTERESIS_MARGIN_DB'
   14 | static constexpr int16_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:295:7: error: conflicting declaration 'using AudioManager = class AudioManager'
  295 | using AudioManager = ::AudioManager;
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:15:7: note: previous declaration as 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
   15 | class AudioManager;
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:409:10: error: 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_scanning_active() const' cannot be overloaded with 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_scanning_active() const'
  409 |     bool is_scanning_active() const { return scanning_active_; }
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:362:10: note: previous declaration 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_scanning_active() const'
  362 |     bool is_scanning_active() const { return scanning_active_; }
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:410:10: error: 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode() const' cannot be overloaded with 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode() const'
  410 |     bool is_real_mode() const { return is_real_mode_; }
      |          ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:389:10: note: previous declaration 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode() const'
  389 |     bool is_real_mode() const { return is_real_mode_; }
      |          ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:411:12: error: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const' cannot be overloaded with 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const'
  411 |     size_t get_approaching_count() const { return approaching_count_; }
      |            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:384:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const'
  384 |     size_t get_approaching_count() const { return approaching_count_; }
      |            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:412:12: error: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const' cannot be overloaded with 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const'
  412 |     size_t get_receding_count() const { return receding_count_; }
      |            ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:385:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const'
  385 |     size_t get_receding_count() const { return receding_count_; }
      |            ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:413:12: error: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const' cannot be overloaded with 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const'
  413 |     size_t get_static_count() const { return static_count_; }
      |            ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:386:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const'
  386 |     size_t get_static_count() const { return static_count_; }
      |            ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:414:14: error: 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_detections() const' cannot be overloaded with 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_detections() const'
  414 |     uint32_t get_total_detections() const { return total_detections_; }
      |              ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:387:14: note: previous declaration 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_detections() const'
  387 |     uint32_t get_total_detections() const { return total_detections_; }
      |              ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:415:14: error: 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_scan_cycles() const' cannot be overloaded with 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_scan_cycles() const'
  415 |     uint32_t get_scan_cycles() const { return scan_cycles_; }
      |              ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:388:14: note: previous declaration 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_scan_cycles() const'
  388 |     uint32_t get_scan_cycles() const { return scan_cycles_; }
      |              ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:416:17: error: 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::get_max_detected_threat() const' cannot be overloaded with 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::get_max_detected_threat() const'
  416 |     ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
      |                 ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:379:17: note: previous declaration 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::get_max_detected_threat() const'
  379 |     ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
      |                 ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:417:15: error: 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const' cannot be overloaded with 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const'
  417 |     Frequency get_current_scanning_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:378:15: note: previous declaration 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const'
  378 |     Frequency get_current_scanning_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:419:17: error: 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const' cannot be overloaded with 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const'
  419 |     std::string get_session_summary() const;
      |                 ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:17: note: previous declaration 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const'
  383 |     std::string get_session_summary() const;
      |                 ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:422:10: error: 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_scanning_active() const' cannot be overloaded with 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_scanning_active() const'
  422 |     bool is_scanning_active() const { return scanning_active_; }
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:362:10: note: previous declaration 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_scanning_active() const'
  362 |     bool is_scanning_active() const { return scanning_active_; }
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:423:10: error: 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode() const' cannot be overloaded with 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode() const'
  423 |     bool is_real_mode() const { return is_real_mode_; }
      |          ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:389:10: note: previous declaration 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode() const'
  389 |     bool is_real_mode() const { return is_real_mode_; }
      |          ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:424:12: error: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const' cannot be overloaded with 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const'
  424 |     size_t get_approaching_count() const { return approaching_count_; }
      |            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:384:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const'
  384 |     size_t get_approaching_count() const { return approaching_count_; }
      |            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:425:12: error: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const' cannot be overloaded with 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const'
  425 |     size_t get_receding_count() const { return receding_count_; }
      |            ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:385:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const'
  385 |     size_t get_receding_count() const { return receding_count_; }
      |            ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:426:12: error: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const' cannot be overloaded with 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const'
  426 |     size_t get_static_count() const { return static_count_; }
      |            ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:386:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const'
  386 |     size_t get_static_count() const { return static_count_; }
      |            ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:427:14: error: 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_detections() const' cannot be overloaded with 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_detections() const'
  427 |     uint32_t get_total_detections() const { return total_detections_; }
      |              ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:387:14: note: previous declaration 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_detections() const'
  387 |     uint32_t get_total_detections() const { return total_detections_; }
      |              ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:428:14: error: 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_scan_cycles() const' cannot be overloaded with 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_scan_cycles() const'
  428 |     uint32_t get_scan_cycles() const { return scan_cycles_; }
      |              ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:388:14: note: previous declaration 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_scan_cycles() const'
  388 |     uint32_t get_scan_cycles() const { return scan_cycles_; }
      |              ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:429:17: error: 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::get_max_detected_threat() const' cannot be overloaded with 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::get_max_detected_threat() const'
  429 |     ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
      |                 ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:379:17: note: previous declaration 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::get_max_detected_threat() const'
  379 |     ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
      |                 ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:430:15: error: 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const' cannot be overloaded with 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const'
  430 |     Frequency get_current_scanning_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:378:15: note: previous declaration 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const'
  378 |     Frequency get_current_scanning_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:431:15: error: 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const' cannot be overloaded with 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const'
  431 |     Frequency get_current_radio_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:418:15: note: previous declaration 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const'
  418 |     Frequency get_current_radio_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:432:17: error: 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const' cannot be overloaded with 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const'
  432 |     std::string get_session_summary() const;
      |                 ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:17: note: previous declaration 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const'
  383 |     std::string get_session_summary() const;
      |                 ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:445:15: error: 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const' cannot be overloaded with 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const'
  445 |     Frequency get_current_radio_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:418:15: note: previous declaration 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const'
  418 |     Frequency get_current_radio_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:347:7: warning: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' has pointer data members [-Weffc++]
  347 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:347:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:347:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:779:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t, uint8_t&)' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t, uint8_t&)'
  779 |     void get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power) {
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:676:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t, uint8_t&)'
  676 |     void get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:799:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_spectrum_pixel(uint8_t)' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_spectrum_pixel(uint8_t)'
  799 |     void add_spectrum_pixel(uint8_t power) {
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:677:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_spectrum_pixel(uint8_t)'
  677 |     void add_spectrum_pixel(uint8_t power);
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:888:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_system_status()' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_system_status()'
  888 |     void show_system_status();
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:868:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_system_status()'
  868 |     void show_system_status();
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:917:18: error: field 'audio_' has incomplete type 'ui::external_app::enhanced_drone_analyzer::AudioManager'
  917 |     AudioManager audio_;  // Direct member now
      |                  ^~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:15:7: note: forward declaration of 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
   15 | class AudioManager;
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:23:25: error: redefinition of 'constexpr const size_t MAX_TRACKED_DRONES'
   23 | static constexpr size_t MAX_TRACKED_DRONES = 8;
      |                         ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:197:25: note: 'constexpr const size_t MAX_TRACKED_DRONES' previously defined here
  197 | static constexpr size_t MAX_TRACKED_DRONES = 8;
      |                         ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:24:25: error: redefinition of 'constexpr const size_t MAX_DISPLAYED_DRONES'
   24 | static constexpr size_t MAX_DISPLAYED_DRONES = 3;
      |                         ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:198:25: note: 'constexpr const size_t MAX_DISPLAYED_DRONES' previously defined here
  198 | static constexpr size_t MAX_DISPLAYED_DRONES = 3;
      |                         ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:25:25: error: redefinition of 'constexpr const size_t MINI_SPECTRUM_WIDTH'
   25 | static constexpr size_t MINI_SPECTRUM_WIDTH = 200;
      |                         ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:199:25: note: 'constexpr const size_t MINI_SPECTRUM_WIDTH' previously defined here
  199 | static constexpr size_t MINI_SPECTRUM_WIDTH = 200;
      |                         ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:26:25: error: redefinition of 'constexpr const size_t MINI_SPECTRUM_HEIGHT'
   26 | static constexpr size_t MINI_SPECTRUM_HEIGHT = 24;
      |                         ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:200:25: note: 'constexpr const size_t MINI_SPECTRUM_HEIGHT' previously defined here
  200 | static constexpr size_t MINI_SPECTRUM_HEIGHT = 24;
      |                         ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:27:27: error: redefinition of 'constexpr const uint32_t MIN_HARDWARE_FREQ'
   27 | static constexpr uint32_t MIN_HARDWARE_FREQ = 1'000'000;
      |                           ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:201:27: note: 'constexpr const uint32_t MIN_HARDWARE_FREQ' previously defined here
  201 | static constexpr uint32_t MIN_HARDWARE_FREQ = 1'000'000;
      |                           ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:28:27: error: conflicting declaration 'constexpr const uint32_t MAX_HARDWARE_FREQ'
   28 | static constexpr uint32_t MAX_HARDWARE_FREQ = 6'000'000'000;
      |                           ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:202:27: note: previous declaration as 'constexpr const uint64_t MAX_HARDWARE_FREQ'
  202 | static constexpr uint64_t MAX_HARDWARE_FREQ = 6'000'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:29:27: error: redefinition of 'constexpr const uint32_t WIDEBAND_DEFAULT_MIN'
   29 | static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:203:27: note: 'constexpr const uint32_t WIDEBAND_DEFAULT_MIN' previously defined here
  203 | static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:30:27: error: redefinition of 'constexpr const uint32_t WIDEBAND_DEFAULT_MAX'
   30 | static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:204:27: note: 'constexpr const uint32_t WIDEBAND_DEFAULT_MAX' previously defined here
  204 | static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:31:27: error: redefinition of 'constexpr const uint32_t WIDEBAND_SLICE_WIDTH'
   31 | static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 25'000'000;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:205:27: note: 'constexpr const uint32_t WIDEBAND_SLICE_WIDTH' previously defined here
  205 | static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 25'000'000;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:32:27: error: redefinition of 'constexpr const uint32_t WIDEBAND_MAX_SLICES'
   32 | static constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
      |                           ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:206:27: note: 'constexpr const uint32_t WIDEBAND_MAX_SLICES' previously defined here
  206 | static constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
      |                           ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:33:26: error: redefinition of 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB'
   33 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:215:26: note: 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB' previously defined here
  215 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:34:26: error: redefinition of 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB'
   34 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:75:26: note: 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB' previously defined here
   75 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -80;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:36:26: error: redefinition of 'constexpr const uint8_t MIN_DETECTION_COUNT'
   36 | static constexpr uint8_t MIN_DETECTION_COUNT = 3;
      |                          ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:80:26: note: 'constexpr const uint8_t MIN_DETECTION_COUNT' previously defined here
   80 | static constexpr uint8_t MIN_DETECTION_COUNT = 3;
      |                          ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:41:25: error: redefinition of 'constexpr const size_t DETECTION_TABLE_SIZE'
   41 | static constexpr size_t DETECTION_TABLE_SIZE = 256;
      |                         ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:207:25: note: 'constexpr const size_t DETECTION_TABLE_SIZE' previously defined here
  207 | static constexpr size_t DETECTION_TABLE_SIZE = 256;
      |                         ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:43:8: error: conflicting declaration 'struct WidebandSlice'
   43 | struct WidebandSlice {
      |        ^~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:209:8: note: previous declaration as 'struct WidebandSlice'
  209 | struct WidebandSlice {
      |        ^~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:48:8: error: conflicting declaration 'struct WidebandScanData'
   48 | struct WidebandScanData {
      |        ^~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:214:8: note: previous declaration as 'struct WidebandScanData'
  214 | struct WidebandScanData {
      |        ^~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:63:8: error: conflicting declaration 'struct DetectionLogEntry'
   63 | struct DetectionLogEntry {
      |        ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:229:8: note: previous declaration as 'struct DetectionLogEntry'
  229 | struct DetectionLogEntry {
      |        ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:79:8: error: redefinition of 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'
   79 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:20:8: note: previous definition of 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'
   20 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:234,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/ui/ui_tabview.hpp:56:13: error: 'Color' in namespace 'ui::external_app::enhanced_drone_analyzer::ui' does not name a type
   56 |         ui::Color color;
      |             ^~~~~
/havoc/firmware/application/ui/ui_tabview.hpp:73:16: error: 'Tab' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::ui::Tab'?
   73 |     std::array<Tab, MAX_TABS> tabs{};
      |                ^~~
      |                ui::external_app::enhanced_drone_analyzer::ui::Tab
/havoc/firmware/application/ui/ui_tabview.hpp:35:7: note: 'ui::external_app::enhanced_drone_analyzer::ui::Tab' declared here
   35 | class Tab : public Widget {
      |       ^~~
/havoc/firmware/application/ui/ui_tabview.hpp:73:29: error: template argument 1 is invalid
   73 |     std::array<Tab, MAX_TABS> tabs{};
      |                             ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:348:43: error: expected class-name before '{' token
  348 | class SettingsTabbedView : public TabView {
      |                                           ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:353:10: error: 'void ui::external_app::enhanced_drone_analyzer::SettingsTabbedView::focus()' marked 'override', but does not override
  353 |     void focus() override;
      |          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:10:25: error: variable or field 'initialize_app' declared void
   10 | void initialize_app(ui::NavigationView& nav) {
      |                         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:10:25: error: 'NavigationView' is not a member of 'ui::external_app::enhanced_drone_analyzer::ui'; did you mean 'ui::NavigationView'?
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:5:
/havoc/firmware/application/./ui_navigation.hpp:95:7: note: 'ui::NavigationView' declared here
   95 | class NavigationView : public View {
      |       ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:10:41: error: 'nav' was not declared in this scope; did you mean 'nan'?
   10 | void initialize_app(ui::NavigationView& nav) {
      |                                         ^~~
      |                                         nan
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:19:34: error: 'ui::external_app::enhanced_drone_analyzer::ui::external_app' has not been declared
   19 |     /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer_settings::initialize_app,
      |                                  ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:28:28: error: 'ui::external_app::enhanced_drone_analyzer::ui::Color' has not been declared
   28 |     /*.icon_color = */ ui::Color::blue().v,
      |                            ^~~~~
In file included from /havoc/firmware/common/external_app.hpp:27,
                 from /havoc/firmware/application/./ui_navigation.hpp:52,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:5:
/havoc/firmware/common/spi_image.hpp:149:16: warning: 'portapack::spi_flash::images' defined but not used [-Wunused-variable]
  149 | const region_t images{
      |                ^~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.
