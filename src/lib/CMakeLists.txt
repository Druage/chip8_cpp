cmake_minimum_required(VERSION 3.15)

project(libchip8)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} chip8_emu.cpp chip8_emu.h)

target_include_directories(${PROJECT_NAME} PUBLIC .)