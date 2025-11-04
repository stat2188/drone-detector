[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp: In member function 'bool AudioManager::is_audio_enabled() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:17:44: error: 'AudioAlertManager' has not been declared
   17 |     bool is_audio_enabled() const { return AudioAlertManager::is_enabled(); }
      |                                            ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp: In member function 'void AudioManager::toggle_audio()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:18:27: error: 'AudioAlertManager' has not been declared
   18 |     void toggle_audio() { AudioAlertManager::set_enabled(!AudioAlertManager::is_enabled()); }
      |                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:18:59: error: 'AudioAlertManager' has not been declared
   18 |     void toggle_audio() { AudioAlertManager::set_enabled(!AudioAlertManager::is_enabled()); }
      |                                                           ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp: In static member function 'static void AudioManager::play_alert_for_threat(ThreatLevel)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:30:28: error: 'AudioAlertManager' does not name a type; did you mean 'AudioManager'?
   30 |         using AlertLevel = AudioAlertManager::AlertLevel;
      |                            ^~~~~~~~~~~~~~~~~
      |                            AudioManager
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:31:9: error: 'AlertLevel' was not declared in this scope
   31 |         AlertLevel level = AlertLevel::LOW;
      |         ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:34:36: error: 'level' was not declared in this scope
   34 |             case ThreatLevel::LOW: level = AlertLevel::LOW; break;
      |                                    ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:34:44: error: 'AlertLevel' is not a class, namespace, or enumeration
   34 |             case ThreatLevel::LOW: level = AlertLevel::LOW; break;
      |                                            ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:35:47: error: 'AlertLevel' is not a class, namespace, or enumeration
   35 |             case ThreatLevel::MEDIUM: level = AlertLevel::HIGH; break;
      |                                               ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:36:45: error: 'AlertLevel' is not a class, namespace, or enumeration
   36 |             case ThreatLevel::HIGH: level = AlertLevel::HIGH; break;
      |                                             ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:37:49: error: 'AlertLevel' is not a class, namespace, or enumeration
   37 |             case ThreatLevel::CRITICAL: level = AlertLevel::CRITICAL; break;
      |                                                 ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:38:30: error: 'AlertLevel' is not a class, namespace, or enumeration
   38 |             default: level = AlertLevel::LOW; break;
      |                              ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:40:9: error: 'AudioAlertManager' has not been declared
   40 |         AudioAlertManager::play_alert(level);
      |         ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:40:39: error: 'level' was not declared in this scope
   40 |         AudioAlertManager::play_alert(level);
      |                                       ^~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:45:12: error: multiple definition of 'enum class ThreatLevel'
   45 | enum class ThreatLevel {
      |            ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:10:12: note: previous definition here
   10 | enum class ThreatLevel {
      |            ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:53:12: error: multiple definition of 'enum class DroneType'
   53 | enum class DroneType {
      |            ^~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:18:12: note: previous definition here
   18 | enum class DroneType {
      |            ^~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:64:12: error: multiple definition of 'enum class MovementTrend'
   64 | enum class MovementTrend {
      |            ^~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:29:12: note: previous definition here
   29 | enum class MovementTrend {
      |            ^~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:71:12: error: multiple definition of 'enum class SpectrumMode'
   71 | enum class SpectrumMode {
      |            ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:36:12: note: previous definition here
   36 | enum class SpectrumMode {
      |            ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:90:7: error: redefinition of 'class WidebandMedianFilter'
   90 | class WidebandMedianFilter {
      |       ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:30:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:16:7: note: previous definition of 'class WidebandMedianFilter'
   16 | class WidebandMedianFilter {
      |       ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:119:8: error: redefinition of 'struct DetectionEntry'
  119 | struct DetectionEntry {
      |        ^~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:30:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:45:8: note: previous definition of 'struct DetectionEntry'
   45 | struct DetectionEntry {
      |        ^~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:126:7: error: redefinition of 'class DetectionRingBuffer'
  126 | class DetectionRingBuffer {
      |       ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:30:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:52:7: note: previous definition of 'class DetectionRingBuffer'
   52 | class DetectionRingBuffer {
      |       ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:35:35: error: expected unqualified-id before '-' token
   35 | #define DEFAULT_RSSI_THRESHOLD_DB -80
      |                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:295:26: note: in expansion of macro 'DEFAULT_RSSI_THRESHOLD_DB'
  295 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:36:30: error: expected unqualified-id before numeric constant
   36 | #define HYSTERESIS_MARGIN_DB 5
      |                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:299:26: note: in expansion of macro 'HYSTERESIS_MARGIN_DB'
  299 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.