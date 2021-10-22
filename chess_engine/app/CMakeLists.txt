
add_executable(chess_game chess.cpp)
target_compile_features(chess_game PRIVATE cxx_std_20)

target_include_directories(chess_game PUBLIC ../include)

target_precompile_headers(chess_game
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

target_link_libraries(chess_game PRIVATE chess_engine_lib )
