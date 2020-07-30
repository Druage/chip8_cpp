cmake_minimum_required(VERSION 3.16)
project(chip8_cpp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/app)
add_subdirectory(src/lib)
add_subdirectory(src/test)

enable_testing()
add_test(NAME all_tests COMMAND testchip8)