[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::DroneDetectionLogger()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:643:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::csv_log_' should be initialized in the member initialization list [-Weffc++]
  643 | DroneDetectionLogger::DroneDetectionLogger()
      | ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::DroneHardwareController(SpectrumMode)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:734:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_channel_statistics_' should be initialized in the member initialization list [-Weffc++]
  734 | DroneHardwareController::DroneHardwareController(SpectrumMode mode)
      | ^~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In lambda function:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:778:110: warning: type qualifiers ignored on cast result type [-Wignored-qualifiers]
  778 |                 this->handle_channel_spectrum_config(static_cast<const ChannelSpectrumConfigMessage* const>(p));
      |                                                                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:896:54: error: definition of 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::SmartThreatHeader(ui::Rect)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
  896 | SmartThreatHeader::SmartThreatHeader(Rect parent_rect)
      |                                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:906:89: error: definition of 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::update(ThreatLevel, size_t, size_t, size_t, Frequency, bool)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
  906 |                                size_t receding, Frequency current_freq, bool is_scanning) {
      |                                                                                         ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:974:58: error: definition of 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::set_max_threat(ThreatLevel)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
  974 | void SmartThreatHeader::set_max_threat(ThreatLevel threat) {
      |                                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:981:101: error: definition of 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::set_movement_counts(size_t, size_t, size_t)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
  981 | void SmartThreatHeader::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
      |                                                                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:986:61: error: definition of 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::set_current_frequency(Frequency)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
  986 | void SmartThreatHeader::set_current_frequency(Frequency freq) {
      |                                                             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:993:60: error: definition of 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::set_scanning_state(bool)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
  993 | void SmartThreatHeader::set_scanning_state(bool is_scanning) {
      |                                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1000:61: error: definition of 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::set_color_scheme(bool)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
 1000 | void SmartThreatHeader::set_color_scheme(bool use_dark_theme) {
      |                                                             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1004:66: error: definition of 'ui::Color ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::get_threat_bar_color(ThreatLevel) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
 1004 | Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) const {
      |                                                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1015:67: error: definition of 'ui::Color ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::get_threat_text_color(ThreatLevel) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
 1015 | Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) const {
      |                                                                   ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:72: error: definition of 'std::string ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::get_threat_icon_text(ThreatLevel) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
 1026 | std::string SmartThreatHeader::get_threat_icon_text(ThreatLevel level) const {
      |                                                                        ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1037:47: error: definition of 'void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::paint(ui::Painter&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' [-fpermissive]
 1037 | void SmartThreatHeader::paint(Painter& painter) {
      |                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1054:59: error: definition of 'ui::external_app::enhanced_drone_analyzer::ThreatCard::ThreatCard(size_t, ui::Rect)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ThreatCard' [-fpermissive]
 1054 | ThreatCard::ThreatCard(size_t card_index, Rect parent_rect)
      |                                                           ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1059:60: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ThreatCard::update_card(const DisplayDroneEntry&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ThreatCard' [-fpermissive]
 1059 | void ThreatCard::update_card(const DisplayDroneEntry& drone) {
      |                                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1073:29: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ThreatCard::clear_card()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ThreatCard' [-fpermissive]
 1073 | void ThreatCard::clear_card() {
      |                             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1079:42: error: definition of 'std::string ui::external_app::enhanced_drone_analyzer::ThreatCard::render_compact() const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ThreatCard' [-fpermissive]
 1079 | std::string ThreatCard::render_compact() const {
      |                                          ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1099:39: error: definition of 'ui::Color ui::external_app::enhanced_drone_analyzer::ThreatCard::get_card_bg_color() const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ThreatCard' [-fpermissive]
 1099 | Color ThreatCard::get_card_bg_color() const {
      |                                       ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1111:41: error: definition of 'ui::Color ui::external_app::enhanced_drone_analyzer::ThreatCard::get_card_text_color() const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ThreatCard' [-fpermissive]
 1111 | Color ThreatCard::get_card_text_color() const {
      |                                         ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1123:40: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ThreatCard::paint(ui::Painter&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ThreatCard' [-fpermissive]
 1123 | void ThreatCard::paint(Painter& painter) {
      |                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1131:70: error: definition of 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::ConsoleStatusBar(size_t, ui::Rect)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar' [-fpermissive]
 1131 | ConsoleStatusBar::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
      |                                                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1137:118: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar' [-fpermissive]
 1137 | void ConsoleStatusBar::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
      |                                                                                                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1162:113: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_alert_status(ThreatLevel, size_t, const string&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar' [-fpermissive]
 1162 | void ConsoleStatusBar::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
      |                                                                                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1181:101: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_normal_status(const string&, const string&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar' [-fpermissive]
 1181 | void ConsoleStatusBar::update_normal_status(const std::string& primary, const std::string& secondary) {
      |                                                                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1195:57: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::set_display_mode(ui::external_app::enhanced_drone_analyzer::DisplayMode)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar' [-fpermissive]
 1195 | void ConsoleStatusBar::set_display_mode(DisplayMode mode) {
      |                                                         ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1211:46: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::paint(ui::Painter&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar' [-fpermissive]
 1211 | void ConsoleStatusBar::paint(Painter& painter) {
      |                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1218:67: error: definition of 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1218 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      |                                                                   ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:734:14: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::marker_pixel_step' will be initialized after [-Wreorder]
  734 |     uint32_t marker_pixel_step = 1000000;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:732:13: warning:   'uint8_t ui::external_app::enhanced_drone_analyzer::DroneDisplayController::min_color_power' [-Wreorder]
  732 |     uint8_t min_color_power = 0;
      |             ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1218:1: warning:   when initialized here [-Wreorder]
 1218 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:739:20: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_config_' will be initialized after [-Wreorder]
  739 |     SpectrumConfig spectrum_config_;
      |                    ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:728:26: warning:   'ChannelSpectrumFIFO* ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_fifo_' [-Wreorder]
  728 |     ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
      |                          ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1218:1: warning:   when initialized here [-Wreorder]
 1218 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:740:21: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::nav_' will be initialized after [-Wreorder]
  740 |     NavigationView& nav_;
      |                     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:727:14: warning:   'Gradient ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_gradient_' [-Wreorder]
  727 |     Gradient spectrum_gradient_;
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1218:1: warning:   when initialized here [-Wreorder]
 1218 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1218:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::detected_drones_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1218:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::displayed_drones_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'DisplayDroneEntry::DisplayDroneEntry()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::frequency' should be initialized in the member initialization list [-Weffc++]
  192 | struct DisplayDroneEntry {
      |        ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::type' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::threat' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::rssi' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::last_seen' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::type_name' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::display_color' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:192:8: warning: 'DisplayDroneEntry::trend' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:15,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array: In constructor 'std::array<DisplayDroneEntry, 3>::array()':
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array:94:12: note: synthesized method 'DisplayDroneEntry::DisplayDroneEntry()' first required here
   94 |     struct array
      |            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1231:37: note: synthesized method 'std::array<DisplayDroneEntry, 3>::array()' first required here
 1231 |       nav_(nav), spectrum_gradient_()
      |                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1246:82: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_detection_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1246 | void DroneDisplayController::update_detection_display(const DroneScanner& scanner) {
      |                                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1318:113: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1318 | void DroneDisplayController::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) {
      |                                                                                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1321:64: error: definition of 'auto ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)::<lambda>::operator()(const DisplayDroneEntry&) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)::<lambda>' [-fpermissive]
 1321 |                           [freq](const DisplayDroneEntry& entry) {
      |                                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1349:50: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::sort_drones_by_rssi()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1349 | void DroneDisplayController::sort_drones_by_rssi() {
      |                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::sort_drones_by_rssi()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1351:72: error: definition of 'auto ui::external_app::enhanced_drone_analyzer::DroneDisplayController::sort_drones_by_rssi()::<lambda>::operator()(const DisplayDroneEntry&, const DisplayDroneEntry&) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::sort_drones_by_rssi()::<lambda>' [-fpermissive]
 1351 |               [](const DisplayDroneEntry& a, const DisplayDroneEntry& b) {
      |                                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1358:83: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1358 | void DroneDisplayController::update_drones_display(const DroneScanner& /*scanner*/) {
      |                                                                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1363:74: error: definition of 'auto ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)::<lambda>::operator()(const DisplayDroneEntry&) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)::<lambda>' [-fpermissive]
 1363 |                       [now, STALE_TIMEOUT](const DisplayDroneEntry& entry) {
      |                                                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1380:56: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1380 | void DroneDisplayController::render_drone_text_display() {
      |                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1403:42: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1403 |         snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
      |                                          ^~~~~~~~~~~~~~~~~~~~
......
 1406 |                 drone.rssi,
      |                 ~~~~~~~~~~                
      |                       |
      |                       int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1428:55: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::initialize_mini_spectrum()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1428 | void DroneDisplayController::initialize_mini_spectrum() {
      |                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1435:88: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1435 | void DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum& spectrum) {
      |                                                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1445:62: error: definition of 'bool ui::external_app::enhanced_drone_analyzer::DroneDisplayController::process_bins(uint8_t*)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1445 | bool DroneDisplayController::process_bins(uint8_t* powerlevel) {
      |                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1464:51: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_mini_spectrum()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1464 | void DroneDisplayController::render_mini_spectrum() {
      |                                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1480:130: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, 3>&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1480 | void DroneDisplayController::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES>& drones) {
      |                                                                                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1494:53: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::clear_spectrum_buffers()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1494 | void DroneDisplayController::clear_spectrum_buffers() {
      |                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1498:55: error: definition of 'bool ui::external_app::enhanced_drone_analyzer::DroneDisplayController::validate_spectrum_data() const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1498 | bool DroneDisplayController::validate_spectrum_data() const {
      |                                                       ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1504:76: error: definition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_safe_spectrum_index(size_t, size_t) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1504 | size_t DroneDisplayController::get_safe_spectrum_index(size_t x, size_t y) const {
      |                                                                            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1511:87: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::set_spectrum_range(Frequency, Frequency)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1511 | void DroneDisplayController::set_spectrum_range(Frequency min_freq, Frequency max_freq) {
      |                                                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1524:77: error: definition of 'size_t ui::external_app::enhanced_drone_analyzer::DroneDisplayController::frequency_to_spectrum_bin(Frequency) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController' [-fpermissive]
 1524 | size_t DroneDisplayController::frequency_to_spectrum_bin(Frequency freq_hz) const {
      |                                                                             ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1540:61: error: definition of 'ui::external_app::enhanced_drone_analyzer::DroneUIController::DroneUIController(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, AudioManager&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1540 |                                    ::AudioManager& audio_mgr)
      |                                                             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1555:39: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_start_scan()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1555 | void DroneUIController::on_start_scan() {
      |                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1563:38: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_stop_scan()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1563 | void DroneUIController::on_stop_scan() {
      |                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1571:40: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_toggle_mode()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1571 | void DroneUIController::on_toggle_mode() {
      |                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1587:35: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_menu()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1587 | void DroneUIController::show_menu() {
      |                                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1592:48: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_load_frequency_file()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1592 | void DroneUIController::on_load_frequency_file() {
      |                                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1601:42: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_save_settings()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1601 | void DroneUIController::on_save_settings() {
      |                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1606:43: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_audio_settings()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1606 | void DroneUIController::on_audio_settings() {
      |                                           ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1612:42: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_spectrum_mode()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1612 | void DroneUIController::on_spectrum_mode() {
      |                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1617:60: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::set_spectrum_mode(SpectrumMode)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1617 | void DroneUIController::set_spectrum_mode(SpectrumMode mode) {
      |                                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1622:45: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_hardware_control()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1622 | void DroneUIController::on_hardware_control() {
      |                                             ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1627:42: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_set_bandwidth()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1627 | void DroneUIController::on_set_bandwidth() {
      |                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1636:44: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_set_center_freq()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1636 | void DroneUIController::on_set_center_freq() {
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1642:46: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_hardware_status()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1642 | void DroneUIController::show_hardware_status() {
      |                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_hardware_status()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1645:21: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'long unsigned int' [-Wformat=]
 1645 |             "Band: %u MHz\nFreq: %.3f GHz",
      |                    ~^
      |                     |
      |                     unsigned int
      |                    %lu
 1646 |             hardware_.get_spectrum_bandwidth() / 1000000,
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                |
      |                                                long unsigned int
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1651:38: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_view_logs()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1651 | void DroneUIController::on_view_logs() {
      |                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1656:34: error: definition of 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::on_about()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::DroneUIController' [-fpermissive]
 1656 | void DroneUIController::on_about() {
      |                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1660:89: error: definition of 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1660 | EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
      |                                                                                         ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1697:71: error: definition of 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::~EnhancedDroneSpectrumAnalyzerView()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1697 | EnhancedDroneSpectrumAnalyzerView::~EnhancedDroneSpectrumAnalyzerView() {
      |                                                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:47: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::focus()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1701 | void EnhancedDroneSpectrumAnalyzerView::focus() {
      |                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1705:63: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::paint(ui::Painter&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1705 | void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
      |                                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1709:66: error: definition of 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_key(ui::KeyEvent)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1709 | bool EnhancedDroneSpectrumAnalyzerView::on_key(const KeyEvent key) {
      |                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1721:72: error: definition of 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_touch(ui::TouchEvent)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1721 | bool EnhancedDroneSpectrumAnalyzerView::on_touch(const TouchEvent event) {
      |                                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1725:49: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_show()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1725 | void EnhancedDroneSpectrumAnalyzerView::on_show() {
      |                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1731:49: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_hide()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1731 | void EnhancedDroneSpectrumAnalyzerView::on_hide() {
      |                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1737:63: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::start_scanning_thread()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1737 | void EnhancedDroneSpectrumAnalyzerView::start_scanning_thread() {
      |                                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1742:62: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1742 | void EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread() {
      |                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1747:66: error: definition of 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1747 | bool EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button() {
      |                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1758:60: error: definition of 'bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_menu_button()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1758 | bool EnhancedDroneSpectrumAnalyzerView::handle_menu_button() {
      |                                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1763:66: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1763 | void EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout() {
      |                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1778:62: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::update_modern_layout()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1778 | void EnhancedDroneSpectrumAnalyzerView::update_modern_layout() {
      |                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1782:63: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1782 | void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
      |                                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1813:90: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1813 |                 snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: %u", total_detections);
      |                                                                                         ~^   ~~~~~~~~~~~~~~~~
      |                                                                                          |   |
      |                                                                                          |   uint32_t {aka long unsigned int}
      |                                                                                          unsigned int
      |                                                                                         %lu
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1828:63: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1828 | void EnhancedDroneSpectrumAnalyzerView::setup_button_handlers() {
      |                                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1829:50: error: definition of 'auto ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()::<lambda>::operator()(ui::Button&) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()::<lambda>' [-fpermissive]
 1829 |     button_start_stop_.on_select = [this](Button&) {
      |                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1832:49: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()::<lambda>::operator()(ui::Button&) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()::<lambda>' [-fpermissive]
 1832 |     button_menu_.on_select = [this](Button&) -> void {
      |                                                 ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1836:77: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()::<lambda>::operator()(size_t, int32_t) const' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::setup_button_handlers()::<lambda>' [-fpermissive]
 1836 |     field_scanning_mode_.on_change = [this](size_t index, int32_t value) -> void {
      |                                                                             ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1842:66: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_scanning_mode()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1842 | void EnhancedDroneSpectrumAnalyzerView::initialize_scanning_mode() {
      |                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:82: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::set_scanning_mode_from_index(size_t)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1847 | void EnhancedDroneSpectrumAnalyzerView::set_scanning_mode_from_index(size_t index) {
      |                                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1855:57: error: definition of 'void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::add_ui_elements()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView' [-fpermissive]
 1855 | void EnhancedDroneSpectrumAnalyzerView::add_ui_elements() {
      |                                                         ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1868:57: error: definition of 'ui::external_app::enhanced_drone_analyzer::LoadingScreenView::LoadingScreenView(ui::NavigationView&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::LoadingScreenView' [-fpermissive]
 1868 | LoadingScreenView::LoadingScreenView(NavigationView& nav)
      |                                                         ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1880:47: error: definition of 'void ui::external_app::enhanced_drone_analyzer::LoadingScreenView::paint(ui::Painter&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::LoadingScreenView' [-fpermissive]
 1880 | void LoadingScreenView::paint(Painter& painter) {
      |                                               ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1896:72: error: definition of 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::ScanningCoordinator(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, ui::external_app::enhanced_drone_analyzer::DroneDisplayController&, AudioManager&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1896 |                                        ::AudioManager& audio_controller)
      |                                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1903:43: error: definition of 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::~ScanningCoordinator()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1903 | ScanningCoordinator::~ScanningCoordinator() {
      |                                           ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1907:54: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::start_coordinated_scanning()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1907 | void ScanningCoordinator::start_coordinated_scanning() {
      |                                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1921:53: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::stop_coordinated_scanning()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1921 | void ScanningCoordinator::stop_coordinated_scanning() {
      |                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1931:62: error: definition of 'msg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::scanning_thread_function(void*)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1931 | msg_t ScanningCoordinator::scanning_thread_function(void* arg) {
      |                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1935:56: error: definition of 'msg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::coordinated_scanning_thread()' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1935 | msg_t ScanningCoordinator::coordinated_scanning_thread() {
      |                                                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:90: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::update_runtime_parameters(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1951 | void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
      |                                                                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1956:74: error: definition of 'void ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::show_session_summary(const string&)' is not in namespace enclosing 'ui::external_app::enhanced_drone_analyzer::ScanningCoordinator' [-fpermissive]
 1956 | void ScanningCoordinator::show_session_summary(const std::string& summary) {
      |                                                                          ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1961:1: error: expected '}' at end of input
 1961 | } // namespace ui::external_app::enhanced_drone_analyzer
      | ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:29:53: note: to match this '{'
   29 | namespace ui::external_app::enhanced_drone_analyzer {
      |                                                     ^
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5789: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.