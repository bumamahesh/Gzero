
project(WaterMark VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

# Find OpenCV package
find_package(OpenCV REQUIRED)

add_library(WaterMark SHARED WaterMarkAlgorithm.cpp ${CORE_SOURCES})

target_include_directories(WaterMark 
    PUBLIC 
    ${COMMON_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}  # Include OpenCV headers
)
# Link OpenCV libraries to WaterMark
target_link_libraries(WaterMark
    PRIVATE
    ${OpenCV_LIBS}  # Link OpenCV libraries
)
set_common_target_properties(WaterMark)
enable_asan(WaterMark)
install_target(WaterMark WaterMarkAlgorithm.h)
