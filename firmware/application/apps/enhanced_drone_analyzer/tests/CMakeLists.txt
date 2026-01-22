cmake_minimum_required(VERSION 3.14)
project(EnhancedDroneAnalyzerTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

add_executable(test_median_filter test_median_filter.cpp)
target_compile_features(test_median_filter PUBLIC cxx_std_17)
add_test(NAME test_median_filter COMMAND test_median_filter)

add_executable(test_spectral_analyzer test_spectral_analyzer.cpp ../ui_spectral_analyzer.cpp)
target_include_directories(test_spectral_analyzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
target_compile_features(test_spectral_analyzer PUBLIC cxx_std_17)
add_test(NAME test_spectral_analyzer COMMAND test_spectral_analyzer)

add_executable(test_drone_validation test_drone_validation.cpp)
target_compile_features(test_drone_validation PUBLIC cxx_std_17)
add_test(NAME test_drone_validation COMMAND test_drone_validation)

enable_testing()
