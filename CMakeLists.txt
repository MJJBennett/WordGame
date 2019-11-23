cmake_minimum_required(VERSION 3.1)
project(WordGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif ()

add_definitions(-g -stdlib=libc++)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/include/")

add_executable(WordGame
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)


target_link_libraries(WordGame sfml-graphics)
