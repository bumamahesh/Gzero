project(MandelbrotSet VERSION 1.0 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/Algos/CommonCmake.txt)

find_package(OpenMP REQUIRED)

add_library(MandelbrotSet SHARED MandelbrotSet.cpp ${CORE_SOURCES})

target_include_directories(MandelbrotSet PUBLIC ${COMMON_INCLUDE_DIRS})
set_common_target_properties(MandelbrotSet)
enable_asan(MandelbrotSet)

if(OpenMP_CXX_FOUND)
    target_link_libraries(MandelbrotSet PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(MandelbrotSet PRIVATE ${OpenMP_CXX_FLAGS})
    message(STATUS "OpenMP enabled with flags: ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found. Build might be slower without parallelism.")
endif()

install_target(MandelbrotSet MandelbrotSet.h)
