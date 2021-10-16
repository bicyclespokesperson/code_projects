#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"




class Square;

/*
 * A Bishop piece for a chess game.
 */
class Bishop : public Piece {

public:
    
    /**
     * Creates a new Bishop
     * @param owner The piece's owner
     * @param color The piece's color
     * @param location The piece's location
     */
    Bishop(Player& owner, std::string color, Square& location);
    
    /**
     * destructor for the bishop
     */
    ~Bishop();
    
    /**
     * Prints a representation of the Bishop to an output stream
     * @param out The output stream to print to.
     */
    virtual void display(std::ostream& out) const;
    
    /**
     * Checks if the piece can legally move to a space.
     * A bishop can move to a space if it is diagonal from the Bishop's
     * current location and there are no pieces between the Bishop
     * and the destination.
     * @param location The square to move to
     * @return True if the piece captured an opposing piece
     */
    virtual bool can_move_to(Square& location) const;
    
    /**
     * @return The value of the piece.
     */
    virtual int value() const;
};
#endif
