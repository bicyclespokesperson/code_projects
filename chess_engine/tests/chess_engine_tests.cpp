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

static const char c_fischer_spassky_result[] = " 8  ___ ___ ___ ___ ___ ___ ___ ___ \
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

static const char c_sigrist_result[] = " 8  R_b ___ ___ ___ ___ ___ ___ ___ \
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

TEST_CASE("A board can be constructed from a pgn file"
          "[board]")
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

TEST_CASE("A board should be initially setup" "[board]")
{
  Board board;

  // Sampling of pieces to ensure the board is setup initially
  REQUIRE(board.square_at({0, 0}).occupier() == Piece::rook);
  REQUIRE(board.square_at({0, 0}).occupier_color() == Color::white);
  REQUIRE(board.square_at({4, 0}).occupier() == Piece::king);
  REQUIRE(board.square_at({4, 0}).occupier_color() == Color::white);
  REQUIRE(board.square_at({3, 0}).occupier() == Piece::queen);
  REQUIRE(board.square_at({3, 0}).occupier_color() == Color::white);

  REQUIRE(board.square_at({3, 3}).occupier() == Piece::empty);

  REQUIRE(board.square_at({6, 6}).occupier() == Piece::pawn);
  REQUIRE(board.square_at({6, 6}).occupier_color() == Color::black);
  REQUIRE(board.square_at({6, 7}).occupier() == Piece::knight);
  REQUIRE(board.square_at({6, 7}).occupier_color() == Color::black);
  REQUIRE(board.square_at({5, 7}).occupier() == Piece::bishop);
  REQUIRE(board.square_at({5, 7}).occupier_color() == Color::black);
}

TEST_CASE("A board should prevent illegal moves" "[board]")
{
  Board board;

  REQUIRE(!board.try_move_algebraic("d5")); // Black cannot move first
  REQUIRE(!board.try_move_algebraic("Bc4")); // Bishops cannot jump over pieces
  REQUIRE(board.try_move_algebraic("e4"));

  REQUIRE(board.try_move_algebraic("e6"));

  REQUIRE(!board.try_move_algebraic("e3")); // Pawns cannot move backwards
  REQUIRE(!board.try_move_algebraic("exf5")); // Pawns cannot capture to an empty square
  REQUIRE(board.try_move_algebraic("e5"));

  REQUIRE(board.try_move_algebraic("f5"));

  REQUIRE(!board.try_move_algebraic("e6")); // Pawns cannot capture forward
  REQUIRE(!board.try_move_algebraic("exd6")); // Test en passent
  REQUIRE(board.try_move_algebraic("exf6"));

  REQUIRE(board.try_move_algebraic("a6"));

  REQUIRE(board.try_move_algebraic("f7+"));

  REQUIRE(!board.try_move_algebraic("Nc6")); // A move cannot leave the king in check
  REQUIRE(board.try_move_algebraic("Kxf7"));

  REQUIRE(board.try_move_algebraic("Qf3+"));

  REQUIRE(board.try_move_algebraic("Nf6")); // Block the check

  REQUIRE(board.try_move_algebraic("Nh3"));

  REQUIRE(!board.try_move_algebraic("Ng4")); // A move cannot leave the king in check
  REQUIRE(board.try_move_algebraic("d5"));

  REQUIRE(board.try_move_algebraic("Be2"));

  REQUIRE(!board.try_move_algebraic("a4")); // Pawns cannot move two spaces besides their first move
  REQUIRE(board.try_move_algebraic("Be7")); // Pawns cannot move two spaces besides their first move

  REQUIRE(board.try_move_algebraic("O-O"));

  REQUIRE(board.try_move_algebraic("Ke8"));

  REQUIRE(!board.try_move_algebraic("Qg5")); // Queens cannot move like Knights
  REQUIRE(board.try_move_algebraic("Qg3")); // Queens cannot move like Knights

  REQUIRE(!board.try_move_algebraic("O-O")); // Cannot castle after the king has already moved
  REQUIRE(board.try_move_algebraic("Rf8"));

  REQUIRE(board.try_move_algebraic("Bb5"));

  REQUIRE(board.try_move_algebraic("xb5"));
}

