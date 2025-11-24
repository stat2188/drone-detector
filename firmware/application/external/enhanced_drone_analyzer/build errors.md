Session Fix Summary - 24.11.2025:
- Fixed std::min type mismatch in progress calculation
- Fixed file.hpp std::filesystem::path constructor for const char*
- Removed problematic MenuView::push call
- Fixed std::string.c_str() format specifier issue

Remaining issues: Format specifier warnings (info only), initialization order warnings (cosmetic)
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_wideband_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:343:46: warning: narrowing conversion of '(Frequency)current_slice.WidebandSlice::center_frequency' from 'Frequency' {aka 'long long unsigned int'} to 'int64_t' {aka 'long long int'} [-Wnarrowing]
  343 |                 .frequency_a = current_slice.center_frequency,
      |                                ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:344:46: warning: narrowing conversion of '(Frequency)current_slice.WidebandSlice::center_frequency' from 'Frequency' {aka 'long long unsigned int'} to 'int64_t' {aka 'long long int'} [-Wnarrowing]
  344 |                 .frequency_b = current_slice.center_frequency,
      |                                ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneScanner::process_wideband_detection_with_override(const freqman_entry&, int32_t, int32_t, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:372:68: warning: unused parameter 'original_threshold' [-Wunused-parameter]
  372 |                                                            int32_t original_threshold, int32_t wideband_threshold) {
      |                                                            ~~~~~~~~^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::DroneDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:641:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
  641 | DroneDetectionLogger::DroneDetectionLogger()
      | ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:695:16: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
  695 |              "%u,%u,%d,%u,%u,%u,%.2f\n",
      |               ~^
      |                |
      |                unsigned int
      |               %lu
  696 |              entry.timestamp, entry.frequency_hz, entry.rssi_db,
      |              ~~~~~~~~~~~~~~~
      |                    |
      |                    uint32_t {aka long unsigned int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:695:19: warning: format '%u' expects argument of type 'unsigned int', but argument 5 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
  695 |              "%u,%u,%d,%u,%u,%u,%.2f\n",
      |                  ~^
      |                   |
      |                   unsigned int
      |                  %lu
  696 |              entry.timestamp, entry.frequency_hz, entry.rssi_db,
      |                               ~~~~~~~~~~~~~~~~~~
      |                                     |
      |                                     uint32_t {aka long unsigned int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:695:22: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
  695 |              "%u,%u,%d,%u,%u,%u,%.2f\n",
      |                     ~^
      |                      |
      |                      int
      |                     %ld
  696 |              entry.timestamp, entry.frequency_hz, entry.rssi_db,
      |                                                   ~~~~~~~~~~~~~
      |                                                         |
      |                                                         int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::format_session_summary(size_t, size_t) const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:235: warning: format '%u' expects argument of type 'unsigned int', but argument 9 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
  716 |     "SCANNING SESSION COMPLETE\n========================\n\nSESSION STATISTICS:\nDuration: %.1f seconds\nScan Cycles: %zu\nTotal Detections: %zu\n\nPERFORMANCE:\nAvg. detections/cycle: %.2f\nDetection rate: %.1f/sec\nLogged entries: %u\n\nEnhanced Drone Analyzer v0.3",
      |                                                                                                                                                                                                                                          ~^
      |                                                                                                                                                                                                                                           |
      |                                                                                                                                                                                                                                           unsigned int
      |                                                                                                                                                                                                                                          %lu
  717 |         static_cast<float>(session_duration_ms) / 1000.0f, scan_cycles, total_detections,
  718 |         avg_detections_per_cycle, detections_per_second, logged_count_);
      |                                                          ~~~~~~~~~~~~~                                                                                                                                                                     
      |                                                          |
      |                                                          uint32_t {aka long unsigned int}
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::DroneHardwareController(SpectrumMode)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:521:13: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::last_valid_rssi_' will be initialized after [-Wreorder]
  521 |     int32_t last_valid_rssi_ = -120;
      |             ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:515:18: warning:   'SpectrumMode ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_mode_' [-Wreorder]
  515 |     SpectrumMode spectrum_mode_;
      |                  ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:731:1: warning:   when initialized here [-Wreorder]
  731 | DroneHardwareController::DroneHardwareController(SpectrumMode mode)
      | ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:731:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_spectrum_config_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:731:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_frame_sync_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:731:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_spectrum_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In lambda function:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:774:110: warning: type qualifiers ignored on cast result type [-Wignored-qualifiers]
  774 |                 this->handle_channel_spectrum_config(static_cast<const ChannelSpectrumConfigMessage* const>(p));
      |                                                                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'virtual void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1012:17: warning: unused variable 'alpha' [-Wunused-variable]
 1012 |         uint8_t alpha = (pulse_timer % 20) < 10 ? 50 : 150;
      |                 ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'std::string ui::external_app::enhanced_drone_analyzer::ThreatCard::render_compact() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1063:64: warning: format '%d' expects argument of type 'int', but argument 7 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1063 |         snprintf(buffer, sizeof(buffer), "DR %-10s %c %5.1fG %4ddB",
      |                                                              ~~^
      |                                                                |
      |                                                                int
      |                                                              %4ld
 1064 |                 threat_name_.c_str(), *trend_symbol, freq_mhz, rssi_);
      |                                                                ~~~~~
      |                                                                |
      |                                                                int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1066:64: warning: format '%d' expects argument of type 'int', but argument 7 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1066 |         snprintf(buffer, sizeof(buffer), "DR %-10s %c %5.0fM %4ddB",
      |                                                              ~~^
      |                                                                |
      |                                                                int
      |                                                              %4ld
 1067 |                 threat_name_.c_str(), *trend_symbol, freq_mhz, rssi_);
      |                                                                ~~~~~
      |                                                                |
      |                                                                int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1055:17: warning: unused variable 'threat_abbr' [-Wunused-variable]
 1055 |     const char* threat_abbr = (threat_ == ThreatLevel::CRITICAL) ? "CRIT" :
      |                 ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1120:43: warning: format '%u' expects argument of type 'unsigned int', but argument 5 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1120 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:%u",
      |                                          ~^
      |                                           |
      |                                           unsigned int
      |                                          %lu
 1121 |             progress_bar, progress_percent, total_cycles, detections);
      |                           ~~~~~~~~~~~~~~~~ 
      |                           |
      |                           uint32_t {aka long unsigned int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1120:50: warning: format '%u' expects argument of type 'unsigned int', but argument 6 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1120 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:%u",
      |                                                 ~^
      |                                                  |
      |                                                  unsigned int
      |                                                 %lu
 1121 |             progress_bar, progress_percent, total_cycles, detections);
      |                                             ~~~~~~~~~~~~
      |                                             |
      |                                             uint32_t {aka long unsigned int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1120:55: warning: format '%u' expects argument of type 'unsigned int', but argument 7 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1120 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:%u",
      |                                                      ~^
      |                                                       |
      |                                                       unsigned int
      |                                                      %lu
 1121 |             progress_bar, progress_percent, total_cycles, detections);
      |                                                           ~~~~~~~~~~
      |                                                           |
      |                                                           uint32_t {aka long unsigned int}
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:699:21: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::nav_' will be initialized after [-Wreorder]
  699 |     NavigationView& nav_;
      |                     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:686:14: warning:   'Gradient ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_gradient_' [-Wreorder]
  686 |     Gradient spectrum_gradient_;
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1191:1: warning:   when initialized here [-Wreorder]
 1191 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:686:14: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_gradient_' will be initialized after [-Wreorder]
  686 |     Gradient spectrum_gradient_;
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:667:10: warning:   'ui::Text ui::external_app::enhanced_drone_analyzer::DroneDisplayController::big_display_' [-Wreorder]
  667 |     Text big_display_{{4, 6 * 16, 28 * 8, 52}, ""};
      |          ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1191:1: warning:   when initialized here [-Wreorder]
 1191 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:693:14: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::marker_pixel_step' will be initialized after [-Wreorder]
  693 |     uint32_t marker_pixel_step = 1000000;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:691:13: warning:   'uint8_t ui::external_app::enhanced_drone_analyzer::DroneDisplayController::min_color_power' [-Wreorder]
  691 |     uint8_t min_color_power = 0;
      |             ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1191:1: warning:   when initialized here [-Wreorder]
 1191 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:698:20: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_config_' will be initialized after [-Wreorder]
  698 |     SpectrumConfig spectrum_config_;
      |                    ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:687:26: warning:   'ChannelSpectrumFIFO* ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_fifo_' [-Wreorder]
  687 |     ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
      |                          ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1191:1: warning:   when initialized here [-Wreorder]
 1191 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1191:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::detected_drones_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1191:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::displayed_drones_' should be initialized in the member initialization list [-Weffc++]
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1203:81: note: synthesized method 'std::array<DisplayDroneEntry, 3>::array()' first required here
 1203 |       mode(LOOKING_GLASS_SINGLEPASS), spectrum_config_(), spectrum_fifo_(nullptr)
      |                                                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_detection_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1243:68: warning: format '%s' expects argument of type 'char*', but argument 4 has type 'std::string' {aka 'std::__cxx11::basic_string<char>'} [-Wformat=]
 1243 |         snprintf(summary_buffer, sizeof(summary_buffer), "THREAT: %s | <%zu ~%zu >%zu",
      |                                                                   ~^
      |                                                                    |
      |                                                                    char*
 1244 |                 get_threat_level_name(max_threat), scanner.get_approaching_count(),
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                   
      |                                      |
      |                                      std::string {aka std::__cxx11::basic_string<char>}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1329:72: warning: unused parameter 'scanner' [-Wunused-parameter]
 1329 | void DroneDisplayController::update_drones_display(const DroneScanner& scanner) {
      |                                                    ~~~~~~~~~~~~~~~~~~~~^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1374:42: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1374 |         snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
      |                                          ^~~~~~~~~~~~~~~~~~~~
......
 1377 |                 drone.rssi,
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
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1764:68: warning: narrowing conversion of 'card_y_pos' from 'size_t' {aka 'unsigned int'} to 'int' [-Wnarrowing]
 1764 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                    ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1794:81: error: no matching function for call to 'min(uint32_t, unsigned int)'
 1794 |             uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), 100u);
      |                                                                                 ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)'
  198 |     min(const _Tp& __a, const _Tp& __b)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1794:81: note:   deduced conflicting types for parameter 'const _Tp' ('long unsigned int' and 'unsigned int')
 1794 |             uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), 100u);
      |                                                                                 ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)'
  246 |     min(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1794:81: note:   deduced conflicting types for parameter 'const _Tp' ('long unsigned int' and 'unsigned int')
 1794 |             uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), 100u);
      |                                                                                 ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5: note: candidate: 'template<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)'
 3444 |     min(initializer_list<_Tp> __l)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1794:81: note:   mismatched types 'std::initializer_list<_Tp>' and 'long unsigned int'
 1794 |             uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), 100u);
      |                                                                                 ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)'
 3450 |     min(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1794:81: note:   mismatched types 'std::initializer_list<_Tp>' and 'long unsigned int'
 1794 |             uint32_t progress = std::min(static_cast<uint32_t>(cycles * 5), 100u);
      |                                                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1806:90: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1806 |                 snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: %u", total_detections);
      |                                                                                         ~^   ~~~~~~~~~~~~~~~~
      |                                                                                          |   |
      |                                                                                          |   uint32_t {aka long unsigned int}
      |                                                                                          unsigned int
      |                                                                                         %lu
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1836:62: warning: format '%d' expects argument of type 'int', but argument 7 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1836 |             snprintf(buffer, sizeof(buffer), "%s %c %.1fMHz %ddB",
      |                                                             ~^
      |                                                              |
      |                                                              int
      |                                                             %ld
 1837 |                     type_name.c_str(), trend_symbol, freq_mhz, drone.rssi);
      |                                                                ~~~~~~~~~~
      |                                                                      |
      |                                                                      int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1852:25: warning: unused variable 'empty_text' [-Wunused-variable]
 1852 |             const char* empty_text = "Drone X: None";
      |                         ^~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp: In instantiation of 'std::filesystem::path::path(const Source&) [with Source = const char*]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/havoc/firmware/application/./file.hpp:89:26: error: no matching function for call to 'begin(const char* const&)'
   89 |         : path{std::begin(source), std::end(source)} {
      |                ~~~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5: note: candidate: 'template<class _Tp> constexpr const _Tp* std::begin(std::initializer_list<_Tp>)'
   89 |     begin(initializer_list<_Tp> __ils) noexcept
      |     ^~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:26: note:   mismatched types 'std::initializer_list<_Tp>' and 'const char*'
   89 |         : path{std::begin(source), std::end(source)} {
      |                ~~~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5: note: candidate: 'template<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&)'
   48 |     begin(_Container& __cont) -> decltype(__cont.begin())
      |     ^~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5: note:   template argument deduction/substitution failed:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of 'template<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&) [with _Container = const char* const]':
/havoc/firmware/application/./file.hpp:89:26:   required from 'std::filesystem::path::path(const Source&) [with Source = const char*]'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:50: error: request for member 'begin' in '__cont', which is of non-class type 'const char* const'
   48 |     begin(_Container& __cont) -> decltype(__cont.begin())
      |                                           ~~~~~~~^~~~~
/havoc/firmware/application/./file.hpp: In instantiation of 'std::filesystem::path::path(const Source&) [with Source = const char*]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5: note: candidate: 'template<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&)'
   58 |     begin(const _Container& __cont) -> decltype(__cont.begin())
      |     ^~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5: note:   template argument deduction/substitution failed:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of 'template<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&) [with _Container = const char*]':
/havoc/firmware/application/./file.hpp:89:26:   required from 'std::filesystem::path::path(const Source&) [with Source = const char*]'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:56: error: request for member 'begin' in '__cont', which is of non-class type 'const char* const'
   58 |     begin(const _Container& __cont) -> decltype(__cont.begin())
      |                                                 ~~~~~~~^~~~~
/havoc/firmware/application/./file.hpp: In instantiation of 'std::filesystem::path::path(const Source&) [with Source = const char*]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5: note: candidate: 'template<class _Tp, unsigned int _Nm> constexpr _Tp* std::begin(_Tp (&)[_Nm])'
   87 |     begin(_Tp (&__arr)[_Nm])
      |     ^~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:26: note:   mismatched types '_Tp [_Nm]' and 'const char* const'
   89 |         : path{std::begin(source), std::end(source)} {
      |                ~~~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31: note: candidate: 'template<class _Tp> _Tp* std::begin(std::valarray<_Tp>&)'
  104 |   template<typename _Tp> _Tp* begin(valarray<_Tp>&);
      |                               ^~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:26: note:   mismatched types 'std::valarray<_Tp>' and 'const char* const'
   89 |         : path{std::begin(source), std::end(source)} {
      |                ~~~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37: note: candidate: 'template<class _Tp> const _Tp* std::begin(const std::valarray<_Tp>&)'
  105 |   template<typename _Tp> const _Tp* begin(const valarray<_Tp>&);
      |                                     ^~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:26: note:   mismatched types 'const std::valarray<_Tp>' and 'const char* const'
   89 |         : path{std::begin(source), std::end(source)} {
      |                ~~~~~~~~~~^~~~~~~~
/havoc/firmware/application/./file.hpp:89:44: error: no matching function for call to 'end(const char* const&)'
   89 |         : path{std::begin(source), std::end(source)} {
      |                                    ~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36,
                 from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5: note: candidate: 'template<class _Tp> constexpr const _Tp* std::end(std::initializer_list<_Tp>)'
   99 |     end(initializer_list<_Tp> __ils) noexcept
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:44: note:   mismatched types 'std::initializer_list<_Tp>' and 'const char*'
   89 |         : path{std::begin(source), std::end(source)} {
      |                                    ~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5: note: candidate: 'template<class _Container> constexpr decltype (__cont.end()) std::end(_Container&)'
   68 |     end(_Container& __cont) -> decltype(__cont.end())
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5: note:   template argument deduction/substitution failed:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of 'template<class _Container> constexpr decltype (__cont.end()) std::end(_Container&) [with _Container = const char* const]':
/havoc/firmware/application/./file.hpp:89:44:   required from 'std::filesystem::path::path(const Source&) [with Source = const char*]'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:48: error: request for member 'end' in '__cont', which is of non-class type 'const char* const'
   68 |     end(_Container& __cont) -> decltype(__cont.end())
      |                                         ~~~~~~~^~~
/havoc/firmware/application/./file.hpp: In instantiation of 'std::filesystem::path::path(const Source&) [with Source = const char*]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5: note: candidate: 'template<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&)'
   78 |     end(const _Container& __cont) -> decltype(__cont.end())
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5: note:   template argument deduction/substitution failed:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of 'template<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&) [with _Container = const char*]':
/havoc/firmware/application/./file.hpp:89:44:   required from 'std::filesystem::path::path(const Source&) [with Source = const char*]'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:54: error: request for member 'end' in '__cont', which is of non-class type 'const char* const'
   78 |     end(const _Container& __cont) -> decltype(__cont.end())
      |                                               ~~~~~~~^~~
/havoc/firmware/application/./file.hpp: In instantiation of 'std::filesystem::path::path(const Source&) [with Source = const char*]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:61:   required from here
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5: note: candidate: 'template<class _Tp, unsigned int _Nm> constexpr _Tp* std::end(_Tp (&)[_Nm])'
   97 |     end(_Tp (&__arr)[_Nm])
      |     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:44: note:   mismatched types '_Tp [_Nm]' and 'const char* const'
   89 |         : path{std::begin(source), std::end(source)} {
      |                                    ~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31: note: candidate: 'template<class _Tp> _Tp* std::end(std::valarray<_Tp>&)'
  106 |   template<typename _Tp> _Tp* end(valarray<_Tp>&);
      |                               ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:44: note:   mismatched types 'std::valarray<_Tp>' and 'const char* const'
   89 |         : path{std::begin(source), std::end(source)} {
      |                                    ~~~~~~~~^~~~~~~~
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37: note: candidate: 'template<class _Tp> const _Tp* std::end(const std::valarray<_Tp>&)'
  107 |   template<typename _Tp> const _Tp* end(const valarray<_Tp>&);
      |                                     ^~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37: note:   template argument deduction/substitution failed:
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/./file.hpp:89:44: note:   mismatched types 'const std::valarray<_Tp>' and 'const char* const'
   89 |         : path{std::begin(source), std::end(source)} {
      |                                    ~~~~~~~~^~~~~~~~
/havoc/firmware/application/./file.hpp:89:52: error: no matching function for call to 'std::filesystem::path::path(<brace-enclosed initializer list>)'
   89 |         : path{std::begin(source), std::end(source)} {
      |                                                    ^
/havoc/firmware/application/./file.hpp:102:5: note: candidate: 'std::filesystem::path::path(const TCHAR*)'
  102 |     path(const TCHAR* const s)
      |     ^~~~
/havoc/firmware/application/./file.hpp:102:5: note:   candidate expects 1 argument, 2 provided
/havoc/firmware/application/./file.hpp:98:5: note: candidate: 'std::filesystem::path::path(const char16_t*)'
   98 |     path(const char16_t* const s)
      |     ^~~~
/havoc/firmware/application/./file.hpp:98:5: note:   candidate expects 1 argument, 2 provided
/havoc/firmware/application/./file.hpp:93:5: note: candidate: 'template<class InputIt> std::filesystem::path::path(InputIt, InputIt)'
   93 |     path(InputIt first,
      |     ^~~~
/havoc/firmware/application/./file.hpp:93:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/./file.hpp:88:5: note: candidate: 'template<class Source> std::filesystem::path::path(const Source&)'
   88 |     path(const Source& source)
      |     ^~~~
/havoc/firmware/application/./file.hpp:88:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/./file.hpp:83:5: note: candidate: 'std::filesystem::path::path(std::filesystem::path&&)'
   83 |     path(path&& p)
      |     ^~~~
/havoc/firmware/application/./file.hpp:83:5: note:   candidate expects 1 argument, 2 provided
/havoc/firmware/application/./file.hpp:79:5: note: candidate: 'std::filesystem::path::path(const std::filesystem::path&)'
   79 |     path(const path& p)
      |     ^~~~
/havoc/firmware/application/./file.hpp:79:5: note:   candidate expects 1 argument, 2 provided
/havoc/firmware/application/./file.hpp:75:5: note: candidate: 'std::filesystem::path::path()'
   75 |     path()
      |     ^~~~
/havoc/firmware/application/./file.hpp:75:5: note:   candidate expects 0 arguments, 2 provided
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:7,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp: In instantiation of 'T* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}]':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1559:42:   required from here
/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:111:69: error: no matching function for call to 'ui::MenuView::MenuView(ui::NavigationView&)'
  111 |         return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
      |                                                                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:37,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:7,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:30,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/ui/ui_menu.hpp:81:5: note: candidate: 'ui::MenuView::MenuView(ui::Rect, bool)'
   81 |     MenuView(Rect new_parent_rect = {0, 0, screen_width, screen_height - 16},
      |     ^~~~~~~~
/havoc/firmware/application/ui/ui_menu.hpp:81:19: note:   no known conversion for argument 1 from 'ui::NavigationView' to 'ui::Rect'
   81 |     MenuView(Rect new_parent_rect = {0, 0, screen_width, screen_height - 16},
      |              ~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5789: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.
