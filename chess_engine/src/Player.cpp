#include "Player.h"
#include <string>
#include <iostream>

class Piece;

using namespace std;

extern int startX;
extern int startY;
extern int endX;
extern int endY;
extern volatile bool moved;

Player::Player(string name, King& myKing, set<Piece*>& myPieces) : 
    _name(name), _pieces(myPieces), _captured(*(new set<Piece*>())),  _king(&myKing)
{
}

Player::~Player()
{
    delete &_captured;
}

bool Player::makeMove()
{
    bool validMove = false;
    Piece* occupier = NULL;
    bool stillPlaying = true;
    pair<Square*, Square*>* move = NULL;
    
    // Keep trying to make a move until a valid one is requested, or until 
    // the player resigns.
    while (!validMove && stillPlaying)
    {
        move = promptMove(cin, cout);
        
        // promptMove will return null if the player resigns
        if (move == NULL)
        {
            stillPlaying = false;
        }
        else if (Board::getBoard().squareAt(move->first->getX(), move->first->getY()).occupied())
        {
            occupier = &Board::getBoard().squareAt(move->first->getX(), move->first->getY()).occupiedBy();

            if (this->myPieces().find(occupier) == myPieces().end())
            {
                cout << "Please move one of your own pieces." << endl;
            }
            else
            {
                validMove = occupier->moveTo(*this, *(move->second));
                if (!validMove)
                {
                    cout << "please enter a valid move for the piece, and ";
                    cout << "ensure that it does not leave the King in check." << endl;
                }
                else
                {
                    // This move was valid. Signal it to the GUI
                    startX = move->first->getX();
                    startY = move->first->getY();
                    endX = move->second->getX();
                    endY = move->second->getY();
                    moved = true;

                    while (moved)
                    {
                        // Wait for the graphics engine to process the move.
                    }
                }
            }
        }
        else
        {
            cout << "please choose a square with a piece on it." << endl;
        }
        
        // Clean up the move
        if (move != NULL)
        {
            delete move->first;
            delete move->second;
            delete move;
        }
    }
    
    return stillPlaying;
}

// Returns a null square if the player resigns.
pair<Square*, Square*>* Player::promptMove(istream& in, ostream& out)
{
    string line = "";
    out << getName() + ", please enter the beginning and ending squares of the ";
    out << "move (ex: A2 A4): ";
    //char fromCol = 0;
    //int toCol = 0;
    pair<Square*, Square*>* result = NULL;
    
    // Get move from the user and ensure that it is of the correct form
    getline(in, line);
    while (!isValid(line))
    {
        out << "Please make sure the move is of the form \"A1 A2\" and stays" <<
                " within the bounds of the 8x8 board, or is \"quit\": ";
        getline(in, line);
    }
    
    if (line != "quit")
    {
        // convert lower case letters to uppercase
        // Only check for > 'a' because isValid guarantees that the letter is below 'h'
        if (line[0] >= 'a')
            line[0] = line[0] - 32;

        if (line[3] >= 'a')
            line[3] = line[3] - 32;

        // Create a pair of squares to return, representing the beginning and 
        // ending squares of the desired move. Subtract 48 and 65 from the lines 
        // since they are read as ascii values from the console
        // (so '0' is 48 and 'A' is 65), but we want to store them as integers, 
        // so we can do array access. This must be deleted by the caller.
        result = new pair<Square*, Square*>(new Square(line[0] - 'A', line[1] - '0' - 1),
                new Square(line[3] - 'A', line[4] - '0' - 1));
    }
    
    // Null if the player resigned
    return result;
}

bool Player::isValid(const string& line)
{
    bool result = true;
    
    // If any character does not  fall in the allowed range,
    // the string is invalid. (valid: "A2 A3")
    // Also, "quit" is a valid input.
    
    
    // Make sure the string is the correct length. This also uses short 
    // circuiting to protect the string from bounds errors.
    if (line != "quit" && (line.length() != 5 ||
            
            // 'A' <= line[0] <= 'H'
            !((line[0] >= 'A' && line[0] <= 'H') || (line[0] >= 'a' && line[0] <= 'h')) ||
            
            // '1' <= line[1] <= '8'
            line[1] < '1' || line[1] > '8' ||
            
            // line[2] should be a space
            line[2] != ' ' ||
            
            // 'A' <= line[3] <= 'H'
            !((line[3] >= 'A' && line[3] <= 'H') || (line[3] >= 'a' && line[3] <= 'h')) ||
            
            // '1' <= line[4] <= '8'
            line[4] < '1' || line[4] > '8'
            ))
    {
        result = false;
    }
    
    return result;
}

string& Player::getName()
{
	return _name;
}

int Player::score()
{
	return 0;
}

void Player::capture(Piece& aPiece)
{
	_captured.insert(&aPiece);
}

void Player::uncapture(Piece& aPiece)
{
    _captured.erase(&aPiece);
}

set<Piece*>& Player::myPieces()
{
	return this->_pieces;
}

King& Player::myKing()
{
	return *_king;
}
