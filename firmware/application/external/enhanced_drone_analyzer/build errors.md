6895632Z [100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
2025-11-06T14:24:32.4570425Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:32.4571411Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11[m[K,
2025-11-06T14:24:32.4572388Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31[m[K:
2025-11-06T14:24:32.4574905Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::ScanningCoordinator[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:32.4576406Z    18 | class [01;35m[KScanningCoordinator[m[K {
2025-11-06T14:24:32.4576772Z       |       [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:32.4578406Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:32.4580626Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:32.4766652Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11[m[K,
2025-11-06T14:24:32.4768421Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31[m[K:
2025-11-06T14:24:32.4771612Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:32.4773634Z   300 | class [01;35m[KDroneScanner[m[K {
2025-11-06T14:24:32.4774122Z       |       [01;35m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:32.4776581Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:32.4780081Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:32.9010124Z [01m[Kcc1plus:[m[K [01;35m[Kwarning: [m[Kunrecognized command line option '[01m[K-Wno-volatile[m[K'
2025-11-06T14:24:32.9664556Z [100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
2025-11-06T14:24:33.0663605Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1117:27:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-06T14:24:33.0665788Z  1117 |         progress_bar[i] = [01;35m[K'█'[m[K;
2025-11-06T14:24:33.0666362Z       |                           [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.0667876Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1362:61:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-06T14:24:33.0669505Z  1362 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'▲'[m[K; break;
2025-11-06T14:24:33.0670375Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.0671881Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1363:58:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-06T14:24:33.0673713Z  1363 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'▼'[m[K; break;
2025-11-06T14:24:33.0674483Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.0676515Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1366:37:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-06T14:24:33.0678097Z  1366 |             default: trend_symbol = [01;35m[K'■'[m[K; break;
2025-11-06T14:24:33.0678816Z       |                                     [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.7329583Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.7330719Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.7332681Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::ScanningCoordinator[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.7334167Z    18 | class [01;35m[KScanningCoordinator[m[K {
2025-11-06T14:24:33.7334495Z       |       [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.7336184Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.7338279Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.7520587Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.7522580Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.7524130Z   300 | class [01;35m[KDroneScanner[m[K {
2025-11-06T14:24:33.7524627Z       |       [01;35m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.7526235Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.7528225Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.8775197Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:50:26:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const int32_t HYSTERESIS_MARGIN_DB[m[K'
2025-11-06T14:24:33.8776267Z    50 | static constexpr int32_t [01;31m[KHYSTERESIS_MARGIN_DB[m[K = 3;
2025-11-06T14:24:33.8776740Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8777432Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:19[m[K,
2025-11-06T14:24:33.8778351Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.8779613Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:14:26:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const int16_t HYSTERESIS_MARGIN_DB[m[K'
2025-11-06T14:24:33.8780613Z    14 | static constexpr int16_t [01;36m[KHYSTERESIS_MARGIN_DB[m[K = 5;
2025-11-06T14:24:33.8781304Z       |                          [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8782291Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:53:26:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB[m[K'
2025-11-06T14:24:33.8783504Z    53 | static constexpr int32_t [01;31m[KDEFAULT_RSSI_THRESHOLD_DB[m[K = -90;
2025-11-06T14:24:33.8783967Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8784645Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18[m[K,
2025-11-06T14:24:33.8785564Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.8786825Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:210:26:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB[m[K' previously defined here
2025-11-06T14:24:33.8787745Z   210 | static constexpr int32_t [01;36m[KDEFAULT_RSSI_THRESHOLD_DB[m[K = -90;
2025-11-06T14:24:33.8788150Z       |                          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8833700Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneScanner()[m[K':
2025-11-06T14:24:33.8836052Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:189:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' does not have any field named '[01m[Kfrequency_list_[m[K'
2025-11-06T14:24:33.8837901Z   189 |       [01;31m[Kfrequency_list_[m[K(),
2025-11-06T14:24:33.8838424Z       |       [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8839293Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.8841671Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:377:10:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.8843111Z   377 |     bool [01;35m[Kscanning_active_[m[K = false;
2025-11-06T14:24:33.8843434Z       |          [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8844540Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:375:13:[m[K [01;35m[Kwarning: [m[K  '[01m[KThread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.8845750Z   375 |     Thread* [01;35m[Kscanning_thread_[m[K = nullptr;
2025-11-06T14:24:33.8846387Z       |             [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8847816Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:174:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.8848932Z   174 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-06T14:24:33.8849223Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8849780Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.8851224Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:400:22:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.8852216Z   400 |     WidebandScanData [01;35m[Kwideband_scan_data_[m[K;
2025-11-06T14:24:33.8852751Z       |                      [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8854156Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:386:18:[m[K [01;35m[Kwarning: [m[K  '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_mode_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.8855335Z   386 |     ScanningMode [01;35m[Kscanning_mode_[m[K = ScanningMode::DATABASE;
2025-11-06T14:24:33.8855711Z       |                  [01;35m[K^~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8856462Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:174:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.8857177Z   174 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-06T14:24:33.8857463Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8858607Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:174:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.8861775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:174:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::tracked_drones_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.8865446Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:174:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.8868694Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:174:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::detection_logger_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.8870549Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_database_and_scanner()[m[K':
2025-11-06T14:24:33.8873174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:203:26:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kopen[m[K'
2025-11-06T14:24:33.8875237Z   203 |     if (!drone_database_.[01;31m[Kopen[m[K(db_path, true)) {
2025-11-06T14:24:33.8875907Z       |                          [01;31m[K^~~~[m[K
2025-11-06T14:24:33.8877911Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::load_frequency_database()[m[K':
2025-11-06T14:24:33.8880189Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:299:23:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kis_open[m[K'
2025-11-06T14:24:33.8881234Z   299 |         if (!freq_db_.[01;31m[Kis_open[m[K()) {
2025-11-06T14:24:33.8881535Z       |                       [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:33.8883749Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:302:22:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-06T14:24:33.8885789Z   302 |         if (freq_db_.[01;31m[Kentry_count[m[K() == 0) {
2025-11-06T14:24:33.8886142Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.8887428Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:308:22:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-06T14:24:33.8888482Z   308 |         if (freq_db_.[01;31m[Kentry_count[m[K() > 100) {
2025-11-06T14:24:33.8888801Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.8889632Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:313:14:[m[K [01;31m[Kerror: [m[Kexception handling disabled, use '[01m[K-fexceptions[m[K' to enable
2025-11-06T14:24:33.8890365Z   313 |     } catch ([01;31m[K...[m[K) {
2025-11-06T14:24:33.8890639Z       |              [01;31m[K^~~[m[K
2025-11-06T14:24:33.8891745Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Ksize_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_database_size() const[m[K':
2025-11-06T14:24:33.8893559Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:319:21:[m[K [01;31m[Kerror: [m[K'[01m[Kconst freqman_db[m[K' {aka '[01m[Kconst class std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kis_open[m[K'
2025-11-06T14:24:33.8894564Z   319 |     return freq_db_.[01;31m[Kis_open[m[K() ? freq_db_.entry_count() : 0;
2025-11-06T14:24:33.8894929Z       |                     [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:33.8896033Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:319:42:[m[K [01;31m[Kerror: [m[K'[01m[Kconst freqman_db[m[K' {aka '[01m[Kconst class std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-06T14:24:33.8897036Z   319 |     return freq_db_.is_open() ? freq_db_.[01;31m[Kentry_count[m[K() : 0;
2025-11-06T14:24:33.8897418Z       |                                          [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.8898641Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K':
2025-11-06T14:24:33.8900610Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:359:19:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kis_open[m[K'
2025-11-06T14:24:33.8901707Z   359 |     if (!freq_db_.[01;31m[Kis_open[m[K() || freq_db_.entry_count() == 0) {
2025-11-06T14:24:33.8902067Z       |                   [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:33.8903389Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:359:41:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-06T14:24:33.8904489Z   359 |     if (!freq_db_.is_open() || freq_db_.[01;31m[Kentry_count[m[K() == 0) {
2025-11-06T14:24:33.8904860Z       |                                         [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.8906217Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:367:43:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-06T14:24:33.8907327Z   367 |     const size_t total_entries = freq_db_.[01;31m[Kentry_count[m[K();
2025-11-06T14:24:33.8907721Z       |                                           [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.8909038Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:372:38:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kget_entry[m[K'
2025-11-06T14:24:33.8910169Z   372 |     const auto& entry_opt = freq_db_.[01;31m[Kget_entry[m[K(current_db_index_);
2025-11-06T14:24:33.8910577Z       |                                      [01;31m[K^~~~~~~~~[m[K
2025-11-06T14:24:33.8911907Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_wideband_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K':
2025-11-06T14:24:33.8914269Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:402:46:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[K(Frequency)current_slice.WidebandSlice::center_frequency[m[K' from '[01m[KFrequency[m[K' {aka '[01m[Klong long unsigned int[m[K'} to '[01m[Kint64_t[m[K' {aka '[01m[Klong long int[m[K'} [[01;35m[K-Wnarrowing[m[K]
2025-11-06T14:24:33.8915527Z   402 |                 .frequency_a = [01;35m[Kcurrent_slice.center_frequency[m[K,
2025-11-06T14:24:33.8915917Z       |                                [01;35m[K~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8917363Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:403:46:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[K(Frequency)current_slice.WidebandSlice::center_frequency[m[K' from '[01m[KFrequency[m[K' {aka '[01m[Klong long unsigned int[m[K'} to '[01m[Kint64_t[m[K' {aka '[01m[Klong long int[m[K'} [[01;35m[K-Wnarrowing[m[K]
2025-11-06T14:24:33.8918607Z   403 |                 .frequency_b = [01;35m[Kcurrent_slice.center_frequency[m[K,
2025-11-06T14:24:33.8918978Z       |                                [01;35m[K~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8919917Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:404:25:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kuint8_t[m[K' {aka '[01m[Kunsigned char[m[K'} to '[01m[Kfreqman_type[m[K' in initialization
2025-11-06T14:24:33.8927998Z   404 |                 .type = [01;31m[Kstatic_cast<uint8_t>(freqman_type::Single)[m[K,
2025-11-06T14:24:33.8928567Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8928899Z       |                         [01;31m[K|[m[K
2025-11-06T14:24:33.8929258Z       |                         [01;31m[Kuint8_t {aka unsigned char}[m[K
2025-11-06T14:24:33.8930410Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:409:13:[m[K [01;31m[Kerror: [m[Kdesignator order for field '[01m[Kfreqman_entry::description[m[K' does not match declaration order in '[01m[Kfreqman_entry[m[K'
2025-11-06T14:24:33.8931321Z   409 |             [01;31m[K}[m[K;
2025-11-06T14:24:33.8931583Z       |             [01;31m[K^[m[K
2025-11-06T14:24:33.8940660Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::process_wideband_detection_with_override(const freqman_entry&, int32_t, int32_t, int32_t)[m[K':
2025-11-06T14:24:33.8942436Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:454:9:[m[K [01;31m[Kerror: [m[K'[01m[Keffective_threshold[m[K' was not declared in this scope
2025-11-06T14:24:33.8943501Z   454 |         [01;31m[Keffective_threshold[m[K = wideband_threshold + HYSTERESIS_MARGIN_DB;
2025-11-06T14:24:33.8943899Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8976472Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:457:17:[m[K [01;31m[Kerror: [m[K'[01m[Keffective_threshold[m[K' was not declared in this scope
2025-11-06T14:24:33.8977332Z   457 |     if (rssi >= [01;31m[Keffective_threshold[m[K) {
2025-11-06T14:24:33.8977654Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8983479Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:430:68:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Koriginal_threshold[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-06T14:24:33.8984679Z   430 |                                                            [01;35m[Kint32_t original_threshold[m[K, int32_t wideband_threshold) {
2025-11-06T14:24:33.8985290Z       |                                                            [01;35m[K~~~~~~~~^~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8988312Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::process_rssi_detection(const freqman_entry&, int32_t)[m[K':
2025-11-06T14:24:33.8990244Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:505:44:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Klookup_frequency[m[K'
2025-11-06T14:24:33.8991479Z   505 |     const auto* db_entry = drone_database_.[01;31m[Klookup_frequency[m[K(entry.frequency_a);
2025-11-06T14:24:33.8991946Z       |                                            [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.8997604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::update_tracked_drone(DroneType, Frequency, int32_t, ThreatLevel)[m[K':
2025-11-06T14:24:33.8999270Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:605:50:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KTrackedDrone& TrackedDrone::operator=(const TrackedDrone&)[m[K'
2025-11-06T14:24:33.9001037Z   605 |     tracked_drones_[oldest_index] = TrackedDrone([01;31m[K)[m[K;
2025-11-06T14:24:33.9001756Z       |                                                  [01;31m[K^[m[K
2025-11-06T14:24:33.9002780Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9003853Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:127:19:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-06T14:24:33.9004586Z   127 |     TrackedDrone& [01;36m[Koperator[m[K=(const TrackedDrone&) = delete;
2025-11-06T14:24:33.9004968Z       |                   [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:33.9006023Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::remove_stale_drones()[m[K':
2025-11-06T14:24:33.9008671Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:625:46:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KTrackedDrone& TrackedDrone::operator=(const TrackedDrone&)[m[K'
2025-11-06T14:24:33.9010561Z   625 |                 tracked_drones_[write_idx] = [01;31m[Kdrone[m[K;
2025-11-06T14:24:33.9011190Z       |                                              [01;31m[K^~~~~[m[K
2025-11-06T14:24:33.9012210Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9014081Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:127:19:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-06T14:24:33.9015253Z   127 |     TrackedDrone& [01;36m[Koperator[m[K=(const TrackedDrone&) = delete;
2025-11-06T14:24:33.9015646Z       |                   [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:33.9016660Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:629:54:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KTrackedDrone& TrackedDrone::operator=(const TrackedDrone&)[m[K'
2025-11-06T14:24:33.9017580Z   629 |             tracked_drones_[read_idx] = TrackedDrone([01;31m[K)[m[K;
2025-11-06T14:24:33.9017960Z       |                                                      [01;31m[K^[m[K
2025-11-06T14:24:33.9018717Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9019658Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:127:19:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-06T14:24:33.9020358Z   127 |     TrackedDrone& [01;36m[Koperator[m[K=(const TrackedDrone&) = delete;
2025-11-06T14:24:33.9020735Z       |                   [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:33.9021769Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[KFrequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const[m[K':
2025-11-06T14:24:33.9023715Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:667:18:[m[K [01;31m[Kerror: [m[K'[01m[Kconst freqman_db[m[K' {aka '[01m[Kconst class std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kis_open[m[K'
2025-11-06T14:24:33.9024748Z   667 |     if (freq_db_.[01;31m[Kis_open[m[K() && current_db_index_ < freq_db_.entry_count()) {
2025-11-06T14:24:33.9025124Z       |                  [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:33.9026216Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:667:60:[m[K [01;31m[Kerror: [m[K'[01m[Kconst freqman_db[m[K' {aka '[01m[Kconst class std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-06T14:24:33.9027241Z   667 |     if (freq_db_.is_open() && current_db_index_ < freq_db_.[01;31m[Kentry_count[m[K()) {
2025-11-06T14:24:33.9027674Z       |                                                            [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.9028786Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:668:42:[m[K [01;31m[Kerror: [m[K'[01m[Kconst freqman_db[m[K' {aka '[01m[Kconst class std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kget_entry[m[K'
2025-11-06T14:24:33.9029814Z   668 |         const auto& entry_opt = freq_db_.[01;31m[Kget_entry[m[K(current_db_index_);
2025-11-06T14:24:33.9030214Z       |                                          [01;31m[K^~~~~~~~~[m[K
2025-11-06T14:24:33.9031201Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const[m[K':
2025-11-06T14:24:33.9033229Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:94:[m[K [01;31m[Kerror: [m[K'[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::DroneDetectionLogger::format_session_summary(size_t, size_t) const[m[K' is private within this context
2025-11-06T14:24:33.9034482Z   681 |     return detection_logger_.format_session_summary(get_scan_cycles(), get_total_detections()[01;31m[K)[m[K;
2025-11-06T14:24:33.9035023Z       |                                                                                              [01;31m[K^[m[K
2025-11-06T14:24:33.9035645Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9037456Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:293:17:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-06T14:24:33.9038752Z   293 |     std::string [01;36m[Kformat_session_summary[m[K(size_t scan_cycles, size_t total_detections) const;
2025-11-06T14:24:33.9039228Z       |                 [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9039808Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:33.9041321Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:689:22:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDetectionLogger[m[K' in '[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' does not name a type
2025-11-06T14:24:33.9042406Z   689 | inline DroneScanner::[01;31m[KDroneDetectionLogger[m[K::DroneDetectionLogger()
2025-11-06T14:24:33.9042832Z       |                      [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9044157Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:694:22:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDetectionLogger[m[K' in '[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' does not name a type
2025-11-06T14:24:33.9045205Z   694 | inline DroneScanner::[01;31m[KDroneDetectionLogger[m[K::~DroneDetectionLogger() {
2025-11-06T14:24:33.9045640Z       |                      [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9046655Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:698:27:[m[K [01;31m[Kerror: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has not been declared
2025-11-06T14:24:33.9047635Z   698 | inline void DroneScanner::[01;31m[KDroneDetectionLogger[m[K::start_session() {
2025-11-06T14:24:33.9048056Z       |                           [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9048921Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::start_session()[m[K':
2025-11-06T14:24:33.9050149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:699:9:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-06T14:24:33.9050864Z   699 |     if ([01;31m[Ksession_active_[m[K) return;
2025-11-06T14:24:33.9051159Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9063687Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:700:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-06T14:24:33.9064453Z   700 |     [01;31m[Ksession_active_[m[K = true;
2025-11-06T14:24:33.9064738Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9081613Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:701:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_start_[m[K' was not declared in this scope
2025-11-06T14:24:33.9082598Z   701 |     [01;31m[Ksession_start_[m[K = chTimeNow();
2025-11-06T14:24:33.9082892Z       |     [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9098953Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:702:5:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-06T14:24:33.9099677Z   702 |     [01;31m[Klogged_count_[m[K = 0;
2025-11-06T14:24:33.9099954Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9118398Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:703:5:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-06T14:24:33.9119358Z   703 |     [01;31m[Kheader_written_[m[K = false;
2025-11-06T14:24:33.9119732Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9120428Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:33.9122085Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:706:27:[m[K [01;31m[Kerror: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has not been declared
2025-11-06T14:24:33.9123772Z   706 | inline void DroneScanner::[01;31m[KDroneDetectionLogger[m[K::end_session() {
2025-11-06T14:24:33.9124336Z       |                           [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9136687Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::end_session()[m[K':
2025-11-06T14:24:33.9137954Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:707:10:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-06T14:24:33.9138673Z   707 |     if (![01;31m[Ksession_active_[m[K) return;
2025-11-06T14:24:33.9138993Z       |          [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9154171Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:708:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-06T14:24:33.9154953Z   708 |     [01;31m[Ksession_active_[m[K = false;
2025-11-06T14:24:33.9155255Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9155812Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:33.9157107Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:711:27:[m[K [01;31m[Kerror: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has not been declared
2025-11-06T14:24:33.9158207Z   711 | inline bool DroneScanner::[01;31m[KDroneDetectionLogger[m[K::log_detection(const DetectionLogEntry& entry) {
2025-11-06T14:24:33.9158718Z       |                           [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9172345Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::log_detection(const DetectionLogEntry&)[m[K':
2025-11-06T14:24:33.9173831Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:712:10:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-06T14:24:33.9174575Z   712 |     if (![01;31m[Ksession_active_[m[K) return false;
2025-11-06T14:24:33.9174889Z       |          [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9191609Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:713:10:[m[K [01;31m[Kerror: [m[K'[01m[Kensure_csv_header[m[K' was not declared in this scope
2025-11-06T14:24:33.9192680Z   713 |     if (![01;31m[Kensure_csv_header[m[K()) return false;
2025-11-06T14:24:33.9193263Z       |          [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9211423Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:715:29:[m[K [01;31m[Kerror: [m[K'[01m[Kformat_csv_entry[m[K' was not declared in this scope
2025-11-06T14:24:33.9212803Z   715 |     std::string csv_entry = [01;31m[Kformat_csv_entry[m[K(entry);
2025-11-06T14:24:33.9213607Z       |                             [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9220286Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:18:[m[K [01;31m[Kerror: [m[K'[01m[Kcsv_log_[m[K' was not declared in this scope
2025-11-06T14:24:33.9221160Z   716 |     auto error = [01;31m[Kcsv_log_[m[K.append(generate_log_filename().string());
2025-11-06T14:24:33.9221699Z       |                  [01;31m[K^~~~~~~~[m[K
2025-11-06T14:24:33.9241473Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:34:[m[K [01;31m[Kerror: [m[K'[01m[Kgenerate_log_filename[m[K' was not declared in this scope
2025-11-06T14:24:33.9242351Z   716 |     auto error = csv_log_.append([01;31m[Kgenerate_log_filename[m[K().string());
2025-11-06T14:24:33.9243179Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9260267Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:720:9:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-06T14:24:33.9260987Z   720 |         [01;31m[Klogged_count_[m[K++;
2025-11-06T14:24:33.9261274Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9261997Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:33.9263816Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:726:27:[m[K [01;31m[Kerror: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has not been declared
2025-11-06T14:24:33.9264851Z   726 | inline bool DroneScanner::[01;31m[KDroneDetectionLogger[m[K::ensure_csv_header() {
2025-11-06T14:24:33.9265289Z       |                           [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9279091Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::ensure_csv_header()[m[K':
2025-11-06T14:24:33.9280369Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:727:9:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-06T14:24:33.9281108Z   727 |     if ([01;31m[Kheader_written_[m[K) return true;
2025-11-06T14:24:33.9281421Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9287659Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:729:18:[m[K [01;31m[Kerror: [m[K'[01m[Kcsv_log_[m[K' was not declared in this scope
2025-11-06T14:24:33.9288539Z   729 |     auto error = [01;31m[Kcsv_log_[m[K.append(generate_log_filename());
2025-11-06T14:24:33.9288932Z       |                  [01;31m[K^~~~~~~~[m[K
2025-11-06T14:24:33.9307029Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:729:34:[m[K [01;31m[Kerror: [m[K'[01m[Kgenerate_log_filename[m[K' was not declared in this scope
2025-11-06T14:24:33.9308214Z   729 |     auto error = csv_log_.append([01;31m[Kgenerate_log_filename[m[K());
2025-11-06T14:24:33.9308713Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9325038Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:733:9:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-06T14:24:33.9326541Z   733 |         [01;31m[Kheader_written_[m[K = true;
2025-11-06T14:24:33.9327141Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9328298Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:33.9330169Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:739:34:[m[K [01;31m[Kerror: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has not been declared
2025-11-06T14:24:33.9331637Z   739 | inline std::string DroneScanner::[01;31m[KDroneDetectionLogger[m[K::format_csv_entry(const DetectionLogEntry& entry) {
2025-11-06T14:24:33.9332324Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9333861Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::format_csv_entry(const DetectionLogEntry&)[m[K':
2025-11-06T14:24:33.9336174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:16:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9337502Z   743 |              "[01;35m[K%u[m[K,%u,%d,%u,%u,%u,%.2f\n",
2025-11-06T14:24:33.9337860Z       |               [01;35m[K~^[m[K
2025-11-06T14:24:33.9338164Z       |                [01;35m[K|[m[K
2025-11-06T14:24:33.9338464Z       |                [01;35m[Kunsigned int[m[K
2025-11-06T14:24:33.9338734Z       |               [32m[K%lu[m[K
2025-11-06T14:24:33.9339343Z   744 |              [32m[Kentry.timestamp[m[K, entry.frequency_hz, entry.rssi_db,
2025-11-06T14:24:33.9340056Z       |              [32m[K~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9340535Z       |                    [32m[K|[m[K
2025-11-06T14:24:33.9341063Z       |                    [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9342323Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:19:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9343487Z   743 |              "%u,[01;35m[K%u[m[K,%d,%u,%u,%u,%.2f\n",
2025-11-06T14:24:33.9343786Z       |                  [01;35m[K~^[m[K
2025-11-06T14:24:33.9344039Z       |                   [01;35m[K|[m[K
2025-11-06T14:24:33.9344313Z       |                   [01;35m[Kunsigned int[m[K
2025-11-06T14:24:33.9344583Z       |                  [32m[K%lu[m[K
2025-11-06T14:24:33.9344960Z   744 |              entry.timestamp, [32m[Kentry.frequency_hz[m[K, entry.rssi_db,
2025-11-06T14:24:33.9345335Z       |                               [32m[K~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9345628Z       |                                     [32m[K|[m[K
2025-11-06T14:24:33.9345971Z       |                                     [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9347070Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:22:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 6 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9348288Z   743 |              "%u,%u,[01;35m[K%d[m[K,%u,%u,%u,%.2f\n",
2025-11-06T14:24:33.9348865Z       |                     [01;35m[K~^[m[K
2025-11-06T14:24:33.9349371Z       |                      [01;35m[K|[m[K
2025-11-06T14:24:33.9350062Z       |                      [01;35m[Kint[m[K
2025-11-06T14:24:33.9350407Z       |                     [32m[K%ld[m[K
2025-11-06T14:24:33.9350805Z   744 |              entry.timestamp, entry.frequency_hz, [32m[Kentry.rssi_db[m[K,
2025-11-06T14:24:33.9351212Z       |                                                   [32m[K~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9351523Z       |                                                         [32m[K|[m[K
2025-11-06T14:24:33.9351861Z       |                                                         [32m[Kint32_t {aka long int}[m[K
2025-11-06T14:24:33.9352468Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:33.9353927Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:751:34:[m[K [01;31m[Kerror: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has not been declared
2025-11-06T14:24:33.9355006Z   751 | inline std::string DroneScanner::[01;31m[KDroneDetectionLogger[m[K::generate_log_filename() const {
2025-11-06T14:24:33.9355493Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9356735Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:751:80:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::generate_log_filename()[m[K' cannot have cv-qualifier
2025-11-06T14:24:33.9357905Z   751 | inline std::string DroneScanner::DroneDetectionLogger::generate_log_filename() [01;31m[Kconst[m[K {
2025-11-06T14:24:33.9358429Z       |                                                                                [01;31m[K^~~~~[m[K
2025-11-06T14:24:33.9359450Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:755:34:[m[K [01;31m[Kerror: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has not been declared
2025-11-06T14:24:33.9360662Z   755 | inline std::string DroneScanner::[01;31m[KDroneDetectionLogger[m[K::format_session_summary(size_t scan_cycles, size_t total_detections) const {
2025-11-06T14:24:33.9361275Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9362447Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:755:124:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::format_session_summary(size_t, size_t)[m[K' cannot have cv-qualifier
2025-11-06T14:24:33.9363904Z   755 | inline std::string DroneScanner::DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) [01;31m[Kconst[m[K {
2025-11-06T14:24:33.9364970Z       |                                                                                                                            [01;31m[K^~~~~[m[K
2025-11-06T14:24:33.9366790Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::format_session_summary(size_t, size_t)[m[K':
2025-11-06T14:24:33.9368122Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:756:50:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_start_[m[K' was not declared in this scope
2025-11-06T14:24:33.9368945Z   756 |     uint32_t session_duration_ms = chTimeNow() - [01;31m[Ksession_start_[m[K;
2025-11-06T14:24:33.9369367Z       |                                                  [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9383949Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:766:58:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-06T14:24:33.9384843Z   766 |         avg_detections_per_cycle, detections_per_second, [01;31m[Klogged_count_[m[K);
2025-11-06T14:24:33.9385476Z       |                                                          [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9404218Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::DroneHardwareController(SpectrumMode)[m[K':
2025-11-06T14:24:33.9407187Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:779:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_spectrum_config_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9409057Z   779 | [01;35m[KDroneHardwareController[m[K::DroneHardwareController(SpectrumMode mode)
2025-11-06T14:24:33.9409818Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9410888Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:781:63:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-06T14:24:33.9412157Z   781 |       spectrum_streaming_active_(false), last_valid_rssi_(-120[01;31m[K)[m[K
2025-11-06T14:24:33.9412594Z       |                                                               [01;31m[K^[m[K
2025-11-06T14:24:33.9413281Z In file included from [01m[K/havoc/firmware/application/ui/ui_rssi.hpp:28[m[K,
2025-11-06T14:24:33.9413801Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:40[m[K,
2025-11-06T14:24:33.9414436Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9415196Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9415943Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9417496Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-06T14:24:33.9419084Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-06T14:24:33.9419654Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9420543Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-06T14:24:33.9422804Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-06T14:24:33.9424553Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-06T14:24:33.9425186Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9426224Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-06T14:24:33.9428429Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:779:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_frame_sync_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9429723Z   779 | [01;35m[KDroneHardwareController[m[K::DroneHardwareController(SpectrumMode mode)
2025-11-06T14:24:33.9430142Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9431150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:781:63:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-06T14:24:33.9432280Z   781 |       spectrum_streaming_active_(false), last_valid_rssi_(-120[01;31m[K)[m[K
2025-11-06T14:24:33.9432696Z       |                                                               [01;31m[K^[m[K
2025-11-06T14:24:33.9433285Z In file included from [01m[K/havoc/firmware/application/ui/ui_rssi.hpp:28[m[K,
2025-11-06T14:24:33.9433792Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:40[m[K,
2025-11-06T14:24:33.9434419Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9435344Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9436710Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9438065Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-06T14:24:33.9439359Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-06T14:24:33.9440171Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9441241Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-06T14:24:33.9442865Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-06T14:24:33.9444621Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-06T14:24:33.9445218Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9445935Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-06T14:24:33.9447443Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:779:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::radio_state_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9448661Z   779 | [01;35m[KDroneHardwareController[m[K::DroneHardwareController(SpectrumMode mode)
2025-11-06T14:24:33.9449076Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9450110Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneHardwareController::initialize_spectrum_collector()[m[K':
2025-11-06T14:24:33.9451813Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:813:22:[m[K [01;31m[Kerror: [m[K'[01m[KChannelSpectrumConfigChange[m[K' is not a member of '[01m[KMessage::ID[m[K'; did you mean '[01m[KChannelSpectrumConfig[m[K'?
2025-11-06T14:24:33.9452779Z   813 |         Message::ID::[01;31m[KChannelSpectrumConfigChange[m[K,
2025-11-06T14:24:33.9453299Z       |                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9453626Z       |                      [32m[KChannelSpectrumConfig[m[K
2025-11-06T14:24:33.9455044Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:818:87:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K'
2025-11-06T14:24:33.9457045Z   818 |         [this](const Message* const p) { (void)p; process_channel_spectrum_data({}); }[01;31m[K)[m[K;
2025-11-06T14:24:33.9457869Z       |                                                                                       [01;31m[K^[m[K
2025-11-06T14:24:33.9458555Z In file included from [01m[K/havoc/firmware/application/ui/ui_rssi.hpp:28[m[K,
2025-11-06T14:24:33.9459058Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:40[m[K,
2025-11-06T14:24:33.9459688Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9460434Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9461173Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9462552Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K'[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K' is implicitly deleted because the default definition would be ill-formed:
2025-11-06T14:24:33.9463724Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-06T14:24:33.9464058Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9465144Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;31m[Kerror: [m[Knon-static const member '[01m[Kconst Message::ID MessageHandlerRegistration::message_id[m[K', can't use default assignment operator
2025-11-06T14:24:33.9467708Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::tune_to_frequency(Frequency)[m[K':
2025-11-06T14:24:33.9470638Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:846:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing RxRadioState = class RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K' {aka '[01m[Kclass RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K'} has no member named '[01m[Ktune_rf[m[K'
2025-11-06T14:24:33.9471835Z   846 |     radio_state_.[01;31m[Ktune_rf[m[K(frequency_hz);
2025-11-06T14:24:33.9472164Z       |                  [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:33.9473300Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneHardwareController::start_spectrum_streaming()[m[K':
2025-11-06T14:24:33.9476283Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:853:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing RxRadioState = class RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K' {aka '[01m[Kclass RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K'} has no member named '[01m[Kreceiver_model[m[K'
2025-11-06T14:24:33.9477574Z   853 |     radio_state_.[01;31m[Kreceiver_model[m[K.start_baseband_streaming();
2025-11-06T14:24:33.9477963Z       |                  [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9479003Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneHardwareController::stop_spectrum_streaming()[m[K':
2025-11-06T14:24:33.9482056Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:858:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing RxRadioState = class RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K' {aka '[01m[Kclass RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K'} has no member named '[01m[Kreceiver_model[m[K'
2025-11-06T14:24:33.9484374Z   858 |     radio_state_.[01;31m[Kreceiver_model[m[K.stop_baseband_streaming();
2025-11-06T14:24:33.9484754Z       |                  [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9486042Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kint32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::get_real_rssi_from_hardware(Frequency)[m[K':
2025-11-06T14:24:33.9487562Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:861:72:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Ktarget_frequency[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-06T14:24:33.9488568Z   861 | int32_t DroneHardwareController::get_real_rssi_from_hardware([01;35m[KFrequency target_frequency[m[K) {
2025-11-06T14:24:33.9489104Z       |                                                              [01;35m[K~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9490223Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::update(ThreatLevel, size_t, size_t, size_t, Frequency, bool)[m[K':
2025-11-06T14:24:33.9491746Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:910:26:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::ProgressBar[m[K' has no member named '[01m[Kset_color[m[K'
2025-11-06T14:24:33.9492725Z   910 |     threat_progress_bar_.[01;31m[Kset_color[m[K(get_threat_bar_color(max_threat));
2025-11-06T14:24:33.9493310Z       |                          [01;31m[K^~~~~~~~~[m[K
2025-11-06T14:24:33.9494201Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:924:56:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-06T14:24:33.9495059Z   924 |     threat_status_main_.set_style([01;31m[Kget_threat_text_color(max_threat)[m[K);
2025-11-06T14:24:33.9495494Z       |                                   [01;31m[K~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9495836Z       |                                                        [01;31m[K|[m[K
2025-11-06T14:24:33.9496202Z       |                                                        [01;31m[Kui::Color[m[K
2025-11-06T14:24:33.9496647Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9497314Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9498056Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9498794Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9499712Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-06T14:24:33.9500349Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-06T14:24:33.9500693Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-06T14:24:33.9501567Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:946:54:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-06T14:24:33.9502429Z   946 |     threat_frequency_.set_style([01;31m[Kget_threat_text_color(max_threat)[m[K);
2025-11-06T14:24:33.9502867Z       |                                 [01;31m[K~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9503315Z       |                                                      [01;31m[K|[m[K
2025-11-06T14:24:33.9503673Z       |                                                      [01;31m[Kui::Color[m[K
2025-11-06T14:24:33.9504112Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9504768Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9505641Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9506384Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9507593Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-06T14:24:33.9508777Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-06T14:24:33.9509417Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-06T14:24:33.9510768Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::paint(ui::Painter&)[m[K':
2025-11-06T14:24:33.9512238Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1020:41:[m[K [01;31m[Kerror: [m[Kinvalid use of non-static member function '[01m[Kuint8_t ui::Color::r()[m[K'
2025-11-06T14:24:33.9513437Z  1020 |         pulse_color = Color([01;31m[Kpulse_color.r[m[K, pulse_color.g, pulse_color.b, alpha);
2025-11-06T14:24:33.9513888Z       |                             [01;31m[K~~~~~~~~~~~~^[m[K
2025-11-06T14:24:33.9514459Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:25[m[K,
2025-11-06T14:24:33.9515530Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:22[m[K,
2025-11-06T14:24:33.9516978Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9518133Z [01m[K/havoc/firmware/common/ui.hpp:119:13:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-06T14:24:33.9518766Z   119 |     uint8_t [01;36m[Kr[m[K() {
2025-11-06T14:24:33.9519279Z       |             [01;36m[K^[m[K
2025-11-06T14:24:33.9520961Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1021:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope; did you mean '[01m[Kparent_rect[m[K'?
2025-11-06T14:24:33.9522632Z  1021 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect_.width(), 4}, pulse_color);
2025-11-06T14:24:33.9523322Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9523633Z       |                                 [32m[Kparent_rect[m[K
2025-11-06T14:24:33.9524652Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1021:111:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-06T14:24:33.9526028Z  1021 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 4}, pulse_color[01;31m[K)[m[K;
2025-11-06T14:24:33.9526939Z       |                                                                                                               [01;31m[K^[m[K
2025-11-06T14:24:33.9527522Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-06T14:24:33.9528022Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-06T14:24:33.9528645Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9529623Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9530970Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9532333Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-06T14:24:33.9533156Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-06T14:24:33.9533497Z       |                         [01;36m[K~~~~~^[m[K
2025-11-06T14:24:33.9534501Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ThreatCard::update_card(const DisplayDroneEntry&)[m[K':
2025-11-06T14:24:33.9535965Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1040:45:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-06T14:24:33.9536775Z  1040 |     card_text_.set_style([01;31m[Kget_card_text_color()[m[K);
2025-11-06T14:24:33.9537145Z       |                          [01;31m[K~~~~~~~~~~~~~~~~~~~^~[m[K
2025-11-06T14:24:33.9537446Z       |                                             [01;31m[K|[m[K
2025-11-06T14:24:33.9537788Z       |                                             [01;31m[Kui::Color[m[K
2025-11-06T14:24:33.9538371Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9539069Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9539824Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9540559Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9541483Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-06T14:24:33.9542353Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-06T14:24:33.9543137Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-06T14:24:33.9545006Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::ThreatCard::render_compact() const[m[K':
2025-11-06T14:24:33.9547027Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1064:79:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 8 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9548624Z  1064 |         snprintf(buffer, sizeof(buffer), "🛰️ %s │ %.1fG │ %s %s │ [01;35m[K%d[m[KdB",
2025-11-06T14:24:33.9549210Z       |                                                                              [01;35m[K~^[m[K
2025-11-06T14:24:33.9549551Z       |                                                                               [01;35m[K|[m[K
2025-11-06T14:24:33.9549901Z       |                                                                               [01;35m[Kint[m[K
2025-11-06T14:24:33.9550241Z       |                                                                              [32m[K%ld[m[K
2025-11-06T14:24:33.9550675Z  1065 |                 threat_name_.c_str(), freq_mhz, trend_symbol, threat_abbr, [32m[Krssi_[m[K);
2025-11-06T14:24:33.9551135Z       |                                                                            [32m[K~~~~~[m[K
2025-11-06T14:24:33.9551465Z       |                                                                            [32m[K|[m[K
2025-11-06T14:24:33.9551829Z       |                                                                            [32m[Kint32_t {aka long int}[m[K
2025-11-06T14:24:33.9553831Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1067:79:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 8 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9555220Z  1067 |         snprintf(buffer, sizeof(buffer), "🛰️ %s │ %.0fM │ %s %s │ [01;35m[K%d[m[KdB",
2025-11-06T14:24:33.9555784Z       |                                                                              [01;35m[K~^[m[K
2025-11-06T14:24:33.9556393Z       |                                                                               [01;35m[K|[m[K
2025-11-06T14:24:33.9557073Z       |                                                                               [01;35m[Kint[m[K
2025-11-06T14:24:33.9557758Z       |                                                                              [32m[K%ld[m[K
2025-11-06T14:24:33.9558563Z  1068 |                 threat_name_.c_str(), freq_mhz, trend_symbol, threat_abbr, [32m[Krssi_[m[K);
2025-11-06T14:24:33.9559298Z       |                                                                            [32m[K~~~~~[m[K
2025-11-06T14:24:33.9560036Z       |                                                                            [32m[K|[m[K
2025-11-06T14:24:33.9560825Z       |                                                                            [32m[Kint32_t {aka long int}[m[K
2025-11-06T14:24:33.9562698Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::ThreatCard::paint(ui::Painter&)[m[K':
2025-11-06T14:24:33.9565462Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1101:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope; did you mean '[01m[Kparent_rect[m[K'?
2025-11-06T14:24:33.9567512Z  1101 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect_.width(), 2}, bg_color);
2025-11-06T14:24:33.9568467Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9569045Z       |                                 [32m[Kparent_rect[m[K
2025-11-06T14:24:33.9570737Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1101:108:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-06T14:24:33.9571846Z  1101 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2}, bg_color[01;31m[K)[m[K;
2025-11-06T14:24:33.9572427Z       |                                                                                                            [01;31m[K^[m[K
2025-11-06T14:24:33.9572866Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-06T14:24:33.9573525Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-06T14:24:33.9574158Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9574912Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9576272Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9577386Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-06T14:24:33.9578046Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-06T14:24:33.9578376Z       |                         [01;36m[K~~~~~^[m[K
2025-11-06T14:24:33.9579719Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)[m[K':
2025-11-06T14:24:33.9581218Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1114:28:[m[K [01;31m[Kerror: [m[Kinitializer-string for array of chars is too long [[01;31m[K-fpermissive[m[K]
2025-11-06T14:24:33.9582619Z  1114 |     char progress_bar[9] = [01;31m[K"░░░░░░░░"[m[K;
2025-11-06T14:24:33.9583478Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9584598Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1117:27:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849672[m[K' to '[01m[K'\210'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-06T14:24:33.9585551Z  1117 |         progress_bar[i] = [01;35m[K'█'[m[K;
2025-11-06T14:24:33.9585882Z       |                           [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.9587221Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1121:43:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9588341Z  1121 |     snprintf(buffer, sizeof(buffer), "%s [01;35m[K%u[m[K%% C:%u D:%u",
2025-11-06T14:24:33.9588738Z       |                                          [01;35m[K~^[m[K
2025-11-06T14:24:33.9589198Z       |                                           [01;35m[K|[m[K
2025-11-06T14:24:33.9589807Z       |                                           [01;35m[Kunsigned int[m[K
2025-11-06T14:24:33.9590411Z       |                                          [32m[K%lu[m[K
2025-11-06T14:24:33.9591180Z  1122 |             progress_bar, [32m[Kprogress_percent[m[K, total_cycles, detections);
2025-11-06T14:24:33.9591950Z       |                           [32m[K~~~~~~~~~~~~~~~~[m[K 
2025-11-06T14:24:33.9592406Z       |                           [32m[K|[m[K
2025-11-06T14:24:33.9592773Z       |                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9594297Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1121:50:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 6 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9595392Z  1121 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:[01;35m[K%u[m[K D:%u",
2025-11-06T14:24:33.9595789Z       |                                                 [01;35m[K~^[m[K
2025-11-06T14:24:33.9596104Z       |                                                  [01;35m[K|[m[K
2025-11-06T14:24:33.9596426Z       |                                                  [01;35m[Kunsigned int[m[K
2025-11-06T14:24:33.9596749Z       |                                                 [32m[K%lu[m[K
2025-11-06T14:24:33.9597170Z  1122 |             progress_bar, progress_percent, [32m[Ktotal_cycles[m[K, detections);
2025-11-06T14:24:33.9597576Z       |                                             [32m[K~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9597886Z       |                                             [32m[K|[m[K
2025-11-06T14:24:33.9598522Z       |                                             [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9600489Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1121:55:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 7 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9602629Z  1121 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:[01;35m[K%u[m[K",
2025-11-06T14:24:33.9603219Z       |                                                      [01;35m[K~^[m[K
2025-11-06T14:24:33.9603547Z       |                                                       [01;35m[K|[m[K
2025-11-06T14:24:33.9603882Z       |                                                       [01;35m[Kunsigned int[m[K
2025-11-06T14:24:33.9604212Z       |                                                      [32m[K%lu[m[K
2025-11-06T14:24:33.9604654Z  1122 |             progress_bar, progress_percent, total_cycles, [32m[Kdetections[m[K);
2025-11-06T14:24:33.9605077Z       |                                                           [32m[K~~~~~~~~~~[m[K
2025-11-06T14:24:33.9605395Z       |                                                           [32m[K|[m[K
2025-11-06T14:24:33.9605763Z       |                                                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9607102Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1128:61:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9608339Z  1128 |         snprintf(buffer, sizeof(buffer), "⚠️ DETECTED: [01;35m[K%u[m[K threats found!", [32m[Kdetections[m[K);
2025-11-06T14:24:33.9608894Z       |                                                            [01;35m[K~^[m[K                  [32m[K~~~~~~~~~~[m[K
2025-11-06T14:24:33.9609281Z       |                                                             [01;35m[K|[m[K                  [32m[K|[m[K
2025-11-06T14:24:33.9609757Z       |                                                             [01;35m[Kunsigned int[m[K       [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9610159Z       |                                                            [32m[K%lu[m[K
2025-11-06T14:24:33.9611300Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_alert_status(ThreatLevel, size_t, const string&)[m[K':
2025-11-06T14:24:33.9612675Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1146:126:[m[K [01;31m[Kerror: [m[Kexpression cannot be used as a function
2025-11-06T14:24:33.9613819Z  1146 |     alert_text_.set_style((threat >= ThreatLevel::CRITICAL) ? Theme::getInstance()->fg_red : Theme::getInstance()->fg_yellow([01;31m[K)[m[K);
2025-11-06T14:24:33.9614455Z       |                                                                                                                              [01;31m[K^[m[K
2025-11-06T14:24:33.9615562Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_normal_status(const string&, const string&)[m[K':
2025-11-06T14:24:33.9617071Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1160:60:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kconst ui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-06T14:24:33.9617998Z  1160 |     normal_text_.set_style([01;31m[KTheme::getInstance()->fg_light->foreground[m[K);
2025-11-06T14:24:33.9618457Z       |                            [01;31m[K~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~[m[K
2025-11-06T14:24:33.9618801Z       |                                                            [01;31m[K|[m[K
2025-11-06T14:24:33.9619318Z       |                                                            [01;31m[Kconst ui::Color[m[K
2025-11-06T14:24:33.9619774Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9620449Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9621200Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9621934Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9622848Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-06T14:24:33.9623615Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-06T14:24:33.9623962Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-06T14:24:33.9624957Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::paint(ui::Painter&)[m[K':
2025-11-06T14:24:33.9626546Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1183:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope; did you mean '[01m[Kparent_rect[m[K'?
2025-11-06T14:24:33.9627610Z  1183 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect_.width(), 2}, Color(32, 0, 0));
2025-11-06T14:24:33.9628130Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9628435Z       |                                 [32m[Kparent_rect[m[K
2025-11-06T14:24:33.9629410Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1183:115:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-06T14:24:33.9630490Z  1183 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2}, Color(32, 0, 0)[01;31m[K)[m[K;
2025-11-06T14:24:33.9631075Z       |                                                                                                                   [01;31m[K^[m[K
2025-11-06T14:24:33.9631500Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-06T14:24:33.9631984Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-06T14:24:33.9632611Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9633472Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9634212Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9635160Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-06T14:24:33.9635813Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-06T14:24:33.9636140Z       |                         [01;36m[K~~~~~^[m[K
2025-11-06T14:24:33.9636710Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9638005Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)[m[K':
2025-11-06T14:24:33.9639830Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:628:21:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::nav_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.9640764Z   628 |     NavigationView& [01;35m[Knav_[m[K;
2025-11-06T14:24:33.9641052Z       |                     [01;35m[K^~~~[m[K
2025-11-06T14:24:33.9642153Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:618:14:[m[K [01;35m[Kwarning: [m[K  '[01m[KGradient ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_gradient_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.9643222Z   618 |     Gradient [01;35m[Kspectrum_gradient_[m[K;
2025-11-06T14:24:33.9643538Z       |              [01;35m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9644308Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-06T14:24:33.9645172Z  1187 | [01;35m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-06T14:24:33.9645585Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9647282Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::detected_drones_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9649933Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::displayed_drones_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9651364Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9652444Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K':
2025-11-06T14:24:33.9654028Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::frequency[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9654944Z   130 | struct [01;35m[KDisplayDroneEntry[m[K {
2025-11-06T14:24:33.9655242Z       |        [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9656266Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9657891Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::threat[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9659492Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::rssi[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9661103Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::last_seen[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9662722Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type_name[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9664635Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::display_color[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9666272Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::trend[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9667429Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:15[m[K,
2025-11-06T14:24:33.9668226Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9669107Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array:[m[K In constructor '[01m[Kstd::array<DisplayDroneEntry, 3>::array()[m[K':
2025-11-06T14:24:33.9670320Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array:94:12:[m[K [01;36m[Knote: [m[Ksynthesized method '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K' first required here
2025-11-06T14:24:33.9670988Z    94 |     struct [01;36m[Karray[m[K
2025-11-06T14:24:33.9671254Z       |            [01;36m[K^~~~~[m[K
2025-11-06T14:24:33.9672292Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)[m[K':
2025-11-06T14:24:33.9673988Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1188:37:[m[K [01;36m[Knote: [m[Ksynthesized method '[01m[Kstd::array<DisplayDroneEntry, 3>::array()[m[K' first required here
2025-11-06T14:24:33.9674853Z  1188 |     : nav_(nav), spectrum_gradient_{[01;36m[K}[m[K
2025-11-06T14:24:33.9675184Z       |                                     [01;36m[K^[m[K
2025-11-06T14:24:33.9676429Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_row[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9677671Z  1187 | [01;35m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-06T14:24:33.9678087Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9680220Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_power_levels_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9682328Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::threat_bins_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9684457Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_config_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9686503Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::message_handler_spectrum_config_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9688487Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1188:37:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-06T14:24:33.9689375Z  1188 |     : nav_(nav), spectrum_gradient_{[01;31m[K}[m[K
2025-11-06T14:24:33.9689698Z       |                                     [01;31m[K^[m[K
2025-11-06T14:24:33.9690111Z In file included from [01m[K/havoc/firmware/application/ui/ui_rssi.hpp:28[m[K,
2025-11-06T14:24:33.9690613Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:40[m[K,
2025-11-06T14:24:33.9691240Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9691998Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9692734Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9694093Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-06T14:24:33.9694901Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-06T14:24:33.9695212Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9695773Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-06T14:24:33.9696915Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-06T14:24:33.9697734Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-06T14:24:33.9698063Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9698598Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-06T14:24:33.9700099Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1187:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::message_handler_frame_sync_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9701366Z  1187 | [01;35m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-06T14:24:33.9701781Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9702786Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1188:37:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-06T14:24:33.9703824Z  1188 |     : nav_(nav), spectrum_gradient_{[01;31m[K}[m[K
2025-11-06T14:24:33.9704149Z       |                                     [01;31m[K^[m[K
2025-11-06T14:24:33.9704558Z In file included from [01m[K/havoc/firmware/application/ui/ui_rssi.hpp:28[m[K,
2025-11-06T14:24:33.9705056Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:40[m[K,
2025-11-06T14:24:33.9705682Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9706424Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9707162Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9708424Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-06T14:24:33.9709219Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-06T14:24:33.9709532Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9710071Z [01m[K/havoc/firmware/application/./event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-06T14:24:33.9711505Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-06T14:24:33.9712888Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-06T14:24:33.9713583Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9714153Z [01m[K/havoc/firmware/application/./event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-06T14:24:33.9715701Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1200:10:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K'
2025-11-06T14:24:33.9716675Z  1200 |         }[01;31m[K)[m[K;
2025-11-06T14:24:33.9716925Z       |          [01;31m[K^[m[K
2025-11-06T14:24:33.9718108Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1213:10:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K'
2025-11-06T14:24:33.9719043Z  1213 |         }[01;31m[K)[m[K;
2025-11-06T14:24:33.9719283Z       |          [01;31m[K^[m[K
2025-11-06T14:24:33.9720465Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_detection_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K':
2025-11-06T14:24:33.9722273Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1228:30:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kchar*[m[K' to '[01m[Krf::Frequency[m[K' {aka '[01m[Klong long int[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-06T14:24:33.9723310Z  1228 |             big_display_.set([01;31m[Kfreq_buffer[m[K);
2025-11-06T14:24:33.9723641Z       |                              [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.9723936Z       |                              [01;31m[K|[m[K
2025-11-06T14:24:33.9724217Z       |                              [01;31m[Kchar*[m[K
2025-11-06T14:24:33.9724652Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9725317Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9726064Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9726802Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9727710Z [01m[K/havoc/firmware/common/ui_widget.hpp:290:34:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::BigFrequency::set(rf::Frequency)[m[K'
2025-11-06T14:24:33.9728375Z   290 |     void set([01;36m[Kconst rf::Frequency frequency[m[K);
2025-11-06T14:24:33.9728719Z       |              [01;36m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-06T14:24:33.9729801Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1230:30:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kconst char*[m[K' to '[01m[Krf::Frequency[m[K' {aka '[01m[Klong long int[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-06T14:24:33.9730878Z  1230 |             big_display_.set([01;31m[K"SCANNING..."[m[K);
2025-11-06T14:24:33.9731216Z       |                              [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9731503Z       |                              [01;31m[K|[m[K
2025-11-06T14:24:33.9731799Z       |                              [01;31m[Kconst char*[m[K
2025-11-06T14:24:33.9732225Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9732883Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9733745Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9734483Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9735511Z [01m[K/havoc/firmware/common/ui_widget.hpp:290:34:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::BigFrequency::set(rf::Frequency)[m[K'
2025-11-06T14:24:33.9736169Z   290 |     void set([01;36m[Kconst rf::Frequency frequency[m[K);
2025-11-06T14:24:33.9736508Z       |              [01;36m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-06T14:24:33.9737591Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1233:26:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kconst char*[m[K' to '[01m[Krf::Frequency[m[K' {aka '[01m[Klong long int[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-06T14:24:33.9738498Z  1233 |         big_display_.set([01;31m[K"READY"[m[K);
2025-11-06T14:24:33.9738814Z       |                          [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:33.9739094Z       |                          [01;31m[K|[m[K
2025-11-06T14:24:33.9739385Z       |                          [01;31m[Kconst char*[m[K
2025-11-06T14:24:33.9739812Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9740467Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9741210Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9742237Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9744160Z [01m[K/havoc/firmware/common/ui_widget.hpp:290:34:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::BigFrequency::set(rf::Frequency)[m[K'
2025-11-06T14:24:33.9745030Z   290 |     void set([01;36m[Kconst rf::Frequency frequency[m[K);
2025-11-06T14:24:33.9745387Z       |              [01;36m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-06T14:24:33.9746552Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1262:75:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9747709Z  1262 |         snprintf(status_buffer, sizeof(status_buffer), "%s - Detections: [01;35m[K%u[m[K",
2025-11-06T14:24:33.9748187Z       |                                                                          [01;35m[K~^[m[K
2025-11-06T14:24:33.9748548Z       |                                                                           [01;35m[K|[m[K
2025-11-06T14:24:33.9748904Z       |                                                                           [01;35m[Kunsigned int[m[K
2025-11-06T14:24:33.9749448Z       |                                                                          [32m[K%lu[m[K
2025-11-06T14:24:33.9749840Z  1263 |                 mode_str.c_str(), [32m[Kscanner.get_total_detections()[m[K);
2025-11-06T14:24:33.9750244Z       |                                   [32m[K~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K           
2025-11-06T14:24:33.9750587Z       |                                                               [32m[K|[m[K
2025-11-06T14:24:33.9750989Z       |                                                               [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9752168Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1273:79:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 6 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9753510Z  1273 |         snprintf(stats_buffer, sizeof(stats_buffer), "Freq: %zu/%zu | Cycle: [01;35m[K%u[m[K",
2025-11-06T14:24:33.9753978Z       |                                                                              [01;35m[K~^[m[K
2025-11-06T14:24:33.9754466Z       |                                                                               [01;35m[K|[m[K
2025-11-06T14:24:33.9754942Z       |                                                                               [01;35m[Kunsigned int[m[K
2025-11-06T14:24:33.9755660Z       |                                                                              [32m[K%lu[m[K
2025-11-06T14:24:33.9756451Z  1274 |                 current_idx + 1, loaded_freqs, [32m[Kscanner.get_scan_cycles()[m[K);
2025-11-06T14:24:33.9757220Z       |                                                [32m[K~~~~~~~~~~~~~~~~~~~~~~~~~[m[K       
2025-11-06T14:24:33.9757576Z       |                                                                       [32m[K|[m[K
2025-11-06T14:24:33.9757956Z       |                                                                       [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:33.9759178Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)[m[K':
2025-11-06T14:24:33.9760931Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1306:25:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-06T14:24:33.9762422Z  1306 |         it->type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-06T14:24:33.9763258Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9764602Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1307:29:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-06T14:24:33.9765997Z  1307 |         it->display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-06T14:24:33.9766671Z       |                             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9768146Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1316:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-06T14:24:33.9769081Z  1316 |             entry.type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-06T14:24:33.9769462Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9770284Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1317:35:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-06T14:24:33.9771293Z  1317 |             entry.display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-06T14:24:33.9771687Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9773490Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1322:27:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display()[m[K'
2025-11-06T14:24:33.9777154Z  1322 |     update_drones_display([01;31m[K)[m[K;
2025-11-06T14:24:33.9777706Z       |                           [01;31m[K^[m[K
2025-11-06T14:24:33.9778699Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9780576Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:571:10:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K'
2025-11-06T14:24:33.9781769Z   571 |     void [01;36m[Kupdate_drones_display[m[K(const DroneScanner& scanner);
2025-11-06T14:24:33.9782338Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9783287Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:571:10:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-06T14:24:33.9785386Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K':
2025-11-06T14:24:33.9787078Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1344:23:[m[K [01;31m[Kerror: [m[K'[01m[Kstruct std::array<DisplayDroneEntry, 3>[m[K' has no member named '[01m[Kclear[m[K'
2025-11-06T14:24:33.9787900Z  1344 |     displayed_drones_.[01;31m[Kclear[m[K();
2025-11-06T14:24:33.9788205Z       |                       [01;31m[K^~~~~[m[K
2025-11-06T14:24:33.9789050Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1334:72:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Kscanner[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-06T14:24:33.9790022Z  1334 | void DroneDisplayController::update_drones_display([01;35m[Kconst DroneScanner& scanner[m[K) {
2025-11-06T14:24:33.9790533Z       |                                                    [01;35m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~[m[K
2025-11-06T14:24:33.9791556Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()[m[K':
2025-11-06T14:24:33.9793480Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1354:5:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_1()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9794506Z  1354 |     [01;31m[Ktext_drone_1[m[K.set("");
2025-11-06T14:24:33.9794786Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9795038Z       |                 [32m[K()[m[K
2025-11-06T14:24:33.9796233Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1355:5:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_2()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9797245Z  1355 |     [01;31m[Ktext_drone_2[m[K.set("");
2025-11-06T14:24:33.9797697Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9797949Z       |                 [32m[K()[m[K
2025-11-06T14:24:33.9799129Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1356:5:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_3()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9800139Z  1356 |     [01;31m[Ktext_drone_3[m[K.set("");
2025-11-06T14:24:33.9800407Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9800654Z       |                 [32m[K()[m[K
2025-11-06T14:24:33.9802000Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1362:61:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849714[m[K' to '[01m[K'\262'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-06T14:24:33.9804218Z  1362 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'▲'[m[K; break;
2025-11-06T14:24:33.9804996Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.9807259Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1363:58:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849724[m[K' to '[01m[K'\274'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-06T14:24:33.9809422Z  1363 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'▼'[m[K; break;
2025-11-06T14:24:33.9809894Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.9810995Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1366:37:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849696[m[K' to '[01m[K'\240'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-06T14:24:33.9811975Z  1366 |             default: trend_symbol = [01;35m[K'■'[m[K; break;
2025-11-06T14:24:33.9812322Z       |                                     [01;35m[K^~~~~[m[K
2025-11-06T14:24:33.9814048Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1376:42:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 6 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:33.9815146Z  1376 |         snprintf(buffer, sizeof(buffer), [01;35m[KDRONE_DISPLAY_FORMAT[m[K,
2025-11-06T14:24:33.9815565Z       |                                          [01;35m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9815801Z ......
2025-11-06T14:24:33.9816016Z  1379 |                 [32m[Kdrone.rssi[m[K,
2025-11-06T14:24:33.9816328Z       |                 [32m[K~~~~~~~~~~[m[K                
2025-11-06T14:24:33.9816601Z       |                       [32m[K|[m[K
2025-11-06T14:24:33.9816901Z       |                       [32m[Kint32_t {aka long int}[m[K
2025-11-06T14:24:33.9818177Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1384:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_1()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9819221Z  1384 |                 [01;31m[Ktext_drone_1[m[K.set(buffer);
2025-11-06T14:24:33.9819528Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9819804Z       |                             [32m[K()[m[K
2025-11-06T14:24:33.9821005Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1385:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_1()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9822242Z  1385 |                 [01;31m[Ktext_drone_1[m[K.set_style(threat_color);
2025-11-06T14:24:33.9822567Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9822849Z       |                             [32m[K()[m[K
2025-11-06T14:24:33.9824296Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1388:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_2()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9825335Z  1388 |                 [01;31m[Ktext_drone_2[m[K.set(buffer);
2025-11-06T14:24:33.9825638Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9825915Z       |                             [32m[K()[m[K
2025-11-06T14:24:33.9827244Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1389:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_2()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9828308Z  1389 |                 [01;31m[Ktext_drone_2[m[K.set_style(threat_color);
2025-11-06T14:24:33.9828628Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9828902Z       |                             [32m[K()[m[K
2025-11-06T14:24:33.9830097Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1392:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_3()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9831131Z  1392 |                 [01;31m[Ktext_drone_3[m[K.set(buffer);
2025-11-06T14:24:33.9831420Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9831690Z       |                             [32m[K()[m[K
2025-11-06T14:24:33.9832901Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1393:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_3()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-06T14:24:33.9834164Z  1393 |                 [01;31m[Ktext_drone_3[m[K.set_style(threat_color);
2025-11-06T14:24:33.9834492Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9834762Z       |                             [32m[K()[m[K
2025-11-06T14:24:33.9835857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum&)[m[K':
2025-11-06T14:24:33.9838107Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1409:9:[m[K [01;31m[Kerror: [m[K'[01m[Kget_max_power_for_current_bin[m[K' was not declared in this scope
2025-11-06T14:24:33.9839390Z  1409 |         [01;31m[Kget_max_power_for_current_bin[m[K(spectrum, current_bin_power);
2025-11-06T14:24:33.9839812Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9840838Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneDisplayController::process_bins(uint8_t*)[m[K':
2025-11-06T14:24:33.9842193Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1420:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel_from_bin[m[K' was not declared in this scope
2025-11-06T14:24:33.9843355Z  1420 |             [01;31m[Kadd_spectrum_pixel_from_bin[m[K(*power_level);
2025-11-06T14:24:33.9843714Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9868150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1422:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel_from_bin[m[K' was not declared in this scope
2025-11-06T14:24:33.9869723Z  1422 |             [01;31m[Kadd_spectrum_pixel_from_bin[m[K(0);
2025-11-06T14:24:33.9870353Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9871438Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:33.9874146Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1434:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t&)[m[K'
2025-11-06T14:24:33.9875643Z  1434 | void [01;31m[KDroneDisplayController[m[K::get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t& max_power) {
2025-11-06T14:24:33.9876435Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9877693Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1434:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t&)[m[K'
2025-11-06T14:24:33.9879004Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9880209Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:551:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneDisplayController[m[K' defined here
2025-11-06T14:24:33.9881045Z   551 | class [01;36m[KDroneDisplayController[m[K {
2025-11-06T14:24:33.9881613Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9883196Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1457:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_spectrum_pixel_from_bin(uint8_t)[m[K'
2025-11-06T14:24:33.9884407Z  1457 | void [01;31m[KDroneDisplayController[m[K::add_spectrum_pixel_from_bin(uint8_t power) {
2025-11-06T14:24:33.9884838Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9885941Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1457:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_spectrum_pixel_from_bin(uint8_t)[m[K'
2025-11-06T14:24:33.9887157Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9888351Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:551:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneDisplayController[m[K' defined here
2025-11-06T14:24:33.9889512Z   551 | class [01;36m[KDroneDisplayController[m[K {
2025-11-06T14:24:33.9890128Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9892000Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_mini_spectrum()[m[K':
2025-11-06T14:24:33.9894021Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1478:10:[m[K [01;31m[Kerror: [m[K'[01m[Kscoped_lock[m[K' is not a member of '[01m[Kstd[m[K'
2025-11-06T14:24:33.9895226Z  1478 |     std::[01;31m[Kscoped_lock[m[K<std::mutex> lock(spectrum_access_mutex_);  // Section 3: Thread safety for spectrum rendering
2025-11-06T14:24:33.9895728Z       |          [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:33.9896740Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:20:1:[m[K [01;36m[Knote: [m[K'[01m[Kstd::scoped_lock[m[K' is defined in header '[01m[K<mutex>[m[K'; did you forget to '[01m[K#include <mutex>[m[K'?
2025-11-06T14:24:33.9897511Z    19 | #include <cstdlib>
2025-11-06T14:24:33.9897757Z   +++ |+[32m[K#include <mutex>[m[K
2025-11-06T14:24:33.9897976Z    20 | #include <memory>
2025-11-06T14:24:33.9898708Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1478:27:[m[K [01;31m[Kerror: [m[K'[01m[Kmutex[m[K' is not a member of '[01m[Kstd[m[K'
2025-11-06T14:24:33.9899671Z  1478 |     std::scoped_lock<std::[01;31m[Kmutex[m[K> lock(spectrum_access_mutex_);  // Section 3: Thread safety for spectrum rendering
2025-11-06T14:24:33.9900172Z       |                           [01;31m[K^~~~~[m[K
2025-11-06T14:24:33.9901264Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1478:27:[m[K [01;36m[Knote: [m[K'[01m[Kstd::mutex[m[K' is defined in header '[01m[K<mutex>[m[K'; did you forget to '[01m[K#include <mutex>[m[K'?
2025-11-06T14:24:33.9905609Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1478:39:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_access_mutex_[m[K' was not declared in this scope
2025-11-06T14:24:33.9906666Z  1478 |     std::scoped_lock<std::mutex> lock([01;31m[Kspectrum_access_mutex_[m[K);  // Section 3: Thread safety for spectrum rendering
2025-11-06T14:24:33.9907211Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9913152Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1478:34:[m[K [01;31m[Kerror: [m[K'[01m[Klock[m[K' was not declared in this scope
2025-11-06T14:24:33.9914162Z  1478 |     std::scoped_lock<std::mutex> [01;31m[Klock[m[K(spectrum_access_mutex_);  // Section 3: Thread safety for spectrum rendering
2025-11-06T14:24:33.9914672Z       |                                  [01;31m[K^~~~[m[K
2025-11-06T14:24:33.9919318Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1487:9:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay[m[K' was not declared in this scope; did you mean '[01m[Kportapack::display[m[K'?
2025-11-06T14:24:33.9921064Z  1487 |         [01;31m[Kdisplay[m[K.draw_pixels(
2025-11-06T14:24:33.9921615Z       |         [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:33.9922204Z       |         [32m[Kportapack::display[m[K
2025-11-06T14:24:33.9923340Z In file included from [01m[K/havoc/firmware/application/./config_mode.hpp:27[m[K,
2025-11-06T14:24:33.9924415Z                  from [01m[K/havoc/firmware/common/portapack_persistent_memory.hpp:36[m[K,
2025-11-06T14:24:33.9925362Z                  from [01m[K/havoc/firmware/application/./rtc_time.hpp:29[m[K,
2025-11-06T14:24:33.9926180Z                  from [01m[K/havoc/firmware/common/ui_widget.hpp:32[m[K,
2025-11-06T14:24:33.9927030Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:33.9928257Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:33.9929719Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:33.9931180Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9933260Z [01m[K/havoc/firmware/application/./portapack.hpp:57:21:[m[K [01;36m[Knote: [m[K'[01m[Kportapack::display[m[K' declared here
2025-11-06T14:24:33.9934278Z    57 | extern lcd::ILI9341 [01;36m[Kdisplay[m[K;
2025-11-06T14:24:33.9934842Z       |                     [01;36m[K^~~~~~~[m[K
2025-11-06T14:24:33.9937975Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController::DroneUIController(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, ui::external_app::enhanced_drone_analyzer::AudioManager&)[m[K':
2025-11-06T14:24:33.9941872Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1558:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController[m[K' does not have any field named '[01m[Kaudio_[m[K'
2025-11-06T14:24:33.9943797Z  1558 |       [01;31m[Kaudio_[m[K(audio),
2025-11-06T14:24:33.9944277Z       |       [01;31m[K^~~~~~[m[K
2025-11-06T14:24:33.9946604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1551:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController::audio_mgr_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9948812Z  1551 | [01;35m[KDroneUIController[m[K::DroneUIController(NavigationView& nav,
2025-11-06T14:24:33.9949578Z       | [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9951695Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1551:1:[m[K [01;31m[Kerror: [m[Kuninitialized reference member in '[01m[Kclass ui::external_app::enhanced_drone_analyzer::AudioManager&[m[K' [[01;31m[K-fpermissive[m[K]
2025-11-06T14:24:33.9955108Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9957988Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:677:19:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::AudioManager& ui::external_app::enhanced_drone_analyzer::DroneUIController::audio_mgr_[m[K' should be initialized
2025-11-06T14:24:33.9960013Z   677 |     AudioManager& [01;36m[Kaudio_mgr_[m[K;
2025-11-06T14:24:33.9960638Z       |                   [01;36m[K^~~~~~~~~~[m[K
2025-11-06T14:24:33.9963276Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1551:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController::settings_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:33.9965547Z  1551 | [01;35m[KDroneUIController[m[K::DroneUIController(NavigationView& nav,
2025-11-06T14:24:33.9966223Z       | [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9967916Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_stop_scan()[m[K':
2025-11-06T14:24:33.9970308Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1575:5:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-06T14:24:33.9971630Z  1575 |     [01;31m[Kaudio_[m[K.stop_audio();
2025-11-06T14:24:33.9972088Z       |     [01;31m[K^~~~~~[m[K
2025-11-06T14:24:33.9972474Z       |     [32m[Kaudio[m[K
2025-11-06T14:24:33.9974034Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_toggle_mode()[m[K':
2025-11-06T14:24:33.9977582Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1583:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-06T14:24:33.9979883Z  1583 |         if (hardware_.[01;31m[Kis_spectrum_streaming_active[m[K()) {
2025-11-06T14:24:33.9980464Z       |                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9981385Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:33.9982910Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:454:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-06T14:24:33.9984200Z   454 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-06T14:24:33.9984756Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9987834Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1588:24:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-06T14:24:33.9990498Z  1588 |         if (!hardware_.[01;31m[Kis_spectrum_streaming_active[m[K()) {
2025-11-06T14:24:33.9991148Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:33.9992252Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0006894Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:454:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-06T14:24:34.0008271Z   454 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-06T14:24:34.0008968Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0010872Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_menu()[m[K':
2025-11-06T14:24:34.0014126Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1612:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-06T14:24:34.0015785Z  1612 |     }[01;31m[K)[m[K;
2025-11-06T14:24:34.0016159Z       |      [01;31m[K^[m[K
2025-11-06T14:24:34.0016991Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0018323Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0019699Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0021521Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-06T14:24:34.0022635Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0023298Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0024232Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-06T14:24:34.0026715Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_select_language()[m[K':
2025-11-06T14:24:34.0029395Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1619:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-06T14:24:34.0030891Z  1619 |     }[01;31m[K)[m[K;
2025-11-06T14:24:34.0031282Z       |      [01;31m[K^[m[K
2025-11-06T14:24:34.0032245Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0033860Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0035200Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0037181Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-06T14:24:34.0038377Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0038847Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0039810Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-06T14:24:34.0041973Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_audio_toggle()[m[K':
2025-11-06T14:24:34.0044587Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1643:26:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-06T14:24:34.0046052Z  1643 |         .audio_enabled = [01;31m[Kaudio_[m[K.is_audio_enabled(),
2025-11-06T14:24:34.0046662Z       |                          [01;31m[K^~~~~~[m[K
2025-11-06T14:24:34.0047137Z       |                          [32m[Kaudio[m[K
2025-11-06T14:24:34.0058672Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1646:15:[m[K [01;31m[Kerror: [m[K'[01m[KDroneAudioSettingsView[m[K' was not declared in this scope; did you mean '[01m[KDroneAudioSettings[m[K'?
2025-11-06T14:24:34.0060483Z  1646 |     nav_.push<[01;31m[KDroneAudioSettingsView[m[K>(audio_settings, audio_);
2025-11-06T14:24:34.0061174Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0061685Z       |               [32m[KDroneAudioSettings[m[K
2025-11-06T14:24:34.0085822Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-06T14:24:34.0087191Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1651:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_spectrum_settings[m[K' was not declared in this scope; did you mean '[01m[Kon_open_settings[m[K'?
2025-11-06T14:24:34.0088967Z  1651 |         {"Spectrum Mode", [this]() { [01;31m[Kon_spectrum_settings[m[K(); }},  // PHASE 3: Now connects restore set_spectrum_mode()
2025-11-06T14:24:34.0089927Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0090291Z       |                                      [32m[Kon_open_settings[m[K
2025-11-06T14:24:34.0091446Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_advanced_settings()[m[K':
2025-11-06T14:24:34.0093650Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1656:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-06T14:24:34.0094589Z  1656 |     }[01;31m[K)[m[K;
2025-11-06T14:24:34.0094832Z       |      [01;31m[K^[m[K
2025-11-06T14:24:34.0095425Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0096285Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0097471Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0099411Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-06T14:24:34.0100417Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0100901Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0101861Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-06T14:24:34.0103365Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_system_status()[m[K':
2025-11-06T14:24:34.0105483Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1666:32:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-06T14:24:34.0106987Z  1666 |     const char* audio_status = [01;31m[Kaudio_[m[K.is_audio_enabled() ? "ENABLED" : "DISABLED";
2025-11-06T14:24:34.0107438Z       |                                [01;31m[K^~~~~~[m[K
2025-11-06T14:24:34.0107735Z       |                                [32m[Kaudio[m[K
2025-11-06T14:24:34.0110075Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1668:45:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-06T14:24:34.0112718Z  1668 |     const char* spectrum_status = hardware_.[01;31m[Kis_spectrum_streaming_active[m[K() ? "YES" : "NO";
2025-11-06T14:24:34.0113537Z       |                                             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0114148Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0115093Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:454:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-06T14:24:34.0115774Z   454 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-06T14:24:34.0116118Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0117138Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_performance_stats()[m[K':
2025-11-06T14:24:34.0118816Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1703:33:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:34.0120005Z  1703 |             "Cycles completed: [01;35m[K%u[m[K\n"
2025-11-06T14:24:34.0120326Z       |                                [01;35m[K~^[m[K
2025-11-06T14:24:34.0120615Z       |                                 [01;35m[K|[m[K
2025-11-06T14:24:34.0120914Z       |                                 [01;35m[Kunsigned int[m[K
2025-11-06T14:24:34.0121215Z       |                                [32m[K%lu[m[K
2025-11-06T14:24:34.0121432Z ......
2025-11-06T14:24:34.0121645Z  1711 |             [32m[Kscan_cycles[m[K,
2025-11-06T14:24:34.0121931Z       |             [32m[K~~~~~~~~~~~[m[K          
2025-11-06T14:24:34.0122193Z       |             [32m[K|[m[K
2025-11-06T14:24:34.0122478Z       |             [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:34.0123768Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1705:33:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 6 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:34.0124801Z  1705 |             "Total detections: [01;35m[K%u[m[K\n"
2025-11-06T14:24:34.0125244Z       |                                [01;35m[K~^[m[K
2025-11-06T14:24:34.0125535Z       |                                 [01;35m[K|[m[K
2025-11-06T14:24:34.0125829Z       |                                 [01;35m[Kunsigned int[m[K
2025-11-06T14:24:34.0126125Z       |                                [32m[K%lu[m[K
2025-11-06T14:24:34.0126339Z ......
2025-11-06T14:24:34.0126585Z  1713 |             [32m[Kscanner_.get_total_detections()[m[K,
2025-11-06T14:24:34.0126909Z       |             [32m[K~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0127204Z       |                                          [32m[K|[m[K
2025-11-06T14:24:34.0127547Z       |                                          [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:34.0128526Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_debug_info()[m[K':
2025-11-06T14:24:34.0130699Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1743:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-06T14:24:34.0132217Z  1743 |             hardware_.[01;31m[Kis_spectrum_streaming_active[m[K() ? "YES" : "NO",
2025-11-06T14:24:34.0132618Z       |                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0133376Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0134305Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:454:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-06T14:24:34.0134974Z   454 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-06T14:24:34.0135311Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0136404Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1744:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kget_current_rssi[m[K'
2025-11-06T14:24:34.0137358Z  1744 |             hardware_.[01;31m[Kget_current_rssi[m[K(),
2025-11-06T14:24:34.0137683Z       |                       [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0139470Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1747:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-06T14:24:34.0141001Z  1747 |             hardware_.[01;31m[Kis_spectrum_streaming_active[m[K() ? "STREAMING" : "IDLE");
2025-11-06T14:24:34.0141425Z       |                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0142005Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0143922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:454:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-06T14:24:34.0145056Z   454 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-06T14:24:34.0145434Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0146634Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_manage_frequencies()[m[K':
2025-11-06T14:24:34.0148029Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1752:15:[m[K [01;31m[Kerror: [m[K'[01m[KDroneFrequencyManagerView[m[K' was not declared in this scope
2025-11-06T14:24:34.0148855Z  1752 |     nav_.push<[01;31m[KDroneFrequencyManagerView[m[K>();
2025-11-06T14:24:34.0149222Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0150199Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1752:42:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<<expression error> >()[m[K'
2025-11-06T14:24:34.0151074Z  1752 |     nav_.push<DroneFrequencyManagerView>([01;31m[K)[m[K;
2025-11-06T14:24:34.0151584Z       |                                          [01;31m[K^[m[K
2025-11-06T14:24:34.0152729Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0154229Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0155371Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0156649Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class T, class ... Args> T* ui::NavigationView::push(Args&& ...)[m[K'
2025-11-06T14:24:34.0157847Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0158342Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0159020Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0159975Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1752:42:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-06T14:24:34.0160678Z  1752 |     nav_.push<DroneFrequencyManagerView>([01;31m[K)[m[K;
2025-11-06T14:24:34.0161041Z       |                                          [01;31m[K^[m[K
2025-11-06T14:24:34.0162061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_hardware_control_menu()[m[K':
2025-11-06T14:24:34.0163793Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1791:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-06T14:24:34.0164872Z  1791 |     }[01;31m[K)[m[K;
2025-11-06T14:24:34.0165102Z       |      [01;31m[K^[m[K
2025-11-06T14:24:34.0165658Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0166450Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0167219Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0169199Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-06T14:24:34.0170369Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0170670Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0171232Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-06T14:24:34.0172866Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_save_settings()[m[K':
2025-11-06T14:24:34.0175340Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1795:53:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings[m[K' to '[01m[Kconst DroneAnalyzerSettings&[m[K'
2025-11-06T14:24:34.0176452Z  1795 |     if (DroneAnalyzerSettingsManager::save_settings([01;31m[Ksettings_[m[K)) {
2025-11-06T14:24:34.0176914Z       |                                                     [01;31m[K^~~~~~~~~[m[K
2025-11-06T14:24:34.0177237Z       |                                                     [01;31m[K|[m[K
2025-11-06T14:24:34.0177735Z       |                                                     [01;31m[Kui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings[m[K
2025-11-06T14:24:34.0178441Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18[m[K,
2025-11-06T14:24:34.0179228Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0180548Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:108:86:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kstatic bool DroneAnalyzerSettingsManager::save_settings(const DroneAnalyzerSettings&)[m[K'
2025-11-06T14:24:34.0181725Z   108 | inline bool DroneAnalyzerSettingsManager::save_settings([01;36m[Kconst DroneAnalyzerSettings& settings[m[K) {
2025-11-06T14:24:34.0182300Z       |                                                         [01;36m[K~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.0183433Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_load_settings()[m[K':
2025-11-06T14:24:34.0185054Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1803:53:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings[m[K' to '[01m[KDroneAnalyzerSettings&[m[K'
2025-11-06T14:24:34.0186124Z  1803 |     if (DroneAnalyzerSettingsManager::load_settings([01;31m[Ksettings_[m[K)) {
2025-11-06T14:24:34.0186574Z       |                                                     [01;31m[K^~~~~~~~~[m[K
2025-11-06T14:24:34.0187064Z       |                                                     [01;31m[K|[m[K
2025-11-06T14:24:34.0187559Z       |                                                     [01;31m[Kui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings[m[K
2025-11-06T14:24:34.0188267Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18[m[K,
2025-11-06T14:24:34.0189051Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0190336Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:101:80:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kstatic bool DroneAnalyzerSettingsManager::load_settings(DroneAnalyzerSettings&)[m[K'
2025-11-06T14:24:34.0191527Z   101 | inline bool DroneAnalyzerSettingsManager::load_settings([01;36m[KDroneAnalyzerSettings& settings[m[K) {
2025-11-06T14:24:34.0192083Z       |                                                         [01;36m[K~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.0193319Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_set_bandwidth_config()[m[K':
2025-11-06T14:24:34.0194922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1842:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-06T14:24:34.0195756Z  1842 |     }[01;31m[K)[m[K;
2025-11-06T14:24:34.0195983Z       |      [01;31m[K^[m[K
2025-11-06T14:24:34.0196530Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0197322Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0198070Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0198699Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-06T14:24:34.0198848Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0198952Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0199355Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-06T14:24:34.0200177Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_set_center_freq_config()[m[K':
2025-11-06T14:24:34.0201039Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-06T14:24:34.0201143Z  1851 |     }[01;31m[K)[m[K;
2025-11-06T14:24:34.0201244Z       |      [01;31m[K^[m[K
2025-11-06T14:24:34.0201658Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0202024Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0202420Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0203120Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-06T14:24:34.0203393Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0203495Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0203900Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-06T14:24:34.0211209Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_about()[m[K':
2025-11-06T14:24:34.0211884Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1876:15:[m[K [01;31m[Kerror: [m[K'[01m[KAuthorContactView[m[K' was not declared in this scope
2025-11-06T14:24:34.0212065Z  1876 |     nav_.push<[01;31m[KAuthorContactView[m[K>();
2025-11-06T14:24:34.0212193Z       |               [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0213149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1876:34:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<<expression error> >()[m[K'
2025-11-06T14:24:34.0213491Z  1876 |     nav_.push<AuthorContactView>([01;31m[K)[m[K;
2025-11-06T14:24:34.0213633Z       |                                  [01;31m[K^[m[K
2025-11-06T14:24:34.0214072Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0214452Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0214816Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0215457Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class T, class ... Args> T* ui::NavigationView::push(Args&& ...)[m[K'
2025-11-06T14:24:34.0215600Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0215705Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0216139Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0216669Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1876:34:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-06T14:24:34.0216834Z  1876 |     nav_.push<AuthorContactView>([01;31m[K)[m[K;
2025-11-06T14:24:34.0216966Z       |                                  [01;31m[K^[m[K
2025-11-06T14:24:34.0217368Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:34.0218264Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1879:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_spectrum_settings()[m[K'
2025-11-06T14:24:34.0218515Z  1879 | void [01;31m[KDroneUIController[m[K::on_spectrum_settings() {
2025-11-06T14:24:34.0218635Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0219500Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1879:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_spectrum_settings()[m[K'
2025-11-06T14:24:34.0219914Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0220663Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:644:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneUIController[m[K' defined here
2025-11-06T14:24:34.0220922Z   644 | class [01;36m[KDroneUIController[m[K {
2025-11-06T14:24:34.0221044Z       |       [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0242034Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_open_constant_settings()[m[K':
2025-11-06T14:24:34.0242859Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1904:12:[m[K [01;31m[Kerror: [m[K'[01m[KConstantSettingsManager[m[K' does not name a type
2025-11-06T14:24:34.0243283Z  1904 |     static [01;31m[KConstantSettingsManager[m[K manager;
2025-11-06T14:24:34.0243451Z       |            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0272781Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1905:15:[m[K [01;31m[Kerror: [m[K'[01m[KConstantSettingsView[m[K' was not declared in this scope
2025-11-06T14:24:34.0273470Z  1905 |     nav_.push<[01;31m[KConstantSettingsView[m[K>(nav_);
2025-11-06T14:24:34.0273763Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0276087Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1905:41:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<<expression error> >(ui::NavigationView&)[m[K'
2025-11-06T14:24:34.0276374Z  1905 |     nav_.push<ConstantSettingsView>(nav_[01;31m[K)[m[K;
2025-11-06T14:24:34.0276546Z       |                                         [01;31m[K^[m[K
2025-11-06T14:24:34.0277100Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0277576Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0278043Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0278860Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class T, class ... Args> T* ui::NavigationView::push(Args&& ...)[m[K'
2025-11-06T14:24:34.0279049Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-06T14:24:34.0279158Z       |        [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0279590Z [01m[K/havoc/firmware/application/./ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0280138Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1905:41:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-06T14:24:34.0280333Z  1905 |     nav_.push<ConstantSettingsView>(nav_[01;31m[K)[m[K;
2025-11-06T14:24:34.0280473Z       |                                         [01;31m[K^[m[K
2025-11-06T14:24:34.0281283Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_add_preset_quick()[m[K':
2025-11-06T14:24:34.0281891Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1910:24:[m[K [01;31m[Kerror: [m[K'[01m[KDroneFrequencyPresets[m[K' has not been declared
2025-11-06T14:24:34.0282174Z  1910 |     auto all_presets = [01;31m[KDroneFrequencyPresets[m[K::get_all_presets();
2025-11-06T14:24:34.0282321Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0283134Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1919:5:[m[K [01;31m[Kerror: [m[K'[01m[KDronePresetSelector[m[K' has not been declared
2025-11-06T14:24:34.0283575Z  1919 |     [01;31m[KDronePresetSelector[m[K::show_preset_menu(nav_,
2025-11-06T14:24:34.0283701Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0288726Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::add_preset_to_scanner(const DronePreset&)[m[K':
2025-11-06T14:24:34.0290222Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1946:33:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KFreqmanDB::FreqmanDB(const char [7])[m[K'
2025-11-06T14:24:34.0290712Z  1946 |     FreqmanDB preset_db("DRONES"[01;31m[K)[m[K; // Use same file as scanner
2025-11-06T14:24:34.0290937Z       |                                 [01;31m[K^[m[K
2025-11-06T14:24:34.0291659Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:34[m[K,
2025-11-06T14:24:34.0292333Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0293624Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr FreqmanDB::FreqmanDB()[m[K'
2025-11-06T14:24:34.0293794Z   200 | class [01;36m[KFreqmanDB[m[K {
2025-11-06T14:24:34.0293902Z       |       [01;36m[K^~~~~~~~~[m[K
2025-11-06T14:24:34.0294444Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-06T14:24:34.0295091Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KFreqmanDB::FreqmanDB(FreqmanDB&&)[m[K'
2025-11-06T14:24:34.0295784Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[K  no known conversion for argument 1 from '[01m[Kconst char [7][m[K' to '[01m[KFreqmanDB&&[m[K'
2025-11-06T14:24:34.0296495Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1947:24:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KFreqmanDB::open()[m[K'
2025-11-06T14:24:34.0296647Z  1947 |     if (preset_db.open([01;31m[K)[m[K) {
2025-11-06T14:24:34.0296767Z       |                        [01;31m[K^[m[K
2025-11-06T14:24:34.0297188Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:34[m[K,
2025-11-06T14:24:34.0297566Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0298282Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:241:10:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kbool FreqmanDB::open(const std::filesystem::path&, bool)[m[K'
2025-11-06T14:24:34.0298568Z   241 |     bool [01;36m[Kopen[m[K(const std::filesystem::path& path, bool create = false);
2025-11-06T14:24:34.0298680Z       |          [01;36m[K^~~~[m[K
2025-11-06T14:24:34.0299210Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:241:10:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-06T14:24:34.0299881Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1957:9:[m[K [01;31m[Kerror: [m[K'[01m[Kfreqman_entry[m[K' has no non-static data member named '[01m[Ktonal[m[K'
2025-11-06T14:24:34.0299990Z  1957 |         [01;31m[K}[m[K;
2025-11-06T14:24:34.0300089Z       |         [01;31m[K^[m[K
2025-11-06T14:24:34.0300715Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1959:19:[m[K [01;31m[Kerror: [m[K'[01m[Kclass FreqmanDB[m[K' has no member named '[01m[Ksave[m[K'
2025-11-06T14:24:34.0300987Z  1959 |         preset_db.[01;31m[Ksave[m[K();
2025-11-06T14:24:34.0301118Z       |                   [01;31m[K^~~~[m[K
2025-11-06T14:24:34.0304353Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_toggle_audio_simple()[m[K':
2025-11-06T14:24:34.0305592Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1972:5:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-06T14:24:34.0305845Z  1972 |     [01;31m[Kaudio_[m[K.toggle_audio();
2025-11-06T14:24:34.0306018Z       |     [01;31m[K^~~~~~[m[K
2025-11-06T14:24:34.0306198Z       |     [32m[Kaudio[m[K
2025-11-06T14:24:34.0327762Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)[m[K':
2025-11-06T14:24:34.0330277Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::button_start_stop_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:34.0331187Z  1981 | [01;35m[KEnhancedDroneSpectrumAnalyzerView[m[K::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
2025-11-06T14:24:34.0331502Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0333668Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::button_menu_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:34.0335948Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::field_scanning_mode_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:34.0337333Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1988:125:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::OptionsField::OptionsField()[m[K'
2025-11-06T14:24:34.0338192Z  1988 |       scanning_coordinator_(std::make_unique<ScanningCoordinator>(nav, *hardware_, *scanner_, *display_controller_, *audio_)[01;31m[K)[m[K
2025-11-06T14:24:34.0338563Z       |                                                                                                                             [01;31m[K^[m[K
2025-11-06T14:24:34.0339054Z In file included from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:34.0339745Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0340418Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0341100Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0342229Z [01m[K/havoc/firmware/common/ui_widget.hpp:689:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kui::OptionsField::OptionsField(ui::Point, size_t, ui::OptionsField::options_t, bool)[m[K'
2025-11-06T14:24:34.0342811Z   689 |     [01;36m[KOptionsField[m[K(Point parent_pos, size_t length, options_t options, bool centered = false);
2025-11-06T14:24:34.0343379Z       |     [01;36m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0344033Z [01m[K/havoc/firmware/common/ui_widget.hpp:689:5:[m[K [01;36m[Knote: [m[K  candidate expects 4 arguments, 0 provided
2025-11-06T14:24:34.0346103Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::smart_header_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:34.0346866Z  1981 | [01;35m[KEnhancedDroneSpectrumAnalyzerView[m[K::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
2025-11-06T14:24:34.0347096Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0349142Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::status_bar_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:34.0351373Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::threat_cards_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-06T14:24:34.0353591Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1992:37:[m[K [01;31m[Kerror: [m[Kinvalid initialization of reference of type '[01m[KDroneAnalyzerSettings&[m[K' from expression of type '[01m[Kui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings[m[K'
2025-11-06T14:24:34.0354007Z  1992 |     if (!load_settings_from_sd_card([01;31m[Kloaded_settings[m[K)) {
2025-11-06T14:24:34.0354251Z       |                                     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0355507Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:57:56:[m[K [01;36m[Knote: [m[Kin passing argument 1 of '[01m[Kbool load_settings_from_sd_card(DroneAnalyzerSettings&)[m[K'
2025-11-06T14:24:34.0355965Z    57 | bool load_settings_from_sd_card([01;36m[KDroneAnalyzerSettings& settings[m[K) {
2025-11-06T14:24:34.0356233Z       |                                 [01;36m[K~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.0365210Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2004:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope; did you mean '[01m[Kbutton_start_stop_[m[K'?
2025-11-06T14:24:34.0365618Z  2004 |     [01;31m[Kbutton_start_[m[K.on_select = [this](Button&) {
2025-11-06T14:24:34.0365835Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0366055Z       |     [32m[Kbutton_start_stop_[m[K
2025-11-06T14:24:34.0382085Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2018:5:[m[K [01;31m[Kerror: [m[Kno match for '[01m[Koperator=[m[K' (operand types are '[01m[Kstd::function<void(unsigned int, long int)>[m[K' and '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>[m[K')
2025-11-06T14:24:34.0382344Z  2018 |     [01;31m[K}[m[K;
2025-11-06T14:24:34.0382539Z       |     [01;31m[K^[m[K
2025-11-06T14:24:34.0386534Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/functional:59[m[K,
2025-11-06T14:24:34.0387180Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/pstl/glue_algorithm_defs.h:13[m[K,
2025-11-06T14:24:34.0387671Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:71[m[K,
2025-11-06T14:24:34.0388577Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6[m[K,
2025-11-06T14:24:34.0389261Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.0389976Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0391747Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:462:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::function<_Res(_ArgTypes ...)>& std::function<_Res(_ArgTypes ...)>::operator=(const std::function<_Res(_ArgTypes ...)>&) [with _Res = void; _ArgTypes = {unsigned int, long int}][m[K'
2025-11-06T14:24:34.0392003Z   462 |       [01;36m[Koperator[m[K=(const function& __x)
2025-11-06T14:24:34.0392168Z       |       [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:34.0394574Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:462:33:[m[K [01;36m[Knote: [m[K  no known conversion for argument 1 from '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>[m[K' to '[01m[Kconst std::function<void(unsigned int, long int)>&[m[K'
2025-11-06T14:24:34.0395078Z   462 |       operator=([01;36m[Kconst function& __x[m[K)
2025-11-06T14:24:34.0395315Z       |                 [01;36m[K~~~~~~~~~~~~~~~~^~~[m[K
2025-11-06T14:24:34.0397092Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:480:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::function<_Res(_ArgTypes ...)>& std::function<_Res(_ArgTypes ...)>::operator=(std::function<_Res(_ArgTypes ...)>&&) [with _Res = void; _ArgTypes = {unsigned int, long int}][m[K'
2025-11-06T14:24:34.0397374Z   480 |       [01;36m[Koperator[m[K=(function&& __x) noexcept
2025-11-06T14:24:34.0397562Z       |       [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:34.0400010Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:480:28:[m[K [01;36m[Knote: [m[K  no known conversion for argument 1 from '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>[m[K' to '[01m[Kstd::function<void(unsigned int, long int)>&&[m[K'
2025-11-06T14:24:34.0400301Z   480 |       operator=([01;36m[Kfunction&& __x[m[K) noexcept
2025-11-06T14:24:34.0400516Z       |                 [01;36m[K~~~~~~~~~~~^~~[m[K
2025-11-06T14:24:34.0402369Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:494:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::function<_Res(_ArgTypes ...)>& std::function<_Res(_ArgTypes ...)>::operator=(std::nullptr_t) [with _Res = void; _ArgTypes = {unsigned int, long int}; std::nullptr_t = std::nullptr_t][m[K'
2025-11-06T14:24:34.0402631Z   494 |       [01;36m[Koperator[m[K=(nullptr_t) noexcept
2025-11-06T14:24:34.0402822Z       |       [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:34.0405242Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:494:17:[m[K [01;36m[Knote: [m[K  no known conversion for argument 1 from '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>[m[K' to '[01m[Kstd::nullptr_t[m[K'
2025-11-06T14:24:34.0405585Z   494 |       operator=([01;36m[Knullptr_t[m[K) noexcept
2025-11-06T14:24:34.0405791Z       |                 [01;36m[K^~~~~~~~~[m[K
2025-11-06T14:24:34.0408434Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:523:2:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Functor> std::function<_Res(_ArgTypes ...)>::_Requires<std::function<_Res(_ArgTypes ...)>::_Callable<typename std::decay<_Functor>::type>, std::function<_Res(_ArgTypes ...)>&> std::function<_Res(_ArgTypes ...)>::operator=(_Functor&&) [with _Functor = _Functor; _Res = void; _ArgTypes = {unsigned int, long int}][m[K'
2025-11-06T14:24:34.0408847Z   523 |  [01;36m[Koperator[m[K=(_Functor&& __f)
2025-11-06T14:24:34.0409018Z       |  [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:34.0409914Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:523:2:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0414859Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h: In substitution of '[01m[Ktemplate<class _Res, class ... _ArgTypes> template<class _Cond, class _Tp> using _Requires = typename std::enable_if<_Cond::value, _Tp>::type [with _Cond = std::function<void(unsigned int, long int)>::_Callable<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>, std::__invoke_result<ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>&, unsigned int, long int> >; _Tp = std::function<void(unsigned int, long int)>&; _Res = void; _ArgTypes = {unsigned int, long int}][m[K':
2025-11-06T14:24:34.0418893Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:523:2:[m[K   required by substitution of '[01m[Ktemplate<class _Functor> std::function<void(unsigned int, long int)>::_Requires<std::function<void(unsigned int, long int)>::_Callable<typename std::decay<_Tp>::type, std::__invoke_result<typename std::decay<_Tp>::type&, unsigned int, long int> >, std::function<void(unsigned int, long int)>&> std::function<void(unsigned int, long int)>::operator=<_Functor>(_Functor&&) [with _Functor = ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>][m[K'
2025-11-06T14:24:34.0419679Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2018:5:[m[K   required from here
2025-11-06T14:24:34.0421014Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:385:8:[m[K [01;31m[Kerror: [m[Kno type named '[01m[Ktype[m[K' in '[01m[Kstruct std::enable_if<false, std::function<void(unsigned int, long int)>&>[m[K'
2025-11-06T14:24:34.0421483Z   385 |  using [01;31m[K_Requires[m[K = typename enable_if<_Cond::value, _Tp>::type;
2025-11-06T14:24:34.0421662Z       |        [01;31m[K^~~~~~~~~[m[K
2025-11-06T14:24:34.0423814Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:532:2:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Functor> std::function<_Res(_ArgTypes ...)>& std::function<_Res(_ArgTypes ...)>::operator=(std::reference_wrapper<_Functor>) [with _Functor = _Functor; _Res = void; _ArgTypes = {unsigned int, long int}][m[K'
2025-11-06T14:24:34.0424215Z   532 |  [01;36m[Koperator[m[K=(reference_wrapper<_Functor> __f) noexcept
2025-11-06T14:24:34.0424427Z       |  [01;36m[K^~~~~~~~[m[K
2025-11-06T14:24:34.0425398Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/std_function.h:532:2:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0427873Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2018:5:[m[K [01;36m[Knote: [m[K  '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)::<lambda(size_t)>[m[K' is not derived from '[01m[Kstd::reference_wrapper<_Tp>[m[K'
2025-11-06T14:24:34.0428078Z  2018 |     [01;36m[K}[m[K;
2025-11-06T14:24:34.0428219Z       |     [01;36m[K^[m[K
2025-11-06T14:24:34.0429556Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2021:26:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::OptionsField[m[K' has no member named '[01m[Kset_value[m[K'; did you mean '[01m[Kset_by_value[m[K'?
2025-11-06T14:24:34.0430106Z  2021 |     field_scanning_mode_.[01;31m[Kset_value[m[K(initial_mode);
2025-11-06T14:24:34.0430348Z       |                          [01;31m[K^~~~~~~~~[m[K
2025-11-06T14:24:34.0430597Z       |                          [32m[Kset_by_value[m[K
2025-11-06T14:24:34.0432290Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::focus()[m[K':
2025-11-06T14:24:34.0433930Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2039:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope; did you mean '[01m[Kbutton_start_stop_[m[K'?
2025-11-06T14:24:34.0434233Z  2039 |     [01;31m[Kbutton_start_[m[K.focus();
2025-11-06T14:24:34.0434414Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0434617Z       |     [32m[Kbutton_start_stop_[m[K
2025-11-06T14:24:34.0435969Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_show()[m[K':
2025-11-06T14:24:34.0437695Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2064:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay[m[K' was not declared in this scope; did you mean '[01m[Kportapack::display[m[K'?
2025-11-06T14:24:34.0438091Z  2064 |     [01;31m[Kdisplay[m[K.scroll_set_area(109, screen_height - 1);
2025-11-06T14:24:34.0438274Z       |     [01;31m[K^~~~~~~[m[K
2025-11-06T14:24:34.0438530Z       |     [32m[Kportapack::display[m[K
2025-11-06T14:24:34.0438993Z In file included from [01m[K/havoc/firmware/application/./config_mode.hpp:27[m[K,
2025-11-06T14:24:34.0439456Z                  from [01m[K/havoc/firmware/common/portapack_persistent_memory.hpp:36[m[K,
2025-11-06T14:24:34.0439820Z                  from [01m[K/havoc/firmware/application/./rtc_time.hpp:29[m[K,
2025-11-06T14:24:34.0440143Z                  from [01m[K/havoc/firmware/common/ui_widget.hpp:32[m[K,
2025-11-06T14:24:34.0440547Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:35[m[K,
2025-11-06T14:24:34.0441236Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-06T14:24:34.0441893Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0442560Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0443584Z [01m[K/havoc/firmware/application/./portapack.hpp:57:21:[m[K [01;36m[Knote: [m[K'[01m[Kportapack::display[m[K' declared here
2025-11-06T14:24:34.0443858Z    57 | extern lcd::ILI9341 [01;36m[Kdisplay[m[K;
2025-11-06T14:24:34.0444071Z       |                     [01;36m[K^~~~~~~[m[K
2025-11-06T14:24:34.0460808Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button()[m[K':
2025-11-06T14:24:34.0462198Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2087:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope; did you mean '[01m[Kbutton_start_stop_[m[K'?
2025-11-06T14:24:34.0462520Z  2087 |         [01;31m[Kbutton_start_[m[K.set_text("START/STOP");
2025-11-06T14:24:34.0462714Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0462925Z       |         [32m[Kbutton_start_stop_[m[K
2025-11-06T14:24:34.0481388Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2090:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope; did you mean '[01m[Kbutton_start_stop_[m[K'?
2025-11-06T14:24:34.0481735Z  2090 |         [01;31m[Kbutton_start_[m[K.set_text("STOP");
2025-11-06T14:24:34.0481949Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0482168Z       |         [32m[Kbutton_start_stop_[m[K
2025-11-06T14:24:34.0492490Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout()[m[K':
2025-11-06T14:24:34.0494491Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2109:68:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[Kcard_y_pos[m[K' from '[01m[Ksize_t[m[K' {aka '[01m[Kunsigned int[m[K'} to '[01m[Kint[m[K' [[01;35m[K-Wnarrowing[m[K]
2025-11-06T14:24:34.0495225Z  2109 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, [01;35m[Kcard_y_pos[m[K, screen_width, 24});
2025-11-06T14:24:34.0495568Z       |                                                                    [01;35m[K^~~~~~~~~~[m[K
2025-11-06T14:24:34.0501166Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()[m[K':
2025-11-06T14:24:34.0502490Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2145:58:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmin(long unsigned int, unsigned int)[m[K'
2025-11-06T14:24:34.0503315Z  2145 |             uint32_t progress = std::min(cycles * 5, 100u[01;31m[K)[m[K; // Better progress calculation
2025-11-06T14:24:34.0503664Z       |                                                          [01;31m[K^[m[K
2025-11-06T14:24:34.0504335Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39[m[K,
2025-11-06T14:24:34.0504842Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40[m[K,
2025-11-06T14:24:34.0505627Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.0506375Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0507790Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)[m[K'
2025-11-06T14:24:34.0508153Z   198 |     [01;36m[Kmin[m[K(const _Tp& __a, const _Tp& __b)
2025-11-06T14:24:34.0508371Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.0509330Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0510961Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2145:58:[m[K [01;36m[Knote: [m[K  deduced conflicting types for parameter '[01m[Kconst _Tp[m[K' ('[01m[Klong unsigned int[m[K' and '[01m[Kunsigned int[m[K')
2025-11-06T14:24:34.0511602Z  2145 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K; // Better progress calculation
2025-11-06T14:24:34.0511917Z       |                                                          [01;36m[K^[m[K
2025-11-06T14:24:34.0512589Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39[m[K,
2025-11-06T14:24:34.0513260Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40[m[K,
2025-11-06T14:24:34.0514233Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.0514950Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0516239Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)[m[K'
2025-11-06T14:24:34.0516559Z   246 |     [01;36m[Kmin[m[K(const _Tp& __a, const _Tp& __b, _Compare __comp)
2025-11-06T14:24:34.0516721Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.0517541Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0519040Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2145:58:[m[K [01;36m[Knote: [m[K  deduced conflicting types for parameter '[01m[Kconst _Tp[m[K' ('[01m[Klong unsigned int[m[K' and '[01m[Kunsigned int[m[K')
2025-11-06T14:24:34.0519788Z  2145 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K; // Better progress calculation
2025-11-06T14:24:34.0520058Z       |                                                          [01;36m[K^[m[K
2025-11-06T14:24:34.0520608Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62[m[K,
2025-11-06T14:24:34.0521262Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6[m[K,
2025-11-06T14:24:34.0521932Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.0522596Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0524070Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)[m[K'
2025-11-06T14:24:34.0524358Z  3444 |     [01;36m[Kmin[m[K(initializer_list<_Tp> __l)
2025-11-06T14:24:34.0524535Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.0525410Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0526862Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2145:58:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Klong unsigned int[m[K'
2025-11-06T14:24:34.0527391Z  2145 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K; // Better progress calculation
2025-11-06T14:24:34.0527660Z       |                                                          [01;36m[K^[m[K
2025-11-06T14:24:34.0528193Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62[m[K,
2025-11-06T14:24:34.0528839Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6[m[K,
2025-11-06T14:24:34.0529512Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.0530177Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0531566Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)[m[K'
2025-11-06T14:24:34.0532086Z  3450 |     [01;36m[Kmin[m[K(initializer_list<_Tp> __l, _Compare __comp)
2025-11-06T14:24:34.0532252Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.0533247Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.0534681Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2145:58:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Klong unsigned int[m[K'
2025-11-06T14:24:34.0535212Z  2145 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K; // Better progress calculation
2025-11-06T14:24:34.0535479Z       |                                                          [01;36m[K^[m[K
2025-11-06T14:24:34.0536785Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2155:99:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::BigFrequency[m[K' has no member named '[01m[Ktext[m[K'
2025-11-06T14:24:34.0537514Z  2155 |             const char* primary_msg = (!display_controller_ || display_controller_->big_display().[01;31m[Ktext[m[K().empty()) ?
2025-11-06T14:24:34.0538005Z       |                                                                                                   [01;31m[K^~~~[m[K
2025-11-06T14:24:34.0539316Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2156:87:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::BigFrequency[m[K' has no member named '[01m[Ktext[m[K'
2025-11-06T14:24:34.0539825Z  2156 |                                      "EDA Ready" : display_controller_->big_display().[01;31m[Ktext[m[K().c_str();
2025-11-06T14:24:34.0540132Z       |                                                                                       [01;31m[K^~~~[m[K
2025-11-06T14:24:34.0541904Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2159:90:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-06T14:24:34.0542675Z  2159 |                 snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: [01;35m[K%u[m[K", [32m[Ktotal_detections[m[K);
2025-11-06T14:24:34.0543193Z       |                                                                                         [01;35m[K~^[m[K   [32m[K~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0543552Z       |                                                                                          [01;35m[K|[m[K   [32m[K|[m[K
2025-11-06T14:24:34.0543992Z       |                                                                                          [01;35m[K|[m[K   [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-06T14:24:34.0544331Z       |                                                                                          [01;35m[Kunsigned int[m[K
2025-11-06T14:24:34.0544639Z       |                                                                                         [32m[K%lu[m[K
2025-11-06T14:24:34.0545851Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2178:32:[m[K [01;31m[Kerror: [m[K'[01m[Kconst class TrackedDrone[m[K' has no member named '[01m[Krssi[m[K'
2025-11-06T14:24:34.0546119Z  2178 |             entry.rssi = drone.[01;31m[Krssi[m[K;
2025-11-06T14:24:34.0546340Z       |                                [01;31m[K^~~~[m[K
2025-11-06T14:24:34.0563607Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2182:35:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-06T14:24:34.0564101Z  2182 |             entry.display_color = [01;31m[Kget_drone_type_color[m[K(entry.type);
2025-11-06T14:24:34.0564629Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0571960Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-06T14:24:34.0573955Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2214:1:[m[K [01;31m[Kerror: [m[Kdefinition of explicitly-defaulted '[01m[Kui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()[m[K'
2025-11-06T14:24:34.0574432Z  2214 | [01;31m[KLoadingScreenView[m[K::~LoadingScreenView() {
2025-11-06T14:24:34.0574639Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0575460Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0577200Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:752:5:[m[K [01;36m[Knote: [m[K'[01m[Kvirtual ui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()[m[K' explicitly defaulted here
2025-11-06T14:24:34.0577465Z   752 |     [01;36m[K~[m[KLoadingScreenView() = default;
2025-11-06T14:24:34.0577586Z       |     [01;36m[K^[m[K
2025-11-06T14:24:34.0580328Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2226:1:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator::ScanningCoordinator(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, ui::external_app::enhanced_drone_analyzer::DroneDisplayController&, ui::external_app::enhanced_drone_analyzer::AudioManager&)[m[K'
2025-11-06T14:24:34.0580686Z  2226 | [01;31m[KScanningCoordinator[m[K::ScanningCoordinator(NavigationView& nav,
2025-11-06T14:24:34.0580816Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0581288Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0581697Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0583918Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:47:8:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator::ScanningCoordinator(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, ui::external_app::enhanced_drone_analyzer::DroneDisplayController&, ui::external_app::enhanced_drone_analyzer::AudioManager&)[m[K' previously defined here
2025-11-06T14:24:34.0584258Z    47 | inline [01;36m[KScanningCoordinator[m[K::ScanningCoordinator(NavigationView& nav,
2025-11-06T14:24:34.0584395Z       |        [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0585376Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2242:1:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator::~ScanningCoordinator()[m[K'
2025-11-06T14:24:34.0585618Z  2242 | [01;31m[KScanningCoordinator[m[K::~ScanningCoordinator() {
2025-11-06T14:24:34.0585738Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0586203Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0586596Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0587529Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:54:8:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator::~ScanningCoordinator()[m[K' previously defined here
2025-11-06T14:24:34.0587915Z    54 | inline [01;36m[KScanningCoordinator[m[K::~ScanningCoordinator() {
2025-11-06T14:24:34.0588035Z       |        [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0588978Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2246:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::start_coordinated_scanning()[m[K'
2025-11-06T14:24:34.0589243Z  2246 | void [01;31m[KScanningCoordinator[m[K::start_coordinated_scanning() {
2025-11-06T14:24:34.0589359Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0589807Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0590185Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0591137Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:58:13:[m[K [01;36m[Knote: [m[K'[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::start_coordinated_scanning()[m[K' previously defined here
2025-11-06T14:24:34.0591535Z    58 | inline void [01;36m[KScanningCoordinator[m[K::start_coordinated_scanning() {
2025-11-06T14:24:34.0591665Z       |             [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0592574Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2259:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::stop_coordinated_scanning()[m[K'
2025-11-06T14:24:34.0592828Z  2259 | void [01;31m[KScanningCoordinator[m[K::stop_coordinated_scanning() {
2025-11-06T14:24:34.0593071Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0593501Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0593866Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0594788Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:64:13:[m[K [01;36m[Knote: [m[K'[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::stop_coordinated_scanning()[m[K' previously defined here
2025-11-06T14:24:34.0595065Z    64 | inline void [01;36m[KScanningCoordinator[m[K::stop_coordinated_scanning() {
2025-11-06T14:24:34.0595186Z       |             [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0596119Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2269:7:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kstatic msg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::scanning_thread_function(void*)[m[K'
2025-11-06T14:24:34.0596407Z  2269 | msg_t [01;31m[KScanningCoordinator[m[K::scanning_thread_function(void* arg) {
2025-11-06T14:24:34.0596523Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0596942Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0597316Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0598278Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:74:14:[m[K [01;36m[Knote: [m[K'[01m[Kstatic msg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::scanning_thread_function(void*)[m[K' previously defined here
2025-11-06T14:24:34.0598579Z    74 | inline msg_t [01;36m[KScanningCoordinator[m[K::scanning_thread_function(void* arg) {
2025-11-06T14:24:34.0598831Z       |              [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0599740Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2274:7:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kmsg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::coordinated_scanning_thread()[m[K'
2025-11-06T14:24:34.0600008Z  2274 | msg_t [01;31m[KScanningCoordinator[m[K::coordinated_scanning_thread() {
2025-11-06T14:24:34.0600130Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0600542Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0600908Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0601837Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:78:14:[m[K [01;36m[Knote: [m[K'[01m[Kmsg_t ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::coordinated_scanning_thread()[m[K' previously defined here
2025-11-06T14:24:34.0602120Z    78 | inline msg_t [01;36m[KScanningCoordinator[m[K::coordinated_scanning_thread() {
2025-11-06T14:24:34.0602244Z       |              [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0603642Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2298:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::update_runtime_parameters(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)[m[K'
2025-11-06T14:24:34.0604043Z  2298 | void [01;31m[KScanningCoordinator[m[K::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
2025-11-06T14:24:34.0604157Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0604580Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0604952Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0606127Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:70:13:[m[K [01;36m[Knote: [m[K'[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::update_runtime_parameters(const ui::external_app::enhanced_drone_analyzer::DroneAnalyzerSettings&)[m[K' previously defined here
2025-11-06T14:24:34.0606533Z    70 | inline void [01;36m[KScanningCoordinator[m[K::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
2025-11-06T14:24:34.0606676Z       |             [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0607627Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2302:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::show_session_summary(const string&)[m[K'
2025-11-06T14:24:34.0607946Z  2302 | void [01;31m[KScanningCoordinator[m[K::show_session_summary(const std::string& summary) {
2025-11-06T14:24:34.0608058Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.0608979Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2302:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ScanningCoordinator::show_session_summary(const string&)[m[K'
2025-11-06T14:24:34.0609388Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.0609761Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.0610534Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::ScanningCoordinator[m[K' defined here
2025-11-06T14:24:34.0610800Z    18 | class [01;36m[KScanningCoordinator[m[K {
2025-11-06T14:24:34.0610921Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.1296410Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1297312Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1298005Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1299164Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-06T14:24:34.1300247Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1301052Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kbegin(const char* const&)[m[K'
2025-11-06T14:24:34.1301538Z    89 |         : path{[01;31m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-06T14:24:34.1301691Z       |                [01;31m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1302137Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-06T14:24:34.1302626Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1303331Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1303776Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1304685Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::begin(std::initializer_list<_Tp>)[m[K'
2025-11-06T14:24:34.1304884Z    89 |     [01;36m[Kbegin[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-06T14:24:34.1305039Z       |     [01;36m[K^~~~~[m[K
2025-11-06T14:24:34.1305602Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1306083Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1306516Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1306943Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1307610Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-06T14:24:34.1307846Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-06T14:24:34.1307988Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1308331Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1308762Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1309183Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1310081Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&)[m[K'
2025-11-06T14:24:34.1310493Z    48 |     [01;36m[Kbegin[m[K(_Container& __cont) -> decltype(__cont.begin())
2025-11-06T14:24:34.1310605Z       |     [01;36m[K^~~~~[m[K
2025-11-06T14:24:34.1311192Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1312111Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&) [with _Container = const char* const][m[K':
2025-11-06T14:24:34.1312649Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-06T14:24:34.1313241Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1313949Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:50:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1314353Z    48 |     begin(_Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-06T14:24:34.1314511Z       |                                           [01;31m[K~~~~~~~^~~~~[m[K
2025-11-06T14:24:34.1315023Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-06T14:24:34.1315461Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1316254Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&)[m[K'
2025-11-06T14:24:34.1316485Z    58 |     [01;36m[Kbegin[m[K(const _Container& __cont) -> decltype(__cont.begin())
2025-11-06T14:24:34.1316589Z       |     [01;36m[K^~~~~[m[K
2025-11-06T14:24:34.1317088Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1317907Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&) [with _Container = const char*][m[K':
2025-11-06T14:24:34.1318432Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-06T14:24:34.1318869Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1319565Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:56:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1319814Z    58 |     begin(const _Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-06T14:24:34.1319983Z       |                                                 [01;31m[K~~~~~~~^~~~~[m[K
2025-11-06T14:24:34.1320490Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-06T14:24:34.1320919Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1321640Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::begin(_Tp (&)[_Nm])[m[K'
2025-11-06T14:24:34.1321894Z    87 |     [01;36m[Kbegin[m[K(_Tp (&__arr)[_Nm])
2025-11-06T14:24:34.1321996Z       |     [01;36m[K^~~~~[m[K
2025-11-06T14:24:34.1322497Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1322907Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1323413Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1323791Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1324249Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1324473Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-06T14:24:34.1324607Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1325025Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1325408Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1325779Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1326442Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::begin(std::valarray<_Tp>&)[m[K'
2025-11-06T14:24:34.1326662Z   104 |   template<typename _Tp> _Tp* [01;36m[Kbegin[m[K(valarray<_Tp>&);
2025-11-06T14:24:34.1326802Z       |                               [01;36m[K^~~~~[m[K
2025-11-06T14:24:34.1327297Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1327700Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1328073Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1328461Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1328999Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1329211Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-06T14:24:34.1329344Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1329636Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1330008Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1330375Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1331061Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::begin(const std::valarray<_Tp>&)[m[K'
2025-11-06T14:24:34.1331306Z   105 |   template<typename _Tp> const _Tp* [01;36m[Kbegin[m[K(const valarray<_Tp>&);
2025-11-06T14:24:34.1331450Z       |                                     [01;36m[K^~~~~[m[K
2025-11-06T14:24:34.1332056Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1332459Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1332838Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1333314Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1333885Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1334099Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-06T14:24:34.1334230Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1334683Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kend(const char* const&)[m[K'
2025-11-06T14:24:34.1334995Z    89 |         : path{std::begin(source), [01;31m[Kstd::end(source)[m[K} {
2025-11-06T14:24:34.1335145Z       |                                    [01;31m[K~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1335489Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-06T14:24:34.1335746Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1336118Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1336484Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1337204Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::end(std::initializer_list<_Tp>)[m[K'
2025-11-06T14:24:34.1337381Z    99 |     [01;36m[Kend[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-06T14:24:34.1337484Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.1337966Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1338363Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1338732Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1339099Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1339653Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-06T14:24:34.1339848Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-06T14:24:34.1339997Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1340288Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1340694Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1341063Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1341804Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&)[m[K'
2025-11-06T14:24:34.1342111Z    68 |     [01;36m[Kend[m[K(_Container& __cont) -> decltype(__cont.end())
2025-11-06T14:24:34.1342217Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.1342705Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1343626Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&) [with _Container = const char* const][m[K':
2025-11-06T14:24:34.1344165Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-06T14:24:34.1344607Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1345296Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:48:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1345618Z    68 |     end(_Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-06T14:24:34.1345768Z       |                                         [01;31m[K~~~~~~~^~~[m[K
2025-11-06T14:24:34.1346289Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-06T14:24:34.1346731Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1347511Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&)[m[K'
2025-11-06T14:24:34.1347719Z    78 |     [01;36m[Kend[m[K(const _Container& __cont) -> decltype(__cont.end())
2025-11-06T14:24:34.1347824Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.1348319Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1349108Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&) [with _Container = const char*][m[K':
2025-11-06T14:24:34.1349640Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-06T14:24:34.1350076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1350759Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:54:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1350994Z    78 |     end(const _Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-06T14:24:34.1351142Z       |                                               [01;31m[K~~~~~~~^~~[m[K
2025-11-06T14:24:34.1351651Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-06T14:24:34.1352086Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:61:61:[m[K   required from here
2025-11-06T14:24:34.1352794Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::end(_Tp (&)[_Nm])[m[K'
2025-11-06T14:24:34.1353149Z    97 |     [01;36m[Kend[m[K(_Tp (&__arr)[_Nm])
2025-11-06T14:24:34.1353255Z       |     [01;36m[K^~~[m[K
2025-11-06T14:24:34.1353745Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1354149Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1354526Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1354891Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1355346Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1355552Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-06T14:24:34.1355693Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1356096Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1356494Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1356871Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1357546Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::end(std::valarray<_Tp>&)[m[K'
2025-11-06T14:24:34.1357763Z   106 |   template<typename _Tp> _Tp* [01;36m[Kend[m[K(valarray<_Tp>&);
2025-11-06T14:24:34.1357897Z       |                               [01;36m[K^~~[m[K
2025-11-06T14:24:34.1358392Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1358807Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1359179Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1359541Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1360095Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1360292Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-06T14:24:34.1360433Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1360736Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-06T14:24:34.1361106Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-06T14:24:34.1361476Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1362165Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::end(const std::valarray<_Tp>&)[m[K'
2025-11-06T14:24:34.1362404Z   107 |   template<typename _Tp> const _Tp* [01;36m[Kend[m[K(const valarray<_Tp>&);
2025-11-06T14:24:34.1362538Z       |                                     [01;36m[K^~~[m[K
2025-11-06T14:24:34.1363267Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1363675Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1364047Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1364416Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1364985Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-06T14:24:34.1365177Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-06T14:24:34.1365325Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-06T14:24:34.1365948Z [01m[K/havoc/firmware/application/./file.hpp:89:52:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kstd::filesystem::path::path(<brace-enclosed initializer list>)[m[K'
2025-11-06T14:24:34.1366245Z    89 |         : path{std::begin(source), std::end(source)[01;31m[K}[m[K {
2025-11-06T14:24:34.1366395Z       |                                                    [01;31m[K^[m[K
2025-11-06T14:24:34.1366879Z [01m[K/havoc/firmware/application/./file.hpp:102:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const TCHAR*)[m[K'
2025-11-06T14:24:34.1367023Z   102 |     [01;36m[Kpath[m[K(const TCHAR* const s)
2025-11-06T14:24:34.1367132Z       |     [01;36m[K^~~~[m[K
2025-11-06T14:24:34.1367494Z [01m[K/havoc/firmware/application/./file.hpp:102:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-06T14:24:34.1367977Z [01m[K/havoc/firmware/application/./file.hpp:98:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const char16_t*)[m[K'
2025-11-06T14:24:34.1368125Z    98 |     [01;36m[Kpath[m[K(const char16_t* const s)
2025-11-06T14:24:34.1368226Z       |     [01;36m[K^~~~[m[K
2025-11-06T14:24:34.1368581Z [01m[K/havoc/firmware/application/./file.hpp:98:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-06T14:24:34.1369168Z [01m[K/havoc/firmware/application/./file.hpp:93:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class InputIt> std::filesystem::path::path(InputIt, InputIt)[m[K'
2025-11-06T14:24:34.1369296Z    93 |     [01;36m[Kpath[m[K(InputIt first,
2025-11-06T14:24:34.1369393Z       |     [01;36m[K^~~~[m[K
2025-11-06T14:24:34.1369773Z [01m[K/havoc/firmware/application/./file.hpp:93:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1370337Z [01m[K/havoc/firmware/application/./file.hpp:88:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class Source> std::filesystem::path::path(const Source&)[m[K'
2025-11-06T14:24:34.1370474Z    88 |     [01;36m[Kpath[m[K(const Source& source)
2025-11-06T14:24:34.1370574Z       |     [01;36m[K^~~~[m[K
2025-11-06T14:24:34.1370959Z [01m[K/havoc/firmware/application/./file.hpp:88:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-06T14:24:34.1371463Z [01m[K/havoc/firmware/application/./file.hpp:83:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(std::filesystem::path&&)[m[K'
2025-11-06T14:24:34.1371586Z    83 |     [01;36m[Kpath[m[K(path&& p)
2025-11-06T14:24:34.1371687Z       |     [01;36m[K^~~~[m[K
2025-11-06T14:24:34.1372037Z [01m[K/havoc/firmware/application/./file.hpp:83:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-06T14:24:34.1372623Z [01m[K/havoc/firmware/application/./file.hpp:79:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const std::filesystem::path&)[m[K'
2025-11-06T14:24:34.1372872Z    79 |     [01;36m[Kpath[m[K(const path& p)
2025-11-06T14:24:34.1373079Z       |     [01;36m[K^~~~[m[K
2025-11-06T14:24:34.1373444Z [01m[K/havoc/firmware/application/./file.hpp:79:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-06T14:24:34.1373902Z [01m[K/havoc/firmware/application/./file.hpp:75:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path()[m[K'
2025-11-06T14:24:34.1374010Z    75 |     [01;36m[Kpath[m[K()
2025-11-06T14:24:34.1374107Z       |     [01;36m[K^~~~[m[K
2025-11-06T14:24:34.1374468Z [01m[K/havoc/firmware/application/./file.hpp:75:5:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 2 provided
2025-11-06T14:24:34.1454399Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-06T14:24:34.1454708Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-06T14:24:34.1455188Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.1455875Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.1456344Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1457980Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h: In instantiation of '[01m[Ktypename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...) [with _Tp = ui::external_app::enhanced_drone_analyzer::AudioManager; _Args = {}; typename std::_MakeUniq<_Tp>::__single_object = std::unique_ptr<ui::external_app::enhanced_drone_analyzer::AudioManager>][m[K':
2025-11-06T14:24:34.1458552Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1985:45:[m[K   required from here
2025-11-06T14:24:34.1459513Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:849:30:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ui::external_app::enhanced_drone_analyzer::AudioManager[m[K'
2025-11-06T14:24:34.1459879Z   849 |     { return unique_ptr<_Tp>([01;31m[Knew _Tp(std::forward<_Args>(__args)...)[m[K); }
2025-11-06T14:24:34.1460097Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-06T14:24:34.1460644Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-06T14:24:34.1461123Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.1461969Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:16:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ui::external_app::enhanced_drone_analyzer::AudioManager[m[K'
2025-11-06T14:24:34.1462104Z    16 | class [01;36m[KAudioManager[m[K;
2025-11-06T14:24:34.1462221Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-06T14:24:34.2063753Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-06T14:24:34.2064577Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-06T14:24:34.2065430Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-06T14:24:34.2066505Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-06T14:24:34.2067603Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-06T14:24:34.2069646Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h: In instantiation of '[01m[Kvoid std::default_delete<_Tp>::operator()(_Tp*) const [with _Tp = ui::external_app::enhanced_drone_analyzer::AudioManager][m[K':
2025-11-06T14:24:34.2072142Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:284:17:[m[K   required from '[01m[Kstd::unique_ptr<_Tp, _Dp>::~unique_ptr() [with _Tp = ui::external_app::enhanced_drone_analyzer::AudioManager; _Dp = std::default_delete<ui::external_app::enhanced_drone_analyzer::AudioManager>][m[K'
2025-11-06T14:24:34.2073836Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1985:45:[m[K   required from here
2025-11-06T14:24:34.2075203Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:79:16:[m[K [01;31m[Kerror: [m[Kinvalid application of '[01m[Ksizeof[m[K' to incomplete type '[01m[Kui::external_app::enhanced_drone_analyzer::AudioManager[m[K'
2025-11-06T14:24:34.2076094Z    79 |  static_assert([01;31m[Ksizeof(_Tp)[m[K>0,
2025-11-06T14:24:34.2076414Z       |                [01;31m[K^~~~~~~~~~~[m[K
2025-11-06T14:24:34.3437677Z [01m[Kcc1plus:[m[K [01;35m[Kwarning: [m[Kunrecognized command line option '[01m[K-Wno-volatile[m[K'
2025-11-06T14:24:34.3476343Z make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5775: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
2025-11-06T14:24:34.3481689Z make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
2025-11-06T14:24:34.3484183Z make: *** [Makefile:101: all] Error 2
2025-11-06T14:24:34.4249031Z ##[error]Process completed with exit code 2.