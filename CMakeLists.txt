cmake_minimum_required(VERSION 3.1)
project(test_server LANGUAGES C CXX)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

include_directories(${PROJECT_SOURCE_DIR}/src/headers)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(server ${SOURCES})

target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)