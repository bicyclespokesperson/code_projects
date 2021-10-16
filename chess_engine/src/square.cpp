#include "board.h"
#include "king.h"
#include "piece.h"
#include "square.h"



Square::Square(int x, int y, Piece* occupier) : _x(x), _y(y), _occupier(occupier)
{
}

Square::~Square()
{   
}

int Square::getX() const
{
	return _x;
}

int Square::getY() const
{
	return _y;
}

bool Square::occupied() const
{
	return (_occupier != NULL);
}

Piece& Square::occupied_by() const
{
	return *_occupier;
}

void Square::set_occupier(Piece& piece)
{
	this->_occupier = &piece;
}

void Square::remove_occupier()
{
    this->_occupier = NULL;
}

void Square::display(std::ostream& out) const
{
    if (this->occupied())
    {
        _occupier->display(out);
    }
    else
    {
        out << "___ ";
    }
}

// Print the Square's location to the console.
// Useful for debugging purposes.
std::ostream& operator<< (std::ostream& out, const Square s)
{
    // Add 65 to _x because it represents a letter but we store it as a 
    // number so we can do array access with it. ('A' == 65)
    out << "Column: " << (char) (s._x + 65) << ", Row: " << s._y << std::endl;
    return out;
}
