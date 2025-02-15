cmake_minimum_required(VERSION 3.10)
project(test_app)

# Set the C++ standard (if you're using C++)
set(CMAKE_CXX_STANDARD 11)

include_directories(../include/ ..//Utils/include include/ )
# Find the SDL2 package
find_package(SDL2)

# Define the source files
set(SOURCES
    main.cpp
    src/AlgoInterfaceManager.cpp
    src/Renderer.cpp
    ../src/AlgoRequest.cpp
    ../src/AlgoDecisionManager.cpp
    ../src/AlgoMetadata.cpp
)
# add_compile_definitions(__RENDER__)
# Create an executable from the source files
add_executable(test_app ${SOURCES})

if(SDL2_FOUND)
add_definitions(-D__RENDER__=1)
# Link SDL2 to the executable
target_link_libraries(test_app SDL2::SDL2)
endif()

option(ENABLE_ASAN "Enable AddressSanitizer (ASAN)" ON)
if(ENABLE_ASAN)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    target_compile_options(test_app PRIVATE -fsanitize=address -fno-omit-frame-pointer -g -ggdb -Wall -fPIC)
    target_link_options(test_app PRIVATE -fsanitize=address -fno-omit-frame-pointer)
endif()