#ifndef KING_H
#define KING_H

#include "restricted_piece.h"

class Piece;
class Player;
class Square;

/**
 * The king is the pivotal piece in a chess game.
 */
class King : public RestrictedPiece
{

public:
  /**
   * Creates a new King
   * @param owner The piece's owner
   * @param color The piece's color
   * @param location The piece's location
   */
  King(Player& owner, std::string color, Square const& location);

  /**
   * The destructor for the King class
   */
  ~King() override;

  /**
   * Print the king to the specified output stream
   * @param out The outstream to print to
   */
  void display(std::ostream& out) const override;

  /**
   * Returns True if the king can move to a specific square.
   * A king can move to any adjacent square.
   * @param location The location to move to.
   * @return True if it can move to that square
   */
  bool can_move_to(Square const& location) const override;

  /**
   * @return the value of the king.
   */
  int value() const override;

  /**
   * @return True if an opposing piece can attack the king
   */
  bool in_check();
};
#endif
