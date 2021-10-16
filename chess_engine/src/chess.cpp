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
    
    Board::get_board().display(std::cout);
    
    // Player.make_move() will return false if the player resigns
    while (Game::get_next_player().make_move())
    {
        Board::get_board().display(std::cout);
    }
    Board::get_board().display(std::cout);
    
    std::cout << "Thanks for playing!" << std::endl;
    return 0;
}

