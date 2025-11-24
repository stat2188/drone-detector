[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::process_wideband_detection_with_override(const freqman_entry&, int32_t, int32_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:350:68: warning: unused parameter 'original_threshold' [-Wunused-parameter]
  350 |                                                            int32_t original_threshold, int32_t wideband_threshold) {
      |                                                            ~~~~~~~~^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::DroneDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:618:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
  618 | DroneDetectionLogger::DroneDetectionLogger()
      | ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::DroneHardwareController(SpectrumMode)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:709:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_channel_statistics_' should be initialized in the member initialization list [-Weffc++]
  709 | DroneHardwareController::DroneHardwareController(SpectrumMode mode)
      | ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In lambda function:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:753:110: warning: type qualifiers ignored on cast result type [-Wignored-qualifiers]
  753 |                 this->handle_channel_spectrum_config(static_cast<const ChannelSpectrumConfigMessage* const>(p));
      |                                                                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1013:17: warning: unused variable 'alpha' [-Wunused-variable]
 1013 |         uint8_t alpha = (pulse_timer % 20) < 10 ? 50 : 150;
      |                 ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::ThreatCard::render_compact() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1056:17: warning: unused variable 'threat_abbr' [-Wunused-variable]
 1056 |     const char* threat_abbr = (threat_ == ThreatLevel::CRITICAL) ? "CRIT" :
      |                 ^~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:695:14: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::marker_pixel_step' will be initialized after [-Wreorder]
  695 |     uint32_t marker_pixel_step = 1000000;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:693:13: warning:   'uint8_t ui::external_app::enhanced_drone_analyzer::DroneDisplayController::min_color_power' [-Wreorder]
  693 |     uint8_t min_color_power = 0;
      |             ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1192:1: warning:   when initialized here [-Wreorder]
 1192 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:700:20: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_config_' will be initialized after [-Wreorder]
  700 |     SpectrumConfig spectrum_config_;
      |                    ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:689:26: warning:   'ChannelSpectrumFIFO* ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_fifo_' [-Wreorder]
  689 |     ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
      |                          ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1192:1: warning:   when initialized here [-Wreorder]
 1192 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:21: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::nav_' will be initialized after [-Wreorder]
  701 |     NavigationView& nav_;
      |                     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:688:14: warning:   'Gradient ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_gradient_' [-Wreorder]
  688 |     Gradient spectrum_gradient_;
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1192:1: warning:   when initialized here [-Wreorder]
 1192 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1192:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::detected_drones_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1192:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::displayed_drones_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'DisplayDroneEntry::DisplayDroneEntry()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::frequency' should be initialized in the member initialization list [-Weffc++]
  173 | struct DisplayDroneEntry {
      |        ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::type' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::threat' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::rssi' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::last_seen' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::type_name' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::display_color' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:173:8: warning: 'DisplayDroneEntry::trend' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:15,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array: In constructor 'std::array<DisplayDroneEntry, 3>::array()':
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array:94:12: note: synthesized method 'DisplayDroneEntry::DisplayDroneEntry()' first required here
   94 |     struct array
      |            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1205:37: note: synthesized method 'std::array<DisplayDroneEntry, 3>::array()' first required here
 1205 |       nav_(nav), spectrum_gradient_()
      |                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1332:72: warning: unused parameter 'scanner' [-Wunused-parameter]
 1332 | void DroneDisplayController::update_drones_display(const DroneScanner& scanner) {
      |                                                    ~~~~~~~~~~~~~~~~~~~~^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1377:42: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1377 |         snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
      |                                          ^~~~~~~~~~~~~~~~~~~~
......
 1380 |                 drone.rssi,
      |                 ~~~~~~~~~~                
      |                       |
      |                       int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_set_bandwidth()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1605:40: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1605 |     snprintf(buffer, sizeof(buffer), "%u", current_bw);
      |                                       ~^   ~~~~~~~~~~
      |                                        |   |
      |                                        |   uint32_t {aka long unsigned int}
      |                                        unsigned int
      |                                       %lu
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_hardware_status()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1619:21: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'long unsigned int' [-Wformat=]
 1619 |             "Band: %u MHz\nFreq: %.3f GHz",
      |                    ~^
      |                     |
      |                     unsigned int
      |                    %lu
 1620 |             hardware_.get_spectrum_bandwidth() / 1000000,
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                |
      |                                                long unsigned int
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1757:68: warning: narrowing conversion of 'card_y_pos' from 'size_t' {aka 'unsigned int'} to 'int' [-Wnarrowing]
 1757 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                    ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1799:90: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1799 |                 snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: %u", total_detections);
      |                                                                                         ~^   ~~~~~~~~~~~~~~~~
      |                                                                                          |   |
      |                                                                                          |   uint32_t {aka long unsigned int}
      |                                                                                          unsigned int
      |                                                                                         %lu
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1121:53: warning: ' D:' directive output may be truncated writing 3 bytes into a region of size between 0 and 25 [-Wformat-truncation=]
 1121 |     snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
      |                                                     ^~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1121:13: note: 'snprintf' output between 12 and 63 bytes into a destination of size 32
 1121 |     snprintf(buffer, sizeof(buffer), "%s %lu%% C:%lu D:%lu",
      |     ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1122 |             progress_bar, (unsigned long)progress_percent, (unsigned long)total_cycles, (unsigned long)detections);
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:4:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::AmplifierControl::is_valid_lna_gain() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:56:57: warning: comparison is always true due to limited range of data type [-Wtype-limits]
   56 |     bool is_valid_lna_gain() const { return lna_gain_db >= 0 && lna_gain_db <= 63; }
      |                                             ~~~~~~~~~~~~^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::AmplifierControl::is_valid_vga_gain() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:57:57: warning: comparison is always true due to limited range of data type [-Wtype-limits]
   57 |     bool is_valid_vga_gain() const { return vga_gain_db >= 0 && vga_gain_db <= 62; }
      |                                             ~~~~~~~~~~~~^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::AmplifierControl::set_lna_gain(uint8_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:60:18: warning: comparison is always true due to limited range of data type [-Wtype-limits]
   60 |         if (gain >= 0 && gain <= 63) lna_gain_db = gain;
      |             ~~~~~^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp: In member function 'void ui::external_app::enhanced_drone_analyzer::AmplifierControl::set_vga_gain(uint8_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.hpp:64:18: warning: comparison is always true due to limited range of data type [-Wtype-limits]
   64 |         if (gain >= 0 && gain <= 62) vga_gain_db = gain;
      |             ~~~~~^~~~
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::SpectrumPresetLoader::parse_preset_line(const string&, ui::external_app::enhanced_drone_analyzer::FrequencyPreset&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:108:19: warning: comparison of integer expressions of different signedness: 'rf::Frequency' {aka 'long long int'} and 'long long unsigned int' [-Wsign-compare]
  108 |         if (range > 500000000ULL) preset.spectrum_mode = SpectrumMode::ULTRA_WIDE;
      |             ~~~~~~^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:109:24: warning: comparison of integer expressions of different signedness: 'rf::Frequency' {aka 'long long int'} and 'long long unsigned int' [-Wsign-compare]
  109 |         else if (range > 100000000ULL) preset.spectrum_mode = SpectrumMode::WIDE;
      |                  ~~~~~~^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp:113:34: error: exception handling disabled, use '-fexceptions' to enable
  113 |     } catch (const std::exception&) {
      |                                  ^
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5803: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_spectrum_settings.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.