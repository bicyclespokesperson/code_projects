/* 
 * File:   chess.cpp
 * Author: 1697518
 *
 * This is the main method for the chess game.
 * Created on March 24, 2013, 5:02 PM
 */

#include "board.h"
#include "chess.h"
#include "game.h"
#include "king.h"
#include "pawn.h"
#include "player.h"


/**
 * Play the chess game.
 * @param argc
 * @param argv
 * @return 
 */
int main(int /* argc */, char* /* argv*/ [])
{
    Game::initialize();
    
    Board::getBoard().display(std::cout);
    
    // Player.makeMove() will return false if the player resigns
    while (Game::getNextPlayer().makeMove())
    {
        Board::getBoard().display(std::cout);
    }
    Board::getBoard().display(std::cout);
    
    std::cout << "Thanks for playing!" << std::endl;
    return 0;
}

