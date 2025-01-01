cmake_minimum_required(VERSION 3.10)
project(AlgoBaseTest)

# GoogleTest
find_package(GTest REQUIRED)
# find_package(GMock REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS}  ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/Utils/include )

# Find all the test source files starting with "test_"
file(GLOB TEST_SOURCES "test_*.cpp")

file(GLOB CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/tests/src/TaskProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/Utils/src/TaskQueue.cpp
)

# Source files
add_executable(test_algo_base ${TEST_SOURCES} ${CORE_SOURCES})

# Add option to control AddressSanitizer
option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    set(ASAN 1)
    set(CMAKE_BUILD_TYPE DEBUG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

    # Enable AddressSanitizer
    target_compile_options(test_algo_base PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(test_algo_base PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    # Set compiler flags for debugging information
    target_compile_options(test_algo_base PRIVATE -g -ggdb -Wall -fPIC)

endif()

# Link libraries
target_link_libraries(test_algo_base ${GTEST_LIBRARIES} pthread gtest gmock)
