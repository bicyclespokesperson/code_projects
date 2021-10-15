#include "restricted_piece.h"

RestrictedPiece::RestrictedPiece(Player& owner, std::string color, Square& location)
: Piece(owner, color, location)
{
}

RestrictedPiece::~RestrictedPiece()
{
}

bool RestrictedPiece::hasMoved() const
{
	return _moved;
}


bool RestrictedPiece::moveTo (Player& byPlayer, Square& to)
{
    bool moveSucceeded = Piece::moveTo(byPlayer, to);
    if (moveSucceeded)
    {
        _moved = true;
    }
    return moveSucceeded;
}
