cmake_minimum_required(VERSION 3.15)

project(test)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} --std=c++20 -Wall -fsanitize=thread")

add_subdirectory(libs/json)

set(SOURCES src/PingPong.cpp src/PlayGame.cpp)
set(HEADERS include/PingPong.h )
set(HEADERS
    include/PingPong.h)
set(SOURCES
    ${HEADERS}
    src/PingPong.cpp
    src/PlayGame.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_executable(ping-pong ${SOURCES})
target_include_directories(ping-pong
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src" 
    "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(ping-pong  nlohmann_json::nlohmann_json)
