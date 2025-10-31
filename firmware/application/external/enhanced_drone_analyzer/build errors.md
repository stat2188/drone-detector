In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::BufferedDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:490:12: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::entries_count_' will be initialized after [-Wreorder]
  490 |     size_t entries_count_;
      |            ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:485:10: warning:   'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::session_active_' [-Wreorder]
  485 |     bool session_active_;
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:428:5: warning:   when initialized here [-Wreorder]
  428 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false),
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:489:12: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::logged_total_count_' will be initialized after [-Wreorder]
  489 |     size_t logged_total_count_;
      |            ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:487:10: warning:   'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::header_written_' [-Wreorder]
  487 |     bool header_written_;
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:428:5: warning:   when initialized here [-Wreorder]
  428 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false),
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:428:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::log_detection(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:438:40: error: 'chVTGetSystemTime' was not declared in this scope
  438 |         const systime_t current_time = chVTGetSystemTime();
      |                                        ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:460:19: error: 'class Optional<std::filesystem::filesystem_error>' has no member named 'has_value'
  460 |         if (error.has_value()) {
      |                   ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:461:32: error: 'chVTGetSystemTime' was not declared in this scope
  461 |             last_flush_time_ = chVTGetSystemTime();
      |                                ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::ensure_csv_header()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:501:19: error: 'class Optional<std::filesystem::filesystem_error>' has no member named 'has_value'
  501 |         if (error.has_value()) {
      |                   ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::format_csv_entry(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:512:28: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
  512 |                  "%lu,%lu,%d,%u,%u,%u,%.2f\n",
      |                           ~^
      |                            |
      |                            int
      |                           %ld
  513 |                  entry.timestamp, entry.frequency_hz, entry.rssi_db,
      |                                                       ~~~~~~~~~~~~~
      |                                                             |
      |                                                             int32_t {aka long int}
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1098:18: error: field 'audio_mgr_' has incomplete type 'ui::external_app::enhanced_drone_analyzer::AudioManager'
 1098 |     AudioManager audio_mgr_;                 // Direct member - RAII safe
      |                  ^~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:275:7: note: forward declaration of 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  275 | class AudioManager;
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:343:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  343 |     FreqDBCache() = default;
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:343:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_freq_db_cache_logic(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1254:21: note: synthesized method 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()' first required here
 1254 |         FreqDBCache test_cache;
      |                     ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1394:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1394 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1437:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1437 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:18:65: error: 'DroneAnalyzerSettings' has not been declared
   18 |     static bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings);
      |                                                                 ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:21:33: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
   21 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                                 ^~~~~~~~~~~~~~~~~~~~~
      |                                 ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:539:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  539 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:21:56: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   21 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                                                        ^~~~~~~~
      |                                                        app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:21:66: error: expected primary-expression before 'const'
   21 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                                                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:21:89: error: expression list treated as compound expression in initializer [-fpermissive]
   21 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                                                                                         ^
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:40: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
   22 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                        ^~~~~~~~~~~~~~~~~~~~~
      |                                        ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:539:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  539 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:63: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   22 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                                               ^~~~~~~~
      |                                                               app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:73: error: expected primary-expression before 'const'
   22 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                                                         ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:99: error: expression list treated as compound expression in initializer [-fpermissive]
   22 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                                                                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:35: error: variable or field 'reset_to_defaults' declared void
   23 |     static void reset_to_defaults(DroneAnalyzerSettings& settings);
      |                                   ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:35: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
   23 |     static void reset_to_defaults(DroneAnalyzerSettings& settings);
      |                                   ^~~~~~~~~~~~~~~~~~~~~
      |                                   ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:539:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  539 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:58: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   23 |     static void reset_to_defaults(DroneAnalyzerSettings& settings);
      |                                                          ^~~~~~~~
      |                                                          app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:27:40: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
   27 |     static bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
      |                                        ^~~~~~~~~~~~~~~~~~~~~
      |                                        ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:539:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  539 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:27:63: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   27 |     static bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
      |                                                               ^~~~~~~~
      |                                                               app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:38:17: error: redefinition of 'bool ScannerSettingsManager::parse_settings_content'
   38 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |                 ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:17: note: 'bool ScannerSettingsManager::parse_settings_content' previously defined here
   22 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                 ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:38:40: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
   38 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |                                        ^~~~~~~~~~~~~~~~~~~~~
      |                                        ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:539:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  539 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:38:63: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   38 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |                                                               ^~~~~~~~
      |                                                               app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:38:73: error: expected primary-expression before 'const'
   38 |     static bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |                                                                         ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:50:17: error: redefinition of 'bool ScannerSettingsManager::parse_key_value'
   50 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |                 ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:21:17: note: 'bool ScannerSettingsManager::parse_key_value' previously defined here
   21 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                 ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:50:33: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
   50 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |                                 ^~~~~~~~~~~~~~~~~~~~~
      |                                 ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:539:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  539 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:50:56: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   50 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |                                                        ^~~~~~~~
      |                                                        app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:50:66: error: expected primary-expression before 'const'
   50 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |                                                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:105:35: error: variable or field 'reset_to_defaults' declared void
  105 |     static void reset_to_defaults(DroneAnalyzerSettings& settings) {
      |                                   ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:105:35: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
  105 |     static void reset_to_defaults(DroneAnalyzerSettings& settings) {
      |                                   ^~~~~~~~~~~~~~~~~~~~~
      |                                   ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:539:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  539 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:105:58: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
  105 |     static void reset_to_defaults(DroneAnalyzerSettings& settings) {
      |                                                          ^~~~~~~~
      |                                                          app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:140:65: error: 'DroneAnalyzerSettings' has not been declared
  140 |     static bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings) {
      |                                                                 ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: In function 'bool ScannerSettingsManager::load_from_txt_impl(const string&, int&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:143:13: error: 'reset_to_defaults' was not declared in this scope
  143 |             reset_to_defaults(settings);
      |             ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:151:13: error: 'reset_to_defaults' was not declared in this scope
  151 |             reset_to_defaults(settings);
      |             ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:155:61: error: 'ScannerSettingsManager::parse_settings_content' cannot be used as a function
  155 |         return parse_settings_content(settings, file_content);
      |                                                             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:140:17: warning: 'bool ScannerSettingsManager::load_from_txt_impl(const string&, int&)' defined but not used [-Wunused-function]
  140 |     static bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings) {
      |                 ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:126:24: warning: 'std::string ScannerSettingsManager::trim_line(const string&)' defined but not used [-Wunused-function]
  126 |     static std::string trim_line(const std::string& line) {
      |                        ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:118:25: warning: 'SpectrumMode ScannerSettingsManager::parse_spectrum_mode(const string&)' defined but not used [-Wunused-function]
  118 |     static SpectrumMode parse_spectrum_mode(const std::string& value) {
      |                         ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:27:17: warning: 'ScannerSettingsManager::load_settings_from_txt' defined but not used [-Wunused-variable]
   27 |     static bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
      |                 ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:21:17: warning: 'ScannerSettingsManager::parse_key_value' defined but not used [-Wunused-variable]
   21 |     static bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                 ^~~~~~~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.