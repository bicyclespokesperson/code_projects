#ifndef SQUARE_H
#define SQUARE_H

class Board;
class Piece;



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
    Piece& occupied_by() const;
    
    /**
     * @param piece The new occupier of the square
     */
    void set_occupier(Piece& piece);
    
    /**
     * Remove the piece from this Square
     * This is needed because null references are illegal, so set_occupier 
     * cannot be used to std::set the occupant to null.
     */
    void remove_occupier();
    
    /**
     * Prints the square
     * @param out The std::ostream to print to
     */
    void display(std::ostream& out) const;
    
    friend std::ostream& operator<< (std::ostream & output, const Square s);
};
#endif
