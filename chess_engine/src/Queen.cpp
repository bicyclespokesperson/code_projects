#include "Board.h"
#include "Piece.h"
#include "Queen.h"
#include "Square.h"
using namespace std;

Queen::Queen(Player& owner, string color, Square& location)
: Piece(owner, color, location)
{
}

Queen::~Queen()
{
}

bool Queen::canMoveTo(Square& location) const
{
	bool result = true;
        
    // If the square is not horizontally, vertically, or diagonally away from the 
    // Queen's current location, the move is invalid.
    if (!(Board::getBoard().isClearDiagonal(this->location(), location) ||
            Board::getBoard().isClearHorizontal(this->location(), location) ||
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

int Queen::value() const
{
	return 9;
}

void Queen::display(ostream& out) const
{
    char color = (this->color() == "black") ? 'b' : 'w';
	out << "Q_" << color << " ";
}
