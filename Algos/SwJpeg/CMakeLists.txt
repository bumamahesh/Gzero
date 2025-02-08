project(SwJpeg VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)
add_library(SwJpeg SHARED SwJpeg.cpp)
target_include_directories(SwJpeg PUBLIC ${COMMON_INCLUDE_DIRS})

find_package(JPEG REQUIRED)
target_include_directories(SwJpeg PRIVATE ${JPEG_INCLUDE_DIR})
target_link_libraries(SwJpeg PRIVATE ${JPEG_LIBRARIES})
set_common_target_properties(SwJpeg)
enable_asan(SwJpeg)

install_target(SwJpeg SwJpeg.h)
