[100%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/scanner_settings.hpp:10,
                 from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:31:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_scanner_combined.hpp:143:26: error: redefinition of 'constexpr const int32_t HYSTERESIS_MARGIN_DB'
  143 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp:28:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_drone_common_types.hpp:48:26: note: 'constexpr const int32_t HYSTERESIS_MARGIN_DB' previously defined here
   48 | static constexpr int32_t HYSTERESIS_MARGIN_DB = 5;
      |                          ^~~~~~~~~~~~~~~~~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5761: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/enhanced_drone_analyzer_scanner_main.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.