#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

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
  Player(std::string name);

  /**
   * The destructor for the player class
   */
  ~Player();

  /**
   * @return The name of the player
   */
  std::string const& get_name() const;

  /**
   * Prompts the user for a move and returns the beginning and ending squares
   * @param in The input stream to read the prompt from
   * @param out The output stream to print prompts to.
   * @return A std::pair of squares representing the beginning and ending of the
   * std::string. Note: The std::pair and both squares must be deleted. Returns nullptr
   * if the player resigns.
   */
  std::optional<std::pair<Coordinates, Coordinates>> prompt_move(std::istream& in, std::ostream& out) const;

private:
  /**
   * Checks if the input std::string is contains the correct form of the
   * move input.
   * @param std::string The std::string of which to check the validity
   * @return true if the std::string is of the form "A2 D2"
   */
  static bool is_valid_(const std::string& line);

  std::string m_name;
};
#endif
