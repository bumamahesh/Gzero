project(Hdr VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

add_library(Hdr SHARED HdrAlgorithm.cpp ${CORE_SOURCES})

target_include_directories(Hdr PUBLIC ${COMMON_INCLUDE_DIRS})
set_common_target_properties(Hdr)
enable_asan(Hdr)
install_target(Hdr HdrAlgorithm.h)
