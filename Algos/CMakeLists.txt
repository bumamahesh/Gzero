# Create AlgoCore library
file(GLOB CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/Utils/src/TaskQueue.cpp
    ${CMAKE_SOURCE_DIR}/src/AlgoMetadata.cpp
    ${CMAKE_SOURCE_DIR}/src/AlgoBase.cpp
    ${CMAKE_SOURCE_DIR}/src/AlgoRequest.cpp
)

add_library(AlgoCore STATIC ${CORE_SOURCES})
set_target_properties(AlgoCore PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Set include directories
set(COMMON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Utils/include
)

target_include_directories(AlgoCore PUBLIC ${COMMON_INCLUDE_DIRS})

# Link AlgoCore with AlgoUtils
target_link_libraries(AlgoCore PRIVATE AlgoUtils)

install(TARGETS AlgoCore DESTINATION lib)