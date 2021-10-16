#ifndef PLAYER_H
#define PLAYER_H

class King;
class Piece;
class Square;

/**
 * A player class represents one of the players in a chess game. It
 * can keep track of its pieces and make a move.
 */
class Player
{

public:
  /**
   * Constructs a new player
   * @param name The player's name
   * @param my_king The player's king
   * @param my_pieces The player's pieces
   */
  Player(std::string name, King& my_king, std::set<Piece*> my_pieces);

  /**
   * The destructor for the player class
   */
  ~Player();

  /**
   * Makes a move
   * @return True if the move is legal
   */
  bool make_move();

  /**
   * @return The name of the player
   */
  std::string& get_name();

  /**
   * @return The player's score (the sum of the values of the captured pieces)
   */
  int score();

  /**
   * Adds a piece to the player's captured collection
   * @param a_piece The piece to capture
   */
  void capture(Piece& a_piece);

  /**
   * Removes a piece from the player's captured collection.
   * @param a_piece the piece to remove
   */
  void uncapture(Piece& a_piece);

  /**
   * @return The player's pieces
   */
  std::set<Piece*>& my_pieces();

  /**
   * @return The player's king
   */
  King& my_king() const;

private:
  /**
   * Prompts the user for a move and returns the beginning and ending squares
   * @param in The input stream to read the prompt from
   * @param out The output stream to print prompts to.
   * @return A std::pair of squares representing the beginning and ending of the
   * std::string. Note: The std::pair and both squares must be deleted. Returns nullptr
   * if the player resigns.
   */
  std::optional<std::pair<Square, Square>> prompt_move_(std::istream& in, std::ostream& out);

  /**
   * Checks if the input std::string is contains the correct form of the
   * move input.
   * @param std::string The std::string of which to check the validity
   * @return true if the std::string is of the form "A2 D2"
   */
  bool is_valid_(const std::string& line);

  std::string _name;
  std::set<Piece*> _pieces{};
  std::set<Piece*> _captured{};
  King* _king{nullptr};
};
#endif
