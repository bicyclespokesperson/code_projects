#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "board.h"

namespace
{
  std::optional<std::string> read_file_contents(std::string const& filename)
{
  std::ifstream infile{filename};
  if (!infile)
  {
    return {};
  }
  return std::string{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
}
} // namespace


static const char c_fischer_spassky_result[] = 
" 8  ___ ___ ___ ___ ___ ___ ___ ___ \
\
7  ___ ___ ___ ___ ___ ___ ___ ___ \
\
6  ___ ___ ___ ___ R_w ___ P_b ___ \
\
5  ___ ___ K_b ___ ___ ___ P_b ___ \
\
4  ___ P_b ___ ___ ___ ___ P_w ___ \
\
3  ___ P_w ___ B_b ___ P_w ___ ___ \
\
2  ___ ___ ___ K_w ___ N_b ___ ___ \
\
1  ___ ___ ___ ___ ___ ___ ___ ___ \
\
    A   B   C   D   E   F   G   H  ";

static const char c_sigrist_result[] = 
" 8  R_b ___ ___ ___ ___ ___ ___ ___ \
\
7  P_b P_b P_b ___ ___ ___ ___ P_b \
\
6  ___ ___ ___ ___ B_w ___ ___ K_b \
\
5  ___ ___ ___ ___ ___ ___ ___ ___ \
\
4  ___ ___ ___ P_w ___ Q_w ___ ___ \
\
3  ___ ___ N_w ___ N_b ___ ___ ___ \
\
2  P_w P_w P_w ___ ___ ___ P_w P_w \
\
1  R_w ___ ___ ___ ___ ___ K_w ___ \
\
    A   B   C   D   E   F   G   H  \
";

TEST_CASE("A board can be constructed from a pgn file" "[board]")
{
  std::string const pgn_filename{"./data/fischer_spassky.pgn"};
  auto contents = read_file_contents(pgn_filename);
  REQUIRE(contents.has_value());

  auto board = Board::from_pgn(*contents);

  REQUIRE(board.has_value());

  std::stringstream out;
  board->display(out);

  std::string result = out.str();
  std::string expected = c_fischer_spassky_result;
  result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
  expected.erase(std::remove_if(expected.begin(), expected.end(), isspace), expected.end());

  // Compare ignoring whitespace
  REQUIRE(result == expected);
}

TEST_CASE("Another pgn file")
{
  std::string const pgn_filename{"./data/sigrist.pgn"};
  auto contents = read_file_contents(pgn_filename);
  REQUIRE(contents.has_value());

  auto board = Board::from_pgn(*contents);
  REQUIRE(board.has_value());

  std::stringstream out;
  board->display(out);

  std::string result = out.str();
  std::string expected = c_sigrist_result;
  result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
  expected.erase(std::remove_if(expected.begin(), expected.end(), isspace), expected.end());

  // Compare ignoring whitespace
  REQUIRE(result == expected);
}

