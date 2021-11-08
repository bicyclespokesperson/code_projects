/*
 * File:   chess.cpp
 * Author: 1697518
 *
 * This is the main method for the chess game.
 * Created on March 24, 2013, 5:02 PM
 */

#include "chess.h"
#include "bitboard.h"
#include "board.h"
#include "game.h"
#include "meneldor_engine.h"
#include "move_generator.h"
#include "my_assert.h"
#include "player.h"
#include <sstream>

void print_bitboard_with_squares(std::vector<std::string> const& squares)
{
  Bitboard bb;
  for (auto const& str : squares)
  {
    if (auto sq = Coordinates::from_str(str))
    {
      bb.set_square(*sq);
    }
    else
    {
      std::cout << "Invalid square: " << str << "\n";
    }
  }
  std::cout << bb << "\nhex: " << bb.hex_str() << "\n";
}


/**
 * Play the chess game.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
#if 0
  Game game;
  game.run();
#else

  if (argc != 2 && argc != 3)
  {
    std::cerr << "Usage: chess [depth] [fen (opt)]";
    exit(-1);
  }

  int depth = std::stoi(argv[1]);

  // Position 2 as described here. perft(1) should be 48, currently returning 46
  // https://www.chessprogramming.org/Perft_Results#Position_2
  std::string fen_position_1{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  std::string fen_position_2{"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"};

  std::string fen_position_3{"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"}; // CORRECT UP TO DEPTH 4, not depth 5

  std::string fen_position_4{"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"};
  std::string pos_4_sub_1 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 1 1";

  std::string fen_position_5{"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"};

  std::string fen_position_6{"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "};

  std::string fen = fen_position_1;
  if (argc == 3)
  {
    fen = argv[2];
  }

  Board board = *Board::from_fen(fen);

  constexpr bool print_moves = false;
  int result = perft(depth, board, print_moves);
  std::cout << "perft(" << std::to_string(depth) << ") = " << std::to_string(result) << "\n";

#endif
  return 0;
}
