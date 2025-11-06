[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:330:30: error: expected class-name before '(' token
  330 |         ~DroneDetectionLogger();
      |                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:331:36: error: 'DroneDetectionLogger' does not name a type
  331 |         DroneDetectionLogger(const DroneDetectionLogger&) = delete;
      |                                    ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:331:61: error: ISO C++ forbids declaration of 'DroneDetectionLogger' with no type [-fpermissive]
  331 |         DroneDetectionLogger(const DroneDetectionLogger&) = delete;
      |                                                             ^~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:332:9: error: 'DroneDetectionLogger' does not name a type
  332 |         DroneDetectionLogger& operator=(const DroneDetectionLogger&) = delete;
      |         ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_scanning_active() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:301:46: error: 'scanning_active_' was not declared in this scope; did you mean 'is_scanning_active'?
  301 |     bool is_scanning_active() const { return scanning_active_; }
      |                                              ^~~~~~~~~~~~~~~~
      |                                              is_scanning_active
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'ui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode ui::external_app::enhanced_drone_analyzer::DroneScanner::get_scanning_mode() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:305:53: error: 'scanning_mode_' was not declared in this scope; did you mean 'scanning_mode_name'?
  305 |     ScanningMode get_scanning_mode() const { return scanning_mode_; }
      |                                                     ^~~~~~~~~~~~~~
      |                                                     scanning_mode_name
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'ThreatLevel ui::external_app::enhanced_drone_analyzer::DroneScanner::get_max_detected_threat() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:318:58: error: 'max_detected_threat_' was not declared in this scope; did you mean 'get_max_detected_threat'?
  318 |     ThreatLevel get_max_detected_threat() const { return max_detected_threat_; }
      |                                                          ^~~~~~~~~~~~~~~~~~~~
      |                                                          get_max_detected_threat
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_approaching_count() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:323:51: error: 'approaching_count_' was not declared in this scope; did you mean 'get_approaching_count'?
  323 |     size_t get_approaching_count() const { return approaching_count_; }
      |                                                   ^~~~~~~~~~~~~~~~~~
      |                                                   get_approaching_count
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_receding_count() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:324:48: error: 'receding_count_' was not declared in this scope; did you mean 'get_receding_count'?
  324 |     size_t get_receding_count() const { return receding_count_; }
      |                                                ^~~~~~~~~~~~~~~
      |                                                get_receding_count
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'size_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_static_count() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:325:46: error: 'static_count_' was not declared in this scope; did you mean 'get_static_count'?
  325 |     size_t get_static_count() const { return static_count_; }
      |                                              ^~~~~~~~~~~~~
      |                                              get_static_count
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'uint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_total_detections() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:326:52: error: 'total_detections_' was not declared in this scope; did you mean 'get_total_detections'?
  326 |     uint32_t get_total_detections() const { return total_detections_; }
      |                                                    ^~~~~~~~~~~~~~~~~
      |                                                    get_total_detections
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'bool ui::external_app::enhanced_drone_analyzer::DroneScanner::is_real_mode() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:327:40: error: 'is_real_mode_' was not declared in this scope; did you mean 'is_real_mode'?
  327 |     bool is_real_mode() const { return is_real_mode_; }
      |                                        ^~~~~~~~~~~~~
      |                                        is_real_mode
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:352:1: error: expected unqualified-id before 'private'
  352 | private:
      | ^~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:367:45: error: non-member function 'Frequency ui::external_app::enhanced_drone_analyzer::get_current_radio_frequency()' cannot have cv-qualifier
  367 |     Frequency get_current_radio_frequency() const;
      |                                             ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:380:5: error: 'ScanningMode' does not name a type
  380 |     ScanningMode scanning_mode_ = ScanningMode::DATABASE;
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:396:5: error: 'DroneDetectionLogger' does not name a type
  396 |     DroneDetectionLogger detection_logger_;
      |     ^~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:559:41: error: 'DroneScanner' does not name a type
  559 |     void update_detection_display(const DroneScanner& scanner);
      |                                         ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:564:38: error: 'DroneScanner' does not name a type
  564 |     void update_drones_display(const DroneScanner& scanner);
      |                                      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:641:22: error: 'DroneScanner' has not been declared
  641 |                      DroneScanner& scanner,
      |                      ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:669:5: error: 'DroneScanner' does not name a type
  669 |     DroneScanner& scanner_;
      |     ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'uint8_t EnhancedDroneSpectrumAnalyzerView::get_spec_iq_phase_calibration_value() const':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:705:66: error: 'iq_phase_calibration_value_' was not declared in this scope; did you mean 'get_spec_iq_phase_calibration_value'?
  705 |     uint8_t get_spec_iq_phase_calibration_value() const { return iq_phase_calibration_value_; }
      |                                                                  ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                                                  get_spec_iq_phase_calibration_value
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: In member function 'void EnhancedDroneSpectrumAnalyzerView::set_spec_iq_phase_calibration_value(uint8_t)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:707:9: error: 'iq_phase_calibration_value_' was not declared in this scope; did you mean 'get_spec_iq_phase_calibration_value'?
  707 |         iq_phase_calibration_value_ = cal_value;
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~
      |         get_spec_iq_phase_calibration_value
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:756:1: error: expected declaration before '}' token
  756 | } // namespace ui::external_app::enhanced_drone_analyzer
      | ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/havoc/firmware/application/./ui_navigation.hpp: In instantiation of 'T* ui::NavigationView::push(Args&& ...) [with T = ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView; Args = {}]':
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:58:92:   required from here
/havoc/firmware/application/./ui_navigation.hpp:111:69: error: invalid use of incomplete type 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
  111 |         return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
      |                                                                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:51:7: note: forward declaration of 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
   51 | class EnhancedDroneSpectrumAnalyzerView;
      |       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.