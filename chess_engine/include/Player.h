#ifndef PLAYER_H
#define PLAYER_H

#include <set>
#include <string>
#include "Square.h"

class King;
class Piece;
class Square;

using namespace std;

/**
 * A player class represents one of the players in a chess game. It 
 * can keep track of its pieces and make a move.
 */
class Player {

public:
    /**
     * Constructs a new player
     * @param name The player's name
     * @param myKing The player's king
     * @param myPieces The player's pieces
     */
    Player(string name, King& myKing, set<Piece*>& myPieces);
    
    /**
     * The destructor for the player class
     */
    ~Player();
    
    /**
     * Makes a move
     * @return True if the move is legal
     */
    bool makeMove();
    
    /**
     * @return The name of the player
     */
    string& getName();
    
    /**
     * @return The player's score (the sum of the values of the captured pieces)
     */
    int score();
    
    /**
     * Adds a piece to the player's captured collection
     * @param aPiece The piece to capture
     */
    void capture(Piece &aPiece);
    
    /**
     * Removes a piece from the player's captured collection.
     * @param aPiece the piece to remove
     */
    void uncapture(Piece& aPiece);
    
    /**
     * @return The player's pieces
     */
    set<Piece*>& myPieces();
    
    /**
     * @return The player's king
     */
    King& myKing();
    
private:
    string _name;
    set<Piece*> _pieces;
    set<Piece*> _captured;
    King* _king;
    
    /**
     * Prompts the user for a move and returns the beginning and ending squares
     * @param in The input stream to read the prompt from
     * @param out The output stream to print prompts to.
     * @return A pair of squares representing the beginning and ending of the
     * string. Note: The pair and both squares must be deleted. Returns NULL
     * if the player resigns.
     */
    pair<Square*, Square*>* promptMove(istream& in, ostream& out);
    
    /**
     * Checks if the input string is contains the correct form of the 
     * move input. 
     * @param string The string of which to check the validity
     * @return true if the string is of the form "A2 D2"
     */
    bool isValid(const string& line);
    
};
#endif
