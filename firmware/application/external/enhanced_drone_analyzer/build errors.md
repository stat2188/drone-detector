[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:45:12: error: multiple definition of 'enum class ThreatLevel'
   45 | enum class ThreatLevel {
      |            ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:10:12: note: previous definition here
   10 | enum class ThreatLevel {
      |            ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:53:12: error: multiple definition of 'enum class DroneType'
   53 | enum class DroneType {
      |            ^~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:18:12: note: previous definition here
   18 | enum class DroneType {
      |            ^~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:64:12: error: multiple definition of 'enum class MovementTrend'
   64 | enum class MovementTrend {
      |            ^~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:29:12: note: previous definition here
   29 | enum class MovementTrend {
      |            ^~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:71:12: error: multiple definition of 'enum class SpectrumMode'
   71 | enum class SpectrumMode {
      |            ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:32,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:36:12: note: previous definition here
   36 | enum class SpectrumMode {
      |            ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void DetectionRingBuffer::update_detection(size_t, uint8_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:136:34: error: 'chVTGetSystemTime' was not declared in this scope
  136 |         systime_t current_time = chVTGetSystemTime();
      |                                  ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
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
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:873:76: error: could not convert '{{80, 190}, 20, ((ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView*)this)->ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::scanning_mode_options_}' from '<brace-enclosed initializer list>' to 'ui::OptionsField'
  873 |     OptionsField field_scanning_mode_{{80, 190}, 20, scanning_mode_options_};
      |                                                                            ^
      |                                                                            |
      |                                                                            <brace-enclosed initializer list>
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11:50: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' conflicts with a previous declaration
   11 | using ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings;
      |                                                  ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:339:8: note: previous declaration 'struct DroneAnalyzerSettings'
  339 | struct DroneAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:12:50: error: 'ui::external_app::enhanced_drone_analyzer::SpectrumMode' has not been declared
   12 | using ui::external_app::enhanced_drone_analyzer::SpectrumMode;
      |                                                  ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: In function 'bool ScannerSettingsManager::parse_key_value(DroneAnalyzerSettings&, const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:90:22: error: 'struct DroneAnalyzerSettings' has no member named 'freqman_path'
   90 |             settings.freqman_path = value.substr(0, 64);
      |                      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: In function 'void ScannerSettingsManager::reset_to_defaults(DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:106:18: error: 'struct DroneAnalyzerSettings' has no member named 'freqman_path'
  106 |         settings.freqman_path = "DRONES";
      |                  ^~~~~~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.