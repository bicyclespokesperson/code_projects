#include "Board.h"
#include "Game.h"
#include "King.h"
#include "Player.h"
#include "Square.h"
#include "RestrictedPiece.h"
using namespace std;

King::King(Player& owner, string color, Square& location)
: RestrictedPiece(owner, color, location)
{
    
}

King::~King()
{
    
}

bool King::inCheck()
{
    bool inCheck = false;
    
    for (set<Piece*>::iterator it = Game::opponentOf(owner()).myPieces().begin();
            it != Game::opponentOf(owner()).myPieces().end() && !inCheck; it++)
    {
        if ((*it)->canMoveTo(owner().myKing().location()))
        {
            (*it)->display(cout);
            inCheck = true;
        }
    }
    return inCheck;
}

bool King::canMoveTo (Square& location) const
{
    bool result = true;
    
    if (Board::getBoard().distanceBetween(this->location(), location) != 1)
    {
        result = false;
    }
    
    // If the target location is occupied by a friend, the move is invalid
    if (Board::getBoard().squareAt(location.getX(), location.getY()).occupied()
            && Board::getBoard().squareAt(location.getX(), location.getY()).occupiedBy().color() == color())
    {
        result = false;
    }
    
    
	return result;
}

int King::value () const
{
	return 10;
}

void King::display(ostream& out) const
{
    char color = (this->color() == "black") ? 'b' : 'w';
	out << "K_" << color << " ";
}
