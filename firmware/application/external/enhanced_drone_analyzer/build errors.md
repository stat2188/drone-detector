[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In lambda function:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:787:110: warning: type qualifiers ignored on cast result type [-Wignored-qualifiers]
  787 |                 this->handle_channel_spectrum_config(static_cast<const ChannelSpectrumConfigMessage* const>(p));
      |                                                                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1396:42: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1396 |         snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
      |                                          ^~~~~~~~~~~~~~~~~~~~
......
 1399 |                 (long int)drone.rssi,
      |                 ~~~~~~~~~~~~~~~~~~~~      
      |                 |
      |                 int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1141:53: warning: ' D:' directive output may be truncated writing 3 bytes into a region of size between 0 and 25 [-Wformat-truncation=]
 1141 |     snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
      |                                                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1141:13: note: 'snprintf' output between 12 and 63 bytes into a destination of size 32
 1141 |     snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
      |     ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1142 |             progress_bar, (unsigned long)progress_percent, (unsigned long)total_cycles, (unsigned long)detections);
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::SpectrumPresetLoader::SpectrumPresetLoader(const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:13:1: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumPresetLoader::settings_' should be initialized in the member initialization list [-Weffc++]
   13 | SpectrumPresetLoader::SpectrumPresetLoader(const std::string& preset_path)
      | ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:4:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::SpectrumAnalyzerSettings()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::preset_ranges' should be initialized in the member initialization list [-Weffc++]
   92 | struct SpectrumAnalyzerSettings {
      |        ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::iq_calibration' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::amplifiers' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:92:8: warning: 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::range_lock' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::SpectrumPresetLoader::SpectrumPresetLoader(const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:14:36: note: synthesized method 'ui::external_app::enhanced_drone_analyzer::SpectrumAnalyzerSettings::SpectrumAnalyzerSettings()' first required here
   14 |     : preset_file_path_(preset_path) {
      |                                    ^
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_signal_processing.cpp.obj
[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_settings_combined.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static void ui::external_app::enhanced_drone_analyzer::EnhancedSettingsManager::create_backup_file(const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:122:18: error: exception handling disabled, use '-fexceptions' to enable
  122 |         } catch (...) {
      |                  ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static std::string ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettingsManager::serialize(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:372:12: warning: enumeration value 'ULTRA_NARROW' not handled in switch [-Wswitch]
  372 |     switch (settings.spectrum_mode) {
      |            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::ScannerConfig::save_to_file(const string&) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:474:12: warning: enumeration value 'ULTRA_NARROW' not handled in switch [-Wswitch]
  474 |     switch (config_data_.spectrum_mode) {
      |            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::AudioSettingsView::AudioSettingsView(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:802:1: warning: 'ui::external_app::enhanced_drone_analyzer::AudioSettingsView::audio_settings_' should be initialized in the member initialization list [-Weffc++]
  802 | AudioSettingsView::AudioSettingsView(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:7,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp: In instantiation of 'T* ui::NavigationView::push(Args&& ...) [with T = ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_type_filtered_presets(ui::NavigationView&, DroneType, ui::external_app::enhanced_drone_analyzer::FilteredPresetMenuView)::FilteredPresetMenuView; Args = {std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >&, std::function<void(const DronePreset&, const std::vector<DronePreset, std::allocator<DronePreset> >&)>&, std::vector<DronePreset, std::allocator<DronePreset> >&}]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:658:71:   required from here
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:111:69: error: no matching function for call to 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_type_filtered_presets(ui::NavigationView&, DroneType, ui::external_app::enhanced_drone_analyzer::FilteredPresetMenuView)::FilteredPresetMenuView::FilteredPresetMenuView(ui::NavigationView&, std::vector<std::__cxx11::basic_string<char> >&, std::function<void(const DronePreset&, const std::vector<DronePreset>&)>&, std::vector<DronePreset>&)'
  111 |         return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
      |                                                                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:628:9: note: candidate: 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_type_filtered_presets(ui::NavigationView&, DroneType, ui::external_app::enhanced_drone_analyzer::FilteredPresetMenuView)::FilteredPresetMenuView::FilteredPresetMenuView(ui::NavigationView&, std::vector<std::__cxx11::basic_string<char> >, std::function<void(const DronePreset&)>, const std::vector<DronePreset>&)'
  628 |         FilteredPresetMenuView(NavigationView& nav, std::vector<std::string> names, std::function<void(const DronePreset&)> on_selected,
      |         ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:628:125: note:   no known conversion for argument 3 from 'function<void(const DronePreset&, const std::vector<DronePreset>&)>' to 'function<void(const DronePreset&)>'
  628 |         FilteredPresetMenuView(NavigationView& nav, std::vector<std::string> names, std::function<void(const DronePreset&)> on_selected,
      |                                                                                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5831: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_settings_combined.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.