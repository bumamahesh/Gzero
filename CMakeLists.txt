project(TopG)
enable_testing()

# Minimum required CMake version
cmake_minimum_required(VERSION 3.10)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/cmake/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/cmake/lib)

# Set the project name and version
project(AlgoLib VERSION 1.0)

file(GLOB CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/*.cpp
)

# Define the shared library target
add_library(AlgoLib SHARED ${CORE_SOURCES})


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Ensure debug symbols are preserved
    # Set debug flags for Debug build type
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")  # Enable debug symbols and disable optimizations
    set(CMAKE_C_FLAGS_DEBUG "-g -O0")    # Same for C files
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Set release flags for Release build type (optional: you can add flags for release optimizations)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")   # Optimize for release
    set(CMAKE_C_FLAGS_RELEASE "-O3")      # Same for C files
endif()

# Set properties for the shared library
set_target_properties(AlgoLib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Include directories (if needed for headers)
target_include_directories(AlgoLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/include
)

# Optionally, install the shared library
install(TARGETS AlgoLib DESTINATION lib)
install(FILES include/Interface.h DESTINATION include)

# Add subdirectories
# add_subdirectory(Utils)
add_subdirectory(Algos/Hdr)
add_subdirectory(Algos/Bokeh)
add_subdirectory(Algos/Nop)
add_subdirectory(Algos/Filter)
add_subdirectory(Algos/MandelbrotSet)
add_subdirectory(Algos/Ldc)
add_subdirectory(Algos/WaterMark)
add_subdirectory(tests)
add_subdirectory(testApp)
