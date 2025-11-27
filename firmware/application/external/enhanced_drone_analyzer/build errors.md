[ 98%] Building CXX object firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_signal_processing.cpp.obj
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:10:110: error: definition of 'void DetectionRingBuffer::update_detection(size_t, uint8_t, int32_t)' is not in namespace enclosing 'DetectionRingBuffer' [-fpermissive]
   10 | void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
      |                                                                                                              ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:10:6: error: redefinition of 'void DetectionRingBuffer::update_detection(size_t, uint8_t, int32_t)'
   10 | void DetectionRingBuffer::update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
      |      ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:1:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:63:10: note: 'void DetectionRingBuffer::update_detection(size_t, uint8_t, int32_t)' previously defined here
   63 |     void update_detection(size_t frequency_hash, uint8_t detection_count, int32_t rssi_value) {
      |          ^~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:24:73: error: definition of 'uint8_t DetectionRingBuffer::get_detection_count(size_t) const' is not in namespace enclosing 'DetectionRingBuffer' [-fpermissive]
   24 | uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
      |                                                                         ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:24:9: error: redefinition of 'uint8_t DetectionRingBuffer::get_detection_count(size_t) const'
   24 | uint8_t DetectionRingBuffer::get_detection_count(size_t frequency_hash) const {
      |         ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:1:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:81:13: note: 'uint8_t DetectionRingBuffer::get_detection_count(size_t) const' previously defined here
   81 |     uint8_t get_detection_count(size_t frequency_hash) const {
      |             ^~~~~~~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:33:68: error: definition of 'int32_t DetectionRingBuffer::get_rssi_value(size_t) const' is not in namespace enclosing 'DetectionRingBuffer' [-fpermissive]
   33 | int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const {
      |                                                                    ^~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:33:9: error: redefinition of 'int32_t DetectionRingBuffer::get_rssi_value(size_t) const'
   33 | int32_t DetectionRingBuffer::get_rssi_value(size_t frequency_hash) const {
      |         ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:1:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:90:13: note: 'int32_t DetectionRingBuffer::get_rssi_value(size_t) const' previously defined here
   90 |     int32_t get_rssi_value(size_t frequency_hash) const {
      |             ^~~~~~~~~~~~~~
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:42:33: error: definition of 'void DetectionRingBuffer::clear()' is not in namespace enclosing 'DetectionRingBuffer' [-fpermissive]
   42 | void DetectionRingBuffer::clear() {
      |                                 ^
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:42:6: error: redefinition of 'void DetectionRingBuffer::clear()'
   42 | void DetectionRingBuffer::clear() {
      |      ^~~~~~~~~~~~~~~~~~~
In file included from /havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.cpp:1:
/havoc/firmware/application/external/enhanced_drone_analyzer/ui_signal_processing.hpp:99:10: note: 'void DetectionRingBuffer::clear()' previously defined here
   99 |     void clear() {
      |          ^~~~~
cc1plus: warning: unrecognized command line option '-Wno-volatile'
make[2]: *** [firmware/application/CMakeFiles/application.elf.dir/build.make:5817: firmware/application/CMakeFiles/application.elf.dir/external/enhanced_drone_analyzer/ui_signal_processing.cpp.obj] Error 1
make[1]: *** [CMakeFiles/Makefile2:1337: firmware/application/CMakeFiles/application.elf.dir/all] Error 2
make: *** [Makefile:101: all] Error 2
Error: Process completed with exit code 2.