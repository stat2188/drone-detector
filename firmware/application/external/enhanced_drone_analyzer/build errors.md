[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:29:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp: In static member function 'static uint32_t AudioAlertManager::get_alert_duration(AudioAlertManager::AlertLevel)':
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_audio.hpp:62:51: warning: unused parameter 'level' [-Wunused-parameter]
   62 |     static uint32_t get_alert_duration(AlertLevel level) {
      |                                        ~~~~~~~~~~~^~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: At global scope:
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11:11: error: 'ui::external_app' has not been declared
   11 | using ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings;
      |           ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:12:11: error: 'ui::external_app' has not been declared
   12 | using ui::external_app::enhanced_drone_analyzer::SpectrumMode;
      |           ^~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:19:28: error: variable or field 'reset_to_defaults' declared void
   19 |     void reset_to_defaults(DroneAnalyzerSettings& settings);
      |                            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:19:28: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'DroneAudioSettings'?
   19 |     void reset_to_defaults(DroneAnalyzerSettings& settings);
      |                            ^~~~~~~~~~~~~~~~~~~~~
      |                            DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:19:51: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   19 |     void reset_to_defaults(DroneAnalyzerSettings& settings);
      |                                                   ^~~~~~~~
      |                                                   app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:26: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'DroneAudioSettings'?
   22 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                          ^~~~~~~~~~~~~~~~~~~~~
      |                          DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:49: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   22 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                                                 ^~~~~~~~
      |                                                 app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:59: error: expected primary-expression before 'const'
   22 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                                                           ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:82: error: expression list treated as compound expression in initializer [-fpermissive]
   22 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |                                                                                  ^
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:33: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'DroneAudioSettings'?
   23 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                 ^~~~~~~~~~~~~~~~~~~~~
      |                                 DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:56: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   23 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                                        ^~~~~~~~
      |                                                        app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:66: error: expected primary-expression before 'const'
   23 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:92: error: expression list treated as compound expression in initializer [-fpermissive]
   23 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |                                                                                            ^
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:24:58: error: 'DroneAnalyzerSettings' has not been declared
   24 |     bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings);
      |                                                          ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:27:33: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'DroneAudioSettings'?
   27 |     bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
      |                                 ^~~~~~~~~~~~~~~~~~~~~
      |                                 DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:27:56: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   27 |     bool load_settings_from_txt(DroneAnalyzerSettings& settings) {
      |                                                        ^~~~~~~~
      |                                                        app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:40:10: error: redefinition of 'bool ScannerSettingsManager::parse_settings_content'
   40 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |          ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:23:10: note: 'bool ScannerSettingsManager::parse_settings_content' previously defined here
   23 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content);
      |          ^~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:40:33: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'DroneAudioSettings'?
   40 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |                                 ^~~~~~~~~~~~~~~~~~~~~
      |                                 DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:40:56: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   40 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |                                                        ^~~~~~~~
      |                                                        app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:40:66: error: expected primary-expression before 'const'
   40 |     bool parse_settings_content(DroneAnalyzerSettings& settings, const std::string& content) {
      |                                                                  ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:52:10: error: redefinition of 'bool ScannerSettingsManager::parse_key_value'
   52 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:22:10: note: 'bool ScannerSettingsManager::parse_key_value' previously defined here
   22 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line);
      |          ^~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:52:26: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'DroneAudioSettings'?
   52 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |                          ^~~~~~~~~~~~~~~~~~~~~
      |                          DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:52:49: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   52 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |                                                 ^~~~~~~~
      |                                                 app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:52:59: error: expected primary-expression before 'const'
   52 |     bool parse_key_value(DroneAnalyzerSettings& settings, const std::string& line) {
      |                                                           ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:96:28: error: variable or field 'reset_to_defaults' declared void
   96 |     void reset_to_defaults(DroneAnalyzerSettings& settings) {
      |                            ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:96:28: error: 'DroneAnalyzerSettings' was not declared in this scope; did you mean 'DroneAudioSettings'?
   96 |     void reset_to_defaults(DroneAnalyzerSettings& settings) {
      |                            ^~~~~~~~~~~~~~~~~~~~~
      |                            DroneAudioSettings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:96:51: error: 'settings' was not declared in this scope; did you mean 'app_settings'?
   96 |     void reset_to_defaults(DroneAnalyzerSettings& settings) {
      |                                                   ^~~~~~~~
      |                                                   app_settings
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:130:58: error: 'DroneAnalyzerSettings' has not been declared
  130 |     bool load_from_txt_impl(const std::string& filepath, DroneAnalyzerSettings& settings) {
      |                                                          ^~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp: In function 'bool ScannerSettingsManager::load_from_txt_impl(const string&, int&)':
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:133:13: error: 'reset_to_defaults' was not declared in this scope
  133 |             reset_to_defaults(settings);
      |             ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:141:13: error: 'reset_to_defaults' was not declared in this scope
  141 |             reset_to_defaults(settings);
      |             ^~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:145:61: error: 'ScannerSettingsManager::parse_settings_content' cannot be used as a function
  145 |         return parse_settings_content(settings, file_content);
      |                                                             ^
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:19:
/havoc/firmware/application/./ui_navigation.hpp: In instantiation of 'T* ui::NavigationView::push(Args&& ...) [with T = ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView; Args = {}]':
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:58:92:   required from here
/havoc/firmware/application/./ui_navigation.hpp:111:69: error: invalid use of incomplete type 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
  111 |         return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
      |                                                                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:51:7: note: forward declaration of 'class ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView'
   51 | class EnhancedDroneSpectrumAnalyzerView;
      |       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
At global scope:
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.