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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'TrackedDrone::TrackedDrone()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:72:5: warning: 'TrackedDrone::frequency' should be initialized in the member initialization list [-Weffc++]
   72 |     TrackedDrone() {
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:72:5: warning: 'TrackedDrone::drone_type' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:72:5: warning: 'TrackedDrone::threat_level' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:72:5: warning: 'TrackedDrone::update_count' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:72:5: warning: 'TrackedDrone::last_seen' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:169:26: error: redefinition of 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB'
  169 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:44:26: note: 'constexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB' previously defined here
   44 | static constexpr int32_t DEFAULT_RSSI_THRESHOLD_DB = -90;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:170:26: error: redefinition of 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB'
  170 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:45:26: note: 'constexpr const int32_t WIDEBAND_RSSI_THRESHOLD_DB' previously defined here
   45 | static constexpr int32_t WIDEBAND_RSSI_THRESHOLD_DB = -95;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:171:26: error: redefinition of 'constexpr const int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB'
  171 | static constexpr int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:46:26: note: 'constexpr const int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB' previously defined here
   46 | static constexpr int32_t WIDEBAND_DYNAMIC_THRESHOLD_OFFSET_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:755:6: error: redefinition of 'static void AudioAlertManager::play_alert(AudioAlertManager::AlertLevel)'
  755 | void AudioAlertManager::play_alert(AlertLevel level) {
      |      ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:17: note: 'static void AudioAlertManager::play_alert(AudioAlertManager::AlertLevel)' previously defined here
   50 |     static void play_alert(AlertLevel level) {
      |                 ^~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:768:6: error: redefinition of 'static void AudioAlertManager::set_enabled(bool)'
  768 | void AudioAlertManager::set_enabled(bool enable) { audio_enabled_ = enable; }
      |      ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:63:17: note: 'static void AudioAlertManager::set_enabled(bool)' previously defined here
   63 |     static void set_enabled(bool enable) { audio_enabled_ = enable; }
      |                 ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:769:6: error: redefinition of 'static bool AudioAlertManager::is_enabled()'
  769 | bool AudioAlertManager::is_enabled() { return audio_enabled_; }
      |      ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:64:17: note: 'static bool AudioAlertManager::is_enabled()' previously defined here
   64 |     static bool is_enabled() { return audio_enabled_; }
      |                 ^~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.