[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:751:2: error: #endif without #if
  751 | #endif // __UI_SCANNER_COMBINED_HPP__
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
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:26,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:16:7: warning: 'class ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' has pointer data members [-Weffc++]
   16 | class ScanningCoordinator {
      |       ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:16:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:16:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:311:5: error: 'ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()' cannot be overloaded with 'virtual ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()'
  311 |     ~DroneScanner();
      |     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:301:13: note: previous declaration 'virtual ui::external_app::enhanced_drone_analyzer::DroneScanner::~DroneScanner()'
  301 |     virtual ~DroneScanner();
      |             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:299:7: warning: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' has pointer data members [-Weffc++]
  299 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:299:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:299:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.
