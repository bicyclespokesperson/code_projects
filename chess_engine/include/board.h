#ifndef BOARD_H
#define BOARD_H

class Square;
class Game;



/**
 * A board class holds Squares, which hold pieces.
 */
class Board {
public:
    /**
     * Returns the square at the xy location
     * @param x The column of the desired square (A letter)
     * @param y The row coordinate of the desired square
     * @return The square at the desired location
     */
    Square& square_at(int x, int y);
    
    /**
     * Checks if every square in between squares from and to are empty,
     * and if the squares are in a vertical line.
     * @param from The initial square
     * @param to The final square
     * @return True if there is a clear vertical line between the squares
     */
    bool is_clear_vertical(Square& from, Square& to);
    
    /**
     * Checks if every square in between squares from and to are empty,
     * and if the squares are in a horizontal line.
     * @param from The initial square
     * @param to The final square
     * @return True if there is a clear horizontal line between the squares
     */
    bool is_clear_horizontal(Square& from, Square& to);
    
    /**
     * Checks if every square in between squares from and to are empty,
     * and if the squares are in a diagonal line.
     * @param from The initial square
     * @param to The final square
     * @return True if there is a clear diagonal line between the squares
     */
    bool is_clear_diagonal(Square& from, Square& to);
    
    /**
     * Returns the distance between two squares on the board.
     * For example, A2 and A5 are three squares apart
     * Returns -1 if the squares are not in a line on the board.
     * @param from The first Square
     * @param to The Second Square
     * @return The distance between the two squares.
     */
    int distance_between(Square& from, Square& to);
    
    /**
     * Prints the square to the specified std::ostream
     * @param out_stream The stream to print to
     */
    void display(std::ostream& out_stream);
    
    /**
     * Initializes the 2D vector of squares properly.
     * This should be called before using the board.
     */
    void setup();
    
    /**
     * The effective constructor for the board
     * @return a pointer to the single instance of the board
     */
    static Board& get_board();
    
    /**
     * Destructor
     */
    ~Board();

private:
    int _DIMENSION;
    std::vector< Square* >* _squares;
    static Board* _theBoard;
    Board();
};
#endif
