#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"



class Square;

/// class Queen - 
class Queen : public Piece {

public:
    /**
     * Constructs a new Queen
     * @param owner The piece's owner
     * @param color The piece's color
     * @param location The piece's initial location
     */
    Queen(Player& owner, std::string color, Square& location);
    
    /**
     * The destructor for the piece
     */
    ~Queen();
    
    /**
     * Prints the piece
     * @param out The std::ostream to print to
     */
    virtual void display(std::ostream& out) const;
    
    /**
     * Checks if the queen can legally move to a square.
     * A queen can move vertically, horizontally, and diagonally.
     * @param location The square to move to.
     * @return True if the queen can move there
     */
    virtual bool canMoveTo(Square& location) const;
    
    /**
     * @return The value of the piece
     */
    virtual int value() const;
};
#endif
