cmake_minimum_required(VERSION 3.10)
project(AlgoBaseTest)

# Enable testing
enable_testing()

# Register the test executable with CTest
add_test(NAME GzeroUnitTest COMMAND GzeroUnitTest)

# GoogleTest
find_package(GTest REQUIRED)
# find_package(GMock REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS}  ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/Utils/include  ${CMAKE_SOURCE_DIR}/test/)

# Find all the test source files starting with "test_"
file(GLOB TEST_SOURCES "test_*.cpp")

file(GLOB CORE_SOURCES
    #${CMAKE_SOURCE_DIR}/tests/src/TaskProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/*.cpp
)

# Source files
add_executable(GzeroUnitTest ${TEST_SOURCES} ${CORE_SOURCES})

# Add option to control AddressSanitizer
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
    set(ASAN 1)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

    # Enable AddressSanitizer
    target_compile_options(GzeroUnitTest PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(GzeroUnitTest PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    # Set compiler flags for debugging information
    target_compile_options(GzeroUnitTest PRIVATE -g -ggdb -Wall -fPIC)

endif()

target_compile_options(GzeroUnitTest PRIVATE  -O1 -g )

# Link libraries
target_link_libraries(GzeroUnitTest ${GTEST_LIBRARIES} pthread gtest gmock)
