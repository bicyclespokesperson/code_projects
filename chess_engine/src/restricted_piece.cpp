#include "restricted_piece.h"

RestrictedPiece::RestrictedPiece(Player& owner, std::string color, Square& location) : Piece(owner, color, location)
{
}

RestrictedPiece::~RestrictedPiece()
{
}

bool RestrictedPiece::has_moved() const
{
  return _moved;
}

bool RestrictedPiece::move_to(Player& by_player, Square& to)
{
  bool move_succeeded = Piece::move_to(by_player, to);
  if (move_succeeded)
  {
    _moved = true;
  }
  return move_succeeded;
}
