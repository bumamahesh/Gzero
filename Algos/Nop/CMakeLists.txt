project(Nop VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

add_library(Nop SHARED NopAlgorithm.cpp)

target_include_directories(Nop PUBLIC ${COMMON_INCLUDE_DIRS})
set_common_target_properties(Nop)
enable_asan(Nop)
install_target(Nop NopAlgorithm.h)