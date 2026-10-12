cmake_minimum_required(VERSION 3.10)
project(r-type_client)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
 

include_directories(
    src
)

set(SOURCE_FILES
    src/main.cpp
)
 
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

find_package(Threads REQUIRED)

add_executable(r-type_server
    src/server/main.cpp
    src/server/server.cpp
    src/server/network/network.cpp
    src/server/game/game_loop.cpp
    src/server/input/input_queue.cpp
)
target_include_directories(r-type_server PRIVATE src/server)
target_compile_features(r-type_server PRIVATE cxx_std_17)
target_link_libraries(r-type_server PRIVATE Threads::Threads)
