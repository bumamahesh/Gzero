
project(WaterMark VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

# Find OpenCV package
find_package(OpenCV)

add_library(WaterMark SHARED WaterMarkAlgorithm.cpp)

target_include_directories(WaterMark 
    PUBLIC 
    ${COMMON_INCLUDE_DIRS}
)

# Link OpenCV if found
if(OpenCV_FOUND)
add_definitions(-D_CV_ENABLED_=1)
    target_include_directories(WaterMark PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(WaterMark PRIVATE ${OpenCV_LIBS})
endif()

set_common_target_properties(WaterMark)
enable_asan(WaterMark)
install_target(WaterMark WaterMarkAlgorithm.h)
