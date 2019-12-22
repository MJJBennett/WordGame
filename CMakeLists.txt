cmake_minimum_required(VERSION 3.1)
project(WordGame LANGUAGES CXX)

project(TestClient LANGUAGES CXX)
project(TestServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_definitions(-g -stdlib=libc++)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(Boost 1.71 COMPONENTS program_options REQUIRED)
find_package(Threads)
add_subdirectory(include/deps/nlohmann_json)

include_directories("${CMAKE_SOURCE_DIR}/include/")
include_directories("${CMAKE_SOURCE_DIR}/src/web/")
include_directories(${Boost_INCLUDE_DIR})

add_executable(WordGame
    # Add headers
    ${CMAKE_SOURCE_DIR}/include/game/item.hpp
    ${CMAKE_SOURCE_DIR}/include/game/multipliers.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/tools.hpp
     
    # Add class and function definitions
    ${CMAKE_SOURCE_DIR}/src/table.cpp ${CMAKE_SOURCE_DIR}/include/framework/table.hpp
    ${CMAKE_SOURCE_DIR}/src/render.cpp ${CMAKE_SOURCE_DIR}/include/framework/render.hpp
    ${CMAKE_SOURCE_DIR}/src/windowcontext.cpp ${CMAKE_SOURCE_DIR}/include/framework/windowcontext.hpp
    ${CMAKE_SOURCE_DIR}/src/window_io.cpp ${CMAKE_SOURCE_DIR}/include/framework/window_io.hpp
    ${CMAKE_SOURCE_DIR}/src/window_items.cpp ${CMAKE_SOURCE_DIR}/include/framework/window_item.hpp
    ${CMAKE_SOURCE_DIR}/src/resourcemanager.cpp ${CMAKE_SOURCE_DIR}/include/framework/resourcemanager.hpp
    ${CMAKE_SOURCE_DIR}/include/framework/resource.hpp
    ${CMAKE_SOURCE_DIR}/include/graphics/display_mode.hpp

    ${CMAKE_SOURCE_DIR}/src/context.cpp ${CMAKE_SOURCE_DIR}/include/game/context.hpp

    ${CMAKE_SOURCE_DIR}/src/web/ws_session.cpp ${CMAKE_SOURCE_DIR}/src/web/ws_session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/session.cpp ${CMAKE_SOURCE_DIR}/src/web/session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/server.cpp ${CMAKE_SOURCE_DIR}/src/web/server.hpp
    ${CMAKE_SOURCE_DIR}/src/web/client.cpp ${CMAKE_SOURCE_DIR}/src/web/client.hpp

    ${CMAKE_SOURCE_DIR}/src/webclient.cpp ${CMAKE_SOURCE_DIR}/include/framework/webclient.hpp

    ${CMAKE_SOURCE_DIR}/src/application.cpp ${CMAKE_SOURCE_DIR}/include/application.hpp 

    # Add main (entry point)
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(WordGame PUBLIC sfml-graphics)
target_link_libraries(WordGame PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(WordGame PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(WordGame PRIVATE ${Boost_LIBRARIES})

add_executable(TestServer
    ${CMAKE_SOURCE_DIR}/src/web/ws_session.cpp ${CMAKE_SOURCE_DIR}/src/web/ws_session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/session.cpp ${CMAKE_SOURCE_DIR}/src/web/session.hpp
    ${CMAKE_SOURCE_DIR}/src/web/server.cpp ${CMAKE_SOURCE_DIR}/src/web/server.hpp ${CMAKE_SOURCE_DIR}/src/web/testserver.cpp
)
#target_link_libraries(TestServer PUBLIC sfml-graphics)
target_link_libraries(TestServer PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(TestServer PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TestServer PRIVATE ${Boost_LIBRARIES})

add_executable(TestClient
    ${CMAKE_SOURCE_DIR}/src/web/client.cpp ${CMAKE_SOURCE_DIR}/src/web/client.hpp ${CMAKE_SOURCE_DIR}/src/web/testclient.cpp
)
#target_link_libraries(TestClient PUBLIC sfml-graphics)
target_link_libraries(TestClient PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(TestClient PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TestClient PRIVATE ${Boost_LIBRARIES})
