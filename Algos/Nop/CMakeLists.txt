cmake_minimum_required(VERSION 3.10)

# Project name and description
project(Nop VERSION 1.0 LANGUAGES CXX)
set(CMAKE_SHARED_LIBRARY_PREFIX "com.Algo.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/cmake/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/cmake/lib)

file(GLOB CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/Utils/src/TaskQueue.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/RequestMonitor.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/ConfigParser.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/Log.cpp
    ${CMAKE_SOURCE_DIR}/src/AlgoBase.cpp
    ${CMAKE_SOURCE_DIR}/src/AlgoRequest.cpp
)
# Add the shared library
add_library(Nop SHARED
    NopAlgorithm.cpp
    ${CORE_SOURCES}
)

# Include directories
target_include_directories(Nop
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/Utils/include 
)

# Set C++ standard
set_target_properties(Nop PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Option to enable/disable ASAN
option(ENABLE_ASAN "Enable AddressSanitizer (ASAN)" OFF)

if(ENABLE_ASAN)
    set(ASAN 1)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

    # Enable AddressSanitizer
    target_compile_options(Nop PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(Nop PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    # Set compiler flags for debugging information
    target_compile_options(Nop PRIVATE -g -ggdb -Wall -fPIC)

endif()

# Export the shared library
install(TARGETS Nop
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install header files
install(FILES NopAlgorithm.h DESTINATION include)
