#ifndef PIECE_H
#define PIECE_H
#include <string>
#include "Square.h"
#include <iostream>

class Square;
class Player;

using namespace std;

/**
 * The superclass for all the Chess pieces
 */
class Piece {
  
public:
    /**
     * Constructs a new piece.
     * @param owner The owner of the piece
     * @param _color The color of the piece
     * @param location The location of the piece
     */
    Piece(Player& owner, string color, Square& location);
    
    /**
     * The destructor of the piece.
     */
    virtual ~Piece();
    
    /**
     * @return True if the piece's color is white.
     */
    bool isWhite();
    
    /**
     * Moves the piece to the specified square
     * @param byPlayer The player who moved the piece
     * @param to The destination square
     * @return True if the piece captured another piece
     */
    virtual bool moveTo (Player& byPlayer, Square& to);
    
    /**
     * @return the value of the piece
     */
    virtual int value() const = 0;
    
    /**
     * @return The color of the piece
     */
    string color() const;
    
    /**
     * Prints the piece
     * @param outStream The outstream to print to
     */
    virtual void display (ostream& outStream) const = 0;
    
    /**
     * Checks if a piece can legally move to a square
     * @param location The square to move to
     * @return True if the move is legal
     */
    virtual bool canMoveTo(Square& location) const = 0;
    
    /**
     * Changes the piece's location
     * @param square The new location
     */
    virtual void setLocation(Square& square);
    
    /**
     * @return True if the piece is on a square
     */
    bool isOnSquare() const;
    
    /**
     * @return The location of the piece
     */
    Square& location() const;
    
    /**
     * @return the piece's owner
     */
    Player& owner();
    
    /**
     * Sets the piece's owner to the specified piece
     * @param newOwner The piece's new owner
     */
    void setOwner(Player& newOwner);
    
private:
  string _color;
  Square* _location;
  Player* _owner;

};
#endif
