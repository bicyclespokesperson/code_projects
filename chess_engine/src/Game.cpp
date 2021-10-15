#include "Bishop.h"
#include "Board.h"
#include "Game.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Piece.h"
#include "Player.h"
#include "Queen.h"
#include "Rook.h"
#include "Square.h"
using namespace std;

// These will be initialized in the initialize method
Player* Game::_player1 = NULL;
Player* Game::_player2 = NULL;
Player* Game::_currentPlayer = _player1;

Game::Game() = default;

Game::~Game() = default;

Player& Game::getNextPlayer()
{
    // Since current player is initialized at null, this will return
    // player one for the first move.
    _currentPlayer = &(opponentOf(*_currentPlayer));

    return *_currentPlayer;
}

/**
 * Initialize the board by putting the pieces in the correct locations
 */
void Game::initialize()
{
    // Set the owner to null since they haven't been initialized yet. We'll add
    // them later
    King* blackKing = new King(*_player1, "black", (Board::getBoard().squareAt(4, 7)));
    King* whiteKing = new King(*_player2, "white", (Board::getBoard().squareAt(4, 0)));
    
    // Create the players
    _player1 = new Player("white", *whiteKing, *(new set<Piece*>));
    _player2 = new Player("black", *blackKing, *(new set<Piece*>));
    
    whiteKing->setOwner(*_player1);
    blackKing->setOwner(*_player2);
    
    // This here be beautiful code!
	Board::getBoard().squareAt(0, 0).setOccupier(*(new Rook (*_player1, "white", (Board::getBoard().squareAt(0, 0)))));
    Board::getBoard().squareAt(1, 0).setOccupier(*(new Knight(*_player1, "white", (Board::getBoard().squareAt(1, 0)))));
	Board::getBoard().squareAt(2, 0).setOccupier(*(new Bishop(*_player1, "white", (Board::getBoard().squareAt(2, 0)))));
	Board::getBoard().squareAt(3, 0).setOccupier(*(new Queen(*_player1, "white", (Board::getBoard().squareAt(3, 0)))));
	Board::getBoard().squareAt(4, 0).setOccupier(*whiteKing);
	Board::getBoard().squareAt(5, 0).setOccupier(*(new Bishop(*_player1, "white", (Board::getBoard().squareAt(5, 0)))));
	Board::getBoard().squareAt(6, 0).setOccupier(*(new Knight(*_player1, "white", (Board::getBoard().squareAt(6, 0)))));
	Board::getBoard().squareAt(7, 0).setOccupier(*(new Rook(*_player1, "white", (Board::getBoard().squareAt(7, 0)))));
	Board::getBoard().squareAt(0, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(0, 1)))));
	Board::getBoard().squareAt(1, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(1, 1)))));
	Board::getBoard().squareAt(2, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(2, 1)))));
	Board::getBoard().squareAt(3, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(3, 1)))));
	Board::getBoard().squareAt(4, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(4, 1)))));
	Board::getBoard().squareAt(5, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(5, 1)))));
	Board::getBoard().squareAt(6, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(6, 1)))));
	Board::getBoard().squareAt(7, 1).setOccupier(*(new Pawn(*_player1, "white", (Board::getBoard().squareAt(7, 1)))));
    
    // Round two!
    Board::getBoard().squareAt(0, 7).setOccupier(*(new Rook(*_player2, "black", (Board::getBoard().squareAt(0, 7)))));
	Board::getBoard().squareAt(1, 7).setOccupier(*(new Knight(*_player2, "black", (Board::getBoard().squareAt(1, 7)))));
	Board::getBoard().squareAt(2, 7).setOccupier(*(new Bishop(*_player2, "black", (Board::getBoard().squareAt(2, 7)))));
	Board::getBoard().squareAt(3, 7).setOccupier(*(new Queen(*_player2, "black", (Board::getBoard().squareAt(3, 7)))));
	Board::getBoard().squareAt(4, 7).setOccupier(*blackKing);
	Board::getBoard().squareAt(5, 7).setOccupier(*(new Bishop(*_player2, "black", (Board::getBoard().squareAt(5, 7)))));
	Board::getBoard().squareAt(6, 7).setOccupier(*(new Knight(*_player2, "black", (Board::getBoard().squareAt(6, 7)))));
	Board::getBoard().squareAt(7, 7).setOccupier(*(new Rook(*_player2, "black", (Board::getBoard().squareAt(7, 7)))));
	Board::getBoard().squareAt(0, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(0, 6)))));
	Board::getBoard().squareAt(1, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(1, 6)))));
	Board::getBoard().squareAt(2, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(2, 6)))));
	Board::getBoard().squareAt(3, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(3, 6)))));
	Board::getBoard().squareAt(4, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(4, 6)))));
	Board::getBoard().squareAt(5, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(5, 6)))));
	Board::getBoard().squareAt(6, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(6, 6)))));
	Board::getBoard().squareAt(7, 6).setOccupier(*(new Pawn(*_player2, "black", (Board::getBoard().squareAt(7, 6)))));
    
    
    // Add the pieces on rows 0 and 1 to the white players collection, and the
    // pieces on rows 6 and 7 to the black players pieces collection.
    for (int i = 0; i < 8; i++)
    {
        _player1->myPieces().insert(&Board::getBoard().squareAt(i, 0).occupiedBy());
        _player1->myPieces().insert(&Board::getBoard().squareAt(i, 1).occupiedBy());
        _player2->myPieces().insert(&Board::getBoard().squareAt(i, 6).occupiedBy());
        _player2->myPieces().insert(&Board::getBoard().squareAt(i, 7).occupiedBy());
    }
}

// Returns player one if argument is null
Player& Game::opponentOf(Player& player)
{
	Player* result = _player1;
    
    // Make sure to return the opposite of the player who was passed in.
    if (&player == _player1)
    {
        result = _player2;
    }
    
    return *result;
}
