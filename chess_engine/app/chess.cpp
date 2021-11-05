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
int main(int /* argc */, char* /* argv*/[])
{
#if 1
  Game game;
  game.run();
#endif

  std::cout << "Kingside castling white\n";
  print_bitboard_with_squares({"f1", "g1"});
  std::cout << "Queenside castling white\n";
  print_bitboard_with_squares({"d1", "c1", "b1"});

  std::cout << "Kingside castling black\n";
  print_bitboard_with_squares({"f8", "g8"});

  std::cout << "Queenside castling black\n";
  print_bitboard_with_squares({"d8", "c8", "b8"});
  return 0;
}
