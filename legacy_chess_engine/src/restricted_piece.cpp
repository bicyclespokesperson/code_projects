#include "restricted_piece.h"

RestrictedPiece::RestrictedPiece(Player& owner, Color color, Square const& location)
    : Piece(owner, color, location)
{
}

RestrictedPiece::~RestrictedPiece() = default;

bool RestrictedPiece::has_moved() const
{
  return _moved;
}

bool RestrictedPiece::move_to(Player& by_player, Square const& to)
{
  bool move_succeeded = Piece::move_to(by_player, to);
  if (move_succeeded)
  {
    _moved = true;
  }
  return move_succeeded;
}
