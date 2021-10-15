#ifndef RESTRICTEDPIECE_H
#define RESTRICTEDPIECE_H

#include "Piece.h"


/**
 * A RestrictedPiece can move specially on it's first move, but after that it 
 * must move normally.
 */
class RestrictedPiece : public Piece {
    public:
        /**
         * Creates a new restricted piece
         * @param owner The piece's owner
         * @param color The pieces color
         * @param location The piece's location
         */
        RestrictedPiece(Player& owner, std::string color, Square& location);
        
        /**
         * Destructor for the restricted piece.
         */
        ~RestrictedPiece();
        
        /**
         * @return The piece's location
         */
        bool hasMoved() const;
        
        /**
        * Moves the piece to the specified square
        * @param byPlayer The player who moved the piece
        * @param to The destination square
        * @return True if the piece captured another piece
        */
        virtual bool moveTo (Player& byPlayer, Square& to);
    private:
        bool _moved;
        
};
#endif
