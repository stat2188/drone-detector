[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:412:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  412 |     FreqDBCache() : cache_mutex_{} {}
      |     ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:499:1: error: version control conflict marker in file
  499 | =======
      | ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:503:1: error: version control conflict marker in file
  503 | =======
      | ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:505:1: error: version control conflict marker in file
  505 | >>>>>>> ccac7db35a1842e525330580ab3eab7b714549a4
      | ^~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:16:1: error: expected unqualified-id before 'namespace'
   16 | namespace ScannerSettingsManager {
      | ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:40:1: error: expected unqualified-id before 'namespace'
   40 | namespace ScannerSettingsManager {
      | ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:32:7: error: expected nested-name-specifier before 'namespace'
   32 | using namespace portapack;
      |       ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:34:1: error: expected unqualified-id before 'namespace'
   34 | namespace ui::external_app::enhanced_drone_analyzer {
      | ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:56:1: error: expected unqualified-id before 'namespace'
   56 | namespace ui::external_app::enhanced_drone_analyzer_scanner {
      | ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:63:8: error: expected unqualified-id before string constant
   63 | extern "C" {
      |        ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:84:1: error: expected '}' at end of input
   84 | }
      | ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:496:31: note: to match this '{'
  496 | class BufferedDetectionLogger {
      |                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::BufferedDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:498:33: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'csv_log_'
  498 |     BufferedDetectionLogger() : csv_log_(), session_active_(false), session_start_(0), header_written_(false), last_flush_time_(0), logged_total_count_(0), entries_count_(0) {}
      |                                 ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:498:45: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'session_active_'
  498 |     BufferedDetectionLogger() : csv_log_(), session_active_(false), session_start_(0), header_written_(false), last_flush_time_(0), logged_total_count_(0), entries_count_(0) {}
      |                                             ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:498:69: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'session_start_'
  498 |     BufferedDetectionLogger() : csv_log_(), session_active_(false), session_start_(0), header_written_(false), last_flush_time_(0), logged_total_count_(0), entries_count_(0) {}
      |                                                                     ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:498:88: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'header_written_'
  498 |     BufferedDetectionLogger() : csv_log_(), session_active_(false), session_start_(0), header_written_(false), last_flush_time_(0), logged_total_count_(0), entries_count_(0) {}
      |                                                                                        ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:498:112: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'last_flush_time_'
  498 |     BufferedDetectionLogger() : csv_log_(), session_active_(false), session_start_(0), header_written_(false), last_flush_time_(0), logged_total_count_(0), entries_count_(0) {}
      |                                                                                                                ^~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:498:133: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'logged_total_count_'
  498 |     BufferedDetectionLogger() : csv_log_(), session_active_(false), session_start_(0), header_written_(false), last_flush_time_(0), logged_total_count_(0), entries_count_(0) {}
      |                                                                                                                                     ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:498:157: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'entries_count_'
  498 |     BufferedDetectionLogger() : csv_log_(), session_active_(false), session_start_(0), header_written_(false), last_flush_time_(0), logged_total_count_(0), entries_count_(0) {}
      |                                                                                                                                                             ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::log_detection(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:511:9: error: 'buffered_entries_' was not declared in this scope
  511 |         buffered_entries_[entries_count_] = entry;
      |         ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:511:27: error: 'entries_count_' was not declared in this scope
  511 |         buffered_entries_[entries_count_] = entry;
      |                           ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:515:40: error: 'chVTGetSystemTime' was not declared in this scope
  515 |         const systime_t current_time = chVTGetSystemTime();
      |                                        ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:517:29: error: 'last_flush_time_' was not declared in this scope
  517 |             (current_time - last_flush_time_) > MS2ST(LOG_BUFFER_FLUSH_MS)) {
      |                             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:523:13: error: 'entries_count_' was not declared in this scope
  523 |         if (entries_count_ == 0) return;
      |             ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:526:14: error: 'ensure_csv_header' was not declared in this scope
  526 |         if (!ensure_csv_header()) return;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:529:32: error: 'entries_count_' was not declared in this scope
  529 |         for (size_t i = 0; i < entries_count_; ++i) {
      |                                ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:530:43: error: 'buffered_entries_' was not declared in this scope
  530 |             batch_log += format_csv_entry(buffered_entries_[i]);
      |                                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:530:26: error: 'format_csv_entry' was not declared in this scope
  530 |             batch_log += format_csv_entry(buffered_entries_[i]);
      |                          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:533:22: error: 'csv_log_' was not declared in this scope
  533 |         auto error = csv_log_.append(generate_log_filename());
      |                      ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:533:38: error: 'generate_log_filename' was not declared in this scope
  533 |         auto error = csv_log_.append(generate_log_filename());
      |                                      ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:537:1: error: version control conflict marker in file
  537 | <<<<<<< HEAD
      | ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:795:31: error: local class 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneScanner' shall not have static data member 'constexpr const uint32_t ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneScanner::SCAN_THREAD_STACK_SIZE' [-fpermissive]
  795 |     static constexpr uint32_t SCAN_THREAD_STACK_SIZE = 4096;
      |                               ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:825:30: error: local class 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneScanner' shall not have static data member 'constexpr const uint8_t ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneScanner::DETECTION_DELAY' [-fpermissive]
  825 |     static constexpr uint8_t DETECTION_DELAY = 3;
      |                              ^~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1074:34: error: local class 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneDisplayController' shall not have static data member 'constexpr const char* const ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneDisplayController::DRONE_DISPLAY_FORMAT' [-fpermissive]
 1074 |     static constexpr const char* DRONE_DISPLAY_FORMAT = "%s %s %-4ddB %c";
      |                                  ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1177:29: error: local class 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::ScanningCoordinator' shall not have static data member 'constexpr const size_t ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::ScanningCoordinator::SCANNING_THREAD_STACK_SIZE' [-fpermissive]
 1177 |     static constexpr size_t SCANNING_THREAD_STACK_SIZE = 8192;  // CORRECTED: Increased stack size for safety (Critical Fix #3)
      |                             ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1261:1: error: version control conflict marker in file
 1261 | <<<<<<< HEAD
      | ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1276:1: error: version control conflict marker in file
 1276 | >>>>>>> ccac7db35a1842e525330580ab3eab7b714549a4
      | ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1333:29: error: local class 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::WidebandMedianFilter' shall not have static data member 'constexpr const size_t ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::WidebandMedianFilter::WINDOW_SIZE' [-fpermissive]
 1333 |     static constexpr size_t WINDOW_SIZE = 11;  // Optimal for embedded: 11 samples
      |                             ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1390:29: error: local class 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator' shall not have static data member 'constexpr const size_t ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator::TEST_ENTRIES' [-fpermissive]
 1390 |     static constexpr size_t TEST_ENTRIES = 16;
      |                             ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator::validate_buffered_logger_logic(ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1504:25: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'is_session_active'
 1504 |         if (test_logger.is_session_active()) {
      |                         ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1509:21: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'start_session'
 1509 |         test_logger.start_session();
      |                     ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1510:26: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'is_session_active'
 1510 |         if (!test_logger.is_session_active()) {
      |                          ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1532:21: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'end_session'
 1532 |         test_logger.end_session();
      |                     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1533:25: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'is_session_active'
 1533 |         if (test_logger.is_session_active()) {
      |                         ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1574:19: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'start_session'
 1574 |         log_cache.start_session();
      |                   ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1580:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1580 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1590:19: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'end_session'
 1590 |         log_cache.end_session();
      |                   ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1617:23: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'start_session'
 1617 |         buffer_logger.start_session();
      |                       ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1623:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1623 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1645:23: error: 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' has no member named 'end_session'
 1645 |         buffer_logger.end_session();
      |                       ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1668:1: error: expected unqualified-id at end of input
 1668 | } // namespace ui::external_app::enhanced_drone_analyzer
      | ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1668:1: error: expected '}' at end of input
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:308:53: note: to match this '{'
  308 | namespace ui::external_app::enhanced_drone_analyzer {
      |                                                     ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:969:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::SmartThreatHeader::paint(ui::Painter&)' used but never defined
  969 |     void paint(Painter& painter) override;
      |          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1006:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::ThreatCard::paint(ui::Painter&)' used but never defined
 1006 |     void paint(Painter& painter) override;
      |          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1032:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::ConsoleStatusBar::paint(ui::Painter&)' used but never defined
 1032 |     void paint(Painter& painter) override;
      |          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1038:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneDisplayController::~DroneDisplayController() noexcept' used but never defined
 1038 |     ~DroneDisplayController();
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1148:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::ScanningCoordinator::~ScanningCoordinator() noexcept' used but never defined
 1148 |     ~ScanningCoordinator();
      |     ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:703:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::DroneScanner::~DroneScanner() noexcept' used but never defined
  703 |     ~DroneScanner();
      |     ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1255:10: warning: 'virtual bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::EnhancedDroneSpectrumAnalyzerView::on_touch(ui::TouchEvent)' used but never defined
 1255 |     bool on_touch(const TouchEvent event) override;
      |          ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1254:10: warning: 'virtual bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::EnhancedDroneSpectrumAnalyzerView::on_key(ui::KeyEvent)' used but never defined
 1254 |     bool on_key(const KeyEvent key) override;
      |          ^~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1257:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::EnhancedDroneSpectrumAnalyzerView::on_hide()' used but never defined
 1257 |     void on_hide() override;
      |          ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1256:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::EnhancedDroneSpectrumAnalyzerView::on_show()' used but never defined
 1256 |     void on_show() override;
      |          ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1253:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::EnhancedDroneSpectrumAnalyzerView::paint(ui::Painter&)' used but never defined
 1253 |     void paint(Painter& painter) override;
      |          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1251:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::EnhancedDroneSpectrumAnalyzerView::focus()' used but never defined
 1251 |     void focus() override;
      |          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1319:10: warning: 'virtual void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()::LoadingScreenView::paint(ui::Painter&)' used but never defined
 1319 |     void paint(Painter& painter) override;
      |          ^~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.