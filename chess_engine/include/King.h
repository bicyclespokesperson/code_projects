#ifndef KING_H
#define KING_H

#include "RestrictedPiece.h"


class Piece;
class Player;
class Square;

/**
 * The king is the pivotal piece in a chess game.
 */
class King : public RestrictedPiece {

public:
    
    /**
     * Creates a new King
     * @param owner The piece's owner
     * @param color The piece's color
     * @param location The piece's location
     */
    King(Player& owner, std::string color, Square& location);
    
    /**
     * The destructor for the King class
     */
    ~King();
    
    /**
     * Print the king to the specified output stream
     * @param out The outstream to print to
     */
    virtual void display(std::ostream& out) const;
    
    /**
     * Returns True if the king can move to a specific square.
     * A king can move to any adjacent square.
     * @param location The location to move to.
     * @return True if it can move to that square
     */
    virtual bool canMoveTo(Square& location) const;
    
    /**
     * @return the value of the king. 
     */
    virtual int value() const;
    
    /**
     * @return True if an opposing piece can attack the king
     */
    bool inCheck();
};
#endif
