2025-11-08T17:45:00.0000000Z [100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj

## INTEGRATION STATUS - PHASE 5 FIXES APPLIED, SYNTAX ERRORS DETECTED

### 1. File Structure Status
- ✅ **Complete file rewrite** - ui_scanner_combined.cpp recreated from scratch
- ✅ **Header file alignment** - All class implementations follow header specifications
- ✅ **Namespace corrections** - Proper ui::external_app::enhanced_drone_analyzer namespace usage
- ✅ **Include file cleanup** - Removed redundant and conflicting includes

### 2. Phase 5 API Fixes Applied
- ✅ **RadioState API corrections** - Changed tune_to_frequency to use radio_state_.tune_rf()
- ✅ **Spectrum streaming fixes** - Changed stop_spectrum_streaming to use baseband::spectrum_streaming_stop()
- ✅ **Message handler added** - Added ChannelSpectrum message handler registration
- ✅ **Missing method added** - Added get_current_radio_frequency() in DroneScanner
- ✅ **Header declaration added** - Added message_handler_spectrum_ in DroneHardwareController

### 3. Compilation Status (CRITICAL SYNTAX ERRORS)
- 🔴 **Syntax errors detected** - Multiple "expected a declaration" and "incomplete type" errors
- 🔴 **DroneHardwareController incomplete** - Class definition issues causing 50+ errors
- 🔴 **Declaration issues** - Lines 655, 779-892 have syntax problems
- 🔴 **Type qualifier errors** - Non-member function qualifiers on member functions
- 🔴 **Missing implementations** - Some methods declared but not properly implemented

### 4. Critical Issues Identified
- 🔴 **Incomplete type "DroneHardwareController"** - Class not fully recognized by compiler
- 🔴 **Expected declarations missing** - Syntax errors preventing proper parsing
- 🔴 **Member access issues** - 'this' used outside member functions
- 🔴 **Function signature mismatches** - Type qualifiers on non-member functions

### 5. Session Summary
**Progress Made:**
- Applied RadioState API corrections as per Phase 5
- Fixed spectrum streaming API calls
- Added missing message handler for spectrum data
- Added missing method implementation

**Next Steps (Immediate Priority):**
1. **Fix syntax errors** - Resolve "expected a declaration" errors
2. **Complete DroneHardwareController** - Ensure all methods are properly implemented
3. **Fix incomplete type issues** - Resolve class definition problems
4. **Validate namespace closure** - Ensure proper C++ syntax
5. **Compile and test** - Run build to verify fixes work
6. **Iterate on remaining errors** - Address any new issues that arise

**Status:** Phase 5 API fixes applied but syntax errors introduced. Major compilation errors due to incomplete implementations and syntax issues. Need immediate syntax fixes before proceeding.


2025-11-08T13:22:47.6444555Z [100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
2025-11-08T13:22:48.3914941Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-08T13:22:48.3916033Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11[m[K,
2025-11-08T13:22:48.3917317Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31[m[K:
2025-11-08T13:22:48.3921771Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::ScanningCoordinator[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:48.3923003Z    18 | class [01;35m[KScanningCoordinator[m[K {
2025-11-08T13:22:48.3923347Z       |       [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:48.3924876Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:48.3927225Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:48.4117747Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:11[m[K,
2025-11-08T13:22:48.4119128Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31[m[K:
2025-11-08T13:22:48.4121622Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:48.4122993Z   300 | class [01;35m[KDroneScanner[m[K {
2025-11-08T13:22:48.4123342Z       |       [01;35m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:48.4124931Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:48.4127414Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:48.8189548Z [01m[Kcc1plus:[m[K [01;35m[Kwarning: [m[Kunrecognized command line option '[01m[K-Wno-volatile[m[K'
2025-11-08T13:22:48.8817092Z [100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj
2025-11-08T13:22:48.9795337Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1130:27:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-08T13:22:48.9797527Z  1130 |         progress_bar[i] = [01;35m[K'█'[m[K;
2025-11-08T13:22:48.9798333Z       |                           [01;35m[K^~~~~[m[K
2025-11-08T13:22:48.9800160Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1379:61:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-08T13:22:48.9802541Z  1379 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'▲'[m[K; break;
2025-11-08T13:22:48.9803393Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-08T13:22:48.9804886Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1380:58:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-08T13:22:48.9806549Z  1380 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'▼'[m[K; break;
2025-11-08T13:22:48.9807603Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-08T13:22:48.9809234Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1383:37:[m[K [01;35m[Kwarning: [m[Kmulti-character character constant [[01;35m[K-Wmultichar[m[K]
2025-11-08T13:22:48.9810806Z  1383 |             default: trend_symbol = [01;35m[K'■'[m[K; break;
2025-11-08T13:22:48.9811509Z       |                                     [01;35m[K^~~~~[m[K
2025-11-08T13:22:49.6240732Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-08T13:22:49.6241828Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.6243680Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::ScanningCoordinator[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:49.6245260Z    18 | class [01;35m[KScanningCoordinator[m[K {
2025-11-08T13:22:49.6245663Z       |       [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.6247555Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::ScanningCoordinator(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:49.6249634Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:18:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::ScanningCoordinator&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:49.6460734Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.6464042Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K'[01m[Kclass ui::external_app::enhanced_drone_analyzer::DroneScanner[m[K' has pointer data members [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:49.6465502Z   300 | class [01;35m[KDroneScanner[m[K {
2025-11-08T13:22:49.6465894Z       |       [01;35m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.6467758Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  but does not override '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:49.6470019Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:300:7:[m[K [01;35m[Kwarning: [m[K  or '[01m[Koperator=(const ui::external_app::enhanced_drone_analyzer::DroneScanner&)[m[K' [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:49.7349142Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:35:27:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const uint32_t SCAN_THREAD_STACK_SIZE[m[K'
2025-11-08T13:22:49.7353000Z    35 | static constexpr uint32_t [01;31m[KSCAN_THREAD_STACK_SIZE[m[K = 2048;
2025-11-08T13:22:49.7353835Z       |                           [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7354997Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7357355Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:62:27:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const uint32_t SCAN_THREAD_STACK_SIZE[m[K' previously defined here
2025-11-08T13:22:49.7359049Z    62 | static constexpr uint32_t [01;36m[KSCAN_THREAD_STACK_SIZE[m[K = 2048;
2025-11-08T13:22:49.7359829Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7361466Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:36:26:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB[m[K'
2025-11-08T13:22:49.7363183Z    36 | static constexpr int32_t [01;31m[KDEFAULT_RSSI_THRESHOLD_DB[m[K = -90;
2025-11-08T13:22:49.7363991Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7365106Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18[m[K,
2025-11-08T13:22:49.7366571Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7368696Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:210:26:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const int32_t DEFAULT_RSSI_THRESHOLD_DB[m[K' previously defined here
2025-11-08T13:22:49.7370736Z   210 | static constexpr int32_t [01;36m[KDEFAULT_RSSI_THRESHOLD_DB[m[K = -90;
2025-11-08T13:22:49.7371499Z       |                          [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7373325Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:37:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MAX_TRACKED_DRONES[m[K'
2025-11-08T13:22:49.7374935Z    37 | static constexpr size_t [01;31m[KMAX_TRACKED_DRONES[m[K = 8;
2025-11-08T13:22:49.7375649Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7376959Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7379006Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:153:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MAX_TRACKED_DRONES[m[K' previously defined here
2025-11-08T13:22:49.7380596Z   153 | static constexpr size_t [01;36m[KMAX_TRACKED_DRONES[m[K = 8;
2025-11-08T13:22:49.7381343Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7383028Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:41:26:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const int32_t HYSTERESIS_MARGIN_DB[m[K'
2025-11-08T13:22:49.7384741Z    41 | static constexpr int32_t [01;31m[KHYSTERESIS_MARGIN_DB[m[K = 5;
2025-11-08T13:22:49.7385500Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7386923Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:19[m[K,
2025-11-08T13:22:49.7388336Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7390222Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:14:26:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const int16_t HYSTERESIS_MARGIN_DB[m[K'
2025-11-08T13:22:49.7391773Z    14 | static constexpr int16_t [01;36m[KHYSTERESIS_MARGIN_DB[m[K = 5;
2025-11-08T13:22:49.7392492Z       |                          [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7394087Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:43:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency WIDEBAND_DEFAULT_MIN[m[K'
2025-11-08T13:22:49.7395762Z    43 | static constexpr Frequency [01;31m[KWIDEBAND_DEFAULT_MIN[m[K = 2400000000ULL;
2025-11-08T13:22:49.7396536Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7397820Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7399882Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:159:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_DEFAULT_MIN[m[K'
2025-11-08T13:22:49.7401604Z   159 | static constexpr uint32_t [01;36m[KWIDEBAND_DEFAULT_MIN[m[K = 2'400'000'000ULL;
2025-11-08T13:22:49.7402428Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7404090Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:44:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency WIDEBAND_DEFAULT_MAX[m[K'
2025-11-08T13:22:49.7405834Z    44 | static constexpr Frequency [01;31m[KWIDEBAND_DEFAULT_MAX[m[K = 2500000000ULL;
2025-11-08T13:22:49.7406583Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7408259Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7410330Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:160:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_DEFAULT_MAX[m[K'
2025-11-08T13:22:49.7412279Z   160 | static constexpr uint32_t [01;36m[KWIDEBAND_DEFAULT_MAX[m[K = 2'500'000'000ULL;
2025-11-08T13:22:49.7413114Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7414730Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:45:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency WIDEBAND_SLICE_WIDTH[m[K'
2025-11-08T13:22:49.7416430Z    45 | static constexpr Frequency [01;31m[KWIDEBAND_SLICE_WIDTH[m[K = 20000000ULL;
2025-11-08T13:22:49.7417398Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7418513Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7420635Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:161:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_SLICE_WIDTH[m[K'
2025-11-08T13:22:49.7422279Z   161 | static constexpr uint32_t [01;36m[KWIDEBAND_SLICE_WIDTH[m[K = 25'000'000;
2025-11-08T13:22:49.7423120Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7424726Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:46:25:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const size_t WIDEBAND_MAX_SLICES[m[K'
2025-11-08T13:22:49.7426385Z    46 | static constexpr size_t [01;31m[KWIDEBAND_MAX_SLICES[m[K = 8;
2025-11-08T13:22:49.7427331Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7428437Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7430539Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:162:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t WIDEBAND_MAX_SLICES[m[K'
2025-11-08T13:22:49.7432128Z   162 | static constexpr uint32_t [01;36m[KWIDEBAND_MAX_SLICES[m[K = 20;
2025-11-08T13:22:49.7432864Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7434369Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:47:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t DETECTION_TABLE_SIZE[m[K'
2025-11-08T13:22:49.7435878Z    47 | static constexpr size_t [01;31m[KDETECTION_TABLE_SIZE[m[K = 1024;
2025-11-08T13:22:49.7436589Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7437912Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7439997Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:163:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t DETECTION_TABLE_SIZE[m[K' previously defined here
2025-11-08T13:22:49.7441578Z   163 | static constexpr size_t [01;36m[KDETECTION_TABLE_SIZE[m[K = 256;
2025-11-08T13:22:49.7442268Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7443763Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:48:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MAX_DISPLAYED_DRONES[m[K'
2025-11-08T13:22:49.7445281Z    48 | static constexpr size_t [01;31m[KMAX_DISPLAYED_DRONES[m[K = 8;
2025-11-08T13:22:49.7446309Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7447608Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7449832Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:154:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MAX_DISPLAYED_DRONES[m[K' previously defined here
2025-11-08T13:22:49.7451369Z   154 | static constexpr size_t [01;36m[KMAX_DISPLAYED_DRONES[m[K = 3;
2025-11-08T13:22:49.7452059Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7453690Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:50:30:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const char* const default_gradient_file[m[K'
2025-11-08T13:22:49.7455464Z    50 | static constexpr const char* [01;31m[Kdefault_gradient_file[m[K = "/sdcard/GRADIENT.BMP";
2025-11-08T13:22:49.7456309Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7457630Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:22[m[K,
2025-11-08T13:22:49.7459126Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7461336Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:31:36:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconst std::filesystem::path default_gradient_file[m[K'
2025-11-08T13:22:49.7462904Z    31 | extern const std::filesystem::path [01;36m[Kdefault_gradient_file[m[K;
2025-11-08T13:22:49.7463674Z       |                                    [01;36m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7465186Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:51:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MINI_SPECTRUM_WIDTH[m[K'
2025-11-08T13:22:49.7466920Z    51 | static constexpr size_t [01;31m[KMINI_SPECTRUM_WIDTH[m[K = 240;
2025-11-08T13:22:49.7467654Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7468736Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7470832Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:155:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MINI_SPECTRUM_WIDTH[m[K' previously defined here
2025-11-08T13:22:49.7472479Z   155 | static constexpr size_t [01;36m[KMINI_SPECTRUM_WIDTH[m[K = 200;
2025-11-08T13:22:49.7473222Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7474768Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:52:25:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const size_t MINI_SPECTRUM_HEIGHT[m[K'
2025-11-08T13:22:49.7476315Z    52 | static constexpr size_t [01;31m[KMINI_SPECTRUM_HEIGHT[m[K = 64;
2025-11-08T13:22:49.7477226Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7478355Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7480455Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:156:25:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const size_t MINI_SPECTRUM_HEIGHT[m[K' previously defined here
2025-11-08T13:22:49.7482072Z   156 | static constexpr size_t [01;36m[KMINI_SPECTRUM_HEIGHT[m[K = 24;
2025-11-08T13:22:49.7482797Z       |                         [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7484587Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:53:28:[m[K [01;31m[Kerror: [m[Kconflicting declaration '[01m[Kconstexpr const Frequency MIN_HARDWARE_FREQ[m[K'
2025-11-08T13:22:49.7486151Z    53 | static constexpr Frequency [01;31m[KMIN_HARDWARE_FREQ[m[K = 50000000ULL;
2025-11-08T13:22:49.7487070Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7498559Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7500679Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:157:27:[m[K [01;36m[Knote: [m[Kprevious declaration as '[01m[Kconstexpr const uint32_t MIN_HARDWARE_FREQ[m[K'
2025-11-08T13:22:49.7502345Z   157 | static constexpr uint32_t [01;36m[KMIN_HARDWARE_FREQ[m[K = 1'000'000;
2025-11-08T13:22:49.7503126Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7504693Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:54:28:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kconstexpr const Frequency MAX_HARDWARE_FREQ[m[K'
2025-11-08T13:22:49.7506318Z    54 | static constexpr Frequency [01;31m[KMAX_HARDWARE_FREQ[m[K = 6000000000ULL;
2025-11-08T13:22:49.7507431Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7508679Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7510775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:158:27:[m[K [01;36m[Knote: [m[K'[01m[Kconstexpr const uint64_t MAX_HARDWARE_FREQ[m[K' previously defined here
2025-11-08T13:22:49.7512440Z   158 | static constexpr uint64_t [01;36m[KMAX_HARDWARE_FREQ[m[K = 6'000'000'000ULL;
2025-11-08T13:22:49.7513181Z       |                           [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7514988Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::DroneScanner()[m[K':
2025-11-08T13:22:49.7518197Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:377:10:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_active_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7520189Z   377 |     bool [01;35m[Kscanning_active_[m[K = false;
2025-11-08T13:22:49.7520804Z       |          [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7522936Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:375:13:[m[K [01;35m[Kwarning: [m[K  '[01m[KThread* ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_thread_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7524755Z   375 |     Thread* [01;35m[Kscanning_thread_[m[K = nullptr;
2025-11-08T13:22:49.7525399Z       |             [01;35m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7527054Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7528462Z   145 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-08T13:22:49.7529009Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7530082Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7532758Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:401:22:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::wideband_scan_data_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7534867Z   401 |     WidebandScanData [01;35m[Kwideband_scan_data_[m[K;
2025-11-08T13:22:49.7535410Z       |                      [01;35m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7537750Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:387:18:[m[K [01;35m[Kwarning: [m[K  '[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::ScanningMode ui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_mode_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7539678Z   387 |     ScanningMode [01;35m[Kscanning_mode_[m[K = ScanningMode::DATABASE;
2025-11-08T13:22:49.7540363Z       |                  [01;35m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7541636Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7542886Z   145 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-08T13:22:49.7543371Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7544306Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7546806Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:387:18:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::scanning_mode_[m[K' will be initialized after [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7548634Z   387 |     ScanningMode [01;35m[Kscanning_mode_[m[K = ScanningMode::DATABASE;
2025-11-08T13:22:49.7549211Z       |                  [01;35m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7550890Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:379:16:[m[K [01;35m[Kwarning: [m[K  '[01m[Kfreqman_db ui::external_app::enhanced_drone_analyzer::DroneScanner::freq_db_[m[K' [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7552303Z   379 |     freqman_db [01;35m[Kfreq_db_[m[K;
2025-11-08T13:22:49.7552744Z       |                [01;35m[K^~~~~~~~[m[K
2025-11-08T13:22:49.7553945Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K  when initialized here [[01;35m[K-Wreorder[m[K]
2025-11-08T13:22:49.7555113Z   145 | [01;35m[KDroneScanner[m[K::DroneScanner()
2025-11-08T13:22:49.7555573Z       | [01;35m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7557715Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:145:1:[m[K [01;35m[Kwarning: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DroneScanner::drone_database_[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:49.7560613Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::initialize_database_and_scanner()[m[K':
2025-11-08T13:22:49.7563545Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:185:14:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kopen[m[K'
2025-11-08T13:22:49.7565266Z   185 |     freq_db_.[01;31m[Kopen[m[K(db_path, false);
2025-11-08T13:22:49.7565746Z       |              [01;31m[K^~~~[m[K
2025-11-08T13:22:49.7567542Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kbool ui::external_app::enhanced_drone_analyzer::DroneScanner::load_frequency_database()[m[K':
2025-11-08T13:22:49.7570200Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:274:14:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kclose[m[K'
2025-11-08T13:22:49.7572167Z   274 |     freq_db_.[01;31m[Kclose[m[K();
2025-11-08T13:22:49.7572630Z       |              [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.7574908Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:278:19:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kopen[m[K'
2025-11-08T13:22:49.7576898Z   278 |     if (!freq_db_.[01;31m[Kopen[m[K(db_path, false)) {
2025-11-08T13:22:49.7577434Z       |                   [01;31m[K^~~~[m[K
2025-11-08T13:22:49.7579596Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:282:18:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-08T13:22:49.7581409Z   282 |     if (freq_db_.[01;31m[Kentry_count[m[K() > 100) {
2025-11-08T13:22:49.7581921Z       |                  [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.7583537Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Ksize_t ui::external_app::enhanced_drone_analyzer::DroneScanner::get_database_size() const[m[K':
2025-11-08T13:22:49.7586318Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:291:21:[m[K [01;31m[Kerror: [m[K'[01m[Kconst freqman_db[m[K' {aka '[01m[Kconst class std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-08T13:22:49.7588072Z   291 |     return freq_db_.[01;31m[Kentry_count[m[K();
2025-11-08T13:22:49.7588589Z       |                     [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.7590652Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In member function '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::perform_database_scan_cycle(ui::external_app::enhanced_drone_analyzer::DroneHardwareController&)[m[K':
2025-11-08T13:22:49.7594000Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:339:43:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-08T13:22:49.7595859Z   339 |     const size_t total_entries = freq_db_.[01;31m[Kentry_count[m[K();
2025-11-08T13:22:49.7596495Z       |                                           [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.7598754Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:344:38:[m[K [01;31m[Kerror: [m[K'[01m[Kusing freqman_db = class std::vector<std::unique_ptr<freqman_entry> >[m[K' {aka '[01m[Kclass std::vector<std::unique_ptr<freqman_entry> >[m[K'} has no member named '[01m[Kentry_count[m[K'
2025-11-08T13:22:49.7600619Z   344 |     if (current_db_index_ < freq_db_.[01;31m[Kentry_count[m[K()) {
2025-11-08T13:22:49.7601215Z       |                                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.7602675Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:346:46:[m[K [01;31m[Kerror: [m[K'[01m[Kconst class std::unique_ptr<freqman_entry>[m[K' has no member named '[01m[Kfrequency_a[m[K'
2025-11-08T13:22:49.7604128Z   346 |             Frequency target_freq_hz = entry.[01;31m[Kfrequency_a[m[K;
2025-11-08T13:22:49.7604781Z       |                                              [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.7606851Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:350:44:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kconst std::unique_ptr<freqman_entry>[m[K' to '[01m[Kconst freqman_entry&[m[K'
2025-11-08T13:22:49.7608407Z   350 |                     process_rssi_detection([01;31m[Kentry[m[K, real_rssi);
2025-11-08T13:22:49.7609173Z       |                                            [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.7609696Z       |                                            [01;31m[K|[m[K
2025-11-08T13:22:49.7610368Z       |                                            [01;31m[Kconst std::unique_ptr<freqman_entry>[m[K
2025-11-08T13:22:49.7611434Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7613850Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:327:54:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::external_app::enhanced_drone_analyzer::DroneScanner::process_rssi_detection(const freqman_entry&, int32_t)[m[K'
2025-11-08T13:22:49.7615756Z   327 |     void process_rssi_detection([01;36m[Kconst freqman_entry& entry[m[K, int32_t rssi);
2025-11-08T13:22:49.7616492Z       |                                 [01;36m[K~~~~~~~~~~~~~~~~~~~~~^~~~~[m[K
2025-11-08T13:22:49.7617653Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.7619334Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:356:7:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[Kelse[m[K'
2025-11-08T13:22:49.7620444Z   356 |     } [01;31m[Kelse[m[K {
2025-11-08T13:22:49.7620849Z       |       [01;31m[K^~~~[m[K
2025-11-08T13:22:49.7622047Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:361:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.7623607Z   361 | void [01;31m[KDroneScanner[m[K::perform_wideband_scan_cycle(DroneHardwareController& hardware) {
2025-11-08T13:22:49.7624325Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7625654Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid perform_wideband_scan_cycle(DroneHardwareController&)[m[K':
2025-11-08T13:22:49.7628039Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:362:9:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_scan_data_[m[K' was not declared in this scope; did you mean '[01m[KWidebandScanData[m[K'?
2025-11-08T13:22:49.7629525Z   362 |     if ([01;31m[Kwideband_scan_data_[m[K.slices_nb == 0) {
2025-11-08T13:22:49.7630079Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7630570Z       |         [32m[KWidebandScanData[m[K
2025-11-08T13:22:49.7631970Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:363:9:[m[K [01;31m[Kerror: [m[K'[01m[Ksetup_wideband_range[m[K' was not declared in this scope
2025-11-08T13:22:49.7633430Z   363 |         [01;31m[Ksetup_wideband_range[m[K(WIDEBAND_DEFAULT_MIN, WIDEBAND_DEFAULT_MAX);
2025-11-08T13:22:49.7634102Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7635709Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:366:9:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_scan_data_[m[K' was not declared in this scope; did you mean '[01m[KWidebandScanData[m[K'?
2025-11-08T13:22:49.7637546Z   366 |     if ([01;31m[Kwideband_scan_data_[m[K.slice_counter >= wideband_scan_data_.slices_nb) {
2025-11-08T13:22:49.7638230Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7638898Z       |         [32m[KWidebandScanData[m[K
2025-11-08T13:22:49.7650926Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:370:42:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_scan_data_[m[K' was not declared in this scope; did you mean '[01m[KWidebandScanData[m[K'?
2025-11-08T13:22:49.7652922Z   370 |     const WidebandSlice& current_slice = [01;31m[Kwideband_scan_data_[m[K.slices[wideband_scan_data_.slice_counter];
2025-11-08T13:22:49.7653855Z       |                                          [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7654418Z       |                                          [32m[KWidebandScanData[m[K
2025-11-08T13:22:49.7655914Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:371:9:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.7657687Z   371 |     if ([01;31m[Khardware[m[K.tune_to_frequency(current_slice.center_frequency)) {
2025-11-08T13:22:49.7658346Z       |         [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:49.7659348Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7661206Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.7662547Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.7663082Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7664521Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:372:30:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.7666180Z   372 |         int32_t slice_rssi = [01;31m[Khardware[m[K.get_real_rssi_from_hardware(current_slice.center_frequency);
2025-11-08T13:22:49.7667112Z       |                              [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:49.7668127Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.7669970Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.7671261Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.7671785Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7674462Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:375:46:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[K(Frequency)current_slice.WidebandSlice::center_frequency[m[K' from '[01m[KFrequency[m[K' {aka '[01m[Klong long unsigned int[m[K'} to '[01m[Kint64_t[m[K' {aka '[01m[Klong long int[m[K'} [[01;35m[K-Wnarrowing[m[K]
2025-11-08T13:22:49.7676577Z   375 |                 .frequency_a = [01;35m[Kcurrent_slice.center_frequency[m[K,
2025-11-08T13:22:49.7677352Z       |                                [01;35m[K~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7679951Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:376:46:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[K(Frequency)current_slice.WidebandSlice::center_frequency[m[K' from '[01m[KFrequency[m[K' {aka '[01m[Klong long unsigned int[m[K'} to '[01m[Kint64_t[m[K' {aka '[01m[Klong long int[m[K'} [[01;35m[K-Wnarrowing[m[K]
2025-11-08T13:22:49.7682125Z   376 |                 .frequency_b = [01;35m[Kcurrent_slice.center_frequency[m[K,
2025-11-08T13:22:49.7682774Z       |                                [01;35m[K~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7684444Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:377:25:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[Kuint8_t[m[K' {aka '[01m[Kunsigned char[m[K'} to '[01m[Kfreqman_type[m[K' in initialization
2025-11-08T13:22:49.7686364Z   377 |                 .type = [01;31m[Kstatic_cast<uint8_t>(freqman_type::Single)[m[K,
2025-11-08T13:22:49.7687405Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7687973Z       |                         [01;31m[K|[m[K
2025-11-08T13:22:49.7688527Z       |                         [01;31m[Kuint8_t {aka unsigned char}[m[K
2025-11-08T13:22:49.7690503Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:382:13:[m[K [01;31m[Kerror: [m[Kdesignator order for field '[01m[Kfreqman_entry::description[m[K' does not match declaration order in '[01m[Kfreqman_entry[m[K'
2025-11-08T13:22:49.7692041Z   382 |             [01;31m[K}[m[K;
2025-11-08T13:22:49.7692472Z       |             [01;31m[K^[m[K
2025-11-08T13:22:49.7693868Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:383:13:[m[K [01;31m[Kerror: [m[K'[01m[Kwideband_detection_override[m[K' was not declared in this scope
2025-11-08T13:22:49.7695478Z   383 |             [01;31m[Kwideband_detection_override[m[K(fake_entry, slice_rssi, WIDEBAND_RSSI_THRESHOLD_DB);
2025-11-08T13:22:49.7696256Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7703857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:385:9:[m[K [01;31m[Kerror: [m[K'[01m[Klast_scanned_frequency_[m[K' was not declared in this scope
2025-11-08T13:22:49.7705344Z   385 |         [01;31m[Klast_scanned_frequency_[m[K = current_slice.center_frequency;
2025-11-08T13:22:49.7706010Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7718249Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:387:13:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_cycles_[m[K' was not declared in this scope
2025-11-08T13:22:49.7719560Z   387 |         if ([01;31m[Kscan_cycles_[m[K % 100 == 0) {
2025-11-08T13:22:49.7720072Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7735721Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:388:13:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_scan_error[m[K' was not declared in this scope
2025-11-08T13:22:49.7738231Z   388 |             [01;31m[Khandle_scan_error[m[K("Hardware tuning failed in wideband mode");
2025-11-08T13:22:49.7738963Z       |             [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7740014Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.7741872Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:394:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.7743806Z   394 | void [01;31m[KDroneScanner[m[K::wideband_detection_override(const freqman_entry& entry, int32_t rssi, int32_t threshold_override) {
2025-11-08T13:22:49.7744842Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7758138Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid wideband_detection_override(const freqman_entry&, int32_t, int32_t)[m[K':
2025-11-08T13:22:49.7760770Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:398:9:[m[K [01;31m[Kerror: [m[K'[01m[Kprocess_wideband_detection_with_override[m[K' was not declared in this scope; did you mean '[01m[Kwideband_detection_override[m[K'?
2025-11-08T13:22:49.7762912Z   398 |         [01;31m[Kprocess_wideband_detection_with_override[m[K(wideband_entry, rssi, DEFAULT_RSSI_THRESHOLD_DB, threshold_override);
2025-11-08T13:22:49.7764111Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7764655Z       |         [32m[Kwideband_detection_override[m[K
2025-11-08T13:22:49.7765661Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.7767734Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:402:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.7769443Z   402 | void [01;31m[KDroneScanner[m[K::process_wideband_detection_with_override(const freqman_entry& entry, int32_t rssi,
2025-11-08T13:22:49.7770285Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7782446Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid process_wideband_detection_with_override(const freqman_entry&, int32_t, int32_t, int32_t)[m[K':
2025-11-08T13:22:49.7784986Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:422:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktotal_detections_[m[K' was not declared in this scope
2025-11-08T13:22:49.7786254Z   422 |     [01;31m[Ktotal_detections_[m[K++;
2025-11-08T13:22:49.7786967Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7789061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:427:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-08T13:22:49.7790963Z   427 |     if ([01;31m[Klocal_detection_ring[m[K.get_rssi_value(freq_hash) < wideband_threshold) {
2025-11-08T13:22:49.7791667Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7792398Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-08T13:22:49.7794215Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-08T13:22:49.7795830Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-08T13:22:49.7796560Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7798783Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:432:33:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-08T13:22:49.7800742Z   432 |         uint8_t current_count = [01;31m[Klocal_detection_ring[m[K.get_detection_count(freq_hash);
2025-11-08T13:22:49.7801517Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7802311Z       |                                 [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-08T13:22:49.7804133Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-08T13:22:49.7805745Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-08T13:22:49.7806474Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7812638Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:447:17:[m[K [01;31m[Kerror: [m[K'[01m[Kdetection_logger_[m[K' was not declared in this scope
2025-11-08T13:22:49.7814019Z   447 |             if ([01;31m[Kdetection_logger_[m[K.is_session_active()) {
2025-11-08T13:22:49.7814826Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7831903Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:450:13:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracked_drone[m[K' was not declared in this scope
2025-11-08T13:22:49.7833940Z   450 |             [01;31m[Kupdate_tracked_drone[m[K(detected_type, entry.frequency_a, rssi, threat_level);
2025-11-08T13:22:49.7835020Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7837484Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:453:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-08T13:22:49.7839158Z   453 |         [01;31m[Klocal_detection_ring[m[K.update_detection(freq_hash, 0, -120);
2025-11-08T13:22:49.7839902Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7840701Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-08T13:22:49.7842772Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-08T13:22:49.7844634Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-08T13:22:49.7845472Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7847485Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:403:68:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Koriginal_threshold[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-08T13:22:49.7849511Z   403 |                                                            [01;35m[Kint32_t original_threshold[m[K, int32_t wideband_threshold) {
2025-11-08T13:22:49.7850411Z       |                                                            [01;35m[K~~~~~~~~^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7851566Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.7853076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:457:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.7854381Z   457 | void [01;31m[KDroneScanner[m[K::perform_hybrid_scan_cycle(DroneHardwareController& hardware) {
2025-11-08T13:22:49.7855200Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7856843Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid perform_hybrid_scan_cycle(DroneHardwareController&)[m[K':
2025-11-08T13:22:49.7859113Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:458:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_cycles_[m[K' was not declared in this scope
2025-11-08T13:22:49.7860494Z   458 |     if ([01;31m[Kscan_cycles_[m[K % 2 == 0) {
2025-11-08T13:22:49.7861044Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7862890Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:461:9:[m[K [01;31m[Kerror: [m[K'[01m[Kperform_database_scan_cycle[m[K' was not declared in this scope; did you mean '[01m[Kperform_wideband_scan_cycle[m[K'?
2025-11-08T13:22:49.7864642Z   461 |         [01;31m[Kperform_database_scan_cycle[m[K(hardware);
2025-11-08T13:22:49.7865011Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7865317Z       |         [32m[Kperform_wideband_scan_cycle[m[K
2025-11-08T13:22:49.7865908Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.7867287Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:465:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.7868223Z   465 | void [01;31m[KDroneScanner[m[K::process_rssi_detection(const freqman_entry& entry, int32_t rssi) {
2025-11-08T13:22:49.7868678Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7880251Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid process_rssi_detection(const freqman_entry&, int32_t)[m[K':
2025-11-08T13:22:49.7881534Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:474:10:[m[K [01;31m[Kerror: [m[K'[01m[Kvalidate_detection_simple[m[K' was not declared in this scope
2025-11-08T13:22:49.7882460Z   474 |     if (![01;31m[Kvalidate_detection_simple[m[K(rssi, ThreatLevel::UNKNOWN)) {
2025-11-08T13:22:49.7882866Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7895110Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:479:28:[m[K [01;31m[Kerror: [m[K'[01m[Kdrone_database_[m[K' was not declared in this scope
2025-11-08T13:22:49.7896011Z   479 |     const auto* db_entry = [01;31m[Kdrone_database_[m[K.lookup_frequency(entry.frequency_a);
2025-11-08T13:22:49.7896450Z       |                            [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7912806Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:502:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktotal_detections_[m[K' was not declared in this scope
2025-11-08T13:22:49.7914458Z   502 |     [01;31m[Ktotal_detections_[m[K++;
2025-11-08T13:22:49.7915206Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7917058Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:509:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-08T13:22:49.7918243Z   509 |     if ([01;31m[Klocal_detection_ring[m[K.get_rssi_value(freq_hash) < detection_threshold) {
2025-11-08T13:22:49.7918664Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7919105Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-08T13:22:49.7920145Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-08T13:22:49.7921078Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-08T13:22:49.7921509Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7922673Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:514:33:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-08T13:22:49.7923797Z   514 |         uint8_t current_count = [01;31m[Klocal_detection_ring[m[K.get_detection_count(freq_hash);
2025-11-08T13:22:49.7924252Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7924700Z       |                                 [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-08T13:22:49.7925727Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-08T13:22:49.7926774Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-08T13:22:49.7927402Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7934437Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:531:17:[m[K [01;31m[Kerror: [m[K'[01m[Kdetection_logger_[m[K' was not declared in this scope
2025-11-08T13:22:49.7935424Z   531 |             if ([01;31m[Kdetection_logger_[m[K.is_session_active()) {
2025-11-08T13:22:49.7935792Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7951353Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:535:13:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracked_drone[m[K' was not declared in this scope
2025-11-08T13:22:49.7953090Z   535 |             [01;31m[Kupdate_tracked_drone[m[K(detected_type, entry.frequency_a, rssi, threat_level);
2025-11-08T13:22:49.7953898Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7955335Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:538:9:[m[K [01;31m[Kerror: [m[K'[01m[Klocal_detection_ring[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K'?
2025-11-08T13:22:49.7956411Z   538 |         [01;31m[Klocal_detection_ring[m[K.update_detection(freq_hash, 0, -120);
2025-11-08T13:22:49.7957015Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7957440Z       |         [32m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K
2025-11-08T13:22:49.7958465Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:109:22:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::local_detection_ring[m[K' declared here
2025-11-08T13:22:49.7959383Z   109 | DetectionRingBuffer& [01;36m[Klocal_detection_ring[m[K = global_detection_ring;
2025-11-08T13:22:49.7959816Z       |                      [01;36m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7960406Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.7961392Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:542:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.7962415Z   542 | void [01;31m[KDroneScanner[m[K::update_tracked_drone(DroneType type, Frequency frequency, int32_t rssi, ThreatLevel threat_level) {
2025-11-08T13:22:49.7962972Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7966861Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_tracked_drone(DroneType, Frequency, int32_t, ThreatLevel)[m[K':
2025-11-08T13:22:49.7968308Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:543:24:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-08T13:22:49.7969196Z   543 |     for (auto& drone : [01;31m[Ktracked_drones_[m[K) {
2025-11-08T13:22:49.7969537Z       |                        [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7969838Z       |                        [32m[KTrackedDrone[m[K
2025-11-08T13:22:49.7981982Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:548:13:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-08T13:22:49.7982893Z   548 |             [01;31m[Kupdate_tracking_counts[m[K();
2025-11-08T13:22:49.7983215Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7983508Z       |             [32m[Kupdate_tracked_drone[m[K
2025-11-08T13:22:49.7993516Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:553:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-08T13:22:49.7995349Z   553 |     if ([01;31m[Ktracked_drones_[m[K.size() < MAX_TRACKED_DRONES) {
2025-11-08T13:22:49.7995795Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.7996256Z       |         [32m[KTrackedDrone[m[K
2025-11-08T13:22:49.8012569Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:560:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8013891Z   560 |         [01;31m[Ktracked_drones_count_[m[K++;
2025-11-08T13:22:49.8014410Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8029562Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:561:9:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-08T13:22:49.8030779Z   561 |         [01;31m[Kupdate_tracking_counts[m[K();
2025-11-08T13:22:49.8031174Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8031541Z       |         [32m[Kupdate_tracked_drone[m[K
2025-11-08T13:22:49.8041411Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:566:29:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-08T13:22:49.8042367Z   566 |     systime_t oldest_time = [01;31m[Ktracked_drones_[m[K[0].last_seen;
2025-11-08T13:22:49.8042774Z       |                             [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8043087Z       |                             [32m[KTrackedDrone[m[K
2025-11-08T13:22:49.8057959Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:579:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-08T13:22:49.8058901Z   579 |     [01;31m[Kupdate_tracking_counts[m[K();
2025-11-08T13:22:49.8059210Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8059502Z       |     [32m[Kupdate_tracked_drone[m[K
2025-11-08T13:22:49.8060076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8061094Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:582:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8061889Z   582 | void [01;31m[KDroneScanner[m[K::remove_stale_drones() {
2025-11-08T13:22:49.8062218Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8070621Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid remove_stale_drones()[m[K':
2025-11-08T13:22:49.8072865Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:586:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-08T13:22:49.8074290Z   586 |     [01;31m[Ktracked_drones_[m[K.erase(
2025-11-08T13:22:49.8074778Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8075216Z       |     [32m[KTrackedDrone[m[K
2025-11-08T13:22:49.8091047Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:593:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8092437Z   593 |     [01;31m[Ktracked_drones_count_[m[K = tracked_drones_.size();
2025-11-08T13:22:49.8093287Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8108188Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:594:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_tracking_counts[m[K' was not declared in this scope; did you mean '[01m[Kupdate_tracked_drone[m[K'?
2025-11-08T13:22:49.8109746Z   594 |     [01;31m[Kupdate_tracking_counts[m[K();
2025-11-08T13:22:49.8110463Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8110949Z       |     [32m[Kupdate_tracked_drone[m[K
2025-11-08T13:22:49.8111939Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8113672Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:597:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8115006Z   597 | void [01;31m[KDroneScanner[m[K::update_tracking_counts() {
2025-11-08T13:22:49.8115571Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8128601Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_tracking_counts()[m[K':
2025-11-08T13:22:49.8129857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:598:5:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8130610Z   598 |     [01;31m[Kapproaching_count_[m[K = 0;
2025-11-08T13:22:49.8130906Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8143445Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:599:5:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8144214Z   599 |     [01;31m[Kreceding_count_[m[K = 0;
2025-11-08T13:22:49.8144501Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8157075Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:600:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8157850Z   600 |     [01;31m[Kstatic_count_[m[K = 0;
2025-11-08T13:22:49.8158137Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8168761Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:602:30:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-08T13:22:49.8169889Z   602 |     for (const auto& drone : [01;31m[Ktracked_drones_[m[K) {
2025-11-08T13:22:49.8170321Z       |                              [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8170685Z       |                              [32m[KTrackedDrone[m[K
2025-11-08T13:22:49.8183986Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:614:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_trends_compact_display[m[K' was not declared in this scope
2025-11-08T13:22:49.8185032Z   614 |     [01;31m[Kupdate_trends_compact_display[m[K();
2025-11-08T13:22:49.8185428Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8186133Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8187519Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:617:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8188517Z   617 | void [01;31m[KDroneScanner[m[K::update_trends_compact_display() {
2025-11-08T13:22:49.8188961Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8189860Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:620:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8190992Z   620 | void [01;31m[KDroneScanner[m[K::reset_scan_cycles() {
2025-11-08T13:22:49.8191379Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8197021Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid reset_scan_cycles()[m[K':
2025-11-08T13:22:49.8198604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:621:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_cycles_[m[K' was not declared in this scope
2025-11-08T13:22:49.8199466Z   621 |     [01;31m[Kscan_cycles_[m[K = 0;
2025-11-08T13:22:49.8199757Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8200334Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8201336Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:624:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8202139Z   624 | void [01;31m[KDroneScanner[m[K::switch_to_real_mode() {
2025-11-08T13:22:49.8202462Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8210518Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid switch_to_real_mode()[m[K':
2025-11-08T13:22:49.8211695Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:625:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_real_mode_[m[K' was not declared in this scope
2025-11-08T13:22:49.8212384Z   625 |     [01;31m[Kis_real_mode_[m[K = true;
2025-11-08T13:22:49.8212668Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8213229Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8214243Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:628:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8215022Z   628 | void [01;31m[KDroneScanner[m[K::switch_to_demo_mode() {
2025-11-08T13:22:49.8215349Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8224144Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid switch_to_demo_mode()[m[K':
2025-11-08T13:22:49.8225359Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:629:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_real_mode_[m[K' was not declared in this scope
2025-11-08T13:22:49.8226066Z   629 |     [01;31m[Kis_real_mode_[m[K = false;
2025-11-08T13:22:49.8226353Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8227086Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8228090Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:632:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8228908Z   632 | void [01;31m[KDroneScanner[m[K::initialize_database_and_scanner() {
2025-11-08T13:22:49.8229266Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8240840Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid initialize_database_and_scanner()[m[K':
2025-11-08T13:22:49.8248509Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:639:38:[m[K [01;31m[Kerror: [m[K'[01m[Kdrone_database_[m[K' was not declared in this scope
2025-11-08T13:22:49.8249376Z   639 |     if (!load_freqman_file("DRONES", [01;31m[Kdrone_database_[m[K, options)) {
2025-11-08T13:22:49.8249789Z       |                                      [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8250894Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:642:5:[m[K [01;31m[Kerror: [m[K'[01m[Kfreq_db_[m[K' was not declared in this scope; did you mean '[01m[Kfreqman_db[m[K'?
2025-11-08T13:22:49.8251723Z   642 |     [01;31m[Kfreq_db_[m[K.open(db_path, false);
2025-11-08T13:22:49.8252033Z       |     [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:49.8252399Z       |     [32m[Kfreqman_db[m[K
2025-11-08T13:22:49.8252969Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8253959Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:645:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8254767Z   645 | void [01;31m[KDroneScanner[m[K::cleanup_database_and_scanner() {
2025-11-08T13:22:49.8255123Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8264905Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid cleanup_database_and_scanner()[m[K':
2025-11-08T13:22:49.8266270Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:646:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope
2025-11-08T13:22:49.8267188Z   646 |     if ([01;31m[Kscanning_thread_[m[K) {
2025-11-08T13:22:49.8267495Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8281600Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:647:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.8283011Z   647 |         [01;31m[Kscanning_active_[m[K = false;
2025-11-08T13:22:49.8283443Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8284021Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8285045Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:653:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8285884Z   653 | void [01;31m[KDroneScanner[m[K::scan_init_from_loaded_frequencies() {
2025-11-08T13:22:49.8286250Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8287169Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:657:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8288055Z   657 | bool [01;31m[KDroneScanner[m[K::validate_detection_simple(int32_t rssi_db, ThreatLevel threat) {
2025-11-08T13:22:49.8288477Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8289193Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:661:11:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8290367Z   661 | Frequency [01;31m[KDroneScanner[m[K::get_current_scanning_frequency() const {
2025-11-08T13:22:49.8291123Z       |           [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8292211Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:661:58:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[KFrequency get_current_scanning_frequency()[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.8293184Z   661 | Frequency DroneScanner::get_current_scanning_frequency() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8293618Z       |                                                          [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8294362Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[KFrequency get_current_scanning_frequency()[m[K':
2025-11-08T13:22:49.8295604Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:662:10:[m[K [01;31m[Kerror: [m[K'[01m[Kfreq_db_[m[K' was not declared in this scope; did you mean '[01m[Kfreqman_db[m[K'?
2025-11-08T13:22:49.8296983Z   662 |     if (![01;31m[Kfreq_db_[m[K.empty() && current_db_index_ < freq_db_.entry_count() && freq_db_[current_db_index_].frequency_a > 0) {
2025-11-08T13:22:49.8297479Z       |          [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:49.8297864Z       |          [32m[Kfreqman_db[m[K
2025-11-08T13:22:49.8309837Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:662:30:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_db_index_[m[K' was not declared in this scope
2025-11-08T13:22:49.8311104Z   662 |     if (!freq_db_.empty() && [01;31m[Kcurrent_db_index_[m[K < freq_db_.entry_count() && freq_db_[current_db_index_].frequency_a > 0) {
2025-11-08T13:22:49.8311630Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8312231Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8313226Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:668:11:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8314097Z   668 | Frequency [01;31m[KDroneScanner[m[K::get_current_radio_frequency() const {
2025-11-08T13:22:49.8314482Z       |           [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8315626Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:668:55:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[KFrequency get_current_radio_frequency()[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.8316752Z   668 | Frequency DroneScanner::get_current_radio_frequency() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8317179Z       |                                                       [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8317946Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:672:21:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8318807Z   672 | const TrackedDrone& [01;31m[KDroneScanner[m[K::getTrackedDrone(size_t index) const {
2025-11-08T13:22:49.8319230Z       |                     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8320482Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:672:65:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kconst TrackedDrone& getTrackedDrone(size_t)[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.8322305Z   672 | const TrackedDrone& DroneScanner::getTrackedDrone(size_t index) [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8323150Z       |                                                                 [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8324418Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kconst TrackedDrone& getTrackedDrone(size_t)[m[K':
2025-11-08T13:22:49.8325724Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:673:17:[m[K [01;31m[Kerror: [m[K'[01m[Ktracked_drones_[m[K' was not declared in this scope; did you mean '[01m[KTrackedDrone[m[K'?
2025-11-08T13:22:49.8326586Z   673 |     if (index < [01;31m[Ktracked_drones_[m[K.size()) {
2025-11-08T13:22:49.8327050Z       |                 [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8327332Z       |                 [32m[KTrackedDrone[m[K
2025-11-08T13:22:49.8327901Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8328875Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:680:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8329847Z   680 | std::string [01;31m[KDroneScanner[m[K::get_session_summary() const {
2025-11-08T13:22:49.8330208Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8331183Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:680:49:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_session_summary()[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.8332185Z   680 | std::string DroneScanner::get_session_summary() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8332582Z       |                                                 [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8340047Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string get_session_summary()[m[K':
2025-11-08T13:22:49.8341261Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:12:[m[K [01;31m[Kerror: [m[K'[01m[Kdetection_logger_[m[K' was not declared in this scope
2025-11-08T13:22:49.8342197Z   681 |     return [01;31m[Kdetection_logger_[m[K.format_session_summary(get_scan_cycles(), get_total_detections());
2025-11-08T13:22:49.8342673Z       |            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8348557Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:53:[m[K [01;31m[Kerror: [m[K'[01m[Kget_scan_cycles[m[K' was not declared in this scope; did you mean '[01m[Kreset_scan_cycles[m[K'?
2025-11-08T13:22:49.8349656Z   681 |     return detection_logger_.format_session_summary([01;31m[Kget_scan_cycles[m[K(), get_total_detections());
2025-11-08T13:22:49.8350181Z       |                                                     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8350524Z       |                                                     [32m[Kreset_scan_cycles[m[K
2025-11-08T13:22:49.8364344Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:681:72:[m[K [01;31m[Kerror: [m[K'[01m[Kget_total_detections[m[K' was not declared in this scope
2025-11-08T13:22:49.8365333Z   681 |     return detection_logger_.format_session_summary(get_scan_cycles(), [01;31m[Kget_total_detections[m[K());
2025-11-08T13:22:49.8365867Z       |                                                                        [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8366451Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8367641Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:684:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8368490Z   684 | void [01;31m[KDroneScanner[m[K::handle_scan_error(const char* error_msg) {
2025-11-08T13:22:49.8368868Z       |      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8376070Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:689:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-08T13:22:49.8377129Z   689 | inline [01;31m[KDroneScanner[m[K::DroneDetectionLogger::DroneDetectionLogger()
2025-11-08T13:22:49.8377543Z       |        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8387098Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:694:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-08T13:22:49.8388053Z   694 | inline [01;31m[KDroneScanner[m[K::DroneDetectionLogger::~DroneDetectionLogger() {
2025-11-08T13:22:49.8388465Z       |        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8389220Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:698:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8390262Z   698 | inline void [01;31m[KDroneScanner[m[K::DroneDetectionLogger::start_session() {
2025-11-08T13:22:49.8390667Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8402546Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid start_session()[m[K':
2025-11-08T13:22:49.8403896Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:699:9:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.8404638Z   699 |     if ([01;31m[Ksession_active_[m[K) return;
2025-11-08T13:22:49.8404948Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8417296Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:700:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.8418055Z   700 |     [01;31m[Ksession_active_[m[K = true;
2025-11-08T13:22:49.8418367Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8431254Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:701:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_start_[m[K' was not declared in this scope
2025-11-08T13:22:49.8432018Z   701 |     [01;31m[Ksession_start_[m[K = chTimeNow();
2025-11-08T13:22:49.8432338Z       |     [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8445229Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:702:5:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8445980Z   702 |     [01;31m[Klogged_count_[m[K = 0;
2025-11-08T13:22:49.8446256Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8460583Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:703:5:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-08T13:22:49.8461355Z   703 |     [01;31m[Kheader_written_[m[K = false;
2025-11-08T13:22:49.8461649Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8462221Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8463245Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:706:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8464103Z   706 | inline void [01;31m[KDroneScanner[m[K::DroneDetectionLogger::end_session() {
2025-11-08T13:22:49.8464509Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8475524Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid end_session()[m[K':
2025-11-08T13:22:49.8476832Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:707:10:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.8477587Z   707 |     if (![01;31m[Ksession_active_[m[K) return;
2025-11-08T13:22:49.8477892Z       |          [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8490891Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:708:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.8491655Z   708 |     [01;31m[Ksession_active_[m[K = false;
2025-11-08T13:22:49.8491954Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8492522Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8493528Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:711:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8494678Z   711 | inline bool [01;31m[KDroneScanner[m[K::DroneDetectionLogger::log_detection(const DetectionLogEntry& entry) {
2025-11-08T13:22:49.8495161Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8505999Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool log_detection(const DetectionLogEntry&)[m[K':
2025-11-08T13:22:49.8507547Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:712:10:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.8508304Z   712 |     if (![01;31m[Ksession_active_[m[K) return false;
2025-11-08T13:22:49.8508635Z       |          [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8522757Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:713:10:[m[K [01;31m[Kerror: [m[K'[01m[Kensure_csv_header[m[K' was not declared in this scope
2025-11-08T13:22:49.8523588Z   713 |     if (![01;31m[Kensure_csv_header[m[K()) return false;
2025-11-08T13:22:49.8523921Z       |          [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8538238Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:715:29:[m[K [01;31m[Kerror: [m[K'[01m[Kformat_csv_entry[m[K' was not declared in this scope
2025-11-08T13:22:49.8539109Z   715 |     std::string csv_entry = [01;31m[Kformat_csv_entry[m[K(entry);
2025-11-08T13:22:49.8539490Z       |                             [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8546407Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:18:[m[K [01;31m[Kerror: [m[K'[01m[Kcsv_log_[m[K' was not declared in this scope
2025-11-08T13:22:49.8547419Z   716 |     auto error = [01;31m[Kcsv_log_[m[K.append(generate_log_filename().string());
2025-11-08T13:22:49.8547822Z       |                  [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:49.8562528Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:716:34:[m[K [01;31m[Kerror: [m[K'[01m[Kgenerate_log_filename[m[K' was not declared in this scope
2025-11-08T13:22:49.8563419Z   716 |     auto error = csv_log_.append([01;31m[Kgenerate_log_filename[m[K().string());
2025-11-08T13:22:49.8563852Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8576870Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:720:9:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8577656Z   720 |         [01;31m[Klogged_count_[m[K++;
2025-11-08T13:22:49.8577942Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8578510Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8579545Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:726:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8580436Z   726 | inline bool [01;31m[KDroneScanner[m[K::DroneDetectionLogger::ensure_csv_header() {
2025-11-08T13:22:49.8580850Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8592460Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool ensure_csv_header()[m[K':
2025-11-08T13:22:49.8593639Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:727:9:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-08T13:22:49.8594370Z   727 |     if ([01;31m[Kheader_written_[m[K) return true;
2025-11-08T13:22:49.8594680Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8600850Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:729:18:[m[K [01;31m[Kerror: [m[K'[01m[Kcsv_log_[m[K' was not declared in this scope
2025-11-08T13:22:49.8601661Z   729 |     auto error = [01;31m[Kcsv_log_[m[K.append(generate_log_filename());
2025-11-08T13:22:49.8602028Z       |                  [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:49.8617760Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:729:34:[m[K [01;31m[Kerror: [m[K'[01m[Kgenerate_log_filename[m[K' was not declared in this scope
2025-11-08T13:22:49.8618640Z   729 |     auto error = csv_log_.append([01;31m[Kgenerate_log_filename[m[K());
2025-11-08T13:22:49.8619061Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8633409Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:733:9:[m[K [01;31m[Kerror: [m[K'[01m[Kheader_written_[m[K' was not declared in this scope
2025-11-08T13:22:49.8634952Z   733 |         [01;31m[Kheader_written_[m[K = true;
2025-11-08T13:22:49.8635545Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8636857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8638271Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:739:20:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8639484Z   739 | inline std::string [01;31m[KDroneScanner[m[K::DroneDetectionLogger::format_csv_entry(const DetectionLogEntry& entry) {
2025-11-08T13:22:49.8640121Z       |                    [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8641112Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string format_csv_entry(const DetectionLogEntry&)[m[K':
2025-11-08T13:22:49.8642794Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:16:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:49.8644109Z   743 |              "[01;35m[K%u[m[K,%u,%d,%u,%u,%u,%.2f\n",
2025-11-08T13:22:49.8644692Z       |               [01;35m[K~^[m[K
2025-11-08T13:22:49.8645177Z       |                [01;35m[K|[m[K
2025-11-08T13:22:49.8645660Z       |                [01;35m[Kunsigned int[m[K
2025-11-08T13:22:49.8646079Z       |               [32m[K%lu[m[K
2025-11-08T13:22:49.8646467Z   744 |              [32m[Kentry.timestamp[m[K, entry.frequency_hz, entry.rssi_db,
2025-11-08T13:22:49.8647039Z       |              [32m[K~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8647320Z       |                    [32m[K|[m[K
2025-11-08T13:22:49.8647651Z       |                    [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-08T13:22:49.8648928Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:19:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:49.8650026Z   743 |              "%u,[01;35m[K%u[m[K,%d,%u,%u,%u,%.2f\n",
2025-11-08T13:22:49.8650332Z       |                  [01;35m[K~^[m[K
2025-11-08T13:22:49.8650603Z       |                   [01;35m[K|[m[K
2025-11-08T13:22:49.8650885Z       |                   [01;35m[Kunsigned int[m[K
2025-11-08T13:22:49.8651159Z       |                  [32m[K%lu[m[K
2025-11-08T13:22:49.8651535Z   744 |              entry.timestamp, [32m[Kentry.frequency_hz[m[K, entry.rssi_db,
2025-11-08T13:22:49.8651916Z       |                               [32m[K~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8652415Z       |                                     [32m[K|[m[K
2025-11-08T13:22:49.8652995Z       |                                     [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-08T13:22:49.8655097Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:743:22:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%d[m[K' expects argument of type '[01m[Kint[m[K', but argument 6 has type '[01m[Kint32_t[m[K' {aka '[01m[Klong int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:49.8656121Z   743 |              "%u,%u,[01;35m[K%d[m[K,%u,%u,%u,%.2f\n",
2025-11-08T13:22:49.8656430Z       |                     [01;35m[K~^[m[K
2025-11-08T13:22:49.8656836Z       |                      [01;35m[K|[m[K
2025-11-08T13:22:49.8657108Z       |                      [01;35m[Kint[m[K
2025-11-08T13:22:49.8657377Z       |                     [32m[K%ld[m[K
2025-11-08T13:22:49.8657760Z   744 |              entry.timestamp, entry.frequency_hz, [32m[Kentry.rssi_db[m[K,
2025-11-08T13:22:49.8658169Z       |                                                   [32m[K~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8658485Z       |                                                         [32m[K|[m[K
2025-11-08T13:22:49.8658828Z       |                                                         [32m[Kint32_t {aka long int}[m[K
2025-11-08T13:22:49.8659439Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8660452Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:751:20:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8661337Z   751 | inline std::string [01;31m[KDroneScanner[m[K::DroneDetectionLogger::generate_log_filename() const {
2025-11-08T13:22:49.8661792Z       |                    [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8662776Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:751:80:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string generate_log_filename()[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.8663788Z   751 | inline std::string DroneScanner::DroneDetectionLogger::generate_log_filename() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8664318Z       |                                                                                [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8665113Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:755:20:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' has not been declared
2025-11-08T13:22:49.8666150Z   755 | inline std::string [01;31m[KDroneScanner[m[K::DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) const {
2025-11-08T13:22:49.8667019Z       |                    [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8668994Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:755:124:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string format_session_summary(size_t, size_t)[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.8670350Z   755 | inline std::string DroneScanner::DroneDetectionLogger::format_session_summary(size_t scan_cycles, size_t total_detections) [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8671055Z       |                                                                                                                            [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8671907Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string format_session_summary(size_t, size_t)[m[K':
2025-11-08T13:22:49.8673077Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:756:50:[m[K [01;31m[Kerror: [m[K'[01m[Ksession_start_[m[K' was not declared in this scope
2025-11-08T13:22:49.8674082Z   756 |     uint32_t session_duration_ms = chTimeNow() - [01;31m[Ksession_start_[m[K;
2025-11-08T13:22:49.8674509Z       |                                                  [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8684989Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:766:58:[m[K [01;31m[Kerror: [m[K'[01m[Klogged_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8685890Z   766 |         avg_detections_per_cycle, detections_per_second, [01;31m[Klogged_count_[m[K);
2025-11-08T13:22:49.8686344Z       |                                                          [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8704201Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8706300Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:779:67:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8708188Z   779 | DroneHardwareController::DroneHardwareController(SpectrumMode mode[01;31m[K)[m[K
2025-11-08T13:22:49.8709118Z       |                                                                   [01;31m[K^[m[K
2025-11-08T13:22:49.8710210Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8711869Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8712792Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8713119Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8713972Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:785:51:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8714895Z   785 | DroneHardwareController::~DroneHardwareController([01;31m[K)[m[K {
2025-11-08T13:22:49.8715313Z       |                                                   [01;31m[K^[m[K
2025-11-08T13:22:49.8715910Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8717148Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8717910Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8718224Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8719047Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:789:51:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8719932Z   789 | void DroneHardwareController::initialize_hardware([01;31m[K)[m[K {
2025-11-08T13:22:49.8720349Z       |                                                   [01;31m[K^[m[K
2025-11-08T13:22:49.8720931Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8721994Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8722737Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8723051Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8723932Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:794:48:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8725811Z   794 | void DroneHardwareController::on_hardware_show([01;31m[K)[m[K {
2025-11-08T13:22:49.8726531Z       |                                                [01;31m[K^[m[K
2025-11-08T13:22:49.8727720Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8728970Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8729728Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8730044Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8730857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:798:48:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8731714Z   798 | void DroneHardwareController::on_hardware_hide([01;31m[K)[m[K {
2025-11-08T13:22:49.8732102Z       |                                                [01;31m[K^[m[K
2025-11-08T13:22:49.8732680Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8733742Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8734481Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8734791Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8735614Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:802:49:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8736510Z   802 | void DroneHardwareController::shutdown_hardware([01;31m[K)[m[K {
2025-11-08T13:22:49.8737461Z       |                                                 [01;31m[K^[m[K
2025-11-08T13:22:49.8738599Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8740160Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8740914Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8741219Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8742038Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:807:54:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8743412Z   807 | void DroneHardwareController::initialize_radio_state([01;31m[K)[m[K {
2025-11-08T13:22:49.8744231Z       |                                                      [01;31m[K^[m[K
2025-11-08T13:22:49.8745236Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8746318Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8747237Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8747555Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8748922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:816:61:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8750592Z   816 | void DroneHardwareController::initialize_spectrum_collector([01;31m[K)[m[K {
2025-11-08T13:22:49.8751241Z       |                                                             [01;31m[K^[m[K
2025-11-08T13:22:49.8751842Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8753013Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8753764Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8754075Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8754892Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:826:58:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8755784Z   826 | void DroneHardwareController::cleanup_spectrum_collector([01;31m[K)[m[K {
2025-11-08T13:22:49.8756225Z       |                                                          [01;31m[K^[m[K
2025-11-08T13:22:49.8756971Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8758045Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8758785Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8759100Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8759919Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:830:66:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8760818Z   830 | void DroneHardwareController::set_spectrum_mode(SpectrumMode mode[01;31m[K)[m[K {
2025-11-08T13:22:49.8761287Z       |                                                                  [01;31m[K^[m[K
2025-11-08T13:22:49.8761868Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8762912Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8763652Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8764129Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8765536Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:834:60:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8767108Z   834 | uint32_t DroneHardwareController::get_spectrum_bandwidth() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8767565Z       |                                                            [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8768151Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8769214Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8770332Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8770732Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8772225Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:838:75:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8773304Z   838 | void DroneHardwareController::set_spectrum_bandwidth(uint32_t bandwidth_hz[01;31m[K)[m[K {
2025-11-08T13:22:49.8773973Z       |                                                                           [01;31m[K^[m[K
2025-11-08T13:22:49.8774568Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8775718Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8776467Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8776916Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8777749Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:842:68:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8778702Z   842 | Frequency DroneHardwareController::get_spectrum_center_frequency() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8779195Z       |                                                                    [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8779784Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8780836Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8781580Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8781882Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8782703Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:846:82:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8783670Z   846 | void DroneHardwareController::set_spectrum_center_frequency(Frequency center_freq[01;31m[K)[m[K {
2025-11-08T13:22:49.8784563Z       |                                                                                  [01;31m[K^[m[K
2025-11-08T13:22:49.8785752Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8787483Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8788252Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8788567Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8789396Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:850:71:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8790342Z   850 | bool DroneHardwareController::tune_to_frequency(Frequency frequency_hz[01;31m[K)[m[K {
2025-11-08T13:22:49.8790817Z       |                                                                       [01;31m[K^[m[K
2025-11-08T13:22:49.8791407Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8792467Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8793208Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8793511Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8794338Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:855:56:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8795397Z   855 | void DroneHardwareController::start_spectrum_streaming([01;31m[K)[m[K {
2025-11-08T13:22:49.8795821Z       |                                                        [01;31m[K^[m[K
2025-11-08T13:22:49.8796402Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8797680Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8798436Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8798737Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8799554Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:861:55:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8800428Z   861 | void DroneHardwareController::stop_spectrum_streaming([01;31m[K)[m[K {
2025-11-08T13:22:49.8800851Z       |                                                       [01;31m[K^[m[K
2025-11-08T13:22:49.8801699Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8803723Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8804517Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8804830Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8805659Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:866:88:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8806789Z   866 | int32_t DroneHardwareController::get_real_rssi_from_hardware(Frequency target_frequency[01;31m[K)[m[K {
2025-11-08T13:22:49.8807336Z       |                                                                                        [01;31m[K^[m[K
2025-11-08T13:22:49.8807939Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8808989Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8809729Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8810038Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8810865Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:871:111:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8811976Z   871 | void DroneHardwareController::handle_channel_spectrum_config(const ChannelSpectrumConfigMessage* const message[01;31m[K)[m[K {
2025-11-08T13:22:49.8812634Z       |                                                                                                               [01;31m[K^[m[K
2025-11-08T13:22:49.8813237Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8814284Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8815017Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8815325Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8816143Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:875:92:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8817455Z   875 | void DroneHardwareController::process_channel_spectrum_data(const ChannelSpectrum& spectrum[01;31m[K)[m[K {
2025-11-08T13:22:49.8818038Z       |                                                                                            [01;31m[K^[m[K
2025-11-08T13:22:49.8819137Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8821082Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8821853Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8822166Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8823001Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:879:65:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8823943Z   879 | int32_t DroneHardwareController::get_configured_sampling_rate() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8824414Z       |                                                                 [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8825014Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8826095Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8826994Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8827309Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8828141Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:883:61:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8829055Z   883 | int32_t DroneHardwareController::get_configured_bandwidth() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8829515Z       |                                                             [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8830102Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8831156Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8831891Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8832196Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8833028Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:887:62:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8833938Z   887 | bool DroneHardwareController::is_spectrum_streaming_active() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8834397Z       |                                                              [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8834975Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8836022Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8837169Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8837589Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8839079Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:891:53:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8840100Z   891 | int32_t DroneHardwareController::get_current_rssi() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.8840532Z       |                                                     [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.8841262Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8842313Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8843052Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8843354Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8844183Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:895:59:[m[K [01;31m[Kerror: [m[Kinvalid use of incomplete type '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8845057Z   895 | void DroneHardwareController::update_spectrum_for_scanner([01;31m[K)[m[K {
2025-11-08T13:22:49.8845485Z       |                                                           [01;31m[K^[m[K
2025-11-08T13:22:49.8846072Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.8847298Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:49:7:[m[K [01;36m[Knote: [m[Kforward declaration of '[01m[Kclass DroneHardwareController[m[K'
2025-11-08T13:22:49.8848038Z    49 | class [01;36m[KDroneHardwareController[m[K;
2025-11-08T13:22:49.8848346Z       |       [01;36m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8849075Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:904:1:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' does not name a type
2025-11-08T13:22:49.8849908Z   904 | [01;31m[KSmartThreatHeader[m[K::SmartThreatHeader(Rect parent_rect)
2025-11-08T13:22:49.8850276Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8851026Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:906:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K)[m[K' token
2025-11-08T13:22:49.8851796Z   906 |       threat_progress_bar_({0, 0, screen_width, 16}[01;31m[K)[m[K,
2025-11-08T13:22:49.8852162Z       |                                                    [01;31m[K^[m[K
2025-11-08T13:22:49.8852928Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:907:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.8853776Z   907 |       threat_status_main_({0, 20, screen_width, 16}[01;31m[K,[m[K "THREAT: LOW | ▲0 ■0 ▼0"),
2025-11-08T13:22:49.8854200Z       |                                                    [01;31m[K^[m[K
2025-11-08T13:22:49.8855242Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:907:54:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.8856738Z   907 |       threat_status_main_({0, 20, screen_width, 16}, [01;31m[K"THREAT: LOW | ▲0 ■0 ▼0"[m[K),
2025-11-08T13:22:49.8857560Z       |                                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8858411Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:908:50:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.8859254Z   908 |       threat_frequency_({0, 38, screen_width, 16}[01;31m[K,[m[K "2400.0MHz SCANNING") {
2025-11-08T13:22:49.8859851Z       |                                                  [01;31m[K^[m[K
2025-11-08T13:22:49.8860610Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:908:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.8861413Z   908 |       threat_frequency_({0, 38, screen_width, 16}, [01;31m[K"2400.0MHz SCANNING"[m[K) {
2025-11-08T13:22:49.8861958Z       |                                                    [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8862731Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:913:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.8863699Z   913 | void [01;31m[KSmartThreatHeader[m[K::update(ThreatLevel max_threat, size_t approaching, size_t static_count,
2025-11-08T13:22:49.8864185Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8865014Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update(ThreatLevel, size_t, size_t, size_t, Frequency, bool)[m[K':
2025-11-08T13:22:49.8866363Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:915:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope; did you mean '[01m[Kcurrent_freq[m[K'?
2025-11-08T13:22:49.8867339Z   915 |     [01;31m[Kcurrent_threat_[m[K = max_threat;
2025-11-08T13:22:49.8867638Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8867905Z       |     [32m[Kcurrent_freq[m[K
2025-11-08T13:22:49.8868761Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:916:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope; did you mean '[01m[Kis_scanning[m[K'?
2025-11-08T13:22:49.8869559Z   916 |     [01;31m[Kis_scanning_[m[K = is_scanning;
2025-11-08T13:22:49.8869853Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8870108Z       |     [32m[Kis_scanning[m[K
2025-11-08T13:22:49.8871218Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:917:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope; did you mean '[01m[Kcurrent_freq[m[K'?
2025-11-08T13:22:49.8872746Z   917 |     [01;31m[Kcurrent_freq_[m[K = current_freq;
2025-11-08T13:22:49.8873254Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8873522Z       |     [32m[Kcurrent_freq[m[K
2025-11-08T13:22:49.8874293Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:918:5:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8875041Z   918 |     [01;31m[Kapproaching_count_[m[K = approaching;
2025-11-08T13:22:49.8875362Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8885678Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:919:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope; did you mean '[01m[Kstatic_count[m[K'?
2025-11-08T13:22:49.8886860Z   919 |     [01;31m[Kstatic_count_[m[K = static_count;
2025-11-08T13:22:49.8887464Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8887995Z       |     [32m[Kstatic_count[m[K
2025-11-08T13:22:49.8889427Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:920:5:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8890345Z   920 |     [01;31m[Kreceding_count_[m[K = receding;
2025-11-08T13:22:49.8890650Z       |     [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8891466Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:923:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_progress_bar_[m[K' was not declared in this scope
2025-11-08T13:22:49.8892454Z   923 |     [01;31m[Kthreat_progress_bar_[m[K.set_value(total_drones * 10);
2025-11-08T13:22:49.8892812Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8893710Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:926:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_icon_text[m[K' was not declared in this scope
2025-11-08T13:22:49.8894579Z   926 |     std::string threat_name = [01;31m[Kget_threat_icon_text[m[K(max_threat);
2025-11-08T13:22:49.8894990Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8895790Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:936:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_status_main_[m[K' was not declared in this scope
2025-11-08T13:22:49.8896521Z   936 |     [01;31m[Kthreat_status_main_[m[K.set(buffer);
2025-11-08T13:22:49.8897014Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8897786Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:955:9:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_frequency_[m[K' was not declared in this scope
2025-11-08T13:22:49.8898515Z   955 |         [01;31m[Kthreat_frequency_[m[K.set(buffer);
2025-11-08T13:22:49.8898826Z       |         [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8899605Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:957:9:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_frequency_[m[K' was not declared in this scope
2025-11-08T13:22:49.8900346Z   957 |         [01;31m[Kthreat_frequency_[m[K.set("NO SIGNAL");
2025-11-08T13:22:49.8900657Z       |         [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8901432Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:959:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_frequency_[m[K' was not declared in this scope
2025-11-08T13:22:49.8902261Z   959 |     [01;31m[Kthreat_frequency_[m[K.set_style(get_threat_text_color(max_threat));
2025-11-08T13:22:49.8902645Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8906777Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:959:33:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_text_color[m[K' was not declared in this scope
2025-11-08T13:22:49.8908327Z   959 |     threat_frequency_.set_style([01;31m[Kget_threat_text_color[m[K(max_threat));
2025-11-08T13:22:49.8909071Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8917144Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:960:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-08T13:22:49.8918390Z   960 |     [01;31m[Kset_dirty[m[K();
2025-11-08T13:22:49.8918841Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:49.8919814Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.8921582Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:963:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.8923093Z   963 | void [01;31m[KSmartThreatHeader[m[K::set_max_threat(ThreatLevel threat) {
2025-11-08T13:22:49.8923745Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8936329Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_max_threat(ThreatLevel)[m[K':
2025-11-08T13:22:49.8938574Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:964:19:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-08T13:22:49.8940101Z   964 |     if (threat != [01;31m[Kcurrent_threat_[m[K) {
2025-11-08T13:22:49.8940643Z       |                   [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8955570Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:965:24:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8957158Z   965 |         update(threat, [01;31m[Kapproaching_count_[m[K, static_count_, receding_count_,
2025-11-08T13:22:49.8957734Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8969019Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:965:44:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8969913Z   965 |         update(threat, approaching_count_, [01;31m[Kstatic_count_[m[K, receding_count_,
2025-11-08T13:22:49.8970382Z       |                                            [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8984032Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:965:59:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.8984962Z   965 |         update(threat, approaching_count_, static_count_, [01;31m[Kreceding_count_[m[K,
2025-11-08T13:22:49.8985429Z       |                                                           [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.8997309Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:966:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-08T13:22:49.8998095Z   966 |                [01;31m[Kcurrent_freq_[m[K, is_scanning_);
2025-11-08T13:22:49.8998419Z       |                [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9009662Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:966:31:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope
2025-11-08T13:22:49.9010431Z   966 |                current_freq_, [01;31m[Kis_scanning_[m[K);
2025-11-08T13:22:49.9010765Z       |                               [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9011353Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9012372Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:970:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9013385Z   970 | void [01;31m[KSmartThreatHeader[m[K::set_movement_counts(size_t approaching, size_t static_count, size_t receding) {
2025-11-08T13:22:49.9013888Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9024998Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_movement_counts(size_t, size_t, size_t)[m[K':
2025-11-08T13:22:49.9027316Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:971:12:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9028784Z   971 |     update([01;31m[Kcurrent_threat_[m[K, approaching, static_count, receding,
2025-11-08T13:22:49.9029443Z       |            [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9041684Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:972:12:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-08T13:22:49.9042983Z   972 |            [01;31m[Kcurrent_freq_[m[K, is_scanning_);
2025-11-08T13:22:49.9043506Z       |            [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9055949Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:972:27:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope
2025-11-08T13:22:49.9058405Z   972 |            current_freq_, [01;31m[Kis_scanning_[m[K);
2025-11-08T13:22:49.9058982Z       |                           [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9060200Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9061983Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:975:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9063471Z   975 | void [01;31m[KSmartThreatHeader[m[K::set_current_frequency(Frequency freq) {
2025-11-08T13:22:49.9064136Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9074707Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_current_frequency(Frequency)[m[K':
2025-11-08T13:22:49.9075942Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:976:17:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-08T13:22:49.9076852Z   976 |     if (freq != [01;31m[Kcurrent_freq_[m[K) {
2025-11-08T13:22:49.9077187Z       |                 [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9089194Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9090736Z   977 |         update([01;31m[Kcurrent_threat_[m[K, approaching_count_, static_count_, receding_count_,
2025-11-08T13:22:49.9091461Z       |                [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9107372Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:33:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.9108985Z   977 |         update(current_threat_, [01;31m[Kapproaching_count_[m[K, static_count_, receding_count_,
2025-11-08T13:22:49.9109781Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9123088Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:53:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.9124279Z   977 |         update(current_threat_, approaching_count_, [01;31m[Kstatic_count_[m[K, receding_count_,
2025-11-08T13:22:49.9124884Z       |                                                     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9137623Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:977:68:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.9138679Z   977 |         update(current_threat_, approaching_count_, static_count_, [01;31m[Kreceding_count_[m[K,
2025-11-08T13:22:49.9139229Z       |                                                                    [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9150061Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:978:22:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope
2025-11-08T13:22:49.9150936Z   978 |                freq, [01;31m[Kis_scanning_[m[K);
2025-11-08T13:22:49.9151287Z       |                      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9151922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9153023Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:982:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9154773Z   982 | void [01;31m[KSmartThreatHeader[m[K::set_scanning_state(bool is_scanning) {
2025-11-08T13:22:49.9155449Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9156265Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_scanning_state(bool)[m[K':
2025-11-08T13:22:49.9157967Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:983:24:[m[K [01;31m[Kerror: [m[K'[01m[Kis_scanning_[m[K' was not declared in this scope; did you mean '[01m[Kis_scanning[m[K'?
2025-11-08T13:22:49.9158881Z   983 |     if (is_scanning != [01;31m[Kis_scanning_[m[K) {
2025-11-08T13:22:49.9159214Z       |                        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9159523Z       |                        [32m[Kis_scanning[m[K
2025-11-08T13:22:49.9170486Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9171407Z   984 |         update([01;31m[Kcurrent_threat_[m[K, approaching_count_, static_count_, receding_count_,
2025-11-08T13:22:49.9171840Z       |                [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9186335Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:33:[m[K [01;31m[Kerror: [m[K'[01m[Kapproaching_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.9187437Z   984 |         update(current_threat_, [01;31m[Kapproaching_count_[m[K, static_count_, receding_count_,
2025-11-08T13:22:49.9187907Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9200002Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:53:[m[K [01;31m[Kerror: [m[K'[01m[Kstatic_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.9200948Z   984 |         update(current_threat_, approaching_count_, [01;31m[Kstatic_count_[m[K, receding_count_,
2025-11-08T13:22:49.9201432Z       |                                                     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9215071Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:984:68:[m[K [01;31m[Kerror: [m[K'[01m[Kreceding_count_[m[K' was not declared in this scope
2025-11-08T13:22:49.9216007Z   984 |         update(current_threat_, approaching_count_, static_count_, [01;31m[Kreceding_count_[m[K,
2025-11-08T13:22:49.9216497Z       |                                                                    [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9229544Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:985:16:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_freq_[m[K' was not declared in this scope
2025-11-08T13:22:49.9230790Z   985 |                [01;31m[Kcurrent_freq_[m[K, is_scanning);
2025-11-08T13:22:49.9231387Z       |                [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9232385Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9234168Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:989:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9235619Z   989 | void [01;31m[KSmartThreatHeader[m[K::set_color_scheme(bool use_dark_theme) {
2025-11-08T13:22:49.9236092Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9237020Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:993:7:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9238981Z   993 | Color [01;31m[KSmartThreatHeader[m[K::get_threat_bar_color(ThreatLevel level) const {
2025-11-08T13:22:49.9239796Z       |       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9240999Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:993:66:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_threat_bar_color(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.9242225Z   993 | Color SmartThreatHeader::get_threat_bar_color(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-08T13:22:49.9242725Z       |                                                                  [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9243528Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1004:7:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9244416Z  1004 | Color [01;31m[KSmartThreatHeader[m[K::get_threat_text_color(ThreatLevel level) const {
2025-11-08T13:22:49.9244848Z       |       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9245840Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1004:67:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_threat_text_color(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.9247063Z  1004 | Color SmartThreatHeader::get_threat_text_color(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-08T13:22:49.9247546Z       |                                                                   [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9248335Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1015:13:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9249202Z  1015 | std::string [01;31m[KSmartThreatHeader[m[K::get_threat_icon_text(ThreatLevel level) const {
2025-11-08T13:22:49.9249630Z       |             [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9250634Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1015:72:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_threat_icon_text(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.9251647Z  1015 | std::string SmartThreatHeader::get_threat_icon_text(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-08T13:22:49.9252135Z       |                                                                        [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9253549Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' has not been declared
2025-11-08T13:22:49.9255084Z  1026 | void [01;31m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-08T13:22:49.9255775Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9257210Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-08T13:22:49.9258603Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1027:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-08T13:22:49.9259446Z  1027 |     View::paint(painter[01;31m[K)[m[K;
2025-11-08T13:22:49.9259755Z       |                        [01;31m[K^[m[K
2025-11-08T13:22:49.9260541Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1028:9:[m[K [01;31m[Kerror: [m[K'[01m[Kcurrent_threat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9261332Z  1028 |     if ([01;31m[Kcurrent_threat_[m[K >= ThreatLevel::HIGH) {
2025-11-08T13:22:49.9261656Z       |         [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9262577Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1033:41:[m[K [01;31m[Kerror: [m[Kinvalid use of non-static member function '[01m[Kuint8_t ui::Color::r()[m[K'
2025-11-08T13:22:49.9263683Z  1033 |         pulse_color = Color([01;31m[Kpulse_color.r[m[K, pulse_color.g, pulse_color.b, alpha);
2025-11-08T13:22:49.9263824Z       |                             [01;31m[K~~~~~~~~~~~~^[m[K
2025-11-08T13:22:49.9264335Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/../../gradient.hpp:25[m[K,
2025-11-08T13:22:49.9264723Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:22[m[K,
2025-11-08T13:22:49.9265092Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.9265361Z [01m[K/havoc/firmware/common/ui.hpp:119:13:[m[K [01;36m[Knote: [m[Kdeclared here
2025-11-08T13:22:49.9265490Z   119 |     uint8_t [01;36m[Kr[m[K() {
2025-11-08T13:22:49.9265602Z       |             [01;36m[K^[m[K
2025-11-08T13:22:49.9268952Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1034:36:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect[m[K' was not declared in this scope
2025-11-08T13:22:49.9269490Z  1034 |         painter.fill_rectangle({0, [01;31m[Kparent_rect[m[K().top(), parent_rect().width(), 4}, pulse_color);
2025-11-08T13:22:49.9269657Z       |                                    [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9270522Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1034:95:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-08T13:22:49.9270880Z  1034 |         painter.fill_rectangle({0, parent_rect().top(), parent_rect().width(), 4}, pulse_color[01;31m[K)[m[K;
2025-11-08T13:22:49.9271066Z       |                                                                                               [01;31m[K^[m[K
2025-11-08T13:22:49.9271318Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-08T13:22:49.9271558Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-08T13:22:49.9271936Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-08T13:22:49.9272311Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-08T13:22:49.9272688Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.9273271Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-08T13:22:49.9273448Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-08T13:22:49.9273584Z       |                         [01;36m[K~~~~~^[m[K
2025-11-08T13:22:49.9277927Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9278567Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1038:1:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' does not name a type
2025-11-08T13:22:49.9278865Z  1038 | [01;31m[KThreatCard[m[K::ThreatCard(size_t card_index, Rect parent_rect)
2025-11-08T13:22:49.9278970Z       | [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9279560Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1043:6:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-08T13:22:49.9279841Z  1043 | void [01;31m[KThreatCard[m[K::update_card(const DisplayDroneEntry& drone) {
2025-11-08T13:22:49.9279953Z       |      [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9289129Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_card(const DisplayDroneEntry&)[m[K':
2025-11-08T13:22:49.9289779Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1044:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.9290112Z  1044 |     [01;31m[Kis_active_[m[K = true;
2025-11-08T13:22:49.9290237Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9295679Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1045:5:[m[K [01;31m[Kerror: [m[K'[01m[Kfrequency_[m[K' was not declared in this scope; did you mean '[01m[KFrequency[m[K'?
2025-11-08T13:22:49.9295853Z  1045 |     [01;31m[Kfrequency_[m[K = drone.frequency;
2025-11-08T13:22:49.9295962Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9296085Z       |     [32m[KFrequency[m[K
2025-11-08T13:22:49.9303114Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1046:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9303393Z  1046 |     [01;31m[Kthreat_[m[K = drone.threat;
2025-11-08T13:22:49.9303583Z       |     [01;31m[K^~~~~~~[m[K
2025-11-08T13:22:49.9309323Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1047:5:[m[K [01;31m[Kerror: [m[K'[01m[Krssi_[m[K' was not declared in this scope
2025-11-08T13:22:49.9309569Z  1047 |     [01;31m[Krssi_[m[K = drone.rssi;
2025-11-08T13:22:49.9309750Z       |     [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9320817Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1048:5:[m[K [01;31m[Kerror: [m[K'[01m[Klast_seen_[m[K' was not declared in this scope
2025-11-08T13:22:49.9321111Z  1048 |     [01;31m[Klast_seen_[m[K = drone.last_seen;
2025-11-08T13:22:49.9321302Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9333311Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1049:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_name_[m[K' was not declared in this scope
2025-11-08T13:22:49.9333608Z  1049 |     [01;31m[Kthreat_name_[m[K = drone.type_name;
2025-11-08T13:22:49.9333817Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9340207Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1050:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktrend_[m[K' was not declared in this scope
2025-11-08T13:22:49.9340563Z  1050 |     [01;31m[Ktrend_[m[K = MovementTrend::STATIC;
2025-11-08T13:22:49.9340747Z       |     [01;31m[K^~~~~~[m[K
2025-11-08T13:22:49.9351121Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1052:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcard_text_[m[K' was not declared in this scope
2025-11-08T13:22:49.9351356Z  1052 |     [01;31m[Kcard_text_[m[K.set(render_compact());
2025-11-08T13:22:49.9351474Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9364957Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1052:20:[m[K [01;31m[Kerror: [m[K'[01m[Krender_compact[m[K' was not declared in this scope
2025-11-08T13:22:49.9365155Z  1052 |     card_text_.set([01;31m[Krender_compact[m[K());
2025-11-08T13:22:49.9365291Z       |                    [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9377754Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1053:26:[m[K [01;31m[Kerror: [m[K'[01m[Kget_card_text_color[m[K' was not declared in this scope; did you mean '[01m[Kget_threat_text_color[m[K'?
2025-11-08T13:22:49.9377983Z  1053 |     card_text_.set_style([01;31m[Kget_card_text_color[m[K());
2025-11-08T13:22:49.9378328Z       |                          [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9378476Z       |                          [32m[Kget_threat_text_color[m[K
2025-11-08T13:22:49.9385700Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1054:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-08T13:22:49.9386016Z  1054 |     [01;31m[Kset_dirty[m[K();
2025-11-08T13:22:49.9386137Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:49.9386554Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9387299Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1057:6:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-08T13:22:49.9387509Z  1057 | void [01;31m[KThreatCard[m[K::clear_card() {
2025-11-08T13:22:49.9387619Z       |      [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9396338Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid clear_card()[m[K':
2025-11-08T13:22:49.9397104Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1058:5:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.9397257Z  1058 |     [01;31m[Kis_active_[m[K = false;
2025-11-08T13:22:49.9397370Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9406893Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1059:5:[m[K [01;31m[Kerror: [m[K'[01m[Kcard_text_[m[K' was not declared in this scope
2025-11-08T13:22:49.9407046Z  1059 |     [01;31m[Kcard_text_[m[K.set("");
2025-11-08T13:22:49.9407161Z       |     [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9415386Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1060:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-08T13:22:49.9415578Z  1060 |     [01;31m[Kset_dirty[m[K();
2025-11-08T13:22:49.9415693Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:49.9416121Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9416881Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1063:13:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-08T13:22:49.9417111Z  1063 | std::string [01;31m[KThreatCard[m[K::render_compact() const {
2025-11-08T13:22:49.9417231Z       |             [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9418052Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1063:42:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string render_compact()[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.9418278Z  1063 | std::string ThreatCard::render_compact() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.9418420Z       |                                          [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9426062Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kstd::string render_compact()[m[K':
2025-11-08T13:22:49.9426939Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1064:10:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.9427111Z  1064 |     if (![01;31m[Kis_active_[m[K) return "";
2025-11-08T13:22:49.9427227Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9433837Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1067:33:[m[K [01;31m[Kerror: [m[K'[01m[Ktrend_[m[K' was not declared in this scope
2025-11-08T13:22:49.9434415Z  1067 |     const char* trend_symbol = ([01;31m[Ktrend_[m[K == MovementTrend::APPROACHING) ? "▲" :
2025-11-08T13:22:49.9434566Z       |                                 [01;31m[K^~~~~~[m[K
2025-11-08T13:22:49.9441631Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1069:32:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9442511Z  1069 |     const char* threat_abbr = ([01;31m[Kthreat_[m[K == ThreatLevel::CRITICAL) ? "CRIT" :
2025-11-08T13:22:49.9442773Z       |                                [01;31m[K^~~~~~~[m[K
2025-11-08T13:22:49.9450249Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1074:41:[m[K [01;31m[Kerror: [m[K'[01m[Kfrequency_[m[K' was not declared in this scope; did you mean '[01m[KFrequency[m[K'?
2025-11-08T13:22:49.9450712Z  1074 |     float freq_mhz = static_cast<float>([01;31m[Kfrequency_[m[K) / 1000000.0f;
2025-11-08T13:22:49.9450985Z       |                                         [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9451233Z       |                                         [32m[KFrequency[m[K
2025-11-08T13:22:49.9460753Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1078:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_name_[m[K' was not declared in this scope; did you mean '[01m[Kthreat_abbr[m[K'?
2025-11-08T13:22:49.9461018Z  1078 |                 [01;31m[Kthreat_name_[m[K.c_str(), *trend_symbol, freq_mhz, rssi_);
2025-11-08T13:22:49.9461149Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9461275Z       |                 [32m[Kthreat_abbr[m[K
2025-11-08T13:22:49.9466322Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1078:64:[m[K [01;31m[Kerror: [m[K'[01m[Krssi_[m[K' was not declared in this scope
2025-11-08T13:22:49.9466763Z  1078 |                 threat_name_.c_str(), *trend_symbol, freq_mhz, [01;31m[Krssi_[m[K);
2025-11-08T13:22:49.9466958Z       |                                                                [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9476437Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1081:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_name_[m[K' was not declared in this scope; did you mean '[01m[Kthreat_abbr[m[K'?
2025-11-08T13:22:49.9476860Z  1081 |                 [01;31m[Kthreat_name_[m[K.c_str(), *trend_symbol, freq_mhz, rssi_);
2025-11-08T13:22:49.9477000Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9477126Z       |                 [32m[Kthreat_abbr[m[K
2025-11-08T13:22:49.9482777Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1081:64:[m[K [01;31m[Kerror: [m[K'[01m[Krssi_[m[K' was not declared in this scope
2025-11-08T13:22:49.9483252Z  1081 |                 threat_name_.c_str(), *trend_symbol, freq_mhz, [01;31m[Krssi_[m[K);
2025-11-08T13:22:49.9483570Z       |                                                                [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9493032Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1069:17:[m[K [01;35m[Kwarning: [m[Kunused variable '[01m[Kthreat_abbr[m[K' [[01;35m[K-Wunused-variable[m[K]
2025-11-08T13:22:49.9494716Z  1069 |     const char* [01;35m[Kthreat_abbr[m[K = (threat_ == ThreatLevel::CRITICAL) ? "CRIT" :
2025-11-08T13:22:49.9494934Z       |                 [01;35m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9495646Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9496871Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1086:7:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-08T13:22:49.9497503Z  1086 | Color [01;31m[KThreatCard[m[K::get_card_bg_color() const {
2025-11-08T13:22:49.9497692Z       |       [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9499112Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1086:39:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_card_bg_color()[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.9499642Z  1086 | Color ThreatCard::get_card_bg_color() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.9499895Z       |                                       [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9507633Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_bg_color()[m[K':
2025-11-08T13:22:49.9508735Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1087:10:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.9509115Z  1087 |     if (![01;31m[Kis_active_[m[K) return Color::black();
2025-11-08T13:22:49.9509323Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9515673Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1088:13:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9517149Z  1088 |     switch ([01;31m[Kthreat_[m[K) {
2025-11-08T13:22:49.9517370Z       |             [01;31m[K^~~~~~~[m[K
2025-11-08T13:22:49.9518098Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9519127Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1098:7:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-08T13:22:49.9519548Z  1098 | Color [01;31m[KThreatCard[m[K::get_card_text_color() const {
2025-11-08T13:22:49.9519737Z       |       [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9521144Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1098:41:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_card_text_color()[m[K' cannot have cv-qualifier
2025-11-08T13:22:49.9521517Z  1098 | Color ThreatCard::get_card_text_color() [01;31m[Kconst[m[K {
2025-11-08T13:22:49.9521764Z       |                                         [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9530954Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_text_color()[m[K':
2025-11-08T13:22:49.9531708Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1099:10:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.9531949Z  1099 |     if (![01;31m[Kis_active_[m[K) return Color::white();
2025-11-08T13:22:49.9532086Z       |          [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9537969Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1100:13:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_[m[K' was not declared in this scope
2025-11-08T13:22:49.9538221Z  1100 |     switch ([01;31m[Kthreat_[m[K) {
2025-11-08T13:22:49.9538399Z       |             [01;31m[K^~~~~~~[m[K
2025-11-08T13:22:49.9539159Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9540207Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1110:6:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' has not been declared
2025-11-08T13:22:49.9540609Z  1110 | void [01;31m[KThreatCard[m[K::paint(Painter& painter) {
2025-11-08T13:22:49.9540794Z       |      [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9542019Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1110:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-08T13:22:49.9543469Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-08T13:22:49.9543887Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-08T13:22:49.9544229Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9545284Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-08T13:22:49.9547029Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1111:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-08T13:22:49.9547289Z  1111 |     View::paint(painter[01;31m[K)[m[K;
2025-11-08T13:22:49.9547509Z       |                        [01;31m[K^[m[K
2025-11-08T13:22:49.9554404Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1112:9:[m[K [01;31m[Kerror: [m[K'[01m[Kis_active_[m[K' was not declared in this scope
2025-11-08T13:22:49.9554640Z  1112 |     if ([01;31m[Kis_active_[m[K) {
2025-11-08T13:22:49.9554846Z       |         [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:49.9567474Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1114:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope
2025-11-08T13:22:49.9568154Z  1114 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect().top(), parent_rect_.width(), 2}, bg_color);
2025-11-08T13:22:49.9568406Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9579859Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1114:54:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect[m[K' was not declared in this scope
2025-11-08T13:22:49.9580342Z  1114 |         painter.fill_rectangle({parent_rect_.left(), [01;31m[Kparent_rect[m[K().top(), parent_rect_.width(), 2}, bg_color);
2025-11-08T13:22:49.9580524Z       |                                                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9581499Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1114:109:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-08T13:22:49.9581960Z  1114 |         painter.fill_rectangle({parent_rect_.left(), parent_rect().top(), parent_rect_.width(), 2}, bg_color[01;31m[K)[m[K;
2025-11-08T13:22:49.9582172Z       |                                                                                                             [01;31m[K^[m[K
2025-11-08T13:22:49.9582449Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-08T13:22:49.9582713Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-08T13:22:49.9583152Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-08T13:22:49.9583576Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-08T13:22:49.9583995Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.9584663Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-08T13:22:49.9584855Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-08T13:22:49.9585193Z       |                         [01;36m[K~~~~~^[m[K
2025-11-08T13:22:49.9595603Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9597797Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1118:1:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' does not name a type
2025-11-08T13:22:49.9598590Z  1118 | [01;31m[KConsoleStatusBar[m[K::ConsoleStatusBar(size_t bar_index, Rect parent_rect)
2025-11-08T13:22:49.9598788Z       | [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9599843Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1124:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-08T13:22:49.9600691Z  1124 | void [01;31m[KConsoleStatusBar[m[K::update_scanning_progress(uint32_t progress_percent, uint32_t total_cycles, uint32_t detections) {
2025-11-08T13:22:49.9600898Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9602018Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_scanning_progress(uint32_t, uint32_t, uint32_t)[m[K':
2025-11-08T13:22:49.9603067Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1125:22:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-08T13:22:49.9603454Z  1125 |     set_display_mode([01;31m[KDisplayMode[m[K::SCANNING);
2025-11-08T13:22:49.9603668Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9620913Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1125:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_display_mode[m[K' was not declared in this scope
2025-11-08T13:22:49.9621396Z  1125 |     [01;31m[Kset_display_mode[m[K(DisplayMode::SCANNING);
2025-11-08T13:22:49.9621570Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9622409Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1127:28:[m[K [01;31m[Kerror: [m[Kinitializer-string for array of chars is too long [[01;31m[K-fpermissive[m[K]
2025-11-08T13:22:49.9622600Z  1127 |     char progress_bar[9] = [01;31m[K"░░░░░░░░"[m[K;
2025-11-08T13:22:49.9622765Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9623690Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1130:27:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849672[m[K' to '[01m[K'\210'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-08T13:22:49.9623843Z  1130 |         progress_bar[i] = [01;35m[K'█'[m[K;
2025-11-08T13:22:49.9623975Z       |                           [01;35m[K^~~~~[m[K
2025-11-08T13:22:49.9624972Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1134:43:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 5 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:49.9625203Z  1134 |     snprintf(buffer, sizeof(buffer), "%s [01;35m[K%u[m[K%% C:%u D:%u",
2025-11-08T13:22:49.9625348Z       |                                          [01;35m[K~^[m[K
2025-11-08T13:22:49.9625477Z       |                                           [01;35m[K|[m[K
2025-11-08T13:22:49.9625634Z       |                                           [01;35m[Kunsigned int[m[K
2025-11-08T13:22:49.9625769Z       |                                          [32m[K%lu[m[K
2025-11-08T13:22:49.9626025Z  1135 |             progress_bar, [32m[Kprogress_percent[m[K, total_cycles, detections);
2025-11-08T13:22:49.9626158Z       |                           [32m[K~~~~~~~~~~~~~~~~[m[K 
2025-11-08T13:22:49.9626468Z       |                           [32m[K|[m[K
2025-11-08T13:22:49.9626797Z       |                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-08T13:22:49.9627994Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1134:50:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 6 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:49.9628246Z  1134 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:[01;35m[K%u[m[K D:%u",
2025-11-08T13:22:49.9628391Z       |                                                 [01;35m[K~^[m[K
2025-11-08T13:22:49.9628527Z       |                                                  [01;35m[K|[m[K
2025-11-08T13:22:49.9628688Z       |                                                  [01;35m[Kunsigned int[m[K
2025-11-08T13:22:49.9628836Z       |                                                 [32m[K%lu[m[K
2025-11-08T13:22:49.9629100Z  1135 |             progress_bar, progress_percent, [32m[Ktotal_cycles[m[K, detections);
2025-11-08T13:22:49.9629254Z       |                                             [32m[K~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9629390Z       |                                             [32m[K|[m[K
2025-11-08T13:22:49.9629586Z       |                                             [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-08T13:22:49.9631272Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1134:55:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 7 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:49.9631735Z  1134 |     snprintf(buffer, sizeof(buffer), "%s %u%% C:%u D:[01;35m[K%u[m[K",
2025-11-08T13:22:49.9632019Z       |                                                      [01;35m[K~^[m[K
2025-11-08T13:22:49.9632292Z       |                                                       [01;35m[K|[m[K
2025-11-08T13:22:49.9632593Z       |                                                       [01;35m[Kunsigned int[m[K
2025-11-08T13:22:49.9632888Z       |                                                      [32m[K%lu[m[K
2025-11-08T13:22:49.9633383Z  1135 |             progress_bar, progress_percent, total_cycles, [32m[Kdetections[m[K);
2025-11-08T13:22:49.9633678Z       |                                                           [32m[K~~~~~~~~~~[m[K
2025-11-08T13:22:49.9633940Z       |                                                           [32m[K|[m[K
2025-11-08T13:22:49.9634298Z       |                                                           [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-08T13:22:49.9635258Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1136:5:[m[K [01;31m[Kerror: [m[K'[01m[Kprogress_text_[m[K' was not declared in this scope; did you mean '[01m[Kprogress_bar[m[K'?
2025-11-08T13:22:49.9635431Z  1136 |     [01;31m[Kprogress_text_[m[K.set(buffer);
2025-11-08T13:22:49.9635550Z       |     [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9635668Z       |     [32m[Kprogress_bar[m[K
2025-11-08T13:22:49.9636272Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1140:26:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-08T13:22:49.9636515Z  1140 |         set_display_mode([01;31m[KDisplayMode[m[K::ALERT);
2025-11-08T13:22:49.9636780Z       |                          [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9637783Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1141:61:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:49.9638341Z  1141 |         snprintf(buffer, sizeof(buffer), "⚠️ DETECTED: [01;35m[K%u[m[K threats found!", [32m[Kdetections[m[K);
2025-11-08T13:22:49.9638653Z       |                                                            [01;35m[K~^[m[K                  [32m[K~~~~~~~~~~[m[K
2025-11-08T13:22:49.9638843Z       |                                                             [01;35m[K|[m[K                  [32m[K|[m[K
2025-11-08T13:22:49.9639122Z       |                                                             [01;35m[Kunsigned int[m[K       [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-08T13:22:49.9639273Z       |                                                            [32m[K%lu[m[K
2025-11-08T13:22:49.9645657Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1142:9:[m[K [01;31m[Kerror: [m[K'[01m[Kalert_text_[m[K' was not declared in this scope
2025-11-08T13:22:49.9645847Z  1142 |         [01;31m[Kalert_text_[m[K.set(buffer);
2025-11-08T13:22:49.9645963Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9654367Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1145:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-08T13:22:49.9654609Z  1145 |     [01;31m[Kset_dirty[m[K();
2025-11-08T13:22:49.9654801Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:49.9655528Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9656549Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1148:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-08T13:22:49.9657377Z  1148 | void [01;31m[KConsoleStatusBar[m[K::update_alert_status(ThreatLevel threat, size_t total_drones, const std::string& alert_msg) {
2025-11-08T13:22:49.9657506Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9658166Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_alert_status(ThreatLevel, size_t, const string&)[m[K':
2025-11-08T13:22:49.9658770Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1149:22:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-08T13:22:49.9658994Z  1149 |     set_display_mode([01;31m[KDisplayMode[m[K::ALERT);
2025-11-08T13:22:49.9659126Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9672933Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1149:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_display_mode[m[K' was not declared in this scope
2025-11-08T13:22:49.9673391Z  1149 |     [01;31m[Kset_display_mode[m[K(DisplayMode::ALERT);
2025-11-08T13:22:49.9673614Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9686298Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1158:5:[m[K [01;31m[Kerror: [m[K'[01m[Kalert_text_[m[K' was not declared in this scope
2025-11-08T13:22:49.9686590Z  1158 |     [01;31m[Kalert_text_[m[K.set(buffer);
2025-11-08T13:22:49.9686960Z       |     [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9688002Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1159:126:[m[K [01;31m[Kerror: [m[Kexpression cannot be used as a function
2025-11-08T13:22:49.9688828Z  1159 |     alert_text_.set_style((threat >= ThreatLevel::CRITICAL) ? Theme::getInstance()->fg_red : Theme::getInstance()->fg_yellow([01;31m[K)[m[K);
2025-11-08T13:22:49.9689198Z       |                                                                                                                              [01;31m[K^[m[K
2025-11-08T13:22:49.9697720Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1160:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-08T13:22:49.9697967Z  1160 |     [01;31m[Kset_dirty[m[K();
2025-11-08T13:22:49.9698145Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:49.9699070Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9700151Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1163:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-08T13:22:49.9700898Z  1163 | void [01;31m[KConsoleStatusBar[m[K::update_normal_status(const std::string& primary, const std::string& secondary) {
2025-11-08T13:22:49.9701092Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9702191Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_normal_status(const string&, const string&)[m[K':
2025-11-08T13:22:49.9703209Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1164:22:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-08T13:22:49.9703581Z  1164 |     set_display_mode([01;31m[KDisplayMode[m[K::NORMAL);
2025-11-08T13:22:49.9703801Z       |                      [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9718321Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1164:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_display_mode[m[K' was not declared in this scope
2025-11-08T13:22:49.9718635Z  1164 |     [01;31m[Kset_display_mode[m[K(DisplayMode::NORMAL);
2025-11-08T13:22:49.9718787Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9731487Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1172:5:[m[K [01;31m[Kerror: [m[K'[01m[Knormal_text_[m[K' was not declared in this scope
2025-11-08T13:22:49.9731649Z  1172 |     [01;31m[Knormal_text_[m[K.set(buffer);
2025-11-08T13:22:49.9731765Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9740475Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1174:5:[m[K [01;31m[Kerror: [m[K'[01m[Kset_dirty[m[K' was not declared in this scope
2025-11-08T13:22:49.9740747Z  1174 |     [01;31m[Kset_dirty[m[K();
2025-11-08T13:22:49.9740942Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:49.9741645Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9742695Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1177:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-08T13:22:49.9743233Z  1177 | void [01;31m[KConsoleStatusBar[m[K::set_display_mode(DisplayMode mode) {
2025-11-08T13:22:49.9743451Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9744607Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1177:41:[m[K [01;31m[Kerror: [m[Kvariable or field '[01m[Kset_display_mode[m[K' declared void
2025-11-08T13:22:49.9745095Z  1177 | void ConsoleStatusBar::set_display_mode([01;31m[KDisplayMode[m[K mode) {
2025-11-08T13:22:49.9745353Z       |                                         [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9747302Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1177:41:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::DisplayMode[m[K'?
2025-11-08T13:22:49.9747970Z  1177 | void ConsoleStatusBar::set_display_mode([01;31m[KDisplayMode[m[K mode) {
2025-11-08T13:22:49.9748223Z       |                                         [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9748713Z       |                                         [32m[Kui::external_app::enhanced_drone_analyzer::DisplayMode[m[K
2025-11-08T13:22:49.9749609Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.9750949Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:527:12:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::DisplayMode[m[K' declared here
2025-11-08T13:22:49.9751334Z   527 | enum class [01;36m[KDisplayMode[m[K { SCANNING, ALERT, NORMAL };
2025-11-08T13:22:49.9751532Z       |            [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9752577Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1193:6:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' has not been declared
2025-11-08T13:22:49.9752995Z  1193 | void [01;31m[KConsoleStatusBar[m[K::paint(Painter& painter) {
2025-11-08T13:22:49.9753188Z       |      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9754392Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1193:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-08T13:22:49.9755592Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-08T13:22:49.9755994Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-08T13:22:49.9756187Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9757411Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-08T13:22:49.9758896Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1194:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-08T13:22:49.9759142Z  1194 |     View::paint(painter[01;31m[K)[m[K;
2025-11-08T13:22:49.9759350Z       |                        [01;31m[K^[m[K
2025-11-08T13:22:49.9760577Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1195:9:[m[K [01;31m[Kerror: [m[K'[01m[Kmode_[m[K' was not declared in this scope; did you mean '[01m[Kmode_t[m[K'?
2025-11-08T13:22:49.9760912Z  1195 |     if ([01;31m[Kmode_[m[K == DisplayMode::ALERT) {
2025-11-08T13:22:49.9761101Z       |         [01;31m[K^~~~~[m[K
2025-11-08T13:22:49.9761278Z       |         [32m[Kmode_t[m[K
2025-11-08T13:22:49.9762313Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1195:18:[m[K [01;31m[Kerror: [m[K'[01m[KDisplayMode[m[K' has not been declared
2025-11-08T13:22:49.9762652Z  1195 |     if (mode_ == [01;31m[KDisplayMode[m[K::ALERT) {
2025-11-08T13:22:49.9762859Z       |                  [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9767953Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1196:33:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect_[m[K' was not declared in this scope
2025-11-08T13:22:49.9768692Z  1196 |         painter.fill_rectangle({[01;31m[Kparent_rect_[m[K.left(), parent_rect_.top(), parent_rect().width(), 2}, Color(32, 0, 0));
2025-11-08T13:22:49.9768936Z       |                                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9780725Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1196:74:[m[K [01;31m[Kerror: [m[K'[01m[Kparent_rect[m[K' was not declared in this scope
2025-11-08T13:22:49.9781696Z  1196 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), [01;31m[Kparent_rect[m[K().width(), 2}, Color(32, 0, 0));
2025-11-08T13:22:49.9782004Z       |                                                                          [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9783610Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1196:116:[m[K [01;31m[Kerror: [m[Kcannot convert '[01m[K<brace-enclosed initializer list>[m[K' to '[01m[Kui::Rect[m[K'
2025-11-08T13:22:49.9784341Z  1196 |         painter.fill_rectangle({parent_rect_.left(), parent_rect_.top(), parent_rect().width(), 2}, Color(32, 0, 0)[01;31m[K)[m[K;
2025-11-08T13:22:49.9784690Z       |                                                                                                                    [01;31m[K^[m[K
2025-11-08T13:22:49.9785106Z In file included from [01m[K/havoc/firmware/application/./theme.hpp:27[m[K,
2025-11-08T13:22:49.9785516Z                  from [01m[K/havoc/firmware/application/./ui_navigation.hpp:33[m[K,
2025-11-08T13:22:49.9786187Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanning_coordinator.hpp:8[m[K,
2025-11-08T13:22:49.9787036Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:27[m[K,
2025-11-08T13:22:49.9787701Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:49.9788747Z [01m[K/havoc/firmware/common/ui_painter.hpp:81:30:[m[K [01;36m[Knote: [m[K  initializing argument 1 of '[01m[Kvoid ui::Painter::fill_rectangle(ui::Rect, ui::Color)[m[K'
2025-11-08T13:22:49.9789055Z    81 |     void fill_rectangle([01;36m[KRect r[m[K, Color c);
2025-11-08T13:22:49.9789275Z       |                         [01;36m[K~~~~~^[m[K
2025-11-08T13:22:49.9798564Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:49.9800028Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1200:1:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' does not name a type; did you mean '[01m[KDroneHardwareController[m[K'?
2025-11-08T13:22:49.9800604Z  1200 | [01;31m[KDroneDisplayController[m[K::DroneDisplayController(NavigationView& nav)
2025-11-08T13:22:49.9800805Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9801030Z       | [32m[KDroneHardwareController[m[K
2025-11-08T13:22:49.9802103Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:38:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9802637Z  1201 |     : nav_(nav), spectrum_gradient_{}[01;31m[K,[m[K big_display_({0, 0, screen_width, 32}, "2400.0MHz"),
2025-11-08T13:22:49.9802880Z       |                                      [01;31m[K^[m[K
2025-11-08T13:22:49.9804146Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:52:[m[K [01;31m[Kerror: [m[Kexpected constructor, destructor, or type conversion before '[01m[K([m[K' token
2025-11-08T13:22:49.9804697Z  1201 |     : nav_(nav), spectrum_gradient_{}, big_display_[01;31m[K([m[K{0, 0, screen_width, 32}, "2400.0MHz"),
2025-11-08T13:22:49.9804954Z       |                                                    [01;31m[K^[m[K
2025-11-08T13:22:49.9806038Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:77:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9806593Z  1201 |     : nav_(nav), spectrum_gradient_{}, big_display_({0, 0, screen_width, 32}[01;31m[K,[m[K "2400.0MHz"),
2025-11-08T13:22:49.9807074Z       |                                                                             [01;31m[K^[m[K
2025-11-08T13:22:49.9808327Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1201:79:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.9808886Z  1201 |     : nav_(nav), spectrum_gradient_{}, big_display_({0, 0, screen_width, 32}, [01;31m[K"2400.0MHz"[m[K),
2025-11-08T13:22:49.9809346Z       |                                                                               [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:49.9810434Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1202:53:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9810939Z  1202 |       text_threat_summary_({0, 32, screen_width, 16}[01;31m[K,[m[K "THREAT: NONE | All clear"),
2025-11-08T13:22:49.9811198Z       |                                                     [01;31m[K^[m[K
2025-11-08T13:22:49.9812247Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1202:55:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.9812750Z  1202 |       text_threat_summary_({0, 32, screen_width, 16}, [01;31m[K"THREAT: NONE | All clear"[m[K),
2025-11-08T13:22:49.9813057Z       |                                                       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9814106Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1203:50:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9814636Z  1203 |       text_status_info_({0, 48, screen_width, 16}[01;31m[K,[m[K "Ready - Enhanced Drone Analyzer"),
2025-11-08T13:22:49.9814943Z       |                                                  [01;31m[K^[m[K
2025-11-08T13:22:49.9815978Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1203:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.9816462Z  1203 |       text_status_info_({0, 48, screen_width, 16}, [01;31m[K"Ready - Enhanced Drone Analyzer"[m[K),
2025-11-08T13:22:49.9816950Z       |                                                    [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9818029Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1204:52:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9818490Z  1204 |       text_scanner_stats_({0, 64, screen_width, 16}[01;31m[K,[m[K "No database loaded"),
2025-11-08T13:22:49.9818743Z       |                                                    [01;31m[K^[m[K
2025-11-08T13:22:49.9819769Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1204:54:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.9820225Z  1204 |       text_scanner_stats_({0, 64, screen_width, 16}, [01;31m[K"No database loaded"[m[K),
2025-11-08T13:22:49.9820510Z       |                                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9821570Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1205:51:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K)[m[K' token
2025-11-08T13:22:49.9821924Z  1205 |       scanning_progress_({0, 80, screen_width, 16}[01;31m[K)[m[K,
2025-11-08T13:22:49.9822177Z       |                                                   [01;31m[K^[m[K
2025-11-08T13:22:49.9823282Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1206:45:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9823609Z  1206 |       text_drone_1({0, 96, screen_width, 16}[01;31m[K,[m[K ""),
2025-11-08T13:22:49.9824047Z       |                                             [01;31m[K^[m[K
2025-11-08T13:22:49.9825073Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1206:47:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.9825399Z  1206 |       text_drone_1({0, 96, screen_width, 16}, [01;31m[K""[m[K),
2025-11-08T13:22:49.9825811Z       |                                               [01;31m[K^~[m[K
2025-11-08T13:22:49.9827076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1207:46:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9827421Z  1207 |       text_drone_2({0, 112, screen_width, 16}[01;31m[K,[m[K ""),
2025-11-08T13:22:49.9827666Z       |                                              [01;31m[K^[m[K
2025-11-08T13:22:49.9828703Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1207:48:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.9829035Z  1207 |       text_drone_2({0, 112, screen_width, 16}, [01;31m[K""[m[K),
2025-11-08T13:22:49.9829278Z       |                                                [01;31m[K^~[m[K
2025-11-08T13:22:49.9830350Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1208:46:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:49.9830677Z  1208 |       text_drone_3({0, 128, screen_width, 16}[01;31m[K,[m[K ""),
2025-11-08T13:22:49.9830915Z       |                                              [01;31m[K^[m[K
2025-11-08T13:22:49.9831943Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1208:48:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:49.9832276Z  1208 |       text_drone_3({0, 128, screen_width, 16}, [01;31m[K""[m[K),
2025-11-08T13:22:49.9832514Z       |                                                [01;31m[K^~[m[K
2025-11-08T13:22:49.9833629Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1241:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:49.9834284Z  1241 | void [01;31m[KDroneDisplayController[m[K::update_detection_display(const DroneScanner& scanner) {
2025-11-08T13:22:49.9834499Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9835534Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1241:61:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-08T13:22:49.9836161Z  1241 | void DroneDisplayController::update_detection_display(const [01;31m[KDroneScanner[m[K& scanner) {
2025-11-08T13:22:49.9836434Z       |                                                             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9837616Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_detection_display(const int&)[m[K':
2025-11-08T13:22:49.9839094Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1242:17:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9839398Z  1242 |     if (scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-08T13:22:49.9839614Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9841150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1243:42:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_current_scanning_frequency[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9841835Z  1243 |         Frequency current_freq = scanner.[01;31m[Kget_current_scanning_frequency[m[K();
2025-11-08T13:22:49.9842121Z       |                                          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9843334Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1245:13:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:49.9843735Z  1245 |             [01;31m[Kbig_display_[m[K.set(to_string_short_freq(current_freq));
2025-11-08T13:22:49.9843935Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9854501Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1247:13:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:49.9854746Z  1247 |             [01;31m[Kbig_display_[m[K.set("2400.0MHz");
2025-11-08T13:22:49.9854913Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9866565Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1250:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:49.9866980Z  1250 |         [01;31m[Kbig_display_[m[K.set("READY");
2025-11-08T13:22:49.9867111Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9868000Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1253:34:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_database_size[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9868234Z  1253 |     size_t total_freqs = scanner.[01;31m[Kget_database_size[m[K();
2025-11-08T13:22:49.9868382Z       |                                  [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9869262Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1254:36:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9869513Z  1254 |     if (total_freqs > 0 && scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-08T13:22:49.9869665Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9884171Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1256:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_progress_[m[K' was not declared in this scope; did you mean '[01m[Kupdate_scanning_progress[m[K'?
2025-11-08T13:22:49.9884772Z  1256 |         [01;31m[Kscanning_progress_[m[K.set_value(std::min(progress_percent, (uint32_t)100));
2025-11-08T13:22:49.9884979Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9885226Z       |         [32m[Kupdate_scanning_progress[m[K
2025-11-08T13:22:49.9899976Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1258:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_progress_[m[K' was not declared in this scope; did you mean '[01m[Kupdate_scanning_progress[m[K'?
2025-11-08T13:22:49.9900195Z  1258 |         [01;31m[Kscanning_progress_[m[K.set_value(0);
2025-11-08T13:22:49.9900342Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9900495Z       |         [32m[Kupdate_scanning_progress[m[K
2025-11-08T13:22:49.9901476Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1261:38:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_max_detected_threat[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9901772Z  1261 |     ThreatLevel max_threat = scanner.[01;31m[Kget_max_detected_threat[m[K();
2025-11-08T13:22:49.9902133Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9903099Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1262:36:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_approaching_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9903767Z  1262 |     bool has_detections = (scanner.[01;31m[Kget_approaching_count[m[K() + scanner.get_receding_count() + scanner.get_static_count()) > 0;
2025-11-08T13:22:49.9903951Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9904914Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1262:70:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_receding_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9905582Z  1262 |     bool has_detections = (scanner.get_approaching_count() + scanner.[01;31m[Kget_receding_count[m[K() + scanner.get_static_count()) > 0;
2025-11-08T13:22:49.9905791Z       |                                                                      [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9906899Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1262:101:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_static_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9907427Z  1262 |     bool has_detections = (scanner.get_approaching_count() + scanner.get_receding_count() + scanner.[01;31m[Kget_static_count[m[K()) > 0;
2025-11-08T13:22:49.9907651Z       |                                                                                                     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9916309Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1267:17:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_level_name[m[K' was not declared in this scope
2025-11-08T13:22:49.9916765Z  1267 |                 [01;31m[Kget_threat_level_name[m[K(max_threat), scanner.get_approaching_count(),
2025-11-08T13:22:49.9916908Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9917775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1267:60:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_approaching_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9918076Z  1267 |                 get_threat_level_name(max_threat), scanner.[01;31m[Kget_approaching_count[m[K(),
2025-11-08T13:22:49.9918243Z       |                                                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9919057Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1268:25:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_static_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9919315Z  1268 |                 scanner.[01;31m[Kget_static_count[m[K(), scanner.get_receding_count());
2025-11-08T13:22:49.9919450Z       |                         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9920283Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1268:53:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_receding_count[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9920543Z  1268 |                 scanner.get_static_count(), scanner.[01;31m[Kget_receding_count[m[K());
2025-11-08T13:22:49.9920702Z       |                                                     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9933498Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1269:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_threat_summary_[m[K' was not declared in this scope
2025-11-08T13:22:49.9933910Z  1269 |         [01;31m[Ktext_threat_summary_[m[K.set(summary_buffer);
2025-11-08T13:22:49.9934039Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9950815Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1272:9:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_threat_summary_[m[K' was not declared in this scope
2025-11-08T13:22:49.9951163Z  1272 |         [01;31m[Ktext_threat_summary_[m[K.set("THREAT: NONE | All clear");
2025-11-08T13:22:49.9951295Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9952330Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1277:17:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9952535Z  1277 |     if (scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-08T13:22:49.9952666Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9953477Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1278:40:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_real_mode[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9953749Z  1278 |         std::string mode_str = scanner.[01;31m[Kis_real_mode[m[K() ? "REAL" : "DEMO";
2025-11-08T13:22:49.9953893Z       |                                        [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9954733Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1280:43:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_total_detections[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9954965Z  1280 |                 mode_str.c_str(), scanner.[01;31m[Kget_total_detections[m[K());
2025-11-08T13:22:49.9955114Z       |                                           [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9969587Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1284:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_status_info_[m[K' was not declared in this scope
2025-11-08T13:22:49.9969918Z  1284 |     [01;31m[Ktext_status_info_[m[K.set(status_buffer);
2025-11-08T13:22:49.9970150Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9971060Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1286:35:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_database_size[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9971291Z  1286 |     size_t loaded_freqs = scanner.[01;31m[Kget_database_size[m[K();
2025-11-08T13:22:49.9971445Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9972280Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1288:17:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9972513Z  1288 |     if (scanner.[01;31m[Kis_scanning_active[m[K() && loaded_freqs > 0) {
2025-11-08T13:22:49.9972656Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9973480Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1291:56:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kget_scan_cycles[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:49.9973742Z  1291 |                 current_idx + 1, loaded_freqs, scanner.[01;31m[Kget_scan_cycles[m[K());
2025-11-08T13:22:49.9974093Z       |                                                        [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9986249Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1297:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_scanner_stats_[m[K' was not declared in this scope
2025-11-08T13:22:49.9986776Z  1297 |     [01;31m[Ktext_scanner_stats_[m[K.set(stats_buffer);
2025-11-08T13:22:49.9986924Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:49.9999099Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1300:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:49.9999399Z  1300 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_red);
2025-11-08T13:22:49.9999523Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0012014Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1302:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:50.0012322Z  1302 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_yellow);
2025-11-08T13:22:50.0012447Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0025166Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1304:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:50.0025474Z  1304 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_orange);
2025-11-08T13:22:50.0025599Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0026445Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1305:24:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kis_scanning_active[m[K' in '[01m[Kscanner[m[K', which is of non-class type '[01m[Kconst int[m[K'
2025-11-08T13:22:50.0026799Z  1305 |     } else if (scanner.[01;31m[Kis_scanning_active[m[K()) {
2025-11-08T13:22:50.0026948Z       |                        [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0038174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1306:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:50.0038482Z  1306 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->fg_green);
2025-11-08T13:22:50.0038598Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0051497Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1308:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbig_display_[m[K' was not declared in this scope
2025-11-08T13:22:50.0052040Z  1308 |         [01;31m[Kbig_display_[m[K.set_style(Theme::getInstance()->bg_darkest);
2025-11-08T13:22:50.0052281Z       |         [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0053006Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0053769Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1312:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0054290Z  1312 | void [01;31m[KDroneDisplayController[m[K::add_detected_drone(Frequency freq, DroneType type, ThreatLevel threat, int32_t rssi) {
2025-11-08T13:22:50.0054421Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0067692Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)[m[K':
2025-11-08T13:22:50.0069099Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1314:28:[m[K [01;31m[Kerror: [m[K'[01m[Kdetected_drones_[m[K' was not declared in this scope; did you mean '[01m[Kadd_detected_drone[m[K'?
2025-11-08T13:22:50.0069823Z  1314 |     auto it = std::find_if([01;31m[Kdetected_drones_[m[K.begin(), detected_drones_.end(),
2025-11-08T13:22:50.0070071Z       |                            [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0070454Z       |                            [32m[Kadd_detected_drone[m[K
2025-11-08T13:22:50.0088195Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1323:25:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-08T13:22:50.0088479Z  1323 |         it->type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-08T13:22:50.0088660Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0103574Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1324:29:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-08T13:22:50.0103912Z  1324 |         it->display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-08T13:22:50.0104179Z       |                             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0106312Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.0107677Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:[m[K In constructor '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K':
2025-11-08T13:22:50.0108745Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::frequency[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0108911Z   130 | struct [01;35m[KDisplayDroneEntry[m[K {
2025-11-08T13:22:50.0109031Z       |        [01;35m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0109931Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0110840Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::threat[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0111714Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::rssi[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0112603Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::last_seen[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0113519Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::type_name[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0114438Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::display_color[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0115312Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:130:8:[m[K [01;35m[Kwarning: [m[K'[01m[KDisplayDroneEntry::trend[m[K' should be initialized in the member initialization list [[01;35m[K-Weffc++[m[K]
2025-11-08T13:22:50.0116157Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid add_detected_drone(Frequency, DroneType, ThreatLevel, int32_t)[m[K':
2025-11-08T13:22:50.0117199Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1327:31:[m[K [01;36m[Knote: [m[Ksynthesized method '[01m[KDisplayDroneEntry::DisplayDroneEntry()[m[K' first required here
2025-11-08T13:22:50.0117377Z  1327 |             DisplayDroneEntry [01;36m[Kentry[m[K;
2025-11-08T13:22:50.0117507Z       |                               [01;36m[K^~~~~[m[K
2025-11-08T13:22:50.0126253Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1333:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_name[m[K' was not declared in this scope
2025-11-08T13:22:50.0126530Z  1333 |             entry.type_name = [01;31m[Kget_drone_type_name[m[K(type);
2025-11-08T13:22:50.0126927Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0142440Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1334:35:[m[K [01;31m[Kerror: [m[K'[01m[Kget_drone_type_color[m[K' was not declared in this scope
2025-11-08T13:22:50.0142875Z  1334 |             entry.display_color = [01;31m[Kget_drone_type_color[m[K(type);
2025-11-08T13:22:50.0143130Z       |                                   [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0159363Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1339:5:[m[K [01;31m[Kerror: [m[K'[01m[Kupdate_drones_display[m[K' was not declared in this scope; did you mean '[01m[Kupdate_detection_display[m[K'?
2025-11-08T13:22:50.0160816Z  1339 |     [01;31m[Kupdate_drones_display[m[K();
2025-11-08T13:22:50.0161042Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0161270Z       |     [32m[Kupdate_detection_display[m[K
2025-11-08T13:22:50.0161994Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0163110Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1342:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0163577Z  1342 | void [01;31m[KDroneDisplayController[m[K::sort_drones_by_rssi() {
2025-11-08T13:22:50.0163786Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0176079Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid sort_drones_by_rssi()[m[K':
2025-11-08T13:22:50.0177739Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1343:15:[m[K [01;31m[Kerror: [m[K'[01m[Kdetected_drones_[m[K' was not declared in this scope; did you mean '[01m[Kadd_detected_drone[m[K'?
2025-11-08T13:22:50.0178210Z  1343 |     std::sort([01;31m[Kdetected_drones_[m[K.begin(), detected_drones_.end(),
2025-11-08T13:22:50.0178424Z       |               [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0178654Z       |               [32m[Kadd_detected_drone[m[K
2025-11-08T13:22:50.0179699Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0181512Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1351:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0183185Z  1351 | void [01;31m[KDroneDisplayController[m[K::update_drones_display(const DroneScanner& scanner) {
2025-11-08T13:22:50.0183966Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0194040Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1351:58:[m[K [01;31m[Kerror: [m[K'[01m[KDroneScanner[m[K' does not name a type
2025-11-08T13:22:50.0195014Z  1351 | void DroneDisplayController::update_drones_display(const [01;31m[KDroneScanner[m[K& scanner) {
2025-11-08T13:22:50.0195542Z       |                                                          [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0208533Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_drones_display(const int&)[m[K':
2025-11-08T13:22:50.0209927Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1354:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdetected_drones_[m[K' was not declared in this scope; did you mean '[01m[Kadd_detected_drone[m[K'?
2025-11-08T13:22:50.0210846Z  1354 |     [01;31m[Kdetected_drones_[m[K.erase(
2025-11-08T13:22:50.0211160Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0211435Z       |     [32m[Kadd_detected_drone[m[K
2025-11-08T13:22:50.0226546Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1361:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplayed_drones_[m[K' was not declared in this scope
2025-11-08T13:22:50.0228065Z  1361 |     [01;31m[Kdisplayed_drones_[m[K.clear();
2025-11-08T13:22:50.0228580Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0241553Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1366:5:[m[K [01;31m[Kerror: [m[K'[01m[Khighlight_threat_zones_in_spectrum[m[K' was not declared in this scope
2025-11-08T13:22:50.0243103Z  1366 |     [01;31m[Khighlight_threat_zones_in_spectrum[m[K(displayed_drones_);
2025-11-08T13:22:50.0243787Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0259943Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1367:5:[m[K [01;31m[Kerror: [m[K'[01m[Krender_drone_text_display[m[K' was not declared in this scope
2025-11-08T13:22:50.0262073Z  1367 |     [01;31m[Krender_drone_text_display[m[K();
2025-11-08T13:22:50.0262624Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0264127Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1351:72:[m[K [01;35m[Kwarning: [m[Kunused parameter '[01m[Kscanner[m[K' [[01;35m[K-Wunused-parameter[m[K]
2025-11-08T13:22:50.0265848Z  1351 | void DroneDisplayController::update_drones_display([01;35m[Kconst DroneScanner& scanner[m[K) {
2025-11-08T13:22:50.0266956Z       |                                                    [01;35m[K~~~~~~~~~~~~~~~~~~~~^~~~~~~[m[K
2025-11-08T13:22:50.0267981Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0269823Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1370:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0271338Z  1370 | void [01;31m[KDroneDisplayController[m[K::render_drone_text_display() {
2025-11-08T13:22:50.0272005Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0278647Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid render_drone_text_display()[m[K':
2025-11-08T13:22:50.0280672Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1371:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_drone_1[m[K' was not declared in this scope
2025-11-08T13:22:50.0281890Z  1371 |     [01;31m[Ktext_drone_1[m[K.set("");
2025-11-08T13:22:50.0282366Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0293771Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1372:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_drone_2[m[K' was not declared in this scope
2025-11-08T13:22:50.0295272Z  1372 |     [01;31m[Ktext_drone_2[m[K.set("");
2025-11-08T13:22:50.0295745Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0308589Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1373:5:[m[K [01;31m[Kerror: [m[K'[01m[Ktext_drone_3[m[K' was not declared in this scope
2025-11-08T13:22:50.0309395Z  1373 |     [01;31m[Ktext_drone_3[m[K.set("");
2025-11-08T13:22:50.0309698Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0325075Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1374:37:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplayed_drones_[m[K' was not declared in this scope
2025-11-08T13:22:50.0326862Z  1374 |     for (size_t i = 0; i < std::min([01;31m[Kdisplayed_drones_[m[K.size(), size_t(3)); ++i) {
2025-11-08T13:22:50.0327597Z       |                                     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0328854Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1379:61:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849714[m[K' to '[01m[K'\262'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-08T13:22:50.0329982Z  1379 |             case MovementTrend::APPROACHING: trend_symbol = [01;35m[K'▲'[m[K; break;
2025-11-08T13:22:50.0330441Z       |                                                             [01;35m[K^~~~~[m[K
2025-11-08T13:22:50.0331560Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1380:58:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849724[m[K' to '[01m[K'\274'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-08T13:22:50.0332631Z  1380 |             case MovementTrend::RECEDING: trend_symbol = [01;35m[K'▼'[m[K; break;
2025-11-08T13:22:50.0333061Z       |                                                          [01;35m[K^~~~~[m[K
2025-11-08T13:22:50.0334172Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1383:37:[m[K [01;35m[Kwarning: [m[Kunsigned conversion from '[01m[Kint[m[K' to '[01m[Kchar[m[K' changes value from '[01m[K14849696[m[K' to '[01m[K'\240'[m[K' [[01;35m[K-Woverflow[m[K]
2025-11-08T13:22:50.0335152Z  1383 |             default: trend_symbol = [01;35m[K'■'[m[K; break;
2025-11-08T13:22:50.0335497Z       |                                     [01;35m[K^~~~~[m[K
2025-11-08T13:22:50.0339214Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1398:30:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_level_color[m[K' was not declared in this scope; did you mean '[01m[Kget_threat_text_color[m[K'?
2025-11-08T13:22:50.0341000Z  1398 |         Color threat_color = [01;31m[Kget_threat_level_color[m[K(drone.threat);
2025-11-08T13:22:50.0341447Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0341780Z       |                              [32m[Kget_threat_text_color[m[K
2025-11-08T13:22:50.0342408Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0343491Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1416:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0344386Z  1416 | void [01;31m[KDroneDisplayController[m[K::initialize_mini_spectrum() {
2025-11-08T13:22:50.0344782Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0357295Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid initialize_mini_spectrum()[m[K':
2025-11-08T13:22:50.0358704Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1417:10:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-08T13:22:50.0359551Z  1417 |     if (![01;31m[Kspectrum_gradient_[m[K.load_file(default_gradient_file)) {
2025-11-08T13:22:50.0360133Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0374662Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1420:5:[m[K [01;31m[Kerror: [m[K'[01m[Kclear_spectrum_buffers[m[K' was not declared in this scope
2025-11-08T13:22:50.0375497Z  1420 |     [01;31m[Kclear_spectrum_buffers[m[K();
2025-11-08T13:22:50.0375804Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0376389Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0377677Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1423:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0378689Z  1423 | void [01;31m[KDroneDisplayController[m[K::process_mini_spectrum_data(const ChannelSpectrum& spectrum) {
2025-11-08T13:22:50.0379192Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0390066Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid process_mini_spectrum_data(const ChannelSpectrum&)[m[K':
2025-11-08T13:22:50.0392342Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1426:9:[m[K [01;31m[Kerror: [m[K'[01m[Kget_max_power_for_current_bin[m[K' was not declared in this scope
2025-11-08T13:22:50.0393887Z  1426 |         [01;31m[Kget_max_power_for_current_bin[m[K(spectrum, bin, current_bin_power);
2025-11-08T13:22:50.0394599Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0406204Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1427:13:[m[K [01;31m[Kerror: [m[K'[01m[Kprocess_bins[m[K' was not declared in this scope
2025-11-08T13:22:50.0407416Z  1427 |         if ([01;31m[Kprocess_bins[m[K(&current_bin_power)) {
2025-11-08T13:22:50.0407851Z       |             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0408572Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0409863Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1433:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0411038Z  1433 | bool [01;31m[KDroneDisplayController[m[K::process_bins(uint8_t* powerlevel) {
2025-11-08T13:22:50.0411565Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0418951Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool process_bins(uint8_t*)[m[K':
2025-11-08T13:22:50.0420989Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1434:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbins_hz_size[m[K' was not declared in this scope
2025-11-08T13:22:50.0422247Z  1434 |     [01;31m[Kbins_hz_size[m[K += each_bin_size;
2025-11-08T13:22:50.0422767Z       |     [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0435462Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1434:21:[m[K [01;31m[Kerror: [m[K'[01m[Keach_bin_size[m[K' was not declared in this scope
2025-11-08T13:22:50.0436943Z  1434 |     bins_hz_size += [01;31m[Keach_bin_size[m[K;
2025-11-08T13:22:50.0437497Z       |                     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0454093Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1435:25:[m[K [01;31m[Kerror: [m[K'[01m[Kmarker_pixel_step[m[K' was not declared in this scope
2025-11-08T13:22:50.0455473Z  1435 |     if (bins_hz_size >= [01;31m[Kmarker_pixel_step[m[K) {
2025-11-08T13:22:50.0456066Z       |                         [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0470657Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1436:27:[m[K [01;31m[Kerror: [m[K'[01m[Kmin_color_power[m[K' was not declared in this scope
2025-11-08T13:22:50.0472015Z  1436 |         if (*powerlevel > [01;31m[Kmin_color_power[m[K)
2025-11-08T13:22:50.0472591Z       |                           [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0489027Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1437:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel[m[K' was not declared in this scope
2025-11-08T13:22:50.0490400Z  1437 |             [01;31m[Kadd_spectrum_pixel[m[K(*powerlevel);
2025-11-08T13:22:50.0490987Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0508080Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1439:13:[m[K [01;31m[Kerror: [m[K'[01m[Kadd_spectrum_pixel[m[K' was not declared in this scope
2025-11-08T13:22:50.0509420Z  1439 |             [01;31m[Kadd_spectrum_pixel[m[K(0);
2025-11-08T13:22:50.0509952Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0521002Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1442:14:[m[K [01;31m[Kerror: [m[K'[01m[Kpixel_index[m[K' was not declared in this scope
2025-11-08T13:22:50.0522316Z  1442 |         if (![01;31m[Kpixel_index[m[K) {
2025-11-08T13:22:50.0522837Z       |              [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.0523919Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0525691Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1451:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0527832Z  1451 | void [01;31m[KDroneDisplayController[m[K::get_max_power_for_current_bin(const ChannelSpectrum& spectrum, uint8_t bin, uint8_t& max_power) {
2025-11-08T13:22:50.0528828Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0530321Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid get_max_power_for_current_bin(const ChannelSpectrum&, uint8_t, uint8_t&)[m[K':
2025-11-08T13:22:50.0532622Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1452:9:[m[K [01;31m[Kerror: [m[K'[01m[Kmode[m[K' was not declared in this scope; did you mean '[01m[Kmodf[m[K'?
2025-11-08T13:22:50.0533989Z  1452 |     if ([01;31m[Kmode[m[K == LOOKING_GLASS_SINGLEPASS) {
2025-11-08T13:22:50.0534515Z       |         [01;31m[K^~~~[m[K
2025-11-08T13:22:50.0534928Z       |         [32m[Kmodf[m[K
2025-11-08T13:22:50.0548237Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1452:17:[m[K [01;31m[Kerror: [m[K'[01m[KLOOKING_GLASS_SINGLEPASS[m[K' was not declared in this scope
2025-11-08T13:22:50.0549315Z  1452 |     if (mode == [01;31m[KLOOKING_GLASS_SINGLEPASS[m[K) {
2025-11-08T13:22:50.0549762Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0561218Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1454:29:[m[K [01;31m[Kerror: [m[K'[01m[KSPEC_NB_BINS[m[K' was not declared in this scope
2025-11-08T13:22:50.0562766Z  1454 |             if (spectrum.db[[01;31m[KSPEC_NB_BINS[m[K - 120 + bin] > max_power)
2025-11-08T13:22:50.0563860Z       |                             [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0564854Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0567079Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1471:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0568653Z  1471 | void [01;31m[KDroneDisplayController[m[K::add_spectrum_pixel(uint8_t power) {
2025-11-08T13:22:50.0569355Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0586065Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid add_spectrum_pixel(uint8_t)[m[K':
2025-11-08T13:22:50.0587890Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1472:10:[m[K [01;31m[Kerror: [m[K'[01m[Kvalidate_spectrum_data[m[K' was not declared in this scope
2025-11-08T13:22:50.0588863Z  1472 |     if (![01;31m[Kvalidate_spectrum_data[m[K()) {
2025-11-08T13:22:50.0589262Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0603405Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1473:9:[m[K [01;31m[Kerror: [m[K'[01m[Kclear_spectrum_buffers[m[K' was not declared in this scope
2025-11-08T13:22:50.0604757Z  1473 |         [01;31m[Kclear_spectrum_buffers[m[K();
2025-11-08T13:22:50.0605281Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0617431Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1476:9:[m[K [01;31m[Kerror: [m[K'[01m[Kpixel_index[m[K' was not declared in this scope
2025-11-08T13:22:50.0618481Z  1476 |     if ([01;31m[Kpixel_index[m[K < spectrum_row.size()) {
2025-11-08T13:22:50.0618883Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.0625317Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1476:23:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_row[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-08T13:22:50.0626424Z  1476 |     if (pixel_index < [01;31m[Kspectrum_row[m[K.size()) {
2025-11-08T13:22:50.0627012Z       |                       [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0627372Z       |                       [32m[Kspectrum_dir[m[K
2025-11-08T13:22:50.0641364Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1477:29:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-08T13:22:50.0642318Z  1477 |         Color pixel_color = [01;31m[Kspectrum_gradient_[m[K.lut[
2025-11-08T13:22:50.0642733Z       |                             [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0659034Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1480:32:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_count_[m[K' was not declared in this scope
2025-11-08T13:22:50.0660049Z  1480 |         for (size_t i = 0; i < [01;31m[Kthreat_bins_count_[m[K; i++) {
2025-11-08T13:22:50.0660484Z       |                                [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0670784Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1481:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_[m[K' was not declared in this scope
2025-11-08T13:22:50.0680734Z  1481 |             if ([01;31m[Kthreat_bins_[m[K[i].bin == pixel_index) {
2025-11-08T13:22:50.0681188Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0682478Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1482:31:[m[K [01;31m[Kerror: [m[K'[01m[Kget_threat_level_color[m[K' was not declared in this scope; did you mean '[01m[Kget_threat_text_color[m[K'?
2025-11-08T13:22:50.0683982Z  1482 |                 pixel_color = [01;31m[Kget_threat_level_color[m[K(threat_bins_[i].threat);
2025-11-08T13:22:50.0684529Z       |                               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0685061Z       |                               [32m[Kget_threat_text_color[m[K
2025-11-08T13:22:50.0685837Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0687430Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1491:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0688576Z  1491 | void [01;31m[KDroneDisplayController[m[K::render_mini_spectrum() {
2025-11-08T13:22:50.0689069Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0699258Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid render_mini_spectrum()[m[K':
2025-11-08T13:22:50.0701330Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1492:10:[m[K [01;31m[Kerror: [m[K'[01m[Kvalidate_spectrum_data[m[K' was not declared in this scope
2025-11-08T13:22:50.0702656Z  1492 |     if (![01;31m[Kvalidate_spectrum_data[m[K()) {
2025-11-08T13:22:50.0703184Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0719272Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1493:9:[m[K [01;31m[Kerror: [m[K'[01m[Kclear_spectrum_buffers[m[K' was not declared in this scope
2025-11-08T13:22:50.0720288Z  1493 |         [01;31m[Kclear_spectrum_buffers[m[K();
2025-11-08T13:22:50.0720690Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0734569Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1496:36:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-08T13:22:50.0735693Z  1496 |     const Color background_color = [01;31m[Kspectrum_gradient_[m[K.lut.size() > 0 ? spectrum_gradient_.lut[0] : Color::black();
2025-11-08T13:22:50.0736251Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0743879Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1497:15:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_row[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-08T13:22:50.0745598Z  1497 |     std::fill([01;31m[Kspectrum_row[m[K.begin(), spectrum_row.end(), background_color);
2025-11-08T13:22:50.0746297Z       |               [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0746937Z       |               [32m[Kspectrum_dir[m[K
2025-11-08T13:22:50.0757921Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1498:9:[m[K [01;31m[Kerror: [m[K'[01m[Kpixel_index[m[K' was not declared in this scope
2025-11-08T13:22:50.0759203Z  1498 |     if ([01;31m[Kpixel_index[m[K > 0) {
2025-11-08T13:22:50.0759692Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.0760665Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0762487Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1507:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0764436Z  1507 | void [01;31m[KDroneDisplayController[m[K::highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, MAX_DISPLAYED_DRONES>& drones) {
2025-11-08T13:22:50.0765474Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0779749Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid highlight_threat_zones_in_spectrum(const std::array<DisplayDroneEntry, 3>&)[m[K':
2025-11-08T13:22:50.0781123Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1508:5:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_count_[m[K' was not declared in this scope
2025-11-08T13:22:50.0782096Z  1508 |     [01;31m[Kthreat_bins_count_[m[K = 0;
2025-11-08T13:22:50.0782413Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0796868Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1511:28:[m[K [01;31m[Kerror: [m[K'[01m[Kfrequency_to_spectrum_bin[m[K' was not declared in this scope
2025-11-08T13:22:50.0797922Z  1511 |             size_t bin_x = [01;31m[Kfrequency_to_spectrum_bin[m[K(drone.frequency);
2025-11-08T13:22:50.0798380Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0809005Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1513:17:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_bins_[m[K' was not declared in this scope
2025-11-08T13:22:50.0809958Z  1513 |                 [01;31m[Kthreat_bins_[m[K[threat_bins_count_].bin = bin_x;
2025-11-08T13:22:50.0810361Z       |                 [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0811017Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0812211Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1521:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0813203Z  1521 | void [01;31m[KDroneDisplayController[m[K::clear_spectrum_buffers() {
2025-11-08T13:22:50.0813666Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0826965Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid clear_spectrum_buffers()[m[K':
2025-11-08T13:22:50.0828581Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1522:15:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_power_levels_[m[K' was not declared in this scope
2025-11-08T13:22:50.0829811Z  1522 |     std::fill([01;31m[Kspectrum_power_levels_[m[K.begin(), spectrum_power_levels_.end(), 0);
2025-11-08T13:22:50.0830381Z       |               [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0831140Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0832523Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1525:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0833687Z  1525 | bool [01;31m[KDroneDisplayController[m[K::validate_spectrum_data() const {
2025-11-08T13:22:50.0834223Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0835286Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1525:55:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kbool validate_spectrum_data()[m[K' cannot have cv-qualifier
2025-11-08T13:22:50.0836351Z  1525 | bool DroneDisplayController::validate_spectrum_data() [01;31m[Kconst[m[K {
2025-11-08T13:22:50.0836985Z       |                                                       [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.0844348Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool validate_spectrum_data()[m[K':
2025-11-08T13:22:50.0845621Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1526:9:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_power_levels_[m[K' was not declared in this scope
2025-11-08T13:22:50.0846857Z  1526 |     if ([01;31m[Kspectrum_power_levels_[m[K.size() != MINI_SPECTRUM_WIDTH) return false;
2025-11-08T13:22:50.0847284Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0860605Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1527:9:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_gradient_[m[K' was not declared in this scope
2025-11-08T13:22:50.0862035Z  1527 |     if ([01;31m[Kspectrum_gradient_[m[K.lut.empty()) return false;
2025-11-08T13:22:50.0862408Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0862997Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0864062Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1531:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0865049Z  1531 | size_t [01;31m[KDroneDisplayController[m[K::get_safe_spectrum_index(size_t x, size_t y) const {
2025-11-08T13:22:50.0865505Z       |        [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0866492Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1531:76:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Ksize_t get_safe_spectrum_index(size_t, size_t)[m[K' cannot have cv-qualifier
2025-11-08T13:22:50.0867752Z  1531 | size_t DroneDisplayController::get_safe_spectrum_index(size_t x, size_t y) [01;31m[Kconst[m[K {
2025-11-08T13:22:50.0868282Z       |                                                                            [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.0869081Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1538:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0870036Z  1538 | void [01;31m[KDroneDisplayController[m[K::set_spectrum_range(Frequency min_freq, Frequency max_freq) {
2025-11-08T13:22:50.0870510Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0876458Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_spectrum_range(Frequency, Frequency)[m[K':
2025-11-08T13:22:50.0878064Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1540:9:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_config_[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-08T13:22:50.0878979Z  1540 |         [01;31m[Kspectrum_config_[m[K.min_freq = WIDEBAND_DEFAULT_MIN;
2025-11-08T13:22:50.0879349Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0879640Z       |         [32m[Kspectrum_dir[m[K
2025-11-08T13:22:50.0891530Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1544:5:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_config_[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-08T13:22:50.0892740Z  1544 |     [01;31m[Kspectrum_config_[m[K.min_freq = min_freq;
2025-11-08T13:22:50.0893067Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0893339Z       |     [32m[Kspectrum_dir[m[K
2025-11-08T13:22:50.0893932Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0894972Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1551:8:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0895949Z  1551 | size_t [01;31m[KDroneDisplayController[m[K::frequency_to_spectrum_bin(Frequency freq_hz) const {
2025-11-08T13:22:50.0896417Z       |        [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0897802Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1551:77:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Ksize_t frequency_to_spectrum_bin(Frequency)[m[K' cannot have cv-qualifier
2025-11-08T13:22:50.0898868Z  1551 | size_t DroneDisplayController::frequency_to_spectrum_bin(Frequency freq_hz) [01;31m[Kconst[m[K {
2025-11-08T13:22:50.0899509Z       |                                                                             [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.0906938Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Ksize_t frequency_to_spectrum_bin(Frequency)[m[K':
2025-11-08T13:22:50.0908825Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1552:32:[m[K [01;31m[Kerror: [m[K'[01m[Kspectrum_config_[m[K' was not declared in this scope; did you mean '[01m[Kspectrum_dir[m[K'?
2025-11-08T13:22:50.0910116Z  1552 |     const Frequency MIN_FREQ = [01;31m[Kspectrum_config_[m[K.min_freq;
2025-11-08T13:22:50.0910519Z       |                                [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0910831Z       |                                [32m[Kspectrum_dir[m[K
2025-11-08T13:22:50.0911418Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0913340Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1563:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0914920Z  1563 | std::string [01;31m[KDroneDisplayController[m[K::get_threat_level_name(ThreatLevel level) const {
2025-11-08T13:22:50.0915837Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0917836Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1563:78:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_threat_level_name(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-08T13:22:50.0919235Z  1563 | std::string DroneDisplayController::get_threat_level_name(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-08T13:22:50.0919766Z       |                                                                              [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.0920588Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1574:13:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0921490Z  1574 | std::string [01;31m[KDroneDisplayController[m[K::get_drone_type_name(DroneType type) const {
2025-11-08T13:22:50.0921945Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0922962Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1574:73:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kstd::string get_drone_type_name(DroneType)[m[K' cannot have cv-qualifier
2025-11-08T13:22:50.0924001Z  1574 | std::string DroneDisplayController::get_drone_type_name(DroneType type) [01;31m[Kconst[m[K {
2025-11-08T13:22:50.0924509Z       |                                                                         [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.0925306Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:7:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0926216Z  1582 | Color [01;31m[KDroneDisplayController[m[K::get_drone_type_color(DroneType type) const {
2025-11-08T13:22:50.0926821Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0927807Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1582:68:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_drone_type_color(DroneType)[m[K' cannot have cv-qualifier
2025-11-08T13:22:50.0929014Z  1582 | Color DroneDisplayController::get_drone_type_color(DroneType type) [01;31m[Kconst[m[K {
2025-11-08T13:22:50.0929500Z       |                                                                    [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.0930396Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1590:7:[m[K [01;31m[Kerror: [m[K'[01m[KDroneDisplayController[m[K' has not been declared
2025-11-08T13:22:50.0931317Z  1590 | Color [01;31m[KDroneDisplayController[m[K::get_threat_level_color(ThreatLevel level) const {
2025-11-08T13:22:50.0931751Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0932767Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1590:73:[m[K [01;31m[Kerror: [m[Knon-member function '[01m[Kui::Color get_threat_level_color(ThreatLevel)[m[K' cannot have cv-qualifier
2025-11-08T13:22:50.0934724Z  1590 | Color DroneDisplayController::get_threat_level_color(ThreatLevel level) [01;31m[Kconst[m[K {
2025-11-08T13:22:50.0935283Z       |                                                                         [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.0936265Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1601:1:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' does not name a type; did you mean '[01m[KDroneHardwareController[m[K'?
2025-11-08T13:22:50.0937403Z  1601 | [01;31m[KDroneUIController[m[K::DroneUIController(NavigationView& nav,
2025-11-08T13:22:50.0937782Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0938068Z       | [32m[KDroneHardwareController[m[K
2025-11-08T13:22:50.0938833Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1614:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.0939614Z  1614 | void [01;31m[KDroneUIController[m[K::on_start_scan() {
2025-11-08T13:22:50.0939948Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0951699Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_start_scan()[m[K':
2025-11-08T13:22:50.0952899Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1615:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.0953672Z  1615 |     if ([01;31m[Kscanning_active_[m[K) return;
2025-11-08T13:22:50.0953986Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0968197Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1616:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.0968975Z  1616 |     [01;31m[Kscanning_active_[m[K = true;
2025-11-08T13:22:50.0969291Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0976395Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1617:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.0977382Z  1617 |     [01;31m[Kscanner_[m[K.start_scanning();
2025-11-08T13:22:50.0977699Z       |     [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.0994823Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1618:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.0995957Z  1618 |     [01;31m[Kdisplay_controller_[m[K->set_scanning_status(true, "Scanning Active");
2025-11-08T13:22:50.0996468Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.0997361Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.0998880Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1622:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.0999881Z  1622 | void [01;31m[KDroneUIController[m[K::on_stop_scan() {
2025-11-08T13:22:50.1000301Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1010678Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_stop_scan()[m[K':
2025-11-08T13:22:50.1011849Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1623:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.1012571Z  1623 |     [01;31m[Kscanning_active_[m[K = false;
2025-11-08T13:22:50.1012873Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1018632Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1624:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.1019441Z  1624 |     [01;31m[Kscanner_[m[K.stop_scanning();
2025-11-08T13:22:50.1019731Z       |     [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.1023845Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1625:5:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_[m[K' was not declared in this scope; did you mean '[01m[Kaudio[m[K'?
2025-11-08T13:22:50.1024698Z  1625 |     [01;31m[Kaudio_[m[K.stop_audio();
2025-11-08T13:22:50.1024979Z       |     [01;31m[K^~~~~~[m[K
2025-11-08T13:22:50.1025234Z       |     [32m[Kaudio[m[K
2025-11-08T13:22:50.1041275Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1626:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.1042162Z  1626 |     [01;31m[Kdisplay_controller_[m[K->set_scanning_status(false, "Stopped");
2025-11-08T13:22:50.1042567Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1043134Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1044174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1630:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1044991Z  1630 | void [01;31m[KDroneUIController[m[K::on_toggle_mode() {
2025-11-08T13:22:50.1045331Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1049763Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_toggle_mode()[m[K':
2025-11-08T13:22:50.1050922Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1631:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.1051665Z  1631 |     if ([01;31m[Kscanner_[m[K.is_real_mode()) {
2025-11-08T13:22:50.1052095Z       |         [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.1069472Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1633:13:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1070397Z  1633 |         if ([01;31m[Khardware_[m[K.is_spectrum_streaming_active()) {
2025-11-08T13:22:50.1070763Z       |             [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1071547Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1638:14:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1072323Z  1638 |         if (![01;31m[Khardware_[m[K.is_spectrum_streaming_active()) {
2025-11-08T13:22:50.1072686Z       |              [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1086358Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1642:5:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.1087346Z  1642 |     [01;31m[Kdisplay_controller_[m[K->set_scanning_status(scanning_active_,
2025-11-08T13:22:50.1087744Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1102344Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1642:46:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.1103263Z  1642 |     display_controller_->set_scanning_status([01;31m[Kscanning_active_[m[K,
2025-11-08T13:22:50.1103701Z       |                                              [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1110501Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1643:44:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.1111377Z  1643 |                                            [01;31m[Kscanner_[m[K.is_real_mode() ? "Real Mode" : "Demo Mode");
2025-11-08T13:22:50.1111774Z       |                                            [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.1112378Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1113400Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1646:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1114541Z  1646 | void [01;31m[KDroneUIController[m[K::show_menu() {
2025-11-08T13:22:50.1115202Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1117452Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-08T13:22:50.1118590Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:22:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1119457Z  1647 |     auto menu_view = [01;31m[Knav_[m[K.push<MenuView>(std::vector<MenuView::Item>{
2025-11-08T13:22:50.1119849Z       |                      [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1120662Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:40:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-08T13:22:50.1121571Z  1647 |     auto menu_view = nav_.push<MenuView[01;31m[K>[m[K(std::vector<MenuView::Item>{
2025-11-08T13:22:50.1121992Z       |                                        [01;31m[K^[m[K
2025-11-08T13:22:50.1122865Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:64:[m[K [01;31m[Kerror: [m[K'[01m[KItem[m[K' is not a member of '[01m[Kui::MenuView[m[K'
2025-11-08T13:22:50.1123726Z  1647 |     auto menu_view = nav_.push<MenuView>(std::vector<MenuView::[01;31m[KItem[m[K>{
2025-11-08T13:22:50.1124160Z       |                                                                [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1124876Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:68:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1125655Z  1647 |     auto menu_view = nav_.push<MenuView>(std::vector<MenuView::Item[01;31m[K>[m[K{
2025-11-08T13:22:50.1126083Z       |                                                                    [01;31m[K^[m[K
2025-11-08T13:22:50.1127017Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1647:68:[m[K [01;31m[Kerror: [m[Ktemplate argument 2 is invalid
2025-11-08T13:22:50.1128144Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1648:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1129132Z  1648 |         {"Load Database", [[01;31m[Kthis[m[K]() { on_load_frequency_file(); }},
2025-11-08T13:22:50.1129511Z       |                            [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1137634Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1139225Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1648:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_load_frequency_file[m[K' was not declared in this scope; did you mean '[01m[Kload_freqman_file[m[K'?
2025-11-08T13:22:50.1140204Z  1648 |         {"Load Database", [this]() { [01;31m[Kon_load_frequency_file[m[K(); }},
2025-11-08T13:22:50.1140610Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1140950Z       |                                      [32m[Kload_freqman_file[m[K
2025-11-08T13:22:50.1141640Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-08T13:22:50.1142749Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1649:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1143542Z  1649 |         {"Save Settings", [[01;31m[Kthis[m[K]() { on_save_settings(); }},
2025-11-08T13:22:50.1143910Z       |                            [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1151657Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1153768Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1649:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_save_settings[m[K' was not declared in this scope; did you mean '[01m[Ksave_settings[m[K'?
2025-11-08T13:22:50.1155344Z  1649 |         {"Save Settings", [this]() { [01;31m[Kon_save_settings[m[K(); }},
2025-11-08T13:22:50.1156003Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1156536Z       |                                      [32m[Ksave_settings[m[K
2025-11-08T13:22:50.1157899Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-08T13:22:50.1159821Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1650:29:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1161204Z  1650 |         {"Audio Settings", [[01;31m[Kthis[m[K]() { on_audio_settings(); }},
2025-11-08T13:22:50.1161840Z       |                             [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1177365Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1179485Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1650:39:[m[K [01;31m[Kerror: [m[K'[01m[Kon_audio_settings[m[K' was not declared in this scope; did you mean '[01m[Kload_settings[m[K'?
2025-11-08T13:22:50.1181069Z  1650 |         {"Audio Settings", [this]() { [01;31m[Kon_audio_settings[m[K(); }},
2025-11-08T13:22:50.1181744Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1182289Z       |                                       [32m[Kload_settings[m[K
2025-11-08T13:22:50.1183469Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-08T13:22:50.1185383Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1651:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1187176Z  1651 |         {"Spectrum Mode", [[01;31m[Kthis[m[K]() { on_spectrum_mode(); }},
2025-11-08T13:22:50.1187799Z       |                            [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1203338Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1205659Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1651:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[KSpectrumMode[m[K'?
2025-11-08T13:22:50.1207489Z  1651 |         {"Spectrum Mode", [this]() { [01;31m[Kon_spectrum_mode[m[K(); }},
2025-11-08T13:22:50.1208148Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1208687Z       |                                      [32m[KSpectrumMode[m[K
2025-11-08T13:22:50.1209882Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-08T13:22:50.1211797Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1652:31:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1213196Z  1652 |         {"Hardware Control", [[01;31m[Kthis[m[K]() { on_hardware_control(); }},
2025-11-08T13:22:50.1213855Z       |                               [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1233819Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1235724Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1652:41:[m[K [01;31m[Kerror: [m[K'[01m[Kon_hardware_control[m[K' was not declared in this scope
2025-11-08T13:22:50.1237444Z  1652 |         {"Hardware Control", [this]() { [01;31m[Kon_hardware_control[m[K(); }},
2025-11-08T13:22:50.1238155Z       |                                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1239351Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-08T13:22:50.1241280Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1653:24:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1242611Z  1653 |         {"View Logs", [[01;31m[Kthis[m[K]() { on_view_logs(); }},
2025-11-08T13:22:50.1243214Z       |                        [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1257370Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1258903Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1653:34:[m[K [01;31m[Kerror: [m[K'[01m[Kon_view_logs[m[K' was not declared in this scope
2025-11-08T13:22:50.1259890Z  1653 |         {"View Logs", [this]() { [01;31m[Kon_view_logs[m[K(); }},
2025-11-08T13:22:50.1260340Z       |                                  [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1261206Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_menu()[m[K':
2025-11-08T13:22:50.1262601Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1654:20:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1263535Z  1654 |         {"About", [[01;31m[Kthis[m[K]() { on_about(); }}
2025-11-08T13:22:50.1263933Z       |                    [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1269157Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1270681Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1654:30:[m[K [01;31m[Kerror: [m[K'[01m[Kon_about[m[K' was not declared in this scope
2025-11-08T13:22:50.1271436Z  1654 |         {"About", [this]() { [01;31m[Kon_about[m[K(); }}
2025-11-08T13:22:50.1271777Z       |                              [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.1272482Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1273494Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1658:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1274318Z  1658 | void [01;31m[KDroneUIController[m[K::on_load_frequency_file() {
2025-11-08T13:22:50.1274675Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1278395Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_load_frequency_file()[m[K':
2025-11-08T13:22:50.1279545Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1659:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.1280309Z  1659 |     if ([01;31m[Kscanner_[m[K.load_frequency_database()) {
2025-11-08T13:22:50.1280633Z       |         [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.1283267Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1661:9:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1284246Z  1661 |         [01;31m[Knav_[m[K.display_modal("Success", "Loaded " + std::to_string(count) + " frequencies");
2025-11-08T13:22:50.1284681Z       |         [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1290672Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1663:9:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1291493Z  1663 |         [01;31m[Knav_[m[K.display_modal("Error", "Failed to load database");
2025-11-08T13:22:50.1291847Z       |         [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1292416Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1293444Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1667:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1294240Z  1667 | void [01;31m[KDroneUIController[m[K::on_save_settings() {
2025-11-08T13:22:50.1294577Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1298223Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_save_settings()[m[K':
2025-11-08T13:22:50.1299512Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1668:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksettings_[m[K' was not declared in this scope; did you mean '[01m[Ksettings_dir[m[K'?
2025-11-08T13:22:50.1300423Z  1668 |     [01;31m[Ksettings_[m[K.save(config::SETTINGS_FILE_PATH);
2025-11-08T13:22:50.1300765Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1301022Z       |     [32m[Ksettings_dir[m[K
2025-11-08T13:22:50.1302176Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1668:20:[m[K [01;31m[Kerror: [m[K'[01m[Kconfig[m[K' has not been declared
2025-11-08T13:22:50.1303604Z  1668 |     settings_.save([01;31m[Kconfig[m[K::SETTINGS_FILE_PATH);
2025-11-08T13:22:50.1304189Z       |                    [01;31m[K^~~~~~[m[K
2025-11-08T13:22:50.1304972Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1669:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1305934Z  1669 |     [01;31m[Knav_[m[K.display_modal("Success", "Settings saved");
2025-11-08T13:22:50.1306265Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1307190Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1309137Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1672:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1309990Z  1672 | void [01;31m[KDroneUIController[m[K::on_audio_settings() {
2025-11-08T13:22:50.1310334Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1311021Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_audio_settings()[m[K':
2025-11-08T13:22:50.1312107Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1673:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1312954Z  1673 |     [01;31m[Knav_[m[K.push<CheckboxView>("Enable Audio Alerts", &settings_.enable_audio_alerts);
2025-11-08T13:22:50.1313376Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1322114Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1673:15:[m[K [01;31m[Kerror: [m[K'[01m[KCheckboxView[m[K' was not declared in this scope
2025-11-08T13:22:50.1323036Z  1673 |     nav_.push<[01;31m[KCheckboxView[m[K>("Enable Audio Alerts", &settings_.enable_audio_alerts);
2025-11-08T13:22:50.1323490Z       |               [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1329589Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1673:53:[m[K [01;31m[Kerror: [m[K'[01m[Ksettings_[m[K' was not declared in this scope; did you mean '[01m[Ksettings_dir[m[K'?
2025-11-08T13:22:50.1330639Z  1673 |     nav_.push<CheckboxView>("Enable Audio Alerts", &[01;31m[Ksettings_[m[K.enable_audio_alerts);
2025-11-08T13:22:50.1331126Z       |                                                     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1331456Z       |                                                     [32m[Ksettings_dir[m[K
2025-11-08T13:22:50.1332056Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1333844Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1676:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1335301Z  1676 | void [01;31m[KDroneUIController[m[K::on_spectrum_mode() {
2025-11-08T13:22:50.1335905Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1336877Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-08T13:22:50.1337977Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1338776Z  1677 |     [01;31m[Knav_[m[K.push<MenuView>(std::vector<MenuView::Item>{
2025-11-08T13:22:50.1339118Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1339902Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:23:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-08T13:22:50.1340740Z  1677 |     nav_.push<MenuView[01;31m[K>[m[K(std::vector<MenuView::Item>{
2025-11-08T13:22:50.1341097Z       |                       [01;31m[K^[m[K
2025-11-08T13:22:50.1341995Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:47:[m[K [01;31m[Kerror: [m[K'[01m[KItem[m[K' is not a member of '[01m[Kui::MenuView[m[K'
2025-11-08T13:22:50.1342958Z  1677 |     nav_.push<MenuView>(std::vector<MenuView::[01;31m[KItem[m[K>{
2025-11-08T13:22:50.1343337Z       |                                               [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1344137Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1344867Z  1677 |     nav_.push<MenuView>(std::vector<MenuView::Item[01;31m[K>[m[K{
2025-11-08T13:22:50.1345363Z       |                                                   [01;31m[K^[m[K
2025-11-08T13:22:50.1346763Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1677:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 2 is invalid
2025-11-08T13:22:50.1348811Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1678:34:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1349927Z  1678 |         {"Ultra Narrow (4MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::ULTRA_NARROW); }},
2025-11-08T13:22:50.1350363Z       |                                  [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1350958Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1352051Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1678:76:[m[K [01;31m[Kerror: [m[K'[01m[KULTRA_NARROW[m[K' is not a member of '[01m[KSpectrumMode[m[K'
2025-11-08T13:22:50.1352977Z  1678 |         {"Ultra Narrow (4MHz)", [this]() { set_spectrum_mode(SpectrumMode::[01;31m[KULTRA_NARROW[m[K); }},
2025-11-08T13:22:50.1353474Z       |                                                                            [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1354428Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1678:44:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-08T13:22:50.1355468Z  1678 |         {"Ultra Narrow (4MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::ULTRA_NARROW); }},
2025-11-08T13:22:50.1355915Z       |                                            [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1356242Z       |                                            [32m[Kon_spectrum_mode[m[K
2025-11-08T13:22:50.1357080Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-08T13:22:50.1358180Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1679:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1359825Z  1679 |         {"Narrow (8MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::NARROW); }},
2025-11-08T13:22:50.1360564Z       |                            [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1361586Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1362829Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1679:38:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-08T13:22:50.1363840Z  1679 |         {"Narrow (8MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::NARROW); }},
2025-11-08T13:22:50.1364249Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1364579Z       |                                      [32m[Kon_spectrum_mode[m[K
2025-11-08T13:22:50.1365440Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-08T13:22:50.1366544Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1680:29:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1367676Z  1680 |         {"Medium (12MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::MEDIUM); }},
2025-11-08T13:22:50.1368082Z       |                             [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1373352Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1374971Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1680:39:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-08T13:22:50.1376066Z  1680 |         {"Medium (12MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::MEDIUM); }},
2025-11-08T13:22:50.1376520Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1377032Z       |                                       [32m[Kon_spectrum_mode[m[K
2025-11-08T13:22:50.1377751Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-08T13:22:50.1378894Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1681:27:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1379776Z  1681 |         {"Wide (20MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::WIDE); }},
2025-11-08T13:22:50.1380178Z       |                           [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1387726Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1388970Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1681:37:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-08T13:22:50.1389993Z  1681 |         {"Wide (20MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::WIDE); }},
2025-11-08T13:22:50.1390406Z       |                                     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1390727Z       |                                     [32m[Kon_spectrum_mode[m[K
2025-11-08T13:22:50.1391417Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_spectrum_mode()[m[K':
2025-11-08T13:22:50.1392526Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1682:33:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1392862Z  1682 |         {"Ultra Wide (24MHz)", [[01;31m[Kthis[m[K]() { set_spectrum_mode(SpectrumMode::ULTRA_WIDE); }}
2025-11-08T13:22:50.1393000Z       |                                 [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1400611Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1401843Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1682:43:[m[K [01;31m[Kerror: [m[K'[01m[Kset_spectrum_mode[m[K' was not declared in this scope; did you mean '[01m[Kon_spectrum_mode[m[K'?
2025-11-08T13:22:50.1402220Z  1682 |         {"Ultra Wide (24MHz)", [this]() { [01;31m[Kset_spectrum_mode[m[K(SpectrumMode::ULTRA_WIDE); }}
2025-11-08T13:22:50.1402378Z       |                                           [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1402726Z       |                                           [32m[Kon_spectrum_mode[m[K
2025-11-08T13:22:50.1403147Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1403865Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1686:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1404161Z  1686 | void [01;31m[KDroneUIController[m[K::set_spectrum_mode(SpectrumMode mode) {
2025-11-08T13:22:50.1404280Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1411172Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid set_spectrum_mode(SpectrumMode)[m[K':
2025-11-08T13:22:50.1412279Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1687:5:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1412606Z  1687 |     [01;31m[Khardware_[m[K.set_spectrum_mode(mode);
2025-11-08T13:22:50.1412800Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1417691Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1688:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1418119Z  1688 |     [01;31m[Knav_[m[K.display_modal("Applied", "Spectrum mode updated");
2025-11-08T13:22:50.1418310Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1419042Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1420128Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1691:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1420587Z  1691 | void [01;31m[KDroneUIController[m[K::on_hardware_control() {
2025-11-08T13:22:50.1420792Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1424650Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hardware_control()[m[K':
2025-11-08T13:22:50.1425735Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1426158Z  1692 |     [01;31m[Knav_[m[K.push<MenuView>(std::vector<MenuView::Item>{
2025-11-08T13:22:50.1426329Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1427621Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:23:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-08T13:22:50.1428045Z  1692 |     nav_.push<MenuView[01;31m[K>[m[K(std::vector<MenuView::Item>{
2025-11-08T13:22:50.1428250Z       |                       [01;31m[K^[m[K
2025-11-08T13:22:50.1429488Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:47:[m[K [01;31m[Kerror: [m[K'[01m[KItem[m[K' is not a member of '[01m[Kui::MenuView[m[K'
2025-11-08T13:22:50.1429852Z  1692 |     nav_.push<MenuView>(std::vector<MenuView::[01;31m[KItem[m[K>{
2025-11-08T13:22:50.1430103Z       |                                               [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1431049Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1431414Z  1692 |     nav_.push<MenuView>(std::vector<MenuView::Item[01;31m[K>[m[K{
2025-11-08T13:22:50.1431656Z       |                                                   [01;31m[K^[m[K
2025-11-08T13:22:50.1432590Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1692:51:[m[K [01;31m[Kerror: [m[Ktemplate argument 2 is invalid
2025-11-08T13:22:50.1433898Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1693:28:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1434405Z  1693 |         {"Set Bandwidth", [[01;31m[Kthis[m[K]() { on_set_bandwidth(); }},
2025-11-08T13:22:50.1434632Z       |                            [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1454435Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1455568Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1693:38:[m[K [01;31m[Kerror: [m[K'[01m[Kon_set_bandwidth[m[K' was not declared in this scope
2025-11-08T13:22:50.1455953Z  1693 |         {"Set Bandwidth", [this]() { [01;31m[Kon_set_bandwidth[m[K(); }},
2025-11-08T13:22:50.1456213Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1457326Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hardware_control()[m[K':
2025-11-08T13:22:50.1458418Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1694:30:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1458800Z  1694 |         {"Set Center Freq", [[01;31m[Kthis[m[K]() { on_set_center_freq(); }},
2025-11-08T13:22:50.1459017Z       |                              [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1480198Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1481314Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1694:40:[m[K [01;31m[Kerror: [m[K'[01m[Kon_set_center_freq[m[K' was not declared in this scope
2025-11-08T13:22:50.1481721Z  1694 |         {"Set Center Freq", [this]() { [01;31m[Kon_set_center_freq[m[K(); }},
2025-11-08T13:22:50.1481977Z       |                                        [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1482913Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hardware_control()[m[K':
2025-11-08T13:22:50.1483988Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1695:29:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1484373Z  1695 |         {"Current Status", [[01;31m[Kthis[m[K]() { show_hardware_status(); }}
2025-11-08T13:22:50.1484588Z       |                             [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1507488Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1508676Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1695:39:[m[K [01;31m[Kerror: [m[K'[01m[Kshow_hardware_status[m[K' was not declared in this scope
2025-11-08T13:22:50.1509083Z  1695 |         {"Current Status", [this]() { [01;31m[Kshow_hardware_status[m[K(); }}
2025-11-08T13:22:50.1509352Z       |                                       [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1510064Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1511142Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1699:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1511553Z  1699 | void [01;31m[KDroneUIController[m[K::on_set_bandwidth() {
2025-11-08T13:22:50.1511983Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1520126Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_set_bandwidth()[m[K':
2025-11-08T13:22:50.1521235Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1700:27:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1521845Z  1700 |     uint32_t current_bw = [01;31m[Khardware_[m[K.get_bandwidth();
2025-11-08T13:22:50.1522093Z       |                           [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1526304Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:18:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1526962Z  1701 |     auto& view = [01;31m[Knav_[m[K.push<NumberInputView>(current_bw, 1000000, 24000000);
2025-11-08T13:22:50.1527167Z       |                  [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1544316Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:28:[m[K [01;31m[Kerror: [m[K'[01m[KNumberInputView[m[K' was not declared in this scope
2025-11-08T13:22:50.1544839Z  1701 |     auto& view = nav_.push<[01;31m[KNumberInputView[m[K>(current_bw, 1000000, 24000000);
2025-11-08T13:22:50.1545118Z       |                            [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1545870Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:45:[m[K [01;35m[Kwarning: [m[Kleft operand of comma operator has no effect [[01;35m[K-Wunused-value[m[K]
2025-11-08T13:22:50.1546176Z  1701 |     auto& view = nav_.push<NumberInputView>([01;35m[Kcurrent_bw[m[K, 1000000, 24000000);
2025-11-08T13:22:50.1546332Z       |                                             [01;35m[K^~~~~~~~~~[m[K
2025-11-08T13:22:50.1547205Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1701:66:[m[K [01;35m[Kwarning: [m[Kright operand of comma operator has no effect [[01;35m[K-Wunused-value[m[K]
2025-11-08T13:22:50.1547513Z  1701 |     auto& view = nav_.push<NumberInputView>(current_bw, 1000000, [01;35m[K24000000[m[K);
2025-11-08T13:22:50.1547683Z       |                                                                  [01;35m[K^~~~~~~~[m[K
2025-11-08T13:22:50.1548329Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1702:24:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1548519Z  1702 |     view.on_changed = [[01;31m[Kthis[m[K](uint32_t bw) {
2025-11-08T13:22:50.1548648Z       |                        [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1549076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1549652Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1703:9:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' is not captured
2025-11-08T13:22:50.1549811Z  1703 |         [01;31m[Khardware_[m[K.set_bandwidth(bw);
2025-11-08T13:22:50.1549926Z       |         [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1550488Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1702:28:[m[K [01;36m[Knote: [m[Kthe lambda has no capture-default
2025-11-08T13:22:50.1550668Z  1702 |     view.on_changed = [this[01;36m[K][m[K(uint32_t bw) {
2025-11-08T13:22:50.1550797Z       |                            [01;36m[K^[m[K
2025-11-08T13:22:50.1551399Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1700:27:[m[K [01;36m[Knote: [m[K'[01m[K<typeprefixerror>hardware_[m[K' declared here
2025-11-08T13:22:50.1551612Z  1700 |     uint32_t current_bw = [01;36m[Khardware_[m[K.get_bandwidth();
2025-11-08T13:22:50.1551939Z       |                           [01;36m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1552636Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1553771Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1707:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1554439Z  1707 | void [01;31m[KDroneUIController[m[K::on_set_center_freq() {
2025-11-08T13:22:50.1554654Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1555315Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_set_center_freq()[m[K':
2025-11-08T13:22:50.1555942Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1708:28:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1556191Z  1708 |     Frequency current_cf = [01;31m[Khardware_[m[K.get_center_frequency();
2025-11-08T13:22:50.1556324Z       |                            [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1559998Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1709:18:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1560422Z  1709 |     auto& view = [01;31m[Knav_[m[K.push<FrequencyInputView>(current_cf);
2025-11-08T13:22:50.1560625Z       |                  [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1578349Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1709:28:[m[K [01;31m[Kerror: [m[K'[01m[KFrequencyInputView[m[K' was not declared in this scope
2025-11-08T13:22:50.1579175Z  1709 |     auto& view = nav_.push<[01;31m[KFrequencyInputView[m[K>(current_cf);
2025-11-08T13:22:50.1579430Z       |                            [01;31m[K^~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1580566Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1711:24:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.1580906Z  1711 |     view.on_changed = [[01;31m[Kthis[m[K](Frequency freq) {
2025-11-08T13:22:50.1581115Z       |                        [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1581864Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In lambda function:
2025-11-08T13:22:50.1582868Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1712:9:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' is not captured
2025-11-08T13:22:50.1583175Z  1712 |         [01;31m[Khardware_[m[K.set_center_frequency(freq);
2025-11-08T13:22:50.1583366Z       |         [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1584339Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1711:28:[m[K [01;36m[Knote: [m[Kthe lambda has no capture-default
2025-11-08T13:22:50.1584686Z  1711 |     view.on_changed = [this[01;36m[K][m[K(Frequency freq) {
2025-11-08T13:22:50.1584901Z       |                            [01;36m[K^[m[K
2025-11-08T13:22:50.1585971Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1708:28:[m[K [01;36m[Knote: [m[K'[01m[K<typeprefixerror>hardware_[m[K' declared here
2025-11-08T13:22:50.1586404Z  1708 |     Frequency current_cf = [01;36m[Khardware_[m[K.get_center_frequency();
2025-11-08T13:22:50.1586841Z       |                            [01;36m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1587611Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1588714Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1716:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1589441Z  1716 | void [01;31m[KDroneUIController[m[K::show_hardware_status() {
2025-11-08T13:22:50.1589644Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1593792Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_hardware_status()[m[K':
2025-11-08T13:22:50.1594639Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1720:13:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1594846Z  1720 |             [01;31m[Khardware_[m[K.get_bandwidth() / 1000000,
2025-11-08T13:22:50.1594970Z       |             [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1599359Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1722:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1599611Z  1722 |     [01;31m[Knav_[m[K.display_modal("Hardware Status", buffer);
2025-11-08T13:22:50.1599719Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1600144Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1600769Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1725:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1601005Z  1725 | void [01;31m[KDroneUIController[m[K::on_view_logs() {
2025-11-08T13:22:50.1601122Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1604767Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_view_logs()[m[K':
2025-11-08T13:22:50.1605369Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1726:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1605600Z  1726 |     [01;31m[Knav_[m[K.push<FileBrowserView>("/LOGS/EDA", ".CSV");
2025-11-08T13:22:50.1605709Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1619644Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1726:15:[m[K [01;31m[Kerror: [m[K'[01m[KFileBrowserView[m[K' was not declared in this scope
2025-11-08T13:22:50.1619887Z  1726 |     nav_.push<[01;31m[KFileBrowserView[m[K>("/LOGS/EDA", ".CSV");
2025-11-08T13:22:50.1620024Z       |               [01;31m[K^~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1620736Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1726:32:[m[K [01;35m[Kwarning: [m[Kleft operand of comma operator has no effect [[01;35m[K-Wunused-value[m[K]
2025-11-08T13:22:50.1620963Z  1726 |     nav_.push<FileBrowserView>([01;35m[K"/LOGS/EDA"[m[K, ".CSV");
2025-11-08T13:22:50.1621114Z       |                                [01;35m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1621533Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1622150Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1729:6:[m[K [01;31m[Kerror: [m[K'[01m[KDroneUIController[m[K' has not been declared
2025-11-08T13:22:50.1622381Z  1729 | void [01;31m[KDroneUIController[m[K::on_about() {
2025-11-08T13:22:50.1622500Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1625037Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_about()[m[K':
2025-11-08T13:22:50.1625660Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1730:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1626306Z  1730 |     [01;31m[Knav_[m[K.display_modal("EDA v1.0", "Enhanced Drone Analyzer\nMayhem Firmware Integration\nBased on Recon & Looking Glass");
2025-11-08T13:22:50.1626415Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1637576Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1638455Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1733:1:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' does not name a type
2025-11-08T13:22:50.1638910Z  1733 | [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::EnhancedDroneSpectrumAnalyzerView(NavigationView& nav)
2025-11-08T13:22:50.1639054Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1639674Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1742:68:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:50.1639990Z  1742 |       button_start_({screen_width - 80, screen_height - 48, 72, 24}[01;31m[K,[m[K "START/STOP"),
2025-11-08T13:22:50.1640158Z       |                                                                    [01;31m[K^[m[K
2025-11-08T13:22:50.1640762Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1742:70:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:50.1641061Z  1742 |       button_start_({screen_width - 80, screen_height - 48, 72, 24}, [01;31m[K"START/STOP"[m[K),
2025-11-08T13:22:50.1641236Z       |                                                                      [01;31m[K^~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1641843Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1743:67:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:50.1642117Z  1743 |       button_menu_({screen_width - 80, screen_height - 24, 72, 24}[01;31m[K,[m[K "MENU"),
2025-11-08T13:22:50.1642277Z       |                                                                   [01;31m[K^[m[K
2025-11-08T13:22:50.1642857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1743:69:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:50.1643126Z  1743 |       button_menu_({screen_width - 80, screen_height - 24, 72, 24}, [01;31m[K"MENU"[m[K),
2025-11-08T13:22:50.1643293Z       |                                                                     [01;31m[K^~~~~~[m[K
2025-11-08T13:22:50.1643897Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1744:74:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:50.1644182Z  1744 |       field_scanning_mode_({0, screen_height - 72, screen_width - 80, 24}[01;31m[K,[m[K 3),
2025-11-08T13:22:50.1644355Z       |                                                                          [01;31m[K^[m[K
2025-11-08T13:22:50.1644941Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1744:76:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before numeric constant
2025-11-08T13:22:50.1645227Z  1744 |       field_scanning_mode_({0, screen_height - 72, screen_width - 80, 24}, [01;31m[K3[m[K),
2025-11-08T13:22:50.1645395Z       |                                                                            [01;31m[K^[m[K
2025-11-08T13:22:50.1646059Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1783:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1646312Z  1783 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::focus() {
2025-11-08T13:22:50.1646458Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1652331Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid focus()[m[K':
2025-11-08T13:22:50.1653463Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1784:5:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-08T13:22:50.1653922Z  1784 |     [01;31m[Kbutton_start_[m[K.focus();
2025-11-08T13:22:50.1654141Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1654812Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1655515Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1787:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1655869Z  1787 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::paint(Painter& painter) {
2025-11-08T13:22:50.1656018Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1656907Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1787:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-08T13:22:50.1657622Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-08T13:22:50.1657875Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-08T13:22:50.1657993Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1658601Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-08T13:22:50.1659431Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1788:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-08T13:22:50.1659583Z  1788 |     View::paint(painter[01;31m[K)[m[K;
2025-11-08T13:22:50.1659700Z       |                        [01;31m[K^[m[K
2025-11-08T13:22:50.1660120Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1660780Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1791:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1661104Z  1791 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_key(const KeyEvent key) {
2025-11-08T13:22:50.1661249Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1671363Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool on_key(ui::KeyEvent)[m[K':
2025-11-08T13:22:50.1672049Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1794:13:[m[K [01;31m[Kerror: [m[K'[01m[Kstop_scanning_thread[m[K' was not declared in this scope
2025-11-08T13:22:50.1672214Z  1794 |             [01;31m[Kstop_scanning_thread[m[K();
2025-11-08T13:22:50.1672349Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1676568Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1795:13:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.1677008Z  1795 |             [01;31m[Knav_[m[K.pop();
2025-11-08T13:22:50.1677204Z       |             [01;31m[K^~~~[m[K
2025-11-08T13:22:50.1678623Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1800:28:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual bool ui::Widget::on_key(ui::KeyEvent)[m[K' without object
2025-11-08T13:22:50.1679133Z  1800 |     return View::on_key(key[01;31m[K)[m[K;
2025-11-08T13:22:50.1679363Z       |                            [01;31m[K^[m[K
2025-11-08T13:22:50.1680076Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1680904Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1803:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1681276Z  1803 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_touch(const TouchEvent event) {
2025-11-08T13:22:50.1681416Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1682019Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool on_touch(ui::TouchEvent)[m[K':
2025-11-08T13:22:50.1682857Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1804:32:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual bool ui::Widget::on_touch(ui::TouchEvent)[m[K' without object
2025-11-08T13:22:50.1683021Z  1804 |     return View::on_touch(event[01;31m[K)[m[K;
2025-11-08T13:22:50.1683157Z       |                                [01;31m[K^[m[K
2025-11-08T13:22:50.1683575Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1684242Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1807:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1684505Z  1807 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_show() {
2025-11-08T13:22:50.1684641Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1685143Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_show()[m[K':
2025-11-08T13:22:50.1685955Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1808:19:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::Widget::on_show()[m[K' without object
2025-11-08T13:22:50.1686090Z  1808 |     View::on_show([01;31m[K)[m[K;
2025-11-08T13:22:50.1686204Z       |                   [01;31m[K^[m[K
2025-11-08T13:22:50.1689088Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1810:5:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1689270Z  1810 |     [01;31m[Khardware_[m[K->on_hardware_show();
2025-11-08T13:22:50.1689382Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1705422Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1706231Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1813:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1706590Z  1813 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::on_hide() {
2025-11-08T13:22:50.1706984Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1707611Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid on_hide()[m[K':
2025-11-08T13:22:50.1708395Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1814:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstop_scanning_thread[m[K' was not declared in this scope
2025-11-08T13:22:50.1708579Z  1814 |     [01;31m[Kstop_scanning_thread[m[K();
2025-11-08T13:22:50.1708944Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1716038Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1815:5:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.1716222Z  1815 |     [01;31m[Khardware_[m[K->on_hardware_hide();
2025-11-08T13:22:50.1716333Z       |     [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.1717491Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1816:19:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::Widget::on_hide()[m[K' without object
2025-11-08T13:22:50.1717649Z  1816 |     View::on_hide([01;31m[K)[m[K;
2025-11-08T13:22:50.1717763Z       |                   [01;31m[K^[m[K
2025-11-08T13:22:50.1718174Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1718855Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1819:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1719172Z  1819 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::start_scanning_thread() {
2025-11-08T13:22:50.1719312Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1732773Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid start_scanning_thread()[m[K':
2025-11-08T13:22:50.1733442Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1820:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-08T13:22:50.1733686Z  1820 |     if ([01;31m[Kscanning_coordinator_[m[K->is_scanning_active()) return;
2025-11-08T13:22:50.1733812Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1749908Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1821:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-08T13:22:50.1750321Z  1821 |     [01;31m[Kscanning_coordinator_[m[K->start_coordinated_scanning();
2025-11-08T13:22:50.1750449Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1750877Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1751552Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1824:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1751884Z  1824 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::stop_scanning_thread() {
2025-11-08T13:22:50.1752030Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1767056Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid stop_scanning_thread()[m[K':
2025-11-08T13:22:50.1767742Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1825:10:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-08T13:22:50.1767987Z  1825 |     if (![01;31m[Kscanning_coordinator_[m[K->is_scanning_active()) return;
2025-11-08T13:22:50.1768120Z       |          [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1784161Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1826:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-08T13:22:50.1784404Z  1826 |     [01;31m[Kscanning_coordinator_[m[K->stop_coordinated_scanning();
2025-11-08T13:22:50.1784530Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1785197Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1785881Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1829:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1786353Z  1829 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_start_stop_button() {
2025-11-08T13:22:50.1786501Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1800838Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool handle_start_stop_button()[m[K':
2025-11-08T13:22:50.1801665Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1830:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_coordinator_[m[K' was not declared in this scope
2025-11-08T13:22:50.1801903Z  1830 |     if ([01;31m[Kscanning_coordinator_[m[K->is_scanning_active()) {
2025-11-08T13:22:50.1802031Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1815305Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1831:9:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.1815497Z  1831 |         [01;31m[Kui_controller_[m[K->on_stop_scan();
2025-11-08T13:22:50.1815619Z       |         [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1829084Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1832:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-08T13:22:50.1829274Z  1832 |         [01;31m[Kbutton_start_[m[K.set_text("START/STOP");
2025-11-08T13:22:50.1829390Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1843330Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1834:9:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.1843777Z  1834 |         [01;31m[Kui_controller_[m[K->on_start_scan();
2025-11-08T13:22:50.1843982Z       |         [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1857489Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1835:9:[m[K [01;31m[Kerror: [m[K'[01m[Kbutton_start_[m[K' was not declared in this scope
2025-11-08T13:22:50.1857838Z  1835 |         [01;31m[Kbutton_start_[m[K.set_text("STOP");
2025-11-08T13:22:50.1858000Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1858438Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1859152Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1840:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1859489Z  1840 | bool [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_menu_button() {
2025-11-08T13:22:50.1859637Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1872118Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kbool handle_menu_button()[m[K':
2025-11-08T13:22:50.1872775Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1841:5:[m[K [01;31m[Kerror: [m[K'[01m[Kui_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.1872945Z  1841 |     [01;31m[Kui_controller_[m[K->show_menu();
2025-11-08T13:22:50.1873061Z       |     [01;31m[K^~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1873475Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1874344Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1845:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1874696Z  1845 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::initialize_modern_layout() {
2025-11-08T13:22:50.1874837Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1884177Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid initialize_modern_layout()[m[K':
2025-11-08T13:22:50.1885356Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:5:[m[K [01;31m[Kerror: [m[K'[01m[Ksmart_header_[m[K' was not declared in this scope
2025-11-08T13:22:50.1886023Z  1846 |     [01;31m[Ksmart_header_[m[K = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48});
2025-11-08T13:22:50.1886252Z       |     [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1888268Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:38:[m[K [01;31m[Kerror: [m[K'[01m[KSmartThreatHeader[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::SmartThreatHeader[m[K'?
2025-11-08T13:22:50.1888924Z  1846 |     smart_header_ = std::make_unique<[01;31m[KSmartThreatHeader[m[K>(Rect{0, 0, screen_width, 48});
2025-11-08T13:22:50.1889205Z       |                                      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1889752Z       |                                      [32m[Kui::external_app::enhanced_drone_analyzer::SmartThreatHeader[m[K
2025-11-08T13:22:50.1890550Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1891622Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:462:7:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::SmartThreatHeader[m[K' declared here
2025-11-08T13:22:50.1891835Z   462 | class [01;36m[KSmartThreatHeader[m[K : public View {
2025-11-08T13:22:50.1891958Z       |       [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1892851Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmake_unique<<expression error> >(ui::Rect)[m[K'
2025-11-08T13:22:50.1893209Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-08T13:22:50.1893395Z       |                                                                                     [01;31m[K^[m[K
2025-11-08T13:22:50.1893715Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1893920Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1894312Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1894708Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1895100Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1895998Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)[m[K'
2025-11-08T13:22:50.1896164Z   848 |     [01;36m[Kmake_unique[m[K(_Args&&... __args)
2025-11-08T13:22:50.1896278Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1896942Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1897660Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1898006Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-08T13:22:50.1898345Z       |                                                                                     [01;31m[K^[m[K
2025-11-08T13:22:50.1898927Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1899329Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1900009Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1900722Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1901441Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1902889Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)[m[K'
2025-11-08T13:22:50.1903186Z   854 |     [01;36m[Kmake_unique[m[K(size_t __num)
2025-11-08T13:22:50.1903385Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1904266Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1905223Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1905755Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-08T13:22:50.1905942Z       |                                                                                     [01;31m[K^[m[K
2025-11-08T13:22:50.1906256Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1906470Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1906962Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1907340Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1907706Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1908613Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)[m[K' <deleted>
2025-11-08T13:22:50.1908771Z   860 |     [01;36m[Kmake_unique[m[K(_Args&&...) = delete;
2025-11-08T13:22:50.1908879Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1909403Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1909950Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1846:85:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1910292Z  1846 |     smart_header_ = std::make_unique<SmartThreatHeader>(Rect{0, 0, screen_width, 48}[01;31m[K)[m[K;
2025-11-08T13:22:50.1910467Z       |                                                                                     [01;31m[K^[m[K
2025-11-08T13:22:50.1911319Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:5:[m[K [01;31m[Kerror: [m[K'[01m[Kstatus_bar_[m[K' was not declared in this scope
2025-11-08T13:22:50.1911721Z  1847 |     [01;31m[Kstatus_bar_[m[K = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16});
2025-11-08T13:22:50.1911940Z       |     [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1912947Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:36:[m[K [01;31m[Kerror: [m[K'[01m[KConsoleStatusBar[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::ConsoleStatusBar[m[K'?
2025-11-08T13:22:50.1913318Z  1847 |     status_bar_ = std::make_unique<[01;31m[KConsoleStatusBar[m[K>(0, Rect{0, screen_height - 32, screen_width, 16});
2025-11-08T13:22:50.1913469Z       |                                    [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1913743Z       |                                    [32m[Kui::external_app::enhanced_drone_analyzer::ConsoleStatusBar[m[K
2025-11-08T13:22:50.1914172Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1914975Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:529:7:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::ConsoleStatusBar[m[K' declared here
2025-11-08T13:22:50.1915322Z   529 | class [01;36m[KConsoleStatusBar[m[K : public View {
2025-11-08T13:22:50.1915562Z       |       [01;36m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1917218Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmake_unique<<expression error> >(int, ui::Rect)[m[K'
2025-11-08T13:22:50.1917939Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-08T13:22:50.1918292Z       |                                                                                                      [01;31m[K^[m[K
2025-11-08T13:22:50.1918838Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1919203Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1919577Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1919954Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1920575Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1921947Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)[m[K'
2025-11-08T13:22:50.1922110Z   848 |     [01;36m[Kmake_unique[m[K(_Args&&... __args)
2025-11-08T13:22:50.1922219Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1922725Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1923274Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1923656Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-08T13:22:50.1924018Z       |                                                                                                      [01;31m[K^[m[K
2025-11-08T13:22:50.1924328Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1924542Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1925039Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1925417Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1925781Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1926548Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)[m[K'
2025-11-08T13:22:50.1926878Z   854 |     [01;36m[Kmake_unique[m[K(size_t __num)
2025-11-08T13:22:50.1926998Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1927485Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1928033Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1928410Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-08T13:22:50.1928595Z       |                                                                                                      [01;31m[K^[m[K
2025-11-08T13:22:50.1928887Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1929087Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1929451Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1929817Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1930187Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1931046Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)[m[K' <deleted>
2025-11-08T13:22:50.1931203Z   860 |     [01;36m[Kmake_unique[m[K(_Args&&...) = delete;
2025-11-08T13:22:50.1931309Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1931804Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1932343Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1847:102:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1932726Z  1847 |     status_bar_ = std::make_unique<ConsoleStatusBar>(0, Rect{0, screen_height - 32, screen_width, 16}[01;31m[K)[m[K;
2025-11-08T13:22:50.1932917Z       |                                                                                                      [01;31m[K^[m[K
2025-11-08T13:22:50.1933540Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1850:28:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_cards_[m[K' was not declared in this scope
2025-11-08T13:22:50.1933746Z  1850 |     for (size_t i = 0; i < [01;31m[Kthreat_cards_[m[K.size(); ++i) {
2025-11-08T13:22:50.1934009Z       |                            [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1934948Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:45:[m[K [01;31m[Kerror: [m[K'[01m[KThreatCard[m[K' was not declared in this scope; did you mean '[01m[Kui::external_app::enhanced_drone_analyzer::ThreatCard[m[K'?
2025-11-08T13:22:50.1935782Z  1851 |         threat_cards_[i] = std::make_unique<[01;31m[KThreatCard[m[K>(i, Rect{0, card_y_pos, screen_width, 24});
2025-11-08T13:22:50.1936132Z       |                                             [01;31m[K^~~~~~~~~~[m[K
2025-11-08T13:22:50.1936834Z       |                                             [32m[Kui::external_app::enhanced_drone_analyzer::ThreatCard[m[K
2025-11-08T13:22:50.1937599Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1938509Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:497:7:[m[K [01;36m[Knote: [m[K'[01m[Kui::external_app::enhanced_drone_analyzer::ThreatCard[m[K' declared here
2025-11-08T13:22:50.1938685Z   497 | class [01;36m[KThreatCard[m[K : public View {
2025-11-08T13:22:50.1938801Z       |       [01;36m[K^~~~~~~~~~[m[K
2025-11-08T13:22:50.1939708Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:68:[m[K [01;35m[Kwarning: [m[Knarrowing conversion of '[01m[Kcard_y_pos[m[K' from '[01m[Ksize_t[m[K' {aka '[01m[Kunsigned int[m[K'} to '[01m[Kint[m[K' [[01;35m[K-Wnarrowing[m[K]
2025-11-08T13:22:50.1940047Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, [01;35m[Kcard_y_pos[m[K, screen_width, 24});
2025-11-08T13:22:50.1940219Z       |                                                                    [01;35m[K^~~~~~~~~~[m[K
2025-11-08T13:22:50.1941033Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmake_unique<<expression error> >(size_t&, ui::Rect)[m[K'
2025-11-08T13:22:50.1941373Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-08T13:22:50.1941557Z       |                                                                                                 [01;31m[K^[m[K
2025-11-08T13:22:50.1941855Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1942051Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1942413Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1942782Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1943155Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1943985Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__single_object std::make_unique(_Args&& ...)[m[K'
2025-11-08T13:22:50.1944140Z   848 |     [01;36m[Kmake_unique[m[K(_Args&&... __args)
2025-11-08T13:22:50.1944246Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1944738Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:848:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1945271Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1945772Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-08T13:22:50.1945954Z       |                                                                                                 [01;31m[K^[m[K
2025-11-08T13:22:50.1946255Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1946545Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1947055Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1947433Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1947803Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1948606Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> typename std::_MakeUniq<_Tp>::__array std::make_unique(std::size_t)[m[K'
2025-11-08T13:22:50.1948759Z   854 |     [01;36m[Kmake_unique[m[K(size_t __num)
2025-11-08T13:22:50.1948868Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1949366Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:854:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1949902Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1950243Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-08T13:22:50.1950429Z       |                                                                                                 [01;31m[K^[m[K
2025-11-08T13:22:50.1950728Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/memory:80[m[K,
2025-11-08T13:22:50.1950923Z                  from [01m[K/havoc/firmware/application/./file.hpp:35[m[K,
2025-11-08T13:22:50.1951286Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.1951829Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.1952568Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.1954176Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class ... _Args> typename std::_MakeUniq<_Tp>::__invalid_type std::make_unique(_Args&& ...)[m[K' <deleted>
2025-11-08T13:22:50.1954438Z   860 |     [01;36m[Kmake_unique[m[K(_Args&&...) = delete;
2025-11-08T13:22:50.1954549Z       |     [01;36m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1955053Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/unique_ptr.h:860:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.1955601Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1851:97:[m[K [01;31m[Kerror: [m[Ktemplate argument 1 is invalid
2025-11-08T13:22:50.1955948Z  1851 |         threat_cards_[i] = std::make_unique<ThreatCard>(i, Rect{0, card_y_pos, screen_width, 24}[01;31m[K)[m[K;
2025-11-08T13:22:50.1956132Z       |                                                                                                 [01;31m[K^[m[K
2025-11-08T13:22:50.1956914Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1855:5:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_scanner_update[m[K' was not declared in this scope
2025-11-08T13:22:50.1957235Z  1855 |     [01;31m[Khandle_scanner_update[m[K();
2025-11-08T13:22:50.1957361Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1957763Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1958534Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1858:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1958861Z  1858 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::update_modern_layout() {
2025-11-08T13:22:50.1959002Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1959529Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_modern_layout()[m[K':
2025-11-08T13:22:50.1960565Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1859:5:[m[K [01;31m[Kerror: [m[K'[01m[Khandle_scanner_update[m[K' was not declared in this scope
2025-11-08T13:22:50.1960941Z  1859 |     [01;31m[Khandle_scanner_update[m[K();
2025-11-08T13:22:50.1961171Z       |     [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1961911Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.1962824Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1862:6:[m[K [01;31m[Kerror: [m[K'[01m[KEnhancedDroneSpectrumAnalyzerView[m[K' has not been declared
2025-11-08T13:22:50.1963157Z  1862 | void [01;31m[KEnhancedDroneSpectrumAnalyzerView[m[K::handle_scanner_update() {
2025-11-08T13:22:50.1963301Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1963832Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid handle_scanner_update()[m[K':
2025-11-08T13:22:50.1964430Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1863:10:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.1964649Z  1863 |     if (![01;31m[Kscanner_[m[K || !smart_header_ || !status_bar_) return;
2025-11-08T13:22:50.1964765Z       |          [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.1974302Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1863:23:[m[K [01;31m[Kerror: [m[K'[01m[Ksmart_header_[m[K' was not declared in this scope
2025-11-08T13:22:50.1974547Z  1863 |     if (!scanner_ || ![01;31m[Ksmart_header_[m[K || !status_bar_) return;
2025-11-08T13:22:50.1974683Z       |                       [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.1985309Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1863:41:[m[K [01;31m[Kerror: [m[K'[01m[Kstatus_bar_[m[K' was not declared in this scope
2025-11-08T13:22:50.1985585Z  1863 |     if (!scanner_ || !smart_header_ || ![01;31m[Kstatus_bar_[m[K) return;
2025-11-08T13:22:50.1985730Z       |                                         [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.1993105Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1865:30:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.1993381Z  1865 |     ThreatLevel max_threat = [01;31m[Kscanner_[m[K->get_max_detected_threat();
2025-11-08T13:22:50.1993518Z       |                              [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.2005606Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1873:9:[m[K [01;31m[Kerror: [m[K'[01m[Ksmart_header_[m[K' was not declared in this scope
2025-11-08T13:22:50.2005959Z  1873 |     if ([01;31m[Ksmart_header_[m[K) {
2025-11-08T13:22:50.2006078Z       |         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2017649Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1878:9:[m[K [01;31m[Kerror: [m[K'[01m[Kstatus_bar_[m[K' was not declared in this scope
2025-11-08T13:22:50.2017928Z  1878 |     if ([01;31m[Kstatus_bar_[m[K) {
2025-11-08T13:22:50.2018373Z       |         [01;31m[K^~~~~~~~~~~[m[K
2025-11-08T13:22:50.2019688Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kmin(long unsigned int, unsigned int)[m[K'
2025-11-08T13:22:50.2020114Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;31m[K)[m[K;
2025-11-08T13:22:50.2020379Z       |                                                          [01;31m[K^[m[K
2025-11-08T13:22:50.2021022Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39[m[K,
2025-11-08T13:22:50.2021538Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40[m[K,
2025-11-08T13:22:50.2022225Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.2022658Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.2023913Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)[m[K'
2025-11-08T13:22:50.2024089Z   198 |     [01;36m[Kmin[m[K(const _Tp& __a, const _Tp& __b)
2025-11-08T13:22:50.2024204Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.2024737Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:198:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.2025613Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  deduced conflicting types for parameter '[01m[Kconst _Tp[m[K' ('[01m[Klong unsigned int[m[K' and '[01m[Kunsigned int[m[K')
2025-11-08T13:22:50.2025856Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-08T13:22:50.2026008Z       |                                                          [01;36m[K^[m[K
2025-11-08T13:22:50.2026369Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/char_traits.h:39[m[K,
2025-11-08T13:22:50.2026795Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:40[m[K,
2025-11-08T13:22:50.2027187Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.2027560Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.2028399Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)[m[K'
2025-11-08T13:22:50.2028608Z   246 |     [01;36m[Kmin[m[K(const _Tp& __a, const _Tp& __b, _Compare __comp)
2025-11-08T13:22:50.2028714Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.2029240Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algobase.h:246:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.2030082Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  deduced conflicting types for parameter '[01m[Kconst _Tp[m[K' ('[01m[Klong unsigned int[m[K' and '[01m[Kunsigned int[m[K')
2025-11-08T13:22:50.2030486Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-08T13:22:50.2030639Z       |                                                          [01;36m[K^[m[K
2025-11-08T13:22:50.2030984Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62[m[K,
2025-11-08T13:22:50.2031472Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6[m[K,
2025-11-08T13:22:50.2031850Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.2032216Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.2032937Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)[m[K'
2025-11-08T13:22:50.2033103Z  3444 |     [01;36m[Kmin[m[K(initializer_list<_Tp> __l)
2025-11-08T13:22:50.2033204Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.2033690Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3444:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.2034485Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Klong unsigned int[m[K'
2025-11-08T13:22:50.2034705Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-08T13:22:50.2034856Z       |                                                          [01;36m[K^[m[K
2025-11-08T13:22:50.2035162Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/algorithm:62[m[K,
2025-11-08T13:22:50.2035522Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:6[m[K,
2025-11-08T13:22:50.2035892Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.2036265Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.2037179Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)[m[K'
2025-11-08T13:22:50.2037382Z  3450 |     [01;36m[Kmin[m[K(initializer_list<_Tp> __l, _Compare __comp)
2025-11-08T13:22:50.2037480Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.2037960Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/stl_algo.h:3450:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.2038750Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1881:58:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Klong unsigned int[m[K'
2025-11-08T13:22:50.2038964Z  1881 |             uint32_t progress = std::min(cycles * 5, 100u[01;36m[K)[m[K;
2025-11-08T13:22:50.2039113Z       |                                                          [01;36m[K^[m[K
2025-11-08T13:22:50.2043201Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1889:41:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.2043904Z  1889 |             const char* primary_msg = (![01;31m[Kdisplay_controller_[m[K || display_controller_->big_display().text().empty()) ?
2025-11-08T13:22:50.2044186Z       |                                         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2045602Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1893:90:[m[K [01;35m[Kwarning: [m[Kformat '[01m[K%u[m[K' expects argument of type '[01m[Kunsigned int[m[K', but argument 4 has type '[01m[Kuint32_t[m[K' {aka '[01m[Klong unsigned int[m[K'} [[01;35m[K-Wformat=[m[K]
2025-11-08T13:22:50.2046164Z  1893 |                 snprintf(secondary_buffer, sizeof(secondary_buffer), "Total detections: [01;35m[K%u[m[K", [32m[Ktotal_detections[m[K);
2025-11-08T13:22:50.2046389Z       |                                                                                         [01;35m[K~^[m[K   [32m[K~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2046786Z       |                                                                                          [01;35m[K|[m[K   [32m[K|[m[K
2025-11-08T13:22:50.2047045Z       |                                                                                          [01;35m[K|[m[K   [32m[Kuint32_t {aka long unsigned int}[m[K
2025-11-08T13:22:50.2047243Z       |                                                                                          [01;35m[Kunsigned int[m[K
2025-11-08T13:22:50.2047422Z       |                                                                                         [32m[K%lu[m[K
2025-11-08T13:22:50.2058270Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1901:48:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_cards_[m[K' was not declared in this scope
2025-11-08T13:22:50.2058559Z  1901 |     for (size_t i = 0; i < std::min(size_t(3), [01;31m[Kthreat_cards_[m[K.size()); ++i) {
2025-11-08T13:22:50.2058714Z       |                                                [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2083531Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1922:41:[m[K [01;31m[Kerror: [m[K'[01m[Kthreat_cards_[m[K' was not declared in this scope
2025-11-08T13:22:50.2083911Z  1922 |     for (size_t i = std::min(size_t(3), [01;31m[Kthreat_cards_[m[K.size()); i < threat_cards_.size(); ++i) {
2025-11-08T13:22:50.2084062Z       |                                         [01;31m[K^~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2100989Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1928:9:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.2101182Z  1928 |     if ([01;31m[Kdisplay_controller_[m[K) {
2025-11-08T13:22:50.2101306Z       |         [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2117464Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2118399Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1933:1:[m[K [01;31m[Kerror: [m[K'[01m[KLoadingScreenView[m[K' does not name a type
2025-11-08T13:22:50.2118780Z  1933 | [01;31m[KLoadingScreenView[m[K::LoadingScreenView(NavigationView& nav)
2025-11-08T13:22:50.2118922Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2119700Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1935:39:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before '[01m[K,[m[K' token
2025-11-08T13:22:50.2119941Z  1935 |       text_eda_(Rect{108, 213, 24, 16}[01;31m[K,[m[K "EDA"),
2025-11-08T13:22:50.2120110Z       |                                       [01;31m[K^[m[K
2025-11-08T13:22:50.2120867Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1935:41:[m[K [01;31m[Kerror: [m[Kexpected unqualified-id before string constant
2025-11-08T13:22:50.2121098Z  1935 |       text_eda_(Rect{108, 213, 24, 16}, [01;31m[K"EDA"[m[K),
2025-11-08T13:22:50.2121273Z       |                                         [01;31m[K^~~~~[m[K
2025-11-08T13:22:50.2133855Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1943:1:[m[K [01;31m[Kerror: [m[K'[01m[KLoadingScreenView[m[K' does not name a type
2025-11-08T13:22:50.2134333Z  1943 | [01;31m[KLoadingScreenView[m[K::~LoadingScreenView() {
2025-11-08T13:22:50.2134548Z       | [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2135726Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1946:6:[m[K [01;31m[Kerror: [m[K'[01m[KLoadingScreenView[m[K' has not been declared
2025-11-08T13:22:50.2136020Z  1946 | void [01;31m[KLoadingScreenView[m[K::paint(Painter& painter) {
2025-11-08T13:22:50.2136139Z       |      [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2137059Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1946:6:[m[K [01;31m[Kerror: [m[Kredefinition of '[01m[Kvoid paint(ui::Painter&)[m[K'
2025-11-08T13:22:50.2137754Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1026:6:[m[K [01;36m[Knote: [m[K'[01m[Kvoid paint(ui::Painter&)[m[K' previously defined here
2025-11-08T13:22:50.2137993Z  1026 | void [01;36m[KSmartThreatHeader[m[K::paint(Painter& painter) {
2025-11-08T13:22:50.2138113Z       |      [01;36m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2138714Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid paint(ui::Painter&)[m[K':
2025-11-08T13:22:50.2139552Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1951:24:[m[K [01;31m[Kerror: [m[Kcannot call member function '[01m[Kvirtual void ui::View::paint(ui::Painter&)[m[K' without object
2025-11-08T13:22:50.2139698Z  1951 |     View::paint(painter[01;31m[K)[m[K;
2025-11-08T13:22:50.2139817Z       |                        [01;31m[K^[m[K
2025-11-08T13:22:50.2151935Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2152592Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1954:1:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' does not name a type
2025-11-08T13:22:50.2152912Z  1954 | [01;31m[KScanningCoordinator[m[K::ScanningCoordinator(NavigationView& nav,
2025-11-08T13:22:50.2153033Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2169018Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1970:1:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' does not name a type
2025-11-08T13:22:50.2169285Z  1970 | [01;31m[KScanningCoordinator[m[K::~ScanningCoordinator() {
2025-11-08T13:22:50.2169408Z       | [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2170033Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1974:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-08T13:22:50.2170321Z  1974 | void [01;31m[KScanningCoordinator[m[K::start_coordinated_scanning() {
2025-11-08T13:22:50.2170446Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2185276Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid start_coordinated_scanning()[m[K':
2025-11-08T13:22:50.2185904Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1975:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.2186153Z  1975 |     if ([01;31m[Kscanning_active_[m[K || scanning_thread_ != nullptr) return;
2025-11-08T13:22:50.2186274Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2199566Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1975:29:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-08T13:22:50.2200013Z  1975 |     if (scanning_active_ || [01;31m[Kscanning_thread_[m[K != nullptr) return;
2025-11-08T13:22:50.2200159Z       |                             [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2200410Z       |                             [32m[Kstop_scanning_thread[m[K
2025-11-08T13:22:50.2215662Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1977:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.2215829Z  1977 |     [01;31m[Kscanning_active_[m[K = true;
2025-11-08T13:22:50.2215945Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2229523Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1979:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-08T13:22:50.2229858Z  1979 |     [01;31m[Kscanning_thread_[m[K = chThdCreateFromHeap(nullptr, SCANNING_THREAD_STACK_SIZE,
2025-11-08T13:22:50.2229977Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2230109Z       |     [32m[Kstop_scanning_thread[m[K
2025-11-08T13:22:50.2246991Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:43:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_function[m[K' was not declared in this scope
2025-11-08T13:22:50.2247375Z  1981 |                                           [01;31m[Kscanning_thread_function[m[K, this);
2025-11-08T13:22:50.2247661Z       |                                           [01;31m[K^~~~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2248779Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1981:69:[m[K [01;31m[Kerror: [m[Kinvalid use of '[01m[Kthis[m[K' in non-member function
2025-11-08T13:22:50.2249145Z  1981 |                                           scanning_thread_function, [01;31m[Kthis[m[K);
2025-11-08T13:22:50.2249433Z       |                                                                     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.2250147Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2251215Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1987:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-08T13:22:50.2251700Z  1987 | void [01;31m[KScanningCoordinator[m[K::stop_coordinated_scanning() {
2025-11-08T13:22:50.2251900Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2265905Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid stop_coordinated_scanning()[m[K':
2025-11-08T13:22:50.2267226Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1988:10:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.2267502Z  1988 |     if (![01;31m[Kscanning_active_[m[K) return;
2025-11-08T13:22:50.2267709Z       |          [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2283666Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1990:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.2283847Z  1990 |     [01;31m[Kscanning_active_[m[K = false;
2025-11-08T13:22:50.2283966Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2297875Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1991:9:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-08T13:22:50.2298375Z  1991 |     if ([01;31m[Kscanning_thread_[m[K) {
2025-11-08T13:22:50.2298583Z       |         [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2298801Z       |         [32m[Kstop_scanning_thread[m[K
2025-11-08T13:22:50.2299643Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2300723Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1997:7:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-08T13:22:50.2301248Z  1997 | msg_t [01;31m[KScanningCoordinator[m[K::scanning_thread_function(void* arg) {
2025-11-08T13:22:50.2301443Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2317608Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kmsg_t scanning_thread_function(void*)[m[K':
2025-11-08T13:22:50.2318738Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:30:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' does not name a type
2025-11-08T13:22:50.2319128Z  1998 |     auto* self = static_cast<[01;31m[KScanningCoordinator[m[K*>(arg);
2025-11-08T13:22:50.2319381Z       |                              [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2320454Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:49:[m[K [01;31m[Kerror: [m[Kexpected '[01m[K>[m[K' before '[01m[K*[m[K' token
2025-11-08T13:22:50.2320837Z  1998 |     auto* self = static_cast<ScanningCoordinator[01;31m[K*[m[K>(arg);
2025-11-08T13:22:50.2321088Z       |                                                 [01;31m[K^[m[K
2025-11-08T13:22:50.2322149Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:49:[m[K [01;31m[Kerror: [m[Kexpected '[01m[K([m[K' before '[01m[K*[m[K' token
2025-11-08T13:22:50.2322530Z  1998 |     auto* self = static_cast<ScanningCoordinator[01;31m[K*[m[K>(arg);
2025-11-08T13:22:50.2322773Z       |                                                 [01;31m[K^[m[K
2025-11-08T13:22:50.2323014Z       |                                                 [32m[K([m[K
2025-11-08T13:22:50.2324113Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:50:[m[K [01;31m[Kerror: [m[Kexpected primary-expression before '[01m[K>[m[K' token
2025-11-08T13:22:50.2324538Z  1998 |     auto* self = static_cast<ScanningCoordinator*[01;31m[K>[m[K(arg);
2025-11-08T13:22:50.2324780Z       |                                                  [01;31m[K^[m[K
2025-11-08T13:22:50.2325830Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1998:56:[m[K [01;31m[Kerror: [m[Kexpected '[01m[K)[m[K' before '[01m[K;[m[K' token
2025-11-08T13:22:50.2326210Z  1998 |     auto* self = static_cast<ScanningCoordinator*>(arg)[01;31m[K;[m[K
2025-11-08T13:22:50.2326453Z       |                                                        [01;31m[K^[m[K
2025-11-08T13:22:50.2326859Z       |                                                        [32m[K)[m[K
2025-11-08T13:22:50.2327578Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2328653Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2002:7:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-08T13:22:50.2329188Z  2002 | msg_t [01;31m[KScanningCoordinator[m[K::coordinated_scanning_thread() {
2025-11-08T13:22:50.2329411Z       |       [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2340448Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kmsg_t coordinated_scanning_thread()[m[K':
2025-11-08T13:22:50.2341563Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2003:12:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.2342159Z  2003 |     while ([01;31m[Kscanning_active_[m[K && !chThdShouldTerminateX()) {
2025-11-08T13:22:50.2342375Z       |            [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2357613Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2003:33:[m[K [01;31m[Kerror: [m[K'[01m[KchThdShouldTerminateX[m[K' was not declared in this scope; did you mean '[01m[KchThdShouldTerminate[m[K'?
2025-11-08T13:22:50.2358057Z  2003 |     while (scanning_active_ && ![01;31m[KchThdShouldTerminateX[m[K()) {
2025-11-08T13:22:50.2358310Z       |                                 [01;31m[K^~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2358586Z       |                                 [32m[KchThdShouldTerminate[m[K
2025-11-08T13:22:50.2366587Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2004:13:[m[K [01;31m[Kerror: [m[K'[01m[Kscanner_[m[K' was not declared in this scope
2025-11-08T13:22:50.2367083Z  2004 |         if ([01;31m[Kscanner_[m[K.is_scanning_active()) {
2025-11-08T13:22:50.2367283Z       |             [01;31m[K^~~~~~~~[m[K
2025-11-08T13:22:50.2376921Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2005:13:[m[K [01;31m[Kerror: [m[K'[01m[Khardware_[m[K' was not declared in this scope
2025-11-08T13:22:50.2377270Z  2005 |             [01;31m[Khardware_[m[K.update_spectrum_for_scanner();
2025-11-08T13:22:50.2377468Z       |             [01;31m[K^~~~~~~~~[m[K
2025-11-08T13:22:50.2395235Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2008:13:[m[K [01;31m[Kerror: [m[K'[01m[Kdisplay_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.2396574Z  2008 |             [01;31m[Kdisplay_controller_[m[K.update_detection_display(scanner_);
2025-11-08T13:22:50.2396961Z       |             [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2413781Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2010:17:[m[K [01;31m[Kerror: [m[K'[01m[Kaudio_controller_[m[K' was not declared in this scope
2025-11-08T13:22:50.2414131Z  2010 |             if ([01;31m[Kaudio_controller_[m[K.is_audio_enabled() &&
2025-11-08T13:22:50.2414359Z       |                 [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2431869Z In file included from [01m[K/havoc/firmware/chibios/os/kernel/include/ch.h:115[m[K,
2025-11-08T13:22:50.2432581Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:8[m[K,
2025-11-08T13:22:50.2433277Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:18[m[K,
2025-11-08T13:22:50.2433936Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.2435052Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2015:32:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_interval_ms_[m[K' was not declared in this scope
2025-11-08T13:22:50.2435430Z  2015 |         chThdSleepMilliseconds([01;31m[Kscan_interval_ms_[m[K);
2025-11-08T13:22:50.2435671Z       |                                [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2436418Z [01m[K/havoc/firmware/chibios/os/kernel/include/chvt.h:67:30:[m[K [01;36m[Knote: [m[Kin definition of macro '[01m[KMS2ST[m[K'
2025-11-08T13:22:50.2437110Z    67 |   ((systime_t)(((((uint32_t)([01;36m[Kmsec[m[K)) * ((uint32_t)CH_FREQUENCY) - 1UL) /     \
2025-11-08T13:22:50.2437555Z       |                              [01;36m[K^~~~[m[K
2025-11-08T13:22:50.2438654Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2015:9:[m[K [01;36m[Knote: [m[Kin expansion of macro '[01m[KchThdSleepMilliseconds[m[K'
2025-11-08T13:22:50.2438986Z  2015 |         [01;36m[KchThdSleepMilliseconds[m[K(scan_interval_ms_);
2025-11-08T13:22:50.2439356Z       |         [01;36m[K^~~~~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2452591Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2017:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_active_[m[K' was not declared in this scope
2025-11-08T13:22:50.2452862Z  2017 |     [01;31m[Kscanning_active_[m[K = false;
2025-11-08T13:22:50.2453053Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2468259Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2018:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscanning_thread_[m[K' was not declared in this scope; did you mean '[01m[Kstop_scanning_thread[m[K'?
2025-11-08T13:22:50.2468557Z  2018 |     [01;31m[Kscanning_thread_[m[K = nullptr;
2025-11-08T13:22:50.2468751Z       |     [01;31m[K^~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2468960Z       |     [32m[Kstop_scanning_thread[m[K
2025-11-08T13:22:50.2469689Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2470766Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2023:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-08T13:22:50.2471469Z  2023 | void [01;31m[KScanningCoordinator[m[K::update_runtime_parameters(const DroneAnalyzerSettings& settings) {
2025-11-08T13:22:50.2471667Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2487940Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid update_runtime_parameters(const DroneAnalyzerSettings&)[m[K':
2025-11-08T13:22:50.2489095Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2024:5:[m[K [01;31m[Kerror: [m[K'[01m[Kscan_interval_ms_[m[K' was not declared in this scope
2025-11-08T13:22:50.2489478Z  2024 |     [01;31m[Kscan_interval_ms_[m[K = settings.scan_interval_ms;
2025-11-08T13:22:50.2489683Z       |     [01;31m[K^~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2490416Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2491523Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2027:6:[m[K [01;31m[Kerror: [m[K'[01m[KScanningCoordinator[m[K' has not been declared
2025-11-08T13:22:50.2492182Z  2027 | void [01;31m[KScanningCoordinator[m[K::show_session_summary(const std::string& summary) {
2025-11-08T13:22:50.2492422Z       |      [01;31m[K^~~~~~~~~~~~~~~~~~~[m[K
2025-11-08T13:22:50.2493998Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kvoid show_session_summary(const string&)[m[K':
2025-11-08T13:22:50.2494864Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2028:5:[m[K [01;31m[Kerror: [m[K'[01m[Knav_[m[K' was not declared in this scope
2025-11-08T13:22:50.2495199Z  2028 |     [01;31m[Knav_[m[K.display_modal("Session Summary", summary.c_str());
2025-11-08T13:22:50.2495362Z       |     [01;31m[K^~~~[m[K
2025-11-08T13:22:50.2495957Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K At global scope:
2025-11-08T13:22:50.2497144Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:2031:1:[m[K [01;31m[Kerror: [m[Kexpected declaration before '[01m[K}[m[K' token
2025-11-08T13:22:50.2497913Z  2031 | [01;31m[K}[m[K // namespace ui::external_app::enhanced_drone_analyzer
2025-11-08T13:22:50.2498103Z       | [01;31m[K^[m[K
2025-11-08T13:22:50.3061077Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3062210Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3062959Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3064046Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-08T13:22:50.3064905Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3065844Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kbegin(const char* const&)[m[K'
2025-11-08T13:22:50.3066313Z    89 |         : path{[01;31m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-08T13:22:50.3066577Z       |                [01;31m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3067502Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-08T13:22:50.3068027Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3068734Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3069410Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3070781Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::begin(std::initializer_list<_Tp>)[m[K'
2025-11-08T13:22:50.3071185Z    89 |     [01;36m[Kbegin[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-08T13:22:50.3071358Z       |     [01;36m[K^~~~~[m[K
2025-11-08T13:22:50.3072290Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:89:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3073097Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3073822Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3074547Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3075691Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-08T13:22:50.3076145Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-08T13:22:50.3076398Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3077198Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3077962Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3078694Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3080201Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&)[m[K'
2025-11-08T13:22:50.3080938Z    48 |     [01;36m[Kbegin[m[K(_Container& __cont) -> decltype(__cont.begin())
2025-11-08T13:22:50.3081141Z       |     [01;36m[K^~~~~[m[K
2025-11-08T13:22:50.3082078Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3083960Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(_Container&) [with _Container = const char* const][m[K':
2025-11-08T13:22:50.3085046Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-08T13:22:50.3085858Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3087346Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:48:50:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3087809Z    48 |     begin(_Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-08T13:22:50.3088094Z       |                                           [01;31m[K~~~~~~~^~~~~[m[K
2025-11-08T13:22:50.3089049Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-08T13:22:50.3089880Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3091416Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&)[m[K'
2025-11-08T13:22:50.3091903Z    58 |     [01;36m[Kbegin[m[K(const _Container& __cont) -> decltype(__cont.begin())
2025-11-08T13:22:50.3092111Z       |     [01;36m[K^~~~~[m[K
2025-11-08T13:22:50.3093044Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3094573Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.begin()) std::begin(const _Container&) [with _Container = const char*][m[K':
2025-11-08T13:22:50.3095592Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-08T13:22:50.3096433Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3097925Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:58:56:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kbegin[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3098417Z    58 |     begin(const _Container& __cont) -> decltype([01;31m[K__cont.begin[m[K())
2025-11-08T13:22:50.3098719Z       |                                                 [01;31m[K~~~~~~~^~~~~[m[K
2025-11-08T13:22:50.3099750Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-08T13:22:50.3100599Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3102011Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::begin(_Tp (&)[_Nm])[m[K'
2025-11-08T13:22:50.3102574Z    87 |     [01;36m[Kbegin[m[K(_Tp (&__arr)[_Nm])
2025-11-08T13:22:50.3102773Z       |     [01;36m[K^~~~~[m[K
2025-11-08T13:22:50.3103672Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:87:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3104686Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3105440Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3106142Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3107275Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3107758Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-08T13:22:50.3108005Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3108590Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3109339Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3110062Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3111370Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::begin(std::valarray<_Tp>&)[m[K'
2025-11-08T13:22:50.3111818Z   104 |   template<typename _Tp> _Tp* [01;36m[Kbegin[m[K(valarray<_Tp>&);
2025-11-08T13:22:50.3112093Z       |                               [01;36m[K^~~~~[m[K
2025-11-08T13:22:50.3113047Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:104:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3113846Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3114594Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3115352Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3116455Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3117171Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-08T13:22:50.3117457Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3118026Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3118753Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3119490Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3120875Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::begin(const std::valarray<_Tp>&)[m[K'
2025-11-08T13:22:50.3121380Z   105 |   template<typename _Tp> const _Tp* [01;36m[Kbegin[m[K(const valarray<_Tp>&);
2025-11-08T13:22:50.3121652Z       |                                     [01;36m[K^~~~~[m[K
2025-11-08T13:22:50.3122859Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:105:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3123652Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3124696Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3125483Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3126805Z [01m[K/havoc/firmware/application/./file.hpp:89:26:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3127284Z    89 |         : path{[01;36m[Kstd::begin(source)[m[K, std::end(source)} {
2025-11-08T13:22:50.3127533Z       |                [01;36m[K~~~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3128410Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kend(const char* const&)[m[K'
2025-11-08T13:22:50.3128802Z    89 |         : path{std::begin(source), [01;31m[Kstd::end(source)[m[K} {
2025-11-08T13:22:50.3129066Z       |                                    [01;31m[K~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3129716Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:36[m[K,
2025-11-08T13:22:50.3130237Z                  from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3130961Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3131696Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3133085Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> constexpr const _Tp* std::end(std::initializer_list<_Tp>)[m[K'
2025-11-08T13:22:50.3133435Z    99 |     [01;36m[Kend[m[K(initializer_list<_Tp> __ils) noexcept
2025-11-08T13:22:50.3133632Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.3134545Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/initializer_list:99:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3135326Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3136037Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3136923Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3137983Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::initializer_list<_Tp>[m[K' and '[01m[Kconst char*[m[K'
2025-11-08T13:22:50.3138342Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-08T13:22:50.3138609Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3139162Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3139876Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3140592Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3142058Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&)[m[K'
2025-11-08T13:22:50.3142752Z    68 |     [01;36m[Kend[m[K(_Container& __cont) -> decltype(__cont.end())
2025-11-08T13:22:50.3142963Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.3143897Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3145696Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(_Container&) [with _Container = const char* const][m[K':
2025-11-08T13:22:50.3147010Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-08T13:22:50.3147873Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3149179Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:68:48:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3149620Z    68 |     end(_Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-08T13:22:50.3149908Z       |                                         [01;31m[K~~~~~~~^~~[m[K
2025-11-08T13:22:50.3150877Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-08T13:22:50.3151682Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3153183Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&)[m[K'
2025-11-08T13:22:50.3153623Z    78 |     [01;36m[Kend[m[K(const _Container& __cont) -> decltype(__cont.end())
2025-11-08T13:22:50.3153844Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.3154773Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3156282Z /opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h: In substitution of '[01m[Ktemplate<class _Container> constexpr decltype (__cont.end()) std::end(const _Container&) [with _Container = const char*][m[K':
2025-11-08T13:22:50.3157629Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K   required from '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K'
2025-11-08T13:22:50.3158453Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3159736Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:78:54:[m[K [01;31m[Kerror: [m[Krequest for member '[01m[Kend[m[K' in '[01m[K__cont[m[K', which is of non-class type '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3160249Z    78 |     end(const _Container& __cont) -> decltype([01;31m[K__cont.end[m[K())
2025-11-08T13:22:50.3160548Z       |                                               [01;31m[K~~~~~~~^~~[m[K
2025-11-08T13:22:50.3161529Z /havoc/firmware/application/./file.hpp: In instantiation of '[01m[Kstd::filesystem::path::path(const Source&) [with Source = const char*][m[K':
2025-11-08T13:22:50.3162358Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:62:61:[m[K   required from here
2025-11-08T13:22:50.3163720Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp, unsigned int _Nm> constexpr _Tp* std::end(_Tp (&)[_Nm])[m[K'
2025-11-08T13:22:50.3164263Z    97 |     [01;36m[Kend[m[K(_Tp (&__arr)[_Nm])
2025-11-08T13:22:50.3164456Z       |     [01;36m[K^~~[m[K
2025-11-08T13:22:50.3165366Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:97:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3166358Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3167336Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3168087Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3168958Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[K_Tp [_Nm][m[K' and '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3169350Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-08T13:22:50.3169596Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3170115Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3170870Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3171594Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3172911Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> _Tp* std::end(std::valarray<_Tp>&)[m[K'
2025-11-08T13:22:50.3173356Z   106 |   template<typename _Tp> _Tp* [01;36m[Kend[m[K(valarray<_Tp>&);
2025-11-08T13:22:50.3173615Z       |                               [01;36m[K^~~[m[K
2025-11-08T13:22:50.3174571Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:106:31:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3175369Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3176133Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3177089Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3178205Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kstd::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3178607Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-08T13:22:50.3178886Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3179458Z In file included from [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/string:54[m[K,
2025-11-08T13:22:50.3180206Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:13[m[K,
2025-11-08T13:22:50.3180941Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3182330Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class _Tp> const _Tp* std::end(const std::valarray<_Tp>&)[m[K'
2025-11-08T13:22:50.3182812Z   107 |   template<typename _Tp> const _Tp* [01;36m[Kend[m[K(const valarray<_Tp>&);
2025-11-08T13:22:50.3183062Z       |                                     [01;36m[K^~~[m[K
2025-11-08T13:22:50.3184235Z [01m[K/opt/build/armbin/arm-none-eabi/include/c++/9.2.1/bits/range_access.h:107:37:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3185023Z In file included from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:8[m[K,
2025-11-08T13:22:50.3186010Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:20[m[K,
2025-11-08T13:22:50.3187083Z                  from [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:4[m[K:
2025-11-08T13:22:50.3188207Z [01m[K/havoc/firmware/application/./file.hpp:89:44:[m[K [01;36m[Knote: [m[K  mismatched types '[01m[Kconst std::valarray<_Tp>[m[K' and '[01m[Kconst char* const[m[K'
2025-11-08T13:22:50.3188611Z    89 |         : path{std::begin(source), [01;36m[Kstd::end(source)[m[K} {
2025-11-08T13:22:50.3188896Z       |                                    [01;36m[K~~~~~~~~^~~~~~~~[m[K
2025-11-08T13:22:50.3190144Z [01m[K/havoc/firmware/application/./file.hpp:89:52:[m[K [01;31m[Kerror: [m[Kno matching function for call to '[01m[Kstd::filesystem::path::path(<brace-enclosed initializer list>)[m[K'
2025-11-08T13:22:50.3190546Z    89 |         : path{std::begin(source), std::end(source)[01;31m[K}[m[K {
2025-11-08T13:22:50.3190847Z       |                                                    [01;31m[K^[m[K
2025-11-08T13:22:50.3191806Z [01m[K/havoc/firmware/application/./file.hpp:102:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const TCHAR*)[m[K'
2025-11-08T13:22:50.3192090Z   102 |     [01;36m[Kpath[m[K(const TCHAR* const s)
2025-11-08T13:22:50.3192305Z       |     [01;36m[K^~~~[m[K
2025-11-08T13:22:50.3193021Z [01m[K/havoc/firmware/application/./file.hpp:102:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-08T13:22:50.3194005Z [01m[K/havoc/firmware/application/./file.hpp:98:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const char16_t*)[m[K'
2025-11-08T13:22:50.3194327Z    98 |     [01;36m[Kpath[m[K(const char16_t* const s)
2025-11-08T13:22:50.3194527Z       |     [01;36m[K^~~~[m[K
2025-11-08T13:22:50.3195202Z [01m[K/havoc/firmware/application/./file.hpp:98:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-08T13:22:50.3196384Z [01m[K/havoc/firmware/application/./file.hpp:93:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class InputIt> std::filesystem::path::path(InputIt, InputIt)[m[K'
2025-11-08T13:22:50.3196881Z    93 |     [01;36m[Kpath[m[K(InputIt first,
2025-11-08T13:22:50.3197105Z       |     [01;36m[K^~~~[m[K
2025-11-08T13:22:50.3197659Z [01m[K/havoc/firmware/application/./file.hpp:93:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3198307Z [01m[K/havoc/firmware/application/./file.hpp:88:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Ktemplate<class Source> std::filesystem::path::path(const Source&)[m[K'
2025-11-08T13:22:50.3198460Z    88 |     [01;36m[Kpath[m[K(const Source& source)
2025-11-08T13:22:50.3198567Z       |     [01;36m[K^~~~[m[K
2025-11-08T13:22:50.3198967Z [01m[K/havoc/firmware/application/./file.hpp:88:5:[m[K [01;36m[Knote: [m[K  template argument deduction/substitution failed:
2025-11-08T13:22:50.3199522Z [01m[K/havoc/firmware/application/./file.hpp:83:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(std::filesystem::path&&)[m[K'
2025-11-08T13:22:50.3199666Z    83 |     [01;36m[Kpath[m[K(path&& p)
2025-11-08T13:22:50.3199770Z       |     [01;36m[K^~~~[m[K
2025-11-08T13:22:50.3200165Z [01m[K/havoc/firmware/application/./file.hpp:83:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-08T13:22:50.3200716Z [01m[K/havoc/firmware/application/./file.hpp:79:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path(const std::filesystem::path&)[m[K'
2025-11-08T13:22:50.3201051Z    79 |     [01;36m[Kpath[m[K(const path& p)
2025-11-08T13:22:50.3201154Z       |     [01;36m[K^~~~[m[K
2025-11-08T13:22:50.3201524Z [01m[K/havoc/firmware/application/./file.hpp:79:5:[m[K [01;36m[Knote: [m[K  candidate expects 1 argument, 2 provided
2025-11-08T13:22:50.3202084Z [01m[K/havoc/firmware/application/./file.hpp:75:5:[m[K [01;36m[Knote: [m[Kcandidate: '[01m[Kstd::filesystem::path::path()[m[K'
2025-11-08T13:22:50.3202195Z    75 |     [01;36m[Kpath[m[K()
2025-11-08T13:22:50.3202293Z       |     [01;36m[K^~~~[m[K
2025-11-08T13:22:50.3202660Z [01m[K/havoc/firmware/application/./file.hpp:75:5:[m[K [01;36m[Knote: [m[K  candidate expects 0 arguments, 2 provided
2025-11-08T13:22:50.4056423Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_bg_color()[m[K':
2025-11-08T13:22:50.4057720Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1096:1:[m[K [01;35m[Kwarning: [m[Kcontrol reaches end of non-void function [[01;35m[K-Wreturn-type[m[K]
2025-11-08T13:22:50.4057876Z  1096 | [01;35m[K}[m[K
2025-11-08T13:22:50.4057997Z       | [01;35m[K^[m[K
2025-11-08T13:22:50.4058785Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:[m[K In function '[01m[Kui::Color get_card_text_color()[m[K':
2025-11-08T13:22:50.4059630Z [01m[K/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.cpp:1108:1:[m[K [01;35m[Kwarning: [m[Kcontrol reaches end of non-void function [[01;35m[K-Wreturn-type[m[K]
2025-11-08T13:22:50.4059751Z  1108 | [01;35m[K}[m[K
2025-11-08T13:22:50.4059861Z       | [01;35m[K^[m[K
2025-11-08T13:22:50.4090400Z At global scope:
2025-11-08T13:22:50.4091080Z [01m[Kcc1plus:[m[K [01;35m[Kwarning: [m[Kunrecognized command line option '[01m[K-Wno-volatile[m[K'
2025-11-08T13:22:50.4131690Z make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5775: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_scanner_combined.cpp.obj] Error 1
2025-11-08T13:22:50.4137827Z make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
2025-11-08T13:22:50.4139903Z make: *** [Makefile:101: all] Error 2
2025-11-08T13:22:50.4881562Z ##[error]Process completed with exit code 2.
