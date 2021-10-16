#ifndef KNIGHT_H
#define KNIGHT_H
#include "piece.h"

/**
 * A knight represents a piece on the chess board
 */
class Knight : public Piece
{
  // Operations
public:
  /**
   * Creates a new Knight
   * @param owner The piece's owner
   * @param color The piece's color
   * @param location The piece's location
   */
  Knight(Player& owner, std::string color, Square const& location);

  /**
   * The destructor for the knight
   */
  ~Knight() override;

  /**
   * Prints the piece to an output stream
   * @param out the output stream to print to.
   */
  void display(std::ostream& out) const override;

  /**
   * Checks if the knight can legally move to the specified location
   * A knight can legally move in an L shape.
   * @param location The location to move to
   * @return True if the knight can move to that square
   */
  bool can_move_to(Square const& location) const override;

  /**
   * @return The value of the knight
   */
  int value() const override;
};
#endif
