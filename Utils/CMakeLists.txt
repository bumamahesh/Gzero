
add_library(AlgoUtils STATIC
    src/ConfigParser.cpp
    src/KpiMonitor.cpp
    src/Log.cpp
    src/RequestMonitor.cpp
 #   src/TaskQueue.cpp
    src/Utils.cpp
    src/ThreadWrapper.cpp
)

set_target_properties(AlgoUtils PROPERTIES POSITION_INDEPENDENT_CODE ON)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fPIC)
endif()
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
target_include_directories(AlgoUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
install(TARGETS AlgoUtils DESTINATION lib)