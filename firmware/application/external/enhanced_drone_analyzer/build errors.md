## EDA Build Status - 2025-11-09

### Current Status: BROKEN - 100+ Compilation Errors

The Enhanced Drone Analyzer (EDA) application has extensive compilation issues that prevent it from building. The code was migrated from Recon/Glass patterns but contains fundamental structural problems.

### Major Issues Identified:

1. **Constant Redefinitions**
   - SCAN_THREAD_STACK_SIZE defined in both .hpp and .cpp
   - DEFAULT_RSSI_THRESHOLD_DB redefined
   - Multiple other constants duplicated

2. **FreqmanDB Integration Issues**
   - Code tries to use vector methods (.size(), .entry_count()) on FreqmanDB
   - Missing proper FreqmanDB API usage
   - freqman_entry access issues (unique_ptr vs direct access)

3. **Missing Member Variables**
   - Classes missing critical member variables (scanning_thread_, scanner_, etc.)
   - Undefined member functions throughout

4. **Forward Declaration Problems**
   - Incomplete types for DroneHardwareController, AudioManager
   - Missing implementations for declared methods

5. **Filesystem Path Issues**
   - std::filesystem::path construction failures
   - Incompatible path usage with const char*

6. **UI Component Issues**
   - Missing unique_ptr initializations
   - Incorrect constructor calls
   - Missing member function implementations

### Required Fixes:

1. **Remove duplicate constants** from .cpp file
2. **Fix FreqmanDB usage** - use proper FreqmanDB API instead of vector methods
3. **Add missing member variables** to all classes
4. **Implement missing methods** for UI components
5. **Fix forward declarations** and complete type definitions
6. **Correct filesystem path usage**

### Next Steps:

The codebase needs a complete rewrite following proper Mayhem firmware patterns. Current approach of copying from Recon/Glass without understanding the architecture has resulted in a non-functional implementation.

**Recommendation:** Start over with a clean implementation based on working Mayhem applications, not direct code copying.

---

