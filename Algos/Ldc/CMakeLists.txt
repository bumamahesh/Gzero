project(Ldc VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

# Find OpenCV package
find_package(OpenCV)

add_library(Ldc SHARED LdcAlgorithm.cpp )

target_include_directories(Ldc PUBLIC ${COMMON_INCLUDE_DIRS})
# Link OpenCV if found
if(OpenCV_FOUND)
add_definitions(-D__OPENCV_ENABLE__=1)
    target_include_directories(Ldc PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(Ldc PRIVATE ${OpenCV_LIBS})
endif()

set_common_target_properties(Ldc)
enable_asan(Ldc)
install_target(Ldc LdcAlgorithm.h)