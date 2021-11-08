#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "board.h"
#include "move_generator.h"

// Perft results source: https://www.chessprogramming.org/Perft_Results

TEST_CASE("Perft position 1", "[Move_generator]")
{
  Board board;
  uint64_t actual = perft(4, board);

  uint64_t expected{197'281};
  REQUIRE(actual == expected);
}

TEST_CASE("Perft position 2", "[Move_generator]")
{
  std::string fen_str = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
  auto board = *Board::from_fen(fen_str);

  uint64_t actual = perft(4, board);
  
  uint64_t expected{4'085'603};
  REQUIRE(actual == expected);
}

TEST_CASE("Perft position 3", "[Move_generator]")
{
  std::string fen_str = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ";
  auto board = *Board::from_fen(fen_str);

  uint64_t actual = perft(5, board);
  
  uint64_t expected{674'624};
  REQUIRE(actual == expected);
}

TEST_CASE("Perft position 4", "[Move_generator]")
{
  std::string fen_str = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  auto board = *Board::from_fen(fen_str);

  uint64_t actual = perft(4, board);
  
  uint64_t expected{422'333};
  REQUIRE(actual == expected);
}

TEST_CASE("Perft position 5", "[Move_generator]")
{
  std::string fen_str = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
  auto board = *Board::from_fen(fen_str);

  uint64_t actual = perft(4, board);
  
  uint64_t expected{2'103'487};
  REQUIRE(actual == expected);
}

TEST_CASE("Perft position 6", "[Move_generator]")
{
  std::string fen_str = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
  auto board = *Board::from_fen(fen_str);

  uint64_t actual = perft(4, board);
  
  uint64_t expected{3'894'594};
  REQUIRE(actual == expected);
}
