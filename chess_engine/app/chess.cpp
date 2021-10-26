/*
 * File:   chess.cpp
 * Author: 1697518
 *
 * This is the main method for the chess game.
 * Created on March 24, 2013, 5:02 PM
 */

#include "chess.h"
#include "board.h"
#include "bitboard.h"
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

  Bitboard b{0};
  b.set_square(0);
  b.set_square(1);
  b.set_square(8);
  b.set_square(63);
  std::cout << b;

  return 0;
}
