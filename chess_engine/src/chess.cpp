/* 
 * File:   chess.cpp
 * Author: 1697518
 *
 * This is the main method for the chess game.
 * Created on March 24, 2013, 5:02 PM
 */

#include <cstdlib>

#include "Player.h"
#include "King.h"
#include "Pawn.h"
#include "Game.h"
#include "Board.h"
#include "chess.h"
#include "thread"

using namespace std;

/**
 * Play the chess game.
 * @param argc
 * @param argv
 * @return 
 */
int main(int /* argc */, char* /* argv*/ [])
{
    Game::initialize();
    
    Board::getBoard().display(cout);
    
    // Player.makeMove() will return false if the player resigns
    while (Game::getNextPlayer().makeMove())
    {
        Board::getBoard().display(cout);
    }
    Board::getBoard().display(cout);
    
    cout << "Thanks for playing!" << endl;
    return 0;
}

