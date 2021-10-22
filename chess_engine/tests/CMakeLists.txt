
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

# Tests need to be added as executables first
add_executable(testlib chess_engine_tests.cpp)

target_compile_features(testlib PRIVATE cxx_std_17)

target_precompile_headers(testlib
  PRIVATE
    <algorithm>
    <cstdint>
    <filesystem>
    <fstream>
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
)

# Should be linked to the main library
target_link_libraries(testlib PRIVATE chess_engine_lib Catch2::Catch2)

include_directories(../include)

add_test(NAME test_chess_engine COMMAND testlib)

