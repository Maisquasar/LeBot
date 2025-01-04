# minimum CMake version required
cmake_minimum_required(VERSION 3.15)
# Project name, version and description
project(discord-bot VERSION 1.0 DESCRIPTION "A discord bot")
 
# Add DPP as dependency
add_subdirectory(libs/DPP)
 
# Create an executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/AudioPlayer.cpp
    src/AudioPlayer.h
    src/Bot.cpp
    src/Bot.h
    src/Event.cpp
    src/Event.h
    src/ThreadManager.cpp
    src/ThreadManager.h
)
 
# Linking libraries
target_link_libraries(${PROJECT_NAME}
    dpp
    ${CMAKE_SOURCE_DIR}/libs/opusfile/lib/opusfile.lib
    ${CMAKE_SOURCE_DIR}/libs/libogg/lib/ogg.lib
    ${CMAKE_SOURCE_DIR}/libs/opus/lib/opus.lib
)
 
# Specify includes
target_include_directories(${PROJECT_NAME} PRIVATE
    libs/DPP/include
    libs/opusfile/include
    libs/libogg/include
    libs/opus/include/opus
)
 
# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)