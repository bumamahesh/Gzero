cmake_minimum_required(VERSION 3.10)

# Project name and description
project(MandelbrotSet VERSION 1.0 LANGUAGES CXX)
set(CMAKE_SHARED_LIBRARY_PREFIX "com.Algo.")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/cmake/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/cmake/lib)

find_package(OpenMP REQUIRED)


file(GLOB CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/Utils/src/TaskQueue.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/Log.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/KpiMonitor.cpp
    ${CMAKE_SOURCE_DIR}/src/AlgoBase.cpp
    ${CMAKE_SOURCE_DIR}/src/AlgoRequest.cpp
)
# Add the shared library
add_library(MandelbrotSet SHARED
    MandelbrotSet.cpp
    ${CORE_SOURCES}
)

# Include directories
target_include_directories(MandelbrotSet
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/Utils/include 
)

# Set C++ standard
set_target_properties(MandelbrotSet PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Link OpenMP to the target if found
if(OpenMP_CXX_FOUND)
    target_link_libraries(MandelbrotSet PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(MandelbrotSet PRIVATE ${OpenMP_CXX_FLAGS})
    message(STATUS "OpenMP enabled with flags: ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found. Build might be slower without parallelism.")
endif()


# Option to enable/disable ASAN
option(ENABLE_ASAN "Enable AddressSanitizer (ASAN)" ON)

if(ENABLE_ASAN)
    set(ASAN 1)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

    # Enable AddressSanitizer
    target_compile_options(MandelbrotSet PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(MandelbrotSet PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    # Set compiler flags for debugging information
    target_compile_options(MandelbrotSet PRIVATE -g -ggdb -Wall -fPIC)

endif()

# Export the shared library
install(TARGETS MandelbrotSet
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install header files
install(FILES MandelbrotSet.h DESTINATION include)
