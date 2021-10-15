#include "bishop.h"
#include "board.h"
#include "square.h"



Bishop::Bishop(Player& owner, std::string color, Square& location)
: Piece(owner, color, location)
{
}

Bishop::~Bishop()
{
}

void Bishop::display(std::ostream& out) const
{
    char color = (this->color() == "black") ? 'b' : 'w';
	out << "B_" << color << " ";
}

bool Bishop::canMoveTo(Square& location) const
{
    bool result = true;
    
    if (!(Board::getBoard().isClearDiagonal(this->location(), location)))
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

int Bishop::value() const
{
	return 3;
}
