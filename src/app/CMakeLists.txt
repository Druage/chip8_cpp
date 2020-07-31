cmake_minimum_required(VERSION 3.16)
project(appchip8)

set(CMAKE_CXX_STANDARD 20)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(COPY roms DESTINATION ${CMAKE_SOURCE_DIR}/build/Debug/bin/)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    file(COPY roms DESTINATION ${CMAKE_SOURCE_DIR}/build/Release/bin/)
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Quick Multimedia REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} libchip8)

