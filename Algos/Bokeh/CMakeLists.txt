project(Bokeh VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

add_library(Bokeh SHARED BokehAlgorithm.cpp ${CORE_SOURCES})

target_include_directories(Bokeh PUBLIC ${COMMON_INCLUDE_DIRS})
set_common_target_properties(Bokeh)
enable_asan(Bokeh)
install_target(Bokeh BokehAlgorithm.h)
