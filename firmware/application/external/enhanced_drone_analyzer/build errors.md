[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp: In constructor 'AudioAlertManager::AudioAlertManager()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:15:27: error: 'bool AudioAlertManager::audio_enabled_' is a static data member; it can only be initialized at its definition
   15 |     AudioAlertManager() : audio_enabled_(true) {}
      |                           ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp: In static member function 'static void AudioAlertManager::play_alert(AudioAlertManager::AlertLevel)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:27:19: error: 'request_audio_beep' is not a member of 'baseband'
   27 |         baseband::request_audio_beep(freq_hz, 48000, 200);
      |                   ^~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:46:7: error: redefinition of 'class AudioAlertManager'
   46 | class AudioAlertManager {
      |       ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:11:7: note: previous definition of 'class AudioAlertManager'
   11 | class AudioAlertManager {
      |       ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:165:26: error: redefinition of 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB'
  165 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:44:26: note: 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB' previously defined here
   44 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:166:26: error: redefinition of 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB'
  166 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:45:26: note: 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB' previously defined here
   45 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:167:26: error: redefinition of 'constexpr const int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB'
  167 | static constexpr int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:46:26: note: 'constexpr const int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB' previously defined here
   46 | static constexpr int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_show()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:724:5: error: 'field_rx_iq_phase_cal' was not declared in this scope
  724 |     field_rx_iq_phase_cal.set_range(0, 63);  // max2839 has 6 bits [0..63]
      |     ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:759:6: error: redefinition of 'static void AudioAlertManager::play_alert(AudioAlertManager::AlertLevel)'
  759 | void AudioAlertManager::play_alert(AlertLevel level) {
      |      ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:17:17: note: 'static void AudioAlertManager::play_alert(AudioAlertManager::AlertLevel)' previously defined here
   17 |     static void play_alert(AlertLevel level) {
      |                 ^~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:772:6: error: redefinition of 'static void AudioAlertManager::set_enabled(bool)'
  772 | void AudioAlertManager::set_enabled(bool enable) { audio_enabled_ = enable; }
      |      ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:30:17: note: 'static void AudioAlertManager::set_enabled(bool)' previously defined here
   30 |     static void set_enabled(bool enable) { audio_enabled_ = enable; }
      |                 ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:773:6: error: redefinition of 'static bool AudioAlertManager::is_enabled()'
  773 | bool AudioAlertManager::is_enabled() { return audio_enabled_; }
      |      ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:31:17: note: 'static bool AudioAlertManager::is_enabled()' previously defined here
   31 |     static bool is_enabled() { return audio_enabled_; }
      |                 ^~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.