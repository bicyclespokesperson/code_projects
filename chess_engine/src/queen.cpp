#include "board.h"
#include "piece.h"
#include "queen.h"
#include "square.h"


Queen::Queen(Player& owner, std::string color, Square& location)
: Piece(owner, color, location)
{
}

Queen::~Queen()
{
}

bool Queen::can_move_to(Square& location) const
{
	bool result = true;
        
    // If the square is not horizontally, vertically, or diagonally away from the 
    // Queen's current location, the move is invalid.
    if (!(Board::get_board().is_clear_diagonal(this->location(), location) ||
            Board::get_board().is_clear_horizontal(this->location(), location) ||
            Board::get_board().is_clear_vertical(this->location(), location)))
    {
        result = false;
    }
    
    // If the target location is occupied by a friend, the move is invalid
    if (Board::get_board().square_at(location.getX(), location.getY()).occupied()
            && Board::get_board().square_at(location.getX(), location.getY()).occupied_by().color() == color())
    {
        result = false;
    }
    
    return result;
}

int Queen::value() const
{
	return 9;
}

void Queen::display(std::ostream& out) const
{
    char color = (this->color() == "black") ? 'b' : 'w';
	out << "Q_" << color << " ";
}
