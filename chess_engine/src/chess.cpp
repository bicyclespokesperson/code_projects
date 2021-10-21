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
#include "player.h"

/**
 * Play the chess game.
 * @param argc
 * @param argv
 * @return
 */
int main(int /* argc */, char* /* argv*/[])
{
  Game game;
  game.run();

  std::cout << "Thanks for playing!" << std::endl;
  return 0;
}
