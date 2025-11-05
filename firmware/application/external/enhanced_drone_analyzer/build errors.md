 [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1082:27:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-05T14:36:44.2494668Z  1082 |         progress_bar[i] = [01;35m[K'█'[m[K;
2025-11-05T14:36:44.2495228Z       |                           [01;35m[K^~~~~[m[K
2025-11-05T14:36:44.2496417Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1327:61:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-05T14:36:44.2497463Z  1327 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'▲'[m[K; break;
2025-11-05T14:36:44.2497986Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-05T14:36:44.2499067Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1328:58:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-05T14:36:44.2501949Z  1328 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'▼'[m[K; break;
2025-11-05T14:36:44.2502725Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-05T14:36:44.2504129Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1331:37:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-05T14:36:44.2505540Z  1331 |             default: trend_symbol = [01;35m[K'■'[m[K; break;
2025-11-05T14:36:44.2506157Z       |                                     [01;35m[K^~~~~[m[K
2025-11-05T14:36:45.0299244Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool load_settings_from_sd_card(DroneAnalyzerSettings&)[m[K':
2025-11-05T14:36:45.0302871Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:28:21:[m[K [01;31m[Kerror: [m[K'[01m[Kclass Optional<std::filesystem::filesystem_error>[m[K' has no member named '[01m[Kis_error[m[K'
2025-11-05T14:36:45.0304715Z    28 |     if (open_result.[01;31m[Kis_error[m[K()) {
2025-11-05T14:36:45.0305277Z       |                     [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.0306986Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:34:42:[m[K [01;31m[Kerror: [m[K'[01m[Kclass File[m[K' has no member named '[01m[Kread_line[m[K'; did you mean '[01m[Kread_file[m[K'?
2025-11-05T14:36:45.0308984Z    34 |         auto read_result = settings_file.[01;31m[Kread_line[m[K(line_buffer, sizeof(line_buffer));
2025-11-05T14:36:45.0309735Z       |                                          [01;31m[K^~~~~~~~~[m[K
2025-11-05T14:36:45.0310264Z       |                                          [32m[Kread_file[m[K
2025-11-05T14:36:45.0405711Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0408335Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneScanner()[m[K':
2025-11-05T14:36:45.0411515Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:377:10:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.0413385Z   377 |     bool [01;35m[Kscanning_active_[m[K = false;
2025-11-05T14:36:45.0413996Z       |          [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0415786Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:375:13:[m[K [01;35m[Kwarning: [m[K  '[01m[KThread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.0417238Z   375 |     Thread* [01;35m[Kscanning_thread_[m[K = nullptr;
2025-11-05T14:36:45.0417710Z       |             [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0419154Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:139:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.0420456Z   139 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-05T14:36:45.0420941Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0421956Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0424453Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:400:22:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.0426573Z   400 |     WidebandScanData [01;35m[Kwideband_scan_data_[m[K;
2025-11-05T14:36:45.0427160Z       |                      [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0429150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:379:15:[m[K [01;35m[Kwarning: [m[K  '[01m[KFreqmanDB ui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.0430703Z   379 |     FreqmanDB [01;35m[Kfreq_db_[m[K;
2025-11-05T14:36:45.0431160Z       |               [01;35m[K^~~~~~~~[m[K
2025-11-05T14:36:45.0432640Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:139:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.0433900Z   139 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-05T14:36:45.0434367Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0436334Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:139:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::tracked_drones_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.0439973Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:139:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.0443421Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:139:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::detection_logger_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.0465063Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:158:5:[m[K [01;31m[Kerror: [m[K'[01m[Kinitialize_wideband_scanning[m[K' was not declared in this scope; did you mean '[01m[Kinitialize_database_and_scanner[m[K'?
2025-11-05T14:36:45.0466921Z   158 |     [01;31m[Kinitialize_wideband_scanning[m[K();
2025-11-05T14:36:45.0467549Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0468362Z       |     [32m[Kinitialize_database_and_scanner[m[K
2025-11-05T14:36:45.0471393Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-05T14:36:45.0474105Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:181:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_wideband_scanning()[m[K'
2025-11-05T14:36:45.0476202Z   181 | void [01;31m[KDroneScanner[m[K::initialize_wideband_scanning() {
2025-11-05T14:36:45.0476889Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0478930Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:181:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_wideband_scanning()[m[K'
2025-11-05T14:36:45.0480708Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0482598Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0483983Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0484757Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0486778Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:186:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::setup_wideband_range(Frequency, Frequency)[m[K'
2025-11-05T14:36:45.0488987Z   186 | void [01;31m[KDroneScanner[m[K::setup_wideband_range(Frequency min_freq, Frequency max_freq) {
2025-11-05T14:36:45.0489724Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0491831Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:186:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::setup_wideband_range(Frequency, Frequency)[m[K'
2025-11-05T14:36:45.0493986Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0496072Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0497411Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0497874Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0499660Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::start_scanning()[m[K':
2025-11-05T14:36:45.0502290Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:224:43:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kconst char*[m[K' to '[01m[Ktprio_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.0503984Z   224 |                                           [01;31m[K"drone_scanner"[m[K, NORMALPRIO,
2025-11-05T14:36:45.0504601Z       |                                           [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0505132Z       |                                           [01;31m[K|[m[K
2025-11-05T14:36:45.0505671Z       |                                           [01;31m[Kconst char*[m[K
2025-11-05T14:36:45.0506432Z In file included from [01m[K/havoc/firmware/chibios/os/kernel/include/ch.h:116[m[K,
2025-11-05T14:36:45.0507206Z                  from [01m[K/havoc/firmware/common/ffconf.h:2[m[K,
2025-11-05T14:36:45.0507966Z                  from [01m[K/havoc/firmware/chibios-portapack/ext/fatfs/src/ff.h:29[m[K,
2025-11-05T14:36:45.0509157Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:26[m[K,
2025-11-05T14:36:45.0510392Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.0511741Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.0513065Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0514948Z [01m[K/havoc/firmware/chibios/os/kernel/include/chschd.h:57:25:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kint[m[K' to '[01m[Ktfunc_t[m[K' {aka '[01m[Klong int (*)(void*)[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.0516428Z    57 | #define NORMALPRIO      [01;31m[K64[m[K          /**< @brief Normal user priority.       */
2025-11-05T14:36:45.0517061Z       |                         [01;31m[K^~[m[K
2025-11-05T14:36:45.0517505Z       |                         [01;31m[K|[m[K
2025-11-05T14:36:45.0517944Z       |                         [01;31m[Kint[m[K
2025-11-05T14:36:45.0519333Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:224:60:[m[K [01;36m[Knote: [m[Kin expansion of macro '[01m[KNORMALPRIO[m[K'
2025-11-05T14:36:45.0520758Z   224 |                                           "drone_scanner", [01;36m[KNORMALPRIO[m[K,
2025-11-05T14:36:45.0521358Z       |                                                            [01;36m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.0523033Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:225:73:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kmsg_t (*)(void*)[m[K' {aka '[01m[Klong int (*)(void*)[m[K'} to '[01m[Kvoid*[m[K' [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.0524638Z   225 |                                           scanning_thread_function, this[01;31m[K)[m[K;
2025-11-05T14:36:45.0525429Z       |                                                                         [01;31m[K^[m[K
2025-11-05T14:36:45.0525992Z       |                                                                         [01;31m[K|[m[K
2025-11-05T14:36:45.0526657Z       |                                                                         [01;31m[Kmsg_t (*)(void*) {aka long int (*)(void*)}[m[K
2025-11-05T14:36:45.0528562Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:225:73:[m[K [01;31m[Kerror: [m[Ktoo many arguments to function '[01m[KThread* chThdCreateFromHeap(MemoryHeap*, size_t, tprio_t, tfunc_t, void*)[m[K'
2025-11-05T14:36:45.0530274Z In file included from [01m[K/havoc/firmware/chibios/os/kernel/include/ch.h:128[m[K,
2025-11-05T14:36:45.0531027Z                  from [01m[K/havoc/firmware/common/ffconf.h:2[m[K,
2025-11-05T14:36:45.0531726Z                  from [01m[K/havoc/firmware/chibios-portapack/ext/fatfs/src/ff.h:29[m[K,
2025-11-05T14:36:45.0532715Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:26[m[K,
2025-11-05T14:36:45.0533864Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.0534956Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.0536061Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0537188Z [01m[K/havoc/firmware/chibios/os/kernel/include/chdynamic.h:60:11:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-05T14:36:45.0538392Z    60 |   Thread *[01;36m[KchThdCreateFromHeap[m[K(MemoryHeap *heapp, size_t size,
2025-11-05T14:36:45.0539064Z       |           [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0540775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::stop_scanning()[m[K':
2025-11-05T14:36:45.0544725Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:241:27:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has no member named '[01m[Kis_session_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::session_active_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.0547372Z   241 |     if (detection_logger_.[01;31m[Kis_session_active[m[K()) {
2025-11-05T14:36:45.0547998Z       |                           [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0549153Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0550812Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:347:14:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.0552145Z   347 |         bool [01;36m[Ksession_active_[m[K;
2025-11-05T14:36:45.0552639Z       |              [01;36m[K^~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0554329Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kmsg_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread()[m[K':
2025-11-05T14:36:45.0556917Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:252:33:[m[K [01;31m[Kerror: [m[K'[01m[KchThdShouldTerminateX[m[K' was not declared in this scope; did you mean '[01m[KchThdShouldTerminate[m[K'?
2025-11-05T14:36:45.0558809Z   252 |     while (scanning_active_ && ![01;31m[KchThdShouldTerminateX[m[K()) {
2025-11-05T14:36:45.0559688Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0560250Z       |                                 [32m[KchThdShouldTerminate[m[K
2025-11-05T14:36:45.0583372Z In file included from [01m[K/havoc/firmware/chibios/os/kernel/include/ch.h:115[m[K,
2025-11-05T14:36:45.0584178Z                  from [01m[K/havoc/firmware/common/ffconf.h:2[m[K,
2025-11-05T14:36:45.0584785Z                  from [01m[K/havoc/firmware/chibios-portapack/ext/fatfs/src/ff.h:29[m[K,
2025-11-05T14:36:45.0585611Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:26[m[K,
2025-11-05T14:36:45.0586559Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.0587554Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.0588855Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0590122Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:253:32:[m[K [01;31m[Kerror: [m[K'[01m[KMIN_SCAN_INTERVAL_MS[m[K' was not declared in this scope
2025-11-05T14:36:45.0591147Z   253 |         chThdSleepMilliseconds([01;31m[KMIN_SCAN_INTERVAL_MS[m[K);
2025-11-05T14:36:45.0600611Z       |                                [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0601643Z [01m[K/havoc/firmware/chibios/os/kernel/include/chvt.h:67:30:[m[K [01;36m[Knote: [m[Kin definition of macro '[01m[KMS2ST[m[K'
2025-11-05T14:36:45.0602379Z    67 |   ((systime_t)(((((uint32_t)([01;36m[Kmsec[m[K)) * ((uint32_t)CH_FREQUENCY) - 1UL) /     \
2025-11-05T14:36:45.0602786Z       |                              [01;36m[K^~~~[m[K
2025-11-05T14:36:45.0603614Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:253:9:[m[K [01;36m[Knote: [m[Kin expansion of macro '[01m[KchThdSleepMilliseconds[m[K'
2025-11-05T14:36:45.0604412Z   253 |         [01;36m[KchThdSleepMilliseconds[m[K(MIN_SCAN_INTERVAL_MS);
2025-11-05T14:36:45.0604776Z       |         [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0605532Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:258:15:[m[K [01;31m[Kerror: [m[K'[01m[KMSG_OK[m[K' was not declared in this scope
2025-11-05T14:36:45.0606224Z   258 |     chThdExit([01;31m[KMSG_OK[m[K);
2025-11-05T14:36:45.0606508Z       |               [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.0607503Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::load_frequency_database()[m[K':
2025-11-05T14:36:45.0609091Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:264:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass FreqmanDB[m[K' has no member named '[01m[Kis_open[m[K'; did you mean '[01m[Kopen[m[K'?
2025-11-05T14:36:45.0610164Z   264 |         if (!freq_db_.[01;31m[Kis_open[m[K()) {
2025-11-05T14:36:45.0610470Z       |                       [01;31m[K^~~~~~~[m[K
2025-11-05T14:36:45.0610753Z       |                       [32m[Kopen[m[K
2025-11-05T14:36:45.0611569Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:278:14:[m[K [01;31m[Kerror: [m[Kexception handling disabled, use '[01m[K-fexceptions[m[K' to enable
2025-11-05T14:36:45.0612309Z   278 |     } catch ([01;31m[K...[m[K) {
2025-11-05T14:36:45.0612572Z       |              [01;31m[K^~~[m[K
2025-11-05T14:36:45.0613539Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Ksize_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_database_size() const[m[K':
2025-11-05T14:36:45.0615152Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:284:21:[m[K [01;31m[Kerror: [m[K'[01m[Kconst class FreqmanDB[m[K' has no member named '[01m[Kis_open[m[K'; did you mean '[01m[Kopen[m[K'?
2025-11-05T14:36:45.0616085Z   284 |     return freq_db_.[01;31m[Kis_open[m[K() ? freq_db_.entry_count() : 0;
2025-11-05T14:36:45.0616460Z       |                     [01;31m[K^~~~~~~[m[K
2025-11-05T14:36:45.0616745Z       |                     [32m[Kopen[m[K
2025-11-05T14:36:45.0632703Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::set_scanning_mode(ui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode)[m[K':
2025-11-05T14:36:45.0635494Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:290:5:[m[K [01;31m[Kerror: [m[K'[01m[Kreset_scan_cycles[m[K' was not declared in this scope
2025-11-05T14:36:45.0636829Z   290 |     [01;31m[Kreset_scan_cycles[m[K();
2025-11-05T14:36:45.0637373Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0638506Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-05T14:36:45.0641337Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:306:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0642812Z   306 | void [01;31m[KDroneScanner[m[K::perform_scan_cycle(DroneHardwareController& hardware) {
2025-11-05T14:36:45.0643247Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0643836Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0645210Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:312:10:[m[K [01;36m[Knote: [m[Kcandidate is: '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_scan_cycle(DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0646881Z   312 |     void [01;36m[Kperform_scan_cycle[m[K(DroneHardwareController& hardware);
2025-11-05T14:36:45.0647351Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0648613Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0649401Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0649688Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0651033Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:323:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0653415Z   323 | void [01;31m[KDroneScanner[m[K::perform_database_scan_cycle(DroneHardwareController& hardware) {
2025-11-05T14:36:45.0654224Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0654834Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0656234Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:366:10:[m[K [01;36m[Knote: [m[Kcandidate is: '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0658400Z   366 |     void [01;36m[Kperform_database_scan_cycle[m[K(DroneHardwareController& hardware);
2025-11-05T14:36:45.0659142Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0660535Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0661334Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0661614Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0663859Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:353:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_wideband_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0666318Z   353 | void [01;31m[KDroneScanner[m[K::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
2025-11-05T14:36:45.0666775Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0667360Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0668931Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:367:10:[m[K [01;36m[Knote: [m[Kcandidate is: '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_wideband_scan_cycle(DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0670029Z   367 |     void [01;36m[Kperform_wideband_scan_cycle[m[K(DroneHardwareController& hardware);
2025-11-05T14:36:45.0670489Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0671418Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0672194Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0672477Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0673675Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:386:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_detection_override(const freqman_entry&, int32_t, int32_t)[m[K'
2025-11-05T14:36:45.0675032Z   386 | void [01;31m[KDroneScanner[m[K::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
2025-11-05T14:36:45.0675567Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0676732Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:386:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_detection_override(const freqman_entry&, int32_t, int32_t)[m[K'
2025-11-05T14:36:45.0678025Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0679523Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0680290Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0680569Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0681834Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:394:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::process_wideband_detection_with_override(const freqman_entry&, int32_t, int32_t, int32_t)[m[K'
2025-11-05T14:36:45.0683302Z   394 | void [01;31m[KDroneScanner[m[K::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
2025-11-05T14:36:45.0683804Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0685051Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:394:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::process_wideband_detection_with_override(const freqman_entry&, int32_t, int32_t, int32_t)[m[K'
2025-11-05T14:36:45.0686900Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0689333Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0690184Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0690478Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0691804Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:448:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_hybrid_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0693126Z   448 | void [01;31m[KDroneScanner[m[K::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
2025-11-05T14:36:45.0693566Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0694138Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0696651Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:368:10:[m[K [01;36m[Knote: [m[Kcandidate is: '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_hybrid_scan_cycle(DroneHardwareController&)[m[K'
2025-11-05T14:36:45.0698577Z   368 |     void [01;36m[Kperform_hybrid_scan_cycle[m[K(DroneHardwareController& hardware);
2025-11-05T14:36:45.0699410Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0701222Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:288:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' defined here
2025-11-05T14:36:45.0702661Z   288 | class [01;36m[KDroneScanner[m[K {
2025-11-05T14:36:45.0703157Z       |       [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0705176Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::process_rssi_detection(const freqman_entry&, int32_t)[m[K':
2025-11-05T14:36:45.0707845Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:457:73:[m[K [01;31m[Kerror: [m[K'[01m[KUNKNOWN[m[K' is not a member of '[01m[KThreatLevel[m[K'
2025-11-05T14:36:45.0708959Z   457 |     if (!SimpleDroneValidation::validate_rssi_signal(rssi, ThreatLevel::[01;31m[KUNKNOWN[m[K)) {
2025-11-05T14:36:45.0709656Z       |                                                                         [01;31m[K^~~~~~~[m[K
2025-11-05T14:36:45.0710476Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:465:55:[m[K [01;31m[Kerror: [m[K'[01m[KUNKNOWN[m[K' is not a member of '[01m[KThreatLevel[m[K'
2025-11-05T14:36:45.0711330Z   465 |     if (!validate_detection_simple(rssi, ThreatLevel::[01;31m[KUNKNOWN[m[K)) {
2025-11-05T14:36:45.0711765Z       |                                                       [01;31m[K^~~~~~~[m[K
2025-11-05T14:36:45.0712741Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:470:44:[m[K [01;31m[Kerror: [m[K'[01m[Kclass FreqmanDB[m[K' has no member named '[01m[Klookup_frequency[m[K'
2025-11-05T14:36:45.0713669Z   470 |     const auto* db_entry = drone_database_.[01;31m[Klookup_frequency[m[K(entry.frequency_a);
2025-11-05T14:36:45.0714127Z       |                                            [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0715864Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:522:35:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger[m[K' has no member named '[01m[Kis_session_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::session_active_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.0717345Z   522 |             if (detection_logger_.[01;31m[Kis_session_active[m[K()) {
2025-11-05T14:36:45.0717728Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0718452Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0719399Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:347:14:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.0720055Z   347 |         bool [01;36m[Ksession_active_[m[K;
2025-11-05T14:36:45.0720354Z       |              [01;36m[K^~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0721516Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::update_tracked_drone(DroneType, Frequency, int32_t, ThreatLevel)[m[K':
2025-11-05T14:36:45.0723150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:570:50:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KTrackedDrone& TrackedDrone::operator=(const TrackedDrone&)[m[K'
2025-11-05T14:36:45.0724056Z   570 |     tracked_drones_[oldest_index] = TrackedDrone([01;31m[K)[m[K;
2025-11-05T14:36:45.0724443Z       |                                                  [01;31m[K^[m[K
2025-11-05T14:36:45.0725047Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0725958Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:127:19:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-05T14:36:45.0726677Z   127 |     TrackedDrone& [01;36m[Koperator[m[K=(const TrackedDrone&) = delete;
2025-11-05T14:36:45.0727054Z       |                   [01;36m[K^~~~~~~~[m[K
2025-11-05T14:36:45.0728198Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::remove_stale_drones()[m[K':
2025-11-05T14:36:45.0731035Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:590:46:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KTrackedDrone& TrackedDrone::operator=(const TrackedDrone&)[m[K'
2025-11-05T14:36:45.0732450Z   590 |                 tracked_drones_[write_idx] = [01;31m[Kdrone[m[K;
2025-11-05T14:36:45.0732803Z       |                                              [01;31m[K^~~~~[m[K
2025-11-05T14:36:45.0733404Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0734320Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:127:19:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-05T14:36:45.0735033Z   127 |     TrackedDrone& [01;36m[Koperator[m[K=(const TrackedDrone&) = delete;
2025-11-05T14:36:45.0735408Z       |                   [01;36m[K^~~~~~~~[m[K
2025-11-05T14:36:45.0736497Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:594:54:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KTrackedDrone& TrackedDrone::operator=(const TrackedDrone&)[m[K'
2025-11-05T14:36:45.0737396Z   594 |             tracked_drones_[read_idx] = TrackedDrone([01;31m[K)[m[K;
2025-11-05T14:36:45.0737770Z       |                                                      [01;31m[K^[m[K
2025-11-05T14:36:45.0738524Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0739436Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:127:19:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-05T14:36:45.0740130Z   127 |     TrackedDrone& [01;36m[Koperator[m[K=(const TrackedDrone&) = delete;
2025-11-05T14:36:45.0740496Z       |                   [01;36m[K^~~~~~~~[m[K
2025-11-05T14:36:45.0741527Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[KFrequency ui::external_app::enhanced_drone_analyzer::DroneScanner::get_current_scanning_frequency() const[m[K':
2025-11-05T14:36:45.0743057Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:632:18:[m[K [01;31m[Kerror: [m[K'[01m[Kconst class FreqmanDB[m[K' has no member named '[01m[Kis_open[m[K'; did you mean '[01m[Kopen[m[K'?
2025-11-05T14:36:45.0744001Z   632 |     if (freq_db_.[01;31m[Kis_open[m[K() && current_db_index_ < freq_db_.entry_count()) {
2025-11-05T14:36:45.0744373Z       |                  [01;31m[K^~~~~~~[m[K
2025-11-05T14:36:45.0744648Z       |                  [32m[Kopen[m[K
2025-11-05T14:36:45.0745583Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:633:42:[m[K [01;31m[Kerror: [m[K'[01m[Kconst class FreqmanDB[m[K' has no member named '[01m[Kget_entry[m[K'; did you mean '[01m[Kinsert_entry[m[K'?
2025-11-05T14:36:45.0746554Z   633 |         const auto& entry_opt = freq_db_.[01;31m[Kget_entry[m[K(current_db_index_);
2025-11-05T14:36:45.0746959Z       |                                          [01;31m[K^~~~~~~~~[m[K
2025-11-05T14:36:45.0747292Z       |                                          [32m[Kinsert_entry[m[K
2025-11-05T14:36:45.0749355Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::DroneScanner::get_session_summary() const[m[K':
2025-11-05T14:36:45.0752098Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:646:53:[m[K [01;31m[Kerror: [m[K'[01m[Kget_scan_cycles[m[K' was not declared in this scope; did you mean '[01m[Kscan_cycles_[m[K'?
2025-11-05T14:36:45.0753870Z   646 |     return detection_logger_.format_session_summary([01;31m[Kget_scan_cycles[m[K(), get_total_detections());
2025-11-05T14:36:45.0754412Z       |                                                     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0754905Z       |                                                     [32m[Kscan_cycles_[m[K
2025-11-05T14:36:45.0757180Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::DroneDetectionLogger()[m[K':
2025-11-05T14:36:45.0759410Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:654:8:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::csv_log_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.0760645Z   654 | inline [01;35m[KDroneScanner[m[K::DroneDetectionLogger::DroneDetectionLogger()
2025-11-05T14:36:45.0761046Z       |        [01;35m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0762429Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::log_detection(const DetectionLogEntry&)[m[K':
2025-11-05T14:36:45.0765009Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:58:[m[K [01;31m[Kerror: [m[K'[01m[Kstd::string[m[K' {aka '[01m[Kclass std::__cxx11::basic_string<char>[m[K'} has no member named '[01m[Kstring[m[K'
2025-11-05T14:36:45.0766239Z   681 |     auto error = csv_log_.append(generate_log_filename().[01;31m[Kstring[m[K());
2025-11-05T14:36:45.0766854Z       |                                                          [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.0768923Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::ensure_csv_header()[m[K':
2025-11-05T14:36:45.0771874Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:695:15:[m[K [01;31m[Kerror: [m[K'[01m[Kclass Optional<std::filesystem::filesystem_error>[m[K' has no member named '[01m[Kis_error[m[K'
2025-11-05T14:36:45.0772796Z   695 |     if (error.[01;31m[Kis_error[m[K()) return false;
2025-11-05T14:36:45.0773108Z       |               [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.0774098Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:697:16:[m[K [01;31m[Kerror: [m[K'[01m[Kclass Optional<std::filesystem::filesystem_error>[m[K' has no member named '[01m[Kis_error[m[K'
2025-11-05T14:36:45.0774934Z   697 |     if (!error.[01;31m[Kis_error[m[K()) {
2025-11-05T14:36:45.0775219Z       |                [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.0776341Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry&)[m[K':
2025-11-05T14:36:45.0778335Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:708:16:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0779377Z   708 |              "[01;35m[K%u[m[K,%u,%d,%u,%u,%u,%.2f\n",
2025-11-05T14:36:45.0779910Z       |               [01;35m[K~^[m[K
2025-11-05T14:36:45.0780355Z       |                [01;35m[K|[m[K
2025-11-05T14:36:45.0780802Z       |                [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.0781304Z       |               [32m[K%lu[m[K
2025-11-05T14:36:45.0781886Z   709 |              [32m[Kentry.timestamp[m[K, entry.frequency_hz, entry.rssi_db,
2025-11-05T14:36:45.0782364Z       |              [32m[K~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0782733Z       |                    [32m[K|[m[K
2025-11-05T14:36:45.0783222Z       |                    [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.0785432Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:708:19:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0787312Z   708 |              "%u,[01;35m[K%u[m[K,%d,%u,%u,%u,%.2f\n",
2025-11-05T14:36:45.0787868Z       |                  [01;35m[K~^[m[K
2025-11-05T14:36:45.0788504Z       |                   [01;35m[K|[m[K
2025-11-05T14:36:45.0789031Z       |                   [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.0789534Z       |                  [32m[K%lu[m[K
2025-11-05T14:36:45.0790034Z   709 |              entry.timestamp, [32m[Kentry.frequency_hz[m[K, entry.rssi_db,
2025-11-05T14:36:45.0790429Z       |                               [32m[K~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0790729Z       |                                     [32m[K|[m[K
2025-11-05T14:36:45.0791064Z       |                                     [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.0792186Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:708:22:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 6 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0793148Z   708 |              "%u,%u,[01;35m[K%d[m[K,%u,%u,%u,%.2f\n",
2025-11-05T14:36:45.0793439Z       |                     [01;35m[K~^[m[K
2025-11-05T14:36:45.0793715Z       |                      [01;35m[K|[m[K
2025-11-05T14:36:45.0793979Z       |                      [01;35m[Kint[m[K
2025-11-05T14:36:45.0794256Z       |                     [32m[K%ld[m[K
2025-11-05T14:36:45.0794637Z   709 |              entry.timestamp, entry.frequency_hz, [32m[Kentry.rssi_db[m[K,
2025-11-05T14:36:45.0795035Z       |                                                   [32m[K~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0795353Z       |                                                         [32m[K|[m[K
2025-11-05T14:36:45.0795694Z       |                                                         [32m[Kint32_t {aka long int}[m[K
2025-11-05T14:36:45.0796931Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::DroneScanner::DroneDetectionLogger::format_session_summary(size_t, size_t) const[m[K':
2025-11-05T14:36:45.0799809Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:729:235:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 9 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0802098Z   729 |     "SCANNING SESSION COMPLETE\n========================\n\nSESSION STATISTICS:\nDuration: %.1f seconds\nScan Cycles: %zu\nTotal Detections: %zu\n\nPERFORMANCE:\nAvg. detections/cycle: %.2f\nDetection rate: %.1f/sec\nLogged entries: [01;35m[K%u[m[K\n\nEnhanced Drone Analyzer v0.3",
2025-11-05T14:36:45.0803184Z       |                                                                                                                                                                                                                                          [01;35m[K~^[m[K
2025-11-05T14:36:45.0803693Z       |                                                                                                                                                                                                                                           [01;35m[K|[m[K
2025-11-05T14:36:45.0804371Z       |                                                                                                                                                                                                                                           [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.0804888Z       |                                                                                                                                                                                                                                          [32m[K%lu[m[K
2025-11-05T14:36:45.0805317Z   730 |         static_cast<float>(session_duration_ms) / 1000.0f, scan_cycles, total_detections,
2025-11-05T14:36:45.0805879Z   731 |         avg_detections_per_cycle, detections_per_second, [32m[Klogged_count_[m[K);
2025-11-05T14:36:45.0806578Z       |                                                          [32m[K~~~~~~~~~~~~~[m[K                                                                                                                                                                     
2025-11-05T14:36:45.0806971Z       |                                                          [32m[K|[m[K
2025-11-05T14:36:45.0807338Z       |                                                          [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.0808585Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::DroneHardwareController(SpectrumMode)[m[K':
2025-11-05T14:36:45.0810511Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:744:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_spectrum_config_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.0811800Z   744 | [01;35m[KDroneHardwareController[m[K::DroneHardwareController(SpectrumMode mode)
2025-11-05T14:36:45.0812226Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0813531Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:746:63:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-05T14:36:45.0815391Z   746 |       spectrum_streaming_active_(false), last_valid_rssi_(-120[01;31m[K)[m[K
2025-11-05T14:36:45.0816185Z       |                                                               [01;31m[K^[m[K
2025-11-05T14:36:45.0817264Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0818363Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0819686Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-05T14:36:45.0820620Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-05T14:36:45.0820928Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0822095Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-05T14:36:45.0824181Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-05T14:36:45.0825146Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-05T14:36:45.0825481Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0826397Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-05T14:36:45.0828726Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:744:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::message_handler_frame_sync_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.0831111Z   744 | [01;35m[KDroneHardwareController[m[K::DroneHardwareController(SpectrumMode mode)
2025-11-05T14:36:45.0831690Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0832888Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:746:63:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-05T14:36:45.0833886Z   746 |       spectrum_streaming_active_(false), last_valid_rssi_(-120[01;31m[K)[m[K
2025-11-05T14:36:45.0834309Z       |                                                               [01;31m[K^[m[K
2025-11-05T14:36:45.0834908Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0835714Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0837005Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-05T14:36:45.0837941Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-05T14:36:45.0838420Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0839122Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-05T14:36:45.0840542Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-05T14:36:45.0841486Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-05T14:36:45.0841819Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0842500Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-05T14:36:45.0844076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:744:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneHardwareController::radio_state_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.0845303Z   744 | [01;35m[KDroneHardwareController[m[K::DroneHardwareController(SpectrumMode mode)
2025-11-05T14:36:45.0845717Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0846752Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneHardwareController::initialize_spectrum_collector()[m[K':
2025-11-05T14:36:45.0848579Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:778:22:[m[K [01;31m[Kerror: [m[K'[01m[KChannelSpectrumConfigChange[m[K' is not a member of '[01m[KMessage::ID[m[K'; did you mean '[01m[KChannelSpectrumConfig[m[K'?
2025-11-05T14:36:45.0849556Z   778 |         Message::ID::[01;31m[KChannelSpectrumConfigChange[m[K,
2025-11-05T14:36:45.0850067Z       |                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0850395Z       |                      [32m[KChannelSpectrumConfig[m[K
2025-11-05T14:36:45.0851585Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:783:87:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K'
2025-11-05T14:36:45.0852745Z   783 |         [this](const Message* const p) { (void)p; process_channel_spectrum_data({}); }[01;31m[K)[m[K;
2025-11-05T14:36:45.0853228Z       |                                                                                       [01;31m[K^[m[K
2025-11-05T14:36:45.0854172Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0855698Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0857552Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K'[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K' is implicitly deleted because the default definition would be ill-formed:
2025-11-05T14:36:45.0858843Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-05T14:36:45.0859180Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0860277Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;31m[Kerror: [m[Knon-static const member '[01m[Kconst Message::ID MessageHandlerRegistration::message_id[m[K', can't use default assignment operator
2025-11-05T14:36:45.0863305Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::tune_to_frequency(Frequency)[m[K':
2025-11-05T14:36:45.0867105Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:811:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing RxRadioState = class RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K' {aka '[01m[Kclass RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K'} has no member named '[01m[Kconfigure_tuning[m[K'
2025-11-05T14:36:45.0868861Z   811 |     radio_state_.[01;31m[Kconfigure_tuning[m[K(frequency_hz);
2025-11-05T14:36:45.0869230Z       |                  [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0870284Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneHardwareController::start_spectrum_streaming()[m[K':
2025-11-05T14:36:45.0873413Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:818:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing RxRadioState = class RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K' {aka '[01m[Kclass RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K'} has no member named '[01m[Kstart_sampling[m[K'
2025-11-05T14:36:45.0874877Z   818 |     radio_state_.[01;31m[Kstart_sampling[m[K();
2025-11-05T14:36:45.0875308Z       |                  [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0876935Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneHardwareController::stop_spectrum_streaming()[m[K':
2025-11-05T14:36:45.0879503Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:823:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing RxRadioState = class RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K' {aka '[01m[Kclass RadioState<ReceiverModel, (& portapack::receiver_model)>[m[K'} has no member named '[01m[Kstop_sampling[m[K'
2025-11-05T14:36:45.0880887Z   823 |     radio_state_.[01;31m[Kstop_sampling[m[K();
2025-11-05T14:36:45.0881198Z       |                  [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0882276Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kint32_t ui::external_app::enhanced_drone_analyzer::DroneHardwareController::get_real_rssi_from_hardware(Frequency)[m[K':
2025-11-05T14:36:45.0883775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:826:72:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Ktarget_frequency[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-05T14:36:45.0884915Z   826 | int32_t DroneHardwareController::get_real_rssi_from_hardware([01;35m[KFrequency target_frequency[m[K) {
2025-11-05T14:36:45.0885480Z       |                                                              [01;35m[K~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0886610Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::update(ThreatLevel, size_t, size_t, size_t, Frequency, bool)[m[K':
2025-11-05T14:36:45.0888309Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:875:56:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-05T14:36:45.0889189Z   875 |     threat_progress_bar_.set_style([01;31m[Kget_threat_bar_color(max_threat)[m[K);
2025-11-05T14:36:45.0889640Z       |                                    [01;31m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0890265Z       |                                                        [01;31m[K|[m[K
2025-11-05T14:36:45.0890974Z       |                                                        [01;31m[Kui::Color[m[K
2025-11-05T14:36:45.0892038Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.0893486Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0894862Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0895828Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-05T14:36:45.0896906Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-05T14:36:45.0897468Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.0898505Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:889:56:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-05T14:36:45.0899383Z   889 |     threat_status_main_.set_style([01;31m[Kget_threat_text_color(max_threat)[m[K);
2025-11-05T14:36:45.0899828Z       |                                   [01;31m[K~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0900165Z       |                                                        [01;31m[K|[m[K
2025-11-05T14:36:45.0900543Z       |                                                        [01;31m[Kui::Color[m[K
2025-11-05T14:36:45.0901129Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.0902256Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0903409Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0904522Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-05T14:36:45.0905636Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-05T14:36:45.0906238Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.0907889Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:911:54:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-05T14:36:45.0909615Z   911 |     threat_frequency_.set_style([01;31m[Kget_threat_text_color(max_threat)[m[K);
2025-11-05T14:36:45.0910393Z       |                                 [01;31m[K~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0911055Z       |                                                      [01;31m[K|[m[K
2025-11-05T14:36:45.0911649Z       |                                                      [01;31m[Kui::Color[m[K
2025-11-05T14:36:45.0912750Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.0914189Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0915627Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0917397Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-05T14:36:45.0918560Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-05T14:36:45.0918913Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.0920002Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::SmartThreatHeader::paint(ui::Painter&)[m[K':
2025-11-05T14:36:45.0921514Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:985:41:[m[K [01;31m[Kerror: [m[Kinvalid use of non-static member function '[01m[Kuint8_t ui::Color::r()[m[K'
2025-11-05T14:36:45.0922463Z   985 |         pulse_color = Color([01;31m[Kpulse_color.r[m[K, pulse_color.g, pulse_color.b, alpha);
2025-11-05T14:36:45.0923262Z       |                             [01;31m[K~~~~~~~~~~~~^[m[K
2025-11-05T14:36:45.0924249Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:25[m[K,
2025-11-05T14:36:45.0925720Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.0926547Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0927190Z [01m[K/havoc/firmware/common/ui.hpp:119:13:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-05T14:36:45.0927595Z   119 |     uint8_t [01;36m[Kr[m[K() {
2025-11-05T14:36:45.0927863Z       |             [01;36m[K^[m[K
2025-11-05T14:36:45.0928915Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:986:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope; did you mean '[01m[Kparent_rect[m[K'?
2025-11-05T14:36:45.0930482Z   986 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect_.width(), 4}, pulse_color);
2025-11-05T14:36:45.0931412Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0931921Z       |                                 [32m[Kparent_rect[m[K
2025-11-05T14:36:45.0933178Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:986:111:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-05T14:36:45.0934262Z   986 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 4}, pulse_color[01;31m[K)[m[K;
2025-11-05T14:36:45.0934839Z       |                                                                                                               [01;31m[K^[m[K
2025-11-05T14:36:45.0935263Z In file included from [01m[K/havoc/firmware/common/ui_widget.hpp:30[m[K,
2025-11-05T14:36:45.0935873Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.0937031Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0938688Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0940493Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-05T14:36:45.0941427Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-05T14:36:45.0941763Z       |                         [01;36m[K~~~~~^[m[K
2025-11-05T14:36:45.0942780Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ThreatCard::update_card(const DisplayDroneEntry&)[m[K':
2025-11-05T14:36:45.0944394Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1005:45:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-05T14:36:45.0945950Z  1005 |     card_text_.set_style([01;31m[Kget_card_text_color()[m[K);
2025-11-05T14:36:45.0946604Z       |                          [01;31m[K~~~~~~~~~~~~~~~~~~~^~[m[K
2025-11-05T14:36:45.0947125Z       |                                             [01;31m[K|[m[K
2025-11-05T14:36:45.0947760Z       |                                             [01;31m[Kui::Color[m[K
2025-11-05T14:36:45.0949007Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.0950426Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0951238Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0952204Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-05T14:36:45.0952855Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-05T14:36:45.0953218Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.0954191Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kstd::string ui::external_app::enhanced_drone_analyzer::ThreatCard::render_compact() const[m[K':
2025-11-05T14:36:45.0955828Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1029:79:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 8 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0956949Z  1029 |         snprintf(buffer, sizeof(buffer), "🛰️ %s │ %.1fG │ %s %s │ [01;35m[K%d[m[KdB",
2025-11-05T14:36:45.0957407Z       |                                                                              [01;35m[K~^[m[K
2025-11-05T14:36:45.0957941Z       |                                                                               [01;35m[K|[m[K
2025-11-05T14:36:45.0958458Z       |                                                                               [01;35m[Kint[m[K
2025-11-05T14:36:45.0958803Z       |                                                                              [32m[K%ld[m[K
2025-11-05T14:36:45.0959249Z  1030 |                 threat_name_.c_str(), freq_mhz, trend_symbol, threat_abbr, [32m[Krssi_[m[K);
2025-11-05T14:36:45.0959677Z       |                                                                            [32m[K~~~~~[m[K
2025-11-05T14:36:45.0960018Z       |                                                                            [32m[K|[m[K
2025-11-05T14:36:45.0960510Z       |                                                                            [32m[Kint32_t {aka long int}[m[K
2025-11-05T14:36:45.0961668Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1032:79:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 8 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0962751Z  1032 |         snprintf(buffer, sizeof(buffer), "🛰️ %s │ %.0fM │ %s %s │ [01;35m[K%d[m[KdB",
2025-11-05T14:36:45.0963195Z       |                                                                              [01;35m[K~^[m[K
2025-11-05T14:36:45.0963537Z       |                                                                               [01;35m[K|[m[K
2025-11-05T14:36:45.0963887Z       |                                                                               [01;35m[Kint[m[K
2025-11-05T14:36:45.0964237Z       |                                                                              [32m[K%ld[m[K
2025-11-05T14:36:45.0964684Z  1033 |                 threat_name_.c_str(), freq_mhz, trend_symbol, threat_abbr, [32m[Krssi_[m[K);
2025-11-05T14:36:45.0965116Z       |                                                                            [32m[K~~~~~[m[K
2025-11-05T14:36:45.0965450Z       |                                                                            [32m[K|[m[K
2025-11-05T14:36:45.0965817Z       |                                                                            [32m[Kint32_t {aka long int}[m[K
2025-11-05T14:36:45.0966841Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::ThreatCard::paint(ui::Painter&)[m[K':
2025-11-05T14:36:45.0968421Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1066:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope; did you mean '[01m[Kparent_rect[m[K'?
2025-11-05T14:36:45.0969484Z  1066 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect_.width(), 2}, bg_color);
2025-11-05T14:36:45.0969992Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0970305Z       |                                 [32m[Kparent_rect[m[K
2025-11-05T14:36:45.0971327Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1066:108:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-05T14:36:45.0972393Z  1066 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2}, bg_color[01;31m[K)[m[K;
2025-11-05T14:36:45.0972973Z       |                                                                                                            [01;31m[K^[m[K
2025-11-05T14:36:45.0973416Z In file included from [01m[K/havoc/firmware/common/ui_widget.hpp:30[m[K,
2025-11-05T14:36:45.0974155Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.0974897Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.0975656Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.0976619Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-05T14:36:45.0977283Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-05T14:36:45.0977726Z       |                         [01;36m[K~~~~~^[m[K
2025-11-05T14:36:45.0978965Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_scanning_progress(uint32_t, uint32_t, uint32_t)[m[K':
2025-11-05T14:36:45.0980491Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1079:28:[m[K [01;31m[Kerror: [m[Kinitializer-string for array of chars is too long [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.0981333Z  1079 |     char progress_bar[9] = [01;31m[K"░░░░░░░░"[m[K;
2025-11-05T14:36:45.0981682Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0982791Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1082:27:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849672[m[K' to '[01m[K'\210'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-05T14:36:45.0983747Z  1082 |         progress_bar[i] = [01;35m[K'█'[m[K;
2025-11-05T14:36:45.0984062Z       |                           [01;35m[K^~~~~[m[K
2025-11-05T14:36:45.0985225Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1086:43:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0986326Z  1086 |     snprintf(buffer, sizeof(buffer), "%s [01;35m[K%u[m[K%% C:%u D:%u",
2025-11-05T14:36:45.0986728Z       |                                          [01;35m[K~^[m[K
2025-11-05T14:36:45.0987025Z       |                                           [01;35m[K|[m[K
2025-11-05T14:36:45.0987348Z       |                                           [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.0987669Z       |                                          [32m[K%lu[m[K
2025-11-05T14:36:45.0988174Z  1087 |             progress_bar, [32m[Kprogress_percent[m[K, total_cycles, detections);
2025-11-05T14:36:45.0988587Z       |                           [32m[K~~~~~~~~~~~~~~~~[m[K 
2025-11-05T14:36:45.0988874Z       |                           [32m[K|[m[K
2025-11-05T14:36:45.0989200Z       |                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.0990393Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1086:50:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 6 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0991495Z  1086 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:[01;35m[K%u[m[K D:%u",
2025-11-05T14:36:45.0991906Z       |                                                 [01;35m[K~^[m[K
2025-11-05T14:36:45.0992219Z       |                                                  [01;35m[K|[m[K
2025-11-05T14:36:45.0992830Z       |                                                  [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.0993380Z       |                                                 [32m[K%lu[m[K
2025-11-05T14:36:45.0994135Z  1087 |             progress_bar, progress_percent, [32m[Ktotal_cycles[m[K, detections);
2025-11-05T14:36:45.0994862Z       |                                             [32m[K~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.0995262Z       |                                             [32m[K|[m[K
2025-11-05T14:36:45.0995611Z       |                                             [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.0996997Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1086:55:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 7 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.0998250Z  1086 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:[01;35m[K%u[m[K",
2025-11-05T14:36:45.0998661Z       |                                                      [01;35m[K~^[m[K
2025-11-05T14:36:45.0998974Z       |                                                       [01;35m[K|[m[K
2025-11-05T14:36:45.0999307Z       |                                                       [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.0999645Z       |                                                      [32m[K%lu[m[K
2025-11-05T14:36:45.1000074Z  1087 |             progress_bar, progress_percent, total_cycles, [32m[Kdetections[m[K);
2025-11-05T14:36:45.1000504Z       |                                                           [32m[K~~~~~~~~~~[m[K
2025-11-05T14:36:45.1000828Z       |                                                           [32m[K|[m[K
2025-11-05T14:36:45.1001192Z       |                                                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.1002400Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1093:61:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.1003654Z  1093 |         snprintf(buffer, sizeof(buffer), "⚠️ DETECTED: [01;35m[K%u[m[K threats found!", [32m[Kdetections[m[K);
2025-11-05T14:36:45.1004214Z       |                                                            [01;35m[K~^[m[K                  [32m[K~~~~~~~~~~[m[K
2025-11-05T14:36:45.1004607Z       |                                                             [01;35m[K|[m[K                  [32m[K|[m[K
2025-11-05T14:36:45.1005081Z       |                                                             [01;35m[Kunsigned int[m[K       [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.1005501Z       |                                                            [32m[K%lu[m[K
2025-11-05T14:36:45.1006631Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_alert_status(ThreatLevel, size_t, const string&)[m[K':
2025-11-05T14:36:45.1008026Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1111:126:[m[K [01;31m[Kerror: [m[Kexpression cannot be used as a function
2025-11-05T14:36:45.1009146Z  1111 |     alert_text_.set_style((threat >= ThreatLevel::CRITICAL) ? Theme::getInstance()->fg_red : Theme::getInstance()->fg_yellow([01;31m[K)[m[K);
2025-11-05T14:36:45.1009798Z       |                                                                                                                              [01;31m[K^[m[K
2025-11-05T14:36:45.1010915Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::update_normal_status(const string&, const string&)[m[K':
2025-11-05T14:36:45.1012585Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1125:60:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kconst ui::Color[m[K' to '[01m[Kconst ui::Style*[m[K'
2025-11-05T14:36:45.1013517Z  1125 |     normal_text_.set_style([01;31m[KTheme::getInstance()->fg_light->foreground[m[K);
2025-11-05T14:36:45.1013981Z       |                            [01;31m[K~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~[m[K
2025-11-05T14:36:45.1014334Z       |                                                            [01;31m[K|[m[K
2025-11-05T14:36:45.1014826Z       |                                                            [01;31m[Kconst ui::Color[m[K
2025-11-05T14:36:45.1015428Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.1016215Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1016974Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1017907Z [01m[K/havoc/firmware/common/ui_widget.hpp:119:33:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Widget::set_style(const ui::Style*)[m[K'
2025-11-05T14:36:45.1018677Z   119 |     void set_style([01;36m[Kconst Style* new_style[m[K);
2025-11-05T14:36:45.1019026Z       |                    [01;36m[K~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.1020034Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::ConsoleStatusBar::paint(ui::Painter&)[m[K':
2025-11-05T14:36:45.1021507Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1148:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope; did you mean '[01m[Kparent_rect[m[K'?
2025-11-05T14:36:45.1022582Z  1148 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect_.width(), 2}, Color(32, 0, 0));
2025-11-05T14:36:45.1023102Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1023411Z       |                                 [32m[Kparent_rect[m[K
2025-11-05T14:36:45.1024402Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1148:115:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-05T14:36:45.1026294Z  1148 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect_.width(), 2}, Color(32, 0, 0)[01;31m[K)[m[K;
2025-11-05T14:36:45.1027176Z       |                                                                                                                   [01;31m[K^[m[K
2025-11-05T14:36:45.1027604Z In file included from [01m[K/havoc/firmware/common/ui_widget.hpp:30[m[K,
2025-11-05T14:36:45.1028422Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.1029221Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1029979Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1030972Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-05T14:36:45.1031812Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-05T14:36:45.1032140Z       |                         [01;36m[K~~~~~^[m[K
2025-11-05T14:36:45.1032731Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1034051Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)[m[K':
2025-11-05T14:36:45.1035909Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:627:21:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::nav_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.1036873Z   627 |     NavigationView& [01;35m[Knav_[m[K;
2025-11-05T14:36:45.1037166Z       |                     [01;35m[K^~~~[m[K
2025-11-05T14:36:45.1038415Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:617:14:[m[K [01;35m[Kwarning: [m[K  '[01m[KGradient ui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_gradient_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.1039399Z   617 |     Gradient [01;35m[Kspectrum_gradient_[m[K;
2025-11-05T14:36:45.1039720Z       |              [01;35m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1040487Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-05T14:36:45.1041365Z  1152 | [01;35m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-05T14:36:45.1041792Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1043051Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::detected_drones_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1045090Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::displayed_drones_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1046453Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1047524Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K':
2025-11-05T14:36:45.1049139Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::frequency[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1050069Z   130 | struct [01;35m[KDisplayDroneEntry[m[K {
2025-11-05T14:36:45.1050371Z       |        [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1051402Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1053044Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::threat[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1054677Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::rssi[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1056448Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::last_seen[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1058213Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type_name[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1060007Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::display_color[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1061678Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::trend[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1062859Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:15[m[K,
2025-11-05T14:36:45.1063663Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1064562Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array:[m[K In constructor '[01m[Kstd::array<DisplayDroneEntry, 3>::array()[m[K':
2025-11-05T14:36:45.1065674Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/array:94:12:[m[K [01;36m[Knote: [m[Ksynthesized method '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K' first required here
2025-11-05T14:36:45.1066361Z    94 |     struct [01;36m[Karray[m[K
2025-11-05T14:36:45.1066624Z       |            [01;36m[K^~~~~[m[K
2025-11-05T14:36:45.1067668Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::DroneDisplayController(ui::NavigationView&)[m[K':
2025-11-05T14:36:45.1069577Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1153:37:[m[K [01;36m[Knote: [m[Ksynthesized method '[01m[Kstd::array<DisplayDroneEntry, 3>::array()[m[K' first required here
2025-11-05T14:36:45.1070465Z  1153 |     : nav_(nav), spectrum_gradient_{[01;36m[K}[m[K
2025-11-05T14:36:45.1070816Z       |                                     [01;36m[K^[m[K
2025-11-05T14:36:45.1072072Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_row[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1073307Z  1152 | [01;35m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-05T14:36:45.1073732Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1075002Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_power_levels_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1077037Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::threat_bins_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1079319Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::spectrum_config_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1081856Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::message_handler_spectrum_config_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1084777Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1153:37:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-05T14:36:45.1085710Z  1153 |     : nav_(nav), spectrum_gradient_{[01;31m[K}[m[K
2025-11-05T14:36:45.1086047Z       |                                     [01;31m[K^[m[K
2025-11-05T14:36:45.1086644Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1087457Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1088889Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-05T14:36:45.1089836Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-05T14:36:45.1090162Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1090865Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-05T14:36:45.1092313Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-05T14:36:45.1093271Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-05T14:36:45.1093611Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1094308Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-05T14:36:45.1095950Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1152:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::message_handler_frame_sync_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1097237Z  1152 | [01;35m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-05T14:36:45.1097666Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1098784Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1153:37:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KMessageHandlerRegistration::MessageHandlerRegistration()[m[K'
2025-11-05T14:36:45.1099683Z  1153 |     : nav_(nav), spectrum_gradient_{[01;31m[K}[m[K
2025-11-05T14:36:45.1100002Z       |                                     [01;31m[K^[m[K
2025-11-05T14:36:45.1100592Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1101398Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1102831Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KMessageHandlerRegistration::MessageHandlerRegistration(Message::ID, std::function<void(Message*)>&&)[m[K'
2025-11-05T14:36:45.1103774Z   149 |     [01;36m[KMessageHandlerRegistration[m[K(
2025-11-05T14:36:45.1104094Z       |     [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1104785Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:149:5:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-05T14:36:45.1106323Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr MessageHandlerRegistration::MessageHandlerRegistration(const MessageHandlerRegistration&)[m[K'
2025-11-05T14:36:45.1107279Z   147 | class [01;36m[KMessageHandlerRegistration[m[K {
2025-11-05T14:36:45.1107621Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1108591Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:147:7:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-05T14:36:45.1110184Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1165:10:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K'
2025-11-05T14:36:45.1111152Z  1165 |         }[01;31m[K)[m[K;
2025-11-05T14:36:45.1111398Z       |          [01;31m[K^[m[K
2025-11-05T14:36:45.1112821Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1178:10:[m[K [01;31m[Kerror: [m[Kuse of deleted function '[01m[KMessageHandlerRegistration& MessageHandlerRegistration::operator=(const MessageHandlerRegistration&)[m[K'
2025-11-05T14:36:45.1114532Z  1178 |         }[01;31m[K)[m[K;
2025-11-05T14:36:45.1114970Z       |          [01;31m[K^[m[K
2025-11-05T14:36:45.1116668Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_detection_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K':
2025-11-05T14:36:45.1118655Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1193:30:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kchar*[m[K' to '[01m[Krf::Frequency[m[K' {aka '[01m[Klong long int[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.1119601Z  1193 |             big_display_.set([01;31m[Kfreq_buffer[m[K);
2025-11-05T14:36:45.1119940Z       |                              [01;31m[K^~~~~~~~~~~[m[K
2025-11-05T14:36:45.1120238Z       |                              [01;31m[K|[m[K
2025-11-05T14:36:45.1120531Z       |                              [01;31m[Kchar*[m[K
2025-11-05T14:36:45.1121102Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.1121899Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1122658Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1123584Z [01m[K/havoc/firmware/common/ui_widget.hpp:290:34:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::BigFrequency::set(rf::Frequency)[m[K'
2025-11-05T14:36:45.1124264Z   290 |     void set([01;36m[Kconst rf::Frequency frequency[m[K);
2025-11-05T14:36:45.1124616Z       |              [01;36m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.1126285Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1195:30:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kconst char*[m[K' to '[01m[Krf::Frequency[m[K' {aka '[01m[Klong long int[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.1127712Z  1195 |             big_display_.set([01;31m[K"SCANNING..."[m[K);
2025-11-05T14:36:45.1128058Z       |                              [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1128510Z       |                              [01;31m[K|[m[K
2025-11-05T14:36:45.1128810Z       |                              [01;31m[Kconst char*[m[K
2025-11-05T14:36:45.1129390Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.1130309Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1131382Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1133254Z [01m[K/havoc/firmware/common/ui_widget.hpp:290:34:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::BigFrequency::set(rf::Frequency)[m[K'
2025-11-05T14:36:45.1134207Z   290 |     void set([01;36m[Kconst rf::Frequency frequency[m[K);
2025-11-05T14:36:45.1134562Z       |              [01;36m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.1136222Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1198:26:[m[K [01;31m[Kerror: [m[Kinvalid conversion from '[01m[Kconst char*[m[K' to '[01m[Krf::Frequency[m[K' {aka '[01m[Klong long int[m[K'} [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.1137713Z  1198 |         big_display_.set([01;31m[K"READY"[m[K);
2025-11-05T14:36:45.1138376Z       |                          [01;31m[K^~~~~~~[m[K
2025-11-05T14:36:45.1138671Z       |                          [01;31m[K|[m[K
2025-11-05T14:36:45.1138974Z       |                          [01;31m[Kconst char*[m[K
2025-11-05T14:36:45.1139546Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.1140337Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1141094Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1142035Z [01m[K/havoc/firmware/common/ui_widget.hpp:290:34:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::BigFrequency::set(rf::Frequency)[m[K'
2025-11-05T14:36:45.1142771Z   290 |     void set([01;36m[Kconst rf::Frequency frequency[m[K);
2025-11-05T14:36:45.1143368Z       |              [01;36m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~~~[m[K
2025-11-05T14:36:45.1145534Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1227:75:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.1147126Z  1227 |         snprintf(status_buffer, sizeof(status_buffer), "%s - Detections: [01;35m[K%u[m[K",
2025-11-05T14:36:45.1147733Z       |                                                                          [01;35m[K~^[m[K
2025-11-05T14:36:45.1148293Z       |                                                                           [01;35m[K|[m[K
2025-11-05T14:36:45.1148719Z       |                                                                           [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.1149089Z       |                                                                          [32m[K%lu[m[K
2025-11-05T14:36:45.1149487Z  1228 |                 mode_str.c_str(), [32m[Kscanner.get_total_detections()[m[K);
2025-11-05T14:36:45.1150075Z       |                                   [32m[K~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K           
2025-11-05T14:36:45.1150423Z       |                                                               [32m[K|[m[K
2025-11-05T14:36:45.1150793Z       |                                                               [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.1152512Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1239:56:[m[K [01;31m[Kerror: [m[K'[01m[Kconst class ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has no member named '[01m[Kget_scan_cycles[m[K'; did you mean '[01m[Kuint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_cycles_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.1153928Z  1239 |                 current_idx + 1, loaded_freqs, scanner.[01;31m[Kget_scan_cycles[m[K());
2025-11-05T14:36:45.1154363Z       |                                                        [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1154969Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1155910Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:14:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.1156556Z   383 |     uint32_t [01;36m[Kscan_cycles_[m[K = 0;
2025-11-05T14:36:45.1156855Z       |              [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1158007Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)[m[K':
2025-11-05T14:36:45.1159648Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1271:25:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-05T14:36:45.1160447Z  1271 |         it->type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-05T14:36:45.1160805Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1161610Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1272:29:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-05T14:36:45.1162410Z  1272 |         it->display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-05T14:36:45.1162785Z       |                             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1163590Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1281:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-05T14:36:45.1164380Z  1281 |             entry.type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-05T14:36:45.1164753Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1165594Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1282:35:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-05T14:36:45.1166409Z  1282 |             entry.display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-05T14:36:45.1166801Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1167948Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1287:27:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display()[m[K'
2025-11-05T14:36:45.1169018Z  1287 |     update_drones_display([01;31m[K)[m[K;
2025-11-05T14:36:45.1169471Z       |                           [01;31m[K^[m[K
2025-11-05T14:36:45.1170061Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1171640Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:570:10:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K'
2025-11-05T14:36:45.1172825Z   570 |     void [01;36m[Kupdate_drones_display[m[K(const DroneScanner& scanner);
2025-11-05T14:36:45.1173205Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1174117Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:570:10:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 0 provided
2025-11-05T14:36:45.1175761Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::update_drones_display(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K':
2025-11-05T14:36:45.1177857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1309:23:[m[K [01;31m[Kerror: [m[K'[01m[Kstruct std::array<DisplayDroneEntry, 3>[m[K' has no member named '[01m[Kclear[m[K'
2025-11-05T14:36:45.1179194Z  1309 |     displayed_drones_.[01;31m[Kclear[m[K();
2025-11-05T14:36:45.1179512Z       |                       [01;31m[K^~~~~[m[K
2025-11-05T14:36:45.1180379Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1299:72:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Kscanner[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-05T14:36:45.1181378Z  1299 | void DroneDisplayController::update_drones_display([01;35m[Kconst DroneScanner& scanner[m[K) {
2025-11-05T14:36:45.1181915Z       |                                                    [01;35m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~[m[K
2025-11-05T14:36:45.1182976Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_drone_text_display()[m[K':
2025-11-05T14:36:45.1184803Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1319:5:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_1()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1185852Z  1319 |     [01;31m[Ktext_drone_1[m[K.set("");
2025-11-05T14:36:45.1186134Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1186397Z       |                 [32m[K()[m[K
2025-11-05T14:36:45.1187614Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1320:5:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_2()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1188775Z  1320 |     [01;31m[Ktext_drone_2[m[K.set("");
2025-11-05T14:36:45.1189051Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1189298Z       |                 [32m[K()[m[K
2025-11-05T14:36:45.1190514Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1321:5:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_3()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1191545Z  1321 |     [01;31m[Ktext_drone_3[m[K.set("");
2025-11-05T14:36:45.1191818Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1192232Z       |                 [32m[K()[m[K
2025-11-05T14:36:45.1193287Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1327:61:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849714[m[K' to '[01m[K'\262'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-05T14:36:45.1194395Z  1327 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'▲'[m[K; break;
2025-11-05T14:36:45.1194850Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-05T14:36:45.1196066Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1328:58:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849724[m[K' to '[01m[K'\274'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-05T14:36:45.1197156Z  1328 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'▼'[m[K; break;
2025-11-05T14:36:45.1197580Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-05T14:36:45.1198778Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1331:37:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849696[m[K' to '[01m[K'\240'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-05T14:36:45.1199771Z  1331 |             default: trend_symbol = [01;35m[K'■'[m[K; break;
2025-11-05T14:36:45.1200113Z       |                                     [01;35m[K^~~~~[m[K
2025-11-05T14:36:45.1201221Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1341:42:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 6 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.1202296Z  1341 |         snprintf(buffer, sizeof(buffer), [01;35m[KDRONE_DISPLAY_FORMAT[m[K,
2025-11-05T14:36:45.1202712Z       |                                          [01;35m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1202954Z ......
2025-11-05T14:36:45.1203167Z  1344 |                 [32m[Kdrone.rssi[m[K,
2025-11-05T14:36:45.1203474Z       |                 [32m[K~~~~~~~~~~[m[K                
2025-11-05T14:36:45.1203756Z       |                       [32m[K|[m[K
2025-11-05T14:36:45.1204051Z       |                       [32m[Kint32_t {aka long int}[m[K
2025-11-05T14:36:45.1205320Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1349:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_1()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1206387Z  1349 |                 [01;31m[Ktext_drone_1[m[K.set(buffer);
2025-11-05T14:36:45.1206698Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1206981Z       |                             [32m[K()[m[K
2025-11-05T14:36:45.1209409Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1350:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_1()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1210574Z  1350 |                 [01;31m[Ktext_drone_1[m[K.set_style(threat_color);
2025-11-05T14:36:45.1210906Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1211195Z       |                             [32m[K()[m[K
2025-11-05T14:36:45.1212461Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1353:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_2()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1213686Z  1353 |                 [01;31m[Ktext_drone_2[m[K.set(buffer);
2025-11-05T14:36:45.1213993Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1214265Z       |                             [32m[K()[m[K
2025-11-05T14:36:45.1215495Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1354:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_2()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1216687Z  1354 |                 [01;31m[Ktext_drone_2[m[K.set_style(threat_color);
2025-11-05T14:36:45.1217016Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1217292Z       |                             [32m[K()[m[K
2025-11-05T14:36:45.1218647Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1357:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_3()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1219708Z  1357 |                 [01;31m[Ktext_drone_3[m[K.set(buffer);
2025-11-05T14:36:45.1220032Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1220296Z       |                             [32m[K()[m[K
2025-11-05T14:36:45.1221519Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1358:17:[m[K [01;31m[Kerror: [m[Kinvalid use of member function '[01m[Kui::Text& ui::external_app::enhanced_drone_analyzer::DroneDisplayController::text_drone_3()[m[K' (did you forget the '[01m[K()[m[K' ?)
2025-11-05T14:36:45.1222594Z  1358 |                 [01;31m[Ktext_drone_3[m[K.set_style(threat_color);
2025-11-05T14:36:45.1222916Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1223188Z       |                             [32m[K()[m[K
2025-11-05T14:36:45.1224311Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::process_mini_spectrum_data(const ChannelSpectrum&)[m[K':
2025-11-05T14:36:45.1225793Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1374:9:[m[K [01;31m[Kerror: [m[K'[01m[Kget_max_power_for_current_bin[m[K' was not declared in this scope
2025-11-05T14:36:45.1226656Z  1374 |         [01;31m[Kget_max_power_for_current_bin[m[K(spectrum, current_bin_power);
2025-11-05T14:36:45.1227052Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1228061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneDisplayController::process_bins(uint8_t*)[m[K':
2025-11-05T14:36:45.1229583Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1385:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel_from_bin[m[K' was not declared in this scope
2025-11-05T14:36:45.1230379Z  1385 |             [01;31m[Kadd_spectrum_pixel_from_bin[m[K(*power_level);
2025-11-05T14:36:45.1230735Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1237639Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1387:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel_from_bin[m[K' was not declared in this scope
2025-11-05T14:36:45.1239307Z  1387 |             [01;31m[Kadd_spectrum_pixel_from_bin[m[K(0);
2025-11-05T14:36:45.1239919Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1241197Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-05T14:36:45.1242772Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1399:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t&)[m[K'
2025-11-05T14:36:45.1244169Z  1399 | void [01;31m[KDroneDisplayController[m[K::get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t& max_power) {
2025-11-05T14:36:45.1244705Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1246066Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1399:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t&)[m[K'
2025-11-05T14:36:45.1247415Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1248761Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:550:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneDisplayController[m[K' defined here
2025-11-05T14:36:45.1249604Z   550 | class [01;36m[KDroneDisplayController[m[K {
2025-11-05T14:36:45.1249918Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1251069Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1422:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_spectrum_pixel_from_bin(uint8_t)[m[K'
2025-11-05T14:36:45.1252230Z  1422 | void [01;31m[KDroneDisplayController[m[K::add_spectrum_pixel_from_bin(uint8_t power) {
2025-11-05T14:36:45.1252676Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1253802Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1422:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::add_spectrum_pixel_from_bin(uint8_t)[m[K'
2025-11-05T14:36:45.1255034Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1256240Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:550:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneDisplayController[m[K' defined here
2025-11-05T14:36:45.1257078Z   550 | class [01;36m[KDroneDisplayController[m[K {
2025-11-05T14:36:45.1257391Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1258503Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneDisplayController::render_mini_spectrum()[m[K':
2025-11-05T14:36:45.1259827Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1443:27:[m[K [01;31m[Kerror: [m[K'[01m[Kmutex[m[K' is not a member of '[01m[Kstd[m[K'
2025-11-05T14:36:45.1260799Z  1443 |     std::scoped_lock<std::[01;31m[Kmutex[m[K> lock(spectrum_access_mutex_);  // Section 3: Thread safety for spectrum rendering
2025-11-05T14:36:45.1261306Z       |                           [01;31m[K^~~~~[m[K
2025-11-05T14:36:45.1262303Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:20:1:[m[K [01;36m[Knote: [m[K'[01m[Kstd::mutex[m[K' is defined in header '[01m[K<mutex>[m[K'; did you forget to '[01m[K#include <mutex>[m[K'?
2025-11-05T14:36:45.1263186Z    19 | #include <cstdlib>
2025-11-05T14:36:45.1263431Z   +++ |+[32m[K#include <mutex>[m[K
2025-11-05T14:36:45.1263655Z    20 | #include <memory>
2025-11-05T14:36:45.1264304Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1443:32:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-05T14:36:45.1265226Z  1443 |     std::scoped_lock<std::mutex[01;31m[K>[m[K lock(spectrum_access_mutex_);  // Section 3: Thread safety for spectrum rendering
2025-11-05T14:36:45.1265727Z       |                                [01;31m[K^[m[K
2025-11-05T14:36:45.1273014Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1443:39:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_access_mutex_[m[K' was not declared in this scope
2025-11-05T14:36:45.1274096Z  1443 |     std::scoped_lock<std::mutex> lock([01;31m[Kspectrum_access_mutex_[m[K);  // Section 3: Thread safety for spectrum rendering
2025-11-05T14:36:45.1274642Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1279187Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1452:9:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay[m[K' was not declared in this scope; did you mean '[01m[Kportapack::display[m[K'?
2025-11-05T14:36:45.1280743Z  1452 |         [01;31m[Kdisplay[m[K.draw_pixels(
2025-11-05T14:36:45.1281042Z       |         [01;31m[K^~~~~~~[m[K
2025-11-05T14:36:45.1281357Z       |         [32m[Kportapack::display[m[K
2025-11-05T14:36:45.1281791Z In file included from [01m[K/havoc/firmware/application/./config_mode.hpp:27[m[K,
2025-11-05T14:36:45.1282365Z                  from [01m[K/havoc/firmware/common/portapack_persistent_memory.hpp:36[m[K,
2025-11-05T14:36:45.1282864Z                  from [01m[K/havoc/firmware/application/./rtc_time.hpp:29[m[K,
2025-11-05T14:36:45.1283281Z                  from [01m[K/havoc/firmware/common/ui_widget.hpp:32[m[K,
2025-11-05T14:36:45.1283859Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../event_m0.hpp:27[m[K,
2025-11-05T14:36:45.1284592Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:29[m[K,
2025-11-05T14:36:45.1285345Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1286204Z [01m[K/havoc/firmware/application/./portapack.hpp:57:21:[m[K [01;36m[Knote: [m[K'[01m[Kportapack::display[m[K' declared here
2025-11-05T14:36:45.1286747Z    57 | extern lcd::ILI9341 [01;36m[Kdisplay[m[K;
2025-11-05T14:36:45.1287054Z       |                     [01;36m[K^~~~~~~[m[K
2025-11-05T14:36:45.1287884Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1443:34:[m[K [01;35m[Kwarning: [m[Kunused variable '[01m[Klock[m[K' [[01;35m[K-Wunused-variable[m[K]
2025-11-05T14:36:45.1289084Z  1443 |     std::scoped_lock<std::mutex> [01;35m[Klock[m[K(spectrum_access_mutex_);  // Section 3: Thread safety for spectrum rendering
2025-11-05T14:36:45.1289617Z       |                                  [01;35m[K^~~~[m[K
2025-11-05T14:36:45.1295239Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController::DroneUIController(ui::NavigationView&, ui::external_app::enhanced_drone_analyzer::DroneHardwareController&, ui::external_app::enhanced_drone_analyzer::DroneScanner&, AudioManager&)[m[K':
2025-11-05T14:36:45.1298024Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1523:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController[m[K' does not have any field named '[01m[Kaudio_[m[K'
2025-11-05T14:36:45.1299357Z  1523 |       [01;31m[Kaudio_[m[K(audio),
2025-11-05T14:36:45.1299637Z       |       [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.1300893Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1516:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController::audio_mgr_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1302050Z  1516 | [01;35m[KDroneUIController[m[K::DroneUIController(NavigationView& nav,
2025-11-05T14:36:45.1302781Z       | [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1304455Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1516:1:[m[K [01;31m[Kerror: [m[Kuninitialized reference member in '[01m[Kclass AudioManager&[m[K' [[01;31m[K-fpermissive[m[K]
2025-11-05T14:36:45.1306469Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1309063Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:676:19:[m[K [01;36m[Knote: [m[K'[01m[KAudioManager& ui::external_app::enhanced_drone_analyzer::DroneUIController::audio_mgr_[m[K' should be initialized
2025-11-05T14:36:45.1310707Z   676 |     AudioManager& [01;36m[Kaudio_mgr_[m[K;
2025-11-05T14:36:45.1311226Z       |                   [01;36m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1313381Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1516:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneUIController::settings_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1315623Z  1516 | [01;35m[KDroneUIController[m[K::DroneUIController(NavigationView& nav,
2025-11-05T14:36:45.1316301Z       | [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1317604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_stop_scan()[m[K':
2025-11-05T14:36:45.1319287Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1540:5:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-05T14:36:45.1320464Z  1540 |     [01;31m[Kaudio_[m[K.stop_audio();
2025-11-05T14:36:45.1320863Z       |     [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.1321107Z       |     [32m[Kaudio[m[K
2025-11-05T14:36:45.1322717Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_toggle_mode()[m[K':
2025-11-05T14:36:45.1326844Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1548:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.1329783Z  1548 |         if (hardware_.[01;31m[Kis_spectrum_streaming_active[m[K()) {
2025-11-05T14:36:45.1330528Z       |                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1331666Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1333421Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:453:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.1334524Z   453 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-05T14:36:45.1335489Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1338845Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1553:24:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.1341050Z  1553 |         if (!hardware_.[01;31m[Kis_spectrum_streaming_active[m[K()) {
2025-11-05T14:36:45.1341758Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1343002Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1344090Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:453:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.1344800Z   453 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-05T14:36:45.1345176Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1346182Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_menu()[m[K':
2025-11-05T14:36:45.1347445Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1563:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1348510Z  1563 |         {[01;31m[KTranslator[m[K::translate("load_database"), [this]() { on_load_frequency_file(); }},
2025-11-05T14:36:45.1348956Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1349695Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1564:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1350571Z  1564 |         {[01;31m[KTranslator[m[K::translate("save_frequency"), [this]() { on_save_frequency(); }},
2025-11-05T14:36:45.1350984Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1351704Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1567:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1352694Z  1567 |         {[01;31m[KTranslator[m[K::translate("toggle_audio"), [this]() { on_toggle_audio_simple(); }}, // PHASE 1: RESTORE Audio Enable Toggle
2025-11-05T14:36:45.1353210Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1353932Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1568:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1354896Z  1568 |         {[01;31m[KTranslator[m[K::translate("audio_settings"), [this]() { on_audio_toggle(); }},
2025-11-05T14:36:45.1355669Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1356680Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1569:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1357699Z  1569 |         {[01;31m[KTranslator[m[K::translate("add_preset"), [this]() { on_add_preset_quick(); }}, // PHASE 4: RESTORE Preset system for drone database
2025-11-05T14:36:45.1367724Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1368744Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1570:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1369668Z  1570 |         {[01;31m[KTranslator[m[K::translate("manage_freq"), [this]() { on_manage_frequencies(); }},
2025-11-05T14:36:45.1370355Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1371121Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1571:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1371998Z  1571 |         {[01;31m[KTranslator[m[K::translate("create_db"), [this]() { on_create_new_database(); }},
2025-11-05T14:36:45.1372412Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1373141Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1572:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1374099Z  1572 |         {[01;31m[KTranslator[m[K::translate("advanced"), [this]() { on_advanced_settings(); }},
2025-11-05T14:36:45.1374514Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1375238Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1573:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1376657Z  1573 |         {[01;31m[KTranslator[m[K::translate("constant_settings"), [this]() { on_open_constant_settings(); }}, // New: Constant settings dialog
2025-11-05T14:36:45.1377619Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1378735Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1574:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1379640Z  1574 |         {[01;31m[KTranslator[m[K::translate("frequency_warning"), [this]() { on_frequency_warning(); }},
2025-11-05T14:36:45.1380075Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1380793Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1575:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1381724Z  1575 |         {[01;31m[KTranslator[m[K::translate("select_language"), [this]() { on_select_language(); }}, // Language selection
2025-11-05T14:36:45.1382186Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1382914Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1576:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1383738Z  1576 |         {[01;31m[KTranslator[m[K::translate("about_author"), [this]() { on_about(); }}
2025-11-05T14:36:45.1384118Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1385169Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1577:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-05T14:36:45.1386028Z  1577 |     }[01;31m[K)[m[K;
2025-11-05T14:36:45.1386266Z       |      [01;31m[K^[m[K
2025-11-05T14:36:45.1386818Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1387625Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1388927Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-05T14:36:45.1389733Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1390011Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1390712Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-05T14:36:45.1392241Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_select_language()[m[K':
2025-11-05T14:36:45.1393511Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1394716Z  1582 |         {[01;31m[KTranslator[m[K::translate("english"), [this]() { Translator::set_language(Language::ENGLISH); nav_.display_modal(Translator::translate("english"), "Language updated to English"); }},
2025-11-05T14:36:45.1395412Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1396101Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-05T14:36:45.1397111Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:55:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1398406Z  1582 |         {Translator::translate("english"), [this]() { [01;31m[KTranslator[m[K::set_language(Language::ENGLISH); nav_.display_modal(Translator::translate("english"), "Language updated to English"); }},
2025-11-05T14:36:45.1399151Z       |                                                       [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1399904Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:80:[m[K [01;31m[Kerror: [m[K'[01m[KLanguage[m[K' has not been declared
2025-11-05T14:36:45.1401074Z  1582 |         {Translator::translate("english"), [this]() { Translator::set_language([01;31m[KLanguage[m[K::ENGLISH); nav_.display_modal(Translator::translate("english"), "Language updated to English"); }},
2025-11-05T14:36:45.1402447Z       |                                                                                [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.1403536Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:119:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1404726Z  1582 |         {Translator::translate("english"), [this]() { Translator::set_language(Language::ENGLISH); nav_.display_modal([01;31m[KTranslator[m[K::translate("english"), "Language updated to English"); }},
2025-11-05T14:36:45.1405882Z       |                                                                                                                       [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1407406Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_select_language()[m[K':
2025-11-05T14:36:45.1408823Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1583:10:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1409941Z  1583 |         {[01;31m[KTranslator[m[K::translate("russian"), [this]() { Translator::set_language(Language::RUSSIAN); nav_.display_modal("Русский", "Язык изменен на русский"); }}
2025-11-05T14:36:45.1410549Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1411125Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-05T14:36:45.1412125Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1583:55:[m[K [01;31m[Kerror: [m[K'[01m[KTranslator[m[K' has not been declared
2025-11-05T14:36:45.1413192Z  1583 |         {Translator::translate("russian"), [this]() { [01;31m[KTranslator[m[K::set_language(Language::RUSSIAN); nav_.display_modal("Русский", "Язык изменен на русский"); }}
2025-11-05T14:36:45.1413854Z       |                                                       [01;31m[K^~~~~~~~~~[m[K
2025-11-05T14:36:45.1415541Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1583:80:[m[K [01;31m[Kerror: [m[K'[01m[KLanguage[m[K' has not been declared
2025-11-05T14:36:45.1417507Z  1583 |         {Translator::translate("russian"), [this]() { Translator::set_language([01;31m[KLanguage[m[K::RUSSIAN); nav_.display_modal("Русский", "Язык изменен на русский"); }}
2025-11-05T14:36:45.1418390Z       |                                                                                [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.1419415Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_select_language()[m[K':
2025-11-05T14:36:45.1422243Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1584:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-05T14:36:45.1423800Z  1584 |     }[01;31m[K)[m[K;
2025-11-05T14:36:45.1424233Z       |      [01;31m[K^[m[K
2025-11-05T14:36:45.1425224Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1426660Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1428960Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-05T14:36:45.1430440Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1430952Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1432145Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-05T14:36:45.1433885Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_audio_toggle()[m[K':
2025-11-05T14:36:45.1435264Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1608:26:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-05T14:36:45.1436104Z  1608 |         .audio_enabled = [01;31m[Kaudio_[m[K.is_audio_enabled(),
2025-11-05T14:36:45.1436463Z       |                          [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.1436758Z       |                          [32m[Kaudio[m[K
2025-11-05T14:36:45.1437740Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1611:15:[m[K [01;31m[Kerror: [m[K'[01m[KDroneAudioSettingsView[m[K' was not declared in this scope; did you mean '[01m[KDroneAudioSettings[m[K'?
2025-11-05T14:36:45.1438882Z  1611 |     nav_.push<[01;31m[KDroneAudioSettingsView[m[K>(audio_settings, audio_);
2025-11-05T14:36:45.1439294Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1439592Z       |               [32m[KDroneAudioSettings[m[K
2025-11-05T14:36:45.1440182Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-05T14:36:45.1441377Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1616:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_spectrum_settings[m[K' was not declared in this scope; did you mean '[01m[Kon_open_settings[m[K'?
2025-11-05T14:36:45.1442465Z  1616 |         {"Spectrum Mode", [this]() { [01;31m[Kon_spectrum_settings[m[K(); }},  // PHASE 3: Now connects restore set_spectrum_mode()
2025-11-05T14:36:45.1443173Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1443502Z       |                                      [32m[Kon_open_settings[m[K
2025-11-05T14:36:45.1444510Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_advanced_settings()[m[K':
2025-11-05T14:36:45.1446083Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1621:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-05T14:36:45.1446925Z  1621 |     }[01;31m[K)[m[K;
2025-11-05T14:36:45.1447293Z       |      [01;31m[K^[m[K
2025-11-05T14:36:45.1447857Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1448784Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1449940Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-05T14:36:45.1450732Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1451015Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1451703Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-05T14:36:45.1453027Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_system_status()[m[K':
2025-11-05T14:36:45.1454414Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1631:32:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-05T14:36:45.1455332Z  1631 |     const char* audio_status = [01;31m[Kaudio_[m[K.is_audio_enabled() ? "ENABLED" : "DISABLED";
2025-11-05T14:36:45.1455773Z       |                                [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.1456078Z       |                                [32m[Kaudio[m[K
2025-11-05T14:36:45.1457757Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1633:45:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.1459466Z  1633 |     const char* spectrum_status = hardware_.[01;31m[Kis_spectrum_streaming_active[m[K() ? "YES" : "NO";
2025-11-05T14:36:45.1459977Z       |                                             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1460983Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1462720Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:453:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.1463477Z   453 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-05T14:36:45.1463827Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1464844Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_performance_stats()[m[K':
2025-11-05T14:36:45.1467056Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1661:37:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has no member named '[01m[Kget_scan_cycles[m[K'; did you mean '[01m[Kuint32_t ui::external_app::enhanced_drone_analyzer::DroneScanner::scan_cycles_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.1468523Z  1661 |     uint32_t scan_cycles = scanner_.[01;31m[Kget_scan_cycles[m[K();
2025-11-05T14:36:45.1469174Z       |                                     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1470474Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1472195Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:383:14:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.1473362Z   383 |     uint32_t [01;36m[Kscan_cycles_[m[K = 0;
2025-11-05T14:36:45.1473802Z       |              [01;36m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1475370Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1668:33:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.1476667Z  1668 |             "Cycles completed: [01;35m[K%u[m[K\n"
2025-11-05T14:36:45.1476993Z       |                                [01;35m[K~^[m[K
2025-11-05T14:36:45.1477275Z       |                                 [01;35m[K|[m[K
2025-11-05T14:36:45.1477584Z       |                                 [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.1477885Z       |                                [32m[K%lu[m[K
2025-11-05T14:36:45.1478278Z ......
2025-11-05T14:36:45.1478498Z  1676 |             [32m[Kscan_cycles[m[K,
2025-11-05T14:36:45.1478792Z       |             [32m[K~~~~~~~~~~~[m[K          
2025-11-05T14:36:45.1479052Z       |             [32m[K|[m[K
2025-11-05T14:36:45.1479347Z       |             [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.1480529Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1670:33:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 6 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-05T14:36:45.1481540Z  1670 |             "Total detections: [01;35m[K%u[m[K\n"
2025-11-05T14:36:45.1481852Z       |                                [01;35m[K~^[m[K
2025-11-05T14:36:45.1482131Z       |                                 [01;35m[K|[m[K
2025-11-05T14:36:45.1482434Z       |                                 [01;35m[Kunsigned int[m[K
2025-11-05T14:36:45.1482734Z       |                                [32m[K%lu[m[K
2025-11-05T14:36:45.1482950Z ......
2025-11-05T14:36:45.1483197Z  1678 |             [32m[Kscanner_.get_total_detections()[m[K,
2025-11-05T14:36:45.1483520Z       |             [32m[K~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1483830Z       |                                          [32m[K|[m[K
2025-11-05T14:36:45.1484175Z       |                                          [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-05T14:36:45.1485196Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::show_debug_info()[m[K':
2025-11-05T14:36:45.1489249Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1708:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.1492092Z  1708 |             hardware_.[01;31m[Kis_spectrum_streaming_active[m[K() ? "YES" : "NO",
2025-11-05T14:36:45.1492591Z       |                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1493193Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1494275Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:453:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.1494980Z   453 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-05T14:36:45.1495324Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1496450Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1709:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kget_current_rssi[m[K'
2025-11-05T14:36:45.1497420Z  1709 |             hardware_.[01;31m[Kget_current_rssi[m[K(),
2025-11-05T14:36:45.1497745Z       |                       [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1499568Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1712:23:[m[K [01;31m[Kerror: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneHardwareController[m[K' has no member named '[01m[Kis_spectrum_streaming_active[m[K'; did you mean '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneHardwareController::spectrum_streaming_active_[m[K'? (not accessible from this context)
2025-11-05T14:36:45.1501107Z  1712 |             hardware_.[01;31m[Kis_spectrum_streaming_active[m[K() ? "STREAMING" : "IDLE");
2025-11-05T14:36:45.1501532Z       |                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1502129Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1503055Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:453:10:[m[K [01;36m[Knote: [m[Kdeclared private here
2025-11-05T14:36:45.1503725Z   453 |     bool [01;36m[Kspectrum_streaming_active_[m[K = false;
2025-11-05T14:36:45.1504063Z       |          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1505061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_manage_frequencies()[m[K':
2025-11-05T14:36:45.1506431Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1717:15:[m[K [01;31m[Kerror: [m[K'[01m[KDroneFrequencyManagerView[m[K' was not declared in this scope
2025-11-05T14:36:45.1506626Z  1717 |     nav_.push<[01;31m[KDroneFrequencyManagerView[m[K>();
2025-11-05T14:36:45.1506758Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1507543Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1717:42:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<<expression error> >()[m[K'
2025-11-05T14:36:45.1507736Z  1717 |     nav_.push<DroneFrequencyManagerView>([01;31m[K)[m[K;
2025-11-05T14:36:45.1507871Z       |                                          [01;31m[K^[m[K
2025-11-05T14:36:45.1508397Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1508903Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1509686Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class T, class ... Args> T* ui::NavigationView::push(Args&& ...)[m[K'
2025-11-05T14:36:45.1509825Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1509937Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1510504Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.1511149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1717:42:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-05T14:36:45.1511346Z  1717 |     nav_.push<DroneFrequencyManagerView>([01;31m[K)[m[K;
2025-11-05T14:36:45.1511487Z       |                                          [01;31m[K^[m[K
2025-11-05T14:36:45.1512961Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_hardware_control_menu()[m[K':
2025-11-05T14:36:45.1514618Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1756:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-05T14:36:45.1514773Z  1756 |     }[01;31m[K)[m[K;
2025-11-05T14:36:45.1514872Z       |      [01;31m[K^[m[K
2025-11-05T14:36:45.1515317Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1515690Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1516478Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-05T14:36:45.1516624Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1516726Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1517274Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-05T14:36:45.1518207Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_save_settings()[m[K':
2025-11-05T14:36:45.1518850Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1760:9:[m[K [01;31m[Kerror: [m[K'[01m[KDroneAnalyzerSettingsManager[m[K' has not been declared
2025-11-05T14:36:45.1519148Z  1760 |     if ([01;31m[KDroneAnalyzerSettingsManager[m[K::save_settings(settings_)) {
2025-11-05T14:36:45.1519290Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1520079Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_load_settings()[m[K':
2025-11-05T14:36:45.1520713Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1768:9:[m[K [01;31m[Kerror: [m[K'[01m[KDroneAnalyzerSettingsManager[m[K' has not been declared
2025-11-05T14:36:45.1521005Z  1768 |     if ([01;31m[KDroneAnalyzerSettingsManager[m[K::load_settings(settings_)) {
2025-11-05T14:36:45.1521135Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1522126Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_set_bandwidth_config()[m[K':
2025-11-05T14:36:45.1523441Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1807:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-05T14:36:45.1523649Z  1807 |     }[01;31m[K)[m[K;
2025-11-05T14:36:45.1523821Z       |      [01;31m[K^[m[K
2025-11-05T14:36:45.1524614Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1525556Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1526638Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-05T14:36:45.1526801Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1526906Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1527459Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-05T14:36:45.1528461Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_set_center_freq_config()[m[K':
2025-11-05T14:36:45.1529344Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1816:6:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<ui::MenuView>(<brace-enclosed initializer list>)[m[K'
2025-11-05T14:36:45.1529456Z  1816 |     }[01;31m[K)[m[K;
2025-11-05T14:36:45.1529560Z       |      [01;31m[K^[m[K
2025-11-05T14:36:45.1529987Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1530363Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1531130Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KT* ui::NavigationView::push(Args&& ...) [with T = ui::MenuView; Args = {}][m[K'
2025-11-05T14:36:45.1531270Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1531379Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1531928Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-05T14:36:45.1532708Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_about()[m[K':
2025-11-05T14:36:45.1533331Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1841:15:[m[K [01;31m[Kerror: [m[K'[01m[KAuthorContactView[m[K' was not declared in this scope
2025-11-05T14:36:45.1533506Z  1841 |     nav_.push<[01;31m[KAuthorContactView[m[K>();
2025-11-05T14:36:45.1533633Z       |               [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1534420Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1841:34:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<<expression error> >()[m[K'
2025-11-05T14:36:45.1534732Z  1841 |     nav_.push<AuthorContactView>([01;31m[K)[m[K;
2025-11-05T14:36:45.1534863Z       |                                  [01;31m[K^[m[K
2025-11-05T14:36:45.1535289Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1535666Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1536446Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class T, class ... Args> T* ui::NavigationView::push(Args&& ...)[m[K'
2025-11-05T14:36:45.1536585Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1536805Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1537375Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.1537916Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1841:34:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-05T14:36:45.1538182Z  1841 |     nav_.push<AuthorContactView>([01;31m[K)[m[K;
2025-11-05T14:36:45.1538317Z       |                                  [01;31m[K^[m[K
2025-11-05T14:36:45.1538727Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-05T14:36:45.1539654Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1844:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_spectrum_settings()[m[K'
2025-11-05T14:36:45.1539887Z  1844 | void [01;31m[KDroneUIController[m[K::on_spectrum_settings() {
2025-11-05T14:36:45.1540007Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1540890Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1844:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_spectrum_settings()[m[K'
2025-11-05T14:36:45.1541307Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1542061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:643:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneUIController[m[K' defined here
2025-11-05T14:36:45.1542210Z   643 | class [01;36m[KDroneUIController[m[K {
2025-11-05T14:36:45.1542329Z       |       [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1558617Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_open_constant_settings()[m[K':
2025-11-05T14:36:45.1559307Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1869:12:[m[K [01;31m[Kerror: [m[K'[01m[KConstantSettingsManager[m[K' does not name a type
2025-11-05T14:36:45.1559509Z  1869 |     static [01;31m[KConstantSettingsManager[m[K manager;
2025-11-05T14:36:45.1559644Z       |            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1590158Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1870:15:[m[K [01;31m[Kerror: [m[K'[01m[KConstantSettingsView[m[K' was not declared in this scope
2025-11-05T14:36:45.1590615Z  1870 |     nav_.push<[01;31m[KConstantSettingsView[m[K>(nav_);
2025-11-05T14:36:45.1590867Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1592134Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1870:41:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kui::NavigationView::push<<expression error> >(ui::NavigationView&)[m[K'
2025-11-05T14:36:45.1592512Z  1870 |     nav_.push<ConstantSettingsView>(nav_[01;31m[K)[m[K;
2025-11-05T14:36:45.1592657Z       |                                         [01;31m[K^[m[K
2025-11-05T14:36:45.1593095Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:44[m[K,
2025-11-05T14:36:45.1593477Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1594913Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class T, class ... Args> T* ui::NavigationView::push(Args&& ...)[m[K'
2025-11-05T14:36:45.1595232Z   110 |     T* [01;36m[Kpush[m[K(Args&&... args) {
2025-11-05T14:36:45.1595435Z       |        [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1596489Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../ui_navigation.hpp:110:8:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.1597497Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1870:41:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-05T14:36:45.1597856Z  1870 |     nav_.push<ConstantSettingsView>(nav_[01;31m[K)[m[K;
2025-11-05T14:36:45.1598325Z       |                                         [01;31m[K^[m[K
2025-11-05T14:36:45.1599625Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_add_preset_quick()[m[K':
2025-11-05T14:36:45.1600250Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1875:24:[m[K [01;31m[Kerror: [m[K'[01m[KDroneFrequencyPresets[m[K' has not been declared
2025-11-05T14:36:45.1600552Z  1875 |     auto all_presets = [01;31m[KDroneFrequencyPresets[m[K::get_all_presets();
2025-11-05T14:36:45.1600701Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1601317Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1884:5:[m[K [01;31m[Kerror: [m[K'[01m[KDronePresetSelector[m[K' has not been declared
2025-11-05T14:36:45.1601551Z  1884 |     [01;31m[KDronePresetSelector[m[K::show_preset_menu(nav_,
2025-11-05T14:36:45.1601675Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1602586Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::add_preset_to_scanner(const DronePreset&)[m[K':
2025-11-05T14:36:45.1603352Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1911:33:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KFreqmanDB::FreqmanDB(const char [7])[m[K'
2025-11-05T14:36:45.1603601Z  1911 |     FreqmanDB preset_db("DRONES"[01;31m[K)[m[K; // Use same file as scanner
2025-11-05T14:36:45.1603730Z       |                                 [01;31m[K^[m[K
2025-11-05T14:36:45.1604169Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:32[m[K,
2025-11-05T14:36:45.1604548Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1605208Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kconstexpr FreqmanDB::FreqmanDB()[m[K'
2025-11-05T14:36:45.1605342Z   200 | class [01;36m[KFreqmanDB[m[K {
2025-11-05T14:36:45.1605639Z       |       [01;36m[K^~~~~~~~~[m[K
2025-11-05T14:36:45.1606178Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 1 provided
2025-11-05T14:36:45.1606823Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[KFreqmanDB::FreqmanDB(FreqmanDB&&)[m[K'
2025-11-05T14:36:45.1607656Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:200:7:[m[K [01;36m[Knote: [m[K  no known conversion for argument 1 from '[01m[Kconst char [7][m[K' to '[01m[KFreqmanDB&&[m[K'
2025-11-05T14:36:45.1609326Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1912:24:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[KFreqmanDB::open()[m[K'
2025-11-05T14:36:45.1609593Z  1912 |     if (preset_db.open([01;31m[K)[m[K) {
2025-11-05T14:36:45.1609725Z       |                        [01;31m[K^[m[K
2025-11-05T14:36:45.1610156Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:32[m[K,
2025-11-05T14:36:45.1610537Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.1611792Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:241:10:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kbool FreqmanDB::open(const std::filesystem::path&, bool)[m[K'
2025-11-05T14:36:45.1612314Z   241 |     bool [01;36m[Kopen[m[K(const std::filesystem::path& path, bool create = false);
2025-11-05T14:36:45.1612516Z       |          [01;36m[K^~~~[m[K
2025-11-05T14:36:45.1613200Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../freqman_db.hpp:241:10:[m[K [01;36m[Knote: [m[K  candidate expects 2 arguments, 0 provided
2025-11-05T14:36:45.1613886Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1922:9:[m[K [01;31m[Kerror: [m[K'[01m[Kfreqman_entry[m[K' has no non-static data member named '[01m[Ktonal[m[K'
2025-11-05T14:36:45.1614000Z  1922 |         [01;31m[K}[m[K;
2025-11-05T14:36:45.1614102Z       |         [01;31m[K^[m[K
2025-11-05T14:36:45.1614778Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1924:19:[m[K [01;31m[Kerror: [m[K'[01m[Kclass FreqmanDB[m[K' has no member named '[01m[Ksave[m[K'
2025-11-05T14:36:45.1614922Z  1924 |         preset_db.[01;31m[Ksave[m[K();
2025-11-05T14:36:45.1615040Z       |                   [01;31m[K^~~~[m[K
2025-11-05T14:36:45.1615880Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneUIController::on_toggle_audio_simple()[m[K':
2025-11-05T14:36:45.1616569Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1937:5:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-05T14:36:45.1616705Z  1937 |     [01;31m[Kaudio_[m[K.toggle_audio();
2025-11-05T14:36:45.1616806Z       |     [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.1616913Z       |     [32m[Kaudio[m[K
2025-11-05T14:36:45.1617904Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)[m[K':
2025-11-05T14:36:45.1619036Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1948:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' does not have any field named '[01m[Khardware_[m[K'
2025-11-05T14:36:45.1619463Z  1948 |       [01;31m[Khardware_[m[K(std::make_unique<DroneHardwareController>()),
2025-11-05T14:36:45.1619573Z       |       [01;31m[K^~~~~~~~~[m[K
2025-11-05T14:36:45.1665502Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1949:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' does not have any field named '[01m[Kscanner_[m[K'
2025-11-05T14:36:45.1665804Z  1949 |       [01;31m[Kscanner_[m[K(std::make_unique<DroneScanner>()),
2025-11-05T14:36:45.1665942Z       |       [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.1710070Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1950:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' does not have any field named '[01m[Kaudio_[m[K'
2025-11-05T14:36:45.1710634Z  1950 |       [01;31m[Kaudio_[m[K(std::make_unique<AudioManager>()),
2025-11-05T14:36:45.1710865Z       |       [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.1759986Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' does not have any field named '[01m[Kui_controller_[m[K'
2025-11-05T14:36:45.1760466Z  1951 |       [01;31m[Kui_controller_[m[K(std::make_unique<DroneUIController>(nav, *hardware_, *scanner_, *audio_)),
2025-11-05T14:36:45.1760614Z       |       [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1782568Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:64:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-05T14:36:45.1783380Z  1951 |       ui_controller_(std::make_unique<DroneUIController>(nav, *[01;31m[Khardware_[m[K, *scanner_, *audio_)),
2025-11-05T14:36:45.1784106Z       |                                                                [01;31m[K^~~~~~~~~[m[K
2025-11-05T14:36:45.1798289Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:76:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-05T14:36:45.1798971Z  1951 |       ui_controller_(std::make_unique<DroneUIController>(nav, *hardware_, *[01;31m[Kscanner_[m[K, *audio_)),
2025-11-05T14:36:45.1799302Z       |                                                                            [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.1807725Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:87:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-05T14:36:45.1808613Z  1951 |       ui_controller_(std::make_unique<DroneUIController>(nav, *hardware_, *scanner_, *[01;31m[Kaudio_[m[K)),
2025-11-05T14:36:45.1808951Z       |                                                                                       [01;31m[K^~~~~~[m[K
2025-11-05T14:36:45.1809261Z       |                                                                                       [32m[Kaudio[m[K
2025-11-05T14:36:45.1811262Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1952:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' does not have any field named '[01m[Kdisplay_controller_[m[K'
2025-11-05T14:36:45.1811799Z  1952 |       [01;31m[Kdisplay_controller_[m[K(std::make_unique<DroneDisplayController>(nav)),
2025-11-05T14:36:45.1812011Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1813921Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1953:7:[m[K [01;31m[Kerror: [m[Kclass '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' does not have any field named '[01m[Kscanning_coordinator_[m[K'
2025-11-05T14:36:45.1815065Z  1953 |       [01;31m[Kscanning_coordinator_[m[K(std::make_unique<ScanningCoordinator>(nav, *hardware_, *scanner_, *display_controller_, *audio_))
2025-11-05T14:36:45.1815276Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1842846Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1953:96:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-05T14:36:45.1844122Z  1953 |       scanning_coordinator_(std::make_unique<ScanningCoordinator>(nav, *hardware_, *scanner_, *[01;31m[Kdisplay_controller_[m[K, *audio_))
2025-11-05T14:36:45.1844451Z       |                                                                                                [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1846094Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1946:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::button_menu_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1846659Z  1946 | [01;35m[KEnhancedDroneSpectrumAnalyzerView[m[K::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
2025-11-05T14:36:45.1846836Z       | [01;35m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1848666Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1946:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::iq_phase_calibration_value_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-05T14:36:45.1869571Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1959:27:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-05T14:36:45.1869920Z  1959 |         loaded_settings = [01;31m[Kui_controller_[m[K->settings();
2025-11-05T14:36:45.1870079Z       |                           [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1894447Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1962:9:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-05T14:36:45.1894727Z  1962 |         [01;31m[Kui_controller_[m[K->settings() = loaded_settings;
2025-11-05T14:36:45.1894876Z       |         [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1914912Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1964:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope; did you mean '[01m[KScanningCoordinator[m[K'?
2025-11-05T14:36:45.1915429Z  1964 |     [01;31m[Kscanning_coordinator_[m[K->update_runtime_parameters(loaded_settings);
2025-11-05T14:36:45.1915633Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1915855Z       |     [32m[KScanningCoordinator[m[K
2025-11-05T14:36:45.1943978Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1967:5:[m[K [01;31m[Kerror: [m[K'[01m[Kinitialize_modern_layout[m[K' was not declared in this scope
2025-11-05T14:36:45.1944252Z  1967 |     [01;31m[Kinitialize_modern_layout[m[K();
2025-11-05T14:36:45.1944470Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1965867Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1969:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-05T14:36:45.1966133Z  1969 |     [01;31m[Kbutton_start_[m[K.on_select = [this](Button&) {
2025-11-05T14:36:45.1966533Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.1994057Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-05T14:36:45.1994957Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1970:9:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_start_stop_button[m[K' was not declared in this scope
2025-11-05T14:36:45.1995169Z  1970 |         [01;31m[Khandle_start_stop_button[m[K();
2025-11-05T14:36:45.1995331Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2023510Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-05T14:36:45.2024627Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1973:9:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_menu_button[m[K' was not declared in this scope
2025-11-05T14:36:45.2024832Z  1973 |         [01;31m[Khandle_menu_button[m[K();
2025-11-05T14:36:45.2024990Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2061262Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)[m[K':
2025-11-05T14:36:45.2062138Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1976:5:[m[K [01;31m[Kerror: [m[K'[01m[Kfield_scanning_mode_[m[K' was not declared in this scope
2025-11-05T14:36:45.2062424Z  1976 |     [01;31m[Kfield_scanning_mode_[m[K.on_change = [this](size_t index) {
2025-11-05T14:36:45.2062576Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2063116Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-05T14:36:45.2063835Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1978:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' is not captured
2025-11-05T14:36:45.2064039Z  1978 |         [01;31m[Kscanner_[m[K->set_scanning_mode(mode);
2025-11-05T14:36:45.2064165Z       |         [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.2064855Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1976:43:[m[K [01;36m[Knote: [m[Kthe lambda has no capture-default
2025-11-05T14:36:45.2065138Z  1976 |     field_scanning_mode_.on_change = [this[01;36m[K][m[K(size_t index) {
2025-11-05T14:36:45.2065299Z       |                                           [01;36m[K^[m[K
2025-11-05T14:36:45.2066050Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:76:[m[K [01;36m[Knote: [m[K'[01m[K<typeprefixerror>scanner_[m[K' declared here
2025-11-05T14:36:45.2066505Z  1951 |       ui_controller_(std::make_unique<DroneUIController>(nav, *hardware_, *[01;36m[Kscanner_[m[K, *audio_)),
2025-11-05T14:36:45.2066710Z       |                                                                            [01;36m[K^~~~~~~~[m[K
2025-11-05T14:36:45.2067431Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1979:9:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' is not captured
2025-11-05T14:36:45.2067799Z  1979 |         [01;31m[Kdisplay_controller_[m[K->set_scanning_status(ui_controller_->is_scanning(),
2025-11-05T14:36:45.2067939Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2068745Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1976:43:[m[K [01;36m[Knote: [m[Kthe lambda has no capture-default
2025-11-05T14:36:45.2069032Z  1976 |     field_scanning_mode_.on_change = [this[01;36m[K][m[K(size_t index) {
2025-11-05T14:36:45.2069391Z       |                                           [01;36m[K^[m[K
2025-11-05T14:36:45.2070173Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1953:96:[m[K [01;36m[Knote: [m[K'[01m[K<typeprefixerror>display_controller_[m[K' declared here
2025-11-05T14:36:45.2070753Z  1953 |       scanning_coordinator_(std::make_unique<ScanningCoordinator>(nav, *hardware_, *scanner_, *[01;36m[Kdisplay_controller_[m[K, *audio_))
2025-11-05T14:36:45.2070992Z       |                                                                                                [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2085616Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1979:50:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-05T14:36:45.2085958Z  1979 |         display_controller_->set_scanning_status([01;31m[Kui_controller_[m[K->is_scanning(),
2025-11-05T14:36:45.2086122Z       |                                                  [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2086704Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1980:48:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' is not captured
2025-11-05T14:36:45.2086902Z  1980 |                                                [01;31m[Kscanner_[m[K->scanning_mode_name());
2025-11-05T14:36:45.2087085Z       |                                                [01;31m[K^~~~~~~~[m[K
2025-11-05T14:36:45.2087628Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1976:43:[m[K [01;36m[Knote: [m[Kthe lambda has no capture-default
2025-11-05T14:36:45.2087866Z  1976 |     field_scanning_mode_.on_change = [this[01;36m[K][m[K(size_t index) {
2025-11-05T14:36:45.2088002Z       |                                           [01;36m[K^[m[K
2025-11-05T14:36:45.2088761Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:76:[m[K [01;36m[Knote: [m[K'[01m[K<typeprefixerror>scanner_[m[K' declared here
2025-11-05T14:36:45.2089130Z  1951 |       ui_controller_(std::make_unique<DroneUIController>(nav, *hardware_, *[01;36m[Kscanner_[m[K, *audio_)),
2025-11-05T14:36:45.2089298Z       |                                                                            [01;36m[K^~~~~~~~[m[K
2025-11-05T14:36:45.2115321Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1982:9:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_modern_layout[m[K' was not declared in this scope
2025-11-05T14:36:45.2115525Z  1982 |         [01;31m[Kupdate_modern_layout[m[K();
2025-11-05T14:36:45.2115682Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2136497Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::EnhancedDroneSpectrumAnalyzerView(ui::NavigationView&)[m[K':
2025-11-05T14:36:45.2137317Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1989:15:[m[K [01;31m[Kerror: [m[K'[01m[Ksmart_header_[m[K' was not declared in this scope
2025-11-05T14:36:45.2137535Z  1989 |     add_child([01;31m[Ksmart_header_[m[K.get());
2025-11-05T14:36:45.2137695Z       |               [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2152815Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1990:15:[m[K [01;31m[Kerror: [m[K'[01m[Kstatus_bar_[m[K' was not declared in this scope
2025-11-05T14:36:45.2152989Z  1990 |     add_child([01;31m[Kstatus_bar_[m[K.get());
2025-11-05T14:36:45.2153122Z       |               [01;31m[K^~~~~~~~~~~[m[K
2025-11-05T14:36:45.2170171Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1991:23:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_cards_[m[K' was not declared in this scope; did you mean '[01m[KThreatCard[m[K'?
2025-11-05T14:36:45.2170555Z  1991 |     for (auto& card : [01;31m[Kthreat_cards_[m[K) {
2025-11-05T14:36:45.2170694Z       |                       [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2170829Z       |                       [32m[KThreatCard[m[K
2025-11-05T14:36:45.2201113Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2000:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_modern_layout[m[K' was not declared in this scope
2025-11-05T14:36:45.2201313Z  2000 |     [01;31m[Kupdate_modern_layout[m[K();
2025-11-05T14:36:45.2201477Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2225609Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::focus()[m[K':
2025-11-05T14:36:45.2226483Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2004:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-05T14:36:45.2226661Z  2004 |     [01;31m[Kbutton_start_[m[K.focus();
2025-11-05T14:36:45.2226800Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2257056Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual bool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_key(ui::KeyEvent)[m[K':
2025-11-05T14:36:45.2259447Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2014:13:[m[K [01;31m[Kerror: [m[K'[01m[Kstop_scanning_thread[m[K' was not declared in this scope
2025-11-05T14:36:45.2259683Z  2014 |             [01;31m[Kstop_scanning_thread[m[K();
2025-11-05T14:36:45.2259851Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2260383Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-05T14:36:45.2261614Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2027:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_show()[m[K'
2025-11-05T14:36:45.2261981Z  2027 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_show() {
2025-11-05T14:36:45.2262146Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2262669Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2263875Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:730:6:[m[K [01;36m[Knote: [m[K'[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_show()[m[K' previously defined here
2025-11-05T14:36:45.2264040Z   730 | void [01;36m[Kon_show[m[K() override {
2025-11-05T14:36:45.2264169Z       |      [01;36m[K^~~~~~~[m[K
2025-11-05T14:36:45.2289751Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvirtual void ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::on_hide()[m[K':
2025-11-05T14:36:45.2290577Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2034:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstop_scanning_thread[m[K' was not declared in this scope
2025-11-05T14:36:45.2290766Z  2034 |     [01;31m[Kstop_scanning_thread[m[K();
2025-11-05T14:36:45.2290930Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2302817Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2035:5:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-05T14:36:45.2303675Z  2035 |     [01;31m[Khardware_[m[K->on_hardware_hide();
2025-11-05T14:36:45.2304028Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-05T14:36:45.2305001Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-05T14:36:45.2307133Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2039:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::start_scanning_thread()[m[K'
2025-11-05T14:36:45.2307773Z  2039 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::start_scanning_thread() {
2025-11-05T14:36:45.2308527Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2310436Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2039:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::start_scanning_thread()[m[K'
2025-11-05T14:36:45.2311356Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2313045Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' defined here
2025-11-05T14:36:45.2313512Z   701 | class [01;36m[KEnhancedDroneSpectrumAnalyzerView[m[K : public View {
2025-11-05T14:36:45.2313745Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2315294Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2044:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread()[m[K'
2025-11-05T14:36:45.2315766Z  2044 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::stop_scanning_thread() {
2025-11-05T14:36:45.2315976Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2317443Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2044:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::stop_scanning_thread()[m[K'
2025-11-05T14:36:45.2318377Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2319996Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' defined here
2025-11-05T14:36:45.2320483Z   701 | class [01;36m[KEnhancedDroneSpectrumAnalyzerView[m[K : public View {
2025-11-05T14:36:45.2320746Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2322654Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2049:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kbool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button()[m[K'
2025-11-05T14:36:45.2323294Z  2049 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_start_stop_button() {
2025-11-05T14:36:45.2323559Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2325460Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2049:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kbool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_start_stop_button()[m[K'
2025-11-05T14:36:45.2326604Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2328446Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' defined here
2025-11-05T14:36:45.2328954Z   701 | class [01;36m[KEnhancedDroneSpectrumAnalyzerView[m[K : public View {
2025-11-05T14:36:45.2329200Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2331336Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2060:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kbool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_menu_button()[m[K'
2025-11-05T14:36:45.2332022Z  2060 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_menu_button() {
2025-11-05T14:36:45.2332322Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2334208Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2060:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kbool ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_menu_button()[m[K'
2025-11-05T14:36:45.2335097Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2336813Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' defined here
2025-11-05T14:36:45.2337326Z   701 | class [01;36m[KEnhancedDroneSpectrumAnalyzerView[m[K : public View {
2025-11-05T14:36:45.2337640Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2339845Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2066:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout()[m[K'
2025-11-05T14:36:45.2340563Z  2066 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::initialize_modern_layout() {
2025-11-05T14:36:45.2340835Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2342775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2066:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::initialize_modern_layout()[m[K'
2025-11-05T14:36:45.2343654Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2345357Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' defined here
2025-11-05T14:36:45.2345859Z   701 | class [01;36m[KEnhancedDroneSpectrumAnalyzerView[m[K : public View {
2025-11-05T14:36:45.2346141Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2348308Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2082:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::update_modern_layout()[m[K'
2025-11-05T14:36:45.2349027Z  2082 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::update_modern_layout() {
2025-11-05T14:36:45.2349324Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2351519Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2082:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::update_modern_layout()[m[K'
2025-11-05T14:36:45.2352381Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2354116Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' defined here
2025-11-05T14:36:45.2354663Z   701 | class [01;36m[KEnhancedDroneSpectrumAnalyzerView[m[K : public View {
2025-11-05T14:36:45.2355212Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2357201Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2087:6:[m[K [01;31m[Kerror: [m[Kno declaration matches '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()[m[K'
2025-11-05T14:36:45.2357885Z  2087 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_scanner_update() {
2025-11-05T14:36:45.2358354Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2360287Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2087:6:[m[K [01;36m[Knote: [m[Kno functions named '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView::handle_scanner_update()[m[K'
2025-11-05T14:36:45.2361174Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2362874Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:701:7:[m[K [01;36m[Knote: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::EnhancedDroneSpectrumAnalyzerView[m[K' defined here
2025-11-05T14:36:45.2363392Z   701 | class [01;36m[KEnhancedDroneSpectrumAnalyzerView[m[K : public View {
2025-11-05T14:36:45.2363670Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2365610Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2179:1:[m[K [01;31m[Kerror: [m[Kdefinition of explicitly-defaulted '[01m[Kui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()[m[K'
2025-11-05T14:36:45.2366104Z  2179 | [01;31m[KLoadingScreenView[m[K::~LoadingScreenView() {
2025-11-05T14:36:45.2366321Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2367152Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2369171Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:752:5:[m[K [01;36m[Knote: [m[K'[01m[Kvirtual ui::external_app::enhanced_drone_analyzer::LoadingScreenView::~LoadingScreenView()[m[K' explicitly defaulted here
2025-11-05T14:36:45.2369525Z   752 |     [01;36m[K~[m[KLoadingScreenView() = default;
2025-11-05T14:36:45.2369730Z       |     [01;36m[K^[m[K
2025-11-05T14:36:45.2370869Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2193:40:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-05T14:36:45.2371247Z  2193 |                                        [01;31m[KDroneScanner[m[K& scanner,
2025-11-05T14:36:45.2371548Z       |                                        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2372771Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2194:40:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-05T14:36:45.2373512Z  2194 |                                        [01;31m[KDroneDisplayController[m[K& display_controller,
2025-11-05T14:36:45.2373802Z       |                                        [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2374998Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2195:70:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2375393Z  2195 |                                        AudioManager& audio_controller[01;31m[K)[m[K
2025-11-05T14:36:45.2375705Z       |                                                                      [01;31m[K^[m[K
2025-11-05T14:36:45.2376690Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2377917Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2378419Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2378656Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2379882Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2207:43:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2380315Z  2207 | ScanningCoordinator::~ScanningCoordinator([01;31m[K)[m[K {
2025-11-05T14:36:45.2380572Z       |                                           [01;31m[K^[m[K
2025-11-05T14:36:45.2381374Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2382591Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2382892Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2383119Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2384369Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2211:54:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2384858Z  2211 | void ScanningCoordinator::start_coordinated_scanning([01;31m[K)[m[K {
2025-11-05T14:36:45.2385136Z       |                                                      [01;31m[K^[m[K
2025-11-05T14:36:45.2385950Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2387119Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2387412Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2387648Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2389050Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2224:53:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2389535Z  2224 | void ScanningCoordinator::stop_coordinated_scanning([01;31m[K)[m[K {
2025-11-05T14:36:45.2389816Z       |                                                     [01;31m[K^[m[K
2025-11-05T14:36:45.2390627Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2391854Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2392169Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2392755Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2394031Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2234:62:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2394582Z  2234 | msg_t ScanningCoordinator::scanning_thread_function(void* arg[01;31m[K)[m[K {
2025-11-05T14:36:45.2394876Z       |                                                              [01;31m[K^[m[K
2025-11-05T14:36:45.2395705Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2397172Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2397504Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2397733Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2399250Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2239:56:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2399820Z  2239 | msg_t ScanningCoordinator::coordinated_scanning_thread([01;31m[K)[m[K {
2025-11-05T14:36:45.2400126Z       |                                                        [01;31m[K^[m[K
2025-11-05T14:36:45.2400947Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2402199Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2402503Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2402733Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2404027Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2263:90:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2404835Z  2263 | void ScanningCoordinator::update_runtime_parameters(const DroneAnalyzerSettings& settings[01;31m[K)[m[K {
2025-11-05T14:36:45.2405219Z       |                                                                                          [01;31m[K^[m[K
2025-11-05T14:36:45.2406042Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2407302Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2407622Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2407813Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.2409167Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2267:74:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2409831Z  2267 | void ScanningCoordinator::show_session_summary(const std::string& summary[01;31m[K)[m[K {
2025-11-05T14:36:45.2410163Z       |                                                                          [01;31m[K^[m[K
2025-11-05T14:36:45.2410986Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.2412321Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:50:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass ScanningCoordinator[m[K'
2025-11-05T14:36:45.2412617Z    50 | class [01;36m[KScanningCoordinator[m[K;
2025-11-05T14:36:45.2413115Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-05T14:36:45.3041658Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3043235Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3044680Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3046774Z /havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-05T14:36:45.3049246Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3051237Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:26:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kbegin(const char* const&)[m[K'
2025-11-05T14:36:45.3052712Z    89 |         : path{[01;31m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-05T14:36:45.3053358Z       |                [01;31m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3054289Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-05T14:36:45.3055460Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3056737Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3058453Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3060731Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::begin(std::initializer_list<_Tp>)[m[K'
2025-11-05T14:36:45.3062296Z    89 |     [01;36m[Kbegin[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-05T14:36:45.3062924Z       |     [01;36m[K^~~~~[m[K
2025-11-05T14:36:45.3064165Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3065788Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3067291Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3068966Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3071127Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-05T14:36:45.3072467Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-05T14:36:45.3073139Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3074053Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3075431Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3076928Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3079265Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&)[m[K'
2025-11-05T14:36:45.3081154Z    48 |     [01;36m[Kbegin[m[K(_Container& __cont) -> decltype(__cont.begin())
2025-11-05T14:36:45.3081818Z       |     [01;36m[K^~~~~[m[K
2025-11-05T14:36:45.3082969Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3085415Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&) [with _Container = const char* const][m[K':
2025-11-05T14:36:45.3088519Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-05T14:36:45.3090507Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3092569Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:50:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3094236Z    48 |     begin(_Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-05T14:36:45.3095006Z       |                                           [01;31m[K~~~~~~~^~~~~[m[K
2025-11-05T14:36:45.3096557Z /havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-05T14:36:45.3098795Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3101286Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&)[m[K'
2025-11-05T14:36:45.3103062Z    58 |     [01;36m[Kbegin[m[K(const _Container& __cont) -> decltype(__cont.begin())
2025-11-05T14:36:45.3103748Z       |     [01;36m[K^~~~~[m[K
2025-11-05T14:36:45.3104988Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3107456Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&) [with _Container = const char*][m[K':
2025-11-05T14:36:45.3110318Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-05T14:36:45.3112382Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3114625Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:56:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3116321Z    58 |     begin(const _Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-05T14:36:45.3117151Z       |                                                 [01;31m[K~~~~~~~^~~~~[m[K
2025-11-05T14:36:45.3118999Z /havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-05T14:36:45.3120970Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3123227Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::begin(_Tp (&)[_Nm])[m[K'
2025-11-05T14:36:45.3124921Z    87 |     [01;36m[Kbegin[m[K(_Tp (&__arr)[_Nm])
2025-11-05T14:36:45.3125464Z       |     [01;36m[K^~~~~[m[K
2025-11-05T14:36:45.3126652Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3128457Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3129970Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3131628Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3133556Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3135115Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-05T14:36:45.3135799Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3136700Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3138040Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3139719Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3141797Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::begin(std::valarray<_Tp>&)[m[K'
2025-11-05T14:36:45.3143275Z   104 |   template<typename _Tp> _Tp* [01;36m[Kbegin[m[K(valarray<_Tp>&);
2025-11-05T14:36:45.3143992Z       |                               [01;36m[K^~~~~[m[K
2025-11-05T14:36:45.3145290Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3147022Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3148743Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3150199Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3152288Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3153886Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-05T14:36:45.3154561Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3155461Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3156777Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3158348Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3160471Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::begin(const std::valarray<_Tp>&)[m[K'
2025-11-05T14:36:45.3162123Z   105 |   template<typename _Tp> const _Tp* [01;36m[Kbegin[m[K(const valarray<_Tp>&);
2025-11-05T14:36:45.3163245Z       |                                     [01;36m[K^~~~~[m[K
2025-11-05T14:36:45.3164538Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3166176Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3167707Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3169476Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3171961Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3173554Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-05T14:36:45.3174222Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3175698Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:44:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kend(const char* const&)[m[K'
2025-11-05T14:36:45.3177208Z    89 |         : path{std::begin(source), [01;31m[Kstd::end(source)[m[K} {
2025-11-05T14:36:45.3177943Z       |                                    [01;31m[K~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3179382Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-05T14:36:45.3180221Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3181079Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3182625Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3184331Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::end(std::initializer_list<_Tp>)[m[K'
2025-11-05T14:36:45.3185315Z    99 |     [01;36m[Kend[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-05T14:36:45.3185691Z       |     [01;36m[K^~~[m[K
2025-11-05T14:36:45.3186479Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3187576Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3188732Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3189687Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3191063Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-05T14:36:45.3192075Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-05T14:36:45.3192529Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3193103Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3193969Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3194739Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3196145Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&)[m[K'
2025-11-05T14:36:45.3196980Z    68 |     [01;36m[Kend[m[K(_Container& __cont) -> decltype(__cont.end())
2025-11-05T14:36:45.3197298Z       |     [01;36m[K^~~[m[K
2025-11-05T14:36:45.3197936Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3199498Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&) [with _Container = const char* const][m[K':
2025-11-05T14:36:45.3200916Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-05T14:36:45.3201954Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3203087Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:48:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3203909Z    68 |     end(_Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-05T14:36:45.3204290Z       |                                         [01;31m[K~~~~~~~^~~[m[K
2025-11-05T14:36:45.3205131Z /havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-05T14:36:45.3206154Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3207382Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&)[m[K'
2025-11-05T14:36:45.3208346Z    78 |     [01;36m[Kend[m[K(const _Container& __cont) -> decltype(__cont.end())
2025-11-05T14:36:45.3208686Z       |     [01;36m[K^~~[m[K
2025-11-05T14:36:45.3209309Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3210596Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&) [with _Container = const char*][m[K':
2025-11-05T14:36:45.3211968Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-05T14:36:45.3212987Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3214121Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:54:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3215203Z    78 |     end(const _Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-05T14:36:45.3215601Z       |                                               [01;31m[K~~~~~~~^~~[m[K
2025-11-05T14:36:45.3216443Z /havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-05T14:36:45.3217598Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:27:61:[m[K   required from here
2025-11-05T14:36:45.3218928Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::end(_Tp (&)[_Nm])[m[K'
2025-11-05T14:36:45.3219661Z    97 |     [01;36m[Kend[m[K(_Tp (&__arr)[_Nm])
2025-11-05T14:36:45.3219928Z       |     [01;36m[K^~~[m[K
2025-11-05T14:36:45.3220554Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3221552Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3222350Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3223105Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3224078Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3224843Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-05T14:36:45.3225213Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3225680Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3226373Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3227122Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3228279Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::end(std::valarray<_Tp>&)[m[K'
2025-11-05T14:36:45.3229058Z   106 |   template<typename _Tp> _Tp* [01;36m[Kend[m[K(valarray<_Tp>&);
2025-11-05T14:36:45.3229423Z       |                               [01;36m[K^~~[m[K
2025-11-05T14:36:45.3230091Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3230964Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3231739Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3232489Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3233559Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3234355Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-05T14:36:45.3234891Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3235382Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-05T14:36:45.3236100Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-05T14:36:45.3236853Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3238258Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::end(const std::valarray<_Tp>&)[m[K'
2025-11-05T14:36:45.3239453Z   107 |   template<typename _Tp> const _Tp* [01;36m[Kend[m[K(const valarray<_Tp>&);
2025-11-05T14:36:45.3239864Z       |                                     [01;36m[K^~~[m[K
2025-11-05T14:36:45.3240540Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3241416Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:26[m[K,
2025-11-05T14:36:45.3242356Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:21[m[K,
2025-11-05T14:36:45.3243117Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:5[m[K:
2025-11-05T14:36:45.3244234Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-05T14:36:45.3245047Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-05T14:36:45.3245412Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-05T14:36:45.3246388Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:89:52:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kstd::filesystem::path::path(<brace-enclosed initializer list>)[m[K'
2025-11-05T14:36:45.3247266Z    89 |         : path{std::begin(source), std::end(source)[01;31m[K}[m[K {
2025-11-05T14:36:45.3247626Z       |                                                    [01;31m[K^[m[K
2025-11-05T14:36:45.3248566Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:102:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const TCHAR*)[m[K'
2025-11-05T14:36:45.3249266Z   102 |     [01;36m[Kpath[m[K(const TCHAR* const s)
2025-11-05T14:36:45.3249552Z       |     [01;36m[K^~~~[m[K
2025-11-05T14:36:45.3250212Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:102:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-05T14:36:45.3251354Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:98:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const char16_t*)[m[K'
2025-11-05T14:36:45.3252060Z    98 |     [01;36m[Kpath[m[K(const char16_t* const s)
2025-11-05T14:36:45.3252340Z       |     [01;36m[K^~~~[m[K
2025-11-05T14:36:45.3252975Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:98:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-05T14:36:45.3254220Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:93:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class InputIt> std::filesystem::path::path(InputIt, InputIt)[m[K'
2025-11-05T14:36:45.3254978Z    93 |     [01;36m[Kpath[m[K(InputIt first,
2025-11-05T14:36:45.3255250Z       |     [01;36m[K^~~~[m[K
2025-11-05T14:36:45.3255917Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:93:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3257155Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:88:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class Source> std::filesystem::path::path(const Source&)[m[K'
2025-11-05T14:36:45.3257914Z    88 |     [01;36m[Kpath[m[K(const Source& source)
2025-11-05T14:36:45.3258456Z       |     [01;36m[K^~~~[m[K
2025-11-05T14:36:45.3259117Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:88:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-05T14:36:45.3260288Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:83:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(std::filesystem::path&&)[m[K'
2025-11-05T14:36:45.3260978Z    83 |     [01;36m[Kpath[m[K(path&& p)
2025-11-05T14:36:45.3261241Z       |     [01;36m[K^~~~[m[K
2025-11-05T14:36:45.3261870Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:83:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-05T14:36:45.3263156Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:79:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const std::filesystem::path&)[m[K'
2025-11-05T14:36:45.3263887Z    79 |     [01;36m[Kpath[m[K(const path& p)
2025-11-05T14:36:45.3264150Z       |     [01;36m[K^~~~[m[K
2025-11-05T14:36:45.3264784Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:79:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-05T14:36:45.3265860Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:75:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path()[m[K'
2025-11-05T14:36:45.3266485Z    75 |     [01;36m[Kpath[m[K()
2025-11-05T14:36:45.3266720Z       |     [01;36m[K^~~~[m[K
2025-11-05T14:36:45.3267361Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../file.hpp:75:5:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 2 provided
2025-11-05T14:36:45.4665499Z [01m[Kcc1plus:[m[K [01;35m[Kwarning: [m[Kunrecognized command line option '[01m[K-Wno-volatile[m[K'
2025-11-05T14:36:45.4699485Z make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5775: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
2025-11-05T14:36:45.4707746Z make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
2025-11-05T14:36:45.4708746Z make: *** [Makefile:101: all] Error 2