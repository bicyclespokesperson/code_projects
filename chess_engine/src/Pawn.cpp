//Grading Template

/* ID         :  1697518
   Assignment :  Chess
   Class      :  CSIS 370 Object-Oriented Programming
   Date       : 4/24/2013

Programming Assessment Standards

PROGRAM CORRECTNESS
   a) the program is complete & functions correctly       (40)________
PROGRAM DESIGN
   b) object-oriented class design & implementation       (10)________
   c) appropriate use of language and features            (10)________
   d) subjective elegance                                 (20)________
PROGRAM READABILITY AND DOCUMENTATION
   e) Header comments are complete & clear                 (5)________
   f) Internal comments are used when appropriate         (15)________

TOTAL                                                    (100)________

******************************************************************/

#include "Pawn.h"
#include "RestrictedPiece.h"
#include "Piece.h"
#include "Queen.h"

using namespace std;

Pawn::Pawn(Player& owner, string color, Square& location)
: RestrictedPiece(owner, color, location)
{
    
}

Pawn::~Pawn()
{
    delete _proxy;
}

void Pawn::setProxy(Piece& proxy)
{
    _proxy = &proxy;
}

int Pawn::value() const
{
	return 1;
}

bool Pawn::canMoveTo (Square& location) const
{
    bool result = false;
    int maxDistance = 1;
    
    if (_proxy != NULL)
    {
        result = _proxy->canMoveTo(location);
    }
    else
    {
        // A pawn can move two spaces on its first move
        if (!hasMoved())
        {
            maxDistance = 2;
        }

        // Make sure the distance of the move is not greater than 1, or 2 if the 
        // piece has not yet moved.
        if (Board::getBoard()
                .distanceBetween(this->location(), location)<= maxDistance)
        {
            // Make sure the pawn is moving forward
            // "this" is needed to distinguish between the parameter location and
            // the method location()
            if ((this->color() == "white" && location.getY() > this->location().getY()) ||
                    (this->color() == "black" && location.getY() < this->location().getY()))
            {

                // If the space ahead of the pawn is clear and vertical
                // This also prevents moves of zero spaces, since it cannot move to
                // a space occupied by itself.
                if (Board::getBoard().isClearVertical(this->location(), location) &&
                        !(Board::getBoard().squareAt(location.getX(), location.getY()).occupied()))
                {
                    result = true;
                }
                // If the square is diagonally forward and occupied by an opponent
                else if (Board::getBoard().isClearDiagonal(this->location(), location) && 
                        Board::getBoard().squareAt(location.getX(), location.getY()).occupied() &&
                        Board::getBoard().squareAt(location.getX(), location.getY())
                        .occupiedBy().color() != this->color())
                {
                    result = true;
                }        
            }
        }
    }
    
	return result;
}

bool Pawn::moveTo (Player& byPlayer, Square& to)
{
    bool moveSucceeded = Piece::moveTo(byPlayer, to);
    
    // Promote pawn if it is on the eighth row
    if (moveSucceeded && (to.getY() == 0 || to.getY() == 7) && _proxy == NULL)
    {
        setProxy(*(new Queen(owner(), ((isWhite()) ? "white" : "black"), location())));
    }
    
    if (_proxy != NULL)
    {
        _proxy->setLocation(to);
    }
    
    return moveSucceeded;
}

void Pawn::display(ostream& out) const
{
    if (_proxy != NULL)
    {
        _proxy->display(out);
    }
    else
    {
        char color = (this->color() == "black") ? 'b' : 'w';
        out << "P_" << color << " ";
    }
}