Original build output:
2025-11-09T02:11:20.2363722Z [100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
2025-11-09T02:11:20.9476895Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-09T02:11:20.9479444Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11[m[K,
2025-11-09T02:11:20.9481110Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31[m[K:
2025-11-09T02:11:20.9486495Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::ScanningCoordinator[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:20.9488911Z    18 | class [01;35m[KScanningCoordinator[m[K {
2025-11-09T02:11:20.9489546Z       |       [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:20.9492239Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:20.9496254Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:20.9670750Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11[m[K,
2025-11-09T02:11:20.9672084Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31[m[K:
2025-11-09T02:11:20.9674219Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:20.9675609Z   300 | class [01;35m[KDroneScanner[m[K {
2025-11-09T02:11:20.9675964Z       |       [01;35m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:20.9677611Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:20.9679924Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:21.3552533Z [01m[Kcc1plus:[m[K [01;35m[Kwarning: [m[Kunrecognized command line option '[01m[K-Wno-volatile[m[K'
2025-11-09T02:11:21.4174613Z [100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
2025-11-09T02:11:21.5141550Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1130:27:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-09T02:11:21.5142441Z  1130 |         progress_bar[i] = [01;35m[K'â–ˆ'[m[K;
2025-11-09T02:11:21.5142760Z       |                           [01;35m[K^~~~~[m[K
2025-11-09T02:11:21.5143633Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1379:61:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-09T02:11:21.5144578Z  1379 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'â–²'[m[K; break;
2025-11-09T02:11:21.5145184Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-09T02:11:21.5146028Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1380:58:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-09T02:11:21.5146916Z  1380 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'â–¼'[m[K; break;
2025-11-09T02:11:21.5147341Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-09T02:11:21.5148174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1383:37:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-09T02:11:21.5149257Z  1383 |             default: trend_symbol = [01;35m[K'â– '[m[K; break;
2025-11-09T02:11:21.5149608Z       |                                     [01;35m[K^~~~~[m[K
2025-11-09T02:11:22.1297659Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-09T02:11:22.1298770Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.1300603Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::ScanningCoordinator[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.1301874Z    18 | class [01;35m[KScanningCoordinator[m[K {
2025-11-09T02:11:22.1302297Z       |       [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.1304106Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.1306222Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.1502600Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.1504354Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.1505467Z   300 | class [01;35m[KDroneScanner[m[K {
2025-11-09T02:11:22.1505755Z       |       [01;35m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.1507049Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.1508869Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.2343649Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:35:27:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const uint32_t SCAN_THREAD_STACK_SIZE[m[K'
2025-11-09T02:11:22.2346292Z    35 | static constexpr uint32_t [01;31m[KSCAN_THREAD_STACK_SIZE[m[K = 2048;
2025-11-09T02:11:22.2347024Z       |                           [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2348043Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2350091Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:62:27:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const uint32_t SCAN_THREAD_STACK_SIZE[m[K' previously defined here
2025-11-09T02:11:22.2351689Z    62 | static constexpr uint32_t [01;36m[KSCAN_THREAD_STACK_SIZE[m[K = 2048;
2025-11-09T02:11:22.2352379Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2354301Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:26:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB[m[K'
2025-11-09T02:11:22.2356483Z    36 | static constexpr int32_t [01;31m[KDEFAULT_RSSI_THRESHOLD_DB[m[K = -90;
2025-11-09T02:11:22.2357209Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2358156Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18[m[K,
2025-11-09T02:11:22.2359012Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2360130Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:210:26:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB[m[K' previously defined here
2025-11-09T02:11:22.2361063Z   210 | static constexpr int32_t [01;36m[KDEFAULT_RSSI_THRESHOLD_DB[m[K = -90;
2025-11-09T02:11:22.2361480Z       |                          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2362539Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:37:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MAX_TRACKED_DRONES[m[K'
2025-11-09T02:11:22.2363385Z    37 | static constexpr size_t [01;31m[KMAX_TRACKED_DRONES[m[K = 8;
2025-11-09T02:11:22.2363754Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2364346Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2365691Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:153:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MAX_TRACKED_DRONES[m[K' previously defined here
2025-11-09T02:11:22.2366536Z   153 | static constexpr size_t [01;36m[KMAX_TRACKED_DRONES[m[K = 8;
2025-11-09T02:11:22.2366911Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2367776Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:41:26:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const int32_t HYSTERESIS_MARGIN_DB[m[K'
2025-11-09T02:11:22.2368638Z    41 | static constexpr int32_t [01;31m[KHYSTERESIS_MARGIN_DB[m[K = 5;
2025-11-09T02:11:22.2369012Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2369603Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:19[m[K,
2025-11-09T02:11:22.2370407Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2371476Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:14:26:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const int16_t HYSTERESIS_MARGIN_DB[m[K'
2025-11-09T02:11:22.2372331Z    14 | static constexpr int16_t [01;36m[KHYSTERESIS_MARGIN_DB[m[K = 5;
2025-11-09T02:11:22.2372713Z       |                          [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2373611Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:43:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency WIDEBAND_DEFAULT_MIN[m[K'
2025-11-09T02:11:22.2374526Z    43 | static constexpr Frequency [01;31m[KWIDEBAND_DEFAULT_MIN[m[K = 2400000000ULL;
2025-11-09T02:11:22.2375125Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2375718Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2376982Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:159:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_DEFAULT_MIN[m[K'
2025-11-09T02:11:22.2377890Z   159 | static constexpr uint32_t [01;36m[KWIDEBAND_DEFAULT_MIN[m[K = 2'400'000'000ULL;
2025-11-09T02:11:22.2378311Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2379178Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:44:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency WIDEBAND_DEFAULT_MAX[m[K'
2025-11-09T02:11:22.2380085Z    44 | static constexpr Frequency [01;31m[KWIDEBAND_DEFAULT_MAX[m[K = 2500000000ULL;
2025-11-09T02:11:22.2380493Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2381072Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2382192Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:160:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_DEFAULT_MAX[m[K'
2025-11-09T02:11:22.2383203Z   160 | static constexpr uint32_t [01;36m[KWIDEBAND_DEFAULT_MAX[m[K = 2'500'000'000ULL;
2025-11-09T02:11:22.2383614Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2384481Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:45:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency WIDEBAND_SLICE_WIDTH[m[K'
2025-11-09T02:11:22.2385594Z    45 | static constexpr Frequency [01;31m[KWIDEBAND_SLICE_WIDTH[m[K = 20000000ULL;
2025-11-09T02:11:22.2386005Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2386595Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2387720Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:161:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_SLICE_WIDTH[m[K'
2025-11-09T02:11:22.2388619Z   161 | static constexpr uint32_t [01;36m[KWIDEBAND_SLICE_WIDTH[m[K = 25'000'000;
2025-11-09T02:11:22.2389016Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2389875Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:46:25:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const size_t WIDEBAND_MAX_SLICES[m[K'
2025-11-09T02:11:22.2390724Z    46 | static constexpr size_t [01;31m[KWIDEBAND_MAX_SLICES[m[K = 8;
2025-11-09T02:11:22.2391092Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2391892Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2394530Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:162:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_MAX_SLICES[m[K'
2025-11-09T02:11:22.2395930Z   162 | static constexpr uint32_t [01;36m[KWIDEBAND_MAX_SLICES[m[K = 20;
2025-11-09T02:11:22.2396334Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2397192Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:47:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t DETECTION_TABLE_SIZE[m[K'
2025-11-09T02:11:22.2398060Z    47 | static constexpr size_t [01;31m[KDETECTION_TABLE_SIZE[m[K = 1024;
2025-11-09T02:11:22.2398446Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2399218Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2400346Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:163:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t DETECTION_TABLE_SIZE[m[K' previously defined here
2025-11-09T02:11:22.2401205Z   163 | static constexpr size_t [01;36m[KDETECTION_TABLE_SIZE[m[K = 256;
2025-11-09T02:11:22.2401584Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2402412Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:48:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MAX_DISPLAYED_DRONES[m[K'
2025-11-09T02:11:22.2403451Z    48 | static constexpr size_t [01;31m[KMAX_DISPLAYED_DRONES[m[K = 8;
2025-11-09T02:11:22.2404031Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2404995Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2406307Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:154:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MAX_DISPLAYED_DRONES[m[K' previously defined here
2025-11-09T02:11:22.2407168Z   154 | static constexpr size_t [01;36m[KMAX_DISPLAYED_DRONES[m[K = 3;
2025-11-09T02:11:22.2407541Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2408442Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:50:30:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const char* const default_gradient_file[m[K'
2025-11-09T02:11:22.2409410Z    50 | static constexpr const char* [01;31m[Kdefault_gradient_file[m[K = "/sdcard/GRADIENT.BMP";
2025-11-09T02:11:22.2409872Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2410480Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:22[m[K,
2025-11-09T02:11:22.2411288Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2412527Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:31:36:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconst std::filesystem::path default_gradient_file[m[K'
2025-11-09T02:11:22.2413409Z    31 | extern const std::filesystem::path [01;36m[Kdefault_gradient_file[m[K;
2025-11-09T02:11:22.2413819Z       |                                    [01;36m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2414841Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:51:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MINI_SPECTRUM_WIDTH[m[K'
2025-11-09T02:11:22.2415691Z    51 | static constexpr size_t [01;31m[KMINI_SPECTRUM_WIDTH[m[K = 240;
2025-11-09T02:11:22.2416071Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2416661Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2417761Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:155:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MINI_SPECTRUM_WIDTH[m[K' previously defined here
2025-11-09T02:11:22.2418602Z   155 | static constexpr size_t [01;36m[KMINI_SPECTRUM_WIDTH[m[K = 200;
2025-11-09T02:11:22.2418971Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2419798Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:52:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MINI_SPECTRUM_HEIGHT[m[K'
2025-11-09T02:11:22.2420761Z    52 | static constexpr size_t [01;31m[KMINI_SPECTRUM_HEIGHT[m[K = 64;
2025-11-09T02:11:22.2421128Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2421719Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2422822Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:156:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MINI_SPECTRUM_HEIGHT[m[K' previously defined here
2025-11-09T02:11:22.2423669Z   156 | static constexpr size_t [01;36m[KMINI_SPECTRUM_HEIGHT[m[K = 24;
2025-11-09T02:11:22.2424042Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2425030Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:53:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency MIN_HARDWARE_FREQ[m[K'
2025-11-09T02:11:22.2425934Z    53 | static constexpr Frequency [01;31m[KMIN_HARDWARE_FREQ[m[K = 50000000ULL;
2025-11-09T02:11:22.2426335Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2427031Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2428147Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:157:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t MIN_HARDWARE_FREQ[m[K'
2025-11-09T02:11:22.2429013Z   157 | static constexpr uint32_t [01;36m[KMIN_HARDWARE_FREQ[m[K = 1'000'000;
2025-11-09T02:11:22.2429402Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2430228Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:54:28:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const Frequency MAX_HARDWARE_FREQ[m[K'
2025-11-09T02:11:22.2431093Z    54 | static constexpr Frequency [01;31m[KMAX_HARDWARE_FREQ[m[K = 6000000000ULL;
2025-11-09T02:11:22.2431494Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2432073Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2433174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:158:27:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const uint64_t MAX_HARDWARE_FREQ[m[K' previously defined here
2025-11-09T02:11:22.2434055Z   158 | static constexpr uint64_t [01;36m[KMAX_HARDWARE_FREQ[m[K = 6'000'000'000ULL;
2025-11-09T02:11:22.2434454Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2435560Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneScanner()[m[K':
2025-11-09T02:11:22.2437247Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:377:10:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2438231Z   377 |     bool [01;35m[Kscanning_active_[m[K = false;
2025-11-09T02:11:22.2438553Z       |          [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2439625Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:375:13:[m[K [01;35m[Kwarning: [m[K  '[01m[KThread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2440653Z   375 |     Thread* [01;35m[Kscanning_thread_[m[K = nullptr;
2025-11-09T02:11:22.2441114Z       |             [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2441872Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2442615Z   145 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-09T02:11:22.2442909Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2443475Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2445148Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:401:22:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2446183Z   401 |     WidebandScanData [01;35m[Kwideband_scan_data_[m[K;
2025-11-09T02:11:22.2446547Z       |                      [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2447966Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:387:18:[m[K [01;35m[Kwarning: [m[K  '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_mode_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2449181Z   387 |     ScanningMode [01;35m[Kscanning_mode_[m[K = ScanningMode::DATABASE;
2025-11-09T02:11:22.2449604Z       |                  [01;35m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2450371Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2451111Z   145 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-09T02:11:22.2451400Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2451960Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2453338Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:387:18:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_mode_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2454449Z   387 |     ScanningMode [01;35m[Kscanning_mode_[m[K = ScanningMode::DATABASE;
2025-11-09T02:11:22.2455062Z       |                  [01;35m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2456375Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:379:16:[m[K [01;35m[Kwarning: [m[K  '[01m[Kfreqman_db ui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2457869Z   379 |     freqman_db [01;35m[Kfreq_db_[m[K;
2025-11-09T02:11:22.2458283Z       |                [01;35m[K^~~~~~~~[m[K
2025-11-09T02:11:22.2459059Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-09T02:11:22.2459822Z   145 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-09T02:11:22.2460118Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2461323Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.2463195Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_database_and_scanner()[m[K':
2025-11-09T02:11:22.2465129Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:185:14:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kopen[m[K'
2025-11-09T02:11:22.2466362Z   185 |     freq_db_.[01;31m[Kopen[m[K(db_path, false);
2025-11-09T02:11:22.2466677Z       |              [01;31m[K^~~~[m[K
2025-11-09T02:11:22.2471942Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::load_frequency_database()[m[K':
2025-11-09T02:11:22.2474129Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:274:14:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kclose[m[K'
2025-11-09T02:11:22.2476380Z   274 |     freq_db_.[01;31m[Kclose[m[K();
2025-11-09T02:11:22.2484922Z       |              [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.2487490Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:278:19:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kopen[m[K'
2025-11-09T02:11:22.2489334Z   278 |     if (!freq_db_.[01;31m[Kopen[m[K(db_path, false)) {
2025-11-09T02:11:22.2489855Z       |                   [01;31m[K^~~~[m[K
2025-11-09T02:11:22.2491825Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:282:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-09T02:11:22.2493678Z   282 |     if (freq_db_.[01;31m[Kentry_count[m[K() > 100) {
2025-11-09T02:11:22.2494017Z       |                  [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.2495336Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Ksize_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_database_size() const[m[K':
2025-11-09T02:11:22.2497056Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:291:21:[m[K [01;31m[Kerror: [m[K'[01m[Kconst freqman_db[m[K' {aka '[01m[Kconst class std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-09T02:11:22.2498025Z   291 |     return freq_db_.[01;31m[Kentry_count[m[K();
2025-11-09T02:11:22.2498350Z       |                     [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.2499596Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K':
2025-11-09T02:11:22.2501607Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:339:43:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-09T02:11:22.2502736Z   339 |     const size_t total_entries = freq_db_.[01;31m[Kentry_count[m[K();
2025-11-09T02:11:22.2503134Z       |                                           [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.2504408Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:344:38:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-09T02:11:22.2505850Z   344 |     if (current_db_index_ < freq_db_.[01;31m[Kentry_count[m[K()) {
2025-11-09T02:11:22.2506218Z       |                                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.2507203Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:346:46:[m[K [01;31m[Kerror: [m[K'[01m[Kconst class std::unique_ptr<freqman_entry>[m[K' has no member named '[01m[Kfrequency_a[m[K'
2025-11-09T02:11:22.2508103Z   346 |             Frequency target_freq_hz = entry.[01;31m[Kfrequency_a[m[K;
2025-11-09T02:11:22.2508489Z       |                                              [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.2509485Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:350:44:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kconst std::unique_ptr<freqman_entry>[m[K' to '[01m[Kconst freqman_entry&[m[K'
2025-11-09T02:11:22.2510641Z   350 |                     process_rssi_detection([01;31m[Kentry[m[K, real_rssi);
2025-11-09T02:11:22.2511581Z       |                                            [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.2512195Z       |                                            [01;31m[K|[m[K
2025-11-09T02:11:22.2512713Z       |                                            [01;31m[Kconst std::unique_ptr<freqman_entry>[m[K
2025-11-09T02:11:22.2513353Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2515026Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:327:54:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::process_rssi_detection(const freqman_entry&, int32_t)[m[K'
2025-11-09T02:11:22.2516188Z   327 |     void process_rssi_detection([01;36m[Kconst freqman_entry& entry[m[K, int32_t rssi);
2025-11-09T02:11:22.2516642Z       |                                 [01;36m[K~~~~~~~~~~~~~~~~~~~~~^~~~~[m[K
2025-11-09T02:11:22.2517228Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.2518246Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:356:7:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[Kelse[m[K'
2025-11-09T02:11:22.2518921Z   356 |     } [01;31m[Kelse[m[K {
2025-11-09T02:11:22.2519176Z       |       [01;31m[K^~~~[m[K
2025-11-09T02:11:22.2519910Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:361:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.2520822Z   361 | void [01;31m[KDroneScanner[m[K::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
2025-11-09T02:11:22.2521272Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2522080Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid perform_wideband_scan_cycle(DroneHardwareController&)[m[K':
2025-11-09T02:11:22.2523470Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:362:9:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_scan_data_[m[K' was not declared in this scope; did you mean '[01m[KWidebandScanData[m[K'?
2025-11-09T02:11:22.2524367Z   362 |     if ([01;31m[Kwideband_scan_data_[m[K.slices_nb == 0) {
2025-11-09T02:11:22.2524801Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2525095Z       |         [32m[KWidebandScanData[m[K
2025-11-09T02:11:22.2533279Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:363:9:[m[K [01;31m[Kerror: [m[K'[01m[Ksetup_wideband_range[m[K' was not declared in this scope
2025-11-09T02:11:22.2534389Z   363 |         [01;31m[Ksetup_wideband_range[m[K(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
2025-11-09T02:11:22.2534964Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2548866Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:366:9:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_scan_data_[m[K' was not declared in this scope; did you mean '[01m[KWidebandScanData[m[K'?
2025-11-09T02:11:22.2549917Z   366 |     if ([01;31m[Kwideband_scan_data_[m[K.slice_counter >= wideband_scan_data_.slices_nb) {
2025-11-09T02:11:22.2550332Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2550618Z       |         [32m[KWidebandScanData[m[K
2025-11-09T02:11:22.2563650Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:370:42:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_scan_data_[m[K' was not declared in this scope; did you mean '[01m[KWidebandScanData[m[K'?
2025-11-09T02:11:22.2566015Z   370 |     const WidebandSlice& current_slice = [01;31m[Kwideband_scan_data_[m[K.slices[wideband_scan_data_.slice_counter];
2025-11-09T02:11:22.2566995Z       |                                          [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2567603Z       |                                          [32m[KWidebandScanData[m[K
2025-11-09T02:11:22.2569142Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:371:9:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.2570370Z   371 |     if ([01;31m[Khardware[m[K.tune_to_frequency(current_slice.center_frequency)) {
2025-11-09T02:11:22.2570770Z       |         [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.2571360Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2572456Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.2573242Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.2573557Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2574395Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:372:30:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.2575506Z   372 |         int32_t slice_rssi = [01;31m[Khardware[m[K.get_real_rssi_from_hardware(current_slice.center_frequency);
2025-11-09T02:11:22.2575981Z       |                              [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.2576572Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.2577633Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.2578395Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.2578701Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2580267Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:375:46:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[K(Frequency)current_slice.WidebandSlice::center_frequency[m[K' from '[01m[KFrequency[m[K' {aka '[01m[Klong long unsigned int[m[K'} to '[01m[Kint64_t[m[K' {aka '[01m[Klong long int[m[K'} [[01;35m[K-Wnarrowing[m[K]
2025-11-09T02:11:22.2582123Z   375 |                 .frequency_a = [01;35m[Kcurrent_slice.center_frequency[m[K,
2025-11-09T02:11:22.2583083Z       |                                [01;35m[K~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2584758Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:376:46:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[K(Frequency)current_slice.WidebandSlice::center_frequency[m[K' from '[01m[KFrequency[m[K' {aka '[01m[Klong long unsigned int[m[K'} to '[01m[Kint64_t[m[K' {aka '[01m[Klong long int[m[K'} [[01;35m[K-Wnarrowing[m[K]
2025-11-09T02:11:22.2586057Z   376 |                 .frequency_b = [01;35m[Kcurrent_slice.center_frequency[m[K,
2025-11-09T02:11:22.2586442Z       |                                [01;35m[K~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2587420Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:377:25:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kuint8_t[m[K' {aka '[01m[Kunsigned char[m[K'} to '[01m[Kfreqman_type[m[K' in initialization
2025-11-09T02:11:22.2588401Z   377 |                 .type = [01;31m[Kstatic_cast<uint8_t>(freqman_type::Single)[m[K,
2025-11-09T02:11:22.2588976Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2589309Z       |                         [01;31m[K|[m[K
2025-11-09T02:11:22.2589627Z       |                         [01;31m[Kuint8_t {aka unsigned char}[m[K
2025-11-09T02:11:22.2590769Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:382:13:[m[K [01;31m[Kerror: [m[Kdesignator order for field '[01m[Kfreqman_entry::description[m[K' does not match declaration order in '[01m[Kfreqman_entry[m[K'
2025-11-09T02:11:22.2591673Z   382 |             [01;31m[K}[m[K;
2025-11-09T02:11:22.2591919Z       |             [01;31m[K^[m[K
2025-11-09T02:11:22.2592732Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:383:13:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_detection_override[m[K' was not declared in this scope
2025-11-09T02:11:22.2593667Z   383 |             [01;31m[Kwideband_detection_override[m[K(fake_entry, slice_rssi, WIDEBAND_RSSI_THRESHOLD_DB);
2025-11-09T02:11:22.2594119Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2600202Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:385:9:[m[K [01;31m[Kerror: [m[K'[01m[Klast_scanned_frequency_[m[K' was not declared in this scope
2025-11-09T02:11:22.2601113Z   385 |         [01;31m[Klast_scanned_frequency_[m[K = current_slice.center_frequency;
2025-11-09T02:11:22.2601514Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2612735Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:387:13:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_cycles_[m[K' was not declared in this scope
2025-11-09T02:11:22.2613525Z   387 |         if ([01;31m[Kscan_cycles_[m[K % 100 == 0) {
2025-11-09T02:11:22.2613839Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2629568Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:388:13:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_scan_error[m[K' was not declared in this scope
2025-11-09T02:11:22.2631170Z   388 |             [01;31m[Khandle_scan_error[m[K("Hardware tuning failed in wideband mode");
2025-11-09T02:11:22.2631841Z       |             [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2632443Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.2634214Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:394:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.2635955Z   394 | void [01;31m[KDroneScanner[m[K::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
2025-11-09T02:11:22.2636491Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2663766Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid wideband_detection_override(const freqman_entry&, int32_t, int32_t)[m[K':
2025-11-09T02:11:22.2665683Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:398:9:[m[K [01;31m[Kerror: [m[K'[01m[Kprocess_wideband_detection_with_override[m[K' was not declared in this scope; did you mean '[01m[Kwideband_detection_override[m[K'?
2025-11-09T02:11:22.2666949Z   398 |         [01;31m[Kprocess_wideband_detection_with_override[m[K(wideband_entry, rssi, DEFAULT_RSSI_THRESHOLD_DB, threshold_override);
2025-11-09T02:11:22.2667518Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2667854Z       |         [32m[Kwideband_detection_override[m[K
2025-11-09T02:11:22.2668447Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.2669633Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:402:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.2670629Z   402 | void [01;31m[KDroneScanner[m[K::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
2025-11-09T02:11:22.2671128Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2683125Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid process_wideband_detection_with_override(const freqman_entry&, int32_t, int32_t, int32_t)[m[K':
2025-11-09T02:11:22.2685877Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:422:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktotal_detections_[m[K' was not declared in this scope
2025-11-09T02:11:22.2687145Z   422 |     [01;31m[Ktotal_detections_[m[K++;
2025-11-09T02:11:22.2687643Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2689825Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:427:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-09T02:11:22.2691818Z   427 |     if ([01;31m[Klocal_detection_ring[m[K.get_rssi_value(freq_hash) < wideband_threshold) {
2025-11-09T02:11:22.2692532Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2693273Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-09T02:11:22.2697920Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-09T02:11:22.2699661Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-09T02:11:22.2700435Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2702598Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:432:33:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-09T02:11:22.2704846Z   432 |         uint8_t current_count = [01;31m[Klocal_detection_ring[m[K.get_detection_count(freq_hash);
2025-11-09T02:11:22.2705672Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2706487Z       |                                 [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-09T02:11:22.2708615Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-09T02:11:22.2710315Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-09T02:11:22.2711108Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2712935Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:447:17:[m[K [01;31m[Kerror: [m[K'[01m[Kdetection_logger_[m[K' was not declared in this scope
2025-11-09T02:11:22.2714339Z   447 |             if ([01;31m[Kdetection_logger_[m[K.is_session_active()) {
2025-11-09T02:11:22.2715110Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2731760Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:450:13:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracked_drone[m[K' was not declared in this scope
2025-11-09T02:11:22.2733230Z   450 |             [01;31m[Kupdate_tracked_drone[m[K(detected_type, entry.frequency_a, rssi, threat_level);
2025-11-09T02:11:22.2733700Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2735814Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:453:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-09T02:11:22.2737596Z   453 |         [01;31m[Klocal_detection_ring[m[K.update_detection(freq_hash, 0, -120);
2025-11-09T02:11:22.2737998Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2738453Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-09T02:11:22.2739518Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-09T02:11:22.2740466Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-09T02:11:22.2740896Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2741814Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:403:68:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Koriginal_threshold[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-09T02:11:22.2742786Z   403 |                                                            [01;35m[Kint32_t original_threshold[m[K, int32_t wideband_threshold) {
2025-11-09T02:11:22.2743226Z       |                                                            [01;35m[K~~~~~~~~^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2743816Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.2745056Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:457:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.2745991Z   457 | void [01;31m[KDroneScanner[m[K::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
2025-11-09T02:11:22.2746428Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2756775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid perform_hybrid_scan_cycle(DroneHardwareController&)[m[K':
2025-11-09T02:11:22.2758215Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:458:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_cycles_[m[K' was not declared in this scope
2025-11-09T02:11:22.2759150Z   458 |     if ([01;31m[Kscan_cycles_[m[K % 2 == 0) {
2025-11-09T02:11:22.2759445Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2768819Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:461:9:[m[K [01;31m[Kerror: [m[K'[01m[Kperform_database_scan_cycle[m[K' was not declared in this scope; did you mean '[01m[Kperform_wideband_scan_cycle[m[K'?
2025-11-09T02:11:22.2770580Z   461 |         [01;31m[Kperform_database_scan_cycle[m[K(hardware);
2025-11-09T02:11:22.2771173Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2771700Z       |         [32m[Kperform_wideband_scan_cycle[m[K
2025-11-09T02:11:22.2772741Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.2774541Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:465:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.2776398Z   465 | void [01;31m[KDroneScanner[m[K::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
2025-11-09T02:11:22.2777161Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2789800Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid process_rssi_detection(const freqman_entry&, int32_t)[m[K':
2025-11-09T02:11:22.2792160Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:474:10:[m[K [01;31m[Kerror: [m[K'[01m[Kvalidate_detection_simple[m[K' was not declared in this scope
2025-11-09T02:11:22.2793781Z   474 |     if (![01;31m[Kvalidate_detection_simple[m[K(rssi, ThreatLevel::UNKNOWN)) {
2025-11-09T02:11:22.2794465Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2807762Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:479:28:[m[K [01;31m[Kerror: [m[K'[01m[Kdrone_database_[m[K' was not declared in this scope
2025-11-09T02:11:22.2809386Z   479 |     const auto* db_entry = [01;31m[Kdrone_database_[m[K.lookup_frequency(entry.frequency_a);
2025-11-09T02:11:22.2810148Z       |                            [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2826090Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:502:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktotal_detections_[m[K' was not declared in this scope
2025-11-09T02:11:22.2827345Z   502 |     [01;31m[Ktotal_detections_[m[K++;
2025-11-09T02:11:22.2827906Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2830065Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:509:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-09T02:11:22.2832064Z   509 |     if ([01;31m[Klocal_detection_ring[m[K.get_rssi_value(freq_hash) < detection_threshold) {
2025-11-09T02:11:22.2832789Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2833535Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-09T02:11:22.2835622Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-09T02:11:22.2837306Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-09T02:11:22.2838049Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2840173Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:514:33:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-09T02:11:22.2842516Z   514 |         uint8_t current_count = [01;31m[Klocal_detection_ring[m[K.get_detection_count(freq_hash);
2025-11-09T02:11:22.2843313Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2844127Z       |                                 [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-09T02:11:22.2846154Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-09T02:11:22.2847867Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-09T02:11:22.2848623Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2855409Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:531:17:[m[K [01;31m[Kerror: [m[K'[01m[Kdetection_logger_[m[K' was not declared in this scope
2025-11-09T02:11:22.2857066Z   531 |             if ([01;31m[Kdetection_logger_[m[K.is_session_active()) {
2025-11-09T02:11:22.2857676Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2873803Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:535:13:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracked_drone[m[K' was not declared in this scope
2025-11-09T02:11:22.2875587Z   535 |             [01;31m[Kupdate_tracked_drone[m[K(detected_type, entry.frequency_a, rssi, threat_level);
2025-11-09T02:11:22.2876351Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2878512Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:538:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-09T02:11:22.2880445Z   538 |         [01;31m[Klocal_detection_ring[m[K.update_detection(freq_hash, 0, -120);
2025-11-09T02:11:22.2881116Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2881860Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-09T02:11:22.2883754Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-09T02:11:22.2885634Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-09T02:11:22.2886402Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2887430Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.2889244Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:542:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.2891109Z   542 | void [01;31m[KDroneScanner[m[K::update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
2025-11-09T02:11:22.2892073Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2896106Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_tracked_drone(DroneType, Frequency, int32_t, ThreatLevel)[m[K':
2025-11-09T02:11:22.2898710Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:543:24:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-09T02:11:22.2900492Z   543 |     for (auto& drone : [01;31m[Ktracked_drones_[m[K) {
2025-11-09T02:11:22.2901072Z       |                        [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2901587Z       |                        [32m[KTrackedDrone[m[K
2025-11-09T02:11:22.2914326Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:548:13:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-09T02:11:22.2915535Z   548 |             [01;31m[Kupdate_tracking_counts[m[K();
2025-11-09T02:11:22.2915863Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2916161Z       |             [32m[Kupdate_tracked_drone[m[K
2025-11-09T02:11:22.2926766Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:553:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-09T02:11:22.2927727Z   553 |     if ([01;31m[Ktracked_drones_[m[K.size() < MAX_TRACKED_DRONES) {
2025-11-09T02:11:22.2928080Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2928530Z       |         [32m[KTrackedDrone[m[K
2025-11-09T02:11:22.2943696Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:560:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.2944495Z   560 |         [01;31m[Ktracked_drones_count_[m[K++;
2025-11-09T02:11:22.2944950Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2958991Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:561:9:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-09T02:11:22.2960636Z   561 |         [01;31m[Kupdate_tracking_counts[m[K();
2025-11-09T02:11:22.2961172Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2961664Z       |         [32m[Kupdate_tracked_drone[m[K
2025-11-09T02:11:22.2972932Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:566:29:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-09T02:11:22.2974637Z   566 |     systime_t oldest_time = [01;31m[Ktracked_drones_[m[K[0].last_seen;
2025-11-09T02:11:22.2975479Z       |                             [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2976002Z       |                             [32m[KTrackedDrone[m[K
2025-11-09T02:11:22.2991113Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:579:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-09T02:11:22.2992742Z   579 |     [01;31m[Kupdate_tracking_counts[m[K();
2025-11-09T02:11:22.2993263Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.2993762Z       |     [32m[Kupdate_tracked_drone[m[K
2025-11-09T02:11:22.2994971Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.2996811Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:582:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.2998218Z   582 | void [01;31m[KDroneScanner[m[K::remove_stale_drones() {
2025-11-09T02:11:22.2998787Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3007014Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid remove_stale_drones()[m[K':
2025-11-09T02:11:22.3008553Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:586:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-09T02:11:22.3009400Z   586 |     [01;31m[Ktracked_drones_[m[K.erase(
2025-11-09T02:11:22.3009686Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3009959Z       |     [32m[KTrackedDrone[m[K
2025-11-09T02:11:22.3024604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:593:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3025674Z   593 |     [01;31m[Ktracked_drones_count_[m[K = tracked_drones_.size();
2025-11-09T02:11:22.3026035Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3040240Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:594:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-09T02:11:22.3041181Z   594 |     [01;31m[Kupdate_tracking_counts[m[K();
2025-11-09T02:11:22.3041649Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3041946Z       |     [32m[Kupdate_tracked_drone[m[K
2025-11-09T02:11:22.3042525Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3043531Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:597:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3044359Z   597 | void [01;31m[KDroneScanner[m[K::update_tracking_counts() {
2025-11-09T02:11:22.3044851Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3057064Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_tracking_counts()[m[K':
2025-11-09T02:11:22.3058302Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:598:5:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3059051Z   598 |     [01;31m[Kapproaching_count_[m[K = 0;
2025-11-09T02:11:22.3059346Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3072910Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:599:5:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3074236Z   599 |     [01;31m[Kreceding_count_[m[K = 0;
2025-11-09T02:11:22.3074876Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3087833Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:600:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3089152Z   600 |     [01;31m[Kstatic_count_[m[K = 0;
2025-11-09T02:11:22.3089628Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3101295Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:602:30:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-09T02:11:22.3102905Z   602 |     for (const auto& drone : [01;31m[Ktracked_drones_[m[K) {
2025-11-09T02:11:22.3103513Z       |                              [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3104040Z       |                              [32m[KTrackedDrone[m[K
2025-11-09T02:11:22.3117918Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:614:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_trends_compact_display[m[K' was not declared in this scope
2025-11-09T02:11:22.3119587Z   614 |     [01;31m[Kupdate_trends_compact_display[m[K();
2025-11-09T02:11:22.3120160Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3121189Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3123011Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:617:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3124474Z   617 | void [01;31m[KDroneScanner[m[K::update_trends_compact_display() {
2025-11-09T02:11:22.3125322Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3126663Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:620:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3128025Z   620 | void [01;31m[KDroneScanner[m[K::reset_scan_cycles() {
2025-11-09T02:11:22.3128586Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3135533Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid reset_scan_cycles()[m[K':
2025-11-09T02:11:22.3137841Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:621:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_cycles_[m[K' was not declared in this scope
2025-11-09T02:11:22.3139105Z   621 |     [01;31m[Kscan_cycles_[m[K = 0;
2025-11-09T02:11:22.3139576Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3140569Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3142384Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:624:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3143779Z   624 | void [01;31m[KDroneScanner[m[K::switch_to_real_mode() {
2025-11-09T02:11:22.3144360Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3152911Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid switch_to_real_mode()[m[K':
2025-11-09T02:11:22.3155218Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:625:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_real_mode_[m[K' was not declared in this scope
2025-11-09T02:11:22.3156507Z   625 |     [01;31m[Kis_real_mode_[m[K = true;
2025-11-09T02:11:22.3156994Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3157981Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3159795Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:628:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3161188Z   628 | void [01;31m[KDroneScanner[m[K::switch_to_demo_mode() {
2025-11-09T02:11:22.3161773Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3171219Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid switch_to_demo_mode()[m[K':
2025-11-09T02:11:22.3173322Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:629:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_real_mode_[m[K' was not declared in this scope
2025-11-09T02:11:22.3174578Z   629 |     [01;31m[Kis_real_mode_[m[K = false;
2025-11-09T02:11:22.3175263Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3176264Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3178076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:632:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3179752Z   632 | void [01;31m[KDroneScanner[m[K::initialize_database_and_scanner() {
2025-11-09T02:11:22.3180378Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3191777Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid initialize_database_and_scanner()[m[K':
2025-11-09T02:11:22.3193019Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:639:38:[m[K [01;31m[Kerror: [m[K'[01m[Kdrone_database_[m[K' was not declared in this scope
2025-11-09T02:11:22.3193863Z   639 |     if (!load_freqman_file("DRONES", [01;31m[Kdrone_database_[m[K, options)) {
2025-11-09T02:11:22.3194276Z       |                                      [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3199775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:642:5:[m[K [01;31m[Kerror: [m[K'[01m[Kfreq_db_[m[K' was not declared in this scope; did you mean '[01m[Kfreqman_db[m[K'?
2025-11-09T02:11:22.3200656Z   642 |     [01;31m[Kfreq_db_[m[K.open(db_path, false);
2025-11-09T02:11:22.3200955Z       |     [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.3201383Z       |     [32m[Kfreqman_db[m[K
2025-11-09T02:11:22.3201945Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3202942Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:645:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3203753Z   645 | void [01;31m[KDroneScanner[m[K::cleanup_database_and_scanner() {
2025-11-09T02:11:22.3204104Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3215265Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid cleanup_database_and_scanner()[m[K':
2025-11-09T02:11:22.3216518Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:646:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope
2025-11-09T02:11:22.3217272Z   646 |     if ([01;31m[Kscanning_thread_[m[K) {
2025-11-09T02:11:22.3217573Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3232813Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:647:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.3233925Z   647 |         [01;31m[Kscanning_active_[m[K = false;
2025-11-09T02:11:22.3234240Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3235013Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3236048Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:653:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3236894Z   653 | void [01;31m[KDroneScanner[m[K::scan_init_from_loaded_frequencies() {
2025-11-09T02:11:22.3237268Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3238017Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:657:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3238912Z   657 | bool [01;31m[KDroneScanner[m[K::validate_detection_simple(int32_t rssi_db, ThreatLevel threat) {
2025-11-09T02:11:22.3239343Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3240066Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:661:11:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3241556Z   661 | Frequency [01;31m[KDroneScanner[m[K::get_current_scanning_frequency() const {
2025-11-09T02:11:22.3242372Z       |           [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3243297Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:661:58:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[KFrequency get_current_scanning_frequency()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.3244278Z   661 | Frequency DroneScanner::get_current_scanning_frequency() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3244848Z       |                                                          [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3245644Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[KFrequency get_current_scanning_frequency()[m[K':
2025-11-09T02:11:22.3246912Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:662:10:[m[K [01;31m[Kerror: [m[K'[01m[Kfreq_db_[m[K' was not declared in this scope; did you mean '[01m[Kfreqman_db[m[K'?
2025-11-09T02:11:22.3247945Z   662 |     if (![01;31m[Kfreq_db_[m[K.empty() && current_db_index_ < freq_db_.entry_count() && freq_db_[current_db_index_].frequency_a > 0) {
2025-11-09T02:11:22.3248565Z       |          [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.3249098Z       |          [32m[Kfreqman_db[m[K
2025-11-09T02:11:22.3260661Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:662:30:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_db_index_[m[K' was not declared in this scope
2025-11-09T02:11:22.3262006Z   662 |     if (!freq_db_.empty() && [01;31m[Kcurrent_db_index_[m[K < freq_db_.entry_count() && freq_db_[current_db_index_].frequency_a > 0) {
2025-11-09T02:11:22.3262549Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3263136Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3264157Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:668:11:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3265182Z   668 | Frequency [01;31m[KDroneScanner[m[K::get_current_radio_frequency() const {
2025-11-09T02:11:22.3265575Z       |           [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3266507Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:668:55:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[KFrequency get_current_radio_frequency()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.3267475Z   668 | Frequency DroneScanner::get_current_radio_frequency() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3267905Z       |                                                       [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3268677Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:672:21:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3269547Z   672 | const TrackedDrone& [01;31m[KDroneScanner[m[K::getTrackedDrone(size_t index) const {
2025-11-09T02:11:22.3269975Z       |                     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3270911Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:672:65:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kconst TrackedDrone& getTrackedDrone(size_t)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.3272448Z   672 | const TrackedDrone& DroneScanner::getTrackedDrone(size_t index) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3273394Z       |                                                                 [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3275048Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kconst TrackedDrone& getTrackedDrone(size_t)[m[K':
2025-11-09T02:11:22.3276661Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:673:17:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-09T02:11:22.3277538Z   673 |     if (index < [01;31m[Ktracked_drones_[m[K.size()) {
2025-11-09T02:11:22.3277860Z       |                 [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3278152Z       |                 [32m[KTrackedDrone[m[K
2025-11-09T02:11:22.3278739Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3279731Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:680:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3280514Z   680 | std::string [01;31m[KDroneScanner[m[K::get_session_summary() const {
2025-11-09T02:11:22.3280880Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3281852Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:680:49:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_session_summary()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.3282878Z   680 | std::string DroneScanner::get_session_summary() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3283285Z       |                                                 [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3292622Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string get_session_summary()[m[K':
2025-11-09T02:11:22.3293832Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:12:[m[K [01;31m[Kerror: [m[K'[01m[Kdetection_logger_[m[K' was not declared in this scope
2025-11-09T02:11:22.3295008Z   681 |     return [01;31m[Kdetection_logger_[m[K.format_session_summary(get_scan_cycles(), get_total_detections());
2025-11-09T02:11:22.3295514Z       |            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3302156Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:53:[m[K [01;31m[Kerror: [m[K'[01m[Kget_scan_cycles[m[K' was not declared in this scope; did you mean '[01m[Kreset_scan_cycles[m[K'?
2025-11-09T02:11:22.3303270Z   681 |     return detection_logger_.format_session_summary([01;31m[Kget_scan_cycles[m[K(), get_total_detections());
2025-11-09T02:11:22.3303788Z       |                                                     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3304149Z       |                                                     [32m[Kreset_scan_cycles[m[K
2025-11-09T02:11:22.3320533Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:72:[m[K [01;31m[Kerror: [m[K'[01m[Kget_total_detections[m[K' was not declared in this scope
2025-11-09T02:11:22.3322352Z   681 |     return detection_logger_.format_session_summary(get_scan_cycles(), [01;31m[Kget_total_detections[m[K());
2025-11-09T02:11:22.3323333Z       |                                                                        [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3324392Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3326424Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:684:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3327959Z   684 | void [01;31m[KDroneScanner[m[K::handle_scan_error(const char* error_msg) {
2025-11-09T02:11:22.3328624Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3335462Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:689:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-09T02:11:22.3337274Z   689 | inline [01;31m[KDroneScanner[m[K::DroneDetectionLogger::DroneDetectionLogger()
2025-11-09T02:11:22.3337983Z       |        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3348094Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:694:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-09T02:11:22.3349686Z   694 | inline [01;31m[KDroneScanner[m[K::DroneDetectionLogger::~DroneDetectionLogger() {
2025-11-09T02:11:22.3350385Z       |        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3351715Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:698:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3353232Z   698 | inline void [01;31m[KDroneScanner[m[K::DroneDetectionLogger::start_session() {
2025-11-09T02:11:22.3353942Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3366443Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid start_session()[m[K':
2025-11-09T02:11:22.3368735Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:699:9:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.3370079Z   699 |     if ([01;31m[Ksession_active_[m[K) return;
2025-11-09T02:11:22.3370596Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3384164Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:700:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.3385677Z   700 |     [01;31m[Ksession_active_[m[K = true;
2025-11-09T02:11:22.3386171Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3399579Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:701:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_start_[m[K' was not declared in this scope
2025-11-09T02:11:22.3400914Z   701 |     [01;31m[Ksession_start_[m[K = chTimeNow();
2025-11-09T02:11:22.3401427Z       |     [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3414352Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:702:5:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3415519Z   702 |     [01;31m[Klogged_count_[m[K = 0;
2025-11-09T02:11:22.3415818Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3429448Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:703:5:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-09T02:11:22.3430257Z   703 |     [01;31m[Kheader_written_[m[K = false;
2025-11-09T02:11:22.3430571Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3431146Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3432149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:706:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3433004Z   706 | inline void [01;31m[KDroneScanner[m[K::DroneDetectionLogger::end_session() {
2025-11-09T02:11:22.3433397Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3444103Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid end_session()[m[K':
2025-11-09T02:11:22.3445424Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:707:10:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.3446367Z   707 |     if (![01;31m[Ksession_active_[m[K) return;
2025-11-09T02:11:22.3446674Z       |          [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3458900Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:708:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.3459686Z   708 |     [01;31m[Ksession_active_[m[K = false;
2025-11-09T02:11:22.3459984Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3460557Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3461568Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:711:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3462543Z   711 | inline bool [01;31m[KDroneScanner[m[K::DroneDetectionLogger::log_detection(const DetectionLogEntry& entry) {
2025-11-09T02:11:22.3463039Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3473677Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool log_detection(const DetectionLogEntry&)[m[K':
2025-11-09T02:11:22.3475259Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:712:10:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.3476037Z   712 |     if (![01;31m[Ksession_active_[m[K) return false;
2025-11-09T02:11:22.3476356Z       |          [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3490227Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:713:10:[m[K [01;31m[Kerror: [m[K'[01m[Kensure_csv_header[m[K' was not declared in this scope
2025-11-09T02:11:22.3491626Z   713 |     if (![01;31m[Kensure_csv_header[m[K()) return false;
2025-11-09T02:11:22.3492192Z       |          [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3507151Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:715:29:[m[K [01;31m[Kerror: [m[K'[01m[Kformat_csv_entry[m[K' was not declared in this scope
2025-11-09T02:11:22.3508656Z   715 |     std::string csv_entry = [01;31m[Kformat_csv_entry[m[K(entry);
2025-11-09T02:11:22.3509311Z       |                             [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3516669Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:18:[m[K [01;31m[Kerror: [m[K'[01m[Kcsv_log_[m[K' was not declared in this scope
2025-11-09T02:11:22.3518164Z   716 |     auto error = [01;31m[Kcsv_log_[m[K.append(generate_log_filename().string());
2025-11-09T02:11:22.3518817Z       |                  [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.3534324Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:34:[m[K [01;31m[Kerror: [m[K'[01m[Kgenerate_log_filename[m[K' was not declared in this scope
2025-11-09T02:11:22.3535842Z   716 |     auto error = csv_log_.append([01;31m[Kgenerate_log_filename[m[K().string());
2025-11-09T02:11:22.3536289Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3548397Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:720:9:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3549282Z   720 |         [01;31m[Klogged_count_[m[K++;
2025-11-09T02:11:22.3549579Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3550159Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3551194Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:726:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3552316Z   726 | inline bool [01;31m[KDroneScanner[m[K::DroneDetectionLogger::ensure_csv_header() {
2025-11-09T02:11:22.3552727Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3564254Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool ensure_csv_header()[m[K':
2025-11-09T02:11:22.3566615Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:727:9:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-09T02:11:22.3567995Z   727 |     if ([01;31m[Kheader_written_[m[K) return true;
2025-11-09T02:11:22.3568541Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3573943Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:729:18:[m[K [01;31m[Kerror: [m[K'[01m[Kcsv_log_[m[K' was not declared in this scope
2025-11-09T02:11:22.3574918Z   729 |     auto error = [01;31m[Kcsv_log_[m[K.append(generate_log_filename());
2025-11-09T02:11:22.3575297Z       |                  [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.3590348Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:729:34:[m[K [01;31m[Kerror: [m[K'[01m[Kgenerate_log_filename[m[K' was not declared in this scope
2025-11-09T02:11:22.3591256Z   729 |     auto error = csv_log_.append([01;31m[Kgenerate_log_filename[m[K());
2025-11-09T02:11:22.3591674Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3605681Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:733:9:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-09T02:11:22.3607132Z   733 |         [01;31m[Kheader_written_[m[K = true;
2025-11-09T02:11:22.3607711Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3608737Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3609897Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:739:20:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3610888Z   739 | inline std::string [01;31m[KDroneScanner[m[K::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry& entry) {
2025-11-09T02:11:22.3611418Z       |                    [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3612291Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string format_csv_entry(const DetectionLogEntry&)[m[K':
2025-11-09T02:11:22.3613874Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:16:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.3615144Z   743 |              "[01;35m[K%u[m[K,%u,%d,%u,%u,%u,%.2f\n",
2025-11-09T02:11:22.3615445Z       |               [01;35m[K~^[m[K
2025-11-09T02:11:22.3615697Z       |                [01;35m[K|[m[K
2025-11-09T02:11:22.3615977Z       |                [01;35m[Kunsigned int[m[K
2025-11-09T02:11:22.3616286Z       |               [32m[K%lu[m[K
2025-11-09T02:11:22.3616974Z   744 |              [32m[Kentry.timestamp[m[K, entry.frequency_hz, entry.rssi_db,
2025-11-09T02:11:22.3617680Z       |              [32m[K~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3618037Z       |                    [32m[K|[m[K
2025-11-09T02:11:22.3618500Z       |                    [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-09T02:11:22.3619694Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:19:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.3620935Z   743 |              "%u,[01;35m[K%u[m[K,%d,%u,%u,%u,%.2f\n",
2025-11-09T02:11:22.3621236Z       |                  [01;35m[K~^[m[K
2025-11-09T02:11:22.3621496Z       |                   [01;35m[K|[m[K
2025-11-09T02:11:22.3621775Z       |                   [01;35m[Kunsigned int[m[K
2025-11-09T02:11:22.3622048Z       |                  [32m[K%lu[m[K
2025-11-09T02:11:22.3622426Z   744 |              entry.timestamp, [32m[Kentry.frequency_hz[m[K, entry.rssi_db,
2025-11-09T02:11:22.3622813Z       |                               [32m[K~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3623108Z       |                                     [32m[K|[m[K
2025-11-09T02:11:22.3623447Z       |                                     [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-09T02:11:22.3624861Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:22:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 6 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.3626618Z   743 |              "%u,%u,[01;35m[K%d[m[K,%u,%u,%u,%.2f\n",
2025-11-09T02:11:22.3627143Z       |                     [01;35m[K~^[m[K
2025-11-09T02:11:22.3627427Z       |                      [01;35m[K|[m[K
2025-11-09T02:11:22.3627697Z       |                      [01;35m[Kint[m[K
2025-11-09T02:11:22.3627966Z       |                     [32m[K%ld[m[K
2025-11-09T02:11:22.3628352Z   744 |              entry.timestamp, entry.frequency_hz, [32m[Kentry.rssi_db[m[K,
2025-11-09T02:11:22.3628755Z       |                                                   [32m[K~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3629081Z       |                                                         [32m[K|[m[K
2025-11-09T02:11:22.3629429Z       |                                                         [32m[Kint32_t {aka long int}[m[K
2025-11-09T02:11:22.3630042Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3631039Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:751:20:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3631926Z   751 | inline std::string [01;31m[KDroneScanner[m[K::DroneDetectionLogger::generate_log_filename() const {
2025-11-09T02:11:22.3632385Z       |                    [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3633378Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:751:80:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string generate_log_filename()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.3634422Z   751 | inline std::string DroneScanner::DroneDetectionLogger::generate_log_filename() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3635126Z       |                                                                                [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3635892Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:755:20:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-09T02:11:22.3636930Z   755 | inline std::string [01;31m[KDroneScanner[m[K::DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) const {
2025-11-09T02:11:22.3637526Z       |                    [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3638557Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:755:124:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string format_session_summary(size_t, size_t)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.3641092Z   755 | inline std::string DroneScanner::DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3641812Z       |                                                                                                                            [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3642693Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string format_session_summary(size_t, size_t)[m[K':
2025-11-09T02:11:22.3643878Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:756:50:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_start_[m[K' was not declared in this scope
2025-11-09T02:11:22.3644835Z   756 |     uint32_t session_duration_ms = chTimeNow() - [01;31m[Ksession_start_[m[K;
2025-11-09T02:11:22.3645274Z       |                                                  [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3656470Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:766:58:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3657409Z   766 |         avg_detections_per_cycle, detections_per_second, [01;31m[Klogged_count_[m[K);
2025-11-09T02:11:22.3657867Z       |                                                          [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3675364Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3677767Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:779:67:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3679528Z   779 | DroneHardwareController::DroneHardwareController(SpectrumMode mode[01;31m[K)[m[K
2025-11-09T02:11:22.3680402Z       |                                                                   [01;31m[K^[m[K
2025-11-09T02:11:22.3681472Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3682748Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3683538Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3683866Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3684852Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:785:51:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3685749Z   785 | DroneHardwareController::~DroneHardwareController([01;31m[K)[m[K {
2025-11-09T02:11:22.3686164Z       |                                                   [01;31m[K^[m[K
2025-11-09T02:11:22.3686771Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3687855Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3688609Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3688925Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3689772Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:789:51:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3690649Z   789 | void DroneHardwareController::initialize_hardware([01;31m[K)[m[K {
2025-11-09T02:11:22.3691252Z       |                                                   [01;31m[K^[m[K
2025-11-09T02:11:22.3691856Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3692945Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3693991Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3694591Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3696306Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:794:48:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3697234Z   794 | void DroneHardwareController::on_hardware_show([01;31m[K)[m[K {
2025-11-09T02:11:22.3697642Z       |                                                [01;31m[K^[m[K
2025-11-09T02:11:22.3698232Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3699490Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3700258Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3700574Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3701428Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:798:48:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3702301Z   798 | void DroneHardwareController::on_hardware_hide([01;31m[K)[m[K {
2025-11-09T02:11:22.3702694Z       |                                                [01;31m[K^[m[K
2025-11-09T02:11:22.3703285Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3704355Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3705282Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3705593Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3706424Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:802:49:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3707921Z   802 | void DroneHardwareController::shutdown_hardware([01;31m[K)[m[K {
2025-11-09T02:11:22.3708381Z       |                                                 [01;31m[K^[m[K
2025-11-09T02:11:22.3708988Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3710064Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3710821Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3711133Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3711994Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:807:54:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3713416Z   807 | void DroneHardwareController::initialize_radio_state([01;31m[K)[m[K {
2025-11-09T02:11:22.3713847Z       |                                                      [01;31m[K^[m[K
2025-11-09T02:11:22.3714803Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3715887Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3716645Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3716964Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3717978Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:816:61:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3719356Z   816 | void DroneHardwareController::initialize_spectrum_collector([01;31m[K)[m[K {
2025-11-09T02:11:22.3719811Z       |                                                             [01;31m[K^[m[K
2025-11-09T02:11:22.3720454Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3721685Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3722455Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3722773Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3723603Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:826:58:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3724508Z   826 | void DroneHardwareController::cleanup_spectrum_collector([01;31m[K)[m[K {
2025-11-09T02:11:22.3725106Z       |                                                          [01;31m[K^[m[K
2025-11-09T02:11:22.3725699Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3726764Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3727517Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3727818Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3728647Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:830:66:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3729574Z   830 | void DroneHardwareController::set_spectrum_mode(SpectrumMode mode[01;31m[K)[m[K {
2025-11-09T02:11:22.3730041Z       |                                                                  [01;31m[K^[m[K
2025-11-09T02:11:22.3730636Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3731692Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3732443Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3732746Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3733725Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:834:60:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3735228Z   834 | uint32_t DroneHardwareController::get_spectrum_bandwidth() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3735684Z       |                                                            [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3736449Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3737526Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3738279Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3738586Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3740070Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:838:75:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3741091Z   838 | void DroneHardwareController::set_spectrum_bandwidth(uint32_t bandwidth_hz[01;31m[K)[m[K {
2025-11-09T02:11:22.3741597Z       |                                                                           [01;31m[K^[m[K
2025-11-09T02:11:22.3742204Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3743416Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3744188Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3744496Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3745475Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:842:68:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3746420Z   842 | Frequency DroneHardwareController::get_spectrum_center_frequency() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3746921Z       |                                                                    [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3747520Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3748586Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3749338Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3749637Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3750466Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:846:82:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3751450Z   846 | void DroneHardwareController::set_spectrum_center_frequency(Frequency center_freq[01;31m[K)[m[K {
2025-11-09T02:11:22.3751976Z       |                                                                                  [01;31m[K^[m[K
2025-11-09T02:11:22.3752575Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3754249Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3755537Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3755850Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3756678Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:850:71:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3757619Z   850 | bool DroneHardwareController::tune_to_frequency(Frequency frequency_hz[01;31m[K)[m[K {
2025-11-09T02:11:22.3758307Z       |                                                                       [01;31m[K^[m[K
2025-11-09T02:11:22.3758908Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3759974Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3760722Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3761027Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3761852Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:855:56:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3762738Z   855 | void DroneHardwareController::start_spectrum_streaming([01;31m[K)[m[K {
2025-11-09T02:11:22.3763167Z       |                                                        [01;31m[K^[m[K
2025-11-09T02:11:22.3763753Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3765044Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3765805Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3766102Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3766930Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:861:55:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3767817Z   861 | void DroneHardwareController::stop_spectrum_streaming([01;31m[K)[m[K {
2025-11-09T02:11:22.3768244Z       |                                                       [01;31m[K^[m[K
2025-11-09T02:11:22.3768830Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3770217Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3771396Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3771710Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3772541Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:866:88:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3773546Z   866 | int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency[01;31m[K)[m[K {
2025-11-09T02:11:22.3774094Z       |                                                                                        [01;31m[K^[m[K
2025-11-09T02:11:22.3774807Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3775883Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3776640Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3776951Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3777780Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:871:111:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3778896Z   871 | void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message[01;31m[K)[m[K {
2025-11-09T02:11:22.3779707Z       |                                                                                                               [01;31m[K^[m[K
2025-11-09T02:11:22.3780318Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3781385Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3782138Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3782440Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3783268Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:875:92:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3784307Z   875 | void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum[01;31m[K)[m[K {
2025-11-09T02:11:22.3784998Z       |                                                                                            [01;31m[K^[m[K
2025-11-09T02:11:22.3785720Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3787057Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3788516Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3788983Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3789838Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:879:65:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3790788Z   879 | int32_t DroneHardwareController::get_configured_sampling_rate() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3791259Z       |                                                                 [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3791863Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3792933Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3793681Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3793996Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3794939Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:883:61:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3795870Z   883 | int32_t DroneHardwareController::get_configured_bandwidth() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3796330Z       |                                                             [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3796918Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3797980Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3798758Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3799064Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3799890Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:887:62:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3800957Z   887 | bool DroneHardwareController::is_spectrum_streaming_active() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3801414Z       |                                                              [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3801998Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3803057Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3803806Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3804106Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3805389Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:891:53:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3807018Z   891 | int32_t DroneHardwareController::get_current_rssi() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.3807568Z       |                                                     [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.3808316Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3809386Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3810146Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3810465Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3811290Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:895:59:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3812189Z   895 | void DroneHardwareController::update_spectrum_for_scanner([01;31m[K)[m[K {
2025-11-09T02:11:22.3812621Z       |                                                           [01;31m[K^[m[K
2025-11-09T02:11:22.3813212Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.3814275Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-09T02:11:22.3815176Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-09T02:11:22.3815486Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3816233Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:904:1:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' does not name a type
2025-11-09T02:11:22.3817081Z   904 | [01;31m[KSmartThreatHeader[m[K::SmartThreatHeader(Rect parent_rect)
2025-11-09T02:11:22.3817457Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3818217Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:906:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K)[m[K' token
2025-11-09T02:11:22.3819029Z   906 |       threat_progress_bar_({0, 0, screen_width, 16}[01;31m[K)[m[K,
2025-11-09T02:11:22.3819404Z       |                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.3820196Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:907:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.3821068Z   907 |       threat_status_main_({0, 20, screen_width, 16}[01;31m[K,[m[K "THREAT: LOW | â–²0 â– 0 â–¼0"),
2025-11-09T02:11:22.3821624Z       |                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.3822478Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:907:54:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.3823919Z   907 |       threat_status_main_({0, 20, screen_width, 16}, [01;31m[K"THREAT: LOW | â–²0 â– 0 â–¼0"[m[K),
2025-11-09T02:11:22.3824375Z       |                                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3825289Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:908:50:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.3826131Z   908 |       threat_frequency_({0, 38, screen_width, 16}[01;31m[K,[m[K "2400.0MHz SCANNING") {
2025-11-09T02:11:22.3826552Z       |                                                  [01;31m[K^[m[K
2025-11-09T02:11:22.3827316Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:908:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.3828122Z   908 |       threat_frequency_({0, 38, screen_width, 16}, [01;31m[K"2400.0MHz SCANNING"[m[K) {
2025-11-09T02:11:22.3828710Z       |                                                    [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3829496Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:913:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.3830477Z   913 | void [01;31m[KSmartThreatHeader[m[K::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
2025-11-09T02:11:22.3830969Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3831777Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update(ThreatLevel, size_t, size_t, size_t, Frequency, bool)[m[K':
2025-11-09T02:11:22.3833149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:915:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope; did you mean '[01m[Kcurrent_freq[m[K'?
2025-11-09T02:11:22.3833985Z   915 |     [01;31m[Kcurrent_threat_[m[K = max_threat;
2025-11-09T02:11:22.3834281Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3834545Z       |     [32m[Kcurrent_freq[m[K
2025-11-09T02:11:22.3835512Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:916:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope; did you mean '[01m[Kis_scanning[m[K'?
2025-11-09T02:11:22.3836321Z   916 |     [01;31m[Kis_scanning_[m[K = is_scanning;
2025-11-09T02:11:22.3836611Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3836871Z       |     [32m[Kis_scanning[m[K
2025-11-09T02:11:22.3837742Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:917:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope; did you mean '[01m[Kcurrent_freq[m[K'?
2025-11-09T02:11:22.3838936Z   917 |     [01;31m[Kcurrent_freq_[m[K = current_freq;
2025-11-09T02:11:22.3839497Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3839810Z       |     [32m[Kcurrent_freq[m[K
2025-11-09T02:11:22.3840728Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:918:5:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3841495Z   918 |     [01;31m[Kapproaching_count_[m[K = approaching;
2025-11-09T02:11:22.3841804Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3842685Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:919:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope; did you mean '[01m[Kstatic_count[m[K'?
2025-11-09T02:11:22.3843665Z   919 |     [01;31m[Kstatic_count_[m[K = static_count;
2025-11-09T02:11:22.3843950Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3844213Z       |     [32m[Kstatic_count[m[K
2025-11-09T02:11:22.3845081Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:920:5:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3845809Z   920 |     [01;31m[Kreceding_count_[m[K = receding;
2025-11-09T02:11:22.3846095Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3846874Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:923:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_progress_bar_[m[K' was not declared in this scope
2025-11-09T02:11:22.3847672Z   923 |     [01;31m[Kthreat_progress_bar_[m[K.set_value(total_drones * 10);
2025-11-09T02:11:22.3848024Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3848923Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:926:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_icon_text[m[K' was not declared in this scope
2025-11-09T02:11:22.3849761Z   926 |     std::string threat_name = [01;31m[Kget_threat_icon_text[m[K(max_threat);
2025-11-09T02:11:22.3850158Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3850954Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:936:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_status_main_[m[K' was not declared in this scope
2025-11-09T02:11:22.3851693Z   936 |     [01;31m[Kthreat_status_main_[m[K.set(buffer);
2025-11-09T02:11:22.3851999Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3852775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:955:9:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_frequency_[m[K' was not declared in this scope
2025-11-09T02:11:22.3853509Z   955 |         [01;31m[Kthreat_frequency_[m[K.set(buffer);
2025-11-09T02:11:22.3853823Z       |         [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3855431Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:957:9:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_frequency_[m[K' was not declared in this scope
2025-11-09T02:11:22.3856658Z   957 |         [01;31m[Kthreat_frequency_[m[K.set("NO SIGNAL");
2025-11-09T02:11:22.3856981Z       |         [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3857764Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:959:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_frequency_[m[K' was not declared in this scope
2025-11-09T02:11:22.3858605Z   959 |     [01;31m[Kthreat_frequency_[m[K.set_style(get_threat_text_color(max_threat));
2025-11-09T02:11:22.3858991Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3872312Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:959:33:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_text_color[m[K' was not declared in this scope
2025-11-09T02:11:22.3873234Z   959 |     threat_frequency_.set_style([01;31m[Kget_threat_text_color[m[K(max_threat));
2025-11-09T02:11:22.3873661Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3880743Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:960:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-09T02:11:22.3881716Z   960 |     [01;31m[Kset_dirty[m[K();
2025-11-09T02:11:22.3881988Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.3882754Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3883803Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:963:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.3884857Z   963 | void [01;31m[KSmartThreatHeader[m[K::set_max_threat(ThreatLevel threat) {
2025-11-09T02:11:22.3885259Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3895748Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_max_threat(ThreatLevel)[m[K':
2025-11-09T02:11:22.3897923Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:964:19:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-09T02:11:22.3899250Z   964 |     if (threat != [01;31m[Kcurrent_threat_[m[K) {
2025-11-09T02:11:22.3899816Z       |                   [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3914325Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:965:24:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3916364Z   965 |         update(threat, [01;31m[Kapproaching_count_[m[K, static_count_, receding_count_,
2025-11-09T02:11:22.3917133Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3929701Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:965:44:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3931251Z   965 |         update(threat, approaching_count_, [01;31m[Kstatic_count_[m[K, receding_count_,
2025-11-09T02:11:22.3932032Z       |                                            [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3946486Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:965:59:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.3947421Z   965 |         update(threat, approaching_count_, static_count_, [01;31m[Kreceding_count_[m[K,
2025-11-09T02:11:22.3947891Z       |                                                           [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3960170Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:966:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-09T02:11:22.3973364Z   966 |                [01;31m[Kcurrent_freq_[m[K, is_scanning_);
2025-11-09T02:11:22.3974023Z       |                [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3978179Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:966:31:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope
2025-11-09T02:11:22.3979576Z   966 |                current_freq_, [01;31m[Kis_scanning_[m[K);
2025-11-09T02:11:22.3980158Z       |                               [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3981194Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.3983047Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:970:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.3985030Z   970 | void [01;31m[KSmartThreatHeader[m[K::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
2025-11-09T02:11:22.3985924Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.3996895Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_movement_counts(size_t, size_t, size_t)[m[K':
2025-11-09T02:11:22.3998361Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:971:12:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-09T02:11:22.3999237Z   971 |     update([01;31m[Kcurrent_threat_[m[K, approaching, static_count, receding,
2025-11-09T02:11:22.3999641Z       |            [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4010092Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:972:12:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-09T02:11:22.4010890Z   972 |            [01;31m[Kcurrent_freq_[m[K, is_scanning_);
2025-11-09T02:11:22.4011238Z       |            [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4022342Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:972:27:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope
2025-11-09T02:11:22.4023176Z   972 |            current_freq_, [01;31m[Kis_scanning_[m[K);
2025-11-09T02:11:22.4023512Z       |                           [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4024264Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4025457Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:975:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.4026344Z   975 | void [01;31m[KSmartThreatHeader[m[K::set_current_frequency(Frequency freq) {
2025-11-09T02:11:22.4026747Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4035951Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_current_frequency(Frequency)[m[K':
2025-11-09T02:11:22.4037178Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:976:17:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-09T02:11:22.4037924Z   976 |     if (freq != [01;31m[Kcurrent_freq_[m[K) {
2025-11-09T02:11:22.4038246Z       |                 [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4050143Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-09T02:11:22.4051758Z   977 |         update([01;31m[Kcurrent_threat_[m[K, approaching_count_, static_count_, receding_count_,
2025-11-09T02:11:22.4052508Z       |                [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4067825Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:33:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.4068799Z   977 |         update(current_threat_, [01;31m[Kapproaching_count_[m[K, static_count_, receding_count_,
2025-11-09T02:11:22.4069279Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4080985Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:53:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.4081934Z   977 |         update(current_threat_, approaching_count_, [01;31m[Kstatic_count_[m[K, receding_count_,
2025-11-09T02:11:22.4082424Z       |                                                     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4095658Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:68:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.4097332Z   977 |         update(current_threat_, approaching_count_, static_count_, [01;31m[Kreceding_count_[m[K,
2025-11-09T02:11:22.4098458Z       |                                                                    [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4109865Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:978:22:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope
2025-11-09T02:11:22.4111240Z   978 |                freq, [01;31m[Kis_scanning_[m[K);
2025-11-09T02:11:22.4111785Z       |                      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4112818Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4114620Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:982:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.4116371Z   982 | void [01;31m[KSmartThreatHeader[m[K::set_scanning_state(bool is_scanning) {
2025-11-09T02:11:22.4117056Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4118331Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_scanning_state(bool)[m[K':
2025-11-09T02:11:22.4120920Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:983:24:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope; did you mean '[01m[Kis_scanning[m[K'?
2025-11-09T02:11:22.4122437Z   983 |     if (is_scanning != [01;31m[Kis_scanning_[m[K) {
2025-11-09T02:11:22.4122991Z       |                        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4123489Z       |                        [32m[Kis_scanning[m[K
2025-11-09T02:11:22.4134971Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-09T02:11:22.4136613Z   984 |         update([01;31m[Kcurrent_threat_[m[K, approaching_count_, static_count_, receding_count_,
2025-11-09T02:11:22.4137361Z       |                [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4152700Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:33:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.4154349Z   984 |         update(current_threat_, [01;31m[Kapproaching_count_[m[K, static_count_, receding_count_,
2025-11-09T02:11:22.4155359Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4168030Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:53:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.4169675Z   984 |         update(current_threat_, approaching_count_, [01;31m[Kstatic_count_[m[K, receding_count_,
2025-11-09T02:11:22.4170520Z       |                                                     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4184344Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:68:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.4185431Z   984 |         update(current_threat_, approaching_count_, static_count_, [01;31m[Kreceding_count_[m[K,
2025-11-09T02:11:22.4185944Z       |                                                                    [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4198299Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:985:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-09T02:11:22.4199460Z   985 |                [01;31m[Kcurrent_freq_[m[K, is_scanning);
2025-11-09T02:11:22.4200320Z       |                [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4201272Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4203161Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:989:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.4204323Z   989 | void [01;31m[KSmartThreatHeader[m[K::set_color_scheme(bool use_dark_theme) {
2025-11-09T02:11:22.4204878Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4206172Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:993:7:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.4207841Z   993 | Color [01;31m[KSmartThreatHeader[m[K::get_threat_bar_color(ThreatLevel level) const {
2025-11-09T02:11:22.4208339Z       |       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4209357Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:993:66:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_threat_bar_color(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.4210558Z   993 | Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.4211043Z       |                                                                  [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4211860Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1004:7:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.4212754Z  1004 | Color [01;31m[KSmartThreatHeader[m[K::get_threat_text_color(ThreatLevel level) const {
2025-11-09T02:11:22.4213190Z       |       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4214194Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1004:67:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_threat_text_color(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.4215370Z  1004 | Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.4215853Z       |                                                                   [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4216638Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1015:13:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.4217510Z  1015 | std::string [01;31m[KSmartThreatHeader[m[K::get_threat_icon_text(ThreatLevel level) const {
2025-11-09T02:11:22.4217953Z       |             [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4218951Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1015:72:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_threat_icon_text(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.4220151Z  1015 | std::string SmartThreatHeader::get_threat_icon_text(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.4221132Z       |                                                                        [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4222604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-09T02:11:22.4223935Z  1026 | void [01;31m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.4224304Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4225210Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-09T02:11:22.4226721Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1027:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-09T02:11:22.4227573Z  1027 |     View::paint(painter[01;31m[K)[m[K;
2025-11-09T02:11:22.4227875Z       |                        [01;31m[K^[m[K
2025-11-09T02:11:22.4228665Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1028:9:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-09T02:11:22.4229458Z  1028 |     if ([01;31m[Kcurrent_threat_[m[K >= ThreatLevel::HIGH) {
2025-11-09T02:11:22.4229784Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4230701Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1033:41:[m[K [01;31m[Kerror: [m[Kinvalid use of non-static member function '[01m[Kuint8_t ui::Color::r()[m[K'
2025-11-09T02:11:22.4231625Z  1033 |         pulse_color = Color([01;31m[Kpulse_color.r[m[K, pulse_color.g, pulse_color.b, alpha);
2025-11-09T02:11:22.4232058Z       |                             [01;31m[K~~~~~~~~~~~~^[m[K
2025-11-09T02:11:22.4232738Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:25[m[K,
2025-11-09T02:11:22.4234183Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:22[m[K,
2025-11-09T02:11:22.4235772Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.4236603Z [01m[K/havoc/firmware/common/ui.hpp:119:13:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-09T02:11:22.4237016Z   119 |     uint8_t [01;36m[Kr[m[K() {
2025-11-09T02:11:22.4237281Z       |             [01;36m[K^[m[K
2025-11-09T02:11:22.4238055Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1034:36:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect[m[K' was not declared in this scope
2025-11-09T02:11:22.4238978Z  1034 |         painter.fill_rectangle({0, [01;31m[Kparent_rect[m[K().top(), parent_rect().width(), 4}, pulse_color);
2025-11-09T02:11:22.4239454Z       |                                    [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4240471Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1034:95:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-09T02:11:22.4241493Z  1034 |         painter.fill_rectangle({0, parent_rect().top(), parent_rect().width(), 4}, pulse_color[01;31m[K)[m[K;
2025-11-09T02:11:22.4242027Z       |                                                                                               [01;31m[K^[m[K
2025-11-09T02:11:22.4242342Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-09T02:11:22.4242806Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-09T02:11:22.4243534Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-09T02:11:22.4244254Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-09T02:11:22.4244945Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.4245574Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-09T02:11:22.4245751Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-09T02:11:22.4245883Z       |                         [01;36m[K~~~~~^[m[K
2025-11-09T02:11:22.4246501Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4247081Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1038:1:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' does not name a type
2025-11-09T02:11:22.4247351Z  1038 | [01;31m[KThreatCard[m[K::ThreatCard(size_t card_index, Rect parent_rect)
2025-11-09T02:11:22.4247454Z       | [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4248030Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1043:6:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-09T02:11:22.4248304Z  1043 | void [01;31m[KThreatCard[m[K::update_card(const DisplayDroneEntry& drone) {
2025-11-09T02:11:22.4248410Z       |      [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4254894Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_card(const DisplayDroneEntry&)[m[K':
2025-11-09T02:11:22.4256075Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1044:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.4256551Z  1044 |     [01;31m[Kis_active_[m[K = true;
2025-11-09T02:11:22.4256759Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4262624Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1045:5:[m[K [01;31m[Kerror: [m[K'[01m[Kfrequency_[m[K' was not declared in this scope; did you mean '[01m[KFrequency[m[K'?
2025-11-09T02:11:22.4262803Z  1045 |     [01;31m[Kfrequency_[m[K = drone.frequency;
2025-11-09T02:11:22.4262916Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4263030Z       |     [32m[KFrequency[m[K
2025-11-09T02:11:22.4269018Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1046:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-09T02:11:22.4269298Z  1046 |     [01;31m[Kthreat_[m[K = drone.threat;
2025-11-09T02:11:22.4269487Z       |     [01;31m[K^~~~~~~[m[K
2025-11-09T02:11:22.4275147Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1047:5:[m[K [01;31m[Kerror: [m[K'[01m[Krssi_[m[K' was not declared in this scope
2025-11-09T02:11:22.4275394Z  1047 |     [01;31m[Krssi_[m[K = drone.rssi;
2025-11-09T02:11:22.4275575Z       |     [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4286254Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1048:5:[m[K [01;31m[Kerror: [m[K'[01m[Klast_seen_[m[K' was not declared in this scope
2025-11-09T02:11:22.4286529Z  1048 |     [01;31m[Klast_seen_[m[K = drone.last_seen;
2025-11-09T02:11:22.4286718Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4298486Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1049:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_name_[m[K' was not declared in this scope
2025-11-09T02:11:22.4298785Z  1049 |     [01;31m[Kthreat_name_[m[K = drone.type_name;
2025-11-09T02:11:22.4298989Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4305225Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1050:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktrend_[m[K' was not declared in this scope
2025-11-09T02:11:22.4305592Z  1050 |     [01;31m[Ktrend_[m[K = MovementTrend::STATIC;
2025-11-09T02:11:22.4305774Z       |     [01;31m[K^~~~~~[m[K
2025-11-09T02:11:22.4316203Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1052:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcard_text_[m[K' was not declared in this scope
2025-11-09T02:11:22.4316544Z  1052 |     [01;31m[Kcard_text_[m[K.set(render_compact());
2025-11-09T02:11:22.4316964Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4330807Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1052:20:[m[K [01;31m[Kerror: [m[K'[01m[Krender_compact[m[K' was not declared in this scope
2025-11-09T02:11:22.4331123Z  1052 |     card_text_.set([01;31m[Krender_compact[m[K());
2025-11-09T02:11:22.4331346Z       |                    [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4343914Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1053:26:[m[K [01;31m[Kerror: [m[K'[01m[Kget_card_text_color[m[K' was not declared in this scope; did you mean '[01m[Kget_threat_text_color[m[K'?
2025-11-09T02:11:22.4344287Z  1053 |     card_text_.set_style([01;31m[Kget_card_text_color[m[K());
2025-11-09T02:11:22.4344532Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4344976Z       |                          [32m[Kget_threat_text_color[m[K
2025-11-09T02:11:22.4353005Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1054:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-09T02:11:22.4353432Z  1054 |     [01;31m[Kset_dirty[m[K();
2025-11-09T02:11:22.4353627Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.4354379Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4355652Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1057:6:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-09T02:11:22.4355997Z  1057 | void [01;31m[KThreatCard[m[K::clear_card() {
2025-11-09T02:11:22.4356181Z       |      [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4365118Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid clear_card()[m[K':
2025-11-09T02:11:22.4366251Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1058:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.4366485Z  1058 |     [01;31m[Kis_active_[m[K = false;
2025-11-09T02:11:22.4366668Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4376509Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1059:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcard_text_[m[K' was not declared in this scope
2025-11-09T02:11:22.4376665Z  1059 |     [01;31m[Kcard_text_[m[K.set("");
2025-11-09T02:11:22.4376779Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4384976Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1060:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-09T02:11:22.4385224Z  1060 |     [01;31m[Kset_dirty[m[K();
2025-11-09T02:11:22.4385411Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.4386162Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4387234Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1063:13:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-09T02:11:22.4387606Z  1063 | std::string [01;31m[KThreatCard[m[K::render_compact() const {
2025-11-09T02:11:22.4387805Z       |             [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4389267Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1063:42:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string render_compact()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.4389648Z  1063 | std::string ThreatCard::render_compact() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.4390108Z       |                                          [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4398018Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string render_compact()[m[K':
2025-11-09T02:11:22.4399183Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1064:10:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.4399458Z  1064 |     if (![01;31m[Kis_active_[m[K) return "";
2025-11-09T02:11:22.4399658Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4405985Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1067:33:[m[K [01;31m[Kerror: [m[K'[01m[Ktrend_[m[K' was not declared in this scope
2025-11-09T02:11:22.4407522Z  1067 |     const char* trend_symbol = ([01;31m[Ktrend_[m[K == MovementTrend::APPROACHING) ? "â–²" :
2025-11-09T02:11:22.4407779Z       |                                 [01;31m[K^~~~~~[m[K
2025-11-09T02:11:22.4413965Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1069:32:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-09T02:11:22.4414940Z  1069 |     const char* threat_abbr = ([01;31m[Kthreat_[m[K == ThreatLevel::CRITICAL) ? "CRIT" :
2025-11-09T02:11:22.4415212Z       |                                [01;31m[K^~~~~~~[m[K
2025-11-09T02:11:22.4422094Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1074:41:[m[K [01;31m[Kerror: [m[K'[01m[Kfrequency_[m[K' was not declared in this scope; did you mean '[01m[KFrequency[m[K'?
2025-11-09T02:11:22.4422565Z  1074 |     float freq_mhz = static_cast<float>([01;31m[Kfrequency_[m[K) / 1000000.0f;
2025-11-09T02:11:22.4422822Z       |                                         [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4423084Z       |                                         [32m[KFrequency[m[K
2025-11-09T02:11:22.4432918Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1078:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_name_[m[K' was not declared in this scope; did you mean '[01m[Kthreat_abbr[m[K'?
2025-11-09T02:11:22.4433194Z  1078 |                 [01;31m[Kthreat_name_[m[K.c_str(), *trend_symbol, freq_mhz, rssi_);
2025-11-09T02:11:22.4433321Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4433446Z       |                 [32m[Kthreat_abbr[m[K
2025-11-09T02:11:22.4437910Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1078:64:[m[K [01;31m[Kerror: [m[K'[01m[Krssi_[m[K' was not declared in this scope
2025-11-09T02:11:22.4438169Z  1078 |                 threat_name_.c_str(), *trend_symbol, freq_mhz, [01;31m[Krssi_[m[K);
2025-11-09T02:11:22.4438332Z       |                                                                [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4448030Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1081:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_name_[m[K' was not declared in this scope; did you mean '[01m[Kthreat_abbr[m[K'?
2025-11-09T02:11:22.4448293Z  1081 |                 [01;31m[Kthreat_name_[m[K.c_str(), *trend_symbol, freq_mhz, rssi_);
2025-11-09T02:11:22.4448420Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4448548Z       |                 [32m[Kthreat_abbr[m[K
2025-11-09T02:11:22.4453313Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1081:64:[m[K [01;31m[Kerror: [m[K'[01m[Krssi_[m[K' was not declared in this scope
2025-11-09T02:11:22.4453575Z  1081 |                 threat_name_.c_str(), *trend_symbol, freq_mhz, [01;31m[Krssi_[m[K);
2025-11-09T02:11:22.4453740Z       |                                                                [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4462369Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1069:17:[m[K [01;35m[Kwarning: [m[Kunused variable '[01m[Kthreat_abbr[m[K' [[01;35m[K-Wunused-variable[m[K]
2025-11-09T02:11:22.4462863Z  1069 |     const char* [01;35m[Kthreat_abbr[m[K = (threat_ == ThreatLevel::CRITICAL) ? "CRIT" :
2025-11-09T02:11:22.4463004Z       |                 [01;35m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4463441Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4464061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1086:7:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-09T02:11:22.4464299Z  1086 | Color [01;31m[KThreatCard[m[K::get_card_bg_color() const {
2025-11-09T02:11:22.4464411Z       |       [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4465358Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1086:39:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_card_bg_color()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.4465799Z  1086 | Color ThreatCard::get_card_bg_color() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.4465947Z       |                                       [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4473280Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_bg_color()[m[K':
2025-11-09T02:11:22.4473932Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1087:10:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.4474150Z  1087 |     if (![01;31m[Kis_active_[m[K) return Color::black();
2025-11-09T02:11:22.4474270Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4480317Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1088:13:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-09T02:11:22.4480569Z  1088 |     switch ([01;31m[Kthreat_[m[K) {
2025-11-09T02:11:22.4480765Z       |             [01;31m[K^~~~~~~[m[K
2025-11-09T02:11:22.4483366Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4484097Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1098:7:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-09T02:11:22.4484361Z  1098 | Color [01;31m[KThreatCard[m[K::get_card_text_color() const {
2025-11-09T02:11:22.4484472Z       |       [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4485419Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1098:41:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_card_text_color()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.4485655Z  1098 | Color ThreatCard::get_card_text_color() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.4485799Z       |                                         [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4492932Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_text_color()[m[K':
2025-11-09T02:11:22.4493584Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1099:10:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.4493800Z  1099 |     if (![01;31m[Kis_active_[m[K) return Color::white();
2025-11-09T02:11:22.4493914Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4499604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1100:13:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-09T02:11:22.4500129Z  1100 |     switch ([01;31m[Kthreat_[m[K) {
2025-11-09T02:11:22.4500345Z       |             [01;31m[K^~~~~~~[m[K
2025-11-09T02:11:22.4501630Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4502747Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1110:6:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-09T02:11:22.4503167Z  1110 | void [01;31m[KThreatCard[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.4503351Z       |      [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4504630Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1110:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-09T02:11:22.4506092Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-09T02:11:22.4506519Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.4506935Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4508046Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-09T02:11:22.4509592Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1111:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-09T02:11:22.4509843Z  1111 |     View::paint(painter[01;31m[K)[m[K;
2025-11-09T02:11:22.4510051Z       |                        [01;31m[K^[m[K
2025-11-09T02:11:22.4516189Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1112:9:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.4516463Z  1112 |     if ([01;31m[Kis_active_[m[K) {
2025-11-09T02:11:22.4516669Z       |         [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.4529149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1114:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope
2025-11-09T02:11:22.4530441Z  1114 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect().top(), parent_rect_.width(), 2}, bg_color);
2025-11-09T02:11:22.4530696Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4541384Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1114:54:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect[m[K' was not declared in this scope
2025-11-09T02:11:22.4541813Z  1114 |         painter.fill_rectangle({parent_rect_.left(), [01;31m[Kparent_rect[m[K().top(), parent_rect_.width(), 2}, bg_color);
2025-11-09T02:11:22.4541979Z       |                                                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4542855Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1114:109:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-09T02:11:22.4543257Z  1114 |         painter.fill_rectangle({parent_rect_.left(), parent_rect().top(), parent_rect_.width(), 2}, bg_color[01;31m[K)[m[K;
2025-11-09T02:11:22.4543453Z       |                                                                                                             [01;31m[K^[m[K
2025-11-09T02:11:22.4543704Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-09T02:11:22.4544123Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-09T02:11:22.4544516Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-09T02:11:22.4545068Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-09T02:11:22.4545449Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.4546057Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-09T02:11:22.4546236Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-09T02:11:22.4546370Z       |                         [01;36m[K~~~~~^[m[K
2025-11-09T02:11:22.4557082Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4557827Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1118:1:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' does not name a type
2025-11-09T02:11:22.4558346Z  1118 | [01;31m[KConsoleStatusBar[m[K::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
2025-11-09T02:11:22.4558468Z       | [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4559080Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1124:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-09T02:11:22.4559573Z  1124 | void [01;31m[KConsoleStatusBar[m[K::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
2025-11-09T02:11:22.4559688Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4560320Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_scanning_progress(uint32_t, uint32_t, uint32_t)[m[K':
2025-11-09T02:11:22.4560910Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1125:22:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-09T02:11:22.4561134Z  1125 |     set_display_mode([01;31m[KDisplayMode[m[K::SCANNING);
2025-11-09T02:11:22.4561262Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4573987Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1125:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_display_mode[m[K' was not declared in this scope
2025-11-09T02:11:22.4574449Z  1125 |     [01;31m[Kset_display_mode[m[K(DisplayMode::SCANNING);
2025-11-09T02:11:22.4576020Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4577192Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1127:28:[m[K [01;31m[Kerror: [m[Kinitializer-string for array of chars is too long [[01;31m[K-fpermissive[m[K]
2025-11-09T02:11:22.4577414Z  1127 |     char progress_bar[9] = [01;31m[K"â–‘â–‘â–‘â–‘â–‘â–‘â–‘â–‘"[m[K;
2025-11-09T02:11:22.4577578Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4578526Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1130:27:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849672[m[K' to '[01m[K'\210'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-09T02:11:22.4578681Z  1130 |         progress_bar[i] = [01;35m[K'â–ˆ'[m[K;
2025-11-09T02:11:22.4578812Z       |                           [01;35m[K^~~~~[m[K
2025-11-09T02:11:22.4579831Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1134:43:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.4580256Z  1134 |     snprintf(buffer, sizeof(buffer), "%s [01;35m[K%u[m[K%% C:%u D:%u",
2025-11-09T02:11:22.4580406Z       |                                          [01;35m[K~^[m[K
2025-11-09T02:11:22.4580536Z       |                                           [01;35m[K|[m[K
2025-11-09T02:11:22.4580686Z       |                                           [01;35m[Kunsigned int[m[K
2025-11-09T02:11:22.4580823Z       |                                          [32m[K%lu[m[K
2025-11-09T02:11:22.4581070Z  1135 |             progress_bar, [32m[Kprogress_percent[m[K, total_cycles, detections);
2025-11-09T02:11:22.4581202Z       |                           [32m[K~~~~~~~~~~~~~~~~[m[K 
2025-11-09T02:11:22.4581322Z       |                           [32m[K|[m[K
2025-11-09T02:11:22.4581495Z       |                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-09T02:11:22.4582598Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1134:50:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 6 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.4582829Z  1134 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:[01;35m[K%u[m[K D:%u",
2025-11-09T02:11:22.4582974Z       |                                                 [01;35m[K~^[m[K
2025-11-09T02:11:22.4583111Z       |                                                  [01;35m[K|[m[K
2025-11-09T02:11:22.4583266Z       |                                                  [01;35m[Kunsigned int[m[K
2025-11-09T02:11:22.4583409Z       |                                                 [32m[K%lu[m[K
2025-11-09T02:11:22.4583660Z  1135 |             progress_bar, progress_percent, [32m[Ktotal_cycles[m[K, detections);
2025-11-09T02:11:22.4583800Z       |                                             [32m[K~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4583937Z       |                                             [32m[K|[m[K
2025-11-09T02:11:22.4584124Z       |                                             [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-09T02:11:22.4585266Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1134:55:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 7 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.4585498Z  1134 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:[01;35m[K%u[m[K",
2025-11-09T02:11:22.4585642Z       |                                                      [01;35m[K~^[m[K
2025-11-09T02:11:22.4585789Z       |                                                       [01;35m[K|[m[K
2025-11-09T02:11:22.4585957Z       |                                                       [01;35m[Kunsigned int[m[K
2025-11-09T02:11:22.4586231Z       |                                                      [32m[K%lu[m[K
2025-11-09T02:11:22.4586745Z  1135 |             progress_bar, progress_percent, total_cycles, [32m[Kdetections[m[K);
2025-11-09T02:11:22.4587055Z       |                                                           [32m[K~~~~~~~~~~[m[K
2025-11-09T02:11:22.4587333Z       |                                                           [32m[K|[m[K
2025-11-09T02:11:22.4587698Z       |                                                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-09T02:11:22.4589206Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1136:5:[m[K [01;31m[Kerror: [m[K'[01m[Kprogress_text_[m[K' was not declared in this scope; did you mean '[01m[Kprogress_bar[m[K'?
2025-11-09T02:11:22.4589667Z  1136 |     [01;31m[Kprogress_text_[m[K.set(buffer);
2025-11-09T02:11:22.4589788Z       |     [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4589904Z       |     [32m[Kprogress_bar[m[K
2025-11-09T02:11:22.4590522Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1140:26:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-09T02:11:22.4590769Z  1140 |         set_display_mode([01;31m[KDisplayMode[m[K::ALERT);
2025-11-09T02:11:22.4590902Z       |                          [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4591906Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1141:61:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.4592281Z  1141 |         snprintf(buffer, sizeof(buffer), "âš ï¸� DETECTED: [01;35m[K%u[m[K threats found!", [32m[Kdetections[m[K);
2025-11-09T02:11:22.4592611Z       |                                                            [01;35m[K~^[m[K                  [32m[K~~~~~~~~~~[m[K
2025-11-09T02:11:22.4592804Z       |                                                             [01;35m[K|[m[K                  [32m[K|[m[K
2025-11-09T02:11:22.4593084Z       |                                                             [01;35m[Kunsigned int[m[K       [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-09T02:11:22.4593237Z       |                                                            [32m[K%lu[m[K
2025-11-09T02:11:22.4601183Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1142:9:[m[K [01;31m[Kerror: [m[K'[01m[Kalert_text_[m[K' was not declared in this scope
2025-11-09T02:11:22.4601399Z  1142 |         [01;31m[Kalert_text_[m[K.set(buffer);
2025-11-09T02:11:22.4601514Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4610198Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1145:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-09T02:11:22.4610477Z  1145 |     [01;31m[Kset_dirty[m[K();
2025-11-09T02:11:22.4610682Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.4611381Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4612030Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1148:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-09T02:11:22.4612518Z  1148 | void [01;31m[KConsoleStatusBar[m[K::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
2025-11-09T02:11:22.4612641Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4613293Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_alert_status(ThreatLevel, size_t, const string&)[m[K':
2025-11-09T02:11:22.4613889Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1149:22:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-09T02:11:22.4614103Z  1149 |     set_display_mode([01;31m[KDisplayMode[m[K::ALERT);
2025-11-09T02:11:22.4614237Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4627642Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1149:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_display_mode[m[K' was not declared in this scope
2025-11-09T02:11:22.4627875Z  1149 |     [01;31m[Kset_display_mode[m[K(DisplayMode::ALERT);
2025-11-09T02:11:22.4628184Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4640325Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1158:5:[m[K [01;31m[Kerror: [m[K'[01m[Kalert_text_[m[K' was not declared in this scope
2025-11-09T02:11:22.4640502Z  1158 |     [01;31m[Kalert_text_[m[K.set(buffer);
2025-11-09T02:11:22.4640617Z       |     [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4641217Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1159:126:[m[K [01;31m[Kerror: [m[Kexpression cannot be used as a function
2025-11-09T02:11:22.4641691Z  1159 |     alert_text_.set_style((threat >= ThreatLevel::CRITICAL) ? Theme::getInstance()->fg_red : Theme::getInstance()->fg_yellow([01;31m[K)[m[K);
2025-11-09T02:11:22.4641899Z       |                                                                                                                              [01;31m[K^[m[K
2025-11-09T02:11:22.4649285Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1160:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-09T02:11:22.4649558Z  1160 |     [01;31m[Kset_dirty[m[K();
2025-11-09T02:11:22.4649757Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.4650533Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4651177Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1163:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-09T02:11:22.4651615Z  1163 | void [01;31m[KConsoleStatusBar[m[K::update_normal_status(const std::string& primary, const std::string& secondary) {
2025-11-09T02:11:22.4651732Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4652371Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_normal_status(const string&, const string&)[m[K':
2025-11-09T02:11:22.4652970Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1164:22:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-09T02:11:22.4653192Z  1164 |     set_display_mode([01;31m[KDisplayMode[m[K::NORMAL);
2025-11-09T02:11:22.4653325Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4666385Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1164:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_display_mode[m[K' was not declared in this scope
2025-11-09T02:11:22.4666631Z  1164 |     [01;31m[Kset_display_mode[m[K(DisplayMode::NORMAL);
2025-11-09T02:11:22.4666751Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4679802Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1172:5:[m[K [01;31m[Kerror: [m[K'[01m[Knormal_text_[m[K' was not declared in this scope
2025-11-09T02:11:22.4679979Z  1172 |     [01;31m[Knormal_text_[m[K.set(buffer);
2025-11-09T02:11:22.4680092Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4688352Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1174:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-09T02:11:22.4688624Z  1174 |     [01;31m[Kset_dirty[m[K();
2025-11-09T02:11:22.4688838Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.4689637Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4690804Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1177:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-09T02:11:22.4691364Z  1177 | void [01;31m[KConsoleStatusBar[m[K::set_display_mode(DisplayMode mode) {
2025-11-09T02:11:22.4691805Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4692485Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1177:41:[m[K [01;31m[Kerror: [m[Kvariable or field '[01m[Kset_display_mode[m[K' declared void
2025-11-09T02:11:22.4692763Z  1177 | void ConsoleStatusBar::set_display_mode([01;31m[KDisplayMode[m[K mode) {
2025-11-09T02:11:22.4692908Z       |                                         [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4694061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1177:41:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::DisplayMode[m[K'?
2025-11-09T02:11:22.4694568Z  1177 | void ConsoleStatusBar::set_display_mode([01;31m[KDisplayMode[m[K mode) {
2025-11-09T02:11:22.4695014Z       |                                         [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4695558Z       |                                         [32m[Kui::external_app::enhanced_drone_analyzer::DisplayMode[m[K
2025-11-09T02:11:22.4696303Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.4697124Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:527:12:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DisplayMode[m[K' declared here
2025-11-09T02:11:22.4697351Z   527 | enum class [01;36m[KDisplayMode[m[K { SCANNING, ALERT, NORMAL };
2025-11-09T02:11:22.4697471Z       |            [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4698090Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1193:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-09T02:11:22.4698336Z  1193 | void [01;31m[KConsoleStatusBar[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.4698450Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4699134Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1193:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-09T02:11:22.4699811Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-09T02:11:22.4700045Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.4700156Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4700737Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-09T02:11:22.4701566Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1194:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-09T02:11:22.4701706Z  1194 |     View::paint(painter[01;31m[K)[m[K;
2025-11-09T02:11:22.4701830Z       |                        [01;31m[K^[m[K
2025-11-09T02:11:22.4702525Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1195:9:[m[K [01;31m[Kerror: [m[K'[01m[Kmode_[m[K' was not declared in this scope; did you mean '[01m[Kmode_t[m[K'?
2025-11-09T02:11:22.4702718Z  1195 |     if ([01;31m[Kmode_[m[K == DisplayMode::ALERT) {
2025-11-09T02:11:22.4702829Z       |         [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.4702938Z       |         [32m[Kmode_t[m[K
2025-11-09T02:11:22.4703516Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1195:18:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-09T02:11:22.4703827Z  1195 |     if (mode_ == [01;31m[KDisplayMode[m[K::ALERT) {
2025-11-09T02:11:22.4703950Z       |                  [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4708266Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1196:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope
2025-11-09T02:11:22.4708703Z  1196 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect().width(), 2}, Color(32, 0, 0));
2025-11-09T02:11:22.4708844Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4719703Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1196:74:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect[m[K' was not declared in this scope
2025-11-09T02:11:22.4720174Z  1196 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), [01;31m[Kparent_rect[m[K().width(), 2}, Color(32, 0, 0));
2025-11-09T02:11:22.4720357Z       |                                                                          [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4721370Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1196:116:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-09T02:11:22.4721804Z  1196 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, Color(32, 0, 0)[01;31m[K)[m[K;
2025-11-09T02:11:22.4721999Z       |                                                                                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.4722248Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-09T02:11:22.4722494Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-09T02:11:22.4722893Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-09T02:11:22.4723272Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-09T02:11:22.4723648Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.4724247Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-09T02:11:22.4724423Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-09T02:11:22.4724554Z       |                         [01;36m[K~~~~~^[m[K
2025-11-09T02:11:22.4733744Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4735467Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1200:1:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' does not name a type; did you mean '[01m[KDroneHardwareController[m[K'?
2025-11-09T02:11:22.4736102Z  1200 | [01;31m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-09T02:11:22.4736326Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4736525Z       | [32m[KDroneHardwareController[m[K
2025-11-09T02:11:22.4737168Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:38:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4737489Z  1201 |     : nav_(nav), spectrum_gradient_{}[01;31m[K,[m[K big_display_({0, 0, screen_width, 32}, "2400.0MHz"),
2025-11-09T02:11:22.4737629Z       |                                      [01;31m[K^[m[K
2025-11-09T02:11:22.4738511Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:52:[m[K [01;31m[Kerror: [m[Kexpected constructor, destructor, or type conversion before '[01m[K([m[K' token
2025-11-09T02:11:22.4738836Z  1201 |     : nav_(nav), spectrum_gradient_{}, big_display_[01;31m[K([m[K{0, 0, screen_width, 32}, "2400.0MHz"),
2025-11-09T02:11:22.4738989Z       |                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.4739632Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:77:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4739953Z  1201 |     : nav_(nav), spectrum_gradient_{}, big_display_({0, 0, screen_width, 32}[01;31m[K,[m[K "2400.0MHz"),
2025-11-09T02:11:22.4740128Z       |                                                                             [01;31m[K^[m[K
2025-11-09T02:11:22.4740727Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:79:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.4741035Z  1201 |     : nav_(nav), spectrum_gradient_{}, big_display_({0, 0, screen_width, 32}, [01;31m[K"2400.0MHz"[m[K),
2025-11-09T02:11:22.4741325Z       |                                                                               [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.4741933Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1202:53:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4742222Z  1202 |       text_threat_summary_({0, 32, screen_width, 16}[01;31m[K,[m[K "THREAT: NONE | All clear"),
2025-11-09T02:11:22.4742370Z       |                                                     [01;31m[K^[m[K
2025-11-09T02:11:22.4742953Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1202:55:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.4743237Z  1202 |       text_threat_summary_({0, 32, screen_width, 16}, [01;31m[K"THREAT: NONE | All clear"[m[K),
2025-11-09T02:11:22.4743411Z       |                                                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4744421Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1203:50:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4745164Z  1203 |       text_status_info_({0, 48, screen_width, 16}[01;31m[K,[m[K "Ready - Enhanced Drone Analyzer"),
2025-11-09T02:11:22.4745473Z       |                                                  [01;31m[K^[m[K
2025-11-09T02:11:22.4746378Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1203:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.4746697Z  1203 |       text_status_info_({0, 48, screen_width, 16}, [01;31m[K"Ready - Enhanced Drone Analyzer"[m[K),
2025-11-09T02:11:22.4746883Z       |                                                    [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4747502Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1204:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4747777Z  1204 |       text_scanner_stats_({0, 64, screen_width, 16}[01;31m[K,[m[K "No database loaded"),
2025-11-09T02:11:22.4747925Z       |                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.4748526Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1204:54:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.4748803Z  1204 |       text_scanner_stats_({0, 64, screen_width, 16}, [01;31m[K"No database loaded"[m[K),
2025-11-09T02:11:22.4749247Z       |                                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4749859Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1205:51:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K)[m[K' token
2025-11-09T02:11:22.4750065Z  1205 |       scanning_progress_({0, 80, screen_width, 16}[01;31m[K)[m[K,
2025-11-09T02:11:22.4750206Z       |                                                   [01;31m[K^[m[K
2025-11-09T02:11:22.4750805Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1206:45:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4750989Z  1206 |       text_drone_1({0, 96, screen_width, 16}[01;31m[K,[m[K ""),
2025-11-09T02:11:22.4751134Z       |                                             [01;31m[K^[m[K
2025-11-09T02:11:22.4751738Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1206:47:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.4751931Z  1206 |       text_drone_1({0, 96, screen_width, 16}, [01;31m[K""[m[K),
2025-11-09T02:11:22.4752179Z       |                                               [01;31m[K^~[m[K
2025-11-09T02:11:22.4752776Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1207:46:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4752963Z  1207 |       text_drone_2({0, 112, screen_width, 16}[01;31m[K,[m[K ""),
2025-11-09T02:11:22.4753104Z       |                                              [01;31m[K^[m[K
2025-11-09T02:11:22.4753679Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1207:48:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.4753866Z  1207 |       text_drone_2({0, 112, screen_width, 16}, [01;31m[K""[m[K),
2025-11-09T02:11:22.4754006Z       |                                                [01;31m[K^~[m[K
2025-11-09T02:11:22.4754598Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1208:46:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.4754912Z  1208 |       text_drone_3({0, 128, screen_width, 16}[01;31m[K,[m[K ""),
2025-11-09T02:11:22.4755053Z       |                                              [01;31m[K^[m[K
2025-11-09T02:11:22.4755626Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1208:48:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.4755808Z  1208 |       text_drone_3({0, 128, screen_width, 16}, [01;31m[K""[m[K),
2025-11-09T02:11:22.4755950Z       |                                                [01;31m[K^~[m[K
2025-11-09T02:11:22.4756561Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1241:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.4756940Z  1241 | void [01;31m[KDroneDisplayController[m[K::update_detection_display(const DroneScanner& scanner) {
2025-11-09T02:11:22.4757067Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4758071Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1241:61:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-09T02:11:22.4758809Z  1241 | void DroneDisplayController::update_detection_display(const [01;31m[KDroneScanner[m[K& scanner) {
2025-11-09T02:11:22.4759105Z       |                                                             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4760018Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_detection_display(const int&)[m[K':
2025-11-09T02:11:22.4760862Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1242:17:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4761047Z  1242 |     if (scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-09T02:11:22.4761175Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4762048Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1243:42:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_current_scanning_frequency[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4762332Z  1243 |         Frequency current_freq = scanner.[01;31m[Kget_current_scanning_frequency[m[K();
2025-11-09T02:11:22.4762503Z       |                                          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4763235Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1245:13:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4763469Z  1245 |             [01;31m[Kbig_display_[m[K.set(to_string_short_freq(current_freq));
2025-11-09T02:11:22.4763587Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4772139Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1247:13:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4772338Z  1247 |             [01;31m[Kbig_display_[m[K.set("2400.0MHz");
2025-11-09T02:11:22.4772459Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4785250Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1250:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4785436Z  1250 |         [01;31m[Kbig_display_[m[K.set("READY");
2025-11-09T02:11:22.4785560Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4786409Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1253:34:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_database_size[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4786636Z  1253 |     size_t total_freqs = scanner.[01;31m[Kget_database_size[m[K();
2025-11-09T02:11:22.4786776Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4787606Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1254:36:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4787832Z  1254 |     if (total_freqs > 0 && scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-09T02:11:22.4787978Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4801917Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1256:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_progress_[m[K' was not declared in this scope; did you mean '[01m[Kupdate_scanning_progress[m[K'?
2025-11-09T02:11:22.4802293Z  1256 |         [01;31m[Kscanning_progress_[m[K.set_value(std::min(progress_percent, (uint32_t)100));
2025-11-09T02:11:22.4802417Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4802564Z       |         [32m[Kupdate_scanning_progress[m[K
2025-11-09T02:11:22.4818105Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1258:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_progress_[m[K' was not declared in this scope; did you mean '[01m[Kupdate_scanning_progress[m[K'?
2025-11-09T02:11:22.4818742Z  1258 |         [01;31m[Kscanning_progress_[m[K.set_value(0);
2025-11-09T02:11:22.4818961Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4819194Z       |         [32m[Kupdate_scanning_progress[m[K
2025-11-09T02:11:22.4820746Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1261:38:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_max_detected_threat[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4821202Z  1261 |     ThreatLevel max_threat = scanner.[01;31m[Kget_max_detected_threat[m[K();
2025-11-09T02:11:22.4821461Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4823000Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1262:36:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_approaching_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4823941Z  1262 |     bool has_detections = (scanner.[01;31m[Kget_approaching_count[m[K() + scanner.get_receding_count() + scanner.get_static_count()) > 0;
2025-11-09T02:11:22.4824200Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4825891Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1262:70:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_receding_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4826706Z  1262 |     bool has_detections = (scanner.get_approaching_count() + scanner.[01;31m[Kget_receding_count[m[K() + scanner.get_static_count()) > 0;
2025-11-09T02:11:22.4827018Z       |                                                                      [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4828525Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1262:101:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_static_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4829360Z  1262 |     bool has_detections = (scanner.get_approaching_count() + scanner.get_receding_count() + scanner.[01;31m[Kget_static_count[m[K()) > 0;
2025-11-09T02:11:22.4829723Z       |                                                                                                     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4840263Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1267:17:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_level_name[m[K' was not declared in this scope
2025-11-09T02:11:22.4840787Z  1267 |                 [01;31m[Kget_threat_level_name[m[K(max_threat), scanner.get_approaching_count(),
2025-11-09T02:11:22.4841012Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4842558Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1267:60:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_approaching_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4843069Z  1267 |                 get_threat_level_name(max_threat), scanner.[01;31m[Kget_approaching_count[m[K(),
2025-11-09T02:11:22.4843363Z       |                                                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4845059Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1268:25:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_static_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4845728Z  1268 |                 scanner.[01;31m[Kget_static_count[m[K(), scanner.get_receding_count());
2025-11-09T02:11:22.4845958Z       |                         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4847476Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1268:53:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_receding_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4847929Z  1268 |                 scanner.get_static_count(), scanner.[01;31m[Kget_receding_count[m[K());
2025-11-09T02:11:22.4848207Z       |                                                     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4860993Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1269:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_threat_summary_[m[K' was not declared in this scope
2025-11-09T02:11:22.4861252Z  1269 |         [01;31m[Ktext_threat_summary_[m[K.set(summary_buffer);
2025-11-09T02:11:22.4861407Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4878090Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1272:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_threat_summary_[m[K' was not declared in this scope
2025-11-09T02:11:22.4878546Z  1272 |         [01;31m[Ktext_threat_summary_[m[K.set("THREAT: NONE | All clear");
2025-11-09T02:11:22.4878762Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4879983Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1277:17:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4880168Z  1277 |     if (scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-09T02:11:22.4880311Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4881153Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1278:40:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_real_mode[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4881435Z  1278 |         std::string mode_str = scanner.[01;31m[Kis_real_mode[m[K() ? "REAL" : "DEMO";
2025-11-09T02:11:22.4881582Z       |                                        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4882424Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1280:43:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_total_detections[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4882651Z  1280 |                 mode_str.c_str(), scanner.[01;31m[Kget_total_detections[m[K());
2025-11-09T02:11:22.4882808Z       |                                           [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4896553Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1284:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_status_info_[m[K' was not declared in this scope
2025-11-09T02:11:22.4896872Z  1284 |     [01;31m[Ktext_status_info_[m[K.set(status_buffer);
2025-11-09T02:11:22.4897059Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4898552Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1286:35:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_database_size[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4898943Z  1286 |     size_t loaded_freqs = scanner.[01;31m[Kget_database_size[m[K();
2025-11-09T02:11:22.4899190Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4900946Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1288:17:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4901345Z  1288 |     if (scanner.[01;31m[Kis_scanning_active[m[K() && loaded_freqs > 0) {
2025-11-09T02:11:22.4901577Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4904277Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1291:56:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_scan_cycles[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4904927Z  1291 |                 current_idx + 1, loaded_freqs, scanner.[01;31m[Kget_scan_cycles[m[K());
2025-11-09T02:11:22.4905224Z       |                                                        [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4917451Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1297:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_scanner_stats_[m[K' was not declared in this scope
2025-11-09T02:11:22.4917974Z  1297 |     [01;31m[Ktext_scanner_stats_[m[K.set(stats_buffer);
2025-11-09T02:11:22.4918188Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4931261Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1300:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4931768Z  1300 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_red);
2025-11-09T02:11:22.4931971Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4944504Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1302:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4945218Z  1302 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_yellow);
2025-11-09T02:11:22.4945422Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4958612Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1304:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4959120Z  1304 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_orange);
2025-11-09T02:11:22.4959322Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4960858Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1305:24:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-09T02:11:22.4961191Z  1305 |     } else if (scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-09T02:11:22.4961432Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4973262Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1306:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4973766Z  1306 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_green);
2025-11-09T02:11:22.4973963Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4986609Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1308:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-09T02:11:22.4987112Z  1308 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->bg_darkest);
2025-11-09T02:11:22.4987315Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.4988065Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.4989444Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1312:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.4990314Z  1312 | void [01;31m[KDroneDisplayController[m[K::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) {
2025-11-09T02:11:22.4990521Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5003875Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)[m[K':
2025-11-09T02:11:22.5005523Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1314:28:[m[K [01;31m[Kerror: [m[K'[01m[Kdetected_drones_[m[K' was not declared in this scope; did you mean '[01m[Kadd_detected_drone[m[K'?
2025-11-09T02:11:22.5006037Z  1314 |     auto it = std::find_if([01;31m[Kdetected_drones_[m[K.begin(), detected_drones_.end(),
2025-11-09T02:11:22.5006276Z       |                            [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5006726Z       |                            [32m[Kadd_detected_drone[m[K
2025-11-09T02:11:22.5023951Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1323:25:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-09T02:11:22.5024235Z  1323 |         it->type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-09T02:11:22.5024407Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5040841Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1324:29:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-09T02:11:22.5041314Z  1324 |         it->display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-09T02:11:22.5041595Z       |                             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5043722Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.5045074Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K':
2025-11-09T02:11:22.5046798Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::frequency[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5047055Z   130 | struct [01;35m[KDisplayDroneEntry[m[K {
2025-11-09T02:11:22.5047253Z       |        [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5048886Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5050522Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::threat[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5052126Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::rssi[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5053772Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::last_seen[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5055829Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type_name[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5057539Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::display_color[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5059259Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::trend[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-09T02:11:22.5060511Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)[m[K':
2025-11-09T02:11:22.5062189Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1327:31:[m[K [01;36m[Knote: [m[Ksynthesized method '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K' first required here
2025-11-09T02:11:22.5062479Z  1327 |             DisplayDroneEntry [01;36m[Kentry[m[K;
2025-11-09T02:11:22.5062702Z       |                               [01;36m[K^~~~~[m[K
2025-11-09T02:11:22.5071609Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1333:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-09T02:11:22.5072002Z  1333 |             entry.type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-09T02:11:22.5072246Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5088412Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1334:35:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-09T02:11:22.5088839Z  1334 |             entry.display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-09T02:11:22.5089092Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5105438Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1339:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_drones_display[m[K' was not declared in this scope; did you mean '[01m[Kupdate_detection_display[m[K'?
2025-11-09T02:11:22.5105728Z  1339 |     [01;31m[Kupdate_drones_display[m[K();
2025-11-09T02:11:22.5105930Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5106157Z       |     [32m[Kupdate_detection_display[m[K
2025-11-09T02:11:22.5106907Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5108051Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1342:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5108520Z  1342 | void [01;31m[KDroneDisplayController[m[K::sort_drones_by_rssi() {
2025-11-09T02:11:22.5108726Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5121349Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid sort_drones_by_rssi()[m[K':
2025-11-09T02:11:22.5122705Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1343:15:[m[K [01;31m[Kerror: [m[K'[01m[Kdetected_drones_[m[K' was not declared in this scope; did you mean '[01m[Kadd_detected_drone[m[K'?
2025-11-09T02:11:22.5123678Z  1343 |     std::sort([01;31m[Kdetected_drones_[m[K.begin(), detected_drones_.end(),
2025-11-09T02:11:22.5123897Z       |               [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5124143Z       |               [32m[Kadd_detected_drone[m[K
2025-11-09T02:11:22.5124597Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5125420Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1351:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5125802Z  1351 | void [01;31m[KDroneDisplayController[m[K::update_drones_display(const DroneScanner& scanner) {
2025-11-09T02:11:22.5125927Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5134541Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1351:58:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-09T02:11:22.5135115Z  1351 | void DroneDisplayController::update_drones_display(const [01;31m[KDroneScanner[m[K& scanner) {
2025-11-09T02:11:22.5135281Z       |                                                          [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5149007Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_drones_display(const int&)[m[K':
2025-11-09T02:11:22.5150471Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1354:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdetected_drones_[m[K' was not declared in this scope; did you mean '[01m[Kadd_detected_drone[m[K'?
2025-11-09T02:11:22.5150717Z  1354 |     [01;31m[Kdetected_drones_[m[K.erase(
2025-11-09T02:11:22.5150915Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5151120Z       |     [32m[Kadd_detected_drone[m[K
2025-11-09T02:11:22.5167700Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1361:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplayed_drones_[m[K' was not declared in this scope
2025-11-09T02:11:22.5167982Z  1361 |     [01;31m[Kdisplayed_drones_[m[K.clear();
2025-11-09T02:11:22.5168185Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5181388Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1366:5:[m[K [01;31m[Kerror: [m[K'[01m[Khighlight_threat_zones_in_spectrum[m[K' was not declared in this scope
2025-11-09T02:11:22.5181818Z  1366 |     [01;31m[Khighlight_threat_zones_in_spectrum[m[K(displayed_drones_);
2025-11-09T02:11:22.5182058Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5198525Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1367:5:[m[K [01;31m[Kerror: [m[K'[01m[Krender_drone_text_display[m[K' was not declared in this scope
2025-11-09T02:11:22.5198839Z  1367 |     [01;31m[Krender_drone_text_display[m[K();
2025-11-09T02:11:22.5199051Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5200339Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1351:72:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Kscanner[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-09T02:11:22.5200983Z  1351 | void DroneDisplayController::update_drones_display([01;35m[Kconst DroneScanner& scanner[m[K) {
2025-11-09T02:11:22.5201280Z       |                                                    [01;35m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~[m[K
2025-11-09T02:11:22.5202017Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5203161Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1370:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5203870Z  1370 | void [01;31m[KDroneDisplayController[m[K::render_drone_text_display() {
2025-11-09T02:11:22.5204078Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5214916Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid render_drone_text_display()[m[K':
2025-11-09T02:11:22.5215587Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1371:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_drone_1[m[K' was not declared in this scope
2025-11-09T02:11:22.5215728Z  1371 |     [01;31m[Ktext_drone_1[m[K.set("");
2025-11-09T02:11:22.5215841Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5226633Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1372:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_drone_2[m[K' was not declared in this scope
2025-11-09T02:11:22.5226808Z  1372 |     [01;31m[Ktext_drone_2[m[K.set("");
2025-11-09T02:11:22.5226918Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5239092Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1373:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_drone_3[m[K' was not declared in this scope
2025-11-09T02:11:22.5239256Z  1373 |     [01;31m[Ktext_drone_3[m[K.set("");
2025-11-09T02:11:22.5239364Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5256409Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1374:37:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplayed_drones_[m[K' was not declared in this scope
2025-11-09T02:11:22.5256767Z  1374 |     for (size_t i = 0; i < std::min([01;31m[Kdisplayed_drones_[m[K.size(), size_t(3)); ++i) {
2025-11-09T02:11:22.5257055Z       |                                     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5258027Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1379:61:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849714[m[K' to '[01m[K'\262'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-09T02:11:22.5258328Z  1379 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'â–²'[m[K; break;
2025-11-09T02:11:22.5258489Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-09T02:11:22.5259426Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1380:58:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849724[m[K' to '[01m[K'\274'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-09T02:11:22.5259695Z  1380 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'â–¼'[m[K; break;
2025-11-09T02:11:22.5259857Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-09T02:11:22.5260779Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1383:37:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849696[m[K' to '[01m[K'\240'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-09T02:11:22.5260963Z  1383 |             default: trend_symbol = [01;35m[K'â– '[m[K; break;
2025-11-09T02:11:22.5261097Z       |                                     [01;35m[K^~~~~[m[K
2025-11-09T02:11:22.5268783Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1398:30:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_level_color[m[K' was not declared in this scope; did you mean '[01m[Kget_threat_text_color[m[K'?
2025-11-09T02:11:22.5269210Z  1398 |         Color threat_color = [01;31m[Kget_threat_level_color[m[K(drone.threat);
2025-11-09T02:11:22.5269553Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5269707Z       |                              [32m[Kget_threat_text_color[m[K
2025-11-09T02:11:22.5270137Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5270805Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1416:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5271113Z  1416 | void [01;31m[KDroneDisplayController[m[K::initialize_mini_spectrum() {
2025-11-09T02:11:22.5271239Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5284549Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid initialize_mini_spectrum()[m[K':
2025-11-09T02:11:22.5285450Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1417:10:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-09T02:11:22.5285709Z  1417 |     if (![01;31m[Kspectrum_gradient_[m[K.load_file(default_gradient_file)) {
2025-11-09T02:11:22.5285994Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5301853Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1420:5:[m[K [01;31m[Kerror: [m[K'[01m[Kclear_spectrum_buffers[m[K' was not declared in this scope
2025-11-09T02:11:22.5302028Z  1420 |     [01;31m[Kclear_spectrum_buffers[m[K();
2025-11-09T02:11:22.5302148Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5302567Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5303198Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1423:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5303616Z  1423 | void [01;31m[KDroneDisplayController[m[K::process_mini_spectrum_data(const ChannelSpectrum& spectrum) {
2025-11-09T02:11:22.5303747Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5317228Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid process_mini_spectrum_data(const ChannelSpectrum&)[m[K':
2025-11-09T02:11:22.5318478Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1426:9:[m[K [01;31m[Kerror: [m[K'[01m[Kget_max_power_for_current_bin[m[K' was not declared in this scope
2025-11-09T02:11:22.5318956Z  1426 |         [01;31m[Kget_max_power_for_current_bin[m[K(spectrum, bin, current_bin_power);
2025-11-09T02:11:22.5319190Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5331362Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1427:13:[m[K [01;31m[Kerror: [m[K'[01m[Kprocess_bins[m[K' was not declared in this scope
2025-11-09T02:11:22.5331570Z  1427 |         if ([01;31m[Kprocess_bins[m[K(&current_bin_power)) {
2025-11-09T02:11:22.5331698Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5332118Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5332759Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1433:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5333080Z  1433 | bool [01;31m[KDroneDisplayController[m[K::process_bins(uint8_t* powerlevel) {
2025-11-09T02:11:22.5333204Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5343711Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool process_bins(uint8_t*)[m[K':
2025-11-09T02:11:22.5344551Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1434:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbins_hz_size[m[K' was not declared in this scope
2025-11-09T02:11:22.5344868Z  1434 |     [01;31m[Kbins_hz_size[m[K += each_bin_size;
2025-11-09T02:11:22.5344985Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5356920Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1434:21:[m[K [01;31m[Kerror: [m[K'[01m[Keach_bin_size[m[K' was not declared in this scope
2025-11-09T02:11:22.5357100Z  1434 |     bins_hz_size += [01;31m[Keach_bin_size[m[K;
2025-11-09T02:11:22.5357233Z       |                     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5373417Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1435:25:[m[K [01;31m[Kerror: [m[K'[01m[Kmarker_pixel_step[m[K' was not declared in this scope
2025-11-09T02:11:22.5373630Z  1435 |     if (bins_hz_size >= [01;31m[Kmarker_pixel_step[m[K) {
2025-11-09T02:11:22.5373769Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5387842Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1436:27:[m[K [01;31m[Kerror: [m[K'[01m[Kmin_color_power[m[K' was not declared in this scope
2025-11-09T02:11:22.5388060Z  1436 |         if (*powerlevel > [01;31m[Kmin_color_power[m[K)
2025-11-09T02:11:22.5388199Z       |                           [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5403673Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1437:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel[m[K' was not declared in this scope
2025-11-09T02:11:22.5403872Z  1437 |             [01;31m[Kadd_spectrum_pixel[m[K(*powerlevel);
2025-11-09T02:11:22.5404015Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5418890Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1439:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel[m[K' was not declared in this scope
2025-11-09T02:11:22.5419063Z  1439 |             [01;31m[Kadd_spectrum_pixel[m[K(0);
2025-11-09T02:11:22.5419195Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5430377Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1442:14:[m[K [01;31m[Kerror: [m[K'[01m[Kpixel_index[m[K' was not declared in this scope
2025-11-09T02:11:22.5430534Z  1442 |         if (![01;31m[Kpixel_index[m[K) {
2025-11-09T02:11:22.5430661Z       |              [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.5431093Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5431747Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1451:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5432292Z  1451 | void [01;31m[KDroneDisplayController[m[K::get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power) {
2025-11-09T02:11:22.5432415Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5436144Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t, uint8_t&)[m[K':
2025-11-09T02:11:22.5436859Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1452:9:[m[K [01;31m[Kerror: [m[K'[01m[Kmode[m[K' was not declared in this scope; did you mean '[01m[Kmodf[m[K'?
2025-11-09T02:11:22.5437039Z  1452 |     if ([01;31m[Kmode[m[K == LOOKING_GLASS_SINGLEPASS) {
2025-11-09T02:11:22.5437381Z       |         [01;31m[K^~~~[m[K
2025-11-09T02:11:22.5437484Z       |         [32m[Kmodf[m[K
2025-11-09T02:11:22.5451596Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1452:17:[m[K [01;31m[Kerror: [m[K'[01m[KLOOKING_GLASS_SINGLEPASS[m[K' was not declared in this scope
2025-11-09T02:11:22.5451803Z  1452 |     if (mode == [01;31m[KLOOKING_GLASS_SINGLEPASS[m[K) {
2025-11-09T02:11:22.5451939Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5464233Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1454:29:[m[K [01;31m[Kerror: [m[K'[01m[KSPEC_NB_BINS[m[K' was not declared in this scope
2025-11-09T02:11:22.5464515Z  1454 |             if (spectrum.db[[01;31m[KSPEC_NB_BINS[m[K - 120 + bin] > max_power)
2025-11-09T02:11:22.5464795Z       |                             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5468513Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5469363Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1471:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5469705Z  1471 | void [01;31m[KDroneDisplayController[m[K::add_spectrum_pixel(uint8_t power) {
2025-11-09T02:11:22.5469830Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5485679Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid add_spectrum_pixel(uint8_t)[m[K':
2025-11-09T02:11:22.5486364Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1472:10:[m[K [01;31m[Kerror: [m[K'[01m[Kvalidate_spectrum_data[m[K' was not declared in this scope
2025-11-09T02:11:22.5486542Z  1472 |     if (![01;31m[Kvalidate_spectrum_data[m[K()) {
2025-11-09T02:11:22.5486671Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5503081Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1473:9:[m[K [01;31m[Kerror: [m[K'[01m[Kclear_spectrum_buffers[m[K' was not declared in this scope
2025-11-09T02:11:22.5503375Z  1473 |         [01;31m[Kclear_spectrum_buffers[m[K();
2025-11-09T02:11:22.5503594Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5515031Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1476:9:[m[K [01;31m[Kerror: [m[K'[01m[Kpixel_index[m[K' was not declared in this scope
2025-11-09T02:11:22.5515383Z  1476 |     if ([01;31m[Kpixel_index[m[K < spectrum_row.size()) {
2025-11-09T02:11:22.5515578Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.5524264Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1476:23:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_row[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-09T02:11:22.5524627Z  1476 |     if (pixel_index < [01;31m[Kspectrum_row[m[K.size()) {
2025-11-09T02:11:22.5525017Z       |                       [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5525246Z       |                       [32m[Kspectrum_dir[m[K
2025-11-09T02:11:22.5540939Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1477:29:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-09T02:11:22.5541319Z  1477 |         Color pixel_color = [01;31m[Kspectrum_gradient_[m[K.lut[
2025-11-09T02:11:22.5541559Z       |                             [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5558075Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1480:32:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.5558576Z  1480 |         for (size_t i = 0; i < [01;31m[Kthreat_bins_count_[m[K; i++) {
2025-11-09T02:11:22.5558720Z       |                                [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5569105Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1481:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_[m[K' was not declared in this scope
2025-11-09T02:11:22.5569317Z  1481 |             if ([01;31m[Kthreat_bins_[m[K[i].bin == pixel_index) {
2025-11-09T02:11:22.5569444Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5579044Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1482:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_level_color[m[K' was not declared in this scope; did you mean '[01m[Kget_threat_text_color[m[K'?
2025-11-09T02:11:22.5579350Z  1482 |                 pixel_color = [01;31m[Kget_threat_level_color[m[K(threat_bins_[i].threat);
2025-11-09T02:11:22.5579494Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5579791Z       |                               [32m[Kget_threat_text_color[m[K
2025-11-09T02:11:22.5580221Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5580859Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1491:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5581155Z  1491 | void [01;31m[KDroneDisplayController[m[K::render_mini_spectrum() {
2025-11-09T02:11:22.5581280Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5595593Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid render_mini_spectrum()[m[K':
2025-11-09T02:11:22.5596284Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1492:10:[m[K [01;31m[Kerror: [m[K'[01m[Kvalidate_spectrum_data[m[K' was not declared in this scope
2025-11-09T02:11:22.5596455Z  1492 |     if (![01;31m[Kvalidate_spectrum_data[m[K()) {
2025-11-09T02:11:22.5611564Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5612397Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1493:9:[m[K [01;31m[Kerror: [m[K'[01m[Kclear_spectrum_buffers[m[K' was not declared in this scope
2025-11-09T02:11:22.5612559Z  1493 |         [01;31m[Kclear_spectrum_buffers[m[K();
2025-11-09T02:11:22.5612682Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5627478Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1496:36:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-09T02:11:22.5627977Z  1496 |     const Color background_color = [01;31m[Kspectrum_gradient_[m[K.lut.size() > 0 ? spectrum_gradient_.lut[0] : Color::black();
2025-11-09T02:11:22.5628133Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5636685Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1497:15:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_row[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-09T02:11:22.5637196Z  1497 |     std::fill([01;31m[Kspectrum_row[m[K.begin(), spectrum_row.end(), background_color);
2025-11-09T02:11:22.5637413Z       |               [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5637622Z       |               [32m[Kspectrum_dir[m[K
2025-11-09T02:11:22.5649371Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1498:9:[m[K [01;31m[Kerror: [m[K'[01m[Kpixel_index[m[K' was not declared in this scope
2025-11-09T02:11:22.5649881Z  1498 |     if ([01;31m[Kpixel_index[m[K > 0) {
2025-11-09T02:11:22.5650074Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.5650827Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5651992Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1507:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5652979Z  1507 | void [01;31m[KDroneDisplayController[m[K::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES>& drones) {
2025-11-09T02:11:22.5653185Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5668644Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, 3>&)[m[K':
2025-11-09T02:11:22.5669307Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1508:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_count_[m[K' was not declared in this scope
2025-11-09T02:11:22.5669615Z  1508 |     [01;31m[Kthreat_bins_count_[m[K = 0;
2025-11-09T02:11:22.5669746Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5686312Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1511:28:[m[K [01;31m[Kerror: [m[K'[01m[Kfrequency_to_spectrum_bin[m[K' was not declared in this scope
2025-11-09T02:11:22.5686772Z  1511 |             size_t bin_x = [01;31m[Kfrequency_to_spectrum_bin[m[K(drone.frequency);
2025-11-09T02:11:22.5687038Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5698918Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1513:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_[m[K' was not declared in this scope
2025-11-09T02:11:22.5699298Z  1513 |                 [01;31m[Kthreat_bins_[m[K[threat_bins_count_].bin = bin_x;
2025-11-09T02:11:22.5699439Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5699872Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5700519Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1521:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5700815Z  1521 | void [01;31m[KDroneDisplayController[m[K::clear_spectrum_buffers() {
2025-11-09T02:11:22.5700938Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5716343Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid clear_spectrum_buffers()[m[K':
2025-11-09T02:11:22.5717035Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1522:15:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_power_levels_[m[K' was not declared in this scope
2025-11-09T02:11:22.5717352Z  1522 |     std::fill([01;31m[Kspectrum_power_levels_[m[K.begin(), spectrum_power_levels_.end(), 0);
2025-11-09T02:11:22.5717492Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5717922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5718562Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1525:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5718872Z  1525 | bool [01;31m[KDroneDisplayController[m[K::validate_spectrum_data() const {
2025-11-09T02:11:22.5719183Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5719922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1525:55:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kbool validate_spectrum_data()[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.5720209Z  1525 | bool DroneDisplayController::validate_spectrum_data() [01;31m[Kconst[m[K {
2025-11-09T02:11:22.5720361Z       |                                                       [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.5733843Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool validate_spectrum_data()[m[K':
2025-11-09T02:11:22.5734518Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1526:9:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_power_levels_[m[K' was not declared in this scope
2025-11-09T02:11:22.5734931Z  1526 |     if ([01;31m[Kspectrum_power_levels_[m[K.size() != MINI_SPECTRUM_WIDTH) return false;
2025-11-09T02:11:22.5735069Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5750023Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1527:9:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-09T02:11:22.5750435Z  1527 |     if ([01;31m[Kspectrum_gradient_[m[K.lut.empty()) return false;
2025-11-09T02:11:22.5750669Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5751224Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5751913Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1531:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5752294Z  1531 | size_t [01;31m[KDroneDisplayController[m[K::get_safe_spectrum_index(size_t x, size_t y) const {
2025-11-09T02:11:22.5752431Z       |        [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5753232Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1531:76:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Ksize_t get_safe_spectrum_index(size_t, size_t)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.5753574Z  1531 | size_t DroneDisplayController::get_safe_spectrum_index(size_t x, size_t y) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.5753750Z       |                                                                            [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.5754368Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1538:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5754883Z  1538 | void [01;31m[KDroneDisplayController[m[K::set_spectrum_range(Frequency min_freq, Frequency max_freq) {
2025-11-09T02:11:22.5755014Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5765581Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_spectrum_range(Frequency, Frequency)[m[K':
2025-11-09T02:11:22.5766385Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1540:9:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_config_[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-09T02:11:22.5766610Z  1540 |         [01;31m[Kspectrum_config_[m[K.min_freq = WIDEBAND_DEFAULT_MIN;
2025-11-09T02:11:22.5766730Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5766855Z       |         [32m[Kspectrum_dir[m[K
2025-11-09T02:11:22.5780281Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1544:5:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_config_[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-09T02:11:22.5780721Z  1544 |     [01;31m[Kspectrum_config_[m[K.min_freq = min_freq;
2025-11-09T02:11:22.5780843Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5780959Z       |     [32m[Kspectrum_dir[m[K
2025-11-09T02:11:22.5781384Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5782031Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1551:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5782408Z  1551 | size_t [01;31m[KDroneDisplayController[m[K::frequency_to_spectrum_bin(Frequency freq_hz) const {
2025-11-09T02:11:22.5782533Z       |        [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5783320Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1551:77:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Ksize_t frequency_to_spectrum_bin(Frequency)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.5783668Z  1551 | size_t DroneDisplayController::frequency_to_spectrum_bin(Frequency freq_hz) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.5783950Z       |                                                                             [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.5794324Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Ksize_t frequency_to_spectrum_bin(Frequency)[m[K':
2025-11-09T02:11:22.5795480Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1552:32:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_config_[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-09T02:11:22.5795959Z  1552 |     const Frequency MIN_FREQ = [01;31m[Kspectrum_config_[m[K.min_freq;
2025-11-09T02:11:22.5796240Z       |                                [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5796435Z       |                                [32m[Kspectrum_dir[m[K
2025-11-09T02:11:22.5797228Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5797999Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1563:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5798350Z  1563 | std::string [01;31m[KDroneDisplayController[m[K::get_threat_level_name(ThreatLevel level) const {
2025-11-09T02:11:22.5798481Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5799669Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1563:78:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_threat_level_name(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.5800223Z  1563 | std::string DroneDisplayController::get_threat_level_name(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.5800562Z       |                                                                              [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.5801546Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1574:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5802176Z  1574 | std::string [01;31m[KDroneDisplayController[m[K::get_drone_type_name(DroneType type) const {
2025-11-09T02:11:22.5802442Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5804068Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1574:73:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_drone_type_name(DroneType)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.5805095Z  1574 | std::string DroneDisplayController::get_drone_type_name(DroneType type) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.5805287Z       |                                                                         [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.5806106Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:7:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5806479Z  1582 | Color [01;31m[KDroneDisplayController[m[K::get_drone_type_color(DroneType type) const {
2025-11-09T02:11:22.5806610Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5807459Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:68:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_drone_type_color(DroneType)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.5807780Z  1582 | Color DroneDisplayController::get_drone_type_color(DroneType type) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.5807951Z       |                                                                    [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.5808719Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1590:7:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-09T02:11:22.5809066Z  1590 | Color [01;31m[KDroneDisplayController[m[K::get_threat_level_color(ThreatLevel level) const {
2025-11-09T02:11:22.5809191Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5810023Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1590:73:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_threat_level_color(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-09T02:11:22.5810359Z  1590 | Color DroneDisplayController::get_threat_level_color(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-09T02:11:22.5810525Z       |                                                                         [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.5822296Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1601:1:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' does not name a type; did you mean '[01m[KDroneHardwareController[m[K'?
2025-11-09T02:11:22.5822612Z  1601 | [01;31m[KDroneUIController[m[K::DroneUIController(NavigationView& nav,
2025-11-09T02:11:22.5822727Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5822861Z       | [32m[KDroneHardwareController[m[K
2025-11-09T02:11:22.5823477Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1614:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.5823689Z  1614 | void [01;31m[KDroneUIController[m[K::on_start_scan() {
2025-11-09T02:11:22.5823804Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5838302Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_start_scan()[m[K':
2025-11-09T02:11:22.5838990Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1615:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.5839159Z  1615 |     if ([01;31m[Kscanning_active_[m[K) return;
2025-11-09T02:11:22.5839282Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5854530Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1616:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.5854839Z  1616 |     [01;31m[Kscanning_active_[m[K = true;
2025-11-09T02:11:22.5854965Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5862659Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1617:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.5863009Z  1617 |     [01;31m[Kscanner_[m[K.start_scanning();
2025-11-09T02:11:22.5863122Z       |     [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.5879997Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1618:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.5880297Z  1618 |     [01;31m[Kdisplay_controller_[m[K->set_scanning_status(true, "Scanning Active");
2025-11-09T02:11:22.5880419Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5880838Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5881467Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1622:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.5881726Z  1622 | void [01;31m[KDroneUIController[m[K::on_stop_scan() {
2025-11-09T02:11:22.5881846Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5896134Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_stop_scan()[m[K':
2025-11-09T02:11:22.5897344Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1623:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.5897607Z  1623 |     [01;31m[Kscanning_active_[m[K = false;
2025-11-09T02:11:22.5897805Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5905235Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1624:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.5905532Z  1624 |     [01;31m[Kscanner_[m[K.stop_scanning();
2025-11-09T02:11:22.5905720Z       |     [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.5910502Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1625:5:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-09T02:11:22.5910746Z  1625 |     [01;31m[Kaudio_[m[K.stop_audio();
2025-11-09T02:11:22.5910918Z       |     [01;31m[K^~~~~~[m[K
2025-11-09T02:11:22.5911091Z       |     [32m[Kaudio[m[K
2025-11-09T02:11:22.5928531Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1626:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.5928991Z  1626 |     [01;31m[Kdisplay_controller_[m[K->set_scanning_status(false, "Stopped");
2025-11-09T02:11:22.5929199Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5929967Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.5931099Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1630:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.5931510Z  1630 | void [01;31m[KDroneUIController[m[K::on_toggle_mode() {
2025-11-09T02:11:22.5931719Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5938533Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_toggle_mode()[m[K':
2025-11-09T02:11:22.5939655Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1631:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.5939929Z  1631 |     if ([01;31m[Kscanner_[m[K.is_real_mode()) {
2025-11-09T02:11:22.5940329Z       |         [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.5949395Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1633:13:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.5949779Z  1633 |         if ([01;31m[Khardware_[m[K.is_spectrum_streaming_active()) {
2025-11-09T02:11:22.5949977Z       |             [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.5959414Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1638:14:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.5959787Z  1638 |         if (![01;31m[Khardware_[m[K.is_spectrum_streaming_active()) {
2025-11-09T02:11:22.5959986Z       |              [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.5977313Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1642:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.5977770Z  1642 |     [01;31m[Kdisplay_controller_[m[K->set_scanning_status(scanning_active_,
2025-11-09T02:11:22.5977967Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.5994282Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1642:46:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.5995492Z  1642 |     display_controller_->set_scanning_status([01;31m[Kscanning_active_[m[K,
2025-11-09T02:11:22.5995775Z       |                                              [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6003621Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1643:44:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.6004104Z  1643 |                                            [01;31m[Kscanner_[m[K.is_real_mode() ? "Real Mode" : "Demo Mode");
2025-11-09T02:11:22.6004390Z       |                                            [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.6005322Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6006467Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1646:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6006874Z  1646 | void [01;31m[KDroneUIController[m[K::show_menu() {
2025-11-09T02:11:22.6007097Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6010168Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-09T02:11:22.6011281Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:22:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6011815Z  1647 |     auto menu_view = [01;31m[Knav_[m[K.push<MenuView>(std::vector<MenuView::Item>{
2025-11-09T02:11:22.6012021Z       |                      [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6013197Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:40:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-09T02:11:22.6013725Z  1647 |     auto menu_view = nav_.push<MenuView[01;31m[K>[m[K(std::vector<MenuView::Item>{
2025-11-09T02:11:22.6013954Z       |                                        [01;31m[K^[m[K
2025-11-09T02:11:22.6015452Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:64:[m[K [01;31m[Kerror: [m[K'[01m[KItem[m[K' is not a member of '[01m[Kui::MenuView[m[K'
2025-11-09T02:11:22.6015936Z  1647 |     auto menu_view = nav_.push<MenuView>(std::vector<MenuView::[01;31m[KItem[m[K>{
2025-11-09T02:11:22.6016427Z       |                                                                [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6017494Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:68:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6017966Z  1647 |     auto menu_view = nav_.push<MenuView>(std::vector<MenuView::Item[01;31m[K>[m[K{
2025-11-09T02:11:22.6018239Z       |                                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.6019227Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:68:[m[K [01;31m[Kerror: [m[Ktemplate argument 2 is invalid
2025-11-09T02:11:22.6020877Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1648:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6021303Z  1648 |         {"Load Database", [[01;31m[Kthis[m[K]() { on_load_frequency_file(); }},
2025-11-09T02:11:22.6021520Z       |                            [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6035919Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6037223Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1648:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_load_frequency_file[m[K' was not declared in this scope; did you mean '[01m[Kload_freqman_file[m[K'?
2025-11-09T02:11:22.6037492Z  1648 |         {"Load Database", [this]() { [01;31m[Kon_load_frequency_file[m[K(); }},
2025-11-09T02:11:22.6037655Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6037810Z       |                                      [32m[Kload_freqman_file[m[K
2025-11-09T02:11:22.6038353Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-09T02:11:22.6039016Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1649:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6039242Z  1649 |         {"Save Settings", [[01;31m[Kthis[m[K]() { on_save_settings(); }},
2025-11-09T02:11:22.6039372Z       |                            [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6049391Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6050318Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1649:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_save_settings[m[K' was not declared in this scope; did you mean '[01m[Ksave_settings[m[K'?
2025-11-09T02:11:22.6050551Z  1649 |         {"Save Settings", [this]() { [01;31m[Kon_save_settings[m[K(); }},
2025-11-09T02:11:22.6050700Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6050846Z       |                                      [32m[Ksave_settings[m[K
2025-11-09T02:11:22.6051368Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-09T02:11:22.6052012Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1650:29:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6052234Z  1650 |         {"Audio Settings", [[01;31m[Kthis[m[K]() { on_audio_settings(); }},
2025-11-09T02:11:22.6052362Z       |                             [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6069757Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6071174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1650:39:[m[K [01;31m[Kerror: [m[K'[01m[Kon_audio_settings[m[K' was not declared in this scope; did you mean '[01m[Kload_settings[m[K'?
2025-11-09T02:11:22.6071421Z  1650 |         {"Audio Settings", [this]() { [01;31m[Kon_audio_settings[m[K(); }},
2025-11-09T02:11:22.6071569Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6071713Z       |                                       [32m[Kload_settings[m[K
2025-11-09T02:11:22.6072223Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-09T02:11:22.6072846Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1651:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6073068Z  1651 |         {"Spectrum Mode", [[01;31m[Kthis[m[K]() { on_spectrum_mode(); }},
2025-11-09T02:11:22.6073195Z       |                            [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6090671Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6092318Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1651:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[KSpectrumMode[m[K'?
2025-11-09T02:11:22.6092713Z  1651 |         {"Spectrum Mode", [this]() { [01;31m[Kon_spectrum_mode[m[K(); }},
2025-11-09T02:11:22.6092970Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6093218Z       |                                      [32m[KSpectrumMode[m[K
2025-11-09T02:11:22.6094140Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-09T02:11:22.6095481Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1652:31:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6095911Z  1652 |         {"Hardware Control", [[01;31m[Kthis[m[K]() { on_hardware_control(); }},
2025-11-09T02:11:22.6096129Z       |                               [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6118171Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6119328Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1652:41:[m[K [01;31m[Kerror: [m[K'[01m[Kon_hardware_control[m[K' was not declared in this scope
2025-11-09T02:11:22.6119754Z  1652 |         {"Hardware Control", [this]() { [01;31m[Kon_hardware_control[m[K(); }},
2025-11-09T02:11:22.6120033Z       |                                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6122149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-09T02:11:22.6123306Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1653:24:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6123651Z  1653 |         {"View Logs", [[01;31m[Kthis[m[K]() { on_view_logs(); }},
2025-11-09T02:11:22.6123866Z       |                        [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6139015Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6140147Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1653:34:[m[K [01;31m[Kerror: [m[K'[01m[Kon_view_logs[m[K' was not declared in this scope
2025-11-09T02:11:22.6140715Z  1653 |         {"View Logs", [this]() { [01;31m[Kon_view_logs[m[K(); }},
2025-11-09T02:11:22.6140961Z       |                                  [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6141892Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-09T02:11:22.6143040Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1654:20:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6143338Z  1654 |         {"About", [[01;31m[Kthis[m[K]() { on_about(); }}
2025-11-09T02:11:22.6143537Z       |                    [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6153099Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6154219Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1654:30:[m[K [01;31m[Kerror: [m[K'[01m[Kon_about[m[K' was not declared in this scope
2025-11-09T02:11:22.6154536Z  1654 |         {"About", [this]() { [01;31m[Kon_about[m[K(); }}
2025-11-09T02:11:22.6154953Z       |                              [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.6156893Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6158049Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1658:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6158521Z  1658 | void [01;31m[KDroneUIController[m[K::on_load_frequency_file() {
2025-11-09T02:11:22.6158733Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6164998Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_load_frequency_file()[m[K':
2025-11-09T02:11:22.6166146Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1659:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.6166493Z  1659 |     if ([01;31m[Kscanner_[m[K.load_frequency_database()) {
2025-11-09T02:11:22.6166701Z       |         [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.6170544Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1661:9:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6171139Z  1661 |         [01;31m[Knav_[m[K.display_modal("Success", "Loaded " + std::to_string(count) + " frequencies");
2025-11-09T02:11:22.6171336Z       |         [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6177864Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1663:9:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6178302Z  1663 |         [01;31m[Knav_[m[K.display_modal("Error", "Failed to load database");
2025-11-09T02:11:22.6178485Z       |         [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6179232Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6180364Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1667:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6180792Z  1667 | void [01;31m[KDroneUIController[m[K::on_save_settings() {
2025-11-09T02:11:22.6180996Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6186964Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_save_settings()[m[K':
2025-11-09T02:11:22.6187731Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1668:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksettings_[m[K' was not declared in this scope; did you mean '[01m[Ksettings_dir[m[K'?
2025-11-09T02:11:22.6188171Z  1668 |     [01;31m[Ksettings_[m[K.save(config::SETTINGS_FILE_PATH);
2025-11-09T02:11:22.6188291Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6188404Z       |     [32m[Ksettings_dir[m[K
2025-11-09T02:11:22.6188994Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1668:20:[m[K [01;31m[Kerror: [m[K'[01m[Kconfig[m[K' has not been declared
2025-11-09T02:11:22.6189305Z  1668 |     settings_.save([01;31m[Kconfig[m[K::SETTINGS_FILE_PATH);
2025-11-09T02:11:22.6189539Z       |                    [01;31m[K^~~~~~[m[K
2025-11-09T02:11:22.6191711Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1669:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6192119Z  1669 |     [01;31m[Knav_[m[K.display_modal("Success", "Settings saved");
2025-11-09T02:11:22.6192298Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6192999Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6193794Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1672:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6194052Z  1672 | void [01;31m[KDroneUIController[m[K::on_audio_settings() {
2025-11-09T02:11:22.6194168Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6196901Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_audio_settings()[m[K':
2025-11-09T02:11:22.6197520Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1673:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6197846Z  1673 |     [01;31m[Knav_[m[K.push<CheckboxView>("Enable Audio Alerts", &settings_.enable_audio_alerts);
2025-11-09T02:11:22.6197956Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6209244Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1673:15:[m[K [01;31m[Kerror: [m[K'[01m[KCheckboxView[m[K' was not declared in this scope
2025-11-09T02:11:22.6209572Z  1673 |     nav_.push<[01;31m[KCheckboxView[m[K>("Enable Audio Alerts", &settings_.enable_audio_alerts);
2025-11-09T02:11:22.6209699Z       |               [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6217216Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1673:53:[m[K [01;31m[Kerror: [m[K'[01m[Ksettings_[m[K' was not declared in this scope; did you mean '[01m[Ksettings_dir[m[K'?
2025-11-09T02:11:22.6217845Z  1673 |     nav_.push<CheckboxView>("Enable Audio Alerts", &[01;31m[Ksettings_[m[K.enable_audio_alerts);
2025-11-09T02:11:22.6218127Z       |                                                     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6218409Z       |                                                     [32m[Ksettings_dir[m[K
2025-11-09T02:11:22.6219170Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6220318Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1676:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6220769Z  1676 | void [01;31m[KDroneUIController[m[K::on_spectrum_mode() {
2025-11-09T02:11:22.6221001Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6222965Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-09T02:11:22.6224288Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6224951Z  1677 |     [01;31m[Knav_[m[K.push<MenuView>(std::vector<MenuView::Item>{
2025-11-09T02:11:22.6225137Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6226295Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:23:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-09T02:11:22.6226731Z  1677 |     nav_.push<MenuView[01;31m[K>[m[K(std::vector<MenuView::Item>{
2025-11-09T02:11:22.6226926Z       |                       [01;31m[K^[m[K
2025-11-09T02:11:22.6228223Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:47:[m[K [01;31m[Kerror: [m[K'[01m[KItem[m[K' is not a member of '[01m[Kui::MenuView[m[K'
2025-11-09T02:11:22.6228620Z  1677 |     nav_.push<MenuView>(std::vector<MenuView::[01;31m[KItem[m[K>{
2025-11-09T02:11:22.6228865Z       |                                               [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6229998Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6230382Z  1677 |     nav_.push<MenuView>(std::vector<MenuView::Item[01;31m[K>[m[K{
2025-11-09T02:11:22.6230625Z       |                                                   [01;31m[K^[m[K
2025-11-09T02:11:22.6231596Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 2 is invalid
2025-11-09T02:11:22.6232735Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1678:34:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6233367Z  1678 |         {"Ultra Narrow (4MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::ULTRA_NARROW); }},
2025-11-09T02:11:22.6233599Z       |                                  [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6235561Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6236792Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1678:76:[m[K [01;31m[Kerror: [m[K'[01m[KULTRA_NARROW[m[K' is not a member of '[01m[KSpectrumMode[m[K'
2025-11-09T02:11:22.6237389Z  1678 |         {"Ultra Narrow (4MHz)", [this]() { set_spectrum_mode(SpectrumMode::[01;31m[KULTRA_NARROW[m[K); }},
2025-11-09T02:11:22.6237707Z       |                                                                            [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6242301Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1678:44:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-09T02:11:22.6242997Z  1678 |         {"Ultra Narrow (4MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::ULTRA_NARROW); }},
2025-11-09T02:11:22.6243849Z       |                                            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6244414Z       |                                            [32m[Kon_spectrum_mode[m[K
2025-11-09T02:11:22.6245198Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-09T02:11:22.6245842Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1679:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6246181Z  1679 |         {"Narrow (8MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::NARROW); }},
2025-11-09T02:11:22.6246506Z       |                            [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6255751Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6257704Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1679:38:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-09T02:11:22.6258442Z  1679 |         {"Narrow (8MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::NARROW); }},
2025-11-09T02:11:22.6259141Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6259414Z       |                                      [32m[Kon_spectrum_mode[m[K
2025-11-09T02:11:22.6260388Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-09T02:11:22.6261561Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1680:29:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6262327Z  1680 |         {"Medium (12MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::MEDIUM); }},
2025-11-09T02:11:22.6262563Z       |                             [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6273856Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6275513Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1680:39:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-09T02:11:22.6276113Z  1680 |         {"Medium (12MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::MEDIUM); }},
2025-11-09T02:11:22.6276381Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6276660Z       |                                       [32m[Kon_spectrum_mode[m[K
2025-11-09T02:11:22.6277650Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-09T02:11:22.6278789Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1681:27:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6279316Z  1681 |         {"Wide (20MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::WIDE); }},
2025-11-09T02:11:22.6279528Z       |                           [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6287880Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6289574Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1681:37:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-09T02:11:22.6290157Z  1681 |         {"Wide (20MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::WIDE); }},
2025-11-09T02:11:22.6290413Z       |                                     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6290682Z       |                                     [32m[Kon_spectrum_mode[m[K
2025-11-09T02:11:22.6291634Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-09T02:11:22.6292770Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1682:33:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6293600Z  1682 |         {"Ultra Wide (24MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::ULTRA_WIDE); }}
2025-11-09T02:11:22.6293828Z       |                                 [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6303443Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6305096Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1682:43:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-09T02:11:22.6305733Z  1682 |         {"Ultra Wide (24MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::ULTRA_WIDE); }}
2025-11-09T02:11:22.6306003Z       |                                           [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6306269Z       |                                           [32m[Kon_spectrum_mode[m[K
2025-11-09T02:11:22.6307017Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6308328Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1686:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6308866Z  1686 | void [01;31m[KDroneUIController[m[K::set_spectrum_mode(SpectrumMode mode) {
2025-11-09T02:11:22.6309075Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6316435Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_spectrum_mode(SpectrumMode)[m[K':
2025-11-09T02:11:22.6317075Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1687:5:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.6317263Z  1687 |     [01;31m[Khardware_[m[K.set_spectrum_mode(mode);
2025-11-09T02:11:22.6317377Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6321465Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1688:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6321724Z  1688 |     [01;31m[Knav_[m[K.display_modal("Applied", "Spectrum mode updated");
2025-11-09T02:11:22.6321835Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6322254Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6322874Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1691:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6323136Z  1691 | void [01;31m[KDroneUIController[m[K::on_hardware_control() {
2025-11-09T02:11:22.6323259Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6327156Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hardware_control()[m[K':
2025-11-09T02:11:22.6327787Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6328042Z  1692 |     [01;31m[Knav_[m[K.push<MenuView>(std::vector<MenuView::Item>{
2025-11-09T02:11:22.6328146Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6328797Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:23:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-09T02:11:22.6329045Z  1692 |     nav_.push<MenuView[01;31m[K>[m[K(std::vector<MenuView::Item>{
2025-11-09T02:11:22.6329162Z       |                       [01;31m[K^[m[K
2025-11-09T02:11:22.6330065Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:47:[m[K [01;31m[Kerror: [m[K'[01m[KItem[m[K' is not a member of '[01m[Kui::MenuView[m[K'
2025-11-09T02:11:22.6330293Z  1692 |     nav_.push<MenuView>(std::vector<MenuView::[01;31m[KItem[m[K>{
2025-11-09T02:11:22.6330444Z       |                                               [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6331009Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6331225Z  1692 |     nav_.push<MenuView>(std::vector<MenuView::Item[01;31m[K>[m[K{
2025-11-09T02:11:22.6331363Z       |                                                   [01;31m[K^[m[K
2025-11-09T02:11:22.6331903Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 2 is invalid
2025-11-09T02:11:22.6332516Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1693:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6332842Z  1693 |         {"Set Bandwidth", [[01;31m[Kthis[m[K]() { on_set_bandwidth(); }},
2025-11-09T02:11:22.6332979Z       |                            [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6351526Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6352473Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1693:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_set_bandwidth[m[K' was not declared in this scope
2025-11-09T02:11:22.6352722Z  1693 |         {"Set Bandwidth", [this]() { [01;31m[Kon_set_bandwidth[m[K(); }},
2025-11-09T02:11:22.6352904Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6353593Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hardware_control()[m[K':
2025-11-09T02:11:22.6354377Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1694:30:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6354821Z  1694 |         {"Set Center Freq", [[01;31m[Kthis[m[K]() { on_set_center_freq(); }},
2025-11-09T02:11:22.6354988Z       |                              [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6375400Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6376575Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1694:40:[m[K [01;31m[Kerror: [m[K'[01m[Kon_set_center_freq[m[K' was not declared in this scope
2025-11-09T02:11:22.6376883Z  1694 |         {"Set Center Freq", [this]() { [01;31m[Kon_set_center_freq[m[K(); }},
2025-11-09T02:11:22.6377072Z       |                                        [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6377770Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hardware_control()[m[K':
2025-11-09T02:11:22.6378565Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1695:29:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6378850Z  1695 |         {"Current Status", [[01;31m[Kthis[m[K]() { show_hardware_status(); }}
2025-11-09T02:11:22.6379010Z       |                             [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6399789Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6401009Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1695:39:[m[K [01;31m[Kerror: [m[K'[01m[Kshow_hardware_status[m[K' was not declared in this scope
2025-11-09T02:11:22.6401661Z  1695 |         {"Current Status", [this]() { [01;31m[Kshow_hardware_status[m[K(); }}
2025-11-09T02:11:22.6401935Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6402696Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6403816Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1699:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6404242Z  1699 | void [01;31m[KDroneUIController[m[K::on_set_bandwidth() {
2025-11-09T02:11:22.6404448Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6412019Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_set_bandwidth()[m[K':
2025-11-09T02:11:22.6412681Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1700:27:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.6413076Z  1700 |     uint32_t current_bw = [01;31m[Khardware_[m[K.get_bandwidth();
2025-11-09T02:11:22.6413216Z       |                           [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6417227Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:18:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6417520Z  1701 |     auto& view = [01;31m[Knav_[m[K.push<NumberInputView>(current_bw, 1000000, 24000000);
2025-11-09T02:11:22.6417638Z       |                  [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6432646Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:28:[m[K [01;31m[Kerror: [m[K'[01m[KNumberInputView[m[K' was not declared in this scope
2025-11-09T02:11:22.6433566Z  1701 |     auto& view = nav_.push<[01;31m[KNumberInputView[m[K>(current_bw, 1000000, 24000000);
2025-11-09T02:11:22.6433907Z       |                            [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6435242Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:45:[m[K [01;35m[Kwarning: [m[Kleft operand of comma operator has no effect [[01;35m[K-Wunused-value[m[K]
2025-11-09T02:11:22.6435626Z  1701 |     auto& view = nav_.push<NumberInputView>([01;35m[Kcurrent_bw[m[K, 1000000, 24000000);
2025-11-09T02:11:22.6435811Z       |                                             [01;35m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.6436720Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:66:[m[K [01;35m[Kwarning: [m[Kright operand of comma operator has no effect [[01;35m[K-Wunused-value[m[K]
2025-11-09T02:11:22.6437090Z  1701 |     auto& view = nav_.push<NumberInputView>(current_bw, 1000000, [01;35m[K24000000[m[K);
2025-11-09T02:11:22.6437291Z       |                                                                  [01;35m[K^~~~~~~~[m[K
2025-11-09T02:11:22.6438078Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1702:24:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6438308Z  1702 |     view.on_changed = [[01;31m[Kthis[m[K](uint32_t bw) {
2025-11-09T02:11:22.6438462Z       |                        [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6438993Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6439701Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1703:9:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' is not captured
2025-11-09T02:11:22.6440168Z  1703 |         [01;31m[Khardware_[m[K.set_bandwidth(bw);
2025-11-09T02:11:22.6440313Z       |         [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6441019Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1702:28:[m[K [01;36m[Knote: [m[Kthe lambda has no capture-default
2025-11-09T02:11:22.6441247Z  1702 |     view.on_changed = [this[01;36m[K][m[K(uint32_t bw) {
2025-11-09T02:11:22.6441405Z       |                            [01;36m[K^[m[K
2025-11-09T02:11:22.6442160Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1700:27:[m[K [01;36m[Knote: [m[K'[01m[K<typeprefixerror>hardware_[m[K' declared here
2025-11-09T02:11:22.6442574Z  1700 |     uint32_t current_bw = [01;36m[Khardware_[m[K.get_bandwidth();
2025-11-09T02:11:22.6442856Z       |                           [01;36m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6443668Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6444308Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1707:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6444854Z  1707 | void [01;31m[KDroneUIController[m[K::on_set_center_freq() {
2025-11-09T02:11:22.6444991Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6445530Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_set_center_freq()[m[K':
2025-11-09T02:11:22.6446135Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1708:28:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.6446370Z  1708 |     Frequency current_cf = [01;31m[Khardware_[m[K.get_center_frequency();
2025-11-09T02:11:22.6446504Z       |                            [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6449381Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1709:18:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6449628Z  1709 |     auto& view = [01;31m[Knav_[m[K.push<FrequencyInputView>(current_cf);
2025-11-09T02:11:22.6449746Z       |                  [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6466581Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1709:28:[m[K [01;31m[Kerror: [m[K'[01m[KFrequencyInputView[m[K' was not declared in this scope
2025-11-09T02:11:22.6467035Z  1709 |     auto& view = nav_.push<[01;31m[KFrequencyInputView[m[K>(current_cf);
2025-11-09T02:11:22.6467284Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6468480Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1711:24:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.6468879Z  1711 |     view.on_changed = [[01;31m[Kthis[m[K](Frequency freq) {
2025-11-09T02:11:22.6469096Z       |                        [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6469818Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-09T02:11:22.6470413Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1712:9:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' is not captured
2025-11-09T02:11:22.6470594Z  1712 |         [01;31m[Khardware_[m[K.set_center_frequency(freq);
2025-11-09T02:11:22.6470710Z       |         [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6471261Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1711:28:[m[K [01;36m[Knote: [m[Kthe lambda has no capture-default
2025-11-09T02:11:22.6471621Z  1711 |     view.on_changed = [this[01;36m[K][m[K(Frequency freq) {
2025-11-09T02:11:22.6471748Z       |                            [01;36m[K^[m[K
2025-11-09T02:11:22.6472359Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1708:28:[m[K [01;36m[Knote: [m[K'[01m[K<typeprefixerror>hardware_[m[K' declared here
2025-11-09T02:11:22.6472594Z  1708 |     Frequency current_cf = [01;36m[Khardware_[m[K.get_center_frequency();
2025-11-09T02:11:22.6472728Z       |                            [01;36m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6473148Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6473745Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1716:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6474000Z  1716 | void [01;31m[KDroneUIController[m[K::show_hardware_status() {
2025-11-09T02:11:22.6474121Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6478342Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_hardware_status()[m[K':
2025-11-09T02:11:22.6479150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1720:13:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.6479339Z  1720 |             [01;31m[Khardware_[m[K.get_bandwidth() / 1000000,
2025-11-09T02:11:22.6479454Z       |             [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6483497Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1722:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6483735Z  1722 |     [01;31m[Knav_[m[K.display_modal("Hardware Status", buffer);
2025-11-09T02:11:22.6483851Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6484278Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6485053Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1725:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6485292Z  1725 | void [01;31m[KDroneUIController[m[K::on_view_logs() {
2025-11-09T02:11:22.6485412Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6488704Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_view_logs()[m[K':
2025-11-09T02:11:22.6489330Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1726:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6489548Z  1726 |     [01;31m[Knav_[m[K.push<FileBrowserView>("/LOGS/EDA", ".CSV");
2025-11-09T02:11:22.6489650Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6503179Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1726:15:[m[K [01;31m[Kerror: [m[K'[01m[KFileBrowserView[m[K' was not declared in this scope
2025-11-09T02:11:22.6503429Z  1726 |     nav_.push<[01;31m[KFileBrowserView[m[K>("/LOGS/EDA", ".CSV");
2025-11-09T02:11:22.6503556Z       |               [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6504272Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1726:32:[m[K [01;35m[Kwarning: [m[Kleft operand of comma operator has no effect [[01;35m[K-Wunused-value[m[K]
2025-11-09T02:11:22.6504500Z  1726 |     nav_.push<FileBrowserView>([01;35m[K"/LOGS/EDA"[m[K, ".CSV");
2025-11-09T02:11:22.6504786Z       |                                [01;35m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6505401Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6506326Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1729:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-09T02:11:22.6506755Z  1729 | void [01;31m[KDroneUIController[m[K::on_about() {
2025-11-09T02:11:22.6506988Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6508066Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_about()[m[K':
2025-11-09T02:11:22.6508801Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1730:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6509246Z  1730 |     [01;31m[Knav_[m[K.display_modal("EDA v1.0", "Enhanced Drone Analyzer\nMayhem Firmware Integration\nBased on Recon & Looking Glass");
2025-11-09T02:11:22.6509363Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6520219Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6521236Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1733:1:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' does not name a type
2025-11-09T02:11:22.6521693Z  1733 | [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
2025-11-09T02:11:22.6521828Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6522452Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1742:68:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.6522767Z  1742 |       button_start_({screen_width - 80, screen_height - 48, 72, 24}[01;31m[K,[m[K "START/STOP"),
2025-11-09T02:11:22.6522939Z       |                                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.6523542Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1742:70:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.6523843Z  1742 |       button_start_({screen_width - 80, screen_height - 48, 72, 24}, [01;31m[K"START/STOP"[m[K),
2025-11-09T02:11:22.6524012Z       |                                                                      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6524792Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1743:67:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.6525091Z  1743 |       button_menu_({screen_width - 80, screen_height - 24, 72, 24}[01;31m[K,[m[K "MENU"),
2025-11-09T02:11:22.6525251Z       |                                                                   [01;31m[K^[m[K
2025-11-09T02:11:22.6525858Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1743:69:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.6526135Z  1743 |       button_menu_({screen_width - 80, screen_height - 24, 72, 24}, [01;31m[K"MENU"[m[K),
2025-11-09T02:11:22.6526314Z       |                                                                     [01;31m[K^~~~~~[m[K
2025-11-09T02:11:22.6526929Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1744:74:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.6527221Z  1744 |       field_scanning_mode_({0, screen_height - 72, screen_width - 80, 24}[01;31m[K,[m[K 3),
2025-11-09T02:11:22.6527380Z       |                                                                          [01;31m[K^[m[K
2025-11-09T02:11:22.6528122Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1744:76:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before numeric constant
2025-11-09T02:11:22.6528411Z  1744 |       field_scanning_mode_({0, screen_height - 72, screen_width - 80, 24}, [01;31m[K3[m[K),
2025-11-09T02:11:22.6528572Z       |                                                                            [01;31m[K^[m[K
2025-11-09T02:11:22.6529258Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1783:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6529516Z  1783 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::focus() {
2025-11-09T02:11:22.6529657Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6534078Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid focus()[m[K':
2025-11-09T02:11:22.6535402Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1784:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-09T02:11:22.6535758Z  1784 |     [01;31m[Kbutton_start_[m[K.focus();
2025-11-09T02:11:22.6535884Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6536310Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6537002Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1787:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6537348Z  1787 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.6537487Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6538198Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1787:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-09T02:11:22.6538889Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-09T02:11:22.6539130Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.6539251Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6539829Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-09T02:11:22.6540659Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1788:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-09T02:11:22.6540806Z  1788 |     View::paint(painter[01;31m[K)[m[K;
2025-11-09T02:11:22.6540927Z       |                        [01;31m[K^[m[K
2025-11-09T02:11:22.6541339Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6542008Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1791:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6542340Z  1791 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_key(const KeyEvent key) {
2025-11-09T02:11:22.6542475Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6551996Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool on_key(ui::KeyEvent)[m[K':
2025-11-09T02:11:22.6552862Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1794:13:[m[K [01;31m[Kerror: [m[K'[01m[Kstop_scanning_thread[m[K' was not declared in this scope
2025-11-09T02:11:22.6553022Z  1794 |             [01;31m[Kstop_scanning_thread[m[K();
2025-11-09T02:11:22.6553156Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6557823Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1795:13:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.6558076Z  1795 |             [01;31m[Knav_[m[K.pop();
2025-11-09T02:11:22.6558275Z       |             [01;31m[K^~~~[m[K
2025-11-09T02:11:22.6559534Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1800:28:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual bool ui::Widget::on_key(ui::KeyEvent)[m[K' without object
2025-11-09T02:11:22.6559702Z  1800 |     return View::on_key(key[01;31m[K)[m[K;
2025-11-09T02:11:22.6559826Z       |                            [01;31m[K^[m[K
2025-11-09T02:11:22.6560253Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6561121Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1803:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6561492Z  1803 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_touch(const TouchEvent event) {
2025-11-09T02:11:22.6561640Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6562260Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool on_touch(ui::TouchEvent)[m[K':
2025-11-09T02:11:22.6563109Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1804:32:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual bool ui::Widget::on_touch(ui::TouchEvent)[m[K' without object
2025-11-09T02:11:22.6563286Z  1804 |     return View::on_touch(event[01;31m[K)[m[K;
2025-11-09T02:11:22.6563415Z       |                                [01;31m[K^[m[K
2025-11-09T02:11:22.6563829Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6564508Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1807:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6564932Z  1807 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_show() {
2025-11-09T02:11:22.6565074Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6565577Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_show()[m[K':
2025-11-09T02:11:22.6566378Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1808:19:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::Widget::on_show()[m[K' without object
2025-11-09T02:11:22.6566588Z  1808 |     View::on_show([01;31m[K)[m[K;
2025-11-09T02:11:22.6574383Z       |                   [01;31m[K^[m[K
2025-11-09T02:11:22.6575871Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1810:5:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.6576052Z  1810 |     [01;31m[Khardware_[m[K->on_hardware_show();
2025-11-09T02:11:22.6576163Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6576584Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6577460Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1813:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6577747Z  1813 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_hide() {
2025-11-09T02:11:22.6577897Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6585726Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hide()[m[K':
2025-11-09T02:11:22.6586420Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1814:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstop_scanning_thread[m[K' was not declared in this scope
2025-11-09T02:11:22.6586572Z  1814 |     [01;31m[Kstop_scanning_thread[m[K();
2025-11-09T02:11:22.6586690Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6595449Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1815:5:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.6595652Z  1815 |     [01;31m[Khardware_[m[K->on_hardware_hide();
2025-11-09T02:11:22.6595760Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.6596788Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1816:19:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::Widget::on_hide()[m[K' without object
2025-11-09T02:11:22.6596930Z  1816 |     View::on_hide([01;31m[K)[m[K;
2025-11-09T02:11:22.6597041Z       |                   [01;31m[K^[m[K
2025-11-09T02:11:22.6597472Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6598143Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1819:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6598466Z  1819 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::start_scanning_thread() {
2025-11-09T02:11:22.6598613Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6611922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid start_scanning_thread()[m[K':
2025-11-09T02:11:22.6612635Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1820:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-09T02:11:22.6612892Z  1820 |     if ([01;31m[Kscanning_coordinator_[m[K->is_scanning_active()) return;
2025-11-09T02:11:22.6613017Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6628521Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1821:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-09T02:11:22.6628775Z  1821 |     [01;31m[Kscanning_coordinator_[m[K->start_coordinated_scanning();
2025-11-09T02:11:22.6628905Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6629335Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6630032Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1824:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6630367Z  1824 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::stop_scanning_thread() {
2025-11-09T02:11:22.6630509Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6645077Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid stop_scanning_thread()[m[K':
2025-11-09T02:11:22.6645957Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1825:10:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-09T02:11:22.6646202Z  1825 |     if (![01;31m[Kscanning_coordinator_[m[K->is_scanning_active()) return;
2025-11-09T02:11:22.6646333Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6661217Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-09T02:11:22.6661476Z  1826 |     [01;31m[Kscanning_coordinator_[m[K->stop_coordinated_scanning();
2025-11-09T02:11:22.6661596Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6662014Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6662702Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1829:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6663217Z  1829 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_start_stop_button() {
2025-11-09T02:11:22.6663368Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6677700Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool handle_start_stop_button()[m[K':
2025-11-09T02:11:22.6678370Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1830:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-09T02:11:22.6678588Z  1830 |     if ([01;31m[Kscanning_coordinator_[m[K->is_scanning_active()) {
2025-11-09T02:11:22.6678727Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6691658Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1831:9:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.6691857Z  1831 |         [01;31m[Kui_controller_[m[K->on_stop_scan();
2025-11-09T02:11:22.6691981Z       |         [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6705378Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1832:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-09T02:11:22.6705576Z  1832 |         [01;31m[Kbutton_start_[m[K.set_text("START/STOP");
2025-11-09T02:11:22.6705699Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6719694Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1834:9:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.6720015Z  1834 |         [01;31m[Kui_controller_[m[K->on_start_scan();
2025-11-09T02:11:22.6720221Z       |         [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6733955Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1835:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-09T02:11:22.6734258Z  1835 |         [01;31m[Kbutton_start_[m[K.set_text("STOP");
2025-11-09T02:11:22.6734459Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6735407Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6736649Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1840:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6737444Z  1840 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_menu_button() {
2025-11-09T02:11:22.6737688Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6751183Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool handle_menu_button()[m[K':
2025-11-09T02:11:22.6752347Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1841:5:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.6752617Z  1841 |     [01;31m[Kui_controller_[m[K->show_menu();
2025-11-09T02:11:22.6752809Z       |     [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6753558Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6754973Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1845:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6755598Z  1845 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::initialize_modern_layout() {
2025-11-09T02:11:22.6755843Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6765993Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid initialize_modern_layout()[m[K':
2025-11-09T02:11:22.6767207Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksmart_header_[m[K' was not declared in this scope
2025-11-09T02:11:22.6767778Z  1846 |     [01;31m[Ksmart_header_[m[K = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
2025-11-09T02:11:22.6767953Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6769811Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:38:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::SmartThreatHeader[m[K'?
2025-11-09T02:11:22.6770397Z  1846 |     smart_header_ = std::make_unique<[01;31m[KSmartThreatHeader[m[K>(Rect{0, 0, screen_width, 48});
2025-11-09T02:11:22.6770650Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6771169Z       |                                      [32m[Kui::external_app::enhanced_drone_analyzer::SmartThreatHeader[m[K
2025-11-09T02:11:22.6771932Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6773330Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:462:7:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::SmartThreatHeader[m[K' declared here
2025-11-09T02:11:22.6773660Z   462 | class [01;36m[KSmartThreatHeader[m[K : public View {
2025-11-09T02:11:22.6773860Z       |       [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6775560Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmake_unique<<expression error> >(ui::Rect)[m[K'
2025-11-09T02:11:22.6776183Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-09T02:11:22.6776497Z       |                                                                                     [01;31m[K^[m[K
2025-11-09T02:11:22.6777034Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6777389Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6778054Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6778930Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6779625Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6781156Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)[m[K'
2025-11-09T02:11:22.6781423Z   848 |     [01;36m[Kmake_unique[m[K(_Args&&... __args)
2025-11-09T02:11:22.6781614Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6782512Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6783537Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6784144Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-09T02:11:22.6784624Z       |                                                                                     [01;31m[K^[m[K
2025-11-09T02:11:22.6785273Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6785601Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6786258Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6787009Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6787765Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6789090Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)[m[K'
2025-11-09T02:11:22.6789364Z   854 |     [01;36m[Kmake_unique[m[K(size_t __num)
2025-11-09T02:11:22.6789547Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6790454Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6791447Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6792046Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-09T02:11:22.6792353Z       |                                                                                     [01;31m[K^[m[K
2025-11-09T02:11:22.6792896Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6793245Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6793902Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6794589Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6795475Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6797051Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)[m[K' <deleted>
2025-11-09T02:11:22.6797541Z   860 |     [01;36m[Kmake_unique[m[K(_Args&&...) = delete;
2025-11-09T02:11:22.6797724Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6798627Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6799632Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6800235Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-09T02:11:22.6800538Z       |                                                                                     [01;31m[K^[m[K
2025-11-09T02:11:22.6801672Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstatus_bar_[m[K' was not declared in this scope
2025-11-09T02:11:22.6802384Z  1847 |     [01;31m[Kstatus_bar_[m[K = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
2025-11-09T02:11:22.6802716Z       |     [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6804558Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:36:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::ConsoleStatusBar[m[K'?
2025-11-09T02:11:22.6805400Z  1847 |     status_bar_ = std::make_unique<[01;31m[KConsoleStatusBar[m[K>(0, Rect{0, screen_height - 32, screen_width, 16});
2025-11-09T02:11:22.6805658Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6806165Z       |                                    [32m[Kui::external_app::enhanced_drone_analyzer::ConsoleStatusBar[m[K
2025-11-09T02:11:22.6806941Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6808362Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:529:7:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::ConsoleStatusBar[m[K' declared here
2025-11-09T02:11:22.6808667Z   529 | class [01;36m[KConsoleStatusBar[m[K : public View {
2025-11-09T02:11:22.6808844Z       |       [01;36m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6810334Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmake_unique<<expression error> >(int, ui::Rect)[m[K'
2025-11-09T02:11:22.6811105Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-09T02:11:22.6811471Z       |                                                                                                      [01;31m[K^[m[K
2025-11-09T02:11:22.6811962Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6812290Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6812963Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6813659Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6814348Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6816087Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)[m[K'
2025-11-09T02:11:22.6816555Z   848 |     [01;36m[Kmake_unique[m[K(_Args&&... __args)
2025-11-09T02:11:22.6816783Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6817678Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6818679Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6819364Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-09T02:11:22.6819690Z       |                                                                                                      [01;31m[K^[m[K
2025-11-09T02:11:22.6820231Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6820572Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6821378Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6822073Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6822745Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6824152Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)[m[K'
2025-11-09T02:11:22.6824410Z   854 |     [01;36m[Kmake_unique[m[K(size_t __num)
2025-11-09T02:11:22.6824597Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6825633Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6826643Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6827333Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-09T02:11:22.6827660Z       |                                                                                                      [01;31m[K^[m[K
2025-11-09T02:11:22.6828190Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6828534Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6829206Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6829892Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6830572Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6832133Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)[m[K' <deleted>
2025-11-09T02:11:22.6832417Z   860 |     [01;36m[Kmake_unique[m[K(_Args&&...) = delete;
2025-11-09T02:11:22.6832602Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6833490Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6834791Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6835494Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-09T02:11:22.6835827Z       |                                                                                                      [01;31m[K^[m[K
2025-11-09T02:11:22.6836982Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1850:28:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_cards_[m[K' was not declared in this scope
2025-11-09T02:11:22.6837329Z  1850 |     for (size_t i = 0; i < [01;31m[Kthreat_cards_[m[K.size(); ++i) {
2025-11-09T02:11:22.6837560Z       |                            [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6839240Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:45:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::ThreatCard[m[K'?
2025-11-09T02:11:22.6840047Z  1851 |         threat_cards_[i] = std::make_unique<[01;31m[KThreatCard[m[K>(i, Rect{0, card_y_pos, screen_width, 24});
2025-11-09T02:11:22.6840316Z       |                                             [01;31m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.6840803Z       |                                             [32m[Kui::external_app::enhanced_drone_analyzer::ThreatCard[m[K
2025-11-09T02:11:22.6841561Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6842910Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:497:7:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::ThreatCard[m[K' declared here
2025-11-09T02:11:22.6843185Z   497 | class [01;36m[KThreatCard[m[K : public View {
2025-11-09T02:11:22.6843367Z       |       [01;36m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.6845200Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:68:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[Kcard_y_pos[m[K' from '[01m[Ksize_t[m[K' {aka '[01m[Kunsigned int[m[K'} to '[01m[Kint[m[K' [[01;35m[K-Wnarrowing[m[K]
2025-11-09T02:11:22.6845807Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, [01;35m[Kcard_y_pos[m[K, screen_width, 24});
2025-11-09T02:11:22.6846095Z       |                                                                    [01;35m[K^~~~~~~~~~[m[K
2025-11-09T02:11:22.6847609Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmake_unique<<expression error> >(size_t&, ui::Rect)[m[K'
2025-11-09T02:11:22.6848212Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-09T02:11:22.6848533Z       |                                                                                                 [01;31m[K^[m[K
2025-11-09T02:11:22.6849066Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6849406Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6850059Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6850772Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6851471Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6853150Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)[m[K'
2025-11-09T02:11:22.6853425Z   848 |     [01;36m[Kmake_unique[m[K(_Args&&... __args)
2025-11-09T02:11:22.6853608Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6854494Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6855659Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6856283Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-09T02:11:22.6856608Z       |                                                                                                 [01;31m[K^[m[K
2025-11-09T02:11:22.6857135Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6857625Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6858286Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6858974Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6859647Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6861022Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)[m[K'
2025-11-09T02:11:22.6861279Z   854 |     [01;36m[Kmake_unique[m[K(size_t __num)
2025-11-09T02:11:22.6861459Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6862346Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6863338Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6863951Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-09T02:11:22.6864269Z       |                                                                                                 [01;31m[K^[m[K
2025-11-09T02:11:22.6864959Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-09T02:11:22.6865358Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-09T02:11:22.6865946Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.6866650Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6867341Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6868952Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)[m[K' <deleted>
2025-11-09T02:11:22.6869233Z   860 |     [01;36m[Kmake_unique[m[K(_Args&&...) = delete;
2025-11-09T02:11:22.6869415Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6870582Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6871614Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-09T02:11:22.6872257Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-09T02:11:22.6872588Z       |                                                                                                 [01;31m[K^[m[K
2025-11-09T02:11:22.6873799Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1855:5:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_scanner_update[m[K' was not declared in this scope
2025-11-09T02:11:22.6874059Z  1855 |     [01;31m[Khandle_scanner_update[m[K();
2025-11-09T02:11:22.6874267Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6875211Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6876659Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1858:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6877183Z  1858 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::update_modern_layout() {
2025-11-09T02:11:22.6877421Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6878401Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_modern_layout()[m[K':
2025-11-09T02:11:22.6879581Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1859:5:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_scanner_update[m[K' was not declared in this scope
2025-11-09T02:11:22.6879839Z  1859 |     [01;31m[Khandle_scanner_update[m[K();
2025-11-09T02:11:22.6880036Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6880776Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.6882008Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1862:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-09T02:11:22.6882569Z  1862 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_scanner_update() {
2025-11-09T02:11:22.6882804Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6883793Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid handle_scanner_update()[m[K':
2025-11-09T02:11:22.6885040Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1863:10:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.6885421Z  1863 |     if (![01;31m[Kscanner_[m[K || !smart_header_ || !status_bar_) return;
2025-11-09T02:11:22.6885622Z       |          [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.6894048Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1863:23:[m[K [01;31m[Kerror: [m[K'[01m[Ksmart_header_[m[K' was not declared in this scope
2025-11-09T02:11:22.6894441Z  1863 |     if (!scanner_ || ![01;31m[Ksmart_header_[m[K || !status_bar_) return;
2025-11-09T02:11:22.6894850Z       |                       [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6906254Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1863:41:[m[K [01;31m[Kerror: [m[K'[01m[Kstatus_bar_[m[K' was not declared in this scope
2025-11-09T02:11:22.6906870Z  1863 |     if (!scanner_ || !smart_header_ || ![01;31m[Kstatus_bar_[m[K) return;
2025-11-09T02:11:22.6907122Z       |                                         [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6915325Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1865:30:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.6915810Z  1865 |     ThreatLevel max_threat = [01;31m[Kscanner_[m[K->get_max_detected_threat();
2025-11-09T02:11:22.6916037Z       |                              [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.6927772Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1873:9:[m[K [01;31m[Kerror: [m[K'[01m[Ksmart_header_[m[K' was not declared in this scope
2025-11-09T02:11:22.6927939Z  1873 |     if ([01;31m[Ksmart_header_[m[K) {
2025-11-09T02:11:22.6928059Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6939585Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1878:9:[m[K [01;31m[Kerror: [m[K'[01m[Kstatus_bar_[m[K' was not declared in this scope
2025-11-09T02:11:22.6939877Z  1878 |     if ([01;31m[Kstatus_bar_[m[K) {
2025-11-09T02:11:22.6940321Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-09T02:11:22.6941702Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmin(long unsigned int, unsigned int)[m[K'
2025-11-09T02:11:22.6942168Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;31m[K)[m[K;
2025-11-09T02:11:22.6942504Z       |                                                          [01;31m[K^[m[K
2025-11-09T02:11:22.6943186Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39[m[K,
2025-11-09T02:11:22.6943707Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40[m[K,
2025-11-09T02:11:22.6944461Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.6945353Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6946450Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)[m[K'
2025-11-09T02:11:22.6946635Z   198 |     [01;36m[Kmin[m[K(const _Tp& __a, const _Tp& __b)
2025-11-09T02:11:22.6946738Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.6947270Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6948152Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  deduced conflicting types for parameter '[01m[Kconst _Tp[m[K' ('[01m[Klong unsigned int[m[K' and '[01m[Kunsigned int[m[K')
2025-11-09T02:11:22.6948396Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-09T02:11:22.6948552Z       |                                                          [01;36m[K^[m[K
2025-11-09T02:11:22.6948903Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39[m[K,
2025-11-09T02:11:22.6949158Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40[m[K,
2025-11-09T02:11:22.6949557Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.6949933Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6950937Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)[m[K'
2025-11-09T02:11:22.6951150Z   246 |     [01;36m[Kmin[m[K(const _Tp& __a, const _Tp& __b, _Compare __comp)
2025-11-09T02:11:22.6951253Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.6951755Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6952606Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  deduced conflicting types for parameter '[01m[Kconst _Tp[m[K' ('[01m[Klong unsigned int[m[K' and '[01m[Kunsigned int[m[K')
2025-11-09T02:11:22.6952831Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-09T02:11:22.6952984Z       |                                                          [01;36m[K^[m[K
2025-11-09T02:11:22.6953325Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62[m[K,
2025-11-09T02:11:22.6953807Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6[m[K,
2025-11-09T02:11:22.6954188Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6954565Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6955409Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)[m[K'
2025-11-09T02:11:22.6955571Z  3444 |     [01;36m[Kmin[m[K(initializer_list<_Tp> __l)
2025-11-09T02:11:22.6955677Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.6956163Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6956960Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Klong unsigned int[m[K'
2025-11-09T02:11:22.6957187Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-09T02:11:22.6957334Z       |                                                          [01;36m[K^[m[K
2025-11-09T02:11:22.6957641Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62[m[K,
2025-11-09T02:11:22.6958010Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6[m[K,
2025-11-09T02:11:22.6958387Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.6958756Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.6959567Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)[m[K'
2025-11-09T02:11:22.6959763Z  3450 |     [01;36m[Kmin[m[K(initializer_list<_Tp> __l, _Compare __comp)
2025-11-09T02:11:22.6959862Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.6960357Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.6961150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Klong unsigned int[m[K'
2025-11-09T02:11:22.6961491Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-09T02:11:22.6961646Z       |                                                          [01;36m[K^[m[K
2025-11-09T02:11:22.6965377Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1889:41:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.6965971Z  1889 |             const char* primary_msg = (![01;31m[Kdisplay_controller_[m[K || display_controller_->big_display().text().empty()) ?
2025-11-09T02:11:22.6966132Z       |                                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6967162Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1893:90:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-09T02:11:22.6967830Z  1893 |                 snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: [01;35m[K%u[m[K", [32m[Ktotal_detections[m[K);
2025-11-09T02:11:22.6968051Z       |                                                                                         [01;35m[K~^[m[K   [32m[K~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.6968244Z       |                                                                                          [01;35m[K|[m[K   [32m[K|[m[K
2025-11-09T02:11:22.6968498Z       |                                                                                          [01;35m[K|[m[K   [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-09T02:11:22.6968691Z       |                                                                                          [01;35m[Kunsigned int[m[K
2025-11-09T02:11:22.6968866Z       |                                                                                         [32m[K%lu[m[K
2025-11-09T02:11:22.6980278Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1901:48:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_cards_[m[K' was not declared in this scope
2025-11-09T02:11:22.6980560Z  1901 |     for (size_t i = 0; i < std::min(size_t(3), [01;31m[Kthreat_cards_[m[K.size()); ++i) {
2025-11-09T02:11:22.6980715Z       |                                                [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7003065Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1922:41:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_cards_[m[K' was not declared in this scope
2025-11-09T02:11:22.7003394Z  1922 |     for (size_t i = std::min(size_t(3), [01;31m[Kthreat_cards_[m[K.size()); i < threat_cards_.size(); ++i) {
2025-11-09T02:11:22.7003551Z       |                                         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7020694Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1928:9:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.7020858Z  1928 |     if ([01;31m[Kdisplay_controller_[m[K) {
2025-11-09T02:11:22.7020985Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7037497Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7038128Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1933:1:[m[K [01;31m[Kerror: [m[K'[01m[KLoadingScreenView[m[K' does not name a type
2025-11-09T02:11:22.7038426Z  1933 | [01;31m[KLoadingScreenView[m[K::LoadingScreenView(NavigationView& nav)
2025-11-09T02:11:22.7038712Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7039349Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1935:39:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-09T02:11:22.7039545Z  1935 |       text_eda_(Rect{108, 213, 24, 16}[01;31m[K,[m[K "EDA"),
2025-11-09T02:11:22.7039679Z       |                                       [01;31m[K^[m[K
2025-11-09T02:11:22.7040277Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1935:41:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-09T02:11:22.7040461Z  1935 |       text_eda_(Rect{108, 213, 24, 16}, [01;31m[K"EDA"[m[K),
2025-11-09T02:11:22.7040596Z       |                                         [01;31m[K^~~~~[m[K
2025-11-09T02:11:22.7054463Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1943:1:[m[K [01;31m[Kerror: [m[K'[01m[KLoadingScreenView[m[K' does not name a type
2025-11-09T02:11:22.7055100Z  1943 | [01;31m[KLoadingScreenView[m[K::~LoadingScreenView() {
2025-11-09T02:11:22.7055301Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7056451Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1946:6:[m[K [01;31m[Kerror: [m[K'[01m[KLoadingScreenView[m[K' has not been declared
2025-11-09T02:11:22.7056821Z  1946 | void [01;31m[KLoadingScreenView[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.7057019Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7057748Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1946:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-09T02:11:22.7058448Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-09T02:11:22.7058693Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-09T02:11:22.7058807Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7059408Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-09T02:11:22.7060233Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-09T02:11:22.7060372Z  1951 |     View::paint(painter[01;31m[K)[m[K;
2025-11-09T02:11:22.7060493Z       |                        [01;31m[K^[m[K
2025-11-09T02:11:22.7072653Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7073818Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1954:1:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' does not name a type
2025-11-09T02:11:22.7074372Z  1954 | [01;31m[KScanningCoordinator[m[K::ScanningCoordinator(NavigationView& nav,
2025-11-09T02:11:22.7074566Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7091133Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1970:1:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' does not name a type
2025-11-09T02:11:22.7091588Z  1970 | [01;31m[KScanningCoordinator[m[K::~ScanningCoordinator() {
2025-11-09T02:11:22.7091779Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7092911Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1974:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-09T02:11:22.7093618Z  1974 | void [01;31m[KScanningCoordinator[m[K::start_coordinated_scanning() {
2025-11-09T02:11:22.7093818Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7109254Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid start_coordinated_scanning()[m[K':
2025-11-09T02:11:22.7109934Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1975:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.7110177Z  1975 |     if ([01;31m[Kscanning_active_[m[K || scanning_thread_ != nullptr) return;
2025-11-09T02:11:22.7110300Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7123109Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1975:29:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-09T02:11:22.7123385Z  1975 |     if (scanning_active_ || [01;31m[Kscanning_thread_[m[K != nullptr) return;
2025-11-09T02:11:22.7123530Z       |                             [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7123837Z       |                             [32m[Kstop_scanning_thread[m[K
2025-11-09T02:11:22.7139241Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1977:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.7139512Z  1977 |     [01;31m[Kscanning_active_[m[K = true;
2025-11-09T02:11:22.7139707Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7154038Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1979:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-09T02:11:22.7154445Z  1979 |     [01;31m[Kscanning_thread_[m[K = chThdCreateFromHeap(nullptr, SCANNING_THREAD_STACK_SIZE,
2025-11-09T02:11:22.7154592Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7154913Z       |     [32m[Kstop_scanning_thread[m[K
2025-11-09T02:11:22.7170771Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:43:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_function[m[K' was not declared in this scope
2025-11-09T02:11:22.7171150Z  1981 |                                           [01;31m[Kscanning_thread_function[m[K, this);
2025-11-09T02:11:22.7171426Z       |                                           [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7172583Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:69:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-09T02:11:22.7172948Z  1981 |                                           scanning_thread_function, [01;31m[Kthis[m[K);
2025-11-09T02:11:22.7173228Z       |                                                                     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.7173977Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7175259Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1987:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-09T02:11:22.7175750Z  1987 | void [01;31m[KScanningCoordinator[m[K::stop_coordinated_scanning() {
2025-11-09T02:11:22.7175954Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7190083Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid stop_coordinated_scanning()[m[K':
2025-11-09T02:11:22.7192220Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1988:10:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.7192508Z  1988 |     if (![01;31m[Kscanning_active_[m[K) return;
2025-11-09T02:11:22.7192716Z       |          [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7208231Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1990:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.7208401Z  1990 |     [01;31m[Kscanning_active_[m[K = false;
2025-11-09T02:11:22.7208514Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7222497Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1991:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-09T02:11:22.7222662Z  1991 |     if ([01;31m[Kscanning_thread_[m[K) {
2025-11-09T02:11:22.7222781Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7222910Z       |         [32m[Kstop_scanning_thread[m[K
2025-11-09T02:11:22.7223509Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7224159Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1997:7:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-09T02:11:22.7224473Z  1997 | msg_t [01;31m[KScanningCoordinator[m[K::scanning_thread_function(void* arg) {
2025-11-09T02:11:22.7224597Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7239597Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kmsg_t scanning_thread_function(void*)[m[K':
2025-11-09T02:11:22.7240289Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:30:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' does not name a type
2025-11-09T02:11:22.7240522Z  1998 |     auto* self = static_cast<[01;31m[KScanningCoordinator[m[K*>(arg);
2025-11-09T02:11:22.7240670Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7241303Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:49:[m[K [01;31m[Kerror: [m[Kexpected '[01m[K>[m[K' before '[01m[K*[m[K' token
2025-11-09T02:11:22.7241531Z  1998 |     auto* self = static_cast<ScanningCoordinator[01;31m[K*[m[K>(arg);
2025-11-09T02:11:22.7241672Z       |                                                 [01;31m[K^[m[K
2025-11-09T02:11:22.7242282Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:49:[m[K [01;31m[Kerror: [m[Kexpected '[01m[K([m[K' before '[01m[K*[m[K' token
2025-11-09T02:11:22.7242505Z  1998 |     auto* self = static_cast<ScanningCoordinator[01;31m[K*[m[K>(arg);
2025-11-09T02:11:22.7242638Z       |                                                 [01;31m[K^[m[K
2025-11-09T02:11:22.7242785Z       |                                                 [32m[K([m[K
2025-11-09T02:11:22.7243411Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:50:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-09T02:11:22.7243625Z  1998 |     auto* self = static_cast<ScanningCoordinator*[01;31m[K>[m[K(arg);
2025-11-09T02:11:22.7243768Z       |                                                  [01;31m[K^[m[K
2025-11-09T02:11:22.7244367Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:56:[m[K [01;31m[Kerror: [m[Kexpected '[01m[K)[m[K' before '[01m[K;[m[K' token
2025-11-09T02:11:22.7244918Z  1998 |     auto* self = static_cast<ScanningCoordinator*>(arg)[01;31m[K;[m[K
2025-11-09T02:11:22.7245066Z       |                                                        [01;31m[K^[m[K
2025-11-09T02:11:22.7245204Z       |                                                        [32m[K)[m[K
2025-11-09T02:11:22.7245612Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7246220Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2002:7:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-09T02:11:22.7246500Z  2002 | msg_t [01;31m[KScanningCoordinator[m[K::coordinated_scanning_thread() {
2025-11-09T02:11:22.7246617Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7256624Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kmsg_t coordinated_scanning_thread()[m[K':
2025-11-09T02:11:22.7258841Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2003:12:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.7260513Z  2003 |     while ([01;31m[Kscanning_active_[m[K && !chThdShouldTerminateX()) {
2025-11-09T02:11:22.7261184Z       |            [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7274845Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2003:33:[m[K [01;31m[Kerror: [m[K'[01m[KchThdShouldTerminateX[m[K' was not declared in this scope; did you mean '[01m[KchThdShouldTerminate[m[K'?
2025-11-09T02:11:22.7276204Z  2003 |     while (scanning_active_ && ![01;31m[KchThdShouldTerminateX[m[K()) {
2025-11-09T02:11:22.7276630Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7276965Z       |                                 [32m[KchThdShouldTerminate[m[K
2025-11-09T02:11:22.7282944Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2004:13:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-09T02:11:22.7283763Z  2004 |         if ([01;31m[Kscanner_[m[K.is_scanning_active()) {
2025-11-09T02:11:22.7284091Z       |             [01;31m[K^~~~~~~~[m[K
2025-11-09T02:11:22.7292740Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2005:13:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-09T02:11:22.7294176Z  2005 |             [01;31m[Khardware_[m[K.update_spectrum_for_scanner();
2025-11-09T02:11:22.7294909Z       |             [01;31m[K^~~~~~~~~[m[K
2025-11-09T02:11:22.7312203Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2008:13:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.7313160Z  2008 |             [01;31m[Kdisplay_controller_[m[K.update_detection_display(scanner_);
2025-11-09T02:11:22.7313549Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7328809Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2010:17:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_controller_[m[K' was not declared in this scope
2025-11-09T02:11:22.7329643Z  2010 |             if ([01;31m[Kaudio_controller_[m[K.is_audio_enabled() &&
2025-11-09T02:11:22.7329989Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7346692Z In file included from [01m[K/havoc/firmware/chibios/os/kernel/include/ch.h:115[m[K,
2025-11-09T02:11:22.7347416Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:8[m[K,
2025-11-09T02:11:22.7348205Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18[m[K,
2025-11-09T02:11:22.7349153Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.7350162Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2015:32:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_interval_ms_[m[K' was not declared in this scope
2025-11-09T02:11:22.7350992Z  2015 |         chThdSleepMilliseconds([01;31m[Kscan_interval_ms_[m[K);
2025-11-09T02:11:22.7351375Z       |                                [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7351987Z [01m[K/havoc/firmware/chibios/os/kernel/include/chvt.h:67:30:[m[K [01;36m[Knote: [m[Kin definition of macro '[01m[KMS2ST[m[K'
2025-11-09T02:11:22.7352684Z    67 |   ((systime_t)(((((uint32_t)([01;36m[Kmsec[m[K)) * ((uint32_t)CH_FREQUENCY) - 1UL) /     \
2025-11-09T02:11:22.7353085Z       |                              [01;36m[K^~~~[m[K
2025-11-09T02:11:22.7353901Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2015:9:[m[K [01;36m[Knote: [m[Kin expansion of macro '[01m[KchThdSleepMilliseconds[m[K'
2025-11-09T02:11:22.7354853Z  2015 |         [01;36m[KchThdSleepMilliseconds[m[K(scan_interval_ms_);
2025-11-09T02:11:22.7355347Z       |         [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7363197Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2017:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-09T02:11:22.7363978Z  2017 |     [01;31m[Kscanning_active_[m[K = false;
2025-11-09T02:11:22.7364282Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7377589Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2018:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-09T02:11:22.7378570Z  2018 |     [01;31m[Kscanning_thread_[m[K = nullptr;
2025-11-09T02:11:22.7378878Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7379156Z       |     [32m[Kstop_scanning_thread[m[K
2025-11-09T02:11:22.7379738Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7380777Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2023:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-09T02:11:22.7381757Z  2023 | void [01;31m[KScanningCoordinator[m[K::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
2025-11-09T02:11:22.7382240Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7395208Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_runtime_parameters(const DroneAnalyzerSettings&)[m[K':
2025-11-09T02:11:22.7396530Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2024:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_interval_ms_[m[K' was not declared in this scope
2025-11-09T02:11:22.7397331Z  2024 |     [01;31m[Kscan_interval_ms_[m[K = settings.scan_interval_ms;
2025-11-09T02:11:22.7397684Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7398557Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7400543Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2027:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-09T02:11:22.7401959Z  2027 | void [01;31m[KScanningCoordinator[m[K::show_session_summary(const std::string& summary) {
2025-11-09T02:11:22.7402393Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-09T02:11:22.7403324Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_session_summary(const string&)[m[K':
2025-11-09T02:11:22.7404468Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2028:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-09T02:11:22.7405392Z  2028 |     [01;31m[Knav_[m[K.display_modal("Session Summary", summary.c_str());
2025-11-09T02:11:22.7405736Z       |     [01;31m[K^~~~[m[K
2025-11-09T02:11:22.7406275Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-09T02:11:22.7407281Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2031:1:[m[K [01;31m[Kerror: [m[Kexpected declaration before '[01m[K}[m[K' token
2025-11-09T02:11:22.7408094Z  2031 | [01;31m[K}[m[K // namespace ui::external_app::enhanced_drone_analyzer
2025-11-09T02:11:22.7408432Z       | [01;31m[K^[m[K
2025-11-09T02:11:22.7948568Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.7952338Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.7954198Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.7956862Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-09T02:11:22.7958756Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.7960438Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kbegin(const char* const&)[m[K'
2025-11-09T02:11:22.7961625Z    89 |         : path{[01;31m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-09T02:11:22.7962220Z       |                [01;31m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.7963115Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-09T02:11:22.7963778Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.7964451Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.7965371Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.7966550Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::begin(std::initializer_list<_Tp>)[m[K'
2025-11-09T02:11:22.7967392Z    89 |     [01;36m[Kbegin[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-09T02:11:22.7967716Z       |     [01;36m[K^~~~~[m[K
2025-11-09T02:11:22.7968379Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.7969274Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.7970074Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.7970829Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.7971812Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-09T02:11:22.7972714Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-09T02:11:22.7973059Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.7973528Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.7974225Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.7975083Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.7976257Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&)[m[K'
2025-11-09T02:11:22.7977140Z    48 |     [01;36m[Kbegin[m[K(_Container& __cont) -> decltype(__cont.begin())
2025-11-09T02:11:22.7977485Z       |     [01;36m[K^~~~~[m[K
2025-11-09T02:11:22.7978129Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.7979769Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&) [with _Container = const char* const][m[K':
2025-11-09T02:11:22.7981284Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-09T02:11:22.7982206Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.7983350Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:50:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.7984209Z    48 |     begin(_Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-09T02:11:22.7984598Z       |                                           [01;31m[K~~~~~~~^~~~~[m[K
2025-11-09T02:11:22.7985411Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-09T02:11:22.7986329Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.7987568Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&)[m[K'
2025-11-09T02:11:22.7988454Z    58 |     [01;36m[Kbegin[m[K(const _Container& __cont) -> decltype(__cont.begin())
2025-11-09T02:11:22.7988807Z       |     [01;36m[K^~~~~[m[K
2025-11-09T02:11:22.7989436Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.7990745Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&) [with _Container = const char*][m[K':
2025-11-09T02:11:22.7991989Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-09T02:11:22.7992903Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.7994047Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:56:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.7995214Z    58 |     begin(const _Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-09T02:11:22.7995643Z       |                                                 [01;31m[K~~~~~~~^~~~~[m[K
2025-11-09T02:11:22.7996338Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-09T02:11:22.7997245Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.7998421Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::begin(_Tp (&)[_Nm])[m[K'
2025-11-09T02:11:22.7999167Z    87 |     [01;36m[Kbegin[m[K(_Tp (&__arr)[_Nm])
2025-11-09T02:11:22.7999442Z       |     [01;36m[K^~~~~[m[K
2025-11-09T02:11:22.8000064Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8001071Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.8001868Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.8002614Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8003453Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8004107Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-09T02:11:22.8004452Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8005041Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.8005737Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.8006485Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8007525Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::begin(std::valarray<_Tp>&)[m[K'
2025-11-09T02:11:22.8008299Z   104 |   template<typename _Tp> _Tp* [01;36m[Kbegin[m[K(valarray<_Tp>&);
2025-11-09T02:11:22.8008673Z       |                               [01;36m[K^~~~~[m[K
2025-11-09T02:11:22.8009339Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8010224Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.8011011Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.8011754Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8012677Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8013360Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-09T02:11:22.8013713Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8014302Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.8015103Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.8015865Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8016961Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::begin(const std::valarray<_Tp>&)[m[K'
2025-11-09T02:11:22.8017814Z   105 |   template<typename _Tp> const _Tp* [01;36m[Kbegin[m[K(const valarray<_Tp>&);
2025-11-09T02:11:22.8018224Z       |                                     [01;36m[K^~~~~[m[K
2025-11-09T02:11:22.8018894Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8019786Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.8020682Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.8021431Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8022389Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8023090Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-09T02:11:22.8023433Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8024081Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kend(const char* const&)[m[K'
2025-11-09T02:11:22.8024822Z    89 |         : path{std::begin(source), [01;31m[Kstd::end(source)[m[K} {
2025-11-09T02:11:22.8025181Z       |                                    [01;31m[K~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8025702Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-09T02:11:22.8026308Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.8026962Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.8027711Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8028813Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::end(std::initializer_list<_Tp>)[m[K'
2025-11-09T02:11:22.8029608Z    99 |     [01;36m[Kend[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-09T02:11:22.8029912Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.8030543Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8031424Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.8032203Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.8032947Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8033884Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-09T02:11:22.8034809Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-09T02:11:22.8035177Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8035649Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.8036334Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.8037078Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8038225Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&)[m[K'
2025-11-09T02:11:22.8039056Z    68 |     [01;36m[Kend[m[K(_Container& __cont) -> decltype(__cont.end())
2025-11-09T02:11:22.8039376Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.8040009Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8041414Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&) [with _Container = const char* const][m[K':
2025-11-09T02:11:22.8042646Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-09T02:11:22.8043566Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.8044811Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:48:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8045660Z    68 |     end(_Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-09T02:11:22.8046042Z       |                                         [01;31m[K~~~~~~~^~~[m[K
2025-11-09T02:11:22.8046723Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-09T02:11:22.8047631Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.8048859Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&)[m[K'
2025-11-09T02:11:22.8049723Z    78 |     [01;36m[Kend[m[K(const _Container& __cont) -> decltype(__cont.end())
2025-11-09T02:11:22.8050062Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.8050684Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8051974Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&) [with _Container = const char*][m[K':
2025-11-09T02:11:22.8053199Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-09T02:11:22.8054106Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.8055321Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:54:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8056296Z    78 |     end(const _Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-09T02:11:22.8056690Z       |                                               [01;31m[K~~~~~~~^~~[m[K
2025-11-09T02:11:22.8057378Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-09T02:11:22.8058274Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-09T02:11:22.8059444Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::end(_Tp (&)[_Nm])[m[K'
2025-11-09T02:11:22.8060184Z    97 |     [01;36m[Kend[m[K(_Tp (&__arr)[_Nm])
2025-11-09T02:11:22.8060453Z       |     [01;36m[K^~~[m[K
2025-11-09T02:11:22.8061079Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8062116Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.8062911Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.8063666Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8064503Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8065247Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-09T02:11:22.8065618Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8066085Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.8066779Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.8067523Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8068568Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::end(std::valarray<_Tp>&)[m[K'
2025-11-09T02:11:22.8069334Z   106 |   template<typename _Tp> _Tp* [01;36m[Kend[m[K(valarray<_Tp>&);
2025-11-09T02:11:22.8069694Z       |                               [01;36m[K^~~[m[K
2025-11-09T02:11:22.8070372Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8071264Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.8072084Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.8072833Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8073761Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8074428Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-09T02:11:22.8074877Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8075460Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-09T02:11:22.8076152Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-09T02:11:22.8076898Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8077977Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::end(const std::valarray<_Tp>&)[m[K'
2025-11-09T02:11:22.8078808Z   107 |   template<typename _Tp> const _Tp* [01;36m[Kend[m[K(const valarray<_Tp>&);
2025-11-09T02:11:22.8079201Z       |                                     [01;36m[K^~~[m[K
2025-11-09T02:11:22.8079872Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8080765Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-09T02:11:22.8081654Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-09T02:11:22.8082408Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-09T02:11:22.8083358Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-09T02:11:22.8084048Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-09T02:11:22.8084405Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-09T02:11:22.8085346Z [01m[K/havoc/firmware/application/./file.hpp:89:52:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kstd::filesystem::path::path(<brace-enclosed initializer list>)[m[K'
2025-11-09T02:11:22.8086093Z    89 |         : path{std::begin(source), std::end(source)[01;31m[K}[m[K {
2025-11-09T02:11:22.8086451Z       |                                                    [01;31m[K^[m[K
2025-11-09T02:11:22.8087113Z [01m[K/havoc/firmware/application/./file.hpp:102:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const TCHAR*)[m[K'
2025-11-09T02:11:22.8087690Z   102 |     [01;36m[Kpath[m[K(const TCHAR* const s)
2025-11-09T02:11:22.8087966Z       |     [01;36m[K^~~~[m[K
2025-11-09T02:11:22.8088473Z [01m[K/havoc/firmware/application/./file.hpp:102:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-09T02:11:22.8089325Z [01m[K/havoc/firmware/application/./file.hpp:98:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const char16_t*)[m[K'
2025-11-09T02:11:22.8089910Z    98 |     [01;36m[Kpath[m[K(const char16_t* const s)
2025-11-09T02:11:22.8090184Z       |     [01;36m[K^~~~[m[K
2025-11-09T02:11:22.8090674Z [01m[K/havoc/firmware/application/./file.hpp:98:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-09T02:11:22.8091629Z [01m[K/havoc/firmware/application/./file.hpp:93:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class InputIt> std::filesystem::path::path(InputIt, InputIt)[m[K'
2025-11-09T02:11:22.8092269Z    93 |     [01;36m[Kpath[m[K(InputIt first,
2025-11-09T02:11:22.8092535Z       |     [01;36m[K^~~~[m[K
2025-11-09T02:11:22.8093045Z [01m[K/havoc/firmware/application/./file.hpp:93:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8094001Z [01m[K/havoc/firmware/application/./file.hpp:88:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class Source> std::filesystem::path::path(const Source&)[m[K'
2025-11-09T02:11:22.8094867Z    88 |     [01;36m[Kpath[m[K(const Source& source)
2025-11-09T02:11:22.8095138Z       |     [01;36m[K^~~~[m[K
2025-11-09T02:11:22.8095654Z [01m[K/havoc/firmware/application/./file.hpp:88:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-09T02:11:22.8096550Z [01m[K/havoc/firmware/application/./file.hpp:83:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(std::filesystem::path&&)[m[K'
2025-11-09T02:11:22.8097129Z    83 |     [01;36m[Kpath[m[K(path&& p)
2025-11-09T02:11:22.8097389Z       |     [01;36m[K^~~~[m[K
2025-11-09T02:11:22.8097879Z [01m[K/havoc/firmware/application/./file.hpp:83:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-09T02:11:22.8098781Z [01m[K/havoc/firmware/application/./file.hpp:79:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const std::filesystem::path&)[m[K'
2025-11-09T02:11:22.8099371Z    79 |     [01;36m[Kpath[m[K(const path& p)
2025-11-09T02:11:22.8099641Z       |     [01;36m[K^~~~[m[K
2025-11-09T02:11:22.8100124Z [01m[K/havoc/firmware/application/./file.hpp:79:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-09T02:11:22.8101048Z [01m[K/havoc/firmware/application/./file.hpp:75:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path()[m[K'
2025-11-09T02:11:22.8101559Z    75 |     [01;36m[Kpath[m[K()
2025-11-09T02:11:22.8101793Z       |     [01;36m[K^~~~[m[K
2025-11-09T02:11:22.8102290Z [01m[K/havoc/firmware/application/./file.hpp:75:5:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 2 provided
2025-11-09T02:11:22.8886550Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_bg_color()[m[K':
2025-11-09T02:11:22.8888033Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1096:1:[m[K [01;35m[Kwarning: [m[Kcontrol reaches end of non-void function [[01;35m[K-Wreturn-type[m[K]
2025-11-09T02:11:22.8888792Z  1096 | [01;35m[K}[m[K
2025-11-09T02:11:22.8889008Z       | [01;35m[K^[m[K
2025-11-09T02:11:22.8889732Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_text_color()[m[K':
2025-11-09T02:11:22.8890930Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1108:1:[m[K [01;35m[Kwarning: [m[Kcontrol reaches end of non-void function [[01;35m[K-Wreturn-type[m[K]
2025-11-09T02:11:22.8891647Z  1108 | [01;35m[K}[m[K
2025-11-09T02:11:22.8891858Z       | [01;35m[K^[m[K
2025-11-09T02:11:22.8919829Z At global scope:
2025-11-09T02:11:22.8920604Z [01m[Kcc1plus:[m[K [01;35m[Kwarning: [m[Kunrecognized command line option '[01m[K-Wno-volatile[m[K'
2025-11-09T02:11:22.8959292Z make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5775: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
2025-11-09T02:11:22.8964422Z make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
2025-11-09T02:11:22.8967123Z make: *** [Makefile:101: all] Error 2
2025-11-09T02:11:22.9707608Z ##[error]Process completed with exit code 2.
