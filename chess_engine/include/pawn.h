#ifndef PAWN_H
#define PAWN_H

#include "restricted_piece.h"

/**
 * A pawn is one of the menial laborors in the game of Chess.
 */
class Pawn : public RestrictedPiece
{

public:
  /**
   * Creates a new Pawn
   * @param owner The piece's owner
   * @param color The piece's color
   * @param location The piece's location
   */
  Pawn(Player& owner, std::string color, Square const& location);

  /**
   * destructor
   */
  ~Pawn();

  /**
   * A pawn can be promoted to a queen if it advances to the eighth row
   * on the board. If this occurs, the pawn will store a queen as a proxy,
   * borrowing methods like move and display from the proxy queen.
   * @param proxy
   */
  void set_proxy(Piece& proxy);

  /**
   * Prints the piece to an output stream
   * @param out The output stream to print to
   */
  virtual void display(std::ostream& out) const;

  /**
   * Checks to see if the pawn can legally move to a square on the board.
   * A pawn can move forward two spaces on its first move, and one space
   * forward thereafter. They capture diagonally, and may not move forward
   * into an occupied square.
   *
   * If the pawn has reached the eighth row, it is converted to a Queen and
   * assumes the same moving capabilities.
   * @param location The square to move to
   * @return True if it can move to that square
   */
  virtual bool can_move_to(Square const& location) const;

  /**
   * Moves the pawn to the specified square if the move is legal
   * @return True if the move succeeded
   */
  virtual bool move_to(Player& by_player, Square const& to);

  /**
   * @return The value of the piece
   */
  virtual int value() const;

private:
  Piece* _proxy;
};
#endif
