#include "knight.h"
#include "square.h"



Knight::Knight(Player& owner, std::string color, Square& location)
: Piece(owner, color, location)
{
}

Knight::~Knight()
{
}

bool Knight::can_move_to (Square& location) const
{
    bool result = false;
    
    // Make sure the move is either two vertical and one horizontal
    if (abs(this->location().getY() - location.getY()) == 2 && abs(this->location().getX() - location.getX()) == 1)
    {
        result = true;
    }
    
    // Or two horizontal and one vertical
    else if (abs(this->location().getX() - location.getX()) == 2 && abs(this->location().getY() - location.getY()) == 1)
    {
        result = true;
    }
    
	return result;
}

int Knight::value () const
{
	return 3;
}

void Knight::display(std::ostream& out) const
{
    char color = (this->color() == "black") ? 'b' : 'w';
	out << "N_" << color << " ";
}
