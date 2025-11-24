[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
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
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:713:14: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::marker_pixel_step' will be initialized after [-Wreorder]
  713 |     uint32_t marker_pixel_step = 1000000;
      |              ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:711:13: warning:   'uint8_t ui::external_app::enhanced_drone_analyzer::DroneDisplayController::min_color_power' [-Wreorder]
  711 |     uint8_t min_color_power = 0;
      |             ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1189:1: warning:   when initialized here [-Wreorder]
 1189 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:718:20: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_config_' will be initialized after [-Wreorder]
  718 |     SpectrumConfig spectrum_config_;
      |                    ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:707:26: warning:   'ChannelSpectrumFIFO* ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_fifo_' [-Wreorder]
  707 |     ChannelSpectrumFIFO* spectrum_fifo_ = nullptr;
      |                          ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1189:1: warning:   when initialized here [-Wreorder]
 1189 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:719:21: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::nav_' will be initialized after [-Wreorder]
  719 |     NavigationView& nav_;
      |                     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:706:14: warning:   'Gradient ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_gradient_' [-Wreorder]
  706 |     Gradient spectrum_gradient_;
      |              ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1189:1: warning:   when initialized here [-Wreorder]
 1189 | DroneDisplayController::DroneDisplayController(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1189:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::detected_drones_' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1189:1: warning: 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::displayed_drones_' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In constructor 'DisplayDroneEntry::DisplayDroneEntry()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::frequency' should be initialized in the member initialization list [-Weffc++]
  191 | struct DisplayDroneEntry {
      |        ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::type' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::threat' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::rssi' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::last_seen' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::type_name' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::display_color' should be initialized in the member initialization list [-Weffc++]
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:191:8: warning: 'DisplayDroneEntry::trend' should be initialized in the member initialization list [-Weffc++]
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:15,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array: In constructor 'std::array<DisplayDroneEntry, 3>::array()':
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array:94:12: note: synthesized method 'DisplayDroneEntry::DisplayDroneEntry()' first required here
   94 |     struct array
      |            ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In constructor 'ui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1202:37: note: synthesized method 'std::array<DisplayDroneEntry, 3>::array()' first required here
 1202 |       nav_(nav), spectrum_gradient_()
      |                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1374:42: warning: format '%d' expects argument of type 'int', but argument 6 has type 'int32_t' {aka 'long int'} [-Wformat=]
 1374 |         snprintf(buffer, sizeof(buffer), DRONE_DISPLAY_FORMAT,
      |                                          ^~~~~~~~~~~~~~~~~~~~
......
 1377 |                 drone.rssi,
      |                 ~~~~~~~~~~                
      |                       |
      |                       int32_t {aka long int}
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In member function 'void ui::external_app::enhanced_drone_analyzer::DroneUIController::show_hardware_status()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1616:21: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'long unsigned int' [-Wformat=]
 1616 |             "Band: %u MHz\nFreq: %.3f GHz",
      |                    ~^
      |                     |
      |                     unsigned int
      |                    %lu
 1617 |             hardware_.get_spectrum_bandwidth() / 1000000,
      |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                |
      |                                                long unsigned int
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1662:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1662 | void EnhancedDroneSpectrumAnalyzerView::set_scanning_mode_from_index(size_t index) {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void set_scanning_mode_from_index(size_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1663:5: error: 'DroneScanner' has not been declared
 1663 |     DroneScanner::ScanningMode mode = static_cast<DroneScanner::ScanningMode>(index);
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1664:5: error: 'scanner_' was not declared in this scope
 1664 |     scanner_->set_scanning_mode(mode);
      |     ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1664:33: error: 'mode' was not declared in this scope; did you mean 'modf'?
 1664 |     scanner_->set_scanning_mode(mode);
      |                                 ^~~~
      |                                 modf
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1665:5: error: 'display_controller_' was not declared in this scope
 1665 |     display_controller_->set_scanning_status(ui_controller_->is_scanning(),
      |     ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1665:46: error: 'ui_controller_' was not declared in this scope
 1665 |     display_controller_->set_scanning_status(ui_controller_->is_scanning(),
      |                                              ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1667:5: error: 'update_modern_layout' was not declared in this scope
 1667 |     update_modern_layout();
      |     ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1662:77: warning: unused parameter 'index' [-Wunused-parameter]
 1662 | void EnhancedDroneSpectrumAnalyzerView::set_scanning_mode_from_index(size_t index) {
      |                                                                      ~~~~~~~^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1672:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1672 | void EnhancedDroneSpectrumAnalyzerView::focus() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void focus()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1673:5: error: 'button_start_stop_' was not declared in this scope
 1673 |     button_start_stop_.focus();
      |     ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1676:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1676 | void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:24: error: cannot call member function 'virtual void ui::View::paint(ui::Painter&)' without object
 1677 |     View::paint(painter);
      |                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1680:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1680 | bool EnhancedDroneSpectrumAnalyzerView::on_key(const KeyEvent key) {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'bool on_key(ui::KeyEvent)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1683:13: error: 'stop_scanning_thread' was not declared in this scope
 1683 |             stop_scanning_thread();
      |             ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1684:13: error: 'nav_' was not declared in this scope
 1684 |             nav_.pop();
      |             ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1689:28: error: cannot call member function 'virtual bool ui::Widget::on_key(ui::KeyEvent)' without object
 1689 |     return View::on_key(key);
      |                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1692 | bool EnhancedDroneSpectrumAnalyzerView::on_touch(const TouchEvent event) {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'bool on_touch(ui::TouchEvent)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1693:32: error: cannot call member function 'virtual bool ui::Widget::on_touch(ui::TouchEvent)' without object
 1693 |     return View::on_touch(event);
      |                                ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1696:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1696 | void EnhancedDroneSpectrumAnalyzerView::on_show() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void on_show()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1697:19: error: cannot call member function 'virtual void ui::Widget::on_show()' without object
 1697 |     View::on_show();
      |                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1699:5: error: 'hardware_' was not declared in this scope
 1699 |     hardware_->on_hardware_show();
      |     ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1702:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1702 | void EnhancedDroneSpectrumAnalyzerView::on_hide() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void on_hide()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1703:5: error: 'stop_scanning_thread' was not declared in this scope
 1703 |     stop_scanning_thread();
      |     ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1704:5: error: 'hardware_' was not declared in this scope
 1704 |     hardware_->on_hardware_hide();
      |     ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1705:19: error: cannot call member function 'virtual void ui::Widget::on_hide()' without object
 1705 |     View::on_hide();
      |                   ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1708:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1708 | void EnhancedDroneSpectrumAnalyzerView::start_scanning_thread() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void start_scanning_thread()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1709:9: error: 'scanning_coordinator_' was not declared in this scope
 1709 |     if (scanning_coordinator_->is_scanning_active()) return;
      |         ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1710:5: error: 'scanning_coordinator_' was not declared in this scope
 1710 |     scanning_coordinator_->start_coordinated_scanning();
      |     ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1713:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1713 | void EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void stop_scanning_thread()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1714:10: error: 'scanning_coordinator_' was not declared in this scope
 1714 |     if (!scanning_coordinator_->is_scanning_active()) return;
      |          ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1715:5: error: 'scanning_coordinator_' was not declared in this scope
 1715 |     scanning_coordinator_->stop_coordinated_scanning();
      |     ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1718:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1718 | bool EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'bool handle_start_stop_button()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1719:9: error: 'scanning_coordinator_' was not declared in this scope
 1719 |     if (scanning_coordinator_->is_scanning_active()) {
      |         ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1720:9: error: 'ui_controller_' was not declared in this scope
 1720 |         ui_controller_->on_stop_scan();
      |         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1721:9: error: 'button_start_stop_' was not declared in this scope
 1721 |         button_start_stop_.set_text("START/STOP");
      |         ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1723:9: error: 'ui_controller_' was not declared in this scope
 1723 |         ui_controller_->on_start_scan();
      |         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1724:9: error: 'button_start_stop_' was not declared in this scope
 1724 |         button_start_stop_.set_text("STOP");
      |         ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1729:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1729 | bool EnhancedDroneSpectrumAnalyzerView::handle_menu_button() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'bool handle_menu_button()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1730:5: error: 'ui_controller_' was not declared in this scope
 1730 |     ui_controller_->show_menu();
      |     ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1734:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1734 | void EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void initialize_modern_layout()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1735:5: error: 'smart_header_' was not declared in this scope
 1735 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |     ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1735:38: error: 'SmartThreatHeader' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader'?
 1735 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |                                      ^~~~~~~~~~~~~~~~~
      |                                      ui::external_app::enhanced_drone_analyzer::SmartThreatHeader
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:546:7: note: 'ui::external_app::enhanced_drone_analyzer::SmartThreatHeader' declared here
  546 | class SmartThreatHeader : public View {
      |       ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1735:85: error: no matching function for call to 'make_unique<<expression error> >(ui::Rect)'
 1735 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |                                                                                     ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)'
  848 |     make_unique(_Args&&... __args)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1735:85: error: template argument 1 is invalid
 1735 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |                                                                                     ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note: candidate: 'template<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)'
  854 |     make_unique(size_t __num)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1735:85: error: template argument 1 is invalid
 1735 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |                                                                                     ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)' <deleted>
  860 |     make_unique(_Args&&...) = delete;
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1735:85: error: template argument 1 is invalid
 1735 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
      |                                                                                     ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1736:5: error: 'status_bar_' was not declared in this scope
 1736 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |     ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1736:36: error: 'ConsoleStatusBar' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar'?
 1736 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |                                    ^~~~~~~~~~~~~~~~
      |                                    ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:614:7: note: 'ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar' declared here
  614 | class ConsoleStatusBar : public View {
      |       ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1736:102: error: no matching function for call to 'make_unique<<expression error> >(int, ui::Rect)'
 1736 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |                                                                                                      ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)'
  848 |     make_unique(_Args&&... __args)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1736:102: error: template argument 1 is invalid
 1736 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |                                                                                                      ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note: candidate: 'template<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)'
  854 |     make_unique(size_t __num)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1736:102: error: template argument 1 is invalid
 1736 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |                                                                                                      ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)' <deleted>
  860 |     make_unique(_Args&&...) = delete;
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1736:102: error: template argument 1 is invalid
 1736 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
      |                                                                                                      ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1739:28: error: 'threat_cards_' was not declared in this scope
 1739 |     for (size_t i = 0; i < threat_cards_.size(); ++i) {
      |                            ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1740:45: error: 'ThreatCard' was not declared in this scope; did you mean 'ui::external_app::enhanced_drone_analyzer::ThreatCard'?
 1740 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                             ^~~~~~~~~~
      |                                             ui::external_app::enhanced_drone_analyzer::ThreatCard
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:581:7: note: 'ui::external_app::enhanced_drone_analyzer::ThreatCard' declared here
  581 | class ThreatCard : public View {
      |       ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1740:68: warning: narrowing conversion of 'card_y_pos' from 'size_t' {aka 'unsigned int'} to 'int' [-Wnarrowing]
 1740 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                    ^~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1740:97: error: no matching function for call to 'make_unique<<expression error> >(size_t&, ui::Rect)'
 1740 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                                                 ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)'
  848 |     make_unique(_Args&&... __args)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1740:97: error: template argument 1 is invalid
 1740 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                                                 ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note: candidate: 'template<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)'
  854 |     make_unique(size_t __num)
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1740:97: error: template argument 1 is invalid
 1740 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                                                 ^
In file included from /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80,
                 from /havoc/firmware/application/./file.hpp:35,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note: candidate: 'template<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)' <deleted>
  860 |     make_unique(_Args&&...) = delete;
      |     ^~~~~~~~~~~
/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5: note:   template argument deduction/substitution failed:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1740:97: error: template argument 1 is invalid
 1740 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24});
      |                                                                                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1744:5: error: 'handle_scanner_update' was not declared in this scope
 1744 |     handle_scanner_update();
      |     ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1747:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1747 | void EnhancedDroneSpectrumAnalyzerView::update_modern_layout() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void update_modern_layout()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1748:5: error: 'handle_scanner_update' was not declared in this scope
 1748 |     handle_scanner_update();
      |     ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1751:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1751 | void EnhancedDroneSpectrumAnalyzerView::handle_scanner_update() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void handle_scanner_update()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1752:10: error: 'scanner_' was not declared in this scope
 1752 |     if (!scanner_ || !smart_header_ || !status_bar_) return;
      |          ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1752:23: error: 'smart_header_' was not declared in this scope
 1752 |     if (!scanner_ || !smart_header_ || !status_bar_) return;
      |                       ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1752:41: error: 'status_bar_' was not declared in this scope
 1752 |     if (!scanner_ || !smart_header_ || !status_bar_) return;
      |                                         ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1754:30: error: 'scanner_' was not declared in this scope
 1754 |     ThreatLevel max_threat = scanner_->get_max_detected_threat();
      |                              ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1762:9: error: 'smart_header_' was not declared in this scope
 1762 |     if (smart_header_) {
      |         ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1767:9: error: 'status_bar_' was not declared in this scope
 1767 |     if (status_bar_) {
      |         ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1778:41: error: 'display_controller_' was not declared in this scope
 1778 |             const char* primary_msg = (!display_controller_) ?
      |                                         ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1782:90: warning: format '%u' expects argument of type 'unsigned int', but argument 4 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]
 1782 |                 snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: %u", total_detections);
      |                                                                                         ~^   ~~~~~~~~~~~~~~~~
      |                                                                                          |   |
      |                                                                                          |   uint32_t {aka long unsigned int}
      |                                                                                          unsigned int
      |                                                                                         %lu
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1792:9: error: 'display_controller_' was not declared in this scope
 1792 |     if (display_controller_) {
      |         ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1797:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1797 | void EnhancedDroneSpectrumAnalyzerView::setup_button_handlers() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void setup_button_handlers()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1798:5: error: 'button_start_stop_' was not declared in this scope
 1798 |     button_start_stop_.on_select = [this](Button&) {
      |     ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1798:37: error: invalid use of 'this' in non-member function
 1798 |     button_start_stop_.on_select = [this](Button&) {
      |                                     ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1801:5: error: 'button_menu_' was not declared in this scope
 1801 |     button_menu_.on_select = [this](Button&) {
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1801:31: error: invalid use of 'this' in non-member function
 1801 |     button_menu_.on_select = [this](Button&) {
      |                               ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In lambda function:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1802:9: error: 'ui_controller_' was not declared in this scope
 1802 |         ui_controller_->show_menu();
      |         ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void setup_button_handlers()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1805:5: error: 'field_scanning_mode_' was not declared in this scope
 1805 |     field_scanning_mode_.on_change = [this](size_t index, int32_t value) {
      |     ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1805:39: error: invalid use of 'this' in non-member function
 1805 |     field_scanning_mode_.on_change = [this](size_t index, int32_t value) {
      |                                       ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1811:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1811 | void EnhancedDroneSpectrumAnalyzerView::initialize_scanning_mode() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void initialize_scanning_mode()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1812:41: error: 'scanner_' was not declared in this scope
 1812 |     int initial_mode = static_cast<int>(scanner_->get_scanning_mode());
      |                                         ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1813:5: error: 'field_scanning_mode_' was not declared in this scope
 1813 |     field_scanning_mode_.set_selected_index(initial_mode);
      |     ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1816:6: error: 'EnhancedDroneSpectrumAnalyzerView' has not been declared
 1816 | void EnhancedDroneSpectrumAnalyzerView::add_ui_elements() {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void add_ui_elements()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1818:19: error: 'smart_header_' was not declared in this scope
 1818 |     add_children({smart_header_.get(), status_bar_.get()});
      |                   ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1818:40: error: 'status_bar_' was not declared in this scope
 1818 |     add_children({smart_header_.get(), status_bar_.get()});
      |                                        ^~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1818:5: error: 'add_children' was not declared in this scope
 1818 |     add_children({smart_header_.get(), status_bar_.get()});
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1819:23: error: 'threat_cards_' was not declared in this scope
 1819 |     for (auto& card : threat_cards_) {
      |                       ^~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1820:9: error: 'add_child' was not declared in this scope
 1820 |         add_child(card.get());
      |         ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1822:20: error: 'button_start_stop_' was not declared in this scope
 1822 |     add_children({&button_start_stop_, &button_menu_});
      |                    ^~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1822:41: error: 'button_menu_' was not declared in this scope
 1822 |     add_children({&button_start_stop_, &button_menu_});
      |                                         ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:1: error: 'LoadingScreenView' does not name a type
 1826 | LoadingScreenView::LoadingScreenView(NavigationView& nav)
      | ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1828:39: error: expected unqualified-id before ',' token
 1828 |       text_eda_(Rect{108, 213, 24, 16}, "EDA"),
      |                                       ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1828:41: error: expected unqualified-id before string constant
 1828 |       text_eda_(Rect{108, 213, 24, 16}, "EDA"),
      |                                         ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1838:6: error: 'LoadingScreenView' has not been declared
 1838 | void LoadingScreenView::paint(Painter& painter) {
      |      ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1838:6: error: redefinition of 'void paint(ui::Painter&)'
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1676:6: note: 'void paint(ui::Painter&)' previously defined here
 1676 | void EnhancedDroneSpectrumAnalyzerView::paint(Painter& painter) {
      |      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void paint(ui::Painter&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1843:24: error: cannot call member function 'virtual void ui::View::paint(ui::Painter&)' without object
 1843 |     View::paint(painter);
      |                        ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1850:1: error: 'ScanningCoordinator' does not name a type
 1850 | ScanningCoordinator::ScanningCoordinator(NavigationView& nav,
      | ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1861:1: error: 'ScanningCoordinator' does not name a type
 1861 | ScanningCoordinator::~ScanningCoordinator() {
      | ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1865:6: error: 'ScanningCoordinator' has not been declared
 1865 | void ScanningCoordinator::start_coordinated_scanning() {
      |      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void start_coordinated_scanning()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1866:9: error: 'scanning_active_' was not declared in this scope
 1866 |     if (scanning_active_) return;
      |         ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1867:5: error: 'scanning_active_' was not declared in this scope
 1867 |     scanning_active_ = true;
      |     ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1870:5: error: 'scanning_thread_' was not declared in this scope; did you mean 'stop_scanning_thread'?
 1870 |     scanning_thread_ = chThdCreateFromHeap(NULL, COORDINATOR_THREAD_STACK_SIZE,
      |     ^~~~~~~~~~~~~~~~
      |     stop_scanning_thread
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1870:50: error: 'COORDINATOR_THREAD_STACK_SIZE' was not declared in this scope; did you mean 'SCANNING_THREAD_STACK_SIZE'?
 1870 |     scanning_thread_ = chThdCreateFromHeap(NULL, COORDINATOR_THREAD_STACK_SIZE,
      |                                                  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                  SCANNING_THREAD_STACK_SIZE
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1872:42: error: 'scanning_thread_function' was not declared in this scope
 1872 |                                          scanning_thread_function, this);
      |                                          ^~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1872:68: error: invalid use of 'this' in non-member function
 1872 |                                          scanning_thread_function, this);
      |                                                                    ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1879:6: error: 'ScanningCoordinator' has not been declared
 1879 | void ScanningCoordinator::stop_coordinated_scanning() {
      |      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'void stop_coordinated_scanning()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1880:10: error: 'scanning_active_' was not declared in this scope
 1880 |     if (!scanning_active_) return;
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:5: error: 'scanning_active_' was not declared in this scope
 1881 |     scanning_active_ = false;
      |     ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1883:9: error: 'scanning_thread_' was not declared in this scope; did you mean 'stop_scanning_thread'?
 1883 |     if (scanning_thread_) {
      |         ^~~~~~~~~~~~~~~~
      |         stop_scanning_thread
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1889:7: error: 'ScanningCoordinator' has not been declared
 1889 | msg_t ScanningCoordinator::scanning_thread_function(void* arg) {
      |       ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'msg_t scanning_thread_function(void*)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1890:24: error: 'ScanningCoordinator' does not name a type
 1890 |     return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
      |                        ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1890:43: error: expected '>' before '*' token
 1890 |     return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
      |                                           ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1890:43: error: expected '(' before '*' token
 1890 |     return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
      |                                           ^
      |                                           (
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1890:44: error: expected primary-expression before '>' token
 1890 |     return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
      |                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1890:50: error: 'void*' is not a pointer-to-object type
 1890 |     return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
      |                                                  ^~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1890:81: error: expected ')' before ';' token
 1890 |     return static_cast<ScanningCoordinator*>(arg)->coordinated_scanning_thread();
      |                                                                                 ^
      |                                                                                 )
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1893:7: error: 'ScanningCoordinator' has not been declared
 1893 | msg_t ScanningCoordinator::coordinated_scanning_thread() {
      |       ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: In function 'msg_t coordinated_scanning_thread()':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1894:12: error: 'scanning_active_' was not declared in this scope
 1894 |     while (scanning_active_) {
      |            ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1897:13: error: 'scanner_' was not declared in this scope
 1897 |             scanner_.perform_scan_cycle(hardware_);
      |             ^~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1897:41: error: 'hardware_' was not declared in this scope
 1897 |             scanner_.perform_scan_cycle(hardware_);
      |                                         ^~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1898:13: error: 'display_controller_' was not declared in this scope
 1898 |             display_controller_.update_detection_display(scanner_);
      |             ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/chibios/os/kernel/include/ch.h:115,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:8,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1901:32: error: 'scan_interval_ms_' was not declared in this scope
 1901 |         chThdSleepMilliseconds(scan_interval_ms_);
      |                                ^~~~~~~~~~~~~~~~~
/havoc/firmware/chibios/os/kernel/include/chvt.h:67:30: note: in definition of macro 'MS2ST'
   67 |   ((systime_t)(((((uint32_t)(msec)) * ((uint32_t)CH_FREQUENCY) - 1UL) /     \
      |                              ^~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1901:9: note: in expansion of macro 'chThdSleepMilliseconds'
 1901 |         chThdSleepMilliseconds(scan_interval_ms_);
      |         ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1903:5: error: 'scanning_active_' was not declared in this scope
 1903 |     scanning_active_ = false;
      |     ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1904:5: error: 'scanning_thread_' was not declared in this scope; did you mean 'stop_scanning_thread'?
 1904 |     scanning_thread_ = nullptr;
      |     ^~~~~~~~~~~~~~~~
      |     stop_scanning_thread
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1909:6: error: 'ScanningCoordinator' has not been declared
 1909 | void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
      |      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1914:6: error: 'ScanningCoordinator' has not been declared
 1914 | void ScanningCoordinator::show_session_summary(const std::string& summary) {
      |      ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1919:1: error: expected declaration before '}' token
 1919 | } // namespace ui::external_app::enhanced_drone_analyzer
      | ^
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5789: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.