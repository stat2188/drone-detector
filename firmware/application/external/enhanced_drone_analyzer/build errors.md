[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/adult_toys_controller/ui_adult_toys_controller.cpp.obj
[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:405:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  405 |     FreqDBCache() = default;
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:405:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_freq_db_cache_logic(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1380:21: note: synthesized method 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()' first required here
 1380 |         FreqDBCache test_cache;
      |                     ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1520:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1520 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1563:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1563 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::BufferedDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:551:12: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::entries_count_' will be initialized after [-Wreorder]
  551 |     size_t entries_count_;
      |            ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:546:10: warning:   'bool ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::session_active_' [-Wreorder]
  546 |     bool session_active_;
      |          ^~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:490:5: warning:   when initialized here [-Wreorder]
  490 |     BufferedDetectionLogger() : last_flush_time_(0), entries_count_(0), session_active_(false), session_start_(0), header_written_(false), logged_total_count_(0) {}
      |     ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:490:5: warning: 'ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:405:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_entries_' should be initialized in the member initialization list [-Weffc++]
  405 |     FreqDBCache() = default;
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:405:5: warning: 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::cache_mutex_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_freq_db_cache_logic(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1380:21: note: synthesized method 'ui::external_app::enhanced_drone_analyzer::FreqDBCache::FreqDBCache()' first required here
 1380 |         FreqDBCache test_cache;
      |                     ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_cache_integration_scenarios(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1520:47: warning: narrowing conversion of '(2400000000 + (((long long unsigned int)(detection % 100)) * 1000000))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1520 |                 .frequency_hz = 2400000000ULL + (detection % 100) * 1000000ULL,
      |                                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::validate_memory_management(ui::external_app::enhanced_drone_analyzer::CacheLogicValidator::TestResult&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1563:47: warning: narrowing conversion of '(2400000000 + ((long long unsigned int)i))' from 'long long unsigned int' to 'uint32_t' {aka 'long unsigned int'} [-Wnarrowing]
 1563 |                 .frequency_hz = 2400000000ULL + i,
      |                                 ~~~~~~~~~~~~~~^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:28:27: error: conflicting declaration 'constexpr const uint32_t MIN_DETECTION_COUNT'
   28 | static constexpr uint32_t MIN_DETECTION_COUNT = 3;
      |                           ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:180:26: note: previous declaration as 'constexpr const uint8_t MIN_DETECTION_COUNT'
  180 | static constexpr uint8_t MIN_DETECTION_COUNT = 3;
      |                          ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:29:26: error: redefinition of 'constexpr const int32_t HYSTERESIS_MARGIN_DB'
   29 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:181:26: note: 'constexpr const int32_t HYSTERESIS_MARGIN_DB' previously defined here
  181 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:30:15: error: conflicting declaration 'size_t DETECTION_TABLE_SIZE'
   30 | static size_t DETECTION_TABLE_SIZE = DETECTION_TABLE_SIZE_DEFAULT; // Made configurable
      |               ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:205:25: note: previous declaration as 'constexpr const size_t DETECTION_TABLE_SIZE'
  205 | static constexpr size_t DETECTION_TABLE_SIZE = 256;
      |                         ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:33:27: error: conflicting declaration 'constexpr const uint32_t FREQ_DB_CACHE_SIZE'
   33 | static constexpr uint32_t FREQ_DB_CACHE_SIZE = 32;
      |                           ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:188:25: note: previous declaration as 'constexpr const size_t FREQ_DB_CACHE_SIZE'
  188 | static constexpr size_t FREQ_DB_CACHE_SIZE = 32;  // Cache 32 most recently used entries
      |                         ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:34:28: error: redefinition of 'constexpr const systime_t FREQ_DB_CACHE_TIMEOUT_MS'
   34 | static constexpr systime_t FREQ_DB_CACHE_TIMEOUT_MS = 30000;  // 30 seconds
      |                            ^~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:189:27: note: 'constexpr const uint32_t FREQ_DB_CACHE_TIMEOUT_MS' previously defined here
  189 | static constexpr uint32_t FREQ_DB_CACHE_TIMEOUT_MS = 30000;  // 30 second cache lifetime
      |                           ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:37:25: error: redefinition of 'constexpr const size_t LOG_BUFFER_SIZE'
   37 | static constexpr size_t LOG_BUFFER_SIZE = 10;
      |                         ^~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:25: note: 'constexpr const size_t LOG_BUFFER_SIZE' previously defined here
  192 | static constexpr size_t LOG_BUFFER_SIZE = 64;     // Buffer 64 log entries before SD write
      |                         ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:38:28: error: redefinition of 'constexpr const systime_t LOG_BUFFER_FLUSH_MS'
   38 | static constexpr systime_t LOG_BUFFER_FLUSH_MS = 5000;  // 5 seconds
      |                            ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:193:27: note: 'constexpr const uint32_t LOG_BUFFER_FLUSH_MS' previously defined here
  193 | static constexpr uint32_t LOG_BUFFER_FLUSH_MS = 5000;  // Flush every 5 seconds
      |                           ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:41:28: error: conflicting declaration 'constexpr const Frequency WIDEBAND_DEFAULT_MIN'
   41 | static constexpr Frequency WIDEBAND_DEFAULT_MIN = 2400000000ULL;
      |                            ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:27: note: previous declaration as 'constexpr const uint32_t WIDEBAND_DEFAULT_MIN'
  173 | static constexpr uint32_t WIDEBAND_DEFAULT_MIN = 2'400'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:42:28: error: conflicting declaration 'constexpr const Frequency WIDEBAND_DEFAULT_MAX'
   42 | static constexpr Frequency WIDEBAND_DEFAULT_MAX = 2500000000ULL;
      |                            ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:174:27: note: previous declaration as 'constexpr const uint32_t WIDEBAND_DEFAULT_MAX'
  174 | static constexpr uint32_t WIDEBAND_DEFAULT_MAX = 2'500'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:43:27: error: redefinition of 'constexpr const uint32_t WIDEBAND_SLICE_WIDTH'
   43 | static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 50000000;
      |                           ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:175:27: note: 'constexpr const uint32_t WIDEBAND_SLICE_WIDTH' previously defined here
  175 | static constexpr uint32_t WIDEBAND_SLICE_WIDTH = 25'000'000;
      |                           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:44:25: error: conflicting declaration 'constexpr const size_t WIDEBAND_MAX_SLICES'
   44 | static constexpr size_t WIDEBAND_MAX_SLICES = 20;
      |                         ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:176:27: note: previous declaration as 'constexpr const uint32_t WIDEBAND_MAX_SLICES'
  176 | static constexpr uint32_t WIDEBAND_MAX_SLICES = 20;
      |                           ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:45:26: error: redefinition of 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB'
   45 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -70;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:178:26: note: 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB' previously defined here
  178 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:49:28: error: conflicting declaration 'constexpr const Frequency MIN_HARDWARE_FREQ'
   49 | static constexpr Frequency MIN_HARDWARE_FREQ = 50000000ULL;      // 50 MHz
      |                            ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:171:27: note: previous declaration as 'constexpr const uint32_t MIN_HARDWARE_FREQ'
  171 | static constexpr uint32_t MIN_HARDWARE_FREQ = 1'000'000;
      |                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:50:28: error: redefinition of 'constexpr const Frequency MAX_HARDWARE_FREQ'
   50 | static constexpr Frequency MAX_HARDWARE_FREQ = 6000000000ULL;    // 6 GHz
      |                            ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:172:27: note: 'constexpr const uint64_t MAX_HARDWARE_FREQ' previously defined here
  172 | static constexpr uint64_t MAX_HARDWARE_FREQ = 6'000'000'000ULL;
      |                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:51:27: error: conflicting declaration 'constexpr const uint32_t DEFAULT_RSSI_THRESHOLD_DB'
   51 | static constexpr uint32_t DEFAULT_RSSI_THRESHOLD_DB = -80;
      |                           ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:177:26: note: previous declaration as 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB'
  177 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:607:8: note: 'ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' declared here
  607 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:67:56: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   67 | bool load_settings_from_sd_card(DroneAnalyzerSettings& settings);
      |                                                        ^~~~~~~~
      |                                                        app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:86:8: error: redefinition of 'struct ui::external_app::enhanced_drone_analyzer::FreqDBCacheEntry'
   86 | struct FreqDBCacheEntry {
      |        ^~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:8: note: previous definition of 'struct ui::external_app::enhanced_drone_analyzer::FreqDBCacheEntry'
  383 | struct FreqDBCacheEntry {
      |        ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:104:7: error: redefinition of 'class ui::external_app::enhanced_drone_analyzer::FreqDBCache'
  104 | class FreqDBCache {
      |       ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:403:7: note: previous definition of 'class ui::external_app::enhanced_drone_analyzer::FreqDBCache'
  403 | class FreqDBCache {
      |       ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:189:7: error: redefinition of 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger'
  189 | class BufferedDetectionLogger {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:488:7: note: previous definition of 'class ui::external_app::enhanced_drone_analyzer::BufferedDetectionLogger'
  488 | class BufferedDetectionLogger {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:293:1: error: 'chMtxObject' does not name a type
  293 | chMtxObject global_detection_ring_mutex;
      | ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void ui::external_app::enhanced_drone_analyzer::demonstrate_cache_functionality()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:360:5: error: 'demonstrate_cache_scenarios' was not declared in this scope; did you mean 'demonstrate_cache_functionality'?
  360 |     demonstrate_cache_scenarios();
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |     demonstrate_cache_functionality
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void ui::external_app::enhanced_drone_analyzer::demonstrate_cache_scenarios()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:370:25: error: cannot convert 'std::string' {aka 'std::__cxx11::basic_string<char>'} to 'char*'
  370 |     snprintf(test_entry.description, 16, "DRONE_TEST");
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:385:21: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  385 |         {Timestamp::now(), 2400000000ULL, -75, ThreatLevel::HIGH, DroneType::MAVIC, 2, 0.85f},
      |                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:386:21: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  386 |         {Timestamp::now() + 100, 2400500000ULL, -80, ThreatLevel::MEDIUM, DroneType::PHANTOM, 1, 0.72f},
      |                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:387:21: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  387 |         {Timestamp::now() + 200, 2401000000ULL, -85, ThreatLevel::LOW, DroneType::UNKNOWN, 1, 0.68f}
      |                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::DetectionRingBuffer()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:417:51: error: type 'std::deque<ui::external_app::enhanced_drone_analyzer::DetectionEntry>' is not a direct base of 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer'
  417 | DetectionRingBuffer::DetectionRingBuffer() : std::deque<DetectionEntry>() {
      |                                                   ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:417:1: warning: 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::entries_' should be initialized in the member initialization list [-Weffc++]
  417 | DetectionRingBuffer::DetectionRingBuffer() : std::deque<DetectionEntry>() {
      | ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:417:1: warning: 'ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::ring_buffer_mutex_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::update_existing_entry(size_t, uint8_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:422:30: error: 'chVTGetSystemTime' was not declared in this scope
  422 |     systime_t current_time = chVTGetSystemTime();
      |                              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:424:28: error: 'begin' was not declared in this scope
  424 |     auto it = std::find_if(begin(), end(),
      |                            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:424:28: note: suggested alternatives:
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:424:37: error: 'end' was not declared in this scope
  424 |     auto it = std::find_if(begin(), end(),
      |                                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:424:37: note: suggested alternatives:
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:440:30: error: 'chVTGetSystemTime' was not declared in this scope
  440 |     systime_t current_time = chVTGetSystemTime();
      |                              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:442:9: error: 'size' was not declared in this scope; did you mean 'std::size'?
  442 |     if (size() >= DETECTION_TABLE_SIZE) {
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:454:5: error: 'push_back' was not declared in this scope
  454 |     push_back(new_entry);
      |     ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::evict_least_recently_used()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:458:9: error: 'empty' was not declared in this scope; did you mean 'std::empty'?
  458 |     if (empty()) return;
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:460:36: error: 'begin' was not declared in this scope
  460 |     auto oldest = std::min_element(begin(), end(),
      |                                    ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:460:36: note: suggested alternatives:
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:460:45: error: 'end' was not declared in this scope
  460 |     auto oldest = std::min_element(begin(), end(),
      |                                             ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:460:45: note: suggested alternatives:
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:465:5: error: 'erase' was not declared in this scope
  465 |     erase(oldest);
      |     ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::find_entry_index(size_t) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:469:28: error: 'begin' was not declared in this scope
  469 |     auto it = std::find_if(begin(), end(),
      |                            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:469:28: note: suggested alternatives:
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:469:37: error: 'end' was not declared in this scope
  469 |     auto it = std::find_if(begin(), end(),
      |                                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:469:37: note: suggested alternatives:
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:478:18: error: 'size' was not declared in this scope; did you mean 'std::size'?
  478 |     if (index >= size()) return;
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:480:15: error: 'begin' was not declared in this scope
  480 |     auto it = begin() + index;
      |               ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:480:15: note: suggested alternatives:
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:481:5: error: 'erase' was not declared in this scope
  481 |     erase(it);
      |     ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer::update_detection(size_t, uint8_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:493:36: error: too many arguments to function 'Mutex* chMtxUnlock()'
  493 |     chMtxUnlock(&ring_buffer_mutex_);
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:497:15: error: invalid conversion from 'const Mutex*' to 'Mutex*' [-fpermissive]
  497 |     chMtxLock(&ring_buffer_mutex_);
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:499:51: error: no match for 'operator[]' (operand types are 'const ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer' and 'size_t' {aka 'unsigned int'})
  499 |     uint8_t result = (index != SIZE_MAX) ? (*this)[index].detection_count : 0;
      |                                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:500:36: error: too many arguments to function 'Mutex* chMtxUnlock()'
  500 |     chMtxUnlock(&ring_buffer_mutex_);
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:505:15: error: invalid conversion from 'const Mutex*' to 'Mutex*' [-fpermissive]
  505 |     chMtxLock(&ring_buffer_mutex_);
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:507:51: error: no match for 'operator[]' (operand types are 'const ui::external_app::enhanced_drone_analyzer::DetectionRingBuffer' and 'size_t' {aka 'unsigned int'})
  507 |     int32_t result = (index != SIZE_MAX) ? (*this)[index].rssi_value : -120;
      |                                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:508:36: error: too many arguments to function 'Mutex* chMtxUnlock()'
  508 |     chMtxUnlock(&ring_buffer_mutex_);
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
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:762:10: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_' will be initialized after [-Wreorder]
  762 |     bool scanning_active_;
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:760:13: warning:   'Thread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_' [-Wreorder]
  760 |     Thread* scanning_thread_;
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:519:1: warning:   when initialized here [-Wreorder]
  519 | DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:792:22: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_' will be initialized after [-Wreorder]
  792 |     WidebandScanData wideband_scan_data_;
      |                      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:764:15: warning:   'FreqmanDB ui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_' [-Wreorder]
  764 |     FreqmanDB freq_db_;
      |               ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:519:1: warning:   when initialized here [-Wreorder]
  519 | DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:789:24: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::detection_processor_' will be initialized after [-Wreorder]
  789 |     DetectionProcessor detection_processor_;  // Unified detection processing
      |                        ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:785:14: warning:   'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_interval_ms_' [-Wreorder]
  785 |     uint32_t scan_interval_ms_;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:519:1: warning:   when initialized here [-Wreorder]
  519 | DroneScanner::DroneScanner(const DroneAnalyzerSettings& config)
      | ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:519:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:519:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::detection_logger_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In destructor 'ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:544:5: error: 'stop_scanning' was not declared in this scope
  544 |     stop_scanning();
      |     ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_memory_usage() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:558:50: error: passing 'const ui::external_app::enhanced_drone_analyzer::DroneScanner' as 'this' argument discards qualifiers [-fpermissive]
  558 |         handle_scan_error("Memory usage critical");
      |                                                  ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:688:10: note:   in call to 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::handle_scan_error(const char*)'
  688 |     void handle_scan_error(const char* error_msg);
      |          ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:644:6: error: no declaration matches 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::start_scanning()'
  644 | void DroneScanner::start_scanning() {
      |      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:644:6: note: no functions named 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::start_scanning()'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:643:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' defined here
  643 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:679:6: error: no declaration matches 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::stop_scanning()'
  679 | void DroneScanner::stop_scanning() {
      |      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:679:6: note: no functions named 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::stop_scanning()'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:643:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' defined here
  643 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::master_wideband_detection_handler(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, Frequency, int32_t, bool)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:726:41: error: 'WILDCARD' is not a member of 'freqman_type'
  726 |     wideband_entry.type = freqman_type::WILDCARD;
      |                                         ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:79: warning: unused parameter 'hardware' [-Wunused-parameter]
  716 | void DroneScanner::master_wideband_detection_handler(DroneHardwareController& hardware, Frequency frequency, int32_t rssi, bool is_approaching) {
      |                                                      ~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::set_scanning_mode(ui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:750:5: error: 'stop_scanning' was not declared in this scope
  750 |     stop_scanning();
      |     ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::process_rssi_detection(const freqman_entry&, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:901:41: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  901 |                 .timestamp = Timestamp::now(),
      |                                         ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:880:13: warning: unused variable 'prev_rssi' [-Wunused-variable]
  880 |     int32_t prev_rssi = local_detection_ring.get_rssi_value(freq_hash);
      |             ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::update_tracked_drone(DroneType, Frequency, int32_t, ThreatLevel)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:929:67: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  929 |             drone.add_rssi(static_cast<int16_t>(rssi), Timestamp::now());
      |                                                                   ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:939:67: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  939 |             drone.add_rssi(static_cast<int16_t>(rssi), Timestamp::now());
      |                                                                   ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:959:83: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  959 |     tracked_drones_[oldest_index].add_rssi(static_cast<int16_t>(rssi), Timestamp::now());
      |                                                                                   ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::remove_stale_drones()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:965:41: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
  965 |     systime_t current_time = Timestamp::now();
      |                                         ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1029:8: error: redefinition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const'
 1029 | size_t DroneScanner::get_approaching_count() const {
      |        ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:697:12: note: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const' previously defined here
  697 |     size_t get_approaching_count() const { return approaching_count_; }
      |            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1033:8: error: redefinition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const'
 1033 | size_t DroneScanner::get_static_count() const {
      |        ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:699:12: note: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const' previously defined here
  699 |     size_t get_static_count() const { return static_count_; }
      |            ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1037:8: error: redefinition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const'
 1037 | size_t DroneScanner::get_receding_count() const {
      |        ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:698:12: note: 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const' previously defined here
  698 |     size_t get_receding_count() const { return receding_count_; }
      |            ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::DroneDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1050:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
 1050 | DroneScanner::DroneDetectionLogger::DroneDetectionLogger()
      | ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1053:5: error: 'create_cache_directory' was not declared in this scope
 1053 |     create_cache_directory();
      |     ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1061:6: error: no declaration matches 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::create_cache_directory()'
 1061 | void DroneScanner::DroneDetectionLogger::create_cache_directory() {
      |      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1061:6: note: no functions named 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::create_cache_directory()'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:716:11: note: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger' defined here
  716 |     class DroneDetectionLogger {
      |           ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::start_session()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1070:33: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1070 |     session_start_ = Timestamp::now();
      |                                 ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::ensure_csv_header()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1102:16: error: 'class Optional<std::filesystem::filesystem_error>' has no member named 'has_value'
 1102 |     if (!error.has_value()) return false;
      |                ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1104:15: error: 'class Optional<std::filesystem::filesystem_error>' has no member named 'has_value'
 1104 |     if (error.has_value()) {
      |               ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1115:24: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1115 |              "%lu,%lu,%d,%u,%u,%u,%.2f\n",
      |                       ~^
      |                        |
      |                        int
      |                       %ld
 1116 |              entry.timestamp, entry.frequency_hz, entry.rssi_db,
      |                                                   ~~~~~~~~~~~~~
      |                                                         |
      |                                                         int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::format_session_summary(size_t, size_t) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1128:47: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1128 |     uint32_t session_duration_ms = Timestamp::now() - session_start_;
      |                                               ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1148:1: error: redefinition of 'ui::external_app::enhanced_drone_analyzer::DetectionProcessor::DetectionProcessor(ui::external_app::enhanced_drone_analyzer::DroneScanner*)'
 1148 | DetectionProcessor::DetectionProcessor(DroneScanner* scanner) : scanner_(scanner) {}
      | ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:593:14: note: 'ui::external_app::enhanced_drone_analyzer::DetectionProcessor::DetectionProcessor(ui::external_app::enhanced_drone_analyzer::DroneScanner*)' previously defined here
  593 |     explicit DetectionProcessor(DroneScanner* scanner) : scanner_(scanner) {}
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1150:6: error: redefinition of 'void ui::external_app::enhanced_drone_analyzer::DetectionProcessor::process_unified_detection(const freqman_entry&, int32_t, int32_t, float, bool)'
 1150 | void DetectionProcessor::process_unified_detection(const freqman_entry& entry, int32_t rssi, int32_t effective_threshold,
      |      ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:596:10: note: 'void ui::external_app::enhanced_drone_analyzer::DetectionProcessor::process_unified_detection(const freqman_entry&, int32_t, int32_t, float, bool)' previously defined here
  596 |     void process_unified_detection(const freqman_entry& entry, int32_t rssi, int32_t effective_threshold,
      |          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::DroneHardwareController(SpectrumMode)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:895:13: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::last_valid_rssi_' will be initialized after [-Wreorder]
  895 |     int32_t last_valid_rssi_;
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:892:26: warning:   'ChannelSpectrumFIFO* ui::external_app::enhanced_drone_analyzer::DroneHardwareController::fifo_' [-Wreorder]
  892 |     ChannelSpectrumFIFO* fifo_;
      |                          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1178:1: warning:   when initialized here [-Wreorder]
 1178 | DroneHardwareController::DroneHardwareController(SpectrumMode mode)
      | ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1178:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_access_mutex_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1178:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::radio_state_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::tune_to_frequency(Frequency)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1250:5: error: 'receiver_model' is not a class, namespace, or enumeration
 1250 |     receiver_model::set_target_frequency(frequency_hz);
      |     ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1275:6: error: no declaration matches 'bool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::wait_for_frequency_lock(systime_t)'
 1275 | bool DroneHardwareController::wait_for_frequency_lock(systime_t timeout_ms) {
      |      ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1275:6: note: no functions named 'bool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::wait_for_frequency_lock(systime_t)'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:810:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneHardwareController' defined here
  810 | class DroneHardwareController {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1304:9: error: no declaration matches 'int32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::read_raw_rssi_from_hardware() const'
 1304 | int32_t DroneHardwareController::read_raw_rssi_from_hardware() const {
      |         ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1304:9: note: no functions named 'int32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::read_raw_rssi_from_hardware() const'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:810:7: note: 'class ui::external_app::enhanced_drone_analyzer::DroneHardwareController' defined here
  810 | class DroneHardwareController {
      |       ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'int32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::get_real_rssi_from_hardware(Frequency)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1324:14: error: 'wait_for_frequency_lock' was not declared in this scope
 1324 |         if (!wait_for_frequency_lock(100)) {  // 100ms timeout for frequency lock
      |              ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1330:29: error: 'read_raw_rssi_from_hardware' was not declared in this scope; did you mean 'get_real_rssi_from_hardware'?
 1330 |     int32_t measured_rssi = read_raw_rssi_from_hardware();
      |                             ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                             get_real_rssi_from_hardware
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1364:28: error: 'Mutex' {aka 'struct Mutex'} has no member named 'lock'
 1364 |     spectrum_access_mutex_.lock();
      |                            ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1381:28: error: 'Mutex' {aka 'struct Mutex'} has no member named 'unlock'
 1381 |     spectrum_access_mutex_.unlock();
      |                            ^~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::update(ThreatLevel, size_t, size_t, size_t, Frequency, bool)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1417:31: error: 'get_threat_text' was not declared in this scope; did you mean 'get_threat_icon_text'?
 1417 |     std::string threat_name = get_threat_text(max_threat);
      |                               ^~~~~~~~~~~~~~~
      |                               get_threat_icon_text
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1433:5: error: member 'ui::Style::font' is uninitialized reference
 1433 |     };
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1433:5: warning: missing initializer for member 'ui::Style::font' [-Wmissing-field-initializers]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1433:5: warning: missing initializer for member 'ui::Style::background' [-Wmissing-field-initializers]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1433:5: error: designator order for field 'ui::Style::background' does not match declaration order in 'ui::Style'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1456:66: error: taking address of rvalue [-fpermissive]
 1456 |     threat_frequency_.set_style(&get_threat_text_color(max_threat));
      |                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1456:33: error: cannot convert 'ui::Color*' to 'const ui::Style*'
 1456 |     threat_frequency_.set_style(&get_threat_text_color(max_threat));
      |                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                 |
      |                                 ui::Color*
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:14,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/common/ui_widget.hpp:119:33: note:   initializing argument 1 of 'void ui::Widget::set_style(const ui::Style*)'
  119 |     void set_style(const Style* new_style);
      |                    ~~~~~~~~~~~~~^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1512:13: error: no declaration matches 'std::string ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::get_threat_text(ThreatLevel) const'
 1512 | std::string SmartThreatHeader::get_threat_text(ThreatLevel level) const {
      |             ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1512:13: note: no functions named 'std::string ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::get_threat_text(ThreatLevel) const'
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:908:7: note: 'class ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' defined here
  908 | class SmartThreatHeader : public View {
      |       ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1533:37: error: 'parent_rect_' was not declared in this scope; did you mean 'parent_rect'?
 1533 |         painter.fill_rectangle(Rect(parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 4), base_color);
      |                                     ^~~~~~~~~~~~
      |                                     parent_rect
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1530:17: warning: unused variable 'alpha' [-Wunused-variable]
 1530 |         uint8_t alpha = (pulse_timer % 20) < 10 ? 50 : 100;
      |                 ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1562:67: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1562 |     progress_text_.set_style(Style{Theme::getInstance()->fg_blue->foreground, Color::black(), Theme::getInstance()->fg_blue->font});
      |                                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1562:126: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_blue->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1562 |     progress_text_.set_style(Style{Theme::getInstance()->fg_blue->foreground, Color::black(), Theme::getInstance()->fg_blue->font});
      |                                                                                               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                                                                              |
      |                                                                                                                              const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1568:45: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1568 |         Style alert_style{(detections > 10) ? Theme::getInstance()->fg_red->foreground : Theme::getInstance()->fg_yellow->foreground,
      |                           ~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1569:75: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_yellow->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1569 |                          Color::black(), Theme::getInstance()->fg_yellow->font};
      |                                          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                           |
      |                                                                           const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_alert_status(ThreatLevel, size_t, const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1587:57: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1587 |     Style alert_style{(threat >= ThreatLevel::CRITICAL) ? Theme::getInstance()->fg_red->foreground : Theme::getInstance()->fg_yellow->foreground,
      |                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1588:71: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_yellow->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1588 |                      Color::black(), Theme::getInstance()->fg_yellow->font};
      |                                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                       |
      |                                                                       const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_normal_status(const string&, const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1603:66: error: invalid initialization of reference of type 'const ui::Font&' from expression of type 'const ui::Color'
 1603 |     normal_text_.set_style(Style{Theme::getInstance()->fg_light->foreground, Color::black(), Theme::getInstance()->fg_light->font});
      |                                  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1603:126: error: could not convert 'ui::Theme::getInstance()->ui::ThemeTemplate::fg_light->ui::Style::font' from 'const ui::Font' to 'const ui::Color'
 1603 |     normal_text_.set_style(Style{Theme::getInstance()->fg_light->foreground, Color::black(), Theme::getInstance()->fg_light->font});
      |                                                                                              ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
      |                                                                                                                              |
      |                                                                                                                              const ui::Font
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1628:37: error: 'parent_rect_' was not declared in this scope; did you mean 'parent_rect'?
 1628 |         painter.fill_rectangle(Rect(parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2), Color(32, 0, 0));
      |                                     ^~~~~~~~~~~~
      |                                     parent_rect
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1661:1: error: expected unqualified-id before '/' token
 1661 | / CORRECTED: Fixed memory leaks using RAII members instead of chained unique_ptr initialization
      | ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_show()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1750:14: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController'
 1750 |     hardware_->on_hardware_show();
      |              ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_hide()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1755:14: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController'
 1755 |     hardware_->on_hardware_hide();
      |              ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1771:9: error: 'ui_controller_' was not declared in this scope; did you mean 'display_controller_'?
 1771 |         ui_controller_->on_stop_scan();
      |         ^~~~~~~~~~~~~~
      |         display_controller_
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1774:9: error: 'ui_controller_' was not declared in this scope; did you mean 'display_controller_'?
 1774 |         ui_controller_->on_start_scan();
      |         ^~~~~~~~~~~~~~
      |         display_controller_
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_menu_button()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1781:5: error: 'ui_controller_' was not declared in this scope; did you mean 'display_controller_'?
 1781 |     ui_controller_->show_menu();
      |     ^~~~~~~~~~~~~~
      |     display_controller_
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1787:56: error: cannot convert 'std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader>::__single_object' {aka 'std::unique_ptr<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader, std::default_delete<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader> >'} to 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader*' in assignment
 1787 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                        |
      |                                                        std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader>::__single_object {aka std::unique_ptr<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader, std::default_delete<ui::external_app::enhanced_drone_analyzer::SmartThreatHeader> >}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1788:53: error: cannot convert 'std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar>::__single_object' {aka 'std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar, std::default_delete<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar> >'} to 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar*' in assignment
 1788 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                     |
      |                                                     std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar>::__single_object {aka std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar, std::default_delete<ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar> >}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1793:68: warning: narrowing conversion of 'card_y_pos' from 'size_t' {aka 'unsigned int'} to 'int' [-Wnarrowing]
 1793 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                    ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1793:56: error: cannot convert 'std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ThreatCard>::__single_object' {aka 'std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ThreatCard, std::default_delete<ui::external_app::enhanced_drone_analyzer::ThreatCard> >'} to 'std::array<ui::external_app::enhanced_drone_analyzer::ThreatCard*, 3>::value_type' {aka 'ui::external_app::enhanced_drone_analyzer::ThreatCard*'} in assignment
 1793 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                        |
      |                                                        std::_MakeUniq<ui::external_app::enhanced_drone_analyzer::ThreatCard>::__single_object {aka std::unique_ptr<ui::external_app::enhanced_drone_analyzer::ThreatCard, std::default_delete<ui::external_app::enhanced_drone_analyzer::ThreatCard> >}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1807:9: error: no match for 'operator!' (operand type is 'ui::external_app::enhanced_drone_analyzer::DroneScanner')
 1807 |     if (!scanner_) return;
      |         ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1807:9: note: candidate: 'operator!(bool)' <built-in>
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1807:9: note:   no known conversion for argument 1 from 'ui::external_app::enhanced_drone_analyzer::DroneScanner' to 'bool'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1810:38: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1810 |     ThreatLevel max_threat = scanner_->get_max_detected_threat();
      |                                      ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1811:34: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1811 |     size_t approaching = scanner_->get_approaching_count();
      |                                  ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1812:35: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1812 |     size_t static_count = scanner_->get_static_count();
      |                                   ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1813:31: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1813 |     size_t receding = scanner_->get_receding_count();
      |                               ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1814:32: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1814 |     bool is_scanning = scanner_->is_scanning_active();
      |                                ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1815:38: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1815 |     Frequency current_freq = scanner_->get_current_scanning_frequency();
      |                                      ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1816:41: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1816 |     uint32_t total_detections = scanner_->get_total_detections();
      |                                         ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1825:35: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1825 |         uint32_t cycles = scanner_->get_scan_cycles();
      |                                   ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:55: error: no matching function for call to 'min(long unsigned int, unsigned int)'
 1826 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:55: note:   deduced conflicting types for parameter 'const _Tp' ('long unsigned int' and 'unsigned int')
 1826 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:55: note:   deduced conflicting types for parameter 'const _Tp' ('long unsigned int' and 'unsigned int')
 1826 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:55: note:   mismatched types 'std::initializer_list<_Tp>' and 'long unsigned int'
 1826 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:55: note:   mismatched types 'std::initializer_list<_Tp>' and 'long unsigned int'
 1826 |         uint32_t progress = std::min(cycles * 10, 100u); // Rough estimate
      |                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1838:60: error: 'MAX_DISPLAYED_DRONES' is not a member of 'DisplayDroneEntry'
 1838 |     for (size_t i = 0; i < std::min(3u, DisplayDroneEntry::MAX_DISPLAYED_DRONES); ++i) {
      |                                                            ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1839:37: error: base operand of '->' has non-pointer type 'ui::external_app::enhanced_drone_analyzer::DroneScanner'
 1839 |         const auto& drone = scanner_->getTrackedDrone(i);
      |                                     ^~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'DisplayDroneEntry::DisplayDroneEntry()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::frequency' should be initialized in the member initialization list [-Weffc++]
  144 | struct DisplayDroneEntry {
      |        ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::type' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::threat' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::rssi' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::last_seen' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::type_name' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::display_color' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:144:8: warning: 'DisplayDroneEntry::trend' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1842:31: note: synthesized method 'DisplayDroneEntry::DisplayDroneEntry()' first required here
 1842 |             DisplayDroneEntry entry;
      |                               ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:42: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1847 |             entry.last_seen = Timestamp::now();
      |                                          ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1860:53: error: 'MAX_DISPLAYED_DRONES' is not a member of 'DisplayDroneEntry'
 1860 |     for (size_t i = std::min(3u, DisplayDroneEntry::MAX_DISPLAYED_DRONES); i < 3; ++i) {
      |                                                     ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::LoadingScreenView::LoadingScreenView(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1870:31: error: 'now' is not a member of 'Timestamp' {aka 'lpc43xx::rtc::RTC'}
 1870 |       timer_start_(Timestamp::now())
      |                               ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1877:1: error: definition of explicitly-defaulted 'ui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()'
 1877 | LoadingScreenView::~LoadingScreenView() {
      | ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1257:5: note: 'virtual ui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()' explicitly defaulted here
 1257 |     ~LoadingScreenView() = default;
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1891:1: error: redefinition of 'ui::external_app::enhanced_drone_analyzer::AudioManager::AudioManager()'
 1891 | AudioManager::AudioManager() : audio_enabled_(true) {}
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:316:5: note: 'ui::external_app::enhanced_drone_analyzer::AudioManager::AudioManager()' previously defined here
  316 |     AudioManager() : audio_enabled_(true) {}
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1892:1: error: definition of explicitly-defaulted 'ui::external_app::enhanced_drone_analyzer::AudioManager::~AudioManager()'
 1892 | AudioManager::~AudioManager() { stop_audio(); }
      | ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:317:5: note: 'ui::external_app::enhanced_drone_analyzer::AudioManager::~AudioManager()' explicitly defaulted here
  317 |     ~AudioManager() = default;
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1894:6: error: redefinition of 'void ui::external_app::enhanced_drone_analyzer::AudioManager::play_detection_beep(ThreatLevel)'
 1894 | void AudioManager::play_detection_beep(ThreatLevel level) {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:322:10: note: 'void ui::external_app::enhanced_drone_analyzer::AudioManager::play_detection_beep(ThreatLevel)' previously defined here
  322 |     void play_detection_beep(ThreatLevel threat) {
      |          ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1914:6: error: redefinition of 'void ui::external_app::enhanced_drone_analyzer::AudioManager::stop_audio()'
 1914 | void AudioManager::stop_audio() {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:325:10: note: 'void ui::external_app::enhanced_drone_analyzer::AudioManager::stop_audio()' previously defined here
  325 |     void stop_audio() {
      |          ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1919:6: error: redefinition of 'void ui::external_app::enhanced_drone_analyzer::AudioManager::toggle_audio()'
 1919 | void AudioManager::toggle_audio() {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:321:10: note: 'void ui::external_app::enhanced_drone_analyzer::AudioManager::toggle_audio()' previously defined here
  321 |     void toggle_audio() { audio_enabled_ = !audio_enabled_; }
      |          ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1923:6: error: redefinition of 'bool ui::external_app::enhanced_drone_analyzer::AudioManager::is_audio_enabled() const'
 1923 | bool AudioManager::is_audio_enabled() const {
      |      ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:320:10: note: 'bool ui::external_app::enhanced_drone_analyzer::AudioManager::is_audio_enabled() const' previously defined here
  320 |     bool is_audio_enabled() const { return audio_enabled_; }
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1928:71: error: definition of 'bool SimpleDroneValidation::validate_frequency_range(Frequency)' is not in namespace enclosing 'SimpleDroneValidation' [-fpermissive]
 1928 | bool SimpleDroneValidation::validate_frequency_range(Frequency freq_hz) {
      |                                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1932:85: error: definition of 'bool SimpleDroneValidation::validate_rssi_signal(int32_t, ThreatLevel)' is not in namespace enclosing 'SimpleDroneValidation' [-fpermissive]
 1932 | bool SimpleDroneValidation::validate_rssi_signal(int32_t rssi_db, ThreatLevel threat) {
      |                                                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1937:76: error: definition of 'ThreatLevel SimpleDroneValidation::classify_signal_strength(int32_t)' is not in namespace enclosing 'SimpleDroneValidation' [-fpermissive]
 1937 | ThreatLevel SimpleDroneValidation::classify_signal_strength(int32_t rssi_db) {
      |                                                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1944:88: error: definition of 'DroneType SimpleDroneValidation::identify_drone_type(Frequency, int32_t)' is not in namespace enclosing 'SimpleDroneValidation' [-fpermissive]
 1944 | DroneType SimpleDroneValidation::identify_drone_type(Frequency freq_hz, int32_t rssi_db) {
      |                                                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:86: error: definition of 'bool SimpleDroneValidation::validate_drone_detection(Frequency, int32_t, DroneType, ThreatLevel)' is not in namespace enclosing 'SimpleDroneValidation' [-fpermissive]
 1951 |                                                    DroneType type, ThreatLevel threat) {
      |                                                                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::ScanningCoordinator(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, ui::external_app::enhanced_drone_analyzer::DroneDisplayController&, ui::external_app::enhanced_drone_analyzer::AudioManager&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1958:1: warning: 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::scan_coordinator_mutex_' should be initialized in the member initialization list [-Weffc++]
 1958 | ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
      | ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'msg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::coordinated_scanning_thread()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2010:22: error: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' has no member named 'is_scanning_active'; did you mean 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_'? (not accessible from this context)
 2010 |         if (scanner_.is_scanning_active()) {
      |                      ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:762:10: note: declared private here
  762 |     bool scanning_active_;
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2040:6: error: redefinition of 'bool ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::is_scanning_active() const'
 2040 | bool ScanningCoordinator::is_scanning_active() const {
      |      ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:1121:10: note: 'bool ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::is_scanning_active() const' previously defined here
 1121 |     bool is_scanning_active() const { return scanning_active_; }
      |          ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'bool ui::external_app::enhanced_drone_analyzer::ScannerSettingsManager::parse_key_value(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&, const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2085:41: error: 'validate_range' was not declared in this scope
 2085 |             settings.scan_interval_ms = validate_range<uint32_t>(
      |                                         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2085:64: error: expected primary-expression before '>' token
 2085 |             settings.scan_interval_ms = validate_range<uint32_t>(
      |                                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2088:44: warning: right operand of comma operator has no effect [-Wunused-value]
 2088 |                 static_cast<uint32_t>(5000U));
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2091:42: error: 'validate_range' was not declared in this scope
 2091 |             settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
      |                                          ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2091:64: error: expected primary-expression before '>' token
 2091 |             settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
      |                                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2091:91: warning: right operand of comma operator has no effect [-Wunused-value]
 2091 |             settings.rssi_threshold_db = validate_range<int32_t>(std::stoi(value), -120, -30);
      |                                                                                           ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2097:49: error: 'validate_range' was not declared in this scope
 2097 |             settings.audio_alert_frequency_hz = validate_range<uint16_t>(
      |                                                 ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2097:72: error: expected primary-expression before '>' token
 2097 |             settings.audio_alert_frequency_hz = validate_range<uint16_t>(
      |                                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2098:17: warning: value computed is not used [-Wunused-value]
 2098 |                 static_cast<uint16_t>(std::stoul(value)),
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2100:44: warning: right operand of comma operator has no effect [-Wunused-value]
 2100 |                 static_cast<uint16_t>(3000U));
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2103:48: error: 'validate_range' was not declared in this scope
 2103 |             settings.audio_alert_duration_ms = validate_range<uint32_t>(
      |                                                ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2103:71: error: expected primary-expression before '>' token
 2103 |             settings.audio_alert_duration_ms = validate_range<uint32_t>(
      |                                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2106:44: warning: right operand of comma operator has no effect [-Wunused-value]
 2106 |                 static_cast<uint32_t>(2000U));
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2109:46: error: 'validate_range' was not declared in this scope
 2109 |             settings.hardware_bandwidth_hz = validate_range<uint32_t>(
      |                                              ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2109:69: error: expected primary-expression before '>' token
 2109 |             settings.hardware_bandwidth_hz = validate_range<uint32_t>(
      |                                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2112:49: warning: right operand of comma operator has no effect [-Wunused-value]
 2112 |                 static_cast<uint32_t>(100000000U));
      |                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2337:25: error: variable or field 'initialize_app' declared void
 2337 | void initialize_app(ui::NavigationView& nav) {
      |                         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2337:25: error: 'NavigationView' is not a member of 'ui::external_app::enhanced_drone_analyzer::ui'; did you mean 'ui::NavigationView'?
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:15,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./ui_navigation.hpp:95:7: note: 'ui::NavigationView' declared here
   95 | class NavigationView : public View {
      |       ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2337:41: error: 'nav' was not declared in this scope; did you mean 'nan'?
 2337 | void initialize_app(ui::NavigationView& nav) {
      |                                         ^~~
      |                                         nan
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2347:81: error: 'initialize_app' is not a member of 'ui::external_app::enhanced_drone_analyzer::ui::external_app::enhanced_drone_analyzer_scanner'; did you mean 'ui::external_app::enhanced_drone_analyzer::initialize_app'?
 2347 |     /*.externalAppEntry = */ ui::external_app::enhanced_drone_analyzer_scanner::initialize_app,
      |                                                                                 ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2181:6: note: 'ui::external_app::enhanced_drone_analyzer::initialize_app' declared here
 2181 | void initialize_app(ui::NavigationView& nav) {
      |      ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2357:28: error: 'ui::external_app::enhanced_drone_analyzer::ui::Color' has not been declared
 2357 |     /*.icon_color = */ ui::Color::red().v,
      |                            ^~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5775: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.