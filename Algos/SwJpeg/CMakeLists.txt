project(SwJpeg VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)
add_library(SwJpeg SHARED SwJpeg.cpp)
target_include_directories(SwJpeg PUBLIC ${COMMON_INCLUDE_DIRS})

# Find JPEG library
find_package(JPEG)
if(JPEG_FOUND)
add_definitions(-D__JPEGLIB__=1)
    message(STATUS "JPEG found: ${JPEG_INCLUDE_DIR}")
    target_include_directories(SwJpeg PRIVATE ${JPEG_INCLUDE_DIR})
    target_link_libraries(SwJpeg PRIVATE ${JPEG_LIBRARIES})
else()
    message(WARNING "JPEG library not found. SwJpeg will be built without JPEG support.")
endif()

set_common_target_properties(SwJpeg)
enable_asan(SwJpeg)
install_target(SwJpeg SwJpeg.h)
