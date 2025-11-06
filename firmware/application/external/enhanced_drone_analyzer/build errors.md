[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7: warning: 'class ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' has pointer data members [-Weffc++]
   18 | class ScanningCoordinator {
      |       ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:405:18: error: 'static msg_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_function(void*)' cannot be overloaded with 'static msg_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_function(void*)'
  405 |     static msg_t scanning_thread_function(void* arg);
      |                  ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:362:18: note: previous declaration 'static msg_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_function(void*)'
  362 |     static msg_t scanning_thread_function(void* arg);
      |                  ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:406:11: error: 'msg_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread()' cannot be overloaded with 'msg_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread()'
  406 |     msg_t scanning_thread();
      |           ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:363:11: note: previous declaration 'msg_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread()'
  363 |     msg_t scanning_thread();
      |           ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:408:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_database_and_scanner()' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_database_and_scanner()'
  408 |     void initialize_database_and_scanner();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:365:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_database_and_scanner()'
  365 |     void initialize_database_and_scanner();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:409:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::cleanup_database_and_scanner()' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::cleanup_database_and_scanner()'
  409 |     void cleanup_database_and_scanner();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:366:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::cleanup_database_and_scanner()'
  366 |     void cleanup_database_and_scanner();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:410:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_init_from_loaded_frequencies()' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_init_from_loaded_frequencies()'
  410 |     void scan_init_from_loaded_frequencies();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:367:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_init_from_loaded_frequencies()'
  367 |     void scan_init_from_loaded_frequencies();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:412:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)'
  412 |     void perform_database_scan_cycle(DroneHardwareController& hardware);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:369:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)'
  369 |     void perform_database_scan_cycle(DroneHardwareController& hardware);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:413:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_wideband_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_wideband_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)'
  413 |     void perform_wideband_scan_cycle(DroneHardwareController& hardware);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:370:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_wideband_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)'
  370 |     void perform_wideband_scan_cycle(DroneHardwareController& hardware);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:414:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_hybrid_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_hybrid_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)'
  414 |     void perform_hybrid_scan_cycle(DroneHardwareController& hardware);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:371:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_hybrid_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)'
  371 |     void perform_hybrid_scan_cycle(DroneHardwareController& hardware);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:416:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_tracking_counts()' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_tracking_counts()'
  416 |     void update_tracking_counts();
      |          ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:373:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_tracking_counts()'
  373 |     void update_tracking_counts();
      |          ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:417:10: error: 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_trends_compact_display()' cannot be overloaded with 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_trends_compact_display()'
  417 |     void update_trends_compact_display();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:358:10: note: previous declaration 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_trends_compact_display()'
  358 |     void update_trends_compact_display();
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:418:10: error: 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::validate_detection_simple(int32_t, ThreatLevel)' cannot be overloaded with 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::validate_detection_simple(int32_t, ThreatLevel)'
  418 |     bool validate_detection_simple(int32_t rssi_db, ThreatLevel threat);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:359:10: note: previous declaration 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::validate_detection_simple(int32_t, ThreatLevel)'
  359 |     bool validate_detection_simple(int32_t rssi_db, ThreatLevel threat);
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:419:15: error: 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const' cannot be overloaded with 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const'
  419 |     Frequency get_current_radio_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:360:15: note: previous declaration 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_radio_frequency() const'
  360 |     Frequency get_current_radio_frequency() const;
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:421:32: error: redeclaration of 'Thread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_'
  421 |     Thread* scanning_thread_ = nullptr;
      |                                ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:375:13: note: previous declaration 'Thread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_'
  375 |     Thread* scanning_thread_ = nullptr;
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:422:56: error: redeclaration of 'constexpr const uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::SCAN_THREAD_STACK_SIZE'
  422 |     static constexpr uint32_t SCAN_THREAD_STACK_SIZE = 2048;
      |                                                        ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:376:31: note: previous declaration 'constexpr const uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::SCAN_THREAD_STACK_SIZE'
  376 |     static constexpr uint32_t SCAN_THREAD_STACK_SIZE = 2048;
      |                               ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:423:29: error: redeclaration of 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_'
  423 |     bool scanning_active_ = false;
      |                             ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:377:10: note: previous declaration 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_'
  377 |     bool scanning_active_ = false;
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:425:16: error: redeclaration of 'freqman_db ui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_'
  425 |     freqman_db freq_db_;
      |                ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:379:16: note: previous declaration 'freqman_db ui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_'
  379 |     freqman_db freq_db_;
      |                ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:426:32: error: redeclaration of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::current_db_index_'
  426 |     size_t current_db_index_ = 0;
      |                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:380:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::current_db_index_'
  380 |     size_t current_db_index_ = 0;
      |            ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:427:41: error: redeclaration of 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::last_scanned_frequency_'
  427 |     Frequency last_scanned_frequency_ = 0;
      |                                         ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:381:15: note: previous declaration 'Frequency ui::external_app::enhanced_drone_analyzer::DroneScanner::last_scanned_frequency_'
  381 |     Frequency last_scanned_frequency_ = 0;
      |               ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:429:29: error: redeclaration of 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_cycles_'
  429 |     uint32_t scan_cycles_ = 0;
      |                             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:14: note: previous declaration 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_cycles_'
  383 |     uint32_t scan_cycles_ = 0;
      |              ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:430:34: error: redeclaration of 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::total_detections_'
  430 |     uint32_t total_detections_ = 0;
      |                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:384:14: note: previous declaration 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::total_detections_'
  384 |     uint32_t total_detections_ = 0;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:432:49: error: redeclaration of 'ui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_mode_'
  432 |     ScanningMode scanning_mode_ = ScanningMode::DATABASE;
      |                                                 ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:386:18: note: previous declaration 'ui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_mode_'
  386 |     ScanningMode scanning_mode_ = ScanningMode::DATABASE;
      |                  ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:433:26: error: redeclaration of 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode_'
  433 |     bool is_real_mode_ = true;
      |                          ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:387:10: note: previous declaration 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode_'
  387 |     bool is_real_mode_ = true;
      |          ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:435:50: error: redeclaration of 'std::array<TrackedDrone, 8> ui::external_app::enhanced_drone_analyzer::DroneScanner::tracked_drones_'
  435 |     std::array<TrackedDrone, MAX_TRACKED_DRONES> tracked_drones_;
      |                                                  ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:389:50: note: previous declaration 'std::array<TrackedDrone, 8> ui::external_app::enhanced_drone_analyzer::DroneScanner::tracked_drones_'
  389 |     std::array<TrackedDrone, MAX_TRACKED_DRONES> tracked_drones_;
      |                                                  ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:436:36: error: redeclaration of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::tracked_drones_count_'
  436 |     size_t tracked_drones_count_ = 0;
      |                                    ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:390:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::tracked_drones_count_'
  390 |     size_t tracked_drones_count_ = 0;
      |            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:438:33: error: redeclaration of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::approaching_count_'
  438 |     size_t approaching_count_ = 0;
      |                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:392:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::approaching_count_'
  392 |     size_t approaching_count_ = 0;
      |            ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:439:30: error: redeclaration of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::receding_count_'
  439 |     size_t receding_count_ = 0;
      |                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:393:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::receding_count_'
  393 |     size_t receding_count_ = 0;
      |            ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:440:28: error: redeclaration of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::static_count_'
  440 |     size_t static_count_ = 0;
      |                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:394:12: note: previous declaration 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::static_count_'
  394 |     size_t static_count_ = 0;
      |            ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:442:53: error: redeclaration of 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::max_detected_threat_'
  442 |     ThreatLevel max_detected_threat_ = ThreatLevel::NONE;
      |                                                     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:396:17: note: previous declaration 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::max_detected_threat_'
  396 |     ThreatLevel max_detected_threat_ = ThreatLevel::NONE;
      |                 ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:443:33: error: redeclaration of 'int32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::last_valid_rssi_'
  443 |     int32_t last_valid_rssi_ = -120;
      |                                 ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:397:13: note: previous declaration 'int32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::last_valid_rssi_'
  397 |     int32_t last_valid_rssi_ = -120;
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:445:48: error: redeclaration of 'constexpr const uint8_t ui::external_app::enhanced_drone_analyzer::DroneScanner::DETECTION_DELAY'
  445 |     static constexpr uint8_t DETECTION_DELAY = 3;
      |                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:399:30: note: previous declaration 'constexpr const uint8_t ui::external_app::enhanced_drone_analyzer::DroneScanner::DETECTION_DELAY'
  399 |     static constexpr uint8_t DETECTION_DELAY = 3;
      |                              ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:446:22: error: redeclaration of 'WidebandScanData ui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_'
  446 |     WidebandScanData wideband_scan_data_;
      |                      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:400:22: note: previous declaration 'WidebandScanData ui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_'
  400 |     WidebandScanData wideband_scan_data_;
      |                      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:447:16: error: redeclaration of 'freqman_db ui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_'
  447 |     freqman_db drone_database_;
      |                ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:401:16: note: previous declaration 'freqman_db ui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_'
  401 |     freqman_db drone_database_;
      |                ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:448:26: error: redeclaration of 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger ui::external_app::enhanced_drone_analyzer::DroneScanner::detection_logger_'
  448 |     DroneDetectionLogger detection_logger_;
      |                          ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:402:26: note: previous declaration 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger ui::external_app::enhanced_drone_analyzer::DroneScanner::detection_logger_'
  402 |     DroneDetectionLogger detection_logger_;
      |                          ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7: warning: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' has pointer data members [-Weffc++]
  300 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.