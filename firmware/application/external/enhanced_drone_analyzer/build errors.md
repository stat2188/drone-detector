[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:4:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::SpectrumAnalyzerSettings()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::preset_ranges' should be initialized in the member initialization list [-Weffc++]
   92 | struct SpectrumAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::iq_calibration' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::amplifiers' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::range_lock' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::SpectrumPresetLoader::SpectrumPresetLoader(const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:14:49: note: synthesized method 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::SpectrumAnalyzerSettings()' first required here
   14 |     : preset_file_path_(preset_path), settings_() {
      |                                                 ^
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_signal_processing.cpp.obj
[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_settings_combined.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static void ui::external_app::enhanced_drone_analyzer::EnhancedSettingsManager::restore_from_backup(const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:135:18: error: exception handling disabled, use '-fexceptions' to enable
  135 |         } catch (...) {
      |                  ^~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5831: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_settings_combined.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.