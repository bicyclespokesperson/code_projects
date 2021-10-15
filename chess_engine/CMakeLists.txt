
# $ cmake CMakeLists.txt
# $ make

cmake_minimum_required(VERSION 3.16)

project (chess_engine)

set(CMAKE_BUILD_TYPE Debug)

# Create a compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Place the complied executable in the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(chess_engine ${SOURCES})

target_precompile_headers(chess_engine
  PRIVATE
    <algorithm>
    <cstdint>
    <filesystem>
    <iostream>
    <map>
    <numeric>
    <random>
    <ranges>
    <set>
    <span>
    <string>
    <unordered_map>
    <vector>
    <catch_amalgamated.hpp>
)

set_target_properties(chess_engine PROPERTIES
            CXX_STANDARD 20
            CXX_EXTENSIONS OFF
            )

if(MSVC)
  target_compile_options(chess_engine PRIVATE /W4 /WX)
else()
  target_compile_options(chess_engine PRIVATE -Wall -Wextra -pedantic -Wno-missing-braces)
endif()
