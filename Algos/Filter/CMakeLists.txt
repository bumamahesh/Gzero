project(Filter VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

add_library(Filter SHARED FilterAlgorithm.cpp )

target_include_directories(Filter PUBLIC ${COMMON_INCLUDE_DIRS})
set_common_target_properties(Filter)
enable_asan(Filter)
install_target(Filter FilterAlgorithm.h)
