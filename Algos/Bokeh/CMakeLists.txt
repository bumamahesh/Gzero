project(Bokeh VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

# Find OpenCV package (adjust path if necessary)
find_package(OpenCV REQUIRED)
add_library(Bokeh SHARED BokehAlgorithm.cpp )

target_link_libraries(Bokeh PUBLIC ${OpenCV_LIBS})
target_include_directories(Bokeh PUBLIC ${COMMON_INCLUDE_DIRS})
set_common_target_properties(Bokeh)
enable_asan(Bokeh)
install_target(Bokeh BokehAlgorithm.h)
