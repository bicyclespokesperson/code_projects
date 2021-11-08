
# Download catch2 (Need to include FetchContent in the parent CMakeLists.txt file)
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

# Tests need to be added as executables first
add_executable(general_tests chess_engine_tests.cpp)
add_executable(perft_tests perft_tests.cpp)

# Compiler settings for the test code

include_directories(../include)

target_compile_features(general_tests PRIVATE cxx_std_20)
target_compile_features(perft_tests PRIVATE cxx_std_20)

target_precompile_headers(perft_tests
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
    <sstream>
    <unordered_map>
    <vector>
)

target_precompile_headers(general_tests
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
    <sstream>
    <unordered_map>
    <vector>
)

# Need to link with the main library to build/test it, and also link with Catch2
target_link_libraries(general_tests PRIVATE chess_engine_lib Catch2::Catch2)

target_link_libraries(perft_tests PRIVATE chess_engine_lib Catch2::Catch2)

# Add this so we can use "make test"
# CMAKE_SOURCE_DIR == the top level of the tree
# Name = arg to make test
# Command = executable make test will run
add_test(NAME general_tests COMMAND general_tests  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
add_test(NAME my_perft_tests COMMAND perft_tests  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
