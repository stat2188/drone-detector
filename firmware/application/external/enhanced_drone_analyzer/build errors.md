In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:17:7: warning: 'class ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' has pointer data members [-Weffc++]
   17 | class ScanningCoordinator {
      |       ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:17:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:17:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)' [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_settings_main.cpp:7:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:343:7: warning: 'class ui::external_app::enhanced_drone_analyzer::DroneScanner' has pointer data members [-Weffc++]
  343 | class DroneScanner {
      |       ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:343:7: warning:   but does not override 'ui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:343:7: warning:   or 'operator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' [-Weffc++]
cc1plus: warning: unrecognized command line option '-Wno-volatile'
[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:20:
/havoc/firmware/application/ui/../ui_navigation.hpp: In instantiation of 'T* ui::NavigationView::push(Args&& ...) [with T = ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView; Args = {}]':
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:59:92:   required from here
/havoc/firmware/application/ui/../ui_navigation.hpp:111:69: error: invalid use of incomplete type 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
  111 |         return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
      |                                                                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:52:7: note: forward declaration of 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
   52 | class EnhancedDroneSpectrumAnalyzerView;
      |       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5775: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.