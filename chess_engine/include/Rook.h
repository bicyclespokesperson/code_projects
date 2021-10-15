#ifndef ROOK_H
#define ROOK_H
#include "RestrictedPiece.h"
using namespace std;

/**
 * A rook is one of the pieces in a Chess game
 */
class Rook : public RestrictedPiece {

public:
    
    /**
     * Creates a new rook
     * @param owner The piece's owner
     * @param color The pieces color
     * @param location The piece's location
     */
    Rook(Player& owner, string color, Square& location);
    
    /**
     * The destructor for the rook
     */
    ~Rook();
    
    /**
     * Prints the piece
     * @param out The outstream to print to
     */
    virtual void display(ostream& out) const;
    
    /**
     * Checks if the rook can move to a location. A rook can move vertically
     * and horizontally.
     * @param location The location to move to
     * @return True if the piece can legally move to the desired location
     */
    virtual bool canMoveTo(Square& location) const;
    
    /**
     * @return the value of the piece
     */
    virtual int value() const;
};
#endif
