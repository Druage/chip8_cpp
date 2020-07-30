cmake_minimum_required(VERSION 3.15)

project(testchip8)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} include/catch.hpp chip8_emu_test.cpp include/utils.h)

target_link_libraries(${PROJECT_NAME} libchip8)

