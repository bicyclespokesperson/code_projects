#ifndef SQUARE_H
#define SQUARE_H

class Board;
class Piece;

using namespace std;

/**
 * Represents a square on a gameboard.
 * A Square can hold a piece, and knows its location in xy coordinates on a 
 * board. It also knows what board is holding it.
 */
class Square {
private:
    
  int _x;  
  int _y;
  Piece* _occupier;

public:
    /**
     * Creates a new square
     * @param x The column of the square (a letter)
     * @param y The row of the square (a number)
     * @param occupier The piece occupying the square
     */
    Square(int x, int y, Piece* occupier = NULL);
    
    /**
     * destructor
     */
    ~Square();
    
    /**
     * @return The column of the square (a letter)
     */
    int getX() const;
    
    /**
     * @return The row of the square (a number)
     */
    int getY() const;
    
    /**
     * @return True is the Square is occupied
     */
    bool occupied() const;
    
    /**
     * @return The piece occupying the square (NULL if empty)
     */
    Piece& occupiedBy() const;
    
    /**
     * @param piece The new occupier of the square
     */
    void setOccupier(Piece& piece);
    
    /**
     * Remove the piece from this Square
     * This is needed because null references are illegal, so setOccupier 
     * cannot be used to set the occupant to null.
     */
    void removeOccupier();
    
    /**
     * Prints the square
     * @param out The ostream to print to
     */
    void display(ostream& out) const;
    
    friend ostream& operator<< (ostream & output, const Square s);
};
#endif
