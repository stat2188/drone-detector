[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::BufferedDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:502:12: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::entries_count_' will be initialized after [-Wreorder]
  502 |     size_t entries_count_;
      |            ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:497:10: warning:   'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::session_active_' [-Wreorder]
  497 |     bool session_active_;
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:440:5: warning:   when initialized here [-Wreorder]
  440 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false),
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:501:12: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::logged_total_count_' will be initialized after [-Wreorder]
  501 |     size_t logged_total_count_;
      |            ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:499:10: warning:   'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::header_written_' [-Wreorder]
  499 |     bool header_written_;
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:440:5: warning:   when initialized here [-Wreorder]
  440 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false),
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:440:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::format_csv_entry(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:524:28: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
  524 |                  "%lu,%lu,%d,%u,%u,%u,%.2f\n",
      |                           ~^
      |                            |
      |                            int
      |                           %ld
  525 |                  entry.timestamp, entry.frequency_hz, entry.rssi_db,
      |                                                       ~~~~~~~~~~~~~
      |                                                             |
      |                                                             int32_t {aka long int}
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1117:18: error: field 'audio_mgr_' has incomplete type 'ui::external_app::enhanced_drone_analyzer::AudioManager'
 1117 |     AudioManager audio_mgr_;                 // Direct member - RAII safe
      |                  ^~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:283:7: note: forward declaration of 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  283 | class AudioManager;
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:355:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  355 |     FreqDBCache() = default;
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:355:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_freq_db_cache_logic(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1276:21: note: synthesized method 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()' first required here
 1276 |         FreqDBCache test_cache;
      |                     ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1416:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1416 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1459:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1459 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:28:17: warning: 'bool ScannerSettingsManager::load_settings_from_txt(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)' defined but not used [-Wunused-function]
   28 |     static bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
      |                 ^~~~~~~~~~~~~~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.