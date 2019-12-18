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
    # Add headers
    ${CMAKE_SOURCE_DIR}/include/game/item.hpp
    ${CMAKE_SOURCE_DIR}/include/game/multipliers.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/tools.hpp
     
    # Add class and function definitions
    ${CMAKE_SOURCE_DIR}/src/table.cpp ${CMAKE_SOURCE_DIR}/include/framework/table.hpp
    ${CMAKE_SOURCE_DIR}/src/render.cpp ${CMAKE_SOURCE_DIR}/include/framework/render.hpp
    ${CMAKE_SOURCE_DIR}/src/resourcemanager.cpp ${CMAKE_SOURCE_DIR}/include/framework/resourcemanager.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/resource.hpp

    ${CMAKE_SOURCE_DIR}/src/context.cpp ${CMAKE_SOURCE_DIR}/include/game/context.hpp

    # Add main (entry point)
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)


target_link_libraries(WordGame sfml-graphics)
