#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"

class Square;

/*
 * A Bishop piece for a chess game.
 */
class Bishop : public Piece
{

public:
  /**
   * Creates a new Bishop
   * @param owner The piece's owner
   * @param color The piece's color
   * @param location The piece's location
   */
  Bishop(Player& owner, std::string color, Square const& location);

  /**
   * destructor for the bishop
   */
  ~Bishop() override;

  /**
   * Prints a representation of the Bishop to an output stream
   * @param out The output stream to print to.
   */
  void display(std::ostream& out) const override;

  /**
   * Checks if the piece can legally move to a space.
   * A bishop can move to a space if it is diagonal from the Bishop's
   * current location and there are no pieces between the Bishop
   * and the destination.
   * @param location The square to move to
   * @return True if the piece captured an opposing piece
   */
  bool can_move_to(Square const& location) const override;

  /**
   * @return The value of the piece.
   */
  int value() const override;
};
#endif
