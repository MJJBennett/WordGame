cmake_minimum_required(VERSION 3.1)

# Temporary, these should be default arguments
set(WordGame_Build_TestClient FALSE)
set(WordGame_Build_TestServer FALSE)
cmake_policy(SET CMP0074 NEW) # Something about _ROOT

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
message("Building in debug mode.")
set(WORDGAME_DEBUG 1)
set(WORDGAME_RELEASE 0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
message("Building in release mode.")
set(WORDGAME_DEBUG 0)
set(WORDGAME_RELEASE 1)
endif()

# Declare our projects
project(WordGame LANGUAGES CXX)
if(WordGame_Build_TestClient)
project(TestClient LANGUAGES CXX)
endif()
if(WordGame_Build_TestServer)
project(TestServer LANGUAGES CXX)
endif()

# Set up our build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(JSON_BuildTests OFF CACHE INTERNAL "")

if (APPLE)
add_definitions(-g -stdlib=libc++)
elseif (WIN32)
#add_compile_definitions(_WIN32_WINNT=0x0601)
add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

if (WORDGAME_RELEASE)
set(SFML_STATIC_LIBRARIES TRUE)
set(WORDGAME_STATIC_BOOST TRUE)
endif ()
if (WORDGAME_STATIC_BOOST)
set(Boost_USE_STATIC_LIBS ON)
endif ()

# Find our libraries
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(Boost 1.71 COMPONENTS program_options REQUIRED)
find_package(Threads)
add_subdirectory(include/deps/nlohmann_json)

# Add include directories
include_directories("${CMAKE_SOURCE_DIR}/include/")
include_directories("${CMAKE_SOURCE_DIR}/src/web/")
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

# Build the application
add_executable(WordGame
    # Add headers
    ${CMAKE_SOURCE_DIR}/include/game/item.hpp
    ${CMAKE_SOURCE_DIR}/include/game/multipliers.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/tools.hpp
     
    # Add class and function definitions
    ${CMAKE_SOURCE_DIR}/src/table.cpp ${CMAKE_SOURCE_DIR}/include/framework/table.hpp
    ${CMAKE_SOURCE_DIR}/src/render.cpp ${CMAKE_SOURCE_DIR}/include/framework/render.hpp
    ${CMAKE_SOURCE_DIR}/src/window_context.cpp ${CMAKE_SOURCE_DIR}/include/framework/window_context.hpp
    ${CMAKE_SOURCE_DIR}/src/window_io.cpp ${CMAKE_SOURCE_DIR}/include/framework/window_io.hpp
    ${CMAKE_SOURCE_DIR}/src/window_items.cpp ${CMAKE_SOURCE_DIR}/include/framework/window_item.hpp
    ${CMAKE_SOURCE_DIR}/src/resourcemanager.cpp ${CMAKE_SOURCE_DIR}/include/framework/resourcemanager.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/resource.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/file_io.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/json_io.hpp
    ${CMAKE_SOURCE_DIR}/include/graphics/display_mode.hpp

    ${CMAKE_SOURCE_DIR}/src/board.cpp ${CMAKE_SOURCE_DIR}/include/game/board.hpp
    ${CMAKE_SOURCE_DIR}/src/context.cpp ${CMAKE_SOURCE_DIR}/include/game/context.hpp
    ${CMAKE_SOURCE_DIR}/src/game_io.cpp ${CMAKE_SOURCE_DIR}/include/game/game_io.hpp

    ${CMAKE_SOURCE_DIR}/src/dev/devclient.cpp ${CMAKE_SOURCE_DIR}/src/dev/devclient.hpp

    ${CMAKE_SOURCE_DIR}/src/web/connections.cpp ${CMAKE_SOURCE_DIR}/src/web/connections.hpp
    ${CMAKE_SOURCE_DIR}/src/web/ws_session.cpp ${CMAKE_SOURCE_DIR}/src/web/ws_session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/session.cpp ${CMAKE_SOURCE_DIR}/src/web/session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/server.cpp ${CMAKE_SOURCE_DIR}/src/web/server.hpp
    ${CMAKE_SOURCE_DIR}/src/web/client.cpp ${CMAKE_SOURCE_DIR}/src/web/client.hpp

    ${CMAKE_SOURCE_DIR}/src/webclient.cpp ${CMAKE_SOURCE_DIR}/include/framework/webclient.hpp

    ${CMAKE_SOURCE_DIR}/src/application.cpp ${CMAKE_SOURCE_DIR}/include/application.hpp 

    # Add main (entry point)
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

if(WORDGAME_RELEASE)
#target_compile_options(WordGame PUBLIC /MT)
#target_link_options(WordGame PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
message("Adding boost library directory: " ${Boost_LIBRARY_DIR})
#link_directories(${Boost_LIBRARY_DIR})
endif()

if (WORDGAME_DEBUG AND APPLE)
    target_compile_options(WordGame PRIVATE -Werror -Wall -Wextra)
endif ()

# Link the application
target_link_libraries(WordGame PUBLIC sfml-graphics)
target_link_libraries(WordGame PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(WordGame PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(WordGame PRIVATE ${Boost_LIBRARIES})

if (WordGame_Build_TestServer)
add_executable(TestServer
    ${CMAKE_SOURCE_DIR}/src/web/ws_session.cpp ${CMAKE_SOURCE_DIR}/src/web/ws_session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/session.cpp ${CMAKE_SOURCE_DIR}/src/web/session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/server.cpp ${CMAKE_SOURCE_DIR}/src/web/server.hpp ${CMAKE_SOURCE_DIR}/src/web/testserver.cpp
)
#target_link_libraries(TestServer PUBLIC sfml-graphics)
target_link_libraries(TestServer PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(TestServer PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TestServer PRIVATE ${Boost_LIBRARIES})
endif()

if (WordGame_Build_TestClient)
add_executable(TestClient
    ${CMAKE_SOURCE_DIR}/src/web/client.cpp ${CMAKE_SOURCE_DIR}/src/web/client.hpp ${CMAKE_SOURCE_DIR}/src/web/testclient.cpp
)
#target_link_libraries(TestClient PUBLIC sfml-graphics)
target_link_libraries(TestClient PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(TestClient PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TestClient PRIVATE ${Boost_LIBRARIES})
endif()
