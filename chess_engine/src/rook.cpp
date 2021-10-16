#include "rook.h"
#include "board.h"
#include "square.h"

class Square;

Rook::Rook(Player& owner, Color color, Square const& location) : RestrictedPiece(owner, color, location)
{
}

Rook::~Rook() = default;

bool Rook::can_move_to(Square const& target) const
{
  if (!(Board::get_board().is_clear_horizontal(location(), target) ||
        Board::get_board().is_clear_vertical(location(), target)))
  {
    return false;
  }

  // If the target location is occupied by a friend, the move is invalid
  if (auto const& square = Board::get_board().square_at(target.get_x(), target.get_y());
      square.occupied() && square.occupied_by().color() == color())
  {
    return false;
  }

  return true;
}

int Rook::value() const
{
  return 5;
}

void Rook::display(std::ostream& out) const
{
  char color = (is_white()) ? 'w' : 'b';
  out << "R_" << color << " ";
}
