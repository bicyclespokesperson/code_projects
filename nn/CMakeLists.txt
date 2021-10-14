
# $ cmake CMakeLists.txt
# $ make

cmake_minimum_required(VERSION 3.16)

project (neural_network)

set(CMAKE_BUILD_TYPE Debug)

# Place the complied executable in the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(neural_network ${SOURCES})

target_precompile_headers(neural_network
  PRIVATE
    <algorithm>
    <cstdint>
    <filesystem>
    <iostream>
    <map>
    <numeric>
    <random>
    <ranges>
    <span>
    <string>
    <unordered_map>
    <vector>
    <catch_amalgamated.hpp>
)

set_target_properties(neural_network PROPERTIES
            CXX_STANDARD 20
            CXX_EXTENSIONS OFF
            )

if(MSVC)
  target_compile_options(neural_network PRIVATE /W4 /WX)
else()
  target_compile_options(neural_network PRIVATE -Wall -Wextra -pedantic -Wno-missing-braces)
endif()
