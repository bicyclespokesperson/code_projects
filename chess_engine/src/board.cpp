#include "board.h"
#include "square.h"


Board* Board::_theBoard = NULL;

Board& Board::get_board()
{
    if (_theBoard == NULL)
    {
        _theBoard = new Board();
    }
    return *_theBoard;
}

// A Board handles its own garbage collection.
Board::Board()
{
    _DIMENSION = 8;
    this->_squares = new std::vector< Square* >(_DIMENSION * _DIMENSION, NULL);
    
    for(char i = 0; i < _DIMENSION; i++)
    {
        for (int j = 0; j < _DIMENSION; j++)
        {
            // The matrix is compressed into a one dimensional array and stored 
            // in row major form.
            _squares->at((i) * _DIMENSION + (j)) = new Square(i, j, NULL);
        }

    }
}

Board::~Board()
{
    for (std::vector<Square*>::iterator it = _squares->begin();
            it != _squares->end(); it++)
    {
        delete *it;
    }
    
    delete _squares;
}

/**
 * Calculates the distance between two squares on the board
 * @param from The first square
 * @param to The second square
 * @return The resulting distance
 */
int Board::distance_between(Square& from, Square& to)
{
    int result = -1;
    
    // If the squares are on the same vertical, return the difference
    // in their horizontals
    if (from.getX() == to.getX())
    {
        result = abs(from.getY() - to.getY());
    }
    
    // If the squares are on the same horizontal, return the difference
    // in their verticals
    else if (from.getY() == to.getY())
    {
        result = abs(from.getX() - to.getX());
    }
    
    // If the squares are on the same diagonal, their x and y differences
    // will be equal, so return their X difference
    else if (abs(from.getX() - to.getX()) == abs(from.getY() - to.getY()))
    {
        result = abs(from.getX() - to.getX());
    }
    
    return result;
}

void Board::setup()
{
    Board::_squares = new std::vector< Square* >(64, NULL);
    for(char i = 0; i < _DIMENSION; i++)
    {
        for (int j = 0; j < _DIMENSION; j++)
        {
            // The squares should be stored with their zero based xy coordinates
            // so that the array access math works out nicely.
            _squares->at((i) * _DIMENSION + (j)) = new Square(i, j, NULL);
        }

    }
}

Square& Board::square_at (int x, int y)
{
    return *(_squares->at(_DIMENSION * x + y));
}

bool Board::is_clear_vertical(Square& from, Square& to)
{
    bool result = true;
    Square* top = &to;
    Square* bottom = &from;
    // Set up the counter for the while loop.
    // Assume we are going to start from one space ahead of the current square
    // and walk straight until we hit the destination square
    
    if (from.getX() != to.getX())
    {
        result = false;
    }
    else
    {
        // Set up the squares so we always walk up.
        // So if to is above from, swap them.
        if (to.getY() < from.getY())
        {
            bottom = &to;
            top = &from;
        }
        
        // Walk along the board and if we find an occupied space, exit the loop
        // and return false.
        for (int i = bottom->getY() + 1; i < top->getY() && result; i++)
        {
            if (square_at(from.getX(), i).occupied())
            {
                result = false;
            }
        }
    }
    
	return result;
}

bool Board::is_clear_horizontal(Square& from, Square& to)
{
    bool result = true;
    Square* right = &to;
    Square* left = &from;
    // Set up the counter for the while loop.
    // Assume we are going to start from one space ahead of the current square
    // and walk straight until we hit the destination square
    
    if (from.getY() != to.getY())
    {
        result = false;
    }
    else
    {
        // Set up the squares so we always walk up.
        // So if to is above from, swap them.
        if (to.getX() < from.getX())
        {
             left = &to;
            right = &from;
        }
        
        // Walk along the board and if we find an occupied space, exit the loop
        // and return false.
        for (int i =  left->getX() + 1; i < right->getX() && result; i++)
        {
            if (square_at(i, from.getY()).occupied())
            {
                result = false;
            }
        }
    }
	return result;
}

bool Board::is_clear_diagonal(Square& from, Square& to)
{
    bool result = true;
    
    // Assume that we are walking up
	int direction = 1;
    
    // Assume we are walking right
    Square* left = &from;
    Square* right = &to;
    
    if (abs(from.getX()-to.getX()) != abs(from.getY() - to.getY()))
    {
        result = false;
    }
    else
    {

        // Verify assumption that we are walking right
        if (from.getX() > to.getX())
        {
            left = &to;
            right = &from;
        }
        
        if (left->getY() > right->getY())
        {
            direction = -1;
        }

        // Walk from "left" to "right"
        for (int i = 1; i < right->getX() - left->getX(); i++)
        {
            // Check to see if square is occupied
            if (Board::get_board().square_at(left->getX() + i, left->getY() + direction * i).occupied())
            {
                result = false;
            }
        }

    }
    return result;
}

void Board::display(std::ostream& out)
{
    out << std::endl;
    for(int i = _DIMENSION - 1; i >= 0; i--)
    {
        out << (i + 1) << "  ";
        for (char j = 0; j < _DIMENSION; j++)
        {
            square_at(j, i).display(out);
        }
        out << std::endl << std::endl;
    }
    
    out << "   ";
    for (int i = 0; i < _DIMENSION;  i++)
    {
        out << " " << (char) (i + 'A') << "  ";
        
    }
    std::cout << std::endl;
}
