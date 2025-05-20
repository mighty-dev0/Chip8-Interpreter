cmake_minimum_required(VERSION 3.26)
project(CHIP-8_Interpreter LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(CHIP8-Interpreter main.cpp chip8.cpp platform.cpp)

target_sources(CHIP8-Interpreter
PRIVATE
    main.cpp
    chip8.h
    chip8.cpp
    platform.h
    platform.cpp
)

target_compile_options(CHIP8-Interpreter PRIVATE -Wall)

target_link_libraries(CHIP8-Interpreter PRIVATE SDL3::SDL3)