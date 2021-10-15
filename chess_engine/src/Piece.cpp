#include "Player.h"
#include "Piece.h"
#include "King.h"
#include <string>

class Square;
using namespace std;

Piece::Piece(Player& owner, string color, Square& location) 
    : _color(color), _location(&location) , _owner(&owner)
{
}

Piece::~Piece()
{
}

bool Piece::isWhite()
{
    return _color == "white";
}

bool Piece::moveTo (Player& byPlayer, Square& to)
{
    // Checks if the move succeeded
    bool result = true;
    Square& original = location();
    Piece* captured;
    bool inCheck = false;
    bool performedCapture = false;
    
    if (canMoveTo(to))
    {


        // If occupied, capture opponent, while storing it in "captured"
        if (Board::getBoard().squareAt(to.getX(), to.getY()).occupied())
        {
            // Save the piece in case we need to uncapture it later
            captured = &Board::getBoard().squareAt(to.getX(), to.getY()).occupiedBy();
            byPlayer.capture(*captured);
            Game::opponentOf(byPlayer).myPieces().erase(captured);

            performedCapture = true;
        }


        // Move piece, (original square saved above, in case we need to go back)
        // Delete the piece from its original square
        Board::getBoard().squareAt(location().getX(), location().getY()).removeOccupier();

        // Place piece on new square
        Board::getBoard().squareAt(to.getX(), to.getY()).setOccupier(*this);

        // Update the piece's location
        this->setLocation(Board::getBoard().squareAt(to.getX(), to.getY()));

        // check for check
        inCheck = _owner->myKing().inCheck();

        // if in check, undo everything we just did.
        if (inCheck)
        {
            // This move is illegal
            result = false;

            // set moved piece location back to old location
            Board::getBoard().squareAt(original.getX(), original.getY()).setOccupier(*this);
            setLocation(Board::getBoard().squareAt(original.getX(), original.getY()));

            if (performedCapture)
            {
                // place piece back in opponent's pieces, and on board
                _owner->uncapture(*captured);
                Game::opponentOf(byPlayer).myPieces().insert(captured);
                Board::getBoard().squareAt(to.getX(), to.getY()).setOccupier(*captured);
                captured->setLocation(Board::getBoard().squareAt(to.getX(), to.getY()));
            }
            else
            {
                Board::getBoard().squareAt(to.getX(), to.getY()).removeOccupier();
            }
        }
    }
    else
    {
        result = false;
    }
    
    return result;
}

string Piece::color() const
{
	return _color;
}

void Piece::setLocation(Square& square)
{
	this->_location = &square;
}

bool Piece::isOnSquare() const
{
	return _location == NULL;
}

Square& Piece::location() const
{
	return *_location;
}

Player& Piece::owner()
{
    return *_owner;
}

void Piece::setOwner(Player& newOwner)
{
    _owner = &newOwner;
}
