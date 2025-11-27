[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In lambda function:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:749:110: warning: type qualifiers ignored on cast result type [-Wignored-qualifiers]
  749 |                 this->handle_channel_spectrum_config(static_cast<const ChannelSpectrumConfigMessage* const>(p));
      |                                                                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1358:42: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1358 |         snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
      |                                          ^~~~~~~~~~~~~~~~~~~~
......
 1361 |                 (long int)drone.rssi,
      |                 ~~~~~~~~~~~~~~~~~~~~      
      |                 |
      |                 int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1103:53: warning: ' D:' directive output may be truncated writing 3 bytes into a region of size between 0 and 25 [-Wformat-truncation=]
 1103 |     snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
      |                                                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1103:13: note: 'snprintf' output between 12 and 63 bytes into a destination of size 32
 1103 |     snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
      |     ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1104 |             progress_bar, (unsigned long)progress_percent, (unsigned long)total_cycles, (unsigned long)detections);
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::EnhancedSettingsManager::save_settings_to_txt(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:45:63: error: exception handling disabled, use '-fexceptions' to enable
   45 |                 throw std::runtime_error("Header write failed");
      |                                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static void ui::external_app::enhanced_drone_analyzer::EnhancedSettingsManager::create_backup_file(const string&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:111:90: error: no matching function for call to 'min(size_t, File::Size)'
  111 |                     size_t to_read = std::min(size_t(1024), orig_file.size() - total_read);
      |                                                                                          ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)'
  198 |     min(const _Tp& __a, const _Tp& __b)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:111:90: note:   deduced conflicting types for parameter 'const _Tp' ('unsigned int' and 'File::Size' {aka 'long long unsigned int'})
  111 |                     size_t to_read = std::min(size_t(1024), orig_file.size() - total_read);
      |                                                                                          ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)'
  246 |     min(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:111:90: note:   deduced conflicting types for parameter 'const _Tp' ('unsigned int' and 'File::Size' {aka 'long long unsigned int'})
  111 |                     size_t to_read = std::min(size_t(1024), orig_file.size() - total_read);
      |                                                                                          ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/functional:65,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:16,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5: note: candidate: 'template<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)'
 3444 |     min(initializer_list<_Tp> __l)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:111:90: note:   mismatched types 'std::initializer_list<_Tp>' and 'unsigned int'
  111 |                     size_t to_read = std::min(size_t(1024), orig_file.size() - total_read);
      |                                                                                          ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/functional:65,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:16,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)'
 3450 |     min(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:111:90: note:   mismatched types 'std::initializer_list<_Tp>' and 'unsigned int'
  111 |                     size_t to_read = std::min(size_t(1024), orig_file.size() - total_read);
      |                                                                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static std::string ui::external_app::enhanced_drone_analyzer::EnhancedSettingsManager::generate_settings_content(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:185:43: error: 'const struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'freqman_path'
  185 |         ss << "freqman_path=" << settings.freqman_path << "\n";
      |                                           ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettingsManager::load(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:241:18: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'settings_file_path'
  241 |     if (settings.settings_file_path.empty()) {
      |                  ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:242:18: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'settings_file_path'
  242 |         settings.settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
      |                  ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:246:29: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'settings_file_path'
  246 |     if (!file.open(settings.settings_file_path, true)) {  // true = read_only
      |                             ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static bool ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettingsManager::save(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:302:18: error: 'const struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'settings_file_path'
  302 |     if (settings.settings_file_path.empty()) {
      |                  ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:304:54: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'settings_file_path'
  304 |         const_cast<DroneAnalyzerSettings&>(settings).settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
      |                                                      ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:308:29: error: 'const struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'settings_file_path'
  308 |     if (!file.open(settings.settings_file_path, false)) {  // false = write mode
      |                             ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:331:43: error: 'const struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'freqman_path'
  331 |     content += "freqman_path=" + settings.freqman_path + "\n";
      |                                           ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static void ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettingsManager::reset_to_defaults(ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:345:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'enable_wideband_scanning'
  345 |     settings.enable_wideband_scanning = false;
      |              ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:346:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'wideband_min_freq_hz'
  346 |     settings.wideband_min_freq_hz = WIDEBAND_DEFAULT_MIN;
      |              ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:347:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'wideband_max_freq_hz'
  347 |     settings.wideband_max_freq_hz = WIDEBAND_DEFAULT_MAX;
      |              ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:348:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'min_frequency_hz'
  348 |     settings.min_frequency_hz = 2400000000ULL;
      |              ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:349:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'max_frequency_hz'
  349 |     settings.max_frequency_hz = 2500000000ULL;
      |              ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:350:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'show_detailed_info'
  350 |     settings.show_detailed_info = true;
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:351:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'auto_save_logs'
  351 |     settings.auto_save_logs = true;
      |              ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:352:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'log_file_path'
  352 |     settings.log_file_path = "/eda_logs";
      |              ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:353:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'freqman_path'
  353 |     settings.freqman_path = "DRONES";
      |              ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:354:14: error: 'struct ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings' has no member named 'settings_file_path'
  354 |     settings.settings_file_path = "/sdcard/ENHANCED_DRONE_ANALYZER_SETTINGS.txt";
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In static member function 'static std::string ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettingsManager::serialize(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:373:12: warning: enumeration value 'ULTRA_NARROW' not handled in switch [-Wswitch]
  373 |     switch (settings.spectrum_mode) {
      |            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::ScannerConfig::save_to_file(const string&) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:475:12: warning: enumeration value 'ULTRA_NARROW' not handled in switch [-Wswitch]
  475 |     switch (config_data_.spectrum_mode) {
      |            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_preset_menu(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::PresetMenuView)::PresetMenuView::PresetMenuView(std::vector<std::__cxx11::basic_string<char> >, std::function<void(const DronePreset&)>, const std::vector<DronePreset>&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:591:48: warning: missing initializer for member 'ui::MenuItem::bitmap' [-Wmissing-field-initializers]
  591 |                 add_item({name, Color::white()});
      |                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:591:48: warning: missing initializer for member 'ui::MenuItem::on_select' [-Wmissing-field-initializers]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_type_filtered_presets(ui::NavigationView&, DroneType, ui::external_app::enhanced_drone_analyzer::FilteredPresetMenuView)::FilteredPresetMenuView::FilteredPresetMenuView(std::vector<std::__cxx11::basic_string<char> >, std::function<void(const DronePreset&)>, const std::vector<DronePreset>&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:630:48: warning: missing initializer for member 'ui::MenuItem::bitmap' [-Wmissing-field-initializers]
  630 |                 add_item({name, Color::white()});
      |                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:630:48: warning: missing initializer for member 'ui::MenuItem::on_select' [-Wmissing-field-initializers]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:740:13: error: redefinition of 'std::string ui::external_app::enhanced_drone_analyzer::HardwareSettingsView::title() const'
  740 | std::string HardwareSettingsView::title() const {
      |             ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:204:17: note: 'virtual std::string ui::external_app::enhanced_drone_analyzer::HardwareSettingsView::title() const' previously defined here
  204 |     std::string title() const override { return "Hardware Settings"; }
      |                 ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::AudioSettingsView::AudioSettingsView(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:799:1: warning: 'ui::external_app::enhanced_drone_analyzer::AudioSettingsView::audio_settings_' should be initialized in the member initialization list [-Weffc++]
  799 | AudioSettingsView::AudioSettingsView(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:808:13: error: redefinition of 'std::string ui::external_app::enhanced_drone_analyzer::AudioSettingsView::title() const'
  808 | std::string AudioSettingsView::title() const {
      |             ^~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:169:17: note: 'virtual std::string ui::external_app::enhanced_drone_analyzer::AudioSettingsView::title() const' previously defined here
  169 |     std::string title() const override { return "Audio Settings"; }
      |                 ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:864:13: error: redefinition of 'std::string ui::external_app::enhanced_drone_analyzer::LoadingView::title() const'
  864 | std::string LoadingView::title() const {
      |             ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:372:17: note: 'virtual std::string ui::external_app::enhanced_drone_analyzer::LoadingView::title() const' previously defined here
  372 |     std::string title() const override { return "Loading"; }
      |                 ^~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:7,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp: In instantiation of 'T* ui::NavigationView::push(Args&& ...) [with T = ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_preset_menu(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::PresetMenuView)::PresetMenuView; Args = {std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >&, std::function<void(const DronePreset&)>&, std::vector<DronePreset, std::allocator<DronePreset> >&}]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:614:65:   required from here
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:111:69: error: no matching function for call to 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_preset_menu(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::PresetMenuView)::PresetMenuView::PresetMenuView(ui::NavigationView&, std::vector<std::__cxx11::basic_string<char> >&, std::function<void(const DronePreset&)>&, std::vector<DronePreset>&)'
  111 |         return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
      |                                                                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:587:9: note: candidate: 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_preset_menu(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::PresetMenuView)::PresetMenuView::PresetMenuView(std::vector<std::__cxx11::basic_string<char> >, std::function<void(const DronePreset&)>, const std::vector<DronePreset>&)'
  587 |         PresetMenuView(std::vector<std::string> names, std::function<void(const DronePreset&)> on_selected,
      |         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:587:9: note:   candidate expects 3 arguments, 4 provided
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:7,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.hpp:21,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp: In instantiation of 'T* ui::NavigationView::push(Args&& ...) [with T = ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_type_filtered_presets(ui::NavigationView&, DroneType, ui::external_app::enhanced_drone_analyzer::FilteredPresetMenuView)::FilteredPresetMenuView; Args = {std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >&, std::function<void(const DronePreset&, const std::vector<DronePreset, std::allocator<DronePreset> >&)>&, std::vector<DronePreset, std::allocator<DronePreset> >&}]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:653:71:   required from here
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:111:69: error: no matching function for call to 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_type_filtered_presets(ui::NavigationView&, DroneType, ui::external_app::enhanced_drone_analyzer::FilteredPresetMenuView)::FilteredPresetMenuView::FilteredPresetMenuView(ui::NavigationView&, std::vector<std::__cxx11::basic_string<char> >&, std::function<void(const DronePreset&, const std::vector<DronePreset>&)>&, std::vector<DronePreset>&)'
  111 |         return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
      |                                                                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:626:9: note: candidate: 'ui::external_app::enhanced_drone_analyzer::DronePresetSelector::show_type_filtered_presets(ui::NavigationView&, DroneType, ui::external_app::enhanced_drone_analyzer::FilteredPresetMenuView)::FilteredPresetMenuView::FilteredPresetMenuView(std::vector<std::__cxx11::basic_string<char> >, std::function<void(const DronePreset&)>, const std::vector<DronePreset>&)'
  626 |         FilteredPresetMenuView(std::vector<std::string> names, std::function<void(const DronePreset&)> on_selected,
      |         ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_settings_combined.cpp:626:9: note:   candidate expects 3 arguments, 4 provided
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5831: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_settings_combined.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.