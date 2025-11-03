[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::BufferedDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:530:12: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::entries_count_' will be initialized after [-Wreorder]
  530 |     size_t entries_count_;
      |            ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:525:10: warning:   'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::session_active_' [-Wreorder]
  525 |     bool session_active_;
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:468:5: warning:   when initialized here [-Wreorder]
  468 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false),
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:527:10: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::header_written_' will be initialized after [-Wreorder]
  527 |     bool header_written_;
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:526:15: warning:   'systime_t ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::session_start_' [-Wreorder]
  526 |     systime_t session_start_;
      |               ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:468:5: warning:   when initialized here [-Wreorder]
  468 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false),
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:468:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  383 |     FreqDBCache() = default;
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_freq_db_cache_logic(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1316:21: note: synthesized method 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()' first required here
 1316 |         FreqDBCache test_cache;
      |                     ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1456:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1456 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1499:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1499 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: In function 'void ScannerSettingsManager::reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:99:77: error: 'ui::external_app::enhanced_drone_analyzer::SpectrumMode' has not been declared
   99 |         settings.spectrum_mode = ui::external_app::enhanced_drone_analyzer::SpectrumMode::MEDIUM;
      |                                                                             ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:101:81: error: 'DEFAULT_RSSI_THRESHOLD_DB' is not a member of 'ui::external_app::enhanced_drone_analyzer'; did you mean 'DEFAULT_RSSI_THRESHOLD_DB'?
  101 |         settings.rssi_threshold_db = ui::external_app::enhanced_drone_analyzer::DEFAULT_RSSI_THRESHOLD_DB;
      |                                                                                 ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:155:26: note: 'DEFAULT_RSSI_THRESHOLD_DB' declared here
  155 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: In function 'SpectrumMode ScannerSettingsManager::parse_spectrum_mode(const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:112:82: error: 'ui::external_app::enhanced_drone_analyzer::SpectrumMode' has not been declared
  112 |         if (value == "NARROW") return ui::external_app::enhanced_drone_analyzer::SpectrumMode::NARROW;
      |                                                                                  ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:113:82: error: 'ui::external_app::enhanced_drone_analyzer::SpectrumMode' has not been declared
  113 |         if (value == "MEDIUM") return ui::external_app::enhanced_drone_analyzer::SpectrumMode::MEDIUM;
      |                                                                                  ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:114:80: error: 'ui::external_app::enhanced_drone_analyzer::SpectrumMode' has not been declared
  114 |         if (value == "WIDE") return ui::external_app::enhanced_drone_analyzer::SpectrumMode::WIDE;
      |                                                                                ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:115:86: error: 'ui::external_app::enhanced_drone_analyzer::SpectrumMode' has not been declared
  115 |         if (value == "ULTRA_WIDE") return ui::external_app::enhanced_drone_analyzer::SpectrumMode::ULTRA_WIDE;
      |                                                                                      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:116:59: error: 'ui::external_app::enhanced_drone_analyzer::SpectrumMode' has not been declared
  116 |         return ui::external_app::enhanced_drone_analyzer::SpectrumMode::MEDIUM;
      |                                                           ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: In function 'bool ScannerSettingsManager::load_from_txt_impl(const string&, ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:133:13: error: 'File' is not a member of 'ui'; did you mean 'File'?
  133 |         ui::File txt_file;
      |             ^~~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/./ui_navigation.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/havoc/firmware/application/./file.hpp:308:7: note: 'File' declared here
  308 | class File {
      |       ^~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:134:14: error: 'txt_file' was not declared in this scope
  134 |         if (!txt_file.open(filepath, true)) {  // true = read_only parameter
      |              ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:139:29: error: 'txt_file' was not declared in this scope
  139 |         file_content.resize(txt_file.size());
      |                             ^~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.