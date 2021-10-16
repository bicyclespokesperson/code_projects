#ifndef QUEEN_H
#define QUEEN_H

#include "piece.h"

class Square;

/// class Queen -
class Queen : public Piece
{

public:
  /**
   * Constructs a new Queen
   * @param owner The piece's owner
   * @param color The piece's color
   * @param location The piece's initial location
   */
  Queen(Player& owner, std::string color, Square const& location);

  /**
   * The destructor for the piece
   */
  ~Queen() override;

  /**
   * Prints the piece
   * @param out The std::ostream to print to
   */
  void display(std::ostream& out) const override;

  /**
   * Checks if the queen can legally move to a square.
   * A queen can move vertically, horizontally, and diagonally.
   * @param location The square to move to.
   * @return True if the queen can move there
   */
  bool can_move_to(Square const& location) const override;

  /**
   * @return The value of the piece
   */
  int value() const override;
};
#endif
