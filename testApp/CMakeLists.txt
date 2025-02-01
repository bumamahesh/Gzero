cmake_minimum_required(VERSION 3.10)
project(test_app)

# Set the C++ standard (if you're using C++)
set(CMAKE_CXX_STANDARD 11)

include_directories(../include/ ..//Utils/include )
# Find the SDL2 package
find_package(SDL2 REQUIRED)

# Define the source files
set(SOURCES
    main.cpp
    ../src/AlgoRequest.cpp
    ../src/AlgoDecisionManager.cpp
    ../src/AlgoMetadata.cpp
)

# Create an executable from the source files
add_executable(test_app ${SOURCES})

# Link SDL2 to the executable
target_link_libraries(test_app SDL2::SDL2)
