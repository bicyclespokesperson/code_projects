/*
 * File:   chess.cpp
 * Author: 1697518
 *
 * This is the main method for the chess game.
 * Created on March 24, 2013, 5:02 PM
 */

#include "chess.h"
#include "board.h"
#include "game.h"
#include "my_assert.h"
#include "player.h"
#include <sstream>

/**
 * Play the chess game.
 * @param argc
 * @param argv
 * @return
 */
int main(int /* argc */, char* /* argv*/[])
{
#if 0
  Game game;
  game.run();
#endif

  static const std::string fen_string
  {"r3k2r/qppb1pp1/2nbpn2/1B1N4/pP1PP1qP/P1P3N1/3BQP2/R3K2R b Qk b3 0 19"};

  auto board = Board::from_fen(fen_string);

  std::stringstream out;
  board->display(out);
  board->display(std::cout);

  return 0;
}
