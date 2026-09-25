# AzerothCore module: mod-camping
cmake_minimum_required(VERSION 3.16)

set(MOD_CAMPING_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_library(mod-camping INTERFACE)

target_sources(mod-camping INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CampingMgr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CampingScripts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camping_loader.cpp
)

target_include_directories(mod-camping INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mod-camping INTERFACE
    game
)
