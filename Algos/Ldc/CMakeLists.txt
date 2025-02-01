project(Ldc VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

add_library(Ldc SHARED LdcAlgorithm.cpp ${CORE_SOURCES})

target_include_directories(Ldc PUBLIC ${COMMON_INCLUDE_DIRS})
set_common_target_properties(Ldc)
enable_asan(Ldc)
install_target(Ldc LdcAlgorithm.h)