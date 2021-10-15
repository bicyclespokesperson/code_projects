#ifndef PLAYER_H
#define PLAYER_H

class King;
class Piece;
class Square;



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
    Player(std::string name, King& myKing, std::set<Piece*>& myPieces);
    
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
    std::string& getName();
    
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
    std::set<Piece*>& myPieces();
    
    /**
     * @return The player's king
     */
    King& myKing();
    
private:
    std::string _name;
    std::set<Piece*> _pieces;
    std::set<Piece*> _captured;
    King* _king;
    
    /**
     * Prompts the user for a move and returns the beginning and ending squares
     * @param in The input stream to read the prompt from
     * @param out The output stream to print prompts to.
     * @return A std::pair of squares representing the beginning and ending of the
     * std::string. Note: The std::pair and both squares must be deleted. Returns NULL
     * if the player resigns.
     */
    std::pair<Square*, Square*>* promptMove(std::istream& in, std::ostream& out);
    
    /**
     * Checks if the input std::string is contains the correct form of the 
     * move input. 
     * @param std::string The std::string of which to check the validity
     * @return true if the std::string is of the form "A2 D2"
     */
    bool isValid(const std::string& line);
    
};
#endif
