#include "board.h"
#include "square.h"
#include "rook.h"


class Square;

Rook::Rook(Player& owner, std::string color, Square& location)
: RestrictedPiece (owner, color, location)
{
}

Rook::~Rook()
{
}

bool Rook::canMoveTo (Square& location) const
{
    bool result = true;
    
    if (!(Board::getBoard().isClearHorizontal(this->location(), location) ||
            Board::getBoard().isClearVertical(this->location(), location)))
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

int Rook::value () const
{
	return 5;
}

void Rook::display(std::ostream& out) const
{
    char color = (this->color() == "black") ? 'b' : 'w';
	out << "R_" << color << " ";
}
