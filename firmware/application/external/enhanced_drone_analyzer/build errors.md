<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 719ae38a77c1adee8dedf345f94060f76a428f43
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:463:6: error: expected ';' after class definition
  463 |     }
      |      ^
      |      ;
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::BufferedDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:428:33: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'last_flush_time_'
  428 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0) {}
      |                                 ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:428:54: error: class 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' does not have any field named 'entries_count_'
  428 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0) {}
      |                                                      ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::log_detection(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:433:9: error: 'buffered_entries_' was not declared in this scope
  433 |         buffered_entries_[entries_count_] = entry;
      |         ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:433:27: error: 'entries_count_' was not declared in this scope
  433 |         buffered_entries_[entries_count_] = entry;
      |                           ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:439:29: error: 'last_flush_time_' was not declared in this scope
  439 |             (current_time - last_flush_time_) > MS2ST(LOG_BUFFER_FLUSH_MS)) {
      |                             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::flush_buffer()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:445:13: error: 'entries_count_' was not declared in this scope
  445 |         if (entries_count_ == 0) return;
      |             ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:448:14: error: 'ensure_csv_header' was not declared in this scope
  448 |         if (!ensure_csv_header()) return;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:451:32: error: 'entries_count_' was not declared in this scope
  451 |         for (size_t i = 0; i < entries_count_; ++i) {
      |                                ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:452:43: error: 'buffered_entries_' was not declared in this scope
  452 |             batch_log += format_csv_entry(buffered_entries_[i]);
      |                                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:452:26: error: 'format_csv_entry' was not declared in this scope
  452 |             batch_log += format_csv_entry(buffered_entries_[i]);
      |                          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:455:22: error: 'csv_log_' was not declared in this scope
  455 |         auto error = csv_log_.append(generate_log_filename());
      |                      ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:455:38: error: 'generate_log_filename' was not declared in this scope
  455 |         auto error = csv_log_.append(generate_log_filename());
      |                                      ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:460:13: error: 'last_flush_time_' was not declared in this scope
  460 |             last_flush_time_ = chTimeNow();
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:461:13: error: 'entries_count_' was not declared in this scope
  461 |             entries_count_ = 0;  // Reset buffer count
      |             ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:465:30: error: non-member function 'bool ui::external_app::enhanced_drone_analyzer::is_session_active()' cannot have cv-qualifier
  465 |     bool is_session_active() const { return session_active_; }
      |                              ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In function 'bool ui::external_app::enhanced_drone_analyzer::is_session_active()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:465:45: error: 'session_active_' was not declared in this scope; did you mean 'is_session_active'?
  465 |     bool is_session_active() const { return session_active_; }
      |                                             ^~~~~~~~~~~~~~~
      |                                             is_session_active
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In function 'void ui::external_app::enhanced_drone_analyzer::start_session()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:468:13: error: 'session_active_' was not declared in this scope; did you mean 'is_session_active'?
  468 |         if (session_active_) return;
      |             ^~~~~~~~~~~~~~~
      |             is_session_active
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:469:9: error: 'session_active_' was not declared in this scope; did you mean 'is_session_active'?
  469 |         session_active_ = true;
      |         ^~~~~~~~~~~~~~~
      |         is_session_active
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:470:9: error: 'session_start_' was not declared in this scope
  470 |         session_start_ = chTimeNow();
      |         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:471:9: error: 'logged_total_count_' was not declared in this scope
  471 |         logged_total_count_ = 0;
      |         ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:472:9: error: 'header_written_' was not declared in this scope
  472 |         header_written_ = false;
      |         ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:473:9: error: 'last_flush_time_' was not declared in this scope
  473 |         last_flush_time_ = chTimeNow();
      |         ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In function 'void ui::external_app::enhanced_drone_analyzer::end_session()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:477:14: error: 'session_active_' was not declared in this scope; did you mean 'is_session_active'?
  477 |         if (!session_active_) return;
      |              ^~~~~~~~~~~~~~~
      |              is_session_active
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:478:9: error: 'flush_buffer' was not declared in this scope
  478 |         flush_buffer();  // Ensure all buffered entries are written
      |         ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:479:9: error: 'session_active_' was not declared in this scope; did you mean 'is_session_active'?
  479 |         session_active_ = false;
      |         ^~~~~~~~~~~~~~~
      |         is_session_active
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:482:1: error: expected unqualified-id before 'private'
  482 | private:
      | ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In function 'bool ui::external_app::enhanced_drone_analyzer::ensure_csv_header()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:497:22: error: 'csv_log_' was not declared in this scope
  497 |         auto error = csv_log_.append(generate_log_filename());
      |                      ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:497:38: error: 'generate_log_filename' was not declared in this scope
  497 |         auto error = csv_log_.append(generate_log_filename());
      |                                      ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In function 'std::string ui::external_app::enhanced_drone_analyzer::format_csv_entry(const DetectionLogEntry&)':
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:520:41: error: non-member function 'std::string ui::external_app::enhanced_drone_analyzer::generate_log_filename()' cannot have cv-qualifier
  520 |     std::string generate_log_filename() const {
      |                                         ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:528:5: error: 'DroneScanner' does not name a type
  528 |     DroneScanner* scanner_;  // Reference to parent scanner for callbacks
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:531:45: error: expected ')' before '*' token
  531 |     explicit DetectionProcessor(DroneScanner* scanner);
      |                                ~            ^
      |                                             )
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:579:29: error: 'DroneHardwareController' has not been declared
  579 |     void perform_scan_cycle(DroneHardwareController& hardware);
      |                             ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:608:38: error: 'DroneHardwareController' has not been declared
  608 |     void start_spectrum_for_scanning(DroneHardwareController& hardware);
      |                                      ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:609:37: error: 'DroneHardwareController' has not been declared
  609 |     void stop_spectrum_for_scanning(DroneHardwareController& hardware);
      |                                     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:610:44: error: 'DroneHardwareController' has not been declared
  610 |     void master_wideband_detection_handler(DroneHardwareController& hardware, Frequency frequency, int32_t rssi, bool is_approaching);
      |                                            ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:650:38: error: 'DroneHardwareController' has not been declared
  650 |     void perform_database_scan_cycle(DroneHardwareController& hardware);
      |                                      ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:651:38: error: 'DroneHardwareController' has not been declared
  651 |     void perform_wideband_scan_cycle(DroneHardwareController& hardware);
      |                                      ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:652:36: error: 'DroneHardwareController' has not been declared
  652 |     void perform_hybrid_scan_cycle(DroneHardwareController& hardware);
      |                                    ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:999:24: error: 'AudioManager' has not been declared
  999 |                        AudioManager& audio_controller);
      |                        ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1025:5: error: 'AudioManager' does not name a type
 1025 |     AudioManager& audio_controller_;
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1034:5: error: 'AudioManager' does not name a type
 1034 |     AudioManager& audio_mgr_;
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1043:5: error: 'AudioManager' has not been declared
 1043 |     AudioManager& audio_mgr);
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1095:21: error: 'AudioManager' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::AudioManager'?
 1095 |     std::unique_ptr<AudioManager> audio_mgr_;
      |                     ^~~~~~~~~~~~
      |                     ui::external_app::enhanced_drone_analyzer::AudioManager
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:274:7: note: 'ui::external_app::enhanced_drone_analyzer::AudioManager' declared here
  274 | class AudioManager;
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1095:33: error: template argument 1 is invalid
 1095 |     std::unique_ptr<AudioManager> audio_mgr_;
      |                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1095:33: error: template argument 2 is invalid
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool CacheLogicValidator::validate_freq_db_cache_logic(CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1251:9: error: 'FreqDBCache' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::FreqDBCache'?
 1251 |         FreqDBCache test_cache;
      |         ^~~~~~~~~~~
      |         ui::external_app::enhanced_drone_analyzer::FreqDBCache
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:340:7: note: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache' declared here
  340 | class FreqDBCache {
      |       ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1264:43: error: 'test_cache' was not declared in this scope
 1264 |         const freqman_entry* null_entry = test_cache.get_entry(0);
      |                                           ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool CacheLogicValidator::validate_buffered_logger_logic(CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1312:9: error: 'BufferedDetectionLogger' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger'?
 1312 |         BufferedDetectionLogger test_logger;
      |         ^~~~~~~~~~~~~~~~~~~~~~~
      |         ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:426:7: note: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' declared here
  426 | class BufferedDetectionLogger {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1315:13: error: 'test_logger' was not declared in this scope
 1315 |         if (test_logger.is_session_active()) {
      |             ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1320:9: error: 'test_logger' was not declared in this scope
 1320 |         test_logger.start_session();
      |         ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool CacheLogicValidator::validate_cache_integration_scenarios(CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1354:9: error: 'FreqDBCache' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::FreqDBCache'?
 1354 |         FreqDBCache freq_cache;
      |         ^~~~~~~~~~~
      |         ui::external_app::enhanced_drone_analyzer::FreqDBCache
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:340:7: note: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache' declared here
  340 | class FreqDBCache {
      |       ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1355:9: error: 'BufferedDetectionLogger' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger'?
 1355 |         BufferedDetectionLogger log_cache;
      |         ^~~~~~~~~~~~~~~~~~~~~~~
      |         ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:426:7: note: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' declared here
  426 | class BufferedDetectionLogger {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1367:21: error: 'freq_cache' was not declared in this scope
 1367 |                 if (freq_cache.get_entry(freq_idx) == nullptr) {
      |                     ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1376:50: error: 'freq_cache' was not declared in this scope
 1376 |                 const freqman_entry* retrieved = freq_cache.get_entry(freq_idx);
      |                                                  ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1385:9: error: 'log_cache' was not declared in this scope
 1385 |         log_cache.start_session();
      |         ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1391:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1391 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool CacheLogicValidator::validate_memory_management(CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1409:9: error: 'FreqDBCache' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::FreqDBCache'?
 1409 |         FreqDBCache size_cache;
      |         ^~~~~~~~~~~
      |         ui::external_app::enhanced_drone_analyzer::FreqDBCache
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:340:7: note: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache' declared here
  340 | class FreqDBCache {
      |       ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1417:13: error: 'size_cache' was not declared in this scope
 1417 |             size_cache.cache_entry(size_test_entry, i, "size_test.csv");
      |             ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1421:13: error: 'size_cache' was not declared in this scope
 1421 |         if (size_cache.size() > FREQ_DB_CACHE_SIZE + 5) { // Allow small margin for implementation
      |             ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1427:9: error: 'BufferedDetectionLogger' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger'?
 1427 |         BufferedDetectionLogger buffer_logger;
      |         ^~~~~~~~~~~~~~~~~~~~~~~
      |         ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:426:7: note: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger' declared here
  426 | class BufferedDetectionLogger {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1428:9: error: 'buffer_logger' was not declared in this scope
 1428 |         buffer_logger.start_session();
      |         ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1434:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1434 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1484:1: error: expected declaration before '}' token
 1484 | } // namespace ui::external_app::enhanced_drone_analyzer
      | ^
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp: In function 'void ui::external_app::enhanced_drone_analyzer::initialize_app(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:51:70: error: aggregate 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings loaded_settings' has incomplete type and cannot be defined
   51 |     ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings loaded_settings;
      |                                                                      ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:57:85: error: 'LoadingScreenView' is not a member of 'ui::external_app::enhanced_drone_analyzer'; did you mean 'LoadingScreenView'?
   57 |     auto loading_view = std::make_unique<ui::external_app::enhanced_drone_analyzer::LoadingScreenView>(nav);
      |                                                                                     ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1128:7: note: 'LoadingScreenView' declared here
 1128 | class LoadingScreenView : public View {
      |       ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:57:107: error: no matching function for call to 'make_unique<<expression error> >(ui::NavigationView&)'
   57 |     auto loading_view = std::make_unique<ui::external_app::enhanced_drone_analyzer::LoadingScreenView>(nav);
      |                                                                                                           ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./theme.hpp:26,
                 from /havoc/firmware/application/./ui_navigation.hpp:33,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)'
  848 |     make_unique(_Args&&... __args)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:57:107: error: template argument 1 is invalid
   57 |     auto loading_view = std::make_unique<ui::external_app::enhanced_drone_analyzer::LoadingScreenView>(nav);
      |                                                                                                           ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./theme.hpp:26,
                 from /havoc/firmware/application/./ui_navigation.hpp:33,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note: candidate: 'template<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)'
  854 |     make_unique(size_t __num)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:57:107: error: template argument 1 is invalid
   57 |     auto loading_view = std::make_unique<ui::external_app::enhanced_drone_analyzer::LoadingScreenView>(nav);
      |                                                                                                           ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./theme.hpp:26,
                 from /havoc/firmware/application/./ui_navigation.hpp:33,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)' <deleted>
  860 |     make_unique(_Args&&...) = delete;
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:57:107: error: template argument 1 is invalid
   57 |     auto loading_view = std::make_unique<ui::external_app::enhanced_drone_analyzer::LoadingScreenView>(nav);
      |                                                                                                           ^
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:65:32: error: no matching function for call to 'ui::NavigationView::replace(std::unique_ptr<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView, std::default_delete<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView> >::pointer)'
   65 |     nav.replace(main_view.get());
      |                                ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/havoc/firmware/application/./ui_navigation.hpp:115:8: note: candidate: 'template<class T, class ... Args> T* ui::NavigationView::replace(Args&& ...)'
  115 |     T* replace(Args&&... args) {
      |        ^~~~~~~
/havoc/firmware/application/./ui_navigation.hpp:115:8: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:65:32: note:   couldn't deduce template parameter 'T'
   65 |     nav.replace(main_view.get());
      |                                ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/havoc/firmware/application/./ui_navigation.hpp:122:10: note: candidate: 'void ui::NavigationView::replace(ui::View*)'
  122 |     void replace(View* v);
      |          ^~~~~~~
/havoc/firmware/application/./ui_navigation.hpp:122:24: note:   no known conversion for argument 1 from 'std::unique_ptr<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView, std::default_delete<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView> >::pointer' {aka 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView*'} to 'ui::View*'
  122 |     void replace(View* v);
      |                  ~~~~~~^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./theme.hpp:26,
                 from /havoc/firmware/application/./ui_navigation.hpp:33,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h: In instantiation of 'typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...) [with _Tp = ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView; _Args = {ui::NavigationView&}; typename std::_MakeUniq<_Tp>::__single_object = std::unique_ptr<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView, std::default_delete<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView> >]':
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:64:120:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:849:30: error: invalid use of incomplete type 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
  849 |     { return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...)); }
      |                              ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:278:7: note: forward declaration of 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
  278 | class EnhancedDroneSpectrumAnalyzerView;
      |       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./theme.hpp:26,
                 from /havoc/firmware/application/./ui_navigation.hpp:33,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h: In instantiation of 'void std::default_delete<_Tp>::operator()(_Tp*) const [with _Tp = ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView]':
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:284:17:   required from 'std::unique_ptr<_Tp, _Dp>::~unique_ptr() [with _Tp = ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView; _Dp = std::default_delete<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView>]'
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:64:120:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:79:16: error: invalid application of 'sizeof' to incomplete type 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
   79 |  static_assert(sizeof(_Tp)>0,
      |                ^~~~~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
<<<<<<< HEAD
=======
=======
[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:412:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  412 |     FreqDBCache() {}
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:412:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1547:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1547 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1590:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1590 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:412:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  412 |     FreqDBCache() {}
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:412:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1547:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1547 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1590:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1590 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:28:27: error: conflicting declaration 'constexpr const uint32_t MIN_DETECTION_COUNT'
   28 | static constexpr uint32_t MIN_DETECTION_COUNT = 3;
      |                           ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:187:26: note: previous declaration as 'constexpr const uint8_t MIN_DETECTION_COUNT'
  187 | static constexpr uint8_t MIN_DETECTION_COUNT = 3;
      |                          ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:29:26: error: redefinition of 'constexpr const int32_t HYSTERESIS_MARGIN_DB'
   29 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:188:26: note: 'constexpr const int32_t HYSTERESIS_MARGIN_DB' previously defined here
  188 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:30:15: error: conflicting declaration 'size_t DETECTION_TABLE_SIZE'
   30 | static size_t DETECTION_TABLE_SIZE = DETECTION_TABLE_SIZE_DEFAULT; // Made configurable
      |               ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:212:25: note: previous declaration as 'constexpr const size_t DETECTION_TABLE_SIZE'
  212 | static constexpr size_t DETECTION_TABLE_SIZE = 256;
      |                         ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:33:27: error: conflicting declaration 'constexpr const uint32_t FREQ_DB_CACHE_SIZE'
   33 | static constexpr uint32_t FREQ_DB_CACHE_SIZE = 32;
      |                           ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:195:25: note: previous declaration as 'constexpr const size_t FREQ_DB_CACHE_SIZE'
  195 | static constexpr size_t FREQ_DB_CACHE_SIZE = 32;  // Cache 32 most recently used entries
      |                         ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:34:28: error: redefinition of 'constexpr const systime_t FREQ_DB_CACHE_TIMEOUT_MS'
   34 | static constexpr systime_t FREQ_DB_CACHE_TIMEOUT_MS = 30000;  // 30 seconds
      |                            ^~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:196:27: note: 'constexpr const uint32_t FREQ_DB_CACHE_TIMEOUT_MS' previously defined here
  196 | static constexpr uint32_t FREQ_DB_CACHE_TIMEOUT_MS = 30000;  // 30 second cache lifetime
      |                           ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:37:25: error: redefinition of 'constexpr const size_t LOG_BUFFER_SIZE'
   37 | static constexpr size_t LOG_BUFFER_SIZE = 10;
      |                         ^~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:199:25: note: 'constexpr const size_t LOG_BUFFER_SIZE' previously defined here
  199 | static constexpr size_t LOG_BUFFER_SIZE = 64;     // Buffer 64 log entries before SD write
      |                         ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:38:28: error: redefinition of 'constexpr const systime_t LOG_BUFFER_FLUSH_MS'
   38 | static constexpr systime_t LOG_BUFFER_FLUSH_MS = 5000;  // 5 seconds
      |                            ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:200:27: note: 'constexpr const uint32_t LOG_BUFFER_FLUSH_MS' previously defined here
  200 | static constexpr uint32_t LOG_BUFFER_FLUSH_MS = 5000;  // Flush every 5 seconds
      |                           ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:41:28: error: conflicting declaration 'constexpr const Frequency WIDEBAND_DEFAULT_MIN'
   41 | static constexpr Frequency WIDEBAND_DEFAULT_MIN = 2400000000ULL;
      |                            ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:180:27: note: previous declaration as 'constexpr const uint32_t WIDEBAND_DEFAULT_MIN'
  180 | static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:42:28: error: conflicting declaration 'constexpr const Frequency WIDEBAND_DEFAULT_MAX'
   42 | static constexpr Frequency WIDEBAND_DEFAULT_MAX = 2500000000ULL;
      |                            ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:181:27: note: previous declaration as 'constexpr const uint32_t WIDEBAND_DEFAULT_MAX'
  181 | static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:43:27: error: redefinition of 'constexpr const uint32_t WIDEBAND_SLICE_WIDTH'
   43 | static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 50000000;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:182:27: note: 'constexpr const uint32_t WIDEBAND_SLICE_WIDTH' previously defined here
  182 | static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 25'000'000;
      |                           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:44:25: error: conflicting declaration 'constexpr const size_t WIDEBAND_MAX_SLICES'
   44 | static constexpr size_t WIDEBAND_MAX_SLICES = 20;
      |                         ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:183:27: note: previous declaration as 'constexpr const uint32_t WIDEBAND_MAX_SLICES'
  183 | static constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
      |                           ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:45:26: error: redefinition of 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB'
   45 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -70;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:185:26: note: 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB' previously defined here
  185 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:49:28: error: conflicting declaration 'constexpr const Frequency MIN_HARDWARE_FREQ'
   49 | static constexpr Frequency MIN_HARDWARE_FREQ = 50000000ULL;      // 50 MHz
      |                            ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:178:27: note: previous declaration as 'constexpr const uint32_t MIN_HARDWARE_FREQ'
  178 | static constexpr uint32_t MIN_HARDWARE_FREQ = 1'000'000;
      |                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:50:28: error: redefinition of 'constexpr const Frequency MAX_HARDWARE_FREQ'
   50 | static constexpr Frequency MAX_HARDWARE_FREQ = 6000000000ULL;    // 6 GHz
      |                            ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:179:27: note: 'constexpr const uint64_t MAX_HARDWARE_FREQ' previously defined here
  179 | static constexpr uint64_t MAX_HARDWARE_FREQ = 6'000'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:51:27: error: conflicting declaration 'constexpr const uint32_t DEFAULT_RSSI_THRESHOLD_DB'
   51 | static constexpr uint32_t DEFAULT_RSSI_THRESHOLD_DB = -80;
      |                           ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:184:26: note: previous declaration as 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB'
  184 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:65:37: error: 'DroneAnalyzerSettings' does not name a type; did you mean 'DroneAudioSettings'?
   65 | bool validate_loaded_settings(const DroneAnalyzerSettings& settings);
      |                                     ^~~~~~~~~~~~~~~~~~~~~
      |                                     DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:66:62: error: 'DroneAnalyzerSettings' has not been declared
   66 | bool parse_settings_from_content(const std::string& content, DroneAnalyzerSettings& settings);
      |                                                              ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:67:33: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings'?
   67 | bool load_settings_from_sd_card(DroneAnalyzerSettings& settings);
      |                                 ^~~~~~~~~~~~~~~~~~~~~
      |                                 ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:614:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  614 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:67:56: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   67 | bool load_settings_from_sd_card(DroneAnalyzerSettings& settings);
      |                                                        ^~~~~~~~
      |                                                        app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:86:8: error: redefinition of 'struct ui::external_app::enhanced_drone_analyzer::FreqDBCacheEntry'
   86 | struct FreqDBCacheEntry {
      |        ^~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:390:8: note: previous definition of 'struct ui::external_app::enhanced_drone_analyzer::FreqDBCacheEntry'
  390 | struct FreqDBCacheEntry {
      |        ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:104:7: error: redefinition of 'class ui::external_app::enhanced_drone_analyzer::FreqDBCache'
  104 | class FreqDBCache {
      |       ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:410:7: note: previous definition of 'class ui::external_app::enhanced_drone_analyzer::FreqDBCache'
  410 | class FreqDBCache {
      |       ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:189:7: error: redefinition of 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger'
  189 | class BufferedDetectionLogger {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:495:7: note: previous definition of 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger'
  495 | class BufferedDetectionLogger {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:295:1: error: 'chMtxObject' does not name a type
  295 | chMtxObject global_detection_ring_mutex;
      | ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void demonstrate_cache_functionality()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:362:5: error: 'demonstrate_cache_scenarios' was not declared in this scope; did you mean 'demonstrate_cache_functionality'?
  362 |     demonstrate_cache_scenarios();
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |     demonstrate_cache_functionality
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void demonstrate_cache_scenarios()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:372:25: error: cannot convert 'std::string' {aka 'std::__cxx11::basic_string<char>'} to 'char*'
  372 |     snprintf(test_entry.description, 16, "DRONE_TEST");
      |              ~~~~~~~~~~~^~~~~~~~~~~
      |                         |
      |                         std::string {aka std::__cxx11::basic_string<char>}
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/cstdio:42,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/ext/string_conversions.h:43,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/basic_string.h:6493,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:55,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/stdio.h:266:15: note:   initializing argument 1 of 'int snprintf(char*, size_t, const char*, ...)'
  266 | int snprintf (char *__restrict, size_t, const char *__restrict, ...)
      |               ^~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:387:21: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  387 |         {Timestamp::now(), 2400000000ULL, -75, ThreatLevel::HIGH, DroneType::MAVIC, 2, 0.85f},
      |                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:388:21: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  388 |         {Timestamp::now() + 100, 2400500000ULL, -80, ThreatLevel::MEDIUM, DroneType::PHANTOM, 1, 0.72f},
      |                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:389:21: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  389 |         {Timestamp::now() + 200, 2401000000ULL, -85, ThreatLevel::LOW, DroneType::UNKNOWN, 1, 0.68f}
      |                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::DetectionRingBuffer()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:419:51: error: type 'std::deque<ui::external_app::enhanced_drone_analyzer::DetectionEntry>' is not a direct base of 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer'
  419 | DetectionRingBuffer::DetectionRingBuffer() : std::deque<DetectionEntry>() {
      |                                                   ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:419:1: warning: 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::entries_' should be initialized in the member initialization list [-Weffc++]
  419 | DetectionRingBuffer::DetectionRingBuffer() : std::deque<DetectionEntry>() {
      | ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:419:1: warning: 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::ring_buffer_mutex_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::update_existing_entry(size_t, uint8_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:424:30: error: 'chVTGetSystemTime' was not declared in this scope
  424 |     systime_t current_time = chVTGetSystemTime();
      |                              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:426:28: error: 'begin' was not declared in this scope
  426 |     auto it = std::find_if(begin(), end(),
      |                            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:426:28: note: suggested alternatives:
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37: note:   'std::begin'
  105 |   template<typename _Tp> const _Tp* begin(const valarray<_Tp>&);
      |                                     ^~~~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:241:34: note:   'std::filesystem::begin'
  241 | inline const directory_iterator& begin(const directory_iterator& iter) noexcept {
      |                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:426:37: error: 'end' was not declared in this scope
  426 |     auto it = std::find_if(begin(), end(),
      |                                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:426:37: note: suggested alternatives:
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37: note:   'std::end'
  107 |   template<typename _Tp> const _Tp* end(const valarray<_Tp>&);
      |                                     ^~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:244:27: note:   'std::filesystem::end'
  244 | inline directory_iterator end(const directory_iterator&) noexcept {
      |                           ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::add_new_entry(size_t, uint8_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:442:30: error: 'chVTGetSystemTime' was not declared in this scope
  442 |     systime_t current_time = chVTGetSystemTime();
      |                              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:444:9: error: 'size' was not declared in this scope; did you mean 'std::size'?
  444 |     if (size() >= DETECTION_TABLE_SIZE) {
      |         ^~~~
      |         std::size
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:252:5: note: 'std::size' declared here
  252 |     size(const _Tp (&/*__array*/)[_Nm]) noexcept
      |     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:456:5: error: 'push_back' was not declared in this scope
  456 |     push_back(new_entry);
      |     ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::evict_least_recently_used()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:460:9: error: 'empty' was not declared in this scope; did you mean 'std::empty'?
  460 |     if (empty()) return;
      |         ^~~~~
      |         std::empty
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:280:5: note: 'std::empty' declared here
  280 |     empty(initializer_list<_Tp> __il) noexcept
      |     ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:462:36: error: 'begin' was not declared in this scope
  462 |     auto oldest = std::min_element(begin(), end(),
      |                                    ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:462:36: note: suggested alternatives:
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37: note:   'std::begin'
  105 |   template<typename _Tp> const _Tp* begin(const valarray<_Tp>&);
      |                                     ^~~~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:241:34: note:   'std::filesystem::begin'
  241 | inline const directory_iterator& begin(const directory_iterator& iter) noexcept {
      |                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:462:45: error: 'end' was not declared in this scope
  462 |     auto oldest = std::min_element(begin(), end(),
      |                                             ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:462:45: note: suggested alternatives:
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37: note:   'std::end'
  107 |   template<typename _Tp> const _Tp* end(const valarray<_Tp>&);
      |                                     ^~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:244:27: note:   'std::filesystem::end'
  244 | inline directory_iterator end(const directory_iterator&) noexcept {
      |                           ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:467:5: error: 'erase' was not declared in this scope
  467 |     erase(oldest);
      |     ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::find_entry_index(size_t) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:471:28: error: 'begin' was not declared in this scope
  471 |     auto it = std::find_if(begin(), end(),
      |                            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:471:28: note: suggested alternatives:
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37: note:   'std::begin'
  105 |   template<typename _Tp> const _Tp* begin(const valarray<_Tp>&);
      |                                     ^~~~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:241:34: note:   'std::filesystem::begin'
  241 | inline const directory_iterator& begin(const directory_iterator& iter) noexcept {
      |                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:471:37: error: 'end' was not declared in this scope
  471 |     auto it = std::find_if(begin(), end(),
      |                                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:471:37: note: suggested alternatives:
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37: note:   'std::end'
  107 |   template<typename _Tp> const _Tp* end(const valarray<_Tp>&);
      |                                     ^~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:244:27: note:   'std::filesystem::end'
  244 | inline directory_iterator end(const directory_iterator&) noexcept {
      |                           ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::remove_at_index(size_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:480:18: error: 'size' was not declared in this scope; did you mean 'std::size'?
  480 |     if (index >= size()) return;
      |                  ^~~~
      |                  std::size
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:252:5: note: 'std::size' declared here
  252 |     size(const _Tp (&/*__array*/)[_Nm]) noexcept
      |     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:482:15: error: 'begin' was not declared in this scope
  482 |     auto it = begin() + index;
      |               ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:482:15: note: suggested alternatives:
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37: note:   'std::begin'
  105 |   template<typename _Tp> const _Tp* begin(const valarray<_Tp>&);
      |                                     ^~~~~
In file included from /havoc/firmware/application/./app_settings.hpp:35,
                 from /havoc/firmware/application/./receiver_model.hpp:29,
                 from /havoc/firmware/application/./portapack.hpp:27,
                 from /havoc/firmware/application/./config_mode.hpp:27,
                 from /havoc/firmware/common/portapack_persistent_memory.hpp:36,
                 from /havoc/firmware/application/./rtc_time.hpp:29,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:241:34: note:   'std::filesystem::begin'
  241 | inline const directory_iterator& begin(const directory_iterator& iter) noexcept {
      |                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:483:5: error: 'erase' was not declared in this scope
  483 |     erase(it);
      |     ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::update_detection(size_t, uint8_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:495:36: error: too many arguments to function 'Mutex* chMtxUnlock()'
  495 |     chMtxUnlock(&ring_buffer_mutex_);
      |                                    ^
In file included from /havoc/firmware/chibios/os/kernel/include/ch.h:119,
                 from /havoc/firmware/chibios/os/hal/include/hal.h:39,
                 from /havoc/firmware/common/lpc43xx_cpp.hpp:27,
                 from /havoc/firmware/application/./rtc_time.hpp:28,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/chibios/os/kernel/include/chmtx.h:61:10: note: declared here
   61 |   Mutex *chMtxUnlock(void);
      |          ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'uint8_t ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::get_detection_count(size_t) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:499:15: error: invalid conversion from 'const Mutex*' to 'Mutex*' [-fpermissive]
  499 |     chMtxLock(&ring_buffer_mutex_);
      |               ^~~~~~~~~~~~~~~~~~~
      |               |
      |               const Mutex*
In file included from /havoc/firmware/chibios/os/kernel/include/ch.h:119,
                 from /havoc/firmware/chibios/os/hal/include/hal.h:39,
                 from /havoc/firmware/common/lpc43xx_cpp.hpp:27,
                 from /havoc/firmware/application/./rtc_time.hpp:28,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/chibios/os/kernel/include/chmtx.h:57:25: note:   initializing argument 1 of 'void chMtxLock(Mutex*)'
   57 |   void chMtxLock(Mutex *mp);
      |                  ~~~~~~~^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:501:51: error: no match for 'operator[]' (operand types are 'const ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer' and 'size_t' {aka 'unsigned int'})
  501 |     uint8_t result = (index != SIZE_MAX) ? (*this)[index].detection_count : 0;
      |                                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:502:36: error: too many arguments to function 'Mutex* chMtxUnlock()'
  502 |     chMtxUnlock(&ring_buffer_mutex_);
      |                                    ^
In file included from /havoc/firmware/chibios/os/kernel/include/ch.h:119,
                 from /havoc/firmware/chibios/os/hal/include/hal.h:39,
                 from /havoc/firmware/common/lpc43xx_cpp.hpp:27,
                 from /havoc/firmware/application/./rtc_time.hpp:28,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/chibios/os/kernel/include/chmtx.h:61:10: note: declared here
   61 |   Mutex *chMtxUnlock(void);
      |          ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'int32_t ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::get_rssi_value(size_t) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:507:15: error: invalid conversion from 'const Mutex*' to 'Mutex*' [-fpermissive]
  507 |     chMtxLock(&ring_buffer_mutex_);
      |               ^~~~~~~~~~~~~~~~~~~
      |               |
      |               const Mutex*
In file included from /havoc/firmware/chibios/os/kernel/include/ch.h:119,
                 from /havoc/firmware/chibios/os/hal/include/hal.h:39,
                 from /havoc/firmware/common/lpc43xx_cpp.hpp:27,
                 from /havoc/firmware/application/./rtc_time.hpp:28,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/chibios/os/kernel/include/chmtx.h:57:25: note:   initializing argument 1 of 'void chMtxLock(Mutex*)'
   57 |   void chMtxLock(Mutex *mp);
      |                  ~~~~~~~^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:509:51: error: no match for 'operator[]' (operand types are 'const ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer' and 'size_t' {aka 'unsigned int'})
  509 |     int32_t result = (index != SIZE_MAX) ? (*this)[index].rssi_value : -120;
      |                                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:510:36: error: too many arguments to function 'Mutex* chMtxUnlock()'
  510 |     chMtxUnlock(&ring_buffer_mutex_);
      |                                    ^
In file included from /havoc/firmware/chibios/os/kernel/include/ch.h:119,
                 from /havoc/firmware/chibios/os/hal/include/hal.h:39,
                 from /havoc/firmware/common/lpc43xx_cpp.hpp:27,
                 from /havoc/firmware/application/./rtc_time.hpp:28,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/chibios/os/kernel/include/chmtx.h:61:10: note: declared here
   61 |   Mutex *chMtxUnlock(void);
      |          ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:516:45: error: reference to 'global_detection_ring' is ambiguous
  516 | DetectionRingBuffer& local_detection_ring = global_detection_ring;
      |                                             ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:378:28: note: candidates are: 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer ui::external_app::enhanced_drone_analyzer::global_detection_ring'
  378 | extern DetectionRingBuffer global_detection_ring;
      |                            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:515:21: note:                 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer global_detection_ring'
  515 | DetectionRingBuffer global_detection_ring;
      |                     ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:769:10: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_' will be initialized after [-Wreorder]
  769 |     bool scanning_active_;
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:767:13: warning:   'Thread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_' [-Wreorder]
  767 |     Thread* scanning_thread_;
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:521:1: warning:   when initialized here [-Wreorder]
  521 | DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:799:22: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_' will be initialized after [-Wreorder]
  799 |     WidebandScanData wideband_scan_data_;
      |                      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:771:15: warning:   'FreqmanDB ui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_' [-Wreorder]
  771 |     FreqmanDB freq_db_;
      |               ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:521:1: warning:   when initialized here [-Wreorder]
  521 | DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:796:24: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::detection_processor_' will be initialized after [-Wreorder]
  796 |     DetectionProcessor detection_processor_;  // Unified detection processing
      |                        ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:792:14: warning:   'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_interval_ms_' [-Wreorder]
  792 |     uint32_t scan_interval_ms_;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:521:1: warning:   when initialized here [-Wreorder]
  521 | DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
      | ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:521:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:521:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::detection_logger_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In destructor 'ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:546:5: error: 'stop_scanning' was not declared in this scope
  546 |     stop_scanning();
      |     ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_memory_usage() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:560:50: error: passing 'const ui::external_app::enhanced_drone_analyzer::DroneScanner' as 'this' argument discards qualifiers [-fpermissive]
  560 |         handle_scan_error("Memory usage critical");
      |                                                  ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:695:10: note:   in call to 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::handle_scan_error(const char*)'
  695 |     void handle_scan_error(const char* error_msg);
      |          ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:646:6: error: no declaration matches 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::start_scanning()'
  646 | void DroneScanner::start_scanning() {
      |      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:646:6: note: no functions named 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::start_scanning()'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:650:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' defined here
  650 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:6: error: no declaration matches 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::stop_scanning()'
  681 | void DroneScanner::stop_scanning() {
      |      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:6: note: no functions named 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::stop_scanning()'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:650:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' defined here
  650 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::master_wideband_detection_handler(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, Frequency, int32_t, bool)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:728:41: error: 'WILDCARD' is not a member of 'freqman_type'
  728 |     wideband_entry.type = freqman_type::WILDCARD;
      |                                         ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:718:79: warning: unused parameter 'hardware' [-Wunused-parameter]
  718 | void DroneScanner::master_wideband_detection_handler(DroneHardwareController& hardware, Frequency frequency, int32_t rssi, bool is_approaching) {
      |                                                      ~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::set_scanning_mode(ui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:752:5: error: 'stop_scanning' was not declared in this scope
  752 |     stop_scanning();
      |     ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::process_rssi_detection(const freqman_entry&, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:903:41: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  903 |                 .timestamp = Timestamp::now(),
      |                                         ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:882:13: warning: unused variable 'prev_rssi' [-Wunused-variable]
  882 |     int32_t prev_rssi = local_detection_ring.get_rssi_value(freq_hash);
      |             ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_tracked_drone(DroneType, Frequency, int32_t, ThreatLevel)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:931:67: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  931 |             drone.add_rssi(static_cast<int16_t>(rssi), Timestamp::now());
      |                                                                   ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:941:67: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  941 |             drone.add_rssi(static_cast<int16_t>(rssi), Timestamp::now());
      |                                                                   ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:961:83: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  961 |     tracked_drones_[oldest_index].add_rssi(static_cast<int16_t>(rssi), Timestamp::now());
      |                                                                                   ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::remove_stale_drones()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:967:41: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  967 |     systime_t current_time = Timestamp::now();
      |                                         ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1033:8: error: redefinition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const'
 1033 | size_t DroneScanner::get_approaching_count() const {
      |        ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:704:12: note: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const' previously defined here
  704 |     size_t get_approaching_count() const { return approaching_count_; }
      |            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1037:8: error: redefinition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const'
 1037 | size_t DroneScanner::get_static_count() const {
      |        ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:706:12: note: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const' previously defined here
  706 |     size_t get_static_count() const { return static_count_; }
      |            ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1041:8: error: redefinition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const'
 1041 | size_t DroneScanner::get_receding_count() const {
      |        ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:705:12: note: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const' previously defined here
  705 |     size_t get_receding_count() const { return receding_count_; }
      |            ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::DroneDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1054:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
 1054 | DroneScanner::DroneDetectionLogger::DroneDetectionLogger()
      | ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1057:5: error: 'create_cache_directory' was not declared in this scope
 1057 |     create_cache_directory();
      |     ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1065:6: error: no declaration matches 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::create_cache_directory()'
 1065 | void DroneScanner::DroneDetectionLogger::create_cache_directory() {
      |      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1065:6: note: no functions named 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::create_cache_directory()'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:723:11: note: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger' defined here
  723 |     class DroneDetectionLogger {
      |           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::start_session()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1074:33: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1074 |     session_start_ = Timestamp::now();
      |                                 ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::ensure_csv_header()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1106:16: error: 'class Optional<std::filesystem::filesystem_error>' has no member named 'has_value'
 1106 |     if (!error.has_value()) return false;
      |                ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1108:15: error: 'class Optional<std::filesystem::filesystem_error>' has no member named 'has_value'
 1108 |     if (error.has_value()) {
      |               ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1119:24: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1119 |              "%lu,%lu,%d,%u,%u,%u,%.2f\n",
      |                       ~^
      |                        |
      |                        int
      |                       %ld
 1120 |              entry.timestamp, entry.frequency_hz, entry.rssi_db,
      |                                                   ~~~~~~~~~~~~~
      |                                                         |
      |                                                         int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::format_session_summary(size_t, size_t) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1132:47: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1132 |     uint32_t session_duration_ms = Timestamp::now() - session_start_;
      |                                               ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1: error: redefinition of 'ui::external_app::enhanced_drone_analyzer::DetectionProcessor::DetectionProcessor(ui::external_app::enhanced_drone_analyzer::DroneScanner*)'
 1152 | DetectionProcessor::DetectionProcessor(DroneScanner* scanner) : scanner_(scanner) {}
      | ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:600:14: note: 'ui::external_app::enhanced_drone_analyzer::DetectionProcessor::DetectionProcessor(ui::external_app::enhanced_drone_analyzer::DroneScanner*)' previously defined here
  600 |     explicit DetectionProcessor(DroneScanner* scanner) : scanner_(scanner) {}
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1154:6: error: redefinition of 'void ui::external_app::enhanced_drone_analyzer::DetectionProcessor::process_unified_detection(const freqman_entry&, int32_t, int32_t, float, bool)'
 1154 | void DetectionProcessor::process_unified_detection(const freqman_entry& entry, int32_t rssi, int32_t effective_threshold,
      |      ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:603:10: note: 'void ui::external_app::enhanced_drone_analyzer::DetectionProcessor::process_unified_detection(const freqman_entry&, int32_t, int32_t, float, bool)' previously defined here
  603 |     void process_unified_detection(const freqman_entry& entry, int32_t rssi, int32_t effective_threshold,
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::DroneHardwareController(SpectrumMode)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:902:13: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::last_valid_rssi_' will be initialized after [-Wreorder]
  902 |     int32_t last_valid_rssi_;
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:899:26: warning:   'ChannelSpectrumFIFO* ui::external_app::enhanced_drone_analyzer::DroneHardwareController::fifo_' [-Wreorder]
  899 |     ChannelSpectrumFIFO* fifo_;
      |                          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1182:1: warning:   when initialized here [-Wreorder]
 1182 | DroneHardwareController::DroneHardwareController(SpectrumMode mode)
      | ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1182:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_access_mutex_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1182:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::radio_state_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::tune_to_frequency(Frequency)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1259:10: error: 'wait_for_frequency_lock' was not declared in this scope
 1259 |     if (!wait_for_frequency_lock(200)) {  // 200ms timeout for frequency stability
      |          ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1286:6: error: no declaration matches 'bool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::wait_for_frequency_lock(systime_t)'
 1286 | bool DroneHardwareController::wait_for_frequency_lock(systime_t timeout_ms) {
      |      ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1286:6: note: no functions named 'bool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::wait_for_frequency_lock(systime_t)'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:817:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneHardwareController' defined here
  817 | class DroneHardwareController {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1315:9: error: no declaration matches 'int32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::read_raw_rssi_from_hardware() const'
 1315 | int32_t DroneHardwareController::read_raw_rssi_from_hardware() const {
      |         ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1315:9: note: no functions named 'int32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::read_raw_rssi_from_hardware() const'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:817:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneHardwareController' defined here
  817 | class DroneHardwareController {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'int32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::get_real_rssi_from_hardware(Frequency)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1335:14: error: 'wait_for_frequency_lock' was not declared in this scope
 1335 |         if (!wait_for_frequency_lock(100)) {  // 100ms timeout for frequency lock
      |              ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1341:29: error: 'read_raw_rssi_from_hardware' was not declared in this scope; did you mean 'get_real_rssi_from_hardware'?
 1341 |     int32_t measured_rssi = read_raw_rssi_from_hardware();
      |                             ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                             get_real_rssi_from_hardware
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1375:28: error: 'Mutex' {aka 'struct Mutex'} has no member named 'lock'
 1375 |     spectrum_access_mutex_.lock();
      |                            ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1392:28: error: 'Mutex' {aka 'struct Mutex'} has no member named 'unlock'
 1392 |     spectrum_access_mutex_.unlock();
      |                            ^~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::update(ThreatLevel, size_t, size_t, size_t, Frequency, bool)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1428:31: error: 'get_threat_text' was not declared in this scope; did you mean 'get_threat_icon_text'?
 1428 |     std::string threat_name = get_threat_text(max_threat);
      |                               ^~~~~~~~~~~~~~~
      |                               get_threat_icon_text
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1444:5: error: member 'ui::Style::font' is uninitialized reference
 1444 |     };
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1444:5: warning: missing initializer for member 'ui::Style::font' [-Wmissing-field-initializers]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1444:5: warning: missing initializer for member 'ui::Style::background' [-Wmissing-field-initializers]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1444:5: error: designator order for field 'ui::Style::background' does not match declaration order in 'ui::Style'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1467:66: error: taking address of rvalue [-fpermissive]
 1467 |     threat_frequency_.set_style(&get_threat_text_color(max_threat));
      |                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1467:33: error: cannot convert 'ui::Color*' to 'const ui::Style*'
 1467 |     threat_frequency_.set_style(&get_threat_text_color(max_threat));
      |                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                 |
      |                                 ui::Color*
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/common/ui_widget.hpp:119:33: note:   initializing argument 1 of 'void ui::Widget::set_style(const ui::Style*)'
  119 |     void set_style(const Style* new_style);
      |                    ~~~~~~~~~~~~~^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1523:13: error: no declaration matches 'std::string ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::get_threat_text(ThreatLevel) const'
 1523 | std::string SmartThreatHeader::get_threat_text(ThreatLevel level) const {
      |             ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1523:13: note: no functions named 'std::string ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::get_threat_text(ThreatLevel) const'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:915:7: note: 'class ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' defined here
  915 | class SmartThreatHeader : public View {
      |       ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1544:37: error: 'parent_rect_' was not declared in this scope; did you mean 'parent_rect'?
 1544 |         painter.fill_rectangle(Rect(parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 4), base_color);
      |                                     ^~~~~~~~~~~~
      |                                     parent_rect
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1541:17: warning: unused variable 'alpha' [-Wunused-variable]
 1541 |         uint8_t alpha = (pulse_timer % 20) < 10 ? 50 : 100;
      |                 ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1571:67: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1571 |     progress_text_.set_style(Style{Theme::getInstance()->fg_blue->foreground, Color::black(), Theme::getInstance()->fg_blue->font});
      |                                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1571:126: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_blue->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1571 |     progress_text_.set_style(Style{Theme::getInstance()->fg_blue->foreground, Color::black(), Theme::getInstance()->fg_blue->font});
      |                                                                                               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                                                                              |
      |                                                                                                                              const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1577:45: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1577 |         Style alert_style{(detections > 10) ? Theme::getInstance()->fg_red->foreground : Theme::getInstance()->fg_yellow->foreground,
      |                           ~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1578:75: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_yellow->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1578 |                          Color::black(), Theme::getInstance()->fg_yellow->font};
      |                                          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                           |
      |                                                                           const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_alert_status(ThreatLevel, size_t, const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1596:57: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1596 |     Style alert_style{(threat >= ThreatLevel::CRITICAL) ? Theme::getInstance()->fg_red->foreground : Theme::getInstance()->fg_yellow->foreground,
      |                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1597:71: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_yellow->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1597 |                      Color::black(), Theme::getInstance()->fg_yellow->font};
      |                                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                       |
      |                                                                       const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_normal_status(const string&, const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1612:66: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1612 |     normal_text_.set_style(Style{Theme::getInstance()->fg_light->foreground, Color::black(), Theme::getInstance()->fg_light->font});
      |                                  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1612:126: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_light->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1612 |     normal_text_.set_style(Style{Theme::getInstance()->fg_light->foreground, Color::black(), Theme::getInstance()->fg_light->font});
      |                                                                                              ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                                                                              |
      |                                                                                                                              const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1637:37: error: 'parent_rect_' was not declared in this scope; did you mean 'parent_rect'?
 1637 |         painter.fill_rectangle(Rect(parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2), Color(32, 0, 0));
      |                                     ^~~~~~~~~~~~
      |                                     parent_rect
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1707:1: error: expected unqualified-id before '/' token
 1707 | / CORRECTED: Fixed memory leaks using RAII members instead of chained unique_ptr initialization
      | ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_show()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1796:14: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController'
 1796 |     hardware_->on_hardware_show();
      |              ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_hide()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1801:14: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController'
 1801 |     hardware_->on_hardware_hide();
      |              ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1817:9: error: 'ui_controller_' was not declared in this scope; did you mean 'display_controller_'?
 1817 |         ui_controller_->on_stop_scan();
      |         ^~~~~~~~~~~~~~
      |         display_controller_
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1820:9: error: 'ui_controller_' was not declared in this scope; did you mean 'display_controller_'?
 1820 |         ui_controller_->on_start_scan();
      |         ^~~~~~~~~~~~~~
      |         display_controller_
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_menu_button()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1827:5: error: 'ui_controller_' was not declared in this scope; did you mean 'display_controller_'?
 1827 |     ui_controller_->show_menu();
      |     ^~~~~~~~~~~~~~
      |     display_controller_
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1833:56: error: cannot convert 'std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader>::__single_object' {aka 'std::unique_ptr<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader, std::default_delete<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader> >'} to 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader*' in assignment
 1833 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                        |
      |                                                        std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader>::__single_object {aka std::unique_ptr<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader, std::default_delete<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader> >}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1834:53: error: cannot convert 'std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar>::__single_object' {aka 'std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar, std::default_delete<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar> >'} to 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar*' in assignment
 1834 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                     |
      |                                                     std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar>::__single_object {aka std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar, std::default_delete<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar> >}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1839:68: warning: narrowing conversion of 'card_y_pos' from 'size_t' {aka 'unsigned int'} to 'int' [-Wnarrowing]
 1839 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                    ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1839:56: error: cannot convert 'std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ThreatCard>::__single_object' {aka 'std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ThreatCard, std::default_delete<ui::external_app::enhanced_drone_analyzer::ThreatCard> >'} to 'std::array<ui::external_app::enhanced_drone_analyzer::ThreatCard*, 3>::value_type' {aka 'ui::external_app::enhanced_drone_analyzer::ThreatCard*'} in assignment
 1839 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                        |
      |                                                        std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ThreatCard>::__single_object {aka std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ThreatCard, std::default_delete<ui::external_app::enhanced_drone_analyzer::ThreatCard> >}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1853:9: error: no match for 'operator!' (operand type is 'ui::external_app::enhanced_drone_analyzer::DroneScanner')
 1853 |     if (!scanner_) return;
      |         ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1853:9: note: candidate: 'operator!(bool)' <built-in>
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1853:9: note:   no known conversion for argument 1 from 'ui::external_app::enhanced_drone_analyzer::DroneScanner' to 'bool'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1856:38: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1856 |     ThreatLevel max_threat = scanner_->get_max_detected_threat();
      |                                      ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1857:34: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1857 |     size_t approaching = scanner_->get_approaching_count();
      |                                  ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1858:35: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1858 |     size_t static_count = scanner_->get_static_count();
      |                                   ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1859:31: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1859 |     size_t receding = scanner_->get_receding_count();
      |                               ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1860:32: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1860 |     bool is_scanning = scanner_->is_scanning_active();
      |                                ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1861:38: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1861 |     Frequency current_freq = scanner_->get_current_scanning_frequency();
      |                                      ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1862:41: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1862 |     uint32_t total_detections = scanner_->get_total_detections();
      |                                         ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1871:35: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1871 |         uint32_t cycles = scanner_->get_scan_cycles();
      |                                   ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1872:55: error: no matching function for call to 'min(long unsigned int, unsigned int)'
 1872 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
      |                                                       ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)'
  198 |     min(const _Tp& __a, const _Tp& __b)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1872:55: note:   deduced conflicting types for parameter 'const _Tp' ('long unsigned int' and 'unsigned int')
 1872 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
      |                                                       ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40,
                 from /havoc/firmware/common/ui_text.hpp:27,
                 from /havoc/firmware/common/ui_widget.hpp:29,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)'
  246 |     min(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1872:55: note:   deduced conflicting types for parameter 'const _Tp' ('long unsigned int' and 'unsigned int')
 1872 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
      |                                                       ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/functional:65,
                 from /havoc/firmware/application/./signal.hpp:27,
                 from /havoc/firmware/application/./rtc_time.hpp:26,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5: note: candidate: 'template<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)'
 3444 |     min(initializer_list<_Tp> __l)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1872:55: note:   mismatched types 'std::initializer_list<_Tp>' and 'long unsigned int'
 1872 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
      |                                                       ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/functional:65,
                 from /havoc/firmware/application/./signal.hpp:27,
                 from /havoc/firmware/application/./rtc_time.hpp:26,
                 from /havoc/firmware/common/ui_widget.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)'
 3450 |     min(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1872:55: note:   mismatched types 'std::initializer_list<_Tp>' and 'long unsigned int'
 1872 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
      |                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1884:60: error: 'MAX_DISPLAYED_DRONES' is not a member of 'DisplayDroneEntry'
 1884 |     for (size_t i = 0; i < std::min(3u, DisplayDroneEntry::MAX_DISPLAYED_DRONES); ++i) {
      |                                                            ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1885:37: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1885 |         const auto& drone = scanner_->getTrackedDrone(i);
      |                                     ^~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'DisplayDroneEntry::DisplayDroneEntry()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::frequency' should be initialized in the member initialization list [-Weffc++]
  151 | struct DisplayDroneEntry {
      |        ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::type' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::threat' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::rssi' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::last_seen' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::type_name' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::display_color' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:151:8: warning: 'DisplayDroneEntry::trend' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1888:31: note: synthesized method 'DisplayDroneEntry::DisplayDroneEntry()' first required here
 1888 |             DisplayDroneEntry entry;
      |                               ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1893:42: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1893 |             entry.last_seen = Timestamp::now();
      |                                          ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1906:53: error: 'MAX_DISPLAYED_DRONES' is not a member of 'DisplayDroneEntry'
 1906 |     for (size_t i = std::min(3u, DisplayDroneEntry::MAX_DISPLAYED_DRONES); i < 3; ++i) {
      |                                                     ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::LoadingScreenView::LoadingScreenView(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1916:31: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1916 |       timer_start_(Timestamp::now())
      |                               ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1923:1: error: definition of explicitly-defaulted 'ui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()'
 1923 | LoadingScreenView::~LoadingScreenView() {
      | ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1284:5: note: 'virtual ui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()' explicitly defaulted here
 1284 |     ~LoadingScreenView() = default;
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1937:1: error: reference to 'AudioManager' is ambiguous
 1937 | AudioManager::AudioManager() : audio_enabled_(true) {}
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:321:7: note: candidates are: 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  321 | class AudioManager {
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:6:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:10:7: note:                 'class AudioManager'
   10 | class AudioManager {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1938:1: error: reference to 'AudioManager' is ambiguous
 1938 | AudioManager::~AudioManager() { stop_audio(); }
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:321:7: note: candidates are: 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  321 | class AudioManager {
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:6:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:10:7: note:                 'class AudioManager'
   10 | class AudioManager {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1940:6: error: reference to 'AudioManager' is ambiguous
 1940 | void AudioManager::play_detection_beep(ThreatLevel level) {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:321:7: note: candidates are: 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  321 | class AudioManager {
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:6:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:10:7: note:                 'class AudioManager'
   10 | class AudioManager {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void play_detection_beep(ThreatLevel)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1941:10: error: 'audio_enabled_' was not declared in this scope
 1941 |     if (!audio_enabled_) return;
      |          ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1960:6: error: reference to 'AudioManager' is ambiguous
 1960 | void AudioManager::stop_audio() {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:321:7: note: candidates are: 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  321 | class AudioManager {
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:6:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:10:7: note:                 'class AudioManager'
   10 | class AudioManager {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1965:6: error: reference to 'AudioManager' is ambiguous
 1965 | void AudioManager::toggle_audio() {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:321:7: note: candidates are: 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  321 | class AudioManager {
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:6:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:10:7: note:                 'class AudioManager'
   10 | class AudioManager {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void toggle_audio()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1966:5: error: 'audio_enabled_' was not declared in this scope
 1966 |     audio_enabled_ = !audio_enabled_;
      |     ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1969:6: error: reference to 'AudioManager' is ambiguous
 1969 | bool AudioManager::is_audio_enabled() const {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:321:7: note: candidates are: 'class ui::external_app::enhanced_drone_analyzer::AudioManager'
  321 | class AudioManager {
      |       ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:6:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:10:7: note:                 'class AudioManager'
   10 | class AudioManager {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1969:39: error: non-member function 'bool is_audio_enabled()' cannot have cv-qualifier
 1969 | bool AudioManager::is_audio_enabled() const {
      |                                       ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'bool is_audio_enabled()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1970:12: error: 'audio_enabled_' was not declared in this scope; did you mean 'is_audio_enabled'?
 1970 |     return audio_enabled_;
      |            ^~~~~~~~~~~~~~
      |            is_audio_enabled
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::ScanningCoordinator(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, ui::external_app::enhanced_drone_analyzer::DroneDisplayController&, ui::external_app::enhanced_drone_analyzer::AudioManager&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2004:1: warning: 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::scan_coordinator_mutex_' should be initialized in the member initialization list [-Weffc++]
 2004 | ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
      | ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'msg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::coordinated_scanning_thread()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2056:22: error: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' has no member named 'is_scanning_active'; did you mean 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_'? (not accessible from this context)
 2056 |         if (scanner_.is_scanning_active()) {
      |                      ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:769:10: note: declared private here
  769 |     bool scanning_active_;
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2086:6: error: redefinition of 'bool ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::is_scanning_active() const'
 2086 | bool ScanningCoordinator::is_scanning_active() const {
      |      ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1128:10: note: 'bool ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::is_scanning_active() const' previously defined here
 1128 |     bool is_scanning_active() const { return scanning_active_; }
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'bool ScannerSettingsManager::parse_key_value(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&, const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2131:41: error: 'validate_range' was not declared in this scope
 2131 |             settings.scan_interval_ms = validate_range<uint32_t>(
      |                                         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2131:64: error: expected primary-expression before '>' token
 2131 |             settings.scan_interval_ms = validate_range<uint32_t>(
      |                                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2134:44: warning: right operand of comma operator has no effect [-Wunused-value]
 2134 |                 static_cast<uint32_t>(5000U));
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2137:42: error: 'validate_range' was not declared in this scope
 2137 |             settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
      |                                          ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2137:64: error: expected primary-expression before '>' token
 2137 |             settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
      |                                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2137:91: warning: right operand of comma operator has no effect [-Wunused-value]
 2137 |             settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
      |                                                                                           ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2143:49: error: 'validate_range' was not declared in this scope
 2143 |             settings.audio_alert_frequency_hz = validate_range<uint16_t>(
      |                                                 ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2143:72: error: expected primary-expression before '>' token
 2143 |             settings.audio_alert_frequency_hz = validate_range<uint16_t>(
      |                                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2144:17: warning: value computed is not used [-Wunused-value]
 2144 |                 static_cast<uint16_t>(std::stoul(value)),
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2146:44: warning: right operand of comma operator has no effect [-Wunused-value]
 2146 |                 static_cast<uint16_t>(3000U));
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2149:48: error: 'validate_range' was not declared in this scope
 2149 |             settings.audio_alert_duration_ms = validate_range<uint32_t>(
      |                                                ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2149:71: error: expected primary-expression before '>' token
 2149 |             settings.audio_alert_duration_ms = validate_range<uint32_t>(
      |                                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2152:44: warning: right operand of comma operator has no effect [-Wunused-value]
 2152 |                 static_cast<uint32_t>(2000U));
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2155:46: error: 'validate_range' was not declared in this scope
 2155 |             settings.hardware_bandwidth_hz = validate_range<uint32_t>(
      |                                              ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2155:69: error: expected primary-expression before '>' token
 2155 |             settings.hardware_bandwidth_hz = validate_range<uint32_t>(
      |                                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2158:49: warning: right operand of comma operator has no effect [-Wunused-value]
 2158 |                 static_cast<uint32_t>(100000000U));
      |                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2348:64: error: 'bool load_settings_from_sd_card(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)' redeclared as different kind of entity
 2348 | bool load_settings_from_sd_card(DroneAnalyzerSettings& settings) {
      |                                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:67:6: note: previous declaration 'bool load_settings_from_sd_card'
   67 | bool load_settings_from_sd_card(DroneAnalyzerSettings& settings);
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void ui::external_app::enhanced_drone_analyzer_scanner::initialize_app(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2385:48: error: 'initialize_app' is not a member of 'ui::external_app::enhanced_drone_analyzer'
 2385 |     ui::external_app::enhanced_drone_analyzer::initialize_app(nav);
      |                                                ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2385:48: note: suggested alternatives:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2227:6: note:   'initialize_app'
 2227 | void initialize_app(ui::NavigationView& nav) {
      |      ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2383:6: note:   'ui::external_app::enhanced_drone_analyzer_scanner::initialize_app'
 2383 | void initialize_app(ui::NavigationView& nav) {
      |      ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2412:1: error: expected declaration before '}' token
 2412 | } // namespace ui::external_app::enhanced_drone_analyzer
      | ^
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5775: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
>>>>>>> ccac7db35a1842e525330580ab3eab7b714549a4
>>>>>>> 719ae38a77c1adee8dedf345f94060f76a428f43
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.