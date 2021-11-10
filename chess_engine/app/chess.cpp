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
#if 1
  if (argc > 2)
  {
    // h -> human, c -> computer
    std::cerr << "Usage: chess_game [hh/hc/ch/cc]\n";
    exit(-1);
  }

  Game game;
  std::string param{"hc"};
  if (argc == 2)
  {
    param = argv[1];
  }


  if (param == "hh")
  {
    game.player_vs_player();
  }
  else if (param == "hc")
  {
    game.player_vs_computer(Color::white);
  }
  else if (param == "ch")
  {
    game.player_vs_computer(Color::black);
  }
  else if (param == "cc")
  {
    game.computer_vs_computer();
  }
  else
  {
    // h -> human, c -> computer
    std::cerr << "Usage: chess [hh/hc/ch/cc]\n";
    exit(-1);
  }
#else

#endif
  return 0;
}
