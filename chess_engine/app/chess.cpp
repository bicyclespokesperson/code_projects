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

#include <fstream>

std::string read_file_contents(std::string const& filename)
{
  std::ifstream infile{filename};
  if (!infile)
  {
    MY_ASSERT(false, "File could not be opened");
  }
  return {std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
}

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

#if 1

  std::string pgn_filename{"./data/fischer_spassky.pgn"};

  auto b = Board::from_pgn(read_file_contents(pgn_filename));
  if (b)
  {
    b->display(std::cout);
  }
#endif

  return 0;
}
