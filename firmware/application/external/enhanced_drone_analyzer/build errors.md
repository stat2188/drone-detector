[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:750:2: error: #endif without #if
  750 | #endif // __UI_SCANNER_COMBINED_HPP__
      |  ^~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp: In static member function 'static void DroneAnalyzerSettingsManager::reset_to_defaults(DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:46:34: error: 'DEFAULT_RSSI_THRESHOLD_DB' was not declared in this scope
   46 |     settings.rssi_threshold_db = DEFAULT_RSSI_THRESHOLD_DB;
      |                                  ^~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp: In static member function 'static bool DroneAnalyzerSettingsManager::save_settings(const DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:108:86: warning: unused parameter 'settings' [-Wunused-parameter]
  108 | inline bool DroneAnalyzerSettingsManager::save_settings(const DroneAnalyzerSettings& settings) {
      |                                                         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:30:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp: In member function 'int16_t WidebandMedianFilter::get_median_threshold() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:39:40: error: 'HYSTERESIS_MARGIN_DB' was not declared in this scope
   39 |         return temp[WINDOW_SIZE / 2] - HYSTERESIS_MARGIN_DB;
      |                                        ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:310:5: error: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()' cannot be overloaded with 'virtual ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()'
  310 |     ~DroneScanner();
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:13: note: previous declaration 'virtual ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()'
  300 |     virtual ~DroneScanner();
      |             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:298:7: warning: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' has pointer data members [-Weffc++]
  298 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:298:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:298:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:715:21: error: 'ScanningCoordinator' was not declared in this scope
  715 |     std::unique_ptr<ScanningCoordinator> scanning_coordinator_;
      |                     ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:715:40: error: template argument 1 is invalid
  715 |     std::unique_ptr<ScanningCoordinator> scanning_coordinator_;
      |                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:715:40: error: template argument 2 is invalid
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:16:12: error: multiple definition of 'enum class SpectrumMode'
   16 | enum class SpectrumMode {
      |            ^~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:11:12: note: previous definition here
   11 | enum class SpectrumMode { NARROW, MEDIUM, WIDE, ULTRA_WIDE };
      |            ^~~~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.