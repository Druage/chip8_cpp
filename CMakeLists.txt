cmake_minimum_required(VERSION 3.16)
project(chip8_cpp)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Debug/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Debug/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Debug/lib")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Release/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Release/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/Release/lib")
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

message(${CMAKE_BUILD_TYPE})

add_subdirectory(src/app)
add_subdirectory(src/app_sdl)
add_subdirectory(src/lib)
add_subdirectory(src/test)

enable_testing()
add_test(NAME all_tests COMMAND testchip8 -r xml -d yes --order lex)

