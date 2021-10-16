#include "rook.h"
#include "board.h"
#include "square.h"

class Square;

Rook::Rook(Player& owner, Color color, Square const& location) : RestrictedPiece(owner, color, location)
{
}

Rook::~Rook()
{
}

bool Rook::can_move_to(Square const& location) const
{
  bool result = true;

  if (!(Board::get_board().is_clear_horizontal(this->location(), location) ||
        Board::get_board().is_clear_vertical(this->location(), location)))
  {
    result = false;
  }

  // If the target location is occupied by a friend, the move is invalid
  if (Board::get_board().square_at(location.get_x(), location.get_y()).occupied() &&
      Board::get_board().square_at(location.get_x(), location.get_y()).occupied_by().color() == color())
  {
    result = false;
  }

  return result;
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
