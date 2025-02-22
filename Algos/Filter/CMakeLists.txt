project(Filter VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)
# Find OpenCV package
find_package(OpenCV)
add_library(Filter SHARED FilterAlgorithm.cpp )

target_include_directories(Filter PUBLIC ${COMMON_INCLUDE_DIRS})
if(OpenCV_FOUND)
add_definitions(-D__OPENCV_ENABLE__=1)
    target_include_directories(Filter PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(Filter PRIVATE ${OpenCV_LIBS})
endif()

set_common_target_properties(Filter)
enable_asan(Filter)
install_target(Filter FilterAlgorithm.h)
