[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:405:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  405 |     FreqDBCache() {}
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:405:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::BufferedDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:551:12: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::entries_count_' will be initialized after [-Wreorder]
  551 |     size_t entries_count_;
      |            ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:546:10: warning:   'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::session_active_' [-Wreorder]
  546 |     bool session_active_;
      |          ^~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:490:5: warning:   when initialized here [-Wreorder]
  490 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false), session_start_(0), header_written_(false), logged_total_count_(0) {}
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:490:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::get_scan_cycles() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1124:45: error: 'scan_cycles_' was not declared in this scope; did you mean 'get_scan_cycles'?
 1124 |     size_t get_scan_cycles() const { return scan_cycles_; }
      |                                             ^~~~~~~~~~~~
      |                                             get_scan_cycles
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'uint32_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::get_total_detections() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1125:52: error: 'total_detections_' was not declared in this scope; did you mean 'get_total_detections'?
 1125 |     uint32_t get_total_detections() const { return total_detections_; }
      |                                                    ^~~~~~~~~~~~~~~~~
      |                                                    get_total_detections
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::get_approaching_count() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1127:51: error: 'approaching_count_' was not declared in this scope; did you mean 'get_approaching_count'?
 1127 |     size_t get_approaching_count() const { return approaching_count_; }
      |                                                   ^~~~~~~~~~~~~~~~~~
      |                                                   get_approaching_count
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::get_receding_count() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1128:48: error: 'receding_count_' was not declared in this scope; did you mean 'get_receding_count'?
 1128 |     size_t get_receding_count() const { return receding_count_; }
      |                                                ^~~~~~~~~~~~~~~
      |                                                get_receding_count
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::get_static_count() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1129:46: error: 'static_count_' was not declared in this scope; did you mean 'get_static_count'?
 1129 |     size_t get_static_count() const { return static_count_; }
      |                                              ^~~~~~~~~~~~~
      |                                              get_static_count
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::is_real_mode() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1130:40: error: 'is_real_mode_' was not declared in this scope; did you mean 'is_real_mode'?
 1130 |     bool is_real_mode() const { return is_real_mode_; }
      |                                        ^~~~~~~~~~~~~
      |                                        is_real_mode
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'ThreatLevel ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::get_max_detected_threat() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1131:58: error: 'max_detected_threat_' was not declared in this scope; did you mean 'get_max_detected_threat'?
 1131 |     ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
      |                                                          ^~~~~~~~~~~~~~~~~~~~
      |                                                          get_max_detected_threat
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1531:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1531 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1574:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1574 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.