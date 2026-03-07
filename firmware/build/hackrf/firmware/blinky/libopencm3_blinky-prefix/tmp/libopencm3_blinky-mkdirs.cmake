# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/Max/Desktop/M/mayhem-firmware/hackrf/firmware/libopencm3")
  file(MAKE_DIRECTORY "C:/Users/Max/Desktop/M/mayhem-firmware/hackrf/firmware/libopencm3")
endif()
file(MAKE_DIRECTORY
  "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix/src/libopencm3_blinky-build"
  "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix"
  "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix/tmp"
  "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix/src/libopencm3_blinky-stamp"
  "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix/src"
  "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix/src/libopencm3_blinky-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix/src/libopencm3_blinky-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Max/Desktop/M/mayhem-firmware/firmware/build/hackrf/firmware/blinky/libopencm3_blinky-prefix/src/libopencm3_blinky-stamp${cfgdir}") # cfgdir has leading slash
endif()
