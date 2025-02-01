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

# Ensure debug symbols are preserved
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")  # Enable debug symbols and disable optimizations
set(CMAKE_C_FLAGS_DEBUG "-g -O0")    # Same for C files
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)

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
